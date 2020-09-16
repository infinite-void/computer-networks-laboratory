#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SERVERPORT 4040
#define FILEPORT 4052

int listening_socket_connection(long port) {
        int socketfd;
        struct sockaddr_in server_addr;

        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                return -1;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        if(bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                fprintf(stderr, "Error in binding socket to port.\n");
                return -1;
        }

        if(listen(socketfd, 5) < 0) {
                fprintf(stderr, "Error in listening in %s:%d.\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
                return -1;
        }
        fprintf(stderr, "Listening at %s:%d.\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
        return socketfd;
}

void file_transfer(int client) {
        int socketfd, clientfd, length = 0;
        struct sockaddr_in server_addr, client_addr;
        char buffer[512], filename[20];
        const char* port = "4052";

        socketfd = listening_socket_connection(FILEPORT);
        if(socketfd < 0)
                return; 
        send(client, port, strlen(port) + 1, 0);
        
        clientfd = accept(socketfd, (struct sockaddr*)&client_addr, &length);
        if(clientfd < 0) {
                fprintf(stderr, "Error in accepting connection.\n");
                close(socketfd);
                return;
        }
        fprintf(stdout, "New file trasfer connection established.\n");
        while(1) {
                recv(clientfd, filename, sizeof(buffer), 0);
                fprintf(stdout, "\nRequested File: %s\n", filename);
                if(strncmp(filename, "exit", strlen("exit")) == 0) {
                        fprintf(stdout, "Client exiting file_transfer.\n");
                        break;
                }
                int fd = open(filename, O_RDONLY);
                if(fd < 0) {
                        send(clientfd, "404", strlen("404") + 1, 0);
                        fprintf(stdout, "Requested file not found.\n");
                        continue;
                }
                
                send(clientfd, "FOUND", strlen("FOUND") + 1, 0);
                fprintf(stdout, "File found waiting confirmation.\n");

                recv(clientfd, buffer, sizeof(buffer), 0);
                if(strncmp(buffer, "ABORT", strlen("ABORT")) == 0) {
                        fprintf(stdout, "Client aborted the transfer.\n");
                        continue;
                }

                read(fd, buffer, sizeof(buffer));
                send(clientfd, buffer, strlen(buffer) + 1, 0);
                fprintf(stdout, "Confirmation received and file sent.\n");
        }
        close(clientfd);
        close(socketfd);
        fprintf(stdout, "Closing the file-transfer connection.\n\n");
        return ;
}

void authorize_and_handle(int clientfd) {
        char buffer[1024];
        char username[20], password[20], linebuffer[512];
        char *user, *pass;
        int n, i, j, flag = 0; 
        AUTHORIZE:
        n = recv(clientfd, buffer, sizeof(buffer), 0);

        if(strncmp(buffer, "req_auth$", sizeof("req_auth$")) == 0) {
                send(clientfd, "Invalid Request", sizeof("Invalid Request") + 1, 0);
                return;
        }
        i = 9;
        j = 0;
        while(buffer[i] != '$' && buffer[i] != '\n' && buffer[i] != '\0') {
                username[j] = buffer[i];
                j++;
                i++;
        }
        username[j] = '\0';
        i++;
        j = 0;
        while(buffer[i] != '$' && buffer[i] != '\n' && buffer[i] != '\0') {
                password[j] = buffer[i];
                j++;
                i++;
        }
        password[j] = '\0';
        fprintf(stdout, "\nRequest for username: %s password: %s\n", username, password);

        FILE* fd = fopen("authorize.txt", "r");

        if(fd < 0) {
                fprintf(stderr, "Error in opening auth file.\n");
                send(clientfd, "ERROR", strlen("ERROR") + 1, 0);
                return;
        }

        while(fgets(linebuffer, sizeof(linebuffer), fd)) {
                user = strtok(linebuffer, "$");
                pass = strtok(NULL, "\n");
                if(strcmp(username, user) == 0&& strcmp(password, pass) == 0) {
                        file_transfer(clientfd);
                        flag = 1;
                        break;
                }
        }
        if(flag == 0) {
                fprintf(stdout, "Invalid Access Denied.\n");
                send(clientfd, "INVALID_CRED", strlen("INVALID_CRED") + 1, 0);
                goto AUTHORIZE;
        }
        return;
}

int main() {
        int socketfd, clientfd, length = 0;
        struct sockaddr_in client_addr;

        socketfd = listening_socket_connection(SERVERPORT);
        if(socketfd < 0) {
                return -1;
        }
        
        while(1) {
                clientfd = accept(socketfd, (struct sockaddr*)&client_addr, &length);
                if(clientfd < 0) {
                        fprintf(stderr, "Error in accepting connection.\n");
                        continue;
                }
                fprintf(stdout, "New control connection established.\n");
                authorize_and_handle(clientfd);
                close(clientfd);
        }
        close(socketfd);
        return 0;
}