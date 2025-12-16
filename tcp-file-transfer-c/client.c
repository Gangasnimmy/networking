#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(){
    //create a client socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd < 0){
        perror("Socket creation failed");
        return -1;
    }
    //construct server address
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
    //addr.sin_addr.s_addr = INADDR_ANY;
    
    //connect to server
    if(connect(client_fd,(struct sockaddr*)&addr,sizeof(addr)) == -1){
        perror("TCP connection failed");
        close(client_fd);
        return -1;
    }
    //open a file to read
    int fd = open("data.txt",O_RDONLY);
    if(fd < 0){
        perror("File open failed");
        return -1;
    }
    //get total bytes of the file
    struct stat st;
    fstat(fd,&st);
    off_t size = st.st_size;
    printf("File size : %ld bytes\n", size);

    //loop through to read from the file and store it in a buffer to send
    char buff[1024];
    ssize_t bytes_read;
    while((bytes_read = read(fd,buff,sizeof(buff))) > 0){
        ssize_t total_sent = 0;
        while(total_sent < bytes_read){
            ssize_t sent = send(client_fd,buff+total_sent,bytes_read - total_sent, 0);
            if(sent < 0){
                perror("TCP send failed");
                close(fd);
                close(client_fd);
                return -1;
            }
            total_sent += sent;
        }
    }

    if(bytes_read == 0){
        printf("File sent successfully\n");
    }

    if(bytes_read < 0){
        perror("read failed : ");
    }
    //cleanup
    shutdown(client_fd, SHUT_WR);
    close(fd);
    close(client_fd);

    return 0;
}