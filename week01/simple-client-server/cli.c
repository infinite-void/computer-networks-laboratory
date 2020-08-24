#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>

int main()
{
int sockfd, n = 0;
char recvBuff[1024];
struct sockaddr_in serv_addr;


 if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
      printf("\n Socket not created \n");
      return 1;
    }
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5000);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
 
  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
      printf("\n Error : Connect Failed \n");
      return 1;
    }
  while((n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
      recvBuff[n] = 0;
      if(fputs(recvBuff, stdout) == EOF)
    {
      printf("\n Error : Fputs error");
    }
      printf("\n");
    }
 
  if( n < 0)
    {
      printf("\n Read Error \n");
    }
 
  return 0;

}
