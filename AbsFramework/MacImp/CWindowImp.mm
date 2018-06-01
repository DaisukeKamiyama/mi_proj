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

	CWindowImp.cp

*/

#include "CWindowImp.h"
//#1012 #include "StSetPort.h"
#include "CAppImp.h"
#include "../Frame.h"
#include "CPrintImp.h"

//#include <QuickTime/QuickTime.h>

#import <Cocoa/Cocoa.h>
#import "CocoaUserPaneView.h"
#import "CocoaWindow.h"
#import "CocoaProgressIndicator.h"
#include "../AbsBase/Cocoa.h"
#include "CWindowImpCocoaObjects.h"
#import "CocoaWebView.h"
#import <WebKit/WebKit.h>
#import "CocoaFontField.h"
#import "CocoaCollapsableView.h"

#pragma mark ===========================

#pragma mark --- NSPoint/NSRect�ϊ�
//#688

/**
ALocalPoint->NSPoint�ϊ�
*/
NSPoint	ConvertFromLocalPointToNSPoint( NSView* inView, const ALocalPoint inLocalPoint ) 
{
	//NSPoint�֕ϊ�
	NSPoint	viewpoint = {inLocalPoint.x,inLocalPoint.y};
	//flipped==NO�̏ꍇ�ANSPoint��y���W�͉��ӂ���̋���
	if( [inView isFlipped] == NO )
	{
		viewpoint.y = [inView frame].size.height - inLocalPoint.y;
	}
	return viewpoint;
}

/**
NSPoint->ALocalPoint�ϊ�
*/
ALocalPoint	ConvertFromNSPointToLocalPoint( NSView* inView, const NSPoint inViewPoint ) 
{
	//ALocalPoint�֕ϊ�
	ALocalPoint	lpt = {inViewPoint.x,inViewPoint.y};
	//flipped==NO�̏ꍇ�ANSPoint��y���W�͉��ӂ���̋���
	if( [inView isFlipped] == NO )
	{
		lpt.y = [inView frame].size.height - inViewPoint.y;
	}
	return lpt;
}

/**
AWindowPoint->NSPoint�ϊ�
*/
NSPoint	ConvertFromWindowPointToNSPoint( NSView* inContentView, const AWindowPoint inWindowPoint ) 
{
	NSPoint	windowpoint = {0};
	//NSPoint�֕ϊ�
	//Window���̂�frame�̓^�C�g���o�[���܂�ł���̂ŁAContent view�ŕϊ�����B
	//Content view��flipped==NO�Ȃ̂ŁANSPoint��y���W�͉��ӂ���̋���
	windowpoint.x = inWindowPoint.x;
	windowpoint.y = [inContentView frame].size.height - inWindowPoint.y;
	return windowpoint;
}

/**
NSPoint->AWindowPoint�ϊ�
*/
AWindowPoint	ConvertFromNSPointToWindowPoint( NSView* inContentView, const NSPoint inContentViewPoint ) 
{
	AWindowPoint	wpt = {0};
	//AWindowPoint�֕ϊ�
	//Window���̂�frame�̓^�C�g���o�[���܂�ł���̂ŁAContent view�ŕϊ�����B
	//Content view��flipped==NO�Ȃ̂ŁANSPoint��y���W�͉��ӂ���̋���
	wpt.x = inContentViewPoint.x;
	wpt.y = [inContentView frame].size.height - inContentViewPoint.y;
	return wpt;
}

/**
AWindowRect-.NSRect�ϊ�
*/
NSRect	ConvertFromWindowRectToNSRect( NSView* inContentView, const AWindowRect inWindowRect )
{
	NSRect	windowrect = {0};
	//AWindowRect�֕ϊ�
	//Window���̂�frame�̓^�C�g���o�[���܂�ł���̂ŁAContent view�ŕϊ�����B
	//Content view��flipped==NO�Ȃ̂ŁANSRect��y���W�͉��ӂ���̋���
	windowrect.origin.x = inWindowRect.left;
	windowrect.origin.y = [inContentView frame].size.height - inWindowRect.bottom;
	windowrect.size.width = inWindowRect.right - inWindowRect.left;
	windowrect.size.height = inWindowRect.bottom - inWindowRect.top;
	return windowrect;
}


#pragma mark ===========================
#pragma mark [�N���X]CWindowImp
#pragma mark ===========================

//OS����̃C�x���g�n���h���R�[���o�b�N���A�ΏۃI�u�W�F�N�g���|�C���^���猩����K�v������B
//���̂Ƃ��́A�|�C���^��CWindowImp*�ł��邪�A���̃|�C���^�̐������`�F�b�N���邽�߂ɁAsAliveWindowArray�ɁA�������̃I�u�W�F�N�g�ւ̃|�C���^���i�[����B
//AWindow�́A�����E�C���h�E��h�L�������g�t���̃E�C���h�E���A�ꌳ�Ǘ�����Ȃ��̂ŁA
//�|�C���^�ł͂Ȃ�ObjectID�����C�x���g�n���h���ɐݒ肷��Ƃ������@�ł́A�킴�킴���̂��߂����̑Ή��H�����傫������̂ŁA���̂悤�ɂ����B
//�i�܂��AAPI�̈�����void*�Ȃ̂ŁA�|�C���^�ȊO�̂��̂�n���̂́A�����I�Ƀ��X�N������B�j
AArray<CWindowImp*>	CWindowImp::sAliveWindowArray;

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^

/**
�R���X�g���N�^
@param inParent �e�I�u�W�F�N�g(AApplication�I�u�W�F�N�g)
@param inAWin AWindow�I�u�W�F�N�g
*/
CWindowImp::CWindowImp( AObjectArrayItem* inParent, AWindow& inAWin ) 
	: AObjectArrayItem(inParent), mAWin(inAWin), mModified(false), mWindowStyle(kWindowStyle_Normal), mWindowRef(NULL), mParentWindowRef(NULL),
		mDefaultOKButton(kControlID_Invalid), mDefaultCancelButton(kControlID_Invalid),
		/*#688 �t�H�[�J�X��OS�ŊǗ� mCurrentFocus(kControlID_Invalid), mLatentFocus(kControlID_Invalid), */
		/*#1034 mNavSheet(NULL),*/ mInputBackslashByYenKeyMode(false),
		mHasSheet(false), mParentWindow(NULL)
		,mDragTrackingControl(kControlID_Invalid)//#236
		//#688 �E�C���h�E�O���[�v�͎g�p����Cocoa��addChildWindow�őΉ� ,mOverlayWindowsGroupRef(NULL) ,mIsOverlayWindow(false)//#291
		,mWindowClass(kWindowClass_Document)//win
		,mFullScreenMode(false)//#404
		//#688 ,mOverlayParentWindowRef(NULL)//#669
		//#688 ,mTSMID(NULL)//#688
		,CocoaObjects(NULL)//#688
		,mMouseMovedTrackingControlID(kControlID_Invalid)//#688
		,mCurrentWindowBounds(kGlobalRect_0000)
,mMinWidth(0),mMinHeight(0),mMaxWidth(100000),mMaxHeight(100000)//#688
,mDoingResizeCompleted(false)
,mFontPanelVirtualControlID(kControlID_Invalid)//#688
,mCloseByEscapeKey(true)//#986
,mIgnoreClick(false)//#1133
,mNonFloatingFloating(false)//#1133
{
	//#688 Cocoa�I�u�W�F�N�g����
	CocoaObjects = new CWindowImpCocoaObjects(*this);
	
	//�����E�C���h�E���X�g�֒ǉ�
	sAliveWindowArray.Add(this);
	
	/*#688
	//TSM
	InterfaceTypeList	tsmtype;
	tsmtype[0] = 'udoc';
	OSErr	err = ::NewTSMDocument(1,tsmtype,&mTSMID,(long)this);
	if( err != noErr )   _ACErrorNum("NewTSMDocument() returned error: ",err,this);
	*/
}

/**
�f�X�g���N�^
*/
CWindowImp::~CWindowImp()
{
	//#688 Cocoa�I�u�W�F�N�g�폜
	delete CocoaObjects;
	CocoaObjects = NULL;
	
	//�����E�C���h�E���X�g����폜
	AIndex	index = sAliveWindowArray.Find(this);
	if( index != kIndex_Invalid )
	{
		sAliveWindowArray.Delete1(index);
	}
	/*#688
	//TSM
	OSErr	err = ::DeleteTSMDocument(mTSMID);
	if( err != noErr )   _ACErrorNum("DeleteTSMDocument() returned error: ",err,this);
	*/
}

//#1034
/**
CocoaWindow�I�u�W�F�N�g�擾
*/
CocoaWindow*	CWindowImp::GetCocoaWindow()
{
	return CocoaObjects->GetWindow();
}

#pragma mark ===========================

#pragma mark --- CWindowImp�I�u�W�F�N�g����

//B0000 080810
/**
WindowRef����CWindowImp�I�u�W�F�N�g������
*/
CWindowImp& CWindowImp::GetWindowImpByWindowRef( const AWindowRef inWindowRef )
{
	for( AIndex i = 0; i < sAliveWindowArray.GetItemCount(); i++ )
	{
		CWindowImp*	win = sAliveWindowArray.Get(i);
		if( win->IsWindowCreated() == true )
		{
			if( win->GetWindowRef() == inWindowRef )
			{
				return *win;
			}
		}
	}
	_ACThrow("",NULL);
	return *(sAliveWindowArray.Get(0));
}

//B0000 080810
/**
WindowRef�ɑΉ�����CWindowImp�I�u�W�F�N�g�����݂��邩�ǂ�����Ԃ�
*/
ABool CWindowImp::ExistWindowImpForWindowRef( const AWindowRef inWindowRef )
{
	for( AIndex i = 0; i < sAliveWindowArray.GetItemCount(); i++ )
	{
		CWindowImp*	win = sAliveWindowArray.Get(i);
		if( win->IsWindowCreated() == true )
		{
			if( win->GetWindowRef() == inWindowRef )
			{
				return true;
			}
		}
	}
	return false;
}

//#688
/**
global point����t���[�e�B���O�E�C���h�E������
*/
CWindowImp*	CWindowImp::FindFloatingWindowByGlobalPoint( const AGlobalPoint& inPoint )
{
	//�e�E�C���h�E���Ƃ̃��[�v
	for( AIndex i = 0; i < sAliveWindowArray.GetItemCount(); i++ )
	{
		CWindowImp*	win = sAliveWindowArray.Get(i);
		if( win->IsWindowCreated() == true )
		{
			if( win->mWindowClass == kWindowClass_Floating && win->IsVisible() == true )
			{
				//�\�����̃t���[�e�B���O�E�C���h�E�̏ꍇ�́A����bounds���Ȃ�CWindowImp*��Ԃ�
				ARect	bounds = {0};
				win->GetWindowBounds(bounds);
				if( inPoint.x >= bounds.left && inPoint.x <= bounds.right &&
							inPoint.y >= bounds.top && inPoint.y <= bounds.bottom )
				{
					return win;
				}
			}
		}
	}
	return NULL;
}

#pragma mark ===========================

#pragma mark --- �E�C���h�E����

/**
�E�C���h�E�����i���\�[�X�����݂���ꍇ�p�j

inTabWindow, inTabCount��Mac�ł͎g�p���Ȃ��i���\�[�X���Őݒ�j
*/
void	CWindowImp::Create( AConstCharPtr inWindowName, const AWindowClass inWindowClass, //win
		const ABool inTabWindow, const AItemCount inTabCount )//win
{
	if( mWindowRef != NULL )
	{
		_ACError("Window is already created",this);
		return;
	}
	
	//win
	mWindowClass = inWindowClass;
	
	//������"main/TextWindow"�Ƃ����`���Ȃ̂ŁA�����Nib�t�@�C������Window���ɕ������āACFStringRef�𐶐�����
	AText	nibWinName(inWindowName);
	ATextArray	nibWinNameArray;
	nibWinName.Explode('/',nibWinNameArray);
	if( nibWinNameArray.GetItemCount() != 2 )
	{
		_ACThrow("wrong nib window name",this);
	}
	AText	nibName, winName;
	nibWinNameArray.Get(0,nibName);
	nibWinNameArray.Get(1,winName);
	
	if( true )
	{
		//Cocoa �E�C���h�E����
		CocoaObjects->CreateWindow(winName);
		mWindowRef = CocoaObjects->GetWindowRef();
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//TSM�h�L�������g���� #688 �R���X�g���N�^����ړ�
		InterfaceTypeList	tsmtype;
		tsmtype[0] = 'udoc';
		OSErr	err = ::NewTSMDocument(1,tsmtype,&mTSMID,(long)this);
		if( err != noErr )   _ACErrorNum("NewTSMDocument() returned error: ",err,this);
		
		AStCreateCFStringFromAText	nibNameRef(nibName);
		AStCreateCFStringFromAText	winNameRef(winName);
		
		//�E�C���h�E����
		if( nibName.Compare("main") == true )
		{
			err = ::CreateWindowFromNib(CAppImp::GetNibRef(),winNameRef.GetRef(),&mWindowRef);
			if( err != noErr )   _ACErrorNum("CreateWindowFromNib() returned error: ",err,this);
		}
		else
		{
			IBNibRef	nibRef;
			err = ::CreateNibReference(nibNameRef.GetRef(),&nibRef);
			if( err != noErr )   _ACErrorNum("CreateNibReference() returned error: ",err,this);
			err = ::CreateWindowFromNib(nibRef,winNameRef.GetRef(),&mWindowRef);
			if( err != noErr )   _ACErrorNum("CreateWindowFromNib() returned error: ",err,this);
			::DisposeNibReference(nibRef);
		}
		//#674 �h�L�������g�E�C���h�E�ɂ͑S��window group����邱�Ƃɂ���B
		//�iwindow group�������E�C���h�E��SendBehind����ƁAwindow group�����݂���S�ẴE�C���h�E�̌��Ɉړ����Ă��܂����̑΍�̂��߁j
		RegisterOverlayWindow(NULL);
		
		//B0385
		HISize	min;
		min.width = 50;
		min.height = 50;
		err = ::SetWindowResizeLimits(mWindowRef,&min,NULL);
		if( err != noErr )   _ACErrorNum("SetWindowResizeLimits() returned error: ",err,this);
		
		//TabView�����݂���ꍇ��1�Ԗڂ�Tab�ɐݒ肷��
		AControlRef	tabViewRootControlRef = GetControlRef('ttab',0);
		if( tabViewRootControlRef != NULL )
		{
			::SetControl32BitValue(tabViewRootControlRef,1);
			UpdateTab();
		}
		
	}
#endif
	//�E�C���h�E�ŏ��T�C�Y�ݒ�i�E�C���h�E�����O�ɍŏ��T�C�Y�ݒ肳�ꂽ�ꍇ�́A�����Ŏ��E�C���h�E�ɐݒ肳���j #688
	SetWindowMinimumSize(mMinWidth,mMinHeight);
	SetWindowMaximumSize(mMaxWidth,mMaxHeight);
	//#404 �ʏ��ԃE�C���h�E�T�C�Y�L��
	GetWindowBounds(mWindowBoundsInStandardState);
	//#404 �t���X�N���[����ԉ���
	mFullScreenMode = false;
}

/**
�E�C���h�E�����i���\�[�X�����݂��Ȃ��ꍇ�p�j
*/
void	CWindowImp::Create( const AWindowClass inWindowClass, 
		const ABool inHasCloseButton, const ABool inHasCollapseButton, 
		const ABool inHasZoomButton, const ABool inHasGrowBox,
		const ABool inIgnoreClick,//#379
		const ABool inHasShadow, const ABool inHasTitlebar,//#688
		const AWindowRef inParentWindowRef,
		const AOverlayWindowLayer inOverlayWindowLayer )//#291 #688
{
	if( mWindowRef != NULL )
	{
		_ACError("Window is already created",this);
		return;
	}
	
	//win
	mWindowClass = inWindowClass;
	
	//#1133
	//�E�C���h�E�̃v���p�e�B���L��
	mIgnoreClick = inIgnoreClick;
	
	if( true )
	{
		switch(mWindowClass)
		{
		  case kWindowClass_Document:
			{
				//�E�C���h�E����
				CocoaObjects->CreateWindow(mWindowClass,nil,inOverlayWindowLayer,
										   inHasCloseButton,inHasCollapseButton,inHasZoomButton,inHasGrowBox,mIgnoreClick,inHasShadow,inHasTitlebar,mNonFloatingFloating);//#1133
				//WindowRef�ݒ�
				mWindowRef = CocoaObjects->GetWindowRef();
				break;
			}
		  case kWindowClass_Overlay:
			{
				//�e�E�C���h�E��CWindowImp�擾
				CWindowImp&	parent = CWindowImp::GetWindowImpByWindowRef(inParentWindowRef);
				//�E�C���h�E�����i�e�E�C���h�E�w��j
				CocoaObjects->CreateWindow(mWindowClass,parent.CocoaObjects->GetWindow(),inOverlayWindowLayer,
										   inHasCloseButton,inHasCollapseButton,inHasZoomButton,inHasGrowBox,mIgnoreClick,inHasShadow,inHasTitlebar,mNonFloatingFloating);//#1133
				//WindowRef�ݒ�
				mWindowRef = CocoaObjects->GetWindowRef();
				break;
			}
		  case kWindowClass_Floating:
			{
				//�E�C���h�E����
				CocoaObjects->CreateWindow(mWindowClass,nil,inOverlayWindowLayer,
										   inHasCloseButton,inHasCollapseButton,inHasZoomButton,inHasGrowBox,mIgnoreClick,inHasShadow,inHasTitlebar,mNonFloatingFloating);//#1133
				mWindowRef = CocoaObjects->GetWindowRef();
				break;
			}
		}
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//�E�C���h�E����
		Rect	bounds;
		bounds.left		= 0;
		bounds.top		= 0;
		bounds.right	= 0;
		bounds.bottom	= 0;
		switch(inWindowClass)//#138
		{
		  case kWindowClass_Document:
			{
				//TSM�h�L�������g���� #688 �R���X�g���N�^����ړ�
				InterfaceTypeList	tsmtype;
				tsmtype[0] = 'udoc';
				OSErr	err = ::NewTSMDocument(1,tsmtype,&mTSMID,(long)this);
				if( err != noErr )   _ACErrorNum("NewTSMDocument() returned error: ",err,this);
				
				//
				err = ::CreateNewWindow(kDocumentWindowClass,
							kWindowCloseBoxAttribute|kWindowFullZoomAttribute|kWindowCollapseBoxAttribute|kWindowResizableAttribute|kWindowLiveResizeAttribute/*B0407*/|
							/* kWindowToolbarButtonAttribute|*/kWindowCompositingAttribute|kWindowStandardHandlerAttribute,&bounds,&mWindowRef);
				if( err != noErr )   _ACErrorNum("CreateNewWindow() returned error: ",err,this);
				//#674 �h�L�������g�E�C���h�E�ɂ͑S��window group����邱�Ƃɂ���B
				//�iwindow group�������E�C���h�E��SendBehind����ƁAwindow group�����݂���S�ẴE�C���h�E�̌��Ɉړ����Ă��܂����̑΍�̂��߁j
				RegisterOverlayWindow(NULL);
				break;
			}
			//
		  case kWindowClass_Toolbar:
			{
				err = ::CreateNewWindow(kToolbarWindowClass,
							kWindowCompositingAttribute|kWindowStandardHandlerAttribute,&bounds,&mWindowRef);
				if( err != noErr )   _ACErrorNum("CreateNewWindow() returned error: ",err,this);
				break;
			}
		  case kWindowClass_Floating:
			{
				//�����ݒ�
				WindowAttributes	attr = kWindowCompositingAttribute|kWindowStandardHandlerAttribute;
				if( inHasCloseButton == true )
				{
					attr |= kWindowCloseBoxAttribute;
				}
				if( inHasCollapseButton == true )
				{
					attr |= kWindowCollapseBoxAttribute;
				}
				if( inHasZoomButton == true )
				{
					attr |= kWindowFullZoomAttribute;
				}
				if( inHasGrowBox == true )
				{
					attr |= kWindowResizableAttribute;
				}
				//�E�C���h�E����
				err = ::CreateNewWindow(kFloatingWindowClass,attr,&bounds,&mWindowRef);
				if( err != noErr )   _ACErrorNum("CreateNewWindow() returned error: ",err,this);
				break;
			}
		  case kWindowClass_Overlay:
			{
				//�����ݒ�
				WindowAttributes	attr = kWindowCompositingAttribute|kWindowStandardHandlerAttribute;
				//#379
				if( inIgnoreClick == true )
				{
					attr |= kWindowIgnoreClicksAttribute;
				}
				//�E�C���h�E����
				err = ::CreateNewWindow(kOverlayWindowClass,attr,&bounds,&mWindowRef);
				if( err != noErr )   _ACErrorNum("CreateNewWindow() returned error: ",err,this);
				/*�f�t�H���gkWindowActivationScopeNone�BOverlay��activation��WindowGroup�ɂ�萧�䂷��
				err = ::SetWindowActivationScope(mWindowRef,kWindowActivationScopeNone);//
				if( err != noErr )   _ACErrorNum("SetWindowActivationScope() returned error: ",err,NULL);
				*/
				mIsOverlayWindow = true;
				
				//�e�E�C���h�E�ɓo�^ #291
				if( inParentWindowRef != NULL )
				{
					mOverlayWindowsGroupRef = //#338 �q�E�C���h�E�ł�WindowGroupRef��ێ�����悤�ɂ���i��ɐe�폜�ł���悤�ɂ���j
							CWindowImp::GetWindowImpByWindowRef(inParentWindowRef).RegisterOverlayWindow(mWindowRef);
					::RetainWindowGroup(mOverlayWindowsGroupRef);//#338 �q�E�C���h�E�ł�WindowGroupRef��ێ�����悤�ɂ���
					mOverlayParentWindowRef = inParentWindowRef;//#669 �e�E�C���h�E�L�� �}�E�X�_�E������Select()�Ɏg�p����
				}
				else
				{
					_ACError("",this);
				}
				break;
			}
		}
		
		//TabView�����݂���ꍇ��1�Ԗڂ�Tab�ɐݒ肷��
		AControlRef	tabViewRootControlRef = GetControlRef('ttab',0);
		if( tabViewRootControlRef != NULL )
		{
			::SetControl32BitValue(tabViewRootControlRef,1);
			UpdateTab();
		}
	}
#endif
	
	//�E�C���h�E�ŏ��T�C�Y�ݒ�i�E�C���h�E�����O�ɍŏ��T�C�Y�ݒ肳�ꂽ�ꍇ�́A�����Ŏ��E�C���h�E�ɐݒ肳���j #688
	SetWindowMinimumSize(mMinWidth,mMinHeight);
	SetWindowMaximumSize(mMaxWidth,mMaxHeight);
	//#404 �ʏ��ԃE�C���h�E�T�C�Y�L��
	GetWindowBounds(mWindowBoundsInStandardState);
	//#404 �t���X�N���[����ԉ���
	mFullScreenMode = false;
}

/*
HIWindowChangeClass()�ɂ��Overlay�N���X�ւ̕ύX�͂ł��Ȃ��H
void	CWindowImp::ChangeWindowClass( const AWindowClass inWindowClass, 
		const ABool inHasCloseButton, const ABool inHasCollapseButton, const ABool inHasZoomButton, const ABool inHasGrowBox )
{
	OSStatus	err = noErr;
	
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	err = ::HIWindowChangeClass(mWindowRef,inWindowClass);
	if( err != noErr )   _ACErrorNum("HIWindowChangeClass() returned error: ",err,NULL);
	
	int	setAttr[16];
	int	clearAttr[16];
	int	i = 0;
	int j = 0;
	setAttr[i] = kWindowCompositingAttribute;
	i++;
	setAttr[i] = kWindowStandardHandlerAttribute;
	i++;
	if( inHasCloseButton == true )
	{
		setAttr[i] = kWindowCloseBoxAttribute;
		i++;
	}
	else
	{
		clearAttr[j] = kWindowCloseBoxAttribute;
		j++;
	}
	if( inHasCollapseButton == true )
	{
		setAttr[i] = kWindowCollapseBoxAttribute;
		i++;
	}
	else
	{
		clearAttr[j] = kWindowCollapseBoxAttribute;
		j++;
	}
	kWindowFullZoomAttribute;
	kWindowResizableAttribute;
	err = ::HIWindowChangeAttributes(mWindowRef,setAttr,clearAttr);
	if( err != noErr )   _ACErrorNum("HIWindowChangeAttributes() returned error: ",err,NULL);
}
*/

/**
�E�C���h�E�폜
*/
void	CWindowImp::CloseWindow()
{
	//fprintf(stderr,"CWindowImp::CloseWindow() - start\n");
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//�V�[�g�E�C���h�E�̏ꍇ�A�e�E�C���h�E��mHasSheet�t���O��OFF�ɂ���
	if( kWindowStyle_Sheet == mWindowStyle && mParentWindow != NULL )
	{
		if( sAliveWindowArray.Find(mParentWindow) != kIndex_Invalid )
		{
			mParentWindow->SetHasSheet(false);
		}
	}
	
	/*#291
	//�q�E�C���h�E�N���[�Y
	if( mChildSheetWindow.GetItemCount() > 0 )
	{
		CloseChildWindow();
	}
	*/
	
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�E�C���h�E�폜
		CocoaObjects->DeleteWindow();
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�C�x���g�n���h���A���C���X�g�[��
		UninstallEventHandler();
		
		//�E�C���h�E�폜
		::DisposeWindow(mWindowRef);
		
		//#688
		if( mTSMID != NULL )
		{
			//TSM
			OSErr	err = ::DeleteTSMDocument(mTSMID);
			if( err != noErr )   _ACErrorNum("DeleteTSMDocument() returned error: ",err,this);
			mTSMID = NULL;
		}
	}
#endif
	
	//WindowRef��NULL�ɐݒ�
	mWindowRef = NULL;
	
	//TextArray���j���[�̓o�^����
	while( mTextArrayMenuArray_ControlID.GetItemCount() > 0 )
	{
		UnregisterTextArrayMenu(mTextArrayMenuArray_ControlID.Get(0));
	}
	
	//�L�[�{�[�h�t�H�[�J�X�O���[�v�폜
	//#135 mFocusGroup.DeleteAll();
	
	/*#688
	//�t�H�[�J�X�ݒ����
	mLatentFocus = kControlID_Invalid;
	mCurrentFocus = kControlID_Invalid;
	*/
	//���W�I�O���[�v�폜
	//#349 mRadioGroup.DeleteAll();
	
	//UserPane�폜
	mUserPaneArray.DeleteAll();
	mUserPaneArray_ControlID.DeleteAll();
	mUserPaneArray_Type.DeleteAll();
	//#688 mUserPaneArray_ControlRef.DeleteAll();//#688
	
#if SUPPORT_CARBON
	//WindowGroup�폜 #291
	if( mOverlayWindowsGroupRef != NULL )
	{
		//::DebugPrintWindowGroup(mOverlayWindowsGroupRef);
		::ReleaseWindowGroup(mOverlayWindowsGroupRef);//Group���̂��ꂼ��̃E�C���h�E��retain���Ă���B�S�Ă�release���������B
		mOverlayWindowsGroupRef = NULL;
	}
	//#669 Overlay�e�E�C���h�E�̋L������
	if( mOverlayParentWindowRef != NULL )
	{
		mOverlayParentWindowRef = NULL;
	}
#endif
	
	//#404
	mFullScreenMode = false;
	//fprintf(stderr,"CWindowImp::CloseWindow() - end\n");
}

#if SUPPORT_CARBON
/**
Window��Drawer�ɐݒ�
@param inParent �e�E�C���h�E
*/
void	CWindowImp::SetWindowStyleToDrawer( CWindowImp& inParent )
{
	OSStatus	err = noErr;
	
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//�E�C���h�E�X�^�C���Ɛe�E�C���h�ERef��ݒ肵��Show()
	mWindowStyle = kWindowStyle_Drawer;
	mParentWindowRef = inParent.GetWindowRef();
	mParentWindow = &inParent;//B0367
	err = ::SetDrawerParent(mWindowRef,mParentWindowRef);
	if( err != noErr )   _ACErrorNum("SetDrawerParent() returned error: ",err,this);
}
#endif

/**
Window��Sheet�ɐݒ�
@param inParent �e�E�C���h�E
*/
void	CWindowImp::SetWindowStyleToSheet( CWindowImp& inParent )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//�E�C���h�E�X�^�C���Ɛe�E�C���h�ERef��ݒ�
	mWindowStyle = kWindowStyle_Sheet;
	mParentWindowRef = inParent.GetWindowRef();
	mParentWindow = &inParent;
	mParentWindow->SetHasSheet(true);
}

/**
�c�[���o�[����
*/
void	CWindowImp::CreateToolbar( const AIndex inType )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	[CocoaObjects->GetWindow() setToolbarType:inType];
	//
	switch(inType)
	{
	  case 0:
		{
			CocoaObjects->CreateToolbar(@"MainWindowToolbar",false);
			break;
		}
	  case 1:
		{
			CocoaObjects->CreateToolbar(@"AppPref",true);
			[[CocoaObjects->GetWindow() toolbar] setSelectedItemIdentifier:@"999000"];
			break;
		}
	  case 2:
		{
			CocoaObjects->CreateToolbar(@"ModePref",true);
			[[CocoaObjects->GetWindow() toolbar] setSelectedItemIdentifier:@"999000"];
			break;
		}
	}
}

/**
�E�C���h�E�\��
*/
void	CWindowImp::Show()
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�e�E�C���h�E�Ɏq�Ƃ��ēo�^�i�q�̏�Ԃ�orderOut����Ɛe��orderOut���邽�߁AHide()���A�e�q��ԉ������Ă���j
		NSWindow*	parent = CocoaObjects->GetParentWindow();
		if( parent != nil )
		{
			[parent addChildWindow:(CocoaObjects->GetWindow()) ordered:NSWindowAbove];
		}
		
		switch(mWindowStyle)
		{
			//Sheet�E�C���h�E�̏ꍇ
		  case kWindowStyle_Sheet:
			{
				if( sAliveWindowArray.Find(mParentWindow) != kIndex_Invalid )
				{
					//�V�[�g�E�C���h�E�\��
					[NSApp beginSheet:CocoaObjects->GetWindow() 
					modalForWindow:mParentWindow->CocoaObjects->GetWindow() 
					modalDelegate:CocoaObjects->GetWindow() 
					didEndSelector:@selector(didEndSheet:returnCode:contextInfo:) 
					contextInfo:nil];
				}
				break;
			}
			//�ʏ�E�C���h�E
		  case kWindowStyle_Normal:
		  default:
			{
				//�őO�ʂɕ\��
				[CocoaObjects->GetWindow() orderFront:nil];
				break;
			}
		}
		/*
		//#688
		//�L�����b�g�I�[�o�[���C�E�C���h�E�擾�i�����j
		NSWindow*	caretWindow = CocoaObjects->GetCaretOverlayWindow();
		if( caretWindow != nil )
		{
			//�L�����b�g�I�[�o�[���C�E�C���h�E���q�Ƃ��Ĕz�u
			[CocoaObjects->GetWindow() addChildWindow:caretWindow ordered:NSWindowAbove];
			//�L�����b�g�I�[�o�[���C�E�C���h�E�\��
			[caretWindow orderFront:nil];
		}
		*/
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		//�C�x���g�n���h���C���X�g�[���ς݂łȂ���΂����ŃC���X�g�[��
		if( mInstalledEventHandler.GetItemCount() == 0 )
		{
			InstallEventHandler();
		}
		
		//
		UpdateTab();
		//�E�C���h�E�\��
		switch(mWindowStyle)
		{
			//Drawer
		  case kWindowStyle_Drawer:
			{
				err = ::OpenDrawer(mWindowRef,kWindowEdgeDefault,false);
				if( err != noErr )   _ACErrorNum("OpenDrawer() returned error: ",err,this);
				break;
			}
			//Sheet
		  case kWindowStyle_Sheet:
			{
				err = ::ShowSheetWindow(mWindowRef,mParentWindowRef);
				if( err != noErr )   _ACErrorNum("ShowSheetWindow() returned error: ",err,this);
				break;
			}
			//�ʏ�E�C���h�E
		  case kWindowStyle_Normal:
		  default:
			{
				::ShowWindow(mWindowRef);
				break;
			}
		}
		/*#370 �����ňʒu�␳����ƍ��[��Constrain�����Ƃ���GetWindowBounds()�̒l�Ǝ��ۂ̕\���ʒu������Ȃ��Ȃ��肪���������̂ł�߂�
		//�E�C���h�E�ʒu�␳
		err = ::ConstrainWindowToScreen(mWindowRef,kWindowContentRgn,kWindowConstrainAllowPartial|kWindowConstrainMoveRegardlessOfFit,NULL,NULL);
		if( err != noErr )   _ACErrorNum("ConstrainWindowToScreen() returned error: ",err,this);
		err = ::ConstrainWindowToScreen(mWindowRef,kWindowTitleBarRgn,kWindowConstrainAllowPartial|kWindowConstrainMoveRegardlessOfFit,NULL,NULL);
		if( err != noErr )   _ACErrorNum("ConstrainWindowToScreen() returned error: ",err,this);
		*/
	}
#endif
}

/**
�E�C���h�E��\��
*/
void	CWindowImp::Hide()
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//�t�H�[�J�X�N���A
	ClearFocus();
	
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�e�E�C���h�E�Ɏq�Ƃ��ēo�^�i�q�̏�Ԃ�orderOut����Ɛe��orderOut���邽�߁j
		NSWindow*	parent = CocoaObjects->GetParentWindow();
		if( parent != nil )
		{
			[parent removeChildWindow:(CocoaObjects->GetWindow())];
		}
		
		switch(mWindowStyle)
		{
			//Sheet�E�C���h�E�̏ꍇ
		  case kWindowStyle_Sheet:
			{
				//�V�[�g�I��
				[NSApp endSheet:CocoaObjects->GetWindow()];
				mParentWindowRef = NULL;
				break;
			}
			//�ʏ�E�C���h�E�̏ꍇ
		  case kWindowStyle_Normal:
		  default:
			{
				//�E�C���h�Ehide
				[CocoaObjects->GetWindow() orderOut:nil];
				break;
			}
		}
		/*
		//#688
		//�L�����b�g�I�[�o�[���C�E�C���h�E�擾
		NSWindow*	caretWindow = CocoaObjects->GetCaretOverlayWindow();
		if( caretWindow != nil )
		{
			//�L�����b�g�I�[�o�[���C�E�C���h�E���q�o�^����
			[CocoaObjects->GetWindow() removeChildWindow:caretWindow];
			//�L�����b�g�I�[�o�[���C�E�C���h�Ehide
			[caretWindow orderOut:nil];
		}
		*/
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		//�E�C���h�E��\��
		switch(mWindowStyle)
		{
			//Drawer
		  case kWindowStyle_Drawer:
			{
				err = ::CloseDrawer(mWindowRef,false);
				if( err != noErr )   _ACErrorNum("CloseDrawer() returned error: ",err,this);
				break;
			}
			//Sheet
		  case kWindowStyle_Sheet:
			{
				err = ::HideSheetWindow(mWindowRef);
				if( err != noErr )   _ACErrorNum("HideSheetWindow() returned error: ",err,this);
				mParentWindowRef = NULL;
				mSheetParentTextDocumentID = kObjectID_Invalid;
				//mWindowStyle = kWindowStyle_Normal;
				break;
			}
			//�ʏ�E�C���h�E
		  case kWindowStyle_Normal:
		  default:
			{
				::HideWindow(mWindowRef);
			}
		}
	}
#endif
}

/**
�E�C���h�E�I��
*/
void	CWindowImp::Select()
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//#240 �E�C���h�E�I�𒼑O�̃C�x���g�i�I����������Activated���R�[�������܂Ŏ኱�̏������Ԃ�������̂ŕ\���̓s�����ɂ���Ă������������i�t���[�e�B���O�E�C���h�EHide���j�������ōs���j
	GetAWin().EVT_DoWindowBeforeSelected();
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		if( [CocoaObjects->GetWindow() canBecomeMainWindow] == YES )
		{
			//�E�C���h�E�őO�ʂɂ���B
			//#1034 ����makeKeyAndOrderFront�������ƁAbecomeKeyWindow�R�[���o�b�N���A
			//[NSApp orderedWindows]�ł܂����̃E�C���h�E���őO�ʂɂȂ��Ă��炸�A
			//AApp().SPI_UpdateFloatingSubWindowsVisibility()�ɂ��A�t���[�e�B���O�E�C���h�E���\������Ȃ���肪��������̂ŁA
			//���orderFront�������s���āA�E�C���h�E���őO�ʂɂ���B
			[CocoaObjects->GetWindow() orderFront:nil];
			//�E�C���h�E�őO�ʁA���A�L�[�t�H�[�J�X�Ώۂɂ���B
			[CocoaObjects->GetWindow() makeKeyAndOrderFront:nil];
			//�q�I�[�o�[���C�E�C���h�E��z-order�Ĕz�u
			ReorderOverlayChildWindows();
		}
		else
		{
			//�L�[�t�H�[�J�X�Ώۂɂ���
			[CocoaObjects->GetWindow() makeKeyWindow];
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�E�C���h�E�I��
		::SelectWindow(mWindowRef);
	}
#endif
}

/**
�E�C���h�E��ʂ̃E�C���h�E�̔w��Ɉړ�
*/
void	CWindowImp::SendBehind( CWindowImp& inBeforeWindow )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//�E�C���h�E��ʂ̃E�C���h�E�̔w��Ɉړ�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�����e�X�g
		if( sAliveWindowArray.Find(&inBeforeWindow) != kIndex_Invalid )
		{
			[CocoaObjects->GetWindow() orderWindow:NSWindowBelow 
			relativeTo:[inBeforeWindow.CocoaObjects->GetWindow() windowNumber]];
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		::SendBehind(mWindowRef,inBeforeWindow.GetWindowRef());
	}
#endif
}

//#567
/**
�E�C���h�E���Ŕw�ʂɈړ�
*/
void	CWindowImp::SendBehindAll()
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//�E�C���h�E��ʂ̃E�C���h�E�̔w��Ɉړ�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�����e�X�g
		[CocoaObjects->GetWindow() orderWindow:NSWindowBelow relativeTo:0];
	}
	/*#1034
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		::SendBehind(mWindowRef,NULL);
	}
	*/
}

//#688
/**
�E�C���h�E��ʂ̃E�C���h�E�̑O�ʂɈړ�
*/
void	CWindowImp::SendAbove( CWindowImp& inBeforeWindow )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//�����e�X�g
	//�E�C���h�E��ʂ̃E�C���h�E�̑O�ʂɈړ�
	if( sAliveWindowArray.Find(&inBeforeWindow) != kIndex_Invalid )
	{
		[CocoaObjects->GetWindow() orderWindow:NSWindowAbove 
		relativeTo:[inBeforeWindow.CocoaObjects->GetWindow() windowNumber]];
	}
}

//#688
/**
�E�C���h�E���Ŕw�ʂɈړ�
*/
void	CWindowImp::SendAboveAll()
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//�����e�X�g
	//�E�C���h�E�𑼂̑S�ẴE�C���h�E�̑O�ʂɈړ�
	[CocoaObjects->GetWindow() orderWindow:NSWindowAbove relativeTo:0];
}

//#688
/**
�q�I�[�o�[���C�E�C���h�E��zorder���Ĕz�u
*/
void	CWindowImp::ReorderOverlayChildWindows()
{
	[CocoaObjects->GetWindow() reorderOverlayChildWindows];
}

/**
�E�C���h�E���t���b�V��
*/
void	CWindowImp::RefreshWindow() const
{
	if( true )
	{
		//�����ĕ`��
		[CocoaObjects->GetWindow() display];
	}
#if SUPPORT_CARBON
	else
	{
		::Draw1Control(::HIViewGetRoot(mWindowRef));
		//InvalWindowRect()��HIViewSetNeedsDisplay()��DrawControls()�ł�redraw����Ȃ��̂ŁADraw1Control���g�p�B
	}
#endif
}

#if SUPPORT_CARBON
/**
�w��WindowRect��`��X�V
*/
void	CWindowImp::RefreshWindowRect( const AWindowRect& inWindowRect )
{
	OSStatus	err = noErr;
	
	HIViewRef content;
	err = ::HIViewFindByID(::HIViewGetRoot(mWindowRef),kHIViewWindowContentID,&content);
	if( err != noErr )   _ACErrorNum("HIViewFindByID() returned error: ",err,this);
	/* HIRect	rect;
	rect.origin.x = inWindowRect.left;
	rect.origin.y = inWindowRect.top;
	rect.size.width = inWindowRect.right - inWindowRect.left;
	rect.size.height = inWindowRect.bottom - inWindowRect.top;
	::HIViewSetNeedsDisplayInRect(content,&rect,true);
	::HIViewRender(content);*/
	//OSX10.3�ł�HIViewSetNeedsDisplayInRect���g���Ȃ��̂ŁA���L�ɕύX
	//�t���[���T�C�Y�Ńg���~���O���Ă����Ȃ���HIViewSetNeedsDisplayInRegion�̂ق��͂��܂������Ȃ����Ƃ�����
	AWindowRect	winRect = inWindowRect;
	if( winRect.top < 0 )   winRect.top = 0;
	if( winRect.bottom > GetWindowHeight() )   winRect.bottom = GetWindowHeight();
	if( winRect.left < 0 )   winRect.left = 0;
	if( winRect.right > GetWindowWidth() )   winRect.right = GetWindowWidth();
	Rect	rect;
	rect.left	= winRect.left;
	rect.right	= winRect.right;
	rect.top	= winRect.top;
	rect.bottom	= winRect.bottom;
	RgnHandle	rgn = ::NewRgn();
	::RectRgn(rgn,&rect);
	err = ::HIViewSetNeedsDisplayInRegion(content,rgn,true);
	if( err != noErr )   _ACErrorNum("HIViewSetNeedsDisplayInRegion() returned error: ",err,this);
	::DisposeRgn(rgn);
	err = ::HIViewRender(content);
	if( err != noErr )   _ACErrorNum("HIViewRender() returned error: ",err,this);
}
#endif

/**
Print�����ǂ�����Ԃ�

CPrintImp�ɂ�WindowProperty���ݒ肳���
*/
ABool	CWindowImp::IsPrintMode() const
{
	if( true )
	{
		//�ЂƂ܂�����̓��[�_���_�C�A���O�ł̑Ή��Ƃ���
		//�V�[�g�E�C���h�E�ł̈���ɑΉ�����ꍇ�́A���炩�̋@�\���K�v
		return false;
	}
#if SUPPORT_CARBON
	else
	{
		CPrintImp*	printImp;
		UInt32	actSize;
		OSErr	err = ::GetWindowProperty(mWindowRef,'MMKE','Prnt',sizeof(printImp),&actSize,&printImp);
		return (err==noErr);
	}
#endif
}

//B0395
/**
Window��Dock������
*/
void	CWindowImp::CollapseWindow( const ABool inCollapse )
{
	if( true )
	{
		if( inCollapse == true )
		{
			[CocoaObjects->GetWindow() miniaturize:nil];
		}
		else
		{
			[CocoaObjects->GetWindow() deminiaturize:nil];
		}
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		err = ::CollapseWindow(mWindowRef,inCollapse);
		if( err != noErr )   _ACErrorNum("CollapseWindow() returned error: ",err,this);
	}
#endif
}

//#182
/**
�E�C���h�E��Drag�iControl���Ń}�E�X�N���b�N���h���b�O�����ꍇ�p�j
*/
void	CWindowImp::DragWindow( const AControlID inControlID, const ALocalPoint& inMouseLocalPoint )
{
	if( true )
	{
		//�����Ή�
	}
#if SUPPORT_CARBON
	else
	{
		AGlobalPoint	gpt;
		GetGlobalPointFromControlLocalPoint(inControlID,inMouseLocalPoint,gpt);
		Point	pt;
		pt.h = gpt.x;
		pt.v = gpt.y;
		::DragWindow(mWindowRef,pt,NULL);
	}
#endif
}

#if SUPPORT_CARBON
//#291
/**
�I�[�o�[���C�E�C���h�E��o�^����i�e���ŃR�[���j
@param inChildWindow �o�^����q�E�C���h�E�iNULL�Ȃ�WindowGroup�̂ݐ����B�q�͓o�^���Ȃ��j#674
*/
WindowGroupRef	CWindowImp::RegisterOverlayWindow( const AWindowRef inChildWindow )//#338
{
	OSStatus	err = noErr;
	
	if( mOverlayWindowsGroupRef == NULL )
	{
		err = ::CreateWindowGroup(
					kWindowGroupAttrMoveTogether | 
					kWindowGroupAttrSelectAsLayer |//#674
					kWindowGroupAttrLayerTogether |
					kWindowGroupAttrSharedActivation|
					kWindowGroupAttrHideOnCollapse
					,&mOverlayWindowsGroupRef);
		if( err != noErr )   _ACErrorNum("CreateWindowGroup() returned error: ",err,NULL);
		err = ::SetWindowGroupParent(mOverlayWindowsGroupRef,::GetWindowGroup(mWindowRef));
		if( err != noErr )   _ACErrorNum("SetWindowGroupParent() returned error: ",err,NULL);
		err = ::SetWindowGroup(mWindowRef,mOverlayWindowsGroupRef);
		if( err != noErr )   _ACErrorNum("SetWindowGroup() returned error: ",err,NULL);
	}
	if( inChildWindow != NULL )//#674 inChildWindow��NULL�̏ꍇ��window group�̂ݐ���
	{
		err = ::SetWindowGroup(inChildWindow,mOverlayWindowsGroupRef);
		if( err != noErr )   _ACErrorNum("SetWindowGroup() returned error: ",err,NULL);
	}
	return mOverlayWindowsGroupRef;//#338
}
#endif

//#291
/**
�����i�I�[�o�[���C�E�C���h�E�j��������WindowGroup��MoveTogether������On/Off����i�q���ŃR�[���j
*/
void	CWindowImp::SetOverlayWindowMoveTogether( const ABool inMoveTogether )
{
	if( true )
	{
		//�����Ȃ�
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		if( inMoveTogether == true )
		{
			err = ::ChangeWindowGroupAttributes(::GetWindowGroup(mWindowRef),kWindowGroupAttrMoveTogether,0);
			if( err != noErr )   _ACErrorNum("ChangeWindowGroupAttributes() returned error: ",err,NULL);
		}
		else
		{
			err = ::ChangeWindowGroupAttributes(::GetWindowGroup(mWindowRef),0,kWindowGroupAttrMoveTogether);
			if( err != noErr )   _ACErrorNum("ChangeWindowGroupAttributes() returned error: ",err,NULL);
		}
	}
#endif
}

//#320
/**
�����i�I�[�o�[���C�E�C���h�E�j��������WindowGroup��kWindowGroupAttrLayerTogether������On/Off����i�q���ŃR�[���j
*/
void	CWindowImp::SetOverlayLayerTogether( const ABool inLayerTogether )
{
	if( true )
	{
		//��������
		/*
		if( inLayerTogether == true )
		{
			[CocoaObjects->GetParentWindow() addChildWindow:CocoaObjects->GetWindow() order:NSWindowAbove];
		}
		else
		{
			[CocoaObjects->GetParentWindow() removeChildWindow:CocoaObjects->GetWindow()];
		}
		*/
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		if( inLayerTogether == true )
		{
			err = ::ChangeWindowGroupAttributes(::GetWindowGroup(mWindowRef),kWindowGroupAttrLayerTogether,0);
			if( err != noErr )   _ACErrorNum("ChangeWindowGroupAttributes() returned error: ",err,NULL);
		}
		else
		{
			err = ::ChangeWindowGroupAttributes(::GetWindowGroup(mWindowRef),0,kWindowGroupAttrLayerTogether);
			if( err != noErr )   _ACErrorNum("ChangeWindowGroupAttributes() returned error: ",err,NULL);
		}
	}
#endif
}

//#688
/**
�E�C���h�E��paint rect
*/
void	CWindowImp::PaintRect( const AWindowRect& inRect, const AColor& inColor )
{
	//�����Ή�
}

//#409
/**
�E�C���h�E�𑦎��X�V����
@note �`�悷�ׂ��̈悪����Ε`��{�E�C���h�Eflush
*/
void	CWindowImp::UpdateWindow()
{
	//fprintf(stderr,"%16X UpdateWindow() - Start\n",(int)(GetObjectID().val));
	
	if( true )
	{
		//�`�悷�ׂ��̈悪����Ε`��
		[CocoaObjects->GetWindow() displayIfNeeded];
		//�E�C���h�Eflush
		[CocoaObjects->GetWindow() flushWindowIfNeeded];
		/*
		//�L�����b�g�I�[�o�[���C�E�C���h�E��flush
		NSWindow*	caretWin = CocoaObjects->GetCaretOverlayWindow(false);
		if( caretWin != nil )
		{
			[caretWin flushWindowIfNeeded];
		}
		*/
	}
#if SUPPORT_CARBON
	else
	{
		//#420 update�̈��DoDraw�����s����
		OSStatus	err = ::HIViewRender(::HIViewGetRoot(mWindowRef));
		if( err != noErr )   _ACErrorNum("HIViewRender() returned error: ",err,this);
		//
		::QDFlushPortBuffer(::GetWindowPort(mWindowRef),NULL);
	}
#endif
	
	//fprintf(stderr,"%16X UpdateWindow() - End\n",(int)(GetObjectID().val));
}

//win
/**
*/
void	CWindowImp::Zoom()
{
	[CocoaObjects->GetWindow() performZoom:nil];
}

//#404
/**
�t���X�N���[�����[�h
*/
void	CWindowImp::SetFullScreenMode( const ABool inFullScreenMode )
{
	if( true )
	{
		//�����Ή�
	}
#if SUPPORT_CARBON
	else
	{
		if( inFullScreenMode == true && mFullScreenMode == false )
		{
			//�E�C���h�E�̈�L��
			GetWindowBounds(mWindowBoundsInStandardState);
			//�t���X�N���[���ݒ�
			::SetSystemUIMode(kUIModeAllSuppressed,0);
			::ChangeWindowAttributes(mWindowRef,kWindowNoTitleBarAttribute,0);
			//#476 �t���X�N���[�����ɃE�C���h�E�T�C�Y���L�����Ȃ��悤�ɐ�Ƀt���X�N���[�����[�h�ݒ�
			mFullScreenMode = inFullScreenMode;
			//�E�C���h�E�̈�ݒ�
			GDHandle	mainMonitor = ::GetMainDevice();
			Rect	rect = (*mainMonitor)->gdRect;
			SetWindowBounds(rect);
		}
		else if( inFullScreenMode == false && mFullScreenMode == true )
		{
			//�t���X�N���[������
			::SetSystemUIMode(kUIModeNormal,0);
			::ChangeWindowAttributes(mWindowRef,0,kWindowNoTitleBarAttribute);
			//#476 �t���X�N���[�����ɃE�C���h�E�T�C�Y���L�����Ȃ��悤�ɐ�Ƀt���X�N���[�����[�h�ݒ�
			mFullScreenMode = inFullScreenMode;
			//�E�C���h�E�̈�����ɖ߂�
			SetWindowBounds(mWindowBoundsInStandardState);
		}
		//
		//#476 mFullScreenMode = inFullScreenMode;
	}
#endif
}

//#505
/**
�E�C���h�E�̍ŏ��T�C�Y�ݒ�
*/
void	CWindowImp::SetWindowMinimumSize( const ANumber inWidth, const ANumber inHeight )
{
	if( true )
	{
		//�ŏ��T�C�Y�f�[�^�ݒ�
		mMinWidth = inWidth;
		mMinHeight = inHeight;
		//�E�C���h�E�����ς݂Ȃ���E�C���h�E�ɐݒ�
		if( IsWindowCreated() == true )
		{
			NSSize	size = {inWidth,inHeight};
			[CocoaObjects->GetWindow() setContentMinSize:size];
		}
	}
#if SUPPORT_CARBON
	else
	{
		HISize	min = {0,0};
		min.width = inWidth;
		min.height = inHeight;
		OSStatus	err = ::SetWindowResizeLimits(mWindowRef,&min,NULL);
		if( err != noErr )   _ACErrorNum("SetWindowResizeLimits() returned error: ",err,this);
	}
#endif
}

/**
�E�C���h�E�̍ŏ��T�C�Y�ݒ�
*/
void	CWindowImp::SetWindowMaximumSize( const ANumber inWidth, const ANumber inHeight )
{
	//�ŏ��T�C�Y�f�[�^�ݒ�
	mMaxWidth = inWidth;
	mMaxHeight = inHeight;
	//�E�C���h�E�����ς݂Ȃ���E�C���h�E�ɐݒ�
	if( IsWindowCreated() == true )
	{
		NSSize	size = {inWidth,inHeight};
		[CocoaObjects->GetWindow() setContentMaxSize:size];
	}
}

/**
�N���X�F�t���[�e�B���O�E�C���h�E�ɕύX
*/
void	CWindowImp::ChangeToFloatingWindow()
{
	switch(mWindowClass)
	{
	  case kWindowClass_Document:
		{
			_ACError("not supported",NULL);
			break;
		}
	  case kWindowClass_Overlay:
		{
			//�N���X�ύX
			mWindowClass = kWindowClass_Floating;
			//#1133
			//�t���[�e�B���O�E�C���h�E�̐ݒ�
			CocoaObjects->SetupFloatingWindow(mNonFloatingFloating,mIgnoreClick);
			/*#1133
			//�t���[�e�B���O�p�l���ݒ�
			[CocoaObjects->GetWindow() setFloatingPanel:YES];
			//�E�C���h�E���x���ݒ� #1034
			[CocoaObjects->GetWindow() setLevel:NSFloatingWindowLevel];
			//�V���h�E�\������
			[CocoaObjects->GetWindow() setHasShadow:YES];
			//�E�C���h�Edeactivate���Ahide����
			[CocoaObjects->GetWindow() setHidesOnDeactivate:YES];
			*/
			//�e�E�C���h�E����̓o�^����
			//if( CWindowImp::GetWindowImpByWindowRef(inParentWindowRef).IsVisible() == true )
			{
				NSWindow*	parent = CocoaObjects->GetParentWindow();
				if( parent != nil )
				{
					[parent removeChildWindow:(CocoaObjects->GetWindow())];
				}
			}
			//�e�E�C���h�E�ւ̃����N���J��
			CocoaObjects->ClearParentWindow();
			break;
		}
	  case kWindowClass_Floating:
		{
			//�����Ȃ�
			break;
		}
	}
}

/**
�N���X�F�I�[�o�[���C�E�C���h�E�ɕύX
*/
void	CWindowImp::ChangeToOverlayWindow( const AWindowRef inParentWindowRef )
{
	switch(mWindowClass)
	{
	  case kWindowClass_Document:
		{
			_ACError("not supported",NULL);
			break;
		}
	  case kWindowClass_Overlay:
		{
			//�����Ȃ�
			break;
		}
	  case kWindowClass_Floating:
		{
			//�N���X�ݒ�
			mWindowClass = kWindowClass_Overlay;
			//�t���[�e�B���O�p�l���ݒ�
			[CocoaObjects->GetWindow() setFloatingPanel:NO];
			//�E�C���h�E���x���ݒ� #1034
			[CocoaObjects->GetWindow() setLevel:NSNormalWindowLevel];
			//�V���h�E�\������
			[CocoaObjects->GetWindow() setHasShadow:NO];
			//�E�C���h�Edeactivate���Ahide���Ȃ�
			[CocoaObjects->GetWindow() setHidesOnDeactivate:NO];
			//�e�E�C���h�E�ւ̃����N��ݒ�
			CocoaObjects->SetParentWindow(CWindowImp::GetWindowImpByWindowRef(inParentWindowRef).CocoaObjects->GetWindow());
			//�e�E�C���h�E�ւ̓o�^
			if( CWindowImp::GetWindowImpByWindowRef(inParentWindowRef).IsVisible() == true )
			{
				NSWindow*	parent = CocoaObjects->GetParentWindow();
				if( parent != nil )
				{
					[parent addChildWindow:(CocoaObjects->GetWindow()) ordered:NSWindowAbove];
					//�e�E�C���h�E�̊e�q�E�C���h�E��z-order����
					[parent reorderOverlayChildWindows];
				}
			}
			break;
		}
	}
}

/**
���̃E�C���h�E�Ń��[�_���Z�b�V�����J�n
*/
void	CWindowImp::StartModalSession()
{
	CocoaObjects->StartModalSession();
}

/**
���̃E�C���h�E�̃��[�_���Z�b�V�����p���`�F�b�N
*/
ABool	CWindowImp::CheckContinueingModalSession()
{
	return CocoaObjects->CheckContinueingModalSession();
}

/**
���̃E�C���h�E�̃��[�_���Z�b�V�����I��
*/
void	CWindowImp::EndModalSession()
{
	CocoaObjects->EndModalSession();
}

/**
���̃E�C���h�E�̃��[�_���Z�b�V�������f
*/
void	CWindowImp::StopModalSession()
{
	CocoaObjects->StopModalSession();
}

/**
���[�_���E�C���h�E���[�v�J�n�i�E�C���h�E���̃C�x���g������StopModalSession���R�[�������܂ŏI�����Ȃ��j
*/
void	CWindowImp::RunModalWindow()
{
	CocoaObjects->RunModalWindow();
}

/**
�E�C���h�E�T�C�Y���}�[�J�[�ʒu�ɍ��킹�Đݒ�
*/
void	CWindowImp::OptimizeWindowBounds( const AControlID inBottomMarker, const AControlID inRightMarker )
{
	NSSize	origMinSize = [CocoaObjects->GetWindow() contentMinSize];
	NSSize	origMaxSize = [CocoaObjects->GetWindow() contentMaxSize];
	//bottom marker�ɍ��킹�Đݒ�
	if( inBottomMarker != kControlID_Invalid )
	{
		NSView*	bottomMarker = CocoaObjects->FindViewByTag(inBottomMarker);
		if( bottomMarker != nil )
		{
			//��Umin/max������
			SetWindowMinimumSize(0,0);
			SetWindowMaximumSize(100000,100000);
			//
			AWindowPoint	wpt = {0};
			GetControlPosition(inBottomMarker,wpt);
			ARect	windowBounds = {0};
			GetWindowBounds(windowBounds);
			windowBounds.bottom = windowBounds.top + wpt.y + GetControlHeight(inBottomMarker);
			SetWindowBounds(windowBounds,true);
			//
			SetWindowMinimumSize(origMinSize.width,windowBounds.bottom-windowBounds.top);
			SetWindowMaximumSize(origMaxSize.width,windowBounds.bottom-windowBounds.top);
		}
	}
	//right marker�ɍ��킹�Đݒ�
	if( inRightMarker != kControlID_Invalid )
	{
		NSView*	rightMarker = CocoaObjects->FindViewByTag(inRightMarker);
		if( rightMarker != nil )
		{
			//��Umin/max������
			SetWindowMinimumSize(0,0);
			SetWindowMaximumSize(100000,100000);
			//
			AWindowPoint	wpt = {0};
			GetControlPosition(inRightMarker,wpt);
			ARect	windowBounds = {0};
			GetWindowBounds(windowBounds);
			windowBounds.right = windowBounds.left + wpt.x + GetControlWidth(inRightMarker);
			SetWindowBounds(windowBounds,true);
			//
			SetWindowMinimumSize(origMinSize.width,windowBounds.bottom-windowBounds.top);
			SetWindowMaximumSize(origMaxSize.width,windowBounds.bottom-windowBounds.top);
		}
	}
}

/**
�c�[���o�[�\���^��\��
*/
void	CWindowImp::ShowHideToolbar()
{
	[CocoaObjects->GetWindow() toggleToolbarShown:nil];
}

/**
�c�[���o�[�J�X�^�}�C�Y��ʕ\��
*/
void	CWindowImp::ShowToolbarCustomize()
{
	[CocoaObjects->GetWindow() runToolbarCustomizationPalette:nil];
}

/**
�c�[���o�[�\���^��\����Ԏ擾
*/
ABool	CWindowImp::IsToolBarShown() const
{
	return ([[CocoaObjects->GetWindow() toolbar] isVisible] == YES);
}

//#688
/**
�t�H���g�p�l���\��
*/
void	CWindowImp::ShowFontPanel( const AControlID inVirtualControlID, const AText& inFontName, const AFloatNumber inFontSize )
{
	//�t�H���g�p�l����virtual control id��ݒ�
	mFontPanelVirtualControlID = inVirtualControlID;
	//�t�H���g���擾
	AText	fontname;
	fontname.SetText(inFontName);
	if( AFontWrapper::IsFontEnabled(fontname) == false )
	{
		AFontWrapper::GetAppDefaultFontName(fontname);
	}
	//�w��t�H���g���t�H���g�p�l���ɐݒ�
	AStCreateNSStringFromAText	str(fontname);
	NSFont*	font = [NSFont fontWithName:str.GetNSString() size:inFontSize];
	[CocoaObjects->GetWindow() showFontPanel:font];
}

//#688
/**
�t���X�N���[����Ԃ��ǂ����𔻒�
*/
ABool	CWindowImp::IsFullScreen() const
{
	return (([CocoaObjects->GetWindow() styleMask]&(1<<14))!=0);
}

#pragma mark ===========================

#pragma mark --- �E�C���h�E���Set

/**
�E�C���h�E�^�C�g���ݒ�
*/
void	CWindowImp::SetTitle( const AText& inText )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//�^�C�g���ݒ�
	AStCreateCFStringFromAText	strref(inText);
	if( true )
	{
		AStCreateNSStringFromAText	titlestr(inText);
		[CocoaObjects->GetWindow() setTitle:titlestr.GetNSString()];
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		err = ::SetWindowTitleWithCFString(mWindowRef,strref.GetRef());
		if( err != noErr )   _ACErrorNum("SetWindowTitleWithCFString() returned error: ",err,this);
	}
#endif
}

/**
�E�C���h�E���ߗ��ݒ�
*/
void	CWindowImp::SetWindowTransparency( const AFloatNumber inTransparency )
{
	//MacOSX�̏ꍇ�A�I�[�o�[���C�E�C���h�E�̏ꍇ�́AAView_List���œh��Ԃ��Ƃ��ɓ����x�h��Ԃ������s����B
	//���̂��߁A�E�C���h�E�S�̂̓����x�ݒ�͍s��Ȃ��B
	//#1255 if( mWindowClass == kWindowClass_Overlay )   return;//win
	
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//���ߗ��ݒ�
	if( true )
	{
		[CocoaObjects->GetWindow() setAlphaValue:inTransparency];
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		err = ::SetWindowAlpha(mWindowRef,inTransparency);
		if( err != noErr )   _ACErrorNum("SetWindowAlpha() returned error: ",err,this);
	}
#endif
}

/**
�E�C���h�E�ҏW��Ԑݒ�
*/
void	CWindowImp::SetModified( const ABool inModified )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( true )
	{
		//Modified��Ԃɂ���iClose�{�^���ɍ��ە\���j
		if( inModified == true )
		{
			[CocoaObjects->GetWindow() setDocumentEdited:YES];
		}
		else
		{
			[CocoaObjects->GetWindow() setDocumentEdited:NO];
		}
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//Modified��Ԃɂ���iClose�{�^���ɍ��ە\���j
		mModified = inModified;
		err = ::SetWindowModified(mWindowRef,mModified);
		if( err != noErr )   _ACErrorNum("SetWindowModified() returned error: ",err,this);
	}
#endif
}

/**
�E�C���h�E�ɑΉ�����Ή��t�@�C���ݒ�i�^�C�g���o�[�ɃA�C�R�����\�������j
*/
void	CWindowImp::SetWindowFile( const AFileAcc& inFile )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( true )
	{
		//�t�@�C���p�X��ݒ�
		AText	path;
		inFile.GetPath(path);
		//#932 �t�@�C���A�N�Z�X�p�x�����炷���߁A�Ō�ɐݒ肵���t�@�C���ƈقȂ�ꍇ�̂ݐݒ肷��
		if( mCurrentWindowFilePath.Compare(path) == false )
		{
			AStCreateNSStringFromAText	str(path);
			[CocoaObjects->GetWindow() setRepresentedFilename:str.GetNSString()];
			//#932 �Ō�ɐݒ肵���t�@�C�����L��
			mCurrentWindowFilePath.SetText(path);
		}
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		/*OSX10.3�Ŏg�p�s��
		FSRef	fsref;
		if( inFile.GetFSRef(fsref) == true )
		{
		::HIWindowSetProxyFSRef(mWindowRef,&fsref);
		}
		*/
		//#557 Mac OS X 10.7����SetWindowProxyFSSpec()�͓���s����ɂȂ�B�b��΍�B
		//"defaults write net.mimikaki.mi ApplePersistenceIgnoreState YES"�ł��΍�ł��邪�A�e�X�g�p�Ƃ̂��ƂȂ̂ŁA�Ƃ肠������߂Ă����B
		/*#557if( AEnvWrapper::GetOSVersion() >= 0x1070 )
		{
		//��������
		return;
		}*/
		//
		FSSpec	fsspec;
		if( inFile.GetFSSpec(fsspec) == true )
		{
			err = ::SetWindowProxyFSSpec(mWindowRef,&fsspec);
			if( err != noErr )   _ACErrorNum("SetWindowProxyFSSpec() returned error: ",err,this);
		}
	}
#endif
}

/**
�E�C���h�E�ɑΉ�����Ή��t�@�C���ݒ�
*/
void	CWindowImp::ClearWindowFile()
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( true )
	{
		//��̕������ݒ�
		AText	path;
		AStCreateNSStringFromAText	str(path);
		[CocoaObjects->GetWindow() setRepresentedFilename:str.GetNSString()];
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		err = ::RemoveWindowProxy(mWindowRef);
		if( err != noErr )   _ACErrorNum("RemoveWindowProxy() returned error: ",err,this);
	}
#endif
}

/**
�f�t�H���gOK�{�^���i���^�[���L�[�ŃN���b�N�ƌ��Ȃ����{�^���j��ݒ�
*/
void	CWindowImp::SetDefaultOKButton( const AControlID inID )
{
	mDefaultOKButton = inID;
	//#688
	//���ۂɂ́AmDefaultOKButton�͎g�p���Ȃ��Ȃ�܂����B
	//Interface Builder��key equivalent�̐ݒ�����邱�ƁB
}

/**
�f�t�H���gCancel�{�^���i���^�[���L�[�ŃN���b�N�ƌ��Ȃ����{�^���j��ݒ�
*/
void	CWindowImp::SetDefaultCancelButton( const AControlID inID )
{
	mDefaultCancelButton = inID;
}

#pragma mark ===========================

#pragma mark --- �E�C���h�E���Get

/**
�E�C���h�E�^�C�g���擾
*/
void	CWindowImp::GetTitle( AText& outText ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( true )
	{
		//�^�C�g���擾
		ACocoa::SetTextFromNSString([CocoaObjects->GetWindow() title],outText);
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//�^�C�g���擾
		CFStringRef	strref = NULL;
		err = ::CopyWindowTitleAsCFString(mWindowRef,&strref);
		if( err != noErr )   _ACErrorNum("CopyWindowTitleAsCFString() returned error: ",err,this);
		if( strref != NULL )
		{
			outText.SetFromCFString(strref);
			::CFRelease(strref);
		}
	}
#endif
}

/**
�E�C���h�E�����ς݂��ǂ������擾
*/
ABool	CWindowImp::IsWindowCreated() const
{
	if( true )
	{
		return (CocoaObjects->GetWindow() != nil );
	}
#if SUPPORT_CARBON
	else
	{
		return (mWindowRef != NULL);
	}
#endif
}

/**
�E�C���h�E�\�������ǂ������擾
*/
ABool	CWindowImp::IsVisible() const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false )   return false;
	
	if( true )
	{
		//�E�C���h�E�\�������ǂ������擾
		return ([CocoaObjects->GetWindow() isVisible]==YES);
	}
#if SUPPORT_CARBON
	else
	{
		//�E�C���h�E�\�������ǂ������擾
		return ::IsWindowVisible(mWindowRef);
	}
#endif
}

/**
�E�C���h�E���A�N�e�B�u���ǂ������擾
*/
ABool	CWindowImp::IsActive() const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false )   return false;
	
	//�t���[�e�B���O�Ȃ���active��Ԃ�
	if( IsFloatingWindow() == true )
	{
		return true;
	}
	
	//
	if( true )
	{
		//�E�C���h�E��Main Window���ǂ������擾
		return ([CocoaObjects->GetWindow() isMainWindow]==YES);
	}
#if SUPPORT_CARBON
	else
	{
		//�E�C���h�E���A�N�e�B�u���ǂ������擾
		return ::IsWindowActive(mWindowRef);
	}
#endif
}

/**
�E�C���h�E��Focus����Ă��邩�ǂ����i��key window���ǂ����j��Ԃ��B
@note key window���ǂ�����Ԃ�
*/
ABool	CWindowImp::IsFocusActive() const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false )   return false;
	
	//�E�C���h�E��Key Window���ǂ������擾
	return ([CocoaObjects->GetWindow() isKeyWindow]==YES);
}

/**
�E�C���h�E���N���[�Y�{�^���������Ă��邩�ǂ������擾
*/
ABool	CWindowImp::HasCloseButton() const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//�����e�X�g
	if( true )
	{
		return ( ([CocoaObjects->GetWindow() styleMask] & NSClosableWindowMask) != 0 );
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//�E�C���h�E���N���[�Y�{�^���������Ă��邩�ǂ������擾
		WindowAttributes	attr;
		err = ::GetWindowAttributes(mWindowRef,&attr);
		if( err != noErr )   _ACErrorNum("GetWindowAttributes() returned error: ",err,this);
		return ((attr&kWindowCloseBoxAttribute)!=0);
	}
#endif
}

/**
�t���[�e�B���O�E�C���h�E���ǂ���
*/
ABool	CWindowImp::IsFloatingWindow() const
{
	//#1133
	//non-floating�E�C���h�E�t���Oon�Ȃ���false��Ԃ�
	if( mNonFloatingFloating == true )
	{
		return false;
	}
	
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	if( true )
	{
		return (mWindowClass==kWindowClass_Floating);
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//�t���[�e�B���O�E�C���h�E���ǂ���
		WindowClass	windowClass;
		err = ::GetWindowClass(mWindowRef,&windowClass);
		if( err != noErr )   _ACErrorNum("GetWindowClass() returned error: ",err,this);
		return (windowClass == kFloatingWindowClass)||(windowClass == kToolbarWindowClass);//#138 kToolbarWindowClass��ǉ�
	}
#endif
}

/**
�^�C�g���o�[�����݂��邩�ǂ������擾
*/
ABool	CWindowImp::HasTitleBar() const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//�����e�X�g
	if( true )
	{
		return ( ([CocoaObjects->GetWindow() styleMask] & NSTitledWindowMask) != 0 );
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//�E�C���h�E�����擾
		WindowAttributes	attr;
		err = ::GetWindowAttributes(mWindowRef,&attr);
		if( err != noErr )   _ACErrorNum("GetWindowAttributes() returned error: ",err,this);
		return ((attr&kWindowNoTitleBarAttribute)==0);
	}
#endif
}

/**
Modal���ǂ������擾
*/
ABool	CWindowImp::IsModalWindow() const 
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	if( true )
	{
		//CWindowImp��modal window�͍쐬���Ȃ��̂ŁAfalse��Ԃ��B
		return false;
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//Modal�����擾
		WindowModality	modality;
		WindowRef	w;
		err = ::GetWindowModality(mWindowRef,&modality,&w);
		if( err != noErr )   _ACErrorNum("GetWindowModality() returned error: ",err,this);
		return (modality == kWindowModalitySystemModal || modality == kWindowModalityAppModal);
	}
#endif
}

/**
Resize�\���ǂ������擾
*/
ABool	CWindowImp::IsResizable() const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//�����e�X�g
	if( true )
	{
		return ( ([CocoaObjects->GetWindow() styleMask] & NSResizableWindowMask) != 0 );
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//�E�C���h�E�����擾
		WindowAttributes	attr;
		err = ::GetWindowAttributes(mWindowRef,&attr);
		if( err != noErr )   _ACErrorNum("GetWindowAttributes() returned error: ",err,this);
		return ((attr&kWindowResizableAttribute)!=0);
	}
#endif
}

/**
Zoom�\���ǂ������擾
*/
ABool	CWindowImp::IsZoomable() const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//
	if( true )
	{
		//��������
		return true;
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//�E�C���h�E�����擾
		WindowAttributes	attr;
		err = ::GetWindowAttributes(mWindowRef,&attr);
		if( err != noErr )   _ACErrorNum("GetWindowAttributes() returned error: ",err,this);
		return ((attr&kWindowHorizontalZoomAttribute)!=0 || (attr&kWindowVerticalZoomAttribute)!=0);
	}
#endif
}

/**
Zoom��Ԃ��ǂ������擾
*/
ABool	CWindowImp::IsZoomed() const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//�����e�X�g
	if( true )
	{
		//zoom��Ԏ擾
		return ([CocoaObjects->GetWindow() isZoomed]==YES);
	}
#if SUPPORT_CARBON
	else
	{
		//�E�C���h�E�����擾
		return ::IsWindowInStandardState(mWindowRef,NULL,NULL);
	}
#endif
}

//win
/**
*/
void	CWindowImp::GetWindowBoundsInStandardState( ARect& outBounds ) const
{
	outBounds = mWindowBoundsInStandardState;
}

#if SUPPORT_CARBON
/**
Window��Content�����ɑΉ����郋�[�g��HIView���擾����
*/
AControlRef	CWindowImp::GetWindowContentView() const
{
	HIViewRef content;
	HIViewFindByID(::HIViewGetRoot(mWindowRef),kHIViewWindowContentID,&content);
	return content;
}
#endif

//B0395
ABool	CWindowImp::IsWindowCollapsed() const
{
	//�����e�X�g
	if( true )
	{
		//miniaturized��Ԏ擾
		return ([CocoaObjects->GetWindow() isMiniaturized]==YES);
	}
#if SUPPORT_CARBON
	else
	{
		return ::IsWindowCollapsed(mWindowRef);
	}
#endif
}

/**
WindowRef�擾
*/
AWindowRef	CWindowImp::GetWindowRef() const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACThrow("Window not created",this);return NULL;}
	//WindowRef�擾
	return mWindowRef;
}

#pragma mark ===========================

#pragma mark --- �E�C���h�E�T�C�Y�^�ʒuGet/Set

#if SUPPORT_CARBON
/**
�E�C���h�E�̈�擾
*/
void	CWindowImp::GetWindowBounds( Rect &outRect ) const
{
	OSStatus	err = noErr;
	
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//�E�C���h�EBounds�擾
	err = ::GetWindowBounds(mWindowRef,kWindowContentRgn,&outRect);
	if( err != noErr )   _ACErrorNum("GetWindowBounds() returned error: ",err,this);
}
#endif

/**
�E�C���h�E�̈�擾
*/
void	CWindowImp::GetWindowBounds( ARect& outRect ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//�E�C���h�EBounds�擾
	if( true )
	{
		NSWindow*	win = CocoaObjects->GetWindow();
		//Content view��frame���擾
		NSRect	contentFrame = [win contentRectForFrameRect:[win frame]];
		//screen��frame���擾
		NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];//(0,0)�̂���X�N���[���̃t���[��
		//screen�̍��������_�Ȃ̂��l�����ĕϊ�
		outRect.left		= contentFrame.origin.x;
		outRect.top			= firstScreenFrame.size.height - (contentFrame.origin.y + contentFrame.size.height);
		outRect.right		= contentFrame.origin.x + contentFrame.size.width;
		outRect.bottom		= firstScreenFrame.size.height - contentFrame.origin.y;
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		Rect	rect;
		err = ::GetWindowBounds(mWindowRef,kWindowContentRgn,&rect);
		if( err != noErr )   _ACErrorNum("GetWindowBounds() returned error: ",err,this);
		outRect.left = rect.left;
		outRect.top = rect.top;
		outRect.right = rect.right;
		outRect.bottom = rect.bottom;
	}
#endif
}

/**
�E�C���h�E�̈�擾
*/
void	CWindowImp::GetWindowBounds( AGlobalRect& outRect ) const
{
	ARect	rect = {0};
	GetWindowBounds(rect);
	outRect.left 	= rect.left;
	outRect.top 	= rect.top;
	outRect.right 	= rect.right;
	outRect.bottom 	= rect.bottom;
}

#if SUPPORT_CARBON
/**
�E�C���h�E�̈�ݒ�
*/
void	CWindowImp::SetWindowBounds( Rect &inRect )
{
	/*#688
	OSStatus	err = noErr;
	
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//�E�C���h�EBounds�ݒ�
	err = ::SetWindowBounds(mWindowRef,kWindowContentRgn,&inRect);
	if( err != noErr )   _ACErrorNum("SetWindowBounds() returned error: ",err,this);
	//#476 �t���X�N���[�����[�h�łȂ��Ƃ��̃E�C���h�E�T�C�Y���L��
	if( mFullScreenMode == false )
	{
		GetWindowBounds(mWindowBoundsInStandardState);
	}
	*/
	ARect	rect;
	rect.left		= inRect.left;
	rect.top		= inRect.top;
	rect.right		= inRect.right;
	rect.bottom		= inRect.bottom;
	SetWindowBounds(rect);
}
#endif

/**
�E�C���h�E�̈�ݒ�
*/
void	CWindowImp::SetWindowBounds( const ARect& inRect, const ABool inAnimate )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( true )
	{
		ARect	rect = inRect;
		//�ŏ��T�C�Y�����Ȃ�ŏ��T�C�Y�ɂ���
		if( rect.right - rect.left < mMinWidth )
		{
			rect.right = rect.left + mMinWidth;
		}
		if( rect.bottom - rect.top < mMinHeight )
		{
			rect.bottom = rect.top + mMinHeight;
		}
		//�ő�T�C�Y�ȏ�Ȃ�ő�T�C�Y�ɂ���
		if( rect.right - rect.left > mMaxWidth )
		{
			rect.right = rect.left + mMaxWidth;
		}
		if( rect.bottom - rect.top > mMaxHeight )
		{
			rect.bottom = rect.top + mMaxHeight;
		}
		//screen��frame���擾
		NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];//(0,0)�̂���X�N���[���̃t���[��
		//screen�̍��������_�Ȃ̂��l�����ĕϊ�
		NSRect	frame = {0};
		frame.origin.x = rect.left;
		frame.origin.y = firstScreenFrame.size.height - rect.bottom;
		frame.size.width 	= rect.right - rect.left;
		frame.size.height 	= rect.bottom - rect.top;
		//frame�ݒ�
		NSWindow*	win = CocoaObjects->GetWindow();
		/*�T�C�h�o�[������؂�h���b�O�ŃE�C���h�E�T�C�Y�������Ȃ��̂ŁA�Ƃ肠�����R�����g�A�E�g
		//
		NSRect	contentFrame = [win contentRectForFrameRect:[win frame]];
		if( contentFrame.origin.x == frame.origin.x && contentFrame.origin.y == frame.origin.y &&
					contentFrame.size.width == frame.size.width && contentFrame.size.height && frame.size.height)
		{
			return;
		}
		*/
		//�E�C���h�E���\�������ǂ������擾 #1090
		BOOL	visible = [win isVisible];
		//�q�E�C���h�E�̏ꍇ�́A�e�E�C���h�E����\�����Ȃ��\���Ƃ݂Ȃ�
		NSWindow*	parent = CocoaObjects->GetParentWindow();
		if( visible == YES && parent != nil )
		{
			visible = [parent isVisible];
		}
		//
		if( inAnimate == true && visible == YES )
		{
			[win setFrame:[win frameRectForContentRect:frame] display:YES animate:YES];
		}
		else
		{
			[win setFrame:[win frameRectForContentRect:frame] display:visible];//#1090 �E�C���h�E��\�����Ȃ�display:NO�Ƃ���
		}
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//�E�C���h�EBounds�ݒ�
		Rect	rect;
		rect.left	= inRect.left;
		rect.right	= inRect.right;
		if( rect.right <= rect.left )   rect.right = rect.left+1;//#212 �G���[���␳
		rect.top	= inRect.top;
		rect.bottom	= inRect.bottom;
		if( rect.bottom <= rect.top )   rect.bottom = rect.top+1;//#212 �G���[���␳
		err = ::SetWindowBounds(mWindowRef,kWindowContentRgn,&rect);
		if( err != noErr )   _ACErrorNum("SetWindowBounds() returned error: ",err,this);
	}
#endif
	//#476 �t���X�N���[�����[�h�łȂ��Ƃ��̃E�C���h�E�T�C�Y���L��
	if( mFullScreenMode == false )
	{
		GetWindowBounds(mWindowBoundsInStandardState);
	}
}	

//#404
/**
�E�C���h�E�̈�ݒ�
*/
void	CWindowImp::SetWindowBounds( const AGlobalRect& inRect, const ABool inAnimate )
{
	ARect	rect;
	rect.left		= inRect.left;
	rect.top		= inRect.top;
	rect.right		= inRect.right;
	rect.bottom		= inRect.bottom;
	SetWindowBounds(rect,inAnimate);
}

/**
�E�C���h�E���擾
*/
ANumber	CWindowImp::GetWindowWidth() const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return 100;}
	//�E�C���h�EBounds�擾
	ARect	rect;
	GetWindowBounds(rect);
	return rect.right-rect.left;
}

/**
�E�C���h�E���ݒ�
*/
void	CWindowImp::SetWindowWidth( const ANumber inWidth )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//�E�C���h�EBounds�擾
	ARect	rect;
	GetWindowBounds(rect);
	//�E�C���h�EBounds�ݒ�
	rect.right = rect.left + inWidth;
	SetWindowBounds(rect);
}

/**
�E�C���h�E�����擾
*/
ANumber	CWindowImp::GetWindowHeight() const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return 100;}
	//�E�C���h�EBounds�擾
	ARect	rect;
	GetWindowBounds(rect);
	return rect.bottom-rect.top;
}

/**
�E�C���h�E�����ݒ�
*/
void	CWindowImp::SetWindowHeight( const ANumber& inHeight )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//�E�C���h�EBounds�擾
	ARect	rect;
	GetWindowBounds(rect);
	//�E�C���h�EBounds�ݒ�
	rect.bottom = rect.top + inHeight;
	SetWindowBounds(rect);
}

/**
�E�C���h�E�T�C�Y�ݒ�
*/
void	CWindowImp::SetWindowSize( const ANumber inWidth, const ANumber inHeight, const ABool inAnimate )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//�E�C���h�EBounds�擾
	ARect	rect;
	GetWindowBounds(rect);
	//�E�C���h�EBounds�ݒ�
	rect.right	= rect.left + inWidth;
	rect.bottom	= rect.top + inHeight;
	SetWindowBounds(rect,inAnimate);
}

/**
�E�C���h�E�ʒu�擾
*/
void	CWindowImp::GetWindowPosition( APoint& outPoint ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//�E�C���h�EBounds�擾
	ARect	rect = {0};
	GetWindowBounds(rect);
	//����|�C���g�擾
	outPoint.x = rect.left;
	outPoint.y = rect.top;
	/*#688
	 OSStatus	err = noErr;
	 
	Rect	rect;
	err = ::GetWindowBounds(mWindowRef,kWindowContentRgn,&rect);
	if( err != noErr )   _ACErrorNum("GetWindowBounds() returned error: ",err,this);
	outPoint.x = rect.left;
	outPoint.y = rect.top;
	*/
}

/**
�E�C���h�E�ʒu�ݒ�
*/
void	CWindowImp::SetWindowPosition( const APoint& inPoint )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//�E�C���h�E�ړ�
	if( true )
	{
		/*
		NSRect	screenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
		NSPoint	origin = {0};
		origin.x = inPoint.x;
		origin.y = screenFrame.size.height - inPoint.y;
		CocoaWindow*	win = CocoaObjects->GetWindow();
		[win setFrameOrigin:origin];
		*/
		ARect	rect = {0};
		GetWindowBounds(rect);
		ANumber	w = rect.right - rect.left;
		ANumber	h = rect.bottom - rect.top;
		rect.left 	= inPoint.x;
		rect.top 	= inPoint.y;
		rect.right 	= rect.left + w;
		rect.bottom = rect.top + h;
		SetWindowBounds(rect);
	}
#if SUPPORT_CARBON
	else
	{
		::MoveWindow(mWindowRef,inPoint.x,inPoint.y,false);
	}
#endif
	//#476 �t���X�N���[�����[�h�łȂ��Ƃ��̃E�C���h�E�T�C�Y���L��
	if( mFullScreenMode == false )
	{
		GetWindowBounds(mWindowBoundsInStandardState);
	}
}

/**
�E�C���h�E�ʒu����ʂɍ��킹�ĕ␳
*/
void	CWindowImp::ConstrainWindowPosition( const ABool inAllowPartial )//#370
{
	//#404 �t���X�N���[�����͈ʒu�␳���Ȃ�
	if( mFullScreenMode == true )   return;
	
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//�ʒu�␳
	if( true )
	{
		/*#688
		OSStatus	err = noErr;
		
		//#370 kWindowConstrainAllowPartial��t����Ƃ��Ƃ��΍����������Ȃ��Ȃ��Ԃ̂Ƃ���Constrain����Ȃ�
		WindowConstrainOptions	option = kWindowConstrainMoveRegardlessOfFit;
		if( inAllowPartial == true )
		{
			option |= kWindowConstrainAllowPartial;
		}
		//
		err = ::ConstrainWindowToScreen(mWindowRef,kWindowContentRgn,option,NULL,NULL);//#370
		if( err != noErr )   _ACErrorNum("ConstrainWindowToScreen() returned error: ",err,this);
		err	= ::ConstrainWindowToScreen(mWindowRef,kWindowTitleBarRgn,option,NULL,NULL);//#370
		if( err != noErr )   _ACErrorNum("ConstrainWindowToScreen() returned error: ",err,this);
		*/
		APoint	pt = {0};
		GetWindowPosition(pt);
		ConstrainWindowPosition(inAllowPartial,pt);
		SetWindowPosition(pt);
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//#370 kWindowConstrainAllowPartial��t����Ƃ��Ƃ��΍����������Ȃ��Ȃ��Ԃ̂Ƃ���Constrain����Ȃ�
		WindowConstrainOptions	option = kWindowConstrainMoveRegardlessOfFit;
		if( inAllowPartial == true )
		{
			option |= kWindowConstrainAllowPartial;
		}
		//
		err = ::ConstrainWindowToScreen(mWindowRef,kWindowContentRgn,option,NULL,NULL);//#370
		if( err != noErr )   _ACErrorNum("ConstrainWindowToScreen() returned error: ",err,this);
		err	= ::ConstrainWindowToScreen(mWindowRef,kWindowTitleBarRgn,option,NULL,NULL);//#370
		if( err != noErr )   _ACErrorNum("ConstrainWindowToScreen() returned error: ",err,this);
	}
#endif
}

/**
�E�C���h�E�ʒu����ʂɍ��킹�ĕ␳�����ꍇ�̈ʒu���擾
*/
void	CWindowImp::ConstrainWindowPosition( const ABool inAllowPartial, APoint& ioPoint ) const
{
	//screen bounds�擾
	AGlobalRect	screenBounds = {0};
	AIndex	screenIndex = FindScreenIndexFromPoint(ioPoint);
	if( screenIndex == kIndex_Invalid )
	{
		//�w��|�C���g���X�N���[���O�̏ꍇ�́A���݂̃E�C���h�E�̂���X�N���[����bounds���擾
		GetAvailableWindowBoundsInSameScreen(screenBounds);
	}
	else
	{
		//�w��|�C���g��������X�N���[����screen bounds���擾
		GetAvailableWindowBounds(screenIndex,screenBounds);
	}
	//
	if( ioPoint.x < screenBounds.left )
	{
		ioPoint.x = screenBounds.left;
	}
	if( ioPoint.x + 20 > screenBounds.right )
	{
		ioPoint.x = screenBounds.right -20;
	}
	if( ioPoint.y < screenBounds.top )
	{
		ioPoint.y = screenBounds.top;
	}
	if( ioPoint.y + 20 > screenBounds.bottom )//+20�̓^�C�g���o�[��
	{
		ioPoint.y = screenBounds.bottom -20;
	}
	//��allow partial���Ή��i���inAllowPartial=true�̓���j
}

//#385
/**
�E�C���h�E�T�C�Y����ʂɍ��킹�ĕ␳
*/
void	CWindowImp::ConstrainWindowSize()
{
	//#404 �t���X�N���[�����͈ʒu�␳���Ȃ�
	if( mFullScreenMode == true )   return;
	
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//��ʃT�C�Y�ɉ����ăE�C���h�E�T�C�Y�␳
	if( true )
	{
		//�����Ȃ�
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		Rect	titlebarRect;
		err = ::GetWindowBounds(mWindowRef,kWindowTitleBarRgn,&titlebarRect);
		if( err != noErr )   _ACErrorNum("GetWindowBounds() returned error: ",err,this);
		Rect	contentRect;
		err = ::GetWindowBounds(mWindowRef,kWindowContentRgn,&contentRect);
		if( err != noErr )   _ACErrorNum("GetWindowBounds() returned error: ",err,this);
		//��ʃT�C�Y�擾
		AGlobalRect	globalRect;
		CWindowImp::GetAvailableWindowPositioningBounds(globalRect);
		/*�T�C�Y�{�b�N�X��������Ηǂ��̂ŁA�������␳�͂��Ȃ�
		if( (contentRect.right-contentRect.left) > (globalRect.right-globalRect.left) )
		{
		contentRect.right = contentRect.left + (globalRect.right-globalRect.left);
		}
		*/
		//�^�C�g���o�[�̍����{���g��������ʃT�C�Y���傫����΁A�T�C�Y�␳
		if( (titlebarRect.bottom-titlebarRect.top)+(contentRect.bottom-contentRect.top) > (globalRect.bottom-globalRect.top) )
		{
			contentRect.bottom = contentRect.top + (globalRect.bottom-globalRect.top) - (titlebarRect.bottom-titlebarRect.top);
			//�T�C�Y�ݒ�
			err = ::SetWindowBounds(mWindowRef,kWindowContentRgn,&contentRect);
			if( err != noErr )   _ACErrorNum("SetWindowBounds() returned error: ",err,this);
			//����������܂�悤�ɁAallowPartial=false�ňʒu�␳
			ConstrainWindowPosition(false);
		}
	}
#endif
	//#476 �t���X�N���[�����[�h�łȂ��Ƃ��̃E�C���h�E�T�C�Y���L��
	if( mFullScreenMode == false )
	{
		GetWindowBounds(mWindowBoundsInStandardState);
	}
}

#pragma mark ===========================

#pragma mark --- �R���g���[������

/**
�R���g���[�������`��
*/
void	CWindowImp::DrawControl( const AControlID inID )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//�`��w��
		[view setNeedsDisplay:YES];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�R���g���[���`��
		::Draw1Control(GetControlRef(inID));
	}
#endif
}

#pragma mark ===========================

#pragma mark --- �R���g���[�����Set

/**
�R���g���[��Enable/Disable
*/
void	CWindowImp::SetEnableControl( const AControlID inID, const ABool inEnable )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//���݂Ɠ����Ȃ牽���������^�[��
	//�E�C���h�E���ŏ��ɊJ�����Ƃ���MLTEPane���D�F�ɂȂ�̂ŉE�͍폜if( IsControlEnabled(inID) == inEnable )   return;
	
	//UserPane�̏ꍇ��PreProcess�����s����
	if( IsUserPane(inID) == true )
	{
		/*#688
		GetUserPane(inID).SetEnableControl(inEnable);
		UpdateFocus();
		return;
		*/
		GetUserPane(inID).PreProcess_SetEnableControl(inEnable);//#688
	}
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//segmented control�ł̎��s������
		if( [CocoaObjects->GetWindow() isKindOfClass:[CocoaWindow class]] == YES )
		{
			CocoaWindow*	cocoaWindow = static_cast<CocoaWindow*>(CocoaObjects->GetWindow());
			BOOL	enable = NO;
			if( inEnable == true )
			{
				enable = YES;
			}
			if( [cocoaWindow setEnableForSegmentControl:inID enable:enable] == YES )
			{
				return;
			}
		}
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSControl�̏ꍇ�Aenabled�ݒ�
		if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			if( inEnable == true )
			{
				[control setEnabled:YES];
			}
			else
			{
				[control setEnabled:NO];
			}
			//�Ή�����stepper�������stepper��������enable/disable����
			if( ExistControl(kControlID_StepperOffset+inID) == true )
			{
				NSStepper*	stepper = CocoaObjects->FindViewByTag(kControlID_StepperOffset+inID);
				if( inEnable == true )
				{
					[stepper setEnabled:YES];
				}
				else
				{
					[stepper setEnabled:NO];
				}
			}
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		OSStatus	err = noErr;
		
		AControlRef	controlRef = GetControlRef(inID);
		if( ::IsControlEnabled(controlRef) == inEnable )   return;//#530
		if( inEnable == true )
		{
			err = ::EnableControl(controlRef);
			if( err != noErr )   _ACErrorNum("EnableControl() returned error: ",err,this);
		}
		else
		{
			err = ::DisableControl(controlRef);
			if( err != noErr )   _ACErrorNum("DisableControl() returned error: ",err,this);
			//���W�I�O���[�v�ɐݒ肳�ꂽ�{�^���̏ꍇ�AOn��disable�ɂ��ꂽ��A���̃{�^���̂ǂꂩ��On�ɂ���B
			/*#349 ���W�I�O���[�v�̑S�Ẵ{�^����dissable�ɂ���Ƃ���On���ړ�����̂ł��̏����̓A�v���w�ŕK�v�ȂƂ��ɍs�����Ƃɂ���
			if( GetBool(inID) == true )
			{
			UpdateRadioButtonDisabled(inID);
			}
			*/
		}
		
		//
		UpdateFocus();
	}
#endif
}

/**
�R���g���[����Enable���ǂ������擾
*/
ABool	CWindowImp::IsControlEnabled( const AControlID inID ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	/*#688
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsControlEnabled();
	}
	*/
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//segmented control�ł̎��s������
		if( [CocoaObjects->GetWindow() isKindOfClass:[CocoaWindow class]] == YES )
		{
			CocoaWindow*	cocoaWindow = static_cast<CocoaWindow*>(CocoaObjects->GetWindow());
			if( [cocoaWindow isSegmentControl:inID] == YES )
			{
				if( [cocoaWindow getEnableForSegmentControl:inID] == YES )
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSControl�̏ꍇ�Aenabled�ݒ�
		if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			return ([control isEnabled]==YES);
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
			return true;
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		AControlRef	controlRef = GetControlRef(inID);
		return ::IsControlEnabled(controlRef);
	}
#endif
}

/**
�R���g���[��Show/Hide
*/
void	CWindowImp::SetShowControl( const AControlID inID, const ABool inShow )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		/*#688
		GetUserPane(inID).SetShowControl(inShow);
		return;
		*/
		GetUserPane(inID).PreProcess_SetShowControl(inShow);//#688
	}
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//Show/Hide�ݒ�isetHidden��NSView�Œ�`����Ă���j
		BOOL	hide = NO;
		if( inShow == false )
		{
			hide = YES;
		}
		[view setHidden:hide];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		AControlRef	controlRef = GetControlRef(inID);
		if( inShow )
		{
			::ShowControl(controlRef);
		}
		else
		{
			::HideControl(controlRef);
		}
	}
#endif
}

/**
�R���g���[����Show/Hide�擾
*/
ABool	CWindowImp::IsControlVisible( const AControlID inID ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	/*#688
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsControlVisible();
	}
	*/
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//toolbar item�ł̎��s������
		NSToolbarItem*	toolbarItem = CocoaObjects->FindToolbarItemByTag(inID);
		if( toolbarItem != nil )
		{
			if( [[[CocoaObjects->GetWindow() toolbar] visibleItems] containsObject:toolbarItem] == YES )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//Show/Hide��Ԏ擾�iisHiddenOrHasHiddenAncestor��NSView�Œ�`����Ă���Bview���g�܂��͐eview��hidden�̂Ƃ�YES�ɂȂ�j
		return ([view isHiddenOrHasHiddenAncestor]==NO);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		AControlRef	controlRef = GetControlRef(inID);
		return ::IsControlVisible(controlRef);
	}
#endif
}

/**
Control�̃e�L�X�g�X�^�C���ݒ�
*/
void	CWindowImp::SetTextStyle( const AControlID inID, const ABool inBold )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetTextStyle(inBold);
		return;
	}
	//�ʏ�R���g���[���̏ꍇ
	//������
}

//�e�L�X�g�t�H���g�ݒ�
/*win void	CWindowImp::SetTextFont( const AControlID inID, const AFont inFont, const AFontSize inFontSize )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetTextFont(inFont,inFontSize);
		return;
	}
	//�ʏ�R���g���[���̏ꍇ
	ControlFontStyleRec	fontStyle;
	fontStyle.font = inFont;
	fontStyle.size = inFontSize;
	fontStyle.flags = kControlUseFontMask + kControlUseSizeMask;
	::SetControlFontStyle(GetControlRef(inID),&fontStyle);
}*/

/**
Control��TextFont��ݒ�
*/
void	CWindowImp::SetTextFont( const AControlID inID, const AText& inFontName, const AFloatNumber inFontSize )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetTextFont(inFontName,inFontSize);
		return;
	}
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSControl�̏ꍇ�Afont�ݒ�
		if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			//font�ݒ�
			AStCreateNSStringFromAText	str(inFontName);
			[control setFont:[NSFont fontWithName:str.GetNSString() size:inFontSize]];
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[���̏ꍇ
		OSStatus	err = noErr;
		
		SInt16	font;
		AFontWrapper::GetFontByName(inFontName,font);
		ControlFontStyleRec	fontStyle;
		fontStyle.font = font;
		fontStyle.size = inFontSize;
		fontStyle.flags = kControlUseFontMask + kControlUseSizeMask;
		err = ::SetControlFontStyle(GetControlRef(inID),&fontStyle);
		if( err != noErr )   _ACErrorNum("SetControlFontStyle() returned error: ",err,this);
	}
#endif
}

/**
���^�[���L�[���R���g���[�����ŏ������邩�ǂ�����ݒ�
*/
void	CWindowImp::SetCatchReturn( const AControlID inID, const ABool inCatchReturn )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetCatchReturn(inCatchReturn);
		return;
	}
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�����Ȃ�
		//���^�[���L�[�^�^�u�L�[�̏�����OS�ɔC����
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[���̏ꍇ
		_AError("not implemented",this);
	}
#endif
}

/**
���^�[���L�[���R���g���[�����ŏ������邩�ǂ������擾
*/
ABool	CWindowImp::IsCatchReturn( const AControlID inID ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsCatchReturn();
	}
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�����Ȃ�
		//���^�[���L�[�^�^�u�L�[�̏�����OS�ɔC����
		return false;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[���̏ꍇ
		_AError("not implemented",this);
		return false;
	}
#endif
}

/**
�^�u�L�[���R���g���[�����ŏ������邩�ǂ�����ݒ�
*/
void	CWindowImp::SetCatchTab( const AControlID inID, const ABool inCatchTab )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetCatchTab(inCatchTab);
		return;
	}
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�����Ȃ�
		//���^�[���L�[�^�^�u�L�[�̏�����OS�ɔC����
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[���̏ꍇ
		_AError("not implemented",this);
	}
#endif
}

/**
�^�u�L�[���R���g���[�����ŏ������邩�ǂ������擾
*/
ABool	CWindowImp::IsCatchTab( const AControlID inID ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsCatchTab();
	}
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�����Ȃ�
		//���^�[���L�[�^�^�u�L�[�̏�����OS�ɔC����
		return false;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[���̏ꍇ
		return false;
	}
#endif
}

/**
Focus��Switch�����Ƃ��ɑS�I�����邩�ǂ�����ݒ�
*/
void	CWindowImp::SetAutoSelectAllBySwitchFocus( const AControlID inID, ABool inAutoSelectAll )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetAutoSelectAllBySwitchFocus(inAutoSelectAll);
		return;
	}
	//�ʏ�R���g���[���̏ꍇ
	if( true )
	{
		//�����Ȃ�
		//select all���邩�ǂ����̓N���X����Ƃ���iNSTextField�Ȃ�I������select all�����j
	}
#if SUPPORT_CARBON
	else
	{
		_AError("not implemented",this);
	}
#endif
}

/**
Focus��Switch�����Ƃ��ɑS�I�����邩�ǂ������擾
*/
ABool	CWindowImp::IsAutoSelectAllBySwitchFocus( const AControlID inID ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsAutoSelectAllBySwitchFocus();
	}
	//�ʏ�R���g���[���̏ꍇ
	if( true )
	{
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSTextField�̏ꍇ�́Afocus�ݒ莞�ɑS�I�������̂ŁAtrue��Ԃ�
		if( [view isKindOfClass:[NSTextField class]] == YES )
		{
			return true;
		}
		//��L�ȊO�́Afalse��Ԃ�
		else
		{
			return false;
		}
	}
#if SUPPORT_CARBON
	else
	{
		//_AError("not implemented",this);
		return false;
	}
#endif
}

/**
�R���g���[���̃f�[�^�^�C�v��ݒ肷��

���̃f�[�^�^�C�v�ɂ���ăL�[���͎��̃f�[�^�`�F�b�N���s����
*/
void	CWindowImp::SetDataType( const AControlID inControlID, const ADataType inDataType )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	mDataType_ControlID.Add(inControlID);
	mDataType_DataType.Add(inDataType);
}

/**
Control�ʒu�ݒ�
*/
void	CWindowImp::SetControlPosition( const AControlID inID, const AWindowPoint& inPoint )
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//������WindowPoint��eView��̈ʒu�֕ϊ�
		//pt�ɐeview��̈ʒu���i�[����
		NSView*	superView = [view superview];
		NSPoint	wpt = ConvertFromWindowPointToNSPoint(CocoaObjects->GetContentView(),inPoint);
		NSPoint	pt = [CocoaObjects->GetContentView() convertPoint:wpt toView:superView];
		
		//�ʒu�ݒ�
		if( [superView isFlipped] == NO )
		{
			//�e��flip:no�̏ꍇ�A�i�qview�̍���ł͂Ȃ��j�qview�̍����ʒu���AsetFrameOrigin�Őݒ肷��
			pt.y = pt.y - [view frame].size.height;
		}
		//#852
		//view�̈ʒu�ɕύX��������Ή��������I��
		NSRect	viewFrame = [view frame];
		if( viewFrame.origin.x == pt.x && viewFrame.origin.y == pt.y )
		{
			return;
		}
		//�ʒu�ݒ�
		[view setFrameOrigin:pt];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		/*#182 �^�u�R���g���[�����̕ʂ�View���ɑ��݂���View�̏ꍇ��View�ʒu�����������C��
		HIViewSetFrame()�͐eView���Έʒu��ݒ肷��̂ŁAHIViewConvertPoint()��WindowPoint��eView���Έʒu�ɕϊ����Đݒ肷��悤�ɂ���
		//win 080618 ::MoveControl(GetControlRef(inID),inPoint.x,inPoint.y);
		//win 080618 
		AIndex	tabIndex = GetControlEmbeddedTabIndex(inID);
		if( tabIndex == kIndex_Invalid )
		{
		::MoveControl(GetControlRef(inID),inPoint.x,inPoint.y);
		}
		else
		{
		HIPoint	pt;
		pt.x = inPoint.x;
		pt.y = inPoint.y;
		::HIViewConvertPoint(&pt,GetWindowContentView(),GetTabControlRef(tabIndex));
		::MoveControl(GetControlRef(inID),pt.x,pt.y);
		}
		*/
		//�eView�擾
		AControlRef	parent = NULL;
		parent = ::HIViewGetSuperview(GetControlRef(inID));
		//������WindowPoint��eView��̈ʒu�֕ϊ�
		HIPoint	pt;
		pt.x = inPoint.x;
		pt.y = inPoint.y;
		err = ::HIViewConvertPoint(&pt,GetWindowContentView(),parent);
		if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
		//���݂̈ʒu�E�T�C�Y���擾
		HIRect	rect;
		err = ::HIViewGetFrame(GetControlRef(inID),&rect);
		if( err != noErr )   _ACErrorNum("HIViewGetFrame() returned error: ",err,this);
		//�ʒu��ύX���Đݒ�
		rect.origin = pt;
		err = ::HIViewSetFrame(GetControlRef(inID),&rect);
		if( err != noErr )   _ACErrorNum("HIViewSetFrame() returned error: ",err,this);
	}
#endif
}

/**
�R���g���[���T�C�Y�ݒ�
*/
void	CWindowImp::SetControlSize( const AControlID inID, const ANumber inWidth, const ANumber inHeight )
{
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//���݂�frame�擾
		NSRect	viewFrame = [view frame];
		//#852
		//view�̃T�C�Y�ɕύX��������Ή��������I��
		if( viewFrame.size.width == inWidth && viewFrame.size.height == inHeight )
		{
			return;
		}
		//�eview��flipped:no�̏ꍇ�Aorigin�͍����Ȃ̂ŁA����Œ�ɂ��邽�߂ɂ́Aorigin���ړ�����K�v������B
		if( [[view superview] isFlipped] == NO )
		{
			viewFrame.origin.y -= (inHeight-viewFrame.size.height);
		}
		//�T�C�Y�ݒ�
		viewFrame.size.width = inWidth;
		viewFrame.size.height = inHeight;
		[view setFrame:viewFrame];
		
		//NSScroller�̏ꍇ�Awidth/height�ɉ����āA�R���g���[���̃T�C�Y�^�C�v��ݒ�
		if( [view isKindOfClass:[NSScroller class]] == YES )
		{
			NSScroller*	scroller = ACocoa::CastNSScrollerFromId(view);
			if( inWidth < 15 || inHeight < 15 )
			{
				[scroller setControlSize:NSSmallControlSize];
			}
			else
			{
				[scroller setControlSize:NSRegularControlSize];
			}
		}
		/*test
		//
		[view removeAllToolTips];
		NSRect	rect = {0};
		rect.size = [view frame].size;
		[view addToolTipRect:rect owner:view userData:nil];
		*/
		/*test
		[view removeAllToolTips];
		for( ANumber y = 0; y < inHeight; y += 2 )
		{
			//
			ALocalPoint	lpt = {0,y};
			NSRect	nsrect = {0};
			nsrect.origin = ConvertFromLocalPointToNSPoint(view,lpt);
			nsrect.size.width  = inWidth;
			nsrect.size.height = 2;
			
			[view addToolTipRect:nsrect owner:view userData:nil];
		}
		*/
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		::SizeControl(GetControlRef(inID),inWidth,inHeight);
	}
#endif
}

/**
HelpTag(Tooltip)�ݒ�
*/
void	CWindowImp::SetHelpTag( const AControlID inID, const AText& inText, const AHelpTagSide inTagSide )//#634
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//tool tip�e�L�X�g�ݒ�
		AStCreateNSStringFromAText	str(inText);
		[view setToolTip:str.GetNSString()];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		//�E�C���h�E�����ς݃`�F�b�N
		if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
		
		HMHelpContentRec helpTag;
		helpTag.version = kMacHelpVersion;
		helpTag.tagSide = kHMDefaultSide;
		//#634
		switch(inTagSide)
		{
		  case kHelpTagSide_Right:
			{
				helpTag.tagSide = kHMOutsideRightCenterAligned;
				break;
			}
		  case kHelpTagSide_Top:
			{
				helpTag.tagSide = kHMOutsideTopCenterAligned;
				break;
			}
		}
		//
		CFStringRef	string = inText.CreateCFString();//#693 ���[�N�΍�
		::SetRect(&helpTag.absHotRect, 0, 0, 0, 0);
		helpTag.content[kHMMinimumContentIndex].contentType = kHMCFStringContent;
		helpTag.content[kHMMinimumContentIndex].u.tagCFString = string;//#693 ���[�N�΍�inText.CreateCFString();//AUtil::CreateCFStringFromText(inText);
		helpTag.content[kHMMaximumContentIndex].contentType = kHMNoContent;
		err = ::HMSetControlHelpContent(GetControlRef(inID), &helpTag); 
		if( err != noErr )   _ACErrorNum("HMSetControlHelpContent() returned error: ",err,this);
		::CFRelease(string);//#693 ���[�N�΍�
	}
#endif
}

//R0246
/**
HelpTag����Hide
*/
void	CWindowImp::HideHelpTag() const
{
	if( true )
	{
		//�����Ȃ�
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		err = ::HMHideTag();
		if( err != noErr )   _ACErrorNum("HMHideTag() returned error: ",err,this);
	}
#endif
}

//R0240
/**
HelpTag�R�[���o�b�N��Enable�ɂ���
*/
void	CWindowImp::EnableHelpTagCallback( const AControlID inControlID, const ABool inShowOnlyWhenNarrow )//#507
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		/*test
		//
		[view removeAllToolTips];
		//
		AItemCount	itemCount = inToolTipRectArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			ALocalRect	localRect = inToolTipRectArray.Get(i);
			ALocalPoint	lpt = {localRect.left,localRect.top};
			NSRect	nsrect = {0};
			nsrect.origin = ConvertFromLocalPointToNSPoint(view,lpt);
			nsrect.size.width = localRect.right - localRect.left;
			nsrect.size.height = localRect.bottom - localRect.top;
			[view addToolTipRect:nsrect owner:view userData:nil];
		}
		*/
		/*
		//
		NSRect	rect = {0};
		rect.size = [view frame].size;
		[view addToolTipRect:rect owner:view userData:nil];
		*/
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		err = ::HMInstallControlContentCallback(GetControlRef(inControlID),NewHMControlContentUPP(STATIC_APICB_ControlHelpTagCallback));
		if( err != noErr )   _ACErrorNum("HMInstallControlContentCallback() returned error: ",err,this);
	}
#endif
}

/**
View��View�̒���Embed����iinChild��Control�̐e��inParent�ɂ���j
*/
void	CWindowImp::EmbedControl( const AControlID inParent, const AControlID inChild )
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	parentView = CocoaObjects->FindViewByTag(inParent);
		NSView*	childView = CocoaObjects->FindViewByTag(inChild);
		
		//���݂�WindowPoint�ł̍��W���L������
		AWindowPoint	wpt = {0};
		GetControlPosition(inChild,wpt);
		//fprintf(stderr,"EmbedControl:%d:wpt(%d,%d)\n",inChild,pt.x,pt.y);
		
		//���݂̐eview����remove
		[childView removeFromSuperview];
		//�w�肳�ꂽ�eview�Ɏq�Ƃ��Ēǉ�
		[parentView addSubview:childView positioned:NSWindowAbove relativeTo:nil];
		
		//�ʒu������WindowPoint�̍��W�ɐݒ肷��
		SetControlPosition(inChild,wpt);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		//���݂�WindowPoint�ł̍��W���L������
		AWindowPoint	pt;
		GetControlPosition(inChild,pt);
		//inParent���ɓ����
		err = ::HIViewAddSubview(GetControlRef(inParent),GetControlRef(inChild));
		if( err != noErr )   _ACErrorNum("HIViewAddSubview() returned error: ",err,this);
		//�ʒu������WindowPoint�̍��W�ɐݒ肷��
		SetControlPosition(inChild,pt);
	}
#endif
}

#if SUPPORT_CARBON
//#236
/**
Drop�\��Control��o�^����

DeleteControl()�œo�^���������
*/
void	CWindowImp::RegisterDropEnableControl( const AControlID inControlID )
{
	mDropEnableControlArray.Add(inControlID);
}
#endif

//#291
/**
�R���g���[����Zorder��ݒ�
*/
void	CWindowImp::SetControlZOrderToTop( const AControlID inControlID )
{
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		//�����Ȃ�
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		err = ::HIViewSetZOrder(GetControlRef(inControlID),kHIViewZOrderAbove,NULL);
		if( err != noErr )   _ACErrorNum("HIViewSetZOrder() returned error: ",err,this);
	}
#endif
}

//#291
void	CWindowImp::SetControlBindings( const AControlID inControlID,
		const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
		const ABool inWidthFixed, const ABool inHeightFixed )//#688
{
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		//autoresizeMask�ݒ�
		NSUInteger	autoResizingMask = NSViewNotSizable;
		if( inLeftMarginFixed == false )
		{
			autoResizingMask |= NSViewMinXMargin;
		}
		if( inTopMarginFixed == false )
		{
			//Superview��flip��Ԃɂ���āAMinY/MaxY�؂�ւ�
			if( [[view superview] isFlipped] == NO )
			{
				autoResizingMask |= NSViewMaxYMargin;
			}
			else
			{
				autoResizingMask |= NSViewMinYMargin;
			}
		}
		if( inRightMarginFixed == false )
		{
			autoResizingMask |= NSViewMaxXMargin;
		}
		if( inBottomMarginFixed == false )
		{
			//Superview��flip��Ԃɂ���āAMinY/MaxY�؂�ւ�
			if( [[view superview] isFlipped] == NO )
			{
				autoResizingMask |= NSViewMinYMargin;
			}
			else
			{
				autoResizingMask |= NSViewMaxYMargin;
			}
		}
		if( inWidthFixed == false )
		{
			autoResizingMask |= NSViewWidthSizable;
		}
		if( inHeightFixed == false )
		{
			autoResizingMask |= NSViewHeightSizable;
		}
		[view setAutoresizingMask:autoResizingMask];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		HILayoutInfo	layoutinfo = {0};
		layoutinfo.version = kHILayoutInfoVersionZero;
		err = ::HIViewGetLayoutInfo(GetControlRef(inControlID),&layoutinfo);
		if( err != noErr )   _ACErrorNum("HIViewGetLayoutInfo() returned error: ",err,NULL);
		//Top
		layoutinfo.binding.top.toView = NULL;
		layoutinfo.binding.top.offset = 0;
		if( inTop == true )
		{
			layoutinfo.binding.top.kind = kHILayoutBindTop;
		}
		else
		{
			layoutinfo.binding.top.kind = kHILayoutBindNone;
		}
		//Left
		layoutinfo.binding.left.toView = NULL;
		layoutinfo.binding.left.offset = 0;
		if( inLeft == true )
		{
			layoutinfo.binding.left.kind = kHILayoutBindLeft;
		}
		else
		{
			layoutinfo.binding.left.kind = kHILayoutBindNone;
		}
		//Bottom
		layoutinfo.binding.bottom.toView = NULL;
		layoutinfo.binding.bottom.offset = 0;
		if( inBottom == true )
		{
			layoutinfo.binding.bottom.kind = kHILayoutBindBottom;
		}
		else
		{
			layoutinfo.binding.bottom.kind = kHILayoutBindNone;
		}
		//Right
		layoutinfo.binding.right.toView = NULL;
		layoutinfo.binding.right.offset = 0;
		if( inRight == true )
		{
			layoutinfo.binding.right.kind = kHILayoutBindRight;
		}
		else
		{
			layoutinfo.binding.right.kind = kHILayoutBindNone;
		}
		err = ::HIViewSetLayoutInfo(GetControlRef(inControlID),&layoutinfo);
		if( err != noErr )   _ACErrorNum("HIViewSetLayoutInfo() returned error: ",err,NULL);
	}
#endif
}

//#565
/**
�R���g���[����FileDrop�\�ɐݒ�
*/
void	CWindowImp::EnableFileDrop( const AControlID inControlID )
{
	if( true )
	{
		//�����Ȃ�
	}
#if SUPPORT_CARBON
	else
	{
		RegisterDropEnableControl(inControlID);
	}
#endif
}

//#688
//CUserPane����ړ�
/**
�`��Refresh�i�w��Rect�j
*/
void	CWindowImp::RefreshControlRect( const AControlID inControlID, const ALocalRect inLocalRect )
{
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//
		ALocalRect	localRect = inLocalRect;
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		//�t���[���T�C�Y�Ńg���~���O
		ALocalRect	localFrameRect = {0,0,0,0};
		localFrameRect.right	= GetControlWidth(inControlID);
		localFrameRect.bottom	= GetControlHeight(inControlID);
		if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
		{
			//�c�������W�ϊ��iApp���W�n->Imp���W�n�j #558
			localRect = ConvertVerticalCoordinateFromAppToImp(inControlID,inLocalRect);
		}
		ALocalRect	intersectRect = {0,0,0,0};
		if( GetIntersectRect(localRect,localFrameRect,intersectRect) == false )   return;
		
		//�g���~���O�����̈��`��w��
		ALocalPoint	intersectLeftTopPoint = {intersectRect.left,intersectRect.top};
		NSRect	nsrect = {0};
		nsrect.origin = ConvertFromLocalPointToNSPoint(view,intersectLeftTopPoint);
		nsrect.size.width  = intersectRect.right - intersectRect.left;
		nsrect.size.height = intersectRect.bottom - intersectRect.top;
		[view setNeedsDisplayInRect:nsrect];
		//Carbon�łł�HIViewRender()�����s���Ă������A
		//Cocoa��displayIfNeeded�����s����ƁA���ɒx���Ȃ�̂ŁA���s���Ȃ����Ƃɂ���B
		//�i�`��^�C�~���O���ς�����̂ŁAScroll�O��display���s����Ȃǂ̑΍�������ɍs�����j
		//[view displayIfNeeded];
		
		//fprintf(stderr,"RefreshControlRect():%d(%d,%d,%d,%d) \n",inControlID,inLocalRect.left,
		//	inLocalRect.top,inLocalRect.right,inLocalRect.bottom);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		ControlRef	controlRef = GetControlRef(inControlID);//#688
		
		OSStatus	err = noErr;
		
		/* HIRect	rect;
		rect.origin.x = inLocalRect.left;
		rect.origin.y = inLocalRect.top;
		rect.size.width = inLocalRect.right - inLocalRect.left;
		rect.size.height = inLocalRect.bottom - inLocalRect.top;
		::HIViewSetNeedsDisplayInRect(mControlRef,&rect,true);
		::HIViewRender(mControlRef);*/
		
		ALocalRect	localFrameRect = {0,0,0,0};
		localFrameRect.right	= GetControlWidth(inControlID);
		localFrameRect.bottom	= GetControlHeight(inControlID);
		//R0108������ �t���[���T�C�Y�g���~���O�̌��ʁAlocalRect.top > localRect.bottom �ƂȂ�ƁADoDraw����UpdateRegion����ŁA�S�͈͑ΏۂɂȂ��Ă��܂��H�̂�
		//RefreshRect���t���[���O�̂Ƃ��͉������Ȃ��悤�ɂ���
		if( inLocalRect.bottom < localFrameRect.top )   return;
		if( inLocalRect.top > localFrameRect.bottom )   return;
		if( inLocalRect.right < localFrameRect.left )   return;
		if( inLocalRect.left > localFrameRect.right )   return;
		//OSX10.3�ł�HIViewSetNeedsDisplayInRect���g���Ȃ��̂ŁA���L�ɕύX
		//�t���[���T�C�Y�Ńg���~���O���Ă����Ȃ���HIViewSetNeedsDisplayInRegion�̂ق��͂��܂������Ȃ����Ƃ�����
		ALocalRect	localRect = inLocalRect;
		if( localRect.top < localFrameRect.top )   localRect.top = localFrameRect.top;
		if( localRect.bottom > localFrameRect.bottom )   localRect.bottom = localFrameRect.bottom;
		if( localRect.left < localFrameRect.left )   localRect.left = localFrameRect.left;
		if( localRect.right > localFrameRect.right )   localRect.right = localFrameRect.right;
		Rect	rect;
		rect.left	= localRect.left;
		rect.right	= localRect.right;
		rect.top	= localRect.top;
		rect.bottom	= localRect.bottom;
		RgnHandle	rgn = ::NewRgn();
		::RectRgn(rgn,&rect);
		err = ::HIViewSetNeedsDisplayInRegion(controlRef,rgn,true);
		if( err != noErr )   _ACErrorNum("HIViewSetNeedsDisplayInRegion() returned error: ",err,this);
		::DisposeRgn(rgn);
		err = ::HIViewRender(controlRef);
		if( err != noErr )   _ACErrorNum("HIViewRender() returned error: ",err,this);
	}
#endif
}

//#688
//CUserPane����ړ�
/**
�X�N���[��
*/
void	CWindowImp::ScrollControl( const AControlID inControlID, const ANumber inDeltaX, const ANumber inDeltaY )
{
	if( inDeltaX == 0 && inDeltaY == 0 )   return;
	
	//
	ANumber	deltaX = inDeltaX;
	ANumber	deltaY = inDeltaY;
	
	//�c�������[�h�Ȃ�X�N���[�������ϊ� #558
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		ANumber	swap = deltaX;
		deltaX = -deltaY;
		deltaY = swap;
	}
	
	//fprintf(stderr,"%16X ScrollControl() - Start\n",(int)(GetObjectID().val));
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�������ۂ��̂��l�������X�N���[�����s�����ǂ����̃t���O #947
		ABool	adjustForRounded = true;
		if( IsUserPane(inControlID) == true )
		{
			adjustForRounded = GetUserPaneConst(inControlID).GetAdjustScrollForRoundedCorner();
		}
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		//������X�N���[���̂Ƃ��́A�X�N���[���͈͂�4pixel�k�߂�B(#947 �������ۂ��Ȃ��Ă���̂ŁB)
		NSRect	scrollbounds = [view bounds];
		if( adjustForRounded == true )
		{
			if( deltaY < 0 )
			{
				scrollbounds.size.height -= 4;
			}
		}
		//�X�N���[�����s
		NSSize	delta = NSMakeSize(deltaX,deltaY);
		[view scrollRect:scrollbounds by:delta];
		//�������ۂ��Ȃ��Ă���̂��l�����ĕ`��X�V�͈͂�ǉ� (#947)
		if( adjustForRounded == true )
		{
			if( deltaY < 0 )
			{
				//������X�N���[���̎��́A�X�V�͈͂�4pixel�ǉ�����B
				NSRect	r = [view bounds];
				r.origin.y = r.size.height + deltaY - 4;
				r.size.height = 4;
				[view setNeedsDisplayInRect:r];
			}
			else
			{
				//�������X�N���[���̎��́A��ԉ���4pixel���X�V�͈͂ɒǉ�����B
				NSRect	r = [view bounds];
				r.origin.y = r.size.height - 4;
				r.size.height = 4;
				[view setNeedsDisplayInRect:r];
				//�㕔���Ɖ������𓯎��Ƀ}�[�L���O������Ԃ�displayIfNeeded����ƑS�̂��\���X�V�����̂ŁA�����ł܂��\���X�V����B
				[CocoaObjects->GetWindow() displayIfNeeded];
			}
		}
		
		//�ĕ`��w��
		NSRect	bounds = [view bounds];
		//�ĕ`��̈��L���^�ɂȂ邪�A�����̗̈��D�悵�č����rect1�Ƃ��A�c��̏c����rect2�Ƃ���
		NSRect	rect1 = bounds;
		NSRect	rect2 = bounds;
		//Y�X�N���[�������ꍇ
		if( deltaY != 0 )
		{
			if( deltaY < 0 )
			{
				//�������X�N���[���̏ꍇ�i�㕔�����ĕ`��ɂȂ�j
				//rect1�͏㕔���̉����̈�
				rect1.origin.y = bounds.origin.y + bounds.size.height + deltaY;
				rect1.size.height = -deltaY;
				//rect2�̍ĕ`��̈悩��rect1�ŕ`�悵����������
				rect2.size.height += deltaY;
			}
			else
			{
				//������X�N���[���̏ꍇ�i���������ĕ`��ɂȂ�j
				rect1.size.height = deltaY;
				//rect2�̍ĕ`��̈悩��rect1�ŕ`�悵����������
				rect2.origin.y = bounds.origin.y + deltaY;
				rect2.size.height -= deltaY;
			}
			[view setNeedsDisplayInRect:rect1];
		}
		//X�X�N���[�������ꍇ
		if( deltaX != 0 )
		{
			if( deltaX < 0 )
			{
				//�������X�N���[���̏ꍇ�i�E�������ĕ`��ɂȂ�j
				rect2.origin.x = bounds.origin.x + bounds.size.width + deltaX;
				rect2.size.width = -deltaX;
			}
			else
			{
				//�E�����X�N���[���̏ꍇ�i���������ĕ`��ɂȂ�j
				rect2.size.width = deltaX;
			}
			[view setNeedsDisplayInRect:rect2];
		}
		//EVT_DoDraw()�����s�i�o�b�t�@�ւ̕`��j
		//������draw���Ȃ��ƁAscroll�̌�A�`��C�x���g�����O�ɍēxscroll���������Ƃ��ɁA
		//����`�悷�ׂ��̈悪�`�悳���O�ɁAscroll����Ă��܂��A�`�悳��Ȃ��B
		//�폜�i���ꂪ�����diff info�E�C���h�E�ƃX�N���[������������Ɠ������Ȃ��̂ŁBNVI_Scroll()�ōŏ���ExecuteDoDrawImmediately()���R�[�����Ă���̂ŁA������displayIfNeeded���R�[�������B�j [view displayIfNeeded];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		ControlRef	controlRef = GetControlRef(inControlID);//#688
		
		OSStatus	err = noErr;
		
		err = ::HIViewScrollRect(controlRef,NULL,deltaX,deltaY);
		if( err != noErr )   _ACErrorNum("HIViewScrollRect() returned error: ",err,this);
	}
#endif
	
	//fprintf(stderr,"%16X ScrollControl() - End\n",(int)(GetObjectID().val));
}

//#688
/**
progress indicator�̒l�ݒ�
*/
void	CWindowImp::SetProgressIndicatorProgress( const AIndex inProgressIndex, const ANumber inProgress, const ABool inShow )
{
	if( inShow == false )
	{
		//progress��\��
		[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) setHidden:YES];
	}
	else
	{
		//progress�\��
		[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) setHidden:NO];
		
		//progress�l�ݒ�
		if( inProgress < 0 )
		{
			//�}�C�i�X�̏ꍇ�́Aindeterminate�\��
			[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) setIndeterminate:YES];
			[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) setUsesThreadedAnimation:YES];
			[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) startAnimation:CocoaObjects->GetWindow()];
		}
		else
		{
			//�l�ݒ�
			[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) setIndeterminate:NO];
			[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) setDoubleValue:inProgress];
		}
	}
}

#pragma mark ===========================

#pragma mark --- �R���g���[�����Get

/**
�R���g���[�����݃`�F�b�N
*/
ABool	CWindowImp::ExistControl( const AControlID inID ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//segment control�ł̎��s������
		if( [CocoaObjects->GetWindow() isKindOfClass:[CocoaWindow class]] == YES )
		{
			CocoaWindow*	cocoaWindow = static_cast<CocoaWindow*>(CocoaObjects->GetWindow());
			if( [cocoaWindow isSegmentControl:inID] == YES )
			{
				return true;
			}
		}
		
		//View������������true
		return (CocoaObjects->FindViewByTag(inID,false) != nil);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		return (GetControlRef(inID)!=NULL);
	}
#endif
}

/**
�ʒu�擾
*/
void	CWindowImp::GetControlPosition( const AControlID inID, AWindowPoint& outPoint ) const
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//LocalPoint��(0,0)�̃|�C���g��WindowPoint�֕ϊ�
		ALocalPoint	lpt = {0,0};
		GetWindowPointFromControlLocalPoint(inID,lpt,outPoint);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		/*win 080618
		�^�u�R���g���[���̏ꍇ�AGetControlBounds()�ł́A�^�u���΂ɂȂ��Ă��܂��i�E�C���h�E���W�ɂȂ�Ȃ��j
		Rect	rect;
		::GetControlBounds(GetControlRef(inID),&rect);
		outPoint.x = rect.left;
		outPoint.y = rect.top;
		*/
		HIPoint	pt = {0,0};
		err = ::HIViewConvertPoint(&pt,GetControlRef(inID),GetWindowContentView());
		if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
		outPoint.x = pt.x;
		outPoint.y = pt.y;
	}
#endif
}

/**
���擾
*/
ANumber	CWindowImp::GetControlWidth( const AControlID inID ) const
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//view���擾
		return ([view frame].size.width);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		Rect	rect;
		::GetControlBounds(GetControlRef(inID),&rect);
		return rect.right - rect.left;
	}
#endif
}

/**
�����擾
*/
ANumber	CWindowImp::GetControlHeight( const AControlID inID ) const
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//view�����擾
		return ([view frame].size.height);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		Rect	rect;
		::GetControlBounds(GetControlRef(inID),&rect);
		return rect.bottom - rect.top;
	}
#endif
}

#pragma mark ===========================

#pragma mark --- ���W�n�ϊ�

/*

�y���W�n�ϊ��z

�E�C���h�E
+--------------+
o--------------+
|     x-----+  |
|     |     |���R���g���[��
|     +-----+  |
+--------------+

o: �E�C���h�E���W�n(AWindowPoint, AWindowRect)�̌��_
x: ���[�J�����W�n(ALocalPoint, ALocalRect)�̌��_

HIViewGetFrame�͐eHIView�ɑ΂�����W�Ȃ̂ŁA�E�C���h�E�[���[�J���ϊ��ɂ͎g���Ȃ��B
HIViewConvertRect���g�p���邪�A���[�g�r���[�̓^�C�g���o�[���܂ނ̂ŃE�C���h�E�r���[�Ƃ��Ă�NULL�ł͂Ȃ�GetWindowContentView()���g�p����B

*/

/**
�R���g���[���̃��[�J�����W����E�C���h�E���W���擾
*/
void	CWindowImp::GetWindowPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AWindowPoint& outWindowPoint ) const
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//LocalPoint����NSPoint�֕ϊ�
		NSPoint	viewPoint = ConvertFromLocalPointToNSPoint(view,inLocalPoint);
		//Content view���̍��W�ɕϊ�
		NSPoint	windowPoint = [view convertPoint:viewPoint toView:CocoaObjects->GetContentView()];
		//NSPoint����WindowPoint�֕ϊ�
		outWindowPoint = ConvertFromNSPointToWindowPoint(CocoaObjects->GetContentView(),windowPoint);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		HIPoint	pt;
		pt.x = inLocalPoint.x;
		pt.y = inLocalPoint.y;
		err = ::HIViewConvertPoint(&pt,GetControlRef(inID),GetWindowContentView());
		if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
		outWindowPoint.x = pt.x;
		outWindowPoint.y = pt.y;
	}
#endif
}

/**
�R���g���[���̃��[�J�����W����E�C���h�E���W���擾
*/
void	CWindowImp::GetWindowRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AWindowRect& outWindowRect ) const
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//����̍��W�ϊ�
		ALocalPoint	viewLeftTop = {inLocalRect.left,inLocalRect.top};
		//LocalPoint����Content view����NSPoint�֕ϊ�
		NSPoint	windowLeftTop = [view 
				convertPoint:ConvertFromLocalPointToNSPoint(view,viewLeftTop) 
				toView:CocoaObjects->GetContentView()];
		//�E��̍��W�ϊ�
		ALocalPoint	viewRightBottom = {inLocalRect.right,inLocalRect.bottom};
		//LocalPoint����Content view����NSPoint�֕ϊ�
		NSPoint	windowRightBottom = [view 
				convertPoint:ConvertFromLocalPointToNSPoint(view,viewRightBottom) 
				toView:CocoaObjects->GetContentView()];
		//NSPoint����WindowPoint�֕ϊ�
		AWindowPoint	lefttop 	= ConvertFromNSPointToWindowPoint(CocoaObjects->GetContentView(),windowLeftTop);
		AWindowPoint	rightbottom	= ConvertFromNSPointToWindowPoint(CocoaObjects->GetContentView(),windowRightBottom);
		//WindowRect�֊i�[
		outWindowRect.left		= lefttop.x;
		outWindowRect.top		= lefttop.y;
		outWindowRect.right		= rightbottom.x;
		outWindowRect.bottom	= rightbottom.y;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		HIRect	rect;
		rect.origin.x 		= inLocalRect.left;
		rect.origin.y		= inLocalRect.top;
		rect.size.width		= inLocalRect.right - inLocalRect.left;
		rect.size.height	= inLocalRect.bottom - inLocalRect.top;
		err = ::HIViewConvertRect(&rect,GetControlRef(inID),GetWindowContentView());
		if( err != noErr )   _ACErrorNum("HIViewConvertRect() returned error: ",err,this);
		outWindowRect.left		= rect.origin.x;
		outWindowRect.top		= rect.origin.y;
		outWindowRect.right		= rect.origin.x + rect.size.width;
		outWindowRect.bottom	= rect.origin.y + rect.size.height;
	}
#endif
}

/**
�E�C���h�E���W����R���g���[���̃��[�J�����W���擾
*/
void	CWindowImp::GetControlLocalPointFromWindowPoint( const AControlID inID, const AWindowPoint& inWindowPoint, ALocalPoint& outLocalPoint ) const
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//WindowPoint����NSPoint�֕ϊ�
		NSPoint	windowPoint = ConvertFromWindowPointToNSPoint(CocoaObjects->GetContentView(),inWindowPoint);
		//View���̍��W�֕ϊ�
		NSPoint	viewPoint = [view convertPoint:windowPoint fromView:CocoaObjects->GetContentView()];
		//NSPoint����LocalPoint�֕ϊ�
		outLocalPoint = ConvertFromNSPointToLocalPoint(view,viewPoint);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		HIPoint	pt;
		pt.x = inWindowPoint.x;
		pt.y = inWindowPoint.y;
		err = ::HIViewConvertPoint(&pt,GetWindowContentView(),GetControlRef(inID));
		if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
		outLocalPoint.x = pt.x;
		outLocalPoint.y = pt.y;
	}
#endif
}

/**
�E�C���h�E���W����R���g���[���̃��[�J�����W���擾
*/
void	CWindowImp::GetControlLocalRectFromWindowRect( const AControlID inID, const AWindowRect& inWindowRect, ALocalRect& outLocalRect ) const
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//����̍��W�ϊ�
		AWindowPoint	windowLeftTop = {inWindowRect.left,inWindowRect.top};
		//WindowPoint����view����NSPoint�֕ϊ�
		NSPoint	viewLeftTop = [view 
				convertPoint:ConvertFromWindowPointToNSPoint(CocoaObjects->GetContentView(),windowLeftTop) 
				fromView:CocoaObjects->GetContentView()];
		//�E��̍��W�ϊ�
		AWindowPoint	windowRightBottom = {inWindowRect.right,inWindowRect.bottom};
		//WindowPoint����view����NSPoint�֕ϊ�
		NSPoint	viewRightBottom = [view 
				convertPoint:ConvertFromWindowPointToNSPoint(CocoaObjects->GetContentView(),windowRightBottom) 
				fromView:CocoaObjects->GetContentView()];
		//NSPoint����LocalPoint�֕ϊ�
		ALocalPoint	lefttop		= ConvertFromNSPointToLocalPoint(view,viewLeftTop);
		ALocalPoint	rightbottom	= ConvertFromNSPointToLocalPoint(view,viewRightBottom);
		//LocalRect�֊i�[
		outLocalRect.left		= lefttop.x;
		outLocalRect.top		= lefttop.y;
		outLocalRect.right		= rightbottom.x;
		outLocalRect.bottom		= rightbottom.y;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		HIRect	rect;
		rect.origin.x 		= inWindowRect.left;
		rect.origin.y		= inWindowRect.top;
		rect.size.width		= inWindowRect.right - inWindowRect.left;
		rect.size.height	= inWindowRect.bottom - inWindowRect.top;
		err = ::HIViewConvertRect(&rect,GetWindowContentView(),GetControlRef(inID));
		if( err != noErr )   _ACErrorNum("HIViewConvertRect() returned error: ",err,this);
		outLocalRect.left	= rect.origin.x;
		outLocalRect.top	= rect.origin.y;
		outLocalRect.right	= rect.origin.x + rect.size.width;
		outLocalRect.bottom	= rect.origin.y + rect.size.height;
	}
#endif
}

/**
LocalPoint->GlobalPoint
*/
void	CWindowImp::GetGlobalPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AGlobalPoint& outGlobalPoint ) const
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//LocalPoint����WindowPoint�֕ϊ�
		AWindowPoint	wpt = {0};
		GetWindowPointFromControlLocalPoint(inID,inLocalPoint,wpt);
		//�E�C���h�E�̍�����W�𑫂�
		AGlobalRect	windowbounds = {0};
		GetWindowBounds(windowbounds);
		outGlobalPoint.x = windowbounds.left + wpt.x;
		outGlobalPoint.y = windowbounds.top + wpt.y;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		StSetPort	s(GetWindowRef());
		AWindowPoint	windowPoint;
		GetWindowPointFromControlLocalPoint(inID,inLocalPoint,windowPoint);
		Point	pt;
		pt.h = windowPoint.x;
		pt.v = windowPoint.y;
		::LocalToGlobal(&pt);
		outGlobalPoint.x = pt.h;
		outGlobalPoint.y = pt.v;
	}
#endif
}

/**
LocalRect->GlobalRect
*/
void	CWindowImp::GetGlobalRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AGlobalRect& outGlobalRect ) const
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//LocalRect����WindowRect�֕ϊ�
		AWindowRect	wrect = {0};
		GetWindowRectFromControlLocalRect(inID,inLocalRect,wrect);
		//�E�C���h�E�̍�����W�𑫂�
		AGlobalRect	windowbounds = {0};
		GetWindowBounds(windowbounds);
		outGlobalRect.left		= windowbounds.left + wrect.left;
		outGlobalRect.top		= windowbounds.top + wrect.top;
		outGlobalRect.right		= windowbounds.left + wrect.right;
		outGlobalRect.bottom	= windowbounds.top + wrect.bottom;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		StSetPort	s(GetWindowRef());
		AWindowRect	windowRect;
		GetWindowRectFromControlLocalRect(inID,inLocalRect,windowRect);
		Point	pt;
		pt.h = windowRect.left;
		pt.v = windowRect.top;
		::LocalToGlobal(&pt);
		outGlobalRect.left 		= pt.h;
		outGlobalRect.top 		= pt.v;
		
		pt.h = windowRect.right;
		pt.v = windowRect.bottom;
		::LocalToGlobal(&pt);
		outGlobalRect.right		= pt.h;
		outGlobalRect.bottom	= pt.v;
	}
#endif
}

//win 080618
/**
�w��Control���܂܂��TabControl��Index���擾����
*/
AIndex	CWindowImp::GetControlEmbeddedTabIndex( const AControlID inID ) const
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//�����Ȃ�
		return kIndex_Invalid;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		AControlRef	tabViewRootControlRef = GetControlRef('ttab',0);
		if( tabViewRootControlRef == NULL )   return kIndex_Invalid;
		
		ANumber	tabMax = ::GetControl32BitMaximum(tabViewRootControlRef);
		AIndex	result = kIndex_Invalid;
		for( AIndex i = 0; i < tabMax; i++ )
		{
			if( IsEmbededInTabView(inID,i) == true )
			{
				result = i;
				break;
			}
		}
		return result;
	}
#endif
}

#if SUPPORT_CARBON
//#688
/**
Carbon Event(kEventParamWindowMouseLocation)�Ŏ擾����Point����ControlLocalPoint�֕ϊ�
*/
void	CWindowImp::GetControlLocalPointFromEventParamWindowMouseLocation( 
		const AControlID inID, const HIPoint& inPoint, ALocalPoint& outControlPoint ) const
{
	HIPoint	pt = inPoint;
	OSErr	err = ::HIViewConvertPoint(&pt,NULL,GetControlRef(inID));//��������NULL
	if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
	outControlPoint.x = pt.x;
	outControlPoint.y = pt.y;
}
#endif

#pragma mark ===========================

#pragma mark --- �R���g���[���l��Set/Get�C���^�[�t�F�C�X

/**
Text�擾
*/
void	CWindowImp::GetText( const AControlID inID, AText& outText ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPaneConst(inID).GetText(outText);
		return;
	}
	
	//#688 Cocoa�Ή�
	outText.DeleteAll();
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//�l�擾
		//NSPopUpButton�̏ꍇ�A�I�𒆂̍��ڂ̃e�L�X�g���擾
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			
			//text array menu�̏ꍇ�́A�I��index�����ɁAtext array menu manager����action text���擾����
			AIndex	textArrayMenuArrayIndex = mTextArrayMenuArray_ControlID.Find(inID);
			if( textArrayMenuArrayIndex != kIndex_Invalid )
			{
				//�I��index���擾
				AIndex	selItemIndex = [popup indexOfSelectedItem];
				if( selItemIndex != -1 )
				{
					//pull down �̏ꍇ�́A�ŏ���1�Ȃ̂�-1����
					if( [popup pullsDown] == YES )
					{
						selItemIndex--;
					}
					//text array menu manager����action text���擾
					AApplication::GetApplication().NVI_GetTextArrayMenuManager().
							GetActionTextFromItemIndex(GetAWinConst().GetObjectID(),inID,selItemIndex,outText);
				}
			}
			else
			{
				//���j���[�̌��݂̑I���ʒu�̃e�L�X�g���擾
				ACocoa::SetTextFromNSString([popup titleOfSelectedItem],outText);
			}
		}
		//���{�^�������j���[�ɑΉ�����ꍇ�͑Ή��K�v
		//NSControl�̏ꍇ�AstringValue���擾
		else if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			ACocoa::SetTextFromNSString([control stringValue],outText);
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[���̏ꍇ
		OSStatus	err = noErr;
		
		AControlRef	controlRef = GetControlRef(inID);
		ControlKind	kind;
		err = ::GetControlKind(controlRef,&kind);
		if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
		//�|�b�v�A�b�v�{�^���̏ꍇ�͌��ݑI�𒆂̃��j���[���ڕ������Ԃ�
		if( kind.kind == kControlKindPopupButton )
		{
			//#375
			AIndex	valueIndependentMenuIndex = mValueIndependentMenuArray_ControlID.Find(inID);
			if( valueIndependentMenuIndex != kIndex_Invalid )
			{
				mValueIndependentMenuArray_ValueArray.GetObjectConst(valueIndependentMenuIndex).
						Get(::GetControl32BitValue(controlRef)-1,outText);
				return;
			}
			//
			ACarbonMenuWrapper::GetMenuItemText(GetMenuRef(inID),::GetControl32BitValue(controlRef)-1,outText);
			return;
		}
		//BevelButton�����j���[�����݂���ꍇ
		if( kind.kind == kControlKindBevelButton && GetMenuRef(inID) != NULL )
		{
			//xcode3 080804 SInt16	value;
			//SInt16	value;
			MenuItemIndex	value;
			err = ::GetBevelButtonMenuValue(controlRef,&value);
			if( err != noErr )   _ACErrorNum("GetBevelButtonMenuValue() returned error: ",err,this);
			ACarbonMenuWrapper::GetMenuItemText(GetMenuRef(inID),value-1,outText);
			return;
		}
		//���̑��R���g���[���̏ꍇGetControlData()�Ŏ擾
		outText.DeleteAll();
		CFStringRef	strref = NULL;
		Size	actSize = 0;
		err = ::GetControlData(controlRef,kControlEntireControl,kControlEditTextCFStringTag,sizeof(strref),&strref,&actSize);
		//#0 �{�^�����̏ꍇ�G���[�ɂȂ�i���̏ꍇ�󔒃e�L�X�g�ŕԂ����Ƃɂ���j if( err != noErr )   _ACErrorNum("GetControlData() returned error: ",err,this);
		if( strref != NULL )
		{
			outText.SetFromCFString(strref);
			::CFRelease(strref);
		}
	}
#endif
}

/**
Text�ݒ�
*/
void	CWindowImp::SetText( const AControlID inID, const AText& inText, const ABool inOnlyText )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetText(inText);
		return;
	}
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSString�擾
		AStCreateNSStringFromAText	str(inText);
		//�l�ݒ�
		//NSPopUpButton�̏ꍇ�A�e�L�X�g�ɑΉ����鍀�ڂ�ݒ�
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			
			if( [popup pullsDown] == YES )
			{
				//==================pull down���j���[�̏ꍇ==================
				AIndex	textArrayMenuArrayIndex = mTextArrayMenuArray_ControlID.Find(inID);
				if( textArrayMenuArrayIndex != kIndex_Invalid )
				{
					//==================text array menu�̏ꍇ==================
					//text array menu manager����A�w��e�L�X�g�ɑΉ����鍀��index������
					AIndex	itemIndex = AApplication::GetApplication().NVI_GetTextArrayMenuManager().GetItemIndexFromActionText(GetAWin().GetObjectID(),inID,inText);
					//������Ȃ���΍ŏ���I��
					if( itemIndex == kIndex_Invalid )   itemIndex = 0;
					
					//����index�ɑΉ����郁�j���[�\���e�L�X�g���擾
					AText	title;
					AApplication::GetApplication().NVI_GetTextArrayMenuManager().GetMenuTextFromItemIndex(GetAWin().GetObjectID(),inID,itemIndex,title);
					//�ŏ��̃��j���[���ځi����ɕ\�������e�L�X�g�j�̃e�L�X�g��ݒ�
					AMenuWrapper::SetMenuItemText([popup menu],0,title);
					
					//�w��ԍ��̍��ڂ�I��
					[popup selectItemAtIndex:itemIndex+1];
					
					//�e�L�X�g�ɑΉ����郁�j���[���ڂɃ`�F�b�N�}�[�N��ݒ�
					AItemCount	itemCount = AMenuWrapper::GetMenuItemCount([popup menu]);
					for( AIndex i = 1; i < itemCount; i++ )
					{
						ABool	check = false;
						if( i-1 == itemIndex )
						{
							check = true;
						}
						AMenuWrapper::SetCheckMark([popup menu],i,check);
					}
					
				}
				else
				{
					//==================text array menu�ł͂Ȃ��ꍇ==================
					//�ŏ��̃��j���[���ځi����ɕ\�������e�L�X�g�j�̃e�L�X�g��ݒ�
					AMenuWrapper::SetMenuItemText([popup menu],0,inText);
					
					//�e�L�X�g�ɑΉ����郁�j���[���ڂɃ`�F�b�N�}�[�N��ݒ�
					AItemCount	itemCount = AMenuWrapper::GetMenuItemCount([popup menu]);
					for( AIndex i = 1; i < itemCount; i++ )
					{
						AText	text;
						AMenuWrapper::GetMenuItemText([popup menu],i,text);
						ABool	check = false;
						if( inText.Compare(text) == true )
						{
							check = true;
							//�w��ԍ��̍��ڂ�I��
							[popup selectItemAtIndex:i];
						}
						AMenuWrapper::SetCheckMark([popup menu],i,check);
					}
				}
			}
			else
			{
				//==================pull down�ł͂Ȃ����j���[�̏ꍇ==================
				AIndex	textArrayMenuArrayIndex = mTextArrayMenuArray_ControlID.Find(inID);
				if( textArrayMenuArrayIndex != kIndex_Invalid )
				{
					//==================text array menu�̏ꍇ==================
					//text array menu����A�w��e�L�X�g�ɑΉ����鍀��index���擾
					AIndex	itemIndex = AApplication::GetApplication().NVI_GetTextArrayMenuManager().GetItemIndexFromActionText(GetAWin().GetObjectID(),inID,inText);
					//������Ȃ���΍ŏ���I��
					if( itemIndex == kIndex_Invalid )   itemIndex = 0;
					//���j���[���ڑI��
					[popup selectItemAtIndex:itemIndex];
				}
				else
				{
					//==================text array menu�ł͂Ȃ��ꍇ==================
					//�e�L�X�g�ɑΉ����郁�j���[���ڂ�index���擾
					NSInteger	itemIndex = [popup indexOfItemWithTitle:str.GetNSString()];
					//������Ȃ���΍ŏ���I��
					if( itemIndex == -1 )   itemIndex = 0;
					//���j���[���ڑI��
					[popup selectItemAtIndex:itemIndex];
				}
			}
		}
		//NSButton�̏ꍇ�Atitle���擾
		else if( [view isKindOfClass:[NSButton class]] == YES )
		{
			NSButton*	button = ACocoa::CastNSButtonFromId(view);
			[button setTitle:str.GetNSString()];
		}
		//���{�^�������j���[�ɑΉ�����ꍇ�͑Ή��K�v
		//NSControl�̏ꍇ�AstringValue��ݒ�
		else if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			[control setStringValue:str.GetNSString()];
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[���̏ꍇ
		OSStatus	err = noErr;
		
		AControlRef	controlRef = GetControlRef(inID);
		ControlKind	kind;
		err = ::GetControlKind(controlRef,&kind);
		if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
		//�|�b�v�A�b�v�{�^���̏ꍇ��inText�ɑΉ����郁�j���[���ڂ�I������
		if( kind.kind == kControlKindPopupButton )
		{
			//#375
			AIndex	valueIndependentMenuIndex = mValueIndependentMenuArray_ControlID.Find(inID);
			if( valueIndependentMenuIndex != kIndex_Invalid )
			{
				AIndex	itemIndex = mValueIndependentMenuArray_ValueArray.GetObjectConst(valueIndependentMenuIndex).
						Find(inText);
				if( itemIndex == kIndex_Invalid )   itemIndex = 0;
				::SetControl32BitValue(controlRef,itemIndex+1);
				return;
			}
			//
			AIndex	itemIndex = ACarbonMenuWrapper::GetMenuItemIndexByText(GetMenuRef(inID),inText);
			if( itemIndex == kIndex_Invalid )   itemIndex = 0;
			::SetControl32BitValue(controlRef,itemIndex+1);
			return;
		}
		//BevelButton�����j���[�����݂���ꍇ
		if( kind.kind == kControlKindBevelButton && GetMenuRef(inID) != NULL )
		{
			if( inOnlyText == false )
			{
				AIndex	itemIndex = ACarbonMenuWrapper::GetMenuItemIndexByText(GetMenuRef(inID),inText);
				if( itemIndex == kIndex_Invalid )   itemIndex = 0;
				err = ::SetBevelButtonMenuValue(controlRef,itemIndex+1);
				if( err != noErr )   _ACErrorNum("SetBevelButtonMenuValue() returned error: ",err,this);
			}
			
			AStCreateCFStringFromAText	strref(inText);
			err = ::SetControlTitleWithCFString(controlRef,strref.GetRef());
			if( err != noErr )   _ACErrorNum("SetControlTitleWithCFString() returned error: ",err,this);
			
			/*AMenuRef	menuRef = GetMenuRef(inID);
			ACarbonMenuWrapper::ClearAllCheckMark(menuRef);
			AIndex	itemIndex = ACarbonMenuWrapper::GetMenuItemIndexByText(menuRef,inText);
			if( itemIndex == kIndex_Invalid )   return;
			ACarbonMenuWrapper::SetCheckMark(menuRef,itemIndex,true);*/
			return;
		}
		//���̑��R���g���[���̏ꍇ
		AStCreateCFStringFromAText	strref(inText);
		if( kind.kind == kControlKindPushButton || kind.kind == kControlKindRadioButton || kind.kind == kControlKindCheckBox || kind.kind == kControlKindBevelButton )
		{
			err = ::SetControlTitleWithCFString(controlRef,strref.GetRef());
			if( err != noErr )   _ACErrorNum("SetControlTitleWithCFString() returned error: ",err,this);
		}
		else
		{
			CFStringRef	ref = strref.GetRef();
			err = ::SetControlData(controlRef,kControlEntireControl,kControlEditTextCFStringTag,sizeof(CFStringRef),&ref);
			if( err != noErr )   _ACErrorNum("SetControlData() returned error: ",err,this);
			::Draw1Control(controlRef);
		}
		//::Draw1Control(controlRef);
	}
#endif
}

/**
Text�ǉ�
*/
void	CWindowImp::InputText( const AControlID inID, const AText& inText )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).InputText(inText);
		return;
	}
	//�ʏ�R���g���[���̏ꍇ
	//������
	_ACError("not implemented",NULL);
}

/**
Text�ǉ�
*/
void	CWindowImp::AddText( const AControlID inID, const AText& inText )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).AddText(inText);
		return;
	}
	//�ʏ�R���g���[���̏ꍇ
	//������
	_ACError("not implemented",NULL);
}

/**
Bool�擾
*/
void	CWindowImp::GetBool( const AControlID inID, ABool& outBool ) const
{
	outBool = GetBool(inID);
}

/**
Bool�擾
*/
ABool	CWindowImp::GetBool( const AControlID inID ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		ABool	b;
		GetUserPaneConst(inID).GetBool(b);
		return b;
	}
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//�l�擾
		//NSPopUpButton�̏ꍇ�A0�Ԗڂ̍��ڑI�𒆂Ȃ�false�A1�Ԗڂ̍��ڑI�𒆂Ȃ�true��Ԃ�
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			return ([popup indexOfSelectedItem]==1);
		}
		//NSButton�̏ꍇ�Astate���擾
		else if( [view isKindOfClass:[NSButton class]] == YES )
		{
			NSButton*	button = ACocoa::CastNSButtonFromId(view);
			//state�擾
			if( [button state] == NSOnState )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
		}
		return false;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[���̏ꍇ
		AControlRef	controlRef = GetControlRef(inID);
		ControlKind	kind;
		::GetControlKind(controlRef,&kind);
		//�|�b�v�A�b�v�{�^���̏ꍇ��1�Ԗڂ̍��ڑI�𒆂Ȃ�false�A2�Ԗڂ̍��ڑI�𒆂Ȃ�true��Ԃ�
		if( kind.kind == kControlKindPopupButton )
		{
			if( ::GetControl32BitValue(controlRef) == 2 )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		//���̑��R���g���[���̏ꍇ
		return ::GetControl32BitValue(controlRef);
	}
#endif
}

/**
Bool�ݒ�
*/
void	CWindowImp::SetBool( const AControlID inID, const ABool inBool )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	/*#349
	//true�ɐݒ肷��ꍇ��UpdateRadioGroup()���R�[�����邱�Ƃɂ��A���ꂪ���W�I�O���[�v���������ꍇ�́A���̃R���g���[����false�ɂ���
	if( inBool == true )
	{
		UpdateRadioGroup(inID);
	}
	*/
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetBool(inBool);
		return;
	}
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//segmented control�̏ꍇ�̏���
		if( [CocoaObjects->GetWindow() isKindOfClass:[CocoaWindow class]] == YES )
		{
			CocoaWindow*	cocoaWindow = static_cast<CocoaWindow*>(CocoaObjects->GetWindow());
			BOOL	sel = NO;
			if( inBool == true )
			{
				sel = YES;
			}
			if( [cocoaWindow setBoolForSegmentControl:inID select:sel] == YES )
			{
				return;
			}
		}
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//�l�ݒ�
		//NSPopUpButton�̏ꍇ�Afalse�Ȃ�0�ԖځAtrue�Ȃ�1�Ԗڂ̍��ڂ�I������
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			if( inBool == true )
			{
				[popup selectItemAtIndex:1];
			}
			else
			{
				[popup selectItemAtIndex:0];
			}
		}
		//NSButton�̏ꍇ�Astate��ݒ�
		else if( [view isKindOfClass:[NSButton class]] == YES )
		{
			NSButton*	button = ACocoa::CastNSButtonFromId(view);
			if( inBool == true )
			{
				[button setState:NSOnState];
			}
			else
			{
				[button setState:NSOffState];
			}
			//CocoaDisclosureButton�̏ꍇ�́A���肽���ݎ��s
			if( [view isKindOfClass:[CocoaDisclosureButton class]] == YES )
			{
				CocoaDisclosureButton*	disclosureButton =  ACocoa::CastNSButtonFromId(view);
				[view updateCollapsableView];
			}
		}
		//Progress indicator�̏ꍇ�A�A�j���[�V�����J�n�^��~
		else if( [view isKindOfClass:[CocoaProgressIndicator class]] == YES )
		{
			CocoaProgressIndicator*	progress = static_cast<CocoaProgressIndicator*>(view);
			if( inBool == true )
			{
				[progress startAnimation:CocoaObjects->GetWindow()];
			}
			else
			{
				[progress stopAnimation:CocoaObjects->GetWindow()];
			}
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[���̏ꍇ
		OSStatus	err = noErr;
		
		AControlRef	controlRef = GetControlRef(inID);
		ControlKind	kind;
		err = ::GetControlKind(controlRef,&kind);
		if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
		//�|�b�v�A�b�v�{�^���̏ꍇ��false�Ȃ�1�ԖځAtrue�Ȃ�2�Ԗڂ̍��ڂ�I������
		if( kind.kind == kControlKindPopupButton )
		{
			if( inBool == true )
			{
				::SetControl32BitValue(controlRef,2);
			}
			else
			{
				::SetControl32BitValue(controlRef,1);
			}
			return;
		}
		//���̑��R���g���[���̏ꍇ
		::SetControl32BitValue(controlRef,inBool);
	}
#endif
}

/**
Color�擾
*/
void	CWindowImp::GetColor( const AControlID inID, AColor& outColor ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPaneConst(inID).GetColor(outColor);
		return;
	}
	
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//�l�ݒ�
	//ColorWell�̏ꍇ
	if( [view isKindOfClass:[NSColorWell class]] == YES )
	{
		NSColorWell*	colorwell = static_cast<NSColorWell*>(view);
		//
		NSColor*	color = [[colorwell color] colorUsingColorSpaceName:NSCalibratedRGBColorSpace];//#945
		outColor.red	= 65535 * [color redComponent];
		outColor.green	= 65535 * [color greenComponent];
		outColor.blue	= 65535 * [color blueComponent];
	}
	//��L�ȊO�̓G���[
	else
	{
		_ACError("",NULL);
	}
}

/**
Color�ݒ�
*/
void	CWindowImp::SetColor( const AControlID inID, const AColor& inColor )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetColor(inColor);
		return;
	}
	
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//�l�ݒ�
	//ColorWell�̏ꍇ
	if( [view isKindOfClass:[NSColorWell class]] == YES )
	{
		NSColorWell*	colorwell = static_cast<NSColorWell*>(view);
		//
		CGFloat	red = inColor.red;
		red /= 65535;
		CGFloat	green = inColor.green;
		green /= 65535;
		CGFloat	blue = inColor.blue;
		blue /= 65535;
		[colorwell setColor:[NSColor colorWithCalibratedRed:red green:green blue:blue alpha:1.0]];
	}
	//��L�ȊO�̓G���[
	else
	{
		_ACError("",NULL);
	}
}

/**
Number�擾
*/
void	CWindowImp::GetNumber( const AControlID inID, ANumber& outNumber ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
#if SUPPORT_CARBON
//#205 #if USE_IMP_TABLEVIEW
	//TebleView�̏ꍇ�͑I�𒆂̍s�̃C���f�b�N�X��Ԃ�
	if( IsTableView(inID) == true )
	{
		outNumber = GetTableViewConstByID(inID).GetSelect();
		return;
	}
//#205 #endif
#endif
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPaneConst(inID).GetNumber(outNumber);
		return;
	}
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//�l�擾
		//�|�b�v�A�b�v�{�^���̏ꍇ
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			//�I��index���擾
			outNumber = [popup indexOfSelectedItem];
			//pull down�̏ꍇ�́A���j���[��1�n�܂�Ȃ̂Ł|�P����
			if( [popup pullsDown] == YES )
			{
				outNumber--;
			}
		}
		//NSScroller�̏ꍇ
		else if( [view isKindOfClass:[NSScroller class]] == YES )
		{
			outNumber = 0;
			NSScroller*	scroller = static_cast<NSScroller*>(view);
			//�l���擾�iscroller�̒l��0-0-1.0�̒l�ɂȂ��Ă���̂ŁA���ۂ̒l�ɕϊ��j
			AIndex	index = mMinMaxArray_ControlID.Find(inID);
			if( index != kIndex_Invalid )
			{
				AFloatNumber	scrollerValue = [scroller floatValue];
				ANumber	min = mMinMaxArray_Min.Get(index);
				ANumber	max = mMinMaxArray_Max.Get(index);
				if( min < max )
				{
					AFloatNumber	value = max-min;
					value *= scrollerValue;
					value += min;
					outNumber = value;
				}
			}
		}
		//���{�^�������j���[�ɑΉ�����ꍇ�͑Ή��K�v
		//�R���g���[���̏ꍇ
		else if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			//intValue���擾
			outNumber = [control intValue];
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
			outNumber = 0;
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		//�ʏ�R���g���[���̏ꍇ
		AControlRef	controlRef = GetControlRef(inID);
		ControlKind	kind;
		err = ::GetControlKind(controlRef,&kind);
		if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
		//���W�I�{�^���O���[�v�A�|�b�v�A�b�v���j���[�{�^���̏ꍇ�͑I�𒆂̍��ڔԍ���0�`�̐��ɕϊ����ĕԂ�
		if( kind.kind == kControlKindRadioGroup || kind.kind == kControlKindPopupButton )
		{
			outNumber = ::GetControl32BitValue(controlRef)-1;
			return;
		}
		//BevelButton�����j���[�����݂���ꍇ
		if( kind.kind == kControlKindBevelButton && GetMenuRef(inID) != NULL )
		{
			//xcode3 080804 SInt16	value;
			MenuItemIndex	value;
			//SInt16	value;
			err = ::GetBevelButtonMenuValue(controlRef,&value);
			if( err != noErr )   _ACErrorNum("GetBevelButtonMenuValue() returned error: ",err,this);
			outNumber = value-1;
			return;
		}
		//���̑��R���g���[���̏ꍇ
		outNumber = ::GetControl32BitValue(controlRef);
	}
#endif
}

/**
Number�擾
*/
ANumber	CWindowImp::GetNumber( const AControlID inID ) const
{
	ANumber	number;
	GetNumber(inID,number);
	return number;
}

/**
FloatNumber�擾
*/
void	CWindowImp::GetFloatNumber( const AControlID inID, AFloatNumber& outNumber ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPaneConst(inID).GetFloatNumber(outNumber);
		return;
	}
	
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//�l�擾
	if( [view isKindOfClass:[NSControl class]] == YES )
	{
		NSControl*	control = ACocoa::CastNSControlFromId(view);
		//floatValue���擾
		outNumber = [control floatValue];
	}
	//��L�ȊO�̓G���[
	else
	{
		_ACError("",NULL);
		outNumber = 0;
	}
}

/**
Number�ݒ�
*/
void	CWindowImp::SetNumber( const AControlID inID, const ANumber inNumber )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
#if SUPPORT_CARBON
//#205 #if USE_IMP_TABLEVIEW
	//TebleView�̏ꍇ�͍s��I��
	if( IsTableView(inID) == true )
	{
		GetTableViewByID(inID).SetSelect(inNumber);
		return;
	}
//#205 #endif
#endif
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetNumber(inNumber);
		return;
	}
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//�l�ݒ�
		//NSPopUpButton�̏ꍇ
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			//inNumber�Ԗڂ̃��j���[���ڂ�I��
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			if( [popup pullsDown] == YES )
			{
				//==================pull down���j���[�̏ꍇ==================
				//pull down���j���[�̏ꍇ�́A�w��ԍ��̃e�L�X�g���擾���āA�\���^�C�g���i=index:0�j�ɐݒ�
				AText	text;
				if( inNumber >= 0 )
				{
					AMenuWrapper::GetMenuItemText([popup menu],inNumber+1,text);
					AMenuWrapper::SetMenuItemText([popup menu],0,text);
					//�w��ԍ��̍��ڂ�I��
					[popup selectItemAtIndex:inNumber+1];
				}
				//�w��ԍ��̍��ڂɃ`�F�b�N�}�[�N��t����
				AItemCount	itemCount = AMenuWrapper::GetMenuItemCount([popup menu]);
				for( AIndex i = 1; i < itemCount; i++ )
				{
					ABool	check = false;
					if( i == inNumber+1 )
					{
						check = true;
					}
					AMenuWrapper::SetCheckMark([popup menu],i,check);
				}
			}
			else
			{
				//==================pull down���j���[�ł͂Ȃ��ꍇ==================
				//�w��ԍ��̍��ڂ�I��
				[popup selectItemAtIndex:inNumber];
			}
		}
		//NSScroller�̏ꍇ
		else if( [view isKindOfClass:[NSScroller class]] == YES )
		{
			//scroller�̍ŏ��^�ő�l���擾���A0.0-1.0�̒l�ɕϊ�����
			AIndex	index = mMinMaxArray_ControlID.Find(inID);
			if( index != kIndex_Invalid )
			{
				ANumber	min = mMinMaxArray_Min.Get(index);
				ANumber	max = mMinMaxArray_Max.Get(index);
				if( min < max )
				{
					//0.0-1.0�ɐ��K�����Đݒ�
					AFloatNumber	num = inNumber;
					num = num / (max-min);
					NSScroller*	scroller = ACocoa::CastNSScrollerFromId(view);
					[scroller setDoubleValue:num];
				}
				else
				{
					//min>=max�Ȃ̂ŁAscroller��SetMinMax()��disable��Ԃɂ���Ă���B��������
				}
			}
			else
			{
				_ACError("",NULL);
			}
		}
		//���{�^�������j���[�ɑΉ�����ꍇ�͑Ή��K�v
		//NSControl�̏ꍇ
		else if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			//#1225 [control setIntValue:inNumber];
			AText	numberText;
			numberText.SetNumber(inNumber);
			AStCreateNSStringFromAText	str(numberText);
			[control setStringValue:str.GetNSString()];
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[���̏ꍇ
		OSStatus	err = noErr;
		
		AControlRef	controlRef = GetControlRef(inID);
		ControlKind	kind;
		err = ::GetControlKind(controlRef,&kind);
		if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
		//�v���O���X�o�[�̏ꍇ�͐i���l�Ƃ��Đݒ�A�������A�}�C�i�X�̏ꍇ��interminate�Ƃ��Đݒ�
		if( kind.kind == kControlKindProgressBar )
		{
			Boolean	indeterminate;
			if( inNumber >= 0 )
			{
				::SetControl32BitValue(controlRef,inNumber);
				indeterminate = false;
			}
			else
			{
				indeterminate = true;
			}
			err = ::SetControlData(controlRef,kControlEntireControl,kControlProgressBarIndeterminateTag,sizeof(indeterminate),&indeterminate);
			if( err != noErr )   _ACErrorNum("SetControlData() returned error: ",err,this);
			return;
		}
		//���W�I�{�^���O���[�v�A�|�b�v�A�b�v���j���[�{�^���̏ꍇ�̓C���f�b�N�X��inNumber�̍��ڂ�I���i������+1�ϊ��j
		if( kind.kind == kControlKindRadioGroup || kind.kind == kControlKindPopupButton )
		{
			::SetControl32BitValue(controlRef,inNumber+1);
			return;
		}
		//BevelButton�����j���[�����݂���ꍇ
		if( kind.kind == kControlKindBevelButton && GetMenuRef(inID) != NULL )
		{
			err = ::SetBevelButtonMenuValue(controlRef,inNumber+1);
			if( err != noErr )   _ACErrorNum("SetBevelButtonMenuValue() returned error: ",err,this);
			
			if( inNumber >= 0 )
			{
				AText	text;
				GetText(inID,text);
				AStCreateCFStringFromAText	strref(text);
				err = ::SetControlTitleWithCFString(controlRef,strref.GetRef());
				if( err != noErr )   _ACErrorNum("SetControlTitleWithCFString() returned error: ",err,this);
			}
			return;
		}
		//���̑��R���g���[���̏ꍇ
		::SetControl32BitValue(controlRef,inNumber);
	}
#endif
}

/**
FloatNumber�ݒ�
*/
void	CWindowImp::SetFloatNumber( const AControlID inID, const AFloatNumber inNumber )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetFloatNumber(inNumber);
		return;
	}
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//�l�ݒ�
		//�R���g���[���̏ꍇ
		if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			[control setFloatValue:inNumber];
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
		}
	}
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[�����Ή�
		_ACError("not yet implemented",this);
	}
}

//�t�H���g�擾
/*win void	CWindowImp::GetFont( const AControlID inID, AFont& outFontNum ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	outFontNum = ACarbonMenuWrapper::GetFontByMenuItemIndex(GetMenuRef(inID),GetNumber(inID));
}*/

//�t�H���g�ݒ�
/*win void	CWindowImp::SetFont( const AControlID inID, AFont inFontNum )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	SetNumber(inID,ACarbonMenuWrapper::GetMenuItemIndexByFont(GetMenuRef(inID),inFontNum));
}*/

//#688
/**
�R���g���[���Ƀt�H���g�ݒ�
*/
void	CWindowImp::SetFont( const AControlID inID, const AText& inFontName, const AFloatNumber inFontSize )
{
	AStCreateNSStringFromAText	fontnamestr(inFontName);
	NSFont*	font = [NSFont fontWithName:fontnamestr.GetNSString() size:inFontSize];
	
	//�w��t�H���g��������Ȃ��ꍇ�̓f�t�H���g�t�H���g���擾
	if( font == nil )
	{
		AText	defaultFontName;
		AFontWrapper::GetAppDefaultFontName(defaultFontName);
		AStCreateNSStringFromAText	fontnamestr2(defaultFontName);
		font = [NSFont fontWithName:fontnamestr2.GetNSString() size:inFontSize];
		if( font == nil )
		{
			font = [NSFont systemFontOfSize:inFontSize];
		}
	}
	
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//NSControl�Ȃ�A�t�H���g�ݒ�
	if( [view isKindOfClass:[NSControl class]] == YES )
	{
		NSControl*	control = ACocoa::CastNSControlFromId(view);
		[control setFont:font];
	}
	//��L�ȊO�̓G���[
	else
	{
		_ACError("",NULL);
	}
}

//#688
/**
�R���g���[������t�H���g�擾
*/
void	CWindowImp::GetFont( const AControlID inID, AText& outFontName, AFloatNumber& outFontSize ) const
{
	//�t�H���g�p�l����virtual control id�̏ꍇ�́A�t�H���g�p�l���őI�𒆂̃t�H���g���擾
	if( inID == mFontPanelVirtualControlID )
	{
		NSFont*	font = [CocoaObjects->GetWindow() getFontPanelFont];
		ACocoa::SetTextFromNSString([font fontName],outFontName);
		outFontSize = [font pointSize];
		return;
	}
	
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//CocoaFontField�Ȃ�t�H���g�擾�i�t�H���g�T�C�Y�͕\���t�H���g�ɏ�������Ă���̂ŁA��p���\�b�h�Ŏ擾����j
	if( [view isKindOfClass:[CocoaFontField class]] == YES )
	{
		CocoaFontField*	fontField = static_cast<CocoaFontField*>(view);
		ACocoa::SetTextFromNSString([[fontField font] fontName],outFontName);
		outFontSize = [fontField getFontSizeData];
	}
	//NSControl�Ȃ�A�t�H���g�擾
	else if( [view isKindOfClass:[NSControl class]] == YES )
	{
		NSControl*	control = ACocoa::CastNSControlFromId(view);
		ACocoa::SetTextFromNSString([[control font] fontName],outFontName);
		outFontSize = [[control font] pointSize];
	}
	//��L�ȊO�̓G���[
	else
	{
		_ACError("",NULL);
	}
}

/**
���ݒ�
*/
void	CWindowImp::SetEmptyValue( const AControlID inID )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetEmptyValue();
		return;
	}
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//0��ݒ�
		SetNumber(inID,0);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[���̏ꍇ
		AControlRef	controlRef = GetControlRef(inID);
		::SetControl32BitValue(controlRef,0);
	}
#endif
}

/**
Icon�ݒ�
*/
void	CWindowImp::SetIcon( const AControlID inControlID, const AIconID inIconID, 
		const ABool inDefaultSize, const ANumber inLeftOffset, const ANumber inTopOffset )//win 080721
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		//�����Ȃ�
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		ControlButtonContentInfo	info;
		IconRef	iconref = GetIconRef(inIconID);
		if( iconref != NULL )
		{
			info.contentType = kControlContentIconRef;
			info.u.iconRef = iconref;
		}
		else
		{
			info.contentType = kControlContentCIconHandle;
			info.u.cIconHandle = GetCIconHandle(inIconID);
			if( info.u.cIconHandle == NULL )   {_ACError("icon handle null",this); return;}
		}
		err = ::SetControlData(GetControlRef(inControlID),kControlEntireControl,kControlBevelButtonContentTag,sizeof(info),&info);
		if( err != noErr )   _ACErrorNum("SetControlData() returned error: ",err,this);
		DrawControl(inControlID);
	}
#endif
}

/**
Icon�ݒ�
*/
/*
void	CWindowImp::SetIconByImageIndex( const AControlID inControlID, const AIndex inImageIndex )
{
	OSStatus	err = noErr;
	
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	ControlButtonContentInfo	info;
	info.contentType = kControlContentPictHandle;
	info.u.picture = GetImage(inImageIndex);
	err = ::SetControlData(GetControlRef(inControlID),kControlEntireControl,kControlBevelButtonContentTag,sizeof(info),&info);
	if( err != noErr )   _ACErrorNum("SetControlData() returned error: ",err,this);
}
*/


/**
�ő�l�^�ŏ��l��ݒ�
*/
void	CWindowImp::SetMinMax( const AControlID inControlID, const ANumber inMin, const ANumber inMax )
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//MinMax�������o�ϐ��ɕۑ�����
		ABool	changed = false;
		AIndex	index = mMinMaxArray_ControlID.Find(inControlID);
		if( index == kIndex_Invalid )
		{
			index = mMinMaxArray_ControlID.Add(inControlID);
			mMinMaxArray_Min.Add(inMin);
			mMinMaxArray_Max.Add(inMax);
			//�ύX�t���OON
			changed = true;
		}
		if( mMinMaxArray_Min.Get(index) != inMin || mMinMaxArray_Max.Get(index) != inMax )
		{
			mMinMaxArray_Min.Set(index,inMin);
			mMinMaxArray_Max.Set(index,inMax);
			//�ύX�t���OON
			changed = true;
		}
		
		//�ύX������΁A�X�N���[���o�[��enable/disable���X�V
		if( changed == true )
		{
			//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
			NSView*	view = CocoaObjects->FindViewByTag(inControlID);
			
			//NSScroller�̏ꍇ�AinMin>=inMax�Ȃ�X�N���[����disable
			if( [view isKindOfClass:[NSScroller class]] == YES )
			{
				NSScroller*	scroller = ACocoa::CastNSScrollerFromId(view);
				if( inMin < inMax )
				{
					[scroller setEnabled:YES];
				}
				else
				{
					[scroller setEnabled:NO];
				}
			}
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		::SetControl32BitMinimum(GetControlRef(inControlID),inMin);
		::SetControl32BitMaximum(GetControlRef(inControlID),inMax);
	}
#endif
}

//#725
/**
�X�N���[���o�[��page unit��ݒ�
*/
void	CWindowImp::SetScrollBarPageUnit( const AControlID inControlID, const ANumber inUnit )
{
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inControlID);
	
	//NSScroller�̏ꍇ�AinMin>=inMax�Ȃ�X�N���[����disable
	if( [view isKindOfClass:[NSScroller class]] == YES )
	{
		NSScroller*	scroller = ACocoa::CastNSScrollerFromId(view);
		AIndex	index = mMinMaxArray_ControlID.Find(inControlID);
		if( index != kIndex_Invalid )
		{
			ANumber	min = mMinMaxArray_Min.Get(index);
			ANumber	max = mMinMaxArray_Max.Get(index);
			if( min < max )
			{
				AFloatNumber	prop = inUnit;
				prop = prop/(max-min+inUnit);
				if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
				{
					[scroller setKnobProportion:prop];
				}
			}
		}
	}
}

//#688
/**
*/
/*
void	CWindowImp::SetMouseTrackingRect( const AControlID inControlID, const AArray<ALocalRect>& inLocalRectArray )
{
	CocoaUserPaneView*	view = CocoaObjects->FindCocoaUserPaneViewByTag(inControlID);
	if( view == nil )   {_ACError("",NULL);return;}
	
	//
	[view setMouseTrackingRect:inLocalRectArray];
}
*/

//#725
/**
�R���g���[���Ƀ��j���[��ݒ�
*/
void	CWindowImp::SetControlMenu( const AControlID inControlID, const AMenuRef inMenuRef )
{
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inControlID);
	
	//NSPopUpButton�̏ꍇ�ATextArray�̓��e�����j���[���ڂƂ��Ēǉ�
	if( [view isKindOfClass:[NSPopUpButton class]] == YES )
	{
		NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
		
		[popup setMenu:(NSMenu*)inMenuRef];
	}
}

//#688
/**
�t�H���g�p�l���\��
*/
void	CWindowImp::ShowControlFontPanel( const AControlID inControlID )
{
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inControlID);
	
	if( [view isKindOfClass:[CocoaFontField class]] == YES )
	{
		CocoaFontField*	fontField = static_cast<CocoaFontField*>(view);
		[fontField doFontChangeButton:nil];
	}
}

#pragma mark ===========================

#pragma mark ---�R���g���[�����I���C���^�[�t�F�C�X

/**
�e�L�X�g�I��
*/
void	CWindowImp::SetSelection( const AControlID inID, const AIndex inStart, const AIndex inEnd )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetSelection(inStart,inEnd);
		return;
	}
	//�ʏ�R���g���[���̏ꍇ
	//������
	_ACError("not implemented",NULL);
}

/**
�e�L�X�g�I���i�S�I���j
*/
void	CWindowImp::SetSelectionAll( const AControlID inID )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetSelectionAll();
		return;
	}
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSTextField�̏ꍇ
		if( [view isKindOfClass:[NSTextField class]] == YES )
		{
			NSTextField*	textfield = ACocoa::CastNSTextFieldFromId(view);
			[textfield selectText:nil];
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//�ʏ�R���g���[���̏ꍇ
		//�e�L�X�g�n�̏ꍇ�̂ݎ��s#353
		OSStatus	err = noErr;
		
		ControlKind	kind;
		err = ::GetControlKind(GetControlRef(inID),&kind);
		if( kind.kind == kControlKindEditText || kind.kind == kControlKindHIComboBox || 
					kind.kind == kControlKindEditUnicodeText )
		{
			//kControlEditTextSelectionTag�ݒ�ɂ��S�I�����s
			ControlEditTextSelectionRec	sel;
			CFStringRef	strref = NULL;
			Size	actSize = 0;
			err = ::GetControlData(GetControlRef(inID),kControlEntireControl,kControlEditTextCFStringTag,sizeof(strref),&strref,&actSize);
			if( err != noErr )   _ACErrorNum("GetControlData() returned error: ",err,this);
			if( strref != NULL )
			{
				sel.selStart = 0;
				sel.selEnd = ::CFStringGetLength(strref);
				::CFRelease(strref);
				err = ::SetControlData(GetControlRef(inID),kControlEntireControl,kControlEditTextSelectionTag,sizeof(sel),&(sel));
				if( err != noErr )   _ACErrorNum("SetControlData() returned error: ",err,this);
			}
		}
	}
#endif
}

#pragma mark ===========================

#pragma mark ---�R���g���[�������^�폜

#if SUPPORT_CARBON
/**
�x�x���{�^������
*/
void	CWindowImp::CreateBevelButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, 
		const AText& inTitle, const ABool inEnableMenu )//win 080618
{
	OSStatus	err = noErr;
	
	Rect	rect;
	rect.left = inPoint.x;
	rect.top = inPoint.y;
	rect.right = rect.left + inWidth;
	rect.bottom = rect.top + inHeight;
	AStCreateCFStringFromAText	strref(inTitle);
	AControlRef	controlRef;
	err = ::CreateBevelButtonControl(mWindowRef,&rect,strref.GetRef(),kControlBevelButtonSmallBevel,kControlBehaviorPushbutton,NULL,0,0,0,&controlRef);
	if( err != noErr )   _ACErrorNum("CreateBevelButtonControl() returned error: ",err,this);
	ControlID	controlID;
	controlID.signature = 0;
	controlID.id = inID;
	err = ::SetControlID(controlRef,&controlID);
	if( err != noErr )   _ACErrorNum("SetControlID() returned error: ",err,this);
	err = ::SetControlCommandID(controlRef,inID);
	if( err != noErr )   _ACErrorNum("SetControlCommandID() returned error: ",err,this);
	
	if( inEnableMenu == true )
	{
		ACarbonMenuRef	menuref = ACarbonMenuWrapper::CreateMenu();
		err = ::SetControlData(controlRef,0,kControlBevelButtonMenuRefTag,sizeof(ACarbonMenuRef),&menuref);
		if( err != noErr )   _ACErrorNum("SetControlData() returned error: ",err,this);
		
		//OS10.3�o�O�΍�
		//http://lists.apple.com/archives/carbon-development/2003/Jun/msg00303.html
		//B0000 OSStatus	err;
		EventHandlerRef	handlerRef;
		EventTypeSpec	typeSpec;
		
		typeSpec.eventClass = kEventClassControl;
		typeSpec.eventKind = kEventControlTrack;
		err = ::InstallEventHandler(::GetControlEventTarget(controlRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_ControlTrackHandler),
					1,&typeSpec,this,&handlerRef);
		if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	}
}
#endif

/*
AView_Button����������ߕs�v
void	CWindowImp::CreateCustomButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, 
		const AText& inTitle, const ABool inEnableMenu, const AControlID inParentID, const ABool inToggle )//win 080618
{
	OSStatus	err = noErr;
	
	Rect	rect;
	rect.left = inPoint.x;
	rect.top = inPoint.y;
	rect.right = rect.left + inWidth;
	rect.bottom = rect.top + inHeight;
	AStCreateCFStringFromAText	strref(inTitle);
	AControlRef	controlRef;
	//win 080618
	ControlBevelButtonBehavior	behavior = kControlBehaviorPushbutton;
	if( inToggle == true )
	{
		behavior |= kControlBehaviorToggles;
	}
	
	err = ::CreateBevelButtonControl(mWindowRef,&rect,strref.GetRef(),kControlBevelButtonSmallBevel,behavior,NULL,0,0,0,&controlRef);
	if( err != noErr )   _ACErrorNum("CreateBevelButtonControl() returned error: ",err,this);
	ControlID	controlID;
	controlID.signature = 0;
	controlID.id = inID;
	err = ::SetControlID(controlRef,&controlID);
	if( err != noErr )   _ACErrorNum("SetControlID() returned error: ",err,this);
	err = ::SetControlCommandID(controlRef,inID);
	if( err != noErr )   _ACErrorNum("SetControlCommandID() returned error: ",err,this);
	
	if( inEnableMenu == true )
	{
		ACarbonMenuRef	menuref = ACarbonMenuWrapper::CreateMenu();
		err = ::SetControlData(controlRef,0,kControlBevelButtonMenuRefTag,sizeof(ACarbonMenuRef),&menuref);
		if( err != noErr )   _ACErrorNum("SetControlData() returned error: ",err,this);
		
		//OS10.3�o�O�΍�
		//http://lists.apple.com/archives/carbon-development/2003/Jun/msg00303.html
		//B0000 OSStatus	err;
		EventHandlerRef	handlerRef;
		EventTypeSpec	typeSpec;
		
		typeSpec.eventClass = kEventClassControl;
		typeSpec.eventKind = kEventControlTrack;
		err = ::InstallEventHandler(::GetControlEventTarget(controlRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_ControlTrackHandler),
				1,&typeSpec,this,&handlerRef);
		if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	}
}
*/

/**
Icon�{�^������
*/
/*
void	CWindowImp::CreateIconButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIconID inIconID )//win 080618
{
	OSStatus	err = noErr;
	
	Rect	rect;
	rect.left = inPoint.x;
	rect.top = inPoint.y;
	rect.right = rect.left + inWidth;
	rect.bottom = rect.top + inHeight;
	ControlButtonContentInfo	info;
	IconRef	iconref = GetIconRef(inIconID);
	if( iconref != NULL )
	{
		info.contentType = kControlContentIconRef;
		info.u.iconRef = iconref;
	}
	else
	{
		info.contentType = kControlContentCIconHandle;
		info.u.cIconHandle = GetCIconHandle(inIconID);
	}
	AControlRef	controlRef;
	err = ::CreateIconControl(mWindowRef,&rect,&info,false,&controlRef);
	if( err != noErr )   _ACErrorNum("CreateIconControl() returned error: ",err,this);
	ControlID	controlID;
	controlID.signature = 0;
	controlID.id = inID;
	err = ::SetControlID(controlRef,&controlID);
	if( err != noErr )   _ACErrorNum("SetControlID() returned error: ",err,this);
	err = ::SetControlCommandID(controlRef,inID);
	if( err != noErr )   _ACErrorNum("SetControlCommandID() returned error: ",err,this);
}
*/

/**
Rounded Rect�{�^������
*/
void	CWindowImp::CreateRoundedRectButton( const AControlID inID, 
		const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex )
{
	//�E�C���h�E���̐����ʒuframe���v�Z
	AWindowRect	wrect = {0};
	wrect.left	= inPoint.x;
	wrect.top	= inPoint.y;
	wrect.right	= inPoint.x + inWidth;
	wrect.bottom= inPoint.y + inHeight;
	NSRect	rect = ConvertFromWindowRectToNSRect(CocoaObjects->GetContentView(),wrect);
	//button����
	NSButton*	button = [[NSButton alloc] initWithFrame:rect];
	[button setButtonType:NSMomentaryPushInButton];
	[button setBezelStyle:NSShadowlessSquareBezelStyle];//NSSmallSquareBezelStyle];
	AText	title;
	AStCreateNSStringFromAText	titlestr(title);
	[button setTitle:titlestr.GetNSString()];
	[CocoaObjects->GetContentView() addSubview:button positioned:NSWindowAbove relativeTo:nil];
	[button release];//addSubview:��content view��retain����A�A�v�����Ń|�C���^���ێ����Ȃ��̂ł������release����B
	[button setTag:inID];
}

/*
NSRegularSquareBezelStyle: Rounded�Aaqua�n����������ƈႤ�B�g���K�v�Ȃ��B
NSShadowlessSquareBezelStyle: �p���{�^���Bgradient�����B
NSCircularBezelStyle: �~�`�{�^���B
NSTexturedSquareBezelStyle: Rounded�Btextured�Ƃ������̂����Â������B�g���K�v�Ȃ��B
NSSmallSquareBezelStyle: �p���{�^���Bgradient�t���B
NSTexturedRoundedBezelStyle: Rounded�BSafari�^�C�v�c�[���o�[�̃{�^���Ɠ����B�����Œ�(22?)�B
NSRoundRectBezelStyle : Rounded�BFinder�́u�ʖ��Őڑ��v�{�^���Ɠ����B�����Œ�B

    NSRoundedBezelStyle          = 1,
    NSRegularSquareBezelStyle    = 2,
    NSThickSquareBezelStyle      = 3,
    NSThickerSquareBezelStyle    = 4,
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3
    NSDisclosureBezelStyle       = 5,
#endif
    NSShadowlessSquareBezelStyle = 6,
    NSCircularBezelStyle         = 7,
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3
    NSTexturedSquareBezelStyle   = 8,
    NSHelpButtonBezelStyle       = 9,
#endif
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
    NSSmallSquareBezelStyle       = 10,
    NSTexturedRoundedBezelStyle   = 11,
    NSRoundRectBezelStyle         = 12,
    NSRecessedBezelStyle          = 13,
    NSRoundedDisclosureBezelStyle = 14,
*/

/**
�v���O���Xindicator����
*/
void	CWindowImp::CreateProgressIndicator( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex )
{
	//�E�C���h�E���̐����ʒuframe���v�Z
	AWindowRect	wrect = {0};
	wrect.left	= inPoint.x;
	wrect.top	= inPoint.y;
	wrect.right	= inPoint.x + inWidth;
	wrect.bottom= inPoint.y + inHeight;
	NSRect	rect = ConvertFromWindowRectToNSRect(CocoaObjects->GetContentView(),wrect);
	//progress indicator����
	CocoaProgressIndicator*	progress = [[CocoaProgressIndicator alloc] initWithFrame:rect];
	[CocoaObjects->GetContentView() addSubview:progress positioned:NSWindowAbove relativeTo:nil];
	[progress release];//addSubview:��content view��retain����A�A�v�����Ń|�C���^���ێ����Ȃ��̂ł������release����B
	[progress setTag:inID];
	//
	[progress setStyle:NSProgressIndicatorSpinningStyle];
	[progress setControlSize:NSSmallControlSize];
	[progress setBezeled:NO];
}

/**
�X�N���[���o�[����
*/
void	CWindowImp::CreateScrollBar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex )//win 080618
{
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�E�C���h�E���̐����ʒuframe���v�Z
		AWindowRect	wrect = {0};
		wrect.left	= inPoint.x;
		wrect.top	= inPoint.y;
		wrect.right	= inPoint.x + inWidth;
		wrect.bottom= inPoint.y + inHeight;
		NSRect	rect = ConvertFromWindowRectToNSRect(CocoaObjects->GetContentView(),wrect);
		//scroller����
		NSScroller*	scroller = [[NSScroller alloc] initWithFrame:rect];
		[CocoaObjects->GetContentView() addSubview:scroller positioned:NSWindowAbove relativeTo:nil];
		[scroller release];//addSubview:��content view��retain����A�A�v�����Ń|�C���^���ێ����Ȃ��̂ł������release����B
		[scroller setTag:inID];
		//scroller�̃T�C�Y�^�C�v��ݒ�iSetControlSize()�ł��ݒ肳���
		if( inWidth < 15 || inHeight < 15 )
		{
			[scroller setControlSize:NSSmallControlSize];
		}
		else
		{
			[scroller setControlSize:NSRegularControlSize];
		}
		//target/action�ݒ�
		[scroller setTarget:CocoaObjects->GetWindow()];
		[scroller setAction:@selector(doScrollbarAction:)];
		//�ŏ��E�ő�l������
		SetMinMax(inID,0,0);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		OSStatus	err = noErr;
		
		Rect	rect;
		rect.left 		= inPoint.x;
		rect.top 		= inPoint.y;
		rect.right 		= rect.left + inWidth;
		rect.bottom 	= rect.top + inHeight;
		AControlRef	controlRef;
		err = ::CreateScrollBarControl(mWindowRef,&rect,0,0,0,0,true,::NewControlActionUPP(STATIC_APICB_ScrollBarActionProc),&controlRef);
		if( err != noErr )   _ACErrorNum("CreateScrollBarControl() returned error: ",err,this);
		ControlID	controlID;
		controlID.signature = 0;
		controlID.id = inID;
		err = ::SetControlID(controlRef,&controlID);
		if( err != noErr )   _ACErrorNum("SetControlID() returned error: ",err,this);
		err = ::SetControlCommandID(controlRef,inID);
		if( err != noErr )   _ACErrorNum("SetControlCommandID() returned error: ",err,this);
		//win 080618
		if( inTabIndex != kIndex_Invalid )
		{
			err = ::EmbedControl(controlRef,GetTabControlRef(inTabIndex));
			if( err != noErr )   _ACErrorNum("EmbedControl() returned error: ",err,this);
			SetControlPosition(inID,inPoint);
		}
	}
#endif
}

/**
�X�N���[���o�[�o�^�i���łɃ��\�[�X�ɑ��݂���ꍇ�p�j
*/
void	CWindowImp::RegisterScrollBar( const AControlID inID )
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�����Ȃ�
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		::SetControlAction(GetControlRef(inID),::NewControlActionUPP(STATIC_APICB_ScrollBarActionProc));
	}
#endif
}

//R0191
/**
SearchField����
*/
/*�g�p����\��Ȃ�
void	CWindowImp::CreateSearchField( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	OSStatus	err = noErr;
	
	HIRect	rect;
	rect.origin.x	= inPoint.x;//0;
	rect.origin.y	= inPoint.y;//0;
	rect.size.width	= inWidth;
	rect.size.height= inHeight;
	AControlRef	controlRef;
	err = ::HISearchFieldCreate(&rect,kHISearchFieldAttributesCancel|kHISearchFieldAttributesSearchIcon,NULL,CFSTR("find"),&controlRef);
	if( err != noErr )   _ACErrorNum("HISearchFieldCreate() returned error: ",err,this);
	HIViewRef content;
	err = ::HIViewFindByID(::HIViewGetRoot(mWindowRef),kHIViewWindowContentID,&content);
	if( err != noErr )   _ACErrorNum("HIViewGetRoot() returned error: ",err,this);
	err = ::EmbedControl(controlRef,content);
	if( err != noErr )   _ACErrorNum("EmbedControl() returned error: ",err,this);
	ControlID	controlID;
	controlID.signature = 0;
	controlID.id = inID;
	err = ::SetControlID(controlRef,&controlID);
	if( err != noErr )   _ACErrorNum("SetControlID() returned error: ",err,this);
	err = ::SetControlCommandID(controlRef,inID);
	if( err != noErr )   _ACErrorNum("SetControlCommandID() returned error: ",err,this);
}
*/

/**
Placard����
*/
/*�g�p����\��Ȃ�
void	CWindowImp::CreatePlacard( const AControlID inID, const APoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	OSStatus	err = noErr;
	
	Rect	rect;
	rect.left 		= inPoint.x;
	rect.top 		= inPoint.y;
	rect.right 		= rect.left + inWidth;
	rect.bottom 	= rect.top + inHeight;
	AControlRef	controlRef;
	err = ::CreatePlacardControl(mWindowRef,&rect,&controlRef);
	if( err != noErr )   _ACErrorNum("CreatePlacardControl() returned error: ",err,this);
	ControlID	controlID;
	controlID.signature = 0;
	controlID.id = inID;
	err = ::SetControlID(controlRef,&controlID);
	if( err != noErr )   _ACErrorNum("SetControlID() returned error: ",err,this);
	err = ::SetControlCommandID(controlRef,inID);
	if( err != noErr )   _ACErrorNum("SetControlCommandID() returned error: ",err,this);
}
*/

/**
�R���g���[���폜
*/
void	CWindowImp::DeleteControl( const AControlID inID )
{
	//#212
	//���݂̃t�H�[�J�X���폜����Ƃ��́A�t�H�[�J�X��������
	if( /*#688 mCurrentFocus*/GetCurrentFocus() == inID )
	{
		ClearFocus();
	}
	//TextArrayMenu�̓o�^����
	AIndex textArrayMenuIndex = mTextArrayMenuArray_ControlID.Find(inID);
	if( textArrayMenuIndex != kIndex_Invalid )
	{
		/*win AApplication::GetApplication().NVI_GetMenuManager().UnregisterTextArrayMenu(GetMenuRef(inID));
		mTextArrayMenuArray_ControlID.Delete1(textArrayMenuIndex);
		mTextArrayMenuArray_TextArrayMenuID.Delete1(textArrayMenuIndex);*/
		UnregisterTextArrayMenu(inID);
	}
#if SUPPORT_CARBON
	//#236
	AIndex dropEnableControlIndex = mDropEnableControlArray.Find(inID);
	if( dropEnableControlIndex != kIndex_Invalid )
	{
		mDropEnableControlArray.Delete1(dropEnableControlIndex);
	}
#endif
	/*#135
	//win 080618
	AIndex	focusindex = mFocusGroup.Find(inID);
	if( focusindex != kIndex_Invalid )
	{
		mFocusGroup.Delete1(focusindex);
		UpdateFocus();
	}
	*/
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//MinMax�ۑ��f�[�^�̍폜
		AIndex	index = mMinMaxArray_ControlID.Find(inID);
		if( index != kIndex_Invalid )
		{
			mMinMaxArray_ControlID.Delete1(index);
			mMinMaxArray_Min.Delete1(index);
			mMinMaxArray_Max.Delete1(index);
		}
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//ControlID�̃L���b�V�����N���A
		CocoaObjects->RemoveFromControlIDCache(inID);
		
		//view��eview����O���BremoveFromSuperview������1��release�����B�iaddSubView�ɂ��retain�ɑΉ��j
		//UserPane�ȊO�Ȃ炱��ɂ��폜�����B
		//UserPane�Ȃ�܂�CocoaObjects��retain���Ă���̂ŁAUnregisterUserPaneView()�ɂ��폜�����B
		[view removeFromSuperview];
		
		//UserPane�̍폜
		AIndex	userpaneindex = mUserPaneArray_ControlID.Find(inID);
		if( userpaneindex != kIndex_Invalid )
		{
			mUserPaneArray.Delete1Object(userpaneindex);
			mUserPaneArray_ControlID.Delete1(userpaneindex);
			mUserPaneArray_Type.Delete1(userpaneindex);
			//#688 mUserPaneArray_ControlRef.Delete1(userpaneindex);//#688
			
			//CocoaObjects�̃R���g���[���o�^����
			CocoaObjects->UnregisterUserPaneView(inID);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		//UserPane�̍폜
		AIndex	userpaneindex = mUserPaneArray_ControlID.Find(inID);
		if( userpaneindex != kIndex_Invalid )
		{
			mUserPaneArray.Delete1Object(userpaneindex);
			mUserPaneArray_ControlID.Delete1(userpaneindex);
			mUserPaneArray_Type.Delete1(userpaneindex);
			mUserPaneArray_ControlRef.Delete1(userpaneindex);//#688
		}
		//
		::DisposeControl(GetControlRef(inID));
	}
#endif
}

#if SUPPORT_CARBON
/**
�X�N���[���o�[���쎞��Callback
*/
void	CWindowImp::APICB_DoScrollBarAction( const AControlID inID, const ControlPartCode inPartCode )
{
	switch(inPartCode)
	{
	  case kControlUpButtonPart:
		{
			GetAWin().EVT_DoScrollBarAction(inID,kScrollBarPart_UpButton);
			break;
		}
	  case kControlDownButtonPart:
		{
			GetAWin().EVT_DoScrollBarAction(inID,kScrollBarPart_DownButton);
			break;
		}
	  case kControlPageUpPart:
		{
			GetAWin().EVT_DoScrollBarAction(inID,kScrollBarPart_PageUpButton);
			break;
		}
	  case kControlPageDownPart:
		{
			GetAWin().EVT_DoScrollBarAction(inID,kScrollBarPart_PageDownButton);
			break;
		}
	  case kControlIndicatorPart:
		{
			GetAWin().EVT_DoScrollBarAction(inID,kScrollBarPart_ThumbTracking);
			break;
		}
	  default:
		{
			//��������
			break;
		}
	}
}

/**
�X�N���[���o�[���쎞��Callback(static)
*/
void	CWindowImp::STATIC_APICB_ScrollBarActionProc( ControlRef theControl, ControlPartCode partCode )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-SBAP)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		AWindowRef	windowRef = ::HIViewGetWindow(theControl);
		for( AIndex i = 0; i < sAliveWindowArray.GetItemCount(); i++ )
		{
			if( (sAliveWindowArray.Get(i))->IsWindowCreated() == true )
			{
				if( (sAliveWindowArray.Get(i))->GetWindowRef() == windowRef )
				{
					ControlID	controlID;
					err = ::GetControlID(theControl,&controlID);
					if( err != noErr )   _ACErrorNum("GetControlID() returned error: ",err,NULL);
					(sAliveWindowArray.Get(i))->APICB_DoScrollBarAction(controlID.id,partCode);
					break;
				}
			}
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_ScrollBarActionProc() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-SBAP]");
	return;
}
#endif

#pragma mark ===========================

#pragma mark ---�L�[�{�[�h�t�H�[�J�X�Ǘ�

/*#135
//�L�[�{�[�h�t�H�[�J�X�̃O���[�v�ɓo�^
void	CWindowImp::RegisterFocusGroup( const AControlID inControlID )
{
	mFocusGroup.Add(inControlID);
}
*/

/**
�L�[�{�[�h�t�H�[�J�X��ݒ�
*/
void	CWindowImp::SetFocus( const AControlID inControlID )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//���݂̃t�H�[�J�X�Ɠ����Ȃ烊�^�[��
	if( inControlID == /*#688 mCurrentFocus*/GetCurrentFocus() )   return;
	
	//
	//#135 if( mFocusGroup.Find(inControlID) == kControlID_Invalid )   return;
	
	//���݂̃t�H�[�J�X���N���A����
	//#688 ���ŕK���ݒ肳���̂ŁA�N���A����K�v�Ȃ��BClearFocus();
	
	/*#688
	//mCurrentFocus��ݒ�
	mCurrentFocus = inControlID;
	mLatentFocus = inControlID;//#0 kControlID_Invalid;
	*/
	
#if SUPPORT_CARBON
	//#668 �t�H�[�J�X��OS����ɂ��邽�߁AUserPane�ł����Ă��ASetFocus()�͒��ڎ��s���Ȃ��B
	//EVT_DoViewFocusActivated()/EVT_DoViewFocusDeactivated()�́A
	//APICB_CocoaViewBecomeFirstResponder()/APICB_CocoaViewResignFirstResponder()�o�R�Ŏ��s�����B
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inControlID) == true )
	{
		OSStatus	err = noErr;
		
		//OS��KeyboardFocus�̓N���A
		err = ::ClearKeyboardFocus(mWindowRef);
		if( err != noErr )   _ACErrorNum("ClearKeyboardFocus() returned error: ",err,this);
		//UserPane��SetFocus()���s
		GetUserPane(inControlID).SetFocus();
		return;
	}
#endif
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		//view��FirstResponder�ɐݒ�
		[CocoaObjects->GetWindow() makeFirstResponder:view];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		//�ʏ�R���g���[���̏ꍇ
		ControlKind	kind;
		err = ::GetControlKind(GetControlRef(inControlID),&kind);
		//if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
		if( kind.kind == kControlKindEditText || kind.kind == kControlKindHIComboBox || 
					kind.kind == kControlKindEditUnicodeText ||
					kind.kind == kControlKindDataBrowser /* ||
			true*/	)
		{
			//OS��KeyboardFocus��ݒ�
			err = ::SetKeyboardFocus(mWindowRef,GetControlRef(inControlID),kControlFocusNextPart);
			if( err != noErr )   _ACErrorNum("SetKeyboardFocus() returned error: ",err,this);
		}
		//#353�@�{�^�����ɂ��t�H�[�J�X�𓖂Ă�Ή��i����L�ł킴�킴�R���g���[�������肵�Ă������R�H�j
		else
		{
			//OS��KeyboardFocus��ݒ�
			err = ::SetKeyboardFocus(mWindowRef,GetControlRef(inControlID),kControlFocusNextPart);
			//	if( err != noErr )   _ACErrorNum("SetKeyboardFocus() returned error: ",err,this);
		}
	}
#endif
}

/**
�O��View�Ƀt�H�[�J�X�ݒ�
*/
void	CWindowImp::SetFocusPrev()
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//�O��view���t�H�[�J�X�ɐݒ�
	[CocoaObjects->GetWindow() selectPreviousKeyView:nil];
}

/**
����View�Ƀt�H�[�J�X�ݒ�
*/
void	CWindowImp::SetFocusNext()
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//����view���t�H�[�J�X�ɐݒ�
	[CocoaObjects->GetWindow() selectNextKeyView:nil];
}

/**
�t�H�[�J�X���N���A
*/
void	CWindowImp::ClearFocus( const ABool inAlsoClearLatentFocus )//#212
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
#if SUPPORT_CARBON
	//#668 �t�H�[�J�X��OS����ɂ��邽�߁AUserPane�ł����Ă��ASetFocus()�͒��ڎ��s���Ȃ��B
	//EVT_DoViewFocusActivated()/EVT_DoViewFocusDeactivated()�́A
	//APICB_CocoaViewBecomeFirstResponder()/APICB_CocoaViewResignFirstResponder()�o�R�Ŏ��s�����B
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( /*#688 mCurrentFocus*/GetCurrentFocus() != kControlID_Invalid )
	{
		if( IsUserPane(/*#688 mCurrentFocus*/GetCurrentFocus()) == true )
		{
			GetUserPane(/*#688 mCurrentFocus*/GetCurrentFocus()).ResetFocus();
		}
	}
#endif
	//OS��KeyboardFocus���N���A
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�E�C���h�E��First Responder�ɂ���
		[CocoaObjects->GetWindow() makeFirstResponder:nil];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		err = ::ClearKeyboardFocus(mWindowRef);
		if( err != noErr )   _ACErrorNum("ClearKeyboardFocus() returned error: ",err,this);
	}
	/*#688
	//mCurrentFocus���N���A
	mCurrentFocus = kControlID_Invalid;
	//inAlsoClearLatentFocus��true�Ȃ�latent���N���A�i�f�t�H���g��latent���N���A�B�������Adeactivated����R�[�������ꍇ��latent�͎c���j #212
	if( inAlsoClearLatentFocus == true )
	{
		mLatentFocus = kControlID_Invalid;
	}
	*/
#endif
}

/*#135
//�ŏ��̃t�H�[�J�X�ɐݒ肷��
void	CWindowImp::SwitchFocusToFirst()
{
	if( mFocusGroup.GetItemCount() == 0 )   return;
	SwitchFocusTo(mFocusGroup.Get(0));
}


//���̃t�H�[�J�X�֐i�߂�
void	CWindowImp::SwitchFocusToNext()
{
	if( mFocusGroup.GetItemCount() == 0 )   return;
	
	//����Focus����focusIndex�ɓ����
	//����Focus���I���̏ꍇ��Focus����0
	AIndex	focusIndex = 0;
	if( mCurrentFocus != kControlID_Invalid )
	{
		//���݂�Focus�̃C���f�b�N�X������
		focusIndex = mFocusGroup.Find(mCurrentFocus);
		
		if( focusIndex == kIndex_Invalid )
		{
			//���݂�Focus�̃C���f�b�N�X��������Ȃ��ꍇ�i�ӂ��͂��肦�Ȃ��j
			_AError("focus not found",this);
			focusIndex = 0;
		}
		else
		{
			//focusIndex�����ɐi�߂�
			focusIndex++;
			if( focusIndex >= mFocusGroup.GetItemCount() )
			{
				focusIndex = 0;
			}
		}
	}
	SwitchFocusTo(mFocusGroup.Get(focusIndex));
}

//�O�̃t�H�[�J�X�֖߂� B0429
void	CWindowImp::SwitchFocusToPrev()
{
	if( mFocusGroup.GetItemCount() == 0 )   return;
	
	//�O��Focus����focusIndex�ɓ����
	//����Focus���I���̏ꍇ��Focus����mFocusGroup.GetItemCount()-1
	AIndex	focusIndex = mFocusGroup.GetItemCount()-1;
	if( mCurrentFocus != kControlID_Invalid )
	{
		//���݂�Focus�̃C���f�b�N�X������
		focusIndex = mFocusGroup.Find(mCurrentFocus);
		
		if( focusIndex == kIndex_Invalid )
		{
			//���݂�Focus�̃C���f�b�N�X��������Ȃ��ꍇ�i�ӂ��͂��肦�Ȃ��j
			_AError("focus not found",this);
			focusIndex = 0;
		}
		else
		{
			//focusIndex��O�֖߂�
			focusIndex--;
			if( focusIndex < 0 )
			{
				focusIndex = mFocusGroup.GetItemCount()-1;
			}
		}
	}
	SwitchFocusTo(mFocusGroup.Get(focusIndex),true);
}

//�t�H�[�J�X���w��ControlID�̃R���g���[���ɐ؂�ւ���
//public
void	CWindowImp::SwitchFocusTo( const AControlID inControlID, const ABool inReverseSearch )//B0429
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( IsActive() == false )
	{
		mLatentFocus = inControlID;
		return;
	}
	
	if( inControlID != kControlID_Invalid )
	{
		AIndex	focusGroupIndex = mFocusGroup.Find(inControlID);
		if( focusGroupIndex != kIndex_Invalid )
		{
			//inControlID���珇�ɁAEnable����Visible�ȃR���g���[����������܂Ō������A���������炻����focus��ݒ�
			for( AIndex i = 0; i < mFocusGroup.GetItemCount(); i++ )
			{
				AControlID	controlID = mFocusGroup.Get(focusGroupIndex);
				if( IsControlEnabled(controlID) == true && IsControlVisible(controlID) == true )
				{
					SetFocus(controlID);
					if( IsAutoSelectAllBySwitchFocus(controlID) == true )
					{
						SetSelectionAll(controlID);
					}
					return;
				}
				if( inReverseSearch == false )
				{
					focusGroupIndex++;
					if( focusGroupIndex >= mFocusGroup.GetItemCount() )
					{
						focusGroupIndex = 0;
					}
				}
				else//B0429 �t�H�[�J�X��O�֖߂��ꍇ�́A�L���ȃR���g���[����O�����ɒT���K�v������
				{
					focusGroupIndex--;
					if( focusGroupIndex < 0 )
					{
						focusGroupIndex = mFocusGroup.GetItemCount()-1;
					}
				}
			}
		}
	}
	//Enable����Visible�ȃR���g���[�����Ȃ��ꍇ�̓t�H�[�J�X���N���A
	ClearFocus();
}
*/

#if SUPPORT_CARBON
//#688 Cocoa�Ή��ɂ��폜�BOS�ł̐���ɔC����
/**
�t�H�[�J�X�̃R���g���[����Disable���ꂽ�ꍇ�A�܂��́A�t�H�[�J�X�����̏�Ԃ���t�H�[�J�X�\�ȃR���g���[����Enable���ꂽ�ꍇ�̏���
*/
void	CWindowImp::UpdateFocus()
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( mCurrentFocus != kControlID_Invalid )
	{
		if( IsControlEnabled(mCurrentFocus) == false )
		{
			GetAWin().NVI_SwitchFocusTo(mCurrentFocus);
		}
	}
	else
	{
		//SwitchFocusToFirst();
	}
}
#endif

/**
���݂̃t�H�[�J�X�擾
*/
AControlID	CWindowImp::GetCurrentFocus() const
{
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		NSResponder*	responder = [CocoaObjects->GetWindow() firstResponder];
		if( responder != nil )
		{
			//firstResponder�����݂��Ă��āAview�ł���΁Atag��Ԃ�
			if( [responder isKindOfClass:[NSView class]] == YES )
			{
				NSView*	view = ACocoa::CastNSViewFromId(responder);
				return [view tag];
			}
		}
		return kControlID_Invalid;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		if( IsActive() == true )
		{
			return mCurrentFocus;
		}
		else
		{
			return mLatentFocus;
		}
	}
#endif
}

#pragma mark ===========================

#pragma mark --- �^�uView�Ǘ�

/**
�^�uView��Update
*/
void	CWindowImp::UpdateTab()
{
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�����Ȃ�
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		//�E�C���h�E�����ς݃`�F�b�N
		if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
		
		AControlRef	tabViewRootControlRef = GetControlRef('ttab',0);
		if( tabViewRootControlRef == NULL )   return;
		
		//�L�[�{�[�h�t�H�[�J�X����U�N���A
		ClearFocus();
		
		//�I�����ꂽ�^�u�̃C���f�b�N�X���擾
		AIndex	selectedTabIndex = ::GetControl32BitValue(tabViewRootControlRef)-1;
		//�I������Ă��Ȃ��^�u���\��
		ANumber	tabMax = ::GetControl32BitMaximum(tabViewRootControlRef);
		for( AIndex tabIndex = 0; tabIndex < tabMax; tabIndex++ )
		{
			AControlRef	tabControlRef = GetTabControlRef(tabIndex);
			if( tabIndex != selectedTabIndex && tabControlRef != NULL )
			{
				//�^�u���Ɣ�\��
				err = ::SetControlVisibility(tabControlRef,false,false);
				if( err != noErr )   _ACErrorNum("SetControlVisibility() returned error: ",err,this);
			}
		}
		//�I�����ꂽ�^�u��\��
		AControlRef	selectedTabRef = GetTabControlRef(selectedTabIndex);
		if( selectedTabRef != NULL )
		{
			//�^�u���ƕ\��
			err = ::SetControlVisibility(selectedTabRef,true,true);
			if( err != noErr )   _ACErrorNum("SetControlVisibility() returned error: ",err,this);
		}
		::Draw1Control(tabViewRootControlRef);
		
		//Tab��Show/Hide�ɍ��킹�āAMLTEPane��Show/Hide����
		//�iHide����Tab�Ɋ܂܂��MLTE�̃X�N���[���o�[�������Ă��܂����̑΍�j
		for( AIndex i = 0; i < mUserPaneArray_ControlID.GetItemCount(); i++ )
		{
			AControlID	controlID = mUserPaneArray_ControlID.Get(i);
			if( GetUserPaneType(controlID) == kUserPaneType_MLTEPane )
			{
				ABool	show = IsEmbededInTabView(controlID,selectedTabIndex);
				SetShowControl(controlID,show);
			}
		}
		
		//�L�[�{�[�h�t�H�[�J�X��ݒ�
		GetAWin().NVI_SwitchFocusToFirst();
	}
#endif
}

//�w��ControlID�̃R���g���[�����w��Tab�Ɋ܂܂�Ă��邩���擾����
ABool	CWindowImp::IsEmbededInTabView( const AControlID inChildControlID, const AIndex inTabIndex ) const
{
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//�����Ȃ�
		return false;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		OSStatus	err = noErr;
		
		//�E�C���h�E�����ς݃`�F�b�N
		if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
		
		if( inChildControlID == kControlID_Invalid )   return false;
		AControlRef	current = GetControlRef(inChildControlID);
		while( current != NULL )
		{
			//ControlRef��Tab��ControlRef�Ɉ�v���邩�ǂ�������
			if( current == GetTabControlRef(inTabIndex) )
			{
				return true;
			}
			//�e�R���g���[�����擾
			AControlRef	parent;
			err = ::GetSuperControl(current,&parent);
			//if( err != noErr )   _ACErrorNum("GetSuperControl() returned error: ",err,this);
			current = parent;
		}
		return false;
	}
#endif
}

#if SUPPORT_CARBON
/**
�w��TabIndex(0�`)�̃^�u��ControlRef���擾����
*/
AControlRef	CWindowImp::GetTabControlRef( const AIndex inTabIndex ) const
{
	return GetControlRef('ttab',inTabIndex+1);
}
#endif

//B0406
/**
�^�uControl�I��
*/
void	CWindowImp::SetTabControlValue( const AIndex inTabIndex )
{
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//tab view item�擾
		NSTabView*	tabView = [CocoaObjects->GetWindow() getTabView];
		[tabView selectTabViewItemAtIndex:inTabIndex];
		
		//�^�u��I��
		//�itab view item��identifier�ɂ́A�Ή�����c�[���o�[���ڂ�control id(tag)�������Ă���B�j
		AText	numtext;
		ACocoa::SetTextFromNSString([[CocoaObjects->GetWindow() toolbar] selectedItemIdentifier],numtext);
		if( numtext.GetNumber() != kControlID_FirstTabSelectorToolbarItem+inTabIndex )
		{
			numtext.SetNumber(kControlID_FirstTabSelectorToolbarItem+inTabIndex);
			AStCreateNSStringFromAText	numstr(numtext);
			[[CocoaObjects->GetWindow() toolbar] setSelectedItemIdentifier:numstr.GetNSString()];
		}
		
		//�Ή�����bottom marker�ɏ]���āA�E�C���h�E�T�C�Y���X�V
		OptimizeWindowBounds(kControlID_FirstTabBottoMarker+inTabIndex,kControlID_Invalid);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		AControlRef	tabViewRootControlRef = GetControlRef('ttab',0);
		if( tabViewRootControlRef != NULL )
		{
			::SetControl32BitValue(tabViewRootControlRef,inTabIndex+1);
			UpdateTab();
		}
	}
#endif
}

#pragma mark ===========================

#pragma mark --- �c�[���o�[

#pragma mark ===========================

#pragma mark --- WebView

//#734
/**
WebView����
*/
void	CWindowImp::CreateWebView( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	//�E�C���h�E���̐����ʒuframe���v�Z
	AWindowRect	wrect = {0};
	wrect.left	= inPoint.x;
	wrect.top	= inPoint.y;
	wrect.right	= inPoint.x + inWidth;
	wrect.bottom= inPoint.y + inHeight;
	NSRect	rect = ConvertFromWindowRectToNSRect(CocoaObjects->GetContentView(),wrect);
	//webview����
	CocoaWebView*	webview = [[CocoaWebView alloc] initWithFrame:rect frameName:nil groupName:nil withControlID:inID];
	[CocoaObjects->GetContentView() addSubview:webview positioned:NSWindowAbove relativeTo:nil];
	[webview release];//addSubview:��content view��retain����A�A�v�����Ń|�C���^���ێ����Ȃ��̂ł������release����B
	/*test�i�r���[�S�̂̊g��k���j
	NSRect	bounds = [webview bounds];
	NSRect	frame = [webview frame];
	NSRect	fr = {0};
	fr.origin.x = 0;
	fr.origin.y = 0;
	fr.size = frame.size;
	[[[webview mainFrame] frameView] setFrame:fr];
	NSRect	bounds2 = [[[webview mainFrame] frameView] bounds];
	NSRect	frame2 = [[[webview mainFrame] frameView] frame];
	//bounds2.size.width = frame2.size.width*1.7;
	//bounds2.size.height = frame2.size.height*1.7;
	[[[webview mainFrame] frameView] setBounds:bounds2];
	*/
}

//#734
/**
WebView��URL�ݒ肵�A���[�h�v��
*/
void	CWindowImp::LoadURLToWebView( const AControlID inID, const AText& inURL )
{
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//loadRequest���M
	AText	url;
	url.SetText(inURL);
	url.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToHFSPlusDecomp(url);//url��HFS decomp�ɕϊ�
	url.ConvertToUTF8FromUTF16();
	url.ConvertToURLEncode();//url�G���R�[�h
	AStCreateNSStringFromAText	str(url);
	[[view mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:str.GetNSString()]]];
}

//#734
/**
WebView�����[�h�v��
*/
void	CWindowImp::ReloadWebView( const AControlID inID )
{
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	/*�e�X�g
	�{���ݒ�
	NSRect	bounds = [view bounds];
	NSRect	frame = [view frame];
	NSRect	bounds2 = [[[view mainFrame] frameView] bounds];
	NSRect	frame2 = [[[view mainFrame] frameView] frame];
	bounds2.size.width = frame2.size.width*1.7;
	bounds2.size.height = frame2.size.height*1.7;
	[[[view mainFrame] frameView] setBounds:bounds2];
	*/
	
	//�����[�h�v��
	[view reload:nil];
}

//#734
/**
WebView���݂�URL���擾
*/
void	CWindowImp::GetCurrentWebViewURL( const AControlID inID, AText& outURL ) const
{
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//URL�擾
	ACocoa::SetTextFromNSString([view mainFrameURL],outURL);
}

//#734
/**
Web view����JavaScript�����s
*/
void	CWindowImp::ExecuteJavaScriptInWebView( const AControlID inID, const AText& inText )
{
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	if( [view isKindOfClass:[CocoaWebView class]] == YES )
	{
		CocoaWebView*	webview = static_cast<CocoaWebView*>(view);
		//URL�擾
		AStCreateNSStringFromAText	str(inText);
		NSString*	res = [webview stringByEvaluatingJavaScriptFromString:str.GetNSString()];
	}
	else
	{
		_ACError("",NULL);
	}
}

//#734
/**
WebView�̃t�H���g�T�C�Y�i�g�嗦�j�ݒ�
*/
void	CWindowImp::SetWebViewFontMultiplier( const AControlID inID, const AFloatNumber inMultiplier )
{
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	if( [view isKindOfClass:[CocoaWebView class]] == YES )
	{
		CocoaWebView*	webview = static_cast<CocoaWebView*>(view);
		//URL�擾
		[webview setTextSizeMultiplier:inMultiplier];
	}
	else
	{
		_ACError("",NULL);
	}
}

#pragma mark ===========================

#pragma mark --- UserPane�Ǘ�
/**
�w��ControlID�̃R���g���[����UserPane���ǂ����𔻒�
*/
ABool	CWindowImp::IsUserPane( const AControlID inControlID ) const
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	return (index != kIndex_Invalid);
}

/**
�w��ControlID��UserPane���擾
*/
CUserPane&	CWindowImp::GetUserPane( const AControlID inControlID )
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )   _ACThrow("no UserPane",this);
	return mUserPaneArray.GetObject(index);
}

/**
�w��ControlID��UserPane���擾(const)
*/
const CUserPane&	CWindowImp::GetUserPaneConst( const AControlID inControlID ) const
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )   _ACThrow("no UserPane",this);
	return mUserPaneArray.GetObjectConst(index);
}

/**
�w��ControlID��UserPane�^�C�v���擾
*/
AUserPaneType	CWindowImp::GetUserPaneType( const AControlID inControlID ) const
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )   return kUserPaneType_Invalid;
	return mUserPaneArray_Type.Get(index);
}

#if SUPPORT_CARBON
/**
MLTEPane��o�^�iOS�̃E�C���h�E��ɂ͐����ς݁j
*/
void	CWindowImp::RegisterTextEditPane( const AControlID inID, const ABool inWrapMode , const ABool inVScrollbar, const ABool inHScrollBar , const ABool inHasFrame )
{
	AControlRef	controlRef = GetControlRef(inID);//#688
	CMLTEPaneFactory	factory(this,*this,/*#688GetControlRef(inID)*/inID,inWrapMode,inVScrollbar,inHScrollBar,inHasFrame);
	mUserPaneArray.AddNewObject(factory);
	mUserPaneArray_ControlID.Add(inID);
	mUserPaneArray_Type.Add(kUserPaneType_MLTEPane);
	mUserPaneArray_ControlRef.Add(controlRef);//#688
	
	//�L�[�{�[�h�t�H�[�J�X�O���[�v�֓o�^
	//#135 AWindow�Ŏ��s RegisterFocusGroup(inID);
}
#endif

//B0323
/**
Undo���N���A
*/
void	CWindowImp::ResetUndoStack( const AControlID inID )
{
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).ResetUndoStack();
	}
	//�����Ȃ�
}

/**
ColorPickerPane��o�^�iOS�̃E�C���h�E��ɂ͐����ς݁j
*/
void	CWindowImp::RegisterColorPickerPane( const AControlID inID )
{
	if( true )
	{
		//�����Ȃ�
	}
#if SUPPORT_CARBON
	else
	{
		AControlRef	controlRef = GetControlRef(inID);//#688
		CColorPickerPaneFactory	factory(this,*this,/*#688GetControlRef(inID)*/inID);
		mUserPaneArray.AddNewObject(factory);
		mUserPaneArray_ControlID.Add(inID);
		mUserPaneArray_Type.Add(kUserPaneType_ColorPickerPane);
		mUserPaneArray_ControlRef.Add(controlRef);//#688
	}
#endif
}

/**
AView�pUserPane��o�^�iOS�̃E�C���h�E��ɂ͐����ς݁j
*/
void	CWindowImp::RegisterPaneForAView( const AControlID inID, const ABool inSupportInputMethod )//#688
{
	if( true )
	{
		//IB�Őݒ肳��Ă���view���擾
		NSView*	oldView = CocoaObjects->FindViewByTag(inID);
		//�eview���擾
		NSView*	superView = [oldView superview];
		//frame���擾
		NSRect	frame = [oldView frame];
		//autoresize�t���O���擾
		NSInteger	autoresize = [oldView autoresizingMask];
		//��view���폜
		[oldView removeFromSuperview];//removeFromSuperview��release�����s����B�A�v�����ł�retain���Ă��Ȃ��̂ŁA�����ŉ�������B
		
		//user pane�𐶐�
		CocoaUserPaneView*	view = nil;
		if( inSupportInputMethod == false )
		{
			//TextInputClient�Ȃ���view�𐶐�
			view = [[CocoaUserPaneView alloc] initWithFrame:frame windowImp:this controlID:inID];
		}
		else
		{
			//TextInputClient: Mac OS X 10.5�ȍ~
			//#0 Mac OS X 10.5�����͖��Ή��Ƃ��� if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
			{
				//TextInputClient�����view�𐶐�
				view = [[CocoaUserPaneTextInputClientView alloc] initWithFrame:frame windowImp:this controlID:inID];
			}
			/*
			else
			{
				//TextInput�����view�𐶐�
				view = [[CocoaUserPaneTextInputView alloc] initWithFrame:frame windowImp:this controlID:inID];
			}
			*/
			//
			[view setNeedsPanelToBecomeKey:true];
		}
		//�eview�֓o�^
		[superView addSubview:view positioned:NSWindowAbove relativeTo:nil];
		//autoresize�t���O��ݒ�
		[view setAutoresizingMask:autoresize];
		
		//view�̃|�C���^��ێ�����̂ŁA�A�v������1��retain�������(alloc�ɂ��retain)�̂܂܂ɂ���B
		//mCocoaUserPaneViewArray_UserPaneView����Delete����Ƃ��ɁAview��release����B
		CocoaObjects->RegisterUserPaneView(inID,view);
		//CocoaObjects����retain����̂ŁA�������release����B
		[view release];
		
		//UserPaneArray�����A�o�^
		CPaneForAViewFactory	factory(this,*this,inID);
		mUserPaneArray.AddNewObject(factory);
		mUserPaneArray_ControlID.Add(inID);
		mUserPaneArray_Type.Add(kUserPaneType_PaneForAView);
		//#688 mUserPaneArray_ControlRef.Add(NULL);
		
		//Overlay/Floating�E�C���h�E��ɍ쐬����ꍇ�́A�ŏ��̃}�E�X�N���b�N����������悤�ݒ肷��
		if( mWindowClass == kWindowClass_Overlay || (mWindowClass == kWindowClass_Floating && mNonFloatingFloating == false) )//#1133
		{
			[view setAcceptsFirstMouse:YES];
		}
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//#205 ���ł�UserPane�ȊO�̃R���g���[�������݂��Ă���ꍇ�́A������폜���ē����ʒu��UserPaneControl���쐬����
		if( GetControlRef(inID) != NULL )
		{
			//�R���g���[����ʎ擾
			ControlKind	kind;
			err = ::GetControlKind(GetControlRef(inID),&kind);
			if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
			if( kind.kind != kControlKindUserPane )
			{
				//���R���g���[���̏����擾
				AControlRef	oldControlRef = GetControlRef(inID);
				AControlRef	parent;
				err = ::GetSuperControl(oldControlRef,&parent);
				if( err != noErr )   _ACErrorNum("GetSuperControl() returned error: ",err,this);
				Rect	rect;
				::GetControlBounds(oldControlRef,&rect);
				//���R���g���[���폜
				::DisposeControl(oldControlRef);
				
				//UserPaneControl����
				AControlRef	controlRef;
				err = ::CreateUserPaneControl(mWindowRef,&rect,/*#182 0*/kControlSupportsEmbedding,&controlRef);
				if( err != noErr )   _ACErrorNum("CreateUserPaneControl() returned error: ",err,this);
				//�ʒu���ݒ�
				::EmbedControl(controlRef,parent);
				ControlID	controlID;
				controlID.signature = 0;
				controlID.id = inID;
				err = ::SetControlID(controlRef,&controlID);
				if( err != noErr )   _ACErrorNum("SetControlID() returned error: ",err,this);
				err = ::SetControlCommandID(controlRef,inID);
				if( err != noErr )   _ACErrorNum("SetControlCommandID() returned error: ",err,this);
				err = ::HIViewSetDrawingEnabled(controlRef,true);
				if( err != noErr )   _ACErrorNum("HIViewSetDrawingEnabled() returned error: ",err,this);
			}
		}
		AControlRef	controlRef = GetControlRef(inID);//#688
		CPaneForAViewFactory	factory(this,*this,/*#688GetControlRef(inID)*/inID);
		mUserPaneArray.AddNewObject(factory);
		mUserPaneArray_ControlID.Add(inID);
		mUserPaneArray_Type.Add(kUserPaneType_PaneForAView);
		mUserPaneArray_ControlRef.Add(controlRef);//#688
		/*#135
		if( inSupportFocus == true )
		{
		RegisterFocusGroup(inID);
		}
		*/
	}
#endif
}

/**
AView�pUserPane�𐶐�
*/
void	CWindowImp::CreatePaneForAView( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, 
		const AControlID inParentControlID, const AControlID inSiblingControlID, //#688
		const AIndex inTabIndex, const ABool inFront, const ABool inSupportInputMethod )//#688 win 080618
{
	//#688
	if( true )
	{
		//�E�C���h�E���̐����ʒuframe���v�Z
		AWindowRect	wrect = {0};
		wrect.left	= inPoint.x;
		wrect.top	= inPoint.y;
		wrect.right	= inPoint.x + inWidth;
		wrect.bottom= inPoint.y + inHeight;
		NSRect	rect = ConvertFromWindowRectToNSRect(CocoaObjects->GetContentView(),wrect);
		//user pane����
		CocoaUserPaneView*	view = nil;
		if( inSupportInputMethod == false )
		{
			//TextInputClient�Ȃ���view�𐶐�
			view = [[CocoaUserPaneView alloc] initWithFrame:rect windowImp:this controlID:inID];
		}
		else
		{
			//TextInputClient: Mac OS X 10.5�ȍ~
			//#0 Mac OS X 10.5�����͖��Ή��Ƃ��� if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
			{
				//TextInputClient�����view�𐶐�
				view = [[CocoaUserPaneTextInputClientView alloc] initWithFrame:rect windowImp:this controlID:inID];
			}
			/*
			else
			{
				//TextInput�����view�𐶐�
				view = [[CocoaUserPaneTextInputView alloc] initWithFrame:rect windowImp:this controlID:inID];
			}
			*/
			//
			[view setNeedsPanelToBecomeKey:true];
		}
		//autoresize�t���O��ݒ�
		//��resize mask�ݒ肷��ƁA�c�[���o�[�̃{�^���������������Ȃ��Ă��܂�[view setAutoresizingMask:(NSViewWidthSizable|NSViewHeightSizable)];
		
		//�eview�֓o�^
		if( inParentControlID != kControlID_Invalid )
		{
			//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
			NSView*	parentView = CocoaObjects->FindViewByTag(inParentControlID);
			
			//�����Ɏw�肳�ꂽ�e�r���[�֓o�^
			[parentView addSubview:view positioned:NSWindowAbove relativeTo:nil];
		}
		else if( inSiblingControlID != kControlID_Invalid )
		{
			//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
			NSView*	siblingView = CocoaObjects->FindViewByTag(inSiblingControlID);
			
			//�����Ɏw�肳�ꂽ�e�r���[�֓o�^
			[[siblingView superview] addSubview:view positioned:NSWindowAbove relativeTo:nil];
		}
		else
		{
			//contentView��e�r���[�Ƃ��ēo�^
			[CocoaObjects->GetContentView() addSubview:view positioned:NSWindowAbove relativeTo:nil];
		}
		//�R���g���[��ID�L���b�V���ɋL���icontrol id, view���킩���Ă���̂ŁA�����œo�^�������������j
		CocoaObjects->AddToControlIDCache(inID,view);
		//
		SetControlPosition(inID,inPoint);
		
		//view�̃|�C���^��ێ�����̂ŁA�A�v������1��retain�������(alloc�ɂ��retain)�̂܂܂ɂ���B
		//mCocoaUserPaneViewArray_UserPaneView����Delete����Ƃ��ɁAview��release����B
		CocoaObjects->RegisterUserPaneView(inID,view);
		//CocoaObjects����retain����̂ŁA�������release����B
		[view release];
		
		//UserPaneArray�����A�o�^
		CPaneForAViewFactory	factory(this,*this,inID);
		mUserPaneArray.AddNewObject(factory);
		mUserPaneArray_ControlID.Add(inID);
		mUserPaneArray_Type.Add(kUserPaneType_PaneForAView);
		//#688 mUserPaneArray_ControlRef.Add(NULL);
		
		//Overlay/Floating�E�C���h�E��ɍ쐬����ꍇ�́A�ŏ��̃}�E�X�N���b�N����������悤�ݒ肷��
		if( mWindowClass == kWindowClass_Overlay || (mWindowClass == kWindowClass_Floating&& mNonFloatingFloating == false) )//#1133
		{
			[view setAcceptsFirstMouse:YES];
		}
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		Rect	rect;
		rect.left = inPoint.x;
		rect.top = inPoint.y;
		rect.right = rect.left + inWidth;
		rect.bottom = rect.top + inHeight;
		/*#360
		UInt32	features = 0;
		if( inSupportFocus == true )
		{
		features = kControlSupportsFocus;
		}
		*/
		AControlRef	controlRef;
		err = ::CreateUserPaneControl(mWindowRef,&rect,/*#182 0*/kControlSupportsEmbedding,&controlRef);
		if( err != noErr )   _ACErrorNum("CreateUserPaneControl() returned error: ",err,this);
		ControlID	controlID;
		controlID.signature = 0;
		controlID.id = inID;
		err = ::SetControlID(controlRef,&controlID);
		if( err != noErr )   _ACErrorNum("SetControlID() returned error: ",err,this);
		err = ::SetControlCommandID(controlRef,inID);
		if( err != noErr )   _ACErrorNum("SetControlCommandID() returned error: ",err,this);
		err = ::HIViewSetDrawingEnabled(controlRef,true);
		if( err != noErr )   _ACErrorNum("HIViewSetDrawingEnabled() returned error: ",err,this);
		
		CPaneForAViewFactory	factory(this,*this,inID);//#688GetControlRef(inID));
		mUserPaneArray.AddNewObject(factory);
		mUserPaneArray_ControlID.Add(inID);
		mUserPaneArray_Type.Add(kUserPaneType_PaneForAView);
		mUserPaneArray_ControlRef.Add(controlRef);//#688
		
		/*#135
		if( inSupportFocus == true )
		{
		RegisterFocusGroup(inID);
		}
		*/
		//win 080618
		if( inTabIndex != kIndex_Invalid )
		{
			::EmbedControl(controlRef,GetTabControlRef(inTabIndex));
			SetControlPosition(inID,inPoint);
		}
		//win 080618
		if( inFront == true )
		{
			err = ::HIViewSetZOrder(controlRef,kHIViewZOrderAbove,NULL);
			if( err != noErr )   _ACErrorNum("HIViewSetZOrder() returned error: ",err,this);
		}
		else
		{
			err = ::HIViewSetZOrder(controlRef,kHIViewZOrderBelow,NULL);
			if( err != noErr )   _ACErrorNum("HIViewSetZOrder() returned error: ",err,this);
		}
		
		//#291 UserPane��SupportFocus�t���O��ݒ�
		//#360 GetUserPane(inID).SetSupportFocus(inSupportFocus);
	}
#endif
}

#if SUPPORT_CARBON
/**
UserPane�̒l���ύX���ꂽ�Ƃ���CUserPane����̃R�[���o�b�N
*/
void	CWindowImp::CB_UserPaneValueChanged( const AControlID inID )
{
	GetAWin().EVT_ValueChanged(inID);
}

//B0322
/**
UserPane��Text���͂��ꂽ�Ƃ���CUserPane����̃R�[���o�b�N
*/
void	CWindowImp::CB_UserPaneTextInputted( const AControlID inID )
{
	GetAWin().EVT_TextInputted(inID);
}
#endif

//#205 #if USE_IMP_TABLEVIEW
#pragma mark ===========================

#pragma mark ---�e�[�u��View�Ǘ�

#if SUPPORT_CARBON
/**
TableView�擾
*/
CTableView&	CWindowImp::GetTableViewByID( const AControlID inControlID )
{
	if( GetUserPaneType(inControlID) != kUserPaneType_TableView )   _ACThrow("not table view",this);
	return dynamic_cast<CTableView&>(GetUserPane(inControlID));
}

/**
TableView�擾
*/
const CTableView&	CWindowImp::GetTableViewConstByID( const AControlID inControlID ) const
{
	if( GetUserPaneType(inControlID) != kUserPaneType_TableView )   _ACThrow("not table view",this);
	return dynamic_cast<const CTableView&>(GetUserPaneConst(inControlID));
}
#endif

/**
TableView���ǂ���
*/
ABool	CWindowImp::IsTableView( const AControlID inControlID ) const
{
	if( true )
	{
		return false;
	}
#if SUPPORT_CARBON
	else
	{
		if( IsUserPane(inControlID) == false )   return false;
		if( GetUserPaneType(inControlID) != kUserPaneType_TableView )   return false;
		return true;
	}
#endif
}

/**
TableView�o�^
*/
void	CWindowImp::RegisterTableView( const AControlID inControlID, const AControlID inUp, const AControlID inDown,
		const AControlID inTop, const AControlID inBottom )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		AControlRef	controlRef = GetControlRef(inControlID);//#688
		CTableViewFactory	factory(this,*this,inControlID);//#688GetControlRef(inControlID));
		mUserPaneArray.AddNewObject(factory);
		mUserPaneArray_ControlID.Add(inControlID);
		mUserPaneArray_Type.Add(kUserPaneType_TableView);
		mUserPaneArray_ControlRef.Add(controlRef);//#688
		mTableViewRowUp_ButtonControlID.Add(inUp);
		mTableViewRowUp_TableControlID.Add(inControlID);
		mTableViewRowDown_ButtonControlID.Add(inDown);
		mTableViewRowDown_TableControlID.Add(inControlID);
		mTableViewRowTop_ButtonControlID.Add(inTop);
		mTableViewRowTop_TableControlID.Add(inControlID);
		mTableViewRowBottom_ButtonControlID.Add(inBottom);
		mTableViewRowBottom_TableControlID.Add(inControlID);
	}
#endif
}

/**
���o�^
*/
void	CWindowImp::RegisterTableViewColumn( const AControlID inControlID, const APrefID inColumnID, const ANumber inDataType )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		switch(inDataType)
		{
		  case kDataType_TextArray:
			{
				GetTableViewByID(inControlID).RegisterColumn_Text(inColumnID);
				break;
			}
		  case kDataType_BoolArray:
			{
				GetTableViewByID(inControlID).RegisterColumn_Bool(inColumnID);
				break;
			}
		  case kDataType_NumberArray:
			{
				GetTableViewByID(inControlID).RegisterColumn_Number(inColumnID);
				break;
			}
		}
	}
#endif
}

//�e�[�u���f�[�^�S�ݒ�

/**
�ݒ�J�n���ɌĂ�
*/
void	CWindowImp::BeginSetTable( const AControlID inControlID )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).BeginSetTable();
	}
#endif
}

/**
��f�[�^��ݒ�(Text)
*/
void	CWindowImp::SetTable_Text( const AControlID inControlID, const APrefID inColumnID, const ATextArray& inTextArray )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).SetTable_Text(inColumnID,inTextArray);
	}
#endif
}

/**
��f�[�^��ݒ�(Bool)
*/
void	CWindowImp::SetTable_Bool( const AControlID inControlID, const APrefID inColumnID, const ABoolArray& inBoolArray )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).SetTable_Bool(inColumnID,inBoolArray);
	}
#endif
}

/**
��f�[�^��ݒ�(Number)
*/
void	CWindowImp::SetTable_Number( const AControlID inControlID, const APrefID inColumnID, const ANumberArray& inNumberArray )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).SetTable_Number(inColumnID,inNumberArray);
	}
#endif
}

/**
��f�[�^��ݒ�(Color)
*/
void	CWindowImp::SetTable_Color( const AControlID inControlID, const APrefID inColumnID, const AColorArray& inColorArray )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).SetTable_Color(inColumnID,inColorArray);
	}
#endif
}

//��f�[�^��ݒ�(IsContainer)
/*�K�w���[�h�����r��void	CWindowImp::SetTable_IsContainer( const AControlID inControlID, const ABoolArray& inIsContaner )
{
	GetTableViewByID(inControlID).SetTable_IsContainer(inIsContaner);
}*/

/**
�S��ݒ芮�����ɌĂ�
*/
void	CWindowImp::EndSetTable( const AControlID inControlID )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).EndSetTable();
	}
#endif
}

//�s�ǉ�

/**
�s�ǉ��J�n���ɌĂ�
*/
void	CWindowImp::BeginInsertRows( const AControlID inControlID )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).BeginInsertRows();
	}
#endif
}

/**
�s�i1�Z���j�ǉ�(Text)
*/
void	CWindowImp::InsertRow_Text( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, const AText& inText )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).InsertRow_Text(inColumnID,inIndex,inText);
	}
#endif
}
void	CWindowImp::InsertRow_Text_SwapContent( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, AText& ioText )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).InsertRow_Text_SwapContent(inColumnID,inIndex,ioText);
	}
#endif
}

/**
�s�i1�Z���j�ǉ�(Number)
*/
void	CWindowImp::InsertRow_Number( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, const ANumber inNumber )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).InsertRow_Number(inColumnID,inIndex,inNumber);
	}
#endif
}

//�s�i1�Z���j�ǉ�(IsContainer)
/*�K�w���[�h�����r��void	CWindowImp::InsertRow_IsContainer( const AControlID inControlID, const AIndex inIndex, const ABool inIsContainer )
{
	GetTableViewByID(inControlID).InsertRow_IsContainer(inIndex,inIsContainer);
}*/

/**
�s�ǉ��������ɌĂ�
*/
void	CWindowImp::EndInsertRows( const AControlID inControlID, const AIndex inRowIndex, const AIndex inRowCount )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).EndInsertRows(inRowIndex,inRowCount);
	}
#endif
}

/**
TableView�X�V
*/
void	CWindowImp::UpdateTableView( const AControlID inControlID )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).Update();
	}
#endif
}

/**
TableView�Z���ҏW���[�h
*/
void	CWindowImp::EnterColumnEditMode( const AControlID inControlID, const ADataID inColumnID )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).EnterEditMode(inColumnID);
	}
#endif
}

/**
�A�C�R���ݒ�i��P�ʁj
*/
void	CWindowImp::SetColumnIcon( const AControlID inControlID, const ADataID inColumnID, const ANumberArray& inIconIDArray )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).SetColumnIcon(inColumnID,inIconIDArray);
	}
#endif
}

/**
�e�L�X�g�ݒ�i��P�ʁj
*/
const ATextArray&	CWindowImp::GetColumn_TextArray( const AControlID inControlID, const APrefID inColumnID ) const
{
	if( true )
	{
		return GetEmptyTextArray();
	}
#if SUPPORT_CARBON
	else
	{
		return GetTableViewConstByID(inControlID).GetColumn_TextArray(inColumnID);
	}
#endif
}

/**
Bool�ݒ�i��P�ʁj
*/
const ABoolArray&	CWindowImp::GetColumn_BoolArray( const AControlID inControlID, const APrefID inColumnID ) const
{
	if( true )
	{
		return GetEmptyBoolArray();
	}
#if SUPPORT_CARBON
	else
	{
		return GetTableViewConstByID(inControlID).GetColumn_BoolArray(inColumnID);
	}
#endif
}

/**
Number�ݒ�i��P�ʁj
*/
const ANumberArray&	CWindowImp::GetColumn_NumberArray( const AControlID inControlID, const APrefID inColumnID ) const
{
	if( true )
	{
		return GetEmptyNumberArray();
	}
#if SUPPORT_CARBON
	else
	{
		return GetTableViewConstByID(inControlID).GetColumn_NumberArray(inColumnID);
	}
#endif
}

/**
�F�ݒ�i��P�ʁj
*/
const AColorArray&	CWindowImp::GetColumn_ColorArray( const AControlID inControlID, const ADataID inColumnID ) const
{
	if( true )
	{
		return GetEmptyColorArray();
	}
#if SUPPORT_CARBON
	else
	{
		return GetTableViewConstByID(inControlID).GetColumn_ColorArray(inColumnID);
	}
#endif
}

/**
��̕��ݒ�
*/
void	CWindowImp::SetColumnWidth( const AControlID inControlID, const APrefID inColumnID, const ANumber inWidth )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).SetColumnWidth(inColumnID,inWidth);
	}
#endif
}

/**
��̕��擾
*/
ANumber	CWindowImp::GetColumnWidth( const AControlID inControlID, const APrefID inColumnID ) const
{
	if( true )
	{
		return 0;
	}
#if SUPPORT_CARBON
	else
	{
		return GetTableViewConstByID(inControlID).GetColumnWidth(inColumnID);
	}
#endif
}

/**
TableView�t�H���g�ݒ�
*/
void	CWindowImp::SetTableFont( const AControlID inControlID, /*win const AFont inFont*/const AText& inFontName, const AFloatNumber inFontSize )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).SetFont(/*win inFont*/inFontName,inFontSize);
	}
#endif
}

//#11
/**
TableView��0�������͂��֎~
*/
void	CWindowImp::SetInhibit0LengthForTable( const AControlID inControlID, const ABool inInhibit0Length )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).SetInhibit0Length(inInhibit0Length);
	}
#endif
}

/**
TableView�̃X�N���[���ʒu�ݒ�
*/
void	CWindowImp::SetTableScrollPosition( const AControlID inControlID, const APoint& inPoint )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).SetScrollPosition(inPoint);
	}
#endif
}

/**
TableView�̃X�N���[���ʒu�擾
*/
void	CWindowImp::GetTableScrollPosition( const AControlID inControlID, APoint& outPoint ) const
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewConstByID(inControlID).GetScrollPosition(outPoint);
	}
#endif
}

//B0367
/**
TableView��ҏW�\���ǂ����ݒ�
*/
void	CWindowImp::SetTableEditable( const AControlID inControlID, const ABool inEditable )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetTableViewByID(inControlID).SetEditable(inEditable);
	}
#endif
}

/**
TableView�̍s�̒l�ύX���̃R�[���o�b�N
*/
void	CWindowImp::CB_TableViewChanged( const AControlID inControlID )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetAWin().EVT_ValueChanged(inControlID);
	}
#endif
}

/**
TableView�̍s�_�u���N���b�N���̃R�[���o�b�N
*/
void	CWindowImp::CB_TableViewDoubleClicked( const AControlID inControlID )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetAWin().EVT_DoubleClicked(inControlID);
	}
#endif
}

/**
TableView�̃R�������̕ύX��Sort���̕ύX�Ȃǂ̏�ԕύX���̃R�[���o�b�N
*/
void	CWindowImp::CB_TableViewStateChanged( const AControlID inControlID )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetAWin().EVT_TableViewStateChanged(inControlID);
	}
#endif
}

//#353
/**
TableView�̑I�����ڕύX���̃R�[���o�b�N
*/
void	CWindowImp::CB_TableViewSelectionChanged( const AControlID inControlID )
{
	if( true )
	{
		
	}
#if SUPPORT_CARBON
	else
	{
		GetAWin().EVT_Clicked(inControlID,0);
	}
#endif
}

//#205 #endif

#pragma mark ===========================

#pragma mark ---�|�b�v�A�b�v���j���[�Ǘ�

/**
�|�b�v�A�b�v���j���[��ATextArray�̓��e��ݒ�
*/
void	CWindowImp::SetMenuItemsFromTextArray( const AControlID inID, const ATextArray& inTextArray )
{
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSPopUpButton�̏ꍇ�ATextArray�̓��e�����j���[���ڂƂ��Ēǉ�
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			
			//���ڑ}���ʒu��������
			AIndex	insertIndex = 0;
			//
			if( [popup pullsDown] == YES )
			{
				//==================pull down���j���[�̏ꍇ==================
				if( AMenuWrapper::GetMenuItemCount([popup menu]) == 0 )
				{
					//�ŏ��̍��ځi�\���^�C�g���j�����݂��Ȃ��ꍇ�́A�ŏ��̍��ڂ�ǉ�
					AText	text;
					AMenuWrapper::AddMenuItem([popup menu],text,kMenuItemID_Invalid,0,0);
				}
				else
				{
					//�ŏ��̍��ڈȊO���폜�i�ŏ��̃��j���[���ڂɂ́A�c�[���o�[�̏ꍇ�A�v���_�E�����j���[�̃^�C�g���A�C�R�����ݒ肳��Ă���ꍇ������j
					while( AMenuWrapper::GetMenuItemCount([popup menu]) >= 2 )
					{
						AMenuWrapper::DeleteMenuItem([popup menu],AMenuWrapper::GetMenuItemCount([popup menu])-1);
					}
				}
				//���ڑ}���ʒu��1����
				insertIndex = 1;
			}
			else
			{
				//==================pull down�ł͂Ȃ����j���[�̏ꍇ==================
				//���j���[���ڑS�폜
				AMenuWrapper::DeleteAllMenuItems([popup menu]);
			}
			
			for( AIndex i = 0; i < inTextArray.GetItemCount(); i++ )
			{
				AText	text;
				inTextArray.Get(i,text);
				//���j���[���ڒǉ�
				//�iNSPopUpButton��addItemWithTitle�́A�����e�L�X�g������Ƃ�����폜���Ēǉ�����̂ŁANSMenu�̃��\�b�h���g���j
				/*
				AStCreateNSStringFromAText	str(text);
				[[popup menu] addItemWithTitle:str.GetNSString() action:NULL keyEquivalent:@""];
				*/
				
				//0x00�Ŏn�܂�e�L�X�g�̏ꍇ�́Adisable���ڂɂ���
				ABool	enable = true;
				if( text.GetItemCount() > 0 )
				{
					if( text.Get(0) == 0x00 )
					{
						enable = false;
						text.Delete1(0);
					}
				}
				//���ڑ}��
				AMenuWrapper::InsertMenuItem([popup menu],insertIndex,text,kMenuItemID_Invalid,0,0);
				//enable/disable�ݒ�
				AMenuWrapper::SetEnableMenuItem([popup menu],insertIndex,enable);
				//���ڑ}���ʒu������
				insertIndex++;
			}
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		ACarbonMenuWrapper::SetMenuItemsFromTextArray(GetMenuRef(inID),inTextArray);
		::SetControl32BitMaximum(GetControlRef(inID),ACarbonMenuWrapper::GetMenuItemCount(GetMenuRef(inID)));
	}
#endif
}

//win
/**
�|�b�v�A�b�v���j���[���ڒǉ�
*/
void	CWindowImp::InsertMenuItem( const AControlID inID, const AIndex inMenuItemIndex, const AText& inText )
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSPopUpButton�̏ꍇ�A���j���[���ڒǉ�
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			
			//���j���[���ڒǉ�
			//�iNSPopUpButton��addItemWithTitle�́A�����e�L�X�g������Ƃ�����폜���Ēǉ�����̂ŁANSMenu�̃��\�b�h���g���j
			/*
			AStCreateNSStringFromAText	str(inText);
			[[popup menu] insertItemWithTitle:str.GetNSString() action:NULL keyEquivalent:@"" atIndex:inMenuItemIndex];
			*/
			
			//���j���[���ڒǉ�
			AIndex	itemIndex = inMenuItemIndex;
			if( [popup pullsDown] == YES )
			{
				itemIndex++;
			}
			//�e�L�X�g�擾
			AText	text;
			text.SetText(inText);
			//0x00�Ŏn�܂�e�L�X�g�̏ꍇ�́Adisable���ڂɂ���
			ABool	enable = true;
			if( text.GetItemCount() > 0 )
			{
				if( text.Get(0) == 0x00 )
				{
					enable = false;
					text.Delete1(0);
				}
			}
			//���ڑ}��
			AMenuWrapper::InsertMenuItem([popup menu],itemIndex,text,kMenuItemID_Invalid,0,0);
			//enable/disable�ݒ�
			AMenuWrapper::SetEnableMenuItem([popup menu],itemIndex,enable);
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		ACarbonMenuWrapper::InsertMenuItem(GetMenuRef(inID),inMenuItemIndex,inText,0,0,0);
		::SetControl32BitMaximum(GetControlRef(inID),ACarbonMenuWrapper::GetMenuItemCount(GetMenuRef(inID)));
	}
#endif
}

//win
/**
�|�b�v�A�b�v���j���[���ڍ폜
*/
void	CWindowImp::DeleteMenuItem( const AControlID inID, const AIndex inMenuItemIndex )
{
	//#688 Cocoa�Ή�
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSPopUpButton�̏ꍇ�A���j���[���ڍ폜
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			//���j���[���ڍ폜
			AIndex	itemIndex = inMenuItemIndex;
			if( [popup pullsDown] == YES )
			{
				itemIndex++;
			}
			[[popup menu] removeItemAtIndex:itemIndex];
		}
		//��L�ȊO�̓G���[
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		ACarbonMenuWrapper::DeleteMenuItem(GetMenuRef(inID),inMenuItemIndex);
		::SetControl32BitMaximum(GetControlRef(inID),ACarbonMenuWrapper::GetMenuItemCount(GetMenuRef(inID)));
	}
#endif
}

/**
�|�b�v�A�b�v���j���[�Ƀt�H���g�ꗗ��ݒ�
*/
void	CWindowImp::RegisterFontMenu( const AControlID inID, const ABool inEnableDefaultFontItem )//#375
{
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		
		//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//
		//��������
	}
#if SUPPORT_CARBON
	else
	{
		AApplication::GetApplication().NVI_GetMenuManager().RegisterFontMenu(GetMenuRef(inID));//win ,GetControlRef(inID));
		//#375
		if( inEnableDefaultFontItem == true )
		{
			//mValueIndependentMenuArray_ControlID�ɓo�^�i���j���[�\�����e�ƁAGetText()/SetText()�ł̒l���قȂ邽�߁j
			AIndex	valueIndependentMenuArrayIndex = mValueIndependentMenuArray_ControlID.Add(inID);
			mValueIndependentMenuArray_ValueArray.AddNewObject();
			//�u�i�f�t�H���g�̃t�H���g�j�v�̃��j���[���ڂ�ǉ�
			AText	defaultFontTitle;
			defaultFontTitle.SetLocalizedText("DefaultFontTitle");
			ACarbonMenuWrapper::InsertMenuItem(GetMenuRef(inID),0,defaultFontTitle,0,0,0);
			AText	defaultFontValue("default");
			mValueIndependentMenuArray_ValueArray.GetObject(valueIndependentMenuArrayIndex).Add(defaultFontValue);
			//�e�t�H���g�̃��j���[���ڂ�mValueIndependentMenuArray_ValueArray�ɒǉ�
			AItemCount	count = ACarbonMenuWrapper::GetMenuItemCount(GetMenuRef(inID));
			for( AIndex i = 1; i < count; i++ )
			{
				AText	text;
				ACarbonMenuWrapper::GetMenuItemText(GetMenuRef(inID),i,text);
				mValueIndependentMenuArray_ValueArray.GetObject(valueIndependentMenuArrayIndex).Add(text);
			}
		}
		::SetControl32BitMaximum(GetControlRef(inID),ACarbonMenuWrapper::GetMenuItemCount(GetMenuRef(inID)));//win
	}
#endif
}

/**
�|�b�v�A�b�v���j���[�Ɏw��ID��TextArrayMenuID��ݒ�

@param inControlID �|�b�v�A�b�v���j���[�{�^��Control
@param inTextArrayMenuID ATextArrayMenuManager�֓o�^����ID
*/
void	CWindowImp::RegisterTextArrayMenu( const AControlID inControlID, const ATextArrayMenuID inTextArrayMenuID )
{
	//�C���X�^���X���z��֓o�^
	mTextArrayMenuArray_ControlID.Add(inControlID);
	/*#232
	mTextArrayMenuArray_TextArrayMenuID.Add(inTextArrayMenuID);
	//static�z��֓o�^
	sTextArrayMenu_MenuRef.Add(GetMenuRef(inControlID));
	sTextArrayMenu_ControlRef.Add(GetControlRef(inControlID));
	sTextArrayMenu_TextArrayMenuID.Add(inTextArrayMenuID);
	//���j���[�֌��݂�DB��TextArray�𔽉f
	ACarbonMenuWrapper::SetMenuItemsFromTextArray(GetMenuRef(inControlID),sTextArrayMenuDB.GetData_ConstTextArrayRef(inTextArrayMenuID));
	::SetControl32BitMaximum(GetControlRef(inControlID),sTextArrayMenuDB.GetItemCount_Array(inTextArrayMenuID));
	*/
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().RegisterControl(GetAWin().GetObjectID(),inControlID,inTextArrayMenuID);
}

/**
�|�b�v�A�b�v���j���[��TextArrayMenuID�ݒ������

@param inControlID �|�b�v�A�b�v���j���[Control
*/
void	CWindowImp::UnregisterTextArrayMenu( const AControlID inControlID )
{
	/*#232
	//win�Ή�
	//AMenuManger����ړ�
	AIndex	staticarrayindex = sTextArrayMenu_MenuRef.Find(GetMenuRef(inControlID));
	if( staticarrayindex == kIndex_Invalid )   {_ACError("",this);return;}
	//static�z�񂩂�폜
	sTextArrayMenu_MenuRef.Delete1(staticarrayindex);
	sTextArrayMenu_ControlRef.Delete1(staticarrayindex);
	sTextArrayMenu_TextArrayMenuID.Delete1(staticarrayindex);
	*/
	//�C���X�^���X�z�񂩂�폜
	AIndex	index = mTextArrayMenuArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )   {_ACError("",this);return;}
	mTextArrayMenuArray_ControlID.Delete1(index);
	//#232 mTextArrayMenuArray_TextArrayMenuID.Delete1(index);
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().UnregisterControl(GetAWin().GetObjectID(),inControlID);
}

/*#232 
//TextArray�̍X�V(static)
void	CWindowImp::UpdateTextArrayMenu( const ATextArrayMenuID inTextArrayMenuID, const ATextArray& inTextArray )
{
	//DB�ɔ��f
	sTextArrayMenuDB.SetData_TextArray(inTextArrayMenuID,inTextArray);
	
	//�o�^���̃��j���[�S�Ăɔ��f
	for( AIndex index = 0; index < sTextArrayMenu_MenuRef.GetItemCount(); index++ )
	{
		if( sTextArrayMenu_TextArrayMenuID.Get(index) == inTextArrayMenuID )
		{
			ACarbonMenuWrapper::SetMenuItemsFromTextArray(sTextArrayMenu_MenuRef.Get(index),inTextArray);
			::SetControl32BitMaximum(sTextArrayMenu_ControlRef.Get(index),sTextArrayMenuDB.GetItemCount_Array(inTextArrayMenuID));
		}
	}
}

//TextArray���j���[�o�^(DB�ǉ�)(static)
void	CWindowImp::RegisterTextArrayMenuID( const ATextArrayMenuID inTextArrayMenuID )
{
	sTextArrayMenuDB.CreateData_TextArray(inTextArrayMenuID,"","");
}
*/

/*#349
#pragma mark ===========================

#pragma mark --- ���W�I�{�^���O���[�v

**
���W�I�{�^���O���[�v����
*
void	CWindowImp::RegisterRadioGroup()
{
	mRadioGroup.AddNewObject();
}

**
�Ō�ɐ����������W�I�{�^���O���[�v�ɃR���g���[����ǉ�
*
void	CWindowImp::AddToLastRegisteredRadioGroup( const AControlID inID )
{
	if( mRadioGroup.GetItemCount() == 0 )   return;
	mRadioGroup.GetObject(mRadioGroup.GetItemCount()-1).Add(inID);
}

**
���W�I�{�^���O���[�v��Update�i�w�肵���R���g���[���ȊO�Afalse�ɐݒ肷��j
*
void	CWindowImp::UpdateRadioGroup( const AControlID inControlID )
{
	//�w��Control���܂܂�郉�W�I�{�^���O���[�v������
	AIndex	foundRadioGroup = kIndex_Invalid;
	AItemCount	radioGroupCount = mRadioGroup.GetItemCount();
	for( AIndex i = 0; i < radioGroupCount; i++ )
	{
		if( mRadioGroup.GetObjectConst(i).Find(inControlID) != kIndex_Invalid )
		{
			foundRadioGroup = i;
			break;
		}
	}
	//���W�I�u���[�v��������Ȃ���΃��^�[��
	if( foundRadioGroup == kIndex_Invalid )   return;
	
	//
	AItemCount	radioButtonCount = mRadioGroup.GetObject(foundRadioGroup).GetItemCount();
	for( AIndex j = 0; j < radioButtonCount; j++ )
	{
		AControlID	controlID = mRadioGroup.GetObject(foundRadioGroup).Get(j);
		if( controlID != inControlID )
		{
			SetBool(controlID,false);
		}
	}
}

**
���W�I�O���[�v�ɐݒ肳�ꂽ�{�^���̏ꍇ�AOn��disable�ɂ��ꂽ��A���̃{�^����On�ɂ���B
*
void	CWindowImp::UpdateRadioButtonDisabled( const AControlID inControlID )
{
	//�w��Control���܂܂�郉�W�I�{�^���O���[�v������
	AIndex	foundRadioGroup = kIndex_Invalid;
	AItemCount	radioGroupCount = mRadioGroup.GetItemCount();
	for( AIndex i = 0; i < radioGroupCount; i++ )
	{
		if( mRadioGroup.GetObjectConst(i).Find(inControlID) != kIndex_Invalid )
		{
			foundRadioGroup = i;
			break;
		}
	}
	//���W�I�u���[�v��������Ȃ���΃��^�[��
	if( foundRadioGroup == kIndex_Invalid )   return;
	
	//
	SetBool(inControlID,false);
	AItemCount	radioButtonCount = mRadioGroup.GetObject(foundRadioGroup).GetItemCount();
	for( AIndex j = 0; j < radioButtonCount; j++ )
	{
		AControlID	controlID = mRadioGroup.GetObject(foundRadioGroup).Get(j);
		if( IsControlEnabled(controlID) == true )
		{
			SetBool(controlID,true);
			return;
		}
	}
}
*/

#pragma mark ===========================

#pragma mark ---�C�x���g�n���h��(Window)
//#688

/**
�E�C���h�Eframe�ύX���̏������s
*/
void	CWindowImp::APICB_CocoaWindowBoundsChanged( const AGlobalRect& inNewFrame )
{
	GetAWin().EVT_WindowBoundsChanged(mCurrentWindowBounds,inNewFrame);
	mCurrentWindowBounds = inNewFrame;
}

/**
�E�C���h�Eresize�������̏������s
*/
void	CWindowImp::APICB_CocoaWindowResizeCompleted()
{
	//Live Resize�����ǂ����̔����false�ɂ��邽�߂̃t���OON
	mDoingResizeCompleted = true;
	//resize�����C�x���g
	GetAWin().EVT_WindowResizeCompleted();
	//Live Resize�����ǂ����̔����false�ɂ��邽�߂̃t���OOFF
	mDoingResizeCompleted = false;
}

/**
�{�^��sentAction�������s
*/
void	CWindowImp::APICB_CocoaDoButtonAction( const AControlID inControlID )
{
	//EVT_Clicked()���s
	if( GetAWin().EVT_Clicked(inControlID,0) == false )
	{
		//EVT_Clicked()�Ŗ������Ȃ�EVT_ValueChanged()�ŏ���
		GetAWin().EVT_ValueChanged(inControlID);
	}
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�|�b�v�A�b�v���j���[sentAction�������s
*/
void	CWindowImp::APICB_CocoaDoPopupMenuAction( const AControlID inControlID )
{
	//EVT_Clicked()���s
	if( GetAWin().EVT_Clicked(inControlID,0) == false )
	{
		//EVT_Clicked()�Ŗ������Ȃ�EVT_ValueChanged()�ŏ���
		GetAWin().EVT_ValueChanged(inControlID);
	}
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
TextField sentAction�������s�iTextField�ŕ������͊����i���^�[���L�[���j���j
*/
void	CWindowImp::APICB_CocoaDoTextFieldAction( const AControlID inControlID )
{
	//EVT_ValueChanged()���s
	GetAWin().EVT_ValueChanged(inControlID);
	//EVT_TextInputted()���s
	GetAWin().EVT_TextInputted(inControlID);
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
TextField �������͎������i�ꕶ�����ƂɃR�[�������j
*/
void	CWindowImp::APICB_CocoaDoTextFieldTextChanged( const AControlID inControlID )
{
	//EVT_TextInputted()���s
	GetAWin().EVT_TextInputted(inControlID);
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

//#688
/**
FontField �t�H���g�ύX(changeFont)������
*/
void	CWindowImp::APICB_CocoaDoFontFieldChanged( const AControlID inControlID )
{
	//EVT_ValueChanged()���s
	GetAWin().EVT_ValueChanged(inControlID);
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

//#688
/**
FontPanel �t�H���g�ύX(changeFont)������
*/
void	CWindowImp::APICB_CocoaDoFontPanelFontChanged()
{
	//EVT_ValueChanged()���s
	GetAWin().EVT_ValueChanged(mFontPanelVirtualControlID);
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�c�[���o�[���ڂ�sentAction����
*/
void	CWindowImp::APICB_CocoaToolbarItemAction( const AControlID inControlID )
{
	//�^�u�R���g���[���̃^�u�I��p�c�[���o�[���ڑI�����́A�^�u�I���������s��
	if( inControlID >= kControlID_FirstTabSelectorToolbarItem && inControlID <= kControlID_MaxTabSelectorToolbarItem )
	{
		AIndex	tabIndex = inControlID-kControlID_FirstTabSelectorToolbarItem;
		SetTabControlValue(tabIndex);
	}
	
	//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
	NSView*	view = CocoaObjects->FindViewByTag(inControlID);
	
	//pull down���j���[�̏ꍇ�A�I�����ڂ̃e�L�X�g�����j���[�^�C�g���i0�Ԗڂ̍��ځj�ɐݒ�
	if( [view isKindOfClass:[NSPopUpButton class]] == YES )
	{
		NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
		if( [popup pullsDown] == YES )
		{
			AText	text;
			ACocoa::SetTextFromNSString([popup titleOfSelectedItem],text);
			AMenuWrapper::SetMenuItemText([popup menu],0,text);
		}
		
		//pull down���j���[�̏ꍇ�A�I�����ꂽ���ڂɃ`�F�b�N�}�[�N��t����
		AItemCount	itemCount = AMenuWrapper::GetMenuItemCount([popup menu]);
		for( AIndex i = 1; i < itemCount; i++ )
		{
			ABool	check = false;
			if( i == [popup indexOfSelectedItem] )
			{
				check = true;
			}
			AMenuWrapper::SetCheckMark([popup menu],i,check);
		}
	}
	
	//EVT_Clicked()���s
	GetAWin().EVT_Clicked(inControlID,0);
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�c�[���o�[���ڒǉ����������s
*/
void	CWindowImp::APICB_CocoaDoToolbarItemAddedAction( const AControlID inControlID )
{
	//CWindowImpCocoaObjects::CreateToolbar()���s���ł͂Ȃ��ꍇ�̂݁A
	//EVT_ToolbarItemAdded()�����s����B
	//�iCWindowImpCocoaObjects::CreateToolbar()���s�����ƁAMac OS X 10.4�ɂ����āAEVT_ToolbarItemAdded()����[toolbar items]���R�[�������Ƃ��ɁA�ēx�������R�[������Ė������[�v�ɂȂ�j
	if( CocoaObjects->IsCreatingToolbar() == false )
	{
		GetAWin().EVT_ToolbarItemAdded(inControlID);
	}
}

/**
�E�C���h�E��MainWindow�ɂȂ����Ƃ��̏���
*/
void	CWindowImp::APICB_CocoaBecomeMainWindow()
{
	//EVT_WindowActivated()���s
	GetAWin().EVT_WindowActivated();
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�E�C���h�E��MainWindow�łȂ��Ȃ����Ƃ��̏���
*/
void	CWindowImp::APICB_CocoaResignMainWindow()
{
	//EVT_WindowDeactivated()���s
	GetAWin().EVT_WindowDeactivated();
}

/**
�E�C���h�E��MainWindow�ɂȂ����Ƃ��̏���
*/
void	CWindowImp::APICB_CocoaBecomeKeyWindow()
{
	//EVT_WindowFocusActivated()���s
	GetAWin().EVT_WindowFocusActivated();
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�E�C���h�E��MainWindow�łȂ��Ȃ����Ƃ��̏���
*/
void	CWindowImp::APICB_CocoaResignKeyWindow()
{
	//EVT_WindowFocusDeactivated()���s
	GetAWin().EVT_WindowFocusDeactivated();
}

/**
�E�C���h�E��Dock�������Ƃ��̏���
*/
void	CWindowImp::APICB_CocoaMiniaturize()
{
	//EVT_WindowCollapsed()���s
	GetAWin().EVT_WindowCollapsed();
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�L�����Z���iESC�L�[���������j����
*/
void	CWindowImp::APICB_CocoaDoCancelOperation( const AModifierKeys inModifierKeys )
{
	if( HasCloseButton() == true && mCloseByEscapeKey == true )//#986
	{
		//�N���[�Y�{�^��������΁AEVT_DoCloseButton()�����s
		GetAWin().EVT_DoCloseButton();
	}
	else
	{
		//�f�t�H���g�L�����Z���{�^���ݒ莞�́A�f�t�H���g�L�����Z���{�^����EVT_Clicked()���s
		if( mDefaultCancelButton != kControlID_Invalid )
		{
			GetAWin().EVT_Clicked(mDefaultCancelButton,inModifierKeys);
		}
	}
}

/**
�E�C���h�E����悤�Ƃ���Ƃ��̏���
*/
void	CWindowImp::APICB_CocoaDoWindowShouldClose()
{
	//AWindow::EVT_DoCloseButton()���s
	GetAWin().EVT_DoCloseButton();
}

#pragma mark ===========================

#pragma mark ---�C�x���g�n���h��(View)

/*
CocoaUserPaneView�̊e�C�x���g�R�[���o�b�N����A������̃��\�b�h���R�[�������B
�iCUserPane���Ƀ��\�b�h�����݂�����̂ɂ��Ă��A�S�āA��U������̃��\�b�h����Ă���B�j
*/

/**
�}�E�X�_�E��
*/
void	CWindowImp::APICB_CocoaMouseDown( const AControlID inControlID,
			const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//�R���g���[����Visible�łȂ���Ή������Ȃ�
	if( IsControlVisible(inControlID) == false )   return;
	
	//#558
	//�c�������[�h�̏ꍇ�A���W�ϊ�
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoMouseDown()�����s
	if( GetAWin().EVT_DoMouseDown(inControlID,localPoint,inModifierKeys,inClickCount) == false )//#232 �Ԃ�l��false�̏ꍇ����EVT_Clicked()���s
	{
		//�N���b�N�񐔂ɂ���āA���ꂼ��EVT_Clicked(), EVT_DoubleClicked()�����s
		if( inClickCount == 1 )
		{
			GetAWin().EVT_Clicked(inControlID,inModifierKeys);
		}
		else if( inClickCount == 2 )
		{
			GetAWin().EVT_Clicked(inControlID,inModifierKeys);//B0303 �E�C���h�E�A�N�e�B�x�[�g�N���b�N���P��ڂɂȂ��Ă��܂��AEVT_Clicked()���ĂׂȂ����Ƃ�����̂�
			GetAWin().EVT_DoubleClicked(inControlID);
		}
	}
	//AWindow::EVT_GetCursorType()�����s
	ACursorWrapper::SetCursor(GetAWin().EVT_GetCursorType(inControlID,localPoint,inModifierKeys));
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�E�}�E�X�_�E��
*/
void	CWindowImp::APICB_CocoaRightMouseDown( const AControlID inControlID,
			const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//�R���g���[����Visible�łȂ���Ή������Ȃ�
	if( IsControlVisible(inControlID) == false )   return;
	
	//#558
	//�c�������[�h�̏ꍇ�A���W�ϊ�
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoContextMenu()�����s
	GetAWin().EVT_DoContextMenu(inControlID,localPoint,inModifierKeys,inClickCount);
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�}�E�Xdrag
*/
void	CWindowImp::APICB_CocoaMouseDragged( const AControlID inControlID,
			const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�R���g���[����Visible�łȂ���Ή������Ȃ�
	if( IsControlVisible(inControlID) == false )   return;
	
	//#558
	//�c�������[�h�̏ꍇ�A���W�ϊ�
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoMouseTracking()�����s
	GetAWin().EVT_DoMouseTracking(inControlID,localPoint,inModifierKeys);
	
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�}�E�Xup
*/
void	CWindowImp::APICB_CocoaMouseUp( const AControlID inControlID, const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�R���g���[����Visible�łȂ���Ή������Ȃ�
	if( IsControlVisible(inControlID) == false )   return;
	
	//#558
	//�c�������[�h�̏ꍇ�A���W�ϊ�
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoMouseTrackEnd()�����s
	GetAWin().EVT_DoMouseTrackEnd(inControlID,localPoint,inModifierKeys);
	
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�}�E�X�z�C�[��
*/
void	CWindowImp::APICB_CocoaMouseWheel( const AControlID inControlID,
			const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
			const ALocalPoint inLocalPoint )
{
	//�R���g���[����Visible�łȂ���Ή������Ȃ�
	if( IsControlVisible(inControlID) == false )   return;
	
	//#558
	//�c�������[�h�̏ꍇ�A���W�ϊ�
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoMouseWheel()�����s
	GetAWin().EVT_DoMouseWheel(inControlID,inDeltaX,inDeltaY,inModifierKeys,localPoint);
}

/**
�}�E�Xmoved
*/
void	CWindowImp::APICB_CocoaMouseMoved( const AControlID inControlID,
			const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�R���g���[����Visible�łȂ���Ή������Ȃ�
	if( IsControlVisible(inControlID) == false )   return;
	/*
	//
	CWindowImp::ProcessMouseExitToAnotherControl(this,inControlID,inLocalPoint);
	
	//AWindow::EVT_DoMouseMoved()�����s
	GetAWin().EVT_DoMouseMoved(inControlID,inLocalPoint,inModifierKeys);
	
	//
	CWindowImp::SetMouseTracking(this,inControlID);
	*/
	
	//#558
	//�c�������[�h�̏ꍇ�A���W�ϊ�
	ALocalPoint	localPoint = inLocalPoint;
	if( inControlID != kControlID_Invalid )
	{
		AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
		if( index != kIndex_Invalid )
		{
			if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
			{
				localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
			}
		}
	}
	
	//
	if( mMouseMovedTrackingControlID != kControlID_Invalid && mMouseMovedTrackingControlID != inControlID )
	{
		//AWindow::EVT_DoMouseLeft()�����s
		GetAWin().EVT_DoMouseLeft(mMouseMovedTrackingControlID,localPoint);
	}
	if( inControlID != kControlID_Invalid )
	{
		//AWindow::EVT_DoMouseMoved()�����s
		GetAWin().EVT_DoMouseMoved(inControlID,localPoint,inModifierKeys);
	}
	//
	mMouseMovedTrackingControlID = inControlID;
	
	//�E�C���h�E��ΏۂƂ���mouse moved����
	AWindowPoint	wpt = {0};
	GetWindowPointFromControlLocalPoint(inControlID,localPoint,wpt);
	GetAWin().EVT_DoMouseMoved(wpt,inModifierKeys);
	
	//�J�[�\���ݒ�
	ACursorWrapper::SetCursor(GetAWin().EVT_GetCursorType(inControlID,localPoint,inModifierKeys));
}

/**
�}�E�Xexited
*/
void	CWindowImp::APICB_CocoaMouseExited( const AControlID inControlID, const ALocalPoint inLocalPoint )
{
	//#558
	//�c�������[�h�̏ꍇ�A���W�ϊ�
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	if( mMouseMovedTrackingControlID != kControlID_Invalid )
	{
		//AWindow::EVT_DoMouseLeft()�����s
		GetAWin().EVT_DoMouseLeft(mMouseMovedTrackingControlID,localPoint);
	}
	//
	mMouseMovedTrackingControlID = kControlID_Invalid;
}

/**
drag��view�ɓ�����
*/
void	CWindowImp::APICB_CocoaDraggingEntered( const AControlID inControlID, const ADragRef inDragRef,
		const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//#558
	//�c�������[�h�̏ꍇ�A���W�ϊ�
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//�V���ɓ�����ControlID��AWindow::EVT_DoDragEnter()�����s
	GetAWin().EVT_DoDragEnter(inControlID,inDragRef,localPoint,inModifierKeys);
	//mDragTrackingControl���X�V
	mDragTrackingControl = inControlID;
}

/**
drag tracking��
*/
void	CWindowImp::APICB_CocoaDraggingUpdated( const AControlID inControlID, const ADragRef inDragRef, 
		const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys, ABool& outIsCopyOperation )
{
	//#558
	//�c�������[�h�̏ꍇ�A���W�ϊ�
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//�V����ControlID���ATracking����ControlID���ƈႤ�ꍇ�́A����Tracking����Control��DoDragLeave�𑗐M (Carbon�����#236�̑΍�𓥏P�j
	if( mDragTrackingControl != kControlID_Invalid && mDragTrackingControl != inControlID )
	{
		//Tracking����ControlID��AWindow::EVT_DoDragLeave()�����s
		GetAWin().EVT_DoDragLeave(mDragTrackingControl,inDragRef,localPoint,inModifierKeys);
		//mDragTrackingControl���N���A
		mDragTrackingControl = kControlID_Invalid;
	}
	//mDragTrackingControl�������ControlID�ƈႤ�ꍇ�́A�V���ɓ�����ControlID��AWindow::EVT_DoDragEnter()�����s
	if( mDragTrackingControl != inControlID )
	{
		GetAWin().EVT_DoDragEnter(inControlID,inDragRef,localPoint,inModifierKeys);
	}
	//AWindow::EVT_DoDragTracking()�����s
	GetAWin().EVT_DoDragTracking(inControlID,inDragRef,localPoint,inModifierKeys,outIsCopyOperation);
	//mDragTrackingControl���X�V
	mDragTrackingControl = inControlID;
}

/**
drag��view����o��
*/
void	CWindowImp::APICB_CocoaDraggingExited( const AControlID inControlID, const ADragRef inDragRef,
		const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//#558
	//�c�������[�h�̏ꍇ�A���W�ϊ�
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//����tracking����control�ɂ��āAAWindow::EVT_DoDragLeave()�����s
	if( mDragTrackingControl != kControlID_Invalid )
	{
		GetAWin().EVT_DoDragLeave(mDragTrackingControl,inDragRef,localPoint,inModifierKeys);
	}
	//mDragTrackingControl���N���A
	mDragTrackingControl = kControlID_Invalid;
}

/**
drop��
*/
void	CWindowImp::APICB_CocoaPerformDragOperation( const AControlID inControlID, const ADragRef inDragRef, 
		const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//#558
	//�c�������[�h�̏ꍇ�A���W�ϊ�
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoDragReceive()���s
	GetAWin().EVT_DoDragReceive(inControlID,inDragRef,localPoint,inModifierKeys);
	//Drop�ς݃t���O��ݒ�iDrag���Ŏg�p����j
	CUserPane::SetDropped(true);
	
	//�t�@�C��Drop
	AFileAcc	file;
	if( AScrapWrapper::GetFileDragData(inDragRef,file) == true )
	{
		GetAWin().EVT_DoFileDropped(inControlID,file);
	}
	
	//���j���[�X�V
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
�X�N���[���o�[���쎞��Callback
*/
void	CWindowImp::APICB_DoScrollBarAction( const AControlID inID, const AScrollBarPart inPart )
{
	GetAWin().EVT_DoScrollBarAction(inID,inPart);
}

/**
�`��
*/
void	CWindowImp::APICB_CocoaDrawRect( const AControlID inControlID, const ALocalRect& inDirtyRect )
{
	GetUserPane(inControlID).APICB_CocoaDrawRect(inDirtyRect);
}

/**
key down�������iraw key�C�x���g�j
*/
ABool	CWindowImp::APICB_CocoaDoKeyDown( const AControlID inControlID, const AText& inText, const AModifierKeys inModifierKeys )//#1080
{
	//AWindow::EVT_DoKeyDown()���s�B���ۂɏ������s��ꂽ�Ƃ���true���Ԃ�B
	//�������raw key�C�x���g�Ȃ̂łǂ����Ă����荞�ޕK�v������ꍇ�ȊO�́AEVT_DoKeyDown()�͏������s��Ȃ��B
	if( GetAWin().EVT_DoKeyDown(inControlID,inText,inModifierKeys) == true )
	{
		//���j���[�X�V
		AApplication::GetApplication().NVI_UpdateMenu();
		//�����ς݂Ȃ̂�true��Ԃ�
		return true;
	}
	//�������Ȃ̂�false��Ԃ�
	return false;
}

/**
�e�L�X�g���͎�����
*/
ABool	CWindowImp::APICB_CocoaDoTextInput( const AControlID inControlID, const AText& inText,
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction )
{
	ABool	updateMenu = false;
	//AWindow::EVT_DoTextInput()�����s
	ABool	result = GetAWin().EVT_DoTextInput(inControlID,inText,inKeyBindKey,inModifierKeys,inKeyBindAction,updateMenu);
	if( updateMenu == true )
	{
		//���j���[�X�V
		AApplication::GetApplication().NVI_UpdateMenu();
	}
	return result;
}

/**
InlineInput�e�L�X�g�X�V
*/
void	CWindowImp::APICB_CocoaDoInlineInput( const AControlID inControlID, 
		const AText& inText, const AItemCount inFixLen,
		const AArray<AIndex>& inHiliteLineStyleIndex, 
		const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
		const AItemCount inReplaceLengthInUTF16 )
{
	ABool	updateMenu = false;
	//AWindow::EVT_DoInlineInput()�����s
	GetAWin().EVT_DoInlineInput(inControlID,inText,inFixLen,
				inHiliteLineStyleIndex,inHiliteStartPos,inHiliteEndPos,inReplaceLengthInUTF16,updateMenu);
	if( updateMenu == true )
	{
		//���j���[�X�V
		AApplication::GetApplication().NVI_UpdateMenu();
	}
}

/**
���݂̑I���e�L�X�g���擾(InlineInput�p)
*/
void	CWindowImp::APICB_CocoaDoInlineInputGetSelectedText( const AControlID inControlID, AText& outText ) const
{
	outText.DeleteAll();
	GetAWinConst().EVT_DoInlineInputGetSelectedText(inControlID,outText);
}

/**
�e�L�X�g��index�ɑΉ�������W���擾(InlineInput�p)
*/
void	CWindowImp::APICB_CocoaFirstRectForCharacterRange( const AControlID inControlID, 
		const AIndex inStart, const AIndex inEnd, AGlobalRect& outRect ) 
{
	outRect.left	= 0;
	outRect.top		= 0;
	outRect.right	= 0;
	outRect.bottom	= 0;
	//AWindow::EVT_DoInlineInputOffsetToPos()���s
	ALocalPoint	localStartPoint = {0}, localEndPoint = {0};
	if( GetAWinConst().EVT_DoInlineInputOffsetToPos(inControlID,inStart,localStartPoint) == true &&
				GetAWinConst().EVT_DoInlineInputOffsetToPos(inControlID,inEnd,localEndPoint) == true )
	{
		//�s�̍������擾 #1124
		ANumber	lineHeight = 0, lineAscent = 0;
		GetUserPane(inControlID).GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
		//�c�������[�h���͍��W�ϊ� #558
		if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
		{
			localStartPoint = ConvertVerticalCoordinateFromAppToImp(inControlID,localStartPoint);
			localEndPoint = ConvertVerticalCoordinateFromAppToImp(inControlID,localEndPoint);
		}
		//�O���[�o�����W�ɕϊ�
		AGlobalPoint	globalStartPoint = {0}, globalEndPoint = {0};
		GetGlobalPointFromControlLocalPoint(inControlID,localStartPoint,globalStartPoint);
		GetGlobalPointFromControlLocalPoint(inControlID,localEndPoint,globalEndPoint);
		//GlobalRect�ݒ�Btop/botom��-2, +2��ݒ�B
		outRect.left	= globalStartPoint.x;
		outRect.top		= globalStartPoint.y-lineHeight-2;//#1124
		outRect.right	= globalEndPoint.x;
		outRect.bottom	= globalEndPoint.y+lineAscent+2;//#1124
	}
}

/**
���W�ɑ΂���e�L�X�g��index���擾(InlineInput�p)
*/
void	CWindowImp::APICB_CocoaCharacterIndexForPoint( const AControlID inControlID, 
		const AGlobalPoint& inGlobalPoint, AIndex& outOffsetUTF8 ) const
{
	outOffsetUTF8 = 0;
	//LocalPoint�֕ϊ�
	AGlobalRect	windowbounds = {0};
	GetWindowBounds(windowbounds);
	AWindowPoint	wpt = {0};
	wpt.x = inGlobalPoint.x - windowbounds.left;
	wpt.y = inGlobalPoint.y - windowbounds.top;
	ALocalPoint	localPoint = {0};
	GetControlLocalPointFromWindowPoint(inControlID,wpt,localPoint);
	//AWindow::EVT_DoInlineInputPosToOffset()�����s
	GetAWinConst().EVT_DoInlineInputPosToOffset(inControlID,localPoint,outOffsetUTF8);
}

/**
View��frame�ύX������
*/
void	CWindowImp::APICB_CocoaViewFrameChanged( const AControlID inControlID )
{
	//AWindow::EVT_DoControlBoundsChanged()�����s
	GetAWin().EVT_DoControlBoundsChanged(inControlID);
}

//�t�H�[�J�X�ړ����s���̃t���O
//selectNextKeyView, selectPreviousKeyView���s���Ƀt���OON�ɂȂ�
ABool	gSwitchingFocusNow = false;

/**
�t�H�[�J�X�ړ����s���̃t���O��ݒ�
*/
void	CWindowImp::SetSwitchingFocusNow( const ABool inSwitchingFocusNow )
{
	gSwitchingFocusNow = inSwitchingFocusNow;
}

//
//�������[�v�h�~�̂��߂̃t���O
//resignFirstResponder���͎��ۂ�focus���O���O�ɃR�[�������̂ŁAEVT_DoViewFocusDeactivated()���̒�����
//SwitchFocus�����ƁA�������[�v��������B�i��FAWindow::NVI_SetControlEnable()�ł̓����l�ݒ�`�F�b�N���͂������ꍇ�A
//AView_EditBox����Value changed���R�[������Aupdate control status�o�R�ŁAswitch focus next�����B�j
ABool	gDoingBecomeOrResignFirstResponder = false;

/**
View��FirstResponder�ɂȂ����Ƃ��̏���
*/
void	CWindowImp::APICB_CocoaViewBecomeFirstResponder( const AControlID inControlID )
{
	//�������[�v�h�~
	if( gDoingBecomeOrResignFirstResponder == true )
	{
		_ACError("",NULL);
		return;
	}
	gDoingBecomeOrResignFirstResponder = true;
	
	//AWindow::EVT_DoViewFocusActivated()�����s
	GetAWin().EVT_DoViewFocusActivated(inControlID);
	
	//�t�H�[�J�X�ړ����s���̃t���O�ݒ蒆�Ȃ�A�r���[�̓��e�������I������B�i�^�u�L�[�ɂ��t�H�[�J�X�ړ����Ƀe�L�X�g��S�I������j
	if( gSwitchingFocusNow == true )
	{
		GetAWin().NVI_AutomaticSelectBySwitchFocus(inControlID);
	}
	
	//�������[�v�h�~
	gDoingBecomeOrResignFirstResponder = false;
}

/**
View��FirstResponder�łȂ��Ȃ����Ƃ��̏���
*/
void	CWindowImp::APICB_CocoaViewResignFirstResponder( const AControlID inControlID )
{
	//�������[�v�h�~
	if( gDoingBecomeOrResignFirstResponder == true )
	{
		_ACError("",NULL);
		return;
	}
	gDoingBecomeOrResignFirstResponder = true;
	
	//AWindow::EVT_DoViewFocusDeactivated()�����s
	GetAWin().EVT_DoViewFocusDeactivated(inControlID);
	
	//�������[�v�h�~
	gDoingBecomeOrResignFirstResponder = false;
}

/**
MouseTracking����Ԃ�ݒ肷��

�ΏۃE�C���h�E����Mouse���o�����Ƃ����m���邽�߁B
*/
void	CWindowImp::SetMouseTracking( CWindowImp* inWindowImp , const AControlID inControlID )
{
	sMouseTrackingMode = true;
	sMouseTrackingWindowImp = inWindowImp;
	sMouseTrackingControlID = inControlID;
}

//#310
/**
Window�ł�MouseMoved�C�x���g�������ɃR�[�������B���̃R���g���[����Tracking����������A���̃R���g���[���ɑ΂���DoMouseExited()���Ƃ΂��B
CWindowImp::APICB_CocoaMouseMoved()���疈��R�[�������B
��������g�p���邩�ǂ����������B
*/
void	CWindowImp::ProcessMouseExitToAnotherControl( CWindowImp* inWindowImp, 
		const AControlID inControlID, const ALocalPoint inLocalPoint )
{
	//Tracking���̏ꍇ�̂ݏ���
	if( sMouseTrackingMode == true )
	{
		if( inWindowImp != sMouseTrackingWindowImp || inControlID != sMouseTrackingControlID )
		{
			//���݂�Tracking��Control�ƈ�v���Ȃ���΁A����Control�ɑ΂���DoMouseExited���R�[������B
			if( CWindowImp::ExistWindowImp(sMouseTrackingWindowImp) == true )
			{
				//AWindow::EVT_DoMouseLeft()�����s
				sMouseTrackingWindowImp->GetAWin().EVT_DoMouseLeft(sMouseTrackingControlID,inLocalPoint);
			}
			//Tracking����
			sMouseTrackingMode = false;
		}
	}
}

//MouseTracking�f�[�^
ABool						CWindowImp::sMouseTrackingMode = false;
CWindowImp*					CWindowImp::sMouseTrackingWindowImp = NULL;
AControlID					CWindowImp::sMouseTrackingControlID = kControlID_Invalid;

//#1026
/**
QuickLook
*/
void	CWindowImp::APICB_CocoaQuickLook( const AControlID inControlID,
										   const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�R���g���[����Visible�łȂ���Ή������Ȃ�
	if( IsControlVisible(inControlID) == false )   return;
	
	//#558
	//�c�������[�h�̏ꍇ�A���W�ϊ�
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoMouseTrackEnd()�����s
	GetAWin().EVT_QuickLook(inControlID,localPoint,inModifierKeys);
}

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---�C�x���g�n���h��(Carbon)

/**
�R���g���[���}�E�X����Tracking���̃C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoControlTrack( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//OS10.3�o�O�΍�
	//http://lists.apple.com/archives/carbon-development/2003/Jun/msg00303.html
	err = ::CallNextEventHandler(inCallRef,inEventRef);
	if( err != noErr )   _ACErrorNum("CallNextEventHandler() returned error: ",err,this);
	
	HIViewRef	view = NULL;
	err = ::GetEventParameter(inEventRef,kEventParamDirectObject,typeControlRef,NULL,sizeof(view),NULL,&view);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	ControlPartCode	partcode = 0;
	err = ::GetEventParameter(inEventRef,kEventParamControlPart,typeControlPartCode,NULL,sizeof(partcode),NULL,&partcode);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	if( partcode == kControlNoPart )
	{
		UInt32	commandID;
		err = ::GetControlCommandID(view,&commandID);
		if( err != noErr )   _ACErrorNum("GetControlCommandID() returned error: ",err,this);
		HICommand	command;
		command.attributes = NULL;
		command.commandID = commandID;
		command.menu.menuRef = NULL;
		command.menu.menuItemIndex = 0;
		err = ::ProcessHICommand(&command);
		if( err != noErr )   _ACErrorNum("ProcessHICommand() returned error: ",err,this);
	}
	return true;
}

/**
�}�E�X�N���b�N�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoMouseDown( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	if( (IsActive() == true) || (IsFloatingWindow() == true) )
	{
		HIViewRef	view;
		err = ::HIViewGetViewForMouseEvent(::HIViewGetRoot(mWindowRef),inEventRef,&view);
		if( err != noErr )   _ACErrorNum("HIViewGetViewForMouseEvent() returned error: ",err,this);
		AControlID	controlID = GetControlID(view);
		//�R���g���[����Enable�Ȃ珈�����s��
		//�i���̔�����O���ꍇ�A�R���e�L�X�g���j���[�̑I�����������K���t�H�[�J�X�̓������Ă���view�ɑ΂��ď��������
		//�iAWindow::EVT_DoMenuItemSelected()���Q�Ɓj������������K�v�L�� #601�������ǉ��̃R�����g�j
		if( IsControlEnabled(controlID) == true )
		{
			//#291 �N���b�N�����R���g���[����Focus�󂯕t���Ȃ��ꍇ�̓t�H�[�J�X�ړ����Ȃ��悤�ɂ���
			/*#360
			ABool	supportFocus = true;
			if( IsUserPane(controlID) == true )
			{
				supportFocus = GetUserPane(controlID).GetSupportFocus();
			}
			*/
			//#391 ���݂̃t�H�[�J�X�ʒu�ƈႤ�R���g���[�����N���b�N�����ꍇ�́A�t�H�[�J�X�R���g���[����ValueChanged��ʒm
			AControlID	oldFocus = GetCurrentFocus();
			if( oldFocus != kControlID_Invalid && oldFocus != controlID )
			{
				GetAWin().EVT_ValueChanged(oldFocus);
			}
			//
			ABool	supportFocus = GetAWin().NVI_GetControlSupportFocus(controlID);//#360
			if( supportFocus == true )//#291
			{
				//
				//#391 AControlID	oldFocus = GetCurrentFocus();
				if( oldFocus != controlID )
				{
					/*#391 ��̏����֕ύX #360�̉e���H�ł��̒���ʂ�Ȃ��ꍇ���o�Ă�������
					//�}�E�X�N���b�N�ɂ��t�H�[�J�X���ړ������Ƃ��ɋ��t�H�[�J�X�̒l�ύX��ʒm����
					if( oldFocus != kControlID_Invalid )
					{
						GetAWin().EVT_ValueChanged(oldFocus);
					}
					*/
					//
					//#360 SetFocus(controlID);
					GetAWin().NVI_SwitchFocusTo(controlID);//#360
				}
			}
			//UserPane���ǂ����𔻒�
			if( IsUserPane(controlID) == true )
			{
				//B0353 ���A�v���N���b�N����mi�̃h�L�������g�E�C���h�E���A�N�e�B�u�̂܂܂ƂȂ���΍�
				ProcessSerialNumber psn = {0, kCurrentProcess};
				err = ::SetFrontProcessWithOptions(&psn,kSetFrontProcessFrontWindowOnly);
				if( err != noErr )   _ACErrorNum("SetFrontProcessWithOptions() returned error: ",err,this);
				if( IsOverlayWindow() == false )//#291 ���̏����������Overlay�E�C���h�E��Control(SupportFocus�łȂ�)���N���b�N�����Ƃ��ɂ��̃E�C���h�E�փt�H�[�J�X�ړ����Ă��܂�
				{
					err = ::SetUserFocusWindow(mWindowRef);//Drawer����t�H�[�J�X���ڂ�����
					if( err != noErr )   _ACErrorNum("SetUserFocusWindow() returned error: ",err,this);
				}
				//UserPane�Ȃ�Click���\�b�h���R�[��
				return GetUserPane(controlID).DoMouseDown(inEventRef);
			}
		}
	}
	/*B0353 ���A�v���N���b�N����mi�̃h�L�������g�E�C���h�E���A�N�e�B�u�̂܂܂ƂȂ���΍�imi�A�v��deactivate��Ƀ}�E�X�_�E���C�x���g������̂�������Ȃ��H�j
	else
	{
		Select();
		ProcessSerialNumber psn = {0, kCurrentProcess};
		//B0346 ::SetFrontProcess(&psn);
		::SetFrontProcessWithOptions(&psn,kSetFrontProcessFrontWindowOnly);
		return true;
	}*/
	//B0000 ���[�h�ݒ�E�C���h�E�����ɂ܂킵�āu�L�[�o�C���h�v��DataBrowser�̍��ڂ��N���b�N����ƁA���[�h�ݒ�E�C���h�E���O�ʂɗ��Ȃ�
	//�����ǉ�����Ɖ������邪�AB0353��肪�������邩�ǂ������s��
	else
	{
		if( mIsOverlayWindow == true && mOverlayParentWindowRef != NULL )//#669
		{
			//Overlay�̎q�E�C���h�E�̏ꍇ�́A�e�E�C���h�E��I������
			GetWindowImpByWindowRef(mOverlayParentWindowRef).Select();//#669
		}
		else
		{
			Select();
		}
	}
	return false;
}

/**
�}�E�XWheel�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoMouseWheel( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//if( (IsActive() == true) || (IsFloatingWindow() == true) )
	{
		HIViewRef	view;
		err = ::HIViewGetViewForMouseEvent(::HIViewGetRoot(mWindowRef),inEventRef,&view);
		if( err != noErr )   _ACErrorNum("HIViewGetViewForMouseEvent() returned error: ",err,this);
		AControlID	controlID = GetControlID(view);
		//#601 �R���g���[��disable�ł��z�C�[���\�ɂ��� if( IsControlEnabled(controlID) == true )
		{
			//UserPane���ǂ����𔻒�
			if( IsUserPane(controlID) == true )
			{
				//UserPane�Ȃ�DoMouseWheel���\�b�h���R�[��
				return GetUserPane(controlID).DoMouseWheel(inEventRef);
			}
		}
	}
	return false;
}

/**
�}�E�X�ړ��C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoMouseMoved( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	if( (IsActive() == true) || (IsFloatingWindow() == true) )
	{
		HIViewRef	view;
		err = ::HIViewGetViewForMouseEvent(::HIViewGetRoot(mWindowRef),inEventRef,&view);
		if( err != noErr )   _ACErrorNum("HIViewGetViewForMouseEvent() returned error: ",err,this);
		AControlID	controlID = GetControlID(view);
		//B0000 080810 �E�C���h�E���ő��̃R���g���[����Ƀ}�E�X���ړ������Ƃ���DoMouseExit()�����s���āACAppImp�ł�tracking������
		/*#310
		AControlID	trackingControlID =  CAppImp::GetMouseTrackingControlID(mWindowRef);
		if( trackingControlID != kControlID_Invalid && trackingControlID != controlID )
		{
			//UserPane���ǂ����𔻒�
			if( IsUserPane(trackingControlID) == true )
			{
				GetUserPane(trackingControlID).DoMouseExited(inEventRef);//R0228
			}
			CAppImp::ClearMouseTracking();
		}
		*/
		CAppImp::ProcessMouseExitToAnotherControl(inEventRef,mWindowRef,controlID);
		
		if( IsControlEnabled(controlID) == true )
		{
			//UserPane���ǂ����𔻒�
			if( IsUserPane(controlID) == true )
			{
				//UserPane�Ȃ�DoMouseWheel���\�b�h���R�[��
				return GetUserPane(controlID).DoMouseMoved(inEventRef);
			}
		}
	}
	return false;
}

/**
�R�}���h���s�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoCommand( const EventHandlerCallRef inCallRef, const EventRef inEventRef, const HICommand inCommand )
{
	OSStatus	err = noErr;
	
	if( inCommand.commandID == 'swin' )   return false;
	//�^�uView�X�V
	if( inCommand.commandID == 'ttab' )
	{
		UpdateTab();
		return true;
	}
	//���j���[�X�V
	if( inCommand.commandID == kCommandID_UpdateCommandStatus )
	{
		if( IsActive() == false )   return false;//B0411
		if( /*#1034 mNavSheet != NULL ||*/ mHasSheet == true )   return false;
		//B0418 TableView��Text�ҏW���[�h�̂Ƃ��́A������D��
		ABool	tableViewFocus = false;
//#205 #if USE_IMP_TABLEVIEW
		for( AIndex i = 0; i < mUserPaneArray.GetItemCount(); i++ )
		{
			if( mUserPaneArray_Type.Get(i) == kUserPaneType_TableView )
			{
				if( GetTableViewByID(mUserPaneArray_ControlID.Get(i)).IsInEditSession() == true )
				{
					GetTableViewByID(mUserPaneArray_ControlID.Get(i)).UpdateMenu();
					tableViewFocus = true;
					break;
				}
			}
		}
//#205 #endif
		//UserPane��Focus������Ȃ�܂�UserPane�Ń��j���[�X�V�i�Ⴆ�΁AMLTEPane�́u�R�s�[�v���j
		if( mCurrentFocus != kControlID_Invalid )
		{
			if( IsUserPane(mCurrentFocus) == true && tableViewFocus == false/*B0418*/ )
			{
				GetUserPane(mCurrentFocus).UpdateMenu();
			}
		}
		
		//Abs�Ń��j���[�X�V
		GetAWin().EVT_UpdateMenu();
		
		//�R�}���h�`�F�[����̃I�u�W�F�N�g�S�Ăɏ������������̂�false�ŕԂ�
		return false;
	}
	/*#539
	//�w���v
	AIndex	helpindex = mHelpAnchor_Number.Find(inCommand.commandID);
	if( helpindex != kIndex_Invalid )
	{
		DisplayHelpPage(inCommand.commandID);
		return true;
	}
	*/
//#205 #if USE_IMP_TABLEVIEW
	//�e�[�u��Row Up, Down, Top, Bottom�{�^��
	AIndex	index = mTableViewRowUp_ButtonControlID.Find(inCommand.commandID);
	if( index != kIndex_Invalid )
	{
		GetTableViewByID(mTableViewRowUp_TableControlID.Get(index)).Up();
	}
	index = mTableViewRowDown_ButtonControlID.Find(inCommand.commandID);
	if( index != kIndex_Invalid )
	{
		GetTableViewByID(mTableViewRowUp_TableControlID.Get(index)).Down();
	}
	index = mTableViewRowTop_ButtonControlID.Find(inCommand.commandID);
	if( index != kIndex_Invalid )
	{
		GetTableViewByID(mTableViewRowUp_TableControlID.Get(index)).Top();
	}
	index = mTableViewRowBottom_ButtonControlID.Find(inCommand.commandID);
	if( index != kIndex_Invalid )
	{
		GetTableViewByID(mTableViewRowUp_TableControlID.Get(index)).Bottom();
	}
//#205 #endif
	
	//NavSheet�\�����̕���͖�������
	//#1034 if( mNavSheet != NULL )
	if( mHasSheet == true )//#1034
	{
		if( inCommand.commandID == 'clos' )   return true;
	}
	
	//�L�[�擾
	UInt32	modifiers = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	
	//
	AText	actiontext;
	AApplication::GetApplication().NVI_GetMenuManager().GetMenuActionText(inCommand.commandID,inCommand.menu.menuRef,inCommand.menu.menuItemIndex-1,actiontext);
	
//#205 #if USE_IMP_TABLEVIEW
	//B0418 TableView��Text�ҏW���[�h�̂Ƃ��́A������D��
	for( AIndex i = 0; i < mUserPaneArray.GetItemCount(); i++ )
	{
		if( mUserPaneArray_Type.Get(i) == kUserPaneType_TableView )
		{
			if( GetTableViewByID(mUserPaneArray_ControlID.Get(i)).IsInEditSession() == true )
			{
				if( GetTableViewByID(mUserPaneArray_ControlID.Get(i)).DoMenuItemSelected(inCommand.commandID,actiontext,modifiers) == true )
				{
					return true;
				}
			}
		}
	}
//#205 #endif
	//UserPane��Focus������Ȃ�UserPane�ɏ��������Ă݂�
	if( mCurrentFocus != kControlID_Invalid )
	{
		if( IsUserPane(mCurrentFocus) == true )
		{
			//�R�}���h����
			if( GetUserPane(mCurrentFocus).DoMenuItemSelected(inCommand.commandID,actiontext,modifiers) == true )
			{
				return true;
			}
		}
	}
	
	/*#349
	//����
	UpdateRadioGroup(inCommand.commandID);
	*/
	if( GetAWin().EVT_DoMenuItemSelected(inCommand.commandID,actiontext,modifiers) == true )
	{
		return true;
	}
	if( inCommand.commandID == kCommandID_UpdateCommandStatus )
	{
		return false;
	}
	
	//inCommand����R���g���[���̎�ނ𓾂邱�Ƃ��o���Ȃ��̂łƂɂ����Q�Ƃ��Ă�
	if( GetAWin().EVT_Clicked(inCommand.commandID,modifiers) == true )
	{
		return true;
	}
	if( GetAWin().EVT_ValueChanged(inCommand.commandID) == true )
	{
		return true;
	}
	
	return false;
}

/**
�L�[�����C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoKeyDown( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	unsigned char	ch = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyMacCharCodes,typeChar,NULL,sizeof(unsigned char),NULL,&ch);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	
	AControlID	controlID = mCurrentFocus;
	if( controlID != kControlID_Invalid )
	{
		if( GetAWin().EVT_DoKeyDown(controlID,ch) == true )   return true;
	}
	
	return false;
}

/**
�������̓C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoTextInput( const EventHandlerCallRef inCallRef, const EventRef inEventRef, ABool& outUpdateMenu )
{
	OSStatus	err = noErr;
	
	Boolean	done = false;
	outUpdateMenu = true;
	
	//���݂̃t�H�[�J�X��ControlID���擾
	AControlID	controlID = mCurrentFocus;
	
	//���͕����擾
	UniChar	unicodeStr[4096];
	UInt32	len = 0;
	err = ::GetEventParameter(inEventRef,kEventParamTextInputSendText,typeUnicodeText,NULL,4096,&len,&(unicodeStr[0]));
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	//�L�[�擾
	//B0429 UInt32	modifiers;
	//B0429 ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	UInt32	modifiers = AKeyWrapper::GetEventKeyModifiers(inEventRef);//B0429 ������kEventTextInputUnicodeForKeyEvent�̃C�x���g�n���h���Ȃ̂ŁAkEventParamKeyModifiers�͎擾�ł��Ȃ��BrawKeyEvent�ɕϊ����Ă���łȂ��ƁAmodifiers�͎擾�ł��Ȃ�
	
	//�e�L�X�g���͏����i�E�C���h�E�^�r���[�őS�ď�������ꍇ�j
	if( controlID != kControlID_Invalid )
	{
		AText	text;
		text.InsertFromUTF16TextPtr(0,&(unicodeStr[0]),len);
		if( GetAWin().EVT_DoTextInput(controlID,text,inEventRef,outUpdateMenu) == true )   return true;
	}
	
	//ESC�L�[���͂̏ꍇ�A���A����{�^���L��̏ꍇ
	ABool	closeshortcurt = ( (len == 2 && unicodeStr[0] == 0x1B) || (len == 2 && unicodeStr[0] == '.' && AKeyWrapper::IsCommandKeyPressed(modifiers) == true ) );//B0430
	if( /*B0430 len == 2 && unicodeStr[0] == 0x1B*/closeshortcurt == true && HasCloseButton() )
	{
		GetAWin().EVT_DoCloseButton();
		done = true;
	}
	//ESC�L�[���͂̏ꍇ//B0285
	else if( /*B0430 len == 2 && unicodeStr[0] == 0x1B*/closeshortcurt == true )
	{
		if( mDefaultCancelButton != kControlID_Invalid )
		{
			GetAWin().EVT_Clicked(mDefaultCancelButton,modifiers);
			done = true;
		}
	}
	
	//���^�[���L�[�A�������́A�G���^�[�L�[���͂̏ꍇ
	else if( len == 2 && (unicodeStr[0] == '\r' || unicodeStr[0] == 3) )//B0285 3(enter)���ǉ�
	{
		//�ΏۃR���g���[�������^�[���L�[���������邩�ǂ����𔻒�B
		ABool	controlCatchReturn = false;
		if( controlID != kControlID_Invalid )
		{
			controlCatchReturn = IsCatchReturn(controlID);
		}
		if( controlCatchReturn == false )
		{
			//�ΏۃR���g���[�������^�[���L�[���������Ȃ��i�E�C���h�E�����^�[���L�[����������j�ꍇ
			
			if( controlID != kControlID_Invalid )
			{
				//�R���g���[���l�ύX��ʒm
				GetAWin().EVT_ValueChanged(controlID);
				//����
				SetSelectionAll(controlID);
			}
			
			//
			if( mDefaultOKButton != kControlID_Invalid )
			{
				if( IsControlEnabled(mDefaultOKButton) == true )
				{
					GetAWin().EVT_Clicked(mDefaultOKButton,modifiers);
				}
			}
			
			//done=true�ɂ��Ă����Ȃ��ƁA::CallNextEventHandler()�ɂ��AMLTE�Ƀ��^�[�����n����Ă��܂�
			done = true;
		}
	}
	//�^�u�L�[���͂̏ꍇ
	else if( len == 2 && unicodeStr[0] == '\t' )
	{
		//�ΏۃR���g���[�������^�[���L�[���������邩�ǂ����𔻒�B
		ABool	controlCatchTab = false;
		if( controlID != kControlID_Invalid )
		{
			controlCatchTab = IsCatchTab(controlID);
		}
		if( controlCatchTab == false )
		{
			//�ΏۃR���g���[�������^�[���L�[���������Ȃ��i�E�C���h�E�����^�[���L�[����������j�ꍇ
			
			if( controlID != kControlID_Invalid )
			{
				//�R���g���[���l�ύX��ʒm
				GetAWin().EVT_ValueChanged(controlID);
			}
			
			//
			//B0429
			if( AKeyWrapper::IsShiftKeyPressed(modifiers) == true )
			{
				GetAWin().NVI_SwitchFocusToPrev();
			}
			else
			{
				GetAWin().NVI_SwitchFocusToNext();
			}
			
			//����
			done = true;
		}
	}
	//\�L�[���́A���A�o�b�N�X���b�V���ϊ����[�h�̏ꍇ
	else if( len == 2 && unicodeStr[0] == 0x00A5 && IsInputBackslashByYenKeyMode() == true )
	{
		AText	backslash;
		backslash.Add(0x5C);//backslash(UTF-8)
		InputText(controlID,backslash);
		done = true;
		//Text���͂�ʒm
		GetAWin().EVT_TextInputted(controlID);
	}
	//�J�[�\���L�[ #353
	else if(	(len == 2 && unicodeStr[0] == 0x001C) || (len == 2 && unicodeStr[0] == 0x001D) ||
				(len == 2 && unicodeStr[0] == 0x001E) || (len == 2 && unicodeStr[0] == 0x001F) )
	{
		if( GetAWin().EVT_ArrowKey(controlID,unicodeStr[0]) == true )
		{
			done = true;
		}
	}
	//�X�y�[�X�L�[ #353
	else if( len == 2 && unicodeStr[0] == 0x0020 )
	{
		AIndex	controlIndex = mUserPaneArray_ControlID.Find(controlID);
		if( controlIndex != kIndex_Invalid )
		{
			//ColorPickerPane�̏ꍇ�Amousedown�Ɠ��������������
			if( mUserPaneArray_Type.Get(controlIndex) == kUserPaneType_ColorPickerPane )
			{
				GetUserPane(controlID).DoMouseDown(inEventRef);
				done = true;
			}
		}
	}
	if( done == false )
	{
		//�f�[�^�^�C�v�ɏ]�����A���͉\��������i�L�[�t�B���^�[�j
		UniCharCount	unicodeLen = len/sizeof(UniChar);
		AIndex	dataTypeIndex = mDataType_ControlID.Find(controlID);
		if( dataTypeIndex != kIndex_Invalid )
		{
			switch(mDataType_DataType.Get(dataTypeIndex))
			{
			  case kDataType_Number:
				{
					for( UniCharCount i = 0; i < unicodeLen; i++ )
					{
						UniChar	ch = unicodeStr[i];
						if( (ch >= '0' && ch <= '9') || ch < 0x20 || ch == '-' )   continue;
						else
						{
							done = true;
							break;
						}
					}
					break;
				}
			  case kDataType_FloatNumber:
				{
					for( UniCharCount i = 0; i < unicodeLen; i++ )
					{
						UniChar	ch = unicodeStr[i];
						if( (ch >= '0' && ch <= '9') || ch < 0x20 || ch == '-' || ch == '.' )   continue;
						else
						{
							done = true;
							break;
						}
					}
					break;
					
				}
			}
		}
	}
	if( done == false )
	{
		err = ::CallNextEventHandler(inCallRef,inEventRef);
		//if( err != noErr )   _ACErrorNum("CallNextEventHandler() returned error: ",err,this);CallNextEventHandler()�͌��ǉ������Ȃ������ꍇnotHandled��Ԃ�
		done = true;
		if( IsWindowCreated() == true )//B0000 CallNextEventHandler()�ŃE�C���h�E��������\�������邽��
		{
			//Text���͂�ʒm
			if( (	(len == 2 && unicodeStr[0] == 0x001C) || (len == 2 && unicodeStr[0] == 0x001D) ||
					(len == 2 && unicodeStr[0] == 0x001E) || (len == 2 && unicodeStr[0] == 0x001F) ) == false )//#7 �J�[�\���L�[�����ŕҏW�Ƃ݂Ȃ������Ή�
			GetAWin().EVT_TextInputted(controlID);
		}
	}
	
	return done;
}

/**
TSM���̓C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoTextInputUpdateActiveInputArea( const EventHandlerCallRef inCallRef, const EventRef inEventRef, ABool& outUpdateMenu )
{
	OSStatus	err = noErr;
	
	Boolean	done = false;
	outUpdateMenu = true;
	
	//�e�L�X�g�擾
	UInt32	actSize = 0;
	err = ::GetEventParameter(inEventRef,kEventParamTextInputSendText,typeUnicodeText,NULL,0,&actSize,NULL);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	AText	inputUTF16Text;
	if( actSize > 0 )//#182 �f�[�^�����݂���ꍇ�̂ݎ擾
	{
		inputUTF16Text.Reserve(0,actSize);
		{//#598
			AStTextPtr	textptr(inputUTF16Text,0,inputUTF16Text.GetItemCount());
			err = ::GetEventParameter(inEventRef,kEventParamTextInputSendText,typeUnicodeText,NULL,textptr.GetByteCount(),&actSize,textptr.GetPtr());
			//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
		}
		if( inputUTF16Text.GetItemCount() != actSize )   _ACError("",this);
	}
	AText	inputUTF8Text;
	inputUTF8Text.SetText(inputUTF16Text);
	inputUTF8Text.ConvertToUTF8FromUTF16();
	inputUTF8Text.GetUTF16TextIndexInfo(mCurrentInlineInputUTF16PosIndexArray,mCurrentInlineInputUTF8PosIndexArray);
	
	//�m�蕶����(UTF-16�����̕������j�擾
	SInt32	fixlen = 0;
	err = ::GetEventParameter(inEventRef,kEventParamTextInputSendFixLen,typeLongInteger,NULL,sizeof(fixlen),NULL,&(fixlen));
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	if( fixlen >= inputUTF16Text.GetItemCount()/sizeof(UniChar) )   fixlen = inputUTF16Text.GetItemCount()/sizeof(UniChar);
	
	//InlineInput�n�C���C�g�������擾 
	actSize = 0;
	err = ::GetEventParameter(inEventRef,kEventParamTextInputSendHiliteRng,typeTextRangeArray,NULL,0,&actSize,NULL);
	AArray<AIndex>	hiliteLineStyleIndex;
	AArray<AIndex>	hiliteStartPos;
	AArray<AIndex>	hiliteEndPos;
	if( actSize > 0 )//#182 hilite�f�[�^�����݂���ꍇ�̂ݎ擾����悤�ɂ���
	{
		//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
		TextRangeArrayPtr hiliteRangePtr = (TextRangeArrayPtr)::NewPtrClear(actSize);
		if( hiliteRangePtr == NULL )   _ACThrow("cannot allocate memory(text input update active input area)",this);
		err = ::GetEventParameter(inEventRef,kEventParamTextInputSendHiliteRng,typeTextRangeArray,NULL,actSize,NULL,hiliteRangePtr);
		//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
		for( AIndex index = 0; index < hiliteRangePtr->fNumOfRanges; index++ )
		{
			if( hiliteRangePtr->fRange[index].fHiliteStyle /*B0351<=*/== 1/*kCaretPosition*/ )
			{
				hiliteLineStyleIndex.Add(kIndex_Invalid);
			}
			else if( hiliteRangePtr->fRange[index].fHiliteStyle >= 2 )//B0351
			{
				hiliteLineStyleIndex.Add(hiliteRangePtr->fRange[index].fHiliteStyle-2);
			}
			else//B0351
			{
				continue;
			}
			AIndex	utf16Start = hiliteRangePtr->fRange[index].fStart;
			if( utf16Start >= inputUTF16Text.GetItemCount() )   utf16Start = inputUTF16Text.GetItemCount();
			hiliteStartPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(utf16Start/sizeof(UniChar)));
			AIndex	utf16End = hiliteRangePtr->fRange[index].fEnd;
			if( utf16End >= inputUTF16Text.GetItemCount() )   utf16End = inputUTF16Text.GetItemCount();
			hiliteEndPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(utf16End/sizeof(UniChar)));
		}
		::DisposePtr((Ptr)hiliteRangePtr);
	}
	
	//���݂̃t�H�[�J�X��ControlID���擾
	AControlID	controlID = mCurrentFocus;
	
	//�e�L�X�g���͏����i�E�C���h�E�^�r���[�őS�ď�������ꍇ�j
	if( controlID != kControlID_Invalid )
	{
		//
		if( GetAWin().EVT_DoInlineInput(controlID,inputUTF8Text,mCurrentInlineInputUTF8PosIndexArray.Get(fixlen),
				hiliteLineStyleIndex,hiliteStartPos,hiliteEndPos,outUpdateMenu) == true )   return true;
	}
	
	//\�L�[���́A���A�o�b�N�X���b�V���ϊ����[�h�̏ꍇ B0332
	if( inputUTF8Text.GetItemCount() == 2 && inputUTF8Text.Get(0) == 0xC2 && inputUTF8Text.Get(1) == 0xA5 && IsInputBackslashByYenKeyMode() == true && fixlen > 0 )
	{
		AText	backslash;
		backslash.Add(0x5C);//backslash(UTF-8)
		InputText(controlID,backslash);
		done = true;
	}
	
	if( done == false )
	{
		err = ::CallNextEventHandler(inCallRef,inEventRef);
		//if( err != noErr )   _ACErrorNum("CallNextEventHandler() returned error: ",err,this);
		done = true;
	}
	
	//Text���͂�ʒm
	GetAWin().EVT_TextInputted(controlID);
	
	return done;
}

/**
TSM�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoTextInputOffsetToPos( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//���݂̃t�H�[�J�X��ControlID���擾
	AControlID	controlID = mCurrentFocus;
	if( controlID != kControlID_Invalid )
	{
		SInt32	offsetUTF16 = 0;
		err = ::GetEventParameter(inEventRef,kEventParamTextInputSendTextOffset,typeLongInteger,NULL,sizeof(offsetUTF16),NULL,&(offsetUTF16));
		//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
		if( offsetUTF16 > mCurrentInlineInputUTF8PosIndexArray.GetItemCount()*sizeof(UniChar) )   offsetUTF16 = mCurrentInlineInputUTF8PosIndexArray.GetItemCount()*sizeof(UniChar);
		AIndex	offsetUTF8 = 0;
		if( offsetUTF16/sizeof(UniChar) < mCurrentInlineInputUTF8PosIndexArray.GetItemCount() )
		{
			mCurrentInlineInputUTF8PosIndexArray.Get(offsetUTF16/sizeof(UniChar));
		}
		ALocalPoint	localPoint;
		if( GetAWin().EVT_DoInlineInputOffsetToPos(controlID,offsetUTF8,localPoint) == true )
		{
			AWindowPoint	windowPoint;
			GetWindowPointFromControlLocalPoint(controlID,localPoint,windowPoint);
			Point	pt;
			pt.h = windowPoint.x;
			pt.v = windowPoint.y;
			StSetPort	s(mWindowRef);
			::LocalToGlobal(&pt);
			err = ::SetEventParameter(inEventRef,kEventParamTextInputReplyPoint,typeQDPoint,sizeof(pt),&pt);
			if( err != noErr )   _ACErrorNum("SetEventParameter() returned error: ",err,this);
			return true;
		}
	}
	return false;
}

/**
TSM�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoTextInputPosToOffset( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//���݂̃t�H�[�J�X��ControlID���擾
	AControlID	controlID = mCurrentFocus;
	if( controlID != kControlID_Invalid )
	{
		Point	pt = {0,0};
		err = ::GetEventParameter(inEventRef,kEventParamTextInputSendCurrentPoint,typeQDPoint,NULL,sizeof(pt),NULL,&(pt));
		//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
		StSetPort	s(mWindowRef);
		::GlobalToLocal(&pt);
		AWindowPoint	windowPoint;
		windowPoint.x = pt.h;
		windowPoint.y = pt.v;
		ALocalPoint	localPoint;
		GetControlLocalPointFromWindowPoint(controlID,windowPoint,localPoint);
		AIndex	offsetUTF8;
		if( GetAWin().EVT_DoInlineInputPosToOffset(controlID,localPoint,offsetUTF8) == true )
		{
			SInt32	offsetUTF16 = 0;
			if( offsetUTF8 >= 0 && offsetUTF8 < mCurrentInlineInputUTF16PosIndexArray.GetItemCount() )
			{
				offsetUTF16 = mCurrentInlineInputUTF16PosIndexArray.Get(offsetUTF8) * sizeof(UniChar);
				err = ::SetEventParameter(inEventRef,kEventParamTextInputReplyTextOffset,typeLongInteger,sizeof(offsetUTF16),&offsetUTF16);
				if( err != noErr )   _ACErrorNum("SetEventParameter() returned error: ",err,this);
			}
			return true;
		}
	}
	return false;
}

/**
TSM�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoTextInputGetSelectedText( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//���݂̃t�H�[�J�X��ControlID���擾
	AControlID	controlID = mCurrentFocus;
	if( controlID != kControlID_Invalid )
	{
		AText	text;
		if( GetAWin().EVT_DoInlineInputGetSelectedText(controlID,text) == true )
		{
			text.ConvertFromUTF8ToUTF16();
			AStTextPtr	textptr(text,0,text.GetItemCount());
			err = ::SetEventParameter(inEventRef,kEventParamTextInputReplyText,typeUnicodeText,textptr.GetByteCount(),textptr.GetPtr());
			if( err != noErr )   _ACErrorNum("SetEventParameter() returned error: ",err,this);
			
			//B0330 TEST
			/*ScriptLanguageRecord	slr;
			slr.fScript = smCurrentScript;
			slr.fLanguage = langJapanese;
			::SetEventParameter(inEventRef,kEventParamTextInputSendSLRec,typeIntlWritingCode,sizeof( ScriptLanguageRecord ),&slr);*/
			
			return true;
		}
	}
	return false;
}

/**
WindowActivated�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoWindowFocusAcquiredHandler( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//�t���[�e�B���O�E�C���h�E�̏ꍇ��UserFocus���t���[�e�B���O�E�C���h�E�ł͂Ȃ��őO�ʂ̃E�C���h�E�ֈړ�������
	if( IsFloatingWindow() == true )
	{
		WindowRef	nonfloat = ::FrontNonFloatingWindow();
		if( nonfloat != NULL )
		{
			err = ::SetUserFocusWindow(nonfloat);
			if( err != noErr )   _ACErrorNum("SetUserFocusWindow() returned error: ",err,this);
		}
		return true;
	}
	/* if( mWindowStyle == kWindowStyle_Drawer )
	{
		::SetUserFocusWindow(mParentWindowRef);
	}*/
	//B0367
	if( mWindowStyle == kWindowStyle_Drawer )
	{
		mParentWindow->GetAWin().NVI_SwitchFocusTo(kControlID_Invalid);
	}
	return false;
}

/**
WindowActivated�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoWindowActivated( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//�ŏ���CallNextEventHandler+HIViewRender�����Ă����Ȃ��ƁAOSX10.3�ŁA�^�C�g���o�[�̕\���������i�C���A�N�e�B�u�ɂȂ�j
	//EVT_WindowActivated()�̂Ȃ���HIViewRender���s���̂��e�����Ă���H
	err = ::CallNextEventHandler(inCallRef,inEventRef);
	if( err != noErr )   _ACErrorNum("CallNextEventHandler() returned error: ",err,this);
	err = ::HIViewRender(::HIViewGetRoot(mWindowRef));
	if( err != noErr )   _ACErrorNum("HIViewRender() returned error: ",err,this);
	
	GetAWin().EVT_WindowActivated();
	
	//UserPane��Activated�ʒm
	for( AIndex i = 0; i < mUserPaneArray.GetItemCount(); i++ )
	{
		mUserPaneArray.GetObject(i).DoWindowActivated();
	}
	
	//�t�H�[�J�X�𕜌�
	if( mLatentFocus != kControlID_Invalid )
	{
		SetFocus(mLatentFocus);
		//#0 mLatentFocus = kControlID_Invalid;
	}
	/*#424 WindowClass��kWindowClass_Document�̏ꍇ�̂�TSM���䂷��ioverlay�E�C���h�E�ő�ʂ�activate����ƈُ�Ɏ��Ԃ�������j
	if( IsFloatingWindow() == true )//#138
	{
		//���t���[�e�B���O�E�C���h�E�̏ꍇ��TSM����͖��Ή�
	}
	else
	*/
	if( mWindowClass == kWindowClass_Document )//#424
	{
		err = ::ActivateTSMDocument(mTSMID);
		if( err != noErr )   _ACErrorNum("ActivateTSMDocument() returned error: ",err,this);
	}
	return true;
}

/**
WindowDeactivated�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoWindowDeactivated( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	/*#424 WindowClass��kWindowClass_Document�̏ꍇ�̂�TSM���䂷��ioverlay�E�C���h�E�ő�ʂ�activate����ƈُ�Ɏ��Ԃ�������j
	if( IsFloatingWindow() == true )//#138
	{
		//���t���[�e�B���O�E�C���h�E�̏ꍇ��TSM����͖��Ή�
	}
	else
	*/
	if( mWindowClass == kWindowClass_Document )//#424
	{
		err = ::FixTSMDocument(mTSMID);
		//if( err != noErr )   _ACErrorNum("FixTSMDocument() returned error: ",err,this);
		err = ::DeactivateTSMDocument(mTSMID);
		//if( err != noErr )   _ACErrorNum("DeactivateTSMDocument() returned error: ",err,this);
	}
	
	//���݂̃t�H�[�J�X��ۑ����āA�t�H�[�J�X���N���A
	if( mCurrentFocus != kControlID_Invalid )//#42 ����Focus���N���A����Ă���Ƃ��̓t�H�[�J�X�N���A�ALatent�ݒ�����Ȃ�
	{
		mLatentFocus = mCurrentFocus;
		ClearFocus(false);//#212
	}
	
	//UserPane��Deactivated�ʒm
	for( AIndex i = 0; i < mUserPaneArray.GetItemCount(); i++ )
	{
		mUserPaneArray.GetObject(i).DoWindowDeactivated();
	}
	
	//B0000
	err = ::CallNextEventHandler(inCallRef,inEventRef);
	if( err != noErr )   _ACErrorNum("CallNextEventHandler() returned error: ",err,this);
	err = ::HIViewRender(::HIViewGetRoot(mWindowRef));
	if( err != noErr )   _ACErrorNum("HIViewRender() returned error: ",err,this);
	
	GetAWin().EVT_WindowDeactivated();
	//B0000 ::CallNextEventHandler(inCallRef,inEventRef);
	return true;
}

/**
WindowCollapsed�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoWindowCollapsed( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	GetAWin().EVT_WindowCollapsed();
	err = ::CallNextEventHandler(inCallRef,inEventRef);
	//if( err != noErr )   _ACErrorNum("CallNextEventHandler() returned error: ",err,this);
	return true;
}

/**
����{�^���N���b�N���C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoWindowCloseButton( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	GetAWin().EVT_DoCloseButton();
	return true;
}

/**
�E�C���h�E�̈�ύX��C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoWindowBoundsChanged( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//win 080618
	AGlobalRect	current, prev;
	Rect	rect = {0,0,0,0};
	err = ::GetEventParameter(inEventRef,kEventParamPreviousBounds,typeQDRectangle,NULL,sizeof(rect),NULL,&(rect));
	if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	prev.left		= rect.left;
	prev.top		= rect.top;
	prev.right		= rect.right;
	prev.bottom		= rect.bottom;
	err = ::GetEventParameter(inEventRef,kEventParamCurrentBounds,typeQDRectangle,NULL,sizeof(rect),NULL,&(rect));
	if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	current.left	= rect.left;
	current.top		= rect.top;
	current.right	= rect.right;
	current.bottom	= rect.bottom;
	
	GetAWin().EVT_WindowBoundsChanged(prev,current);
	return false;
}

/**
�E�C���h�E���T�C�Y�ύX�ς݃C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoWindowResizeCompleted( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	GetAWin().EVT_WindowResizeCompleted();
	return false;
}

/**
�J�[�\���ύX�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoWindowCursorChange( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//�C�x���g�Ώۂ̃R���g���[����ControlID���擾
	HIViewRef	view;
	err = ::HIViewGetViewForMouseEvent(::HIViewGetRoot(mWindowRef),inEventRef,&view);
	if( err != noErr )   _ACErrorNum("HIViewGetViewForMouseEvent() returned error: ",err,this);
	
	if( view == NULL )   return false;
	
	AControlID	controlID = GetControlID(view);
	
	//�R���g���[����ʂ��擾
	ControlKind	kind;
	err = ::GetControlKind(view,&kind);
	if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
	
	//NavSheet�\�����͖��J�[�\��
	//#1034 if( mNavSheet != NULL )
	if( mHasSheet == true )//#1034
	{
		err = ::SetThemeCursor(kThemeArrowCursor);
		if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,this);
		return true;
	}
	//UserPane�̏ꍇ��UserPane�̃��\�b�h���R�[��
	if( IsUserPane(controlID) == true )
	{
		return GetUserPane(controlID).DoAdjustCursor(inEventRef);
	}
	//�e�L�X�g�n�̃R���g���[���̏ꍇ��I�r�[���J�[�\���ɕύX
	else if( IsActive() && 
			(kind.kind == kControlKindEditText || kind.kind == kControlKindHIComboBox ||
			kind.kind == kControlKindEditUnicodeText) )
	{
		err = ::SetThemeCursor(kThemeIBeamCursor);
		if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,this);
	}
	//����ȊO�͖��J�[�\���ɕύX
	else
	{
		err = ::SetThemeCursor(kThemeArrowCursor);
		if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,this);
	}
	return true;
}

/**
Drag Tracking���C�x���g�n���h��
*/
void	CWindowImp::APICB_DoDragTracking( const DragTrackingMessage inMessage, const DragRef inDragRef )
{
	OSStatus	err = noErr;
	
	Point	pt;
	err = ::GetDragMouse(inDragRef,&pt,NULL);
	if( err != noErr )   _ACErrorNum("GetDragMouse() returned error: ",err,this);
	::GlobalToLocal(&pt);
	/*#236
	AControlRef	controlRef;
	::FindControl(pt,mWindowRef,&controlRef);
	AControlID	controlID = GetControlID(controlRef);
	*/
	AWindowPoint	windowPoint;
	windowPoint.x = pt.h;
	windowPoint.y = pt.v;
	//#236 Drop�\��Control������Drop��Control��T��
	AControlID	controlID = FindDroppedControl(windowPoint);
	//
	//#236 ALocalPoint	localPoint;
	//#236 GetControlLocalPointFromWindowPoint(controlID,windowPoint,localPoint);
	SInt16	modifiers = 0;
	err = ::GetDragModifiers(inDragRef,&modifiers,NULL,NULL);
	if( err != noErr )   _ACErrorNum("GetDragModifiers() returned error: ",err,this);
	switch(inMessage)
	{
		/*#236
		kDragTrackingEnterWindow, kDragTrackingInWindow, kDragTrackingLeaveWindow�͂����܂�
		Window�ɓ������ETracking�E�o���Ƃ��ɒʒm�����C�x���g�Ȃ̂ŁAControl�P�ʂɕϊ�����B
		*/
	  case kDragTrackingEnterWindow:
		{
			/*#236
			if( IsUserPane(controlID) == true )
			{
				GetUserPane(controlID).DoDragEnter(inDragRef,localPoint,modifiers);
			}
			*/
			//#236�@�V���ɓ�����Control��DoDragEnter�𑗐M
			if( controlID != kControlID_Invalid )
			{
				ALocalPoint	localPoint;
				GetControlLocalPointFromWindowPoint(controlID,windowPoint,localPoint);
				GetUserPane(controlID).DoDragEnter(inDragRef,localPoint,modifiers);
			}
			//mDragTrackingControl���X�V
			//�������ꏊ�ɓo�^Control�������ꍇ��mDragTrackingControl��Invalid�ݒ肳���
			mDragTrackingControl = controlID;
			break;
		}
	  case kDragTrackingInWindow:
		{
			/*#236
			if( IsUserPane(controlID) == true )
			{
				GetUserPane(controlID).DoDragTracking(inDragRef,localPoint,modifiers);
			}
			*/
			//#236 ���݂̃}�E�X�ʒu���ATracking����ControlID���ɖ����ꍇ�́A����Tracking����Control��DoDragLeave�𑗐M
			if( mDragTrackingControl != kControlID_Invalid && mDragTrackingControl != controlID )
			{
				ALocalPoint	localPoint;
				GetControlLocalPointFromWindowPoint(mDragTrackingControl,windowPoint,localPoint);
				GetUserPane(mDragTrackingControl).DoDragLeave(inDragRef,localPoint,modifiers);
				mDragTrackingControl = kControlID_Invalid;
			}
			//#236�@�}�E�X�ʒu��Control��DoDragEnter, DoDragTracking�𑗐M
			//DoDragEnter�͐V���ɂ���Control�ɓ������ꍇ�̂ݑ��M����B
			if( controlID != kControlID_Invalid )//#236
			{
				ALocalPoint	localPoint;
				GetControlLocalPointFromWindowPoint(controlID,windowPoint,localPoint);
				if( mDragTrackingControl != controlID )
				{
					GetUserPane(controlID).DoDragEnter(inDragRef,localPoint,modifiers);
				}
				GetUserPane(controlID).DoDragTracking(inDragRef,localPoint,modifiers);
			}
			//mDragTrackingControl���X�V
			//�������ꏊ�ɓo�^Control�������ꍇ��mDragTrackingControl��Invalid�ݒ肳���
			mDragTrackingControl = controlID;
			break;
		}
	  case kDragTrackingLeaveWindow:
		{
			/*#236
			if( IsUserPane(controlID) == true )
			{
				GetUserPane(controlID).DoDragLeave(inDragRef,localPoint,modifiers);
			}
			*/
			//#236�@����Tracking����Control��DoDragLeave�𑗐M
			if( mDragTrackingControl != kControlID_Invalid )
			{
				if( ExistControl(mDragTrackingControl) == true )//#0 Drag&Drop��ɂ���View��delete�����ꍇ�A���̌�leave�������Ƃ��ɂ�View���폜����Ă��邽�߃`�F�b�N�K�v
				{
					ALocalPoint	localPoint;
					GetControlLocalPointFromWindowPoint(mDragTrackingControl,windowPoint,localPoint);
					GetUserPane(mDragTrackingControl).DoDragLeave(inDragRef,localPoint,modifiers);
				}
				mDragTrackingControl = kControlID_Invalid;
			}
			break;
		}
	  default:
		{
			//��������
			break;
		}
	}
}

/**
Drag Receive�C�x���g�n���h��
*/
void	CWindowImp::APICB_DoDragReceive( const DragRef inDragRef )
{
	OSStatus	err = noErr;
	
	Point	pt;
	err = ::GetDragMouse(inDragRef,&pt,NULL);
	if( err != noErr )   _ACErrorNum("GetDragMouse() returned error: ",err,this);
	::GlobalToLocal(&pt);
	/*#236
	AControlRef	controlRef;
	::FindControl(pt,mWindowRef,&controlRef);
	AControlID	controlID = GetControlID(controlRef);
	*/
	AWindowPoint	windowPoint;
	windowPoint.x = pt.h;
	windowPoint.y = pt.v;
	//#236 Drop�\��Control������Drop��Control��T��
	AControlID	controlID = FindDroppedControl(windowPoint);
	if( controlID == kControlID_Invalid )   return;
	//
	ALocalPoint	localPoint;
	GetControlLocalPointFromWindowPoint(controlID,windowPoint,localPoint);
	SInt16	modifiers;
	err = ::GetDragModifiers(inDragRef,&modifiers,NULL,NULL);
	if( err != noErr )   _ACErrorNum("GetDragModifiers() returned error: ",err,this);
	if( IsUserPane(controlID) == true )
	{
		GetUserPane(controlID).DoDragReceive(inDragRef,localPoint,modifiers);
	}
	//R0000
	//�t�@�C��Drop
	AFileAcc	file;
	if( AScrapWrapper::GetFileDragData(inDragRef,file) == true )
	{
		GetAWin().EVT_DoFileDropped(controlID,file);
	}
}

//#236
/**
Drop���ꂽWindowPoint����Control����������
*/
AControlID	CWindowImp::FindDroppedControl( const AWindowPoint inPoint ) const
{
	AItemCount	itemCount = mDropEnableControlArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AControlID	controlID = mDropEnableControlArray.Get(i);
		if( IsControlVisible(controlID) == true )
		{
			AWindowPoint	wpt;
			GetControlPosition(controlID,wpt);
			ANumber	w = GetControlWidth(controlID);
			ANumber	h = GetControlHeight(controlID);
			if( inPoint.x >= wpt.x && inPoint.x <= wpt.x + w &&
						inPoint.y >= wpt.y && inPoint.y <= wpt.y + h )
			{
				return mDropEnableControlArray.Get(i);
			}
		}
	}
	return kControlID_Invalid;
}

//B0370
/**
GetTypes�T�[�r�X�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoServiceGetTypes( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	AArray<AScrapType>	copyTypeArray, pasteTypeArray;
	if( GetAWin().EVT_DoServiceGetTypes(copyTypeArray,pasteTypeArray) == true )
	{
		CFMutableArrayRef	aCopyTypes = NULL;
		err = ::GetEventParameter(inEventRef,kEventParamServiceCopyTypes,typeCFMutableArrayRef,NULL,sizeof(CFMutableArrayRef),NULL,&aCopyTypes);
		//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
		if( aCopyTypes != NULL )
		{
			for( AIndex i = 0; i < copyTypeArray.GetItemCount(); i++ )
			{
				CFStringRef	strref = ::CreateTypeStringWithOSType(copyTypeArray.Get(i));
				if( strref == NULL )   break;
				::CFArrayAppendValue(aCopyTypes,strref);
				::CFRelease(strref);
			}
		}
		CFMutableArrayRef	aPasteTypes = NULL;
		err = ::GetEventParameter(inEventRef,kEventParamServicePasteTypes,typeCFMutableArrayRef,NULL,sizeof(CFMutableArrayRef),NULL,&aPasteTypes);
		//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
		if( aPasteTypes != NULL )
		{
			for( AIndex i = 0; i < pasteTypeArray.GetItemCount(); i++ )
			{
				CFStringRef	strref = ::CreateTypeStringWithOSType(pasteTypeArray.Get(i));
				if( strref == NULL )   break;
				::CFArrayAppendValue(aPasteTypes,strref);
				::CFRelease(strref);
			}
		}
	}
	return true;
}

//B0370
/**
Copy�T�[�r�X�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoServiceCopy( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	ScrapRef	scrap = NULL;
	err = ::GetEventParameter(inEventRef,kEventParamScrapRef,typeScrapRef,NULL,sizeof(ScrapRef),NULL,&scrap);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	if( scrap == NULL )   return false;
	return GetAWin().EVT_DoServiceCopy(scrap);
}

//B0370
/**
Paste�T�[�r�X�C�x���g�n���h��
*/
ABool	CWindowImp::APICB_DoServicePaste( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	ScrapRef	scrap = NULL;
	err = ::GetEventParameter(inEventRef,kEventParamScrapRef,typeScrapRef,NULL,sizeof(ScrapRef),NULL,&scrap);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	if( scrap == NULL )   return false;
	return GetAWin().EVT_DoServicePaste(scrap);
}

/**
CAppImp����R�[�������MouseExited�C�x���g�n���h��

mouse moved�C�x���g��Window�ŏ�������Ȃ������ꍇ�ɁAsMouseTrackingWindowRef�̃E�C���h�E�ɑ΂��ăR�[�������
*/
void	CWindowImp::DoMouseExited( const AControlID inID, const EventRef inEventRef )//R0228
{
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).DoMouseExited(inEventRef);//R0228
	}
}
#endif

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---�R���g���[��ID/Ref�ϊ�
/**
ControlRef(HIViewRef)�̎擾
*/
AControlRef	CWindowImp::GetControlRef( const AControlID inID ) const
{
	OSStatus	err = noErr;
	
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACThrow("Window not created",this);return NULL;}
	
	//#353 -2��tabcontrol�Ɋ��蓖�Ă�Btabcontrol�ւ̃t�H�[�J�X�ݒ�̂��߁B
	if( inID == kControlID_TabControl )
	{
		return GetControlRef('ttab',0);
	}
	
	//#379 HIViewFindByID()�d���̂ŁA�������i���[�U�[�y�C���Ȃ炻�̃N���X����擾�j
	AIndex	userPaneIndex = mUserPaneArray_ControlID.Find(inID);
	if( userPaneIndex != kIndex_Invalid )
	{
		//#688 return mUserPaneArray.GetObjectConst(userPaneIndex).GetControlRef();
		return mUserPaneArray_ControlRef.Get(userPaneIndex);//#688
	}
	
	//
	AControlRef	result = NULL;
	HIViewID	viewid;
	viewid.signature = 0;
	viewid.id = inID;
	err = ::HIViewFindByID(::HIViewGetRoot(mWindowRef), viewid, &result);
	//if( err != noErr )   _ACErrorNum("HIViewFindByID() returned error: ",err,this);
	return result;
}

/**
ControlRef(HIViewRef)�̎擾�i��{�I��signature��0�Œ肾���ATab������Ȃ��̂̂��߁j
*/
AControlRef	CWindowImp::GetControlRef( const OSType inSignature, const AControlID inID ) const
{
	OSStatus	err = noErr;
	
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACThrow("Window not created",this);return NULL;}
	
	AControlRef	result = NULL;
	HIViewID	viewid;
	viewid.signature = inSignature;
	viewid.id = inID;
	err = ::HIViewFindByID(::HIViewGetRoot(mWindowRef), viewid, &result);
	//if( err != noErr )   _ACErrorNum("HIViewFindByID() returned error: ",err,this);
	return result;
}

/**
�R���g���[��ID�̎擾
*/
AControlID	CWindowImp::GetControlID( const AControlRef inControlRef ) const
{
	OSStatus	err = noErr;
	
	HIViewID	id;
	err = ::GetControlID(inControlRef,&id);
	//if( err != noErr )   _ACErrorNum("GetControlID() returned error: ",err,this);
	return id.id;
}

/**
���j���[Ref�̎擾
*/
ACarbonMenuRef	CWindowImp::GetMenuRef( const AControlID inID ) const
{
	//Carbon�E�C���h�E�̏ꍇ
	
	OSStatus	err = noErr;
	
	MenuRef	menuref = NULL;
	err = ::GetControlData(GetControlRef(inID),0,kControlPopupButtonMenuRefTag,sizeof(menuref),&menuref,NULL);
	//if( err != noErr )   _ACErrorNum("GetControlData() returned error: ",err,this);
	if( menuref != NULL )   return menuref;
	err = ::GetControlData(GetControlRef(inID),0,kControlBevelButtonMenuRefTag,sizeof(menuref),&menuref,NULL);
	//if( err != noErr )   _ACErrorNum("GetControlData() returned error: ",err,this);
	return menuref;
}
#endif

#pragma mark ===========================

#pragma mark ---�t�@�C���^�t�H���_�I��

//�t�H���_�I���V�[�g�\��
//inVirtualControlID: �t�H���_�I����������AWindow�փR�[���o�b�N�����Ƃ��ɒʒm�����i���z�I�ȁjControlID
void	CWindowImp::ShowChooseFolderWindow( const AFileAcc& inDefaultFolder, const AText& inMessage,
		const AControlID inVirtualControlID, const ABool inSheet )//#551
{
	//#1034
	//virtual control ID�ݒ�
	mChooseFolderVirtualControlID = inVirtualControlID;
	//�f�t�H���g�t�H���_�p�X�ݒ�
	AText	dirpathtext;
	inDefaultFolder.GetPath(dirpathtext);
	NSString*	dirpath = nil;
	if( dirpathtext.GetItemCount() > 0 )
	{
		AStCreateNSStringFromAText	dirpathstr(dirpathtext);
		dirpath = [[[NSString alloc] initWithString:dirpathstr.GetNSString()] autorelease];
	}
	//���b�Z�[�W�ݒ�
	NSString*	message = nil;
	if( inMessage.GetItemCount() > 0 )
	{
		AStCreateNSStringFromAText	messagestr(inMessage);
		message = [[[NSString alloc] initWithString:messagestr.GetNSString()] autorelease];
	}
	//�V�[�g���[�h�Ȃ�V�[�g�t���O�ݒ�
	if( inSheet == true )
	{
		mHasSheet = true;
	}
	//�t�H���_�I���V�[�g�^�_�C�A���O�\��
	[CocoaObjects->GetWindow() showChooseFolderWindow:dirpath message:message isSheet:inSheet];
	/*#1034
	OSStatus	err = noErr;
	
	if( mNavSheet != NULL )   {_ACError("Nav already displayed",this);return;}
	NavDialogCreationOptions	options;
	err = ::NavGetDefaultDialogCreationOptions(&options);
	if( err != noErr )   _ACErrorNum("NavGetDefaultDialogCreationOptions() returned error: ",err,this);
	if( inSheet == true )
	{
		options.modality = kWindowModalityWindowModal;
		options.parentWindow = mWindowRef;
	}
	else
	{
		options.modality = kWindowModalityAppModal;
	}
	if( inMessage.GetItemCount() > 0 )
	{
		options.message = inMessage.CreateCFString();
	}
	mChooseFolderVirtualControlID = inVirtualControlID;
	mDefaultFile = inDefaultFolder;//#551
	err = ::NavCreateChooseFolderDialog(&options,::NewNavEventUPP(CWindowImp::STATIC_NavChooseFolderCB),NULL,this,&mNavSheet);
	if( err != noErr )   _ACErrorNum("NavCreateChooseFolderDialog() returned error: ",err,this);
	err = ::NavDialogRun(mNavSheet);
	if( err != noErr )   _ACErrorNum("NavDialogRun() returned error: ",err,this);
	*/
}

/**
�t�H���_�I���V�[�g�R�[���o�b�N
�i�L�����Z�����͈����󕶎��ŃR�[�������j
*/
void	CWindowImp::APICB_ShowChooseFolderWindowAction( const AText& inPath )
{
	//#1034
	//�V�[�g�t���O����
	mHasSheet = false;
	//�t�H���_�I�����������s
	if( inPath.GetItemCount() > 0 )
	{
		AFileAcc	file;
		file.Specify(inPath);
		GetAWin().EVT_FolderChoosen(mChooseFolderVirtualControlID,file);
	}
	//virtual control ID������
	mChooseFolderVirtualControlID = kControlID_Invalid;
}
/*#1034
void	CWindowImp::DoNavChooseFolderCB( const NavEventCallbackMessage selector, const NavCBRecPtr params )
{
	OSStatus	err = noErr;
	
	switch( selector )
	{
	  case kNavCBUserAction:
		{
			NavUserAction	userAction = ::NavDialogGetUserAction(mNavSheet);
			if( userAction != kNavUserActionCancel && userAction != kNavUserActionNone )
			{
				NavReplyRecord	reply;
				if( ::NavDialogGetReply(mNavSheet,&reply) == noErr )
				{
					if( reply.validRecord )
					{
						SInt32	count;
						err = ::AECountItems(&(reply.selection),&count);
						if( count >= 1 )
						{
							FSRef	fsref;
							AEKeyword	key;
							DescType	type;
							Size	size;
							err = ::AEGetNthPtr(&(reply.selection),1,typeFSRef,&key,&type,(Ptr)&fsref,sizeof(FSRef),&size);
							AFileAcc	file;
							file.SpecifyByFSRef(fsref);
							GetAWin().EVT_FolderChoosen(mChooseFolderVirtualControlID,file);
						}
						err = ::NavDisposeReply(&reply);
						if( err != noErr )   _ACErrorNum("NavDisposeReply() returned error: ",err,this);
					}
				}
			}
			break;
		}
	  case kNavCBTerminate:
		::NavDialogDispose(mNavSheet);
		mNavSheet = NULL;
		AApplication::GetApplication().NVI_UpdateMenu();
		break;
		//�f�t�H���g�t�H���_ #551
	  case kNavCBStart:
		{
			if( mDefaultFile.Exist() == true )
			{
				AEDesc	desc;
				err = ::AECreateDesc(typeNull,NULL,0,&desc);
				FSRef	defaultFile;
				mDefaultFile.GetFSRef(defaultFile);
				err = ::AECreateDesc(typeFSRef,(Ptr)(&defaultFile),sizeof(FSRef),&desc);
				err = ::NavCustomControl(params->context,kNavCtlSetLocation,&desc);
				//if( err != noErr )   _ACErrorNum("NavCustomControl() returned error: ",err,this);
				err = ::AEDisposeDesc(&desc);
			}
			break;
		}
	}
}
*/
/**
�t�H���_�I���V�[�g�R�[���o�b�N�iOS�R�[���o�b�N�pstatic�j
*/
/*#1034
pascal void	CWindowImp::STATIC_NavChooseFolderCB( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD )
{
	//�C�x���g�g�����U�N�V�����O�����^�㏈�� #0 �����ɂ��ꂪ�Ȃ��ƁARefresh���o�R��OS����C�x���gCB���������Ƃ��ɁA������sEventDepth==1�ƂȂ��Ă��܂��A
	//Window���̃I�u�W�F�N�g�ɑ΂���DoObjectArrayItemTrashDelete()���������A�����o�[�ϐ��ւ̃������A�N�Z�X���������A�����Ă��܂��B
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		CWindowImp*	window = (CWindowImp*)callbackUD;
		if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return;
		window->DoNavChooseFolderCB(selector,params);
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_NavChooseFolderCB() caught exception.",NULL);//#199
	}
}
*/

/**
�t�@�C���I���V�[�g�\��
@param inVirtualControlID �t�H���_�I����������AWindow�փR�[���o�b�N�����Ƃ��ɒʒm�����i���z�I�ȁjControlID
*/
void	CWindowImp::ShowChooseFileWindow( const AFileAcc& inDefaultFolder, const AText& inMessage,
		const AControlID inVirtualControlID, const ABool inOnlyApp, const ABool inSheet )
{
	//#1034
	//virtual control ID�ݒ�
	mChooseFileVirtualControlID = inVirtualControlID;
	//�f�t�H���g�t�H���_�p�X�ݒ�
	AText	dirpathtext;
	inDefaultFolder.GetPath(dirpathtext);
	NSString*	dirpath = nil;
	if( dirpathtext.GetItemCount() > 0 )
	{
		AStCreateNSStringFromAText	dirpathstr(dirpathtext);
		dirpath = [[[NSString alloc] initWithString:dirpathstr.GetNSString()] autorelease];
	}
	//���b�Z�[�W�ݒ�
	NSString*	message = nil;
	if( inMessage.GetItemCount() > 0 )
	{
		AStCreateNSStringFromAText	messagestr(inMessage);
		message = [[[NSString alloc] initWithString:messagestr.GetNSString()] autorelease];
	}
	//�V�[�g���[�h�Ȃ�V�[�g�t���O�ݒ�
	if( inSheet == true )
	{
		mHasSheet = true;
	}
	//�t�@�C���I���V�[�g�^�_�C�A���O�\��
	[CocoaObjects->GetWindow() showChooseFileWindow:dirpath message:message isSheet:inSheet onlyApp:inOnlyApp];
	/*#1034
	OSStatus	err = noErr;
	
	if( mNavSheet != NULL )   {_ACError("Nav already displayed",this);return;}
	NavDialogCreationOptions	options;
	err = ::NavGetDefaultDialogCreationOptions(&options);
	if( err != noErr )   _ACErrorNum("NavGetDefaultDialogCreationOptions() returned error: ",err,this);
	options.optionFlags |= kNavSupportPackages;
	if( inMessage.GetItemCount() > 0 )
	{
		options.message = inMessage.CreateCFString();
	}
	if( inSheet == true )
	{
		options.modality = kWindowModalityWindowModal;
		options.parentWindow = mWindowRef;
	}
	else
	{
		options.modality = kWindowModalityAppModal;
	}
	if( inOnlyApp == true )
	{
		err = ::NavCreateChooseFileDialog(&options,NULL,::NewNavEventUPP(CWindowImp::STATIC_NavChooseFileCB),NULL,
				::NavObjectFilterProcPtr(CWindowImp::STATIC_NavFilterCB_ApplicationFile),this,&mNavSheet);
		if( err != noErr )   _ACErrorNum("NavCreateChooseFileDialog() returned error: ",err,this);
	}
	else
	{
		err = ::NavCreateChooseFileDialog(&options,NULL,::NewNavEventUPP(CWindowImp::STATIC_NavChooseFileCB),NULL,NULL,this,&mNavSheet);
		if( err != noErr )   _ACErrorNum("NavCreateChooseFileDialog() returned error: ",err,this);
	}
	mDefaultFile = inDefaultFile;
	mChooseFileVirtualControlID = inVirtualControlID;
	err = ::NavDialogRun(mNavSheet);
	if( err != noErr )   _ACErrorNum("NavDialogRun() returned error: ",err,this);
	*/
}

/**
�t�@�C���I���V�[�g�\��
@param inVirtualControlID �t�H���_�I����������AWindow�փR�[���o�b�N�����Ƃ��ɒʒm�����i���z�I�ȁjControlID
*/
void	CWindowImp::ShowChooseFileWindowWithFileFilter( const AFileAcc& inDefaultFile, const AControlID inVirtualControlID, const ABool inSheet,
		const AFileAttribute inFilterAttribute )
{
	//��������
	
	/*#1034
	OSStatus	err = noErr;
	
	if( mNavSheet != NULL )   {_ACError("Nav already displayed",this);return;}
	NavDialogCreationOptions	options;
	err = ::NavGetDefaultDialogCreationOptions(&options);
	if( err != noErr )   _ACErrorNum("NavGetDefaultDialogCreationOptions() returned error: ",err,this);
	options.optionFlags |= kNavSupportPackages;
	if( inSheet == true )
	{
		options.modality = kWindowModalityWindowModal;
		options.parentWindow = mWindowRef;
	}
	else
	{
		options.modality = kWindowModalityAppModal;
	}
	mNavFileFilter_Creator = inFilterAttribute.creator;
	mNavFileFilter_Type = inFilterAttribute.type;
	mDefaultFile = inDefaultFile;
	mChooseFileVirtualControlID = inVirtualControlID;
	err = ::NavCreateChooseFileDialog(&options,NULL,::NewNavEventUPP(CWindowImp::STATIC_NavChooseFileCB),NULL,
			::NavObjectFilterProcPtr(CWindowImp::STATIC_NavFilterCB_CreatorTypeFilter),this,&mNavSheet);
	if( err != noErr )   _ACErrorNum("NavCreateChooseFileDialog() returned error: ",err,this);
	err = ::NavDialogRun(mNavSheet);
	if( err != noErr )   _ACErrorNum("NavDialogRun() returned error: ",err,this);
	*/
}

/**
�t�@�C���I���V�[�g�R�[���o�b�N
�i�L�����Z�����͈����󕶎��ŃR�[�������j
*/
void	CWindowImp::APICB_ShowChooseFileWindowAction( const AText& inPath )
{
	//#1034
	//�V�[�g�t���O����
	mHasSheet = false;
	//�t�H���_�I�����������s
	if( inPath.GetItemCount() > 0 )
	{
		AFileAcc	file;
		file.Specify(inPath);
		GetAWin().EVT_FileChoosen(mChooseFileVirtualControlID,file);
	}
	//virtual control ID������
	mChooseFileVirtualControlID = kControlID_Invalid;
}
/*#1034
void	CWindowImp::DoNavChooseFileCB( const NavEventCallbackMessage selector, const NavCBRecPtr params )
{
	OSStatus	err = noErr;
	
	switch( selector )
	{
	  case kNavCBUserAction:
		{
			NavUserAction	userAction = ::NavDialogGetUserAction(mNavSheet);
			if( userAction != kNavUserActionCancel && userAction != kNavUserActionNone )
			{
				NavReplyRecord	reply;
				if( ::NavDialogGetReply(mNavSheet,&reply) == noErr )
				{
					if( reply.validRecord )
					{
						SInt32	count;
						err = ::AECountItems(&(reply.selection),&count);
						if( count >= 1 )
						{
							FSRef	fsref;
							AEKeyword	key;
							DescType	type;
							Size	size;
							err = ::AEGetNthPtr(&(reply.selection),1,typeFSRef,&key,&type,(Ptr)&fsref,sizeof(FSRef),&size);
							AFileAcc	file;
							file.SpecifyByFSRef(fsref);
							GetAWin().EVT_FileChoosen(mChooseFileVirtualControlID,file);
						}
						err = ::NavDisposeReply(&reply);
						if( err != noErr )   _ACErrorNum("NavDisposeReply() returned error: ",err,this);
					}
				}
			}
			break;
		}
	  case kNavCBTerminate:
		{
			::NavDialogDispose(mNavSheet);
			mNavSheet = NULL;
			AApplication::GetApplication().NVI_UpdateMenu();
			break;
		}
		//�f�t�H���g�t�@�C��
	  case kNavCBStart:
		{
			AEDesc	desc;
			err = ::AECreateDesc(typeNull,NULL,0,&desc);
			FSRef	defaultFile;
			mDefaultFile.GetFSRef(defaultFile);
			err = ::AECreateDesc(typeFSRef,(Ptr)(&defaultFile),sizeof(FSRef),&desc);
			err = ::NavCustomControl(params->context,kNavCtlSetLocation,&desc);
			//if( err != noErr )   _ACErrorNum("NavCustomControl() returned error: ",err,this);
			err = ::AEDisposeDesc(&desc);
			break;
		}
	}
}
*/
/**
�t�@�C���I���V�[�g�R�[���o�b�N�iOS�R�[���o�b�N�pstatic�j
*/
/*#1034
pascal void	CWindowImp::STATIC_NavChooseFileCB( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD )
{
	//�C�x���g�g�����U�N�V�����O�����^�㏈�� #0 �����ɂ��ꂪ�Ȃ��ƁARefresh���o�R��OS����C�x���gCB���������Ƃ��ɁA������sEventDepth==1�ƂȂ��Ă��܂��A
	//Window���̃I�u�W�F�N�g�ɑ΂���DoObjectArrayItemTrashDelete()���������A�����o�[�ϐ��ւ̃������A�N�Z�X���������A�����Ă��܂��B
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		CWindowImp*	window = (CWindowImp*)callbackUD;
		if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return;
		window->DoNavChooseFileCB(selector,params);
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_NavChooseFileCB() caught exception.",NULL);//#199
	}
}
*/

/**
�A�v���P�[�V�����t�@�C���݂̂�I���\�ɂ��邽�߂̃t�B���^�R�[���o�b�N
*/
/*#1034
Boolean	CWindowImp::STATIC_NavFilterCB_ApplicationFile( AEDesc * inDesc, void * inInfo, void * inCallBackUD, NavFilterModes inFilterMode )
{
	//�C�x���g�g�����U�N�V�����O�����^�㏈�� #0 �����ɂ��ꂪ�Ȃ��ƁARefresh���o�R��OS����C�x���gCB���������Ƃ��ɁA������sEventDepth==1�ƂȂ��Ă��܂��A
	//Window���̃I�u�W�F�N�g�ɑ΂���DoObjectArrayItemTrashDelete()���������A�����o�[�ϐ��ւ̃������A�N�Z�X���������A�����Ă��܂��B
	AStEventTransactionPrePostProcess	oneevent(false);
	
	OSStatus	err = noErr;
	
	try
	{
		err = ::AECoerceDesc(inDesc,typeFSRef,inDesc);
		FSRef	fsref;
		if( ::AEGetDescData(inDesc,&fsref,sizeof(FSRef)) != noErr )   return true;
		
		NavFileOrFolderInfo *info = (NavFileOrFolderInfo*)inInfo;
		if( info->isFolder == true )
		{
			CFURLRef	urlref = ::CFURLCreateFromFSRef(kCFAllocatorDefault,&fsref);
			if( urlref == NULL )   return true;
			CFBundleRef	bundleref = ::CFBundleCreate(kCFAllocatorDefault,urlref);
			::CFRelease(urlref);
			if( bundleref == NULL )   return true;
			UInt32	type = NULL, creator = NULL;
			::CFBundleGetPackageInfo(bundleref,&type,&creator);
			::CFRelease(bundleref);
			if( type == 'APPL' || type == 'BNDL' )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			FSCatalogInfo	catinfo;
			err = ::FSGetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo,NULL,NULL,NULL);
			if( err != noErr )   _ACErrorNum("FSGetCatalogInfo() returned error: ",err,NULL);
			OSType	type = ((FileInfo*)(&(catinfo.finderInfo[0])))->fileType;
			if( type == 'APPL' )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_NavFilterCB_ApplicationFile() caught exception.",NULL);//#199
	}
	return false;
}
*/

/**
�w��Creator/Type�t�@�C���݂̂�I���\�ɂ��邽�߂̃t�B���^�R�[���o�b�N
*/
/*#1034
Boolean	CWindowImp::STATIC_NavFilterCB_CreatorTypeFilter( AEDesc * inDesc, void * inInfo, void * inCallBackUD, NavFilterModes inFilterMode )
{
	//�C�x���g�g�����U�N�V�����O�����^�㏈�� #0 �����ɂ��ꂪ�Ȃ��ƁARefresh���o�R��OS����C�x���gCB���������Ƃ��ɁA������sEventDepth==1�ƂȂ��Ă��܂��A
	//Window���̃I�u�W�F�N�g�ɑ΂���DoObjectArrayItemTrashDelete()���������A�����o�[�ϐ��ւ̃������A�N�Z�X���������A�����Ă��܂��B
	AStEventTransactionPrePostProcess	oneevent(false);
	
	OSStatus	err = noErr;
	
	try
	{
		CWindowImp*	window = (CWindowImp*)inCallBackUD;
		if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return false;
		
		err = ::AECoerceDesc(inDesc,typeFSRef,inDesc);
		FSRef	fsref;
		if( ::AEGetDescData(inDesc,&fsref,sizeof(FSRef)) != noErr )   return true;
		
		NavFileOrFolderInfo *info = (NavFileOrFolderInfo*)inInfo;
		if( info->isFolder == true )
		{
			return true;
		}
		else
		{
			FSCatalogInfo	catinfo;
			err = ::FSGetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo,NULL,NULL,NULL);
			if( err != noErr )   _ACErrorNum("FSGetCatalogInfo() returned error: ",err,NULL);
			OSType	type = ((FileInfo*)(&(catinfo.finderInfo[0])))->fileType;
			OSType	creator = ((FileInfo*)(&(catinfo.finderInfo[0])))->fileCreator;
			if( type == window->mNavFileFilter_Type && creator == window->mNavFileFilter_Creator )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_NavFilterCB_CreatorTypeFilter() caught exception.",NULL);//#199
	}
	return false;
}
*/

//#1034
#if 0
/**
AskSaveChanges�_�C�A���O�\��
*/
void	CWindowImp::ShowAskSaveChangesWindow( const AText& inFileName, const AObjectID inDocumentID, 
											 const ABool inModal )
{
	OSStatus	err = noErr;
	
	if( mNavSheet != NULL )   {_ACError("Nav already displayed",this);return;}
	NavDialogCreationOptions	options;
	err = ::NavGetDefaultDialogCreationOptions(&options);
	if( err != noErr )   _ACErrorNum("NavGetDefaultDialogCreationOptions() returned error: ",err,this);
	options.modality = kWindowModalityWindowModal;
	if( inModal == true )
	{
		options.modality = kWindowModalityAppModal;
	}
	options.parentWindow = mWindowRef;
	options.optionFlags |= kNavSupportPackages| kNavAllowOpenPackages;//B0100
	options.saveFileName = inFileName.CreateCFString();//AUtil::CreateCFStringFromText(inFileName);
	AText	text;
	text.SetLocalizedText("AppName");
	options.clientName = text.CreateCFString();//AUtil::CreateCFStringFromText(text);
	err = ::NavCreateAskSaveChangesDialog(&options,kNavSaveChangesClosingDocument,::NewNavEventUPP(STATIC_NavAskSaveChangesCB),this,&mNavSheet);
	if( err != noErr )   _ACErrorNum("NavCreateAskSaveChangesDialog() returned error: ",err,this);
	err = ::NavDialogRun(mNavSheet);
	if( err != noErr )   _ACErrorNum("NavDialogRun() returned error: ",err,this);
	mAskSaveChangesDocumentID = inDocumentID;
}

/**
AskSaveChanges�_�C�A���O�R�[���o�b�N�iOS�R�[���o�b�N�pstatic�j
*/
pascal void	CWindowImp::STATIC_NavAskSaveChangesCB( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD )
{
	//�C�x���g�g�����U�N�V�����O�����^�㏈�� #0 �����ɂ��ꂪ�Ȃ��ƁARefresh���o�R��OS����C�x���gCB���������Ƃ��ɁA������sEventDepth==1�ƂȂ��Ă��܂��A
	//Window���̃I�u�W�F�N�g�ɑ΂���DoObjectArrayItemTrashDelete()���������A�����o�[�ϐ��ւ̃������A�N�Z�X���������A�����Ă��܂��B
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		CWindowImp*	window = (CWindowImp*)callbackUD;
		if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return;
		window->DoAskSaveChangesCB(selector,params);
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_NavAskSaveChangesCB() caught exception.",NULL);//#199
	}
}

/**
AskSaveChanges�_�C�A���O�R�[���o�b�N
*/
void	CWindowImp::DoAskSaveChangesCB( const NavEventCallbackMessage selector, const NavCBRecPtr params )
{
	switch( selector )
	{
	  case kNavCBUserAction:
		{
			NavUserAction	userAction = kNavUserActionNone;
			if( mNavSheet != NULL )
			{
				userAction = ::NavDialogGetUserAction(mNavSheet);
				::NavDialogDispose(mNavSheet);//�����p�ς݂Ȃ̂�kNavCBTerminate��҂����ɎE��
				mNavSheet = NULL;
			}
			AAskSaveChangesResult	askSaveChanges = kAskSaveChangesResult_Cancel;
			switch(userAction)
			{
			  case kNavUserActionSaveChanges:
				{
					askSaveChanges = kAskSaveChangesResult_Save;
					break;
				}
			  case kNavUserActionDontSaveChanges:
				{
					askSaveChanges = kAskSaveChangesResult_DontSave;
					break;
				}
			  case kNavUserActionNone:
			  case kNavUserActionCancel:
			  default:
				break;
			}
			GetAWin().EVT_AskSaveChangesReply(mAskSaveChangesDocumentID,askSaveChanges);
			AApplication::GetApplication().NVI_UpdateMenu();
			break;
		}
	}
}
#endif

//AskSaveChanges�����N���[�Y B0000
/*OSX10.4�ŃN���b�V������̂ł�߂�BNavDialogDispose()��NG�H
void	CWindowImp::CloseAskSaveChanges()
{
	if( mNavSheet != NULL )
	{
		::NavDialogDispose(mNavSheet);
		mNavSheet = NULL;
		AApplication::GetApplication().NVI_UpdateMenu();
	}
}*/

/**
�ۑ��_�C�A���O�\��
*/
void	CWindowImp::ShowSaveWindow( const AText& inFileName, const ADocumentID inDocumentID, 
		const AFileAttribute inFileAttr, const AText& inFilter, const AText& inRefText, const AFileAcc& inDefaultFolder )
{
	//��inFilter������
	
	//#1034
	//�Ώۃh�L�������g�ݒ�
	mSaveWindowDocumentID = inDocumentID;
	//�ۑ��^�C�v�Q�ƃe�L�X�g�ݒ�
	mSaveWindowRefText.SetText(inRefText);
	//�ۑ��t�@�C�������ݒ�
	mSaveWindowFileAttribute = inFileAttr;
	//�f�t�H���g�t�@�C�����ݒ�
	NSString*	filename = nil;
	if( inFileName.GetItemCount() > 0 )
	{
		AStCreateNSStringFromAText	filenamestr(inFileName);
		filename = [[[NSString alloc] initWithString:filenamestr.GetNSString()] autorelease];
	}
	//�f�t�H���g�t�H���_�p�X�ݒ�
	AText	dirpathtext;
	inDefaultFolder.GetPath(dirpathtext);
	NSString*	dirpath = nil;
	if( dirpathtext.GetItemCount() > 0 )
	{
		AStCreateNSStringFromAText	dirpathstr(dirpathtext);
		dirpath = [[[NSString alloc] initWithString:dirpathstr.GetNSString()] autorelease];
	}
	//�V�[�g�t���O�ݒ�
	mHasSheet = true;
	//�t�@�C���ۑ��V�[�g�\��
	[CocoaObjects->GetWindow() showSaveWindow:dirpath filename:filename];
	/*#1034
	OSStatus	err = noErr;
	
	if( mNavSheet != NULL )   {_ACError("Nav already displayed",this);return;}
	NavDialogCreationOptions	options;
	err = ::NavGetDefaultDialogCreationOptions(&options);
	if( err != noErr )   _ACErrorNum("NavGetDefaultDialogCreationOptions() returned error: ",err,this);
	options.modality = kWindowModalityWindowModal;
	options.parentWindow = mWindowRef;
	options.optionFlags |= *R0095kNavAllowInvisibleFiles|* kNavSupportPackages| kNavAllowOpenPackages;//B0100
	options.saveFileName = inFileName.CreateCFString();//AUtil::CreateCFStringFromText(inFileName);
	AText	text;
	text.SetLocalizedText("AppName");
	options.clientName = text.CreateCFString();//AUtil::CreateCFStringFromText(text);
	err = ::NavCreatePutFileDialog(&options,inFileAttr.type,inFileAttr.creator,::NewNavEventUPP(STATIC_NavPutDialogCB),this,&mNavSheet);
	if( err != noErr )   _ACErrorNum("NavCreatePutFileDialog() returned error: ",err,this);
	mSaveWindowDocumentID = inDocumentID;
	mSaveWindowRefText.SetText(inRefText);
	mSaveWindowDefaultFolder.CopyFrom(inDefaultFolder);
	mSaveWindowFileAttribute = inFileAttr;
	err = ::NavDialogRun(mNavSheet);
	if( err != noErr )   _ACErrorNum("NavDialogRun() returned error: ",err,this);
	*/
}

/**
�ۑ��_�C�A���O�R�[���o�b�N�iOS�R�[���o�b�N�pstatic�j
�i�L�����Z�����͈����󕶎��ŃR�[�������j
*/
void	CWindowImp::APICB_ShowSaveWindowAction( const AText& inPath )
{
	//#1034
	//�V�[�g�t���O����
	mHasSheet = false;
	//�ۑ��{�^���N���b�N����
	if( inPath.GetItemCount() > 0 )
	{
		AFileAcc	file;
		file.Specify(inPath);
		if( file.Exist() == true )
		{
			//�t�@�C���u�������̏ꍇ�́A���t�@�C����u�������邱�Ƃ��ł��邩�����B
			file.DeleteFile();
			if( file.Exist() == true )
			{
				//�s�v�����H
				AText	mes1, mes2;
				mes1.SetLocalizedText("SaveFailed_ReplaceError1");
				mes2.SetLocalizedText("SaveFailed_ReplaceError2");
				AApplication::GetApplication().NVI_ShowModalAlertWindow(mes1,mes2);//#291
				return;
			}
		}
		//�t�@�C������
		file.CreateFile();
		file.SetFileAttribute(mSaveWindowFileAttribute);
		//�t�@�C���ۑ��������s
		GetAWin().EVT_SaveWindowReply(mSaveWindowDocumentID,file,mSaveWindowRefText);
	}
}
/*#1034
pascal void	CWindowImp::STATIC_NavPutDialogCB( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD )
{
	//�C�x���g�g�����U�N�V�����O�����^�㏈�� #0 �����ɂ��ꂪ�Ȃ��ƁARefresh���o�R��OS����C�x���gCB���������Ƃ��ɁA������sEventDepth==1�ƂȂ��Ă��܂��A
	//Window���̃I�u�W�F�N�g�ɑ΂���DoObjectArrayItemTrashDelete()���������A�����o�[�ϐ��ւ̃������A�N�Z�X���������A�����Ă��܂��B
	AStEventTransactionPrePostProcess	oneevent(false);
	
	try
	{
		CWindowImp*	window = (CWindowImp*)callbackUD;
		if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return;
		window->DoNavPutDialogCB(selector,params);
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_NavPutDialogCB() caught exception.",NULL);//#199
	}
}
*/

/**
�ۑ��_�C�A���O�R�[���o�b�N
*/
/*#1034
void	CWindowImp::DoNavPutDialogCB( const NavEventCallbackMessage selector, const NavCBRecPtr params )
{
	OSStatus	err = noErr;
	
	switch( selector )
	{
	  case kNavCBUserAction:
		{
			NavUserAction	userAction = ::NavDialogGetUserAction(mNavSheet);
			switch(userAction)
			{
			  case kNavUserActionSaveAs:
				{
					NavReplyRecord	reply;
					if( ::NavDialogGetReply(mNavSheet,&reply) == noErr )
					{
						if( reply.validRecord )
						{
							FSRef	fsref;
							AEKeyword	key;
							DescType	type;
							Size	size;
							err = ::AEGetNthPtr(&(reply.selection),1,typeFSRef,&key,&type,(Ptr)&fsref,sizeof(FSRef),&size);
							AFileAcc	folder;
							folder.SpecifyByFSRef(fsref);
							CFStringRef	nameRef = ::NavDialogGetSaveFileName(mNavSheet);
							if( nameRef != NULL )
							{
								AText	filename;
								filename.SetFromCFString(nameRef);
								AFileAcc	file;
								file.SpecifyChildFile(folder,filename);
								if( reply.replacing == true )
								{
									//�t�@�C���u�������̏ꍇ�́A���t�@�C����u�������邱�Ƃ��ł��邩�����B
									file.DeleteFile();
									if( file.Exist() == true )
									{
										//�s�v�����H
										AText	mes1, mes2;
										mes1.SetLocalizedText("SaveFailed_ReplaceError1");
										mes2.SetLocalizedText("SaveFailed_ReplaceError2");
										AApplication::GetApplication().NVI_ShowModalAlertWindow(mes1,mes2);//#291
										//#291 ShowChildWindow_OK(mes1,mes2);
										err = ::NavDisposeReply(&reply);
										if( err != noErr )   _ACErrorNum("NavDisposeReply() returned error: ",err,this);
										break;
									}
								}
								file.CreateFile();
								//win AFileWrapper::SetFileAttribute(file,mSaveWindowFileAttribute);
								file.SetFileAttribute(mSaveWindowFileAttribute);
								GetAWin().EVT_SaveWindowReply(mSaveWindowDocumentID,file,mSaveWindowRefText);
								err = ::NavDisposeReply(&reply);
								if( err != noErr )   _ACErrorNum("NavDisposeReply() returned error: ",err,this);
							}
						}
					}
					break;
				}
			  case kNavUserActionCancel:
				{
					//closingSaveMode��false�ɂ���H
					break;
				}
			  default:
				{
					break;
				}
			}
			break;
		}
	  case kNavCBTerminate:
		{
			::NavDialogDispose(mNavSheet);
			mNavSheet = NULL;
			AApplication::GetApplication().NVI_UpdateMenu();
			break;
		}
		//R0148
	  case kNavCBStart:
		{
			if( mSaveWindowDefaultFolder.IsSpecified() == true )
			{
				AEDesc	folderDesc;
				err = ::AECreateDesc(typeNull,NULL,0,&folderDesc);
				FSRef	folderRef;
				mSaveWindowDefaultFolder.GetFSRef(folderRef);
				err = ::AECreateDesc(typeFSRef,(Ptr)(&folderRef),sizeof(FSRef),&folderDesc);
				err = ::NavCustomControl(params->context,kNavCtlSetLocation,&folderDesc);
				//if( err != noErr )   _ACErrorNum("NavCustomControl() returned error: ",err,this);
				err = ::AEDisposeDesc(&folderDesc);
			}
			break;
		}
	}
}
*/
#pragma mark ===========================

#pragma mark ---�q�E�C���h�E�Ǘ�

/*#291
//B0244
**
�qSheet�E�C���h�E�\���iOK�{�^���̂݁j
*
void	CWindowImp::ShowChildWindow_OK( const AText& inMessage1, const AText& inMessage2 )
{
	//���ɊJ���Ă���q�V�[�g�E�C���h�E���������烊�^�[��
	if( mChildSheetWindow.GetItemCount() > 0 )
	{
		_AError("sheet window double open",this);
		return;
	}
	//�qSheet�𐶐��A�ݒ�A�\��
	CChildSheetWindowImpFactory	factory(this,*this,GetAWin(),kControlID_Invalid);
	AIndex	index = mChildSheetWindow.AddNewObject(factory);
	mChildSheetWindow.GetObject(index).Create("main/SheetAlert");
	mChildSheetWindow.GetObject(index).SetWindowStyleToSheet(*this);
	mChildSheetWindow.GetObject(index).SetText(10003,inMessage1);
	mChildSheetWindow.GetObject(index).SetText(10004,inMessage2);
	mChildSheetWindow.GetObject(index).SetDefaultOKButton(10001);
	mChildSheetWindow.GetObject(index).SetDefaultCancelButton(10001);
	mChildSheetWindow.GetObject(index).Show();
}

**
�qSheet�E�C���h�E�\���iOK�{�^��+Cancel�{�^���j
*
void	CWindowImp::ShowChildWindow_OKCancel( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
		const AControlID inOKButtonVirtualControlID )
{
	//���ɊJ���Ă���q�V�[�g�E�C���h�E���������烊�^�[��
	if( mChildSheetWindow.GetItemCount() > 0 )
	{
		_AError("sheet window double open",this);
		return;
	}
	//�qSheet�𐶐��A�ݒ�A�\��
	CChildSheetWindowImpFactory	factory(this,*this,GetAWin(),inOKButtonVirtualControlID);
	AIndex	index = mChildSheetWindow.AddNewObject(factory);
	mChildSheetWindow.GetObject(index).Create("main/SheetOKCancel");
	mChildSheetWindow.GetObject(index).SetWindowStyleToSheet(*this);
	mChildSheetWindow.GetObject(index).SetText(10003,inMessage1);
	mChildSheetWindow.GetObject(index).SetText(10004,inMessage2);
	if( inOKButtonMessage.GetItemCount() > 0 )
	{
		mChildSheetWindow.GetObject(index).SetText(10001,inOKButtonMessage);
	}
	mChildSheetWindow.GetObject(index).SetDefaultOKButton(10002);
	mChildSheetWindow.GetObject(index).SetDefaultCancelButton(10002);
	mChildSheetWindow.GetObject(index).Show();
}

//#8
**
�qSheet�E�C���h�E�\���iOK�{�^��+Cancel�{�^��+Discard�{�^���j
*
void	CWindowImp::ShowChildWindow_OKCancelDiscard( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
		const AControlID inOKButtonVirtualControlID, const AControlID inDiscardButtonVirtualControlID )
{
	//���ɊJ���Ă���q�V�[�g�E�C���h�E���������烊�^�[��
	if( mChildSheetWindow.GetItemCount() > 0 )
	{
		_AError("sheet window double open",this);
		return;
	}
	//�qSheet�𐶐��A�ݒ�A�\��
	CChildSheetWindowImpFactory	factory(this,*this,GetAWin(),inOKButtonVirtualControlID,inDiscardButtonVirtualControlID);
	AIndex	index = mChildSheetWindow.AddNewObject(factory);
	mChildSheetWindow.GetObject(index).Create("main/SheetOKCancelDiscard");
	mChildSheetWindow.GetObject(index).SetWindowStyleToSheet(*this);
	mChildSheetWindow.GetObject(index).SetText(10003,inMessage1);
	mChildSheetWindow.GetObject(index).SetText(10004,inMessage2);
	if( inOKButtonMessage.GetItemCount() > 0 )
	{
		mChildSheetWindow.GetObject(index).SetText(10001,inOKButtonMessage);
	}
	mChildSheetWindow.GetObject(index).SetDefaultOKButton(10002);
	mChildSheetWindow.GetObject(index).SetDefaultCancelButton(10002);
	mChildSheetWindow.GetObject(index).Show();
}

//B0244 B0285 CloseAlert->HideAlert�i�I�u�W�F�N�g��delete�̓R�[�����Ŏ��{����BCallback���s�r����delete��h�~���邽�߁j
**
�qSheet�E�C���h�E�N���[�Y 
*
void	CWindowImp::CloseChildWindow()
{
	//�q�V�[�g�E�C���h�E��������΃��^�[��
	if( mChildSheetWindow.GetItemCount() == 0 )
	{
		_AError("no sheet window",this);
		return;
	}
	//�폜
	mChildSheetWindow.GetObject(0).Hide();
	mChildSheetWindow.GetObject(0).CloseWindow();
	mChildSheetWindow.DeleteAll();
}
*/

/**
�q�E�C���h�E�\�������ǂ������擾
*/
ABool	CWindowImp::ChildWindowVisible() const
{
	//#1034 if( mNavSheet != NULL /*#291|| mChildSheetWindow.GetItemCount() > 0*/ )   return true;
	if( mHasSheet == true )   return true;//#1034
	else return false;
}

#pragma mark ===========================

#pragma mark ---�w�i

//#688
AArray<NSImage*>	gBackgroundImageArray;
AObjectArray<AFileAcc>	gBackgroundImageArray_File;

/**
�w�i��Offscreen�ɕۑ�
*/
AIndex	CWindowImp::RegisterBackground( const AFileAcc& inBackgroundImageFile )
{
	//�w�i�摜�t�@�C�������݂��Ȃ��Ƃ��̓��^�[������ #1273
	if( inBackgroundImageFile.Exist() == false )
	{
		return kIndex_Invalid;
	}
	
	//
	for( AIndex i = 0; i < gBackgroundImageArray_File.GetItemCount(); i++ )
	{
		if( gBackgroundImageArray_File.GetObject(i).Compare(inBackgroundImageFile) == true )
		{
			return i;
		}
	}
	
	//�t�@�C���p�X�擾
	AText	filepath;
	inBackgroundImageFile.GetPath(filepath);
	AStCreateNSStringFromAText	filepathstr(filepath);
	//NSImage����
	NSImage*	image = [[NSImage alloc] initByReferencingFile:filepathstr.GetNSString()];//�|�C���^��ێ�����̂�retainCount=1�̂܂�(init�ɂ��)�ɂ���
	//Array�Ɋi�[
	gBackgroundImageArray.Add(image);
	AIndex index = gBackgroundImageArray_File.AddNewObject();
	gBackgroundImageArray_File.GetObject(index).CopyFrom(inBackgroundImageFile);
	return index;
	
	/*#688
	FSSpec	file;
	if( inBackgroundImageFile.GetFSSpec(file) == false )   return kIndex_Invalid;
	Handle	picHandle = NULL;
	GWorldPtr	gw = NULL;
	try
	{
	ComponentInstance	ci;
	if( ::GetGraphicsImporterForFile(&file,&ci) != noErr )   throw 0;
	if( ::GraphicsImportGetAsPicture(ci,(PicHandle*)&picHandle) != noErr )   throw 0;
	::CloseComponent(ci);
	
	CGrafPtr	svPort;
	GDHandle	svDevice;
	::GetGWorld(&svPort,&svDevice);
	Rect	rect;
	rect = (*(PicHandle)picHandle)->picFrame;
	rect.left 		= ::CFSwapInt16BigToHost(rect.left);
	rect.top 		= ::CFSwapInt16BigToHost(rect.top);
	rect.right 		= ::CFSwapInt16BigToHost(rect.right);
	rect.bottom		= ::CFSwapInt16BigToHost(rect.bottom);
	if( ::NewGWorld(&gw,0,&rect,NULL,NULL,(GWorldFlags)0) != noErr )   throw 0;
	::SetGWorld(gw,NULL);
	::LockPixels(::GetGWorldPixMap(gw));
	::EraseRect(&rect);
	::DrawPicture((PicHandle)picHandle,&rect);
	::UnlockPixels(::GetGWorldPixMap(gw));
	::SetGWorld(svPort,svDevice);
	::DisposeHandle(picHandle);
	
	sBackgroundImageArray.Add(gw);
	AIndex index = sBackgroundImageArray_File.AddNewObject();
	sBackgroundImageArray_File.GetObject(index).CopyFrom(inBackgroundImageFile);
	sBackgroundImageArray_Bound.Add(rect);
	return index;
	}
	catch(...)
	{
	if( picHandle != NULL )   ::DisposeHandle(picHandle);
	if( gw != NULL )   ::DisposeGWorld(gw);
	}
	return kIndex_Invalid;
	*/
}

/**
inDrawRect�Ŏw�肷��̈��w�i�摜�ŕ`�悷��
*/
void	CWindowImp::DrawBackgroundImage( const AControlID inControlID, const ALocalRect& inEraseRect, const AImageRect& inImageFrameRect, 
		const ANumber inLeftOffset, const AIndex inBackgroundImageIndex, const AFloatNumber inAlpha )
{
	//NSImage�擾
	NSImage*	image = gBackgroundImageArray.Get(inBackgroundImageIndex);
	ANumber	imagewidth = [image size].width;
	ANumber	imageheight = [image size].height;
	//width or height��0�Ȃ牽�������I���i0���Z�`�F�b�N�j
	if( imagewidth == 0 || imageheight == 0 )   return;
	
	//inImageFrameRect���܂ށA���߂́Aimage�̊J�nx,y���擾�i�^�C���̋�؂�ʒu�j
	ANumber	startx = inImageFrameRect.left/imagewidth;
	startx *= imagewidth;
	ANumber	starty = inImageFrameRect.top/imageheight;
	starty *= imageheight;
	for( ANumber x = startx; x < inImageFrameRect.right + inLeftOffset; x += imagewidth )
	{
		for( ANumber y = starty; y < inImageFrameRect.bottom; y += imageheight )
		{
			//Image��\��ʒu��Image���W�n�Ŏ擾
			AImageRect	pictimage = {0};
			pictimage.left 		= x - inLeftOffset;
			pictimage.top		= y;
			pictimage.right		= x - inLeftOffset + imagewidth;
			pictimage.bottom	= y + imageheight;
			//�������r���[
			//
			ALocalRect	pictrect = {0};
			pictrect.left		= pictimage.left 	- inImageFrameRect.left;
			pictrect.top		= pictimage.top		- inImageFrameRect.top;
			pictrect.right		= pictimage.right	- inImageFrameRect.left;
			pictrect.bottom		= pictimage.bottom	- inImageFrameRect.top;
			//
			ALocalRect	sectRect = {0};
			if( GetIntersectRect(pictrect,inEraseRect,sectRect) == true )
			{
				//
				NSRect	drawrect = {0};
				drawrect.origin.x = sectRect.left;
				drawrect.origin.y = -sectRect.bottom;
				drawrect.size.width = sectRect.right-sectRect.left;
				drawrect.size.height = sectRect.bottom-sectRect.top;
				//
				NSRect	srcrect = {0};
				srcrect.origin.x = sectRect.left - pictrect.left;
				srcrect.origin.y = -(sectRect.bottom - pictrect.bottom);
				srcrect.size.width = sectRect.right-sectRect.left;
				srcrect.size.height = sectRect.bottom-sectRect.top;
				//
				[image drawInRect:drawrect fromRect:srcrect operation:NSCompositeCopy fraction:1.0];
			}
		}
	}
}

/**
�w�iOffscreen���擾
*/
/*#688
void	CWindowImp::GetBackgroundData( const AIndex inBackgroundImageIndex, GWorldPtr& outGWorldPtr, Rect& outBoundRect ) const
{
	outGWorldPtr = sBackgroundImageArray.Get(inBackgroundImageIndex);
	outBoundRect = sBackgroundImageArray_Bound.Get(inBackgroundImageIndex);
}
*/

//�w�iOffscreen�f�[�^
/*#688
AArray<GWorldPtr>	CWindowImp::sBackgroundImageArray;
AObjectArray<AFileAcc>	CWindowImp::sBackgroundImageArray_File;
AArray<Rect>	CWindowImp::sBackgroundImageArray_Bound;
*/

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---�C���[�W

/**
�C���[�W�o�^
*/
AIndex	CWindowImp::RegisterImage( const AFileAcc& inImageFile )
{
	FSSpec	file;
	if( inImageFile.GetFSSpec(file) == false )   return kIndex_Invalid;
	PicHandle	picHandle = NULL;
	try
	{
		ComponentInstance	ci;
		if( ::GetGraphicsImporterForFile(&file,&ci) != noErr )   throw 0;
		if( ::GraphicsImportGetAsPicture(ci,&picHandle) != noErr )   throw 0;
		::CloseComponent(ci);
		/*�T�C�Y�k���e�X�g
		PicHandle	picHandle2 = NULL;
		Rect	rect;
		rect.left		= 0;
		rect.top		= 0;
		rect.right		= 20;
		rect.bottom		= 20;
		picHandle2 = ::OpenPicture(&rect);
		if( picHandle2 == NULL )   throw 0;
		::DrawPicture((PicHandle)picHandle,&rect);
		::ClosePicture();
		::DisposeHandle((Handle)picHandle);
		*/
		sImagePicHandle.Add(picHandle);
		return sImagePicHandle.GetItemCount()-1;
	}
	catch(...)
	{
		if( picHandle != NULL )   ::DisposeHandle((Handle)picHandle);
	}
	return kIndex_Invalid;
}

/**
�o�^�C���[�W�擾
*/
PicHandle	CWindowImp::GetImage( const AIndex inImageIndex )
{
	return sImagePicHandle.Get(inImageIndex);
}

//�o�^�C���[�W�f�[�^
AArray<PicHandle>	CWindowImp::sImagePicHandle;
#endif

#pragma mark ===========================

#pragma mark --- �A�C�R��

/**
�A�C�R���o�^(static)
*/
void	CWindowImp::RegisterIcon( const AIconID inIconID, const AFileAcc& inIconFile, const AConstCstringPtr inName )//#232
{
	OSStatus	err = noErr;
	
	FSRef	fsref;
	inIconFile.GetFSRef(fsref);
	AIconRef	iconref;
	err = ::RegisterIconRefFromFSRef('MMKE','icns',&fsref,&iconref);
	if( err != noErr )   _ACErrorNum("RegisterIconRefFromFSRef() returned error: ",err,NULL);
	sIconIDArray.Add(inIconID);
	sIconRefArray.Add(iconref);
	//#1034 sCIconHandleArray.Add(NULL);
	sIconFileArray.GetObject(sIconFileArray.AddNewObject()).CopyFrom(inIconFile);//#232
	AText	name;//#232
	name.SetCstring(inName);//#232
	sIconNameArray.Add(name);//#232
}

//#1034
#if 0
/**
�A�C�R���o�^�i���\�[�X(CIcon)����o�^�EAIconID�͎������蓖�āj
*/
AIconID	CWindowImp::RegisterIconFromResouce( const AIndex inResID )
{
	CIconHandle	h = ::GetCIcon(inResID);
	if( h == NULL )   return kIndex_Invalid;
	AIconID iconID = AssignDynamicIconID();
	sIconIDArray.Add(iconID);
	sIconRefArray.Add(NULL);
	sCIconHandleArray.Add(h);
	sIconFileArray.AddNewObject();//#232
	AText	name;//#232
	sIconNameArray.Add(name);//#232
	return iconID;
}

/**
�A�C�R���o�^�i���\�[�X(CIcon)����o�^�EAIconID�͎w��j
*/
void	CWindowImp::RegisterIconFromResouce( const AIconID inIconID, const AIndex inResID )
{
	CIconHandle	h = ::GetCIcon(inResID);
	if( h == NULL )   { _ACError("",NULL); return; }
	sIconIDArray.Add(inIconID);
	sIconRefArray.Add(NULL);
	sCIconHandleArray.Add(h);
	sIconFileArray.AddNewObject();//#232
	AText	name;//#232
	sIconNameArray.Add(name);//#232
}
#endif
//#1012 
#if 0
//R0217
//�f�o�b�O��
AIconID	CWindowImp::RegisterDynamicIconFromImageFile( const AFileAcc& inFile )
{
	//http://lists.apple.com/archives/carbon-development/2003/Aug/msg01181.html
	FSSpec	file;
	if( inFile.GetFSSpec(file) == false )   return kIconID_NoIcon;
	
	AIconID	iconID = kIconID_NoIcon;
	Handle	picHandle = NULL;
	PicHandle	picHandle2 = NULL;
	try
	{
		ComponentInstance	ci;
		if( ::GetGraphicsImporterForFile(&file,&ci) != noErr )   throw 0;
		if( ::GraphicsImportGetAsPicture(ci,(PicHandle*)&picHandle) != noErr )   throw 0;
		::CloseComponent(ci);
		
		Rect	rect;
		rect.left		= 0;
		rect.top		= 0;
		rect.right		= 256;
		rect.bottom		= 256;
		picHandle2 = ::OpenPicture(&rect);
		if( picHandle2 == NULL )   throw 0;
		::DrawPicture((PicHandle)picHandle,&rect);
		/*�����������݃e�X�g::MoveTo(0,100);
		::TextSize(96);
		RGBColor	color = {0,0,0};
		::RGBForeColor(&color);
		::DrawString("\ptest");*/
		::ClosePicture();
		
		IconFamilyHandle	iconFamily = (IconFamilyHandle)NewHandle(0);
		OSStatus err = ::SetIconFamilyData(iconFamily,'PICT',(Handle)picHandle2);
		
		IconRef	iconRef = NULL;
		err = ::RegisterIconRefFromIconFamily('MMKE','----',iconFamily, &iconRef);
		::DisposeHandle((Handle)iconFamily);
		iconID = AssignDynamicIconID();
		sIconIDArray.Add(iconID);
		sIconRefArray.Add(iconRef);
		sCIconHandleArray.Add(NULL);
		sIconFileArray.GetObject(sIconFileArray.AddNewObject()).CopyFrom(inFile);//#232
		AText	name;//#232
		sIconNameArray.Add(name);//#232
		
		::DisposeHandle(picHandle);
	}
	catch(...)
	{
		if( picHandle != NULL )   ::DisposeHandle(picHandle);
		if( picHandle2 != NULL )   ::DisposeHandle((Handle)picHandle2);
	}
	return iconID;
}
#endif

/**
�A�C�R���o�^(static)
*/
AIconID	CWindowImp::RegisterDynamicIconFromFile( const AFileAcc& inFile, const ABool inLoadEvenIfNotCustomIcon )//#232
{
	OSStatus	err = noErr;
	
	FSSpec	fsspec;
	inFile.GetFSSpec(fsspec);
	AIconID	iconID = AssignDynamicIconID();
	//
	ABool	loaded = false;
	//#232 .icns�t�@�C���̑��݃`�F�b�N
	if( loaded == false )
	{
		AText	filename;
		inFile.GetFilename(filename);
		filename.AddCstring(".icns");
		AFileAcc	icnsfile;
		icnsfile.SpecifySister(inFile,filename);
		if( icnsfile.Exist() == true )
		{
			RegisterIcon(iconID,icnsfile);
			loaded = true;
		}
	}
	/*#1034
	//
	if( loaded == false )
	{
		//B0307 �J�X�^���A�C�R�����݃`�F�b�N�ǉ�
		FSSpec	fsspec;
		inFile.GetFSSpec(fsspec);
		FInfo	fi;
		err = ::FSpGetFInfo(&fsspec,&fi);
		//if( err != noErr )   _ACErrorNum("FSpGetFInfo() returned error: ",err,NULL);
		ABool	customIconExists = ((fi.fdFlags&kHasCustomIcon)!=0);
		if( customIconExists == true )
		{
			IconRef	iconRef;
			SInt16	label;
			if( ::GetIconRefFromFile(&fsspec,&iconRef,&label) == noErr )
			{
				sIconIDArray.Add(iconID);
				sIconRefArray.Add(iconRef);
				//#1034 sCIconHandleArray.Add(NULL);
				sIconFileArray.GetObject(sIconFileArray.AddNewObject()).CopyFrom(inFile);//#232
				AText	name;//#232
				sIconNameArray.Add(name);//#232
				loaded = true;
			}
		}
	}
	//B0307 �D�揇�ʕύX�i�J�X�^���A�C�R������j
	if( loaded == false )//B0307
	{
		short	rRefNum = ::FSpOpenResFile(&fsspec,fsRdPerm);
		if( rRefNum != -1 )
		{
			CIconHandle	iconhandle = ::GetCIcon(128);
			if( iconhandle != NULL )
			{
				sIconIDArray.Add(iconID);
				sIconRefArray.Add(NULL);
				//#1034 sCIconHandleArray.Add(iconhandle);
				sIconFileArray.GetObject(sIconFileArray.AddNewObject()).CopyFrom(inFile);//#232
				AText	name;//#232
				sIconNameArray.Add(name);//#232
				loaded = true;
			}
			::CloseResFile(rRefNum);
		}
	}
	//B0307 ����ł��Ȃ���΃J�X�^���A�C�R���Ȃ��ł��t�@�C���E�t�H���_�̃A�C�R�����擾
	if( loaded == false && inLoadEvenIfNotCustomIcon == true )//#232
	{
		IconRef	iconRef;
		SInt16	label;
		if( ::GetIconRefFromFile(&fsspec,&iconRef,&label) == noErr )
		{
			sIconIDArray.Add(iconID);
			sIconRefArray.Add(iconRef);
			//#1034 sCIconHandleArray.Add(NULL);
			sIconFileArray.GetObject(sIconFileArray.AddNewObject()).CopyFrom(inFile);//#232
			AText	name;//#232
			sIconNameArray.Add(name);//#232
			loaded = true;
		}
	}
	*/
	if( loaded == false )
	{
		return kIconID_NoIcon;
	}
	return iconID;
}

/**
�A�C�R���o�^����
*/
void	CWindowImp::UnregisterDynamicIcon( const AIconID inIconID )
{
	AIndex	index = sIconIDArray.Find(inIconID);
	if( index == kIndex_Invalid )   return;
	if( sIconRefArray.Get(index) != NULL )
	{
		::ReleaseIconRef(sIconRefArray.Get(index));
	}
	/*#1034 
	if( sCIconHandleArray.Get(index) != NULL )
	{
		::DisposeCIcon(sCIconHandleArray.Get(index));
	}
	*/
	sIconIDArray.Delete1(index);
	sIconRefArray.Delete1(index);
	//#1034 sCIconHandleArray.Delete1(index);
	sIconFileArray.Delete1Object(index);//#232
	sIconNameArray.Delete1(index);//#232
}

//#232
/**
�A�C�R�����O����IconID�擾
*/
AIconID	CWindowImp::GetIconIDByName( const AText& inName )
{
	AIndex	index = sIconNameArray.Find(inName);
	if( index == kIndex_Invalid )   return kIconID_NoIcon;
	return sIconIDArray.Get(index);
}

//#232
/**
�A�C�R�����t�@�C���փR�s�[

����A�t�@�C�����烍�[�h�����ꍇ�̂ݑΉ��B
*/
void	CWindowImp::CopyIconToFile( const AIconID inIconID, AFileAcc& inDestIconFile )
{
	AIndex	index = sIconIDArray.Find(inIconID);
	if( index == kIndex_Invalid )   return;
	
	AFileAcc	srcfile;
	srcfile.CopyFrom(sIconFileArray.GetObject(index));
	if( srcfile.Exist() == true )
	{
		srcfile.CopyFileTo(inDestIconFile,true);
	}
}

//#232
/**
�A�C�R���t�@�C���g���q�擾
*/
void	CWindowImp::GetIconFileSuffix( AText& outSuffix )
{
	outSuffix.SetCstring(".icns");
}

/**
���I����IconID�����蓖�Ă�
*/
AIconID	CWindowImp::AssignDynamicIconID()
{
	while( sIconIDArray.Find(sNextDynamicIconID) != kIndex_Invalid )
	{
		sNextDynamicIconID++;
		if( sNextDynamicIconID >= kDynamicIconIDEnd )
		{
			sNextDynamicIconID = kDynamicIconIDStart;
		}
	}
	AIconID	id = sNextDynamicIconID;
	sNextDynamicIconID++;
	return id;
}

/**
�A�C�R���擾(static)
*/
IconRef	CWindowImp::GetIconRef( const AIconID inIconID )
{
	AIndex	index = sIconIDArray.Find(inIconID);
	if( index == kIndex_Invalid )
	{
		_ACError("icon not found",NULL);
		return sIconRefArray.Get(0);
	}
	return sIconRefArray.Get(index);
}
/**
�A�C�R���擾(static)
*/
/*#1034
CIconHandle	CWindowImp::GetCIconHandle( const AIconID inIconID )
{
	AIndex	index = sIconIDArray.Find(inIconID);
	if( index == kIndex_Invalid )
	{
		_ACError("icon not found",NULL);
		return sCIconHandleArray.Get(0);
	}
	return sCIconHandleArray.Get(index);
}
*/
/**
���I���蓖��IconID���ǂ������擾
*/
ABool	CWindowImp::IsDynamicIcon( const AIconID inIconID )
{
	return ((inIconID>=kDynamicIconIDStart)&&(inIconID<=kDynamicIconIDEnd));
}

//�f�[�^
AHashArray<AIconID>	CWindowImp::sIconIDArray;
AArray<IconRef>	CWindowImp::sIconRefArray;
//#1034 AArray<CIconHandle>	CWindowImp::sCIconHandleArray;
AObjectArray<AFileAcc>	CWindowImp::sIconFileArray;//#232
AHashTextArray	CWindowImp::sIconNameArray;//#232
AIconID	CWindowImp::sNextDynamicIconID = kDynamicIconIDStart;

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_MouseDownHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-MDH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//�}�E�X�N���b�N�ʒu�̃`�F�b�N�i�E�C���h�E�O�͏������Ȃ��j
	WindowPartCode	partcode = 0;
	err = ::GetEventParameter(inEventRef,'wpar'/*kEventParamWindowPartCode*/,'wpar'/*typeWindowPartCode*/,NULL,sizeof(partcode),NULL,&partcode);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,NULL);
	if( partcode != inContent )   return result;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoMouseDown(inCallRef,inEventRef) == true )
		{
			result = noErr;
			AApplication::GetApplication().NVI_UpdateMenu();
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_MouseDownHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-MDH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_MouseWheelHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-MWH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//�}�E�XWheel�ʒu�̃`�F�b�N�i�E�C���h�E�O�͏������Ȃ��j
	WindowPartCode	partcode = 0;
	err = ::GetEventParameter(inEventRef,'wpar'/*kEventParamWindowPartCode*/,'wpar'/*typeWindowPartCode*/,NULL,sizeof(partcode),NULL,&partcode);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,NULL);
	if( partcode != inContent )   return result;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoMouseWheel(inCallRef,inEventRef) == true )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_MouseWheelHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-MWH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_DoMouseMoved( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-MM)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//�}�E�XWheel�ʒu�̃`�F�b�N�i�E�C���h�E�O�͏������Ȃ��j
	WindowPartCode	partcode = 0;
	err = ::GetEventParameter(inEventRef,'wpar'/*kEventParamWindowPartCode*/,'wpar'/*typeWindowPartCode*/,NULL,sizeof(partcode),NULL,&partcode);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,NULL);
	if( partcode != inContent )   return result;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoMouseMoved(inCallRef,inEventRef) == true )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_DoMouseMoved() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-MM]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_CommandHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-CmH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	//�R�}���h�擾
	HICommand	command = {0,0};
	err = ::GetEventParameter(inEventRef,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&command);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,NULL);
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoCommand(inCallRef,inEventRef,command) )
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
		_ACError("CWindowImp::STATIC_APICB_CommandHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-CmH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_KeyDownHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-KDH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoKeyDown(inCallRef,inEventRef) )
		{
			result = noErr;
			AApplication::GetApplication().NVI_UpdateMenu();
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_KeyDownHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-KDH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_TextInputHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TIH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		ABool	updateMenu = true;
		if( window->APICB_DoTextInput(inCallRef,inEventRef,updateMenu) )
		{
			result = noErr;
			if( updateMenu == true )
			{
				AApplication::GetApplication().NVI_UpdateMenu();
			}
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_TextInputHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-TIH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_TextInputUpdateActiveInputAreaHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TIUAIAH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		ABool	updateMenu = true;
		if( window->APICB_DoTextInputUpdateActiveInputArea(inCallRef,inEventRef,updateMenu) )
		{
			result = noErr;
			if( updateMenu == true )
			{
				AApplication::GetApplication().NVI_UpdateMenu();
			}
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_TextInputUpdateActiveInputAreaHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-TIUAIAH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_TextInputOffsetToPosHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TIOTPH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoTextInputOffsetToPos(inCallRef,inEventRef) )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_TextInputOffsetToPosHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-TIOTPH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_TextInputPosToOffsetHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TIPTOH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoTextInputPosToOffset(inCallRef,inEventRef) )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_TextInputPosToOffsetHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-TIPTOH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_TextInputGetSelectedText( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TIGST)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoTextInputGetSelectedText(inCallRef,inEventRef) )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_TextInputGetSelectedText() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-TIGST]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�jB330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessLockDocument( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-Lock)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		result = noErr;
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_TSMDocumentAccessLockDocument() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-TSMDA-Lock]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�jB330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessUnlockDocument( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-Unlock)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		result = noErr;
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_TSMDocumentAccessUnlockDocument() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-TSMDA-Unlock]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�jB330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessGetLength( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-GetLength)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		result = noErr;
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_TSMDocumentAccessGetLength() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-TSMDA-GetLength]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�jB330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessGetSelectedRange( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-GetSelectedRange)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		result = noErr;
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_TSMDocumentAccessGetSelectedRange() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-TSMDA-GetSelectedRange]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�jB330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessGetCharacters( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-GetCharacters)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		result = noErr;
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_TSMDocumentAccessGetCharacters() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-TSMDA-GetCharacters]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�jB330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessGetFont( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-GetFont)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		result = noErr;
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_TSMDocumentAccessGetFont() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-TSMDA-GetFont]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�jB330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessGetGlyphInfo( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-GetGlyphInfo)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		result = noErr;
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_TSMDocumentAccessGetGlyphInfo() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-TSMDA-GetGlyphInfo]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowActivatedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-AH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoWindowActivated(inCallRef,inEventRef) )
		{
			result = noErr;
			AApplication::GetApplication().NVI_UpdateMenu();
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_WindowActivatedHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-AH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowDeactivatedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-DH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoWindowDeactivated(inCallRef,inEventRef) )
		{
			result = noErr;
			AApplication::GetApplication().NVI_UpdateMenu();
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_WindowDeactivatedHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-DH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowCollapsedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-CoH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoWindowCollapsed(inCallRef,inEventRef) )
		{
			result = noErr;
			AApplication::GetApplication().NVI_UpdateMenu();
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_WindowCollapsedHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-CoH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowBoundsChangedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-BCH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(true);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoWindowBoundsChanged(inCallRef,inEventRef) )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_WindowBoundsChangedHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-BCH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowResizeCompletedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-RCH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoWindowResizeCompleted(inCallRef,inEventRef) )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_WindowResizeCompletedHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-RCH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowZoomedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-WZH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoWindowResizeCompleted(inCallRef,inEventRef) )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_WindowZoomedHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-WZH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowCursorChangeHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-CCH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(true);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoWindowCursorChange(inCallRef,inEventRef) )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_WindowCursorChangeHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-CCH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowFocusAcquiredHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-FAH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoWindowFocusAcquiredHandler(inCallRef,inEventRef) )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_WindowFocusAcquiredHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-FAH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_ControlTrackHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-CTH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		window->APICB_DoControlTrack(inCallRef,inEventRef);
		AApplication::GetApplication().NVI_UpdateMenu();
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_ControlTrackHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-CTH]");
	return noErr;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowCloseHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-ClH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoWindowCloseButton(inCallRef,inEventRef) )
		{
			result = noErr;
			AApplication::GetApplication().NVI_UpdateMenu();
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_WindowCloseHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-ClH]");
	return result;
}

/*
//�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
pascal OSStatus CWindowImp::STATIC_APICB_ContextMenuHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	fprintf(stderr,"TEST ");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		//if( window->DoWindowCloseButton(inCallRef,inEventRef) )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_ContextMenuHandler() caught exception.",NULL);//#199
	}
	return result;
}
*/

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
OSErr	CWindowImp::STATIC_APICB_DragTracking( DragTrackingMessage inMessage, AWindowRef inWindowRef, void * inHandlerRefCon, DragRef inDragRef )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-DrT)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inHandlerRefCon;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;
	
	//B0000 OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		window->APICB_DoDragTracking(inMessage,inDragRef);
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_DragTracking() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-DrT]");
	return noErr;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
OSErr	CWindowImp::STATIC_APICB_DragReceive( AWindowRef inWindowRef, void * inHandlerRefCon, DragRef inDragRef )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-DrR)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inHandlerRefCon;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;
	
	//B0000 OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		window->APICB_DoDragReceive(inDragRef);
		AApplication::GetApplication().NVI_UpdateMenu();
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_DragReceive() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-DrR]");
	return noErr;
	
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j B0370
*/
pascal OSStatus CWindowImp::STATIC_APICB_ServiceGetTypes( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-SeGT)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoServiceGetTypes(inCallRef,inEventRef) )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_ServiceGetTypes() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-SeGT]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j B0370
*/
pascal OSStatus CWindowImp::STATIC_APICB_ServiceCopy( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-SeC)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoServiceCopy(inCallRef,inEventRef) )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_ServiceCopy() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-SeC]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j B0370
*/
pascal OSStatus CWindowImp::STATIC_APICB_ServicePaste( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-SeP)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImp�I�u�W�F�N�g�̃C�x���g�n���h�����s
	try
	{
		if( window->APICB_DoServicePaste(inCallRef,inEventRef) )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_ServicePaste() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-SeP]");
	return result;
}

//R0240
/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j B0370
*/
pascal OSStatus	CWindowImp::STATIC_APICB_ControlHelpTagCallback( ControlRef inControl,
			Point inGlobalMouse, HMContentRequest inRequest,
			HMContentProvidedType *outContentProvided,
			HMHelpContentPtr ioHelpContent )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-HelpTag)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�擾
	if( ExistWindowImpForWindowRef(::GetControlOwner(inControl)) == false )   return noErr;//#0
	CWindowImp&	window = CWindowImp::GetWindowImpByWindowRef(::GetControlOwner(inControl));
	AControlID	controlID = window.GetControlID(inControl);
	
	OSErr status = noErr;
	
	try
	{
		if( inRequest == kHMSupplyContent )
		{
			ioHelpContent->version = kMacHelpVersion;
			ioHelpContent->tagSide = kHMDefaultSide;
			::SetRect(&ioHelpContent->absHotRect, 0, 0, 0, 0);
			
			//mouse�|�C���g��Local�ɕϊ�
			StSetPort	s(window.GetWindowRef());
			Point	pt = inGlobalMouse;
			::GlobalToLocal(&pt);
			AWindowPoint	windowPoint;
			windowPoint.x = pt.h;
			windowPoint.y = pt.v;
			ALocalPoint	localPoint;
			window.GetControlLocalPointFromWindowPoint(controlID,windowPoint,localPoint);
			
			//rect�́A�w���v�^�O�`��ʒu�i�f�t�H���g�ŃR���g���[����rect�����Ă����BEVT_DoGetHelpTag()�Œl���ݒ肳���j
			ALocalRect	rect;
			rect.left		= 0;
			rect.top		= 0;
			rect.right		= window.GetControlWidth(controlID);
			rect.bottom		= window.GetControlHeight(controlID);
			AText	text1, text2;
			//AWindow, AView�I�u�W�F�N�g����A���݂̃}�E�X�|�C���^�ɑΉ�����w���v�^�O�����擾
			AHelpTagSide	tagside = kHelpTagSide_Default;//#643
			if( window.GetAWin().EVT_DoGetHelpTag(controlID,localPoint,text1,text2,rect,tagside) == true )//#643
			{
				//#643
				switch(tagside)
				{
				  case kHelpTagSide_Default:
					{
						ioHelpContent->tagSide = kHMDefaultSide;
						break;
					}
				  case kHelpTagSide_Right:
					{
						ioHelpContent->tagSide = kHMOutsideRightCenterAligned;
						break;
					}
				  case kHelpTagSide_Top:
					{
						ioHelpContent->tagSide = kHMOutsideTopCenterAligned;
						break;
					}
				}
				
				//�w���v�^�O�`��ʒu�ݒ�
				localPoint.x = rect.left;
				localPoint.y = rect.top;
				window.GetWindowPointFromControlLocalPoint(controlID,localPoint,windowPoint);
				pt.h = windowPoint.x;
				pt.v = windowPoint.y;
				::LocalToGlobal(&pt);
				ioHelpContent->absHotRect.left = pt.h;
				ioHelpContent->absHotRect.top = pt.v;
				
				//�w���v�^�O�`��ʒu�ݒ�
				localPoint.x = rect.right;
				localPoint.y = rect.bottom;
				window.GetWindowPointFromControlLocalPoint(controlID,localPoint,windowPoint);
				pt.h = windowPoint.x;
				pt.v = windowPoint.y;
				::LocalToGlobal(&pt);
				ioHelpContent->absHotRect.right = pt.h;
				ioHelpContent->absHotRect.bottom = pt.v;
				
				//�e�L�X�g�ݒ�
				if( text1.GetItemCount() > 0 )
				{
					ioHelpContent->content[kHMMinimumContentIndex].contentType = kHMCFStringContent;
					ioHelpContent->content[kHMMinimumContentIndex].u.tagCFString = text1.CreateCFString();
				}
				else
				{
					ioHelpContent->content[kHMMinimumContentIndex].contentType = kHMNoContent;
				}
				if( text2.GetItemCount() > 0 )
				{
					ioHelpContent->content[kHMMaximumContentIndex].contentType = kHMCFStringContent;
					ioHelpContent->content[kHMMaximumContentIndex].u.tagCFString = text2.CreateCFString();
				}
				else
				{
					ioHelpContent->content[kHMMaximumContentIndex].contentType = kHMNoContent;
				}
				*outContentProvided = kHMContentProvided;
			}
			else
			{
				ioHelpContent->content[kHMMinimumContentIndex].contentType = kHMNoContent;
				ioHelpContent->content[kHMMaximumContentIndex].contentType = kHMNoContent;
				*outContentProvided = kHMContentNotProvided;
			}
		}
		else if( inRequest == kHMDisposeContent ) 
		{
			//�e�L�X�gCFString�I�u�W�F�N�g���
			if( ioHelpContent->content[kHMMinimumContentIndex].contentType == kHMCFStringContent )
			{
				::CFRelease(ioHelpContent->content[kHMMinimumContentIndex].u.tagCFString);
			}
			if( ioHelpContent->content[kHMMaximumContentIndex].contentType == kHMCFStringContent )
			{
				::CFRelease(ioHelpContent->content[kHMMaximumContentIndex].u.tagCFString);
			}
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_ControlHelpTagCallback() caught exception.",NULL);//#199
	}
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-HelpTag]");
	return status;
}

/**
�C�x���g�n���h���o�^
*/
void CWindowImp::InstallEventHandler()
{
	mInstalledEventHandler.DeleteAll();
	
	OSStatus	err = noErr;
	EventHandlerRef	handlerRef;
	EventTypeSpec	typeSpec;
	
	typeSpec.eventClass = kEventClassMouse;
	typeSpec.eventKind = kEventMouseDown;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_MouseDownHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassMouse;
	typeSpec.eventKind = kEventMouseWheelMoved;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_MouseWheelHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassMouse;
	typeSpec.eventKind = kEventMouseMoved;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_DoMouseMoved),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassCommand;
	typeSpec.eventKind = kEventCommandProcess;
	::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_CommandHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassKeyboard;
	typeSpec.eventKind = kEventRawKeyDown;
	::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_KeyDownHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassTextInput; 
	typeSpec.eventKind = kEventTextInputUnicodeForKeyEvent;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_TextInputHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassTextInput;
	typeSpec.eventKind = kEventTextInputUpdateActiveInputArea;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_TextInputUpdateActiveInputAreaHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassTextInput;
	typeSpec.eventKind = kEventTextInputOffsetToPos;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_TextInputOffsetToPosHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassTextInput;
	typeSpec.eventKind = kEventTextInputPosToOffset;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_TextInputPosToOffsetHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassTextInput;
	typeSpec.eventKind = kEventTextInputGetSelectedText;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_TextInputGetSelectedText),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	//B0330
	/*
	typeSpec.eventClass = kEventClassTSMDocumentAccess;
	typeSpec.eventKind = kEventTSMDocumentAccessLockDocument;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_TSMDocumentAccessLockDocument),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACError("",this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassTSMDocumentAccess;
	typeSpec.eventKind = kEventTSMDocumentAccessUnlockDocument;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_TSMDocumentAccessUnlockDocument),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACError("",this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassTSMDocumentAccess;
	typeSpec.eventKind = kEventTSMDocumentAccessGetLength;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_TSMDocumentAccessGetLength),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACError("",this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassTSMDocumentAccess;
	typeSpec.eventKind = kEventTSMDocumentAccessGetSelectedRange;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_TSMDocumentAccessGetSelectedRange),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACError("",this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassTSMDocumentAccess;
	typeSpec.eventKind = kEventTSMDocumentAccessGetCharacters;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_TSMDocumentAccessGetCharacters),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACError("",this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassTSMDocumentAccess;
	typeSpec.eventKind = kEventTSMDocumentAccessGetFont;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_TSMDocumentAccessGetFont),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACError("",this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassTSMDocumentAccess;
	typeSpec.eventKind = kEventTSMDocumentAccessGetGlyphInfo;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_TSMDocumentAccessGetGlyphInfo),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACError("",this);
	mInstalledEventHandler.Add(handlerRef);
	*/
	
	typeSpec.eventClass = kEventClassWindow;
	typeSpec.eventKind = kEventWindowActivated;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_WindowActivatedHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassWindow;
	typeSpec.eventKind = kEventWindowDeactivated;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_WindowDeactivatedHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassWindow;
	typeSpec.eventKind = kEventWindowCollapsed;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_WindowCollapsedHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassWindow;
	typeSpec.eventKind = kEventWindowClose;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_WindowCloseHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassWindow;
	typeSpec.eventKind = kEventWindowBoundsChanged;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_WindowBoundsChangedHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassWindow;
	typeSpec.eventKind = kEventWindowResizeCompleted;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_WindowResizeCompletedHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassWindow;
	typeSpec.eventKind = kEventWindowZoomed;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_WindowZoomedHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassWindow;
	typeSpec.eventKind = kEventWindowCursorChange;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_WindowCursorChangeHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassWindow;
	typeSpec.eventKind = kEventWindowFocusAcquired;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_WindowFocusAcquiredHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	//B0370
	typeSpec.eventClass = kEventClassService;
	typeSpec.eventKind = kEventServiceGetTypes;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_ServiceGetTypes),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassService;
	typeSpec.eventKind = kEventServiceCopy;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_ServiceCopy),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	typeSpec.eventClass = kEventClassService;
	typeSpec.eventKind = kEventServicePaste;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_ServicePaste),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACErrorNum("InstallEventHandler() returned error: ",err,this);
	mInstalledEventHandler.Add(handlerRef);
	
	/*
	kEventWindowContextualMenuSelect�́Acomposite�ł͂Ȃ��E�C���h�E(StandardWindow Handler)�p
	typeSpec.eventClass = kEventClassWindow;
	typeSpec.eventKind = kEventWindowContextualMenuSelect;
	err = ::InstallEventHandler(::GetWindowEventTarget(mWindowRef),::NewEventHandlerUPP(CWindowImp::STATIC_APICB_ContextMenuHandler),
			1,&typeSpec,this,&handlerRef);
	if( err != noErr )   _ACError("");
	mInstalledEventHandler.Add(handlerRef);
	*/
	
	::InstallTrackingHandler(::NewDragTrackingHandlerUPP(STATIC_APICB_DragTracking),mWindowRef,this);
	::InstallReceiveHandler(::NewDragReceiveHandlerUPP(STATIC_APICB_DragReceive),mWindowRef,this);
}

/**
�C�x���g�n���h���폜
*/
void	CWindowImp::UninstallEventHandler()
{
	OSStatus	err = noErr;
	
	//�C�x���g�n���h���[�̍폜
	for( AIndex i = 0; i < mInstalledEventHandler.GetItemCount(); i++ )
	{
		err = ::RemoveEventHandler(mInstalledEventHandler.Get(i));
		if( err != noErr )   _ACErrorNum("RemoveEventHandler() returned error: ",err,this);
	}
	mInstalledEventHandler.DeleteAll();
}
#endif
#pragma mark ===========================

#pragma mark ---���[�e�B���e�B

/**
���݂̃}�E�X�ʒu�擾
@note �C�x���g��M���ł͂Ȃ��A���݂̃}�E�X�ʒu�B�C�x���g��M�����ƁA�C�x���g��M��ɃE�C���h�E�ړ������ꍇ�Ȃǂɖ�蔭������̂ŁA���̊֐����g���B
*/
void	CWindowImp::GetCurrentMouseLocation( AGlobalPoint& outGlobalPoint )
{
	//�}�E�X�ʒu�擾
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	NSPoint	mouseScreenPoint = [NSEvent mouseLocation];
	outGlobalPoint.x = mouseScreenPoint.x;
	outGlobalPoint.y = firstScreenFrame.size.height - mouseScreenPoint.y;
}


#pragma mark ===========================

#pragma mark ---��ʃT�C�Y�擾

//win AWindowWrapper����ړ�
/**
�E�C���h�E�z�u�\�ȗ̈���擾�iDock, Menubar���ɏd�Ȃ�Ȃ��̈�j�i(0,0)�����݂���X�N���[�����j
*/
void	CWindowImp::GetAvailableWindowBoundsInMainScreen( AGlobalRect& outRect )
{
	/*#688
	OSStatus	err = noErr;
	
	Rect	rect;
	err = ::GetAvailableWindowPositioningBounds(::GetMainDevice(),&rect);
	if( err != noErr )   _ACErrorNum("GetAvailableWindowPositioningBounds() returned error: ",err,NULL);
	outRect.left 	= rect.left;
	outRect.top 	= rect.top;
	outRect.right 	= rect.right;
	outRect.bottom	= rect.bottom;
	*/
	NSRect	screenVisibleFrame = [[[NSScreen screens] objectAtIndex:0] visibleFrame];
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	outRect.left		= screenVisibleFrame.origin.x;
	outRect.top			= firstScreenFrame.size.height - screenVisibleFrame.origin.y - screenVisibleFrame.size.height;
	outRect.right		= screenVisibleFrame.origin.x + screenVisibleFrame.size.width;
	outRect.bottom		= firstScreenFrame.size.height - screenVisibleFrame.origin.y;
	
}

/**
�E�C���h�E�z�u�\�ȗ̈���擾�iDock, Menubar���ɏd�Ȃ�Ȃ��̈�j�i�E�C���h�E�����݂���X�N���[�����j
*/
void	CWindowImp::GetAvailableWindowBoundsInSameScreen( AGlobalRect& outRect ) const
{
	NSRect	screenVisibleFrame = [[CocoaObjects->GetWindow() screen] visibleFrame];
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	outRect.left		= screenVisibleFrame.origin.x;
	outRect.top			= firstScreenFrame.size.height - screenVisibleFrame.origin.y - screenVisibleFrame.size.height;
	outRect.right		= screenVisibleFrame.origin.x + screenVisibleFrame.size.width;
	outRect.bottom		= firstScreenFrame.size.height - screenVisibleFrame.origin.y;
}

/**
�E�C���h�E�z�u�\�ȗ̈���擾�iDock, Menubar���ɏd�Ȃ�Ȃ��̈�j�i�w��X�N���[�����j
*/
void	CWindowImp::GetAvailableWindowBounds( const AIndex inScreenIndex, AGlobalRect& outRect ) 
{
	NSRect	screenVisibleFrame = [[[NSScreen screens] objectAtIndex:inScreenIndex] visibleFrame];
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	outRect.left		= screenVisibleFrame.origin.x;
	outRect.top			= firstScreenFrame.size.height - screenVisibleFrame.origin.y - screenVisibleFrame.size.height;
	outRect.right		= screenVisibleFrame.origin.x + screenVisibleFrame.size.width;
	outRect.bottom		= firstScreenFrame.size.height - screenVisibleFrame.origin.y;
}

/**
�w��|�C���g����screen index���擾
*/
AIndex	CWindowImp::FindScreenIndexFromPoint( const APoint inPoint )
{
	//�X�N���[�����̃��[�v
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	AItemCount	screenCount = [[NSScreen screens] count];
	for( AIndex i = 0; i < screenCount; i++ )
	{
		//�E�C���h�E�z�u�\�̈�擾
		NSRect	screenVisibleFrame = [[[NSScreen screens] objectAtIndex:i] visibleFrame];
		ARect	screenBounds = {0};
		screenBounds.left		= screenVisibleFrame.origin.x;
		screenBounds.top		= firstScreenFrame.size.height - screenVisibleFrame.origin.y - screenVisibleFrame.size.height;
		screenBounds.right		= screenVisibleFrame.origin.x + screenVisibleFrame.size.width;
		screenBounds.bottom		= firstScreenFrame.size.height - screenVisibleFrame.origin.y;
		//�|�C���g���͈͓����ǂ����̔���
		if( inPoint.x >= screenBounds.left && inPoint.x <= screenBounds.right &&
					inPoint.y >= screenBounds.top && inPoint.y <= screenBounds.bottom )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

#pragma mark ===========================

#pragma mark ---LineBreak�擾

/**
�sBreak�ʒu�̌v�Z
*/
/*win
AItemCount	CWindowImp::GetLineBreak( const AText& inUTF16Text, const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAlias,
		const ANumber inViewWidth )
{
	OSStatus	err = noErr;
	
	ATSUStyle	atsuStyle = NULL;
	err = ::ATSUCreateStyle(&atsuStyle);
	if( err != noErr )   _ACErrorNum("ATSUCreateStyle() returned error: ",err,NULL);
	if( atsuStyle == NULL ) { _ACError("",NULL); return 1; }
	SInt16	font;//win
	AFontWrapper::GetFontByName(inFontName,font);//win
	SetATSUStyle(atsuStyle,font,inFontSize,inAntiAlias,kColor_Black,kTextStyle_Normal);
	ATSUTextLayout	textLayout = CreateTextLayout(atsuStyle,inUTF16Text,inAntiAlias,NULL);
	if( textLayout == NULL ) { _ACError("",NULL); return 1; }
	UniCharArrayOffset	endUniPos = 1;
	OSStatus status = ::ATSUBreakLine(textLayout,kATSUFromTextBeginning,Long2Fix(inViewWidth),false,&endUniPos);
	if( status != noErr )
	{
		//��������
	}
	err = ::ATSUDisposeTextLayout(textLayout);
	if( err != noErr )   _ACErrorNum("ATSUDisposeTextLayout() returned error: ",err,NULL);
	err = ::ATSUDisposeStyle(atsuStyle);
	if( err != noErr )   _ACErrorNum("ATSUDisposeStyle() returned error: ",err,NULL);
	return endUniPos;
}
*/

//B0000 �s�܂�Ԃ��v�Z������
/**
�sBreak�ʒu�̌v�Z
*/
AItemCount	CWindowImp::GetLineBreak( CTextDrawData& ioTextDrawData, AItemCount inStartOffset, 
									  const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAlias,
									  const ANumber inViewWidth, const ABool inLetterWrap )//#1113
{
	OSStatus	err = noErr;
	
	/*#1034
	//ATSUStyle�ݒ�
	if( ioTextDrawData.mATSUStyle == NULL )
	{
		if( ioTextDrawData.GetTextLayout() != NULL )   _ACThrow("",NULL);
		err = ::ATSUCreateStyle(&(ioTextDrawData.mATSUStyle));
		if( err != noErr )   _ACErrorNum("ATSUCreateStyle() returned error: ",err,NULL);
		if( ioTextDrawData.mATSUStyle == NULL ) { _ACError("",NULL); return 1; }
		//#688 SInt16	font;//win
		//#688 AFontWrapper::GetFontByName(inFontName,font);//win
		SetATSUStyle(ioTextDrawData.mATSUStyle,inFontName,inFontSize,inAntiAlias,kColor_Black,1.0,kTextStyle_Normal);
	}
	//TextLayout����
	if( ioTextDrawData.GetTextLayout() == NULL )
	{
		ATSUTextLayout	textLayout = CreateTextLayout(ioTextDrawData.mATSUStyle,ioTextDrawData.UTF16DrawText,inAntiAlias,
						ioTextDrawData.IsFontFallbackEnabled(),NULL);
		if( textLayout == NULL ) { _ACError("",NULL); return 1; }
		ioTextDrawData.SetTextLayout(textLayout);
	}
	*/
	if( ioTextDrawData.GetCTTypesetterRef() == NULL )
	{
		//�\���e�L�X�g�擾
		AStCreateNSStringFromUTF16Text	str(ioTextDrawData.UTF16DrawText);
		//�\���e�L�X�g��NSMutableAttributedString�擾
		NSMutableAttributedString *attrString = [[NSMutableAttributedString alloc] initWithString:str.GetNSString()];
		//attrString�̓C�x���g���[�v�I���ŉ���BCTLine�ŃR�s�[���Ă��邩�Q�Ƃ��Ă��邩�͕s�������A�Q�Ƃ��Ă���Ȃ�retain()���Ă���͂��B
		[attrString autorelease];
		//NSMutableAttributedString�̕ҏW�J�n
		[attrString beginEditing];
		
		//�ʏ�t�H���g�擾
		/*#1090
		AStCreateNSStringFromAText	fontname(inFontName);
		NSFontDescriptor*	fontDescriptor = [NSFontDescriptor fontDescriptorWithName: fontname.GetNSString() 
																				 size: inFontSize];
		NSFont*	normalfont = [NSFont fontWithDescriptor:fontDescriptor size:inFontSize];
		if( normalfont == NULL )
		{
			//�t�H���g�擾�ł��Ȃ��Ƃ��̓V�X�e���t�H���g���g�p����
			normalfont = [NSFont systemFontOfSize:inFontSize];
		}
		*/
		NSFont*	normalfont = GetNSFont(inFontName,inFontSize,kTextStyle_Normal);
		// ==================== �e�L�X�g�����ݒ� ====================
		//range�ݒ�
		NSRange	range = NSMakeRange(0,[attrString length]);
		//�t�H���g�ݒ�
		[attrString addAttribute:NSFontAttributeName value:normalfont range:range];
		
		//NSMutableAttributedString�̕ҏW�I��
		[attrString endEditing];
		
		//CTTypesetter�擾
		ioTextDrawData.SetCTTypesetterRef(::CTTypesetterCreateWithAttributedString((CFAttributedStringRef)attrString));
	}
	
	//��������������ƃo�C�i���t�@�C���̍s����v�Z�Ő��̕s���̃o�O���N����̂ŁA�ꕶ���͕\���ł�����x�̕����m�ۂ���
	ANumber	minwidth = 24;
	ANumber	width = inViewWidth;
	if( width <= minwidth )   width = minwidth;
	
	//�s�܂�Ԃ��ʒu�擾
	CFIndex start = inStartOffset;
	CFIndex count = 0;
	if( inLetterWrap == false )
	{
		count = ::CTTypesetterSuggestLineBreak(ioTextDrawData.GetCTTypesetterRef(), start, width);
	}
	//#1113 �������ƍs�܂�Ԃ�
	else
	{
		count = ::CTTypesetterSuggestClusterBreak(ioTextDrawData.GetCTTypesetterRef(), start, width);
	}
	AIndex	endUniPos = start + count;
	
	/*#1034
	UniCharArrayOffset	endUniPos = 1;
	OSStatus status = ::ATSUBreakLine(ioTextDrawData.GetTextLayout(),inStartOffset,Long2Fix(width),true,&endUniPos);
	if( status != noErr )
	{
		//��������
	}
	*/
	//fprintf(stderr,"%d,%d  ",ioTextDrawData.mTextLayout,status);
	//ATSUStyle, TextLayout�͍ė��p���ACTextDrawData�̍폜�EInit���ɉ��
	/*::ATSUDisposeTextLayout(ioTextDrawData.mTextLayout);
	ioTextDrawData.mTextLayout = NULL;
	::ATSUDisposeStyle(ioTextDrawData.mATSUStyle);
	ioTextDrawData.mATSUStyle = NULL;*/
	
	return endUniPos;
}

//#853
/**
�e�L�X�g�̍s�܂�Ԃ��v�Z�i���s�R�[�h�Ή��EUTF8�ł�linestart, length���擾�j
*/
void	CWindowImp::CalcLineBreaks( const AText& inText, 
									const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAlias,
									const ANumber inViewWidth, const ABool inLetterWrap, //#1113
									AArray<AIndex>& outUTF8LineStartArray, AArray<AItemCount>& outUTF8LineLengthArray )
{
	//���ʏ�����
	outUTF8LineStartArray.DeleteAll();
	outUTF8LineLengthArray.DeleteAll();
	//�e�L�X�g�e�������̃��[�v
	AItemCount	len = inText.GetItemCount();
	for( AIndex pos = 0; pos < len; )
	{
		//�i���J�n�ʒu��paraStart�ɋL��
		AIndex	paraStart = pos;
		//�i���擾
		AText	paraText;
		inText.GetLine(pos,paraText);
		//�i����text draw data���擾
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(paraText,1,0,true,true,false,0,paraText.GetItemCount());
		//�܂�Ԃ��v�Z�iUTF16�ł�index���擾�j
		AArray<AIndex>	startOffsetArray, endOffsetArray;
		CalcLineBreaks(textDrawData,0,inFontName,inFontSize,inAntiAlias,inViewWidth,inLetterWrap,startOffsetArray,endOffsetArray);//#1113
		//�e�L�X�g�̍ŏ�����́AUTF8��pos�ɕϊ����āA���ʊi�[
		for( AIndex i = 0; i < startOffsetArray.GetItemCount(); i++ )
		{
			//�J�n�ʒu
			outUTF8LineStartArray.Add(paraStart + textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(startOffsetArray.Get(i)));
			//�����O�X
			AItemCount	lineLength = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(endOffsetArray.Get(i)) - 
					textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(startOffsetArray.Get(i));
			if( i == startOffsetArray.GetItemCount()-1 )
			{
				lineLength = pos - paraStart - textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(startOffsetArray.Get(i));
			}
			outUTF8LineLengthArray.Add(lineLength);
		}
	}
}

//#853
/**
�e�L�X�g�̍s�܂�Ԃ��v�Z�i���s�R�[�h���Ή��EUTF16�ł�lineStart, length���擾�j
*/
void	CWindowImp::CalcLineBreaks( CTextDrawData& ioTextDrawData, AItemCount inStartOffset, 
									const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAlias,
									const ANumber inViewWidth, const ABool inLetterWrap, //#1113
									AArray<AIndex>& outUTF16StartOffsetArray, AArray<AIndex>& outUTF16EndOffsetArray )
{
	//���ʏ�����
	outUTF16StartOffsetArray.DeleteAll();
	outUTF16EndOffsetArray.DeleteAll();
	//�e�s�����[�v
	AIndex	startOffset = inStartOffset;
	while( ioTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - startOffset > 0 )
	{
		//�s�܂�Ԃ��ʒu�擾
		AItemCount	endOffset = CWindowImp::GetLineBreak(ioTextDrawData,startOffset,inFontName,inFontSize,inAntiAlias,inViewWidth,inLetterWrap);//#1113
		if( endOffset <= startOffset )   endOffset = startOffset+1;//�������[�v�h�~
		
		//���ʊi�[
		outUTF16StartOffsetArray.Add(startOffset);
		outUTF16EndOffsetArray.Add(endOffset);
		startOffset = endOffset;
	}
	//���ʖ����̏ꍇ�̏���
	if( outUTF16StartOffsetArray.GetItemCount() == 0 )
	{
		outUTF16StartOffsetArray.Add(0);
		outUTF16EndOffsetArray.Add(0);
	}
}

//#1090
/**
NSFont�L���b�V��
*/
AHashTextArray	CWindowImp::sNSFontCache_Key;
AArray<NSFont*>	CWindowImp::sNSFontCache_NSFont;
//�Ō�Ɏg�����t�H���g���L���b�V�����Ă��� #1275
NSFont*			CWindowImp::sLastNSFont = nil;
AText			CWindowImp::sLastNSFont_FontName;
AFloatNumber	CWindowImp::sLastNSFont_FontSize = 0;
ATextStyle		CWindowImp::sLastNSFont_TextStyle = kTextStyle_Normal;

//#1090
/**
NSFont�擾
*/
NSFont*	CWindowImp::GetNSFont( const AText& inFontName, const AFloatNumber inFontSize, const ATextStyle inTextStyle )
{
	//�Ō�Ɏg�����L���b�V���Ɠ����t�H���g�Ȃ炻���Ԃ��isLast�`�A�N�Z�X���X���b�h�Z�[�t�ł͂Ȃ��̂Ń��C���X���b�h�̏ꍇ�̂݁j #1275
	if( ABaseFunction::InMainThread() == true )
	{
		if( sLastNSFont != nil && inFontName.Compare(sLastNSFont_FontName) == true && inFontSize == sLastNSFont_FontSize && inTextStyle == sLastNSFont_TextStyle )
				{
			return sLastNSFont;
		}
	}
	//�t�H���g���E�t�H���g�T�C�Y�E�X�^�C������key�����񐶐�
	AText	key;
	key.SetText(inFontName);
	key.Add('\t');
	key.AddNumber((ANumber)(inFontSize*10.0));
	key.Add('\t');
	key.AddNumber((ANumber)(inTextStyle));
	//�L���b�V�����猟��
	AIndex	cacheIndex = sNSFontCache_Key.Find(key);
	if( cacheIndex != kIndex_Invalid )
	{
		//�L���b�V���Ƀq�b�g������A�����Ԃ�
		//�L���b�V������t�H���g�擾
		NSFont* font = sNSFontCache_NSFont.Get(cacheIndex);
		//�Ō�Ɏg�����L���b�V����ۑ�����isLast�`�A�N�Z�X���X���b�h�Z�[�t�ł͂Ȃ��̂Ń��C���X���b�h�̏ꍇ�̂݁j #1275
		if( ABaseFunction::InMainThread() == true )
		{
			sLastNSFont = font;
			sLastNSFont_FontName.SetText(inFontName);
			sLastNSFont_FontSize = inFontSize;
			sLastNSFont_TextStyle = inTextStyle;
		}
		return font;
	}
	else
	{
		//�Ԃ�l
		NSFont*	font;
		//�ʏ�t�H���g�擾
		AStCreateNSStringFromAText	fontname(inFontName);
		NSFontDescriptor*	fontDescriptor = [NSFontDescriptor fontDescriptorWithName: fontname.GetNSString() size:inFontSize];
		NSFont*	normalfont = [NSFont fontWithDescriptor:fontDescriptor size:inFontSize];
		if( normalfont == NULL )
		{
			//�t�H���g�擾�ł��Ȃ��Ƃ��̓V�X�e���t�H���g���g�p����
			normalfont = [NSFont systemFontOfSize:inFontSize];
		}
		if( inTextStyle == kTextStyle_Normal )
		{
			//�ʏ�t�H���g
			font = normalfont;
		}
		else
		{
			//bold, italic�t�H���g
			NSInteger	weight = 5;
			NSFontTraitMask	fontTraitMask = 0;
			if( (inTextStyle&kTextStyle_Bold) != 0 )
			{
				fontTraitMask |= NSBoldFontMask;
				weight = 9;
			}
			if( (inTextStyle&kTextStyle_Italic) != 0 )
			{
				fontTraitMask |= NSItalicFontMask;
			}
			//bold,italic�t�H���g�擾
			font = [[NSFontManager sharedFontManager] convertFont:normalfont toHaveTrait:fontTraitMask];
			//���ɉ��L�̂悤�Ȏ擾���@�����������A�ǂ�������iMonaco��bold�͎擾�ł��Ȃ��j
			//NSFont*	boldfont = [[NSFontManager sharedFontManager] convertWeight:YES ofFont:normalfont];
			//NSFont*	boldfont = [[NSFontManager sharedFontManager] fontWithFamily:@"Monaco" traits:NSFontBoldTrait
			//															  weight:15 size:mFontSize];
			//NSFont*	boldfont = [NSFont fontWithDescriptor:[fontDescriptor fontDescriptorWithSymbolicTraits:NSFontBoldTrait] 
			//										 size:mFontSize];
			/*
			NSDictionary *fontAttributes =
					[NSDictionary dictionaryWithObjectsAndKeys:
					  fontname.GetNSString(), (NSString *)kCTFontFamilyNameAttribute,
					  [NSNumber numberWithFloat:mFontSize],
					 (NSString *)kCTFontSizeAttribute,
					 [NSNumber numberWithInt:kCTFontItalicTrait],
					 (NSString *)kCTFontSymbolicTrait,
					  [NSNumber numberWithFloat:1.0],
					  (NSString *)kCTFontWeightTrait,
					 nil];
			NSFontDescriptor*	boldFontDescriptor = [NSFontDescriptor fontDescriptorWithFontAttributes:fontAttributes];
			NSFont*	boldfont = [NSFont fontWithDescriptor:boldFontDescriptor size:mFontSize];
			*/
			if( font == NULL || font == normalfont )
			{
				//bold�t�H���g�擾�ł��Ȃ��ꍇ�A�܂��́A�ʏ�t�H���g�Ɠ����Ƃ���Helvetica��bold/italic���擾����
				font = [[NSFontManager sharedFontManager] fontWithFamily:@"Helvetica" traits:fontTraitMask 
				weight:weight size:inFontSize];
				if( font == NULL ) 
				{
					//�t�H���g�擾�ł��Ȃ��Ƃ��͒ʏ�t�H���g���g�p����
					font = normalfont;
				}
			}
		}
		//�L���b�V���ɓo�^
		[font retain];//���ꂪ�Ȃ��ƁA�^�u����̃E�C���h�E����āA�ēx�A�E�C���h�E���J�����Ƃ��ɗ�����B�ifont�I�u�W�F�N�g��release�����炵���B�j
		sNSFontCache_Key.Add(key);
		sNSFontCache_NSFont.Add(font);
		//
		return font;
	}
}

#pragma mark ===========================

#pragma mark ---�C�����C������

/**
�C�����C�����͂������m��
*/
void	CWindowImp::FixInlineInput( const AControlID inControlID )
{
	if( true )
	{
		//if( inControlID != kControlID_Invalid )
		{
			//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
			NSView*	view = CocoaObjects->FindViewByTag(inControlID);
			
			//view��fixInlineInput���s
			[view fixInlineInput];
		}
		/*
		else
		{
			AItemCount	itemCount = mUserPaneArray_ControlID.GetItemCount();
			for( AIndex i = 0; i < itemCount; i++ )
			{
				AControlID	controlID = mUserPaneArray_ControlID.Get(i);
				
				//AControlID(=NSView��tag�Ƃ��Đݒ肳��Ă���)����ANSView������
				NSView*	view = CocoaObjects->FindViewByTag(controlID);
				
				//view��fixInlineInput���s
				[view fixInlineInput];
			}
		}
		*/
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		err = ::FixTSMDocument(mTSMID);
		//if( err != noErr )   _ACErrorNum("FixTSMDocument() returned error: ",err,this);
	}
#endif
}

//#1034
#if 0
#pragma mark ===========================

#pragma mark ---ATSU���[�e�B���e�B

/**
ATSUStyle��ݒ�
*/
void	CWindowImp::SetATSUStyle( ATSUStyle& ioATSUStyle, const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAliasing,
		const AColor inColor, const AFloatNumber inAlpha, const ATextStyle inTextStyle )
{
	OSStatus	err = noErr;
	
	ItemCount	itemcount = 0;
	err = ::ATSUClearAttributes(ioATSUStyle,kATSUClearAll,NULL);
	if( err != noErr )   _ACErrorNum("ATSUClearAttributes() returned error: ",err,NULL);
	ATSUAttributeTag	attrTag[16];
	ByteCount	tagValueSize[16];
	ATSUAttributeValuePtr	tagValuePtr[16];
	ATSUFontID	fontID = 0;
	//#688 ATSUFONDtoFontID(inFont,NULL,&fontID);
	//�t�H���g������ATSUFontID�擾
	//�P�D�܂��f�t�H���g�t�H���g�w��Ȃ�f�t�H���g�t�H���g���擾
	if( inFontName.GetItemCount() == 0 || inFontName.Compare("default") == true )
	{
		AText	defaultFontName;
		AFontWrapper::GetAppDefaultFontName(defaultFontName);
		fontID = AFontWrapper::GetATSUFontIDByName(defaultFontName);
	}
	//�Q�D�f�t�H���g�t�H���g�w��łȂ��ꍇ�́A�t�H���g���ɏ]���ăt�H���g�擾
	if( fontID == 0 )
	{
		fontID = AFontWrapper::GetATSUFontIDByName(inFontName);
	}
	//�R�D�t�H���g�擾���s�̏ꍇ�́A�A�v���P�[�V�����̃f�t�H���g�t�H���g��ݒ�
	if( fontID == 0 )
	{
		AText	defaultFontName;
		AFontWrapper::GetAppDefaultFontName(defaultFontName);
		fontID = AFontWrapper::GetATSUFontIDByName(defaultFontName);
	}
	//�S�D�A�v���P�[�V�����f�t�H���g�t�H���g�擾���s�̏ꍇ�́Adialog font��ݒ�idialog font�̓V�X�e���ŕK���L���ȃt�H���g���擾�ł���j
	if( fontID == 0 )
	{
		AText	defaultFontName;
		AFontWrapper::GetDialogDefaultFontName(defaultFontName);
		fontID = AFontWrapper::GetATSUFontIDByName(defaultFontName);
	}
	//
	attrTag[itemcount] = kATSUFontTag;
	tagValueSize[itemcount]= sizeof(ATSUFontID);
	tagValuePtr[itemcount] = &fontID;
	itemcount++;
	Fixed	size = ConvertFloatToFixed(inFontSize);
	attrTag[itemcount] = kATSUSizeTag;
	tagValueSize[itemcount] = sizeof(Fixed);
	tagValuePtr[itemcount] = &size;
	itemcount++;
	/*zantei 
	if( inAntiAliasing )//B0183  �A���`�G�C���A�XOFF����tracking�ݒ肵�Ȃ��B
	{
	attrTag[itemcount] = kATSUTrackingTag;//R0105
	tagValueSize[itemcount] = sizeof(Fixed);
	Fixed	tracking =  mTextTracking;
	tagValuePtr[itemcount] = &tracking;
	itemcount++;
	}*/
	err = ::ATSUSetAttributes(ioATSUStyle,itemcount,attrTag,tagValueSize,tagValuePtr);
	if( err != noErr )   _ACErrorNum("ATSUSetAttributes() returned error: ",err,NULL);
	
	ATSUFontFeatureType	fontType[16];
	ATSUFontFeatureSelector	fontSelector[16];
	fontType[0] = kAllTypographicFeaturesType;
	fontSelector[0] = kAllTypeFeaturesOffSelector;
	err = ::ATSUSetFontFeatures(ioATSUStyle,1,fontType,fontSelector);
	if( err != noErr )   _ACErrorNum("ATSUSetFontFeatures() returned error: ",err,NULL);
	
	//B0000 ������ 
	//color, style��mATSUStyle�ɐݒ�ł���悤�ɂ���BDrawText()��color, style�w�薳���łŁAmATSUStyle���g���΃f�t�H���gcolor, style���g�p�����悤�ɂ���B
	//�i�ȉ��A��{�I��DrawText()����R�s�[�j
	
	//
	attrTag[0] = kATSURGBAlphaColorTag;//kATSUColorTag;
	tagValueSize[0]= sizeof(ATSURGBAlphaColor);//sizeof(RGBColor);
	RGBColor	c = inColor;
	ATSURGBAlphaColor	color = {0};
	color.red = c.red;
	color.red /= 65535;
	color.green = c.green;
	color.green /= 65535;
	color.blue = c.blue;
	color.blue /= 65535;
	color.alpha = inAlpha;
	
	//�F�f�[�^��ǉ�
	tagValuePtr[0] = &color;
	err = ::ATSUSetAttributes(ioATSUStyle,1,attrTag,tagValueSize,tagValuePtr);
	if( err != noErr )   _ACErrorNum("ATSUSetAttributes() returned error: ",err,NULL);
	
	//�X�^�C���f�[�^��ǉ�
	//DrawText()��runstyle�ݒ�ŁA�ݒ肵�Ȃ���΃f�t�H���g�A�Ƃ��邽�߂ɁAb��true�ł�false�ł��l��ݒ肷��B
	attrTag[0] = kATSUQDBoldfaceTag;
	tagValueSize[0] = sizeof(Boolean);
	Boolean	b = ((inTextStyle&kTextStyle_Bold) != 0);
	tagValuePtr[0] = &b;
	err = ::ATSUSetAttributes(ioATSUStyle,1,attrTag,tagValueSize,tagValuePtr);
	if( err != noErr )   _ACErrorNum("ATSUSetAttributes() returned error: ",err,NULL);
	attrTag[0] = kATSUQDUnderlineTag;
	tagValueSize[0] = sizeof(Boolean);
	b = ((inTextStyle&kTextStyle_Underline) != 0);
	tagValuePtr[0] = &b;
	::ATSUSetAttributes(ioATSUStyle,1,attrTag,tagValueSize,tagValuePtr);
	//R0195
	attrTag[0] = kATSUQDItalicTag;
	tagValueSize[0] = sizeof(Boolean);
	b = ((inTextStyle&kTextStyle_Italic) != 0);
	tagValuePtr[0] = &b;
	::ATSUSetAttributes(ioATSUStyle,1,attrTag,tagValueSize,tagValuePtr);
	/*attrTag[0] = kATSUStyleDropShadowTag;
	tagValueSize[0] = sizeof(Boolean);
	b = ((inTextStyle&kTextStyle_DropShadow) != 0);
	tagValuePtr[0] = &b;
	::ATSUSetAttributes(ioATSUStyle,1,attrTag,tagValueSize,tagValuePtr);*/
}

//��s�ɕ\�����镶�����̍ő� #408 �i�������ɂ���ƁA���s�f�[�^���Ȃ��Ƃ��Ɉُ�ɒx���Ȃ�j#408
//#572 �g�p���Ă��Ȃ��̂� const ANumber	kLineTextDrawMax = 100000;

/**
TextLayout����
@note ��������TextLayout���������܂ŁAinUTF16Text�͉���s�B
*/
ATSUTextLayout	CWindowImp::CreateTextLayout( const ATSUStyle inATSUStyle, const AText& inUTF16Text, const ABool inAntiAliasing, 
				const ABool inUseFontFallback, ADrawContextRef inContextRef )//#703
{
	OSStatus	err = noErr;
	
	ATSUTextLayout	textLayout = NULL;
	{
		AItemCount	unilen = inUTF16Text.GetItemCount()/sizeof(UniChar);//#408
		//�s�܂�Ԃ�����̏ꍇ�A1�i���������Ȃ肤��̂ł�͂萧���͕s�� if( unilen > kLineTextDrawMax )   unilen = kLineTextDrawMax;//#408
		AStTextPtr	ptr(inUTF16Text,0,inUTF16Text.GetItemCount());
		UniCharCount	runLength[1];
		runLength[0] = unilen;//#408 inUTF16Text.GetItemCount()/sizeof(UniChar);
		ATSUStyle	style[1];
		style[0] = inATSUStyle;
		err = ::ATSUCreateTextLayoutWithTextPtr((UniChar*)(ptr.GetPtr()),kATSUFromTextBeginning,kATSUToTextEnd,
				/*#408 inUTF16Text.GetItemCount()/sizeof(UniChar)*/unilen,1,runLength,style,&textLayout);
		//if( status != noErr )   _ACError("",NULL);
	}
	if( textLayout == NULL )   _ACThrow("cannot create textLayout",NULL);
	//�t�H���g�t�H�[���o�b�N�g�p
	err = ::ATSUSetTransientFontMatching(textLayout,inUseFontFallback);//true);
	if( err != noErr )   _ACErrorNum("ATSUSetTransientFontMatching() returned error: ",err,NULL);
	//�`��I�v�V�����ݒ�
	ATSUAttributeTag	attrTag[3];
	ByteCount	tagValueSize[3];
	ATSUAttributeValuePtr	tagValuePtr[3];
	ItemCount	attrCount = 0;
	attrTag[attrCount] = kATSULineLayoutOptionsTag;
	tagValueSize[attrCount] = sizeof(ATSLineLayoutOptions);
	ATSLineLayoutOptions	options = kATSLineHasNoHangers + 
							//R0108 http://lists.apple.com/archives/carbon-dev/2005/Sep/msg00731.html
							kATSLineDisableAllGlyphMorphing + kATSLineDisableAllKerningAdjustments +  
							kATSLineDisableAllBaselineAdjustments + kATSLineDisableAllTrackingAdjustments;
	tagValuePtr[attrCount] = &options;
	if( inAntiAliasing == true )
	{
		options |= kATSLineApplyAntiAliasing;
	}
	else
	{
		options |= kATSLineNoAntiAliasing | kATSLineUseDeviceMetrics;
	}
	attrCount++;
	//#703
	//Font Fallback�ݒ�
	if( inUseFontFallback == true )
	{
		ATSUFontFallbacks	fontFallbacks = CAppImp::GetATSUFontFallbacks();
		if( fontFallbacks != NULL )
		{
			attrTag[attrCount] = kATSULineFontFallbacksTag;
			tagValueSize[attrCount] = sizeof(fontFallbacks);
			tagValuePtr[attrCount] = &fontFallbacks;
			attrCount++;
		}
	}
	//CGContext��ݒ�
	if( inContextRef != NULL )
	{
		attrTag[attrCount] = kATSUCGContextTag;
		tagValueSize[attrCount] = sizeof(inContextRef);
		tagValuePtr[attrCount] = &inContextRef;
		attrCount++;
	}
	err = ::ATSUSetLayoutControls(textLayout,attrCount,attrTag,tagValueSize,tagValuePtr);
	if( err != noErr )   _ACErrorNum("ATSUSetLayoutControls() returned error: ",err,NULL);
	return textLayout;
}
#endif
//#539
#if 0
#pragma mark ===========================

#pragma mark ---�w���v

/**
Help�A���J�[�ݒ�
*/
void	CWindowImp::RegisterHelpAnchor( const ANumber inHelpNumber, AConstCstringPtr inAnchor )
{
	AText	text;
	text.SetCstring(inAnchor);
	mHelpAnchor_Number.Add(inHelpNumber);
	mHelpAnchor_Anchor.Add(text);
}

/**
Help�y�[�W�\��
*/
void	CWindowImp::DisplayHelpPage( const ANumber inHelpNumber )
{
	AIndex	index = mHelpAnchor_Number.Find(inHelpNumber);
	if( index == kIndex_Invalid ) 
	{
		_AError("no help page registered",this);
		return;
	}
	//#539 CFBundleHelpBookName��net.mimikaki.mi.help�ɕύX�����̂ŁAApple Title�ƈ�v���Ȃ��Ȃ����B
	AText	helpTitle;
	helpTitle.SetLocalizedText("HelpTitle");
	AStCreateCFStringFromAText	helpTitleStr(helpTitle);
	//
	//#539 CFBundleRef	appBundle = ::CFBundleGetMainBundle();
	//#539 CFStringRef myBookName = NULL;
	//#539 myBookName = (CFStringRef)::CFBundleGetValueForInfoDictionaryKey(appBundle,CFSTR("CFBundleHelpBookName"));
	AStCreateCFStringFromAText	str(mHelpAnchor_Anchor.GetTextConst(index));
	::AHLookupAnchor(/*#539 myBookName*/helpTitleStr.GetRef(),str.GetRef());//#539
}
#endif

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---FullKeyboardAcess

//#353
/**
�t���L�[�{�[�h�A�N�Z�X�ݒ肩�ǂ����𔻒肷��
*/
ABool	CWindowImp::STATIC_CheckIsFocusGroup2ndPattern()
{
	OSStatus	err = noErr;
	//�`�F�b�N�p�_�~�[�E�C���h�E�擾
	WindowRef	windowRef;
	IBNibRef	nibRef;
	err = ::CreateNibReference(CFSTR("AbsFrameworkCommonResource"),&nibRef);
	if( err != noErr )   {_ACErrorNum("CreateNibReference() returned error: ",err,NULL);return false;}
	err = ::CreateWindowFromNib(nibRef,CFSTR("CheckFullKeyboardAccess"),&windowRef);
	if( err != noErr )   {_ACErrorNum("CreateWindowFromNib() returned error: ",err,NULL);return false;}
	::DisposeNibReference(nibRef);
	//ID=1�̃R���g���[���Ƀt�H�[�J�X�ݒ�
	HIViewID	viewid;
	viewid.signature = 0;
	viewid.id = 1;
	HIViewRef	controlRef = NULL;
	err = ::HIViewFindByID(::HIViewGetRoot(windowRef), viewid, &controlRef);
	err = ::SetKeyboardFocus(windowRef,controlRef,kControlFocusNextPart);
	//���̃R���g���[���Ɉړ�
	err = ::HIViewAdvanceFocus(::HIViewGetRoot(windowRef),0);
	HIViewRef	nextControlRef = NULL;
	//���݂̃R���g���[���擾
	err = ::GetKeyboardFocus(windowRef,&nextControlRef);
	//�_�~�[�E�C���h�E�폜
	::DisposeWindow(windowRef);
	sIsFocusGroup2ndPattern =  (controlRef!=nextControlRef);
	if( nextControlRef == 0 )   sIsFocusGroup2ndPattern = false;//#494
	return sIsFocusGroup2ndPattern;
}

ABool	CWindowImp::sIsFocusGroup2ndPattern = false;
#endif

#pragma mark ===========================

#pragma mark ---�E�C���h�E�擾
//win AWindowWrapper����ړ�

/**
�őO�ʃE�C���h�E�擾
*/
AWindowRef	CWindowImp::GetMostFrontWindow()
{
	/*#1034
	if( false )
	{
		return ::FrontNonFloatingWindow();
	}
	else
	*/
	{
		//orderedWindows�擾
		NSArray*	windowsArray = [NSApp orderedWindows];
		if( windowsArray != nil )
		{
			//�e�E�C���h�E�̃��[�v
			NSUInteger	count = [windowsArray count];
			for( AIndex i = 0; i < count; i++ )
			{
				//�E�C���h�E�擾
				NSWindow*	window = [windowsArray objectAtIndex:i];
				if( [window level] != NSFloatingWindowLevel )
				{
					//floating window�ȊO�ŁA�ŏ��Ɍ��������E�C���h�E��windowRef��Ԃ�
					AWindowRef	windowRef = [window windowNumber];//#1275 (AWindowRef)[window windowRef];
					return windowRef;
				}
			}
		}
		return NULL;
	}
}

/**
���̃E�C���h�E�擾
*/
AWindowRef	CWindowImp::GetNextOrderWindow( const AWindowRef inWindowRef )
{
	if( inWindowRef == NULL )   return NULL;
	/*#1034
	if( false )
	{
		return ::GetNextWindow(inWindowRef);
	}
	else
	*/
	{
		//orderedWindows�擾
		NSArray*	windowsArray = [NSApp orderedWindows];
		if( windowsArray != nil )
		{
			//�e�E�C���h�E�̃��[�v
			NSUInteger	count = [windowsArray count];
			for( AIndex i = 0; i < count; i++ )
			{
				//�E�C���h�E�擾
				NSWindow*	window = [windowsArray objectAtIndex:i];
				//�����̃E�C���h�E��windowRef����v������A���̃E�C���h�E���擾���āA����windowRef��Ԃ�
				AWindowRef	windowRef = [window windowNumber];//#1275 (AWindowRef)[window windowRef];
				if( windowRef == inWindowRef )
				{
					if( i+1 < count )
					{
						NSWindow*	win = (NSWindow*)[windowsArray objectAtIndex:i+1];
						return [win windowNumber];//#1275 (AWindowRef)[win windowRef];
					}
				}
			}
		}
		return NULL;
	}
}

/**
N�Ԗڂ̃E�C���h�E�擾
*/
AWindowRef	CWindowImp::GetNthWindow( const ANumber inNumber )
{
	if( inNumber <= 0 )   return NULL;
	/*#1034
	if( false )
	{
		ANumber	num = 0;
		for( AWindowRef windowRef = GetMostFrontWindow(); windowRef != NULL; windowRef = GetNextWindow(windowRef) )
		{
			num++;
			if( num == inNumber )   return windowRef;
		}
		return NULL;
	}
	else
	*/
	{
		//orderedWindows�擾
		NSArray*	windowsArray = [NSApp orderedWindows];
		if( windowsArray != nil )
		{
			//�w��Ԗڂ̃E�C���h�E��windowRef��Ԃ�
			NSUInteger	count = [windowsArray count];
			if( inNumber-1 < count )
			{
				NSWindow*	window = (NSWindow*)[windowsArray objectAtIndex:inNumber-1];
				return [window windowNumber];//#1275 (AWindowRef)[window windowRef];
			}
		}
		return NULL;
	}
}

#pragma mark ===========================

#pragma mark ---�c�����p���W�ϊ�
//#558

/**
�c�����p���W�ϊ��iApp->Imp�j
*/
ALocalPoint	CWindowImp::ConvertVerticalCoordinateFromAppToImp( const AControlID inControlID, const ALocalPoint& inAppPoint ) const
{
	ANumber	width = GetControlWidth(inControlID);
	ALocalPoint	impPoint = {0};
	impPoint.x = width - inAppPoint.y;
	impPoint.y = inAppPoint.x;
	return impPoint;
}

/**
�c�����p���W�ϊ��iApp->Imp�j
*/
ALocalRect	CWindowImp::ConvertVerticalCoordinateFromAppToImp( const AControlID inControlID, const ALocalRect& inAppRect ) const
{
	ANumber	width = GetControlWidth(inControlID);
	ALocalRect	impRect = {0};
	impRect.left	= width - inAppRect.bottom;
	impRect.top		= inAppRect.left;
	impRect.right	= width - inAppRect.top;
	impRect.bottom	= inAppRect.right;
	return impRect;
}

/**
�c�����p���W�ϊ��iImp->App�j
*/
ALocalPoint	CWindowImp::ConvertVerticalCoordinateFromImpToApp( const AControlID inControlID, const ALocalPoint& inImpPoint ) const
{
	ALocalPoint	localPoint = {0};
	localPoint.x = inImpPoint.y;
	localPoint.y = GetControlWidth(inControlID) - inImpPoint.x;
	return localPoint;
}

/**
�c�����p���W�ϊ��iImp->App�j
*/
ALocalRect	CWindowImp::ConvertVerticalCoordinateFromImpToApp( const AControlID inControlID, const ALocalRect& inImpRect ) const
{
	ALocalRect	localRect = {0};
	localRect.left		= inImpRect.top;
	localRect.top		= GetControlWidth(inControlID) - inImpRect.right;
	localRect.right		= inImpRect.bottom;
	localRect.bottom	= GetControlWidth(inControlID) - inImpRect.left;
	return localRect;
}


