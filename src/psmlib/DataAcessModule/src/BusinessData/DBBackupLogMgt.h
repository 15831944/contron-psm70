/**************************************************************************************************
文件名：DBBackupLogMgt.h
功  能：数据库备份日志管理模块
说  明：提供新增数据库备份日志、删除数据库备份日志、修改数据库备份日志信息、查询所有数据库备份日志等一系列功能
注  意：暂无
作 者： 刘高舜 2014-11-27
**************************************************************************************************/

#ifndef DBBACKUPLOGMGT_H
#define DBBACKUPLOGMGT_H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_DBBACKUPLOG_COUNT 200								//最大数据库备份日志数量

class CDBBackupLogMgt : public CBusinessDataSet
{
public:
    CDBBackupLogMgt();
    ~CDBBackupLogMgt();

    int retrieve();
    int add(TDBBackupLog *pInputParam);
    int del(const DWORD32 ID);
    int update(const DWORD32 ID, TDBBackupLog *pInputParam);

    int getListCount();
    TDBBackupLog * getListByIndex(DWORD32 index);

private:
    TDBBackupLog *m_pList;				//列表
    DWORD32 m_count;					//列表记录数
    DWORD32 m_sumCount;					//现最大记录数

private:
    int add2List(TDBBackupLog *pInputParam);
    int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TDBBackupLog *pInputParam);
    int isIDExist(const DWORD32 ID);
    int getMoreMemory();
};

#endif // DBBACKUPLOGMGT_H
