#include "stdafx.h"
#include "RoleMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-15
**************************************************************************************************/
CRoleMgt::CRoleMgt()
{
	m_count = 0;
	m_sumCount = MAX_ROLE_COUNT;

	m_pRoleList = new TRole[m_sumCount];
	if(NULL == m_pRoleList)
	{
		WLOutDebug("CRoleMgt::CRoleMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-15
**************************************************************************************************/
CRoleMgt::~CRoleMgt()
{
	if(NULL != m_pRoleList)
	{
		delete [] m_pRoleList;
		m_pRoleList = NULL;
	}
}

/**************************************************************************************************
功能：加载所有角色列表
参数：无
返回：-2，未初始化数据库连接
	  -3，插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-15
**************************************************************************************************/
int CRoleMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    int loadCount = 0;
    const char *sql = "SELECT * FROM t_role";
	memset(m_pRoleList, 0x00, sizeof(TRole)*m_sumCount);
	m_count = 0;

	try
	{
		otl_stream selStream(1, sql, m_db);

		TRole temp = {0};
		while(!selStream.eof())
		{
			selStream >> temp.ID;
			selStream >> temp.name;

			printf("Load TRole << ID : %d, name : %s \n", temp.ID, temp.name);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;

			loadCount++;
		}
        m_db.commit();
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找角色操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}

	return 0;
}

/**************************************************************************************************
功能：添加角色记录
参数：pName，角色名称
返回：-1，参数错误
	  -2,数据库尚未连接
	  -3，添加到列表失败
	  -4,数据库添加操作失败
	  0，添加成功
作者：李施施 2014-07-15
**************************************************************************************************/
int CRoleMgt::add( const TRole *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	if(!m_isInit)			return -2;

    if( checkDBConnect() < 0 )      return -4;

    TRole temp = {0};
	memcpy(&temp.name, pInputParam->name, strlen(pInputParam->name));

	int lastAddID;
	const char *sql = "insert into t_role values(NULL,:f1_name<char[32]>)";

	try
	{	
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->name;

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
            otl_stream selectIDStream(1, "SELECT LAST_INSERT_ID()", m_db);
            insertStream.set_commit(0);
            selectIDStream >> lastAddID;
            temp.ID = lastAddID;
        }

		m_db.commit();				//提交事务

		printf("数据库插入记录ID: %d,name:%s\n", temp.ID, temp.name);
		int ret = add2List(&temp);
		if(ret < 0)			return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("添加角色操作失败！");
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
功能：删除角色记录
参数：ID，删除的记录的ID
返回：-2，数据库尚未连接
	  -3, 从列表中删除记录失败
	  -4， 数据库删除操作失败
	  -5，对应该ID的记录不存在
	  0， 成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CRoleMgt::del( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    int index = isIDExist(ID);
	if(index < 0)		return -5;

	const char *sql = "DELETE FROM t_role WHERE ID = :f1_ID<unsigned int>";
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
		WLOutDebug("删除权限角色模板关系操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}

	return 0;
}

/**************************************************************************************************
功能：删除角色记录
参数：ID，删除的记录的ID
返回：-2，数据库尚未连接
	  -3, 从列表中删除记录失败
	  -4， 数据库删除操作失败
	  -5，对应该ID的记录不存在
	  0， 成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CRoleMgt::forceDel( const DWORD32 ID, CPrivilegeMgt *pPrivilegeMgt )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int flag = isIDExist(ID);
	if(-1 == flag)	return -3;

	const char *deleteRoleSql = "DELETE FROM t_role WHERE ID = :f1<unsigned int>";
	const char *delPrivSql = "DELETE FROM t_privilege WHERE roleID = :f2<unsigned int>";
	const char *roleSql =  "SELECT ID FROM t_privilege WHERE roleID = :f3<unsigned int>";

	DWORD32 count = 0;								//记录对应角色ID下的权限数量
	DWORD32 listCount = MAX_PRIVILEGE_COUNT;		//用来记录对应角色ID下的权限数量的数组大小
	DWORD32 *pPrivilegeIDList = new DWORD32[listCount];	//记录对应角色ID下下的权限数量

	if(NULL == pPrivilegeIDList)
	{
		WLOutDebug("CRoleMgt::forceDel new int1 Error!");
		return -8;
	}

	try
	{
		//查找出对应roleID下的所有privilegeID
		if(pPrivilegeMgt != NULL)
		{
			otl_stream privStream(1, roleSql, m_db);
			privStream.set_commit(0);
			privStream << ID;

			while(!privStream.eof())
			{
				//内存不够，重新分配空间
				if(count >= listCount)
				{

					int ret = getMoreMemory(&pPrivilegeIDList, count, listCount);
					if(ret < 0)
					{
						delete [] pPrivilegeIDList;
						pPrivilegeIDList = NULL;
						return -5;
					}
				}

				privStream >> pPrivilegeIDList[count];
				count++;
			}
		}

		//删除对应roleID下的所有privilege
		if(pPrivilegeMgt != NULL)
		{
			otl_stream delStaff(1, delPrivSql, m_db);
			delStaff.set_commit(0);
			delStaff << ID;
		}

		//删除对应role
		otl_stream deleteStream(1, deleteRoleSql, m_db);
		deleteStream.set_commit(0);
		deleteStream << ID;

		m_db.commit();

		printf("数据库删除记录 ID: %d\n", ID);

		//删除角色列表
		int ret = delFromList(ID);
		if(-1 == ret)
		{
			delete [] pPrivilegeIDList;
			return -6;
		}

		//删除权限列表
		if(pPrivilegeMgt != NULL)
		{
			ret = pPrivilegeMgt->delFromList(pPrivilegeIDList, count);
			if (ret < 0)	
			{
				delete [] pPrivilegeIDList;
				return -7;
			}
		}
	}catch(otl_exception& p)
	{
		WLOutDebug("删除部门操作失败！");
		m_db.rollback();			//事务回滚
		delete [] pPrivilegeIDList;	//释放空间

		cerr<<p.msg<<endl;			// print out error message
		cerr<<p.stm_text<<endl;		// print out SQL that caused the error
		cerr<<p.sqlstate<<endl;		// print out SQLSTATE message
		cerr<<p.var_info<<endl;		// print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：更新指定ID的角色记录
参数：ID，部门ID
      pRole，更新的角色信息
返回：-1,参数错误
	  -2, 数据库尚未连接
	  -3，该ID不存在
	  -4, 数据库操作失败
	  -5，ID不存在
	  0，更新成功
作者：李施施 2014-07-15
**************************************************************************************************/
int CRoleMgt::update( const DWORD32 ID, const TRole *pRole )
{
	if(NULL == pRole)			return -1;

	if(!m_isInit)				return -2;

    if( checkDBConnect() < 0 )      return -4;

    int flag = isIDExist(ID);
	if(-1 == flag)				return -5;

	const char *sql = "UPDATE t_role SET name=:f1<char[32]> WHERE ID=:f4<unsigned int>";

	try
	{
		otl_stream updateStream(1,sql, m_db);

		updateStream << pRole->name;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID, pRole);
		if(ret < 0)		return -3;

		printf("数据库更新记录 ID: %d, name: %s\n", ID, pRole->name);
	}
	catch(otl_exception& p)
	{
		WLOutDebug("更新角色操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}

	return 0;
}

/**************************************************************************************************
功能：添加到列表
参数：pRoleTemplate，添加的记录
返回：-1，参数错误
	  -2， 重复记录
	  -3， 申请内存失败
	  0， 成功
作者：李施施 2014-07-15
**************************************************************************************************/
int CRoleMgt::add2List( const TRole *pRole )
{
	if(NULL == pRole)		return -1;

	int index = isIDExist(pRole->ID);
	if(index >= 0)			return -2;

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
	memcpy(&m_pRoleList[m_count], pRole, sizeof(TRole));
	m_count++;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：李施施 2014-07-15
**************************************************************************************************/
int CRoleMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(-1 == index)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pRoleList[i], &m_pRoleList[i+1], sizeof(TRole));
	}
	memset(&m_pRoleList[m_count-1], 0x00, sizeof(TRole));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：李施施 2014-07-15
**************************************************************************************************/
int CRoleMgt::isIDExist( const DWORD32 ID )
{
	int index = -1;

	m_pCS->enter();
	for(int i=0; i<m_count; i++)
	{
		if(m_pRoleList[i].ID == ID)	index = i;
	}
	m_pCS->leave();

	return index;
}

/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：李施施 2014-07-15
**************************************************************************************************/
int CRoleMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_ROLE_COUNT;
	TRole *pTempList = new TRole[sumCount];
	if (NULL == pTempList)
	{
		WLOutDebug("getMoreMemory():CRoleMgt::CRoleMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pTempList, 0x00, sizeof(TRole)*m_sumCount);
	memcpy(pTempList, m_pRoleList, sizeof(TRole)*m_count);
	delete [] m_pRoleList;

	m_pRoleList = pTempList;
	return 0;
}

/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：李施施 2014-07-16
**************************************************************************************************/
int CRoleMgt::getMoreMemory( DWORD32 **pIDList, DWORD32 count, DWORD32 &listCount)
{
	if(NULL == pIDList || count <= 0 || listCount <= 0)		return -1;

	DWORD32 tempCount = listCount + MAX_ROLE_COUNT;
	DWORD32 *tempList = new DWORD32[tempCount];
	if (NULL == tempList)
	{
		WLOutDebug("CRoleMgt::forceDel new int2 Error!");
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
DWORD32 CRoleMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TRole * CRoleMgt::getListByIndex( DWORD32 index ) const
{
	if(index > m_count-1)	return NULL;

	return &m_pRoleList[index];
}

/**************************************************************************************************
功能：更新本地列表记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
int CRoleMgt::updateList( const DWORD32 ID, const TRole *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TRole *temp = getListByIndex(index);
	memset(temp->name, 0x00, sizeof(pInputParam->name));
	memcpy(temp->name, pInputParam->name, strlen(pInputParam->name));

	return 0;
}

