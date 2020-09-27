#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#define ROOTPORT 4041
extern int errno;
int main() {
        int socketfd = 0, clientfd = 0, length = 0, sentbytes, recvbytes;
        struct sockaddr_in host_addr, client_addr;
        char buffer[20];

        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                return -1;
        }

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(ROOTPORT);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

        if(bind(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
                fprintf(stderr, "Error in binding port to socket.\n");
                return -1;
        }
        
        while(1) {
                recvbytes = recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &length);

                fprintf(stdout, "%s %d\n", buffer, length);
                FILE* fd = fopen("rootdns.txt", "r");
                if(!fd) {
                        fprintf(stderr, "Could not access DNS records.\n");
                        sendto(socketfd, "ERROR", strlen("ERROR") + 1, 0, (struct sockaddr*)&client_addr, length);
                        continue;
                }
                char linebuff[40], filebuff[400], ip[20], tempbuff[40], lastbuff[40];
                char *temp, *iptemp;
                int flag = 0, i;
                linebuff[0] = '\0';
                lastbuff[0] = '\0';
                filebuff[0] = '\0';
                ip[0] = '\0';
                while( fgets(linebuff, sizeof(linebuff), fd) ) {
                        strcpy(tempbuff, linebuff);
                        temp = strtok(tempbuff, " ");
                        if(flag == 0 && strncmp(temp, buffer, strlen(temp)) == 0) {
                                flag = 1;
                                strcpy(lastbuff, linebuff);
                                iptemp = strtok(NULL, "\n");
                                for(i = 0; *iptemp != '\0'; i++, iptemp++)
                                        ip[i] = *iptemp;
                                ip[i] = '\0';
                        }
                        else {
                                strcat(filebuff, linebuff);
                        }
                }
                fclose(fd);
                if(flag == 0) {
                        sentbytes = sendto(socketfd, "404", strlen("404") + 1, 0, (struct sockaddr*)&client_addr, length);
                }
                else {
                        int fdes = open("rootdns.txt", O_WRONLY);
                        strcat(filebuff, lastbuff);
                        write(fdes, filebuff, strlen(filebuff));
                        close(fdes);
                        fprintf(stdout, "%s %d %ld\n", ip, length, sizeof(struct sockaddr));

                        sentbytes = sendto(socketfd, ip, strlen(ip) + 1, 0, (struct sockaddr*)&client_addr, length);
                        perror("Errro: ");
                        fprintf(stdout, "%d %d\n", sentbytes, errno);
                }

        }
        close(socketfd);
        return 0;
}