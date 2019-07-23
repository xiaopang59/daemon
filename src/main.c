#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "list.h"

#define MAX_PATH 1024

struct list *tmp;
int status;

void daemonize(void)
{
	pid_t pid;
    	if ((pid = fork()) < 0)  // 创建进程
		sys_err("fork");
    	else if(pid != 0)   	// 父进程挂掉
		exit(0);

    	setsid();   	// 修改自身的会话id

    	if(chdir("/home/wmb") < 0)  	// 修改运行目录
        	sys_err("chdir");

    	umask(0);   	// 修改权限掩码
    	close(0);   	// 0,1,2三个文件描述符重定向到NULL
    	open("/dev/null", O_RDWR);
    	dup2(0, 1);
    	dup2(0, 2);
}

int main(void)
{
	struct list *first = NULL;	// 正常维护的链表
    	struct list *second = NULL;    // 用来比对文件是否增加或删除的链表
	char fileBuf[1024];
	
	if (getcwd(fileBuf, sizeof(fileBuf)) == NULL)
		sys_err("getcwd");	
	strcat(fileBuf, "/dir");

	CreateList(&first);  
    	CreateList(&second);

    	daemonize();    

	tmp = first;	// 把第一次first的地址给tmp，然后通过load_list函数来加载目录
    	LoadDir(fileBuf);  // 第一次加载目录

    	while(1) {
        	sleep(3); 	// 运行周期10分钟
		status++;
        	tmp = second;	// 加载第二遍目录列表，用来和first链表比对
        	LoadDir(fileBuf); 	// 加载第二遍
        	ContrastList(first, second); 	// 对比两个链表  
        	CheckDeleteNode(first); 	// 把已经删除的文件所对应的first中的链表元素删除
        	FreeList(second);		// 释放second的内存，方便下次使用
   	}

    	return 0;
}
