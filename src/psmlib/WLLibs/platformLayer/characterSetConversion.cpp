#include "stdafx.h"
#include "charactersetconversion.h"
#include "gb2312ToUnicodeTable.h"
#include "gb2312UnicodeCrossTable.h"
#include <stdio.h>
#include <string.h>

#ifdef WL_IDE_VC6
	#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[]=__FILE__;
	#define new DEBUG_NEW
	#endif
#endif

/*******************************************************************************
���ܣ�GB2312�ַ���תUTF8�ַ���
������
���أ�<0��ʧ��(���� -1 Ϊuft8�ַ�������ռ����)��>0���ɹ�
���ߣ�Ҷ����	2013-08-29
*******************************************************************************/
//int gb2312StrToUTF8(char *pGB2312Str,char * pUTF8Str,char count,DWORD & uftLen)
int gb2312StrToUTF8(char *pGB2312Str,char * pUTF8Str,int count,DWORD & uftLen)
{
	int isrcLen = strlen(pGB2312Str);
	memset(pUTF8Str,0,count);

	BYTE * pSrcGB2312 = (BYTE *)pGB2312Str;
	BYTE * pDesUTF8 = (BYTE *)pUTF8Str;
	BYTE highByte,lowByte;
	WORD unicodeWord;
	int x,y;
	BYTE uft8Word[3] = {0};

	uftLen = 0;
	while ('\0' != * pSrcGB2312)
	{
		printf("*pSrcGB2312 = %.2X",*pSrcGB2312);
		if(*pSrcGB2312 >= 0xA0) //Ϊ���������gb2312��תunicode��unioce��תutf8
		{
			//GB2312 to unicode
			highByte = *(pSrcGB2312    )-0xA0 -1;//�����
			if (highByte > 0x09)	highByte = highByte - 6;
			lowByte  = *(pSrcGB2312 +1 )-0xA0 -1;//
			x = (int)highByte;
			y = (int)lowByte;
			unicodeWord = CodeTableGB2312ToUnicode[x][y];//ȡunicode����

			//unicode to GB2312
			highByte= (0xff00 & unicodeWord)>>8;
			lowByte = (0x00ff & unicodeWord);

			//ȡuft8����
			uft8Word[0] = 0xE0 | ( (0xF0 & highByte)>>4);
			uft8Word[1] = 0x80 | (((0x0F & highByte)<<2) | ((0xC0 & lowByte)>>6));
			uft8Word[2] = 0x80 | (  0x3F & lowByte );

			memcpy(pDesUTF8,uft8Word,sizeof(uft8Word));

			//��ָ��
			pSrcGB2312 += 2;
			pDesUTF8   += 3;
			uftLen     += 3;
		}
		else             //�Ǻ��������ֱ��copy
		{
			memcpy(pDesUTF8,pSrcGB2312,1*sizeof(char)); //�Ǻ���ֱ�Ӹ�ֵ

			//��ָ��
			pSrcGB2312 += 1;
			pDesUTF8   += 1;	
			uftLen     +=1;
		}

	}

	return 1;

}


/*******************************************************************************
���ܣ�GB2312�ַ���תunicode�ַ���
������
���أ�<0��ʧ��(���� -1 Ϊuft8�ַ�������ռ����)��>0���ɹ�
���ߣ���С��	2014-08-08
*******************************************************************************/
int gb2312StrToUnicode(char *pGB2312Str, wchar_t * pUnicode, int count, DWORD & unicodeLen)
{
	int isrcLen = strlen(pGB2312Str);
	memset(pUnicode,0,count);

	BYTE * pSrcGB2312  = (BYTE *)pGB2312Str;
	BYTE * pDesUnicode = (BYTE *)pUnicode;
	BYTE highByte,lowByte;
	WORD unicodeWord;
	int x,y;

	unicodeLen = 0;
	while ('\0' != * pSrcGB2312)
	{
		if(*pSrcGB2312 >= 0xA0) //Ϊ���������gb2312תunicode������������費��Ҫת
		{
			//GB2312 to unicode
			highByte = *(pSrcGB2312    )-0xA0 -1;//�����
			if (highByte > 0x09)	highByte = highByte - 6;
			lowByte  = *(pSrcGB2312 +1 )-0xA0 -1;//
			x = (int)highByte;
			y = (int)lowByte;
			unicodeWord = CodeTableGB2312ToUnicode[x][y];//ȡunicode����

			//ȡunicode����
			memcpy(pDesUnicode, (BYTE *)&unicodeWord, sizeof(unicodeWord));

			//��ָ��
			pSrcGB2312  += 2;
			pDesUnicode += 2;
			unicodeLen  += 2;
		}
		else                    //�Ǻ�������������ֽ�ת˫�ֽ�
		{
			unicodeWord = *pSrcGB2312;

			//ȡunicode����
			memcpy(pDesUnicode, (BYTE *)&unicodeWord, sizeof(unicodeWord));

			//��ָ��
			pSrcGB2312  += 1;
			pDesUnicode += 2;
			unicodeLen  += 2;
		}
	}

	return 1;
}

int getUtf8ByteNumForWord(BYTE firstCh);
WORD searchCodeTable(WORD unicodeKey);
/*******************************************************************************
���ܣ�UTF8�ַ���תGB2312
������
���أ�<0��ʧ��(���� -1 Ϊgb2312�ַ�������ռ����)��>0���ɹ�
���ߣ�Ҷ����	2013-08-29
*******************************************************************************/
int utf8StrToGB2312(char * pUTF8Str,char *pGB2312Str,char count,DWORD & gb2312Len)
{
	//APP_PRINT("utf8->unicode: \n");
	//APP_PRINT("utf8: [");
	//for (int k = 0; k < len; k++)
	//{
	//	APP_PRINT("%02x ", utf8[k]);
	//}
	//APP_PRINT("]\n");

	int byteCount = 0;
	int i = 0;
	int j = 0;

	WORD unicodeKey = 0;
	WORD gbKey = 0;

	char * utf8 = pUTF8Str;
	int len = strlen(pUTF8Str);
	char * temp = pGB2312Str;
///////////////////////////////////////
	BYTE * pByte = (BYTE *)&unicodeKey;
	BYTE c;

	//ѭ������
	while (i < len)
	{   
		switch(getUtf8ByteNumForWord((BYTE)utf8[i]))
		{
		case 0:
			temp[j] = utf8[i];
			byteCount = 1;
			break;

		case 2:
			temp[j] = pUTF8Str[i];
			temp[j + 1] = utf8[i + 1];
			byteCount = 2;
			break;

		case 3:
			//����Ϳ�ʼ����UTF8->Unicode
			temp[j + 1] = ((utf8[i] & 0x0F) << 4) | ((utf8[i + 1] >> 2) & 0x0F);
			temp[j] = ((utf8[i + 1] & 0x03) << 6) + (utf8[i + 2] & 0x3F);
			//temp[j ] = ((utf8[i] & 0x0F) << 4) | ((utf8[i + 1] >> 2) & 0x0F);
			//temp[j+1] = ((utf8[i + 1] & 0x03) << 6) + (utf8[i + 2] & 0x3F);

			//ȡ��Unicode��ֵ
			memcpy(&unicodeKey, (temp + j), 2);
			//�ߵ�λ����
			//BYTE * pByte = (BYTE *)&unicodeKey;
			//c = pByte[0];
			//pByte[0] = pByte[1];
			//pByte[1] = c;

			//c = pB[0];
			//pB[0] = pB[1];
			//pB[1] = c;
			//APP_PRINT("unicode key is: 0x%04X\n", unicodeKey);

			//�������ֵ���ȡ�ö�Ӧ��GB2312��ֵ
			gbKey = searchCodeTable(unicodeKey);
			//APP_PRINT("gb2312 key is: 0x%04X\n", gbKey);

			if (gbKey != 0)
			{
				//here change the byte
				//��Ϊ0��ʾ�����������ߵ������ֽڵ���������Ҫ����ʽ
				//gbKey = (gbKey >> 8) | (gbKey << 8);
				//APP_PRINT("after changing, gb2312 key is: 0x%04X\n", gbKey);
				memcpy((temp + j), &gbKey, 2);
			}

			byteCount = 3;
			break;

		case 4:
			byteCount = 4;
			break;
		case 5:
			byteCount = 5;
			break;
		case 6:
			byteCount = 6;
			break;

		default:
			//APP_PRINT("the len is more than 6\n");
			break;    
		}

		i += byteCount;
		if (byteCount == 1)
		{
			j++;
		}
		else
		{
			j += 2;
		}

	}
	//APP_PRINT("utf8: [");
	//for (k = 0; k < j; k++)
	//{
	//	APP_PRINT("%02x ", temp[k]);
	//}
	//APP_PRINT("]\n");

	return 1;
}

//��������������Եõ��ַ������Ǽ����ֽ���һ��ġ���ΪUTF8���ֻ��6���ֽڣ����Ծ͸��ݷ���ֵ������������ֻ������3���ֽں�1���ֽڵ�UTF8�ı��룬��Ϊһ����˵������UTF8����3���ֽڡ�
int getUtf8ByteNumForWord(BYTE firstCh)
{
	BYTE temp = 0x80;
	int num = 0;

	while (temp & firstCh)
	{
		num++;
		temp = (temp >> 1);
	}

	//APP_PRINT("the num is: %d", num);
	return num;
}

WORD searchCodeTable(WORD unicodeKey)
{
	//BYTE b[2] = {0};
	//memcpy(b,&unicodeKey,2);
	//BYTE c = 0;
	//b[0] = c;
	//b[0] = b[1];
	//b[1] = c;
	//WORD unicodeWord =0;
	//memcpy(&unicodeWord,b,2);

	TRecUnicodeGB2312 * p = NULL;
	for(int i =0;i<81*94;i++)
	{
		p = &CodeTableGB2312UnicodeCross[i];
		if (p->unicodeCode == unicodeKey)	
		{
			return p->gb2312Code;
		}
		//if (p->unicodeCode == unicodeWord)	return p->gb2312Code;
	}
	return 0;
}
