#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <list>

typedef int (*worker_function_t)(void *arg);

typedef enum {
	THREAD_POOL_STOPPED = -1,
	THREAD_POOL_INIT,
	THREAD_POOL_RUNNING
}thread_pool_status_t;

class thread_worker {

public:
	thread_worker(worker_function_t func, const void *arg);
	virtual ~thread_worker();

private:
	worker_function_t _function;
	void * _arg;
};

class thread_pool {
	
public:
	thread_pool(const int size);
	virtual ~thread_pool();

	int init();
	int add_worker(worker_function_t func, const void *arg);
	void destroy();

private:	
	void _thread_process_routine();
		
	std::list<thread_worker> *_worker_list;

	int _pool_size;
	int _free_count;
	thread_pool_status_t _status;

	pthread_t *_threads;
	pthread_mutex_t _lock;
	pthread_cond_t _cond;
};

#endif  /* _THREAD_POOL_H */
