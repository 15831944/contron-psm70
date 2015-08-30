/**************************************************************************************************
文件名：LinkDeviceMgt.h
功  能：设备联动管理模块
说  明：提供新增设备联动、删除设备联动、修改设备联动信息、查询所有设备联动等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __LINKDEVICEMGT__H
#define __LINKDEVICEMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_LINKDIVICE_COUNT 200							//最大设备联动数量

class CLinkDeviceMgt : public CBusinessDataSet
{
public:
	CLinkDeviceMgt();
	~CLinkDeviceMgt();

	int retrieve();
	int retrieveByStationID(const DWORD32 ID);
    int retrieveByMasterDeviceID(const DWORD32 ID);
	int add(TLinkDevice *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TLinkDevice *pInputParam);

	int getListCount() const;
	TLinkDevice * getListByIndex(DWORD32 index) const;

private:
	TLinkDevice *m_pList;			//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TLinkDevice *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TLinkDevice *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
