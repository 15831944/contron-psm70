/**************************************************************************************************
文件名：PrivilegeMgt.h
功  能：权限管理模块
说  明：提供新增权限、删除权限、修改权限信息、查询所有权限等一系列功能
注  意：在进行增删改之前必须先调用retrieve（）函数将权限列表检索出来，否则权限列表为空将无法进行操作
作 者： 李施施 2014-07-15
**************************************************************************************************/
#ifndef __PRIVILEGEMGT_H__
#define __PRIVILEGEMGT_H__

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define  MAX_PRIVILEGE_COUNT 200							//最大角色数量		

class CPrivilegeMgt : public CBusinessDataSet
{
public:
	CPrivilegeMgt();
	~CPrivilegeMgt();

	int retrieve();
	int retrieve(const DWORD32 roleID);
	int retrieveByRoleIDandResourceCode(const DWORD32 roleID, const DWORD32 resourceCode);
	int add(const TPrivilege *pInputParam);
    int insertBatch(const TPrivilege *pList,int count);
    int del(const DWORD32 ID);
    int delBatch(const DWORD32 *pIDList,int count);
	int update(const DWORD32 ID, TPrivilege *pDepartment);

	int delFromList(const DWORD32 *IDList, DWORD32 count);
	DWORD32 getListCount() const;
	TPrivilege * getListByIndex(DWORD32 index) const;

private:

	TPrivilege *m_pPrivilegeList;							//权限列表
	DWORD32 m_count;										//权限列表记录数
	DWORD32 m_sumCount;										//现最大权限记录数量

private:
	int add2List(TPrivilege *pPrivilege);
	int isIDExist(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TPrivilege *pInputParam);
	int delFromList(const DWORD32 ID);
	int getMoreMemory();
};

#endif
