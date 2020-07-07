#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include<netinet/in.h> 
#include<sys/wait.h>
#include<arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include<time.h>
int main(int argv,char* argc[]){
    if(argv!=3){
        perror("参数错误\n");
        return -1;
    }
    int socketfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serveradd;
    bzero(&serveradd,sizeof(serveradd));
    serveradd.sin_family=AF_INET;
    serveradd.sin_addr.s_addr=inet_addr(argc[1]);
    serveradd.sin_port=htons(atoi(argc[2]));
    printf("请输入你的用户名\n");
    connect(socketfd,(struct sockaddr*)&serveradd,sizeof(struct sockaddr));
    char buf[128]={0};
    recv(socketfd,buf,sizeof(buf),0);
   // printf("%s\n",buf);
    bzero(buf,sizeof(buf));
    read(STDIN_FILENO,buf,sizeof(buf));
    send(socketfd,buf,strlen(buf)-1,0);
    char buffer[128]={0};
    fd_set monitor;
    while(1){
        FD_ZERO(&monitor);
        FD_SET(STDIN_FILENO,&monitor);
        FD_SET(socketfd,&monitor);
        if(select(socketfd+1,&monitor,NULL,NULL,NULL)>0){
            if(FD_ISSET(socketfd,&monitor)){
                bzero(buffer,sizeof(buffer));
                int ret= recv(socketfd,buffer,sizeof(buffer),0);
                if(ret){
                    printf("%s\n",buffer);
                }
                else{
                   printf("服务器已经断开\n");
                   bzero(buffer,sizeof(buffer));
                   break;
                }
            }
            if(FD_ISSET(STDIN_FILENO,&monitor)){
                bzero(buffer,sizeof(buffer));
                read(STDIN_FILENO,buffer,sizeof(buffer));
                time_t mytime;
                time(&mytime);
                char temp[200]={0};
                bzero(temp,sizeof(temp));
                ctime_r(&mytime,temp);
                strcat(temp,buffer);
                send(socketfd,temp,strlen(temp)-1,0);
            }
        }
    }
    close(socketfd);
    return 0;
}
