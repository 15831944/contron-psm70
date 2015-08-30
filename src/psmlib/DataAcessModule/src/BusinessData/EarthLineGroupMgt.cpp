#include "stdafx.h"
#include "EarthLineGroupMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：佘佳明 2015-1-19
**************************************************************************************************/
CEarthLineGroupMgt::CEarthLineGroupMgt()
{
	m_count = 0;
    m_sumCount = MAX_EARTHLINEGROUP_COUNT;

    m_pList = new TEarthLineGroup[m_sumCount];
	if(NULL == m_pList)
	{
		WLOutDebug("CBayMgt::CBayMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：佘佳明 2015-1-19
**************************************************************************************************/
CEarthLineGroupMgt::~CEarthLineGroupMgt()
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
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLineGroup));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_earthlinegroup where ID=:f_ID<unsigned int> ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

        TEarthLineGroup temp = {0};

		while (!selectStream.eof())
        {
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
            selectStream >> temp.volID;
            selectStream >> temp.coding;
            selectStream >> temp.label;
            selectStream >> temp.type;
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
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::retrieveByStationID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLineGroup));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_earthlinegroup where stationID=:f_ID<unsigned int> ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

        TEarthLineGroup temp = {0};

		while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.volID;
            selectStream >> temp.coding;
            selectStream >> temp.label;
            selectStream >> temp.type;
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
功能：根据站ID与地线组类型加载列表
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::retrieveByStationIDAndType( const DWORD32 stationID, const DWORD32 type)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLineGroup));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_earthlinegroup where stationID=:f_ID<unsigned int> AND type=:f_type<unsigned int>";

    try
    {

        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << stationID;
        selectStream << type;

        TEarthLineGroup temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.volID;
            selectStream >> temp.coding;
            selectStream >> temp.label;
            selectStream >> temp.type;
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
功能：根据地线组类型加载列表
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::retrieveByType(const DWORD32 type)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLineGroup));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_earthlinegroup where type=:f_type<unsigned int>";

    try
    {

        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << type;

        TEarthLineGroup temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.volID;
            selectStream >> temp.coding;
            selectStream >> temp.label;
            selectStream >> temp.type;
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
功能：加载所有列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TEarthLineGroup));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_earthlinegroup";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

        TEarthLineGroup temp = {0};

		while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.volID;
            selectStream >> temp.coding;
            selectStream >> temp.label;
            selectStream >> temp.type;
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
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::add( TEarthLineGroup *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TEarthLineGroup temp = {0};
    memcpy(&temp, pInputParam, sizeof(TEarthLineGroup));

	int lastAddID;
    const char *sql = "insert into t_earthlinegroup values(NULL,:f1<unsigned int>,:f2<unsigned int>,:f4<unsigned int>,:f5<char[64]>,:f6<unsigned int>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->stationID;
        insertStream << pInputParam->volID;
        insertStream << pInputParam->coding;
        insertStream << pInputParam->label;
        insertStream << pInputParam->type;

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
        WLOutDebug("添加地线组操作失败！");
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
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

    const char *sql = "DELETE FROM t_earthlinegroup WHERE ID = :f1_ID<unsigned int>";
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
        WLOutDebug("删除地线组操作失败！");

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
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::update( const DWORD32 ID, TEarthLineGroup *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

    const char *sql = "UPDATE t_earthlinegroup SET stationID=:f1<unsigned int>,  \
            volID=:f2<unsigned int>,  \
            coding=:f4<unsigned int>,    \
            label=:f5<char[64]>,    \
            type=:f6<unsigned int>      \
            WHERE ID=:f7<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
        updateStream << pInputParam->stationID;
        updateStream << pInputParam->volID;
        updateStream << pInputParam->coding;
        updateStream << pInputParam->label;
        updateStream << pInputParam->type;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;
	}
	catch (otl_exception& p)
	{
        WLOutDebug("更新地线组操作失败！");

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
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::add2List( TEarthLineGroup *pInputParam )
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

    memcpy(&m_pList[m_count], pInputParam, sizeof(TEarthLineGroup));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
        memcpy(&m_pList[i], &m_pList[i+1], sizeof(TEarthLineGroup));
	}
    memset(&m_pList[m_count-1], 0x00, sizeof(TEarthLineGroup));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::isIDExist( const DWORD32 ID )
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
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::getMoreMemory()
{
    int sumCount = m_sumCount + MAX_EARTHLINEGROUP_COUNT;
    TEarthLineGroup *pResource = new TEarthLineGroup[sumCount];

	if(NULL == pResource)
	{
        WLOutDebug("getMoreMemory():CEarthLineGroupMgt::CEarthLineGroupMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
    memset(pResource, 0x00, sizeof(TEarthLineGroup)*m_sumCount);
    memcpy(pResource, m_pList, sizeof(TEarthLineGroup)*m_count);
	delete [] m_pList;

	m_pList = pResource;
	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：佘佳明 2015-1-19
**************************************************************************************************/
TEarthLineGroup * CEarthLineGroupMgt::getListByIndex( DWORD32 index ) const
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
作者：佘佳明 2015-1-19
**************************************************************************************************/
int CEarthLineGroupMgt::updateList( const DWORD32 ID, const TEarthLineGroup *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

    TEarthLineGroup *temp = getListByIndex(index);
	temp->stationID = pInputParam->stationID;
    temp->volID = pInputParam->volID;
    temp->coding = pInputParam->coding;
    temp->type = pInputParam->type;
    memset(temp->label, 0x00, sizeof(temp->label));
    memcpy(temp->label, pInputParam->label, strlen(pInputParam->label));

	return 0;
}
