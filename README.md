# SoalShiftSISOP20_modul3_E02

### Outline
1. [Soal #1](#soal-1) (Source Code : [soal1.c]())
2. [Soal #2](#soal-2) (Source Code : [soal2.c]())
3. [Soal #3](#soal-3) (Source Code : [soal3.c]())
4. [Soal #4](#soal-4) (Source Code : [soal4a.c](), [soal4b.c](), [soal4c.c]())

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
