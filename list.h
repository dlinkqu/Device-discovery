#ifndef _AUTH_CLIENT_QUEUE_H_
#define _AUTH_CLIENT_QUEUE_H_


#define MAX_HEARTBEAT_LOSS_CNT     3

#define IP_STR_LEN     15
#define MAC_STR_LEN    17
#define UUID_STR_LEN   36

/*--------------------------------------------------------------------------
          **���ܣ�Ӧ��C����ʵ�ֵ�����ĸ������
          **
          **
          **                  1�������ڵ�
          **                  2����ӡ������    
          **                  3��β��
          **                  4��βɾ
          **                  5��ͷ��       
          **                  6��ͷɾ    
          **                  7�������������   
          **                  8����ȡ������    
          **                  9����������    
          **                10����ĳλ�ú��������    
          **                11��ɾ��ĳλ�õ�����
          **                12��ɾ��һ����ͷ������ķ�β�ڵ�
          **                13������ͷ�������һ����ͷ�ڵ�ǰ����һ���ڵ�
          **                14�������м�ڵ�
          **                15�����ҵ�����k���ڵ�(Ҫ��ֻ�ܱ���һ��)
          **                16�����Ŵ�ӡ������
          **                17�����õ�����
          **                18���ϲ��������������ϲ�����Ȼ����
          **                19��ð������
          **  
          **
          **                                                        By ��Lynn-Zhang
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
    devNode dev;    /* ����   */
    struct SListNode* _next;   /*    ָ����һ���ڵ��ָ��    */
}SListNode;

/* ���Ҫ�޸�����ͱ��������   */
void    InitList(SListNode **list);
SListNode* BuySListNode(devNode dev);    /*�����ڵ� */
void SListPrint(SListNode* pHead);     /*��ӡ������ */
void SListPushBack(SListNode** pHead, devNode dev); /*β��(�����������ã�ָ����list�ı���������ʱ���Σ����ô���ַ)(������.c�ļ��в�����)    */

void SListPopBack(SListNode** pHead);   /*βɾ  */
void SListPushFront(SListNode** pHead, devNode dev);    /*ͷ��  */
void SListPopFront(SListNode** pHead);    /*    ͷɾ    */
void SListDestory(SListNode** pHead);     /* �����������    */
SListNode* SListFindHBTimeoutDev(SListNode* pHead);   /*    ͷ��ĳ�ض��ڵ�  */
int SListlenth(SListNode* pHead);     /*   ��ȡ������    */
SListNode* SListFind(SListNode* pHead, char *uuid);    /*   ��������    */
int SListInsest(SListNode** ppHead, SListNode* pos, devNode dev);     /* ��ĳλ�ú�������� */
int SListErase(SListNode** pHead, SListNode* pos);        /*    ɾ��ĳλ�õ�����    */
void SListResetHBlose(SListNode** node);
void SListAddHBlose(SListNode* pHead);

#endif  /* _AUTH_CLIENT_QUEUE_H_ */
