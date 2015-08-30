#include "stdafx.h"
#include "TicketPrintSNConfig.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：张林颢 2014-10-11
**************************************************************************************************/
CTicketPrintSNConfig::CTicketPrintSNConfig()
{
	m_count = 0;
    m_sumCount = MAX_TicketPrintSNConfig_COUNT;

    m_pList = new TTicketSNParam[m_sumCount];
	if(NULL == m_pList)
	{
        WLOutDebug("CTicketPrintSNConfig::CTicketPrintSNConfig()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：张林颢 2014-10-11
**************************************************************************************************/
CTicketPrintSNConfig::~CTicketPrintSNConfig()
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
作者：张林颢 2014-10-11
**************************************************************************************************/
int CTicketPrintSNConfig::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TTicketSNParam));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_ticketprintsn ";

	try
	{
        m_db.commit();
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

        TTicketSNParam temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
            selectStream >> temp.stationID;
            DWORD32 addtype;
            selectStream >>addtype;
            temp.addType = NumAddType(addtype);
            DWORD32 clearType;
            selectStream >> clearType;
            temp.clearType = NumClearType(clearType);
            selectStream >> temp.currentNum;
            selectStream >> temp.numDigits;
            selectStream >> temp.SNParam;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
        WLOutDebug("查找操作票打印参数设置失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
    return 0;
}

/**************************************************************************************************
功能：根据站ID查找记录
参数：
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：刘远安 2014-10-13
**************************************************************************************************/
int CTicketPrintSNConfig::retrieveByStationID(DWORD32 stationID)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TTicketSNParam));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_ticketprintsn where stationID=:f1<unsigned int>";

    try
    {
        m_db.commit();
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << stationID;

        TTicketSNParam temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            DWORD32 addtype;
            selectStream >>addtype;
            temp.addType = NumAddType(addtype);
            DWORD32 clearType;
            selectStream >> clearType;
            temp.clearType = NumClearType(clearType);
            selectStream >> temp.currentNum;
            selectStream >> temp.numDigits;
            selectStream >> temp.SNParam;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找操作票打印参数设置失败！");
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
作者：张林颢 2014-10-11
**************************************************************************************************/
int CTicketPrintSNConfig::add( TTicketSNParam *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TTicketSNParam temp = {0};
    temp.stationID = pInputParam->stationID;
    temp.addType = pInputParam->addType;
    temp.clearType = pInputParam->clearType;
    temp.currentNum = pInputParam->currentNum;
    temp.numDigits = pInputParam->numDigits;
    strcpy(temp.SNParam, pInputParam->SNParam);

	int lastAddID;
    char sql[1024] = {0};
    strcat(sql, "insert into t_ticketprintsn values(NULL,:f1<unsigned int>,:f2<unsigned int>,");
    strcat(sql, ":f3<unsigned int>,:f4<unsigned int>,:f5<unsigned int>,:f6<char[128]>)");

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
        insertStream << temp.stationID;
        insertStream << DWORD32(temp.addType);
        insertStream << DWORD32(temp.clearType);
        insertStream << temp.currentNum;
        insertStream << temp.numDigits;
        insertStream << temp.SNParam;

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
        WLOutDebug("添加操作票打印参数设置失败！");
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
作者：张林颢 2014-10-11
**************************************************************************************************/
int CTicketPrintSNConfig::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

    const char *sql = "DELETE FROM t_ticketprintsn WHERE ID = :f1_ID<unsigned int>";
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
        WLOutDebug("删除操作票打印参数设置失败！");

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
作者：张林颢 2014-07-23
**************************************************************************************************/
int CTicketPrintSNConfig::update(const DWORD32 ID, TTicketSNParam *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

    char sql[1024] = {0};
    strcat(sql, "UPDATE t_ticketprintsn set stationID=:f1<unsigned int>,addType=:f2<unsigned int>,clearType=:f3<unsigned int>,");
    strcat(sql, "currentNum=:f4<unsigned int>,numDigits=:f5<unsigned int>,SNParam=:f6<char[128]> ");
    strcat(sql, "WHERE ID=:f7<unsigned int>");

	try
    {
		otl_stream updateStream(1, sql, m_db);
        updateStream << pInputParam->stationID;
        updateStream << DWORD32(pInputParam->addType);
        updateStream << DWORD32(pInputParam->clearType);
        updateStream << pInputParam->currentNum;
        updateStream << pInputParam->numDigits;
        updateStream << pInputParam->SNParam;
        updateStream << pInputParam->ID;

        m_db.commit();

		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;
	}
	catch (otl_exception& p)
	{
        WLOutDebug("更新操作票打印参数设置失败！");

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
作者：张林颢 2014-10-11
**************************************************************************************************/
int CTicketPrintSNConfig::add2List( TTicketSNParam *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int ret = isIDExist(pInputParam->ID);
	if(ret >= 0)				return -2;

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

    memcpy(&m_pList[m_count], pInputParam, sizeof(TTicketSNParam));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：张林颢 2014-10-11
**************************************************************************************************/
int CTicketPrintSNConfig::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
        memcpy(&m_pList[i], &m_pList[i+1], sizeof(TTicketSNParam));
	}
    memset(&m_pList[m_count-1], 0x00, sizeof(TTicketSNParam));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：张林颢 2014-10-11
**************************************************************************************************/
int CTicketPrintSNConfig::isIDExist( const DWORD32 ID )
{
	int index = -1;

	m_pCS->enter();
	for(int i=0; i<m_count; i++)
	{
		if(m_pList[i].ID== ID)	index = i;
	}
	m_pCS->leave();

	return index;
}

/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：张林颢 2014-10-11
**************************************************************************************************/
int CTicketPrintSNConfig::getMoreMemory()
{
    int sumCount = m_sumCount + MAX_TicketPrintSNConfig_COUNT;
    TTicketSNParam *pResource = new TTicketSNParam[sumCount];

	if(NULL == pResource)
	{
        WLOutDebug("CTicketPrintSNConfig::getMoreMemory()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
    memset(pResource, 0x00, sizeof(TTicketSNParam)*m_sumCount);
    memcpy(pResource, m_pList, sizeof(TTicketSNParam)*m_count);
	delete [] m_pList;

	m_pList = pResource;
	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：张林颢 2014-10-11
**************************************************************************************************/
int CTicketPrintSNConfig::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：张林颢 2014-10-11
**************************************************************************************************/
TTicketSNParam * CTicketPrintSNConfig::getListByIndex( DWORD32 index ) const
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
作者：张林颢 2014-10-11
**************************************************************************************************/
int CTicketPrintSNConfig::updateList( const DWORD32 ID, const TTicketSNParam *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

    TTicketSNParam *temp = getListByIndex(index);
    memcpy(temp, pInputParam, sizeof(TTicketSNParam));

	return 0;
}

