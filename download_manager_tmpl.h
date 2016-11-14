/* 
 * download_manager_tmpl.h
 * Class template ,manages the whole download procedure 
 * Cao liang<caoliang@gmail.com>
 */ 

#ifndef _DOWNLOAD_MANAGER_TMPL_H
#define _DOWNLOAD_MANAGER_TMPL_H

#include "http_curl_downloader_tmpl.h"
#include <memory>
#include <string>
#include <vector>
#include <thread>

// T : Data handler
// U : Downloader
template<typename T, typename U = http_curl_downloader_tmpl<T> >
class download_manager_tmpl
{
public:
    download_manager_tmpl(const char *remote_url, 
                            const char *path, 
                            size_t count);
    ~download_manager_tmpl();

    void set_remote_url(const char *url);
    void set_local_path(const char *path);
    void set_thread_count(int c);

    int init();
    int start();
    int download_file_block(size_t offset, size_t size);
    void wait_all_tasks_done();
    int destroy();
		
private:
    typedef T handler_type;
    typedef U downloader_type;
    typedef std::unique_ptr<std::thread> thread_ptr;
	
    std::string remote_url_;
    std::string local_path_;
    size_t file_size_;
    handler_type file_handler_;
    downloader_type downloader_;

    std::vector<thread_ptr> threads_;
    size_t thread_count_;
};

#include "download_manager_tmpl.inl"

#endif /* _DOWNLOAD_MANAGER_TMPL_H */
