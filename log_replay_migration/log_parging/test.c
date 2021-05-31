#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	
	char message[BUFSIZ];
	printf("argv[0] : %s %d\n",argv[1] ,BUFSIZ);
	sprintf(message, "./docker_json.o %s",argv[1]);
	system(message);
	return 0;
}
