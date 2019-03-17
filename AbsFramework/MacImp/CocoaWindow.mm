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

CocoaWindow

*/

#import "CocoaWindow.h"
#import "CocoaPanelWindow.h"
#import "CocoaTextField.h"
#import "CocoaFontField.h"
#import "CocoaTabSelectButton.h"
#import "CocoaProgressIndicator.h"
#import "CocoaCollapsableView.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"
#include "CWindowImpCocoaObjects.h"
#include "../../App/Source/ATypes.h"
#include "../../App/Source/AApp.h"
#import "AppDelegate.h"

@implementation CASWindow
#pragma mark ===========================
#pragma mark [�N���X]CASWindow
#pragma mark ===========================

#pragma mark --- property

/**
init
*/
- (id)init:(AWindowID)windowID
{
	self = [super init];
	if(self)
	{
		mWindowID = windowID;
	}
	return self;
}

// -------------------- bounds �v���p�e�B --------------------

/**
bounds�擾
*/
- (NSAppleEventDescriptor*)asBounds
{
	ARect	rect;
	AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_GetWindowBounds(rect);
	return [NSAppleEventDescriptor descriptorWithARect:rect];
}

/**
bounds�ݒ�
*/
- (void)setAsBounds:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	ARect	rect = [aeDesc arectValue];
	if( rect.right - rect.left > 0 )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_SetWindowBounds(rect);
	}
	else
	{
		//�G���[
		[CocoaApp appleScriptNotHandledError];
	}
}

// -------------------- closable �v���p�e�B --------------------

/**
closable�擾
*/
- (NSAppleEventDescriptor*)asClosable
{
	Boolean b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_HasCloseButton();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- titled �v���p�e�B --------------------

/**
titled�擾
*/
- (NSAppleEventDescriptor*)asTitled
{
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_HasTitleBar();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- index �v���p�e�B --------------------

/**
index�擾
*/
- (NSAppleEventDescriptor*)asIndex
{
	//
	AItemCount	count = GetApp().NVI_GetWindowCount(kWindowType_Invalid);
	for( ANumber num = 1; num <= count; num++ )
	{
		if( GetApp().NVI_GetNthWindowID(kWindowType_Invalid,num) == mWindowID )
		{
			return [NSAppleEventDescriptor descriptorWithInt32:num];
		}
	}
	return nil;
}

/**
index�ݒ�
*/
- (void)setAsIndex:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	[self setWindowOrder:[aeDesc int32Value]];
}

/**
�E�C���h�E���ݒ�
*/
- (void)setWindowOrder:(SInt32)orderNumber
{
	//
	if( orderNumber <= 1 )
	{
		//�őO�ʂɂ���
		AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_SelectWindow();
	}
	else
	{
		//���݂̃E�C���h�E���擾
		AItemCount	count = GetApp().NVI_GetWindowCount(kWindowType_Invalid);
		ANumber currentnum = 1;
		for( ; currentnum <= count; currentnum++ )
		{
			if( GetApp().NVI_GetNthWindowID(kWindowType_Invalid,currentnum) == mWindowID )
			{
				break;
			}
		}
		//���݂�number�Ɠ����Ȃ牽������return�A���݂�number������ɔz�u����Ȃ�w��number��+1����B
		ANumber	num = orderNumber;
		if( currentnum == num )   return;
		if( currentnum < num )
		{
			num++;
		}
		//���O�̃E�C���h�E�擾
		AWindowID winID = GetApp().NVI_GetNthWindowID(kWindowType_Invalid,num-1);
		if( winID == kObjectID_Invalid )
		{
			//�G���[
			[CocoaApp appleScriptNotHandledError];
		}
		else
		{
			//���O�̃E�C���h�E�̌��Ɉړ�����
			AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_SendBehind(winID);
		}
	}
}

// -------------------- floating �v���p�e�B --------------------

/**
floating�擾
*/
- (NSAppleEventDescriptor*)asFloating
{
	//
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_IsFloating();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- modal �v���p�e�B --------------------

/**
modal�擾
*/
- (NSAppleEventDescriptor*)asModal
{
	//
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_IsModal();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- resizable �v���p�e�B --------------------

/**
resizable�擾
*/
- (NSAppleEventDescriptor*)asResizable
{
	//
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_IsResizable();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- zoomable �v���p�e�B --------------------

/**
zoomable�擾
*/
- (NSAppleEventDescriptor*)asZoomable
{
	//
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_IsZoomable();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- zoomed �v���p�e�B --------------------

/**
zoomed�擾
*/
- (NSAppleEventDescriptor*)asZoomed
{
	//
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_IsZoomed();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- name �v���p�e�B --------------------

/**
name�擾
*/
- (NSAppleEventDescriptor*)asName
{
	//
	AText	text;
	AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_GetCurrentTitle(text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
name�ݒ�
*/
- (void)setAsName:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	//
	AText	text;
	text.SetFromNSString([aeDesc stringValue]);
	AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_SetDefaultTitle(text);
}

// -------------------- visible �v���p�e�B --------------------

/**
visible�擾
*/
- (NSAppleEventDescriptor*)asVisible
{
	//
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_IsWindowVisible();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- position �v���p�e�B --------------------

/**
position�擾
*/
- (NSAppleEventDescriptor*)asPosition
{
	APoint	pt = {0};
	AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_GetWindowPosition(pt);
	return [NSAppleEventDescriptor descriptorWithAPoint:pt];
}
@end

@implementation CocoaWindow

#pragma mark ===========================
#pragma mark [�N���X]CocoaWindow
#pragma mark ===========================

#pragma mark ===========================

#pragma mark ---�����ݒ�

/**
view�̏����ݒ�
*/
- (void)setupView: (NSView*)view
{
	//tabView�ɂ��ẮA���ݑI������Ă���tab���ȊO��view��subviews�Ɋ܂܂�Ȃ��̂ŁA
	//tabView�̊etab�ɂ��Č�������K�v������B
	if( [view isKindOfClass:[NSTabView class]] == YES )
	{
		//tab view�擾
		NSTabView*	tabview = static_cast<NSTabView*>(view);
		/*tab view��bottom tab��tabless�֎����ϊ�����e�X�g
		if( [tabview tabViewType] == NSBottomTabsBezelBorder )
		{
			[tabview setTabViewType:NSNoTabsNoBorder];
			//
			NSRect	frame = [tabview frame];
			frame.origin.x += 10;
			frame.origin.y -= 10;
			[tabview setFrame:frame];
		}
		*/
		//tab view item���Ƃ̃��[�v
		AItemCount	tabViewItemCount = [[tabview tabViewItems] count];
		for( AIndex i = 0; i < tabViewItemCount; i++ )
		{
			//tab view item����view�ɂ���setupView���s
			[self setupView:[[tabview tabViewItemAtIndex:i] view]];
		}
		//tab view���X�g�ɒǉ�
		mTabViewArray.Add(tabview);
	}
	//
	if( [view isKindOfClass:[NSControl class]] == YES )
	{
		NSControl*	control = (NSControl*)view;
		if( [control tag] > 0 && [control target] == nil )
		{
			//NSPopUpButton�̏ꍇ
			if( [control isKindOfClass:[NSPopUpButton class]] == YES )
			{
				//target/action�ݒ�
				[control setTarget:self];
				[control setAction:@selector(doPopupMenuAction:)];
			}
			//NSScroller�̏ꍇ
			else if( [control isKindOfClass:[NSScroller class]] == YES )
			{
				//target/action�ݒ�
				[control setTarget:self];
				[control setAction:@selector(doScrollbarAction:)];
			}
			//NSButton�̏ꍇ
			else if( [control isKindOfClass:[NSButton class]] == YES )
			{
				//target/action�ݒ�
				[control setTarget:self];
				[control setAction:@selector(doButtonAction:)];
			}
			//CocoaTextField�̏ꍇ
			else if( [control isKindOfClass:[CocoaTextField class]] == YES )
			{
				CocoaTextField*	cocoaTextField = reinterpret_cast<CocoaTextField*>(control);
				//�����ݒ�
				[cocoaTextField setup:mWindowImp];
				//target/action�ݒ�
				[cocoaTextField setTarget:self];
				[cocoaTextField setAction:@selector(doTextFieldAction:)];
			}
			//CocoaFontField�̏ꍇ
			else if( [control isKindOfClass:[CocoaFontField class]] == YES )
			{
				CocoaFontField*	cocoaFontField = reinterpret_cast<CocoaFontField*>(control);
				//�����ݒ�
				[cocoaFontField setup:mWindowImp];
				//target/action�ݒ�͂��Ȃ�
			}
			//NSTextField�̏ꍇ
			else if( [control isKindOfClass:[NSTextField class]] == YES )
			{
				//target/action�ݒ�
				[control setTarget:self];
				[control setAction:@selector(doTextFieldAction:)];
			}
			//NSStepper�̏ꍇ
			else if( [control isKindOfClass:[NSStepper class]] == YES )
			{
				//target/action�ݒ�
				[control setTarget:self];
				[control setAction:@selector(doStepper:)];
				//
				NSStepper*	stepper = reinterpret_cast<NSStepper*>(control);
				[stepper setMinValue:0];
				[stepper setMaxValue:100];
				[stepper setIntValue:50];
				[stepper setIncrement:1];
			}
			//��L�ȊO�̏ꍇ
			else 
			{
				//target/action�ݒ�
				[control setTarget:self];
				[control setAction:@selector(doButtonAction:)];
			}
		}
	}
	//�qview������΁A�ċA�I��setupViews���s
	if( [[view subviews] count] > 0 )
	{
		[self setupViews:view];
	}
}

/**
�eview�̏����ݒ�
*/
- (void)setupViews:(NSView*)inParentView
{
	//�eview�擾
	NSArray*	subviews = [inParentView subviews];
	NSInteger itemCount = [subviews count];
	for( NSInteger index = 0; index < itemCount; index++  )
	{
		//index�Ԗڂ�view�擾
		NSView*	view = [subviews objectAtIndex:index];
		[self setupView:view];
	}
}

/**
Toolbar�̏����ݒ�
*/
- (void)setupToolbar
{
	//toolbar�����Ȃ牽�����Ȃ�
	if( [self toolbar] == nil )   return;
	
	//�e�c�[���o�[���ږ��̃��[�v
	NSArray*	toolbarItems = [[self toolbar] items];
	NSInteger	itemCount = [toolbarItems count];
	for( NSInteger index = 0; index < itemCount; index++ )
	{
		//�c�[���o�[���ڂ�target/action�ݒ�
		NSToolbarItem*	item = [toolbarItems objectAtIndex:index];
		[item setTarget:self];
		[item setAction:@selector(doToolbarItemAction:)];
		//�c�[���o�[���ړ���view�ɂ���setup���s��
		NSView*	view = [item view];
		if( view != nil )
		{
			[self setupView:view];
		}
	}
}

/**
�����ݒ�
*/
- (void)setup:(CWindowImp*)inWindowImp
{
	//CWindowImp�ւ̃|�C���^�ݒ�
	mWindowImp = inWindowImp;
	//Overlay�E�C���h�E���C���[�����ݒ�
	mOverlayWindowLayer = kOverlayWindowLayer_None;
	//mCanBecomeMainWindow, mCanBecomeKeyWindow�̃f�t�H���g�ݒ�
	mCanBecomeMainWindow = YES;
	mCanBecomeKeyWindow = YES;
	//mIgnoresMouseEvents = NO;
	//�eview�̏����ݒ�
	[self setupViews:[self contentView]];
	//�c�[���o�[�̏����ݒ�
	[self setupToolbar];
	//mousemoved�C�x���g���󂯎��
	[self setAcceptsMouseMovedEvents:YES];
	//�N���[�Y����release�͂��Ȃ��iCocoaObjects�ŊǗ�����j
	[self setReleasedWhenClosed:NO];
	//�ǉ����悤�Ƃ��Ă���c�[���o�[���ڂւ̃����N��������
	mAddingToolbarItem = nil;
	//�c�[���o�[�^�C�v
	mToolbarType = 0;
}

/**
�c�[���o�[�^�C�v�ݒ�
*/
- (void)setToolbarType:(AIndex)inType
{
	mToolbarType = inType;
}

/**
Overlay�E�C���h�E���C���[�ݒ�
*/
- (void)setOverlayWindowLayer:(AOverlayWindowLayer)inOverlayWindowLayer
{
	mOverlayWindowLayer = inOverlayWindowLayer;
}

/**
Overlay�E�C���h�E���C���[�擾
*/
- (AOverlayWindowLayer)getOverlayWindowLayer
{
	return mOverlayWindowLayer;
}

/**
ControlID����view������
*/
- (NSView*)findViewByControlID:(AControlID)inControlID
{
	//viewWithTag���g���Č���
	NSView*	view = [[self contentView] viewWithTag:inControlID];
	if( view == nil )
	{
		//segmented control����tag��viewWithTag�Ō�������Ȃ��̂ŁA���ꂼ���NSSegmentedControl�ɂ��Č���
		//�������Asegment�S�̂�view��Ԃ��̂ŁAsegment���ƂɃA�N�Z�X����K�v������ꍇ�́A�ʓrmSegmentedControlArray_Segment���g���ăA�N�Z�X����K�v������
		AIndex	segmentedControlIndex = mSegmentedControlArray_ControlID.Find(inControlID);
		if( segmentedControlIndex != kIndex_Invalid )
		{
			return mSegmentedControlArray_SegmentedControl.Get(segmentedControlIndex);
		}
		
		//NSTabView���́A�\������Ă��Ȃ����ڂ́AviewWithTag�Ō�������Ȃ��̂ŁA���ꂼ���NSTabView�ɂ��Č���
		//mTabViewArray��setupView���Őݒ肳���B�E�C���h�E���̑S�Ă�NSTabView�������Ă���B
		AItemCount	itemCount = mTabViewArray.GetItemCount();
		for( AIndex j = 0; j < itemCount; j++ )
		{
			//NSTabView���̊eNSTabViewItem���̃��[�v
			AItemCount	tabViewItemCount = [[mTabViewArray.Get(j) tabViewItems] count];
			for( AIndex i = 0; i < tabViewItemCount; i++ )
			{
				//NSTabViewItem��view�ɂ���viewWithTag���g���Č���
				view = [[[mTabViewArray.Get(j) tabViewItemAtIndex:i] view] viewWithTag:inControlID];
				if( view != nil )
				{
					return view;
				}
			}
		}
	}
	return view;
}

//#1034
/**
WindowID���擾
*/
- (AWindowID)windowID
{
	return mWindowImp->GetAWin().GetObjectID();
}

#pragma mark ===========================

#pragma mark ---�e��ݒ�^�擾

/**
IB�Őݒ肳�ꂽProgressIndicator�擾
*/
- (NSProgressIndicator*)getProgressIndicator
{
	return progressIndicator;
}

/**
tab view�擾
*/
- (NSTabView*)getTabView
{
	return tabView;
}

/**
mCanBecomeMainWindow�ݒ�
*/
- (void)setCanBecomeMainWindow:(BOOL)inVal
{
	mCanBecomeMainWindow = inVal;
}

/**
mCanBecomeKeyWindow�ݒ�
*/
- (void)setCanBecomeKeyWindow:(BOOL)inVal
{
	mCanBecomeKeyWindow = inVal;
}

/**
�}�E�X�C�x���gignore���邩�ǂ�����ݒ�
*/
/*
- (void)setIgnoresMouseEvents:(BOOL)inVal
{
	mIgnoresMouseEvents = inVal;
}
*/

/**
�E�C���h�E��frame��AGlobalRect�Ŏ擾
*/
- (AGlobalRect)getGlobalRectFromFrame:(NSRect)frame
{
	AGlobalRect	rect = {0};
	NSRect	contentFrame = [self contentRectForFrameRect:frame];
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	rect.left		= contentFrame.origin.x;
	rect.top		= firstScreenFrame.size.height - (contentFrame.origin.y + contentFrame.size.height);
	rect.right		= contentFrame.origin.x + contentFrame.size.width;
	rect.bottom		= firstScreenFrame.size.height - contentFrame.origin.y;
	return rect;
}

#pragma mark ===========================

#pragma mark ---�I�[�o�[���C�h

/**
FirstResponder��
*/
- (BOOL)acceptsFirstResponder
{
	return YES;
}

/**
MainWindow��
*/
- (BOOL)canBecomeMainWindow
{
	return mCanBecomeMainWindow;
}

/**
KeyWindow��
*/
- (BOOL)canBecomeKeyWindow
{
	return mCanBecomeKeyWindow;
}

/**
�}�E�X�C�x���gignore���邩�ǂ���
*/
/*
- (BOOL)ignoresMouseEvents
{
	return mIgnoresMouseEvents;
}
*/

#pragma mark ===========================

#pragma mark ---

/**
�q�I�[�o�[���C�E�C���h�E��zorder��mOverlayWindowLayer�ɏ]���čĔz�u
*/
- (void)reorderOverlayChildWindows
{
	NSArray*	childwindows = [self childWindows];
	AItemCount	childCount = [childwindows count];
	//�e���C���[���̃��[�v
	for( AOverlayWindowLayer overlayLayer = kOverlayWindowLayer_None; overlayLayer <= kOverlayWindowLayer_Top; overlayLayer++ )
	{
		//�e�q�E�C���h�E���Ƃ̃��[�v
		for( AIndex i = 0; i < childCount; i++ )
		{
			//�q�E�C���h�E�擾
			NSWindow*	w = [childwindows objectAtIndex:i];
			//�q�E�C���h�E��CocoaPanelWindow�̏ꍇ�Ɍ���A�E�C���h�E���Ĕz�u����B
			if( [w isKindOfClass:[CocoaPanelWindow class]] == YES )
			{
				CocoaPanelWindow*	childWindow = w;
				if( [childWindow getOverlayWindowLayer] == overlayLayer )
				{
					[self removeChildWindow:childWindow];
					[self addChildWindow:childWindow ordered:NSWindowAbove];
				}
			}
		}
	}
}

#pragma mark ===========================

#pragma mark ---�t�H���g�p�l��

/**
�t�H���g�p�l���\��
*/
- (void)showFontPanel:(NSFont*)inFont
{
	//�w��t�H���g���L��
	mCurrentFontPanelFont = inFont;
	//�w��t�H���g���t�H���g�p�l���ɐݒ�
	[[NSFontManager sharedFontManager] setSelectedFont:mCurrentFontPanelFont isMultiple:NO];
	//�t�H���g�p�l���\��
	[[NSFontPanel sharedFontPanel] orderFront:nil];
	//delegate, target�������ɐݒ肷��
	[[NSFontManager sharedFontManager] setDelegate:self];
	[[NSFontManager sharedFontManager] setTarget:self];
}

/**
�t�H���g�ύX�������iNSFontManager��delegate�j
*/
- (void)changeFont:(id)sender
{
	//Apple�h�L�������g�ɋL�ڂ���Ă���R�[�h
	NSFont *newFont = [sender convertFont:mCurrentFontPanelFont];
	mCurrentFontPanelFont = newFont;
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_ValueChanged()���s
	mWindowImp->APICB_CocoaDoFontPanelFontChanged();
	
	OS_CALLBACKBLOCK_END;
}

/**
���݂̃t�H���g�p�l���ɐݒ肳��Ă���t�H���g���擾
*/
- (NSFont*)getFontPanelFont
{
	return mCurrentFontPanelFont;
}

#pragma mark ===========================

#pragma mark ---�e��C�x���g�I�[�o�[���C�h

/**
MainWindow�ɂȂ����Ƃ��̏���
*/
- (void)becomeMainWindow
{
	//�p���������s
	[super becomeMainWindow];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowActivated()���s
	mWindowImp->APICB_CocoaBecomeMainWindow();
	
	OS_CALLBACKBLOCK_END;
}

/**
MainWindow�łȂ��Ȃ����Ƃ��̏���
*/
- (void)resignMainWindow
{
	//�p���������s
	[super resignMainWindow];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowDeactivated()���s
	mWindowImp->APICB_CocoaResignMainWindow();
	
	OS_CALLBACKBLOCK_END;
}

/**
KeyWindow�ɂȂ����Ƃ��̏���
*/
- (void)becomeKeyWindow
{
	//�p���������s
	[super becomeKeyWindow];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowActivated()���s
	mWindowImp->APICB_CocoaBecomeKeyWindow();
	
	OS_CALLBACKBLOCK_END;
}

/**
KeyWindow�łȂ��Ȃ����Ƃ��̏���
*/
- (void)resignKeyWindow
{
	//�p���������s
	[super resignKeyWindow];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowDeactivated()���s
	mWindowImp->APICB_CocoaResignKeyWindow();
	
	OS_CALLBACKBLOCK_END;
}

/**
Dock�����ꂽ�Ƃ��̏���
*/
- (void)miniaturize:(id)sender
{
	//�p���������s
	[super miniaturize:sender];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowCollapsed()���s
	mWindowImp->APICB_CocoaMiniaturize();
	
	OS_CALLBACKBLOCK_END;
}

/**
Zoom�{�^���N���b�N������
*/
- (void)zoom:(id)sender
{
	//�p���������s
	[super zoom:sender];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowResizeCompleted()���s
	mWindowImp->APICB_CocoaWindowResizeCompleted();
	
	OS_CALLBACKBLOCK_END;
}

/**
Frame�ݒ莞����
*/
- (void)setFrame:(NSRect)windowFrame display:(BOOL)displayViews
{
	//fprintf(stderr,"CocoaWindow#setFrame - start \n");
	//�ύX�Oframe�擾
	NSRect	oldFrame = [self frame];
	
	//�p���������s
	[super setFrame:windowFrame display:displayViews];
	
	//setup�O�ɌĂ΂ꂽ�ꍇ�́AAPICB_CocoaWindowSetFrame()�����s�����ɏI��
	if( mWindowImp == nil )
	{
		return;
	}
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowBoundsChanged()���s
	//AGlobalRect	oldFrameGlobalRect = [self getGlobalRectFromFrame:oldFrame];
	AGlobalRect	newFrameGlobalRect = [self getGlobalRectFromFrame:[self frame]];
	mWindowImp->APICB_CocoaWindowBoundsChanged(newFrameGlobalRect);
	
	OS_CALLBACKBLOCK_END;
	//fprintf(stderr,"CocoaWindow#setFrame - end \n");
}

/**
�L�����Z���L�[
*/
- (void)cancelOperation:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//modifierKeys�擾
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]);
	//�L�����Z���L�[�������s
	mWindowImp->APICB_CocoaDoCancelOperation(modifier);
	
	OS_CALLBACKBLOCK_END;
}

/**
�^�u�L�[���ɂ��L�[�t�H�[�J�X�ړ�������
*/
- (void)selectNextKeyView:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//�t�H�[�J�X�ړ����s���̃t���O��ݒ肷��
	mWindowImp->SetSwitchingFocusNow(true);
	
	//�p���������s
	[super selectNextKeyView:sender];
	
	//�t�H�[�J�X�ړ����s���̃t���O����������
	mWindowImp->SetSwitchingFocusNow(false);
	
	OS_CALLBACKBLOCK_END;
}

/**
�^�u�L�[���ɂ��L�[�t�H�[�J�X�ړ�������
*/
- (void)selectPreviousKeyView:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//�t�H�[�J�X�ړ����s���̃t���O��ݒ肷��
	mWindowImp->SetSwitchingFocusNow(true);
	
	//�p���������s
	[super selectPreviousKeyView:sender];
	
	//�t�H�[�J�X�ړ����s���̃t���O����������
	mWindowImp->SetSwitchingFocusNow(false);
	
	OS_CALLBACKBLOCK_END;
}

#pragma mark ===========================

#pragma mark ---�e��Action�R�[���o�b�N����

/**
�{�^����sentAction
*/
- (IBAction) doButtonAction:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//CocoaTabSelectButton�N���b�N�������i�^�u�I���j
	if( [sender isKindOfClass:[CocoaTabSelectButton class]] == YES )
	{
		CocoaTabSelectButton*	button = static_cast<CocoaTabSelectButton*>(sender);
		if( [button state] == NSOnState )
		{
			[button selectTabView];
		}
	}
	
	//CocoaDisclosureButton�N���b�N�������i�܂肽���݁j
	if( [sender isKindOfClass:[CocoaDisclosureButton class]] == YES )
	{
		CocoaDisclosureButton*	button = static_cast<CocoaDisclosureButton*>(sender);
		[button updateCollapsableView];
	}
	
	//�{�^��sentAction�������s
	mWindowImp->APICB_CocoaDoButtonAction([sender tag]);
	
	OS_CALLBACKBLOCK_END;
}

/**
�X�N���[���o�[�e�p�[�g�N���b�N������
*/
- (IBAction) doScrollbarAction:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	NSScroller*	scroller = static_cast<NSScroller*>(sender);
	AScrollBarPart	scrollbarPart = kScrollBarPart_None;
	switch([scroller hitPart])
	{
	  case NSScrollerDecrementLine:
		{
			scrollbarPart = kScrollBarPart_UpButton;
			break;
		}
	  case NSScrollerIncrementLine:
		{
			scrollbarPart = kScrollBarPart_DownButton;
			break;
		}
	  case NSScrollerDecrementPage:
		{
			scrollbarPart = kScrollBarPart_PageUpButton;
			break;
		}
	  case NSScrollerIncrementPage:
		{
			scrollbarPart = kScrollBarPart_PageDownButton;
			break;
		}
	  case NSScrollerKnob:
		{
			scrollbarPart = kScrollBarPart_ThumbTracking;
			break;
		}
	  default:
		{
			//��������
			break;
		}
	}
	
	//�{�^��sentAction�������s
	mWindowImp->APICB_DoScrollBarAction([sender tag],scrollbarPart);
	
	OS_CALLBACKBLOCK_END;
}


/**
�|�b�v�A�b�v���j���[��sentAction
*/
- (IBAction) doPopupMenuAction:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//�|�b�v�A�b�v���j���[sentAction�������s
	mWindowImp->APICB_CocoaDoPopupMenuAction([sender tag]);
	
	OS_CALLBACKBLOCK_END;
}

/**
�e�L�X�g�t�B�[���h��sentAction
*/
- (IBAction) doTextFieldAction:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//�e�L�X�g�t�B�[���h��sentAction�������s
	mWindowImp->APICB_CocoaDoTextFieldAction([sender tag]);
	
	OS_CALLBACKBLOCK_END;
}

/**
Stepper�l�ύX������
*/
- (IBAction) doStepper:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//stepper�I�u�W�F�N�g�擾
	NSStepper*	stepper = reinterpret_cast<NSStepper*>(sender);
	//�����E��������i���50�ɂ��Ă����āA49��51���ő������ۂ𔻒�j
	ABool	inc = true;
	if( [stepper intValue] < 50 )
	{
		inc = false;
	}
	[stepper setMinValue:0];
	[stepper setMaxValue:100];
	[stepper setIntValue:50];
	[stepper setIncrement:1];
	
	//�ΏۃR���g���[���擾�istepper��controlID����kControlID_StepperOffset���������l���ΏۃR���g���[����id�j
	AControlID	targetControlID = [sender tag]-kControlID_StepperOffset;
	//�ΏۃR���g���[���̒l�𑝌�
	ANumber	num = mWindowImp->GetNumber(targetControlID);
	if( inc == true )
	{
		mWindowImp->SetNumber(targetControlID,num+1);
	}
	else
	{
		mWindowImp->SetNumber(targetControlID,num-1);
	}
	//�ΏۃR���g���[���̒l�ύX������
	mWindowImp->APICB_CocoaDoTextFieldAction(targetControlID);
	
	OS_CALLBACKBLOCK_END;
}

/**
�c�[���o�[���ڂ�sentAction
*/
- (IBAction) doToolbarItemAction:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	NSInteger	tag = [sender tag];
	//segmented control�̏ꍇ�́Asegment����tag���擾
	if( [sender isKindOfClass:[NSSegmentedControl class]] == YES )
	{
		NSInteger clickedSegment = [sender selectedSegment];
		tag = [[sender cell] tagForSegment:clickedSegment];
	}
	//�c�[���o�[���ڂ�sentAction�������s
	mWindowImp->APICB_CocoaToolbarItemAction(tag);
	
	OS_CALLBACKBLOCK_END;
}

/**
mouse moved���A�q�E�C���h�E��ŊY������view����������B
@return �q�E�C���h�E��ŏ������s�ς݂Ȃ�true
*/
- (ABool)doMouseMovedForChildWindows:(NSPoint)inGloalPoint modifierKeys:(AModifierKeys)modifier
{
	//�q�E�C���h�E���hitTest���s��
	//�q�E�C���h�E���X�g�擾
	NSArray*	childwindows = [self childWindows];
	AItemCount	childCount = [childwindows count];
	//�E�C���h�E���C���[���̃��[�v�i�g�b�v���珇�Ɂj
	for( AOverlayWindowLayer overlayLayer = kOverlayWindowLayer_Top; overlayLayer >= kOverlayWindowLayer_None; overlayLayer-- )
	{
		//�e�q�E�C���h�E���Ƃ̃��[�v
		for( AIndex i = 0; i < childCount; i++ )
		{
			NSWindow*	w = [childwindows objectAtIndex:i];
			//�q�E�C���h�E���}�E�X�C�x���g�����Ȃ�ΏۊO
			if( [w ignoresMouseEvents] == YES )
			{
				continue;
			}
			//�q�E�C���h�E��CocoaPanelWindow�̏ꍇ�A�q�E�C���h�E���hitTest���s���Ahit�����炻��view�ɑ΂��Amouse moved �������s���B
			if( [w isKindOfClass:[CocoaPanelWindow class]] == YES )
			{
				CocoaPanelWindow*	childWindow = w;
				if( [childWindow getOverlayWindowLayer] == overlayLayer )
				{
					//�q�E�C���h�E��̍��W���擾
					NSPoint	ptInChildWindow = inGloalPoint;
					NSRect	childWindowFrame = [childWindow frame];
					ptInChildWindow.x -= childWindowFrame.origin.x;
					ptInChildWindow.y -= childWindowFrame.origin.y;
					//�q�E�C���h�E���hitTest
					NSView*	childContentView = [childWindow contentView];
					NSPoint ptInChildContentView = [childContentView convertPoint:ptInChildWindow fromView:nil];
					NSView*	view = [childContentView hitTest:ptInChildContentView];
					//view������������AWindow::EVT_DoMouseMoved()�����s���āA���^�[��
					if( view != nil )
					{
						//window ref���擾���Awindow ref����WindowImp���擾
						AWindowRef	winRef = childWindow;//#1290 [childWindow windowNumber];//#1275 (WindowRef)([childWindow windowRef]);
						if( CWindowImp::ExistWindowImpForWindowRef(winRef) == true )
						{
							//Local���W�ɕϊ�
							NSPoint curPoint = [view convertPoint:ptInChildWindow fromView:nil];
							ALocalPoint	localPoint = {curPoint.x,curPoint.y};
							CWindowImp::GetWindowImpByWindowRef(winRef).APICB_CocoaMouseMoved([view tag],localPoint,modifier);
						}
						return true;
					}
				}
			}
		}
	}
	return false;
}

/**
mouse moved������
*/
- (void)mouseMoved:(NSEvent *)inEvent
{
	//�E�C���h�E������������mouseMoved�����邱�Ƃ�����̂ŁA
	//�E�C���h�E��visible�łȂ���΁A�����������^�[��
	if( [self isVisible] == NO )
	{
		return;
	}
	
	OS_CALLBACKBLOCK_START(false);
	
	//modifierKeys�擾
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent(inEvent);
	
	//�}�E�X�ʒu���O���[�o�����W�ɕϊ�
	NSPoint	ptInGlobal = [inEvent locationInWindow];
	NSRect	winFrame = [self frame];
	ptInGlobal.x += winFrame.origin.x;
	ptInGlobal.y += winFrame.origin.y;
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	AGlobalPoint	globalPoint = {0};
	globalPoint.x = ptInGlobal.x;
	globalPoint.y = firstScreenFrame.size.height - ptInGlobal.y;
	//fprintf(stderr,"mouseModed(%d,%d) ",globalPoint.x,globalPoint.y);
	
	//�P�D�t���[�e�B���O�E�C���h�E��Ō������A�t���[�e�B���O��Ȃ�t���[�e�B���O�E�C���h�E��Ŏ��s
	CWindowImp*	floatingWindowImp = CWindowImp::FindFloatingWindowByGlobalPoint(globalPoint);
	if( floatingWindowImp != NULL )
	{
		[floatingWindowImp->GetCocoaObjects().GetWindow() doMouseMovedForThisWindow:ptInGlobal modifierKeys:modifier];
	}
	else
	{
		//�Q�D�e�E�C���h�E�i���g���e�Ȃ玩�E�C���h�E�j��ŁA�q�E�C���h�E��ŊY��view�̌������s���Ahit������q�E�C���h�E�ŏ�������B
		NSWindow*	rootwin = self;
		if( [self parentWindow] != nil )
		{
			rootwin = [self parentWindow];
		}
		if( [rootwin doMouseMovedForChildWindows:ptInGlobal modifierKeys:modifier] == false )
		{
			//�R�D�q�E�C���h�E��Ŗ����s�Ȃ�A���E�C���h�E�Ŏ��s
			[self doMouseMovedForThisWindow:ptInGlobal modifierKeys:modifier];
		}
	}
	
	OS_CALLBACKBLOCK_END;
}

/**
mouse moved�������i���̃E�C���h�E�ł̌����Ȃ��B�K�����E�C���h�E�Ŏ��s�j
*/
- (void)doMouseMovedForThisWindow:(NSPoint)inGloalPoint modifierKeys:(AModifierKeys)modifier
{
	//�E�C���h�E���|�C���g�֕ϊ�
	NSPoint	ptInWindow = inGloalPoint;
	NSRect	windowFrame = [self frame];
	ptInWindow.x -= windowFrame.origin.x;
	ptInWindow.y -= windowFrame.origin.y;
	//hitTest���s���A�Ή�����view������
	NSView*	contentView = [self contentView];
	NSPoint ptInContentView = [contentView convertPoint:ptInWindow fromView:nil];
	NSView*	view = [contentView hitTest:ptInContentView];
	if( view != nil )
	{
		//view�����݂���ꍇ
		
		//fprintf(stderr,"%d ",[view tag]);
		
		//Local���W�ɕϊ�
		NSPoint curPoint = [view convertPoint:ptInWindow fromView:nil];
		ALocalPoint	localPoint = {curPoint.x,curPoint.y};
		//AWindow::EVT_DoMouseMoved()�����s
		mWindowImp->APICB_CocoaMouseMoved([view tag],localPoint,modifier);
	}
	else
	{
		//view�����݂��Ȃ��ꍇ
		
		ALocalPoint	localPoint = {0,0};
		//modifierKeys�擾
		//AWindow::EVT_DoMouseMoved()�����s�i��������kControlID_Invalid�ɂ���j
		mWindowImp->APICB_CocoaMouseMoved(kControlID_Invalid,localPoint,modifier);
	}
}

/**
�c�[���o�[�\���^��\���ؑ֎�����
*/
- (void)toggleToolbarShown:(id)sender
{
	//�p������
	[super toggleToolbarShown:sender];
	
	OS_CALLBACKBLOCK_START(false);
	
	//bounds�ύX������
	AGlobalRect	newFrameGlobalRect = [self getGlobalRectFromFrame:[self frame]];
	mWindowImp->APICB_CocoaWindowBoundsChanged(newFrameGlobalRect);
	
	OS_CALLBACKBLOCK_END;
}

#pragma mark ===========================

#pragma mark ---Open/Save�V�[�g
//#1034

/**
�t�H���_�I���_�C�A���O�^�V�[�g�\��
*/
- (void)showChooseFolderWindow:(NSString*)dirPath message:(NSString*)message isSheet:(ABool)isSheet
{
	//OpenPanel�擾
	NSOpenPanel* panel = [NSOpenPanel openPanel];
	[panel setCanChooseFiles:NO];
	[panel setCanChooseDirectories:YES];
	[panel setResolvesAliases:YES];
	[panel setAllowsMultipleSelection:NO];
	[panel setExtensionHidden:NO];
	[panel setTreatsFilePackagesAsDirectories:YES];
	[panel setShowsHiddenFiles:YES];
	if( message != nil )
	{
		[panel setMessage:message];
	}
	
	//OpenPanel�\��
	if( isSheet == false )
	{
		//���[�_���_�C�A���O
		int	returnCode = [panel runModal];
		[self showChooseFolderWindowDidEnd:panel returnCode:returnCode contextInfo:nil];
	}
	else
	{
		//�V�[�g
		[panel beginSheetForDirectory:dirPath file:nil modalForWindow:self 
			modalDelegate:self didEndSelector:@selector(showChooseFolderWindowDidEnd:returnCode:contextInfo:) 
		contextInfo:nil];
	}
}

/**
�t�H���_�I���_�C�A���O�^�V�[�g����������
*/
- (void)showChooseFolderWindowDidEnd:(NSSavePanel *)panel returnCode:(int)returnCode contextInfo:(void *)contextInfo;
{
	OS_CALLBACKBLOCK_START(false);
	
	//OK/Cancel����
	if( returnCode == NSFileHandlingPanelOKButton )
	{
		//���ʎ擾
		NSArray*	urls = [panel URLs];
		if( [urls count] > 0 )
		{
			NSURL*	url = [urls objectAtIndex:0];
			AText	path;
			path.SetCstring([[url path] UTF8String]);
			//WindowImp�֒ʒm
			mWindowImp->APICB_ShowChooseFolderWindowAction(path);
		}
	}
	else
	{
		//WindowImp�֒ʒm�i�V�[�g�\����ԉ����̂��߁j
		mWindowImp->APICB_ShowChooseFolderWindowAction(GetEmptyText());
	}
	
	OS_CALLBACKBLOCK_END;
}

/**
�t�@�C���I���_�C�A���O�^�V�[�g�\��
*/
- (void)showChooseFileWindow:(NSString*)dirPath message:(NSString*)message isSheet:(ABool)isSheet onlyApp:(ABool)onlyApp
{
	//��onlyApp������
	
	//OpenPanel�擾
	NSOpenPanel* panel = [NSOpenPanel openPanel];
	[panel setCanChooseFiles:YES];
	[panel setCanChooseDirectories:NO];
	[panel setResolvesAliases:YES];
	[panel setAllowsMultipleSelection:NO];
	[panel setExtensionHidden:NO];
	[panel setTreatsFilePackagesAsDirectories:YES];
	[panel setShowsHiddenFiles:YES];
	if( message != nil )
	{
		[panel setMessage:message];
	}
	
	//OpenPanel�\��
	if( isSheet == false )
	{
		//���[�_���_�C�A���O
		int	returnCode = [panel runModal];
		[self showChooseFileWindowDidEnd:panel returnCode:returnCode contextInfo:nil];
	}
	else
	{
		//�V�[�g
		[panel beginSheetForDirectory:dirPath file:nil modalForWindow:self 
			modalDelegate:self didEndSelector:@selector(showChooseFileWindowDidEnd:returnCode:contextInfo:) 
		contextInfo:nil];
	}
}

/**
�t�@�C���I���_�C�A���O�^�V�[�g����������
*/
- (void)showChooseFileWindowDidEnd:(NSSavePanel *)panel returnCode:(int)returnCode contextInfo:(void *)contextInfo;
{
	OS_CALLBACKBLOCK_START(false);
	
	//OK/Cancel����
	if( returnCode == NSFileHandlingPanelOKButton )
	{
		//���ʎ擾
		NSArray*	urls = [panel URLs];
		if( [urls count] > 0 )
		{
			NSURL*	url = [urls objectAtIndex:0];
			AText	path;
			path.SetCstring([[url path] UTF8String]);
			//WindowImp�֒ʒm
			mWindowImp->APICB_ShowChooseFileWindowAction(path);
		}
	}
	else
	{
		//WindowImp�֒ʒm�i�V�[�g�\����ԉ����̂��߁j
		mWindowImp->APICB_ShowChooseFileWindowAction(GetEmptyText());
	}
	
	OS_CALLBACKBLOCK_END;
}

/**
�ۑ��V�[�g�\��
*/
- (void)showSaveWindow:(NSString*)dirPath filename:(NSString*)filename
{
	//SavePanel�擾
	NSSavePanel* panel = [NSSavePanel savePanel];
	[panel setAllowsOtherFileTypes:YES];
	[panel setCanCreateDirectories:YES];
	[panel setCanSelectHiddenExtension:NO];
	[panel setTreatsFilePackagesAsDirectories:YES];
	[panel setShowsHiddenFiles:YES];
	[panel setExtensionHidden:NO];
	//�t�@�C�����ݒ�
	NSString*	fn = nil;
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_6 )
	{
		[panel setNameFieldStringValue:filename];
	}
	else
	{
		//10.6�����̏ꍇ��beginSheetForDirectory�̈����Ńt�@�C�����w��
		fn = filename;
	}
	
	//�V�[�g
	[panel beginSheetForDirectory:dirPath file:fn modalForWindow:self 
		modalDelegate:self didEndSelector:@selector(showSaveWindowDidEnd:returnCode:contextInfo:) 
	contextInfo:nil];
}

/**
�ۑ��V�[�g����������
*/
- (void)showSaveWindowDidEnd:(NSSavePanel *)panel returnCode:(int)returnCode contextInfo:(void *)contextInfo;
{
	OS_CALLBACKBLOCK_START(false);
	
	//OK/Cancel����
	if( returnCode == NSFileHandlingPanelOKButton )
	{
		//���ʎ擾
		NSURL*	url = [panel URL];
		AText	path;
		path.SetCstring([[url path] UTF8String]);
		//WindowImp�֒ʒm
		mWindowImp->APICB_ShowSaveWindowAction(path);
	}
	else
	{
		//WindowImp�֒ʒm�i�V�[�g�\����ԉ����̂��߁j
		mWindowImp->APICB_ShowSaveWindowAction(GetEmptyText());
	}
	
	OS_CALLBACKBLOCK_END;
}
#pragma mark ===========================

#pragma mark ---�V�[�g�I��������

/**
�V�[�g�I��������
*/
- (void)didEndSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	[self orderOut:nil];
}

#pragma mark ===========================

#pragma mark ---�c�[���o�[�֘A

/**
�c�[���o�[���ڂ��ǉ������Ƃ��̏���
*/
- (void) toolbarWillAddItem:(NSNotification *)notification
{
	OS_CALLBACKBLOCK_START(false);
	
	mAddingToolbarItem = [[notification userInfo] objectForKey: @"item"];
	
	//�{�^��sentAction�������s
	mWindowImp->APICB_CocoaDoToolbarItemAddedAction([mAddingToolbarItem tag]);
	
	mAddingToolbarItem = nil;
	
	OS_CALLBACKBLOCK_END;
}

/**
�c�[���o�[�폜������
*/
- (void)toolbarDidRemoveItem:(NSNotification *)notification
{
	OS_CALLBACKBLOCK_START(false);
	
	//�{�^��sentAction�������s
	mWindowImp->APICB_CocoaDoToolbarItemAddedAction(kControlID_Invalid);
	
	OS_CALLBACKBLOCK_END;
}

/**
�ǉ������c�[���o�[���ڂ��擾
*/
- (NSToolbarItem*)getAddingToolbarItem
{
	return mAddingToolbarItem;
}

/**
�w��control��segmented control���ǂ������擾
*/
- (BOOL)isSegmentControl:(AControlID)inControlID
{
	AIndex	index = mSegmentedControlArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		return YES;
	}
	return NO;
}

/**
segmented control�ɒl�ݒ�
*/
- (BOOL)setBoolForSegmentControl:(AControlID)inControlID select:(BOOL)inSelect
{
	AIndex	index = mSegmentedControlArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		NSSegmentedControl*	segmentedControl = mSegmentedControlArray_SegmentedControl.Get(index);
		NSInteger	segment = mSegmentedControlArray_Segment.Get(index);
		[[segmentedControl cell] setSelected:inSelect forSegment:segment];
		return YES;
	}
	return NO;
}

/**
segmented control��enable/disable
*/
- (BOOL)setEnableForSegmentControl:(AControlID)inControlID enable:(BOOL)inEnable
{
	AIndex	index = mSegmentedControlArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		NSSegmentedControl*	segmentedControl = mSegmentedControlArray_SegmentedControl.Get(index);
		NSInteger	segment = mSegmentedControlArray_Segment.Get(index);
		[[segmentedControl cell] setEnabled:inEnable forSegment:segment];
		return YES;
	}
	return NO;
}

/**
segmented control��enable/disable��Ԃ��擾
*/
- (BOOL)getEnableForSegmentControl:(AControlID)inControlID
{
	AIndex	index = mSegmentedControlArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		NSSegmentedControl*	segmentedControl = mSegmentedControlArray_SegmentedControl.Get(index);
		NSInteger	segment = mSegmentedControlArray_Segment.Get(index);
		return [[segmentedControl cell] isEnabledForSegment:segment];
	}
	return NO;
}





//==================�c�[���o�[�ݒ�==================
//�A�v���ėp���ł��Ă��Ȃ�

/**
�c�[���o�[�S����
*/
- (NSArray *) toolbarAllowedItemIdentifiers: (NSToolbar *) toolbar 
{
	switch( mToolbarType )
	{
	  case 0:
		{
			return [NSArray arrayWithObjects: 
			NSToolbarSpaceItemIdentifier,
			NSToolbarFlexibleSpaceItemIdentifier,
			@"TB_SubWindow",
			@"TB_Document",
			@"TB_TextEncoding",
			@"TB_ReturnCode",
			@"TB_WrapMode",
			@"TB_Mode",
			//@"TB_Diff",
			@"TB_Nav",
			//@"TB_Keyword",
			@"TB_LayoutLeft",
			@"TB_LayoutRight",
			@"TB_Split",
			@"TB_KeyRecord",
			@"TB_ProhibitPopup",
			@"TB_ShowHideFloating",
			@"TB_Pref",
			@"TB_MultiFileFind",
			//@"TB_MultiFileFindExecute",
			@"TB_Find",
			@"TB_Marker",
			@"TB_SearchField",
			@"TB_Commit",
				@"TB_Save",//#1389
				@"TB_UndoRedo",//#1389
				@"TB_FindDialog",//#1389
				@"TB_Copy",//#1400
				@"TB_Cut",//#1400
				@"TB_Paste",//#1400
			nil];
			break;
		}
		//apppref
	  case 1:
		{
			return [NSArray arrayWithObjects: 
			@"999000",
			@"999009",
			@"999010",
			@"999001",
			@"999002",
			@"999003",
			@"999004",
			@"999005",
			@"999006",
			@"999008",
			@"999007",
			nil];
			break;
		}
		//modepref
	  case 2:
		{
			return [NSArray arrayWithObjects: 
			@"999000",
			@"999001",
			@"999002",
			@"999011",
			@"999003",
			@"999004",
			@"999005",
			@"999006",
			@"999008",
			@"999009",
			@"999007",
			@"999010",
			nil];
			break;
		}
	}
	return [[NSArray alloc] init];
}

/**
�c�[���o�[�f�t�H���g����
*/
- (NSArray *) toolbarDefaultItemIdentifiers: (NSToolbar *) toolbar 
{
	switch( mToolbarType )
	{
	  case 0:
		{
			return [NSArray arrayWithObjects: 
			@"TB_Document",
			@"TB_SubWindow",
			@"TB_TextEncoding",
			@"TB_ReturnCode",
			//@"TB_WrapMode",
			@"TB_Mode",
			//@"TB_Diff",
			//@"TB_Nav",
			//NSToolbarSpaceItemIdentifier,
			@"TB_Split",
			@"TB_Nav",
			//NSToolbarSpaceItemIdentifier,
			//@"TB_KeyRecord",
			NSToolbarFlexibleSpaceItemIdentifier,
			//@"TB_Nav",
			//@"TB_Keyword",
			@"TB_Commit",
			//@"TB_ProhibitPopup",
			//@"TB_ShowHideFloating",
			@"TB_Pref",
			@"TB_LayoutRight",
			NSToolbarSpaceItemIdentifier,
			@"TB_MultiFileFind",
			//@"TB_Find",
			//@"TB_Marker",
			@"TB_SearchField",
			nil];
			break;
		}
		//apppref
	  case 1:
		{
			return [NSArray arrayWithObjects: 
			@"999000",
			@"999009",
			@"999010",
			@"999001",
			@"999002",
			@"999003",
			@"999004",
			@"999005",
			@"999006",
			@"999008",
			@"999007",
			nil];
			break;
		}
		//modepref
	  case 2:
		{
			return [NSArray arrayWithObjects: 
			@"999000",
			@"999001",
			@"999002",
			@"999011",
			@"999003",
			@"999004",
			@"999005",
			@"999006",
			@"999008",
			@"999009",
			@"999007",
			@"999010",
			nil];
			break;
		}
	}
	return [[NSArray alloc] init];
}

/**
�I���\�c�[���o�[����
*/
- (NSArray *)toolbarSelectableItemIdentifiers: (NSToolbar *)toolbar
{
	switch( mToolbarType )
	{
		//apppref
	  case 1:
		{
			return [NSArray arrayWithObjects: 
			@"999000",
			@"999009",
			@"999010",
			@"999001",
			@"999002",
			@"999003",
			@"999004",
			@"999005",
			@"999006",
			@"999008",
			@"999007",
			nil];
			break;
		}
		//modepref
	  case 2:
		{
			return [NSArray arrayWithObjects: 
			@"999000",
			@"999001",
			@"999002",
			@"999011",
			@"999003",
			@"999004",
			@"999005",
			@"999006",
			@"999008",
			@"999009",
			@"999007",
			@"999010",
			nil];
			break;
		}
	}
	return [[NSArray alloc] init];
}

//�e���ڃT�C�Y
const ANumber	kWidth_Toolbar1Button = 34;
const ANumber	kWidth_Toolbar2Button = 80;
const ANumber	kWidth_Toolbar3Button = 120;
const ANumber	kWidth_Toolbar4Button = 160;
const ANumber	kHeight_ToolbarButton = 25;//24�ȉ��ɂ���ƃ{�^���ʒu�������H
const ANumber	kWidth_ToolbarImageButton = 32;
const ANumber	kHeight_ToolbarImageButton = 32;

/**
�c�[���o�[���ځi�C���[�W�{�^���j����
*/
-(NSToolbarItem*)createToolbarItem_ImageButton:(NSString *)itemIdentifier 
tag:(NSInteger)tag image:(NSImage*)image labelText:(const char*)labelText
toggleMode:(BOOL)toggleMode
{
	
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	NSRect	rect = {0};
	rect.size.width = kWidth_ToolbarImageButton;
	rect.size.height = kHeight_ToolbarImageButton;
	//
	NSSize	minsize = {0};
	minsize.width = kWidth_ToolbarImageButton;
	minsize.height = kHeight_ToolbarImageButton;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = kWidth_ToolbarImageButton;
	maxsize.height = kHeight_ToolbarImageButton;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	[toolbarItem setTag:tag];
	[toolbarItem setImage:image];
	return toolbarItem;
}

/**
�c�[���o�[���ځi�P�{�^���j����
*/
-(NSToolbarItem*)createToolbarItem_Button:(NSString *)itemIdentifier 
tag:(NSInteger)tag image:(NSImage*)image labelText:(const char*)labelText
toggleMode:(BOOL)toggleMode
{
	
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//rect�̒l��ݒ肵�Ă��Ӗ��������悤�����A�O�̂��ߕ��ƍ�����ݒ肷��
	NSRect	rect = {0};
	rect.size.width = kWidth_Toolbar1Button;
	rect.size.height = kHeight_ToolbarButton;
	//�{�^������
	NSButton*	button = [[NSButton alloc] initWithFrame:rect];
	//
	if( toggleMode == YES )
	{
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
		{
			[button setButtonType:NSSwitchButton];
		}
		else
		{
			[button setButtonType:NSPushOnPushOffButton];
		}
		[button setTitle:@""];
	}
	else
	{
		[button setButtonType:NSMomentaryPushInButton];
	}
	[button setImagePosition:NSImageOnly];
	//
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
	{
		[button setBezelStyle:NSTexturedRoundedBezelStyle];
	}
	else
	{
		[button setBezelStyle:NSRegularSquareBezelStyle];
	}
	[button setTag:tag];
	[button setImage:image];
	[button setBordered:YES];
	//
	NSSize	minsize = {0};
	minsize.width = kWidth_Toolbar1Button;
	minsize.height = kHeight_ToolbarButton-2;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = kWidth_Toolbar1Button;
	maxsize.height = kHeight_ToolbarButton;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setView:button];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	[toolbarItem setTag:tag];
	return toolbarItem;
	
	/*
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//rect�̒l��ݒ肵�Ă��Ӗ��������悤�����A�O�̂��ߕ��ƍ�����ݒ肷��
	NSRect	rect = {0};
	rect.size.width = kWidth_Toolbar1Button-16;
	rect.size.height = kHeight_ToolbarButton;
	//
	NSSegmentedControl*	segment = [[NSSegmentedControl alloc] initWithFrame:rect];
	[segment setSegmentCount:1];
	[[segment cell] setTag:tag forSegment:0];
	[segment setImage:image forSegment:0];
	//
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		[segment setSegmentStyle:NSSegmentStyleAutomatic];
	}
	[[segment cell] setTrackingMode:NSSegmentSwitchTrackingMomentary];
	//
	NSSize	minsize = {0};
	minsize.width = kWidth_Toolbar1Button;
	minsize.height = kHeight_ToolbarButton;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = kWidth_Toolbar1Button;
	maxsize.height = kHeight_ToolbarButton;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setView:segment];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	return toolbarItem;
	*/
}

/**
�c�[���o�[���ځi�Q�{�^���j����
*/
-(NSToolbarItem*)createToolbarItem_2Buttons:(NSString *)itemIdentifier 
tag1:(NSInteger)tag1 image1:(NSImage*)image1
tag2:(NSInteger)tag2 image2:(NSImage*)image2
labelText:(const char*)labelText
{
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//rect�̒l��ݒ肵�Ă��Ӗ��������悤�����A�O�̂��ߕ��ƍ�����ݒ肷��
	NSRect	rect = {0};
	rect.size.width = kWidth_Toolbar2Button;
	rect.size.height = kHeight_ToolbarButton;
	//
	NSSegmentedControl*	segment = [[NSSegmentedControl alloc] initWithFrame:rect];
	[segment setSegmentCount:2];
	[[segment cell] setTag:tag1 forSegment:0];
	[[segment cell] setTag:tag2 forSegment:1];
	[segment setImage:image1 forSegment:0];
	[segment setImage:image2 forSegment:1];
	[segment setWidth:0 forSegment:0];
	[segment setWidth:0 forSegment:1];
	//
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		[segment setSegmentStyle:NSSegmentStyleAutomatic];
	}
	[[segment cell] setTrackingMode:NSSegmentSwitchTrackingMomentary];
	//
	NSSize	minsize = {0};
	minsize.width = kWidth_Toolbar2Button;
	minsize.height = kHeight_ToolbarButton;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = kWidth_Toolbar2Button;
	maxsize.height = kHeight_ToolbarButton;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setView:segment];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	//
	mSegmentedControlArray_ControlID.Add(tag1);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(0);
	//
	mSegmentedControlArray_ControlID.Add(tag2);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(1);
	return toolbarItem;
}

/**
�c�[���o�[���ځi�R�{�^���j����
*/
-(NSToolbarItem*)createToolbarItem_3Buttons:(NSString *)itemIdentifier 
tag1:(NSInteger)tag1 image1:(NSImage*)image1
tag2:(NSInteger)tag2 image2:(NSImage*)image2
tag3:(NSInteger)tag3 image3:(NSImage*)image3
labelText:(const char*)labelText
{
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//rect�̒l��ݒ肵�Ă��Ӗ��������悤�����A�O�̂��ߕ��ƍ�����ݒ肷��
	NSRect	rect = {0};
	rect.size.width = kWidth_Toolbar3Button;
	rect.size.height = kHeight_ToolbarButton;
	//
	NSSegmentedControl*	segment = [[NSSegmentedControl alloc] initWithFrame:rect];
	[segment setSegmentCount:3];
	[[segment cell] setTag:tag1 forSegment:0];
	[[segment cell] setTag:tag2 forSegment:1];
	[[segment cell] setTag:tag3 forSegment:2];
	[segment setImage:image1 forSegment:0];
	[segment setImage:image2 forSegment:1];
	[segment setImage:image3 forSegment:2];
	//
	[[segment cell] setTrackingMode:NSSegmentSwitchTrackingMomentary];
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		[segment setSegmentStyle:NSSegmentStyleAutomatic];
	}
	//
	NSSize	minsize = {0};
	minsize.width = kWidth_Toolbar3Button;
	minsize.height = kHeight_ToolbarButton;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = kWidth_Toolbar3Button;
	maxsize.height = kHeight_ToolbarButton;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setView:segment];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	//
	mSegmentedControlArray_ControlID.Add(tag1);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(0);
	//
	mSegmentedControlArray_ControlID.Add(tag2);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(1);
	//
	mSegmentedControlArray_ControlID.Add(tag3);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(2);
	return toolbarItem;
}

/**
�c�[���o�[���ځi�S�{�^���j����
*/
-(NSToolbarItem*)createToolbarItem_4Buttons:(NSString *)itemIdentifier 
tag1:(NSInteger)tag1 image1:(NSImage*)image1
tag2:(NSInteger)tag2 image2:(NSImage*)image2
tag3:(NSInteger)tag3 image3:(NSImage*)image3
tag4:(NSInteger)tag4 image4:(NSImage*)image4
labelText:(const char*)labelText
toggleMode:(BOOL)toggleMode
{
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//rect�̒l��ݒ肵�Ă��Ӗ��������悤�����A�O�̂��ߕ��ƍ�����ݒ肷��
	NSRect	rect = {0};
	rect.size.width = kWidth_Toolbar4Button;
	rect.size.height = kHeight_ToolbarButton;
	//
	NSSegmentedControl*	segment = [[NSSegmentedControl alloc] initWithFrame:rect];
	[segment setSegmentCount:4];
	[[segment cell] setTag:tag1 forSegment:0];
	[[segment cell] setTag:tag2 forSegment:1];
	[[segment cell] setTag:tag3 forSegment:2];
	[[segment cell] setTag:tag4 forSegment:3];
	[segment setImage:image1 forSegment:0];
	[segment setImage:image2 forSegment:1];
	[segment setImage:image3 forSegment:2];
	[segment setImage:image4 forSegment:3];
	/*
	//
	[[segment cell] setToolTip:@"test0" forSegment:0];
	[[segment cell] setToolTip:@"test1" forSegment:1];
	[[segment cell] setToolTip:@"test2" forSegment:2];
	[[segment cell] setToolTip:@"test3" forSegment:3];
	*/
	//
	if( toggleMode == YES )
	{
		[[segment cell] setTrackingMode:NSSegmentSwitchTrackingSelectOne];
	}
	else
	{
		[[segment cell] setTrackingMode:NSSegmentSwitchTrackingMomentary];
	}
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		[segment setSegmentStyle:NSSegmentStyleAutomatic];
	}
	//
	NSSize	minsize = {0};
	minsize.width = kWidth_Toolbar4Button;
	minsize.height = kHeight_ToolbarButton;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = kWidth_Toolbar4Button;
	maxsize.height = kHeight_ToolbarButton;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setView:segment];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	//
	mSegmentedControlArray_ControlID.Add(tag1);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(0);
	//
	mSegmentedControlArray_ControlID.Add(tag2);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(1);
	//
	mSegmentedControlArray_ControlID.Add(tag3);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(2);
	//
	mSegmentedControlArray_ControlID.Add(tag4);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(3);
	return toolbarItem;
}

/**
�c�[���o�[���ځi�|�b�v�A�b�v�{�^���j����
*/
-(NSToolbarItem*)createToolbarItem_PopupButton:(NSString *)itemIdentifier  width:(ANumber)width
tag:(NSInteger)tag image:(NSImage*)image labelText:(const char*)labelText
{
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//�{�^����frame�T�C�Y��ݒ�
	NSRect	rect = {0};
	rect.size.width = width;
	rect.size.height = kHeight_ToolbarButton;
	//�|�b�v�A�b�v�{�^������
	NSPopUpButton*	button = [[[NSPopUpButton alloc] initWithFrame:rect pullsDown:YES] autorelease];
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
	{
		[button setBezelStyle:NSTexturedRoundedBezelStyle];
	}
	else
	{
		[button setBezelStyle:NSRoundedBezelStyle];
	}
	[button setTag:tag];
	[button setBordered:YES];
	[[button cell] setArrowPosition:NSPopUpArrowAtBottom];
	//enable/disable�͎蓮�X�V
	[[button menu] setAutoenablesItems:NO];
	//
	NSMenuItem*	menuItem = [ACocoa::GetNSMenu([button menu]) insertItemWithTitle:@"" action:nil keyEquivalent:@"" atIndex:0];
	if( image != nil )
	{
		[menuItem setImage:image];
	}
	/*setView���Ƀ{�^���̃T�C�Y�ɍ��킹�ăc�[���o�[���ڂ̃T�C�Y�͐ݒ肳���B
	NSSize	minsize = {0};
	minsize.width = width;
	minsize.height = kHeight_ToolbarButton;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = width;
	maxsize.height = kHeight_ToolbarButton;
	[toolbarItem setMaxSize:maxsize];
	*/
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setView:button];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	[toolbarItem setTag:tag];
	return toolbarItem;
}

/**
�c�[���o�[���ځi�����{�b�N�X�j����
*/
-(NSToolbarItem*)createToolbarItem_SearchBox:(NSString *)itemIdentifier  minwidth:(ANumber)minwidth maxwidth:(ANumber)maxwidth
tag:(NSInteger)tag labelText:(const char*)labelText
{
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//rect�̒l��ݒ肵�Ă��Ӗ��������悤�����A�O�̂��ߕ��ƍ�����ݒ肷��
	NSRect	rect = {0};
	rect.size.width = maxwidth;
	rect.size.height = kHeight_ToolbarButton-1;
	//�T�[�`�t�B�[���h����
	NSSearchField*	searchfield = [[[NSSearchField alloc] initWithFrame:rect] autorelease];
	[[searchfield cell] setSendsWholeSearchString:YES];
	[searchfield setTag:tag];
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == false )
	{
		[[searchfield cell] setRecentsAutosaveName:labelstr.GetNSString()];
		[[searchfield cell] setMaximumRecents:20];
		
		NSMenu *cellMenu = [[NSMenu alloc] initWithTitle:@"SearchBox"];
		
		AText	recentTitleMenuItemText;
		recentTitleMenuItemText.SetLocalizedText("SearchBoxMenu_Recent");
		AStCreateNSStringFromAText	recentTitleMenuItemStr(recentTitleMenuItemText);
		NSMenuItem*	recentTitleMenuItem = [[NSMenuItem alloc] initWithTitle:recentTitleMenuItemStr.GetNSString() 
																	 action:NULL keyEquivalent:@""];
		[recentTitleMenuItem setTag:NSSearchFieldRecentsTitleMenuItemTag];
		[cellMenu insertItem:recentTitleMenuItem atIndex:0];
		
		AText	recentSearchMenuItemText;
		recentSearchMenuItemText.SetLocalizedText("SearchBoxMenu_Recent");
		AStCreateNSStringFromAText	recentSearchMenuItemStr(recentSearchMenuItemText);
		NSMenuItem*	recentSearchMenuItem = [[NSMenuItem alloc] initWithTitle:recentSearchMenuItemStr.GetNSString() 
																	  action:NULL keyEquivalent:@""];
		[recentSearchMenuItem setTag:NSSearchFieldRecentsMenuItemTag];
		[cellMenu insertItem:recentSearchMenuItem atIndex:1];
		
		NSMenuItem*	separatorMenuItem = [NSMenuItem separatorItem];
		[separatorMenuItem setTag:NSSearchFieldRecentsTitleMenuItemTag];
		[cellMenu insertItem:separatorMenuItem atIndex:2];
		
		AText	clearMenuItemText;
		clearMenuItemText.SetLocalizedText("SearchBoxMenu_Clear");
		AStCreateNSStringFromAText	clearMenuItemStr(clearMenuItemText);
		NSMenuItem*	clearMenuItem = [[NSMenuItem alloc] initWithTitle:clearMenuItemStr.GetNSString() 
															   action:NULL keyEquivalent:@""];
		[clearMenuItem setTag:NSSearchFieldClearRecentsMenuItemTag];
		[cellMenu insertItem:clearMenuItem atIndex:3];
		
		[[searchfield cell] setSearchMenuTemplate:cellMenu];
	}
	NSSize	minsize = {0};
	minsize.width = minwidth;
	minsize.height = kHeight_ToolbarButton-1;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = maxwidth;
	maxsize.height = kHeight_ToolbarButton-1;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setView:searchfield];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	[toolbarItem setTag:tag];
	return toolbarItem;
}

/**
�c�[���o�[���ڐ���
*/
-(NSToolbarItem *)toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSString *)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag
{
	switch( mToolbarType )
	{
	  case 0:
		{
			if( [itemIdentifier compare:@"TB_TextEncoding"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:110 tag:995001 image:nil labelText:"TB_TextEncoding"];
			}
			if( [itemIdentifier compare:@"TB_ReturnCode"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:110 tag:995002 image:nil labelText:"TB_ReturnCode"];
			}
			if( [itemIdentifier compare:@"TB_WrapMode"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:110 tag:995003 image:nil labelText:"TB_WrapMode"];
			}
			if( [itemIdentifier compare:@"TB_Mode"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:110 tag:995004 image:nil labelText:"TB_Mode"];
			}
			if( [itemIdentifier compare:@"TB_Diff"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:110 tag:995005 image:nil labelText:"TB_Diff"];
			}
			if( [itemIdentifier compare:@"TB_Document"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:42 tag:995013 
				image:[NSImage imageNamed:@"iconTbDocument.Template"] labelText:"TB_Document"];
			}
			if( [itemIdentifier compare:@"TB_SubWindow"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:42 tag:995027
				image:[NSImage imageNamed:@"iconTbSubwindows.Template"] labelText:"TB_SubWindow"];
				//pdf���ƃA�C�R�����\������Ȃ��̂œ���png�ɂ��Ă����ipdf: iconTbSubwindows.Template�j�����������̂�pdf�ɂ���B
			}
			if( [itemIdentifier compare:@"TB_Pref"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:42 tag:995012
				image:[NSImage imageNamed:@"iconTbPreference.Template"] labelText:"TB_Pref"];
			}
			if( [itemIdentifier compare:@"TB_MultiFileFind"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995014 
				image:[NSImage imageNamed:@"iconTbFolderMagnifier.Template"]
				labelText:"TB_MultiFileFind" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_MultiFileFindExecute"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995033 
				image:[NSImage imageNamed:@"iconTbFolderMagnifier.Template"]
				labelText:"TB_MultiFileFindExecute" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_Find"] == NSOrderedSame )
			{
				return [self createToolbarItem_2Buttons:itemIdentifier 
				tag1:995016 image1:[NSImage imageNamed:@"iconTbMagnifierUp.Template"]
				tag2:995015 image2:[NSImage imageNamed:@"iconTbMagnifierDown.Template"]
				labelText:"TB_Find"];
			}
			if( [itemIdentifier compare:@"TB_Marker"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:42 tag:995017 
				image:[NSImage imageNamed:@"iconTbMarkerPen.Template"] labelText:"TB_Marker"];
			}
			if( [itemIdentifier compare:@"TB_Nav"] == NSOrderedSame )
			{
				return [self createToolbarItem_2Buttons:itemIdentifier 
				tag1:995018 image1:[NSImage imageNamed:@"NSGoLeftTemplate"]//NSGoLeftTemplate"]
				tag2:995019 image2:[NSImage imageNamed:@"NSGoRightTemplate"]//NSGoRightTemplate"]
				labelText:"TB_Nav"];
			}
			if( [itemIdentifier compare:@"TB_Keyword"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995020 
				image:[NSImage imageNamed:@"iconTbKey.Template"]
				labelText:"TB_Keyword" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_Commit"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995021 
				image:[NSImage imageNamed:@"iconTbDiscUp.Template"]
				labelText:"TB_Commit" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_LayoutLeft"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier 
				tag:995022 image:[NSImage imageNamed:@"iconTbSidebarLeft.Template"]
				labelText:"TB_LayoutLeft" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_LayoutRight"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier 
				tag:995025 image:[NSImage imageNamed:@"iconTbSidebarRight.Template"]
				labelText:"TB_LayoutRight" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_Split"] == NSOrderedSame )
			{
				return [self createToolbarItem_2Buttons:itemIdentifier 
				tag1:995023 image1:[NSImage imageNamed:@"iconTbSeparateHorizontal.Template"]
				tag2:995024 image2:[NSImage imageNamed:@"iconTbSeparateVertical.Template"]
				labelText:"TB_Split"];
			}
			if( [itemIdentifier compare:@"TB_SearchField"] == NSOrderedSame )
			{
				return [self createToolbarItem_SearchBox:itemIdentifier minwidth:50 maxwidth:200 tag:995101
				labelText:"TB_SearchField"];
			}
			if( [itemIdentifier compare:@"TB_KeyRecord"] == NSOrderedSame )
			{
				return [self createToolbarItem_4Buttons:itemIdentifier 
				tag1:995029 image1:[NSImage imageNamed:@"iconTbKeyRecord.Template"]
				tag2:995030 image2:[NSImage imageNamed:@"iconTbKeyStop.Template"]
				tag3:995031 image3:[NSImage imageNamed:@"iconTbKeyPlay.Template"]
				tag4:995028 image4:[NSImage imageNamed:@"iconTbCircleCursorButton.Template"]
				labelText:"TB_KeyRecord" toggleMode:YES];
			}
			/*
	if( [itemIdentifier compare:@"TB_ModePref"] == NSOrderedSame )
	{
		return [self createToolbarItem_Button:itemIdentifier tag:995011 
		image:[NSImage imageNamed:@"iconTbModePreference.Template"]
		labelText:"TB_ModePref" toggleMode:NO];
	}
	if( [itemIdentifier compare:@"TB_AppPref"] == NSOrderedSame )
	{
		return [self createToolbarItem_Button:itemIdentifier tag:995012 
		image:[NSImage imageNamed:@"iconTbPreference.Template"]
		labelText:"TB_AppPref" toggleMode:NO];
	}
	*/
			if( [itemIdentifier compare:@"TB_ProhibitPopup"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995032 
				image:[NSImage imageNamed:@"iconTbProhibitPopup.Template"]
				labelText:"TB_ProhibitPopup" toggleMode:YES];
			}
			if( [itemIdentifier compare:@"TB_ShowHideFloating"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995034 
				image:[NSImage imageNamed:@"iconTbProhibitPopup.Template"]
				labelText:"TB_ShowHideFloating" toggleMode:YES];
			}
			//#1389
			if( [itemIdentifier compare:@"TB_Save"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995035 
					image:[NSImage imageNamed:@"iconTbSave.Template"]
				labelText:"TB_Save" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_UndoRedo"] == NSOrderedSame )
			{
				return [self createToolbarItem_2Buttons:itemIdentifier 
					tag1:995036 image1:[NSImage imageNamed:@"iconTbUndo.Template"]
					tag2:995037 image2:[NSImage imageNamed:@"iconTbRedo.Template"]
				labelText:"TB_UndoRedo"];
			}
			if( [itemIdentifier compare:@"TB_FindDialog"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995038 
					image:[NSImage imageNamed:@"iconTbFind.Template"]
				labelText:"TB_FindDialog" toggleMode:NO];
			}
			//#1400
			if( [itemIdentifier compare:@"TB_Copy"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995039 
					image:[NSImage imageNamed:@"iconTbCopy.Template"]
				labelText:"TB_Copy" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_Cut"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995040 
					image:[NSImage imageNamed:@"iconTbCut.Template"]
				labelText:"TB_Cut" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_Paste"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995041 
					image:[NSImage imageNamed:@"iconTbPaste.Template"]
				labelText:"TB_Paste" toggleMode:NO];
			}
			break;
		}
	  case 1:
		{
			//
			if( [itemIdentifier compare:@"999000"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999000 
				image:[NSImage imageNamed:@"NSPreferencesGeneral"]
				labelText:"AppPref_General" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999001"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999001 
				image:[NSImage imageNamed:@"iconEnvSubwindow"]
				labelText:"AppPref_SubWindows" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999002"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999002 
				image:[NSImage imageNamed:@"iconEnvText"]
				labelText:"AppPref_Text" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999003"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999003 
				image:[NSImage imageNamed:@"iconEnvMagnifier"]
				labelText:"AppPref_Find" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999004"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999004 
				image:[NSImage imageNamed:@"iconEnvRemoteConnection"]
				labelText:"AppPref_RemoteFile" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999005"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999005 
				image:[NSImage imageNamed:@"iconEnvCompareDocument"]
				labelText:"AppPref_Compare" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999006"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999006 
				image:[NSImage imageNamed:@"iconModePrinter"]
				labelText:"AppPref_Print" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999007"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999007 
				image:[NSImage imageNamed:@"NSAdvanced"]
				labelText:"AppPref_Detail" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999008"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999008 
				image:[NSImage imageNamed:@"iconModeInputSupport"]
				labelText:"AppPref_Features" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999009"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999009 
					image:[NSImage imageNamed:@"iconModeView"]
				labelText:"AppPref_Display" toggleMode:NO];
			}
			//#1373
			if( [itemIdentifier compare:@"999010"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999010 
					image:[NSImage imageNamed:@"iconEnvOperation"]
				labelText:"AppPref_Operation" toggleMode:NO];
			}
			break;
		}
	  case 2:
		{
			//
			if( [itemIdentifier compare:@"999000"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999000 
				image:[NSImage imageNamed:@"iconModeMode"]
				labelText:"ModePref_General" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999011"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999011 
				image:[NSImage imageNamed:@"NSColorPanel"]
				labelText:"ModePref_Colors" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999002"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999002 
				image:[NSImage imageNamed:@"iconModeEdit"]
				labelText:"ModePref_Edit" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999001"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999001 
				image:[NSImage imageNamed:@"iconModeView"]
				labelText:"ModePref_Display" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999003"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999003 
				image:[NSImage imageNamed:@"iconModeKey"]
				labelText:"ModePref_Keyword" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999004"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999004 
				image:[NSImage imageNamed:@"iconModeHeading"]
				labelText:"ModePref_Header" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999005"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999005 
				image:[NSImage imageNamed:@"iconModeSyntax"]
				labelText:"ModePref_Syntax" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999007"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999007 
				image:[NSImage imageNamed:@"iconModeInputSupport"]
				labelText:"ModePref_Features" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999006"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999006 
				image:[NSImage imageNamed:@"iconModeIndent"]
				labelText:"ModePref_Indent" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999008"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999008 
				image:[NSImage imageNamed:@"iconModeKeybind"]
				labelText:"ModePref_KeyBind" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999009"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999009 
				image:[NSImage imageNamed:@"iconModeTool"]
				labelText:"ModePref_Tool" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999010"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999010 
				image:[NSImage imageNamed:@"NSAdvanced"]
				labelText:"ModePref_Advanced" toggleMode:NO];
			}
			break;
		}
	}
	return nil;
}

@end

