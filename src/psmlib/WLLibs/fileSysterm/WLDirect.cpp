// WLDirect.cpp: implementation of the CWLDirect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WLDirect.h"
#include <stdio.h>
#include <stdlib.h>
#include "../platformLayer/WLOutDebug.h"

#ifdef WL_OS_CORE_WIN
#include "windows.h"
#include <io.h>
#include <tchar.h>
#include <direct.h>
#include <errno.h>
#elif defined WL_OS_CORE_LINUX  
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#endif

#ifdef WL_IDE_VC6
	#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[]=__FILE__;
	#define new DEBUG_NEW
	#endif
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*********************************************************************
���ܣ���ȡ��ǰ����·��
������dir������������ڴ�ռ��ɵ��������룩
      len��dir�ռ��С
���أ�1���ɹ���
      <=0��ʧ�ܣ�
	  0���ռ����̫С��
	  -1������������Ϸ�
	  -2�����������ض�ƽ̨�ϲ�֧�֣�
      -3��ִ��ʧ��
���ߣ�Ҷ����	2013-11-27 ������޸� GetCurrentDirectoryA(len,dir)����ֵ����0��ɹ�
*********************************************************************/
int WLGetCurrentDirectory(char * dir,DWORD len)
{
	if (dir == NULL)	return -1;

#ifdef WL_OS_CORE_WIN
	DWORD ret = GetCurrentDirectoryA(len,dir);
	if (ret > 0)	return 1;
	return -3;
#elif defined WL_OS_CORE_LINUX
	char * ret = getcwd(dir, len);
	if (ret == NULL)	return -3;	else return 1;
#endif	
}

/*********************************************************************
���ܣ���ȡ��ǰ����ȫ·���ļ���
������fullName������������ڴ�ռ��ɵ��������룩
      len��fullName�ռ��С
���أ�1���ɹ���
      <=0��ʧ�ܣ�
	  0���ռ����̫С��
	  -1������������Ϸ�
	  -2�����������ض�ƽ̨�ϲ�֧�֣�
      -3��ִ��ʧ��
���ߣ�Ҷ����	2013-11-27
*********************************************************************/
int WLGetModuleFileName(char * fullName,DWORD len )
{
	if (fullName == NULL)	return -1;

#ifdef WL_OS_CORE_WIN
	DWORD ret = GetModuleFileNameA(NULL,fullName,len);
	if (ret == 0)	return -3;
	else if (ret >0)	return 1;
	else return -3;
#elif defined WL_OS_CORE_LINUX
	return -3;
#endif
}

/*********************************************************************
���ܣ��ж�Ŀ¼�Ƿ����
������dir��Ŀ¼ȫ��
���أ�1�����ڣ�
      -1��������
���ߣ�Ҷ����	2013-11-27
*********************************************************************/
int WLIsExsitDir(const char * dir)
{
	if (dir == NULL)	return -1;
#ifdef WL_OS_CORE_WIN
	return -2;
#elif defined WL_OS_CORE_LINUX
	DIR *dp;
	if((dp = opendir(dir)) == NULL) 
	{
		return 0;
	}

	if (dp)
	{
		closedir(dp);
	}
	return 1;
#endif
}

/*********************************************************************
���ܣ�����Ŀ¼
������dir��Ŀ¼ȫ��
���أ�1���ɹ���
      <=0��ʧ�ܣ�
���ߣ�Ҷ����	2013-11-27
�޸ģ���Զ�� ����QT����Ŀ¼
*********************************************************************/
#ifdef WL_3LIB_QT      //ʹ����QT��
#include <QDir>
#endif

int WLMakeDir(const char * dir)
{
	if (dir == NULL)	return -1;
	int rev = 0;
#ifdef WL_OS_CORE_WIN
  #ifdef WL_3LIB_QT      //ʹ����QT��
	QDir temp;
	bool exist = temp.exists(dir);
	if(exist)
		return 0;
	else
	{
		bool ok = temp.mkdir(dir);
		if( ok )	return 1;
		else return -2;
	}
  #else
  	rev = mkdir(dir);
	if (0 == rev)       return 1;

	//����
	if (EEXIST == errno)         //�Ѿ�����
		return -1;
	else if (ENOENT == errno)    //·������
		return -2;
	return -3;                   //����
  #endif
#elif defined WL_OS_CORE_LINUX
	rev = mkdir(dir,0777);
	if (0 == rev)       return 1;
	WLOutDebug("madir failed, errno is %d", errno);

	//����
	if (EEXIST == errno)         //�Ѿ�����
		return -1;
	else if (ENOENT == errno)    //·������
		return -2;
	return -3;                   //����
#endif
}

/*********************************************************************
���ܣ�ɾ��Ŀ¼
������dir��Ŀ¼ȫ��
���أ�1���ɹ���
      -1��ʧ��
���ߣ�Ҷ����	2013-11-27
*********************************************************************/
int WLDelDir(const char * dir)
{
	if (dir == NULL)	return -1;
	int rev = 0;
#ifdef WL_OS_CORE_WIN
	//��ɾ���ļ��м���������
	char cmd[256] = {0};
	sprintf(cmd, "rd/s/q %s", dir);
	rev = system(cmd);
	if (0 != rev)
	{
		return -1;
	}
	return 1;
#elif defined WL_OS_CORE_LINUX
	DIR *dp;
	if((dp = opendir(dir)) == NULL) 
	{
		WLOutDebug("WLDelDir opendir failed, errno is %d", errno);
		return -1;
	}

	rev = rmdir(dir);
	WLOutDebug("WLDelDir rmdir rev is %d", rev);
	closedir(dp);
	if (0 != rev)
		return -1;
	return 1;

//	char cmd[256] = {0};
//	//sprintf(cmd, "rmdir %s", dir);
//	sprintf(cmd, "rm -rf %s", dir);
////	rev = system(cmd);
//	rev = rmdir(dir);
//	WLOutDebug("WLDelDir system rev is %d", rev);
//	if (0 != rev)
//	{
//		WLOutDebug("WLDelDir system errno is %d", errno);
//		return -1;
//	}
//	return 1;

	//rev = remove(dir);
	//if (0 == rev)  return 1;
	////����
	//if (ENOENT == errno)    //Ŀ¼������
	//	return -1;
	//return -2;              //����
#endif
}


/*********************************************************************
���ܣ���ȡָ��Ŀ¼�׸��ļ���Ϣ
������dir��Ŀ¼ȫ��
      pFindFileData�������ļ���Ϣ�ռ�
���أ����
      NULL��ʧ�ܣ�
���ߣ���С��	2013-11-28
*********************************************************************/
TWLHandle WLFindFirstFile(const char * dir,TFindData * pFindFileData)
{
#ifdef WL_OS_CORE_WIN
	if ((NULL == dir) || (NULL == pFindFileData))  return NULL;
	WIN32_FIND_DATA data = {0};
	wchar_t wdestbuf[1024] = {0};
	wchar_t wsrcbuf[1024]  = {0};
	WLchar2wchar(dir, wdestbuf, sizeof(wdestbuf));
	WLchar2wchar("\\*.*", wsrcbuf, sizeof(wsrcbuf));
//	_tcscat_s(wdestbuf, _T("\\*.*"));     //����һ��Ҫָ��ͨ�������Ȼ�����ȡ�����ļ���Ŀ¼
	wcscat(wdestbuf, wsrcbuf);     //����һ��Ҫָ��ͨ�������Ȼ�����ȡ�����ļ���Ŀ¼
	HANDLE h = ::FindFirstFile(wdestbuf, &data);
	if (NULL == h)  return NULL;

	//������Ϣ
	pFindFileData->dwFileAttributes = data.dwFileAttributes;
	//pFindFileData->ftCreationTime = data.ftCreationTime;
	//pFindFileData->ftLastAccessTime = data.ftLastAccessTime;
	//pFindFileData->ftLastWriteTime = data.ftLastWriteTime;
	pFindFileData->nFileSizeHigh = data.nFileSizeHigh;
	pFindFileData->nFileSizeLow = data.nFileSizeLow;
	WLwchar2char(data.cFileName, pFindFileData->cFileName, sizeof(pFindFileData->cFileName));
//	strcpy(pFindFileData->cFileName, data->cFileName);
	return (TWLHandle)h;
#elif defined WL_OS_CORE_LINUX
	if ((NULL == dir) || (NULL == pFindFileData))  return NULL;

	DIR *dp;
	struct dirent *entry;
    struct stat statbuf;
	if((dp = opendir(dir)) == NULL) 
	{
		WLOutDebug("WLFindFirstFile opendir failed, errno is %d", errno);
		return NULL;
	}

	chdir(dir);

	int rev = 0;
	int flag = 0;
	while ((entry = readdir(dp)) != NULL)
	{
		rev = lstat(entry->d_name,&statbuf);
		if(S_ISDIR(statbuf.st_mode))
		{
			if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
				continue;
		}

		//������Ϣ
		pFindFileData->dwFileAttributes = statbuf.st_mode;
		//pFindFileData.ftLastAccessTime = statbuf.st_atime;
		//pFindFileData.ftLastWriteTime  = statbuf.st_mtime;
		//pFindFileData.ftCreationTime   = statbuf.st_ctime;
		pFindFileData->nFileSizeHigh    = 0;
		pFindFileData->nFileSizeLow     = statbuf.st_size;
		strcpy(pFindFileData->cFileName, entry->d_name);

		flag = 1;
		break;
	}

	if (flag <= 0)  
	{
		closedir(dp);
		return NULL;
	}

	return (TWLHandle)dp;
#endif
}


/*********************************************************************
���ܣ���ȡ��һ���ļ���Ϣ
������hdl��WLFindFirstFile�������ľ��
      pFindFileData�������ļ���Ϣ�ռ�
���أ�TRUE���ɹ�
	  FALSE��ʧ�ܣ�
���ߣ���С��	2013-11-28
*********************************************************************/
BOOL WLGetNextFile(TWLHandle hdl,TFindData * pFindFileData)
{
#ifdef WL_OS_CORE_WIN
	if ((NULL == hdl) || (NULL == pFindFileData))  return FALSE;
	HANDLE h = (HANDLE)hdl;
	WIN32_FIND_DATA data = {0};

	BOOL rev = ::FindNextFile(h, &data);
	if (!rev)  return rev;

	//������Ϣ
	pFindFileData->dwFileAttributes = data.dwFileAttributes;
	//pFindFileData->ftCreationTime = data.ftCreationTime;
	//pFindFileData->ftLastAccessTime = data.ftLastAccessTime;
	//pFindFileData->ftLastWriteTime = data.ftLastWriteTime;
	pFindFileData->nFileSizeHigh = data.nFileSizeHigh;
	pFindFileData->nFileSizeLow = data.nFileSizeLow;
	WLwchar2char(data.cFileName, pFindFileData->cFileName, sizeof(pFindFileData->cFileName));
	//strcpy(pFindFileData->cFileName, data->cFileName);

	return TRUE;

#elif defined WL_OS_CORE_LINUX
	if ((NULL == hdl) || (NULL == pFindFileData))  return FALSE;

	DIR *dp;
	struct dirent *entry;
    struct stat statbuf;
	dp = (DIR *)hdl;

	int rev = 0;
	while (NULL != (entry = readdir(dp)))
	{
		rev = lstat(entry->d_name,&statbuf);
		if(S_ISDIR(statbuf.st_mode))
		{
			if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
				continue;
		}

		//������Ϣ
		pFindFileData->dwFileAttributes = statbuf.st_mode;
		//pFindFileData.ftLastAccessTime = statbuf.st_atime;
		//pFindFileData.ftLastWriteTime  = statbuf.st_mtime;
		//pFindFileData.ftCreationTime   = statbuf.st_ctime;
		pFindFileData->nFileSizeHigh    = 0;
		pFindFileData->nFileSizeLow     = statbuf.st_size;
		strcpy(pFindFileData->cFileName, entry->d_name);

		return TRUE;
	}

	return FALSE;
#endif
}


/*********************************************************************
���ܣ��ر�WLFindFirstFile�����ľ��
������hdl��WLFindFirstFile�������ľ��
���أ�TRUE���ɹ�
      FALSE��ʧ�ܣ�
���ߣ���С��	2013-11-28
*********************************************************************/
BOOL WLCloseFindFile(TWLHandle hdl)
{
#ifdef WL_OS_CORE_WIN
	if (NULL == hdl)  return FALSE;

	HANDLE h = (HANDLE)hdl;
	return ::FindClose(h);

#elif defined WL_OS_CORE_LINUX
	if (NULL == hdl)  return FALSE;

	DIR *dp = (DIR *)hdl;
	closedir(dp);
	return TRUE;
#endif
}


/*********************************************************************
���ܣ��ж��Ƿ�Ϊ�ļ���
������hdl��WLFindFirstFile�������ľ��
���أ�TRUE���ɹ�
      FALSE��ʧ�ܣ�
���ߣ���С��	2013-11-28
�޸ģ���С��    2014-09-19  ����windows����
*********************************************************************/
BOOL WLIsFolder(TFindData * pFindData)
{
	if (NULL == pFindData)  return FALSE;
#ifdef WL_OS_CORE_WIN
//	if (pFindData->cFileName[0] == _T('.'))  return FALSE;
	if (pFindData->cFileName[0] == '.')  return FALSE;
	if (pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return TRUE;
	}
	return FALSE;
#elif defined WL_OS_CORE_LINUX
	if(S_ISDIR((mode_t)pFindData->dwFileAttributes))
	{
		return TRUE;
	}
	else
	{
		//����sdcard,��android�б�ʶ��Ϊ���ļ��С���׼ȷ���ļ��а���ָ���ַ������
		char * p = strstr(pFindData->cFileName, "sdcard");
		if (p)  return TRUE;
	}

	return FALSE;
#endif
}


/*********************************************************************
���ܣ��ж��Ƿ�Ϊ�ļ���
������hdl��WLFindFirstFile�������ľ��
���أ�TRUE���ɹ�
      FALSE��ʧ�ܣ�
���ߣ���С��	2014-09-19  ����windows���ж�
*********************************************************************/
BOOL WLIsStringFolder(const char * pPath)
{
	if (NULL == pPath)  return FALSE;

#ifdef WL_OS_CORE_WIN
	int ret = _access(pPath, 0);
	if (-1 == ret)  return FALSE;
	return TRUE;
#endif

#ifdef WL_OS_CORE_LINUX
	int result = -1;
	struct stat my_stat;
	lstat(pPath, &my_stat);
	result = S_ISDIR(my_stat.st_mode);
	if (result > 0)  return TRUE;
	else             return FALSE;
#endif
}


void WLchar2wchar(const char *chr, wchar_t *wchar, int size)
{
#ifdef WL_OS_CORE_WIN
	MultiByteToWideChar(CP_ACP, 0, chr, strlen(chr)+1, wchar, size/sizeof(wchar[0]));
#elif defined WL_OS_CORE_LINUX
    //remove warning
    chr = chr; wchar = wchar; size = size;
#endif
}


void WLwchar2char(const wchar_t *wchar, char *chr, int length)
{
#ifdef WL_OS_CORE_WIN
	WideCharToMultiByte( CP_ACP, 0, wchar, -1, chr, length, NULL, NULL);
#elif defined WL_OS_CORE_LINUX
    //remove warning
    chr = chr; wchar = wchar; length = length;
#endif
}

/*********************************************************************
���ܣ����ļ��ľ���·������ȡ�ļ���·��
������dir���ļ��ľ���·��
	  folderDir����������������ļ��о���·��
	  len,folderDir������ڴ�ռ�
���أ�<0��ʧ�ܡ�-1����������Ƿ�
     >0���ɹ�,·������
���ߣ���Զ��  2013-04-01
*********************************************************************/
int WLGetFolderDir( const char *pdir,char *folderDir,int len )
{
	if (NULL == pdir || NULL == folderDir || len <= 0)	return -1;
	memset(folderDir,0,len);

	int strLen = strlen(pdir);
	const char *pIndex = pdir + strLen - 1;
	while(*pIndex != '\\' && *pIndex != '/')	pIndex--;

	strLen = pIndex - pdir;

	if (strLen > len)	return -2;
	if (pIndex <= pdir)//��'\\'��'/'
	{		
		memcpy(folderDir,pdir,strlen(pdir));
		return strlen(pdir);
	}

	memcpy(folderDir,pdir,strLen);
	return strLen;
}

/*********************************************************************
���ܣ����ļ���·������ȡ�ļ�������
������dir���ļ��еľ���·��
	  folderName����������������ļ�������
	  len,folderName������ڴ�ռ�
���أ�<0��ʧ�ܡ�-1����������Ƿ�
     >0���ɹ�,·������
���������룺��D:/001/002/003��
	  �������003��
���ߣ���Զ��  2013-04-01
*********************************************************************/
int WLGetSubFolderName( const char *pdir,char *folderName,int len )
{
	if (NULL == pdir || NULL == folderName || len <= 0)	return -1;
	memset(folderName,0,len);

	int strLen = strlen(pdir);
	const char *pIndex = pdir + strLen - 1;
	while((*pIndex != '\\') && (*pIndex != '/') && (pIndex >= pdir))	pIndex--;

	if((*pIndex == '\\') || (*pIndex == '/'))	pIndex++;
	int cpyLen = pdir + strLen - pIndex;
	memcpy(folderName,pIndex,cpyLen);

	return 0;
}

/*********************************************************************
���ܣ����ļ��ľ���·������ȡ�ļ�����
������dir���ļ��ľ���·��
	  fileName����������������ļ�����
	  len,fileName������ڴ�ռ�
���أ�<0��ʧ�ܡ�-1����������Ƿ�
     >0���ɹ�,·������
���ߣ���Զ��  2013-04-01
*********************************************************************/
int WLGetFileNameFromDir( const char *pdir,char *fileName,int len )
{
	int ret = -1;
	if (NULL == pdir || NULL == fileName || len <= 0)	return ret;
	memset(fileName,0,len);

	//�ַ�������
	int strLen = strlen(pdir);
	//int point = strLen - 1;
	//ָ��ָ���ַ���ĩβ
	const char *pIndex = pdir + strLen - 1;

	//��ĩβ��ʼ���ҳ��ֵĵ�һ����/����\��
	while((*pIndex != '\\') && (*pIndex != '/') && (pIndex >= pdir))	
	{		
		pIndex--;
	}

	ret--;
	if (pIndex <= pdir)//��'\\'��'/'
	{
		if (strLen > len)	return ret;

		memcpy(fileName,pdir,strLen);
	}
	else			  //��'\\'��'/'
	{
		pIndex++;
		strLen = strlen(pIndex);
		if (strLen > len)	return ret;

		memcpy(fileName,pIndex,strLen);
	}

	return strLen;
}


CWLDirect::CWLDirect()
{

}

CWLDirect::~CWLDirect()
{

}
