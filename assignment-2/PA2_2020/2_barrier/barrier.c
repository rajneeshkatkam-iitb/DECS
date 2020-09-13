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
	//printf("Barrier thread_count initialised with: %d \n",barrier->thread_count);
	//sleep(2);
	return;
}

void barrier_wait(struct barrier_t * barrier)
{
	pthread_mutex_lock(&barrier->m_lock);

	//printf("Barrier value: %p \n", &barrier);

	barrier->count-=1;

	if(barrier->count!=0)
	{
		// printf("I'm thread number: %d\n",global_thread_count-barrier->thread_count);
		// fflush(stdout);

		pthread_cond_wait(&barrier->condition_variable,&barrier->m_lock);

		//Only for debugging
		// sleep(2);
		//printf("Thread wakeup \n");
		//fflush(stdout);
	}
	else
	{
		pthread_cond_broadcast(&barrier->condition_variable);
		barrier->count=barrier->thread_count_initial;
		// printf("I'm the last thread \n");
		// fflush(stdout);
	}

	//Only for debugging

	// printf("Unlocked global_loop:%d \n",++global_loop);
	// fflush(stdout);
	pthread_mutex_unlock(&barrier->m_lock);
	//sleep(1);
	return;
}