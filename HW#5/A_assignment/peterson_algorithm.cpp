#include <atomic>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define COUNTING_NUMBER 2000000

using namespace std;

int critical_section_variable = 0;

//set atomic value
atomic_int turn(0);
atomic_int flag0(0), flag1(0);

//lock with atomic class function store()
//with atomic function, order of flag and turn will not change
void lock(int self){
	if(self == 0){
		flag0.store(1);
		turn.store(1);
		while(flag1 && turn == 1);
	}else{
		flag1.store(1);
		turn.store(0);
		while(flag0 && turn ==0);
	}
}

//unlock
void unlock(int self){
	if(self == 0) flag0 = 0;
	else flag1 = 0;
}

//after lock, increase critical value
//agter increase critical value, unlock
void* func(void *s){
	int* thread_num = (int *)s;
	int i;
	for(i = 0; i < COUNTING_NUMBER; i++){
		lock(*thread_num);
		critical_section_variable++;
		unlock(*thread_num);
	}
}

int main(void){
	pthread_t p1, p2;
	
	//thread id
	int parameter[2] = {0, 1};
	
	//create thread
	pthread_create(&p1, NULL, func, (void*)&parameter[0]);
	pthread_create(&p2, NULL, func, (void*)&parameter[1]);
	
	//wait thread
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	
	//print result
	printf("Actual Count: %d | Expected Count: %d\n", critical_section_variable, COUNTING_NUMBER * 2);

	return 0;
}
