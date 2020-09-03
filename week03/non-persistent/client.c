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

int main() {
        int socketfd = 0, i;
        struct sockaddr_in host_addr;
        char temp[5], filename[20], request[1024], response[1024];

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(PORT);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);
        
        for(i = 0; i < 5; i++) {
                socketfd = socket(AF_INET, SOCK_STREAM, 0);
                if(socketfd < 0) {
                        fprintf(stderr, "Error in socket creation.\n");
                        return -1;
                }

                if(connect(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
                        fprintf(stderr, "Error in connecting to server.\n");
                        return -1;
                }
                fprintf(stdout, "****************** CONNECTION ESTABLISHED ******************\n");
                request[0] = '\0';
                filename[0] = '\0';
                strcat(filename, "file");
                sprintf(temp, "%d", i + 1);
                strcat(filename, temp);
                strcat(filename, ".html");
                strcat(request, "GET ");
                strcat(request, filename);
                strcat(request, " HTTP/1.1 \nHost: www.annauniv.edu\nConnection: keep-alive\nUpgrade-Insecure-Requests: 1\nUser-Agent: Mozilla/5.0 (Windows NT 6.2; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/84.0.4147.135 Safari/537.36\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\nAccept-Encoding: gzip, deflate\nAccept-Language: en-US,en;q=0.9\nCookie: _ga=GA1.2.90932385.1550119096; \n");//__utma=97656655.90932385.1550119096.1552536021.1595047413.2; __utmz=97656655.1595047413.2.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none)\n"); 
                fprintf(stdout, "---------- SENDING REQUEST -----------------\n");
                send(socketfd, request, strlen(request) + 1, 0);
                fprintf(stdout, "---------- REQUEST SENT -----------------\n");
                fprintf(stdout, "---------- RESPONSE RECEIVED ---------------\n");
                recv(socketfd, response, sizeof(response), 0);
                fprintf(stdout, "%s\n", response);
                fprintf(stdout, "---------- END OF RESPONSE -----------------\n");
                send(socketfd, "EXIT", sizeof("EXIT") + 1, 0);
                close(socketfd);
                fprintf(stdout, "****************** CONNECTION TERMINATED ******************\n");
        }
        return 0;
}