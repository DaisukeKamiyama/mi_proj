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
#pragma mark MacOSX�p

//DateTimerFormatter
CFDateFormatterRef	gMediumStyleDateTimeFormatter = NULL;
CFDateFormatterRef	gMediumStyleTimeFormatter = NULL;

//TimeZone
CFTimeZoneRef	gSystemTimeZone = NULL;

/**
���t������擾
*/
void	ADateTimeWrapper::GetDateTimeText( const ADateTime inDateTime, AText& outText )
{
	/*#380
	DateTimeRec	datetime;
	::SecondsToDate(inDateTime,&datetime);
	outText.SetFormattedCstring("%d/%d/%d %02d:%02d",datetime.year,datetime.month,datetime.day,datetime.hour,datetime.minute);
	*/
	
	//���`��DateTimerFormatter���擾�Ȃ�擾
	if( gMediumStyleDateTimeFormatter == NULL )
	{
		CFLocaleRef	currentLocale = ::CFLocaleCopyCurrent();
		gMediumStyleDateTimeFormatter = ::CFDateFormatterCreate(NULL,currentLocale,
					kCFDateFormatterMediumStyle,kCFDateFormatterMediumStyle);//���`���E���`��
	}
	
	CFDateRef	date = ::CFDateCreate(NULL, inDateTime);
	CFStringRef	formattedString = ::CFDateFormatterCreateStringWithDate(NULL, gMediumStyleDateTimeFormatter, date);
	outText.SetFromCFString(formattedString);
	::CFRelease(formattedString);
	::CFRelease(date);
}

/**
���݂̓��t������擾
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
	
	//���`��DateTimerFormatter���擾�Ȃ�擾
	if( gMediumStyleDateTimeFormatter == NULL )
	{
		CFLocaleRef	currentLocale = ::CFLocaleCopyCurrent();
		gMediumStyleDateTimeFormatter = ::CFDateFormatterCreate(NULL,currentLocale,
					kCFDateFormatterMediumStyle,kCFDateFormatterMediumStyle);//���`���E���`��
	}
	
	CFDateRef	date = ::CFDateCreate(NULL, ::CFAbsoluteTimeGetCurrent());
	CFStringRef	formattedString = ::CFDateFormatterCreateStringWithDate(NULL, gMediumStyleDateTimeFormatter, date);
	outText.SetFromCFString(formattedString);
	::CFRelease(formattedString);
	::CFRelease(date);
}

/**
���݂̓��t������擾�i���`���j
*/
void	ADateTimeWrapper::GetLongDateText( AText& outText )
{
	CFDateRef	date = ::CFDateCreate(NULL, ::CFAbsoluteTimeGetCurrent());
	CFLocaleRef	currentLocale = ::CFLocaleCopyCurrent();
	CFDateFormatterRef	dateFormatter = ::CFDateFormatterCreate(NULL,currentLocale,
			kCFDateFormatterFullStyle,kCFDateFormatterNoStyle);//�t���`��
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
/*	���L����DateString()�Ƃ͎኱�����񂪕ς���Ă��܂��̂�DateString()�̂܂܂ɂ���
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
���݂̓��t������擾�i�Z�`���j
*/
void	ADateTimeWrapper::GetShortDateText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d/%d/%d",datetime.year,datetime.month,datetime.day);
}

/**
���݂̓��t������擾�i6�����`���j
*/
void	ADateTimeWrapper::Get6LettersDateText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d%02d%02d",(datetime.year%100),datetime.month,datetime.day);
}

/**
���݂̎���������擾
*/
void	ADateTimeWrapper::GetTimeText( AText& outText )
{
	//���`��TimerFormatter���擾�Ȃ�擾
	if( gMediumStyleTimeFormatter == NULL )
	{
		CFLocaleRef	currentLocale = ::CFLocaleCopyCurrent();
		gMediumStyleTimeFormatter = ::CFDateFormatterCreate(NULL,currentLocale,
					kCFDateFormatterNoStyle,kCFDateFormatterMediumStyle);//���`��
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
���݂̔N������擾
*/
void	ADateTimeWrapper::GetYearText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d",datetime.year);
}

/**
���݂̔N������擾�i2�����`���j
*/
void	ADateTimeWrapper::Get2LettersYearText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d",(datetime.year%100));
}

/**
���݂̌�������擾
*/
void	ADateTimeWrapper::GetMonthText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d",datetime.month);
}

/**
���݂̌�������擾�i2�����`���j
*/
void	ADateTimeWrapper::Get2LettersMonthText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d",datetime.month);
}

/**
���݂̓�������擾
*/
void	ADateTimeWrapper::GetDayText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d",datetime.day);
}

/**
���݂̓�������擾�i2�����`���j
*/
void	ADateTimeWrapper::Get2LettersDayText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d",datetime.day);
}

//#537
/**
�j���擾
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
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	SInt32	dayOfWeek = ::CFAbsoluteTimeGetDayOfWeek(GetCurrentTime(),gSystemTimeZone);
	outText.SetLocalizedText("DayOfWeek",dayOfWeek%7);
}

//#537
/**
�j���擾
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
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	SInt32	dayOfWeek = ::CFAbsoluteTimeGetDayOfWeek(GetCurrentTime(),gSystemTimeZone);
	outText.SetLocalizedText("DayOfWeekLong",dayOfWeek%7);
}

//R0116
/**
���݂̎��ԕ�����擾�i24���Ԑ��j
*/
void	ADateTimeWrapper::GetHourText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d",datetime.hour);
}

//R0116
/**
���݂̎��ԕ�����擾�i12���Ԑ��j
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
���݂̎��ԕ�����擾�i24���Ԑ��j�i2�����`���j
*/
void	ADateTimeWrapper::Get2LettersHourText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d",datetime.hour);
}

//R0116
/**
���݂̎��ԕ�����擾�i12���Ԑ��j�i2�����`���j
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
���݂�AM/PM������擾
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
���݂̎��ԕ�����AAM/PM�擾�i12���Ԑ��j
*/
void	ADateTimeWrapper::Get12HourAMPM( ANumber& outHour, ABool& outPM )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	//0����AM12��
	if( datetime.hour == 0 )
	{
		outHour = 12;
		outPM = false;
	}
	//1����AM1���`11����AM11��
	else if( datetime.hour > 0 && datetime.hour < 12 )
	{
		outHour = datetime.hour;
		outPM = false;
	}
	//12����PM12��
	else if( datetime.hour == 12 )
	{
		outHour = 12;
		outPM = true;
	}
	//13����PM1���`23����PM11��
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
���݂̕�������擾
*/
void	ADateTimeWrapper::GetMinuteText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d",datetime.minute);
}

//R0116
/**
���݂̕�������擾�i2�����`���j
*/
void	ADateTimeWrapper::Get2LettersMinuteText( AText& outText )
{
	/*#1034
	unsigned long	secs;
	::GetDateTime(&secs);
	DateTimeRec	datetime;
	::SecondsToDate(secs,&datetime);
	*/
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d",datetime.minute);
}

//R0116
/**
���݂̕b������擾
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
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%d",(int)(datetime.second));
}

//R0116
/**
���݂̕b������擾�i2�����`���j
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
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�V�X�e���^�C���]�[���ł̎w�������������擾
	CFGregorianDate	datetime = ::CFAbsoluteTimeGetGregorianDate(GetCurrentTime(),gSystemTimeZone);
	outText.SetFormattedCstring("%02d",(int)(datetime.second));
}

//#695
/**
���݂̎������擾
*/
ADateTime	ADateTimeWrapper::GetCurrentTime()
{
	return ::CFAbsoluteTimeGetCurrent();
}

//#81

/**
�w��N���������b��DateTime���擾
*/
ADateTime	ADateTimeWrapper::GetDateTime( const ANumber inYear, const ANumber inMonth, const ANumber inDay,
			const ANumber inHour, const ANumber inMinute, const ANumber inSecond )
{
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//datetime�擾
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
���ԍ������擾
*/
void	ADateTimeWrapper::GetDateTimeDiff( const ADateTime inDateTime1, const ADateTime inDateTime2, 
		ANumber& outYear, ANumber& outMonth, ANumber& outDay,
		ANumber& outHour, ANumber& outMinute, ANumber& outSecond )
{
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�����擾
	CFGregorianUnits	diff = ::CFAbsoluteTimeGetDifferenceAsGregorianUnits(inDateTime1,inDateTime2,gSystemTimeZone,kCFGregorianAllUnits);
	outYear = diff.years;
	outMonth = diff.months;
	outDay = diff.days;
	outHour = diff.hours;
	outMinute = diff.minutes;
	outSecond = diff.seconds;
}

/**
�b�������擾
*/
ANumber	ADateTimeWrapper::GetDateTimeDiff( const ADateTime inDateTime1, const ADateTime inDateTime2 )
{
	//TimeZone���擾�Ȃ�擾
	if( gSystemTimeZone == NULL )
	{
		gSystemTimeZone = ::CFTimeZoneCopySystem();
	}
	//�b�����擾
	CFGregorianUnits	diff = ::CFAbsoluteTimeGetDifferenceAsGregorianUnits(inDateTime1,inDateTime2,gSystemTimeZone,kCFGregorianUnitsSeconds);
	return diff.seconds;
}

#endif

#if IMPLEMENTATION_FOR_WINDOWS

#pragma mark ===========================
#pragma mark Windows�p

#include <time.h>

/**
���t������擾
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
	//���t�i�Z�`���j
	buf[0] = 0;
	int	ret = ::GetDateFormatW(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,
		buf,sizeof(buf)/sizeof(buf[0]));
	if( ret > 0 )
	{
		outText.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	}
	outText.Add(kUChar_Space);
	//����
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
���݂̓��t������擾
*/
void	ADateTimeWrapper::GetDateTimeText( AText& outText )
{
	ADateTime	t;
	_time64(&t);
	GetDateTimeText(t,outText);
}

/**
���݂̓��t������擾�i���`���j
*/
void	ADateTimeWrapper::GetLongDateText( AText& outText )
{
	outText.DeleteAll();
	wchar_t	buf[1024];
	//���t�i���`���j
	buf[0] = 0;
	int	ret = ::GetDateFormatW(LOCALE_USER_DEFAULT,DATE_LONGDATE,NULL,NULL,
		buf,sizeof(buf)/sizeof(buf[0]));
	if( ret > 0 )
	{
		outText.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	}
}

/**
���݂̓��t������擾�i�Z�`���j
*/
void	ADateTimeWrapper::GetShortDateText( AText& outText )
{
	outText.DeleteAll();
	wchar_t	buf[1024];
	//���t�i�Z�`���j
	buf[0] = 0;
	int	ret = ::GetDateFormatW(LOCALE_USER_DEFAULT,DATE_SHORTDATE,NULL,NULL,
		buf,sizeof(buf)/sizeof(buf[0]));
	if( ret > 0 )
	{
		outText.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	}
}

/**
���݂̓��t������擾�i6�����`���j
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
���݂̎���������擾
*/
void	ADateTimeWrapper::GetTimeText( AText& outText )
{
	outText.DeleteAll();
	wchar_t	buf[1024];
	//����
	buf[0] = 0;
	int	ret = ::GetTimeFormatW(LOCALE_USER_DEFAULT,0,NULL,NULL,
		buf,sizeof(buf)/sizeof(buf[0]));
	if( ret > 0 )
	{
		outText.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	}
}

/**
���݂̔N������擾
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
���݂̔N������擾�i2�����`���j
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
���݂̌�������擾
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
���݂̌�������擾�i2�����`���j
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
���݂̓�������擾
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
���݂̓�������擾�i2�����`���j
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
�j���擾
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
�j���擾
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
���݂̎��ԕ�����擾�i24���Ԑ��j
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
���݂̎��ԕ�����擾�i12���Ԑ��j
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
���݂̎��ԕ�����擾�i24���Ԑ��j�i2�����`���j
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
���݂̎��ԕ�����擾�i12���Ԑ��j�i2�����`���j
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
���݂�AM/PM������擾
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
���݂̎��ԕ�����AAM/PM�擾�i12���Ԑ��j
*/
void	ADateTimeWrapper::Get12HourAMPM( ANumber& outHour, ABool& outPM )
{
	ADateTime	t = 0;
	_time64(&t);
	struct tm	time = {0};
	::_localtime64_s(&time,&t);
	//0����AM12��
	if( time.tm_hour == 0 )
	{
		outHour = 12;
		outPM = false;
	}
	//1����AM1���`11����AM11��
	else if( time.tm_hour > 0 && time.tm_hour < 12 )
	{
		outHour = time.tm_hour;
		outPM = false;
	}
	//12����PM12��
	else if( time.tm_hour == 12 )
	{
		outHour = 12;
		outPM = true;
	}
	//13����PM1���`23����PM11��
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
���݂̕�������擾
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
���݂̕�������擾�i2�����`���j
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
���݂̕b������擾
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
���݂̕b������擾�i2�����`���j
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
���݂̎������擾
*/
ADateTime	ADateTimeWrapper::GetCurrentTime()
{
	ADateTime	t = 0;
	_time64(&t);
	return t;
}

#endif

#pragma mark ===========================
#pragma mark ����

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

