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

APrefFileImp

*/

#include "stdafx.h"

#include "../AbsBase/ABase.h"

#if IMPLEMENTATION_FOR_MACOSX
#pragma mark ===========================
#pragma mark MacOSX用

APrefFileImp::APrefFileImp()
{
	mDictionary = ::CFDictionaryCreateMutable( kCFAllocatorDefault,0,
				&kCFTypeDictionaryKeyCallBacks,
				&kCFTypeDictionaryValueCallBacks );
}

APrefFileImp::~APrefFileImp()
{
	if( mDictionary != NULL )   	::CFRelease(mDictionary);
}

ABool	APrefFileImp::Load( const AFileAcc& inFile, const ResType& inResType )
{
	if( inFile.Exist() == false )   return false;
	AText	text;
	if( inResType == /*B0000 NULL*/kOSTypeNULL )
	{
		inFile.ReadTo(text);
	}
	else
	{
		if( inFile.ExistResource(inResType,1) == false )   return false;
		inFile.ReadResouceTo(text,inResType,1);
	}
	if( text.GetLength() == 0 )   return false;
	
	CFDataRef	xmlData = NULL;
	{
		AStTextPtr	textptr(text,0,text.GetItemCount());
		xmlData = ::CFDataCreate(kCFAllocatorDefault,textptr.GetPtr(),textptr.GetByteCount());
	}
	if( xmlData == NULL )
	{
		return false;
	}
	
	if( mDictionary != NULL )   	::CFRelease(mDictionary);
	mDictionary = (CFMutableDictionaryRef)::CFPropertyListCreateFromXMLData(kCFAllocatorDefault,xmlData,kCFPropertyListImmutable,NULL);
	::CFRelease(xmlData);
	if( mDictionary == NULL )
	{
		//ここに来るのはXMLファイルが壊れていた時？
		_AError("cannot create list from xml data",NULL);
		mDictionary = ::CFDictionaryCreateMutable( kCFAllocatorDefault,0,
				&kCFTypeDictionaryKeyCallBacks,
				&kCFTypeDictionaryValueCallBacks );
	}
	return true;
}

void	APrefFileImp::Write( const AFileAcc& inFile/*#1034, const ResType& inResType*/ )
{
	inFile.CreateFile();
	CFDataRef	xmlData = ::CFPropertyListCreateXMLData(kCFAllocatorDefault,mDictionary);
	AText	buffer;
	buffer.Reserve(0,::CFDataGetLength(xmlData));
	{
		AStTextPtr	textptr(buffer,0,buffer.GetItemCount());
		CFRange	range;
		range.location = 0;
		range.length = textptr.GetByteCount();
		::CFDataGetBytes(xmlData,range,(UInt8*)(textptr.GetPtr()));
	}
	::CFRelease(xmlData);
	//#1034 if( inResType == /*B0000 NULL*/kOSTypeNULL )
	{
		inFile.WriteFile(buffer);
	}
	/*#1034
	else
	{
		inFile.WriteResouceFile(buffer,inResType,1);
	}
	*/
}

#pragma mark ---Bool

ABool	APrefFileImp::GetData( const AText& inKey, ABool& outData )
{
	ABool	result = false;
	AStCreateCFStringFromAText	keystrref(inKey);
	CFBooleanRef	boo = (CFBooleanRef)::CFDictionaryGetValue(mDictionary,keystrref.GetRef());
	if( boo != NULL )
	{
		if( ::CFGetTypeID(boo) == ::CFBooleanGetTypeID() )
		{
			outData = ::CFBooleanGetValue(boo);
			result = true;
		}
	}
	return result;
}

void	APrefFileImp::SetData( const AText& inKey, const ABool& inData )
{
	AStCreateCFStringFromAText	keystrref(inKey);
	if( inData == true )
	{
		::CFDictionarySetValue(mDictionary,keystrref.GetRef(),kCFBooleanTrue);
	}
	else
	{
		::CFDictionarySetValue(mDictionary,keystrref.GetRef(),kCFBooleanFalse);
	}
}

#pragma mark ---Number

ABool	APrefFileImp::GetData( const AText& inKey, ANumber& outData )
{
	ABool	result = false;
	AStCreateCFStringFromAText	keystrref(inKey);
	CFNumberRef	num = (CFNumberRef)::CFDictionaryGetValue(mDictionary,keystrref.GetRef());
	if( num != NULL )
	{
		if( ::CFGetTypeID(num) == ::CFNumberGetTypeID() )
		{
			SInt64	data = 0;
			::CFNumberGetValue(num,kCFNumberSInt64Type,&data);
			outData = data;
			result = true;
		}
	}
	return result;
}

void	APrefFileImp::SetData( const AText& inKey, const ANumber& inData )
{
	AStCreateCFStringFromAText	keystrref(inKey);
	SInt64	data = inData;
	CFNumberRef	num = ::CFNumberCreate(kCFAllocatorDefault,kCFNumberSInt64Type,&data);
	::CFDictionarySetValue(mDictionary,keystrref.GetRef(),num);
}

#pragma mark ---FloatNumber

ABool	APrefFileImp::GetData( const AText& inKey, AFloatNumber& outData )
{
	ABool	result = false;
	AStCreateCFStringFromAText	keystrref(inKey);
	CFNumberRef	num = (CFNumberRef)::CFDictionaryGetValue(mDictionary,keystrref.GetRef());
	if( num != NULL )
	{
		if( ::CFGetTypeID(num) == ::CFNumberGetTypeID() )
		{
			Float32	data = 0;
			::CFNumberGetValue(num,kCFNumberFloat32Type,&data);
			outData = data;
			result = true;
		}
	}
	return result;
}

void	APrefFileImp::SetData( const AText& inKey, const AFloatNumber& inData )
{
	AStCreateCFStringFromAText	keystrref(inKey);
	Float32	data = inData;
	CFNumberRef	num = ::CFNumberCreate(kCFAllocatorDefault,kCFNumberFloat32Type,&data);
	::CFDictionarySetValue(mDictionary,keystrref.GetRef(),num);
}

#pragma mark ---Text

ABool	APrefFileImp::GetData( const AText& inKey, AText& outData )
{
	ABool	result = false;
	AStCreateCFStringFromAText	keystrref(inKey);
	CFStringRef str = (CFStringRef)::CFDictionaryGetValue(mDictionary,keystrref.GetRef());
	if( str != NULL )
	{
		if( ::CFGetTypeID(str) == ::CFStringGetTypeID() )
		{
			outData.SetFromCFString(str);
			result = true;
		}
	}
	return result;
}

void	APrefFileImp::SetData( const AText& inKey, const AText& inData )
{
	AStCreateCFStringFromAText	keystrref(inKey);
	AStCreateCFStringFromAText	data(inData);
	::CFDictionarySetValue(mDictionary,keystrref.GetRef(),::CFStringCreateCopy(kCFAllocatorDefault,data.GetRef()));
}

#pragma mark ---Point

ABool	APrefFileImp::GetData( const AText& inKey, APoint& outData )
{
	ABool	result = true;
	AText	key;
	
	key.SetText(inKey);
	key.AddCstring(" H");
	if( GetData(key,outData.x) == false )   result = false;
	
	key.SetText(inKey);
	key.AddCstring(" V");
	if( GetData(key,outData.y) == false )   result = false;
	return result;
}

void	APrefFileImp::SetData( const AText& inKey, const APoint& inData )
{
	AText	key;
	
	key.SetText(inKey);
	key.AddCstring(" H");
	SetData(key,inData.x);
	
	key.SetText(inKey);
	key.AddCstring(" V");
	SetData(key,inData.y);
}

#pragma mark ---Rect

ABool	APrefFileImp::GetData( const AText& inKey, ARect& outData )
{
	ABool	result = true;
	AText	key;
	
	key.SetText(inKey);
	key.AddCstring(" Left");
	if( GetData(key,outData.left) == false )   result = false;
	
	key.SetText(inKey);
	key.AddCstring(" Top");
	if( GetData(key,outData.top) == false )   result = false;
	
	key.SetText(inKey);
	key.AddCstring(" Right");
	if( GetData(key,outData.right) == false )   result = false;
	
	key.SetText(inKey);
	key.AddCstring(" Bottom");
	if( GetData(key,outData.bottom) == false )   result = false;
	return result;
}

void	APrefFileImp::SetData( const AText& inKey, const ARect& inData )
{
	AText	key;
	
	key.SetText(inKey);
	key.AddCstring(" Left");
	SetData(key,inData.left);
	
	key.SetText(inKey);
	key.AddCstring(" Top");
	SetData(key,inData.top);
	
	key.SetText(inKey);
	key.AddCstring(" Right");
	SetData(key,inData.right);
	
	key.SetText(inKey);
	key.AddCstring(" Bottom");
	SetData(key,inData.bottom);
}

/*win
#pragma mark ---Font

ABool	APrefFileImp::GetData( const AText& inKey, AFont& outData )
{
	//暫定
	ANumber	data;
	ABool	result = GetData(inKey,data);
	if( result == true )   outData = data;
	return result;
}

void	APrefFileImp::SetData( const AText& inKey, const AFont& inData )
{
	ANumber	data = inData;
	SetData(inKey,data);
}
*/
#pragma mark ---Color

ABool	APrefFileImp::GetData( const AText& inKey, AColor& outData )
{
	ABool	result = true;
	AText	key;
	ANumber	num;
	
	key.SetText(inKey);
	key.AddCstring(" Red");
	if( GetData(key,num) == false )   result = false;
	else	outData.red = num;
	
	key.SetText(inKey);
	key.AddCstring(" Green");
	if( GetData(key,num) == false )   result = false;
	else	outData.green = num;
	
	key.SetText(inKey);
	key.AddCstring(" Blue");
	if( GetData(key,num) == false )   result = false;
	else	outData.blue = num;
	
	return result;
}

void	APrefFileImp::SetData( const AText& inKey, const AColor& inData )
{
	AText	key;
	ANumber	num;
	
	key.SetText(inKey);
	key.AddCstring(" Red");
	num = inData.red;
	SetData(key,num);
	
	key.SetText(inKey);
	key.AddCstring(" Green");
	num = inData.green;
	SetData(key,num);
	
	key.SetText(inKey);
	key.AddCstring(" Blue");
	num = inData.blue;
	SetData(key,num);
}

#pragma mark ---TextArray

ABool	APrefFileImp::GetData( const AText& inKey, ATextArray& outData )
{
	ABool	result = false;
	AStCreateCFStringFromAText	keystrref(inKey);
	CFArrayRef array = (CFArrayRef)::CFDictionaryGetValue(mDictionary,keystrref.GetRef());
	if( array != NULL )
	{
		outData.DeleteAll();
		CFIndex	arrayCount = ::CFArrayGetCount(array);
		for( CFIndex i = 0; i < arrayCount; i++ )
		{
			CFStringRef	str = (CFStringRef)::CFArrayGetValueAtIndex(array,i);
			if( str != NULL )
			{
				if( ::CFGetTypeID(str) == ::CFStringGetTypeID() )
				{
					AText	text;
					text.SetFromCFString(str);
					outData.Add(text);
				}
			}
		}
		result = true;
	}
	return result;
}

void	APrefFileImp::SetData( const AText& inKey, const ATextArray& inData )
{
	CFMutableArrayRef	array = ::CFArrayCreateMutable(kCFAllocatorDefault,0,&kCFTypeArrayCallBacks);
	if( array == NULL )   return;
	for( AIndex i = 0; i < inData.GetItemCount(); i++ )
	{
		AText	text;
		inData.Get(i,text);
		//CFStringRef	str = AUtil::CreateCFStringFromText(text);
		AStTextPtr	textptr(text,0,text.GetItemCount());
		CFStringRef		str = ::CFStringCreateWithBytes(NULL,textptr.GetPtr(),textptr.GetByteCount(),kCFStringEncodingUTF8,false);
		if( str != NULL )
		{
			::CFArrayAppendValue(array,str);
		}
	}
	AStCreateCFStringFromAText	keystrref(inKey);
	::CFDictionarySetValue(mDictionary,keystrref.GetRef(),array);
}

#pragma mark ---BoolArray

ABool	APrefFileImp::GetData( const AText& inKey, ABoolArray& outData )
{
	ABool	result = false;
	AStCreateCFStringFromAText	keystrref(inKey);
	CFArrayRef array = (CFArrayRef)::CFDictionaryGetValue(mDictionary,keystrref.GetRef());
	if( array != NULL )
	{
		outData.DeleteAll();
		CFIndex	arrayCount = ::CFArrayGetCount(array);
		for( CFIndex i = 0; i < arrayCount; i++ )
		{
			CFBooleanRef	b = (CFBooleanRef)::CFArrayGetValueAtIndex(array,i);
			if( b != NULL )
			{
				if( ::CFGetTypeID(b) == ::CFBooleanGetTypeID() )
				{
					ABool	data = ::CFBooleanGetValue(b);
					outData.Add(data);
				}
			}
		}
		result = true;
	}
	return result;
}

void	APrefFileImp::SetData( const AText& inKey, const ABoolArray& inData )
{
	CFMutableArrayRef	array = ::CFArrayCreateMutable(kCFAllocatorDefault,0,&kCFTypeArrayCallBacks);
	if( array == NULL )   return;
	for( AIndex i = 0; i < inData.GetItemCount(); i++ )
	{
		if( inData.Get(i) == true )
		{
			::CFArrayAppendValue(array,kCFBooleanTrue);
		}
		else
		{
			::CFArrayAppendValue(array,kCFBooleanFalse);
		}
	}
	AStCreateCFStringFromAText	keystrref(inKey);
	::CFDictionarySetValue(mDictionary,keystrref.GetRef(),array);
}

#pragma mark ---NumberArray

ABool	APrefFileImp::GetData( const AText& inKey, ANumberArray& outData )
{
	ABool	result = false;
	AStCreateCFStringFromAText	keystrref(inKey);
	CFArrayRef array = (CFArrayRef)::CFDictionaryGetValue(mDictionary,keystrref.GetRef());
	if( array != NULL )
	{
		outData.DeleteAll();
		CFIndex	arrayCount = ::CFArrayGetCount(array);
		for( CFIndex i = 0; i < arrayCount; i++ )
		{
			CFNumberRef	num = (CFNumberRef)::CFArrayGetValueAtIndex(array,i);
			if( num != NULL )
			{
				if( ::CFGetTypeID(num) == ::CFNumberGetTypeID() )
				{
					SInt64	data = 0;
					::CFNumberGetValue(num,kCFNumberSInt64Type,&data);
					outData.Add(data);
				}
			}
		}
		result = true;
	}
	return result;
}

void	APrefFileImp::SetData( const AText& inKey, const ANumberArray& inData )
{
	CFMutableArrayRef	array = ::CFArrayCreateMutable(kCFAllocatorDefault,0,&kCFTypeArrayCallBacks);
	if( array == NULL )   return;
	for( AIndex i = 0; i < inData.GetItemCount(); i++ )
	{
		SInt64	data = inData.Get(i);
		CFNumberRef	num = ::CFNumberCreate(kCFAllocatorDefault,kCFNumberSInt64Type,&data);
		if( num != NULL )
		{
			::CFArrayAppendValue(array,num);
		}
	}
	AStCreateCFStringFromAText	keystrref(inKey);
	::CFDictionarySetValue(mDictionary,keystrref.GetRef(),array);
}

#pragma mark ---ColorArray

ABool	APrefFileImp::GetData( const AText& inKey, AColorArray& outData )
{
	ABool	result = true;
	AText	key;
	ANumberArray	red, green, blue;
	
	key.SetText(inKey);
	key.AddCstring(" Red");
	if( GetData(key,red) == false )   result = false;
	
	key.SetText(inKey);
	key.AddCstring(" Green");
	if( GetData(key,green) == false )   result = false;
	
	key.SetText(inKey);
	key.AddCstring(" Blue");
	if( GetData(key,blue) == false )   result = false;
	
	if( result == true )
	{
		outData.DeleteAll();
		for( AIndex i = 0; i < red.GetItemCount(); i++ )
		{
			AColor	color;
			color.red = red.Get(i);
			color.green = green.Get(i);
			color.blue = blue.Get(i);
			outData.Add(color);
		}
	}
	return result;
}

void	APrefFileImp::SetData( const AText& inKey, const AColorArray& inData )
{
	AText	key;
	ANumberArray	red, green, blue;
	
	for( AIndex i = 0; i < inData.GetItemCount(); i++ )
	{
		AColor	data = inData.Get(i);
		red.Add(data.red);
		green.Add(data.green);
		blue.Add(data.blue);
	}
	
	key.SetText(inKey);
	key.AddCstring(" Red");
	SetData(key,red);
	
	key.SetText(inKey);
	key.AddCstring(" Green");
	SetData(key,green);
	
	key.SetText(inKey);
	key.AddCstring(" Blue");
	SetData(key,blue);
}

#endif

#if IMPLEMENTATION_FOR_WINDOWS

#pragma mark ===========================
#pragma mark Windows用

//使用しない

APrefFileImp::APrefFileImp()
{
	_ACError("not implemented",NULL);
}

APrefFileImp::~APrefFileImp()
{
	_ACError("not implemented",NULL);
}

ABool	APrefFileImp::Load( const AFileAcc& inFile )
{
	_ACError("not implemented",NULL);
	return false;
}

void	APrefFileImp::Write( const AFileAcc& inFile )
{
	_ACError("not implemented",NULL);
}

#pragma mark ---Bool

ABool	APrefFileImp::GetData( const AText& inKey, ABool& outData )
{
	_ACError("not implemented",NULL);
	return false;
}

void	APrefFileImp::SetData( const AText& inKey, const ABool& inData )
{
	_ACError("not implemented",NULL);
}

#pragma mark ---Number

ABool	APrefFileImp::GetData( const AText& inKey, ANumber& outData )
{
	_ACError("not implemented",NULL);
	return false;
}

void	APrefFileImp::SetData( const AText& inKey, const ANumber& inData )
{
	_ACError("not implemented",NULL);
}

#pragma mark ---FloatNumber

ABool	APrefFileImp::GetData( const AText& inKey, AFloatNumber& outData )
{
	_ACError("not implemented",NULL);
	return false;
}

void	APrefFileImp::SetData( const AText& inKey, const AFloatNumber& inData )
{
	_ACError("not implemented",NULL);
}

#pragma mark ---Text

ABool	APrefFileImp::GetData( const AText& inKey, AText& outData )
{
	_ACError("not implemented",NULL);
	return false;
}

void	APrefFileImp::SetData( const AText& inKey, const AText& inData )
{
	_ACError("not implemented",NULL);
}

#pragma mark ---Point

ABool	APrefFileImp::GetData( const AText& inKey, APoint& outData )
{
	_ACError("not implemented",NULL);
	return false;
}

void	APrefFileImp::SetData( const AText& inKey, const APoint& inData )
{
	_ACError("not implemented",NULL);
}

#pragma mark ---Rect

ABool	APrefFileImp::GetData( const AText& inKey, ARect& outData )
{
	_ACError("not implemented",NULL);
	return false;
}

void	APrefFileImp::SetData( const AText& inKey, const ARect& inData )
{
	_ACError("not implemented",NULL);
}

#pragma mark ---Color

ABool	APrefFileImp::GetData( const AText& inKey, AColor& outData )
{
	_ACError("not implemented",NULL);
	return false;
}

void	APrefFileImp::SetData( const AText& inKey, const AColor& inData )
{
	_ACError("not implemented",NULL);
}

#pragma mark ---TextArray

ABool	APrefFileImp::GetData( const AText& inKey, ATextArray& outData )
{
	_ACError("not implemented",NULL);
	return false;
}

void	APrefFileImp::SetData( const AText& inKey, const ATextArray& inData )
{
	_ACError("not implemented",NULL);
}

#pragma mark ---BoolArray

ABool	APrefFileImp::GetData( const AText& inKey, ABoolArray& outData )
{
	_ACError("not implemented",NULL);
	return false;
}

void	APrefFileImp::SetData( const AText& inKey, const ABoolArray& inData )
{
	_ACError("not implemented",NULL);
}

#pragma mark ---NumberArray

ABool	APrefFileImp::GetData( const AText& inKey, ANumberArray& outData )
{
	_ACError("not implemented",NULL);
	return false;
}

void	APrefFileImp::SetData( const AText& inKey, const ANumberArray& inData )
{
	_ACError("not implemented",NULL);
}

#pragma mark ---ColorArray

ABool	APrefFileImp::GetData( const AText& inKey, AColorArray& outData )
{
	_ACError("not implemented",NULL);
	return false;
}

void	APrefFileImp::SetData( const AText& inKey, const AColorArray& inData )
{
	_ACError("not implemented",NULL);
}

#endif

