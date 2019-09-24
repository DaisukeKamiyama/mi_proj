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

AApplication
�A�v���P�[�V����

*/

#include "stdafx.h"

#include "../Frame.h"
//#1159 #include <curl/curl.h>

//�f�o�b�O���[�h
ABool	AApplication::sEnableAPICBTraceMode = false;
ABool	AApplication::sEnableDebugTraceMode = false;

//�A�v���P�[�V�����I�u�W�F�N�g�ւ̃|�C���^
AApplication*	AApplication::sApplication = NULL;

#pragma mark ===========================
#pragma mark [�N���X]AApplication
#pragma mark ===========================
//�A�v���P�[�V����

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AApplication::AApplication() : AObjectArrayItem(NULL), mEnableDebugMenu(false), mEnableTraceLog(false)
		,mIsActive(true)//B0442
		,mWindowRotating(false), mRotateActiveWindowID(kObjectID_Invalid)//#175
		,mModalAlertWindowID(kObjectID_Invalid)//#291
		,mModalCancelAlertWindowID(kObjectID_Invalid)//#1217
,mDocumentArray(NULL)//#693,true)//#417
,mWindowArray(NULL)//#693 ,true)//#417
,mViewArray(NULL)//#693 ,true)//#417
,mThreadArray(NULL)//#693 ,true)//#417
,mLuaVM(NULL)//#567 #1170
,mToolTipWindowID(kObjectID_Invalid)//#688
,mNowDoingInternalEvent(false)
{
	//cURL������ #427
	//#1159 curl_global_init(CURL_GLOBAL_ALL);
	
	sApplication = this;
	//Wrapper Init B0000 ������
	AFontWrapper::Init();//win ABase()����ړ��iLocalizedText���g�p���邽��)Windows�p��LocalizedText��������CAppImp::Setup()��
	AColorWrapper::Init();
	AScrapWrapper::Init();//win
	
	//AWindow Init
	AWindow::STATIC_Init();//win
	
	//Unicode Data   win 080618
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	AFileAcc	unicodeDataFile, widthFile;
#if IMPLEMENTATION_FOR_MACOSX
	unicodeDataFile.SpecifyChild(appFile,"Contents/Resources/UnicodeData.txt");
	widthFile.SpecifyChild(appFile,"Contents/Resources/EastAsianWidth.txt");//#415
#else
	unicodeDataFile.SpecifySister(appFile,"UnicodeData.txt");
	widthFile.SpecifySister(appFile,"EastAsianWidth.txt");//#415
#endif
	mUnicodeData.Init(unicodeDataFile,widthFile);//#852
	
	//#1012
	AText	name;
	name.SetCstring("icon_CheckBoxOn.png");
	CUserPane::RegisterImageByName(kImageID_iconChecked,name);
	name.SetCstring("icon_CheckBoxOff.png");
	CUserPane::RegisterImageByName(kImageID_iconUnchecked,name);
	
	/*#1012
	//�A�C�R���o�^
	AFileAcc	resFolder;
	resFolder.SpecifyChild(appFile,"Contents/Resources");
	AFileAcc	iconfile;
	
	iconfile.SpecifyChild(resFolder,"icon_CheckBoxOn.icns");
	CWindowImp::RegisterIcon(kIconID_CheckBoxOn,iconfile);
	iconfile.SpecifyChild(resFolder,"icon_CheckBoxOff.icns");
	CWindowImp::RegisterIcon(kIconID_CheckBoxOff,iconfile);
	*/
	//iconfile.SpecifyChild(resFolder,"icon_menutriangle.icns");//#232
	//CWindowImp::RegisterIcon(kIconID_Abs_MenuTriangle,iconfile);//#232
	/*
#if IMPLEMENTATION_FOR_MACOSX
	iconfile.SpecifyChild(resFolder,"icon_NoIcon.icns");//B0000 �_�~�[�A�C�R���i�^�����j
	CWindowImp::RegisterIcon(kIconID_NoIcon,iconfile);//B0000
	iconfile.SpecifyChild(resFolder,"icon_RowMoveUp.icns");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveUp,iconfile);
	iconfile.SpecifyChild(resFolder,"icon_RowMoveDown.icns");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveDown,iconfile);
	iconfile.SpecifyChild(resFolder,"icon_RowMoveTop.icns");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveTop,iconfile);
	iconfile.SpecifyChild(resFolder,"icon_RowMoveBottom.icns");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveBottom,iconfile);
	iconfile.SpecifyChild(resFolder,"doc.icns");//win
	CWindowImp::RegisterIcon(kIconID_Abs_Document,iconfile,"icon_doc");//win
	iconfile.SpecifyChild(resFolder,"folder.icns");//win
	CWindowImp::RegisterIcon(kIconID_Abs_Folder,iconfile,"icon_folder");//win
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	CWindowImp::RegisterIcon(kIconID_NoIcon,L"NoIcon");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveUp,L"abs_rowmoveup");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveDown,L"abs_rowmovedown");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveTop,L"abs_rowmovetop");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveBottom,L"abs_rowmovebottom");
	CWindowImp::RegisterIcon(kIconID_Abs_MenuTriangle,L"abs_menutriangle");
	CWindowImp::RegisterIcon(kIconID_Abs_Document,L"abs_document","icon_doc");//win
	CWindowImp::RegisterIcon(kIconID_Abs_Folder,L"abs_folder","icon_folder");//win
#endif
	*/
	NVI_GetMenuManager().RegisterContextMenu(kContextMenuID_ButtonView);//#232 #0
	NVI_GetMenuManager().RegisterContextMenu("AbsFrameworkCommonResource/ContextMenu_EditBox",kContextMenuID_EditBox);//#135
	
	
	//EditBoxView�̃f�t�H���g�t�H���g�E�T�C�Y��ݒ� win
	AText	defaultFontName;
	AFontWrapper::GetAppDefaultFontName(defaultFontName);//#375
	AView_EditBox::SPI_SetDefaultFont(defaultFontName,9.0);
	
#if IMPLEMENTATION_FOR_WINDOWS
	//�s�܂�Ԃ��v�Z�pOffscreen����
	CWindowImp::InitLineBreakOffscreen();
#endif
	//#567 Lua VM���J�� #1170
    mLuaVM = ::lua_open();
	::luaL_openlibs(mLuaVM);
}
/**
�f�X�g���N�^
*/
AApplication::~AApplication()
{
	//#693
	//temp�t�H���_�S�̂��폜�i�t�@�C���E�t�H���_���ׂāj
	AFileAcc	tempFolder;
	AFileWrapper::GetTempFolder(tempFolder);
	tempFolder.DeleteFileOrFolderRecursive();
	
	//
	//#567 Lua VM����� #1170
	::lua_close(mLuaVM);
	
	//sApplication = NULL;
	//cURL�I������ #427
	//#1159 curl_global_cleanup();
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---Run/Quit

//#182
/**
main����R�[�������
*/
int	AApplication::NVI_Run()
{
	_AInfo("AApp::NVI_Run() started.",this);
	
	//�A�v�����s��Ԃֈڍs�i���s��ԂłȂ��ƁA��ʕ`�悳��Ȃ��j#0 NVIDO_Run�̑O�Ɏ��s����悤�ɏC�� #0
	NVM_SetWorking(true);
	
	if( NVIDO_Run() == true )
	{
		//#291
		AWindowDefaultFactory<AWindow_ModalAlert>	modalAlertWindowFactory;
		mModalAlertWindowID = NVI_CreateWindow(modalAlertWindowFactory);
		
		//���[�_��Cancel�{�^���t��Alert #1217
		AWindowDefaultFactory<AWindow_ModalCancelAlert>	modalCancelAlertWindowFactory;
		mModalCancelAlertWindowID = NVI_CreateWindow(modalCancelAlertWindowFactory);
		
		//#688
		//�c�[���`�b�v�E�C���h�E����
		AWindowDefaultFactory<AWindow_ToolTip>	tooltipWindowFactory;
		mToolTipWindowID = NVI_CreateWindow(tooltipWindowFactory);
		NVI_GetToolTipWindow().NVI_Create(kObjectID_Invalid);
		
		//win 080728
		NVI_UpdateMenu();
		
		//B0000
		ATrace::StartPostInternalEvent();
		
		_AInfo("Before calling mAppImp.Run().",this);
		
		//B0372
		//SPI_CreateNewWindowAutomatic();//AppleEvent AppOpen����M���ɃR�[������悤�C���i���ꂾ�ƁA���ރ_�u���N���b�N�ƂƂ��ɋN�����鎞�ɖ��j
		
		//Windows: mAppImp.Run()�̓A�v���I���܂ŕԂ��Ă��Ȃ�
		//Mac OS X: mAppImp.Run()�͂����ɏI������i#688�ɂ��awakeFromNib����NVI_Run()���R�[������悤�ɂ�������
		int result = mAppImp.Run();
		_AInfo("After calling mAppImp.Run().",this);
		return result;
	}
	else
	{
		return 0;
	}
}

//#1102
/**
�I�������s
�i���j���[������̏I���͂��̏������R�[������B�j
*/
void	AApplication::NVI_TryQuit()
{
	//OS�ɑ΂��ďI�����w���BOS����NVI_Quit()���R�[�������B
	GetImp().Quit();
}

//#182
/**
OS�̏I���O�����imAppImp.Quit()�̌�j����R�[�������
*/
ABool	AApplication::NVI_Quit(/*#1102 const ABool inContinuing */)//#433
{
	//�h���N���X�ł̏I������
	if( NVIDO_Quit(/*#1102 inContinuing*/) == true )//#433
	{
		//#291
		NVI_DeleteWindow(mModalAlertWindowID);
		mModalAlertWindowID = kObjectID_Invalid;
		
		//���[�_��Cancel�{�^���t��Alert #1217
		NVI_DeleteWindow(mModalCancelAlertWindowID);
		mModalCancelAlertWindowID = kObjectID_Invalid;
		
		//#688
		//�c�[���`�b�v�E�C���h�E�폜
		NVI_DeleteWindow(mToolTipWindowID);
		mToolTipWindowID = kObjectID_Invalid;
		
		//NVIDO_Quit()���ō폜����Ȃ������E�C���h�E�S�폜 #688 �����ō폜���Ȃ���AApplication�I�u�W�F�N�g�������view�Ƃ̏����s��ō폜����Ă��܂�
		while( mWindowArray.GetItemCount() > 0 )
		{
			AWindowID	winID = mWindowArray.GetObject(0).GetObjectID();
			mWindowArray.GetObject(0).NVI_Close();
			NVI_DeleteWindow(winID);
		}
		
		//�h�L�������g�S�폜 #688 AApp::NVIDO_Quit()����ړ�
		NVI_DeleteAllDocument();
		
		//NVIDO_Quit()���ō폜����Ȃ������X���b�h��S�폜
		while( mThreadArray.GetItemCount() > 0 )
		{
			NVI_DeleteThread(mThreadArray.GetObject(0).GetObjectID());
		}
		
		//Imp�����I���i����ɂ��A�{�C�x���g�g�����U�N�V�����I����ARun()���I������B�j
		//#1102 mAppImp.Quit();
		
		return true;
	}
	else
	{
		return false;
	}
}

#pragma mark ===========================

/**
�S�E�C���h�E�A�h�L�������g��Update���s��
�i���ݒ�̔��f���̂��߂ɗp����j
*/
void	AApplication::NVI_UpdateAll()
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		mWindowArray.GetObject(index).NVI_UpdateProperty();
	}
	/*#182
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		mDocumentArray.GetObject(index).NVI_UpdateProperty();
	}
	*/
	NVIDO_UpdateAll();
}

/*#199
//�S�ẴE�C���h�E�����B
//NVI_TryCloseForQuit()���R�[�����A�P�ł�false������΁A���̎��_�ō폜�������I�����Afalse��Ԃ�
ABool	AApplication::NVI_CloseAllWindow( const ABool inQuit )
{
	while( mWindowArray.GetItemCount() > 0 )
	{
		AWindowID	winID = mWindowArray.GetObject(0).GetObjectID();
		//�E�C���h�E�폜���s�i�폜�s�̏ꍇ��false�ŕԂ��Ă���j
		if( NVI_GetWindowByID(winID).NVI_TryCloseForAllClose(inQuit) == false )
		{
			return false;
		}
	}
	return true;
}
*/

#pragma mark ===========================

#pragma mark ---���C���X���b�h����

/**
NVI_WakeTrigger()�ŋN�������܂ŃX���[�v����
�imWakeFlag��true�ɂȂ�܂Ńu���b�N��Ԃɂ���j
*/
void	AApplication::NVI_Sleep()
{
	AStMutexLocker	mutexlocker(mWakeFlagMutex);
	mWakeFlag = false;
	while( mWakeFlag == false )
	{
		//mWakeFlagCondition�ɃV�O�i����������܂Ńu���b�N�A���A�u���b�N����mutex�̓A�����b�N�����
		mWakeFlagCondition.WaitForCondition(mWakeFlagMutex);
	}
	mWakeFlag = false;
}

/**
NVI_WakeTrigger()�ŋN�������܂ŃX���[�v����
�imWakeFlag��true�ɂȂ�܂Ńu���b�N��Ԃɂ���j
*/
ABool	AApplication::NVI_SleepWithTimer( const float inTimerSecond )//#1483 ANumber��float
{
	ABool	result = true;
	AStMutexLocker	mutexlocker(mWakeFlagMutex);
	mWakeFlag = false;
	{
		//mWakeFlagCondition�ɃV�O�i����������܂Ńu���b�N�A���A�u���b�N����mutex�̓A�����b�N�����
		//�������A�^�C�}�[���������炻���Ńu���b�N����
		result = mWakeFlagCondition.WaitForConditionWithTimer(mWakeFlagMutex,inTimerSecond);
	}
	mWakeFlag = false;
	return result;
}

/**
�X���[�v��Ԃ̃X���b�h���N����
*/
void	AApplication::NVI_WakeTrigger()
{
	AStMutexLocker	mutexlocker(mWakeFlagMutex);
	if( mWakeFlag == false )//B0314
	{
		mWakeFlag = true;
		mWakeFlagCondition.SignalWithCondition();
	}
}

#pragma mark ===========================

#pragma mark ---���j���[�X�V

/**
���j���[�X�V
*/
void	AApplication::NVI_UpdateMenu()
{
	//#688
	//���݂Ə�Ԃ��Ⴄ���ڂ̂�enable/disable�ݒ肷�邽�߂ɁA�o�b�t�@�[���[�h��ݒ肷��B
	//����EndEnableMenuItemBufferredMode()�Ńo�b�t�@�[���[�h���I������B�i�I�����Ɏ��ۂ̃��j���[���ڂւ�enable/disable�ݒ肪�Ȃ����j
	AApplication::GetApplication().NVI_GetMenuManager().StartEnableMenuItemBufferredMode();
	
	//�܂��A�S�Ẵ��j���[��Disable�ɂ���
	AApplication::GetApplication().NVI_GetMenuManager().DisableAllMenuBarMenuItems();
	//#688 �I����������UpdateMenu�����s���Ȃ��悤�ɂ���
	if( NVI_IsWorking() == false )   return;
	
	//#688 EVT_UpdateMenu()�𒼐ڎ��s�i�ȑO��MenuUpdate�C�x���g�����g�Ɍ������ē����Ă������A�K�v�Ȃ������Ȃ̂ƁA�C�x���g����Ⴂ���̉\�����Ȃ������߁A���ڎ��s�ɕύX�B�j
	//�t�H�[�J�X�E�C���h�E�ɂ���EVT_UpdateMenu()�����s
	AWindowID	winID = AApplication::GetApplication().NVI_GetFocusWindowID();
	if( winID != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(winID).EVT_UpdateMenu();
	}
	
	//�A�v���P�[�V�����ɂ���EVT_UpdateMenu()�����s
	AApplication::GetApplication().EVT_UpdateMenu();
	
	//#688
	//Mac(Cocoa)�̏ꍇ�͏��menu��enable/disable�̓o�b�t�@�[���[�h�œ������B
	//OS����automatic validation�����ɂ��AAppDelegate��validateMenuItem:����A�o�b�t�@�[��ǂݏo���āA�X�V�����B
#if !IMPLEMENTATION_FOR_MACOSX
	AApplication::GetApplication().NVI_GetMenuManager().EndEnableMenuItemBufferredMode();
#endif
	
	/*#688 ���ڎ��s�ɕύX�imenu��enable�ɂȂ�܂Ń^�C�����O���ł���ƁA�I����R�s�[���̂Ƃ��A�R�s�[�̃V���[�g�J�b�g�������Ȃ��^�C�~���O�����邩������Ȃ��j
#if IMPLEMENTATION_FOR_MACOSX
	//MenuUpdate�C�x���g�����g�Ɍ������ē�����i���j���[�R�}���h�����Ɠ������[�g��ʂ��j
	HICommand	command;
	command.attributes = NULL;
	command.commandID = kCommandID_UpdateCommandStatus;
	command.menu.menuRef = NULL;
	command.menu.menuItemIndex = 0;
	::ProcessHICommand(&command);
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	::PostMessage(CWindowImp::GetMostFrontWindow(),WM_APP_UPDATEMENU,0,0);
#endif
	*/
}

#pragma mark ===========================

#pragma mark ---�C�x���g����

//#182
/**
���j���[���ڑI��������
*/
ABool	AApplication::EVT_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	/*#513 ��͂�h���N���X�ŏ���
	//Quit�̂�AApplication�ŏ�������
	switch(inMenuItemID)
	{
	  case kMenuItemID_Quit:
		{
			NVI_Quit();
			return true;
		}
	}
	*/
	//�h���N���X���ŏ���
	return EVTDO_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys);
}

//#182
/**
�����C�x���g����

�h���N���X�ŃI�[�o�[���C�h����B
�����A�I�[�o�[���C�h���Ȃ��Ă��A_ACThrow()���̃��O�_���v���ł���悤�ɁA�f�t�H���g������p�ӂ����B
*/
void	AApplication::EVTDO_DoInternalEvent( ABool& outUpdateMenu )
{
	outUpdateMenu = false;
	
	//�����ɗ������_�ŃL���[�ɂ��܂��Ă���C�x���g����������������B
	//����ɂ��A�i�����C�x���g�����݂��Ă�����胋�[�v������ꍇ�ƈ���āj
	//�}�E�X�C�x���g��\���X�V�C�x���g��OS�C�x���g�L���[�ɂ��߂��܂�EVT_DoInternalEvent()���ł��邮���葱���邱�Ƃ͂Ȃ��A
	//queueCount�����I��������AOS�֏�����Ԃ����Ƃ��ł���B
	AItemCount	queueCount = ABase::GetMainInternalEventQueueCount();
	
	AInternalEventType	type;
	AObjectID	senderObjectID = kObjectID_Invalid;
	ANumber	num;
	AText	text;
	AObjectIDArray	objectIDArray;
	for( AIndex i = 0; i < queueCount; i++ )
	{
		if( ABase::DequeueFromMainInternalEventQueue(type,senderObjectID,num,text,objectIDArray) == true )
		{
			if( NVI_DoInternalEvent_FrameCommon(type,senderObjectID,num,text,objectIDArray) == false )//win
			switch(type)
			{
			  case kInternalEvent_TraceLog:
				{
					AStCreateCstringFromAText	cstr(text);
					fprintf(stderr,"%s",cstr.GetPtr());
					break;
				}
			}
		}
	}
}

#pragma mark ===========================

#pragma mark ---�t���[�����ʓ����C�x���g����
//win

/**
�t���[�����ʓ����C�x���g����
*/
ABool	AApplication::NVI_DoInternalEvent_FrameCommon( const AInternalEventType inType, const AObjectID inSenderObjectID, 
		const ANumber inNumber, const AText& inText, const AObjectIDArray& inObjectIDArray )
{
	ABool	result = false;
	switch(inType)
	{
	  case kInternalEvent_Quit:
		{
			NVI_TryQuit();//#1102
			result = true;
			break;
		}
	  default:
		{
			//��������
			//�h���N���X�ŏ������邱��
			break;
		}
	}
	return result;
}

#pragma mark ===========================

#pragma mark ---FileOpenDialog
//#182

/**
FileOpenDialog���J��
*/
void	AApplication::NVI_ShowFileOpenWindow( const ABool inShowInvisibleFile )
{
	mAppImp.ShowFileOpenWindow(inShowInvisibleFile);
}

#pragma mark ===========================

#pragma mark ---ModalAlert
//#291

/**
���[�_���Ȍx���E�C���h�E��\������
*/
void	AApplication::NVI_ShowModalAlertWindow( const AText& inMessage1, const AText& inMessage2 )
{
	NVI_GetWindowByID(mModalAlertWindowID).NVI_Create(kObjectID_Invalid);
	(dynamic_cast<AWindow_ModalAlert&>(NVI_GetWindowByID(mModalAlertWindowID))).SPNVI_Show(inMessage1,inMessage2);
}

//#1217
/**
���[�_���Ȍx���E�C���h�E��\������
*/
void	AApplication::NVI_ShowModalAlertWindow( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText )
{
	NVI_GetWindowByID(mModalAlertWindowID).NVI_Create(kObjectID_Invalid);
	(dynamic_cast<AWindow_ModalAlert&>(NVI_GetWindowByID(mModalAlertWindowID))).SPNVI_Show(inMessage1,inMessage2,inOKButtonText);
}

#pragma mark ===========================

#pragma mark ---ModalCancelAlert

//#1217
/**
���[�_���Ȍx���E�C���h�E�iCancel�{�^���t���j��\������
*/
ABool	AApplication::NVI_ShowModalCancelAlertWindow( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText, const AText& inCancelButtonText )
{
	NVI_GetWindowByID(mModalCancelAlertWindowID).NVI_Create(kObjectID_Invalid);
	return (dynamic_cast<AWindow_ModalCancelAlert&>(NVI_GetWindowByID(mModalCancelAlertWindowID))).SPNVI_Show(inMessage1,inMessage2,inOKButtonText,inCancelButtonText);
}

#pragma mark ===========================

#pragma mark ---Tooltip

/**
�c�[���`�b�v���ǂ��擾
*/
AWindow_ToolTip&	AApplication::NVI_GetToolTipWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_ToolTip,kWindowType_ToolTip,mToolTipWindowID);
}

/**
�c�[���`�b�v�E�C���h�E��\��
*/
void	AApplication::NVI_HideToolTip()
{
	if( mToolTipWindowID == kObjectID_Invalid )
	{
		return;
	}
	
	if( NVI_GetToolTipWindow().NVI_IsWindowVisible() == true )
	{
		NVI_GetToolTipWindow().NVI_Hide();
	}
}


#pragma mark ===========================

#pragma mark ---�A�v���P�[�V�������擾
//#182

/**
�o�[�W�����擾
*/
void	AApplication::NVI_GetVersion( AText& outText ) const
{
	mAppImp.GetVersion(outText);
}

//#1096
/**
�r���h�ԍ��擾
*/
void	AApplication::NVI_GetBuildNumber( AText& outText ) const
{
	mAppImp.GetBuildNumber(outText);
}

//#305
/**
���݂̌���擾
*/
ALanguage	AApplication::NVI_GetLanguage() const
{
	return mAppImp.GetLanguage();
}

/**
���ꖈ�t�H���_���擾
*/
void	AApplication::NVI_GetLanguageName( AText& outText ) const
{
	switch(NVI_GetLanguage())
	{
	  case kLanguage_Japanese:
		{
			outText.SetCstring("ja");
			break;
		}
	  case kLanguage_English:
	  default:
		{
			outText.SetCstring("en");
			break;
		}
	}
}

//#1316
/**
�_�[�N���[�h���ǂ����̔���
*/
ABool	AApplication::NVI_IsDarkMode() const
{
	return GetImpConst().IsDarkMode();
}

//#1316
/**
�A�s�A�����X�^�C�v�ݒ�
*/
void	AApplication::NVI_SetAppearanceType( const AAppearanceType inAppearanceType )
{
	GetImp().SetAppearanceType(inAppearanceType);
}

#pragma mark ===========================

#pragma mark <���L�N���X(ADocument)�����^�폜�^�擾>

#pragma mark ===========================

#pragma mark ---�h�L�������g�����^�폜

/**
�h�L�������g�̐���
@note thread-safe
*/
AObjectID	AApplication::NVI_CreateDocument( ADocumentFactory& inFactory )
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	AIndex	docIndex = mDocumentArray.AddNewObject(inFactory);
	mDocumentUniqIDArray.AddItem();//#693
	return mDocumentArray.GetObject(docIndex).GetObjectID();
}

/**
�h�L�������g�̍폜
@note thread-safe
*/
void	AApplication::NVI_DeleteDocument( const AObjectID inDocID )
{
	//#896
	//�h�L�������g�폜������
	NVI_GetDocumentByID(inDocID).NVI_DocumentWillBeDeleted();
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	AIndex	docIndex = mDocumentArray.GetIndexByID(inDocID);
	mDocumentArray.Delete1Object(docIndex);
	mDocumentUniqIDArray.Delete1Item(docIndex);//#693
}

/**
�h�L�������g�̑S�폜
@note thread-safe
*/
void	AApplication::NVI_DeleteAllDocument()
{
	//#896
	//�h�L�������g�폜������
	for( AIndex i = 0; i < mDocumentArray.GetItemCount(); i++ )
	{
		mDocumentArray.GetObject(i).NVI_DocumentWillBeDeleted();
	}
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//�h�L�������g�S�폜
	mDocumentArray.DeleteAll();
	mDocumentUniqIDArray.DeleteAll();
}


#pragma mark ===========================

#pragma mark ---�h�L�������g�擾

/**
DocumentID����document�I�u�W�F�N�g�擾
@note thread-safe
*/
ADocument&	AApplication::NVI_GetDocumentByID( const ADocumentID inDocID )
{
	/*#272
	return mDocumentArray.GetObject(mDocumentArray.GetIndexByID(inDocID));
	*/
	return mDocumentArray.GetObjectByID(inDocID);
}

/**
DocumentID����document�I�u�W�F�N�g�擾(const)
@note thread-safe
*/
const ADocument&	AApplication::NVI_GetDocumentConstByID( const ADocumentID inDocID ) const
{
	/*#272
	return mDocumentArray.GetObjectConst(mDocumentArray.GetIndexByID(inDocID));
	*/
	return mDocumentArray.GetObjectConstByID(inDocID);
}

/**
�w��DocumentType�̃h�L�������g�����擾
@note thread-safe
*/
AItemCount	AApplication::NVI_GetDocumentCount( const ADocumentType inDocType ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	AItemCount	result = 0;
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObjectConst(index).NVI_GetDocumentType() == inDocType )
		{
			result++;
		}
	}
	return result;
}

/**
�w��DocumentType�̍ŏ��̃h�L�������g��ID���擾
@note thread-safe
*/
ADocumentID	AApplication::NVI_GetFirstDocumentID( const ADocumentType inDocType ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	return GetFirstDocumentID(inDocType);
}
ADocumentID	AApplication::GetFirstDocumentID( const ADocumentType inDocType ) const
{
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObjectConst(index).NVI_GetDocumentType() == inDocType )
		{
			return mDocumentArray.GetObjectConst(index).GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

/**
�w��DocID�̎��̃h�L�������g�iDocumentType���������́j��ID���擾
@note thread-safe
*/
ADocumentID	AApplication::NVI_GetNextDocumentID( const AObjectID inDocID ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	return GetNextDocumentID(inDocID);
}
ADocumentID	AApplication::GetNextDocumentID( const AObjectID inDocID ) const
{
	if( mDocumentArray.GetItemCount() == 0 )   return kObjectID_Invalid;
	AIndex	index = mDocumentArray.GetIndexByID(inDocID);
	if( index == kIndex_Invalid )
	{
		_ACError("document not found",this);
		return kObjectID_Invalid;
	}
	ADocumentType	docType = mDocumentArray.GetObjectConst(index).NVI_GetDocumentType();
	for( index++; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObjectConst(index).NVI_GetDocumentType() == docType )
		{
			return mDocumentArray.GetObjectConst(index).GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

/*#272
const ADocument&	AApplication::NVI_GetDocumentByIDConst( const ADocumentID inDocID ) const
{
	return mDocumentArray.GetObjectConst(mDocumentArray.GetIndexByID(inDocID));
}
*/

/**
DocumentID����document�^�C�v���擾
@note thread-safe
*/
ADocumentType	AApplication::NVI_GetDocumentTypeByID( const ADocumentID inDocID ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	/*#272
	return mDocumentArray.GetObjectConst(mDocumentArray.GetIndexByID(inDocID)).NVI_GetDocumentType();
	*/
	return mDocumentArray.GetObjectConstByID(inDocID).NVI_GetDocumentType();
}

/**
�^�C�g������document������
*/
ADocumentID	AApplication::NVI_GetDocumentIDByTitle( const ADocumentType inDocType, const AText& inTitle ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	for( AObjectID docID = GetFirstDocumentID(inDocType); docID != kObjectID_Invalid; docID = GetNextDocumentID(docID) )
	{
		AText	title;
		NVI_GetDocumentConstByID(docID).NVI_GetTitle(title);//#272
		if( inTitle.Compare(title) == true )
		{
			return docID;
		}
	}
	return kObjectID_Invalid;
}

/**
�t�@�C������document������
*/
ADocumentID	AApplication::NVI_GetDocumentIDByFile( const ADocumentType inDocType, const AFileAcc& inFile ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	for( AObjectID docID = GetFirstDocumentID(inDocType); docID != kObjectID_Invalid; docID = GetNextDocumentID(docID) )
	{
		AFileAcc	file;
		if( NVI_GetDocumentConstByID(docID).NVI_GetFile(file) == true )//#272
		{
			if( inFile.Compare(file) == true )
			{
				return docID;
			}
		}
	}
	return kObjectID_Invalid;
}

/**
�w��DocumentType���ŁA�E�C���h�E����N�Ԗڂ̃h�L�������g��ID���擾
*/
ADocumentID	AApplication::NVI_GetNthDocumentID( const ADocumentType inDocType, const ANumber inNumber ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	ANumber	num = 0;
	//�őO�ʂ̃E�C���h�E���珇�Ƀ��[�v
	for( AWindowRef	windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID != kObjectID_Invalid )
		{
			if( NVI_GetWindowConstByID(winID).NVI_IsDocumentWindow() == false)   continue;//#673 �t���[�e�B���O�E�C���h�E���͑ΏۊO�ɂ���
			for( AIndex zorderIndex = 0; 
						zorderIndex <  NVI_GetWindowConstByID(winID).NVI_GetSelectableTabCount();//#673NVI_GetTabCount(); 
						zorderIndex++ )
			{
				AIndex	tabIndex = NVI_GetWindowConstByID(winID).NVI_GetTabIndexByZOrderIndex(zorderIndex);
				ADocumentID	docID = NVI_GetWindowConstByID(winID).NVI_GetDocumentIDByTabIndex(tabIndex);
				if( docID == kObjectID_Invalid )   continue;//#92
				if( NVI_GetDocumentConstByID(docID).NVI_GetDocumentType() == inDocType )
				{
					num++;
					if( num == inNumber )
					{
						return docID;
					}
				}
			}
		}
	}
	return kObjectID_Invalid;
}

//#1034
/**
�e�L�X�g�h�L�������g���E�C���h�Eorder���Ɏ擾
�iAppleScript����g�p�����B�j
*/
void	AApplication::NVI_GetOrderedDocumentIDArray( AArray<ADocumentID>& outDocumentIDArray, const ADocumentType inDocType, const ABool inOnlyDocWithDocImp ) const
{
	//���ʊi�[�parray������
	outDocumentIDArray.DeleteAll();
	//document array�����b�N
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	ANumber	num = 0;
	//�őO�ʂ̃E�C���h�E���珇�Ƀ��[�v
	for( AWindowRef	windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
		 windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID != kObjectID_Invalid )
		{
			if( NVI_GetWindowConstByID(winID).NVI_IsDocumentWindow() == false)   continue;//#673 �t���[�e�B���O�E�C���h�E���͑ΏۊO�ɂ���
			for( AIndex zorderIndex = 0; 
				 zorderIndex <  NVI_GetWindowConstByID(winID).NVI_GetSelectableTabCount();//#673NVI_GetTabCount(); 
				 zorderIndex++ )
			{
				AIndex	tabIndex = NVI_GetWindowConstByID(winID).NVI_GetTabIndexByZOrderIndex(zorderIndex);
				ADocumentID	docID = NVI_GetWindowConstByID(winID).NVI_GetDocumentIDByTabIndex(tabIndex);
				if( docID == kObjectID_Invalid )   continue;//#92
				if( NVI_GetDocumentConstByID(docID).NVI_GetDocumentType() == inDocType )
				{
					AObjectID	docImpID = NVI_GetDocumentConstByID(docID).NVI_GetDocImpID();
					if( inOnlyDocWithDocImp == false || docImpID != kObjectID_Invalid )
					{
						outDocumentIDArray.Add(docID);
					}
				}
			}
		}
	}
}

//#1034
/**
�e�L�X�g�h�L�������g�̃E�C���h�Eorder���ł�index��Ԃ�
�iAppleScript����g�p�����B�j
*/
AIndex	AApplication::NVI_GetOrderIndexOfDocument( const ADocumentID inDocumentID, const ADocumentType inDocType, const ABool inOnlyDocWithDocImp ) const
{
	AArray<ADocumentID>	docIDArray;
	NVI_GetOrderedDocumentIDArray(docIDArray,inDocType,inOnlyDocWithDocImp);
	return docIDArray.Find(inDocumentID);
}

/**
�w��DocumentType���ŁA�E�C���h�E����1�Ԗڂ̃h�L�������g��ID���擾
*/
ADocumentID	AApplication::NVI_GetMostFrontDocumentID( const ADocumentType inDocType ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	//�őO�ʂ̃E�C���h�E���珇�Ƀ��[�v
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID != kObjectID_Invalid )//#216
		{
			ADocumentID	currentDocID = NVI_GetWindowConstByID(winID).NVI_GetCurrentDocumentID();
			if( currentDocID != kObjectID_Invalid )
			{
				if( NVI_GetDocumentConstByID(currentDocID).NVI_GetDocumentType() == inDocType )
				{
					return currentDocID;
				}
			}
		}
	}
	return kObjectID_Invalid;
}

/**
�S�h�L�������g����A�E�C���h�E����1�Ԗڂ̃h�L�������g��ID���擾�i�őO�ʂ��h�L�������g�E�C���h�E�łȂ��ꍇ�́AkObjectID_Invalid��Ԃ��j
*/
AObjectID	AApplication::NVI_GetDocumentIDOfMostFrontWindow() const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	//�őO�ʂ̃E�C���h�E���珇�Ƀ��[�v
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID != kObjectID_Invalid )//#216
		{
			ADocumentID	currentDocID = NVI_GetWindowConstByID(winID).NVI_GetCurrentDocumentID();
			if( currentDocID != kObjectID_Invalid )
			{
				return currentDocID;
			}
		}
	}
	return kObjectID_Invalid;
}

/**
�w��DocumentID�̃h�L�������g�����݂��邩�ǂ�����Ԃ�
@note thread-safe
*/
ABool	AApplication::NVI_IsDocumentValid( const AObjectID inDocID ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	/*#272
	return (mDocumentArray.GetIndexByID(inDocID)!=kIndex_Invalid);
	*/
	return mDocumentArray.ExistObject(inDocID);
}

//#693
/**
�h�L�������gUniqID���擾
*/
AUniqID	AApplication::NVI_GetDocumentUniqID( const ADocumentID inDocID ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	return mDocumentUniqIDArray.Get(mDocumentArray.GetIndexByID(inDocID));
}

//#693
/**
�h�L�������gUniqID����DocumentID(ObjectID)���擾
*/
ADocumentID	AApplication::NVI_GetDocumentIDByUniqID( const AUniqID inDocUniqID ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	AIndex	index = mDocumentUniqIDArray.Find(inDocUniqID);
	if( index == kIndex_Invalid )   return kObjectID_Invalid;
	return mDocumentArray.GetObjectConst(index).GetObjectID();
}

#pragma mark ===========================

#pragma mark <���L�N���X(AWindow)�����^�폜�^�擾>

#pragma mark ===========================

#pragma mark ---���I�����E�C���h�E�����^�폜

/**
���I�����E�C���h�E�̐���
*/
AObjectID	AApplication::NVI_CreateWindow( AWindowFactory& inFactory )
{
	AIndex	winIndex = mWindowArray.AddNewObject(inFactory);
	return mWindowArray.GetObject(winIndex).GetObjectID();
}

/**
���I�����E�C���h�E�̍폜
*/
void	AApplication::NVI_DeleteWindow( const AWindowID inWindowID )
{
	AIndex	winIndex = mWindowArray.GetIndexByID(inWindowID);
	mWindowArray.Delete1Object(winIndex);
}

#pragma mark ===========================

#pragma mark ---���I�E�Œ萶���E�C���h�E�擾

/**
WindowID����E�C���h�E�擾
*/
AWindow&	AApplication::NVI_GetWindowByID( const AWindowID inWindowID )
{
	/*#272
	AIndex	winIndex = mWindowArray.GetIndexByID(inWindowID);
	return mWindowArray.GetObject(winIndex);
	*/
	return mWindowArray.GetObjectByID(inWindowID);
}

/**
WindowID����E�C���h�E�擾
*/
const AWindow&	AApplication::NVI_GetWindowConstByID( const AWindowID inWindowID ) const
{
	/*#272
	AIndex	winIndex = mWindowArray.GetIndexByID(inWindowID);
	return mWindowArray.GetObjectConst(winIndex);
	*/
	return mWindowArray.GetObjectConstByID(inWindowID);
}

/**
�w��WindowType�̃h�L�������g�����擾
*/
AItemCount	AApplication::NVI_GetWindowCount( const AWindowType inWinType ) const
{
	AItemCount	result = 0;
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 ������
	for( AIndex index = 0; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		if( mWindowArray.GetObjectConst(index).NVI_GetWindowType() == inWinType || inWinType == kWindowType_Invalid )
		{
			result++;
		}
	}
	return result;
}

/**
�w��WindowType���ŁA�E�C���h�E����N�Ԗڂ̃E�C���h�E��ID���擾
*/
AWindowID	AApplication::NVI_GetNthWindowID( const AWindowType inWinType, const ANumber inNumber ) const
{
	//#654 ������ NVI_GetWindowIDByWindowRef()�ł̊eWindow�I�u�W�F�N�g�����WindowRef�������x���i���[�v�����[�v�j�̂ŉ��P
	
	//�����Y������Window��WindowRef, WindowID��z��Ɋi�[����
	AHashArray<AWindowRef>	winRefArray;
	AArray<AWindowID>	winIDArray;
	AItemCount	windowCount = mWindowArray.GetItemCount();
	for( AIndex i = 0; i < windowCount; i++ )
	{
		const AWindow&	window = mWindowArray.GetObjectConst(i);
		if( window.NVI_IsWindowVisible() == false )   continue;//#212 ��\���E�C���h�E�̓J�E���g���Ȃ�
		if( window.NVI_GetOverlayMode() == true )   continue;//#212 Overlay�E�C���h�E�̓J�E���g���Ȃ�
		if( window.NVI_GetWindowType() == inWinType || inWinType == kWindowType_Invalid )
		{
			winRefArray.Add(window.NVI_GetWindowRef());
			winIDArray.Add(window.GetObjectID());
		}
	}
	//�����Y���E�C���h�E�Ȃ���΂����Ń��^�[��
	if( winRefArray.GetItemCount() == 0 )   return kObjectID_Invalid;
	
	//�őO�ʂ̃E�C���h�E���珇�Ƀ��[�v
	ANumber	num = 0;
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		//��ō쐬�����z�񂩂猟��
		AIndex	index = winRefArray.Find(windowRef);
		if( index != kIndex_Invalid )
		{
			//WindowRef����v������J�E���^�C���N�������g���A����Nth�Ɉ�v������A����WindowID��Ԃ�
			num++;
			if( num == inNumber )
			{
				return winIDArray.Get(index);
			}
		}
	}
	return kObjectID_Invalid;
	
	/*#654
	ANumber	num = 0;
	//�őO�ʂ̃E�C���h�E���珇�Ƀ��[�v
	for( AWindowRef	windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID != kObjectID_Invalid )
		{
			if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == false )   continue;//#212 ��\���E�C���h�E�̓J�E���g���Ȃ�
			if( NVI_GetWindowConstByID(winID).NVI_GetOverlayMode() == true )   continue;//#212 Overlay�E�C���h�E�̓J�E���g���Ȃ�
			if( NVI_GetWindowConstByID(winID).NVI_GetWindowType() == inWinType || inWinType == kWindowType_Invalid )
			{
				num++;
				if( num == inNumber )
				{
					return winID;
				}
			}
		}
	}
	return kObjectID_Invalid;
	*/
}

//#1034
/**
�E�C���h�E���E�C���h�Eorder���Ɏ擾
�iAppleScript����g�p�����B�j
*/
void	AApplication::NVI_GetOrderedWindowIDArray( AArray<AWindowID>& outArray, const AWindowType inWindowType )
{
	//���ʊi�[�parray������
	outArray.DeleteAll();
	//�����Y������Window��WindowRef, WindowID��z��Ɋi�[����
	AHashArray<AWindowRef>	winRefArray;
	AArray<AWindowID>	winIDArray;
	AItemCount	windowCount = mWindowArray.GetItemCount();
	for( AIndex i = 0; i < windowCount; i++ )
	{
		const AWindow&	window = mWindowArray.GetObjectConst(i);
		if( window.NVI_IsWindowVisible() == false )   continue;//#212 ��\���E�C���h�E�̓J�E���g���Ȃ�
		if( window.NVI_GetOverlayMode() == true )   continue;//#212 Overlay�E�C���h�E�̓J�E���g���Ȃ�
		if( window.NVI_GetWindowType() == inWindowType || inWindowType == kWindowType_Invalid )
		{
			winRefArray.Add(window.NVI_GetWindowRef());
			winIDArray.Add(window.GetObjectID());
		}
	}
	//�����Y���E�C���h�E�Ȃ���΂����Ń��^�[��
	if( winRefArray.GetItemCount() == 0 )   return;
	
	//�őO�ʂ̃E�C���h�E���珇�Ƀ��[�v
	ANumber	num = 0;
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
		 windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		//��ō쐬�����z�񂩂猟��
		AIndex	index = winRefArray.Find(windowRef);
		if( index != kIndex_Invalid )
		{
			outArray.Add(winIDArray.Get(index));
		}
	}
	//�h���N���X�ŃE�C���h�E�ǉ�
	NVIDO_GetOrderedWindowIDArray(outArray,inWindowType);
}

/**
�w��WindowType���ŁA�E�C���h�E����1�Ԗڂ̃h�L�������g��ID���擾
*/
AWindowID	AApplication::NVI_GetMostFrontWindowID( const AWindowType inWinType, const ABool inIncludeFloating ) const
{
	//#654 ������ NVI_GetWindowIDByWindowRef()�ł̊eWindow�I�u�W�F�N�g�����WindowRef�������x���i���[�v�����[�v�j�̂ŉ��P
	
	//�����Y������Window��WindowRef, WindowID��z��Ɋi�[����
	AHashArray<AWindowRef>	winRefArray;
	AArray<AWindowID>	winIDArray;
	AItemCount	windowCount = mWindowArray.GetItemCount();
	for( AIndex i = 0; i < windowCount; i++ )
	{
		const AWindow&	window = mWindowArray.GetObjectConst(i);
		if( window.NVI_IsWindowVisible() == false )   continue;//#212 ��\���E�C���h�E�̓J�E���g���Ȃ�
		if( window.NVI_GetOverlayMode() == true )   continue;//#212 Overlay�E�C���h�E�̓J�E���g���Ȃ�
		//����inIncludeFloating��false�̏ꍇ�́A�t���[�e�B���O�E�C���h�E�͑ΏۃE�C���h�E�Ƃ��ăJ�E���g���Ȃ� #959
		if( inIncludeFloating == false )
		{
			if( window.NVI_IsFloating() == true )   continue;
		}
		//
		if( window.NVI_GetWindowType() == inWinType || inWinType == kWindowType_Invalid )//#688
		{
			winRefArray.Add(window.NVI_GetWindowRef());
			winIDArray.Add(window.GetObjectID());
		}
	}
	//�����Y���E�C���h�E�Ȃ���΂����Ń��^�[��
	if( winRefArray.GetItemCount() == 0 )   return kObjectID_Invalid;
	//�őO�ʂ̃E�C���h�E���珇�Ƀ��[�v
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		//��ō쐬�����z�񂩂猟��
		AIndex	index = winRefArray.Find(windowRef);
		if( index != kIndex_Invalid )
		{
			//WindowRef����v������A����WindowID��Ԃ�
			return winIDArray.Get(index);
		}
	}
	return kObjectID_Invalid;
	
	/*#654
	//�őO�ʂ̃E�C���h�E���珇�Ƀ��[�v
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID == kObjectID_Invalid )   continue;
		if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == false )   continue;//#212 ��\���E�C���h�E�̓J�E���g���Ȃ�
		if( NVI_GetWindowConstByID(winID).NVI_GetOverlayMode() == true )   continue;//#212 Overlay�E�C���h�E�̓J�E���g���Ȃ�
		if( NVI_GetWindowConstByID(winID).NVI_GetWindowType() == inWinType )
		{
			return winID;
		}
	}
	return kObjectID_Invalid;
	*/
}

//#567
/**
�w��WindowType���ŁA�E�C���h�E����1�Ԗڂ̃h�L�������g��ID���擾�i���O�E�C���h�E�w��L��j
*/
AWindowID	AApplication::NVI_GetMostFrontWindowID( const AWindowType inWinType, 
			const AWindowID inExceptWindowID ) const
{
	//#654 ������ NVI_GetWindowIDByWindowRef()�ł̊eWindow�I�u�W�F�N�g�����WindowRef�������x���i���[�v�����[�v�j�̂ŉ��P
	
	//�����Y������Window��WindowRef, WindowID��z��Ɋi�[����
	AHashArray<AWindowRef>	winRefArray;
	AArray<AWindowID>	winIDArray;
	AItemCount	windowCount = mWindowArray.GetItemCount();
	for( AIndex i = 0; i < windowCount; i++ )
	{
		const AWindow&	window = mWindowArray.GetObjectConst(i);
		if( window.NVI_IsWindowVisible() == false )   continue;//#212 ��\���E�C���h�E�̓J�E���g���Ȃ�
		if( window.NVI_GetOverlayMode() == true )   continue;//#212 Overlay�E�C���h�E�̓J�E���g���Ȃ�
		if( window.GetObjectID() == inExceptWindowID )   continue;
		if( window.NVI_GetWindowType() == inWinType || inWinType == kWindowType_Invalid )//#688
		{
			winRefArray.Add(window.NVI_GetWindowRef());
			winIDArray.Add(window.GetObjectID());
		}
	}
	//�����Y���E�C���h�E�Ȃ���΂����Ń��^�[��
	if( winRefArray.GetItemCount() == 0 )   return kObjectID_Invalid;
	//�őO�ʂ̃E�C���h�E���珇�Ƀ��[�v
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		//��ō쐬�����z�񂩂猟��
		AIndex	index = winRefArray.Find(windowRef);
		if( index != kIndex_Invalid )
		{
			//WindowRef����v������A����WindowID��Ԃ�
			return winIDArray.Get(index);
		}
	}
	return kObjectID_Invalid;
	
	/*#654
	//�őO�ʂ̃E�C���h�E���珇�Ƀ��[�v
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID == kObjectID_Invalid )   continue;
		if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == false )   continue;//#212 ��\���E�C���h�E�̓J�E���g���Ȃ�
		if( NVI_GetWindowConstByID(winID).NVI_GetOverlayMode() == true )   continue;//#212 Overlay�E�C���h�E�̓J�E���g���Ȃ�
		if( winID == inExceptWindowID )   continue;
		if( NVI_GetWindowConstByID(winID).NVI_GetWindowType() == inWinType )
		{
			return winID;
		}
	}
	return kObjectID_Invalid;
	*/
}

/**
AWindowRef���瓮�I�����E�C���h�E��ObjectID������
*/
AWindowID	AApplication::NVI_GetWindowIDByWindowRef( const AWindowRef inWindowRef ) const
{
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 ������
	for( AIndex index = 0; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		const AWindow&	window = mWindowArray.GetObjectConst(index);//#654
		if( /*#654 mWindowArray.GetObjectConst(index)*/window.NVI_IsWindowCreated() == false )   continue;//#199
		if( /*#654 mWindowArray.GetObjectConst(index)*/window.NVI_GetWindowRef() == inWindowRef )
		{
			return /*#654 mWindowArray.GetObjectConst(index)*/window.GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

/**
�w��WindowType�̍ŏ��̃E�C���h�E��ID���擾
*/
AWindowID	AApplication::NVI_GetFirstWindowID( const AWindowType inWinType ) const
{
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 ������
	for( AIndex index = 0; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		if( mWindowArray.GetObjectConst(index).NVI_GetWindowType() == inWinType )
		{
			return mWindowArray.GetObjectConst(index).GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

/**
�w��WindowID�̎��̃h�L�������g�iWindowType���������́j��ID���擾
*/
AWindowID	AApplication::NVI_GetNextWindowID( const AWindowID inWinType ) const
{
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 ������
	if( /*#654 mWindowArray.GetItemCount()*/windowCount == 0 )   return kObjectID_Invalid;
	AIndex	index = mWindowArray.GetIndexByID(inWinType);
	if( index == kIndex_Invalid )
	{
		_ACError("window not found",this);
		return kObjectID_Invalid;
	}
	AWindowType	winType = mWindowArray.GetObjectConst(index).NVI_GetWindowType();
	for( index++; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		if( mWindowArray.GetObjectConst(index).NVI_GetWindowType() == winType )
		{
			return mWindowArray.GetObjectConst(index).GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

/**
�S�ẴE�C���h�E��Refresh����
*/
void	AApplication::NVI_RefreshAllWindow()
{
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 ������
	for( AIndex index = 0; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		mWindowArray.GetObject(index).NVI_RefreshWindow();
	}
}

/**
�S�ẴE�C���h�E��update property����
*/
void	AApplication::NVI_UpdatePropertyAllWindow()
{
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 ������
	for( AIndex index = 0; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		mWindowArray.GetObject(index).NVI_UpdateProperty();
	}
}

//#379
/**
�e�E�C���h�E��Title�o�[�X�V
*/
void	AApplication::NVI_UpdateTitleBar()
{
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 ������
	for( AIndex index = 0; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		mWindowArray.GetObject(index).NVI_UpdateTitleBar();
	}
}

//#688
/**
���݂̃t�H�[�J�X�E�C���h�E�ikey window�j������
*/
AWindowID	AApplication::NVI_GetFocusWindowID() const
{
	AItemCount	windowCount = mWindowArray.GetItemCount();
	for( AIndex index = 0; index < windowCount; index++ )
	{
		const AWindow&	window = mWindowArray.GetObjectConst(index);
		if( window.NVI_IsWindowCreated() == false )   continue;
		if( window.NVI_IsWindowFocusActive() == true )
		{
			return window.GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

//#1075
/**
ViewID��View�����݂��Ă��邩�ǂ�����Ԃ�
*/
ABool	AApplication::NVI_ExistWindow( const AViewID inWindowID ) const
{
	return mWindowArray.ExistObject(inWindowID);
}

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//#175
/**
����ΏۃE�C���h�E�ɒǉ�
*/
void	AApplication::NVI_AddToWindowRotateArray( const AWindowID inWindowID )
{
	mWindowRotateArray.Insert1(0,inWindowID);
}

//#175
/**
����ΏۃE�C���h�E����폜
*/
void	AApplication::NVI_DeleteFromWindowRotateArray( const AWindowID inWindowID )
{
	AIndex	index = mWindowRotateArray.Find(inWindowID);
	if( index != kIndex_Invalid )
	{
		mWindowRotateArray.Delete1(index);
	}
	//#196 (#175�o�O�C��) �}���`�������ʃE�C���h�E������\��������ԂŃR�}���h+^��throw�������C��
	//mRotateActiveWindowID���폜�ΏۃE�C���h�E�Ȃ�AmRotateActiveWindowID��Invalid�ɂ���
	if( mRotateActiveWindowID == inWindowID )
	{
		mRotateActiveWindowID = kObjectID_Invalid;
	}
}

//#175
/**
Activate���ꂽ��R�[�������
����ΏۃE�C���h�E�Ȃ�A�E�C���h�E���񏇂��X�V����
*/
void	AApplication::NVI_RotateWindowActivated( const AWindowID inWindowID )
{
	if( mWindowRotating == true )   return;
	
	AIndex	index = mWindowRotateArray.Find(inWindowID);
	if( index != kIndex_Invalid )
	{
		mWindowRotateArray.Delete1(index);
		mWindowRotateArray.Insert1(0,inWindowID);
		mRotateActiveWindowID = inWindowID;
	}
}

//#175
/**
�E�C���h�E����
*/
void	AApplication::NVI_RotateWindow( const ABool inNext )
{
	if( mWindowRotateArray.GetItemCount() <= 1 )   return;
	
	if( inNext == true )
	{
		//�^�u����
		if( mRotateActiveWindowID != kObjectID_Invalid )
		{
			/*#559
			AItemCount	tabCount = NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetTabCount();
			AIndex	tabIndex = NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetCurrentTabIndex();
			if( tabIndex != kIndex_Invalid && tabIndex+1 < tabCount )
			{
				//���݂̃^�u���Ō�̃^�u�ȊO�Ȃ�A���̃^�u�ɐ؂�ւ��ă��W���[���I��
				//#379 �I���\�ȃ^�u������
				tabIndex++;
				while(tabIndex<tabCount)
				{
					if( NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetTabSelectable(tabIndex) == true )   break;
					tabIndex++;
				}
				if( tabIndex < tabCount )
				{
					//
					NVI_GetWindowByID(mRotateActiveWindowID).NVI_SelectTab(tabIndex);
					return;
				}
			}
			*/
			AIndex	tabIndex = NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetCurrentTabIndex();
			if( tabIndex != kIndex_Invalid )
			{
				if( NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetNextDisplayTab(tabIndex,false) == true )
				{
					NVI_GetWindowByID(mRotateActiveWindowID).NVI_SelectTab(tabIndex);
					return;
				}
			}
		}
		
		//���񏈗����t���OOn
		mWindowRotating = true;
		//
		AIndex	startindex = mWindowRotateArray.Find(mRotateActiveWindowID);
		if( startindex != kIndex_Invalid )
		{
			AIndex	index = startindex;
			//���̃E�C���h�E��
			index++;
			if( index >= mWindowRotateArray.GetItemCount() )
			{
				index = 0;
			}
			//mWindowRotateArray�̂Ȃ��ŁA�\�����̃E�C���h�E���T�[�`���Ă����A���������炻���I��
			for( ; index != startindex; )
			{
				//
				AWindowID	winID = mWindowRotateArray.Get(index);
				if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true && NVI_GetWindowByID(winID).NVI_IsWindowCollapsed() == false)
				{
					NVI_GetWindowByID(winID).NVI_SelectWindow();
					NVI_GetWindowByID(winID).NVI_SelectTab(NVI_GetWindowByID(winID).NVI_GetFirstDisplayTab());//#559 0);
					mRotateActiveWindowID = winID;
					break;
				}
				//���̃E�C���h�E��
				index++;
				if( index >= mWindowRotateArray.GetItemCount() )
				{
					index = 0;
				}
			}
			//#196 (#175�o�O�C��)
			//�\�����̃E�C���h�E�����ɖ��������ꍇ�A���݂̃E�C���h�E�̍ŏ��̃^�u�ɐ؂�ւ���
			if( index == startindex )
			{
				NVI_GetWindowByID(mRotateActiveWindowID).NVI_SelectTab(//#559 0);
							NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetFirstDisplayTab());
			}
		}
		//���񏈗����t���OOff
		mWindowRotating = false;
	}
	else//#552
	{
		//�^�u����
		if( mRotateActiveWindowID != kObjectID_Invalid )
		{
			AIndex	tabIndex = NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetCurrentTabIndex();
			if( tabIndex != kIndex_Invalid )
			{
				if( NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetPrevDisplayTab(tabIndex,false) == true )
				{
					NVI_GetWindowByID(mRotateActiveWindowID).NVI_SelectTab(tabIndex);
					return;
				}
			}
		}
		
		//���񏈗����t���OOn
		mWindowRotating = true;
		//
		AIndex	startindex = mWindowRotateArray.Find(mRotateActiveWindowID);
		if( startindex != kIndex_Invalid )
		{
			AIndex	index = startindex;
			//�O�̃E�C���h�E��
			index--;
			if( index < 0 )
			{
				index = mWindowRotateArray.GetItemCount()-1;
			}
			//mWindowRotateArray�̂Ȃ��ŁA�\�����̃E�C���h�E���T�[�`���Ă����A���������炻���I��
			for( ; index != startindex; )
			{
				//
				AWindowID	winID = mWindowRotateArray.Get(index);
				if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true && NVI_GetWindowByID(winID).NVI_IsWindowCollapsed() == false)
				{
					NVI_GetWindowByID(winID).NVI_SelectWindow();
					NVI_GetWindowByID(winID).NVI_SelectTab(NVI_GetWindowByID(winID).NVI_GetLastDisplayTab());
					mRotateActiveWindowID = winID;
					break;
				}
				//�O�̃E�C���h�E��
				index--;
				if( index < 0 )
				{
					index = mWindowRotateArray.GetItemCount()-1;
				}
			}
			//#196 (#175�o�O�C��)
			//�\�����̃E�C���h�E�����ɖ��������ꍇ�A���݂̃E�C���h�E�̍Ō�̃^�u�ɐ؂�ւ���
			if( index == startindex )
			{
				NVI_GetWindowByID(mRotateActiveWindowID).NVI_SelectTab(
							NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetLastDisplayTab());
			}
		}
		//���񏈗����t���OOff
		mWindowRotating = false;
	}
}

#pragma mark ===========================

#pragma mark --- AllClose
//#199

/**
AllClose�ΏۃE�C���h�E��ǉ�
*/
void	AApplication::NVI_AddToAllCloseTargetWindowArray( const AWindowID inWindowID )
{
	mAllCloseTargetWindowArray.Add(inWindowID);
}

/**
AllClose�ΏۃE�C���h�E���폜
*/
void	AApplication::NVI_DeleteFromAllCloseTargetWindowArray( const AWindowID inWindowID )
{
	AIndex	index = mAllCloseTargetWindowArray.Find(inWindowID);
	if( index == kIndex_Invalid )   return;
	
	mAllCloseTargetWindowArray.Delete1(index);
}

/**
�S�ẴE�C���h�E�����B
�܂��A�S�ẴE�C���h�E�ɂ��āAdirty�ȃE�C���h�E�^�^�u�ɂ��āA�ۑ��^�ۑ����Ȃ��^�L�����Z���m�F�_�C�A���O��\�����A�L�����Z�����ꂽ��u�P�������Ɂv�I��
*/
ABool	AApplication::NVI_CloseAllWindow()
{
	//�ŏ���winArray�֑ΏۃE�C���h�E�̔z����R�s�[
	AArray<AWindowID>	winArray;
	for( AIndex i = 0; i < mAllCloseTargetWindowArray.GetItemCount(); i++ )
	{
		winArray.Add(mAllCloseTargetWindowArray.Get(i));
	}
	//�R�s�[�����z��ɂ��āA�e�E�C���h�E�ɂ��āA�ۑ��^�ۑ����Ȃ��^�L�����Z���m�F�_�C�A���O��\������
	for( AIndex i = 0; i < winArray.GetItemCount(); i++ )
	{
		//�ۑ��^�ۑ����Ȃ��^�L�����Z���m�F�_�C�A���O��\��
		//�_�C�A���O�̓��[�_��
		//�ۑ��̏ꍇ�F�ۑ����s�B�Ԃ�l��true
		//�ۑ����Ȃ��ꍇ�F�������Ȃ��B�Ԃ�l��true
		//�L�����Z���̏ꍇ�F�������Ȃ��B�Ԃ�l��false
		if( NVI_GetWindowByID(winArray.Get(i)).NVI_AskSaveForAllTabs() == false )
		{
			//�L�����Z�����ꂽ�炻���ŏI��
			return false;
		}
	}
	//�ۑ��^�ۑ����Ȃ��^�L�����Z���m�F�_�C�A���O�ŃL�����Z�����Ȃ���΁A�E�C���h�E��S�ċ����N���[�Y
	for( AIndex i = 0; i < winArray.GetItemCount(); i++ )
	{
		//�E�C���h�E�폜���s�i�����N���[�Y�j
		NVI_GetWindowByID(winArray.Get(i)).NVI_ExecuteClose();
	}
	return true;
}

#pragma mark ===========================

#pragma mark ---�^�C�}�[����
//#138

/**
�^�C�}�[�����ΏۃE�C���h�E��ǉ�
*/
void	AApplication::NVI_AddToTimerActionWindowArray( const AWindowID inWindowID )
{
	mTimerActionWindowArray.Add(inWindowID);
}

/**
�^�C�}�[�����ΏۃE�C���h�E���폜
*/
void	AApplication::NVI_DeleteFromTimerActionWindowArray( const AWindowID inWindowID )
{
	AIndex	index = mTimerActionWindowArray.Find(inWindowID);
	if( index == kIndex_Invalid )   return;
	
	mTimerActionWindowArray.Delete1(index);
}

/**
�^�C�}�[����
*/
void	AApplication::EVT_DoTickTimerAction()
{
	for( AIndex i = 0; i < mTimerActionWindowArray.GetItemCount(); i++ )
	{
		AWindow&	win = NVI_GetWindowByID(mTimerActionWindowArray.Get(i));
		if( win.NVI_IsWindowCreated() == true )//#402
		{
			if( win.NVI_IsWindowActive() == true || win.NVI_IsFloating() == true )
			{
				win.EVT_DoTickTimer();
			}
		}
	}
	EVTDO_DoTickTimerAction();
}

#pragma mark ===========================

#pragma mark <���L�N���X(AView)�����^�폜�^�擾>
//#199
#pragma mark ===========================

#pragma mark ---�r���[�����^�폜

/**
���I�����E�C���h�E�̐���
*/
AViewID	AApplication::NVI_CreateView( AViewFactory& inFactory )
{
	AIndex	viewIndex = mViewArray.AddNewObject(inFactory);
	mViewUniqIDArray.AddItem();//#693
	return mViewArray.GetObject(viewIndex).GetObjectID();
}

/**
���I�����E�C���h�E�̍폜
*/
void	AApplication::NVI_DeleteView( const AViewID inViewID )
{
	AIndex	viewIndex = mViewArray.GetIndexByID(inViewID);
	mViewArray.Delete1Object(viewIndex);
	mViewUniqIDArray.Delete1Item(viewIndex);//#693
}

#pragma mark ===========================

#pragma mark ---�r���[�擾

/**
ViewID����E�C���h�E�擾
*/
AView&	AApplication::NVI_GetViewByID( const AViewID inViewID )
{
	/*#272
	AIndex	viewIndex = mViewArray.GetIndexByID(inViewID);
	return mViewArray.GetObject(viewIndex);
	*/
	return mViewArray.GetObjectByID(inViewID);
}

/**
ViewID����E�C���h�E�擾
*/
const AView&	AApplication::NVI_GetViewConstByID( const AViewID inViewID ) const
{
	/*#272
	AIndex	viewIndex = mViewArray.GetIndexByID(inViewID);
	return mViewArray.GetObjectConst(viewIndex);
	*/
	return mViewArray.GetObjectConstByID(inViewID);
}

/**
ViewID��View�����݂��Ă��邩�ǂ�����Ԃ�
*/
ABool	AApplication::NVI_ExistView( const AViewID inViewID ) const
{
	/*#272
	return (mViewArray.GetIndexByID(inViewID)!=kIndex_Invalid);
	*/
	return mViewArray.ExistObject(inViewID);
}

//#693
/**
�r���[UniqID�擾
*/
AUniqID	AApplication::NVI_GetViewUniqID( const AViewID inViewID ) const
{
	return mViewUniqIDArray.Get(mViewArray.GetIndexByID(inViewID));
}

//#693
/**
�r���[UniqID����ViewID(ObjectID)���擾
*/
AViewID	AApplication::NVI_GetViewIDByUniqID( const AUniqID inDocUniqID ) const
{
	AIndex	index = mViewUniqIDArray.Find(inDocUniqID);
	if( index == kIndex_Invalid )   return kObjectID_Invalid;
	return mViewArray.GetObjectConst(index).GetObjectID();
}

#pragma mark ===========================

#pragma mark <���L�N���X(AThread)�����^�폜�^�擾>

#pragma mark ===========================

#pragma mark ---�X���b�h�����^�폜

/**
�X���b�h����
*/
AObjectID	AApplication::NVI_CreateThread( AThreadFactory& inFactory )
{
	AIndex	index = mThreadArray.AddNewObject(inFactory);
	return mThreadArray.GetObject(index).GetObjectID();
}

//#402
/**
�X���b�h�폜
*/
void	AApplication::NVI_DeleteThread( const AObjectID inThreadID, const ABool inWaitAbortThread )
{
	if( inWaitAbortThread == true )
	{
		//�X���b�h��~
		mThreadArray.GetObjectByID(inThreadID).NVI_WaitAbortThread();
	}
	//�X���b�h�I�u�W�F�N�g�폜
	AIndex	index = mThreadArray.GetIndexByID(inThreadID);
	mThreadArray.Delete1Object(index);
}

/**
�X���b�h�擾
*/
AThread&	AApplication::NVI_GetThreadByID( const AObjectID inThreadID )
{
	/*#272
	AIndex	index = mThreadArray.GetIndexByID(inThreadID);
	return mThreadArray.GetObject(index);
	*/
	return mThreadArray.GetObjectByID(inThreadID);
}
/**
�X���b�h�擾(const)
*/
const AThread&	AApplication::NVI_GetThreadByIDConst( const AObjectID inThreadID ) const
{
	/*#272
	AIndex	index = mThreadArray.GetIndexByID(inThreadID);
	return mThreadArray.GetObjectConst(index);
	*/
	return mThreadArray.GetObjectConstByID(inThreadID);
}

/**
�X���b�h�N��
*/
void	AApplication::NVI_RunThread( const AObjectID inThreadID )
{
	NVI_GetThreadByID(inThreadID).NVI_Run();
}

//#699
/*
AObjectID	AApplication::NVI_GetCurrentThreadObjectID() const
{
	AItemCount	itemCount = mThreadArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		if( mThreadArray.GetObjectConst(i).NVI_GetThraedID() == AThreadWrapper::GetCurrentThreadID() )
		{
			return mThreadArray.GetObjectConst(i).GetObjectID();
		}
	}
	return kObjectID_Invalid;
}
*/
#pragma mark ===========================

#pragma mark ---Speech
//#851

/**
�e�L�X�g�ǂݏグ�J�n
*/
void	AApplication::NVI_SpeakText( const AText& inText )
{
	mAppImp.SpeakText(inText);
}

/**
�e�L�X�g�ǂݏグ��~
*/
void	AApplication::NVI_StopSpeaking()
{
	mAppImp.StopSpeaking();
}

/**
�e�L�X�g�ǂݏグ�����ǂ�����Ԃ�
*/
ABool	AApplication::NVI_IsSpeaking() const
{
	return mAppImp.IsSpeaking();
}

#pragma mark ===========================

#pragma mark ---Lua
//#567

/**
Lua�t�@�C�����s
*/
int	AApplication::NVI_Lua_dofile( const AFileAcc& inFile, AText& outErrorText )
{
	AText	path;
	inFile.GetPath(path);
	AStCreateCstringFromAText	pathcstr(path);
	int	result = luaL_dofile(mLuaVM,pathcstr.GetPtr());
	if( result != 0 )
	{
		Lua_PopText(mLuaVM,outErrorText);
	}
	return result;
}

/**
Lua�e�L�X�g���s
*/
int	AApplication::NVI_Lua_dostring( const AText& inString, AText& outErrorText )
{
	AStCreateCstringFromAText	cstr(inString);
	int	result = luaL_dostring(mLuaVM,cstr.GetPtr());
	if( result != 0 )
	{
		Lua_PopText(mLuaVM,outErrorText);
	}
	return result;
}

/**
Lua�G���[�����ilongjmp�����j
*/
void	AApplication::Lua_Error( lua_State* L, const char* inStringPtr )
{
	luaL_error(L,inStringPtr);
}

/**
Lua�h�L�������g���݃`�F�b�N
*/
void	AApplication::Lua_CheckExistDocument( lua_State* L, const ADocumentID inDocID, const ADocumentType inType ) const
{
	if( NVI_IsDocumentValid(inDocID) == false )
	{
		Lua_Error(L,"(AB001) bad object id (document)");
	}
	if( NVI_GetDocumentTypeByID(inDocID) != inType )
	{
		Lua_Error(L,"(AB001) bad object id (document)");
	}
}

/**
Lua�r���[���݃`�F�b�N
*/
void	AApplication::Lua_CheckExistView( lua_State* L, const AViewID inViewID, const AViewType inType ) const
{
	if( NVI_ExistView(inViewID) == false )
	{
		Lua_Error(L,"(AB002) bad object id (view)");
	}
	if( NVI_GetViewConstByID(inViewID).NVI_GetViewType() != inType )
	{
		Lua_Error(L,"(AB002) bad object id (view)");
	}
}

#pragma mark ===========================

#pragma mark ---Lua�X�^�b�N����

/**
�X�^�b�N�v�f���擾
*/
AItemCount	AApplication::Lua_GetStackCount( lua_State* L )
{
	return lua_gettop(L);
}

/**
�X�^�b�N��S�ăN���A
*/
void	AApplication::Lua_ClearStack( lua_State* L )
{
	lua_settop(L,0);
}

#pragma mark ===========================

#pragma mark ---Lua�X�^�b�N����i�����`�F�b�N�E�G���[�����L��j

/**
Push(integer)
*/
void	AApplication::Lua_PushInteger( lua_State* L, const lua_Integer inInteger )
{
	lua_pushinteger(L,inInteger);
}

/**
Push(number)
*/
void	AApplication::Lua_PushNumber( lua_State* L, const lua_Number inNumber )
{
	lua_pushnumber(L,inNumber);
}

/**
Push(text)
*/
void	AApplication::Lua_PushText( lua_State* L, const AText& inText )
{
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	luaL_Buffer	b;
	luaL_buffinit(L,&b);
	luaL_addlstring(&b,(const char*)textptr.GetPtr(),textptr.GetByteCount());
	luaL_pushresult(&b);
}

/**
Push(table)
*/
void	AApplication::Lua_PushNewTable( lua_State* L )
{
	lua_newtable(L);
}

/**
�e�[�u���̃t�B�[���h�ɐݒ�(integer)
*/
void	AApplication::Lua_SetTableFieldInteger( lua_State* L, const char* inFieldName, const lua_Integer inValue )
{
	lua_pushinteger(L,inValue);
	lua_setfield(L,-2,inFieldName);
}

/**
Pop(text)
*/
void	AApplication::Lua_PopText( lua_State* L, AText& outText )
{
	size_t	len = 0;
	AConstUCharPtr	textCstr = (AConstUCharPtr)(lua_tolstring(L,-1,&len));
	outText.SetFromTextPtr(textCstr,len);
	lua_pop(L,1);
}

/**
�����擾(integer)
*/
lua_Integer	AApplication::Lua_PopArgumentInteger( lua_State* L )
{
	lua_Integer	result = luaL_checkinteger(L,-1);
	lua_pop(L,1);
	return result;
}

/**
�����擾(text)
*/
void	AApplication::Lua_PopArgumentText( lua_State* L, AText& outText )
{
	size_t	len = 0;
	AConstUCharPtr	textCstr = (AConstUCharPtr)(luaL_checklstring(L,-1,&len));
	outText.SetFromTextPtr(textCstr,len);
	lua_pop(L,1);
}

/**
�����擾(UniqID)
*/
AUniqID	AApplication::Lua_PopArgumentUniqID( lua_State* L )//#693
{
	AUniqID	result = kUniqID_Invalid;
	result.val = Lua_PopArgumentInteger(L);
	if( result.val == -1 )
	{
		result = kUniqID_Invalid;
	}
	return result;
}

/**
�����I���`�F�b�N�i���ɃX�^�b�N����ł��邱�Ƃ��`�F�b�N�j
*/
void	AApplication::Lua_CheckArgumentEnd( lua_State* L )
{
	if( Lua_GetStackCount(L) != 0 )
	{
		luaL_error(L,"too many arguments");
	}
}

#pragma mark ===========================

#pragma mark ---JavaScript
//#904

/**
JavaScript���s
*/
ABool	AApplication::NVI_DoJavaScript( const AText& inScript, const AFileAcc& inFolder )
{
	//JavaScript���s
	mCurrentJavaScriptFolder = inFolder;
	GetImp().DoJavaScript(inScript);
	mCurrentJavaScriptFolder.Unspecify();
	return true;
}

/*NSOperationQueue�g�p�e�X�g
ABool	AApplication::NVI_IsExecutingJavaScript()
{
	return GetImp().IsExecutingJavaScript();
}

void	AApplication::NVI_CancelExecutingJavaScript()
{
	GetImp().CancelExecutingJavaScript();
}
*/

/**
JavaScript���W���[�����[�h
*/
void	AApplication::NVI_LoadJavaScriptModule( const AText& inModuleName )
{
	//���ݎ��s����JavaScript�t�@�C���̃t�H���_���́AinModuleName+.js���t�@�C�����Ƃ���t�@�C����ǂݍ���Ŏ��s����
	AText	modulename;
	modulename.SetText(inModuleName);
	AFileAcc	file;
	file.SpecifyChild(mCurrentJavaScriptFolder,modulename);
	if( file.Exist() == false )
	{
		modulename.AddCstring(".js");
		file.SpecifyChild(mCurrentJavaScriptFolder,modulename);
	}
	AText	script;
	file.ReadTo(script);
	GetImp().DoJavaScript(script);
}

#pragma mark ===========================

#pragma mark ---�v���I�G���[�������̃f�[�^�o�b�N�A�b�v
//#182

/**
�v���I�G���[�������̃f�[�^�o�b�N�A�b�v
*/
void	BackupDataForCriticalError()
{
	try
	{
		AApplication::GetApplication().NVI_BackupDataForCriticalError();
	}
	catch(...)
	{
	}
}

#pragma mark ===========================

#pragma mark ---�C�x���gtransaction�I��������
//#182

/**
�C�x���gtransaction�I��������
*/
void	EventTransactionPostProcess( const ABool inPeriodical )
{
	try
	{
		AApplication::GetApplication().NVI_EventTransactionPostProcess(inPeriodical);
	}
	catch(...)
	{
	}
}

