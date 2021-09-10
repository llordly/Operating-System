#include <stdio.h>
#include <unistd.h>

int main(void){
	int i = 0;
	char str[5];
	FILE* file;
	for(i; i < 6; i++){
		file = fopen("test.txt", "r");
		fgets(str, 5, file);
		printf("child2 : %s", str);
		sleep(2);
	}
	fclose(file);
	
	return 0;
}
