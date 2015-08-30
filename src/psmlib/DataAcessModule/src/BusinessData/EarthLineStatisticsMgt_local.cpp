#include "stdafx.h"
#include "EarthLineStatisticsMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：佘佳明 2015-1-22
**************************************************************************************************/
CEarthLineStatisticsMgt::CEarthLineStatisticsMgt()
{
	m_count = 0;
    m_sumCount = MAX_EARTHLINESTATISTICS_COUNT;

    m_pList = new TEarthLineStatistics[m_sumCount];
	if(NULL == m_pList)
	{
        WLOutDebug("CEarthLineStatisticsMgt::CEarthLineStatisticsMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：佘佳明 2015-1-22
**************************************************************************************************/
CEarthLineStatisticsMgt::~CEarthLineStatisticsMgt()
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
作者：佘佳明 2015-1-22
**************************************************************************************************/
int CEarthLineStatisticsMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLineStatistics));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_earthlinestatistics where ID=:f_ID<unsigned int> ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

        TEarthLineStatistics temp = {0};

		while (!selectStream.eof())
        {
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
            selectStream >> temp.groupID;
            selectStream >> temp.earthLineID;
            selectStream >> temp.pileID;
			loadCount++;


			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找间隔操作失败！");
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
int CEarthLineStatisticsMgt::retrieveByStationID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLineStatistics));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_earthlinestatistics where stationID=:f_ID<unsigned int> ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

        TEarthLineStatistics temp = {0};

		while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.groupID;
            selectStream >> temp.earthLineID;
            selectStream >> temp.pileID;
			loadCount++;


			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找地线操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：根据组ID加载列表
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：刘高舜 2014-08-29
**************************************************************************************************/
int CEarthLineStatisticsMgt::retrieveByELID( const DWORD32 ID )
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLineStatistics));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_earthlinestatistics where earthLineID=:f_ID<unsigned int> ";

    try
    {

        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << ID;

        TEarthLineStatistics temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.groupID;
            selectStream >> temp.earthLineID;
            selectStream >> temp.pileID;
            loadCount++;


            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找地线操作失败！");
        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}


/**************************************************************************************************
功能：根据组站ID，地线ID，固定接地点ID加载列表
参数：stationID，站ID
      earthLineID，地线ID
	  pileID，固定接地点
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：唐小灿 2015-02-03
**************************************************************************************************/
int CEarthLineStatisticsMgt::retrieve(DWORD32 stationID, DWORD32 earthLineID, DWORD32 pileID)
{
	if(!m_isInit)		return -2;

	if( checkDBConnect() < 0 )      return -4;

	memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLineStatistics));
	int loadCount = 0;
	m_count = 0;

	const char *sql = "SELECT * FROM t_earthlinestatistics where stationID=:f_1<unsigned int> and earthLineID=:f_2<unsigned int> and pileID=f_3<unsigned int>";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << stationID;
		selectStream << earthLineID;
		selectStream << pileID;

		TEarthLineStatistics temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.groupID;
			selectStream >> temp.earthLineID;
			selectStream >> temp.pileID;
			loadCount++;


			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

		m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找地线统计操作失败！");
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
作者：佘佳明 2015-1-22
**************************************************************************************************/
int CEarthLineStatisticsMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLineStatistics));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_earthlinestatistics";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

        TEarthLineStatistics temp = {0};

		while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.groupID;
            selectStream >> temp.earthLineID;
            selectStream >> temp.pileID;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找间隔操作失败！");
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
作者：佘佳明 2015-1-22
**************************************************************************************************/
int CEarthLineStatisticsMgt::add( TEarthLineStatistics *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TEarthLineStatistics temp = {0};
	temp.stationID = pInputParam->stationID;
    temp.groupID = pInputParam->groupID;
    temp.earthLineID = pInputParam->earthLineID;
    temp.pileID = pInputParam->pileID;

	int lastAddID;
    const char *sql = "insert into t_earthlinestatistics values(NULL,:f1<unsigned int>,:f2<unsigned int>,:f3<unsigned int>,:f4<unsigned int>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->stationID;
        insertStream << pInputParam->groupID;
        insertStream << pInputParam->earthLineID;
        insertStream << pInputParam->pileID;

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
		WLOutDebug("添加地线操作失败！");
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
作者：佘佳明 2015-1-22
**************************************************************************************************/
int CEarthLineStatisticsMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

    const char *sql = "DELETE FROM t_earthlinestatistics WHERE ID = :f1_ID<unsigned int>";
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
		WLOutDebug("删除底线操作失败！");

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
作者：佘佳明 2015-1-22
**************************************************************************************************/
int CEarthLineStatisticsMgt::update( const DWORD32 ID, TEarthLineStatistics *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

    const char *sql = "UPDATE t_earthlinestatistics SET stationID=:f1<unsigned int>,  \
            groupID=:f2<unsigned int>,  \
            earthLineID=:f3<unsigned int>,  \
            pileID=:f4<unsigned int>    \
            WHERE ID=:f5<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
        updateStream << pInputParam->stationID;
        updateStream << pInputParam->groupID;
        updateStream << pInputParam->earthLineID;
        updateStream << pInputParam->pileID;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("更新地线操作失败！");

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
作者：佘佳明 2015-1-22
**************************************************************************************************/
int CEarthLineStatisticsMgt::add2List( TEarthLineStatistics *pInputParam )
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

    memcpy(&m_pList[m_count], pInputParam, sizeof(TEarthLineStatistics));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：佘佳明 2015-1-22
**************************************************************************************************/
int CEarthLineStatisticsMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
        memcpy(&m_pList[i], &m_pList[i+1], sizeof(TEarthLineStatistics));
	}
    memset(&m_pList[m_count-1], 0x00, sizeof(TEarthLineStatistics));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：佘佳明 2015-1-22
**************************************************************************************************/
int CEarthLineStatisticsMgt::isIDExist( const DWORD32 ID )
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
作者：佘佳明 2015-1-22
**************************************************************************************************/
int CEarthLineStatisticsMgt::getMoreMemory()
{
    int sumCount = m_sumCount + MAX_EARTHLINESTATISTICS_COUNT;
    TEarthLineStatistics *pResource = new TEarthLineStatistics[sumCount];

	if(NULL == pResource)
	{
        WLOutDebug("getMoreMemory():CEarthLineStatisticsMgt::CEarthLineStatisticsMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
    memset(pResource, 0x00, sizeof(TEarthLineStatistics)*m_sumCount);
    memcpy(pResource, m_pList, sizeof(TEarthLineStatistics)*m_count);
	delete [] m_pList;

	m_pList = pResource;
	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：佘佳明 2015-1-22
**************************************************************************************************/
int CEarthLineStatisticsMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：佘佳明 2015-1-22
**************************************************************************************************/
TEarthLineStatistics * CEarthLineStatisticsMgt::getListByIndex( DWORD32 index ) const
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
作者：佘佳明 2015-1-22
**************************************************************************************************/
int CEarthLineStatisticsMgt::updateList( const DWORD32 ID, const TEarthLineStatistics *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

    TEarthLineStatistics *temp = getListByIndex(index);
	temp->stationID = pInputParam->stationID;
    temp->groupID = pInputParam->groupID;
    temp->earthLineID = pInputParam->earthLineID;
    temp->pileID = pInputParam->pileID;

	return 0;
}
