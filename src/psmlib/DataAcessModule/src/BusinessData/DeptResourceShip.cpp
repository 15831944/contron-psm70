#include "stdafx.h"
#include "DeptResourceShip.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-11
**************************************************************************************************/
CDptResourceShip::CDptResourceShip()
{
	m_count = 0;
	m_sumCount = MAX_DPTRESOURCESHIP_COUNT;

	m_pDptResourceList = new TDeptResourceShip[m_sumCount];
	if(NULL == m_pDptResourceList)
	{
		WLOutDebug("CDptResourceShip::CDptResourceShip()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-14
**************************************************************************************************/
CDptResourceShip::~CDptResourceShip()
{
	if(NULL != m_pDptResourceList)
	{
		delete [] m_pDptResourceList;
		m_pDptResourceList = NULL;
	}
}

/**************************************************************************************************
功能：加载所有部门资源关系列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CDptResourceShip::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    const char *sql = "SELECT * FROM t_dptresourcerelationship ";
	int loadCount = 0;
	memset(m_pDptResourceList, 0x00, sizeof(TDeptResourceShip)*m_sumCount);
	m_count = 0;

	try
	{
		otl_stream selectStream(200, sql, m_db);

		TDeptResourceShip temp = {0};
		while(!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.dptID;
			selectStream >> temp.resourceID;

			printf("Load TDptResourceShip << ID : %d,  dptID : %d, resourceID : %d\n", temp.ID, temp.dptID, temp.resourceID);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
			loadCount++;
		}

        m_db.commit();
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找部门资源关系操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：添加部门资源关系记录
参数：dptID，部门ID
	  resourceID，资源ID；
返回：-2,数据库尚未连接
	  -3，添加到列表失败
	  -4,数据库添加操作失败
	  -5，指定部门ID不存在
	  -6，指定资源ID不存在
	  -7,已经存在相同dptID+resourceID的记录
	  0，添加成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CDptResourceShip::add( const DWORD32 dptID, const DWORD32 resourceID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TDeptResourceShip temp = {0};
	temp.dptID = dptID;
	temp.resourceID = resourceID;

	int selectDptCount ;
	int selectRsrcCount;
	int selUniqueCount;
	int lastAddID;
	const char *selectDpt = "SELECT count(*) FROM t_department where ID=:f1_dptID<unsigned int>";
	const char *selectRec = "SELECT count(*) FROM t_resource where ID=:f1_resourceID<unsigned int>";
	const char *selUniqueSql = "SELECT count(*) FROM t_dptresourcerelationship where dptID=:f2_dptID<unsigned int> and resourceID=:f2_res_ID<unsigned int>";
	const char *sql = "insert into t_dptresourcerelationship values(NULL,:f1_dptID<unsigned int>,:f2_rsrcID<unsigned int>)";

	try
	{
        m_db.commit();
		otl_stream selDptStream(1, selectDpt, m_db);
		selDptStream.set_commit(0);
		selDptStream << dptID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selDptStream >> strCount;
            selectDptCount = atoi(strCount);
        }
        else
        {
            selDptStream >> selectDptCount;
        }
		if(selectDptCount <= 0)			return	-5;

		otl_stream selRecStream(1, selectRec, m_db);
		selRecStream.set_commit(0);
		selRecStream << resourceID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selRecStream >> strCount;
            selectRsrcCount = atoi(strCount);
        }
        else
        {
            selRecStream >> selectRsrcCount;
        }
		if(selectRsrcCount <= 0)		return -6;

		otl_stream selUniqueStream(1,selUniqueSql, m_db);
		selUniqueStream.set_commit(0);
		selUniqueStream <<dptID;
		selUniqueStream << resourceID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selUniqueStream >> strCount;
            selUniqueCount = atoi(strCount);
        }
        else
        {
            selUniqueStream >> selUniqueCount;
        }
		if(selUniqueCount > 0)			return -7;

		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << dptID;
		insertStream << resourceID;

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

		printf("数据库插入记录ID: %d,dptID:%d,resourceID:%d\n", temp.ID, temp.dptID, temp.resourceID);
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
功能：删除资源记录
参数：ID，删除的记录的ID
返回：-2，数据库尚未连接
	  -3, 从部门记录列表中删除记录失败
	  -4， 数据库删除操作失败
	  -5，对应该ID的记录不存在
	  0， 成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CDptResourceShip::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

	const char *sql = "DELETE FROM t_dptresourcerelationship WHERE ID = :f1_ID<unsigned int>";
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
功能：更新指定ID部门资源关系的记录
参数：ID，部门ID
      pDptResourceShip，更新的部门资源关系信息
返回：-1,参数错误
	  -2, 数据库尚未连接
	  -5，该ID不存在
	  -4, 数据库操作失败
	  -6， 部门ID不存在
	  -7，资源ID不存在
	  -8,已经存在相同dptID+resourceID的记录
	  0，更新成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CDptResourceShip::update( const DWORD32 ID, const TDeptResourceShip *pDptResourceShip )
{
	if(NULL == pDptResourceShip)	return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -5;

	int selectDptCount ;
	int selectRsrcCount;
	int selUniqueCount;
	const char *selectDpt = "SELECT count(*) FROM t_department where ID=:f1_dptID<unsigned int>";
	const char *selectRec = "SELECT count(*) FROM t_resource where ID=:f1_resourceID<unsigned int>";
	const char *selUniqueSql = "SELECT count(*) FROM t_dptresourcerelationship where dptID=:f2_dptID<unsigned int> and resourceID=:f2_res_ID<unsigned int>";
	const char *sql = "UPDATE t_dptresourcerelationship SET dptID=:f1<unsigned int>,resourceID=:f2<unsigned int> WHERE ID=:f3<unsigned int>";
	try
	{
		otl_stream selDptStream(1, selectDpt, m_db);
		selDptStream << pDptResourceShip->dptID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selDptStream >> strCount;
            selectDptCount = atoi(strCount);
        }
        else
        {
            selDptStream >> selectDptCount;
        }
		if(selectDptCount <= 0)			return	-6;

		otl_stream selRecStream(1, selectRec, m_db);
		selRecStream << pDptResourceShip->resourceID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selRecStream >> strCount;
            selectRsrcCount = atoi(strCount);
        }
        else
        {
            selRecStream >> selectRsrcCount;
        }
		if(selectRsrcCount <= 0)		return -7;

		otl_stream selUniqueStream(1,selUniqueSql, m_db);
		selUniqueStream.set_commit(0);
		selUniqueStream << pDptResourceShip->dptID;
		selUniqueStream << pDptResourceShip->resourceID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selUniqueStream >> strCount;
            selUniqueCount = atoi(strCount);
        }
        else
        {
            selUniqueStream >> selUniqueCount;
        }
		if(selUniqueCount > 0)			return -8;

		otl_stream updateStream(1, sql, m_db);
		updateStream << pDptResourceShip->dptID;
		updateStream << pDptResourceShip->resourceID;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID, pDptResourceShip);
		if(ret < 0)		return -3;

		printf("数据库更新记录ID:ID:%d,depID:%d,resourceID:%d\n", ID, pDptResourceShip->dptID, pDptResourceShip->resourceID);

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
功能：添加部门资源关系记录到部门资源关系记录列表
参数：pResource，添加的资源记录
返回：-1，参数错误
	  -2， 重复记录
	  -3， 申请内存失败
	  0， 成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CDptResourceShip::add2List( const TDeptResourceShip *pResource )
{
	if(NULL == pResource)		return -1;

	int ret = isIDExist(pResource->ID);
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

	memcpy(&m_pDptResourceList[m_count], pResource, sizeof(TDeptResourceShip));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从部门资源关系列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：李施施 2014-07-14
**************************************************************************************************/
int CDptResourceShip::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pDptResourceList[i], &m_pDptResourceList[i+1], sizeof(TDeptResourceShip));
	}
	memset(&m_pDptResourceList[m_count-1], 0x00, sizeof(TDeptResourceShip));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：从人员列表中批量删除指定ID的记录
参数：IDList，人员ID列表
	  count， 列表数量
返回：=0，删除成功
	  -1，参数错误
	  -2，列表中有ID不存在
作者：李施施 2014-07-10
**************************************************************************************************/
int CDptResourceShip::delFromList( const DWORD32 *IDList, DWORD32 count )
{
	if(NULL == IDList || count < 0)		return -1;

	for(int i=0; i<count; i++)
	{
		int ret = delFromList(IDList[i]);
		if(ret < 0)		return -2;
	}

	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：李施施 2014-07-14
**************************************************************************************************/
int CDptResourceShip::isIDExist( const DWORD32 ID )
{
	int index = -1;

	m_pCS->enter();
	for(int i=0; i<m_count; i++)
	{
		if(m_pDptResourceList[i].ID == ID)	index = i;
	}
	m_pCS->leave();

	return index;
}

/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：李施施 2014-07-14
**************************************************************************************************/
int CDptResourceShip::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_DPTRESOURCESHIP_COUNT;
	TDeptResourceShip *pResource = new TDeptResourceShip[sumCount];

	if(NULL == pResource)
	{
		WLOutDebug("getMoreMemory():CResourceMgt::CResourceMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TDeptResourceShip)*m_sumCount);
	memcpy(pResource, m_pDptResourceList, sizeof(TDeptResourceShip)*m_count);
	delete [] m_pDptResourceList;

	m_pDptResourceList = pResource;
	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：李施施 2014-07-29
**************************************************************************************************/
DWORD32 CDptResourceShip::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TDeptResourceShip * CDptResourceShip::getListByIndex( DWORD32 index ) const
{
	if(index > m_count-1)	return NULL;

	return &m_pDptResourceList[index];
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
int CDptResourceShip::updateList( const DWORD32 ID, const TDeptResourceShip *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TDeptResourceShip *temp = getListByIndex(index);
	temp->dptID = pInputParam->dptID;
	temp->resourceID = pInputParam->resourceID;

	return 0;
}

