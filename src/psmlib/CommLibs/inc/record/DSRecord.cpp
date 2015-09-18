//#include "stdafx.h"
#include "DSRecord.h"
#include "platformLayer/WLCriticalSection.h"

#include <stdio.h>
#include <string.h>

/***********************************************************
功能:	构造函数
作者:	佘佳明 2014-6-16
***********************************************************/
CDSRecord::CDSRecord()
{
	m_count = 0;
	m_sumCount = BLOCK_RECORD_SIZE*INIT_RECORD_BLOCK_CNT;
	m_pRecordList = new TRecord[m_sumCount];
	if (m_pRecordList == NULL)
	{
		printf("CDSRecord::CDSRecord()-->初始申请内存失败!");
	}
	initEmpFileHeader();

    m_pCritical = new CWLCriticalSection;
}

/************************************************************
功能:	从文件中导入记录数据
参数:	szFile 数据文件
返回:	<0 导入失败
		>=0 导入成功
作者:	佘佳明 2014-6-16
************************************************************/
int CDSRecord::load(const char *szFile)
{
	if (szFile == NULL)
	{
		printf("CDSRecord::load()-->传入参数不合法:szFile地址=%d\n", szFile);
		return -1;
	}
	FILE *f = fopen(szFile, "rb");
	if (f == NULL)	
	{
		printf("CDSRecord::load()-->打开文件失败:szFile=%s\n", szFile);
		return -2;
	}
    m_pCritical->enter();
	//读文件头
	TFileHeaderBusiness fileHeader;
	int len = fread(&fileHeader, 1, sizeof(TFileHeaderBusiness), f);
	if (len != sizeof(TFileHeaderBusiness))
	{
		printf("CDSRecord::load()-->读取文件头失败\n");
		fclose(f);
        m_pCritical->leave();
		return -3;
	}
	while (fileHeader.recoredCount >= m_sumCount)
	{
		int ret = getMoreMemory();
		if (ret < 0)
		{
			printf("CDSRecord::load()-->申请内容失败\n");
			fclose(f);
			m_pCritical->leave();
			return -4;
		}
	}
	DWORD32 readLen = fileHeader.perRecLen * fileHeader.recoredCount;
	len = fread(m_pRecordList, 1, readLen, f);
	if (len != readLen)
	{
		printf("CDSRecord::load()-->读取文件体失败\n");
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
功能:	添加站
参数:	pRecord 站对象指针
返回:	<0 添加失败
		>=0 添加成功
作者:	佘佳明 2014-6-16
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
功能:	清空数据
参数:	无
返回:	<0 清空失败
        >=0 清空成功
作者:	佘佳明 2014-6-16
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
功能：通过innerID获取记录
参数：innerID
返回：NULL 找不到
      非NULL 找到
作者：佘佳明	2014-6-17
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
功能：通过索引查找
参数：index 索引
返回：NULL 找不到
      非NULL 找到
作者：佘佳明	2014-6-17
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
功能：通过设备主编号查找
参数：num 设备主编号
        vindex
返回：NULL 找不到
      非NULL 找到
作者：佘佳明	2014-6-17
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
功能:	将数据保存到文件
参数:	szFile 数据文件
返回:	<0 保存失败
		>=0 保存成功
作者:	佘佳明 2014-6-16
************************************************************/
int CDSRecord::save(const char *szFile)
{
	if (szFile == NULL)
	{
		return -1;
	}
	//打开文件
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

	//关闭文件
	fclose(f);

    m_pCritical->leave();
	return 0;
}

/************************************************************
功能:	初始(清空)文件头内容
参数:	无
返回:	无
作者:	佘佳明 2014-6-16
************************************************************/
void CDSRecord::initEmpFileHeader()
{
	memset(&m_header, 0, sizeof(TFileHeaderBusiness));
	m_header.perRecLen = sizeof(TRecord);
}

/************************************************************
功能:	获取更多内存(每次获取一个数据块内存)
参数:	无
返回:	<0 获取失败
		>=0	获取成功
作者:	佘佳明 2014-6-16
************************************************************/
int CDSRecord::getMoreMemory()
{
	int sumCount = m_sumCount + BLOCK_RECORD_SIZE;
	TRecord *pRecordList = new TRecord[sumCount];
	if (pRecordList == NULL)
	{
		printf("CDSRecord::CDSRecord()-->申请内存失败!");
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
功能:	析构函数
作者:	佘佳明 2014-6-16
***********************************************************/
CDSRecord::~CDSRecord()
{
	delete [] m_pRecordList;
	delete m_pCritical;
}
