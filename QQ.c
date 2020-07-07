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
#include<signal.h>
typedef struct newno{
    char name[128];
    int new_fd;
    struct newno *next;
}list,*li;
li tree=NULL,tail=NULL;
void signal_handel(int signal){
    printf("%d\n",signal);
}
int main(int argv,char* argc[]){
    if(argv!=3){
        perror("参数错误\n");
        return -1;
    }
    printf("记录聊天记录\n");
    signal(SIGINT,signal_handel);
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
    char buffer[128]={0};
    fd_set set;
    int ret;
    FD_ZERO(&monitor);
    FD_SET(STDIN_FILENO,&monitor);
    FD_SET(socketfd,&monitor);
    int new_fd;
   // li tree=NULL,tail=NULL;
    while(1){
        FD_ZERO(&set);
        memcpy(&set,&monitor,sizeof(monitor));
        if(select(128,&set,NULL,NULL,NULL)>0){
            if(FD_ISSET(socketfd,&set)){
              //  printf("你已经与服务器连接上\n");
                char name[128]={0};              
                bzero(&clientaddr,sizeof(clientaddr));
                socklen_t len=sizeof(clientaddr);
                new_fd = accept(socketfd,(struct sockaddr*)&clientaddr,&len);
                char word[]="请输入你的用户名\n";
                send(new_fd,word,strlen(word)-1,0);
                recv(new_fd,buffer,sizeof(buffer),0);
                strcpy(name,buffer);
                bzero(buffer,sizeof(buffer));
                printf("客户端的ip是%s,端口号是:%d,用户名:%s\n",
                       inet_ntoa(clientaddr.sin_addr),
                       ntohs(clientaddr.sin_port),name);
                li newnode=(li)malloc(sizeof(list));
                bzero(newnode->name,sizeof(newnode->name));
                strcpy(newnode->name,name);
                newnode->new_fd=new_fd;
                if(!tree){
                    tree=newnode;
                    tail=newnode;
                    tail->next=NULL;
                }
                else{
                    tail->next=newnode;
                    tail=tail->next;
                }
                FD_SET(tail->new_fd,&monitor);
            }
            li temp1=tree,temp2=tree,temp3=tree;
            while(temp1!=NULL||tree==NULL){
                if(FD_ISSET(temp1->new_fd,&set)){
                    bzero(buffer,sizeof(buffer));
                    ret= recv(temp1->new_fd,buffer,sizeof(buffer),0);
                    if(ret){
                        printf("1111111\n");
                        printf("%s\n",buffer);
                        while(temp2!=NULL){
                            if(temp2==temp1){
                         //      printf("跳过\n");
                            }
                           else{ 
                          // printf("%s\n",temp2->name);
                            char buf[128];
                            bzero(buf,sizeof(buf));
                            strcat(buf,temp1->name);
                            strcat(buf,":");
                            strcat(buf,buffer);
                            send(temp2->new_fd,buf,strlen(buf),0);
                         }
                            temp2=temp2->next;
                        }
                        temp2=tree;               
                    }
                    else{
                        printf("客户端%s已断开\n",temp1->name);    
                        //断开则要从链表中删除该节点
                        if(temp1==tree){  //如果删除的是头结点
                            tree=tree->next;
                            FD_CLR(temp1->new_fd,&monitor);
                            free(temp1);
                            temp1=tree;
                        }
                        else{
                            temp3->next=temp1->next;
                            FD_CLR(temp1->new_fd,&monitor);
                            free(temp1);
                            temp1=temp3->next;
                        }
                        if(temp3->next==NULL){
                            tail=temp3;
                        }
                        continue;
                    }
                }
                temp3=temp1;
                temp1=temp1->next;
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
