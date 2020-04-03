#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080

int id_socket[2],id_heal[2]={100,100};
int player = 0, game = 0;
pthread_t tid[100],tid2[2];
char heal[10];



void* playandcount(void *arg)
{
char username[100], *filename;
char buffer[1024] = {0};
char *cek1 = "1. Login\n2. Register\n   Choices : ", *cek2 ="1. Find Match\n2. Logout\n   Choices : ";
int flag = 0;

    int new_socket = *(int *)arg;

    // printf("new_socket %d\n", new_socket);

    // int new_socket = *(int*) arg;
    // printf("thread %d\n",new_socket);
    FILE * fp,*fo;
    int flag2 = 1;
    if((fp = fopen ( "/home/excel/Desktop/SoalShiftSISOP20_modul3_E02/soal2/database.txt", "r" ) ) == NULL)
        flag2 = 0;
    fclose(fp);
    if(flag2 == 0){
        fp = fopen ( "/home/excel/Desktop/SoalShiftSISOP20_modul3_E02/soal2/database.txt", "w+" );
        fclose(fp);
    }
    int health = 100;
    // printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
    // printf("new socket %d\n",new_socket);

    for(;;) {
        // printf("game %d\n",game);
        if(game == 1){
            // if
            bzero(buffer, sizeof(buffer));
            read(new_socket, buffer, 1024);
            // printf("nwe sokt %d, bufdd %s\n",new_socket,buffer);
            if (!(strncmp(buffer,"hit",3))){
            // int
                // printf("new socket1 %d\nbuffer %s\n-----\nheal1 = %d\nheal2 = %d\n",new_socket,buffer,id_heal[1],id_heal[0]);
                if(new_socket == id_socket[1]){
                    id_heal[0] = id_heal[0] - 10;
                    sprintf(heal,"%d",id_heal[0]);
                    bzero(buffer, sizeof(buffer));
                    // printf("*****\nbuffer2 %s\n*****\n",buffer);
                    strcpy(buffer,"heal ");
                    strcat(buffer,heal);
                    // printf("*****\nbuffer2 %s\n*****\n",buffer);
                    send( id_socket[0], buffer, sizeof(buffer), 0);
                }
                else if(new_socket == id_socket[0]){
                    id_heal[1] = id_heal[1] - 10;
                    sprintf(heal,"%d",id_heal[1]);
                    bzero(buffer, sizeof(buffer));
                    // printf("+++++\nbuffer1 %s\n+++++\n",buffer);
                    strcpy(buffer,"heal ");
                    strcat(buffer,heal);
                    // printf("+++++\nbuffer1 %s\n+++++\n",buffer);
                    send( id_socket[1], buffer, sizeof(buffer), 0);
                }
                else
                    printf("error\n");


                if(id_heal[1] <= 0 || id_heal[0] <= 0 ) {
                    if((new_socket == id_socket[0] && id_heal[0] <= 0) || (new_socket == id_socket[1] && id_heal[1] >= 0)){
                        bzero(buffer, sizeof(buffer));
                        send( id_socket[1], "win", 3, 0);
                        bzero(buffer, sizeof(buffer));
                        send( id_socket[0], "dead", 4, 0);
                    }
                    else{
                        bzero(buffer, sizeof(buffer));
                        send( id_socket[0], "win", 3, 0);
                        bzero(buffer, sizeof(buffer));
                        send( id_socket[1], "dead", 4, 0);
                    }
                    player = 0;
                    game = 0;
                    // id_heal[1]=100;
                    // id_heal[0]=100;
                // int
                    // send( new_socket, "end", 7, 0);
                }
                // printf("new socket1 %d\nbuffer %s\n-----\n",new_socket,buffer);
            }
        }
        else{
            // printf("flag = %d\n",flag);
            bzero(buffer, sizeof(buffer));
            if(flag == 0 && game == 0 )
            strcpy(buffer,cek1);
            else if(flag == 1 && game == 0)
            strcpy(buffer,cek2);
            // printf("buffer 1 -> %s\n",buffer);
            send ( new_socket, buffer, sizeof(buffer), 0) ;

            bzero(buffer, sizeof(buffer));
            read(new_socket, buffer, 1024);
            // if(!(strncmp(buffer,"hit",3)))
            //     // printf("buffer 2 -> - \n");
            // else
            // printf("buffer 5 ->%s\n",buffer);
            // printf("%d",strcmp(buffer,"register"));
            if(!(strncmp(buffer,"login",5))){
                // printf("2\n");
                char line[160];
                send ( new_socket, "   Username : ", 14, 0);
                bzero(buffer, sizeof(buffer));
                read(new_socket, buffer, 1024);
                // printf("aa = %s\n",buffer);
                fp = fopen ("/home/excel/Desktop/SoalShiftSISOP20_modul3_E02/soal2/database.txt","r");
                strcpy(username,buffer);
                // printf("dd = %s\n",username);
                send( new_socket, "   Password : ", 14, 0);
                bzero(buffer, sizeof(buffer));
                read(new_socket, buffer, 1024);
                // printf("bb = %s\n",buffer);
                strcat(username," - ");
                strcat(username,buffer);
                // printf("cc = %s\n",username);
                while (fgets(line, sizeof(line), fp) != NULL) {
                    // printf("%s ",line);
                    if (strstr(line, username) != NULL) {
                        flag = 1;
                        printf("Auth success\n");
                        break;
                    }
                }
                if(flag == 0){
                    printf("Auth Failed\n");
                }
                // fprintf(fp, "%s\n",username);
                bzero(buffer, sizeof(buffer));
                fclose(fp);
            }
            else if (!(strncmp(buffer,"register",8))) {
                // printf("2\n");
                send ( new_socket, "   Username : ", 14, 0);
                bzero(buffer, sizeof(buffer));
                read(new_socket, buffer, 1024);
                // printf("aa = %s\n",buffer);
                fp = fopen ("/home/excel/Desktop/SoalShiftSISOP20_modul3_E02/soal2/database.txt","a");
                strcpy(username,buffer);
                // printf("dd = %s\n",username);
                send( new_socket, "   Password : ", 14, 0);
                bzero(buffer, sizeof(buffer));
                read(new_socket, buffer, 1024);
                // printf("bb = %s\n",buffer);
                strcat(username," - ");
                strcat(username,buffer);
                // printf("cc = %s\n",username);
                fprintf(fp, "%s\n",username);
                bzero(buffer, sizeof(buffer));
                fclose(fp);
                char line[100];
                fp = fopen ("/home/excel/Desktop/SoalShiftSISOP20_modul3_E02/soal2/database.txt","r");
                while (fgets(line, sizeof(line), fp) != NULL) {
                    printf("%s",line);
                }
                fclose(fp);
            }
            else if (!(strncmp(buffer,"find",4)) && flag == 1){
                player++;
                while(player < 2){
                    // printf("buffer 3 ->%s wait\n",buffer);
                    send( new_socket, "wait", 4, 0);
                    // printf("player %d\n",player);
                    bzero(buffer, sizeof(buffer));
                sleep(1);
                }
                if(player == 2){
                game = 1;
                // he
                // printf("buffer 4 ->%s play\n",buffer);
                send( new_socket, "play", 4, 0);
                }

                
            }
            else if (!(strncmp(buffer,"logout",6)) && flag == 1){
                flag = 0;
                printf("flag %d player %d\n",flag,player);
            }
            else
                printf("gagal\n");

        }
    }
}

int main (int argc, char const *argv[]) {
     int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {
        id_socket[i] = new_socket;
        pthread_create(&(tid[i]), NULL, playandcount, &new_socket);
        i++;
    }
    return 0;
}
