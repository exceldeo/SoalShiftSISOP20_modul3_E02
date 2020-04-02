#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080

int player = 0;
pthread_t tid[100];

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

    int health = 0;
    // printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
    // printf("new socket %d\n",new_socket);

    for(;;) {
        // printf("flag = %d\n",flag);
        bzero(buffer, sizeof(buffer));
        if(flag == 0 )
        strcpy(buffer,cek1);
        else if(flag == 1)
        strcpy(buffer,cek2);

        send ( new_socket, buffer, sizeof(buffer), 0) ;

        bzero(buffer, sizeof(buffer));
        read(new_socket, buffer, 1024);
        // printf("%s",buffer);
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
                send( new_socket, "wait", 4, 0);
                // printf("player %d\n",player);
            sleep(1);
            }
                        
            // send( new_socket, "play", 4, 0);

            // bzero(buffer, sizeof(buffer));
            
        }
        else if (!(strncmp(buffer,"endgame",7)) && flag == 1){
            
        }
        else if (!(strncmp(buffer,"logout",6)) && flag == 1){
            flag = 0;
            printf("flag %d player %d\n",flag,player);
        }
        else if (!(strncmp(buffer," ",1)) && player<1){
            health++;
            if(health >= 10 ) {
                send( new_socket, "endgame", 7, 0);
                bzero(buffer, sizeof(buffer));
            }

                
        }
        else
            printf("gagal\n");
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

        pthread_create(&(tid[i]), NULL, playandcount, &new_socket);
        i++;
    }
    return 0;
}
