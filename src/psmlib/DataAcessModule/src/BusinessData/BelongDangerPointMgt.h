/**************************************************************************************************
文件名：BelongDangerPointMgt.h
功  能：专用危险点管理模块
说  明：提供新增专用危险点、删除专用危险点、修改专用危险点信息、查询所有专用危险点等一系列功能
注  意：暂无
作 者： 李施施 2014-07-23
**************************************************************************************************/

#ifndef __BELONGDANGERPOINTTERMMGT__H
#define __BELONGDANGERPOINTTERMMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_BELONGDANGERPOINTTERM_COUNT 200							//最大专用危险点数量

class CBelongDangerPointMgt : public CBusinessDataSet
{
public:
	CBelongDangerPointMgt();
	~CBelongDangerPointMgt();

	int retrieve();
	int add(TBelongDangerPoint *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TBelongDangerPoint *pInputParam);

	int getListCount() const;
	TBelongDangerPoint * getListByIndex(DWORD32 index) const;

private:
	TBelongDangerPoint *m_pList;	//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TBelongDangerPoint *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TBelongDangerPoint *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif