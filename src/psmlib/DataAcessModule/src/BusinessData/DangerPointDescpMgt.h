/**************************************************************************************************
文件名：DangerPointDescpMgt.h
功  能：危险点描述管理模块
说  明：提供新增危险点描述、删除危险点描述、修改危险点描述信息、查询所有危险点描述等一系列功能
注  意：暂无
作 者： 李施施 2014-07-23
**************************************************************************************************/

#ifndef __DANGERPOINTDESCPMGT__H
#define __DANGERPOINTDESCPMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_DANGERPOINTDESCP_COUNT 200							//最大锁具数量

class CDangerPointDescpMgt : public CBusinessDataSet
{
public:
	CDangerPointDescpMgt();
	~CDangerPointDescpMgt();

	int retrieve();
	int retrieveByStationID(const DWORD32 ID);
	int add(TDangerPointDescp *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TDangerPointDescp *pInputParam);

	int getListCount() const;
	TDangerPointDescp* getListByIndex(DWORD32 index) const;

private:
	TDangerPointDescp *m_pList;		//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TDangerPointDescp *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TDangerPointDescp *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif