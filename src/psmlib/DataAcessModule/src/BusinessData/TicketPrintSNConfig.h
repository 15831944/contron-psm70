/**************************************************************************************************
文件名：TicketPrintSNConfig.h
功  能：操作票打印票编号参数设置
说  明：提供查询设置票编号参数、保存设置参数等一系列功能
注  意：暂无
作 者： 张林颢 2014-12-02
**************************************************************************************************/

#ifndef __TICKETPRINTSNCONFIG__H
#define __TICKETPRINTSNCONFIG__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_TicketPrintSNConfig_COUNT 200

class CTicketPrintSNConfig : public CBusinessDataSet
{
public:
    CTicketPrintSNConfig();
    ~CTicketPrintSNConfig();

	int retrieve();
    int retrieveByStationID(DWORD32 stationID);
    int add(TTicketSNParam *pInputParam);
	int del(const DWORD32 ID);
    int update(const DWORD32 ID, TTicketSNParam *pInputParam);

	int getListCount() const;
    TTicketSNParam * getListByIndex(DWORD32 index) const;

private:
    TTicketSNParam *m_pList;            //列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TTicketSNParam *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TTicketSNParam *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
