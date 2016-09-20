#include "http_curl_downloader.h"
#include "download_manager.h"
#include <iostream>
#include <stdlib.h>

const static size_t _default_thread_count = 5;
#define TEST_URL "http://mirrors.cnnic.cn/apache//httpd/httpd-2.4.23.tar.gz"
#define TEST_PATH "/home/scutech/Downloads/test.tar.gz"

int main(int argc, char **argv)
{
	if (argc < 3) {
		std::cout << "usage:\nmy_downloader remote_url local_path [thread_count]" << std::endl;
		return 0;
	}

	size_t thread_count = _default_thread_count;

	if (argc >= 4) {
		thread_count = static_cast<size_t>(atoi(argv[3]));
	}

	download_manager *mgr = new download_manager(NULL);
	if (!mgr) {
		std::cout << "new download_manager failed" << std::endl;
		return -1;
	}

	downloader *dloader = new http_curl_downloader();
	if (!dloader) {
		std::cout << "new downloader failed" << std::endl;
		return -1;
	}

	mgr->set_remote_url(argv[1]);
	mgr->set_local_path(argv[2]);
	mgr->set_task_count(thread_count);
	mgr->set_downloader(dloader);

	if (mgr->init() < 0) {
		return -1;
	}

	if (mgr->start() < 0) {
		return -1;
	}

	mgr->wait();
	mgr->destroy();

	return 0;
}
