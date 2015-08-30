#include "stdafx.h"
#include "WLOutDebugFile.h"
#include "../fileSysterm/WLDirect.h"

CWLOutDebugFile::CWLOutDebugFile(char * pPath)
{
	m_pHandle = NULL;
	memset(m_path, 0x00, sizeof(m_path));
	memset((BYTE *)&m_baseTime, 0x00, sizeof(TWLTime));

	if (NULL == pPath)  return;

	//�洢·��
	memcpy(m_path, pPath, strlen(pPath));

	//ȷ���ļ���
	WLGetLocalTime(&m_baseTime);
	char filePath[256] = {0};
	sprintf(filePath, "%s\\%04d%02d%02d%02d%02d.txt",pPath,m_baseTime.year,m_baseTime.month,m_baseTime.day,m_baseTime.hour,m_baseTime.minute);

	//�����ļ�
	FILE * fp = fopen(filePath, "wb");
	if (NULL != fp)
	{
		m_pHandle = (void *)fp;
	}
}


CWLOutDebugFile::~CWLOutDebugFile()
{
	if (NULL == m_pHandle)
		return;

	FILE * fp = (FILE *)m_pHandle;
	fclose(fp);
}


int CWLOutDebugFile::WLOutDebugFile(const char * fmt,...)
{
	if (NULL == m_pHandle)  return -1;
	FILE * fp = (FILE *)m_pHandle;
//m_pCSSend->enter();
	//static char sprint_buf[1024]; 
	//memset(sprint_buf,0,1024);
	char sprint_buf[1024] = {0};

	TWLTime t = {0};
	WLGetLocalTime(&t);
	//��Сʱ�����ļ�
	//if (t.hour != m_baseTime.hour)
	//{
	//	fclose(fp);
	//	m_pHandle = 0;
	//	char filePath[256] = {0};
	//	sprintf(filePath, "%s\\%04d%02d%02d%02d.txt",m_path,t.year,t.month,t.day,t.hour);
	//	//�����ļ�
	//	fp = fopen(filePath, "wb");
	//	if (NULL != fp)
	//	{
	//		m_pHandle = (void *)fp;
	//	}
	//	memcpy((BYTE*)&m_baseTime, (BYTE*)&t, sizeof(TWLTime));
	//}
	//�ְ�Сʱ�����ļ�
	if ((t.minute==0 && m_baseTime.minute != 0) || (t.minute==30 && m_baseTime.minute != 30))
	{
		fclose(fp);
		m_pHandle = 0;
		char filePath[256] = {0};
		sprintf(filePath, "%s\\%04d%02d%02d%02d%02d.txt",m_path,t.year,t.month,t.day,t.hour,t.minute);
		//�����ļ�
		fp = fopen(filePath, "wb");
		if (NULL != fp)
		{
			m_pHandle = (void *)fp;
		}
		memcpy((BYTE*)&m_baseTime, (BYTE*)&t, sizeof(TWLTime));
	}
	int timeLen = sprintf(sprint_buf, "[%04d%02d%02d%02d%02d%02d] ", t.year,t.month,t.day,t.hour,t.minute,t.second);

	va_list args; 
	int n; 
	va_start(args, fmt); 
	n = vsprintf(&sprint_buf[timeLen], fmt, args); 
	va_end(args);  
	sprint_buf[timeLen+n] = '\n';

	int writeLen = fwrite( sprint_buf, 1, strlen(sprint_buf), fp);
	fflush(fp);
//m_pCSSend->leave();
	return writeLen;
}


int CWLOutDebugFile::WLOutDebugFileStream(BYTE * pStream,int len)
{
	if (NULL == m_pHandle)  return -1;
	FILE * fp = (FILE *)m_pHandle;
//m_pCSSend->enter();
	//static char buf[1024] = {0};
	//memset(buf,0,1024);
	if (len >64) len = 64;
	char buf[1024] = {0};

	int useLen = 0;
	TWLTime t = {0};
	WLGetLocalTime(&t);
	//��Сʱ�����ļ�
	//if (t.hour != m_baseTime.hour)
	//{
	//	fclose(fp);
	//	m_pHandle = 0;
	//	char filePath[256] = {0};
	//	sprintf(filePath, "%s\\%04d%02d%02d%02d.txt",m_path,t.year,t.month,t.day,t.hour);
	//	//�����ļ�
	//	fp = fopen(filePath, "wb");
	//	if (NULL != fp)
	//	{
	//		m_pHandle = (void *)fp;
	//	}
	//	memcpy((BYTE*)&m_baseTime, (BYTE*)&t, sizeof(TWLTime));
	//}
	//�ְ�Сʱ�����ļ�
	if ((t.minute==0 && m_baseTime.minute != 0) || (t.minute==30 && m_baseTime.minute != 30))
	{
		fclose(fp);
		m_pHandle = 0;
		char filePath[256] = {0};
		sprintf(filePath, "%s\\%04d%02d%02d%02d%02d.txt",m_path,t.year,t.month,t.day,t.hour,t.minute);
		//�����ļ�
		fp = fopen(filePath, "wb");
		if (NULL != fp)
		{
			m_pHandle = (void *)fp;
		}
		memcpy((BYTE*)&m_baseTime, (BYTE*)&t, sizeof(TWLTime));
	}
	useLen = sprintf(buf, "[%04d%02d%02d%02d%02d%02d] ", t.year,t.month,t.day,t.hour,t.minute,t.second);

//	for (int i =0;i<len;i++)	useLen += sprintf(&buf[useLen+i*2],"%.2X",pStream[i]);
	for (int i =0;i<len;i++)	useLen += sprintf(&buf[useLen],"%.2X",pStream[i]);
	buf[useLen] = '\n';

//	int lenlen = strlen(buf);
	int writeLen = fwrite( buf, 1, strlen(buf), fp);
	fflush(fp);
//m_pCSSend->leave();
	return writeLen;
}


int CWLOutDebugFile::WLOutDebugFileStream(const char * header,BYTE * pStream,int len)
{
	if (NULL == m_pHandle)  return -1;
	FILE * fp = (FILE *)m_pHandle;
//m_pCSSend->enter();
	//static char buf[1024] = {0};
	//memset(buf,0,1024);
	int headerlen = strlen(header);
	if (len - headerlen > 64)	len = 64 - headerlen;
	char buf[1024] = {0};

	int useLen = 0;
	TWLTime t = {0};
	WLGetLocalTime(&t);
	//��Сʱ�����ļ�
	//if (t.hour != m_baseTime.hour)
	//{
	//	fclose(fp);
	//	m_pHandle = 0;
	//	char filePath[256] = {0};
	//	sprintf(filePath, "%s\\%04d%02d%02d%02d.txt",m_path,t.year,t.month,t.day,t.hour);
	//	//�����ļ�
	//	fp = fopen(filePath, "wb");
	//	if (NULL != fp)
	//	{
	//		m_pHandle = (void *)fp;
	//	}
	//	memcpy((BYTE*)&m_baseTime, (BYTE*)&t, sizeof(TWLTime));
	//}
	//�ְ�Сʱ�����ļ�
	if ((t.minute==0 && m_baseTime.minute != 0) || (t.minute==30 && m_baseTime.minute != 30))
	{
		fclose(fp);
		m_pHandle = 0;
		char filePath[256] = {0};
		sprintf(filePath, "%s\\%04d%02d%02d%02d%02d.txt",m_path,t.year,t.month,t.day,t.hour,t.minute);
		//�����ļ�
		fp = fopen(filePath, "wb");
		if (NULL != fp)
		{
			m_pHandle = (void *)fp;
		}
		memcpy((BYTE*)&m_baseTime, (BYTE*)&t, sizeof(TWLTime));
	}

	useLen += sprintf(buf, "[%04d%02d%02d%02d%02d%02d] ", t.year,t.month,t.day,t.hour,t.minute,t.second);

	memcpy(&buf[useLen],header,headerlen);
	useLen += headerlen;
	for (int i = 0;i<len;i++) useLen += sprintf(&buf[useLen],"%.2X",pStream[i]);
	buf[useLen] = '\n';

	int writeLen = fwrite( buf, 1, strlen(buf), fp);
	fflush(fp);
//m_pCSSend->leave();
	return writeLen;
}


























void  * g_pHandle   = NULL;
char    g_path[256] = {0};
TWLTime g_baseTime  = {0};
CWLCriticalSection * g_pCSSend = NULL;
/******************************************************************
����:	��ʼ����־�ļ�
����:	·��
����:	>0�ɹ�
        <0ʧ��
����:	��С��	2014-04-26
******************************************************************/
int WLOutDebugFileInit(const char * pFilePath)
{
	if (NULL == pFilePath)  return -1;

	g_pHandle = NULL;
	memset(g_path, 0x00, sizeof(g_path));
	memset((BYTE *)&g_baseTime, 0x00, sizeof(TWLTime));

	//�洢·��
	memcpy(g_path, pFilePath, strlen(pFilePath));

	//ȷ���ļ���
	WLGetLocalTime(&g_baseTime);
	char filePath[256] = {0};
	sprintf(filePath, "%s\\%04d%02d%02d%02d%02d.txt",pFilePath,g_baseTime.year,g_baseTime.month,g_baseTime.day,g_baseTime.hour,g_baseTime.minute);

	//�����ļ�
	FILE * fp = fopen(filePath, "wb");
	if (NULL == fp)
	{
		return -1;
	}

	g_pHandle = (void *)fp;

	g_pCSSend = new CWLCriticalSection();

	return 1;
}


/******************************************************************
����:	д��־�ļ�
����:	��ʽ������
����:	>0�ɹ�
        <0ʧ��
����:	��С��	2014-04-26
******************************************************************/
int    WLOutDebugFile(const char * fmt,...)
{
	if (NULL == g_pHandle)  return -1;
	FILE * fp = (FILE *)g_pHandle;
    
	//���ٽ���
	g_pCSSend->enter();

	char sprint_buf[1024] = {0};

	TWLTime t = {0};
	WLGetLocalTime(&t);

	//�ְ�Сʱ�����ļ�
	if ((t.minute==0 && g_baseTime.minute != 0) || (t.minute==30 && g_baseTime.minute != 30))
	{
		fclose(fp);
		g_pHandle = 0;
		char filePath[256] = {0};
		sprintf(filePath, "%s\\%04d%02d%02d%02d%02d.txt",g_path,t.year,t.month,t.day,t.hour,t.minute);
		//�����ļ�
		fp = fopen(filePath, "wb");
		if (NULL != fp)
		{
			g_pHandle = (void *)fp;
		}
		memcpy((BYTE*)&g_baseTime, (BYTE*)&t, sizeof(TWLTime));
	}
//	WLOutDebugFileCkeckNewFile();

	int timeLen = sprintf(sprint_buf, "[%04d%02d%02d%02d%02d%02d] ", t.year,t.month,t.day,t.hour,t.minute,t.second);

	va_list args; 
	int n; 
	va_start(args, fmt); 
	n = vsprintf(&sprint_buf[timeLen], fmt, args); 
	va_end(args);  
	sprint_buf[timeLen+n] = '\n';

	int writeLen = fwrite( sprint_buf, 1, strlen(sprint_buf), fp);
	fflush(fp);

	//���ٽ���
    g_pCSSend->leave();

	return writeLen;
}


/******************************************************************
����:	д��־�ļ���д����ʽ����
����:	pStream��������
        len��    �����ݳ���
����:	>0�ɹ�
        <0ʧ��
����:	��С��	2014-04-26
******************************************************************/
int     WLOutDebugFileStream(BYTE * pStream,int len)
{
	if (NULL == g_pHandle)  return -1;
	FILE * fp = (FILE *)g_pHandle;

	//���ٽ���
	g_pCSSend->enter();

	if (len >64) len = 64;
	char buf[1024] = {0};

	int useLen = 0;
	TWLTime t = {0};
	WLGetLocalTime(&t);

	//�ְ�Сʱ�����ļ�
	if ((t.minute==0 && g_baseTime.minute != 0) || (t.minute==30 && g_baseTime.minute != 30))
	{
		fclose(fp);
		g_pHandle = 0;
		char filePath[256] = {0};
		sprintf(filePath, "%s\\%04d%02d%02d%02d%02d.txt",g_path,t.year,t.month,t.day,t.hour,t.minute);
		//�����ļ�
		fp = fopen(filePath, "wb");
		if (NULL != fp)
		{
			g_pHandle = (void *)fp;
		}
		memcpy((BYTE*)&g_baseTime, (BYTE*)&t, sizeof(TWLTime));
	}
//	WLOutDebugFileCkeckNewFile();

	useLen = sprintf(buf, "[%04d%02d%02d%02d%02d%02d] ", t.year,t.month,t.day,t.hour,t.minute,t.second);

	for (int i =0;i<len;i++)	useLen += sprintf(&buf[useLen],"%.2X",pStream[i]);
	buf[useLen] = '\n';


	int writeLen = fwrite( buf, 1, strlen(buf), fp);
	fflush(fp);

	//���ٽ���
	g_pCSSend->leave();

	return writeLen;
}


/******************************************************************
����:	д��־�ļ���д����ʽ����
����:	header�� ���������־����
        pStream��������
        len��    �����ݳ���
����:	>0�ɹ�
        <0ʧ��
����:	��С��	2014-04-26
******************************************************************/
int     WLOutDebugFileStream(const char * header,BYTE * pStream,int len)
{
	if (NULL == g_pHandle)  return -1;
	FILE * fp = (FILE *)g_pHandle;

	//���ٽ���
	g_pCSSend->enter();

	int headerlen = strlen(header);
	if (len - headerlen > 64)	len = 64 - headerlen;
	char buf[1024] = {0};

	int useLen = 0;
	TWLTime t = {0};
	WLGetLocalTime(&t);
	
	//�ְ�Сʱ�����ļ�
	if ((t.minute==0 && g_baseTime.minute != 0) || (t.minute==30 && g_baseTime.minute != 30))
	{
		fclose(fp);
		g_pHandle = 0;
		char filePath[256] = {0};
		sprintf(filePath, "%s\\%04d%02d%02d%02d%02d.txt",g_path,t.year,t.month,t.day,t.hour,t.minute);
		//�����ļ�
		fp = fopen(filePath, "wb");
		if (NULL != fp)
		{
			g_pHandle = (void *)fp;
		}
		memcpy((BYTE*)&g_baseTime, (BYTE*)&t, sizeof(TWLTime));
	}
//	WLOutDebugFileCkeckNewFile();

	useLen += sprintf(buf, "[%04d%02d%02d%02d%02d%02d] ", t.year,t.month,t.day,t.hour,t.minute,t.second);

	memcpy(&buf[useLen],header,headerlen);
	useLen += headerlen;
	for (int i = 0;i<len;i++) useLen += sprintf(&buf[useLen],"%.2X",pStream[i]);
	buf[useLen] = '\n';

	int writeLen = fwrite( buf, 1, strlen(buf), fp);
	fflush(fp);

	//���ٽ���
	g_pCSSend->leave();

	return writeLen;
}


/******************************************************************
����:	�������
����:	��
����:	>0�ɹ�
        <0ʧ��
����:	��С��	2014-04-26
******************************************************************/
int     WLOutDebugFileClear()
{
	if (NULL == g_pHandle)  return -1;
	FILE * fp = (FILE *)g_pHandle;

	//���ٽ���
	g_pCSSend->enter();

	fclose(fp);
	g_pHandle = NULL;
	if (NULL != g_pCSSend)  
	{
		delete g_pCSSend;
		g_pCSSend = NULL;
	}

	//���ٽ���
	g_pCSSend->leave();

	return 1;
}


//int     WLOutDebugFileCkeckNewFile()
//{
//	TWLTime t = {0};
//	WLGetLocalTime(&t);
//
//	//�ְ�Сʱ�����ļ�
//	if ((t.minute==0 && g_baseTime.minute != 0) || (t.minute==30 && g_baseTime.minute != 30))
//	{
//		fclose(fp);
//		g_pHandle = 0;
//		char filePath[256] = {0};
//		sprintf(filePath, "%s\\%04d%02d%02d%02d%02d.txt",g_path,t.year,t.month,t.day,t.hour,t.minute);
//		//�����ļ�
//		fp = fopen(filePath, "wb");
//		if (NULL != fp)
//		{
//			g_pHandle = (void *)fp;
//		}
//		memcpy((BYTE*)&g_baseTime, (BYTE*)&t, sizeof(TWLTime));
//	}
//}