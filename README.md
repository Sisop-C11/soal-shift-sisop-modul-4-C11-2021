# soal-shift-sisop-modul-4-C11-2021
## Anggota
* James Raferty Lee 	(05111940000055)
* Mohammad Tauchid		(05111940000136)
* Kevin Davi Samuel		(05111940000157)

## Jawaban Nomor 1
```c
char *encrypt(char* str, bool cek){
	int i, k = 0;
	char *ext = strrchr(str, '.');
	if(cek && ext != NULL) k = strlen(ext);
	
	for(i = 0; i < strlen(str) - k; i++){
	 if(!((str[i]>=0&&str[i]<65)||(str[i]>90&&str[i]<97)||(str[i]>122&&str[i]<=127))){
	   if(str[i]>='A'&&str[i]<='Z')
	   str[i]= 'Z'+'A'-str[i];
	   if(str[i]>='a'&&str[i]<='z')
	   str[i]= 'z'+'a'-str[i];
	 } 
  }
	return str;
}
```

Pertama, kita memiliki fungsi encrypt, yaitu fungsi untuk meng-enkripsi dan dekripsi sesuai dengan Atbash cypher.

```c
void substr(char *s, char *sub, int y, int z) {
   int aa = 0;
   while (aa < z) {
      sub[aa] = s[y + aa];
      aa++;
   }
   sub[aa] = '\0';
}

void loopEncry(char *str, int flag){
	struct dirent *dp;
	DIR *dir = opendir(str);
	
	if(!dir){
		return;
	}
	
	while((dp = readdir(dir)) != NULL){
		if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
        	char path[2000000], nama[1000000], namabaru[1000000];
        	campurPath(path, str, dp->d_name);
			strcpy(nama, dp->d_name);
			if(flag == 1) {
				campurPath(namabaru, str, encrypt(nama, 1));
			}
			else if(flag == -1) {
				campurPath(namabaru, str, encrypt(nama, 1));
			}
			if(dp->d_type == DT_REG){
				rename(path, namabaru);
			}
			else if(dp->d_type == DT_DIR){
				rename(path, namabaru);
				loopEncry(namabaru, flag);
			}
        }
	}
}

```

Terlihat bahwa fungsi di atas melakukan looping terhadap direktori pada path yang dipassing melalui parameter, kemudian looping dilakukan secara rekursif, ditandai dengan fungsi listFilesRecursivelyEnc1(path); dipanggil dalam loop folder, agar tiap folder dalam folder juga dicek dan dienskripsi.

Kemudian setiap akan memanggil fungsi enkripsi, maka harus memanggil fungsi substring yang telah kami declare sendiri untuk mendapatkan namafilenya saja, tanpa ekstensi, kemudian setelah namafile sudah dienkripsi, baru digabungkan kembali dengan sprintf.

```c
int cekAtoz(char *str){
	int what = 0;
	char *haha = strtok(str, "/");
	while(haha){
		char mini[1024];
		substr(haha, mini, 0, 5);
		if(!strcmp(mini, "AtoZ_")) {
			what = 1;
		}
		haha = strtok(NULL, "/");
	}
	return what;
}
```

Ini fungsi untuk mengecek apakah nama memiliki tulisan 'AtoZ_' di depan nama menggunakan fungsi substring yang tadi sudah dibuat.

```c
static int xmp_mkdir(const char *path, mode_t mode){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	
	int jadi;

	jadi = mkdir(jalanCek(fpath), mode);
	if (jadi == -1) return -errno;
	
    char checking[1024];
    if(bagianAkhir(fpath) == 0) return 0;
    char absPath[1000000];
    strcpy(absPath, bagianAkhir(fpath));
    substr(absPath, checking, 0, 5);
	if(strcmp(checking, "AtoZ_") == 0) //folder fullencr
	{
		FILE *filez1;
		filez1 = fopen("sebuah.log", "a+");
		fprintf(filez1, "Tambah : %s → %s\n", absPath, absPath);
		fclose(filez1);
		fullencr(fpath, 1);	
	}
	return 0;
}
```

Ini fungsi berarti bahwa jika kita membuat folder dengan depan nama 'AtoZ', kita akan memasukkan ke log.

```c
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	int jadi = 0;
	
	DIR *dp;
	struct dirent *de;
	(void) offset;
	(void) fi;
	dp = opendir(jalanCek(fpath));
	if (dp == NULL) return -errno;
	
	int flag = cekAtoz(fpath);
	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;		
		st.st_mode = de->d_type << 12;
		char nama[1000000];
		strcpy(nama, de->d_name);
		if(flag == 1){
			if(de->d_type == DT_REG) encrypt(nama, 1);
			else if(de->d_type == DT_DIR && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) encrypt(nama, 0);
			jadi = (filler(buf, nama, &st, 0));
			if(jadi!=0) break;
		}
		else{
			jadi = (filler(buf, nama, &st, 0));
			if(jadi!=0) break;
		}
	}
	closedir(dp);
	return 0;
}
```

Ini fungsi saat membaca direktori. Menggunakan cekAtoz, kita bisa dengan mudah dan cepat mengetahui apakah nama memiliki depan 'AtoZ_', dengan itu, jika true, akan dienkripsi, folder juga.

```c
static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create     = xmp_create,
	.release	= xmp_release,
	.fsync		= xmp_fsync,
};
```

Seluruh system-call tersebut pada dasarnya hanya memanggil system-call linux dengan mengganti path yang diinputkan pada system-call tersebut menjadi path pada mount-pointnya, yaitu pada variable dirpath.

_getattr(): untuk mendapatkan stat dari path yang diinputkan.
_access(): untuk mengakses path yang diinputkan.
_readlink(): untuk membaca symbolic link dari path.
_mkdir(): untuk membuat direktori pada path.
_symlink(): untuk membuat symbolic link pada path.
_unlink(): untuk menghapus sebuah file pada path.
_rmdir(): untuk menghapus directory pada path.
_rename(): untuk me-rename dari path awal menjadi path tujuan.
_link(): untuk membuat hard-link pada path.
_chmod(): untuk mengubah mode (hak akses) dari path.
_chown(): untuk mengubah kepimilikan (user dan group) dari path.
_truncate(): untuk melakukan truncate (membesarkan atau mengecilkan size) dari path.
_utimens(): untuk mengubah status time dari path.
_open(): untuk meng-open (membuka) path.
_create(): untuk membuat path berdasarkan mode yang diinputkan.
_read(): untuk membaca isi dari path.
_write(): untuk menulis kedalam path.
_statfs(): untuk melaukan stat terhadap file yang diinputkan.
_release(): untuk meng-release memori yang dialokasikan untuk system-call _open().

## Screenshot
![image](https://user-images.githubusercontent.com/68369091/121811612-1b93e900-cc8f-11eb-85b1-1dfe0df62eea.png)

![image](https://user-images.githubusercontent.com/68369091/121811618-251d5100-cc8f-11eb-846b-8d900b40e747.png)

![image](https://user-images.githubusercontent.com/68369091/121811636-31091300-cc8f-11eb-9e97-8e30d2249a2e.png)

![image](https://user-images.githubusercontent.com/68369091/121811645-39614e00-cc8f-11eb-99d9-77e6cfef8313.png)

![image](https://user-images.githubusercontent.com/68369091/121811693-5ac23a00-cc8f-11eb-9dd5-d1095bb0b01c.png)

## Kendala
- Nomor 2, 3, dan 4 terlalu susah.
- Materi susah
- Membingungkan


