/**************************************************************************************************
文件名：EarthLineMgt.h
功  能：地线管理模块
说  明：提供新增地线、删除地线、修改地线信息、查询所有地线等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __EARTHLINEACTION__H
#define __EARTHLINEACTION__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_EARTHLINEACTION_COUNT 200							//最大地线数量

class CEarthLineActionMgt : public CBusinessDataSet
{
public:
    CEarthLineActionMgt();
    ~CEarthLineActionMgt();

	int retrieve();
    int retrieve(const DWORD32 ID);
    int retrieveByGroupID(const DWORD32 ID);
    int retrieveByTicketID( const DWORD32 ticketID);
    int retrieveByTicketAndActionID( const DWORD32 ticketID,  const DWORD32 actionID);
    int add(TEarthLineAction *pInputParam);
	int del(const DWORD32 ID);
	int delByTicketID(const DWORD32 ticketID);
    int deleteByTicketID(DWORD32 ticketID);
    int update(const DWORD32 ID, TEarthLineAction *pInputParam);
    int update(DWORD32 actionIDOfPrev, DWORD32 actionID);

	int getListCount() const;
    TEarthLineAction * getListByIndex(DWORD32 index) const;

private:
    TEarthLineAction *m_pList;			//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TEarthLineAction *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TEarthLineAction *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
