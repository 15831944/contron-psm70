/****************************************************************
˵����	����Ϊͨ���࣬���������շ�,��Ϊ����������Ļ���
���ߣ�	Ҷ����

�޸ģ���Զ��
˵�������ڶ���߳�ʹ�õ����࣬�����һ�����̰߳�ȫ���⣺
1.Զ��UDP�ͻ��˷������ݵ�UDP�����
2.UDP����˼�¼��Զ�̿ͻ�����·����
3.UDP������յ����ݺ�׼�����ͻ��˻ظ����ظ�������ҪԶ�̿ͻ�����·����
4.����ϵͳ�ж���UDP����˻ظ����ݵĹ��̣���ʱ�ͻ��˼�ط����߳̽���Զ�̿ͻ�����·����ɾ����
5.����ϵͳ��ִ��Ȩ���ظ�UDP����˷��������߳�
6.�������
Ϊ�˽���������⣬���˼·���£�
1.�ͻ��˼���̼߳�⵽�ͻ�����·����ʱ
2.���ÿͻ�����·����Ļ�Ծ״̬��ΪFALSE����ɾ���ÿͻ�����·����
3.�ͻ��˼���̼߳�⵽�ͻ�����·����Ļ�Ծ״̬ΪFALSE������ǻ�Ծ״̬������һ��ʱ�䣬��ɾ���ÿͻ�����·����
****************************************************************/

#pragma once

#include "../platformLayer/WLDEF.h"
#include <stdlib.h>

//ͨ��ͨ������
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
	DWORD64  m_lastRecvTC;                    //����յ�����ʱ���
	DWORD  m_count;                           //�����ݻ���buf����Ч���ݳ���
	BYTE * m_precvbuf;                        //�����ݻ��棨����ʮ������ģ�
	void * m_pExt;                            //��չָ�루Ϊ�˽ṹ����չ������Ա������ָ����ʽ��
	DWORD  m_maxCatheCount;                   //�������󳤶�
	BOOL   m_isActive;						  //����·�����Ƿ��Ծ
	DWORD64  m_isNotActiveTC;                 //����Ϊ�ǻ�Ծ״̬ʱ��ʱ���
};
