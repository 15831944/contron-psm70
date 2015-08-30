#ifndef DSRECORD_H
#define DSRECORD_H

#include "platformLayer/WLPlatformLayer.h"
#include "inc/P7DataType.h"

#define		BLOCK_RECORD_SIZE			1024				// ���С
#define		INIT_RECORD_BLOCK_CNT		1					// ��ʼ��ʱ������ٿ�


typedef struct _FileHeaderBusiness 
{
	DWORD32  recoredCount;            //��¼����
	DWORD32  perRecLen;               //������¼����
	BYTE fileCreatTime[8];            //�ļ���������
	BYTE fileUpdateTime[8];           //����޸�����	
}TFileHeaderBusiness;

typedef struct _Record
{
	DWORD32 stationID;		// վID
	BYTE macNum[16];		// �豸�����
	DWORD32 pointID;		// ���
	DWORD32 innerID;		// �ڲ�ID
	DWORD32 macID;			// �豸ID
	DWORD32 type;			// ����
	DWORD32 vtype;			// 1��ʾң��,2��ʾң��
	DWORD32 vindex;			// 0��ʾΨһ����Ȩ, 1��ʾ״̬
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

    const TRecord * findByInnerID(DWORD32 innerID);								//	ͨ��innerID����
    const TRecord * findByIndex(UINT index);									//	ͨ����������
    const TRecord * findByMacNum(const char *num, ERTDBValYXIndex vindex=RTDB_ValYX_State);	//	ͨ���豸����Ų���

protected:
	void initEmpFileHeader();
	int getMoreMemory();

private:
    DWORD32 m_count, m_sumCount;                                                // m_count ��ǰ�洢����,m_sumCount ������ܴ洢����
	TFileHeaderBusiness m_header;
	TRecord *m_pRecordList;
    CWLCriticalSection * m_pCritical; 											//	�ٽ���
};

#endif	// DSRECORD_H