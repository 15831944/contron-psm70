// WLTime.cpp: implementation of the CWLTime class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WLTime.h"

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

char * WLTimeToStr(TWLTime * time,char str[20],int len)
{
	memset(str,0,len);

	sprintf(str,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",time->year,time->month,time->day,time->hour,time->minute,time->second);
	return str;
}

/*********************************************************************
功能：将天数转换为日期
参数：days:如果是闰年，则days为[1,366]，否则为[1,365]；ptwlDate：日期结构体指针
返回：返回0，表示转换成功；返回-1，表示转换失败
作者：钱志华	2013-09-03
*********************************************************************/
int Days2Date(unsigned long days, TWLTime *ptwlTime)
{
	int bleap;
	int leapMonth[WLT_MONTH_SPER_YEAR] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int noLeapMonth[WLT_MONTH_SPER_YEAR] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int *pMonth;
	int month;
	int temp = days;
	if (!ptwlTime) return -1;
	//判断当年是否是闰年
	if((ptwlTime->year%4==0 && ptwlTime->year%100!=0)|| (ptwlTime->year%400==0)) 
	{
		bleap = TRUE;
		pMonth = leapMonth;
	}
	else
	{
		bleap = FALSE;
		pMonth = noLeapMonth;
	}
	if ((bleap == TRUE && days < 1 && days > WLT_DAY_SPER_YEAR) || (bleap == FALSE && days < 1 && days > WLT_DAY_SPER_YEAR)) return -1; 

	for(month = 1; month <= WLT_MONTH_SPER_YEAR; month ++)
	{
		ptwlTime->day = temp; 
		temp -= pMonth[month-1];
		if (temp <= 0 || month == WLT_MONTH_SPER_YEAR) break;
	}

	ptwlTime->month = month;
	return 0;
}

/*********************************************************************
功能：将日期转换为天数
参数：ptwlTime：时间结构体指针
返回：返回天数
作者：钱志华	2013-09-03
*********************************************************************/
int Date2Days(TWLTime *ptwlTime)
{
	int bleap;
	int leapMonth[WLT_MONTH_SPER_YEAR] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int noLeapMonth[WLT_MONTH_SPER_YEAR] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int *pMonth;
	int index;
	int days = 0;
	if (!ptwlTime) return -1;
	//判断当年是否是闰年
	if((ptwlTime->year%4==0 && ptwlTime->year%100!=0)|| (ptwlTime->year%400==0)) 
	{
		bleap = TRUE;
		pMonth = leapMonth;
	}
	else
	{
		bleap = FALSE;
		pMonth = noLeapMonth;
	}

	for(index = 0; index < WLT_MONTH_SPER_YEAR; index ++)
	{
		if (ptwlTime->month <= (index+1)) break;
		days += pMonth[index];		
	}

	days += ptwlTime->day;
	return days;
}

/*********************************************************************
功能：将时间（精确到秒）转换为TDays_CurrentdaySeconds（天数值，秒数值）
参数：ptwlTime：时间结构体指针
返回：返回结构体TDays_CurrentdaySeconds
作者：钱志华	2013-09-03
*********************************************************************/
TDays_CurrentdaySeconds TWLTime2Value(TWLTime *ptwlTime)
{
	TDays_CurrentdaySeconds twlValue;
	int year2days, month2days, day2days, datadays;
	int leapdays, bleap;
	int bafterStandardTime;  //在基准时间之后
	if (!ptwlTime || (ptwlTime->month <1 || ptwlTime->month > WLT_MONTH_SPER_YEAR) || (ptwlTime->day < 1 || ptwlTime->day > 31) 
		|| (ptwlTime->hour < 0 || ptwlTime->hour > 23) || (ptwlTime->minute < 0 || ptwlTime->minute > 59) || (ptwlTime->second < 0 || ptwlTime->second > 59)) 
	{ 
		printf("参数值错误\n");
		twlValue.days = 0;
		twlValue.currentDaySeconds = 0;
		return twlValue;
	}

	//判断当年是否是闰年
	if((ptwlTime->year%4==0 && ptwlTime->year%100!=0)|| (ptwlTime->year%400==0))  bleap = TRUE;
	else bleap =FALSE;
    //判断2月份日期的合法性
	if ((bleap == FALSE) && (ptwlTime->month == 2 && ptwlTime->day == 29))
	{ 
		printf("参数值错误\n");
		twlValue.days = 0;
		twlValue.currentDaySeconds = 0;
		return twlValue;
	}

	//相对于基准时间，闰年的天数
	leapdays = (ptwlTime->year - WLT_BASE_YEAR)/4 - (ptwlTime->year - WLT_BASE_YEAR)/100 + (ptwlTime->year - WLT_BASE_YEAR)/400;
    
	//年折换成天数，闰年：366天，平年：365天
    year2days = (ptwlTime->year - WLT_BASE_YEAR)*WLT_DAY_SPER_YEAR;

	if (bleap == FALSE)  year2days ++;

	//日期转换成天数
	datadays = Date2Days(ptwlTime);
	
	//实际天数
	twlValue.days = year2days + datadays//month2days + day2days + leapdays;
    //当天运行秒数
    twlValue.currentDaySeconds = ((ptwlTime->hour)*60 + ptwlTime->minute)*60 + ptwlTime->second;
    return twlValue;
}


/*********************************************************************
功能：将秒数值转换为时间 精确到秒
参数：twlValue：天数值，秒数值
返回：返回时间
作者：钱志华	2013-09-03
*********************************************************************/
TWLTime Value2TWLTime(TDays_CurrentdaySeconds twlValue)
{
    TWLTime twlTime;
	unsigned long midvalue = twlValue.currentDaySeconds;
	int days;
	int leapdays;
	int bleap;

	//计算时分秒
    twlTime.second = midvalue%60;
    midvalue -= twlTime.second;
	midvalue /= 60;
    twlTime.minute = midvalue%60;
	midvalue -= twlTime.minute;
	midvalue /= 60;
    twlTime.hour = midvalue%24;
	midvalue -= twlTime.hour;
	midvalue /= 24;

	days = twlValue.days;
	//计算年月日,先按平年计算，再根据闰年进行调整
    twlTime.year = WLT_BASE_YEAR + days/WLT_DAY_SPER_YEAR;
	midvalue = days - (days/WLT_DAY_SPER_YEAR)*WLT_DAY_SPER_YEAR; //一年内剩余的天数

	//判断当年是否是闰年
	if((twlTime.year%4==0 && twlTime.year%100!=0)|| (twlTime.year%400==0))  bleap = TRUE;
	else bleap =FALSE;
	//相对于基准时间，闰年的天数
	leapdays = (twlTime.year - WLT_BASE_YEAR)/4 - (twlTime.year - WLT_BASE_YEAR)/100 + (twlTime.year - WLT_BASE_YEAR)/400;
/*	if (bleap == TRUE && midvalue < (31+28))
	{
		leapdays -= 1;//减去当年是闰年且值小于59的
	} */
	if (bleap == FALSE// || (bleap == TRUE && midvalue == leapdays + 1)) midvalue --;

    //根据闰年的个数进行调整 年和天数
	if (midvalue <= leapdays)
	{
		leapdays = (twlTime.year - WLT_BASE_YEAR)/4 - (twlTime.year - WLT_BASE_YEAR)/100 + (twlTime.year - WLT_BASE_YEAR)/400;
		twlTime.year -= 1;
		
		if((twlTime.year%4==0 && twlTime.year%100!=0)|| (twlTime.year%400==0))
		{
			midvalue = midvalue + WLT_DAY_SPER_YEAR - leapdays;
		}
		else
		{
		    midvalue = midvalue + WLT_DAY_SPER_YEAR - leapdays;
		}
		days = midvalue;
	}
	else
	{
		days = midvalue - leapdays;
	}
	
    Days2Date(days, &twlTime);
	
	return twlTime;
}

/*********************************************************************
功能：将时间pInTime增加年份，输出到时间pOutTime
参数：pInTime：输入时间，year:需增加的年份，pOutTime：输出时间
返回：
作者：钱志华	2013-09-04
*********************************************************************/
void WLDTIncYear(TWLTime * pInTime,int year,TWLTime * pOutTime)
{
	if (!pOutTime || !pInTime || (pInTime->month <1 || pInTime->month > WLT_MONTH_SPER_YEAR) || (pInTime->day < 1 || pInTime->day > 31) 
		|| (pInTime->hour < 0 || pInTime->hour > 23) || (pInTime->minute < 0 || pInTime->minute > 59) || (pInTime->second < 0 || pInTime->second > 59))  return;
	if (pInTime->year + year < 0) return;

	pOutTime->year = pInTime->year + year;
	pOutTime->month = pInTime->month;
	pOutTime->day = pInTime->day;
	pOutTime->hour = pInTime->hour;
	pOutTime->minute = pInTime->minute;
	pOutTime->second = pInTime->second;
}

/*********************************************************************
功能：将时间pInTime增加月份，输出到时间pOutTime
参数：pInTime：输入时间，month:需增加的月份，pOutTime：输出时间
返回：
作者：钱志华	2013-09-04
*********************************************************************/
void WLDTIncMonth(TWLTime * pInTime,int month,TWLTime * pOutTime)
{
	int tmpTotalMonth;//临时存储总月份数
	if (!pOutTime || !pInTime || (pInTime->month <1 || pInTime->month > WLT_MONTH_SPER_YEAR) || (pInTime->day < 1 || pInTime->day > 31) 
		|| (pInTime->hour < 0 || pInTime->hour > 23) || (pInTime->minute < 0 || pInTime->minute > 59) || (pInTime->second < 0 || pInTime->second > 59))  return;

	tmpTotalMonth = pInTime->year * WLT_MONTH_SPER_YEAR + pInTime->month + month;
	if (tmpTotalMonth < 0) return;
	pOutTime->year = tmpTotalMonth/WLT_MONTH_SPER_YEAR;
	pOutTime->month = tmpTotalMonth % WLT_MONTH_SPER_YEAR;
	pOutTime->day = pInTime->day;
	pOutTime->hour = pInTime->hour;
	pOutTime->minute = pInTime->minute;
	pOutTime->second = pInTime->second;
}

/*********************************************************************
功能：将时间pInTime增加星期，输出到时间pOutTime
参数：pInTime：输入时间，week:需增加的星期，pOutTime：输出时间
返回：
作者：钱志华	2013-09-04
*********************************************************************/
void WLDTIncWeek(TWLTime * pInTime,int week,TWLTime * pOutTime)
{
	TDays_CurrentdaySeconds twlValue;
	if (!pOutTime || !pInTime || (pInTime->month <1 || pInTime->month > WLT_MONTH_SPER_YEAR) || (pInTime->day < 1 || pInTime->day > 31) 
		|| (pInTime->hour < 0 || pInTime->hour > 23) || (pInTime->minute < 0 || pInTime->minute > 59) || (pInTime->second < 0 || pInTime->second > 59))  return;

	twlValue = TWLTime2Value(pInTime);
	twlValue.days += WLT_DAY_SPER_WEEK*week;
	*pOutTime = Value2TWLTime(twlValue);
}

/*********************************************************************
功能：将时间pInTime增加天数，输出到时间pOutTime
参数：pInTime：输入时间，day:需增加的天数，pOutTime：输出时间
返回：
作者：钱志华	2013-09-04
*********************************************************************/
void WLDTIncDay(TWLTime * pInTime,int day,TWLTime * pOutTime)
{
    TDays_CurrentdaySeconds twlValue;
	if (!pOutTime || !pInTime || (pInTime->month <1 || pInTime->month > WLT_MONTH_SPER_YEAR) || (pInTime->day < 1 || pInTime->day > 31) 
		|| (pInTime->hour < 0 || pInTime->hour > 23) || (pInTime->minute < 0 || pInTime->minute > 59) || (pInTime->second < 0 || pInTime->second > 59))  return;

	twlValue = TWLTime2Value(pInTime);
	twlValue.days += day;
	*pOutTime = Value2TWLTime(twlValue);
}

/*********************************************************************
功能：将时间pInTime增加小时数，输出到时间pOutTime
参数：pInTime：输入时间，hour:需增加的小时数，pOutTime：输出时间
返回：
作者：钱志华	2013-09-04
*********************************************************************/
void WLDTIncHour(TWLTime * pInTime,int hour,TWLTime * pOutTime)
{
	TDays_CurrentdaySeconds twlValue;
	if (!pOutTime || !pInTime || (pInTime->month <1 || pInTime->month > WLT_MONTH_SPER_YEAR) || (pInTime->day < 1 || pInTime->day > 31) 
		|| (pInTime->hour < 0 || pInTime->hour > 23) || (pInTime->minute < 0 || pInTime->minute > 59) || (pInTime->second < 0 || pInTime->second > 59))  return;


	twlValue = TWLTime2Value(pInTime);	
	twlValue.days += hour/WLT_HOUR_SPER_DAY;
	twlValue.currentDaySeconds += (hour%WLT_HOUR_SPER_DAY)*(WLT_SECOND_SPER_HOUR);
	if (twlValue.currentDaySeconds < 0)
	{
		twlValue.days --;
		twlValue.currentDaySeconds += WLT_SECOND_SPER_DAY;
	}
	*pOutTime = Value2TWLTime(twlValue);
}

/*********************************************************************
功能：将时间pInTime增加分钟数，输出到时间pOutTime
参数：pInTime：输入时间，minute:需增加的分钟数，pOutTime：输出时间
返回：
作者：钱志华	2013-09-04
*********************************************************************/
void WLDTIncMinute(TWLTime * pInTime,int minute,TWLTime * pOutTime)
{
	TDays_CurrentdaySeconds twlValue;
	if (!pOutTime || !pInTime || (pInTime->month <1 || pInTime->month > WLT_MONTH_SPER_YEAR) || (pInTime->day < 1 || pInTime->day > 31) 
		|| (pInTime->hour < 0 || pInTime->hour > 23) || (pInTime->minute < 0 || pInTime->minute > 59) || (pInTime->second < 0 || pInTime->second > 59))  return;

	twlValue = TWLTime2Value(pInTime);
	twlValue.days += minute/(WLT_HOUR_SPER_DAY*WLT_SECOND_SPER_MINUTE);
	twlValue.currentDaySeconds += (minute%(WLT_HOUR_SPER_DAY*WLT_SECOND_SPER_MINUTE))*WLT_SECOND_SPER_MINUTE;
	if (twlValue.currentDaySeconds < 0)
	{
		twlValue.days --;
		twlValue.currentDaySeconds += WLT_SECOND_SPER_DAY;
	}
	*pOutTime = Value2TWLTime(twlValue);
}

/*********************************************************************
功能：将时间pInTime增加秒数，输出到时间pOutTime
参数：pInTime：输入时间，minute:需增加的秒数，pOutTime：输出时间
返回：
作者：钱志华	2013-09-04
*********************************************************************/
void WLDTIncSecond(TWLTime * pInTime,int second,TWLTime * pOutTime)
{
	TDays_CurrentdaySeconds twlValue;
	if (!pOutTime || !pInTime || (pInTime->month <1 || pInTime->month > WLT_MONTH_SPER_YEAR) || (pInTime->day < 1 || pInTime->day > 31) 
		|| (pInTime->hour < 0 || pInTime->hour > 23) || (pInTime->minute < 0 || pInTime->minute > 59) || (pInTime->second < 0 || pInTime->second > 59))  return;

	twlValue = TWLTime2Value(pInTime);
	twlValue.days += second/WLT_SECOND_SPER_DAY;
	twlValue.currentDaySeconds += second%WLT_SECOND_SPER_DAY;
	if (twlValue.currentDaySeconds < 0)
	{
		twlValue.days --;
		twlValue.currentDaySeconds += WLT_SECOND_SPER_DAY;
	}
	*pOutTime = Value2TWLTime(twlValue);
}

/*********************************************************************
功能：将时间pInTime增加微秒数，输出到时间pOutTime
参数：pInTime：输入时间，milliSecond:需增加的微秒数，pOutTime：输出时间
返回：
作者：钱志华	2013-09-04
*********************************************************************/
void WLDTIncMilliSecond(TWLTime * pInTime,int milliSecond,TWLTime * pOutTime)
{
	WLDTIncSecond(pInTime, milliSecond/1000, pOutTime);
}

void WLDTUTC2Beijin(TWLTime * pUTCTime,TWLTime * pBeijinTime)
{
	WLDTIncHour(pUTCTime,8,pBeijinTime);
}

///*********************************************************************
//功能：将输入的时间字符串strTime转换为时间，输出到pOutTime
//参数：strTime：输入时间字符串，pOutTime：输出时间
//返回：
//作者：钱志华	2013-09-04
//注意：strTime字符串格式为：yyyy-mm-dd hh:mm:ss zzz 24个字符
///*******************************************************************
//void StrToDateTime(char * strTime, TWLTime * pOutTime)
//{
//	int index = 0;
//	char tmpStr[5];
//	int tmpIndex = 0;
//	int len;
//    if (!strTime || !pOutTime) return;
//	if ((len =strlen(strTime)) != 24) return;
//    
//    for (; strTime[index] != '\0'; index ++)
//    {
//		tmpStr[tmpIndex] = strTime[index];
//		tmpIndex ++;
//		if (index == 3) //获得年
//		{
//			tmpStr[tmpIndex] = '\0';
//			pOutTime->year = (WORD)atoi(tmpStr);
//			index ++;
//			tmpIndex = 0;
//		}
//		if (index == 6) //获得月
//		{
//			tmpStr[tmpIndex] = '\0';
//			pOutTime->month = (BYTE)atoi(tmpStr);
//			index ++;
//			tmpIndex = 0;
//		}
//		if (index == 9) //获得日
//		{
//			tmpStr[tmpIndex] = '\0';
//			pOutTime->day = (BYTE)atoi(tmpStr);
//			index ++;
//			tmpIndex = 0;
//		}
//		if (index == 12) //获得时
//		{
//			tmpStr[tmpIndex] = '\0';
//			pOutTime->hour = (BYTE)atoi(tmpStr);
//			index ++;
//			tmpIndex = 0;
//		}
//		if (index == 15) //获得分
//		{
//			tmpStr[tmpIndex] = '\0';
//			pOutTime->minute = (BYTE)atoi(tmpStr);
//			index ++;
//			tmpIndex = 0;
//		}
//		if (index == 18) //获得秒
//		{
//			tmpStr[tmpIndex] = '\0';
//			pOutTime->second = (BYTE)atoi(tmpStr);
//			index ++;
//			tmpIndex = 0;
//		}
//    }
//}
//
///*********************************************************************
//功能：将输入时间pInTime，转换为时间字符串strTime
//参数：pInTime：输入时间，strTime：时间字符串
//返回：
//作者：钱志华	2013-09-04
//注意：strTime字符串格式为：yyyy-mm-dd hh:mm:ss zzz 24个字符
///*******************************************************************
//void DateTimeToStr(TWLTime * pInTime, char * strTime)
//{
//	int len = strlen(strTime);
//	if (!pInTime || (pInTime->month <1 || pInTime->month > WLT_MONTH_SPER_YEAR) || (pInTime->day < 1 || pInTime->day > 31) 
//		|| (pInTime->hour < 0 || pInTime->hour > 23) || (pInTime->minute < 0 || pInTime->minute > 59) || (pInTime->second < 0 || pInTime->second > 59))  return;
//    
//	memset(strTime, 0, sizeof(char)*len);
//	sprintf(strTime, "%04d-%02d-%02d %02d:%02d:%02d 0000", pInTime->year, pInTime->month, pInTime->day, pInTime->hour, pInTime->minute, pInTime->second);
//}
