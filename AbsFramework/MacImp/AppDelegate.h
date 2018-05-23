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

#pragma once

#import <Cocoa/Cocoa.h>
#import <JavaScriptCore/JavaScriptCore.h>
#import "JSWrappers.h"
#import "Functions.h"
#include "../AbsBase/ABase.h"
#include "../AbsBase/ADataBase.h"
#include "../AbsFrame/AbsFrame.h"
#import "CocoaApp.h"

/**
アプリケーションdelegate
*/
@interface AppDelegate : NSResponder<NSApplicationDelegate,NSSpeechSynthesizerDelegate>//#1203 一旦保留,NSTouchBarDelegate>
{
	NSTimer*	tickTimer;
	
	BOOL	keyHandled;//#1015
	
#if BUILD_WITH_JAVASCRIPTCORE
	//
	//NSOperationQueueテスト NSOperationQueue*	mJSOperationQueue;
	
	//JavaScript
	JSGlobalContextRef	mJSContext;
	JSWrappers*			mJSWrapper;
	
	//#851
	//Speech
	NSSpeechSynthesizer*				mSpeechSynthesizer;
	//NSArray<NSString *>*				mSpeechVoiceList;
	BOOL								mIsSpeaking;
#endif
}

- (IBAction)doMenuActions:(id)sender;
- (void)doJavaScript:(NSString*)inScript;
//- (BOOL)executingJavaScript;
//- (void)cancelExecutingJavaScript;

//#1015
- (void)setKeyHandled:(BOOL)inKeyHandled;
- (BOOL)isKeyHandled;

- (void)startTickTimer;
- (void)stopTickTimer;
- (void)restartTickTimer;

//Speech
- (void)speakText:(NSString*)text;
- (void)stopSpeaking;
- (BOOL)isSpeaking;

@end

JSClassRef DocClass( void );
JSClassRef AppClass( void );

/*NSOperationQueueテスト
@interface JavaScriptOperation : NSOperation 
{
	BOOL	executing;
	BOOL	finished;
	//
	JSWrappers*		mJSWrapper;
	NSString*		mScript;
}
- (void)completeOperation;
@end
*/
