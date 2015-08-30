#include "stdafx.h"
#include "HistoryTicketActionListMgt.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：佘佳明 2014-11-24
**************************************************************************************************/
CHistoryTicketActionListMgt::CHistoryTicketActionListMgt()
{
	m_count = 0;
    m_sumCount = MAX_HISTORYTICKETACTIONLIST_COUNT;

	m_pList = new TTicketActionList[m_sumCount];
	if(NULL == m_pList)
	{
		WLOutDebug("COperateTicketMgt::COperateTicketMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：佘佳明 2014-11-24
**************************************************************************************************/
CHistoryTicketActionListMgt::~CHistoryTicketActionListMgt()
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
作者：佘佳明 2014-11-24
**************************************************************************************************/
int CHistoryTicketActionListMgt::retrieve(  )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TTicketActionList));
	int loadCount = 0;
	m_count = 0;

    char *sql = (char*)"SELECT * FROM t_historyticketactionlist ORDER BY `index`";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

		TTicketActionList temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.ticketID;
			selectStream >> temp.index;
			selectStream >> temp.deviceID;
			selectStream >> temp.isCompleted;
			selectStream >> temp.conditions;
			selectStream >> temp.action;
			selectStream >> temp.beforeState;
			selectStream >> temp.afterState;
			selectStream >> temp.operateTime;
			selectStream >> temp.isMonitor;
            selectStream >> temp.isTicketItem;
            selectStream >> temp.isKeyItem;
            selectStream >> temp.keyItemIndex;
            selectStream >> temp.isLinkItem;
            selectStream >> temp.linkItemIndex;
            selectStream >> temp.actionType;
            selectStream >> temp.WBID;
            selectStream >> temp.lockCode;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
		}
        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找操作动作票操作失败！");
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
作者：佘佳明 2014-11-24
**************************************************************************************************/
int CHistoryTicketActionListMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TTicketActionList));
	int loadCount = 0;
	m_count = 0;
    char *sql = (char*)"SELECT * FROM t_historyticketactionlist where ticketID=:f<unsigned int>  ORDER BY `index`";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream <<ID;

		TTicketActionList temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.ticketID;
			selectStream >> temp.index;
			selectStream >> temp.deviceID;
			selectStream >> temp.isCompleted;
			selectStream >> temp.conditions;
			selectStream >> temp.action;
			selectStream >> temp.beforeState;
			selectStream >> temp.afterState;
			selectStream >> temp.operateTime;
			selectStream >> temp.isMonitor;
            selectStream >> temp.isTicketItem;
            selectStream >> temp.isKeyItem;
            selectStream >> temp.keyItemIndex;
            selectStream >> temp.isLinkItem;
            selectStream >> temp.linkItemIndex;
            selectStream >> temp.actionType;
            selectStream >> temp.WBID;
            selectStream >> temp.lockCode;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
		}
        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找操作动作票操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：加载指定ID列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：刘高舜 2014-08-29
**************************************************************************************************/
int CHistoryTicketActionListMgt::retrieveByTicketID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TTicketActionList));
	int loadCount = 0;
    m_count = 0;

    char *sql = (char*)"SELECT * FROM t_historyticketactionlist where ticketID=:f<unsigned int>  ORDER BY `index`";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream <<ID;

		TTicketActionList temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.ticketID;
			selectStream >> temp.index;
			selectStream >> temp.deviceID;
			selectStream >> temp.isCompleted;
			selectStream >> temp.conditions;
			selectStream >> temp.action;
			selectStream >> temp.beforeState;
			selectStream >> temp.afterState;
			selectStream >> temp.operateTime;
			selectStream >> temp.isMonitor;
            selectStream >> temp.isTicketItem;
            selectStream >> temp.isKeyItem;
            selectStream >> temp.keyItemIndex;
            selectStream >> temp.isLinkItem;
            selectStream >> temp.linkItemIndex;
            selectStream >> temp.actionType;
            selectStream >> temp.WBID;
            selectStream >> temp.lockCode;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
		}
        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找操作动作票操作失败！");
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
作者：佘佳明 2014-11-24
**************************************************************************************************/
int CHistoryTicketActionListMgt::add( TTicketActionList *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TTicketActionList temp = {0};
    memcpy(&temp, pInputParam, sizeof(TTicketActionList));

	int lastAddID;
	char operateTime[MAX_PARAM_LEN] = {0};
	sprintf(operateTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->operateTime.year, pInputParam->operateTime.month,pInputParam->operateTime.day,pInputParam->operateTime.hour,pInputParam->operateTime.minute,pInputParam->operateTime.second);

	char *sql = (char *)malloc(2000);
    sprintf(sql,"insert into t_historyticketactionlist values(:f1_ID<unsigned int>, \
            :f2_ticketID<unsigned int>, \
            :f3_index<unsigned int>, \
            :f4_devID<unsigned int>, \
            :f5_iscomp<unsigned int>, \
            :f6_conditions<char[128]>, \
            :f7_action<char[32]>, \
            :f8_bstate<unsigned int>, \
             :f9_astate<unsigned int>, \
             '%s', \
             :f10_ismoni<unsigned int>, \
             :f11_isticketitem<unsigned int>, \
             :f12_iskeyitem<unsigned int>, \
             :f13_keyitemindex<unsigned int>, \
             :f14_islinkitem<unsigned int>, \
             :f15_linkitemindex<unsigned int>, \
             :f16_actiontype<unsigned int>, \
             :f17_WBID<unsigned int>,       \
             :f18_lockCode<unsigned int>)", operateTime);
	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
        insertStream << pInputParam->ID;
		insertStream << pInputParam->ticketID;
		insertStream << pInputParam->index;
		insertStream << pInputParam->deviceID;
		insertStream << pInputParam->isCompleted;
		//insertStream << pInputParam->makeTicketTime;
		insertStream << pInputParam->conditions;
		insertStream << pInputParam->action;
		insertStream << pInputParam->beforeState;
		insertStream << pInputParam->afterState;
        insertStream << pInputParam->isMonitor;
        insertStream << pInputParam->isTicketItem;
        insertStream << pInputParam->isKeyItem;
        insertStream << pInputParam->keyItemIndex;
        insertStream << pInputParam->isLinkItem;
        insertStream << pInputParam->linkItemIndex;
        insertStream << pInputParam->actionType;
        insertStream << pInputParam->WBID;
        insertStream << pInputParam->lockCode;

        m_db.commit();
	}
	catch (otl_exception& p)
	{
		WLOutDebug("添加间隔操作失败！");
		m_db.rollback();
		free(sql);

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	free(sql);

	return temp.ID;
}

/**************************************************************************************************
功能：删除记录
参数：ID，删除的记录的ID
返回：-2，数据库尚未连接
	  -3, 从列表中删除记录失败
	  -4， 数据库删除操作失败
	  0， 成功
作者：佘佳明 2014-11-24
**************************************************************************************************/
int CHistoryTicketActionListMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

    char *sql = (char*)"DELETE FROM t_historyticketactionlist WHERE ID = :f1_ID<unsigned int>";
	try
	{
		otl_stream delStream(1, sql, m_db);
		delStream << ID;
        m_db.commit();
		printf("数据库删除记录 ID: %d\n", ID);

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
作者：佘佳明 2014-11-24
**************************************************************************************************/
int CHistoryTicketActionListMgt::update( const DWORD32 ID, TTicketActionList *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

	char operateTime[MAX_PARAM_LEN] = {0};
	sprintf(operateTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->operateTime.year, pInputParam->operateTime.month,pInputParam->operateTime.day,pInputParam->operateTime.hour,pInputParam->operateTime.minute,pInputParam->operateTime.second);

	char *sql = (char *)malloc(2000);
    sprintf(sql,"UPDATE t_historyticketactionlist set ticketID=:f1_ticketID<unsigned int>, \
            `index`=:f2_index<unsigned int>, \
            deviceID=:f3_devID<unsigned int>, \
            isCompleted=:f4_iscomp<unsigned int>, \
            conditions=:f5_conditions<char[128]>, \
            action=:f6_action<char[32]>, \
            beforeState=:f7_bstate<unsigned int>, \
            afterState=:f8_astate<unsigned int>, \
            operateTime='%s', \
            isMonitor=:f10_ismoni<unsigned int>, \
            isTicketItem=:f11_isticketitem<unsigned int>, \
            isKeyItem=:f12_iskeyitem<unsigned int>, \
            keyItemIndex=:f13_keyitemindex<unsigned int>, \
            isLinkItem=:f14_islinkitem<unsigned int>, \
            linkItemIndex=:f15_linkitemindex<unsigned int>, \
            actionType=:f16_actiontype<unsigned int>,   \
            WBID=:f17_WBID<unsigned int>,               \
            lockCode=:f18_lockCode<unsigned int> where ID=:f19_id<unsigned int>",operateTime);
	try
	{
		otl_stream updateStream(1, sql, m_db);
		updateStream << pInputParam->ticketID;
		updateStream << pInputParam->index;
		updateStream << pInputParam->deviceID;
		updateStream << pInputParam->isCompleted;
		//insertStream << pInputParam->makeTicketTime;
		updateStream << pInputParam->conditions;
		updateStream << pInputParam->action;
		updateStream << pInputParam->beforeState;
		updateStream << pInputParam->afterState;
		updateStream << pInputParam->isMonitor;
        updateStream << pInputParam->isTicketItem;
        updateStream << pInputParam->isKeyItem;
        updateStream << pInputParam->keyItemIndex;
        updateStream << pInputParam->isLinkItem;
        updateStream << pInputParam->linkItemIndex;
        updateStream << pInputParam->actionType;
        updateStream << pInputParam->WBID;
        updateStream << pInputParam->lockCode;
		updateStream << ID;

        m_db.commit();
		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;

	}
	catch (otl_exception& p)
	{
		WLOutDebug("更新间隔资源关系操作失败！");
		free(sql);

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	free(sql);

	return 0;
}

/**************************************************************************************************
功能：添加记录到列表
参数：pStaion，待添加的记录
返回：-1，参数错误
	  -2， 重复记录
	  -3， 申请内存失败
	  0， 成功
作者：佘佳明 2014-11-24
**************************************************************************************************/
int CHistoryTicketActionListMgt::add2List( TTicketActionList *pInputParam )
{
	if(NULL == pInputParam)		return -1;

    int ret = -1;
//    int ret = isIDExist(pInputParam->ID);
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

	memcpy(&m_pList[m_count], pInputParam, sizeof(TTicketActionList));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：佘佳明 2014-11-24
**************************************************************************************************/
int CHistoryTicketActionListMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pList[i], &m_pList[i+1], sizeof(TTicketActionList));
	}
	memset(&m_pList[m_count-1], 0x00, sizeof(TTicketActionList));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：佘佳明 2014-11-24
**************************************************************************************************/
int CHistoryTicketActionListMgt::isIDExist( const DWORD32 ID )
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
作者：佘佳明 2014-11-24
**************************************************************************************************/
int CHistoryTicketActionListMgt::getMoreMemory()
{
    int sumCount = m_sumCount + MAX_HISTORYTICKETACTIONLIST_COUNT;
	TTicketActionList *pResource = new TTicketActionList[sumCount];

	if(NULL == pResource)
	{
		WLOutDebug("getMoreMemory():COperateTicketMgt::COperateTicketMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TTicketActionList)*m_sumCount);
	memcpy(pResource, m_pList, sizeof(TTicketActionList)*m_count);
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
int CHistoryTicketActionListMgt::getListCount()
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TTicketActionList * CHistoryTicketActionListMgt::getListByIndex( DWORD32 index )
{
	if(index > m_count-1)	return NULL;

	return &m_pList[index];
}

/**************************************************************************************************
功能：根据票动作ID获取票动作
参数：ID   票动作ID
返回：NULL 没有此动作ID对应的票动作
     非 NULL 票动作对象指针
作者：佘佳明 2014-10-24
**************************************************************************************************/
const TTicketActionList *CHistoryTicketActionListMgt::getListByID(const DWORD32 ID)
{
    TTicketActionList *pAction = NULL;
    for (int i=0; i<m_count; i++)
    {
        pAction = &m_pList[i];
        if (pAction == NULL)
        {
            continue ;
        }
        if (pAction->ID != ID)
        {
            continue ;
        }
        return pAction;
    }
    return pAction;
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
int CHistoryTicketActionListMgt::updateList( const DWORD32 ID, const TTicketActionList *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TTicketActionList *temp = getListByIndex(index);
    memcpy(temp, pInputParam, sizeof(TTicketActionList));

	return 0;
}


