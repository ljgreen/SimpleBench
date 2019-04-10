/*
 * Name: 		Liu, Jian
 * Project: 	PA-1 (Simplebench)
 * Instructor: 	Feng Chen
 * Class:		cs7700-sp18
 * LogonID:		cs770098
 */

#ifndef _LIST_H_
#define _LIST_H_
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
typedef struct list{
	int64_t offset; 	// the ID of the file
	int64_t length;
	struct list* next;
}List;

List* ListHead;	
List *init_list();
List * add_list_node(List *head,List* val);
void delete_list_node(List *head,int64_t offset,int64_t length);
List* lookup_list_node(List *head,int64_t offset,int64_t length);

void clear_list(List *head);
int64_t list_num(List *head);
#endif





