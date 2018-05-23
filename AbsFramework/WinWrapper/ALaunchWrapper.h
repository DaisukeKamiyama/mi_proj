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

ALaunchWrapper

*/

#pragma once

#include "../AbsBase/ABase.h"

#pragma mark ===========================
#pragma mark [クラス]AAppAcc
//

enum AAppAccType
{
	kAppAccType_File
};

class AAppAcc
{
	//コンストラクタ／デストラクタ
  public:
	  AAppAcc();
	AAppAcc( const AFileAcc& inFile );
	
  public:
	AAppAccType		GetAppAccType() const { return mAppAccType; }
	const AFileAcc&	GetFile() const { return mFile; }
	
  public:
	ABool			SetFromToolCommandText( const AText& inText );
	
  private:
	AAppAccType		mAppAccType;
	AFileAcc		mFile;
};

#pragma mark ===========================
#pragma mark [クラス]ALaunchWrapper
//
class ALaunchWrapper
{
public:
	static void		OpenURL( const AText& inText );
	static void		OpenURL( AConstCharPtr inText );
	static void		OpenURL( const AFileAcc& inFile );//R0212
	static void		OpenURLWithSpecifiedApp( const AAppAcc& inApp, const AText& inText );//#214
	static void		OpenURLWithDefaultBrowser( const AText& inURL );//#539
	static void		Reveal( const AFileAcc& inFile );
	static ABool	Launch( const AAppAcc& inApp );
	static ABool	Drop( const AAppAcc& inApp, const AFileAcc& inFile );
	static ABool	Open( const AFileAcc& inFile );//B0326
	static ABool	SendOpenDocument( const AAppAcc& inAppAcc, const AFileAcc& inFile );
	static void		KillProcess( const APID inPID );//win
	static void		ExecCommand( const AText& inCommand );//#436
	static void		ChangeDirectory( const AText& inDir );//#436
  private:
};


