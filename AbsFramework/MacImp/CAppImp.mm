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

CAppImp.cpp

*/

#include "../Imp.h"
#include "CAppleEvent.h"
#include "AApplication.h"
#import <Cocoa/Cocoa.h>
#include "../AbsBase/Cocoa.h"
#import "AppDelegate.h"
#include "../../AbsFramework/MacWrapper/ODBEditorSuite.h"

//#688 void	InitCocoa();

//AppleScript
ComponentInstance	CAppImp::sScriptingComponent;

#if SUPPORT_CARBON
//NibRef
IBNibRef	CAppImp::sNibRef = NULL;
IBNibRef	CAppImp::sMenuNibRef = NULL;//#0
#endif

//#263
//�R�}���h����
int							CAppImp::sArgc = 0;
const char**				CAppImp::sArgv = NULL;



#pragma mark ===========================
#pragma mark [�N���X]CAppImp
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

/**
�R���X�g���N�^
*/
CAppImp::CAppImp():AObjectArrayItem(NULL)
{
#if SUPPORT_CARBON
	sMouseTrackingMode = false;//B0000 080810
	
	::RegisterAppearanceClient();
	InstallEventHandler();
#endif
	/*#688
	//R0199
	//R0199 OSX10.3�ł͎g�p�s�\�̂悤���i�t�@�C�����X�g�E�C���h�E���N���b�N���Ă��t�@�C�����J���Ȃ��A�e�L�X�g�E�C���h�E�N���b�N���Ă��L�����b�g�ݒ肳��Ȃ��Ȃǁj
	//��deploy target�ݒ��10.3�ɂ��邱�Ƃ��K�v�����������ł����B080802
	//�ł��A10.3�ł́A�r������X�y���`�F�b�N�������Ȃ��Ȃ����肷��̂ł���ς萧��
	//R0199 10.4�ȏ�Ȃ�Cocoa�g�p����悤�ɂ��܂����B
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
	{
		InitCocoa();
	}
	*/
	CWindowImp::sAliveWindowArray.SetThrowIfOutOfBounds();//B0000
	//#305
	//���݂̌�����擾
	mLanguage = kLanguage_Japanese;
	CFMutableArrayRef	locArray = ::CFArrayCreateMutable(kCFAllocatorSystemDefault, 0, &kCFTypeArrayCallBacks);
	if( locArray != NULL )
	{
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"ja",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"en",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"fr",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"de",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"es",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"it",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"nl",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"sv",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"nb",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"da",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"fi",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"pt",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"zh_CN",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"zh_TW",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"ko",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"pl",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"pt_PT",kCFStringEncodingUTF8));
		::CFArrayAppendValue(locArray,CFStringCreateWithCString(NULL,"ru",kCFStringEncodingUTF8));
		CFArrayRef	preferLangArray = ::CFBundleCopyPreferredLocalizationsFromArray(locArray);
		if( preferLangArray != NULL )
		{
			if( ::CFArrayGetCount(preferLangArray) != 0 )
			{
				CFStringRef	preferLang = (CFStringRef)::CFArrayGetValueAtIndex(preferLangArray,0);
				if( ::CFStringCompare(preferLang,CFSTR("ja"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_Japanese;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("en"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_English;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("fr"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_French;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("de"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_German;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("es"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_Spanish;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("it"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_Italian;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("nl"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_Dutch;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("sv"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_Swedish;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("nb"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_Norwegian;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("da"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_Danish;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("fi"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_Finnish;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("pt"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_Portuguese;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("zh_CN"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_SimplifiedChinese;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("zh_TW"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_TraditionalChinese;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("ko"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_Korean;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("pl"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_Polish;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("pt_PT"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_PortuguesePortugal;
				}
				else if( ::CFStringCompare(preferLang,CFSTR("ru"),0) == kCFCompareEqualTo )
				{
					mLanguage = kLanguage_Russian;
				}
			}
			::CFRelease(preferLangArray);
		}
		::CFRelease(locArray);
	}
	//�A�v���P�[�V�����o�[�W�����ǂݍ���#645
	CFBundleRef	appBundle = ::CFBundleGetMainBundle();
	CFStringRef	str = (CFStringRef)::CFBundleGetValueForInfoDictionaryKey(appBundle,CFSTR("CFBundleShortVersionString"));//#1096
	if( str != NULL )
	{
		mApplicationVersionText.SetFromCFString(str);
	}
	//#1096
	str = (CFStringRef)::CFBundleGetValueForInfoDictionaryKey(appBundle,CFSTR("CFBundleVersion"));
	if( str != NULL )
	{
		mApplicationBuildNumberText.SetFromCFString(str);
	}
	//#557
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
	{
		system("defaults write net.mimikaki.mi ApplePersistenceIgnoreState YES");
	}
	//#1034
	//�ۑ��_�C�A���O�́u�g���q���B���v��OFF�ɂ���i��OS�ł͂��̐ݒ肪�K�v���ۂ��j
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setInteger:0 forKey:@"NSNavLastUserSetHideExtensionButtonState"];
	[defaults synchronize];
	
	//#852
	//�t�H���g�f�[�^������
	AFontWrapper::Init();
	/*#1034
	//�f�t�H���gATSUStyle�擾
	::ATSUCreateStyle(&sDefaultATSUStyle);
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);
	CWindowImp::SetATSUStyle(sDefaultATSUStyle,fontname,9.0,true,kColor_Black,1.0,kTextStyle_Normal);
	*/
	
	//#703
	//Font Fallback
	/* #1005
	OSStatus	err = ::ATSUCreateFontFallbacks(&sFontFallbacks);
	if( err == noErr )
	{
		//�w��t�H���g�����Ńt�H���g�t�H�[���o�b�N��ݒ肷��
		ATSUFontID	fonts[2];
		AItemCount	fontIDCount = 0;
		//ATSUFontID	fontID = 0;
		*test
		fontname.SetCstring("Monaco");
		if( AFontWrapper::GetATSUFontIDByName(fontname,fontID) == true )
		{
			fonts[fontIDCount] = fontID;
			fontIDCount++;
		}
		fontname.SetCstring("Osaka");
		if( AFontWrapper::GetATSUFontIDByName(fontname,fontID) == true )
		{
			fonts[fontIDCount] = fontID;
			fontIDCount++;
		}
		*
		err = ::ATSUSetObjFontFallbacks(sFontFallbacks,fontIDCount,fonts,kATSUSequentialFallbacksPreferred);//kATSUSequentialFallbacksExclusive);
		if( err != noErr ) {_ACError("",NULL);}
	}
	else
	{
		_ACError("",NULL);
	}
	*/
}

//#891
//�h�b�g�P�ʃX�N���[��
ABool	CAppImp::sUseWheelScrollFloatValue = true;

//#852
//#1034 ATSUStyle	CAppImp::sDefaultATSUStyle = NULL;

//#703
//Font Fallback
ATSUFontFallbacks	CAppImp::sFontFallbacks = NULL;

//#852
/**
�f�t�H���gASTSUStyle�擾
*/
/*#1034
ATSUStyle	CAppImp::GetDefaultATSUStyle()
{
	return sDefaultATSUStyle;
}
*/

//���ݕ\������Context menu�̑ΏۃE�C���h�E�^�R���g���[��
AWindowID	CAppImp::sContextMenuWindowID = kObjectID_Invalid;
AControlID	CAppImp::sContextMenuViewControlID = kControlID_Invalid;

/**
���ݕ\������Context menu�̑ΏۃE�C���h�E�^�R���g���[�����L��
*/
void	CAppImp::SetContextMenuTarget( const AWindowID inWindowID, const AControlID inControlID )
{
	sContextMenuWindowID = inWindowID;
	sContextMenuViewControlID = inControlID;
}

#if SUPPORT_CARBON
/**
"main"��NibRef���擾
*/
IBNibRef	CAppImp::GetNibRef()
{
	OSStatus	err = noErr;
	
	if( sNibRef == NULL )
	{
		err = ::CreateNibReference(CFSTR("main"),&sNibRef);
		if( err != noErr )   _ACErrorNum("CreateNibReference() returned error: ",err,NULL);
	}
	return sNibRef;
}

//#0
/**
"menu"��NibRef���擾
*/
IBNibRef	CAppImp::GetMenuNibRef()
{
	OSStatus	err = noErr;
	
	if( sMenuNibRef == NULL )
	{
		err = ::CreateNibReference(CFSTR("menu"),&sMenuNibRef);
	}
	return sMenuNibRef;
}
#endif

//#263
/**
�R�}���h���C��������ݒ�
*/
void	CAppImp::Setup( int argc, const char **argv )
{
	sArgc = argc;
	sArgv = argv;
}

/**
�R�}���h���C��������TextArray���擾
*/
void	CAppImp::GetCmdLineArgTextArray( ATextArray& outTextArray )
{
	for( AIndex i = 1; i < sArgc; i++ )
	{
		AText	text;
		text.SetCstring(sArgv[i]);
		outTextArray.Add(text);
	}
}


#pragma mark ===========================

#pragma mark ---Run/Quit

/**
�A�v�����s
*/
int	CAppImp::Run()
{
	OSStatus	err = noErr;
	
	sScriptingComponent = ::OpenDefaultComponent(kOSAComponentType,kAppleScriptSubtype);
	/*#1034
	err = ::HMSetTagDelay(500);//#644
	*/
	if( err != noErr )   _ACErrorNum("HMSetTagDelay() returned error: ",err,NULL);
	StartTimer();//B0000
	//#688 main()�ɂ�NSApplicationMain()�R�[���ɂ��A���C�����[�v���s�ς� ::RunApplicationEventLoop();
	return 0;
}

/**
�A�v���I��
*/
void	CAppImp::Quit()
{
	//#688 ::QuitApplicationEventLoop();
	[NSApp terminate:nil];//#668 NSApp�i�A�v���P�[�V�����I�u�W�F�N�g�j��terminate���M�isender��nil�Ƃ���j
}

#pragma mark ===========================

#pragma mark ---�W������
//#688

/**
���̃A�v���P�[�V�������B��
*/
void	CAppImp::HideThisApplication()
{
	[NSApp hide:nil];
}

/**
���̃A�v���P�[�V�������B��
*/
void	CAppImp::HideOtherApplications()
{
	[NSApp hideOtherApplications:nil];
}

/**
�S�ẴA�v���P�[�V������\��
*/
void	CAppImp::ShowAllApplications()
{
	[NSApp unhideAllApplications:nil];
}

//#688
/**
�S�Ă���O�ɕ\��
*/
void	CAppImp::BringAllToFront()
{
	[NSApp arrangeInFront:nil];
}

#pragma mark ===========================

#pragma mark ---Speech
//#851

/**
�e�L�X�g�ǂݏグ�J�n
*/
void	CAppImp::SpeakText( const AText& inText )
{
	AppDelegate*	appdelegate = (AppDelegate*)[NSApp delegate];
	AStCreateNSStringFromAText	str(inText);
	[appdelegate speakText:str.GetNSString()];
}

/**
�e�L�X�g�ǂݏグ��~
*/
void	CAppImp::StopSpeaking()
{
	AppDelegate*	appdelegate = (AppDelegate*)[NSApp delegate];
	[appdelegate stopSpeaking];
}

/**
�e�L�X�g�ǂݏグ�����ǂ�����Ԃ�
*/
ABool	CAppImp::IsSpeaking() const
{
	AppDelegate*	appdelegate = (AppDelegate*)[NSApp delegate];
	return ([appdelegate isSpeaking] == YES);
}

#pragma mark ===========================

#pragma mark ---�C�x���g�n���h��

//#688
/**
���j���[���ڑI��������
*/
void	CAppImp::APICB_CocoaDoMenuAction( const AMenuItemID inMenuItemID, const AModifierKeys inModifierKeys,
		const AMenuRef inMenuRef, const AIndex inMenuItemIndex )
{
	ABool	handled = false;
	
	//���j���[���ڂɑΉ�����action text�擾
	AText	actiontext;
	AApplication::GetApplication().NVI_GetMenuManager().GetMenuActionText(inMenuItemID,
				inMenuRef,inMenuItemIndex,actiontext);
	
	//context menu�\�����Ȃ�A���̑ΏۃE�C���h�E�^�R���g���[���ŏ���������
	if( sContextMenuWindowID != kObjectID_Invalid )
	{
		//�ΏۃR���g���[���Ŏ��s
		if( AApplication::GetApplication().NVI_GetWindowByID(sContextMenuWindowID).
					NVI_GetViewByControlID(sContextMenuViewControlID).
					EVT_DoMenuItemSelected(inMenuItemID,actiontext,inModifierKeys) == true )
		{
			handled = true;
		}
		//�ΏۃE�C���h�E�Ŏ��s
		if( handled == false )
		{
			if( AApplication::GetApplication().NVI_GetWindowByID(sContextMenuWindowID).
						EVT_DoMenuItemSelected(inMenuItemID,actiontext,inModifierKeys) == true )
			{
				handled = true;
			}
		}
	}
	
	if( handled == false )
	{
		//�t�H�[�J�X�E�C���h�E��EVT_DoMenuItemSelected()���s
		AWindowID	winID = AApplication::GetApplication().NVI_GetFocusWindowID();
		if( winID != kObjectID_Invalid )
		{
			if( AApplication::GetApplication().NVI_GetWindowByID(winID).
						EVT_DoMenuItemSelected(inMenuItemID,actiontext,inModifierKeys) == true )
			{
				handled = true;
			}
		}
	}
	//�E�C���h�E�Ŏ��s����Ȃ������ꍇ�A�A�v���P�[�V������EVT_DoMenuItemSelected()���s
	if( handled == false )
	{
		if( AApplication::GetApplication().
					EVT_DoMenuItemSelected(inMenuItemID,actiontext,inModifierKeys) == true )
		{
			handled = true;
		}
	}
	
	//NVI_UpdateMenu()�̓R�[������doMenuActions:�ɂď�Ɏ��s
}

/**
Tick�^�C�}�[�A�N�V��������
*/
void	CAppImp::APICB_CocoaDoTickTimerAction()
{
	//AApplication::EVT_DoTickTimerAction()���s
	AApplication::GetApplication().EVT_DoTickTimerAction();
}

/**
�A�v���P�[�V����activate������
*/
void	CAppImp::APICB_CocoaDoBecomeActive()
{
	//AApplication::EVT_DoAppActivated()���s
	AApplication::GetApplication().EVT_DoAppActivated();
}

/**
�A�v���P�[�V����deactivate������
*/
void	CAppImp::APICB_CocoaDoResignActive()
{
	//AApplication::EVT_DoAppDeactivated()���s
	AApplication::GetApplication().EVT_DoAppDeactivated();
}

#if SUPPORT_CARBON
/**
�A�v����Deactivated���ꂽ�Ƃ��̃C�x���g�n���h��
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoAppDeactivated( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-AD)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		AApplication::GetApplication().EVT_DoAppDeactivated();//B0442
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoAppDeactivated() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-AD]");
	return noErr;
}

/**
�A�v����Activated���ꂽ�Ƃ��̃C�x���g�n���h��
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoAppActivated( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-AA)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		AApplication::GetApplication().EVT_DoAppActivated();
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoAppActivated() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-AA]");
	return noErr;
}

/**
�R�}���h�C�x���g�n���h��
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoCommand( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-C)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	
	OSStatus	result = eventNotHandledErr;
	
	//�R�}���h�擾
	HICommand	command = {0,0};
	err = ::GetEventParameter(inEventRef,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&command);
	//�L�[�擾
	UInt32	modifiers = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	
	try
	{
		AText	actiontext;
		AApplication::GetApplication().NVI_GetMenuManager().GetMenuActionText(command.commandID,command.menu.menuRef,command.menu.menuItemIndex-1,actiontext);
		//kCommandID_UpdateCommandStatus�Ȃ烁�j���[�X�V
		if( command.commandID == kCommandID_UpdateCommandStatus )
		{
			AApplication::GetApplication().EVT_UpdateMenu();
		}
		//
		else if( AApplication::GetApplication().EVT_DoMenuItemSelected(command.commandID,actiontext,modifiers) == true )
		{
			result = noErr;
			if( command.commandID != kCommandID_UpdateCommandStatus )
			{
				AApplication::GetApplication().NVI_UpdateMenu();
			}
		}
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoCommand() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-C]");
	return result;
}

/**
�R�}���h���Update�C�x���g�n���h��
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoCommandUpdateStatus( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-CUS)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		//��������
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoCommandUpdateStatus() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-CUS]");
	return eventNotHandledErr;
}

/**
�L�[�����C�x���g�n���h��
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoKeyDown( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-KD)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		//��������
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoKeyDown() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-KD]");
	return eventNotHandledErr;
}

/**
�e�L�X�g���̓C�x���g�n���h��
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoTextInput( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-TI)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		//��������
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoTextInput() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-TI]");
	return eventNotHandledErr;
}

/**
�}�E�X�ړ��C�x���g�n���h��
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoMouseMoved( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-MM)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(true);
	
	ABool	result = eventNotHandledErr;
	try
	{
		if( AApplication::GetApplication().EVT_DoMouseMoved() == true )
		{
			result = noErr;
		}
		//B0000 080810
		//STATIC_APICB_DoMouseMoved()�́AWindow��mouse moved�C�x���g����������Ȃ������ꍇ�ɃR�[�������
		if( sMouseTrackingMode == true )
		{
			if( CWindowImp::ExistWindowImpForWindowRef(sMouseTrackingWindowRef) == true )
			{
				CWindowImp::GetWindowImpByWindowRef(sMouseTrackingWindowRef).DoMouseExited(sMouseTrackingControlID,inEventRef);//R0228
			}
			sMouseTrackingMode = false;
		}
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoMouseMoved() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-MM]");
	return eventNotHandledErr;//result�ł�cursorchange�C�x���g�����Ȃ��H
}

/**
AppleEvent�C�x���g�n���h��
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoAppleEvent( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-AE)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		//http://developer.apple.com/documentation/AppleScript/Conceptual/AppleEvents/dispatch_aes_aepg/chapter_4_section_3.html
		
		Boolean     release = false;
		EventRecord eventRecord;
		OSErr       ignoreErrForThisSample;
		
		// Events of type kEventAppleEvent must be removed from the queue
		//  before being passed to AEProcessAppleEvent.
		if (IsEventInQueue(GetMainEventQueue(), inEventRef))
		{
			// RemoveEventFromQueue will release the event, which will
			//  destroy it if we don't retain it first.
			RetainEvent(inEventRef);
			release = true;
			RemoveEventFromQueue(GetMainEventQueue(), inEventRef);
		}
		
		// Convert the event ref to the type AEProcessAppleEvent expects.
		ConvertEventRefToEventRecord(inEventRef, &eventRecord);
		ignoreErrForThisSample = AEProcessAppleEvent(&eventRecord);
		
		if (release)
		ReleaseEvent(inEventRef);
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoAppleEvent() caught exception.",NULL);//#199
	}
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-AE]");
	// This Carbon event has been handled, even if no AppleEvent handlers
	//  were installed for the Apple event.
	return noErr;
}

/*pascal OSErr	CAppImp::STATIC_APICB_DoAEOpenDocument( const AppleEvent* inAppleEvent, AppleEvent* outReply, long inRefcon )
{
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(true);
	try
	{
		CAppleEventReader	ae(inAppleEvent);
		GetApp().EVT_DoAEOpenDocument(ae);
	}
	catch(...)
	{
	}
	return noErr;
}
*/

/**
�����C�x���g�C�x���g�n���h��
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoInternalEvent( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-IE)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		ABool	updateMenu = false;//#698
		AApplication::GetApplication().EVT_DoInternalEvent(updateMenu);//#698
		if( updateMenu == true )//#698
		{
			AApplication::GetApplication().NVI_UpdateMenu();
		}
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoInternalEvent() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-IE]");
	return noErr;
}

/**
Timer�C�x���g�n���h��
*/
pascal void	CAppImp::STATIC_APICB_DoTickTimerAction( EventLoopTimerRef inTimer, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-TTA)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(true);
	try
	{
		AApplication::GetApplication().EVT_DoTickTimerAction();
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoTickTimerAction() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-TTA]");
}

/**
�C�x���g�n���h���C���X�g�[��
*/
void	CAppImp::InstallEventHandler()
{
	OSStatus	err = noErr;
	
	EventHandlerRef	handlerRef;
	EventTypeSpec	typeSpec;
	typeSpec.eventClass = kEventClassApplication;
	typeSpec.eventKind = kEventAppDeactivated;
	err = ::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(STATIC_APICB_DoAppDeactivated),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,NULL);
	typeSpec.eventClass = kEventClassApplication;
	typeSpec.eventKind = kEventAppActivated;
	err = ::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(STATIC_APICB_DoAppActivated),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,NULL);
	typeSpec.eventClass = kEventClassCommand;
	typeSpec.eventKind = kEventProcessCommand;
	err = ::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(STATIC_APICB_DoCommand),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,NULL);
	typeSpec.eventClass = kEventClassCommand;
	typeSpec.eventKind = kEventCommandUpdateStatus;
	err = ::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(STATIC_APICB_DoCommandUpdateStatus),
			1,&typeSpec,this,&handlerRef);//D0004
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,NULL);
	typeSpec.eventClass = kEventClassKeyboard;
	typeSpec.eventKind = kEventRawKeyDown;
	err = ::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(STATIC_APICB_DoKeyDown),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,NULL);
	typeSpec.eventClass = kEventClassTextInput; 
	typeSpec.eventKind = kEventTextInputUnicodeForKeyEvent;
	err = ::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(STATIC_APICB_DoTextInput),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,NULL);
	
	typeSpec.eventClass = kEventClassMouse;
	typeSpec.eventKind = kEventMouseMoved;
	err = ::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(STATIC_APICB_DoMouseMoved),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,NULL);
	
	
	typeSpec.eventClass = kEventClassAppleEvent; 
	typeSpec.eventKind = kEventAppleEvent;
	err = ::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(STATIC_APICB_DoAppleEvent),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,NULL);
	
	//::AEInstallEventHandler(kCoreEventClass,kAEOpenDocuments,::NewAEEventHandlerUPP(STATIC_APICB_DoAEOpenDocument),0,false);
	
	/*typeSpec.eventClass = kEventClassMouse;
	typeSpec.eventKind = kEventMouseMoved;
	::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(STATIC_APICB_DoMouseMoved),
			1,&typeSpec,this,&handlerRef);*/
	/*
	���܂荂�������ʂ��Ȃ��̂ŏ]����AppleEvent�^�ɂ���
	typeSpec.eventClass = kEventClassTextInput;
	typeSpec.eventKind = kEventUpdateActiveInputArea;
	::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(CMimiApp::UpdateInputAreaHandler),
	1,&typeSpec,this,&handlerRef);*/
	/*�͂����ėl�q��
	typeSpec.eventClass = kEventClassMenu;
	typeSpec.eventKind = kEventMenuBeginTracking;
	::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(CMimiApp::MenuBeginTracking),
	1,&typeSpec,this,&handlerRef);
	typeSpec.eventClass = kEventClassMenu;
	typeSpec.eventKind = kEventMenuEndTracking;
	::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(CMimiApp::MenuEndTracking),
	1,&typeSpec,this,&handlerRef);*/
	
	/*typeSpec.eventClass = kEventClassMenu;
	typeSpec.eventKind = kEventMenuMatchKey;
	::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(CMimiApp::CommandUpdateStatusHandler),
	1,&typeSpec,this,&handlerRef);*/
		
	//�����C�x���g�g���K�[
	typeSpec.eventClass = 'user';
	typeSpec.eventKind = 'user';
	err = ::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(STATIC_APICB_DoInternalEvent),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,NULL);
	
}
#endif

//B0000
/**
�^�C�}�[�J�n
*/
void	CAppImp::StartTimer()
{
	//�^�C�}�[�J�n
	[((AppDelegate*)[NSApp delegate]) startTickTimer];
	
#if SUPPORT_CARBON
	OSStatus	err = noErr;
	
	//Tick�^�C�}�[
	err = ::InstallEventLoopTimer(::GetMainEventLoop(),::TicksToEventTime(1),::TicksToEventTime(1),
			::NewEventLoopTimerUPP(STATIC_APICB_DoTickTimerAction),this,&mTickTimerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,NULL);
#endif
}

//#288
/**
�^�C�}�[���ꎞ�I�ɒ�~
*/
void	CAppImp::StopTimer()
{
	//�^�C�}�[��~
	[((AppDelegate*)[NSApp delegate]) stopTickTimer];
	
#if SUPPORT_CARBON
	OSStatus	err = noErr;
	
	err = ::SetEventLoopTimerNextFireTime(mTickTimerRef,kEventDurationForever);
	if( err != noErr )   _ACErrorNum("SetEventLoopTimerNextFireTime() returned error: ",err,NULL);
#endif
}

//#288
/**
�^�C�}�[���ĊJ
*/
void	CAppImp::RestartTimer()
{
	//�^�C�}�[�ĊJ
	[((AppDelegate*)[NSApp delegate]) restartTickTimer];
	
#if SUPPORT_CARBON
	OSStatus	err = noErr;
	
	err = ::SetEventLoopTimerNextFireTime(mTickTimerRef,::TicksToEventTime(1));
	if( err != noErr )   _ACErrorNum("SetEventLoopTimerNextFireTime() returned error: ",err,NULL);
#endif
}


#pragma mark ===========================

#pragma mark ---�t�@�C���I���E�C���h�E

/**
�t�@�C���I���_�C�A���O�\��
*/
void	CAppImp::ShowFileOpenWindow( const ABool inShowInvisibleFile )
{
	//OpenPanel�擾 #1034
	NSOpenPanel* panel = [NSOpenPanel openPanel];
	[panel setCanChooseFiles:YES];
	[panel setCanChooseDirectories:NO];
	[panel setResolvesAliases:YES];
	[panel setAllowsMultipleSelection:YES];
	[panel setExtensionHidden:NO];
	[panel setTreatsFilePackagesAsDirectories:YES];
	if( inShowInvisibleFile == true )
	{
		[panel setShowsHiddenFiles:YES];
	}
	//CustomView�ݒ�
	AApplication::GetApplication().NVI_MakeCustomControlInFileOpenWindow(panel);
	//OpenPanel�\��
	if( [panel runModal] == NSFileHandlingPanelOKButton )
	{
		//���ʎ擾
		NSArray*	urls = [panel URLs];
		NSUInteger	count = [urls count];
		for( NSUInteger i = 0; i < count; i++ )
		{
			NSURL*	url = [urls objectAtIndex:i];
			AText	path;
			path.SetCstring([[url path] UTF8String]);
			AFileAcc	file;
			file.Specify(path);
			//�t�@�C�����J��
			AApplication::GetApplication().NVI_OpenFile(file,panel);
		}
	}
	/*#1034
	OSStatus	err = noErr;
	
	NavDialogCreationOptions	options;
	err = ::NavGetDefaultDialogCreationOptions(&options);
	if( err != noErr )   _ACErrorNum("NavGetDefaultDialogCreationOptions() returned error: ",err,NULL);
	options.modality = kWindowModalityAppModal;
	options.optionFlags |= kNavSupportPackages| kNavAllowOpenPackages;//B0100
	if( inShowInvisibleFile )
	{
		options.optionFlags |= kNavAllowInvisibleFiles;
	}
	AText	text;
	text.SetLocalizedText("AppName");
	options.clientName = text.CreateCFString();//AUtil::CreateCFStringFromText(text);
	err = ::NavCreateGetFileDialog(&options,NULL,::NewNavEventUPP(STATIC_APICB_FileOpenWindow),NULL,NULL,this,&mOpenDialog);
	if( err != noErr )   _ACErrorNum("NavCreateGetFileDialog() returned error: ",err,NULL);
	err = ::NavDialogRun(mOpenDialog);
	if( err != noErr )   _ACErrorNum("NavDialogRun() returned error: ",err,NULL);
	*/
}

#if 0
/**
�t�@�C���I���_�C�A���OCallback
*/
pascal void	CAppImp::STATIC_APICB_FileOpenWindow( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD )
{
	//�C�x���g�g�����U�N�V�����O�����^�㏈�� #0 �����ɂ��ꂪ�Ȃ��ƁARefresh���o�R��OS����C�x���gCB���������Ƃ��ɁA������sEventDepth==1�ƂȂ��Ă��܂��A
	//Window���̃I�u�W�F�N�g�ɑ΂���DoObjectArrayItemTrashDelete()���������A�����o�[�ϐ��ւ̃������A�N�Z�X���������A�����Ă��܂��B
	AStEventTransactionPrePostProcess	oneevent(false);
	
	OSStatus	err = noErr;
	
	try
	{
		CAppImp	*object = (CAppImp*)callbackUD;
		switch( selector )
		{
		  case kNavCBUserAction:
			{
				NavUserAction	userAction = ::NavDialogGetUserAction(object->mOpenDialog);
				if( userAction != kNavUserActionCancel && userAction != kNavUserActionNone )
				{
					NavReplyRecord	reply;
					if( ::NavDialogGetReply(object->mOpenDialog,&reply) == noErr )
					{
						if( reply.validRecord )
						{
							/*R0198 NVI_OpenFile()�ɂăJ�X�^���f�[�^�擾����悤�ɕύX
							AText	tec;
							ControlRef	controlRef = NULL;
							HIViewID	id;
							id.signature = 0;
							id.id = 1000;
							::HIViewFindByID(::HIViewGetRoot(params->window),id,&controlRef);
							if( controlRef != NULL )
							{
								MenuRef	menuref = NULL;
								::GetControlData(controlRef,0,kControlPopupButtonMenuRefTag,sizeof(menuref),&menuref,NULL);
								CMenuImp::STATIC_GetMenuItemText(menuref,::GetControl32BitValue(controlRef)-1,customdata.textEncodingName);
								AText	automatic;
								automatic.SetLocalizedText("OpenCustom_TextEncodingAutomatic");
								if( customdata.textEncodingName.Compare(automatic) == true )   customdata.textEncodingName.DeleteAll();
							}
							*/
							
							SInt32	count;
							err = ::AECountItems(&(reply.selection),&count);
							for( SInt32	i = 1; i <= count; i++ )
							{
								FSRef	fsref;
								AEKeyword	key;
								DescType	type;
								Size	size;
								err = ::AEGetNthPtr(&(reply.selection),i,typeFSRef,&key,&type,(Ptr)&fsref,sizeof(FSRef),&size);
								AFileAcc	file;
								file.SpecifyByFSRef(fsref);
								//GetApp().SPNVI_CreateDocumentFromLocalFile(file,tec);
								AApplication::GetApplication().NVI_OpenFile(file,params->window);
							}
							err = ::NavDisposeReply(&reply);
						}
					}
				}
				break;
			}
		  case kNavCBTerminate:
			{
				::NavDialogDispose(object->mOpenDialog);
				break;
			}
		  case kNavCBCustomize:
			{
				//R0198 �J�X�^���R���g���[��
				ARect	rect;
				rect.left		= params->customRect.left;
				rect.top		= params->customRect.top;
				rect.right		= params->customRect.right;
				rect.bottom		= params->customRect.bottom;
				AApplication::GetApplication().NVI_GetCustomControlRectInFileOpenWindow(rect);
				params->customRect.left		=rect.left;
				params->customRect.top		=rect.top;
				params->customRect.right	=rect.right;
				params->customRect.bottom	=rect.bottom;
				/*R0198 NVIDO_GetCustomControlRectInFileOpenWindow()�Ɉړ�
				if( params->customRect.right == 0 && params->customRect.bottom == 0 )
				{
					WindowRef	customWinRef;
					::CreateWindowFromNib(CAppImp::GetNibRef(),CFSTR("OpenCustom"),&customWinRef);
					Rect	bounds;
					::GetWindowPortBounds(customWinRef,&bounds);
					params->customRect.right = params->customRect.left + (bounds.right - bounds.left);
					params->customRect.bottom = params->customRect.top + (bounds.bottom - bounds.top);
					::DisposeWindow(customWinRef);
				}*/
				break;
			}
			//R0148
		  case kNavCBStart:
			{
				//�f�t�H���g�t�@�C���̐ݒ�
				/* B0000 ���Ȃ��Ƃ�mi2.1.8a8�`16�Ő���ɓ��삵�Ă��Ȃ��������A�Ō�ɊJ�����t�@�C���Ɠ����t�H���_�̃t�@�C���́A
				�u�����t�H���_����J���v�ŊJ�����Ƃ��ł���̂ŁA���܂胁���b�g���Ȃ��BOS�ɔC���������ǂ������B
				����āA�A�v���Ǝ��ɐݒ肵�Ȃ����ƂƂ���B
				AEDesc	folderDesc;
				::AECreateDesc(typeNull,NULL,0,&folderDesc);
				AFileAcc	lastopenedfile;
				if( GetApp().GetAppPref().GetLastOpenedFile(lastopenedfile) == true )
				{
					AFileAcc	folder;
					folder.SpecifyParent(lastopenedfile);
					FSRef	folderRef;
					folder.GetFSRef(folderRef);
					::AECreateDesc(typeFSRef,(Ptr)(&folderRef),sizeof(FSRef),&folderDesc);
					::NavCustomControl(params->context,kNavCtlSetLocation,&folderDesc);
					::AEDisposeDesc(&folderDesc);
				}*/
				//R0198 �J�X�^���R���g���[���쐬��NVI_MakeCustomControlInFileOpenWindow()�ɂčs���iCAppImp�ɂ̓A�v���ʂ̃R�[�h�֎~�j
				//�J�X�^���R���g���[��
				/*R0198 WindowRef	customWinRef;
				::CreateWindowFromNib(CAppImp::GetNibRef(),CFSTR("OpenCustom"),&customWinRef);
				ControlID	userPaneID = {'usrp',100};
				HIViewRef	userPane;
				::HIViewFindByID(::HIViewGetRoot(customWinRef),userPaneID,&userPane);
				//10.3���O�̃o�[�W�����ɑΉ�����ɂ�EmbedControl�̏����K�v
				::NavCustomControl(params->context,kNavCtlAddControl,userPane);
				::DisposeWindow(customWinRef);
				//
				//R0198 �b��
				ControlRef	controlRef = NULL;
				HIViewID	id;
				id.signature = 0;
				id.id = 1000;
				::HIViewFindByID(::HIViewGetRoot(params->window),id,&controlRef);
				if( controlRef != NULL )
				{
					AText	text;
					text.SetLocalizedText("OpenCustom_TextEncodingAutomatic");
					ATextArray	textArray;
					textArray.Add(text);
					text.SetCstring("-");
					textArray.Add(text);
					textArray.Add(GetApp().GetAppPref().GetTextEncodingMenuFixedTextArrayRef());
					textArray.Add(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kTextEncodingMenu));
					MenuRef	menuref = NULL;
					::GetControlData(controlRef,0,kControlPopupButtonMenuRefTag,sizeof(menuref),&menuref,NULL);
					CMenuImp::STATIC_SetMenuItemsFromTextArray(menuref,textArray);
					::SetControl32BitMaximum(controlRef,CMenuImp::STATIC_GetMenuItemCount(menuref));
				}*/
				AApplication::GetApplication().NVI_MakeCustomControlInFileOpenWindow(params->context,params->window);
				break;
			}
			//R0198 �J�X�^���R���g���[���C�x���g�n���h��
			//�Q�l�F
			//http://developer.apple.com/samplecode/AddNibToNav/listing1.html
		  case kNavCBEvent:
			{
				if( params->eventData.eventDataParms.event->what == mouseDown )
				{
					//�E�C���h�E�ʒu�擾 #1012
					OSStatus	err = noErr;
					Rect	windowBoundsRect = {0};
					err = ::GetWindowBounds(params->window,kWindowContentRgn,&windowBoundsRect);
					
					EventRecord *evt = params->eventData.eventDataParms.event;
					Point	where = evt->where;
					//#1012 ::GlobalToLocal(&where);
					//���[�J�����W�ɕϊ� #1012
					where.h -= windowBoundsRect.left;
					where.v -= windowBoundsRect.top;
					//
					ControlRef whichControl = ::FindControlUnderMouse(where,params->window,NULL);
					if( whichControl != NULL )
					{
						::HandleControlClick(whichControl,where,evt->modifiers,NULL);
					}
				}
				break;
			}
		  case kNavCBAdjustRect:
			{
				ControlID	userPaneID = {'usrp',100};
				HIViewRef	userPane = NULL;
				err = ::HIViewFindByID(::HIViewGetRoot(params->window),userPaneID,&userPane);
				//if( err != noErr )   _ACErrorNum("HIViewFindByID() returned error: ",err,NULL);
				if( userPane != NULL )
				{
					::MoveControl(userPane,params->customRect.left,params->customRect.top);
				}
				break;
			}
		}
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_FileOpenWindow() caught exception.",NULL);//#199
	}
}
#endif

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---MouseTracking�ݒ�

/**
MouseTracking����Ԃ�ݒ肷��

�ΏۃE�C���h�E����Mouse���o�����Ƃ����m���邽�߁B
*/
void	CAppImp::SetMouseTracking( const AWindowRef inWindowRef, const AControlID inControlID )
{
	sMouseTrackingMode = true;
	sMouseTrackingWindowRef = inWindowRef;
	sMouseTrackingControlID = inControlID;
}

/*#310
void	CAppImp::ClearMouseTracking()
{
	sMouseTrackingMode = false;
}

AControlID	CAppImp::GetMouseTrackingControlID( const AWindowRef inWindowRef )
{
	if( sMouseTrackingMode == true && sMouseTrackingWindowRef == inWindowRef )
	{
		return sMouseTrackingControlID;
	}
	else
	{
		return kControlID_Invalid;
	}
}
*/

//#310
/**
Window�ł�MouseMoved�C�x���g�������ɃR�[�������B���̃R���g���[����Tracking����������A���̃R���g���[���ɑ΂���DoMouseExited()���Ƃ΂��B

CWindowImp::APICB_DoMouseMoved()���疈��R�[�������B
�Ȃ��Ami��Window�O�Ƀ}�E�X�ړ��̏ꍇ�́ACAppImp::STATIC_APICB_DoMouseMoved()�ɂē����悤�ɏ��������B
*/
void	CAppImp::ProcessMouseExitToAnotherControl( const EventRef inEventRef, const AWindowRef inWindowRef, const AControlID inControlID )
{
	//Tracking���̏ꍇ�̂ݏ���
	if( sMouseTrackingMode == true )
	{
		if( inWindowRef != sMouseTrackingWindowRef || inControlID != sMouseTrackingControlID )
		{
			//���݂�Tracking��Control�ƈ�v���Ȃ���΁A����Control�ɑ΂���DoMouseExited���R�[������B
			if( CWindowImp::ExistWindowImpForWindowRef(sMouseTrackingWindowRef) == true )
			{
				CWindowImp::GetWindowImpByWindowRef(sMouseTrackingWindowRef).DoMouseExited(sMouseTrackingControlID,inEventRef);//R0228
			}
			//Tracking����
			sMouseTrackingMode = false;
		}
	}
}

//MouseTracking�f�[�^
ABool						CAppImp::sMouseTrackingMode;
AWindowRef					CAppImp::sMouseTrackingWindowRef;
AControlID					CAppImp::sMouseTrackingControlID;

#endif

#pragma mark ===========================

#pragma mark ---���擾

/**
�o�[�W�����擾
*/
void	CAppImp::GetVersion( AText& outText ) const
{
	/*#645 �������iCAppImp::CAppImp()�œǂݍ��ނ悤�ɂ���j
	CFBundleRef	appBundle = ::CFBundleGetMainBundle();
	CFStringRef	str = (CFStringRef)::CFBundleGetValueForInfoDictionaryKey(appBundle,CFSTR("CFBundleVersion"));
	if( str != NULL )
	{
		outText.SetFromCFString(str);
	}
	*/
	outText.SetText(mApplicationVersionText);
}

//#1096
/**
�r���h�ԍ��擾
*/
void	CAppImp::GetBuildNumber( AText& outText ) const
{
	outText.SetText(mApplicationBuildNumberText);
}

//#305
/**
����擾
*/
ALanguage	CAppImp::GetLanguage() const
{
	return mLanguage;
}

//#1316
/**
�_�[�N���[�h���ǂ����̔���
*/
ABool	CAppImp::IsDarkMode() const
{
	if( NSApp.effectiveAppearance == [NSAppearance appearanceNamed:NSAppearanceNameDarkAqua] )
	{
		return true;
	}
	else
	{
		return false;
	}
}

#pragma mark ===========================

#pragma mark ---JavaScript
//#904

/**
JavaScript�����s
*/
void	CAppImp::DoJavaScript( const AText& inScript )
{
	AppDelegate*	appdelegate = (AppDelegate*)[NSApp delegate];//10.10SDK�ł̓L���X�g�K�v�ɂȂ����̂ŁA(AppDelegate*)�L���X�g�ǉ�
	AStCreateNSStringFromAText	str(inScript);
	[appdelegate doJavaScript:str.GetNSString()];
}

/*
ABool	CAppImp::IsExecutingJavaScript()
{
	AppDelegate*	appdelegate = [NSApp delegate];
	return ([appdelegate executingJavaScript] == YES);
}

void	CAppImp::CancelExecutingJavaScript()
{
	AppDelegate*	appdelegate = [NSApp delegate];
	[appdelegate cancelExecutingJavaScript];
}
*/

#pragma mark ===========================

#pragma mark ---CDocImp�Ǘ�

//#1078
/**
CDocImp�I�u�W�F�N�g����
*/
AObjectID	CAppImp::CreateDocImp()
{
	CDocImpFactory	factory(this);
	AIndex	index = mDocImpArray.AddNewObject(factory);
	return mDocImpArray.GetObject(index).GetObjectID();
}

/**
CDocImp�I�u�W�F�N�g�폜
*/
void	CAppImp::DeleteDocImp( const AObjectID inDocImpID )
{
	AIndex	index = mDocImpArray.GetIndexByID(inDocImpID);
	mDocImpArray.Delete1Object(index);
}

/**
CDocImp�I�u�W�F�N�g�擾
*/
CDocImp&	CAppImp::GetDocImpByID( const AObjectID inDocImpID )
{
	return mDocImpArray.GetObjectByID(inDocImpID);
}

#pragma mark ===========================
#pragma mark [�N���X]CDocImp
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

/**
�R���X�g���N�^
*/
CDocImp::CDocImp( AObjectArrayItem*	inParent ):AObjectArrayItem(inParent), mNSDocument(nil)
{
}

/**
�f�X�g���N�^
*/
CDocImp::~CDocImp()
{
	if( mNSDocument != nil )
	{
		[mNSDocument close];
	}
}

//#1034
/**
ADocument��DocumentID��ݒ�
*/
void	CDocImp::SetDocumentID( const ADocumentID inDocumentID )
{
	[mNSDocument setDocumentID:inDocumentID];
}


