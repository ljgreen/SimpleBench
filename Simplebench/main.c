/*
 * Name: 		Liu, Jian
 * Project: 	PA-1 (Simplebench)
 * Instructor: 	Feng Chen
 * Class:		cs7700-sp18
 * LogonID:		cs770098
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "global.h"
double total_read_num = 0;
double total_write_num = 0;
double total_read_amount = 0;
double total_write_amount = 0;
double total_read_time = 0;
double total_write_time = 0;
pthread_rwlock_t rwlock;
pthread_mutex_t mutex;    
pthread_cond_t conflict;

void* read_thread(void* arg){
	int64_t ret;
	ThArgs *p = (ThArgs*) arg;    
	int64_t offset = p->offset;
	int64_t length = p->length;
	double timestamp;
	double latency;
	struct timeval a;
	struct timeval b;
	struct timeval c;
	
	pthread_rwlock_wrlock(&tlock);
	TIMER_GET(a);
	timestamp = a.tv_sec + (double)a.tv_usec/1000000;
	ReqList* node = new_req_node();
	node->timestamp = timestamp;
	node->pid = pthread_self();
	strcpy(node->type,"R");
	node->offset = offset;
	node->length = length;
	push_queue(ReqHead,node);
	pthread_rwlock_unlock(&tlock);
	
	pthread_rwlock_rdlock(&rwlock);
	if(strcmp(p->direct,"F") == 0){
		RwArgs* args = new_args(length);
		args->fd = open_file(p->path,O_RDONLY | O_LARGEFILE);
		args->offset = offset;
		args->length = length;
		ret = read_file(args);
		if(ret < 0){
			dprint("read error int thread %u!\n",pthread_self());
			return FAILURE;
		}
		close_file(args->fd);
		free_args(args);
	} else {
		RwArgs* args = new_args(length);
		args->fd = open_file(p->path,O_RDONLY | O_DIRECT | O_LARGEFILE);
		args->offset = ((offset + getpagesize())/getpagesize())*getpagesize();//offset;
		args->length = length;
		free(args->buffer);
		int rt = posix_memalign((void**)&args->buffer,getpagesize(),length);
		ret = read_file(args);
		if(ret < 0){
			dprint("read error int thread %u new!\n",pthread_self());
			return FAILURE;
		}
		close_file(args->fd);
		free_args(args);
	}
	pthread_rwlock_unlock(&rwlock);
	
	pthread_rwlock_wrlock(&tlock);
	TIMER_GET(b);
	latency = b.tv_usec-a.tv_usec+100000*(b.tv_sec-a.tv_sec);
	total_read_time += latency/1000000;
	node->latency = latency/1000;
	pthread_rwlock_unlock(&tlock);
	
	//dprint("You are now in read thread: %u\n",pthread_self());
	if(p && p->path)
		free(p->path);
	if(p)
		free(p);
	
}

void* write_thread(void* arg){    
	int64_t ret;
	ThArgs *p = (ThArgs*) arg;    
	off_t offset = p->offset;
	int64_t length = p->length;
	double timestamp;
	double latency;
	struct timeval a;
	struct timeval b;
	struct timeval c;
	
	pthread_rwlock_wrlock(&tlock);
	TIMER_GET(a);
	timestamp = a.tv_sec + (double)a.tv_usec/1000000;
	ReqList* node = new_req_node();
	node->timestamp = timestamp;
	node->pid = pthread_self();
	strcpy(node->type,"W");
	node->offset = offset;
	node->length = length;
	push_queue(ReqHead,node);
	pthread_rwlock_unlock(&tlock);
	
	pthread_rwlock_wrlock(&rwlock);
	TIMER_GET(a);
	timestamp = a.tv_sec + (double)a.tv_usec/1000000;
	if(strcmp(p->direct,"F") == 0){
		RwArgs* args = new_args(length);
		args->fd = open_file(p->path,O_WRONLY | O_LARGEFILE);
		args->offset = offset;
		args->length = length;
		fill_buffer_zero(args->buffer,length);
		ret = write_file(args);
		if(ret < 0){
			dprint("write error int thread %u!\n",pthread_self());
			return FAILURE;
		}
		close_file(args->fd);
		free_args(args);
	} else {
		RwArgs* args = new_args(length);
		args->fd = open_file(p->path,O_WRONLY | O_DIRECT | O_LARGEFILE);
		args->offset = ((offset + getpagesize())/getpagesize())*getpagesize();//offset;
		args->length = length;
		free(args->buffer);
		int rt = posix_memalign((void**)&args->buffer,getpagesize(),length);
		ret = write_file(args);
		if(ret < 0){
			dprint("write error int thread %u!\n",pthread_self());
			return FAILURE;
		}
		close_file(args->fd);
		free_args(args);
	}
	TIMER_GET(b);
	latency = b.tv_usec-a.tv_usec+100000*(b.tv_sec-a.tv_sec);
	total_write_time += latency/1000000;
	pthread_rwlock_unlock(&rwlock);
	
	pthread_rwlock_wrlock(&tlock);
	node->latency = latency/1000;
	pthread_rwlock_unlock(&tlock);
	//dprint("You are now in write thread: %u\n",pthread_self());
	
	if(pthread_mutex_lock(&mutex) != 0){            
		return; 
	}
	delete_list_node(ListHead,offset,length);
	pthread_cond_broadcast(&conflict);
	if(pthread_mutex_unlock(&mutex) != 0){            
		return; 
	}
	if(p && p->path)
		free(p->path);
	if(p)
		free(p);
}
int data_write(int fd,int64_t offset,int64_t length){
	int ret;
	int64_t rt;
	int64_t tmpoffset = offset;
	int64_t tmplength = length;
	if(tmplength < BUFFER_SIZE){
		RwArgs* args = new_args(tmplength);
		args->fd = fd;
		args->offset = tmpoffset;
		args->length = tmplength;
		fill_buffer_zero(args->buffer,args->length);
		rt = write_file(args);
		free_args(args);
	} else {
		while(tmplength >= BUFFER_SIZE){
			RwArgs* args = new_args(BUFFER_SIZE);
			args->fd = fd;
			args->offset = tmpoffset;
			args->length = BUFFER_SIZE;
			fill_buffer_zero(args->buffer,args->length);
			rt = write_file(args);
			tmpoffset += BUFFER_SIZE;
			tmplength -= BUFFER_SIZE;
			free_args(args);
		}
		if(tmplength > 0){
			RwArgs* args = new_args(tmplength);
			args->fd = fd;
			args->offset = tmpoffset;
			args->length = tmplength;
			fill_buffer_zero(args->buffer,args->length);
			rt = read_file(args);
			free_args(args);
		}
	}
	return 0;
}

int main(int argc, char **argv){
	int opt;
	int ret; 
	int fd;
	FILE* File;
	int ThreadNum = 0;
	double Duration = 0;
	int64_t Range = 0;
	int64_t Size = 0;
	char Pattern[REG_LEN] = "S";
	char Direct[REG_LEN] = "F";
	char Type[REG_LEN] = "W";
	char TargetFile[MAX_NAME_LEN];
	char TraceFile[MAX_NAME_LEN];
	double t = 0;
	double t1 = 0;
	while ((opt = getopt(argc, argv, "e:f:r:s:t:p:q:d:o:h")) != -1) {
		switch (opt) {
			case 'e':
				Duration = atof(optarg);
				break;
			case 'f':
				strcpy(TargetFile,optarg);
				break;
			case 'r':
				Range = atol(optarg);
				Range = Range * 1024 * 1024;
				break;
			case 's':
				Size = atol(optarg);
				Size = Size * 1024;
				break;
			case 't':
				strcpy(Type,optarg);
				break;
			case 'p':
				strcpy(Pattern,optarg);
				break;
			case 'q':
				ThreadNum = atoi(optarg);
				break;
			case 'd':
				strcpy(Direct,optarg);
				break;
			case 'o':
				strcpy(TraceFile,optarg);
				break;
			case 'h':
 				puts("sudo ./main					\n");
				puts("Arguments:					\n");
				puts("-e		[Duration (s)]\n");
				puts("-f		[Device or File]\n");
				puts("-r 		[Range (MBs)]\n");
				puts("-s 		[Request Size (KBs)]\n");
				puts("-t		[Type (Read:R or Write:W)]\n");
				puts("-p		[Pattern (Sequential:S or Random:R)]\n");
				puts("-q 		[Num of Threads (1-32)]\n");
				puts("-d 		[O_DIRECT or not]\n");
				puts("-o	 	[Output Trace]\n");
				puts("-h		[Help]\n");
				return;
			default:
				dprint("Please enter [./main -h] for help!\n");
				return;
		}
	} 
	if(Duration <= 0 || !TargetFile || Range <= 0 || Size <= 0 || ThreadNum <= 0 || !TraceFile){
		puts("The input argument is error!\n");
		return FAILURE;
	}
	if(!(strcmp(Direct,"T") == 0 || strcmp(Direct,"F") == 0) || !(strcmp(Pattern,"S") == 0 || strcmp(Pattern,"R") == 0) || \
		!(strcmp(Type,"W") == 0 || strcmp(Type,"R") == 0) ){
		puts("The input argument is error!\n");
		return FAILURE;
	} 
	dprint("The duration of the test in seconds is: %.3fs\n",Duration);
	dprint("The target device or file is: %s\n",TargetFile);
	dprint("The accessible range in Megabytes is: %.3fMB\n",Range/(1024*1024));
	dprint("The request size in Kilobytes is: %.3fKB\n",Size/1024);
	dprint("Read (R) requests or Write (W) requests is: %s\n",Type);
	dprint("The pattern (S for sequential, R for random) is: %s\n",Pattern);
	dprint("The queue depth (parallel threads) is: %d\n",ThreadNum);
	dprint("Use direct I/O (T) or (F) is: %s\n",Direct);
	dprint("The output trace file name is: %s\n",TraceFile);
	
	pthread_rwlock_init(&rwlock,NULL);
	if(pthread_mutex_init(&mutex, 0)|| pthread_cond_init(&conflict, 0)){        
		puts("Failed to init mutex or conflict!");        
		return FAILURE;   
	}
	trace_stop = 0;
	ReqHead = init_req_queue();
	ret = trace_init(TraceFile);
	ListHead = init_list();
	if(ret < 0){
		puts("Init trace error!\n");
		return FAILURE;
	}
	struct threadpool *pool = threadpool_init(ThreadNum, 100);
	TIMER_DECLARE(d,e);
	TIMER_GET(d);
	t = d.tv_sec + (double)d.tv_usec/1000000;
	if(access(TargetFile,0) != 0){
		ret = create_file(TargetFile);
		fd = open_file(TargetFile,O_WRONLY);
		data_write(fd,0,Range);
		close_file(fd);
	}
	int64_t offset = 0;
	int64_t length = 0;
	while((t1-t) <= Duration){
		if(strcmp(Pattern,"S") == 0){
			if(offset >= Range){
				offset = 0;
			}
			if((Range - offset) < Size){
				length = (Range - offset);
			} else {
				length = Size;
			}
			if(strcmp(Type,"W") == 0){
				if(pthread_mutex_lock(&mutex) != 0){            
					return; 
				}
				List* node = lookup_list_node(ListHead,offset,length);
				if(node == NULL){
					node = malloc(sizeof(List));
					node->offset = offset;
					node->length = length;
					ListHead = add_list_node(ListHead,node);
					if(pthread_mutex_unlock(&mutex) != 0){            
						return; 
					}
					total_write_num += 1;
					total_write_amount += length;
					ThArgs* thargs = malloc(sizeof(ThArgs));
					thargs->direct = malloc(REG_LEN);
					strcpy(thargs->direct,Direct);
					thargs->path = malloc(MAX_NAME_LEN);
					strcpy(thargs->path,TargetFile);
					thargs->offset = offset;
					thargs->length = length;
					threadpool_add_job(pool, write_thread, thargs); 
					offset += length;
				} else {
					while((node = lookup_list_node(ListHead,offset,length)) != NULL){
						pthread_cond_wait(&conflict, &mutex);
					}
					node = malloc(sizeof(List));
					node->offset = offset;
					node->length = length;
					ListHead = add_list_node(ListHead,node);
					if(pthread_mutex_unlock(&mutex) != 0){            
						return; 
					}
					total_write_num += 1;
					total_write_amount += length;
					ThArgs* thargs = malloc(sizeof(ThArgs));
					thargs->direct = malloc(REG_LEN);
					strcpy(thargs->direct,Direct);
					thargs->path = malloc(MAX_NAME_LEN);
					strcpy(thargs->path,TargetFile);
					thargs->offset = offset;
					thargs->length = length;
					threadpool_add_job(pool, write_thread, thargs); 
					offset += length;
					
				}
			} else {
				total_read_num += 1;
				total_read_amount += length;
				ThArgs* thargs = malloc(sizeof(ThArgs));
				thargs->direct = malloc(REG_LEN);
				strcpy(thargs->direct,Direct);
				thargs->path = malloc(MAX_NAME_LEN);
				strcpy(thargs->path,TargetFile);
				thargs->offset = offset;
				thargs->length = length;
				threadpool_add_job(pool, read_thread, thargs); 
				offset += length;
			} 
			
		} else {
			int rand = -1;
			fd = open("/dev/urandom",O_RDONLY); 
			while(rand < 0){
				ret = read(fd,&rand,sizeof(int));
			}
			close(fd);
			offset = rand % Range;
			while((Range - offset) < Size){
				fd = open("/dev/urandom",O_RDONLY); 
				while(rand < 0){
					ret = read(fd,&rand,sizeof(int));
				}
				close(fd);
				offset = rand % Range;
			}
			length = Size;
			if(strcmp(Type,"W") == 0){
				if(pthread_mutex_lock(&mutex) != 0){            
					return; 
				}
				List* node = lookup_list_node(ListHead,offset,length);
				if(node == NULL){
					node = malloc(sizeof(List));
					node->offset = offset;
					node->length = length;
					ListHead = add_list_node(ListHead,node);
					if(pthread_mutex_unlock(&mutex) != 0){            
						return; 
					}
					total_write_num += 1;
					total_write_amount += length;
					ThArgs* thargs = malloc(sizeof(ThArgs));
					thargs->direct = malloc(REG_LEN);
					strcpy(thargs->direct,Direct);
					thargs->path = malloc(MAX_NAME_LEN);
					strcpy(thargs->path,TargetFile);
					thargs->offset = offset;
					thargs->length = length;
					threadpool_add_job(pool, write_thread, thargs); 
				} else {
					while((node = lookup_list_node(ListHead,offset,length)) != NULL){
						pthread_cond_wait(&conflict, &mutex);
					}
					node = malloc(sizeof(List));
					node->offset = offset;
					node->length = length;
					ListHead = add_list_node(ListHead,node);
					if(pthread_mutex_unlock(&mutex) != 0){            
						return; 
					}
					total_write_num += 1;
					total_write_amount += length;
					ThArgs* thargs = malloc(sizeof(ThArgs));
					thargs->direct = malloc(REG_LEN);
					strcpy(thargs->direct,Direct);
					thargs->path = malloc(MAX_NAME_LEN);
					strcpy(thargs->path,TargetFile);
					thargs->offset = offset;
					thargs->length = length;
					threadpool_add_job(pool, write_thread, thargs); 
				}
				
			} else {
				total_read_num += 1;
				total_read_amount += length;
				ThArgs* thargs = malloc(sizeof(ThArgs));
				thargs->direct = malloc(REG_LEN);
				strcpy(thargs->direct,Direct);
				thargs->path = malloc(MAX_NAME_LEN);
				strcpy(thargs->path,TargetFile);
				thargs->offset = offset;
				thargs->length = length;
				threadpool_add_job(pool, read_thread, thargs); 
			} 
		}
		TIMER_GET(e)
		t1 = e.tv_sec + (double)e.tv_usec/1000000;
		//dprint("Running time = %f,Duration = %ld\n",(t1-t),Duration);
	}
	/*--------------Resource released-----------------*/
	trace_stop = 1;
	pthread_rwlock_destroy(&rwlock);
	threadpool_destroy(pool);
	trace_destroy();
	pthread_mutex_destroy(&mutex);    
	pthread_cond_destroy(&conflict);
	clear_list(ListHead);
	
	dprint("-----------------Summary----------------\n");
	t = Duration;
	dprint("Total Duration = %.3fs\n",t);
	dprint("Total read and write requests number is: %.2f\n",(total_read_num + total_write_num));
	dprint("Total volume of data transferrd in MBs:%.2fMB\n",(total_read_amount+total_write_amount)/(1024*1024));
	dprint("Average latency in milliseconds is : %.2fms\n",(total_read_time + total_write_time)*1000/(total_read_num + total_write_num));
	dprint("Throughput in IOPS = %.2f/s\n",(total_read_num + total_write_num)/t);
	dprint("Bandwidth in MB/second = %.2fMB/s\n",(total_read_amount + total_write_amount)/(1024*1024)/t);
	
	dprint("-------------------End------------------\n");
	return SUCCESS;
}




