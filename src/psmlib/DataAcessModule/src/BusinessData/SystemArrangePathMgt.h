/**************************************************************************************************
文件名：SystemArrangePathMgt.h
功  能：系统文件部署路径管理模块
说  明：提供新增系统文件部署路径、删除系统文件部署路径、修改系统文件部署路径信息、查询所有系统文件部署路径等一系列功能
注  意：暂无
作 者： 刘高舜 2014-10-24
**************************************************************************************************/

#ifndef SYSTEMARRANGEPATHMGT_H
#define SYSTEMARRANGEPATHMGT_H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_SYSTEMARRANGEPATH_COUNT 200							//最大系统文件部署路径数量

class CSystemArrangePathMgt : public CBusinessDataSet
{
public:
    CSystemArrangePathMgt();
    ~CSystemArrangePathMgt();

    int retrieve();
    int retrieveByPathType(const DWORD32 pathType);
    int add(TSystemArrangePath *pInputParam);
    int del(const DWORD32 ID);
    int delByPathType(const DWORD32 pathType);
    int update(const DWORD32 ID, TSystemArrangePath *pInputParam);

    int getListCount() const;
    TSystemArrangePath * getListByIndex(DWORD32 index) const;

private:
    TSystemArrangePath *m_pList;	//列表
    DWORD32 m_count;				//列表记录数
    DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TSystemArrangePath *pInputParam);
    int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TSystemArrangePath *pInputParam);
    int isIDExist(const DWORD32 ID);
    int getMoreMemory();
};

#endif // SYSTEMARRANGEPATHMGT_H
