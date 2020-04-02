#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h>

static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  new = old; /* make new settings same as old settings */
  new.c_lflag &= ~ICANON; /* disable buffered i/o */
  new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}

#define PORT 8080
int game = 0;

void func (int sockfd) {

    char buffer[1024] = {0};
    char cek[100];
    int tem;
    int heal;

    for (;;) {
        bzero(buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));
        // printf("buff2 %s buff2\n",buffer);
        while(!(strncmp(buffer,"wait",4))) {
            printf("Waiting for player ...\n");
            bzero(buffer, sizeof(buffer));
            sleep(1);
            read(sockfd, buffer, sizeof(buffer));
        }
        int hit = 0;
        char c,spasi = ' ';
        while(!(strncmp(buffer,"play",4))) {
            if(hit == 0)
            printf("Game started\n");
            // while(  && !(strncmp(buffer,"play",4))){
                // printf("buffer %s\n apaloooooo\n",buffer);
                c = getch();
                if(c == ' '){
                    send( sockfd," ", 1, 0 );
                    printf("ok");
                    hit++;
                    // send( sockfd, "hit", 3, 0 );    
                    // }
                }
        }
        // if(hit >= 10){
        // printf("endgamemm\n");
        // send( sockfd, "endgame", 7, 0 );
        // }
        // if(!(strncmp(buffer,"endgame",7)))
        //     continue;

        

        printf("%s", buffer);
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