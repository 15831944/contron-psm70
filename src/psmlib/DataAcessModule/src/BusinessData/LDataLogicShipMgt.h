/**************************************************************************************************
文件名：LDataLogicShipMgt.h
功  能：五防逻辑关系管理模块
说  明：提供新增五防逻辑关系、删除五防逻辑关系、修改五防逻辑关系、查询所有五防逻辑关系等一系列功能
注  意：暂无
作 者： 李施施 2014-07-23
**************************************************************************************************/

#ifndef __LDATALOGICSHIPMGT__H
#define __LDATALOGICSHIPMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_LDATALOGICSHIP_COUNT 200							//最大五防逻辑关系数量

class CLDataLogicShipMgt : public CBusinessDataSet
{
public:
	CLDataLogicShipMgt();
	~CLDataLogicShipMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByLDataLogicID(const DWORD32 ID);
    int retrieveByMinMaxID(const DWORD32 minID,const DWORD32 maxID);
	int add(TLDataLogicShip *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TLDataLogicShip *pInputParam);

	int getListCount() const;
	TLDataLogicShip * getListByIndex(DWORD32 index) const;

    int getNextInsertID(DWORD32 &outMaxID);
    int getCurrentMaxID(DWORD32 &outMaxID);

private:
	TLDataLogicShip *m_pList;		//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TLDataLogicShip *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TLDataLogicShip *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
