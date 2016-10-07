#include "file_handler.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <iostream>

file_handler::file_handler():
	fd(-1) , size(0)
{
}

file_handler::~file_handler()
{
	close();
}

int file_handler::open(const char *local_path, size_t size)
{
	fd = ::open(local_path, O_RDWR | O_CREAT, 0755);
	if (fd < 0) {
		return -1;
	}

	size = size;
	return 0;
}

size_t file_handler::write(const void *data, size_t offset, size_t len)
{
	return ::pwrite(fd, data, len, offset);
}

void file_handler::close()
{
	if (fd >= 0) {
		::close(fd);
		fd = -1;
	}
}

mmap_file_handler::mmap_file_handler():
	fd(-1), map_addr(NULL), size(0) 
{
}

mmap_file_handler::~mmap_file_handler()
{
	close();
}

int mmap_file_handler::open(const char *local_path, size_t size)
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

size_t mmap_file_handler::write(const void *data, size_t offset, size_t len)
{
	char *dst = static_cast<char *>(map_addr) + offset;
	const char *src = static_cast<const char *>(data);

	memcpy(dst, src, len);
	return len;
}

void mmap_file_handler::close()
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
