#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
	int socketfd = 0, length;
	struct sockaddr_in server_addr;
	char msgbuff[1025], recvbuff[1025];

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0) {
		fprintf(stderr, "Error in socket creation.\n");
		return 1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4040);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	if(connect(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		fprintf(stderr, "Error in connecting to %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
		return 1;
	}
	else 
		fprintf(stdout, "Connected to %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

	fprintf(stdout, "Enter message: ");
	fgets(msgbuff, sizeof(msgbuff), stdin);
	send(socketfd, msgbuff, sizeof(msgbuff), 0);
	recv(socketfd, recvbuff, sizeof(recvbuff), 0);
	fprintf(stdout, "Echoed from server : %s\n", recvbuff);
	close(socketfd);
	return 0;
}
