// WLPlatformLayer.h: interface for the CWLPlatformLayer class.
// 本文件是对win32、QT、linux等平台常用函数的封装，以前平台定义
// 原因：由于在不同的平台下 gettickcount sleep等函数名称可能不一样，并
//       且参数意思义也可能不一样
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WLPLATFORMLAYER_H__31AA7D9A_B5A4_4FE7_AA1A_AF2BFB473646__INCLUDED_)
#define AFX_WLPLATFORMLAYER_H__31AA7D9A_B5A4_4FE7_AA1A_AF2BFB473646__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WLDEF.h"
#include <string.h>
#include "WLOutDebug.h"
#include "stdio.h"

int WLGetLocalTime(TWLTime * pTime);
int WLSetLocalTime(const TWLTime * pTime);
int getLocalWeek();
int getLocalWeekStr(char * weekStr,DWORD len);

DWORD WLGetTickCount();
DWORD64 WLGetTickCount64();
void WLSleep(DWORD mSec);
void WLNoFreezeSleep(DWORD mSec);

void getCompileDateTime( TWLTime * pTime );

int str2int(const char* pstr);
double str2double(const char* pstr);
int timeStr2TWLTime(const char * timeStr, TWLTime * pDestTime);//yyyyMMddhhmmss

TMemAddr WLGetNewOffsetMemAddr(TMemAddr org,DWORD32 offset);
TMemAddr WLPointerToTMemAddr(void * p);
void * WLTMemAddrToPointer(TMemAddr addr);
void * WLGetNewOffsetMemAddr(void * pAddrBase, DWORD32 offset);

TMemAddr  WLPointer2MemAddr(void * p);
void    * WLMemAddr2Pointer(TMemAddr addr);
TMemAddr  WLMemAddrInc(TMemAddr addr,DWORD32 offset);
void    * WLPointerInc(void * p ,DWORD32 offset);

//产生随机数
DWORD32 getRand();

BOOL WLExecuteApp(const char *appPath,char *pParameters,const char *pWorkDir,TWLHandle *outHandle);


//Unicode与ASCII互转
//void gAnsiToUnicode(const char* ansistr,TCHAR* unicodestr);
//void gWideCharToAnsi(const TCHAR* unicodestr,char* ansistr);

//BOOL ExecuteApp(char *pszPath,char *pParameters);

//DWORD WLGetCurrentProcess();
//BOOL WLTerminateProcess(DWORD hProcess);
//BOOL WLTerminateProcessWaitEnd(DWORD hProcess);
//DWORD WLGetCurrentProcessId();
//BOOL WLKillProcessWaitEnd(DWORD processId);

//void WLTouchCalibrate();
//void WLSuspendSystem();
//void WLSetLocalTime(TWLTime * pTime);


#endif // !defined(AFX_WLPLATFORMLAYER_H__31AA7D9A_B5A4_4FE7_AA1A_AF2BFB473646__INCLUDED_)
