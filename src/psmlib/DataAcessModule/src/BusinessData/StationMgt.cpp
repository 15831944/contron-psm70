#include "stdafx.h"
#include "StationMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-21
**************************************************************************************************/
CStationMgt::CStationMgt()
{
	m_count = 0;
	m_sumCount = MAX_STATION_COUNT;

	m_pList = new TStaion[m_sumCount];
	if(NULL == m_pList)
	{
		WLOutDebug("CStationMgt::CStationMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-21
**************************************************************************************************/
CStationMgt::~CStationMgt()
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
int CStationMgt::retrieve( )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )           return -4;

	memset(m_pList, 0x00, m_sumCount * sizeof(TStaion));
	int loadCount = 0;
	m_count = 0;

    char *sql = (char*)"SELECT * FROM t_station ";

	try
	{
        m_db.commit();
		//查找对应ID的站记录
		otl_stream selectStream(200, sql, m_db);

		TStaion temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.number;
			selectStream >> temp.name;
			selectStream >> temp.taskNameTemplate;
			selectStream >> temp.departmentID;
			selectStream >> temp.volID;
			loadCount++;

            WLOutDebug("Load Staion<<ID: %d,number:%d,name:%s,taskNameTemplate:%d,departmentID:%d,volID:%d\n", temp.ID, temp.number, temp.name, temp.taskNameTemplate, temp.departmentID, temp.volID);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
        WLOutDebug("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找站操作失败！");
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
int CStationMgt::add( TStaion *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )           return -4;

	TStaion temp = {0};
	temp.number = pInputParam->number;
	memcpy(&temp.name, pInputParam->name, strlen(pInputParam->name));
	temp.taskNameTemplate = pInputParam->taskNameTemplate;
	temp.departmentID = pInputParam->departmentID;
	temp.volID = pInputParam->volID;

	int lastAddID;
	char *sql = (char*)"insert into t_station values(NULL,:f_number<unsigned int>,:f_name<char[64]>,:f_taskNameTemplate<unsigned int>,:f_departmentID<unsigned int>,:f_volID<unsigned int>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->number;
		insertStream << pInputParam->name;
		insertStream << pInputParam->taskNameTemplate;
		insertStream << pInputParam->departmentID;
		insertStream << pInputParam->volID;

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

        WLOutDebug("数据库插入记录ID: %d,number:%d,name:%s,taskNameTemplate:%d,departmentID:%d,volID:%d\n", temp.ID, temp.number, temp.name, temp.taskNameTemplate, temp.departmentID, temp.volID);
		int ret = add2List(&temp);
		if(ret < 0)		return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("添加部门资源关系操作失败！");
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
int CStationMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )           return -4;

	int ret = isIDExist(ID);
	if(ret < 0)		return -5;

	char *sql = (char*)"DELETE FROM t_station WHERE ID = :f1_ID<unsigned int>";
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
		WLOutDebug("删除部门资源关系操作失败！");
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
int CStationMgt::update( const DWORD32 ID, TStaion *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )           return -4;

	int ret = isIDExist(ID);
	if(ret < 0)						return -3;

	char *sql = (char*)"UPDATE t_station SET number=:f1<unsigned int>,name=:f2<char[64]>,taskNameTemplate=:f3<unsigned int>, departmentID=:f4<unsigned int>, volID=:f5<unsigned int> WHERE ID=:f6<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
		updateStream << pInputParam->number;
		updateStream << pInputParam->name;
		updateStream << pInputParam->taskNameTemplate;
		updateStream << pInputParam->departmentID;
		updateStream << pInputParam->volID;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;

        WLOutDebug("数据库更新记录ID:ID:%d,number:%d,name:%s,taskNameTemplate:%d, departmentID:%d, volID:%d\n", ID, pInputParam->number, pInputParam->name, pInputParam->taskNameTemplate, pInputParam->departmentID, pInputParam->volID);

	}
	catch (otl_exception& p)
	{
		WLOutDebug("更新部门资源关系操作失败！");
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
int CStationMgt::add2List( TStaion *pInputParam )
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

	memcpy(&m_pList[m_count], pInputParam, sizeof(TStaion));
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
int CStationMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pList[i], &m_pList[i+1], sizeof(TStaion));
	}
	memset(&m_pList[m_count-1], 0x00, sizeof(TStaion));
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
int CStationMgt::isIDExist( const DWORD32 ID )
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
int CStationMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_STATION_COUNT;
	TStaion *pResource = new TStaion[sumCount];

	if(NULL == pResource)
	{
		WLOutDebug("getMoreMemory():CStationMgt::CStationMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TStaion)*m_sumCount);
	memcpy(pResource, m_pList, sizeof(TStaion)*m_count);
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
int CStationMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TStaion * CStationMgt::getListByIndex( DWORD32 index ) const
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
int CStationMgt::updateList( const DWORD32 ID, const TStaion *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TStaion *temp = getListByIndex(index);
	temp->number = pInputParam->number;
	memset(temp->name, 0x00, sizeof(temp->name));
	memcpy(temp->name, pInputParam->name, strlen(pInputParam->name));
	temp->taskNameTemplate = pInputParam->taskNameTemplate;
	temp->departmentID = pInputParam->departmentID;
	temp->volID = pInputParam->volID;

	return 0;
}
