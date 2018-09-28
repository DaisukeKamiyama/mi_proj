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
#pragma mark [�N���X]AppDelegate
#pragma mark ===========================

@implementation AppDelegate

#pragma mark ===========================

#pragma mark ---�����ݒ�

/**
�����ݒ�
*/
- (id)init
{
	//�p���������s
	self = [super init];
	
	if( self != nil )
	{
		//�����ݒ�
		tickTimer = nil;
		keyHandled = NO;//#1015
	}
	
	return self;
}

/*#1203 ��U�ۗ�
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
/*#1203 ��U�ۗ�
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
���C��Nib�ǂݍ��݊������̏���
*/
- (void)awakeFromNib
{
	try
	{
		//�}���`�X���b�h�A�v���ł��邱�Ƃ�OS�ɔF�������邽�߂ɁA�_�~�[��Cocoa�X���b�h�𐶐�
		[NSThread detachNewThreadSelector:@selector(dummyMethodForFirstThread:) toTarget:self withObject:nil];
		if( [NSThread isMultiThreaded] == NO )
		{
			_ACError("",NULL);
		}
		
		//#1262 #1263 OS�������Ń^�u�֘A���j���[��ǉ����Ȃ��悤�ɂ���
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_12 )
		{
			[NSWindow setAllowsAutomaticWindowTabbing:NO];
		}
		
		//���C�����j���[�̏�����
		NSMenu*	mainmenu = [NSApp mainMenu];
		ACocoa::InitMenuActionAndTarget(mainmenu);
		//AApp�I�u�W�F�N�g����
		new AApp;
#if BUILD_WITH_JAVASCRIPTCORE
		//NSOperationQueue�e�X�g mJSOperationQueue = [[NSOperationQueue alloc] init];
		
		//JavaScript�R���e�L�X�g�擾
		mJSContext = JSGlobalContextCreate( NULL );
		
		//JSWrapper�I�u�W�F�N�g����
		mJSWrapper = [[JSWrappers alloc] initWithContext:mJSContext];
		
		//�O���[�o���ϐ�version, program�Ƀo���h������ݒ�
		[mJSWrapper addGlobalStringProperty:@"version" withValue:
		[[NSBundle mainBundle] objectForInfoDictionaryKey: (NSString*) kCFBundleVersionKey]];
		[mJSWrapper addGlobalStringProperty:@"program" withValue:
		[[NSBundle mainBundle] objectForInfoDictionaryKey: (NSString*) kCFBundleNameKey]];
		
		//�O���[�o���֐�messagebox��`
		[mJSWrapper addGlobalFunctionProperty:@"messagebox" withCallback: MessagBoxFunction];
		
		//�O���[�o���I�u�W�F�N�gmiimp��ǉ�
		[mJSWrapper addGlobalObject:@"miimp" ofClass:AppClass() withPrivateData:self];
		
		//Document�N���X�𐶐����Ă���
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
		
		//�A�v���P�[�V���������s��Ԃɂ���
		AApplication::GetApplication().NVI_Run();
	}
	catch(...)
	{
		fprintf(stderr,"AppDelegate::awakeFromNib catched throw");
	}
	
	/*AE�n���h��64bit�ڍs�e�X�g�iAEResolve���͂ǂ�����H�j
	[[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
		andSelector:@selector(handleAppleEvent:withReplyEvent:)
		forEventClass:'core'
	andEventID:'getd'];
	*/
	
	//URL�X�L�[�� #1003
	[[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
		andSelector:@selector(handleAppleEvent:withReplyEvent:)
		forEventClass:'GURL'
	andEventID:'GURL'];
	
	//�R�}���h���C������̑��d�N�����̈����󂯎��p�n���h�� #1034
	[[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
		andSelector:@selector(handleAppleEvent:withReplyEvent:)
		forEventClass:'misc'
	andEventID:'args'];
	
	//#851
	//Speech�I�u�W�F�N�g������
	mSpeechSynthesizer = [[NSSpeechSynthesizer alloc] initWithVoice:nil];//�V�X�e���ݒ�Őݒ肵���f�t�H���g�{�C�X���ݒ肳��� https://developer.apple.com/library/prerelease/content/documentation/Cocoa/Conceptual/Speech/Articles/SynthesizeSpeech.html
	[mSpeechSynthesizer setDelegate:self];
	//mSpeechVoiceList = [NSSpeechSynthesizer availableVoices];
	mIsSpeaking = NO;
}

/**
AE�n���h��
*/
- (void)handleAppleEvent:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent
{
	AEEventClass eventClass = [event eventClass];
	AEEventID eventID = [event eventID];
	
	//URL�X�L�[�� #1003
	if( eventClass == 'GURL' && eventID == 'GURL' )
	{
		NSString *URLString = [[event paramDescriptorForKeyword:keyDirectObject] stringValue];
		AText	uri;
		ACocoa::SetTextFromNSString(URLString,uri);
		AApplication::GetApplication().NVI_ExecuteURIScheme(uri);
	}
	//�R�}���h���C������̑��d�N�����̈����󂯎��p�n���h�� #1034
	if( eventClass == 'misc' && eventID == 'args' )
	{
		NSString *str = [[event paramDescriptorForKeyword:keyDirectObject] stringValue];
		AText	text;
		ACocoa::SetTextFromNSString(str,text);
		//TextArray�ɍ\�����Ĉ������s
		ATextArray	argsArray;
		for( AIndex pos = 0; pos < text.GetItemCount(); )
		{
			AText	line;
			text.GetLine(pos,line);
			argsArray.Add(line);
		}
		//�������s
		AApplication::GetApplication().NVI_ExecCommandLineArgs(argsArray);
	}
}

//#1317 TextDocument/initWithContentsOfURL��AppeEvent���擾�ł��Ȃ����̑΍�
/**
open Apple Event�̏���
https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/ScriptableCocoaApplications/SApps_handle_AEs/SAppsHandleAEs.html#//apple_ref/doc/uid/20001239
*/
- (void)application:(NSApplication *)sender openFiles:(NSArray<NSString *> *)filenames
{
	for( NSString* filename in filenames )
	{
		//==================�e�L�X�g�G���R�[�f�B���O�擾==================
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
		
		//==================�h�L�������g����==================
		AText	path;
		path.SetFromNSString(filename);
		AFileAcc	file;
		file.Specify(path);
		//#1006 �����Œ��ڃh�L�������g�����͂����A�����C�x���g�փL���[�C���O���Ƃɂ��� ADocumentID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(path,tecname);
		//==================ODB==================
		//#1006
		//ODB�f�[�^�擾
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
				//#1006 �����Œ��ڃh�L�������g�����͂����A�����C�x���g�փL���[�C���O���Ƃɂ��� GetApp().SPI_GetTextDocumentByID(docID).SPI_SetODBMode(odbServer,odbSenderToken,odbCustomPath);
				odbMode = true;
			}
		}
		
		//�h�L�������g�������邽�߂̓����C�x���g���쐬
		//�t�@�C���p�X�擾
		AText	filepath;
		file.GetPath(filepath);
		//NULL������؂�Ńh�L�������g����text�Ɋi�[
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
		
		//�����C�x���g�փL���[�C���O
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(kObjectID_Invalid);//#1078
		ABase::PostToMainInternalEventQueue(kInternalEvent_AppleEventOpenDoc,kObjectID_Invalid,0,text,objectIDArray);
	}
}

#pragma mark ===========================

#pragma mark ---Speech
//#851

/**
�e�L�X�g�ǂݏグ�J�n
*/
- (void)speakText:(NSString*)text
{
	//�ǂݏグ���Ȃ�A�ǂݏグ���~
	if( mIsSpeaking == YES )
	{
		[self stopSpeaking];
	}
	//�ǂݏグ�J�n
	//NSString *voiceID =[[NSSpeechSynthesizer availableVoices] objectAtIndex:[voiceList indexOfSelectedItem]];
	//[mSpeechSynthesizer setVoice:voiceID];
	[mSpeechSynthesizer startSpeakingString:text];
	//�ǂݏグ���t���O��ON
	mIsSpeaking = YES;
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�e�L�X�g�ǂݏグ��~
*/
- (void)stopSpeaking
{
	//�ǂݏグ��~
	[mSpeechSynthesizer stopSpeaking];
	//�ǂݏグ���t���O��OFF
	mIsSpeaking = NO;
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�e�L�X�g�ǂݏグ�����ǂ�����Ԃ�
*/
- (BOOL)isSpeaking
{
	return mIsSpeaking;
}

/**
�e�L�X�g�ǂݏグ����������
*/
- (void)speechSynthesizer:(NSSpeechSynthesizer *)sender didFinishSpeaking:(BOOL)finishedSpeaking
{
	//�ǂݏグ���t���O��OFF
	mIsSpeaking = NO;
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

#pragma mark ===========================

#pragma mark ---�_�~�[�X���b�h����

- (void)dummyMethodForFirstThread:(id)unusedObject
{
	//��������
}

#pragma mark ===========================

#pragma mark ---�I������

/**
�A�v���P�[�V�����I�����̏���
#1102 applicationWillTerminate����applicationShouldTerminate�֕ύX�ANVI_Quit()���R�[������悤�ɕύX�B
*/
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	//dock����I���Ȃǂ̏ꍇ�A���j���[quit���o�R�����ɂ����ɗ���̂ŁAquit�����s����B#953
	if( AApplication::GetApplication().NVI_IsWorking() == true )
	{
		if( AApplication::GetApplication().NVI_Quit() == false )
		{
			return NSTerminateCancel;
		}
	}
	//AApp�̉������
	delete &(AApplication::GetApplication());
	return NSTerminateNow;
}

#pragma mark ===========================

#pragma mark ---�^�C�}�[

/**
Tick�^�C�}�[�J�n
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
Tick�^�C�}�[��~
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
Tick�^�C�}�[�ĊJ
*/
- (void)restartTickTimer
{
	[self stopTickTimer];
	[self startTickTimer];
}

/**
Tick�^�C�}�[expire������
*/
- (void)doTickerTimerAction:(NSTimer*)inTimer
{
	OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(true);
	
	//AApplication::EVT_DoTickTimerAction()���s
	AApplication::GetApplication().GetImp().APICB_CocoaDoTickTimerAction();
	
	OS_CALLBACKBLOCK_END;
}

#pragma mark ===========================

#pragma mark ---�A�v���P�[�V����Activated/Deactivated

/**
�A�v���P�[�V����Activated
*/
- (void)applicationDidBecomeActive:(NSNotification *)inNotification
{
	OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(false);
	
	//AApplication::EVT_DoAppActivated()���s
	AApplication::GetApplication().GetImp().APICB_CocoaDoBecomeActive();
	
	OS_CALLBACKBLOCK_END;
}

/**
�A�v���P�[�V����Deactivated
*/
- (void)applicationDidResignActive:(NSNotification *)inNotification
{
	OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(false);
	
	//AApplication::EVT_DoAppDeactivated()���s
	AApplication::GetApplication().GetImp().APICB_CocoaDoResignActive();
	
	OS_CALLBACKBLOCK_END;
}

//#1056
/**
���̖��ݒ�h�L�������g����
*/
- (BOOL)applicationOpenUntitledFile:(NSApplication *)theApplication
{
	OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP_WITHERRRETURNVALUE(false,YES);
	
	AApplication::GetApplication().EVT_OpenUntitledFile();
	
	OS_CALLBACKBLOCK_END;
	return YES;
}

#pragma mark ===========================

#pragma mark ---���j���[�A�N�V�����R�[���o�b�N

/**
���j���[���ڑI��������
*/
- (IBAction)doMenuActions:(id)sender
{
	OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(false);
	
	//�I���������j���[���ځA���j���[�A�C���f�b�N�X���擾
	NSMenuItem*	menuItem = ACocoa::CastNSMenuItemFromId(sender);
	NSMenu*	menu = [menuItem menu];
	NSInteger	menuIndex = [menu indexOfItem:menuItem];
	AMenuRef	menuRef = static_cast<AMenuRef>(menu);
	//modifierKeys�擾
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]);
	//CAppImp�o�R�ōőO�ʂ�AWindow�A����сAAApplication�ɑ΂��AEVT_DoMenuItemSelected()�����s
	AApplication::GetApplication().GetImp().APICB_CocoaDoMenuAction([sender tag],modifier,menuRef,menuIndex);
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
	
	OS_CALLBACKBLOCK_END;
}

/**
���j���[enable/disable����
*/
- (BOOL)validateMenuItem:(NSMenuItem *)item
{
	AMenuItemID	itemID = [item tag];
	NSMenu*	menu = [item menu];
	AIndex	menuItemIndex = [menu indexOfItem:item];
	if( itemID == 0 )
	{
		//ItemID��0�Ȃ���enable
		return YES;
	}
	else
	{
		//#688
		//���I���j���[�̏ꍇ�́AGetDynamicMenuEnableMenuItem()���g�p���āA�e���j���[���ڂ�Enable/Disable���擾����
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
		//MenuManager�̃o�b�t�@�[�ɐݒ肳�ꂽ���݂�enable��Ԃ�Ԃ�
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
�L�[�����ς݃t���O�ݒ�
*/
- (void)setKeyHandled:(BOOL)inKeyHandled
{
	keyHandled = inKeyHandled;
}

//#1015
/**
�L�[�����ς݃t���O�擾
*/
- (BOOL)isKeyHandled
{
	return keyHandled;
}

#pragma mark ===========================

#pragma mark ---JavaScript
//#904

/**
JavaScript���s
*/
- (void)doJavaScript:(NSString*)inScript
{
#if BUILD_WITH_JAVASCRIPTCORE
	[mJSWrapper evaluateJavaScript:inScript];
	
	/*NSOperationQueue�e�X�g 
	JavaScriptOperation*	operation = [[JavaScriptOperation alloc] init:inScript withJSWrapper:mJSWrapper];
	[mJSOperationQueue addOperation:operation];
	*/
#endif
}

/*NSOperationQueue�e�X�g 
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

#pragma mark ---JavaScript�������f�`�F�b�N

//#1221
/**
JavaScript�������f�`�F�b�N
*/
ABool	CheckJavaScriptCancel( JSContextRef ctx, JSValueRef *exception )
{
	//��莞�Ԉȏ�o�߂��Ă�����v���O���X�E�C���h�E��\�����A���[�U�[����̃L�����Z�����󂯕t����B
	if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_JavaScript,0,true,-1,0) == false )
	{
		//�L�����Z���{�^�����N���b�N���ꂽ��A��O�𔭐�������B�i�X�N���v�g���ŏ������Ă��炤�B�ȑO�̓X���b�hkill���鏈���ɂ��Ă������A��������Ƃ��̌�JavaScriptCore�ŃN���b�V������j
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

#pragma mark ---Document�N���X

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
�e�L�X�g�̑S�̒������擾
*/
static JSValueRef js_doc_getLength( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AIndex	unicodeIndex = 0;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�e�L�X�g�擾==================
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
�i�������擾
*/
static JSValueRef js_doc_getParagraphCount( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AIndex	paraCount = 1;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�e�L�X�g�擾==================
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
�w��͈̓e�L�X�g���擾
*/
static JSValueRef js_doc_getText( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AText	text;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�e�L�X�g�擾==================
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
�I��͈͎擾
*/
static JSValueRef js_doc_getSelectionRange( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	JSObjectRef	obj = JSObjectMake(ctx,NULL,NULL);
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�I��͈͎擾==================
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
�i���͈͎擾
*/
static JSValueRef js_doc_getParagraphRange( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	JSObjectRef	obj = JSObjectMake(ctx,NULL,NULL);
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�I��͈͎擾==================
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
pos����x,y���擾
*/
static JSValueRef js_doc_getTextPointFromPos( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	JSObjectRef	obj = JSObjectMake(ctx,NULL,NULL);
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�I��͈͎擾==================
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
pos����x,y���擾
*/
static JSValueRef js_doc_getPosFromTextPoint( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AIndex	unicodeIndex = 0;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�I��͈͎擾==================
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
�I��͈͂̐��擾�i�ʏ�I�����͏��1�A��`�I���̏ꍇ�̂�2�ȏ�j
*/
static JSValueRef js_doc_getSelectionCount( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AItemCount	selectionCount = 1;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�I��͈͂̐��擾==================
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
�I��͈͐ݒ�
*/
static JSValueRef js_doc_setSelectionRange( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�I��͈͐ݒ�==================
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
�w��͈̓e�L�X�g��ݒ�i�u���j
*/
static JSValueRef js_doc_setText( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�e�L�X�g�u��==================
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
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�e�L�X�g�u��==================
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
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AText	stateText, pushedStateText;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================state�擾==================
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
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AText	modeName;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================���[�h���擾==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeRawName(modeName);
		}
	}
	catch(...)
	{
	}
	//�Ԃ�l�ݒ�
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
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	AText	url;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================URL�擾==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextDocumentByID(docID).SPI_GetURL(url);
		}
	}
	catch(...)
	{
	}
	//�Ԃ�l�ݒ�
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
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�`�F�b�J�[���[�j���O�S�N���A==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextDocumentByID(docID).SPI_ClearCheckerPluginWarnings();
			result = true;
		}
	}
	catch(...)
	{
	}
	//�Ԃ�l�ݒ�
	return JSValueMakeBoolean( ctx, result );
}

/**
addSyntaxCheckerWarning
*/
static JSValueRef js_doc_addSyntaxCheckerWarning( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	ABool	result = false;
	try
	{
		//==================�h�L�������g�E�E�C���h�E�I�u�W�F�N�g�擾==================
		ADocumentID	docID = kObjectID_Invalid;
		AWindowID	winID = kObjectID_Invalid;
		GetDocumentIDAndWindowIDFromJSObject(thisObject,docID,winID);
		
		//==================�`�F�b�J�[���[�j���O�ǉ�==================
		if( docID != kObjectID_Invalid && winID != kObjectID_Invalid )
		{
			if ( ( argumentCount >= 4 ) && 
				 JSValueIsNumber( ctx, arguments[0] ) &&
				 JSValueIsNumber( ctx, arguments[1] ) &&
				 JSValueIsString( ctx, arguments[2] ) &&
				 JSValueIsString( ctx, arguments[3] ) )
			{
				//�i���ԍ��擾
				AIndex	paraIndex = JSValueToNumber(ctx,arguments[0],exception);
				//��擾
				AIndex	colIndex = JSValueToNumber(ctx,arguments[1],exception);
				//���[�j���O�^�C�g���擾
				NSString*	str1 = [NSString stringWithJSValue:arguments[2] fromContext:ctx];
				AText	title;
				ACocoa::SetTextFromNSString(str1,title);
				//���[�j���O�ڍ׎擾
				NSString*	str2 = [NSString stringWithJSValue:arguments[3] fromContext:ctx];
				AText	detail;
				ACocoa::SetTextFromNSString(str2,detail);
				//���[�j���O�^�C�v�擾
				AIndex	typeIndex = kIndex_Invalid;
				if ( ( argumentCount >= 5 ) && 
					 JSValueIsNumber( ctx, arguments[4] ) )
				{
					typeIndex = JSValueToNumber(ctx,arguments[4],exception);
				}
				//���[�j���O��TextView�ɕ\�����邩�ǂ������擾
				ABool	displayInTextView = false;
				if ( ( argumentCount >= 6 ) && 
					 JSValueIsBoolean( ctx, arguments[5] ) )
				{
					displayInTextView = JSValueToBoolean(ctx,arguments[5]);
				}
				//���[�j���O�ǉ�
				GetApp().SPI_GetTextDocumentByID(docID).
						SPI_AddCheckerPluginWarning(paraIndex,colIndex,title,detail,typeIndex,displayInTextView);
				result = true;
			}
		}
	}
	catch(...)
	{
	}
	//�Ԃ�l�ݒ�
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

#pragma mark ---Application�N���X

/**
���݂̃h�L�������g���擾
*/
static JSValueRef js_app_getCurrentDocument( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
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
�V�K�h�L�������g�𐶐�
*/
static JSValueRef js_app_createDocument( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
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
�h�L�������gUniqID����h�L�������g�I�u�W�F�N�g�擾
*/
static JSValueRef js_app_getDocumentByID( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
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
			//�h�L�������gUniqID�擾
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	docUniqID;
			ACocoa::SetTextFromNSString(str1,docUniqID);
			//UniqID����h�L�������g��ObjectID���擾
			AUniqID	uniqID = {docUniqID.GetNumber()};
			docID = GetApp().NVI_GetDocumentIDByUniqID(uniqID);
		}
		//�h�L�������g�I�u�W�F�N�g�擾
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
JS���W���[�������[�h
*/
static JSValueRef js_app_loadModule( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
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
�v���O�C���o�^
*/
static JSValueRef js_registerPlugin( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
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
			//�Ώۃ��[�h��
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	modeName;
			ACocoa::SetTextFromNSString(str1,modeName);
			//�v���O�C��ID
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	pluginID;
			ACocoa::SetTextFromNSString(str2,pluginID);
			//�v���O�C����
			NSString*	str3 = [NSString stringWithJSValue:arguments[2] fromContext:ctx];
			AText	pluginName;
			ACocoa::SetTextFromNSString(str3,pluginName);
			//�f�t�H���g��Enable���ǂ���
			ABool	defaultEnabled = JSValueToBoolean(ctx,arguments[3]);
			//�f�t�H���g�I�v�V����
			NSString*	str4 = [NSString stringWithJSValue:arguments[4] fromContext:ctx];
			AText	options;
			ACocoa::SetTextFromNSString(str4,options);
			//���[�h�����烂�[�h���擾
			AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modeName);
			if( modeIndex != kIndex_Invalid )
			{
				//�v���O�C���o�^
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
�v���O�C���\�����ݒ�
*/
static JSValueRef js_setPluginDisplayName( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
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
			//�Ώۃ��[�h��
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	modeName;
			ACocoa::SetTextFromNSString(str1,modeName);
			//�v���O�C��ID
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	pluginID;
			ACocoa::SetTextFromNSString(str2,pluginID);
			//���ꖼ
			NSString*	str3 = [NSString stringWithJSValue:arguments[2] fromContext:ctx];
			AText	langName;
			ACocoa::SetTextFromNSString(str3,langName);
			//�v���O�C���\����
			NSString*	str4 = [NSString stringWithJSValue:arguments[3] fromContext:ctx];
			AText	displayName;
			ACocoa::SetTextFromNSString(str4,displayName);
			//���[�h�����烂�[�h���擾
			AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modeName);
			if( modeIndex != kIndex_Invalid )
			{
				//�v���O�C���o�^
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
�v���O�C��Enabled�擾
*/
static JSValueRef js_getPluginEnabled( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
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
			//�Ώۃ��[�h��
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	modeName;
			ACocoa::SetTextFromNSString(str1,modeName);
			//�v���O�C��ID
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	pluginID;
			ACocoa::SetTextFromNSString(str2,pluginID);
			//���[�h�擾
			AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modeName);
			if( modeIndex != kIndex_Invalid )
			{
				//�v���O�C��Enabled��Ԏ擾
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
�v���O�C���I�v�V�����擾
*/
static JSValueRef js_getPluginOptions( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
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
			//�Ώۃ��[�h��
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	modeName;
			ACocoa::SetTextFromNSString(str1,modeName);
			//�v���O�C��ID
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	pluginID;
			ACocoa::SetTextFromNSString(str2,pluginID);
			//���[�h�擾
			AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modeName);
			if( modeIndex != kIndex_Invalid )
			{
				//�v���O�C���I�v�V�����擾
				 GetApp().SPI_GetModePrefDB(modeIndex).GetPluginOptions(pluginID,options);
			}
		}
	}
	catch(...)
	{
	}
	//�Ԃ�l�ݒ�
	AStCreateNSStringFromAText	str(options);
	return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
}

/**
�v���O�C�����[�h���̃��[�h���擾
*/
static JSValueRef js_getCurrentModeNameForPluginLoad( JSContextRef ctx, JSObjectRef function,
			JSObjectRef thisObject, size_t argumentCount,
			const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//���[�h���擾
	AText	modeName;
	GetApp().SPI_GetCurrentModeNameForPluginLoad(modeName);
	//�Ԃ�l�ݒ�
	AStCreateNSStringFromAText	str(modeName);
	return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
}

//#1217
/**
�_�C�A���O�\��
*/
static JSValueRef js_showDialog( JSContextRef ctx, JSObjectRef function,
										 JSObjectRef thisObject, size_t argumentCount,
										 const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
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
			//�^�C�g���擾
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	title;
			ACocoa::SetTextFromNSString(str1,title);
			//���b�Z�[�W�擾
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	message;
			ACocoa::SetTextFromNSString(str2,message);
			//OK�{�^��
			NSString*	str3 = [NSString stringWithJSValue:arguments[2] fromContext:ctx];
			AText	okButton;
			ACocoa::SetTextFromNSString(str3,okButton);
			//�L�����Z���{�^��
			NSString*	str4 = [NSString stringWithJSValue:arguments[3] fromContext:ctx];
			AText	cancelButton;
			ACocoa::SetTextFromNSString(str4,cancelButton);
			//�_�C�A���O�\��
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
				//OK��
				AStCreateNSStringFromAText	str(okButton);
				return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
			}
			else
			{
				//Cancel��
				AStCreateNSStringFromAText	str(cancelButton);
				return JSValueMakeString( ctx, JSStringCreateWithCFString((CFStringRef)str.GetNSString()) );
			}
		}
		else if ( ( argumentCount >= 3 ) && 
				  JSValueIsString( ctx, arguments[0] ) &&
				  JSValueIsString( ctx, arguments[1] ) &&
				  JSValueIsString( ctx, arguments[2] )  )
		{
			//�^�C�g���擾
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	title;
			ACocoa::SetTextFromNSString(str1,title);
			//���b�Z�[�W�擾
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	message;
			ACocoa::SetTextFromNSString(str2,message);
			//OK�{�^��
			NSString*	str3 = [NSString stringWithJSValue:arguments[2] fromContext:ctx];
			AText	okButton;
			ACocoa::SetTextFromNSString(str3,okButton);
			//�_�C�A���O�\��
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
			//�^�C�g���擾
			NSString*	str1 = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	title;
			ACocoa::SetTextFromNSString(str1,title);
			//���b�Z�[�W�擾
			NSString*	str2 = [NSString stringWithJSValue:arguments[1] fromContext:ctx];
			AText	message;
			ACocoa::SetTextFromNSString(str2,message);
			//�_�C�A���O�\��
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
�W���G���[�o��
*/
static JSValueRef js_app_outputToStderr( JSContextRef ctx, JSObjectRef function,
										JSObjectRef thisObject, size_t argumentCount,
										const JSValueRef arguments[], JSValueRef *exception )
{
	//�L�����Z���|�C���g
	//#1221 pthread_testcancel();
	//���[�U�[�L�����Z���`�F�b�N�i���[�U�[���L�����Z���������O����������j#1221
	if( CheckJavaScriptCancel(ctx,exception) == true )
	{
		return JSValueMakeNull(ctx);
	}
	//
	try
	{
		if ( ( argumentCount >= 1 ) && JSValueIsString( ctx, arguments[0] ) )
		{
			//�e�L�X�g�擾
			NSString*	str = [NSString stringWithJSValue:arguments[0] fromContext:ctx];
			AText	text;
			ACocoa::SetTextFromNSString(str,text);
			//�W���G���[�ɏo��
			//#1215 NSLog()�֕ύX
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
		 //�v���O�C��
		 { "registerPlugin", js_registerPlugin, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 { "setPluginDisplayName", js_setPluginDisplayName, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 { "getPluginEnabled", js_getPluginEnabled, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 { "getPluginOptions", js_getPluginOptions, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 { "getCurrentModeNameForPluginLoad", js_getCurrentModeNameForPluginLoad, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 //�_�C�A���O�\��
		 { "showDialog", js_showDialog, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
		 //�f�o�b�O
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

//NSOperation�e�X�g
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

