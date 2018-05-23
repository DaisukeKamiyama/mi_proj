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

#include "AFileWrapper.h"
//#include <QuickTime/ImageCompression.h>
#include "ALaunchWrapper.h"
#include "../AbsBase/Cocoa.h"

#pragma mark ===========================

#pragma mark ---特殊フォルダ・ファイル取得

//#263
/**
Current Diractory取得
*/
void	AFileWrapper::GetCurrentDirectory( AFileAcc& outFolder )
{
	char	buf[4096];
	::getcwd(buf,4096);
	AText	path;
	path.SetCstring(buf);
	outFolder.Specify(path);
}

/**
デスクトップフォルダ取得
*/
void	AFileWrapper::GetDesktopFolder( AFileAcc& outFolder )
{
	OSStatus	err = noErr;
	
	FSRef	ref;
	err =::FSFindFolder(kUserDomain,kDesktopFolderType,true,&ref);
	if( err != noErr )   _ACErrorNum("FSFindFolder() returned error: ",err,NULL);
	outFolder.SpecifyByFSRef(ref);
}

/**
ゴミ箱フォルダ取得
*/
void	AFileWrapper::GetTrashFolder( AFileAcc& outFolder )
{
	OSStatus	err = noErr;
	
	FSRef	ref;
	err = ::FSFindFolder(kUserDomain,kTrashFolderType,true,&ref);
	if( err != noErr )   _ACErrorNum("FSFindFolder() returned error: ",err,NULL);
	outFolder.SpecifyByFSRef(ref);
}

/**
ドキュメントフォルダ取得
*/
void	AFileWrapper::GetDocumentsFolder( AFileAcc& outFolder )
{
	OSStatus	err = noErr;
	
	FSRef	ref;
	err = ::FSFindFolder(kUserDomain,kDocumentsFolderType,true,&ref);
	if( err != noErr )   _ACErrorNum("FSFindFolder() returned error: ",err,NULL);
	outFolder.SpecifyByFSRef(ref);
}

//#1231
/**
ユーザーフォルダ取得
*/
void	AFileWrapper::GetUserFolder( AFileAcc& outFolder )
{
	OSStatus	err = noErr;
	
	FSRef	ref;
	err = ::FSFindFolder(kUserDomain,kCurrentUserFolderType,true,&ref);
	if( err != noErr )   _ACErrorNum("FSFindFolder() returned error: ",err,NULL);
	outFolder.SpecifyByFSRef(ref);
}

//アプリケーション設定フォルダ
AFileAcc	gAppPrefFolder;

/**
Preferencesフォルダを取得
*/
const AFileAcc&	AFileWrapper::GetAppPrefFolder()
{
	//gAppPrefFolder未取得なら、取得する
	if( gAppPrefFolder.IsSpecified() == false )
	{
		OSStatus	err = noErr;
		
		FSRef	ref;
		//#460
		err = ::FSFindFolder(kUserDomain,kCurrentUserFolderType,true,&ref);
		if( err != noErr )   _ACErrorNum("FSFindFolder() returned error: ",err,NULL);
		AFileAcc	homeFolder;
		homeFolder.SpecifyByFSRef(ref);
		AFileAcc	prefroot;
		prefroot.SpecifyChild(homeFolder,"Library/Application Support");
		//旧バージョンMac OS Xでフォルダ存在しないかもしれないので念のためここで生成
		if( prefroot.Exist() == false )
		{
			prefroot.CreateFolder();
		}
		//
		AText	filename;
		filename.SetLocalizedText("AppPrefFolderName");
		gAppPrefFolder.SpecifyChild(prefroot,filename);
	}
	return gAppPrefFolder;
}
void	AFileWrapper::GetAppPrefFolder( AFileAcc& outFileAcc )
{
	outFileAcc = AFileWrapper::GetAppPrefFolder();
}

/*:
アプリケーションのPreferenceファイルを取得
*/
/*#844
void	AFileWrapper::GetAppPrefFile( AFileAcc& outFileAcc )
{
	AFileAcc	preffolder;
	GetAppPrefFolder(preffolder);
	AText	filename;
	filename.SetLocalizedText("AppPrefFileName");
	outFileAcc.SpecifyChild(preffolder,filename);
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

/*
アプリケーション自身のファイルを取得
*/
void	AFileWrapper::GetAppFile( AFileAcc& outFileAcc )
{
	FSRef	fsRef;
	ProcessSerialNumber psn = {0, kCurrentProcess};
	::GetProcessBundleLocation(&psn, &fsRef);
	outFileAcc.SpecifyByFSRef(fsRef);
}

//B0408
/**
Localizeされたファイルを取得
*/
ABool	AFileWrapper::GetLocalizedResourceFile( const AConstCharPtr inName, AFileAcc& outFile )
{
	OSStatus	err = noErr;
	
	AText	name(inName);
	AStCreateCFStringFromAText	str(name);
	CFBundleRef	appBundle = ::CFBundleGetMainBundle();
	CFURLRef	url = ::CFBundleCopyResourceURL(appBundle,str.GetRef(),NULL,NULL);
	if( url == NULL )   return false;
	FSRef	fsref;
	err = ::CFURLGetFSRef(url,&fsref);
	//if( err != noErr )   _ACErrorNum("CFURLGetFSRef() returned error: ",err,NULL);
	outFile.SpecifyByFSRef(fsref);
	::CFRelease(url);
	return true;
}

//#166
/**
リソースファイルを取得（ローカライズでないファイル）
*/
void	AFileWrapper::GetResourceFile( const AConstCharPtr inName, AFileAcc& outFile )
{
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	AFileAcc	folder;
	folder.SpecifyChild(appFile,"Contents/Resources");
	outFile.SpecifyChild(folder,inName);
}

//#539
/**
ヘルプフォルダ取得（Contents/Resources/help/）
*/
void	AFileWrapper::GetHelpFolder( const AConstCharPtr inLang, AFileAcc& outHelpFolder )
{
	AFileAcc	resourceFile;
	GetResourceFile("help",resourceFile);
	outHelpFolder.SpecifyChild(resourceFile,inLang);
}

//#361
/**
プラグインフォルダ取得(Contents/PlugIns/)
*/
void	AFileWrapper::GetPluginsFolder( AFileAcc& outFolder )
{
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	outFolder.SpecifyChild(appFile,"Contents/PlugIns");
}

#pragma mark ===========================

#pragma mark ---情報取得

/**
ファイルのTypeを取得
*/
OSType	AFileWrapper::GetType( const AFileAcc& inFile )
{
	OSStatus	err = noErr;
	
	FSRef	fsref;
	if( inFile.GetFSRef(fsref) == false )   return 0;
	FSCatalogInfo	info;
	err = ::FSGetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&info,NULL,NULL,NULL);
	if( err != noErr )   _ACErrorNum("FSGetCatalogInfo() returned error: ",err,NULL);
	FileInfo	fi = *(reinterpret_cast<FileInfo*>(info.finderInfo));
	return fi.fileType;
}

/**
イメージファイルの情報を取得
*/
ABool	AFileWrapper::GetImageInfo( const AFileAcc& inFileAcc, ANumber& outWidth, ANumber& outHeight )
{
	/*#1034
	FSSpec	fsspec;
	inFileAcc.GetFSSpec(fsspec);
	outWidth = outHeight = 0;
	PicHandle	picH = NULL;
	ComponentInstance	ci;
	//#1034未★
	if( ::GetGraphicsImporterForFile(&fsspec,&ci) != noErr )   return false;
	if( ::GraphicsImportGetAsPicture(ci,&picH) != noErr )   return false;
	::CloseComponent(ci);
	if( picH != NULL )
	{
		Rect	rect = (*picH)->picFrame;
		outWidth = rect.right-rect.left;
		outHeight = rect.bottom-rect.top;
		
		//B0229
		outWidth = ::CFSwapInt16BigToHost(outWidth);
		outHeight = ::CFSwapInt16BigToHost(outHeight);
		
		::DisposeHandle((Handle)picH);
		return true;
	}
	*/
	
	//パス取得
	AText	path;
	inFileAcc.GetNormalizedPath(path);
	//ファイルパスNSString取得
	AStCreateNSStringFromAText	pathstr(path);
	NSImage*	image = [[[NSImage alloc] initByReferencingFile:pathstr.GetNSString()] autorelease];
	NSSize	size = [image size];
	outWidth = size.width;
	outHeight = size.height;
	return true;
}
/*win
AFileAccへ移動
void	AFileWrapper::SetFileAttribute( const AFileAcc& inFileAcc, const AFileAttribute& inFileAttribute )
{
	//クリエータ／タイプ書き込み
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	inFileAcc.GetFSRef(fsref);
  ::FSGetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo,NULL,NULL,NULL);
	((FileInfo*)(&(catinfo.finderInfo[0])))->fileCreator = inFileAttribute.creator;
	((FileInfo*)(&(catinfo.finderInfo[0])))->fileType = inFileAttribute.type;
  ::FSSetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo);
	
}
AAppへ移動
void	AFileWrapper::GetDefaultFileAttribute( AFileAttribute& outFileAtribute )
{
	outFileAtribute.creator = NULL;
	outFileAtribute.type = NULL;
}
*/

#pragma mark ===========================

#pragma mark ---その他

//#1034 MoveToTrash(), NotifyFileChanged()はALaunchWrapperへ移動

/**
ファイルサイズ取得
*/
AItemCount	AFileWrapper::GetFileSize( const AText& inPath ) 
{
	AItemCount	result = 0;
	AStCreateNSStringFromAText	str(inPath);
	NSDictionary *attr = [[NSFileManager defaultManager] attributesOfItemAtPath:str.GetNSString() error:nil];
	if( attr )
	{
		NSNumber *theFileSize = [attr objectForKey:NSFileSize];
		result = [theFileSize longLongValue];
	}
	return result;
}

#pragma mark ===========================

#pragma mark ---ファイルロック

/**
ファイルロック　コンストラクタ
*/
AFileWriteLocker::AFileWriteLocker() : mFd(-1)//#527 mFileRefNum(0)
{
}

/**
ファイルロック　デストラクタ
*/
AFileWriteLocker::~AFileWriteLocker()
{
	/*#1243
	if( mFd != -1 )   ::close(mFd);
	*/
}

/**
ロックするファイルを設定
*/
void	AFileWriteLocker::Specify( const AFileAcc& inFileAcc )
{
	/*#1243
	mFileAcc.CopyFrom(inFileAcc);
	*/
}

/**
ロック
*/
ABool	AFileWriteLocker::Lock()
{
	/*#1243
	if( mFd != -1 )   return true;//already locked
	
	FSRef	ref;
	if( mFileAcc.GetFSRef(ref) == false )
	{
		_ACError("file not exist",NULL);
		return false;
	}
	UInt8	path[4096];
	path[0] = 0;
	::FSRefMakePath(&ref,path,4096);
	mFd = ::open((const char*)path,O_NONBLOCK|O_SHLOCK);//O_NONBLOCKをつけないと他アプリが書き込みオープンしているときにopenが返ってこない
	return (mFd>=0);
	*/
	return true;
}

/**
アンロック
*/
void	AFileWriteLocker::Unlock()
{
	/*#1243
	if( mFd != -1 )   ::close(mFd);
	mFd = -1;
	*/
}

