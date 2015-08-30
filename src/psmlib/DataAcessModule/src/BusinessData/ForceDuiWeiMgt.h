/**************************************************************************************************
文件名：ForceDuiWeiMgt.h
功  能：强制对位设备管理模块
说  明：提供新增强制对位设备、删除强制对位设备、修改强制对位设备信息、查询所有强制对位设备等一系列功能
注  意：暂无
作 者： 刘高舜 2014-10-15
**************************************************************************************************/

#ifndef __FORCEDUIWEIMGT__H
#define __FORCEDUIWEIMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_FORCEDUIWEI_COUNT 200							//最大强制对位设备表数量

class CForceDuiWeiMgt : public CBusinessDataSet
{
public:
    CForceDuiWeiMgt();
    ~CForceDuiWeiMgt();

	int retrieve();
    int retrieveByStationID(const DWORD32 ID);
    int add(TForceDuiWei *pInputParam);
	int del(const DWORD32 ID);
    int update(const DWORD32 ID, TForceDuiWei *pInputParam);

	int getListCount();
    TForceDuiWei * getListByIndex(DWORD32 index);

private:
    TForceDuiWei *m_pList;		//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TForceDuiWei *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TForceDuiWei *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
