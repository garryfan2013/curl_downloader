#include "curl/curl.h"
#include <iostream>
#include <unistd.h>

using namespace std;

template <typename T>
http_curl_downloader<T>::http_curl_downloader()
{
	
}

template <typename T>
http_curl_downloader<T>::~http_curl_downloader()
{

}

template <typename T>
int http_curl_downloader<T>::init()
{
	curl_global_init(CURL_GLOBAL_ALL);	
	return 0;
}

template <typename T>
size_t http_curl_downloader<T>::get_file_size(const char *url)
{
	double len = 0;

	CURL *curl = curl_easy_init();
	if (!curl) {
		std::cout << "curl init failed" << std::endl;
		return 0;
	}

	std::cout << "get_file_size: " << url << std::endl;

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, http_curl_downloader::_header_function);
	
	CURLcode code = curl_easy_perform(curl);
	if (CURLE_OK == code) {
		if (CURLE_OK != curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &len)) {
			std::cout << "get_file_size failed: " << std::endl;
			len = 0;
		} else {
			std::cout << "get_file_size " << len << std::endl;
		}
	} else {
		std::cout << "curl_easy_perform failed: " << code << std::endl;
	}

	curl_easy_cleanup(curl);
	return static_cast<size_t>(len);
}

template <typename T>
int http_curl_downloader<T>::download(
	const char *remote_url, size_t offset, size_t size, handler_type &handler)
{
	std::cout << "[task]:" << remote_url << " " << offset << " " << size << std::endl;

	CURL *curl = curl_easy_init();
	if (!curl) {
		std::cout << "download curl init failed" << std::endl;
		return -1;
	}

	char range[32] = {0};
	snprintf(range, 32, "%lu-%lu", offset, (offset + size - 1));
	write_data_info info(offset, size, handler);

	curl_easy_setopt(curl, CURLOPT_URL, remote_url);
	curl_easy_setopt(curl, CURLOPT_RANGE, range);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_function);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&info));
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

	CURLcode code = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	if (CURLE_OK != code) {
		std::cout << "curl_easy_perform failed: " << code << std::endl;
		return -1;
	}

	return 0;
}

template <typename T>
int http_curl_downloader<T>::destroy()
{
	curl_global_cleanup();
	return 0;
}

template <typename T>
size_t http_curl_downloader<T>::_header_function(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return size*nmemb;
}

template <typename T>
size_t http_curl_downloader<T>::_write_function(void *ptr, size_t size, size_t nmemb, void *stream)
{
	write_data_info *info = static_cast<write_data_info *>(stream);
	size_t bytes_to_write = ((nmemb*size) > info->size)?(info->size):(nmemb*size);
	size_t n = info->handler->write(ptr, info->offset, bytes_to_write);
	if (n != bytes_to_write) {
		//TO DO
		std::cout << "fwrite " << n << " bytes, but we should write " << bytes_to_write << " bytes";
	} else {
		std::cout << "write " << n << std::endl;
		if (info->size == bytes_to_write) {
			std:cout << "job's done!" << std::endl;
		}
	}

	info->offset += n;
	info->size -= n;
	return n;
}

