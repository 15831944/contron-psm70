#include "stdafx.h"
#include "ForceDuiWeiMgt.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：刘高舜 2014-10-15
**************************************************************************************************/
CForceDuiWeiMgt::CForceDuiWeiMgt()
{
	m_count = 0;
    m_sumCount = MAX_FORCEDUIWEI_COUNT;

    m_pList = new TForceDuiWei[m_sumCount];
	if(NULL == m_pList)
	{
        WLOutDebug("CForceDuiWeiMgt::CForceDuiWeiMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：刘高舜 2014-10-15
**************************************************************************************************/
CForceDuiWeiMgt::~CForceDuiWeiMgt()
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
作者：刘高舜 2014-10-15
**************************************************************************************************/
int CForceDuiWeiMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TForceDuiWei));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_forceduiwei";

	try
	{
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

        TForceDuiWei temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
            selectStream >> temp.stationID;
			selectStream >> temp.deviceID;
            selectStream >> temp.staffID;
			selectStream >> temp.beforeState;
			selectStream >> temp.afterState;
            selectStream >> temp.time;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}
        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
        WLOutDebug("查找强制对位设备表操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：加载指定站ID列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：刘高舜 2014-10-15
**************************************************************************************************/
int CForceDuiWeiMgt::retrieveByStationID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TForceDuiWei));
	int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_forceduiwei where stationID=:f<unsigned int> ";

	try
	{
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream <<ID;

        TForceDuiWei temp = {0};
		while (!selectStream.eof())
		{
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.deviceID;
            selectStream >> temp.staffID;
            selectStream >> temp.beforeState;
            selectStream >> temp.afterState;
            selectStream >> temp.time;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}
        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
        WLOutDebug("查找强制对位设备表操作失败！");
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
作者：刘高舜 2014-10-15
**************************************************************************************************/
int CForceDuiWeiMgt::add( TForceDuiWei *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TForceDuiWei temp = {0};
    temp.stationID = pInputParam->stationID;
	temp.deviceID = pInputParam->deviceID;
    temp.staffID = pInputParam->staffID;
	temp.beforeState = pInputParam->beforeState;
	temp.afterState = pInputParam->afterState;
	memcpy(&temp.time, &pInputParam->time, sizeof(otl_datetime));

	int lastAddID;
	char operateTime[MAX_PARAM_LEN] = {0};
    sprintf(operateTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->time.year, pInputParam->time.month,pInputParam->time.day,pInputParam->time.hour,pInputParam->time.minute,pInputParam->time.second);

	char *sql = (char *)malloc(2000);
    sprintf(sql,"insert into t_forceduiwei values(NULL,:f1_stationID<unsigned int>,:f2_deviceID<unsigned int>,:f3_staffID<unsigned int>,:f4_bstate<unsigned int>,:f5_astate<unsigned int>,'%s')",operateTime);
	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
        insertStream << pInputParam->stationID;
		insertStream << pInputParam->deviceID;
        insertStream << pInputParam->staffID;
		insertStream << pInputParam->beforeState;
		insertStream << pInputParam->afterState;

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
		if(ret < 0)
		{
			free(sql);
			return -3;
		}
	}
	catch (otl_exception& p)
	{
        WLOutDebug("添加强制对位设备操作失败！");
		m_db.rollback();
		free(sql);

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	free(sql);

	return temp.ID;
}

/**************************************************************************************************
功能：删除记录
参数：ID，删除的记录的ID
返回：-2，数据库尚未连接
	  -3, 从列表中删除记录失败
	  -4， 数据库删除操作失败
	  0， 成功
作者：刘高舜 2014-10-15
**************************************************************************************************/
int CForceDuiWeiMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

    const char *sql = "DELETE FROM t_forceduiwei WHERE ID = :f1_ID<unsigned int>";
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
        WLOutDebug("删除强制对位设备失败！");

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
作者：刘高舜 2014-10-15
**************************************************************************************************/
int CForceDuiWeiMgt::update( const DWORD32 ID, TForceDuiWei *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

	char operateTime[MAX_PARAM_LEN] = {0};
    sprintf(operateTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->time.year, pInputParam->time.month,pInputParam->time.day,pInputParam->time.hour,pInputParam->time.minute,pInputParam->time.second);

	char *sql = (char *)malloc(2000);
    sprintf(sql,"UPDATE t_forceduiwei set stationID=:f1_stationID<unsigned int>,deviceID=:f2_devID<unsigned int>,staffID=:f3_staffID<unsigned int>,beforeState=:f4_bstate<unsigned int>,afterState=:f5_astate<unsigned int>,time='%s' where ID=:f7_id<unsigned int>",operateTime);
	try
	{
		otl_stream updateStream(1, sql, m_db);
        updateStream << pInputParam->stationID;
		updateStream << pInputParam->deviceID;
        updateStream << pInputParam->staffID;
		updateStream << pInputParam->beforeState;
		updateStream << pInputParam->afterState;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;

	}
	catch (otl_exception& p)
	{
        WLOutDebug("更新强制对位设备操作失败！");
		free(sql);

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	free(sql);

	return 0;
}

/**************************************************************************************************
功能：添加记录到列表
参数：pInputParam，待添加的记录
返回：-1，参数错误
	  -2， 重复记录
	  -3， 申请内存失败
	  0， 成功
作者：刘高舜 2014-10-15
**************************************************************************************************/
int CForceDuiWeiMgt::add2List( TForceDuiWei *pInputParam )
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

    memcpy(&m_pList[m_count], pInputParam, sizeof(TForceDuiWei));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：刘高舜 2014-10-15
**************************************************************************************************/
int CForceDuiWeiMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
        memcpy(&m_pList[i], &m_pList[i+1], sizeof(TForceDuiWei));
	}
    memset(&m_pList[m_count-1], 0x00, sizeof(TForceDuiWei));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：刘高舜 2014-10-15
**************************************************************************************************/
int CForceDuiWeiMgt::isIDExist( const DWORD32 ID )
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
作者：刘高舜 2014-10-15
**************************************************************************************************/
int CForceDuiWeiMgt::getMoreMemory()
{
    int sumCount = m_sumCount + MAX_FORCEDUIWEI_COUNT;
    TForceDuiWei *pResource = new TForceDuiWei[sumCount];

	if(NULL == pResource)
	{
        WLOutDebug("getMoreMemory():CForceDuiWeiMgt::CForceDuiWeiMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
    memset(pResource, 0x00, sizeof(TForceDuiWei)*m_sumCount);
    memcpy(pResource, m_pList, sizeof(TForceDuiWei)*m_count);
	delete [] m_pList;

	m_pList = pResource;
	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：刘高舜 2014-07-29
**************************************************************************************************/
int CForceDuiWeiMgt::getListCount()
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：刘高舜 2014-07-29
**************************************************************************************************/
TForceDuiWei * CForceDuiWeiMgt::getListByIndex( DWORD32 index )
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
作者：刘高舜 2014-07-30
**************************************************************************************************/
int CForceDuiWeiMgt::updateList( const DWORD32 ID, const TForceDuiWei *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

    TForceDuiWei *temp = getListByIndex(index);
    temp->stationID = pInputParam->stationID;
	temp->deviceID = pInputParam->deviceID;
    temp->staffID = pInputParam->staffID;
	temp->beforeState = pInputParam->beforeState;
	temp->afterState = pInputParam->afterState;
	memset(&temp->time, 0x00, sizeof(otl_datetime));
	memcpy(&temp->time, &pInputParam->time, sizeof(otl_datetime));
	return 0;
}


