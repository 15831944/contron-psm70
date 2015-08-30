/**************************************************************************************************
文件名：EarthLineMgt.h
功  能：地线管理模块
说  明：提供新增地线、删除地线、修改地线信息、查询所有地线等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __WBSTATISTICS__H
#define __WBSTATISTICS__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_WBSTATISTICS_COUNT 200							//最大标示牌数量

class CWBStatisticsMgt : public CBusinessDataSet
{
public:
    CWBStatisticsMgt();
    ~CWBStatisticsMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByStationID(const DWORD32 ID);
    int retrieveByWBID(const DWORD32 ID);
    int retrieveByDeviceID(const DWORD32 deviceID);
    int retrieve(DWORD32 stationID, DWORD32 WBID, DWORD32 deviceID);
    int add(TWBStatistics *pInputParam);
	int del(const DWORD32 ID);
    int delByWBAndDevice(DWORD32 stationID, DWORD32 WBID, DWORD32 deviceID);
    int update(const DWORD32 ID, TWBStatistics *pInputParam);

	int getListCount() const;
    TWBStatistics * getListByIndex(DWORD32 index) const;

private:
    TWBStatistics *m_pList;			//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TWBStatistics *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TWBStatistics *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
