#include "stdafx.h"
#include "CommunicationPeerLink.h"
#include "../platformLayer/WLPlatformLayer.h"

/****************************************************************
功能：	构造函数，初始化工作
参数：	无
返回：	无
作者：	叶文林
****************************************************************/
CCommunicationPeerLink::CCommunicationPeerLink(DWORD maxCatheCount)
{
	m_lastRecvTC = 0;                         //最后收到数据时间戳
	m_count = 0;                              //收数据缓存buf中有效数据长度
	m_precvbuf = NULL;                        //收数据缓存（最多存十条最大报文）
	m_pExt = NULL;                            //扩展指针（为此结构体扩展其它成员，采用指针形式）

	//申请缓存空间
	m_maxCatheCount = maxCatheCount;
	m_precvbuf = new BYTE[m_maxCatheCount];

	m_isActive = TRUE;						  //该链路对象是否活跃
}

/****************************************************************
功能：	析构函数，清理工作
参数：	无
返回：	无
作者：	叶文林
****************************************************************/
CCommunicationPeerLink::~CCommunicationPeerLink(void)
{
	delete [] m_precvbuf;
	m_precvbuf = NULL;
}

/****************************************************************
功能：	复制数据到收数据缓存
参数：	pbuf，		  复制的源数据
		len，		  源数据的长度
		maxInterval， 最大间隔时间
返回：  收数据缓存中数据的有有效长度
作者：	叶文林
****************************************************************/
int CCommunicationPeerLink::addBufToRecvCache(void * pbuf,DWORD len,DWORD maxInterval)
{
	//判断新旧数据包是否可能为同一条报文（是否是拼包情况，根据时间判断）
	//DWORD maxInterval = 1000;
	DWORD64 tc = WLGetTickCount64();
	if (0xFFFFFFFFU - m_lastRecvTC < maxInterval)                                    //当计数器处于快溢出情况下
	{
		if (0xFFFFFFFFU - m_lastRecvTC + tc > maxInterval)	  m_count =0;            //两次收包间隔太大，不可能是同一条命令，丢弃先前缓存
		m_lastRecvTC = tc;
	}
	else 
	{
		if (tc - m_lastRecvTC >maxInterval)	m_count =0;                              //两次收包间隔太大，不可能是同一条命令，丢弃先前缓存
		m_lastRecvTC = tc;
	}
	DWORD len1 = (len + m_count <= m_maxCatheCount) ? len : m_maxCatheCount - m_count;
	memcpy(m_precvbuf + m_count,pbuf,len1);
	m_count += len1;

	return m_count;
}

/****************************************************************
功能：	从收数据缓存复制数据
参数：	ppbuf，  复制数据的目的地址
		ppLen，	复制数据的长度的指针
返回：  收数据缓存中数据的有有效长度
作者：	叶文林
****************************************************************/
int CCommunicationPeerLink::getCache(void ** ppbuf,DWORD ** ppLen)
{
	*ppbuf = m_precvbuf;
	*ppLen  = &m_count;

	return m_count;
}

/****************************************************************
功能：设置最后收到数据时间戳
参数：tc，最后收到数据时间戳
返回：无
作者：刘远安  2014-06-25
****************************************************************/
void CCommunicationPeerLink::setLastRecvTC( DWORD64 tc )
{
	m_lastRecvTC = tc;
}

/****************************************************************
功能：获取最后收到数据时间戳
参数：无
返回：最后收到数据时间戳
作者：刘远安  2014-06-25
****************************************************************/
DWORD64 CCommunicationPeerLink::getLastRecvTC()
{
	return m_lastRecvTC;
}

/****************************************************************
功能：获取链路的活跃状态
参数：无
返回：链路的活跃状态
作者：刘远安  2014-06-25
****************************************************************/
BOOL CCommunicationPeerLink::getIsActive()
{
	return m_isActive;
}

/****************************************************************
功能：设置链路的活跃状态
参数：isActive，链路的活跃状态
返回：无
作者：刘远安  2014-06-25
****************************************************************/
void CCommunicationPeerLink::setIsActive( BOOL isActive )
{
	m_isActive = isActive;
}

/****************************************************************
功能：设置链路被置为非活跃状态时的时间戳
参数：tc，链路被置为非活跃状态时的时间戳
返回：无
作者：刘远安  2014-06-25
****************************************************************/
void CCommunicationPeerLink::setIsNotActiveTick( DWORD64 tc )
{
	m_isNotActiveTC = tc;
}

/****************************************************************
功能：获取链路被置为非活跃状态时的时间戳
参数：无
返回：链路被置为非活跃状态时的时间戳
作者：刘远安  2014-06-25
****************************************************************/
DWORD64 CCommunicationPeerLink::getIsNotActiveTick()
{
	return m_isNotActiveTC;
}

//int CCommunicationPeerLink::getCacheLen()
//{
//	return m_count;
//}
//
//BYTE * CCommunicationPeerLink::getCache()
//{
//	return m_precvbuf;
//}
//
////丢弃部份缓存，并且一定从缓存头开始丢
//int CCommunicationPeerLink::discardCache(int count)
//{
//	if (count <=0)	return -1;
//	if (count >= m_count) m_count = 0;  //全丢弃
//
//	int newCount = m_count - count;
//	memcpy(m_precvbuf ,m_precvbuf + count,newCount);
//	m_count = newCount;
//
//	return m_count;
//}
//
//int CCommunicationPeerLink::cleanCache()
//{
//	m_count = 0;
//	return m_count;
//}