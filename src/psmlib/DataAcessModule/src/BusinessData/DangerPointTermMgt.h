/**************************************************************************************************
文件名：DangerPointTermMgt.h
功  能：危险点术语管理模块
说  明：提供新增危险点术语、删除危险点术语、修改危险点术语信息、查询所有危险点术语等一系列功能
注  意：暂无
作 者： 李施施 2014-07-23
**************************************************************************************************/

#ifndef __DANGERPOINTTERMMGT__H
#define __DANGERPOINTTERMMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_DANGERPOINTTERM_COUNT 200							//最大术语数量

class CDangerPointTermMgt : public CBusinessDataSet
{
public:
	CDangerPointTermMgt();
	~CDangerPointTermMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByDangerPointDescriptionID(const DWORD32 ID);
	int add(TDangerPointTerm *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TDangerPointTerm *pInputParam);

	int getListCount() const;
	TDangerPointTerm * getListByIndex(DWORD32 index) const;

private:
	TDangerPointTerm *m_pList;		//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TDangerPointTerm *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TDangerPointTerm *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif