/**************************************************************************************************
文件名：FuncRoleTemplateShip.h
功  能：功能角色模板关系模块
说  明：提供新增功能角色模板关系、删除功能角色模板关系、修改功能角色模板关系、查询所有功能角色模板
		关系等一系列接口
注  意：暂无
作 者： 李施施 2014-07-14
**************************************************************************************************/

#ifndef __FUNCROLETEMPLATESHIP__H
#define __FUNCROLETEMPLATESHIP__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_FUNCROLETEMPLATESHIP_COUNT 200

class CFuncRoleTemplateShip : public CBusinessDataSet
{

public:
	CFuncRoleTemplateShip();
	~CFuncRoleTemplateShip();

    void disConnect();
	int retrieve();
	int retrieve(const DWORD32 ID);
	int add(const TFuncRoleTemplateShip *pInputParam);
    int insertBatch(const TFuncRoleTemplateShip *pList,int count);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, const TFuncRoleTemplateShip *pFuncRoleTemplateShip);

	int delFromList(const DWORD32 *IDList, DWORD32 count);
	DWORD32 getListCount() const;
	TFuncRoleTemplateShip * getListByIndex(DWORD32 index) const;

private:
	TFuncRoleTemplateShip *m_pFuncRoleTempList;		//角色模板列表
	DWORD32 m_count;								//角色模板列表记录数
	DWORD32 m_sumCount;								//现最大记录数

private:
	int add2List(const TFuncRoleTemplateShip *pRoleTemplate);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TFuncRoleTemplateShip *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
