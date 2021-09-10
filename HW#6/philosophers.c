#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <wait.h>

#define COUNTING_NUMBER 3

// semaphore for chopsticks
sem_t chopstick[6];

void *philosopher(void *n){
	int i;
	// philosopher id
	int id = *((int *)n);
	for(i = 0; i < COUNTING_NUMBER; i++){
		// if philosopher's id is even number, pick up the left chopstick first, then the right chopstick
		if(id % 2 == 0){
			sem_wait(&chopstick[id]);
			sem_wait(&chopstick[(id+1)%6]);
		}
		else{
			// if philosopher's id is odd number, pick up the right chopstick first, then the left chopstick
			sem_wait(&chopstick[(id+1)%6]);
			sem_wait(&chopstick[id]);
		}
		// print the id of the philosopher who is eating
		printf("philosopher[%d] is eating\n", id);
		// put down right chopstick and left chopstick
		sem_post(&chopstick[(id+1)%6]);
		sem_post(&chopstick[id]);
		// print the id of the philosopher who is thinking
		printf("philosopher[%d] is thinking\n\n", id);
	}
}



int main(void){
	int i;

	// thread for each philosopher
	pthread_t philosopherID[6];
	
	// philosopher id
	int argument[6] = {0, 1, 2, 3, 4, 5};
	
	// init semaphore for chopstick
	for(i = 0; i < 6; i++){
		sem_init(&chopstick[i], 0, 1);
	}
	
	// create thread
	for(i = 0; i < 6; i++){
		pthread_create(&(philosopherID[i]), NULL, (void *)&philosopher, (void *)&argument[i]);
	}
	
	// wait thread
	for(i = 0; i < 6; i++){
		pthread_join(philosopherID[i], NULL);
	}
	
	// destroy semaphore
	for(i = 0; i < 6; i++){
		sem_destroy(&chopstick[i]);
	}	
	return 0;
}

