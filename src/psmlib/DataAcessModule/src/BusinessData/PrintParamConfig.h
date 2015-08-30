/**************************************************************************************************
文件名：PrintParamConfig.h
功  能：操作票打印参数设置
说  明：提供查询设置参数、保存设置参数等一系列功能
注  意：暂无
作 者： 张林颢 2014-11-20
**************************************************************************************************/

#ifndef __PRINTPARAMCONFIG__H
#define __PRINTPARAMCONFIG__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_PRINTPARAM_COUNT 200

class CPrintParamConfig : public CBusinessDataSet
{
public:
    CPrintParamConfig();
    ~CPrintParamConfig();

	int retrieve();
    int retrieveByStationID(DWORD32 stationID);
    int add(TPrintParam *pInputParam);
	int del(const DWORD32 ID);
    int update(const DWORD32 ID, TPrintParam *pInputParam);

	int getListCount() const;
    TPrintParam * getListByIndex(DWORD32 index) const;

private:
    TPrintParam *m_pList;	//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TPrintParam *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TPrintParam *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
