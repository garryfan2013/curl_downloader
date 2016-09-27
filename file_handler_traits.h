/* 
 * file_handler_traits.h
 * Traits for normal file handler & mmap file handler
 * Cao liang<caoliang@gmail.com>
 */

#ifndef _FILE_HANDLER_TRAITS_H
#define _FILE_HANDLER_TRAITS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>

template <typename T>
struct file_handler_traits
{
	typedef T handler_type;
};

struct file_handler
{
	file_handler(): fd(-1) , size(0)
	{
		
	}

	int open(const char *local_path, size_t size) 
	{
		fd = ::open(local_path, O_RDWR | O_CREAT, 0755);
		if (fd < 0) {
			return -1;
		}

		size = size;
		return 0;
	}

	size_t write(const void *data, size_t offset, size_t len)
	{
		return ::pwrite(fd, data, len, offset);
	}

	void close()
	{
		if (fd >= 0) {
			::close(fd);
			fd = -1;
		}
	}

	int fd;
	size_t size;
};

struct mmap_file_handler
{
	mmap_file_handler(): fd(-1), map_addr(NULL), size(0) 
	{
		
	}

	int open(const char *local_path, size_t size)
	{
		fd = ::open(local_path, O_RDWR | O_CREAT, 0755);
		if (fd < 0) {
			std::cout << "open " << local_path << " failed" << std::endl;
			return -1;
		}

		//Prepare the file for mapping
		::lseek(fd, size - 1, SEEK_SET);
		::write(fd, "a", 1);

		void *ptr = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
		if (ptr == MAP_FAILED) {
			std::cout << "mmap failed:" << std::endl;
			return -1;
		}

		map_addr = ptr;
		size = size;
		return 0;
	}

	size_t write(const void *data, size_t offset, size_t len)
	{
		char *dst = static_cast<char *>(map_addr) + offset;
		const char *src = static_cast<const char *>(data);

		memcpy(dst, src, len);
		return len;
	}

	void close()
	{
		if (map_addr) {
			munmap(map_addr, size);
			map_addr = NULL;
		}

		if (fd >= 0) {
			::close(fd);
			fd = -1;
		}
	}
	
	int fd;
	void *map_addr;
	size_t size;
};

template <>
struct file_handler_traits<file_handler>
{
	typedef file_handler handler_type;
};

template <>
struct file_handler_traits<mmap_file_handler>
{
	typedef mmap_file_handler handler_type;
};

typedef file_handler_traits<file_handler> normal_file_handler_type;
typedef file_handler_traits<mmap_file_handler> mmap_file_handler_type;



#endif /* _FILE_HANDLER_TRAITS_H */

