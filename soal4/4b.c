#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>

int main () {

    int row = 4;
    int column = 5;
    int *matrix;

    key_t key = 1234;
    int shmid = shmget(key, sizeof(int)*row*column, IPC_CREAT | 0666);
    matrix = (int *)shmat(shmid, NULL, 0);

    for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++){
            printf("%d\t", matrix[i*column + j]);
        }
        printf("\n");
    }

    shmdt(matrix);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}