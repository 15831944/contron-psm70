#include "stdafx.h"
#include "ResourceMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-11
**************************************************************************************************/
CResourceMgt::CResourceMgt()
{
	m_count = 0;
	m_sumCount = MAX_RESOURCE_COUNT;

	m_pResourceList = new TResource[m_sumCount];
	if(NULL == m_pResourceList)
	{
		WLOutDebug("CResourceMgt::CResourceMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-11
**************************************************************************************************/
CResourceMgt::~CResourceMgt()
{
	if(NULL != m_pResourceList)
	{
		delete [] m_pResourceList;
		m_pResourceList = NULL;
	}
}

/**************************************************************************************************
功能：加载所有资源列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-11
**************************************************************************************************/
int CResourceMgt::retrieve()
{
	if(!m_isInit)			return -2;

    if( checkDBConnect() < 0 )      return -4;

    const char *sql = "SELECT * FROM t_resource";
	memset(m_pResourceList, 0x00, sizeof(TResource) * m_sumCount);
	int loadCount = 0;
	m_count = 0;

	try
	{
		otl_stream selectStream(200, sql, m_db);

		TResource resource = {0};
		while (!selectStream.eof())
		{
			selectStream >> resource.ID;
			selectStream >> resource.resourceCode;
			selectStream >> resource.type;
			printf("Load Resource<<ID: %d,resourceCode:%d,type:%d\n", resource.ID, resource.resourceCode, resource.type);

			int ret = add2List(&resource);
			if(ret < 0)		return -3;

			loadCount ++;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找资源操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：根据资源编码加载资源列表
参数：resourceCode，资源编码
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-11
**************************************************************************************************/
int CResourceMgt::retrieve( const DWORD32 resourceCode )
{
	if(!m_isInit)			return -2;

    if( checkDBConnect() < 0 )      return -4;

    const char *sql = "SELECT * FROM t_resource where resourceCode=:f1<unsigned int>";
	memset(m_pResourceList, 0x00, sizeof(TResource) * m_sumCount);
	int loadCount = 0;
	m_count = 0;

	try
	{
		otl_stream selectStream(200, sql, m_db);
		selectStream << resourceCode;

		TResource resource = {0};
		while (!selectStream.eof())
		{
			selectStream >> resource.ID;
			selectStream >> resource.resourceCode;
			selectStream >> resource.type;
			printf("Load Resource<<ID: %d,resourceCode:%d,type:%d\n", resource.ID, resource.resourceCode, resource.type);

			int ret = add2List(&resource);
			if(ret < 0)		return -3;

			loadCount ++;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找资源操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：添加资源记录
参数：reourceCode，资源编码
	  type，类型；
返回：-2,数据库尚未连接
	  -3，添加到列表失败
	  -4,数据库添加操作失败
	  0，添加成功
作者：李施施 2014-07-11
**************************************************************************************************/
int CResourceMgt::add( const  TResource *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TResource temp = {0};
	temp.resourceCode = pInputParam->resourceCode;
	temp.type = pInputParam->type;

	DWORD32 lastAddID;
	DWORD32 count;
	DWORD32 maxResCode;
	const char *sql = "insert into t_resource values(NULL,:f1_resourceCode<unsigned int>,:f2_type<unsigned int>)";
	const char *selCountSql = "select count(*) from t_resource where resourceCode=:f3_reourceCode<unsigned int>";
	const char *updateSql = "update t_resource set resourceCode=:f3_code<unsigned int> where ID=:f3_ID<unsigned int>";
	const char *seluUniqyeSql = "select count(*) from t_resource where resourceCode=:f4_pric<unsigned int>";

	try
	{
		//查询数据库中是否存在相同resourceCode的记录，如果存在不加入
		otl_stream selCountStream(1,selCountSql, m_db);
		selCountStream.set_commit(0);
		selCountStream << pInputParam->resourceCode;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selCountStream >> strCount;
            count = atoi(strCount);
        }
        else
        {
            selCountStream >> count;
        }
		if(count > 0)		return -5;

		//插入数据库
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << temp.resourceCode;
		insertStream << temp.type;

		//获得最新插入数据的ID
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

		//传入privilegeCode=0则由程序按照以下算法填充
		if(0 == pInputParam->resourceCode)
		{
			//判断是否存在以ID值为privilegeCode的记录
			otl_stream selUniqueStream(1, seluUniqyeSql, m_db);
			selUniqueStream.set_commit(0);
			selUniqueStream << lastAddID;
            if(1 == m_dbType)//SQLite数据库
            {
                char strCount[20] = {0};
                selUniqueStream >> strCount;
                count = atoi(strCount);
            }
            else
            {
                selUniqueStream >> count;
            }

			//如果存在，以最大privilegeCode数作为插入的privilegeCode的值，否则，用ID值作为privilegeCode的值
			if(count > 0)
			{
				otl_stream selMaxStream(1,"SELECT MAX(resourceCode) + 1 FROM t_resource", m_db);
				selMaxStream.set_commit(0);
				selMaxStream >> maxResCode;

				otl_stream upStream(1, "update t_resource set resourceCode=:f5_max<unsigned int> where ID=:f3_ID<unsigned int>", m_db);
				upStream.set_commit(0);
				upStream << maxResCode;
				upStream << lastAddID;

				temp.resourceCode = maxResCode;
			}
			else
			{
				otl_stream updateStream(1, updateSql, m_db);
				updateStream.set_commit(0);
				updateStream << lastAddID;
				updateStream << lastAddID;

				temp.resourceCode = lastAddID;
			}
		}

		m_db.commit();

		printf("数据库插入记录ID: %d,resourceCode:%d,type:%d\n", temp.ID, temp.resourceCode, temp.type);
		int ret = add2List(&temp);
		if(ret < 0)		return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("添加资源操作失败！");
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
	  -5，对应该ID的部门记录不存在
	  0， 成功
作者：李施施 2014-07-11
**************************************************************************************************/
int CResourceMgt::del( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    int index = isIDExist(ID);
	if(-1 == index)		return -5;

	const char *deleteSql = "DELETE FROM t_resource WHERE ID = :f1<unsigned int>";

	try
	{
		otl_stream deleteStream(1, deleteSql, m_db);
		deleteStream << ID;

		printf("数据库删除记录 ID: %d\n", ID);

        m_db.commit();
		int ret = delFromList(ID);
		if(-1 == ret)	return -3;
	}catch(otl_exception& p)
	{
		WLOutDebug("删除资源操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：强制删除资源记录，先删除部门资源表记录，再删除权限表记录
参数：ID，删除的记录的ID
返回：-2，数据库尚未连接
	  -3, 从部门记录列表中删除记录失败
	  -4， 数据库删除操作失败
	  -5，对应该ID的部门记录不存在
	  0， 成功
作者：李施施 2014-07-11
**************************************************************************************************/
int CResourceMgt::forceDel( const DWORD32 ID, CDptResourceShip *pDptResShip, CPrivilegeMgt *pPrivilegeMgt )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(-1 == ret)		return -5;

	DWORD32 count = 0;								//记录对应resourceID下的部门资源对应关系数量
	DWORD32 listCount = MAX_DPTRESOURCESHIP_COUNT;	//用来记录对应resourceID下的部门资源对应关系的数组大小
	DWORD32 count2 = 0;								//记录对应resourceID下的权限记录数量
	DWORD32 listCount2 = MAX_PRIVILEGE_COUNT;		//用来记录对应resourceID下的权限记录的数组大小
	DWORD32 resourceCode;

	const char *selDptResSql = "select ID from t_dptresourcerelationship where resourceID=:f1_resID<unsigned int>";
	const char *delDptResSql = "delete from t_dptresourcerelationship where resourceID=:f1_resID<unsigned int>";
	const char *selResCodeSql = "select resourceCode from t_resource where ID=:f1_resID<unsigned int>";
	const char *selPriIDSql = "select ID from t_privilege where resourceCode=:f1_resID<unsigned int>";
	const char *delPriSql = "delete from t_privilege where resourceCode=:f1_resID<unsigned int>";
	const char *sql = "DELETE FROM t_resource WHERE ID = :f1_ID<unsigned int>";

	DWORD32 *pDptResIDList = new DWORD32[listCount];
	if(NULL == pDptResIDList)
	{
		WLOutDebug("CResourceMgt::forceDel new int1 Error!");
		return -6;
	}

	DWORD32 *pPriIDList = new DWORD32 [listCount2];
	if(NULL == pDptResIDList)
	{
		WLOutDebug("CResourceMgt::forceDel new int1 Error!");
		delete [] pDptResIDList;
		return -7;
	}

	try
	{
		//查找对应resourceID下所有的资源部门对应关系列表
		otl_stream selDptResStream(1, selDptResSql, m_db);
		selDptResStream.set_commit(0);
		selDptResStream << ID;

		while (!selDptResStream.eof())
		{
			//内存不够，重新分配空间
			if(count >= listCount)
			{
				int ret = getMoreMemory(&pDptResIDList,count, listCount);
				ret = 0;
				if(ret < 0)
				{
					delete [] pDptResIDList;
					delete [] pPriIDList;
					pDptResIDList = NULL;
					pPriIDList = NULL;
					return -8;
				}
			}

			selDptResStream >> pDptResIDList[count];
			count++;
		}

		//删除资源部门对应关系记录
		otl_stream delDptResStream(1, delDptResSql, m_db);
		delDptResStream.set_commit(0);
		delDptResStream << ID;

		//找到对应ID的资源编码
		otl_stream selResCodeStream(1, selResCodeSql, m_db);
		selResCodeStream.set_commit(0);
		selResCodeStream << ID;
		selResCodeStream >> resourceCode;

		//查找对应resourceCode的ID列表
		otl_stream selPriIDStream(1,selPriIDSql, m_db);
		selPriIDStream.set_commit(0);
		selPriIDStream << resourceCode;
		while (!selPriIDStream.eof())
		{
			//内存不够，重新分配空间
			if(count2 >= listCount2)
			{
				int ret = getMoreMemory(&pPriIDList,count2, listCount2);
				if(ret < 0)
				{
					delete [] pDptResIDList;
					delete [] pPriIDList;
					pDptResIDList = NULL;
					pPriIDList = NULL;
					return -9;
				}
			}
			selPriIDStream >> pPriIDList[count2];
			count2++;
		}

		//删除对应resourceCode的权限记录
		otl_stream delPriDtream(1, delPriSql, m_db);
		delPriDtream.set_commit(0);
		delPriDtream << resourceCode;


		//删除资源记录
		otl_stream delStream(1, sql, m_db);
		delStream.set_commit(0);
		delStream << ID;

		printf("数据库删除记录 ID: %d\n", ID);

		m_db.commit();

		if(NULL != pDptResShip)
		{
			int ret = pDptResShip->delFromList(pDptResIDList, count);
			if(ret < 0)
			{
				delete [] pDptResIDList;
				delete [] pPriIDList;
				pDptResIDList = NULL;
				pPriIDList = NULL;
				return -10;
			}
		}

		if(NULL != pPrivilegeMgt)
		{
			int ret = pPrivilegeMgt->delFromList(pPriIDList, count2);
			if(ret < 0)
			{
				delete [] pDptResIDList;
				delete [] pPriIDList;
				pDptResIDList = NULL;
				pPriIDList = NULL;
				return -11;
			}
		}
		
		int ret = delFromList(ID);
		if(ret < 0)		
		{
			delete [] pDptResIDList;
			delete [] pPriIDList;
			pDptResIDList = NULL;
			pPriIDList = NULL;
			return -3;
		}
	}
	catch (otl_exception& p)
	{
		WLOutDebug("删除资源操作失败！");
		m_db.rollback();
		delete [] pDptResIDList;
		delete [] pPriIDList;
		pDptResIDList = NULL;
		pPriIDList = NULL;

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}

	delete [] pDptResIDList;
	delete [] pPriIDList;
	pDptResIDList = NULL;
	pPriIDList = NULL;

	return 0;
}

/**************************************************************************************************
功能：更新指定ID资源的记录
参数：ID，部门ID
      pDepartment，更新的部门信息
返回：-1,参数错误
	  -2, 数据库尚未连接
	  -5，该ID不存在
	  -4, 数据库操作失败
	  0，更新成功
作者：李施施 2014-07-11
**************************************************************************************************/
int CResourceMgt::update( const DWORD32 ID, const TResource *pResource)
{
	if(NULL == pResource)	return -1;

	if(!m_isInit)			return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)				return -5;

    const char *sql = "UPDATE t_resource SET resourceCode=:f1<unsigned int>,`type`=:f2<unsigned int> WHERE ID=:f3<unsigned int>";

	try
	{
		//更新记录
		otl_stream updateSream(1, sql,m_db);
		updateSream << pResource->resourceCode;
		updateSream << pResource->type;
		updateSream << ID;

        m_db.commit();
		int ret = updateList(ID,pResource);
		if(ret < 0)		return -3;
		
		printf("数据库更新记录ID:ID:%d,resourceCode:%d,type:%d\n", pResource->ID, pResource->resourceCode, pResource->type);
	}
	catch (otl_exception& p)
	{
		WLOutDebug("更新资源操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：添加资源记录到资源记录列表
参数：pResource，添加的资源记录
返回：-1，参数错误
	  -2， 重复记录
	  -3， 申请内存失败
	  0， 成功
作者：李施施 2014-07-11
**************************************************************************************************/
int CResourceMgt::add2List( const TResource *pResource )
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

	memcpy(&m_pResourceList[m_count], pResource, sizeof(TResource));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从资源列表中删除指定ID的记录
参数：ID，人员ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：李施施 2014-07-11
**************************************************************************************************/
int CResourceMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(-1 == index)		return -1;
	
	m_pCS->enter();
	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pResourceList[i], &m_pResourceList[i+1], sizeof(TResource));
	}
	memset(&m_pResourceList[m_count-1], 0x00, sizeof(TResource));
	m_count--;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：判断资源ID是否存在
参数：ID，人员ID
返回：>0，该ID存在
	  -1，ID不存在
作者：李施施 2014-07-11
**************************************************************************************************/
int CResourceMgt::isIDExist( const DWORD32 ID )
{
	int index = -1;

	m_pCS->enter();
	for(int i=0; i<m_count; i++)
	{
		if(m_pResourceList[i].ID == ID)	index = i;
	}
	m_pCS->leave();

	return index;
}

/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：李施施 2014-07-11
**************************************************************************************************/
int CResourceMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_RESOURCE_COUNT;
	TResource *pResource = new TResource[sumCount];

	if(NULL == pResource)
	{
		WLOutDebug("getMoreMemory():CResourceMgt::CResourceMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TResource)*m_sumCount);
	memcpy(pResource, m_pResourceList, sizeof(TResource)*m_count);
	delete [] m_pResourceList;

	m_pResourceList = pResource;
	return 0;
}


/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：李施施 2014-07-16
**************************************************************************************************/
int CResourceMgt::getMoreMemory( DWORD32 **pIDList, DWORD32 count, DWORD32 &listCount)
{
	if(NULL == pIDList || count <= 0 || listCount <= 0)		return -1;

	DWORD32 tempCount = listCount + MAX_DPTRESOURCESHIP_COUNT;
	DWORD32 *tempList = new DWORD32[tempCount];
	if (NULL == tempList)
	{
		WLOutDebug("CResourceMgt::forceDel new int2 Error!");
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
DWORD32 CResourceMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TResource * CResourceMgt::getListByIndex( DWORD32 index ) const
{
	if(index > m_count-1)	return NULL;

	return &m_pResourceList[index];
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
int CResourceMgt::updateList( const DWORD32 ID, const TResource *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TResource *temp = getListByIndex(index);
	temp->resourceCode = pInputParam->resourceCode;
	temp->type = pInputParam->type;

	return 0;
}


