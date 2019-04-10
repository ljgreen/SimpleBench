/*
 * Name: 		Liu, Jian
 * Project: 	PA-1 (Simplebench)
 * Instructor: 	Feng Chen
 * Class:		cs7700-sp18
 * LogonID:		cs770098
 */

#include "list.h"
#include "global.h"

List *init_list(){	
	List *head = (List *)malloc(sizeof(List));
	head->offset = -1;
	head->length = -1;
    head->next = NULL;
	return head;
}

List *add_list_node(List *head,List* val){
	val->next = head->next;
	head->next = val;
	return head;
}
void delete_list_node(List *head,int64_t offset,int64_t length){
	List* p = head;
	List* q;
	while(p){
		if(p->next->offset == offset && p->next->length == length){
			q = p->next;
			p->next = q->next;
			free(q);
			return;
		}
		
		p = p->next;
	}
	return;
}

List* lookup_list_node(List *head,int64_t offset,int64_t length){
	List* p = head;
	List* q;
	while(p->next){
		if(!((p->next->offset + length) <= offset || (offset + length) <= p->next->offset)){
			q = p->next;
			//dprint("qq offset = %ld,length = %ld\n",q->offset,q->length);
			return q;
		}
		p = p->next;
	}
	return NULL;
}

void clear_list(List *head){
	List* p = head;
	List* q;
	while(p->next){
		q = p->next;
		p->next = q->next;
		free(q);
	}
	if(p)
		free(p);
}

int64_t list_num(List* head){
	int64_t count = 0;
	List *p = head;
	while(p->next != NULL){
		count++;
		p = p->next;
	}
	return count;
}


