/**************************************************************************************************
文件名：RoleTemplate.h
功  能：角色模板管理模块
说  明：提供新增角色模板、删除角色模板、修改角色模板、查询所有角色模板等一系列接口
注  意：暂无
作 者： 李施施 2014-07-14
**************************************************************************************************/

#ifndef __ROLETEMPLATEMGT__H
#define __ROLETEMPLATEMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"
#include "FuncRoleTemplateShip.h"

#define MAX_ROLETEMPLATEMGT_COUNT 200

class CRoleTemplate : public CBusinessDataSet
{
	
public:
	CRoleTemplate();
	~CRoleTemplate();

	int retrieve();
	int add(const TRoleTemplate *pInputPara);
	int del(const DWORD32 ID);
	int forceDel(const DWORD32 ID, CFuncRoleTemplateShip *pFuncRoleShip= NULL);
	int update(const DWORD32 ID, const TRoleTemplate *pRoleTemplate);

	DWORD32 getListCount() const;
	TRoleTemplate * getListByIndex(DWORD32 index) const;

private:
	TRoleTemplate *m_pRoleTemplateList;		//角色模板列表
	DWORD32 m_count;						//角色模板列表记录数
	DWORD32 m_sumCount;						//现最大记录数

private:
	int add2List(const TRoleTemplate *pRoleTemplate);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TRoleTemplate *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
	int getMoreMemory( DWORD32 **pIDList, DWORD32 count, DWORD32 &listCount);
};

#endif
