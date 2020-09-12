#include <pthread.h>

struct barrier_t
{
	/*
		Todo
		Barrier related variables
	*/
	int thread_count;
	
};

void barrier_init(struct barrier_t *b, int i);
void barrier_wait(struct barrier_t *b);