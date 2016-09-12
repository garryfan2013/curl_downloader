#include <http_curl_downloader.h>
#include <download_manager.h>
#include <iostream>


#define DEFAULT_THREAD_COUNT 5

#define TEST_URL "http://mirrors.cnnic.cn/apache//httpd/httpd-2.4.23.tar.gz"
#define TEST_PATH "/home/scutech/Downloads/test.tar.gz"

int main(int argc, char **argv)
{
	//my_downloader http://openstack.org/p/etherpad.tgz 5
#if 0
	if (argc < 3) {
		std::cout << "usage: my_downloader remote_url local_path [thread_count]";
		return 0;
	}

	size_t thread_count = DEFAULT_THREAD_COUNT;

	if (argc >= 4) {
		thread_count = static_cast<size_t>(atoi(argv[3]));
	}

	download_manager *mgr = new download_manager(NULL);
	downloader *dloader = new http_curl_downloader();

	mgr->set_url(argv[1]);
	mgr->set_path(argv[2]);
	mgr->set_thread_count(thread_count);
	mgr->set_downloader(dloader);
#else
	download_manager *mgr = new download_manager(NULL);
	downloader *dloader = new http_curl_downloader();
	mgr->set_url(TEST_URL);
	mgr->set_path(TEST_PATH);
	mgr->set_thread_count(1);
	mgr->set_downloader(dloader);
#endif
	mgr->init();
	mgr->start();
	mgr->wait();
	mgr->stop();
	return 0;
}
