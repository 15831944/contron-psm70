﻿/**************************************************************************************************
文件名：TermTemplateMgt.h
功  能：专家术语管理模块
说  明：提供新增专家术语、删除专家术语、修改专家术语、查询所有专家术语等一系列功能
注  意：暂无
作 者： 李施施 2014-07-21
**************************************************************************************************/

#ifndef __TERMTEMPLATEMGT__H
#define __TERMTEMPLATEMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_TERMTEMPLATE_COUNT 200							//最大专家术语数量

class CTermTemplateMgt : public CBusinessDataSet
{
public:
	CTermTemplateMgt();
	~CTermTemplateMgt();

	int retrieve();
    int retrieveByID(const DWORD32 ID);
    int retrieveByStationID(const DWORD32 stationID);
    int retrieveByStationIDAnd0(const DWORD32 stationID);
	int add(TTermTemplate *pInputParam);
	int del(const DWORD32 ID);
	int update(const DWORD32 ID, TTermTemplate *pInputParam);

	int getListCount() const;
	TTermTemplate * getListByIndex(DWORD32 index) const;

private:
	TTermTemplate *m_pList;		//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TTermTemplate *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TTermTemplate *pInputParam);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif