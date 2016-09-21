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
				int write_fd);

	int destroy();

private:
	static size_t _header_function(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t _write_function(void *ptr, size_t size, size_t nmemb, void *stream);
};


#endif /* _HTTP_CURL_DOWNLOADER_H */
