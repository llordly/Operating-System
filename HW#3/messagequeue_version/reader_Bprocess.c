#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MSG_SIZE 100

//contain msg & msg type
struct msgbuf{
	long msgtype;
	char mtext[MSG_SIZE];
};

int main(int argc, char **argv){
	key_t key_id;
	int i = 0;
	struct msgbuf rsvbuf;
	// define msgtype
	int msgtype = 3;

	//get the msg object which key is 1234
	key_id = msgget((key_t)1234, IPC_CREAT|0666);

	if(key_id == -1){
		perror("msgget error :");
		return 0;
	}
	while(1){
		//receive msg
		if(msgrcv(key_id, (void *)&rsvbuf, sizeof(struct msgbuf), msgtype, 0) == -1){
			perror("msgrcv error :");
		}else{
			//received msg is 'exit' program quit
			if(strcmp(rsvbuf.mtext, "exit") == 0){
				printf("-exit this program-\n");
				return 0;
			}
			printf("The received message is: %s\n", rsvbuf.mtext);
		}
	}
	return 0;
}
