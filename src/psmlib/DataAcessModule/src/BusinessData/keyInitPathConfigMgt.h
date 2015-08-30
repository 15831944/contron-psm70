/**************************************************************************************************
文件名：KeyInitPathConfig.h
功  能：钥匙打包文件路径配置管理模块
说  明：提供新增钥匙打包文件路径配置、删除钥匙打包文件路径配置、修改钥匙打包文件路径配置信息、查询所有钥匙打包文件路径配置等一系列功能
注  意：暂无
作 者： 刘高舜 2014-10-13
**************************************************************************************************/

#ifndef __KEYINITPATHCONFIG__H
#define __KEYINITPATHCONFIG__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_KEYINITPATHCONFIG_COUNT 200							//最大钥匙打包文件路径配置数量

class CKeyInitPathConfig : public CBusinessDataSet
{
public:
    CKeyInitPathConfig();
    ~CKeyInitPathConfig();

	int retrieve();
    int retrieveByStationID(DWORD32 stationID);
    int add(TKeyInitPathConfig *pInputParam);
	int del(const DWORD32 ID);
    int update(const DWORD32 ID, TKeyInitPathConfig *pInputParam);

	int getListCount() const;
    TKeyInitPathConfig * getListByIndex(DWORD32 index) const;

private:
    TKeyInitPathConfig *m_pList;	//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TKeyInitPathConfig *pInputParam);
	int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TKeyInitPathConfig *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
