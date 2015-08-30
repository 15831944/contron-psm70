#include "stdafx.h"
#include "EarthLineMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-21
**************************************************************************************************/
CEarthLineMgt::CEarthLineMgt()
{
	m_count = 0;
	m_sumCount = MAX_EARTHLINE_COUNT;

	m_pList = new TEarthLine[m_sumCount];
	if(NULL == m_pList)
	{
		WLOutDebug("CBayMgt::CBayMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-21
**************************************************************************************************/
CEarthLineMgt::~CEarthLineMgt()
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
作者：李施施 2014-07-21
**************************************************************************************************/
int CEarthLineMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLine));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_earthline where ID=:f_ID<unsigned int> ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TEarthLine temp = {0};

		while (!selectStream.eof())
        {
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
            selectStream >> temp.groupID;
            selectStream >> temp.controllerID;
			selectStream >> temp.volID;
            selectStream >> temp.coding;
			selectStream >> temp.label;
			selectStream >> temp.state;
            selectStream >> temp.type;
			selectStream >> temp.isBelongDevice;
			selectStream >> temp.belongDeviceID;
			loadCount++;


			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
        WLOutDebug("共 %d 条记录\n", loadCount);
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
int CEarthLineMgt::retrieveByStationID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLine));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_earthline where stationID=:f_ID<unsigned int> ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TEarthLine temp = {0};

		while (!selectStream.eof())
		{
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.groupID;
            selectStream >> temp.controllerID;
            selectStream >> temp.volID;
            selectStream >> temp.coding;
            selectStream >> temp.label;
            selectStream >> temp.state;
            selectStream >> temp.type;
            selectStream >> temp.isBelongDevice;
            selectStream >> temp.belongDeviceID;
			loadCount++;


			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
        WLOutDebug("共 %d 条记录\n", loadCount);
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
int CEarthLineMgt::retrieveByGroupID( const DWORD32 ID )
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLine));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_earthline where groupID=:f_ID<unsigned int> ";

    try
    {

        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << ID;

        TEarthLine temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.groupID;
            selectStream >> temp.controllerID;
            selectStream >> temp.volID;
            selectStream >> temp.coding;
            selectStream >> temp.label;
            selectStream >> temp.state;
            selectStream >> temp.type;
            selectStream >> temp.isBelongDevice;
            selectStream >> temp.belongDeviceID;
            loadCount++;


            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        WLOutDebug("共 %d 条记录\n", loadCount);
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
功能：根据组ID与站ID加载列表
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：刘高舜 2015-01-26
**************************************************************************************************/
int CEarthLineMgt::retrieveByGroupIDAndStation(const DWORD32 groupID, const DWORD32 stationID)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLine));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_earthline where groupID=:f_groupID<unsigned int> AND stationID=:f_stationID<unsigned int> ";

    try
    {
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << groupID;
        selectStream << stationID;

        TEarthLine temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.groupID;
            selectStream >> temp.controllerID;
            selectStream >> temp.volID;
            selectStream >> temp.coding;
            selectStream >> temp.label;
            selectStream >> temp.state;
            selectStream >> temp.type;
            selectStream >> temp.isBelongDevice;
            selectStream >> temp.belongDeviceID;
            loadCount++;


            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        WLOutDebug("共 %d 条记录\n", loadCount);
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
功能：根据过滤组ID加载列表
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：刘高舜 2014-08-29
**************************************************************************************************/
int CEarthLineMgt::retrieveByFilterGroupID(const DWORD32 ID)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLine));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_earthline where groupID!=:f_ID<unsigned int> ";

    try
    {

        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << ID;

        TEarthLine temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.groupID;
            selectStream >> temp.controllerID;
            selectStream >> temp.volID;
            selectStream >> temp.coding;
            selectStream >> temp.label;
            selectStream >> temp.state;
            selectStream >> temp.type;
            selectStream >> temp.isBelongDevice;
            selectStream >> temp.belongDeviceID;
            loadCount++;


            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        WLOutDebug("共 %d 条记录\n", loadCount);
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
功能：根据站ID与控制器ID与地线编号加载列表
参数：stationID    站 ID
     controllerID   控制器ID
     coding         地线编号
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：佘佳明 2015-01-30
**************************************************************************************************/
int CEarthLineMgt::retrieveByStationAndControllerAndCoding(DWORD32 stationID, DWORD32 controllerID, DWORD32 coding)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLine));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_earthline \
            WHERE stationID=:f_stationID<unsigned int> \
            AND controllerID=:f_controllerID<unsigned int> \
            AND coding=:f_coding<unsigned int>";

    try
    {

        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << stationID;
        selectStream << controllerID;
        selectStream << coding;

        TEarthLine temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.groupID;
            selectStream >> temp.controllerID;
            selectStream >> temp.volID;
            selectStream >> temp.coding;
            selectStream >> temp.label;
            selectStream >> temp.state;
            selectStream >> temp.type;
            selectStream >> temp.isBelongDevice;
            selectStream >> temp.belongDeviceID;
            loadCount++;


            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
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
功能：加载所有列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-21
**************************************************************************************************/
int CEarthLineMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLine));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_earthline";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

		TEarthLine temp = {0};

		while (!selectStream.eof())
		{
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.groupID;
            selectStream >> temp.controllerID;
            selectStream >> temp.volID;
            selectStream >> temp.coding;
            selectStream >> temp.label;
            selectStream >> temp.state;
            selectStream >> temp.type;
            selectStream >> temp.isBelongDevice;
            selectStream >> temp.belongDeviceID;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
        WLOutDebug("共 %d 条记录\n", loadCount);
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
作者：李施施 2014-07-21
**************************************************************************************************/
int CEarthLineMgt::add( TEarthLine *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TEarthLine temp = {0};
	temp.stationID = pInputParam->stationID;
    temp.groupID = pInputParam->groupID;
    temp.controllerID = pInputParam->controllerID;
	temp.volID = pInputParam->volID;
    temp.coding = pInputParam->coding;
	memcpy(temp.label, pInputParam->label, strlen(pInputParam->label));
	temp.state = pInputParam->state;
    temp.type = pInputParam->type;
	temp.isBelongDevice = pInputParam->isBelongDevice;
	temp.belongDeviceID = pInputParam->belongDeviceID;

	int lastAddID;
    const char *sql = "insert into t_earthline values(NULL,:f1<unsigned int>,:f2<unsigned int>,:f3<unsigned int>,:f4<unsigned int>,:f5<unsigned int>,:f6<char[64]>,:f7<unsigned int>,:f8<unsigned int>,:f9<unsigned int>,:f10<unsigned int>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->stationID;
        insertStream << pInputParam->groupID;
        insertStream << pInputParam->controllerID;
		insertStream << pInputParam->volID;
        insertStream << pInputParam->coding;
		insertStream << pInputParam->label;
		insertStream << pInputParam->state;
        insertStream << pInputParam->type;
		insertStream << pInputParam->isBelongDevice;
		insertStream << pInputParam->belongDeviceID;

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
作者：李施施 2014-07-21
**************************************************************************************************/
int CEarthLineMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

	const char *sql = "DELETE FROM t_earthline WHERE ID = :f1_ID<unsigned int>";
	try
	{
		otl_stream delStream(1, sql, m_db);
		delStream << ID;

        WLOutDebug("数据库删除记录 ID: %d\n", ID);

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
作者：李施施 2014-07-21
**************************************************************************************************/
int CEarthLineMgt::update( const DWORD32 ID, TEarthLine *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

    const char *sql = "UPDATE t_earthline SET stationID=:f1<unsigned int>,  \
            groupID=:f2<unsigned int>,  \
            controllerID=:f3<unsigned int>,  \
            volID=:f4<unsigned int>,        \
            coding=:f5<unsigned int>,           \
            label=:f6<char[64]>,            \
            state=:f7<unsigned int>,        \
            type=:f8<unsigned int>,         \
            isBelongDevice=:f9<unsigned int>,  \
            belongDeviceID=:f10<unsigned int>    \
            WHERE ID=:f11<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
        updateStream << pInputParam->stationID;
        updateStream << pInputParam->groupID;
        updateStream << pInputParam->controllerID;
        updateStream << pInputParam->volID;
        updateStream << pInputParam->coding;
        updateStream << pInputParam->label;
        updateStream << pInputParam->state;
        updateStream << pInputParam->type;
        updateStream << pInputParam->isBelongDevice;
        updateStream << pInputParam->belongDeviceID;
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

/***************************************************
 * 功能:  将对应的GroupID置0
 * 参数:  GroupID 地线组ID
 * 返回:  <0 失败
 *       >=0 成功
 * 作者:  刘高舜 2015-2-28
 * ************************************************/
int CEarthLineMgt::resetGroupByGroupID(const DWORD32 ID)
{
    if(!m_isInit)					return -1;

    if( checkDBConnect() < 0 )      return -2;

    const char *sql = "UPDATE t_earthline SET groupID=0 WHERE groupID=:f1<unsigned int>";

    try
    {
        otl_stream resetStream(1, sql, m_db);
        resetStream << ID;

        m_db.commit();
        int ret = updateList(ID);
        if(ret < 0)			return -3;
    }
    catch(otl_exception& p)
    {
        WLOutDebug("重置操作失败！");
        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}

/***************************************************
 * 功能:  根据ControllerID将对应的ControllerID和GroupID置0
 * 参数:  GroupID 地线组ID
 * 返回:  <0 失败
 *       >=0 成功
 * 作者:  刘高舜 2015-2-28
 * ************************************************/
int CEarthLineMgt::resetCtrlIDAndGroupIDByCtrlID(const DWORD32 ControllerID)
{
    if(!m_isInit)					return -1;

    if( checkDBConnect() < 0 )      return -2;

    const char *sql = "UPDATE t_earthline SET groupID=0 ,controllerID=0 WHERE controllerID=:f1<unsigned int>";

    try
    {
        otl_stream resetStream(1, sql, m_db);
        resetStream << ControllerID;

        m_db.commit();
        int ret = updateListWithCtrlID(ControllerID);
        if(ret < 0)			return -3;
    }
    catch(otl_exception& p)
    {
        WLOutDebug("重置操作失败！");
        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}

/**************************************************************************************************
功能：更新指定参数的值
参数：stationID    站 ID
      controllerID  控制器ID
      pileNo        桩号
      state         状态值
返回：-1,参数错误
      -2, 数据库尚未连接
      -3，该ID不存在
      -4, 数据库操作失败
      0，更新成功
作者：佘佳明 2015-1-28
**************************************************************************************************/
int CEarthLineMgt::updateState(DWORD32 stationID, DWORD32 controllerID, DWORD32 coding, DWORD32 state)
{
    if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    const char *sql = "UPDATE t_earthline SET state=:f1<unsigned int>  \
            WHERE stationID=:f2<unsigned int> AND controllerID=:f3<unsigned int> AND coding=:f4<unsigned int>";
    try
    {
        otl_stream updateStream(1, sql, m_db);
        updateStream << state;
        updateStream << stationID;
        updateStream << controllerID;
        updateStream << coding;

        m_db.commit();
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
作者：李施施 2014-07-21
**************************************************************************************************/
int CEarthLineMgt::add2List( TEarthLine *pInputParam )
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

	memcpy(&m_pList[m_count], pInputParam, sizeof(TEarthLine));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：李施施 2014-07-21
**************************************************************************************************/
int CEarthLineMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pList[i], &m_pList[i+1], sizeof(TEarthLine));
	}
	memset(&m_pList[m_count-1], 0x00, sizeof(TEarthLine));
	m_count--;

	m_pCS->leave();
    return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：李施施 2014-07-21
**************************************************************************************************/
int CEarthLineMgt::isIDExist( const DWORD32 ID )
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
作者：李施施 2014-07-21
**************************************************************************************************/
int CEarthLineMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_EARTHLINE_COUNT;
	TEarthLine *pResource = new TEarthLine[sumCount];

	if(NULL == pResource)
	{
		WLOutDebug("getMoreMemory():CEarthLineMgt::CEarthLineMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TEarthLine)*m_sumCount);
	memcpy(pResource, m_pList, sizeof(TEarthLine)*m_count);
	delete [] m_pList;

	m_pList = pResource;
	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：李施施 2014-07-29
**************************************************************************************************/
int CEarthLineMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TEarthLine * CEarthLineMgt::getListByIndex( DWORD32 index ) const
{
	if(index > m_count-1)	return NULL;

	return &m_pList[index];
}

/**************************************************************************************************
功能：更新本地列表记录
参数：GroupID,地线组ID
返回：-1,参数错误
      -2，记录不存在
      0，成功
作者：刘高舜 2015-02-28
**************************************************************************************************/
int CEarthLineMgt::updateList(const DWORD32 groupID)
{
    for ( DWORD32 i = 0; i < m_count; i++ )
    {
        TEarthLine *temp = getListByIndex(i);
        if ( temp->groupID == groupID )
        {
            temp->groupID = 0;
        }
    }
    return 0;
}

int CEarthLineMgt::updateListWithCtrlID(const DWORD32 controllerID)
{
    for ( DWORD32 i = 0; i < m_count; i++ )
    {
        TEarthLine *temp = getListByIndex(i);
        if ( temp->controllerID == controllerID)
        {
            temp->controllerID = 0;
        }
    }
    return 0;
}

/**************************************************************************************************
功能：更新本地列表记录
参数：ID,记录ID
	  pInputParam，待更新记录
返回：-1,参数错误
	  -2，记录不存在
	  0，成功
作者：李施施 2014-07-30
**************************************************************************************************/
int CEarthLineMgt::updateList( const DWORD32 ID, const TEarthLine *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TEarthLine *temp = getListByIndex(index);
	temp->stationID = pInputParam->stationID;
    temp->groupID = pInputParam->groupID;
    temp->controllerID = pInputParam->controllerID;
    temp->volID = pInputParam->volID;
    temp->coding = pInputParam->coding;
	memset(temp->label, 0x00, sizeof(temp->label));
	memcpy(temp->label, pInputParam->label, strlen(pInputParam->label));
	temp->state = pInputParam->state;
    temp->type = pInputParam->type;
	temp->isBelongDevice = pInputParam->isBelongDevice;
	temp->belongDeviceID = pInputParam->belongDeviceID;

	return 0;
}
