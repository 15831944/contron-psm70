/**************************************************************************************************
文件名：DataServiceConfigMgt.h 
功  能：大数据服务配置管理模块
说  明：提供新增大数据服务配置、删除大数据服务配置、修改大数据服务配置信息、查询所有大数据服务配置等一系列功能
注  意：暂无
作 者： 刘高舜 2014-10-11
**************************************************************************************************/

#ifndef __DATASERVICECONFIGMGT__H
#define __DATASERVICECONFIGMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_DATASERVICECONFIG_COUNT 200							//最大大数据服务配置数量

class CDataServiceConfigMgt : public CBusinessDataSet
{
public:
	CDataServiceConfigMgt();
	~CDataServiceConfigMgt();

	int retrieve();
	int add(TDataServiceConfig *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TDataServiceConfig *pInputParam);

	int getListCount() const;
	TDataServiceConfig * getListByIndex(DWORD32 index) const;

private:
	TDataServiceConfig *m_pList;	//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TDataServiceConfig *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TDataServiceConfig *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif