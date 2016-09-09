#include <download_manager.h>

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
		std::out << "download file size not correct" << std::endl;
		return -1;
	}

	_thread_pool = new thread_pool(_thread_count);
	if ( NULL == _thread_pool || _thread_pool->init() < 0 ) {
		std::out << "thread pool init failed" << std::endl;
		return -1;
	}

	_file = fopen(_path, "w+");
	if (!file) {
		std::out << "open local file failed" << std:endl;
		return -1;
	}

	return 0;
}

int download_manager::start()
{
	int block_size = _file_size/_thread_count;
	int last_block_size = block_size + _file_size%_thread_count;

	for(int i=0; i<_thread_count; i++) {
		int offset = i*block_size;
		int size = (i == (_thread_count - 1))?block_size:last_block_size

		std::function<int(void*)> fp = 
				std:bind(&(_downloader->download), _url.c_str(), offset, size, std::placeholders::_1);
		_thread_pool->add_worker(fp, _path.c_str());
	}
}

int download_manager:stop()
{
	_thread_pool->destroy();
}
