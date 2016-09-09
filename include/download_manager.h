#ifndef _DOWNLOAD_MANAGER_H
#define _DOWNLOAD_MANAGER_H

#include <string>

#include <downloader.h>
#include <thread_pool.h>

using namespace std;


class download_manager
{
public:
	download_manager(downloader *downloader);
	virtual ~download_manager();

	void set_url(const char *url);

	void set_path(const char *path);

	void set_thread_count(const int c);

	void set_downloader(downloader *downloader);

	int init();

	int start();

	int stop();	
		
private:
	int _wrapper_worker_function(void *arg);

	int _thread_count;
	int _file_size;
	
	std::string _url;
	std::string _path;

	downloader *_downloader;
	thread_pool *_thread_pool;
};







































#endif /* _DOWNLOAD_MANAGER_H */
