#include "stdafx.h"
#include "WarningBoardMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-22
**************************************************************************************************/
CWarningBoardMgt::CWarningBoardMgt()
{
	m_count = 0;
	m_sumCount = MAX_WARNINGBOARD_COUNT;

	m_pList = new TWarningBoard[m_sumCount];
	if(NULL == m_pList)
	{
		WLOutDebug("CWarningBoardMgt::CWarningBoardMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-22
**************************************************************************************************/
CWarningBoardMgt::~CWarningBoardMgt()
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
int CWarningBoardMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TWarningBoard));
	int loadCount = 0;
	m_count = 0;

    char *sql = (char*)"SELECT * FROM t_warningboard ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

		TWarningBoard temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.name;
			selectStream >> temp.width;
			selectStream >> temp.height;
			selectStream >> temp.fontColor;
			selectStream >> temp.backgoundColor;
			selectStream >> temp.isBelongDevice;
			selectStream >> temp.belongDeviceID;
			loadCount++;

			printf("Load WarningBoard<<ID: %d,voltageValue:%d,name: %s,width:%d,height: %d,fontColor:%d,backgoundColor:%d,isBelongDevice: %d,belongDeviceID:%d\n", temp.ID, temp.stationID, temp.name,temp.width, temp.height, temp.fontColor,temp.isBelongDevice, temp.belongDeviceID);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找标志板操作失败！");
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
**************************************************************************************************/
int CWarningBoardMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TWarningBoard));
	int loadCount = 0;

    char *sql = (char*)"SELECT * FROM t_warningboard where stationID=:f1<unsigned int> OR stationID='0'";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TWarningBoard temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.name;
			selectStream >> temp.width;
			selectStream >> temp.height;
			selectStream >> temp.fontColor;
			selectStream >> temp.backgoundColor;
			selectStream >> temp.isBelongDevice;
			selectStream >> temp.belongDeviceID;
			loadCount++;

			printf("Load WarningBoard<<ID: %d,voltageValue:%d,name: %s,width:%d,height: %d,fontColor:%d,backgoundColor:%d,isBelongDevice: %d,belongDeviceID:%d\n", temp.ID, temp.stationID, temp.name,temp.width, temp.height, temp.fontColor,temp.isBelongDevice, temp.belongDeviceID);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找标志板操作失败！");
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
int CWarningBoardMgt::add( TWarningBoard *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TWarningBoard temp = {0};
	temp.stationID= pInputParam->stationID;
	memcpy(&temp.name, pInputParam->name, strlen(pInputParam->name));
	temp.width = pInputParam->width;
	temp.height = pInputParam->height;
	temp.fontColor = pInputParam->fontColor;
	temp.backgoundColor = pInputParam->backgoundColor;
	temp.isBelongDevice = pInputParam->isBelongDevice;
	temp.belongDeviceID = pInputParam->belongDeviceID;

	int lastAddID;
	char *sql = (char*)"insert into t_warningboard values(NULL,:f1<unsigned int>,:f2<char[64]>,:f3<unsigned int>,:f4<unsigned int>,:f5<unsigned int>,:f6<unsigned int>,:f7<unsigned int>,:f8<unsigned int>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->stationID;
		insertStream << pInputParam->name;
		insertStream << pInputParam->width;
		insertStream << pInputParam->height;
		insertStream << pInputParam->fontColor;
		insertStream << pInputParam->backgoundColor;
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

		printf("数据库插入记录ID: %d,voltageValue:%d,name: %s,width:%d,height: %d,fontColor:%d,backgoundColor:%d,isBelongDevice: %d,belongDeviceID:%d\n", temp.ID, temp.stationID, temp.name,temp.width, temp.height, temp.fontColor,temp.isBelongDevice, temp.belongDeviceID);
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
int CWarningBoardMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

	char *sql = (char*)"DELETE FROM t_warningboard WHERE ID = :f1_ID<unsigned int>";
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
**************************************************************************************************/
int CWarningBoardMgt::update( const DWORD32 ID, TWarningBoard *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

	char *sql = (char*)"UPDATE t_warningboard SET stationID=:f1<unsigned int> ,name=:f2<char[64]>, width=:f3<unsigned int>, height=:f4<unsigned int>,fontColor=:f5<unsigned int>,backgoundColor=:f6<unsigned int>,isBelongDevice=:f7<unsigned int>,belongDeviceID=:f8<unsigned int> WHERE ID=:f9<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
		updateStream << pInputParam->stationID;
		updateStream << pInputParam->name;
		updateStream << pInputParam->width;
		updateStream << pInputParam->height;
		updateStream << pInputParam->fontColor;
		updateStream << pInputParam->backgoundColor;
		updateStream << pInputParam->isBelongDevice;
		updateStream << pInputParam->belongDeviceID;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;

		printf("数据库更新记录ID: %d,voltageValue:%d,name: %s,width:%d,height: %d,fontColor:%d,backgoundColor:%d,isBelongDevice: %d,belongDeviceID:%d\n", pInputParam->ID, pInputParam->stationID, pInputParam->name,pInputParam->width, pInputParam->height, pInputParam->fontColor,pInputParam->isBelongDevice,pInputParam->belongDeviceID);

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
int CWarningBoardMgt::add2List( TWarningBoard *pInputParam )
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

	memcpy(&m_pList[m_count], pInputParam, sizeof(TWarningBoard));
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
int CWarningBoardMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pList[i], &m_pList[i+1], sizeof(TWarningBoard));
	}
	memset(&m_pList[m_count-1], 0x00, sizeof(TWarningBoard));
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
int CWarningBoardMgt::isIDExist( const DWORD32 ID )
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
int CWarningBoardMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_WARNINGBOARD_COUNT;
	TWarningBoard *pResource = new TWarningBoard[sumCount];

	if(NULL == pResource)
	{
		WLOutDebug("getMoreMemory():CWarningBoardMgt::CWarningBoardMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TWarningBoard)*m_sumCount);
	memcpy(pResource, m_pList, sizeof(TWarningBoard)*m_count);
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
int CWarningBoardMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TWarningBoard * CWarningBoardMgt::getListByIndex( DWORD32 index ) const
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
int CWarningBoardMgt::updateList( const DWORD32 ID, const TWarningBoard *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TWarningBoard *temp = getListByIndex(index);
	temp->stationID = pInputParam->stationID;
	memset(temp->name, 0x00, sizeof(temp->name));
	memcpy(temp->name, pInputParam->name, strlen(pInputParam->name));
	temp->width = pInputParam->width;
	temp->height = pInputParam->height;
	temp->fontColor = pInputParam->fontColor;
	temp->backgoundColor = pInputParam->backgoundColor;
	temp->isBelongDevice = pInputParam->isBelongDevice;
	temp->belongDeviceID = pInputParam->belongDeviceID;

	return 0;
}


