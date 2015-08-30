#include "stdafx.h"
#include "DBBackupLogMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：刘高舜 2014-11-27
**************************************************************************************************/
CDBBackupLogMgt::CDBBackupLogMgt()
{
    m_count = 0;
    m_sumCount = MAX_DBBACKUPLOG_COUNT;

    m_pList = new TDBBackupLog[m_sumCount];
    if(NULL == m_pList)
    {
        WLOutDebug("CDBBackupLogMgt::CDBBackupLogMgt()-->初始申请内存失败!");
    }
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：刘高舜 2014-11-27
**************************************************************************************************/
CDBBackupLogMgt::~CDBBackupLogMgt()
{
    if (NULL != m_pList)
    {
        delete [] m_pList;
        m_pList = NULL;
    }
}

/**************************************************************************************************
功能：加载所有列表
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：刘高舜 2014-11-27
**************************************************************************************************/
int CDBBackupLogMgt::retrieve()
{
    if(!m_isInit)		return -2;

    memset(m_pList, 0x00, m_sumCount * sizeof(TDBBackupLog));
    int loadCount = 0;
    m_count = 0;
    const char *sql = "SELECT * FROM t_databasebackuplog ";

    try
    {
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);

        TDBBackupLog temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.staffID;
            selectStream >> temp.type;
            selectStream >> temp.fileName;
            selectStream >> temp.time;
            selectStream >> temp.reason;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }
        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找数据库备份日志失败！");
        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}

/**************************************************************************************************
功能：添加记录
参数：pStaion,待插入的记录
返回：-2,数据库尚未连接
      -3，添加到列表失败
      -4,数据库添加操作失败
      0，添加成功
作者：刘高舜 2014-11-27
**************************************************************************************************/
int CDBBackupLogMgt::add( TDBBackupLog *pInputParam )
{
    if(!m_isInit)	return -2;

    TDBBackupLog temp = {0};
    temp.staffID = pInputParam->staffID;
    temp.type = pInputParam->type;
    memcpy(&temp.fileName, &pInputParam->fileName, sizeof(pInputParam->fileName));
    memcpy(&temp.time, &pInputParam->time, sizeof(otl_datetime));
    memcpy(&temp.reason, &pInputParam->reason, sizeof(pInputParam->reason));

    int lastAddID;
    char pTime[MAX_PARAM_LEN] = {0};
    sprintf(pTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->time.year, pInputParam->time.month,pInputParam->time.day,pInputParam->time.hour,pInputParam->time.minute,pInputParam->time.second);

    char *sql = (char *)malloc(2000);
    sprintf(sql,"insert into t_databasebackuplog values(NULL,:f1_staffID<unsigned int>,:f2_type<unsigned int>,:f3_fileName<char[64]>,'%s',:f4_reason<char[255]>)",pTime);
    try
    {
        otl_stream insertStream(1, sql, m_db);
        insertStream.set_commit(0);
        insertStream << pInputParam->staffID;
        insertStream << pInputParam->type;
        insertStream << pInputParam->fileName;
//		insertStream << pInputParam->time;
        insertStream << pInputParam->reason;

        if(1 == m_dbType)//SQLite数据库
        {
            char strLastAddID[20] = {0};
            otl_stream selectStream(1, "select last_insert_rowid()", m_db);
            selectStream.set_commit(0);
            selectStream >> strLastAddID;
            temp.ID = atoi(strLastAddID);
        }
        else
        {
            otl_stream selectStream(1, "SELECT LAST_INSERT_ID()", m_db);
            selectStream.set_commit(0);
            selectStream >> lastAddID;
            temp.ID = lastAddID;
        }

        m_db.commit();

        int ret = add2List(&temp);
        if(ret < 0)
        {
            free(sql);
            return -3;
        }
    }
    catch (otl_exception& p)
    {
        WLOutDebug("添加数据库备份日志失败！");
        m_db.rollback();
        free(sql);

        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    free(sql);

    return temp.ID;
}

/**************************************************************************************************
功能：删除记录
参数：ID，删除的记录的ID
返回：-2，数据库尚未连接
      -3, 从列表中删除记录失败
      -4， 数据库删除操作失败
      0， 成功
作者：刘高舜 2014-11-27
**************************************************************************************************/
int CDBBackupLogMgt::del( const DWORD32 ID )
{
    if(!m_isInit)	return -2;

    int ret = isIDExist(ID);
    if(ret < 0)		return -5;

    const char *sql = "DELETE FROM t_databasebackuplog WHERE ID = :f1_ID<unsigned int>";
    try
    {
        otl_stream delStream(1, sql, m_db);
        delStream << ID;
        m_db.commit();
        printf("数据库删除记录 ID: %d\n", ID);

        int ret = delFromList(ID);
        if(ret < 0)		return -3;
    }
    catch (otl_exception& p)
    {
        WLOutDebug("删除数据库备份日志失败！");

        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}

/**************************************************************************************************
功能：更新指定ID的记录
参数：ID，ID
      pStation，待更新的记录
返回：-1,参数错误
      -2, 数据库尚未连接
      -3，该ID不存在
      -4, 数据库操作失败
      0，更新成功
作者：刘高舜 2014-11-27
**************************************************************************************************/
int CDBBackupLogMgt::update( const DWORD32 ID, TDBBackupLog *pInputParam )
{
    if(NULL == pInputParam)			return -1;

    if(!m_isInit)					return -2;

    int ret = isIDExist(ID);
    if(ret < 0)						return -3;

    char ptime[MAX_PARAM_LEN] = {0};
    sprintf(ptime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->time.year, pInputParam->time.month,pInputParam->time.day,pInputParam->time.hour,pInputParam->time.minute,pInputParam->time.second);

    char *sql = (char *)malloc(2000);
    sprintf(sql,"UPDATE t_databasebackuplog set staffID=:f1_staffID<unsigned int>,type=:f2_type<unsigned int>,fileName=:f3_fileName<char[64]>,time='%s',reason=:f4_reason<char[255]> where ID=:f_id<unsigned int>",ptime);
    try
    {
        otl_stream updateStream(1, sql, m_db);
        updateStream << pInputParam->staffID;
        updateStream << pInputParam->type;
        updateStream << pInputParam->fileName;
        //insertStream << pInputParam->time;
        updateStream << pInputParam->reason;
        updateStream << ID;

        m_db.commit();
        updateList(ID, pInputParam);
    }
    catch (otl_exception& p)
    {
        WLOutDebug("更新数据库备份日志失败！");
        free(sql);

        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    free(sql);

    return 0;
}

/**************************************************************************************************
功能：添加记录到列表
参数：pStaion，待添加的记录
返回：-1，参数错误
      -2， 重复记录
      -3， 申请内存失败
      0， 成功
作者：刘高舜 2014-11-27
**************************************************************************************************/
int CDBBackupLogMgt::add2List( TDBBackupLog *pInputParam )
{
    if(NULL == pInputParam)		return -1;

    int ret = isIDExist(pInputParam->ID);
    if(ret >= 0)			return -2;

    m_pCS->enter();
    if(m_count >= m_sumCount)
    {
        ret = getMoreMemory();
        if(ret < 0)
        {
            m_pCS->leave();
            return -3;
        }
    }

    memcpy(&m_pList[m_count], pInputParam, sizeof(TDBBackupLog));
    m_count++;
    m_pCS->leave();

    return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
      -1，ID不存在
作者：刘高舜 2014-11-27
**************************************************************************************************/
int CDBBackupLogMgt::delFromList( const DWORD32 ID )
{
    int index = isIDExist(ID);
    if(index < 0)		return -1;

    m_pCS->enter();

    for(int i=index; i<m_count-1; i++)
    {
        memcpy(&m_pList[i], &m_pList[i+1], sizeof(TDBBackupLog));
    }
    memset(&m_pList[m_count-1], 0x00, sizeof(TDBBackupLog));
    m_count--;

    m_pCS->leave();
    return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
      -1，ID不存在
作者：刘高舜 2014-11-27
**************************************************************************************************/
int CDBBackupLogMgt::isIDExist( const DWORD32 ID )
{
    int index = -1;

    m_pCS->enter();
    for(int i=0; i<m_count; i++)
    {
        if(m_pList[i].ID == ID)	index = i;
    }
    m_pCS->leave();

    return index;
}

/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
      -1，内存申请失败
作者：刘高舜 2014-11-27
**************************************************************************************************/
int CDBBackupLogMgt::getMoreMemory()
{
    int sumCount = m_sumCount + MAX_DBBACKUPLOG_COUNT;
    TDBBackupLog *pResource = new TDBBackupLog[sumCount];

    if(NULL == pResource)
    {
        WLOutDebug("getMoreMemory():CDBBackupLogMgt::CDBBackupLogMgt()-->申请内存失败!");
        return -1;
    }
    m_sumCount = sumCount;
    memset(pResource, 0x00, sizeof(TDBBackupLog)*m_sumCount);
    memcpy(pResource, m_pList, sizeof(TDBBackupLog)*m_count);
    delete [] m_pList;

    m_pList = pResource;
    return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：刘高舜 2014-11-27
**************************************************************************************************/
int CDBBackupLogMgt::getListCount()
{
    return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：刘高舜 2014-11-27
**************************************************************************************************/
TDBBackupLog * CDBBackupLogMgt::getListByIndex( DWORD32 index )
{
    if(index > m_count-1)	return NULL;

    return &m_pList[index];
}

/**************************************************************************************************
功能：更新本地列表记录
参数：ID,记录ID
      pInputParam，待更新记录
返回：-1,参数错误
      -2，记录不存在
      0，成功
作者：刘高舜 2014-11-27
**************************************************************************************************/
int CDBBackupLogMgt::updateList( const DWORD32 ID, const TDBBackupLog *pInputParam )
{
    if(NULL == pInputParam)		return -1;

    int index = isIDExist(ID);
    if (index < 0)				return -2;

    TDBBackupLog *temp = getListByIndex(index);
    temp->staffID = pInputParam->staffID;
    temp->type = pInputParam->type;
    memset(temp->fileName, 0x00, sizeof(temp->fileName));
    memcpy(temp->fileName, pInputParam->fileName, strlen(pInputParam->fileName));
    memset(&temp->time, 0x00, sizeof(otl_datetime));
    memcpy(&temp->time, &pInputParam->time, sizeof(otl_datetime));
    memset(temp->reason, 0x00, sizeof(temp->reason));
    memcpy(temp->reason, pInputParam->reason, strlen(pInputParam->reason));

    return 0;
}
