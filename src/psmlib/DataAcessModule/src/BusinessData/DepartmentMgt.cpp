#include "stdafx.h"
#include "DepartmentMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-08
**************************************************************************************************/
CDepartmentMgt::CDepartmentMgt()
{
	m_Count = 0;
	m_sumCount = MAX_DEPARTMENT_COUNT;

	m_pDepartmentList = new TDepartment[m_sumCount];
	if(NULL == m_pDepartmentList)
	{
		printf("CDepartmentMgt::CDepartmentMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-08
**************************************************************************************************/
CDepartmentMgt::~CDepartmentMgt()
{
	if(NULL != m_pDepartmentList)
	{
		delete [] m_pDepartmentList;
		m_pDepartmentList = NULL;
	}
}

/**************************************************************************************************
功能：加载全部部门列表
参数：无
返回：-2，数据库尚未连接
	  -3，添加列表失败
	  -4，查找操作失败
	  0，查找成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CDepartmentMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    const char *sql = "select * from t_department";
	int loadCount = 0;
	memset(m_pDepartmentList, 0x00, m_sumCount * sizeof(TDepartment));
	m_Count = 0;

	try
	{
        m_db.commit();
		otl_stream selectStream(200, sql, m_db);

		TDepartment departmentTemp = {0};
		while (!selectStream.eof())
		{
			selectStream >> departmentTemp.ID;
			selectStream >> departmentTemp.name;
			selectStream >> departmentTemp.UPID;
			selectStream >> departmentTemp.type;

			printf("Load TDepartment << ID : %d,  name : %s, UPID : %d, type : %d\n", departmentTemp.ID, departmentTemp.name, departmentTemp.UPID, departmentTemp.type);

			int ret = add2List(&departmentTemp);
			if(ret < 0)		return -3;
			loadCount++;
		}

        m_db.commit();
		printf("共 %d 条记录\n\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找部门操作失败！");
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
      UPID，上级部门ID；
	  type，类型
返回：-1,参数错误
	  -2,数据库尚未连接
	  -3，添加到列表失败
	  -4,数据库添加操作失败
	  0，添加成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CDepartmentMgt::add( const char *pName, const DWORD32 UPID, const DWORD32 type )
{
	if(NULL == pName)	return -1;

	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    TDepartment temp = {0};
	memcpy(&temp.name, pName, strlen(pName));
	temp.UPID = UPID;
	temp.type = type;

	int lastAddID;

	const char *sql = "insert into t_department values(NULL,:f1_name<char[32]>,:f2_UPID<unsigned int>,:f3_type<unsigned int>)";

	try
	{
		otl_stream insertStream(1,sql, m_db);
		insertStream.set_commit(0);
		insertStream << pName;
		insertStream << UPID;
		insertStream << type;

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

		printf("数据库插入记录 ID: %d, name: %s, UPID：%d, type：%d\n", lastAddID, temp.name, temp.UPID, temp.type);

		int ret = add2List(&temp);
		if(ret < 0)		return -3;
	
	}
	catch(otl_exception& p)
	{
		WLOutDebug("新增部门操作失败！");
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
功能：删除部门记录
参数：pDepartment，添加的部门记录
返回：-1，数据库尚未连接
	  -2，对应该ID的部门记录不存在
	  -3, 从部门记录列表中删除记录失败
	  -4， 数据库删除操作失败
	  0， 成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CDepartmentMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -1;

    if( checkDBConnect() < 0 )      return -4;

    int flag = isIDExist(ID);
	if(-1 == flag)	return -2;

	const char *deleteSql = "DELETE FROM t_department WHERE ID = :f1<unsigned int>";

	try
	{
		otl_stream deleteStream(1, deleteSql, m_db);
		deleteStream << ID;

        m_db.commit();
		printf("数据库删除记录 ID: %d\n", ID);

		int ret = delFromList(ID);
		if(-1 == ret)	return -3;
	}catch(otl_exception& p)
	{
		WLOutDebug("删除部门操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：强制删除部门记录，适用于部门下有员工的情况，将员工和部门一次性删除
参数：ID，部门ID
	  pStaffMgt， 员工对象，默认为NULL
返回：-2，数据库尚未连接
	  -3，从部门记录列表中删除记录失败
	  -4， 数据库删除操作失败
	  -5，ID不存在
	  -6，申请内存失败
	  -7，申请内存失败（方便定位）
	  -8，申请更大内存失败
	  -9，申请更大内存失败
	  -10，删除人员列表失败
	  -11，删除部门资源对应关系列表失败
	  0， 成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CDepartmentMgt::forceDel( const DWORD32 ID, CStaffMgt *pStaffMgt, CDptResourceShip *pDptRecShip)
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int index = isIDExist(ID);
	if(-1 == index)	return -5;

	const char *deleteSql = "DELETE FROM t_department WHERE ID = :f1<unsigned int>";							//删除部门表中对应ID记录
	const char *delStaffSql = "DELETE FROM t_staff WHERE dptID = :f2<unsigned int>";							//删除人员表对应部门ID的记录
	const char *staffSql =  "SELECT ID FROM t_staff WHERE dptID = :f3<unsigned int>";							//找到人员表中对应部门ID的所有ID
	const char *dptRecShipSql =  "SELECT ID FROM t_dptresourcerelationship WHERE dptID = :f4<unsigned int>";	//找到部门资源对应关系表中对应部门ID的所有ID
	const char *delDptRecShipSql = "DELETE FROM t_dptresourcerelationship WHERE dptID = :f5<unsigned int>";		//删除部门资源对应关系表中对应部门ID的记录

	DWORD32 count = 0;									//记录对应部门ID下的人员数量
	DWORD32 listCount = MAX_STAFF_COUNT;				//用来记录对应部门ID下的人员数量的数组大小
	DWORD32 listCount2 = MAX_DPTRESOURCESHIP_COUNT;		//记录对应部门ID的部门资源关系列表记录数量
	DWORD32 count2 = 0;									//记录对应部门ID的部门资源关系记录数

	DWORD32 *pStaffIDList = new DWORD32[listCount];		//记录对应部门ID下的人员ID
	if(NULL == pStaffIDList)
	{
		WLOutDebug("CDepartmentMgt::forceDel new int1 Error!");
		return -6;
	}

	DWORD32 *pDptResShipIDList = new DWORD32[listCount2];
	if(NULL == pDptResShipIDList)
	{
		WLOutDebug("CDepartmentMgt::forceDel new int3 Error!");
		delete [] pStaffIDList;
		pStaffIDList = NULL;
		return -7;
	}

	try
	{
		//查找出对应dptID下的所有staffID
		otl_stream staffStream(1, staffSql, m_db);
		staffStream.set_commit(0);
		staffStream << ID;

		while(!staffStream.eof())
		{
			//内存不够，重新分配空间
			if(count >= listCount)
			{
				int ret = getMoreMemory(&pStaffIDList,count, listCount);
				if(ret < 0)
				{
					delete [] pStaffIDList;
					pStaffIDList = NULL;
					return -8;
				}
			}

			staffStream >> pStaffIDList[count];
			count++;
		}

		//删除对应dptID下的所有staff
		otl_stream delStaff(1, delStaffSql, m_db);
		delStaff.set_commit(0);
		delStaff << ID;

		//查找对应depID下的所有部门资源关系ID
		otl_stream shipStream(1, dptRecShipSql, m_db);
		shipStream.set_commit(0);
		shipStream << ID;

		while(!shipStream.eof())
		{
			//内存不够，重新分配空间
			if(count2 >= listCount2)
			{
				int ret = getMoreMemory(&pDptResShipIDList,count2, listCount2);
				if(ret < 0)
				{
					delete [] pStaffIDList;
					delete [] pDptResShipIDList;
					pStaffIDList = NULL;
					pDptResShipIDList = NULL;
					return -9;
				}
			}

			shipStream >> pDptResShipIDList[count2];
			count2++;
		}

		//删除对应dptID下的所有部门资源关系记录
		otl_stream delShip(1, delDptRecShipSql, m_db);
		delShip.set_commit(0);
		delShip << ID;

		//删除对应department
		otl_stream deleteStream(1, deleteSql, m_db);
		deleteStream.set_commit(0);
		deleteStream << ID;

		m_db.commit();

		printf("数据库删除记录 ID: %d\n", ID);

		//删除部门列表
		int ret = delFromList(ID);
		if(-1 == ret)
		{
			delete [] pStaffIDList;
			delete [] pDptResShipIDList;
			pStaffIDList = NULL;
			pDptResShipIDList = NULL;
			return -3;
		}

		//删除人员列表
		if(pStaffMgt != NULL)
		{
			ret = pStaffMgt->delFromList(pStaffIDList, count);
			if (ret < 0)	
			{
				delete [] pStaffIDList;
				delete [] pDptResShipIDList;
				pStaffIDList = NULL;
				pDptResShipIDList = NULL;
				return -10;
			}
		}

		//删除部门资源对应关系列表
		if(pDptRecShip != NULL)
		{
			ret = pDptRecShip->delFromList(pDptResShipIDList, count2);
			if (ret < 0)	
			{
				delete [] pStaffIDList;
				delete [] pDptResShipIDList;
				pStaffIDList = NULL;
				pDptResShipIDList = NULL;
				return -11;
			}
		}

	}catch(otl_exception& p)
	{
		WLOutDebug("删除部门操作失败！");
		m_db.rollback();			//事务回滚
		delete [] pStaffIDList;		//释放空间
		delete [] pDptResShipIDList;//释放空间
		pStaffIDList = NULL;
		pDptResShipIDList = NULL;

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	delete [] pStaffIDList;		//释放空间
	delete [] pDptResShipIDList;//释放空间
	pStaffIDList = NULL;
	pDptResShipIDList = NULL;

	return 0;
}

/**************************************************************************************************
功能：更新指定ID部门的记录
参数：ID，部门ID
      pDepartment，更新的部门信息
返回：-1,参数错误
	  -2, 数据库尚未连接
	  -3，该ID不存在
	  -4, 更新失败
	  0，更新成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CDepartmentMgt::update( const DWORD32 ID, TDepartment *pDepartment )
{
	if(NULL == pDepartment)		return -1;

	if(!m_isInit)				return -2;

    if( checkDBConnect() < 0 )      return -4;

    int flag = isIDExist(ID);
	if(-1 == flag)					return -2;

	const char *sql = "UPDATE t_department SET name=:f1<char[32]>,UPID=:f2<unsigned int>,type=:f3<unsigned int> WHERE ID=:f4<unsigned int>";

	try
	{
		otl_stream updateStream(1,sql, m_db);

		updateStream << pDepartment->name;
		updateStream << pDepartment->UPID;
		updateStream << pDepartment->type;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID, pDepartment);
		if(ret < 0)		return -2;
		printf("数据库更新记录 ID: %d, name: %s, UPID：%d, type：%d\n", ID, pDepartment->name, pDepartment->UPID, pDepartment->type);
	}
	catch(otl_exception& p)
	{
		WLOutDebug("更新部门操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -3;
	}

	return 0;
}

/**************************************************************************************************
功能：添加部门记录到部门记录列表
参数：pDepartment，添加的部门记录
返回：-1，参数错误
	  -2， 重复记录
	  -3， 申请内存失败
	  0， 成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CDepartmentMgt::add2List( TDepartment *pDepartment )
{
	if(NULL == pDepartment)							return -1;

	int ret = isIDExist(pDepartment->ID);
	if(ret >= 0)									return -2;

	m_pCS->enter();

	if (m_Count >= m_sumCount)
	{
		int ret = getMoreMemory();
		if (ret < 0)
		{
			m_pCS->leave();
			return -3;
		}
	}

	memcpy(&m_pDepartmentList[m_Count], pDepartment, sizeof(TDepartment));
	m_Count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：判断部门ID是否存在
参数：ID，部门ID
返回：>0，该ID存在
	  -1，ID不存在
作者：李施施 2014-07-08
**************************************************************************************************/
int CDepartmentMgt::isIDExist( const DWORD32 ID )
{
	int flag = -1;

	m_pCS->enter();
	for(int i=0; i<m_Count; i++)
	{
		if(m_pDepartmentList[i].ID == ID)	flag = i;
	}
	m_pCS->leave();

	return flag;
}

/**************************************************************************************************
功能：从部门列表中删除指定ID的记录
参数：ID，部门ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：李施施 2014-07-08
**************************************************************************************************/
int CDepartmentMgt::delFromList( const DWORD32 ID )
{
	int flag = isIDExist(ID);
	if(-1 == flag)		return -1;

	m_pCS->enter();

	for(int i=flag; i<m_Count-1; i++)
	{
		memcpy(&m_pDepartmentList[i], &m_pDepartmentList[i+1], sizeof(TDepartment));
	}
	memset(&m_pDepartmentList[m_Count-1], 0x00, sizeof(TDepartment));
	m_Count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：李施施 2014-07-09
**************************************************************************************************/
int CDepartmentMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_DEPARTMENT_COUNT;

	TDepartment *pDepartment = new TDepartment[sumCount];
	if(NULL == pDepartment)
	{
		printf("getMoreMemory():CDepartmentMgt::CDepartmentMgt()-->申请内存失败!\n");
		return -1;
	}

	m_sumCount = sumCount;
	memset(pDepartment, 0x00, sizeof(TDepartment) * m_sumCount);
	memcpy(pDepartment, m_pDepartmentList, sizeof(TDepartment) * m_Count);
	delete [] m_pDepartmentList;
	m_pDepartmentList = pDepartment;

	return 0;
}

/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：李施施 2014-07-16
**************************************************************************************************/
int CDepartmentMgt::getMoreMemory( DWORD32 **pIDList, DWORD32 count, DWORD32 &listCount)
{
	if(NULL == pIDList || count <= 0 || listCount <= 0)		return -1;

	DWORD32 tempCount = listCount + MAX_DEPARTMENT_COUNT;
	DWORD32 *tempList = new DWORD32[tempCount];
	if (NULL == tempList)
	{
		WLOutDebug("CDepartmentMgt::forceDel new int2 Error!");
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
DWORD32 CDepartmentMgt::getListCount() const
{
	return m_Count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TDepartment * CDepartmentMgt::getListByIndex( DWORD32 index ) const
{
	if(index > m_Count-1)	return NULL;

	return &m_pDepartmentList[index];
}

/**************************************************************************************************
功能：更新本地列表记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-30
**************************************************************************************************/
int CDepartmentMgt::updateList( const DWORD32 ID, const TDepartment *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TDepartment *temp = getListByIndex(index);
	memset(temp->name, 0x00, sizeof(temp->name));
	memcpy(temp->name, pInputParam->name, strlen(pInputParam->name));
	temp->type = pInputParam->type;
	temp->UPID = pInputParam->UPID;

	return 0;
}
