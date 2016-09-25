/* 
 * http_curl_downloader.h
 * description: http curl implementation of downloader
 * Author: xxx<xxx@gmail.com>
 */ 

#ifndef _HTTP_CURL_DOWNLOADER_H
#define _HTTP_CURL_DOWNLOADER_H

#include <string>
#include <map>

template <typename T>
class http_curl_downloader
{
public:
	http_curl_downloader();
	virtual ~http_curl_downloader();

	int init();

	size_t get_file_size(const char *url);

	int download(const char *remote_url, 
				const size_t offset, 
				const size_t size,
				typename T::handler_type &handler);

	int destroy();

private:
	struct write_data_info
	{
		write_data_info(size_t offset, size_t size, typename T::handler_type handler):
			offset(offset), size(size), handler(handler) { };

		size_t offset;
		size_t size;
		typename T::handler_type handler;
	};

	static size_t _header_function(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t _write_function(void *ptr, size_t size, size_t nmemb, void *stream);
};

#include "http_curl_downloader.inl"
#endif /* _HTTP_CURL_DOWNLOADER_H */