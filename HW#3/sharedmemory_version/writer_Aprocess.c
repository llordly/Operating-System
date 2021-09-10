#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

#define MSG_SIZE 80

struct shared{
	int isEntered;
	char input[MSG_SIZE];
};

int main(void){
	int shmid;
	int i;
	void *shmaddr;
	struct shared* shared_mem;
	int ret;
	char input[MSG_SIZE];

	//make a shared memory
	shmid = shmget((key_t)1234, sizeof(struct shared), IPC_CREAT|0666);
	if(shmid < 0){
		perror("shmget");
		return 0;
	}
	
	//atach the shared memory
	shmaddr = shmat(shmid, (void *)0, 0);
	if(shmaddr == (char *) - 1){
		perror("attach failed\n");
		return 0;
	}

	shared_mem = (struct shared*)shmaddr;

	while(1){
		fgets(input, sizeof(input), stdin);
		input[strlen(input) - 1] = '\0';
		strcpy(shared_mem->input, input);
		//if you write msg, change isEntered to written
		shared_mem->isEntered = 1;

	//if you enter exit program quit
		if(strcmp(input, "exit") == 0){
			printf("-exit this program-\n");
			break;
		}
	}
	//detach the shared memory
	ret = shmdt(shmaddr);
	if(ret == -1){
		perror("detach failed\n");
		return 0;
	}

	return 0;
}

