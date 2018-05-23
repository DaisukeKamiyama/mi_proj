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

CocoaApp

*/

#import "AppDelegate.h"
#include "../../AbsFramework/Frame.h"
#include "../../App/Source/AApp.h"
#include "../AbsBase/Cocoa.h"
#import "CocoaWindow.h"
#include "CocoaMacro.h"
#include "AApp.h"
//#1097 #import <HockeySDK/HockeySDK.h>
#include "CWindowImpCocoaObjects.h"

#pragma mark ===========================
#pragma mark [クラス]CocoaApp
#pragma mark ===========================

@implementation CocoaApp

#pragma mark ===========================

#pragma mark ---起動時処理

//#1034
/**
起動時処理
*/
- (void)finishLaunching
{
	[super finishLaunching];
	openCustomView = nil;
	//Sparkleオブジェクト生成 #1102
#ifndef Target_Mac_OS_X_10_6
#ifdef __LP64__
	//10.7以上、64bitの場合のみSparkleオブジェクト生成
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
	{
		[[SUUpdater alloc] init];
	}
#endif
#endif
}

#pragma mark ===========================

#pragma mark ---イベント処理オーバーライド

/**
イベント処理の大元のオーバーライド
*/
- (void)sendEvent:(NSEvent *)inEvent
{
	//ApplicationDefinedイベントの処理
	if( [inEvent type] == NSApplicationDefined )
	{
		switch([inEvent subtype])
		{
			//InternalEvent
		  case kNSEvent_InternalEvent:
			{
				OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(false);
				
				//AApplication::EVT_DoInternalEvent()実行
				AApplication::GetApplication().EVT_DoInternalEvent();
				
				OS_CALLBACKBLOCK_END;
				return;
				break;
			}
		}
	}
	
	//#1015
	//キー処理完了フラグをクリアする
	[((AppDelegate*)[NSApp delegate]) setKeyHandled:NO];
	
	//継承処理実行
	[super sendEvent:inEvent];
}

//#1034
/**
open custom view取得
*/
- (NSView*)getOpenCustomView
{
	return openCustomView;
}

#pragma mark ===========================

#pragma mark --- AppleScript (applicationオブジェクト)

#pragma mark ===========================

#pragma mark --- element

/**
specifierで指定された子elementのindex（子element array内でのindex）を取得する
※CASApp::GetSubModelByPosition()に対応
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifierで指定の場合 ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//NSIndexSpecifierで指定されているindexを取得
		AIndex	index = [indexSpec index];
		// ---------- document ----------
		if( [[indexSpec key] isEqual:@"asDocumentElement"] == YES )
		{
			try
			{
				//ADocumentリスト（ウインドウz-order順リスト）取得
				AArray<ADocumentID>	docIDArray;
				GetApp().NVI_GetOrderedDocumentIDArray(docIDArray,kDocumentType_Text,true);
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += docIDArray.GetItemCount();
				}
				//indexが範囲外のときはエラー
				if( index < 0 || index >= docIDArray.GetItemCount() )   throw 0;
				//indexをそのまま結果へ格納
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				//エラー発生時はno such errorを設定して、空のarrayを返す
				[CocoaApp appleScriptNoSuchObjectError:@"The document not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- window ----------
		else if( [[indexSpec key] isEqual:@"asWindowElement"] == YES )
		{
			try
			{
				//AWindowリスト（ウインドウz-order順リスト）取得
				AArray<AWindowID>	winIDArray;
				GetApp().NVI_GetOrderedWindowIDArray(winIDArray,kWindowType_Invalid);
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += winIDArray.GetItemCount();
				}
				//indexが範囲外のときはエラー
				if( index < 0 || index >= winIDArray.GetItemCount() )   throw 0;
				//indexをそのまま結果へ格納
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				//エラー発生時はno such errorを設定して、空のarrayを返す
				[CocoaApp appleScriptNoSuchObjectError:@"The window not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- indexwindow ----------
		else if( [[indexSpec key] isEqual:@"asIndexwindowElement"] == YES )
		{
			try
			{
				//indexが範囲外のときはエラー
				if( index < 0 || index >= 1 )   throw 0;
				//indexをそのまま結果へ格納
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:0]];
				return result;
			}
			catch(...)
			{
				//エラー発生時はno such errorを設定して、空のarrayを返す
				[CocoaApp appleScriptNoSuchObjectError:@"The document not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- 上記以外オブジェクトの場合はエラー ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else if( [specifier isKindOfClass:[NSUniqueIDSpecifier class]] == YES )
	{
		// ==================== NSUniqueIDSpecifierで指定の場合 ====================
		
		//NSUniqueIDSpecifierで指定されているidを取得
		NSUniqueIDSpecifier*	idSpec = (NSUniqueIDSpecifier*)specifier;
		AUniqID	uniqID;
		uniqID.val = [[idSpec uniqueID] intValue];
		
		// ---------- document ----------
		if( [[idSpec key] isEqual:@"asDocumentElement"] == YES )
		{
			try
			{
				//指定されたidに対応するドキュメントのADocumentID取得
				ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(uniqID);
				if( docID == kObjectID_Invalid )   throw 0;
				//ADocumentリスト（ウインドウz-order順リスト）内でのindexを取得
				AIndex	orderIndex = GetApp().NVI_GetOrderIndexOfDocument(docID,kDocumentType_Text,true);
				//取得したindexを結果へ格納
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:orderIndex]];
				return result;
			}
			catch(...)
			{
				//エラー発生時はno such errorを設定して、空のarrayを返す
				[CocoaApp appleScriptNoSuchObjectError:@"The document not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- indexwindow ----------
		if( [[idSpec key] isEqual:@"asIndexwindowElement"] == YES )
		{
			try
			{
				//指定されたidに対応するドキュメントのADocumentID取得
				ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(uniqID);
				if( docID == kObjectID_Invalid )   throw 0;
				//ADocumentリスト（ウインドウz-order順リスト）内でのindexを取得
				AIndex	orderIndex = GetApp().NVI_GetOrderIndexOfDocument(docID,kDocumentType_IndexWindow,true);
				//取得したindexを結果へ格納
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:orderIndex]];
				return result;
			}
			catch(...)
			{
				[CocoaApp appleScriptNoSuchObjectError:@"The document not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- 上記以外オブジェクトの場合はエラー ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else if( [specifier isKindOfClass:[NSNameSpecifier class]] == YES )
	{
		// ==================== NSNameSpecifierで指定の場合 ====================
		
		NSNameSpecifier*	nameSpec = (NSNameSpecifier*)specifier;
		//NSNameSpecifierで指定されているname取得
		AText	text;
		text.SetFromNSString([nameSpec name]);
		
		// ---------- document ----------
		if( [[nameSpec key] isEqual:@"asDocumentElement"] == YES )
		{
			try
			{
				//指定されたnameに対応するドキュメントのADocumentID取得
				AObjectID	docID = GetApp().NVI_GetDocumentIDByTitle(kDocumentType_Text,text);
				if( docID == kObjectID_Invalid )   throw 0;
				//ADocumentリスト（ウインドウz-order順リスト）内でのindexを取得
				AIndex	orderIndex = GetApp().NVI_GetOrderIndexOfDocument(docID,kDocumentType_Text,true);
				//取得したindexを結果へ格納
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:orderIndex]];
				return result;
			}
			catch(...)
			{
				//エラー発生時はno such errorを設定して、空のarrayを返す
				[CocoaApp appleScriptNoSuchObjectError:@"The document not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- indexwindow ----------
		if( [[nameSpec key] isEqual:@"asIndexwindowElement"] == YES )
		{
			try
			{
				//指定されたnameに対応するドキュメントのADocumentID取得
				AObjectID	docID = GetApp().NVI_GetDocumentIDByTitle(kDocumentType_IndexWindow,text);
				if( docID == kObjectID_Invalid )   throw 0;
				//ADocumentリスト（ウインドウz-order順リスト）内でのindexを取得
				AIndex	orderIndex = GetApp().NVI_GetOrderIndexOfDocument(docID,kDocumentType_IndexWindow,true);
				//取得したindexを結果へ格納
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:orderIndex]];
				return result;
			}
			catch(...)
			{
				//エラー発生時はno such errorを設定して、空のarrayを返す
				[CocoaApp appleScriptNoSuchObjectError:@"The document not found."];
				return [NSMutableArray array];
			}
		}
		//#1141
		// ---------- window ----------
		if( [[nameSpec key] isEqual:@"asWindowElement"] == YES )
		{
			try
			{
				//指定されたnameに対応するドキュメントのADocumentID取得
				AObjectID	docID = GetApp().NVI_GetDocumentIDByTitle(kDocumentType_Text,text);
				if( docID == kObjectID_Invalid )   throw 0;
				//ドキュメントに対応するウインドウを取得
				AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
				//ウインドウのorderIndexを取得
				AArray<AWindowID>	winIDArray;
				GetApp().NVI_GetOrderedWindowIDArray(winIDArray,kWindowType_Invalid);
				AIndex	orderIndex = winIDArray.Find(winID);
				//取得したindexを結果へ格納
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:orderIndex]];
				return result;
			}
			catch(...)
			{
				//エラー発生時はno such errorを設定して、空のarrayを返す
				[CocoaApp appleScriptNoSuchObjectError:@"The window not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- 上記以外オブジェクトの場合はエラー ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else
	{
		// ==================== 上記以外で指定の場合はエラー ====================
		// プロパティ取得の場合もここに来るので、エラーにせず、nilを返す
		return nil;
	}
}

/**
子element「document」のarray（ウインドウz-order順リスト）を取得
*/
- (NSArray *)asDocumentElement
{
	//ADocumentリスト取得
	AArray<ADocumentID>	docIDArray;
	GetApp().NVI_GetOrderedDocumentIDArray(docIDArray,kDocumentType_Text,true);
	//TextDocumentオブジェクトのポインタのarrayを生成
	NSMutableArray*	result = [NSMutableArray array];
	for( AIndex i = 0; i < docIDArray.GetItemCount(); i++ )
	{
		ADocumentID	docID = docIDArray.Get(i);
		AObjectID	docImpID = GetApp().SPI_GetTextDocumentByID(docID).NVI_GetDocImpID();
		if( docImpID != kObjectID_Invalid )
		{
			TextDocument*	doc = (TextDocument*)(GetApp().GetImp().GetDocImpByID(docImpID).GetNSDocument());
			[result addObject:doc];
		}
	}
	return result;
}

/**
子element「window」のarray（ウインドウz-order順リスト）を取得
*/
- (NSArray *)asWindowElement
{
	//AWindowリスト取得
	AArray<AWindowID>	winIDArray;
	GetApp().NVI_GetOrderedWindowIDArray(winIDArray,kWindowType_Invalid);
	//NSArrayへCASWindowオブジェクトのポインタを格納
	NSMutableArray*	result = [NSMutableArray array];
	for( AIndex i = 0; i < winIDArray.GetItemCount(); i++ )
	{
		AWindowID	winID = winIDArray.Get(i);
		CASWindow*	win = AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_GetWindowImp().GetCocoaObjects().GetCASWindow();
        [result addObject:win];
		
	}
	return result;
}

/**
子element「indexwindow」のarray（ウインドウz-order順リスト）を取得
*/
- (NSArray *)asIndexwindowElement
{
	//NSArrayへIndexWindowDocumentオブジェクトのポインタを格納
	NSMutableArray*	result = [NSMutableArray array];
	//
	ADocumentID	docID = GetApp().SPI_GetOrCreateFindResultWindowDocument();
	AObjectID	docImpID = GetApp().SPI_GetIndexWindowDocumentByID(docID).NVI_GetDocImpID();
	if( docImpID != kObjectID_Invalid )
	{
		IndexWindowDocument*	doc = (IndexWindowDocument*)(GetApp().GetImp().GetDocImpByID(docImpID).GetNSDocument());
		[result addObject:doc];
	}
	return result;
}

#pragma mark ===========================

#pragma mark --- command handler

/**
openコマンドハンドラ
*/
- (id)handleOpenScriptCommand:(MIOpenCommand*)command
{
	//direct parameter取得
	NSURL*	directParameter = [command directParameter];
	//path取得
	AText	path;
	path.SetCstring([[directParameter path] UTF8String]);
	AFileAcc	file;
	file.Specify(path);
	//ドキュメント生成
	ADocumentID docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file);
    return nil;
}

/**
existsコマンドハンドラ
*/
- (id)handleExistsScriptCommand:(MIExistsCommand*)command
{
	return @(YES);
}

//#1143
/**
countコマンドハンドラ
*/
- (id)handleCountScriptCommand:(MICountCommand*)command
{
	//
	NSDictionary *args = [command evaluatedArguments];
	NSUInteger	objectType = [((NSNumber*)[args valueForKey:@"ObjectClass"]) intValue];
	AItemCount	count = 0;
	switch(objectType)
	{
	  case kASObjectCode_Document:
		{
			AArray<ADocumentID>	docIDArray;
			GetApp().NVI_GetOrderedDocumentIDArray(docIDArray,kDocumentType_Text,true);
			count = docIDArray.GetItemCount();
			break;
		}
	  case kASObjectCode_Window:
		{
			AArray<AWindowID>	winIDArray;
			GetApp().NVI_GetOrderedWindowIDArray(winIDArray,kWindowType_Invalid);
			count = winIDArray.GetItemCount();
			break;
		}
	  case kASObjectCode_IndexWindow:
		{
			AArray<ADocumentID>	docIDArray;
			GetApp().NVI_GetOrderedDocumentIDArray(docIDArray,kDocumentType_IndexWindow,true);
			count = docIDArray.GetItemCount();
			break;
		}
	  default:
		{
			[CocoaApp appleScriptNotHandledError];
			break;
		}
	}
	return [NSNumber numberWithInteger:count];
}

#pragma mark ===========================

#pragma mark --- AppleScript errors

/**
AppleScript not handledエラーを設定
*/
+ (void)appleScriptNotHandledError
{
	NSScriptCommand *currentScriptCommand = [NSScriptCommand currentCommand];
	[currentScriptCommand setScriptErrorNumber:-1708];
	[currentScriptCommand setScriptErrorString:@"Command not handled."];
}

/**
AppleScript not handledエラーを設定（ドキュメントがread only）
*/
+ (void)appleScriptNotHandledError_DocumentReadOnly
{
	NSScriptCommand *currentScriptCommand = [NSScriptCommand currentCommand];
	[currentScriptCommand setScriptErrorNumber:-1708];
	[currentScriptCommand setScriptErrorString:@"Command not handled. The document is read only."];
}

/**
AppleScript no such objectエラーを設定
*/
+ (void)appleScriptNoSuchObjectError:(NSString*)errorString
{
	NSScriptCommand *currentScriptCommand = [NSScriptCommand currentCommand];
	if( [currentScriptCommand isKindOfClass:[MIExistsCommand class]] == YES )
	{
		//existコマンド実行中はエラーにしない
		return;
	}
	[currentScriptCommand setScriptErrorNumber:-1728];
	[currentScriptCommand setScriptErrorString:errorString];
}


@end

