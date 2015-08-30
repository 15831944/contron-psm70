/**************************************************************************************************
文件名：BlackBoxTicOperateLogMgt.h
功  能：黑匣子票操作日志管理模块
说  明：提供新增黑匣子票操作日志、删除黑匣子票操作日志、修改黑匣子票操作日志信息、查询所有黑匣子票操作日志等一系列功能
注  意：暂无
作 者： 刘高舜 2014-11-27
**************************************************************************************************/
#ifndef BLACKBOXTICOPERATELOGMGT_H
#define BLACKBOXTICOPERATELOGMGT_H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_BLACKBOXTICOPERATELOG_COUNT 200								//最大匣子票日志数量

class CBlackBoxTicOperateLogMgt : public CBusinessDataSet
{
public:
    CBlackBoxTicOperateLogMgt();
    ~CBlackBoxTicOperateLogMgt();

    int retrieve();
    int add(TBlackBoxTicOperateLog *pInputParam);
    int del(const DWORD32 ID);
    int update(const DWORD32 ID, TBlackBoxTicOperateLog *pInputParam);

    int getListCount();
    TBlackBoxTicOperateLog * getListByIndex(DWORD32 index);

private:
    TBlackBoxTicOperateLog *m_pList;				//列表
    DWORD32 m_count;					//列表记录数
    DWORD32 m_sumCount;					//现最大记录数

private:
    int add2List(TBlackBoxTicOperateLog *pInputParam);
    int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TBlackBoxTicOperateLog *pInputParam);
    int isIDExist(const DWORD32 ID);
    int getMoreMemory();
};


#endif // BLACKBOXTICOPERATELOGMGT_H
