// BusinessDataSet.h: interface for the CFixedLenRecordFile class.
// ��װ������¼���ļ����ݼ��������ƶ�����¼���ļ��洢����ʽ���ļ�ͷ+�ļ���
// �����Ƕ�����¼���ļ����ݼ���Ļ��࣬�������ݼ��ɸ�����Ҫ�̳б���
// ������ڶ��߳��ǰ�ȫ��
// Ҷ����	2012-08-22
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUSINESSDATASET_H__7845BE8E_8BD4_417F_8016_F11EE5008838__INCLUDED_)
#define AFX_BUSINESSDATASET_H__7845BE8E_8BD4_417F_8016_F11EE5008838__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include "../platformLayer/WLCriticalSection.h"
#include "../platformLayer/WLPlatformLayer.h"

//��ҵ����������ݵ��ļ�ͷ����ҵ��������ļ��Զ����������ļ���ʽ�洢����ʽ���ļ�ͷ+�ļ���
typedef struct _FileHeaderFixedLenRecord{
	DWORD   version;                    //�汾��
	DWORD   recoredCount;               //��¼����
	DWORD   perRecLen;                  //������¼����
	DWORD   createrID;                  //�����߱�ʶ
	TWLTime fileCreatTime;              //�ļ���������
	DWORD   updaterID;                  //�޸��߱�ʶ
	TWLTime fileUpdateTime;             //����޸�����	
	BYTE    rfu[16];                    //Ԥ���ֽ�
	BYTE    customFields[48];           //�Զ��������洢���ݽṹ�����յ��ļ����
}TFileHeaderFixedLenRecord;


class CFixedLenRecordFile  
{
public:
	CFixedLenRecordFile(DWORD editerID = 0);
	virtual ~CFixedLenRecordFile();

	virtual int load(const char * filename);
	int saveas(const char * filename);
	int reload();
	int save();
	int getRecordCount();
	void clear();
	virtual void dump();

    void setSaveFileName(const char * fileName){saveFileName(fileName);}

protected:
	virtual void dumpHeader();
	virtual void initEmpFileHeader();
	virtual int setReservedRecCountForBody(int count);
	int readFileHeader(TFileHeaderFixedLenRecord * pHeader,FILE * f);
	void saveFileName(const char * fileName);
	virtual int getMoreMemory();
	virtual int resetRecList();
    virtual void initPerRecLen();

protected:
	char m_filename[MAX_PATH];        				//�洢�ļ�����
	CWLCriticalSection * m_pCritical; 				//�ٽ���

    TFileHeaderFixedLenRecord  m_header;     		//�ļ�ͷ
	BYTE               * m_pBody;      				//�ļ���
	DWORD                m_maxRecCount;				//����¼������������������������ʱ����ֵ��
	DWORD                m_editerID;   				//���ļ��Ĳ�����

	DWORD                m_reservedRecCountForBody;	//load�ɹ���ӦԤ����¼���������ռ䣩����������������ʱ����ֵ��
	DWORD                m_recCountForBodyMem;		//�����������ڴ���(��������ֱ���ڴ��ֽ���)
    DWORD                m_perRecLen;               //ÿ����¼�ĳ���,�����ļ��Ϸ��Լ��
};

#endif // !defined(AFX_BUSINESSDATASET_H__7845BE8E_8BD4_417F_8016_F11EE5008838__INCLUDED_)
