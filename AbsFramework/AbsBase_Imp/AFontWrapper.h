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

AFontWrapper

*/

#pragma once

#include "../AbsBase/ABase.h"

/**
フォントラッパー
*/
class AFontWrapper
{
  public:
	static void		Init();//win
	static void		GetDialogDefaultFontName( AText& outText ) ;
	static void		GetAppDefaultFontName( AText& outText );
	static void		SetAppDefaultFontName( const AText& inText );//#375
	static ABool	IsFontEnabled( const AText& inFontName );//#521
#if IMPLEMENTATION_FOR_MACOSX
	//#1034 static ATSUFontID	GetATSUFontIDByName( const AText& inFontName );//#688
/*#688
	static AFontNumber	GetDefaultFont();
	static ABool	GetFontByName( const AText& inText, AFontNumber& outFont );
	static ABool	GetFontName( const AFontNumber inFont, AText& outText );
*/
#endif
	//#375
  private:
	static AText	sDialogDefaultFontName;
	static AText	sAppDefaultFontName;
};



