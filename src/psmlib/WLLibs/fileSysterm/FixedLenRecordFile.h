// BusinessDataSet.h: interface for the CFixedLenRecordFile class.
// 封装定长记录型文件数据集，二进制定长记录型文件存储，格式：文件头+文件体
// 本类是定长记录型文件数据集类的基类，具体数据集可根据需要继承本类
// 本类对于多线程是安全的
// 叶文林	2012-08-22
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUSINESSDATASET_H__7845BE8E_8BD4_417F_8016_F11EE5008838__INCLUDED_)
#define AFX_BUSINESSDATASET_H__7845BE8E_8BD4_417F_8016_F11EE5008838__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include "../platformLayer/WLCriticalSection.h"
#include "../platformLayer/WLPlatformLayer.h"

//本业务存数据数据的文件头，本业务的数据文件以定长二进制文件方式存储，格式：文件头+文件体
typedef struct _FileHeaderFixedLenRecord{
	DWORD   version;                    //版本号
	DWORD   recoredCount;               //记录数量
	DWORD   perRecLen;                  //单条记录长度
	DWORD   createrID;                  //创建者标识
	TWLTime fileCreatTime;              //文件创建日期
	DWORD   updaterID;                  //修改者标识
	TWLTime fileUpdateTime;             //最后修改日期	
	BYTE    rfu[16];                    //预留字节
	BYTE    customFields[48];           //自定义区，存储内容结构与最终的文件相关
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
	char m_filename[MAX_PATH];        				//存储文件名称
	CWLCriticalSection * m_pCritical; 				//临界区

    TFileHeaderFixedLenRecord  m_header;     		//文件头
	BYTE               * m_pBody;      				//文件体
	DWORD                m_maxRecCount;				//最大记录数（条数，子类根据情况构造时赋初值）
	DWORD                m_editerID;   				//本文件的操作者

	DWORD                m_reservedRecCountForBody;	//load成功后应预留记录数（多分配空间）（子类根据情况构造时赋初值）
	DWORD                m_recCountForBodyMem;		//数据区分配内存数(条数，非直接内存字节数)
    DWORD                m_perRecLen;               //每条记录的长度,用于文件合法性检查
};

#endif // !defined(AFX_BUSINESSDATASET_H__7845BE8E_8BD4_417F_8016_F11EE5008838__INCLUDED_)
