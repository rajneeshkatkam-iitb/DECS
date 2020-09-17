#include "cs_thread.h"

struct Repositioning {
	char player; 		// T for turtle and H for hare
	int time; 		// At what time god interrupt's
	int distance;		// How much does god move any of the player. 
							// distance can be negetive or positive.
							// using this distance if any of players position is less than zero then bring him to start line.
							// If more than finish_distance then make him win.
							// If time is after completion of game than you can ignore it as we will already have winner.
};

struct race {
	
	//	Don't change these variables.
	//	speeds are unit distance per unit time.
	int printing_delay;
	int tortoise_speed;					// speed of Turtle
	int hare_speed;						// speed of hare
	int hare_sleep_time; 				// how much time does hare sleep (in case he decides to sleep)
	int hare_turtle_distance_for_sleep; // minimum lead which hare has on turtle after which hare decides to move
										// Any lead greater than this distance and hare will ignore turtle and go to sleep
	int finish_distance;				// Distance between start and finish line
	struct Repositioning* reposition;	// pointer to array containing Randomizer's decision
	int repositioning_count;			// number of elements in array of repositioning structure
	
	//	Add your custom variables here.

	
	
};


struct RandomizerRepositionData{
	struct Repositioning* reposition_array;
	int repositioning_count;
};

// void* Turtle(void *race);
// void* Hare(void *race);
// void* Randomizer(void *race);
// void* Report(void *race);
// void* clock_tick_function();


long long int clock_tick=-1;
pthread_t turtle_thread, hare_thread, randomizer_thread, reporter_thread;
int no_winner=1; // no_winner =0 -- We got the winner

struct lock reporter_lock, turtle_reporter_lock, hare_reporter_lock, randomizer_reporter_lock;
struct condition turtle_to_reporter_cv, reporter_to_turtle_cv,  hare_to_reporter_cv, reporter_to_hare_cv;
struct condition randomizer_to_reporter_cv, reporter_to_randomizer_cv;
int previous_reporter=0,new_reporter=0;

// Finish Distance set to global for everyone's access
int finish_distance;


struct lock randomizer_main_lock;
struct condition randomizer_main_cv;
int randomizer_thread_reached=0;

struct lock turtle_main_lock, hare_main_lock;
struct condition turtle_main_cv, hare_main_cv;
int turtle_thread_reached=0, hare_thread_reached=0;


long int turtle_distance=0;
struct lock turtle_distance_lock;
char overall_winner='-';

long int hare_distance=0, hare_sleep_counter;
struct lock hare_distance_lock;




// // Turtle Thread
// long int long_tortoise_speed=race->tortoise_speed;
void* Turtle(void *arg)
{
	long int long_tortoise_speed=(long int) arg;
	int curr_time;
	while(no_winner){

		// This if condition will satisfy only once at the beginning
		if(turtle_thread_reached==0)
		{
			lock_acquire(&turtle_main_lock);
			turtle_thread_reached=1;
			printf("Turtle Intialise Position Reached \n");
			lock_acquire(&turtle_reporter_lock); // Holding the turtle_reporter_lock before reporter
			cond_signal(&turtle_main_cv,&turtle_main_lock);
			lock_release(&turtle_main_lock);
		}
		else
		{
			lock_acquire(&turtle_reporter_lock);
		}
		
		cond_wait(&reporter_to_turtle_cv,&turtle_reporter_lock);
		lock_acquire(&reporter_lock);
		curr_time=clock_tick;
		lock_release(&reporter_lock);

		//Perform Action of Turtle if no Winner
		if(no_winner)
		{
			lock_acquire(&turtle_distance_lock);
			turtle_distance+=long_tortoise_speed;
			printf("Turtle curr_time : %14d \t turtle_distance: %ld\n",curr_time, turtle_distance);
			
			if(turtle_distance>=finish_distance)
			{
				overall_winner='T';
				turtle_distance=finish_distance;
				no_winner=0;
			}
			lock_release(&turtle_distance_lock);
		}
		else if(no_winner==0 && overall_winner=='H' && hare_distance==turtle_distance)
		{
			printf("Both reached the final distance at same time. Hence Turtle is declared winner!!\n");
			overall_winner='T';
		}
		


		cond_signal(&turtle_to_reporter_cv,&turtle_reporter_lock);
		lock_release(&turtle_reporter_lock);
	}
	printf("Turtle: curr_time: %d \t turtle_distance: %ld \t overall_winner: %c\n",curr_time,turtle_distance,overall_winner);
	printf("Turtle thread exited..\n");
	pthread_exit((void*)0);
}


// // Hare Thread
// 	long int *long_hare_array=(long int*)malloc(3*sizeof(long int));
// 	long_hare_array[0]=race->hare_speed;
// 	long_hare_array[1]=race->hare_sleep_time;
// 	long_hare_array[2]=race->hare_turtle_distance_for_sleep;
void* Hare(void *arg)
{
	long int *temp_long_hare_array=(long int *)arg;
	long int long_hare_speed=temp_long_hare_array[0];
	long int long_hare_sleep_time=temp_long_hare_array[1];
	long int long_hare_turtle_distance_for_sleep=temp_long_hare_array[2];
	int curr_time;
	hare_sleep_counter=long_hare_sleep_time; // Intialised to max in order to satisfy condition initially

	while(no_winner){

		// This if condition will satisfy only once at the beginning
		if(hare_thread_reached==0)
		{
			lock_acquire(&hare_main_lock);
			hare_thread_reached=1;
			printf("Hare Intialise Position Reached \n");
			lock_acquire(&hare_reporter_lock); // Holding the hare_reporter_lock before reporter
			cond_signal(&hare_main_cv,&hare_main_lock);
			lock_release(&hare_main_lock);
		}
		else
		{
			lock_acquire(&hare_reporter_lock);
		}

		cond_wait(&reporter_to_hare_cv,&hare_reporter_lock);
		lock_acquire(&reporter_lock);
		curr_time=clock_tick;
		lock_release(&reporter_lock);

		//Perform Action of Hare if no Winner
		if(no_winner)
		{
			if(hare_sleep_counter==long_hare_sleep_time)
			{
				lock_acquire(&turtle_distance_lock);
				if((hare_distance-turtle_distance)<=long_hare_turtle_distance_for_sleep)
				{
					// Hare will work
					lock_acquire(&hare_distance_lock);
					hare_distance+=long_hare_speed;
					printf("Hare Worked: curr_time : %8d \t hare_sleep_counter: %ld \t hare_distance: %ld\n",curr_time, hare_sleep_counter, hare_distance);
					if(hare_distance>=finish_distance)
					{
						overall_winner='H';
						hare_distance=finish_distance;
						no_winner=0;
					}
					lock_release(&hare_distance_lock);
				}
				// Hare will sleep
				hare_sleep_counter=0;
				printf("Hare Sleept: curr_time : %8d \t hare_sleep_counter: %ld \t hare_distance: %ld\n",curr_time, hare_sleep_counter, hare_distance);
				//printf("Hare action performed : %10d \t long_hare_speed: %ld \t long_hare_sleep_time: %ld \t long_hare_turtle_distance_for_sleep: %ld\n",curr_time, long_hare_speed, long_hare_sleep_time, long_hare_turtle_distance_for_sleep);
				lock_release(&turtle_distance_lock);
			}
			else
			{
				hare_sleep_counter+=1;
				printf("Hare Sleeping: curr_time : %8d \t hare_sleep_counter: %ld \t hare_distance: %ld\n",curr_time, hare_sleep_counter, hare_distance);
			}

		}

		cond_signal(&hare_to_reporter_cv,&hare_reporter_lock);
		lock_release(&hare_reporter_lock);

	}
	printf("Hare: curr_time: %d \t hare_distance: %ld \t overall_winner: %c\n",curr_time,hare_distance,overall_winner);
	printf("Hare thread exited..\n");
	pthread_exit((void*)0);
}


// struct Repositioning {
// 	char player; 		// T for turtle and H for hare
// 	int time; 		// At what time god interrupt's
// 	int distance;

// // Randomizer Thread
// 	struct RandomizerRepositionData *repo_data= (struct RandomizerRepositionData*) malloc(sizeof(struct RandomizerRepositionData));
// 	repo_data->reposition_array=race->reposition;
// 	repo_data->repositioning_count=race->repositioning_count;
void* Randomizer(void *arg)
{	
	struct RandomizerRepositionData *temp_repo_data=(struct RandomizerRepositionData *)arg;
	struct Repositioning *temp_reposition_array=temp_repo_data->reposition_array;
	int temp_repositioning_count=temp_repo_data->repositioning_count;
	int array_index=0;  // This is used to read the values of the time from the array.


	printf("Reposition Array Data:\n");
	for(int i=0;i<temp_repositioning_count;i++)
	{
		printf("player: %c \t time: %d \t distance: %d \n",temp_reposition_array[i].player, temp_reposition_array[i].time, temp_reposition_array[i].distance);
	}


	int curr_time;
	while(no_winner){

		// This if condition will satisfy only once at the beginning
		if(randomizer_thread_reached==0)
		{
			lock_acquire(&randomizer_main_lock);
			randomizer_thread_reached=1;
			printf("Randomizer Intialise Position Reached \n");
			lock_acquire(&randomizer_reporter_lock); // Holding the randomizer_reporter_lock before reporter
			cond_signal(&randomizer_main_cv,&randomizer_main_lock);
			lock_release(&randomizer_main_lock);
		}
		else
		{
			lock_acquire(&randomizer_reporter_lock);
		}
		
		cond_wait(&reporter_to_randomizer_cv,&randomizer_reporter_lock);
		lock_acquire(&reporter_lock);
		curr_time=clock_tick;
		lock_release(&reporter_lock);

		//Perform Action of Randomizer if no Winner
		if(no_winner)
		{
			if(array_index<temp_repositioning_count && temp_reposition_array[array_index].time==curr_time)
			{

				printf("\n\n\n\n\n \t \t \t Randomizer Invoked at curr_time: %5d \n",curr_time);

				if(temp_reposition_array[array_index].player=='H')
				{
					hare_distance+=temp_reposition_array[array_index].distance;

					if(hare_distance<0)
						hare_distance=0;
					else if (hare_distance>=finish_distance)
					{
						overall_winner='H';
						hare_distance=finish_distance;
						no_winner=0;
					}

					printf("\t \t \t Hare repositoned to: %ld \t with shifting distance: %d \n\n\n\n", hare_distance, temp_reposition_array[array_index].distance);

				}
				else if (temp_reposition_array[array_index].player=='T')
				{
					turtle_distance+=temp_reposition_array[array_index].distance;

					if(turtle_distance<0)
						turtle_distance=0;
					else if (turtle_distance>=finish_distance)
					{
						overall_winner='T';
						turtle_distance=finish_distance;
						no_winner=0;
					}

					printf("\t \t \t Turtle repositoned to: %ld \t with shifting distance: %d \n\n\n\n", turtle_distance, temp_reposition_array[array_index].distance);

				}


				array_index++;
			}
		}


		cond_signal(&randomizer_to_reporter_cv,&randomizer_reporter_lock);
		lock_release(&randomizer_reporter_lock);



	}	
	printf("Randomizer: curr_time: %d \n",curr_time);
	printf("Randomizer thread exited..\n");
	pthread_exit((void*)0);
}



// // Reporter Thread
// long int long_printing_delay=race->printing_delay;
void* Report(void *arg)
{
	long int printing_delay=(long int)arg;
	while(no_winner){

		//Performing Time Incrementing
		lock_acquire(&reporter_lock);
		clock_tick+=1;
		lock_release(&reporter_lock);


		// Randomizer will perform its tasks first
		lock_acquire(&randomizer_reporter_lock);
		cond_signal(&reporter_to_randomizer_cv,&randomizer_reporter_lock);
		cond_wait(&randomizer_to_reporter_cv,&randomizer_reporter_lock);
		lock_release(&randomizer_reporter_lock);

		//Reporting at the specified printing delay
		if(clock_tick%printing_delay==0)
			printf("\n\n\n\n \t \t \t Hare Position: %ld \t Turtle Position: %ld \t at Time: %3lld \n\n\n\n", hare_distance, turtle_distance, clock_tick);


		// Turtle will perform for 1 unit
		lock_acquire(&turtle_reporter_lock);
		cond_signal(&reporter_to_turtle_cv,&turtle_reporter_lock);
		cond_wait(&turtle_to_reporter_cv,&turtle_reporter_lock);
		lock_release(&turtle_reporter_lock);



		// Hare will perform for 1 unit
		lock_acquire(&hare_reporter_lock);
		cond_signal(&reporter_to_hare_cv,&hare_reporter_lock);
		cond_wait(&hare_to_reporter_cv,&hare_reporter_lock);
		lock_release(&hare_reporter_lock);

		// If we got the winner (i.e. no_winner=0). Stop the other threads
		if(no_winner==0)
		{
			lock_acquire(&randomizer_reporter_lock);
			cond_signal(&reporter_to_randomizer_cv,&randomizer_reporter_lock);
			lock_release(&randomizer_reporter_lock);

			lock_acquire(&turtle_reporter_lock);
			cond_signal(&reporter_to_turtle_cv,&turtle_reporter_lock);
			lock_release(&turtle_reporter_lock);

			lock_acquire(&hare_reporter_lock);
			cond_signal(&reporter_to_hare_cv,&hare_reporter_lock);
			lock_release(&hare_reporter_lock);

		}

		usleep(100000);
		//usleep(500000);
		//sleep(1);
		printf("\n\n");

	}
	printf("Report: clock_tick: %lld \n",clock_tick);
	printf("Report thread exited..\n");
	pthread_exit((void*)0);
}







char init(struct race *race)
{
	
	// Race Final Distance
	finish_distance=race->finish_distance;
	printf("TOTAL DISTANCE OF RACE: %4d\n", finish_distance);

	// Locks used for threads
	lock_init(&reporter_lock);
	lock_init(&turtle_reporter_lock);
	lock_init(&hare_reporter_lock);
	lock_init(&randomizer_reporter_lock);
	lock_init(&hare_distance_lock);
	lock_init(&turtle_distance_lock);

	// CVs used inside for thread synchronization
	cond_init(&turtle_to_reporter_cv);
	cond_init(&reporter_to_turtle_cv);
	cond_init(&hare_to_reporter_cv);
	cond_init(&reporter_to_hare_cv);
	cond_init(&randomizer_to_reporter_cv);
	cond_init(&reporter_to_randomizer_cv);

	// Locks used for initial stage of main and thread
	lock_init(&randomizer_main_lock);
	lock_init(&turtle_main_lock);
	lock_init(&hare_main_lock);

	// CVs used for initial stage of main and thread synchronization
	cond_init(&randomizer_main_cv);
	cond_init(&turtle_main_cv);
	cond_init(&hare_main_cv);



	// struct race {
	
	// //	Don't change these variables.
	// //	speeds are unit distance per unit time.
	// int printing_delay;
	// int tortoise_speed;					// speed of Turtle
	// int hare_speed;						// speed of hare
	// int hare_sleep_time; 				// how much time does hare sleep (in case he decides to sleep)
	// int hare_turtle_distance_for_sleep; // minimum lead which hare has on turtle after which hare decides to move
	// 									// Any lead greater than this distance and hare will ignore turtle and go to sleep
	// int finish_distance;				// Distance between start and finish line
	// struct Repositioning* reposition;	// pointer to array containing Randomizer's decision
	// int repositioning_count;			// number of elements in array of repositioning structure
	
	// //	Add your custom variables here.

	// };


	// Randomizer Thread
	struct RandomizerRepositionData *repo_data= (struct RandomizerRepositionData*) malloc(sizeof(struct RandomizerRepositionData));
	repo_data->reposition_array=race->reposition;
	repo_data->repositioning_count=race->repositioning_count;
	int randomizer_thread_status=pthread_create(&randomizer_thread,NULL,Randomizer,(void*)repo_data);
	if(randomizer_thread_status)
	{
		printf("Error in creating randomizer_thread: %d",randomizer_thread_status);
		exit(-1);
	}

	// Waiting for Randomizer Thread to Reach to desired position
	lock_acquire(&randomizer_main_lock);
	if(randomizer_thread_reached==0)
		cond_wait(&randomizer_main_cv,&randomizer_main_lock);
	lock_release(&randomizer_main_lock);




	// Turtle Thread
	long int long_tortoise_speed=race->tortoise_speed;
	int turtle_thread_status=pthread_create(&turtle_thread,NULL,Turtle,(void*)long_tortoise_speed);
	if(turtle_thread_status)
	{
		printf("Error in creating turtle_thread: %d",turtle_thread_status);
		exit(-1);
	}

	// Waiting for Turtle Thread to Reach to desired position
	lock_acquire(&turtle_main_lock);
	if(turtle_thread_reached==0)
		cond_wait(&turtle_main_cv,&turtle_main_lock);
	lock_release(&turtle_main_lock);





	// Hare Thread
	long int *long_hare_array=(long int*)malloc(3*sizeof(long int));
	long_hare_array[0]=race->hare_speed;
	long_hare_array[1]=race->hare_sleep_time;
	long_hare_array[2]=race->hare_turtle_distance_for_sleep;
	int hare_thread_status=pthread_create(&hare_thread,NULL,Hare,(void*)long_hare_array);
	if(hare_thread_status)
	{
		printf("Error in creating hare_thread: %d",hare_thread_status);
		exit(-1);
	}

	// Waiting for Hare Thread to Reach to desired position
	lock_acquire(&hare_main_lock);
	if(hare_thread_reached==0)
		cond_wait(&hare_main_cv,&hare_main_lock);
	lock_release(&hare_main_lock);






	// Reporter Thread
	long int long_printing_delay=race->printing_delay;
	int reporter_thread_status=pthread_create(&reporter_thread,NULL,Report, (void*)long_printing_delay);
	if(reporter_thread_status)
	{
		printf("Error in creating reporter_thread: %d",reporter_thread_status);
		exit(-1);
	}

	// Joining threads to the main thread
	pthread_join(randomizer_thread, NULL);
	pthread_join(turtle_thread, NULL);
	pthread_join(hare_thread, NULL);
	pthread_join(reporter_thread, NULL);

	// Freeing of Heap allocated memory
	free(long_hare_array);
	free(repo_data);

	return overall_winner; 
}