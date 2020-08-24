#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
	int socketfd = 0, clientfd = 0, length, msglength;
	struct sockaddr_in server_addr, client_addr;
	char msgbuff[1025];

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0) {
		fprintf(stderr, "Error in socket creation.\n");
		return 1;
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4040);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		fprintf(stderr, "Error in binding socket and port.\n");
		return 1;
	}

	if(listen(socketfd, 5) < 0) {
		fprintf(stderr, "Could not listen on the specified port.\n");
		return 1;
	}
	else 
		fprintf(stdout, "Listening on %s:%d.\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

	while(1) {
		clientfd = accept(socketfd, (struct sockaddr*)&client_addr, &length);
		if(clientfd < 0) {
			fprintf(stderr, "Could not accept connections.\n");
			return 1;
		}
		fprintf(stdout, "client connected with ip address: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(server_addr.sin_port));
		read(clientfd, msgbuff, sizeof(msgbuff));
		fprintf(stdout, "Message received from client: %s\n", msgbuff);
		write(clientfd, msgbuff, sizeof(msgbuff));
		close(clientfd);
	}
	close(socketfd);
	return 1;
}		
