/**************************************************************************************************
文件名：TicketActionListMgt.h
功  能：操作票动作管理模块
说  明：提供新增操作票动作、删除操作票动作、修改操作票动作信息、查询所有操作票动作等一系列功能
注  意：暂无
作 者： 李施施 2014-07-22
**************************************************************************************************/

#ifndef __HISTORYTICKETACTIONLISTMGT__H
#define __HISTORYTICKETACTIONLISTMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_HISTORYTICKETACTIONLIST_COUNT 200							//最大操作票动作表数量

class CHistoryTicketActionListMgt : public CBusinessDataSet
{
public:
    CHistoryTicketActionListMgt();
    ~CHistoryTicketActionListMgt();

	int retrieve();
	int retrieve(const DWORD32 ID);
	int retrieveByTicketID(const DWORD32 ID);
	int add(TTicketActionList *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TTicketActionList *pInputParam);
    int isIDExist(const DWORD32 ID);

	int getListCount();
	TTicketActionList * getListByIndex(DWORD32 index);
    const TTicketActionList *getListByID(const DWORD32 ID);

private:
	TTicketActionList *m_pList;		//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TTicketActionList *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TTicketActionList *pInputParam);
	int getMoreMemory();
};

#endif
