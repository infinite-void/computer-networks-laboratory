#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>

int main()
{
int listenfd = 0, connfd = 0;
struct sockaddr_in serv_addr;
char sendBuff[1025]; 


 if((listenfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
      printf("\n Socket not created\n");
      return 1;
    }

  serv_addr.sin_family = AF_INET;    
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  serv_addr.sin_port = htons(5000);

  if((bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)))>0)
    {
      printf("\n Not binded \n");
      return 1;
    }	

  if(listen(listenfd, 10) == -1){
      printf("Failed to listen\n");
      return -1;
  }

 while(1)
    {      
      connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL); // accept awaiting request
       
      strcpy(sendBuff, "Message from server--Hello");
      write(connfd, sendBuff, strlen(sendBuff));
      close(connfd);    
      sleep(1);
    } 
 
  return 0;
 
}
