#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

float calcUtil(char msgbuff[]) {
        char *temp, *op;
	float result = 0, op1, op2, op3;
	char g;
	g = msgbuff[0];
	if(msgbuff[0] != 'a' && msgbuff[0] != 's' && msgbuff[0] != 'm' && msgbuff[0] != 'd') {
		temp = strtok(msgbuff, " ");
	        op1 = atof(temp);
        	temp = strtok(NULL, " ");
	        op = temp;
        	temp = strtok(NULL, " ");
	        op2 = atof(temp);
	}
	else {	
        	temp = strtok(msgbuff, " ");
		op = temp;	
        	
		temp = strtok(NULL, " ");
	        op1 = atof(temp);
		
		if(g == 's') goto skip;
		temp = strtok(NULL, " ");
		op2 = atof(temp);


		if(g == 'a') {
			temp = strtok(NULL, " ");
			op3 = atof(temp);
		}

	}
	skip:
        switch(*op) {
                case 'a':
                        result = op1 + op2 + op3;
			result = result / 3;
                        break;
                case 's':
                        result = sqrt((double)op1);
                        break;
                case 'm':
		case '*':
                        result = op1 * op2;
                        break;
                case 'd':
		case '/':
                        result = op1 / op2;
                        break;
                default:
			return 0;
	}
	return result;
}

void calc(int clientfd) {
	char msgbuff[30];
	float result = 0;
	int size = 0;
	while(1) {
		memset(msgbuff, 0, sizeof(msgbuff));
                size = recv(clientfd, msgbuff, sizeof(msgbuff), 0);

                if(size < 1) {
                        fprintf(stderr, "Error in receiving message.\n");
                        return;
                }

                if(strncmp(msgbuff, "exit", 4) == 0) 
                        break;
		fprintf(stdout, "%s : ", msgbuff);
		result = calcUtil(msgbuff);
		fprintf(stdout, "%f\n", result);
		send(clientfd, &result, sizeof(float), 0);
	}
	close(clientfd);
	return;
}

int main() {
	int socketfd = 0, clientfd = 0, length;
	struct sockaddr_in server_addr, client_addr;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0) {
		fprintf(stderr, "Error in socket creation.\n");
		return 1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4040);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		fprintf(stderr, "Error in binding port and socket.\n");
		return 1;
	}

	if(listen(socketfd, 5) < 0) {
		fprintf(stderr, "Error in listening on port %d.\n", ntohs(server_addr.sin_port));
		return 1;
	}
	else
		fprintf(stdout, "Listening on %s:%d.\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

	
	while(1) {
		clientfd = accept(socketfd, (struct sockaddr*)&client_addr, &length);
		if(clientfd < 0) {
			fprintf(stderr, "Could not accept the connection.\n");
			return 1;
		}
		fprintf(stdout, "New Connection established.\n");
		calc(clientfd);
	}
	close(socketfd);
	return 0;
}
