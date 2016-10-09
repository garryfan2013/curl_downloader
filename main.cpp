#include "download_manager.h"
#include <iostream>
#include <stdlib.h>

#define DEFAULT_THREAD_COUNT 5;

//test url: "http://mirrors.cnnic.cn/apache//httpd/httpd-2.4.23.tar.gz"

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

int make_file_path(const string &url, string &path)
{
	int pos = url.find_last_of('/');
	char *cwd = getcwd(NULL, 0);
	if (cwd) {
		path = cwd;
		path.append(url.substr(pos));
		free(cwd);
		return 0;
	}

	std::cout << "cant get current working directory" << std::endl;
	return -1;
}

int parse_args(int argc, char **argv, string &url, string &path, int &cnt)
{
	int ch;
	while((ch = getopt(argc, argv, "c:o:")) != -1) {
		switch(ch) {
			case 'c':
				cnt = atoi(optarg);
				break;

			case 'o':
				path = optarg;
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
	url = argv[optind];
	return 0;
}

int main(int argc, char **argv)
{
	string remote_url = "";
	string local_path = "";
	int thread_count = DEFAULT_THREAD_COUNT;

	if ( parse_args(argc, argv, remote_url, local_path, thread_count) < 0 ) {
		return -1;
	}

	if ("" == local_path) {
		if ( make_file_path(remote_url, local_path) < 0) {
			return -1;
		}
	}

	download_manager dm(remote_url.c_str(), local_path.c_str(), thread_count);
	if (dm.init() < 0 || dm.start() < 0) {
		return -1;
	}

	dm.wait_all_task_done();
	dm.destroy();
	return 0;
}
