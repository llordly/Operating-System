#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <wait.h>

#define COUNTING_NUMBER 2000000


//struct about process's information and global critical variable
struct smStruct{
	int processidassign;
	int turn;
	int flag[2];
	int critical_section_variable;
};

int main(void){
	int smid;
	void *smaddr;
	struct smStruct* smstruct;
	int ret;

	//make a shared memory
	smid = shmget((key_t)1234, sizeof(struct smStruct), IPC_CREAT | 0666);
	if(smid < 0){
		perror("shmget error\n");
		return 0;
	}

	//attach the shared memory
	smaddr = shmat(smid, (void*)0, 0);

	if(smaddr == (char *) - 1){
		perror("failed attact address");
		return 0;
	}
	
	//type casting
	smaddr = (struct smStruct*)smaddr;

	//defalt setting about struct
	smstruct = smaddr;
	smstruct->turn = 0;
	smstruct->processidassign = 0;
	smstruct->flag[0] = 0;
	smstruct->flag[1] = 0;
	smstruct->critical_section_variable = 0;

	pid_t pid1, pid2;
	int status1, status2;

	//fork first child
	pid1 = fork();
	if(pid1 == 0){
		//exec child program
		execl("./child", "", NULL);
		printf("exec error\n");
	}
	//fork second child
	pid2 = fork();
	if(pid2 == 0){
		//wait 2000 milsecond to ordering
		usleep(2000);
		//set order number of second child
		smstruct->processidassign = 1;
		//exec child program
		execl("./child", "", NULL);
		printf("exec error\n");
	}
	
	//parent process wait child process
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);

	//print result
	printf("Actual Count: %d | Expected Count: %d\n", smstruct->critical_section_variable, COUNTING_NUMBER * 2);
	
	//detach the shared memory
	ret = shmdt(smaddr);
	if(ret == -1){
		perror("detach failed\n");
		return 0;
	}
	
	//remove the shared memory
	ret = shmctl(smid, IPC_RMID, 0);
	if(ret == -1){
		perror("remove failed\n");
		return 0;
	}

	return 0;
}
