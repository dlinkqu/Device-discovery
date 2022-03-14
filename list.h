#ifndef _AUTH_CLIENT_QUEUE_H_
#define _AUTH_CLIENT_QUEUE_H_


#define MAX_HEARTBEAT_LOSS_CNT     3

#define IP_STR_LEN     15
#define MAC_STR_LEN    17
#define UUID_STR_LEN   36

/*--------------------------------------------------------------------------
          **功能：应用C语言实现单链表的各项操作
          **
          **
          **                  1：建立节点
          **                  2：打印单链表    
          **                  3：尾插
          **                  4：尾删
          **                  5：头插       
          **                  6：头删    
          **                  7：清空整个链表   
          **                  8：获取链表长度    
          **                  9：查找数据    
          **                10：在某位置后插入数据    
          **                11：删除某位置的数据
          **                12：删除一个无头单链表的非尾节点
          **                13：在无头单链表的一个非头节点前插入一个节点
          **                14：查找中间节点
          **                15：查找倒数第k个节点(要求只能遍历一次)
          **                16：倒着打印单链表
          **                17：逆置单链表
          **                18：合并两个有序链表（合并后依然有序）
          **                19：冒泡排序
          **  
          **
          **                                                        By ：Lynn-Zhang
          **                                                         
---------------------------------------------------------------------------   */                         
typedef struct dev_node
{
    int  lose;
    char ip[IP_STR_LEN];
    char mac[MAC_STR_LEN];
    char uuid[UUID_STR_LEN];
} devNode;

typedef struct SListNode
{
    devNode dev;    /* 数据   */
    struct SListNode* _next;   /*    指向下一个节点的指针    */
}SListNode;

/* 如果要修改链表就必须加引用   */
void    InitList(SListNode **list);
SListNode* BuySListNode(devNode dev);    /*建立节点 */
void SListPrint(SListNode* pHead);     /*打印单链表 */
void SListPushBack(SListNode** pHead, devNode dev); /*尾插(这里用了引用，指明是list的别名，调用时传参，不用传地址)(引用在.c文件中不可用)    */

void SListPopBack(SListNode** pHead);   /*尾删  */
void SListPushFront(SListNode** pHead, devNode dev);    /*头插  */
void SListPopFront(SListNode** pHead);    /*    头删    */
void SListDestory(SListNode** pHead);     /* 清空整个链表    */
SListNode* SListFindHBTimeoutDev(SListNode* pHead);   /*    头找某特定节点  */
int SListlenth(SListNode* pHead);     /*   获取链表长度    */
SListNode* SListFind(SListNode* pHead, char *uuid);    /*   查找数据    */
int SListInsest(SListNode** ppHead, SListNode* pos, devNode dev);     /* 在某位置后插入数据 */
int SListErase(SListNode** pHead, SListNode* pos);        /*    删除某位置的数据    */
void SListResetHBlose(SListNode** node);
void SListAddHBlose(SListNode* pHead);

#endif  /* _AUTH_CLIENT_QUEUE_H_ */
