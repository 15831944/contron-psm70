/**************************************************************************************************
文件名：CommonTermGroupMgt.h
功  能：常用术语组管理模块
说  明：提供新增常用术语组语、删除常用术语组、修改常用术语组、查询所有常用术语组等一系列功能
注  意：暂无
作 者： 李施施 2014-07-22
**************************************************************************************************/

#ifndef __COMMONTERMGROUPMGT__H
#define __COMMONTERMGROUPMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_COMMONTERMGROUP_COUNT 200							//最大常用术语组数量

class CCommonTermGroupMgt : public CBusinessDataSet
{
public:
	CCommonTermGroupMgt();
	~CCommonTermGroupMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByStationID(const DWORD32 ID);
	int add(TCommonTermGroup *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TCommonTermGroup *pInputParam);

	int getListCount() const;
	TCommonTermGroup * getListByIndex(DWORD32 index) const;

private:
	TCommonTermGroup *m_pList;				//列表
	DWORD32 m_count;						//列表记录数
	DWORD32 m_sumCount;						//现最大记录数

private:
	int add2List(TCommonTermGroup *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TCommonTermGroup *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif