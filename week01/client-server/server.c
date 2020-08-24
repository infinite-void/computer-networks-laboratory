#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int main() {
	int socketfd = 0, clientfd = 0, client_addr_len;
	struct sockaddr_in server_addr, client_addr;
	char msgbuff[1025];

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0) {
		fprintf(stderr, "Error in socket creation\n");
		return 1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(4040);

	if(bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < -1) {
		fprintf(stderr, "Error in binding socket to port.\n");
		return 1;
	}

	if(listen(socketfd, 10) < -1) {
		fprintf(stderr, "Error in listening.\n");
		return 1;
	}
	else {
		fprintf(stdout, "Listening on port: %d\n", ntohs(server_addr.sin_port));
	}	

	while(1) {
		clientfd = accept(socketfd, (struct sockaddr*)&client_addr, &client_addr_len);
		if(clientfd < 0) {
			fprintf(stderr, "Error in accepting connection.\n");
			return 1;
		}
		strcpy(msgbuff, "Message sent from server\n");
		write(clientfd, msgbuff, strlen(msgbuff));
		close(clientfd);
		sleep(1);
	}
	close(socketfd);
	return 0;
}
