#include "stdafx.h"
#include "FuncRoleTemplateShip.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-15
**************************************************************************************************/
CFuncRoleTemplateShip::CFuncRoleTemplateShip()
{
	m_count = 0;
	m_sumCount = MAX_FUNCROLETEMPLATESHIP_COUNT;

	m_pFuncRoleTempList = new TFuncRoleTemplateShip[m_sumCount];
	if (NULL == m_pFuncRoleTempList)
	{
		WLOutDebug("CFuncRoleTemplateShip::CFuncRoleTemplateShip()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-15
**************************************************************************************************/
CFuncRoleTemplateShip::~CFuncRoleTemplateShip()
{
	if(NULL != m_pFuncRoleTempList)
	{
		delete [] m_pFuncRoleTempList;
		m_pFuncRoleTempList = NULL;
    }
}

void CFuncRoleTemplateShip::disConnect()
{
    return m_db.logoff(); // disconnect from Oracle
}

/**************************************************************************************************
功能：加载所有权限与角色模板关系列表
参数：无
返回：-2，未初始化数据库连接
	  -3，插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CFuncRoleTemplateShip::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    int loadCount = 0;
    const char *sql = "SELECT * FROM t_privilegeroletemplaterelationship";
	memset(m_pFuncRoleTempList, 0x00, sizeof(TFuncRoleTemplateShip)*m_sumCount);
	m_count = 0;

	try
	{
		otl_stream selStream(1, sql, m_db);

		TFuncRoleTemplateShip temp = {0};
		while(!selStream.eof())
		{
			selStream >> temp.ID;
			selStream >> temp.roleTemplateID;
			selStream >> temp.privilegeCode;

			printf("Load TFuncRoleTemplateShip << ID : %d, roleTemplateID : %d, privilegeCode : %d \n", temp.ID, temp.roleTemplateID, temp.privilegeCode);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;

			loadCount++;
		}
        m_db.commit();
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找权限角色模板关系操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：加载特定权限ID的权限与角色模板关系记录
参数：无
返回：-2，未初始化数据库连接
	  -3，插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-30
**************************************************************************************************/
int CFuncRoleTemplateShip::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    int loadCount = 0;
    const char *sql = "SELECT * FROM t_privilegeroletemplaterelationship where roleTemplateID=:f1_roleID<unsigned int>";
	memset(m_pFuncRoleTempList, 0x00, sizeof(TFuncRoleTemplateShip)*m_sumCount);
	m_count = 0;

	try
	{
		otl_stream selStream(1, sql, m_db);
		selStream << ID;

		TFuncRoleTemplateShip temp = {0};
		while(!selStream.eof())
		{
			selStream >> temp.ID;
			selStream >> temp.roleTemplateID;
			selStream >> temp.privilegeCode;

			printf("Load TFuncRoleTemplateShip << ID : %d, roleTemplateID : %d, privilegeCode : %d \n", temp.ID, temp.roleTemplateID, temp.privilegeCode);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;

			loadCount++;
		}

        m_db.commit();
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找权限角色模板关系操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：添加权限与角色模板关系记录
参数：roleTemplateID，角色模板ID
	  privilegeCode，权限编码
返回：-1,传入参数不合法
	  -2,数据库尚未连接
	  -3，添加到列表失败
	  -4,数据库添加操作失败
	  -5，指定角色模板ID不存在
	  -6，指定权限编码不存在
	  0，添加成功
作者：李施施 2014-07-15
**************************************************************************************************/
int CFuncRoleTemplateShip::add( const TFuncRoleTemplateShip *pInputParam )
{
	if(NULL == pInputParam )	return -1;

	if(!m_isInit)				return -2;

    if( checkDBConnect() < 0 )      return -4;

    TFuncRoleTemplateShip temp = {0};
	temp.roleTemplateID = pInputParam->roleTemplateID;
	temp.privilegeCode = pInputParam->privilegeCode;
    WLOutDebug("roleTemplateID=%d,privilegeCode=%d",pInputParam->roleTemplateID,pInputParam->privilegeCode);

	int selectRoleCount ;
	int selectPriCount;
	int selCount;
	int lastAddID;
    const char *selectRoleSql = "SELECT count(*) FROM t_roletemplate where ID=:f1_roleTemplateID<unsigned int>";
	const char *selectPriSql = "SELECT count(*) FROM t_function where privilegeCode=:f1_privilegeCode<unsigned int>";
	const char *selUniqueSql = "select count(*) from t_privilegeroletemplaterelationship where roleTemplateID=:f2_role<unsigned int> and privilegeCode=:f2_pric<unsigned int>";
	const char *sql = "insert into t_privilegeroletemplaterelationship values(NULL,:f1_roleTemplateID<unsigned int>,:f1_privilegeCode<unsigned int>)";

	try
	{
        otl_stream selUniqueStream(1, selUniqueSql, m_db);
        selUniqueStream.set_commit(0);
        selUniqueStream << pInputParam->privilegeCode;
        selUniqueStream << pInputParam->roleTemplateID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selUniqueStream >> strCount;
            selCount = atoi(strCount);
        }
        else
        {
            selUniqueStream >> selCount;
        }
		//

		otl_stream selRoleStream(1, selectRoleSql, m_db);
        selRoleStream.set_commit(0);
		selRoleStream << pInputParam->roleTemplateID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selRoleStream >> strCount;
            selectRoleCount = atoi(strCount);
        }
        else
        {
            selRoleStream >> selectRoleCount;
        }
        WLOutDebug("selectRoleCount=%d",selectRoleCount);
        if(selectRoleCount <= 0)	return -5;

		otl_stream selPriStream(1, selectPriSql, m_db);
        selPriStream.set_commit(0);
		selPriStream << pInputParam->privilegeCode;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selPriStream >> strCount;
            selectPriCount = atoi(strCount);
        }
        else
        {
            selPriStream >> selectPriCount;
        }
        if(selectPriCount <= 0)		return -6;

        otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->roleTemplateID;
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
            selectStream.set_commit(0);
            selectStream >> lastAddID;
            temp.ID = lastAddID;
        }

		m_db.commit();

		printf("数据库插入记录ID: %d,roleTemplateID:%d,privilegeCode:%d\n", temp.ID, temp.roleTemplateID, temp.privilegeCode);
		int ret = add2List(&temp);
		if(ret < 0)					return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("添加权限角色模板关系操作失败！");
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
int CFuncRoleTemplateShip::insertBatch(const TFuncRoleTemplateShip *pList, int count)
{
    int ret = -1;
    if((NULL == pList) || (count <= 0))	return ret;

    ret--;
    if(!m_isInit)   return ret;

    if( checkDBConnect() < 0 )      return -4;

    int lastAddID;
    const char *sql = "insert into t_privilegeroletemplaterelationship values(NULL,:f1_roleTemplateID<unsigned int>,:f1_privilegeCode<unsigned int>)";

    ret--;
    try
    {
        otl_stream insertStream(count,sql,m_db);
        insertStream.set_commit(0);
        for(int i=0;i<count;i++)
        {
            insertStream << pList[i].roleTemplateID;
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
功能：删除权限与角色模板关系记录
参数：ID，删除的记录的ID
返回：-2，数据库尚未连接
	  -3, 从列表中删除记录失败
	  -4， 数据库删除操作失败
	  -5，对应该ID的记录不存在
	  0， 成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CFuncRoleTemplateShip::del( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    int index = isIDExist(ID);
	if(index < 0)		return -5;

	const char *sql = "DELETE FROM t_privilegeroletemplaterelationship WHERE ID = :f1_ID<unsigned int>";
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
功能：更新指定ID部门资源关系的记录
参数：ID，部门ID
      pDptResourceShip，更新的部门资源关系信息
返回：-1,参数错误
	  -2, 数据库尚未连接
	  -4, 数据库操作失败
	  -5，该ID不存在
	  -6， 部门ID不存在
	  -7，资源ID不存在
	  0，更新成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CFuncRoleTemplateShip::update( const DWORD32 ID, const TFuncRoleTemplateShip *pFuncRoleTemplateShip )
{
	if(NULL == pFuncRoleTemplateShip)		return -1;

	if(!m_isInit)							return -2;

    if( checkDBConnect() < 0 )              return -4;

    int index = isIDExist(ID);
	if(index < 0)							return -5;

	int selectRoleCount ;
	int selectPriCount;
	const char *selectRoleSql = "SELECT count(*) FROM t_roletemplate where ID=:f1_roleTemplateID<unsigned int>";
	const char *selectPriSql = "SELECT count(*) FROM t_function where privilegeCode=:f1_privilegeCode<unsigned int>";
	const char *sql = "UPDATE t_privilegeroletemplaterelationship SET roleTemplateID=:f1<unsigned int>,privilegeCode=:f2<unsigned int> WHERE ID=:f3<unsigned int>";
	try
	{
		otl_stream selDptStream(1, selectRoleSql, m_db);
		selDptStream << pFuncRoleTemplateShip->roleTemplateID;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selDptStream >> strCount;
            selectRoleCount = atoi(strCount);
        }
        else
        {
            selDptStream >> selectRoleCount;
        }
		if(selectRoleCount <= 0)			return	-6;

		otl_stream selRecStream(1, selectPriSql, m_db);
		selRecStream << pFuncRoleTemplateShip->privilegeCode;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selRecStream >> strCount;
            selectPriCount = atoi(strCount);
        }
        else
        {
            selRecStream >> selectPriCount;
        }
		if(selectPriCount <= 0)				return -7;

		otl_stream updateStream(1, sql, m_db);
		updateStream << pFuncRoleTemplateShip->roleTemplateID;
		updateStream << pFuncRoleTemplateShip->privilegeCode;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID, pFuncRoleTemplateShip);
		if(ret < 0)		return -3;

		printf("数据库更新记录ID:ID:%d,roleTemplateID:%d,ID:%d\n", ID, pFuncRoleTemplateShip->roleTemplateID, pFuncRoleTemplateShip->privilegeCode);

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
功能：添加到列表
参数：pRoleTemplate，添加的记录
返回：-1，参数错误
	  -2， 重复记录
	  -3， 申请内存失败
	  0， 成功
作者：李施施 2014-07-15
**************************************************************************************************/
int CFuncRoleTemplateShip::add2List( const TFuncRoleTemplateShip *pRoleTemplate )
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
	memcpy(&m_pFuncRoleTempList[m_count], pRoleTemplate, sizeof(TFuncRoleTemplateShip));
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
int CFuncRoleTemplateShip::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pFuncRoleTempList[i], &m_pFuncRoleTempList[i+1], sizeof(TFuncRoleTemplateShip));
	}
	memset(&m_pFuncRoleTempList[m_count-1], 0x00, sizeof(TFuncRoleTemplateShip));
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
int CFuncRoleTemplateShip::delFromList( const DWORD32 *IDList, DWORD32 count )
{
	if(NULL == IDList || count < 0)		return -1;

	for(int i=0; i<count; i++)
	{
		int ret = delFromList(IDList[i]);
		if(ret < 0)						return -2;
	}

	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：李施施 2014-07-15
**************************************************************************************************/
int CFuncRoleTemplateShip::isIDExist( const DWORD32 ID )
{
	int index = -1;

	m_pCS->enter();
	for(int i=0; i<m_count; i++)
	{
		if(m_pFuncRoleTempList[i].ID == ID)	index = i;
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
int CFuncRoleTemplateShip::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_FUNCROLETEMPLATESHIP_COUNT;
	TFuncRoleTemplateShip *pTempList = new TFuncRoleTemplateShip[sumCount];
	if(NULL == pTempList)
	{
		WLOutDebug("getMoreMemory():CFuncRoleTemplateShip::CFuncRoleTemplateShip()-->申请内存失败!");
		return -1;
	}

	m_sumCount = sumCount;
	memset(pTempList, 0x00, sizeof(TFuncRoleTemplateShip)*m_sumCount);
	memcpy(pTempList, m_pFuncRoleTempList, sizeof(TFuncRoleTemplateShip)*m_count);
	delete [] m_pFuncRoleTempList;

	m_pFuncRoleTempList = pTempList;
	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：李施施 2014-07-29
**************************************************************************************************/
DWORD32 CFuncRoleTemplateShip::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TFuncRoleTemplateShip * CFuncRoleTemplateShip::getListByIndex( DWORD32 index ) const
{
	if(index > m_count-1)		return NULL;

	return &m_pFuncRoleTempList[index];
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
int CFuncRoleTemplateShip::updateList( const DWORD32 ID, const TFuncRoleTemplateShip *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TFuncRoleTemplateShip *temp = getListByIndex(index);
	temp->roleTemplateID = pInputParam->roleTemplateID;
	temp->privilegeCode = pInputParam->privilegeCode;

	return 0;
}

