/**************************************************************************************************
文件名：DepartmentMgt.h
功  能：部门管理模块
说  明：提供新增部门、删除部门、修改部门信息、查询所有部门等一系列功能
注  意：在进行增删改之前必须先调用retrieve（）函数将部门列表检索出来，否则部门列表为空将无法进行操作
作 者： 李施施 2014-07-08
**************************************************************************************************/
#ifndef __DEPARTMENTMGT_H__
#define __DEPARTMENTMGT_H__

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"
#include "StaffMgt.h"
#include "DeptResourceShip.h"

#define  MAX_DEPARTMENT_COUNT 200							//最大部门数量		

class CDepartmentMgt : public CBusinessDataSet
{
public:
	CDepartmentMgt();
	~CDepartmentMgt();

	int retrieve();
	int add(const char *pName, const DWORD32 UPID, const DWORD32 type);
	int del(const DWORD32 ID);											
	int forceDel(const DWORD32 ID, CStaffMgt *pStaffMgt = NULL, CDptResourceShip *pDptRecShip = NULL);
	int update(const DWORD32 ID, TDepartment *pDepartment);

	DWORD32 getListCount() const;
	TDepartment * getListByIndex(DWORD32 index) const;

private:

	TDepartment *m_pDepartmentList;							//部门列表
	DWORD32 m_Count;										//部门列表记录数
	DWORD32 m_sumCount;										//现最大部门记录数量

private:
	int add2List(TDepartment *pDepartment);
	int isIDExist(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TDepartment *pInputParam);
	int delFromList(const DWORD32 ID);
	int getMoreMemory();
	int getMoreMemory(DWORD32 **pIDList, DWORD32 count, DWORD32 &listcount);
};

#endif
