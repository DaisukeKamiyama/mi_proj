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

CTextDrawData (Windows)

*/

#include "stdafx.h"

#include "CTextDrawData.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]CTextDrawData
#pragma mark ===========================
//
#pragma mark ---コンストラクタ／デストラクタ
CTextDrawData::CTextDrawData() 
{
	//B0386 改行コード位置に変な色が表示されることがある問題修正
	selectionStart = 0;
	//B0000 行折り返し計算高速化
	Init();
}

CTextDrawData::~CTextDrawData()
{
}

void	CTextDrawData::Init()
{
	UTF16DrawText.DeleteAll();
	UTF8DrawText.DeleteAll();
	OriginalTextArray_UnicodeOffset.DeleteAll();
	UTF16DrawTextArray_OriginalTextIndex.DeleteAll();
	TabLetters.DeleteAll();
	attrPos.DeleteAll();
	attrColor.DeleteAll();
	attrStyle.DeleteAll();
	drawSelection = false;
	selectionStart = kIndex_Invalid;
	selectionEnd = kIndex_Invalid;
	selectionColor = kColor_Blue;
	selectionAlpha = 0.8;
	misspellStartArray.DeleteAll();
	misspellEndArray.DeleteAll();
	startSpaceToIndentUTF16DrawTextArray_OriginalTextIndex.DeleteAll();//#117
	controlCodeStartUTF8Index.DeleteAll();//#473
	controlCodeEndUTF8Index.DeleteAll();//#473
}

#pragma mark ===========================

#pragma mark --- TextDrawData作成

//win AView_Edit作成のため、ATextInfoからコピーして作成
/**
テキスト描画用データ生成

UTF16化、タブのスペース化、制御文字の可視化等を行う。
CTextDrawDataクラスのうち、下記を設定する。
UTF16DrawText: UTF16での描画用テキスト
UTF8DrawText: UTF8での描画用テキスト
UnicodeDrawTextIndexArray: インデックス：ドキュメントが保持している元のテキストのインデックス　値：描画テキストのUnicode文字単位でのインデックス
*/
void	CTextDrawData::MakeTextDrawDataWithoutStyle( const AText& inText, const ANumber inTabWidth, const AItemCount inLetterCountLimit, 
			const ABool inGetUTF16Text, const ABool inCountAs2Letters, const ABool inDisplayControlCode,
			const AIndex inStartIndex, const AItemCount inLength,
			const ABool inStartSpaceToIndent, const AItemCount inStartSpaceCount, const AItemCount inStartIndentWidth )//#117
{
	//B0000 行折り返し計算高速化
	Init();
	
	//行情報取得
	AIndex	lineInfo_Start = inStartIndex;
	//B0000 行折り返し計算高速化
	AItemCount	lineInfo_LengthWithoutCR = inLength;
	
	AItemCount	textcount = inText.GetItemCount();//B0000
	
	//#733
	//★Convert1CharToUCS4以外の部分の高速化も検討
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	const AUChar*	textp = textptr.GetPtr();
	const AItemCount	textlen = textptr.GetByteCount();
	
	//inTextを参照して、Unicode文字１文字毎にタブスペース化、制御文字可視化を行って、UTF8DrawTextへ格納する
	//また、同時に元のテキストと、描画用Unicodeテキストとの位置対応関係を配列に格納する。
	//UTF16DrawTextArray_OriginalTextIndex: 元のテキストのインデックス(index)が値に入る配列。添字はUnicode文字の文字数(uniOffset)。マウスクリック位置からテキスト位置への変換に使われる。
	//OriginalTextArray_UnicodeOffset: Unicode文字の文字数(uniOffset)が値に入る配列。添字は元のテキストのインデックス(index)
	AIndex	uniOffset = 0;
	AIndex	tabletters = 0;//B0154
	AIndex	tabinc0800 = 1;//B0154
	if( inCountAs2Letters )   tabinc0800 = 2;//B0154
	AIndex	index = 0;
	ABool	gyoutou = true;//#117
	for( ; index < lineInfo_LengthWithoutCR;  )
	{
		if( inLetterCountLimit > 0 )
		{
			if( tabletters >= inLetterCountLimit )   break;
		}
		AUChar	ch = inText.Get(lineInfo_Start+index);
		if( ch != kUChar_Space && ch != kUChar_Tab )   gyoutou = false;//#117
		//#414
		AUCS4Char	ucs4Char = 0;
		inText.Convert1CharToUCS4(textp,textlen,lineInfo_Start+index,ucs4Char);//#733
		//
		AItemCount	bc = inText.GetUTF8ByteCount(ch);
		if( ch == kUChar_LineEnd )
		{
			break;
		}
		else if( ch == kUChar_Tab )
		{
			//タブ文字
			
			//
			OriginalTextArray_UnicodeOffset.Add(uniOffset);
			TabLetters.Add(tabletters);
			
			//Unicode一文字追加
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
			UTF8DrawText.Add(kUChar_Space);
			uniOffset++;
			
			//
			tabletters++;//B0154
			while( (tabletters%inTabWidth) != 0 )//B0154
			{
				//Unicode一文字追加
				//タブ部分の前半／後半で、対応する元のテキストの位置を変える。
				if( (tabletters%inTabWidth) > inTabWidth/2 )
				{
					UTF16DrawTextArray_OriginalTextIndex.Add(index+1);
				}
				else
				{
					UTF16DrawTextArray_OriginalTextIndex.Add(index);
				}
				UTF8DrawText.Add(kUChar_Space);
				uniOffset++;
				
				//
				tabletters++;//B0154
			}
			
			//
			index++;
		}
		else if( inDisplayControlCode == true && ch <= 0x1F )
		{
			//制御文字をHEX表示する
			
			//
			OriginalTextArray_UnicodeOffset.Add(uniOffset);
			TabLetters.Add(tabletters);
			
			//Unicode一文字追加
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
			UTF8DrawText.Add(kUChar_Space);
			uniOffset++;
			//Unicode一文字追加
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
			UTF8DrawText.Add(hex2asc((ch&0xF0)/0x10));
			uniOffset++;
			//Unicode一文字追加
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
			UTF8DrawText.Add(hex2asc(ch&0xF));
			uniOffset++;
			//Unicode一文字追加
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
			UTF8DrawText.Add(kUChar_Space);
			uniOffset++;
			
			//
			tabletters++;
			
			//#473
			controlCodeStartUTF8Index.Add(index);
			controlCodeEndUTF8Index.Add(index+bc);
			
			//
			index++;
		}
		//#473 SJISロスレスフォールバック 2バイト目文字なしエラーを赤表示する
		else if( ucs4Char >= 0x105C00 && ucs4Char <= 0x105CFF )
		{
			//エラー文字（1バイト目）取得
			AUChar	sjisch = (ucs4Char&0xFF);
			
			//オリジナルテキストに対応する配列のほうにデータ設定
			for( AIndex i = 0; i < bc; i++ )
			{
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				TabLetters.Add(tabletters);
			}
			
			//エラーテキスト取得
			AText	t;
			t.SetLocalizedText("SJIS_Error_2ndByte");
			AText	text;
			text.Add(' ');
			text.Add(hex2asc((sjisch&0xF0)/0x10));
			text.Add(hex2asc(sjisch&0xF));
			text.Add(' ');
			text.AddText(t);
			//エラーテキストを設定
			for( AIndex i = 0; i < text.GetItemCount(); i++ )
			{
				AUChar	ch = text.Get(i);
				UTF8DrawText.Add(ch);
				if( ( IsUTF8Multibyte(ch) == false ) || ( IsUTF8Multibyte1(ch) == true ) )
				{
					//Unicode一文字追加
					UTF16DrawTextArray_OriginalTextIndex.Add(index);
					uniOffset++;
				}
			}
			
			//タブ
			tabletters++;
			
			//制御文字範囲設定
			controlCodeStartUTF8Index.Add(index);
			controlCodeEndUTF8Index.Add(index+bc);
			
			//文字インデックス更新
			index += bc;
		}
		else
		{
			//#117
			if( gyoutou == true && inStartSpaceToIndent == true && inStartSpaceCount > 0 && inStartIndentWidth > 0 &&
						index+inStartSpaceCount <= lineInfo_LengthWithoutCR )
			{
				//現在のindex位置からinStartSpaceCount分spaceが連続しているかどうかをチェックする
				ABool	nspace = true;
				for( AIndex i = index ; i < index+inStartSpaceCount; i++ )
				{
					AUChar	ch = inText.Get(lineInfo_Start+i);
					if( ch != kUChar_Space )
					{
						nspace = false;
						break;
					}
				}
				//n個のspaceが存在するならば、inStartIndentWidth個のspaceに置き換えて追加
				if( nspace == true )
				{
					startSpaceToIndentUTF16DrawTextArray_OriginalTextIndex.Add(index);
					//
					for( AIndex i = 0; i < inStartSpaceCount; i++ )
					{
						//方向キーによるキャレット移動で半角スペース分キャレット位置がうつるようにする
						AIndex	uo = uniOffset + i;
						if( uo >= uniOffset+inStartIndentWidth )   uo = uniOffset+inStartIndentWidth-1;
						OriginalTextArray_UnicodeOffset.Add(uo);
						TabLetters.Add(tabletters);
						tabletters++;
					}
					//Unicode n文字追加
					for( AIndex i = 0; i < inStartIndentWidth; i++ )
					{
						//クリック時、左半分なら疑似タブ前、右半分なら疑似タブ後になるようにする
						if( i > inStartIndentWidth/2 )
						{
							UTF16DrawTextArray_OriginalTextIndex.Add(index+inStartSpaceCount);
						}
						else
						{
							UTF16DrawTextArray_OriginalTextIndex.Add(index);
						}
						UTF8DrawText.Add(kUChar_Space);
						uniOffset++;
					}
					//index移動
					index += inStartSpaceCount;
					continue;
				}
			}
			
			//それ以外の普通の文字
			
			//B0000たとえば"ﾃ"のみ記述されたファイルを読み込み、エンコーディングを誤認識した場合等、bcを足すとテキスト全体をオーバーする可能性がある
			if( lineInfo_Start+index+bc > textcount )
			{
				ch = 0x20;
				bc = 1;
			}
			
			//
			for( AIndex i = 0; i < bc; i++ )
			{
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				TabLetters.Add(tabletters);
			}
			
			//Unicode一文字追加
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
			//#412
			if( ucs4Char >= 0x10000 )
			{
				UTF16DrawTextArray_OriginalTextIndex.Add(index);
				uniOffset++;
			}
			UTF8DrawText.Add(ch);
			index++;
			for( AIndex i = 1; i < bc; i++ )
			{
				UTF8DrawText.Add(inText.Get(lineInfo_Start+index));
				index++;
			}
			uniOffset++;
			//B0000 MacOSX10.4ではU+FFFF(EF BF BF)を使うとおかしくなる？ので、U+FFFFはU+3000(全角スペース)にする
			if( ch == 0xEF )
			{
				if(	UTF8DrawText.Get(UTF8DrawText.GetItemCount()-2) == 0xBF &&
					UTF8DrawText.Get(UTF8DrawText.GetItemCount()-1) == 0xBF )
				{
					UTF8DrawText.Set(UTF8DrawText.GetItemCount()-3,0xE3);
					UTF8DrawText.Set(UTF8DrawText.GetItemCount()-2,0x80);
					UTF8DrawText.Set(UTF8DrawText.GetItemCount()-1,0x80);
				}
			}
			
			//
			//#414 if( bc <= 2 )
			if( AApplication::GetApplication().NVI_GetUnicodeData().IsWideWidth(ucs4Char) == false )//#414
			{
				tabletters++;
			}
			else
			{
				tabletters += tabinc0800;
			}
		}
	}
	UTF16DrawTextArray_OriginalTextIndex.Add(index);
	OriginalTextArray_UnicodeOffset.Add(uniOffset);
	TabLetters.Add(tabletters);
	
	if( inGetUTF16Text == true )
	{
		//UTF8DrawTextからUTF16DrawTextへ変換
		UTF16DrawText.SetText(UTF8DrawText);
		UTF16DrawText.ConvertFromUTF8ToUTF16();
		//
		while( UTF16DrawTextArray_OriginalTextIndex.GetItemCount() <= UTF16DrawText.GetItemCount()/2 )
		{
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
		}
	}
}

//R0240
void	CTextDrawData::MakeTextDrawDataWithoutStyle( const AText& inText )
{
	MakeTextDrawDataWithoutStyle(inText,4,0,true,true,false,0,inText.GetItemCount());
}

//#532
/**
追加
*/
void	CTextDrawData::AddText( const AText& inUTF8Text, const AText& inUTF16Text, const AColor& inColor )
{
	//最後のデータを取得
	AIndex	index = UTF16DrawTextArray_OriginalTextIndex.Get(UTF16DrawTextArray_OriginalTextIndex.GetItemCount()-1);
	AIndex	uniOffset = OriginalTextArray_UnicodeOffset.Get(OriginalTextArray_UnicodeOffset.GetItemCount()-1);
	AItemCount	tabletters = TabLetters.Get(TabLetters.GetItemCount()-1);
	
	//最後の１つを消す
	UTF16DrawTextArray_OriginalTextIndex.Delete1(UTF16DrawTextArray_OriginalTextIndex.GetItemCount()-1);
	OriginalTextArray_UnicodeOffset.Delete1(OriginalTextArray_UnicodeOffset.GetItemCount()-1);
	TabLetters.Delete1(TabLetters.GetItemCount()-1);
	
	//色設定
	attrPos.Add(uniOffset);
	attrColor.Add(inColor);
	attrStyle.Add(kTextStyle_Normal);
	
	//UTF8テキスト設定
	UTF8DrawText.AddText(inUTF8Text);
	//UTF16テキスト設定
	UTF16DrawText.AddText(inUTF16Text);
	
	//UTF8-UTF16 インデックス変換配列設定
	for( AIndex pos = 0; pos < inUTF8Text.GetItemCount(); )
	{
		AItemCount	bc = inUTF8Text.GetUTF8ByteCount(pos);
		for( AIndex i = 0; i < bc; i++ )
		{
			OriginalTextArray_UnicodeOffset.Add(uniOffset);
			TabLetters.Add(tabletters);
		}
		pos+= bc;
		uniOffset++;
		UTF16DrawTextArray_OriginalTextIndex.Add(index);
		index++;
		tabletters++;
	}
	OriginalTextArray_UnicodeOffset.Add(uniOffset);
	UTF16DrawTextArray_OriginalTextIndex.Add(index);
	TabLetters.Add(tabletters);
}

