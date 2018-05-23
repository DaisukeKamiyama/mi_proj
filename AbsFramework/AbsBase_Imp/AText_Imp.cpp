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

AText_Imp
テキスト

*/

#include "stdafx.h"

#include "../AbsBase/ABase.h"

#pragma mark ===========================
#pragma mark [クラス]AText
#pragma mark ===========================
#pragma mark ---ATextのうちMacOSX専用部分
/**
Pstringからの生成
*/
AText::AText( Str255 inString ):AArray<AUChar>(NULL,256,256)
{
	SetPstring(inString);
}

/**
Pstring取得
*/
void	AText::GetPstring( Str255 outString, const ANumber& inMaxLength ) const
{
	ANumber	length = GetItemCount();
	if( length > inMaxLength )   length = inMaxLength;
	if( length < 0 || length > 255 )
	{
		_ACError("invalid length",this);
		outString[0] = 0;
		return;
	}
	AStTextPtr	textptr(*this,0,GetItemCount());
	AMemoryWrapper::Memmove(&(outString[1]),textptr.GetPtr(),length);
	outString[0] = length;
}

/**
Pstringから設定
*/
void	AText::SetPstring( const Str255 inString )
{
	DeleteAll();
	InsertFromTextPtr(0,&(inString[1]),inString[0]);
}

/**
CFStringから設定
*/
void	AText::SetFromCFString( const CFStringRef inString )
{
	DeleteAll();
	if( inString == NULL )   return;
	CFRange	range;
	range.location = 0;
	range.length = ::CFStringGetLength(inString);
	UniChar *buffer = static_cast<UniChar*>(malloc(range.length*sizeof(UniChar)));
	::CFStringGetCharacters(inString,range,buffer);
	SetTextFromUTF16CharPtr(buffer,range.length*sizeof(UniChar));//#200 メソッド名変更
	::free(buffer);
}

/**
CFStringRef取得

注意：コールした側で::CFRelease()必要！
*/
CFStringRef AText::CreateCFString() const
{
	AStTextPtr	textptr(*this,0,GetItemCount());
	return ::CFStringCreateWithBytes(NULL,textptr.GetPtr(),textptr.GetByteCount(),kCFStringEncodingUTF8,false);
}

/**
Text->OSType
*/
OSType	AText::GetOSTypeFromText() const
{
	AText	text;
	text.SetText(*this);
	while( text.GetItemCount() < 4 )
	{
		text.Add(0);
	}
	if( text.GetItemCount() != 4 )   return NULL;
	OSType	d, e;
	d = (OSType)(text.Get(0));
	d *= 0x1000000;
	e = (OSType)(text.Get(1));
	e *= 0x10000;
	d += e;
	e = (OSType)(text.Get(2));
	e *= 0x100;
	d += e;
	e = (OSType)(text.Get(3));
	d += e;
	return d;
}

/**
OSType->Text
*/
void	AText::SetFromOSType( const OSType inOSType )
{
	DeleteAll();
	if( (inOSType&0xff000000)/0x1000000 != 0 )
	{
		Add((inOSType&0xff000000)/0x1000000);
	}
	if( (inOSType&  0xff0000)/  0x10000 != 0 )
	{
		Add((inOSType&  0xff0000)/  0x10000);
	}
	if( (inOSType&    0xff00)/    0x100 != 0 )
	{
		Add((inOSType&    0xff00)/    0x100);
	}
	if( (inOSType&      0xff) != 0 )
	{
		Add((inOSType&      0xff));
	}
}

#pragma mark ===========================
#pragma mark [クラス]AStCreateCFStringFromAText
#pragma mark ===========================
//ATextからCFStringを取得するためのクラス(Mac専用)
//（スタック生成専用クラス）

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AStCreateCFStringFromAText::AStCreateCFStringFromAText( const AText& inText )
{
	//CFString取得
	{//#598 arrayptrの有効範囲を最小にする
		AStTextPtr	textptr(inText,0,inText.GetItemCount());
		mStringRef = ::CFStringCreateWithBytes(NULL,textptr.GetPtr(),textptr.GetByteCount(),kCFStringEncodingUTF8,false);
	}
	if( mStringRef == NULL )
	{
		//正しいUTF8コードでなかった場合等、CFString生成不可となるので、文字列長0のCFStringを生成して返す
		//_ACError("cannot create CFString",NULL);
		mStringRef = ::CFStringCreateWithCString(NULL,"",kCFStringEncodingUTF8);
		//文字列長0のCFStringすら生成できない場合（ふつうありえない）throwしとく
		if( mStringRef == NULL )   _ACThrow("cannot create CFString with null string",NULL);
	}
}

/**
デストラクタ
*/
AStCreateCFStringFromAText::~AStCreateCFStringFromAText()
{
	//CFString解放
	if( mStringRef != NULL )   ::CFRelease(mStringRef);
}

/**
CFStringRef取得
*/
CFStringRef	AStCreateCFStringFromAText::GetRef()
{
	return mStringRef;
}

#pragma mark ===========================
#pragma mark [クラス]AStCreateCFMutableStringFromAText
#pragma mark ===========================
//ATextからCFStringを取得するためのクラス(Mac専用)
//（スタック生成専用クラス）

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AStCreateCFMutableStringFromAText::AStCreateCFMutableStringFromAText( const AText& inText )
{
	//CFString取得
	CFStringRef	strref = NULL;
	{//#598 arrayptrの有効範囲を最小にする
		AStTextPtr	textptr(inText,0,inText.GetItemCount());
		strref = ::CFStringCreateWithBytes(NULL,textptr.GetPtr(),textptr.GetByteCount(),kCFStringEncodingUTF8,false);
	}
	if( strref == NULL )
	{
		//正しいUTF8コードでなかった場合等、CFString生成不可となるので、文字列長0のCFStringを生成して返す
		//_ACError("cannot create CFString",NULL);
		strref = ::CFStringCreateWithCString(NULL,"",kCFStringEncodingUTF8);
		//文字列長0のCFStringすら生成できない場合（ふつうありえない）throwしとく
		if( strref == NULL )   _ACThrow("cannot create CFString with null string",NULL);
	}
	mStringRef = ::CFStringCreateMutableCopy(NULL,0,strref);
	::CFRelease(strref);
}

/**
デストラクタ
*/
AStCreateCFMutableStringFromAText::~AStCreateCFMutableStringFromAText()
{
	//CFString解放
	if( mStringRef != NULL )   ::CFRelease(mStringRef);
}

/**
CFStringRef取得
*/
CFMutableStringRef	AStCreateCFMutableStringFromAText::GetRef()
{
	return mStringRef;
}

