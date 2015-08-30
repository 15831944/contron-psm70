#include "stdafx.h"
#include "BayMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-21
**************************************************************************************************/
CBayMgt::CBayMgt()
{
	m_count = 0;
	m_sumCount = MAX_BAY_COUNT;

	m_pList = new TBay[m_sumCount];
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
CBayMgt::~CBayMgt()
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
int CBayMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

	memset(m_pList, 0x00, m_sumCount * sizeof(TBay));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_bay where stationID=:f_ID<unsigned int> ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TBay temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.name;
			selectStream >> temp.type;
			selectStream >> temp.volID;
            selectStream >> temp.deviceID;
            selectStream >> temp.term;
            selectStream >> temp.remark;
			loadCount++;

            printf("Load Bay<<ID: %d,stationID:%d,name:%s,type:%d,volID:%d,deviceID:%d,term:%s,remark:%s\n", temp.ID, temp.stationID, temp.name, temp.type, temp.volID, temp.deviceID, temp.term, temp.remark);

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
int CBayMgt::retrieveByStationID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

	memset(m_pList, 0x00, m_sumCount * sizeof(TBay));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_bay where stationID=:f_ID<unsigned int> ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TBay temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.name;
			selectStream >> temp.type;
			selectStream >> temp.volID;
            selectStream >> temp.deviceID;
            selectStream >> temp.term;
            selectStream >> temp.remark;
			loadCount++;

            printf("Load Bay<<ID: %d,stationID:%d,name:%s,type:%d,volID:%d,deviceID:%d,term:%s,remark:%s\n", temp.ID, temp.stationID, temp.name, temp.type, temp.volID, temp.deviceID, temp.term, temp.remark);

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
功能：加载所有列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-21
**************************************************************************************************/
int CBayMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )           return -4;

	memset(m_pList, 0x00, m_sumCount * sizeof(TBay));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_bay ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

		TBay temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.name;
			selectStream >> temp.type;
			selectStream >> temp.volID;
            selectStream >> temp.deviceID;
            selectStream >> temp.term;
            selectStream >> temp.remark;
			loadCount++;

            printf("Load Bay<<ID: %d,stationID:%d,name:%s,type:%d,volID:%d,deviceID:%d,term:%s,remark:%s\n", temp.ID, temp.stationID, temp.name, temp.type, temp.volID, temp.deviceID, temp.term, temp.remark);

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
作者：李施施 2014-07-21
**************************************************************************************************/
int CBayMgt::add( TBay *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )           return -4;

    TBay temp = {0};
    memcpy(&temp, pInputParam, sizeof(TBay));

	int lastAddID;
    const char *sql = "insert into t_bay values(NULL,:f1<unsigned int>,:f2<char[64]>,:f3<unsigned int>,:f4<unsigned int>,:f5<unsigned int>,:f6<char[255]>,:f7<char[255]>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->stationID;
		insertStream << pInputParam->name;
		insertStream << pInputParam->type;
		insertStream << pInputParam->volID;
        insertStream << pInputParam->deviceID;
        insertStream << pInputParam->term;
        insertStream << pInputParam->remark;

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

        printf("数据库插入记录ID: %d,stationID:%d,name:%s,type:%d,volID:%d,deviceID:%d,term:%s,remark:%s\n", temp.ID, temp.stationID, temp.name, temp.type,temp.volID,temp.deviceID,temp.term,temp.remark);
		int ret = add2List(&temp);
		if(ret < 0)		return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("添加间隔操作失败！");
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
int CBayMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )           return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

	const char *sql = "DELETE FROM t_bay WHERE ID = :f1_ID<unsigned int>";
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
		WLOutDebug("删除间隔操作失败！");

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
int CBayMgt::update( const DWORD32 ID, TBay *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )           return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

    const char *sql = "UPDATE t_bay SET stationID=:f1<unsigned int>,name=:f2<char[64]>,type=:f3<unsigned int>, volID=:f4<unsigned int>, deviceID=:f5<unsigned int>, term=:f6<char[255]>, remark=:f7<char[255]> WHERE ID=:f8<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
		updateStream << pInputParam->stationID;
		updateStream << pInputParam->name;
		updateStream << pInputParam->type;
		updateStream << pInputParam->volID;
        updateStream << pInputParam->deviceID;
        updateStream << pInputParam->term;
        updateStream << pInputParam->remark;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;

        printf("数据库更新记录ID:ID:%d,stationID:%d,name:%s,type:%d, volID:%d, deviceID:%d, term:%s, remark:%s\n", ID, pInputParam->stationID, pInputParam->name, pInputParam->type, pInputParam->volID, pInputParam->deviceID, pInputParam->term, pInputParam->remark);

	}
	catch (otl_exception& p)
	{
		WLOutDebug("更新间隔资源关系操作失败！");

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
int CBayMgt::add2List( TBay *pInputParam )
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

	memcpy(&m_pList[m_count], pInputParam, sizeof(TBay));
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
int CBayMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pList[i], &m_pList[i+1], sizeof(TBay));
	}
	memset(&m_pList[m_count-1], 0x00, sizeof(TBay));
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
int CBayMgt::isIDExist( const DWORD32 ID )
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
int CBayMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_BAY_COUNT;
	TBay *pResource = new TBay[sumCount];

	if(NULL == pResource)
	{
        WLOutDebug("getMoreMemory():CBayMgt::CBayMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TBay)*m_sumCount);
	memcpy(pResource, m_pList, sizeof(TBay)*m_count);
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
int CBayMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TBay * CBayMgt::getListByIndex( DWORD32 index ) const
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
作者：李施施 2014-07-30
**************************************************************************************************/
int CBayMgt::updateList( const DWORD32 ID, const TBay *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

    TBay *temp = getListByIndex(index);
    memcpy(temp, pInputParam, sizeof(TBay));

	return 0;
}

