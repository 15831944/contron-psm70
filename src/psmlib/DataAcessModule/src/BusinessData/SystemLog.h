/**************************************************************************************************
文件名：DeviceLog.h
功  能：系统日志管理模块
说  明：提供新增系统日志、删除系统日志、修改系统日志信息、查询所有系统日志等一系列功能
注  意：暂无
作 者： 刘高舜 2014-11-11
**************************************************************************************************/

#ifndef SYSTEMLOG_H
#define SYSTEMLOG_H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_SYSTEMLOG_COUNT 200								//最大操作票语数量

class CSystemLog : public CBusinessDataSet
{
public:
    CSystemLog();
    ~CSystemLog();

    int retrieve();
    int add(TSystemLog *pInputParam);
    int del(const DWORD32 ID);
    int update(const DWORD32 ID, TSystemLog *pInputParam);

    int getListCount();
    TSystemLog * getListByIndex(DWORD32 index);

private:
    TSystemLog *m_pList;				//列表
    DWORD32 m_count;					//列表记录数
    DWORD32 m_sumCount;					//现最大记录数

private:
    int add2List(TSystemLog *pInputParam);
    int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TSystemLog *pInputParam);
    int isIDExist(const DWORD32 ID);
    int getMoreMemory();
};

#endif // SYSTEMLOG_H
