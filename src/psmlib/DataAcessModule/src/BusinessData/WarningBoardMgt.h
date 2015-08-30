/**************************************************************************************************
文件名：WarningBoardMgt.h
功  能：标识牌管理模块
说  明：提供新增标识牌、删除标识牌、修改标识牌信息、查询所有标识牌等一系列功能
注  意：暂无
作 者： 李施施 2014-07-22
**************************************************************************************************/

#ifndef __WARNINGBOARDMGT__H
#define __WARNINGBOARDMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_WARNINGBOARD_COUNT 200							//最大标识牌数量

class CWarningBoardMgt : public CBusinessDataSet
{
public:
	CWarningBoardMgt();
	~CWarningBoardMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int add(TWarningBoard *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TWarningBoard *pInputParam);

	int getListCount() const;
	TWarningBoard * getListByIndex(DWORD32 index) const;

private:
	TWarningBoard *m_pList;			//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TWarningBoard *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TWarningBoard *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif