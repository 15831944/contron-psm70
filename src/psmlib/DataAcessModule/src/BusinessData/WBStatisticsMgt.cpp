﻿#include "stdafx.h"
#include "WBStatisticsMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：佘佳明 2015-3-4
**************************************************************************************************/
CWBStatisticsMgt::CWBStatisticsMgt()
{
	m_count = 0;
    m_sumCount = MAX_WBSTATISTICS_COUNT;

    m_pList = new TWBStatistics[m_sumCount];
	if(NULL == m_pList)
	{
        WLOutDebug("CWBStatisticsMgt::CWBStatisticsMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：佘佳明 2015-3-4
**************************************************************************************************/
CWBStatisticsMgt::~CWBStatisticsMgt()
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
作者：佘佳明 2015-3-4
**************************************************************************************************/
int CWBStatisticsMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TWBStatistics));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_wbstatistics where ID=:f_ID<unsigned int> ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

        TWBStatistics temp = {0};

		while (!selectStream.eof())
        {
			selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.WBID;
            selectStream >> temp.deviceID;
			loadCount++;


			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找标志牌统计操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：根据站ID加载列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：刘高舜 2014-08-29
**************************************************************************************************/
int CWBStatisticsMgt::retrieveByStationID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TWBStatistics));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_wbstatistics where stationID=:f_ID<unsigned int> ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

        TWBStatistics temp = {0};

		while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.WBID;
            selectStream >> temp.deviceID;
			loadCount++;


			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找标志牌统计操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：根据标示牌ID加载列表
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：刘高舜 2014-08-29
**************************************************************************************************/
int CWBStatisticsMgt::retrieveByWBID( const DWORD32 ID )
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TWBStatistics));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_wbstatistics where WBID=:f_ID<unsigned int> ";

    try
    {

        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << ID;

        TWBStatistics temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.WBID;
            selectStream >> temp.deviceID;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找标志牌统计操作失败！");
        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}

/**************************************************************************************************
功能：根据设备ID加载列表
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：佘佳明 2015-2-4
**************************************************************************************************/
int CWBStatisticsMgt::retrieveByDeviceID(const DWORD32 deviceID)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TWBStatistics));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_wbstatistics where deviceID=:f_ID<unsigned int> ";

    try
    {
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << deviceID;

        TWBStatistics temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.WBID;
            selectStream >> temp.deviceID;
            loadCount++;


            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找标志牌统计操作失败！");
        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}


/**************************************************************************************************
功能：根据组站ID，标志牌ID，设备ID加载列表
参数：stationID，站ID
      WBID，标示牌ID
      deviceID，设备ID
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：唐小灿 2015-02-03
**************************************************************************************************/
int CWBStatisticsMgt::retrieve(DWORD32 stationID, DWORD32 WBID, DWORD32 deviceID)
{
	if(!m_isInit)		return -2;

	if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TWBStatistics));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_wbstatistics where stationID=:f1<unsigned int> and WBID=:f2<unsigned int> and deviceID=:f3<unsigned int>";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << stationID;
        selectStream << WBID;
        selectStream << deviceID;

        TWBStatistics temp = {0};

		while (!selectStream.eof())
		{
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.WBID;
            selectStream >> temp.deviceID;
			loadCount++;


			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

		m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找标志牌统计操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：加载所有列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：佘佳明 2015-3-4
**************************************************************************************************/
int CWBStatisticsMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TWBStatistics));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_wbstatistics";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

        TWBStatistics temp = {0};

		while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.WBID;
            selectStream >> temp.deviceID;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找标志牌统计操作失败！");
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
作者：佘佳明 2015-3-4
**************************************************************************************************/
int CWBStatisticsMgt::add( TWBStatistics *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TWBStatistics temp = {0};
    memcpy(&temp, pInputParam, sizeof(TWBStatistics));

	int lastAddID;
    const char *sql = "insert into t_wbstatistics values(NULL,:f1<unsigned int>,:f2<unsigned int>,:f3<unsigned int>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
        insertStream << pInputParam->stationID;
        insertStream << pInputParam->WBID;
        insertStream << pInputParam->deviceID;

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
		if(ret < 0)		return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("添加标志牌统计操作失败！");
		m_db.rollback();

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return temp.ID;
}

/**************************************************************************************************
功能：删除记录
参数：ID，删除的记录的ID
返回：-2，数据库尚未连接
     -3, 从列表中删除记录失败
     -4， 数据库删除操作失败
     0， 成功
作者：佘佳明 2015-3-4
**************************************************************************************************/
int CWBStatisticsMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

    const char *sql = "DELETE FROM t_wbstatistics WHERE ID = :f1_ID<unsigned int>";
	try
	{
		otl_stream delStream(1, sql, m_db);
		delStream << ID;

		printf("数据库删除记录 ID: %d\n", ID);

        m_db.commit();
		int ret = delFromList(ID);
		if(ret < 0)		return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("删除标志牌统计操作失败！");

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：根据指定参数删除记录
参数：stationID，删除的记录的站ID
     WBID       标示牌ID
     deviceID   设备ID
返回：-2，数据库尚未连接
     -3, 从列表中删除记录失败
     -4， 数据库删除操作失败
     0， 成功
作者：佘佳明 2015-3-2
**************************************************************************************************/
int CWBStatisticsMgt::delByWBAndDevice(DWORD32 stationID, DWORD32 WBID, DWORD32 deviceID)
{
    if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    const char *sql = "DELETE FROM t_wbstatistics WHERE \
            stationID=:f1_SID<unsigned int> AND                 \
            WBID=:f3_ELID<unsigned int> AND              \
            deviceID=:f4_PID<unsigned int>";
    try
    {
        otl_stream delStream(1, sql, m_db);
        delStream << stationID << WBID << deviceID;

        m_db.commit();
    }
    catch (otl_exception& p)
    {
        WLOutDebug("删除标志牌统计操作失败！");

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
作者：佘佳明 2015-3-4
**************************************************************************************************/
int CWBStatisticsMgt::update( const DWORD32 ID, TWBStatistics *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

    const char *sql = "UPDATE t_wbstatistics SET stationID=:f1<unsigned int>,  \
            WBID=:f2<unsigned int>,  \
            deviceID=:f3<unsigned int>    \
            WHERE ID=:f4<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
        updateStream << pInputParam->stationID;
        updateStream << pInputParam->WBID;
        updateStream << pInputParam->deviceID;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("更新标志牌统计操作失败！");

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：添加记录到列表
参数：pStaion，待添加的记录
返回：-1，参数错误
	  -2， 重复记录
	  -3， 申请内存失败
	  0， 成功
作者：佘佳明 2015-3-4
**************************************************************************************************/
int CWBStatisticsMgt::add2List( TWBStatistics *pInputParam )
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

    memcpy(&m_pList[m_count], pInputParam, sizeof(TWBStatistics));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：佘佳明 2015-3-4
**************************************************************************************************/
int CWBStatisticsMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
        memcpy(&m_pList[i], &m_pList[i+1], sizeof(TWBStatistics));
	}
    memset(&m_pList[m_count-1], 0x00, sizeof(TWBStatistics));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：佘佳明 2015-3-4
**************************************************************************************************/
int CWBStatisticsMgt::isIDExist( const DWORD32 ID )
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
作者：佘佳明 2015-3-4
**************************************************************************************************/
int CWBStatisticsMgt::getMoreMemory()
{
    int sumCount = m_sumCount + MAX_WBSTATISTICS_COUNT;
    TWBStatistics *pResource = new TWBStatistics[sumCount];

	if(NULL == pResource)
	{
        WLOutDebug("getMoreMemory():CWBStatisticsMgt::CWBStatisticsMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
    memset(pResource, 0x00, sizeof(TWBStatistics)*m_sumCount);
    memcpy(pResource, m_pList, sizeof(TWBStatistics)*m_count);
	delete [] m_pList;

	m_pList = pResource;
	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：佘佳明 2015-3-4
**************************************************************************************************/
int CWBStatisticsMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：佘佳明 2015-3-4
**************************************************************************************************/
TWBStatistics * CWBStatisticsMgt::getListByIndex( DWORD32 index ) const
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
作者：佘佳明 2015-3-4
**************************************************************************************************/
int CWBStatisticsMgt::updateList( const DWORD32 ID, const TWBStatistics *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

    TWBStatistics *temp = getListByIndex(index);
    memcpy(temp, pInputParam, sizeof(TWBStatistics));

	return 0;
}
