#include "curl/curl.h"
#include <memory>
#include <iostream>
#include <unistd.h>

using namespace std;

template <typename T>
http_curl_downloader_tmpl<T>::http_curl_downloader_tmpl()
{	
}

template <typename T>
http_curl_downloader_tmpl<T>::~http_curl_downloader_tmpl()
{
    curl_global_cleanup();
}

template <typename T>
int http_curl_downloader_tmpl<T>::init()
{
    curl_global_init(CURL_GLOBAL_ALL);
    return 0;
}

template <typename T>
size_t http_curl_downloader_tmpl<T>::get_file_size(const char *url)
{
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cout << "curl init failed" << std::endl;
        return 0;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, http_curl_downloader_tmpl::_header_function);
	
    double len = 0;
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
int http_curl_downloader_tmpl<T>::_download(const char *remote_url, write_data_info *info)
{
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cout << "download curl init failed" << std::endl;
        return -1;
    }

    char range[128] = {0};
    snprintf(range, sizeof(range), "%lu-%lu", info->offset, (info->offset + info->size - 1));

    curl_easy_setopt(curl, CURLOPT_URL, remote_url);
    curl_easy_setopt(curl, CURLOPT_RANGE, range);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_function);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(info));
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
int http_curl_downloader_tmpl<T>::download(
    const char *remote_url, size_t offset, size_t size, handler_type &handler)
{
    unique_ptr<write_data_info> info(new write_data_info(offset, size, handler));
    int retry_count = MAX_RETRY_COUNT;
    do {
        if (0 == _download(remote_url, info.get())) {
            return 0;
        }

        std::cout << "_download failed , retry left: " << retry_count << std::endl;
        ::sleep(RETRY_INTERVAL);
    } while(retry_count-- > 0);

    return -1;
}

template <typename T>
int http_curl_downloader_tmpl<T>::destroy()
{
    curl_global_cleanup();
    return 0;
}

template <typename T>
size_t http_curl_downloader_tmpl<T>::_header_function(void *ptr, size_t size, size_t nmemb, void *stream)
{
    return size*nmemb;
}

template <typename T>
size_t http_curl_downloader_tmpl<T>::_write_function(void *ptr, size_t size, size_t nmemb, void *stream)
{
    write_data_info *info = static_cast<write_data_info *>(stream);
    size_t bytes_to_write = ((nmemb*size) > info->size)?(info->size):(nmemb*size);
    size_t n = info->handler->write(ptr, info->offset, bytes_to_write);
    if (n != bytes_to_write) {
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

