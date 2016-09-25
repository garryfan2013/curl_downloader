#include "download_manager.h"
#include "http_curl_downloader.h"
#include "file_handler_traits.h"
#include <iostream>
#include <stdlib.h>

#define DEFAULT_THREAD_COUNT 5;

#define TEST_URL "http://mirrors.cnnic.cn/apache//httpd/httpd-2.4.23.tar.gz"
#define TEST_PATH "/home/scutech/Downloads/test.tar.gz"

int main(int argc, char **argv)
{
	if (argc < 3) {
		std::cout << "usage:\nmy_downloader remote_url local_path [thread_count]" << std::endl;
		return 0;
	}

	int thread_count = DEFAULT_THREAD_COUNT;

	if (argc >= 4) {
		thread_count = atoi(argv[3]);
		if (thread_count <= 0) {
			thread_count = DEFAULT_THREAD_COUNT;
		}
	}
#ifdef USE_MMAP
	std::cout << "USE_MMAP" << std::endl;
	download_manager<mmap_file_handler_type> manager(argv[1], argv[2], thread_count);
#else
	download_manager<normal_file_handler_type> manager(argv[1], argv[2], thread_count);
#endif

	if (manager.init() < 0 || manager.start() < 0) {
		return -1;
	}

	manager.wait();
	manager.destroy();
	return 0;
}