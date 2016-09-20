/* 
 * task_manager.h
 * description: simple manager for the threaded tasks
 * Author: xxx<xxx@gmail.com>
 */ 

#ifndef _TASK_MANAGER_H
#define _TASK_MANAGER_H

#include <pthread.h>
#include <list>

class task_manager
{
public:
	task_manager();
	virtual ~task_manager();

	typedef int (*task_handler_t)(void *);
	typedef void *task_param_t;

	typedef struct 
	{
		task_handler_t handler;
		task_param_t param;
	}task_t;

	int init();
	int start_task(task_t &t);
	void wait_all_task_done();
	void destroy();

private:
	static void *_handle_task(void *arg);
		
	std::list<pthread_t> thread_list_;
};

#endif  /* _TASK_MANAGER_H */
