#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int main() {
	int socketfd = 0, inputSize;
	struct sockaddr_in server_addr;
	char msgbuff[1025];

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0) {
		fprintf(stderr, "Error in socket creation\n");
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

	while((inputSize = read(socketfd, msgbuff, sizeof(msgbuff) - 1)) > 0) {
		msgbuff[inputSize] = '\0';
		if(fputs(msgbuff, stdout) == EOF) {
			fprintf(stderr, "\nError in message display.\n");
		}
	}
	close(socketfd);
	return 0;
}
