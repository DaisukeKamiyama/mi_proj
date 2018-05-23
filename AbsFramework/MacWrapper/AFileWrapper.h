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

AFileWrapper

*/

#pragma once
#include "../AbsBase/ABase.h"

#pragma mark ===========================
#pragma mark [クラス]AFileWrapper
/**
ファイル Wrapper
*/
class AFileWrapper
{
  public:
	//特殊フォルダ・ファイル取得
	static void	GetCurrentDirectory( AFileAcc& outFolder );//#263
	static void	GetDesktopFolder( AFileAcc& outFolder );
	static void	GetTrashFolder( AFileAcc& outFolder );
	static void	GetDocumentsFolder( AFileAcc& outFolder );
	static void	GetUserFolder( AFileAcc& outFolder );//#1231
	static void	GetAppPrefFolder( AFileAcc& outFileAcc );
	static const AFileAcc&	GetAppPrefFolder();
	//#844 static void	GetAppPrefFile( AFileAcc& outFileAcc );
	static void	GetTempFolder( AFileAcc& outTempFolder );//#693
	static void	GetAppFile( AFileAcc& outFileAcc );
	static void	GetPluginsFolder( AFileAcc& outFolder );//#361
	
	//情報取得
	static OSType	GetType( const AFileAcc& inFile );
	static ABool	GetLocalizedResourceFile( const AConstCharPtr inName, AFileAcc& outFile );//B0408
	static void	GetResourceFile( const AConstCharPtr inName, AFileAcc& outFile );//#166
	static void		GetHelpFolder( const AConstCharPtr inLang, AFileAcc& outHelpFolder );//#539
	static ABool	GetImageInfo( const AFileAcc& inFileAcc, ANumber& outWidth, ANumber& outHeight );
	
	//#1034 MoveToTrash(), NotifyFileChanged()はALaunchWrapperへ移動
	
	//win static void	SetFileAttribute( const AFileAcc& inFileAcc, const AFileAttribute& inFileAttribute );
	//win static void	GetDefaultFileAttribute( AFileAttribute& outFileAtribute );
	
	//ファイルサイズ
	static AItemCount	GetFileSize( const AText& inPath );
};

#pragma mark ===========================
#pragma mark [クラス]AFileWriteLocker
/**
ファイルロック用クラス
*/
class AFileWriteLocker
{
  public:
	AFileWriteLocker();
	~AFileWriteLocker();
	void	Specify( const AFileAcc& inFileAcc );
	ABool	IsSpecified() const { return mFileAcc.IsSpecified(); }
  private:
	AFileAcc	mFileAcc;
	//#527 SInt16		mFileRefNum;//B0388
	int			mFd;//#527
	
  public:
	ABool	Lock();
	void	Unlock();
	//#1243 ABool	IsLocked() const { return (mFd!=-1); }//#527 (mFileRefNum!=0); }
};



