/**************************************************************************************************
功能：实时库客户端自定义数据类型
说明：int 和 char 未在本文件中定义，int 为 四个字节有符号整型；char 一个字节有符号字符型
作者：唐小灿  2014-06-17
注意：该文件为实时库客户端所使用的基本数据类型的扩展，使用实时库客户端的用户可以自行定义本文件中
      定义的数据类型，但请遵循本文件定义。
**************************************************************************************************/


typedef unsigned int        DWORD32;   //无符号四字节
typedef unsigned long long  DWORD64;   //无符号八字节，long long型，32位和64位中都为八字节

#ifdef _WIN32
typedef unsigned long       DWORD;
#else
typedef unsigned int        DWORD;
#endif
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

typedef void *              PVOID;