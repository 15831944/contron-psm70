#include "stdafx.h"
#include "CommunicationPeerLink.h"
#include "../platformLayer/WLPlatformLayer.h"

/****************************************************************
���ܣ�	���캯������ʼ������
������	��
���أ�	��
���ߣ�	Ҷ����
****************************************************************/
CCommunicationPeerLink::CCommunicationPeerLink(DWORD maxCatheCount)
{
	m_lastRecvTC = 0;                         //����յ�����ʱ���
	m_count = 0;                              //�����ݻ���buf����Ч���ݳ���
	m_precvbuf = NULL;                        //�����ݻ��棨����ʮ������ģ�
	m_pExt = NULL;                            //��չָ�루Ϊ�˽ṹ����չ������Ա������ָ����ʽ��

	//���뻺��ռ�
	m_maxCatheCount = maxCatheCount;
	m_precvbuf = new BYTE[m_maxCatheCount];

	m_isActive = TRUE;						  //����·�����Ƿ��Ծ
}

/****************************************************************
���ܣ�	����������������
������	��
���أ�	��
���ߣ�	Ҷ����
****************************************************************/
CCommunicationPeerLink::~CCommunicationPeerLink(void)
{
	delete [] m_precvbuf;
	m_precvbuf = NULL;
}

/****************************************************************
���ܣ�	�������ݵ������ݻ���
������	pbuf��		  ���Ƶ�Դ����
		len��		  Դ���ݵĳ���
		maxInterval�� �����ʱ��
���أ�  �����ݻ��������ݵ�����Ч����
���ߣ�	Ҷ����
****************************************************************/
int CCommunicationPeerLink::addBufToRecvCache(void * pbuf,DWORD len,DWORD maxInterval)
{
	//�ж��¾����ݰ��Ƿ����Ϊͬһ�����ģ��Ƿ���ƴ�����������ʱ���жϣ�
	//DWORD maxInterval = 1000;
	DWORD64 tc = WLGetTickCount64();
	if (0xFFFFFFFFU - m_lastRecvTC < maxInterval)                                    //�����������ڿ���������
	{
		if (0xFFFFFFFFU - m_lastRecvTC + tc > maxInterval)	  m_count =0;            //�����հ����̫�󣬲�������ͬһ�����������ǰ����
		m_lastRecvTC = tc;
	}
	else 
	{
		if (tc - m_lastRecvTC >maxInterval)	m_count =0;                              //�����հ����̫�󣬲�������ͬһ�����������ǰ����
		m_lastRecvTC = tc;
	}
	DWORD len1 = (len + m_count <= m_maxCatheCount) ? len : m_maxCatheCount - m_count;
	memcpy(m_precvbuf + m_count,pbuf,len1);
	m_count += len1;

	return m_count;
}

/****************************************************************
���ܣ�	�������ݻ��渴������
������	ppbuf��  �������ݵ�Ŀ�ĵ�ַ
		ppLen��	�������ݵĳ��ȵ�ָ��
���أ�  �����ݻ��������ݵ�����Ч����
���ߣ�	Ҷ����
****************************************************************/
int CCommunicationPeerLink::getCache(void ** ppbuf,DWORD ** ppLen)
{
	*ppbuf = m_precvbuf;
	*ppLen  = &m_count;

	return m_count;
}

/****************************************************************
���ܣ���������յ�����ʱ���
������tc������յ�����ʱ���
���أ���
���ߣ���Զ��  2014-06-25
****************************************************************/
void CCommunicationPeerLink::setLastRecvTC( DWORD64 tc )
{
	m_lastRecvTC = tc;
}

/****************************************************************
���ܣ���ȡ����յ�����ʱ���
��������
���أ�����յ�����ʱ���
���ߣ���Զ��  2014-06-25
****************************************************************/
DWORD64 CCommunicationPeerLink::getLastRecvTC()
{
	return m_lastRecvTC;
}

/****************************************************************
���ܣ���ȡ��·�Ļ�Ծ״̬
��������
���أ���·�Ļ�Ծ״̬
���ߣ���Զ��  2014-06-25
****************************************************************/
BOOL CCommunicationPeerLink::getIsActive()
{
	return m_isActive;
}

/****************************************************************
���ܣ�������·�Ļ�Ծ״̬
������isActive����·�Ļ�Ծ״̬
���أ���
���ߣ���Զ��  2014-06-25
****************************************************************/
void CCommunicationPeerLink::setIsActive( BOOL isActive )
{
	m_isActive = isActive;
}

/****************************************************************
���ܣ�������·����Ϊ�ǻ�Ծ״̬ʱ��ʱ���
������tc����·����Ϊ�ǻ�Ծ״̬ʱ��ʱ���
���أ���
���ߣ���Զ��  2014-06-25
****************************************************************/
void CCommunicationPeerLink::setIsNotActiveTick( DWORD64 tc )
{
	m_isNotActiveTC = tc;
}

/****************************************************************
���ܣ���ȡ��·����Ϊ�ǻ�Ծ״̬ʱ��ʱ���
��������
���أ���·����Ϊ�ǻ�Ծ״̬ʱ��ʱ���
���ߣ���Զ��  2014-06-25
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
////�������ݻ��棬����һ���ӻ���ͷ��ʼ��
//int CCommunicationPeerLink::discardCache(int count)
//{
//	if (count <=0)	return -1;
//	if (count >= m_count) m_count = 0;  //ȫ����
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