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

//B0370 ScrapRefを指定するインターフェイスに変更

/**
AScrapRefをタイプチェック後、NSPasteboardにキャスト
*/
NSPasteboard*	GetNSPasteboardFromAScrapRef( const AScrapRef inScrapRef )
{
	//ポインタNULLチェック
	if( inScrapRef == NULL )
	{
		_ACThrow("AScrapRef is NULL",NULL);
	}
	//idへキャスト
	id	pasteboard = static_cast<id>(inScrapRef);
	//クラスタイプチェック
	if( [pasteboard isKindOfClass:[NSPasteboard class]] == NO )
	{
		_ACThrow("Class error",NULL);
	}
	//NSPasteboard*へキャスト
	return static_cast<NSPasteboard*>(inScrapRef);
}

#pragma mark ===========================

#pragma mark ---ScrapDataが存在するかどうかを調べる

//AScrapRef指定あり

/**
ScrapDataが存在するかどうかを取得（AScrapRef指定あり）
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
TextのScrapDataが存在するかどうかを取得（AScrapRef指定あり）
*/
ABool	AScrapWrapper::ExistTextScrapData( const AScrapRef inScrapRef )
{
	return (ExistScrapData(inScrapRef,kScrapType_UnicodeText)==true);
}

//AScrapRef指定なし

/**
ScrapDataが存在するかどうかを取得（AScrapRef指定なし）
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
TextのScrapDataが存在するかどうかを取得（AScrapRef指定なし）
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

//AScrapRef指定あり

/**
ScrapData設定（AScrapRef指定あり）
*/
void	AScrapWrapper::SetScrapData( const AScrapRef inScrapRef, const AScrapType inScrapType, const AText& inText )
{
	//inScrapRefがNULLなら何もしない
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
TextのScrapData設定（AScrapRef指定あり）
*/
void	AScrapWrapper::SetTextScrap( const AScrapRef inScrapRef, const AText& inText, const ABool inConvertToCanonicalComp )//#1044 #688 , const ATextEncoding inLegacyTextEncoding )
{
	//inScrapRefがNULLなら何もしない
	if( inScrapRef == NULL )   return;
	
	//#931
	//改行コードをLFへ変換
	AText	text;
	text.SetText(inText);
	text.ConvertLineEndToLF();
	
	//inConvertToCanonicalCompがtrueならcanonical comp形式へ変換 #1044
	if( inConvertToCanonicalComp == true )
	{
		text.ConvertToCanonicalComp();
	}
	
	//テキストをpasteboardに設定
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
TextのScrapData設定（AScrapRef指定あり・Unicodeのみ）
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

//AScrapRef指定なし

/**
ScrapData設定（AScrapRef指定なし）
*/
void	AScrapWrapper::SetClipboardScrapData( const AScrapType inScrapType, const AText& inText, const ABool inClearScrap )
{
	//inClearScrapがtrueならpasteboardをまずクリア
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
	//クリップボード設定
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
TextのScrapData設定（AScrapRef指定なし）
*/
void	AScrapWrapper::SetClipboardTextScrap( const AText& inText, const ABool inClearScrap, const ABool inConvertToCanonicalComp )//#1044
{
	//inClearScrapがtrueならpasteboardをまずクリア
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
	//クリップボード設定
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

//AScrapRef指定あり

#if 0
/**
ScrapData取得（AScrapRef指定あり）
*/
void	AScrapWrapper::GetScrapData( const AScrapRef inScrapRef, const AScrapType inScrapType, AText& outText )
{
	//inScrapRefがNULLなら何もしない
	if( inScrapRef == NULL )   return;
	
	NSString*	str = [GetNSPasteboardFromAScrapRef(inScrapRef) stringForType:(ACocoa::GetPasteboardTypeNSString(inScrapType))];
	ACocoa::SetTextFromNSString(str,outText);
	
	/*#688
	OSStatus	err = noErr;
	
	outText.DeleteAll();
	if( inScrapRef == NULL )   return;
	Size	scrapSize = 0;//B0377 0初期化絶対必要。
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
TextのScrapData取得（AScrapRef指定あり）
*/
void	AScrapWrapper::GetTextScrap( const AScrapRef inScrapRef, AText& outText )
{
	//指定スクラップからテキスト取得
	NSString*	str = [GetNSPasteboardFromAScrapRef(inScrapRef) stringForType:NSStringPboardType];
	ACocoa::SetTextFromNSString(str,outText);
	//改行コードをCRに変換
	outText.ConvertReturnCodeToCR();
	
	/*#688
	outText.DeleteAll();
	//まずUnicodeで試す
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
	//B0377 OSX10.5の場合、'ut16'だけが来た場合（Finderからのファイルのコピー＆ペースト等）、'utxt'に自動変換されない。
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
	//Unicodeでのテキストデータが無かった場合は、レガシーで試す
	if( outText.GetItemCount() == 0 )
	{
		if( ExistScrapData(inScrapRef,kScrapType_Text) == true )
		{
			GetScrapData(inScrapRef,kScrapType_Text,outText);
			//PageMill等、最後にNULL文字をくっつけるアプリがあったための対策
			if( outText.GetItemCount() > 0 )
			{
				if( outText.Get(outText.GetItemCount()-1) == 0 )
				{
					outText.Delete1(outText.GetItemCount()-1);
				}
			}
			//B0342 0x80（MacJapaneseでバックスラッシュ？）をForceASCIIでUTF-8に変換するとU+F87Fというゴミがついてしまうので、回避策
			if( ::GetTextEncodingBase(inLegacyTextEncoding) == kTextEncodingMacJapanese )
			{
				AItemCount	count = outText.GetItemCount();
				for( AIndex i = 0; i < count; i++ )
				{
					AUChar	ch = outText.Get(i);
					
					//B0366 「ム」が「ソ」に化ける問題修正（ShiftJISなので２バイト文字判断必要）
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

//AScrapRef指定なし

#if 0
/**
ScrapData取得（AScrapRef指定なし）
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
TextのScrapData取得（AScrapRef指定なし）
*/
void	AScrapWrapper::GetClipboardTextScrap( AText& outText )//#688, const ATextEncoding inLegacyTextEncoding, const ABool inConvertToCanonicalComp )
{
	//クリップボードからテキスト取得
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

#pragma mark ---Drag関連

#pragma mark ===========================

/**
ADragRefをタイプチェック後、NSPasteboardにキャスト
*/
NSPasteboard*	GetNSPasteboardFromADragRef( const ADragRef inDragRef )
{
	//ポインタNULLチェック
	if( inDragRef == NULL )
	{
		_ACThrow("ADragRef is NULL",NULL);
	}
	//idへキャスト
	id	pasteboard = static_cast<id>(inDragRef);
	//クラスタイプチェック
	if( [pasteboard isKindOfClass:[NSPasteboard class]] == NO )
	{
		_ACThrow("Class error",NULL);
	}
	//NSPasteboard*へキャスト
	return static_cast<NSPasteboard*>(inDragRef);
}

#pragma mark ===========================

#pragma mark ---DragDataが存在するかどうかを調べる

/**
DragDataが存在するかどうかを調べる
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
TextのDragDataが存在するかどうかを調べる
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
DragData取得
*/
void	AScrapWrapper::GetDragData( const ADragRef inDragRef, const AScrapType inScrapType, AText& outText )
{
	ACocoa::SetTextFromNSString([GetNSPasteboardFromADragRef(inDragRef) 
	  stringForType:(ACocoa::GetPasteboardTypeNSString(inScrapType))],outText);
	/*#688
	OSStatus	err = noErr;
	
	outText.DeleteAll();
	//該当ItemRef取得
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
	//データサイズ取得
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
FileのDragData取得
*/
ABool	AScrapWrapper::GetFileDragData( const ADragRef inDragRef, AFileAcc& outFile )
{
	AObjectArray<AFileAcc>	fileArray;
	GetFileDragData(inDragRef,fileArray);
	if( fileArray.GetItemCount() > 0 )
	{
		//スクラップ内のファイルリストの最初のファイルを返す
		outFile.CopyFrom(fileArray.GetObjectConst(0));
		return true;
	}
	else
	{
		//スクラップ内のファイルリストが空の場合、falseを返す
		return false;
	}
	/*#688
	OSStatus	err = noErr;
	
	if( ExistDragData(inDragRef,kScrapType_File) == false )   return false;
	//該当ItemRef取得
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
	//データサイズ取得
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
FileのDragData取得
*/
void	AScrapWrapper::GetFileDragData( const ADragRef inDragRef, AObjectArray<AFileAcc>& outFileArray )
{
	outFileArray.DeleteAll();
	
	//Fileタイプスクラップが無ければ何もしない
	if( ExistDragData(inDragRef,kScrapType_File) == false )   return;
	
	//スクラップからファイルのリスト取得
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
	//該当ItemRef取得
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
			//データサイズ取得
			Size	dataSize;
			err = ::GetFlavorDataSize(inDragRef,itemRef,kScrapType_File,&dataSize);
			if( dataSize <= 0 )   continue;
			//File取得
			HFSFlavor	hfsf;
			err = ::GetFlavorData(inDragRef,itemRef,kScrapType_File,&hfsf,&dataSize,0);
			outFileArray.GetObject(outFileArray.AddNewObject()).SpecifyByFSSpec(hfsf.fileSpec);
		}
	}
	*/
}

/**
URLのDragData取得
*/
ABool	AScrapWrapper::GetURLDragData( const ADragRef inDragRef, AText& outURL, AText& outTitle )//#688 , const ATextEncoding inLegacyTextEncoding )
{
	outURL.DeleteAll();
	outTitle.DeleteAll();
	
	//URLタイプスクラップが無ければ何もしない
	if( ExistDragData(inDragRef,kScrapType_URL) == false )   return false;
	
	//スクラップからファイルのリスト取得
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
	
	//B0122 アプリによってUTF8で来るか旧Macコードで来るかわからないので補正
	{
		//UTF8チェックエラーならMacコードと見なし文字変換
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
TextのDragData取得
*/
void	AScrapWrapper::GetTextDragData( const ADragRef inDragRef, AText& outText )//#688 , const ATextEncoding inLegacyTextEncoding )
{
	GetDragData(inDragRef,kScrapType_UnicodeText,outText);
	
	/*#688
	outText.DeleteAll();
	//まずUnicodeで試す
	if( ExistDragData(inDragRef,kScrapType_UnicodeText) == true )
	{
		GetDragData(inDragRef,kScrapType_UnicodeText,outText);
		ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(outText);
		outText.ConvertToUTF8FromUTF16();
		outText.ConvertReturnCodeToCR();//B0439
	}
	//Unicodeでのテキストデータが無かった場合は、レガシーで試す
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

