/**************************************************************************************************
文件名：TaskTypeMgt.h
功  能：任务类型管理模块
说  明：提供新增任务类型、删除任务类型、修改任务类型信息、查询所有任务类型等一系列功能
注  意：暂无
作 者： 刘高舜 2014-08-29
**************************************************************************************************/

#ifndef __TASKTYPEMGT__H
#define __TASKTYPEMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_TASKTYPE_COUNT 200							//最大间隔数量

class CTaskTypeMgt : public CBusinessDataSet
{
public:
	CTaskTypeMgt();
	~CTaskTypeMgt();

	int retrieve();
	int add(TTaskType *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TTaskType *pInputParam);

	int getListCount() const;
	TTaskType * getListByIndex(DWORD32 index) const;

private:
	TTaskType *m_pList;				//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TTaskType *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TTaskType *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif