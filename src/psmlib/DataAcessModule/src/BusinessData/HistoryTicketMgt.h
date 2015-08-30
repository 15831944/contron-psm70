/**************************************************************************************************
文件名：OperateTicketMgt.h
功  能：操作票管理模块
说  明：提供新增操作票、删除操作票、修改操作票、查询所有操作票等一系列功能
注  意：暂无
作 者： 李施施 2014-07-22
**************************************************************************************************/

#ifndef __HISTORYTICKETMGT__H
#define __HISTORYTICKETMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_OPERATETICKET_COUNT 200								//最大操作票语数量

class CHistoryTicketMgt : public CBusinessDataSet
{
public:
    CHistoryTicketMgt();
    ~CHistoryTicketMgt();

	int retrieve();
    int retrieve_DESC();
	int retrieve(const DWORD32 ID);
    int retrieveByID(const DWORD32 ID);
	int retrieveByStationID(const DWORD32 ID);
    int retrieveByType(const DWORD32 type);
    int retrieveByType_DESC(const DWORD32 type);
    int retrieveByTicketID(const DWORD32 ID);
    int retrieveByStationIDAndTypeOrderByDate_DESC(const DWORD32 ID, const DWORD32 type);
	int add(TOperateTicket *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TOperateTicket *pInputParam);

	int getListCount();
	TOperateTicket * getListByIndex(DWORD32 index);

private:
	TOperateTicket *m_pList;				//列表
	DWORD32 m_count;						//列表记录数
	DWORD32 m_sumCount;						//现最大记录数

private:
	int add2List(TOperateTicket *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TOperateTicket *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
