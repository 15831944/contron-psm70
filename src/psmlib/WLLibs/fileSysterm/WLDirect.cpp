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
功能：获取当前工具路径
参数：dir，输出参数（内存空间由调用者申请）
      len，dir空间大小
返回：1，成功；
      <=0，失败：
	  0，空间分配太小；
	  -1，传入参数不合法
	  -2，此命令在特定平台上不支持，
      -3，执行失败
作者：叶文林	2013-11-27 张林颢修改 GetCurrentDirectoryA(len,dir)返回值大于0则成功
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
功能：获取当前程序全路径文件名
参数：fullName，输出参数（内存空间由调用者申请）
      len，fullName空间大小
返回：1，成功；
      <=0，失败：
	  0，空间分配太小；
	  -1，传入参数不合法
	  -2，此命令在特定平台上不支持，
      -3，执行失败
作者：叶文林	2013-11-27
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
功能：判断目录是否存在
参数：dir，目录全名
返回：1，存在；
      -1，不存在
作者：叶文林	2013-11-27
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
功能：创建目录
参数：dir，目录全名
返回：1，成功；
      <=0，失败：
作者：叶文林	2013-11-27
修改：刘远安 增加QT创建目录
*********************************************************************/
#ifdef WL_3LIB_QT      //使用了QT库
#include <QDir>
#endif

int WLMakeDir(const char * dir)
{
	if (dir == NULL)	return -1;
	int rev = 0;
#ifdef WL_OS_CORE_WIN
  #ifdef WL_3LIB_QT      //使用了QT库
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

	//错误
	if (EEXIST == errno)         //已经存在
		return -1;
	else if (ENOENT == errno)    //路径错误
		return -2;
	return -3;                   //其他
  #endif
#elif defined WL_OS_CORE_LINUX
	rev = mkdir(dir,0777);
	if (0 == rev)       return 1;
	WLOutDebug("madir failed, errno is %d", errno);

	//错误
	if (EEXIST == errno)         //已经存在
		return -1;
	else if (ENOENT == errno)    //路径错误
		return -2;
	return -3;                   //其他
#endif
}

/*********************************************************************
功能：删除目录
参数：dir，目录全名
返回：1，成功；
      -1，失败
作者：叶文林	2013-11-27
*********************************************************************/
int WLDelDir(const char * dir)
{
	if (dir == NULL)	return -1;
	int rev = 0;
#ifdef WL_OS_CORE_WIN
	//可删除文件夹及其下内容
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
	////错误
	//if (ENOENT == errno)    //目录不存在
	//	return -1;
	//return -2;              //其他
#endif
}


/*********************************************************************
功能：获取指定目录首个文件信息
参数：dir，目录全名
      pFindFileData，接收文件信息空间
返回：句柄
      NULL，失败：
作者：唐小灿	2013-11-28
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
//	_tcscat_s(wdestbuf, _T("\\*.*"));     //这里一定要指明通配符，不然不会读取所有文件和目录
	wcscat(wdestbuf, wsrcbuf);     //这里一定要指明通配符，不然不会读取所有文件和目录
	HANDLE h = ::FindFirstFile(wdestbuf, &data);
	if (NULL == h)  return NULL;

	//设置信息
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

		//设置信息
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
功能：获取下一个文件信息
参数：hdl，WLFindFirstFile所产生的句柄
      pFindFileData，接收文件信息空间
返回：TRUE，成功
	  FALSE，失败：
作者：唐小灿	2013-11-28
*********************************************************************/
BOOL WLGetNextFile(TWLHandle hdl,TFindData * pFindFileData)
{
#ifdef WL_OS_CORE_WIN
	if ((NULL == hdl) || (NULL == pFindFileData))  return FALSE;
	HANDLE h = (HANDLE)hdl;
	WIN32_FIND_DATA data = {0};

	BOOL rev = ::FindNextFile(h, &data);
	if (!rev)  return rev;

	//设置信息
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

		//设置信息
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
功能：关闭WLFindFirstFile产生的句柄
参数：hdl，WLFindFirstFile所产生的句柄
返回：TRUE，成功
      FALSE，失败：
作者：唐小灿	2013-11-28
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
功能：判断是否为文件夹
参数：hdl，WLFindFirstFile所产生的句柄
返回：TRUE，成功
      FALSE，失败：
作者：唐小灿	2013-11-28
修改：唐小灿    2014-09-19  增加windows处理
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
		//处理sdcard,在android中被识别为非文件夹【不准确，文件中包含指定字符则错误】
		char * p = strstr(pFindData->cFileName, "sdcard");
		if (p)  return TRUE;
	}

	return FALSE;
#endif
}


/*********************************************************************
功能：判断是否为文件夹
参数：hdl，WLFindFirstFile所产生的句柄
返回：TRUE，成功
      FALSE，失败：
作者：唐小灿	2014-09-19  增加windows下判断
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
功能：从文件的绝对路径中提取文件夹路径
参数：dir，文件的绝对路径
	  folderDir，传出参数，存放文件夹绝对路径
	  len,folderDir分配的内存空间
返回：<0，失败。-1，传入参数非法
     >0，成功,路径长度
作者：刘远安  2013-04-01
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
	if (pIndex <= pdir)//无'\\'或'/'
	{		
		memcpy(folderDir,pdir,strlen(pdir));
		return strlen(pdir);
	}

	memcpy(folderDir,pdir,strLen);
	return strLen;
}

/*********************************************************************
功能：从文件夹路径中提取文件夹名称
参数：dir，文件夹的绝对路径
	  folderName，传出参数，存放文件夹名称
	  len,folderName分配的内存空间
返回：<0，失败。-1，传入参数非法
     >0，成功,路径长度
举例：输入：“D:/001/002/003”
	  输出：“003”
作者：刘远安  2013-04-01
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
功能：从文件的绝对路径中提取文件名称
参数：dir，文件的绝对路径
	  fileName，传出参数，存放文件名称
	  len,fileName分配的内存空间
返回：<0，失败。-1，传入参数非法
     >0，成功,路径长度
作者：刘远安  2013-04-01
*********************************************************************/
int WLGetFileNameFromDir( const char *pdir,char *fileName,int len )
{
	int ret = -1;
	if (NULL == pdir || NULL == fileName || len <= 0)	return ret;
	memset(fileName,0,len);

	//字符串长度
	int strLen = strlen(pdir);
	//int point = strLen - 1;
	//指针指向字符串末尾
	const char *pIndex = pdir + strLen - 1;

	//从末尾开始查找出现的第一个‘/’或‘\’
	while((*pIndex != '\\') && (*pIndex != '/') && (pIndex >= pdir))	
	{		
		pIndex--;
	}

	ret--;
	if (pIndex <= pdir)//无'\\'或'/'
	{
		if (strLen > len)	return ret;

		memcpy(fileName,pdir,strLen);
	}
	else			  //有'\\'或'/'
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
