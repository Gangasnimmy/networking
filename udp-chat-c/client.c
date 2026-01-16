#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX(A,B)    ( A > B ? A : B)
#define MAX_MSG_LEN 1024
#define NAME_LEN    32

int main(){
    int clientSock;
    char buffer[MAX_MSG_LEN];
    char userName[NAME_LEN];
    fd_set rfds;
    
    clientSock = socket(AF_INET,SOCK_DGRAM,0);
    if(clientSock < 0){
        perror("Socket creation failed :");
        return -1;
    }
    
    struct sockaddr_in serverAddr, clientAddr;
    memset(&clientAddr,0,sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(0);
    clientAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(clientSock, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) == -1) {
        perror("client bind failed");
        return -1;
    }

    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    inet_pton(AF_INET,"127.0.0.1",&serverAddr.sin_addr);
    socklen_t addr_len = sizeof(serverAddr);

    printf("\t\t -- Welcome to the chat --\nEnter your name to join and 'quit' to leave the chat.\n");
    printf("Enter name: ");
    fgets(userName,sizeof(userName),stdin);
    snprintf(buffer,sizeof(buffer),"JOIN %s",userName);
    if(sendto(clientSock, buffer, strlen(buffer), 0, (struct sockaddr*)&serverAddr, addr_len) == -1){
        perror("Join request Failed.");
        return -1;
    }

    while(1){
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(clientSock,&rfds);
        int nfds = MAX(STDIN_FILENO,clientSock) + 1;
        char message[MAX_MSG_LEN];
        int retVal = select(nfds,&rfds,NULL,NULL,NULL);
        //printf("select woke up with %d\n",retVal);
        if(retVal == -1){
            perror("select()");
        } else if(retVal == 0){
            printf("No data. Timedout.\n");
        } else {
            if(FD_ISSET(clientSock,&rfds)){
                memset(buffer,0,sizeof(buffer));
                ssize_t n = recvfrom(clientSock,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&serverAddr,&addr_len);
                if(n>0){
                    //buffer[strcspn(buffer,"\n")] = '\0';
                    printf("%s\n",buffer);
                }   
            }

            if(FD_ISSET(STDIN_FILENO,&rfds)){
                memset(buffer,0,sizeof(buffer));
                fgets(buffer, sizeof(buffer), stdin);
                if(buffer[0] != '\n'){
                    buffer[strcspn(buffer,"\n")] = '\0';
                    if(strcmp(buffer,"quit") == 0)
                        strcpy(message,"LEAVE");
                    else
                        snprintf(message,sizeof(message),"MSG %s",buffer);
                    printf("You: %s \n",buffer);
                    if(sendto(clientSock, message, strlen(message), 0,(struct sockaddr*)&serverAddr,addr_len) == -1)
                        perror("Message Failed\n");
                } else {
                    printf("Type something to send\n");
                }
            }
        } 
    }

    close(clientSock);
    return 0;
}