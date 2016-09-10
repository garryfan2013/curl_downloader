
#include <thread_pool.h>
#include <functional>

thread_pool::thread_pool(int size)
{
	_pool_size = size;
	_free_count = size;
	_status = THREAD_POOL_STOPPED;
}

thread_pool::~thread_pool()
{
	std::list<thread_worker *>::iterator iter = _worker_list->begin();

	while(iter != _worker_list->end()) {
		thread_worker *w = *iter;
		if (NULL != w) {
			delete(w);
		}

		iter++;
	}

	if (NULL != _worker_list) {
		delete(_worker_list);
		_worker_list = NULL;
	}

	if (NULL != _threads) {
		free(_threads);
		_threads = NULL;
	}
}

int thread_pool::init()
{
	if (_pool_size <= 0) {
		printf("Wrong pool size!\n");
		return -1;		
	}

	_worker_list = new std::list<thread_worker *>();
	_threads = (pthread_t *)malloc(sizeof(pthread_t)*_pool_size);
	
	pthread_mutex_init(&_lock, NULL);
	pthread_cond_init(&_cond, NULL);

	for(int i=0; i<_pool_size; i++) {
		pthread_create(&(_threads[i]), NULL, 
			std::bind(&thread_pool::_thread_process_routine, this, std::placeholders::_1), NULL);
	}

	_status = THREAD_POOL_RUNNING;
	
	return 0;
}

void thread_pool::destroy()
{
	_status = THREAD_POOL_STOPPED;
}

int thread_pool::add_worker(worker_function_t func, void *arg) 
{
	thread_worker *w = new thread_worker(func, arg);

	pthread_mutex_lock(&_lock);
	_worker_list->push_back(w);
	pthrad_mutex_unlock(&_lock);
	
	return 0;
}

void* thread_pool::_thread_process_routine(void *arg)
{
	while(true) {
		pthread_mutex_lock(&_lock);
		
		while(_worker_list.size() == 0) {
			pthread_cond_wait(&_cond, &_lock);
		}

		if ( THREAD_POOL_STOPPED == _status ) {
			pthread_mutex_unlock(&_lock);
			pthread_exit(NULL);
		}

		thread_worker *worker = _worker_list.pup_front();
		(*(worker->func))(worker->arg);

		delete(worker);
		worker = NULL;
	}

	pthread_exit(NULL);
}

