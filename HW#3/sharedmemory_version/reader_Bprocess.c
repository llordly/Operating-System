#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MSG_SIZE 80

struct shared{
	int isEntered;
	char input[MSG_SIZE];
};

int main(void){
	int shmid;
	void *shmaddr;
	struct shared* shared_mem;
	int ret;
	//get shared memory id
	shmid = shmget((key_t)1234, sizeof(struct shared), IPC_CREAT|0666);
	if(shmid == -1){
		perror("shared memory access is failed\n");
		return 0;
		}

	//attach the shared memory
	shmaddr = shmat(shmid, (void *)0, 0);
	if(shmaddr == (char *)-1){
		perror("attach failed\n");
		return 0;
	}
	shared_mem = (struct shared*)shmaddr;

	//check msg is written
	shared_mem->isEntered = 0;

	while(1){
	//if msg is written, print msg
		if(shared_mem->isEntered){
	//if received msg is exit program quit
			if(strcmp(shared_mem->input, "exit") == 0){
				printf("-exit this program-\n");
				break;
			}
			printf("data read from shared memory: %s\n", shared_mem->input);
			shared_mem->isEntered = 0;
		}
	}
	//detach the shared memory
	ret = shmdt(shmaddr);
	if(ret == -1){
		perror("detach failed\n");
		return 0;
	}
	
	//remove the shared memory
	ret = shmctl(shmid, IPC_RMID, 0);
	if(ret == -1){
		perror("remove failed\n");
		return 0;
	}
	return 0;
}
