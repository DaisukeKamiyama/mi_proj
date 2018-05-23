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

#include "stdafx.h"

#include "ALaunchWrapper.h"
#include "AFileWrapper.h"
#include "../Imp.h"
#include <shellapi.h>

#pragma mark ===========================
#pragma mark [クラス]AAppAcc
#pragma mark ===========================

//
AAppAcc::AAppAcc() : mAppAccType(kAppAccType_File)
{
}

//
AAppAcc::AAppAcc( const AFileAcc& inFile ) : mAppAccType(kAppAccType_File)
{
	mFile.CopyFrom(inFile);
}

//
ABool	AAppAcc::SetFromToolCommandText( const AText& inText )
{
	return false;
}

#pragma mark ===========================
#pragma mark [クラス]ALaunchWrapper
#pragma mark ===========================
//
void	ALaunchWrapper::OpenURL( const AText& inText )
{
	AStCreateWcharStringFromAText	str(inText);
	::ShellExecuteW(NULL,L"open",str.GetPtr(),NULL,NULL,SW_SHOWNORMAL);
}

/**
指定アプリでURLを開く
*/
void	ALaunchWrapper::OpenURLWithSpecifiedApp( const AAppAcc& inApp, const AText& inText )
{
	//★暫定　アプリ指定不可
	OpenURL(inText);
}

void	ALaunchWrapper::OpenURL( AConstCharPtr inText )
{
	AText	text(inText);
	OpenURL(text);
}

void	ALaunchWrapper::OpenURL( const AFileAcc& inFile )
{
	AText	path;
	inFile.GetPath(path);
	OpenURL(path);
}

//#539
/**
デフォルトブラウザで開く
*/
void	ALaunchWrapper::OpenURLWithDefaultBrowser( const AText& inURL )
{
	AStCreateWcharStringFromAText	str(inURL);
	::ShellExecuteW(NULL,L"open",str.GetPtr(),NULL,NULL,SW_SHOWNORMAL);
}

void	ALaunchWrapper::Reveal( const AFileAcc& inFile )
{
	AText	path;
	inFile.GetPath(path);
	AStCreateWcharStringFromAText	pathstr(path);
	::ShellExecuteW(NULL,L"explore",pathstr.GetPtr(),NULL,NULL,SW_SHOWNORMAL);
}

ABool	ALaunchWrapper::Launch( const AAppAcc& inApp )
{
	return false;
}

//inFileをinAppを使用して開く
ABool	ALaunchWrapper::Drop( const AAppAcc& inApp, const AFileAcc& inFile )
{
	//★暫定 BROWSEをとりあえず動作させるため。
	AText	path;
	inFile.GetPath(path);
	OpenURL(path);
	return true;
}

//B0326 inFileをOSに開かせる（inFileのダブルクリックに相当）
ABool	ALaunchWrapper::Open( const AFileAcc& inFile )
{
	AText	path;
	inFile.GetPath(path);
	AStCreateWcharStringFromAText	str(path);
	HINSTANCE	hi = ::ShellExecuteW(NULL,L"open",str.GetPtr(),NULL,NULL,SW_SHOWNORMAL);
	if( hi > (HINSTANCE)32 )//成功すると32より大きい値
	{
		return true;
	}
	else
	{
		return false;
	}
}

//起動済みのアプリケーションに対してOpenDocumentのAppleEventを送信する
ABool	ALaunchWrapper::SendOpenDocument( const AAppAcc& inAppAcc, const AFileAcc& inFile )
{
	return false;
}

/**
プロセスをKill（異常系用強制手段なので通常ルートでは使わないこと）
*/
void	ALaunchWrapper::KillProcess( const APID inPID )
{
	HANDLE	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS,TRUE,inPID);
	::TerminateProcess(hProcess,0);
	::CloseHandle(hProcess);
}

//#436
/**
コマンド実行
*/
void	ALaunchWrapper::ExecCommand( const AText& inCommand )
{
	//★対応必要
}

//#436
/**
ディレクトリ
*/
void	ALaunchWrapper::ChangeDirectory( const AText& inDir )
{
	//★対応必要
}


