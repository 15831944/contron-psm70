/****************************************************************
˵����	����ΪЭ���࣬��������ʹ����յ��ı���,��Ϊ����Э����Ļ���
���ߣ�	Ҷ����
****************************************************************/

#pragma once

#include "CommunicationPeerLink.h"

//��Լ����
class CCommunicationProtocol
{
public:
	CCommunicationProtocol(void);
	~CCommunicationProtocol(void);

	virtual int analyseProtocol(BYTE *pCache,DWORD * cachelen,BYTE * pCmd,DWORD * len) = 0;		//��pLink->precvbuf���У�����������
	virtual void procRecvCmd(BYTE *pCmd,DWORD len,CCommunicationPeerLink * pLink) = 0;			//�����յ��ı���
};
