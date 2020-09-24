#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "barrier.h"

int global_loop=0;

void barrier_init(struct barrier_t * barrier, int nthreads)
{
	pthread_mutex_init(&barrier->m_lock,NULL);
	pthread_cond_init(&barrier->condition_variable,NULL);
	barrier->thread_count_initial=nthreads;
	barrier->count=nthreads;

	return;
}

void barrier_wait(struct barrier_t * barrier)
{
	pthread_mutex_lock(&barrier->m_lock);

	barrier->count-=1;

	if(barrier->count!=0)
	{
		pthread_cond_wait(&barrier->condition_variable,&barrier->m_lock);

	}
	else
	{
		pthread_cond_broadcast(&barrier->condition_variable);
		barrier->count=barrier->thread_count_initial;

	}

	pthread_mutex_unlock(&barrier->m_lock);
	return;
}