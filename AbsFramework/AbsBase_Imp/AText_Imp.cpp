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
�e�L�X�g

*/

#include "stdafx.h"

#include "../AbsBase/ABase.h"

#pragma mark ===========================
#pragma mark [�N���X]AText
#pragma mark ===========================
#pragma mark ---AText�̂���MacOSX��p����
/**
Pstring����̐���
*/
AText::AText( Str255 inString ):AArray<AUChar>(NULL,256,256)
{
	SetPstring(inString);
}

/**
Pstring�擾
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
Pstring����ݒ�
*/
void	AText::SetPstring( const Str255 inString )
{
	DeleteAll();
	InsertFromTextPtr(0,&(inString[1]),inString[0]);
}

/**
CFString����ݒ�
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
	SetTextFromUTF16CharPtr(buffer,range.length*sizeof(UniChar));//#200 ���\�b�h���ύX
	::free(buffer);
}

/**
CFStringRef�擾

���ӁF�R�[����������::CFRelease()�K�v�I
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
#pragma mark [�N���X]AStCreateCFStringFromAText
#pragma mark ===========================
//AText����CFString���擾���邽�߂̃N���X(Mac��p)
//�i�X�^�b�N������p�N���X�j

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AStCreateCFStringFromAText::AStCreateCFStringFromAText( const AText& inText )
{
	//CFString�擾
	{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
		AStTextPtr	textptr(inText,0,inText.GetItemCount());
		mStringRef = ::CFStringCreateWithBytes(NULL,textptr.GetPtr(),textptr.GetByteCount(),kCFStringEncodingUTF8,false);
	}
	if( mStringRef == NULL )
	{
		//������UTF8�R�[�h�łȂ������ꍇ���ACFString�����s�ƂȂ�̂ŁA������0��CFString�𐶐����ĕԂ�
		//_ACError("cannot create CFString",NULL);
		mStringRef = ::CFStringCreateWithCString(NULL,"",kCFStringEncodingUTF8);
		//������0��CFString���琶���ł��Ȃ��ꍇ�i�ӂ����肦�Ȃ��jthrow���Ƃ�
		if( mStringRef == NULL )   _ACThrow("cannot create CFString with null string",NULL);
	}
}

/**
�f�X�g���N�^
*/
AStCreateCFStringFromAText::~AStCreateCFStringFromAText()
{
	//CFString���
	if( mStringRef != NULL )   ::CFRelease(mStringRef);
}

/**
CFStringRef�擾
*/
CFStringRef	AStCreateCFStringFromAText::GetRef()
{
	return mStringRef;
}

#pragma mark ===========================
#pragma mark [�N���X]AStCreateCFMutableStringFromAText
#pragma mark ===========================
//AText����CFString���擾���邽�߂̃N���X(Mac��p)
//�i�X�^�b�N������p�N���X�j

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AStCreateCFMutableStringFromAText::AStCreateCFMutableStringFromAText( const AText& inText )
{
	//CFString�擾
	CFStringRef	strref = NULL;
	{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
		AStTextPtr	textptr(inText,0,inText.GetItemCount());
		strref = ::CFStringCreateWithBytes(NULL,textptr.GetPtr(),textptr.GetByteCount(),kCFStringEncodingUTF8,false);
	}
	if( strref == NULL )
	{
		//������UTF8�R�[�h�łȂ������ꍇ���ACFString�����s�ƂȂ�̂ŁA������0��CFString�𐶐����ĕԂ�
		//_ACError("cannot create CFString",NULL);
		strref = ::CFStringCreateWithCString(NULL,"",kCFStringEncodingUTF8);
		//������0��CFString���琶���ł��Ȃ��ꍇ�i�ӂ����肦�Ȃ��jthrow���Ƃ�
		if( strref == NULL )   _ACThrow("cannot create CFString with null string",NULL);
	}
	mStringRef = ::CFStringCreateMutableCopy(NULL,0,strref);
	::CFRelease(strref);
}

/**
�f�X�g���N�^
*/
AStCreateCFMutableStringFromAText::~AStCreateCFMutableStringFromAText()
{
	//CFString���
	if( mStringRef != NULL )   ::CFRelease(mStringRef);
}

/**
CFStringRef�擾
*/
CFMutableStringRef	AStCreateCFMutableStringFromAText::GetRef()
{
	return mStringRef;
}

