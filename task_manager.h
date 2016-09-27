/* 
 * task_manager.h
 * Simple manager for threaded tasks
 * Cao liang<caoliang@gmail.com>
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

	typedef int (*task_handler_type)(void *);
	typedef void *task_param_type;

	typedef struct 
	{
		task_handler_type handler;
		task_param_type param;
	}task_type;

	int init();
	int start_task(task_type &t);
	void wait_all_task_done();
	void destroy();

private:
	static void *_handle_task(void *arg);
		
	std::list<pthread_t> thread_list_;
};

#endif  /* _TASK_MANAGER_H */
