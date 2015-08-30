#include "stdafx.h"
#include "BusinessDataSet.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-09
**************************************************************************************************/
CBusinessDataSet::CBusinessDataSet()
{
	m_isInit = false;							//将连接置为断开
	m_isRemoteAccess = false;					//将其设为远程连接否
	m_pCS = new CWLCriticalSection();			//初始化临界区

    m_dbType = 0;
	memset(m_IP,  0x00, sizeof(m_IP));			//初始化用户名
	memset(m_uid, 0x00, sizeof(m_uid));			//初始化用户名
	memset(m_psw, 0x00, sizeof(m_psw));			//初始化密码
	memset(m_dsn, 0x00, sizeof(m_dsn));			//初始化数据源
	memset(m_driver, 0x00, sizeof(m_driver));	//初始化数据库驱动名称
	memset(m_dbName, 0x00, sizeof(m_dbName));	//初始化数据库名称
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-08
**************************************************************************************************/
CBusinessDataSet::~CBusinessDataSet()
{
	if (NULL != m_pCS)
	{
		delete m_pCS;
		m_pCS = NULL;
	}
}

/**************************************************************************************************
功能：设置数据库相关参数，初始化数据库连接对象
参数：uid，用户名
	  psw，密码
	  dsn，数据源名称
返回：-1，参数错误
	  -2，连接失败
	  0，成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CBusinessDataSet::setConnectParam( const char * uid,const char * psw,const char * dsn, const char *IP )
{
	if(NULL == uid || NULL == psw || NULL == dsn)		return -1;

	if(NULL != IP)
	{
		strcpy(m_IP, IP);
		m_isRemoteAccess = true;
	}

	strcpy(m_uid, uid);
	strcpy(m_psw, psw);
	strcpy(m_dsn, dsn);

	int ret = connect();
	if(-1 == ret)		return -2;

	m_isInit = true;
	
	return 0;
}

/**************************************************************************************************
功能：设置数据库相关参数，初始化数据库连接对象
参数：dbType,数据库类型：0,MySQL;1,SQLite;2,SQLServer
	  driver，数据库驱动名称
	  dbName，数据库名称
	  uid，用户名
	  psw，密码
	  dsn，数据源名称
	  IP，数据库服务IP
返回：-1，参数错误
	  -2，连接失败
	  0，成功
作者：刘远安 2014-07-08
**************************************************************************************************/
int CBusinessDataSet::setConnectParam( DWORD32 dbType,const char * driver,const char * dbName, const char * uid,const char * psw,const char * dsn, const char *IP)
{
	if(NULL == dbName)		return -1;

    m_dbType = dbType;

	int ret = 0;
	if(0 == dbType)
	{
		ret = connectToMySQL(driver,dbName,uid,psw,dsn,IP);
		if(ret < 0)		return -2;
	}
	else if(1 == dbType)
	{
		ret = connectToSQLite(driver,dbName,uid,psw,dsn,IP);
		if(ret < 0)		return -2;
	}
	else if(2 == dbType)
	{
		ret = connectToSQLServer(driver,dbName,uid,psw,dsn,IP);
		if(ret < 0)		return -2;
	}
    else
    {
        return -3;
    }

	m_isInit = true;

	return 0;
}

/**************************************************************************************************
功能：连接数据库
参数：无
返回：-1，连接失败
	  0，初始化成功
作者：李施施 2014-07-08
**************************************************************************************************/
int CBusinessDataSet::connect()
{
	int ret = 0;
	char constr[MAX_PARAM_LEN] = {0};

	//尝试连接数据库
	//这里提供两种方式连接数据库：第一种，在代码中配置数据源（对应if中的情况），因此不需要手工的配置数据源；第二种，不在代码中配置数据源，因此需要手工配置数据源
    if(m_isRemoteAccess)	sprintf(constr,"Driver={MySQL ODBC 5.2 ANSI Driver};Server=%s; Database=%s; Uid=%s; Pwd=%s; CharSet=UTF8",m_IP, m_dsn,m_uid,m_psw);
	else					sprintf(constr,"%s/%s@%s",m_uid,m_psw,m_dsn);

	otl_connect::otl_initialize();                  // initialize ODBC environment
	try
	{
		m_db.rlogon(constr);						// connect to ODBC
		m_db.auto_commit_off();						//关闭事务自动提交

        WLOutDebug("Database connection success!\n");
	}
	catch(otl_exception& p){						// intercept OTL exceptions
		cerr<<p.msg<<endl;							// print out error message
		cerr<<p.stm_text<<endl;						// print out SQL that caused the error
		cerr<<p.sqlstate<<endl;						// print out SQLSTATE message
		cerr<<p.var_info<<endl;						// print out the variable that caused the error
		ret--;
	}

    return ret;
}

/**************************************************************************************************
功能：重新连接数据库
参数：无
返回：<0，连接失败
     >=0，连接成功
作者：刘远安 2014-07-08
**************************************************************************************************/
int CBusinessDataSet::reConnect()
{
    int ret = 0;
    if ( m_isInit == true )
    {
        ret = disConnect();
        if ( ret < 0 )
        {
            return -1;
        }
//        ret = connect();
        if ( m_dbType == 0)
        {
            ret = connectToMySQL(m_driver, m_dbName, m_uid, m_psw, m_dsn, m_IP);
        }
        else if ( m_dbType == 1 )
        {
            ret = connectToSQLite(m_driver, m_dbName, m_uid, m_psw, m_dsn, m_IP);
        }
        else if ( m_dbType == 2 )
        {
            ret = connectToSQLServer(m_driver, m_dbName, m_uid, m_psw, m_dsn, m_IP);
        }
        if ( ret < 0 )
        {
            return -2;
        }
    }
    m_isInit = true;
    return 0;
}

/**************************************************************************************************
功能：查询当前数据库连接是否处于连接状态(主要用于解决mysql空闲连接超8小时自动断开)
参数：无
返回：<0，连接失败
     >=0，连接成功
作者：刘高舜 2015-01-13
**************************************************************************************************/
int CBusinessDataSet::checkDBConnect()
{
    if(!m_isInit)					return -1;

//    const char *sql = "CREATE TABLE t_testconnect (isConnect INT(11));";
    const char *sql = "SELECT *FROM t_testconnect";
    try
    {
        otl_stream insertStream(1, sql, m_db);
        m_db.commit();
    }
    catch (otl_exception& p)
    {
        WLOutDebug("搜索测试连接表操作失败！");
        cerr<<p.msg<<endl;							// print out error message
        cerr<<p.stm_text<<endl;						// print out SQL that caused the error
        cerr<<p.sqlstate<<endl;						// print out SQLSTATE message
        cerr<<p.var_info<<endl;						// print out the variable that caused the error
        return reConnect();
    }
    return 0;
}

/**************************************************************************************************
功能：断开数据库连接
参数：无
返回：<0，操作失败
	  0，操作成功
作者：刘远安 2014-07-08
**************************************************************************************************/
int CBusinessDataSet::disConnect()
{
	otl_connect::otl_initialize();                  // initialize ODBC environment
	try
	{
		m_db.logoff();								// disconnect from Oracle
        m_isInit = false;
        WLOutDebug("Database connection success\n");
	}
	catch(otl_exception& p){						// intercept OTL exceptions
        WLOutDebug("Database connection failed!\n");
		cerr<<p.msg<<endl;							// print out error message
		cerr<<p.stm_text<<endl;						// print out SQL that caused the error
		cerr<<p.sqlstate<<endl;						// print out SQLSTATE message
		cerr<<p.var_info<<endl;						// print out the variable that caused the error
		return -4;
	}

	return 0;
}

/**************************************************************************************************
功能：连接MySQL数据库
参数：driver，数据库驱动名称
	  dbName，数据库名称
	  uid，用户名
	  psw，密码
	  dsn，数据源名称
	  IP，数据库服务IP
返回：<0，连接失败
	  >=0，连接成功
作者：刘远安 2014-07-08
**************************************************************************************************/
int CBusinessDataSet::connectToMySQL(const char * driver,const char * dbName, const char * uid,const char * psw,const char * dsn, const char *IP)
{
	//IP连接方式
	if(NULL != IP && strlen(IP) > 0)
	{
		if(NULL == uid || NULL == psw || NULL == driver || NULL == dbName || NULL == IP)		return -1;
	}
	else//数据源连接方式
	{
		if(NULL == uid || NULL == psw || NULL == dsn)		return -1;
	}
	
	//IP连接方式
	if(NULL != IP && strlen(IP) > 0)
	{
		strcpy(m_IP, IP);
		strcpy(m_uid, uid);
		strcpy(m_psw, psw);
		strcpy(m_dbName, dbName);
		strcpy(m_driver, driver);
		m_isRemoteAccess = true;
	}
	else
	{
		strcpy(m_uid, uid);
		strcpy(m_psw, psw);
		strcpy(m_dsn, dsn);
	}

	int ret = -1;
	char constr[MAX_PARAM_LEN] = {0};

	//尝试连接数据库
	//这里提供两种方式连接数据库：第一种，在代码中配置数据源（对应if中的情况），因此不需要手工的配置数据源；第二种，不在代码中配置数据源，因此需要手工配置数据源
#ifdef _WIN32
    if(m_isRemoteAccess)	sprintf(constr,"Driver={%s};Server=%s; Database=%s; Uid=%s; Pwd=%s; CharSet=UTF8",m_driver,m_IP, m_dbName,m_uid,m_psw);
    else					sprintf(constr,"%s/%s@%s",m_uid,m_psw,m_dsn);
#else
	if(m_isRemoteAccess)	sprintf(constr,"Driver={%s};Server=%s; Database=%s; Uid=%s; Pwd=%s; CharSet=UTF8",m_driver,m_IP,m_dbName,m_uid,m_psw);
	else					sprintf(constr,"%s/%s@%s",m_uid,m_psw,m_dsn);
#endif

	otl_connect::otl_initialize();                  // initialize ODBC environment
	try
	{
		m_db.rlogon(constr);						// connect to ODBC
		m_db.auto_commit_off();						//关闭事务自动提交

        WLOutDebug("Database connection success!\n");
	}
	catch(otl_exception& p){						// intercept OTL exceptions
		cerr<<p.msg<<endl;							// print out error message
		cerr<<p.stm_text<<endl;						// print out SQL that caused the error
		cerr<<p.sqlstate<<endl;						// print out SQLSTATE message
		cerr<<p.var_info<<endl;						// print out the variable that caused the error
		return -2;
	}

	return 0;
}

/**************************************************************************************************
功能：连接SQLite数据库
参数：driver，数据库驱动名称
	  dbName，数据库名称
	  uid，用户名
	  psw，密码
	  dsn，数据源名称
	  IP，数据库服务IP
返回：<0，连接失败
	  >=0，连接成功
作者：刘远安 2014-07-08
**************************************************************************************************/
int CBusinessDataSet::connectToSQLite(const char * driver,const char * dbName, const char * uid,const char * psw,const char * dsn, const char *IP)
{
	//数据源连接方式
	if(NULL != dsn && strlen(dsn) > 0)
	{
		if(NULL == uid || NULL == psw || NULL == dsn)		return -1;
	}
	else
	{
		if(NULL == driver || NULL == dbName)		return -1;
	}

	if(NULL == dsn || 0 == strlen(dsn))
	{
		strcpy(m_dbName, dbName);
		strcpy(m_driver, driver);
		m_isRemoteAccess = true;
	}
	else
	{
		strcpy(m_uid, uid);
		strcpy(m_psw, psw);
		strcpy(m_dsn, dsn);
	}

	int ret = -1;
	char constr[MAX_PARAM_LEN] = {0};

	//尝试连接数据库
	//这里提供两种方式连接数据库：第一种，在代码中配置数据源（对应if中的情况），因此不需要手工的配置数据源；第二种，不在代码中配置数据源，因此需要手工配置数据源
//#ifdef _WIN32
/*	if(m_isRemoteAccess)	sprintf(constr,"Driver={%s};Database=%s; OEMCP=1;SyncPragma=NORMAL;Timeout=100000; CharSet=UTF8",m_driver,m_dbName);
    else					sprintf(constr,"%s/%s@%s",m_uid,m_psw,m_dsn);*/
//#else
    if(m_isRemoteAccess)	sprintf(constr,"Driver={%s};Database=%s; OEMCP=1;SyncPragma=NORMAL;Timeout=100000; CharSet=UTF8",m_driver,m_dbName);
    else					sprintf(constr,"%s/%s@%s",m_uid,m_psw,m_dsn);
//#endif

	otl_connect::otl_initialize();                  // initialize ODBC environment
	try
	{
		m_db.rlogon(constr);						// connect to ODBC
		m_db.auto_commit_off();						//关闭事务自动提交

        WLOutDebug("Database connection success!\n");
	}
	catch(otl_exception& p){						// intercept OTL exceptions
		cerr<<p.msg<<endl;							// print out error message
		cerr<<p.stm_text<<endl;						// print out SQL that caused the error
		cerr<<p.sqlstate<<endl;						// print out SQLSTATE message
		cerr<<p.var_info<<endl;						// print out the variable that caused the error
		return -2;
	}

	return 0;
}

/**************************************************************************************************
功能：连接SQLServer数据库
参数：driver，数据库驱动名称
	  dbName，数据库名称
	  uid，用户名
	  psw，密码
	  dsn，数据源名称
	  IP，数据库服务IP
返回：<0，连接失败
	  >=0，连接成功
作者：刘远安 2014-07-08
**************************************************************************************************/
int CBusinessDataSet::connectToSQLServer(const char * driver,const char * dbName, const char * uid,const char * psw,const char * dsn, const char *IP)
{
	//IP连接方式
	if(NULL != IP && strlen(IP) > 0)
	{
		if(NULL == uid || NULL == psw || NULL == driver || NULL == dbName || NULL == IP)		return -1;
	}
	else//数据源连接方式
	{
		if(NULL == uid || NULL == psw || NULL == dsn)		return -1;
	}

	//IP连接方式
	if(NULL != IP && strlen(IP) > 0)
	{
		strcpy(m_IP, IP);
		strcpy(m_uid, uid);
		strcpy(m_psw, psw);
		strcpy(m_dbName, dbName);
		strcpy(m_driver, driver);
		m_isRemoteAccess = true;
	}
	else
	{
		strcpy(m_uid, uid);
		strcpy(m_psw, psw);
		strcpy(m_dsn, dsn);
	}

	int ret = -1;
	char constr[MAX_PARAM_LEN] = {0};

	//尝试连接数据库
	//这里提供两种方式连接数据库：第一种，在代码中配置数据源（对应if中的情况），因此不需要手工的配置数据源；第二种，不在代码中配置数据源，因此需要手工配置数据源
#ifdef _WIN32
    if(m_isRemoteAccess)	sprintf(constr,"Driver={%s};Server=%s; Database=%s; Uid=%s; Pwd=%s; CharSet=UTF8",m_driver,m_IP, m_dbName,m_uid,m_psw);
	else					sprintf(constr,"%s/%s@%s",m_uid,m_psw,m_dsn);
#else
	if(m_isRemoteAccess)	sprintf(constr,"Driver={%s};Server=%s; Database=%s; Uid=%s; Pwd=%s; CharSet=UTF8",m_driver,m_IP,m_dbName,m_uid,m_psw);
	else					sprintf(constr,"%s/%s@%s",m_uid,m_psw,m_dsn);
#endif

	otl_connect::otl_initialize();                  // initialize ODBC environment
	try
	{
		m_db.rlogon(constr);						// connect to ODBC
		m_db.auto_commit_off();						//关闭事务自动提交

        WLOutDebug("Database connection success!\n");
	}
	catch(otl_exception& p){						// intercept OTL exceptions
		cerr<<p.msg<<endl;							// print out error message
		cerr<<p.stm_text<<endl;						// print out SQL that caused the error
		cerr<<p.sqlstate<<endl;						// print out SQLSTATE message
		cerr<<p.var_info<<endl;						// print out the variable that caused the error
		return -2;
	}

	return 0;
}


