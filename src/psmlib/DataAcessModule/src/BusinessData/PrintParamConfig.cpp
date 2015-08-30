#include "stdafx.h"
#include "PrintParamConfig.h"
#include "platformLayer/WLOutDebug.h"

/**************************************************************************************************
功能：构造函数
参数：无
返回：无
作者：张林颢 2014-10-11
**************************************************************************************************/
CPrintParamConfig::CPrintParamConfig()
{
	m_count = 0;
    m_sumCount = MAX_PRINTPARAM_COUNT;

    m_pList = new TPrintParam[m_sumCount];
	if(NULL == m_pList)
	{
        WLOutDebug("CPrintParamConfig::CPrintParamConfig()-->初始申请内存失败!");
	}
}

/**************************************************************************************************
功能：析构函数
参数：无
返回：无
作者：张林颢 2014-10-11
**************************************************************************************************/
CPrintParamConfig::~CPrintParamConfig()
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
作者：张林颢 2014-10-11
**************************************************************************************************/
int CPrintParamConfig::retrieve()
{
	if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TPrintParam));
	int loadCount = 0;
	m_count = 0;

    const char *sql = "SELECT * FROM t_printparam ";

	try
	{
        m_db.commit();
		//查找对应ID的记录
		otl_stream selectStream(200, sql, m_db);

        TPrintParam temp = {0};
		while (!selectStream.eof())
		{
			selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.bodyFontFamily;
            selectStream >> temp.fontSize;
            selectStream >> temp.indexCol;
            selectStream >> temp.contentsCol;
            selectStream >> temp.isFixedTicketName;
            selectStream >> temp.fixedTicketName;
            selectStream >> temp.isfirstLineGetUp;
            selectStream >> temp.getUpText;
            selectStream >> temp.isLastLineGetDown;
            selectStream >> temp.getDownText;
            selectStream >> temp.isFirstLineIndex;
            selectStream >> temp.isFirstLineFlag;
            selectStream >> temp.firstLineText;
            selectStream >> temp.isLastLineIndex;
            selectStream >> temp.isLastLineFlag;
            selectStream >> temp.lastLineText;
            selectStream >> temp.isEndFlag;
            int endType;
            selectStream >> endType;
            temp.endType = EndType(endType);
            selectStream >> temp.endText;
            selectStream >> temp.dateTimeFaram;
            selectStream >> temp.currentPageFaram;
            selectStream >> temp.pagesCountFaram;
            selectStream >> temp.isPrintTicketName;
            selectStream >> temp.endPicPath;
			loadCount++;

			int ret = add2List(&temp);
			if(ret < 0)		return -3;
		}

        m_db.commit();
		printf("共 %d 条记录\n", loadCount);
	}catch(otl_exception& p)
	{
        WLOutDebug("查找操作票打印参数设置失败！");
		cerr<<p.msg<<endl;		// print out error message
		cerr<<p.stm_text<<endl; // print out SQL that caused the error
		cerr<<p.sqlstate<<endl; // print out SQLSTATE message
		cerr<<p.var_info<<endl; // print out the variable that caused the error
		return -4;
	}
    return 0;
}

/**************************************************************************************************
功能：根据站ID查找记录
参数：
返回：-2，未初始化数据库连接
      -3， 插入列表失败
      -4，数据库操作失败
      0，成功
作者：刘远安 2014-10-13
**************************************************************************************************/
int CPrintParamConfig::retrieveByStationID(DWORD32 stationID)
{
    if(!m_isInit)		return -2;

    if( checkDBConnect() < 0 )      return -4;

    memset(m_pList, 0x00, m_sumCount * sizeof(TPrintParam));
    int loadCount = 0;
    m_count = 0;

    const char *sql = "SELECT * FROM t_printparam where stationID=:f1<unsigned int>";

    try
    {
        m_db.commit();
        //查找对应ID的记录
        otl_stream selectStream(200, sql, m_db);
        selectStream << stationID;

        TPrintParam temp = {0};
        while (!selectStream.eof())
        {
            selectStream >> temp.ID;
            selectStream >> temp.stationID;
            selectStream >> temp.bodyFontFamily;
            selectStream >> temp.fontSize;
            selectStream >> temp.indexCol;
            selectStream >> temp.contentsCol;
            selectStream >> temp.isFixedTicketName;
            selectStream >> temp.fixedTicketName;
            selectStream >> temp.isfirstLineGetUp;
            selectStream >> temp.getUpText;
            selectStream >> temp.isLastLineGetDown;
            selectStream >> temp.getDownText;
            selectStream >> temp.isFirstLineIndex;
            selectStream >> temp.isFirstLineFlag;
            selectStream >> temp.firstLineText;
            selectStream >> temp.isLastLineIndex;
            selectStream >> temp.isLastLineFlag;
            selectStream >> temp.lastLineText;
            selectStream >> temp.isEndFlag;
            int endType;
            selectStream >> endType;
            temp.endType = EndType(endType);
            selectStream >> temp.endText;
            selectStream >> temp.dateTimeFaram;
            selectStream >> temp.currentPageFaram;
            selectStream >> temp.pagesCountFaram;
            selectStream >> temp.isPrintTicketName;
            selectStream >> temp.endPicPath;
            loadCount++;

            int ret = add2List(&temp);
            if(ret < 0)		return -3;
        }

        m_db.commit();
        printf("共 %d 条记录\n", loadCount);
    }catch(otl_exception& p)
    {
        WLOutDebug("查找操作票打印参数设置失败！");
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
作者：张林颢 2014-10-11
**************************************************************************************************/
int CPrintParamConfig::add( TPrintParam *pInputParam )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    TPrintParam temp = {0};
    temp.ID = pInputParam->ID;
    temp.stationID = pInputParam->stationID;
    strcpy(temp.bodyFontFamily, pInputParam->bodyFontFamily);
    temp.fontSize = pInputParam->fontSize;
    temp.indexCol = pInputParam->indexCol;
    temp.contentsCol = pInputParam->contentsCol;
    temp.isFixedTicketName = pInputParam->isFixedTicketName;
    strcpy(temp.fixedTicketName, pInputParam->fixedTicketName);
    temp.isfirstLineGetUp = pInputParam->isfirstLineGetUp;
    strcpy(temp.getUpText, pInputParam->getUpText);
    temp.isLastLineGetDown = pInputParam->isLastLineGetDown;
    strcpy(temp.getDownText, pInputParam->getDownText);
    temp.isFirstLineIndex = pInputParam->isFirstLineIndex;
    temp.isFirstLineFlag = pInputParam->isFirstLineFlag;
    strcpy(temp.firstLineText, pInputParam->firstLineText);
    temp.isLastLineIndex = pInputParam->isLastLineIndex;
    temp.isLastLineFlag = pInputParam->isLastLineFlag;
    strcpy(temp.lastLineText, pInputParam->lastLineText);
    temp.isEndFlag = pInputParam->isEndFlag;
    temp.endType = pInputParam->endType;
    strcpy(temp.endText, pInputParam->endText);
    strcpy(temp.dateTimeFaram, pInputParam->dateTimeFaram);
    strcpy(temp.currentPageFaram, pInputParam->currentPageFaram);
    strcpy(temp.pagesCountFaram, pInputParam->pagesCountFaram);
    temp.isPrintTicketName = pInputParam->isPrintTicketName;
    strcpy(temp.endPicPath, pInputParam->endPicPath);

	int lastAddID;
	char sql[10240] = {0};
    strcat(sql, "insert into t_printparam values(NULL,:f1<unsigned int>,:f2<char[64]>,:f3<unsigned int>,");
    strcat(sql, ":f4<unsigned int>,:f5<unsigned int>,:f6<unsigned int>,:f7<char[64]>,:f8<unsigned int>,:f9<char[64]>,");
    strcat(sql, ":f10<unsigned int>,:f11<char[64]>,:f12<unsigned int>,:f13<unsigned int>,:f14<char[64]>,");
    strcat(sql, ":f15<unsigned int>,:f16<unsigned int>,:f17<char[64]>,:f18<unsigned int>,:f19<unsigned int>,");
	strcat(sql, ":f20<char[64]>,:f21<char[32]>,:f22<char[32]>,:f23<char[32]>,:f24<unsigned int>,:f25<char[255]>)");

	try
	{
		otl_stream insertStream(1, sql, m_db);
		insertStream.set_commit(0);
        insertStream << temp.stationID;
        insertStream << temp.bodyFontFamily;
        insertStream << temp.fontSize;
        insertStream << temp.indexCol;
        insertStream << temp.contentsCol;
        insertStream << temp.isFixedTicketName;
        insertStream << temp.fixedTicketName;
        insertStream << temp.isfirstLineGetUp;
        insertStream << temp.getUpText;
        insertStream << temp.isLastLineGetDown;
        insertStream << temp.getDownText;
        insertStream << temp.isFirstLineIndex;
        insertStream << temp.isFirstLineFlag;
        insertStream << temp.firstLineText;
        insertStream << temp.isLastLineIndex;
        insertStream << temp.isLastLineFlag;
        insertStream << temp.lastLineText;
        insertStream << temp.isEndFlag;
        insertStream << DWORD32(temp.endType);
        insertStream << temp.endText;
        insertStream << temp.dateTimeFaram;
        insertStream << temp.currentPageFaram;
        insertStream << temp.pagesCountFaram;
        insertStream << temp.isPrintTicketName;
        insertStream << temp.endPicPath;

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

		int ret = add2List(&temp);
		if(ret < 0)		return -3;
	}
	catch (otl_exception& p)
	{
        WLOutDebug("添加操作票打印参数设置失败！");
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
作者：张林颢 2014-10-11
**************************************************************************************************/
int CPrintParamConfig::del( const DWORD32 ID )
{
	if(!m_isInit)	return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)		return -5;

    const char *sql = "DELETE FROM t_printparam WHERE ID = :f1_ID<unsigned int>";
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
        WLOutDebug("删除操作票打印参数设置失败！");

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
作者：张林颢 2014-07-23
**************************************************************************************************/
int CPrintParamConfig::update( const DWORD32 ID, TPrintParam *pInputParam )
{
	if(NULL == pInputParam)			return -1;

	if(!m_isInit)					return -2;

    if( checkDBConnect() < 0 )      return -4;

    int ret = isIDExist(ID);
	if(ret < 0)						return -3;

	char sql[10240] = {0};
	strcat(sql, "UPDATE t_printparam set stationID=:f1<unsigned int>,bodyFontFamily=:f2<char[64]>,fontSize=:f3<unsigned int>,");
    strcat(sql, "indexCol=:f4<unsigned int>,contentsCol=:f5<unsigned int>,isFixedTicketName=:f6<unsigned int>,");
    strcat(sql, "fixedTicketName=:f7<char[64]>,isfirstLineGetUp=:f8<unsigned int>,getUpText=:f9<char[64]>,");
    strcat(sql, "isLastLineGetDown=:f10<unsigned int>,getDownText=:f11<char[64]>,isFirstLineIndex=:f12<unsigned int>,");
    strcat(sql, "isFirstLineFlag=:f13<unsigned int>,firstLineText=:f14<char[64]>,isLastLineIndex=:f15<unsigned int>,");
    strcat(sql, "isLastLineFlag=:f16<unsigned int>,lastLineText=:f17<char[64]>,isEndFlag=:f18<unsigned int>,");
    strcat(sql, "endType=:f19<unsigned int>,endText=:f20<char[64]>,dateTimeFaram=:f21<char[32]>,currentPageFaram=:f22<char[32]>,");
	strcat(sql, "PagesCountFaram=:f23<char[32]>,isPrintTicketName=:f24<unsigned int>,endPicPath=:f25<char[255]> ");
    strcat(sql, "WHERE ID=:f26<unsigned int>");

	try
	{
		otl_stream updateStream(1, sql, m_db);
        updateStream << pInputParam->stationID;
        updateStream << pInputParam->bodyFontFamily;
        updateStream << pInputParam->fontSize;
        updateStream << pInputParam->indexCol;
        updateStream << pInputParam->contentsCol;
        updateStream << pInputParam->isFixedTicketName;
        updateStream << pInputParam->fixedTicketName;
        updateStream << pInputParam->isfirstLineGetUp;
        updateStream << pInputParam->getUpText;
        updateStream << pInputParam->isLastLineGetDown;
        updateStream << pInputParam->getDownText;
        updateStream << pInputParam->isFirstLineIndex;
        updateStream << pInputParam->isFirstLineFlag;
        updateStream << pInputParam->firstLineText;
        updateStream << pInputParam->isLastLineIndex;
        updateStream << pInputParam->isLastLineFlag;
        updateStream << pInputParam->lastLineText;
        updateStream << pInputParam->isEndFlag;
        updateStream << DWORD32(pInputParam->endType);
        updateStream << pInputParam->endText;
        updateStream << pInputParam->dateTimeFaram;
        updateStream << pInputParam->currentPageFaram;
        updateStream << pInputParam->pagesCountFaram;
        updateStream << pInputParam->isPrintTicketName;
        updateStream << pInputParam->endPicPath;
        updateStream << pInputParam->ID;

        m_db.commit();

		int ret = updateList(ID,pInputParam);
		if(ret < 0)			return -3;
	}
	catch (otl_exception& p)
	{
        WLOutDebug("更新操作票打印参数设置失败！");

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
作者：张林颢 2014-10-11
**************************************************************************************************/
int CPrintParamConfig::add2List( TPrintParam *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int ret = isIDExist(pInputParam->ID);
	if(ret >= 0)				return -2;

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

    memcpy(&m_pList[m_count], pInputParam, sizeof(TPrintParam));
	m_count++;
	m_pCS->leave();

	return 0;
}

/**************************************************************************************************
功能：从列表中删除指定ID的记录
参数：ID，ID
返回：>0，该ID存在，删除成功
	  -1，ID不存在
作者：张林颢 2014-10-11
**************************************************************************************************/
int CPrintParamConfig::delFromList( const DWORD32 ID )
{
	int index = isIDExist(ID);
	if(index < 0)		return -1;

	m_pCS->enter();

	for(int i=index; i<m_count-1; i++)
	{
        memcpy(&m_pList[i], &m_pList[i+1], sizeof(TPrintParam));
	}
    memset(&m_pList[m_count-1], 0x00, sizeof(TPrintParam));
	m_count--;

	m_pCS->leave();
	return 0;
}

/**************************************************************************************************
功能：判断该ID是否存在
参数：ID，ID
返回：>0，该ID存在
	  -1，ID不存在
作者：张林颢 2014-10-11
**************************************************************************************************/
int CPrintParamConfig::isIDExist( const DWORD32 ID )
{
	int index = -1;

	m_pCS->enter();
	for(int i=0; i<m_count; i++)
	{
		if(m_pList[i].ID== ID)	index = i;
	}
	m_pCS->leave();

	return index;
}

/**************************************************************************************************
功能：申请更大内存
参数：无
返回：>0，申请成功
	  -1，内存申请失败
作者：张林颢 2014-10-11
**************************************************************************************************/
int CPrintParamConfig::getMoreMemory()
{
    int sumCount = m_sumCount + MAX_PRINTPARAM_COUNT;
    TPrintParam *pResource = new TPrintParam[sumCount];

	if(NULL == pResource)
	{
        WLOutDebug("CPrintParamConfig::getMoreMemory()-->申请内存失败!");
		return -1;
	}
	m_sumCount = sumCount;
    memset(pResource, 0x00, sizeof(TPrintParam)*m_sumCount);
    memcpy(pResource, m_pList, sizeof(TPrintParam)*m_count);
	delete [] m_pList;

	m_pList = pResource;
	return 0;
}

/**************************************************************************************************
功能：获取列表数
参数：无
返回：>0，列表数
作者：张林颢 2014-10-11
**************************************************************************************************/
int CPrintParamConfig::getListCount() const
{
	return m_count;
}

/**************************************************************************************************
功能：获取指定索引的记录
参数：无
返回：null，参数错误
作者：张林颢 2014-10-11
**************************************************************************************************/
TPrintParam * CPrintParamConfig::getListByIndex( DWORD32 index ) const
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
作者：张林颢 2014-10-11
**************************************************************************************************/
int CPrintParamConfig::updateList( const DWORD32 ID, const TPrintParam *pInputParam )
{
	if(NULL == pInputParam)		return -1;

	int index = isIDExist(ID);
	if (index < 0)				return -2;

    TPrintParam *temp = getListByIndex(index);
    memcpy(temp, pInputParam, sizeof(TPrintParam));

	return 0;
}

