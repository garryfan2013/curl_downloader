#include "http_curl_downloader.h"
#include "curl/curl.h"
#include <iostream>
#include <unistd.h>

using namespace std;

typedef struct write_data_info
{
	write_data_info(size_t offset, size_t size, int fd):
		offset(offset), size(size), fd(fd) { };

	size_t offset;
	size_t size;
	int fd;
}write_data_info_t;

http_curl_downloader::http_curl_downloader()
{
	
}

http_curl_downloader::~http_curl_downloader()
{

}

int http_curl_downloader::init()
{
	curl_global_init(CURL_GLOBAL_ALL);	
	return 0;
}

size_t http_curl_downloader::get_file_size(const char *url)
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
	
	if (CURLE_OK == curl_easy_perform(curl)) {
		if (CURLE_OK != curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &len)) {
			std::cout << "get_file_size failed:" << std::endl;
			len = 0;
		} else {
			std::cout << "get_file_size " << len << std::endl;
		}
	}

	curl_easy_cleanup(curl);
	return static_cast<size_t>(len);
}

int http_curl_downloader::download(
	const char *remote_url, size_t offset, size_t size, int fd)
{
	std::cout << "[task]:" << remote_url << " " << offset << " " << size << std::endl;

	CURL *curl = curl_easy_init();
	if (!curl) {
		std::cout << "download curl init failed" << std::endl;
		return -1;
	}

	char range[32] = {0};
	snprintf(range, 32, "%lu-%lu", offset, (offset + size - 1));
	write_data_info_t info(offset, size, fd);

	curl_easy_setopt(curl, CURLOPT_URL, remote_url);
	curl_easy_setopt(curl, CURLOPT_RANGE, range);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_function);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&info));
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	CURLcode code = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (CURLE_OK != code) {
		std::cout << "curl_easy_perform failed:" << code << std::endl;
		return -1;
	}

	return 0;
}

int http_curl_downloader::destroy()
{
	curl_global_cleanup();
	return 0;
}

size_t http_curl_downloader::_header_function(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return size*nmemb;
}

size_t http_curl_downloader::_write_function(void *ptr, size_t size, size_t nmemb, void *stream)
{
	write_data_info_t *info = static_cast<write_data_info_t *>(stream);

	int fd = info->fd;
	if (fd < 0) {
		std::cout << "file handle invalid!" << std::endl;
		return 0;
	}
	
	size_t bytes_to_write = ((nmemb*size) > info->size)?(info->size):(nmemb*size);
	size_t n = pwrite(fd, ptr, bytes_to_write, info->offset);
	if (n != bytes_to_write) {
		//To be added here , what if the bytes actually written , not equal to the bytes
		//we should write into the file, the strategy needs to be improved in the future
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

