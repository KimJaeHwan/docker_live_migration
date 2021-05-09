#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define DOCKER_IMAGE_TAR "nginx_docker_test.tar"
#define VM1_port 8888

void error_handling(char * message);

int main(int argc, char * argv[])
{
	int sock, vm1_sock, vm2_sock;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;
	struct sockaddr_in vm1_adr, vm2_adr;
	
	socklen_t vm2_adr_sz;

	FILE * fp;
	int read_cnt;

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
		fputs("Connected with controller......\n",stdout);

	/* connect with vm2 */
	vm1_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(vm1_sock == -1)
		error_handling("sock_vm1() error");
	
	memset(&vm1_adr,0,sizeof(vm1_adr));
	vm1_adr.sin_family = AF_INET;
	vm1_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	vm1_adr.sin_port = htons(VM1_port);

	if(bind(vm1_sock, (struct sockaddr*)&vm1_adr, sizeof(vm1_adr)) == -1)
		error_handling("vm1_bind() error");
	
	if(listen(vm1_sock, 5) == -1)
		error_handling("vm1_listen() error");
	
	vm2_adr_sz = sizeof(vm2_adr);
	vm2_sock = accept(vm1_sock, (struct sockaddr*)&vm2_adr,&vm2_adr_sz);
	if(vm2_sock == -1)
		error_handling("vm2_accept() error");
	else
		printf("Connected VM2!!\n");

	
	
	str_len = recv(sock,message,BUF_SIZE,0);	// recv start message
	if(message[0] != 'y')
		exit(1);
	
	message[str_len] = 0;
	printf("recv message from server : %s\n",message);	// test print

	/* start Full-copy migration */

	printf("Full copy migration\n");
	system("./migration.rc");
	/* end Full-copy migration */


	/* start sendgin docker image file */
	strcpy(message,DOCKER_IMAGE_TAR);
	send(vm2_sock,message,strlen(message),0);
	printf("send message from VM2 : %s", message);

	fp = fopen(DOCKER_IMAGE_TAR,"rb");
	while(1)
	{
		str_len = fread((void*)message, 1, BUF_SIZE, fp);
		if(str_len < BUF_SIZE)
		{
			if(feof(fp) != 0){
				send(vm2_sock, message, str_len,0);
				break;
			}else
				printf("fail to send file\n");
		}
		send(vm2_sock, message, str_len,0);
	}
	/* end sending docker image file */
	
	fclose(fp);
	close(vm1_sock);
	close(vm2_sock);
	close(sock);
	return 0;
}
void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
