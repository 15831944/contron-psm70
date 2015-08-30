/**************************************************************************************************
文件名：CommonTermpMgt.h
功  能：常用术语管理模块
说  明：提供新增常用术语语、删除常用术语、修改常用术语、查询所有常用术语等一系列功能
注  意：暂无
作 者： 李施施 2014-07-22
**************************************************************************************************/

#ifndef __COMMONTERMMGT__H
#define __COMMONTERMMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_COMMONTERM_COUNT 200							//最大常用术语数量

class CCommonTermMgt : public CBusinessDataSet
{
public:
	CCommonTermMgt();
	~CCommonTermMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
    int retrieveByCommonTermGroupID(const DWORD32 ID);
	int add(TCommonTerm *pInputParam);
	int del(const DWORD32 ID);
    int delByGroupID(const DWORD32 ID);
	int update(const DWORD32 ID, TCommonTerm *pInputParam);

	int getListCount() const;
	TCommonTerm* getListByIndex(DWORD32 index) const;

private:
	TCommonTerm *m_pList;					//列表
	DWORD32 m_count;						//列表记录数
	DWORD32 m_sumCount;						//现最大记录数

private:
	int add2List(TCommonTerm *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TCommonTerm *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
