#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "list.h"

extern int status;

/*
 * 创建链表空间
 * */
void CreateList(struct list **list) 
{
	struct list *head = NULL;
	
	head = (struct list*)malloc(sizeof(struct list));
	if (head == NULL) {
		printf("malloc error!\n");
		exit(-1);	
	}
	
	*list = head;
}

/*
 * 添加链表元素
 * */
void AddList(struct list *list, char *name, struct stat *st) 
{
    	struct list *p = list;
	struct list *tmp = NULL; 

    	while(p->next != NULL) {
        	p = p->next;
    	}   
	
    	CreateList(&tmp);
    	tmp->next = p->next;
    	p->next = tmp;
    	strcpy(tmp->name, name);
    	memcpy(&tmp->st, st, sizeof(struct stat));
	tmp->status = status % 2;
}

/*
 * 遍历打印链表元素，调试的时候使用
 * */
void ShowList(struct list *list)
{
    	struct list *p = list->next;

    	while(p != NULL) {
		printf("%s\t%d\t%d\n", p->name, p->status, (int)p->st.st_ino);
        	p = p->next;
    	}
}

/*
 * 删除链表中特定的元素
 * */
void DeleteNode(struct list *list, char *name, struct stat *st) 
{
    	struct list *p = list;
	struct list *tmp = NULL;

    	while (p->next != NULL) {
        	if( (strcmp(p->next->name, name) == 0) && (p->next->st.st_ino == st->st_ino) )
            	break;
        	p = p->next;
    	}
     	
	tmp = p->next;
	p->next = tmp->next;
    	free(tmp);
}

/*
 * 释放链表
 * */
void FreeList(struct list *list)
{
    	struct list *p = list->next;
	struct list *tmp = NULL;

    	while(p != NULL) {
       	tmp = p->next;
        	free(p);
        	p = tmp;
   	}
    	list->next = NULL;
}

/*
 * 采用冒泡排序法对链表进行排序，用过文件的inode号排序
 * */
void ListSort(struct list *list)
{
    	struct list *i = list->next;
    	struct list *j = list->next;
    	struct list *tmp = NULL;

	CreateList(&tmp);

    	while(i->next) {
        	while(j->next) {
            	if(j->st.st_ino > j->next->st.st_ino) {
                		strcpy(tmp->name, j->name);
                		strcpy(j->name, j->next->name);
                		strcpy(j->next->name, tmp->name);

                		memcpy(&tmp->st, &j->st, sizeof(struct stat));
                		memcpy(&j->st, &j->next->st, sizeof(struct stat));
                		memcpy(&j->next->st, &tmp->st, sizeof(struct stat));

				tmp->status = j->status;
				j->status = j->next->status;
				j->next->status = tmp->next->status;
            		}

            	j = j->next;
        	}	
        	j = list->next;
        	i = i->next;
    	}
    	free(tmp);
}
