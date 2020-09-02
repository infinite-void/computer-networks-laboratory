#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
	int socketfd;
	struct sockaddr_in host_addr;
	char filename[20], buffer[512];

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket < 0) {
		fprintf(stderr, "Error in socket creation.\n");
		return -1;
	}

	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(4040);
	inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

	if(connect(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
		fprintf(stderr, "Error in binding socket to port.\n");
		return -1;
	}
	else
		fprintf(stdout, "Connected to %s:%d.\n", inet_ntoa(host_addr.sin_addr), ntohs(host_addr.sin_port));

	fprintf(stdout, "Enter filename : ");
	scanf("%s", filename);
	send(socketfd, filename, sizeof(filename), 0);
	fprintf(stdout, "----------GETTING_FILE_%s----------\n", filename);
	while(recv(socketfd, buffer, sizeof(buffer), 0)) 
		fprintf(stdout, "%s", buffer);
	fprintf(stdout, "---------END_OF_FILE-----------\n");
	close(socketfd);
	return 0;
}	
