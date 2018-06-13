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

CocoaPanelWindow

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

@implementation CocoaPanelWindow

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

#pragma mark ---�V�[�g�I��������

/**
�V�[�g�I��������
*/
- (void)didEndSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	[self orderOut:nil];
}

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

@end

