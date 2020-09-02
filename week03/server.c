#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void sendFile(int clientfd, char filename[]) {
	FILE* fp = fopen(filename, "r");
	char buffer[512];
	while(fgets(buffer, sizeof(buffer), fp))
		send(clientfd, buffer, sizeof(buffer), 0);
	fclose(fp);
	close(clientfd);
}

int main() {
	int socketfd, clientfd = -1, length;
	struct sockaddr_in server_addr, client_addr;
	char filename[20];

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0) {
		fprintf(stderr, "Error in socket creation.\n");
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(4040);

	if(bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		fprintf(stderr, "Error in binding socket to port.\n");
		return -1;
	}

	if(listen(socketfd, 5) < 0) {
		fprintf(stderr, "Error in listening to %s:%d.\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
		return -1;
	}
	else 
		fprintf(stdout, "Listening on %s:%d.\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

	while(1) {
		clientfd = accept(socketfd, (struct sockaddr*)&client_addr, &length);
		if(clientfd < 0) {
			fprintf(stderr, "Could not receive the connection.\n");
			return -1;
		}
		fprintf(stdout, "Connection established.\n");
		recv(clientfd, filename, sizeof(filename), 0);
		sendFile(clientfd, filename);
	}
	return 0;
}
