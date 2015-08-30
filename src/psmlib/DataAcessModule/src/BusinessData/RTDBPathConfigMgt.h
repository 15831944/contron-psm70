/**************************************************************************************************
文件名：RTDBPathConfigMgt.h
功  能：实时库数据文件路径配置管理模块
说  明：提供新增实时库数据文件路径配置、删除实时库数据文件路径配置、修改实时库数据文件路径配置信息、查询所有实时库数据文件路径配置等一系列功能
注  意：暂无
作 者： 刘高舜 2014-10-13
**************************************************************************************************/

#ifndef __RTDBPATHCONFIGMGT__H
#define __RTDBPATHCONFIGMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_RTDBPATHCONFIGMGT_COUNT 200							//最大实时库数据文件路径配置数量

class CRTDBPathConfigMgt : public CBusinessDataSet
{
public:
    CRTDBPathConfigMgt();
    ~CRTDBPathConfigMgt();

	int retrieve();
    int add(TRTDBPathConfig *pInputParam);
	int del(const DWORD32 ID);
    int update(const DWORD32 ID, TRTDBPathConfig *pInputParam);

	int getListCount() const;
    TRTDBPathConfig * getListByIndex(DWORD32 index) const;

private:
    TRTDBPathConfig *m_pList;	//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TRTDBPathConfig *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TRTDBPathConfig *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
