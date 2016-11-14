/* 
 * download_manager.h
 * download_manager typedef
 * Cao liang<caoliang@gmail.com>
 */ 

#ifndef _DOWNLOAD_MANAGER_H
#define _DOWNLOAD_MANAGER_H

#include "download_manager_tmpl.h"
#include "file_handler.h"

typedef download_manager_tmpl<file_handler> download_manager;

#endif /* _DOWNLOAD_MANAGER_H */
