#include <pthread.h>

struct barrier_t
{
	/*
		Todo
		Barrier related variables
	*/
	int thread_count_initial;
	int count;
	pthread_mutex_t m_lock;
	pthread_cond_t condition_variable;
	
};

void barrier_init(struct barrier_t *b, int i);
void barrier_wait(struct barrier_t *b);