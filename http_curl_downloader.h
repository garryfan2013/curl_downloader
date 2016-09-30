/* 
 * http_curl_downloader.h
 * Implementation of downloader use http curl
 * Cao liang<caoliang@gmail.com>
 */ 

#ifndef _HTTP_CURL_DOWNLOADER_H
#define _HTTP_CURL_DOWNLOADER_H

#include <string>

// T: file handler
template <typename T>
class http_curl_downloader
{
public:
	typedef typename T::handler_type handler_type;

	http_curl_downloader();
	~http_curl_downloader();

	int init();

	size_t get_file_size(const char *url);

	int download(const char *remote_url, 
				const size_t offset, 
				const size_t size,
				handler_type &handler);

	int destroy();

private:
	struct write_data_info
	{
		write_data_info(size_t offset, size_t size, handler_type &handler):
			offset(offset), size(size), handler(&handler)
		{
		};

		size_t offset;
		size_t size;
		handler_type *handler;
	};

	static size_t _header_function(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t _write_function(void *ptr, size_t size, size_t nmemb, void *stream);
};

#include "http_curl_downloader.inl"
#endif /* _HTTP_CURL_DOWNLOADER_H */
