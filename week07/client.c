#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 4040

int main() {
        int socketfd = 0;
        struct sockaddr_in host_addr;
        socklen_t length = sizeof(struct sockaddr_in);

        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in creating socket.\n");
                return -1;
        }

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(PORT);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

        if(connect(socketfd, (struct sockaddr*)&host_addr, length) < 0) {
                fprintf(stderr, "Error in connecting to server.\n");
                return -1;
        }
        fprintf(stdout, "Connection established.\n");

        float rtt, delay;
        char input[128];
        fprintf(stdout, "Enter Round Trip Time : ");
        scanf("%f", &rtt);
        fprintf(stdout, "Enter Uniform Delay : ");
        scanf("%f", &delay);
        fprintf(stdout, "Enter Message : ");
        scanf("%s", input);
        int value = (int)(rtt / delay), trips = 0;
        float totalRTT = 0;
        send(socketfd, &value, sizeof(int), 0);
        int i = 0, packet = 1;
        while(i < strlen(input)) {
                
                int j, temp;
                char msg[2], recvbuff[10];
                fprintf(stdout, "Sending : ");
                for(j = 0; j < packet && i < strlen(input); j++, i++) {
                        msg[0] = input[i];
                        msg[1] = '\0';
                        fprintf(stdout, "%s", msg);
                        send(socketfd, msg, sizeof(msg), 0);
                }
                if(i >= strlen(input))
                        send(socketfd, "$", strlen("$") + 1, 0);

                fprintf(stdout, "\nStream sent. %d packets.\n\n", j);
                packet = value;

                char ackdata[10];
                temp = sprintf(ackdata, "ACK %d", j);
                ackdata[temp] = '\0';
                
                recv(socketfd, recvbuff, sizeof(recvbuff), 0);
                if(strncmp(recvbuff, ackdata, strlen(ackdata)) == 0) {
                        fprintf(stdout, "Acknowledgement Received for %d packets.\n\n", atoi(&recvbuff[4]));
                        totalRTT += rtt;
                        trips += 1;
                }
                
        }
        fprintf(stdout, "Total Round Trips : %d\n Total Time : %f.\n", trips, totalRTT);
        return 0;
}