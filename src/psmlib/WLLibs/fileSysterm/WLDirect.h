// WLDirect.h: interface for the CWLDirect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WLDIRECT_H__FEA4BBC5_2CF7_4046_92C0_EC531F8E7FAD__INCLUDED_)
#define AFX_WLDIRECT_H__FEA4BBC5_2CF7_4046_92C0_EC531F8E7FAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../platformLayer/WLDEF.h"

typedef struct _FindData{
	DWORD    dwFileAttributes; //windows下：32，文件属性；16，文件夹属性
	TWLTime ftCreationTime; 
	TWLTime ftLastAccessTime; 
	TWLTime ftLastWriteTime; 
	DWORD    nFileSizeHigh; 
	DWORD    nFileSizeLow; 
//	TCHAR    cFileName[ MAX_PATH ]; 
	char    cFileName[ MAX_PATH ]; 
}TFindData;

int WLGetCurrentDirectory(char * dir,DWORD len);
#ifdef WL_OS_CORE_WIN
	int WLGetModuleFileName(char * fullName,DWORD len); //本函数暂未找到完美跨平台方法，请不要使用
	int WLGetModuleDir(char * dir,DWORD len);
#endif
int WLIsExsitDir(const char * dir);
int WLMakeDir(const char * dir);
int WLDelDir(const char * dir);

TWLHandle WLFindFirstFile(const char * dir,TFindData * pFindFileData);
BOOL WLGetNextFile(TWLHandle hdl,TFindData * pFindFileData);
BOOL WLCloseFindFile(TWLHandle hdl);
BOOL WLIsFolder(TFindData * pFindData);
BOOL WLIsStringFolder(const char * pPath);

void WLchar2wchar(const char *chr, wchar_t *wchar, int size);
void WLwchar2char(const wchar_t *wchar, char *chr, int length);

int WLGetFolderDir(const char *pdir,char *folderDir,int len);										//从文件的绝对路径中提取文件夹路径
int WLGetSubFolderName( const char *pdir,char *folderName,int len );								//从文件夹路径中提取文件夹名称
int WLGetFileNameFromDir(const char *pdir,char *fileName,int len);									//从文件的绝对路径中提取文件名称


class CWLDirect  
{
public:
	CWLDirect();
	virtual ~CWLDirect();

};

#endif // !defined(AFX_WLDIRECT_H__FEA4BBC5_2CF7_4046_92C0_EC531F8E7FAD__INCLUDED_)
