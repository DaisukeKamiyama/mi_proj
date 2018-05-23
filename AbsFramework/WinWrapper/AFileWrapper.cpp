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

#include "stdafx.h"

#include <shlobj.h>
#include <share.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "AFileWrapper.h"
#include "../Imp.h"
#include <Shellapi.h>
#include "gdiplus.h"

void	AFileWrapper::GetDesktopFolder( AFileAcc& outFolder )
{
	wchar_t	buf[MAX_PATH];
	::SHGetFolderPathW(NULL,CSIDL_DESKTOPDIRECTORY,NULL,0,buf);
	AText	path;
	path.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	outFolder.Specify(path);
}

void	AFileWrapper::GetTrashFolder( AFileAcc& outFolder )
{
	wchar_t	buf[MAX_PATH];
	::SHGetFolderPathW(NULL,CSIDL_BITBUCKET,NULL,0,buf);
	AText	path;
	path.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	outFolder.Specify(path);
}

void	AFileWrapper::MoveToTrash( const AFileAcc& inFile )
{
	AText	srcpath;
	inFile.GetPath(srcpath);
	srcpath.Add(0);//double 0 terminate
	AStCreateWcharStringFromAText	srcFilePathStr(srcpath);
	//ゴミ箱に移動
	SHFILEOPSTRUCT	op = {0};
	op.pFrom = srcFilePathStr.GetPtr();
	op.hwnd = NULL;
	op.wFunc = FO_DELETE;
	op.pTo = NULL;
	op.fFlags = FOF_ALLOWUNDO|FOF_NOCONFIRMATION;//確認ダイアログ表示しない
	op.fAnyOperationsAborted = FALSE;
	op.hNameMappings = NULL;
	op.lpszProgressTitle = NULL;
	::SHFileOperation(&op);
}

//#263
void	AFileWrapper::GetCurrentDirectory( AFileAcc& outFolder )
{
	wchar_t	buf[4096];
	if( ::GetCurrentDirectory(4096,buf) > 0 )
	{
		AText	path;
		path.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
		outFolder.Specify(path);
	}
}

void	AFileWrapper::GetSendToFolder( AFileAcc& outFolder )
{
	wchar_t	buf[MAX_PATH];
	::SHGetFolderPathW(NULL,CSIDL_SENDTO,NULL,0,buf);
	AText	path;
	path.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	outFolder.Specify(path);
}

void	AFileWrapper::GetDocumentsFolder( AFileAcc& outFolder )
{
	wchar_t	buf[MAX_PATH];
	::SHGetFolderPathW(NULL,CSIDL_PERSONAL,NULL,0,buf);
	AText	path;
	path.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	outFolder.Specify(path);
}

void	AFileWrapper::GetAppPrefFolder( AFileAcc& outFileAcc )
{
	/* MyDocument\mi Preferencesから%appdata%へ移動(3.0.0a20)
	wchar_t	buf[MAX_PATH];
	::SHGetFolderPathW(NULL,CSIDL_PERSONAL,NULL,0,buf);
	AText	path;
	path.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	AFileAcc	mydocfolder;
	mydocfolder.Specify(path);
	outFileAcc.SpecifyChild(mydocfolder,"mi Preferences");
	*/
	wchar_t	buf[MAX_PATH];
	::SHGetFolderPathW(NULL,CSIDL_APPDATA,NULL,0,buf);
	AText	path;
	path.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	AFileAcc	appDataFolder;
	appDataFolder.Specify(path);
	AText	filename;
	filename.SetLocalizedText("AppPrefFolderName");
	outFileAcc.SpecifyChild(appDataFolder,filename);
}

/*#844
void	AFileWrapper::GetAppPrefFile( AFileAcc& outFileAcc )
{
	AFileAcc	apppreffolder;
	GetAppPrefFolder(apppreffolder);
	outFileAcc.SpecifyChild(apppreffolder,"Preferences");
}
*/

//#693
/**
テンポラリフォルダ取得（なければ生成）
*/
void	AFileWrapper::GetTempFolder( AFileAcc& outTempFolder )
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outTempFolder.SpecifyChild(appPrefFolder,"temp");
	outTempFolder.CreateFolder();
}

void	AFileWrapper::GetAppFile( AFileAcc& outFileAcc )
{
	wchar_t	buf[4096];
	::GetModuleFileNameW(CAppImp::GetHInstance(),buf,4096);
	AText	path;
	path.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	outFileAcc.Specify(path);
}

void	AFileWrapper::GetAppFolder( AFileAcc& outFileAcc )
{
	AFileAcc	appfile;
	GetAppFile(appfile);
	outFileAcc.SpecifyParent(appfile);
}

void	AFileWrapper::NotifyFileChanged( const AFileAcc& inFileAcc )//B0197
{
	//★暫定 未対応
}

ABool	AFileWrapper::GetImageInfo( const AFileAcc& inFileAcc, ANumber& outWidth, ANumber& outHeight )
{
	AText	path;
	inFileAcc.GetPath(path);
	AStCreateWcharStringFromAText	pathcstr(path);
	Gdiplus::Image	image(pathcstr.GetPtr());
	outWidth = image.GetWidth();
	outHeight = image.GetHeight();
	return false;
}

void	AFileWrapper::GetLocalizedResourceFile( const ACharPtr inName, AFileAcc& outFile )
{
	AFileAcc	appfolder;
	AFileWrapper::GetAppFolder(appfolder);
	AFileAcc	localizedFolder;
	localizedFolder.SpecifyChild(appfolder,"jp");
	outFile.SpecifyChild(localizedFolder,inName);
}

//#166
/**
リソースファイル取得
*/
void	AFileWrapper::GetResourceFile( const ACharPtr inName, AFileAcc& outFile )
{
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	outFile.SpecifySister(appFile,inName);
}

//#539
/**
ヘルプフォルダ取得（Contents/Resources/help/）
*/
void	AFileWrapper::GetHelpFolder( const ACharPtr inLang, AFileAcc& outHelpFolder )
{
	AFileAcc	resourceFile;
	GetResourceFile("help",resourceFile);
	outHelpFolder.SpecifyChild(resourceFile,inLang);
}

#pragma mark ===========================

#pragma mark ---ファイルロック

AFileWriteLocker::AFileWriteLocker() : mFH(-1)
{
}

AFileWriteLocker::~AFileWriteLocker()
{
	if( mFH != -1 )   _close(mFH);
}

void	AFileWriteLocker::Specify( const AFileAcc& inFileAcc )
{
	mFileAcc.CopyFrom(inFileAcc);
}

ABool	AFileWriteLocker::Lock()
{
	if( mFH != -1 )   return false;//already locked
	if( mFileAcc.IsSpecified() == false )   return false;
	
	AText	path;
	mFileAcc.GetPath(path);
	AStCreateWcharStringFromAText	pathstr(path);
	ABool	result = ( _wsopen_s(&mFH,pathstr.GetPtr(),_O_RDONLY,_SH_DENYWR,_S_IREAD) == 0 );
	return result;
}

void	AFileWriteLocker::Unlock()
{
	if( mFH != -1 )   _close(mFH);
	mFH = -1;
}

