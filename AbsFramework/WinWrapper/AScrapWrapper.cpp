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

#include "stdafx.h"

#include "AScrapWrapper.h"
#include <objidl.h>
#include <Shlobj.h>

/**
初期化
*/
void	AScrapWrapper::Init()
{
	sCF_INETURL = ::RegisterClipboardFormatW(CFSTR_INETURL);
}

UINT	AScrapWrapper::sCF_INETURL = 0;

#pragma mark ===========================

#pragma mark ---ClipboardScrapDataが存在するかどうかを調べる

ABool	AScrapWrapper::ExistClipboardScrapData( const AScrapType inScrapType )
{
	return (::IsClipboardFormatAvailable(inScrapType)==TRUE);
}

ABool	AScrapWrapper::ExistClipboardTextScrapData()
{
	return (ExistClipboardScrapData(kScrapType_UnicodeText)==true);
}

#pragma mark ===========================

#pragma mark ---SetScrap

void	AScrapWrapper::SetClipboardScrapData( const AScrapType inScrapType, const AText& inText, const ABool inClearScrap )
{
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	HGLOBAL	hglobal = ::GlobalAlloc(GHND|GMEM_SHARE,textptr.GetByteCount()+sizeof(AUTF16Char));
	AUCharPtr	ptr = (AUCharPtr)::GlobalLock(hglobal);
	if( ptr != NULL )
	{
		for( AIndex i = 0; i < textptr.GetByteCount(); i++ )
		{
			ptr[i] = (textptr.GetPtr())[i];
		}
		::GlobalUnlock(hglobal);
	}
	
	::OpenClipboard(NULL);
	if( inClearScrap == true )
	{
		::EmptyClipboard();
	}
	::SetClipboardData(inScrapType,hglobal);
	::CloseClipboard();
}

void	AScrapWrapper::SetClipboardTextScrap( const AText& inText, const ATextEncoding inLegacyTextEncoding, const ABool inClearScrap )
{
	//改行コード変換
	AText	utf8Text;
	utf8Text.SetText(inText);
	for( AIndex pos = 0; pos < utf8Text.GetItemCount(); pos++ )
	{
		AUChar	ch = utf8Text.Get(pos);
		if( ch == kUChar_CR )
		{
			utf8Text.Insert1(pos+1,kUChar_LF);
			pos++;
		}
	}
	//
	AText	utf16text;
	utf16text.SetText(utf8Text);
	utf16text.ConvertFromUTF8ToAPIUnicode();
	SetClipboardScrapData(kScrapType_UnicodeText,utf16text,inClearScrap);
	
	//★暫定 LegacyTextEncoding未対応
}

//#564
/**
TextのScrapData設定（AScrapRef指定なし・Unicodeのみ）
*/
void	AScrapWrapper::SetClipboardTextScrap( const AText& inText, const ABool inClearScrap )
{
	//改行コード変換
	AText	utf8Text;
	utf8Text.SetText(inText);
	for( AIndex pos = 0; pos < utf8Text.GetItemCount(); pos++ )
	{
		AUChar	ch = utf8Text.Get(pos);
		if( ch == kUChar_CR )
		{
			utf8Text.Insert1(pos+1,kUChar_LF);
			pos++;
		}
	}
	//
	AText	utf16text;
	utf16text.SetText(utf8Text);
	utf16text.ConvertFromUTF8ToAPIUnicode();
	SetClipboardScrapData(kScrapType_UnicodeText,utf16text,inClearScrap);
}

#pragma mark ===========================

#pragma mark ---GetScrap

void	AScrapWrapper::GetClipboardScrapData( const AScrapType inScrapType, AText& outText )
{
	outText.DeleteAll();
	::OpenClipboard(NULL);
	HGLOBAL	hglobal = ::GetClipboardData(inScrapType);
	if( hglobal != NULL )
	{
		AUCharPtr	ptr = (AUCharPtr)::GlobalLock(hglobal);
		if( ptr != NULL )
		{
			AByteCount	count = ::GlobalSize(hglobal)-sizeof(AUTF16Char);//文字列はNULL終端される
			outText.Reserve(0,count);
			{
				AStTextPtr	textptr(outText,0,outText.GetItemCount());
				for( AIndex i = 0; i < textptr.GetByteCount(); i++ )
				{
					(textptr.GetPtr())[i] = ptr[i];
				}
			}
			outText.ConvertToUTF8FromAPIUnicode();
			outText.ConvertReturnCodeToCR();
			::GlobalUnlock(hglobal);
		}
	}
	::CloseClipboard();
}

void	AScrapWrapper::GetClipboardTextScrap( AText& outText, const ATextEncoding inLegacyTextEncoding, const ABool inConvertToCanonicalComp )
{
	GetClipboardScrapData(kScrapType_UnicodeText,outText);
	
	//★暫定 LegacyTextEncoding未対応
}

#pragma mark ===========================

#pragma mark ---DragDataが存在するかどうかを調べる

ABool	AScrapWrapper::ExistDragData( const ADragRef inDragRef, const AScrapType inScrapType )
{
	if( inDragRef == NULL )   return false;

	FORMATETC	fmt;
	fmt.cfFormat = inScrapType;
	fmt.ptd = NULL;
	fmt.dwAspect = DVASPECT_CONTENT;
	fmt.lindex = -1;
	fmt.tymed = TYMED_HGLOBAL;
	if( inDragRef->QueryGetData(&fmt) == S_OK )   return true;
	else return false;
}

ABool	AScrapWrapper::ExistTextDragData( const ADragRef inDragRef )
{
	if( inDragRef == NULL )   return false;

	FORMATETC	fmt;
	fmt.cfFormat = CF_UNICODETEXT;
	fmt.ptd = NULL;
	fmt.dwAspect = DVASPECT_CONTENT;
	fmt.lindex = -1;
	fmt.tymed = TYMED_HGLOBAL;
	if( inDragRef->QueryGetData(&fmt) == S_OK )   return true;
	else return false;
}

ABool	AScrapWrapper::ExistURLDragData( const ADragRef inDragRef )
{
	if( inDragRef == NULL )   return false;

	FORMATETC	fmt;
	fmt.cfFormat = sCF_INETURL;
	fmt.ptd = NULL;
	fmt.dwAspect = DVASPECT_CONTENT;
	fmt.lindex = -1;
	fmt.tymed = TYMED_HGLOBAL;
	if( inDragRef->QueryGetData(&fmt) == S_OK )   return true;
	else return false;
}

#pragma mark ===========================

#pragma mark ---GetDrag

void	AScrapWrapper::GetDragData( const ADragRef inDragRef, const AScrapType inScrapType, AText& outText )
{
	if( inDragRef == NULL )   return;
	
	//取得データをそのままoutTextへ格納
	FORMATETC	fmt;
	fmt.cfFormat = inScrapType;
	fmt.ptd = NULL;
	fmt.dwAspect = DVASPECT_CONTENT;
	fmt.lindex = -1;
	fmt.tymed = TYMED_HGLOBAL;
	STGMEDIUM	stg;
	if( inDragRef->GetData(&fmt,&stg) == S_OK )
	{
		AUCharPtr	ptr = (AUCharPtr)::GlobalLock(stg.hGlobal);
		if( ptr != NULL )
		{
			AByteCount	count = ::GlobalSize(stg.hGlobal)-sizeof(AUTF16Char);//NULL終端される
			outText.Reserve(0,count);
			{
				AStTextPtr	textptr(outText,0,outText.GetItemCount());
				for( AIndex i = 0; i < textptr.GetByteCount(); i++ )
				{
					(textptr.GetPtr())[i] = ptr[i];
				}
			}
			::GlobalUnlock(stg.hGlobal);
		}
		::ReleaseStgMedium(&stg);
	}
}

/**
Dropデータからファイルデータ抽出
*/
ABool	AScrapWrapper::GetFileDragData( const ADragRef inDragRef, AFileAcc& outFile )
{
	AObjectArray<AFileAcc>	fileArray;
	GetFileDragData(inDragRef,fileArray);
	if( fileArray.GetItemCount() > 0 )
	{
		outFile.CopyFrom(fileArray.GetObjectConst(0));
		return true;
	}
	else
	{
		return false;
	}
}

//#384
/**
FileのDragData取得
*/
void	AScrapWrapper::GetFileDragData( const ADragRef inDragRef, AObjectArray<AFileAcc>& outFileArray )
{
	outFileArray.DeleteAll();

	if( inDragRef == NULL )   return;

	if( ExistDragData(inDragRef,kScrapType_File) == false )   return;
	
	//
	FORMATETC	fmt;
	fmt.cfFormat = CF_HDROP;//Shell Clipboard Formats参照。CF_HDROPはファイル
	fmt.ptd = NULL;
	fmt.dwAspect = DVASPECT_CONTENT;
	fmt.lindex = -1;
	fmt.tymed = TYMED_HGLOBAL;
	STGMEDIUM	stg;
	if( inDragRef->GetData(&fmt,&stg) == S_OK )
	{
		unsigned char*	ptr = (unsigned char*)::GlobalLock(stg.hGlobal);
		if( ptr != NULL )
		{
			//DROPFILESデータ取得
			DROPFILES	dropfiles = *((LPDROPFILES)(ptr));
			//pFilesはパスデータへのオフセット
			AIndex	offset = dropfiles.pFiles;
			ATextArray	filePathArray;
			if( dropfiles.fWide == TRUE )
			{
				//Unicodeの場合
				
				//テキストポインタ・レングス取得
				AUTF16Char*	textptr = (AUTF16Char*)(ptr+offset);
				AItemCount	textcount = (::GlobalSize(stg.hGlobal)-offset)/sizeof(AUTF16Char);
				if( textptr[textcount-1] != 0x0000 )
				{
					//最後がNULLで無い場合はエラー
					_ACError("Data is not null terminated",NULL);
				}
				else
				{
					for( AIndex pos = 0; pos < textcount; )
					{
						//ダブルNULL終端に達したら終了
						if( textptr[pos] == 0x0000 )   break;
						//テキスト取得
						AText	path;
						path.SetFromWcharString(&(textptr[pos]),textcount-pos);
						filePathArray.Add(path);
						//次のNULLの後まで移動
						for( ; pos < textcount; pos++ )
						{
							if( textptr[pos] == 0x0000 )
							{
								pos++;
								break;
							}
						}
					}
				}
				//返り値設定
				for( AIndex i = 0; i < filePathArray.GetItemCount(); i++ )
				{
					outFileArray.GetObject(outFileArray.AddNewObject()).Specify(filePathArray.GetTextConst(i));
				}
			}
			else
			{
				//ANSIの場合（★暫定 未ﾃｽﾄ）
				
				//テキストポインタ・レングス取得
				char*	textptr = (char*)(ptr+offset);
				AItemCount	textcount = (::GlobalSize(stg.hGlobal)-offset)/sizeof(char);
				if( textptr[textcount-1] != 0x00 )
				{
					//最後がNULLで無い場合はエラー
					_ACError("Data is not null terminated",NULL);
				}
				else
				{
					for( AIndex pos = 0; pos < textcount; )
					{
						//ダブルNULL終端に達したら終了
						if( textptr[pos] == 0x00 )   break;
						//テキスト取得
						AText	path;
						path.SetCstring(&(textptr[pos]));
						filePathArray.Add(path);
						//次のNULLの後まで移動
						for( ; pos < textcount; pos++ )
						{
							if( textptr[pos] == 0x00 )
							{
								pos++;
								break;
							}
						}
					}
				}
				//返り値設定
				for( AIndex i = 0; i < filePathArray.GetItemCount(); i++ )
				{
					outFileArray.GetObject(outFileArray.AddNewObject()).Specify(filePathArray.GetTextConst(i));
				}
			}
			::GlobalUnlock(stg.hGlobal);
		}
		::ReleaseStgMedium(&stg);
	}
}

/**
DropデータからURLデータ抽出
*/
ABool	AScrapWrapper::GetURLDragData( const ADragRef inDragRef, AText& outURL, AText& outTitle, const ATextEncoding inLegacyTextEncoding )
{	
	if( inDragRef == NULL )   return false;
	
	//データ取得はCF_UNICODETEXTで取得するので、本当のUnicodeTextと見分けがつかない。URL以外を取得しないよう、先に判定する。
	if( ExistURLDragData(inDragRef) == false )   return false;
	
	ABool	result = false;
	//★暫定 IE8からのDrag&Drop時、%00が大量に付加される
	FORMATETC	fmt;
	fmt.cfFormat = CF_UNICODETEXT;
	fmt.ptd = NULL;
	fmt.dwAspect = DVASPECT_CONTENT;
	fmt.lindex = -1;
	fmt.tymed = TYMED_HGLOBAL;
	STGMEDIUM	stg;
	if( inDragRef->GetData(&fmt,&stg) == S_OK )
	{
		AText	text;
		AUCharPtr	ptr = (AUCharPtr)::GlobalLock(stg.hGlobal);
		if( ptr != NULL )
		{
			AByteCount	count = ::GlobalSize(stg.hGlobal)-sizeof(AUTF16Char);
			text.Reserve(0,count);
			{
				AStTextPtr	textptr(text,0,text.GetItemCount());
				for( AIndex i = 0; i < textptr.GetByteCount(); i++ )
				{
					(textptr.GetPtr())[i] = ptr[i];
				}
			}
			text.ConvertToUTF8FromAPIUnicode();
			text.ConvertReturnCodeToCR();
			::GlobalUnlock(stg.hGlobal);
			outURL.SetText(text);
			result = true;
		}
		::ReleaseStgMedium(&stg);
	}
	return result;
}

/**
Dropデータからテキストデータ抽出
*/
void	AScrapWrapper::GetTextDragData( const ADragRef inDragRef, AText& outText, const ATextEncoding inLegacyTextEncoding )
{
	if( inDragRef == NULL )   return;
	
	FORMATETC	fmt;
	fmt.cfFormat = CF_UNICODETEXT;
	fmt.ptd = NULL;
	fmt.dwAspect = DVASPECT_CONTENT;
	fmt.lindex = -1;
	fmt.tymed = TYMED_HGLOBAL;
	STGMEDIUM	stg;
	if( inDragRef->GetData(&fmt,&stg) == S_OK )
	{
		AUCharPtr	ptr = (AUCharPtr)::GlobalLock(stg.hGlobal);
		if( ptr != NULL )
		{
			AByteCount	count = ::GlobalSize(stg.hGlobal)-sizeof(AUTF16Char);
			outText.Reserve(0,count);
			{
				AStTextPtr	textptr(outText,0,outText.GetItemCount());
				for( AIndex i = 0; i < textptr.GetByteCount(); i++ )
				{
					(textptr.GetPtr())[i] = ptr[i];
				}
			}
			outText.ConvertToUTF8FromAPIUnicode();
			outText.ConvertReturnCodeToCR();
			::GlobalUnlock(stg.hGlobal);
		}
		::ReleaseStgMedium(&stg);
	}
	
	//★暫定 LegacyTextEncoding未対応
}

