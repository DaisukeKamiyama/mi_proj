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
//
class AFileWrapper
{
  public:
	static void	GetDesktopFolder( AFileAcc& outFolder );
	static void	GetTrashFolder( AFileAcc& outFolder );
	static void	MoveToTrash( const AFileAcc& inFile );
	static void	GetDocumentsFolder( AFileAcc& outFolder );
	static void	GetAppPrefFolder( AFileAcc& outFileAcc );
	static void	GetTempFolder( AFileAcc& outTempFolder );//#693
	//#844 static void	GetAppPrefFile( AFileAcc& outFileAcc );
	static void	GetAppFile( AFileAcc& outFileAcc );
	static void	GetAppFolder( AFileAcc& outFileAcc );
	static void	NotifyFileChanged( const AFileAcc& inFileAcc );
	static ABool	GetImageInfo( const AFileAcc& inFileAcc, ANumber& outWidth, ANumber& outHeight );
	static void	GetLocalizedResourceFile( const ACharPtr inName, AFileAcc& outFile );
	static void	GetResourceFile( const ACharPtr inName, AFileAcc& outFile );//#166
	static void	GetHelpFolder( const ACharPtr inLang, AFileAcc& outHelpFolder );//#539
	static void	GetSendToFolder( AFileAcc& outFolder );
	static void	GetCurrentDirectory( AFileAcc& outFolder );//#263
};

#pragma mark ===========================
#pragma mark [クラス]AFileWriteLocker
//
class AFileWriteLocker
{
  public:
	AFileWriteLocker();
	~AFileWriteLocker();
	void	Specify( const AFileAcc& inFileAcc );
	ABool	IsSpecified() const { return mFileAcc.IsSpecified(); }
  private:
	AFileAcc	mFileAcc;
	int	mFH;
	
  public:
	ABool	Lock();
	void	Unlock();
	ABool	IsLocked() const {return (mFH!=-1);}
};



