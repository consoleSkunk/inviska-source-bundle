#ifndef IComSysAbsoluteDay_h
#define IComSysAbsoluteDay_h

#define DAY_SUNDAY		0
#define DAY_MONDAY		1
#define DAY_TUESDAY		2
#define DAY_WEDNESDAY	3
#define DAY_THURSDAY	4
#define DAY_FRIDAY		5
#define DAY_SATURDAY	6


class IComSysAbsoluteDay
{
public:
	static const char* m_krgszDaysOfWeek[7];
	static const char* m_krgszAbrDaysOfWeek[7];
	static const char* m_krgszMonthsOfYear[12];
	static const char* m_krgszAbrMonthsOfYear[12];

	
public:
    static int LastDayOfGregorianMonth(const int kiMonth, const int kiYear);

    static int ConvertDateToAbsoluteDay(const int kiDay, const int kiMonth, const int kiYear);
	static int ConvertYYYY_MM_DDToAbsoluteDay(const char * szDate);
	static int ConvertMM_DD_YYYYToAbsoluteDay(const char * szDate);

    static void ConvertAbsoluteDayToDate(const int kiAbsDay, int & riDay, int & riMonth, int & riYear);
    static void ConvertAbsoluteDayToDate(const int kiAbsDay, int & riDay, int & riMonth);
    static void ConvertAbsoluteDayToDate(const int kiAbsDay, int & riDay);
    static void ConvertAbsoluteDayToDate(const int kiAbsDay, char* szDay);

    static bool IsDayWeekend(const int kiAbsDay);

    static int ConvertAbsoluteDayToDayOfWeek(const int kiAbsDay)    {return kiAbsDay%7;}
};

#endif // IComSysAbsoluteDay_h
