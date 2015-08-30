/**************************************************************************************************
文件名：StationMgt.h
功  能：站管理模块
说  明：提供新增站、删除站、修改站信息、查询所有站等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __STATIONMGT__H
#define __STATIONMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_STATION_COUNT 200						//最大站数量

class CStationMgt : public CBusinessDataSet
{
public:
	CStationMgt();
	~CStationMgt();

	int retrieve();
	int add(TStaion *TStaion);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TStaion *pStation);

	int getListCount() const;
	TStaion * getListByIndex(DWORD32 index) const;

private:
	TStaion *m_pList;				//站列表
	DWORD32 m_count;				//站列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TStaion *TStaion);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TStaion *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif