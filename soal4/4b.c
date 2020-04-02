#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>

#define MAX 500

int matriks[4][5];

struct args {
    int n;
};

void* playandcount (void *arg) {

    struct args* data;
    data = (struct args *) arg;

    int n = data->n;
    
    n = ((n * (n + 1)) / 2);
    printf("%d\t", n);

}

int main () {

    int row = 4;
    int column = 5;
    int *matrix;

    key_t key = 1234;
    int shmid = shmget(key, sizeof(int)*row*column, IPC_CREAT | 0666);
    matrix = (int *)shmat(shmid, NULL, 0);

    printf("MATRIKS PERKALIAN\n");

    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++){
            printf("%d\t", matrix[i*column + j]);
            matriks[i][j] = matrix[i*column + j];
        }
        printf("\n");
    }

    printf("\nMATRIKS PENAMBAHAN\n");

    pthread_t tid[20];

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++){

            struct args* data = (struct args*)malloc(sizeof(struct args));
            data->n = matriks[i][j];

            // printf("Matriks[%d][%d] %d ", i, j, data->n);

            pthread_create(&(tid[i * column + j]), NULL, playandcount, (void*)data);
            pthread_join(tid[i * column + j], NULL);
        }
        printf("\n");
    }

    shmdt(matrix);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}