/**************************************************************************************************
文件名：TicketItemListMgt.h
功  能：操作票项管理模块
说  明：提供新增操作票项、删除操作票项、修改操作票项、查询所有操作票项等一系列功能
注  意：暂无
作 者： 李施施 2014-07-22
**************************************************************************************************/

#ifndef __HISTORYTICKETITEMLISTMGT__H
#define __HISTORYTICKETITEMLISTMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_TICKETITEMLIST_COUNT 200								//最大操作票项数量

class CHistoryTicketItemListMgt : public CBusinessDataSet
{
public:
    CHistoryTicketItemListMgt();
    ~CHistoryTicketItemListMgt();

	int retrieve();
    int retrieve(const DWORD32 ID);
    int retrieveByTicketActionID( const DWORD32 ID );
    int retrieveByTicketID( const DWORD32 ID );
	int add(TTicketItemList *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TTicketItemList *pInputParam);
    int isIDExist(const DWORD32 ID);

	int getListCount();
	TTicketItemList * getListByIndex(DWORD32 index);
    const TTicketItemList * getListByID(DWORD32 ID);

private:
	TTicketItemList *m_pList;		//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TTicketItemList *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TTicketItemList *pInputParam);
	int getMoreMemory();
};

#endif
