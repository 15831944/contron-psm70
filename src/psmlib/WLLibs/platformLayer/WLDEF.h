
/********************************************************************************************************
功能：跨平台C/C++定义文件：定义操作系统、IDE、编择器、基础库、基本数据类型和日期时间类型
说明：（1）int 和 char 未在本文件中定义，int 为 四个字节有符号整型；char 一个字节有符号字符型
      （2）编译前请先打开相应的“操作系统”、“IDE”、“编译器”、“基础库”开关（以下有说明）
作者：叶文林	2013-11-26
********************************************************************************************************/

#if !defined(AFX_WLDEF_H__9E4D44CD_1C8B_4DD4_9821_D4369D2ABA7F__INCLUDED_)
#define AFX_WLDEF_H__9E4D44CD_1C8B_4DD4_9821_D4369D2ABA7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/********************************************************************************************************
功能：操作系统开关
说明：以下有且只能有一行不注释
********************************************************************************************************/

#ifdef _WIN64
#define WL_OS_WIN64 //64位非嵌入式windows平台(x64 Windows XP/Vista/7 and Windows Server 2003/2008)
#elif defined(_WIN32)
#define WL_OS_WIN32 //32位非嵌入式windows平台(x86 Windows 2000/XP/Vista/7 and Windows Server 2003/2008)
#endif

#ifdef __unix
#define WL_OS_LINUX   //普通Linux
#endif

//#define WL_OS_WINCE   //(Windows CE 5.0)
//#define WL_OS_ANDROID //安卓
//*****************************************************************************************************

/********************************************************************************************************
功能：VC6开发工具开关
说明：以下有且只能有一行不注释或全注释（如果不是很明白，建议全注释）
********************************************************************************************************/
//#define WL_IDE_VC6      //一般为微软开发平台，如：VC6 VS200X
//*****************************************************************************************************

/********************************************************************************************************
功能：编译器开关
说明：以下有且只能有一行不注释
********************************************************************************************************/
#define WL_C_VCC        //一般为微软开发平台，如：VC6 VS200X
//#define WL_C_GCC      //一般为非微软件开发平台，如：QT Creator、以及其它用gcc、g++等编译平台
//*****************************************************************************************************

/********************************************************************************************************
功能：基础库开关
说明：以下有且只能有一行不注释或全注释（如果不是很明白，建议全注释）
********************************************************************************************************/
//#define WL_3LIB_QT      //使用了QT库
//*****************************************************************************************************

/********************************************************************************************************
功能：调试器开关
说明：以下根据情况注释，最终默认打印开关请见 WLOutDeDug.h 上的定义
********************************************************************************************************/
#define WL_DEBUG_CONSOLE        //一般为控制台程序
//#define WL_DEBUG_VS             //微软VS调试窗口
//#define WL_DEBUG_QTCREATOR      //QTCreator调试窗口
//#define WL_DEBUG_UDPSERVER      //调试信息输出到某一个UDP服务上
//#define	WL_DEBUG_COM          //调试信息输出到串口上
//#define WL_DEBUG_ECLIPSE        //调试信息输出到eclipse
//*****************************************************************************************************

//OS define

/*
   The operating system, must be one of: (WL_OS_x)

     DARWIN   - Darwin OS (synonym for Q_OS_MAC)
     SYMBIAN  - Symbian
     MSDOS    - MS-DOS and Windows
     OS2      - OS/2
     OS2EMX   - XFree86 on OS/2 (not PM)
     WIN32    - Win32 (Windows 2000/XP/Vista/7 and Windows Server 2003/2008)
     WINCE    - WinCE (Windows CE 5.0)
     CYGWIN   - Cygwin
     SOLARIS  - Sun Solaris
     HPUX     - HP-UX
     ULTRIX   - DEC Ultrix
     LINUX    - Linux
     FREEBSD  - FreeBSD
     NETBSD   - NetBSD
     OPENBSD  - OpenBSD
     BSDI     - BSD/OS
     IRIX     - SGI Irix
     OSF      - HP Tru64 UNIX
     SCO      - SCO OpenServer 5
     UNIXWARE - UnixWare 7, Open UNIX 8
     AIX      - AIX
     HURD     - GNU Hurd
     DGUX     - DG/UX
     RELIANT  - Reliant UNIX
     DYNIX    - DYNIX/ptx
     QNX      - QNX
     LYNX     - LynxOS
     BSD4     - Any BSD 4.4 system
     UNIX     - Any UNIX BSD/SYSV system
*/
//WINDOWS内核
#if defined(WL_OS_WINCE) || defined(WL_OS_WIN32) || defined(WL_OS_WIN64)
#  define WL_OS_CORE_WIN
#endif
//linux内核
#if defined(WL_OS_LINUX) || defined(WL_OS_ANDROID) 
#  define WL_OS_CORE_LINUX
#endif

#define TRUE 1
#define FALSE 0

#ifndef MAX_PATH  
#define MAX_PATH  260
#endif

typedef unsigned int        DWORD32;   //无符号四字节
typedef unsigned long long  DWORD64;   //无符号八字节，long long型，32位和64位中都为八字节
typedef DWORD64             TMemAddr;  //叶文林 2013-12-30 修改。改为定长8字节。//原注释，存放内存地址类型，必须与指针长度保持一致：关于long型，32位中为四字节；64位中为八字节

#ifdef WL_OS_CORE_WIN
typedef unsigned long       DWORD;
#else
typedef unsigned int        DWORD;
#endif
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;

//typedef DWORD               TWLHandle; //句柄型。对于32位机，用DWORD
typedef void *              TWLHandle; //句柄型。对于32位机，用DWORD
//typedef unsigned short      TCHAR;



typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

typedef void *              PVOID;

/*
说明：linux中为socklen_t类型，windows中对应为int型
作者：唐小灿  2014-07-25
*/
#ifdef WL_OS_CORE_WIN
typedef int                 socklen_t;
#elif defined WL_OS_CORE_LINUX
typedef int                 SOCKET;
//typedef unsigned short      TCHAR;
#endif

//自定义时间数据类型
typedef struct _WLTime {
	WORD year;
	BYTE month;
	BYTE day;
	BYTE hour;
	BYTE minute;
	BYTE second;
	BYTE RFU;
}TWLTime;

#endif // !defined(AFX_WLDEF_H__9E4D44CD_1C8B_4DD4_9821_D4369D2ABA7F__INCLUDED_)
