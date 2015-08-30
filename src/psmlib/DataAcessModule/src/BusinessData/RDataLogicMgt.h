/**************************************************************************************************
文件名：RDataLogicMgt.h
功  能：操作逻辑管理模块
说  明：提供新增操作逻辑、删除五操作逻辑、修改操作逻辑、查询所有操作逻辑等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __RDATALOGICMGT__H
#define __RDATALOGICMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_RDATALOGIC_COUNT 200							//最大操作逻辑数量

class CRDataLogicMgt : public CBusinessDataSet
{
public:
	CRDataLogicMgt();
	~CRDataLogicMgt();

	int retrieve();
	int retrieveByStationID(const DWORD32 ID);
    int retrieveByDevIDAndType(const DWORD32 devID, const DWORD32 type);
	int add(TRDataLogic *pInputParam);
	int del(const DWORD32 ID);
    int delByStationID(const DWORD32 ID);
	int update(const DWORD32 ID, TRDataLogic *pInputParam);

	int getListCount() const;
	TRDataLogic * getListByIndex(DWORD32 index) const;

    int getMinMaxIDByStationID(const DWORD32 ID,DWORD32 &outMinID,DWORD32 &outMaxID);

private:
	TRDataLogic *m_pList;			//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TRDataLogic *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TRDataLogic *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
