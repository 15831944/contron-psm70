/**************************************************************************************************
文件名：BayMgt.h
功  能：间隔管理模块
说  明：提供新增间隔、删除间隔、修改间隔信息、查询所有间隔等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __BAYMGT__H
#define __BAYMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_BAY_COUNT 200							//最大间隔数量

class CBayMgt : public CBusinessDataSet
{
public:
	CBayMgt();
	~CBayMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByStationID(const DWORD32 ID);
	int add(TBay *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TBay *pInputParam);

	int getListCount() const;
	TBay * getListByIndex(DWORD32 index) const;

private:
	TBay *m_pList;					//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TBay *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TBay *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif