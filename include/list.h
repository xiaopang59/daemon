#ifndef LIST_H
#define LIST_H

struct list {   
    	char name[256]; //文件名
    	int status; //删除状态位
    	struct stat st;     //文件信息
    	struct list *next;
};

void CreateList(struct list **list);  //创建
void AddList(struct list *list, char *name, struct stat *st); //添加
void ShowList(struct list *list); //遍历打印
void DeleteNode(struct list *list, char *name, struct stat *st); //删除元素
void FreeList(struct list *list) ; //释放链表
void ListSort(struct list *list);  //链表排序
void sys_err(const char *str); 
void LoadDir(char *dirname);     //加载目录
void ReadFile(char *dirname);     //遍历目录
void ContrastList(struct list *list1, struct list *list2);    //对比两个链表
int  SearchNode(struct list *list, const struct list *node);    //查找链表中时候存在某个元素
void CheckDeleteNode(struct list *list);  //删除文件不存在的元素
void WriteLog(struct list *ls, char *buff);    //写日志

#endif
