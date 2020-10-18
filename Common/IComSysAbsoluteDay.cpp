#include <iostream>
#include "IComSysAbsoluteDay.h"

const char* IComSysAbsoluteDay::m_krgszDaysOfWeek[7]		= {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char* IComSysAbsoluteDay::m_krgszAbrDaysOfWeek[7]		= {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
const char* IComSysAbsoluteDay::m_krgszMonthsOfYear[12]		= {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const char* IComSysAbsoluteDay::m_krgszAbrMonthsOfYear[12]  = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


int IComSysAbsoluteDay::LastDayOfGregorianMonth(const int kiMonth, const int kiYear)
{
    switch (kiMonth)
	{
    case 2:	if ((((kiYear % 4) == 0) && ((kiYear % 100) != 0)) || ((kiYear % 400) == 0))
				return 29;
			else
				return 28;

	case 4:
	case 6:
	case 9:
	case 11: return 30;

	default: return 31;
	}
}


int IComSysAbsoluteDay::ConvertDateToAbsoluteDay(const int kiDay, const int kiMonth, const int kiYear)
{ 
    int iAbsDate = kiDay;
    for (int iMonthCount = kiMonth - 1 ; iMonthCount > 0 ; --iMonthCount)
        iAbsDate = iAbsDate + LastDayOfGregorianMonth(iMonthCount, kiYear);
   
    return (iAbsDate + (365 * (kiYear - 1)) + ((kiYear - 1)/4) - ((kiYear - 1)/100) + ((kiYear - 1)/400));
}


int IComSysAbsoluteDay::ConvertYYYY_MM_DDToAbsoluteDay(const char * szDate)
{ 
	int iYear = atoi(szDate);
	int iMonth = atoi(&szDate[5]);
	int iDay = atoi(&szDate[8]);
   	return ConvertDateToAbsoluteDay(iDay, iMonth, iYear);
}


int IComSysAbsoluteDay::ConvertMM_DD_YYYYToAbsoluteDay(const char * szDate)
{ 
	int iMonth = atoi(szDate);
	int iDay = atoi(&szDate[3]);
	int iYear = atoi(&szDate[6]);
   	return ConvertDateToAbsoluteDay(iDay, iMonth, iYear);
}
  

void IComSysAbsoluteDay::ConvertAbsoluteDayToDate(const int kiAbsDay, int & riDay, int & riMonth, int & riYear)
{    
    riYear = kiAbsDay / 366;
    while (kiAbsDay >= ConvertDateToAbsoluteDay(1, 1, riYear+1))
		riYear++;
    
    riMonth = 1;
    while (kiAbsDay > ConvertDateToAbsoluteDay(LastDayOfGregorianMonth(riMonth, riYear), riMonth, riYear))
		riMonth++;

    riDay = kiAbsDay - ConvertDateToAbsoluteDay(1, riMonth, riYear) + 1;
}


void IComSysAbsoluteDay::ConvertAbsoluteDayToDate(const int kiAbsDay, int & riDay, int & riMonth)
{    
    int iYear = kiAbsDay / 366;
    while (kiAbsDay >= ConvertDateToAbsoluteDay(1, 1, iYear+1))
		iYear++;
    
    riMonth = 1;
    while (kiAbsDay > ConvertDateToAbsoluteDay(LastDayOfGregorianMonth(riMonth, iYear), riMonth, iYear))
		riMonth++;

    riDay = kiAbsDay - ConvertDateToAbsoluteDay(1, riMonth, iYear) + 1;
}


void IComSysAbsoluteDay::ConvertAbsoluteDayToDate(const int kiAbsDay, int & riDay)
{    
    int iYear = kiAbsDay / 366;
    while (kiAbsDay >= ConvertDateToAbsoluteDay(1, 1, iYear+1))
		iYear++;
    
    int iMonth = 1;
    while (kiAbsDay > ConvertDateToAbsoluteDay(LastDayOfGregorianMonth(iMonth, iYear), iMonth, iYear))
		iMonth++;

    riDay = kiAbsDay - ConvertDateToAbsoluteDay(1, iMonth, iYear) + 1;
}


void IComSysAbsoluteDay::ConvertAbsoluteDayToDate(const int kiAbsDay, char* szDay)
{    
    int iYear = kiAbsDay / 366;
    while (kiAbsDay >= ConvertDateToAbsoluteDay(1, 1, iYear+1))
		iYear++;
    
    int iMonth = 1;
    while (kiAbsDay > ConvertDateToAbsoluteDay(LastDayOfGregorianMonth(iMonth, iYear), iMonth, iYear))
		iMonth++;

    int iDay = kiAbsDay - ConvertDateToAbsoluteDay(1, iMonth, iYear) + 1;

	if (iDay == 1 || iDay == 21 || iDay == 31)
		sprintf(szDay, "%dst", iDay);
	else if (iDay == 2 || iDay == 22)
		sprintf(szDay, "%dnd", iDay);
	else if (iDay == 3 || iDay == 23)
		sprintf(szDay, "%drd", iDay);
	else
		sprintf(szDay, "%dth", iDay);
}


bool IComSysAbsoluteDay::IsDayWeekend(const int kiAbsDay)
{
    int iDayOfWeek = kiAbsDay % 7;
	if (iDayOfWeek == DAY_SATURDAY || iDayOfWeek == DAY_SUNDAY)
		return true;
	return false;
}
