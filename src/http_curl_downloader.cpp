#include <http_curl_downloader.h>
#include <curl/curl.h>
#include <iostream>

using namespace std;


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
	const char *remote_url, size_t offset, size_t size, FILE *file, pthread_mutex_t *lock)
{
	std::cout << "[task]:" << remote_url << " " << offset << " " << size << std::endl;

	CURL *curl = curl_easy_init();
	if (!curl) {
		std::cout << "download curl init failed" << std::endl;
		return -1;
	}

	char range[32] = {0};
	snprintf(range, 32, "%lu-%lu", offset, (offset + size - 1));

	file_block_info *fbi = new file_block_info(offset, size, file, lock);
	if (!fbi) {
		std::cout << "new file_block_info failed" << std::endl;
		curl_easy_cleanup(curl);	
		return -1;
	}

	curl_easy_setopt(curl, CURLOPT_URL, remote_url);
	curl_easy_setopt(curl, CURLOPT_RANGE, range);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_function);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)fbi);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	
	CURLcode code = curl_easy_perform(curl);
	
	curl_easy_cleanup(curl);
	delete fbi;

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
	file_block_info *fbi = static_cast<file_block_info *>(stream);

	pthread_mutex_t *lock = fbi->lock();
	pthread_mutex_lock(lock);

	std::cout << "onWrite fbi: " << fbi->offset() << " " << fbi->size() << std::endl;
	std::cout << "onWrtie cb: " << size*nmemb << std::endl; 

	FILE *fp = fbi->file();
	if (!fp) {
		std::cout << "file block info invalid FILE " << std::endl;
		pthread_mutex_unlock(lock);
		return 0;
	}

	fseek(fp, fbi->offset(), SEEK_SET);
	
	size_t bytes_to_write = ((nmemb*size) > fbi->size())?(fbi->size()):(nmemb*size);
	size_t n = fwrite(ptr, 1, bytes_to_write, fp);
	if (n != bytes_to_write) {
		//To be added here , what if the bytes actually written , not equal to the bytes
		//we should write into the file, the strategy needs to be improved in the future
		std::cout << "fwrite " << n << " bytes, but we should write " << bytes_to_write << " bytes";
	} else {
		std::cout << "write " << n << std::endl;
		if (fbi->size() == bytes_to_write) {
			std:cout << "job's done!" << std::endl;
		}
	}

	fbi->offset(fbi->offset() + bytes_to_write);
	fbi->size(fbi->size() - bytes_to_write);

	pthread_mutex_unlock(lock);

	return n;
}

