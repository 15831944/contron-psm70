#include "stdafx.h"
#include "RecordPathConfigMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：刘高舜 2014-10-13
**************************************************************************************************/
CRecordPathConfigMgt::CRecordPathConfigMgt()
{
	m_count = 0;
    m_sumCount = MAX_RECORDPATHCONFIGMGT_COUNT;

    m_pList = new TRecordPathConfig[m_sumCount];
	if(NULL == m_pList)
	{
        WLOutDebug("CRecordPathConfigMgt::CRecordPathConfigMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：刘高舜 2014-10-13
**************************************************************************************************/
CRecordPathConfigMgt::~CRecordPathConfigMgt()
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
作者：刘高舜 2014-10-13
**************************************************************************************************/
int CRecordPathConfigMgt::retrieve()
{
	if(!m_isInit)		return -2;

    memset(m_pList, 0x00, m_sumCount * sizeof(TRecordPathConfig));
	int loadCount = 0;
	m_count = 0;

    char *sql = (char*)"SELECT * FROM t_recordpathconfig ";

	try
	{
        m_db.commit();
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

        TRecordPathConfig temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
            selectStream >> temp.number;
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
        WLOutDebug("查找设备点表文件路径配置失败！");
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
int CRecordPathConfigMgt::retrieveByStationID(DWORD32 stationID)
{
    if(!m_isInit)		return -2;

    memset(m_pList, 0x00, m_sumCount * sizeof(TRecordPathConfig));
    int loadCount = 0;
    m_count = 0;

    char *sql = (char*)"SELECT * FROM t_recordpathconfig where number=:f1<unsigned int>";

    try
    {
        m_db.commit();
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << stationID;

        TRecordPathConfig temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.number;
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
        WLOutDebug("查找设备点表文件路径配置失败！");
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
作者：刘高舜 2014-10-13
**************************************************************************************************/
int CRecordPathConfigMgt::add( TRecordPathConfig *pInputParam )
{
	if(!m_isInit)	return -2;

    TRecordPathConfig temp = {0};
    temp.number = pInputParam->number;
    memcpy(&temp.path, pInputParam->path,strlen(pInputParam->path));
    temp.isFile = pInputParam->isFile;

	int lastAddID;
    char *sql = (char*)"insert into t_recordpathconfig values(NULL,:f1<unsigned int>,:f2<char[255]>,:f3<unsigned int>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
        insertStream << pInputParam->number;
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
        WLOutDebug("添加设备点表文件路径配置失败！");
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
作者：刘高舜 2014-10-13
**************************************************************************************************/
int CRecordPathConfigMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

	int ret = isIDExist(ID);
	if(ret < 0)		return -5;

    char *sql = (char*)"DELETE FROM t_recordpathconfig WHERE ID = :f1_ID<unsigned int>";
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
        WLOutDebug("删除设备点表文件路径配置失败！");

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
作者：刘高舜 2014-07-23
**************************************************************************************************/
int CRecordPathConfigMgt::update( const DWORD32 ID, TRecordPathConfig *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

	int ret = isIDExist(ID);
	if(ret < 0)						return -3;

    char *sql = (char*)"UPDATE t_recordpathconfig set number=:f1<unsigned int>,path=:f2<char[255]>,isFile=:f3<unsigned int> where ID=:f4<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
        updateStream << pInputParam->number;
        updateStream << pInputParam->path;
        updateStream << pInputParam->isFile;
		updateStream << ID;

        m_db.commit();

		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;
	}
	catch (otl_exception& p)
	{
        WLOutDebug("更新设备点表文件路径配置操作失败！");

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
作者：刘高舜 2014-10-13
**************************************************************************************************/
int CRecordPathConfigMgt::add2List( TRecordPathConfig *pInputParam )
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

    memcpy(&m_pList[m_count], pInputParam, sizeof(TRecordPathConfig));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：刘高舜 2014-10-13
**************************************************************************************************/
int CRecordPathConfigMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
        memcpy(&m_pList[i], &m_pList[i+1], sizeof(TRecordPathConfig));
	}
    memset(&m_pList[m_count-1], 0x00, sizeof(TRecordPathConfig));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：刘高舜 2014-10-13
**************************************************************************************************/
int CRecordPathConfigMgt::isIDExist( const DWORD32 ID )
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
作者：刘高舜 2014-10-13
**************************************************************************************************/
int CRecordPathConfigMgt::getMoreMemory()
{
    int sumCount = m_sumCount + MAX_RECORDPATHCONFIGMGT_COUNT;
    TRecordPathConfig *pResource = new TRecordPathConfig[sumCount];

	if(NULL == pResource)
	{
        WLOutDebug("getMoreMemory():CRecordPathConfigMgt::CRecordPathConfigMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
    memset(pResource, 0x00, sizeof(TRecordPathConfig)*m_sumCount);
    memcpy(pResource, m_pList, sizeof(TRecordPathConfig)*m_count);
	delete [] m_pList;

	m_pList = pResource;
	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：刘高舜 2014-10-13
**************************************************************************************************/
int CRecordPathConfigMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：刘高舜 2014-10-13
**************************************************************************************************/
TRecordPathConfig * CRecordPathConfigMgt::getListByIndex( DWORD32 index ) const
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
作者：刘高舜 2014-10-13
**************************************************************************************************/
int CRecordPathConfigMgt::updateList( const DWORD32 ID, const TRecordPathConfig *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

    TRecordPathConfig *temp = getListByIndex(index);
    temp->number = pInputParam->number;
    memset(temp->path, 0x00, sizeof(temp->path));
    memcpy(temp->path, pInputParam->path, strlen(pInputParam->path));
    temp->isFile = pInputParam->isFile;

	return 0;
}

