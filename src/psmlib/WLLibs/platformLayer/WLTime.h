// WLTime.h: interface for the CWLTime class.
//
//////////////////////////////////////////////////////////////////////

/********************************************************************************************************
功能：封装关于 TWLTime 共公库函数
说明：
作者：叶文林	2013-11-26
********************************************************************************************************/

#if !defined(AFX_WLTIME_H__C60326E5_5F8D_4D26_B566_2FC6B65457D2__INCLUDED_)
#define AFX_WLTIME_H__C60326E5_5F8D_4D26_B566_2FC6B65457D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WLDEF.h"
#include <string.h>
#include <stdio.h>

#define WLT_BASE_YEAR 1972 //基准时间选定需要是闰年：选定基准时间： 1972年1月1日0点0分0秒0毫秒

#define WLT_SECOND_SPER_MINUTE      60
#define WLT_SECOND_SPER_HOUR       (60*WLT_SECOND_SPER_MINUTE)
#define WLT_HOUR_SPER_DAY           24
#define WLT_DAY_SPER_WEEK           7
#define WLT_SECOND_SPER_DAY        (WLT_HOUR_SPER_DAY*WLT_SECOND_SPER_HOUR)
#define WLT_MONTH_SPER_YEAR         12
#define WLT_DAY_SPER_YEAR           365

typedef struct _Days_CurrentdaySeconds{
	int days;                                            //天数
	int currentDaySeconds;                               //当天运行秒数
}TDays_CurrentdaySeconds;

//字符串格式为：yyyy-mm-dd hh:mm:ss 个字符
char * WLTimeToStr(TWLTime * time,char str[20],int len); //时间类型转字符串
BOOL strToWLTime(const char * timestr,TWLTime * time);   //字符串转时间

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
