/**************************************************************************************************
文件名：StaffMgt.h
功  能：人员管理模块
说  明：提供新增人员、删除人员、修改人员信息、查询指定部门下所有人员等一系列功能
注  意：暂无
作 者： 李施施 2014-07-09
**************************************************************************************************/

#ifndef __STAFFMGT__H
#define __STAFFMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_STAFF_COUNT 200					//最大人员数量

class CStaffMgt : public CBusinessDataSet
{

public:
	CStaffMgt();
	~CStaffMgt();

    int retrieve();
	int retrieve(const DWORD32 dptID);
	int add(const TStaff *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TStaff *pStaff);

	int delFromList(const DWORD32 *IDList, DWORD32 count);
	DWORD32 getListCount() const;
	TStaff * getListByIndex(DWORD32 index) const;

private:
	TStaff *m_pStaffList;			//人员列表
	DWORD32 m_count;				//人员列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TStaff *pStaff);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TStaff *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};


#endif
