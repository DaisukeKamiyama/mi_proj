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

#include "ALaunchWrapper.h"
#include "AFileWrapper.h"
//#include "../Abs/AUtil.h"
#include "ODBEditorSuite.h"
#include "../Imp.h"
#include "../AbsBase/Cocoa.h"

#pragma mark ===========================
#pragma mark [�N���X]AAppAcc
#pragma mark ===========================

//win
/**
�R���X�g���N�^
*/
AAppAcc::AAppAcc() : mProcessType(NULL), mCreator(NULL), mAppAccType(kAppAccType_Creator)
{
}

/**
�R���X�g���N�^
*/
AAppAcc::AAppAcc( const AFileAcc& inFile ) : mProcessType(NULL), mCreator(NULL), mAppAccType(kAppAccType_File)
{
	mFile.CopyFrom(inFile);
}

/**
�R���X�g���N�^
*/
AAppAcc::AAppAcc( OSType inType, OSType inCreator ) : mProcessType(inType), mCreator(inCreator), mAppAccType(kAppAccType_Creator)
{
}

//
/**
ToolCommand�t�H�[�}�b�g�̃e�L�X�g����A�v�����肷��
*/
ABool	AAppAcc::SetFromToolCommandText( const AText& inText )
{
	//B0387 LS���g�p���Č���
	//LS��D�悳����B����ŁA"appFile.IsFolder()==false"�̔���͍폜�B�A�v���P�[�V�����͏�Ƀt�H���_�Ȃ̂ŁA�Ԉ���Ă���B
	//����ŁA�ȉ��̏ꍇ�S�ĂɑΉ��ł���B
	//�ELAUNCH("�A�v���P�[�V������")��"�A�v���P�[�V������"��Applications���̃t�H���_���Ɠ����ꍇ
	//�ELAUNCH(/Adobe Reader 8/Adobe Reader.app)�i��appFile.IsFolder()==false�̔��肪�����Ă���ƃA�v���̓t�H���_�Ȃ̂�NG�ƂȂ��Ă����B�j
	//�ELAUNCH(/Adobe Reader 8/Adobe Reader)�i��appFile.IsFolder()==false�̔��肪�����Ă��Ă��A���̊֐��̍Ō�Őݒ肳���l�ŋN���ł��Ă����B�j
	//�ELAUNCH(Adobe Reader)
	{
		{
			FSRef	appfsref;
			AStCreateCFStringFromAText	str(inText);
			if( ::LSFindApplicationForInfo(kLSUnknownCreator,NULL,str.GetRef(),&appfsref,NULL) == noErr )
			{
				mFile.SpecifyByFSRef(appfsref);
				mAppAccType = kAppAccType_File;
				return true;
			}
		}
		AText	text;
		text.SetText(inText);
		text.AddCstring(".app");
		{
			FSRef	appfsref;
			AStCreateCFStringFromAText	str(text);
			if( ::LSFindApplicationForInfo(kLSUnknownCreator,NULL,str.GetRef(),&appfsref,NULL) == noErr )
			{
				mFile.SpecifyByFSRef(appfsref);
				mAppAccType = kAppAccType_File;
				return true;
			}
		}
	}
	
	AFileAcc	applicationsFolder;
	/*#1034
	short	vRefNum;
	long	dirID;
	if( ::FindFolder(kLocalDomain,kApplicationsFolderType,true,&vRefNum,&dirID) == noErr ) 
	{
		FSSpec	fsspec;
		::FSMakeFSSpec(vRefNum,dirID,"\p",&fsspec);
		applicationsFolder.SpecifyByFSSpec(fsspec);
	}
	else
	{
		AFileAcc	file;
		AFileWrapper::GetAppFile(file);
		applicationsFolder.SpecifyParent(file);
	}
	*/
	AText	appPath("/Applications");
	applicationsFolder.Specify(appPath);
	
	AFileAcc	appFile;
	appFile.SpecifyChild(applicationsFolder,inText);
	if( appFile.Exist() == true /*&& appFile.IsFolder()==false�A�v���P�[�V�����̓t�H���_�E�E�EB0387*//*B0326*/ )
	{
		mFile.CopyFrom(appFile);
		mAppAccType = kAppAccType_File;
		return true;
	}
	//B0387 �t���p�X�w��
	appFile.SpecifyWithAnyFilePathType(inText);
	if( appFile.Exist() == true )
	{
		mFile.CopyFrom(appFile);
		mAppAccType = kAppAccType_File;
		return true;
	}
	//B0326 LS���g�p���Č���
	/*{
		FSRef	appfsref;
		AStCreateCFStringFromAText	str(inText);
		if( ::LSFindApplicationForInfo(kLSUnknownCreator,NULL,str.GetRef(),&appfsref,NULL) == noErr )
		{
			mFile.SpecifyByFSRef(appfsref);
			mAppAccType = kAppAccType_File;
			return true;
		}
	}*/
	
	//.app�����Ă݂�
	AText	text;
	text.SetText(inText);
	text.AddCstring(".app");
	appFile.SpecifyChild(applicationsFolder,text);
	if( appFile.Exist() == true /*&& appFile.IsFolder()==false�A�v���P�[�V�����̓t�H���_�E�E�EB0387*//*B0326*/ )
	{
		mFile.CopyFrom(appFile);
		mAppAccType = kAppAccType_File;
		return true;
	}
	//B0387 �t���p�X�w��
	appFile.SpecifyWithAnyFilePathType(text);
	if( appFile.Exist() == true )
	{
		mFile.CopyFrom(appFile);
		mAppAccType = kAppAccType_File;
		return true;
	}
	//B0326 LS���g�p���Č���
	/*{
		FSRef	appfsref;
		AStCreateCFStringFromAText	str(text);
		if( ::LSFindApplicationForInfo(kLSUnknownCreator,NULL,str.GetRef(),&appfsref,NULL) == noErr )
		{
			mFile.SpecifyByFSRef(appfsref);
			mAppAccType = kAppAccType_File;
			return true;
		}
	}*/
	
	if( inText.GetItemCount() == 4 )
	{
		mProcessType = 'APPL';
		mCreator = inText.GetOSTypeFromText();//AUtil::GetOSTypeFromAText(inText);
		mAppAccType = kAppAccType_Creator;
		return true;
	}
	
	mFile.CopyFrom(appFile);
	mAppAccType = kAppAccType_File;
	return false;
}

#pragma mark ===========================
#pragma mark [�N���X]ALaunchWrapper
#pragma mark ===========================

/**
URL���J��
*/
void	ALaunchWrapper::OpenURL( const AText& inText )
{
	AStCreateCFStringFromAText	cfstr(inText);
	CFURLRef	urlref = ::CFURLCreateWithString(kCFAllocatorDefault,cfstr.GetRef(),NULL);
	if( urlref == NULL )   return;
	::LSOpenCFURLRef(urlref,NULL);
}

/**
URL���J��
*/
void	ALaunchWrapper::OpenURL( AConstCharPtr inText )
{
	AText	text(inText);
	OpenURL(text);
}

//R0212
/**
URL���J��
*/
void	ALaunchWrapper::OpenURL( const AFileAcc& inFile )
{
	FSRef	fsref;
	if( inFile.GetFSRef(fsref) == false )   return;
	CFURLRef	urlref = ::CFURLCreateFromFSRef(kCFAllocatorDefault,&fsref);
	if( urlref == NULL )   return;
	::LSOpenCFURLRef(urlref,NULL);
}

//#214
/**
�w��A�v����URL���J��
*/
void	ALaunchWrapper::OpenURLWithSpecifiedApp( const AAppAcc& inApp, const AText& inText )
{
	if( SendAppleEvent_Text(inApp,'GURL','GURL',inText) == false )
	{
		//AE���M�G���[�̎��́A�A�v���N�����Ă��瑗�M���Ă݂�
		Launch(inApp,true);//�����N��
		SendAppleEvent_Text(inApp,'GURL','GURL',inText);
	}
	//�A�v����O�ʂɂ���
	ProcessSerialNumber	psn;
	if( FindAProcess(inApp,psn) == true )
	{
		::SetFrontProcess(&psn);
	}
}

//#539
/**
�f�t�H���g�u���E�U�ŊJ��
*/
void	ALaunchWrapper::OpenURLWithDefaultBrowser( const AText& inURL )
{
	//Mac�ł̓f�t�H���g�u���E�U�̎w�肪�Ȃ������Ȃ̂ŁASafari���w�肷��
	AText	text;
	text.SetCstring("Safari");
	AAppAcc	appAcc;
	appAcc.SetFromToolCommandText(text);
	OpenURLWithSpecifiedApp(appAcc,inURL);
}

/**
Finder�ŊJ��
*/
void	ALaunchWrapper::Reveal( const AFileAcc& inFile )
{
	//#1425
	NSArray*	array = [NSArray arrayWithObjects:inFile.GetNSURL(), nil];
	[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:array];
	/*#1425
	ProcessSerialNumber	psn;
	AAppAcc	appAcc('FNDR','MACS');
	if( FindAProcess(appAcc,psn) == false ) 
	{
		return;
	}
	AEDesc	target, desc;
	AppleEvent	ae, reply;
	::AECreateDesc(typeNull,NULL,0,&target);
	::AECreateDesc(typeNull,NULL,0,&desc);
	::AECreateDesc(typeNull,NULL,0,&ae);
	::AECreateDesc(typeNull,NULL,0,&reply);
	try
			{
		OSErr	err = ::AECreateDesc(typeProcessSerialNumber,(Ptr)&psn,sizeof(ProcessSerialNumber),&target);
		if( err != noErr )   throw 0;
		err = ::AECreateAppleEvent('misc','mvis',&target,kAutoGenerateReturnID,kAnyTransactionID,&ae);
		if( err != noErr )   throw 0;
		FSRef	fsref;
		if( inFile.GetFSRef(fsref) == false )   throw 0;
		err = ::AECreateDesc(typeFSRef,&fsref,(Size)(sizeof(fsref)),&desc);
		if( err != noErr )   throw 0;
		err = ::AEPutParamDesc(&ae,keyDirectObject,&desc);
		if( err != noErr )   throw 0;
		err =:: AESend(&ae,&reply,kAENoReply,kAENormalPriority,0L,NULL,NULL);
	}
	catch(...)
	{
	}
	::AEDisposeDesc(&target);
	::AEDisposeDesc(&ae);
	::AEDisposeDesc(&reply);
	::AEDisposeDesc(&desc);
	//B0197
	::SetFrontProcess(&psn);
	*/
}

/**
�A�v���P�[�V�������N������
*/
ABool	ALaunchWrapper::Launch( const AAppAcc& inApp, const ABool inSync )//#241 �����Ńt���O�ǉ�
{
	FSRef	appfsref;
	switch(inApp.GetAppAccType())
	{
	  case kAppAccType_File:
		{
			if( inApp.GetFile().GetFSRef(appfsref) == false )   return false;
			break;
		}
	  case kAppAccType_Creator:
		{
			if( ::LSFindApplicationForInfo(inApp.GetCreator(),NULL,NULL,&appfsref,NULL) != noErr )   return false;
			break;
		}
	  default:
		{
			return false;
		}
	}
	if( inSync == false )
	{
		//�񓯊���
		AFileAcc	app;
		app.SpecifyByFSRef(appfsref);
		return Open(app);
	}
	else
	{
		//������ #214
#ifdef __LP64__
		LaunchParamBlockRec	lpbr;
		lpbr.launchBlockID = extendedBlock;
		lpbr.launchEPBLength = extendedBlockLen;
		lpbr.launchFileFlags = launchNoFileFlags;
		lpbr.launchControlFlags = launchContinue + launchNoFileFlags;
		lpbr.launchAppRef = &appfsref;
		lpbr.launchAppParameters = NULL;
#else
		FSSpec	fsspec;
		::FSGetCatalogInfo(&appfsref,kFSCatInfoNone,NULL,NULL,&fsspec,NULL);
		LaunchParamBlockRec	lpbr;
		lpbr.launchBlockID = extendedBlock;
		lpbr.launchEPBLength = extendedBlockLen;
		lpbr.launchFileFlags = launchNoFileFlags;
		lpbr.launchControlFlags = launchContinue + launchNoFileFlags;
		lpbr.launchAppSpec = &fsspec;
		lpbr.launchAppParameters = NULL;
#endif
		if( ::LaunchApplication(&lpbr) != noErr )   return false;
		return true;
	}
	/*B0326 LS�g�p�ɕύX
	ProcessSerialNumber	psn;
	if( FindAProcess(inApp,psn) == true )
	{
	  ::SetFrontProcess(&psn);
	}
	else
	{
		FSSpec	fsspec;
		switch(inApp.GetAppAccType())
		{
		  case kAppAccType_File:
			{
				inApp.GetFile().GetFSSpec(fsspec);
				break;
			}
		  case kAppAccType_Creator:
			{
				if( FindAppFromClassicDB(inApp.GetCreator(),fsspec) == false )
				{
					return false;
				}
				break;
			}
		}
		LaunchParamBlockRec	lpbr;
		lpbr.launchBlockID = extendedBlock;
		lpbr.launchEPBLength = extendedBlockLen;
		lpbr.launchFileFlags = launchNoFileFlags;
		lpbr.launchControlFlags = launchContinue + launchNoFileFlags;
		lpbr.launchAppSpec = &fsspec;
		lpbr.launchAppParameters = NULL;
		if( ::LaunchApplication(&lpbr) != noErr )   return false;
	}
	return true;
	*/
}

/**
inFile��inApp���g�p���ĊJ��
*/
ABool	ALaunchWrapper::Drop( const AAppAcc& inApp, const AFileAcc& inFile )
{
	FSRef	appfsref;
	switch(inApp.GetAppAccType())
	{
	  case kAppAccType_File:
		{
			if( inApp.GetFile().GetFSRef(appfsref) == false )   return false;
			break;
		}
	  case kAppAccType_Creator:
		{
			if( ::LSFindApplicationForInfo(inApp.GetCreator(),NULL,NULL,&appfsref,NULL) != noErr )   return false;
			break;
		}
	  default:
		{
			return false;
		}
	}
	
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_12 )
	{
		//macOS 10.12�ȍ~�̏ꍇ
		
		//macOS 10.12�ȍ~�ł�LSOpenFromRefSpec()�����퓮�삵�Ȃ��̂ŁACocoa API���g���悤�ɂ���
		AText	docPath;
		inFile.GetNormalizedPath(docPath);
		AText	appPath;
		AFileAcc	appFile;
		appFile.SpecifyByFSRef(appfsref);
		appFile.GetNormalizedPath(appPath);
		AStCreateNSStringFromAText	appstr(appPath);
		AStCreateNSStringFromAText	docstr(docPath);
		return ([[NSWorkspace sharedWorkspace] openFile:docstr.GetNSString() withApplication:appstr.GetNSString()] == YES);
	}
	else
	{
		//Mac OS X 10.11�ȑO�̏ꍇ
		
		FSRef	docfsref;
		if( inFile.GetFSRef(docfsref) == false )   return false;
		LSLaunchFSRefSpec	launchspec;
		launchspec.appRef = &appfsref;
		launchspec.numDocs = 1;
		launchspec.itemRefs = &docfsref;
		launchspec.passThruParams = NULL;
		launchspec.launchFlags = 0;
		launchspec.asyncRefCon = NULL;
		return (::LSOpenFromRefSpec(&launchspec,NULL)==noErr);
	}
	
	/*B0326 LS�g�p�ɕύX
	ProcessSerialNumber	psn;
	if( FindAProcess(inApp,psn) == true )
	{
		if( SendOpenDocument(inApp,inFile) == false )   return false;
	  ::SetFrontProcess(&psn);
	}
	else
	{
		FSSpec	fsspec;
		switch(inApp.GetAppAccType())
		{
		  case kAppAccType_File:
			{
				inApp.GetFile().GetFSSpec(fsspec);
				break;
			}
		  case kAppAccType_Creator:
			{
				if( FindAppFromClassicDB(inApp.GetCreator(),fsspec) == false )
				{
					return false;
				}
				break;
			}
		}
		FSSpec	docSpec;
		if( inFile.GetFSSpec(docSpec) == false )   return false;
		if( TellFinderToOpenUsing(docSpec,fsspec) == false )   return false;
	}
	return true;*/
}

//B0326
/**
inFile��OS�ɊJ������iinFile�̃_�u���N���b�N�ɑ����j
*/
ABool	ALaunchWrapper::Open( const AFileAcc& inFile )
{
	FSRef	fsref;
	if( inFile.GetFSRef(fsref) == false )   return false;
	return (::LSOpenFSRef(&fsref,NULL)==noErr);
}

//
/**
�w��A�v���P�[�V�����̃v���Z�X�ԍ����擾����
*/
ABool	ALaunchWrapper::FindAProcess( const AAppAcc& inAppAcc, ProcessSerialNumber& outPSN )
{
	outPSN.lowLongOfPSN = kNoProcess;
	outPSN.highLongOfPSN = kNoProcess;
	
	ABool	found = false;
	while( found == false )
	{
		OSErr	status = ::GetNextProcess(&outPSN);
		ProcessInfoRec	info;
		info.processInfoLength = sizeof(ProcessInfoRec);
		info.processName = NULL;
#ifdef __LP64__
		FSRef	fsref = {0};
		info.processAppRef = &fsref;
#else
		FSSpec	spec;
		info.processAppSpec = &spec;
#endif
		if( status == noErr ) 
		{
			status = ::GetProcessInformation(&outPSN,&info);
		}
		if( status == noErr ) 
		{
			switch(inAppAcc.GetAppAccType())
			{
			  case kAppAccType_File:
				{
					AFileAcc	file;
#ifdef __LP64__
					file.SpecifyByFSRef(fsref);
#else
					file.SpecifyByFSSpec(spec);
#endif
					AText	path1, path2;
					file.GetPath(path1);
					inAppAcc.GetFile().GetPath(path2);
					if( path1.CompareMin(path2) == true )
					{
						found = true;
					}
					break;
				}
			  case kAppAccType_Creator:
				{
					if( info.processSignature == inAppAcc.GetCreator() && info.processType == inAppAcc.GetProcessType() )
					{
						found = true;
					}
					break;
				}
			}
		}
		else break;
	}
	return found;
}

/**
�N���ς݂̃A�v���P�[�V�����ɑ΂���OpenDocument��AppleEvent�𑗐M����
*/
ABool	ALaunchWrapper::SendOpenDocument( const AAppAcc& inAppAcc, const AFileAcc& inFile )
{
	return SendAppleEvent_File(inAppAcc,kCoreEventClass,kAEOpenDocuments,inFile);
}

/*B0326
ABool	ALaunchWrapper::FindAppFromClassicDB( const OSType inAppCreator, FSSpec &outAppFile )
{
	DTPBRec		dtpb;
	HVolumeParam	vlpb;
	short		idx;
	OSErr		result;
	
	Str255	name;
	idx = 1;
	do 
	{
		vlpb.ioVolIndex = idx;
		name[0] = 0;
		vlpb.ioNamePtr = name;
		result = PBHGetVInfoSync((HParmBlkPtr)&vlpb);
		if( result == noErr ) 
		{
			name[0] = 0;
			dtpb.ioNamePtr = name;
			dtpb.ioVRefNum = vlpb.ioVRefNum;
			result = PBDTGetPath(&dtpb);
		}
		if( result == noErr ) 
		{
			name[0] = 0;
			dtpb.ioNamePtr = name;
			dtpb.ioIndex = 0;
			dtpb.ioFileCreator = inAppCreator;
			result = PBDTGetAPPLSync(&dtpb);
		}
		if( result == noErr ) 
		{
			::FSMakeFSSpec(dtpb.ioVRefNum,dtpb.ioAPPLParID,name,&outAppFile);
			return true;
		}
		idx++;
	} while (result != nsvErr);
	return false;
}

ABool	ALaunchWrapper::TellFinderToOpenUsing( const FSSpec &docSpec, FSSpec &appSpec )
{
	ProcessSerialNumber	psn;
	AAppAcc	appAcc('FNDR','MACS');
	if( FindAProcess(appAcc,psn) == false ) 
	{
		return false;
	}
	
	Boolean	result = false;
	OSErr	err;
	AliasHandle	alias = NULL;
	AEDescList	docList;
	AEDesc	docDesc, appDesc, targetDesc, reply;
	AppleEvent	ae;
	AECreateDesc(typeNull,NULL,0,&docDesc);
	AECreateDesc(typeNull,NULL,0,&targetDesc);
	AECreateDesc(typeNull,NULL,0,&reply);
	AECreateDesc(typeNull,NULL,0,&ae);
	AECreateDesc(typeNull,NULL,0,&appDesc);
	try
	{
		err = AECreateList(NULL,0,false,&docList);
		if( err != noErr )   throw 0;
		err = NewAliasMinimal(&docSpec,&alias);
		if( err != noErr )   throw 0;
		HLock((Handle)alias);
		err = AECreateDesc(typeAlias,(Ptr)*alias,GetHandleSize((Handle)alias),&docDesc);
		HUnlock((Handle)alias);
		if( err != noErr )   throw 0;
		err = AECoerceDesc(&docDesc,typeAEList,&docList);
		if( err != noErr )   throw 0;
		AEDisposeDesc(&docDesc);
		AECreateDesc(typeNull,NULL,0,&docDesc);
		DisposeHandle((Handle)alias);
		alias = NULL;
		
		err = NewAliasMinimal(&appSpec,&alias);
		if( err != noErr )   throw 0;
		HLock((Handle)alias);
		err = AECreateDesc(typeAlias,(Ptr)*alias,GetHandleSize((Handle)alias),&appDesc);
		HUnlock((Handle)alias);
		if( err != noErr )   throw 0;
		DisposeHandle((Handle)alias);
		alias = NULL;
		
		err = AECreateDesc(typeProcessSerialNumber,(Ptr)&psn,sizeof(ProcessSerialNumber),&targetDesc);
		if( err != noErr )   throw 0;
		err = AECreateAppleEvent(kCoreEventClass,kAEOpenDocuments,&targetDesc,kAutoGenerateReturnID,
				kAnyTransactionID,&ae);
		if( err != noErr )   throw 0;
		err = AEPutParamDesc(&ae,keyDirectObject,&docList);
		if( err != noErr )   throw 0;
		err = AEPutParamDesc(&ae,'usin',&appDesc);
		if( err != noErr )   throw 0;
		err = AESend(&ae,&reply,kAENoReply,kAENormalPriority,kAEDefaultTimeout,NULL,NULL);
		if( err != noErr )  throw 0;
		result = true;
	}
	catch(...)
	{
	}
	if( alias != NULL )   DisposeHandle((Handle)alias);
	AEDisposeDesc(&docDesc);
	AEDisposeDesc(&reply);
	AEDisposeDesc(&ae);
	AEDisposeDesc(&targetDesc);
	AEDisposeDesc(&appDesc);
	return result;
}
*/

/**
Text�̃p�����[�^�݂̂�����AppleEvent�𑗐M����
*/
ABool	ALaunchWrapper::SendAppleEvent_Text( const AAppAcc& inApp, const OSType inEventClass, const OSType inEventID, const AText& inText )
{
	ProcessSerialNumber	psn;
	if( FindAProcess(inApp,psn) == false )   return false;
	
	Boolean	result = false;
	OSErr	err;
	AEDesc	target, desc;
	AppleEvent	ae, reply;
	AECreateDesc(typeNull,NULL,0,&desc);
	AECreateDesc(typeNull,NULL,0,&ae);
	AECreateDesc(typeNull,NULL,0,&reply);
	try
	{
		err = AECreateDesc(typeProcessSerialNumber,(Ptr)&psn,sizeof(ProcessSerialNumber),&target);
		if( err != noErr )   throw 0;
		err = AECreateAppleEvent(inEventClass,inEventID,&target,kAutoGenerateReturnID,kAnyTransactionID,&ae);
		if( err != noErr )   throw 0;
		{
			AStTextPtr	textptr(inText,0,inText.GetItemCount());
			err = AECreateDesc(typeChar,textptr.GetPtr(),(Size)(textptr.GetByteCount()),&desc);
		}
		if( err != noErr )   throw 0;
		err = AEPutParamDesc(&ae,keyDirectObject,&desc);
		if( err != noErr )   throw 0;
		err = AESend(&ae,&reply,kAENoReply,kAENormalPriority,0L,NULL,NULL);
		result = true;
	}
	catch(...)
	{
	}
	AEDisposeDesc(&target);
	AEDisposeDesc(&ae);
	AEDisposeDesc(&reply);
	AEDisposeDesc(&desc);
	return result;
}

/**
Text�̃p�����[�^�݂̂�����AppleEvent�𑗐M���A������Text�Ŏ�M����
*/
ABool	ALaunchWrapper::SendAppleEvent_Text( const AAppAcc& inApp, const OSType inEventClass, const OSType inEventID, const AText& inText, AText& outText )
{
	ProcessSerialNumber	psn;
	if( FindAProcess(inApp,psn) == false )   return false;
	
	Boolean	result = false;
	OSErr	err;
	AEDesc	target, desc;
	AppleEvent	ae, reply;
	::AECreateDesc(typeNull,NULL,0,&desc);
	::AECreateDesc(typeNull,NULL,0,&ae);
	::AECreateDesc(typeNull,NULL,0,&reply);
	try
	{
		err = ::AECreateDesc(typeProcessSerialNumber,(Ptr)&psn,sizeof(ProcessSerialNumber),&target);
		if( err != noErr )   throw 0;
		err = ::AECreateAppleEvent(inEventClass,inEventID,&target,kAutoGenerateReturnID,kAnyTransactionID,&ae);
		if( err != noErr )   throw 0;
		{
			AStTextPtr	textptr(inText,0,inText.GetItemCount());
			err = ::AECreateDesc(typeChar,textptr.GetPtr(),(Size)(textptr.GetByteCount()),&desc);
		}
		if( err != noErr )   throw 0;
		err = ::AEPutParamDesc(&ae,keyDirectObject,&desc);
		if( err != noErr )   throw 0;
		err = ::AESend(&ae,&reply,kAENoReply,kAENormalPriority,0L,NULL,NULL);
		if( err != noErr )   throw 0;
		CAppleEventReader	rep(&reply);
		rep.GetParamText(keyDirectObject,typeUTF8Text,outText);
		result = true;
	}
	catch(...)
	{
	}
	::AEDisposeDesc(&target);
	::AEDisposeDesc(&ae);
	::AEDisposeDesc(&reply);
	::AEDisposeDesc(&desc);
	return result;
}

/**
File�̃p�����[�^�݂̂�����AppleEvent�𑗐M����
*/
ABool	ALaunchWrapper::SendAppleEvent_File( const AAppAcc& inApp, const OSType inEventClass, const OSType inEventID, const AFileAcc& inFile )
{
	ProcessSerialNumber	psn;
	if( FindAProcess(inApp,psn) == false )   return false;
	
	OSErr	err;
	
	//#1034 FSSpec	docSpec;
	//#1034 if( inFile.GetFSSpec(docSpec) == false )   return false;
	FSRef	docFSRef;//#1034
	if( inFile.GetFSRef(docFSRef) == false )   return false;//#1034
	AliasHandle	alias = NULL;
	AEDescList	docList;
	AEDesc	docDesc, targetDesc, reply;
	AppleEvent	ae;
	AECreateDesc(typeNull,NULL,0,&docDesc);
	AECreateDesc(typeNull,NULL,0,&targetDesc);
	AECreateDesc(typeNull,NULL,0,&reply);
	AECreateDesc(typeNull,NULL,0,&ae);
	try
	{
		err = AECreateList(NULL,0,false,&docList);
		if( err != noErr )   throw 0;
		//#1034 err = NewAliasMinimal(&docSpec,&alias);
		err = FSNewAliasMinimal(&docFSRef,&alias);//#1034
		if( err != noErr )   throw 0;
		HLock((Handle)alias);
		err = AECreateDesc(typeAlias,(Ptr)*alias,GetHandleSize((Handle)alias),&docDesc);
		HUnlock((Handle)alias);
		if( err != noErr )   throw 0;
		err = AECoerceDesc(&docDesc,typeAEList,&docList);
		if( err != noErr )   throw 0;
		AEDisposeDesc(&docDesc);
		AECreateDesc(typeNull,NULL,0,&docDesc);
		DisposeHandle((Handle)alias);
		alias = NULL;
		
		err = AECreateDesc(typeProcessSerialNumber,(Ptr)&psn,sizeof(ProcessSerialNumber),&targetDesc);
		if( err != noErr )   throw 0;
		err = AECreateAppleEvent(inEventClass,inEventID,&targetDesc,kAutoGenerateReturnID,kAnyTransactionID,&ae);
		if( err != noErr )   throw 0;
		err = AEPutParamDesc(&ae,keyDirectObject,&docList);
		if( err != noErr )   throw 0;
		err = AESend(&ae,&reply,kAENoReply,kAENormalPriority,kAEDefaultTimeout,NULL,NULL);
		if( err != noErr )   throw 0;
	}
	catch(...)
	{
	}
	if( alias != NULL )   DisposeHandle((Handle)alias);
	AEDisposeDesc(&docDesc);
	AEDisposeDesc(&reply);
	AEDisposeDesc(&ae);
	AEDisposeDesc(&targetDesc);
	return true;
}

//#229
/**
SendAppleEvent_File��BooleanValue�t����

���e�X�g
#229�̑΍�̂��ߍ쐬�������A'nec?'��true�ɂ��Ă����������Ȃ����߁A�R�����g�A�E�g�B64bit���Ή�
*/
/*
ABool	ALaunchWrapper::SendAppleEvent_File_WithBooleanValue( const AAppAcc& inApp, const OSType inEventClass, const OSType inEventID, 
		const OSType inBooleanValueClass, const ABool inBooleanValue, const AFileAcc& inFile )
{
	ProcessSerialNumber	psn;
	if( FindAProcess(inApp,psn) == false )   return false;
	
	OSErr	err;
	
	FSSpec	docSpec;
	if( inFile.GetFSSpec(docSpec) == false )   return false;
	AliasHandle	alias = NULL;
	AEDescList	docList;
	AEDesc	docDesc, targetDesc, reply;
	AppleEvent	ae;
	AECreateDesc(typeNull,NULL,0,&docDesc);
	AECreateDesc(typeNull,NULL,0,&targetDesc);
	AECreateDesc(typeNull,NULL,0,&reply);
	AECreateDesc(typeNull,NULL,0,&ae);
	
	AEDesc	boolvalueDesc;//#229 �ύX�ӏ�
	AECreateDesc(typeNull,NULL,0,&boolvalueDesc);//#229 �ύX�ӏ�
	try
	{
		err = AECreateList(NULL,0,false,&docList);
		if( err != noErr )   throw 0;
		err = NewAliasMinimal(&docSpec,&alias);
		if( err != noErr )   throw 0;
		HLock((Handle)alias);
		err = AECreateDesc(typeAlias,(Ptr)*alias,GetHandleSize((Handle)alias),&docDesc);
		HUnlock((Handle)alias);
		if( err != noErr )   throw 0;
		err = AECoerceDesc(&docDesc,typeAEList,&docList);
		if( err != noErr )   throw 0;
		AEDisposeDesc(&docDesc);
		AECreateDesc(typeNull,NULL,0,&docDesc);
		DisposeHandle((Handle)alias);
		alias = NULL;
		
		err = AECreateDesc(typeBoolean,(Ptr)&inBooleanValue,sizeof(Boolean),&boolvalueDesc);//#229 �ύX�ӏ�
		if( err != noErr )   throw 0;//#229 �ύX�ӏ�
		
		err = AECreateDesc(typeProcessSerialNumber,(Ptr)&psn,sizeof(ProcessSerialNumber),&targetDesc);
		if( err != noErr )   throw 0;
		err = AECreateAppleEvent(inEventClass,inEventID,&targetDesc,kAutoGenerateReturnID,kAnyTransactionID,&ae);
		if( err != noErr )   throw 0;
		err = AEPutParamDesc(&ae,keyDirectObject,&docList);
		if( err != noErr )   throw 0;
		
		err = AEPutParamDesc(&ae,inBooleanValueClass,&boolvalueDesc);//#229 �ύX�ӏ�
		if( err != noErr )   throw 0;//#229 �ύX�ӏ�
		
		err = AESend(&ae,&reply,kAENoReply,kAENormalPriority,kAEDefaultTimeout,NULL,NULL);
		if( err != noErr )   throw 0;
	}
	catch(...)
	{
	}
	AEDisposeDesc(&boolvalueDesc);//#229 �ύX�ӏ�
	
	if( alias != NULL )   DisposeHandle((Handle)alias);
	AEDisposeDesc(&docDesc);
	AEDisposeDesc(&reply);
	AEDisposeDesc(&ae);
	AEDisposeDesc(&targetDesc);
	return true;
}
*/

//B0103
/**
ODB�C���^�[�t�F�C�X�ő��A�v���P�[�V�����֕ύX��ʒm����
*/
void	ALaunchWrapper::SendODBModified( const OSType inODBSender, const AText& inODBToken, const AFileAcc& inODBFile, 
		const ABool inFileLocationChanged, const AFileAcc& inNewFile )
{
	AAppAcc	app('APPL',inODBSender);
	ProcessSerialNumber	psn;
	if( FindAProcess(app,psn) == false )   return;
	OSErr	err;
	AEDesc	target, desc1, desc2, desc3;
	AppleEvent	ae, reply;
	::AECreateDesc(typeNull,NULL,0,&desc1);
	::AECreateDesc(typeNull,NULL,0,&desc2);
	::AECreateDesc(typeNull,NULL,0,&desc3);
	::AECreateDesc(typeNull,NULL,0,&ae);
	::AECreateDesc(typeNull,NULL,0,&reply);
	try
	{
		err = ::AECreateDesc(typeProcessSerialNumber,(Ptr)&psn,sizeof(ProcessSerialNumber),&target);
		if( err != noErr )   throw 0;
		err = ::AECreateAppleEvent(kODBEditorSuite,kAEModifiedFile,&target,kAutoGenerateReturnID,kAnyTransactionID,&ae);
		if( err != noErr )  throw 0;
		
		FSRef	fsref;
		if( inODBFile.GetFSRef(fsref) == false )   throw 0;
		err = ::AECreateDesc(typeFSRef,&fsref,sizeof(fsref),&desc1);
		if( err != noErr )   throw 0;
		err = ::AEPutParamDesc(&ae,keyDirectObject,&desc1);
		if( err != noErr )   throw 0;
		
		if( inFileLocationChanged == true )
		{
			FSRef	newfsref;
			if( inNewFile.GetFSRef(newfsref) == false )   throw 0;
			err = ::AECreateDesc(typeFSRef,&newfsref,sizeof(newfsref),&desc2);
			if( err != noErr )   throw 0;
			err = ::AEPutParamDesc(&ae,keyNewLocation,&desc2);
			if( err != noErr )   throw 0;
		}
		CAEDesc::SetText(inODBToken,desc3);
		err = ::AEPutParamDesc(&ae,keyFileSenderToken,&desc3);
		if( err != noErr )   throw 0;
		
		err = ::AESend(&ae,&reply,kAENoReply,kAENormalPriority,0L,NULL,NULL);
	}
	catch(...)
	{
	}
	::AEDisposeDesc(&target);
	::AEDisposeDesc(&ae);
	::AEDisposeDesc(&reply);
	::AEDisposeDesc(&desc1);
	::AEDisposeDesc(&desc2);
	::AEDisposeDesc(&desc3);
}

//B0103
/**
ODB�C���^�[�t�F�C�X�ő��A�v���P�[�V�����֕ҏW�I����ʒm����
*/
void	ALaunchWrapper::SendODBClosed( const OSType inODBSender, const AText& inODBToken, const AFileAcc& inODBFile )
{
	AAppAcc	app('APPL',inODBSender);
	ProcessSerialNumber	psn;
	if( FindAProcess(app,psn) == false )   return;
	OSErr	err;
	AEDesc	target, desc1, /*desc2, */desc3;
	AppleEvent	ae, reply;
	::AECreateDesc(typeNull,NULL,0,&desc1);
	::AECreateDesc(typeNull,NULL,0,&desc3);
	::AECreateDesc(typeNull,NULL,0,&ae);
	::AECreateDesc(typeNull,NULL,0,&reply);
	try
	{
		err = ::AECreateDesc(typeProcessSerialNumber,(Ptr)&psn,sizeof(ProcessSerialNumber),&target);
		if( err != noErr )   throw 0;
		err = ::AECreateAppleEvent(kODBEditorSuite,kAEClosedFile,&target,kAutoGenerateReturnID,kAnyTransactionID,&ae);
		if( err != noErr )   throw 0;
		
		FSRef	fsref;
		if( inODBFile.GetFSRef(fsref) == false )   throw 0;
		err = ::AECreateDesc(typeFSRef,&fsref,sizeof(fsref),&desc1);
		if( err != noErr )   throw 0;
		err = ::AEPutParamDesc(&ae,keyDirectObject,&desc1);
		if( err != noErr )   throw 0;
		
		CAEDesc::SetText(inODBToken,desc3);
		if( err != noErr )   throw 0;
		err = ::AEPutParamDesc(&ae,keyFileSenderToken,&desc3);
		if( err != noErr )   throw 0;
		
		err = ::AESend(&ae,&reply,kAENoReply,kAENormalPriority,0L,NULL,NULL);
	}
	catch(...)
	{
	}
	::AEDisposeDesc(&target);
	::AEDisposeDesc(&ae);
	::AEDisposeDesc(&reply);
	::AEDisposeDesc(&desc1);
	::AEDisposeDesc(&desc3);
}

//win
/**
�v���Z�X��Kill
*/
void	ALaunchWrapper::KillProcess( const APID inPID )
{
	::kill(inPID,SIGTERM);
}

//#436
/**
�R�}���h���s
*/
void	ALaunchWrapper::ExecCommand( const AText& inCommand )
{
	AStCreateCstringFromAText	textptr(inCommand);
	::system(textptr.GetPtr());
}

//#436
/**
�f�B���N�g��
*/
void	ALaunchWrapper::ChangeDirectory( const AText& inDir )
{
	AStCreateCstringFromAText	textptr(inDir);
	::chdir(textptr.GetPtr());
}

//#1034 AFileWrapper����ړ�
/**
�t�@�C�����S�~���ֈړ�
*/
void	ALaunchWrapper::MoveToTrash( const AFileAcc& inFile )
{
	OSStatus	err = noErr;
	
	AFileAcc	trashfolder;
	AFileWrapper::GetTrashFolder(trashfolder);
	FSRef	trashref;
	trashfolder.GetFSRef(trashref);
	FSRef	fileref;
	inFile.GetFSRef(fileref);
	err = ::FSMoveObject(&fileref,&trashref,NULL);
	//B0000
	if( err == dupFNErr )
	{
		AText	newfilename;
		inFile.GetFilename(newfilename);
		AText	datetimetext;
		ADateTimeWrapper::GetDateTimeTextForFileName(datetimetext);
		newfilename.AddText(datetimetext);
		AFileAcc	file;
		file.CopyFrom(inFile);
		file.Rename(newfilename);
		file.GetFSRef(fileref);
		err = ::FSMoveObject(&fileref,&trashref,NULL);
		if( err != noErr )   _ACErrorNum("FSMoveObject() returned error: ",err,NULL);
	}
	ALaunchWrapper::NotifyFileChanged(trashfolder);
}

//#1034 AFileWrapper����ړ�
/**
�t�@�C�����ύX���ꂽ���Ƃ�OS�֒ʒm
*/
void	ALaunchWrapper::NotifyFileChanged( const AFileAcc& inFileAcc )//B0197
{
	AAppAcc	finder('FNDR','MACS');
	ALaunchWrapper::SendAppleEvent_File(finder,kAEFinderSuite,kAESync,inFileAcc);
}


