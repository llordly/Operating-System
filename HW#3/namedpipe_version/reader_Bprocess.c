#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define PIPENAME "./named_pipe_file"

int main(){
	int ret;
	char msg[80];
	int fd;
	pid_t pid;

	//open the named pipe
	fd = open(PIPENAME, O_RDWR);
	if(fd < 0){
		printf("Opening of named pipe failed\n");
		return 0;
	}

	while(1){
		ret = read(fd, msg, sizeof(msg));
		if(ret < 0){
			printf("Read failed\n");
			return 0;
		}
		//if received msg is exit program quit
		if(strcmp(msg, "exit") == 0){
			printf("-exit this program-\n");
			break;
		}
		printf("the received message is : %s\n", msg);
	}
	return 0;
}
