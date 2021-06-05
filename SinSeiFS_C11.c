#define FUSE_USE_VERSION 28
#include<fuse.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<errno.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdbool.h>

char dirpath[50] = "/home/solxius/Downloads";
char ext[100000] = "\0";
int id = 0;

void substr(char *s, char *sub, int y, int z);
char *encrypt(char* str, bool cek);
char *bagianAkhir(char *str);
char *jalanCek(char *str);
char *campurPath(char *fin, char *str1, const char *str2);
int cekAtoz(char *str);
void loopEncry(char *str, int flag);
void fullencr(char *str, int flag);
static int xmp_getattr(const char *path, struct stat *stbuf);
static int xmp_access(const char *path, int mask);
static int xmp_readlink(const char *path, char *buf, size_t size);
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi);
static int xmp_mknod(const char *path, mode_t mode, dev_t rdev);
static int xmp_mkdir(const char *path, mode_t mode);
static int xmp_unlink(const char *path);
static int xmp_rmdir(const char *path);
static int xmp_symlink(const char *from, const char *to);
static int xmp_rename(const char *from, const char *to);
static int xmp_link(const char *from, const char *to);
static int xmp_chmod(const char *path, mode_t mode);
static int xmp_chown(const char *path, uid_t uid, gid_t gid);
static int xmp_truncate(const char *path, off_t size);
static int xmp_open(const char *path, struct fuse_file_info *fi);
static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
static int xmp_statfs(const char *path, struct statvfs *stbuf);		     
static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi);
static int xmp_release(const char *path, struct fuse_file_info *fi);
static int xmp_fsync(const char *path, int isdatasync, struct fuse_file_info *fi);

void substr(char *s, char *sub, int y, int z) {
   int aa = 0;
   while (aa < z) {
      sub[aa] = s[y + aa];
      aa++;
   }
   sub[aa] = '\0';
}

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

char *bagianAkhir(char *str){
	if(!strcmp(str, "/")) {
		return NULL;
	}
	return strrchr(str, '/') + 1;
}

char *jalanCek(char *str){
	bool encr;
	int start, id;
	encr = false; 
	start = 1; 
	id = strchr(str + start, '/') - str - 1;
	char curpos[1024];
	while(id < strlen(str)){
		strcpy(curpos, "");
		strncpy(curpos, str + start, id - start + 1);
		curpos[id - start + 1] = '\0';
		if(encr){
			encrypt(curpos, 0);
			strncpy(str + start, curpos, id - start + 1);
		}
		if(!encr && strstr(str + start, "AtoZ_") == str + start) encr = true;
		start = id + 2;
		id = strchr(str + start, '/') - str - 1;
	}
	id = strlen(str); id--;
	strncpy(curpos, str + start, id - start + 1);
	curpos[id - start + 1] = '\0';
	if(encr){
		encrypt(curpos, 1);
		strncpy(str + start, curpos, id - start + 1);
	}
	return str;
}

char *campurPath(char *fin, char *str1, const char *str2){
	strcpy(fin, str1);
	if(!strcmp(str2, "/")) {
		return fin;
	}
	if(str2[0] != '/'){
		fin[strlen(fin) + 1] = '\0';
		fin[strlen(fin)] = '/';
	}
	strcat(fin, str2);
	return fin;
}

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

void loopEncry(char *str, int flag){
	struct dirent *dp;
	DIR *dir = opendir(str);
	
	if(!dir) return;
	
	while((dp = readdir(dir)) != NULL)
	{
		if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
        	char path[2000000], name[1000000], newname[1000000];
        	campurPath(path, str, dp->d_name);
			strcpy(name, dp->d_name);
			if(flag == 1) campurPath(newname, str, encrypt(name, 1));
			else if(flag == -1) campurPath(newname, str, encrypt(name, 1));
			if(dp->d_type == DT_REG) rename(path, newname);
			else if(dp->d_type == DT_DIR)
			{
				rename(path, newname);
				loopEncry(newname, flag);
			}
        }
	}
}

void fullencr(char *str, int flag){
	struct stat add;
	stat(str, &add);
	if(!S_ISDIR(add.st_mode)) return;
	printf("%s\n", str);
	loopEncry(str, flag);
}

static int xmp_getattr(const char *path, struct stat *stbuf){
	int res;
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	res = lstat(jalanCek(fpath), stbuf);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_access(const char *path, int mask){
	int res;
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	res = access(jalanCek(fpath), mask);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size){
	int res;
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	res = readlink(jalanCek(fpath), buf, size - 1);
	if (res == -1) return -errno;
	buf[res] = '\0';
	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	int res = 0;
	
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
		if(flag == 1)
		{
			if(de->d_type == DT_REG) encrypt(nama, 1);
			else if(de->d_type == DT_DIR && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) encrypt(nama, 0);
			res = (filler(buf, nama, &st, 0));
			if(res!=0) break;
		}
		else
		{
			res = (filler(buf, nama, &st, 0));
			if(res!=0) break;
		}
	}
	closedir(dp);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	jalanCek(fpath);
	int res;
	
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0) res = close(res);
	} 
	else if (S_ISFIFO(mode)) res = mkfifo(fpath, mode);
	else res = mknod(fpath, mode, rdev);
	if (res == -1) return -errno;
	
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	
	int res;

	res = mkdir(jalanCek(fpath), mode);
	if (res == -1) return -errno;
	
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

static int xmp_unlink(const char *path){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	int res;

	res = unlink(jalanCek(fpath));
	if (res == -1) return -errno;
	return 0;
}

static int xmp_rmdir(const char *path){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	int res;

	res = rmdir(jalanCek(fpath));
	if (res == -1) return -errno;
	return 0;
}

static int xmp_symlink(const char *from, const char *to){
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to){    
    char ffrom[1000];
	campurPath(ffrom, dirpath, from);
	
    char final[1000];
	campurPath(final, dirpath, to);
	
	int res;

	res = rename(jalanCek(ffrom), jalanCek(final));
	
	if (res == -1)
		return -errno;
	
	int fromm = 0, too = 0;
	char checking[1024], cek2[1024];
    if(bagianAkhir(ffrom) == 0) return 0;
    char absPath[1000000];
    strcpy(absPath, bagianAkhir(ffrom));
    substr(absPath, checking, 0, 5);
	if(strcmp(checking, "AtoZ_") == 0){
		fromm = 1;
	}
	
    if(bagianAkhir(final) == 0) return 0;
    strcpy(absPath, bagianAkhir(final));
    substr(absPath, checking, 0, 5);
	if(strcmp(cek2, "AtoZ_") == 0){
		too = 1;
	}
	
	printf("rename");
	
	if(fromm == 0 && too == 1){
		printf("log created");
		FILE *filez1;
		filez1 = fopen("sebuah.log", "a+");
		fprintf(filez1, "%s → %s\n", from, to);
		fclose(filez1);
		
	}

	return 0;
}

static int xmp_link(const char *from, const char *to){
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	int res;

	res = chmod(jalanCek(fpath), mode);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	int res;

	res = lchown(jalanCek(fpath), uid, gid);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_truncate(const char *path, off_t size){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	int res;

	res = truncate(jalanCek(fpath), size);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	int res;

	res = open(jalanCek(fpath), fi->flags);
	if (res == -1) return -errno;
	close(res);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	int fd = 0;
	int res = 0;

	(void) fi;
	fd = open(jalanCek(fpath), O_RDONLY);
	if (fd == -1) return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1) res = -errno;
	close(fd);
	
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi){
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	int fd;
	int res;

	(void) fi;
	fd = open(jalanCek(fpath), O_WRONLY);
	if (fd == -1) return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1) res = -errno;

	close(fd);
	return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	char fpath[1000];
	campurPath(fpath, dirpath, path);
	int res;

	res = statvfs(jalanCek(fpath), stbuf);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {
	char fpath[1000];
	campurPath(fpath, dirpath, path);
    (void) fi;

    int res;
    res = creat(jalanCek(fpath), mode);
    if(res == -1) return -errno;
	
    close(res);
    return 0;
}


static int xmp_release(const char *path, struct fuse_file_info *fi){
	(void) path;
	(void) fi;
	return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}


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

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
