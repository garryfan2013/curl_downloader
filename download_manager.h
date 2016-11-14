/* 
 * download_manager.h
 * Manage the whole download procedure 
 * Cao liang<caoliang@gmail.com>
 */ 

#ifndef _DOWNLOAD_MANAGER_H
#define _DOWNLOAD_MANAGER_H

#include "http_curl_downloader.h"
#include <string>
#include <vector>
#include <thread>

// T : file handler
// U : downloader
template<typename T, typename U = http_curl_downloader<T> >
class download_manager
{
public:
	typedef T handler_type;

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
	void wait_all_task_done();
	int destroy();
		
private:
	typedef std::shared_ptr<std::thread> thread_ptr;
	
	std::string remote_url_;
	std::string local_path_;
	size_t file_size_;
	handler_type file_handler_;
	U downloader_;

	std::vector<thread_ptr> threads_;
	size_t task_count_;
};

#include "download_manager.inl"

#endif /* _DOWNLOAD_MANAGER_H */
