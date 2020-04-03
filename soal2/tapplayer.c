#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <termios.h>

static struct termios old, new;
pthread_t tid1, tid2;

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
char c,spasi = ' ';
int win = 0, dead = 0,play = 0;
char buffer[1024] = {0};
int hit = 0;

void* tulis(void *arg)
{
    int sockfd = *(int *)arg;
    c = getch();
    if(c == ' '){
        send( sockfd,"hit", 3, 0 );
        printf("ok");
        hit++;
    }
}

void* baca(void *arg)
{
    while(!dead && !win){

        int sockfd = *(int *)arg;
        bzero(buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));
        // printf("win = %d\ndead = %d\nplay = %d\n",win,dead,play);
        if(!(strncmp(buffer,"win",3))){
            win = 1;
            // play = 0;
        }
        else if(!(strncmp(buffer,"dead",4))){
            dead = 1;
            // play = 0;
        }
        printf("%s\n",buffer);
    }
}

void func (int sockfd) {

    char cek[100];
    int tem;
    int heal;

    for (;;) {
        if(play == 1){
            printf("Game started\n");

            pthread_create(&(tid2), NULL, baca, &sockfd);
            while(!dead && !win){
                c = getch();
                if(c == ' '){
                    send( sockfd,"hit", 3, 0 );
                    // printf("ok");
                    hit++;
                }
            }
            pthread_join(tid2,NULL);
            play = 0;
        }
        else{
            printf("win = %d\nlose = %d\n",win,dead);
            bzero(buffer, sizeof(buffer));
            read(sockfd, buffer, sizeof(buffer));
            // printf("buff2 %s buff2\n",buffer);
            while(!(strncmp(buffer,"wait",4))) {
                printf("Waiting for player ...\n");
                bzero(buffer, sizeof(buffer));
                sleep(1);
                read(sockfd, buffer, sizeof(buffer));
            }

            if(!(strncmp(buffer,"play",4))) {
                play = 1;
            }
            if(win == 0 && dead == 0 && play == 0 ){

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
            win = 0;dead = 0;
        }
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