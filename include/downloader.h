#ifndef _DOWNLOADER_H
#define _DOWNLOADER_H


class downloader
{
public:
	downloader() {};
	virtual ~downloader() {};

	virtual int init() = 0;

	virtual size_t get_file_size(const char *url) = 0;

	virtual int download(const char *remote_url, 
						const char *local_path, 
						size_t offset, 
						size_t size) = 0;

	virtual int destroy() = 0;
};


#endif /* _DOWNLOADER_H */
