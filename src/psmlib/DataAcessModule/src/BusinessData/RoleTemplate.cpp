#include "stdafx.h"
#include "RoleTemplate.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-14
**************************************************************************************************/
CRoleTemplate::CRoleTemplate()
{
	m_count = 0;
	m_sumCount = MAX_ROLETEMPLATEMGT_COUNT;

	m_pRoleTemplateList = new TRoleTemplate[m_sumCount];
	if(NULL == m_pRoleTemplateList)
	{
		WLOutDebug("CRoleTemplate::CRoleTemplate()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-14
**************************************************************************************************/
CRoleTemplate::~CRoleTemplate()
{
	if(NULL != m_pRoleTemplateList)
	{
		delete [] m_pRoleTemplateList;
		m_pRoleTemplateList = NULL;
    }
}

/**************************************************************************************************
功能：加载所有角色模板系列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CRoleTemplate::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    char *sql = (char*)"SELECT * FROM t_roletemplate";
	int loadCount = 0;
	memset(m_pRoleTemplateList, 0x00, sizeof(TRoleTemplate)*m_sumCount);
	m_count = 0;

	try
	{
		otl_stream selectStream(1,sql,m_db);

		TRoleTemplate temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.name;

			printf("Load TRoleTemplate << ID : %d, name : %s \n", temp.ID, temp.name);

            m_db.commit();
			int ret = add2List(&temp);
			if(ret < 0)		return -3;

			loadCount++;
		}
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找角色模板操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}

	return 0;
}

/**************************************************************************************************
功能：添加角色模板记录
参数：pName，名称
返回：-1，参数错误
	  -2,数据库尚未连接
	  -3，添加到列表失败
	  -4,数据库添加操作失败
	  0，添加成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CRoleTemplate::add( const TRoleTemplate *pInputPara )
{
	if(NULL == pInputPara)	return -1;

	if(!m_isInit)			return -2;

    if( checkDBConnect() < 0 )      return -4;

    TRoleTemplate temp = {0};
	memcpy(&temp.name, pInputPara->name, strlen(pInputPara->name));

	char *sql = (char*)"insert into t_roletemplate values(NULL,:f1_name<char[32]>)";
	int lastAddID;


	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputPara->name;

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

		printf("数据库插入记录ID: %d,name:%s\n", temp.ID, temp.name);
		int ret = add2List(&temp);
		if(ret < 0)		return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("新增角色模板操作失败！");
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
功能：删除功能记录
参数：ID，删除的角色模板的ID
返回：-2，数据库尚未连接
	  -3, 从角色模板列表中删除记录失败
	  -4， 数据库删除操作失败
	  -5，对应该ID的记录不存在
	  0， 成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CRoleTemplate::del( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    int index = isIDExist(ID);
	if(index < 0)		return -5;

	char *sql = (char*)"DELETE FROM t_roletemplate WHERE ID = :f1_ID<unsigned int>";

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
        WLOutDebug("删除角色模板操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：强制删除记录
参数：ID，删除的角色模板的ID
	  pFunRoleShip，权限和角色模板关系对象
返回：-2，数据库尚未连接
	  -3, 从角色模板列表中删除记录失败
	  -4， 数据库删除操作失败
	  -5，对应该ID的记录不存在
	  -6，new int失败
	  -7，申请内存失败
	  -8，在权限角色列表删除记录失败
	  0， 成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CRoleTemplate::forceDel( const DWORD32 ID, CFuncRoleTemplateShip *pFunRoleShip//= NULL )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    int index = isIDExist(ID);
	if(index < 0)		return -5;

	const char *selShipIDSql = "select ID from t_privilegeroletemplaterelationship where roleTemplateID=:f1<unsigned int>";
	const char *delShipIDSql = "delete from t_privilegeroletemplaterelationship where roleTemplateID=:f2<unsigned int>";
	const char *sql = "DELETE FROM t_roletemplate WHERE ID = :f1_ID<unsigned int>";

	DWORD32 count = 0;
	DWORD32 listCount = MAX_FUNCROLETEMPLATESHIP_COUNT;
	DWORD32 *pIDList = new DWORD32[listCount];
	if (NULL == pIDList)
	{
		WLOutDebug("CRoleTemplate::forceDel() new int fail!");
		return -6;
	}

	try
	{
		//在权限角色模板关联表里面找到对应角色ID的所有ID记录
		otl_stream selShipIDStream(200, selShipIDSql, m_db);
		selShipIDStream.set_commit(0);
		selShipIDStream << ID;
		while (!selShipIDStream.eof())
		{
			if(count >= listCount)
			{
				int ret = getMoreMemory(&pIDList,count, listCount);
				if(ret < 0)
                {
					delete [] pIDList;
					pIDList = NULL;
					return -7;
				}
			}
			selShipIDStream >> pIDList[count];
			count++;
		}

		//在权限角色模板关联表里面删除对应角色ID的记录
		otl_stream delIDStream(1, delShipIDSql, m_db);
		delIDStream.set_commit(0);
		delIDStream << ID;

		//在角色末班里面删除对应ID的记录
		otl_stream delStream(1, sql, m_db);
		delStream.set_commit(0);
		delStream << ID;

		m_db.commit();

		printf("数据库删除记录 ID: %d\n", ID);

		if(NULL != pFunRoleShip)
		{
			int ret = pFunRoleShip->delFromList(pIDList, count);
			if(ret < 0)		
			{
				delete [] pIDList;
				return -8;
			}
		}

		int ret = delFromList(ID);
		if(ret < 0)
		{
			delete [] pIDList;
			return -3;
		}
	}
	catch (otl_exception& p)
	{
		WLOutDebug("新增角色模板操作失败！");
		delete [] pIDList;
		m_db.rollback();

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	delete [] pIDList;
	pIDList = NULL;

	return 0;
}

/**************************************************************************************************
功能：更新指定ID角色模板的记录
参数：ID，部门ID
      pRoleTemplate，更新的角色模板信息
返回：-1,参数错误
	  -2, 数据库尚未连接
	  -3，该ID不存在
	  -4, 数据库操作失败
	  -5， 部门ID不存在
	  0，更新成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CRoleTemplate::update( const DWORD32 ID, const TRoleTemplate *pRoleTemplate )
{
	if(NULL == pRoleTemplate)		return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int index = isIDExist(ID);
	if(index < 0)					return -5;

	const char *sql =  "UPDATE t_roletemplate SET name=:f1<char[32]> WHERE ID=:f2<unsigned int>";

	try
	{
		otl_stream updateStream(1, sql, m_db);
		updateStream << pRoleTemplate->name;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pRoleTemplate);
		if(ret < 0)		return -3;

		printf("数据库更新记录ID:ID:%d,name:%s\n", ID, pRoleTemplate->name);
	}
	catch (otl_exception& p)
	{
		WLOutDebug("新增角色模板操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：添加角色模板到角色模板列表
参数：pRoleTemplate，添加的角色模板
返回：-1，参数错误
	  -2， 重复记录
	  -3， 申请内存失败
	  0， 成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CRoleTemplate::add2List( const TRoleTemplate *pRoleTemplate )
{
	if(NULL == pRoleTemplate)		return -1;

	int index = isIDExist(pRoleTemplate->ID);
	if(index >= 0)					return -2;

	m_pCS->enter();

	if(m_count >= m_sumCount)
	{
		int ret = getMoreMemory();
		if(ret < 0)
		{
			m_pCS->leave();
			return -3;
		}
	}
	memcpy(&m_pRoleTemplateList[m_count], pRoleTemplate, sizeof(TRoleTemplate));
	m_count++;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：从功能列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：李施施 2014-07-14
**************************************************************************************************/
int CRoleTemplate::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for (int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pRoleTemplateList[i], &m_pRoleTemplateList[i+1], sizeof(TRoleTemplate));
	}
	memset(&m_pRoleTemplateList[m_count-1], 0x00, sizeof(TRoleTemplate));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：李施施 2014-07-14
**************************************************************************************************/
int CRoleTemplate::isIDExist( const DWORD32 ID )
{
	int index = -1;

	m_pCS->enter();
	for(int i=0; i<m_count; i++)
	{
		if(m_pRoleTemplateList[i].ID == ID)	index = i;
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
int CRoleTemplate::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_ROLETEMPLATEMGT_COUNT;
	TRoleTemplate *pTempList = new TRoleTemplate[sumCount];
	if(NULL == pTempList)
	{
		WLOutDebug("getMoreMemory():CRoleTemplate::CRoleTemplate()-->申请内存失败!");
		return -1;
	}

	m_sumCount = sumCount;
	memset(pTempList, 0x00, sizeof(TRoleTemplate)*m_sumCount);
	memcpy(pTempList, m_pRoleTemplateList, sizeof(TRoleTemplate)*m_count);
	delete [] m_pRoleTemplateList;

	m_pRoleTemplateList = pTempList;
	return 0;
}


/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：李施施 2014-07-16
**************************************************************************************************/
int CRoleTemplate::getMoreMemory( DWORD32 **pIDList, DWORD32 count, DWORD32 &listCount)
{
	if(NULL == pIDList || count <= 0 || listCount <= 0)		return -1;

	int tempCount = listCount + MAX_FUNCROLETEMPLATESHIP_COUNT;
	DWORD32 *tempList = new DWORD32[tempCount];
	if (NULL == tempList)
	{
		WLOutDebug("CRoleTemplate::forceDel new int2 Error!");
		return -2;
	}

	listCount = tempCount;

	memset(tempList, 0x00, sizeof(int) * listCount);
	memcpy(tempList, *pIDList, sizeof(int) * count);
	delete [] *pIDList;

	*pIDList = tempList;
	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：李施施 2014-07-29
**************************************************************************************************/
DWORD32 CRoleTemplate::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TRoleTemplate * CRoleTemplate::getListByIndex( DWORD32 index ) const
{
	if(index > m_count-1)	return NULL;

	return &m_pRoleTemplateList[index];
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
int CRoleTemplate::updateList( const DWORD32 ID, const TRoleTemplate *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TRoleTemplate *temp = getListByIndex(index);
	memset(temp->name, 0x00, sizeof(temp->name));
	memcpy(temp->name, pInputParam->name, strlen(pInputParam->name));

	return 0;
}
