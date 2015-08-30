/********************************************************************************************************************
���ܣ���־д�ļ�ģ�飬����������󻯷�ʽʵ�֣��̲߳���ȫ
���ߣ���С��	2014-02-28
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
	void *  m_pHandle;    //�ļ��������
	char    m_path[256];  //�ļ��洢·��
	TWLTime m_baseTime;   //��ǰ������׼ʱ��(��ȷ��Сʱ���ļ���Сʱ�洢)
//	CWLCriticalSection * m_pCSSend;
};

int     WLOutDebugFileInit(const char * pFilePath);
int     WLOutDebugFile(const char * fmt,...);
int     WLOutDebugFileStream(BYTE * pStream,int len);
int     WLOutDebugFileStream(const char * header,BYTE * pStream,int len);
int     WLOutDebugFileClear();
//int     WLOutDebugFileCkeckNewFile();