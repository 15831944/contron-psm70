/********************************************************************************************************************
功能：日志写文件模块，采用面向对象化方式实现，线程不安全
作者：唐小灿	2014-02-28
********************************************************************************************************************/
#pragma once

#include "WLPlatformLayer.h"
#include "WLCriticalSection.h"

class CWLOutDebugFile
{
public:
	CWLOutDebugFile(char * pPath);
	~CWLOutDebugFile();

	int WLOutDebugFile(const char * fmt,...);
	int WLOutDebugFileStream(BYTE * pStream,int len);
	int WLOutDebugFileStream(const char * header,BYTE * pStream,int len);

private:
	void *  m_pHandle;    //文件操作句柄
	char    m_path[256];  //文件存储路径
	TWLTime m_baseTime;   //当前操作基准时间(精确到小时，文件按小时存储)
//	CWLCriticalSection * m_pCSSend;
};

int     WLOutDebugFileInit(const char * pFilePath);
int     WLOutDebugFile(const char * fmt,...);
int     WLOutDebugFileStream(BYTE * pStream,int len);
int     WLOutDebugFileStream(const char * header,BYTE * pStream,int len);
int     WLOutDebugFileClear();
//int     WLOutDebugFileCkeckNewFile();