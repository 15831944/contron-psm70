/**************************************************************************************************
文件名：DeptResourceShip.h
功  能：部门资源对应关系模块
说  明：提供新增部门资源关系、删除部门资源关系、修改部门资源关系、查询所有部门资源关系等一系列功能
注  意：暂无
作 者： 李施施 2014-07-11
**************************************************************************************************/

#ifndef __DPTRESOURCESHIP__H
#define __DPTRESOURCESHIP__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_DPTRESOURCESHIP_COUNT 200

class CDptResourceShip : public CBusinessDataSet
{

public:
	CDptResourceShip();
	~CDptResourceShip();

	int retrieve();
	int add(const DWORD32 dptID, const DWORD32 resourceID);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, const TDeptResourceShip *pDptResourceShip);

	int delFromList( const DWORD32 *IDList, DWORD32 count );
	DWORD32 getListCount() const;
	TDeptResourceShip * getListByIndex(DWORD32 index) const;

private:
	TDeptResourceShip *m_pDptResourceList;	//部门资源关系列表
	DWORD32 m_count;						//部门资源关系列表记录数
	DWORD32 m_sumCount;						//现最大记录数

private:
	int add2List(const TDeptResourceShip *pResource);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TDeptResourceShip *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};
#endif