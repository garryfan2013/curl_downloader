#ifndef _DOWNLOADER_H
#define _DOWNLOADER_H


class downloader
{
public:
	downloader();
	virtual ~downloader();

	virtual int init() = 0;

	virtual size_t get_file_size(const char *url) = 0;

	virtual size_t download(
					const char *url, size_t offset, size_t size, const char *path) = 0;

	virtual int destory() = 0;
};


#endif /* _DOWNLOADER_H */
