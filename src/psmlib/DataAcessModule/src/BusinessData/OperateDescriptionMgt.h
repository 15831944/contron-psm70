/**************************************************************************************************
文件名：OperateDescriptionMgt.h
功  能：操作描述管理模块
说  明：提供新增操作描述、删除操作描述、修改操作描述、查询所有操作描述等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __OPERATEDESCRIPTIONMGT__H
#define __OPERATEDESCRIPTIONMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_OPERATEDESCRIPTION_COUNT 200							//最大操作描述数量

class COperateDescriptionMgt : public CBusinessDataSet
{
public:
	COperateDescriptionMgt();
	~COperateDescriptionMgt();

	int retrieve();
//	int retrieveByStationID(const DWORD32 ID);
	int retrieveByTermTemplateID(const DWORD32 ID);
	int add(TOperateDescription *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TOperateDescription *pInputParam);

	int getListCount() const;
	TOperateDescription * getListByIndex(DWORD32 index) const;

private:
	TOperateDescription *m_pList;	//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TOperateDescription *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TOperateDescription *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
