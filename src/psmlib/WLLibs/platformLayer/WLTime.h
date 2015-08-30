// WLTime.h: interface for the CWLTime class.
//
//////////////////////////////////////////////////////////////////////

/********************************************************************************************************
���ܣ���װ���� TWLTime �����⺯��
˵����
���ߣ�Ҷ����	2013-11-26
********************************************************************************************************/

#if !defined(AFX_WLTIME_H__C60326E5_5F8D_4D26_B566_2FC6B65457D2__INCLUDED_)
#define AFX_WLTIME_H__C60326E5_5F8D_4D26_B566_2FC6B65457D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WLDEF.h"
#include <string.h>
#include <stdio.h>

#define WLT_BASE_YEAR 1972 //��׼ʱ��ѡ����Ҫ�����꣺ѡ����׼ʱ�䣺 1972��1��1��0��0��0��0����

#define WLT_SECOND_SPER_MINUTE      60
#define WLT_SECOND_SPER_HOUR       (60*WLT_SECOND_SPER_MINUTE)
#define WLT_HOUR_SPER_DAY           24
#define WLT_DAY_SPER_WEEK           7
#define WLT_SECOND_SPER_DAY        (WLT_HOUR_SPER_DAY*WLT_SECOND_SPER_HOUR)
#define WLT_MONTH_SPER_YEAR         12
#define WLT_DAY_SPER_YEAR           365

typedef struct _Days_CurrentdaySeconds{
	int days;                                            //����
	int currentDaySeconds;                               //������������
}TDays_CurrentdaySeconds;

//�ַ�����ʽΪ��yyyy-mm-dd hh:mm:ss ���ַ�
char * WLTimeToStr(TWLTime * time,char str[20],int len); //ʱ������ת�ַ���
BOOL strToWLTime(const char * timestr,TWLTime * time);   //�ַ���תʱ��

void WLDTIncYear(TWLTime * pInTime,int year,TWLTime * pOutTime);
void WLDTIncMonth(TWLTime * pInTime,int month,TWLTime * pOutTime);
void WLDTIncWeek(TWLTime * pInTime,int week,TWLTime * pOutTime);
void WLDTIncDay(TWLTime * pInTime,int day,TWLTime * pOutTime);
void WLDTIncHour(TWLTime * pInTime,int hour,TWLTime * pOutTime);
void WLDTIncMinute(TWLTime * pInTime,int minute,TWLTime * pOutTime);
void WLDTIncSecond(TWLTime * pInTime,int second,TWLTime * pOutTime);
void WLDTIncMilliSecond(TWLTime * pInTime,int milliSecond,TWLTime * pOutTime);

void WLDTUTC2Beijin(TWLTime * pUTCTime,TWLTime * pBeijinTime);

class CWLTime  
{
public:
	CWLTime();
	virtual ~CWLTime();

};

#endif // !defined(AFX_WLTIME_H__C60326E5_5F8D_4D26_B566_2FC6B65457D2__INCLUDED_)
