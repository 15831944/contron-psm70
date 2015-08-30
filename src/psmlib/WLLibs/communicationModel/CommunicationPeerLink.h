/****************************************************************
说明：	本类为通道类，负责数据收发,作为其他连接类的基类
作者：	叶文林

修改：刘远安
说明：由于多个线程使用到本类，会出现一个多线程安全问题：
1.远程UDP客户端发送数据到UDP服务端
2.UDP服务端记录该远程客户端链路对象
3.UDP服务端收到数据后准备给客户端回复（回复数据需要远程客户端链路对象）
4.操作系统中断了UDP服务端回复数据的过程，此时客户端监控服务线程将该远程客户端链路对象删除了
5.操作系统将执行权交回给UDP服务端发送数据线程
6.程序崩溃
为了解决以上问题，解决思路如下：
1.客户端监控线程检测到客户端链路对象超时
2.将该客户端链路对象的活跃状态置为FALSE，不删除该客户端链路对象
3.客户端监控线程检测到客户端链路对象的活跃状态为FALSE，如果非活跃状态保持了一定时间，则删除该客户端链路对象
****************************************************************/

#pragma once

#include "../platformLayer/WLDEF.h"
#include <stdlib.h>

//通信通道基类
class CCommunicationPeerLink
{
public:
	CCommunicationPeerLink(DWORD maxCatheCount);
	~CCommunicationPeerLink(void);

	virtual int send(void * buf,DWORD len) = 0;
	virtual int recv(void * buf,DWORD len) = 0;

	int addBufToRecvCache(void * pbuf,DWORD len,DWORD maxInterval);
	int getCache(void ** ppbuf,DWORD ** ppLen);
	void setLastRecvTC(DWORD64 tc);
	DWORD64 getLastRecvTC();
	BOOL getIsActive();
	void setIsActive(BOOL isActive);
	void setIsNotActiveTick(DWORD64 tc);
	DWORD64 getIsNotActiveTick();
	//int getCacheLen();
	//BYTE * getCache();
	//int discardCache(int count);
	//int cleanCache();
protected:
	DWORD64  m_lastRecvTC;                    //最后收到数据时间戳
	DWORD  m_count;                           //收数据缓存buf中有效数据长度
	BYTE * m_precvbuf;                        //收数据缓存（最多存十条最大报文）
	void * m_pExt;                            //扩展指针（为此结构体扩展其它成员，采用指针形式）
	DWORD  m_maxCatheCount;                   //缓存的最大长度
	BOOL   m_isActive;						  //该链路对象是否活跃
	DWORD64  m_isNotActiveTC;                 //被置为非活跃状态时的时间戳
};
