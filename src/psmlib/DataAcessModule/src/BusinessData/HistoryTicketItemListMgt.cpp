#include "stdafx.h"
#include "HistoryTicketItemListMgt.h"


/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：佘佳明 2014-11-24
**************************************************************************************************/
CHistoryTicketItemListMgt::CHistoryTicketItemListMgt()
{
	m_count = 0;
	m_sumCount = MAX_TICKETITEMLIST_COUNT;

	m_pList = new TTicketItemList[m_sumCount];
	if(NULL == m_pList)
	{
        WLOutDebug("CHistoryTicketItemListMgt::CHistoryTicketItemListMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：佘佳明 2014-11-24
**************************************************************************************************/
CHistoryTicketItemListMgt::~CHistoryTicketItemListMgt()
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
int CHistoryTicketItemListMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TTicketItemList));
	int loadCount = 0;
	m_count = 0;

    char *sql = (char*)"SELECT * FROM t_historyticketitemlist ORDER BY `index`";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

		TTicketItemList temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
            selectStream >> temp.ticketID;
			selectStream >> temp.ticketActionID;
			selectStream >> temp.index;
			selectStream >> temp.itemType;
			selectStream >> temp.itemContent;
			selectStream >> temp.isPrint;
			selectStream >> temp.itemShipType;
			selectStream >> temp.subItemIndex;
            selectStream >> temp.isCompleted;
            selectStream >> temp.isTicketItem;
            selectStream >> temp.isKeyItem;
            selectStream >> temp.keyItemIndex;
            selectStream >> temp.isLinkItem;
            selectStream >> temp.linkItemIndex;
            selectStream >> temp.isToKey;
            selectStream >> temp.promptType;
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
int CHistoryTicketItemListMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TTicketItemList));
    m_count = 0;
	int loadCount = 0;
    char *sql = (char*)"SELECT * FROM t_historyticketitemlist where ticketActionID=:f_id<unsigned int> ORDER BY `index`";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TTicketItemList temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
            selectStream >> temp.ticketID;
			selectStream >> temp.ticketActionID;
			selectStream >> temp.index;
			selectStream >> temp.itemType;
			selectStream >> temp.itemContent;
			selectStream >> temp.isPrint;
			selectStream >> temp.itemShipType;
			selectStream >> temp.subItemIndex;
            selectStream >> temp.isCompleted;
            selectStream >> temp.isTicketItem;
            selectStream >> temp.isKeyItem;
            selectStream >> temp.keyItemIndex;
            selectStream >> temp.isLinkItem;
            selectStream >> temp.linkItemIndex;
            selectStream >> temp.isToKey;
            selectStream >> temp.promptType;
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
功能：加载指定ID下的列表
参数：无
返回：-2，未初始化数据库连接
	  -3， 插入列表失败
	  -4，数据库操作失败
	  0，成功
作者：刘高舜 2014-08-29
**************************************************************************************************/
int CHistoryTicketItemListMgt::retrieveByTicketActionID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TTicketItemList));
    m_count = 0;
	int loadCount = 0;
    char *sql = (char*)"SELECT * FROM t_historyticketitemlist where ticketActionID=:f_id<unsigned int> ORDER BY `index`";

	try
	{
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TTicketItemList temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
            selectStream >> temp.ticketID;
			selectStream >> temp.ticketActionID;
			selectStream >> temp.index;
			selectStream >> temp.itemType;
			selectStream >> temp.itemContent;
			selectStream >> temp.isPrint;
			selectStream >> temp.itemShipType;
			selectStream >> temp.subItemIndex;
            selectStream >> temp.isCompleted;
            selectStream >> temp.isTicketItem;
            selectStream >> temp.isKeyItem;
            selectStream >> temp.keyItemIndex;
            selectStream >> temp.isLinkItem;
            selectStream >> temp.linkItemIndex;
            selectStream >> temp.isToKey;
            selectStream >> temp.promptType;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
		}
        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找票项表操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：加载指定票ID下的列表
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：佘佳明 2014-10-23
**************************************************************************************************/
int CHistoryTicketItemListMgt::retrieveByTicketID( const DWORD32 ID )
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TTicketItemList));
    m_count = 0;
    int loadCount = 0;
    char *sql = (char*)"SELECT * FROM t_historyticketitemlist where ticketID=:f_id<unsigned int> ORDER BY `index`";

    try
    {
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << ID;

        TTicketItemList temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.ticketID;
            selectStream >> temp.ticketActionID;
            selectStream >> temp.index;
            selectStream >> temp.itemType;
            selectStream >> temp.itemContent;
            selectStream >> temp.isPrint;
            selectStream >> temp.itemShipType;
            selectStream >> temp.subItemIndex;
            selectStream >> temp.isCompleted;
            selectStream >> temp.isTicketItem;
            selectStream >> temp.isKeyItem;
            selectStream >> temp.keyItemIndex;
            selectStream >> temp.isLinkItem;
            selectStream >> temp.linkItemIndex;
            selectStream >> temp.isToKey;
            selectStream >> temp.promptType;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }
        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找票项表操作失败！");
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
int CHistoryTicketItemListMgt::add( TTicketItemList *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TTicketItemList temp = {0};
    memcpy(&temp, pInputParam, sizeof(TTicketItemList));

	int lastAddID;
    char *sql = (char*)"insert into t_historyticketitemlist values(:f1<unsigned int>, \
            :f2<unsigned int>, \
            :f3<unsigned int>, \
             :f4<unsigned int>, \
             :f5<unsigned int>, \
             :f6<char[255]>, \
             :f7<unsigned int>, \
             :f8<unsigned int>, \
             :f9<unsigned int>, \
             :f10<unsigned int>, \
             :f11<unsigned int>, \
             :f12<unsigned int>, \
             :f13<unsigned int>, \
             :f14<unsigned int>, \
             :f15<unsigned int>, \
             :f16<unsigned int>, \
             :f17<unsigned int>)";
	
	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
        insertStream << pInputParam->ID;
        insertStream << pInputParam->ticketID;
		insertStream << pInputParam->ticketActionID;
		insertStream << pInputParam->index;
		insertStream << pInputParam->itemType;
		insertStream << pInputParam->itemContent;
		//insertStream << pInputParam->makeTicketTime;
		insertStream << pInputParam->isPrint;
		insertStream << pInputParam->itemShipType;
		insertStream << pInputParam->subItemIndex;
        insertStream << pInputParam->isCompleted;
        insertStream << pInputParam->isTicketItem;
        insertStream << pInputParam->isKeyItem;
        insertStream << pInputParam->keyItemIndex;
        insertStream << pInputParam->isLinkItem;
        insertStream << pInputParam->linkItemIndex;
        insertStream << pInputParam->isToKey;
        insertStream << pInputParam->promptType;

        m_db.commit();
	}
	catch (otl_exception& p)
	{
        WLOutDebug("添加票项表操作失败！");
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
作者：佘佳明 2014-11-24
**************************************************************************************************/
int CHistoryTicketItemListMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

    char *sql = (char*)"DELETE FROM t_historyticketitemlist WHERE ID = :f1_ID<unsigned int>";
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
        WLOutDebug("删除票项表操作失败！");

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
int CHistoryTicketItemListMgt::update( const DWORD32 ID, TTicketItemList *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

    char *sql = (char*)"UPDATE t_historyticketitemlist SET ticketID=:f1<unsigned int>, \
            ticketActionID=:f2<unsigned int>, \
            `index`=:f3<unsigned int>, \
            itemType=:f4<unsigned int>, \
            itemContent=:f5<char[255]>, \
            isPrint=:f6<unsigned int>, \
            itemShipType=:f7<unsigned int>, \
            subItemIndex=:f8<unsigned int>, \
            isCompleted=:f9<unsigned int>, \
            isTicketItem=:f10<unsigned int>, \
            isKeyItem=:f11<unsigned int>, \
            keyItemIndex=:f12<unsigned int>, \
            isLinkItem=:f13<unsigned int>, \
            linkItemIndex=:f14<unsigned int>, \
            isToKey=:f15<unsigned int>, \
            promptType=:f16<unsigned int> WHERE ID=:f17<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
        updateStream << pInputParam->ticketID;
		updateStream << pInputParam->ticketActionID;
		updateStream << pInputParam->index;
		updateStream << pInputParam->itemType;
		updateStream << pInputParam->itemContent;
		updateStream << pInputParam->isPrint;
		updateStream << pInputParam->itemShipType;
		updateStream << pInputParam->subItemIndex;
        updateStream << pInputParam->isCompleted;
        updateStream << pInputParam->isTicketItem;
        updateStream << pInputParam->isKeyItem;
        updateStream << pInputParam->keyItemIndex;
        updateStream << pInputParam->isLinkItem;
        updateStream << pInputParam->linkItemIndex;
        updateStream << pInputParam->isToKey;
        updateStream << pInputParam->promptType;
		updateStream << ID;

        m_db.commit();
	}
	catch (otl_exception& p)
	{
        WLOutDebug("更新票项表操作失败！");

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
作者：佘佳明 2014-11-24
**************************************************************************************************/
int CHistoryTicketItemListMgt::add2List( TTicketItemList *pInputParam )
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

	memcpy(&m_pList[m_count], pInputParam, sizeof(TTicketItemList));
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
int CHistoryTicketItemListMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pList[i], &m_pList[i+1], sizeof(TTicketItemList));
	}
	memset(&m_pList[m_count-1], 0x00, sizeof(TTicketItemList));
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
int CHistoryTicketItemListMgt::isIDExist( const DWORD32 ID )
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
int CHistoryTicketItemListMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_TICKETITEMLIST_COUNT;
	TTicketItemList *pResource = new TTicketItemList[sumCount];

	if(NULL == pResource)
	{
        WLOutDebug("getMoreMemory():CHistoryTicketItemListMgt::CHistoryTicketItemListMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TTicketItemList)*m_sumCount);
	memcpy(pResource, m_pList, sizeof(TTicketItemList)*m_count);
	delete [] m_pList;

	m_pList = pResource;
	return 0;
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
int CHistoryTicketItemListMgt::updateList( const DWORD32 ID, const TTicketItemList *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

    TTicketItemList *temp = getListByIndex(index);
    memcpy(temp, pInputParam, sizeof(TTicketItemList));

	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：李施施 2014-07-29
**************************************************************************************************/
int CHistoryTicketItemListMgt::getListCount()
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TTicketItemList * CHistoryTicketItemListMgt::getListByIndex( DWORD32 index )
{
	if(index > m_count-1)	return NULL;

	return &m_pList[index];
}

/**************************************************************************************************
功能：根据ID获取操作项
参数：ID   项 ID
返回：NULL 不存在对应项
作者：佘佳明 2014-10-27
**************************************************************************************************/
const TTicketItemList * CHistoryTicketItemListMgt::getListByID(DWORD32 ID)
{
    for (int i=0; i<m_count; i++)
    {
        if (m_pList[i].ID != ID)
        {
            continue ;
        }
        return &m_pList[i];
    }
    return NULL;
}
