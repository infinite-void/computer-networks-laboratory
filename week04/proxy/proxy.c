#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SERVERPORT 4040
#define PROXYPORT 4043

void handleConnection(int clientfd) {
        FILE *log_file = NULL, *wlog_file = NULL, *fd = NULL;
        char request[512], filename[20], timestamp[30], response[512], buff[512];
        char filebuffer[512], timebuffer[30], timebuffer_f[30], linebuffer[512], logwrite[512], temp[512];
        char server_response[512], *filebuff, *timebuff;
        int socketfd = 0;
        int newlog_flag = 0;
        struct sockaddr_in server_addr;
        int i = 0;
        logwrite[0] = '\0';
        response[0] = '\0';
        request[0] = '\0';
        logwrite[0] = '\0';
        filebuffer[0] = '\0';
        timebuffer[0] = '\0';
        recv(clientfd, request, sizeof(request), 0);
        while(request[i] != '\0' && request[i] != '\n') {
                filename[i] = request[i];
                i++;
        }
        filename[i] = '\0';
        
        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                strcpy(response, "error");
                send(clientfd, response, strlen(response) + 1, 0);
                return;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVERPORT);
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        if(connect(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                fprintf(stderr, "Error in connecting to server.\n");
                strcpy(response, "error");
                send(clientfd, response, strlen(response) + 1, 0);
                return;
        }
        fprintf(stdout, "Connected to server.\n");
        
        request[0] = '\0';
        strcat(request, filename);
        strcat(request, "$");
        log_file = fopen(filename, "r");
        if(log_file == NULL) {
                fprintf(stdout, "fileNot found in cache.\n");
                send(socketfd, request, strlen(request) + 1, 0);
                recv(socketfd, timebuffer, sizeof(timebuffer), 0);
                strcpy(timebuffer_f, timebuffer);
                recv(socketfd, filebuffer, sizeof(filebuffer), 0);  
        }
        else {
                fd = fopen("proxylog.txt", "r");
                while(fgets(linebuffer, sizeof(linebuffer), fd)) {
                        filebuff = strtok(linebuffer, "$");
                        timebuff = strtok(NULL, "\n");
                        if(strcmp(filebuff, filename) == 0) {
                                strcat(request, timebuff);
                                send(socketfd, request, strlen(request) + 1, 0);
                                recv(socketfd, buff, sizeof(buff), 0);
                                if(strncmp(buff, "uptodate", 8) == 0) {
                                        fprintf(stdout, "file in cache uptodate\n");
                                        while(fgets(temp, sizeof(temp), log_file)) {
                                                strcat(filebuffer, temp);
                                                strcat(filebuffer, "\n");
                                        }
                                        send(clientfd, filebuffer, sizeof(filebuffer), 0);
                                        return;
                                }
                                else {
                                        newlog_flag = 1;
                                        fprintf(stdout, "file outdated in cache");
                                        strcpy(timebuffer_f, buff);
                                        buff[0] = '\0';
                                        recv(socketfd, buff, sizeof(buff), 0);
                                        strcpy(filebuffer, buff);
                                        fprintf(stdout, "New data: %s %s ", timebuffer_f, filebuffer);
                                        continue;
                                }
                        }
                        strcat(logwrite, filebuff);
                        strcat(logwrite, "$");
                        strcat(logwrite, timebuff);
                        strcat(logwrite, "\n");
                }
               fclose(fd);

        }
        if(newlog_flag) 
                wlog_file = fopen("proxylog.txt", "w");
        else 
                wlog_file = fopen("proxylog.txt", "a");
        
        strcat(logwrite, filename);
        strcat(logwrite, "$");
        strcat(logwrite, timebuffer_f);
        fputs(logwrite, wlog_file);
        fclose(wlog_file);

        fd = fopen(filename, "w");
        fputs(filebuffer, fd);
        fclose(fd);
        send(clientfd, filebuffer, sizeof(filebuffer), 0);
        
        return;
}
int main() {
        int socketfd = 0, clientfd = 0, length = 0;
        struct sockaddr_in host_addr, client_addr;
        char buffer[1024];

        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                return -1;
        }

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(PROXYPORT);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

        if(bind(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
                fprintf(stderr, "Error in binding port.\n");
                return -1;
        }

        if(listen(socketfd, 5) < 0) {
                fprintf(stderr, "Error in listening on port.\n");
                return -1;
        }
        fprintf(stdout, "Listening on %s:%d.\n", inet_ntoa(host_addr.sin_addr), ntohs(host_addr.sin_port));

        while(1) {
                clientfd = accept(socketfd, (struct sockaddr*)&client_addr, &length);
                if(clientfd < 0) {
                        fprintf(stderr, "Error in accepting connection.\n");
                        return -1;
                }
                fprintf(stdout, "New Connection established.\n");
                handleConnection(clientfd);
                close(clientfd);
        }
        close(socketfd);
        return 0;
}