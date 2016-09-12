#ifndef _DOWNLOAD_MANAGER_H
#define _DOWNLOAD_MANAGER_H

#include <string>

#include <downloader.h>
#include <thread_pool.h>

using namespace std;


typedef struct {
	size_t offset;
	size_t size;
	class download_manager *manager;
}worker_arg_t;

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

	int download_file_block(size_t offset, size_t size);

	void wait();

	int stop();	
		
private:
	void _clean_up();

	static int _wrapper_worker_function(void *arg);
	static worker_arg_t *_worker_args;

	int _thread_count;
	int _file_size;

	FILE *_file;
	pthread_mutex_t _file_lock;
	
	std::string _url;
	std::string _path;

	downloader *_downloader;
	thread_pool *_thread_pool;
};







































#endif /* _DOWNLOAD_MANAGER_H */
