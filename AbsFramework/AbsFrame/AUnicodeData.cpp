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

#include "stdafx.h"

#include "AUnicodeData.h"

#pragma mark ===========================
#pragma mark [クラス]AUnicodeLinkData
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
AUnicodeLinkData::AUnicodeLinkData()
{
	for( short i = 0; i < kUnicodeLinkDataDecompositionSize; i++ )
	{
		mDecomposition[i] = 0;
	}
	mUpper = 0;
	mLower =0;
	mLink = NULL;
}

AUnicodeLinkData::~AUnicodeLinkData()
{
	if( mLink != NULL )
	{
		delete mLink;
	}
}

AUCS4Char	AUnicodeLinkData::GetCompatibilityDecomposition( const AUInt8 inNumber )
{
	if( inNumber >= kUnicodeLinkDataDecompositionSize )
	{
		if( mLink != NULL )
		{
			return mLink->GetCompatibilityDecomposition(inNumber-kUnicodeLinkDataDecompositionSize);
		}
		else
		{
			fprintf(stderr,"UnicodeData error.\n");
			return 0;
		}
	}
	else
	{
		return mDecomposition[inNumber];
	}
}

void AUnicodeLinkData::SetCompatibilityDecomposition( const AUInt8 inNumber, const AUCS4Char inChar )
{
	if( inNumber >= kUnicodeLinkDataDecompositionSize )
	{
		if( mLink == NULL )
		{
			mLink = new AUnicodeLinkData();
		}
		mLink->SetCompatibilityDecomposition(inNumber-kUnicodeLinkDataDecompositionSize,inChar);
	}
	else
	{
		mDecomposition[inNumber] = inChar;
	}
}

#pragma mark ===========================
#pragma mark [クラス]AUnicodeData
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
AUnicodeData::AUnicodeData():mUnicodeData(NULL)
{
	//#852 mUnicodeData = (AUnicodeCharData*)AMemoryWrapper::Malloc(sizeof(AUnicodeCharData)*kUnicodeDataSize);//#407
}

//#852
/**
データファイル設定（アプリケーション起動時にAApplicationからコールされる）
*/
void	AUnicodeData::Init( const AFileAcc& inUnicodeDataFile, const AFileAcc& inEastAsianWidthFile )
{
	mUnicodeDataFile.CopyFrom(inUnicodeDataFile);
	mEastAsianWidthFile.CopyFrom(inEastAsianWidthFile);
	
	//#852
	//メモリ確保
	mUnicodeData = (AUnicodeCharData*)AMemoryWrapper::Malloc(sizeof(AUnicodeCharData)*kUnicodeDataSize);//#407
	
	//サイズチェック
	AByteCount	bytecount = AMemoryWrapper::AllocatedSize(mUnicodeData);
	//#0 AByteCount	bytecount0 = sizeof(AUnicodeCharData);
	if( bytecount < sizeof(AUnicodeCharData)*kUnicodeDataSize )
	{
		_ACError("UnicodeData size Error",NULL);
	}
	//
	for( AUCS4Char i = 0; i < kUnicodeDataSize; i++ )
	{
		mUnicodeData[i].combiningClass = 0;
		mUnicodeData[i].decompositionLen = 0;
		mUnicodeData[i].linkData = NULL;
		mUnicodeData[i].category = kUnicodeCharCategory_Others;//B0360
		mUnicodeData[i].flags = 0;//#407
	}
	ReadUnicodeDataTxt(mUnicodeDataFile);
	ReadEastAsianWidthTxt(mEastAsianWidthFile);//#415
	//0-9 #416
	for( AUCS4Char i = 0x0030; i <= 0x0039; i++ )
	{
		mUnicodeData[i].flags |= kFlags_Number;
	}
	//B0360
	//ラテン、数字
	//0-9
	for( AUCS4Char i = 0x0030; i <= 0x0039; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_LatinAndNumber;
	}
	//A-Z
	for( AUCS4Char i = 0x0041; i <= 0x005A; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_LatinAndNumber;
	}
	//a-z
	for( AUCS4Char i = 0x0061; i <= 0x007A; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_LatinAndNumber;
	}
	//ラテン文字拡張
	for( AUCS4Char i = 0x00C0; i <= 0x02AF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_LatinAndNumber;
	}
	//ウラル発音記号拡張、ラテン文字拡張
	for( AUCS4Char i = 0x1D00; i <= 0x1EFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_LatinAndNumber;
	}
	//アルファベット表示形 
	for( AUCS4Char i = 0xFB00; i <= 0xFB4F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_LatinAndNumber;
	}
	
	
	//各種全角、半角 
	//０-９ 
	for( AUCS4Char i = 0xFF10; i <= 0xFF19; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_ZenkakuLatinAndNumber;
	}
	//Ａ-Ｚ 
	for( AUCS4Char i = 0xFF21; i <= 0xFF3A; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_ZenkakuLatinAndNumber;
	}
	//ａ-ｚ 
	for( AUCS4Char i = 0xFF41; i <= 0xFF5A; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_ZenkakuLatinAndNumber;
	}
	// 
	for( AUCS4Char i = 0x3041; i <= 0x309F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_ZenkakuHiragana;
	}
	// 
	for( AUCS4Char i = 0x30A1; i <= 0x30FA; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_ZenkakuKatakana;
	}
	// 
	for( AUCS4Char i = 0x31F0; i <= 0x31FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_ZenkakuKatakana;
	}
	//ー 
	mUnicodeData[0x30FC].category = kUnicodeCharCategory_ZenkakuOtohiki;
	//ｦ-ﾟ 
	for( AUCS4Char i = 0xFF66; i <= 0xFF9F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_HankakuKatakana;
	}
	
	//漢字 
	//々 
	mUnicodeData[0x3005].category = kUnicodeCharCategory_Kanji;
	//CJK統合漢字拡張A
	for( AUCS4Char i = 0x3400; i <= 0x4DBF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Kanji;
	}
	//CJK統合漢字 
	for( AUCS4Char i = 0x4E00; i <= 0x9FFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Kanji;
	}
	//CJK互換漢字 
	for( AUCS4Char i = 0xF900; i <= 0xFAFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Kanji;
	}
	//CJK統合漢字拡張B, CJK互換漢字追加 
	for( AUCS4Char i = 0x20000; i <= 0x2FFFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Kanji;
	}
	
	//各国文字 
	//ギリシャ文字 
	for( AUCS4Char i = 0x0370; i <= 0x03FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Greek;
	}
	for( AUCS4Char i = 0x1F00; i <= 0x1FFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Greek;
	}
	//キリル文字 
	for( AUCS4Char i = 0x0400; i <= 0x052F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Cyrillic;
	}
	//アルメニア文字 
	for( AUCS4Char i = 0x0530; i <= 0x058F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Armenian;
	}
	//ヘブライ文字 
	for( AUCS4Char i = 0x0590; i <= 0x05FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Hebrew;
	}
	//アラビア文字 
	for( AUCS4Char i = 0x0600; i <= 0x06FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Arabic;
	}
	for( AUCS4Char i = 0x0750; i <= 0x077F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Arabic;
	}
	//アラビア文字表示形A
	for( AUCS4Char i = 0xFB50; i <= 0xFDFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Arabic;
	}
	//アラビア文字表示形B
	for( AUCS4Char i = 0xFE70; i <= 0xFEFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Arabic;
	}
	//シリア文字 
	for( AUCS4Char i = 0x0700; i <= 0x074F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Syriac;
	}
	//ターナ文字 
	for( AUCS4Char i = 0x0780; i <= 0x07FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Thaana;
	}
	//デーバナーガリー文字 
	for( AUCS4Char i = 0x0900; i <= 0x097F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Devanagari;
	}
	//ベンガル文字 
	for( AUCS4Char i = 0x0980; i <= 0x09FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Bengali;
	}
	//グルムキー文字 
	for( AUCS4Char i = 0x0A00; i <= 0x0A7F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Gurmukhi;
	}
	//グジャラート文字 
	for( AUCS4Char i = 0x0A80; i <= 0x0AFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Gujarati;
	}
	//オリヤー文字 
	for( AUCS4Char i = 0x0B00; i <= 0x0B7F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Oriya;
	}
	//タミール文字 
	for( AUCS4Char i = 0x0B80; i <= 0x0BFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Tamil;
	}
	//テルグ文字 
	for( AUCS4Char i = 0x0C00; i <= 0x0C7F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Telugu;
	}
	//カンナダ文字 
	for( AUCS4Char i = 0x0C80; i <= 0x0CFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Kannada;
	}
	//マラヤーラム文字 
	for( AUCS4Char i = 0x0D00; i <= 0x0D7F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Malayalam;
	}
	//シンハラ文字 
	for( AUCS4Char i = 0x0D80; i <= 0x0DFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Sinhala;
	}
	//タイ文字 
	for( AUCS4Char i = 0x0E00; i <= 0x0E7F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Thai;
	}
	//ラオ文字 
	for( AUCS4Char i = 0x0E80; i <= 0x0EFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Lao;
	}
	//チベット文字 
	for( AUCS4Char i = 0x0F00; i <= 0x0FFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Tibetan;
	}
	//ミャンマー文字 
	for( AUCS4Char i = 0x1000; i <= 0x109F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Myanmar;
	}
	//グルジア文字 
	for( AUCS4Char i = 0x10A0; i <= 0x10FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Georgian;
	}
	//ハングル文字 
	for( AUCS4Char i = 0x1100; i <= 0x11FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Hangul;
	}
	for( AUCS4Char i = 0x3130; i <= 0x318F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Hangul;
	}
	for( AUCS4Char i = 0xAC00; i <= 0xD7FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Hangul;
	}
	//エチオピア文字 
	for( AUCS4Char i = 0x1200; i <= 0x139F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Ethiopic;
	}
	//チェロキー文字 
	for( AUCS4Char i = 0x13A0; i <= 0x13FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Cherokee;
	}
	//カナダ先住民音節  
	for( AUCS4Char i = 0x1400; i <= 0x167F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_CanadianSyllabics;
	}
	//オガム文字 
	for( AUCS4Char i = 0x1680; i <= 0x169F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Ogham;
	}
	//ルーン文字 
	for( AUCS4Char i = 0x16A0; i <= 0x16FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Runic;
	}
	//タガログ文字 
	for( AUCS4Char i = 0x1700; i <= 0x171F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Tagalog;
	}
	//ハヌノオ文字 
	for( AUCS4Char i = 0x1720; i <= 0x173F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Hanunoo;
	}
	//ブヒッド文字 
	for( AUCS4Char i = 0x1740; i <= 0x175F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Buhid;
	}
	//タグバヌワ文字 
	for( AUCS4Char i = 0x1760; i <= 0x177F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Tagbanwa;
	}
	//クメール文字 
	for( AUCS4Char i = 0x1780; i <= 0x17FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Khmer;
	}
	//モンゴル文字 
	for( AUCS4Char i = 0x1800; i <= 0x18FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Mongolian;
	}
	//リンブー文字 
	for( AUCS4Char i = 0x1900; i <= 0x194F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Limbu;
	}
	//タイナ文字 
	for( AUCS4Char i = 0x1950; i <= 0x19DF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_TaiLe;
	}
	//点字 
	for( AUCS4Char i = 0x2801; i <= 0x28FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_BraillePattern;
	}
	//イ文字 
	for( AUCS4Char i = 0xA000; i <= 0xABFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Yi;
	}
}

AUnicodeData::~AUnicodeData()
{
	//初期化済み（＝メモリ確保済み）ならメモリ解放
	if( mUnicodeData != NULL )
	{
		for( AUCS4Char i = 0; i < kUnicodeDataSize; i++ )
		{
			if( mUnicodeData[i].linkData != NULL )
			{
				delete mUnicodeData[i].linkData;
			}
		}
		//#407
		AMemoryWrapper::Free(mUnicodeData);
	}
}

//B0360
AUnicodeCharCategory	AUnicodeData::GetUnicodeCharCategory( const AUCS4Char inChar ) 
{
	if( inChar < kUnicodeDataSize )
	{
		return mUnicodeData[inChar].category;
	}
	else
	{
		return kUnicodeCharCategory_Others;
	}
}

/*#166
//
void	AUnicodeData::Cat( AArray<AUCS4Char>& ioText, const AUCS4Char inAddChar, const ABool inLower, const ABool inYenToBackslash ) const//#412
{
	AUCS4Char	addChar = inAddChar;
	if( addChar < kUnicodeDataSize )
	{
		if( inLower )
		{
			if( mUnicodeData[addChar].linkData != NULL )
			{
				if( mUnicodeData[addChar].linkData->mLower != 0 )
				{
					addChar = mUnicodeData[addChar].linkData->mLower;
				}
			}
		}
		if( inYenToBackslash )
		{
			if( addChar == 0x00A5 )
			{
				addChar = 0x005C;
			}
		}
	}
	ioText.Add(addChar);
}
*/

//ioTextに、inAddCharに対するNFKDテキスト列を追加する。 
//#166 void	AUnicodeData::CatNFKD( AArray<AUCS4Char>& ioText, const AUCS4Char inAddChar, const ABool inLower, const ABool inYenToBackslash ) const//#412

//#166
/**
正規化して追加
@return 結果バッファ（ioTextPtr）オーバーの場合はfalse
*/
ABool	AUnicodeData::CatNormalizedText( 
										AUCS4Char*	ioTextPtr, AIndex& ioTextLen, const AItemCount inTextMaxLen,
										const AUCS4Char inAddChar, 
										const AUnicodeNormalizeType inNormalizeType, //#427
										const ABool inLower, const ABool inHiragana, const ABool inYenToBackslash ) 
{
	AHashArray<AUCS4Char>	from;
	AObjectArray< AArray<AUCS4Char> >	to;
	return CatNormalizedText(ioTextPtr,ioTextLen,inTextMaxLen,inAddChar,
		inNormalizeType,inLower,inHiragana,inYenToBackslash,false,from,to);
}
ABool	AUnicodeData::CatNormalizedText( 
										AUCS4Char*	ioTextPtr, AIndex& ioTextLen, const AItemCount inTextMaxLen,
										const AUCS4Char inAddChar, 
										const AUnicodeNormalizeType inNormalizeType, //#427
										const ABool inLower, const ABool inHiragana, const ABool inYenToBackslash,
										const ABool inUseNormalizeArray,
										const AHashArray<AUCS4Char>& inNormalizeArray_from, 
										const AObjectArray< AArray<AUCS4Char> >& inNormalizeArray_to ) 
{
	//配列データによる正規化
	if( inUseNormalizeArray == true )
	{
		//まずすべてひらがなにする（正規化ファイルにはひらがなの分だけ記述する）
		AUCS4Char	fromch = inAddChar;
		if( fromch >= 0x30A1 && fromch <= 0x30F6 )
		{
			fromch -= (0x30A0-0x3040);
		}
		//配列データによる正規化実行（正規化データに対しても再度再帰的に正規化実行する。他の正規化を実行させるため。）
		AIndex	normalizeArrayIndex = inNormalizeArray_from.Find(fromch);
		if( normalizeArrayIndex != kIndex_Invalid )
		{
			const AArray<AUCS4Char>&	addText = inNormalizeArray_to.GetObjectConst(normalizeArrayIndex);
			for( AIndex i = 0; i < addText.GetItemCount(); i++ )
			{
				if( CatNormalizedText(ioTextPtr,ioTextLen,inTextMaxLen,addText.Get(i),
									  inNormalizeType,inLower,inHiragana,inYenToBackslash,inUseNormalizeArray,//#427
									  inNormalizeArray_from,inNormalizeArray_to) == false )
				{
					return false;
				}
			}
			return true;
		}
	}
	//
	if( inAddChar < kUnicodeDataSize )
	{
		//
		ABool	needDecomp = false;
		if( inNormalizeType == kUnicodeNormalizeType_KD && mUnicodeData[inAddChar].decompositionLen > 0 )
		{
			needDecomp = true;
		}
		if( inNormalizeType == kUnicodeNormalizeType_D && mUnicodeData[inAddChar].decompositionLen > 0 &&
					((mUnicodeData[inAddChar].flags&kFlags_HasCompatible) == 0 ))
		{
			needDecomp = true;
		}
		//
		if( /*#427 inNFKD == true && mUnicodeData[inAddChar].decompositionLen > 0*/ needDecomp == true )//#166
		{
			//compatible/decompへ変換（decomp:が→か＋濁点）
			for( AUInt8 i = 0; i < mUnicodeData[inAddChar].decompositionLen; i++ )
			{
				//#166 CatNFKD(ioText,mUnicodeData[inAddChar].linkData->GetCompatibilityDecomposition(i),inLower,inYenToBackslash);
				if( CatNormalizedText(ioTextPtr,ioTextLen,inTextMaxLen,
									  mUnicodeData[inAddChar].linkData->GetCompatibilityDecomposition(i),
									  inNormalizeType,inLower,inHiragana,inYenToBackslash,inUseNormalizeArray,//#427
									  inNormalizeArray_from,inNormalizeArray_to) == false )
				{
					return false;
				}
			}
		}
		else
		{
			AUCS4Char	addChar = inAddChar;
			//小文字へ正規化
			if( inLower )
			{
				if( mUnicodeData[addChar].linkData != NULL )
				{
					if( mUnicodeData[addChar].linkData->mLower != 0 )
					{
						addChar = mUnicodeData[addChar].linkData->mLower;
					}
				}
			}
			//ひらがなへ正規化 #166
			if( inHiragana == true )
			{
				if( addChar >= 0x30A1 && addChar <= 0x30F6 )
				{
					addChar -= (0x30A0-0x3040);
				}
			}
			//半角円記号からバックスラッシュへ正規化
			if( inYenToBackslash )
			{
				if( addChar == 0x00A5 )
				{
					addChar = 0x005C;
				}
			}
			//正規化後文字追加
			//#733 ioText.Add(addChar);
			if( ioTextLen >= inTextMaxLen )
			{
				//バッファオーバー
				//_ACThrow("",NULL);
				return false;
			}
			else
			{
				ioTextPtr[ioTextLen] = addChar;
				ioTextLen++;
			}
			
			//reorder algorithm
			if( /*#427 inNFKD == true*/ inNormalizeType == kUnicodeNormalizeType_KD || inNormalizeType == kUnicodeNormalizeType_D )//#166
			{
				AUInt8	addCharClass = mUnicodeData[addChar].combiningClass;
				if( addCharClass > 0 )
				{
					for( AIndex i = /*#733 ioText.GetItemCount()-1*/ioTextLen-1; i > 0; i-- )
					{
						AUInt8	prevCharClass = 0;//#407
						if( /*#733 ioText.Get(i-1)*/ioTextPtr[i-1] < kUnicodeDataSize )//#407
						{
							prevCharClass = mUnicodeData[/*#733 ioText.Get(i-1)*/ioTextPtr[i-1]].combiningClass;
						}
						if( prevCharClass <= addCharClass )
						{
							break;
						}
						//#733 ioText.Set(i,ioText.Get(i-1));
						ioTextPtr[i] = ioTextPtr[i-1];//#733
						//#733 ioText.Set(i-1,addChar);
						ioTextPtr[i-1] = addChar;//#733
					}
				}
			}
		}
	}
	else
	{
		//テーブル外のUnicode文字の場合は変換せずに単純に追加
		//#733 ioText.Add(inAddChar);
		if( ioTextLen >= inTextMaxLen )
		{
			//バッファオーバー
			//_ACThrow("",NULL);
			return false;
		}
		else
		{
			ioTextPtr[ioTextLen] = inAddChar;
			ioTextLen++;
		}
	}
	return true;
}

void	AUnicodeData::ReadUnicodeDataTxt( const AFileAcc& inFile )
{
	try
	{
		AText	text;
		inFile.ReadTo(text);
		//
		AStTextPtr	textptr(text,0,text.GetItemCount());
		const AUChar*	ptr = textptr.GetPtr();
		const AItemCount	textlen = textptr.GetByteCount();
		//
		AIndex	pos = 0;
		while( pos < textlen )
		{
			AUCS4Char	unicode = 0;
			AText	str;
			AIndex	p = pos;
			
			//行の終了位置検索（pの連続でも良いが、念のため、位置補正する。） 
			while( pos < textlen )
			{
				unsigned char	ch = ptr[pos];
				pos++;
				if( ch == '\r' || ch == '\n' )
				{
					if( pos < textlen )
					{
						ch = ptr[pos];
						if( ch == '\r' || ch == '\n' )
						{
							pos++;
						}
					}
					break;
				}
			}
			
			//unicode取得 
			AText::GetToken(ptr,textlen,p,str);
			unicode = str.GetIntegerFromHexText();
			if( unicode >= kUnicodeDataSize )
			{
				continue;
			}
			p++;
			
			//Name読み飛ばし 
			while( ptr[p] != ';' )   p++;
			p++;
			//General_Category
			AText	t;
			while( ptr[p] != ';' )
			{
				t.Add(ptr[p]);
				p++;
			}
			//アルファベット判定
			if( t.Compare("Lu") == true || t.Compare("Ll") == true || t.Compare("Lt") == true )
			{
				//http://www.unicode.org/reports/tr44/#General_Category_Values
				//Lu: an uppercase letter
				//Ll: a lowercase letter
				//Lt: a digraphic character, with first part uppercase
				//Lm: a modifier letter （濁音記号等はここ）
				//Lo: other letters, including syllables and ideographs （漢字・ひらがな等はここ）
				mUnicodeData[unicode].flags |= kFlags_Alphabet;
			}
			p++;
			//Canonical_Combining_Class
			AText::GetToken(ptr,textlen,p,str);
			mUnicodeData[unicode].combiningClass = str.GetIntegerFromHexText();
			p++;
			//Bidi_Class読み飛ばし 
			while( ptr[p] != ';' )   p++;
			p++;
			//------------------Decomposition_Mapping------------------
			//Compatible Decompかどうかの判定（compatibleの場合、最初に<>で囲まれた部分がある。（<fraction>等））
			if( ptr[p] == '<' )
			{
				while( ptr[p] != '>' )   p++;
				p++;
				mUnicodeData[unicode].flags |= kFlags_HasCompatible;//#427
			}
			//decomp文字ごとのループ（decomp文字が連なって記述されているのでそれを読み込む）
			AItemCount	decompositionLen = 0;
			AUCS4Char	firstDecompChar = 0;//#863 最初のdecomp文字を記憶 canonical decomp→comp変換用のデータに使用
			AUCS4Char	lastDecompChar = 0;//#863 最後のdecomp文字を記憶 canonical decomp→comp変換用のデータに使用
			for( ; decompositionLen < 256; decompositionLen++)
			{
				if( ptr[p] == ';' )    break;
				AText::GetToken(ptr,textlen,p,str);
				if( mUnicodeData[unicode].linkData == NULL )
				{
					mUnicodeData[unicode].linkData = new AUnicodeLinkData();
				}
				//decomp文字を取得 #863
				lastDecompChar = str.GetIntegerFromHexText();
				if( decompositionLen == 0 )
				{
					//最初のdecomp文字を記憶
					firstDecompChar = lastDecompChar;
				}
				//link dataにdecompを追加
				mUnicodeData[unicode].linkData->SetCompatibilityDecomposition(decompositionLen,lastDecompChar);
			}
			//decompレングス設定
			mUnicodeData[unicode].decompositionLen = decompositionLen;
			//------------------canonical decomp→comp変換用のデータを作成する------------------
			if( (mUnicodeData[unicode].flags&kFlags_HasCompatible) == 0 && 
						decompositionLen >= 2 &&
						lastDecompChar < kUnicodeDataSize )
			{
				//canonical compの場合、最後のdecomp文字のkFlags_CanonicalDecomp2ndCharフラグをONにする
				mUnicodeData[lastDecompChar].flags |= kFlags_CanonicalDecomp2ndChar;
				//mCanonicalCompArray_Decomp2ndChar, mCanonicalCompArray_Decomp1stCharArray, mCanonicalCompArray_CompCharArrayに
				//最後のdecomp文字毎に、{decomp一文字目,comp文字}のリストを格納する
				AIndex	index = mCanonicalCompArray_Decomp2ndChar.Find(lastDecompChar);
				if( index == kIndex_Invalid )
				{
					index = mCanonicalCompArray_Decomp2ndChar.Add(lastDecompChar);
					mCanonicalCompArray_Decomp1stCharArray.AddNewObject();
					mCanonicalCompArray_CompCharArray.AddNewObject();
				}
				mCanonicalCompArray_Decomp1stCharArray.GetObject(index).Add(firstDecompChar);
				mCanonicalCompArray_CompCharArray.GetObject(index).Add(unicode);
			}
			//decompositionが256以上ある場合はエラー（規格上、18文字が最長）
			if( ptr[p] != ';' )
			{
				mUnicodeData[unicode].decompositionLen = 0;
				if( mUnicodeData[unicode].linkData != NULL )
				{
					delete mUnicodeData[unicode].linkData;
					mUnicodeData[unicode].linkData = NULL;
				}
				fprintf(stderr,"The length of the compatibility decomposition is more than %d characters for U+%06X.\n",256,unicode);
			}
			p++;
			//Numeric_Value読み飛ばし 
			while( ptr[p] != ';' )   p++;
			p++;
			//Numeric_Value読み飛ばし 
			while( ptr[p] != ';' )   p++;
			p++;
			//Numeric_Value読み飛ばし 
			while( ptr[p] != ';' )   p++;
			p++;
			//Bidi_Mirrored読み飛ばし 
			while( ptr[p] != ';' )   p++;
			p++;
			//Unicode_1_Name読み飛ばし 
			while( ptr[p] != ';' )   p++;
			p++;
			//ISO_Comment読み飛ばし 
			while( ptr[p] != ';' )   p++;
			p++;
			//Simple_Uppercase_Mapping
			if( ptr[p] != ';' )
			{
				AText::GetToken(ptr,textlen,p,str);
				if( mUnicodeData[unicode].linkData == NULL )
				{
					mUnicodeData[unicode].linkData = new AUnicodeLinkData();
				}
				mUnicodeData[unicode].linkData->mUpper = str.GetIntegerFromHexText();
			}
			p++;
			//Simple_Lowercase_Mapping
			if( ptr[p] != ';' )
			{
				AText::GetToken(ptr,textlen,p,str);
				if( mUnicodeData[unicode].linkData == NULL )
				{
					mUnicodeData[unicode].linkData = new AUnicodeLinkData();
				}
				mUnicodeData[unicode].linkData->mLower = str.GetIntegerFromHexText();
			}
			p++;
		}
	}
	catch(...)
	{
	}
}

//#415
void	AUnicodeData::ReadEastAsianWidthTxt( const AFileAcc& inFile )
{
	try
	{
		AText	text;
		inFile.ReadTo(text);
		//
		AStTextPtr	textptr(text,0,text.GetItemCount());
		const AUChar*	ptr = textptr.GetPtr();
		const AItemCount	textlen = textptr.GetByteCount();
		//
		AIndex	pos = 0;
		while( pos < textlen )
		{
			AUCS4Char	unicode_s = 0, unicode_e = 0;//#430
			AText	str;
			AIndex	p = pos;
			
			//行の終了位置検索
			while( pos < textlen )
			{
				unsigned char	ch = ptr[pos];
				pos++;
				if( ch == '\r' || ch == '\n' )
				{
					if( pos < textlen )
					{
						ch = ptr[pos];
						if( ch == '\r' || ch == '\n' )
						{
							pos++;
						}
					}
					break;
				}
			}
			
			//unicode取得 
			AText::GetToken(ptr,textlen,p,str);
			if( str.Compare("#") == true )   continue;
			unicode_s = str.GetIntegerFromHexText();
			if( unicode_s >= kUnicodeDataSize )
			{
				continue;
			}
			//#430 p++;
			//#430 範囲指定対応
			AText::GetToken(ptr,textlen,p,str);
			if( str.Compare(".") == true )
			{
				AText::GetToken(ptr,textlen,p,str);//もう１つ"."読み込み
				AText::GetToken(ptr,textlen,p,str);
				unicode_e = str.GetIntegerFromHexText();
				if( unicode_e >= kUnicodeDataSize )
				{
					unicode_e = kUnicodeDataSize-1;
				}
				AText::GetToken(ptr,textlen,p,str);//";"
			}
			else
			{
				unicode_e = unicode_s;
			}
			
			//F/H/W/Na/A読み込み
			AText::GetToken(ptr,textlen,p,str);
			if( str.GetItemCount() == 1 )
			{
				AUChar	ch = str.Get(0);
				if( ch == 'F' || ch == 'W' || ch == 'A' )//#561
				{
					for( AUCS4Char uni = unicode_s; uni <= unicode_e; uni++ )//#430
					{
						mUnicodeData[uni].flags |= kFlags_WideWidth;
					}
				}
			}
		}
	}
	catch(...)
	{
	}
}

//#863
/**
decomp文字からcomp文字を取得（canonical compのみ対応）
*/
AUCS4Char	AUnicodeData::GetCanonicalCompChar( const AUCS4Char in2ndChar, const AUCS4Char in1stChar ) const
{
	//2番目decoomp文字から、テーブル検索
	AIndex	comparrayindex = mCanonicalCompArray_Decomp2ndChar.Find(in2ndChar);
	if( comparrayindex == kIndex_Invalid )
	{
		_ACError("",NULL);
		return 0;
	}
	
	//1番目decomp文字を検索
	AIndex	index = mCanonicalCompArray_Decomp1stCharArray.GetObjectConst(comparrayindex).Find(in1stChar);
	if( index == kIndex_Invalid )
	{
		return 0;
	}
	else
	{
		//対応するcomp文字を返す
		return mCanonicalCompArray_CompCharArray.GetObjectConst(comparrayindex).Get(index);
	}
}

//#427
/**
UTF-8テキスト正規化(NormalizeD)
*/
/*
//★
void	AUnicodeData::NormalizeD( const AText& inText, AText& outText ) const
{
	outText.DeleteAll();
	//inTextから一文字ずつ読んでNormalizeDし、textへ格納
	AArray<AUCS4Char>	text;
	AItemCount	len = inText.GetItemCount();
	for( AIndex pos = 0; pos < len; )
	{
		AUCS4Char	ch = 0;
		pos += inText.Convert1CharToUCS4(pos,ch);
		CatNormalizedText(text,ch,kUnicodeNormalizeType_D,false,false,false);
	}
	//UCS4->UTF8
	AItemCount	ucs4textlen = text.GetItemCount();
	for( AIndex i = 0; i  < ucs4textlen; i++ )
	{
		outText.AddFromUCS4Char(text.Get(i));
	}
}
*/

//#863
/**
前の文字位置を取得（canonical decomp文字はスキップ）
*/
AIndex	AUnicodeData::GetPrevCharPosSkipCanonicalDecomp( const AText& inText, const AIndex inPos, const ABool inReturnMinusWhen0 ) 
{
	AIndex	pos = inText.GetPrevCharPos(inPos,inReturnMinusWhen0);
	if( pos < inPos )
	{
		//inPosの１つ前の文字を取得
		AUCS4Char	ucs4char = 0;
		inText.Convert1CharToUCS4(pos,ucs4char);
		//inPosの１つ前の文字がdecomp2文字目かどうかを判定
		if( IsCanonicalDecomp2ndChar(ucs4char) == true )
		{
			//------------------inPosの１つ前の文字がdecomp2文字目の場合------------------
			//1つ前の文字の位置（＝decomp1文字目）を取得
			AIndex	prevpos = inText.GetPrevCharPos(pos,inReturnMinusWhen0);
			if( prevpos < pos )
			{
				//decompの前の文字を取得
				AUCS4Char	prevUcs4char = 0;
				inText.Convert1CharToUCS4(prevpos,prevUcs4char);
				//comp文字が取得できるかどうかを試す
				AUCS4Char	compchar = GetCanonicalCompChar(ucs4char,prevUcs4char);
				if( compchar != 0 )
				{
					//comp文字有りなら、decomp１文字目の位置を返り値にする
					pos = prevpos;
				}
			}
		}
	}
	return pos;
}
AIndex	AUnicodeData::GetPrevCharPosSkipCanonicalDecomp( const AUChar* inTextPtr, const AItemCount inTextLen, const AIndex inPos, const ABool inReturnMinusWhen0 ) 
{
	AIndex	pos = AText::GetPrevCharPos(inTextPtr,inTextLen,inPos,inReturnMinusWhen0);
	if( pos < inPos )
	{
		//inPosの１つ前の文字を取得
		AUCS4Char	ucs4char = 0;
		AText::Convert1CharToUCS4(inTextPtr,inTextLen,pos,ucs4char);
		//inPosの１つ前の文字がdecomp2文字目かどうかを判定
		if( IsCanonicalDecomp2ndChar(ucs4char) == true )
		{
			//------------------inPosの１つ前の文字がdecomp2文字目の場合------------------
			//1つ前の文字の位置（＝decomp1文字目）を取得
			AIndex	prevpos = AText::GetPrevCharPos(inTextPtr,inTextLen,pos,inReturnMinusWhen0);
			if( prevpos < pos )
			{
				//decompの前の文字を取得
				AUCS4Char	prevUcs4char = 0;
				AText::Convert1CharToUCS4(inTextPtr,inTextLen,prevpos,prevUcs4char);
				//comp文字が取得できるかどうかを試す
				AUCS4Char	compchar = GetCanonicalCompChar(ucs4char,prevUcs4char);
				if( compchar != 0 )
				{
					//comp文字有りなら、decomp１文字目の位置を返り値にする
					pos = prevpos;
				}
			}
		}
	}
	return pos;
}

//#863
/**
次の文字位置を取得（canonical decomp文字はスキップ）
*/
AIndex	AUnicodeData::GetNextCharPosSkipCanonicalDecomp( const AText& inText, const AIndex inPos ) 
{
	AIndex	pos = inText.GetNextCharPos(inPos);
	if( pos > inPos && pos < inText.GetItemCount() )
	{
		//posの位置の文字（＝inPosの位置の文字の次の文字）を取得
		AUCS4Char	ucs4char = 0;
		inText.Convert1CharToUCS4(pos,ucs4char);
		//posの位置の文字がdecomp2文字目かどうかを判定
		if( IsCanonicalDecomp2ndChar(ucs4char) == true )
		{
			//inPosの位置の文字を取得
			AUCS4Char	currentUcs4char = 0;
			inText.Convert1CharToUCS4(inPos,currentUcs4char);
			//comp文字が取得できるかどうかを試す
			AUCS4Char	compchar = GetCanonicalCompChar(ucs4char,currentUcs4char);
			if( compchar != 0 )
			{
				//comp文字有りなら、posの次の位置を返り値にする
				pos = inText.GetNextCharPos(pos);
			}
		}
	}
	return pos;
}
AIndex	AUnicodeData::GetNextCharPosSkipCanonicalDecomp( const AUChar* inTextPtr, const AItemCount inTextLen, const AIndex inPos ) 
{
	AIndex	pos = AText::GetNextCharPos(inTextPtr,inTextLen,inPos);
	if( pos > inPos && pos < inTextLen )
	{
		//posの位置の文字（＝inPosの位置の文字の次の文字）を取得
		AUCS4Char	ucs4char = 0;
		AText::Convert1CharToUCS4(inTextPtr,inTextLen,pos,ucs4char);
		//posの位置の文字がdecomp2文字目かどうかを判定
		if( IsCanonicalDecomp2ndChar(ucs4char) == true )
		{
			//inPosの位置の文字を取得
			AUCS4Char	currentUcs4char = 0;
			AText::Convert1CharToUCS4(inTextPtr,inTextLen,inPos,currentUcs4char);
			//comp文字が取得できるかどうかを試す
			AUCS4Char	compchar = GetCanonicalCompChar(ucs4char,currentUcs4char);
			if( compchar != 0 )
			{
				//comp文字有りなら、posの次の位置を返り値にする
				pos = AText::GetNextCharPos(inTextPtr,inTextLen,pos);
			}
		}
	}
	return pos;
}


