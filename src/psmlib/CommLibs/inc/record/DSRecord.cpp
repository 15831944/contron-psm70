//#include "stdafx.h"
#include "DSRecord.h"
#include "platformLayer/WLCriticalSection.h"

#include <stdio.h>
#include <string.h>

/***********************************************************
����:	���캯��
����:	�ܼ��� 2014-6-16
***********************************************************/
CDSRecord::CDSRecord()
{
	m_count = 0;
	m_sumCount = BLOCK_RECORD_SIZE*INIT_RECORD_BLOCK_CNT;
	m_pRecordList = new TRecord[m_sumCount];
	if (m_pRecordList == NULL)
	{
		printf("CDSRecord::CDSRecord()-->��ʼ�����ڴ�ʧ��!");
	}
	initEmpFileHeader();

    m_pCritical = new CWLCriticalSection;
}

/************************************************************
����:	���ļ��е����¼����
����:	szFile �����ļ�
����:	<0 ����ʧ��
		>=0 ����ɹ�
����:	�ܼ��� 2014-6-16
************************************************************/
int CDSRecord::load(const char *szFile)
{
	if (szFile == NULL)
	{
		printf("CDSRecord::load()-->����������Ϸ�:szFile��ַ=%d\n", szFile);
		return -1;
	}
	FILE *f = fopen(szFile, "rb");
	if (f == NULL)	
	{
		printf("CDSRecord::load()-->���ļ�ʧ��:szFile=%s\n", szFile);
		return -2;
	}
    m_pCritical->enter();
	//���ļ�ͷ
	TFileHeaderBusiness fileHeader;
	int len = fread(&fileHeader, 1, sizeof(TFileHeaderBusiness), f);
	if (len != sizeof(TFileHeaderBusiness))
	{
		printf("CDSRecord::load()-->��ȡ�ļ�ͷʧ��\n");
		fclose(f);
        m_pCritical->leave();
		return -3;
	}
	while (fileHeader.recoredCount >= m_sumCount)
	{
		int ret = getMoreMemory();
		if (ret < 0)
		{
			printf("CDSRecord::load()-->��������ʧ��\n");
			fclose(f);
			m_pCritical->leave();
			return -4;
		}
	}
	DWORD32 readLen = fileHeader.perRecLen * fileHeader.recoredCount;
	len = fread(m_pRecordList, 1, readLen, f);
	if (len != readLen)
	{
		printf("CDSRecord::load()-->��ȡ�ļ���ʧ��\n");
		fclose(f);
        m_pCritical->leave();
		return -5;
	}

	m_count = fileHeader.recoredCount;
	m_header.recoredCount = m_count;

	fclose(f);
	m_pCritical->leave();
	return m_count;
}

/************************************************************
����:	���վ
����:	pRecord վ����ָ��
����:	<0 ���ʧ��
		>=0 ��ӳɹ�
����:	�ܼ��� 2014-6-16
************************************************************/
int CDSRecord::add(TRecord *pRecord)
{
	if (pRecord == NULL)
	{
		return -1;
	}
    m_pCritical->enter();
	while (m_count >= m_sumCount)
	{
		int ret = getMoreMemory();
		if (ret < 0)
		{
            m_pCritical->leave();
			return -2;
		}
	}

	memcpy(&m_pRecordList[m_count], pRecord, sizeof(TRecord));
	m_count++;
	m_header.recoredCount = m_count;

    m_pCritical->leave();
	return m_count;
}

/************************************************************
����:	�������
����:	��
����:	<0 ���ʧ��
        >=0 ��ճɹ�
����:	�ܼ��� 2014-6-16
************************************************************/
int CDSRecord::clear()
{
    if (m_count <= 0)
    {
        return 0;
    }
    m_pCritical->enter();
    memset(&m_pRecordList[0], 0, sizeof(TRecord)*m_count);
    m_count = 0;
    m_header.recoredCount = 0;
    m_pCritical->leave();
    return 0;
}

/*********************************************************************
���ܣ�ͨ��innerID��ȡ��¼
������innerID
���أ�NULL �Ҳ���
      ��NULL �ҵ�
���ߣ��ܼ���	2014-6-17
*********************************************************************/
const TRecord * CDSRecord::findByInnerID(DWORD32 innerID)
{
    m_pCritical->enter();
    for(int i=0; i<m_count; i++)
    {
        if(m_pRecordList[i].innerID == innerID)
        {
            m_pCritical->leave();
            return &m_pRecordList[i];
        }
    }
    m_pCritical->leave();
    return NULL;
}


/*********************************************************************
���ܣ�ͨ����������
������index ����
���أ�NULL �Ҳ���
      ��NULL �ҵ�
���ߣ��ܼ���	2014-6-17
*********************************************************************/
const TRecord * CDSRecord::findByIndex(UINT index)
{
    if (index >= m_count)
    {
        return NULL;
    }
    return &m_pRecordList[index];
}

/*********************************************************************
���ܣ�ͨ���豸����Ų���
������num �豸�����
        vindex
���أ�NULL �Ҳ���
      ��NULL �ҵ�
���ߣ��ܼ���	2014-6-17
*********************************************************************/
const TRecord *CDSRecord::findByMacNum(const char *num, ERTDBValYXIndex vindex)
{
    if (num == NULL)
    {
        return NULL;
    }
    m_pCritical->enter();
    for(int i=0; i<m_count; i++)
    {
        if((strcmp((char *)m_pRecordList[i].macNum, num) == 0) &&
            m_pRecordList[i].vindex == vindex)
        {
            m_pCritical->leave();
            return &m_pRecordList[i];
        }
    }
    m_pCritical->leave();
    return NULL;
}

/************************************************************
����:	�����ݱ��浽�ļ�
����:	szFile �����ļ�
����:	<0 ����ʧ��
		>=0 ����ɹ�
����:	�ܼ��� 2014-6-16
************************************************************/
int CDSRecord::save(const char *szFile)
{
	if (szFile == NULL)
	{
		return -1;
	}
	//���ļ�
	FILE * f = fopen(szFile, "wb");
	if (f == NULL)
	{
		return -2;
	}
    m_pCritical->enter();
	int len = fwrite(&m_header, 1, sizeof(TFileHeaderBusiness), f);
	if (len != sizeof(TFileHeaderBusiness))
	{
		fclose(f);
        m_pCritical->leave();
		return -3;
	}
	len = fwrite(m_pRecordList, 1, m_count*sizeof(TRecord), f);
	if (len != m_count*sizeof(TRecord))
	{
		fclose(f);
        m_pCritical->leave();
		return -4;
	}

	//�ر��ļ�
	fclose(f);

    m_pCritical->leave();
	return 0;
}

/************************************************************
����:	��ʼ(���)�ļ�ͷ����
����:	��
����:	��
����:	�ܼ��� 2014-6-16
************************************************************/
void CDSRecord::initEmpFileHeader()
{
	memset(&m_header, 0, sizeof(TFileHeaderBusiness));
	m_header.perRecLen = sizeof(TRecord);
}

/************************************************************
����:	��ȡ�����ڴ�(ÿ�λ�ȡһ�����ݿ��ڴ�)
����:	��
����:	<0 ��ȡʧ��
		>=0	��ȡ�ɹ�
����:	�ܼ��� 2014-6-16
************************************************************/
int CDSRecord::getMoreMemory()
{
	int sumCount = m_sumCount + BLOCK_RECORD_SIZE;
	TRecord *pRecordList = new TRecord[sumCount];
	if (pRecordList == NULL)
	{
		printf("CDSRecord::CDSRecord()-->�����ڴ�ʧ��!");
		return -1;
	}

	m_sumCount = sumCount;
	memset(pRecordList, 0, sizeof(TRecord)*m_sumCount);
	memcpy(pRecordList, m_pRecordList, sizeof(TRecord)*m_count);
	delete [] m_pRecordList;
	m_pRecordList = pRecordList;

	return 0;
}

/***********************************************************
����:	��������
����:	�ܼ��� 2014-6-16
***********************************************************/
CDSRecord::~CDSRecord()
{
	delete [] m_pRecordList;
	delete m_pCritical;
}
