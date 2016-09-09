#include "http_curl_downloader.h"
#include "download_manager.h"

#define DEFAULT_THREAD_COUNT 5

int main(int argc, void **argv)
{
	//my_downloader http://openstack.org/p/etherpad.tgz 5

	if (argc < 3) {
		std::out << "usage: my_downloader remote_url local_path [thread_count]"
		return 0;
	}

	size_t thread_count = DEFAULT_THREAD_COUNT;

	if (argc >= 4) {
		thread_count = static_cast<size_t>atoi(argv[2]);
	}

	download_manager *mgr = new download_manager();
	downloader *dloader = new http_curl_downloader();

	mgr->set_url(argv[1]);
	mgr->set_path(argv[2]);
	mgr->set_thread_count(thread_count);
	mgr->set_downloader(dloader);

	mgr->init();

	mgr->start();
}
