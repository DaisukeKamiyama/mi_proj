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

/*#688
const AScrapType	kScrapType_UnicodeText = CF_UNICODETEXT;
const AScrapType	kScrapType_Text = CF_TEXT;
const AScrapType	kScrapType_File = CF_HDROP;
*/

class AScrapWrapper
{
	//èâä˙âª
  public:
	static void		Init();
	
  public:
	//Clipboard
	static ABool	ExistClipboardScrapData( const AScrapType inScrapType );
	static ABool	ExistClipboardTextScrapData();
	static void		SetClipboardScrapData( const AScrapType inScrapType, const AText& inText, const ABool inClearScrap );
	static void		SetClipboardTextScrap( const AText& inText, const ATextEncoding inLegacyTextEncoding, const ABool inClearScrap );
	static void		SetClipboardTextScrap( const AText& inText, const ABool inClearScrap );//#564
	static void		GetClipboardScrapData( const AScrapType inScrapType, AText& outText );
	static void		GetClipboardTextScrap( AText& outText, const ATextEncoding inLegacyTextEncoding, const ABool inConvertToCanonicalComp );
	
	//Drag
	static ABool	ExistDragData( const ADragRef inDragRef, const AScrapType inScrapType );
	static ABool	ExistTextDragData( const ADragRef inDragRef );
	static ABool	ExistURLDragData( const ADragRef inDragRef );
	static void	GetDragData( const ADragRef inDragRef, const AScrapType inScrapType, AText& outText );
	static void	GetTextDragData( const ADragRef inDragRef, AText& outText, const ATextEncoding inLegacyTextEncoding );
	static ABool	GetFileDragData( const ADragRef inDragRef, AFileAcc& outFile );
	static void		GetFileDragData( const ADragRef inDragRef, AObjectArray<AFileAcc>& outFileArray );//#384
	static ABool	GetURLDragData( const ADragRef inDragRef, AText& outURL, AText& outTitle, const ATextEncoding inLegacyTextEncoding );
	
  private:
	static UINT					sCF_INETURL;
};


