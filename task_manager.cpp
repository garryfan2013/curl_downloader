#include "task_manager.h"

using namespace std;

task_manager::task_manager()
{
	
}

task_manager::~task_manager()
{
	thread_list_.clear();
}

int task_manager::init()
{
	return 0;
}

void task_manager::destroy()
{
	thread_list_.clear();
}

int task_manager::start_task(task_type &task)
{
	pthread_t tid;
	pthread_create(&tid, NULL, task_manager::_handle_task, &task);
	thread_list_.push_back(tid);
	return 0;
}

void task_manager::wait_all_task_done()
{
	for (list<pthread_t>::iterator iter = thread_list_.begin(); 
							iter != thread_list_.end(); iter++) {
		pthread_join((*iter), NULL);
	}
}

void* task_manager::_handle_task(void *arg)
{
	task_type *task = static_cast<task_type *>(arg);
	(*(task->handler))(task->param);
	return NULL;
}

