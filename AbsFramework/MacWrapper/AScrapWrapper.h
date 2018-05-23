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

AScrapWrapper

*/

#pragma once

#include "../AbsBase/ABase.h"

/*#688 ABaseTypes.h‚ÖˆÚ“®
const AScrapType	kScrapType_UnicodeText = 0;//#688 'utxt';
const AScrapType	kScrapType_Text = 1;//#688 'TEXT';
const AScrapType	kScrapType_File = 2;//#688 'hfs ';
const AScrapType	kScrapType_URL = 3;//#688
*/

/**
Scrap Wrapper
*/
class AScrapWrapper
{
  public:
	static void		Init() {}//win
	
  public:
	//AScrapRefŽw’è‚ ‚è
	static ABool	ExistScrapData( const AScrapRef inScrapRef, const AScrapType inScrapType );
	static ABool	ExistTextScrapData( const AScrapRef inScrapRef );
	static void		SetScrapData( const AScrapRef inScrapRef, const AScrapType inScrapType, const AText& inText );
	static void		SetTextScrap( const AScrapRef inScrapRef, const AText& inText, const ABool inConvertToCanonicalComp );//#688 , const ATextEncoding inLegacyTextEncoding ); #1044
	//#688 static void		SetTextScrap( const AScrapRef inScrapRef, const AText& inText );//#564
	//#688 static void		GetScrapData( const AScrapRef inScrapRef, const AScrapType inScrapType, AText& outText );
	static void		GetTextScrap( const AScrapRef inScrapRef, AText& outText );//#688, const ATextEncoding inLegacyTextEncoding, const ABool inConvertToCanonicalComp );
	
	//AScrapRefŽw’è‚È‚µ
	static ABool	ExistClipboardScrapData( const AScrapType inScrapType );
	static ABool	ExistClipboardTextScrapData();
	static void		SetClipboardScrapData( const AScrapType inScrapType, const AText& inText, const ABool inClearScrap );
	//#688 static void		SetClipboardTextScrap( const AText& inText );//#688 , const ATextEncoding inLegacyTextEncoding, const ABool inClearScrap );
	static void		SetClipboardTextScrap( const AText& inText, const ABool inClearScrap, const ABool inConvertToCanonicalComp );//#564 #1044
	//#688 static void		GetClipboardScrapData( const AScrapType inScrapType, AText& outText );
	static void		GetClipboardTextScrap( AText& outText );//#688 , const ATextEncoding inLegacyTextEncoding, const ABool inConvertToCanonicalComp );
	
	//Drag
	static ABool	ExistDragData( const ADragRef inDragRef, const AScrapType inScrapType );
	static ABool	ExistTextDragData( const ADragRef inDragRef );
	static ABool	ExistURLDragData( const ADragRef inDragRef );//#358
	static void	GetDragData( const ADragRef inDragRef, const AScrapType inScrapType, AText& outText );
	static void	GetTextDragData( const ADragRef inDragRef, AText& outText );//#688 , const ATextEncoding inLegacyTextEncoding );
	static ABool	GetFileDragData( const ADragRef inDragRef, AFileAcc& outFile );
	static void		GetFileDragData( const ADragRef inDragRef, AObjectArray<AFileAcc>& outFileArray );//#384
	static ABool	GetURLDragData( const ADragRef inDragRef, AText& outURL, AText& outTitle );//#688 , const ATextEncoding inLegacyTextEncoding );
};


