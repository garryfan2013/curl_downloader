/* 
 * download_manager.h
 * Manage the whole download procedure 
 * Cao liang<caoliang@gmail.com>
 */ 

#ifndef _DOWNLOAD_MANAGER_H
#define _DOWNLOAD_MANAGER_H

#include "http_curl_downloader.h"
#include "task_manager.h"
#include <string>

// T : file handler
// U : downloader
template<typename T, typename U = http_curl_downloader<T> >
class download_manager
{
public:
	typedef typename T::handler_type handler_type;

	download_manager(const char *remote_url, 
					const char *path, 
					size_t count);
	~download_manager();

	void set_remote_url(const char *url);
	void set_local_path(const char *path);
	void set_task_count(int c);

	int init();
	int start();
	int download_file_block(size_t offset, size_t size);
	void wait();
	int destroy();
		
private:
	struct task_data
	{
		size_t offset;
		size_t size;
		download_manager<T, U> *manager;
	};

	void _clean_up();
	static int _wrapper_task_handler(void *arg);

	std::string remote_url_;
	std::string local_path_;
	size_t file_size_;
	handler_type file_handler_;
	U downloader_;
	
	size_t task_count_;
	task_manager task_manager_;
	task_manager::task_type *tasks_;
};

#include "download_manager.inl"

#endif /* _DOWNLOAD_MANAGER_H */
