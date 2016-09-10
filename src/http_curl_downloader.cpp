#include <http_curl_downloader.h>
#include <curl/curl.h>
#include <iostream>

using namespace std;

http_curl_downloader::http_curl_downloader()
{
	
}

http_curl_downloader::~http_curl_downloader()
{
	map<string, pthread_mutex_t *>::iterator iter;
	iter = _file_lock_map.begin();

	while(iter != _file_lock_map.end()) {
		if (NULL != iter->second) {
			delete iter->second;
			iter->second = NULL;
		}

		iter++;
	}
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

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_function);
	
	if (CURLE_OK == curl_easy_perform(curl)) {
		if (CURLE_OK != curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &len)) {
			std::cout << "get_file_size failed:" << std::endl;
			len = 0;
		}
	}

	curl_easy_cleanup(curl);

	return static_cast<size_t>(len);
}


int http_curl_downloader::download(
	const char *url, size_t offset, size_t size, const char *path)
{
	std::cout << "[task]:" << url << " " << offset << " " << size << std:endl;

	CURL *curl = curl_easy_init();
	if (!curl) {
		std::cout << "download curl init failed" << std::endl;
		return -1;
	}

	char range[32] = {0};
	snprintf(range, 32, "%lld-%lld", offseet, offset + size);

	file_block_info *fbi = new file_block_info(offset, size, path);
	if (!fbi) {
		std::cout << "new file_block_info failed" << std::endl;
		curl_easy_cleanup();		
		return -1;
	}

	curl_easy_setopt(curl, CURLOPT_RANGE, range);
	curl_easy_setopt(curl, CURLOPT_READFUNCTIN, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function);
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

size_t _header_function(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return size*nmemb;
}

size_t _write_function(void *ptr, size_t size, size_t nmemb, void *stream)
{
	file_block_info *fbi = static_cast<file_block_info *>stream;

	pthread_mutex_t *lock = _get_file_lock(fbi->path());
	pthread_mutex_lock(lock);

	FILE *fp = fopen(fbi->path(), "wb+");
	if (!fp) {
		std::cout << "open file failed " << fbi->path() << std::endl;
		pthread_mutex_unlock(lock);
		return 0;
	}

	bytes_to_write = ((nmemb*size) > fbi->size())?(fbi->size()):(nmemb*size);

	fseek(fp, fbi->offset(), SEEK_SET);
	size_t n = fwrite(ptr, 1, bytes_to_write, fp);
	if (n != bytes_to_write) {
		//To be added here , what if the bytes actually written , not equal to the bytes
		//we should write into the file, the strategy needs to be improved in the future
		std::cout << "fwrite " << n << " bytes, but we should write " << bytes_to_write << " bytes"
	}

	fbi->offset(fbi->offset() + bytes_to_write);
	fbi->size(fbi->size() - bytes_to_write);

	fclose(fp);	

	pthread_mutex_unlock(lock);

	return n;
}

pthread_mutext_t *_get_file_lock(const char *path)
{
	map<string, pthread_mutex_t *>::iterator it = _file_lock_map.find(path);

	if (it != _file_lock_map.end()) {
		return it->sencond;
	} else {
		pthread_mutex_t *lock = (pthread_mutext_t *)malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(lock, NULL);
		_file_lock_map.insert(pair<string, pthread_mutex_t *>(string(path), lock));

		return lock;
	}
}

