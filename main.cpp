#include "download_manager.h"
#include <iostream>
#include <stdlib.h>

#define DEFAULT_THREAD_COUNT 5;

#define TEST_URL "http://mirrors.cnnic.cn/apache//httpd/httpd-2.4.23.tar.gz"
#define TEST_PATH "/home/scutech/Downloads/test.tar.gz"

using namespace std;

void display_usage()
{
	cout << 
		"Usage: my_downloader [URL] [OPTION]...\n"
		"options include:\n"
		"    -c [COUNT], specify thread count used for downloading\n"
		"    -o [PATH], specify the output file path to save\n"
		<< endl;
}

// To do 
// It's better to get the real file name from http server other than url
void make_file_path(const string &url, string &path)
{
	int pos = url.find_last_of('/');
	char cwd[128];
	getcwd(cwd, 128);
	path = cwd;
	path.append(url.substr(pos));
}

int main(int argc, char **argv)
{
	string remote_url = "";
	string local_path = "";
	int thread_count = DEFAULT_THREAD_COUNT;

	int ch;
	while((ch = getopt(argc, argv, "c:o:")) != -1) {
		switch(ch) {
			case 'c':
				thread_count = atoi(optarg);
				break;

			case 'o':
				local_path = optarg;
				break;
					
			default:
				cout << "ignore unsupported options :" << argv[optind] << endl;
				break;
		}
	}

	if (optind >= argc) {
		display_usage();
		return -1;
	}
	remote_url = argv[optind];

	if ("" == local_path) {
		make_file_path(remote_url, local_path);
	}

	download_manager dm(remote_url.c_str(), local_path.c_str(), thread_count);
	if (dm.init() < 0 || dm.start() < 0) {
		return -1;
	}

	dm.wait_all_task_done();
	dm.destroy();
	return 0;
}
