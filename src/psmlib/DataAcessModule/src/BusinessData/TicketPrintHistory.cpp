#include "stdafx.h"
#include "TicketPrintHistory.h"
#include "platformLayer/WLOutDebug.h"
#include <time.h>

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：韦树远 2014-12-03
**************************************************************************************************/
CTicketPrintHistory::CTicketPrintHistory()
{
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：韦树远 2014-12-03
**************************************************************************************************/
CTicketPrintHistory::~CTicketPrintHistory()
{
}

/**************************************************************************************************
功能：生成操作票打印流水号
参数：
返回：-2，未初始化数据库连接
      -4，数据库操作失败
      非0，操作票打印流水号
作者：韦树远 2014-10-13
**************************************************************************************************/
int CTicketPrintHistory::generate(DWORD32 ticketID, DWORD32 stationID, DWORD32 pageCount)
{
	if(!m_isInit) return -2;

	//根据站ID查找配置
	int loadCount = 0;
    TTicketSNParam config = {0};
	const char *sql = "SELECT * FROM t_ticketprintsn where stationID=:f1<unsigned int>";

	try
	{
		m_db.commit();

		otl_stream selectStream(200, sql, m_db);
		selectStream << stationID;

		while (!selectStream.eof())
		{
			selectStream >> config.ID;
			selectStream >> config.stationID;
			DWORD32 addtype;
			selectStream >> addtype;
			config.addType = NumAddType(addtype);
			DWORD32 clearType;
			selectStream >> clearType;
			config.clearType = NumClearType(clearType);
			selectStream >> config.currentNum;
			selectStream >> config.numDigits;
			selectStream >> config.SNParam;
			loadCount++;
		}

        m_db.commit();
		printf("站 %d 共 %d 条配置\n", stationID, loadCount);
	}catch(otl_exception& p)
	{
		WLOutDebug("查找操作票打印参数设置失败！");
		cerr << p.msg << endl;		// print out error message
		cerr << p.stm_text << endl; // print out SQL that caused the error
		cerr << p.sqlstate <<endl; // print out SQLSTATE message
		cerr << p.var_info <<endl; // print out the variable that caused the error
		return -4;
	}

	//在操作票打印票号历史表中查询
    sql = "SELECT * FROM t_ticketprinthistory where ticketID=:f1<unsigned int> and stationID=:f2<unsigned int> and pageCount=:f3<unsigned int>";
	loadCount = 0;
	TTicketPrintHistoryItem item = {0};

	try
	{
		m_db.commit();

		otl_stream selectStream(200, sql, m_db);
		selectStream << ticketID;
		selectStream << stationID;
		selectStream << pageCount;

		while (!selectStream.eof())
		{
			selectStream >> item.ID;
			selectStream >> item.ticketID;
			selectStream >> item.stationID;
			selectStream >> item.pageCount;
			selectStream >> item.ticketPrintID;
			selectStream >> item.time;

			loadCount++;
		}

        m_db.commit();
		printf("操作票%d共 %d 条记录\n", ticketID, loadCount);
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找操作票打印历史记录失败！");
		cerr << p.msg << endl;		// print out error message
		cerr << p.stm_text << endl; // print out SQL that caused the error
		cerr << p.sqlstate << endl; // print out SQLSTATE message
		cerr << p.var_info << endl; // print out the variable that caused the error
		return -4;
	}

	//在操作票打印历史表中有记录，返回历史值
	if (loadCount > 0)
	{
		return item.ticketPrintID;
	}

	//重新分配打印流水号

	//按站查询最晚打印的操作票的打印时间
	sql = "SELECT max(time) FROM t_ticketprinthistory where stationID=:f1<unsigned int>";
	loadCount = 0;
	TTicketPrintHistoryItem max_item = {0};

	try
	{
		m_db.commit();

		otl_stream selectStream(200, sql, m_db);
		selectStream << stationID;

		while (!selectStream.eof())
		{
			selectStream >> max_item.time;

			loadCount++;
		}

        m_db.commit();
		//没有记录，取出为空值
		if (max_item.time.year == 0 && max_item.time.month == 0)
		{
			loadCount = 0;
		}

		printf("站%d共 %d 条最新打印记录\n", stationID, loadCount);
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找操作票打印时间最大值记录失败！");
		cerr << p.msg << endl;		// print out error message
		cerr << p.stm_text << endl; // print out SQL that caused the error
		cerr << p.sqlstate << endl; // print out SQLSTATE message
		cerr << p.var_info << endl; // print out the variable that caused the error
		return -4;
	}

	//该站没有相应记录
	if (max_item.time.year == 0 && max_item.time.month == 0)
	{
		return config.currentNum + 1;
	}
	else
	{
		//取系统当前时间
		time_t currentTime;
		struct tm* ptm;

		time(&currentTime);
		ptm = localtime(&currentTime);

		WLOutDebug("站 %d 最后打印时间为 %d - %d,回滚策略: %s", stationID, max_item.time.year, max_item.time.month,(config.clearType == NumClearType_PerYear)?"年":"月");

		//按年清零
		if (config.clearType == NumClearType_PerYear && (ptm->tm_year + 1900) != max_item.time.year )
		{
			return 1;
		}
		//按月清零
		else if (config.clearType == NumClearType_PerMonth && (ptm->tm_mon + 1) != max_item.time.month)
		{
			return 1;
		}
		else
		{
			return config.currentNum + 1;
		}
	}

	return 1;
}

/**************************************************************************************************
功能：保存操作票打印流水号
参数：
返回：-2，未初始化数据库连接
      -4，数据库操作失败
      非0，操作票打印流水号
作者：韦树远 2014-10-13
**************************************************************************************************/
int CTicketPrintHistory::save(DWORD32 ticketID, DWORD32 ticketPrintID, DWORD32 stationID, DWORD32 pageCount)
{
	int lastAddID = 0;

	if(!m_isInit) return -2;

	//查询是否有记录
	try
	{
		int loadCount = 0;

		m_db.commit();

		const char* sql = "SELECT * FROM t_ticketprinthistory where ticketID=:f1<unsigned int> and stationID=:f2<unsigned int> and pageCount=:f3<unsigned int>";
		otl_stream selectStream(200, sql, m_db);
		selectStream << ticketID;
		selectStream << stationID;
		selectStream << pageCount;

		TTicketPrintHistoryItem item = {0};
		while (!selectStream.eof())
		{
			selectStream >> item.ID;
			selectStream >> item.ticketID;
			selectStream >> item.stationID;
			selectStream >> item.pageCount;
			selectStream >> item.ticketPrintID;
			selectStream >> item.time;

			loadCount++;
		}

        m_db.commit();
		lastAddID = item.ID;

		printf("查询操作票%d共 %d 条记录\n", ticketID, loadCount);
	}
	catch (otl_exception& p)
	{
		WLOutDebug("查找操作票打印历史记录失败！");
		cerr << p.msg << endl;		// print out error message
		cerr << p.stm_text << endl; // print out SQL that caused the error
		cerr << p.sqlstate << endl; // print out SQLSTATE message
		cerr << p.var_info << endl; // print out the variable that caused the error
		return -4;
	}

	if (lastAddID > 0)
	{
		return lastAddID;
	}

	//没有记录，插入新记录

	//取系统当前时间
	time_t currentTime;
	struct tm* ptm;

	time(&currentTime);
	ptm = localtime(&currentTime);

	char strCreatIDTime[MAX_PARAM_LEN] = {0};
	sprintf(strCreatIDTime,"%d-%02d-%02d %02d:%02d:%02d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

	char sql[1024] = {0};
	sprintf(sql,"insert into t_ticketprinthistory values(NULL,:f1<unsigned int>,:f2<unsigned int>,:f3<unsigned int>,:f4<unsigned int>,'%s')", strCreatIDTime);

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);

		insertStream << ticketID;
		insertStream << stationID;
		insertStream << pageCount;
		insertStream << ticketPrintID;

        if(1 == m_dbType)//SQLite数据库
        {
            char strLastAddID[20] = {0};
            otl_stream selectStream(1, "select last_insert_rowid()", m_db);
            selectStream.set_commit(0);
            selectStream >> strLastAddID;
            lastAddID = atoi(strLastAddID);
        }
        else
        {
            otl_stream selectStream(1, "SELECT LAST_INSERT_ID()", m_db);
            selectStream.set_commit(0);
            selectStream >> lastAddID;
        }

		m_db.commit();
	}
	catch (otl_exception& p)
	{
		WLOutDebug("添加操作票打印历史记录失败！");
		m_db.rollback();

		cerr << p.msg << endl;		// print out error message
		cerr << p.stm_text << endl; // print out SQL that caused the error
		cerr << p.sqlstate << endl; // print out SQLSTATE message
		cerr << p.var_info << endl; // print out the variable that caused the error
		return -4;
	}

	return lastAddID;
}
