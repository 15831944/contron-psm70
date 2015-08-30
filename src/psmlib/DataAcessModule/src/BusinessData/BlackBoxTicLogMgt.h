/**************************************************************************************************
文件名：BlackBoxTicLogMgt.h
功  能：黑匣子票日志管理模块
说  明：提供新增黑匣子票日志、删除黑匣子票日志、修改黑匣子票日志信息、查询所有黑匣子票日志等一系列功能
注  意：暂无
作 者： 刘高舜 2014-11-27
**************************************************************************************************/
#ifndef BLACKBOXTICLOGMGT_H
#define BLACKBOXTICLOGMGT_H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_BLACKBOXTICLOG_COUNT 200								//最大匣子票日志数量

class CBlackBoxTicLogMgt : public CBusinessDataSet
{
public:
    CBlackBoxTicLogMgt();
    ~CBlackBoxTicLogMgt();

    int retrieve();
    int add(TBlackBoxTicLog *pInputParam);
    int del(const DWORD32 ID);
    int update(const DWORD32 ID, TBlackBoxTicLog *pInputParam);

    int getListCount();
    TBlackBoxTicLog * getListByIndex(DWORD32 index);

private:
    TBlackBoxTicLog *m_pList;				//列表
    DWORD32 m_count;					//列表记录数
    DWORD32 m_sumCount;					//现最大记录数

private:
    int add2List(TBlackBoxTicLog *pInputParam);
    int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TBlackBoxTicLog *pInputParam);
    int isIDExist(const DWORD32 ID);
    int getMoreMemory();
};

#endif // BLACKBOXTICLOGMGT_H
