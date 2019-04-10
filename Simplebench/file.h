/*
 *	Written by Jian Liu
 *  March 08, 2018
 */
#ifndef __FILE_H__
#define __FILE_H__

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include "global.h"
typedef struct rw_args{
        int fd;
		char *buffer;
		off_t offset;
		int64_t length;
}RwArgs;
typedef struct thread_args{
		char *direct;
		char *path;
		off_t offset;
		int64_t length;
}ThArgs;

RwArgs* new_args(int64_t buffersize);
int free_args(RwArgs* args);

int create_file(char* path);
int open_file(char* path,int type);
int64_t write_file(RwArgs* args);
int fsync_file(int fd);
int64_t read_file(RwArgs* args);
int delete_file(char* path);
void close_file(int fd);
int create_dirs(char* multidirs);
int delete_dirs(char* path);
void bench_info(const char *fmt, ...) __attribute__((format(printf,1,2)));

#endif 


