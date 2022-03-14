/*************************************************************************
    > File Name: ipc_server_c.c
  > Author: hlx
  > Mail: 327209194@qq.com 
  > Created Time: 2019年12月02日 星期一 14时11分59秒
 ************************************************************************/
#if defined(WIN32)
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <Iphlpapi.h>
#elif defined(LINUX)
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h> 
#include <netinet/if_ether.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "list.h"

#define MULTICAST_GROUP_ADDR    "239.255.255.250"

#ifdef WIN32
#pragma comment (lib, "Ws2_32.lib")     //For winsock
#pragma comment (lib, "Iphlpapi.lib")

#endif

#define BUFLEN 4095

SListNode *devList;
SListNode *reqList;
void *devScan(void *arg);

#ifndef SOCKET
# ifdef WIN32
#  define SOCKET SOCKET
#  define CLOSESOCKET(n) closesocket(n)
#  define SNPRINTF       _snprintf
# else
#  define SOCKET int
#  define CLOSESOCKET(n) close(n)
#  define SNPRINTF       snprintf
# endif
#endif

#ifdef WIN32
#define SLEEP(n)    Sleep(1000 * (n))
#else
#define SLEEP(n)    sleep((n))
#endif

#define SOCKLEN_T unsigned int


#ifdef LINUX
int getClientMAC(const char *ip_str, char *mac) {
    SOCKET              sock;
    struct arpreq       areq;
    struct sockaddr_in *sin;
    struct in_addr      ipaddr;
    struct ifaddrs *ifa = NULL, *ifList;
    int                 findarp = 0;

    if(ip_str == NULL || mac == NULL)
        return -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		return -2;
	}
	
	/* Make the ARP request. 
	*/
	memset(&areq, 0, sizeof(areq));
	sin = (struct sockaddr_in *) &areq.arp_pa;
	sin->sin_family = AF_INET;

	if (inet_aton(ip_str, &ipaddr) == 0) {
		fprintf(stderr, "-- Error: bad dotted-decimal IP '%s'.\n", ip_str);
        close(sock);
		return -3;
	}
	
	sin->sin_addr = ipaddr;
	sin = (struct sockaddr_in *) &areq.arp_ha;
	sin->sin_family = ARPHRD_ETHER;


    if (getifaddrs(&ifList) < 0)
    {
        CLOSESOCKET(sock);
        return -5;
    }

    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next)
    {
        if(ifa->ifa_addr->sa_family == AF_INET)
        {            
            strncpy(areq.arp_dev, ifa->ifa_name, 15);
            if (ioctl(sock, SIOCGARP, (caddr_t) &areq) == 0) 
            {
                findarp = 1;
                break;
            }

        }
    }
    freeifaddrs(ifList);

    if(!findarp)
    {
        CLOSESOCKET(sock);
	    return -4;
    }


	
    unsigned char *ptr = (unsigned char *) areq.arp_ha.sa_data;

    
    SNPRINTF(mac, MAC_STR_LEN+1, "%02X:%02X:%02X:%02X:%02X:%02X", (ptr[0] & 0xff), (ptr[1] & 0xff), (ptr[2] & 0xff), (ptr[3] & 0xff), (ptr[4] & 0xff), (ptr[5] & 0xff));

    CLOSESOCKET(sock);
	return 0;
}

int add_multicast_group(int multicast_sockfd)
{
    unsigned char loop = 0;
    struct ip_mreq mreq; 

    /* 设置要加入组播的地址 */ 
    memset(&mreq, 0, sizeof (struct ip_mreq));
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP_ADDR);
    /* 设置发送组播消息的源主机的地址信息 */ 
    //mreq.imr_interface.s_addr = inet_addr ("172.18.12.114"); 
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); 
    /* 把本机加入组播地址，即本机网卡作为组播成员，只有加入组才能收到组播消息 */ 
    if (setsockopt(multicast_sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP , (char*)&mreq,sizeof (struct ip_mreq)) == -1)
    {
        perror ("setsockopt:");
        return -1;   
    }
    loop = 0;
    if (setsockopt(multicast_sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop,sizeof (loop)) == -1)
    {
        printf("IP_MULTICAST_LOOP set fail!\n");
    }

    return 0;
}
#endif

#ifdef WIN32
int getClientMAC(const char *ip_str, unsigned char *mac)
{
    WSADATA firstsock;
	IPAddr srcip;
    IPAddr destip;
    
	DWORD ret;
	ULONG MacAddr[2];
	ULONG PhyAddrLen = 6;  /* default to length of six bytes */
	int i;

    if(ip_str == NULL || mac == NULL)
        return -1;
	
	if (WSAStartup(MAKEWORD(2,2),&firstsock) != 0) 
	{
		printf("\nFailed to initialise winsock.");
		printf("\nError Code : %d",WSAGetLastError());
		return 1;	//Return 1 on error
	}
		
	//Ask user to select the device he wants to use
	destip = inet_addr(ip_str);
    srcip = 0;

	//Send an arp packet
	ret = SendARP(destip , srcip , MacAddr , &PhyAddrLen);
	
	//Prepare the mac address
	if(PhyAddrLen)
	{
		BYTE *bMacAddr = (BYTE *) & MacAddr;
		//for (i = 0; i < (int) PhyAddrLen; i++)
		{
            
            SNPRINTF(mac, MAC_STR_LEN+1, "%02X:%02X:%02X:%02X:%02X:%02X", (bMacAddr[0] & 0xff), (bMacAddr[1] & 0xff), (bMacAddr[2] & 0xff), (bMacAddr[3] & 0xff), (bMacAddr[4] & 0xff), (bMacAddr[5] & 0xff));
			//mac[i] = (char)bMacAddr[i] & 0xff;
		}
	}
    return 0;
}

int add_multicast_group(int multicast_sockfd)
{
    unsigned char loop = 0;
    struct ip_mreq mreq; 
    
    /* 设置要加入组播的地址 */ 
    int  i;
    //PIP_ADAPTER_INFO结构体指针存储本机网卡信息
    IP_ADAPTER_INFO * pIPAdapterInfo = (PIP_ADAPTER_INFO)malloc(sizeof(IP_ADAPTER_INFO));
    //得到结构体大小,用于GetAdaptersInfo参数
    unsigned long stSize = sizeof(IP_ADAPTER_INFO);
    //调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
    int nRel = GetAdaptersInfo(pIPAdapterInfo,&stSize);
    //记录网卡数量
    int netCardNum = 0;
    //记录每张网卡上的IP地址数量
    int IPnumPerNetCard = 0;
    if (ERROR_BUFFER_OVERFLOW == nRel)
    {
        //如果函数返回的是ERROR_BUFFER_OVERFLOW
        //则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
        //释放原来的内存空间
        free(pIPAdapterInfo);
        //重新申请内存空间用来存储所有网卡信息
         pIPAdapterInfo = (PIP_ADAPTER_INFO)malloc(stSize);
        //再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
        nRel=GetAdaptersInfo(pIPAdapterInfo,&stSize);    
    }
    if (ERROR_SUCCESS == nRel)
    {
        while (pIPAdapterInfo)
        {
            
            switch(pIPAdapterInfo->Type)
            {
                case MIB_IF_TYPE_ETHERNET:
                {
                    //可能网卡有多IP,因此通过循环去判断
                    IP_ADDR_STRING *pIpAddrString =&(pIPAdapterInfo->IpAddressList);
                    do 
                    {                        
                        memset(&mreq, 0, sizeof (struct ip_mreq));
                        mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP_ADDR);
                        /* 设置发送组播消息的源主机的地址信息 */ 
                        //mreq.imr_interface.s_addr = inet_addr ("172.18.12.114"); 
                        mreq.imr_interface.s_addr = inet_addr(pIpAddrString->IpAddress.String); 
                        /* 把本机加入组播地址，即本机网卡作为组播成员，只有加入组才能收到组播消息 */ 
                        if (setsockopt(multicast_sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP , (char*)&mreq,sizeof (struct ip_mreq)) == -1)
                        {
                            pIpAddrString=pIpAddrString->Next;
                            continue;
                        }
                        
                        if (setsockopt(multicast_sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop,sizeof (loop)) == -1)
                        {
                            printf("IP_MULTICAST_LOOP set fail!\n");
                        }
                        pIpAddrString=pIpAddrString->Next;
                    } while (pIpAddrString);
                    
                    
                }
                break;
                    
                default:
                    break;
            }
            pIPAdapterInfo = pIPAdapterInfo->Next;
        }
    
    }
    //释放内存空间
    if (pIPAdapterInfo)
    {
        free(pIPAdapterInfo);
    }
 
    return 0;
}

#endif

static void lanuchDevListScanThread()
{
    pthread_t Tid;
    pthread_attr_t Attr;

    
    pthread_attr_init(&Attr);
    pthread_attr_setdetachstate(&Attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&Tid, &Attr, (void *)devScan, (void *)NULL);
    pthread_attr_destroy(&Attr);
}

void *devScan(void *arg)
{
    SListNode *pNode;
    while(1)
    {
        if(SListlenth(devList) == 0)
        {
            SLEEP(1);
            continue;
        }
        
        SListAddHBlose(devList);
        pNode = SListFindHBTimeoutDev(devList);
        if(pNode) 
        {
            SListErase(&devList, pNode);
            if(SListlenth(devList) == 0)
            {
                continue;
            }
        }

        SListPrint(devList);
        SLEEP(1);
    }
}

int main (int argc, char **argv) 
{
#ifdef WIN32
    WSADATA wsaData;
#endif
    struct sockaddr_in groupcast_addr, the_member;
    SOCKET sockfd; 
    SOCKLEN_T socklen, n; 
    char recmsg[BUFLEN + 1];
    char *puuid = 0;
    devNode dev;
    int ret;

    /* Init dev list */
    InitList(&devList);

    /* Lanuch dev scan thread. */
    lanuchDevListScanThread();

#ifdef WIN32
    /* Initialize Winsock */
    ret = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (ret != 0) {
        printf("Could not open Windows connection.\n");
        return (-1);
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2 ) {
        printf("the version of WinSock DLL is not 2.2.\n");
        return -1;
    }
#endif

    /* 创建 socket 用于UDP通讯 */ 
    sockfd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    if (sockfd < 0)
    {          
        perror("socket:");
        exit (-1);        
    }

    /* */
    socklen = sizeof (struct sockaddr_in); 
    memset (&groupcast_addr, 0, socklen); 
    groupcast_addr.sin_family = AF_INET;
    groupcast_addr.sin_port = htons (13702);
    groupcast_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* 绑定自己的端口和IP信息到socket上 */ 
    if (bind(sockfd, (struct sockaddr *) &groupcast_addr, sizeof(struct sockaddr_in)) != 0)
    {      
        perror ("bind:");
        exit (-1);    
    }

    add_multicast_group(sockfd);

    while (1)
    {
        char mac_buf[MAC_STR_LEN+1]={0};
        SListNode*  pDev;
        
        memset (recmsg, 0, BUFLEN + 1);
        n = recvfrom (sockfd, recmsg, BUFLEN, 0, (struct sockaddr *) &the_member, &socklen);
        if (n < 0)
        {      
            printf ("recvfrom err in udptalk!\n");      
            exit (4);    
        }
        else{      
            recmsg[n] = 0;
#if 0            
            //printf ("recv:[%s]\n\n", recmsg);    
            printf("ip:%s\n",inet_ntoa(the_member.sin_addr));
            printf("port:%d\n", ntohs(the_member.sin_port));
#endif
        }

        /* Get client MAC */
        ret = getClientMAC(inet_ntoa(the_member.sin_addr), mac_buf);
        if(ret) 
        {
            printf("Get Client MAC fail ret=%d\n", ret);
            continue;
        }
        
        puuid = strstr(recmsg, "uuid:"); //获取recmsg字符串中 子字符串"uuid:"的位置
        if (puuid == 0)
        {
            printf("uuid: err!\n");
            return 0;
        }
        puuid += strlen("uuid:");   //获取接收的uuid的值

        memset(&dev, 0, sizeof(dev));
        
        strncpy(dev.uuid, puuid, sizeof(dev.uuid));
        strncpy(dev.mac, mac_buf, sizeof(dev.mac));
        strncpy(dev.ip, inet_ntoa(the_member.sin_addr), sizeof(dev.ip));

        //printf("uuid:%.*s\nip:%.*s\nmac:%.*s\n", UUID_STR_LEN, dev.uuid, IP_STR_LEN, dev.ip, MAC_STR_LEN, dev.mac);
        
        pDev = SListFind(devList, dev.uuid);
        if(pDev)
        {
            //printf("This dev has been add dev list.\n");
            SListResetHBlose(&pDev);
        }
        else
        {
            SListPushBack(&devList, dev);
        }
		
    }
    CLOSESOCKET(sockfd);
#ifdef WIN32
    WSACleanup();
#endif
}


