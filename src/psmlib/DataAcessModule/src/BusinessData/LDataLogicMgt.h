/**************************************************************************************************
文件名：LDataLogicMgt.h
功  能：五防逻辑管理模块
说  明：提供新增五防逻辑、删除五防逻辑、修改五防逻辑、查询所有五防逻辑等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __LDATALOGICMGT__H
#define __LDATALOGICMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_LDATALOGIC_COUNT 200							//最大五防逻辑数量

class CLDataLogicMgt : public CBusinessDataSet
{
public:
	CLDataLogicMgt();
	~CLDataLogicMgt();

	int retrieve();
	int retrieveByStationID(const DWORD32 ID);
	int add(TLDataLogic *pInputParam);
	int del(const DWORD32 ID);
    int delByStationID(const DWORD32 ID);
	int update(const DWORD32 ID, TLDataLogic *pInputParam);

	int getListCount() const;
	TLDataLogic * getListByIndex(DWORD32 index) const;

    int getMinMaxIDByStationID(const DWORD32 ID,DWORD32 &outMinID,DWORD32 &outMaxID);

    int getNextInsertID(DWORD32 &outID);
    int getCurrentMaxID(DWORD32 &outMaxID);
private:
	TLDataLogic *m_pList;			//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TLDataLogic *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TLDataLogic *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
