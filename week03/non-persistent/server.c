#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4040

void sendFiles(int clientfd) {
        time_t clock;
        char request[512], response[1024], c;
        const char* header = "Server: Apache/2.4.6 (CentOS) OpenSSL/1.0.2k-fips PHP/5.4.16\nLocation: http://somerandomweb.com/index.php\nContent-Length: 229\nKeep-Alive: timeout=5, max=100\nConnection: Keep-Alive\nContent-Type: text/html; charset=iso-8859-1\n"; 
        while(recv(clientfd, request, sizeof(request), 0)) {
                if(strncmp(request, "EXIT", 4) == 0)
                        break;
                fprintf(stdout, "--------------- REQUEST RECEIVED ------------------\n");
                fprintf(stdout, "%s\n", request);
                fprintf(stdout, "--------------- END OF REQUEST ------------------\n");
                char file[20];
                char *temp = &request[0], *fname = &file[0];
                while(*(temp++) != ' ');
                while(*temp != ' ') {
                        *fname = *temp;
                        fname++;
                        temp++;
                }
                *fname = '\0';
                fprintf(stdout, "FILE TO SEND:: %s\n", file);
                fprintf(stdout, "--------------- SENDING RESPONSE ------------------\n");
                FILE* fp = fopen(file, "r");
                response[0] = '\0';
                if(fp == 0) {
                        strcat(response, "HTTP/1.1 404 Not Found\n");
			strcat(response, "Date: ");
			time(&clock);
			strcat(response,asctime(localtime(&clock)));
			strcat(response, header);
                }
                else {
                        strcat(response, "HTTP/1.1 302 Found \n");
			strcat(response, "Date: ");
			time(&clock);
			strcat(response,asctime(localtime(&clock)));
			strcat(response, header);
                        int i = strlen(response);
                        while((c = fgetc(fp)) != EOF) 
                                response[i++] = c;
                        response[i] = '\0';
                }
                send(clientfd, response, strlen(response) + 1, 0);
                fprintf(stdout, "-------------- RESPONSE SENT ------------------------\n");
                request[0] = '\0';
        }
        fprintf(stdout, "****************** CONNECTION ENDED ******************\n");
        close(clientfd);
}
int main() {
        int socketfd = 0, clientfd = 0, length;
        struct sockaddr_in server_addr, client_addr;
        char buffer[512];

        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                return -1;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(PORT);

        if(bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                fprintf(stderr, "Error in binding port to socket.\n");
                return -1;
        }

        if(listen(socketfd, 5) < 0) {
                fprintf(stderr, "Error in listening on port.\n");
                return -1;
        }
        fprintf(stdout, "Listening on %d.\n", ntohs(server_addr.sin_port));

        while(1) {
                clientfd = accept(socketfd, (struct sockaddr*)&client_addr, &length);
                if(clientfd < 0) {
                        fprintf(stdout, "Error in accpeting a client connection.");
                        continue;
                }
                fprintf(stdout, "****************** CONNECTION ESTABLISHED ******************\n");
                sendFiles(clientfd);
        }
        close(socketfd);
        return 0;
}