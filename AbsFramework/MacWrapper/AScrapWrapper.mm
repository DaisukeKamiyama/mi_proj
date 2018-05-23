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

AScrapWrapper

*/

#include "AScrapWrapper.h"
//#include "../Abs/AApp.h"
#include "../Frame.h"
#import <Cocoa/Cocoa.h>
#include "../AbsBase/Cocoa.h"

//B0370 ScrapRef���w�肷��C���^�[�t�F�C�X�ɕύX

/**
AScrapRef���^�C�v�`�F�b�N��ANSPasteboard�ɃL���X�g
*/
NSPasteboard*	GetNSPasteboardFromAScrapRef( const AScrapRef inScrapRef )
{
	//�|�C���^NULL�`�F�b�N
	if( inScrapRef == NULL )
	{
		_ACThrow("AScrapRef is NULL",NULL);
	}
	//id�փL���X�g
	id	pasteboard = static_cast<id>(inScrapRef);
	//�N���X�^�C�v�`�F�b�N
	if( [pasteboard isKindOfClass:[NSPasteboard class]] == NO )
	{
		_ACThrow("Class error",NULL);
	}
	//NSPasteboard*�փL���X�g
	return static_cast<NSPasteboard*>(inScrapRef);
}

#pragma mark ===========================

#pragma mark ---ScrapData�����݂��邩�ǂ����𒲂ׂ�

//AScrapRef�w�肠��

/**
ScrapData�����݂��邩�ǂ������擾�iAScrapRef�w�肠��j
*/
ABool	AScrapWrapper::ExistScrapData( const AScrapRef inScrapRef, const AScrapType inScrapType )
{
	if( inScrapRef == NULL )   return false;
	
	return ([[GetNSPasteboardFromAScrapRef(inScrapRef) types] 
		containsObject:ACocoa::GetPasteboardTypeNSString(inScrapType)] == YES);
	
	/*#688
	if( inScrapRef == NULL )   return false;
	ScrapFlavorFlags	flags = 0;
	return (::GetScrapFlavorFlags(inScrapRef,inScrapType,&flags) == noErr);
	*/
}

/**
Text��ScrapData�����݂��邩�ǂ������擾�iAScrapRef�w�肠��j
*/
ABool	AScrapWrapper::ExistTextScrapData( const AScrapRef inScrapRef )
{
	return (ExistScrapData(inScrapRef,kScrapType_UnicodeText)==true);
}

//AScrapRef�w��Ȃ�

/**
ScrapData�����݂��邩�ǂ������擾�iAScrapRef�w��Ȃ��j
*/
ABool	AScrapWrapper::ExistClipboardScrapData( const AScrapType inScrapType )
{
	return ExistScrapData([NSPasteboard generalPasteboard],inScrapType);
	/*#688
	OSStatus	err = noErr;
	
	AScrapRef	scrap = NULL;
	err = ::GetCurrentScrap(&scrap);
	if( err != noErr )   _ACErrorNum("GetCurrentScrap() returned error: ",err,NULL);
	return ExistScrapData(scrap,inScrapType);
	*/
}

/**
Text��ScrapData�����݂��邩�ǂ������擾�iAScrapRef�w��Ȃ��j
*/
ABool	AScrapWrapper::ExistClipboardTextScrapData()
{
	return ExistTextScrapData([NSPasteboard generalPasteboard]);
	/*#688
	OSStatus	err = noErr;
	
	AScrapRef	scrap = NULL;
	err = ::GetCurrentScrap(&scrap);
	if( err != noErr )   _ACErrorNum("GetCurrentScrap() returned error: ",err,NULL);
	return ExistTextScrapData(scrap);
	*/
}

#pragma mark ===========================

#pragma mark ---SetScrap

//AScrapRef�w�肠��

/**
ScrapData�ݒ�iAScrapRef�w�肠��j
*/
void	AScrapWrapper::SetScrapData( const AScrapRef inScrapRef, const AScrapType inScrapType, const AText& inText )
{
	//inScrapRef��NULL�Ȃ牽�����Ȃ�
	if( inScrapRef == NULL )   return;
	
	//
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	NSData*	data = [[NSData alloc] initWithBytes:textptr.GetPtr() length:textptr.GetByteCount()];
	NSString*	type = ACocoa::GetPasteboardTypeNSString(inScrapType);
	[GetNSPasteboardFromAScrapRef(inScrapRef) setData:data forType:type];
	
	/*#688
	OSStatus	err = noErr;
	
	if( inScrapRef == NULL )   return;
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	err = ::PutScrapFlavor(inScrapRef,inScrapType,kScrapFlavorMaskNone,textptr.GetByteCount(),textptr.GetPtr());
	if( err != noErr )   _ACErrorNum("PutScrapFlavor() returned error: ",err,NULL);
	*/
}

/**
Text��ScrapData�ݒ�iAScrapRef�w�肠��j
*/
void	AScrapWrapper::SetTextScrap( const AScrapRef inScrapRef, const AText& inText, const ABool inConvertToCanonicalComp )//#1044 #688 , const ATextEncoding inLegacyTextEncoding )
{
	//inScrapRef��NULL�Ȃ牽�����Ȃ�
	if( inScrapRef == NULL )   return;
	
	//#931
	//���s�R�[�h��LF�֕ϊ�
	AText	text;
	text.SetText(inText);
	text.ConvertLineEndToLF();
	
	//inConvertToCanonicalComp��true�Ȃ�canonical comp�`���֕ϊ� #1044
	if( inConvertToCanonicalComp == true )
	{
		text.ConvertToCanonicalComp();
	}
	
	//�e�L�X�g��pasteboard�ɐݒ�
	AStCreateNSStringFromAText	str(text);
	[GetNSPasteboardFromAScrapRef(inScrapRef) setString:str.GetNSString() forType:NSStringPboardType];
	
	/*#688
	AText	utf16text;
	utf16text.SetText(inText);
	utf16text.ConvertFromUTF8ToUTF16();
	SetScrapData(inScrapRef,kScrapType_UnicodeText,utf16text);
	
	AText	legacyText;
	legacyText.SetText(inText);
	legacyText.ConvertFromUTF8(inLegacyTextEncoding);//B0337
	SetScrapData(inScrapRef,kScrapType_Text,legacyText);
	*/
}

#if 0
//#564
/**
Text��ScrapData�ݒ�iAScrapRef�w�肠��EUnicode�̂݁j
*/
void	AScrapWrapper::SetTextScrap( const AScrapRef inScrapRef, const AText& inText )
{
	//
	if( inScrapRef == NULL )   return;
	
	//
	AStCreateNSStringFromAText	str(inText);
	[GetNSPasteboardFromAScrapRef(inScrapRef) setString:str.GetNSString() forType:NSStringPboardType];
	
	/*#688
	AText	utf16text;
	utf16text.SetText(inText);
	utf16text.ConvertFromUTF8ToUTF16();
	SetScrapData(inScrapRef,kScrapType_UnicodeText,utf16text);
	*/
}
#endif

//AScrapRef�w��Ȃ�

/**
ScrapData�ݒ�iAScrapRef�w��Ȃ��j
*/
void	AScrapWrapper::SetClipboardScrapData( const AScrapType inScrapType, const AText& inText, const ABool inClearScrap )
{
	//inClearScrap��true�Ȃ�pasteboard���܂��N���A
	if( inClearScrap == true )
	{
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_6 )
		{
			[[NSPasteboard generalPasteboard] clearContents];
		}
		else
		{
			NSString*	type = ACocoa::GetPasteboardTypeNSString(inScrapType);
			[[NSPasteboard generalPasteboard] declareTypes:[NSArray arrayWithObject:type] owner:nil];
		}
	}
	//�N���b�v�{�[�h�ݒ�
	SetScrapData([NSPasteboard generalPasteboard],inScrapType,inText);
	
	/*#688
	OSStatus	err = noErr;
	
	if( inClearScrap == true )
	{
		err = ::ClearCurrentScrap();
		if( err != noErr )   _ACErrorNum("ClearCurrentScrap() returned error: ",err,NULL);
	}
	AScrapRef	scrap = NULL;
	err = ::GetCurrentScrap(&scrap);
	if( err != noErr )   _ACErrorNum("GetCurrentScrap() returned error: ",err,NULL);
	SetScrapData(scrap,inScrapType,inText);
	*/
}

/**
Text��ScrapData�ݒ�iAScrapRef�w��Ȃ��j
*/
void	AScrapWrapper::SetClipboardTextScrap( const AText& inText, const ABool inClearScrap, const ABool inConvertToCanonicalComp )//#1044
{
	//inClearScrap��true�Ȃ�pasteboard���܂��N���A
	if( inClearScrap == true )
	{
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_6 )
		{
			[[NSPasteboard generalPasteboard] clearContents];
		}
		else
		{
			[[NSPasteboard generalPasteboard] declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:nil];
		}
	}
	//�N���b�v�{�[�h�ݒ�
	SetTextScrap([NSPasteboard generalPasteboard],inText,inConvertToCanonicalComp);//#1044
	
	/*#688
	OSStatus	err = noErr;
	
	if( inClearScrap == true )
	{
		err = ::ClearCurrentScrap();
		if( err != noErr )   _ACErrorNum("ClearCurrentScrap() returned error: ",err,NULL);
	}
	AScrapRef	scrap = NULL;
	err = ::GetCurrentScrap(&scrap);
	if( err != noErr )   _ACErrorNum("GetCurrentScrap() returned error: ",err,NULL);
	SetTextScrap(scrap,inText,inLegacyTextEncoding);
	*/
}


#pragma mark ===========================

#pragma mark ---GetScrap

//AScrapRef�w�肠��

#if 0
/**
ScrapData�擾�iAScrapRef�w�肠��j
*/
void	AScrapWrapper::GetScrapData( const AScrapRef inScrapRef, const AScrapType inScrapType, AText& outText )
{
	//inScrapRef��NULL�Ȃ牽�����Ȃ�
	if( inScrapRef == NULL )   return;
	
	NSString*	str = [GetNSPasteboardFromAScrapRef(inScrapRef) stringForType:(ACocoa::GetPasteboardTypeNSString(inScrapType))];
	ACocoa::SetTextFromNSString(str,outText);
	
	/*#688
	OSStatus	err = noErr;
	
	outText.DeleteAll();
	if( inScrapRef == NULL )   return;
	Size	scrapSize = 0;//B0377 0��������ΕK�v�B
	err = ::GetScrapFlavorSize(inScrapRef,inScrapType,&scrapSize);
	//fprintf(stderr,"size:%d(%X) ",scrapSize,scrapSize);
	if( scrapSize <= 0 )   return;
	outText.Reserve(0,scrapSize);
	AStTextPtr	textptr(outText,0,outText.GetItemCount());
	err = ::GetScrapFlavorData(inScrapRef,inScrapType,&scrapSize,textptr.GetPtr());
	*/
}
#endif

/**
Text��ScrapData�擾�iAScrapRef�w�肠��j
*/
void	AScrapWrapper::GetTextScrap( const AScrapRef inScrapRef, AText& outText )
{
	//�w��X�N���b�v����e�L�X�g�擾
	NSString*	str = [GetNSPasteboardFromAScrapRef(inScrapRef) stringForType:NSStringPboardType];
	ACocoa::SetTextFromNSString(str,outText);
	//���s�R�[�h��CR�ɕϊ�
	outText.ConvertReturnCodeToCR();
	
	/*#688
	outText.DeleteAll();
	//�܂�Unicode�Ŏ���
	if( ExistScrapData(inScrapRef,kScrapType_UnicodeText) == true )
	{
		GetScrapData(inScrapRef,kScrapType_UnicodeText,outText);
		if( inConvertToCanonicalComp == true )
		{
			ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(outText);
		}
		outText.ConvertToUTF8FromUTF16();
		outText.ConvertReturnCodeToCR();//B0439
	}
	//B0377 OSX10.5�̏ꍇ�A'ut16'�����������ꍇ�iFinder����̃t�@�C���̃R�s�[���y�[�X�g���j�A'utxt'�Ɏ����ϊ�����Ȃ��B
	if( outText.GetItemCount() == 0 )
	{
		if( ExistScrapData(inScrapRef,'ut16') == true )
		{
			GetScrapData(inScrapRef,'ut16',outText);
			outText.ConvertToUTF8(ATextEncodingWrapper::GetUTF16BETextEncoding());
			outText.ConvertFromUTF8(ATextEncodingWrapper::GetUTF16TextEncoding());
			if( inConvertToCanonicalComp == true )
			{
				ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(outText);
			}
			outText.ConvertToUTF8FromUTF16();
			outText.ConvertReturnCodeToCR();//B0439
		}
	}
	//Unicode�ł̃e�L�X�g�f�[�^�����������ꍇ�́A���K�V�[�Ŏ���
	if( outText.GetItemCount() == 0 )
	{
		if( ExistScrapData(inScrapRef,kScrapType_Text) == true )
		{
			GetScrapData(inScrapRef,kScrapType_Text,outText);
			//PageMill���A�Ō��NULL��������������A�v�������������߂̑΍�
			if( outText.GetItemCount() > 0 )
			{
				if( outText.Get(outText.GetItemCount()-1) == 0 )
				{
					outText.Delete1(outText.GetItemCount()-1);
				}
			}
			//B0342 0x80�iMacJapanese�Ńo�b�N�X���b�V���H�j��ForceASCII��UTF-8�ɕϊ������U+F87F�Ƃ����S�~�����Ă��܂��̂ŁA�����
			if( ::GetTextEncodingBase(inLegacyTextEncoding) == kTextEncodingMacJapanese )
			{
				AItemCount	count = outText.GetItemCount();
				for( AIndex i = 0; i < count; i++ )
				{
					AUChar	ch = outText.Get(i);
					
					//B0366 �u���v���u�\�v�ɉ�������C���iShiftJIS�Ȃ̂łQ�o�C�g�������f�K�v�j
					if( (ch>=0x81&&ch<=0x9F)||(ch>=0xE0&&ch<=0xFC) )
					{
						i++;
						continue;
					}
					
					if( ch == 0x80 )
					{
						outText.Set(i,kUChar_Backslash);
					}
				}
			}
			
			//#307 ATextEncoding	resultEncoding;
			//B0400 AUtil::ConvertToUTF8CRAnyway(outText,inLegacyTextEncoding,resultEncoding);
			AApplication::GetApplication().NVI_ConvertToUTF8CRAnyway(outText,inLegacyTextEncoding,false);//B0400 #182 #307
		}
	}
	*/
}

//AScrapRef�w��Ȃ�

#if 0
/**
ScrapData�擾�iAScrapRef�w��Ȃ��j
*/
void	AScrapWrapper::GetClipboardScrapData( const AScrapType inScrapType, AText& outText )
{
	GetScrapData([NSPasteboard generalPasteboard],inScrapType,outText);
	
	/*#688
	OSStatus	err = noErr;
	
	ScrapRef	scrap = NULL;
	err = ::GetCurrentScrap(&scrap);
	if( err != noErr )   _ACErrorNum("GetCurrentScrap() returned error: ",err,NULL);
	GetScrapData(scrap,inScrapType,outText);
	*/
}
#endif

/**
Text��ScrapData�擾�iAScrapRef�w��Ȃ��j
*/
void	AScrapWrapper::GetClipboardTextScrap( AText& outText )//#688, const ATextEncoding inLegacyTextEncoding, const ABool inConvertToCanonicalComp )
{
	//�N���b�v�{�[�h����e�L�X�g�擾
	GetTextScrap([NSPasteboard generalPasteboard],outText);
	
	/*#688
	OSStatus	err = noErr;
	
	ScrapRef	scrap = NULL;
	err = ::GetCurrentScrap(&scrap);
	if( err != noErr )   _ACErrorNum("GetCurrentScrap() returned error: ",err,NULL);
	GetTextScrap(scrap,outText,inLegacyTextEncoding,inConvertToCanonicalComp);
	*/
}

#pragma mark ===========================

#pragma mark ---Drag�֘A

#pragma mark ===========================

/**
ADragRef���^�C�v�`�F�b�N��ANSPasteboard�ɃL���X�g
*/
NSPasteboard*	GetNSPasteboardFromADragRef( const ADragRef inDragRef )
{
	//�|�C���^NULL�`�F�b�N
	if( inDragRef == NULL )
	{
		_ACThrow("ADragRef is NULL",NULL);
	}
	//id�փL���X�g
	id	pasteboard = static_cast<id>(inDragRef);
	//�N���X�^�C�v�`�F�b�N
	if( [pasteboard isKindOfClass:[NSPasteboard class]] == NO )
	{
		_ACThrow("Class error",NULL);
	}
	//NSPasteboard*�փL���X�g
	return static_cast<NSPasteboard*>(inDragRef);
}

#pragma mark ===========================

#pragma mark ---DragData�����݂��邩�ǂ����𒲂ׂ�

/**
DragData�����݂��邩�ǂ����𒲂ׂ�
*/
ABool	AScrapWrapper::ExistDragData( const ADragRef inDragRef, const AScrapType inScrapType )
{
	return ([[GetNSPasteboardFromADragRef(inDragRef) types] 
		containsObject:ACocoa::GetPasteboardTypeNSString(inScrapType)] == YES);
	/*#688
	OSStatus	err = noErr;
	
	UInt16	itemCount;
	err = ::CountDragItems(inDragRef,&itemCount);
	if( err != noErr )   _ACErrorNum("CountDragItems() returned error: ",err,NULL);
	for( AIndex i = 0; i < itemCount; i++ )
	{
		DragItemRef	itemRef;
		err = ::GetDragItemReferenceNumber(inDragRef,1+i,&itemRef);
		if( err != noErr )   _ACErrorNum("GetDragItemReferenceNumber() returned error: ",err,NULL);
		FlavorFlags	flags;
		if( ::GetFlavorFlags(inDragRef,itemRef,inScrapType,&flags) == noErr )   return true;
	}
	return false;
	*/
}

/**
Text��DragData�����݂��邩�ǂ����𒲂ׂ�
*/
ABool	AScrapWrapper::ExistTextDragData( const ADragRef inDragRef )
{
	return (ExistDragData(inDragRef,kScrapType_UnicodeText)==true);
}

#if 0
//#358
/**
*/
ABool	AScrapWrapper::ExistURLDragData( const ADragRef inDragRef )
{
	//#688 return (ExistDragData(inDragRef,'URLD')||ExistDragData(inDragRef,'url '));
	return (ExistDragData(inDragRef,kScrapType_URL)==true);//#688
}
#endif

#pragma mark ===========================

#pragma mark ---GetDrag

/**
DragData�擾
*/
void	AScrapWrapper::GetDragData( const ADragRef inDragRef, const AScrapType inScrapType, AText& outText )
{
	ACocoa::SetTextFromNSString([GetNSPasteboardFromADragRef(inDragRef) 
	  stringForType:(ACocoa::GetPasteboardTypeNSString(inScrapType))],outText);
	/*#688
	OSStatus	err = noErr;
	
	outText.DeleteAll();
	//�Y��ItemRef�擾
	DragItemRef	itemRef;
	UInt16	itemCount;
	err = ::CountDragItems(inDragRef,&itemCount);
	if( err != noErr )   _ACErrorNum("CountDragItems() returned error: ",err,NULL);
	ABool	found = false;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		err = ::GetDragItemReferenceNumber(inDragRef,1+i,&itemRef);
		if( err != noErr )   _ACErrorNum("GetDragItemReferenceNumber() returned error: ",err,NULL);
		FlavorFlags	flags;
		if( ::GetFlavorFlags(inDragRef,itemRef,inScrapType,&flags) == noErr )
		{
			found = true;
			break;
		}
	}
	if( found == false )   return;
	//�f�[�^�T�C�Y�擾
	Size	dataSize;
	err = ::GetFlavorDataSize(inDragRef,itemRef,inScrapType,&dataSize);
	if( dataSize <= 0 )   return;
	outText.Reserve(0,dataSize);
	AStTextPtr	textptr(outText,0,outText.GetItemCount());
	dataSize = textptr.GetByteCount();
	err = ::GetFlavorData(inDragRef,itemRef,inScrapType,textptr.GetPtr(),&dataSize,0);
	*/
}

/**
File��DragData�擾
*/
ABool	AScrapWrapper::GetFileDragData( const ADragRef inDragRef, AFileAcc& outFile )
{
	AObjectArray<AFileAcc>	fileArray;
	GetFileDragData(inDragRef,fileArray);
	if( fileArray.GetItemCount() > 0 )
	{
		//�X�N���b�v���̃t�@�C�����X�g�̍ŏ��̃t�@�C����Ԃ�
		outFile.CopyFrom(fileArray.GetObjectConst(0));
		return true;
	}
	else
	{
		//�X�N���b�v���̃t�@�C�����X�g����̏ꍇ�Afalse��Ԃ�
		return false;
	}
	/*#688
	OSStatus	err = noErr;
	
	if( ExistDragData(inDragRef,kScrapType_File) == false )   return false;
	//�Y��ItemRef�擾
	DragItemRef	itemRef;
	UInt16	itemCount;
	err = ::CountDragItems(inDragRef,&itemCount);
	if( err != noErr )   _ACErrorNum("CountDragItems() returned error: ",err,NULL);
	ABool	found = false;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		err = ::GetDragItemReferenceNumber(inDragRef,1+i,&itemRef);
		if( err != noErr )   _ACErrorNum("GetDragItemReferenceNumber() returned error: ",err,NULL);
		FlavorFlags	flags;
		if( ::GetFlavorFlags(inDragRef,itemRef,kScrapType_File,&flags) == noErr )
		{
			found = true;
			break;
		}
	}
	if( found == false )   return false;
	//�f�[�^�T�C�Y�擾
	Size	dataSize;
	err = ::GetFlavorDataSize(inDragRef,itemRef,kScrapType_File,&dataSize);
	if( dataSize <= 0 )   return false;
	HFSFlavor	hfsf;
	err = ::GetFlavorData(inDragRef,itemRef,kScrapType_File,&hfsf,&dataSize,0);
	outFile.SpecifyByFSSpec(hfsf.fileSpec);
	return true;
	*/
}

//#384
/**
File��DragData�擾
*/
void	AScrapWrapper::GetFileDragData( const ADragRef inDragRef, AObjectArray<AFileAcc>& outFileArray )
{
	outFileArray.DeleteAll();
	
	//File�^�C�v�X�N���b�v��������Ή������Ȃ�
	if( ExistDragData(inDragRef,kScrapType_File) == false )   return;
	
	//�X�N���b�v����t�@�C���̃��X�g�擾
	NSArray	*fileList = [GetNSPasteboardFromADragRef(inDragRef) propertyListForType:NSFilenamesPboardType];
	AItemCount	itemCount = [fileList count];
	for( AIndex i = 0; i < itemCount; i++ )
	{
		id	item = [fileList objectAtIndex:i];
		if( [item isKindOfClass:[NSString class]] == YES )
		{
			AText	filepath;
			ACocoa::SetTextFromNSString(item,filepath);
			AFileAcc	file;
			file.Specify(filepath);
			outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(file);
		}
	}
	
	/*#688
	OSStatus	err = noErr;
	outFileArray.DeleteAll();
	
	if( ExistDragData(inDragRef,kScrapType_File) == false )   return;
	//�Y��ItemRef�擾
	DragItemRef	itemRef;
	UInt16	itemCount;
	err = ::CountDragItems(inDragRef,&itemCount);
	if( err != noErr )   _ACErrorNum("CountDragItems() returned error: ",err,NULL);
	for( AIndex i = 0; i < itemCount; i++ )
	{
		err = ::GetDragItemReferenceNumber(inDragRef,1+i,&itemRef);
		if( err != noErr )   _ACErrorNum("GetDragItemReferenceNumber() returned error: ",err,NULL);
		FlavorFlags	flags;
		if( ::GetFlavorFlags(inDragRef,itemRef,kScrapType_File,&flags) == noErr )
		{
			//�f�[�^�T�C�Y�擾
			Size	dataSize;
			err = ::GetFlavorDataSize(inDragRef,itemRef,kScrapType_File,&dataSize);
			if( dataSize <= 0 )   continue;
			//File�擾
			HFSFlavor	hfsf;
			err = ::GetFlavorData(inDragRef,itemRef,kScrapType_File,&hfsf,&dataSize,0);
			outFileArray.GetObject(outFileArray.AddNewObject()).SpecifyByFSSpec(hfsf.fileSpec);
		}
	}
	*/
}

/**
URL��DragData�擾
*/
ABool	AScrapWrapper::GetURLDragData( const ADragRef inDragRef, AText& outURL, AText& outTitle )//#688 , const ATextEncoding inLegacyTextEncoding )
{
	outURL.DeleteAll();
	outTitle.DeleteAll();
	
	//URL�^�C�v�X�N���b�v��������Ή������Ȃ�
	if( ExistDragData(inDragRef,kScrapType_URL) == false )   return false;
	
	//�X�N���b�v����t�@�C���̃��X�g�擾
	NSArray	*array = [GetNSPasteboardFromADragRef(inDragRef) propertyListForType:@"WebURLsWithTitlesPboardType"];
	if( [array count] >= 2 )
	{
		id	urlarray = [array objectAtIndex:0];
		id	titlearray = [array objectAtIndex:1];
		if( [urlarray isKindOfClass:[NSArray class]] == YES && [titlearray isKindOfClass:[NSArray class]] == YES )
		{
			if( [urlarray count] >= 1 && [titlearray count] >= 1 )
			{
				id	url0 = [urlarray objectAtIndex:0];
				id	title0 = [titlearray objectAtIndex:0];
				if( [url0 isKindOfClass:[NSString class]] == YES && [title0 isKindOfClass:[NSString class]] == YES )
				{
					ACocoa::SetTextFromNSString(url0,outURL);
					ACocoa::SetTextFromNSString(title0,outTitle);
					return true;
				}
			}
		}
	}
	return false;
	
	/*#688
	ABool	found = false;
	if( ExistDragData(inDragRef,'URLD') == true )
	{
		AText	text;
		GetDragData(inDragRef,'URLD',text);
		AIndex	pos = 0;
		text.GetLine(pos,outURL);
		text.GetLine(pos,outTitle);
		found = true;
	}
	else if( ExistDragData(inDragRef,'url ') == true )
	{
		GetDragData(inDragRef,'url ',outURL);
		GetDragData(inDragRef,'urln',outTitle);
		found = true;
	}
	if( found == false )   return false;
	
	//B0122 �A�v���ɂ����UTF8�ŗ��邩��Mac�R�[�h�ŗ��邩�킩��Ȃ��̂ŕ␳
	{
		//UTF8�`�F�b�N�G���[�Ȃ�Mac�R�[�h�ƌ��Ȃ������ϊ�
		if( outURL.CheckUTF8Text() == false )
		{
			outURL.ConvertToUTF8(inLegacyTextEncoding);
		}
		if( outTitle.CheckUTF8Text() == false )
		{
			outTitle.ConvertToUTF8(inLegacyTextEncoding);
		}
	}
	return true;
	*/
}

/**
Text��DragData�擾
*/
void	AScrapWrapper::GetTextDragData( const ADragRef inDragRef, AText& outText )//#688 , const ATextEncoding inLegacyTextEncoding )
{
	GetDragData(inDragRef,kScrapType_UnicodeText,outText);
	
	/*#688
	outText.DeleteAll();
	//�܂�Unicode�Ŏ���
	if( ExistDragData(inDragRef,kScrapType_UnicodeText) == true )
	{
		GetDragData(inDragRef,kScrapType_UnicodeText,outText);
		ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(outText);
		outText.ConvertToUTF8FromUTF16();
		outText.ConvertReturnCodeToCR();//B0439
	}
	//Unicode�ł̃e�L�X�g�f�[�^�����������ꍇ�́A���K�V�[�Ŏ���
	if( outText.GetItemCount() == 0 )
	{
		if( ExistDragData(inDragRef,kScrapType_Text) == true )
		{
			GetDragData(inDragRef,kScrapType_Text,outText);
			//#307 ATextEncoding	resultEncoding;
			//B0400 AUtil::ConvertToUTF8CRAnyway(outText,inLegacyTextEncoding,resultEncoding);
			AApplication::GetApplication().NVI_ConvertToUTF8CRAnyway(outText,inLegacyTextEncoding,false);//B0400 #182 #307
		}
	}
	*/
}

