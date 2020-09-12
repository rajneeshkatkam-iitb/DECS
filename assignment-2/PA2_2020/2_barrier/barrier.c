#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "barrier.h"


pthread_mutex_t m_lock;
pthread_cond_t condition_variable;
int global_thread_count;

int global_loop=0;

void barrier_init(struct barrier_t * barrier, int nthreads)
{
	pthread_mutex_init(&m_lock,NULL);
	pthread_cond_init(&condition_variable,NULL);
	barrier->thread_count=nthreads;
	global_thread_count=nthreads;
	//printf("Barrier thread_count initialised with: %d \n",barrier->thread_count);
	//sleep(2);
	return;
}

void barrier_wait(struct barrier_t * barrier)
{
	pthread_mutex_lock(&m_lock);

	//printf("Barrier value: %p \n", &barrier);

	barrier->thread_count-=1;

	if(barrier->thread_count!=0)
	{
		// printf("I'm thread number: %d\n",global_thread_count-barrier->thread_count);
		// fflush(stdout);
		if(barrier->thread_count!=0)
		{
			pthread_cond_wait(&condition_variable,&m_lock);
		}

		//Only for debugging
		// sleep(2);
		//printf("Thread wakeup \n");
		//fflush(stdout);
	}
	else
	{
		pthread_cond_broadcast(&condition_variable);
		barrier->thread_count=global_thread_count;
		// printf("I'm the last thread \n");
		// fflush(stdout);
	}

	//Only for debugging

	// printf("Unlocked global_loop:%d \n",++global_loop);
	// fflush(stdout);
	pthread_mutex_unlock(&m_lock);
	sleep(1);
	return;
}