/* 
 * download_manager.cpp
 * description: manage the whole download procedure 
 * Author: xxx<xxx@gmail.com>
 */ 
 
#include "downloader.h"
#include "task_manager.h"
#include "download_manager.h"
#include <iostream>

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

	/*
	* it's the upper level controller's responsibility to release it. 
	*/
	downloader_ = NULL;
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
	if (file_) {
		fclose(file_);
		file_ = NULL;
	}

	if (task_manager_) {
		delete task_manager_;
		task_manager_ = NULL;
	}

	if (tasks_) {
		for (int i=0; i<task_count_; i++) {
			if (NULL != tasks_[i].param) {
				delete static_cast<task_data_t *>(tasks_[i].param);
			}
		}
		delete[] tasks_;
	}
}

int download_manager::init()
{
	file_size_ = downloader_->get_file_size(remote_url_.c_str());
	if (file_size_ <= 0) {
		std::cout << "download file size not correct" << std::endl;
		return -1;
	}
	
	file_ = fopen(local_path_.c_str(), "wb+");
	if (!file_ ) {
		std::cout << "download_manager open file failed: " 
						<< local_path_.c_str() << std::endl;
		return -1;
	}

	if (pthread_mutex_init(&file_lock_, NULL) < 0) {
		std::cout << "file lock init failed" << std::endl;
		_clean_up();
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
	return downloader_->download(remote_url_.c_str(), 
								offset, size, file_, &file_lock_);
}

int download_manager::start()
{
	size_t block_size = file_size_/task_count_;
	size_t last_block_size = block_size + file_size_%task_count_;

	for(int i=0; i<task_count_; i++) {
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
