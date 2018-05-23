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

ADocument_EditBox

*/

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]ADocument_EditBox
#pragma mark ===========================
//テキストドキュメントのクラス
#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
ADocument_EditBox::ADocument_EditBox( AObjectArrayItem* inParent ) : ADocument(inParent,kObjectID_Invalid), mUndoer(*this)
{
	//最初に行情報を生成する #137
	CalcLineStarts();
}

/**
デストラクタ
*/
ADocument_EditBox::~ADocument_EditBox()
{
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---テキスト編集

/**
テキスト挿入
*/
void	ADocument_EditBox::SPI_InsertText( const ATextIndex inInsertIndex, const AText& inText )
{
	//Undo記録
	mUndoer.RecordInsertText(inInsertIndex,inText.GetItemCount());
	
	//テキスト挿入
	mText.InsertText(inInsertIndex,inText);
	
	//行計算
	CalcLineStarts();
}

/**
テキスト挿入
*/
void	ADocument_EditBox::SPI_InsertText( const ATextPoint& inInsertPoint, const AText& inText )
{
	SPI_InsertText(SPI_GetTextIndexFromTextPoint(inInsertPoint),inText);
}

/**
テキスト削除
*/
void	ADocument_EditBox::SPI_DeleteText( const ATextIndex inStart, const ATextIndex inEnd )
{
	//Undo記録
	mUndoer.RecordDeleteText(inStart,inEnd);
	
	//テキスト削除
	mText.Delete(inStart,inEnd-inStart);
	
	//行計算
	CalcLineStarts();
}

/**
テキスト削除
*/
void	ADocument_EditBox::SPI_DeleteText( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint )
{
	SPI_DeleteText(SPI_GetTextIndexFromTextPoint(inStartPoint),SPI_GetTextIndexFromTextPoint(inEndPoint));
}

#pragma mark ===========================

#pragma mark ---テキストに対する各種操作

/**
Undo
*/
void	ADocument_EditBox::SPI_Undo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	mUndoer.Undo(outSelectTextIndex,outSelectTextLength);
}

/**
Redo
*/
void	ADocument_EditBox::SPI_Redo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	mUndoer.Redo(outSelectTextIndex,outSelectTextLength);
}

/**
Undoメニュー用文字列取得

@param outText メニュー用文字列(out)
@return Undo可否
*/
ABool	ADocument_EditBox::SPI_GetUndoText( AText& outText ) const
{
	return mUndoer.GetUndoMenuText(outText);
}

/**
Redoメニュー用文字列取得

@param outText メニュー用文字列(out)
@return Redo可否
*/
ABool	ADocument_EditBox::SPI_GetRedoText( AText& outText ) const
{
	return mUndoer.GetRedoMenuText(outText);
}

/**
UndoActionTagを設定
*/
void	ADocument_EditBox::SPI_RecordUndoActionTag( const AEditBoxUndoTag inTag )
{
	mUndoer.RecordActionTag(inTag);
}

//#135
/**
Undo情報をクリア
*/
void	ADocument_EditBox::SPI_ClearUndoInfo()
{
	mUndoer.Init();
}

#pragma mark ===========================

#pragma mark ---テキスト取得

/**
テキスト取得（inStart>inEndでも可）
*/
void	ADocument_EditBox::SPI_GetText( const ATextIndex inStart, const ATextIndex inEnd, AText& outText ) const
{
	ATextIndex	start = inStart;
	ATextIndex	end = inEnd;
	if( inEnd < inStart )
	{
		start = inEnd;
		end = inStart;
	}
	mText.GetText(start,end-start,outText);
}
/**
テキスト取得（inStart>inEndでも可）
*/
void	ADocument_EditBox::SPI_GetText( const ATextPoint& inStart, const ATextPoint& inEnd, AText& outText ) const
{
	SPI_GetText(SPI_GetTextIndexFromTextPoint(inStart),SPI_GetTextIndexFromTextPoint(inEnd),outText);
}
/**
テキスト取得（全テキスト）
*/
void	ADocument_EditBox::SPI_GetText( AText& outText ) const
{
	SPI_GetText(0,mText.GetItemCount(),outText);
}

/**
テキスト描画データ取得
*/
void	ADocument_EditBox::SPI_GetTextDrawData( const AIndex inLineIndex, CTextDrawData& outTextDrawData ) const
{
	AText	linetext;
	AIndex	pos = GetLineStart(inLineIndex);
	mText.GetLine(pos,linetext);
	outTextDrawData.MakeTextDrawDataWithoutStyle(linetext,4,0,true,true,false,0,linetext.GetItemCount());
}

/**
指定Indexの1つ前の文字の位置を取得
*/
AIndex	ADocument_EditBox::SPI_GetPrevCharTextIndex( const ATextIndex inIndex ) const
{
	//#863 return mText.GetPrevCharPos(inIndex);
	return AApplication::GetApplication().NVI_GetUnicodeData().GetPrevCharPosSkipCanonicalDecomp(mText,inIndex);//#863
}

/**
指定Indexの1つ次の文字の位置を取得
*/
AIndex	ADocument_EditBox::SPI_GetNextCharTextIndex( const ATextIndex inIndex ) const
{
	//#863 return mText.GetNextCharPos(inIndex);
	return AApplication::GetApplication().NVI_GetUnicodeData().GetNextCharPosSkipCanonicalDecomp(mText,inIndex);//#863
}

/**
指定Indexの現在の文字の位置を取得（マルチバイトの1バイト目の位置（そのバイトの直前の位置）を取得）
*/
AIndex	ADocument_EditBox::SPI_GetCurrentCharTextIndex( const ATextIndex inIndex ) const
{
	return mText.GetCurrentCharPos(inIndex);
}

/**
単語範囲を取得
*/
void	ADocument_EditBox::SPI_FindWord( const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd ) const
{
	ABool	multibyteStart = false;
	if( inStartPos == SPI_GetTextLength() )//B0125
	{
		multibyteStart = false;
	}
	else
	{
		multibyteStart = mText.IsUTF8MultiByte(inStartPos);
	}
	if( multibyteStart == false ) //その他<test
	{
		ABool	isAlphabet = false;
		if( inStartPos < SPI_GetTextLength() )
		{
			isAlphabet = SPI_IsAsciiAlphabet(inStartPos);
		}
		if( isAlphabet == false ) 
		{
			AUChar	ch = 0;
			if( inStartPos < SPI_GetTextLength() )
			{
				ch = mText.Get(inStartPos);
			}
			if( ch == kUChar_Space || ch == kUChar_Tab ) 
			{
				AIndex	pos;
				for( pos = inStartPos-1; pos >= 0; pos-- ) 
				{
					AUChar	ch = mText.Get(pos);
					if( ch != kUChar_Space && ch != kUChar_Tab ) 
					{
						break;
					}
				}
				outStart = pos+1;
				for( pos = inStartPos+1; pos < SPI_GetTextLength(); pos++ ) 
				{
					AUChar	ch = mText.Get(pos);
					if( ch != kUChar_Space && ch != kUChar_Tab ) 
					{
						break;
					}
				}
				outEnd = pos;
			}
			else 
			{
				outStart = inStartPos;
				outEnd = inStartPos+1;
				if( outEnd > SPI_GetTextLength() )   outEnd = SPI_GetTextLength();//B0125
			}
		}
		else 
		{
			AIndex	pos;
			for( pos = inStartPos; pos >= 0; pos-- ) 
			{
				isAlphabet = SPI_IsAsciiAlphabet(pos);
				if( isAlphabet == false )   break;
				if( pos >= 1 ) 
				{
					if( mText.IsUTF8FirstByteInMultiByte(pos-1) == true )
					{
						break;
					}
				}
			}
			pos++;
			outStart = pos;
			for( pos = inStartPos+1; pos < SPI_GetTextLength(); pos++ ) 
			{
				isAlphabet = SPI_IsAsciiAlphabet(pos);
				if( isAlphabet == false )   break;
			}
			outEnd = pos;
		}
	}
	else 
	{
		ABool	bunsetsuSelect = false;
		AIndex	startPos = mText.GetCurrentCharPos(inStartPos);
		AUTF16Char	utf16char;
		AItemCount	bc = mText.Convert1CharToUTF16(startPos,utf16char);
		//B0360
		AUnicodeData&	unicodeData = AApplication::GetApplication().NVI_GetUnicodeData();
		AUnicodeCharCategory	firstCharCategory = unicodeData.GetUnicodeCharCategory(utf16char);
		if( firstCharCategory == kUnicodeCharCategory_Others )
		{
			outStart = startPos;
			outEnd = startPos+bc;
		}
		else
		{
			AIndex	pos;
			for( pos = mText.GetPrevCharPos(startPos); pos > 0 ; pos = mText.GetPrevCharPos(pos) )
			{
				AItemCount	bc = mText.Convert1CharToUTF16(pos,utf16char);
				AUnicodeCharCategory	category = unicodeData.GetUnicodeCharCategory(utf16char);
				if( firstCharCategory != category )
				{
					if( (firstCharCategory==kUnicodeCharCategory_ZenkakuHiragana||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) &&
								category == kUnicodeCharCategory_ZenkakuOtohiki )
					{
						//継続
					}
					else if( firstCharCategory == kUnicodeCharCategory_ZenkakuOtohiki && 
								(category==kUnicodeCharCategory_ZenkakuHiragana||category==kUnicodeCharCategory_ZenkakuKatakana) )
					{
						firstCharCategory = category;
					}
					else if( bunsetsuSelect == true && (firstCharCategory == kUnicodeCharCategory_ZenkakuHiragana && 
								(category==kUnicodeCharCategory_Kanji||category==kUnicodeCharCategory_ZenkakuKatakana)) )
					{
						//（漢字またはカタカナ）←ひらがな　の場合
						firstCharCategory = category;
					}
					else
					{
						pos += bc;
						break;
					}
				}
			}
			if( pos < 0 )   pos = 0;
			outStart = pos;
			
			for( pos = startPos; pos < SPI_GetTextLength(); pos = mText.GetNextCharPos(pos) )
			{
				bc = mText.Convert1CharToUTF16(pos,utf16char);
				AUnicodeCharCategory	category = unicodeData.GetUnicodeCharCategory(utf16char);
				if( firstCharCategory != category )
				{
					if( (firstCharCategory==kUnicodeCharCategory_ZenkakuHiragana||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) &&
								category == kUnicodeCharCategory_ZenkakuOtohiki )
					{
						//継続
					}
					else if( bunsetsuSelect == true && ((firstCharCategory==kUnicodeCharCategory_Kanji||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) && 
								category == kUnicodeCharCategory_ZenkakuHiragana) )
					{
						//（漢字またはカタカナ）→ひらがな　の場合
						firstCharCategory = category;
					}
					else
					{
						break;
					}
				}
			}
			outEnd = pos;
			if( outStart == outEnd )
			{
				outEnd = mText.GetNextCharPos(outStart);
			}
		}
	}
}

/**
A-Z,a-z,0-9,_のいずれかどうかを返す
*/
ABool	ADocument_EditBox::SPI_IsAsciiAlphabet( const ATextIndex inPos ) const
{
	/*#416
	AUChar ch = mText.Get(inPos);
	return Macro_IsAlphabet(ch);
	*/
	AUCS4Char	ch = 0;
	mText.Convert1CharToUCS4(inPos,ch);
	return AApplication::GetApplication().NVI_GetUnicodeData().IsAlphabetOrNumberOrUnderbar(ch);
}

/**
UTF-8のマルチバイト文字かどうかを返す
*/
ABool	ADocument_EditBox::SPI_IsUTF8Multibyte( const ATextIndex inIndex ) const
{
	return mText.IsUTF8MultiByte(inIndex);
}

/*
指定Indexのバイトのchar（1バイト）を返す
*/
AUChar	ADocument_EditBox::SPI_GetTextChar( const ATextIndex inIndex ) const
{
	return mText.Get(inIndex);
}

#pragma mark ===========================

#pragma mark ---TextPoint/TextIndex変換

/**
TextPointからIndex取得
*/
AIndex	ADocument_EditBox::SPI_GetTextIndexFromTextPoint( const ATextPoint& inPoint ) const
{
	return GetLineStart(inPoint.y) + inPoint.x;
}

/**
IndexからTextPoint取得
*/
void	ADocument_EditBox::SPI_GetTextPointFromTextIndex( const AIndex inTextPosition, ATextPoint& outPoint, const ABool inPreferNextLine ) const 
{
	//inIndexが不正の場合の処理
	if( inTextPosition > GetTextLength() || inTextPosition < 0 ) 
	{
		_ACError("index invalid",this);
		outPoint.y = GetLineCount()-1;
		outPoint.x = GetTextLength() - GetLineStart(GetLineCount()-1);
		return;
	}
	//2分法
	AIndex	startLineIndex = 0;
	AIndex	endLineIndex = GetLineCount();
	AIndex	lineIndex = (startLineIndex+endLineIndex)/2;
	AItemCount	lineCount = GetLineCount();
	for( AIndex i = 0; i < lineCount*2; i++ )//無限ループ防止（多くとも行数回のループで終わるはず）
	{
		AIndex	lineStart = GetLineStart(lineIndex);
		AIndex	lineLength = GetLineLengthWithoutLineEnd(lineIndex);
		if( inTextPosition >= lineStart && inTextPosition <= lineStart+lineLength ) 
		{
			outPoint.y = lineIndex;
			outPoint.x = inTextPosition-lineStart;
			if( inPreferNextLine == true && lineIndex+1 < GetLineCount() )
			{
				if( outPoint.x == GetLineLengthWithLineEnd(lineIndex) )
				{
					outPoint.x = 0;
					outPoint.y++;
				}
			}
			return;
		}
		if( inTextPosition < lineStart )
		{
			endLineIndex = lineIndex;
			lineIndex = (startLineIndex+endLineIndex)/2;
		}
		else if( inTextPosition > lineStart+lineLength )
		{
			startLineIndex = lineIndex+1;
			lineIndex = (startLineIndex+endLineIndex)/2;
		}
		if( startLineIndex+1 >= endLineIndex )
		{
			endLineIndex = startLineIndex+1;
			lineIndex = startLineIndex;
		}
	}
	_ACError("cannot find text index",this);
}

#pragma mark ===========================

#pragma mark ---行計算

/**
行開始位置計算
*/
void	ADocument_EditBox::CalcLineStarts()
{
	mLineStartArray.DeleteAll();
	mLineLengthArray.DeleteAll();
	//
	mLineStartArray.Add(0);
	for( AIndex i = 0; i < mText.GetItemCount(); i++ )
	{
		if( mText.Get(i) == kUChar_LineEnd )
		{
			mLineLengthArray.Add(i+1-mLineStartArray.Get(mLineStartArray.GetItemCount()-1));
			mLineStartArray.Add(i+1);
		}
	}
	mLineLengthArray.Add(mText.GetItemCount()-mLineStartArray.Get(mLineStartArray.GetItemCount()-1));
}

/**
指定Indexの行開始位置を取得
*/
AIndex	ADocument_EditBox::GetLineStart( const AIndex inLineIndex ) const
{
	if( inLineIndex >= mLineStartArray.GetItemCount() )
	{
		return mText.GetItemCount();
	}
	else
	{
		return mLineStartArray.Get(inLineIndex);
	}
}

/**
行数取得
*/
AItemCount	ADocument_EditBox::SPI_GetLineCount() const
{
	return GetLineCount();
}

/**
テキスト全体の長さ取得
*/
AItemCount	ADocument_EditBox::GetTextLength() const
{
	return mLineStartArray.Get(GetLineCount()-1) + mLineLengthArray.Get(GetLineCount()-1);
}

/**
行数取得
*/
AItemCount	ADocument_EditBox::GetLineCount() const
{
	return mLineStartArray.GetItemCount();
}

/**
LineEndコードを含めない行レングス取得
*/
AItemCount	ADocument_EditBox::GetLineLengthWithoutLineEnd( const AIndex inLineIndex ) const
{
	if( inLineIndex >= mLineLengthArray.GetItemCount() - 1 )
	{
		return mLineLengthArray.Get(inLineIndex);
	}
	else
	{
		return mLineLengthArray.Get(inLineIndex) - 1;
	}
}

/**
LineEndコードを含めた行レングス取得
*/
AItemCount	ADocument_EditBox::GetLineLengthWithLineEnd( const AIndex inLineIndex ) const
{
	return mLineLengthArray.Get(inLineIndex);
}

/**
指定行が改行コードで終わるかどうかを取得
*/
ABool	ADocument_EditBox::SPI_ExistLineEnd( const AIndex inLineIndex ) const
{
	if( inLineIndex == GetLineCount()-1 )
	{
		return false;
	}
	else
	{
		return true;
	}
}





