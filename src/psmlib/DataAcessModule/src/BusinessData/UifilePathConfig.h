﻿/**************************************************************************************************
文件名：UifilePathConfigMgt.h
功  能：图形文件路径配置管理模块
说  明：提供新增图形文件路径配置、删除图形文件路径配置、修改图形文件路径配置信息、查询所有图形文件路径配置等一系列功能
注  意：暂无
作 者： 刘高舜 2014-10-11
**************************************************************************************************/

#ifndef __UIFILEPATHCONFIGMGT__H
#define __UIFILEPATHCONFIGMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_UIFILEPATHCONFIG_COUNT 200							//最大图形文件路径配置数量

class CUifilePathConfigMgt : public CBusinessDataSet
{
public:
    CUifilePathConfigMgt();
    ~CUifilePathConfigMgt();

	int retrieve();
    int retrieveByStationID(DWORD32 stationID);
    int add(TUifilePathConfig *pInputParam);
	int del(const DWORD32 ID);
    int update(const DWORD32 ID, TUifilePathConfig *pInputParam);

	int getListCount() const;
    TUifilePathConfig * getListByIndex(DWORD32 index) const;

private:
    TUifilePathConfig *m_pList;	//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TUifilePathConfig *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TUifilePathConfig *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
