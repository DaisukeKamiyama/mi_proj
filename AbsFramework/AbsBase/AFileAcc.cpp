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

AFileAcc
ファイル（フォルダ）アクセスクラス

*/

#include "stdafx.h"

//#include "../miedit.h"
#include "../AbsBase/ABase.h"
#if IMPLEMENTATION_FOR_MACOSX
#include <sys/xattr.h>
#include <sys/stat.h>
#endif
#include "../MacWrapper/AFileWrapper.h"
#include "../AbsBase/Cocoa.h"
#import <Cocoa/Cocoa.h>

#if IMPLEMENTATION_FOR_WINDOWS
#include "io.h"
#include <stdio.h>
#include <fcntl.h>
#include <share.h>
#include <sys/stat.h>
#include <direct.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <Shlwapi.h>
#endif

#pragma mark ===========================
#pragma mark [クラス]AFileAcc
#pragma mark ===========================
//ファイルアクセスのためのクラス
//「ファイル実体に対する参照」的なクラスであり、ファイルアクセスはメソッド内で完結する。ファイルの状態は保持しない。

#pragma mark ---コンストラクタ／デストラクタ

AFileAcc::AFileAcc( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent), mSpecified(false)//, mFSRefSpecified(false)
{
}

AFileAcc::AFileAcc( const AText& inPath ) : AObjectArrayItem(NULL), mSpecified(false)//, mFSRefSpecified(false)
{
	Specify(inPath);
}

AFileAcc::AFileAcc( const AFileAcc& inFile ) : AObjectArrayItem(NULL), mSpecified(false)//, mFSRefSpecified(false)
{
	CopyFrom(inFile);
}

AFileAcc& AFileAcc::operator = ( const AFileAcc& inFile )
{
	CopyFrom(inFile);
	return (*this);
}

void AFileAcc::CopyFrom( const AFileAcc& inSrcFile )
{
	mPath.DeleteAll();
	mPath.AddText(inSrcFile.mPath);
	mSpecified = inSrcFile.IsSpecified();
	//mFSRefSpecified = inSrcFile.mFSRefSpecified;
	//mFSRef = inSrcFile.mFSRef;
}


#pragma mark ===========================

#pragma mark --- ファイル位置指定

/*
ファイルパスについて

【MacOSXの場合】
①mPathが保持するパスはUNIXパス形式、エンコードはUTF8(CanonicalComp)とする。
ただし、Specify時にUTF8(CanonicalComp)であることのチェックや変換はせず、受け取ったままのコードで保存する。
また、ファイル名のスラッシュ／コロンは、コロンで表現する。
GetPath()/Specify()のデフォルト(kFilePathType_Default)はこの形式を使用する。
OS API関数コール時に、HFSPlusDecomp(OS側)とCanonicalComp(AFileAcc側)を変換する。
②kFilePathType_1
GetPath()/Specify()で使用できるパス形式。GetPath()/Specify()内で①の形式と相互変換する。
/volumename/folder1/folder2/test.txt
の形式で、エンコードはUTF8(CanonicalComp)
ファイル名のスラッシュ／コロンは、コロンで表現する。
③kFilePathType_2
GetPath()/Specify()で使用できるパス形式。GetPath()/Specify()内で①の形式と相互変換する。
:volumename:folder1:folder2:test.txt
の形式で、エンコードはUTF8(CanonicalComp)
ファイル名のスラッシュ／コロンは、スラッシュで表現する。

*/

//ファイルの場所を指定する
// ただし、Specifyでは、パス文字列の設定のみを行い、ファイルが存在するかどうかのチェック等は行わない。
#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::Specify( const AText& inPath, const AFilePathType inFilePathType )
{
	mPath.DeleteAll();
	switch(inFilePathType)
	{
		//UNIXパス形式
	  case kFilePathType_Default:
		{
			//mPathへそのまま設定
			//mPathはCannonicalCompを想定しているが、もしdecompであったとしても、AFileAcc内では特に問題ない。
			//スラッシュ／コロンは、inPath内で既にコロンで表現されているべき。（このメソッド内では変換のしようがない）
			mPath.AddText(inPath);
			break;
		}
		//  /volumename/folder1/folder2/test.txtの形式(kFilePathType_1)
		//  :volumename:folder1:folder2:test.txtの形式(kFilePathType_2)
	  case kFilePathType_1:
	  case kFilePathType_2:
		{
			//パス区切り文字の設定
			AUChar	delimiter = GetPathDelimiter(inFilePathType);
			/*win kUChar_Slash;
			if( inFilePathType == kFilePathType_2 )
			{
				delimiter = kUChar_Colon;
			}*/
			//スラッシュをデリミタとしてテキスト配列に分解
			ATextArray	pathArray;
			inPath.Explode(delimiter,pathArray);
			
			//分解テキストが2つより小さいとき＝スラッシュ存在しないときは何もせずリターン
			if( pathArray.GetItemCount() < 2 )   return;
			
			//スラッシュで始まるので、最初のtextは必ず空のはず
			AText	text;
			pathArray.Get(0,text);
			if( text.GetItemCount() != 0 )   return;
			
			//ボリューム検索
			FSRef	volumeFSRef;
			AText	volumeNameText;
			pathArray.Get(1,volumeNameText);
			ABool	volumeFound = false;
			FSVolumeRefNum	vRefNum;
			for( ItemCount i = 1; ; i++ )
			{
				HFSUniStr255	volName;
				OSStatus	status = ::FSGetVolumeInfo(kFSInvalidVolumeRefNum,i,&vRefNum,kFSVolInfoNone,NULL,&volName,&volumeFSRef);
				if( status != noErr )   break;
				AText	text;
				SetTextByHFSUniStr255(volName,text);
				if( text.Compare(volumeNameText) == true )
				{
					volumeFound = true;
					break;
				}
			}
			//ボリュームが見つからないときは何もせずリターン
			if( volumeFound == false )   return;
			
			//フォルダ階層を下へ検索
			FSRef	currentFSRef = volumeFSRef;
			for( AIndex i = 2; i < pathArray.GetItemCount(); i++ )
			{
				AText	text;
				pathArray.Get(i,text);
				//..なら親フォルダへ移動
				if( text.Compare("..") == true )
				{
					if( ::FSCompareFSRefs(&currentFSRef,&volumeFSRef) == noErr )   return;
					//親フォルダのFSRef取得
					::FSGetCatalogInfo(&currentFSRef,kFSCatInfoNone,NULL,NULL,NULL,&currentFSRef);
				}
				//.は何もしない
				else if( text.Compare(".") == true )
				{
					//処理無し
				}
				//子フォルダ／ファイル
				else
				{
					//B0445
					if( inFilePathType == kFilePathType_1 )
					{
						text.ReplaceChar(':','/');
					}
					//子の名前テキストを、UTF16(HFSPlusDecomp)へ変換し、FSRefを取得
					text.ConvertFromUTF8ToUTF16();
					ATextEncodingWrapper::ConvertUTF16ToHFSPlusDecomp(text);
					FSRef	newFSRef;
					{//#598 arrayptrの有効範囲を最小にする
						AStTextPtr	textptr(text,0,text.GetItemCount());
						OSStatus status = ::FSMakeFSRefUnicode(&currentFSRef,textptr.GetByteCount()/sizeof(UniChar),reinterpret_cast<const UniChar*>(textptr.GetPtr()),kTextEncodingUnknown,&newFSRef);
						if( status != noErr )   return;
					}
					//エイリアス解決
					Boolean	isFolder,isAlias;
					::FSResolveAliasFile(&newFSRef,true,&isFolder,&isAlias);
					currentFSRef = newFSRef;
				}
			}
			
			//FSRefでSpecify
			SpecifyByFSRef(currentFSRef);
			break;
		}
	}
	mSpecified = true;
	//mFSRefSpecified = false;
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::Specify( const AText& inPath, const AFilePathType inFilePathType )
{
	mPath.DeleteAll();
	mPath.AddText(inPath);
	mSpecified = true;
}
#endif

#if IMPLEMENTATION_FOR_MACOSX
//B0389 OK いずれかのFilePathTypeでファイルが見つかるまで試す
void	AFileAcc::SpecifyWithAnyFilePathType( const AText& inPath )
{
	Specify(inPath,kFilePathType_Default);
	if( Exist() == true )   return;
	Specify(inPath,kFilePathType_1);
	if( Exist() == true )   return;
	AText	path;
	path.SetCstring(":");
	path.AddText(inPath);
	Specify(path,kFilePathType_2);
	if( Exist() == true )   return;
	Specify(inPath,kFilePathType_2);
	if( Exist() == true )   return;
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::SpecifyWithAnyFilePathType( const AText& inPath )
{
	Specify(inPath);
}
#endif

/**
あるファイルの親フォルダを指定する
*/
ABool	AFileAcc::SpecifyParent( const AFileAcc& inBaseFile ) //#427
{
	ABool	result = true;
	//ベースファイルオブジェクトのパスをコピーする
	inBaseFile.GetPath(mPath);
	if( mPath.GetLength() == 0 )   return false;//#427
	
	//1つ上のフォルダを示すように文字列操作
	AIndex pos = mPath.GetLength();
	if( pos >= 1 )   pos--;//最後の/は無視したいので-2する
	if( pos >= 1 )   pos--;
	if( mPath.GoBackToChar(pos,GetPathDelimiter()) == false )
	{
		//#427 /が見つからなかった場合はエラーにする
		result = false;
	}
	pos++;//これで/の直後に来る
	mPath.DeleteAfter(pos);
	mSpecified = true;
	//mFSRefSpecified = false;
	return result;//#427
}

//あるファイルの同フォルダの別のファイル（またはフォルダやその下のファイル）を指定する
void	AFileAcc::SpecifySister( const AFileAcc& inBaseFile, AConstCharPtr inPath, const AFilePathType inFilePathType )
{
	AFileAcc	parent;
	parent.SpecifyParent(inBaseFile);
	SpecifyChild(parent,inPath,inFilePathType);
}
void	AFileAcc::SpecifySister( const AFileAcc& inBaseFile, const AText& inPath, const AFilePathType inFilePathType )
{
	AFileAcc	parent;
	parent.SpecifyParent(inBaseFile);
	SpecifyChild(parent,inPath,inFilePathType);
}

//あるフォルダの子ファイル（またはフォルダやその下のファイル）を指定する
void	AFileAcc::SpecifyChild( const AFileAcc& inBaseFile, AConstCharPtr inPath, const AFilePathType inFilePathType )
{
	AText	path;
	path.AddCstring(inPath);
	SpecifyChild(inBaseFile,path,inFilePathType);
}
/*#695
#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::SpecifyChild( const AFileAcc& inBaseFile, const AText& inPath, const AFilePathType inFilePathType )
{
	//pathに、inFilePathTypeの形式でファイルパスを再構成する
	AText	path;
	
	//ベースファイルオブジェクトのパスを取得
	inBaseFile.GetPath(path,inFilePathType);
	//パスの最後の文字が/でなければ、/を付加する
	if( path.GetLastChar() != GetPathDelimiter(inFilePathType) )
	{
		path.Add(GetPathDelimiter(inFilePathType));
	}
	//inPathを付加
	path.AddText(inPath);
	
	//
	Specify(path,inFilePathType);
	
	mSpecified = true;
	//mFSRefSpecified = false;
}
//対応必要 childに../が入っている場合
#endif
#if IMPLEMENTATION_FOR_WINDOWS
*/
void	AFileAcc::SpecifyChild( const AFileAcc& inBaseFile, const AText& inPath, const AFilePathType inFilePathType )
{
	//pathに、inFilePathTypeの形式でファイルパスを再構成する
	AText	path;
	
	//ベースファイルオブジェクトのパスを取得
	inBaseFile.GetPath(path,inFilePathType);
	//パスの最後の文字が/でなければ、/を付加する
	if( path.GetLastChar() != GetPathDelimiter(inFilePathType) )
	{
		path.Add(GetPathDelimiter(inFilePathType));
	}
	
	//pathへinPathをくっつける
	
	//デリミタ取得
	AUChar	delimiter = GetPathDelimiter(inFilePathType);
	//相対パス取得 
	AText	relativePath;
	relativePath.SetText(inPath);
	//パス区切りはdelimiterで統一
	//（Windowsの場合に、スラッシュをバックスラッシュに変換する）
	if( delimiter != kUChar_Slash )
	{
		relativePath.ReplaceChar(kUChar_Slash,delimiter);//#695 kUChar_Backslash);
	}
	
	//relativePathをくっつける位置をcatposに求める。最初は最後尾から始める。
	AIndex	catpos = path.GetLength();
	//relativePathの最初から"./"と"../"を探していく
	AIndex	pos = 0;
	AItemCount	len = relativePath.GetLength();
	//最初の文字がバックスラッシュだったら削除する（上のほうでバックスラッシュをつけているので、重複しないようにするため）
	if( len > 0 )
	{
		if( relativePath.Get(0) == /*#695 kUChar_Backslash*/ delimiter )
		{
			relativePath.Delete1(0);
		}
	}
	//
	while( pos < len )
	{
		//".\"だったら何もせずに進める
		if( pos+1 < len )
		{
			if( relativePath.Get(pos) == kUChar_Period && relativePath.Get(pos+1) == /*#695 kUChar_Backslash*/ delimiter )
			{
				pos += 2;
				continue;
			}
		}
		//"..\"だったらcatposを１つ前の/の直後にする。
		if( pos+2 < len )
		{
			if( relativePath.Get(pos) == kUChar_Period && relativePath.Get(pos+1) == kUChar_Period && 
						relativePath.Get(pos+2) == /*#695 kUChar_Backslash*/ delimiter )
			{
				pos += 3;
				catpos -= 2;
				path.GoBackToChar(catpos,/*#695 kUChar_Backslash*/ delimiter);
				catpos++;
				continue;
			}
		}
		//".\"や"..\"以外だったらcatpos以降を削除して、inRelativePathの現在位置以降をくっつける
		path.DeleteAfter(catpos);
		relativePath.Delete(0,pos);
		path.AddText(relativePath);
		break;
	}
	
	//
	Specify(path,inFilePathType);
	
	mSpecified = true;
}
//#695 #endif

//あるフォルダの子ファイルを指定する
void	AFileAcc::SpecifyChildFile( const AFileAcc& inBaseFile, const AText& inFilename ) 
{
	AText	filename;
	filename.SetText(inFilename);
#if IMPLEMENTATION_FOR_MACOSX
	//ファイル名内のスラッシュは全てコロンに変換する
	filename.ReplaceChar(kUChar_Slash,kUChar_Colon);
#endif
	SpecifyChild(inBaseFile,filename,kFilePathType_Default);
}

//inFolder内で、他のファイルのファイル名と重複しないファイルを指定する。
void	AFileAcc::SpecifyUniqChildFile( const AFileAcc& inFolder, AConstCharPtr inFilename )
{
	AText	filename;
	filename.AddCstring(inFilename);
	SpecifyUniqChildFile(inFolder,filename);
}
void	AFileAcc::SpecifyUniqChildFile( const AFileAcc& inFolder, const AText& inFilename )
{
	SpecifyChildFile(inFolder,inFilename);
	
	if( Exist() == false )
	{
		return;
	}
	AText	originalPath;
	GetPath(originalPath);
	for( ANumber i = 2; i < 1000000; i++ )
	{
		AText	number;
		number.SetFormattedCstring("-%d",i);//#0 スペースからハイフォンへ変更
		AText	path;
		path.SetText(originalPath);
		//R0137 path.AddText(number);
		path.InsertText(path.GetFirstSuffixStartPos(),number);
		
		Specify(path);
		if( Exist() == false )
		{
			return;
		}
	}
	_ACThrow("",this);
}

void	AFileAcc::SpecifyChangeSuffix( const AFileAcc& inBaseFile, const AText& inSuffix )
{
	AText	path;
	inBaseFile.GetPath(path);
	for( AIndex i = path.GetItemCount()-1; i >= 0; i-- )
	{
		AUChar	ch = path.Get(i);
		if( ch == kUChar_Period )
		{
			path.DeleteAfter(i);
			path.AddText(inSuffix);
			break;
		}
	}
	Specify(path);
}

void	AFileAcc::Unspecify()
{
	mPath.DeleteAll();
	mSpecified = false;
	//mFSRefSpecified = false;
}

#pragma mark ===========================

#pragma mark --- パス取得

#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::GetPath( AText& outPath, const AFilePathType inFilePathType ) const
{
	outPath.DeleteAll();
	switch(inFilePathType)
	{
		//UNIXパス形式
	  case kFilePathType_Default:
		{
			outPath.AddText(mPath);
			//#530 GetPath()の出力形式を統一
			//最後のスラッシュは取り除く
			if( outPath.GetItemCount() > 0 )
			{
				if( outPath.Get(outPath.GetItemCount()-1) == kUChar_Slash )
				{
					outPath.DeleteAfter(outPath.GetItemCount()-1);
				}
			}
			break;
		}
		//  /volumename/folder1/folder2/test.txtの形式(kFilePathType_1)
		//  :volumename:folder1:folder2:test.txtの形式(kFilePathType_2)
	  case kFilePathType_1:
	  case kFilePathType_2:
		{
			FSRef	fsref;
			if( GetFSRef(fsref) == false )   return;
			//パス区切り文字の設定
			AUChar	pathdelimiter = GetPathDelimiter(inFilePathType);
			/*win kUChar_Slash;
			if( inFilePathType == kFilePathType_2 )
			{
				pathdelimiter = kUChar_Colon;
			}*/
			//フォルダ階層を上へ検索
			HFSUniStr255	name;
			FSCatalogInfo	catalogInfo;
			FSRef	parent;
			while(true)
			{
				//ファイル／フォルダ名と、親フォルダを取得
				::FSGetCatalogInfo(&fsref,kFSCatInfoParentDirID,&catalogInfo,&name,NULL,&parent);
				//パス文字列に追加
				AText	nametext;
				SetTextByHFSUniStr255(name,nametext);
				//B0445
				if( inFilePathType == kFilePathType_1 )
				{
					nametext.ReplaceChar('/',':');
				}
				
				outPath.InsertText(0,nametext);
				outPath.Insert1(0,pathdelimiter);
				//ルート（ボリューム）に到達したら終了
				if( catalogInfo.parentDirID == fsRtParID )   break;
				fsref = parent;
			}
			break;
		}
	}
}

//#1087
/**
ファイル名等の文字列をファイルシステム用の文字列に変換
*/
void	AFileAcc::GetNormalizedPathString( AText& ioPath ) 
{
	@try 
	{
		if( ioPath.GetItemCount() > 0 )
		{
			//ファイルシステム用文字列取得
			AStCreateNSStringFromAText	str(ioPath);
			ioPath.SetCstring([str.GetNSString() fileSystemRepresentation]);
		}
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::GetNormalizedPathString() @catch",NULL);
	}
}

//B0000
void	AFileAcc::GetNormalizedPath( AText& outPath ) const
{
	//HFS+ decomp形式に変換（mPathにはdecomp形式、comp形式両方設定されうるので。最後のスラッシュはGetPath()出力時点で削除済み。）
	outPath.DeleteAll();
	//パス取得
	GetPath(outPath);
	//ファイルシステム用文字列に変換
	AFileAcc::GetNormalizedPathString(outPath);
	
	/*
	outPath.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToHFSPlusDecomp(outPath);
	outPath.ConvertToUTF8FromUTF16();
	*/
	/*#695
	outPath.DeleteAll();
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return;
	UInt8	path[4096];
	path[0] = 0;
	::FSRefMakePath(&fsref,path,4096);
	outPath.SetCstring(reinterpret_cast<char*>(path));
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::GetPath( AText& outPath, const AFilePathType inFilePathType ) const
{
	GetNormalizedPath(outPath);
}
void	AFileAcc::GetNormalizedPath( AText& outPath ) const
{
	outPath.SetText(mPath);
	//最後のバックスラッシュは取り除く
	if( outPath.GetItemCount() > 0 )
	{
		if( outPath.Get(outPath.GetItemCount()-1) == kUChar_Backslash )
		{
			outPath.DeleteAfter(outPath.GetItemCount()-1);
		}
	}
	//★確認
	//GetFullPathName?
}
#endif

//相対パス取得
void	AFileAcc::GetPartialPath( const AFileAcc& inBaseFile, AText& outPath, const AFilePathType inFilePathType ) const
{
	outPath.DeleteAll();
	if( inBaseFile.IsSpecified() == false )
	{
		GetNormalizedPath(outPath);
		return;
	}
	//パス区切り文字の設定
	AUChar	pathdelimiter = GetPathDelimiter(inFilePathType);
	//ターゲットのパス取得
	AText	target;
	GetNormalizedPath(target);
	//#614 フォルダからの相対パスが正しく取得できない問題対策
	if( IsFolder() == true )
	{
		target.AddPathDelimiter(pathdelimiter);
	}
	//ベースのパス取得
	AText	base;
	inBaseFile.GetNormalizedPath(base);
	//#614 フォルダからの相対パスが正しく取得できない問題対策
	if( inBaseFile.IsFolder() == true )
	{
		base.AddPathDelimiter(pathdelimiter);
	}
	
	/*win kUChar_Slash;
	if( inFilePathType == kFilePathType_2 )
	{
		pathdelimiter = kUChar_Colon;
	}*/
	
	//相対パス計算
	//１．ターゲットとベースの文字が最初に異なる位置を取得
	AIndex	pos = 0;
	for( ; pos < target.GetItemCount(); pos++ )
	{
		if( pos >= base.GetItemCount() )   break;
		if( target.Get(pos) != base.Get(pos) )   break;
	}
	//２．１で取得した位置から戻って最初のパスデリミタの位置を取得
	for( pos--; pos > 0; pos-- )
	{
		if( target.Get(pos) == pathdelimiter )   break;
	}
	pos++;
	//３．ベースから２のパスデリミタより後のパスデリミタの数を取得
	AItemCount	basedepth = 0;
	for( AIndex basepos = pos; basepos < base.GetItemCount(); basepos++ )
	{
		if( base.Get(basepos) == pathdelimiter )
		{
			basedepth++;
		}
	}
	//４．３の数だけ../を追加
	for( AIndex i = 0; i < basedepth; i++ )
	{
		outPath.AddCstring("..");
		outPath.Add(pathdelimiter);
	}
	//５．ターゲットから２のパスデリミタより後の文字列を追加
	AText	text;
	target.GetText(pos,target.GetItemCount()-pos,text);
	outPath.AddText(text);
}

//#539
/**
URL取得
*/
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::GetURL( AText& outURL ) const
{
	//#1425
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	NSURL*	url = [NSURL fileURLWithPath:pathstr.GetNSString()];
	if( url != nil )
	{
		outURL.SetFromNSString(url.path);
		return true;
	}
	else
	{
		return false;
	}
	
	/*#1425
	FSRef	ref;
	if( GetFSRef(ref) == false )   return false;
	
	ABool	result = false;
	CFURLRef	url = ::CFURLCreateFromFSRef(NULL,&ref);
	if( url != NULL )
	{
		CFStringRef	str = ::CFURLGetString(url);
		if( str != NULL )
		{
			outURL.SetFromCFString(str);
			result = true;
			::CFRelease(str);
		}
		//#1012 これがあるとSIGABRT発生するのでコメントアウト::CFRelease(url);
	}
	return result;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::GetURL( AText& outURL ) const
{
	AText	path;
	GetNormalizedPath(path);
	AStCreateWcharStringFromAText	pathstr(path);
	wchar_t	url[4096];
	DWORD	len = 4096;
	HRESULT	res = ::UrlCreateFromPath(pathstr.GetPtr(),url,&len,NULL);
	outURL.SetFromWcharString(url,len);
	return (res==S_OK);
}
#endif

#pragma mark ===========================

#pragma mark --- ファイル名取得

//ファイル名を取得
void	AFileAcc::GetFilename( AText& outName, const ABool inConvertForDisplay ) const
{
	outName.DeleteAll();
	/*B0000 高速化
	ATextArray	textArray;
	mPath.Explode(GetPathDelimiter(),textArray);
	if( textArray.GetItemCount() == 0 )   return;
	textArray.Get(textArray.GetItemCount()-1,outName);
	if( outName.GetItemCount() == 0 )
	{
		//folder
		if( textArray.GetItemCount() < 2 )   return;
		textArray.Get(textArray.GetItemCount()-2,outName);
	}
	*/
	mPath.GetFilename(outName);
	
	/*#1040
	outName.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(outName);
	outName.ConvertToUTF8FromUTF16();
	*/
	outName.ConvertToCanonicalComp();
	
	if( inConvertForDisplay == true )
	{
#if IMPLEMENTATION_FOR_MACOSX
		//UnixPathでは/は:に変換されている。MacOX純APIでは/を使うので、:を/に変換。
		outName.ReplaceChar(kUChar_Colon,kUChar_Slash);
#endif
	}
}

//ファイル名を取得 R0006
//UNIXコマンドに渡して正しく動作する形式で返す
void	AFileAcc::GetRawFilename( AText& outName ) const
{
	outName.DeleteAll();
	AText	path;
	GetNormalizedPath(path);
	path.GetFilename(outName);
}

#pragma mark ===========================

#pragma mark --- 比較

//ファイル比較
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::Compare( const AFileAcc& inFileAcc ) const
{
	/*#695
	//ファイルが存在する場合は同じファイルかどうかを比較
	FSRef	fsref1, fsref2;
	ABool	exist = false;
	exist = GetFSRef(fsref1);
	if( exist == true )
	{
		exist = inFileAcc.GetFSRef(fsref2);
	}
	if( exist == true )
	{
		return (::FSCompareFSRefs(&fsref1,&fsref2) == noErr);
	}
	*/
	//ファイルが存在しない場合はパステキストを比較
	AText	path1, path2;
	//パス取得
	inFileAcc.GetPath(path1);
	//最後のパスデリミタを削除
	if( path1.GetLength() > 0 )
	{
		if( path1.Get(path1.GetLength()-1) == GetPathDelimiter() )
		{
			path1.Delete(path1.GetLength()-1,1);
		}
	}
	/*#1040
	path1.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(path1);
	*/
	path1.ConvertToCanonicalComp();
	
	//パス取得
	GetPath(path2);
	//最後のパスデリミタを削除
	if( path2.GetLength() > 0 )
	{
		if( path2.Get(path2.GetLength()-1) == GetPathDelimiter() )
		{
			path2.Delete(path2.GetLength()-1,1);
		}
	}
	/*#1040
	path2.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(path2);
	*/
	path2.ConvertToCanonicalComp();
	return path1.Compare(path2);
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::Compare( const AFileAcc& inFileAcc ) const
{
	//パステキストを比較
	AText	path1, path2;
	//パス取得
	inFileAcc.GetPath(path1);
	//最後のパスデリミタを削除
	if( path1.GetLength() > 0 )
	{
		if( path1.Get(path1.GetLength()-1) == GetPathDelimiter() )
		{
			path1.Delete(path1.GetLength()-1,1);
		}
	}
	path1.ChangeCaseLowerFast();
	
	//パス取得
	GetPath(path2);
	//最後のパスデリミタを削除
	if( path2.GetLength() > 0 )
	{
		if( path2.Get(path2.GetLength()-1) == GetPathDelimiter() )
		{
			path2.Delete(path2.GetLength()-1,1);
		}
	}
	path2.ChangeCaseLowerFast();
	
	return path1.Compare(path2);
}
#endif

//ファイル名比較
ABool	AFileAcc::CompareFilename( const AText& inFilename ) const
{
	AText	filename1;
	GetFilename(filename1);
	/*#1040
	filename1.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(filename1);
	*/
	filename1.ConvertToCanonicalComp();
	
	AText	filename2;
	filename2.SetText(inFilename);
	/*#1040
	filename2.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(filename2);
	*/
	filename2.ConvertToCanonicalComp();
	
	return filename1.Compare(filename2);
}

//拡張子比較
ABool	AFileAcc::CompareSuffix( const AText& inSuffix ) const
{
	AText	filename;
	GetFilename(filename);
	AText	suffix;
	filename.GetSuffix(suffix);
	return suffix.Compare(inSuffix);
}

#pragma mark ===========================

#pragma mark --- ファイル情報取得

//ファイル存在チェック
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::Exist() const
{
	ABool	result = false;
	@try 
	{
		//パス取得
		AText	path;
		GetPath(path);//#1090 テストしたところ、fileExistsAtPathはcomp/decomp関係なく動作するようなので、高速化のためnormalizeしない。
		AStCreateNSStringFromAText	pathstr(path);
		//ファイル存在チェック
		result = ([[NSFileManager defaultManager] fileExistsAtPath:pathstr.GetNSString()]==YES);
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::Exist() @catch",this);
	}
	return result;
	/*#1034
	FSRef	ref;
	return GetFSRef(ref);
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::Exist() const
{
/*	if( mPath.GetItemCount() < 2 )   return false;
	if( mPath.Get(1) != ':' )   return false;
	//ドライブ存在チェック（未テスト・ドライブ非存在ダイアログが表示される不具合→非再現。再現したらテスト）
	DWORD	bitmask = 0;
	AUChar	ch = mPath.Get(0);
	if( ch >= 'A' && ch <= 'Z' )
	{
		bitmask = (1<<(ch-'A'));
	}
	else if( ch >= 'a' && ch <= 'z' )
	{
		bitmask = (1<<(ch-'a'));
	}
	else return false;
	if( (::GetLogicalDrives()&bitmask) == 0 )   return false;*/
	
	if( mPath.GetItemCount() == 0 )   return false;
	
	//最後に\があると正常に判定されない
	AText	path(mPath);
	if( path.Get(path.GetItemCount()-1) == GetPathDelimiter() )
	{
		path.DeleteAfter(path.GetItemCount()-1);
	}
	
	//存在チェック
	AStCreateWcharStringFromAText	pathstr(path);
	return (_waccess_s(pathstr.GetPtr(),0)==0);
}
#endif

//フォルダか？
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::IsFolder() const
{
	ABool	result = false;
	@try 
	{
		//パス取得
		AText	path;
		GetPath(path);//#1090 テストしたところ、fileExistsAtPathはcomp/decomp関係なく動作するようなので、高速化のためnormalizeしない。
		AStCreateNSStringFromAText	pathstr(path);
		//ファイル存在チェック＆フォルダかどうか判定
		BOOL	isDir = NO;
		if( [[NSFileManager defaultManager] fileExistsAtPath:pathstr.GetNSString() isDirectory:&isDir] == YES )
		{
			result = (isDir==YES);
		}
		else
		{
			//ファイル非存在ならfalse
			result = false;
		}
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::IsFolder() @catch",this);
	}
	return result;
	/*#1034
	FSCatalogInfo	catalog;
	FSRefParam	param;
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	param.ref = &fsref;
	param.whichInfo = kFSCatInfoNodeFlags;
	param.catInfo = &catalog;
	param.spec = NULL;
	param.parentRef = NULL;
	param.outName = NULL;
	::PBGetCatalogInfoSync(&param);
	return ((param.catInfo->nodeFlags&kFSNodeIsDirectoryMask)!=0);
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::IsFolder() const
{
	if( mPath.GetItemCount() == 0 )   return false;
	
	//最後に\があると正常に判定されない
	AText	path(mPath);
	if( path.Get(path.GetItemCount()-1) == GetPathDelimiter() )
	{
		return true;
	}
	
	//フォルダかどうかの判定
	AStCreateWcharStringFromAText	pathstr(path);
	struct __stat64	st = {0};
	if( _wstat64(pathstr.GetPtr(),&st) == 0 )//【OS互換性】Win95/98では使えない。WinNT以降
	{
		return ((st.st_mode&_S_IFDIR)!=0);
	}
	else
	{
		return false;
	}
}
#endif

//読み込み専用ファイルか？
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::IsReadOnly() const
{
	/*readonlyのファイルでも0になってしまう FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	FSCatalogInfo	catinfo;
  ::FSGetCatalogInfo(&fsref,kFSCatInfoNodeFlags,&catinfo,NULL,NULL,NULL);
	return ((catinfo.nodeFlags&kFSNodeLockedMask)!=0);*/
	
	//読み込み専用かの判定
	@try
	{
		//パス取得
		AText	path;
		GetNormalizedPath(path);
		AStCreateNSStringFromAText	pathstr(path);
		
		//書き込み可否（パーミション）チェック
		if( [[NSFileManager defaultManager] isWritableFileAtPath:pathstr.GetNSString()] == NO )
		{
			//書き込みパーミションなければtrueを返す
			return true;
		}
		
		/*fileHandleForWritingAtPathはO_SHLOCKされたファイルであってもfile handleを返すので使えない。
		NSFileHandle* fh = [NSFileHandle fileHandleForWritingAtPath:pathstr.GetNSString()];
		if( fh == nil )
		{
			return true;
		}
		*/
		
		/*#1243
		//排他ロックをとってみる
		AStCreateCstringFromAText	str(path);
		int	fd = ::open((const char*)(str.GetPtr()),O_NONBLOCK|O_EXLOCK);
		if( fd != -1 )
		{
			::close(fd);
		}
		else
		{
			//排他ロックとれなければtrueを返す
			return true;
		}
		*/
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::IsReadOnly() @catch",this);
	}
	return false;
	
	/*#1034
	FSSpec	fsspec;
	if( GetFSSpec(fsspec) == false )   return false;
	HFileParam		fp;
	fp.ioNamePtr = fsspec.name;
	fp.ioVRefNum = fsspec.vRefNum;
	fp.ioFDirIndex = 0;
	fp.ioDirID = fsspec.parID;
	::PBHGetFInfoSync((HParmBlkPtr)&fp);
	if( (fp.ioFlAttrib&1) != 0 )   return true;
	
	ABool	result = false;
	//write permission check
	short	fRefNum;
	//OSErr	err = UFSSpec::OpenFileWriteLock(mFileSpec,&fRefNum);
	OSErr err = ::FSpOpenDF(&fsspec,fsWrPerm,&fRefNum);
	if( err != noErr || fRefNum == 0 )
	{
		result = true;
	}
	else 
	{ 
	  ::FSClose(fRefNum);
	}
	//B0019 リソースフォークが書き込み可能かどうかも調べるように変更
	short rRefNum = ::FSpOpenResFile(&fsspec,fsRdPerm);//まず読み取りパーミションで開く
	if( rRefNum != -1 )//リソースフォークが無いなら-1
	{
	  ::CloseResFile(rRefNum);
		
		rRefNum = ::FSpOpenResFile(&fsspec,fsWrPerm);
		if( rRefNum == -1 )
		{
			result =true;
		}
		else
		{
		  ::CloseResFile(rRefNum);
		}
	}
	return result;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::IsReadOnly() const
{
	if( mPath.GetItemCount() == 0 )   return false;
	
	//最後に\があると正常に判定されない
	AText	path(mPath);
	if( path.Get(path.GetItemCount()-1) == GetPathDelimiter() )
	{
		path.DeleteAfter(path.GetItemCount()-1);
	}
	
	//書き込み権限チェック
	AStCreateWcharStringFromAText	pathstr(path);
	return (_waccess_s(pathstr.GetPtr(),2)!=0);//2:書き込み権限チェック
}
#endif

//Rootか？
ABool	AFileAcc::IsRoot() const
{
	return (mPath.GetItemCount() <= 1);
}

//B0313
//Invisibleファイル・フォルダか？
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::IsInvisible() const
{
	AText	filename;
	GetFilename(filename);
	if( filename.GetItemCount() > 0 )
	{
		if( filename.Get(0) == '.' )   return true;
	}
	return false;
	/*#1034 #1090
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	GetFSRef(fsref);
	::FSGetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo,NULL,NULL,NULL);
	return (((((FileInfo*)(&(catinfo.finderInfo[0])))->finderFlags)&kIsInvisible)!=0);
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::IsInvisible() const
{
	AText	filename;
	GetFilename(filename);
	if( filename.GetItemCount() > 0 )
	{
		if( filename.Get(0) == '.' )   return true;
	}
	//★確認 その他のinvisible条件検討
	return false;
}
#endif

//R0000 security
//★3.0.0b6時点でどこからも使用されていないため未テスト
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::IsExecutable() const
{
	@try
	{
		AText	path;
		GetNormalizedPath(path);
		AStCreateNSStringFromAText	pathstr(path);
		
		if( [[NSFileManager defaultManager] isExecutableFileAtPath:pathstr.GetNSString()] == NO )
		{
			return true;
		}
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::IsExecutable() @catch",this);
	}
	return false;
	
	/*#1034
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	GetFSRef(fsref);
	::FSGetCatalogInfo(&fsref,kFSCatInfoPermissions,&catinfo,NULL,NULL,NULL);
	UInt16	mode = ((FSPermissionInfo*)(&(catinfo.permissions[0])))->mode;
	return (((mode&S_IXUSR)!=0)||((mode&S_IXGRP)!=0)||((mode&S_IXOTH)!=0));
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::IsExecutable() const
{
	if( mPath.GetItemCount() == 0 )   return false;
	
	//最後に\があると正常に判定されない
	AText	path(mPath);
	if( path.Get(path.GetItemCount()-1) == GetPathDelimiter() )
	{
		path.DeleteAfter(path.GetItemCount()-1);
	}
	
	//フォルダかどうかの判定
	AStCreateWcharStringFromAText	pathstr(path);
	struct __stat64	st = {0};
	if( _wstat64(pathstr.GetPtr(),&st) == 0 )//【OS互換性】Win95/98では使えない。WinNT以降
	{
		return ((st.st_mode&_S_IEXEC)!=0);
	}
	else
	{
		return false;
	}
	//★未テスト
}
#endif

#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::GetFileAttribute( AFileAttribute& outFileAttribute ) const
{
	//#1425
	//パス取得
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	//attribute取得
	NSError *anError = nil;
	NSDictionary*	dic = [[NSFileManager defaultManager] attributesOfItemAtPath:pathstr.GetNSString() error:&anError];
	if( dic != nil )
	{
		//creator/type取得（creator/type未設定なら0が取得される）
		outFileAttribute.creator = [dic fileHFSCreatorCode];
		outFileAttribute.type = [dic fileHFSTypeCode];
		return true;
	}
	return false;
	
	/*#1425
	outFileAttribute.creator = 0;
	outFileAttribute.type = 0;
	//クリエータ／タイプ
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	::FSGetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo,NULL,NULL,NULL);
	outFileAttribute.creator = ((FileInfo*)(&(catinfo.finderInfo[0])))->fileCreator;
	outFileAttribute.type = ((FileInfo*)(&(catinfo.finderInfo[0])))->fileType;
	return true;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::GetFileAttribute( AFileAttribute& outFileAttribute ) const
{
	//当面、ダミー処理のみ
	outFileAttribute.tmp = 0;
	return true;
}
#endif

#if IMPLEMENTATION_FOR_MACOSX
//win
ABool	AFileAcc::SetFileAttribute( const AFileAttribute& inFileAttribute ) const
{
	//#1425
	//パス取得
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	//creator/typeのNSDictionary作成
	NSDictionary*	dic = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithUnsignedLong:inFileAttribute.creator], NSFileHFSCreatorCode,
		[NSNumber numberWithUnsignedLong:inFileAttribute.type], NSFileHFSTypeCode,
		nil
	];
	//attribute設定
	NSError *anError = nil;
	if( [[NSFileManager defaultManager] setAttributes:dic ofItemAtPath:pathstr.GetNSString() error:&anError] == YES )
	{
		return true;
	}
	else
	{
		return false;
	}
	
	/*#1425
	//クリエータ／タイプ書き込み
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	::FSGetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo,NULL,NULL,NULL);
	((FileInfo*)(&(catinfo.finderInfo[0])))->fileCreator = inFileAttribute.creator;
	((FileInfo*)(&(catinfo.finderInfo[0])))->fileType = inFileAttribute.type;
	::FSSetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo);
	return true;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::SetFileAttribute( const AFileAttribute& inFileAttribute ) const
{
	//当面、処理なし
	return true;
}
#endif

//R0208
//ファイルサイズ取得
#if IMPLEMENTATION_FOR_MACOSX
AItemCount	AFileAcc::GetFileSize() const
{
	return AFileWrapper::GetFileSize(mPath);
	/*
	SInt64	forksize = 0;
	SInt16	forkref = 0;
	try
	{
		FSRef	ref;
		if( GetFSRef(ref) == false )   throw 0;
		HFSUniStr255	forkName;
		::FSGetDataForkName(&forkName);
		OSErr	err;
		err = ::FSOpenFork(&ref,forkName.length,forkName.unicode,fsRdPerm,&forkref);
		if( err != noErr )   throw 0;
		err = ::FSGetForkSize(forkref,&forksize);
		::FSCloseFork(forkref);
	}
	catch(...)
	{
		if( forkref != 0 )   ::FSCloseFork(forkref);
	}
	return forksize;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
AItemCount	AFileAcc::GetFileSize() const
{
	AItemCount	len = 0;
	FILE*	fp = NULL;
	try
	{
		AStCreateWcharStringFromAText	pathstr(mPath);
		if( _wfopen_s(&fp,pathstr.GetPtr(),L"rb") != 0 )   throw 0;//_wfopen_sはWin95から対応
		len = _filelength(_fileno(fp));
		fclose(fp);
	}
	catch(...)
	{
		if( fp != NULL )   fclose(fp);
	}
	return len;
	//★未テスト
}
#endif

#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::GetLastModifiedDataTime( ADateTime& outDateTime ) const
{
	//#1425
	//パス取得
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	//attribute取得
	NSError *anError = nil;
	NSDictionary*	dic = [[NSFileManager defaultManager] attributesOfItemAtPath:pathstr.GetNSString() error:&anError];
	if( dic != nil )
	{
		//ファイル変更日時取得
		NSDate*	modificationDate = [dic fileModificationDate];
		if( modificationDate != nil )
		{
			//ADateTime(CFAbsoluteTime)へ変換
			outDateTime = ::CFDateGetAbsoluteTime((CFDateRef)modificationDate);
			return true;
		}
	}
	return false;
	
	/*#1425
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	OSErr	err = ::FSGetCatalogInfo(&fsref,kFSCatInfoContentMod,&catinfo,NULL,NULL,NULL);
	if( err != noErr )   return false;//#653
	*#380
	LocalDateTime	local;
	::ConvertUTCToLocalDateTime(&(catinfo.contentModDate),&local);
	outDateTime = local.lowSeconds;
	*
	::UCConvertUTCDateTimeToCFAbsoluteTime(&(catinfo.contentModDate),&outDateTime);//#380
	return true;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::GetLastModifiedDataTime( ADateTime& outDateTime ) const
{
	if( mPath.GetItemCount() == 0 )   return false;
	
	//最後に\があると正常に判定されない
	AText	path(mPath);
	if( path.Get(path.GetItemCount()-1) == GetPathDelimiter() )
	{
		path.DeleteAfter(path.GetItemCount()-1);
	}
	
	//
	AStCreateWcharStringFromAText	pathstr(path);
	struct __stat64	st = {0};
	int err = _wstat64(pathstr.GetPtr(),&st);//【OS互換性】Win95/98では使えない。WinNT以降
	outDateTime = st.st_mtime;
	return (err==0);
}
#endif

//R0137
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::IsBundleFolder() const
{
	if( IsFolder() == false )   return false;
	
	ABool	result = false;
	@try 
	{
		//パス取得
		AText	path;
		GetNormalizedPath(path);
		AStCreateNSStringFromAText	pathstr(path);
		//パッケージかどうかの判定
		result = ([[NSWorkspace sharedWorkspace] isFilePackageAtPath:pathstr.GetNSString()]==YES);
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::IsBundleFolder() @catch",this);
	}
	return result;
	/*#1034
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	
	LSItemInfoRecord	info;
	::LSCopyItemInfoForRef(&fsref,kLSRequestAllFlags,&info);
	if( (info.flags & kLSItemInfoIsPackage ) != 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::IsBundleFolder() const
{
	return false;
}
#endif

/**
指定フォルダの子ファイルかどうかを取得
*/
ABool	AFileAcc::IsChildOfFolder( const AFileAcc& inFolder ) const
{
	//パス取得
	AText	path;
	GetPath(path);
	//フォルダパス取得
	AText	folderPath;
	inFolder.GetPath(folderPath);
	//フォルダ配下のファイルかどうかを判定
	if( folderPath.GetItemCount() < path.GetItemCount() )
	{
		if( folderPath.Compare(path,0,folderPath.GetItemCount()) == true )
		{
			return true;
		}
	}
	return false;
}

#pragma mark ===========================

#pragma mark --- フォルダ内ファイル取得

//フォルダ内ファイル取得
#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::GetChildren( AObjectArray<AFileAcc>& outChilds ) const
{
	outChilds.DeleteAll();
	//#1034
	@try 
	{
		//パス取得
		AText	path;
		GetNormalizedPath(path);
		AStCreateNSStringFromAText	pathstr(path);
		//子ファイル・フォルダリスト取得
		NSArray*	contents_raw = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:pathstr.GetNSString() error:nil];//★解放必要？
		if( contents_raw != nil )
		{
			//名前順にソート #1245
			NSArray*  contents = [contents_raw sortedArrayUsingComparator:^NSComparisonResult(id obj1, id obj2) {
					NSString*	str1 = (NSString*)obj1;
					NSString*	str2 = (NSString*)obj2;
					return [str1 compare:str2];
			}];//★解放必要？
			
			//子ファイル・フォルダごとのループ
			AItemCount	count = [contents count];
			for( AIndex i = 0; i < count; i++ )
			{
				//子のファイル名取得
				NSString*	str = [contents objectAtIndex:i];
				AText	childText;
				ACocoa::SetTextFromNSString(str,childText);
				//結果格納
				AIndex	newIndex = outChilds.AddNewObject();
				outChilds.GetObject(newIndex).SpecifyChild(path,childText);
			}
		}
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::GetChildren() @catch",this);
	}
	/*#1034
	OSErr	err;
	try
	{
		FSRef	ref;
		if( GetFSRef(ref) == false )   throw 0;
		FSCatalogBulkParam	param;
		err = ::FSOpenIterator(&ref,kFSIterateFlat,&(param.iterator));
		if( err != noErr )   throw 0;
		param.container = &ref;
		param.catalogInfo = NULL;
		param.maximumItems = 1;
		param.whichInfo = kFSCatInfoNone;
		FSRef	childfsref;
		param.refs = &childfsref;
		param.names = NULL;
		param.specs = NULL;
		while(true)
		{
			err = ::PBGetCatalogInfoBulkSync(&param);
			if( err == errFSNoMoreItems )   break;
			if( err != noErr )   throw 0;
			AIndex	newIndex = outChilds.AddNewObject();
			outChilds.GetObject(newIndex).SpecifyByFSRef(childfsref);
		}
		::FSCloseIterator(param.iterator);
	}
	catch(...)
	{
	}
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::GetChildren( AObjectArray<AFileAcc>& outChilds ) const
{
	outChilds.DeleteAll();
	AText	p;
	p.SetText(mPath);
	if( p.GetLastChar() != '\\' )   p.Add('\\');
	p.AddCstring("*.*");
	AStCreateWcharStringFromAText	pathstr(p);
	struct _wfinddata_t finddata;
	long	h = ::_wfindfirst(pathstr.GetPtr(),&finddata);///【OS互換性】Win95/98では使えない。WinNT以降
	if( h != -1 )
	{
		while( true )
		{
			AText	filename;
			filename.SetFromWcharString(finddata.name,sizeof(finddata.name)/sizeof(finddata.name[0]));
			if( filename.Compare(".") == false && filename.Compare("..") == false )
			{
				AText	path;
				path.SetText(mPath);
				if( path.GetLastChar() != '\\' )   path.Add('\\');
				path.AddText(filename);
				AIndex	newIndex = outChilds.AddNewObject();
				outChilds.GetObject(newIndex).Specify(path);
			}
			if( ::_wfindnext(h,&finddata) != 0 )   break;
		}
		::_findclose(h);
	}
}
#endif

#pragma mark ===========================

#pragma mark --- ファイル読み書き

//読み込み
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::ReadTo( AText &outText/*#1034, ABool inResourceFork*/ ) const
{
	__block	ABool	result = false;//#1422
	outText.DeleteAll();
	
	//File Coordinator生成 #1422
	NSFileCoordinator*	fileCoordinator = [[NSFileCoordinator alloc] initWithFilePresenter:nil];
	//File Coordinatorにてファイル読み込み #1422
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	NSURL*	url = [NSURL fileURLWithPath:pathstr.GetNSString() isDirectory:NO];
	NSError *anError = nil;
	[fileCoordinator coordinateReadingItemAtURL:url options:0 error:&anError byAccessor:^(NSURL *newURL)
		{
			//
			NSFileHandle* fh = nil;
			@try 
			{
				//パス取得
				NSString*	newPath = newURL.path;//#1422 File Coordinatorで取得したpathへ書き込むようにする。
				//ファイルハンドラ取得
				fh = [NSFileHandle fileHandleForReadingAtPath:newPath];//#1422
				if( fh != nil )
				{
					//NSDataに読み込み
					NSData* data = [fh readDataToEndOfFile];
					if( data != nil )
					{
						//outTextへ格納
						AByteCount	len = [data length];
						if( len > 0 )
						{
							outText.Reserve(0,len);
							outText.SetFromTextPtr((const AConstUCharPtr)[data bytes],len);
							result = true;
						}
					}
				}
			}
			@catch(NSException *theException)
			{
				_ACError("AFileAcc::ReadTo() @catch",this);
			}
			//ファイルハンドラクローズ
			if( fh != nil )
			{
				[fh closeFile];
			}
	}];
	//File Coordinator解放 #1422
	[fileCoordinator release];
	return result;
	
	/*#1034
	SInt16	forkref = 0;
	try
	{
		FSRef	ref;
		if( GetFSRef(ref) == false )   throw 0;
		HFSUniStr255	forkName;
		if( not inResourceFork )
		{
			::FSGetDataForkName(&forkName);
		}
		else
		{
			::FSGetResourceForkName(&forkName);
		}
		OSErr	err;
		err = ::FSOpenFork(&ref,forkName.length,forkName.unicode,fsRdPerm,&forkref);
		if( err != noErr )   throw 0;
		SInt64	forksize = 0;
		err = ::FSGetForkSize(forkref,&forksize);
		outText.Reserve(0,forksize);
		ByteCount	actualCount = 0;
		{//#598 arrayptrの有効範囲を最小にする
			AStTextPtr	textptr(outText,0,outText.GetItemCount());
			err = ::FSReadFork(forkref,fsFromStart,0,textptr.GetByteCount(),textptr.GetPtr(),&actualCount);
		}
		if( actualCount < outText.GetLength() )
		{
			_ACError("file read actual count is less than file size",this);
			outText.Delete(actualCount,outText.GetLength()-actualCount);//★ もしこの事象が発生した場合、lock中（textptr使用中）なのでDeleteは不可
		}
		if( err != noErr )   throw 0;
		::FSCloseFork(forkref);
	}
	catch(...)
	{
		outText.DeleteAll();
		if( forkref != 0 )   ::FSCloseFork(forkref);
		result = false;
	}
	*/
	return result;
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::ReadTo( AText &outText, ABool inResourceFork ) const
{
	ABool	result = true;
	outText.DeleteAll();
	
	FILE*	fp = NULL;
	try
	{
		AStCreateWcharStringFromAText	pathstr(mPath);
		if( _wfopen_s(&fp,pathstr.GetPtr(),L"rb") != 0 )   throw 0;//_wfopen_sはWin95から対応
		AItemCount	len = _filelength(_fileno(fp));
		outText.Reserve(0,len);
		size_t	readsize = 0;
		{//#598 arrayptrの有効範囲を最小にする
			AStTextPtr	textptr(outText,0,outText.GetItemCount());
			readsize = fread(textptr.GetPtr(),sizeof(char),textptr.GetByteCount(),fp);
		}
		if( readsize < outText.GetItemCount() )
		{
			_ACError("file read actual count is less than file size",this);
			outText.Delete(readsize,outText.GetItemCount()-readsize);
		}
		fclose(fp);
	}
	catch(...)
	{
		outText.DeleteAll();
		if( fp != NULL )   fclose(fp);
		result = false;
	}
	return result;
}
#endif

#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::WriteFile( const AText& inText, AFileError& outError ) const
{
	__block ABool	result = false;//#1422
	outError = kFileError_General;
	
	//File Coordinator生成 #1422
	NSFileCoordinator*	fileCoordinator = [[NSFileCoordinator alloc] initWithFilePresenter:nil];
	//File Coordinatorにてファイル書き込み #1422
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	NSURL*	url = [NSURL fileURLWithPath:pathstr.GetNSString() isDirectory:NO];
	NSError *anError = nil;
	[fileCoordinator coordinateWritingItemAtURL:url options:NSFileCoordinatorWritingForReplacing error:&anError byAccessor:^(NSURL *newURL)
		{
			//
			NSFileHandle* fh;
			@try 
			{
				//パス取得
				NSString*	newPath = newURL.path;//#1422 File Coordinatorで取得したpathへ書き込むようにする。
				//ファイルハンドラ取得
				fh = [NSFileHandle fileHandleForWritingAtPath:newPath];//#1422
				if( fh != nil )
				{
					//NSData作成
					AStTextPtr	textptr(inText,0,inText.GetItemCount());
					NSData*	data = [NSData dataWithBytes:textptr.GetPtr() length:textptr.GetByteCount()];
					if( data != nil )
					{
						//ファイルに書き込み
						[fh writeData:data];
						//サイズ設定
						[fh truncateFileAtOffset:textptr.GetByteCount()];
						//結果OK
						result = true;
					}
				}
			}
			@catch(NSException *theException)
			{
				//_AError("AFileAcc::WriteFile() @catch",this);
				outError = kFileError_DiskFull;
			}
			//ファイルハンドラクローズ
			if( fh != nil )
			{
				[fh closeFile];
			}
	}];
	//File Coordinator解放 #1422
	[fileCoordinator release];
	return result;
	
	
	/*#1304
	SInt16	forkref = 0;
	try
	{
		FSRef	ref;
		if( GetFSRef(ref) == false )   throw 0;
		HFSUniStr255	forkName;
		::FSGetDataForkName(&forkName);
		outError = ::FSOpenFork(&ref,forkName.length,forkName.unicode,fsRdWrPerm,&forkref);
		if( outError != noErr )   throw 0;
		ByteCount	actualCount = 0;
		{//#598 arrayptrの有効範囲を最小にする
			AStTextPtr	textptr(inText,0,inText.GetItemCount());
			outError = ::FSWriteFork(forkref,fsFromStart,0,textptr.GetByteCount(),textptr.GetPtr(),&actualCount);
		}
		if( actualCount < inText.GetLength() )
		{
			_ACError("file write actual count is less than file size",this);
			result = false;
		}
		//if( outError != noErr )   throw 0;
		::FSSetForkSize(forkref,fsFromStart,actualCount);
		::FSCloseFork(forkref);
	}
	catch(...)
	{
		if( forkref != 0 )   ::FSCloseFork(forkref);
		result = false;
	}
	return result;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::WriteFile( const AText& inText, AFileError& outError ) const
{
	ABool	result = true;
	FILE*	fp = NULL;
	try
	{
		AStCreateWcharStringFromAText	pathstr(mPath);
		if( _wfopen_s(&fp,pathstr.GetPtr(),L"wb") != 0 )   throw 0;//_wfopen_sはWin95から対応
		size_t	count = 0;
		{//#598 arrayptrの有効範囲を最小にする
			AStTextPtr	textptr(inText,0,inText.GetItemCount());
			count = fwrite(textptr.GetPtr(),sizeof(char),textptr.GetByteCount(),fp);
		}
		if( count < inText.GetLength() )
		{
			_ACError("file write actual count is less than file size",this);
			result = false;
		}
		fclose(fp);
	}
	catch(...)
	{
		if( fp != NULL )   fclose(fp);
		result = false;
	}
	return result;
}
#endif

#pragma mark ===========================

#pragma mark --- ファイル操作

//ファイル作成
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::CreateFile() const
{
	ABool	result = false;
	//既にファイル存在なら何もせずリターン
	//（createFileAtPathはファイル上書きしてしまうので、すでに存在しているならここでリターン必要。）
	if( Exist() == true )
	{
		return true;
	}
	@try 
	{
		//空データ作成
		NSData*	data = [NSData data];
		//パス取得
		AText	path;
		GetNormalizedPath(path);
		AStCreateNSStringFromAText	pathstr(path);
		//ファイル生成
		result = ([[NSFileManager defaultManager] createFileAtPath:pathstr.GetNSString() 
														  contents:data 
														attributes:nil] == YES);
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::CreateFile() @catch",this);
	}
	return result;
	/*#1034
	AFileAcc	parent;
	parent.SpecifyParent(*this);
	FSRef	parentref;
	if( parent.GetFSRef(parentref) == false )   return false;
	AText	filename;
	GetFilename(filename);
	AUniText	filenameUTF16;
	filename.GetUTF16Text(filenameUTF16);
	AStUniTextPtr	textptr(filenameUTF16,0,filenameUTF16.GetItemCount());
	OSErr	err = ::FSCreateFileUnicode(&parentref,filenameUTF16.GetItemCount(),textptr.GetPtr(),kFSCatInfoNone,NULL,NULL,NULL);
	return (err==noErr);
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::CreateFile() const
{
	AStCreateWcharStringFromAText	pathstr(mPath);
	int	fh = NULL;
	if( _wsopen_s(&fh,pathstr.GetPtr(),_O_CREAT,_SH_DENYRW,_S_IREAD|_S_IWRITE) != 0 )   return false;
	_close(fh);
	return true;
}
#endif

//フォルダ作成
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::CreateFolder()
{
	if( Exist() )
	{
		if( IsFolder() )   return true;
		else return false;
	}
	ABool	result = false;
	@try 
	{
		//パス取得
		AText	path;
		GetNormalizedPath(path);
		AStCreateNSStringFromAText	pathstr(path);
		//フォルダ生成
		result = ([[NSFileManager defaultManager] createDirectoryAtPath:pathstr.GetNSString() 
											withIntermediateDirectories:NO attributes:nil error:nil] == YES);
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::CreateFolder() @catch",this);
	}
	return result;
	/*#1034
	AFileAcc	parent;
	parent.SpecifyParent(*this);
	FSRef	parentref;
	if( parent.GetFSRef(parentref) == false )   return false;
	AText	filename;
	GetFilename(filename);
	AUniText	filenameUTF16;
	filename.GetUTF16Text(filenameUTF16);
	AStUniTextPtr	textptr(filenameUTF16,0,filenameUTF16.GetItemCount());
	OSErr	err = ::FSCreateDirectoryUnicode(&parentref,filenameUTF16.GetItemCount(),textptr.GetPtr(),kFSCatInfoNone,NULL,NULL,NULL,NULL);
	return (err==noErr);
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::CreateFolder()
{
	if( Exist() == true )
	{
		if( IsFolder() == true )   return true;
		else return false;
	}
	//
	AStCreateWcharStringFromAText	pathstr(mPath);
	if( _wmkdir(pathstr.GetPtr()) != 0 )   return false;//【OS互換性】_wmkdirはWin95/98未対応
	return true;
}
#endif

//#427
/**
フォルダ作成（再帰的）
*/
ABool	AFileAcc::CreateFolderRecursive()
{
	AFileAcc	parent;
	if( parent.SpecifyParent(*this) == false )   return false;//パスデリミタが存在しない場合
	if( parent.Exist() == false )
	{
		//親フォルダが存在していなければ、親フォルダを再帰的に作成
		if( parent.CreateFolderRecursive() == false )   return false;
	}
	return CreateFolder();
}

/**
名称変更
@note AFileAcc自体の名称も変更
*/
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::Rename( const AText& inNewName )
{
	//#1425
	//パス取得
	AText	oldPath;
	GetNormalizedPath(oldPath);
	AStCreateNSStringFromAText	oldPathstr(oldPath);
	//親フォルダ取得
	AFileAcc	parent;
	if( parent.SpecifyParent(*this) == false )   return false;
	//名称変更後パス取得
	AFileAcc	newFile;
	newFile.SpecifyChild(parent, inNewName);
	AText	newPath;
	newFile.GetNormalizedPath(newPath);
	AStCreateNSStringFromAText	newPathstr(newPath);
	
	//ファイル名変更
	NSError *anError = nil;
  if( [[NSFileManager defaultManager] moveItemAtPath:oldPathstr.GetNSString() toPath:newPathstr.GetNSString() error:&anError] == NO )
	{
		return false;
	}
	
	/*#1425
	FSRef	ref;
	if( GetFSRef(ref) == false )   return false;
	
	AFileAcc	parent;
	parent.SpecifyParent(*this);
	AUniText	filenameUTF16;
	inNewName.GetUTF16Text(filenameUTF16);
	{//#598 arrayptrの有効範囲を最小にする
		AStUniTextPtr	textptr(filenameUTF16,0,filenameUTF16.GetItemCount());
		if( ::FSRenameUnicode(&ref,filenameUTF16.GetItemCount(),textptr.GetPtr(),kTextEncodingUnknown,NULL) != noErr )   return false;
	}
	*/
	
	//B0417 SpecifyChild(parent,inNewName);
	SpecifyChildFile(parent,inNewName);
	return true;//B0000
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::Rename( const AText& inNewName )
{
	AStCreateWcharStringFromAText	oldpathstr(mPath);
	AFileAcc	newFileAcc;
	newFileAcc.SpecifySister(*this,inNewName);
	AText	path;
	newFileAcc.GetPath(path);
	AStCreateWcharStringFromAText	newnamestr(path);
	if( _wrename(oldpathstr.GetPtr(),newnamestr.GetPtr()) != 0 )   return false;//【OS互換性】_wrenameはWin95/98未対応
	
	//自オブジェクトのデータを更新
	CopyFrom(newFileAcc);
	return true;
}
#endif

//ファイル削除
#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::DeleteFile()
{
	/*#1404 不具合の原因は不明だがFSDeleteObject()はdeprecatedなので新APIに差し替え
	FSRef	ref;
	if( GetFSRef(ref) == false )   return;
	::FSDeleteObject(&ref);
	*/
	DeleteFileOrFolder();
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::DeleteFile()
{
	AStCreateWcharStringFromAText	pathstr(mPath);
	_wremove(pathstr.GetPtr());
}
#endif

//#427
/**
フォルダ削除
*/
#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::DeleteFolder()
{
	/*#1404 不具合の原因は不明だがFSDeleteObject()はdeprecatedなので新APIに差し替え
	FSRef	ref;
	if( GetFSRef(ref) == false )   return;
	::FSDeleteObject(&ref);
	*/
	DeleteFileOrFolder();
}

//#1404
/**
ファイル・フォルダ削除
*/
void	AFileAcc::DeleteFileOrFolder()
{
	//パス取得
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	if( [pathstr.GetNSString() length] == 0 ) { _ACError("",this); return; }//pathstrの文字長が0の場合は、何もせずリターン（念の為）
	
	//フォルダが空でない場合は何もせずリターン（removeItemAtPathは常にrecursive削除なので、従来のFSDeleteObject()の動作に合わせる。これにより、万一コール元に（ファイル削除のつもりで違うフォルダを指定してしまっている、などの）不具合があっても内容のあるフォルダを削除することはない。）
	NSArray*	contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:pathstr.GetNSString() error:nil];
	if( contents != nil )
	{
		if( [contents count] > 0 )
		{
			return;
		}
	}
	//削除
	[[NSFileManager defaultManager] removeItemAtPath:pathstr.GetNSString() error:nil];
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::DeleteFolder()
{
	AStCreateWcharStringFromAText	pathstr(mPath);
	_wrmdir(pathstr.GetPtr());
}
#endif

//#427
/**
ファイル・フォルダ削除（再帰的）
*/
void	AFileAcc::DeleteFileOrFolderRecursive()
{
	if( IsFolder() == true )
	{
		//子をすべて再帰的に削除
		AObjectArray<AFileAcc>	children;
		GetChildren(children);
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			children.GetObject(i).DeleteFileOrFolderRecursive();
		}
		//フォルダ削除
		DeleteFolder();
	}
	else
	{
		//ファイル削除
		DeleteFile();
	}
}

//ファイルをinDstへコピー
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::CopyFileTo( AFileAcc& inDst, const ABool inInhibitOverwrite ) const//#427
{
	//コピー元とコピー先が同じファイルのときはエラー
	if( Compare(inDst) == true )
	{
		_ACError("",this);
		return false;
	}
	
	//
	if( not Exist() )   return false;
	
	//#427 書き込み先ファイル存在チェック
	if( inDst.Exist() == true && inInhibitOverwrite == true )
	{
		return false;
	}
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
	{
		/*#1425
		//R0000 MacOSX10.4以降はやっとAPIでcopyができる
		FSRef	srcFSRef;
		if( GetFSRef(srcFSRef) == false )   return false;//#427
		*/
		//コピー先親フォルダ取得
		AFileAcc	dstParent;
		dstParent.SpecifyParent(inDst);
		/*#1425
		FSRef	dstParentFSRef;
		if( dstParent.GetFSRef(dstParentFSRef) == false )   return false;//#427
		*/
		//コピー先ファイル名取得
		AText	dstfilename;
		inDst.GetFilename(dstfilename);
		
		//------------------コピー先のファイルが存在している場合はリネームする------------------
		AFileAcc	renamedDstFile;
		if( inDst.Exist() == true )
		{
			//ユニークファイル名を取得
			AFileAcc	uniqChildDstFile;
			uniqChildDstFile.SpecifyUniqChildFile(dstParent,dstfilename);
			AText	renamedDstFileName;
			uniqChildDstFile.GetFilename(renamedDstFileName);
			//既存のコピー先ファイルをユニークファイル名にリネームする
			renamedDstFile = inDst;
			renamedDstFile.Rename(renamedDstFileName);
		}
		//------------------コピー実行------------------
		{
			//#1425
			//コピー元パス取得
			AText	srcPath;
			GetNormalizedPath(srcPath);
			AStCreateNSStringFromAText	srcPathstr(srcPath);
			//コピー先パス取得
			AText	dstPath;
			inDst.GetNormalizedPath(dstPath);
			AStCreateNSStringFromAText	dstPathstr(dstPath);
			
			//ファイルコピー
			NSError *anError = nil;
			if( [[NSFileManager defaultManager] copyItemAtPath:srcPathstr.GetNSString() toPath:dstPathstr.GetNSString() error:&anError] == NO )
			{
				//処理なし（コピー先ファイル有無でコピー成功を判断しているため）
			}
			
			/*#1425
			AStCreateCFStringFromAText	destName(dstfilename);
			::FSCopyObjectSync(&srcFSRef,&dstParentFSRef,destName.GetRef(),NULL,kFSFileOperationDefaultOptions);
			*/
		}
		//------------------リネームファイルを作成した場合は、コピー成功時：リネームファイルを削除、コピー失敗時：リネームファイルを元の名前に戻す------------------
		if( renamedDstFile.IsSpecified() == true )
		{
			if( inDst.Exist() == true )
			{
				//コピー成功時：リネームファイルを削除
				renamedDstFile.DeleteFile();
			}
			else
			{
				//コピー失敗時：リネームファイルを元の名前に戻す
				renamedDstFile.Rename(dstfilename);
			}
		}
	}
	else
	{
		//#1034 Mac OS X 10.5未満は未対応とする
		_ACError("",NULL);
#if 0
		//
		inDst.DeleteFile();//#427
		//
		if( inDst.CreateFile() == false )   return false;//#427
		AText	buffer;
		//DF
		ReadTo(buffer);
		inDst.WriteFile(buffer);
		/*#1034
		//RF
		ReadTo(buffer,true);
		inDst.WriteResourceFork(buffer);
		*/
		//creator/type
		FSSpec	srcFSSpec, dstFSSpec;
		GetFSSpec(srcFSSpec);
		inDst.GetFSSpec(dstFSSpec);
		FInfo	src_fi;//B0000, dst_fi;
		::FSpGetFInfo(&srcFSSpec,&src_fi);
		//B0000::FSpGetFInfo(&dstFSSpec,&dst_fi);
		//B0000dst_fi.fdCreator = src_fi.fdCreator;
		//B0000dst_fi.fdType = src_fi.fdType;
		::FSpSetFInfo(&dstFSSpec,&src_fi);
#endif
	}
	return true;//#427
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::CopyFileTo( AFileAcc& inDst, const ABool inInhibitOverwrite ) const//#427
{
	//コピー元とコピー先が同じファイルのときはエラー
	if( Compare(inDst) == true )
	{
		_ACError("",this);
		return false;
	}
	
	//
	if( Exist() == false )   return false;
	
	//#427 書き込み先ファイル存在チェック
	if( inDst.Exist() == true && inInhibitOverwrite == true )
	{
		return false;
	}
	
	inDst.CreateFile();
	if( inDst.Exist() == false )   return false;
	AText	buffer;
	ReadTo(buffer);
	inDst.WriteFile(buffer);
	
	return true;//#427
	//★SHFileOperationを使用した方がよい?
}
#endif

/*#427
//フォルダをinDstへコピー
#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::CopyFolderTo( AFileAcc& inDst ) const
{
	if( not Exist() )   return;
	if( AEnvWrapper::GetOSVersion() >= 0x1040 )
	{
		//R0000 MacOSX10.4以降はやっとAPIでcopyができる
		FSRef	srcFSRef;
		if( GetFSRef(srcFSRef) == false )   return;
		AFileAcc	dstParent;
		dstParent.SpecifyParent(inDst);
		FSRef	dstParentFSRef;
		if( dstParent.GetFSRef(dstParentFSRef) == false )   return;
		AText	dstfilename;
		inDst.GetFilename(dstfilename);
		{
			AStCreateCFStringFromAText	destName(dstfilename);
			::FSCopyObjectSync(&srcFSRef,&dstParentFSRef,destName.GetRef(),NULL,kFSFileOperationDefaultOptions);
		}
	}
	else
	{
	if( inDst.CreateFolder() == false )   return;
	AObjectArray<AFileAcc>	children;
	GetChildren(children);
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		AText	filename;
		children.GetObject(i).GetFilename(filename,false);
		AFileAcc	dstChild;
		dstChild.SpecifyChildFile(inDst,filename);
		if( children.GetObject(i).IsFolder() && children.GetObject(i).IsLink() == false )//#0 シンボリックリンク・エイリアスによる無限ループ防止
		{
			children.GetObject(i).CopyFolderTo(dstChild);
		}
		else
		{
			children.GetObject(i).CopyFileTo(dstChild);
		}
	}
	}
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
*/

//#427
/**
*/
void	AFileAcc::CopyFolderTo( AFileAcc& inDst, const ABool inInhibitOverwrite, const ABool inOnlyWhenNoDstFolder ) const //#427
{
	if( Exist() == false )   return;
	
	//#427
	if( inOnlyWhenNoDstFolder == true )
	{
		if( inDst.Exist() == true )   return;
	}
	//
	inDst.CreateFolderRecursive();
	//
	AObjectArray<AFileAcc>	children;
	GetChildren(children);
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		AText	filename;
		children.GetObject(i).GetFilename(filename,false);
		AFileAcc	dstChild;
		dstChild.SpecifyChildFile(inDst,filename);
		if( children.GetObject(i).IsFolder() && children.GetObject(i).IsLink() == false )//#0 シンボリックリンク・エイリアスによる無限ループ防止
		{
			children.GetObject(i).CopyFolderTo(dstChild,inInhibitOverwrite,inOnlyWhenNoDstFolder);//#427
		}
		else
		{
			children.GetObject(i).CopyFileTo(dstChild,inInhibitOverwrite);//#427
		}
	}
}
//#427 #endif

//#427
/**
ファイルまたはフォルダを指定先へコピー
*/
void	AFileAcc::CopyFileOrFolderTo( AFileAcc& inDst, const ABool inInhibitOverwrite, const ABool inOnlyWhenNoDstFolder )
{
	if( IsFolder() == true && IsLink() == false )
	{
		//==================フォルダを指定先へコピー==================
		CopyFolderTo(inDst,inInhibitOverwrite,inOnlyWhenNoDstFolder);
	}
	else
	{
		//==================ファイルを指定先へコピー==================
		//コピー先の親フォルダが未生成ならフォルダ生成
		AFileAcc	parentFolder;
		parentFolder.SpecifyParent(inDst);
		parentFolder.CreateFolderRecursive();
		//ファイルをコピー
		CopyFileTo(inDst,inInhibitOverwrite);
	}
}

#pragma mark ===========================

#pragma mark --- リンク／エイリアス

#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::ResolveAnyLink( const AFileAcc& inFileMayLink )
{
	//#1425
	//まずinFileMayLinkを自身にコピー
	CopyFrom(inFileMayLink);
	//エイリアス解決
	ResolveAlias();
	
	/*#1425
	FSRef	fsref;
	if( inFileMayLink.GetFSRef(fsref) == false )   return;
	Boolean isFolder,isAlias;
	::FSResolveAliasFile(&fsref,true,&isFolder,&isAlias);
	SpecifyByFSRef(fsref);
	*/
}
ABool	AFileAcc::ResolveAlias()
{
	//URL取得
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	NSURL*	url = [NSURL fileURLWithPath:pathstr.GetNSString()];
	//URLからbookmarkを生成（URLがエイリアスファイルでなければ、bookmarkはNULLとなる）
	CFErrorRef	error = nil;
	CFDataRef	bookmark = ::CFURLCreateBookmarkDataFromFile(NULL, (CFURLRef)url, &error);
	if( bookmark != NULL )
	{
		//bookmarkからエイリアス解決済みURLを生成
		ABool	isStale = false;
		NSURL*	resolvedURL = (NSURL*)::CFURLCreateByResolvingBookmarkData(NULL, bookmark, kCFBookmarkResolutionWithoutUIMask, NULL, NULL, &isStale, &error);
		if( resolvedURL != nil )
		{
			//bookmarkから生成したURLはファイル参照URLになっているので、普通のファイルパスURLへ変換する。
			NSURL*	filePathURL = resolvedURL.filePathURL;
			if( filePathURL != nil )
			{
				//自身に解決済みパスを設定
				AText	resolvedPath;
				resolvedPath.SetFromNSString(filePathURL.path);
				Specify(resolvedPath);
				return true;
			}
		}
	}
	return false;
	
	/*#1425
	//エイリアス解決
	FSRef	fsref;
	if( GetFSRef(fsref) == true )
	{
		Boolean	isFolder,isAlias;
		::FSResolveAliasFile(&fsref,true,&isFolder,&isAlias);
		SpecifyByFSRef(fsref);
	}
	*/
}
//#0
/**
シンボリックリンク・エイリアスかどうかを返す
*/
ABool	AFileAcc::IsLink()
{
	//エイリアスかどうかを取得
	//#1425
	AFileAcc	file;
	file.CopyFrom(*this);
	if( file.ResolveAlias() == true )
	{
		return true;
	}
	/*#1425
	FSRef	fsref;
	if( GetFSRef(fsref) == true )
	{
		Boolean	isFolder,isAlias;
		::FSResolveAliasFile(&fsref,true,&isFolder,&isAlias);
		if( isAlias == true )   return true;
	}
	*/
	//シンボリックリンクかどうかを取得
	AText	path;
	GetPath(path);
	AStCreateCstringFromAText	pathstr(path);
	struct stat	sta;
	::lstat(pathstr.GetPtr(),&sta);
	if( S_ISLNK(sta.st_mode) == true )   return true;
	return false;
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::ResolveAnyLink( const AFileAcc& inFileMayLink )
{
	CopyFrom(inFileMayLink);
	ResolveAlias();
}
void	AFileAcc::ResolveAlias()
{
	if( Exist() == false )   return;
	if( IsLink() == false )   return;
	
	// Get a pointer to the IShellLink interface. 
	IShellLink*	psl = NULL;
	HRESULT	hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
	if( hres != S_OK )   {_ACError("",NULL);return;}
	
	// Get a pointer to the IShellLink interface. 
	IPersistFile* ppf = NULL;
	hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
	if( hres != S_OK )   {psl->Release();_ACError("",NULL);return;}
	
	// Load the shortcut. 
	AStCreateWcharStringFromAText	pathstr(mPath);
	hres = ppf->Load(pathstr.GetPtr(), STGM_READ); 
	if( hres != S_OK )   {ppf->Release();psl->Release();_ACError("",NULL);return;}
	
	// Resolve the link. 
	hres = psl->Resolve(NULL, 0); 
	if( hres != S_OK )   {ppf->Release();psl->Release();_ACError("",NULL);return;}
	
	// Get the path to the link target. 
	wchar_t	path[MAX_PATH];
	WIN32_FIND_DATA wfd = {0};
	hres = psl->GetPath(path,MAX_PATH,(WIN32_FIND_DATA*)&wfd,SLGP_UNCPRIORITY);
	if( hres == S_OK )
	{
		mPath.SetFromWcharString(path,sizeof(path)/sizeof(path[0]));
	}
	//Release
	ppf->Release();
	psl->Release();
}
//#0
/**
シンボリックリンク・エイリアスかどうかを返す
*/
ABool	AFileAcc::IsLink()//#0
{
	AText	suffix;
	mPath.GetSuffix(suffix);
	return (suffix.Compare(".lnk")==true);
}

//参考：http://msdn.microsoft.com/en-us/library/bb776891(VS.85).aspx
ABool	AFileAcc::CreateShellLink( const AFileAcc& inLinkFile ) const
{
	if( Exist() == false )   return false;
	
	// Get a pointer to the IShellLink interface. 
	IShellLink*	psl = NULL;
	HRESULT	hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
	if( hres != S_OK )   {_ACError("",NULL);return false;}
	// Set the path to the shortcut target and add the description. 
	AStCreateWcharStringFromAText	pathstr(mPath);
	psl->SetPath(pathstr.GetPtr());
	psl->SetDescription(L"");
	// Query IShellLink for the IPersistFile interface for saving the 
	// shortcut in persistent storage. 
	IPersistFile*	ppf = NULL;
	psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
	if( hres != S_OK )   {psl->Release();_ACError("",NULL);return false;}
	// Save the link by calling IPersistFile::Save. 
	AText	linkpath;
	inLinkFile.GetPath(linkpath);
	AStCreateWcharStringFromAText	linkpathstr(linkpath);
	hres = ppf->Save(linkpathstr.GetPtr(), TRUE); 
	ppf->Release(); 
	psl->Release();
	return (hres==S_OK);
}
#endif

#pragma mark ===========================

#pragma mark --- その他

#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::SpecifyAsUnitTestFolder()
{
	AFileAcc	docfolder;
	FSRef	ref;
	::FSFindFolder(kUserDomain,kDocumentsFolderType,true,&ref);
	docfolder.SpecifyByFSRef(ref);
	SpecifyChild(docfolder,"UnitTest");
	CreateFolder();
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::SpecifyAsUnitTestFolder()
{
	//マイドキュメント内のUnitTestフォルダ
	wchar_t	buf[MAX_PATH];
	::SHGetFolderPathW(NULL,CSIDL_PERSONAL,NULL,0,buf);
	AText	path;
	path.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	AFileAcc	mydocfolder;
	mydocfolder.Specify(path);
	SpecifyChild(mydocfolder,"UnitTest");
}
#endif

#pragma mark ===========================

#pragma mark --- MacOS専用

#if IMPLEMENTATION_FOR_MACOSX

//リソースフォーク用Mutex
AThreadMutex	AFileAcc::sResourceForkMutex;//#92

//MacOS限定
ABool	AFileAcc::ExistResource( const ResType& inResType, const short& inResNum ) const
{
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	
	//リソースアクセスロック
	//#92 複数スレッドからリソースファイルを開くとクラッシュするらしいので
	AStMutexLocker	mutexlocker(sResourceForkMutex);
	
	/*#1034 short*/ResFileRefNum	rRefNum = -1;
	HFSUniStr255	forkName;
	::FSGetResourceForkName(&forkName);
	::FSOpenResourceFile(&fsref,forkName.length,forkName.unicode,fsRdPerm,&rRefNum);
	if( rRefNum != -1 )
	{
		::CloseResFile(rRefNum);
		return true;
	}
	return false;
}

//MacOS限定
ABool	AFileAcc::ReadResouceTo( AText &outText, ResType inResType, short inResNum ) const
{
	ABool	result = false;
	/*#1034 short*/ResFileRefNum	rRefNum = -1;
	try
	{
		FSRef	fsref;
		if( GetFSRef(fsref) == false )   _AThrow("cannot find file",this);
		
		//リソースアクセスロック
		//#92 複数スレッドからリソースファイルを開くとクラッシュするらしいので
		AStMutexLocker	mutexlocker(sResourceForkMutex);
		
		HFSUniStr255	forkName;
		::FSGetResourceForkName(&forkName);
		::FSOpenResourceFile(&fsref,forkName.length,forkName.unicode,fsRdPerm,&rRefNum);
		if( rRefNum == -1 )   _AThrow("cannot open resource file",this);
		
		Handle	res = ::Get1Resource(inResType,inResNum);
		if( res != NULL )
		{
			::HLock(res);
			outText.SetFromTextPtr(*res,::GetHandleSize(res));
			::HUnlock(res);
			::ReleaseResource(res);
			result = true;
		}
		::CloseResFile(rRefNum);
	}
	catch(...)
	{
		outText.DeleteAll();
		if( rRefNum != -1 )   ::CloseResFile(rRefNum);
	}
	return result;
}

//#1034
#if 0
ABool	AFileAcc::WriteResouceFile( const AText &inText, const ResType inResType, const short inResNum ) const
{
	ABool	result = true;
	short	rRefNum = -1;
	Handle	res = NULL;
	try
	{
		FSRef	fsref;
		if( GetFSRef(fsref) == false )   _AThrow("cannot find file",this);
		
		FSSpec	fsspec;
		GetFSSpec(fsspec);
		//B0000  ::FSpCreateResFile(&fsspec,NULL,NULL,0);
		//B0000 リソースフォークが存在しないファイルにcreator,type設定→新規にリソースフォーク作成、でcreator, typeが消える問題修正
		AFileAttribute	attr;
		GetFileAttribute(attr);
		::FSpCreateResFile(&fsspec,attr.creator,attr.type,0);
		
		//リソースアクセスロック
		//#92 複数スレッドからリソースファイルを開くとクラッシュするらしいので
		AStMutexLocker	mutexlocker(sResourceForkMutex);
		
		HFSUniStr255	forkName;
		::FSGetResourceForkName(&forkName);
		/*B0000 OSErr err =*/ ::FSOpenResourceFile(&fsref,forkName.length,forkName.unicode,fsWrPerm,&rRefNum);
		//short	rRefNum = ::FSOpenResFile(&fsref,fsRdPerm);←書き込み権限がなくてもかけてしまう
		if( rRefNum == -1 )   _AThrow("cannot open resource file",this);
		
		res = ::Get1Resource(inResType,inResNum);
		if( res != NULL )
		{
			::RemoveResource(res);
			::DisposeHandle(res);
		}
		{//#598 arrayptrの有効範囲を最小にする
			AStTextPtr	textptr(inText,0,inText.GetItemCount());
			res = ::NewHandle(textptr.GetByteCount());
			if( res == NULL )   _ACThrow("cannot create newhandle",this);
			::HLock(res);
			AMemoryWrapper::Memmove(*res,textptr.GetPtr(),textptr.GetByteCount());
			::HUnlock(res);
		}
		::AddResource(res,inResType,inResNum,"\p");
		::WriteResource(res);
		::ReleaseResource(res);
		::CloseResFile(rRefNum);
	}
	catch(...)
	{
		if( res != NULL )   ::DisposeHandle(res);
		if( rRefNum != -1 )   ::CloseResFile(rRefNum);
		result = false;
	}
	return result;
}
#endif

//MacOSX固有の処理
ABool	AFileAcc::GetFSRef( FSRef& outFSRef ) const
{
	/* if( mFSRefSpecified == true )
	{
		outFSRef = mFSRef;
		return true;
	}*/
	AText	path;
	GetPath(path);
	path.ConvertFromUTF8ToUTF16();//★速度対策検討
	ATextEncodingWrapper::ConvertUTF16ToHFSPlusDecomp(path);
	path.ConvertToUTF8FromUTF16();
	AStCreateCstringFromAText	cstr(path);
	OSStatus err = ::FSPathMakeRef((UInt8*)(cstr.GetPtr()),&outFSRef,NULL);
	if( err == noErr )
	{
		/* mFSRef = outFSRef;
		mFSRefSpecified = true;*/
		return true;
	}
	else
	{
		return false;
	}
}

ABool	AFileAcc::GetFSSpec( FSSpec& outFSSpec ) const
{
	FSRef	ref;
	if( GetFSRef(ref) == false )   return false;
	::FSGetCatalogInfo(&ref,kFSCatInfoNone,NULL,NULL,&outFSSpec,NULL);
	return true;
}

/*#1034
ABool	AFileAcc::WriteResourceFork( const AText& inText ) const
{
	bool	result = true;
	SInt16	forkref = 0;
	try
	{
		FSRef	ref;
		if( GetFSRef(ref) == false )   throw 0;
		
		//リソースアクセスロック
		//#92 複数スレッドからリソースファイルを開くとクラッシュするらしいので
		AStMutexLocker	mutexlocker(sResourceForkMutex);
		
		HFSUniStr255	forkName;
		::FSGetResourceForkName(&forkName);
		OSErr err = ::FSOpenFork(&ref,forkName.length,forkName.unicode,fsRdWrPerm,&forkref);
		if( err != noErr )   throw 0;
		ByteCount	actualCount = 0;
		{//#598 arrayptrの有効範囲を最小にする
			AStTextPtr	textptr(inText,0,inText.GetItemCount());
			err = ::FSWriteFork(forkref,fsFromStart,0,textptr.GetByteCount(),textptr.GetPtr(),&actualCount);
		}
		if( actualCount < inText.GetLength() )
		{
			_ACError("file write actual count is less than file size",this);
		}
		if( err != noErr )   throw 0;
		::FSSetForkSize(forkref,fsFromStart,actualCount);
		::FSCloseFork(forkref);
	}
	catch(...)
	{
		if( forkref != 0 )   ::FSCloseFork(forkref);
		result = false;
	}
	return result;
}
*/

/*#1425
void	AFileAcc::SetCreatorType( OSType inCreator, OSType inType )
{
	*#1034
	FSSpec	fsspec;
	GetFSSpec(fsspec);
	FInfo	fi;
	::FSpGetFInfo(&fsspec,&fi);
	fi.fdCreator = inCreator;
	fi.fdType = inType;
	::FSpSetFInfo(&fsspec,&fi);
	*
	FSRef	fsref;
	GetFSRef(fsref);
	FSCatalogInfo catInfo;
	::FSGetCatalogInfo(&fsref, kFSCatInfoFinderInfo, &catInfo, NULL, NULL, NULL);
	FileInfo *info = (FileInfo*)catInfo.finderInfo;
	info->fileCreator = inCreator;
	info->fileType = inType;
	::FSSetCatalogInfo(&fsref, kFSCatInfoFinderInfo, &catInfo);
}
*/

void	AFileAcc::GetCreatorType( OSType& outCreator, OSType& outType ) const
{
	//#1425
	AFileAttribute	attribute = {0};
	if( GetFileAttribute(attribute) == true )
	{
		outCreator = attribute.creator;
		outType = attribute.type;
	}
	/*#1425
	//クリエータ／タイプ読み込み
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	GetFSRef(fsref);
	::FSGetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo,NULL,NULL,NULL);
	outCreator = ((FileInfo*)(&(catinfo.finderInfo[0])))->fileCreator;
	outType = ((FileInfo*)(&(catinfo.finderInfo[0])))->fileType;
	*/
}

void	AFileAcc::SpecifyByFSRef( const FSRef& inFSRef )
{
	char	buf[4096];
	buf[0] = 0;
	::FSRefMakePath(&inFSRef,(UInt8*)buf,4096);
	AText	text;
	text.AddCstring(buf);
	Specify(text);
	/* mFSRef = inFSRef;
	mFSRefSpecified = true;*/
}

#ifndef __LP64__
void	AFileAcc::SpecifyByFSSpec( const FSSpec& inFSSpec )
{
	FSRef	ref;
	::FSpMakeFSRef(&inFSSpec,&ref);
	SpecifyByFSRef(ref);
}
#endif

//OSから受信したUTF16(HFSPlusDecomp variant)をUTF8(CanonicalComp)に変換してATextに格納する
//HFSPlusDecompでは濁点等が分解されている
void	AFileAcc::SetTextByHFSUniStr255( const HFSUniStr255 inHFSUniString, AText& outText ) const
{
	outText.DeleteAll();
	outText.InsertFromTextPtr(0,reinterpret_cast<AConstUCharPtr>(inHFSUniString.unicode),inHFSUniString.length*sizeof(UniChar));
	/*#1040
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(outText);
	outText.ConvertToUTF8FromUTF16();
	*/
	outText.ConvertToUTF8FromUTF16();
	outText.ConvertToCanonicalComp();
}

//B0000
ABool	AFileAcc::IsAppleScriptFile()
{
	AText	filename;
	GetFilename(filename);
	AText	suffix;
	filename.GetSuffix(suffix);
	if( suffix.Compare(".scpt") == true )   return true;
	if( suffix.Compare(".scptd") == true )   return true;//#206 スクリプトバンドルを実行できない問題を修正
	AFileAttribute	attr;
	GetFileAttribute(attr);
	if( attr.type == 'osas' )   return true;
	//R0137
	if( GetBundleFolderAttribute(attr) == true )
	{
		if( attr.type == 'osas' )   return true;
	}
	return false;
}

//R0230
//※今はこの関数は使用していない。今後、使用する場合は、GetTextEncodingIANAName()等を考慮すること。
ABool	AFileAcc::GetTextEncodingByXattr( AText& outTextEncodingName ) const
{
	if( AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_4 )   return false;
#if !BUILD_FOR_MACOSX_10_3
	AText	path;
	GetPath(path);
	AStCreateCstringFromAText	pathstr(path);
	ssize_t	len = ::getxattr(pathstr.GetPtr(),"com.apple.TextEncoding",NULL,0,0,0);
	if( len > 0 )
	{
		AText	value;
		value.Reserve(0,len);
		{//#598
			AStTextPtr	textptr(value,0,value.GetItemCount());
			::getxattr(pathstr.GetPtr(),"com.apple.TextEncoding",textptr.GetPtr(),textptr.GetByteCount(),0,0);
		}
		ATextArray	v;
		value.Explode(';',v);
		if( v.GetItemCount() == 2 )
		{
			/*
			http://developer.apple.com/releasenotes/Cocoa/Foundation.html
			フォーマット例：
			MACINTOSH;0
			UTF-8;134217984
			UTF-8;
			;3071
			*/
			if( v.GetTextConst(0).GetItemCount() > 0 )
			{
				if( ATextEncodingWrapper::CheckTextEncodingAvailability(v.GetTextConst(0)) == true )
				{
					outTextEncodingName.SetText(v.GetTextConst(0));
					return true;
				}
			}
			if( v.GetTextConst(1).GetItemCount() > 0 )
			{
				ATextEncoding	tecnum = v.GetTextConst(1).GetNumber();
				return ATextEncodingWrapper::GetTextEncodingName(tecnum,outTextEncodingName);
			}
		}
		return false;
	}
#endif
	return false;
}

//R0230
void	AFileAcc::SetTextEncodingByXattr( const AText& inTextEncodingName )
{
	if( AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_4 )   return;
#if !BUILD_FOR_MACOSX_10_3
	AText	path;
	GetPath(path);
	AStCreateCstringFromAText	pathstr(path);
	if( inTextEncodingName.GetItemCount() > 0 )
	{
		AText	value;
		value.SetText(inTextEncodingName);
		value.Add(';');
		ATextEncoding	tec;
		if( ATextEncodingWrapper::GetTextEncodingFromName(inTextEncodingName,tec) == true )
		{
			value.AddNumber(tec);
		}
		{//#598
			AStTextPtr	textptr(value,0,value.GetItemCount());
			::setxattr(pathstr.GetPtr(),"com.apple.TextEncoding",textptr.GetPtr(),textptr.GetByteCount(),0,0);
		}
	}
	else
	{
		::removexattr(pathstr.GetPtr(),"com.apple.TextEncoding",0);
	}
#endif
}

//R0137
ABool	AFileAcc::GetBundleFolderAttribute( AFileAttribute& outFileAttribute ) const
{
	outFileAttribute.creator = 0;
	outFileAttribute.type = 0;
	
	if( IsBundleFolder() == false )   return false;
	
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	
	CFURLRef	urlref = ::CFURLCreateFromFSRef(kCFAllocatorDefault,&fsref);
	if( urlref == NULL )   return false;
	CFBundleRef	bundleref = ::CFBundleCreate(kCFAllocatorDefault,urlref);
	::CFRelease(urlref);
	if( bundleref == NULL )   return false;
	::CFBundleGetPackageInfo(bundleref,&outFileAttribute.type,&outFileAttribute.creator);
	::CFRelease(bundleref);
	return true;
}

#endif

#pragma mark ===========================

#pragma mark --- path delimiter

#if IMPLEMENTATION_FOR_MACOSX
//AFileAcc内部で使用する（＝mPathへ格納するときの）パス区切り文字を返す
AUChar	AFileAcc::GetPathDelimiter( const AFilePathType inFilePathType )
{
	switch(inFilePathType)
	{
		//UNIXパス形式
	  case kFilePathType_Default:
		{
			return kUChar_Slash;
		}
		//  /volumename/folder1/folder2/test.txtの形式(kFilePathType_1)
	  case kFilePathType_1:
		{
			return kUChar_Slash;
		}
		//  :volumename:folder1:folder2:test.txtの形式(kFilePathType_2)
	  case kFilePathType_2:
		{
			return kUChar_Colon;
		}
	  default:
		{
			_ACError("",NULL);
			return kUChar_Slash;
		}
	}
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
AUChar	AFileAcc::GetPathDelimiter( const AFilePathType inFilePathType )
{
	return kUChar_Backslash;
}
#endif



