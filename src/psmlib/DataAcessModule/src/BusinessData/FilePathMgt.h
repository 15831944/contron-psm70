/**************************************************************************************************
文件名：FilePathMgt.h
功  能：文件路径管理模块
说  明：提供新增文件路径、删除文件路径、修改文件路径信息、查询所有文件路径等一系列功能
注  意：暂无
作 者： 刘高舜 2014-10-24
**************************************************************************************************/

#ifndef FILEPATHMGT_H
#define FILEPATHMGT_H

#include "BusinessDataType.h"
#include "BusinessDataSet.h"
#include "platformLayer/WLCriticalSection.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define MAX_FILEPATH_COUNT 200							//最大文件路径数量

class CFilePathMgt : public CBusinessDataSet
{
public:
    CFilePathMgt();
    ~CFilePathMgt();

    int retrieve();
    int retrieveByNumber(DWORD32 number);
    int retrieveByNumberAndType(DWORD32 number,DWORD32 fileType);
    int add(TFilePath *pInputParam);
    int del(const DWORD32 ID);
    int update(const DWORD32 ID, TFilePath *pInputParam);

    int getListCount() const;
    TFilePath * getListByIndex(DWORD32 index) const;

private:
    TFilePath *m_pList;	//列表
    DWORD32 m_count;				//列表记录数
    DWORD32 m_sumCount;				//现最大记录数

private:
    int add2List(TFilePath *pInputParam);
    int delFromList(const DWORD32 ID);
    int updateList(const DWORD32 ID, const TFilePath *pInputParam);
    int isIDExist(const DWORD32 ID);
    int getMoreMemory();
};

#endif // FILEPATHMGT_H
