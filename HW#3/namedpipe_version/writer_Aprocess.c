#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define MSG_SIZE 80
#define PIPENAME "./named_pipe_file"

int main(void){
	char msg[MSG_SIZE];
	char input[MSG_SIZE];
	int fd;
	int ret, i;
	
	ret = access(PIPENAME, F_OK);
	if(ret == 0){
		//delete the pip
		unlink(PIPENAME);
	}

	// create a named pipe
	ret = mkfifo(PIPENAME, 0666);
	if(ret < 0){
		printf("Creation of named pipe failed\n");
		return 0;
	}

	//open the named pipe
	fd = open(PIPENAME, O_WRONLY);
	if(fd < 0){
		printf("Open failed\n");
		return 0;
	}

	while(1){
		//get msg from keyboard about 1 line
		fgets(input, sizeof(input), stdin);
		input[strlen(input) - 1] = '\0';
		snprintf(msg, sizeof(msg), "%s", input);
		ret = write(fd, msg, sizeof(msg));
		if(ret < 0){
			printf("Write failed\n");
			return 0;
		}
		//if you enter 'exit' program quit
		if(strcmp(msg, "exit") == 0){
			printf("-exit this program-\n");
			return 0;
		}
	}
	return 0;
}

