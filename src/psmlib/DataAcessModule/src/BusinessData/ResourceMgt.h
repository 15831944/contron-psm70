/**************************************************************************************************
文件名：ResourceMgt.h
功  能：资源管理模块
说  明：提供新增资源、删除资源、修改资源信息、查询所有资源等一系列功能
注  意：暂无
作 者： 李施施 2014-07-11
**************************************************************************************************/

#ifndef __RESOURCEMGT__H
#define __RESOURCEMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"
#include "DeptResourceShip.h"
#include "PrivilegeMgt.h"

#define MAX_RESOURCE_COUNT 200

class CResourceMgt : public CBusinessDataSet
{

public:
	CResourceMgt();
	~CResourceMgt();

	int retrieve();
	int retrieve(const DWORD32 resourceCode);
	int add(const  TResource *pInputParam);
	int del(const DWORD32 ID);
	int forceDel(const DWORD32 ID, CDptResourceShip *pDptResShip = NULL, CPrivilegeMgt *pPrivilegeMgt = NULL);
	int update(const DWORD32 ID, const TResource *pResource);

	DWORD32 getListCount() const;
	TResource * getListByIndex(DWORD32 index) const;

private:
	TResource *m_pResourceList;		//资源列表
	DWORD32 m_count;				//资源列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(const TResource *pResource);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TResource *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
	int getMoreMemory( DWORD32 **pIDList, DWORD32 count, DWORD32 &listCount);
};
#endif