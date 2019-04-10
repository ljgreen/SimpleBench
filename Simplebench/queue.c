/*
 * Name: 		Liu, Jian
 * Project: 	PA-1 (Simplebench)
 * Instructor: 	Feng Chen
 * Class:		cs7700-sp18
 * LogonID:		cs770098
 */

#include "queue.h"

ReqList *new_req_node(){
	ReqList *node = (ReqList *)malloc(sizeof(ReqList));
	node->timestamp = -1;
	node->pid = -1;
	node->offset = -1;
	node->length = -1;
	node->latency = -1;
    node->next = NULL;
	return node;
}

ReqList *init_req_queue(){	
	ReqList *head = (ReqList *)malloc(sizeof(ReqList));
	head->timestamp = -1;
	head->pid = -1;
	head->offset = -1;
	head->length = -1;
	head->latency = -1;
    head->next = NULL;
	return head;
}
void push_queue(ReqList *head,ReqList* val){
	val->next = head->next;
	head->next = val;
}
ReqList * pop_queue(ReqList *head){
	ReqList* p = head;
	ReqList* q;
	while(p->next){
		q = p->next;
		if(q->next == NULL && q->latency != -1){
			p->next = q->next;
			return q;
		}
		p = p->next;
	}
	return NULL;
}
void clear_req_queue(ReqList *head){
	ReqList* p = head;
	ReqList* q;
	while(p->next){
		q = p->next;
		p->next = q->next;
		free(q);
	}
	if(p)
		free(p);
}

int list_req_num(ReqList* head){
	int count = 0;
	ReqList *p = head;
	while(p->next != NULL){
		count++;
		p = p->next;
	}
	return count;
}


