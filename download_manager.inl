#include <iostream>
#include <unistd.h>
#include <stdio.h>

using namespace std;

template <typename T, typename U>
download_manager<T, U>::download_manager(
	const char *remote_url, const char *local_path, size_t count):
	remote_url_(remote_url), local_path_(local_path), 
	task_count_(count), tasks_(NULL)
{
}

template <typename T, typename U>
download_manager<T, U>::~download_manager()
{
	_clean_up();
}

template <typename T, typename U>
inline void download_manager<T, U>::set_remote_url(const char *url)
{
	remote_url_ = url;
}

template <typename T, typename U>
inline void download_manager<T, U>::set_local_path(const char *path)
{
	local_path_ = path;
}

template <typename T, typename U>
inline void download_manager<T, U>::set_task_count(int count)
{
	task_count_ = count;
}

template <typename T, typename U>
void download_manager<T, U>::_clean_up()
{
	if (tasks_) {
		for (int i = 0; i < task_count_; i++) {
			if (NULL != tasks_[i].param) {
				delete static_cast<task_data *>(tasks_[i].param);
			}
		}
		delete[] tasks_;
		tasks_ = NULL;
	}
}

template <typename T, typename U>
int download_manager<T, U>::init()
{
	file_size_ = downloader_.get_file_size(remote_url_.c_str());
	if (file_size_ <= 0) {
		std::cout << "download file size not correct" << std::endl;
		return -1;
	}

	//if file already exists , add an index to the local file.
	int index = 1;
	while (0 == access(local_path_.c_str(), F_OK)) {
		char str[32];
		snprintf(str, 32, "(%d)", index++);
		int pos = local_path_.find_last_of('.');
		if (pos == string::npos) {
			local_path_.append(str);
		} else {
			local_path_.insert(pos, str);
		}
	}

	if (file_handler_.open(local_path_.c_str(), file_size_) < 0) {
		std::cout << "download_manager open file failed: " 
						<< local_path_.c_str() << std::endl;
		return -1;
	}

	if (downloader_.init() < 0) {
		std::cout << "downloader init failed" << std::endl;
		file_handler_.close();
		return -1;
	}

	if (task_manager_.init() < 0) {
		std::cout << "task manager init failed" << std::endl;
		downloader_.destroy();
		file_handler_.close();
		return -1;
	}

	tasks_ = new task_manager::task_type[task_count_];
	if (!tasks_) {
		std::cout << "task holder init failed" << std::endl;
		downloader_.destroy();
		file_handler_.close();
		task_manager_.destroy();
		return -1;
	}

	return 0;
}

template <typename T, typename U>
int download_manager<T, U>::_wrapper_task_handler(void *arg)
{
	task_data *data = static_cast<task_data *>(arg);
	download_manager<T, U> *dm = data->manager;
	return dm->download_file_block(data->offset, data->size);
}

template <typename T, typename U>
int download_manager<T, U>::download_file_block(size_t offset, size_t size)
{
	return downloader_.download(remote_url_.c_str(), offset, size, file_handler_);
}

template <typename T, typename U>
int download_manager<T, U>::start()
{
	size_t block_size = file_size_/task_count_;
	size_t last_block_size = block_size + file_size_%task_count_;

	for (int i = 0; i < task_count_; i++) {
		size_t offset = i*block_size;
		size_t size;

		if (i == (task_count_ - 1)) {
			size = last_block_size;
		} else {
			size = block_size;
		}

		task_data *tdata = new task_data();
		tdata->offset = offset;
		tdata->size = size;
		tdata->manager = this;
		tasks_[i].handler = download_manager::_wrapper_task_handler;
		tasks_[i].param = static_cast<void *>(tdata);
		task_manager_.start_task(tasks_[i]);
	}

	return 0;
}

template <typename T, typename U>
void download_manager<T, U>::wait()
{
	task_manager_.wait_all_task_done();
}

template <typename T, typename U>
int download_manager<T, U>::destroy()
{
	task_manager_.destroy();
	downloader_.destroy();
	file_handler_.close();
	_clean_up();
	return 0;
}

