/**************************************************************************************************
文件名：VoltageMgt.h
功  能：电压管理模块
说  明：提供新增电压、删除电压、修改电压信息、查询所有电压等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __VOLTAGEMGTMGT__H
#define __VOLTAGEMGTMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_VOLTAGE_COUNT 200							//最大电压数量

class CVoltageMgt : public CBusinessDataSet
{
public:
	CVoltageMgt();
	~CVoltageMgt();

	int retrieve();
    int retrieveOrderByVoltage();
	int add(TVoltage *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TVoltage *pInputParam);

	int getListCount() const;
	TVoltage * getListByIndex(DWORD32 index) const;

private:
	TVoltage *m_pList;				//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TVoltage *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TVoltage *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
