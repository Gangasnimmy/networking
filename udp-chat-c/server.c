#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENT 5
#define NAME_LEN 32
#define MAX_MSG_LEN 1024
#define LOG_NAME "Server"

typedef enum {
    JOIN,
    LEAVE,
    MSG,
    UNKNOWN
} msg_type_t;

typedef struct {
    struct sockaddr_in addr;
    char name[NAME_LEN];
    bool active;
} client_t;

client_t clients[MAX_CLIENT] = {0};
int client_count = 0;

msg_type_t parseType(const char *cmd) {
    if(!cmd)
        return UNKNOWN;

    if (strcmp(cmd, "JOIN") == 0)  return JOIN;
    if (strcmp(cmd, "LEAVE") == 0) return LEAVE;
    if (strcmp(cmd, "MSG") == 0) return MSG;
    return UNKNOWN;
}

bool isSameByAddress(const struct sockaddr_in* a, const struct sockaddr_in* b){
    if(!a || !b)
        return false;

    if(a->sin_port == b->sin_port && a->sin_addr.s_addr == b->sin_addr.s_addr)
        return true;

    return false;
}

int isClientExist(const struct sockaddr_in* addr){
    if(!addr){
        printf("[%s] Received address is NULL.\n",LOG_NAME);
        return -1;
    }

    for(int i=0;i<MAX_CLIENT;i++){
        if(clients[i].active){
            if(isSameByAddress(&clients[i].addr,addr)){
                return i;
            }
        }
    }
    return -1;
}

int findFreeSlot(){
    for(int i=0;i<MAX_CLIENT;i++){
        if(!clients[i].active)
            return i;
    }
    return -1;
}

bool addClient(struct sockaddr_in* addr, char* name){  

    bool response = false;

    if(!addr || !name){
        printf("[%s] Invalid arguments..\n",LOG_NAME); 
        return response;
    }

    if(isClientExist(addr) < 0){
        if(client_count < MAX_CLIENT){
            int i = findFreeSlot();
            if(i >= 0){
                clients[i].addr = *addr;
                snprintf(clients[i].name, sizeof(clients[i].name), "%s", name);
                clients[i].active = true;
                client_count++;
                response = true;
                printf("[%s] %s joined.\n",LOG_NAME, name);
            } else {
                printf("[%s] No free slot available.\n",LOG_NAME);
            }
        } else {
            printf("[%s] Max client count reached.\n",LOG_NAME);
        }
    } else {
        printf("[%s] client already exist.\n",LOG_NAME);
    }                 
    return response;           
}

void removeClient(int index){
    char userName[NAME_LEN];
    strcpy(userName,clients[index].name);
    memset(&clients[index].addr, 0, sizeof(clients[index].addr));
    clients[index].name[0] = '\0';
    clients[index].active = false;
    if(client_count > 0)
        client_count--;
    printf("[%s] %s removed.\n",LOG_NAME,userName);
    
}

void sendAcknowledge(int sock, struct sockaddr_in* addr, char* msg){
    if(sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)addr, sizeof(*addr)) == -1){
        perror("Failed to send Acknowlege.\n");
    }
}

void broadcastMsg(int sock, struct sockaddr_in* addr, char* msg, bool flag){

    int val = -1;

    if(!msg){
        printf("[%s] Message is NULL.\n",LOG_NAME);
        return;
    }
    if(!flag)
        val = isClientExist(addr);
    if((val >= 0 && addr != NULL) || (addr == NULL && val == -1)){
        printf("Inside if case.../n");
        char buffer[MAX_MSG_LEN];
        if(flag)
            strcpy(buffer,msg);
        else
            snprintf(buffer, MAX_MSG_LEN, "%s: %s",clients[val].name,msg);
        //printf("Broadcasting...%s\n",buffer);
        for(int i=0;i<MAX_CLIENT;i++){
            if(i != val && clients[i].active){
                //printf("Sending message to %s",clients[i].name);
                if(sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr*)&clients[i].addr,sizeof(clients[i].addr)) == -1){
                    printf("[%s] Failed to send message to %s.\n",LOG_NAME,clients[i].name);
                }
            }
        }
    } else {
        printf("[%s] Unknown client. Not broadcasting the message.\n",LOG_NAME);
    }
}

int main(){
    int serverSock;
    char buffer[1024];
    fd_set rfds;
    struct sockaddr_in serverAddr; 
    int opt = 1;

    serverSock = socket(AF_INET,SOCK_DGRAM,0);
    if(serverSock < 0){
        perror("Socket creation failed\n");
        return -1;
    }

    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if(setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
        perror("set option failed");
        return -1;
    }

    if(bind(serverSock,(struct sockaddr*)&serverAddr,sizeof(serverAddr)) < 0){
        perror("bind failed");
        return -1;
    }

    printf("UDP server listening on port 9000...\n");

    while(1){
        FD_ZERO(&rfds);
        FD_SET(serverSock,&rfds);
        int retVal = select(serverSock + 1, &rfds, NULL, NULL, NULL);
        
        printf("select woke up with %d\n",retVal);

        if(retVal == -1){
            perror("select()");
        } else if(retVal == 0){
            printf("No data. Timedout\n");
        } else {
            if(FD_ISSET(serverSock,&rfds)){
                struct sockaddr_in fromAddr;
                memset(&fromAddr,0,sizeof(fromAddr));
                socklen_t addr_len = sizeof(fromAddr);
                ssize_t n = recvfrom(serverSock, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&fromAddr, &addr_len);
                printf("Message received from %s:%d\n", inet_ntoa(fromAddr.sin_addr), ntohs(fromAddr.sin_port));

                if(n>0){
                    buffer[n] = '\0';
                    char* msg_type = strtok(buffer," \n");
                    if(msg_type){
                        printf("[%s] Received %s request\n",LOG_NAME,msg_type);
                        switch(parseType(msg_type)){
                            case JOIN:      
                                        char* name = strtok(NULL,"\n");
                                        if(name)
                                            if(addClient(&fromAddr,name)){
                                                char message[MAX_MSG_LEN];
                                                snprintf(message,sizeof(message),"\t\t%s joined the chat",name);
                                                sendAcknowledge(serverSock, &fromAddr,"You Joined");
                                                broadcastMsg(serverSock, &fromAddr, message, 1);
                                            } else { 
                                                printf("[%s] Failed to add client.\n",LOG_NAME); 
                                                sendAcknowledge(serverSock, &fromAddr,"Unable to join. Try again...");
                                            }
                                        else
                                            printf("[%s] Failed to extract client name.\n",LOG_NAME);
                                        break;
                            case LEAVE: 
                                        char userName[NAME_LEN];
                                        char message[MAX_MSG_LEN];
                                        int i = isClientExist(&fromAddr); 
                                        if(i >= 0){
                                            strcpy(userName,clients[i].name);
                                            removeClient(i);
                                            snprintf(message,sizeof(message),"\t\t%s left the chat",userName);
                                            sendAcknowledge(serverSock, &fromAddr,"\t\tYou left");
                                            broadcastMsg(serverSock, NULL, message, 1);
                                        } else {
                                            printf("[%s] Client doesn't exist.\n",LOG_NAME);
                                        }
                                        break;
                            case MSG:  
                                        char* msg = strtok(NULL,"\n");
                                        if(msg)
                                            broadcastMsg(serverSock, &fromAddr,msg, 0);
                                        else
                                            printf("[%s] Empty message. Ignored.\n",LOG_NAME);
                                        break;
                            case UNKNOWN:   
                                        printf("[%s] Unknown message. Ignored.\n",LOG_NAME);
                        }
                    }
                }
            }
        }
        
    }
    close(serverSock);
    return 0;
}