/**************************************************************************************************
文件名：ClientPathConfigMgt.h 
功  能：客户端路径配置管理模块
说  明：提供新增客户端路径配置、删除客户端路径配置、修改客户端路径配置信息、查询所有客户端路径配置等一系列功能
注  意：暂无
作 者： 刘高舜 2014-10-11
**************************************************************************************************/

#ifndef __CLIENTPATHCONFIGMGT__H
#define __CLIENTPATHCONFIGMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_CLIENTPATHCONFIG_COUNT 200							//最大客户端路径配置数量

class CClientPathConfigMgt : public CBusinessDataSet
{
public:
    CClientPathConfigMgt();
    ~CClientPathConfigMgt();

	int retrieve();
    int retrieve(const char *appName,DWORD32 system,DWORD32 systemVersion);
    int retrieve(DWORD32 appType,DWORD32 system,DWORD32 systemVersion);
    int add(TClientPathConfig *pInputParam);
	int del(const DWORD32 ID);
    int update(const DWORD32 ID, TClientPathConfig *pInputParam);

	int getListCount() const;
    TClientPathConfig * getListByIndex(DWORD32 index) const;

private:
    TClientPathConfig *m_pList;	//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TClientPathConfig *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TClientPathConfig *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
