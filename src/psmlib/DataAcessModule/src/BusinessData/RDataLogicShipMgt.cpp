#include "stdafx.h"
#include "RDataLogicShipMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-23
**************************************************************************************************/
CRDataLogicShipMgt::CRDataLogicShipMgt()
{
	m_count = 0;
	m_sumCount = MAX_RDATALOGICSHIP_COUNT;

	m_pList = new TRDataLogicShip[m_sumCount];
	if(NULL == m_pList)
	{
		WLOutDebug("CRDataLogicShipMgt::CRDataLogicShipMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-23
**************************************************************************************************/
CRDataLogicShipMgt::~CRDataLogicShipMgt()
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
int CRDataLogicShipMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TRDataLogicShip));
	int loadCount = 0;
	m_count = 0;
    const char *sql = "SELECT * FROM t_rdatalogicship ORDER BY `index` ";

	try
	{
        m_db.commit();
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

		TRDataLogicShip temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.rDataLogicID;
			selectStream >> temp.condition;
			selectStream >> temp.expression;
			selectStream >> temp.index;
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
功能：加载所有列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-23
**************************************************************************************************/
int CRDataLogicShipMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TRDataLogicShip));
	int loadCount = 0;

    const char *sql = "SELECT * FROM t_rdatalogicship where rDataLogicID=:f<unsigned int> ";

	try
	{
        m_db.commit();
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TRDataLogicShip temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.rDataLogicID;
			selectStream >> temp.condition;
			selectStream >> temp.expression;
			selectStream >> temp.index;
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
作者：刘高舜 2014-08-29
**************************************************************************************************/
int CRDataLogicShipMgt::retrieveByRDataLogicID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TRDataLogicShip));
	int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_rdatalogicship where rDataLogicID=:f<unsigned int> ";

	try
	{
        m_db.commit();
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TRDataLogicShip temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.rDataLogicID;
			selectStream >> temp.condition;
			selectStream >> temp.expression;
			selectStream >> temp.index;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找操作逻辑关系表操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
    return 0;
}

/**************************************************************************************************
功能：加载指定ID下的列表,按index倒序排序
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：佘佳明 2014-12-24
**************************************************************************************************/
int CRDataLogicShipMgt::retrieveByRDataLogicID_index_desc(const DWORD32 ID)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TRDataLogicShip));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_rdatalogicship where rDataLogicID=:f<unsigned int> order by `index` desc";

    try
    {
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << ID;

        TRDataLogicShip temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.rDataLogicID;
            selectStream >> temp.condition;
            selectStream >> temp.expression;
            selectStream >> temp.index;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找操作逻辑关系表操作失败！");
        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}

/**************************************************************************************************
功能：加载指定父ID区间的记录
参数：
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：刘远安 2014-08-29
**************************************************************************************************/
int CRDataLogicShipMgt::retrieveByMinMaxID(const DWORD32 minID, const DWORD32 maxID)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TRDataLogicShip));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_rdatalogicship where rDataLogicID >=:f1<unsigned int> and rDataLogicID <=:f2<unsigned int>";

    try
    {
        m_db.commit();
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << minID;
        selectStream << maxID;

        TRDataLogicShip temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.rDataLogicID;
            selectStream >> temp.condition;
            selectStream >> temp.expression;
            selectStream >> temp.index;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找五防逻辑关系表操作失败！");
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
int CRDataLogicShipMgt::add( TRDataLogicShip *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TRDataLogicShip temp = {0};
	temp.rDataLogicID = pInputParam->rDataLogicID;
	memcpy(&temp.condition, pInputParam->condition, strlen(pInputParam->condition));
	memcpy(&temp.expression, pInputParam->expression, strlen(pInputParam->expression));
	temp.index = pInputParam->index;

	int lastAddID;
	const char *sql = "insert into t_rdatalogicship values(NULL,:f1<unsigned int>,:f2<char[64]>,:f3<char[255]>,:f4<unsigned int>)";

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->rDataLogicID;
		insertStream << pInputParam->condition;
		insertStream << pInputParam->expression;
        insertStream << pInputParam->index;

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
int CRDataLogicShipMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

	const char *sql = "DELETE FROM t_rdatalogicship WHERE ID = :f1_ID<unsigned int>";
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
作者：李施施 2014-07-23
**************************************************************************************************/
int CRDataLogicShipMgt::update( const DWORD32 ID, TRDataLogicShip *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

	const char *sql = "UPDATE t_rdatalogicship set rDataLogicID=:f1<unsigned int>,`condition`=:f2<char[64]>,expression=:f3<char[255]>, `index`=:f4<unsigned int> where ID=:f5<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
		updateStream << pInputParam->rDataLogicID;
		updateStream << pInputParam->condition;
		updateStream << pInputParam->expression;
		updateStream << pInputParam->index;
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
int CRDataLogicShipMgt::add2List( TRDataLogicShip *pInputParam )
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

	memcpy(&m_pList[m_count], pInputParam, sizeof(TRDataLogicShip));
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
int CRDataLogicShipMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pList[i], &m_pList[i+1], sizeof(TRDataLogicShip));
	}
	memset(&m_pList[m_count-1], 0x00, sizeof(TRDataLogicShip));
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
int CRDataLogicShipMgt::isIDExist( const DWORD32 ID )
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
作者：李施施 2014-07-23
**************************************************************************************************/
int CRDataLogicShipMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_RDATALOGICSHIP_COUNT;
	TRDataLogicShip *pResource = new TRDataLogicShip[sumCount];

	if(NULL == pResource)
	{
		WLOutDebug("getMoreMemory():CRDataLogicShipMgt::CRDataLogicShipMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TRDataLogicShip)*m_sumCount);
	memcpy(pResource, m_pList, sizeof(TRDataLogicShip)*m_count);
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
int CRDataLogicShipMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TRDataLogicShip * CRDataLogicShipMgt::getListByIndex( DWORD32 index ) const
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
int CRDataLogicShipMgt::updateList( const DWORD32 ID, const TRDataLogicShip *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TRDataLogicShip *temp = getListByIndex(index);
	temp->rDataLogicID = pInputParam->rDataLogicID;
	memset(temp->condition, 0x00, sizeof(temp->condition));
	memcpy(temp->condition, pInputParam->condition, strlen(pInputParam->condition));
	memset(temp->expression, 0x00, sizeof(temp->expression));
	memcpy(temp->expression, pInputParam->expression, strlen(pInputParam->expression));

	return 0;
}

