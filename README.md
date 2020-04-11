# SoalShiftSISOP20_modul3_E02

### Outline
1. [Soal #1](#soal-1) (Source Code : [soal1_traizone.c](https://github.com/exceldeo/SoalShiftSISOP20_modul3_E02/blob/master/soal1/soal1_trainzone.c), [soal1_pokezone.c](https://github.com/exceldeo/SoalShiftSISOP20_modul3_E02/blob/master/soal1/soal1_pokezone.c))
2. [Soal #2](#soal-2) (Source Code : [soal2_tapplayer.c](https://github.com/exceldeo/SoalShiftSISOP20_modul3_E02/blob/master/soal2/tapplayer.c), [soal2_tapserver.c](https://github.com/exceldeo/SoalShiftSISOP20_modul3_E02/blob/master/soal2/tapserver.c))
3. [Soal #3](#soal-3) (Source Code : [soal3.c](https://github.com/exceldeo/SoalShiftSISOP20_modul3_E02/blob/master/soal3/soal3.c))
4. [Soal #4](#soal-4) (Source Code : [soal4a.c](https://github.com/exceldeo/SoalShiftSISOP20_modul3_E02/blob/master/soal4/soal4a.c), [soal4b.c](https://github.com/exceldeo/SoalShiftSISOP20_modul3_E02/blob/master/soal4/soal4b.c), [soal4c.c](https://github.com/exceldeo/SoalShiftSISOP20_modul3_E02/blob/master/soal4/soal4c.c))

## Soal #1

Program **soal1_traizone.c** memiliki 2 fitur yaitu Normal Mode dan Capture Mode. Pada Normal Mode terdapat 3 menu yaitu Cari Pokemon, Pokedex, dan Shop. Variabel `mencari` pada kode di bawah ini mulanya bernilai 0.
```c
if (mode == 0) {
    if (mencari) printf("== Normal Mode ==\n1. Berhenti Mencari\n2. Pokedex\n3. Shop\n4. Go to capture mode\nInput: ");
    else printf("== Normal Mode ==\n1. Cari Pokemon\n2. Pokedex\n3. Shop\n4. Go to capture mode\nInput: ");
} 
```
Jika user memilih menu Cari Pokemon, maka program akan memanggil fungsi `caripokemon` dan mengubah opsi Cari Pokemon menjadi Berhenti Mencari. Kemudian program akan masuk ke dalam thread `fthreadcari` untuk mencari pokemon.
```c
void caripokemon() {
	mencari = !mencari;
	if (mencari) {
		pthread_create(&threadcari, NULL, fthreadcari, NULL);
	} else {
		pthread_cancel(threadcari);
	}
}
```
Ketentuan untuk opsi Cari Pokemon sendiri di antaranya jika diaktifkan maka setiap 10 detik akan memiliki 60% chance untuk menemui pokemon. Adapun untuk tipe pokemon dan apakah pokemon tersebut shiny atau tidak diatur soal program soal1_pokezone.c. Berikut ini merupakan kode dari thread `fthreadcari`.
```c
void* fthreadcari() {
	while (1) {
		sleep(10);
		if (rand() % 100 < 60) {
			mencari = 0;
			pthread_mutex_lock(&pokemoncapturewrite);
			pokemoncapture = *shmpokemon;
			pthread_mutex_unlock(&pokemoncapturewrite);
			pthread_create(&pokemoncapturethread, NULL, fpokemoncapturethread, NULL); //lepas
			mencari = 0;
			mode = 1;
			input = -1;
			pthread_cancel(inputthread);
			pthread_exit(0);
		}
	}
}
```
Pada thread ini terdapat fungsi `pthread_mutex_lock(&pokemoncapturewrite)` yang digunakan untuk mengunci variabel `pokemoncapturewrite`. Variabel `pokemoncapturewrite` nantinya akan berisi tipe pokemon yang diperoleh dari program soal1_pokezone.c. Fungsi `pthread_mutex_unlock(&pokemoncapturewrite)` digunakan untuk mengunlock variabel `pokemoncapturewrite`. Kegunaan dari fungsi tersebut adalah untuk menjaga sumber daya suatu thread agar tidak digunakan oleh thread lain sebelum ia menyelesaikan pekerjaannya.

Kita kemudian akan masuk ke dalam thread `fpokemoncapturethread` yang setiap 20 detik akan mengecek apakah pokemon yang sudah berhasil ditemukan kabur atau tidak. Variabel `tres` sendiri berisi kemungkinan pokemon tersebut lepas.
```c
void* fpokemoncapturethread() {
	int thres = (pokemoncapture % 15) / 5;
	if (thres == 0) thres = 5;
	else if (thres == 1) thres = 10;
	else if (thres == 2) thres = 20;
	if (pokemoncapture >= 15) thres += 5;
	while (1) {
		sleep(20);
		if (effect) continue;
		if (rand() % 100 < thres) {
			printf("Pokemon telah escape dari pencarian pokemon.\n");
			pthread_mutex_lock(&pokemoncapturewrite);
			pokemoncapture = -1;
			pthread_mutex_unlock(&pokemoncapturewrite);
			pthread_exit(0);
		}
	}
}
```

Setelah berhasil menemukan pokemon, kita akan memasuki Capture Mode yang memiliki 3 opsi menu yaitu Tangkap, Item, dan Keluar.
```c
else if (mode == 1) {
    if (pokemoncapture == -1) printf("== Capture Mode ==\n1. Tangkap\n2. Item\n3. Keluar\nInput: ");
    else printf("== Capture Mode ==\nMenemukan pokemon: %s\n1. Tangkap\n2. Item\n3. Keluar\nInput: ", namapokemon[pokemoncapture]);
}
```

Apabila kita memilih tangkap, maka pokemon akan ditangkap menggunakan pokeball yang kita miliki. Pokeball akan tetap berkurang walaupun pokemon tidak berhasil ditangkap. Penangkapan pokemon ini terjadi di dalam fungsi `tangkap`. Berikut ini merupakan beberapa baris kode dalam fungsi `tangkap`.
```c
int thres = (pokemoncapture % 15) / 5;

if (thres == 0) thres = 70;
else if (thres == 1) thres = 50;
else if (thres == 2) thres = 30;

if (pokemoncapture >= 15) thres -= 20;

if (effect) thres += 20;

if (rand() % 100 < thres) {
	printf("Berhasil menangkap %s!\n", namapokemon[pokemoncapture]);
	int slot = -1;
	
    for (int i = 0; i < 7; i++) {
		if (pokemon[i] == -1) {
			slot = i;
			break;
		}
	}
	if (slot == -1) {
		int money = (pokemoncapture % 15) / 5;
		if (money == 0) money = 80;
		else if (money == 1) money = 100;
		else if (money == 2) money = 200;
		if (pokemoncapture >= 15) money += 5000;
		pokedollar += money;
		printf("Slot pokemon penuh! Anda mendapatkan %d.\n", money);
	} else {
		pthread_mutex_lock(&pokemonwrite);
		pokemon[slot] = pokemoncapture;
		pokemonAP[slot] = 100;
		pthread_mutex_unlock(&pokemonwrite);
		pthread_mutex_lock(&pokeslotwrite);
		pokeslot = slot;
		pthread_create(&pokemonthread[slot], NULL, fpokemonthread, NULL);
	    	pthread_cancel(pokemoncapturethread);
		pthread_mutex_lock(&pokemoncapturewrite);
		pokemoncapture = -1;
		pthread_mutex_unlock(&pokemoncapturewrite);
		}
} else {
	printf("Tidak berhasil menangkap %s.\n", namapokemon[pokemoncapture]);
}
```

Variabel `thres` dalam fungsi ini merupakan kemungkinan berhasil tidaknya suatu pokemon ditangkap. Apabila pokemon berhasil ditangkap sedangkan slot pokemon kita telah penuh maka pokemon yang ditangkap akan berubah menjadi pokedollar. Sedangkan apabila slot pokemon kita belum penuh maka kita akan masuk ke thread `fpokemonthread` yang mengurangi 10 Affection Point (AP) per 10 detik sejak waktu ditangkap dan jika AP tersebut bernilai 0 maka pokemon tersebut memiliki 90% chance untuk lepas tanpa memberikan pokedollar atau 10% chance untuk reset AP menjadi 50 AP. Berikut merupakan kode dari thread `fpokemonthread`.
```c
void* fpokemonthread() {
	int slot = pokeslot;
	pthread_mutex_unlock(&pokeslotwrite);
	while (1) {
		sleep(1);
		if (mode == 1) continue;
		pthread_mutex_lock(&pokemonwrite);
		pokemonAP[slot] -= 10;
		if (pokemonAP[slot] == 0) {
			if (rand() % 100 < 90) {
				printf("Ada pokemon yang terlepas.\n");
				pokemon[slot] = -1;
				pokemonAP[slot] = -1;
				pthread_exit(0);
			} else {
				pokemonAP[slot] = 50;
			}
		}
		pthread_mutex_unlock(&pokemonwrite);
	}
}
```
Kembali pada opsi Capture Mode, apabila kita memilih opsi Item maka kita akan masuk ke dalam fungsi `useitem` dan dapat menggunakan item Lullaby Powder yang meningkatkan chance untuk menangkap pokemon sebesar 20% serta menurunkan escape rate menjadi 0% sehingga pokemon tidak bisa lari. Penggunaan Lullaby Powder ini akan mengarahkan kita pada thread `flullabythread` yang akan mem-pause program selama 10 detik sesuai dengan durasi dari Lullaby Powder ketika digunakan. Sedangkan apabila kita milih opsi Keluar pada Capture Mode, maka kita akan kembali pada Normal Mode.

Apabila kita memilih opsi Pokedex pada Normal Mode, maka kita akan diarahkan menuju fungsi `pokedex()` yang menampilkan list pokemon yang kita miliki beserta affection pointnya. Pada opsi ini kita juga dapat melepas pokemon yang telah ditangkap atau memberi berry kepada semua pokemon sehingga affection pointnya meningkat sebesar 10. Jika kita memilih untuk melepas pokemon, maka kita akan mendapatkan pokedollar sesuai dengan tipe pokemon yang kita lepaskan.

Apabila kita memiih opsi Shop pada Normal Mode, maka kita dapat membeli item dari soal1_pokezone.c. Item yang dapat dibeli di antaranya ada Lullaby Powder, Pokeball, dan Berry. Pembelian item akan mengurangi pokedollar yang kita miliki. Masing-masing item yang dapat dibeli dan dimiliki trainer maksimal ada 99.

Sekarang beralih pada program **soal1_pokezone.c**. Pada program ini terdapat 3 fitur yaitu shutdown game, jual item, dan menyediakan random pokemon. Karena program ini berhubungan dengan soal1_traizone.c maka disini digunakan shared memory. Shared memory ini juga digunakan pada soal1_traizone.c.
```c
shmpokemon = shmat(shmidpokemon, NULL, 0);
shmlp = shmat(shmidlp, NULL, 0);
shmpb = shmat(shmidpb, NULL, 0);
shmb  = shmat(shmidb, NULL, 0);
```
Variabel `shmpokemon` ini berisi index untuk menentukan tipe pokemon, variabel `shmlp` berisi item Lullaby Powder yang tersedia di Shop,  variabel `shmpb` berisi item Pokeball yang tersedia di Shop, dan variabel `shmb` berisi item Berry yang tersedia di Shop.

Apabila kita memilih shutdown game maka program akan membunuh semua proses yang dijalankan oleh soal1_traizone.c. Berikut ini merupakan baris kode yang akan menshutdown game ketika dipilih. Kode `snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid)` digunakan untuk mencari program yang akan di kill.
```c
if (x == 1) {
    DIR* dir = opendir("/proc");
    struct dirent* ent;
    char* endptr;
    while ((ent = readdir(dir)) != NULL) {
        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0') continue;
            
        char buf[512];
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");
        if (fp) {
            if (fgets(buf, sizeof(buf), fp) != NULL) {
                char* first = strtok(buf, " ");
                if (strcmp(first, "./soal1_traizone") == 0) {
                    pid_t child = fork();
                    if (child == 0) {
                        char cmd[500];
                        sprintf(cmd, "%ld", lpid);
                        execl("/bin/kill", "kill", "-9", cmd, NULL);
                    }
                }
            }
            fclose(fp);
        }
    }
    closedir(dir);
    int status;
    while(wait(&status) > 0);

    shmdt(shmpokemon);
    shmdt(shmlp);
    shmdt(shmpb);
    shmdt(shmb);
    shmctl(shmidpokemon, IPC_RMID, NULL);
    shmctl(shmidlp, IPC_RMID, NULL);
    shmctl(shmidpb, IPC_RMID, NULL);
    shmctl(shmidb, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}
```
Fitur Jual Item berada pada thread `frestockthread` yang akan menambahkan masing-masing item 10 buah setiap 10 detik. Maksimum masing-masing item di dalam Shop adalah 200 buah.
```c
void* frestockthread() {
    while (1) {
        sleep(10);
        *shmlp += 10;
        if (*shmlp > 200) *shmlp = 200;
        *shmpb += 10;
        if (*shmpb > 200) *shmpb = 200;
        *shmb += 10;
        if (*shmb > 200) *shmb = 200;
    }
}
```
Sementara itu, fitur random pokemon berada pada thread `frandomthread` yang nantinya akan memilih salah satu tipe pokemon berdasarkan encounter ratenya. 
```c
void* frandomthread() {
    while (1) {
        int poke;
        int r = rand() % 100;
        if (r < 5) {
            poke = (rand() % 5) + 10;
        } else if (r < 20) {
            poke = (rand() % 5) + 5;
        } else {
            poke = (rand() % 5);
        }
        if (rand() % 8000 == 0) {
            poke += 15;
        }
        *shmpokemon = poke;
        sleep(1);
    }
}
```
**Kendala**
Affection Point langsung 0 setelah 10 detik pokemon ditangkap dan masih tidak bisa menangkap pokemon kedua.

---

## Soal #2

Pada soal ini di minta untuk membuat program client dan server menggunakan socket.

Pada client terdapat 2 screen yaitu screen login dan register pada screen pertama dan find match dan logout pada screen 2 

Pertama kita hubungkan kedua socket.
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
Untuk menerima kiriman dari server. Pertama buffer sebagai penerima maka kita kosongkan dulu lalu di baca jika bukan wait dan play maka yang kirim adalah screen 1 dan screen 2 lalu kita akan mengirim inputan dari player
```c
            bzero(buffer, sizeof(buffer));
            read(sockfd, buffer, sizeof(buffer));

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
Jika server telah mengirim play maka game akan di mulai. setiap player dapat menyerang musuh dengan menekan tombol spasi dan player yang di serang akan di beritahukan jika terkena hit
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
Di sini kita membuat thread untuk membaca heal jika terkena serangan sampai salah satu player menang atau mati 
```c
    while(!dead && !win){

        int sockfd = *(int *)arg;
        bzero(buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));

        if(!(strncmp(buffer,"win",3))){
            win = 1;

        }
        else if(!(strncmp(buffer,"dead",4))){
            dead = 1;

        }
        printf("%s\n",buffer);
    }
```

Berikut cara untuk membaca spasi menggunakan termios
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
Selanjutnya di sisi server. server akan menyimpan setian socket yang masuk dengan cara menyimpannya di array lalu jalankan thread 
```c
    while((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {
        id_socket[i] = new_socket;
        pthread_create(&(tid[i]), NULL, playandcount, &new_socket);
        i++;
    }
```

Selanjutnya server akan mengirim buffer dan menerima buffer dari player
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
Jika player pilih login dan mengirim username serta password maka server akan mengecek apakah ada username dan password tersebut di database
```c
            if(!(strncmp(buffer,"login",5))){

                char line[160];
                send ( new_socket, "   Username : ", 14, 0);
                bzero(buffer, sizeof(buffer));
                read(new_socket, buffer, 1024);

                fp = fopen ("/home/excel/Desktop/SoalShiftSISOP20_modul3_E02/soal2/database.txt","r");
                strcpy(username,buffer);

                send( new_socket, "   Password : ", 14, 0);
                bzero(buffer, sizeof(buffer));
                read(new_socket, buffer, 1024);

                strcat(username," - ");
                strcat(username,buffer);
		
                while (fgets(line, sizeof(line), fp) != NULL) {

                    if (strstr(line, username) != NULL) {
                        flag = 1;
                        printf("Auth success\n");
                        break;
                    }
                }
                if(flag == 0){
                    printf("Auth Failed\n");
                }

                bzero(buffer, sizeof(buffer));
                fclose(fp);
```
Jika player memilih register maka server akan menambahkan username dan password ke dalam database dan menampilkan database tersebut di server
```c
            else if (!(strncmp(buffer,"register",8))) {

                send ( new_socket, "   Username : ", 14, 0);
                bzero(buffer, sizeof(buffer));
                read(new_socket, buffer, 1024);

                fp = fopen ("/home/excel/Desktop/SoalShiftSISOP20_modul3_E02/soal2/database.txt","a");
                strcpy(username,buffer);

                send( new_socket, "   Password : ", 14, 0);
                bzero(buffer, sizeof(buffer));
                read(new_socket, buffer, 1024);

                strcat(username," - ");
                strcat(username,buffer);

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
Jika player telah login maka server akan mengirimkan screen 2 pada player jika player ingin mencari match maka player mengirim find ke server dan jika hanya 1 player yang mengirim find maka server akan melooping untuk menunggu player ke 2
```c
                player++;
                while(player < 2){
                    send( new_socket, "wait", 4, 0);
                    bzero(buffer, sizeof(buffer));
                    sleep(1);
                }
                if(player == 2){
                game = 1;

                send( new_socket, "play", 4, 0);
                }
```
Selanjutnya jika telah memasuki game maka server akan membaca kiriman dari player jika player 1 mengirim hit maka player 2 akan berkurang nyawanya begitu sebalikanya dan kalau salah satu player nyawanya telah habis maka server akan mengirim dead kepada player yang kebabisan nyawa dan mengirim win kepada player yang masih memiliki nyawa 
```c
           bzero(buffer, sizeof(buffer));
            read(new_socket, buffer, 1024);

            if (!(strncmp(buffer,"hit",3))){

                if(new_socket == id_socket[1]){
                    id_heal[0] = id_heal[0] - 10;
                    sprintf(heal,"%d",id_heal[0]);
                    bzero(buffer, sizeof(buffer));

                    strcpy(buffer,"heal ");
                    strcat(buffer,heal);

                    send( id_socket[0], buffer, sizeof(buffer), 0);
                }
                else if(new_socket == id_socket[0]){
                    id_heal[1] = id_heal[1] - 10;
                    sprintf(heal,"%d",id_heal[1]);
                    bzero(buffer, sizeof(buffer));

                    strcpy(buffer,"heal ");
                    strcat(buffer,heal);

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

---

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
