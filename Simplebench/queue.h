/*
 * Name: 		Liu, Jian
 * Project: 	PA-1 (Simplebench)
 * Instructor: 	Feng Chen
 * Class:		cs7700-sp18
 * LogonID:		cs770098
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_
#include "global.h"

typedef struct request{
	double timestamp;
	int64_t pid;
	char type[4];
	int64_t offset;
	int64_t length;
	double latency;
	struct request* next;
}ReqList;

ReqList* ReqHead;
ReqList *new_req_node();

ReqList *init_req_queue();
void push_queue(ReqList *head,ReqList* val);
ReqList * pop_queue(ReqList *head);
void clear_req_queue(ReqList *head);
int list_req_num(ReqList *head);
#endif





