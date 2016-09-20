/* 
 * http_curl_downloader.h
 * description: http curl implementation of downloader
 * Author: xxx<xxx@gmail.com>
 */ 

#ifndef _HTTP_CURL_DOWNLOADER_H
#define _HTTP_CURL_DOWNLOADER_H

#include "downloader.h"
#include <string>
#include <map>

class file_block_info
{
public:
	file_block_info(size_t offset, size_t size, FILE *file, pthread_mutex_t *lock) 
	{
		_offset = offset;
		_size = size;
		_file = file;
		_lock = lock;
	};

	virtual ~file_block_info() {};

	size_t offset() const { return _offset; };\
	void offset(size_t offset) { _offset = offset; };

	size_t size() const { return _size; };
	void size(size_t size) { _size = size; };

	FILE *file() const { return _file; };
	void file(FILE *file) { _file = file; };

	pthread_mutex_t *lock() const { return _lock; };
	void lock(pthread_mutex_t *lock) { _lock = lock; };

private:
	size_t _offset;
	size_t _size;
	
	FILE *_file;
	pthread_mutex_t *_lock;
};

class http_curl_downloader: public downloader
{
public:
	http_curl_downloader();
	virtual ~http_curl_downloader();

	int init();

	size_t get_file_size(const char *url);

	int download(const char *remote_url, 
				const size_t offset, 
				const size_t size,
				FILE *file,
				pthread_mutex_t *lock);

	int destroy();

private:
	static size_t _header_function(void *ptr, size_t size, size_t nmemb, void *stream);

	static size_t _write_function(void *ptr, size_t size, size_t nmemb, void *stream);
};


#endif /* _HTTP_CURL_DOWNLOADER */
