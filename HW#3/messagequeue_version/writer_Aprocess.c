#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MSG_SIZE 80

//contain msg & msg type
struct msgbuf{
	long msgtype;
	char mtext[MSG_SIZE];
};

int main(void){
	key_t key_id;
	struct msgbuf sndbuf;
	char input[MSG_SIZE];
	
	//get msg object which key is 1234
	key_id = msgget((key_t)1234, IPC_CREAT|0666);

	if(key_id == -1){
		perror("msgget error :");
		return 0;
	}
	while(1){
		//send msg from keyboard about 1 line
		fgets(input, sizeof(input), stdin);
		input[strlen(input) - 1] = '\0';
		//define msg type
		sndbuf.msgtype = 3;
		strcpy(sndbuf.mtext, input);
		if(msgsnd(key_id, (void *)&sndbuf, sizeof(struct msgbuf), 0) == -1){
			perror("msgsnd error :");
		}
		//if entered msg is 'exit', program quit
		if(strcmp(sndbuf.mtext, "exit") == 0){
			printf("-exit this program-\n");
			break;
		}
	}
	
	return 0;
}
