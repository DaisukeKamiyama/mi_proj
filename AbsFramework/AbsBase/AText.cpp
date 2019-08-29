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

AText

*/

#include "stdafx.h"

#include "ABase.h"
#include "../Imp.h"
#include "Cocoa.h"//#1034

#if IMPLEMENTATION_FOR_WINDOWS
#include <math.h>
#endif

#pragma mark ===========================
#pragma mark [クラス]AText
#pragma mark ===========================

//クラス生成時のメモリ割り当てサイズ
const AItemCount kTextInitialAllocateCountDefault = 32;//B0373
//メモリ割り当て追加時の追加サイズ
const AItemCount kTextReallocateStepCount = 32;//B0373

#pragma mark ---コンストラクタ／デストラクタ

/**
コンストラクタ
*/
AText::AText( AObjectArrayItem* inParent ) : AArray<AUChar>(inParent,kTextInitialAllocateCountDefault,kTextReallocateStepCount)//B0373
{
}

//B0373
/**
コンストラクタ
*/
AText::AText( AObjectArrayItem* inParent, const AItemCount inInitialAllocateCount, const AItemCount inReallocateStepCount )
: AArray<AUChar>(inParent,inInitialAllocateCount,inReallocateStepCount)
{
}

/**
コンストラクタ
*/
AText::AText( const AText& inText ) : AArray<AUChar>(NULL,kTextInitialAllocateCountDefault,kTextReallocateStepCount)//B0373
{
	SetText(inText);
}

/**
コンストラクタ
*/
AText::AText( const AConstCharPtr inString ) :AArray<AUChar>(NULL,kTextInitialAllocateCountDefault,kTextReallocateStepCount)//B0373
{
	AddCstring(inString);
}

/**
デストラクタ
*/
AText::~AText()
{
}

#pragma mark ===========================

#pragma mark ---代入演算子

/**
代入演算子
*/
AText& AText::operator = ( const AText& inText ) 
{
	SetText(inText);
	return (*this);
}


#pragma mark ===========================

#pragma mark --- テキストポインタから挿入／追加／設定

/*
テキストポインタから挿入(unsigned char用)
*/
void	AText::InsertFromTextPtr( const AIndex inIndex, const AConstUCharPtr inSrcPtr, const AByteCount inByteCount )
{
	//NULLポインタチェック
	if( inSrcPtr == NULL )
	{
		_ACError("",this);
		return;
	}
	//
	AItemCount	itemCount = static_cast<AItemCount>(inByteCount/sizeof(AUChar));
	if( itemCount == 0 )   return;
	//領域確保
	Reserve(inIndex,itemCount);
	//コピー先ポインタ取得
	AStTextPtr	textptr(*this,inIndex,itemCount);
	//コピー実行
	AMemoryWrapper::Memmove(textptr.GetPtr(),inSrcPtr,textptr.GetByteCount());
}

//Insert (char用)
void	AText::InsertFromTextPtr( const AIndex inIndex, const AConstCharPtr inSrcPtr, const AByteCount inByteCount )
{
	InsertFromTextPtr(inIndex,reinterpret_cast<AConstUCharPtr>(inSrcPtr),inByteCount);
}

//Add (unsigned char用)
void	AText::AddFromTextPtr( const AConstUCharPtr inSrcPtr, const AByteCount inByteCount )
{
	InsertFromTextPtr(GetItemCount(),inSrcPtr,inByteCount);
}

//Add (char用)
void	AText::AddFromTextPtr( const AConstCharPtr inSrcPtr, const AByteCount inByteCount )
{
	InsertFromTextPtr(GetItemCount(),inSrcPtr,inByteCount);
}

//Set (unsigned char用)
void	AText::SetFromTextPtr( const AConstUCharPtr inSrcPtr, const AByteCount inByteCount )
{
	DeleteAll();
	InsertFromTextPtr(0,inSrcPtr,inByteCount);
}

//Set (char用)
void	AText::SetFromTextPtr( const AConstCharPtr inSrcPtr, const AByteCount inByteCount )
{
	DeleteAll();
	InsertFromTextPtr(0,inSrcPtr,inByteCount);
}

void	AText::InsertFromUTF16TextPtr( const AIndex inIndex, const AUTF16CharPtr inSrcPtr, const AByteCount inByteCount )
{
	//B0000 追加後全体に対してConvertToUTF8FromUTF16()をかけていた問題を修正
	AText	text;
	text.InsertFromTextPtr(0,reinterpret_cast<AConstUCharPtr>(inSrcPtr),inByteCount);
	text.ConvertToUTF8FromUTF16();
	InsertText(inIndex,text);
}

#pragma mark --- テキストポインタへコピー

//テキストポインタへコピー
//inMaxByteCountにコピー先バッファの最大サイズを指定する必要有。
void	AText::CopyToTextPtr( AUCharPtr outDstPtr, const AByteCount inMaxByteCount, const AIndex inIndex, const AItemCount inItemCount ) const
{
	AItemCount	itemCount = inItemCount;
	//コピーサイズのチェック
	if( inMaxByteCount < itemCount*sizeof(AUChar) )
	{
		//コピーサイズ(inItemCount)がコピー先の最大エリアよりも大きい場合、それに収まるようにinItemCountを調整する。（UTF-8の文字区切り位置補正も実施）
		AIndex	lastCharPos = inIndex+inMaxByteCount/sizeof(AUChar);
		lastCharPos = GetPrevCharPos(lastCharPos);
		itemCount = lastCharPos-inIndex;
	}
	//コピー元ポインタ取得
	AStTextPtr	textptr(*this,inIndex,itemCount);
	//コピー実行
	AMemoryWrapper::Memmove(outDstPtr,textptr.GetPtr(),itemCount*sizeof(AUChar));
}

#pragma mark ===========================

#pragma mark --- C文字列から挿入／追加／設定

//Insert
void	AText::InsertCstring( const AIndex inIndex, const AConstCstringPtr inString )
{
	//NULLポインタチェック
	if( inString == NULL )
	{
		_ACError("",this);
		return;
	}
	//
	InsertFromTextPtr(inIndex,inString,ACstringWrapper::GetLength(inString));
}

//Add
void	AText::AddCstring( const AConstCstringPtr inString )
{
	InsertCstring(GetItemCount(),inString);
}

//Set
void AText::SetCstring( const AConstCstringPtr inString )
{
	DeleteAll();
	AddCstring(inString);
}

#pragma mark --- printf同等の挿入／追加／設定

/*#1034
void	AText::AddFormattedCstring_( const AConstCstringPtr inFormat, va_list inArgList )
{
	AByteCount len = ACstringWrapper::GetFormattedCstringLength(inFormat,inArgList);
	AIndex	insertIndex = GetItemCount();
	Reserve(insertIndex,static_cast<AItemCount>(len+1));
	{
		AStTextPtr	textptr(*this,insertIndex,len+1);
		ACstringWrapper::Sprintf(reinterpret_cast<char*>(textptr.GetPtr()),textptr.GetByteCount(),inFormat,inArgList);
	}
	//最後のNULL文字を削除
	Delete1(GetItemCount()-1);
}
*/

void	AText::AddFormattedCstring( const AConstCstringPtr inFormat, ... )
{
	//#1034 64bit対応 va_listを都度設定
	
	//引数リスト
	va_list	args;
	
	//サイズ取得
	va_start(args,inFormat);
	char	dummy;
	AByteCount len = vsnprintf(&dummy,0,inFormat,args);//サイズが無制限だったとした場合のサイズが返る（最後のNULL文字は含まない）
	va_end(args);
	
	//vsnprintf文字列取得
	va_start(args,inFormat);
	AIndex	insertIndex = GetItemCount();
	Reserve(insertIndex,static_cast<AItemCount>(len+1));
	{
		AStTextPtr	textptr(*this,insertIndex,len+1);
		vsnprintf(reinterpret_cast<char*>(textptr.GetPtr()),textptr.GetByteCount(),inFormat,args);
	}
	//最後のNULL文字を削除
	Delete1(GetItemCount()-1);
	va_end(args);
}

/*#1034
void	AText::SetFormattedCstring_( const AConstCstringPtr inFormat, va_list inArgList )
{
	DeleteAll();
	AddFormattedCstring_(inFormat,inArgList);
}
*/

void	AText::SetFormattedCstring( const AConstCstringPtr inFormat, ... )
{
	//全削除
	DeleteAll();
	
	//以下、AddFormattedCstring()と同じ処理
	//#1034 64bit対応 va_listを都度設定
	
	//引数リスト
	va_list	args;
	
	//サイズ取得
	va_start(args,inFormat);
	char	dummy;
	AByteCount len = vsnprintf(&dummy,0,inFormat,args);//サイズが無制限だったとした場合のサイズが返る（最後のNULL文字は含まない）
	va_end(args);
	
	//vsnprintf文字列取得
	va_start(args,inFormat);
	AIndex	insertIndex = GetItemCount();
	Reserve(insertIndex,static_cast<AItemCount>(len+1));
	{
		AStTextPtr	textptr(*this,insertIndex,len+1);
		vsnprintf(reinterpret_cast<char*>(textptr.GetPtr()),textptr.GetByteCount(),inFormat,args);
	}
	//最後のNULL文字を削除
	Delete1(GetItemCount()-1);
	va_end(args);
}

#pragma mark --- C文字列+AText+C文字列の組み合わせ

void	AText::SetCstringAndText( const AConstCstringPtr inString1, const AText& inText, const AConstCstringPtr inString2 )
{
	SetCstring(inString1);
	AddText(inText);
	AddCstring(inString2);
}

#if IMPLEMENTATION_FOR_WINDOWS
void	AText::SetFromWcharString( const wchar_t* inString, AItemCount inMaxLength )
{
	DeleteAll();
	//security AItemCount	len = ::wcslen(inString);
	AItemCount	len = 0;
	for( ; len < inMaxLength; len++ )
	{
		if( inString[len] == 0x0000 )
		{
			break;
		}
	}
	if( len == 0 )   return;
	AddFromTextPtr(reinterpret_cast<const char*>(inString),len*sizeof(wchar_t));
	ConvertToUTF8FromAPIUnicode();
}
#endif

#pragma mark ===========================

#pragma mark --- ATextから挿入／追加／設定

void AText::InsertText( const AIndex inDstIndex, const AText& inSrcText, const AIndex inSrcIndex, const AItemCount inSrcItemCount )
{
	AStTextPtr	textptr(inSrcText,inSrcIndex,inSrcItemCount);
	InsertFromTextPtr(inDstIndex,textptr.GetPtr(),textptr.GetByteCount());
}

void	AText::InsertText( const AIndex inDstIndex, const AText& inSrcText )
{
	InsertText(inDstIndex,inSrcText,0,inSrcText.GetItemCount());
}

void AText::AddText( const AText& inText )
{
	InsertText(GetItemCount(),inText);
}

void AText::SetText( const AText& inText )
{
	// コピー元と、このオブジェクトが同じ場合は何もしない #1231 （もし同じ場合に、この下を実行すると、DeleteAll()で全削除されてしまう）
	if( GetDataPtrWithoutCheck() == inText.GetDataPtrWithoutCheck() )
	{
		return;
	}
	//
	DeleteAll();
	AddText(inText);
}

#pragma mark ---ATextへコピー
//指定インデックス(inIndex)から、指定文字数(inCount)を、outTextへSetする。
void	AText::GetText( const AIndex& inIndex, const AItemCount& inCount, AText& outText ) const
{
	outText.DeleteAll();
	outText.InsertText(0,*this,inIndex,inCount);
}

//#467
/**
テキストを取得（スペース系文字は削除）
*/
void	AText::GetTextWithoutSpaces( const AIndex inIndex, const AItemCount inCount, AText& outText ) const
{
	outText.DeleteAll();
	for( AIndex i = inIndex; i < inIndex+inCount; i++ )
	{
		AUChar	ch = Get(i);
		if( ch != kUChar_Space && ch != kUChar_Tab && ch != kUChar_LineEnd )
		{
			outText.Add(ch);
		}
	}
}

//#467
/**
テキストを取得（スペース系文字は削除）
*/
void	AText::GetTextWithoutSpaces( AText& outText ) const
{
	GetTextWithoutSpaces(0,GetItemCount(),outText);
}

#pragma mark ===========================

#pragma mark ---削除

/*AArrayに移動
void AText::DeleteAll()
{
	Delete(0,GetItemCount());
}

void AText::DeleteAfter( const AIndex inIndex )
{
	Delete(inIndex,GetItemCount()-inIndex);
}
*/
//UTF16のヌル文字以降を削除 win
void	AText::DeleteAfterUTF16NullChar()
{
	for( AIndex i = 0; i < GetItemCount(); i += sizeof(AUTF16Char) )
	{
		if( Get(i) == 0 && Get(i+1) == 0 )
		{
			DeleteAfter(i);
			return;
		}
	}
}

#pragma mark ===========================

#pragma mark ---比較

ABool	AText::Compare( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	/*#695
	AItemCount	targetItemCount = inTargetItemCount;
	if( targetItemCount == kIndex_Invalid )   targetItemCount = inTargetText.GetItemCount();
	if( GetLength() != targetItemCount )   return false;
	AStTextPtr	ptr1(*this,0,GetItemCount());
	AStTextPtr	ptr2(inTargetText,inTargetIndex,targetItemCount);
	return AMemoryWrapper::Memcmp(ptr1.GetPtr(),ptr2.GetPtr(),ptr1.GetByteCount());
	*/
	return Compare(0,GetItemCount(),inTargetText,inTargetIndex,inTargetItemCount);
}

/**
比較
#695
*/
ABool	AText::Compare( const AIndex inIndex, const AItemCount inItemCount,
		const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	//ItemCount取得（kIndex_Invalidの場合はGetItemCount()を取得
	AItemCount	itemCount = inItemCount;
	if( itemCount == kIndex_Invalid )   itemCount = GetItemCount();
	AItemCount	targetItemCount = inTargetItemCount;
	if( targetItemCount == kIndex_Invalid )   targetItemCount = inTargetText.GetItemCount();
	//ItemCountが違う場合はここでreturn
	if( itemCount != targetItemCount )   return false;
	//メモリ比較
	AStTextPtr	ptr1(*this,inIndex,itemCount);
	AStTextPtr	ptr2(inTargetText,inTargetIndex,targetItemCount);
	return AMemoryWrapper::Memcmp(ptr1.GetPtr(),ptr2.GetPtr(),ptr1.GetByteCount());
}

ABool	AText::Compare( const AConstCstringPtr inString ) const
{
	if( GetLength() != ACstringWrapper::GetLength(inString) )   return false;
	AStTextPtr	ptr(*this,0,GetItemCount());
	return AMemoryWrapper::Memcmp(ptr.GetPtr(),inString,ptr.GetByteCount());
}

ABool	AText::CompareMin( const AText& inText ) const
{
	AItemCount	count = inText.GetLength();
	if( GetLength() < count )
	{
		count = GetLength();
	}
	AStTextPtr	ptr1(*this,0,count);
	AStTextPtr	ptr2(inText,0,count);
	return AMemoryWrapper::Memcmp(ptr1.GetPtr(),ptr2.GetPtr(),ptr1.GetByteCount());
}

ABool	AText::CompareMin( const AConstCstringPtr inString ) const
{
	AItemCount	count = ACstringWrapper::GetLength(inString);
	if( GetLength() < count )
	{
		count = GetLength();
	}
	AStTextPtr	ptr1(*this,0,count);
	return AMemoryWrapper::Memcmp(ptr1.GetPtr(),inString,ptr1.GetByteCount());
}

ABool	AText::CompareIgnoreCase( const AText& inText ) const
{
	AText	text1, text2;
	text1.SetText(*this);
	text1.ChangeCaseLower();
	text2.SetText(inText);
	text2.ChangeCaseLower();
	return text1.Compare(text2);
}

ABool	AText::IsLessThan( const AText& inText ) const
{
#if IMPLEMENTATION_FOR_MACOSX
	AStCreateCFStringFromAText	cfstr1(*this);
	AStCreateCFStringFromAText	cfstr2(inText);
	return (::CFStringCompare(cfstr1.GetRef(),cfstr2.GetRef(),kCFCompareCaseInsensitive) == kCFCompareLessThan);
#elif IMPLEMENTATION_FOR_WINDOWS
	AStCreateWcharStringFromAText	str1(*this);
	AStCreateWcharStringFromAText	str2(inText);
	return (::lstrcmp(str1.GetPtr(),str2.GetPtr())<0);//★検討CompareStringのほうがよい？★★速度検討（変換候補遅い。例えばGet）
#else
	not implemented
#endif
}

#pragma mark ===========================
#pragma mark ---置換
//テキスト中の指定文字(inFromChar)を、指定文字(inToChar)で置換する。
void	AText::ReplaceChar( const AUChar inFromChar, const AUChar inToChar )
{
	for( AIndex pos = 0; pos < GetItemCount(); pos = GetNextCharPos(pos) )
	{
		AUChar	ch = Get(pos);
		if( ch == inFromChar )
		{
			Set(pos,inToChar);
		}
	}
}

//テキスト中の指定文字(inFromChar)を、inToTextで置換する。
void	AText::ReplaceChar( const AUChar inFromChar, AText& inToText )
{
	for( AIndex pos = 0; pos < GetItemCount(); )
	{
		if( Get(pos) == inFromChar )
		{
			Delete(pos,1);
			InsertText(pos,inToText);
			pos += inToText.GetItemCount();
		}
		else
		{
			pos = GetNextCharPos(pos);
		}
	}
}

//テキスト中の指定テキスト(inFromText)を、inToTextで置換する
void	AText::ReplaceText( const AText& inFromText, const AText& inToText )
{
	for( AIndex pos = 0; pos < GetItemCount(); )
	{
		AIndex	foundPos;
		if( FindText(pos,inFromText,foundPos) == true )
		{
			Delete(foundPos,inFromText.GetItemCount());
			InsertText(foundPos,inToText);
			pos = foundPos+inToText.GetItemCount();//B0000 次の検索は置換文字列の後ろから始めるべき
		}
		else
		{
			break;
		}
	}
}

//テキスト中の指定テキスト(inFromCstring)を、inToCstringで置換する
void	AText::ReplaceCstring( const AConstCstringPtr inFromCstring, const AConstCstringPtr inToCstring )
{
	AText	fromText, toText;
	fromText.SetCstring(inFromCstring);
	toText.SetCstring(inToCstring);
	ReplaceText(fromText,toText);
}

/**
テキスト中の"^x"（xはinCharで指定）部分を、inTextで置換する。
*/
void	AText::ReplaceParamText( const AUChar inChar, const AText& inText )
{
	for( AIndex pos = 0; pos < GetItemCount(); pos++ )
	{
		if( Get(pos) == '^' )
		{
			if( pos+1 < GetItemCount() )
			{
				if( Get(pos+1) == inChar )
				{
					Delete(pos,2);
					InsertText(pos,inText);
					//追加したテキスト内の^Xを置換すると無限ループするので、追加したテキスト分、posを進める（forで++するので、-1）
					pos += inText.GetItemCount()-1;
				}
			}
		}
	}
}
/**
テキスト中の"^x"（xはinCharで指定）部分を、inNumberで置換する。
*/
void	AText::ReplaceParamText( const AUChar inChar, const ANumber inNumber )
{
	AText	text;
	text.SetNumber(inNumber);
	ReplaceParamText(inChar,text);
}

//#450
/**
スペース・タブを削除
*/
void	AText::RemoveSpaceTab()
{
	AIndex	dstpos = 0;
	{
		AStTextPtr	textptr(*this,0,GetItemCount());
		AUChar*	p = textptr.GetPtr();
		AItemCount	len = textptr.GetByteCount();
		//
		for( AIndex pos = 0; pos < len; pos++ )
		{
			AUChar	ch = p[pos];
			if( ch == kUChar_Space || ch == kUChar_Tab )
			{
				//処理無し（テキストには入れない）
			}
			else
			{
				//テキストに追加
				p[dstpos] = ch;
				dstpos++;
			}
		}
	}
	DeleteAfter(dstpos);
}

//テキスト中の大文字を全て小文字に変換する
void	AText::ChangeCaseLower()
{
#if IMPLEMENTATION_FOR_MACOSX
	AStCreateCFMutableStringFromAText	strref(*this);
	::CFStringLowercase(strref.GetRef(),NULL);
	SetFromCFString(strref.GetRef());
#elif IMPLEMENTATION_FOR_WINDOWS
	AStCreateWcharStringFromAText	strref(*this);
	CharLowerW(strref.GetWritablePtr());
	SetFromWcharString(strref.GetPtr(),strref.GetWcharCount());
#else
	not implemented
#endif
}

//B0368
ABool	AText::ChangeCaseLowerFast()
{
	ABool	changed = false;//B0000
	AItemCount	itemcount = GetItemCount();//B0000
	for( AIndex pos = 0; pos < itemcount; pos++ )//B0000
	{
		AUChar	ch = Get(pos);
		if( ch >= 'A' && ch <= 'Z' )
		{
			ch += 'a'-'A';
			Set(pos,ch);
			changed = true;
		}
	}
	return changed;
}



//テキスト中の小文字を全て大文字に変換する
void	AText::ChangeCaseUpper()
{
#if IMPLEMENTATION_FOR_MACOSX
	AStCreateCFMutableStringFromAText	strref(*this);
	::CFStringUppercase(strref.GetRef(),NULL);
	SetFromCFString(strref.GetRef());
	return;
#elif IMPLEMENTATION_FOR_WINDOWS
	AStCreateWcharStringFromAText	strref(*this);
	CharUpperW(strref.GetWritablePtr());
	SetFromWcharString(strref.GetPtr(),strref.GetWcharCount());
#else
	not implemented
#endif
	
}

/**
指定文字を指定個数繰り返したテキストを設定
*/
void	AText::SetRepeatedCstring( const AConstCstringPtr inCstring, const AItemCount inCount )
{
	DeleteAll();
	for( AIndex i = 0; i < inCount; i++ )
	{
		AddCstring(inCstring);
	}
}

//#914
/**
modifierキーを表すテキストを設定
*/
void	AText::AddModifiersKeyText( const AModifierKeys inModifierKeys )
{
	//
	if( (inModifierKeys&kModifierKeysMask_Shift) != 0 )
	{
		AddLocalizedText("ShiftKey");
	}
	if( (inModifierKeys&kModifierKeysMask_Control) != 0 )
	{
		AddLocalizedText("ControlKey");
	}
	if( (inModifierKeys&kModifierKeysMask_Option) != 0 )
	{
		AddLocalizedText("OptionKey");
	}
	if( (inModifierKeys&kModifierKeysMask_Command) != 0 )
	{
		AddLocalizedText("CommandKey");
	}
}

#pragma mark ===========================
#pragma mark ---テキスト解析
//改行位置かどうかを判定し、改行位置なら次の行の最初位置をoutPositionに格納し、trueを返す。改行位置でないなら、outPosition変更せず、falseを返す。
ABool	AText::IsLineEnd( const AIndex inPosition, AIndex& outPosition ) const
{
	AUChar	ch = Get(inPosition);
	if( ch == kUChar_CR )
	{
		if( inPosition+1 < GetItemCount() )
		{
			ch = Get(inPosition+1);
			if( ch == kUChar_LF )
			{
				outPosition = inPosition+2;
				return true;
			}
		}
		outPosition = inPosition+1;
		return true;
	}
	if( ch == kUChar_LF )
	{
		outPosition = inPosition+1;
		return true;
	}
	return false;
}

/**
１行をoutTextへ読み込み、ioPositionを次の行頭へ移動する
返すATextには改行コードは含まない
*/
void	AText::GetLine( AIndex& ioPosition, AText& outText ) const
{
	AIndex	pos = ioPosition;
	for( ; pos < GetItemCount(); pos++ )
	{
		AIndex	lineEndPos;
		if( IsLineEnd(pos,lineEndPos) == true )
		{
			//返すATextには改行コードは含まない
			GetText(ioPosition,pos-ioPosition,outText);
			//次の行頭へ移動
			ioPosition = lineEndPos;
			return;
		}
	}
	//改行コードなしの場合
	GetText(ioPosition,pos-ioPosition,outText);
	ioPosition = pos;
}

//ioPositionを次の行頭へ移動する
void	AText::GoForwardToNextLineStart( AIndex& ioPosition ) const
{
	for( ; ioPosition < GetItemCount(); ioPosition++ )
	{
		AIndex	lineEndPos;
		if( IsLineEnd(ioPosition,lineEndPos) == true )
		{
			ioPosition = lineEndPos;
			return;
		}
	}
}

/**
指定文字(inChar)が見つかるまで、ioPositionを戻す。
見つからない場合、ioPositionは-1になる
*/
ABool	AText::GoBackToChar( AIndex& ioPosition, const AUChar inChar ) const //#427
{
	ABool	result = false;//#427
	for( ; ioPosition >= 0 ; ioPosition-- )//#427 見つかったかどうかを返すために、>を>=に変更。
	{
		if( Get(ioPosition) == inChar )
		{
			result = true;//#427
			break;
		}
	}
	return result;//#427
}

/**
指定文字(inChar)が見つかるまで、ioPositionを進める
見つからない場合、ioPositionはitemcount-1になる
*/
ABool	AText::GoForwardToChar( AIndex& ioPosition, const AUChar inChar ) const //#427
{
	ABool	result = false;//#427
	for( ; ioPosition < GetItemCount(); ioPosition++ )
	{
		if( Get(ioPosition) == inChar )
		{
			result = true;//#427
			break;
		}
	}
	return result;//#427
}

//指定デリミタ(inDelimiter)で文字列を区切り、outTextArrayに順に格納する。
//デリミタが2文字連続する場合は、空白ATextが格納される
//最後の文字がデリミタの場合は、最後に空白ATextが格納される
void	AText::Explode( const AUChar inDelimiter, ATextArray& outTextArray ) const
{
	/*#693 ATextArray::ExplodeFromText()とコード重複のため、それをコールするように変更
	outTextArray.DeleteAll();
	AIndex	spos = 0;
	AIndex pos = 0;
	for( ; pos < GetItemCount(); pos++ )
	{
		AUChar	ch = Get(pos);
		if( ch == inDelimiter )
		{
			AText	text;
			GetText(spos,pos-spos,text);
			outTextArray.Add(text);
			spos = pos+1;
		}
	}
	AText	text;
	GetText(spos,pos-spos,text);
	outTextArray.Add(text);
	*/
	outTextArray.ExplodeFromText(*this,inDelimiter);
}

#pragma mark ===========================
#pragma mark ---UTF-8文字列操作

/**
UTF-8の1文字を単位として、1文字前の位置を得る
@note 引数inReturnMinusWhen0によって、inPos=0の場合に-1を返すか、0を返すかを選択可能。（デフォルト：false(0を返す)）
*/
AIndex	AText::GetPrevCharPos( const AIndex inPos, const ABool inReturnMinusWhen0 ) const
{
	//#789
	//inReturnMinusWhen0がtrueの場合、inPos=0の場合に-1を返す。（inReturnMinusWhen0がfalseの場合は、0を返す）
	if( inReturnMinusWhen0 == true )
	{
		if( inPos <= 0 )
		{
			return inPos-1;
		}
	}
	//inPosが0なら、0を返す
	if( inPos <= 0 )   return inPos;
	//
	AIndex	pos = inPos-1;
	for( ; pos > 0; pos--)
	{
		AUChar	ch =  Get(pos);
		if( ( IsUTF8Multibyte(ch) == false ) || ( IsUTF8Multibyte1(ch) == true ) )   break;
	}
	return pos;
}
AIndex	AText::GetPrevCharPos( const AUChar* inTextPtr, const AItemCount inTextLen,
		const AIndex inPos, const ABool inReturnMinusWhen0 )
{
	//#789
	//inReturnMinusWhen0がtrueの場合、inPos=0の場合に-1を返す。（inReturnMinusWhen0がfalseの場合は、0を返す）
	if( inReturnMinusWhen0 == true )
	{
		if( inPos <= 0 )
		{
			return inPos-1;
		}
	}
	//inPosが0なら、0を返す
	if( inPos <= 0 )   return inPos;
	//
	AIndex	pos = inPos-1;
	for( ; pos > 0; pos--)
	{
		AUChar	ch =  inTextPtr[pos];
		if( ( IsUTF8Multibyte(ch) == false ) || ( IsUTF8Multibyte1(ch) == true ) )   break;
	}
	return pos;
}

//UTF-8の1文字を単位として、1文字後の位置を得る
AIndex	AText::GetNextCharPos( const AIndex inPos ) const
{
	if( inPos == GetItemCount() )   return inPos;
	AIndex	pos = inPos+1;
	for( ; pos < GetItemCount(); pos++ )
	{
		AUChar	ch =  Get(pos);
		if( ( IsUTF8Multibyte(ch) == false ) || ( IsUTF8Multibyte1(ch) == true ) )   break;
	}
	return pos;
}
AIndex	AText::GetNextCharPos( const AUChar* inTextPtr, const AItemCount inTextLen, const AIndex inPos )
{
	if( inPos == inTextLen )   return inPos;
	AIndex	pos = inPos+1;
	for( ; pos < inTextLen; pos++ )
	{
		AUChar	ch =  inTextPtr[pos];
		if( ( IsUTF8Multibyte(ch) == false ) || ( IsUTF8Multibyte1(ch) == true ) )   break;
	}
	return pos;
}

//UTF-8の1文字を単位として、現在の文字の最初のバイト位置を得る
AIndex	AText::GetCurrentCharPos( const AIndex inPos ) const
{
	if( inPos == 0 )   return inPos;
	if( inPos == GetItemCount() )   return inPos;
	AIndex	pos = inPos;
	for( ; pos > 0; pos--)
	{
		AUChar	ch =  Get(pos);
		if( ( IsUTF8Multibyte(ch) == false ) || ( IsUTF8Multibyte1(ch) == true ) )   break;
	}
	return pos;
}

ABool	AText::IsUTF8FirstByteInMultiByte( const AIndex inPos ) const
{
	return IsUTF8Multibyte1(Get(inPos));
}
ABool	AText::IsUTF8FirstByteInMultiByte( const AUChar inChar ) const
{
	return IsUTF8Multibyte1(inChar);
}

ABool	AText::IsUTF8MultiByte( const AIndex inPos ) const
{
	return IsUTF8Multibyte(Get(inPos));
}
ABool	AText::IsUTF8MultiByte( const AUChar inChar ) const
{
	return IsUTF8Multibyte(inChar);
}

//Unicode一文字分のUTF8マルチバイト数を返す
AItemCount	AText::GetUTF8ByteCount( const AIndex inPos ) const
{
	return GetUTF8ByteCount(Get(inPos));
}
AItemCount	AText::GetUTF8ByteCount( const AUChar inChar ) const
{
	//#412 GetUTF8ByteCount()の正否を返すルーチンを作成
	AItemCount	byteCount = 1;
	GetUTF8ByteCount(inChar,byteCount);
	return byteCount;
}
ABool	AText::GetUTF8ByteCount( const AUChar inChar, AItemCount& outByteCount ) const//#412
{
	ABool	result = true;
	if( (inChar&0x80) == 0 )   outByteCount = 1;
	else if( (inChar&0xE0) == 0xC0 )   outByteCount = 2;
	else if( (inChar&0xF0) == 0xE0 )   outByteCount = 3;
	else if( (inChar&0xF8) == 0xF0 )   outByteCount = 4;
	//else if( (inChar&0xFC) == 0xF8 )   return 5; UnicodeはU+10FFFFまでなので、UTF-8は4バイトまで
	//else if( (inChar&0xFE) == 0xFC )   return 6;
	else
	{
		//UTF-8の2バイト目以降など
		outByteCount = 1;//#412
		result = false;
	}
	return result;
}

void	AText::Get1UTF8Char( AIndex& ioIndex, AText& outChar ) const
{
	AItemCount	count = GetUTF8ByteCount(ioIndex);
	GetText(ioIndex,count,outChar);
	ioIndex += count;
}

AItemCount	AText::Convert1CharToUTF16( const AIndex inIndex, AUTF16Char& outUniChar ) const
{
	unsigned char	ch1,ch2,ch3;
	ch1 = Get(inIndex);
	if( (ch1&0x80) == 0 )
	{
		outUniChar = ch1;
		return 1;
	}
	else if( (ch1&0xE0) == 0xC0 )
	{
		//B0000
		if( inIndex+1 >= GetItemCount() )
		{
			outUniChar = 0;
			return GetItemCount()-inIndex;
		}
		
		ch2 = Get(inIndex+1);
		outUniChar = ((ch1&0x1C)>>2)*0x100 + ((ch1&0x03)<<6)+((ch2&0x3F));
		return 2;
	}
	else if( (ch1&0xF0) == 0xE0 )
	{
		//B0000
		if( inIndex+2 >= GetItemCount() )
		{
			outUniChar = 0;
			return GetItemCount()-inIndex;
		}
		
		ch2 = Get(inIndex+1);
		ch3 = Get(inIndex+2);
		outUniChar = (((ch1&0x0F)<<4)+((ch2&0x3C)>>2))*0x100 + ((ch2&0x03)<<6)+((ch3&0x3F));
		return 3;
	}
	else
	{
		outUniChar = 0;
		return 4;
	}
}

//#412
/**
指定index以降のUnicode一文字をUCS4で取得
*/
AItemCount	AText::Convert1CharToUCS4( const AIndex inIndex, AUCS4Char& outUniChar ) const
{
	AStTextPtr	textptr(*this,0,GetItemCount());
	return AText::Convert1CharToUCS4(textptr.GetPtr(),textptr.GetByteCount(),inIndex,outUniChar);
}
/**
指定index以降のUnicode一文字をUCS4で取得（テキストポインタ指定・static）
#733 検索高速化のためテキストポインタ指定版を作成
*/
AItemCount	AText::Convert1CharToUCS4( const AUChar* inTextPtr, const AItemCount inTextLen,
			const AIndex inIndex, AUCS4Char& outUniChar ) //#733
{
	unsigned char	ch1,ch2,ch3,ch4;
	ch1 = inTextPtr[inIndex];//#733 Get(inIndex);
	if( (ch1&0x80) == 0 )
	{
		outUniChar = ch1;
		return 1;
	}
	else if( (ch1&0xE0) == 0xC0 )
	{
		if( inIndex+1 >= /*#733 GetItemCount()*/inTextLen )
		{
			outUniChar = 0;
			return /*#733 GetItemCount()*/inTextLen -inIndex;
		}
		
		ch2 = inTextPtr[inIndex+1];//#733 Get(inIndex+1);
		outUniChar = ((ch1&0x1C)>>2)*0x100 + ((ch1&0x03)<<6)+((ch2&0x3F));
		return 2;
	}
	else if( (ch1&0xF0) == 0xE0 )
	{
		if( inIndex+2 >= /*#733 GetItemCount()*/inTextLen )
		{
			outUniChar = 0;
			return /*#733 GetItemCount()*/inTextLen -inIndex;
		}
		
		ch2 = inTextPtr[inIndex+1];//#733 Get(inIndex+1);
		ch3 = inTextPtr[inIndex+2];//#733 Get(inIndex+2);
		outUniChar = (((ch1&0x0F)<<4)+((ch2&0x3C)>>2))*0x100 + ((ch2&0x03)<<6)+((ch3&0x3F));
		return 3;
	}
	else if( (ch1&0xF8) == 0xF0 )
	{
		if( inIndex+3 >= /*#733 GetItemCount()*/inTextLen )
		{
			outUniChar = 0;
			return /*#733 GetItemCount()*/inTextLen -inIndex;
		}
		
		ch2 = inTextPtr[inIndex+1];//#733 Get(inIndex+1);
		ch3 = inTextPtr[inIndex+2];//#733 Get(inIndex+2);
		ch4 = inTextPtr[inIndex+3];//#733 Get(inIndex+3);
		outUniChar = (((ch1&0x07)<<2)+((ch2&0x30)>>4))*0x10000 +
				(((ch2&0x0F)<<4)+((ch3&0x3C)>>2))*0x100 + ((ch3&0x03)<<6)+((ch4&0x3F));
		return 4;
	}
	else
	{
		outUniChar = 0;
		return 1;
	}
}

//#427
/**
UCS4文字を挿入
*/
void	AText::InsertFromUCS4Char( const AIndex inIndex, const AUCS4Char inUniChar )
{
	if( inUniChar < 0x0080 )
	{
		Insert1(inIndex,inUniChar);
	}
	else if( inUniChar < 0x0800 )
	{
		Insert1(inIndex  ,0xC0|((inUniChar&0x07C0)>>6));
		Insert1(inIndex+1,0x80|((inUniChar&0x003F)));
	}
	else if( inUniChar < 0x10000 )
	{
		Insert1(inIndex  ,0xE0|((inUniChar&0xF000)>>12));
		Insert1(inIndex+1,0x80|((inUniChar&0x0FC0)>>6));
		Insert1(inIndex+2,0x80|((inUniChar&0x003F)));
	}
	else if( inUniChar < 0x200000 )
	{
		Insert1(inIndex  ,0xF0|((inUniChar&0x1C0000)>>18));
		Insert1(inIndex+1,0x80|((inUniChar&0x03F000)>>12));
		Insert1(inIndex+2,0x80|((inUniChar&0x000FC0)>>6));
		Insert1(inIndex+3,0x80|((inUniChar&0x00003F)));
	}
	else
	{
		_ACError("invalid UCS4 character",this);
	}
}

/**
UCS4文字を追加
*/
void	AText::AddFromUCS4Char( const AUCS4Char inUniChar )
{
	InsertFromUCS4Char(GetItemCount(),inUniChar);
}

#pragma mark ===========================
//指定デリミタ(inDelimiter)まで、もしくは、改行までの文字列をoutTextに取得し、ioPositionを進める。
/*R-abs void AText::GetTokenBeforeLineEnd( AIndex& ioPosition, AText& outText, const AUChar inDelimiter ) const
{
	outText.DeleteAll();
	
	while( inPosition < GetItemCount() )
	{
		unsigned char	ch = Get(inPosition);
		if( ch == A_CHAR_CR || ch == A_CHAR_LF )
		{
			break;
		}
		inPosition++;
		if( ch == inDelimiter )                                                                                                                                                             
		{
			break;
		}
		outText.Add(ch);
	}
}*/

//拡張子取得（
void AText::GetSuffix( AText& outText ) const
{
	outText.DeleteAll();
	if( GetLength() == 0 )   return;
	AIndex	pos = GetItemCount()-1;
	if( GoBackToChar(pos,A_CHAR_PERIOD) == false )   return;//#427 .が見つからない場合はreturn
	if( pos >= GetItemCount() )   return;
	if( Get(pos) != '.' )   return;
	GetText(pos,GetItemCount()-pos,outText);
}
AIndex	AText::GetSuffixStartPos() const
{
	if( GetLength() == 0 )   return 0;
	for( AIndex	pos = GetItemCount()-1; pos >= 0; pos-- )
	{
		if( Get(pos) == kUChar_Period )   return pos;
	}
	return GetItemCount();
}
//R0137
AIndex	AText::GetFirstSuffixStartPos() const
{
	for( AIndex	pos = 0; pos < GetItemCount(); pos++ )
	{
		if( Get(pos) == kUChar_Period )   return pos;
	}
	return GetItemCount();
}

/**
token取得
*/
void AText::GetToken( AIndex& ioPosition, AText& outText ) const
{
	AStTextPtr	textptr(*this,0,GetItemCount());
	GetToken(textptr.GetPtr(),textptr.GetByteCount(),ioPosition,outText);
}
void AText::GetToken( const AUChar* inTextPtr, const AItemCount inTextLen,
					 AIndex& ioPosition, AText& outText ) 
{
	outText.DeleteAll();
	
	if( ioPosition >= inTextLen )   return;
	AUChar	ch = inTextPtr[ioPosition];
	ioPosition++;
	if( ch == kUChar_Space || ch == kUChar_Tab )
	{
		while( ioPosition < inTextLen )
		{
			ch = inTextPtr[ioPosition];
			ioPosition++;
			if( ch == kUChar_Space || ch == kUChar_Tab )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
	outText.Add(ch);
	if( (((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')||ch=='_'||ch=='-')) == false )
	{
		return;
	}
	while( ioPosition < inTextLen )
	{
		AUChar	ch = inTextPtr[ioPosition];
		if( (((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')||ch=='_'||ch=='-')) == false )
		{
			break;
		}
		outText.Add(ch);
		ioPosition++;
	}
}

void	AText::GetTextToTabSpaceLineEnd( AIndex& ioPos, AText& outText ) const
{
	outText.DeleteAll();
	//
	if( ioPos >= GetItemCount() )   return;
	//現在位置のスペース・タブ・改行はとばす
	AUChar	ch = Get(ioPos);
	if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_CR || ch == kUChar_LF )
	{
		ioPos++;
		for( ; ioPos < GetItemCount(); ioPos++ )
		{
			ch = Get(ioPos);
			if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_CR || ch == kUChar_LF )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
	if( ioPos >= GetItemCount() )   return;
	//開始位置決定
	AIndex	spos = ioPos;
	//
	ioPos++;
	for( ; ioPos < GetItemCount(); ioPos++ )
	{
		ch = Get(ioPos);
		if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_CR || ch == kUChar_LF )
		{
			break;
		}
		else
		{
			continue;
		}
	}
	//
	GetText(spos,ioPos-spos,outText);
}

void AText::GoBackGetToken( AIndex& inPosition, AText& outText ) const
{
	outText.DeleteAll();
	
	inPosition--;
	if( inPosition < 0 )   return;
	unsigned char	ch = Get(inPosition);
	if( ch == A_CHAR_SPACE || ch == A_CHAR_TAB )
	{
		while( inPosition > 0 )
		{
			inPosition--;
			ch = Get(inPosition);
			if( ch == A_CHAR_SPACE || ch == A_CHAR_TAB )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
	outText.Add(ch);
	if( (((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')||ch=='_'||ch=='-')) == false )
	{
		return;
	}
	while( inPosition > 0 )
	{
		inPosition--;
		unsigned char	ch = Get(inPosition);
		if( (((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')||ch=='_'||ch=='-')) == false )
		{
			inPosition++;
			break;
		}
		outText.Insert1(0,ch);
	}
}

/**
inStartPos以降からinLengthバイト分、スペース文字以外の文字を取得する
*/
AIndex	AText::GetCharactersOtherThanSpaces( const AUChar* inTextPtr, const AItemCount inTextLen,
											const AIndex inStartPos, const AItemCount inLength, AText& outText ) 
{
	//結果初期化
	outText.DeleteAll();
	//文字毎のループ
	AIndex pos = inStartPos;
	for( ; pos < inTextLen; pos++ )
	{
		//文字取得
		AUChar	ch = inTextPtr[pos];
		if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_LineEnd )
		{
			//スペース文字なら何もせずループ継続
			continue;
		}
		else
		{
			//結果に文字追加
			outText.Add(ch);
			//指定文字数取得したら終了
			if( outText.GetItemCount() == inLength )
			{
				pos++;
				break;
			}
		}
	}
	return pos;
}

#pragma mark ===========================

ANumber	AText::GetIntegerByHexChar( const AIndex inIndex ) const
{
	AUChar	ch = Get(inIndex);
	if( ch >= '0' && ch <= '9' )
	{
		ch -= '0';
	}
	else if( ch >= 'A' && ch <= 'F' )
	{
		ch = ch-'A'+10;
	}
	else if( ch >= 'a' && ch <= 'f' )
	{
		ch = ch-'a'+10;
	}
	else 
	{
		return 0;
	}
	return ch;
}

AUInt32	AText::GetIntegerFromHexText() const
{
	AUInt32	result = 0;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		result = result * 0x10;
		result += GetIntegerByHexChar(i);
	}
	return result;
}

#pragma mark 正規化

void AText::Normalize()
{
	NormalizeLineEnd();
	//
}

void AText::NormalizeLineEnd()
{
	ConvertLineEndTo(A_CHAR_LineEnd);
}


//#200 メソッド名変更 AddTextFromUTF16CharPtr→SetTextFromUTF16CharPtr  最後でConvertToUTF8FromUTF16()をしているから、Addとしては機能しない
/**
UTF16CharからTextを設定する
*/
void AText::SetTextFromUTF16CharPtr( AUTF16Char* inPtr, unsigned long inLength )
{
	DeleteAll();//#200 メソッド名変更AddTextFromUTF16CharPtr→SetTextFromUTF16CharPtrに伴い処理追加
	InsertFromTextPtr(GetItemCount(),reinterpret_cast<AConstUCharPtr>(inPtr),inLength);
	//Insert(GetItemCount(),reinterpret_cast<const unsigned char*>(inPtr),inLength);
	ConvertToUTF8FromUTF16();
}

void	AText::AddNumber( long inNumber )
{
	char	str[256];
	sprintf(str,"%ld",inNumber);
	AddCstring(str);
}

void	AText::SetNumber( ANumber inNumber )
{
	DeleteAll();
	AddNumber(inNumber);
}

//#427
/**
64bit数値をテキストに変換して追加
*/
void	AText::AddNumber64bit( const long long inNumber )
{
	char	str[256];
	sprintf(str,"%lld",inNumber);
	AddCstring(str);
}

//#427
/**
64bit数値をテキストに変換して設定
*/
void	AText::SetNumber64bit( const ANumber64bit inNumber )
{
	DeleteAll();
	AddNumber64bit(inNumber);
}

AUChar	AText::GetLastChar() const
{
	if( GetItemCount() == 0 )   return 0;
	else return Get(GetItemCount()-1);
}
#pragma mark 参照
/*B0000 高速化
AIndex AText::GetLength() const
{
	return GetItemCount();
}
*/
//取得


#pragma mark ---文字列各種操作

void	AText::LimitLength( const AItemCount inMin, const AItemCount inMax )
{
	while( GetLength() < inMin )
	{
		Add(' ');
	}
	if( GetLength() > inMax )
	{
		AIndex	pos = GetCurrentCharPos(inMax);
		DeleteAfter(pos);
	}
}

void	AText::LimitLengthAsSJIS( const AItemCount& inMin, const AItemCount& inMax )
{
	while( GetLength() < inMin )
	{
		Add(' ');
	}
	if( GetLength() > inMax )
	{
		AIndex pos = 0;
		for( ; pos < GetItemCount(); )
		{
			AUChar	c = Get(pos);
			if( ((c>=0x81&&c<=0x9F)||(c>=0xE0&&c<=0xFC)) )
			{
				if( pos+2 >= inMax )   break;
				pos += 2;
			}
			else
			{
				if( pos+1 >= inMax )   break;
				pos++;
			}
		}
		DeleteAfter(pos);
	}
}

//#723
/**
文字長さ制限（長いときはellipsisで省略）
*/
void	AText::LimitLengthWithEllipsis( const AItemCount inMax )
{
	if( GetItemCount() > inMax )
	{
		AIndex	pos = GetCurrentCharPos(inMax-3);
		DeleteAfter(pos);
		AddCstring("...");
	}
}

ABool	AText::IsCharContained( unsigned char inChar )
{
	AIndex pos = 0;
	while( pos < GetItemCount() )
	{
		unsigned char	ch = Get(pos);
		if( ch == inChar )
		{
			return true;
		}
		pos++;
	}
	return false;
}

//R0208
ABool	AText::IsCharContained( const AUChar inChar, const AIndex inStartIndex, const AIndex inEndIndex ) const
{
	for( AIndex pos = inStartIndex; pos < inEndIndex; pos++ )
	{
		AUChar	ch = Get(pos);
		if( ch == inChar )
		{
			return true;
		}
		//B0399 pos++;
	}
	return false;
}

//#695
/**
指定範囲内に含まれるinCharの数をカウントする
*/
AItemCount	AText::GetCountOfChar( const AUChar inChar ) const
{
	return GetCountOfChar(inChar,0,GetItemCount());
}
AItemCount	AText::GetCountOfChar( const AUChar inChar, const AIndex inIndex, const AItemCount inCount ) const
{
	AItemCount	count = 0;
	AStArrayPtr<AUChar>	arrayptr(*this,inIndex,inCount);
	AUChar*	p = arrayptr.GetPtr();
	for( AIndex i = 0; i < inCount; i++ )
	{
		if( p[i] == inChar )
		{
			count++;
		}
	}
	return count;
}

//#1050
/**
テキスト内に指定文字列が含まれるかどうかを返す
*/
ABool	AText::Contains( const AConstCstringPtr inStringPtr ) const
{
	AIndex	pos = 0;
	if( FindCstring(0,inStringPtr,pos) == true )
	{
		return true;
	}
	else
	{
		return false;
	}
}

#pragma mark 改行コード操作

/**
改行コード変換
*/
void AText::ConvertLineEndTo( const AUChar inChar )
{
	//読み取り位置、書き込み位置初期化
	AIndex srcpos = 0, dstpos = 0;
	{
		//ポインタ取得
		AStTextPtr	textptr(*this,0,GetItemCount());
		AUChar*	p = textptr.GetPtr();
		AItemCount	origlen = GetItemCount();
		
		//バイト毎のループ
		while( srcpos < origlen )
		{
			//読み取り
			AUChar	ch = p[srcpos];
			srcpos++;
			//文字によって分岐
			switch(ch)
			{
				//CRの場合
			  case kUChar_CR:
				{
					//指定改行コードを書き込み
					p[dstpos] = inChar;
					dstpos++;
					//次の文字を読み取って、LFならその文字は読み飛ばす
					if( srcpos < origlen )
					{
						if( p[srcpos] == kUChar_LF )
						{
							srcpos++;
						}
					}
					break;
				}
				//LFの場合
			  case kUChar_LF:
				{
					//指定改行コードを書き込み
					p[dstpos] = inChar;
					dstpos++;
					break;
				}
				//その他の文字の場合
			  default:
				{
					//読み取ったバイトをそのまま書き込み
					p[dstpos] = ch;
					dstpos++;
					break;
				}
			}
		}
	}
	//書き込んだ最後以降は削除
	DeleteAfter(dstpos);
}

void AText::ConvertLineEndToLF()
{
	ConvertLineEndTo(kUChar_LF);
}

void AText::ConvertLineEndToCR()
{
	ConvertLineEndTo(kUChar_CR);
}

#pragma mark ===========================

#pragma mark ---テキストエンコーディング／改行コード推測

/*行折り返しは推測せず、ドキュメント設定があればその設定、無ければモード設定で設定した値を使う。改行コードはConvertReturnCodeToCR()で認識。
void	AText::GuessReturnCodeAndWrap( AReturnCode& outReturnCode, ABool& outWrap ) const
{
	AIndex	end = 5*1024;//最初の5KB
	if( end > GetItemCount() )   end = GetItemCount();
	ANumber	oneline = 0;
	ANumber	over100 = 0;
	ANumber	cr = 0, crnl = 0, nl = 0;
	AUChar ch, ch2;
	for( AIndex pos = 0; pos < end; pos++ ) 
	{
		oneline++;
		ch = Get(pos);
		if( pos+1 < end )
		{
			ch2= Get(pos+1);
		}
		else
		{
			ch2 = 0x20;
		}
		if( ch == kUChar_CR && ch2 == kUChar_LF ) 
		{
			crnl++;
			pos++;
			if( oneline > 100 ) 
			{
				over100 += oneline;
			}
			oneline = 0;
		}
		else if( ch == kUChar_CR ) 
		{
			cr++;
			if( oneline > 100 ) 
			{
				over100 += oneline;
			}
			oneline = 0;
		}
		else if( ch == 10 ) 
		{
			nl++;
			if( oneline > 100 ) 
			{
				over100 += oneline;
			}
			oneline = 0;
		}
	}
	if( over100 > GetItemCount()/10 ) 
	{
		outWrap = true;
	}
	else 
	{
		outWrap = false;
	}
	if( nl >= 1 )   outReturnCode = returnCode_LF;
	else if( crnl >= 1 )   outReturnCode = returnCode_CRLF;
	else outReturnCode = returnCode_CR;
}
*/

//B0390 ioTextEncodingはコール元で初期値を設定されていなければならない
ABool	AText::GuessTextEncoding( ATextEncoding& ioTextEncoding ) const
{
	//B0390 outTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
	//①BOM
	ATextEncoding	tec = ioTextEncoding;//B0390
	//#764 if( inUseBOM == true )
	{
		if( GuessTextEncodingFromBOM(tec) == true )//B0390
		{
			ioTextEncoding = tec;//B0390
			return true;
		}
	}
	//②charset
	//#764 if( inUseCharset == true )
	{
		if( GuessTextEncodingFromCharset(tec) == true )//B0390
		{
			ioTextEncoding = tec;//B0390
			return true;
		}
	}
	//③従来方式
	//#764 if( inUseLegacy == true )
	{
		if( GuessTextEncodingByAnalysis(tec) == true )//B0390
		{
			ioTextEncoding = tec;//B0390
			return true;
		}
	}
	/*#764
	//B0166
	//全てasciiコード内ならioTextEncodingは初期値のままで、かつ、結果はtrue
	ABool	onlyAscii = true;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		if( Get(i) >= 0x80 )
		{
			onlyAscii = false;
			break;
		}
	}
	if( onlyAscii == true )
	{
		//B0390 outTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
		return true;
	}
	//B0390
	if( inSetDefaultWhenNotDecided == false )
	{
		ioTextEncoding = tec;
	}
	*/
	return false;
}

//R0045 
ABool	AText::GuessTextEncodingFromCharset( ATextEncoding &outTextEncoding ) const
{
	ABool	found = false;
	AText	tecname;
	AItemCount	lineCount = 0;
	for( AIndex pos = 0; pos < GetItemCount();  )
	{
		AText	line;
		GetLine(pos,line);
		lineCount++;
		
		//B0139 20行目までに限定する
		if( lineCount > 20 )
		{
			break;
		}
		
		/*
		HTML4
		<META http-equiv="Content-Type" content="text/html;charset=UTF-8">
		HTML5
		<meta charset="UTF-8">
		XML
		<?xml version="1.0" encoding="UTF-8"?>
		CSS
		@charset "UTF-8";
		*/
		
		AIndex	p = 0;
		//------------------@charset "UTF-8";------------------
		if( line.FindCstring(0,"@charset",p) == true )
		{
			p += 8;
			//p以降に"UTF-8"
			AIndex	spos = p;
			if( line.FindCstring(p,"\"",spos) == true )
			{
				spos++;
				//spos以降にUTF-8"
				AIndex	epos = spos;
				if( line.FindCstring(spos,"\"",epos) == true )
				{
					//spos～epos（"の前まで）のテキストを取得
					line.GetText(spos,epos-spos,tecname);
					if( ATextEncodingWrapper::GetTextEncodingFromName(tecname,outTextEncoding) == true )
					{
						found = true;
						break;
					}
				}
			}
		}
		//------------------charset=UTF-8"、または、charset="UTF-8"------------------
		else if( line.FindCstring(0,"charset",p) == true )
		{
			p += 7;
			AText	token;
			line.GetToken(p,token);
			if( token.Compare("=") == true )//=のチェック
			{
				//sposは=の直後
				AIndex	spos = p;
				line.GetToken(p,token);
				if( token.Compare("\"") == true )
				{
					//開始"が有る場合（HTML5）、sposを"の直後にする
					spos = p;
				}
				//spos以降にUTF-8"
				AIndex	epos = spos;
				if( line.FindCstring(spos,"\"",epos) == true )
				{
					//spos～epos（"の前まで）のテキストを取得
					line.GetText(spos,epos-spos,tecname);
					if( ATextEncodingWrapper::GetTextEncodingFromName(tecname,outTextEncoding) == true )
					{
						found = true;
						break;
					}
				}
			}
		}
		//Ruby
		//------------------encoding: UTF-8------------------
		else if( line.FindCstring(0,"encoding:",p,true,true) == true )
		{
			AText	tecname;
			line.GetTextToTabSpaceLineEnd(p,tecname);
			if( tecname.GetItemCount() > 0 )
			{
				if( ATextEncodingWrapper::GetTextEncodingFromName(tecname,outTextEncoding) == true )
				{
					found = true;
					break;
				}
			}
		}
		//------------------encoding="UTF-8"------------------
		else if( line.FindCstring(0,"encoding",p) == true )
		{
			p += 8;
			AText	token;
			line.GetToken(p,token);
			if( token.Compare("=") == true )//=のチェック
			{
				//sposは=の直後
				line.GetToken(p,token);
				if( token.Compare("\"") == true )
				{
					//sposを"の直後にする
					AIndex	spos = p;
					//spos以降にUTF-8"
					AIndex	epos = spos;
					if( line.FindCstring(spos,"\"",epos) == true )
					{
						//spos～epos（"の前まで）のテキストを取得
						line.GetText(spos,epos-spos,tecname);
						if( ATextEncodingWrapper::GetTextEncodingFromName(tecname,outTextEncoding) == true )
						{
							found = true;
							break;
						}
					}
				}
			}
		}
	}
	if( found == true )
	{
		return true;
	}
	return false;
}

//
ABool	AText::GuessTextEncodingFromBOM( ATextEncoding &outTextEncoding ) const
{
	ABool	result = false;
	if( GetItemCount() >= 3 )
	{
		if( Get(0) == 0xEF && Get(1) == 0xBB && Get(2) == 0xBF )
		{
			outTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
			result = true;
		}
	}
	if( GetItemCount() >= 2 )
	{
		if( (Get(0) == 0xFE && Get(1) == 0xFF) || (Get(0) == 0xFF && Get(1) == 0xFE) ) 
		{
			outTextEncoding = ATextEncodingWrapper::GetUTF16TextEncoding();
			result = true;
		}
	}
	return result;
}

//#764
/**
エラー率による推測
*/
ABool	AText::GuessTextEncodingByAnalysis( ATextEncoding &outTextEncoding ) const
{
	ABool	result = false;
	
	//==================JIS, UTF16判定==================
	//BOMが存在する場合はBOMによる推測でUTF16と判定済み。
	{
		//ポインタ取得
		AStTextPtr	textptr(*this,0,GetItemCount());
		AUChar*	p = textptr.GetPtr();
		//最初の1MBを検索
		AItemCount	len = textptr.GetByteCount();
		if( len > 1024*1024 )
		{
			len = 1024*1024;
		}
		
		AItemCount	jisCount = 0, utf16count = 0;
		for( AIndex pos = 0; pos < len-1; pos++ )
		{
			AUChar	ch1 = p[pos];
			AUChar	ch2 = p[pos+1];
			//JIS判定
			if( ch1 == 0x1B && (ch2 == '$' || ch2 == 'K') )
			{
				jisCount++;
			}
			//UTF16判定
			if( (ch1 == 0x0A && ch2 == 0x00) || (ch1 == 0x0D && ch2 == 0x00) )
			{
				utf16count++;
			}
			//
			if( jisCount >= 1 )
			{
				//JIS確定
				outTextEncoding = ATextEncodingWrapper::GetJISTextEncoding();
				return true;
			}
			if( utf16count >= 1 )
			{
				//UTF16確定
				//★UTF16LE?
				outTextEncoding = ATextEncodingWrapper::GetUTF16TextEncoding();
				return true;
			}
		}
	}
	
	//エラー率
	AFloatNumber	errorRate = 0.0;
	
	//==================UTF-8判定==================
	if( CalcUTF8ErrorRate(1024*1024,errorRate) == true )
	{
		//エラー率10%未満ならUTF-8と推測
		if( errorRate < 0.1 )
		{
			outTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
			return true;
		}
	}
	
	//==================SJIS判定==================
	if( CalcSJISErrorRate(1024*1024,errorRate) == true )
	{
		//エラー率1%未満ならSJISと推測
		if( errorRate < 0.01 )
		{
			outTextEncoding = ATextEncodingWrapper::GetSJISTextEncoding();
			return true;
		}
	}
	
	//==================EUC判定================== #1040 順番入れ替え（SJIS優先にする。もうEUC使うことは少ないでしょう。）
	if( CalcEUCErrorRate(1024*1024,errorRate) == true )
	{
		//エラー率1%未満ならEUCと推測
		if( errorRate < 0.01 )
		{
			outTextEncoding = ATextEncodingWrapper::GetEUCTextEncoding();
			return true;
		}
	}
	
	return false;
}

/**
UTF-8でのエラー率計算
@return ascii外文字があればtrue
*/
ABool	AText::CalcUTF8ErrorRate( const AItemCount inLimit, AFloatNumber& outErrorRate ) const
{
	//結果初期化
	outErrorRate = 0.0;
	//ポインタ取得
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	p = textptr.GetPtr();
	//最初のinLimitバイトを検索
	AItemCount	len = textptr.GetByteCount();
	if( len > inLimit )
	{
		len = inLimit;
	}
	
	//各文字ごとのループ
	AItemCount	asciiCount = 0;
	AItemCount	utf8OKCount = 0;
	AItemCount	utf8NGCount = 0;
	for( AIndex pos = 0; pos < len;  )
	{
		AItemCount	bc = 1;
		if( GetUTF8ByteCount(p[pos],bc) == false )
		{
			//最初のバイトのエラー
			utf8NGCount++;
			bc = 1;
		}
		else
		{
			if( bc == 1 )
			{
				//アスキー文字
				asciiCount++;
			}
			else
			{
				//OK
				utf8OKCount++;
				//途中文字のUTF-8形式チェック
				for( AIndex pos2 = 1; pos2 < bc; pos2++ )
				{
					if( pos+pos2 < len )
					{
						AUChar	ch = p[pos+pos2];
						if( (ch&0xC0)!=0x80 )
						{
							utf8OKCount--;
							utf8NGCount++;
							break;
						}
					}
				}
			}
		}
		pos += bc;
	}
	
	//エラー率計算
	if( utf8OKCount+utf8NGCount > 0 )
	{
		outErrorRate = utf8NGCount;
		outErrorRate = outErrorRate/(utf8OKCount+utf8NGCount);
		return true;
	}
	else
	{
		return false;
	}
}

/**
EUCでのエラー率計算
@return ascii外文字があればtrue
*/
ABool	AText::CalcEUCErrorRate( const AItemCount inLimit, AFloatNumber& outErrorRate ) const
{
	//結果初期化
	outErrorRate = 0.0;
	//ポインタ取得
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	p = textptr.GetPtr();
	//最初のinLimitバイトを検索
	AItemCount	len = textptr.GetByteCount();
	if( len > inLimit )
	{
		len = inLimit;
	}
	
	//各文字ごとのループ
	AItemCount	asciiCount = 0;
	AItemCount	eucOKCount = 0;
	AItemCount	eucNGCount = 0;
	for( AIndex pos = 0; pos < len-1; pos++ )
	{
		AUChar	ch = p[pos];
		AUChar	ch2 = p[pos+1];
		//
		if( ch >= 0x80 )
		{
			if( ch >= 0x80 && ch <= 0xA0 && ch != 0x8E && ch!= 0x8F )
			{
				//1バイト目NG
				eucNGCount++;
			}
			else
			{
				pos++;
				//2バイト目判定
				if( ch2 >= 0xA1 )
				{
					eucOKCount++;
				}
				else
				{
					eucNGCount++;
				}
			}
		}
	}
	//エラー率計算
	if( eucOKCount+eucNGCount > 0 )
	{
		outErrorRate = eucNGCount;
		outErrorRate = outErrorRate/(eucOKCount+eucNGCount);
		return true;
	}
	else
	{
		return false;
	}
}

/**
SJISでのエラー率計算
@return ascii外文字があればtrue
*/
ABool	AText::CalcSJISErrorRate( const AItemCount inLimit, AFloatNumber& outErrorRate ) const
{
	//結果初期化
	outErrorRate = 0.0;
	//ポインタ取得
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	p = textptr.GetPtr();
	//最初のinLimitバイトを検索
	AItemCount	len = textptr.GetByteCount();
	if( len > inLimit )
	{
		len = inLimit;
	}
	
	//各文字ごとのループ
	AItemCount	asciiCount = 0;
	AItemCount	sjisOKCount = 0;
	AItemCount	sjisNGCount = 0;
	for( AIndex pos = 0; pos < len-1; pos++ )
	{
		AUChar	ch = p[pos];
		AUChar	ch2 = p[pos+1];
		//
		if( ch <= 0x7F )
		{
			asciiCount++;
		}
		else if( ((ch>=0x81&&ch<=0x9F)||(ch>=0xE0&&ch<=0xFC)) )
		{
			if( ((ch2>=0x40&&ch2<=0x7E)||(ch2>=0x80&&ch2<=0xFC)) )
			{
				pos++;
				//
				sjisOKCount++;
			}
			else
			{
				//
				sjisNGCount++;
			}
		}
		/*
		else
		{
			//半角カタカナ等
			sjisOKCount++;
		}
		*/
	}
	
	//エラー率計算
	if( sjisOKCount+sjisNGCount > 0 )
	{
		outErrorRate = sjisNGCount;
		outErrorRate = outErrorRate/(sjisOKCount+sjisNGCount);
		return true;
	}
	else
	{
		return false;
	}
}

ABool	AText::GuessTextEncodingLegacy( ATextEncoding &outTextEncoding ) const
{
	//MacOSによる推測のテスト→精度低い
#if 0
	OSStatus status = noErr;
	ItemCount actualCount = 0;
	
	ItemCount	encodingsCount = 0;
	status = ::TECCountAvailableTextEncodings(&encodingsCount);
	if( status != noErr )
	{
		return false;
	}
	
	TextEncoding*	encodingArray = (TextEncoding*)::malloc(sizeof(TextEncoding) * encodingsCount);
	
	status = ::TECGetAvailableTextEncodings(encodingArray,encodingsCount,&actualCount);
	if( status != noErr )
	{
		return false;
	}
	/*
	for( AIndex i = 0; i < actualCount; i++ )
	{
		fprintf(stderr,"%8X ",encodingArray[i]);
		AText	name;
		ATextEncodingWrapper::GetTextEncodingName(encodingArray[i],name);
		name.OutputToStderr();
		fprintf(stderr,"\n");
	}
	*/
	TECSnifferObjectRef	sniffer;
	
	TextEncoding	encs[5];
	encs[0] = ATextEncodingWrapper::GetSJISTextEncoding();
	encs[1] = ATextEncodingWrapper::GetJISTextEncoding();
	encs[2] = ATextEncodingWrapper::GetEUCTextEncoding();
	encs[3] = ATextEncodingWrapper::GetUTF8TextEncoding();
	encs[4] = ATextEncodingWrapper::GetUTF16LETextEncoding();
	status = ::TECCreateSniffer(&sniffer,encs,5);
	
	
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	p = textptr.GetPtr();
	AByteCount	len = textptr.GetByteCount();
	if( len > 1024 )   len = 1024;
	
	/*
	TextEncoding*	encodings = (TextEncoding*)::malloc(sizeof(TextEncoding) * actualCount);
	::memcpy(encodings, encodingArray, sizeof(TextEncoding) * actualCount);
	
	ItemCount*	errors = (ItemCount*)malloc(sizeof(ItemCount) * actualCount);
	ItemCount*	features = (ItemCount*)malloc(sizeof(ItemCount) * actualCount);
	*/
	
	TextEncoding	encodings[5];
	encodings[0] = 1;//ATextEncodingWrapper::GetSJISTextEncoding();
	encodings[1] = 0x820;//ATextEncodingWrapper::GetJISTextEncoding();
	encodings[2] = 0x920;//ATextEncodingWrapper::GetEUCTextEncoding();
	encodings[3] = 0x8000103;//ATextEncodingWrapper::GetUTF8TextEncoding();
	encodings[4] = 0x103;//ATextEncodingWrapper::GetUTF16LETextEncoding();
	ItemCount	errors[5];
	ItemCount	features[5];
	
	status = ::TECSniffTextEncoding(sniffer,p,len,encodings,5,errors,5,features,5);
	
	AText	tecname;
	ATextEncodingWrapper::GetTextEncodingName(encodings[0],tecname);
	tecname.OutputToStderr();
	
	/*
	free(encodings);
	free(errors);
	free(features);
	*/
	
	::TECDisposeSniffer(sniffer);
	free(encodingArray);
	return false;
	#endif
	
	ABool	outKakutei = false;//R0045
	outTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
	
	ABool	kanjiExist = false;
	AIndex	end = 1024*1024;//B0015 判定に使用するテキスト：最初の5KB→1MB
	ANumber	jesc = 0, s8182 = 0, eA1A4 = 0;
	ANumber	OutOfEUC = 0;//B0044
	ANumber	uni000D = 0, uni000A = 0, uniE3 = 0;
	AUChar ch, ch2;
	if( end > GetItemCount() )   end = GetItemCount();
	ABool	kanjiMode = false;
	for( AIndex pos = 0; pos < end; pos++ ) 
	{
		ch = Get(pos);
		if( pos+1 < end )
		{
			ch2= Get(pos+1);
		}
		else
		{
			ch2 = 0x20;
		}
		if( kanjiMode )
		{
			kanjiMode = false;
			continue;
		}
		if( ch >= 0x80 )   kanjiExist = true;
		if( ch >= 0x80 && ch <= 0xA0 && ch != 0x8E && ch!= 0x8F )//B0044
		{
			OutOfEUC++;
		}
		if( ch == 0x1B ) 
		{
			if( ch2 == '$' || ch2 == 'K' )   jesc++;
			kanjiExist = true;
		}
		else if( ch == 0x81 || ch == 0x82 ) {
			s8182++;
			kanjiExist = true;
		}
		else if( ch == 0xA1 || ch == 0xA4 || ch == 0xA5 ) {//B0044 A5を追加（カタカナ用）
			eA1A4++;
			kanjiExist = true;
		}
		else if( ch == 0 && ch2 == 13 ) 
		{
			uni000D++;
			kanjiExist = true;
		}
		else if( ch == 0 && ch2 == 10 ) 
		{
			uni000A++;
			kanjiExist = true;
		}
		else if( ch == 0xE3 ) 
		{
			uniE3++;
			kanjiExist = true;
		}
		if( ch >= 0x80 ) 
		{
			kanjiMode = true;
		}
	}
	if( jesc >= 1 )
	{
		outTextEncoding = ATextEncodingWrapper::GetJISTextEncoding();
		outKakutei = true;//R0045  ESC+$/KがあればJISは確定
	}
	else if( ( uni000D >= 1 || uni000A >= 1 ) )
	{
		outTextEncoding = ATextEncodingWrapper::GetUTF16TextEncoding();
		outKakutei = true;//R0045  000D/000AがあればUTF-16確定
	}
	else if( ((uniE3 >= 2 && uniE3 > s8182-1 && uniE3 > eA1A4-1 && uniE3 > uni000D && uniE3 > uni000A) 
			/*|| uniE3 > 10  B0007*/ ) )
	{
		outTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
		if( uniE3 > /*B0274 10*/5 )//R0045  E3が10個以上あればUTF-8確定
		{
			outKakutei = true;
		}
	}
//	else if( s8182 >= 2 && s8182 > eA1A4 && s8182 > uni000D && s8182 > uni000A )   mKanjiCode = kanjiCode_MacCode;B0044
	else if( eA1A4 >= 2 && OutOfEUC == 0 )
	{
		outTextEncoding = ATextEncodingWrapper::GetEUCTextEncoding();
		if( eA1A4 > /*B0274 10*/5 )//R0045 A1/A4が10個以上あればEUC-JP確定
		{
			outKakutei = true;
		}
	}
	else //if(  s8182 > 2 )
	{
		outTextEncoding = ATextEncodingWrapper::GetSJISTextEncoding();
		if(  s8182 > /*B0274 10*/5 )//81/82が10個以上あればShiftJIS確定
		{
			outKakutei = true;
		}
	}
	return outKakutei;
	/*B0044
	EUC: 第１バイト　0xA1-0xFE, 0x8E, 0x8F
	第２バイト　0xA1-0xFE
	SJIS: 第１バイト　0x81-0x9F, 0xE0-0xFC（残りは半角カナ）
	第２バイト　全コード？
	
	暫定対策
	OutOfEUCカウンタ（8E,8F以外の80-A0）が存在したらEUCにはしない
	
	抜本対策案：
	JIS:ESC+$/Kが1つ以上
	EUC:最初から上記ルールで矛盾がないかどうかを調べ続け、矛盾発生時点でEUCフラグおとす
	UCS2:00が2つ以上
	UTF8:E3が2つ以上、かつ81/82よりも多い
	SJIS:それ以外
	*/
}

#pragma mark ===========================

#pragma mark ---テキストエンコーディング／改行コード変換

//各種エンコーディング／改行コードからエンコーディング：UTF8, 改行コード：CRへ変換
//変換エラー時のテキスト内容は破壊される
ABool	AText::ConvertToUTF8CR( const ATextEncoding inSrcTextEncoding, const ABool inUseFallback, ABool& outFallbackUsed )
{
	ABool	result = ConvertToUTF8(inSrcTextEncoding,inUseFallback,outFallbackUsed);
	ConvertReturnCodeToCR();
	return result;
}

//UTF8, CRから各種エンコーディング、改行コードへ変換
//テキスト内容は正常／エラーどちらの場合も元のまま保持される
ABool	AText::ConvertFromUTF8CR( const ATextEncoding inDstTextEncoding, const AReturnCode inDstReturnCode, AText& outText,
								  const ABool inConvertToCanonical ) const //#1044
{
	ConvertReturnCodeFromCR(inDstReturnCode,outText);
	return outText.ConvertFromUTF8(inDstTextEncoding,inConvertToCanonical);//#1044
}

//改行コードをCRへ変換
AReturnCode	AText::ConvertReturnCodeToCR( const AReturnCode inDefaultReturnCode )//#307
{
	ABool	notMixed = true;
	return ConvertReturnCodeToCR(inDefaultReturnCode,notMixed);
}
AReturnCode	AText::ConvertReturnCodeToCR( const AReturnCode inDefaultReturnCode, ABool& outNotMixed ) //#995
{
	outNotMixed = true;//#995
	/*B0313 Delete()すると大きいファイルを開いたときに負荷が大きい
	AIndex pos = 0;
	while( pos < GetItemCount() )
	{
		AUChar	ch = Get(pos);
		pos++;
		if( ch == kUChar_CR )
		{ 
			if( pos < GetItemCount() )
			{
				if( Get(pos) == kUChar_LF )
				{
					Delete(pos,1);
				}
			}
		}
		else if( ch == kUChar_LF )
		{
			Set(pos-1,kUChar_CR);
		}
	}*/
	AItemCount	crCount = 0, crlfCount = 0, lfCount = 0;//#995
	AItemCount	u0085Count = 0, u2028Count = 0, u2029Count = 0;//#1472
	AReturnCode	resultReturnCode = inDefaultReturnCode;//returnCode_CR;//#307
	AItemCount	count = GetItemCount();
	AText	buffer;
	buffer.Reserve(0,count);
	AIndex	dstpos = 0;
	{
		AStTextPtr	dsttextptr(buffer,0,buffer.GetItemCount());
		AUChar*	dsttextp = dsttextptr.GetPtr();
		AStTextPtr	srctextptr(*this,0,GetItemCount());
		AUChar*	srctextp = srctextptr.GetPtr();
		for( AIndex pos = 0; pos < count;  )
		{
			AUChar	ch = srctextp[pos];
			pos++;
			if( ch == kUChar_CR )
			{
				if( pos < count )
				{
					if( srctextp[pos] == kUChar_LF )
					{
						pos++;
						resultReturnCode = returnCode_CRLF;//#307
						crlfCount++;//#995
					}
					else
					{
						resultReturnCode = returnCode_CR;//#995
						crCount++;//#995
					}
				}
				else
				{
					resultReturnCode = returnCode_CR;//#995
					crCount++;//#995
				}
			}
			else if( ch == kUChar_LF )
			{
				ch = kUChar_CR;
				resultReturnCode = returnCode_LF;//#307
				lfCount++;//#995
			}
			//#1472
			//U+2028 (UTF-8: 0xE2 0x80 0xA8) LINE SEPARATOR
			//U+2029 (UTF-8: 0xE2 0x80 0xA9) PARAGRAPH SEPARATOR
			//http://unicode.org/versions/Unicode5.2.0/ch05.pdf
			else if( ch == 0xE2 )
			{
				if( pos+1 < count )
				{
					if( srctextp[pos] == 0x80 && srctextp[pos+1] == 0xA8 )
					{
						pos += 2;
						ch = kUChar_CR;
						u2028Count++;
					}
					else if( srctextp[pos] == 0x80 && srctextp[pos+1] == 0xA9 )
					{
						pos += 2;
						ch = kUChar_CR;
						u2029Count++;
					}
				}
			}
			//#1472
			//U+0085 (UTF-8: 0xC2 0x85) NEXT LINE
			else if( ch == 0xC2 )
			{
				if( pos < count )
				{
					if( srctextp[pos] == 0x85 )
					{
						pos++;
						ch = kUChar_CR;
						u0085Count++;
					}
				}
			}
			dsttextp[dstpos] = ch;
			dstpos++;
		}
	}
	buffer.DeleteAfter(dstpos);
	//#695 高速化 SetText(buffer);
	SwapContent(buffer);//#695
	//#1472
	AItemCount	totalCount = crCount + crlfCount + lfCount + u0085Count + u2028Count + u2029Count;
	//#995
	//決定した改行コードと違う改行コードカウンタが1以上ならエラー（outNotMixedをfalseにする）
	switch(resultReturnCode)
	{
	  case returnCode_CR:
		{
			if( crCount != totalCount )//#1472
			{
				outNotMixed = false;
			}
			break;
		}
	  case returnCode_CRLF:
		{
			if( crlfCount != totalCount )//#1472
			{
				outNotMixed = false;
			}
			break;
		}
	  case returnCode_LF:
		{
			if( lfCount != totalCount )//#1472
			{
				outNotMixed = false;
			}
			break;
		}
	}
	//#1472
	//U+0085/U+2028/U+2029については、すべての改行コードがそれになっている場合のみ、resultReturnCodeに設定する。
	//U+0085
	if( u0085Count > 0 && u0085Count == totalCount )
	{
		resultReturnCode = returnCode_U0085;
	}
	//U+2028
	if( u2028Count > 0 && u2028Count == totalCount )
	{
		resultReturnCode = returnCode_U2028;
	}
	//U+2029
	if( u2029Count > 0 && u2029Count == totalCount )
	{
		resultReturnCode = returnCode_U2029;
	}
	return resultReturnCode;//#307
}

//改行コードをCRからinDstReturnCodeへ変換して、outTextへ格納
void	AText::ConvertReturnCodeFromCR( const AReturnCode inDstReturnCode, AText& outText ) const
{
	outText.DeleteAll();
	if( inDstReturnCode == returnCode_CR )
	{
		outText.SetText(*this);
	}
	else
	{
		//★速度改善検討
		AStTextPtr	textptr(*this,0,GetItemCount());
		AByteCount	bytecount = textptr.GetByteCount();//#598
		AUChar*	ptr = textptr.GetPtr();
		for( AIndex pos = 0; pos < /*#598 GetItemCount()*/bytecount; pos++ )
		{
			unsigned char	ch = (ptr)[pos];
			if( ch == kUChar_CR )
			{
				switch(inDstReturnCode)
				{
				  case returnCode_LF:
					{
						outText.Add(kUChar_LF);
						break;
					}
				  case returnCode_CR:
					{
						outText.Add(kUChar_CR);
						break;
					}
				  case returnCode_CRLF:
					{
						outText.Add(kUChar_CR);
						outText.Add(kUChar_LF);
						break;
					}
					//#1472
				  case returnCode_U0085:
					{
						outText.Add(0xC2);
						outText.Add(0x85);
						break;
					}
					//#1472
				  case returnCode_U2028:
					{
						outText.Add(0xE2);
						outText.Add(0x80);
						outText.Add(0xA8);
						break;
					}
					//#1472
				  case returnCode_U2029:
					{
						outText.Add(0xE2);
						outText.Add(0x80);
						outText.Add(0xA9);
						break;
					}
				}
			}
			else
			{
				outText.Add(ch);
			}
		}
	}
}

//各種エンコーディングからUTF8へ変換
//変換エラー時のテキスト内容は破壊される
ABool	AText::ConvertToUTF8( const ATextEncoding inSrcTextEncoding, const ABool inUseFallback, ABool& outFallbackUsed, const ABool inFromExternalData )//B0400
{
	return ATextEncodingWrapper::ConvertToUTF8(*this,inSrcTextEncoding,inUseFallback,outFallbackUsed,inFromExternalData);//B0400
}
ABool	AText::ConvertToUTF8( const ATextEncoding inSrcTextEncoding )
{
	ABool	fallback = false;
	return ConvertToUTF8(inSrcTextEncoding,true,fallback);
}

//UTF8から各種エンコーディングへ変換
//変換エラー時のテキスト内容は破壊される
ABool	AText::ConvertFromUTF8( const ATextEncoding inDstTextEncoding, const ABool inConvertToCanonical )//#1044
{
	return ATextEncodingWrapper::ConvertFromUTF8(*this,inDstTextEncoding,inConvertToCanonical);//#1044
}

//UTF16からUTF8へ変換
void	AText::ConvertToUTF8FromUTF16()
{
	ABool	fallback;
	if( ATextEncodingWrapper::ConvertToUTF8(*this,ATextEncodingWrapper::GetUTF16TextEncoding(),true,fallback,false) == false )   _ACError("",this);
}

//UTF8からUTF16へ変換
ABool	AText::ConvertFromUTF8ToUTF16()
{
	if( ATextEncodingWrapper::ConvertFromUTF8(*this,ATextEncodingWrapper::GetUTF16TextEncoding()) == false )
	{
		//ここはCUserPane::DrawText()等からコールされるが、文字化けしているときに来うるので、_ACErrorにはしない。
		//CTextDrawData::MakeTextDrawDataWithoutStyle()からコールされたときに失敗すると、無限ループに陥るので、
		//下記のむりやり変換を施す。win
		AText	text;
		for( AIndex i = 0; i < GetItemCount(); i++ )
		{
			if( ATextEncodingWrapper::UTF16IsLE() == true )
			{
				text.Add(Get(i));
				text.Add(0);
			}
			else
			{
				text.Add(0);
				text.Add(Get(i));
			}
		}
		SetText(text);
		return false;
	}
	return true;
}

//API用UnicodeからUTF8へ変換
void	AText::ConvertToUTF8FromAPIUnicode()
{
	ABool	fallback;
	if( ATextEncodingWrapper::ConvertToUTF8(*this,ATextEncodingWrapper::GetUTF16TextEncoding(),true,fallback,false) == false )   _ACError("",this);
}

//UTF8からAPI用Unicodeへ変換
void	AText::ConvertFromUTF8ToAPIUnicode()
{
	if( ATextEncodingWrapper::ConvertFromUTF8(*this,ATextEncodingWrapper::GetUTF16TextEncoding()) == false )   
			{/*_ACError("");ここはCUserPane::DrawText()からコールされるが、文字化けしているときに来うるので、_ACErrorにはしない。検討必要*/}
}

//API用LegacyテキストエンコーディングからUTF8へ変換
void	AText::ConvertToUTF8FromAPILegacy()
{
	ABool	fallback;
	if( ATextEncodingWrapper::ConvertToUTF8(*this,ATextEncodingWrapper::GetSJISTextEncoding(),true,fallback,false) == false )   _ACError("",this);
	//★SJISとは限らないので対策必要
}

//UTF8からAPI用Legacyテキストエンコーディングへ変換
void	AText::ConvertFromUTF8ToAPILegacy()
{
	if( ATextEncodingWrapper::ConvertFromUTF8(*this,ATextEncodingWrapper::GetSJISTextEncoding()) == false )   
			{/*_ACError("");ここはCUserPane::DrawText()からコールされるが、文字化けしているときに来うるので、_ACErrorにはしない。検討必要*/}
	//★SJISとは限らないので対策必要
}

//ATextに格納中のUTF-8文字列に対して、UTF-16文字列へ変換した場合の、UTF-8バイト位置とUTF-16文字位置の対応情報を得る
//outUTF16PosIndexArray: UTF-8文字列でのバイト位置をインデックスとして、対応するUTF-16文字列での位置を格納した配列
//outUTF8PosIndexArray: UTF-16文字列での位置をインデックスとして、対応するUTF-8文字列での最初のバイトの位置を格納した配列
void	AText::GetUTF16TextIndexInfo( AArray<AIndex>& outUTF16PosIndexArray, AArray<AIndex>& outUTF8PosIndexArray ) const
{
	outUTF16PosIndexArray.DeleteAll();
	outUTF8PosIndexArray.DeleteAll();
	AItemCount	utf8textlength = GetItemCount();
	AIndex	utf16pos = 0;
	for( AIndex utf8pos = 0; utf8pos < utf8textlength; )
	{
		//Unicode一文字毎に処理
		//UTF-8のUnicode一文字分に、現在のUTF-16側位置を格納
		AItemCount	bc = GetUTF8ByteCount(Get(utf8pos));
		for( AIndex i = 0; i < bc; i++ )
		{
			outUTF16PosIndexArray.Add(utf16pos);
		}
		//UTF-16位置を更新し、UTF-8のUnicode一文字分に、現在のUTF-8側位置を格納
		utf16pos++;
		outUTF8PosIndexArray.Add(utf8pos);
		//#560 サロゲートペアの場合、UTF-16の2文字で、Unicode一文字(U+010000-U+10FFFF)
		if( bc >= 4 )
		{
			utf16pos++;
			outUTF8PosIndexArray.Add(utf8pos);
		}
		//
		utf8pos += bc;
	}
	outUTF8PosIndexArray.Add(utf8textlength);
	outUTF16PosIndexArray.Add(utf16pos);
}


//UTF8として正しいかチェックする
ABool	AText::CheckUTF8Text( const AItemCount inLimitLength ) const
{
	AItemCount	limitlength = inLimitLength;
	if( limitlength > GetItemCount() )
	{
		limitlength = GetItemCount();
	}
	//高速化のためポインタ使用
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	ptr = textptr.GetPtr();
	AItemCount	count = textptr.GetByteCount();
	for( AIndex pos = 0; pos < limitlength; )
	{
		AItemCount	bc = 1;
		if( GetUTF8ByteCount((ptr)[pos],bc) == false )//#412
		//#412 if( bc == 5 )
		{
			return false;
		}
		pos += bc;
		if( pos > count )
		{
			return false;//B0000
		}
	}
	return true;
}

void	AText::GetUTF16Text( AUniText& outText ) const
{
	outText.DeleteAll();
	AText	text;
	text.SetText(*this);
	text.ConvertFromUTF8ToUTF16();
	AStTextPtr	textptr(text,0,text.GetItemCount());
	outText.InsertFromPtr(0,reinterpret_cast<AUTF16Char*>(textptr.GetPtr()),textptr.GetByteCount()/sizeof(AUTF16Char));
}

/*
void AText::NormalizeTextEncodingFromUTF16()
{
	//暫定
#if IMPLEMENTATION_FOR_MACOSX
	CText	text;
	{
		AStTextPtr	textptr(*this,0,GetItemCount());
		text.AddTextPtr(textptr.GetPtr(),textptr.GetByteCount());
	}
	text.ConvertToUTF8();
	DeleteAll();
	text.Lock();
	AddFromTextPtr(*(text.mTextH),text.mLength);
	text.Unlock();
#endif
}

void AText::GetUTF16Text( AUniText& outText ) const
{
	outText.DeleteAll();
	//暫定
#if IMPLEMENTATION_FOR_MACOSX
	AText	tmp;
	tmp.SetText(*this);
	AStTextPtr	textptr(tmp,0,tmp.GetItemCount());
	CText	text;
	text.AddTextPtr(textptr.GetPtr(),textptr.GetByteCount());
	CText	utf16text;
	text.ConvertFromUTF8(utf16text);
	utf16text.Lock();
	outText.InsertFromPtr(0,reinterpret_cast<AUTF16Char*>(*(utf16text.mTextH)),utf16text.mLength/2);
	utf16text.Unlock();
#endif
#if A_Windows
	wchar_t	str[4096];
	AStTextPtr	textptr(this);
	int unilen = MultiByteToWideChar(65001,0,reinterpret_cast<LPCSTR>(textptr.Get()),
			GetLength(),str,2048);
	outText.Insert(0,str,unilen);
#endif
}

void AText::GetUTF16TextForDisplay( AItemCount inTabWidth, bool inCountAs2Letters0800, bool inDisplayControlCode, AUniText& outText )
{
	GetUTF16Text(outText);
	AItemCount	tabletters = 0;
	AItemCount	tabinc0800 = 1;
	if( inCountAs2Letters0800 )   tabinc0800 = 2;
	for( AIndex pos = 0; pos < outText.GetItemCount(); pos++ )
	{
		AUTF16Char	ch = outText.Get(pos);
		if( ch == 0x0009 )
		{
			outText.Set(pos,0x0020);
			tabletters++;
			AItemCount	spaces = 0;
			while((tabletters%inTabWidth)!=0)
			{
				spaces++;
				tabletters++;
			}
			if( spaces > 0 )
			{
				outText.Reserve(pos,spaces);
				for( AItemCount i = 0; i < spaces; i++ )
				{
					outText.Set(pos+i,0x0020);
				}
			}
			pos += spaces;
		}
		else if( inDisplayControlCode && ch <= 0x001F )
		{
			//暫定
			//スタブ
		}
		else 
		{
			if( ch < 0x0800 )
			{
				tabletters++;
			}
			else
			{
				tabletters += tabinc0800;
			}
		}
	}
}

void AText::GetUTF16TextForDisplayVirtual( AIndex& ioPosition, AItemCount inTabWidth, 
		bool inCountAs2Letters0800, bool inDisplayControlCode, AIndex& ioUniPos, AItemCount& ioTabLetters )
{
	unsigned char	ch = Get(ioPosition);
	AItemCount	bc = GetUTF8ByteCount(ioPosition);
	AItemCount	tabinc0800 = 1;
	if( inCountAs2Letters0800 )   tabinc0800 = 2;
	if( bc < 3 )
	{
		ioUniPos++;
		ioTabLetters++;
	}
	else if( bc < 4 )
	{
		ioUniPos++;
		ioTabLetters += tabinc0800;
	}
	else
	{
		ioUniPos += 2;
		ioTabLetters += tabinc0800;
	}
	if( ch == A_CHAR_TAB )
	{
		while((ioTabLetters%inTabWidth)!=0)
		{
			ioUniPos++;
			ioTabLetters++;
		}
	}
	else if( inDisplayControlCode && ch < 0x1F )
	{
		ioUniPos += 3;
	}
	ioPosition += bc;
}

//暫定
void	AText::ConvertToCanonicalComp()
{
	CText	text, unitext;
	AUtil::GetCTextFromAText(*this,text);
	text.ConvertFromUTF8(unitext);
	unitext.ConvertToCanonicalComp();
	unitext.ConvertToUTF8();
	AUtil::GetATextFromCText(unitext,*this);
}

//暫定
void	AText::ConvertToHFSPlusDecomp()
{
	CText	text, unitext;
	AUtil::GetCTextFromAText(*this,text);
	text.ConvertFromUTF8(unitext);
	unitext.ConvertToHFSPlusDecomp();
	unitext.ConvertToUTF8();
	AUtil::GetATextFromCText(unitext,*this);
}
*/

/*void AText::GetLine( const AIndex& inPosition, AText& outText )
{
	AIndex	pos = inPosition;
	while( pos < GetItemCount() )
	{
		if( mDataPtr[pos] == A_CHAR_LineEnd )
		{
			break;
		}
		pos++;
	}
	GetText(inPosition,pos-inPosition,outText);
}*/

//タブ、スペースを読み飛ばして、それ以外の文字までioPosを進める。
ABool	AText::SkipTabSpace( AIndex& ioPos, const AIndex inEndPos ) const
{
	AStTextPtr	textptr(*this,0,GetItemCount());
	return SkipTabSpace(textptr.GetPtr(),textptr.GetByteCount(),ioPos,inEndPos);
}
ABool	AText::SkipTabSpace( const AUChar* inTextPtr, const AItemCount inTextLen,
							AIndex& ioPos, const AIndex inEndPos ) 
{
	//
	if( inEndPos > inTextLen )
	{
		_ACError("",NULL);
		return false;
	}
	//
	if( ioPos >= inEndPos )   return false;
	AUChar	ch = inTextPtr[ioPos];
	if( ch == kUChar_Space || ch == kUChar_Tab )
	{
		ioPos++;
		for( ;ioPos < inEndPos; ioPos++ )
		{
			ch = inTextPtr[ioPos];
			if( ch == kUChar_Space || ch == kUChar_Tab )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
	if( ioPos >= inEndPos )   return false;
	else return true;
}

//タブ、スペースを読み飛ばして、それ以外の文字までioPosを進める。
ABool	AText::SkipTabSpaceLineEnd( AIndex& ioPos, const AIndex inEndPos ) const
{
	if( ioPos >= inEndPos )   return false;
	AUChar	ch = Get(ioPos);
	if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_LineEnd )
	{
		ioPos++;
		for( ;ioPos < inEndPos; ioPos++ )
		{
			ch = Get(ioPos);
			if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_LineEnd )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
	if( ioPos >= inEndPos )   return false;
	else return true;
}

//#318
/**
タブ・スペース（全角スペース含む）をスキップする
*/
ABool	AText::SkipTabSpaceIncludingZenkakuSpace( AIndex& ioPos, const AIndex inEndPos ) const
{
	if( ioPos >= inEndPos )   return false;
	//全角スペース、半角スペース、タブ
	AText	zenkakuSpaceText;
	zenkakuSpaceText.SetZenkakuSpace();
	AText	spaceText(" ");
	AText	tabText("\t");
	//ioPos以降で次にスペース・タブではない箇所をioPosに設定する
	AText	text;
	GetText(ioPos,GetUTF8ByteCount(ioPos),text);
	if( text.Compare(spaceText) == true || text.Compare(tabText) == true || text.Compare(zenkakuSpaceText) == true )
	{
		ioPos += text.GetItemCount();
		for( ;ioPos < inEndPos; ioPos += text.GetItemCount() )
		{
			GetText(ioPos,GetUTF8ByteCount(ioPos),text);
			if( text.Compare(spaceText) == true || text.Compare(tabText) == true || text.Compare(zenkakuSpaceText) == true )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
	if( ioPos >= inEndPos )   return false;
	else return true;
}

//#318
/**
全角スペースを設定する
*/
void	AText::SetZenkakuSpace()
{
	DeleteAll();
	Add(0xE3);
	Add(0x80);
	Add(0x80);
}

//#942
/**
テキストが全角スペースかどうかを判定
*/
ABool	AText::CompareZenkakuSpace() const
{
	if( GetItemCount() != 3 )
	{
		return false;
	}
	else
	{
		return ( Get(0)==0xE3 && Get(1)==0x80 && Get(2)==0x80 );
	}
}

void	AText::ConvertToCanonicalComp()
{
	/*#1040
	ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(*this);
	ConvertToUTF8FromUTF16();
	*/
	ATextEncodingWrapper::ConvertToCanonicalComp(*this);
}

//UTF16Textから1文字のUTF16Charを取得 R0199
AUTF16Char	AText::GetUTF16CharFromUTF16Text( const AIndex inIndex ) const
{
	if( ATextEncodingWrapper::UTF16IsLE() == true )
	{
		return Get(inIndex) + Get(inIndex+1)*0x100;
	}
	else
	{
		return Get(inIndex)*0x100 + Get(inIndex+1);
	}
}

//win
/**
UTF-16文字を挿入（LE/BE判断し、適切な順番で挿入）
*/
void	AText::InsertUTF16Char( const AIndex inIndex, AUTF16Char inChar )
{
	if( ATextEncodingWrapper::UTF16IsLE() == true )
	{
		Insert1(inIndex,(inChar&0xFF));
		Insert1(inIndex+1,(inChar&0xFF00)/0x100);
	}
	else
	{
		Insert1(inIndex,(inChar&0xFF00)/0x100);
		Insert1(inIndex+1,(inChar&0xFF));
	}
}

//#764
/**
バイナリファイルかどうかを推測する
*/
ABool	AText::SuspectBinaryData() const
{
	//==================UTF8整合性チェック==================
	//最初の10000バイトをチェックし、UTF8としておかしければバイナリファイルとみなす
	if( CheckUTF8Text(10000) == false )
	{
		return true;
	}
	
	//==================NULL文字数チェック==================
	//最初の10000バイトをチェックし、NULL文字が10文字より多くあれば、バイナリファイルとみなす
	AItemCount	checklength = 10000;
	if( checklength > GetItemCount() )
	{
		checklength = GetItemCount();
	}
	//NULL文字数
	AItemCount	nullCharCount = 0;
	//ポインタ取得
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	p = textptr.GetPtr();
	for( AIndex i = 0; i < checklength; i++ )
	{
		AUChar	ch = p[i];
		if( ch == 0x00 )
		{
			nullCharCount++;
		}
		if( nullCharCount > 10 )
		{
			return true;
		}
	}
	
	return false;
}

#pragma mark ===========================

#pragma mark ---テキスト解析

//整数値のParse
ABool	AText::ParseIntegerNumber( AIndex& ioPos, ANumber& outNumber, const ABool inAllowMinus ) const
{
	AIndex	startPos = ioPos;
	ABool	result = false;
	ANumber	number = 0;
	ANumber	mul = 1;
	AIndex	digits = 9;//9桁まで
	for( ; ioPos < GetItemCount(); ioPos++ )
	{
		AUChar	ch = Get(ioPos);
		if( ch == kUChar_Minus && inAllowMinus == true && ioPos == startPos )
		{
			mul = -1;
		}
		else if( ch >= '0' && ch <= '9' )
		{
			number = number*10;
			number += ch-'0';
			result = true;
			digits--;
			if( digits == 0 )   break;
		}
		else break;
	}
	if( result == true )
	{
		outNumber = number*mul;
	}
	return result;
}
ABool	AText::ParseIntegerNumber( ANumber& outNumber, const ABool inAllowMinus ) const
{
	AIndex	pos = 0;
	return ParseIntegerNumber(pos,outNumber,inAllowMinus);
}

//#427
/**
64bit整数値のParse
*/
ABool	AText::ParseIntegerNumber64bit( AIndex& ioPos, ANumber64bit& outNumber, const ABool inAllowMinus ) const
{
	ABool	result = false;
	ANumber64bit	number = 0;
	ANumber64bit	mul = 1;
	AIndex	digits = 18;//18桁まで
	for( ; ioPos < GetItemCount(); ioPos++ )
	{
		AUChar	ch = Get(ioPos);
		if( ch == kUChar_Minus && inAllowMinus == true )
		{
			mul = -1;
		}
		else if( ch >= '0' && ch <= '9' )
		{
			number = number*10;
			number += ch-'0';
			result = true;
			digits--;
			if( digits == 0 )   break;
		}
		else break;
	}
	if( result == true )
	{
		outNumber = number*mul;
	}
	return result;
}
ABool	AText::ParseIntegerNumber64bit( ANumber64bit& outNumber, const ABool inAllowMinus ) const
{
	AIndex	pos = 0;
	return ParseIntegerNumber64bit(pos,outNumber,inAllowMinus);
}

//実数値のParse
ABool	AText::ParseFloatNumber( AIndex& ioPos, AFloatNumber& outNumber, const ABool inAllowMinus ) const
{
	if( ioPos >= GetItemCount() )   return false;
	//整数部分Parse
	ANumber	integer = 0;
	ABool	result = ParseIntegerNumber(ioPos,integer,inAllowMinus);
	outNumber = integer;
	//ピリオドが続く場合以外はここでリターン
	if( ioPos >= GetItemCount() )   return result;
	if( Get(ioPos) != kUChar_Period )   return result;
	//小数点以下Parse
	ioPos++;
	AIndex	prePos = ioPos;
	if( ParseIntegerNumber(ioPos,integer,false) == false )   return true;
	AItemCount	digits = ioPos - prePos;
	AFloatNumber	num = integer;
	num /= pow(10.0,digits);
	outNumber += num;
	return true;
}
ABool	AText::ParseFloatNumber( AFloatNumber& outNumber, const ABool inAllowMinus ) const
{
	AIndex	pos = 0;
	return ParseFloatNumber(pos,outNumber,inAllowMinus);
}

//テキストから整数値を取得（テキストの最初に必ず整数値が存在することがわかっている場合用）
ANumber	AText::GetNumber() const
{
	ANumber	result = 0;
	ParseIntegerNumber(result);
	return result;
}

//テキストから実数値を取得（テキストの最初に必ず実数値が存在することがわかっている場合用）
AFloatNumber	AText::GetFloatNumber() const
{
	AFloatNumber	result = 0.0;
	ParseFloatNumber(result);
	return result;
}

//#427
/**
テキストから64bit整数値を取得
*/
ANumber64bit	AText::GetNumber64bit() const
{
	ANumber64bit	result = 0;
	ParseIntegerNumber64bit(result);
	return result;
}

//テキスト検索（１文字ずつずらしながら一致判定）
//outPosition: この位置以降に一致テキスト
ABool	AText::FindText( AIndex inPosition, const AText& inText, AIndex& outPosition, const AIndex inEndPos,
						const ABool inIgnoreSpace ) const
{
	AItemCount	targetLen = inText.GetLength();
	if( targetLen == 0 )   return false;
	//
	AIndex	endPos = GetItemCount();
	if( inEndPos != kIndex_Invalid )
	{
		endPos = inEndPos;
	}
	//this側のテキストのポインタを取得
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	ptr = textptr.GetPtr();
	//比較対象テキストのポインタを取得
	AStTextPtr	targettextptr(inText,0,inText.GetItemCount());
	AUChar*	targetptr = targettextptr.GetPtr();
	
	//this側テキストの文字ごとループ
	AIndex	pos = inPosition;
	while( pos <= endPos-targetLen )
	{
		ABool	same = true;
		//pos以降の文字列を照合する
		AIndex	targetpos = 0;
		for( AIndex p = pos; p < endPos; p++ )
		{
			//this側文字取得
			AUChar	ch = ptr[p];
			//inIgnoreSpaceがtrueの場合、スペース文字なら、何もせずループ継続
			if( inIgnoreSpace == true )
			{
				if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_LineEnd )
				{
					continue;
				}
			}
			//比較対象テキストの現在位置の文字と比較して、違っていたらループ終了。（不一致）
			if( ch != targetptr[targetpos] )
			{
				same = false;
				break;
			}
			//比較対象テキストの現在位置を次の文字に移動。比較対象テキストを全部読んだらループ終了。（一致）
			targetpos++;
			if( targetpos >= inText.GetItemCount() )
			{
				break;
			}
		}
		if( same == true ) 
		{
			outPosition = pos;
			return true;
		}
		pos++;
	}
	return false;
}

//#1231
/**
C文字列検索
*/
ABool	AText::FindCstring( const AConstCstringPtr inString ) const
{
	AIndex pos = 0;
	return FindCstring(pos,inString,pos);
}

ABool	AText::FindCstring( AIndex inPosition, const AConstCstringPtr inString, AIndex& outPosition, 
						   const ABool inIgnoreSpace, const ABool inProceedPositionAfterMatchedText ) const
{
	AText	target;
	target.SetCstring(inString);
	ABool	result = FindText(inPosition,target,outPosition,kIndex_Invalid,inIgnoreSpace);
	if( result == true && inProceedPositionAfterMatchedText == true )
	{
		outPosition += target.GetItemCount();
	}
	return result;
}

/**
段落計算
*/
void	AText::CalcParagraphBreaks( AArray<AIndex>& outStartPosArray, AArray<AItemCount>& outLengthArray ) const
{
	//結果初期化
	outStartPosArray.DeleteAll();
	outLengthArray.DeleteAll();
	//最初の段落開始位置
	outStartPosArray.Add(0);
	//文字毎ループ
	AIndex	lineStart = 0;
	for( AIndex p = 0; p < GetItemCount(); p++ )
	{
		if( Get(p) == kUChar_LineEnd )
		{
			//段落長さ
			outLengthArray.Add(p+1-lineStart);
			//次の段落開始位置
			lineStart = p+1;
			//段落開始位置
			outStartPosArray.Add(lineStart);
		}
	}
	//最後の段落長さ
	outLengthArray.Add(GetItemCount()-lineStart);
}

//#193
/**
正規表現のグループに一致するテキストを抽出
*/
ABool	AText::ExtractUsingRegExp( const AConstCharPtr inRegExpString, const AIndex inExtractGroup, AText& outText ) const
{
	AText	re;
	re.SetCstring(inRegExpString);
	ARegExp	regexp;
	regexp.SetRE(re,false,false);
	AIndex	pos = 0;
	if( regexp.Match(*this,pos,GetItemCount()) == true )
	{
		if( inExtractGroup == 0 )
		{
			GetText(0,pos,outText);
		}
		else
		{
			AIndex	groupspos = 0, groupepos = 0;
			regexp.GetGroupRange(inExtractGroup-1,groupspos,groupepos);
			GetText(groupspos,groupepos-groupspos,outText);
		}
		return true;
	}
	else
	{
		return false;
	}
}

#pragma mark --- Localized Text

//★AbsFramework用の共通文字列を分ける必要有り

void	AText::SetLocalizedText( const AText& inKey )
{
#if IMPLEMENTATION_FOR_MACOSX
	AStCreateCFStringFromAText	keystrref(inKey);
	CFStringRef	strref = ::CFCopyLocalizedString(keystrref.GetRef(),"");
	if( strref == NULL )   return;
	SetFromCFString(strref);
	::CFRelease(strref);
#elif IMPLEMENTATION_FOR_WINDOWS
	CAppImp::GetLocalizedText(inKey,*this);
#else
	not implemented
#endif
}

void	AText::SetLocalizedText( AConstCharPtr inKey )
{
	AText	keyText;
	keyText.SetCstring(inKey);
	SetLocalizedText(keyText);
}

void	AText::SetLocalizedText( AConstCharPtr inKey, const ANumber inNumber )
{
	AText	keyText;
	keyText.SetCstring(inKey);
	keyText.AddFormattedCstring("%d",inNumber);
	SetLocalizedText(keyText);
}

void	AText::AddLocalizedText( AConstCharPtr inKey )
{
	AText	text;
	text.SetLocalizedText(inKey);
	AddText(text);
}

#pragma mark ---URL

void	AText::SetFTPURL( const AText& inConnectionType, 
						  const AText& inServer, const AText& inUser, const AText& inPath, const APortNumber inPortNumber )//#193
{
	//#193
	//コネクションタイプから"("以降を削除したテキストを://の前に付ける
	SetText(inConnectionType);
	AIndex	foundpos = 0;
	if( FindCstring(0,"(",foundpos) == true )
	{
		DeleteAfter(foundpos);
	}
	ChangeCaseLower();
	//"://"
	AddCstring("://");
	//ユーザー名
	AddText(inUser);
	//サーバー名
	AddCstring("@");
	AddText(inServer);
	//ポート番号 #193
	if( inPortNumber > 0 )
	{
		AddCstring(":");
		AddNumber(inPortNumber);
	}
	//パス
	AddText(inPath);
}

//#193
/**
URL文字列からプロトコルを抽出
*/
void	AText::GetFTPURLProtocol( AText& outProtocol ) const
{
	outProtocol.DeleteAll();
	// "://"の前までを抜き出す
	AIndex	foundpos;
	if( FindCstring(0,"://",foundpos) == false )   return;
	//
	GetText(0,foundpos,outProtocol);
}

void	AText::GetFTPURLServer( AText& outServer ) const
{
	outServer.DeleteAll();
	// "://"以降から、最初の"/"の前までを抜き出す。その中に@があれば@以降、なければ全体を返す。
	AIndex	foundpos;
	if( FindCstring(0,"://",foundpos) == false )   return;
	AIndex	spos = foundpos+3;
	foundpos = GetLength();
	FindCstring(spos,"/",foundpos);
	AText	text;
	AIndex	epos = foundpos;
	GetText(spos,epos-spos,text);
	if( text.FindCstring(0,"@",foundpos) == true )
	{
		text.FindCstring(foundpos+1,"@",foundpos);//ユーザー名に@が含まれる場合の対策
		text.GetText(foundpos+1,text.GetLength()-(foundpos+1),outServer);
	}
	else
	{
		outServer.SetText(text);
	}
	// :以降（ポート番号）を削除する
	if( outServer.FindCstring(0,":",foundpos) == true )
	{
		outServer.DeleteAfter(foundpos);
	}
}

void	AText::GetFTPURLUser( AText& outUser ) const
{
	outUser.DeleteAll();
	// "://"以降から、最初の"/"の前までを抜き出す。その中に@があれば@より前、なければemptyを返す。
	AIndex	foundpos;
	if( FindCstring(0,"://",foundpos) == false )   return;
	AIndex	spos = foundpos+3;
	foundpos = GetLength();
	FindCstring(spos,"/",foundpos);
	AText	text;
	AIndex	epos = foundpos;
	GetText(spos,epos-spos,text);
	if( text.FindCstring(0,"@",foundpos) == true )
	{
		text.FindCstring(foundpos+1,"@",foundpos);//ユーザー名に@が含まれる場合の対策
		text.GetText(0,foundpos,outUser);
	}
}

void	AText::GetFTPURLPath( AText& outPath ) const
{
	outPath.DeleteAll();
	//"://"以降、最初の"/"以降を返す。
	AIndex	foundpos;
	if( FindCstring(0,"://",foundpos) == false )   return;
	AIndex	spos = foundpos+3;
	if( FindCstring(spos,"/",foundpos) == false )   return;
	GetText(foundpos,GetLength()-foundpos,outPath);
}

//#193
/**
URL文字列からポート番号を抽出
*/
APortNumber	AText::GetFTPURLPortNumber() const
{
	AText	server;
	// "://"以降から、最初の"/"の前までを抜き出す。その中に@があれば@以降、なければ全体を返す。
	AIndex	foundpos;
	if( FindCstring(0,"://",foundpos) == false )   return 0;
	AIndex	spos = foundpos+3;
	foundpos = GetLength();
	FindCstring(spos,"/",foundpos);
	AText	text;
	AIndex	epos = foundpos;
	GetText(spos,epos-spos,text);
	if( text.FindCstring(0,"@",foundpos) == true )
	{
		text.FindCstring(foundpos+1,"@",foundpos);//ユーザー名に@が含まれる場合の対策
		text.GetText(foundpos+1,text.GetLength()-(foundpos+1),server);
	}
	else
	{
		server.SetText(text);
	}
	// :より後を取得し、数値化
	if( server.FindCstring(0,":",foundpos) == true )
	{
		server.Delete(0,foundpos+1);
		return server.GetNumber();
	}
	else
	{
		return 0;
	}
}

void	AText::AddPathDelimiter( const AUChar inDelimiter )//win 080713
{
	ABool	exist = false;
	if( GetLength() > 0 )
	{
		if( Get(GetLength()-1) == /*'/' win 080713*/inDelimiter )   exist = true;
	}
	if( exist == false )
	{
		Add(/*'/' win 080713*/inDelimiter);
	}
}

void	AText::CatFTPURLPathText( const AText& inRelativePath )
{
	AddPathDelimiter(kUChar_Slash);//win 080713
	if( inRelativePath.GetLength() == 0 )   return;
	if( inRelativePath.Get(0) == '/' )
	{
		AIndex	pos = 0;
		//"://"があれば、その後まで進める
		AIndex	foundpos;
		if( FindCstring(pos,"://",foundpos) == true )
		{
			pos = foundpos+3;
		}
		//最初にある'/'まで進める
		FindCstring(pos,"/",foundpos);
		pos = foundpos;
		//それ以降を削除して、inRelativePathをくっつける
		DeleteAfter(pos);
		AddText(inRelativePath);
		return;
	}
	else
	{
		//inRelativePathをくっつける位置をcatposに求める。最初は最後尾から始める。
		AIndex	catpos = GetLength();
		//まずcatposを１つ前の/の直後にする。
		{
			catpos -= 2;
			GoBackToChar(catpos,'/');
			catpos++;
		}
		//inRelativePathの最初から"./"と"../"を探していく
		AIndex	pos = 0;
		AItemCount	len = inRelativePath.GetLength();
		while( pos < len )
		{
			//"./"だったら何もせずに進める
			if( pos+1 < len )
			{
				if( inRelativePath.Get(pos) == '.' && inRelativePath.Get(pos+1) == '/' )
				{
					pos += 2;
					continue;
				}
			}
			//"../"だったらcatposを１つ前の/の直後にする。
			if( pos+2 < len )
			{
				if( inRelativePath.Get(pos) == '.' && inRelativePath.Get(pos+1) == '.' && inRelativePath.Get(pos+2) == '/' )
				{
					pos += 3;
					catpos -= 2;
					GoBackToChar(catpos,'/');
					catpos++;
					continue;
				}
			}
			//"./"や"../"以外だったらcatpos以降を削除して、inRelativePathの現在位置以降をくっつける
			DeleteAfter(catpos);
			AText	text = inRelativePath;
			text.Delete(0,pos);
			AddText(text);
			return;
		}
	}
}

//ファイル名・フォルダ名取得
//最後の文字以外で、最後に'/'が出現する箇所より後を、ファイル名・フォルダ名とみなす。ただし、最後が/ならそれは削除
void	AText::GetFilename( AText& outFilename ) const
{
	//
	AIndex pos = GetItemCount()-2;
	if( pos < 0 )   pos = 0;
	for( ; pos > 0; pos -- )
	{
		if( Get(pos) == /*win 080703 kUChar_Slash*/AFileAcc::GetPathDelimiter() )
		{
			pos++;
			break;
		}
	}
	GetText(pos,GetItemCount()-pos,outFilename);
	if( GetLastChar() == /*win 080703 kUChar_Slash*/AFileAcc::GetPathDelimiter() )   outFilename.Delete(outFilename.GetItemCount()-1,1);
}

//#892
/**
最後のパス区切り文字を削除
*/
void	AText::RemoveLastPathDelimiter()
{
	if( GetItemCount() > 0 )
	{
		if( GetLastChar() == AFileAcc::GetPathDelimiter() )   Delete(GetItemCount()-1,1);
	}
}

//#892
/**
最後のパス区切り文字でなければ、パス区切り文字を追加
*/
void	AText::AddLastPathDelimiter()
{
	if( GetItemCount() > 0 )
	{
		if( GetLastChar() != AFileAcc::GetPathDelimiter() )
		{
			Add(AFileAcc::GetPathDelimiter());
		}
	}
	else
	{
		Add(AFileAcc::GetPathDelimiter());
	}
}

#pragma mark ===========================

#pragma mark ---URL

//#427
/**
子URLを設定
*/
void	AText::SetChildURL( const AText& inParentURL, const AText& inChildName )
{
	//親URL設定
	SetText(inParentURL);
	//最後の文字が/でなければ/を追加
	if( GetItemCount() > 0 )
	{
		if( Get(GetItemCount()-1) != '/' )   Add('/');
	}
	//子name追加
	AddText(inChildName);
}

//#427
/**
子URLを設定
*/
void	AText::SetChildURL( const AText& inParentURL, const AConstCstringPtr inChildName )
{
	AText	childname(inChildName);
	SetChildURL(inParentURL,childname);
}

//#361
/**
URLパスについて、親フォルダのパスを取得
*/
ABool	AText::GetParentURL( AText& outParentURL ) const
{
	outParentURL.DeleteAll();
	for( AIndex pos = GetItemCount()-2; pos > 0; pos-- )
	{
		if( Get(pos) == '/' )
		{
			GetText(0,pos+1,outParentURL);
			return true;
		}
	}
	return false;
}

//#363
/**
URLエンコード実行
RFC3986
*/
void	AText::ConvertToURLEncode()
{
	AText	t;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AUChar	ch = Get(i);
		switch(ch)
		{
			//reserved    = gen-delims / sub-delims
			//gen-delims  = ":" / "/" / "?" / "#" / "[" / "]" / "@"
			// sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"/ "*" / "+" / "," / ";" / "="
		  case ':':
		  case '/':
		  case '?':
		  case '#':
		  case '[':
		  case ']':
		  case '@':
		  case '!':
		  case '$':
		  case '&':
		  case '\'':
		  case '(':
		  case ')':
		  case '*':
		  case '+':
		  case ',':
		  case ';':
		  case '=':
			//unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"
		  case '-':
		  case '.':
		  case '_':
		  case '~':
			//%
		  case '%':
			{
				t.Add(ch);
				break;
			}
		  default:
			//ALPHA / DIGIT
			if( (ch>='A'&&ch<='Z') || (ch>='a'&&ch<='z') || (ch>='0'&&ch<='9') )
			{
				t.Add(ch);
				break;
			}
			//%-escape
			else
			{
				t.Add('%');
				t.AddFormattedCstring("%02X",ch);
				break;
			}
		}
	}
	SetText(t);
}

//R0000
void	AText::ConvertToURLEscape()
{
	AText	t;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		t.Add('%');
		t.AddFormattedCstring("%02X",Get(i));
	}
	SetText(t);
}

//#358
/**
*/
void	AText::ConvertFromURLEscape()
{
	AText	t;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		if( Get(i) == '%' && i+2 < GetItemCount() )
		{
			AUChar	ch = (GetIntegerByHexChar(i+1))*0x10 + GetIntegerByHexChar(i+2);
			t.Add(ch);
			i += 2;
		}
		else
		{
			t.Add(Get(i));
		}
	}
	SetText(t);
}

#pragma mark --- Pref Text Escape

void	AText::ConvertToPrefTextEscape()
{
	AText	t;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AUChar	ch = Get(i);
		if( ch == '\r' )
		{
			t.Add('\\');
			t.Add('r');
		}
		else if( ch == '\"' )
		{
			t.Add('\\');
			t.Add('\"');
		}
		else if( ch == '\\' )
		{
			t.Add('\\');
			t.Add('\\');
		}
		else
		{
			t.Add(ch);
		}
	}
	SetText(t);
}

void	AText::ExtractUnescapedTextFromPrefText( AIndex& ioPos, AText& outText, const AUChar inTerminateChar ) const
{
	outText.DeleteAll();
	for( ; ioPos < GetItemCount(); ioPos++ )
	{
		AUChar	ch = Get(ioPos);
		if( ch == inTerminateChar )   break;
		if( ch == '\\' )
		{
			ioPos++;
			ch = Get(ioPos);
			if( ch == 'r' )
			{
				ch = '\r';
			}
		}
		outText.Add(ch);
	}
}

#pragma mark --- 汎用escaped text

/**
改行コード／ダブルクオーテーションをエスケープ
*/
void	AText::ConvertToEscapedText( const ABool inEscapeLineEnd, const ABool inEscapeDoubleQuotation )
{
	AText	t;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AUChar	ch = Get(i);
		if( ch == '\r' && inEscapeLineEnd == true )
		{
			t.Add('\\');
			t.Add('r');
		}
		else if( ch == '\"' && inEscapeDoubleQuotation == true )
		{
			t.Add('\\');
			t.Add('\"');
		}
		else if( ch == '\\' )
		{
			t.Add('\\');
			t.Add('\\');
		}
		else
		{
			t.Add(ch);
		}
	}
	SetText(t);
}

/**
エスケープされた文字列をエスケープ解除
*/
void	AText::ConvertToUnescapedText() 
{
	AText	t;
	for( AIndex pos = 0; pos < GetItemCount(); pos++ )
	{
		AUChar	ch = Get(pos);
		if( ch == '\\' )
		{
			pos++;
			ch = Get(pos);
			if( ch == 'r' )
			{
				ch = '\r';
			}
		}
		t.Add(ch);
	}
	SetText(t);
}


#pragma mark --- 正規表現用escaped text

/**
通常のテキストを正規表現エスケープテキストに変換
*/
void	AText::ConvertToRegExpEscapedText()
{
	AText	t;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AUChar	ch = Get(i);
		switch(ch)
		{
		  case '(':
			{
				t.AddCstring("\\(");
				break;
			}
		  case ')':
			{
				t.AddCstring("\\)");
				break;
			}
		  case '[':
			{
				t.AddCstring("\\[");
				break;
			}
		  case ']':
			{
				t.AddCstring("\\]");
				break;
			}
		  case '{':
			{
				t.AddCstring("\\{");
				break;
			}
		  case '}':
			{
				t.AddCstring("\\}");
				break;
			}
		  case '\\':
			{
				t.AddCstring("\\\\");
				break;
			}
		  case '*':
			{
				t.AddCstring("\\*");
				break;
			}
		  case '+':
			{
				t.AddCstring("\\+");
				break;
			}
		  case '?':
			{
				t.AddCstring("\\?");
				break;
			}
		  case '|':
			{
				t.AddCstring("\\|");
				break;
			}
		  case '^':
			{
				t.AddCstring("\\^");
				break;
			}
		  case '$':
			{
				t.AddCstring("\\$");
				break;
			}
		  case '.':
			{
				t.AddCstring("\\.");
				break;
			}
		  default:
			{
				t.Add(ch);
				break;
			}
		}
	}
	SetText(t);
}

#pragma mark --- Cocoa

//#1034
/**
ATextからNSStringを生成する
*/
NSString*	AText::CreateNSString( const ABool inAutorelease ) const
{
	AStCreateNSStringFromAText	str(*this);
	NSString*	string = str.GetNSString();
	[string retain];
	if( inAutorelease == true )
	{
		[string autorelease];
	}
	return string;
}

//#1034
/**
NSStringからATextへテキスト設定する
*/
void	AText::SetFromNSString( const NSString* inNSString )
{
	ACocoa::SetTextFromNSString(inNSString,*this);
}

#pragma mark デバッグ

void AText::OutputToStderr() const
{
#if IMPLEMENTATION_FOR_MACOSX
	AStCreateCstringFromAText	cstr(*this);
	fprintf(stderr,"%s",cstr.GetPtr());
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	AStCreateWcharStringFromAText	wcstr(*this);
	OutputDebugStringW(wcstr.GetPtr());
#endif
}

void	AText::OutputToStderrHex() const
{
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		fprintf(stderr,"%02X ",Get(i));
	}
	fprintf(stderr,"\n");
}

#pragma mark ===========================
#pragma mark [クラス]AStCreateCstringFromAText
#pragma mark ===========================
//ATextからC文字列ポインタを取得するためのクラス
//（スタック生成専用クラス）

#pragma mark ---コンストラクタ／デストラクタ
//生成時にATextが確定できる場合用
AStCreateCstringFromAText::AStCreateCstringFromAText( const AText& inText )
{
	Create(inText);
}

//生成時にATextが確定できない場合用
AStCreateCstringFromAText::AStCreateCstringFromAText()
{
}

void AStCreateCstringFromAText::Create( const AText& inText )
{
	//mCstringTextへ、元のAText+NULL文字を格納する。
	mCstringText.SetText(inText);
	mCstringText.Add(0);
	//ロック
	mCstringText.LockByArrayPtr();
}

//デストラクタ
AStCreateCstringFromAText::~AStCreateCstringFromAText()
{
	//アンロック
	mCstringText.UnlockByArrayPtr();//検討必要　mCstringTextの生存
}

#pragma mark ---ポインタ取得
//C文字列のポインタとして取得
ACstringPtr AStCreateCstringFromAText::GetPtr()
{
	return reinterpret_cast<ACstringPtr>(mCstringText.GetDataPtr(0));
}

#if IMPLEMENTATION_FOR_WINDOWS
#pragma mark ===========================
#pragma mark [クラス]AStCreateWcharStringFromAText
#pragma mark ===========================
//ATextからwchar_t文字列ポインタを取得するためのクラス
//（スタック生成専用クラス）

//生成時にATextが確定できる場合用
AStCreateWcharStringFromAText::AStCreateWcharStringFromAText( const AText& inText )
{
	Create(inText);
}

//生成時にATextが確定できない場合用
AStCreateWcharStringFromAText::AStCreateWcharStringFromAText()
{
}

void AStCreateWcharStringFromAText::Create( const AText& inText )
{
	mWcharText.SetText(inText);
	mWcharText.Add(0);
	mWcharText.ConvertFromUTF8ToAPIUnicode();
	//ロック
	mWcharText.LockByArrayPtr();
}

//デストラクタ
AStCreateWcharStringFromAText::~AStCreateWcharStringFromAText()
{
	//アンロック
	mWcharText.UnlockByArrayPtr();//検討必要　mCstringTextの生存
}

//Wide文字列のポインタとして取得
const wchar_t* AStCreateWcharStringFromAText::GetPtr()
{
	return reinterpret_cast<wchar_t*>(mWcharText.GetDataPtr(0));
}

//Wide文字列のポインタとして取得
wchar_t* AStCreateWcharStringFromAText::GetWritablePtr()
{
	return reinterpret_cast<wchar_t*>(mWcharText.GetDataPtr(0));
}

//Wide文字列の文字数を取得
int	AStCreateWcharStringFromAText::GetWcharCount() const
{
	return mWcharText.GetItemCount()/sizeof(wchar_t);
}

#endif

#pragma mark ===========================
#pragma mark [クラス]AUniText
#pragma mark ===========================

void	AUniText::InsertFromPtr( AIndex inIndex, AUTF16CharPtr inSrcPtr, AItemCount inItemCount )
{
	//領域確保
	try
	{
		Reserve(inIndex,inItemCount);
	}
	catch(...)
	{
		throw 0;
	}
	//コピー元ポインタ取得
	AStUniTextPtr	textptr(*this,inIndex,inItemCount);
	//コピー実行
	AMemoryWrapper::Memmove(textptr.GetPtr(),inSrcPtr,textptr.GetByteCount());
}






