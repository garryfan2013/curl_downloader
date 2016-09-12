#ifndef _HTTP_CURL_DOWNLOADER_H
#define _HTTP_CURL_DOWNLOADER_H

#include <string>
#include <map>

#include <downloader.h>

class file_block_info
{
public:
	file_block_info(size_t offset, size_t size, const char *path) 
	{
		_offset = offset;
		_size = size;
		_path = path;
	};

	virtual ~file_block_info() {};

	size_t offset() { return _offset; };\
	void offset(size_t offset) { _offset = offset; };

	size_t size() { return _size; };
	void size(size_t size) { _size = size; };

	const char *url() { return _url.c_str(); };
	void url(const char *url) { _url = url; };

	const char *path() { return _path.c_str(); };
	void path(const char *path) { _path = path; };

private:
	size_t _offset;
	size_t _size;

	std::string _url;
	std::string _path;
};

class http_curl_downloader: public downloader
{
public:
	http_curl_downloader();
	virtual ~http_curl_downloader();

	int init();

	size_t get_file_size(const char *url);

	int download(const char *remote_url, 
				const char *local_path, 
				const size_t offset, 
				const size_t size);

	int destroy();

	static pthread_mutex_t* _get_file_lock(const char *path);
	static std::map<std::string, pthread_mutex_t *> _file_lock_map;

private:
	static size_t _header_function(void *ptr, size_t size, size_t nmemb, void *stream);

	static size_t _write_function(void *ptr, size_t size, size_t nmemb, void *stream);
};


#endif /* _HTTP_CURL_DOWNLOADER */
