/****************************************************************
说明：	本类为协议类，负责分析和处理收到的报文,作为其他协议类的基类
作者：	叶文林
****************************************************************/

#pragma once

#include "CommunicationPeerLink.h"

//规约基类
class CCommunicationProtocol
{
public:
	CCommunicationProtocol(void);
	~CCommunicationProtocol(void);

	virtual int analyseProtocol(BYTE *pCache,DWORD * cachelen,BYTE * pCmd,DWORD * len) = 0;		//从pLink->precvbuf包中，分析出命令
	virtual void procRecvCmd(BYTE *pCmd,DWORD len,CCommunicationPeerLink * pLink) = 0;			//处理收到的报文
};
