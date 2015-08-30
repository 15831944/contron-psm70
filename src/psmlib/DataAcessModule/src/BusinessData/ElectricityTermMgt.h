/**************************************************************************************************
文件名：ElectricityTermMgt.h
功  能：验电术语管理模块
说  明：提供新增验电术语、删除验电术语、修改验电术语、查询所有验电术语等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __ELECTRICIYTERMMGT__H
#define __ELECTRICIYTERMMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_ELECTRICITYTERM_COUNT 200							//最大验电术语数量

class CElectricityTermMgt : public CBusinessDataSet
{
public:
	CElectricityTermMgt();
	~CElectricityTermMgt();

	int retrieve();
    int retrieveByID(DWORD32 ID);
	int add(TElectricityTerm *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TElectricityTerm *pInputParam);

	int getListCount() const;
	TElectricityTerm * getListByIndex(DWORD32 index) const;

private:
	TElectricityTerm *m_pList;		//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TElectricityTerm *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TElectricityTerm *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
