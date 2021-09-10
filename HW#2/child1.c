#include <stdio.h>
#include <unistd.h>

int main(void){
	int i = 0;
	for (i = 0; i < 12; i++){
		FILE* file = fopen("test.txt", "w");
		fprintf(file, "%d\n", i);
		printf("Child1 wrote %d.\n", i);
		fclose(file);
		sleep(1);
	}
	return 0;
}

