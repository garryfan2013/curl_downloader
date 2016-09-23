#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <errno.h>

template<typename T>
struct file_handler_traits
{
	typedef T handle_type;

	bool invalid_handle(T handle);
	
	T open(const char *local_path, size_t size = 0);

	size_t write(T handle, const void *data, size_t offset, size_t len);

	void close(T handle );
};

template<>
struct file_handler_traits<int>
{
	typedef int handle_type;

	bool invalid_handle(int handle) {
		return ((handle < 0)?true:false);
	}
	
	int open(const char *local_path, size_t size) 
	{
		int fd = ::open(local_path, O_WRONLY | O_CREAT);

		if (fd < 0) {
			return -1;
		}

		return fd;
	}

	size_t write(int handle, const void *data, size_t offset, size_t len)
	{
		return pwrite(handle, data, len, offset);
	}

	void close(int handle)
	{
		::close(handle);
	}
};

typedef struct
{
	void *map_addr;
	int fd;
	size_t size;
}mmap_handle_t;

template<>
struct file_handler_traits<mmap_handle_t *>
{
	typedef mmap_handle_t *handle_type;
	
	bool invalid_handle(mmap_handle_t *handle) {
		return ((!handle)?true:false);
	}

	mmap_handle_t *open(const char *local_path, size_t size)
	{
		int fd = ::open(local_path, O_RDWR | O_CREAT);
		if (fd < 0) {
			std::cout << "open " << local_path << " failed" << std::endl;
			return NULL;
		}

		void *ptr = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
		if (ptr == MAP_FAILED) {
			std::cout << "mmap failed:" << errno << std::endl;
			return NULL;
		}

		mmap_handle_t *handle = new mmap_handle_t;
		handle->map_addr = ptr;
		handle->fd = fd;
		handle->size = size;

		return handle;
	}

	size_t write(mmap_handle_t *handle, const void *data, size_t offset, size_t len)
	{
		char *dst = static_cast<char *>(handle->map_addr) + offset;
		const char *src = static_cast<const char *>(data);

		memcpy(dst, src, len);
		return len;
	}

	void close(mmap_handle_t *handle)
	{
		munmap(handle->map_addr, handle->size);
		::close(handle->fd);
		delete handle;
	}
};


typedef file_handler_traits<int> normal_file_handler_type;

typedef file_handler_traits<mmap_handle_t *> mmap_file_handler_type;

































