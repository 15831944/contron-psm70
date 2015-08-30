/**************************************************************************************************
文件名:BusinessDataSet.h: interface for the CBusinessDataSet class.
功  能:封装数据库操作的数据集类，是所有操作数据库模块的基类，所有集体的数据库操作都需要集成此类
作  者：李施施 2014-07-08
**************************************************************************************************/

#ifndef BUSINESSDATASET__H
#define	BUSINESSDATASET__H

#pragma once

#include <stdio.h>
#include "BusinessDataType.h"
#include "platformLayer/WLCriticalSection.h"
#include "platformLayer/WLPlatformLayer.h"
#include "DataBaseLibs/WLDatabaseDefine.h"

#define  MAX_PARAM_LEN 256				

class CBusinessDataSet  
{
public:
	CBusinessDataSet();
	virtual ~CBusinessDataSet() = 0;

	int setConnectParam( const char * uid,const char * psw,const char * dsn, const char *IP= 0);
	int setConnectParam(DWORD32 dbType,const char * driver,const char * dbName, const char * uid,const char * psw,const char * dsn, const char *IP);
	int connect();
    int reConnect();
	int disConnect();
    int checkDBConnect();

	int retrieve();
	int del(const DWORD32 ID);

protected:
	int connectToMySQL(const char * driver,const char * dbName, const char * uid,const char * psw,const char * dsn, const char *IP);
	int connectToSQLite(const char * driver,const char * dbName, const char * uid,const char * psw,const char * dsn, const char *IP);
	int connectToSQLServer(const char * driver,const char * dbName, const char * uid,const char * psw,const char * dsn, const char *IP);

protected:
	CWLCriticalSection * m_pCS; 					//临界区

    DWORD32 m_dbType;                               //连接数据库类型：0，MySql;1,SQLite;2,SQLServer
	char m_IP[MAX_PARAM_LEN];						//IP地址
	char m_uid[MAX_PARAM_LEN];						//数据库用户名
	char m_psw[MAX_PARAM_LEN];						//数据库密码
	char m_dsn[MAX_PARAM_LEN];						//数据源
	char m_driver[MAX_PARAM_LEN];					//数据库驱动名称
	char m_dbName[MAX_PARAM_LEN];					//数据库名称

	otl_connect m_db;								//数据库操作对象
	bool m_isInit;									//数据库是否连接成功
    bool m_isRemoteAccess;							//是否为远程连接（是否代码中配置数据源）
};

#endif 
