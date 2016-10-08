/* 
 * http_curl_downloader_tmpl.h
 * Implementation of downloader use http curl
 * Cao liang<caoliang@gmail.com>
 */ 

#ifndef _HTTP_CURL_DOWNLOADER_TMPL_H
#define _HTTP_CURL_DOWNLOADER_TMPL_H

#include <string>

// T: Data handler
template <typename T>
class http_curl_downloader_tmpl
{
public:
	typedef T handler_type;

	http_curl_downloader_tmpl();
	~http_curl_downloader_tmpl();

	int init();
	size_t get_file_size(const char *url);
	int download(const char *remote_url, 
				const size_t offset, 
				const size_t size,
				handler_type &handler);
	int destroy();

private:
	enum {
		MAX_RETRY_COUNT = 5,
		RETRY_INTERVAL = 15
	};

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
	
	int _download(const char *remote_url, write_data_info *info);
	static size_t _header_function(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t _write_function(void *ptr, size_t size, size_t nmemb, void *stream);
};

#include "http_curl_downloader_tmpl.inl"
#endif /* _HTTP_CURL_DOWNLOADER_TMPL_H */
