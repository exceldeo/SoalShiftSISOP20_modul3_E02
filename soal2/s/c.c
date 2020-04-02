#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080

void create_call_login(char buffer[], int sock, int valread) {
char username[200] = {0};
char password[100] = {0};
printf("Username : ");
scanf("%s", username);
printf("Password : ");
scanf("%s", password);
strcat(username, "|");
strcat(username, password);

// printf(" username %s",username);
send(sock , username , strlen(username) , 0 );
valread = read(sock , buffer, 1024);
printf("%s\n", buffer);
}

void create_call_register(char buffer[], int sock, int valread) {

}

int main() {

struct sockaddr_in address;
int sock = 0, valread;
struct sockaddr_in serv_address;
int command;
char buffer[1024] = {0};
if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
printf("\n Socket creation error \n");
return -1;
}

memset(&serv_address, '0', sizeof(serv_address));

serv_address.sin_family = AF_INET;
serv_address.sin_port = htons(PORT);

if(inet_pton(AF_INET, "127.0.0.1", &serv_address.sin_addr)<=0) {
printf("\nInvalid address/ Address not supported \n");
return -1;
}

if (connect(sock, (struct sockaddr *)&serv_address, sizeof(serv_address)) < 0) {
printf("\nConnection Failed \n");
return -1;
}
while(1) {
printf("1. Login\n2. Register\nChoices : ");
scanf("%d", &command);
if (command == 1){
send( sock, buffer, sizeof(buffer), 0 );
create_call_login(buffer, sock, valread);
}else{
create_call_register(buffer, sock, valread);
}
}
return 0;
}