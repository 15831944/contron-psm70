#include "stdafx.h"
#include "FunctionMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-14
**************************************************************************************************/
CFunctionMgt::CFunctionMgt()
{
	m_count = 0;
	m_sumCount = MAX_FUNCTION_COUNT;

	m_pFunctionList = new TFunction[m_sumCount];
	if(NULL == m_pFunctionList)
	{
		WLOutDebug("CFunctionMgt::CFunctionMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-14
**************************************************************************************************/
CFunctionMgt::~CFunctionMgt()
{
	if(NULL != m_pFunctionList)
	{
		delete [] m_pFunctionList;
		m_pFunctionList = NULL;
	}
}

/**************************************************************************************************
功能：加载所有功能系列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CFunctionMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    const char *sql = "SELECT * FROM t_function ORDER BY privilegeCode";
	int loadCount = 0;
	memset(m_pFunctionList, 0x00, sizeof(TFunction)*m_sumCount);
	m_count = 0;

	try
	{
		otl_stream selectStream(200, sql, m_db);

		TFunction temp = {0};
		while(!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.privilegeCode;
			selectStream >> temp.name;
			selectStream >> temp.UPID;
			selectStream >> temp.type;
			printf("Load TFunction << ID : %d,  privilegeCode : %d, name : %s, UPID: %d, type: %d \n", temp.ID, temp.privilegeCode, temp.name, temp.UPID, temp.type);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;

			loadCount++;
		}
        m_db.commit();
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找功能操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}

	return 0;
}

/**************************************************************************************************
功能：加载所有功能系列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：李施施 2014-07-30
**************************************************************************************************/
int CFunctionMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    const char *sql = "SELECT * FROM t_function where privilegeCode=:f1_pricode<unsigned int>";
	int loadCount = 0;
	memset(m_pFunctionList, 0x00, sizeof(TFunction)*m_sumCount);
	m_count = 0;

	try
	{
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TFunction temp = {0};
		while(!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.privilegeCode;
			selectStream >> temp.name;
			selectStream >> temp.UPID;
			selectStream >> temp.type;
			printf("Load TFunction << ID : %d,  privilegeCode : %d, name : %s, UPID: %d, type: %d \n", temp.ID, temp.privilegeCode, temp.name, temp.UPID, temp.type);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;

			loadCount++;
		}
        m_db.commit();
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找功能操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}

	return 0;
}

int CFunctionMgt::retrieveByUPID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    const char *sql = "SELECT * FROM t_function where UPID=:f1_pricode<unsigned int>";
	int loadCount = 0;
	memset(m_pFunctionList, 0x00, sizeof(TFunction)*m_sumCount);
	m_count = 0;

	try
	{
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TFunction temp = {0};
		while(!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.privilegeCode;
			selectStream >> temp.name;
			selectStream >> temp.UPID;
			selectStream >> temp.type;
			printf("Load TFunction << ID : %d,  privilegeCode : %d, name : %s, UPID: %d, type: %d \n", temp.ID, temp.privilegeCode, temp.name, temp.UPID, temp.type);

			int ret = add2List(&temp);
			if(ret < 0)		return -3;

			loadCount++;
		}
        m_db.commit();
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找功能操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}

	return 0;
}

/**************************************************************************************************
功能：添加功能记录
参数：privilegeCode，权限编码
	  pName，名称
	  UPID，父ID
	  type，类型；
返回：-2,数据库尚未连接
	  -3，添加到列表失败
	  -4,数据库添加操作失败
	  -6, 重复记录
      -7    功能编码已存在
	  0，添加成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CFunctionMgt::add( const TFunction *pInputPara)
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    if (isPrivilegeCodeExised(pInputPara->privilegeCode) >= 0)
    {
        return -7;
    }

	TFunction temp = {0};
	temp.privilegeCode = pInputPara->privilegeCode;
	memcpy(&temp.name, pInputPara->name, strlen(pInputPara->name));
	temp.UPID = pInputPara->UPID;
	temp.type = pInputPara->type;

	int codeCount;
	DWORD32 maxPriCode;
	const char *selCountSql = "select count(*) from t_function where privilegeCode=:f1_pCode<unsigned int>";
	const char *sql = "insert into t_function values(NULL,:f1_privilegeID<unsigned int>,:f2_name<char[32]>,:f3_UPID<unsigned int>, :f4_type<unsigned int>)";
	const char *updateSql = "update t_function set privilegeCode=:f3_code<unsigned int> where ID=:f3_ID<unsigned int>";
	const char *seluUniqyeSql = "select count(*) from t_function where privilegeCode=:f4_pric<unsigned int>";
	DWORD32 lastAddID;

	try
	{
		//判断数据库中是否已和待更新记录的privilegeCode相同的记录
		otl_stream selCountStream(1, selCountSql, m_db);
		selCountStream.set_commit(0);
		selCountStream << pInputPara->privilegeCode;
        if(1 == m_dbType)//SQLite数据库
        {
            char strCount[20] = {0};
            selCountStream >> strCount;
            codeCount = atoi(strCount);
        }
        else
        {
            selCountStream >> codeCount;
        }
		if(codeCount > 0)		return -6;

		//插入记录
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputPara->privilegeCode;
		insertStream << pInputPara->name;
		insertStream << pInputPara->UPID;
		insertStream << pInputPara->type;

		//获取刚插入记录的ID值
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
		if(0 == pInputPara->privilegeCode)
		{
			//判断是否存在以ID值为privilegeCode的记录
			otl_stream selUniqueStream(1, seluUniqyeSql, m_db);
			selUniqueStream.set_commit(0);
			selUniqueStream << lastAddID;
            if(1 == m_dbType)//SQLite数据库
            {
                char strCount[20] = {0};
                selUniqueStream >> strCount;
                codeCount = atoi(strCount);
            }
            else
            {
                selUniqueStream >> codeCount;
            }

			//如果存在，以最大privilegeCode数作为插入的privilegeCode的值，否则，用ID值作为privilegeCode的值
			if(codeCount > 0)
			{
				otl_stream selMaxStream(1,"SELECT MAX(privilegeCode) + 1 FROM t_function", m_db);
				selMaxStream.set_commit(0);
				selMaxStream >> maxPriCode;

				otl_stream upStream(1, "update t_function set privilegeCode=:f5_max<unsigned int> where ID=:f3_ID<unsigned int>", m_db);
				upStream.set_commit(0);
				upStream << maxPriCode;
				upStream << lastAddID;

				temp.privilegeCode = maxPriCode;
			}
			else
			{
				otl_stream updateStream(1, updateSql, m_db);
				updateStream.set_commit(0);
				updateStream << lastAddID;
				updateStream << lastAddID;

				temp.privilegeCode = lastAddID;
			}
		}

		m_db.commit();

		printf("数据库插入记录ID: %d,privilegeCode:%d,name:%s,UPID:%d,type:%d\n", temp.ID, temp.privilegeCode, temp.name, temp.UPID, temp.type);
		int ret = add2List(&temp);
		if(ret < 0)		return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("新增功能操作失败！");
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
参数：ID，删除的功能记录的ID
返回：-2，数据库尚未连接
	  -3, 从功能呢过记录列表中删除记录失败
	  -4， 数据库删除操作失败
	  -5，对应该ID的记录不存在
	  0， 成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CFunctionMgt::del( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    int index = isIDExist(ID);
	if(index < 0)		return -5;

	const char *sql = "DELETE FROM t_function WHERE ID = :f1_ID<unsigned int>";

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
		WLOutDebug("删除功能操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

int CFunctionMgt::forceDel( const DWORD32 ID, CPrivilegeMgt *pPrivlegeMgt //= NULL, CFuncRoleTemplateShip *pFunRoleShip)
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    int index = isIDExist(ID);
	if(index < 0)		return -5;

	const char *selPriCodeSql1 = "select privilegeCode from t_function where ID=:f1_ID<unsigned int>";
	const char *selIDSql = "SELECT ID FROM t_privilege WHERE privilegeCode=:f2_ID<unsigned int>";
	const char *selShipSql = "SELECT ID FROM t_privilegeroletemplaterelationship WHERE privilegeCode=:f3_ID<unsigned int>";
	const char *delPriSql = "delete from t_privilege where privilegeCode=:f4_privilegeCode<unsigned int>";
	const char *delShipSql = "DELETE FROM t_privilegeroletemplaterelationship WHERE privilegeCode=:f5_privilegeCode<unsigned int>";
	const char *sql = "delete from t_function where ID=:f6_ID<unsigned int>";

	DWORD32 listCount = MAX_PRIVILEGE_COUNT;
	DWORD32 count = 0;
	DWORD32 privilegeCode;

	DWORD32 listCount2 = MAX_FUNCROLETEMPLATESHIP_COUNT;
	DWORD32 count2 = 0;

	DWORD32 *priIDList = new DWORD32[listCount];
	if(NULL == priIDList)
	{
		WLOutDebug("CFunctionMgt::forceDel():new int 失败");
		return -6;
	}

	DWORD32 *shipIDList = new DWORD32[listCount2];
	if(NULL == priIDList)
	{
		WLOutDebug("CFunctionMgt::forceDel():new int 失败");
		return -7;
	}

	try
	{
		//查询对应ID的privilegeCode
		otl_stream selPriCodeSql(1, selPriCodeSql1, m_db);
		selPriCodeSql.set_commit(0);
		selPriCodeSql << ID;
		selPriCodeSql >> privilegeCode;

		//根据privilegeCode在权限表中找到所有ID列表
		otl_stream selIDStream(200, selIDSql, m_db);
		selIDStream.set_commit(0);
		selIDStream << privilegeCode;
		while (!selIDStream.eof())
		{
			if(count >= listCount)
			{
				int ret = getMoreMemory(&priIDList,count, listCount);
				if(ret < 0)
				{
					delete [] priIDList;
					delete [] shipIDList;
					priIDList = NULL;
					shipIDList = NULL;
					return -7;
				}
			}
			selIDStream >> priIDList[count];
			count++;
		}

		//根据privilegeCode在权限角色模板关联表中找到所有ID列表
		otl_stream selShipIDStream(200, selShipSql, m_db);
		selShipIDStream.set_commit(0);
		selShipIDStream << privilegeCode;
		while (!selShipIDStream.eof())
		{
			if(count2 >= listCount2)
			{
				int ret = getMoreMemory(&shipIDList,count2, listCount2);
				if(ret < 0)
				{
					delete [] priIDList;
					delete [] shipIDList;
					priIDList = NULL;
					shipIDList =NULL;
					return -8;
				}
			}
			selShipIDStream >> shipIDList[count2];
			count2++;
		}

		//删除对应privilegeCode的权限记录
		otl_stream delPriStream(1, delPriSql, m_db);
		delPriStream.set_commit(0);
		delPriStream << privilegeCode;

		//删除对应privilegeCode的权限角色模板关联记录
		otl_stream delShipStream(1, delShipSql, m_db);
		delShipStream.set_commit(0);
		delShipStream << privilegeCode;

		//删除对应ID的功能记录
		otl_stream delStream(1, sql, m_db);
		delStream.set_commit(0);
		delStream << ID;

		m_db.commit();

		//删除权限列表记录
		if(NULL != pPrivlegeMgt)
		{
			int ret = pPrivlegeMgt->delFromList(priIDList, count);
			if(ret < 0)
			{
				delete [] priIDList;
				delete [] shipIDList;
				priIDList = NULL;
				shipIDList =NULL;
				return -8;
			}
		}

		//删除权限角色关联列表记录
		if(NULL != pFunRoleShip)
		{
			int ret = pFunRoleShip->delFromList(shipIDList, count2);
			if(ret < 0)
			{
				delete [] priIDList;
				delete [] shipIDList;
				priIDList = NULL;
				shipIDList =NULL;
				return -8;
			}
		}

		//删除功能列表记录
		int ret = delFromList(ID);
		if(ret < 0)
		{
			delete [] priIDList;
			delete [] shipIDList;
			priIDList = NULL;
			shipIDList =NULL;
			return -8;
		}
	}
	catch (otl_exception& p)
	{
		WLOutDebug("强制删除功能操作失败！");
		m_db.rollback();
		delete [] priIDList;
		delete [] shipIDList;
		priIDList = NULL;
		shipIDList =NULL;

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}

	delete [] priIDList;
	delete [] shipIDList;
	priIDList = NULL;
	shipIDList =NULL;

	return 0;
}

/**************************************************************************************************
功能：更新指定ID功能的记录
参数：ID，部门ID
      pFunction，更新的功能信息
返回：-1,参数错误
	  -2, 数据库尚未连接
	  -3，该ID不存在
	  -4, 数据库操作失败
	  -5，部门ID不存在
	  -6,存在相同记录
      -7    功能编码已存在
	  0，更新成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CFunctionMgt::update( const DWORD32 ID, const TFunction *pFunction )
{
	if(NULL == pFunction)		return -1;

	if(!m_isInit)				return -2;

    if( checkDBConnect() < 0 )      return -4;

    int index= isIDExist(ID);
	if(index < 0)				return -5;

    if (isPrivilegeCodeExised(pFunction->privilegeCode) >= 0)
    {
        return -7;
    }

	const char *sql = "UPDATE t_function SET privilegeCode=:f1<unsigned int>,name=:f2<char[32]>,UPID=:f3<unsigned int>,type=:f4<unsigned int> WHERE ID=:f5<unsigned int>";

	try
	{
		otl_stream updateStream(1, sql, m_db);
		updateStream << pFunction->privilegeCode;
		updateStream << pFunction->name;
		updateStream << pFunction->UPID;
		updateStream << pFunction->type;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pFunction);
		if(ret < 0)		return -3;

		printf("数据库更新记录ID:ID:%d,privilegeCode:%d,name:%s,UPID=%d,type=%d\n", ID, pFunction->privilegeCode, pFunction->name,pFunction->UPID,pFunction->type);
	}
	catch (otl_exception& p)
	{
		WLOutDebug("更新功能操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
    return 0;
}

/**************************************************************************************************
功能：根据指定父ID获取记录列表
参数：upID，父ID
     pOutList，输出参数，记录列表
     inCount，pOutList分配的内存单元数
     outCount，输出参数，记录数量
返回：<0,失败；-1,参数错误
             -2, 分配内存不足
     >=0，成功
作者：刘远安
注意：调用本函数前必须先调用retrieve()函数
**************************************************************************************************/
int CFunctionMgt::getRecordListByUpID(const DWORD32 upID, TFunction *pOutList, int inCount, int &outCount)
{
    int ret = -1;
    if((NULL == pOutList) || (inCount <= 0))    return ret;

    outCount = 0;
    m_pCS->enter();

    ret--;
    for(int i=0;i<m_count;i++)
    {
        if(m_pFunctionList[i].UPID == upID)
        {
            if(outCount >= inCount)
            {
                m_pCS->leave();
                return ret;
            }
            memcpy(&pOutList[outCount],&m_pFunctionList[i],sizeof(TFunction));
            outCount++;
        }
    }

    m_pCS->leave();
    return 0;
}

/**************************************************************************************************
功能：添加功能记录到功能记录列表
参数：pFunction，添加的功能记录
返回：-1，参数错误
	  -2， 重复记录
	  -3， 申请内存失败
	  0， 成功
作者：李施施 2014-07-14
**************************************************************************************************/
int CFunctionMgt::add2List( const TFunction *pFunction )
{
	if(NULL == pFunction)	return -1;

	int index = isIDExist(pFunction->ID);
	if(index >= 0)			return -2;

	m_pCS->enter();

	if (m_count >= m_sumCount)
	{
		int ret = getMoreMemory();
		if(ret < 0)	
		{
			m_pCS->leave();
			return -3;
		}
	}
	memcpy(&m_pFunctionList[m_count], pFunction, sizeof(TFunction));
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
int CFunctionMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pFunctionList[i], &m_pFunctionList[i+1], sizeof(TFunction));
	}
	memset(&m_pFunctionList[m_count-1], 0x00, sizeof(TFunction));
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
int CFunctionMgt::isIDExist( const DWORD32 ID )
{
	int index = -1;

	m_pCS->enter();
	for(int i=0; i<m_count; i++)
	{
		if(m_pFunctionList[i].ID == ID)	index = i;
	}
	m_pCS->leave();

	return index;

}

/**************************************************************************************************
功能：判断功能编码是否存在
参数：Code     功能编码
返回：>0，该功能编码存在
      -1，功能编码不存在
作者：佘佳明 2014-12-15
**************************************************************************************************/
int CFunctionMgt::isPrivilegeCodeExised(const DWORD32 Code)
{
    int index = -1;

    m_pCS->enter();
    for(int i=0; i<m_count; i++)
    {
        if(m_pFunctionList[i].privilegeCode == Code)	index = i;
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
int CFunctionMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_FUNCTION_COUNT;
	TFunction *pFunction = new TFunction[sumCount];

	if(NULL == pFunction)
	{
		WLOutDebug("getMoreMemory():CFunctionMgt::CFunctionMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pFunction, 0x00, sizeof(TFunction)*m_sumCount);
	memcpy(pFunction, m_pFunctionList, sizeof(TFunction)*m_count);
	delete [] m_pFunctionList;

	m_pFunctionList = pFunction;
	return 0;
}

/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：李施施 2014-07-16
**************************************************************************************************/
int CFunctionMgt::getMoreMemory( DWORD32 **pIDList, DWORD32 count, DWORD32 &listCount)
{
	if(NULL == pIDList || count <= 0 || listCount <= 0)		return -1;

	int tempCount = listCount + MAX_PRIVILEGE_COUNT;
	DWORD32 *tempList = new DWORD32[tempCount];
	if (NULL == tempList)
	{
		WLOutDebug("CFunctionMgt::forceDel new int2 Error!");
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
DWORD32 CFunctionMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TFunction * CFunctionMgt::getListByIndex( DWORD32 index ) const
{
	if(index > m_count-1)	return NULL;

	return &m_pFunctionList[index];
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
int CFunctionMgt::updateList( const DWORD32 ID, const TFunction *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TFunction *temp = getListByIndex(index);
	temp->privilegeCode = pInputParam->privilegeCode;
	memset(temp->name, 0x00, sizeof(temp->name));
	memcpy(temp->name, pInputParam->name, strlen(pInputParam->name));
	temp->UPID = pInputParam->UPID;
	temp->type = pInputParam->type;

	return 0;
}

