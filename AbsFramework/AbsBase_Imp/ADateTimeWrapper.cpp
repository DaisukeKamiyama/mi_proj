/*

Copyright (C) 2018  Daisuke Kamiyama (https://www.mimikaki.net)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
/*

ADateTimeWrapper

*/

#include "stdafx.h"

#include "ADateTimeWrapper.h"

#if IMPLEMENTATION_FOR_MACOSX
#pragma mark ===========================
#pragma mark MacOSX用

//DateTimerFormatter
CFDateFormatterRef	gMediumStyleDateTimeFormatter = NULL;
CFDateFormatterRef	gMediumStyleTimeFormatter = NULL;

//TimeZone
CFTimeZoneRef	gSystemTimeZone = NULL;

/**
日付文字列取得
*/
void	ADateTimeWrapper::GetDateTimeText( const ADateTime inDateTime, AText& outText )
{
	/*#380
	DateTimeRec	datetime;
	::SecondsToDate(inDateTime,&datetime);
	outText.SetFormattedCstring("%d/%d/%d %02d:%02d",datetime.year,datetime.month,datetime.day,datetime.hour,datetime.minute);
	*/
	
	//中形式DateTimerFormatter未取得なら取得
	if( gMediumStyleDateTimeFormatter == NULL )
	{
		CFLocaleRef	currentLocale = ::CFLocaleCopyCurrent();
		gMediumStyleDateTimeFormatter = ::CFDateFormatterCreate(NULL,currentLocale,
					kCFDateFormatterMediumStyle,kCFDateFormatterMediumStyle);//中形式・中形式
	}
	
	CFDateRef	date = ::CFDateCreate(NULL, inDateTime);
	CFStringRef	formattedString = ::CFDateFormatterCreateStringWithDate(NULL, gMediumStyleDateTimeFormatter, date);
	outText.SetFromCFString(formattedString);
	::CFRelease(formattedString);
	::CFRelease(date);
}

/**
現在の日付文字列取得
*/
void	ADateTimeWrapper::GetDateTimeText( AText& outText )
{
	/*#380
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	outText.SetFormattedCstring("%d/%d/%d %02d:%02d",datetime.year,datetime.month,datetime.day,datetime.hour,datetime.minute);
	*/
	
	//中形式DateTimerFormatter未取得なら取得
	if( gMediumStyleDateTimeFormatter == NULL )
	{
		CFLocaleRef	currentLocale = ::CFLocaleCopyCurrent();
		gMediumStyleDateTimeFormatter = ::CFDateFormatterCreate(NULL,currentLocale,
					kCFDateFormatterMediumStyle,kCFDateFormatterMediumStyle);//中形式・中形式
	}
	
	CFDateRef	date = ::CFDateCreate(NULL, ::CFAbsoluteTimeGetCurrent());
	CFStringRef	formattedString = ::CFDateFormatterCreateStringWithDate(NULL, gMediumStyleDateTimeFormatter, date);
	outText.SetFromCFString(formattedString);
	::CFRelease(formattedString);
	::CFRelease(date);
}

/**
現在の日付文字列取得（長形式）
*/
void	ADateTimeWrapper::GetLongDateText( AText& outText )
{
	CFDateRef	date = ::CFDateCreate(NULL, ::CFAbsoluteTimeGetCurrent());
	CFLocaleRef	currentLocale = ::CFLocaleCopyCurrent();
	CFDateFormatterRef	dateFormatter = ::CFDateFormatterCreate(NULL,currentLocale,
			kCFDateFormatterFullStyle,kCFDateFormatterNoStyle);//フル形式
	CFStringRef	formattedString = ::CFDateFormatterCreateStringWithDate(NULL, dateFormatter, date);
	outText.SetFromCFString(formattedString);
	::CFRelease(formattedString);
	::CFRelease(dateFormatter);
	::CFRelease(currentLocale);
	::CFRelease(date);
/*#380
	unsigned long	secs;
	::GetDateTime(&secs);
	Str255	str;
	::DateString(secs,longDate,str,::GetIntlResource(1));
	AText	text(str);
	TextEncoding	tec;
	::UpgradeScriptInfoToTextEncoding(smSystemScript,kTextLanguageDontCare,kTextRegionDontCare,NULL,&tec);
	text.ConvertToUTF8(tec);
	outText.SetText(text);
*/
/*	下記だとDateString()とは若干文字列が変わってしまうのでDateString()のままにする
	CFDateFormatterRef	formatterRef = NULL;
	CFStringRef	strRef = NULL;
	try
	{
		::CFLocaleCopyCurrent()
		formatterRef = ::CFDateFormatterCreate(kCFAllocatorDefault,,kCFDateFormatterFullStyle,kCFDateFormatterNoStyle);
		if( formatterRef == NULL )   throw 0;
		strRef = ::CFDateFormatterCreateStringWithAbsoluteTime(kCFAllocatorDefault,formatterRef,::CFAbsoluteTimeGetCurrent());
		if( strRef == NULL )   throw 0;
		outText.SetFromCFString(strRef);
	}
	catch(...)
	{
	}
	if( formatterRef != NULL )   ::CFRelease(formatterRef);
	if( strRef != NULL )   ::CFRelease(strRef);
*/
}

/**
現在の日付文字列取得（短形式）
*/
void	ADateTimeWrapper::GetShortDateText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d/%d/%d",datetime.year,datetime.month,datetime.day);
}

/**
現在の日付文字列取得（6文字形式）
*/
void	ADateTimeWrapper::Get6LettersDateText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d%02d%02d",(datetime.year%100),datetime.month,datetime.day);
}

/**
現在の時刻文字列取得
*/
void	ADateTimeWrapper::GetTimeText( AText& outText )
{
	//中形式TimerFormatter未取得なら取得
	if( gMediumStyleTimeFormatter == NULL )
	{
		CFLocaleRef	currentLocale = ::CFLocaleCopyCurrent();
		gMediumStyleTimeFormatter = ::CFDateFormatterCreate(NULL,currentLocale,
					kCFDateFormatterNoStyle,kCFDateFormatterMediumStyle);//中形式
	}
	
	CFDateRef	date = ::CFDateCreate(NULL, ::CFAbsoluteTimeGetCurrent());
	CFStringRef	formattedString = ::CFDateFormatterCreateStringWithDate(NULL, gMediumStyleTimeFormatter, date);
	outText.SetFromCFString(formattedString);
	::CFRelease(formattedString);
	::CFRelease(date);
	/*#380
	unsigned long	secs;
	::GetDateTime(&secs);
	Str255	str;
	::TimeString(secs,true,str,::GetIntlResource(0));
	AText	text(str);
	TextEncoding	tec;
	::UpgradeScriptInfoToTextEncoding(smSystemScript,kTextLanguageDontCare,kTextRegionDontCare,NULL,&tec);
	text.ConvertToUTF8(tec);
	outText.SetText(text);
	*/
}

/**
現在の年文字列取得
*/
void	ADateTimeWrapper::GetYearText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d",datetime.year);
}

/**
現在の年文字列取得（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersYearText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d",(datetime.year%100));
}

/**
現在の月文字列取得
*/
void	ADateTimeWrapper::GetMonthText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d",datetime.month);
}

/**
現在の月文字列取得（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersMonthText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d",datetime.month);
}

/**
現在の日文字列取得
*/
void	ADateTimeWrapper::GetDayText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d",datetime.day);
}

/**
現在の日文字列取得（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersDayText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d",datetime.day);
}

//#537
/**
曜日取得
*/
void	ADateTimeWrapper::GetDayOfWeek( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	outText.SetLocalizedText("DayOfWeek",datetime.dayOfWeek-1);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	SInt32	dayOfWeek = ::CFAbsoluteTimeGetDayOfWeek(GetCurrentTime(),gSystemTimeZone);
	outText.SetLocalizedText("DayOfWeek",dayOfWeek%7);
}

//#537
/**
曜日取得
*/
void	ADateTimeWrapper::GetDayOfWeekLong( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	outText.SetLocalizedText("DayOfWeekLong",datetime.dayOfWeek-1);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	SInt32	dayOfWeek = ::CFAbsoluteTimeGetDayOfWeek(GetCurrentTime(),gSystemTimeZone);
	outText.SetLocalizedText("DayOfWeekLong",dayOfWeek%7);
}

//R0116
/**
現在の時間文字列取得（24時間制）
*/
void	ADateTimeWrapper::GetHourText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d",datetime.hour);
}

//R0116
/**
現在の時間文字列取得（12時間制）
*/
void	ADateTimeWrapper::GetHour12Text( AText& outText )
{
	ANumber	hour;
	ABool	pm;
	Get12HourAMPM(hour,pm);
	outText.SetFormattedCstring("%d",hour);
}

//R0116
/**
現在の時間文字列取得（24時間制）（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersHourText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d",datetime.hour);
}

//R0116
/**
現在の時間文字列取得（12時間制）（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersHour12Text( AText& outText )
{
	ANumber	hour;
	ABool	pm;
	Get12HourAMPM(hour,pm);
	outText.SetFormattedCstring("%02d",hour);
}

//R0116
/**
現在のAM/PM文字列取得
*/
void	ADateTimeWrapper::GetAMPM( AText& outText )
{
	ANumber	hour;
	ABool	pm;
	Get12HourAMPM(hour,pm);
	if( pm == true )
	{
		outText.SetCstring("PM");
	}
	else
	{
		outText.SetCstring("AM");
	}
}

//R0116
/**
現在の時間文字列、AM/PM取得（12時間制）
*/
void	ADateTimeWrapper::Get12HourAMPM( ANumber& outHour, ABool& outPM )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	//0時→AM12時
	if( datetime.hour == 0 )
	{
		outHour = 12;
		outPM = false;
	}
	//1時→AM1時～11時→AM11時
	else if( datetime.hour > 0 && datetime.hour < 12 )
	{
		outHour = datetime.hour;
		outPM = false;
	}
	//12時→PM12時
	else if( datetime.hour == 12 )
	{
		outHour = 12;
		outPM = true;
	}
	//13時→PM1時～23時→PM11時
	else if( datetime.hour > 12 )
	{
		outHour = datetime.hour-12;
		outPM = true;
	}
	else
	{
		_ACError("",NULL);
	}
}

//R0116
/**
現在の分文字列取得
*/
void	ADateTimeWrapper::GetMinuteText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d",datetime.minute);
}

//R0116
/**
現在の分文字列取得（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersMinuteText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d",datetime.minute);
}

//R0116
/**
現在の秒文字列取得
*/
void	ADateTimeWrapper::GetSecondText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	outText.SetFormattedCstring("%d",datetime.second);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d",(int)(datetime.second));
}

//R0116
/**
現在の秒文字列取得（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersSecondText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	outText.SetFormattedCstring("%02d",datetime.second);
	*/
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//システムタイムゾーンでの指定日時文字列を取得
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d",(int)(datetime.second));
}

//#695
/**
現在の時刻を取得
*/
ADateTime	ADateTimeWrapper::GetCurrentTime()
{
	return ::CFAbsoluteTimeGetCurrent();
}

//#81

/**
指定年月日時分秒のDateTimeを取得
*/
ADateTime	ADateTimeWrapper::GetDateTime( const ANumber inYear, const ANumber inMonth, const ANumber inDay,
			const ANumber inHour, const ANumber inMinute, const ANumber inSecond )
{
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//datetime取得
	CFGregorianDate	gregorian = {0};
	gregorian.year = inYear;
	gregorian.month = inMonth;
	gregorian.day = inDay;
	gregorian.hour = inHour;
	gregorian.minute = inMinute;
	gregorian.second = inSecond;
	ADateTime	datetime = ::CFGregorianDateGetAbsoluteTime(gregorian,gSystemTimeZone);
	return datetime;
}

/**
時間差分を取得
*/
void	ADateTimeWrapper::GetDateTimeDiff( const ADateTime inDateTime1, const ADateTime inDateTime2, 
		ANumber& outYear, ANumber& outMonth, ANumber& outDay,
		ANumber& outHour, ANumber& outMinute, ANumber& outSecond )
{
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//差分取得
	CFGregorianUnits	diff = ::CFAbsoluteTimeGetDifferenceAsGregorianUnits(inDateTime1,inDateTime2,gSystemTimeZone,kCFGregorianAllUnits);
	outYear = diff.years;
	outMonth = diff.months;
	outDay = diff.days;
	outHour = diff.hours;
	outMinute = diff.minutes;
	outSecond = diff.seconds;
}

/**
秒差分を取得
*/
ANumber	ADateTimeWrapper::GetDateTimeDiff( const ADateTime inDateTime1, const ADateTime inDateTime2 )
{
	//TimeZone未取得なら取得
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//秒差分取得
	CFGregorianUnits	diff = ::CFAbsoluteTimeGetDifferenceAsGregorianUnits(inDateTime1,inDateTime2,gSystemTimeZone,kCFGregorianUnitsSeconds);
	return diff.seconds;
}

#endif

#if IMPLEMENTATION_FOR_WINDOWS

#pragma mark ===========================
#pragma mark Windows用

#include <time.h>

/**
日付文字列取得
*/
void	ADateTimeWrapper::GetDateTimeText( const ADateTime inDateTime, AText& outText )
{
	struct tm	time;
	::_localtime64_s(&time,&inDateTime);
	/*
	outText.SetFormattedCstring("%d/%d/%d %02d:%02d",
	time.tm_year+1900,time.tm_mon+1,time.tm_mday,time.tm_hour,time.tm_min);
	*/
	SYSTEMTIME	st = {0};
	st.wYear		= time.tm_year+1900;
	st.wMonth		= time.tm_mon+1;
	st.wDayOfWeek	= time.tm_wday;
	st.wDay			= time.tm_mday;
	st.wHour		= time.tm_hour;
	st.wMinute		= time.tm_min;
	st.wSecond		= time.tm_sec;
	st.wMilliseconds= 0;
	//
	outText.DeleteAll();
	wchar_t	buf[1024];
	//日付（短形式）
	buf[0] = 0;
	int	ret = ::GetDateFormatW(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,
		buf,sizeof(buf)/sizeof(buf[0]));
	if( ret > 0 )
	{
		outText.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	}
	outText.Add(kUChar_Space);
	//時刻
	buf[0] = 0;
	ret = ::GetTimeFormatW(LOCALE_USER_DEFAULT,0,&st,NULL,
		buf,sizeof(buf)/sizeof(buf[0]));
	if( ret > 0 )
	{
		AText	t;
		t.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
		outText.AddText(t);
	}
}

/**
現在の日付文字列取得
*/
void	ADateTimeWrapper::GetDateTimeText( AText& outText )
{
	ADateTime	t;
	_time64(&t);
	GetDateTimeText(t,outText);
}

/**
現在の日付文字列取得（長形式）
*/
void	ADateTimeWrapper::GetLongDateText( AText& outText )
{
	outText.DeleteAll();
	wchar_t	buf[1024];
	//日付（長形式）
	buf[0] = 0;
	int	ret = ::GetDateFormatW(LOCALE_USER_DEFAULT,DATE_LONGDATE,NULL,NULL,
		buf,sizeof(buf)/sizeof(buf[0]));
	if( ret > 0 )
	{
		outText.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	}
}

/**
現在の日付文字列取得（短形式）
*/
void	ADateTimeWrapper::GetShortDateText( AText& outText )
{
	outText.DeleteAll();
	wchar_t	buf[1024];
	//日付（短形式）
	buf[0] = 0;
	int	ret = ::GetDateFormatW(LOCALE_USER_DEFAULT,DATE_SHORTDATE,NULL,NULL,
		buf,sizeof(buf)/sizeof(buf[0]));
	if( ret > 0 )
	{
		outText.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	}
}

/**
現在の日付文字列取得（6文字形式）
*/
void	ADateTimeWrapper::Get6LettersDateText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%02d%02d%02d",
			(time.tm_year%100),time.tm_mon+1,time.tm_mday);
}

/**
現在の時刻文字列取得
*/
void	ADateTimeWrapper::GetTimeText( AText& outText )
{
	outText.DeleteAll();
	wchar_t	buf[1024];
	//時刻
	buf[0] = 0;
	int	ret = ::GetTimeFormatW(LOCALE_USER_DEFAULT,0,NULL,NULL,
		buf,sizeof(buf)/sizeof(buf[0]));
	if( ret > 0 )
	{
		outText.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	}
}

/**
現在の年文字列取得
*/
void	ADateTimeWrapper::GetYearText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%d",time.tm_year+1900);
}

/**
現在の年文字列取得（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersYearText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%02d",(time.tm_year%100));
}

/**
現在の月文字列取得
*/
void	ADateTimeWrapper::GetMonthText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%d",time.tm_mon+1);
}

/**
現在の月文字列取得（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersMonthText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%02d",time.tm_mon+1);
}

/**
現在の日文字列取得
*/
void	ADateTimeWrapper::GetDayText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%d",time.tm_mday);
}

/**
現在の日文字列取得（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersDayText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%02d",time.tm_mday);
}

//#537
/**
曜日取得
*/
void	ADateTimeWrapper::GetDayOfWeek( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetLocalizedText("DayOfWeek",time.tm_wday);
}

//#537
/**
曜日取得
*/
void	ADateTimeWrapper::GetDayOfWeekLong( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetLocalizedText("DayOfWeekLong",time.tm_wday);
}

//R0116
/**
現在の時間文字列取得（24時間制）
*/
void	ADateTimeWrapper::GetHourText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%d",time.tm_hour);
}

//R0116
/**
現在の時間文字列取得（12時間制）
*/
void	ADateTimeWrapper::GetHour12Text( AText& outText )
{
	ANumber	hour;
	ABool	pm;
	Get12HourAMPM(hour,pm);
	outText.SetFormattedCstring("%d",hour);
}

//R0116
/**
現在の時間文字列取得（24時間制）（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersHourText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%02d",time.tm_hour);
}

//R0116
/**
現在の時間文字列取得（12時間制）（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersHour12Text( AText& outText )
{
	ANumber	hour;
	ABool	pm;
	Get12HourAMPM(hour,pm);
	outText.SetFormattedCstring("%02d",hour);
}

//R0116
/**
現在のAM/PM文字列取得
*/
void	ADateTimeWrapper::GetAMPM( AText& outText )
{
	ANumber	hour;
	ABool	pm;
	Get12HourAMPM(hour,pm);
	if( pm == true )
	{
		outText.SetCstring("PM");
	}
	else
	{
		outText.SetCstring("AM");
	}
}

//R0116
/**
現在の時間文字列、AM/PM取得（12時間制）
*/
void	ADateTimeWrapper::Get12HourAMPM( ANumber& outHour, ABool& outPM )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	//0時→AM12時
	if( time.tm_hour == 0 )
	{
		outHour = 12;
		outPM = false;
	}
	//1時→AM1時～11時→AM11時
	else if( time.tm_hour > 0 && time.tm_hour < 12 )
	{
		outHour = time.tm_hour;
		outPM = false;
	}
	//12時→PM12時
	else if( time.tm_hour == 12 )
	{
		outHour = 12;
		outPM = true;
	}
	//13時→PM1時～23時→PM11時
	else if( time.tm_hour > 12 )
	{
		outHour = time.tm_hour-12;
		outPM = true;
	}
	else
	{
		_ACError("",NULL);
	}
}

//R0116
/**
現在の分文字列取得
*/
void	ADateTimeWrapper::GetMinuteText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%d",time.tm_min);
}

//R0116
/**
現在の分文字列取得（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersMinuteText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%02d",time.tm_min);
}

//R0116
/**
現在の秒文字列取得
*/
void	ADateTimeWrapper::GetSecondText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%d",time.tm_sec);
}

//R0116
/**
現在の秒文字列取得（2文字形式）
*/
void	ADateTimeWrapper::Get2LettersSecondText( AText& outText )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	outText.SetFormattedCstring("%02d",time.tm_sec);
}

//#695
/**
現在の時刻を取得
*/
ADateTime	ADateTimeWrapper::GetCurrentTime()
{
	ADateTime	t = 0;
	_time64(&t);
	return t;
}

#endif

#pragma mark ===========================
#pragma mark 共通

//R0000
void	ADateTimeWrapper::GetDateTimeTextForFileName( AText& outText )
{
	outText.SetCstring("_");
	AText	text;
	ADateTimeWrapper::Get6LettersDateText(text);
	outText.AddText(text);
	outText.Add('_');
	ADateTimeWrapper::GetTimeText(text);
	text.ReplaceChar(':','_');
	text.ReplaceChar(' ','_');
	outText.AddText(text);
}

