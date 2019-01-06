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
//コマンド引数
int							CAppImp::sArgc = 0;
const char**				CAppImp::sArgv = NULL;



#pragma mark ===========================
#pragma mark [クラス]CAppImp
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ

/**
コンストラクタ
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
	//R0199 OSX10.3では使用不可能のようだ（ファイルリストウインドウをクリックしてもファイルが開かない、テキストウインドウクリックしてもキャレット設定されないなど）
	//↑deploy target設定を10.3にすることが必要だっただけでした。080802
	//でも、10.3では、途中からスペルチェックが動かなくなったりするのでやっぱり制限
	//R0199 10.4以上ならCocoa使用するようにしました。
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
	{
		InitCocoa();
	}
	*/
	CWindowImp::sAliveWindowArray.SetThrowIfOutOfBounds();//B0000
	//#305
	//現在の言語を取得
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
	//アプリケーションバージョン読み込み#645
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
	//保存ダイアログの「拡張子を隠す」をOFFにする（旧OSではこの設定が必要っぽい）
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setInteger:0 forKey:@"NSNavLastUserSetHideExtensionButtonState"];
	[defaults synchronize];
	
	//#852
	//フォントデータ初期化
	AFontWrapper::Init();
	/*#1034
	//デフォルトATSUStyle取得
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
		//指定フォント無しでフォントフォールバックを設定する
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
//ドット単位スクロール
ABool	CAppImp::sUseWheelScrollFloatValue = true;

//#852
//#1034 ATSUStyle	CAppImp::sDefaultATSUStyle = NULL;

//#703
//Font Fallback
ATSUFontFallbacks	CAppImp::sFontFallbacks = NULL;

//#852
/**
デフォルトASTSUStyle取得
*/
/*#1034
ATSUStyle	CAppImp::GetDefaultATSUStyle()
{
	return sDefaultATSUStyle;
}
*/

//現在表示中のContext menuの対象ウインドウ／コントロール
AWindowID	CAppImp::sContextMenuWindowID = kObjectID_Invalid;
AControlID	CAppImp::sContextMenuViewControlID = kControlID_Invalid;

/**
現在表示中のContext menuの対象ウインドウ／コントロールを記憶
*/
void	CAppImp::SetContextMenuTarget( const AWindowID inWindowID, const AControlID inControlID )
{
	sContextMenuWindowID = inWindowID;
	sContextMenuViewControlID = inControlID;
}

#if SUPPORT_CARBON
/**
"main"のNibRefを取得
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
"menu"のNibRefを取得
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
コマンドライン引数を設定
*/
void	CAppImp::Setup( int argc, const char **argv )
{
	sArgc = argc;
	sArgv = argv;
}

/**
コマンドライン引数のTextArrayを取得
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
アプリ実行
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
	//#688 main()にてNSApplicationMain()コールにより、メインループ実行済み ::RunApplicationEventLoop();
	return 0;
}

/**
アプリ終了
*/
void	CAppImp::Quit()
{
	//#688 ::QuitApplicationEventLoop();
	[NSApp terminate:nil];//#668 NSApp（アプリケーションオブジェクト）にterminate送信（senderはnilとする）
}

#pragma mark ===========================

#pragma mark ---標準動作
//#688

/**
このアプリケーションを隠す
*/
void	CAppImp::HideThisApplication()
{
	[NSApp hide:nil];
}

/**
他のアプリケーションを隠す
*/
void	CAppImp::HideOtherApplications()
{
	[NSApp hideOtherApplications:nil];
}

/**
全てのアプリケーションを表示
*/
void	CAppImp::ShowAllApplications()
{
	[NSApp unhideAllApplications:nil];
}

//#688
/**
全てを手前に表示
*/
void	CAppImp::BringAllToFront()
{
	[NSApp arrangeInFront:nil];
}

#pragma mark ===========================

#pragma mark ---Speech
//#851

/**
テキスト読み上げ開始
*/
void	CAppImp::SpeakText( const AText& inText )
{
	AppDelegate*	appdelegate = (AppDelegate*)[NSApp delegate];
	AStCreateNSStringFromAText	str(inText);
	[appdelegate speakText:str.GetNSString()];
}

/**
テキスト読み上げ停止
*/
void	CAppImp::StopSpeaking()
{
	AppDelegate*	appdelegate = (AppDelegate*)[NSApp delegate];
	[appdelegate stopSpeaking];
}

/**
テキスト読み上げ中かどうかを返す
*/
ABool	CAppImp::IsSpeaking() const
{
	AppDelegate*	appdelegate = (AppDelegate*)[NSApp delegate];
	return ([appdelegate isSpeaking] == YES);
}

#pragma mark ===========================

#pragma mark ---イベントハンドラ

//#688
/**
メニュー項目選択時処理
*/
void	CAppImp::APICB_CocoaDoMenuAction( const AMenuItemID inMenuItemID, const AModifierKeys inModifierKeys,
		const AMenuRef inMenuRef, const AIndex inMenuItemIndex )
{
	ABool	handled = false;
	
	//メニュー項目に対応するaction text取得
	AText	actiontext;
	AApplication::GetApplication().NVI_GetMenuManager().GetMenuActionText(inMenuItemID,
				inMenuRef,inMenuItemIndex,actiontext);
	
	//context menu表示中なら、その対象ウインドウ／コントロールで処理を試す
	if( sContextMenuWindowID != kObjectID_Invalid )
	{
		//対象コントロールで実行
		if( AApplication::GetApplication().NVI_GetWindowByID(sContextMenuWindowID).
					NVI_GetViewByControlID(sContextMenuViewControlID).
					EVT_DoMenuItemSelected(inMenuItemID,actiontext,inModifierKeys) == true )
		{
			handled = true;
		}
		//対象ウインドウで実行
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
		//フォーカスウインドウでEVT_DoMenuItemSelected()実行
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
	//ウインドウで実行されなかった場合、アプリケーションでEVT_DoMenuItemSelected()実行
	if( handled == false )
	{
		if( AApplication::GetApplication().
					EVT_DoMenuItemSelected(inMenuItemID,actiontext,inModifierKeys) == true )
		{
			handled = true;
		}
	}
	
	//NVI_UpdateMenu()はコール元のdoMenuActions:にて常に実行
}

/**
Tickタイマーアクション処理
*/
void	CAppImp::APICB_CocoaDoTickTimerAction()
{
	//AApplication::EVT_DoTickTimerAction()実行
	AApplication::GetApplication().EVT_DoTickTimerAction();
}

/**
アプリケーションactivate時処理
*/
void	CAppImp::APICB_CocoaDoBecomeActive()
{
	//AApplication::EVT_DoAppActivated()実行
	AApplication::GetApplication().EVT_DoAppActivated();
}

/**
アプリケーションdeactivate時処理
*/
void	CAppImp::APICB_CocoaDoResignActive()
{
	//AApplication::EVT_DoAppDeactivated()実行
	AApplication::GetApplication().EVT_DoAppDeactivated();
}

#if SUPPORT_CARBON
/**
アプリがDeactivatedされたときのイベントハンドラ
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoAppDeactivated( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-AD)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
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
アプリがActivatedされたときのイベントハンドラ
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoAppActivated( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-AA)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
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
コマンドイベントハンドラ
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoCommand( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-C)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	
	OSStatus	result = eventNotHandledErr;
	
	//コマンド取得
	HICommand	command = {0,0};
	err = ::GetEventParameter(inEventRef,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&command);
	//キー取得
	UInt32	modifiers = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	
	try
	{
		AText	actiontext;
		AApplication::GetApplication().NVI_GetMenuManager().GetMenuActionText(command.commandID,command.menu.menuRef,command.menu.menuItemIndex-1,actiontext);
		//kCommandID_UpdateCommandStatusならメニュー更新
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
コマンド状態Updateイベントハンドラ
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoCommandUpdateStatus( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-CUS)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		//処理無し
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoCommandUpdateStatus() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-CUS]");
	return eventNotHandledErr;
}

/**
キー押下イベントハンドラ
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoKeyDown( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-KD)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		//処理無し
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoKeyDown() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-KD]");
	return eventNotHandledErr;
}

/**
テキスト入力イベントハンドラ
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoTextInput( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-TI)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		//処理無し
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoTextInput() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-TI]");
	return eventNotHandledErr;
}

/**
マウス移動イベントハンドラ
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoMouseMoved( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-MM)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(true);
	
	ABool	result = eventNotHandledErr;
	try
	{
		if( AApplication::GetApplication().EVT_DoMouseMoved() == true )
		{
			result = noErr;
		}
		//B0000 080810
		//STATIC_APICB_DoMouseMoved()は、Windowでmouse movedイベントが処理されなかった場合にコールされる
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
	return eventNotHandledErr;//resultではcursorchangeイベントがこない？
}

/**
AppleEventイベントハンドラ
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoAppleEvent( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-AE)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
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
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
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
内部イベントイベントハンドラ
*/
pascal OSStatus	CAppImp::STATIC_APICB_DoInternalEvent( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-IE)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
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
Timerイベントハンドラ
*/
pascal void	CAppImp::STATIC_APICB_DoTickTimerAction( EventLoopTimerRef inTimer, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-TTA)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return;
	//イベントトランザクション前処理／後処理
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
イベントハンドラインストール
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
	あまり高速化効果がないので従来のAppleEvent型にする
	typeSpec.eventClass = kEventClassTextInput;
	typeSpec.eventKind = kEventUpdateActiveInputArea;
	::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(CMimiApp::UpdateInputAreaHandler),
	1,&typeSpec,this,&handlerRef);*/
	/*はずして様子見
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
		
	//内部イベントトリガー
	typeSpec.eventClass = 'user';
	typeSpec.eventKind = 'user';
	err = ::InstallEventHandler(::GetApplicationEventTarget(),::NewEventHandlerUPP(STATIC_APICB_DoInternalEvent),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,NULL);
	
}
#endif

//B0000
/**
タイマー開始
*/
void	CAppImp::StartTimer()
{
	//タイマー開始
	[((AppDelegate*)[NSApp delegate]) startTickTimer];
	
#if SUPPORT_CARBON
	OSStatus	err = noErr;
	
	//Tickタイマー
	err = ::InstallEventLoopTimer(::GetMainEventLoop(),::TicksToEventTime(1),::TicksToEventTime(1),
			::NewEventLoopTimerUPP(STATIC_APICB_DoTickTimerAction),this,&mTickTimerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,NULL);
#endif
}

//#288
/**
タイマーを一時的に停止
*/
void	CAppImp::StopTimer()
{
	//タイマー停止
	[((AppDelegate*)[NSApp delegate]) stopTickTimer];
	
#if SUPPORT_CARBON
	OSStatus	err = noErr;
	
	err = ::SetEventLoopTimerNextFireTime(mTickTimerRef,kEventDurationForever);
	if( err != noErr )   _ACErrorNum("SetEventLoopTimerNextFireTime() returned error: ",err,NULL);
#endif
}

//#288
/**
タイマーを再開
*/
void	CAppImp::RestartTimer()
{
	//タイマー再開
	[((AppDelegate*)[NSApp delegate]) restartTickTimer];
	
#if SUPPORT_CARBON
	OSStatus	err = noErr;
	
	err = ::SetEventLoopTimerNextFireTime(mTickTimerRef,::TicksToEventTime(1));
	if( err != noErr )   _ACErrorNum("SetEventLoopTimerNextFireTime() returned error: ",err,NULL);
#endif
}


#pragma mark ===========================

#pragma mark ---ファイル選択ウインドウ

/**
ファイル選択ダイアログ表示
*/
void	CAppImp::ShowFileOpenWindow( const ABool inShowInvisibleFile )
{
	//OpenPanel取得 #1034
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
	//CustomView設定
	AApplication::GetApplication().NVI_MakeCustomControlInFileOpenWindow(panel);
	//OpenPanel表示
	if( [panel runModal] == NSFileHandlingPanelOKButton )
	{
		//結果取得
		NSArray*	urls = [panel URLs];
		NSUInteger	count = [urls count];
		for( NSUInteger i = 0; i < count; i++ )
		{
			NSURL*	url = [urls objectAtIndex:i];
			AText	path;
			path.SetCstring([[url path] UTF8String]);
			AFileAcc	file;
			file.Specify(path);
			//ファイルを開く
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
ファイル選択ダイアログCallback
*/
pascal void	CAppImp::STATIC_APICB_FileOpenWindow( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD )
{
	//イベントトランザクション前処理／後処理 #0 ここにこれがないと、Refresh等経由でOSからイベントCB発生したときに、そこでsEventDepth==1となってしまい、
	//Window等のオブジェクトに対してDoObjectArrayItemTrashDelete()が発生し、メンバー変数へのメモリアクセスが発生し、落ちてしまう。
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
							/*R0198 NVI_OpenFile()にてカスタムデータ取得するように変更
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
				//R0198 カスタムコントロール
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
				/*R0198 NVIDO_GetCustomControlRectInFileOpenWindow()に移動
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
				//デフォルトファイルの設定
				/* B0000 少なくともmi2.1.8a8～16で正常に動作していなかったし、最後に開いたファイルと同じフォルダのファイルは、
				「同じフォルダから開く」で開くことができるので、あまりメリットがない。OSに任せた方が良さそう。
				よって、アプリ独自に設定しないこととする。
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
				//R0198 カスタムコントロール作成はNVI_MakeCustomControlInFileOpenWindow()にて行う（CAppImpにはアプリ個別のコード禁止）
				//カスタムコントロール
				/*R0198 WindowRef	customWinRef;
				::CreateWindowFromNib(CAppImp::GetNibRef(),CFSTR("OpenCustom"),&customWinRef);
				ControlID	userPaneID = {'usrp',100};
				HIViewRef	userPane;
				::HIViewFindByID(::HIViewGetRoot(customWinRef),userPaneID,&userPane);
				//10.3より前のバージョンに対応するにはEmbedControlの処理必要
				::NavCustomControl(params->context,kNavCtlAddControl,userPane);
				::DisposeWindow(customWinRef);
				//
				//R0198 暫定
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
			//R0198 カスタムコントロールイベントハンドラ
			//参考：
			//http://developer.apple.com/samplecode/AddNibToNav/listing1.html
		  case kNavCBEvent:
			{
				if( params->eventData.eventDataParms.event->what == mouseDown )
				{
					//ウインドウ位置取得 #1012
					OSStatus	err = noErr;
					Rect	windowBoundsRect = {0};
					err = ::GetWindowBounds(params->window,kWindowContentRgn,&windowBoundsRect);
					
					EventRecord *evt = params->eventData.eventDataParms.event;
					Point	where = evt->where;
					//#1012 ::GlobalToLocal(&where);
					//ローカル座標に変換 #1012
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

#pragma mark ---MouseTracking設定

/**
MouseTracking中状態を設定する

対象ウインドウからMouseが出たことを検知するため。
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
WindowでのMouseMovedイベント発生時にコールされる。他のコントロールでTracking中だったら、そのコントロールに対してDoMouseExited()をとばす。

CWindowImp::APICB_DoMouseMoved()から毎回コールされる。
なお、miのWindow外にマウス移動の場合は、CAppImp::STATIC_APICB_DoMouseMoved()にて同じように処理される。
*/
void	CAppImp::ProcessMouseExitToAnotherControl( const EventRef inEventRef, const AWindowRef inWindowRef, const AControlID inControlID )
{
	//Tracking中の場合のみ処理
	if( sMouseTrackingMode == true )
	{
		if( inWindowRef != sMouseTrackingWindowRef || inControlID != sMouseTrackingControlID )
		{
			//現在のTracking中Controlと一致しなければ、そのControlに対してDoMouseExitedをコールする。
			if( CWindowImp::ExistWindowImpForWindowRef(sMouseTrackingWindowRef) == true )
			{
				CWindowImp::GetWindowImpByWindowRef(sMouseTrackingWindowRef).DoMouseExited(sMouseTrackingControlID,inEventRef);//R0228
			}
			//Tracking解除
			sMouseTrackingMode = false;
		}
	}
}

//MouseTrackingデータ
ABool						CAppImp::sMouseTrackingMode;
AWindowRef					CAppImp::sMouseTrackingWindowRef;
AControlID					CAppImp::sMouseTrackingControlID;

#endif

#pragma mark ===========================

#pragma mark ---情報取得

/**
バージョン取得
*/
void	CAppImp::GetVersion( AText& outText ) const
{
	/*#645 高速化（CAppImp::CAppImp()で読み込むようにする）
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
ビルド番号取得
*/
void	CAppImp::GetBuildNumber( AText& outText ) const
{
	outText.SetText(mApplicationBuildNumberText);
}

//#305
/**
言語取得
*/
ALanguage	CAppImp::GetLanguage() const
{
	return mLanguage;
}

//#1316
/**
ダークモードかどうかの判定
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
JavaScriptを実行
*/
void	CAppImp::DoJavaScript( const AText& inScript )
{
	AppDelegate*	appdelegate = (AppDelegate*)[NSApp delegate];//10.10SDKではキャスト必要になったので、(AppDelegate*)キャスト追加
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

#pragma mark ---CDocImp管理

//#1078
/**
CDocImpオブジェクト生成
*/
AObjectID	CAppImp::CreateDocImp()
{
	CDocImpFactory	factory(this);
	AIndex	index = mDocImpArray.AddNewObject(factory);
	return mDocImpArray.GetObject(index).GetObjectID();
}

/**
CDocImpオブジェクト削除
*/
void	CAppImp::DeleteDocImp( const AObjectID inDocImpID )
{
	AIndex	index = mDocImpArray.GetIndexByID(inDocImpID);
	mDocImpArray.Delete1Object(index);
}

/**
CDocImpオブジェクト取得
*/
CDocImp&	CAppImp::GetDocImpByID( const AObjectID inDocImpID )
{
	return mDocImpArray.GetObjectByID(inDocImpID);
}

#pragma mark ===========================
#pragma mark [クラス]CDocImp
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ

/**
コンストラクタ
*/
CDocImp::CDocImp( AObjectArrayItem*	inParent ):AObjectArrayItem(inParent), mNSDocument(nil)
{
}

/**
デストラクタ
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
ADocumentのDocumentIDを設定
*/
void	CDocImp::SetDocumentID( const ADocumentID inDocumentID )
{
	[mNSDocument setDocumentID:inDocumentID];
}


