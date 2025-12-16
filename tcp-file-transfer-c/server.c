#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

int main(){

    int server_fd = socket(AF_INET,SOCK_STREAM,0);

    if(server_fd < 0){
        perror("socket creation failed : ");
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);

    //int opt = 1;
    //setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(server_fd,(struct sockaddr*)&addr,sizeof(addr)) == -1){
        perror("Bind failed");
        close(server_fd);
        return -1;
    }

    if(listen(server_fd,5) == -1){
        perror("Listen failed");
        close(server_fd);
        return -1;
    }

    printf("Server listening to port 8000\n");

    struct sockaddr_in clientAddr;
    memset(&clientAddr,0,sizeof(clientAddr));
    socklen_t addrLen = sizeof(clientAddr);
    int clientSock = accept(server_fd,(struct sockaddr*)&clientAddr, &addrLen);
    if(clientSock == -1){
        perror("accept failed");
        close(server_fd);
        return -1;
    }

    printf("connection accepted from client\n");

    //open or create a file
    int fd = open("received.txt",O_CREAT | O_WRONLY, 0644);
    if(fd < 0){
        perror("Failed to open/create file : ");
        close(server_fd);
        close(clientSock);
        return -1;
    }
    //loop through to receive the data and write it in a file
    char buffer[1024];
    ssize_t n;
    while((n = recv(clientSock,buffer,sizeof(buffer)-1,0)) > 0){

        buffer[n] = '\0';
        ssize_t written = 0;
        while(written < n){
            ssize_t w = write(fd,buffer + written, n - written);
            if(w < 0){
                perror("File write failed : ");
                close(fd);
                close(clientSock);
                close(server_fd);
                return -1;
            }
            written += w;
        }  
        
    }

    if(n == 0)
        printf("client sent all data\n");

    if(n < 0){
        perror("recv failed : ");
    }

    close(clientSock);
    close(server_fd);
    close(fd);

    return 0;
}