#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>

char cwd[100];
pthread_t tid[3]; //inisialisasi array untuk menampung thread dalam kasus ini ada 2 thread
pid_t child;
char *arr[4],*arr2[20],arr3[100],abc[100];
int n = 0, m = 0;

int length=5; //inisialisasi jumlah untuk looping
void* playandcount(void *arg)
{
    strcpy(abc,arg);
    char *token,*token1;

	unsigned long i=0;
	pthread_t id=pthread_self();
	int iter;

    token1 = strtok(abc, "/");
    while( token1 != NULL ) {
        // printf( "token %d = %s\n", m , token1 );
        arr2[m] = token1;
        m++;
        token1 = strtok(NULL, "/");
    }
    strcpy(arr3,arr2[m-1]);
    // printf("arr 3 %s\n",arr3);
    token = strtok(arr2[m-1], ".");
    while( token != NULL ) {
        // printf( "token %d = %s\n", n , token );
        arr[n] = token;
        n++;
        token = strtok(NULL, ".");
    }

    char aa[100];
    strcpy(aa,arr[n-1]);
    for(int i = 0; aa[i]; i++){
        aa[i] = tolower(aa[i]);
    }
  
    DIR *folder, *folderopen;
    struct dirent *entry;
    char tempat2[100],tempat3[100];
    folder = opendir(cwd);
    int available = 0;
    printf("n = %d\n",n);
    if( n > 1 ){

        if(folder == NULL)
        {
            printf("error\n");
        }
        while( (entry=readdir(folder)) )
        {
            // printf("%s %d\n",entry->d_name,entry->d_type);
            // printf("kk\n");
            if(strcmp(entry->d_name,aa) == 0 && entry->d_type == 4){
                // printf("ccc %s\n",entry->d_name);
                available = 1;
                break;
            }
        }

        // printf("available %d\n",available);
        // printf("aa %s\n",tempat2);
        if(available == 0){
            // printf("bisa\n");
            strcpy(tempat2,cwd);
            strcat(tempat2,"/");
            strcat(tempat2,aa);
            printf("aaaa = %s\n%s\n",aa,tempat2);
            mkdir(tempat2, 0777);
            
        }
    }
    else{
        strcpy(tempat2,cwd);
        strcat(tempat2,"/");
        strcat(tempat2,"Unknown");
        printf("bbbb= %s\n %s\n",aa,tempat2);
        mkdir(tempat2, 0777);
    }
	
    int ch;
    FILE *fp1, *fp2;
    char source[1024], target[1024];

    printf("Enter your source file name:\n");
    // strcpy(source,cwd);
    // strcat(source,"/");
    strcpy(source,arg);
    printf("souce %s\n",source);
    printf("Enter your destination file name:");
    strcpy(target,cwd);
    strcat(target,"/");
    printf("target1 %d %s\n",n,target);
    if(n == 1 ){
        strcat(target,"Unknown");
    }
    else{
        strcat(target,aa);
    }
    strcat(target,"/");
    strcat(target,arr3);
    printf("target2 %s\n arr = %s\n",target,arr3);
    fp1 = fopen(source, "r");
    fp2 = fopen(target, "w");

    if (!fp1) {
            printf("Unable to open source file to read!!\n");
            fclose(fp2);
            return 0;
    }

    if (!fp2) {
            printf("Unable to open target file to write\n");
            return 0;
    }

    while ((ch = fgetc(fp1)) != EOF) {
            fputc(ch, fp2);
    }

    fclose(fp1);
    fclose(fp2);

    remove(source);
    n = 0;
    m = 0;

	return NULL;
}

int main(int argc, char *argv[]) {
    if(getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } 
    int i=0,j=0;
	int err;
    if (strcmp(argv[1],"-f") == 0) {
        for(j = 2 ; j < argc ; j++ ){
            int err;
            pthread_create(&(tid[i]),NULL,playandcount,argv[j]);
            pthread_join(tid[i],NULL);
            i++;
        }
    }
     
    else if (strcmp(argv[1],"*") == 0) {
        i = 0;
        DIR *fd, *fdo;
        struct dirent *masuk;
        char tempata[100],tempatb[100];
        // printf("bbb\n");
        fd = opendir(cwd);
        int available = 0;

        if(fd == NULL)
        {
            printf("error\n");
        }
        while( (masuk=readdir(fd)) )
        {
            if ( !strcmp(masuk->d_name, ".") || !strcmp(masuk->d_name, "..") )
            continue;
            printf("%s %d\n",masuk->d_name,masuk->d_type);

            int err;
            strcpy(tempata,cwd);
            strcat(tempata,"/");
            strcat(tempata,masuk->d_name);
            if(masuk->d_type == 8){
            pthread_create(&(tid[i]),NULL,playandcount,tempata); //membuat thread
            pthread_join(tid[i],NULL);
            i++;
            }
            
        }

    }
    else if (strcmp(argv[1],"-d") == 0) {
        
    }


    return 0; 
}