/**************************************************************************************************
文件名：RecordPathConfigMgt.h
功  能：设备点表文件路径配置管理模块
说  明：提供新增设备点表文件路径配置、删除设备点表文件路径配置、修改设备点表文件路径配置信息、查询所有设备点表文件路径配置等一系列功能
注  意：暂无
作 者： 刘高舜 2014-10-13
**************************************************************************************************/

#ifndef __RECORDPATHCONFIGMGT__H
#define __RECORDPATHCONFIGMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_RECORDPATHCONFIGMGT_COUNT 200							//最大设备点表文件路径配置数量

class CRecordPathConfigMgt : public CBusinessDataSet
{
public:
    CRecordPathConfigMgt();
    ~CRecordPathConfigMgt();

	int retrieve();
    int retrieveByStationID(DWORD32 stationID);
    int add(TRecordPathConfig *pInputParam);
	int del(const DWORD32 ID);
    int update(const DWORD32 ID, TRecordPathConfig *pInputParam);

	int getListCount() const;
    TRecordPathConfig * getListByIndex(DWORD32 index) const;

private:
    TRecordPathConfig *m_pList;	//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TRecordPathConfig *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TRecordPathConfig *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
