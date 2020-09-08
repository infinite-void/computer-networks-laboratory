#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
	int socketfd = 0, n;
	struct sockaddr_in host_addr;
	char msgbuff[30];
	float result;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0) {
		fprintf(stderr, "Error in socket creation.\n");
		return -1;
	}

	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(4040);
	inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

	if(connect(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
		fprintf(stderr, "Error in connecting to %s:%d.\n", inet_ntoa(host_addr.sin_addr), ntohs(host_addr.sin_port));
		return -1;
	}
	else
		fprintf(stdout, "Connected to %s:%d.\n", inet_ntoa(host_addr.sin_addr), ntohs(host_addr.sin_port));

	fprintf(stdout, "Calculator connected. Now use first letter for (a)ddition, (s)ubtraction, (m)ultiplication, (d)ivision for an operation.\n Example: \"a 4 5\" for addition or \"m 7 8\" for multiplication.You can also use space separated expressions i.e., \"4 + 5\" or \"5 - 4\" or \"78 * 4\" or \"34 / 2 \"\n");

	while(1) {		
		fprintf(stdout, "Enter expression: ");
		bzero(msgbuff, sizeof(msgbuff));
		fgets(msgbuff, sizeof(msgbuff), stdin); 
		send(socketfd, msgbuff, sizeof(msgbuff), 0);
		if(strncmp(msgbuff, "exit", 4) == 0) {
			fprintf(stdout, "Exiting...\n");
			break;
		}
		recv(socketfd, &result, sizeof(float), 0); 
		fprintf(stdout, "Result: %f.\n", result);
	}
	close(socketfd);
	return 0;
}
