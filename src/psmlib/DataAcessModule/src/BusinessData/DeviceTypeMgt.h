/**************************************************************************************************
文件名：DeviceTypeMgt.h
功  能：设备类型管理模块
说  明：提供新增设备类型、删除设备类型、修改设备类型信息、查询所有设备类型等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __DEVICETYPEMGT__H
#define __DEVICETYPEMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_DEVICETYPE_COUNT 200							//最大设备类型数量

class CDeviceTypeMgt : public CBusinessDataSet
{
public:
	CDeviceTypeMgt();
	~CDeviceTypeMgt();

	int retrieve();
    int retrieveByTypeID(const DWORD32 ID);
    int retrieveByStateCount(const DWORD32 stateCount);
	int add(TDeviceType *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TDeviceType *pInputParam);

	int getListCount() const;
	TDeviceType * getListByIndex(DWORD32 index) const;

private:
	TDeviceType *m_pList;				//列表
	DWORD32 m_count;					//列表记录数
	DWORD32 m_sumCount;					//现最大记录数

private:
	int add2List(TDeviceType *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TDeviceType *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
