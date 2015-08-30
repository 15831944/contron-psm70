#include "stdafx.h"
#include "LockMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-23
**************************************************************************************************/
CLockMgt::CLockMgt()
{
	m_count = 0;
	m_sumCount = MAX_LOCK_COUNT;

	m_pList = new TLock[m_sumCount];
	if(NULL == m_pList)
	{
		WLOutDebug("CLockMgt::CLockMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-23
**************************************************************************************************/
CLockMgt::~CLockMgt()
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
作者：李施施 2014-07-23
**************************************************************************************************/
int CLockMgt::retrieve( )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TLock));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_lock  ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

		TLock temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.lockType;
			selectStream >> temp.lockCodeType;
			selectStream >> temp.lockCode;
			selectStream >> temp.rfidCode;
			selectStream >> temp.deviceID;
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
功能：加载指定ID下的列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：刘高舜 2014-0-29
**************************************************************************************************/
int CLockMgt::retrieveByDeviceID(const DWORD32 ID)
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TLock));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_lock where deviceID=:f_ID<unsigned int> ";

	try
	{
        m_db.commit();
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
        selectStream << ID;

		TLock temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.lockType;
			selectStream >> temp.lockCodeType;
			selectStream >> temp.lockCode;
			selectStream >> temp.rfidCode;
			selectStream >> temp.deviceID;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找锁具表操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：按照设备ID排序加载锁具列表
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：佘佳明 2014-9-29
**************************************************************************************************/
int CLockMgt::retrieveOrderByDeviceID()
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TLock));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_lock ORDER BY deviceID";

    try
    {
        m_db.commit();
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);

        TLock temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.lockType;
            selectStream >> temp.lockCodeType;
            selectStream >> temp.lockCode;
            selectStream >> temp.rfidCode;
            selectStream >> temp.deviceID;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找锁具表操作失败！");
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
作者：李施施 2014-07-23
**************************************************************************************************/
int CLockMgt::add( TLock *pInputParam )
{	
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TLock temp = {0};
	temp.lockType = pInputParam->lockType;
	temp.lockCodeType = pInputParam->lockCodeType;
	temp.lockCode = pInputParam->lockCode;
	memcpy(&temp.rfidCode, pInputParam->rfidCode,strlen(pInputParam->rfidCode));
	temp.deviceID = pInputParam->deviceID;

	int lastAddID;
	const char *sql = "insert into t_lock values(NULL,:f1<unsigned int>,:f2<unsigned int>,:f3<unsigned int>,:f4<char[32]>,:f5<unsigned int>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->lockType;
		insertStream << pInputParam->lockCodeType;
		insertStream << pInputParam->lockCode;
		insertStream << pInputParam->rfidCode;
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
作者：李施施 2014-07-23
**************************************************************************************************/
int CLockMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

	const char *sql = "DELETE FROM t_lock WHERE ID = :f1_ID<unsigned int>";
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
功能：删除指定设备ID的记录
参数：deviceID，设备ID
返回：-2，数据库尚未连接
      -3, 从列表中删除记录失败
      -4， 数据库删除操作失败
      0， 成功
注意：记录删除后没有更新本地的记录列表m_pList
作者：刘远安 2014-07-23
**************************************************************************************************/
int CLockMgt::delByDeviceID(const DWORD32 deviceID)
{
    if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    const char *sql = "DELETE FROM t_lock WHERE deviceID = :f1_ID<unsigned int>";
    try
    {
        m_db.commit();
        otl_stream delStream(1, sql, m_db);
        delStream << deviceID;

        m_db.commit();
        printf("数据库删除记录 deviceID: %d\n", deviceID);
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
作者：李施施 2014-07-23
**************************************************************************************************/
int CLockMgt::update( const DWORD32 ID, TLock *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

    const char *sql = "UPDATE t_lock set lockType=:f1<unsigned int>,lockCodeType=:f2<unsigned int>,lockCode=:f3<unsigned int>,rfidCode=:f4<char[32]>,deviceID=:f5<unsigned int> where ID=:f6<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
		updateStream << pInputParam->lockType;
		updateStream << pInputParam->lockCodeType;
		updateStream << pInputParam->lockCode;
		updateStream << pInputParam->rfidCode;
		updateStream << pInputParam->deviceID;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;
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
作者：李施施 2014-07-23
**************************************************************************************************/
int CLockMgt::add2List( TLock *pInputParam )
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

	memcpy(&m_pList[m_count], pInputParam, sizeof(TLock));
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
int CLockMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pList[i], &m_pList[i+1], sizeof(TLock));
	}
	memset(&m_pList[m_count-1], 0x00, sizeof(TLock));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：李施施 2014-07-23
**************************************************************************************************/
int CLockMgt::isIDExist( const DWORD32 ID )
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
作者：李施施 2014-07-23
**************************************************************************************************/
int CLockMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_LOCK_COUNT;
	TLock *pResource = new TLock[sumCount];

	if(NULL == pResource)
	{
		WLOutDebug("getMoreMemory():TLock::TLock()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TLock)*m_sumCount);
	memcpy(pResource, m_pList, sizeof(TLock)*m_count);
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
int CLockMgt::getListCount() const
{
    return m_count;
}

/**************************************************************************************************
功能：对相同的设备设置相同锁码
参数：deviceID 设备ID
     lockCode 锁码
返回：>=0, 成功
     <0,  失败
作者：刘高舜 2015-03-12
**************************************************************************************************/
int CLockMgt::updateLockCodeByDeviceID(DWORD32 deviceID, DWORD32 lockCode)
{
    if(!m_isInit)					return -1;

    if( checkDBConnect() < 0 )      return -2;

    const char *sql = "UPDATE t_lock set lockCode=:f1<unsigned int> where deviceID=:f2<unsigned int>";

    try
    {
        otl_stream updateStream(1, sql, m_db);
        updateStream << lockCode;
        updateStream << deviceID;

        m_db.commit();
        int ret = updateListOfLockCode(deviceID,lockCode);
        if(ret < 0)			return -3;
    }
    catch (otl_exception& p)
    {
        WLOutDebug("更新锁具表操作失败！");

        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TLock * CLockMgt::getListByIndex( DWORD32 index ) const
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
int CLockMgt::updateList( const DWORD32 ID, const TLock *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TLock *temp = getListByIndex(index);
	temp->lockType = pInputParam->lockType;
	temp->lockCodeType = pInputParam->lockCodeType;
	temp->lockCode = pInputParam->lockCode;
	memset(temp->rfidCode, 0x00, sizeof(temp->rfidCode));
	memcpy(temp->rfidCode, pInputParam->rfidCode, strlen(pInputParam->rfidCode));
	temp->deviceID = pInputParam->deviceID;

    return 0;
}

/**************************************************************************************************
功能：锁码更新时,更新本地列表记录
参数：deviceID,设备ID
     lockCode,锁码
返回：-1,参数错误
      -2，记录不存在
      0，成功
作者：李施施 2014-07-30
**************************************************************************************************/
int CLockMgt::updateListOfLockCode(const DWORD32 deviceID, const DWORD32 lockCode)
{
    for ( DWORD32 i = 0; i < m_count; i++ )
    {
        TLock *temp = getListByIndex(i);
        if ( temp->deviceID == deviceID )
        {
            temp->lockCode = lockCode;
        }
    }
    return 0;
}


