#include "stdafx.h"
#include "CommunicationServer.h"
#include <stdio.h>

/****************************************************************
功能：	构造函数，初始化工作
参数：	无
返回：	无
作者：	叶文林
****************************************************************/
CCommunicationServer::CCommunicationServer(CCommunicationProtocol * pProtocol)
{
	m_pProtocol = pProtocol;
	m_peerCount = 0;
}

/****************************************************************
功能：	析构函数，清理工作
参数：	无
返回：	无
作者：	叶文林
****************************************************************/
CCommunicationServer::~CCommunicationServer(void)
{
}

/*********************************************************************
功能：根据索引获取链路对象
参数：index，链路管理列表的索引
返回：非NULL，成功；NULL，失败
作者:刘远安 2014-06-25
*********************************************************************/
CCommunicationPeerLink * CCommunicationServer::getPeerLink( int index )
{
	if((index <0) || (index >= m_peerCount))	return NULL;

	//如果链路为非活跃状态
	if(FALSE == m_peerlinkList[index]->getIsActive())	return NULL;

	return m_peerlinkList[index];
}
