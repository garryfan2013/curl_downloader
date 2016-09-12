#include <download_manager.h>
#include <iostream>
#include <functional>

worker_arg_t *download_manager::_worker_args = NULL;

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
	if (NULL == _thread_pool || _thread_pool->init() < 0) {
		std::cout << "thread pool init failed" << std::endl;
		return -1;
	}

	_worker_args = new worker_arg_t[_thread_count]();
	if (!_worker_args) {
		std::cout << "woker_args init failed" << std::endl;
		return -1;
	}

	return 0;
}

int download_manager::_wrapper_worker_function(void *arg)
{
	worker_arg_t *wa = static_cast<worker_arg_t *>(arg);
	download_manager *dm = wa->manager;
	return dm->download(wa->url.c_str(), 
						wa->path.c_str(), 
						wa->offset, 
						wa->size);
}

int download_manager::download(
	const char *remote_url, const char *local_path, size_t offset, size_t size)
{
	return _downloader->download(remote_url, local_path, offset, size );
}

int download_manager::start()
{
	int block_size = _file_size/_thread_count;
	int last_block_size = block_size + _file_size%_thread_count;

	for(int i=0; i<_thread_count; i++) {
		int offset = i*block_size;
		int size = (i == (_thread_count - 1))?block_size:last_block_size;

		_worker_args[i].url = _url;
		_worker_args[i].offset = offset;
		_worker_args[i].size = size;
		_worker_args[i].path = _path;
		_worker_args[i].manager = this;

		_thread_pool->add_worker(download_manager::_wrapper_worker_function,
									 static_cast<void *>(&(_worker_args[i])));
	}
}

void download_manager::wait()
{
	_thread_pool->wait_all();
}

int download_manager::stop()
{
	_thread_pool->destroy();
}
