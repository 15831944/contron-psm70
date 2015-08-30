#include "stdafx.h"
#include "PrivilegeMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-15
**************************************************************************************************/
CPrivilegeMgt::CPrivilegeMgt()
{
	m_count = 0;
	m_sumCount = MAX_PRIVILEGE_COUNT;

	m_pPrivilegeList = new TPrivilege[m_sumCount];
	if(NULL == m_pPrivilegeList)
	{
		WLOutDebug("CPrivilegeMgt::CPrivilegeMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-15
**************************************************************************************************/
CPrivilegeMgt::~CPrivilegeMgt()
{
	if (NULL != m_pPrivilegeList)
	{
		delete [] m_pPrivilegeList;
		m_pPrivilegeList = NULL;
	}
}

/**************************************************************************************************
功能：加载指定角色下所有权限列表
参数：roleID,角色ID
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-15
**************************************************************************************************/
int CPrivilegeMgt::retrieve(const DWORD32 roleID)
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pPrivilegeList, 0x00, m_sumCount * sizeof(TPrivilege));
	int loadCount = 0;
    const char *sql = "SELECT * FROM t_privilege where roleID=:f1<unsigned int>";
	m_count = 0;

	try
	{
		otl_stream selectStream(200, sql, m_db);
		selectStream << roleID;

		TPrivilege temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.roleID;
			selectStream >> temp.resourceCode;
			selectStream >> temp.privilegeCode;
			loadCount++;

//            printf("Load Privilege <<ID: %d,roleID:%d,resourceCode:%d,privilegeCode:%d\n", temp.ID, temp.roleID, temp.resourceCode, temp.privilegeCode);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找权限操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：加载指定角色与资源下所有权限列表
参数：roleID,角色ID  resourceCode，资源码
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：刘高舜 2014-09-03
**************************************************************************************************/
int CPrivilegeMgt::retrieveByRoleIDandResourceCode( const DWORD32 roleID, const DWORD32 resourceCode )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pPrivilegeList, 0x00, m_sumCount * sizeof(TPrivilege));
	int loadCount = 0;
    const char *sql = "SELECT * FROM t_privilege where roleID=:f1<unsigned int> AND resourceCode=:f2<unsigned int>";
	m_count = 0;

	try
	{
		otl_stream selectStream(200, sql, m_db);
		selectStream << roleID;
		selectStream << resourceCode;

		TPrivilege temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.roleID;
			selectStream >> temp.resourceCode;
			selectStream >> temp.privilegeCode;
			loadCount++;

			printf("Load Privilege <<ID: %d,roleID:%d,resourceCode:%d,privilegeCode:%d\n", temp.ID, temp.roleID, temp.resourceCode, temp.privilegeCode);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找权限操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：加载所有权限列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-15
**************************************************************************************************/
int CPrivilegeMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pPrivilegeList, 0x00, m_sumCount * sizeof(TPrivilege));
	int loadCount = 0;
    const char *sql = "SELECT * FROM t_privilege ";
	m_count = 0;

	try
	{
		otl_stream selectStream(200, sql, m_db);

		TPrivilege temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.roleID;
			selectStream >> temp.resourceCode;
			selectStream >> temp.privilegeCode;
			loadCount++;

//			printf("Load Privilege <<ID: %d,roleID:%d,resourceCode:%d,privilegeCode:%d\n", temp.ID, temp.roleID, temp.resourceCode, temp.privilegeCode);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找权限操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：添加部门记录
参数：pName，部门名称
	  dptID，上级部门ID；
	  roleID，角色ID
	  loginID，登陆ID
	  pPsw，密码
	  defaultStation，默认站
返回：-1,参数错误
	  -2,数据库尚未连接
	  -3，添加到列表失败
	  -4,数据库添加操作失败
	  -5, 指定dptID在department表中不存在
	  0，添加成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CPrivilegeMgt::add( const TPrivilege *pInputParam )
{
	if(NULL == pInputParam)				return -1;

	if(!m_isInit)						return -2;

    if( checkDBConnect() < 0 )      return -4;

    TPrivilege temp = {0};
	temp.roleID = pInputParam->roleID;
	temp.resourceCode = pInputParam->resourceCode;
	temp.privilegeCode = pInputParam->privilegeCode;

	int lastAddID;
	int recCount;
	int funcCount;
	int roleCount;
	int uniqueCount;
	const char *selRecSql = "SELECT count(*) FROM t_resource where resourceCode=:f1_resourceID<unsigned int>";
	const char *selFuncSql = "SELECT count(*) FROM t_function where privilegeCode=:f1_privilegeCode<unsigned int>";
	const char *selRoleSql = "SELECT count(*) FROM t_role where ID=:f6_roleID<unsigned int>";
	const char *selUniqueSql = "SELECT count(*) FROM t_privilege WHERE roleID=:f3_1<unsigned int> AND resourceCode=:f3_2<unsigned int> AND privilegeCode=:f3_3<unsigned int>";
	const char *sql = "insert into t_privilege values(NULL,:f1_roleID<unsigned int>,:f2_resourceCode<unsigned int>,:f3_privilegeCode<unsigned int>)";

	try
	{
		otl_stream selRecStream(1,selRecSql, m_db);
		selRecStream.set_commit(0);
		selRecStream << pInputParam->resourceCode;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selRecStream >> strCount;
            recCount = atoi(strCount);
        }
        else
        {
            selRecStream >> recCount;
        }
		if(recCount <= 0)		return -5;

		otl_stream selFuncStream(1,selFuncSql, m_db);
		selFuncStream.set_commit(0);
		selFuncStream << pInputParam->privilegeCode;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selFuncStream >> strCount;
            funcCount = atoi(strCount);
        }
        else
        {
            selFuncStream >> funcCount;
        }
		if(funcCount <= 0)		return -6;

		otl_stream selRoleStream(1, selRoleSql, m_db);
		selRoleStream.set_commit(0);
		selRoleStream << pInputParam->roleID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selRoleStream >> strCount;
            roleCount = atoi(strCount);
        }
        else
        {
            selRoleStream >> roleCount;
        }
		if(roleCount <= 0)		return -7;

		otl_stream selUniqueStream(1, selUniqueSql, m_db);
		selUniqueStream.set_commit(0);
		selUniqueStream << pInputParam->roleID;
		selUniqueStream << pInputParam->resourceCode;
		selUniqueStream << pInputParam->privilegeCode;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selUniqueStream >> strCount;
            uniqueCount = atoi(strCount);
        }
        else
        {
            selUniqueStream >> uniqueCount;
        }
        if(uniqueCount > 0)		return -8;

		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->roleID;
		insertStream << pInputParam->resourceCode;
		insertStream << pInputParam->privilegeCode;

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
            selectStream >> lastAddID;
            temp.ID = lastAddID;
        }

		m_db.commit();
//		printf("数据库插入记录ID: %d,roleID:%d,resourceCode:%d,privilegeCode:%d\n", temp.ID, temp.roleID, temp.resourceCode, temp.privilegeCode);

		int ret = add2List(&temp);
		if(ret < 0)		return -3;

	}
	catch(otl_exception& p)
	{
		WLOutDebug("添加权限操作失败！");
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
功能：批量添加记录
参数：pList，需要插入的记录列表；count，需要插入的记录数量
返回：-1，参数不合法
     -2，数据库尚未连接
     -3，数据库添加操作失败
     >0，成功
注意：1.不判断记录的角色模板ID在角色模板表中是否存在
     2.不判断权限编码在权限编码表中是否存在
     3.不将新增的记录添加到本类的记录列表中
作者：刘远安 2014-09-02
**************************************************************************************************/
int CPrivilegeMgt::insertBatch(const TPrivilege *pList, int count)
{
    int ret = -1;
    if((NULL == pList) || (count <= 0))	return ret;

    ret--;
    if(!m_isInit)   return ret;

    if( checkDBConnect() < 0 )      return -4;

    int lastAddID;
    const char *sql = "insert into t_privilege values(NULL,:f1_roleID<unsigned int>,:f2_resourceCode<unsigned int>,:f3_privilegeCode<unsigned int>)";

    ret--;
    try
    {
        otl_stream insertStream(count,sql,m_db);
        insertStream.set_commit(0);
        for(int i=0;i<count;i++)
        {
            insertStream << pList[i].roleID;
            insertStream << pList[i].resourceCode;
            insertStream << pList[i].privilegeCode;
        }

        otl_stream lastAddStream(1, "SELECT LAST_INSERT_ID()", m_db);
        lastAddStream.set_commit(0);
        lastAddStream >> lastAddID;

        m_db.commit();
    }
    catch (otl_exception& p)
    {
        WLOutDebug("添加权限角色模板关系操作失败！");
        m_db.rollback();

        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return ret;
    }

    return lastAddID;
}

/**************************************************************************************************
功能：删除权限记录
参数：ID，删除的权限记录ID
返回：-2，数据库尚未连接
	  -3, 从部门记录列表中删除记录失败
	  -4， 数据库删除操作失败
	  -5，对应该ID的部门记录不存在
	  0， 成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CPrivilegeMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int flag = isIDExist(ID);
	if(-1 == flag)	return -5;

	const char *sql = "DELETE FROM t_privilege WHERE ID = :f1<unsigned int>";

	try
	{
		otl_stream delStream(1,sql, m_db);

		delStream << ID;
		printf("数据库删除记录 ID: %d\n", ID);

        m_db.commit();
		int ret = delFromList(ID);
		if(-1 == ret)		return -3;
	}
	catch(otl_exception& p)
	{
		WLOutDebug("删除权限操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
    return 0;
}

/**************************************************************************************************
功能：批量删除权限记录
参数：pIDList，需要删除的权限记录ID列表
     count，需要删除的记录数量
返回：-1，参数非法
     -2，数据库尚未连接
     -3, 数据库删除操作失败
      >=， 成功
注意：批量删除权限记录后，不更新本类的记录列表
作者：刘远安 2014-09-02
**************************************************************************************************/
int CPrivilegeMgt::delBatch(const DWORD32 *pIDList, int count)
{
    int ret = -1;
    if((NULL == pIDList) || (count <= 0))	return ret;

    ret--;
    if(!m_isInit)   return ret;

    if( checkDBConnect() < 0 )      return -4;

    const char *sql = "DELETE FROM t_privilege WHERE ID = :f1<unsigned int>";

    ret--;
    try
    {
        otl_stream delStream(count,sql,m_db);
        delStream.set_commit(0);
        for(int i=0;i<count;i++)
        {
            delStream << pIDList[i];
        }

        m_db.commit();
    }
    catch (otl_exception& p)
    {
        WLOutDebug("删除权限操作失败！");
        m_db.rollback();

        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return ret;
    }

    return 1;
}

/**************************************************************************************************
功能：更新指定ID权限的记录
参数：ID，权限ID
      pPrivilege，更新的权限信息
返回：-1,参数错误
	  -2, 数据库尚未连接
	  -3，该ID不存在
	  -4, 数据库操作失败
	  -5， 资源ID不存在
	  -6，功能ID不存在
	  0，更新成功
作者：李施施 2014-07-15
**************************************************************************************************/
int CPrivilegeMgt::update( const DWORD32 ID, TPrivilege *pPrivilege )
{
	if(NULL == pPrivilege)		return -1;

	if (!m_isInit)				return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(-1 == ret)				return -5;

	int recCount;
	int priCount;
	int roleCount;
	const char *selRecSql= "SELECT count(*) FROM t_resource where resourceCode=:f1_resourceID<unsigned int>";
	const char *selFuncSql= "SELECT count(*) FROM t_function where privilegeCode=:f1_privilegeCode<unsigned int>";
	const char *selRoleSql= "SELECT count(*) FROM t_role where ID=:f6_roleID<unsigned int>";
	const char *sql = "UPDATE t_privilege SET roleID=:f1<unsigned int>,resourceCode=:f2<unsigned int>,privilegeCode=:f3<unsigned int> where ID=:f4<unsigned int>";

	try
	{
		otl_stream selRecStream(1, selRecSql, m_db);
		selRecStream << pPrivilege->resourceCode;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selRecStream >> strCount;
            recCount = atoi(strCount);
        }
        else
        {
            selRecStream >> recCount;
        }
		if(recCount <= 0)	return -6;

		otl_stream selPriStream(1, selFuncSql, m_db);
		selPriStream << pPrivilege->privilegeCode;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selPriStream >> strCount;
            priCount = atoi(strCount);
        }
        else
        {
            selPriStream >> priCount;
        }
		if(priCount <= 0)	return -7;

		otl_stream selRoleStream(1, selRoleSql, m_db);
		selRoleStream.set_commit(0);
		selRoleStream << pPrivilege->roleID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selRoleStream >> strCount;
            roleCount = atoi(strCount);
        }
        else
        {
            selRoleStream >> roleCount;
        }
		if(roleCount <= 0)		return -8;

		otl_stream updateStream(1,sql,m_db);
		updateStream << pPrivilege->roleID;
		updateStream << pPrivilege->resourceCode;
		updateStream << pPrivilege->privilegeCode;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pPrivilege);
		if(ret < 0)		return -3;

		printf("数据库更新记录ID: %d,roleID:%d,resourceCode:%d,privilegeCode:%d\n", ID, pPrivilege->roleID, pPrivilege->resourceCode, pPrivilege->privilegeCode);

	}catch(otl_exception& p)
	{
		WLOutDebug("更新权限操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：添加权限记录到列表
参数：pPrivilege，添加的权限记录
返回：-1，参数错误
	  -2， 重复记录
	  -3， 申请内存失败
	  0， 成功
作者：李施施 2014-07-15
**************************************************************************************************/
int CPrivilegeMgt::add2List( TPrivilege *pPrivilege )
{
	if(NULL == pPrivilege)		return -1;

	int ret = isIDExist(pPrivilege->ID);
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

	memcpy(&m_pPrivilegeList[m_count], pPrivilege, sizeof(TPrivilege));
	m_count++;
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
int CPrivilegeMgt::isIDExist( const DWORD32 ID )
{
	int rev = -1;
	m_pCS->enter();
	for(int i=0; i<m_count; i++)
	{
		if(ID == m_pPrivilegeList[i].ID)
		{
			rev = i;
			break;
		}
	}
	m_pCS->leave();

	return rev;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：李施施 2014-07-15
**************************************************************************************************/
int CPrivilegeMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(-1 == index)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pPrivilegeList[i], &m_pPrivilegeList[i+1], sizeof(TPrivilege));
	}
	memset(&m_pPrivilegeList[m_count-1], 0x00, sizeof(TPrivilege));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：从列表中批量删除指定ID的记录
参数：IDList，ID列表
	  count， 列表数量
返回：=0，删除成功
	  -1，参数错误
	  -2，列表中有ID不存在
作者：李施施 2014-07-17
**************************************************************************************************/
int CPrivilegeMgt::delFromList( const DWORD32 *IDList, DWORD32 count )
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
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：李施施 2014-07-15
**************************************************************************************************/
int CPrivilegeMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_PRIVILEGE_COUNT;

	TPrivilege *pPrivilegeList = new TPrivilege[sumCount];
	if (NULL == pPrivilegeList)
	{
		WLOutDebug("getMoreMemory():CStaffMgt::CStaffMgt()-->申请内存失败!");
		return -1;
	}

	m_sumCount = sumCount;
	memset(pPrivilegeList, 0x00, m_sumCount*sizeof(TPrivilege));
	memcpy(pPrivilegeList, m_pPrivilegeList, m_count*sizeof(TPrivilege));
	delete [] m_pPrivilegeList;
	m_pPrivilegeList = pPrivilegeList;

	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：李施施 2014-07-29
**************************************************************************************************/
DWORD32 CPrivilegeMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TPrivilege * CPrivilegeMgt::getListByIndex( DWORD32 index ) const
{	
	if(index > m_count-1)	return NULL;

	return &m_pPrivilegeList[index];
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
int CPrivilegeMgt::updateList( const DWORD32 ID, const TPrivilege *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TPrivilege *temp = getListByIndex(index);
	temp->roleID = pInputParam->roleID;
	temp->resourceCode = pInputParam->resourceCode;
	temp->privilegeCode = pInputParam->privilegeCode;

	return 0;
}

