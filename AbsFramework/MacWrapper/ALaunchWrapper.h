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
kAppAccType_File = 0,
kAppAccType_Creator
};

/**
アプリケーションAccessor
*/
class AAppAcc
{
	//コンストラクタ／デストラクタ
  public:
	AAppAcc();//win
	AAppAcc( const AFileAcc& inFile );
	AAppAcc( OSType inType, OSType inCreator );
	
  public:
	AAppAccType		GetAppAccType() const { return mAppAccType; }
	const AFileAcc&	GetFile() const { return mFile; }
	OSType			GetProcessType() const { return mProcessType; }
	OSType			GetCreator() const { return mCreator; }
	
  public:
	ABool			SetFromToolCommandText( const AText& inText );
	
  private:
	AAppAccType		mAppAccType;
	AFileAcc		mFile;
	OSType			mProcessType;
	OSType			mCreator;
};

#pragma mark ===========================
#pragma mark [クラス]ALaunchWrapper
/**
アプリケーション起動 Wrapper
*/
class ALaunchWrapper
{
  public:
	static void		OpenURL( const AText& inText );
	static void		OpenURL( AConstCharPtr inText );
	static void		OpenURL( const AFileAcc& inFile );//R0212
	static void		OpenURLWithSpecifiedApp( const AAppAcc& inApp, const AText& inText );//#214
	static void		OpenURLWithDefaultBrowser( const AText& inURL );//#539
	static void		Reveal( const AFileAcc& inFile );
	static ABool	Launch( const AAppAcc& inApp, const ABool inSync = false );//#214 同期版フラグ追加
	static ABool	Drop( const AAppAcc& inApp, const AFileAcc& inFile );
	static ABool	Open( const AFileAcc& inFile );//B0326
	static ABool	SendAppleEvent_Text( const AAppAcc& inApp, const OSType inEventClass, const OSType inEventID, const AText& inText );
	static ABool	SendAppleEvent_Text( const AAppAcc& inApp, const OSType inEventClass, const OSType inEventID, const AText& inText, AText& outText );
	static ABool	SendAppleEvent_File( const AAppAcc& inApp, const OSType inEventClass, const OSType inEventID, const AFileAcc& inFile );
	static ABool	SendAppleEvent_File_WithBooleanValue( const AAppAcc& inApp, const OSType inEventClass, const OSType inEventID, 
					const OSType inBooleanValueClass, const ABool inBooleanValue, const AFileAcc& inFile );//#229
	static void		SendODBModified( const OSType inODBSender, const AText& inODBToken, const AFileAcc& inODBFile, 
					const ABool inFileLocationChanged, const AFileAcc& inNewFile );
	static void		SendODBClosed( const OSType inODBSender, const AText& inODBToken, const AFileAcc& inODBFile );
	static void		KillProcess( const APID inPID );//win
	static void		ExecCommand( const AText& inCommand );//#436
	static void		ChangeDirectory( const AText& inDir );//#436
//#263  private:
	static ABool	FindAProcess( const AAppAcc& inAppAcc, ProcessSerialNumber& outPSN );
	//B0326 static ABool	FindAppFromClassicDB( const OSType inAppCreator, FSSpec &outAppFile );
	static ABool	SendOpenDocument( const AAppAcc& inAppAcc, const AFileAcc& inFile );
	//B0326 static ABool	TellFinderToOpenUsing( const FSSpec &docSpec, FSSpec &appSpec );
	
	//その他 #1034 AFileWrapperから移動
	static void	MoveToTrash( const AFileAcc& inFile );
	static void	NotifyFileChanged( const AFileAcc& inFileAcc );
	
};


