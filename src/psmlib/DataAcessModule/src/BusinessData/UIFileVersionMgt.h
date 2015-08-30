/**************************************************************************************************
文件名：UIFileVersionMgt.h
功  能：图形文件版本管理模块
注  意：暂无
作 者： 刘高舜 2015-01-31
**************************************************************************************************/

#ifndef UIFILEVERSIONMGT_H
#define UIFILEVERSIONMGT_H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_UIFILEVERSION_COUNT 200

class CUIFileVersionMgt : public CBusinessDataSet
{
public:
    CUIFileVersionMgt();
    ~CUIFileVersionMgt();

    int retrieve();
    int add(TUIFileVersion *pInputParam);
    int del(const DWORD32 ID);
    int update(const DWORD32 ID, TUIFileVersion *pInputParam);

    int getListCount() const;
    TUIFileVersion * getListByIndex(DWORD32 index) const;

private:
    TUIFileVersion *m_pList;		//列表
    DWORD32 m_count;				//列表记录数
    DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TUIFileVersion *pInputParam);
    int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TUIFileVersion *pInputParam);
    int isIDExist(const DWORD32 ID);
    int getMoreMemory();
};

#endif // UIFILEVERSIONMGT_H
