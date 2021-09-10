#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

int main(void){
	pid_t  pid, pid2;
	int status1, status2;
	printf("Waiting for Child Processes\n");
	pid = fork();
	if(pid == 0){
		execl("child1", "", NULL);
		printf("nono\n");
	}
	pid2 = fork();
	if(pid2 == 0){
		usleep(500000);
		execl("child2", "", NULL);
		printf("nono\n");
	}
	waitpid(pid, &status1, 0);
	waitpid(pid2, &status2, 0);
	
	printf("Child processes are exit (%d) (%d)\n", status1, status2);
	return 0;
}
