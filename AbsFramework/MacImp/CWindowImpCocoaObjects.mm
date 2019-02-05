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

	CWindowImpCocoaObjects

*/

#include "CWindowImpCocoaObjects.h"

#pragma mark ===========================

#pragma mark ---�R���X�g���N�^

/**
�R���X�g���N�^
*/
CWindowImpCocoaObjects::CWindowImpCocoaObjects( CWindowImp& inWindowImp ) : mWindowImp(inWindowImp),
		mWindow(nil),mWindowDelegate(nil),mWindowController(nil),mParentWindow(nil),mModalSession(nil)
,mCreatingToolbar(false),mCASWindow(nil)
{
}

/**
�f�X�g���N�^
*/
CWindowImpCocoaObjects::~CWindowImpCocoaObjects()
{
	//�E�C���h�E�폜
	DeleteWindow();
}

#pragma mark ===========================

#pragma mark ---�E�C���h�E�����^�폜

/**
�E�C���h�E�����iInterfaceBuilder�Œ�`�ς݂̃E�C���h�E�j
*/
void	CWindowImpCocoaObjects::CreateWindow( const AText& inIBName )
{
	AStCreateNSStringFromAText	winNameRef(inIBName);
	//Window Controller��IB���琶��
	mWindowController = [[NSWindowController alloc] initWithWindowNibName:winNameRef.GetNSString()];
	//�E�C���h�E�ւ̃|�C���^�擾
	mWindow = [mWindowController window];
	//�|�C���^�ێ�����̂�retain�J�E���^�C���N�������g�B
	[mWindow retain];
	//�E�C���h�Ehide
	[mWindow orderOut:nil];
	//�E�C���h�E�^�C�v�ɂ���Ă��ꂼ�ꏈ��
	if( [mWindow isKindOfClass:[CocoaWindow class]] == YES )
	{
		//�ʏ�E�C���h�E
		CocoaWindow*	win = reinterpret_cast<CocoaWindow*>(mWindow);
		//�����ݒ�
		[win setup:(&mWindowImp)];
		//�E�C���h�Ehide�i�݌v��A�ŏ��̏�Ԃ�hide��ԂŌŒ肵�Ă����������߁j
		[win orderOut:nil];
		//LiveResize�̊��������m���邽�߂̃_�~�[view�𐶐��B�E�C���h�E�֓o�^�B
		//��addSubview�����r���[�̓E�C���h�E�폜����release����邩�ǂ����m�F�K�v
		[GetContentView() addSubview:[[[CocoaLiveResizeDetectionView alloc] initWithWindowImp:(&mWindowImp)] autorelease] 
		positioned:NSWindowAbove relativeTo:nil];
		//Delegate�ݒ�
		mWindowDelegate = [[CocoaWindowDelegate alloc] initWithWindowImp:(&mWindowImp)];
		[win setDelegate:mWindowDelegate];
		//�E�C���h�E���x���ݒ� #1034
		[win setLevel:NSNormalWindowLevel];
		//�E�C���h�Edeactivate���Ahide���Ȃ�
		[win setHidesOnDeactivate:NO];
		//�E�C���h�E��MainWindow, KeyWindow�ɂȂ��
		[win setCanBecomeMainWindow:YES];
		[win setCanBecomeKeyWindow:YES];
		//�}�E�X�C�x���g�������Ȃ�
		[win setIgnoresMouseEvents:NO];
		//IB�ɐݒ肳�ꂽProgressIndicator���擾���AmProgressIndicatorArray�֋L��
		NSProgressIndicator*	progress = [win getProgressIndicator];
		if( progress != nil )
		{
			mProgressIndicatorArray.Add(progress);
			[progress retain];
		}
		//#330
		//https://developer.apple.com/library/mac/#documentation/Cocoa/Conceptual/WinPanel/Articles/SettingWindowCollectionBehavior.html
		//IB�Œ�`���ꂽ�E�C���h�E��Spaces�̃A�N�e�B�uspace�ɕ\�������悤�ɂ���
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
		{
			[win setCollectionBehavior:NSWindowCollectionBehaviorMoveToActiveSpace];
		}
		//key view���[�v�������v�Z����悤�ɂ���
		[win setAutorecalculatesKeyViewLoop:YES];
	}
	else if( [mWindow isKindOfClass:[CocoaPanelWindow class]] == YES )
	{
		//Panel window�i�t���[�e�B���O�E�C���h�E�j
		CocoaPanelWindow*	win = reinterpret_cast<CocoaPanelWindow*>(mWindow);
		//�����ݒ�
		[win setup:(&mWindowImp)];
		//�E�C���h�Ehide�i�݌v��A�ŏ��̏�Ԃ�hide��ԂŌŒ肵�Ă����������߁j
		[win orderOut:nil];
		//LiveResize�̊��������m���邽�߂̃_�~�[view�𐶐��B�E�C���h�E�֓o�^�B
		//��addSubview�����r���[�̓E�C���h�E�폜����release����邩�ǂ����m�F�K�v
		[GetContentView() addSubview:[[[CocoaLiveResizeDetectionView alloc] initWithWindowImp:(&mWindowImp)] autorelease] 
		positioned:NSWindowAbove relativeTo:nil];
		//Delegate�ݒ�
		mWindowDelegate = [[CocoaWindowDelegate alloc] initWithWindowImp:(&mWindowImp)];
		[win setDelegate:mWindowDelegate];
		//�E�C���h�E���x���ݒ� #1034
		[win setLevel:NSFloatingWindowLevel];
		//�E�C���h�Edeactivate���Ahide���Ȃ��i���ʁAhide���Ȃ��悤�ɂ���B�N�����̃��[�_���v���O���X��hide����Ȃ��悤�ɂ��邽�߁B�j
		[win setHidesOnDeactivate:NO];//YES];
		//�E�C���h�E��MainWindow, KeyWindow�ɂȂ�Ȃ�
		[win setCanBecomeMainWindow:NO];
		[win setCanBecomeKeyWindow:YES];
		//�}�E�X�C�x���g�������Ȃ�
		[win setIgnoresMouseEvents:NO];
		//�t���[�e�B���O�p�l���ݒ�
		[win setFloatingPanel:YES];
		//IB�ɐݒ肳�ꂽProgressIndicator���擾���AmProgressIndicatorArray�֋L��
		NSProgressIndicator*	progress = [win getProgressIndicator];
		if( progress != nil )
		{
			mProgressIndicatorArray.Add(progress);
			[progress retain];
		}
		//#330
		//https://developer.apple.com/library/mac/#documentation/Cocoa/Conceptual/WinPanel/Articles/SettingWindowCollectionBehavior.html
		//IB�Œ�`���ꂽ�E�C���h�E��Spaces�̃A�N�e�B�uspace�ɕ\�������悤�ɂ���
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
		{
			[win setCollectionBehavior:NSWindowCollectionBehaviorMoveToActiveSpace];
		}
		//key view���[�v�������v�Z����悤�ɂ���
		[win setAutorecalculatesKeyViewLoop:YES];
	}
	else
	{
		_ACError("",NULL);
	}
	//AppleScript�p�I�u�W�F�N�g���� #1034
	mCASWindow = [[CASWindow alloc] init:mWindowImp.GetAWin().GetObjectID()];
}

/**
�E�C���h�E�����iInterfaceBuilder�Œ�`����Ă��Ȃ��E�C���h�E�j
*/
void	CWindowImpCocoaObjects::CreateWindow( const AWindowClass inWindowClass, NSWindow* inParentWindow,
		const AOverlayWindowLayer inOverlayWindowLayer,
		const ABool inHasCloseButton, const ABool inHasCollapseButton, 
		const ABool inHasZoomButton, const ABool inHasGrowBox,
		const ABool inIgnoreClick, const ABool inHasShadow, const ABool inHasTitlebar, 
		const ABool inNonFloating )//#1133
{
	//��inHasZoomButton���g�p�B�ݒ���@����
	//
	switch(inWindowClass)
	{
	  case kWindowClass_Document:
		{
			//Window Controller�͐������Ȃ�
			mWindowController = nil;
			//�E�C���h�E�̏���frame�ݒ�
			NSRect	rect = NSMakeRect(-10000,-10000,100,100);
			//�X�^�C���ݒ�
			//�g�ݍ��킹�ɂ͐���������B
			//��Fresize box��NSTitledWindowMask | NSUtilityWindowMask�ł����g���Ȃ��B
			//https://developer.apple.com/library/mac/#documentation/Cocoa/Reference/ApplicationKit/Classes/NSPanel_Class/Reference/Reference.html
			NSUInteger	stylemask = 0;//#1316 NSTexturedBackgroundWindowMask��obsolete�Ȃ̂ō폜�B NSTexturedBackgroundWindowMask;
			if( inHasTitlebar == true )
			{
				stylemask |= NSTitledWindowMask;
			}
			if( inHasCloseButton == true )
			{
				stylemask |= NSClosableWindowMask;
			}
			if( inHasCollapseButton == true )
			{
				stylemask |= NSMiniaturizableWindowMask;
			}
			//if( inHasGrowBox == true )
			{
				stylemask |= NSResizableWindowMask;
			}
			//�E�C���h�E����
			CocoaPanelWindow*	win = (CocoaPanelWindow*)[[CocoaWindow alloc] initWithContentRect:rect styleMask:stylemask 
								backing:NSBackingStoreBuffered //buffered�iMac OS X�̕W���j
								defer:NO];//defer:NO�ɂ���BYES�ɂ���ƁAmousemoved�����삵�Ȃ��B
			mWindow = win;
			//�����ݒ�
			[win setup:(&mWindowImp)];
			//�I�[�o�[���C�E�C���h�E���C���[�ݒ�
			[win setOverlayWindowLayer:inOverlayWindowLayer];
			//�E�C���h�Ehide
			[win orderOut:nil];
			//LiveResize�̊��������m���邽�߂̃_�~�[view�𐶐��B�E�C���h�E�֓o�^�B
			//��addSubview�����r���[�̓E�C���h�E�폜����release����邩�ǂ����m�F�K�v
			[GetContentView() addSubview:[[[CocoaLiveResizeDetectionView alloc] initWithWindowImp:(&mWindowImp)] autorelease] 
			positioned:NSWindowAbove relativeTo:nil];
			//Delegate�ݒ�
			mWindowDelegate = [[CocoaWindowDelegate alloc] initWithWindowImp:(&mWindowImp)];
			[win setDelegate:mWindowDelegate];
			//�E�C���h�E���x���ݒ� #1034
			[win setLevel:NSNormalWindowLevel];
			//�E�C���h�Edeactivate���Ahide���Ȃ�
			[win setHidesOnDeactivate:NO];
			//KeyView loop�����ݒ肵�Ȃ� #1091�΍�e�X�g
			[win setAutorecalculatesKeyViewLoop:NO];
			//�}�E�X�N���b�N��������
			if( inIgnoreClick == true )
			{
				//�E�C���h�E��MainWindow,KeyWindow�ɂȂ�Ȃ�
				[win setCanBecomeMainWindow:NO];
				[win setCanBecomeKeyWindow:NO];
				//�}�E�X�C�x���g��������
				[win setIgnoresMouseEvents:YES];
			}
			else
			{
				//�E�C���h�E��MainWindow�ɂȂ��AKeyWindow�ɂȂ��
				[win setCanBecomeMainWindow:YES];
				[win setCanBecomeKeyWindow:YES];
				//�}�E�X�C�x���g�������Ȃ�
				[win setIgnoresMouseEvents:NO];
			}
			//�V���h�E�\��
			if( inHasShadow == true )
			{
				[win setHasShadow:YES];
			}
			else
			{
				[win setHasShadow:NO];
			}
			//�w�i�F�ݒ�
			//[win setBackgroundColor:[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.0]];
			//�����E�C���h�E�ݒ�
			[win setOpaque:YES];
			//�ʏ�h�L�������g�E�C���h�E�̓t���X�N���[���Ή��ɂ���
			if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
			{
				[win setCollectionBehavior:(1 << 7)];
			}
			break;
		}
	  case kWindowClass_Overlay:
		{
			//Window Controller�͐������Ȃ�
			mWindowController = nil;
			//�E�C���h�E�̏���frame�ݒ�
			NSRect	rect = NSMakeRect(-10000,-10000,100,100);
			//�X�^�C���ݒ�
			//�g�ݍ��킹�ɂ͐���������B
			//��Fresize box��NSTitledWindowMask | NSUtilityWindowMask�ł����g���Ȃ��B
			//https://developer.apple.com/library/mac/#documentation/Cocoa/Reference/ApplicationKit/Classes/NSPanel_Class/Reference/Reference.html
			NSUInteger	stylemask = NSBorderlessWindowMask;
			if( inHasTitlebar == true )
			{
				stylemask |= NSTitledWindowMask | NSUtilityWindowMask;
			}
			if( inHasCloseButton == true )
			{
				stylemask |= NSClosableWindowMask;
			}
			if( inHasCollapseButton == true )
			{
				stylemask |= NSMiniaturizableWindowMask;
			}
			if( inHasGrowBox == true )
			{
				stylemask |= NSResizableWindowMask;
			}
			//�E�C���h�E����
			CocoaPanelWindow*	win = [[CocoaPanelWindow alloc] initWithContentRect:rect styleMask:stylemask 
								backing:NSBackingStoreBuffered //buffered�iMac OS X�̕W���j
								defer:NO];//defer:NO�ɂ���BYES�ɂ���ƁAmousemoved�����삵�Ȃ��B
			mWindow = win;
			//�����ݒ�
			[win setup:(&mWindowImp)];
			//�I�[�o�[���C�E�C���h�E���C���[�ݒ�
			[win setOverlayWindowLayer:inOverlayWindowLayer];
			//�E�C���h�Ehide
			[win orderOut:nil];
			//LiveResize�̊��������m���邽�߂̃_�~�[view�𐶐��B�E�C���h�E�֓o�^�B
			//��addSubview�����r���[�̓E�C���h�E�폜����release����邩�ǂ����m�F�K�v
			[GetContentView() addSubview:[[[CocoaLiveResizeDetectionView alloc] initWithWindowImp:(&mWindowImp)] autorelease] 
			positioned:NSWindowAbove relativeTo:nil];
			//�E�C���h�E���x���ݒ� #1034
			[win setLevel:NSNormalWindowLevel];
			//Delegate�ݒ�
			mWindowDelegate = [[CocoaWindowDelegate alloc] initWithWindowImp:(&mWindowImp)];
			[win setDelegate:mWindowDelegate];
			//�E�C���h�Edeactivate���Ahide���Ȃ�
			[win setHidesOnDeactivate:NO];
			//�}�E�X�N���b�N��������
			if( inIgnoreClick == true )
			{
				//�E�C���h�E��MainWindow,KeyWindow�ɂȂ�Ȃ�
				[win setCanBecomeMainWindow:NO];
				[win setCanBecomeKeyWindow:NO];
				//�}�E�X�C�x���g��������
				[win setIgnoresMouseEvents:YES];
			}
			else
			{
				//�E�C���h�E��MainWindow�ɂȂ�Ȃ��AKeyWindow�ɂȂ��
				[win setCanBecomeMainWindow:NO];
				[win setCanBecomeKeyWindow:YES];
				//�}�E�X�C�x���g�������Ȃ�
				[win setIgnoresMouseEvents:NO];
			}
			//�V���h�E�\��
			if( inHasShadow == true )
			{
				[win setHasShadow:YES];
			}
			else
			{
				[win setHasShadow:NO];
			}
			//�w�i�F�ݒ�
			[win setBackgroundColor:[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.0]];
			//�����E�C���h�E�ݒ�
			[win setOpaque:NO];//�����ݒ肵�Ȃ��Ɠ����E�������E�C���h�E�͎����ł��Ȃ�
			//�K�v�łȂ��ꍇ�i�{�^���N���b�N���j�ł�key window�ɂȂ�Ȃ��悤�ɂ���
			[win setBecomesKeyOnlyIfNeeded:YES];
			break;
		}
	  case kWindowClass_Floating:
		{
			//Window Controller�͐������Ȃ�
			mWindowController = nil;
			//�E�C���h�E�̏���frame�ݒ�
			NSRect	rect = NSMakeRect(-10000,-10000,100,100);
			//�X�^�C���ݒ�
			NSUInteger	stylemask = NSBorderlessWindowMask;
			if( inHasTitlebar == true )
			{
				stylemask |= NSTitledWindowMask | NSUtilityWindowMask;
			}
			if( inHasCloseButton == true )
			{
				stylemask |= NSClosableWindowMask;
			}
			if( inHasCollapseButton == true )
			{
				stylemask |= NSMiniaturizableWindowMask;
			}
			if( inHasGrowBox == true )
			{
				stylemask |= NSResizableWindowMask;
			}
			//�E�C���h�E����
			CocoaPanelWindow*	win = [[CocoaPanelWindow alloc] initWithContentRect:rect styleMask:stylemask 
								backing:NSBackingStoreBuffered //buffered�iMac OS X�̕W���j
								defer:NO];//defer:NO�ɂ���BYES�ɂ���ƁAmousemoved�����삵�Ȃ��B
			mWindow = win;
			//�����ݒ�
			[win setup:(&mWindowImp)];
			//�I�[�o�[���C�E�C���h�E���C���[�ݒ�
			[win setOverlayWindowLayer:inOverlayWindowLayer];
			//�E�C���h�Ehide
			[win orderOut:nil];
			//LiveResize�̊��������m���邽�߂̃_�~�[view�𐶐��B�E�C���h�E�֓o�^�B
			//��addSubview�����r���[�̓E�C���h�E�폜����release����邩�ǂ����m�F�K�v
			[GetContentView() addSubview:[[[CocoaLiveResizeDetectionView alloc] initWithWindowImp:(&mWindowImp)] autorelease] 
			positioned:NSWindowAbove relativeTo:nil];
			//�t���[�e�B���O�E�C���h�E�ݒ� #1133
			SetupFloatingWindow(inNonFloating,inIgnoreClick);
			//Delegate�ݒ�
			mWindowDelegate = [[CocoaWindowDelegate alloc] initWithWindowImp:(&mWindowImp)];
			[win setDelegate:mWindowDelegate];
			break;
		}
	}
	//�e�E�C���h�E���w�肳��Ă���ꍇ�͋L���i���ۂɐe�q�֌W��ݒ肷��̂�Show()/Hide()���j
	if( inParentWindow != nil )
	{
		//�e�E�C���h�E�ւ̃|�C���^���L��
		mParentWindow = inParentWindow;
		//�|�C���^���L������̂�retain
		[mParentWindow retain];
	}
	//AppleScript�p�I�u�W�F�N�g���� #1034
	mCASWindow = [[CASWindow alloc] init:mWindowImp.GetAWin().GetObjectID()];
}

//#1133
/**
�t���[�e�B���O�E�C���h�E�ݒ�
*/
void	CWindowImpCocoaObjects::SetupFloatingWindow( const ABool inNonFloating, const ABool inIgnoreClick )
{
	if( inNonFloating == false )
	{
		//-------------------- �ʏ�t���[�e�B���O --------------------
		
		//�E�C���h�E���x���ݒ� #1034
		[mWindow setLevel:NSFloatingWindowLevel];
		//�E�C���h�Edeactivate���Ahide����
		[mWindow setHidesOnDeactivate:YES];
		//�}�E�X�N���b�N��������
		if( inIgnoreClick == true )
		{
			//�E�C���h�E��MainWindow,KeyWindow�ɂȂ�Ȃ�
			[mWindow setCanBecomeMainWindow:NO];
			[mWindow setCanBecomeKeyWindow:NO];
			//�}�E�X�C�x���g��������
			[mWindow setIgnoresMouseEvents:YES];
		}
		else
		{
			//�E�C���h�E��MainWindow, KeyWindow�ɂȂ��
			[mWindow setCanBecomeMainWindow:NO];
			[mWindow setCanBecomeKeyWindow:YES];
			//�}�E�X�C�x���g�������Ȃ�
			[mWindow setIgnoresMouseEvents:NO];
		}
		//�t���[�e�B���O�p�l���ݒ�
		[mWindow setFloatingPanel:YES];
		//�V���h�E�\������
		[mWindow setHasShadow:YES];
		//�w�i�F�ݒ�
		[mWindow setBackgroundColor:[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.0]];
		//�����E�C���h�E�ݒ�
		[mWindow setOpaque:NO];//�����ݒ肵�Ȃ��Ɠ����E�������E�C���h�E�͎����ł��Ȃ�
		//�K�v�łȂ��ꍇ�i�{�^���N���b�N���j�ł�key window�ɂȂ�Ȃ��悤�ɂ���
		[mWindow setBecomesKeyOnlyIfNeeded:YES];
	}
	else
	{
		//-------------------- non-flating �t���[�e�B���O --------------------
		
		//�E�C���h�E���x���ݒ� #1034
		[mWindow setLevel:NSNormalWindowLevel];
		//�E�C���h�Edeactivate���Ahide����
		[mWindow setHidesOnDeactivate:NO];
		//�}�E�X�N���b�N��������
		if( inIgnoreClick == true )
		{
			//�E�C���h�E��MainWindow,KeyWindow�ɂȂ�Ȃ�
			[mWindow setCanBecomeMainWindow:NO];
			[mWindow setCanBecomeKeyWindow:NO];
			//�}�E�X�C�x���g��������
			[mWindow setIgnoresMouseEvents:YES];
		}
		else
		{
			//�E�C���h�E��MainWindow, KeyWindow�ɂȂ��
			[mWindow setCanBecomeMainWindow:YES];
			[mWindow setCanBecomeKeyWindow:YES];
			//�}�E�X�C�x���g�������Ȃ�
			[mWindow setIgnoresMouseEvents:NO];
		}
		//�t���[�e�B���O�p�l���ݒ�
		[mWindow setFloatingPanel:NO];
		//�V���h�E�\������
		[mWindow setHasShadow:YES];
		//�w�i�F�ݒ�
		[mWindow setBackgroundColor:[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.0]];
		//�����E�C���h�E�ݒ�
		[mWindow setOpaque:NO];//�����ݒ肵�Ȃ��Ɠ����E�������E�C���h�E�͎����ł��Ȃ�
		//�K�v�łȂ��ꍇ�i�{�^���N���b�N���j�ł�key window�ɂȂ�Ȃ��悤�ɂ���
		[mWindow setBecomesKeyOnlyIfNeeded:NO];
	}
}

/**
�L�����b�g�`��p�̃I�[�o�[���C�E�C���h�E����
*/
/*
void	CWindowImpCocoaObjects::CreateCaretOverlayWindow( const AIndex inCaretWindowIndex )
{
	//�E�C���h�E��frame�ݒ�i��j
	NSRect	rect = NSMakeRect(0,0,10,10);
	//�X�^�C���ݒ�
	NSUInteger	stylemask = NSBorderlessWindowMask;//Overlay�̏ꍇ�͏��NSBorderlessWindowMask�Œ�
	//�E�C���h�E���� CocoaCaretOverlayWindow��NSPanel�{ignoresMouseEvents��YES��Ԃ��悤�ɂ�������
	CocoaCaretOverlayWindow*	win = [[CocoaCaretOverlayWindow alloc] initWithContentRect:rect styleMask:stylemask 
								backing:NSBackingStoreBuffered //buffered�iMac OS X�̕W���j
								defer:NO];//defer:NO�ɂ���BYES�ɂ���ƁAmousemoved�����삵�Ȃ��B
	//
	mCaretOverlayWindowArray.Set(inCaretWindowIndex,win);
	//�E�C���h�Ehide
	[win orderOut:nil];
	//�E�C���h�Edeactivate���Ahide���Ȃ�
	[win setHidesOnDeactivate:NO];
	//�V���h�E�\�����Ȃ�
	[win setHasShadow:NO];
	//�w�i�F�ݒ�
	[win setBackgroundColor:[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.0]];
	//�����E�C���h�E�ݒ�
	[win setOpaque:NO];
	//���ݒ�
	[win setAlphaValue:0.7];
	
	//�E�C���h�Eshow
	[win orderFront:nil];
	//�e�E�C���h�E�ݒ�
	[mWindow addChildWindow:win ordered:NSWindowAbove];
	
	//view�����i�|�C���^�ێ��j
	CocoaCaretOverlayView*	view = [[CocoaCaretOverlayView alloc] initWithFrame:NSMakeRect(0,0,0,0)];
	//
	mCaretOverlayViewArray.Set(inCaretWindowIndex,view);
	//window��addSubview
	[[win contentView] addSubview:view positioned:NSWindowAbove relativeTo:nil];
}
*/
/**
�L�����b�g�`��p�̃I�[�o�[���C�E�C���h�E�擾�i�������Ȃ琶���j
*/
/*
CocoaCaretOverlayWindow*	CWindowImpCocoaObjects::GetCaretOverlayWindow( const AIndex inCaretWindowIndex, const ABool inCreateIfNotCreated )
{
	//
	while( mCaretOverlayWindowArray.GetItemCount()-1 < inCaretWindowIndex )
	{
		mCaretOverlayWindowArray.Add(nil);
		mCaretOverlayViewArray.Add(nil);
	}
	
	//�������Ȃ琶��
	if( mCaretOverlayWindowArray.Get(inCaretWindowIndex) == nil && inCreateIfNotCreated == true)
	{
		CreateCaretOverlayWindow(inCaretWindowIndex);
	}
	//�擾
	return mCaretOverlayWindowArray.Get(inCaretWindowIndex);
}
*/
/**
�E�C���h�E�폜
*/
void	CWindowImpCocoaObjects::DeleteWindow()
{
	//fprintf(stderr,"CWindowImpCocoaObjects::DeleteWindow() - start \n");
	//progress indicator�폜
	for( AIndex i = 0; i < mProgressIndicatorArray.GetItemCount(); i++ )
	{
		[mProgressIndicatorArray.Get(i) release];
	}
	mProgressIndicatorArray.DeleteAll();
	//user pane view���폜
	for( AIndex i = 0; i < mCocoaUserPaneViewArray_UserPaneView.GetItemCount(); i++ )
	{
		[mCocoaUserPaneViewArray_UserPaneView .Get(i) release];
	}
	mCocoaUserPaneViewArray_ControlID.DeleteAll();
	mCocoaUserPaneViewArray_UserPaneView.DeleteAll();
	//control id�L���b�V���폜
	for( AIndex i = 0; i < mControlIDCacheArray_NSView.GetItemCount(); i++ )
	{
		[mControlIDCacheArray_NSView.Get(i) release];
	}
	mControlIDCacheArray_ControlID.DeleteAll();
	mControlIDCacheArray_NSView.DeleteAll();
	
	/*
	//�L�����b�g�`��p�I�[�o�[���C�E�C���h�E�폜
	for( AIndex i = 0; i < mCaretOverlayWindowArray.GetItemCount(); i++ )
	{
		CocoaCaretOverlayWindow*	win = mCaretOverlayWindowArray.Get(i);
		if( win != nil )
		{
			//�L�����b�g�`��view��release�i�|�C���^�ێ����Ă���̂Łj
			[mCaretOverlayViewArray.Get(i) release];
			//�e����o�^���͂�����hide
			//�i�����œo�^���O���Ă����Ȃ��ƁA�e�E�C���h�E���V�[�g�E�C���h�E�������ꍇ�A
			//close���ɃV�[�g�E�C���h�E�̐e�E�C���h�E��hide�����Ȃǂ̉e�����o��B�j
			[mWindow removeChildWindow:win];
			[win orderOut:nil];
			//�N���[�Y
			[win close];
			//���
			[win release];
		}
	}
	mCaretOverlayWindowArray.DeleteAll();
	mCaretOverlayViewArray.DeleteAll();
	*/
	//�E�C���h�E�폜
	if( mWindow != nil )
	{
		//�N���[�Y
		[mWindow close];
		//���
		[mWindow release];
		mWindow = nil;
	}
	//�E�C���h�Edelegate�폜
	if( mWindowDelegate != nil )
	{
		//���
		[mWindowDelegate release];
		mWindowDelegate = nil;
	}
	//Window Controller�폜
	if( mWindowController != nil )
	{
		//���
		[mWindowController release];
		mWindowController = nil;
	}
	//�e�E�C���h�E�ւ̃|�C���^���
	if( mParentWindow != nil )
	{
		//���
		[mParentWindow release];
		mParentWindow = nil;
	}
	//AppleScript�p�I�u�W�F�N�g��� #1034
	if( mCASWindow != nil )
	{
		[mCASWindow release];
		mCASWindow = nil;
	}
	//fprintf(stderr,"CWindowImpCocoaObjects::DeleteWindow() - end \n");
}

/**
�e�E�C���h�E�ւ̃����N���L��
*/
void	CWindowImpCocoaObjects::SetParentWindow( NSWindow* inParentWindow )
{
	if( inParentWindow != nil )
	{
		//�e�E�C���h�E�ւ̃|�C���^���L��
		mParentWindow = inParentWindow;
		//�|�C���^���L������̂�retain
		[mParentWindow retain];
	}
}

/**
�e�E�C���h�E�ւ̃����N�����
*/
void	CWindowImpCocoaObjects::ClearParentWindow()
{
	if( mParentWindow != nil )
	{
		//���
		[mParentWindow release];
		mParentWindow = nil;
	}
}

/**
�c�[���o�[����
*/
void	CWindowImpCocoaObjects::CreateToolbar( /*#1012 const*/ NSString* inString, const ABool inForPref )
{
	//CreateToolbar()���s���t���OON�iEVT_ToolbarItemAdded()���s��}�����邽�߁B�j
	mCreatingToolbar = true;
	//
	NSToolbar*	toolbar = [[NSToolbar alloc] initWithIdentifier:inString];
	//
	if( inForPref == false )
	{
		[toolbar setAllowsUserCustomization:YES];
		[toolbar setAutosavesConfiguration:YES];
		[toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
		[toolbar setSizeMode:NSToolbarSizeModeRegular];
		[toolbar setDelegate:(id<NSToolbarDelegate>)GetWindow()];
		//
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
		{
			[toolbar setShowsBaselineSeparator:NO];
		}
	}
	else
	{
		[toolbar setAllowsUserCustomization:NO];
		[toolbar setAutosavesConfiguration:NO];
		[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
		[toolbar setSizeMode:NSToolbarSizeModeRegular];
		[toolbar setDelegate:(id<NSToolbarDelegate>)GetWindow()];
		//
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
		{
			[toolbar setShowsBaselineSeparator:YES];
		}
	}
	[GetWindow() setToolbar:toolbar];
	//CreateToolbar()���s���t���OOFF
	mCreatingToolbar = false;
}

#pragma mark ===========================

#pragma mark ---

/**
���̃E�C���h�E�Ń��[�_���Z�b�V�����J�n
*/
void	CWindowImpCocoaObjects::StartModalSession()
{
	//
	mModalSession = [NSApp beginModalSessionForWindow:mWindow];
}

/**
���̃E�C���h�E�ł̃��[�_���Z�b�V�����ł̌p���`�F�b�N
*/
ABool	CWindowImpCocoaObjects::CheckContinueingModalSession()
{
	//
	if( [NSApp runModalSession:mModalSession] != NSRunContinuesResponse )
	{
		return false;
	}
	else
	{
		return true;
	}
}

/**
���̃E�C���h�E�ł̃��[�_���Z�b�V�����I��
@note �Ȃ��A�E�C���h�E��close���Ȃ��ƁA���[�_���Z�b�V�����͂��ꂢ�ɏI���Ȃ����悤�iMac OS X 10.6�ł̓���B�ُ�ɂ������Ȃ�j�Bhide�ł͂��߁B
*/
void	CWindowImpCocoaObjects::EndModalSession()
{
	//
	[NSApp stopModal];
	[NSApp endModalSession:mModalSession];
	//
	mModalSession = nil;
}

/**
���[�_���E�C���h�E���[�v�J�n�i�E�C���h�E���̃C�x���g������StopModalSession���R�[�������܂ŏI�����Ȃ��j
*/
void	CWindowImpCocoaObjects::RunModalWindow()
{
	//��ʒ����ɕ\��
	[mWindow center];
	//show #1034
	[mWindow orderFront:nil];
	//���[�_���J�n�i�I������܂Ńu���b�N�j
	[NSApp runModalForWindow:mWindow];
}

/**
���̃E�C���h�E�ł̃��[�_���Z�b�V������~
*/
void	CWindowImpCocoaObjects::StopModalSession()
{
	//
	[NSApp stopModal];
}

#pragma mark ===========================

#pragma mark ---UserPaneView�o�^�^����

/**
�R���g���[���o�^
*/
void	CWindowImpCocoaObjects::RegisterUserPaneView( const AControlID inControlID, CocoaUserPaneView* inUserPane )
{
	//�R���g���[���o�^
	mCocoaUserPaneViewArray_ControlID.Add(inControlID);
	mCocoaUserPaneViewArray_UserPaneView.Add(inUserPane);
	//�|�C���^�ێ����Ă���̂�retain����
	[inUserPane retain];
}

/**
*/
void	CWindowImpCocoaObjects::UnregisterUserPaneView( const AControlID inControlID )
{
	//��userpane�łȂ��R���g���[���̍폜���{���ɕs�v���ǂ����m�F�K�v
	//window��release���ɁA�qview�ɂ��ׂ�release�����M�����Ȃ�A���Ȃ��B
	
	AIndex	index = mCocoaUserPaneViewArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		//RegisterControl()����retain���Ă���̂ŁA������release����
		[mCocoaUserPaneViewArray_UserPaneView.Get(index) release];
		//�R���g���[���o�^����
		mCocoaUserPaneViewArray_ControlID.Delete1(index);
		mCocoaUserPaneViewArray_UserPaneView.Delete1(index);
	}
}

#pragma mark ===========================

#pragma mark ---�R���g���[��ID(=tag)/id�ϊ�

/**
Tag(ControlID)����View���擾
*/
NSView*	CWindowImpCocoaObjects::FindViewByTag( const AControlID inControlID, const ABool inThrowIfNotFound ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( GetWindow() == nil ) {_ACThrow("Window not created",NULL);return NULL;}
	
	//UserPane�̏ꍇ�́AHash�z�񂩂猟��
	AIndex	userPaneIndex = mCocoaUserPaneViewArray_ControlID.Find(inControlID);
	if( userPaneIndex != kIndex_Invalid )
	{
		return mCocoaUserPaneViewArray_UserPaneView.Get(userPaneIndex);
	}
	
	//�L���b�V���ɂ���΁A�L���b�V������擾
	AIndex	index = mControlIDCacheArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		return mControlIDCacheArray_NSView.Get(index);
	}
	
	//tag(=ControlID)����view������
	NSView*	view = [GetWindow() findViewByControlID:inControlID];
	if( view == nil )
	{
		//�c�[���o�[���ڂ��猟��
		NSToolbarItem*	item = FindToolbarItemByTag(inControlID);
		if( item != nil )
		{
			return [item view];
		}
		if( inThrowIfNotFound == true )
		{
			_ACThrow("NSView is not found by tag.",NULL);
		}
		return nil;
	}
	//�L���b�V���ɒǉ�
	AddToControlIDCache(inControlID,view);
	//
	return view;
}

/**
Tag(ControlID)����UserPaneView���擾
*/
CocoaUserPaneView*	CWindowImpCocoaObjects::FindCocoaUserPaneViewByTag( const AControlID inControlID ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( GetWindow() == nil ) {_ACThrow("Window not created",NULL);return NULL;}
	
	//Hash�z�񂩂猟��
	AIndex	userPaneIndex = mCocoaUserPaneViewArray_ControlID.Find(inControlID);
	if( userPaneIndex != kIndex_Invalid )
	{
		return mCocoaUserPaneViewArray_UserPaneView.Get(userPaneIndex);
	}
	
	//������Ȃ�
	_ACError("CocoaUserPane not found",NULL);
	return nil;
}

/**
ProgressIndicator�擾
*/
NSProgressIndicator*	CWindowImpCocoaObjects::FindProgressIndicatorByIndex( const AIndex inProgressIndex ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( GetWindow() == nil ) {_ACThrow("Window not created",NULL);return NULL;}
	
	//index�ɑΉ�����ProgressIndicator��Ԃ�
	return mProgressIndicatorArray.Get(inProgressIndex);
}

/**
Tag(ControlID)����c�[���o�[���擾
*/
NSToolbarItem*	CWindowImpCocoaObjects::FindToolbarItemByTag( const AControlID inControlID ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( GetWindow() == nil ) {_ACThrow("Window not created",NULL);return NULL;}
	
	//�c�[���o�[�������Ƃ���nil��Ԃ�
	if( [GetWindow() toolbar] == nil )
	{
		return nil;
	}
	//�c�[���o�[���ږ��̃��[�v
	NSArray*	toolbarItems = [[GetWindow() toolbar] items];
	NSInteger	itemCount = [toolbarItems count];
	for( NSInteger index = 0; index < itemCount; index++ )
	{
		//�c�[���o�[���ڎ擾���Atag��r
		NSToolbarItem*	item = [toolbarItems objectAtIndex:index];
		if( inControlID == [item tag] )
		{
			return item;
		}
	}
	//�ǉ����̃c�[���o�[���ڂɈ�v����΂����Ԃ�
	NSToolbarItem*	addingToolbarItem = [GetWindow() getAddingToolbarItem];
	if( addingToolbarItem != nil )
	{
		if( [addingToolbarItem tag] == inControlID )
		{
			return addingToolbarItem;
		}
	}
	return nil;
}

/**
control ID�L���b�V���ɒǉ�
*/
void	CWindowImpCocoaObjects::AddToControlIDCache( const AControlID inControlID, NSView* inView ) const
{
	AIndex	index = mControlIDCacheArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )
	{
		//�L���b�V���֒ǉ�
		index = mControlIDCacheArray_ControlID.Add(inControlID);
		mControlIDCacheArray_NSView.Add(inView);
		//view retain
		[inView retain];
	}
}

/**
control ID�L���b�V������폜
*/
void	CWindowImpCocoaObjects::RemoveFromControlIDCache( const AControlID inControlID ) const
{
	AIndex	index = mControlIDCacheArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		//view release
		[mControlIDCacheArray_NSView.Get(index) release];
		//�L���b�V������폜
		mControlIDCacheArray_ControlID.Delete1(index);
		mControlIDCacheArray_NSView.Delete1(index);
	}
}


//#1034
/**
CUserPaneCocoaObjects
*/
CUserPaneCocoaObjects::CUserPaneCocoaObjects() : mNormalFont(NULL),mBoldFont(NULL),mItalicFont(NULL),mBoldItalicFont(NULL)
{
}

CUserPaneCocoaObjects::~CUserPaneCocoaObjects()
{
	//�t�H���g���
	if( mNormalFont != NULL )
	{
		[mNormalFont release];
	}
	if( mBoldFont != NULL )
	{
		[mBoldFont release];
	}
	if( mItalicFont != NULL )
	{
		[mItalicFont release];
	}
	if( mBoldItalicFont != NULL )
	{
		[mBoldItalicFont release];
	}
}



