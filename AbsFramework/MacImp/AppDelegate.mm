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

AppDelegate
#688

*/

#import "AppDelegate.h"
#include "../../AbsFramework/Frame.h"
#include "../../App/Source/AApp.h"
#include "../AbsBase/Cocoa.h"
#import "CocoaWindow.h"
#include "CocoaMacro.h"
#include "AApp.h"
//#1097 #import <HockeySDK/HockeySDK.h>
#include "../../AbsFramework/MacWrapper/ODBEditorSuite.h"

#pragma mark ===========================
#pragma mark [クラス]AppDelegate
#pragma mark ===========================

@implementation AppDelegate

#pragma mark ===========================

#pragma mark ---初期設定

/**
初期設定
*/
- (id)init
{
	//継承処理実行
	self = [super init];
	
	if( self != nil )
	{
		//初期設定
		tickTimer = nil;
		keyHandled = NO;//#1015
	}
	
	return self;
}

/*#1203 一旦保留
- (NSTouchBar *)makeTouchBar
{
    NSTouchBar *bar = [[NSTouchBar alloc] init];
    bar.delegate = self;
    
    // Set the default ordering of items.
    bar.defaultItemIdentifiers =
			@[@"com.TouchBarCatalog.windowController.label", NSTouchBarItemIdentifierCandidateList, NSTouchBarItemIdentifierOtherItemsProxy];
	bar.customizationAllowedItemIdentifiers = @[@"com.TouchBarCatalog.windowController.label"];
    
    return bar;
}

- (nullable NSTouchBarItem *)touchBar:(NSTouchBar *)touchBar makeItemForIdentifier:(NSTouchBarItemIdentifier)identifier
{
    if ([identifier isEqualToString:@"com.TouchBarCatalog.windowController.label"])
    {
        NSTextField *theLabel = [NSTextField labelWithString:NSLocalizedString(@"Catalog", @"")];
        
        NSCustomTouchBarItem *customItemForLabel =
        [[NSCustomTouchBarItem alloc] initWithIdentifier:@"com.TouchBarCatalog.windowController.label"];
        customItemForLabel.view = theLabel;
        
        // We want this label to always be visible no matter how many items are in the NSTouchBar instance.
        customItemForLabel.visibilityPriority = NSTouchBarItemPriorityHigh;
        
        return customItemForLabel;
    }
    
    return nil;
}
*/

/*#1097
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification 
{
	[[BITHockeyManager sharedHockeyManager] configureWithIdentifier:@"ffd3f528e1859fc12b270dcdd9a9e596"];
	[[BITHockeyManager sharedHockeyManager] startManager];
}
*/
/*#1203 一旦保留
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification 
{
	// Here we just opt-in for allowing our bar to be customized throughout the app.
	//if ([[NSApplication sharedApplication] respondsToSelector:@selector(isAutomaticCustomizeTouchBarMenuItemEnabled)])
	{
		[NSApplication sharedApplication].automaticCustomizeTouchBarMenuItemEnabled = YES;
	}
}
*/

/**
メインNib読み込み完了時の処理
*/
- (void)awakeFromNib
{
	try
	{
		//マルチスレッドアプリであることをOSに認識させるために、ダミーのCocoaスレッドを生成
		[NSThread detachNewThreadSelector:@selector(dummyMethodForFirstThread:) toTarget:self withObject:nil];
		if( [NSThread isMultiThreaded] == NO )
		{
			_ACError("",NULL);
		}
		
		//#1262 #1263 OSが自動でタブ関連メニューを追加しないようにする
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_12 )
		{
			[NSWindow setAllowsAutomaticWindowTabbing:NO];
		}
		
		//メインメニューの初期化
		NSMenu*	mainmenu = [NSApp mainMenu];
		ACocoa::InitMenuActionAndTarget(mainmenu);
		//AAppオブジェクト生成
		new AApp;
#if BUILD_WITH_JAVASCRIPTCORE
		//NSOperationQueueテスト mJSOperationQueue = [[NSOperationQueue alloc] init];
		
		//JavaScriptコンテキスト取得
		mJSContext = JSGlobalContextCreate( NULL );
		
		//JSWrapperオブジェクト生成
		mJSWrapper = [[JSWrappers alloc] initWithContext:mJSContext];
		
		//グローバル変数version, programにバンドル情報を設定
		[mJSWrapper addGlobalStringProperty:@"version" withValue:
		[[NSBundle mainBundle] objectForInfoDictionaryKey: (NSString*) kCFBundleVersionKey]];
		[mJSWrapper addGlobalStringProperty:@"program" withValue:
		[[NSBundle mainBundle] objectForInfoDictionaryKey: (NSString*) kCFBundleNameKey]];
		
		//グローバル関数messagebox定義
		[mJSWrapper addGlobalFunctionProperty:@"messagebox" withCallback: MessagBoxFunction];
		
		//グローバルオブジェクトmiimpを追加
		[mJSWrapper addGlobalObject:@"miimp" ofClass:AppClass() withPrivateData:self];
		
		//Documentクラスを生成しておく
		DocClass();
#endif
		
		//test
		/*
		int	fd = ::open("/Volumes/daisuke/AApp.cpp",O_RDWR|O_NONBLOCK);
		AText	text("/Volumes/daisuke/AApp.cpp");
		AStCreateNSStringFromAText	str(text);
		BOOL	isDir = NO;
		BOOL	result = [[NSFileManager defaultManager] fileExistsAtPath:str.GetNSString() isDirectory:&isDir];
		*/
		
		//アプリケーションを実行状態にする
		AApplication::GetApplication().NVI_Run();
	}
	catch(...)
	{
		fprintf(stderr,"AppDelegate::awakeFromNib catched throw");
	}
	
	/*AEハンドラ64bit移行テスト（AEResolve等はどうする？）
	[[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
		andSelector:@selector(handleAppleEvent:withReplyEvent:)
		forEventClass:'core'
	andEventID:'getd'];
	*/
	
	//URLスキーム #1003
	[[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
		andSelector:@selector(handleAppleEvent:withReplyEvent:)
		forEventClass:'GURL'
	andEventID:'GURL'];
	
	//コマンドラインからの多重起動時の引数受け取り用ハンドラ #1034
	[[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
		andSelector:@selector(handleAppleEvent:withReplyEvent:)
		forEventClass:'misc'
	andEventID:'args'];
	
	//#851
	//Speechオブジェクト初期化
	mSpeechSynthesizer = [[NSSpeechSynthesizer alloc] initWithVoice:nil];//システム設定で設定したデフォルトボイスが設定される https://developer.apple.com/library/prerelease/content/documentation/Cocoa/Conceptual/Speech/Articles/SynthesizeSpeech.html
	[mSpeechSynthesizer setDelegate:self];
	//mSpeechVoiceList = [NSSpeechSynthesizer availableVoices];
	mIsSpeaking = NO;
}

/**
AEハンドラ
*/
- (void)handleAppleEvent:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent
{
	AEEventClass eventClass = [event eventClass];
	AEEventID eventID = [event eventID];
	
	//URLスキーム #1003
	if( eventClass == 'GURL' && eventID == 'GURL' )
	{
		NSString *URLString = [[event paramDescriptorForKeyword:keyDirectObject] stringValue];
		AText	uri;
		ACocoa::SetTextFromNSString(URLString,uri);
		AApplication::GetApplication().NVI_ExecuteURIScheme(uri);
	}
	//コマンドラインからの多重起動時の引数受け取り用ハンドラ #1034
	if( eventClass == 'misc' && eventID == 'args' )
	{
		NSString *str = [[event paramDescriptorForKeyword:keyDirectObject] stringValue];
		AText	text;
		ACocoa::SetTextFromNSString(str,text);
		//TextArrayに構成して引数実行
		ATextArray	argsArray;
		for( AIndex pos = 0; pos < text.GetItemCount(); )
		{
			AText	line;
			text.GetLine(pos,line);
			argsArray.Add(line);
		}
		//引数実行
		AApplication::GetApplication().NVI_ExecCommandLineArgs(argsArray);
	}
}

//#1317 TextDocument/initWithContentsOfURLでAppeEventを取得できない問題の対策
/**
open Apple Eventの処理
https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/ScriptableCocoaApplications/SApps_handle_AEs/SAppsHandleAEs.html#//apple_ref/doc/uid/20001239
*/
- (void)application:(NSApplication *)sender openFiles:(NSArray<NSString *> *)filenames
{
	for( NSString* filename in filenames )
	{
		//==================テキストエンコーディング取得==================
		//
		AText	tecname;
		//
		NSAppleEventDescriptor*	evtDesc = [[NSAppleEventManager sharedAppleEventManager] currentAppleEvent];
		if( evtDesc != nil )
		{
			NSAppleEventDescriptor*	tecDesc = [evtDesc descriptorForKeyword:keyCharacterCode];
			if( tecDesc != nil )
			{
				NSString*	tecStr = [tecDesc stringValue];
				if( tecStr != nil )
				{
					ACocoa::SetTextFromNSString(tecStr,tecname);
				}
			}
		}
		
		//==================ドキュメント生成==================
		AText	path;
		path.SetFromNSString(filename);
		AFileAcc	file;
		file.Specify(path);
		//#1006 ここで直接ドキュメント生成はせず、内部イベントへキューイングことにする ADocumentID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(path,tecname);
		//==================ODB==================
		//#1006
		//ODBデータ取得
		ABool	odbMode = false;
		OSType	odbServer = 0;
		AText	odbSenderToken;
		AText	odbCustomPath;
		if( evtDesc != nil )
		{
			NSAppleEventDescriptor*	odbServerDesc = [evtDesc paramDescriptorForKeyword:keyFileSender];
			NSAppleEventDescriptor*	odbSenderTokenDesc = [evtDesc paramDescriptorForKeyword:keyFileSenderToken];
			NSAppleEventDescriptor*	odbCustomPathDesc = [evtDesc paramDescriptorForKeyword:keyFileCustomPath];
			if( odbServerDesc != nil && odbSenderTokenDesc != nil && odbCustomPathDesc != nil )
			{
				odbServer = [odbServerDesc typeCodeValue];
				NSString*	odbSenderTokenStr = [odbSenderTokenDesc stringValue];
				if( odbSenderTokenStr != nil )
				{
					ACocoa::SetTextFromNSString(odbSenderTokenStr,odbSenderToken);
				}
				NSString*	odbCustomPathStr = [odbCustomPathDesc stringValue];
				if( odbCustomPathStr != nil )
				{
					ACocoa::SetTextFromNSString(odbCustomPathStr,odbCustomPath);
				}
				//#1006 ここで直接ドキュメント生成はせず、内部イベントへキューイングことにする GetApp().SPI_GetTextDocumentByID(docID).SPI_SetODBMode(odbServer,odbSenderToken,odbCustomPath);
				odbMode = true;
			}
		}
		
		//ドキュメント生成するための内部イベントを作成
		//ファイルパス取得
		AText	filepath;
		file.GetPath(filepath);
		//NULL文字区切りでドキュメント情報をtextに格納
		AText	text;
		text.SetText(filepath);
		text.Add(0);
		text.AddText(tecname);
		if( odbMode == true )
		{
			text.Add(0);
			AText	odbServerText;
			odbServerText.SetFromOSType(odbServer);
			text.AddText(odbServerText);
			text.Add(0);
			text.AddText(odbSenderToken);
			text.Add(0);
			text.AddText(odbCustomPath);
		}
		
		//内部イベントへキューイング
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(kObjectID_Invalid);//#1078
		ABase::PostToMainInternalEventQueue(kInternalEvent_AppleEventOpenDoc,kObjectID_Invalid,0,text,objectIDArray);
	}
}

#pragma mark ===========================

#pragma mark ---Speech
//#851

/**
テキスト読み上げ開始
*/
- (void)speakText:(NSString*)text
{
	//読み上げ中なら、読み上げを停止
	if( mIsSpeaking == YES )
	{
		[self stopSpeaking];
	}
	//読み上げ開始
	//NSString *voiceID =[[NSSpeechSynthesizer availableVoices] objectAtIndex:[voiceList indexOfSelectedItem]];
	//[mSpeechSynthesizer setVoice:voiceID];
	[mSpeechSynthesizer startSpeakingString:text];
	//読み上げ中フラグをON
	mIsSpeaking = YES;
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
テキスト読み上げ停止
*/
- (void)stopSpeaking
{
	//読み上げ停止
	[mSpeechSynthesizer stopSpeaking];
	//読み上げ中フラグをOFF
	mIsSpeaking = NO;
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
テキスト読み上げ中かどうかを返す
*/
- (BOOL)isSpeaking
{
	return mIsSpeaking;
}

/**
テキスト読み上げ完了時処理
*/
- (void)speechSynthesizer:(NSSpeechSynthesizer *)sender didFinishSpeaking:(BOOL)finishedSpeaking
{
	//読み上げ中フラグをOFF
	mIsSpeaking = NO;
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

#pragma mark ===========================

#pragma mark ---ダミースレッド処理

- (void)dummyMethodForFirstThread:(id)unusedObject
{
	//処理無し
}

#pragma mark ===========================

#pragma mark ---終了処理

/**
アプリケーション終了時の処理
#1102 applicationWillTerminateからapplicationShouldTerminateへ変更、NVI_Quit()をコールするように変更。
*/
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	//dockから終了などの場合、メニューquitを経由せずにここに来るので、quitを実行する。#953
	if( AApplication::GetApplication().NVI_IsWorking() == true )
	{
		if( AApplication::GetApplication().NVI_Quit() == false )
		{
			return NSTerminateCancel;
		}
	}
	//AAppの解放処理
	delete &(AApplication::GetApplication());
	return NSTerminateNow;
}

#pragma mark ===========================

#pragma mark ---タイマー

/**
Tickタイマー開始
*/
- (void)startTickTimer
{
	if( tickTimer == nil )
	{
		tickTimer = [NSTimer scheduledTimerWithTimeInterval:(kTickMsec/1000.0)
				target:self selector:@selector(doTickerTimerAction:) userInfo:nil repeats:YES];
	}
}

/**
Tickタイマー停止
*/
- (void)stopTickTimer
{
	if( tickTimer != nil )
	{
		[tickTimer invalidate];
		tickTimer = nil;
	}
}

/**
Tickタイマー再開
*/
- (void)restartTickTimer
{
	[self stopTickTimer];
	[self startTickTimer];
}

/**
Tickタイマーexpire時処理
*/
- (void)doTickerTimerAction:(NSTimer*)inTimer
{
	OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(true);
	
	//AApplication::EVT_DoTickTimerAction()実行
	AApplication::GetApplication().GetImp().APICB_CocoaDoTickTimerAction();
	
	OS_CALLBACKBLOCK_END;
}

#pragma mark ===========================

#pragma mark ---アプリケーションActivated/Deactivated

/**
アプリケーションActivated
*/
- (void)applicationDidBecomeActive:(NSNotification *)inNotification
{
	OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(false);
	
	//AApplication::EVT_DoAppActivated()実行
	AApplication::GetApplication().GetImp().APICB_CocoaDoBecomeActive();
	
	OS_CALLBACKBLOCK_END;
}

/**
アプリケーションDeactivated
*/
- (void)applicationDidResignActive:(NSNotification *)inNotification
{
	OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(false);
	
	//AApplication::EVT_DoAppDeactivated()実行
	AApplication::GetApplication().GetImp().APICB_CocoaDoResignActive();
	
	OS_CALLBACKBLOCK_END;
}

//#1056
/**
名称未設定ドキュメント生成
*/
- (BOOL)applicationOpenUntitledFile:(NSApplication *)theApplication
{
	OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP_WITHERRRETURNVALUE(false,YES);
	
	AApplication::GetApplication().EVT_OpenUntitledFile();
	
	OS_CALLBACKBLOCK_END;
	return YES;
}

#pragma mark ===========================

#pragma mark ---メニューアクションコールバック

/**
メニュー項目選択時処理
*/
- (IBAction)doMenuActions:(id)sender
{
	OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(false);
	
	//選択したメニュー項目、メニュー、インデックスを取得
	NSMenuItem*	menuItem = ACocoa::CastNSMenuItemFromId(sender);
	NSMenu*	menu = [menuItem menu];
	NSInteger	menuIndex = [menu indexOfItem:menuItem];
	AMenuRef	menuRef = static_cast<AMenuRef>(menu);
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]);
	//CAppImp経由で最前面のAWindow、および、AApplicationに対し、EVT_DoMenuItemSelected()を実行
	AApplication::GetApplication().GetImp().APICB_CocoaDoMenuAction([sender tag],modifier,menuRef,menuIndex);
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
	
	OS_CALLBACKBLOCK_END;
}

/**
メニューenable/disable処理
*/
- (BOOL)validateMenuItem:(NSMenuItem *)item
{
	AMenuItemID	itemID = [item tag];
	NSMenu*	menu = [item menu];
	AIndex	menuItemIndex = [menu indexOfItem:item];
	if( itemID == 0 )
	{
		//ItemIDが0なら常にenable
		return YES;
	}
	else
	{
		//#688
		//動的メニューの場合は、GetDynamicMenuEnableMenuItem()を使用して、各メニュー項目のEnable/Disableを取得する
		ABool	isEnabled = false;
		if( AApplication::GetApplication().NVI_GetMenuManager().GetDynamicMenuEnableMenuItem(itemID,menuItemIndex,isEnabled) == true )
		{
			if( isEnabled == true )
			{
				return YES;
			}
			else
			{
				return NO;
			}
		}
		//MenuManagerのバッファーに設定された現在のenable状態を返す
		if( AApplication::GetApplication().NVI_GetMenuManager().GetBufferredEnableMenuItem(itemID) == true )
		{
			return YES;
		}
		else
		{
			return NO;
		}
	}
}

//#1015
/**
キー処理済みフラグ設定
*/
- (void)setKeyHandled:(BOOL)inKeyHandled
{
	keyHandled = inKeyHandled;
}

//#1015
/**
キー処理済みフラグ取得
*/
- (BOOL)isKeyHandled
{
	return keyHandled;
}

#pragma mark ===========================

#pragma mark ---JavaScript
//#904

/**
JavaScript実行
*/
- (void)doJavaScript:(NSString*)inScript
{
#if BUILD_WITH_JAVASCRIPTCORE
	[mJSWrapper evaluateJavaScript:inScript];
	
	/*NSOperationQueueテスト 
	JavaScriptOperation*	operation = [[JavaScriptOperation alloc] init:inScript withJSWrapper:mJSWrapper];
	[mJSOperationQueue addOperation:operation];
	*/
#endif
}

/*NSOperationQueueテスト 
- (BOOL)executingJavaScript
{
	NSArray*	array = [mJSOperationQueue operations];
	return ([array count] > 0);
}

- (void)cancelExecutingJavaScript
{
	[mJSOperationQueue cancelAllOperations];
}
*/

@end

#if BUILD_WITH_JAVASCRIPTCORE
#pragma mark ===========================

#pragma mark ---JavaScript処理中断チェック

//#1221
/**
JavaScript処理中断チェック
*/
ABool	CheckJavaScriptCancel( JSContextRef ctx, JSValueRef *exception )
{
	//一定時間以上経過していたらプログレスウインドウを表示し、ユーザーからのキャンセルを受け付ける。
	if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_JavaScript,0,true,-1,0) == false )
	{
		//キャンセルボタンをクリックされたら、例外を発生させる。（スクリプト側で処理してもらう。以前はスレッドkillする処理にしていたが、そうするとその後JavaScriptCoreでクラッシュする）
		NSString*	str = @"mi exception:User canceled.";
		*exception = JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str) );
		return true;
	}
	else
	{
		return false;
	}
}


#pragma mark ===========================

#pragma mark ---Documentクラス

/**
*/
void	GetDocumentIDAndWindowIDFromJSObject( JSObjectRef thisObject, ADocumentID& outDocID, AWindowID& outWinID )
{
	outDocID = AObjectID::GetObjectIDFromPtr((AObjectArrayItem*)JSObjectGetPrivate( thisObject ));
	outWinID = kObjectID_Invalid;
	if( outDocID != kObjectID_Invalid )
	{
		outWinID = GetApp().SPI_GetTextDocumentByID(outDocID).SPI_GetFirstWindowID();
	}
}

/**
getContent
テキストの全体長さを取得
*/
static JSValueRef js_doc_getLength( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AIndex	unicodeIndex = 0;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================テキスト取得==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			AIndex	len = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength();
			unicodeIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetUnicodeTextIndexFromTextIndex(len);
		}
	}
	catch(...)
	{
	}
	return JSValueMakeNumber( ctx, unicodeIndex );
}

/**
getParagraphCount
段落数を取得
*/
static JSValueRef js_doc_getParagraphCount( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AIndex	paraCount = 1;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================テキスト取得==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			paraCount = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphCount();
		}
	}
	catch(...)
	{
	}
	return JSValueMakeNumber( ctx, paraCount );
}

/**
getText
指定範囲テキストを取得
*/
static JSValueRef js_doc_getText( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AText	text;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================テキスト取得==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			//
			if ( ( argumentCount >= 2 ) && 
				JSValueIsNumber( ctx, arguments[0] ) && 
				JSValueIsNumber( ctx, arguments[1] ) )
			{
				//
				AIndex	unicodeSpos = JSValueToNumber(ctx,arguments[0],exception);
				AIndex	unicodeEpos = JSValueToNumber(ctx,arguments[1],exception);
				//
				AIndex	spos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextIndexFromUnicodeTextIndex(unicodeSpos);
				AIndex	epos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextIndexFromUnicodeTextIndex(unicodeEpos);
				//
				if( spos <= epos )
				{
					//
					GetApp().SPI_GetTextDocumentByID(docID).SPI_GetText(spos,epos,text);
				}
			}
		}
	}
	catch(...)
	{
	}
	//
	AStCreateNSStringFromAText	str(text);
	return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
}

/**
getSelectionRange
選択範囲取得
*/
static JSValueRef js_doc_getSelectionRange( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	JSObjectRef	obj = JSObjectMake(ctx,NULL,NULL);
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================選択範囲取得==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			if( obj != NULL )
			{
				AIndex	selIndex = 0;
				if ( ( argumentCount >= 1 ) && JSValueIsNumber( ctx, arguments[0] ) )
				{
					//
					selIndex = JSValueToNumber(ctx,arguments[0],exception);
				}
				//
				AArray<AIndex>	sposArray, eposArray;
				GetApp().SPI_GetTextWindowByID(winID).SPI_GetCurrentFocusTextView().SPI_GetSelect(sposArray,eposArray);
				//
				if( selIndex >= 0 && selIndex < sposArray.GetItemCount() )
				{
					//
					AIndex	unicodeSpos = GetApp().SPI_GetTextDocumentByID(docID).
							SPI_GetUnicodeTextIndexFromTextIndex(sposArray.Get(selIndex));
					AIndex	unicodeEpos = GetApp().SPI_GetTextDocumentByID(docID).
							SPI_GetUnicodeTextIndexFromTextIndex(eposArray.Get(selIndex));
					//
					JSStringRef	propertyStart = JSStringCreateWithCFString((CFStringRef)(@"start"));
					JSStringRef	propertyEnd = JSStringCreateWithCFString((CFStringRef)(@"end"));
					JSObjectSetProperty(ctx,obj,propertyStart,JSValueMakeNumber(ctx,unicodeSpos),kJSPropertyAttributeReadOnly, NULL );
					JSObjectSetProperty(ctx,obj,propertyEnd,JSValueMakeNumber(ctx,unicodeEpos),kJSPropertyAttributeReadOnly, NULL );
					JSStringRelease(propertyStart);
					JSStringRelease(propertyEnd);
				}
			}
		}
	}
	catch(...)
	{
	}
	return obj;
}

/**
getParagraphRange
段落範囲取得
*/
static JSValueRef js_doc_getParagraphRange( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	JSObjectRef	obj = JSObjectMake(ctx,NULL,NULL);
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================選択範囲取得==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			if( obj != NULL )
			{
				AIndex	paraIndex = 0;
				if ( ( argumentCount >= 1 ) && JSValueIsNumber( ctx, arguments[0] ) )
				{
					//
					paraIndex = JSValueToNumber(ctx,arguments[0],exception);
				}
				//
				if( paraIndex >= 0 && paraIndex < GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphCount() )
				{
					//
					AIndex	spos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphStartTextIndex(paraIndex);
					AIndex	epos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphStartTextIndex(paraIndex+1);
					//
					AIndex	unicodeSpos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetUnicodeTextIndexFromTextIndex(spos);
					AIndex	unicodeEpos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetUnicodeTextIndexFromTextIndex(epos);
					//
					JSStringRef	propertyStart = JSStringCreateWithCFString((CFStringRef)(@"start"));
					JSStringRef	propertyEnd = JSStringCreateWithCFString((CFStringRef)(@"end"));
					JSObjectSetProperty(ctx,obj,propertyStart,JSValueMakeNumber(ctx,unicodeSpos),kJSPropertyAttributeReadOnly, NULL );
					JSObjectSetProperty(ctx,obj,propertyEnd,JSValueMakeNumber(ctx,unicodeEpos),kJSPropertyAttributeReadOnly, NULL );
					JSStringRelease(propertyStart);
					JSStringRelease(propertyEnd);
				}
			}
		}
	}
	catch(...)
	{
	}
	return obj;
}

/**
getTextPointFromPos
posからx,yを取得
*/
static JSValueRef js_doc_getTextPointFromPos( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	JSObjectRef	obj = JSObjectMake(ctx,NULL,NULL);
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================選択範囲取得==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			if( obj != NULL )
			{
				AIndex	unicodeIndex = 0;
				if ( ( argumentCount >= 1 ) && JSValueIsNumber( ctx, arguments[0] ) )
				{
					//
					unicodeIndex = JSValueToNumber(ctx,arguments[0],exception);
				}
				//
				AIndex	pos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextIndexFromUnicodeTextIndex(unicodeIndex);
				//
				ATextPoint	textpt = {0};
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextPointFromTextIndex(pos,textpt);
				//
				AIndex	paraIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphIndexByLineIndex(textpt.y);
				AIndex	paraStart = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphStartTextIndex(paraIndex);
				AIndex	colIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetUnicodeTextIndexFromTextIndex(pos) - 
						GetApp().SPI_GetTextDocumentByID(docID).SPI_GetUnicodeTextIndexFromTextIndex(paraStart);
				//
				JSStringRef	propertyX = JSStringCreateWithCFString((CFStringRef)(@"x"));
				JSStringRef	propertyY = JSStringCreateWithCFString((CFStringRef)(@"y"));
				JSObjectSetProperty(ctx,obj,propertyX,JSValueMakeNumber(ctx,colIndex),kJSPropertyAttributeReadOnly, NULL );
				JSObjectSetProperty(ctx,obj,propertyY,JSValueMakeNumber(ctx,paraIndex),kJSPropertyAttributeReadOnly, NULL );
				JSStringRelease(propertyX);
				JSStringRelease(propertyY);
			}
		}
	}
	catch(...)
	{
	}
	return obj;
}

/**
getPosFromTextPoint
posからx,yを取得
*/
static JSValueRef js_doc_getPosFromTextPoint( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AIndex	unicodeIndex = 0;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================選択範囲取得==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			//
			if ( ( argumentCount >= 2 ) && 
				JSValueIsNumber( ctx, arguments[0] ) && 
				JSValueIsNumber( ctx, arguments[1] ) )
			{
				//
				AIndex	col = JSValueToNumber(ctx,arguments[0],exception);
				AIndex	para = JSValueToNumber(ctx,arguments[1],exception);
				//
				AIndex	paraStart = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphStartTextIndex(para);
				unicodeIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetUnicodeTextIndexFromTextIndex(paraStart) + col;
			}
		}
	}
	catch(...)
	{
	}
	return JSValueMakeNumber( ctx, unicodeIndex );
}

/**
getSelectionCount
選択範囲の数取得（通常選択時は常に1、矩形選択の場合のみ2以上）
*/
static JSValueRef js_doc_getSelectionCount( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AItemCount	selectionCount = 1;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================選択範囲の数取得==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			//
			AArray<AIndex>	sposArray, eposArray;
			GetApp().SPI_GetTextWindowByID(winID).SPI_GetCurrentFocusTextView().SPI_GetSelect(sposArray,eposArray);
			//
			selectionCount = sposArray.GetItemCount();
		}
	}
	catch(...)
	{
	}
	return JSValueMakeNumber( ctx, selectionCount );
}

/**
setSelectionRange
選択範囲設定
*/
static JSValueRef js_doc_setSelectionRange( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================選択範囲設定==================
		//
		if( winID != kObjectID_Invalid && docID != kObjectID_Invalid )
		{
			if ( ( argumentCount >= 1 ) && JSValueIsNumber( ctx, arguments[0] ) )
			{
				//
				AIndex	unicodeSpos = JSValueToNumber(ctx,arguments[0],exception);
				AIndex	unicodeEpos = unicodeSpos;
				if( argumentCount >= 2 && JSValueIsNumber( ctx, arguments[1] ) )
				{
					unicodeEpos = JSValueToNumber(ctx,arguments[1],exception);
				}
				//
				AIndex	spos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextIndexFromUnicodeTextIndex(unicodeSpos);
				AIndex	epos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextIndexFromUnicodeTextIndex(unicodeEpos);
				//
				if( spos <= epos )
				{
					GetApp().SPI_GetTextWindowByID(winID).SPI_GetCurrentFocusTextView().SPI_SetSelect(spos,epos);
					//
					result = true;
				}
			}
		}
	}
	catch(...)
	{
	}
	//
	return JSValueMakeBoolean( ctx, result );
}

/**
setText
指定範囲テキストを設定（置換）
*/
static JSValueRef js_doc_setText( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================テキスト置換==================
		//
		if( winID != kObjectID_Invalid && docID != kObjectID_Invalid )
		{
			if ( ( argumentCount >= 3 ) && 
				JSValueIsNumber( ctx, arguments[0] ) && 
				JSValueIsNumber( ctx, arguments[1] ) &&
				JSValueIsString( ctx, arguments[2] ) )
			{
				//
				AIndex	unicodeSpos = JSValueToNumber(ctx,arguments[0],exception);
				AIndex	unicodeEpos = JSValueToNumber(ctx,arguments[1],exception);
				NSString*	str = [NSString stringWithJSValue:arguments[2] fromContext:ctx];
				AText	text;
				ACocoa::SetTextFromNSString(str,text);
				//
				AIndex	spos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextIndexFromUnicodeTextIndex(unicodeSpos);
				AIndex	epos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextIndexFromUnicodeTextIndex(unicodeEpos);
				//
				if( spos <= epos )
				{
					GetApp().SPI_GetTextDocumentByID(docID).SPI_DeleteText(spos,epos);
					GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(spos,text);
					//
					result = true;
				}
			}
		}
	}
	catch(...)
	{
	}
	//
	return JSValueMakeBoolean( ctx, result );
}

/**
setDocInfoText
*/
static JSValueRef js_doc_setDocInfoText( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================テキスト置換==================
		//
		if( winID != kObjectID_Invalid && docID != kObjectID_Invalid )
		{
			if ( ( argumentCount >= 1 ) && JSValueIsString( ctx, arguments[0] ) )
			{
				//
				NSString*	str = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
				AText	text;
				ACocoa::SetTextFromNSString(str,text);
				//
				GetApp().SPI_UpdateDocInfoWindows_PluginText(winID,text);
				result = true;
			}
		}
	}
	catch(...)
	{
	}
	//
	return JSValueMakeBoolean( ctx, result );
}

/**
getSyntaxStateNameFromPos
*/
static JSValueRef js_doc_getSyntaxStateNameFromPos( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AText	stateText, pushedStateText;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================state取得==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			AIndex	unicodeIndex = 0;
			if ( ( argumentCount >= 1 ) && JSValueIsNumber( ctx, arguments[0] ) )
			{
				//
				unicodeIndex = JSValueToNumber(ctx,arguments[0],exception);
			}
			//
			AIndex	pos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextIndexFromUnicodeTextIndex(unicodeIndex);
			//
			ATextPoint	textpt = {0};
			GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextPointFromTextIndex(pos,textpt);
			//
			AIndex	stateIndex = kIndex_Invalid;
			AIndex	pushedStateIndex = kIndex_Invalid;
			GetApp().SPI_GetTextDocumentByID(docID).SPI_GetCurrentStateIndexAndName(textpt,stateIndex,pushedStateIndex,stateText,pushedStateText);
	}
	}
	catch(...)
	{
	}
	//
	AStCreateNSStringFromAText	str(stateText);
	return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
}

/**
getModeName
*/
static JSValueRef js_doc_getModeName( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AText	modeName;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================モード名取得==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeRawName(modeName);
		}
	}
	catch(...)
	{
	}
	//返り値設定
	AStCreateNSStringFromAText	str(modeName);
	return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
}

/**
getFileURL
*/
static JSValueRef js_doc_getURL( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AText	url;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================URL取得==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextDocumentByID(docID).SPI_GetURL(url);
		}
	}
	catch(...)
	{
	}
	//返り値設定
	AStCreateNSStringFromAText	str(url);
	return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
}

/**
clearSyntaxCheckerWarnings
*/
static JSValueRef js_doc_clearSyntaxCheckerWarnings( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================チェッカーワーニング全クリア==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextDocumentByID(docID).SPI_ClearCheckerPluginWarnings();
			result = true;
		}
	}
	catch(...)
	{
	}
	//返り値設定
	return JSValueMakeBoolean( ctx, result );
}

/**
addSyntaxCheckerWarning
*/
static JSValueRef js_doc_addSyntaxCheckerWarning( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		//==================ドキュメント・ウインドウオブジェクト取得==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================チェッカーワーニング追加==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			if ( ( argumentCount >= 4 ) && 
				 JSValueIsNumber( ctx, arguments[0] ) &&
				 JSValueIsNumber( ctx, arguments[1] ) &&
				 JSValueIsString( ctx, arguments[2] ) &&
				 JSValueIsString( ctx, arguments[3] ) )
			{
				//段落番号取得
				AIndex	paraIndex = JSValueToNumber(ctx,arguments[0],exception);
				//列取得
				AIndex	colIndex = JSValueToNumber(ctx,arguments[1],exception);
				//ワーニングタイトル取得
				NSString*	str1 = [NSString stringWithJSValue:arguments[2] fromContext:ctx];
				AText	title;
				ACocoa::SetTextFromNSString(str1,title);
				//ワーニング詳細取得
				NSString*	str2 = [NSString stringWithJSValue:arguments[3] fromContext:ctx];
				AText	detail;
				ACocoa::SetTextFromNSString(str2,detail);
				//ワーニングタイプ取得
				AIndex	typeIndex = kIndex_Invalid;
				if ( ( argumentCount >= 5 ) && 
					 JSValueIsNumber( ctx, arguments[4] ) )
				{
					typeIndex = JSValueToNumber(ctx,arguments[4],exception);
				}
				//ワーニングをTextViewに表示するかどうかを取得
				ABool	displayInTextView = false;
				if ( ( argumentCount >= 6 ) && 
					 JSValueIsBoolean( ctx, arguments[5] ) )
				{
					displayInTextView = JSValueToBoolean(ctx,arguments[5]);
				}
				//ワーニング追加
				GetApp().SPI_GetTextDocumentByID(docID).
						SPI_AddCheckerPluginWarning(paraIndex,colIndex,title,detail,typeIndex,displayInTextView);
				result = true;
			}
		}
	}
	catch(...)
	{
	}
	//返り値設定
	return JSValueMakeBoolean( ctx, result );
}

/**
*/
static JSStaticValue docStaticValues[] = 
{
	{ 0, 0, 0, 0 }
};

/**
*/
static JSStaticFunction docStaticFunctions[] = 
{
			{ "getLength", js_doc_getLength, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
			{ "getParagraphCount", js_doc_getParagraphCount, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
			{ "getSelectionRange", js_doc_getSelectionRange, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
			{ "getSelectionCount", js_doc_getSelectionCount, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
			{ "getText", js_doc_getText, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
			{ "getTextPointFromPos", js_doc_getTextPointFromPos, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
			{ "getPosFromTextPoint", js_doc_getPosFromTextPoint, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
			{ "getParagraphRange", js_doc_getParagraphRange, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
			{ "setSelectionRange", js_doc_setSelectionRange, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
			{ "setText", js_doc_setText, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
	//
			{ "setDocInfoText", js_doc_setDocInfoText, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
	//
			{ "getSyntaxStateNameFromPos", js_doc_getSyntaxStateNameFromPos, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
 //
			{ "getModeName", js_doc_getModeName, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
			{ "getURL", js_doc_getURL, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
 //
			{ "clearSyntaxCheckerWarnings",js_doc_clearSyntaxCheckerWarnings, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
			{ "addSyntaxCheckerWarning",js_doc_addSyntaxCheckerWarning, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
			{ 0, 0, 0 }
};


/**
*/
JSClassRef DocClass( void )
{
	static JSClassRef docClass = NULL;
	if ( docClass == NULL )
	{
		JSClassDefinition docClassDefinition = kJSClassDefinitionEmpty;
		docClassDefinition.staticValues = docStaticValues;
		docClassDefinition.staticFunctions = docStaticFunctions;
		docClass = JSClassCreate( &docClassDefinition );
    }
    return docClass;
}

#pragma mark ===========================

#pragma mark ---Applicationクラス

/**
現在のドキュメントを取得
*/
static JSValueRef js_app_getCurrentDocument( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	try
	{
		AWindowID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
		if( winID != kObjectID_Invalid )
		{
			//
			ADocumentID	docID = GetApp().SPI_GetTextWindowByID(winID).SPI_GetCurrentFocusTextDocument().GetObjectID();
			//
			JSObjectRef	obj = JSObjectMake(ctx,DocClass(),docID.val);
			return obj;
		}
	}
	catch(...)
	{
	}
	return JSValueMakeNull(ctx);
}

/**
新規ドキュメントを生成
*/
static JSValueRef js_app_createDocument( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	try
	{
		ADocumentID	docID = GetApp().SPNVI_CreateNewTextDocument();
		//
		JSObjectRef	obj = JSObjectMake(ctx,DocClass(),docID.val);
		return obj;
	}
	catch(...)
	{
	}
	return JSValueMakeNull(ctx);
}

//#994
/**
ドキュメントUniqIDからドキュメントオブジェクト取得
*/
static JSValueRef js_app_getDocumentByID( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	try
	{
		ADocumentID	docID = kObjectID_Invalid;
		if ( ( argumentCount >= 1 ) && JSValueIsString( ctx, arguments[0] ) )
		{
			//ドキュメントUniqID取得
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	docUniqID;
			ACocoa::SetTextFromNSString(str1,docUniqID);
			//UniqIDからドキュメントのObjectIDを取得
			AUniqID	uniqID = {docUniqID.GetNumber()};
			docID = GetApp().NVI_GetDocumentIDByUniqID(uniqID);
		}
		//ドキュメントオブジェクト取得
		if( docID != kObjectID_Invalid )
		{
			JSObjectRef	obj = JSObjectMake(ctx,DocClass(),docID.val);
			return obj;
		}
	}
	catch(...)
	{
	}
	return JSValueMakeNull(ctx);
}

/**
JSモジュールをロード
*/
static JSValueRef js_app_loadModule( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	try
	{
		if ( ( argumentCount >= 1 ) && JSValueIsString( ctx, arguments[0] ) )
		{
			//
			NSString*	str = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	modulename;
			ACocoa::SetTextFromNSString(str,modulename);
			//
			AApplication::GetApplication().NVI_LoadJavaScriptModule(modulename);
		}
		return JSValueMakeNull(ctx);
	}
	catch(...)
	{
	}
	return JSValueMakeNull(ctx);
}

/**
プラグイン登録
*/
static JSValueRef js_registerPlugin( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		if ( ( argumentCount >= 5 ) && 
			JSValueIsString( ctx, arguments[0] ) &&
			JSValueIsString( ctx, arguments[1] ) &&
			JSValueIsString( ctx, arguments[2] ) &&
			 JSValueIsBoolean( ctx, arguments[3] ) &&
			 JSValueIsString( ctx, arguments[4] ) )
		{
			//対象モード名
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	modeName;
			ACocoa::SetTextFromNSString(str1,modeName);
			//プラグインID
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	pluginID;
			ACocoa::SetTextFromNSString(str2,pluginID);
			//プラグイン名
			NSString*	str3 = [NSString stringWithJSValue:arguments[2] fromContext:ctx];
			AText	pluginName;
			ACocoa::SetTextFromNSString(str3,pluginName);
			//デフォルトでEnableかどうか
			ABool	defaultEnabled = JSValueToBoolean(ctx,arguments[3]);
			//デフォルトオプション
			NSString*	str4 = [NSString stringWithJSValue:arguments[4] fromContext:ctx];
			AText	options;
			ACocoa::SetTextFromNSString(str4,options);
			//モード名からモードを取得
			AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modeName);
			if( modeIndex != kIndex_Invalid )
			{
				//プラグイン登録
				GetApp().SPI_GetModePrefDB(modeIndex).RegisterPlugin(pluginID,pluginName,defaultEnabled,options);
				result = true;
			}
		}
	}
	catch(...)
	{
	}
	return JSValueMakeBoolean( ctx, result );
}

/**
プラグイン表示名設定
*/
static JSValueRef js_setPluginDisplayName( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		if ( ( argumentCount >= 4 ) && 
			JSValueIsString( ctx, arguments[0] ) &&
			JSValueIsString( ctx, arguments[1] ) &&
			JSValueIsString( ctx, arguments[2] ) &&
			JSValueIsString( ctx, arguments[3] ) )
		{
			//対象モード名
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	modeName;
			ACocoa::SetTextFromNSString(str1,modeName);
			//プラグインID
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	pluginID;
			ACocoa::SetTextFromNSString(str2,pluginID);
			//言語名
			NSString*	str3 = [NSString stringWithJSValue:arguments[2] fromContext:ctx];
			AText	langName;
			ACocoa::SetTextFromNSString(str3,langName);
			//プラグイン表示名
			NSString*	str4 = [NSString stringWithJSValue:arguments[3] fromContext:ctx];
			AText	displayName;
			ACocoa::SetTextFromNSString(str4,displayName);
			//モード名からモードを取得
			AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modeName);
			if( modeIndex != kIndex_Invalid )
			{
				//プラグイン登録
				GetApp().SPI_GetModePrefDB(modeIndex).SetPluginDisplayName(pluginID,langName,displayName);
				result = true;
			}
		}
	}
	catch(...)
	{
	}
	return JSValueMakeBoolean( ctx, result );
}

/**
プラグインEnabled取得
*/
static JSValueRef js_getPluginEnabled( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		if ( ( argumentCount >= 2 ) && 
			JSValueIsString( ctx, arguments[0] ) &&
			JSValueIsString( ctx, arguments[1] )  )
		{
			//対象モード名
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	modeName;
			ACocoa::SetTextFromNSString(str1,modeName);
			//プラグインID
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	pluginID;
			ACocoa::SetTextFromNSString(str2,pluginID);
			//モード取得
			AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modeName);
			if( modeIndex != kIndex_Invalid )
			{
				//プラグインEnabled状態取得
				result = GetApp().SPI_GetModePrefDB(modeIndex).GetPluginEnabled(pluginID);
			}
		}
	}
	catch(...)
	{
	}
	return JSValueMakeBoolean( ctx, result );
}

/**
プラグインオプション取得
*/
static JSValueRef js_getPluginOptions( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AText	options;
	try
	{
		if ( ( argumentCount >= 2 ) && 
			JSValueIsString( ctx, arguments[0] ) &&
			JSValueIsString( ctx, arguments[1] )  )
		{
			//対象モード名
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	modeName;
			ACocoa::SetTextFromNSString(str1,modeName);
			//プラグインID
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	pluginID;
			ACocoa::SetTextFromNSString(str2,pluginID);
			//モード取得
			AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modeName);
			if( modeIndex != kIndex_Invalid )
			{
				//プラグインオプション取得
				 GetApp().SPI_GetModePrefDB(modeIndex).GetPluginOptions(pluginID,options);
			}
		}
	}
	catch(...)
	{
	}
	//返り値設定
	AStCreateNSStringFromAText	str(options);
	return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
}

/**
プラグインロード中のモード名取得
*/
static JSValueRef js_getCurrentModeNameForPluginLoad( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//モード名取得
	AText	modeName;
	GetApp().SPI_GetCurrentModeNameForPluginLoad(modeName);
	//返り値設定
	AStCreateNSStringFromAText	str(modeName);
	return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
}

//#1217
/**
ダイアログ表示
*/
static JSValueRef js_showDialog( JSContextRef ctx, JSObjectRef function,
										 JSObjectRef thisObject, size_t argumentCount,
										 const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	try
	{
		if ( ( argumentCount >= 4 ) && 
			 JSValueIsString( ctx, arguments[0] ) &&
			 JSValueIsString( ctx, arguments[1] ) &&
			 JSValueIsString( ctx, arguments[2] ) &&
			 JSValueIsString( ctx, arguments[3] )  )
		{
			//タイトル取得
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	title;
			ACocoa::SetTextFromNSString(str1,title);
			//メッセージ取得
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	message;
			ACocoa::SetTextFromNSString(str2,message);
			//OKボタン
			NSString*	str3 = [NSString stringWithJSValue:arguments[2] fromContext:ctx];
			AText	okButton;
			ACocoa::SetTextFromNSString(str3,okButton);
			//キャンセルボタン
			NSString*	str4 = [NSString stringWithJSValue:arguments[3] fromContext:ctx];
			AText	cancelButton;
			ACocoa::SetTextFromNSString(str4,cancelButton);
			//ダイアログ表示
			ABool	aborted = false;
			ABool	ok = GetApp().SPI_ShowJavaScriptModalCancelAlertWindow(title,message,okButton,cancelButton,aborted);
			if( aborted == true )
			{
				NSString*	str = @"mi exception:User canceled.";
				*exception = JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str) );
				return JSValueMakeNull(ctx);
			}
			if( ok == true )
			{
				//OK時
				AStCreateNSStringFromAText	str(okButton);
				return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
			}
			else
			{
				//Cancel時
				AStCreateNSStringFromAText	str(cancelButton);
				return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
			}
		}
		else if ( ( argumentCount >= 3 ) && 
				  JSValueIsString( ctx, arguments[0] ) &&
				  JSValueIsString( ctx, arguments[1] ) &&
				  JSValueIsString( ctx, arguments[2] )  )
		{
			//タイトル取得
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	title;
			ACocoa::SetTextFromNSString(str1,title);
			//メッセージ取得
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	message;
			ACocoa::SetTextFromNSString(str2,message);
			//OKボタン
			NSString*	str3 = [NSString stringWithJSValue:arguments[2] fromContext:ctx];
			AText	okButton;
			ACocoa::SetTextFromNSString(str3,okButton);
			//ダイアログ表示
			ABool	aborted = false;
			GetApp().SPI_ShowJavaScriptModalAlertWindow(title,message,okButton,aborted);
			if( aborted == true )
			{
				NSString*	str = @"mi exception:User canceled.";
				*exception = JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str) );
				return JSValueMakeNull(ctx);
			}
			AStCreateNSStringFromAText	str(okButton);
			return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
		}
		else if ( ( argumentCount >= 2 ) && 
				  JSValueIsString( ctx, arguments[0] ) &&
				  JSValueIsString( ctx, arguments[1] )  )
		{
			//タイトル取得
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	title;
			ACocoa::SetTextFromNSString(str1,title);
			//メッセージ取得
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	message;
			ACocoa::SetTextFromNSString(str2,message);
			//ダイアログ表示
            AText   okButton("OK");
			ABool	aborted = false;
			GetApp().SPI_ShowJavaScriptModalAlertWindow(title,message,okButton,aborted);
			if( aborted == true )
			{
				NSString*	str = @"mi exception:User canceled.";
				*exception = JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str) );
				return JSValueMakeNull(ctx);
			}
			return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)@"OK") );
		}
		else return JSValueMakeNull(ctx);
	}
	catch(...)
	{
	}
	return JSValueMakeNull(ctx);
}

/**
標準エラー出力
*/
static JSValueRef js_app_outputToStderr( JSContextRef ctx, JSObjectRef function,
										JSObjectRef thisObject, size_t argumentCount,
										const JSValueRef arguments[], JSValueRef *exception )
{
	//キャンセルポイント
	//#1221 pthread_testcancel();
	//ユーザーキャンセルチェック（ユーザーがキャンセルしたら例外発生させる）#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	try
	{
		if ( ( argumentCount >= 1 ) && JSValueIsString( ctx, arguments[0] ) )
		{
			//テキスト取得
			NSString*	str = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	text;
			ACocoa::SetTextFromNSString(str,text);
			//標準エラーに出力
			//#1215 NSLog()へ変更
			//text.OutputToStderr();
			NSLog(str);
		}
		return JSValueMakeNull(ctx);
	}
	catch(...)
	{
	}
	return JSValueMakeNull(ctx);
}

/**
*/
static JSStaticValue appStaticValues[] = 
{
	{ 0, 0, 0, 0 }
};

/**
*/
static JSStaticFunction appStaticFunctions[] = 
		{
		 { "getCurrentDocument", js_app_getCurrentDocument, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 { "createDocument", js_app_createDocument, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 { "getDocumentByID", js_app_getDocumentByID, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 { "loadModule", js_app_loadModule, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 //プラグイン
		 { "registerPlugin", js_registerPlugin, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 { "setPluginDisplayName", js_setPluginDisplayName, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 { "getPluginEnabled", js_getPluginEnabled, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 { "getPluginOptions", js_getPluginOptions, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 { "getCurrentModeNameForPluginLoad", js_getCurrentModeNameForPluginLoad, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 //ダイアログ表示
		 { "showDialog", js_showDialog, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 //デバッグ
		 { "outputToStderr", js_app_outputToStderr, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 { 0, 0, 0 }
		 };

/**
*/
JSClassRef AppClass( void )
{
	static JSClassRef appClass = NULL;
	if ( appClass == NULL )
	{
		JSClassDefinition appClassDefinition = kJSClassDefinitionEmpty;
		appClassDefinition.staticValues = appStaticValues;
		appClassDefinition.staticFunctions = appStaticFunctions;
		appClass = JSClassCreate( &appClassDefinition );
    }
    return appClass;
}

#endif

//NSOperationテスト
#if 0
#pragma mark ===========================

#pragma mark ---

#if BUILD_WITH_JAVASCRIPTCORE

@implementation JavaScriptOperation

- (id)init:(NSString*)inScript withJSWrapper:(JSWrappers*)inJSWrapper
{
	self = [super init];
	if( self )
	{
		executing = NO;
		finished = NO;
		//
		mJSWrapper = inJSWrapper;
		mScript = [NSString stringWithString:inScript];
	}
	return self;
}

- (BOOL)isConcurrent 
{
	return YES;
}

- (BOOL)isExecuting 
{
	return executing;
}

- (BOOL)isFinished 
{
	return finished;
}

- (void)start 
{
	// Always check for cancellation before launching the task.
	if ([self isCancelled])
	{
		// Must move the operation to the finished state if it is canceled.
		[self willChangeValueForKey:@"isFinished"];
		finished = YES;
		[self didChangeValueForKey:@"isFinished"];
		return;
	}
	
	// If the operation is not canceled, begin executing the task.
	[self willChangeValueForKey:@"isExecuting"];
	[NSThread detachNewThreadSelector:@selector(main) toTarget:self withObject:nil];
	executing = YES;
	[self didChangeValueForKey:@"isExecuting"];
}

/**
*/
- (void)main 
{
	@try 
	{
		//Execute JavaScript using JSEvaluateScript
		[mJSWrapper evaluateJavaScript:mScript];
		
		[self completeOperation];
	}
	@catch(...) 
	{
		// Do not rethrow exceptions.
	}
}

/**
*/
- (void)completeOperation 
{
	[self willChangeValueForKey:@"isFinished"];
	[self willChangeValueForKey:@"isExecuting"];
	
	executing = NO;
	finished = YES;
	
	[self didChangeValueForKey:@"isExecuting"];
	[self didChangeValueForKey:@"isFinished"];
}

@end

#endif
#endif

