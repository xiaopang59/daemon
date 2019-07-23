#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include "list.h"

#define MAX_PATH 1024

extern struct list *tmp;    // 引用外部变量，用于加载链表
extern int status;

void sys_err(const char *str)
{
    	perror(str);
    	exit(0);
}

/*
 * 把文件信息加载到链表中
 * */
void LoadDir(char *dirname)  
{
	struct stat st; 

    	if(stat(dirname, &st) < 0)
        	sys_err("stat");

    	if((st.st_mode & S_IFMT) == S_IFDIR)
        	ReadFile(dirname);

    	AddList(tmp, dirname, &st);    // 加载
}

/*
 * 遍历目录中的文件
 * */
void ReadFile(char *dirname) 
{
    	char name[MAX_PATH];
    	struct dirent *read;
    	DIR *dir;

    	if((dir = opendir(dirname)) == NULL)
        	sys_err("opendir");

    	while((read = readdir(dir)) != NULL) {
        	if(strcmp(read->d_name,".") == 0 || strcmp(read->d_name, "..") == 0)
            		continue;
        	if(strlen(dirname) + strlen(read->d_name) + 2 > sizeof(name))
            		continue;
        	else {
            		sprintf(name, "%s/%s", dirname, read->d_name);
            		LoadDir(name);
	 	}
     	}

    	closedir(dir);
}

/*
 * 对比两个链表元素，如果第二个链表中存在，但是第一个链表没有，就增加到第一个链表，并写log
 * */
void ContrastList(struct list *list1, struct list *list2)
{
    	struct list *first = list1->next;
    	struct list *second = list2->next;

    	while (second != NULL) {
        	if(SearchNode(first, second) != 0) {
            	AddList(first, second->name, &second->st);
            	WriteLog(second, "create");
        	}
        	second = second->next;
    	}
}

/*
 * 查找链表中是否存在该元素，有就返回0，并更改第一个链表中的status位，没有就返回-1
 * */
int  SearchNode(struct list *list, const struct list *node) 
{
    	struct list *first = list;

    	while(first != NULL) {
        	if(node->st.st_ino == first->st.st_ino && strcmp(first->name, node->name) == 0) {
			first->status = status % 2;
                	return 0;
		}
		else if (node->st.st_ino != first->st.st_ino && strcmp(first->name, node->name) == 0) {
			first->status = status % 2;
			memcpy(&first->st, &node->st, sizeof(struct stat));
			WriteLog((struct list*)node, "update");
                	return 0;		
		}

        	first = first->next;
    	}

    	return -1;
}

/*
 * 检查第一个链表中的status这个字段，来判断文件是否已删除
 * */
void CheckDeleteNode(struct list *list)
{
    	struct list *p = list->next;
	
    	while(p != NULL) {
        	if(p->status != status % 2) {
            		printf("inode = %d, name = %s, status = %d, size = %d\n", (int)p->st.st_ino, p->name, p->status, (int)p->st.st_size);
            		WriteLog(p, "delete");
            		DeleteNode(list, p->name, &p->st);
        	}
        	p = p->next;
    	}
}

/*
 * 写日志
 * */
void WriteLog(struct list *list, char *buf)
{
    	int fd;
    	time_t t;
    	char str[1024] = {0};
	char fileBuf[1024];

	if (getcwd(fileBuf, sizeof(fileBuf)) == NULL)
		sys_err("getcwd");	
	strcat(fileBuf, "/filechangelog");

    	fd = open(fileBuf, O_CREAT|O_RDWR|O_APPEND, 0664);
    	time(&t);
    	sprintf(str, "%s\t%s\t%s", buf, list->name, ctime(&t));
    	write(fd, str, strlen(str));
    	close(fd);
}

