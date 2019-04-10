/*
 * Name: 		Liu, Jian
 * Project: 	PA-1 (Simplebench)
 * Instructor: 	Feng Chen
 * Class:		cs7700-sp18
 * LogonID:		cs770098
 */

#ifndef _TRACE_H_
#define _TRACE_H_
#include "global.h"

void fill_buffer_zero(char* buffer, int64_t length);
int write_trace(double timestamp,int64_t pid, char* type,int64_t offset,int64_t length,double latency);
int trace_init(char* path);
void trace_destroy();

pthread_t pt; 
pthread_rwlock_t tlock;
int trace_stop;
#endif





