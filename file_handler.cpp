#include "file_handler.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

file_handler::file_handler():
    fd(-1)
{
}

file_handler::~file_handler()
{
    close();
}

int file_handler::open(const char *local_path)
{
    fd = ::open(local_path, O_RDWR | O_CREAT, 0755);
    if (fd < 0) {
        return -1;
    }

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
