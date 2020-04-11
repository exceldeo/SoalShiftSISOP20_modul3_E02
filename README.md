# SoalShiftSISOP20_modul3_E02

### Outline
1. [Soal #1](#soal-1) (Source Code : [soal1.c]())
2. [Soal #2](#soal-2) (Source Code : [soal2.c]())
3. [Soal #3](#soal-3) (Source Code : [soal3.c]())
4. [Soal #4](#soal-4) (Source Code : [soal4a.c](), [soal4b.c](), [soal4c.c]())

## Soal #2

Pada soal ini di minta untuk membuat program client dan server menggunakan socket.

pada client terdapat 2 screen yaitu screen login dan register pada screen pertama dan find match dan logout pada screen 2 

pertama kita hubungkan kedua socket 
```c
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
```
untuk menerima kiriman dari server. pertama buffer sebagai penerima maka kita kosongkan dulu lalu di baca jika bukan wait dan play maka yang kirim adalah screen 1 dan screen 2 lalu kita akan mengirim inputan dari player
```c
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
```
jika server telah mengirim play maka game akan di mulai. setiap player dapat menyerang musuh dengan menekan tombol spasi dan player yang di serang akan di beritahukan jika terkena hit
```c
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
```
di sini kita membuat thread untuk membaca heal jika terkena serangan sampai salah satu player menang atau mati 
```c
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
```

berikut cara untuk membaca spasi menggunakan termios
```c
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
```
selanjutnya di sisi server. server akan menyimpan setian socket yang masuk dengan cara menyimpannya di array lalu jalankan thread 
```c
    while((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {
        id_socket[i] = new_socket;
        pthread_create(&(tid[i]), NULL, playandcount, &new_socket);
        i++;
    }
```

selanjutnya server akan mengirim buffer dan menerima buffer dari player
```c
            bzero(buffer, sizeof(buffer));
            if(flag == 0 && game == 0 )
            strcpy(buffer,cek1);
            else if(flag == 1 && game == 0)
            strcpy(buffer,cek2);
            send ( new_socket, buffer, sizeof(buffer), 0) ;

            bzero(buffer, sizeof(buffer));
            read(new_socket, buffer, 1024);
```
jika player pilih login dan mengirim username serta password maka server akan mengecek apakah ada username dan password tersebut di database
```c
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
```
jika player memilih register maka server akan menambahkan username dan password ke dalam database dan menampilkan database tersebut di server
```c
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
```
jika player telah login maka server akan mengirimkan screen 2 pada player jika player ingin mencari match maka player mengirim find ke server dan jika hanya 1 player yang mengirim find maka server akan melooping untuk menunggu player ke 2
```c
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
```
selanjutnya jika telah memasuki game maka server akan membaca kiriman dari player jika player 1 mengirim hit maka player 2 akan berkurang nyawanya begitu sebalikanya dan kalau salah satu player nyawanya telah habis maka server akan mengirim dead kepada player yang kebabisan nyawa dan mengirim win kepada player yang masih memiliki nyawa 
```c
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
```

## Soal #3
Pada soal ini, kita diminta untuk membuat program C yang dapat mengkategorikan file sesuai dengan ekstensinya. Apabila file tersebut tidak memiliki ekstensi maka file tersebut akan dipindahkan ke folder Unknown. Terdapat tiga mode untuk mengkategorikan file, yaitu -f, *, dan -d.

Jika mode yang dipilih adalah mode -f, maka user bisa menambahkan argumen file yang ingin dikategorikan sebanyak mungkin.
```c
if (strcmp(argv[1],"-f") == 0) {
    for(j = 2 ; j < argc ; j++ ){
        pthread_create(&(tid[i]),NULL,playandcount,argv[j]);
        pthread_join(tid[i],NULL);
        i++;
    }
}
```
Kode di atas akan melakukan looping sebanyak argumen file yang diinput dan memasukkannya ke thread yang dibuat untuk memindahkan file.

Mode * dan -d hampir mirip karena sama-sama mengkategorikan seluruh file yang ada dalam suatu direktori. Perbedaannya mode * mengkategorikan seluruh file di working directory sedangkan mode -d mengkategorikan seluruh file di direktori yang user inputkan dan hasilnya akan disimpan pada working directory.
```c
i = 0;
DIR *fd, *fdo;
struct dirent *masuk;
char tempata[100];

fd = opendir(cwd);
int available = 0;

if (fd == NULL) {
    printf("error\n");
}

while( (masuk=readdir(fd)) ) {
    if ( !strcmp(masuk->d_name, ".") || !strcmp(masuk->d_name, "..") )
        continue;
    
    printf("%s %d\n",masuk->d_name,masuk->d_type);

    strcpy(tempata,cwd);
    strcat(tempata,"/");
    strcat(tempata,masuk->d_name);
    
    if(masuk->d_type == 8){
        pthread_create(&(tid[i]),NULL,playandcount,tempata); //membuat thread
        pthread_join(tid[i],NULL);
        i++;
    }
}
```
Kode di atas untuk membuka direktori yang seluruh filenya ingin dikategorikan. Kode tersebut berlaku baik untuk mode * dan -d. Berikut merupakan sedikit penjelasan mengenai kode tersebut.
+ `fd = opendir(cwd)` untuk membuka direktori. Jika fd tidak dapat dibuka atau `if (fd == NULL)` maka `printf("error\n")`.
+ `masuk = readdir(fd)` untuk membaca apa saja yang terdapat pada direktori.
+ `strcpy(tempata,cwd)`,`strcat(tempata,"/")`, dan `strcat(tempata,masuk->d_name)` menyimpan path dari file yang akan dikategorikan pada `tempata`. Adapun `cwd` atau current working directory diperoleh dari `getcwd(cwd, sizeof(cwd))`.
+ `if(masuk->d_type == 8)` untuk mengecek apakah file atau bukan. Apabila `masuk` merupakan file maka akan dibuat thread untuk mengkategorikan file tersebut.

Kode-kode di bawah ini merupakan isi dari thread `playandcount` yang mengkategorikan suatu file.
```c
token1 = strtok(abc, "/");
while( token1 != NULL ) {

    arr2[m] = token1;
    m++;
    token1 = strtok(NULL, "/");
}
strcpy(arr3,arr2[m-1]);
```

Kode tersebut untuk mendapatkan nama file beserta ekstensinya. Sementara kode di bawah ini untuk mendapatkan ekstensi dari file tersebut.
```c
token = strtok(arr2[m-1], ".");
while( token != NULL ) {
    arr[n] = token;
    n++;
    token = strtok(NULL, ".");
}
```

Karena ekstensi yang diminta tidak case sensitive, maka digunakan kode berikut untuk mengubah apabila terdapat huruf kapital menjadi huruf kecil. String `arr[n-1]` berisi ekstensi dari file yang akan dikategorikan.
```c
char aa[100];
strcpy(aa,arr[n-1]);
for(int i = 0; aa[i]; i++) aa[i] = tolower(aa[i]);
```

Jika file yang akan dikategorikan memiliki ekstensi, maka terlebih dahulu dicek apakah folder dengan nama ekstensi tersebut sudah ada atau belum dengan `if(strcmp(entry->d_name,aa) == 0 && entry->d_type == 4)`. Adapun `folder` berisi `folder = opendir(cwd)`. Apabila folder dengan nama ekstensi tersebut belum ada (`avaiable == 0`) maka akan dibuat folder dengan nama ekstensi tersebut dengan `mkdir(tempat2, 0777)`. `tempat2` berisi path folder yang akan dibuat.
```c
if( n > 1 ){

    if(folder == NULL) {
        printf("error\n");
    }

    while( (entry=readdir(folder)) ) {

        if(strcmp(entry->d_name,aa) == 0 && entry->d_type == 4){
            available = 1;
            break;
        }
    }

    if (available == 0) {
        strcpy(tempat2,cwd);
        strcat(tempat2,"/");
        strcat(tempat2,aa);

        mkdir(tempat2, 0777);    
    }
}
```
Sementara apabila file yang akan dikategorikan tidak memiliki ekstensi, maka akan dibuat folder `Unknown` dengan fungsi `mkdir` seperti ketika membuat folder dengan nama ekstensi file.

Untuk memindahkan file ke dalam folder sesuai dengan ekstensinya digunakan fungsi `rename(source, destination)` dimana `source` berisi path awal file yang ingin dikategorikan dan `destination` berisi path ke folder dengan nama ekstensi file tersebut di working directory.

---

## Soal #4

**Perkalian matriks** pada program 4a dilakukan antara matriks 4x2 dan matriks 2x5 sehingga menghasilkan matriks 4x5. Untuk melakukan perkalian matriks ini, kami menggunakan thread `playandcount` dengan kode sebagai berikut. 
```c
void* playandcount (void *arg) {

    pthread_t id = pthread_self();

    if (pthread_equal(id, tid[0]))      // baris 1 matriks c
        for (int i = 0; i < 5; i++) 
            C[0][i] = A[0][0]*B[0][i] + A[0][1]*B[1][i];
    else if (pthread_equal(id, tid[1])) // baris 2 matriks c
        for (int i = 0; i < 5; i++)
            C[1][i] = A[1][0]*B[0][i] + A[1][1]*B[1][i];
    else if (pthread_equal(id, tid[2])) // baris 3 matriks c
        for (int i = 0; i < 5; i++)
            C[2][i] = A[2][0]*B[0][i] + A[2][1]*B[1][i];
    else if (pthread_equal(id, tid[3])) // baris 4 matriks c
        for (int i = 0; i < 5; i++)
            C[3][i] = A[3][0]*B[0][i] + A[3][1]*B[1][i];

}
```
Karena hasil dari perkalian matriks ini akan digunakan pada soal 4b, maka setelah berhasil melakukan perkalian, hasil dari perkalian matriks tersebut akan kami simpan dalam shared memory dengan kode berikut.
```c
key_t key = 1234;
int shmid = shmget(key, sizeof(int)*row*column, IPC_CREAT | 0666);
matrix = (int *)shmat(shmid, NULL, 0);

for (int i = 0; i < row; i++){
    for (int j = 0; j < column; j++){
        matrix[i*column + j] = C[i][j];
        printf("%d\t", matrix[i*column + j]);
    }
    printf("\n");   
}

sleep(10);

shmdt(matrix);
shmctl(shmid, IPC_RMID, NULL);
```
Berikut ini sedikit penjelasan mengenai kode di atas.
+ `shmid` disini mengembalikan nilai dari pemanggilan sistem `shmget(key, sizeof(int)*row*column, IPC_CREAT | 0666)` yang digunakan untuk membuat suatu segmen dalam memori.
+ `matrix = (int *)shmat(shmid, NULL, 0)` mendaftarkan atau mengattach segmen ke data space dari proses.
+ `shmdt(matrix)` untuk mendetach segmen dari data space dari proses.
+ `shmctl(shmid, IPC_RMID, NULL)` untuk mengetahui atau mengubah informasi yang berkaitan dengan suatu shared memory.

Untuk **menampilkan hasil perkalian** dari program 4a pada program 4b dapat dilakukan dengan menggunakan shared memory seperti kode di atas. Hasil dari perkalian tersebut kemudian kami simpan dalam `matriks[i][j]` agar dapat digunakan untuk menghitung penjumlahan dari n sampai 1.
```c
for (int i = 0; i < row; i++){
    for (int j = 0; j < column; j++){
        printf("%d\t", matrix[i*column + j]);
        matriks[i][j] = matrix[i*column + j];
    }
    printf("\n");
}
```
Sedangkan untuk melakukan **penjumlahan dari n sampai 1**, kami menggunakan thread `playandcount` yang di dalamnya terdapat rumus deret geometri. Variabel `n` disini menampung nilai `matriks[i][j]` yang ingin kita jumlahkan dari n sampai 1.
```c
void* playandcount (void *arg) {

    struct args* data;
    data = (struct args *) arg;

    int n = data->n;
    
    n = ((n * (n + 1)) / 2);
    printf("%d\t", n);

}
```

Pada program 4c, kita diminta untuk membuat program yang dapat mengetahui jumlah file dan folder di direktori saat ini dengan command **ls | wc -l**. Selain itu, program ini juga harus menggunakan **IPC pipes**.

Disini, untuk membuat program tersebut, kami menggunakan 2 pipe dengan fork. Pembuatan pipe dapat dilakukan dengan fungsi `pipe()`. Apabila fork berhasil dilakukan dan child process berhasil dibuat (`p == 0`), maka pipe pertama `fd1[1]`akan diduplikasi menjadi file deskriptor dari stdout, lalu pipe pertama tersebut `fd1` akan ditutup dan program akan mengeksekusi command `ls`.
```c
else if (p == 0) { 
    
    dup2(fd1[1], 1);

    close(fd1[0]);
    close(fd1[1]);

    char *argm1[] = {"ls", NULL};
    execv("/bin/ls", argm1);
} 
```
Kemudian barulah parent processnya dieksekusi dengan terlebih dahulu menutup pipe kedua `fd2`, menduplikasi pipe pertama `fd1[0]` menjadi file deskriptor dari stdin, menduplikasi pipe kedua `fd2[1]` menjadi file deskriptor dari stdout, dan menutup pipe pertama `fd1`. Parent process disini akan mengeksekusi command `wc -l`.
```c
else {

  close(fd2[0]);
  close(fd2[1]);
		
   dup2(fd1[0], 0);
   dup2(fd2[1], 1);

   close(fd1[0]);
   close(fd1[1]);

   char *argm2[] = {"wc", "-l", NULL};
   execv("/usr/bin/wc", argm2);
}
```
