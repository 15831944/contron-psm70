/**************************************************************************************************
文件名：RedLineDeviceTermMgt.h
功  能：红线设备术语管理模块
说  明：提供新增红线设备术语、删除红线设备术语、修改红线设备术语、查询所有红线设备术语等一系列功能
注  意：暂无
作 者： 李施施 2014-07-22
**************************************************************************************************/

#ifndef __REDLINEDEVICETERMMGT__H
#define __REDLINEDEVICETERMMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_REDLINEDEVICETERM_COUNT 200							//最大红线设备术语数量


class CRedLineDeviceTermMgt : public CBusinessDataSet
{
public:
	CRedLineDeviceTermMgt();
	~CRedLineDeviceTermMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByStationID(const DWORD32 ID);
	int add(TRedLineDeviceTerm *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TRedLineDeviceTerm *pInputParam);

	int getListCount();
	TRedLineDeviceTerm * getListByIndex(DWORD32 index);

private:
	TRedLineDeviceTerm *m_pList;			//列表
	DWORD32 m_count;						//列表记录数
	DWORD32 m_sumCount;						//现最大记录数

private:
	int add2List(TRedLineDeviceTerm *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TRedLineDeviceTerm *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif