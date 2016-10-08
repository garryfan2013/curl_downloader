main.o main.d : main.cpp download_manager.h download_manager_tmpl.h \
 http_curl_downloader_tmpl.h http_curl_downloader_tmpl.inl curl/curl.h \
 curl/curlver.h curl/curlbuild.h curl/curlrules.h curl/easy.h \
 curl/multi.h curl/curl.h download_manager_tmpl.inl file_handler.h
