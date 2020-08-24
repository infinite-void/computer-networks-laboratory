#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 4040

void chat(int clientfd) {
	char msgbuff[20];
	int n;
	while(1) {
		memset(msgbuff, 0, sizeof(msgbuff));
		read(clientfd, msgbuff, sizeof(msgbuff));
		if(strncmp("exit", msgbuff, 4) == 0) {
                        printf("Client exiting from chat...\n");
                        break;
                }
		fprintf(stdout, "Client: %sServer: ", msgbuff);
		memset(msgbuff, 0, sizeof(msgbuff));
		n = 0;
		fgets(msgbuff, 20, stdin);
		write(clientfd, msgbuff, sizeof(msgbuff));

		if(strncmp("exit", msgbuff, 4) == 0) {
			printf("Exiting from chat...\n");
			break;
		}
	}
}

int main() {
	int socketfd = 0, clientfd = 0, length = 0;
	struct sockaddr_in server_addr, client_addr;
	
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd == -1) {
		fprintf(stderr, "Error in socket creation.\n");
		return 1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4040);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		fprintf(stderr, "Error in binding socket and port.\n");
		return 1;
	}
	
	if(listen(socketfd, 5) < 0) {
		fprintf(stderr, "Error in listening.\n");
		return 1;
	}
	else 
		fprintf(stdout, "Listening on port, %d.\n", ntohs(server_addr.sin_port));
	
	clientfd = accept(socketfd, (struct sockaddr*)&client_addr, &length);
	if(clientfd < 0) {
		fprintf(stderr, "Server could not accept any connection.\n");
		return 1;
	}
	else
		fprintf(stdout, "Connection established.\n");

	chat(clientfd);
	close(clientfd);
	close(socketfd);
}
		
