/*
 * Name: 		Liu, Jian
 * Project: 	PA-1 (Simplebench)
 * Instructor: 	Feng Chen
 * Class:		cs7700-sp18
 * LogonID:		cs770098
 */
#ifndef __GLOBAL__
#define __GLOBAL__
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>  
#include <sys/shm.h>  
#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "file.h"
#include "threadpool.h"
#include "trace.h"
#include "list.h"
#include "queue.h"
#define MAX(a,b) ({(a > b) ? a : b;})
#define MIN(a,b) ({(a < b) ? a : b; })

#define SUCCESS 			(0)
#define FAILURE				(-1)
#define MAX_NAME_LEN 		(0x64)
#define REG_LEN 			(0x4)
#define BUFFER_SIZE 		(0x6400000)

#define OSD_DEBUG 			(1)
#if OSD_DEBUG
#define dprint(fmt,args...) \
	do {\
		bench_info(fmt,##args);\
	} while(0)
		
#else
#define dprint(fmt,args...) do {} while(0) 
#endif

#define TIMER_DECLARE(a,b) 	struct timeval a,b;
#define TIMER_GET(a) 		gettimeofday(&a,NULL);


/*#define TIMER_DECLARE_NEW(b,e) struct timeval b,e;
#define TIMER_BEGIN(b) gettimeofday(&b, NULL);
#define TIMER_END(t,b,e) gettimeofday(&e, NULL); \
		(t)=e.tv_usec-b.tv_usec+100000*(e.tv_sec-b.tv_sec);

#define TIMER_DECLARE_NEW(b,e) struct timespec b,e;
#define TIMER_BEGIN(b) clock_gettime(CLOCK_THREAD_CPUTIME_ID,&b);
#define TIMER_END(t,b,e) clock_gettime(CLOCK_THREAD_CPUTIME_ID,&e); \
	do{											\
		if(e.tv_nsec < b.tv_nsec){				\
			(t)=(e.tv_sec-b.tv_sec - 1)*1000+(double)(e.tv_nsec-b.tv_nsec + 1000000000)/1000000;\
		} else {														\
			(t)=(e.tv_sec-b.tv_sec)*1000+(double)(e.tv_nsec-b.tv_nsec)/1000000;\
		}																\
	} while(0)*/
#endif
