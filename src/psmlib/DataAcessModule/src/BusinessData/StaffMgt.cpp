#include "stdafx.h"
#include "StaffMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-09
**************************************************************************************************/
CStaffMgt::CStaffMgt()
{
	m_count = 0;
	m_sumCount = MAX_STAFF_COUNT;

	m_pStaffList = new TStaff[m_sumCount];
	if(NULL == m_pStaffList)
	{
		WLOutDebug("CStaffMgt::CStaffMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-09
**************************************************************************************************/
CStaffMgt::~CStaffMgt()
{
	if (NULL != m_pStaffList)
	{
		delete [] m_pStaffList;
		m_pStaffList = NULL;
	}
}

/**************************************************************************************************
功能：加载所有人员列表
参数：无
返回：-2，未初始化数据库连接
     -3， 插入列表失败
     -4，数据库操作失败
     0，成功
作者：佘佳明 2014-09-28
**************************************************************************************************/
int CStaffMgt::retrieve()
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pStaffList, 0x00, m_sumCount * sizeof(TStaff));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_staff";

    try
    {
        //查找对应部门ID的人员记录
        otl_stream selectStream(200, sql, m_db);

        TStaff staff = {0};

        while (!selectStream.eof())
        {
            selectStream >> staff.ID;
            selectStream >> staff.name;
            selectStream >> staff.dptID;
            selectStream >> staff.roleID;
            selectStream >> staff.loginID;
            selectStream >> staff.psw;
            selectStream >> staff.defaultStation;
            loadCount++;

            printf("Load Staff<<ID: %d,name:%s,dptID:%d,roleID:%d,loginID:%d,psw:%s,default:%d\n", staff.ID, staff.name, staff.dptID, staff.roleID, staff.loginID, staff.psw, staff.defaultStation);

            int ret = add2List(&staff);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找人员操作失败！");
        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}

/**************************************************************************************************
功能：加载指定部门下所有人员列表
参数：dptID,部门ID
返回：-2，未初始化数据库连接
	 -3， 插入列表失败
	 -4，数据库操作失败
	 -5, 对应的dptID在部门表中不存在
	 0，成功
作者：李施施 2014-07-09
**************************************************************************************************/
int CStaffMgt::retrieve(const DWORD32 dptID)
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pStaffList, 0x00, m_sumCount * sizeof(TStaff));
	int loadCount = 0;
	int dptCount;
	m_count = 0;

	const char *selDptSql = "SELECT count(*) FROM t_department where ID=:f1_dptID<unsigned int>";
    const char *sql = "SELECT * FROM t_staff where dptID=:f1<unsigned int>";

	try
	{
		//查找对应部门ID的部门记录数量
		otl_stream selDptStream(1, selDptSql, m_db);
		selDptStream << dptID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selDptStream >> strCount;
            dptCount = atoi(strCount);
        }
        else
        {
            selDptStream >> dptCount;
        }
		if(dptCount <= 0)		return -5;

		//查找对应部门ID的人员记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << dptID;

		TStaff staff = {0};

		while (!selectStream.eof())
		{
			selectStream >> staff.ID;
			selectStream >> staff.name;
			selectStream >> staff.dptID;
			selectStream >> staff.roleID;
			selectStream >> staff.loginID;
			selectStream >> staff.psw;
			selectStream >> staff.defaultStation;
			loadCount++;

			printf("Load Staff<<ID: %d,name:%s,dptID:%d,roleID:%d,loginID:%d,psw:%s,default:%d\n", staff.ID, staff.name, staff.dptID, staff.roleID, staff.loginID, staff.psw, staff.defaultStation);

			int ret = add2List(&staff);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找人员操作失败！");
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
	  -6, 指定roleID在角色表中不存在
	  0，添加成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CStaffMgt::add( const TStaff *pInputParam)
{
	if (NULL == pInputParam )					return -1;

	if(!m_isInit)								return -2;

    if( checkDBConnect() < 0 )      return -4;

    TStaff temp = {0};
	memcpy(&temp.name, pInputParam->name, strlen(pInputParam->name));
	memcpy(&temp.psw, pInputParam->psw, strlen(pInputParam->psw));
	temp.dptID = pInputParam->dptID;
	temp.roleID = pInputParam->roleID;
	temp.loginID = pInputParam->loginID;
	temp.defaultStation = pInputParam->defaultStation;

	DWORD32 lastAddID;
	DWORD32 dptCount;
	DWORD32 roleCount;
	DWORD32 maxLoginID;
	DWORD32 count;
	const char *selDptSql= "SELECT count(*) FROM t_department where ID=:f1_dptID<unsigned int>";
	const char *selRoleSql= "SELECT count(*) FROM t_role where ID=:f1_roleID<unsigned int>";
	const char *sql = "insert into t_staff values(NULL,:f1_name<char[32]>,:f2_dptID<unsigned int>,:f3_roleID<unsigned int>,:f4_loginID<unsigned int>,:f5_psw<char[8]>,:f6_default<unsigned int>)";
	const char *updateSql = "update t_staff set loginID=:f4_loginID<unsigned int> where ID=:f4_ID<unsigned int>";
	const char *seluUniqyeSql = "select count(*) from t_staff where loginID=:f5_loginID<unsigned int>";

	try
	{
        m_db.commit();
		//查找对应部门ID的部门记录数
		otl_stream selDptStream(1, selDptSql, m_db);
		selDptStream.set_commit(0);
		selDptStream << pInputParam->dptID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selDptStream >> strCount;
            dptCount = atoi(strCount);
        }
        else
        {
            selDptStream >> dptCount;
        }
		if(dptCount <= 0)	return -5;

		//插入记录
		otl_stream insertStream(1,sql,m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->name;
		insertStream << pInputParam->dptID;
		insertStream << pInputParam->roleID;
		insertStream << pInputParam->loginID;
		insertStream << pInputParam->psw;
		insertStream << pInputParam->defaultStation;

		//查找刚插入记录的ID
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

		if(0 == pInputParam->loginID)
		{
			//判断是否存在以ID值为loginID的记录
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

			if(count > 0)
			{
				otl_stream selMaxStream(1,"SELECT MAX(loginID) + 1 FROM t_staff", m_db);
				selMaxStream.set_commit(0);
				selMaxStream >> maxLoginID;

				otl_stream upStream(1, "update t_staff set loginID=:f6_max<unsigned int> where ID=:f6_ID<unsigned int>", m_db);
				upStream.set_commit(0);
				upStream << maxLoginID;
				upStream << lastAddID;

				temp.loginID = maxLoginID;
			}
			else
			{
				otl_stream updateStream(1, updateSql, m_db);
				updateStream.set_commit(0);
				updateStream << lastAddID;
				updateStream << lastAddID;

				temp.loginID = lastAddID;
			}
		}

		m_db.commit();

		printf("数据库插入记录ID: %d,name:%s,dptID:%d,roleID:%d,loginID:%d,psw:%s,default:%d\n", temp.ID, temp.name, temp.dptID, temp.roleID, temp.loginID, temp.psw, temp.defaultStation);

		int ret = add2List(&temp);
		if(ret < 0)		return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("插入人员操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}

	return temp.ID;
}

/**************************************************************************************************
功能：删除部门记录
参数：pDepartment，添加的部门记录
返回：-2，数据库尚未连接
	  -3, 从部门记录列表中删除记录失败
	  -4， 数据库删除操作失败
	  0， 成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CStaffMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int flag = isIDExist(ID);
	if(-1 == flag)	return -5;

	const char *sql = "DELETE FROM t_staff WHERE ID = :f1<unsigned int>";
	
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
		WLOutDebug("删除人员操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：更新指定ID部门的记录
参数：ID，部门ID
      pDepartment，更新的部门信息
返回：-1,参数错误
	  -2, 数据库尚未连接
	  -3，该ID不存在
	  -4, 数据库操作失败
	  -5, 指定dptID在department表中不存在
	  -6, 指定roleID在角色表中不存在
	  -7, 待更新记录的loginID在表中已经存在
	  0，更新成功
作者：李施施 2014-07-09
**************************************************************************************************/
int CStaffMgt::update( const DWORD32 ID, TStaff *pStaff )
{
	if(NULL == pStaff)		return -1;

	if (!m_isInit)			return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(-1 == ret)			return -3;

	int dptCount;
	int roleCount;
	int loginIDCount;

	const char *selDptSql= "SELECT count(*) FROM t_department where ID=:f1_dptID<unsigned int>";
	const char *selRoleSql= "SELECT count(*) FROM t_role where ID=:f1_roleID<unsigned int>";
	const char *sql = "UPDATE t_staff SET name=:f1<char[32]>,dptID=:f2<unsigned int>,roleID=:f3<unsigned int>,loginID=:f4<unsigned int>,psw=:f5<char[8]>,defaultStation=:f6<unsigned int> WHERE ID=:f7<unsigned int>";

	try
	{
        m_db.commit();
		//查找对应部门ID的部门记录数
		otl_stream selDptStream(1, selDptSql, m_db);
		selDptStream << pStaff->dptID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selDptStream >> strCount;
            dptCount = atoi(strCount);
        }
        else
        {
            selDptStream >> dptCount;
        }
		if(dptCount <= 0)		return -5;

		//查找对应角色ID的角色记录数
//		otl_stream selRoleStream(1, selRoleSql, m_db);
//		selRoleStream << pStaff->roleID;
//		selRoleStream >> roleCount;
//		if(roleCount <= 0)		return -6;

		//更新记录
		otl_stream updateStream(1,sql,m_db);
		updateStream << pStaff->name;
		updateStream << pStaff->dptID;
		updateStream << pStaff->roleID;
		updateStream << pStaff->loginID;
		updateStream << pStaff->psw;
		updateStream << pStaff->defaultStation;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID, pStaff);
		if(ret < 0)		return -3;

		printf("数据库更新记录ID: %d,name:%s,dptID:%d,roleID:%d,loginID:%d,psw:%s,default:%d\n", pStaff->ID, pStaff->name, pStaff->dptID, pStaff->roleID, pStaff->loginID, pStaff->psw, pStaff->defaultStation);

	}catch(otl_exception& p)
	{
		WLOutDebug("更新人员操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：添加人员记录到部门记录列表
参数：pStaff，添加的人员记录
返回：-1，参数错误
	  -2， 重复记录
	  -3， 申请内存失败
	  0， 成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CStaffMgt::add2List( TStaff *pStaff )
{
	if(NULL == pStaff)		return -1;

	int ret = isIDExist(pStaff->ID);
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

	memcpy(&m_pStaffList[m_count], pStaff, sizeof(TStaff));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：判断人员ID是否存在
参数：ID，人员ID
返回：>0，该ID存在
	  -1，ID不存在
作者：李施施 2014-07-09
**************************************************************************************************/
int CStaffMgt::isIDExist( const DWORD32 ID )
{
	int index = -1;

	m_pCS->enter();
	for(int i=0; i<m_count; i++)
	{
		if(m_pStaffList[i].ID == ID)	index = i;
	}
	m_pCS->leave();

	return index;
}

/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：李施施 2014-07-09
**************************************************************************************************/
int CStaffMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_STAFF_COUNT;

	TStaff *pStaffList = new TStaff[sumCount];
	if (NULL == pStaffList)
	{
		WLOutDebug("getMoreMemory():CStaffMgt::CStaffMgt()-->申请内存失败!");
		return -1;
	}

	m_sumCount = sumCount;
	memset(pStaffList, 0x00, m_sumCount*sizeof(TStaff));
	memcpy(pStaffList, m_pStaffList, m_count*sizeof(TStaff));
	delete [] m_pStaffList;
	m_pStaffList = pStaffList;

	return 0;
}

/**************************************************************************************************
功能：从人员列表中删除指定ID的记录
参数：ID，人员ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：李施施 2014-07-09
**************************************************************************************************/
int CStaffMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(-1 == index)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pStaffList[i], &m_pStaffList[i+1], sizeof(TStaff));
	}
	memset(&m_pStaffList[m_count-1], 0x00, sizeof(TStaff));
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
int CStaffMgt::delFromList( const DWORD32 *IDList, DWORD32 count )
{
	if( NULL == IDList )		return -1;

	for(int i=0; i<count; i++)
	{
		int ret = delFromList(IDList[i]);
		if(ret < 0)				return -2;
	}

	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：李施施 2014-07-29
**************************************************************************************************/
DWORD32 CStaffMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TStaff * CStaffMgt::getListByIndex( DWORD32 index ) const
{
	if(index > m_count-1)	return NULL;

	return &m_pStaffList[index];
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
int CStaffMgt::updateList( const DWORD32 ID, const TStaff *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TStaff *temp = getListByIndex(index);
	memset(temp->name, 0x00, sizeof(temp->name));
	memcpy(temp->name, pInputParam->name, strlen(pInputParam->name));
	temp->roleID = pInputParam->roleID;
	temp->loginID = pInputParam->loginID;
	memset(temp->psw, 0x00, sizeof(temp->psw));
	memcpy(temp->psw, pInputParam->psw, strlen(pInputParam->psw));
	temp->defaultStation = pInputParam->defaultStation;

	return 0;
}


