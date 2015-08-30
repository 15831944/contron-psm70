/**************************************************************************************************
文件名：FunctionMgt.h
功  能：功能管理模块
说  明：提供新增功能、删除功能、修改功能、查询所有功能等一系列接口
注  意：暂无
作 者： 李施施 2014-07-14
**************************************************************************************************/

#ifndef __FUNCTIONMGT__H
#define __FUNCTIONMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"
#include "PrivilegeMgt.h"
#include "FuncRoleTemplateShip.h"

#define MAX_FUNCTION_COUNT 200

class CFunctionMgt : public CBusinessDataSet
{

public:
	CFunctionMgt();
	~CFunctionMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByUPID(const DWORD32 ID);
	int add(const TFunction *pInputPara);
	int del(const DWORD32 ID);
	int forceDel(const DWORD32 ID, CPrivilegeMgt *pPrivlegeMgt = NULL, CFuncRoleTemplateShip * pFunRoleShip= NULL);
    int update(const DWORD32 ID, const TFunction *pFunction);

    int getRecordListByUpID(const DWORD32 upID,TFunction *pOutList,int inCount,int &outCount);

	DWORD32 getListCount() const;
	TFunction * getListByIndex(DWORD32 index) const;

private:
	TFunction *m_pFunctionList;				//功能列表
	DWORD32 m_count;						//功能列表记录数
	DWORD32 m_sumCount;						//现最大记录数

private:
	int add2List(const TFunction *pFunction);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TFunction *pInputParam);
	int isIDExist(const DWORD32 ID);
    int isPrivilegeCodeExised(const DWORD32 Code);
	int getMoreMemory();
	int getMoreMemory(DWORD32 **pIDList, DWORD32 count, DWORD32 &listcount);
};

#endif
