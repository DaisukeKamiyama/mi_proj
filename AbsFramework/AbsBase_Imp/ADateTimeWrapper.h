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

ADateWrapper

*/

#pragma once

#include "../AbsBase/ABase.h"

/**
ì˙éûéÊìæÉâÉbÉpÅ[
*/
class ADateTimeWrapper
{
  public:
	static void	GetDateTimeText( const ADateTime inDateTime, AText& outText );
	static void	GetDateTimeText( AText& outText );
	static void	GetLongDateText( AText& outText );
	static void	GetShortDateText( AText& outText );
	static void	Get6LettersDateText( AText& outText );
	static void	GetTimeText( AText& outText );
	static void	GetYearText( AText& outText );
	static void	Get2LettersYearText( AText& outText );
	static void	GetMonthText( AText& outText );
	static void	Get2LettersMonthText( AText& outText );
	static void	GetDayText( AText& outText );
	static void	Get2LettersDayText( AText& outText );
	static void	GetDayOfWeek( AText& outText );//#537
	static void	GetDayOfWeekLong( AText& outText );//#537
	static void	GetDateTimeTextForFileName( AText& outText );//R0000
	static void	GetHourText( AText& outText );//R0116
	static void	GetHour12Text( AText& outText );//R0116
	static void	Get2LettersHourText( AText& outText );//R0116
	static void	Get2LettersHour12Text( AText& outText );//R0116
	static void	GetAMPM( AText& outText );//R0116
	static void	GetMinuteText( AText& outText );//R0116
	static void	Get2LettersMinuteText( AText& outText );//R0116
	static void	GetSecondText( AText& outText );//R0116
	static void	Get2LettersSecondText( AText& outText );//R0116
	static ADateTime	GetCurrentTime();//#695
	static ADateTime	GetDateTime( const ANumber inYear, const ANumber inMonth, const ANumber inDay,
						const ANumber inHour, const ANumber inMinute, const ANumber inSecond );//#81
	static void	GetDateTimeDiff( const ADateTime inDateTime1, const ADateTime inDateTime2, 
				ANumber& outYear, ANumber& outMonth, ANumber& outDay,
				ANumber& outHour, ANumber& outMinute, ANumber& outSecond );//#81
	static ANumber	GetDateTimeDiff( const ADateTime inDateTime1, const ADateTime inDateTime2 );
  private:
	static void	Get12HourAMPM( ANumber& outHour, ABool& outPM );//R0116
	
};



