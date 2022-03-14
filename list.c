#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "list.h"

void    InitList(SListNode **list)
{
    *list = NULL;
}

SListNode* BuySListNode(devNode dev)
{
    SListNode* node = (SListNode*)malloc(sizeof(SListNode));
    if(!node) return NULL;

    memset(node, 0, sizeof(SListNode));
    strncpy((char *)(node->dev.ip), (char *)dev.ip, sizeof(node->dev.ip));
    strncpy((char *)(node->dev.mac), (char *)dev.mac, sizeof(node->dev.mac));
    strncpy((char *)(node->dev.uuid), (char *)dev.uuid, sizeof(node->dev.uuid));
    node->_next = NULL;
    return node;
}


void SListPrint(SListNode* pHead)   /* 打印单链表   */
{
    int i;
    SListNode* cur = pHead;
#ifdef WIN32
    system("cls");
#endif
#ifdef LINUX
    system("clear");
#endif
    for(i=0;i<76;i++)
        printf("*");
    printf("\n");
    printf("|UUID%-35s|IP%-13s|MAC%-15s|\n", " ", " ", " ");
    while (cur)
    {
        for(i=0;i<76;i++)
            printf("-");
        printf("\n");
        printf("|%.*s\t|%.*s\t|%.*s |\n", UUID_STR_LEN, cur->dev.uuid, IP_STR_LEN, cur->dev.ip, MAC_STR_LEN, cur->dev.mac);
        cur = cur->_next;
    } 
    for(i=0;i<76;i++)
        printf("*");
    printf("\n\n");
}

void SListPushBack(SListNode** ppHead, devNode dev)
{
    if (*ppHead == NULL)
    {
        *ppHead = BuySListNode(dev);
    }
    else
    {
        SListNode* cur = *ppHead;
        while (cur->_next)
        {
            cur = cur->_next;
        }
        cur->_next = BuySListNode(dev);
    }
}

void SListPopBack(SListNode** ppHead)
{
    if (*ppHead == NULL)
    {
        return;
    }
    else
    {
        SListNode* cur = *ppHead;
        if (cur->_next == NULL)
        {
            free(cur);
            *ppHead = NULL;
        }
        else
        {   
            SListNode* tmp = cur;
            while (cur->_next)
            {
                tmp = cur;
                cur = cur->_next;
            }
            tmp->_next = cur->_next;
            free(cur);
        }
    }
}

void SListPushFront(SListNode** ppHead, devNode dev)
{
    if (*ppHead == NULL)
    {
        *ppHead = BuySListNode(dev);
    }
    else
    {
        SListNode* pHead = BuySListNode(dev);
        SListNode* cur = *ppHead;
        pHead->_next = cur;
        *ppHead = pHead;
    }
}

void SListPopFront(SListNode** ppHead)
{
    if (*ppHead == NULL)
    {
        return;
    }
    else
    {
        SListNode* cur = *ppHead;
        SListNode* tmp = cur;
        cur = cur->_next;
        *ppHead = cur;
        free(tmp);
    }
}

SListNode* SListFind(SListNode* pHead, char *uuid)
{
    if (pHead == NULL)
    {
        return NULL;
    }
    else
    {
        SListNode* cur = NULL;
        cur = pHead;
        while (cur)
        {
            if (!strncmp(cur->dev.uuid, uuid, sizeof(cur->dev.uuid)))
            {
                return cur;
            }
            else
            {
                cur = cur->_next;
            }
        }
        return NULL;
    }
}

SListNode* SListFindHBTimeoutDev(SListNode* pHead)   /*    头找某特定节点  */
{
    /*
       1.空
       2.一个或以上的节点
    */
    
    SListNode* cur = pHead;
    if (pHead == NULL)
    {
        return NULL;
    }

    while (cur)
    {
        /* 查找未申请KEY的认证请求 */
        if (cur->dev.lose >= MAX_HEARTBEAT_LOSS_CNT)
        {
            return cur;
        }
        cur = cur->_next;
    }
    return NULL;
}

void SListResetHBlose(SListNode** node)   /*   头找某特定节点 */
{    
    SListNode* cur;
    if(!*node) return;

    cur = *node;

    cur->dev.lose = 0;
}

void SListAddHBlose(SListNode* pHead)   /*   头找某特定节点 */
{
    SListNode* cur = pHead;
    if (pHead == NULL)
    {
        return;
    }

    while (cur)
    {
        cur->dev.lose++;
        cur = cur->_next;
    }
}



void SListDestory(SListNode** ppHead)   /*    清空整个链表    */
{
    SListNode* cur = *ppHead;
    while (cur)
    {
        SListNode* tmp = cur;
        cur = cur->_next;
        free(tmp);
    }
 
    *ppHead = NULL;
}

int SListlenth(SListNode* list)
{
    SListNode* cur = list;
    int count = 0;
    
    if(!list) return 0;
    while (cur)
    {
        count++;
        cur = cur->_next;
    }
    return count;
}


int SListInsest(SListNode** ppHead, SListNode* pos, devNode dev)
{
    SListNode* newnode = BuySListNode(dev);
    if (*ppHead == pos)
    {
        newnode->_next = pos;
        *ppHead = newnode;
        return 1;
    }
    else
    {
        SListNode* cur = *ppHead;
        while (cur)
        {
            if (cur->_next == pos)
            {
                cur->_next = newnode;
                newnode->_next = pos;
                return 1;
            }
            else
            {
                cur = cur->_next;
            }
        }
    }
    return -1;
}


int SListErase(SListNode** ppHead, SListNode* pos)
{
    if(!*ppHead || !pos) return -1;

    if (*ppHead == pos)
    {
        SListNode* cur = *ppHead;
        *ppHead = (*ppHead)->_next;
        free(cur);
        return 1;
    }
    else
    {
        SListNode* cur = *ppHead;
        while (cur->_next)
        {
            if (cur->_next == pos)
            {
                cur->_next = pos->_next;
                free(pos);
                return 1;
            }
            else
            {
                cur = cur->_next;
            }
        }
    }
    return -1;
}

