/*
  *  Written by Jian Liu
  *  March 08, 2018
  */
#include "file.h"

RwArgs* new_args(int64_t buffersize){
	RwArgs* args = malloc(sizeof(RwArgs));
	args->fd = -1;
	args->length = -1;
	args->offset = -1;
	args->buffer = malloc(buffersize);
	return args;
}
int free_args(RwArgs* args){
	if(args->buffer)
		free(args->buffer);
	free(args);
}
void __attribute__((format(printf,1,2)))
bench_info(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
}

int create_file(char* path){
	int ret;
	if(access(path,0) == 0){
		return 0;
	}
	char* str = strrchr(path,'/');
	if(!str){
		dprint("create_file path = %s\n",path);
		ret = creat(path,0777);
	} else {
		*str = '\0';
		ret = create_dirs(path);
		*str = '/';
		ret = creat(path,0777);
	}
	return ret;
}
int open_file(char* path,int type){
	if(!path){
		dprint("1File does not exist!\n");
		return -1;
	} else {
		if(access(path,0) != 0){
			dprint("2File does not exist!\n");
			return -1;
		}
	}	
	int fd = 0;
	fd = open(path,type); 
	if (fd < 0){
		dprint("3Open file error!\n");
		return -1;
	}
	return fd;
}
int64_t write_file(RwArgs* args){
	if(args->length == 0){
		return 0;
	}
	ssize_t ret = 0;
	//ret = lseek(args->fd,args->offset,SEEK_SET);
	//ret = write(args->fd,args->buffer,args->length);
	ret = pwrite(args->fd,args->buffer,args->length,args->offset);
	return (int64_t)ret;
}
int fsync_file(int fd){
	int ret;
	ret = fsync(fd);
	return ret;
}
int64_t read_file(RwArgs* args){
	if(args->length == 0){
		return 0;
	}
	ssize_t ret = 0;
	ret = pread(args->fd, args->buffer, args->length,args->offset);
	return (int64_t)ret;
}
int delete_file(char* path){
	int ret;
	ret = remove(path);
	if(ret < 0){
		dprint("File delete error!\n");
	}
	return ret;
} 
	
void close_file(int fd){
	close(fd);
}

int create_dirs(char* multidirs){
	int i,length,ret;
	char path[MAX_NAME_LEN];
	memcpy(path,multidirs,MAX_NAME_LEN);
	length = strlen(path);
	for(i = 1;i < length;i++){
		if(path[i] == '/'){
			path[i] = '\0';
			if(access(path, F_OK) != 0){
				ret = mkdir(path, 0777);
				if(ret < 0){
					dprint("Create dirs error!\n");
					return -1;
				}
			} 
			path[i] = '/';
		}
	}
	if(length > 0){
		if((access(path, F_OK) != 0)){
			ret = mkdir(path, 0777);
			if(ret < 0){
				dprint("Create dirs error!\n");
				return -1;
			}
		}
	}
	
	return 0;
}

int delete_dirs(char* path){
	int ret = 0;
	char dir_name[MAX_NAME_LEN];
	DIR *dir = NULL;
	struct dirent *ent = NULL;
	
	if ((dir = opendir(path)) == NULL)
		return -1;
	while ((ent = readdir(dir)) != NULL) {
		if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")){
			continue;
		}
		sprintf(dir_name, "%s/%s", path, ent->d_name);
		if (ent->d_type == DT_DIR){
			ret = delete_dirs(dir_name);
		} else {
			ret = unlink(dir_name);
		}
		if (ret != 0)
			return -1;
	}
	closedir(dir);
	ret = rmdir(path);
	if (ret != 0)
		return -1;
	return 0;
}

