/* 
 * file_handler.h
 * Normal file handler simple implementation
 * Cao liang<caoliang@gmail.com>
 */

#ifndef _FILE_HANDLER_H
#define _FILE_HANDLER_H

#include <cstring>

struct file_handler
{
	file_handler();
	~file_handler();

	int open(const char *local_path);
	size_t write(const void *data, size_t offset, size_t len);
	void close();

private:
	int fd;
};

#endif /* _FILE_HANDLER_H */

