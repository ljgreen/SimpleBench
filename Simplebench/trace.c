/*
 * Name: 		Liu, Jian
 * Project: 	PA-1 (Simplebench)
 * Instructor: 	Feng Chen
 * Class:		cs7700-sp18
 * LogonID:		cs770098
 */

#include "trace.h"

static FILE* fp;

int64_t time_init = 0;

void fill_buffer_zero(char* buffer, int64_t length){
	memset(buffer,0,length);
}

int write_trace(double timestamp,int64_t pid, char* type,int64_t offset,int64_t length,double latency){
	int count = 0;
	char valid = '\t';
	char new_line = '\n';
	if(time_init == 0){
		time_init = timestamp;
		timestamp = 0;
	} else {
		timestamp -= time_init;
	}
	if (fprintf(fp, "%f	%ld	%s	%ld	%ld	%f\n",timestamp, pid, type, offset, length, latency) < 0) {
		pthread_rwlock_unlock(&tlock);
		return FAILURE;
	}
	return SUCCESS;
}

void* trace_thread(void *arg){
	
	int i;
	ReqList* p = NULL;
	while (1) {
		pthread_rwlock_wrlock(&tlock);
		p = pop_queue(ReqHead);
		if(p != NULL){
			write_trace(p->timestamp,p->pid, p->type,p->offset,p->length,p->latency);
			free(p);
		}
		pthread_rwlock_unlock(&tlock);
		usleep(10);
		if(trace_stop)
			break;
	}
	return NULL ;
}

void start_trace_thread(){
	pthread_create(&pt, NULL, trace_thread, NULL);
}
void stop_trace_thread(){
	pthread_join(pt, NULL);
	pthread_rwlock_destroy(&tlock);
	fclose(fp);
}
int trace_init(char* path){
	create_file(path);
	fp = fopen(path, "w+");
	if ((fp = fopen(path, "w+")) <= 0) {
		dprint("init_trace_job failed!\n");
		fclose(fp);
		return FAILURE;
	}
	pthread_rwlock_init(&tlock,NULL);
	start_trace_thread();
}

void trace_destroy(){
	stop_trace_thread();
	free(ReqHead);
}


