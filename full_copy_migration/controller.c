#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
void error_handling(char * message);	// control error

int main(int argc, char * argv[])
{

	int serv_sock;
	int clnt_sock[3];		// sock[0] : user, sock[1] : VM1, sock[2] : VM2
	char message[BUF_SIZE];
	int str_len, i;

	struct sockaddr_in serv_adr;
	struct sockaddr_in clnt_adr[3];		// sock[0] : user, sock[1] : VM1, sock[2] : VM2

	socklen_t clnt_adr_sz[3]; 			// * 
	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");

	if(listen(serv_sock ,5) == -1)
		error_handling("listen() error");

	clnt_adr_sz[0] = sizeof(clnt_adr[0]);	// user
	clnt_adr_sz[1] = sizeof(clnt_adr[1]);	// VM1
	clnt_adr_sz[2] = sizeof(clnt_adr[2]);	// VM2
	
	for(i = 0;i<3;i++){
		clnt_sock[i] = accept(serv_sock, (struct sockaddr*)clnt_adr,clnt_adr_sz);
		if(clnt_sock[i] == -1)
			error_handling("accept() error");
		else
			printf("Connected client : %d\n",i);
	}
	/* Communication with User */
	
	str_len = recv(clnt_sock[0], message, BUF_SIZE, 0);	// recv start message
	message[str_len] = 0;
	printf("receive message from User : %s\n",message);
	if(message[0] != 'y')		// not service 
		exit(1);
	
	/* send to VM1 */
	send(clnt_sock[1], message, 1 ,0);	// message[0] = 'y'
	printf("send message to VM1 : %c\n",message[0]);

	str_len = recv(clnt_sock[2], message, BUF_SIZE, 0);	// recv from vm2
	message[str_len] = 0;
	printf("receive message from VM2 : %s\n",message);

	if(message[0] == 'y'){
		printf("Complete Full copy migration successfully!!\n");
		send(clnt_sock[0], message,1,0);	// send success message to user 
	}

	
	/* close all socket */
	for(i = 0;i<3;i++)
	{
		close(clnt_sock[i]);
	}
	close(serv_sock);
	return 0;
}
void error_handling(char * message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}





