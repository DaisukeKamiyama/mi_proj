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

CAppleEvent

*/

#include "CAppleEvent.h"

#pragma mark ===========================
#pragma mark [�N���X]CAppleEventReader
#pragma mark ===========================

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^

/**
�R���X�g���N�^
*/
CAppleEventReader::CAppleEventReader( const AppleEvent* inAppleEvent ) : mAppleEvent(inAppleEvent)
{
}
/**
�f�X�g���N�^
*/
CAppleEventReader::~CAppleEventReader()
{
}
 
#pragma mark ===========================

#pragma mark --- Param�擾

/**
ParamDesc�擾
*/
void	CAppleEventReader::GetParamDesc( const AEKeyword inAEKeyword, const DescType inDesiredType, CAEDesc& outAEDesc ) const
{
	OSStatus	err = noErr;
	err = ::AEGetParamDesc(mAppleEvent,inAEKeyword,inDesiredType,outAEDesc.GetDescPtrForWrite());
}

/**
inDesiredType��Param��Text�Ƃ��Ď擾
*/
ABool	CAppleEventReader::GetParamText( const AEKeyword inAEKeyword, const DescType inDesiredType, AText& outText ) const
{
	OSStatus	err = noErr;
	
	Size	actualSize = 0;
	if( ::AEGetParamPtr(mAppleEvent,inAEKeyword,inDesiredType,NULL,NULL,0,&actualSize) != noErr )   return false;
	if( actualSize == 0 )   return false;
	outText.Reserve(0,actualSize);
	{//#598
		AStTextPtr	textptr(outText,0,outText.GetItemCount());
		err = ::AEGetParamPtr(mAppleEvent,inAEKeyword,inDesiredType,NULL,textptr.GetPtr(),textptr.GetByteCount(),&actualSize);
	}
	if( outText.GetItemCount() != actualSize )   _ACError("",NULL);
	return true;
}

/**
�t�@�C����Param���擾
*/
ABool	CAppleEventReader::GetParamFile( const AEKeyword inAEKeyword, AFileAcc& outFile ) const
{
	OSStatus	err = noErr;
	
	Size	actualSize = 0;
	if( ::AEGetParamPtr(mAppleEvent,inAEKeyword,typeFSRef,NULL,NULL,0,&actualSize) != noErr )//#528   return false;
	{
		//#528 �t�@�C�������݂��Ȃ��ƒ���FSRef�͎擾�ł��Ȃ�
		//'furl'���擾
		actualSize = 0;
		err = ::AEGetParamPtr(mAppleEvent,inAEKeyword,typeFileURL,NULL,NULL,0,&actualSize);
		if( err != noErr )   return false;
		if( actualSize == 0 )   return false;
		ABool	result = false;
		UInt8*	dataPtr = (UInt8*)::malloc(actualSize);
		if( dataPtr != NULL )
		{
			//URL��dataPtr�֊i�[
			err = ::AEGetParamPtr(mAppleEvent,inAEKeyword,typeFileURL,NULL,dataPtr,actualSize,&actualSize);
			//CFURL����
			CFURLRef	urlref = ::CFURLCreateWithBytes(NULL,dataPtr,actualSize,kCFStringEncodingUTF8,NULL);
			if( urlref != NULL )
			{
				//�e�t�H���_��FSRef�擾���A�t�@�C�����Ńp�X�����
				//�Q�l�Fhttp://developer.apple.com/library/mac/#technotes/tn/tn2022.html
				CFURLRef	parentURL = ::CFURLCreateCopyDeletingLastPathComponent(NULL,urlref);
				if( parentURL != NULL )
				{
					FSRef	parentDirectoryFSRef;
					::CFURLGetFSRef(parentURL,&parentDirectoryFSRef);
					CFStringRef	fileNameRef = ::CFURLCopyLastPathComponent(urlref);
					if( fileNameRef != NULL )
					{
						AText	filename;
						filename.SetFromCFString(fileNameRef);
						::CFRelease(fileNameRef);
						//
						AFileAcc	parent;
						parent.SpecifyByFSRef(parentDirectoryFSRef);
						outFile.SpecifyChild(parent,filename);
						result = true;
					}
					::CFRelease(parentURL);
				}
				::CFRelease(urlref);
			}
			::free((void*)dataPtr);
		}
		return result;
	}
	if( actualSize == 0 )   return false;
	FSRef	fsref;
	err = ::AEGetParamPtr(mAppleEvent,inAEKeyword,typeFSRef,NULL,&fsref,sizeof(fsref),&actualSize);
	outFile.SpecifyByFSRef(fsref);
	return true;
}

/**
typeLongInteger��Param���擾
*/
ABool	CAppleEventReader::GetParamInteger( const AEKeyword inAEKeyword, ANumber& outInteger ) const
{
	OSStatus	err = noErr;
	
	Size	actualSize = 0;
	if( ::AEGetParamPtr(mAppleEvent,inAEKeyword,/*#1034 typeLongInteger*/typeSInt32,NULL,NULL,0,&actualSize) != noErr )   return false;
	if( actualSize == 0 )   return false;
	SInt32	num;
	err = ::AEGetParamPtr(mAppleEvent,inAEKeyword,/*#1034 typeLongInteger*/typeSInt32,NULL,&num,sizeof(num),&actualSize);
	outInteger = num;
	return true;
}

/**
typeEnumeration��Param���擾
*/
ABool	CAppleEventReader::GetParamEnum( const AEKeyword inAEKeyword, DescType& outEnum ) const
{
	Size	actualSize = 0;
	if( ::AEGetParamPtr(mAppleEvent,inAEKeyword,typeEnumeration,NULL,NULL,0,&actualSize) != noErr )   return false;
	if( actualSize == 0 )   return false;
	::AEGetParamPtr(mAppleEvent,inAEKeyword,typeEnumeration,NULL,&outEnum,sizeof(outEnum),&actualSize);
	return true;
}

/**
typeType��Param���擾
*/
ABool	CAppleEventReader::GetParamOSType( const AEKeyword inAEKeyword, OSType& outType ) const
{
	Size	actualSize = 0;
	if( ::AEGetParamPtr(mAppleEvent,inAEKeyword,typeType,NULL,NULL,0,&actualSize) != noErr )   return false;
	if( actualSize == 0 )   return false;
	::AEGetParamPtr(mAppleEvent,inAEKeyword,typeType,NULL,&outType,sizeof(outType),&actualSize);
	return true;
}

/**
pContents���擾
*/
void	CAppleEventReader::GetPropertyText( const AEKeyword inAEKeyword, AText& outText ) const
{
	AEDesc	props;
	::AECreateDesc(typeNull,NULL,0,&props);
	::AEGetParamDesc(mAppleEvent,keyAEPropData,typeAERecord,&props);
	if( props.descriptorType != typeNull ) 
	{
		long	max;
		::AECountItems(&props,&max);
		for( long i = 1; i <= max; i++ ) 
		{
			DescType	keyword;
			AEDesc	rec;
			::AECreateDesc(typeNull,NULL,0,&rec);
			::AEGetNthDesc(&props,i,typeWildCard,&keyword,&rec);
			if( keyword == pContents ) 
			{
				CAEDescReader	descreader(&rec);
				descreader.GetText(outText);
			}
			::AEDisposeDesc(&rec);
		}
	}
	::AEDisposeDesc(&props);
}

#pragma mark ===========================
#pragma mark [�N���X]CAEDescReader
#pragma mark ===========================

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^

/**
�R���X�g���N�^
*/
CAEDescReader::CAEDescReader( const AEDesc* inAEDesc ) : mDesc(inAEDesc)
{
}
/**
�f�X�g���N�^
*/
CAEDescReader::~CAEDescReader()
{
}

#pragma mark ===========================

#pragma mark --- Desc���e�擾

/**
Text�擾
*/
void	CAEDescReader::GetText( AText& outText ) const
{
	AEDesc	desc;
	OSErr	err = ::AECoerceDesc(mDesc,typeUTF8Text,&desc);
	if( err != noErr )
	{
		//��������
	}
	Size	size = ::AEGetDescDataSize(&desc);
	if( size > 0 )
	{
		outText.Reserve(0,size);
		AStTextPtr	textptr(outText,0,outText.GetItemCount());
		::AEGetDescData(&desc,textptr.GetPtr(),textptr.GetByteCount());
	}
	::AEDisposeDesc(&desc);
}

/**
File�擾
*/
void	CAEDescReader::GetFile( AFileAcc& outFile ) const
{
	AEDesc	desc;
	OSErr	err = ::AECoerceDesc(mDesc,typeFSRef,&desc);
	if( err != noErr )
	{
		//��������
	}
	Size	size = ::AEGetDescDataSize(&desc);
	if( size > 0 )
	{
		FSRef	fsref;
		::AEGetDescData(&desc,&fsref,sizeof(fsref));
		outFile.SpecifyByFSRef(fsref);
	}
	::AEDisposeDesc(&desc);
}

#pragma mark ===========================
#pragma mark [�N���X]CAEDesc
#pragma mark ===========================

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^

/**
�R���X�g���N�^
*/
CAEDesc::CAEDesc()
{
	::AECreateDesc(typeNull,NULL,0,&mDesc);
}
/**
�f�X�g���N�^
*/
CAEDesc::~CAEDesc()
{
	::AEDisposeDesc(&mDesc);
}

#pragma mark ===========================

#pragma mark --- 

/**
�������ݗp�|�C���^�擾
*/
AEDesc*	CAEDesc::GetDescPtrForWrite()
{
	ClearData();
	return &mDesc;
}

/*
������
*/
void	CAEDesc::ClearData()
{
	if( ::AEGetDescDataSize(&mDesc) > 0 )
	{
		::AEDisposeDesc(&mDesc);
		::AECreateDesc(typeNull,NULL,0,&mDesc);
	}
}

#pragma mark ===========================

#pragma mark --- List

/**
ListItem�擾
*/
void	CAEDesc::GetListItem( const AIndex inIndex, AFileAcc& outFile ) const
{
	AEKeyword	aekeyword;
	DescType	desctype;
	FSRef	fsref;
	Size	actualSize;
	::AEGetNthPtr(&mDesc,1+inIndex,typeFSRef,&aekeyword,&desctype,(Ptr)&fsref,sizeof(FSRef),&actualSize);
	outFile.SpecifyByFSRef(fsref);
}

/**
ListItem���擾
*/
AItemCount	CAEDesc::GetListItemCount() const
{
	long	count;
	::AECountItems(&mDesc,&count);
	return count;
}

#pragma mark ===========================

#pragma mark --- �f�[�^�ݒ�

/*
Text�ݒ�
*/
void	CAEDesc::SetText( const AText& inText, AEDesc& ioDesc )
{
	AText	text;
	text.SetText(inText);
	text.ConvertFromUTF8ToUTF16();
	AStTextPtr	textptr(text,0,text.GetItemCount());
	::AECreateDesc(typeUnicodeText,textptr.GetPtr(),textptr.GetByteCount(),&ioDesc);
}




