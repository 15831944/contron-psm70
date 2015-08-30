﻿/**************************************************************************************************
文件名：EarthLineMgt.h
功  能：地线管理模块
说  明：提供新增地线、删除地线、修改地线信息、查询所有地线等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __EARTHLINEMGT__H
#define __EARTHLINEMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_EARTHLINE_COUNT 200							//最大地线数量

class CEarthLineMgt : public CBusinessDataSet
{
public:
	CEarthLineMgt();
	~CEarthLineMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByStationID(const DWORD32 ID);
    int retrieveByGroupID(const DWORD32 ID);
    int retrieveByGroupIDAndStation(const DWORD32 groupID, const DWORD32 stationID);
    int retrieveByFilterGroupID(const DWORD32 ID);
    int retrieveByStationAndControllerAndCoding(DWORD32 stationID, DWORD32 controllerID, DWORD32 coding);
	int add(TEarthLine *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TEarthLine *pInputParam);
    int resetGroupByGroupID(const DWORD32 ID);
    int resetCtrlIDAndGroupIDByCtrlID(const DWORD32 ControllerID);
    int updateState(DWORD32 stationID, DWORD32 controllerID, DWORD32 coding, DWORD32 state);

	int getListCount() const;
	TEarthLine * getListByIndex(DWORD32 index) const;

private:
	TEarthLine *m_pList;			//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TEarthLine *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 groupID);
    int updateListWithCtrlID(const DWORD32 controllerID);
	int updateList(const DWORD32 ID, const TEarthLine *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
