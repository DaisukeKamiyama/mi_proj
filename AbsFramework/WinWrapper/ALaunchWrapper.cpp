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
#pragma mark [�N���X]AAppAcc
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
#pragma mark [�N���X]ALaunchWrapper
#pragma mark ===========================
//
void	ALaunchWrapper::OpenURL( const AText& inText )
{
	AStCreateWcharStringFromAText	str(inText);
	::ShellExecuteW(NULL,L"open",str.GetPtr(),NULL,NULL,SW_SHOWNORMAL);
}

/**
�w��A�v����URL���J��
*/
void	ALaunchWrapper::OpenURLWithSpecifiedApp( const AAppAcc& inApp, const AText& inText )
{
	//���b��@�A�v���w��s��
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
�f�t�H���g�u���E�U�ŊJ��
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

//inFile��inApp���g�p���ĊJ��
ABool	ALaunchWrapper::Drop( const AAppAcc& inApp, const AFileAcc& inFile )
{
	//���b�� BROWSE���Ƃ肠�������삳���邽�߁B
	AText	path;
	inFile.GetPath(path);
	OpenURL(path);
	return true;
}

//B0326 inFile��OS�ɊJ������iinFile�̃_�u���N���b�N�ɑ����j
ABool	ALaunchWrapper::Open( const AFileAcc& inFile )
{
	AText	path;
	inFile.GetPath(path);
	AStCreateWcharStringFromAText	str(path);
	HINSTANCE	hi = ::ShellExecuteW(NULL,L"open",str.GetPtr(),NULL,NULL,SW_SHOWNORMAL);
	if( hi > (HINSTANCE)32 )//���������32���傫���l
	{
		return true;
	}
	else
	{
		return false;
	}
}

//�N���ς݂̃A�v���P�[�V�����ɑ΂���OpenDocument��AppleEvent�𑗐M����
ABool	ALaunchWrapper::SendOpenDocument( const AAppAcc& inAppAcc, const AFileAcc& inFile )
{
	return false;
}

/**
�v���Z�X��Kill�i�ُ�n�p������i�Ȃ̂Œʏ탋�[�g�ł͎g��Ȃ����Ɓj
*/
void	ALaunchWrapper::KillProcess( const APID inPID )
{
	HANDLE	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS,TRUE,inPID);
	::TerminateProcess(hProcess,0);
	::CloseHandle(hProcess);
}

//#436
/**
�R�}���h���s
*/
void	ALaunchWrapper::ExecCommand( const AText& inCommand )
{
	//���Ή��K�v
}

//#436
/**
�f�B���N�g��
*/
void	ALaunchWrapper::ChangeDirectory( const AText& inDir )
{
	//���Ή��K�v
}


