/* 
 * download_manager.cpp
 * description: manage the whole download procedure 
 * Author: xxx<xxx@gmail.com>
 */ 
 
#include "downloader.h"
#include "task_manager.h"
#include "download_manager.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

typedef struct
{
	size_t offset;
	size_t size;
	download_manager *manager;
}task_data_t;

download_manager::download_manager(
	const char *remote_url, const char *local_path,
	size_t count, downloader *downloader):
	remote_url_(remote_url), local_path_(local_path),
	task_count_(count), downloader_(downloader)
{

}

download_manager::~download_manager()
{
	_clean_up();
}

void download_manager::set_remote_url(const char *url)
{
	remote_url_ = url;
}

void download_manager::set_local_path(const char *path)
{
	local_path_ = path;
}

void download_manager::set_task_count(int count)
{
	task_count_ = count;
}

void download_manager::set_downloader(downloader *downloader)
{
	downloader_ = downloader;
}

void download_manager::_clean_up()
{
	if (local_fd_ >= 0) {
		close(local_fd_);
		local_fd_ = -1;
	}

	if (task_manager_) {
		delete task_manager_;
		task_manager_ = NULL;
	}

	if (tasks_) {
		for (int i = 0; i < task_count_; i++) {
			if (NULL != tasks_[i].param) {
				delete static_cast<task_data_t *>(tasks_[i].param);
			}
		}
		delete[] tasks_;
		tasks_ = NULL;
	}

	downloader_ = NULL;
}

int download_manager::init()
{
	file_size_ = downloader_->get_file_size(remote_url_.c_str());
	if (file_size_ <= 0) {
		std::cout << "download file size not correct" << std::endl;
		return -1;
	}

	int index = 1;
	while(0 == access(local_path_.c_str(), F_OK)) {
		char str[32];
		snprintf(str, 32, "(%d)", index++);
		int pos = local_path_.find_last_of('.');
		if (pos == string::npos) {
			local_path_.append(str);
		} else {
			local_path_.insert(pos, str);
		}
	}

	local_fd_ = open(local_path_.c_str(), O_WRONLY|O_CREAT);
	if (local_fd_ < 0) {
		std::cout << "download_manager open file failed: " 
						<< local_path_.c_str() << std::endl;
		return -1;
	}

	task_manager_ = new task_manager();
	if (!task_manager_ || task_manager_->init() < 0) {
		std::cout << "thread pool init failed" << std::endl;
		_clean_up();
		return -1;
	}

	tasks_ = new task_manager::task_t[task_count_];
	if (!tasks_) {
		std::cout << "task holder init failed" << std::endl;
		_clean_up();
		return -1;
	}

	return 0;
}

int download_manager::_wrapper_task_handler(void *arg)
{
	task_data_t *data = static_cast<task_data_t *>(arg);
	download_manager *dm = data->manager;
	return dm->download_file_block(data->offset, data->size);
}

int download_manager::download_file_block(size_t offset, size_t size)
{
	return downloader_->download(remote_url_.c_str(), offset, size, local_fd_);
}

int download_manager::start()
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

		task_data_t *tdata = new task_data_t;
		tdata->offset = offset;
		tdata->size = size;
		tdata->manager = this;
		tasks_[i].handler = download_manager::_wrapper_task_handler;
		tasks_[i].param = static_cast<void *>(tdata);
		task_manager_->start_task(tasks_[i]);
	}
}

void download_manager::wait()
{
	task_manager_->wait_all_task_done();
}

int download_manager::destroy()
{
	_clean_up();
}
