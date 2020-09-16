#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SERVERPORT 4040

int get_connected_socket(long port) {
        int socketfd = 0, n = 0;
        struct sockaddr_in server_addr;
        
        
        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                return -1;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        if(connect(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                fprintf(stderr, "Error in connection.\n");
                return -1;
        }
        fprintf(stdout, "Connection established with %s:%d.\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
        return socketfd;
}

void file_transfer(int file_port) {
        int socketfd = 0;
        char filename[20], buffer[512];
        filename[0] = '\0';
        char cnfm;

        socketfd = get_connected_socket(file_port);
        if(socketfd < 0) {
                return ;
        }

        fprintf(stdout, "Enter exit to close the connection.\n");
        while(1) {
                fprintf(stdout, "\n\n>> Enter filename : ");
                scanf("%s", filename);
                if(strncmp(filename, "exit", strlen("exit")) == 0) {
                        send(socketfd, "exit", strlen("exit") + 1, 0);
                        break;
                }
                send(socketfd, filename, strlen(filename) + 1, 0);
                recv(socketfd, buffer, sizeof(buffer), 0);
                if(strncmp(buffer, "404", strlen("404")) == 0) {
                        fprintf(stdout, "File not found. Try again.\n");
                        continue;
                }
                else if(strncmp(buffer, "FOUND", strlen("FOUND")) == 0) {
                        fprintf(stdout, "File found. Initiate Transfer [y/n] : ");
                }
                else {
                        fprintf(stdout, "Unexpected Error.\n");
                        break;
                }
                
                scanf(" %c", &cnfm);
                if(cnfm == 'y') 
                        send(socketfd, "CONFIRMED", strlen("CONFIRMED") + 1, 0);                
                else {
                        send(socketfd, "ABORT", strlen("ABORT") + 1, 0);
                        continue;
                }
                recv(socketfd, buffer, sizeof(buffer), 0);
                int fd = open(filename, O_CREAT | O_WRONLY);
                write(fd, buffer, strlen(buffer) + 1);
                close(fd);
                fprintf(stdout, "FILE RECEIVED AND STORED LOCALLY.\n");
                fprintf(stdout, "_____________FILE CONTENTS_______________\n");
                fprintf(stdout, "%s\n", buffer);
        }
        close(socketfd);
        return;
}
int main() {
        int socketfd;
        struct sockaddr_in server_addr;
        char buffer[512], username[20], password[20];

        socketfd = get_connected_socket(SERVERPORT);
        if(socketfd < 0) {
                return -1;
        }

        REQ_CRED:
        buffer[0] = '\0';
        fprintf(stdout, "\n\nEnter Credentials :\n");
        fprintf(stdout, ">> username : ");
        fscanf(stdin, "%s", username);
        fprintf(stdout, ">> password : ");
        fscanf(stdin, "%s", password);
        strcat(buffer, "req_auth$");
        strcat(buffer, username);
        strcat(buffer, "$");
        strcat(buffer, password);
        strcat(buffer, "$");
        send(socketfd, buffer, strlen(buffer) + 1, 0);
        
        recv(socketfd, buffer, sizeof(buffer), 0);
        if(strncmp(buffer, "INVALID_CRED", strlen("INVALID_CRED")) == 0) {
                fprintf(stdout, "\nInvalid Credentials. Try Again.\n");
                goto REQ_CRED;
        }

        if(strncmp(buffer, "ERROR", strlen("ERROR")) == 0) {
                fprintf(stdout, "Couldn't authorize credentials now. Try Again.\n");
                return -1;
        }
        int file_port = atoi(buffer);
        file_transfer(file_port);
        close(socketfd);
        return 0;
}