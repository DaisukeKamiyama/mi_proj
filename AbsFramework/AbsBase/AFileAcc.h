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

*/

#pragma once
#include "AObjectArrayItem.h"
#include "../AbsBase/ABase.h"

enum AFilePathType
{
	kFilePathType_Default = 0,
	kFilePathType_1,
	kFilePathType_2
};

#pragma mark ===========================
#pragma mark [クラス]AFileAcc
/**
ファイルアクセサ
ファイル実体ではなく、ファイルへのポインタ（ハンドラ）としてのクラス。
ファイルへのパスを保持するのみで、内容は保持しない。
*/
class AFileAcc : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	AFileAcc( AObjectArrayItem* inParent = NULL );
	AFileAcc( const AText& inPath );
	AFileAcc( const AFileAcc& inFile );
	AFileAcc&	operator = ( const AFileAcc& inFile );
	void	CopyFrom( const AFileAcc& inSrcFile );
	
	//ファイル位置指定
  public:
	void	Specify( const AText& inPath, const AFilePathType inFilePathType = kFilePathType_Default );
	void	SpecifyWithAnyFilePathType( const AText& inPath );//B0389
	ABool	SpecifyParent( const AFileAcc& inBaseFile );//#427
	void	SpecifySister( const AFileAcc& inBaseFile, AConstCharPtr inPath, const AFilePathType inFilePathType = kFilePathType_Default );
	void	SpecifySister( const AFileAcc& inBaseFile, const AText& inPath, const AFilePathType inFilePathType = kFilePathType_Default );
	void	SpecifyChild( const AFileAcc& inBaseFile, AConstCharPtr inPath, const AFilePathType inFilePathType = kFilePathType_Default );
	void	SpecifyChild( const AFileAcc& inBaseFile, const AText& inPath, const AFilePathType inFilePathType = kFilePathType_Default );
	void	SpecifyChildFile( const AFileAcc& inBaseFile, const AText& inFilename );
	void	SpecifyUniqChildFile( const AFileAcc& inFolder, AConstCharPtr inFilename );
	void	SpecifyUniqChildFile( const AFileAcc& inFolder, const AText& inFilename );
	void	SpecifyChangeSuffix( const AFileAcc& inBaseFile, const AText& inSuffix );
	void	Unspecify();
	ABool	IsSpecified() const { return mSpecified; }
  private:
	ABool	mSpecified;
	AText	mPath;
	
	
	//パス取得
  public:
	void	GetPath( AText& outPath, const AFilePathType inFilePathType = kFilePathType_Default ) const;
	void	GetNormalizedPath( AText& outPath ) const;//B0000
	void	GetPartialPath( const AFileAcc& inBaseFile, AText& outPath, const AFilePathType inFilePathType = kFilePathType_Default ) const;
	ABool	GetURL( AText& outURL ) const;//#539
	static void	GetNormalizedPathString( AText& ioPath );//#1087
	
	//ファイル名取得
  public:
	void	GetFilename( AText& outName, const ABool inConvertForDisplay = true ) const;
	void	GetRawFilename( AText& outName ) const;//R0006
	
	//比較
  public:
	ABool	Compare( const AFileAcc& inFileAcc ) const;
	ABool	CompareFilename( const AText& inFilename ) const;
	ABool	CompareSuffix( const AText& inSuffix ) const;
	
	//ファイル情報取得
  public:
	ABool	Exist() const;
	ABool	IsFolder() const;
	ABool	IsReadOnly() const;
	ABool	IsRoot() const;
	ABool	IsInvisible() const;
	ABool	IsExecutable() const;//R0000
	ABool	GetFileAttribute( AFileAttribute& outFileAttribute ) const;
	ABool	SetFileAttribute( const AFileAttribute& inFileAttribute ) const;//win
	AItemCount	GetFileSize() const;
	ABool	GetLastModifiedDataTime( ADateTime& outDateTime ) const;
	ABool	IsBundleFolder() const;//R0137
	ABool	IsChildOfFolder( const AFileAcc& inFolder ) const;
	
	//フォルダ内ファイル取得
  public:
	void	GetChildren( AObjectArray<AFileAcc>& outChilds ) const;
	
	//ファイル読み書き
  public:
	ABool	ReadTo( AText &outText/*#1034, ABool inResourceFork = false*/ ) const;
	ABool	WriteFile( const AText& inText, AFileError& outError ) const;
	ABool	WriteFile( const AText& inText ) const { AFileError err; return WriteFile(inText,err); }
	
	//ファイル操作
  public:
	ABool	CreateFile() const;
	ABool	CreateFolder();
	ABool	CreateFolderRecursive();//#427
	ABool	Rename( const AText& inNewName );
	void	DeleteFile();
	void	DeleteFolder();//#427
	void	DeleteFileOrFolder();//#1404
	void	DeleteFileOrFolderRecursive();//#427
	ABool	CopyFileTo( AFileAcc& inDst, const ABool inInhibitOverwrite ) const;//#427
	void	CopyFolderTo( AFileAcc& inDst, const ABool inInhibitOverwrite, const ABool inOnlyWhenNoDstFolder ) const;//#427
	void	CopyFileOrFolderTo( AFileAcc& inDst, const ABool inInhibitOverwrite, const ABool inOnlyWhenNoDstFolder );//#427
	
	//リンク／エイリアス
  public:
	void	ResolveAnyLink( const AFileAcc& inFileMayLink );
	ABool	ResolveAlias();//#1425
	ABool	IsLink();//#0
#if IMPLEMENTATION_FOR_WINDOWS
	ABool	CreateShellLink( const AFileAcc& inLinkFile ) const;
#endif
	
	//その他
  public:
	void	SpecifyAsUnitTestFolder();
	
	//MacOS専用
#if IMPLEMENTATION_FOR_MACOSX
  public:
	ABool	ExistResource( const ResType& inResType, const short& inResNum ) const;
	ABool	ReadResouceTo( AText &outText, ResType inResType, short inResNum ) const;
	//#1034 ABool	WriteResourceFork( const AText& inText ) const;
	//#1034 ABool	WriteResouceFile( const AText &inText, const ResType inResType, const short inResNum ) const;
	ABool	GetFSRef( FSRef& outFSRef ) const;
	ABool	GetFSSpec( FSSpec& outFSSpec ) const;
	//#1425 void	SetCreatorType( OSType inCreator, OSType inType );
	void	GetCreatorType( OSType& outCreator, OSType& outType ) const;
	void	SpecifyByFSRef( const FSRef& inFSRef );
#ifndef __LP64__
	void	SpecifyByFSSpec( const FSSpec& inFSSpec );
#endif
	void	SetTextByHFSUniStr255( const HFSUniStr255 inHFSUniString, AText& outText ) const;
	ABool	IsAppleScriptFile();
	ABool	GetTextEncodingByXattr( AText& outTextEncodingName ) const;
	void	SetTextEncodingByXattr( const AText& inTextEncodingName );
	ABool	GetBundleFolderAttribute( AFileAttribute& outFileAttribute ) const;//R0137
  private:
	static AThreadMutex					sResourceForkMutex;//#92
#endif
	
	//path delimiter
  public:
	static AUChar	GetPathDelimiter( const AFilePathType inFilePathType = kFilePathType_Default );//win 080703
	//テスト
  public:
	ABool	UnitTest();
	
	
  /* private:
	mutable ABool	mFSRefSpecified;
	mutable FSRef	mFSRef;*/
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AFileAcc"; }
	
};

