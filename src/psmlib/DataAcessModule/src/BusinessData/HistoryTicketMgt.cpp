#include "stdafx.h"
#include "HistoryTicketMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：佘佳明 2014-11-24
**************************************************************************************************/
CHistoryTicketMgt::CHistoryTicketMgt()
{
	m_count = 0;
	m_sumCount = MAX_OPERATETICKET_COUNT;

	m_pList = new TOperateTicket[m_sumCount];
	if(NULL == m_pList)
	{
        WLOutDebug("CHistoryTicketMgt::CHistoryTicketMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：佘佳明 2014-11-24
**************************************************************************************************/
CHistoryTicketMgt::~CHistoryTicketMgt()
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
int CHistoryTicketMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TOperateTicket));
	int loadCount = 0;
	m_count = 0;
    const char *sql = "SELECT * FROM t_historyticket ";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

		TOperateTicket temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.name;
			selectStream >> temp.number;
			selectStream >> temp.makeTicketPerson;
			selectStream >> temp.makeTicketTime;
			selectStream >> temp.type;
            selectStream >> temp.keyTicketType;
			selectStream >> temp.ticketSource;
			selectStream >> temp.bayID;
            selectStream >> temp.volID;
			selectStream >> temp.state;
			selectStream >> temp.stopCause;
			selectStream >> temp.completeTime;
			selectStream >> temp.operateWayBeginState;
			selectStream >> temp.operateWayEndState;
			selectStream >> temp.auditState;
            selectStream >> temp.operater;
			selectStream >> temp.guardian;
			selectStream >> temp.dutyManager;
			selectStream >> temp.sendUnit;
			selectStream >> temp.sendPerson;
			selectStream >> temp.recvUnit;
			selectStream >> temp.recvPerson;
			selectStream >> temp.dangerDescriptionID;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
		}
        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找操作票操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：加载所有列表,通过ID值逆序排列
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：佘佳明  2014-09-12
**************************************************************************************************/
int CHistoryTicketMgt::retrieve_DESC()
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TOperateTicket));
    int loadCount = 0;
    m_count = 0;
    const char *sql = "SELECT * FROM t_historyticket ORDER BY ID DESC";

    try
    {

        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);

        TOperateTicket temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.name;
            selectStream >> temp.number;
            selectStream >> temp.makeTicketPerson;
            selectStream >> temp.makeTicketTime;
            selectStream >> temp.type;
            selectStream >> temp.keyTicketType;
            selectStream >> temp.ticketSource;
            selectStream >> temp.bayID;
            selectStream >> temp.volID;
            selectStream >> temp.state;
            selectStream >> temp.stopCause;
            selectStream >> temp.completeTime;
            selectStream >> temp.operateWayBeginState;
            selectStream >> temp.operateWayEndState;
            selectStream >> temp.auditState;
            selectStream >> temp.operater;
            selectStream >> temp.guardian;
            selectStream >> temp.dutyManager;
            selectStream >> temp.sendUnit;
            selectStream >> temp.sendPerson;
            selectStream >> temp.recvUnit;
            selectStream >> temp.recvPerson;
            selectStream >> temp.dangerDescriptionID;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }
        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找操作票操作失败！");
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
int CHistoryTicketMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TOperateTicket));
    m_count = 0;
	int loadCount = 0;

    const char *sql = "SELECT * FROM t_historyticket where ID=:f<unsigned int>";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TOperateTicket temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.name;
			selectStream >> temp.number;
			selectStream >> temp.makeTicketPerson;
			selectStream >> temp.makeTicketTime;
			selectStream >> temp.type;
            selectStream >> temp.keyTicketType;
			selectStream >> temp.ticketSource;
			selectStream >> temp.bayID;
			selectStream >> temp.volID;
			selectStream >> temp.state;
			selectStream >> temp.stopCause;
			selectStream >> temp.completeTime;
			selectStream >> temp.operateWayBeginState;
			selectStream >> temp.operateWayEndState;
			selectStream >> temp.auditState;
            selectStream >> temp.operater;
			selectStream >> temp.guardian;
			selectStream >> temp.dutyManager;
			selectStream >> temp.sendUnit;
			selectStream >> temp.sendPerson;
			selectStream >> temp.recvUnit;
			selectStream >> temp.recvPerson;
			selectStream >> temp.dangerDescriptionID;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
		}
        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找操作票操作失败！");
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
作者：刘高舜 2014-09-10
**************************************************************************************************/
int CHistoryTicketMgt::retrieveByID(const DWORD32 ID)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TOperateTicket));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_historyticket where stationID=:f<unsigned int>";

    try
    {

        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << ID;

        TOperateTicket temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.name;
            selectStream >> temp.number;
            selectStream >> temp.makeTicketPerson;
            selectStream >> temp.makeTicketTime;
            selectStream >> temp.type;
            selectStream >> temp.keyTicketType;
            selectStream >> temp.ticketSource;
            selectStream >> temp.bayID;
            selectStream >> temp.volID;
            selectStream >> temp.state;
            selectStream >> temp.stopCause;
            selectStream >> temp.completeTime;
            selectStream >> temp.operateWayBeginState;
            selectStream >> temp.operateWayEndState;
            selectStream >> temp.auditState;
            selectStream >> temp.operater;
            selectStream >> temp.guardian;
            selectStream >> temp.dutyManager;
            selectStream >> temp.sendUnit;
            selectStream >> temp.sendPerson;
            selectStream >> temp.recvUnit;
            selectStream >> temp.recvPerson;
            selectStream >> temp.dangerDescriptionID;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找操作票操作失败！");
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
int CHistoryTicketMgt::retrieveByStationID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TOperateTicket));
	int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_historyticket where stationID=:f<unsigned int>";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
        selectStream << ID;

		TOperateTicket temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.name;
			selectStream >> temp.number;
			selectStream >> temp.makeTicketPerson;
			selectStream >> temp.makeTicketTime;
			selectStream >> temp.type;
            selectStream >> temp.keyTicketType;
			selectStream >> temp.ticketSource;
			selectStream >> temp.bayID;
			selectStream >> temp.volID;
			selectStream >> temp.state;
			selectStream >> temp.stopCause;
			selectStream >> temp.completeTime;
			selectStream >> temp.operateWayBeginState;
			selectStream >> temp.operateWayEndState;
			selectStream >> temp.auditState;
            selectStream >> temp.operater;
			selectStream >> temp.guardian;
			selectStream >> temp.dutyManager;
			selectStream >> temp.sendUnit;
			selectStream >> temp.sendPerson;
			selectStream >> temp.recvUnit;
			selectStream >> temp.recvPerson;
			selectStream >> temp.dangerDescriptionID;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
		}
        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找操作票操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：根据票类型加载列表
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：李施施 2014-8-7
**************************************************************************************************/
int CHistoryTicketMgt::retrieveByType( const DWORD32 type )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TOperateTicket));
	int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_historyticket where type=:f<unsigned int>";

	try
	{

		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << type;

		TOperateTicket temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.name;
			selectStream >> temp.number;
			selectStream >> temp.makeTicketPerson;
			selectStream >> temp.makeTicketTime;
			selectStream >> temp.type;
            selectStream >> temp.keyTicketType;
			selectStream >> temp.ticketSource;
			selectStream >> temp.bayID;
			selectStream >> temp.volID;
			selectStream >> temp.state;
			selectStream >> temp.stopCause;
			selectStream >> temp.completeTime;
			selectStream >> temp.operateWayBeginState;
			selectStream >> temp.operateWayEndState;
			selectStream >> temp.auditState;
            selectStream >> temp.operater;
			selectStream >> temp.guardian;
			selectStream >> temp.dutyManager;
			selectStream >> temp.sendUnit;
			selectStream >> temp.sendPerson;
			selectStream >> temp.recvUnit;
			selectStream >> temp.recvPerson;
			selectStream >> temp.dangerDescriptionID;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找操作票操作失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
	return 0;
}

/**************************************************************************************************
功能：根据票类型加载列表,以ID逆序排列
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：佘佳明  2014-9-12
**************************************************************************************************/
int CHistoryTicketMgt::retrieveByType_DESC( const DWORD32 type )
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TOperateTicket));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_historyticket where type=:f<unsigned int> ORDER BY ID DESC";

    try
    {

        m_db.commit();
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << type;

        TOperateTicket temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.name;
            selectStream >> temp.number;
            selectStream >> temp.makeTicketPerson;
            selectStream >> temp.makeTicketTime;
            selectStream >> temp.type;
            selectStream >> temp.keyTicketType;
            selectStream >> temp.ticketSource;
            selectStream >> temp.bayID;
            selectStream >> temp.volID;
            selectStream >> temp.state;
            selectStream >> temp.stopCause;
            selectStream >> temp.completeTime;
            selectStream >> temp.operateWayBeginState;
            selectStream >> temp.operateWayEndState;
            selectStream >> temp.auditState;
            selectStream >> temp.operater;
            selectStream >> temp.guardian;
            selectStream >> temp.dutyManager;
            selectStream >> temp.sendUnit;
            selectStream >> temp.sendPerson;
            selectStream >> temp.recvUnit;
            selectStream >> temp.recvPerson;
            selectStream >> temp.dangerDescriptionID;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找操作票操作失败！");
        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}

/**************************************************************************************************
功能：根据票ID加载列表
参数：ID   票 ID
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：佘佳明  2014-9-12
**************************************************************************************************/
int CHistoryTicketMgt::retrieveByTicketID(const DWORD32 ID)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TOperateTicket));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_historyticket where ID=:f<unsigned int>";

    try
    {

        m_db.commit();
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << ID;

        TOperateTicket temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.name;
            selectStream >> temp.number;
            selectStream >> temp.makeTicketPerson;
            selectStream >> temp.makeTicketTime;
            selectStream >> temp.type;
            selectStream >> temp.keyTicketType;
            selectStream >> temp.ticketSource;
            selectStream >> temp.bayID;
            selectStream >> temp.volID;
            selectStream >> temp.state;
            selectStream >> temp.stopCause;
            selectStream >> temp.completeTime;
            selectStream >> temp.operateWayBeginState;
            selectStream >> temp.operateWayEndState;
            selectStream >> temp.auditState;
            selectStream >> temp.operater;
            selectStream >> temp.guardian;
            selectStream >> temp.dutyManager;
            selectStream >> temp.sendUnit;
            selectStream >> temp.sendPerson;
            selectStream >> temp.recvUnit;
            selectStream >> temp.recvPerson;
            selectStream >> temp.dangerDescriptionID;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找操作票操作失败！");
        cerr<<p.msg<<endl;		// print out error message
        cerr<<p.stm_text<<endl; // print out SQL that caused the error
        cerr<<p.sqlstate<<endl; // print out SQLSTATE message
        cerr<<p.var_info<<endl; // print out the variable that caused the error
        return -4;
    }
    return 0;
}

/**************************************************************************************************
功能：根据站ID加载列表,以日期倒序排序
参数：ID   站 ID
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：佘佳明  2014-11-17
**************************************************************************************************/
int CHistoryTicketMgt::retrieveByStationIDAndTypeOrderByDate_DESC(const DWORD32 ID, const DWORD32 type)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TOperateTicket));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_historyticket where stationID=:f1<unsigned int> AND type=:f2<unsigned int> ORDER BY makeTicketTime DESC";

    try
    {

        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << ID;
        selectStream << type;

        TOperateTicket temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.name;
            selectStream >> temp.number;
            selectStream >> temp.makeTicketPerson;
            selectStream >> temp.makeTicketTime;
            selectStream >> temp.type;
            selectStream >> temp.keyTicketType;
            selectStream >> temp.ticketSource;
            selectStream >> temp.bayID;
            selectStream >> temp.volID;
            selectStream >> temp.state;
            selectStream >> temp.stopCause;
            selectStream >> temp.completeTime;
            selectStream >> temp.operateWayBeginState;
            selectStream >> temp.operateWayEndState;
            selectStream >> temp.auditState;
            selectStream >> temp.operater;
            selectStream >> temp.guardian;
            selectStream >> temp.dutyManager;
            selectStream >> temp.sendUnit;
            selectStream >> temp.sendPerson;
            selectStream >> temp.recvUnit;
            selectStream >> temp.recvPerson;
            selectStream >> temp.dangerDescriptionID;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }
        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找操作票操作失败！");
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
int CHistoryTicketMgt::add( TOperateTicket *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TOperateTicket temp = {0};
    temp.ID = pInputParam->ID;
	temp.stationID = pInputParam->stationID;
	memcpy(&temp.name, pInputParam->name, strlen(pInputParam->name));
	temp.number = pInputParam->number;
	memcpy(&temp.makeTicketPerson, pInputParam->makeTicketPerson, strlen(pInputParam->makeTicketPerson));
	memcpy(&temp.makeTicketTime, &pInputParam->makeTicketTime, sizeof(otl_datetime));
	temp.type = pInputParam->type;
    temp.keyTicketType = pInputParam->keyTicketType;
	temp.ticketSource = pInputParam->ticketSource;
    temp.bayID = pInputParam->bayID;
	temp.volID = pInputParam->volID;
	temp.state = pInputParam->state;
	memcpy(&temp.stopCause, pInputParam->stopCause, strlen(pInputParam->stopCause));
	memcpy(&temp.completeTime, &pInputParam->completeTime, sizeof(otl_datetime));

	temp.operateWayBeginState = pInputParam->operateWayBeginState;
	temp.operateWayEndState = pInputParam->operateWayEndState;
	temp.auditState = pInputParam->auditState;
    temp.operater = pInputParam->operater;
    temp.guardian = pInputParam->guardian;
    temp.dutyManager = pInputParam->dutyManager;
	memcpy(&temp.sendUnit, pInputParam->sendUnit, strlen(pInputParam->sendUnit));
	memcpy(&temp.sendPerson, pInputParam->sendPerson, strlen(pInputParam->sendPerson));
	memcpy(&temp.recvUnit, pInputParam->recvUnit, strlen(pInputParam->recvUnit));
	memcpy(&temp.recvPerson, pInputParam->recvPerson, strlen(pInputParam->recvPerson));
	temp.dangerDescriptionID = pInputParam->dangerDescriptionID;

	int lastAddID;
	char pMakeTicketTime[MAX_PARAM_LEN] = {0};
	sprintf(pMakeTicketTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->makeTicketTime.year, pInputParam->makeTicketTime.month,pInputParam->makeTicketTime.day,pInputParam->makeTicketTime.hour,pInputParam->makeTicketTime.minute,pInputParam->makeTicketTime.second);
	char pCompleteTime[MAX_PARAM_LEN] = {0};
	sprintf(pCompleteTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->completeTime.year, pInputParam->completeTime.month,pInputParam->completeTime.day,pInputParam->completeTime.hour,pInputParam->completeTime.minute,pInputParam->completeTime.second);

	char *sql = (char *)malloc(2000);
    sprintf(sql,"insert into t_historyticket values(\
            :f1_ID<unsigned int>,                                   \
            :f2_stationID<unsigned int>,                            \
            :f3_name<char[255]>,                                    \
            :f4_number<unsigned int>,                               \
            :f5_person<char[16]>,                                   \
             '%s',                                                  \
             :f6_type<unsigned int>,                                \
             :f7_keyTicketType<unsigned int>,                       \
             :f8_src<unsigned int>,                                 \
             :f9_bayID<unsigned int>,                               \
             :f10_volID<unsigned int>,                              \
             :f11_state<unsigned int>,                              \
             :f12_stopCou<char[255]>,                               \
             '%s',                                                  \
             :f13_begin<unsigned int>,                              \
             :f14_end<unsigned int>,                                \
             :f15_auditState<unsigned int>,                         \
             :f16_operater<unsigned int>,                           \
             :f17_gua<unsigned int>,                                \
             :f18_manager<unsigned int>,                            \
             :f19_sendunit<char[16]>,                               \
             :f20_sendp<char[64]>,                                  \
             :f21_recvu<char[64]>,                                  \
             :f22_recvp<char[16]>,                                  \
             :f23<unsigned int>)",                                  \
            pMakeTicketTime,pCompleteTime);
	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
        insertStream << pInputParam->ID;
		insertStream << pInputParam->stationID;
		insertStream << pInputParam->name;
		insertStream << pInputParam->number;
		insertStream << pInputParam->makeTicketPerson;
		//insertStream << pInputParam->makeTicketTime;
		insertStream << pInputParam->type;
        insertStream << pInputParam->keyTicketType;
		insertStream << pInputParam->ticketSource;
		insertStream << pInputParam->bayID;
		insertStream << pInputParam->volID;
		insertStream << pInputParam->state;
		insertStream << pInputParam->stopCause;
		//insertStream << pInputParam->completeTime;
		insertStream << pInputParam->operateWayBeginState;
		insertStream << pInputParam->operateWayEndState;
		insertStream << pInputParam->auditState;
        insertStream << pInputParam->operater;
		insertStream << pInputParam->guardian;
		insertStream << pInputParam->dutyManager;
		insertStream << pInputParam->sendUnit;
		insertStream << pInputParam->sendPerson;
		insertStream << pInputParam->recvUnit;
		insertStream << pInputParam->recvPerson;
		insertStream << pInputParam->dangerDescriptionID;

        m_db.commit();
	}
	catch (otl_exception& p)
	{
		WLOutDebug("添加操作票操作失败！");
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
int CHistoryTicketMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

    const char *sql = "DELETE FROM t_historyticket WHERE ID = :f1_ID<unsigned int>";
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
		WLOutDebug("删除操作票操作失败！");

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
int CHistoryTicketMgt::update( const DWORD32 ID, TOperateTicket *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

	char pMakeTicketTime[MAX_PARAM_LEN] = {0};
	sprintf(pMakeTicketTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->makeTicketTime.year, pInputParam->makeTicketTime.month,pInputParam->makeTicketTime.day,pInputParam->makeTicketTime.hour,pInputParam->makeTicketTime.minute,pInputParam->makeTicketTime.second);
	char pCompleteTime[MAX_PARAM_LEN] = {0};
	sprintf(pCompleteTime,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->completeTime.year, pInputParam->completeTime.month,pInputParam->completeTime.day,pInputParam->completeTime.hour,pInputParam->completeTime.minute,pInputParam->completeTime.second);

	char *sql = (char *)malloc(2000);
    sprintf(sql,"UPDATE t_historyticket set stationID=:f1_stationID<unsigned int>,name=:f2_name<char[255]>,number=:f3_number<unsigned int>,makeTicketPerson=:f4_person<char[16]>,makeTicketTime='%s',type=:f6_type<unsigned int>,keyTicketType=:f7_keyTicketType<unsigned int>,ticketSource=:f8_src<unsigned int>,bayID=:f9_bayID<unsigned int>,volID=:f10_volID<unsigned int>,state=:f11_state<unsigned int>,stopCause=:f12_stopCou<char[255]>,completeTime='%s',operateWayBeginState=:f13_begin<unsigned int>,operateWayEndState=:f14_end<unsigned int>,auditState=:f15_auditState<unsigned int>,operater=:f16_operater<unsigned int>,guardian=:f17_gua<unsigned int>,dutyManager=:f18_manager<unsigned int>,sendUnit=:f19_sendunit<char[16]>,sendPerson=:f20_sendp<char[64]>,recvUnit=:f21_recvu<char[64]>,recvPerson=:f22_recvp<char[16]>,dangerDescriptionID=:f23<unsigned int> where ID=:f_id<unsigned int>",pMakeTicketTime,pCompleteTime);
    //char *sql = "UPDATE t_historyticket SET stationID=:f1<unsigned int>,name=:f2<char[64]>,type=:f3<unsigned int>, volID=:f4<unsigned int> WHERE ID=:f5<unsigned int>";
	try
	{
		otl_stream updateStream(1, sql, m_db);
		updateStream << pInputParam->stationID;
		updateStream << pInputParam->name;
		updateStream << pInputParam->number;
		updateStream << pInputParam->makeTicketPerson;
		//insertStream << pInputParam->makeTicketTime;
		updateStream << pInputParam->type;
        updateStream << pInputParam->keyTicketType;
		updateStream << pInputParam->ticketSource;
		updateStream << pInputParam->bayID;
		updateStream << pInputParam->volID;
 		updateStream << pInputParam->state;
		updateStream << pInputParam->stopCause;
		//insertStream << pInputParam->completeTime;
		updateStream << pInputParam->operateWayBeginState;
		updateStream << pInputParam->operateWayEndState;
		updateStream << pInputParam->auditState;
        updateStream << pInputParam->operater;
		updateStream << pInputParam->guardian;
		updateStream << pInputParam->dutyManager;
		updateStream << pInputParam->sendUnit;
		updateStream << pInputParam->sendPerson;
		updateStream << pInputParam->recvUnit;
		updateStream << pInputParam->recvPerson;
		updateStream << pInputParam->dangerDescriptionID;
		updateStream << ID;

        m_db.commit();
        updateList(ID, pInputParam);
	}
	catch (otl_exception& p)
	{
		WLOutDebug("更新操作票操作失败！");
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
int CHistoryTicketMgt::add2List( TOperateTicket *pInputParam )
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

	memcpy(&m_pList[m_count], pInputParam, sizeof(TOperateTicket));
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
int CHistoryTicketMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pList[i], &m_pList[i+1], sizeof(TOperateTicket));
	}
	memset(&m_pList[m_count-1], 0x00, sizeof(TOperateTicket));
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
int CHistoryTicketMgt::isIDExist( const DWORD32 ID )
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
int CHistoryTicketMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_OPERATETICKET_COUNT;
	TOperateTicket *pResource = new TOperateTicket[sumCount];

	if(NULL == pResource)
	{
        WLOutDebug("getMoreMemory():CHistoryTicketMgt::CHistoryTicketMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TOperateTicket)*m_sumCount);
	memcpy(pResource, m_pList, sizeof(TOperateTicket)*m_count);
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
int CHistoryTicketMgt::getListCount()
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TOperateTicket * CHistoryTicketMgt::getListByIndex( DWORD32 index )
{
	if(index > m_count-1)	return NULL;

	return &m_pList[index];
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
int CHistoryTicketMgt::updateList( const DWORD32 ID, const TOperateTicket *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TOperateTicket *temp = getListByIndex(index);
	temp->stationID = pInputParam->stationID;
	memset(temp->name, 0x00, sizeof(temp->name));
	memcpy(temp->name, pInputParam->name, strlen(pInputParam->name));
	temp->number = pInputParam->number;
	memset(temp->makeTicketPerson, 0x00, sizeof(temp->makeTicketPerson));
	memcpy(temp->makeTicketPerson, pInputParam->makeTicketPerson, strlen(pInputParam->makeTicketPerson));
	memset(&temp->makeTicketTime, 0x00, sizeof(otl_datetime));
	memcpy(&temp->makeTicketTime, &pInputParam->makeTicketTime, sizeof(otl_datetime));
	temp->type = pInputParam->type;
    temp->keyTicketType = pInputParam->keyTicketType;
	temp->ticketSource = pInputParam->ticketSource;
	temp->bayID = pInputParam->bayID;
	temp->volID = pInputParam->volID;
    temp->state = pInputParam->state;
	memset(temp->stopCause, 0x00, sizeof(temp->stopCause));
	memcpy(temp->stopCause, pInputParam->stopCause, strlen(pInputParam->stopCause));
	memset(&temp->completeTime, 0x00, sizeof(otl_datetime));
	memcpy(&temp->completeTime, &pInputParam->completeTime, sizeof(otl_datetime));
	temp->operateWayBeginState = pInputParam->operateWayBeginState;
	temp->operateWayEndState = pInputParam->operateWayEndState;
	temp->auditState = pInputParam->auditState;
    temp->operater = pInputParam->operater;
    temp->guardian = pInputParam->guardian;
    temp->dutyManager = pInputParam->dutyManager;
	memset(temp->sendUnit, 0x00, sizeof(temp->sendUnit));
	memcpy(temp->sendUnit, pInputParam->sendUnit, strlen(pInputParam->sendUnit));

	return 0;
}


