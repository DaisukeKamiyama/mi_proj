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
#pragma mark [�N���X]CocoaApp
#pragma mark ===========================

@implementation CocoaApp

#pragma mark ===========================

#pragma mark ---�N��������

//#1034
/**
�N��������
*/
- (void)finishLaunching
{
	[super finishLaunching];
	openCustomView = nil;
	//Sparkle�I�u�W�F�N�g���� #1102
#ifndef Target_Mac_OS_X_10_6
#ifdef __LP64__
	//10.7�ȏ�A64bit�̏ꍇ�̂�Sparkle�I�u�W�F�N�g����
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
	{
		[[SUUpdater alloc] init];
	}
#endif
#endif
}

#pragma mark ===========================

#pragma mark ---�C�x���g�����I�[�o�[���C�h

/**
�C�x���g�����̑匳�̃I�[�o�[���C�h
*/
- (void)sendEvent:(NSEvent *)inEvent
{
	//ApplicationDefined�C�x���g�̏���
	if( [inEvent type] == NSApplicationDefined )
	{
		switch([inEvent subtype])
		{
			//InternalEvent
		  case kNSEvent_InternalEvent:
			{
				OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(false);
				
				//AApplication::EVT_DoInternalEvent()���s
				AApplication::GetApplication().EVT_DoInternalEvent();
				
				OS_CALLBACKBLOCK_END;
				return;
				break;
			}
		}
	}
	
	//#1015
	//�L�[���������t���O���N���A����
	[((AppDelegate*)[NSApp delegate]) setKeyHandled:NO];
	
	//�p���������s
	[super sendEvent:inEvent];
}

//#1034
/**
open custom view�擾
*/
- (NSView*)getOpenCustomView
{
	return openCustomView;
}

#pragma mark ===========================

#pragma mark --- AppleScript (application�I�u�W�F�N�g)

#pragma mark ===========================

#pragma mark --- element

/**
specifier�Ŏw�肳�ꂽ�qelement��index�i�qelement array���ł�index�j���擾����
��CASApp::GetSubModelByPosition()�ɑΉ�
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifier�Ŏw��̏ꍇ ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//NSIndexSpecifier�Ŏw�肳��Ă���index���擾
		AIndex	index = [indexSpec index];
		// ---------- document ----------
		if( [[indexSpec key] isEqual:@"asDocumentElement"] == YES )
		{
			try
			{
				//ADocument���X�g�i�E�C���h�Ez-order�����X�g�j�擾
				AArray<ADocumentID>	docIDArray;
				GetApp().NVI_GetOrderedDocumentIDArray(docIDArray,kDocumentType_Text,true);
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += docIDArray.GetItemCount();
				}
				//index���͈͊O�̂Ƃ��̓G���[
				if( index < 0 || index >= docIDArray.GetItemCount() )   throw 0;
				//index�����̂܂܌��ʂ֊i�[
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				//�G���[��������no such error��ݒ肵�āA���array��Ԃ�
				[CocoaApp appleScriptNoSuchObjectError:@"The document not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- window ----------
		else if( [[indexSpec key] isEqual:@"asWindowElement"] == YES )
		{
			try
			{
				//AWindow���X�g�i�E�C���h�Ez-order�����X�g�j�擾
				AArray<AWindowID>	winIDArray;
				GetApp().NVI_GetOrderedWindowIDArray(winIDArray,kWindowType_Invalid);
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += winIDArray.GetItemCount();
				}
				//index���͈͊O�̂Ƃ��̓G���[
				if( index < 0 || index >= winIDArray.GetItemCount() )   throw 0;
				//index�����̂܂܌��ʂ֊i�[
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				//�G���[��������no such error��ݒ肵�āA���array��Ԃ�
				[CocoaApp appleScriptNoSuchObjectError:@"The window not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- indexwindow ----------
		else if( [[indexSpec key] isEqual:@"asIndexwindowElement"] == YES )
		{
			try
			{
				//index���͈͊O�̂Ƃ��̓G���[
				if( index < 0 || index >= 1 )   throw 0;
				//index�����̂܂܌��ʂ֊i�[
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:0]];
				return result;
			}
			catch(...)
			{
				//�G���[��������no such error��ݒ肵�āA���array��Ԃ�
				[CocoaApp appleScriptNoSuchObjectError:@"The document not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- ��L�ȊO�I�u�W�F�N�g�̏ꍇ�̓G���[ ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else if( [specifier isKindOfClass:[NSUniqueIDSpecifier class]] == YES )
	{
		// ==================== NSUniqueIDSpecifier�Ŏw��̏ꍇ ====================
		
		//NSUniqueIDSpecifier�Ŏw�肳��Ă���id���擾
		NSUniqueIDSpecifier*	idSpec = (NSUniqueIDSpecifier*)specifier;
		AUniqID	uniqID;
		uniqID.val = [[idSpec uniqueID] intValue];
		
		// ---------- document ----------
		if( [[idSpec key] isEqual:@"asDocumentElement"] == YES )
		{
			try
			{
				//�w�肳�ꂽid�ɑΉ�����h�L�������g��ADocumentID�擾
				ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(uniqID);
				if( docID == kObjectID_Invalid )   throw 0;
				//ADocument���X�g�i�E�C���h�Ez-order�����X�g�j���ł�index���擾
				AIndex	orderIndex = GetApp().NVI_GetOrderIndexOfDocument(docID,kDocumentType_Text,true);
				//�擾����index�����ʂ֊i�[
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:orderIndex]];
				return result;
			}
			catch(...)
			{
				//�G���[��������no such error��ݒ肵�āA���array��Ԃ�
				[CocoaApp appleScriptNoSuchObjectError:@"The document not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- indexwindow ----------
		if( [[idSpec key] isEqual:@"asIndexwindowElement"] == YES )
		{
			try
			{
				//�w�肳�ꂽid�ɑΉ�����h�L�������g��ADocumentID�擾
				ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(uniqID);
				if( docID == kObjectID_Invalid )   throw 0;
				//ADocument���X�g�i�E�C���h�Ez-order�����X�g�j���ł�index���擾
				AIndex	orderIndex = GetApp().NVI_GetOrderIndexOfDocument(docID,kDocumentType_IndexWindow,true);
				//�擾����index�����ʂ֊i�[
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
		// ---------- ��L�ȊO�I�u�W�F�N�g�̏ꍇ�̓G���[ ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else if( [specifier isKindOfClass:[NSNameSpecifier class]] == YES )
	{
		// ==================== NSNameSpecifier�Ŏw��̏ꍇ ====================
		
		NSNameSpecifier*	nameSpec = (NSNameSpecifier*)specifier;
		//NSNameSpecifier�Ŏw�肳��Ă���name�擾
		AText	text;
		text.SetFromNSString([nameSpec name]);
		
		// ---------- document ----------
		if( [[nameSpec key] isEqual:@"asDocumentElement"] == YES )
		{
			try
			{
				//�w�肳�ꂽname�ɑΉ�����h�L�������g��ADocumentID�擾
				AObjectID	docID = GetApp().NVI_GetDocumentIDByTitle(kDocumentType_Text,text);
				if( docID == kObjectID_Invalid )   throw 0;
				//ADocument���X�g�i�E�C���h�Ez-order�����X�g�j���ł�index���擾
				AIndex	orderIndex = GetApp().NVI_GetOrderIndexOfDocument(docID,kDocumentType_Text,true);
				//�擾����index�����ʂ֊i�[
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:orderIndex]];
				return result;
			}
			catch(...)
			{
				//�G���[��������no such error��ݒ肵�āA���array��Ԃ�
				[CocoaApp appleScriptNoSuchObjectError:@"The document not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- indexwindow ----------
		if( [[nameSpec key] isEqual:@"asIndexwindowElement"] == YES )
		{
			try
			{
				//�w�肳�ꂽname�ɑΉ�����h�L�������g��ADocumentID�擾
				AObjectID	docID = GetApp().NVI_GetDocumentIDByTitle(kDocumentType_IndexWindow,text);
				if( docID == kObjectID_Invalid )   throw 0;
				//ADocument���X�g�i�E�C���h�Ez-order�����X�g�j���ł�index���擾
				AIndex	orderIndex = GetApp().NVI_GetOrderIndexOfDocument(docID,kDocumentType_IndexWindow,true);
				//�擾����index�����ʂ֊i�[
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:orderIndex]];
				return result;
			}
			catch(...)
			{
				//�G���[��������no such error��ݒ肵�āA���array��Ԃ�
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
				//�w�肳�ꂽname�ɑΉ�����h�L�������g��ADocumentID�擾
				AObjectID	docID = GetApp().NVI_GetDocumentIDByTitle(kDocumentType_Text,text);
				if( docID == kObjectID_Invalid )   throw 0;
				//�h�L�������g�ɑΉ�����E�C���h�E���擾
				AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
				//�E�C���h�E��orderIndex���擾
				AArray<AWindowID>	winIDArray;
				GetApp().NVI_GetOrderedWindowIDArray(winIDArray,kWindowType_Invalid);
				AIndex	orderIndex = winIDArray.Find(winID);
				//�擾����index�����ʂ֊i�[
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:orderIndex]];
				return result;
			}
			catch(...)
			{
				//�G���[��������no such error��ݒ肵�āA���array��Ԃ�
				[CocoaApp appleScriptNoSuchObjectError:@"The window not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- ��L�ȊO�I�u�W�F�N�g�̏ꍇ�̓G���[ ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else
	{
		// ==================== ��L�ȊO�Ŏw��̏ꍇ�̓G���[ ====================
		// �v���p�e�B�擾�̏ꍇ�������ɗ���̂ŁA�G���[�ɂ����Anil��Ԃ�
		return nil;
	}
}

/**
�qelement�udocument�v��array�i�E�C���h�Ez-order�����X�g�j���擾
*/
- (NSArray *)asDocumentElement
{
	//ADocument���X�g�擾
	AArray<ADocumentID>	docIDArray;
	GetApp().NVI_GetOrderedDocumentIDArray(docIDArray,kDocumentType_Text,true);
	//TextDocument�I�u�W�F�N�g�̃|�C���^��array�𐶐�
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
�qelement�uwindow�v��array�i�E�C���h�Ez-order�����X�g�j���擾
*/
- (NSArray *)asWindowElement
{
	//AWindow���X�g�擾
	AArray<AWindowID>	winIDArray;
	GetApp().NVI_GetOrderedWindowIDArray(winIDArray,kWindowType_Invalid);
	//NSArray��CASWindow�I�u�W�F�N�g�̃|�C���^���i�[
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
�qelement�uindexwindow�v��array�i�E�C���h�Ez-order�����X�g�j���擾
*/
- (NSArray *)asIndexwindowElement
{
	//NSArray��IndexWindowDocument�I�u�W�F�N�g�̃|�C���^���i�[
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
open�R�}���h�n���h��
*/
- (id)handleOpenScriptCommand:(MIOpenCommand*)command
{
	//direct parameter�擾
	NSURL*	directParameter = [command directParameter];
	//path�擾
	AText	path;
	path.SetCstring([[directParameter path] UTF8String]);
	AFileAcc	file;
	file.Specify(path);
	//�h�L�������g����
	ADocumentID docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file);
    return nil;
}

/**
exists�R�}���h�n���h��
*/
- (id)handleExistsScriptCommand:(MIExistsCommand*)command
{
	return @(YES);
}

//#1143
/**
count�R�}���h�n���h��
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
AppleScript not handled�G���[��ݒ�
*/
+ (void)appleScriptNotHandledError
{
	NSScriptCommand *currentScriptCommand = [NSScriptCommand currentCommand];
	[currentScriptCommand setScriptErrorNumber:-1708];
	[currentScriptCommand setScriptErrorString:@"Command not handled."];
}

/**
AppleScript not handled�G���[��ݒ�i�h�L�������g��read only�j
*/
+ (void)appleScriptNotHandledError_DocumentReadOnly
{
	NSScriptCommand *currentScriptCommand = [NSScriptCommand currentCommand];
	[currentScriptCommand setScriptErrorNumber:-1708];
	[currentScriptCommand setScriptErrorString:@"Command not handled. The document is read only."];
}

/**
AppleScript no such object�G���[��ݒ�
*/
+ (void)appleScriptNoSuchObjectError:(NSString*)errorString
{
	NSScriptCommand *currentScriptCommand = [NSScriptCommand currentCommand];
	if( [currentScriptCommand isKindOfClass:[MIExistsCommand class]] == YES )
	{
		//exist�R�}���h���s���̓G���[�ɂ��Ȃ�
		return;
	}
	[currentScriptCommand setScriptErrorNumber:-1728];
	[currentScriptCommand setScriptErrorString:errorString];
}


@end

