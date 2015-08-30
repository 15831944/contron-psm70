/**************************************************************************************************
文件名：DeviceLog.h
功  能：设备日志管理模块
说  明：提供新增设备日志、删除设备日志、修改设备日志信息、查询所有设备日志等一系列功能
注  意：暂无
作 者： 刘高舜 2014-11-11
**************************************************************************************************/

#ifndef DEVICELOG_H
#define DEVICELOG_H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_DEVICELOG_COUNT 200								//最大操作票语数量

class CDeviceLog : public CBusinessDataSet
{
public:
    CDeviceLog();
    ~CDeviceLog();

    int retrieve();
    int retrieveBystationID(const DWORD32 ID);
    int add(TDeviceLog *pInputParam);
    int del(const DWORD32 ID);
    int update(const DWORD32 ID, TDeviceLog *pInputParam);

    int getListCount();
    TDeviceLog * getListByIndex(DWORD32 index);

private:
    TDeviceLog *m_pList;				//列表
    DWORD32 m_count;					//列表记录数
    DWORD32 m_sumCount;					//现最大记录数

private:
    int add2List(TDeviceLog *pInputParam);
    int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TDeviceLog *pInputParam);
    int isIDExist(const DWORD32 ID);
    int getMoreMemory();
};

#endif // DEVICELOG_H
