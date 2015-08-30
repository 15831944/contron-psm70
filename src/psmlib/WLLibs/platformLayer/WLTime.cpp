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
���ܣ�������ת��Ϊ����
������days:��������꣬��daysΪ[1,366]������Ϊ[1,365]��ptwlDate�����ڽṹ��ָ��
���أ�����0����ʾת���ɹ�������-1����ʾת��ʧ��
���ߣ�Ǯ־��	2013-09-03
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
	//�жϵ����Ƿ�������
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
���ܣ�������ת��Ϊ����
������ptwlTime��ʱ��ṹ��ָ��
���أ���������
���ߣ�Ǯ־��	2013-09-03
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
	//�жϵ����Ƿ�������
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
���ܣ���ʱ�䣨��ȷ���룩ת��ΪTDays_CurrentdaySeconds������ֵ������ֵ��
������ptwlTime��ʱ��ṹ��ָ��
���أ����ؽṹ��TDays_CurrentdaySeconds
���ߣ�Ǯ־��	2013-09-03
*********************************************************************/
TDays_CurrentdaySeconds TWLTime2Value(TWLTime *ptwlTime)
{
	TDays_CurrentdaySeconds twlValue;
	int year2days, month2days, day2days, datadays;
	int leapdays, bleap;
	int bafterStandardTime;  //�ڻ�׼ʱ��֮��
	if (!ptwlTime || (ptwlTime->month <1 || ptwlTime->month > WLT_MONTH_SPER_YEAR) || (ptwlTime->day < 1 || ptwlTime->day > 31) 
		|| (ptwlTime->hour < 0 || ptwlTime->hour > 23) || (ptwlTime->minute < 0 || ptwlTime->minute > 59) || (ptwlTime->second < 0 || ptwlTime->second > 59)) 
	{ 
		printf("����ֵ����\n");
		twlValue.days = 0;
		twlValue.currentDaySeconds = 0;
		return twlValue;
	}

	//�жϵ����Ƿ�������
	if((ptwlTime->year%4==0 && ptwlTime->year%100!=0)|| (ptwlTime->year%400==0))  bleap = TRUE;
	else bleap =FALSE;
    //�ж�2�·����ڵĺϷ���
	if ((bleap == FALSE) && (ptwlTime->month == 2 && ptwlTime->day == 29))
	{ 
		printf("����ֵ����\n");
		twlValue.days = 0;
		twlValue.currentDaySeconds = 0;
		return twlValue;
	}

	//����ڻ�׼ʱ�䣬���������
	leapdays = (ptwlTime->year - WLT_BASE_YEAR)/4 - (ptwlTime->year - WLT_BASE_YEAR)/100 + (ptwlTime->year - WLT_BASE_YEAR)/400;
    
	//���ۻ������������꣺366�죬ƽ�꣺365��
    year2days = (ptwlTime->year - WLT_BASE_YEAR)*WLT_DAY_SPER_YEAR;

	if (bleap == FALSE)  year2days ++;

	//����ת��������
	datadays = Date2Days(ptwlTime);
	
	//ʵ������
	twlValue.days = year2days + datadays//month2days + day2days + leapdays;
    //������������
    twlValue.currentDaySeconds = ((ptwlTime->hour)*60 + ptwlTime->minute)*60 + ptwlTime->second;
    return twlValue;
}


/*********************************************************************
���ܣ�������ֵת��Ϊʱ�� ��ȷ����
������twlValue������ֵ������ֵ
���أ�����ʱ��
���ߣ�Ǯ־��	2013-09-03
*********************************************************************/
TWLTime Value2TWLTime(TDays_CurrentdaySeconds twlValue)
{
    TWLTime twlTime;
	unsigned long midvalue = twlValue.currentDaySeconds;
	int days;
	int leapdays;
	int bleap;

	//����ʱ����
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
	//����������,�Ȱ�ƽ����㣬�ٸ���������е���
    twlTime.year = WLT_BASE_YEAR + days/WLT_DAY_SPER_YEAR;
	midvalue = days - (days/WLT_DAY_SPER_YEAR)*WLT_DAY_SPER_YEAR; //һ����ʣ�������

	//�жϵ����Ƿ�������
	if((twlTime.year%4==0 && twlTime.year%100!=0)|| (twlTime.year%400==0))  bleap = TRUE;
	else bleap =FALSE;
	//����ڻ�׼ʱ�䣬���������
	leapdays = (twlTime.year - WLT_BASE_YEAR)/4 - (twlTime.year - WLT_BASE_YEAR)/100 + (twlTime.year - WLT_BASE_YEAR)/400;
/*	if (bleap == TRUE && midvalue < (31+28))
	{
		leapdays -= 1;//��ȥ������������ֵС��59��
	} */
	if (bleap == FALSE// || (bleap == TRUE && midvalue == leapdays + 1)) midvalue --;

    //��������ĸ������е��� �������
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
���ܣ���ʱ��pInTime������ݣ������ʱ��pOutTime
������pInTime������ʱ�䣬year:�����ӵ���ݣ�pOutTime�����ʱ��
���أ�
���ߣ�Ǯ־��	2013-09-04
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
���ܣ���ʱ��pInTime�����·ݣ������ʱ��pOutTime
������pInTime������ʱ�䣬month:�����ӵ��·ݣ�pOutTime�����ʱ��
���أ�
���ߣ�Ǯ־��	2013-09-04
*********************************************************************/
void WLDTIncMonth(TWLTime * pInTime,int month,TWLTime * pOutTime)
{
	int tmpTotalMonth;//��ʱ�洢���·���
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
���ܣ���ʱ��pInTime�������ڣ������ʱ��pOutTime
������pInTime������ʱ�䣬week:�����ӵ����ڣ�pOutTime�����ʱ��
���أ�
���ߣ�Ǯ־��	2013-09-04
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
���ܣ���ʱ��pInTime���������������ʱ��pOutTime
������pInTime������ʱ�䣬day:�����ӵ�������pOutTime�����ʱ��
���أ�
���ߣ�Ǯ־��	2013-09-04
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
���ܣ���ʱ��pInTime����Сʱ���������ʱ��pOutTime
������pInTime������ʱ�䣬hour:�����ӵ�Сʱ����pOutTime�����ʱ��
���أ�
���ߣ�Ǯ־��	2013-09-04
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
���ܣ���ʱ��pInTime���ӷ������������ʱ��pOutTime
������pInTime������ʱ�䣬minute:�����ӵķ�������pOutTime�����ʱ��
���أ�
���ߣ�Ǯ־��	2013-09-04
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
���ܣ���ʱ��pInTime���������������ʱ��pOutTime
������pInTime������ʱ�䣬minute:�����ӵ�������pOutTime�����ʱ��
���أ�
���ߣ�Ǯ־��	2013-09-04
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
���ܣ���ʱ��pInTime����΢�����������ʱ��pOutTime
������pInTime������ʱ�䣬milliSecond:�����ӵ�΢������pOutTime�����ʱ��
���أ�
���ߣ�Ǯ־��	2013-09-04
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
//���ܣ��������ʱ���ַ���strTimeת��Ϊʱ�䣬�����pOutTime
//������strTime������ʱ���ַ�����pOutTime�����ʱ��
//���أ�
//���ߣ�Ǯ־��	2013-09-04
//ע�⣺strTime�ַ�����ʽΪ��yyyy-mm-dd hh:mm:ss zzz 24���ַ�
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
//		if (index == 3) //�����
//		{
//			tmpStr[tmpIndex] = '\0';
//			pOutTime->year = (WORD)atoi(tmpStr);
//			index ++;
//			tmpIndex = 0;
//		}
//		if (index == 6) //�����
//		{
//			tmpStr[tmpIndex] = '\0';
//			pOutTime->month = (BYTE)atoi(tmpStr);
//			index ++;
//			tmpIndex = 0;
//		}
//		if (index == 9) //�����
//		{
//			tmpStr[tmpIndex] = '\0';
//			pOutTime->day = (BYTE)atoi(tmpStr);
//			index ++;
//			tmpIndex = 0;
//		}
//		if (index == 12) //���ʱ
//		{
//			tmpStr[tmpIndex] = '\0';
//			pOutTime->hour = (BYTE)atoi(tmpStr);
//			index ++;
//			tmpIndex = 0;
//		}
//		if (index == 15) //��÷�
//		{
//			tmpStr[tmpIndex] = '\0';
//			pOutTime->minute = (BYTE)atoi(tmpStr);
//			index ++;
//			tmpIndex = 0;
//		}
//		if (index == 18) //�����
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
//���ܣ�������ʱ��pInTime��ת��Ϊʱ���ַ���strTime
//������pInTime������ʱ�䣬strTime��ʱ���ַ���
//���أ�
//���ߣ�Ǯ־��	2013-09-04
//ע�⣺strTime�ַ�����ʽΪ��yyyy-mm-dd hh:mm:ss zzz 24���ַ�
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
