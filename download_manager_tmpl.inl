#include <iostream>
#include <unistd.h>
#include <stdio.h>

using namespace std;

template <typename T, typename U>
download_manager_tmpl<T, U>::download_manager_tmpl(
    const char *remote_url, const char *local_path, size_t count):
    remote_url_(remote_url), local_path_(local_path), thread_count_(count)
{
}

template <typename T, typename U>
download_manager_tmpl<T, U>::~download_manager_tmpl()
{
}

template <typename T, typename U>
inline void download_manager_tmpl<T, U>::set_remote_url(const char *url)
{
    remote_url_ = url;
}

template <typename T, typename U>
inline void download_manager_tmpl<T, U>::set_local_path(const char *path)
{
    local_path_ = path;
}

template <typename T, typename U>
inline void download_manager_tmpl<T, U>::set_thread_count(int count)
{
    thread_count_ = count;
}

template <typename T, typename U>
int download_manager_tmpl<T, U>::init()
{
    if (downloader_.init() < 0) {
        cout << "downloader init failed" << endl;
        return -1;
    }

    file_size_ = downloader_.get_file_size(remote_url_.c_str());
    if (file_size_ <= 0) {
        cout << "download file size not correct" << endl;
        downloader_.destroy();
        return -1;
    }

    //if file already exists , add an index to the local file.
    int index = 1;
    while (0 == access(local_path_.c_str(), F_OK)) {
        char str[32];
        snprintf(str, 32, "(%d)", index++);

        int pos = local_path_.find_last_of('.');
        if (pos == string::npos) {
            local_path_.append(str);
        } else {
            local_path_.insert(pos, str);
        }
    }

    if (file_handler_.open(local_path_.c_str()) < 0) {
        cout << "file handler open failed: " 
                << local_path_.c_str() << endl;
        downloader_.destroy();
        return -1;
    }

    return 0;
}

template <typename T, typename U>
int download_manager_tmpl<T, U>::download_file_block(size_t offset, size_t size)
{
    return downloader_.download(remote_url_.c_str(), offset, size, file_handler_);
}

template <typename T, typename U>
int download_manager_tmpl<T, U>::start()
{
    size_t block_size = file_size_/thread_count_;
    size_t last_block_size = block_size + file_size_%thread_count_;

    for (int i = 0; i < thread_count_; i++) {
        size_t offset = i*block_size;
        size_t size;
        if (i == (thread_count_ - 1)) {
            size = last_block_size;
        } else {
            size = block_size;
        }

        thread_ptr tp(new thread(&download_manager_tmpl<T, U>::download_file_block, 
                                    this, offset, size));
        threads_.push_back(move(tp));
    }

    return 0;
}

template <typename T, typename U>
void download_manager_tmpl<T, U>::wait_all_tasks_done()
{
    for (vector<thread_ptr>::iterator iter = threads_.begin();
                                iter != threads_.end(); ) {
        if ((*iter)->joinable()) {
            (*iter)->join();
        }
        iter = threads_.erase(iter);
    }
}

template <typename T, typename U>
int download_manager_tmpl<T, U>::destroy()
{
    downloader_.destroy();
    file_handler_.close();
    return 0;
}

