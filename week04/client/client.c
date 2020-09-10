#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PROXYPORT 4043

int main() {
        int socketfd = 0, n = 0;
        struct sockaddr_in server_addr;
        char buffer[512], filename[20];

        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                return -1;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PROXYPORT);
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        if(connect(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                fprintf(stderr, "Error in connection.\n");
                return -1;
        }
        fprintf(stdout, "Connection established with %s:%d.\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
        fprintf(stdout, "Enter filename to retreive : ");
        scanf("%s", filename);

        n = send(socketfd, filename, strlen(filename) + 1, 0);
        if(n < 0) {
                fprintf(stderr, "Error in sending request.\n");
                return -1;
        }

        n = recv(socketfd, buffer, sizeof(buffer), 0);
        if(n < 0) {
                fprintf(stderr, "Error in receving bytes from server.\n");
                return -1;
        }

        if(strncmp(buffer, "404", 3) == 0) {
                fprintf(stderr, "Requested file not found.\n");
                return -1;
        }

        if(strncmp(buffer, "error", 5) == 0) {
                fprintf(stderr, "Connection Error!.\n");
                return -1;
        }

        fprintf(stdout, "File Received: %s", buffer);
        return 0;
}