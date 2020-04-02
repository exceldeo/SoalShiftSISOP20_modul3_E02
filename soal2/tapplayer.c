#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
int game = 0;

void func (int sockfd) {

    char buffer[1024] = {0};
    char cek[100];
    int tem;

    for (;;) {
        bzero(buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));
        printf("%s", buffer);
        if(!(strncmp(buffer,"game",4)))
            game = 1;
        if(!(strncmp(buffer,"endgame",7)))
            game = 0;

        bzero(buffer, sizeof(buffer));
        // scanf("%[^\n]s",buffer);
        int n = 0;
        char com;
        while ((com = getchar()) != '\n'){
            if(com != '\n')
                buffer[n] = com;
            n++;
        }

        
        
        send( sockfd, buffer, sizeof(buffer), 0 );
    }
}

int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int new_socket = 0;
    struct sockaddr_in serv_addr;

    if ((new_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/Address not supported \n");
        return -1;
    }

    if (connect(new_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    func(new_socket);

    return 0;
}