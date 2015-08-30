#include "stdafx.h"
#include "BlackBoxTicLogMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：刘高舜 2014-11-27
**************************************************************************************************/
CBlackBoxTicLogMgt::CBlackBoxTicLogMgt()
{
    m_count = 0;
    m_sumCount = MAX_BLACKBOXTICLOG_COUNT;

    m_pList = new TBlackBoxTicLog[m_sumCount];
    if(NULL == m_pList)
    {
        WLOutDebug("CBlackBoxTicLogMgt::CBlackBoxTicLogMgt()-->初始申请内存失败!");
    }
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：刘高舜 2014-11-27
**************************************************************************************************/
CBlackBoxTicLogMgt::~CBlackBoxTicLogMgt()
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
int CBlackBoxTicLogMgt::retrieve()
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )           return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TBlackBoxTicLog));
    int loadCount = 0;
    m_count = 0;
    const char *sql = "SELECT * FROM t_blackboxticlog ";

    try
    {
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);

        TBlackBoxTicLog temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.ticketID;
            selectStream >> temp.stationNumber;
            selectStream >> temp.ticketProperty;
            selectStream >> temp.stepCount;
            selectStream >> temp.createTime;
            selectStream >> temp.endTime;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }
        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找黑匣子票日志失败！");
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
int CBlackBoxTicLogMgt::add( TBlackBoxTicLog *pInputParam )
{
    if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )           return -4;

    TBlackBoxTicLog temp = {0};
    temp.ticketID = pInputParam->ticketID;
    temp.stationNumber = pInputParam->stationNumber;
    temp.ticketProperty = pInputParam->ticketProperty;
    temp.stepCount = pInputParam->stepCount;
    memcpy(&temp.createTime, &pInputParam->createTime, sizeof(otl_datetime));
    memcpy(&temp.endTime, &pInputParam->endTime, sizeof(otl_datetime));

    int lastAddID;
    char pCreateTime[MAX_PARAM_LEN] = {0};
    char pEndTime[MAX_PARAM_LEN] = {0};
    sprintf(pCreateTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->createTime.year, pInputParam->createTime.month,pInputParam->createTime.day,pInputParam->createTime.hour,pInputParam->createTime.minute,pInputParam->createTime.second);
    sprintf(pEndTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->endTime.year, pInputParam->endTime.month,pInputParam->endTime.day,pInputParam->endTime.hour,pInputParam->endTime.minute,pInputParam->endTime.second);

    char *sql = (char *)malloc(2000);
    sprintf(sql,"insert into t_blackboxticlog values(NULL,:f1_ticketID<unsigned int>,:f2_stationNumber<unsigned int>,:f3_ticketProperty<unsigned int>,:f4_stepCount<unsigned int>,'%s','%s')",pCreateTime,pEndTime);
    try
    {
        otl_stream insertStream(1, sql, m_db);
        insertStream.set_commit(0);
        insertStream << pInputParam->ticketID;
        insertStream << pInputParam->stationNumber;
        insertStream << pInputParam->ticketProperty;
        insertStream << pInputParam->stepCount;

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
        WLOutDebug("添加黑匣子票日志失败！");
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
int CBlackBoxTicLogMgt::del( const DWORD32 ID )
{
    if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )           return -4;

    int ret = isIDExist(ID);
    if(ret < 0)		return -5;

    const char *sql = "DELETE FROM t_blackboxticlog WHERE ID = :f1_ID<unsigned int>";
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
        WLOutDebug("删除黑匣子票日志失败！");

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
int CBlackBoxTicLogMgt::update( const DWORD32 ID, TBlackBoxTicLog *pInputParam )
{
    if(NULL == pInputParam)			return -1;

    if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )           return -4;

    int ret = isIDExist(ID);
    if(ret < 0)						return -3;

    char pCreateTime[MAX_PARAM_LEN] = {0};
    char pEndTime[MAX_PARAM_LEN] = {0};
    sprintf(pCreateTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->createTime.year, pInputParam->createTime.month,pInputParam->createTime.day,pInputParam->createTime.hour,pInputParam->createTime.minute,pInputParam->createTime.second);
    sprintf(pEndTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->endTime.year, pInputParam->endTime.month,pInputParam->endTime.day,pInputParam->endTime.hour,pInputParam->endTime.minute,pInputParam->endTime.second);

    char *sql = (char *)malloc(2000);
    sprintf(sql,"UPDATE t_blackboxticlog set ticketID=:f1_ticketID<unsigned int>,stationNumber=:f2_stationNumber<unsigned int>,ticketProperty=:f3_ticketProperty<unsigned int>,stepCount=:f4_stepCount<unsigned int>,createTime='%s',endTime='%s' where ID=:f_id<unsigned int>",pCreateTime,pEndTime);
    try
    {
        otl_stream updateStream(1, sql, m_db);
        updateStream << pInputParam->ticketID;
        updateStream << pInputParam->stationNumber;
        updateStream << pInputParam->ticketProperty;
        updateStream << pInputParam->stepCount;
        updateStream << ID;

        m_db.commit();
        updateList(ID, pInputParam);
    }
    catch (otl_exception& p)
    {
        WLOutDebug("更新黑匣子票日志失败！");
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
int CBlackBoxTicLogMgt::add2List( TBlackBoxTicLog *pInputParam )
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

    memcpy(&m_pList[m_count], pInputParam, sizeof(TBlackBoxTicLog));
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
int CBlackBoxTicLogMgt::delFromList( const DWORD32 ID )
{
    int index = isIDExist(ID);
    if(index < 0)		return -1;

    m_pCS->enter();

    for(int i=index; i<m_count-1; i++)
    {
        memcpy(&m_pList[i], &m_pList[i+1], sizeof(TBlackBoxTicLog));
    }
    memset(&m_pList[m_count-1], 0x00, sizeof(TBlackBoxTicLog));
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
int CBlackBoxTicLogMgt::isIDExist( const DWORD32 ID )
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
int CBlackBoxTicLogMgt::getMoreMemory()
{
    int sumCount = m_sumCount + MAX_BLACKBOXTICLOG_COUNT;
    TBlackBoxTicLog *pResource = new TBlackBoxTicLog[sumCount];

    if(NULL == pResource)
    {
        WLOutDebug("getMoreMemory():CBlackBoxTicLogMgt::CBlackBoxTicLogMgt()-->申请内存失败!");
        return -1;
    }
    m_sumCount = sumCount;
    memset(pResource, 0x00, sizeof(TBlackBoxTicLog)*m_sumCount);
    memcpy(pResource, m_pList, sizeof(TBlackBoxTicLog)*m_count);
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
int CBlackBoxTicLogMgt::getListCount()
{
    return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：刘高舜 2014-11-27
**************************************************************************************************/
TBlackBoxTicLog * CBlackBoxTicLogMgt::getListByIndex( DWORD32 index )
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
int CBlackBoxTicLogMgt::updateList( const DWORD32 ID, const TBlackBoxTicLog *pInputParam )
{
    if(NULL == pInputParam)		return -1;

    int index = isIDExist(ID);
    if (index < 0)				return -2;

    TBlackBoxTicLog *temp = getListByIndex(index);
    temp->ticketID = pInputParam->ticketID;
    temp->stationNumber = pInputParam->stationNumber;
    temp->ticketProperty = pInputParam->ticketProperty;
    temp->stepCount = pInputParam->stepCount;
    memset(&temp->createTime, 0x00, sizeof(otl_datetime));
    memcpy(&temp->createTime, &pInputParam->createTime, sizeof(otl_datetime));
    memset(&temp->endTime, 0x00, sizeof(otl_datetime));
    memcpy(&temp->endTime, &pInputParam->endTime, sizeof(otl_datetime));

    return 0;
}
