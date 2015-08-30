#include "stdafx.h"
#include "DeviceMgt.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：李施施 2014-07-21
**************************************************************************************************/
CDeviceMgt::CDeviceMgt()
{
	m_count = 0;
	m_sumCount = MAX_DEVICE_COUNT;

	m_pList = new TDevice[m_sumCount];
	if(NULL == m_pList)
	{
		WLOutDebug("CDeviceMgt::CDeviceMgt()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：李施施 2014-07-21
**************************************************************************************************/
CDeviceMgt::~CDeviceMgt()
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
作者：李施施 2014-07-21
**************************************************************************************************/
int CDeviceMgt::retrieve( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )           return -4;

	memset(m_pList, 0x00, m_sumCount * sizeof(TDevice));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_device where stationID=:f_ID<unsigned int> ";

    m_db.commit();

	try
	{
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TDevice temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.number;
			selectStream >> temp.doubleNumber;
			selectStream >> temp.volID;
			selectStream >> temp.deviceTypeID;
			selectStream >> temp.curState;
			selectStream >> temp.bayID;
			selectStream >> temp.termTemplateID;
			selectStream >> temp.isLocked;
            selectStream >> temp.isMonitor;
            selectStream >> temp.isYX;
			selectStream >> temp.isHaveWarningBoard;
			selectStream >> temp.warningBoardID;
			selectStream >> temp.isHaveBelongWarningBoard;
			selectStream >> temp.belongWarningBoardID;
			selectStream >> temp.isHaveEarthLine;
			selectStream >> temp.earthLineID;
			selectStream >> temp.isHaveBelongEarthLine;
			selectStream >> temp.belongEarthLineID;
			selectStream >> temp.earthLineCheck;
			selectStream >> temp.isWorking;
			selectStream >> temp.isRedLineDevice;
			selectStream >> temp.isHaveBelongCommonTerm;
			selectStream >> temp.belongCommonTermID;
			selectStream >> temp.isHaveElectricityTerm;
			selectStream >> temp.electricityTermID;
			selectStream >> temp.isVirtual;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		//printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找设备操作失败！");

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
作者：刘高舜 2014-08-29
**************************************************************************************************/
int CDeviceMgt::retrieveByStationID( const DWORD32 ID )
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )           return -4;

	memset(m_pList, 0x00, m_sumCount * sizeof(TDevice));
	int loadCount = 0;
	m_count = 0;

    m_db.commit();

    const char *sql = "SELECT * FROM t_device where stationID=:f_ID<unsigned int> ";

	try
	{
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);
		selectStream << ID;

		TDevice temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.number;
			selectStream >> temp.doubleNumber;
			selectStream >> temp.volID;
			selectStream >> temp.deviceTypeID;
			selectStream >> temp.curState;
			selectStream >> temp.bayID;
			selectStream >> temp.termTemplateID;
			selectStream >> temp.isLocked;
            selectStream >> temp.isMonitor;
            selectStream >> temp.isYX;
			selectStream >> temp.isHaveWarningBoard;
			selectStream >> temp.warningBoardID;
			selectStream >> temp.isHaveBelongWarningBoard;
			selectStream >> temp.belongWarningBoardID;
			selectStream >> temp.isHaveEarthLine;
			selectStream >> temp.earthLineID;
			selectStream >> temp.isHaveBelongEarthLine;
			selectStream >> temp.belongEarthLineID;
			selectStream >> temp.earthLineCheck;
			selectStream >> temp.isWorking;
			selectStream >> temp.isRedLineDevice;
			selectStream >> temp.isHaveBelongCommonTerm;
			selectStream >> temp.belongCommonTermID;
			selectStream >> temp.isHaveElectricityTerm;
			selectStream >> temp.electricityTermID;
			selectStream >> temp.isVirtual;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
        WLOutDebug("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找设备操作失败！");

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
    return 0;
}

/**************************************************************************************************
功能：加载指定设备ID列表
参数：无
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：刘高舜 2014-09-18
**************************************************************************************************/
int CDeviceMgt::retrieveByDeviceID(const DWORD32 ID)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )           return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TDevice));
    int loadCount = 0;
    m_count = 0;

    m_db.commit();

    const char *sql = "SELECT * FROM t_device where ID=:f_ID<unsigned int> ";

    try
    {
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << ID;

        TDevice temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.number;
            selectStream >> temp.doubleNumber;
            selectStream >> temp.volID;
            selectStream >> temp.deviceTypeID;
            selectStream >> temp.curState;
            selectStream >> temp.bayID;
            selectStream >> temp.termTemplateID;
            selectStream >> temp.isLocked;
            selectStream >> temp.isMonitor;
            selectStream >> temp.isYX;
            selectStream >> temp.isHaveWarningBoard;
            selectStream >> temp.warningBoardID;
            selectStream >> temp.isHaveBelongWarningBoard;
            selectStream >> temp.belongWarningBoardID;
            selectStream >> temp.isHaveEarthLine;
            selectStream >> temp.earthLineID;
            selectStream >> temp.isHaveBelongEarthLine;
            selectStream >> temp.belongEarthLineID;
            selectStream >> temp.earthLineCheck;
            selectStream >> temp.isWorking;
            selectStream >> temp.isRedLineDevice;
            selectStream >> temp.isHaveBelongCommonTerm;
            selectStream >> temp.belongCommonTermID;
            selectStream >> temp.isHaveElectricityTerm;
            selectStream >> temp.electricityTermID;
            selectStream >> temp.isVirtual;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        WLOutDebug("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找设备操作失败！");

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
作者：李施施 2014-07-21
**************************************************************************************************/
int CDeviceMgt::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )           return -4;

	memset(m_pList, 0x00, m_sumCount * sizeof(TDevice));
	int loadCount = 0;
	m_count = 0;

    m_db.commit();

    const char *sql = "SELECT * FROM t_device ";

	try
	{
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

		TDevice temp = {0};

		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
			selectStream >> temp.stationID;
			selectStream >> temp.number;
			selectStream >> temp.doubleNumber;
			selectStream >> temp.volID;
			selectStream >> temp.deviceTypeID;
			selectStream >> temp.curState;
			selectStream >> temp.bayID;
			selectStream >> temp.termTemplateID;
			selectStream >> temp.isLocked;
            selectStream >> temp.isMonitor;
            selectStream >> temp.isYX;
			selectStream >> temp.isHaveWarningBoard;
			selectStream >> temp.warningBoardID;
			selectStream >> temp.isHaveBelongWarningBoard;
			selectStream >> temp.belongWarningBoardID;
			selectStream >> temp.isHaveEarthLine;
			selectStream >> temp.earthLineID;
			selectStream >> temp.isHaveBelongEarthLine;
			selectStream >> temp.belongEarthLineID;
			selectStream >> temp.earthLineCheck;
			selectStream >> temp.isWorking;
			selectStream >> temp.isRedLineDevice;
			selectStream >> temp.isHaveBelongCommonTerm;
			selectStream >> temp.belongCommonTermID;
			selectStream >> temp.isHaveElectricityTerm;
			selectStream >> temp.electricityTermID;
			selectStream >> temp.isVirtual;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
        WLOutDebug("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找设备操作失败！");

		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
    return 0;
}

int CDeviceMgt::retrieveByID(const DWORD32 ID)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )           return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TDevice));
    int loadCount = 0;
    m_count = 0;

    m_db.commit();

    const char *sql = "SELECT * FROM t_device where ID=:f_ID<unsigned int> ";

    try
    {
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << ID;

        TDevice temp = {0};

        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.number;
            selectStream >> temp.doubleNumber;
            selectStream >> temp.volID;
            selectStream >> temp.deviceTypeID;
            selectStream >> temp.curState;
            selectStream >> temp.bayID;
            selectStream >> temp.termTemplateID;
            selectStream >> temp.isLocked;
            selectStream >> temp.isMonitor;
            selectStream >> temp.isYX;
            selectStream >> temp.isHaveWarningBoard;
            selectStream >> temp.warningBoardID;
            selectStream >> temp.isHaveBelongWarningBoard;
            selectStream >> temp.belongWarningBoardID;
            selectStream >> temp.isHaveEarthLine;
            selectStream >> temp.earthLineID;
            selectStream >> temp.isHaveBelongEarthLine;
            selectStream >> temp.belongEarthLineID;
            selectStream >> temp.earthLineCheck;
            selectStream >> temp.isWorking;
            selectStream >> temp.isRedLineDevice;
            selectStream >> temp.isHaveBelongCommonTerm;
            selectStream >> temp.belongCommonTermID;
            selectStream >> temp.isHaveElectricityTerm;
            selectStream >> temp.electricityTermID;
            selectStream >> temp.isVirtual;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        WLOutDebug("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找设备操作失败！");

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
作者：李施施 2014-07-21
修改：刘远安 设备的ID为0xffe时自动加1，这是跳步码专用ID
**************************************************************************************************/
int CDeviceMgt::add( TDevice *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )           return -4;

	TDevice temp = {0};
	temp.stationID = pInputParam->stationID;
	memcpy(&temp.number, pInputParam->number, strlen(pInputParam->number));
	memcpy(&temp.doubleNumber, pInputParam->doubleNumber, strlen(pInputParam->doubleNumber));
	temp.volID = pInputParam->volID;
	temp.deviceTypeID = pInputParam->deviceTypeID;
	temp.curState = pInputParam->curState;
	temp.bayID = pInputParam->bayID;
	temp.termTemplateID = pInputParam->termTemplateID;
	temp.isLocked = pInputParam->isLocked;
    temp.isMonitor = pInputParam->isMonitor;
    temp.isYX = pInputParam->isYX;
	temp.isHaveWarningBoard = pInputParam->isHaveWarningBoard;
	temp.warningBoardID = pInputParam->warningBoardID;
	temp.isHaveBelongWarningBoard = pInputParam->isHaveBelongWarningBoard;
	temp.belongWarningBoardID = pInputParam->belongWarningBoardID;
	temp.isHaveEarthLine = pInputParam->isHaveBelongEarthLine;
	temp.earthLineID = pInputParam->earthLineID;
	temp.isHaveBelongEarthLine = pInputParam->isHaveBelongEarthLine;
	temp.belongEarthLineID = pInputParam->belongEarthLineID;
	memcpy(&temp.earthLineCheck, &pInputParam->earthLineCheck, sizeof(otl_datetime));
	temp.isWorking = pInputParam->isWorking;
	temp.isRedLineDevice = pInputParam->isRedLineDevice;
	temp.isHaveBelongCommonTerm = pInputParam->isHaveBelongCommonTerm;
	temp.belongCommonTermID = pInputParam->belongCommonTermID;
	temp.isHaveElectricityTerm = pInputParam->isHaveElectricityTerm;
	temp.electricityTermID = pInputParam->electricityTermID;
	temp.isVirtual = pInputParam->isVirtual;

	int lastAddID;

	char earthLineCheck[MAX_PARAM_LEN] = {0};
	sprintf(earthLineCheck,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->earthLineCheck.year, pInputParam->earthLineCheck.month,pInputParam->earthLineCheck.day,pInputParam->earthLineCheck.hour,pInputParam->earthLineCheck.minute,pInputParam->earthLineCheck.second);

	char *sql = (char *)malloc(2000);
    sprintf(sql,"insert into t_device values(NULL,:f1<unsigned int>,:f2<char[64]>,:f3<char[128]>,:f4<unsigned int>,:f5<unsigned int>,:f6<unsigned int>,:f7<unsigned int>,:f8<unsigned int>,:f9<unsigned int>,:f10<unsigned int>,:f11<unsigned int>,:f12<unsigned int>,:f13<unsigned int>,:f14<unsigned int>,:f15<unsigned int>,:f16<unsigned int>,:f17<unsigned int>,:f18<unsigned int>,:f19<unsigned int>,'%s',:f20<unsigned int>,:f21<unsigned int>,:f22<unsigned int>,:f23<unsigned int>,:f24<unsigned int>,:f25<unsigned int>, :f26<unsigned int>)",earthLineCheck);

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
		insertStream << pInputParam->stationID;
		insertStream << pInputParam->number;
		insertStream << pInputParam->doubleNumber;
		insertStream << pInputParam->volID;
		insertStream << pInputParam->deviceTypeID;
		insertStream << pInputParam->curState;
		insertStream << pInputParam->bayID;
		insertStream << pInputParam->termTemplateID;
		insertStream << pInputParam->isLocked;
        insertStream << pInputParam->isMonitor;
        insertStream << pInputParam->isYX;
		insertStream << pInputParam->isHaveWarningBoard;
		insertStream << pInputParam->warningBoardID;
		insertStream << pInputParam->isHaveBelongWarningBoard;
		insertStream << pInputParam->belongWarningBoardID;
		insertStream << pInputParam->isHaveEarthLine;
		insertStream << pInputParam->earthLineID;
		insertStream << pInputParam->isHaveBelongEarthLine;
		insertStream << pInputParam->belongEarthLineID;
		//insertStream << pInputParam->earthLineCheck;
		insertStream << pInputParam->isWorking;
		insertStream << pInputParam->isRedLineDevice;
		insertStream << pInputParam->isHaveBelongCommonTerm;
		insertStream << pInputParam->belongCommonTermID;
		insertStream << pInputParam->isHaveElectricityTerm;
		insertStream << pInputParam->electricityTermID;
		insertStream << pInputParam->isVirtual;

        if(1 == m_dbType)//SQLite数据库
        {
            char strLastAddID[20] = {0};
            otl_stream selectStream(1, "select last_insert_rowid()", m_db);
            selectStream.set_commit(0);
            selectStream >> strLastAddID;
            temp.ID = atoi(strLastAddID);
            lastAddID = temp.ID;
        }
        else
        {
            otl_stream selectStream(1, "SELECT LAST_INSERT_ID()", m_db);
            selectStream.set_commit(0);
            selectStream >> lastAddID;
            temp.ID = lastAddID;
        }

        //跳步码的内码不能被使用
        if(lastAddID == 0xffe)
        {
            const char *delSql = "DELETE FROM t_device WHERE ID = :f1_ID<unsigned int>";
            otl_stream delStream(1, delSql, m_db);
            delStream << lastAddID;
            m_db.commit();

            printf("数据库删除记录 ID: %d\n", lastAddID);

            //--------------重新添加记录
            otl_stream insertStream2(1, sql, m_db);
            insertStream2.set_commit(0);
            insertStream2 << pInputParam->stationID;
            insertStream2 << pInputParam->number;
            insertStream2 << pInputParam->doubleNumber;
            insertStream2 << pInputParam->volID;
            insertStream2 << pInputParam->deviceTypeID;
            insertStream2 << pInputParam->curState;
            insertStream2 << pInputParam->bayID;
            insertStream2 << pInputParam->termTemplateID;
            insertStream2 << pInputParam->isLocked;
            insertStream2 << pInputParam->isMonitor;
            insertStream2 << pInputParam->isYX;
            insertStream2 << pInputParam->isHaveWarningBoard;
            insertStream2 << pInputParam->warningBoardID;
            insertStream2 << pInputParam->isHaveBelongWarningBoard;
            insertStream2 << pInputParam->belongWarningBoardID;
            insertStream2 << pInputParam->isHaveEarthLine;
            insertStream2 << pInputParam->earthLineID;
            insertStream2 << pInputParam->isHaveBelongEarthLine;
            insertStream2 << pInputParam->belongEarthLineID;
            //insertStream2 << pInputParam->earthLineCheck;
            insertStream2 << pInputParam->isWorking;
            insertStream2 << pInputParam->isRedLineDevice;
            insertStream2 << pInputParam->isHaveBelongCommonTerm;
            insertStream2 << pInputParam->belongCommonTermID;
            insertStream2 << pInputParam->isHaveElectricityTerm;
            insertStream2 << pInputParam->electricityTermID;
            insertStream2 << pInputParam->isVirtual;

            otl_stream selectStream2(1, "SELECT LAST_INSERT_ID()", m_db);
            selectStream2.set_commit(0);
            selectStream2 >> lastAddID;
            m_db.commit();
        }

		temp.ID = lastAddID;

        m_db.commit();

		int ret = add2List(&temp);
		if(ret < 0)
		{
			free(sql);
			return -3;
		}
	}
	catch (otl_exception& p)
	{
		WLOutDebug("添加间隔操作失败！");
		free(sql);
		m_db.rollback();

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
作者：李施施 2014-07-21
**************************************************************************************************/
int CDeviceMgt::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )           return -4;

	int ret = isIDExist(ID);
	if(ret < 0)		return -5;

    const char *sql = "DELETE FROM t_device WHERE ID = :f1_ID<unsigned int>";
	try
	{
//        m_db.commit();
        otl_stream delStream(1, sql, m_db);
        delStream << ID;

        WLOutDebug("数据库删除记录 ID: %d\n", ID);

        m_db.commit();
		int ret = delFromList(ID);
		if(ret < 0)		return -3;
	}
	catch (otl_exception& p)
	{
		WLOutDebug("删除设备操作失败！");

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
作者：李施施 2014-07-21
**************************************************************************************************/
int CDeviceMgt::update( const DWORD32 ID, TDevice *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )           return -4;

	int ret = isIDExist(ID);
	if(ret < 0)						return -3;

	char earthLineCheck[MAX_PARAM_LEN] = {0};
	sprintf(earthLineCheck,"%d-%02d-%02d %02d:%02d:%02d", pInputParam->earthLineCheck.year, pInputParam->earthLineCheck.month,pInputParam->earthLineCheck.day,pInputParam->earthLineCheck.hour,pInputParam->earthLineCheck.minute,pInputParam->earthLineCheck.second);

    char *sql = (char *)malloc(2000);
    sprintf(sql,"UPDATE t_device SET stationID=:f1<unsigned int>,   \
            number=:f2<char[64]>,                                   \
            doubleNumber=:f3<char[128]>,                            \
            volID=:f4<unsigned int>,                                \
            deviceTypeID=:f5<unsigned int>,                         \
            curState=:f6<unsigned int>,                             \
            bayID=:f7<unsigned int>,                                \
            termTemplateID=:f8<unsigned int>,                       \
            isLocked=:f9<unsigned int>,                             \
            isMonitor=:f10<unsigned int>,                           \
            isYX=:f11<unsigned int>,                                \
            isHaveWarningBoard=:f12<unsigned int>,                  \
            warningBoardID=:f13<unsigned int>,                      \
            isHaveBelongWarningBoard=:f14<unsigned int>,            \
            belongWarningBoardID=:f15<unsigned int>,                \
            isHaveEarthLine=:f16<unsigned int>,                     \
            earthLineID=:f17<unsigned int>,                         \
            isHaveBelongEarthLine=:f18<unsigned int>,               \
            belongEarthLineID=:f19<unsigned int>,                   \
            earthLineCheck='%s',                                    \
            isWorking=:f20<unsigned int>,                           \
            isRedLineDevice=:f21<unsigned int>,                     \
            isHaveBelongCommonTerm=:f22<unsigned int>,              \
            belongCommonTermID=:f23<unsigned int>,                  \
            isHaveElectricityTerm=:f24<unsigned int>,               \
            electricityTermID=:f25<unsigned int>,                   \
            isVirtual=:f26<unsigned int>                            \
            where ID=:f27<unsigned int>",                           \
            earthLineCheck);
	try
	{
        otl_stream updateStream(1, sql, m_db);
		updateStream << pInputParam->stationID;
        updateStream << pInputParam->number;
        updateStream << pInputParam->doubleNumber;
        updateStream << pInputParam->volID;
        updateStream << pInputParam->deviceTypeID;
        updateStream << pInputParam->curState;
        updateStream << pInputParam->bayID;
        updateStream << pInputParam->termTemplateID;
        updateStream << pInputParam->isLocked;
        updateStream << pInputParam->isMonitor;
        updateStream << pInputParam->isYX;
        updateStream << pInputParam->isHaveWarningBoard;
        updateStream << pInputParam->warningBoardID;
        updateStream << pInputParam->isHaveBelongWarningBoard;
        updateStream << pInputParam->belongWarningBoardID;
        updateStream << pInputParam->isHaveEarthLine;
        updateStream << pInputParam->earthLineID;
        updateStream << pInputParam->isHaveBelongEarthLine;
        updateStream << pInputParam->belongEarthLineID;
        //updateStream << pInputParam->earthLineCheck;
        updateStream << pInputParam->isWorking;
        updateStream << pInputParam->isRedLineDevice;
        updateStream << pInputParam->isHaveBelongCommonTerm;
        updateStream << pInputParam->belongCommonTermID;
        updateStream << pInputParam->isHaveElectricityTerm;
        updateStream << pInputParam->electricityTermID;
        updateStream << pInputParam->isVirtual;
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
作者：李施施 2014-07-21
**************************************************************************************************/
int CDeviceMgt::add2List( TDevice *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int ret = isIDExist(pInputParam->ID);
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

	memcpy(&m_pList[m_count], pInputParam, sizeof(TDevice));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：李施施 2014-07-21
**************************************************************************************************/
int CDeviceMgt::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
		memcpy(&m_pList[i], &m_pList[i+1], sizeof(TDevice));
	}
	memset(&m_pList[m_count-1], 0x00, sizeof(TDevice));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：李施施 2014-07-21
**************************************************************************************************/
int CDeviceMgt::isIDExist( const DWORD32 ID )
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
作者：李施施 2014-07-21
**************************************************************************************************/
int CDeviceMgt::getMoreMemory()
{
	int sumCount = m_sumCount + MAX_DEVICE_COUNT;
	TDevice *pResource = new TDevice[sumCount];

	if(NULL == pResource)
	{
		WLOutDebug("getMoreMemory():CDeviceMgt::CDeviceMgt()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
	memset(pResource, 0x00, sizeof(TDevice)*m_sumCount);
	memcpy(pResource, m_pList, sizeof(TDevice)*m_count);
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
int CDeviceMgt::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：李施施 2014-07-29
**************************************************************************************************/
TDevice * CDeviceMgt::getListByIndex( DWORD32 index ) const
{
    if(index > m_count-1)
    {
        return NULL;
    }

	return &m_pList[index];
}

/***************************************************************************************
功能：通过设备主编号获取设备结构体
参数：szNumber 设备主编号
返回：NULL    获取失败
     非 NULL    获取成功
作者：佘佳明 2014-09-09
****************************************************************************************/
const TDevice * CDeviceMgt::getDeviceByNumber(const char *szNumber)
{
    if (szNumber == NULL)
    {
        return NULL;
    }
    TDevice *pRetDevice = NULL;
    for (int i=0; i<m_count; i++)
    {
        TDevice *pDevice = &m_pList[i];
        if (pDevice == NULL)
        {
            continue ;
        }
        if (strcmp(pDevice->number, szNumber) == 0)
        {
            pRetDevice = pDevice;
            break;
        }
    }
    return pRetDevice;
}

/***************************************************************************************
功能：通过设备ID获取设备结构体
参数：ID 设备ID
返回：NULL    获取失败
     非 NULL    获取成功
作者：佘佳明 2014-10-24
****************************************************************************************/
const TDevice * CDeviceMgt::getDeviceByID(const DWORD32 ID)
{
    TDevice *pDevice = NULL;
    for (int i=0; i<m_count; i++)
    {
        pDevice = &m_pList[i];
        if (pDevice == NULL)
        {
            continue ;
        }
        if (pDevice->ID != ID)
        {
            continue ;
        }
        return pDevice;
    }
    pDevice = NULL;
    return pDevice;
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
int CDeviceMgt::updateList( const DWORD32 ID, const TDevice *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

	TDevice *temp = getListByIndex(index);
	temp->stationID = pInputParam->stationID;
	memset(temp->number, 0x00, sizeof(temp->number));
	memcpy(temp->number, pInputParam->number, strlen(pInputParam->number));
	memset(temp->doubleNumber, 0x00, sizeof(temp->doubleNumber));
	memcpy(temp->doubleNumber, pInputParam->doubleNumber, strlen(pInputParam->doubleNumber));
	temp->volID = pInputParam->volID;
	temp->deviceTypeID = pInputParam->deviceTypeID;
	temp->curState = pInputParam->curState;

	temp->bayID = pInputParam->bayID;
	temp->termTemplateID = pInputParam->termTemplateID;
    temp->isLocked = pInputParam->isLocked;
    temp->isMonitor = pInputParam->isMonitor;
    temp->isYX = pInputParam->isYX;
	temp->isHaveWarningBoard = pInputParam->isHaveWarningBoard;
	temp->warningBoardID = pInputParam->warningBoardID;
	temp->isHaveBelongWarningBoard = pInputParam->isHaveBelongWarningBoard;
	temp->belongWarningBoardID = pInputParam->belongWarningBoardID;
	temp->isHaveEarthLine = pInputParam->isHaveEarthLine;
	temp->earthLineID = pInputParam->earthLineID;
	temp->isHaveBelongEarthLine = pInputParam->isHaveBelongEarthLine;
	temp->belongEarthLineID = pInputParam->belongEarthLineID;
	temp->earthLineCheck = pInputParam->earthLineCheck;
	temp->isWorking = pInputParam->isWorking;
	temp->isRedLineDevice = pInputParam->isRedLineDevice;
	temp->isHaveBelongCommonTerm = pInputParam->isHaveBelongCommonTerm;
	temp->belongCommonTermID = pInputParam->belongCommonTermID;
	temp->isHaveElectricityTerm = pInputParam->isHaveElectricityTerm;
	temp->electricityTermID = pInputParam->electricityTermID;
	temp->isVirtual = pInputParam->isVirtual;

	return 0;
}

