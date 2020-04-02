#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>

struct args {
    char nama[100];
    char direktori[100];
    char ekstensi[100];
};

void* pindah (void *arg) {

    struct args *data;
    data = (struct args *) arg;

    char file[100];
    strcpy(file, data->nama);

    char extension[100];
    strcpy(extension, data->ekstensi);

    char directory[100];
    strcpy(directory, data->direktori);
    strcat(directory, extension);

    // printf("Thread: %s %s\n", file, directory);

    DIR *folder;
    folder = opendir(directory);

    // Make directory
    if (folder == NULL) mkdir(directory, 0777);
    else closedir(folder);
    
    char temp[100];
    strcpy(temp, file);

    char *filename = basename(temp);

    char baru[100];
    sprintf(baru, "%s/%s", directory, filename);
    printf("baru : %s\n\n", baru);

    rename(file, baru);
}

int main (int argc, char *argv[]) {

    pthread_t tid[argc-2];
    int iter = 0;

    // Get current working directory
    char cwd[100];
    if(getcwd(cwd, sizeof(cwd)) != NULL)
        strcat(cwd, "/");
    else
        perror("Error\n");


    if (strcmp(argv[1], "-f") == 0) {

        for (int i = 2; i < argc; i++){

            struct args* data = (struct args*)malloc(sizeof(struct args));
            strcpy(data->nama, argv[i]);
            strcpy(data->direktori, cwd);

            // Get extension
            char *ext = strrchr(argv[i], '.');

            if ( ext == NULL )
                strcpy(data->ekstensi, "Unknown");
            else
                strcpy(data->ekstensi, ext + 1);

            printf("%s %s %s\n", data->ekstensi, data->nama, data->direktori);


            pthread_create(&(tid[iter]), NULL, pindah, (void*)data);
            pthread_join(tid[iter], NULL);

            iter++;
        }
    }

    // pthread_join(tid[0], NULL);
    // pthread_join(tid[1], NULL);
    // else if(strcmp(argv[1], "-d") == 0)
    // else if(strcmp(argv[1], "*") == 0)

    // Get extension
    // for (int i = 1; i < argc; i++) {

    //     char *ext = strrchr(argv[i], '.');

    //     if (ext == NULL)
    //         printf("Unknown\n");
    //     else
    //     {
    //         printf("Extension: %s\n", ext + 1);
    //     }
    // }

    // Move file
    // char lama[100];
    // strcpy(lama, argv[1]);

    // char baru[100];
    // strcat(baru, cwd);
    // strcat(baru, "/");
    // strcat(baru, filename);
    // printf("old: %s\nnew: %s\n", lama, baru);


    // rename(lama, baru);

    // DIR *folder;
    // struct dirent *entry;

    // folder = opendir(dir);

    // if (folder == NULL) {
    //     perror("Unable to read directory");
    //     return(1);
    // }

    // chdir(dir);
    // rename(lama, baru);
    // chdir("..");
    // closedir(folder);

    return 0;
}