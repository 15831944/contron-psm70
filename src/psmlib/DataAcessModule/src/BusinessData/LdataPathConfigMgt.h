/**************************************************************************************************
文件名：LdataPathConfigMgt.h
功  能：防误校核文件路径配置管理模块
说  明：提供新增防误校核文件路径配置、删除防误校核文件路径配置、修改防误校核文件路径配置信息、查询所有防误校核文件路径配置等一系列功能
注  意：暂无
作 者： 刘高舜 2014-10-13
**************************************************************************************************/

#ifndef __LDATAPATHCONFIGMGT__H
#define __LDATAPATHCONFIGMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_LDATAPATHCONFIGMGT_COUNT 200							//最大防误校核文件路径配置数量

class CLdataPathConfigMgt : public CBusinessDataSet
{
public:
    CLdataPathConfigMgt();
    ~CLdataPathConfigMgt();

	int retrieve();
    int retrieveByStationID(DWORD32 stationID);
    int add(TLdataPathConfig *pInputParam);
	int del(const DWORD32 ID);
    int update(const DWORD32 ID, TLdataPathConfig *pInputParam);

	int getListCount() const;
    TLdataPathConfig * getListByIndex(DWORD32 index) const;

private:
    TLdataPathConfig *m_pList;	//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TLdataPathConfig *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TLdataPathConfig *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
