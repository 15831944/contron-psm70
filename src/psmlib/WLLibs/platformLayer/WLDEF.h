
/********************************************************************************************************
���ܣ���ƽ̨C/C++�����ļ����������ϵͳ��IDE���������������⡢�����������ͺ�����ʱ������
˵������1��int �� char δ�ڱ��ļ��ж��壬int Ϊ �ĸ��ֽ��з������ͣ�char һ���ֽ��з����ַ���
      ��2������ǰ���ȴ���Ӧ�ġ�����ϵͳ������IDE�����������������������⡱���أ�������˵����
���ߣ�Ҷ����	2013-11-26
********************************************************************************************************/

#if !defined(AFX_WLDEF_H__9E4D44CD_1C8B_4DD4_9821_D4369D2ABA7F__INCLUDED_)
#define AFX_WLDEF_H__9E4D44CD_1C8B_4DD4_9821_D4369D2ABA7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/********************************************************************************************************
���ܣ�����ϵͳ����
˵������������ֻ����һ�в�ע��
********************************************************************************************************/

#ifdef _WIN64
#define WL_OS_WIN64 //64λ��Ƕ��ʽwindowsƽ̨(x64 Windows XP/Vista/7 and Windows Server 2003/2008)
#elif defined(_WIN32)
#define WL_OS_WIN32 //32λ��Ƕ��ʽwindowsƽ̨(x86 Windows 2000/XP/Vista/7 and Windows Server 2003/2008)
#endif

#ifdef __unix
#define WL_OS_LINUX   //��ͨLinux
#endif

//#define WL_OS_WINCE   //(Windows CE 5.0)
//#define WL_OS_ANDROID //��׿
//*****************************************************************************************************

/********************************************************************************************************
���ܣ�VC6�������߿���
˵������������ֻ����һ�в�ע�ͻ�ȫע�ͣ�������Ǻ����ף�����ȫע�ͣ�
********************************************************************************************************/
//#define WL_IDE_VC6      //һ��Ϊ΢����ƽ̨���磺VC6 VS200X
//*****************************************************************************************************

/********************************************************************************************************
���ܣ�����������
˵������������ֻ����һ�в�ע��
********************************************************************************************************/
#define WL_C_VCC        //һ��Ϊ΢����ƽ̨���磺VC6 VS200X
//#define WL_C_GCC      //һ��Ϊ��΢�������ƽ̨���磺QT Creator���Լ�������gcc��g++�ȱ���ƽ̨
//*****************************************************************************************************

/********************************************************************************************************
���ܣ������⿪��
˵������������ֻ����һ�в�ע�ͻ�ȫע�ͣ�������Ǻ����ף�����ȫע�ͣ�
********************************************************************************************************/
//#define WL_3LIB_QT      //ʹ����QT��
//*****************************************************************************************************

/********************************************************************************************************
���ܣ�����������
˵�������¸������ע�ͣ�����Ĭ�ϴ�ӡ������� WLOutDeDug.h �ϵĶ���
********************************************************************************************************/
#define WL_DEBUG_CONSOLE        //һ��Ϊ����̨����
//#define WL_DEBUG_VS             //΢��VS���Դ���
//#define WL_DEBUG_QTCREATOR      //QTCreator���Դ���
//#define WL_DEBUG_UDPSERVER      //������Ϣ�����ĳһ��UDP������
//#define	WL_DEBUG_COM          //������Ϣ�����������
//#define WL_DEBUG_ECLIPSE        //������Ϣ�����eclipse
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
//WINDOWS�ں�
#if defined(WL_OS_WINCE) || defined(WL_OS_WIN32) || defined(WL_OS_WIN64)
#  define WL_OS_CORE_WIN
#endif
//linux�ں�
#if defined(WL_OS_LINUX) || defined(WL_OS_ANDROID) 
#  define WL_OS_CORE_LINUX
#endif

#define TRUE 1
#define FALSE 0

#ifndef MAX_PATH  
#define MAX_PATH  260
#endif

typedef unsigned int        DWORD32;   //�޷������ֽ�
typedef unsigned long long  DWORD64;   //�޷��Ű��ֽڣ�long long�ͣ�32λ��64λ�ж�Ϊ���ֽ�
typedef DWORD64             TMemAddr;  //Ҷ���� 2013-12-30 �޸ġ���Ϊ����8�ֽڡ�//ԭע�ͣ�����ڴ��ַ���ͣ�������ָ�볤�ȱ���һ�£�����long�ͣ�32λ��Ϊ���ֽڣ�64λ��Ϊ���ֽ�

#ifdef WL_OS_CORE_WIN
typedef unsigned long       DWORD;
#else
typedef unsigned int        DWORD;
#endif
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;

//typedef DWORD               TWLHandle; //����͡�����32λ������DWORD
typedef void *              TWLHandle; //����͡�����32λ������DWORD
//typedef unsigned short      TCHAR;



typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

typedef void *              PVOID;

/*
˵����linux��Ϊsocklen_t���ͣ�windows�ж�ӦΪint��
���ߣ���С��  2014-07-25
*/
#ifdef WL_OS_CORE_WIN
typedef int                 socklen_t;
#elif defined WL_OS_CORE_LINUX
typedef int                 SOCKET;
//typedef unsigned short      TCHAR;
#endif

//�Զ���ʱ����������
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
