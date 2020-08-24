#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void chat(int socketfd) {
	char msgbuff[20];
	while(1) {
		memset(msgbuff, 0, 20);
		fprintf(stdout, "Client: ");
		fgets(msgbuff, 20, stdin);
		write(socketfd, msgbuff, sizeof(msgbuff));
		if(strncmp(msgbuff, "exit", 4) == 0) {
                        fprintf(stdout, "Exiting from chat...\n");
                        break;
               	}
		read(socketfd, msgbuff, sizeof(msgbuff));
		fprintf(stdout, "Server: %s", msgbuff);
		if(strncmp(msgbuff, "exit", 4) == 0) {
                        fprintf(stdout, "Server exiting from chat...\n");
                        break;
                }
	}
}

int main() {
	int socketfd = 0;
	struct sockaddr_in server_addr;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket < 0) {
		fprintf(stderr, "Error in socket creation.\n");
		return 1;
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4040);
	//server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	if(connect(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		fprintf(stderr, "Error in connecting to %d:%d\n", server_addr.sin_addr.s_addr, ntohs(server_addr.sin_port));
		return 1;
	}
	else 
		fprintf(stdout, "Connected to %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
	
	chat(socketfd);
	close(socketfd);
	return 0;
}
