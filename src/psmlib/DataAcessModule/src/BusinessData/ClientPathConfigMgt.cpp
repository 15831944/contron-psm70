#include "stdafx.h"
#include "ClientPathConfigMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：刘高舜 2014-10-11
**************************************************************************************************/
CClientPathConfigMgt::CClientPathConfigMgt()
{
	m_count = 0;
    m_sumCount = MAX_CLIENTPATHCONFIG_COUNT;

    m_pList = new TClientPathConfig[m_sumCount];
	if(NULL == m_pList)
	{
        WLOutDebug("CClientPathConfigMgt::CClientPathConfigMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：刘高舜 2014-10-11
**************************************************************************************************/
CClientPathConfigMgt::~CClientPathConfigMgt()
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
作者：刘高舜 2014-10-11
**************************************************************************************************/
int CClientPathConfigMgt::retrieve()
{
	if(!m_isInit)		return -2;

    memset(m_pList, 0x00, m_sumCount * sizeof(TClientPathConfig));
	int loadCount = 0;
	m_count = 0;

    char *sql = (char*)"SELECT * FROM t_clientpathconfig";

	try
	{
        m_db.commit();
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

        TClientPathConfig temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
            selectStream >> temp.name;
            selectStream >> temp.appType;
            selectStream >> temp.system;
            selectStream >> temp.systemVersion;
            selectStream >> temp.programVersion;
            selectStream >> temp.isForceUpdate;
            selectStream >> temp.path;
            selectStream >> temp.isFile;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
        WLOutDebug("查找客户端路径配置失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
    return 0;
}

/**************************************************************************************************
功能：根据应用程序名称、操作系统、操作系统版本查找记录
参数：
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：刘远安 2014-10-11
**************************************************************************************************/
int CClientPathConfigMgt::retrieve(const char *appName, DWORD32 system, DWORD32 systemVersion)
{
    if(!m_isInit)		return -2;

    memset(m_pList, 0x00, m_sumCount * sizeof(TClientPathConfig));
    int loadCount = 0;
    m_count = 0;

    char *sql = (char*)"SELECT * FROM t_clientpathconfig where name=:f1<char[64]> and system=:f2<unsigned int> and systemVersion=:f3<unsigned int>";
//    char *sql = "SELECT * FROM t_clientpathconfig where system=:f1<unsigned int> and systemVersion=:f2<unsigned int>";

    try
    {
        m_db.commit();
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << appName;
        selectStream << system;
        selectStream << systemVersion;


        TClientPathConfig temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.name;
            selectStream >> temp.appType;
            selectStream >> temp.system;
            selectStream >> temp.systemVersion;
            selectStream >> temp.programVersion;
            selectStream >> temp.isForceUpdate;
            selectStream >> temp.path;
            selectStream >> temp.isFile;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找客户端路径配置失败！");
        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}

/**************************************************************************************************
功能：根据应用程序类型、操作系统、操作系统版本查找记录
参数：
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：刘远安 2014-10-11
**************************************************************************************************/
int CClientPathConfigMgt::retrieve(DWORD32 appType, DWORD32 system, DWORD32 systemVersion)
{
    if(!m_isInit)		return -2;

    memset(m_pList, 0x00, m_sumCount * sizeof(TClientPathConfig));
    int loadCount = 0;
    m_count = 0;

    char *sql = (char*)"SELECT * FROM t_clientpathconfig where appType=:f1<unsigned int> and system=:f2<unsigned int> and systemVersion=:f3<unsigned int>";

    try
    {
        m_db.commit();
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << appType;
        selectStream << system;
        selectStream << systemVersion;


        TClientPathConfig temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.name;
            selectStream >> temp.appType;
            selectStream >> temp.system;
            selectStream >> temp.systemVersion;
            selectStream >> temp.programVersion;
            selectStream >> temp.isForceUpdate;
            selectStream >> temp.path;
            selectStream >> temp.isFile;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找客户端路径配置失败！");
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
作者：刘高舜 2014-10-11
**************************************************************************************************/
int CClientPathConfigMgt::add( TClientPathConfig *pInputParam )
{
	if(!m_isInit)	return -2;

    TClientPathConfig temp = {0};
    memcpy(&temp.name, pInputParam->name,strlen(pInputParam->name));
    temp.appType = pInputParam->appType;
    temp.system = pInputParam->system;
    temp.systemVersion = pInputParam->systemVersion;
    temp.programVersion = pInputParam->programVersion;
    temp.isForceUpdate = pInputParam->isForceUpdate;
    memcpy(&temp.path, pInputParam->path,strlen(pInputParam->path));
    temp.isFile = pInputParam->isFile;

	int lastAddID;
    char *sql = (char*)"insert into t_clientpathconfig values(NULL,:f1<char[64]>,:f2<unsigned int>,:f3<unsigned int>,:f4<unsigned int>,:f5<unsigned int>,:f6<char[255]>,:f7<unsigned int>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
        insertStream << pInputParam->name;
        insertStream << pInputParam->appType;
        insertStream << pInputParam->system;
        insertStream << pInputParam->systemVersion;
        insertStream << pInputParam->programVersion;
        insertStream << pInputParam->isForceUpdate;
        insertStream << pInputParam->path;
        insertStream << pInputParam->isFile;

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
        WLOutDebug("添加客户端路径配置失败！");
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
作者：刘高舜 2014-10-11
**************************************************************************************************/
int CClientPathConfigMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

	int ret = isIDExist(ID);
	if(ret < 0)		return -5;

    char *sql = (char*)"DELETE FROM t_clientpathconfig WHERE ID = :f1_ID<unsigned int>";
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
        WLOutDebug("删除客户端路径配置失败！");

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
作者：刘高舜 2014-10-11
**************************************************************************************************/
int CClientPathConfigMgt::update( const DWORD32 ID, TClientPathConfig *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

	int ret = isIDExist(ID);
	if(ret < 0)						return -3;

    char *sql = (char*)"UPDATE t_clientpathconfig set name=:f1<char[64]>,appType=:f2<unsigned int>,system=:f3<unsigned int>,systemVersion=:f4<unsigned int>,programVersion=:f5<unsigned int>,isForceUpdate=:f6<unsigned int>,path=:f7<char[255]>,isFile=:f8<unsigned int> where ID=:f9<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
        updateStream << pInputParam->name;
        updateStream << pInputParam->appType;
        updateStream << pInputParam->system;
        updateStream << pInputParam->systemVersion;
        updateStream << pInputParam->programVersion;
        updateStream << pInputParam->isForceUpdate;
        updateStream << pInputParam->path;
        updateStream << pInputParam->isFile;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;
	}
	catch (otl_exception& p)
	{
        WLOutDebug("更新客户端路径配置操作失败！");

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
作者：刘高舜 2014-10-11
**************************************************************************************************/
int CClientPathConfigMgt::add2List( TClientPathConfig *pInputParam )
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

    memcpy(&m_pList[m_count], pInputParam, sizeof(TClientPathConfig));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：刘高舜 2014-10-11
**************************************************************************************************/
int CClientPathConfigMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
        memcpy(&m_pList[i], &m_pList[i+1], sizeof(TClientPathConfig));
	}
    memset(&m_pList[m_count-1], 0x00, sizeof(TClientPathConfig));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：刘高舜 2014-10-11
**************************************************************************************************/
int CClientPathConfigMgt::isIDExist( const DWORD32 ID )
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
作者：刘高舜 2014-10-11
**************************************************************************************************/
int CClientPathConfigMgt::getMoreMemory()
{
    int sumCount = m_sumCount + MAX_CLIENTPATHCONFIG_COUNT;
    TClientPathConfig *pResource = new TClientPathConfig[sumCount];

	if(NULL == pResource)
	{
        WLOutDebug("getMoreMemory():CClientPathConfigMgt::CClientPathConfigMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
    memset(pResource, 0x00, sizeof(TClientPathConfig)*m_sumCount);
    memcpy(pResource, m_pList, sizeof(TClientPathConfig)*m_count);
	delete [] m_pList;

	m_pList = pResource;
	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：刘高舜 2014-10-11
**************************************************************************************************/
int CClientPathConfigMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：刘高舜 2014-10-11
**************************************************************************************************/
TClientPathConfig * CClientPathConfigMgt::getListByIndex( DWORD32 index ) const
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
作者：刘高舜 2014-10-11
**************************************************************************************************/
int CClientPathConfigMgt::updateList( const DWORD32 ID, const TClientPathConfig *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

    TClientPathConfig *temp = getListByIndex(index);
    memset(temp->name, 0x00, sizeof(temp->name));
    memcpy(temp->name, pInputParam->name, strlen(pInputParam->name));
    temp->appType = pInputParam->appType;
    temp->system = pInputParam->system;
    temp->systemVersion = pInputParam->systemVersion;
    temp->programVersion = pInputParam->programVersion;
    temp->isForceUpdate = pInputParam->isForceUpdate;
    memset(temp->path, 0x00, sizeof(temp->path));
    memcpy(temp->path, pInputParam->path, strlen(pInputParam->path));
    temp->isFile = pInputParam->isFile;

	return 0;
}

