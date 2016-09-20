/* 
 * downloader.h
 * description: abstract class for base downloader
 * Author: xxx<xxx@gmail.com>
 */

#ifndef _DOWNLOADER_H
#define _DOWNLOADER_H

#include <stdio.h>
#include <pthread.h>

class downloader
{
public:
	downloader() {};
	virtual ~downloader() {};

	virtual int init() = 0;

	virtual size_t get_file_size(const char *url) = 0;

	virtual int download(const char *remote_url, 
						size_t offset, 
						size_t size,
						FILE *file,
						pthread_mutex_t *lock) = 0;

	virtual int destroy() = 0;
};


#endif /* _DOWNLOADER_H */
