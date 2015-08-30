/**************************************************************************************************
文件名：TaskNameTemplate.h
功  能：操作任务名模板模块
说  明：提供新增操作任务名模板、删除任务名模板、修改任务名模板、查询所有任务名模板等一系列功能
注  意：暂无
作 者： 李施施 2014-07-22
**************************************************************************************************/

#ifndef __TASKNAMETEMPLATE__H
#define __TASKNAMETEMPLATE__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_TASKNAMETEMPLATE_COUNT 200								//最大任务名模板数量

class CTaskNameTemplate : public CBusinessDataSet
{
public:
	CTaskNameTemplate();
	~CTaskNameTemplate();

	int retrieve();
	int retrieveByTaskTypeID(const DWORD32 ID);
	int add(TTaskNameTemplate *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TTaskNameTemplate *pInputParam);
    int delByTaskTypeID(const DWORD32 ID);

	int getListCount() const;
	TTaskNameTemplate * getListByIndex(DWORD32 index) const;

private:
	TTaskNameTemplate *m_pList;		//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TTaskNameTemplate *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TTaskNameTemplate *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
