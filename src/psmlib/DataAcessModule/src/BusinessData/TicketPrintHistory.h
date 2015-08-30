/**************************************************************************************************
文件名：TicketPrintHistory.h
功  能：操作票打印票编号历史表
说  明：提供生成操作票打印票编号的功能
注  意：暂无
作 者： 韦树远 2014-12-03
**************************************************************************************************/

#ifndef __TICKETPRINTHISTORY_H
#define __TICKETPRINTHISTORY_H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

class CTicketPrintHistory : public CBusinessDataSet
{
public:
    CTicketPrintHistory();
    ~CTicketPrintHistory();

	int generate(DWORD32 ticketID, DWORD32 stationID, DWORD32 pageCount);
	int save(DWORD32 ticketID, DWORD32 ticketPrintID, DWORD32 stationID, DWORD32 pageCount);
};

#endif
