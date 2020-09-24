#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#define NUMBER_OF_THREADS 10

/* 
 * This is the data array which is to
 * be incremented by all the threads created 
 * by your program as specified in the problem state
*/
int data[10];
pthread_mutex_t m_lock[NUMBER_OF_THREADS];

void *array_data_incrementor(void* id)
{
	long int thread_id = (long int) id;
	for(long int i=0;i<1000;)
	{
		pthread_mutex_lock(&m_lock[thread_id]);
		data[thread_id]+=1;
		i++;
		pthread_mutex_unlock(&m_lock[thread_id]);
	}

	pthread_exit((void *)0);

}


int main(int argc, char const *argv[])
{
	/* Write you code to create 10 threads here*/
	/* Increment the data array as specified in the problem statement*/
	pthread_t threads[NUMBER_OF_THREADS];
	int pthread_create_error;
	void *status;

	for(int i=0;i<NUMBER_OF_THREADS;i++)
	{
		pthread_mutex_init(&m_lock[i], NULL);
	}


	for(long int i=0;i<NUMBER_OF_THREADS;i++)
	{
		pthread_create_error = pthread_create(&threads[i],NULL,array_data_incrementor, (void *) i);
		if(pthread_create_error)
		{
			printf("Error in creating thread: %ld",i);
			exit(-1);
		}
	}

	for(long int i=0;i<1000;i++)
	{
		for(long int j=0;j<NUMBER_OF_THREADS;j++)
		{
			pthread_mutex_lock(&m_lock[j]);
			data[j]+=1;
			pthread_mutex_unlock(&m_lock[j]);
		}
	}


	for(long int i=0;i<NUMBER_OF_THREADS;i++)
	{
		pthread_join(threads[i], &status);
	}

	/* Make sure you reap all threads created by your program
	 before printing the counter*/

	for(long int i=0; i<NUMBER_OF_THREADS;i++)
	{
		pthread_mutex_destroy(&m_lock[i]);
	}

	for (int i = 0; i < 10; ++i)
	{
		printf("%d\n", data[i]);
	}
	
	//sleep(10000);
	return 0;
}