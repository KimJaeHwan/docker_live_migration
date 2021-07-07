#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define BUF_SIZE 1024
void error_handling(char * message);

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];
	int str_len,num;
	struct sockaddr_in serv_adr;
	float start_time, end_time;
	char input_message[99],temp;

	if(argc != 3)
	{
		printf("Usage : %s <IP> <port>\n",argv[0]);
		exit(1);
	}
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(sock,(struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error");
	else
		fputs("Connected..........",stdout);

	fputs("migrantion start[Y/n] : ",stdout);
	//scanf("%c%c",&input_message,temp);
	fgets(input_message,sizeof(input_message),stdin);
	if( input_message[0] != 0 && input_message[0] != 'y' && input_message[0] !='Y')
		exit(1);
	message[0] = 'y';
	start_time = clock();
	send(sock,message,1,0);		// send start message 
	printf("send message from controller %c\n",message[0]);

	str_len = recv(sock,message, BUF_SIZE,0);	// recv result of migration
	message[str_len] = 0;
	printf("receive message from contoller : %s\n",message);

	if(message[0] == 'y')
		printf("migration complete!!!!!\n");
	end_time = clock();
	printf("process time : %.3f\n",(end_time - start_time)/CLOCKS_PER_SEC);

	close(sock);

	return 0;
}
void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}
