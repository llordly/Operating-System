#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define COUNTING_NUMBER 2000000

//child's local variable about counting
int localcount = 0;

//struct about process's information and global variable critical variable
//same with parent's struct
struct smStruct{
	int processidassign;
	int turn;
	int flag[2];
	int critical_section_variable;
};

//lock with shared memory's variable
void lock(struct smStruct *smstruct, int self){
	smstruct->flag[self] = 1;
	smstruct->turn = 1 - self;
	while(smstruct->flag[1 - self] && smstruct->turn == 1 - self);
}

//unlock with shared memory's variable
void unlock(struct smStruct *smstruct, int self){
	smstruct->flag[self] = 0;
}

//first increase local count; it's result is same with COUNTING_NUMBER
//after lock, increase critical variable about shared memory
//after increase critical variable, unlock
void func(struct smStruct *smstruct, int Myorder){
	int i;
	for(i = 0; i < COUNTING_NUMBER; i++){
		localcount++;
		lock(smstruct, Myorder);
		smstruct->critical_section_variable++;
		unlock(smstruct, Myorder);
	}
}


int main(void){
	int smid;
	void *smaddr;
	struct smStruct* smstruct;

	//create shared memory
	smid = shmget((key_t)1234, sizeof(struct smStruct), IPC_CREAT | 0666);
	if(smid < 0){
		perror("shmget error\n");
		return 0;
	}

	//attach shared memory
	smaddr = shmat(smid, (void *)0, 0);
	if(smaddr == (char *) - 1){
		perror("failed attact address");
		return 0;
	}

	//type casting
	smstruct = (struct smStruct*)smaddr;
	//set order number from struct's processisassign value
	//this value is same with child process's number
	int Myorder = smstruct->processidassign;
	//print order and process id
	printf("Myorder = %d,process pid = %d\n", Myorder, getpid());

	//increase critical variable and local count with this func
	func(smstruct, Myorder);

	//print local count and exit this program
	printf(" child finish! local count = %d\n", localcount);
	
	return 0;
}
