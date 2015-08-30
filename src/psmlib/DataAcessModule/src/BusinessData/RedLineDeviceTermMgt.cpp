#include "stdafx.h"
#include "RedLineDeviceTermMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-22
**************************************************************************************************/
CRedLineDeviceTermMgt::CRedLineDeviceTermMgt()
{
	m_count = 0;
	m_sumCount = MAX_REDLINEDEVICETERM_COUNT;

	m_pList = new TRedLineDeviceTerm[m_sumCount];
	if(NULL == m_pList)
	{
		WLOutDebug("CRedLineDeviceTermMgt::CRedLineDeviceTermMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-22
**************************************************************************************************/
CRedLineDeviceTermMgt::~CRedLineDeviceTermMgt()
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
作者：李施施 2014-07-22
修改：刘高舜 2014-11-03
**************************************************************************************************/
int CRedLineDeviceTermMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TRedLineDeviceTerm));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_redlinedeviceterm ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

		TRedLineDeviceTerm temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.beforeState;
			selectStream >> temp.afterState;
			selectStream >> temp.term;
			selectStream >> temp.isPrint;
			selectStream >> temp.isBelongDevice;
			selectStream >> temp.belongDeviceID;
            selectStream >> temp.isUse;
			loadCount++;

            printf("Load WarningBoard<<ID: %d,stationID:%d,beforeState: %d,afterState:%d,term:%s,isPrint:%d,isBelongDevice: %d,belongDeviceID:%d,isUse:%d\n", temp.ID, temp.stationID, temp.beforeState,temp.afterState, temp.term, temp.isPrint,temp.isBelongDevice, temp.belongDeviceID, temp.isUse);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找红线设备操作失败！");
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
参数：ID，部门ID
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-22
修改：刘高舜 2014-11-03
**************************************************************************************************/
int CRedLineDeviceTermMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TRedLineDeviceTerm));
	int loadCount = 0;
    const char *sql = "SELECT * FROM t_redlinedeviceterm where stationID=:f<unsigned int>";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TRedLineDeviceTerm temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.beforeState;
			selectStream >> temp.afterState;
			selectStream >> temp.term;
			selectStream >> temp.isPrint;
			selectStream >> temp.isBelongDevice;
			selectStream >> temp.belongDeviceID;
            selectStream >> temp.isUse;
			loadCount++;

            printf("Load RedLineDevice<<ID: %d,stationID:%d,beforeState: %d,afterState:%d,term:%s,isPrint:%d,isBelongDevice: %d,belongDeviceID:%d,isUse:%d\n", temp.ID, temp.stationID, temp.beforeState,temp.afterState, temp.term, temp.isPrint,temp.isBelongDevice, temp.belongDeviceID, temp.isUse);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找红线设备操作失败！");
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
参数：ID，部门ID
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：刘高舜 2014-08-29
修改：刘高舜 2014-11-03
**************************************************************************************************/
int CRedLineDeviceTermMgt::retrieveByStationID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TRedLineDeviceTerm));
	int loadCount = 0;
    const char *sql = "SELECT * FROM t_redlinedeviceterm where stationID=:f<unsigned int>";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TRedLineDeviceTerm temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.beforeState;
			selectStream >> temp.afterState;
			selectStream >> temp.term;
			selectStream >> temp.isPrint;
			selectStream >> temp.isBelongDevice;
			selectStream >> temp.belongDeviceID;
            selectStream >> temp.isUse;
			loadCount++;

            printf("Load RedLineDevice<<ID: %d,stationID:%d,beforeState: %d,afterState:%d,term:%s,isPrint:%d,isBelongDevice: %d,belongDeviceID:%d,isUse:%d\n", temp.ID, temp.stationID, temp.beforeState,temp.afterState, temp.term, temp.isPrint,temp.isBelongDevice, temp.belongDeviceID, temp.isUse);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找红线设备术语表失败！");
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
修改：刘高舜 2014-11-03
**************************************************************************************************/
int CRedLineDeviceTermMgt::add( TRedLineDeviceTerm *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TRedLineDeviceTerm temp = {0};
	temp.stationID= pInputParam->stationID;
	memcpy(&temp.term, pInputParam->term, strlen(pInputParam->term));
	temp.beforeState = pInputParam->beforeState;
	temp.afterState = pInputParam->afterState;
	temp.isPrint = pInputParam->isPrint;
	temp.isBelongDevice = pInputParam->isBelongDevice;
	temp.belongDeviceID = pInputParam->belongDeviceID;
    temp.isUse = pInputParam->isUse;

	int lastAddID;
    const char *sql = "insert into t_redlinedeviceterm values(NULL,:f1<unsigned int>,:f2<unsigned int>,:f3<unsigned int>,:f4<char[64]>,:f5<unsigned int>,:f6<unsigned int>,:f7<unsigned int>,:f8<unsigned int>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->stationID;
		insertStream << pInputParam->beforeState;
		insertStream << pInputParam->afterState;
		insertStream << pInputParam->term;
		insertStream << pInputParam->isPrint;
		insertStream << pInputParam->isBelongDevice;
		insertStream << pInputParam->belongDeviceID;
        insertStream << pInputParam->isUse;

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

        printf("数据库插入记录ID: %d,stationID:%d,beforeState: %d,afterState:%d,term:%s,isPrint:%d,isBelongDevice: %d,belongDeviceID:%d,isUse:%d\n", temp.ID, temp.stationID, temp.beforeState,temp.afterState, temp.term, temp.isPrint,temp.isBelongDevice, temp.belongDeviceID, temp.isUse);
		int ret = add2List(&temp);
		if(ret < 0)		return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("添加红线设备操作失败！");
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
int CRedLineDeviceTermMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

	const char *sql = "DELETE FROM t_redlinedeviceterm WHERE ID = :f1_ID<unsigned int>";
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
		WLOutDebug("删除标识牌操作失败！");

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
修改：刘高舜 2014-11-03
**************************************************************************************************/
int CRedLineDeviceTermMgt::update( const DWORD32 ID, TRedLineDeviceTerm *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

    const char *sql = "UPDATE t_redlinedeviceterm SET stationID=:f1<unsigned int> ,beforeState=:f2<unsigned int>, afterState=:f3<unsigned int>, term=:f4<char[64]>,isPrint=:f5<unsigned int>,isBelongDevice=:f6<unsigned int>,belongDeviceID=:f7<unsigned int>,isUse=:f8<unsigned int> WHERE ID=:f9<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
		updateStream << pInputParam->stationID;
		updateStream << pInputParam->beforeState;
		updateStream << pInputParam->afterState;
		updateStream << pInputParam->term;
		updateStream << pInputParam->isPrint;
		updateStream << pInputParam->isBelongDevice;
		updateStream << pInputParam->belongDeviceID;
        updateStream << pInputParam->isUse;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;

        printf("数据库更新记录ID: %d,stationID:%d,beforeState: %d,afterState:%d,term:%s,isPrint:%d,isBelongDevice: %d,belongDeviceID:%d,isUse:%d\n", pInputParam->ID, pInputParam->stationID, pInputParam->beforeState,pInputParam->afterState, pInputParam->term, pInputParam->isPrint,pInputParam->isBelongDevice, pInputParam->belongDeviceID, pInputParam->isUse);

	}
	catch (otl_exception& p)
	{
		WLOutDebug("更新标志牌操作失败！");

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
int CRedLineDeviceTermMgt::add2List( TRedLineDeviceTerm *pInputParam )
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

	memcpy(&m_pList[m_count], pInputParam, sizeof(TRedLineDeviceTerm));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：李施施 2014-07-22
**************************************************************************************************/
int CRedLineDeviceTermMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pList[i], &m_pList[i+1], sizeof(TRedLineDeviceTerm));
	}
	memset(&m_pList[m_count-1], 0x00, sizeof(TRedLineDeviceTerm));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：李施施 2014-07-22
**************************************************************************************************/
int CRedLineDeviceTermMgt::isIDExist( const DWORD32 ID )
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
作者：李施施 2014-07-22
**************************************************************************************************/
int CRedLineDeviceTermMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_REDLINEDEVICETERM_COUNT;
	TRedLineDeviceTerm *pResource = new TRedLineDeviceTerm[sumCount];

	if(NULL == pResource)
	{
		WLOutDebug("getMoreMemory():CRedLineDeviceTermMgt::CRedLineDeviceTermMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TRedLineDeviceTerm)*m_sumCount);
	memcpy(pResource, m_pList, sizeof(TRedLineDeviceTerm)*m_count);
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
int CRedLineDeviceTermMgt::getListCount()
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TRedLineDeviceTerm * CRedLineDeviceTermMgt::getListByIndex( DWORD32 index )
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
修改：刘高舜 2014-11-03
**************************************************************************************************/
int CRedLineDeviceTermMgt::updateList( const DWORD32 ID, const TRedLineDeviceTerm *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TRedLineDeviceTerm *temp = getListByIndex(index);
	temp->stationID = pInputParam->stationID;
	temp->beforeState = pInputParam->beforeState;
	temp->afterState = pInputParam->afterState;
	memset(temp->term, 0x00, sizeof(temp->term));
	memcpy(temp->term, pInputParam->term, strlen(pInputParam->term));
	temp->isPrint = pInputParam->isPrint;
	temp->isBelongDevice = pInputParam->isBelongDevice;
	temp->belongDeviceID = pInputParam->belongDeviceID;
    temp->isUse = pInputParam->isUse;

	return 0;
}

