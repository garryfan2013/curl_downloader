#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <list>
#include <functional>

typedef std::function<int (void *)> worker_function_t;

typedef enum {
	THREAD_POOL_STOPPED = -1,
	THREAD_POOL_INIT,
	THREAD_POOL_RUNNING
}thread_pool_status_t;

class thread_worker {

public:
	thread_worker(worker_function_t func, void *arg) {
		_func = func; 
		_arg = arg;  
	};

	virtual ~thread_worker();

	worker_function_t function() { return _func; };

	void* arg() { return _arg; };

private:
	worker_function_t _func;
	void * _arg;
};

class thread_pool {
	
public:
	thread_pool(const int size);
	virtual ~thread_pool();

	int init();
	int add_worker(worker_function_t func, void *arg);
	void destroy();

private:
	void *_process_function();
	static void *_wrapper_process_function(void *arg);
		
	std::list<thread_worker *> *_worker_list;

	int _pool_size;
	int _free_count;
	thread_pool_status_t _status;

	pthread_t *_threads;
	pthread_mutex_t _lock;
	pthread_cond_t _cond;
};

#endif  /* _THREAD_POOL_H */
