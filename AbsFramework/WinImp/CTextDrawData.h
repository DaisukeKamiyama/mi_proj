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

#pragma once

#include "../AbsBase/ABase.h"

#pragma mark ===========================
#pragma mark [クラス]CTextDrawData
/**
TextDrawDataクラス
色等の情報付き描画用データを格納し、Impクラスに渡すためのクラス
*/
class CTextDrawData
{
	//コンストラクタ／デストラクタ
  public:
	CTextDrawData();
	~CTextDrawData();
	
	//B0000 行折り返し計算高速化
  public:
	void	Init();
	
	//TextDrawData作成
  public:
	void	MakeTextDrawDataWithoutStyle( const AText& inText, const ANumber inTabWidth, const AItemCount inLetterCountLimit, 
			const ABool inGetUTF16Text, const ABool inCountAs2Letters, const ABool inDisplayControlCode,
			const AIndex inStartIndex, const AItemCount inLength,
			const ABool inStartSpaceToIndent = false, const AItemCount inStartSpaceCount = 0, const AItemCount inStartIndentWidth = 0 );//#117
	void	MakeTextDrawDataWithoutStyle( const AText& inText );//R0240
	void	AddText( const AText& inUTF8Text, const AText& inUTF16Text, const AColor& inColor );//#532
	
  public:
	//SPI_GetTextDrawDataWithoutStyle()で設定されるメンバ
	AText	UTF16DrawText;
	AText	UTF8DrawText;
	AArray<AIndex>	OriginalTextArray_UnicodeOffset;//インデックス：ドキュメントが保持している元のテキストのインデックス　値：描画テキストのUnicode文字単位でのインデックス
	AArray<AIndex>	UTF16DrawTextArray_OriginalTextIndex;//インデックス：描画テキストのUnicode文字単位でのインデックス　値：ドキュメントが保持している元のテキストのインデックス
	AArray<AItemCount>	TabLetters;
	//SPI_GetTextDrawDataWithStyle()で設定されるメンバ
	AArray<AIndex>	attrPos;
	AArray<AColor>	attrColor;
	AArray<ATextStyle>	attrStyle;
	AArray<AIndex>	controlCodeStartUTF8Index;//#473
	AArray<AIndex>	controlCodeEndUTF8Index;//#473
	ABool	drawSelection;
	AIndex	selectionStart;
	AIndex	selectionEnd;
	AColor	selectionColor;
	AFloatNumber	selectionAlpha;
	//R0199
	AArray<AIndex>	misspellStartArray;
	AArray<AIndex>	misspellEndArray;
	//#117
	AHashArray<AIndex>	startSpaceToIndentUTF16DrawTextArray_OriginalTextIndex;
};

