#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
/* 
 * This is the counter value which is to
 * be incremented by all the threads created 
 * by your program
*/
int counter = 0;
pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;

void *count_incrementor(void* id){
	long long thread_id=(long long int) id;
	//sleep(1);
	pthread_mutex_lock(&m_lock);
	//printf("T:%lld \t",thread_id);
	fflush(stdout);
	counter++;
	pthread_mutex_unlock(&m_lock);

	pthread_exit((void *)0);
	
}

int main(int argc, char const *argv[])
{
	/* Write you code to create n threads here*/
	/* Each thread must increment the counter once and exit*/

	long long int NUMBER_OF_THREADS;
	int thread_create_status;
	void* status;

	if(argc >1)
	{
		NUMBER_OF_THREADS=atoi(argv[1]);
		//printf("NUMBER_OF_THREADS: %lld\n",NUMBER_OF_THREADS);
		if(NUMBER_OF_THREADS==0)
			return 0;
	}
	else
	{
		printf("Please enter the thread count in argument.\n");
			return 0;
	}

	pthread_t threads[NUMBER_OF_THREADS];

	for(long long int i=0;i<NUMBER_OF_THREADS;i++)
	{
		thread_create_status=pthread_create(&threads[i],NULL, count_incrementor,(void *) i);
		if(thread_create_status){
			printf("Error in creating thread: %lld",i);
			exit(-1);
		}
	}

	for(long long int i=0;i<NUMBER_OF_THREADS;i++)
	{
		pthread_join(threads[i], &status);
	}

	pthread_mutex_destroy(&m_lock);
	
	/* Make sure you reap all threads created by your program
	 before printing the counter*/
	printf("%d\n", counter);
	//sleep(10000);
	return 0;
}