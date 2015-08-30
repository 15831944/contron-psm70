/**************************************************************************************************
文件名：LockMgt.h
功  能：锁具管理模块
说  明：提供新增锁具、删除锁具、修改锁具信息、查询所有锁具等一系列功能
注  意：暂无
作 者： 李施施 2014-07-23
**************************************************************************************************/

#ifndef __LOCKMGT__H
#define __LOCKMGT__H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_LOCK_COUNT 200							//最大锁具数量

class CLockMgt : public CBusinessDataSet
{
public:
	CLockMgt();
	~CLockMgt();

	int retrieve();
	int retrieveByDeviceID(const DWORD32 ID);
    int retrieveOrderByDeviceID();
	int add(TLock *pInputParam);
	int del(const DWORD32 ID);
    int delByDeviceID(const DWORD32 deviceID);
	int update(const DWORD32 ID, TLock *pInputParam);

	int getListCount() const;
    int updateLockCodeByDeviceID(DWORD32 deviceID, DWORD32 lockCode);
	TLock * getListByIndex(DWORD32 index) const;

private:
	TLock *m_pList;					//列表
	DWORD32 m_count;				//列表记录数
	DWORD32 m_sumCount;				//现最大记录数

private:
	int add2List(TLock *pInputParam);
	int delFromList(const DWORD32 ID);
	int updateList(const DWORD32 ID, const TLock *pInputParam);
    int updateListOfLockCode(const DWORD32 deviceID, const DWORD32 lockCode);
	int isIDExist(const DWORD32 ID);
	int getMoreMemory();
};

#endif
