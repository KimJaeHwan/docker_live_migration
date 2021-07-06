#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

#define VM1_port 8888
#define VM1_IP	"192.168.139.142"

void error_handling(char * message);

int main(int argc, char * argv[])
{
	int sock, vm1_sock;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;
	struct sockaddr_in vm1_adr;
	
	FILE * fp;
	if(argc != 3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	/* connect with controller */
	
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
		fputs("Connected with controller.....\n",stdout);

	/* connect with VM1 */
	
	vm1_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(vm1_sock == -1)
		error_handling("socket() error");
	
	memset(&vm1_adr, 0, sizeof(vm1_adr));
	vm1_adr.sin_family = AF_INET;
	vm1_adr.sin_addr.s_addr = inet_addr(VM1_IP);
	vm1_adr.sin_port = htons(VM1_port);

	if(connect(vm1_sock,(struct sockaddr*)&vm1_adr, sizeof(vm1_adr)) == -1)
		error_handling("connect() error");
	else
		fputs("Connected with VM1......\n",stdout);

	/* start receving docker image file */

	str_len = recv(vm1_sock, message, BUF_SIZE, 0);
	message[str_len] = 0;
	printf("receive message from VM1 : %s %d\n",message,str_len);
	send(vm1_sock, message, BUF_SIZE,0);	// synchronize

	fp = fopen("dockerfile","wb");
	
	while((str_len = recv(vm1_sock, message, BUF_SIZE,0)) != 0)
		fwrite((void*)message, 1, str_len, fp);
	
	send(vm1_sock,"recv dockerfile successfully!!",25,0);

	/*
	while(1)
	{
		str_len = recv(vm1_sock, message,BUF_SIZE,0);
		if(str_len < BUF_SIZE )
		{
			fwrite((void*)message, 1, str_len, fp);
			printf("recv docker image successfully!!\n");
			break;
		}else{
			fwrite((void*)message,1,BUF_SIZE,fp);
		}
	}
	*/

	fclose(fp);

	printf("recv dockerfile  successfully!!\n");
	
	/* end receving docker image file */

	


	message[0] = 'y';
	send(sock,message,1,0);
	printf("send message from controller : %c\n",message[0]);
	printf(" sucess ~~\n");
	
	system("docker image build -t log_replay_test .");
	system("docker run -d -it log_replay_test /bin/bash");
	
	close(sock);
	close(vm1_sock);
	return 0;
}

void error_handling(char * message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
