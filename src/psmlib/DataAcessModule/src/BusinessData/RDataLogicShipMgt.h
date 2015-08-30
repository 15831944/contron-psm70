/**************************************************************************************************
文件名：RDataLogicShipMgt.h
功  能：操作逻辑关系管理模块
说  明：提供新增操作逻辑关系、删除操作逻辑关系、修改操作逻辑关系、查询所有操作逻辑关系等一系列功能
注  意：暂无
作 者： 李施施 2014-07-23
**************************************************************************************************/

#ifndef __RDATALOGICSHIPMGT__H
#define __RDATALOGICSHIPMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_RDATALOGICSHIP_COUNT 200							//最大五防逻辑关系数量

class CRDataLogicShipMgt : public CBusinessDataSet
{
public:
	CRDataLogicShipMgt();
	~CRDataLogicShipMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByRDataLogicID(const DWORD32 ID);
    int retrieveByRDataLogicID_index_desc(const DWORD32 ID);
    int retrieveByMinMaxID(const DWORD32 minID,const DWORD32 maxID);
	int add(TRDataLogicShip *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TRDataLogicShip *pInputParam);

	int getListCount() const;
	TRDataLogicShip * getListByIndex(DWORD32 index) const;


private:
	TRDataLogicShip *m_pList;		//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TRDataLogicShip *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TRDataLogicShip *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
