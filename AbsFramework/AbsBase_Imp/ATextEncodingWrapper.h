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

ATextEncodingWrapper

*/

#pragma once

#include "../AbsBase/ABase.h"
/**
テキストエンコーディングImpクラス
*/
class ATextEncodingWrapper
{
  public:
	static void	Init();
	
	//TextEncoding変換
  public:
	static ABool	ConvertToUTF8( AText& ioText, const ATextEncoding inSrcTextEncoding, const ABool inUseFallback, 
					ABool& outFallbackUsed, const ABool inFromExternalData );//B0400
	static ABool	ConvertFromUTF8( AText& ioText, const ATextEncoding inDstTextEncoding,
									 const ABool inConvertToCanonical = false );//#1044
	static void		ConvertToCanonicalComp( AText& ioUTF16Text );//#1040
	static ABool	UTF16IsLE();
#if IMPLEMENTATION_FOR_MACOSX
	static void		ConvertUTF16ToHFSPlusDecomp( AText& ioUTF16Text );
#endif
	static ABool	IsSJISSeriesTextEncoding( const ATextEncoding inTextEncoding );//#1044
	
	//TextEncoding独自変換モジュール部分
  private:
	static void	ChangeByteOrder( AText& ioText );
	static void	ConvertFromJISToSJIS( AText& ioText );
	static void	ConvertFromEUCToSJIS( AText& ioText );
	static void	ConvertFromSJISToJIS( const AText& inText, AText& outBuffer );
	static void	ConvertFromSJISToEUC( const AText& inText, AText& outBuffer );
	static ABool	IsSJISKanjiChar( const AUChar c );
	static void		ConvertCFMutableStringChar( CFMutableStringRef ioString ,const AUCS4Char inFromChar, const AUCS4Char inToChar );
	
	//TextEncoding取得
  public:
	static ATextEncoding	GetTextEncodingFromName( const AText& inName );
	static ABool	GetTextEncodingFromName( AConstCharPtr inName, ATextEncoding& outEncoding );
	static ABool	GetTextEncodingFromName( const AText& inName, ATextEncoding& outEncoding );
	static ABool	GetTextEncodingName( const ATextEncoding& inEncoding, AText& outName );
	static ABool	CheckTextEncodingAvailability( const AText& inName );
	static void 	ConvertToCarbonTextEncodingName( AText& ioText );//#1040
	static ABool	GetTextEncodingFromIANAName( const AText& inName, ATextEncoding& outEncoding );//#1040
	static ABool	GetTextEncodingIANAName( const ATextEncoding& inEncoding, AText& outName );//#1040
	
	static ATextEncoding	GetJISTextEncoding();
	static ATextEncoding	GetEUCTextEncoding();
	static ATextEncoding	GetSJISTextEncoding();
	static ATextEncoding	GetSJISX0213TextEncoding();
	static ATextEncoding	GetUTF8TextEncoding();
	static ATextEncoding	GetUTF16TextEncoding();
	//#1040 static ATextEncoding	GetUTF16CanonicalCompTextEncoding();//#1044
	static ATextEncoding	GetUTF16LETextEncoding();
	static ATextEncoding	GetUTF16BETextEncoding();//B0377
	static ATextEncoding	GetOSDefaultTextEncoding();//#427
	//#688 static ATextEncoding	GetLegacyTextEncodingFromFont( /*win const AFont inFont*/const AText& inFontName );
	//#688 static ATextEncoding	GetLegacyTextEncodingFromLanguage( const ALanguage inLanguage );//#189
	
	static void				CheckJISX0208( const ATextEncoding inTextEncoding, const AText& inText, ABool& outKishuIzon, ABool& outHankakuKana );
	
	//TextEncoding変換用内部モジュール
	static ABool	TECConvertToUTF16( const AText& inText, AText& outText, const ATextEncoding inSrc, ABool inUseFallback, ABool &outFallbackUsed );
	static ABool	TECConvertFromUTF16( const AText& inText, AText& outText, const TextEncoding inDst );
	
private:
	static ABool	TECConvertUTFx( const AText& inText, AText& outText, const ATextEncoding inSrc, const ATextEncoding inDst );
	static void	jis2sjis( unsigned char &a, unsigned char &b );
	static void	euc2sjis( unsigned char &a, unsigned char &b );
	static void	sjis2jis( unsigned char &a, unsigned char &b );
	static void	sjis2euc( unsigned char &a, unsigned char &b );
	
#if IMPLEMENTATION_FOR_MACOSX
  private:
	static ATextEncoding	sUTF16TextEncoding;
	//#1040 static ATextEncoding	sUTF16CanonicalCompTextEncoding;//#1044
	static ATextEncoding	sUTF8TextEncoding;
	static ATextEncoding	sJISTextEncoding;
	static ATextEncoding	sEUCTextEncoding;
	static ATextEncoding	sSJISTextEncoding;
	static ATextEncoding	sSJISX0213TextEncoding;
	static ATextEncoding	sUTF16LETextEncoding;
	static ATextEncoding	sUTF16BETextEncoding;//B0377
	
	static TECObjectRef	GetConverter( const ATextEncoding& inSrc, const ATextEncoding& inDst );
	static TECObjectRef	sConverterFromUTF8ToUTF16;
	static TECObjectRef	sConverterFromUTF16ToUTF8;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
  private:
	static void	RegisterTextEncodingName( AConstCharPtr inName, const ATextEncoding inCodePage );
	static AHashTextArray	sTextEncodingArray_Name;
	static AArray<ATextEncoding>	sTextEncodingArray_CodePage;
#endif
	
	//#369 携帯絵文字変換テーブル
  private:
	static AHashNumberArray			sEmojiConvertTable_SJIS0213;
	static AHashNumberArray			sEmojiConvertTable_Emoji;
	
};



