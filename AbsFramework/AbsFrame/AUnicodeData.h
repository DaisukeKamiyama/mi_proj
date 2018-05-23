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

AUnicodeData


*/

#pragma once

#include "../AbsBase/ABase.h"

//UnicodeはU+10FFFFまでだがUnicodeDataクラスとしてはU+30000までを対応
const AUCS4Char kUnicodeDataSize = 0x30000;
//
const AUInt8 kUnicodeLinkDataDecompositionSize = 4;

//B0360
enum AUnicodeCharCategory
{
	kUnicodeCharCategory_Others = 0,
	kUnicodeCharCategory_LatinAndNumber,
	kUnicodeCharCategory_ZenkakuHiragana,
	kUnicodeCharCategory_ZenkakuKatakana,
	kUnicodeCharCategory_HankakuKatakana,
	kUnicodeCharCategory_ZenkakuLatinAndNumber,
	kUnicodeCharCategory_ZenkakuOtohiki,
	kUnicodeCharCategory_Kanji,
	kUnicodeCharCategory_Greek,
	kUnicodeCharCategory_Cyrillic,
	kUnicodeCharCategory_Armenian,
	kUnicodeCharCategory_Hebrew,
	kUnicodeCharCategory_Arabic,
	kUnicodeCharCategory_Syriac,
	kUnicodeCharCategory_Thaana,
	kUnicodeCharCategory_Devanagari,
	kUnicodeCharCategory_Bengali,
	kUnicodeCharCategory_Gurmukhi,
	kUnicodeCharCategory_Gujarati,
	kUnicodeCharCategory_Oriya,
	kUnicodeCharCategory_Tamil,
	kUnicodeCharCategory_Telugu,
	kUnicodeCharCategory_Kannada,
	kUnicodeCharCategory_Malayalam,
	kUnicodeCharCategory_Sinhala,
	kUnicodeCharCategory_Thai,
	kUnicodeCharCategory_Lao,
	kUnicodeCharCategory_Tibetan,
	kUnicodeCharCategory_Myanmar,
	kUnicodeCharCategory_Georgian,
	kUnicodeCharCategory_Hangul,
	kUnicodeCharCategory_Ethiopic,
	kUnicodeCharCategory_Cherokee,
	kUnicodeCharCategory_CanadianSyllabics,
	kUnicodeCharCategory_Ogham,
	kUnicodeCharCategory_Runic,
	kUnicodeCharCategory_Tagalog,
	kUnicodeCharCategory_Hanunoo,
	kUnicodeCharCategory_Buhid,
	kUnicodeCharCategory_Tagbanwa,
	kUnicodeCharCategory_Khmer,
	kUnicodeCharCategory_Mongolian,
	kUnicodeCharCategory_Limbu,
	kUnicodeCharCategory_TaiLe,
	kUnicodeCharCategory_BraillePattern,
	kUnicodeCharCategory_Yi
	
};

class AUnicodeLinkData
{
  public:
	AUnicodeLinkData();
	~AUnicodeLinkData();
	
	AUCS4Char	GetCompatibilityDecomposition( const AUInt8 inNumber );
	void SetCompatibilityDecomposition( const AUInt8 inNumber, const AUCS4Char inChar );
	
	AUCS4Char	mUpper;
	AUCS4Char	mLower;
  private:
	AUCS4Char	mDecomposition[kUnicodeLinkDataDecompositionSize];
	AUnicodeLinkData	*mLink;
};

struct AUnicodeCharData
{
	AUInt8	combiningClass;
	AUInt8	decompositionLen;
	AUInt8	flags;//#407
	AUnicodeCharCategory	category;//B0360
	AUnicodeLinkData	*linkData;
};

//#427
enum AUnicodeNormalizeType
{
	kUnicodeNormalizeType_None = 0,
	kUnicodeNormalizeType_KD,
	kUnicodeNormalizeType_D
};

class AUnicodeData
{
  public:
	AUnicodeData();
	~AUnicodeData();
  public:
	void	Init( const AFileAcc& inUnicodeDataFile, const AFileAcc& inEastAsianWidthFile );//#415 #852
  private:
	//#852
	//初期化
	//データファイル
	AFileAcc	mUnicodeDataFile;
	AFileAcc	mEastAsianWidthFile;
	
  public:
	/*#166
	void	Cat( AArray<AUCS4Char>& ioText, const AUCS4Char inAddChar, const ABool inLower, const ABool inYenToBackslash ) const;//#412
	void	CatNFKD( AArray<AUCS4Char>& ioText, const AUCS4Char inAddChar, const ABool inLower, const ABool inYenToBackslash ) const;//#412
	*/
	ABool	CatNormalizedText( 
							  AUCS4Char*	ioTextPtr, AIndex& ioTextLen, const AItemCount inTextMaxLen,
							  const AUCS4Char inAddChar, 
							  const AUnicodeNormalizeType inNormalizeType,
							  const ABool inLower, const ABool inHiragana, const ABool inYenToBackslash ) ;//#427
	ABool	CatNormalizedText( 
							  AUCS4Char*	ioTextPtr, AIndex& ioTextLen, const AItemCount inTextMaxLen,
							  const AUCS4Char inAddChar, 
							  const AUnicodeNormalizeType inNormalizeType, //#427
							  const ABool inLower, const ABool inHiragana, const ABool inYenToBackslash,
							  const ABool inUseNormalizeArray,
							  const AHashArray<AUCS4Char>& inNormalizeArray_from, 
							  const AObjectArray< AArray<AUCS4Char> >& inNormalizeArray_to ) ;//#166
	AUnicodeCharCategory	GetUnicodeCharCategory( const AUCS4Char inChar ) ;//B0360
  private:
	void	ReadUnicodeDataTxt( const AFileAcc& inFile );
	void	ReadEastAsianWidthTxt( const AFileAcc& inFile );//#415
	//★void	NormalizeD( const AText& inText, AText& outText ) const;//#427
	//#407 UnicodeDataのGeneral Categoryを使ってアルファベット判定
  private:
	static const AUInt8	kFlags_Alphabet = 1;
	static const AUInt8	kFlags_Number = 2;
	static const AUInt8	kFlags_WideWidth = 4;
	static const AUInt8 kFlags_HasCompatible = 8;
	static const AUInt8 kFlags_CanonicalDecomp2ndChar = 16;//#863
  public:
	ABool	IsAlphabet( const AUCS4Char inChar ) 
	{
		if( inChar >= kUnicodeDataSize )   return false;
		return( (mUnicodeData[inChar].flags&kFlags_Alphabet) != 0 );
	}
	ABool	IsAlphabetOrNumber( const AUCS4Char inChar ) 
	{
		if( inChar >= kUnicodeDataSize )   return false;
		return( (mUnicodeData[inChar].flags&(kFlags_Alphabet|kFlags_Number)) != 0 );
	}
	ABool	IsAlphabetOrNumberOrUnderbar( const AUCS4Char inChar ) 
	{
		if( inChar == '_' )   return true;
		if( inChar >= kUnicodeDataSize )   return false;
		return( (mUnicodeData[inChar].flags&(kFlags_Alphabet|kFlags_Number)) != 0 );
	}
	ABool	IsWideWidth( const AUCS4Char inChar ) 
	{
		if( inChar >= kUnicodeDataSize )   return false;
		return( (mUnicodeData[inChar].flags&kFlags_WideWidth) != 0 );
	}
	//#427
	ABool	HasCompatible( const AUCS4Char inChar ) 
	{
		if( inChar >= kUnicodeDataSize )   return false;
		return( (mUnicodeData[inChar].flags&kFlags_HasCompatible) != 0 );
	}
	//#863
	ABool	IsCanonicalDecomp2ndChar( const AUCS4Char inChar )
	{
		if( inChar >= kUnicodeDataSize )   return false;
		return( (mUnicodeData[inChar].flags&kFlags_CanonicalDecomp2ndChar) != 0 );
	}
  private:
	//#407 AUnicodeCharData	mUnicodeData[kUnicodeDataSize];
	AUnicodeCharData*	mUnicodeData;//#407
	
	//#863
  public:
	AUCS4Char					GetCanonicalCompChar( const AUCS4Char in2ndChar, const AUCS4Char in1stChar ) const;
  private:
	AHashArray<AUCS4Char>					mCanonicalCompArray_Decomp2ndChar;
	AObjectArray< AHashArray<AUCS4Char> >	mCanonicalCompArray_Decomp1stCharArray;
	AObjectArray< AHashArray<AUCS4Char> >	mCanonicalCompArray_CompCharArray;
	
	//#863
  public:
	AIndex						GetPrevCharPosSkipCanonicalDecomp( const AText& inText, const AIndex inPos, const ABool inReturnMinusWhen0 = false );
	AIndex						GetNextCharPosSkipCanonicalDecomp( const AText& inText, const AIndex inPos );
	
	//#863
  public:
	AIndex						GetPrevCharPosSkipCanonicalDecomp( const AUChar* inTextPtr, const AItemCount inTextLen, const AIndex inPos, const ABool inReturnMinusWhen0 = false ) ;
	AIndex						GetNextCharPosSkipCanonicalDecomp( const AUChar* inTextPtr, const AItemCount inTextLen, const AIndex inPos );
	
};

