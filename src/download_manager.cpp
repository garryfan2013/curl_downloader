#include <download_manager.h>
#include <iostream>
#include <functional>

download_manager::download_manager(downloader *downloader)
{
	_downloader = downloader;
}

download_manager::~download_manager()
{
	if (_thread_pool) {
		delete _thread_pool;
		_thread_pool = NULL;
	}

	/*
	* This is assigned by the invoker , so it's the invoker's responsibility 
	* to release it.
	*/
	_downloader = NULL;
}

void download_manager::set_url(const char *url)
{
	_url = url;
}

void download_manager::set_path(const char *path)
{
	_path = path;
}

void download_manager::set_thread_count(int count)
{
	_thread_count = count;
}

void download_manager::set_downloader(downloader *downloader)
{
	_downloader = downloader;
}

int download_manager::init()
{
	_file_size = _downloader->get_file_size(_url.c_str());
	if (_file_size <= 0) {
		std::cout << "download file size not correct" << std::endl;
		return -1;
	}

	_thread_pool = new thread_pool(_thread_count);
	if ( NULL == _thread_pool || _thread_pool->init() < 0 ) {
		std::cout << "thread pool init failed" << std::endl;
		return -1;
	}

	return 0;
}

typedef struct {
	const char *url;
	size_t offset;
	size_t size;
	const char *path;
}_worker_arg_t;

int download_manager::_wrapper_worker_function(void *arg)
{
	_worker_arg_t * wa = static_cast<_worker_arg_t *>(arg);
	return _downloader->download(wa->url, wa->offset, wa->size, wa->path);
}

int download_manager::start()
{
	int block_size = _file_size/_thread_count;
	int last_block_size = block_size + _file_size%_thread_count;

	for(int i=0; i<_thread_count; i++) {
		int offset = i*block_size;
		int size = (i == (_thread_count - 1))?block_size:last_block_size;

		_worker_arg_t wa;
		wa.url = _url.c_str();
		wa.offset = offset;
		wa.size = size;
		wa.path = _path.c_str();

		worker_function_t fp = std::bind(&download_manager::_wrapper_worker_function, 
										this, 
										std::placeholders::_1);
		_thread_pool->add_worker(fp, static_cast<void *>(&wa));
	}
}

int download_manager::stop()
{
	_thread_pool->destroy();
}
