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
    bind(socketfd,(struct sockaddr*)&serveradd,sizeof(struct sockaddr));
    listen(socketfd,128);
    fd_set monitor;
    struct sockaddr_in clientaddr;
   // bzero(&clientaddr,sizeof(clientaddr));
   // socklen_t len=sizeof(clientaddr);
   // int new_fd = accept(socketfd,(struct sockaddr*)&clientaddr,&len);
   // printf("得到客户端的ip是%s,端口号是:%d\n",
    //       inet_ntoa(clientaddr.sin_addr),
     //      ntohs(clientaddr.sin_port));
    char buffer[128]={0};
    fd_set set;
    int ret;
    FD_ZERO(&monitor);
    FD_SET(STDIN_FILENO,&monitor);
    FD_SET(socketfd,&monitor);
    int new_fd;
    while(1){
        FD_ZERO(&set);
        memcpy(&set,&monitor,sizeof(monitor));
        if(select(10,&set,NULL,NULL,NULL)>0){
            if(FD_ISSET(socketfd,&set)){
                printf("客户端与服务器已经连接上\n");
                bzero(&clientaddr,sizeof(clientaddr));
                socklen_t len=sizeof(clientaddr);
                new_fd = accept(socketfd,(struct sockaddr*)&clientaddr,&len);
                printf("客户端的ip是%s,端口号是:%d\n",
                       inet_ntoa(clientaddr.sin_addr),
                       ntohs(clientaddr.sin_port));
                FD_SET(new_fd,&monitor);
            }
            if(FD_ISSET(new_fd,&set)){
                bzero(buffer,sizeof(buffer));
                ret= recv(new_fd,buffer,sizeof(buffer),0);
                if(ret){
                    printf("客户端:%s\n",buffer);

                }
                else{
                    printf("客户端已断开\n");
                    FD_CLR(new_fd,&monitor);
                    continue;
                }
            }               
            if(FD_ISSET(STDIN_FILENO,&set)){
                bzero(buffer,sizeof(buffer));
                read(STDIN_FILENO,buffer,sizeof(buffer));
                send(new_fd,buffer,strlen(buffer)-1,0);
            }
        }   
    }
    close(socketfd);
    return 0;
}
