/**************************************************************************************************
文件名：EarthLineMgt.h
功  能：地线管理模块
说  明：提供新增地线、删除地线、修改地线信息、查询所有地线等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __EARTHLINEGROUPMGT__H
#define __EARTHLINEGROUPMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_EARTHLINEGROUP_COUNT 200							//最大地线数量

class CEarthLineGroupMgt : public CBusinessDataSet
{
public:
    CEarthLineGroupMgt();
    ~CEarthLineGroupMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByStationID(const DWORD32 ID);
    int retrieveByStationIDAndType( const DWORD32 stationID, const DWORD32 type);
    int retrieveByType(const DWORD32 type);
    int add(TEarthLineGroup *pInputParam);
	int del(const DWORD32 ID);
    int update(const DWORD32 ID, TEarthLineGroup *pInputParam);

	int getListCount() const;
    TEarthLineGroup * getListByIndex(DWORD32 index) const;

private:
    TEarthLineGroup *m_pList;			//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TEarthLineGroup *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TEarthLineGroup *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
