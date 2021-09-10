#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <wait.h>

#define COUNTING_NUMBER 10

// S is semaphore about readcount, wrt is about cur_writer and cur_count
sem_t S;
sem_t wrt;
int cur_writer, cur_count = 0;
int readcount = 0;

void *writer(void *n){
	int i;
	// count is local variable in this thread
	// count is number of times this thread has accessed a critical section 
	int count = 0;
	int number = *((int *)n);
	for(i = 0; i < COUNTING_NUMBER; i++){
		count++;
		usleep(100000);
		// wait if other writer or reader acesses a critical section
		sem_wait(&wrt);
		// cur_writer is this writer's id
		cur_writer = number;
		// cur_count is this writer's count
		cur_count = count;
		//print about this writer's id and count
		printf("\twriter[%d] writes count %d\n", cur_writer, cur_count);
	
		// increase wrt's value about 1
		sem_post(&wrt);
	}
}

void reader(){
	int i;

	for(i = 0; i < COUNTING_NUMBER; i++){
		usleep(30000);
		// readcount access protection
		sem_wait(&S);
		readcount++;
		// when the first reader comes in, it waits if there is already a writer
		if(readcount == 1)
			sem_wait(&wrt);
		// after increase readcount, release access protection about readcount
		sem_post(&S);
		
		// print the writer id that recently accessed cur_count
		printf("The most recent writer id:\t[%d], count:\t[%d]\n", cur_writer, cur_count);
		// readcount access protection
		sem_wait(&S);
		readcount--;
		// if it is the last reader to go out, it signals the writer that it is okay to come in 
		if(readcount == 0)
			sem_post(&wrt);
		// after decrease readcount, release access proctection about readcount
		sem_post(&S);

	}
}

int main(void){
	int i;

	// thread about reader and writer
	pthread_t readerID[2];
	pthread_t writerID[5];

	// writer's id
	int argument[5] = {0, 1, 2, 3, 4};
	
	// init semaphore
	sem_init(&S, 0, 1);
	sem_init(&wrt, 0, 1);

	// create thread about writer
	for(i = 0; i < 5; i++){
		pthread_create(&(writerID[i]), NULL, (void *)&writer, (void *)&argument[i]);
	}
	// create thread about reader
	for(i = 0; i < 2; i++){
		pthread_create(&(readerID[i]), NULL, (void *)&reader, NULL);
	}

	// wait reader thread
	for(i = 0; i < 2; i++){
		pthread_join(readerID[i], NULL);
	}
	// wait writer thread
	for(i = 0; i < 5; i++){
		pthread_join(writerID[i], NULL);
	}
	
	// destroy semaphore
	sem_destroy(&S);
	sem_destroy(&wrt);
	
	return 0;
}

