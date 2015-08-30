#include "stdafx.h"
#include "CommunicationServer.h"
#include <stdio.h>

/****************************************************************
���ܣ�	���캯������ʼ������
������	��
���أ�	��
���ߣ�	Ҷ����
****************************************************************/
CCommunicationServer::CCommunicationServer(CCommunicationProtocol * pProtocol)
{
	m_pProtocol = pProtocol;
	m_peerCount = 0;
}

/****************************************************************
���ܣ�	����������������
������	��
���أ�	��
���ߣ�	Ҷ����
****************************************************************/
CCommunicationServer::~CCommunicationServer(void)
{
}

/*********************************************************************
���ܣ�����������ȡ��·����
������index����·�����б������
���أ���NULL���ɹ���NULL��ʧ��
����:��Զ�� 2014-06-25
*********************************************************************/
CCommunicationPeerLink * CCommunicationServer::getPeerLink( int index )
{
	if((index <0) || (index >= m_peerCount))	return NULL;

	//�����·Ϊ�ǻ�Ծ״̬
	if(FALSE == m_peerlinkList[index]->getIsActive())	return NULL;

	return m_peerlinkList[index];
}
