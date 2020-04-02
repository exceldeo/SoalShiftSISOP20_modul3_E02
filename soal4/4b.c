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
    int res[MAX];
    
    // Initialize result
    res[0] = 1;
    int res_size = 1;

    // Apply simple factorial formula 1 * 2 * 3 * ...
    for(int x = 2; x <= n; x++) {
        int carry = 0;

        for(int i = 0; i < res_size; i++) {
            int product = res[i] * x + carry;

            // Store last digit of product in res[]
            res[i] = product % 10;

            // Put rest in carry
            carry = product / 10;
        }

        while (carry) {
            res[res_size] = carry % 10;
            carry = carry/10;
            res_size++;
        }
    }

    for (int i = res_size - 1; i >= 0; i--)
        printf("%d", res[i]);
    
    printf(" ");

}

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
            matriks[i][j] = matrix[i*column + j];
        }
        printf("\n");
    }

    printf("\nFACTORIAL\n");

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