/* 
 * download_manager.h
 * description: manage the whole download procedure 
 * Author: xxx<xxx@gmail.com>
 */ 

#ifndef _DOWNLOAD_MANAGER_H
#define _DOWNLOAD_MANAGER_H

#include "task_manager.h"
#include "downloader.h"
#include <string>
#include <pthread.h>

class download_manager
{
public:
	download_manager(const char *remote_url = NULL, 
					const char *path = NULL, 
					size_t count = 0, 
					downloader *downloader = NULL);
	virtual ~download_manager();

	void set_remote_url(const char *url);
	void set_local_path(const char *path);
	void set_task_count(int c);
	void set_downloader(downloader *downloader);

	int init();
	int start();
	int download_file_block(size_t offset, size_t size);
	void wait();
	int destroy();
		
private:
	void _clean_up();

	static int _wrapper_task_handler(void *arg);
	
	size_t file_size_;
	FILE *file_;
	pthread_mutex_t file_lock_;

	std::string remote_url_;
	std::string local_path_;

	downloader *downloader_;
	
	size_t task_count_;
	task_manager *task_manager_;
	task_manager::task_t *tasks_;
};



#endif /* _DOWNLOAD_MANAGER_H */
