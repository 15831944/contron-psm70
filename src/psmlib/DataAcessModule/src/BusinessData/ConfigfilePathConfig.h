/**************************************************************************************************
文件名：ConfigfilePathConfigMgt.h
功  能：配置文件路径配置管理模块
说  明：提供新增配置文件路径配置、删除配置文件路径配置、修改配置文件路径配置信息、查询所有配置文件路径配置等一系列功能
注  意：暂无
作 者： 刘高舜 2014-10-11
**************************************************************************************************/

#ifndef __CONFIGFILEPATHCONFIGMGT__H
#define __CONFIGFILEPATHCONFIGMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_CONFIGFILEPATHCONFIG_COUNT 200							//最大配置文件路径配置数量

class CConfigfilePathConfigMgt : public CBusinessDataSet
{
public:
    CConfigfilePathConfigMgt();
    ~CConfigfilePathConfigMgt();

	int retrieve();
    int retrieveByType( const DWORD32 type );
    int add(TConfigfilePathConfig *pInputParam);
	int del(const DWORD32 ID);
    int update(const DWORD32 ID, TConfigfilePathConfig *pInputParam);

	int getListCount() const;
    TConfigfilePathConfig * getListByIndex(DWORD32 index) const;

private:
    TConfigfilePathConfig *m_pList;	//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TConfigfilePathConfig *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TConfigfilePathConfig *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
