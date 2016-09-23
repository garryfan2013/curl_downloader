#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

template<typename T>
struct file_handler_traits
{
	typedef T handle_type;

	typename T::handle_type open(const char *local_path, size_t size = 0);

	size_t write(handle_type handle, void *data, size_t offset, size_t len);

	void close(handle_type handle );
};

template<>
struct file_handler_traits<int>
{
	typedef int handle_type;
	static const handle_type INVALID_HANDLE = -1;
	
	int open(const char *local_path, size_t size) 
	{
		int fd = open(local_path, O_WRONLY | O_CREATE);

		if (fd < 0) {
			return INVALID_HANDLE;
		}

		return fd;
	}

	static size_t write(int handle, void *data, size_t offset, size_t len)
	{
		return pwrite(handle, data, len, offset);
	}

	static void close(int handle)
	{
		close(handle);
	}
}

typedef struct
{
	void *map_addr;
	int fd;
	size_t size;
}mmap_handle_t;

template<>
struct file_handler_traits<mmap_handle_t *>
{
	typedef (mmap_handle*) handle_type;
	static const handle_type INVALID_HANDLE = NULL;

	mmap_handle *open(const char *local_path, size_t size)
	{
		int fd = open(local_path, O_WRONLY | O_CREATE);
		if (fd < 0) {
			return INVALID_HANDLE;
		}

		void *ptr = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
		if (ptr == MAP_FAILED) {
			return INVALID_HANDLE;
		}

		mmap_handle_t *handle = new mmap_handle_t;
		handle->map_addr = ptr;
		handle->fd = fd;
		handle->size = size;

		return handle;
	}

	static size_t write(mmap_handle_t *handle, void *data, size_t offset, size_t len)
	{
		memcpy(handle->map_addr + offset, data, len);
		return len;
	}

	static void close(mmap_handle_t *handle)
	{
		munmap(handle->fd, handle->size);
		close(handle->fd);
		delete handle;
	}
}


typedef file_handler_traits<int> normal_file_handler_type;
typedef file_handler_traits<mmap_handle> mmap_file_handler_type;

































