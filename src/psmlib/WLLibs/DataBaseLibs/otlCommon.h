/**************************************************************************************************
�ļ�����otlCommon.h
��  �ܣ����ݿ�ͨ�ò���
**************************************************************************************************/
#ifndef __OTLCOMMON_H__
#define __OTLCOMMON_H__

#include <Windows.h>


BOOL configDataSoure(const char * pDB, const char * pDBName, const char * pDNS, const char * pDriver);
BOOL configMySqlDataSoure(const char * pAddress, const char * pDBName, const char * pUid, const char * pPwd, const char * pDsn);


#endif
