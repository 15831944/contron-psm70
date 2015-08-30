/**************************************************************************************************
文件名：OperateTermMgt.h
功  能：操作术语管理模块
说  明：提供新增操作术语语、删除操作术语、修改操作术语、查询所有操作术语等一系列功能
注  意：暂无
作 者： 李施施 2014-07-22
**************************************************************************************************/

#ifndef __OPERATETERMMGT__H
#define __OPERATETERMMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_OPERATETERM_COUNT 200							//最大操作术语数量

class COperateTermMgt : public CBusinessDataSet
{
public:
	COperateTermMgt();
	~COperateTermMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByOperateDescriptionID(const DWORD32 ID);
	int add(TOperateTerm *pInputParam);
	int del(const DWORD32 ID);
    int delByOptDescID(const DWORD32 optDescID);
	int update(const DWORD32 ID, TOperateTerm *pInputParam);

	int getListCount();
	TOperateTerm * getListByIndex(DWORD32 index);

private:
	TOperateTerm *m_pList;					//列表
	DWORD32 m_count;						//列表记录数
	DWORD32 m_sumCount;						//现最大记录数

private:
	int add2List(TOperateTerm *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TOperateTerm *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
