#include "stdafx.h"
#include "OperateTermMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-23
**************************************************************************************************/
COperateTermMgt::COperateTermMgt()
{
	m_count = 0;
	m_sumCount = MAX_OPERATETERM_COUNT;

	m_pList = new TOperateTerm[m_sumCount];
	if(NULL == m_pList)
	{
		WLOutDebug("COperateTermMgt::COperateTermMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-23
**************************************************************************************************/
COperateTermMgt::~COperateTermMgt()
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
**************************************************************************************************/
int COperateTermMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TOperateTerm));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_operateterm ORDER BY `index`";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

		TOperateTerm temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.operateDescriptionID;
			selectStream >> temp.operateType;
			selectStream >> temp.operateTerm;
			selectStream >> temp.conditions;
			selectStream >> temp.itemShipType;
			selectStream >> temp.isPrint;
			selectStream >> temp.index;
			selectStream >> temp.isBelongStation;
			selectStream >> temp.belongStationID;
			selectStream >> temp.isBelongDevice;
			selectStream >> temp.belongDeviceID;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找常用术语操作失败！");
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
作者：李施施 2014-07-22
**************************************************************************************************/
int COperateTermMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TOperateTerm));
	int loadCount = 0;

    const char *sql = "SELECT * FROM t_operateterm where operateDescriptionID=:f<unsigned int> ORDER BY `index`";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TOperateTerm temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.operateDescriptionID;
			selectStream >> temp.operateType;
			selectStream >> temp.operateTerm;
			selectStream >> temp.conditions;
			selectStream >> temp.itemShipType;
			selectStream >> temp.isPrint;
			selectStream >> temp.index;
			selectStream >> temp.isBelongStation;
			selectStream >> temp.belongStationID;
			selectStream >> temp.isBelongDevice;
			selectStream >> temp.belongDeviceID;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找常用术语操作失败！");
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
作者：刘高舜 2014-08-29
**************************************************************************************************/
int COperateTermMgt::retrieveByOperateDescriptionID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TOperateTerm));
	int loadCount = 0;

    const char *sql = "SELECT * FROM t_operateterm where operateDescriptionID=:f<unsigned int> ORDER BY `index`";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TOperateTerm temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.operateDescriptionID;
			selectStream >> temp.operateType;
			selectStream >> temp.operateTerm;
			selectStream >> temp.conditions;
			selectStream >> temp.itemShipType;
			selectStream >> temp.isPrint;
			selectStream >> temp.index;
			selectStream >> temp.isBelongStation;
			selectStream >> temp.belongStationID;
			selectStream >> temp.isBelongDevice;
			selectStream >> temp.belongDeviceID;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找操作术语表失败！");
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
int COperateTermMgt::add( TOperateTerm *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TOperateTerm temp = {0};
	temp.operateDescriptionID = pInputParam->operateDescriptionID;
	temp.operateType= pInputParam->operateType;
	memcpy(&temp.operateTerm, pInputParam->operateTerm, strlen(pInputParam->operateTerm));
	memcpy(&temp.conditions, pInputParam->conditions, strlen(pInputParam->conditions));
	temp.itemShipType = pInputParam->itemShipType;
	temp.isPrint = pInputParam->isPrint;
	temp.index = pInputParam->index;
	temp.isBelongStation = pInputParam->isBelongStation;
	temp.belongStationID = pInputParam->belongStationID;
	temp.isBelongDevice = pInputParam->isBelongDevice;
	temp.belongDeviceID = pInputParam->belongDeviceID;

	int lastAddID;
	const char *sql = "insert into t_operateterm values(NULL,:f1<unsigned int>,:f2<unsigned int>,:f3<char[255]>,:f4<char[128]>,:f5<unsigned int>,:f6<unsigned int>,:f7<unsigned int>,:f8<unsigned int>,:f9<unsigned int>,:f10<unsigned int>,:f11<unsigned int>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->operateDescriptionID;
		insertStream << pInputParam->operateType;
		insertStream << pInputParam->operateTerm;
		insertStream << pInputParam->conditions;
		insertStream << pInputParam->itemShipType;
		insertStream << pInputParam->isPrint;
		insertStream << pInputParam->index;
		insertStream << pInputParam->isBelongStation;
		insertStream << pInputParam->belongStationID;
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
		WLOutDebug("添加常用术语操作失败！");
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
int COperateTermMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

	const char *sql = "DELETE FROM t_operateterm WHERE ID = :f1_ID<unsigned int>";
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
		WLOutDebug("删除常用术语操作失败！");

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：删除操作描述对应的术语记录
参数：optDescID 操作描述ID
返回：-2，数据库尚未连接
      -3, 从列表中删除记录失败
      -4， 数据库删除操作失败
      0， 成功
作者：佘佳明 2015-01-16
**************************************************************************************************/
int COperateTermMgt::delByOptDescID(const DWORD32 optDescID)
{
    if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    const char *sql = "DELETE FROM t_operateterm WHERE operateDescriptionID = :f1_ID<unsigned int>";
    try
    {
        otl_stream delStream(1, sql, m_db);
        delStream << optDescID;

        printf("数据库删除记录 ID: %d\n", optDescID);

        m_db.commit();
    }
    catch (otl_exception& p)
    {
        WLOutDebug("删除常用术语操作失败！");

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
int COperateTermMgt::update( const DWORD32 ID, TOperateTerm *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

	const char *sql = "UPDATE t_operateterm SET operateDescriptionID=:f1<unsigned int>,operateType=:f2<unsigned int>,operateTerm=:f3<char[255]>,conditions=:f4<char[128]>,itemShipType=:f5<unsigned int>,isPrint=:f6<unsigned int>,`index`=:f7<unsigned int>,isBelongStation=:f8<unsigned int>,belongStationID=:f9<unsigned int>,isBelongDevice=:f10<unsigned int>,belongDeviceID=:f11<unsigned int> where ID=:f12<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
		updateStream << pInputParam->operateDescriptionID;
		updateStream << pInputParam->operateType;
		updateStream << pInputParam->operateTerm;
		updateStream << pInputParam->conditions;
		updateStream << pInputParam->itemShipType;
		updateStream << pInputParam->isPrint;
		updateStream << pInputParam->index;
		updateStream << pInputParam->isBelongStation;
		updateStream << pInputParam->belongStationID;
		updateStream << pInputParam->isBelongDevice;
		updateStream << pInputParam->belongDeviceID;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;

	}
	catch (otl_exception& p)
	{
		WLOutDebug("更新常用术语操作失败！");

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
作者：李施施 2014-07-22
**************************************************************************************************/
int COperateTermMgt::add2List( TOperateTerm *pInputParam )
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

	memcpy(&m_pList[m_count], pInputParam, sizeof(TOperateTerm));
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
int COperateTermMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pList[i], &m_pList[i+1], sizeof(TOperateTerm));
	}
	memset(&m_pList[m_count-1], 0x00, sizeof(TOperateTerm));
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
int COperateTermMgt::isIDExist( const DWORD32 ID )
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
int COperateTermMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_OPERATETERM_COUNT;
	TOperateTerm *pResource = new TOperateTerm[sumCount];

	if(NULL == pResource)
	{
		WLOutDebug("getMoreMemory():COperateTermMgt::CCommonTermMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TOperateTerm)*m_sumCount);
	memcpy(pResource, m_pList, sizeof(TOperateTerm)*m_count);
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
int COperateTermMgt::getListCount()
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TOperateTerm * COperateTermMgt::getListByIndex( DWORD32 index )
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
int COperateTermMgt::updateList( const DWORD32 ID, const TOperateTerm *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TOperateTerm *temp = getListByIndex(index);
	temp->operateDescriptionID = pInputParam->operateDescriptionID;
	temp->operateType = pInputParam->operateType;
	memset(temp->operateTerm, 0x00, sizeof(temp->operateTerm));
	memcpy(temp->operateTerm, pInputParam->operateTerm, strlen(pInputParam->operateTerm));
	memset(temp->conditions, 0x00, sizeof(temp->conditions));
	memcpy(temp->conditions, pInputParam->conditions, strlen(pInputParam->conditions));
	temp->itemShipType = pInputParam->itemShipType;
	temp->isPrint = pInputParam->isPrint;
	temp->index = pInputParam->index;
	temp->isBelongStation = pInputParam->isBelongStation;
	temp->belongStationID = pInputParam->belongStationID;
	temp->isBelongDevice = pInputParam->isBelongDevice;
	temp->belongDeviceID = pInputParam->belongDeviceID;

	return 0;
}


