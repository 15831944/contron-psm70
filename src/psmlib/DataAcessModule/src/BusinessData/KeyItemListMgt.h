/**************************************************************************************************
文件名：KeyItemListMgt.h
功  能：钥匙项管理模块
说  明：提供新增钥匙项、删除钥匙项、修改钥匙项信息、查询所有钥匙项等一系列功能
注  意：暂无
作 者： 李施施 2014-07-22
**************************************************************************************************/

#ifndef __KEYITEMLISTMGT__H
#define __KEYITEMLISTMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_KEYITEMLIST_COUNT 200							//最大间隔数量

class CKeyItemListMgt : public CBusinessDataSet
{
public:
	CKeyItemListMgt();
	~CKeyItemListMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByTicketActionID(const DWORD32 ID);
	int add(TKeyItemList *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TKeyItemList *pInputParam);

	int getListCount() const;
	TKeyItemList * getListByIndex(DWORD32 index) const;

private:
	TKeyItemList *m_pList;			//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TKeyItemList *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TKeyItemList *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif