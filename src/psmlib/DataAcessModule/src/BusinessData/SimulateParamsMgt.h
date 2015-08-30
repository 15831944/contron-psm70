/**************************************************************************************************
文件名：DeviceMgt.h
功  能：设备管理模块
说  明：提供新增设备、删除设备、修改设备信息、查询所有设备等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __SIMULATEPARAMSMGT__H
#define __SIMULATEPARAMSMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_SIMULATEPARAMS_COUNT 200							//最大记录数量

class CSimulateParamsMgt : public CBusinessDataSet
{
public:
    CSimulateParamsMgt();
    ~CSimulateParamsMgt();

	int retrieve();
    int retrieveByID(const DWORD32 ID);
    int retrieveByStationID(const DWORD32 ID);
    int add(TSimulateParams *pInputParam);
	int del(const DWORD32 ID);
    int update(const DWORD32 ID, TSimulateParams *pInputParam);

	int getListCount() const;
    TSimulateParams * getListByIndex(DWORD32 index) const;
    const TSimulateParams * getListByNumber(DWORD32 number) const;

private:
    TSimulateParams *m_pList;					//列表
    DWORD32 m_count;                            //列表记录数
    DWORD32 m_sumCount;                         //现最大记录数

private:
    int add2List(TSimulateParams *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TSimulateParams *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
