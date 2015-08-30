#ifndef _CHARACTERSETCONVERSION_
#define _CHARACTERSETCONVERSION_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "WLDEF.h"

//#define DWORD unsigned short
	
//GB2312�ַ���תUTF8�ַ���
int gb2312StrToUTF8(char *pGB2312Str,char * pUTF8Str,int count,DWORD & uftLen);

//UTF8�ַ���תGB2312
int utf8StrToGB2312(char * pUTF8Str,char *pGB2312Str,char count,DWORD & gb2312Len);

//GB2312�ַ���תתunicode
int gb2312StrToUnicode(char *pGB2312Str, wchar_t * pUnicode, int count, DWORD & unicodeLen);

#endif