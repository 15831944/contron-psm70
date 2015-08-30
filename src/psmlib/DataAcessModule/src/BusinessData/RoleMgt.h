/**************************************************************************************************
文件名：RoleMgt.h
功  能：角色管理模块
说  明：提供新增角色、删除角色、修改角色、查询所有角色等一系列接口
注  意：暂无
作 者： 李施施 2014-07-15
**************************************************************************************************/

#ifndef __ROLEMGT__H
#define __ROLEMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"
#include "PrivilegeMgt.h"

#define MAX_ROLE_COUNT 200

class CRoleMgt : public CBusinessDataSet
{

public:
	CRoleMgt();
	~CRoleMgt();

	int retrieve();
	int add(const TRole *pInputParam);
	int del(const DWORD32 ID);
	int forceDel(const DWORD32 ID, CPrivilegeMgt *pPrivilegeMgt = NULL);
	int update(const DWORD32 ID, const TRole *pRole);

	DWORD32 getListCount() const;
	TRole * getListByIndex(DWORD32 index) const;

private:
	TRole *m_pRoleList;								//角色模板列表
	DWORD32 m_count;								//角色模板列表记录数
	DWORD32 m_sumCount;								//现最大记录数

private:
	int add2List(const TRole *pRole);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TRole *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
	int getMoreMemory(DWORD32 **pIDList, DWORD32 count, DWORD32 &listcount);
};

#endif