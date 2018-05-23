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

AFontWrapper

*/

#include "stdafx.h"

#include "AFontWrapper.h"
#include "Cocoa.h"

//#375
AText	AFontWrapper::sDialogDefaultFontName;
AText	AFontWrapper::sAppDefaultFontName;

/**
�_�C�A���O�p�f�t�H���g�t�H���g���擾
*/
void	AFontWrapper::GetDialogDefaultFontName( AText& outText ) 
{
	outText.SetText(sDialogDefaultFontName);
}

/**
�A�v���P�[�V�����f�t�H���g�t�H���g���擾
*/
void	AFontWrapper::GetAppDefaultFontName( AText& outText )
{
	outText.SetText(sAppDefaultFontName);
}

/**
�A�v���P�[�V�����f�t�H���g�t�H���g���ݒ�
*/
void	AFontWrapper::SetAppDefaultFontName( const AText& inText )
{
	sAppDefaultFontName.SetText(inText);
}

//#521
/**
�t�H���g�g�p�\���ǂ������擾
*/
ABool	AFontWrapper::IsFontEnabled( const AText& inFontName )
{
#if IMPLEMENTATION_FOR_MACOSX
	/*#688 
	AFontNumber	fn = 0;
	return (GetFontByName(inFontName,fn));
	*/
	//#1034 return (GetATSUFontIDByName(inFontName)!=0);
	AStCreateNSStringFromAText	fontname(inFontName);
	NSFontDescriptor*	fontDescriptor = [NSFontDescriptor fontDescriptorWithName: fontname.GetNSString() size: 10.0f];
	NSFont*	normalfont = [NSFont fontWithDescriptor:fontDescriptor size:10.0f];
	return (normalfont!=nil);
#else
	//������(windows)
	return true;
#endif
}

#if IMPLEMENTATION_FOR_MACOSX
#pragma mark ===========================
#pragma mark MacOSX�p

//#688
//�t�H���g��-�t�H���gID�e�[�u��
AHashTextArray		gFontArray_Name;
AArray<ATSUFontID>	gFontArray_ATSUFontID;
AThreadMutex		gFontArrayMutex;

//#1034
#if 0
//#688
/**
�t�H���g������ATSUFontID���擾
*/
ATSUFontID	AFontWrapper::GetATSUFontIDByName( const AText& inFontName )
{
	ATSUFontID	fontID = 0;
	//�t�H���g��-�t�H���gID�e�[�u�����猟�����A������΂����Ԃ�
	{
		AStMutexLocker	locker(gFontArrayMutex);
		AIndex	fontIndex = gFontArray_Name.Find(inFontName);
		if( fontIndex != kIndex_Invalid )
		{
			return gFontArray_ATSUFontID.Get(fontIndex);
		}
	}
	//
	AStTextPtr	textptr(inFontName,0,inFontName.GetItemCount());
	OSStatus	err = noErr;
	err = ::ATSUFindFontFromName(textptr.GetPtr(),textptr.GetByteCount(),kFontFullName,
				kFontNoPlatformCode,kFontNoScriptCode,kFontNoLanguageCode,&fontID);
	if( err != noErr )
	{
		err = ::ATSUFindFontFromName(textptr.GetPtr(),textptr.GetByteCount(),kFontNoNameCode,
					kFontNoPlatformCode,kFontNoScriptCode,kFontNoLanguageCode,&fontID);
	}
	if( err != noErr )
	{
		err = ::ATSUFindFontFromName(textptr.GetPtr(),textptr.GetByteCount(),kFontFamilyName,
					kFontNoPlatformCode,kFontNoScriptCode,kFontNoLanguageCode,&fontID);
	}
	//�t�H���g��-�t�H���gID�e�[�u���ɒǉ�
	{
		AStMutexLocker	locker(gFontArrayMutex);
		gFontArray_Name.Add(inFontName);
		gFontArray_ATSUFontID.Add(fontID);
	}
	return fontID;
}
#endif

/*#375
**
�A�v���P�[�V�����f�t�H���g�t�H���g���擾
*
void	AFontWrapper::GetDefaultFontName( AText& outText ) 
{
	GetFontName(::GetAppFont(),outText);
}
*/

#if 0
/**
�A�v���P�[�V�����f�t�H���g�t�H���g�擾�iMacOSX�p�̂݁j
*/
AFontNumber	AFontWrapper::GetDefaultFont()
{
	//#521 return ::GetAppFont();
	//#521 �g�p�\�ȃt�H���g��Ԃ��B
	AFontNumber	fontnum = 0;
	GetFontByName(sAppDefaultFontName,fontnum);
	return fontnum;
}

/*#375
**
�Œ蕶�����f�t�H���g�t�H���g���擾
*
void	AFontWrapper::GetFixWidthFontName( AText& outText )
{
	outText.SetCstring("Monaco");
}
*/

//�t�H���g��-�t�H���g�ԍ��e�[�u��
AHashTextArray	gFontNameTable;//#348
ANumberArray	gFontNumberTable;

/*
Font���j���[�Ɏg�p���悤�Ƃ������AATSUFindFontFromName()�ŁA�ϊ��ł���t�H���g�Ƃł��Ȃ��t�H���g������A���܂������Ȃ�
GetFontFamilyFromMenuSelection()���g�p���邱�Ƃɂ���
���������A�c�[���R�}���h�ŕK�v�Ȃ̂ŕ����B����ς蓮���Ȃ��̂ŁA�ނ���B
*/
/**
�t�H���g������t�H���g�ԍ��擾
*/
ABool	AFontWrapper::GetFontByName( const AText& inText, AFontNumber& outFont )
{
	//#375
	if( inText.Compare("default") == true )
	{
		return GetFontByName(sAppDefaultFontName,outFont);
	}
	/*AText	name;
	name.SetText(inText);
	name.ConvertFromUTF8ToUTF16();
	AStTextPtr	textptr(name,0,name.GetItemCount());
	ATSUFontID	fontID;
	OSStatus	err;
	err = ATSUFindFontFromName(textptr.GetPtr(),textptr.GetByteCount(),kFontFullName,
	kFontNoPlatformCode,kFontNoScriptCode,kFontNoLanguageCode,&fontID);
	if( err != noErr )
	{
		err = ::ATSUFindFontFromName(textptr.GetPtr(),textptr.GetByteCount(),kFontFamilyName,
		kFontNoPlatformCode,kFontNoScriptCode,kFontNoLanguageCode,&fontID);
	}
	if( err != noErr )
	{
		err = ::ATSUFindFontFromName(textptr.GetPtr(),textptr.GetByteCount(),kFontNoNameCode,
		kFontNoPlatformCode,kFontNoScriptCode,kFontNoLanguageCode,&fontID);
	}
	if( err == noErr )
	{
		Style	style;
		::ATSUFontIDtoFOND(fontID,&outFont,&style);
		return true;
	}
	return false;*/
	AIndex	index = gFontNameTable.Find(inText);
	if( index != kIndex_Invalid )
	{
		outFont = gFontNumberTable.Get(index);
		return true;
	}
	else
	{
		/*#521
		outFont = ::GetAppFont();
		return false;
		*/
		//�t�H���g�����݂��Ȃ������ꍇ�A�f�t�H���g�t�H���g��Ԃ��B
		index = gFontNameTable.Find(sAppDefaultFontName);
		if( index != kIndex_Invalid )
		{
			//�f�t�H���g�t�H���g�����݂��Ă����ꍇ
			outFont = gFontNumberTable.Get(index);
		}
		else
		{
			//�f�t�H���g�t�H���g�����݂��Ȃ�������GetAppFont()�������B
			outFont = ::GetAppFont();
		}
		return false;
	}
	/*win
	ABool	result = false;
	ACarbonMenuRef	menuRef = NULL;
	::CreateNewMenu(0,0,&menuRef);
	::CreateStandardFontMenu(menuRef,0,300,0,NULL);
	for( AIndex index = 0; index < ::CountMenuItems(menuRef); index++ )
	{
		AText	text;
		CFStringRef	strref = NULL;
		::CopyMenuItemTextAsCFString(menuRef,1+index,&strref);
		if( strref == NULL )   continue;
		text.SetFromCFString(strref);
		::CFRelease(strref);
		if( text.Compare(inText) == true )
		{
			FMFontFamily	fontFamily;
			FMFontStyle	style;
			::GetFontFamilyFromMenuSelection(menuRef,1+index,&fontFamily,&style);
			outFont = fontFamily;
			result = true;
			break;
		}
	}
	::DisposeMenu(menuRef);
	return result;
	*/
}

/**
�t�H���g�ԍ�����t�H���g���擾
*/
ABool	AFontWrapper::GetFontName( const AFontNumber inFont, AText& outText )
{
	AIndex	index = gFontNumberTable.Find(inFont);
	if( index != kIndex_Invalid )
	{
		gFontNameTable.Get(index,outText);
		return true;
	}
	else
	{
		outText.DeleteAll();
		return false;
	}
	/*win
	ABool	result = false;
	ACarbonMenuRef	menuRef = NULL;
	::CreateNewMenu(0,0,&menuRef);
	::CreateStandardFontMenu(menuRef,0,300,0,NULL);
	for( AIndex index = 0; index < ::CountMenuItems(menuRef); index++ )
	{
		FMFontFamily	fontFamily;
		FMFontStyle	style;
		::GetFontFamilyFromMenuSelection(menuRef,1+index,&fontFamily,&style);
		if( fontFamily == inFont )
		{
			CFStringRef	strref = NULL;
			::CopyMenuItemTextAsCFString(menuRef,1+index,&strref);
			if( strref == NULL )   continue;
			outText.SetFromCFString(strref);
			::CFRelease(strref);
			result = true;
			break;
		}
	}
	::DisposeMenu(menuRef);
	return result;
	*/
}
#endif

extern void	GetCocoaSystemFontName( AText& outFontName );

//win
/**
�t�H���g�e�[�u��������
*/
void	AFontWrapper::Init()
{
	/*#688
	MenuRef	menuRef = NULL;
	::CreateNewMenu(0,0,&menuRef);
	::CreateStandardFontMenu(menuRef,0,300,0,NULL);
	for( AIndex index = 0; index < ::CountMenuItems(menuRef); index++ )
	{
		//
		FMFontFamily	fontFamily = 0;
		FMFontStyle	style = 0;
		::GetFontFamilyFromMenuSelection(menuRef,1+index,&fontFamily,&style);
		CFStringRef	strref = NULL;
		::CopyMenuItemTextAsCFString(menuRef,1+index,&strref);
		if( strref == NULL )   continue;
		AText	text;
		text.SetFromCFString(strref);
		::CFRelease(strref);
		//
		ATSUFontID	fontID = 0;//#521
		if( ::ATSUFONDtoFontID(fontFamily,NULL,&fontID) == noErr )//#521 �t�H���g���݃`�F�b�N
		{
			gFontNameTable.Add(text);
			gFontNumberTable.Add(fontFamily);
		}
	}
	::DisposeMenu(menuRef);
	
	//#375
	//#521 GetFontName(::GetAppFont(),sDialogDefaultFontName);
	//#521 �_�C�A���O�p�t�H���g�ݒ�
	AFontNumber	appFontNum = ::GetAppFont();
	ATSUFontID	fontID = 0;
	if( ::ATSUFONDtoFontID(appFontNum,NULL,&fontID) == noErr )
	{
		//AppFont�t�H���g�����݂��Ă�����A�����ݒ�i���{�ꂾ��"Osaka"��AppFont�����AFontBook��Disable�ɐݒ�ł���j
		GetFontName(appFontNum,sDialogDefaultFontName);
	}
	else
	{
		//AppFont�t�H���g�����݂��Ă��Ȃ�������ACourier��ݒ�
		sDialogDefaultFontName.SetCstring("Courier");
	}
	*/
	
	//sDialogDefaultFontName��system font��ݒ� #688
	GetCocoaSystemFontName(sDialogDefaultFontName);
	
	//sAppDefaultFontName��Monaco��ݒ�
	sAppDefaultFontName.SetCstring("Monaco");
}

#endif

#if IMPLEMENTATION_FOR_WINDOWS

#pragma mark ===========================
#pragma mark Windows�p

/*#375
void	AFontWrapper::GetDialogDefaultFontName( AText& outText ) 
{
	outText.SetCstring("Consolas");//"MS Shell Dlg");
}

void	AFontWrapper::GetAppDefaultFontName( AText& outText )
{
	outText.SetCstring("Consolas");//("Courier New");
}
*/

void	AFontWrapper::Init()
{
	//#375
	AOSVersion	ver = AEnvWrapper::GetOSVersion();
	if( ver >= kOSVersion_Windows7 )
	{
		sDialogDefaultFontName.SetLocalizedText("DefaultDialogFontName_Windows7");
		sAppDefaultFontName.SetLocalizedText("DefaultAppFontName_Windows7");
	}
	else if( ver >= kOSVersion_WindowsVista )
	{
		sDialogDefaultFontName.SetLocalizedText("DefaultDialogFontName_WindowsVista");
		sAppDefaultFontName.SetLocalizedText("DefaultAppFontName_WindowsVista");
	}
	else if( ver >= kOSVersion_WindowsXP )
	{
		sDialogDefaultFontName.SetLocalizedText("DefaultDialogFontName_WindowsXP");
		sAppDefaultFontName.SetLocalizedText("DefaultAppFontName_WindowsXP");
	}
	else
	{
		sDialogDefaultFontName.SetLocalizedText("DefaultDialogFontName_Windows2000");
		sAppDefaultFontName.SetLocalizedText("DefaultAppFontName_Windows2000");
	}
	/*
	sDialogDefaultFontName.SetCstring("Meiryo UI");//MS Shell Dlg");XP��MS UI Gothic�H
	sAppDefaultFontName.SetCstring("Consolas");
	*/
}

#endif


