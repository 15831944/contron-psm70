#ifndef DSRECORD_H
#define DSRECORD_H

#include "platformLayer/WLPlatformLayer.h"
#include "inc/P7DataType.h"

#define		BLOCK_RECORD_SIZE			1024				// 块大小
#define		INIT_RECORD_BLOCK_CNT		1					// 初始化时分配多少块


typedef struct _FileHeaderBusiness 
{
	DWORD32  recoredCount;            //记录数量
	DWORD32  perRecLen;               //单条记录长度
	BYTE fileCreatTime[8];            //文件创建日期
	BYTE fileUpdateTime[8];           //最后修改日期	
}TFileHeaderBusiness;

typedef struct _Record
{
	DWORD32 stationID;		// 站ID
	BYTE macNum[16];		// 设备主编号
	DWORD32 pointID;		// 点号
	DWORD32 innerID;		// 内部ID
	DWORD32 macID;			// 设备ID
	DWORD32 type;			// 类型
	DWORD32 vtype;			// 1表示遥信,2表示遥测
	DWORD32 vindex;			// 0表示唯一操作权, 1表示状态
	DWORD32 value;
}TRecord;

class CWLCriticalSection;

class CDSRecord
{
public:
	CDSRecord();
	virtual ~CDSRecord();

	int load(const char *szFile);
	int add(TRecord *pRecord);
    int clear();
    int save(const char *szFile);
    int count() const { return m_count; }

    const TRecord * findByInnerID(DWORD32 innerID);								//	通过innerID查找
    const TRecord * findByIndex(UINT index);									//	通过索引查找
    const TRecord * findByMacNum(const char *num, ERTDBValYXIndex vindex=RTDB_ValYX_State);	//	通过设备主编号查找

protected:
	void initEmpFileHeader();
	int getMoreMemory();

private:
    DWORD32 m_count, m_sumCount;                                                // m_count 当前存储数量,m_sumCount 分配的总存储数量
	TFileHeaderBusiness m_header;
	TRecord *m_pRecordList;
    CWLCriticalSection * m_pCritical; 											//	临界区
};

#endif	// DSRECORD_H