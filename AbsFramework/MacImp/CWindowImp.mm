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

#pragma mark --- NSPoint/NSRect変換
//#688

/**
ALocalPoint->NSPoint変換
*/
NSPoint	ConvertFromLocalPointToNSPoint( NSView* inView, const ALocalPoint inLocalPoint ) 
{
	//NSPointへ変換
	NSPoint	viewpoint = {inLocalPoint.x,inLocalPoint.y};
	//flipped==NOの場合、NSPointのy座標は下辺からの距離
	if( [inView isFlipped] == NO )
	{
		viewpoint.y = [inView frame].size.height - inLocalPoint.y;
	}
	return viewpoint;
}

/**
NSPoint->ALocalPoint変換
*/
ALocalPoint	ConvertFromNSPointToLocalPoint( NSView* inView, const NSPoint inViewPoint ) 
{
	//ALocalPointへ変換
	ALocalPoint	lpt = {inViewPoint.x,inViewPoint.y};
	//flipped==NOの場合、NSPointのy座標は下辺からの距離
	if( [inView isFlipped] == NO )
	{
		lpt.y = [inView frame].size.height - inViewPoint.y;
	}
	return lpt;
}

/**
AWindowPoint->NSPoint変換
*/
NSPoint	ConvertFromWindowPointToNSPoint( NSView* inContentView, const AWindowPoint inWindowPoint ) 
{
	NSPoint	windowpoint = {0};
	//NSPointへ変換
	//Window自体のframeはタイトルバーも含んでいるので、Content viewで変換する。
	//Content viewはflipped==NOなので、NSPointのy座標は下辺からの距離
	windowpoint.x = inWindowPoint.x;
	windowpoint.y = [inContentView frame].size.height - inWindowPoint.y;
	return windowpoint;
}

/**
NSPoint->AWindowPoint変換
*/
AWindowPoint	ConvertFromNSPointToWindowPoint( NSView* inContentView, const NSPoint inContentViewPoint ) 
{
	AWindowPoint	wpt = {0};
	//AWindowPointへ変換
	//Window自体のframeはタイトルバーも含んでいるので、Content viewで変換する。
	//Content viewはflipped==NOなので、NSPointのy座標は下辺からの距離
	wpt.x = inContentViewPoint.x;
	wpt.y = [inContentView frame].size.height - inContentViewPoint.y;
	return wpt;
}

/**
AWindowRect-.NSRect変換
*/
NSRect	ConvertFromWindowRectToNSRect( NSView* inContentView, const AWindowRect inWindowRect )
{
	NSRect	windowrect = {0};
	//AWindowRectへ変換
	//Window自体のframeはタイトルバーも含んでいるので、Content viewで変換する。
	//Content viewはflipped==NOなので、NSRectのy座標は下辺からの距離
	windowrect.origin.x = inWindowRect.left;
	windowrect.origin.y = [inContentView frame].size.height - inWindowRect.bottom;
	windowrect.size.width = inWindowRect.right - inWindowRect.left;
	windowrect.size.height = inWindowRect.bottom - inWindowRect.top;
	return windowrect;
}


#pragma mark ===========================
#pragma mark [クラス]CWindowImp
#pragma mark ===========================

//OSからのイベントハンドラコールバック時、対象オブジェクトをポインタから見つける必要がある。
//このときの、ポインタはCWindowImp*であるが、そのポインタの生存をチェックするために、sAliveWindowArrayに、生存中のオブジェクトへのポインタを格納する。
//AWindowは、検索ウインドウやドキュメント付属のウインドウ等、一元管理されないので、
//ポインタではなくObjectID等をイベントハンドラに設定するという方法では、わざわざこのためだけの対応工数が大きすぎるので、このようにした。
//（また、APIの引数がvoid*なので、ポインタ以外のものを渡すのは、将来的にリスクがある。）
AArray<CWindowImp*>	CWindowImp::sAliveWindowArray;

#pragma mark --- コンストラクタ／デストラクタ

/**
コンストラクタ
@param inParent 親オブジェクト(AApplicationオブジェクト)
@param inAWin AWindowオブジェクト
*/
CWindowImp::CWindowImp( AObjectArrayItem* inParent, AWindow& inAWin ) 
	: AObjectArrayItem(inParent), mAWin(inAWin), mModified(false), mWindowStyle(kWindowStyle_Normal), mWindowRef(NULL), mParentWindowRef(NULL),
		mDefaultOKButton(kControlID_Invalid), mDefaultCancelButton(kControlID_Invalid),
		/*#688 フォーカスはOSで管理 mCurrentFocus(kControlID_Invalid), mLatentFocus(kControlID_Invalid), */
		/*#1034 mNavSheet(NULL),*/ mInputBackslashByYenKeyMode(false),
		mHasSheet(false), mParentWindow(NULL)
		,mDragTrackingControl(kControlID_Invalid)//#236
		//#688 ウインドウグループは使用せずCocoaのaddChildWindowで対応 ,mOverlayWindowsGroupRef(NULL) ,mIsOverlayWindow(false)//#291
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
	//#688 Cocoaオブジェクト生成
	CocoaObjects = new CWindowImpCocoaObjects(*this);
	
	//生存ウインドウリストへ追加
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
デストラクタ
*/
CWindowImp::~CWindowImp()
{
	//#688 Cocoaオブジェクト削除
	delete CocoaObjects;
	CocoaObjects = NULL;
	
	//生存ウインドウリストから削除
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
CocoaWindowオブジェクト取得
*/
CocoaWindow*	CWindowImp::GetCocoaWindow()
{
	return CocoaObjects->GetWindow();
}

#pragma mark ===========================

#pragma mark --- CWindowImpオブジェクト検索

//B0000 080810
/**
WindowRefからCWindowImpオブジェクトを検索
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
WindowRefに対応するCWindowImpオブジェクトが存在するかどうかを返す
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
global pointからフローティングウインドウを検索
*/
CWindowImp*	CWindowImp::FindFloatingWindowByGlobalPoint( const AGlobalPoint& inPoint )
{
	//各ウインドウごとのループ
	for( AIndex i = 0; i < sAliveWindowArray.GetItemCount(); i++ )
	{
		CWindowImp*	win = sAliveWindowArray.Get(i);
		if( win->IsWindowCreated() == true )
		{
			if( win->mWindowClass == kWindowClass_Floating && win->IsVisible() == true )
			{
				//表示中のフローティングウインドウの場合は、そのbounds内ならCWindowImp*を返す
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

#pragma mark --- ウインドウ制御

/**
ウインドウ生成（リソースが存在する場合用）

inTabWindow, inTabCountはMacでは使用しない（リソース内で設定）
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
	
	//引数は"main/TextWindow"という形式なので、これをNibファイル名とWindow名に分解して、CFStringRefを生成する
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
		//Cocoa ウインドウ生成
		CocoaObjects->CreateWindow(winName);
		mWindowRef = CocoaObjects->GetWindowRef();
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//TSMドキュメント生成 #688 コンストラクタから移動
		InterfaceTypeList	tsmtype;
		tsmtype[0] = 'udoc';
		OSErr	err = ::NewTSMDocument(1,tsmtype,&mTSMID,(long)this);
		if( err != noErr )   _ACErrorNum("NewTSMDocument() returned error: ",err,this);
		
		AStCreateCFStringFromAText	nibNameRef(nibName);
		AStCreateCFStringFromAText	winNameRef(winName);
		
		//ウインドウ生成
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
		//#674 ドキュメントウインドウには全てwindow groupを作ることにする。
		//（window groupが無いウインドウをSendBehindすると、window groupが存在する全てのウインドウの後ろに移動してしまう問題の対策のため）
		RegisterOverlayWindow(NULL);
		
		//B0385
		HISize	min;
		min.width = 50;
		min.height = 50;
		err = ::SetWindowResizeLimits(mWindowRef,&min,NULL);
		if( err != noErr )   _ACErrorNum("SetWindowResizeLimits() returned error: ",err,this);
		
		//TabViewが存在する場合は1番目のTabに設定する
		AControlRef	tabViewRootControlRef = GetControlRef('ttab',0);
		if( tabViewRootControlRef != NULL )
		{
			::SetControl32BitValue(tabViewRootControlRef,1);
			UpdateTab();
		}
		
	}
#endif
	//ウインドウ最小サイズ設定（ウインドウ生成前に最小サイズ設定された場合は、ここで実ウインドウに設定される） #688
	SetWindowMinimumSize(mMinWidth,mMinHeight);
	SetWindowMaximumSize(mMaxWidth,mMaxHeight);
	//#404 通常状態ウインドウサイズ記憶
	GetWindowBounds(mWindowBoundsInStandardState);
	//#404 フルスクリーン状態解除
	mFullScreenMode = false;
}

/**
ウインドウ生成（リソースが存在しない場合用）
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
	//ウインドウのプロパティを記憶
	mIgnoreClick = inIgnoreClick;
	
	if( true )
	{
		switch(mWindowClass)
		{
		  case kWindowClass_Document:
			{
				//ウインドウ生成
				CocoaObjects->CreateWindow(mWindowClass,nil,inOverlayWindowLayer,
										   inHasCloseButton,inHasCollapseButton,inHasZoomButton,inHasGrowBox,mIgnoreClick,inHasShadow,inHasTitlebar,mNonFloatingFloating);//#1133
				//WindowRef設定
				mWindowRef = CocoaObjects->GetWindowRef();
				break;
			}
		  case kWindowClass_Overlay:
			{
				//親ウインドウのCWindowImp取得
				CWindowImp&	parent = CWindowImp::GetWindowImpByWindowRef(inParentWindowRef);
				//ウインドウ生成（親ウインドウ指定）
				CocoaObjects->CreateWindow(mWindowClass,parent.CocoaObjects->GetWindow(),inOverlayWindowLayer,
										   inHasCloseButton,inHasCollapseButton,inHasZoomButton,inHasGrowBox,mIgnoreClick,inHasShadow,inHasTitlebar,mNonFloatingFloating);//#1133
				//WindowRef設定
				mWindowRef = CocoaObjects->GetWindowRef();
				break;
			}
		  case kWindowClass_Floating:
			{
				//ウインドウ生成
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
		
		//ウインドウ生成
		Rect	bounds;
		bounds.left		= 0;
		bounds.top		= 0;
		bounds.right	= 0;
		bounds.bottom	= 0;
		switch(inWindowClass)//#138
		{
		  case kWindowClass_Document:
			{
				//TSMドキュメント生成 #688 コンストラクタから移動
				InterfaceTypeList	tsmtype;
				tsmtype[0] = 'udoc';
				OSErr	err = ::NewTSMDocument(1,tsmtype,&mTSMID,(long)this);
				if( err != noErr )   _ACErrorNum("NewTSMDocument() returned error: ",err,this);
				
				//
				err = ::CreateNewWindow(kDocumentWindowClass,
							kWindowCloseBoxAttribute|kWindowFullZoomAttribute|kWindowCollapseBoxAttribute|kWindowResizableAttribute|kWindowLiveResizeAttribute/*B0407*/|
							/* kWindowToolbarButtonAttribute|*/kWindowCompositingAttribute|kWindowStandardHandlerAttribute,&bounds,&mWindowRef);
				if( err != noErr )   _ACErrorNum("CreateNewWindow() returned error: ",err,this);
				//#674 ドキュメントウインドウには全てwindow groupを作ることにする。
				//（window groupが無いウインドウをSendBehindすると、window groupが存在する全てのウインドウの後ろに移動してしまう問題の対策のため）
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
				//属性設定
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
				//ウインドウ生成
				err = ::CreateNewWindow(kFloatingWindowClass,attr,&bounds,&mWindowRef);
				if( err != noErr )   _ACErrorNum("CreateNewWindow() returned error: ",err,this);
				break;
			}
		  case kWindowClass_Overlay:
			{
				//属性設定
				WindowAttributes	attr = kWindowCompositingAttribute|kWindowStandardHandlerAttribute;
				//#379
				if( inIgnoreClick == true )
				{
					attr |= kWindowIgnoreClicksAttribute;
				}
				//ウインドウ生成
				err = ::CreateNewWindow(kOverlayWindowClass,attr,&bounds,&mWindowRef);
				if( err != noErr )   _ACErrorNum("CreateNewWindow() returned error: ",err,this);
				/*デフォルトkWindowActivationScopeNone。OverlayのactivationはWindowGroupにより制御する
				err = ::SetWindowActivationScope(mWindowRef,kWindowActivationScopeNone);//
				if( err != noErr )   _ACErrorNum("SetWindowActivationScope() returned error: ",err,NULL);
				*/
				mIsOverlayWindow = true;
				
				//親ウインドウに登録 #291
				if( inParentWindowRef != NULL )
				{
					mOverlayWindowsGroupRef = //#338 子ウインドウでもWindowGroupRefを保持するようにする（先に親削除できるようにする）
							CWindowImp::GetWindowImpByWindowRef(inParentWindowRef).RegisterOverlayWindow(mWindowRef);
					::RetainWindowGroup(mOverlayWindowsGroupRef);//#338 子ウインドウでもWindowGroupRefを保持するようにする
					mOverlayParentWindowRef = inParentWindowRef;//#669 親ウインドウ記憶 マウスダウン時のSelect()に使用する
				}
				else
				{
					_ACError("",this);
				}
				break;
			}
		}
		
		//TabViewが存在する場合は1番目のTabに設定する
		AControlRef	tabViewRootControlRef = GetControlRef('ttab',0);
		if( tabViewRootControlRef != NULL )
		{
			::SetControl32BitValue(tabViewRootControlRef,1);
			UpdateTab();
		}
	}
#endif
	
	//ウインドウ最小サイズ設定（ウインドウ生成前に最小サイズ設定された場合は、ここで実ウインドウに設定される） #688
	SetWindowMinimumSize(mMinWidth,mMinHeight);
	SetWindowMaximumSize(mMaxWidth,mMaxHeight);
	//#404 通常状態ウインドウサイズ記憶
	GetWindowBounds(mWindowBoundsInStandardState);
	//#404 フルスクリーン状態解除
	mFullScreenMode = false;
}

/*
HIWindowChangeClass()によるOverlayクラスへの変更はできない？
void	CWindowImp::ChangeWindowClass( const AWindowClass inWindowClass, 
		const ABool inHasCloseButton, const ABool inHasCollapseButton, const ABool inHasZoomButton, const ABool inHasGrowBox )
{
	OSStatus	err = noErr;
	
	//ウインドウ生成済みチェック
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
ウインドウ削除
*/
void	CWindowImp::CloseWindow()
{
	//fprintf(stderr,"CWindowImp::CloseWindow() - start\n");
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//シートウインドウの場合、親ウインドウのmHasSheetフラグをOFFにする
	if( kWindowStyle_Sheet == mWindowStyle && mParentWindow != NULL )
	{
		if( sAliveWindowArray.Find(mParentWindow) != kIndex_Invalid )
		{
			mParentWindow->SetHasSheet(false);
		}
	}
	
	/*#291
	//子ウインドウクローズ
	if( mChildSheetWindow.GetItemCount() > 0 )
	{
		CloseChildWindow();
	}
	*/
	
	if( true )
	{
		//Cocoaウインドウの場合
		
		//ウインドウ削除
		CocoaObjects->DeleteWindow();
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//イベントハンドラアンインストール
		UninstallEventHandler();
		
		//ウインドウ削除
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
	
	//WindowRefをNULLに設定
	mWindowRef = NULL;
	
	//TextArrayメニューの登録解除
	while( mTextArrayMenuArray_ControlID.GetItemCount() > 0 )
	{
		UnregisterTextArrayMenu(mTextArrayMenuArray_ControlID.Get(0));
	}
	
	//キーボードフォーカスグループ削除
	//#135 mFocusGroup.DeleteAll();
	
	/*#688
	//フォーカス設定解除
	mLatentFocus = kControlID_Invalid;
	mCurrentFocus = kControlID_Invalid;
	*/
	//ラジオグループ削除
	//#349 mRadioGroup.DeleteAll();
	
	//UserPane削除
	mUserPaneArray.DeleteAll();
	mUserPaneArray_ControlID.DeleteAll();
	mUserPaneArray_Type.DeleteAll();
	//#688 mUserPaneArray_ControlRef.DeleteAll();//#688
	
#if SUPPORT_CARBON
	//WindowGroup削除 #291
	if( mOverlayWindowsGroupRef != NULL )
	{
		//::DebugPrintWindowGroup(mOverlayWindowsGroupRef);
		::ReleaseWindowGroup(mOverlayWindowsGroupRef);//Group内のそれぞれのウインドウでretainしている。全てがreleaseしたら解放。
		mOverlayWindowsGroupRef = NULL;
	}
	//#669 Overlay親ウインドウの記憶消去
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
WindowをDrawerに設定
@param inParent 親ウインドウ
*/
void	CWindowImp::SetWindowStyleToDrawer( CWindowImp& inParent )
{
	OSStatus	err = noErr;
	
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//ウインドウスタイルと親ウインドウRefを設定してShow()
	mWindowStyle = kWindowStyle_Drawer;
	mParentWindowRef = inParent.GetWindowRef();
	mParentWindow = &inParent;//B0367
	err = ::SetDrawerParent(mWindowRef,mParentWindowRef);
	if( err != noErr )   _ACErrorNum("SetDrawerParent() returned error: ",err,this);
}
#endif

/**
WindowをSheetに設定
@param inParent 親ウインドウ
*/
void	CWindowImp::SetWindowStyleToSheet( CWindowImp& inParent )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//ウインドウスタイルと親ウインドウRefを設定
	mWindowStyle = kWindowStyle_Sheet;
	mParentWindowRef = inParent.GetWindowRef();
	mParentWindow = &inParent;
	mParentWindow->SetHasSheet(true);
}

/**
ツールバー生成
*/
void	CWindowImp::CreateToolbar( const AIndex inType )
{
	//ウインドウ生成済みチェック
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
ウインドウ表示
*/
void	CWindowImp::Show()
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( true )
	{
		//Cocoaウインドウの場合
		
		//親ウインドウに子として登録（子の状態でorderOutすると親もorderOutするため、Hide()時、親子状態解除している）
		NSWindow*	parent = CocoaObjects->GetParentWindow();
		if( parent != nil )
		{
			[parent addChildWindow:(CocoaObjects->GetWindow()) ordered:NSWindowAbove];
		}
		
		switch(mWindowStyle)
		{
			//Sheetウインドウの場合
		  case kWindowStyle_Sheet:
			{
				if( sAliveWindowArray.Find(mParentWindow) != kIndex_Invalid )
				{
					//シートウインドウ表示
					[NSApp beginSheet:CocoaObjects->GetWindow() 
					modalForWindow:mParentWindow->CocoaObjects->GetWindow() 
					modalDelegate:CocoaObjects->GetWindow() 
					didEndSelector:@selector(didEndSheet:returnCode:contextInfo:) 
					contextInfo:nil];
				}
				break;
			}
			//通常ウインドウ
		  case kWindowStyle_Normal:
		  default:
			{
				//最前面に表示
				[CocoaObjects->GetWindow() orderFront:nil];
				break;
			}
		}
		/*
		//#688
		//キャレットオーバーレイウインドウ取得（生成）
		NSWindow*	caretWindow = CocoaObjects->GetCaretOverlayWindow();
		if( caretWindow != nil )
		{
			//キャレットオーバーレイウインドウを子として配置
			[CocoaObjects->GetWindow() addChildWindow:caretWindow ordered:NSWindowAbove];
			//キャレットオーバーレイウインドウ表示
			[caretWindow orderFront:nil];
		}
		*/
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		OSStatus	err = noErr;
		
		//イベントハンドラインストール済みでなければここでインストール
		if( mInstalledEventHandler.GetItemCount() == 0 )
		{
			InstallEventHandler();
		}
		
		//
		UpdateTab();
		//ウインドウ表示
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
			//通常ウインドウ
		  case kWindowStyle_Normal:
		  default:
			{
				::ShowWindow(mWindowRef);
				break;
			}
		}
		/*#370 ここで位置補正すると左端でConstrainしたときにGetWindowBounds()の値と実際の表示位置が合わなくなる問題が発生したのでやめる
		//ウインドウ位置補正
		err = ::ConstrainWindowToScreen(mWindowRef,kWindowContentRgn,kWindowConstrainAllowPartial|kWindowConstrainMoveRegardlessOfFit,NULL,NULL);
		if( err != noErr )   _ACErrorNum("ConstrainWindowToScreen() returned error: ",err,this);
		err = ::ConstrainWindowToScreen(mWindowRef,kWindowTitleBarRgn,kWindowConstrainAllowPartial|kWindowConstrainMoveRegardlessOfFit,NULL,NULL);
		if( err != noErr )   _ACErrorNum("ConstrainWindowToScreen() returned error: ",err,this);
		*/
	}
#endif
}

/**
ウインドウ非表示
*/
void	CWindowImp::Hide()
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//フォーカスクリア
	ClearFocus();
	
	if( true )
	{
		//Cocoaウインドウの場合
		
		//親ウインドウに子として登録（子の状態でorderOutすると親もorderOutするため）
		NSWindow*	parent = CocoaObjects->GetParentWindow();
		if( parent != nil )
		{
			[parent removeChildWindow:(CocoaObjects->GetWindow())];
		}
		
		switch(mWindowStyle)
		{
			//Sheetウインドウの場合
		  case kWindowStyle_Sheet:
			{
				//シート終了
				[NSApp endSheet:CocoaObjects->GetWindow()];
				mParentWindowRef = NULL;
				break;
			}
			//通常ウインドウの場合
		  case kWindowStyle_Normal:
		  default:
			{
				//ウインドウhide
				[CocoaObjects->GetWindow() orderOut:nil];
				break;
			}
		}
		/*
		//#688
		//キャレットオーバーレイウインドウ取得
		NSWindow*	caretWindow = CocoaObjects->GetCaretOverlayWindow();
		if( caretWindow != nil )
		{
			//キャレットオーバーレイウインドウを子登録解除
			[CocoaObjects->GetWindow() removeChildWindow:caretWindow];
			//キャレットオーバーレイウインドウhide
			[caretWindow orderOut:nil];
		}
		*/
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		OSStatus	err = noErr;
		
		//ウインドウ非表示
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
			//通常ウインドウ
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
ウインドウ選択
*/
void	CWindowImp::Select()
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//#240 ウインドウ選択直前のイベント（選択処理からActivatedがコールされるまで若干の処理時間がかかるので表示の都合上先にやっておきたい処理（フローティングウインドウHide等）をここで行う）
	GetAWin().EVT_DoWindowBeforeSelected();
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		if( [CocoaObjects->GetWindow() canBecomeMainWindow] == YES )
		{
			//ウインドウ最前面にする。
			//#1034 次のmakeKeyAndOrderFrontだけだと、becomeKeyWindowコールバック時、
			//[NSApp orderedWindows]でまだこのウインドウが最前面になっておらず、
			//AApp().SPI_UpdateFloatingSubWindowsVisibility()により、フローティングウインドウが表示されない問題が発生するので、
			//先にorderFrontだけ実行して、ウインドウを最前面にする。
			[CocoaObjects->GetWindow() orderFront:nil];
			//ウインドウ最前面、かつ、キーフォーカス対象にする。
			[CocoaObjects->GetWindow() makeKeyAndOrderFront:nil];
			//子オーバーレイウインドウのz-order再配置
			ReorderOverlayChildWindows();
		}
		else
		{
			//キーフォーカス対象にする
			[CocoaObjects->GetWindow() makeKeyWindow];
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//ウインドウ選択
		::SelectWindow(mWindowRef);
	}
#endif
}

/**
ウインドウを別のウインドウの背後に移動
*/
void	CWindowImp::SendBehind( CWindowImp& inBeforeWindow )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//ウインドウを別のウインドウの背後に移動
	if( true )
	{
		//Cocoaウインドウの場合
		
		//★未テスト
		if( sAliveWindowArray.Find(&inBeforeWindow) != kIndex_Invalid )
		{
			[CocoaObjects->GetWindow() orderWindow:NSWindowBelow 
			relativeTo:[inBeforeWindow.CocoaObjects->GetWindow() windowNumber]];
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		::SendBehind(mWindowRef,inBeforeWindow.GetWindowRef());
	}
#endif
}

//#567
/**
ウインドウを最背面に移動
*/
void	CWindowImp::SendBehindAll()
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//ウインドウを別のウインドウの背後に移動
	if( true )
	{
		//Cocoaウインドウの場合
		
		//★未テスト
		[CocoaObjects->GetWindow() orderWindow:NSWindowBelow relativeTo:0];
	}
	/*#1034
	else
	{
		//Carbonウインドウの場合
		
		::SendBehind(mWindowRef,NULL);
	}
	*/
}

//#688
/**
ウインドウを別のウインドウの前面に移動
*/
void	CWindowImp::SendAbove( CWindowImp& inBeforeWindow )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//★未テスト
	//ウインドウを別のウインドウの前面に移動
	if( sAliveWindowArray.Find(&inBeforeWindow) != kIndex_Invalid )
	{
		[CocoaObjects->GetWindow() orderWindow:NSWindowAbove 
		relativeTo:[inBeforeWindow.CocoaObjects->GetWindow() windowNumber]];
	}
}

//#688
/**
ウインドウを最背面に移動
*/
void	CWindowImp::SendAboveAll()
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//★未テスト
	//ウインドウを他の全てのウインドウの前面に移動
	[CocoaObjects->GetWindow() orderWindow:NSWindowAbove relativeTo:0];
}

//#688
/**
子オーバーレイウインドウのzorderを再配置
*/
void	CWindowImp::ReorderOverlayChildWindows()
{
	[CocoaObjects->GetWindow() reorderOverlayChildWindows];
}

/**
ウインドウリフレッシュ
*/
void	CWindowImp::RefreshWindow() const
{
	if( true )
	{
		//強制再描画
		[CocoaObjects->GetWindow() display];
	}
#if SUPPORT_CARBON
	else
	{
		::Draw1Control(::HIViewGetRoot(mWindowRef));
		//InvalWindowRect()やHIViewSetNeedsDisplay()やDrawControls()ではredrawされないので、Draw1Controlを使用。
	}
#endif
}

#if SUPPORT_CARBON
/**
指定WindowRectを描画更新
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
	//OSX10.3ではHIViewSetNeedsDisplayInRectが使えないので、下記に変更
	//フレームサイズでトリミングしておかないとHIViewSetNeedsDisplayInRegionのほうはうまくいかないことがある
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
Print中かどうかを返す

CPrintImpにてWindowPropertyが設定される
*/
ABool	CWindowImp::IsPrintMode() const
{
	if( true )
	{
		//ひとまず印刷はモーダルダイアログでの対応とする
		//シートウインドウでの印刷に対応する場合は、何らかの機構が必要
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
WindowをDock化する
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
ウインドウをDrag（Control内でマウスクリック＆ドラッグした場合用）
*/
void	CWindowImp::DragWindow( const AControlID inControlID, const ALocalPoint& inMouseLocalPoint )
{
	if( true )
	{
		//★未対応
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
オーバーレイウインドウを登録する（親側でコール）
@param inChildWindow 登録する子ウインドウ（NULLならWindowGroupのみ生成。子は登録しない）#674
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
	if( inChildWindow != NULL )//#674 inChildWindowがNULLの場合はwindow groupのみ生成
	{
		err = ::SetWindowGroup(inChildWindow,mOverlayWindowsGroupRef);
		if( err != noErr )   _ACErrorNum("SetWindowGroup() returned error: ",err,NULL);
	}
	return mOverlayWindowsGroupRef;//#338
}
#endif

//#291
/**
自分（オーバーレイウインドウ）が属するWindowGroupのMoveTogether属性をOn/Offする（子側でコール）
*/
void	CWindowImp::SetOverlayWindowMoveTogether( const ABool inMoveTogether )
{
	if( true )
	{
		//処理なし
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
自分（オーバーレイウインドウ）が属するWindowGroupのkWindowGroupAttrLayerTogether属性をOn/Offする（子側でコール）
*/
void	CWindowImp::SetOverlayLayerTogether( const ABool inLayerTogether )
{
	if( true )
	{
		//処理無し
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
ウインドウ内paint rect
*/
void	CWindowImp::PaintRect( const AWindowRect& inRect, const AColor& inColor )
{
	//★未対応
}

//#409
/**
ウインドウを即時更新する
@note 描画すべき領域があれば描画＋ウインドウflush
*/
void	CWindowImp::UpdateWindow()
{
	//fprintf(stderr,"%16X UpdateWindow() - Start\n",(int)(GetObjectID().val));
	
	if( true )
	{
		//描画すべき領域があれば描画
		[CocoaObjects->GetWindow() displayIfNeeded];
		//ウインドウflush
		[CocoaObjects->GetWindow() flushWindowIfNeeded];
		/*
		//キャレットオーバーレイウインドウもflush
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
		//#420 update領域のDoDrawを実行する
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
フルスクリーンモード
*/
void	CWindowImp::SetFullScreenMode( const ABool inFullScreenMode )
{
	if( true )
	{
		//★未対応
	}
#if SUPPORT_CARBON
	else
	{
		if( inFullScreenMode == true && mFullScreenMode == false )
		{
			//ウインドウ領域記憶
			GetWindowBounds(mWindowBoundsInStandardState);
			//フルスクリーン設定
			::SetSystemUIMode(kUIModeAllSuppressed,0);
			::ChangeWindowAttributes(mWindowRef,kWindowNoTitleBarAttribute,0);
			//#476 フルスクリーン時にウインドウサイズを記憶しないように先にフルスクリーンモード設定
			mFullScreenMode = inFullScreenMode;
			//ウインドウ領域設定
			GDHandle	mainMonitor = ::GetMainDevice();
			Rect	rect = (*mainMonitor)->gdRect;
			SetWindowBounds(rect);
		}
		else if( inFullScreenMode == false && mFullScreenMode == true )
		{
			//フルスクリーン解除
			::SetSystemUIMode(kUIModeNormal,0);
			::ChangeWindowAttributes(mWindowRef,0,kWindowNoTitleBarAttribute);
			//#476 フルスクリーン時にウインドウサイズを記憶しないように先にフルスクリーンモード設定
			mFullScreenMode = inFullScreenMode;
			//ウインドウ領域を元に戻す
			SetWindowBounds(mWindowBoundsInStandardState);
		}
		//
		//#476 mFullScreenMode = inFullScreenMode;
	}
#endif
}

//#505
/**
ウインドウの最小サイズ設定
*/
void	CWindowImp::SetWindowMinimumSize( const ANumber inWidth, const ANumber inHeight )
{
	if( true )
	{
		//最小サイズデータ設定
		mMinWidth = inWidth;
		mMinHeight = inHeight;
		//ウインドウ生成済みなら実ウインドウに設定
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
ウインドウの最小サイズ設定
*/
void	CWindowImp::SetWindowMaximumSize( const ANumber inWidth, const ANumber inHeight )
{
	//最小サイズデータ設定
	mMaxWidth = inWidth;
	mMaxHeight = inHeight;
	//ウインドウ生成済みなら実ウインドウに設定
	if( IsWindowCreated() == true )
	{
		NSSize	size = {inWidth,inHeight};
		[CocoaObjects->GetWindow() setContentMaxSize:size];
	}
}

/**
クラス：フローティングウインドウに変更
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
			//クラス変更
			mWindowClass = kWindowClass_Floating;
			//#1133
			//フローティングウインドウの設定
			CocoaObjects->SetupFloatingWindow(mNonFloatingFloating,mIgnoreClick);
			/*#1133
			//フローティングパネル設定
			[CocoaObjects->GetWindow() setFloatingPanel:YES];
			//ウインドウレベル設定 #1034
			[CocoaObjects->GetWindow() setLevel:NSFloatingWindowLevel];
			//シャドウ表示する
			[CocoaObjects->GetWindow() setHasShadow:YES];
			//ウインドウdeactivate時、hideする
			[CocoaObjects->GetWindow() setHidesOnDeactivate:YES];
			*/
			//親ウインドウからの登録解除
			//if( CWindowImp::GetWindowImpByWindowRef(inParentWindowRef).IsVisible() == true )
			{
				NSWindow*	parent = CocoaObjects->GetParentWindow();
				if( parent != nil )
				{
					[parent removeChildWindow:(CocoaObjects->GetWindow())];
				}
			}
			//親ウインドウへのリンクを開放
			CocoaObjects->ClearParentWindow();
			break;
		}
	  case kWindowClass_Floating:
		{
			//処理なし
			break;
		}
	}
}

/**
クラス：オーバーレイウインドウに変更
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
			//処理なし
			break;
		}
	  case kWindowClass_Floating:
		{
			//クラス設定
			mWindowClass = kWindowClass_Overlay;
			//フローティングパネル設定
			[CocoaObjects->GetWindow() setFloatingPanel:NO];
			//ウインドウレベル設定 #1034
			[CocoaObjects->GetWindow() setLevel:NSNormalWindowLevel];
			//シャドウ表示する
			[CocoaObjects->GetWindow() setHasShadow:NO];
			//ウインドウdeactivate時、hideしない
			[CocoaObjects->GetWindow() setHidesOnDeactivate:NO];
			//親ウインドウへのリンクを設定
			CocoaObjects->SetParentWindow(CWindowImp::GetWindowImpByWindowRef(inParentWindowRef).CocoaObjects->GetWindow());
			//親ウインドウへの登録
			if( CWindowImp::GetWindowImpByWindowRef(inParentWindowRef).IsVisible() == true )
			{
				NSWindow*	parent = CocoaObjects->GetParentWindow();
				if( parent != nil )
				{
					[parent addChildWindow:(CocoaObjects->GetWindow()) ordered:NSWindowAbove];
					//親ウインドウの各子ウインドウのz-order調整
					[parent reorderOverlayChildWindows];
				}
			}
			break;
		}
	}
}

/**
このウインドウでモーダルセッション開始
*/
void	CWindowImp::StartModalSession()
{
	CocoaObjects->StartModalSession();
}

/**
このウインドウのモーダルセッション継続チェック
*/
ABool	CWindowImp::CheckContinueingModalSession()
{
	return CocoaObjects->CheckContinueingModalSession();
}

/**
このウインドウのモーダルセッション終了
*/
void	CWindowImp::EndModalSession()
{
	CocoaObjects->EndModalSession();
}

/**
このウインドウのモーダルセッション中断
*/
void	CWindowImp::StopModalSession()
{
	CocoaObjects->StopModalSession();
}

/**
モーダルウインドウループ開始（ウインドウ内のイベント処理でStopModalSessionがコールされるまで終了しない）
*/
void	CWindowImp::RunModalWindow()
{
	CocoaObjects->RunModalWindow();
}

/**
ウインドウサイズをマーカー位置に合わせて設定
*/
void	CWindowImp::OptimizeWindowBounds( const AControlID inBottomMarker, const AControlID inRightMarker )
{
	NSSize	origMinSize = [CocoaObjects->GetWindow() contentMinSize];
	NSSize	origMaxSize = [CocoaObjects->GetWindow() contentMaxSize];
	//bottom markerに合わせて設定
	if( inBottomMarker != kControlID_Invalid )
	{
		NSView*	bottomMarker = CocoaObjects->FindViewByTag(inBottomMarker);
		if( bottomMarker != nil )
		{
			//一旦min/maxを解除
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
	//right markerに合わせて設定
	if( inRightMarker != kControlID_Invalid )
	{
		NSView*	rightMarker = CocoaObjects->FindViewByTag(inRightMarker);
		if( rightMarker != nil )
		{
			//一旦min/maxを解除
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
ツールバー表示／非表示
*/
void	CWindowImp::ShowHideToolbar()
{
	[CocoaObjects->GetWindow() toggleToolbarShown:nil];
}

/**
ツールバーカスタマイズ画面表示
*/
void	CWindowImp::ShowToolbarCustomize()
{
	[CocoaObjects->GetWindow() runToolbarCustomizationPalette:nil];
}

/**
ツールバー表示／非表示状態取得
*/
ABool	CWindowImp::IsToolBarShown() const
{
	return ([[CocoaObjects->GetWindow() toolbar] isVisible] == YES);
}

//#688
/**
フォントパネル表示
*/
void	CWindowImp::ShowFontPanel( const AControlID inVirtualControlID, const AText& inFontName, const AFloatNumber inFontSize )
{
	//フォントパネルのvirtual control idを設定
	mFontPanelVirtualControlID = inVirtualControlID;
	//フォント名取得
	AText	fontname;
	fontname.SetText(inFontName);
	if( AFontWrapper::IsFontEnabled(fontname) == false )
	{
		AFontWrapper::GetAppDefaultFontName(fontname);
	}
	//指定フォントをフォントパネルに設定
	AStCreateNSStringFromAText	str(fontname);
	NSFont*	font = [NSFont fontWithName:str.GetNSString() size:inFontSize];
	[CocoaObjects->GetWindow() showFontPanel:font];
}

//#688
/**
フルスクリーン状態かどうかを判定
*/
ABool	CWindowImp::IsFullScreen() const
{
	return (([CocoaObjects->GetWindow() styleMask]&(1<<14))!=0);
}

#pragma mark ===========================

#pragma mark --- ウインドウ情報Set

/**
ウインドウタイトル設定
*/
void	CWindowImp::SetTitle( const AText& inText )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//タイトル設定
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
ウインドウ透過率設定
*/
void	CWindowImp::SetWindowTransparency( const AFloatNumber inTransparency )
{
	//MacOSXの場合、オーバーレイウインドウの場合は、AView_List等で塗りつぶすときに透明度塗りつぶしを実行する。
	//そのため、ウインドウ全体の透明度設定は行わない。
	//#1255 if( mWindowClass == kWindowClass_Overlay )   return;//win
	
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//透過率設定
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
ウインドウ編集状態設定
*/
void	CWindowImp::SetModified( const ABool inModified )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( true )
	{
		//Modified状態にする（Closeボタンに黒丸表示）
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
		
		//Modified状態にする（Closeボタンに黒丸表示）
		mModified = inModified;
		err = ::SetWindowModified(mWindowRef,mModified);
		if( err != noErr )   _ACErrorNum("SetWindowModified() returned error: ",err,this);
	}
#endif
}

/**
ウインドウに対応する対応ファイル設定（タイトルバーにアイコンが表示される）
*/
void	CWindowImp::SetWindowFile( const AFileAcc& inFile )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( true )
	{
		//ファイルパスを設定
		AText	path;
		inFile.GetPath(path);
		//#932 ファイルアクセス頻度を減らすため、最後に設定したファイルと異なる場合のみ設定する
		if( mCurrentWindowFilePath.Compare(path) == false )
		{
			AStCreateNSStringFromAText	str(path);
			[CocoaObjects->GetWindow() setRepresentedFilename:str.GetNSString()];
			//#932 最後に設定したファイルを記憶
			mCurrentWindowFilePath.SetText(path);
		}
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		/*OSX10.3で使用不可
		FSRef	fsref;
		if( inFile.GetFSRef(fsref) == true )
		{
		::HIWindowSetProxyFSRef(mWindowRef,&fsref);
		}
		*/
		//#557 Mac OS X 10.7だとSetWindowProxyFSSpec()は動作不安定になる。暫定対策。
		//"defaults write net.mimikaki.mi ApplePersistenceIgnoreState YES"でも対策できるが、テスト用とのことなので、とりあえずやめておく。
		/*#557if( AEnvWrapper::GetOSVersion() >= 0x1070 )
		{
		//処理無し
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
ウインドウに対応する対応ファイル設定
*/
void	CWindowImp::ClearWindowFile()
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( true )
	{
		//空の文字列を設定
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
デフォルトOKボタン（リターンキーでクリックと見なされるボタン）を設定
*/
void	CWindowImp::SetDefaultOKButton( const AControlID inID )
{
	mDefaultOKButton = inID;
	//#688
	//実際には、mDefaultOKButtonは使用しなくなりました。
	//Interface Builderでkey equivalentの設定をすること。
}

/**
デフォルトCancelボタン（リターンキーでクリックと見なされるボタン）を設定
*/
void	CWindowImp::SetDefaultCancelButton( const AControlID inID )
{
	mDefaultCancelButton = inID;
}

#pragma mark ===========================

#pragma mark --- ウインドウ情報Get

/**
ウインドウタイトル取得
*/
void	CWindowImp::GetTitle( AText& outText ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( true )
	{
		//タイトル取得
		ACocoa::SetTextFromNSString([CocoaObjects->GetWindow() title],outText);
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//タイトル取得
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
ウインドウ生成済みかどうかを取得
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
ウインドウ表示中かどうかを取得
*/
ABool	CWindowImp::IsVisible() const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false )   return false;
	
	if( true )
	{
		//ウインドウ表示中かどうかを取得
		return ([CocoaObjects->GetWindow() isVisible]==YES);
	}
#if SUPPORT_CARBON
	else
	{
		//ウインドウ表示中かどうかを取得
		return ::IsWindowVisible(mWindowRef);
	}
#endif
}

/**
ウインドウがアクティブかどうかを取得
*/
ABool	CWindowImp::IsActive() const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false )   return false;
	
	//フローティングなら常にactiveを返す
	if( IsFloatingWindow() == true )
	{
		return true;
	}
	
	//
	if( true )
	{
		//ウインドウがMain Windowかどうかを取得
		return ([CocoaObjects->GetWindow() isMainWindow]==YES);
	}
#if SUPPORT_CARBON
	else
	{
		//ウインドウがアクティブかどうかを取得
		return ::IsWindowActive(mWindowRef);
	}
#endif
}

/**
ウインドウがFocusされているかどうか（＝key windowかどうか）を返す。
@note key windowかどうかを返す
*/
ABool	CWindowImp::IsFocusActive() const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false )   return false;
	
	//ウインドウがKey Windowかどうかを取得
	return ([CocoaObjects->GetWindow() isKeyWindow]==YES);
}

/**
ウインドウがクローズボタンを持っているかどうかを取得
*/
ABool	CWindowImp::HasCloseButton() const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//★未テスト
	if( true )
	{
		return ( ([CocoaObjects->GetWindow() styleMask] & NSClosableWindowMask) != 0 );
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//ウインドウがクローズボタンを持っているかどうかを取得
		WindowAttributes	attr;
		err = ::GetWindowAttributes(mWindowRef,&attr);
		if( err != noErr )   _ACErrorNum("GetWindowAttributes() returned error: ",err,this);
		return ((attr&kWindowCloseBoxAttribute)!=0);
	}
#endif
}

/**
フローティングウインドウかどうか
*/
ABool	CWindowImp::IsFloatingWindow() const
{
	//#1133
	//non-floatingウインドウフラグonなら常にfalseを返す
	if( mNonFloatingFloating == true )
	{
		return false;
	}
	
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	if( true )
	{
		return (mWindowClass==kWindowClass_Floating);
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//フローティングウインドウかどうか
		WindowClass	windowClass;
		err = ::GetWindowClass(mWindowRef,&windowClass);
		if( err != noErr )   _ACErrorNum("GetWindowClass() returned error: ",err,this);
		return (windowClass == kFloatingWindowClass)||(windowClass == kToolbarWindowClass);//#138 kToolbarWindowClassを追加
	}
#endif
}

/**
タイトルバーが存在するかどうかを取得
*/
ABool	CWindowImp::HasTitleBar() const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//★未テスト
	if( true )
	{
		return ( ([CocoaObjects->GetWindow() styleMask] & NSTitledWindowMask) != 0 );
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//ウインドウ属性取得
		WindowAttributes	attr;
		err = ::GetWindowAttributes(mWindowRef,&attr);
		if( err != noErr )   _ACErrorNum("GetWindowAttributes() returned error: ",err,this);
		return ((attr&kWindowNoTitleBarAttribute)==0);
	}
#endif
}

/**
Modalかどうかを取得
*/
ABool	CWindowImp::IsModalWindow() const 
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	if( true )
	{
		//CWindowImpでmodal windowは作成しないので、falseを返す。
		return false;
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//Modal属性取得
		WindowModality	modality;
		WindowRef	w;
		err = ::GetWindowModality(mWindowRef,&modality,&w);
		if( err != noErr )   _ACErrorNum("GetWindowModality() returned error: ",err,this);
		return (modality == kWindowModalitySystemModal || modality == kWindowModalityAppModal);
	}
#endif
}

/**
Resize可能かどうかを取得
*/
ABool	CWindowImp::IsResizable() const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//★未テスト
	if( true )
	{
		return ( ([CocoaObjects->GetWindow() styleMask] & NSResizableWindowMask) != 0 );
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//ウインドウ属性取得
		WindowAttributes	attr;
		err = ::GetWindowAttributes(mWindowRef,&attr);
		if( err != noErr )   _ACErrorNum("GetWindowAttributes() returned error: ",err,this);
		return ((attr&kWindowResizableAttribute)!=0);
	}
#endif
}

/**
Zoom可能かどうかを取得
*/
ABool	CWindowImp::IsZoomable() const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//
	if( true )
	{
		//★未実装
		return true;
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//ウインドウ属性取得
		WindowAttributes	attr;
		err = ::GetWindowAttributes(mWindowRef,&attr);
		if( err != noErr )   _ACErrorNum("GetWindowAttributes() returned error: ",err,this);
		return ((attr&kWindowHorizontalZoomAttribute)!=0 || (attr&kWindowVerticalZoomAttribute)!=0);
	}
#endif
}

/**
Zoom状態かどうかを取得
*/
ABool	CWindowImp::IsZoomed() const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//★未テスト
	if( true )
	{
		//zoom状態取得
		return ([CocoaObjects->GetWindow() isZoomed]==YES);
	}
#if SUPPORT_CARBON
	else
	{
		//ウインドウ属性取得
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
WindowのContent部分に対応するルートのHIViewを取得する
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
	//★未テスト
	if( true )
	{
		//miniaturized状態取得
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
WindowRef取得
*/
AWindowRef	CWindowImp::GetWindowRef() const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACThrow("Window not created",this);return NULL;}
	//WindowRef取得
	return mWindowRef;
}

#pragma mark ===========================

#pragma mark --- ウインドウサイズ／位置Get/Set

#if SUPPORT_CARBON
/**
ウインドウ領域取得
*/
void	CWindowImp::GetWindowBounds( Rect &outRect ) const
{
	OSStatus	err = noErr;
	
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//ウインドウBounds取得
	err = ::GetWindowBounds(mWindowRef,kWindowContentRgn,&outRect);
	if( err != noErr )   _ACErrorNum("GetWindowBounds() returned error: ",err,this);
}
#endif

/**
ウインドウ領域取得
*/
void	CWindowImp::GetWindowBounds( ARect& outRect ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//ウインドウBounds取得
	if( true )
	{
		NSWindow*	win = CocoaObjects->GetWindow();
		//Content viewのframeを取得
		NSRect	contentFrame = [win contentRectForFrameRect:[win frame]];
		//screenのframeを取得
		NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];//(0,0)のあるスクリーンのフレーム
		//screenの左下が原点なのを考慮して変換
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
ウインドウ領域取得
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
ウインドウ領域設定
*/
void	CWindowImp::SetWindowBounds( Rect &inRect )
{
	/*#688
	OSStatus	err = noErr;
	
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//ウインドウBounds設定
	err = ::SetWindowBounds(mWindowRef,kWindowContentRgn,&inRect);
	if( err != noErr )   _ACErrorNum("SetWindowBounds() returned error: ",err,this);
	//#476 フルスクリーンモードでないときのウインドウサイズを記憶
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
ウインドウ領域設定
*/
void	CWindowImp::SetWindowBounds( const ARect& inRect, const ABool inAnimate )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	if( true )
	{
		ARect	rect = inRect;
		//最小サイズ未満なら最小サイズにする
		if( rect.right - rect.left < mMinWidth )
		{
			rect.right = rect.left + mMinWidth;
		}
		if( rect.bottom - rect.top < mMinHeight )
		{
			rect.bottom = rect.top + mMinHeight;
		}
		//最大サイズ以上なら最大サイズにする
		if( rect.right - rect.left > mMaxWidth )
		{
			rect.right = rect.left + mMaxWidth;
		}
		if( rect.bottom - rect.top > mMaxHeight )
		{
			rect.bottom = rect.top + mMaxHeight;
		}
		//screenのframeを取得
		NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];//(0,0)のあるスクリーンのフレーム
		//screenの左下が原点なのを考慮して変換
		NSRect	frame = {0};
		frame.origin.x = rect.left;
		frame.origin.y = firstScreenFrame.size.height - rect.bottom;
		frame.size.width 	= rect.right - rect.left;
		frame.size.height 	= rect.bottom - rect.top;
		//frame設定
		NSWindow*	win = CocoaObjects->GetWindow();
		/*サイドバー水平区切りドラッグでウインドウサイズがかわらないので、とりあえずコメントアウト
		//
		NSRect	contentFrame = [win contentRectForFrameRect:[win frame]];
		if( contentFrame.origin.x == frame.origin.x && contentFrame.origin.y == frame.origin.y &&
					contentFrame.size.width == frame.size.width && contentFrame.size.height && frame.size.height)
		{
			return;
		}
		*/
		//ウインドウが表示中かどうかを取得 #1090
		BOOL	visible = [win isVisible];
		//子ウインドウの場合は、親ウインドウが非表示中なら非表示とみなす
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
			[win setFrame:[win frameRectForContentRect:frame] display:visible];//#1090 ウインドウ非表示中ならdisplay:NOとする
		}
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//ウインドウBounds設定
		Rect	rect;
		rect.left	= inRect.left;
		rect.right	= inRect.right;
		if( rect.right <= rect.left )   rect.right = rect.left+1;//#212 エラー回避補正
		rect.top	= inRect.top;
		rect.bottom	= inRect.bottom;
		if( rect.bottom <= rect.top )   rect.bottom = rect.top+1;//#212 エラー回避補正
		err = ::SetWindowBounds(mWindowRef,kWindowContentRgn,&rect);
		if( err != noErr )   _ACErrorNum("SetWindowBounds() returned error: ",err,this);
	}
#endif
	//#476 フルスクリーンモードでないときのウインドウサイズを記憶
	if( mFullScreenMode == false )
	{
		GetWindowBounds(mWindowBoundsInStandardState);
	}
}	

//#404
/**
ウインドウ領域設定
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
ウインドウ幅取得
*/
ANumber	CWindowImp::GetWindowWidth() const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return 100;}
	//ウインドウBounds取得
	ARect	rect;
	GetWindowBounds(rect);
	return rect.right-rect.left;
}

/**
ウインドウ幅設定
*/
void	CWindowImp::SetWindowWidth( const ANumber inWidth )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//ウインドウBounds取得
	ARect	rect;
	GetWindowBounds(rect);
	//ウインドウBounds設定
	rect.right = rect.left + inWidth;
	SetWindowBounds(rect);
}

/**
ウインドウ高さ取得
*/
ANumber	CWindowImp::GetWindowHeight() const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return 100;}
	//ウインドウBounds取得
	ARect	rect;
	GetWindowBounds(rect);
	return rect.bottom-rect.top;
}

/**
ウインドウ高さ設定
*/
void	CWindowImp::SetWindowHeight( const ANumber& inHeight )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//ウインドウBounds取得
	ARect	rect;
	GetWindowBounds(rect);
	//ウインドウBounds設定
	rect.bottom = rect.top + inHeight;
	SetWindowBounds(rect);
}

/**
ウインドウサイズ設定
*/
void	CWindowImp::SetWindowSize( const ANumber inWidth, const ANumber inHeight, const ABool inAnimate )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//ウインドウBounds取得
	ARect	rect;
	GetWindowBounds(rect);
	//ウインドウBounds設定
	rect.right	= rect.left + inWidth;
	rect.bottom	= rect.top + inHeight;
	SetWindowBounds(rect,inAnimate);
}

/**
ウインドウ位置取得
*/
void	CWindowImp::GetWindowPosition( APoint& outPoint ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//ウインドウBounds取得
	ARect	rect = {0};
	GetWindowBounds(rect);
	//左上ポイント取得
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
ウインドウ位置設定
*/
void	CWindowImp::SetWindowPosition( const APoint& inPoint )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//ウインドウ移動
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
	//#476 フルスクリーンモードでないときのウインドウサイズを記憶
	if( mFullScreenMode == false )
	{
		GetWindowBounds(mWindowBoundsInStandardState);
	}
}

/**
ウインドウ位置を画面に合わせて補正
*/
void	CWindowImp::ConstrainWindowPosition( const ABool inAllowPartial )//#370
{
	//#404 フルスクリーン中は位置補正しない
	if( mFullScreenMode == true )   return;
	
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//位置補正
	if( true )
	{
		/*#688
		OSStatus	err = noErr;
		
		//#370 kWindowConstrainAllowPartialを付けるとたとえば左側が見えなくなる状態のときにConstrainされない
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
		
		//#370 kWindowConstrainAllowPartialを付けるとたとえば左側が見えなくなる状態のときにConstrainされない
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
ウインドウ位置を画面に合わせて補正した場合の位置を取得
*/
void	CWindowImp::ConstrainWindowPosition( const ABool inAllowPartial, APoint& ioPoint ) const
{
	//screen bounds取得
	AGlobalRect	screenBounds = {0};
	AIndex	screenIndex = FindScreenIndexFromPoint(ioPoint);
	if( screenIndex == kIndex_Invalid )
	{
		//指定ポイントがスクリーン外の場合は、現在のウインドウのあるスクリーンのboundsを取得
		GetAvailableWindowBoundsInSameScreen(screenBounds);
	}
	else
	{
		//指定ポイントが属するスクリーンのscreen boundsを取得
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
	if( ioPoint.y + 20 > screenBounds.bottom )//+20はタイトルバー分
	{
		ioPoint.y = screenBounds.bottom -20;
	}
	//★allow partial未対応（常にinAllowPartial=trueの動作）
}

//#385
/**
ウインドウサイズを画面に合わせて補正
*/
void	CWindowImp::ConstrainWindowSize()
{
	//#404 フルスクリーン中は位置補正しない
	if( mFullScreenMode == true )   return;
	
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//画面サイズに応じてウインドウサイズ補正
	if( true )
	{
		//処理なし
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
		//画面サイズ取得
		AGlobalRect	globalRect;
		CWindowImp::GetAvailableWindowPositioningBounds(globalRect);
		/*サイズボックスが押せれば良いので、横方向補正はしない
		if( (contentRect.right-contentRect.left) > (globalRect.right-globalRect.left) )
		{
		contentRect.right = contentRect.left + (globalRect.right-globalRect.left);
		}
		*/
		//タイトルバーの高さ＋中身高さが画面サイズより大きければ、サイズ補正
		if( (titlebarRect.bottom-titlebarRect.top)+(contentRect.bottom-contentRect.top) > (globalRect.bottom-globalRect.top) )
		{
			contentRect.bottom = contentRect.top + (globalRect.bottom-globalRect.top) - (titlebarRect.bottom-titlebarRect.top);
			//サイズ設定
			err = ::SetWindowBounds(mWindowRef,kWindowContentRgn,&contentRect);
			if( err != noErr )   _ACErrorNum("SetWindowBounds() returned error: ",err,this);
			//きっちり収まるように、allowPartial=falseで位置補正
			ConstrainWindowPosition(false);
		}
	}
#endif
	//#476 フルスクリーンモードでないときのウインドウサイズを記憶
	if( mFullScreenMode == false )
	{
		GetWindowBounds(mWindowBoundsInStandardState);
	}
}

#pragma mark ===========================

#pragma mark --- コントロール制御

/**
コントロール強制描画
*/
void	CWindowImp::DrawControl( const AControlID inID )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//描画指示
		[view setNeedsDisplay:YES];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//コントロール描画
		::Draw1Control(GetControlRef(inID));
	}
#endif
}

#pragma mark ===========================

#pragma mark --- コントロール情報Set

/**
コントロールEnable/Disable
*/
void	CWindowImp::SetEnableControl( const AControlID inID, const ABool inEnable )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//現在と同じなら何もせずリターン
	//ウインドウを最初に開いたときにMLTEPaneが灰色になるので右は削除if( IsControlEnabled(inID) == inEnable )   return;
	
	//UserPaneの場合はPreProcessを実行する
	if( IsUserPane(inID) == true )
	{
		/*#688
		GetUserPane(inID).SetEnableControl(inEnable);
		UpdateFocus();
		return;
		*/
		GetUserPane(inID).PreProcess_SetEnableControl(inEnable);//#688
	}
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//segmented controlでの実行を試す
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
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSControlの場合、enabled設定
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
			//対応するstepperがあればstepperも同時にenable/disableする
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
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
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
			//ラジオグループに設定されたボタンの場合、Onでdisableにされたら、他のボタンのどれかをOnにする。
			/*#349 ラジオグループの全てのボタンをdissableにするときにOnが移動するのでこの処理はアプリ層で必要なときに行うことにする
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
コントロールがEnableかどうかを取得
*/
ABool	CWindowImp::IsControlEnabled( const AControlID inID ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	/*#688
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsControlEnabled();
	}
	*/
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//segmented controlでの実行を試す
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
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSControlの場合、enabled設定
		if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			return ([control isEnabled]==YES);
		}
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
			return true;
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		AControlRef	controlRef = GetControlRef(inID);
		return ::IsControlEnabled(controlRef);
	}
#endif
}

/**
コントロールShow/Hide
*/
void	CWindowImp::SetShowControl( const AControlID inID, const ABool inShow )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		/*#688
		GetUserPane(inID).SetShowControl(inShow);
		return;
		*/
		GetUserPane(inID).PreProcess_SetShowControl(inShow);//#688
	}
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//Show/Hide設定（setHiddenはNSViewで定義されている）
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
		//Carbonウインドウの場合
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
コントロールのShow/Hide取得
*/
ABool	CWindowImp::IsControlVisible( const AControlID inID ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPaneの場合は対応メソッドをコール
	/*#688
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsControlVisible();
	}
	*/
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//toolbar itemでの実行を試す
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
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//Show/Hide状態取得（isHiddenOrHasHiddenAncestorはNSViewで定義されている。view自身または親viewがhiddenのときYESになる）
		return ([view isHiddenOrHasHiddenAncestor]==NO);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		AControlRef	controlRef = GetControlRef(inID);
		return ::IsControlVisible(controlRef);
	}
#endif
}

/**
Controlのテキストスタイル設定
*/
void	CWindowImp::SetTextStyle( const AControlID inID, const ABool inBold )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetTextStyle(inBold);
		return;
	}
	//通常コントロールの場合
	//未実装
}

//テキストフォント設定
/*win void	CWindowImp::SetTextFont( const AControlID inID, const AFont inFont, const AFontSize inFontSize )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetTextFont(inFont,inFontSize);
		return;
	}
	//通常コントロールの場合
	ControlFontStyleRec	fontStyle;
	fontStyle.font = inFont;
	fontStyle.size = inFontSize;
	fontStyle.flags = kControlUseFontMask + kControlUseSizeMask;
	::SetControlFontStyle(GetControlRef(inID),&fontStyle);
}*/

/**
ControlのTextFontを設定
*/
void	CWindowImp::SetTextFont( const AControlID inID, const AText& inFontName, const AFloatNumber inFontSize )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetTextFont(inFontName,inFontSize);
		return;
	}
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSControlの場合、font設定
		if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			//font設定
			AStCreateNSStringFromAText	str(inFontName);
			[control setFont:[NSFont fontWithName:str.GetNSString() size:inFontSize]];
		}
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロールの場合
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
リターンキーをコントロール内で処理するかどうかを設定
*/
void	CWindowImp::SetCatchReturn( const AControlID inID, const ABool inCatchReturn )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetCatchReturn(inCatchReturn);
		return;
	}
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//処理なし
		//リターンキー／タブキーの処理はOSに任せる
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロールの場合
		_AError("not implemented",this);
	}
#endif
}

/**
リターンキーをコントロール内で処理するかどうかを取得
*/
ABool	CWindowImp::IsCatchReturn( const AControlID inID ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsCatchReturn();
	}
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//処理なし
		//リターンキー／タブキーの処理はOSに任せる
		return false;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロールの場合
		_AError("not implemented",this);
		return false;
	}
#endif
}

/**
タブキーをコントロール内で処理するかどうかを設定
*/
void	CWindowImp::SetCatchTab( const AControlID inID, const ABool inCatchTab )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetCatchTab(inCatchTab);
		return;
	}
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//処理なし
		//リターンキー／タブキーの処理はOSに任せる
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロールの場合
		_AError("not implemented",this);
	}
#endif
}

/**
タブキーをコントロール内で処理するかどうかを取得
*/
ABool	CWindowImp::IsCatchTab( const AControlID inID ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsCatchTab();
	}
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//処理なし
		//リターンキー／タブキーの処理はOSに任せる
		return false;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロールの場合
		return false;
	}
#endif
}

/**
FocusをSwitchしたときに全選択するかどうかを設定
*/
void	CWindowImp::SetAutoSelectAllBySwitchFocus( const AControlID inID, ABool inAutoSelectAll )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetAutoSelectAllBySwitchFocus(inAutoSelectAll);
		return;
	}
	//通常コントロールの場合
	if( true )
	{
		//処理なし
		//select allするかどうかはクラス次第とする（NSTextFieldなら選択時にselect allされる）
	}
#if SUPPORT_CARBON
	else
	{
		_AError("not implemented",this);
	}
#endif
}

/**
FocusをSwitchしたときに全選択するかどうかを取得
*/
ABool	CWindowImp::IsAutoSelectAllBySwitchFocus( const AControlID inID ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsAutoSelectAllBySwitchFocus();
	}
	//通常コントロールの場合
	if( true )
	{
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSTextFieldの場合は、focus設定時に全選択されるので、trueを返す
		if( [view isKindOfClass:[NSTextField class]] == YES )
		{
			return true;
		}
		//上記以外は、falseを返す
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
コントロールのデータタイプを設定する

このデータタイプによってキー入力時のデータチェックが行われる
*/
void	CWindowImp::SetDataType( const AControlID inControlID, const ADataType inDataType )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	mDataType_ControlID.Add(inControlID);
	mDataType_DataType.Add(inDataType);
}

/**
Control位置設定
*/
void	CWindowImp::SetControlPosition( const AControlID inID, const AWindowPoint& inPoint )
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//引数のWindowPointを親View上の位置へ変換
		//ptに親view上の位置を格納する
		NSView*	superView = [view superview];
		NSPoint	wpt = ConvertFromWindowPointToNSPoint(CocoaObjects->GetContentView(),inPoint);
		NSPoint	pt = [CocoaObjects->GetContentView() convertPoint:wpt toView:superView];
		
		//位置設定
		if( [superView isFlipped] == NO )
		{
			//親がflip:noの場合、（子viewの左上ではなく）子viewの左下位置を、setFrameOriginで設定する
			pt.y = pt.y - [view frame].size.height;
		}
		//#852
		//viewの位置に変更が無ければ何もせず終了
		NSRect	viewFrame = [view frame];
		if( viewFrame.origin.x == pt.x && viewFrame.origin.y == pt.y )
		{
			return;
		}
		//位置設定
		[view setFrameOrigin:pt];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		OSStatus	err = noErr;
		
		/*#182 タブコントロール内の別のView内に存在するViewの場合にView位置がずれる問題を修正
		HIViewSetFrame()は親View相対位置を設定するので、HIViewConvertPoint()でWindowPointを親View相対位置に変換して設定するようにする
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
		//親View取得
		AControlRef	parent = NULL;
		parent = ::HIViewGetSuperview(GetControlRef(inID));
		//引数のWindowPointを親View上の位置へ変換
		HIPoint	pt;
		pt.x = inPoint.x;
		pt.y = inPoint.y;
		err = ::HIViewConvertPoint(&pt,GetWindowContentView(),parent);
		if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
		//現在の位置・サイズを取得
		HIRect	rect;
		err = ::HIViewGetFrame(GetControlRef(inID),&rect);
		if( err != noErr )   _ACErrorNum("HIViewGetFrame() returned error: ",err,this);
		//位置を変更して設定
		rect.origin = pt;
		err = ::HIViewSetFrame(GetControlRef(inID),&rect);
		if( err != noErr )   _ACErrorNum("HIViewSetFrame() returned error: ",err,this);
	}
#endif
}

/**
コントロールサイズ設定
*/
void	CWindowImp::SetControlSize( const AControlID inID, const ANumber inWidth, const ANumber inHeight )
{
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//現在のframe取得
		NSRect	viewFrame = [view frame];
		//#852
		//viewのサイズに変更が無ければ何もせず終了
		if( viewFrame.size.width == inWidth && viewFrame.size.height == inHeight )
		{
			return;
		}
		//親viewがflipped:noの場合、originは左下なので、左上固定にするためには、originを移動する必要がある。
		if( [[view superview] isFlipped] == NO )
		{
			viewFrame.origin.y -= (inHeight-viewFrame.size.height);
		}
		//サイズ設定
		viewFrame.size.width = inWidth;
		viewFrame.size.height = inHeight;
		[view setFrame:viewFrame];
		
		//NSScrollerの場合、width/heightに応じて、コントロールのサイズタイプを設定
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
		//Carbonウインドウの場合
		
		::SizeControl(GetControlRef(inID),inWidth,inHeight);
	}
#endif
}

/**
HelpTag(Tooltip)設定
*/
void	CWindowImp::SetHelpTag( const AControlID inID, const AText& inText, const AHelpTagSide inTagSide )//#634
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//tool tipテキスト設定
		AStCreateNSStringFromAText	str(inText);
		[view setToolTip:str.GetNSString()];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		OSStatus	err = noErr;
		
		//ウインドウ生成済みチェック
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
		CFStringRef	string = inText.CreateCFString();//#693 リーク対策
		::SetRect(&helpTag.absHotRect, 0, 0, 0, 0);
		helpTag.content[kHMMinimumContentIndex].contentType = kHMCFStringContent;
		helpTag.content[kHMMinimumContentIndex].u.tagCFString = string;//#693 リーク対策inText.CreateCFString();//AUtil::CreateCFStringFromText(inText);
		helpTag.content[kHMMaximumContentIndex].contentType = kHMNoContent;
		err = ::HMSetControlHelpContent(GetControlRef(inID), &helpTag); 
		if( err != noErr )   _ACErrorNum("HMSetControlHelpContent() returned error: ",err,this);
		::CFRelease(string);//#693 リーク対策
	}
#endif
}

//R0246
/**
HelpTag強制Hide
*/
void	CWindowImp::HideHelpTag() const
{
	if( true )
	{
		//処理なし
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
HelpTagコールバックをEnableにする
*/
void	CWindowImp::EnableHelpTagCallback( const AControlID inControlID, const ABool inShowOnlyWhenNarrow )//#507
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
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
		//Carbonウインドウの場合
		
		OSStatus	err = noErr;
		
		err = ::HMInstallControlContentCallback(GetControlRef(inControlID),NewHMControlContentUPP(STATIC_APICB_ControlHelpTagCallback));
		if( err != noErr )   _ACErrorNum("HMInstallControlContentCallback() returned error: ",err,this);
	}
#endif
}

/**
Viewを他Viewの中にEmbedする（inChildのControlの親をinParentにする）
*/
void	CWindowImp::EmbedControl( const AControlID inParent, const AControlID inChild )
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	parentView = CocoaObjects->FindViewByTag(inParent);
		NSView*	childView = CocoaObjects->FindViewByTag(inChild);
		
		//現在のWindowPointでの座標を記憶する
		AWindowPoint	wpt = {0};
		GetControlPosition(inChild,wpt);
		//fprintf(stderr,"EmbedControl:%d:wpt(%d,%d)\n",inChild,pt.x,pt.y);
		
		//現在の親viewからremove
		[childView removeFromSuperview];
		//指定された親viewに子として追加
		[parentView addSubview:childView positioned:NSWindowAbove relativeTo:nil];
		
		//位置を元のWindowPointの座標に設定する
		SetControlPosition(inChild,wpt);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		OSStatus	err = noErr;
		
		//現在のWindowPointでの座標を記憶する
		AWindowPoint	pt;
		GetControlPosition(inChild,pt);
		//inParent内に入れる
		err = ::HIViewAddSubview(GetControlRef(inParent),GetControlRef(inChild));
		if( err != noErr )   _ACErrorNum("HIViewAddSubview() returned error: ",err,this);
		//位置を元のWindowPointの座標に設定する
		SetControlPosition(inChild,pt);
	}
#endif
}

#if SUPPORT_CARBON
//#236
/**
Drop可能なControlを登録する

DeleteControl()で登録解除される
*/
void	CWindowImp::RegisterDropEnableControl( const AControlID inControlID )
{
	mDropEnableControlArray.Add(inControlID);
}
#endif

//#291
/**
コントロールのZorderを設定
*/
void	CWindowImp::SetControlZOrderToTop( const AControlID inControlID )
{
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		//処理なし
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
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
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		//autoresizeMask設定
		NSUInteger	autoResizingMask = NSViewNotSizable;
		if( inLeftMarginFixed == false )
		{
			autoResizingMask |= NSViewMinXMargin;
		}
		if( inTopMarginFixed == false )
		{
			//Superviewのflip状態によって、MinY/MaxY切り替え
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
			//Superviewのflip状態によって、MinY/MaxY切り替え
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
		//Carbonウインドウの場合
		
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
コントロールをFileDrop可能に設定
*/
void	CWindowImp::EnableFileDrop( const AControlID inControlID )
{
	if( true )
	{
		//処理なし
	}
#if SUPPORT_CARBON
	else
	{
		RegisterDropEnableControl(inControlID);
	}
#endif
}

//#688
//CUserPaneから移動
/**
描画Refresh（指定Rect）
*/
void	CWindowImp::RefreshControlRect( const AControlID inControlID, const ALocalRect inLocalRect )
{
	if( true )
	{
		//Cocoaウインドウの場合
		
		//
		ALocalRect	localRect = inLocalRect;
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		//フレームサイズでトリミング
		ALocalRect	localFrameRect = {0,0,0,0};
		localFrameRect.right	= GetControlWidth(inControlID);
		localFrameRect.bottom	= GetControlHeight(inControlID);
		if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
		{
			//縦書き座標変換（App座標系->Imp座標系） #558
			localRect = ConvertVerticalCoordinateFromAppToImp(inControlID,inLocalRect);
		}
		ALocalRect	intersectRect = {0,0,0,0};
		if( GetIntersectRect(localRect,localFrameRect,intersectRect) == false )   return;
		
		//トリミングした領域を描画指示
		ALocalPoint	intersectLeftTopPoint = {intersectRect.left,intersectRect.top};
		NSRect	nsrect = {0};
		nsrect.origin = ConvertFromLocalPointToNSPoint(view,intersectLeftTopPoint);
		nsrect.size.width  = intersectRect.right - intersectRect.left;
		nsrect.size.height = intersectRect.bottom - intersectRect.top;
		[view setNeedsDisplayInRect:nsrect];
		//Carbon版ではHIViewRender()を実行していたが、
		//CocoaでdisplayIfNeededを実行すると、非常に遅くなるので、実行しないことにする。
		//（描画タイミングが変わったので、Scroll前にdisplay実行するなどの対策も同時に行った）
		//[view displayIfNeeded];
		
		//fprintf(stderr,"RefreshControlRect():%d(%d,%d,%d,%d) \n",inControlID,inLocalRect.left,
		//	inLocalRect.top,inLocalRect.right,inLocalRect.bottom);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
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
		//R0108高速化 フレームサイズトリミングの結果、localRect.top > localRect.bottom となると、DoDraw時のUpdateRegion判定で、全範囲対象になってしまう？ので
		//RefreshRectがフレーム外のときは何もしないようにする
		if( inLocalRect.bottom < localFrameRect.top )   return;
		if( inLocalRect.top > localFrameRect.bottom )   return;
		if( inLocalRect.right < localFrameRect.left )   return;
		if( inLocalRect.left > localFrameRect.right )   return;
		//OSX10.3ではHIViewSetNeedsDisplayInRectが使えないので、下記に変更
		//フレームサイズでトリミングしておかないとHIViewSetNeedsDisplayInRegionのほうはうまくいかないことがある
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
//CUserPaneから移動
/**
スクロール
*/
void	CWindowImp::ScrollControl( const AControlID inControlID, const ANumber inDeltaX, const ANumber inDeltaY )
{
	if( inDeltaX == 0 && inDeltaY == 0 )   return;
	
	//
	ANumber	deltaX = inDeltaX;
	ANumber	deltaY = inDeltaY;
	
	//縦書きモードならスクロール方向変換 #558
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		ANumber	swap = deltaX;
		deltaX = -deltaY;
		deltaY = swap;
	}
	
	//fprintf(stderr,"%16X ScrollControl() - Start\n",(int)(GetObjectID().val));
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//左下が丸いのを考慮したスクロールを行うかどうかのフラグ #947
		ABool	adjustForRounded = true;
		if( IsUserPane(inControlID) == true )
		{
			adjustForRounded = GetUserPaneConst(inControlID).GetAdjustScrollForRoundedCorner();
		}
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		//上方向スクロールのときは、スクロール範囲を4pixel縮める。(#947 左下が丸くなっているので。)
		NSRect	scrollbounds = [view bounds];
		if( adjustForRounded == true )
		{
			if( deltaY < 0 )
			{
				scrollbounds.size.height -= 4;
			}
		}
		//スクロール実行
		NSSize	delta = NSMakeSize(deltaX,deltaY);
		[view scrollRect:scrollbounds by:delta];
		//左下が丸くなっているのを考慮して描画更新範囲を追加 (#947)
		if( adjustForRounded == true )
		{
			if( deltaY < 0 )
			{
				//上方向スクロールの時は、更新範囲を4pixel追加する。
				NSRect	r = [view bounds];
				r.origin.y = r.size.height + deltaY - 4;
				r.size.height = 4;
				[view setNeedsDisplayInRect:r];
			}
			else
			{
				//下方向スクロールの時は、一番下の4pixelを更新範囲に追加する。
				NSRect	r = [view bounds];
				r.origin.y = r.size.height - 4;
				r.size.height = 4;
				[view setNeedsDisplayInRect:r];
				//上部分と下部分を同時にマーキングした状態でdisplayIfNeededすると全体が表示更新されるので、ここでまず表示更新する。
				[CocoaObjects->GetWindow() displayIfNeeded];
			}
		}
		
		//再描画指示
		NSRect	bounds = [view bounds];
		//再描画領域はL字型になるが、横長の領域を優先して作ってrect1とし、残りの縦長をrect2とする
		NSRect	rect1 = bounds;
		NSRect	rect2 = bounds;
		//Yスクロールした場合
		if( deltaY != 0 )
		{
			if( deltaY < 0 )
			{
				//下方向スクロールの場合（上部分が再描画になる）
				//rect1は上部分の横長領域
				rect1.origin.y = bounds.origin.y + bounds.size.height + deltaY;
				rect1.size.height = -deltaY;
				//rect2の再描画領域からrect1で描画した分を除く
				rect2.size.height += deltaY;
			}
			else
			{
				//上方向スクロールの場合（下部分が再描画になる）
				rect1.size.height = deltaY;
				//rect2の再描画領域からrect1で描画した分を除く
				rect2.origin.y = bounds.origin.y + deltaY;
				rect2.size.height -= deltaY;
			}
			[view setNeedsDisplayInRect:rect1];
		}
		//Xスクロールした場合
		if( deltaX != 0 )
		{
			if( deltaX < 0 )
			{
				//左方向スクロールの場合（右部分が再描画になる）
				rect2.origin.x = bounds.origin.x + bounds.size.width + deltaX;
				rect2.size.width = -deltaX;
			}
			else
			{
				//右方向スクロールの場合（左部分が再描画になる）
				rect2.size.width = deltaX;
			}
			[view setNeedsDisplayInRect:rect2];
		}
		//EVT_DoDraw()を実行（バッファへの描画）
		//ここでdrawしないと、scrollの後、描画イベント発生前に再度scroll発生したときに、
		//今回描画すべき領域が描画される前に、scrollされてしまい、描画されない。
		//削除（これがあるとdiff infoウインドウとスクロールがきっちりと同期しないので。NVI_Scroll()で最初にExecuteDoDrawImmediately()をコールしているので、そこでdisplayIfNeededがコールされる。） [view displayIfNeeded];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
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
progress indicatorの値設定
*/
void	CWindowImp::SetProgressIndicatorProgress( const AIndex inProgressIndex, const ANumber inProgress, const ABool inShow )
{
	if( inShow == false )
	{
		//progress非表示
		[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) setHidden:YES];
	}
	else
	{
		//progress表示
		[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) setHidden:NO];
		
		//progress値設定
		if( inProgress < 0 )
		{
			//マイナスの場合は、indeterminate表示
			[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) setIndeterminate:YES];
			[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) setUsesThreadedAnimation:YES];
			[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) startAnimation:CocoaObjects->GetWindow()];
		}
		else
		{
			//値設定
			[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) setIndeterminate:NO];
			[CocoaObjects->FindProgressIndicatorByIndex(inProgressIndex) setDoubleValue:inProgress];
		}
	}
}

#pragma mark ===========================

#pragma mark --- コントロール情報Get

/**
コントロール存在チェック
*/
ABool	CWindowImp::ExistControl( const AControlID inID ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	if( true )
	{
		//Cocoaウインドウの場合
		
		//segment controlでの実行を試す
		if( [CocoaObjects->GetWindow() isKindOfClass:[CocoaWindow class]] == YES )
		{
			CocoaWindow*	cocoaWindow = static_cast<CocoaWindow*>(CocoaObjects->GetWindow());
			if( [cocoaWindow isSegmentControl:inID] == YES )
			{
				return true;
			}
		}
		
		//Viewが見つかったらtrue
		return (CocoaObjects->FindViewByTag(inID,false) != nil);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		return (GetControlRef(inID)!=NULL);
	}
#endif
}

/**
位置取得
*/
void	CWindowImp::GetControlPosition( const AControlID inID, AWindowPoint& outPoint ) const
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//LocalPointで(0,0)のポイントをWindowPointへ変換
		ALocalPoint	lpt = {0,0};
		GetWindowPointFromControlLocalPoint(inID,lpt,outPoint);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		OSStatus	err = noErr;
		
		/*win 080618
		タブコントロールの場合、GetControlBounds()では、タブ相対になってしまう（ウインドウ座標にならない）
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
幅取得
*/
ANumber	CWindowImp::GetControlWidth( const AControlID inID ) const
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//view幅取得
		return ([view frame].size.width);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		Rect	rect;
		::GetControlBounds(GetControlRef(inID),&rect);
		return rect.right - rect.left;
	}
#endif
}

/**
高さ取得
*/
ANumber	CWindowImp::GetControlHeight( const AControlID inID ) const
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//view高さ取得
		return ([view frame].size.height);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		Rect	rect;
		::GetControlBounds(GetControlRef(inID),&rect);
		return rect.bottom - rect.top;
	}
#endif
}

#pragma mark ===========================

#pragma mark --- 座標系変換

/*

【座標系変換】

ウインドウ
+--------------+
o--------------+
|     x-----+  |
|     |     |←コントロール
|     +-----+  |
+--------------+

o: ウインドウ座標系(AWindowPoint, AWindowRect)の原点
x: ローカル座標系(ALocalPoint, ALocalRect)の原点

HIViewGetFrameは親HIViewに対する座標なので、ウインドウーローカル変換には使えない。
HIViewConvertRectを使用するが、ルートビューはタイトルバーも含むのでウインドウビューとしてはNULLではなくGetWindowContentView()を使用する。

*/

/**
コントロールのローカル座標からウインドウ座標を取得
*/
void	CWindowImp::GetWindowPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AWindowPoint& outWindowPoint ) const
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//LocalPointからNSPointへ変換
		NSPoint	viewPoint = ConvertFromLocalPointToNSPoint(view,inLocalPoint);
		//Content view内の座標に変換
		NSPoint	windowPoint = [view convertPoint:viewPoint toView:CocoaObjects->GetContentView()];
		//NSPointからWindowPointへ変換
		outWindowPoint = ConvertFromNSPointToWindowPoint(CocoaObjects->GetContentView(),windowPoint);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
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
コントロールのローカル座標からウインドウ座標を取得
*/
void	CWindowImp::GetWindowRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AWindowRect& outWindowRect ) const
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//左上の座標変換
		ALocalPoint	viewLeftTop = {inLocalRect.left,inLocalRect.top};
		//LocalPointからContent view内のNSPointへ変換
		NSPoint	windowLeftTop = [view 
				convertPoint:ConvertFromLocalPointToNSPoint(view,viewLeftTop) 
				toView:CocoaObjects->GetContentView()];
		//右上の座標変換
		ALocalPoint	viewRightBottom = {inLocalRect.right,inLocalRect.bottom};
		//LocalPointからContent view内のNSPointへ変換
		NSPoint	windowRightBottom = [view 
				convertPoint:ConvertFromLocalPointToNSPoint(view,viewRightBottom) 
				toView:CocoaObjects->GetContentView()];
		//NSPointからWindowPointへ変換
		AWindowPoint	lefttop 	= ConvertFromNSPointToWindowPoint(CocoaObjects->GetContentView(),windowLeftTop);
		AWindowPoint	rightbottom	= ConvertFromNSPointToWindowPoint(CocoaObjects->GetContentView(),windowRightBottom);
		//WindowRectへ格納
		outWindowRect.left		= lefttop.x;
		outWindowRect.top		= lefttop.y;
		outWindowRect.right		= rightbottom.x;
		outWindowRect.bottom	= rightbottom.y;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
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
ウインドウ座標からコントロールのローカル座標を取得
*/
void	CWindowImp::GetControlLocalPointFromWindowPoint( const AControlID inID, const AWindowPoint& inWindowPoint, ALocalPoint& outLocalPoint ) const
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//WindowPointからNSPointへ変換
		NSPoint	windowPoint = ConvertFromWindowPointToNSPoint(CocoaObjects->GetContentView(),inWindowPoint);
		//View内の座標へ変換
		NSPoint	viewPoint = [view convertPoint:windowPoint fromView:CocoaObjects->GetContentView()];
		//NSPointからLocalPointへ変換
		outLocalPoint = ConvertFromNSPointToLocalPoint(view,viewPoint);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
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
ウインドウ座標からコントロールのローカル座標を取得
*/
void	CWindowImp::GetControlLocalRectFromWindowRect( const AControlID inID, const AWindowRect& inWindowRect, ALocalRect& outLocalRect ) const
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//左上の座標変換
		AWindowPoint	windowLeftTop = {inWindowRect.left,inWindowRect.top};
		//WindowPointからview内のNSPointへ変換
		NSPoint	viewLeftTop = [view 
				convertPoint:ConvertFromWindowPointToNSPoint(CocoaObjects->GetContentView(),windowLeftTop) 
				fromView:CocoaObjects->GetContentView()];
		//右上の座標変換
		AWindowPoint	windowRightBottom = {inWindowRect.right,inWindowRect.bottom};
		//WindowPointからview内のNSPointへ変換
		NSPoint	viewRightBottom = [view 
				convertPoint:ConvertFromWindowPointToNSPoint(CocoaObjects->GetContentView(),windowRightBottom) 
				fromView:CocoaObjects->GetContentView()];
		//NSPointからLocalPointへ変換
		ALocalPoint	lefttop		= ConvertFromNSPointToLocalPoint(view,viewLeftTop);
		ALocalPoint	rightbottom	= ConvertFromNSPointToLocalPoint(view,viewRightBottom);
		//LocalRectへ格納
		outLocalRect.left		= lefttop.x;
		outLocalRect.top		= lefttop.y;
		outLocalRect.right		= rightbottom.x;
		outLocalRect.bottom		= rightbottom.y;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
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
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//LocalPointからWindowPointへ変換
		AWindowPoint	wpt = {0};
		GetWindowPointFromControlLocalPoint(inID,inLocalPoint,wpt);
		//ウインドウの左上座標を足す
		AGlobalRect	windowbounds = {0};
		GetWindowBounds(windowbounds);
		outGlobalPoint.x = windowbounds.left + wpt.x;
		outGlobalPoint.y = windowbounds.top + wpt.y;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
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
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//LocalRectからWindowRectへ変換
		AWindowRect	wrect = {0};
		GetWindowRectFromControlLocalRect(inID,inLocalRect,wrect);
		//ウインドウの左上座標を足す
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
		//Carbonウインドウの場合
		
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
指定Controlが含まれるTabControlのIndexを取得する
*/
AIndex	CWindowImp::GetControlEmbeddedTabIndex( const AControlID inID ) const
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//処理なし
		return kIndex_Invalid;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
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
Carbon Event(kEventParamWindowMouseLocation)で取得したPointからControlLocalPointへ変換
*/
void	CWindowImp::GetControlLocalPointFromEventParamWindowMouseLocation( 
		const AControlID inID, const HIPoint& inPoint, ALocalPoint& outControlPoint ) const
{
	HIPoint	pt = inPoint;
	OSErr	err = ::HIViewConvertPoint(&pt,NULL,GetControlRef(inID));//第二引数はNULL
	if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
	outControlPoint.x = pt.x;
	outControlPoint.y = pt.y;
}
#endif

#pragma mark ===========================

#pragma mark --- コントロール値のSet/Getインターフェイス

/**
Text取得
*/
void	CWindowImp::GetText( const AControlID inID, AText& outText ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPaneConst(inID).GetText(outText);
		return;
	}
	
	//#688 Cocoa対応
	outText.DeleteAll();
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//値取得
		//NSPopUpButtonの場合、選択中の項目のテキストを取得
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			
			//text array menuの場合は、選択indexを元に、text array menu managerからaction textを取得する
			AIndex	textArrayMenuArrayIndex = mTextArrayMenuArray_ControlID.Find(inID);
			if( textArrayMenuArrayIndex != kIndex_Invalid )
			{
				//選択indexを取得
				AIndex	selItemIndex = [popup indexOfSelectedItem];
				if( selItemIndex != -1 )
				{
					//pull down の場合は、最初が1なので-1する
					if( [popup pullsDown] == YES )
					{
						selItemIndex--;
					}
					//text array menu managerからaction textを取得
					AApplication::GetApplication().NVI_GetTextArrayMenuManager().
							GetActionTextFromItemIndex(GetAWinConst().GetObjectID(),inID,selItemIndex,outText);
				}
			}
			else
			{
				//メニューの現在の選択位置のテキストを取得
				ACocoa::SetTextFromNSString([popup titleOfSelectedItem],outText);
			}
		}
		//★ボタン内メニューに対応する場合は対応必要
		//NSControlの場合、stringValueを取得
		else if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			ACocoa::SetTextFromNSString([control stringValue],outText);
		}
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロールの場合
		OSStatus	err = noErr;
		
		AControlRef	controlRef = GetControlRef(inID);
		ControlKind	kind;
		err = ::GetControlKind(controlRef,&kind);
		if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
		//ポップアップボタンの場合は現在選択中のメニュー項目文字列を返す
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
		//BevelButtonかつメニューが存在する場合
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
		//その他コントロールの場合GetControlData()で取得
		outText.DeleteAll();
		CFStringRef	strref = NULL;
		Size	actSize = 0;
		err = ::GetControlData(controlRef,kControlEntireControl,kControlEditTextCFStringTag,sizeof(strref),&strref,&actSize);
		//#0 ボタン等の場合エラーになる（この場合空白テキストで返すことにする） if( err != noErr )   _ACErrorNum("GetControlData() returned error: ",err,this);
		if( strref != NULL )
		{
			outText.SetFromCFString(strref);
			::CFRelease(strref);
		}
	}
#endif
}

/**
Text設定
*/
void	CWindowImp::SetText( const AControlID inID, const AText& inText, const ABool inOnlyText )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetText(inText);
		return;
	}
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSString取得
		AStCreateNSStringFromAText	str(inText);
		//値設定
		//NSPopUpButtonの場合、テキストに対応する項目を設定
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			
			if( [popup pullsDown] == YES )
			{
				//==================pull downメニューの場合==================
				AIndex	textArrayMenuArrayIndex = mTextArrayMenuArray_ControlID.Find(inID);
				if( textArrayMenuArrayIndex != kIndex_Invalid )
				{
					//==================text array menuの場合==================
					//text array menu managerから、指定テキストに対応する項目indexを検索
					AIndex	itemIndex = AApplication::GetApplication().NVI_GetTextArrayMenuManager().GetItemIndexFromActionText(GetAWin().GetObjectID(),inID,inText);
					//見つからなければ最初を選択
					if( itemIndex == kIndex_Invalid )   itemIndex = 0;
					
					//項目indexに対応するメニュー表示テキストを取得
					AText	title;
					AApplication::GetApplication().NVI_GetTextArrayMenuManager().GetMenuTextFromItemIndex(GetAWin().GetObjectID(),inID,itemIndex,title);
					//最初のメニュー項目（＝常に表示されるテキスト）のテキストを設定
					AMenuWrapper::SetMenuItemText([popup menu],0,title);
					
					//指定番号の項目を選択
					[popup selectItemAtIndex:itemIndex+1];
					
					//テキストに対応するメニュー項目にチェックマークを設定
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
					//==================text array menuではない場合==================
					//最初のメニュー項目（＝常に表示されるテキスト）のテキストを設定
					AMenuWrapper::SetMenuItemText([popup menu],0,inText);
					
					//テキストに対応するメニュー項目にチェックマークを設定
					AItemCount	itemCount = AMenuWrapper::GetMenuItemCount([popup menu]);
					for( AIndex i = 1; i < itemCount; i++ )
					{
						AText	text;
						AMenuWrapper::GetMenuItemText([popup menu],i,text);
						ABool	check = false;
						if( inText.Compare(text) == true )
						{
							check = true;
							//指定番号の項目を選択
							[popup selectItemAtIndex:i];
						}
						AMenuWrapper::SetCheckMark([popup menu],i,check);
					}
				}
			}
			else
			{
				//==================pull downではないメニューの場合==================
				AIndex	textArrayMenuArrayIndex = mTextArrayMenuArray_ControlID.Find(inID);
				if( textArrayMenuArrayIndex != kIndex_Invalid )
				{
					//==================text array menuの場合==================
					//text array menuから、指定テキストに対応する項目indexを取得
					AIndex	itemIndex = AApplication::GetApplication().NVI_GetTextArrayMenuManager().GetItemIndexFromActionText(GetAWin().GetObjectID(),inID,inText);
					//見つからなければ最初を選択
					if( itemIndex == kIndex_Invalid )   itemIndex = 0;
					//メニュー項目選択
					[popup selectItemAtIndex:itemIndex];
				}
				else
				{
					//==================text array menuではない場合==================
					//テキストに対応するメニュー項目のindexを取得
					NSInteger	itemIndex = [popup indexOfItemWithTitle:str.GetNSString()];
					//見つからなければ最初を選択
					if( itemIndex == -1 )   itemIndex = 0;
					//メニュー項目選択
					[popup selectItemAtIndex:itemIndex];
				}
			}
		}
		//NSButtonの場合、titleを取得
		else if( [view isKindOfClass:[NSButton class]] == YES )
		{
			NSButton*	button = ACocoa::CastNSButtonFromId(view);
			[button setTitle:str.GetNSString()];
		}
		//★ボタン内メニューに対応する場合は対応必要
		//NSControlの場合、stringValueを設定
		else if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			[control setStringValue:str.GetNSString()];
		}
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロールの場合
		OSStatus	err = noErr;
		
		AControlRef	controlRef = GetControlRef(inID);
		ControlKind	kind;
		err = ::GetControlKind(controlRef,&kind);
		if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
		//ポップアップボタンの場合はinTextに対応するメニュー項目を選択する
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
		//BevelButtonかつメニューが存在する場合
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
		//その他コントロールの場合
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
Text追加
*/
void	CWindowImp::InputText( const AControlID inID, const AText& inText )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).InputText(inText);
		return;
	}
	//通常コントロールの場合
	//未実装
	_ACError("not implemented",NULL);
}

/**
Text追加
*/
void	CWindowImp::AddText( const AControlID inID, const AText& inText )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).AddText(inText);
		return;
	}
	//通常コントロールの場合
	//未実装
	_ACError("not implemented",NULL);
}

/**
Bool取得
*/
void	CWindowImp::GetBool( const AControlID inID, ABool& outBool ) const
{
	outBool = GetBool(inID);
}

/**
Bool取得
*/
ABool	CWindowImp::GetBool( const AControlID inID ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		ABool	b;
		GetUserPaneConst(inID).GetBool(b);
		return b;
	}
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//値取得
		//NSPopUpButtonの場合、0番目の項目選択中ならfalse、1番目の項目選択中ならtrueを返す
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			return ([popup indexOfSelectedItem]==1);
		}
		//NSButtonの場合、stateを取得
		else if( [view isKindOfClass:[NSButton class]] == YES )
		{
			NSButton*	button = ACocoa::CastNSButtonFromId(view);
			//state取得
			if( [button state] == NSOnState )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
		}
		return false;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロールの場合
		AControlRef	controlRef = GetControlRef(inID);
		ControlKind	kind;
		::GetControlKind(controlRef,&kind);
		//ポップアップボタンの場合は1番目の項目選択中ならfalse、2番目の項目選択中ならtrueを返す
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
		//その他コントロールの場合
		return ::GetControl32BitValue(controlRef);
	}
#endif
}

/**
Bool設定
*/
void	CWindowImp::SetBool( const AControlID inID, const ABool inBool )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	/*#349
	//trueに設定する場合はUpdateRadioGroup()をコールすることにより、それがラジオグループ内だった場合は、他のコントロールをfalseにする
	if( inBool == true )
	{
		UpdateRadioGroup(inID);
	}
	*/
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetBool(inBool);
		return;
	}
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//segmented controlの場合の処理
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
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//値設定
		//NSPopUpButtonの場合、falseなら0番目、trueなら1番目の項目を選択する
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
		//NSButtonの場合、stateを設定
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
			//CocoaDisclosureButtonの場合は、おりたたみ実行
			if( [view isKindOfClass:[CocoaDisclosureButton class]] == YES )
			{
				CocoaDisclosureButton*	disclosureButton =  ACocoa::CastNSButtonFromId(view);
				[view updateCollapsableView];
			}
		}
		//Progress indicatorの場合、アニメーション開始／停止
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
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロールの場合
		OSStatus	err = noErr;
		
		AControlRef	controlRef = GetControlRef(inID);
		ControlKind	kind;
		err = ::GetControlKind(controlRef,&kind);
		if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
		//ポップアップボタンの場合はfalseなら1番目、trueなら2番目の項目を選択する
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
		//その他コントロールの場合
		::SetControl32BitValue(controlRef,inBool);
	}
#endif
}

/**
Color取得
*/
void	CWindowImp::GetColor( const AControlID inID, AColor& outColor ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPaneConst(inID).GetColor(outColor);
		return;
	}
	
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//値設定
	//ColorWellの場合
	if( [view isKindOfClass:[NSColorWell class]] == YES )
	{
		NSColorWell*	colorwell = static_cast<NSColorWell*>(view);
		//
		NSColor*	color = [[colorwell color] colorUsingColorSpaceName:NSCalibratedRGBColorSpace];//#945
		outColor.red	= 65535 * [color redComponent];
		outColor.green	= 65535 * [color greenComponent];
		outColor.blue	= 65535 * [color blueComponent];
	}
	//上記以外はエラー
	else
	{
		_ACError("",NULL);
	}
}

/**
Color設定
*/
void	CWindowImp::SetColor( const AControlID inID, const AColor& inColor )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetColor(inColor);
		return;
	}
	
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//値設定
	//ColorWellの場合
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
	//上記以外はエラー
	else
	{
		_ACError("",NULL);
	}
}

/**
Number取得
*/
void	CWindowImp::GetNumber( const AControlID inID, ANumber& outNumber ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
#if SUPPORT_CARBON
//#205 #if USE_IMP_TABLEVIEW
	//TebleViewの場合は選択中の行のインデックスを返す
	if( IsTableView(inID) == true )
	{
		outNumber = GetTableViewConstByID(inID).GetSelect();
		return;
	}
//#205 #endif
#endif
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPaneConst(inID).GetNumber(outNumber);
		return;
	}
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//値取得
		//ポップアップボタンの場合
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			//選択indexを取得
			outNumber = [popup indexOfSelectedItem];
			//pull downの場合は、メニューは1始まりなので－１する
			if( [popup pullsDown] == YES )
			{
				outNumber--;
			}
		}
		//NSScrollerの場合
		else if( [view isKindOfClass:[NSScroller class]] == YES )
		{
			outNumber = 0;
			NSScroller*	scroller = static_cast<NSScroller*>(view);
			//値を取得（scrollerの値は0-0-1.0の値になっているので、実際の値に変換）
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
		//★ボタン内メニューに対応する場合は対応必要
		//コントロールの場合
		else if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			//intValueを取得
			outNumber = [control intValue];
		}
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
			outNumber = 0;
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		OSStatus	err = noErr;
		
		//通常コントロールの場合
		AControlRef	controlRef = GetControlRef(inID);
		ControlKind	kind;
		err = ::GetControlKind(controlRef,&kind);
		if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
		//ラジオボタングループ、ポップアップメニューボタンの場合は選択中の項目番号を0～の数に変換して返す
		if( kind.kind == kControlKindRadioGroup || kind.kind == kControlKindPopupButton )
		{
			outNumber = ::GetControl32BitValue(controlRef)-1;
			return;
		}
		//BevelButtonかつメニューが存在する場合
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
		//その他コントロールの場合
		outNumber = ::GetControl32BitValue(controlRef);
	}
#endif
}

/**
Number取得
*/
ANumber	CWindowImp::GetNumber( const AControlID inID ) const
{
	ANumber	number;
	GetNumber(inID,number);
	return number;
}

/**
FloatNumber取得
*/
void	CWindowImp::GetFloatNumber( const AControlID inID, AFloatNumber& outNumber ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPaneConst(inID).GetFloatNumber(outNumber);
		return;
	}
	
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//値取得
	if( [view isKindOfClass:[NSControl class]] == YES )
	{
		NSControl*	control = ACocoa::CastNSControlFromId(view);
		//floatValueを取得
		outNumber = [control floatValue];
	}
	//上記以外はエラー
	else
	{
		_ACError("",NULL);
		outNumber = 0;
	}
}

/**
Number設定
*/
void	CWindowImp::SetNumber( const AControlID inID, const ANumber inNumber )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
#if SUPPORT_CARBON
//#205 #if USE_IMP_TABLEVIEW
	//TebleViewの場合は行を選択
	if( IsTableView(inID) == true )
	{
		GetTableViewByID(inID).SetSelect(inNumber);
		return;
	}
//#205 #endif
#endif
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetNumber(inNumber);
		return;
	}
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//値設定
		//NSPopUpButtonの場合
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			//inNumber番目のメニュー項目を選択
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			if( [popup pullsDown] == YES )
			{
				//==================pull downメニューの場合==================
				//pull downメニューの場合は、指定番号のテキストを取得して、表示タイトル（=index:0）に設定
				AText	text;
				if( inNumber >= 0 )
				{
					AMenuWrapper::GetMenuItemText([popup menu],inNumber+1,text);
					AMenuWrapper::SetMenuItemText([popup menu],0,text);
					//指定番号の項目を選択
					[popup selectItemAtIndex:inNumber+1];
				}
				//指定番号の項目にチェックマークを付ける
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
				//==================pull downメニューではない場合==================
				//指定番号の項目を選択
				[popup selectItemAtIndex:inNumber];
			}
		}
		//NSScrollerの場合
		else if( [view isKindOfClass:[NSScroller class]] == YES )
		{
			//scrollerの最小／最大値を取得し、0.0-1.0の値に変換する
			AIndex	index = mMinMaxArray_ControlID.Find(inID);
			if( index != kIndex_Invalid )
			{
				ANumber	min = mMinMaxArray_Min.Get(index);
				ANumber	max = mMinMaxArray_Max.Get(index);
				if( min < max )
				{
					//0.0-1.0に正規化して設定
					AFloatNumber	num = inNumber;
					num = num / (max-min);
					NSScroller*	scroller = ACocoa::CastNSScrollerFromId(view);
					[scroller setDoubleValue:num];
				}
				else
				{
					//min>=maxなので、scrollerはSetMinMax()でdisable状態にされている。処理無し
				}
			}
			else
			{
				_ACError("",NULL);
			}
		}
		//★ボタン内メニューに対応する場合は対応必要
		//NSControlの場合
		else if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			//#1225 [control setIntValue:inNumber];
			AText	numberText;
			numberText.SetNumber(inNumber);
			AStCreateNSStringFromAText	str(numberText);
			[control setStringValue:str.GetNSString()];
		}
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロールの場合
		OSStatus	err = noErr;
		
		AControlRef	controlRef = GetControlRef(inID);
		ControlKind	kind;
		err = ::GetControlKind(controlRef,&kind);
		if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
		//プログレスバーの場合は進捗値として設定、ただし、マイナスの場合はinterminateとして設定
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
		//ラジオボタングループ、ポップアップメニューボタンの場合はインデックスがinNumberの項目を選択（ただし+1変換）
		if( kind.kind == kControlKindRadioGroup || kind.kind == kControlKindPopupButton )
		{
			::SetControl32BitValue(controlRef,inNumber+1);
			return;
		}
		//BevelButtonかつメニューが存在する場合
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
		//その他コントロールの場合
		::SetControl32BitValue(controlRef,inNumber);
	}
#endif
}

/**
FloatNumber設定
*/
void	CWindowImp::SetFloatNumber( const AControlID inID, const AFloatNumber inNumber )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetFloatNumber(inNumber);
		return;
	}
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//値設定
		//コントロールの場合
		if( [view isKindOfClass:[NSControl class]] == YES )
		{
			NSControl*	control = ACocoa::CastNSControlFromId(view);
			[control setFloatValue:inNumber];
		}
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
		}
	}
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロール未対応
		_ACError("not yet implemented",this);
	}
}

//フォント取得
/*win void	CWindowImp::GetFont( const AControlID inID, AFont& outFontNum ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	outFontNum = ACarbonMenuWrapper::GetFontByMenuItemIndex(GetMenuRef(inID),GetNumber(inID));
}*/

//フォント設定
/*win void	CWindowImp::SetFont( const AControlID inID, AFont inFontNum )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	SetNumber(inID,ACarbonMenuWrapper::GetMenuItemIndexByFont(GetMenuRef(inID),inFontNum));
}*/

//#688
/**
コントロールにフォント設定
*/
void	CWindowImp::SetFont( const AControlID inID, const AText& inFontName, const AFloatNumber inFontSize )
{
	AStCreateNSStringFromAText	fontnamestr(inFontName);
	NSFont*	font = [NSFont fontWithName:fontnamestr.GetNSString() size:inFontSize];
	
	//指定フォントが見つからない場合はデフォルトフォントを取得
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
	
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//NSControlなら、フォント設定
	if( [view isKindOfClass:[NSControl class]] == YES )
	{
		NSControl*	control = ACocoa::CastNSControlFromId(view);
		[control setFont:font];
	}
	//上記以外はエラー
	else
	{
		_ACError("",NULL);
	}
}

//#688
/**
コントロールからフォント取得
*/
void	CWindowImp::GetFont( const AControlID inID, AText& outFontName, AFloatNumber& outFontSize ) const
{
	//フォントパネルのvirtual control idの場合は、フォントパネルで選択中のフォントを取得
	if( inID == mFontPanelVirtualControlID )
	{
		NSFont*	font = [CocoaObjects->GetWindow() getFontPanelFont];
		ACocoa::SetTextFromNSString([font fontName],outFontName);
		outFontSize = [font pointSize];
		return;
	}
	
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//CocoaFontFieldならフォント取得（フォントサイズは表示フォントに上限かけているので、専用メソッドで取得する）
	if( [view isKindOfClass:[CocoaFontField class]] == YES )
	{
		CocoaFontField*	fontField = static_cast<CocoaFontField*>(view);
		ACocoa::SetTextFromNSString([[fontField font] fontName],outFontName);
		outFontSize = [fontField getFontSizeData];
	}
	//NSControlなら、フォント取得
	else if( [view isKindOfClass:[NSControl class]] == YES )
	{
		NSControl*	control = ACocoa::CastNSControlFromId(view);
		ACocoa::SetTextFromNSString([[control font] fontName],outFontName);
		outFontSize = [[control font] pointSize];
	}
	//上記以外はエラー
	else
	{
		_ACError("",NULL);
	}
}

/**
空を設定
*/
void	CWindowImp::SetEmptyValue( const AControlID inID )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetEmptyValue();
		return;
	}
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//0を設定
		SetNumber(inID,0);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロールの場合
		AControlRef	controlRef = GetControlRef(inID);
		::SetControl32BitValue(controlRef,0);
	}
#endif
}

/**
Icon設定
*/
void	CWindowImp::SetIcon( const AControlID inControlID, const AIconID inIconID, 
		const ABool inDefaultSize, const ANumber inLeftOffset, const ANumber inTopOffset )//win 080721
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		//処理なし
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
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
Icon設定
*/
/*
void	CWindowImp::SetIconByImageIndex( const AControlID inControlID, const AIndex inImageIndex )
{
	OSStatus	err = noErr;
	
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	ControlButtonContentInfo	info;
	info.contentType = kControlContentPictHandle;
	info.u.picture = GetImage(inImageIndex);
	err = ::SetControlData(GetControlRef(inControlID),kControlEntireControl,kControlBevelButtonContentTag,sizeof(info),&info);
	if( err != noErr )   _ACErrorNum("SetControlData() returned error: ",err,this);
}
*/


/**
最大値／最小値を設定
*/
void	CWindowImp::SetMinMax( const AControlID inControlID, const ANumber inMin, const ANumber inMax )
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//MinMaxをメンバ変数に保存する
		ABool	changed = false;
		AIndex	index = mMinMaxArray_ControlID.Find(inControlID);
		if( index == kIndex_Invalid )
		{
			index = mMinMaxArray_ControlID.Add(inControlID);
			mMinMaxArray_Min.Add(inMin);
			mMinMaxArray_Max.Add(inMax);
			//変更フラグON
			changed = true;
		}
		if( mMinMaxArray_Min.Get(index) != inMin || mMinMaxArray_Max.Get(index) != inMax )
		{
			mMinMaxArray_Min.Set(index,inMin);
			mMinMaxArray_Max.Set(index,inMax);
			//変更フラグON
			changed = true;
		}
		
		//変更があれば、スクロールバーのenable/disableを更新
		if( changed == true )
		{
			//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
			NSView*	view = CocoaObjects->FindViewByTag(inControlID);
			
			//NSScrollerの場合、inMin>=inMaxならスクロールはdisable
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
		//Carbonウインドウの場合
		
		::SetControl32BitMinimum(GetControlRef(inControlID),inMin);
		::SetControl32BitMaximum(GetControlRef(inControlID),inMax);
	}
#endif
}

//#725
/**
スクロールバーのpage unitを設定
*/
void	CWindowImp::SetScrollBarPageUnit( const AControlID inControlID, const ANumber inUnit )
{
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inControlID);
	
	//NSScrollerの場合、inMin>=inMaxならスクロールはdisable
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
コントロールにメニューを設定
*/
void	CWindowImp::SetControlMenu( const AControlID inControlID, const AMenuRef inMenuRef )
{
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inControlID);
	
	//NSPopUpButtonの場合、TextArrayの内容をメニュー項目として追加
	if( [view isKindOfClass:[NSPopUpButton class]] == YES )
	{
		NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
		
		[popup setMenu:(NSMenu*)inMenuRef];
	}
}

//#688
/**
フォントパネル表示
*/
void	CWindowImp::ShowControlFontPanel( const AControlID inControlID )
{
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inControlID);
	
	if( [view isKindOfClass:[CocoaFontField class]] == YES )
	{
		CocoaFontField*	fontField = static_cast<CocoaFontField*>(view);
		[fontField doFontChangeButton:nil];
	}
}

#pragma mark ===========================

#pragma mark ---コントロール内選択インターフェイス

/**
テキスト選択
*/
void	CWindowImp::SetSelection( const AControlID inID, const AIndex inStart, const AIndex inEnd )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetSelection(inStart,inEnd);
		return;
	}
	//通常コントロールの場合
	//未実装
	_ACError("not implemented",NULL);
}

/**
テキスト選択（全選択）
*/
void	CWindowImp::SetSelectionAll( const AControlID inID )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetSelectionAll();
		return;
	}
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSTextFieldの場合
		if( [view isKindOfClass:[NSTextField class]] == YES )
		{
			NSTextField*	textfield = ACocoa::CastNSTextFieldFromId(view);
			[textfield selectText:nil];
		}
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//通常コントロールの場合
		//テキスト系の場合のみ実行#353
		OSStatus	err = noErr;
		
		ControlKind	kind;
		err = ::GetControlKind(GetControlRef(inID),&kind);
		if( kind.kind == kControlKindEditText || kind.kind == kControlKindHIComboBox || 
					kind.kind == kControlKindEditUnicodeText )
		{
			//kControlEditTextSelectionTag設定により全選択実行
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

#pragma mark ---コントロール生成／削除

#if SUPPORT_CARBON
/**
ベベルボタン生成
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
		
		//OS10.3バグ対策
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
AView_Buttonを作ったため不要
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
		
		//OS10.3バグ対策
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
Iconボタン生成
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
Rounded Rectボタン生成
*/
void	CWindowImp::CreateRoundedRectButton( const AControlID inID, 
		const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex )
{
	//ウインドウ内の生成位置frameを計算
	AWindowRect	wrect = {0};
	wrect.left	= inPoint.x;
	wrect.top	= inPoint.y;
	wrect.right	= inPoint.x + inWidth;
	wrect.bottom= inPoint.y + inHeight;
	NSRect	rect = ConvertFromWindowRectToNSRect(CocoaObjects->GetContentView(),wrect);
	//button生成
	NSButton*	button = [[NSButton alloc] initWithFrame:rect];
	[button setButtonType:NSMomentaryPushInButton];
	[button setBezelStyle:NSShadowlessSquareBezelStyle];//NSSmallSquareBezelStyle];
	AText	title;
	AStCreateNSStringFromAText	titlestr(title);
	[button setTitle:titlestr.GetNSString()];
	[CocoaObjects->GetContentView() addSubview:button positioned:NSWindowAbove relativeTo:nil];
	[button release];//addSubview:でcontent viewにretainされ、アプリ側でポインタも保持しないのでこちらはreleaseする。
	[button setTag:inID];
}

/*
NSRegularSquareBezelStyle: Rounded、aqua系だがちょっと違う。使う必要なし。
NSShadowlessSquareBezelStyle: 角いボタン。gradient無し。
NSCircularBezelStyle: 円形ボタン。
NSTexturedSquareBezelStyle: Rounded。texturedという名称だが古い感じ。使う必要なし。
NSSmallSquareBezelStyle: 角いボタン。gradient付き。
NSTexturedRoundedBezelStyle: Rounded。Safariタイプツールバーのボタンと同じ。高さ固定(22?)。
NSRoundRectBezelStyle : Rounded。Finderの「別名で接続」ボタンと同じ。高さ固定。

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
プログレスindicator生成
*/
void	CWindowImp::CreateProgressIndicator( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex )
{
	//ウインドウ内の生成位置frameを計算
	AWindowRect	wrect = {0};
	wrect.left	= inPoint.x;
	wrect.top	= inPoint.y;
	wrect.right	= inPoint.x + inWidth;
	wrect.bottom= inPoint.y + inHeight;
	NSRect	rect = ConvertFromWindowRectToNSRect(CocoaObjects->GetContentView(),wrect);
	//progress indicator生成
	CocoaProgressIndicator*	progress = [[CocoaProgressIndicator alloc] initWithFrame:rect];
	[CocoaObjects->GetContentView() addSubview:progress positioned:NSWindowAbove relativeTo:nil];
	[progress release];//addSubview:でcontent viewにretainされ、アプリ側でポインタも保持しないのでこちらはreleaseする。
	[progress setTag:inID];
	//
	[progress setStyle:NSProgressIndicatorSpinningStyle];
	[progress setControlSize:NSSmallControlSize];
	[progress setBezeled:NO];
}

/**
スクロールバー生成
*/
void	CWindowImp::CreateScrollBar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex )//win 080618
{
	if( true )
	{
		//Cocoaウインドウの場合
		
		//ウインドウ内の生成位置frameを計算
		AWindowRect	wrect = {0};
		wrect.left	= inPoint.x;
		wrect.top	= inPoint.y;
		wrect.right	= inPoint.x + inWidth;
		wrect.bottom= inPoint.y + inHeight;
		NSRect	rect = ConvertFromWindowRectToNSRect(CocoaObjects->GetContentView(),wrect);
		//scroller生成
		NSScroller*	scroller = [[NSScroller alloc] initWithFrame:rect];
		[CocoaObjects->GetContentView() addSubview:scroller positioned:NSWindowAbove relativeTo:nil];
		[scroller release];//addSubview:でcontent viewにretainされ、アプリ側でポインタも保持しないのでこちらはreleaseする。
		[scroller setTag:inID];
		//scrollerのサイズタイプを設定（SetControlSize()でも設定される
		if( inWidth < 15 || inHeight < 15 )
		{
			[scroller setControlSize:NSSmallControlSize];
		}
		else
		{
			[scroller setControlSize:NSRegularControlSize];
		}
		//target/action設定
		[scroller setTarget:CocoaObjects->GetWindow()];
		[scroller setAction:@selector(doScrollbarAction:)];
		//最小・最大値初期化
		SetMinMax(inID,0,0);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
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
スクロールバー登録（すでにリソースに存在する場合用）
*/
void	CWindowImp::RegisterScrollBar( const AControlID inID )
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//処理なし
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		::SetControlAction(GetControlRef(inID),::NewControlActionUPP(STATIC_APICB_ScrollBarActionProc));
	}
#endif
}

//R0191
/**
SearchField生成
*/
/*使用する予定なし
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
Placard生成
*/
/*使用する予定なし
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
コントロール削除
*/
void	CWindowImp::DeleteControl( const AControlID inID )
{
	//#212
	//現在のフォーカスを削除するときは、フォーカス解除する
	if( /*#688 mCurrentFocus*/GetCurrentFocus() == inID )
	{
		ClearFocus();
	}
	//TextArrayMenuの登録解除
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
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//MinMax保存データの削除
		AIndex	index = mMinMaxArray_ControlID.Find(inID);
		if( index != kIndex_Invalid )
		{
			mMinMaxArray_ControlID.Delete1(index);
			mMinMaxArray_Min.Delete1(index);
			mMinMaxArray_Max.Delete1(index);
		}
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//ControlIDのキャッシュをクリア
		CocoaObjects->RemoveFromControlIDCache(inID);
		
		//viewを親viewから外す。removeFromSuperview内部で1回releaseされる。（addSubViewによるretainに対応）
		//UserPane以外ならこれにより削除される。
		//UserPaneならまだCocoaObjectsでretainしているので、UnregisterUserPaneView()により削除される。
		[view removeFromSuperview];
		
		//UserPaneの削除
		AIndex	userpaneindex = mUserPaneArray_ControlID.Find(inID);
		if( userpaneindex != kIndex_Invalid )
		{
			mUserPaneArray.Delete1Object(userpaneindex);
			mUserPaneArray_ControlID.Delete1(userpaneindex);
			mUserPaneArray_Type.Delete1(userpaneindex);
			//#688 mUserPaneArray_ControlRef.Delete1(userpaneindex);//#688
			
			//CocoaObjectsのコントロール登録解除
			CocoaObjects->UnregisterUserPaneView(inID);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		//UserPaneの削除
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
スクロールバー操作時のCallback
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
			//処理無し
			break;
		}
	}
}

/**
スクロールバー操作時のCallback(static)
*/
void	CWindowImp::STATIC_APICB_ScrollBarActionProc( ControlRef theControl, ControlPartCode partCode )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-SBAP)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return;
	//イベントトランザクション前処理／後処理
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

#pragma mark ---キーボードフォーカス管理

/*#135
//キーボードフォーカスのグループに登録
void	CWindowImp::RegisterFocusGroup( const AControlID inControlID )
{
	mFocusGroup.Add(inControlID);
}
*/

/**
キーボードフォーカスを設定
*/
void	CWindowImp::SetFocus( const AControlID inControlID )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//現在のフォーカスと同じならリターン
	if( inControlID == /*#688 mCurrentFocus*/GetCurrentFocus() )   return;
	
	//
	//#135 if( mFocusGroup.Find(inControlID) == kControlID_Invalid )   return;
	
	//現在のフォーカスをクリアする
	//#688 下で必ず設定されるので、クリアする必要なし。ClearFocus();
	
	/*#688
	//mCurrentFocusを設定
	mCurrentFocus = inControlID;
	mLatentFocus = inControlID;//#0 kControlID_Invalid;
	*/
	
#if SUPPORT_CARBON
	//#668 フォーカスはOS制御にするため、UserPaneであっても、SetFocus()は直接実行しない。
	//EVT_DoViewFocusActivated()/EVT_DoViewFocusDeactivated()は、
	//APICB_CocoaViewBecomeFirstResponder()/APICB_CocoaViewResignFirstResponder()経由で実行される。
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inControlID) == true )
	{
		OSStatus	err = noErr;
		
		//OSのKeyboardFocusはクリア
		err = ::ClearKeyboardFocus(mWindowRef);
		if( err != noErr )   _ACErrorNum("ClearKeyboardFocus() returned error: ",err,this);
		//UserPaneのSetFocus()実行
		GetUserPane(inControlID).SetFocus();
		return;
	}
#endif
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inControlID);
		
		//viewをFirstResponderに設定
		[CocoaObjects->GetWindow() makeFirstResponder:view];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		OSStatus	err = noErr;
		
		//通常コントロールの場合
		ControlKind	kind;
		err = ::GetControlKind(GetControlRef(inControlID),&kind);
		//if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
		if( kind.kind == kControlKindEditText || kind.kind == kControlKindHIComboBox || 
					kind.kind == kControlKindEditUnicodeText ||
					kind.kind == kControlKindDataBrowser /* ||
			true*/	)
		{
			//OSのKeyboardFocusを設定
			err = ::SetKeyboardFocus(mWindowRef,GetControlRef(inControlID),kControlFocusNextPart);
			if( err != noErr )   _ACErrorNum("SetKeyboardFocus() returned error: ",err,this);
		}
		//#353　ボタン等にもフォーカスを当てる対応（★上記でわざわざコントロールを限定していた理由？）
		else
		{
			//OSのKeyboardFocusを設定
			err = ::SetKeyboardFocus(mWindowRef,GetControlRef(inControlID),kControlFocusNextPart);
			//	if( err != noErr )   _ACErrorNum("SetKeyboardFocus() returned error: ",err,this);
		}
	}
#endif
}

/**
前のViewにフォーカス設定
*/
void	CWindowImp::SetFocusPrev()
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//前のviewをフォーカスに設定
	[CocoaObjects->GetWindow() selectPreviousKeyView:nil];
}

/**
次のViewにフォーカス設定
*/
void	CWindowImp::SetFocusNext()
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//次のviewをフォーカスに設定
	[CocoaObjects->GetWindow() selectNextKeyView:nil];
}

/**
フォーカスをクリア
*/
void	CWindowImp::ClearFocus( const ABool inAlsoClearLatentFocus )//#212
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
#if SUPPORT_CARBON
	//#668 フォーカスはOS制御にするため、UserPaneであっても、SetFocus()は直接実行しない。
	//EVT_DoViewFocusActivated()/EVT_DoViewFocusDeactivated()は、
	//APICB_CocoaViewBecomeFirstResponder()/APICB_CocoaViewResignFirstResponder()経由で実行される。
	//UserPaneの場合は対応メソッドをコール
	if( /*#688 mCurrentFocus*/GetCurrentFocus() != kControlID_Invalid )
	{
		if( IsUserPane(/*#688 mCurrentFocus*/GetCurrentFocus()) == true )
		{
			GetUserPane(/*#688 mCurrentFocus*/GetCurrentFocus()).ResetFocus();
		}
	}
#endif
	//OSのKeyboardFocusをクリア
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//ウインドウをFirst Responderにする
		[CocoaObjects->GetWindow() makeFirstResponder:nil];
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		OSStatus	err = noErr;
		
		err = ::ClearKeyboardFocus(mWindowRef);
		if( err != noErr )   _ACErrorNum("ClearKeyboardFocus() returned error: ",err,this);
	}
	/*#688
	//mCurrentFocusをクリア
	mCurrentFocus = kControlID_Invalid;
	//inAlsoClearLatentFocusがtrueならlatentもクリア（デフォルトはlatentもクリア。ただし、deactivatedからコールされる場合はlatentは残す） #212
	if( inAlsoClearLatentFocus == true )
	{
		mLatentFocus = kControlID_Invalid;
	}
	*/
#endif
}

/*#135
//最初のフォーカスに設定する
void	CWindowImp::SwitchFocusToFirst()
{
	if( mFocusGroup.GetItemCount() == 0 )   return;
	SwitchFocusTo(mFocusGroup.Get(0));
}


//次のフォーカスへ進める
void	CWindowImp::SwitchFocusToNext()
{
	if( mFocusGroup.GetItemCount() == 0 )   return;
	
	//次のFocus候補をfocusIndexに入れる
	//現在Focus未選択の場合はFocus候補は0
	AIndex	focusIndex = 0;
	if( mCurrentFocus != kControlID_Invalid )
	{
		//現在のFocusのインデックスを検索
		focusIndex = mFocusGroup.Find(mCurrentFocus);
		
		if( focusIndex == kIndex_Invalid )
		{
			//現在のFocusのインデックスが見つからない場合（ふつうはありえない）
			_AError("focus not found",this);
			focusIndex = 0;
		}
		else
		{
			//focusIndexを次に進める
			focusIndex++;
			if( focusIndex >= mFocusGroup.GetItemCount() )
			{
				focusIndex = 0;
			}
		}
	}
	SwitchFocusTo(mFocusGroup.Get(focusIndex));
}

//前のフォーカスへ戻す B0429
void	CWindowImp::SwitchFocusToPrev()
{
	if( mFocusGroup.GetItemCount() == 0 )   return;
	
	//前のFocus候補をfocusIndexに入れる
	//現在Focus未選択の場合はFocus候補はmFocusGroup.GetItemCount()-1
	AIndex	focusIndex = mFocusGroup.GetItemCount()-1;
	if( mCurrentFocus != kControlID_Invalid )
	{
		//現在のFocusのインデックスを検索
		focusIndex = mFocusGroup.Find(mCurrentFocus);
		
		if( focusIndex == kIndex_Invalid )
		{
			//現在のFocusのインデックスが見つからない場合（ふつうはありえない）
			_AError("focus not found",this);
			focusIndex = 0;
		}
		else
		{
			//focusIndexを前へ戻す
			focusIndex--;
			if( focusIndex < 0 )
			{
				focusIndex = mFocusGroup.GetItemCount()-1;
			}
		}
	}
	SwitchFocusTo(mFocusGroup.Get(focusIndex),true);
}

//フォーカスを指定ControlIDのコントロールに切り替える
//public
void	CWindowImp::SwitchFocusTo( const AControlID inControlID, const ABool inReverseSearch )//B0429
{
	//ウインドウ生成済みチェック
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
			//inControlIDから順に、EnableかつVisibleなコントロールが見つかるまで検索し、見つかったらそこへfocusを設定
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
				else//B0429 フォーカスを前へ戻す場合は、有効なコントロールを前方向に探す必要がある
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
	//EnableかつVisibleなコントロールがない場合はフォーカスをクリア
	ClearFocus();
}
*/

#if SUPPORT_CARBON
//#688 Cocoa対応により削除。OSでの制御に任せる
/**
フォーカスのコントロールがDisableされた場合、または、フォーカス無しの状態からフォーカス可能なコントロールがEnableされた場合の処理
*/
void	CWindowImp::UpdateFocus()
{
	//ウインドウ生成済みチェック
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
現在のフォーカス取得
*/
AControlID	CWindowImp::GetCurrentFocus() const
{
	if( true )
	{
		//Cocoaウインドウの場合
		
		NSResponder*	responder = [CocoaObjects->GetWindow() firstResponder];
		if( responder != nil )
		{
			//firstResponderが存在していて、viewであれば、tagを返す
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
		//Carbonウインドウの場合
		
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

#pragma mark --- タブView管理

/**
タブViewのUpdate
*/
void	CWindowImp::UpdateTab()
{
	if( true )
	{
		//Cocoaウインドウの場合
		
		//処理なし
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		OSStatus	err = noErr;
		
		//ウインドウ生成済みチェック
		if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
		
		AControlRef	tabViewRootControlRef = GetControlRef('ttab',0);
		if( tabViewRootControlRef == NULL )   return;
		
		//キーボードフォーカスを一旦クリア
		ClearFocus();
		
		//選択されたタブのインデックスを取得
		AIndex	selectedTabIndex = ::GetControl32BitValue(tabViewRootControlRef)-1;
		//選択されていないタブを非表示
		ANumber	tabMax = ::GetControl32BitMaximum(tabViewRootControlRef);
		for( AIndex tabIndex = 0; tabIndex < tabMax; tabIndex++ )
		{
			AControlRef	tabControlRef = GetTabControlRef(tabIndex);
			if( tabIndex != selectedTabIndex && tabControlRef != NULL )
			{
				//タブごと非表示
				err = ::SetControlVisibility(tabControlRef,false,false);
				if( err != noErr )   _ACErrorNum("SetControlVisibility() returned error: ",err,this);
			}
		}
		//選択されたタブを表示
		AControlRef	selectedTabRef = GetTabControlRef(selectedTabIndex);
		if( selectedTabRef != NULL )
		{
			//タブごと表示
			err = ::SetControlVisibility(selectedTabRef,true,true);
			if( err != noErr )   _ACErrorNum("SetControlVisibility() returned error: ",err,this);
		}
		::Draw1Control(tabViewRootControlRef);
		
		//TabのShow/Hideに合わせて、MLTEPaneをShow/Hideする
		//（HideしたTabに含まれるMLTEのスクロールバーが見えてしまう問題の対策）
		for( AIndex i = 0; i < mUserPaneArray_ControlID.GetItemCount(); i++ )
		{
			AControlID	controlID = mUserPaneArray_ControlID.Get(i);
			if( GetUserPaneType(controlID) == kUserPaneType_MLTEPane )
			{
				ABool	show = IsEmbededInTabView(controlID,selectedTabIndex);
				SetShowControl(controlID,show);
			}
		}
		
		//キーボードフォーカスを設定
		GetAWin().NVI_SwitchFocusToFirst();
	}
#endif
}

//指定ControlIDのコントロールが指定Tabに含まれているかを取得する
ABool	CWindowImp::IsEmbededInTabView( const AControlID inChildControlID, const AIndex inTabIndex ) const
{
	if( true )
	{
		//Cocoaウインドウの場合
		
		//処理なし
		return false;
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		OSStatus	err = noErr;
		
		//ウインドウ生成済みチェック
		if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
		
		if( inChildControlID == kControlID_Invalid )   return false;
		AControlRef	current = GetControlRef(inChildControlID);
		while( current != NULL )
		{
			//ControlRefがTabのControlRefに一致するかどうか判定
			if( current == GetTabControlRef(inTabIndex) )
			{
				return true;
			}
			//親コントロールを取得
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
指定TabIndex(0～)のタブのControlRefを取得する
*/
AControlRef	CWindowImp::GetTabControlRef( const AIndex inTabIndex ) const
{
	return GetControlRef('ttab',inTabIndex+1);
}
#endif

//B0406
/**
タブControl選択
*/
void	CWindowImp::SetTabControlValue( const AIndex inTabIndex )
{
	if( true )
	{
		//Cocoaウインドウの場合
		
		//tab view item取得
		NSTabView*	tabView = [CocoaObjects->GetWindow() getTabView];
		[tabView selectTabViewItemAtIndex:inTabIndex];
		
		//タブを選択
		//（tab view itemのidentifierには、対応するツールバー項目のcontrol id(tag)が入っている。）
		AText	numtext;
		ACocoa::SetTextFromNSString([[CocoaObjects->GetWindow() toolbar] selectedItemIdentifier],numtext);
		if( numtext.GetNumber() != kControlID_FirstTabSelectorToolbarItem+inTabIndex )
		{
			numtext.SetNumber(kControlID_FirstTabSelectorToolbarItem+inTabIndex);
			AStCreateNSStringFromAText	numstr(numtext);
			[[CocoaObjects->GetWindow() toolbar] setSelectedItemIdentifier:numstr.GetNSString()];
		}
		
		//対応するbottom markerに従って、ウインドウサイズを更新
		OptimizeWindowBounds(kControlID_FirstTabBottoMarker+inTabIndex,kControlID_Invalid);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
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

#pragma mark --- ツールバー

#pragma mark ===========================

#pragma mark --- WebView

//#734
/**
WebView生成
*/
void	CWindowImp::CreateWebView( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	//ウインドウ内の生成位置frameを計算
	AWindowRect	wrect = {0};
	wrect.left	= inPoint.x;
	wrect.top	= inPoint.y;
	wrect.right	= inPoint.x + inWidth;
	wrect.bottom= inPoint.y + inHeight;
	NSRect	rect = ConvertFromWindowRectToNSRect(CocoaObjects->GetContentView(),wrect);
	//webview生成
	CocoaWebView*	webview = [[CocoaWebView alloc] initWithFrame:rect frameName:nil groupName:nil withControlID:inID];
	[CocoaObjects->GetContentView() addSubview:webview positioned:NSWindowAbove relativeTo:nil];
	[webview release];//addSubview:でcontent viewにretainされ、アプリ側でポインタも保持しないのでこちらはreleaseする。
	/*test（ビュー全体の拡大縮小）
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
WebViewにURL設定し、ロード要求
*/
void	CWindowImp::LoadURLToWebView( const AControlID inID, const AText& inURL )
{
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//loadRequest送信
	AText	url;
	url.SetText(inURL);
	url.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToHFSPlusDecomp(url);//urlをHFS decompに変換
	url.ConvertToUTF8FromUTF16();
	url.ConvertToURLEncode();//urlエンコード
	AStCreateNSStringFromAText	str(url);
	[[view mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:str.GetNSString()]]];
}

//#734
/**
WebViewリロード要求
*/
void	CWindowImp::ReloadWebView( const AControlID inID )
{
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	/*テスト
	倍率設定
	NSRect	bounds = [view bounds];
	NSRect	frame = [view frame];
	NSRect	bounds2 = [[[view mainFrame] frameView] bounds];
	NSRect	frame2 = [[[view mainFrame] frameView] frame];
	bounds2.size.width = frame2.size.width*1.7;
	bounds2.size.height = frame2.size.height*1.7;
	[[[view mainFrame] frameView] setBounds:bounds2];
	*/
	
	//リロード要求
	[view reload:nil];
}

//#734
/**
WebView現在のURLを取得
*/
void	CWindowImp::GetCurrentWebViewURL( const AControlID inID, AText& outURL ) const
{
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	
	//URL取得
	ACocoa::SetTextFromNSString([view mainFrameURL],outURL);
}

//#734
/**
Web view内でJavaScriptを実行
*/
void	CWindowImp::ExecuteJavaScriptInWebView( const AControlID inID, const AText& inText )
{
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	if( [view isKindOfClass:[CocoaWebView class]] == YES )
	{
		CocoaWebView*	webview = static_cast<CocoaWebView*>(view);
		//URL取得
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
WebViewのフォントサイズ（拡大率）設定
*/
void	CWindowImp::SetWebViewFontMultiplier( const AControlID inID, const AFloatNumber inMultiplier )
{
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inID);
	if( [view isKindOfClass:[CocoaWebView class]] == YES )
	{
		CocoaWebView*	webview = static_cast<CocoaWebView*>(view);
		//URL取得
		[webview setTextSizeMultiplier:inMultiplier];
	}
	else
	{
		_ACError("",NULL);
	}
}

#pragma mark ===========================

#pragma mark --- UserPane管理
/**
指定ControlIDのコントロールがUserPaneかどうかを判定
*/
ABool	CWindowImp::IsUserPane( const AControlID inControlID ) const
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	return (index != kIndex_Invalid);
}

/**
指定ControlIDのUserPaneを取得
*/
CUserPane&	CWindowImp::GetUserPane( const AControlID inControlID )
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )   _ACThrow("no UserPane",this);
	return mUserPaneArray.GetObject(index);
}

/**
指定ControlIDのUserPaneを取得(const)
*/
const CUserPane&	CWindowImp::GetUserPaneConst( const AControlID inControlID ) const
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )   _ACThrow("no UserPane",this);
	return mUserPaneArray.GetObjectConst(index);
}

/**
指定ControlIDのUserPaneタイプを取得
*/
AUserPaneType	CWindowImp::GetUserPaneType( const AControlID inControlID ) const
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )   return kUserPaneType_Invalid;
	return mUserPaneArray_Type.Get(index);
}

#if SUPPORT_CARBON
/**
MLTEPaneを登録（OSのウインドウ上には生成済み）
*/
void	CWindowImp::RegisterTextEditPane( const AControlID inID, const ABool inWrapMode , const ABool inVScrollbar, const ABool inHScrollBar , const ABool inHasFrame )
{
	AControlRef	controlRef = GetControlRef(inID);//#688
	CMLTEPaneFactory	factory(this,*this,/*#688GetControlRef(inID)*/inID,inWrapMode,inVScrollbar,inHScrollBar,inHasFrame);
	mUserPaneArray.AddNewObject(factory);
	mUserPaneArray_ControlID.Add(inID);
	mUserPaneArray_Type.Add(kUserPaneType_MLTEPane);
	mUserPaneArray_ControlRef.Add(controlRef);//#688
	
	//キーボードフォーカスグループへ登録
	//#135 AWindowで実行 RegisterFocusGroup(inID);
}
#endif

//B0323
/**
Undo情報クリア
*/
void	CWindowImp::ResetUndoStack( const AControlID inID )
{
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).ResetUndoStack();
	}
	//処理なし
}

/**
ColorPickerPaneを登録（OSのウインドウ上には生成済み）
*/
void	CWindowImp::RegisterColorPickerPane( const AControlID inID )
{
	if( true )
	{
		//処理なし
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
AView用UserPaneを登録（OSのウインドウ上には生成済み）
*/
void	CWindowImp::RegisterPaneForAView( const AControlID inID, const ABool inSupportInputMethod )//#688
{
	if( true )
	{
		//IBで設定されているviewを取得
		NSView*	oldView = CocoaObjects->FindViewByTag(inID);
		//親viewを取得
		NSView*	superView = [oldView superview];
		//frameを取得
		NSRect	frame = [oldView frame];
		//autoresizeフラグを取得
		NSInteger	autoresize = [oldView autoresizingMask];
		//旧viewを削除
		[oldView removeFromSuperview];//removeFromSuperviewはreleaseを実行する。アプリ側ではretainしていないので、ここで解放される。
		
		//user paneを生成
		CocoaUserPaneView*	view = nil;
		if( inSupportInputMethod == false )
		{
			//TextInputClientなしのviewを生成
			view = [[CocoaUserPaneView alloc] initWithFrame:frame windowImp:this controlID:inID];
		}
		else
		{
			//TextInputClient: Mac OS X 10.5以降
			//#0 Mac OS X 10.5未満は未対応とする if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
			{
				//TextInputClientありのviewを生成
				view = [[CocoaUserPaneTextInputClientView alloc] initWithFrame:frame windowImp:this controlID:inID];
			}
			/*
			else
			{
				//TextInputありのviewを生成
				view = [[CocoaUserPaneTextInputView alloc] initWithFrame:frame windowImp:this controlID:inID];
			}
			*/
			//
			[view setNeedsPanelToBecomeKey:true];
		}
		//親viewへ登録
		[superView addSubview:view positioned:NSWindowAbove relativeTo:nil];
		//autoresizeフラグを設定
		[view setAutoresizingMask:autoresize];
		
		//viewのポインタを保持するので、アプリ側で1つretainした状態(allocによるretain)のままにする。
		//mCocoaUserPaneViewArray_UserPaneViewからDeleteするときに、viewをreleaseする。
		CocoaObjects->RegisterUserPaneView(inID,view);
		//CocoaObjects側でretainするので、こちらはreleaseする。
		[view release];
		
		//UserPaneArray生成、登録
		CPaneForAViewFactory	factory(this,*this,inID);
		mUserPaneArray.AddNewObject(factory);
		mUserPaneArray_ControlID.Add(inID);
		mUserPaneArray_Type.Add(kUserPaneType_PaneForAView);
		//#688 mUserPaneArray_ControlRef.Add(NULL);
		
		//Overlay/Floatingウインドウ上に作成する場合は、最初のマウスクリックを処理するよう設定する
		if( mWindowClass == kWindowClass_Overlay || (mWindowClass == kWindowClass_Floating && mNonFloatingFloating == false) )//#1133
		{
			[view setAcceptsFirstMouse:YES];
		}
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		//#205 すでにUserPane以外のコントロールが存在している場合は、それを削除して同じ位置にUserPaneControlを作成する
		if( GetControlRef(inID) != NULL )
		{
			//コントロール種別取得
			ControlKind	kind;
			err = ::GetControlKind(GetControlRef(inID),&kind);
			if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
			if( kind.kind != kControlKindUserPane )
			{
				//旧コントロールの情報を取得
				AControlRef	oldControlRef = GetControlRef(inID);
				AControlRef	parent;
				err = ::GetSuperControl(oldControlRef,&parent);
				if( err != noErr )   _ACErrorNum("GetSuperControl() returned error: ",err,this);
				Rect	rect;
				::GetControlBounds(oldControlRef,&rect);
				//旧コントロール削除
				::DisposeControl(oldControlRef);
				
				//UserPaneControl生成
				AControlRef	controlRef;
				err = ::CreateUserPaneControl(mWindowRef,&rect,/*#182 0*/kControlSupportsEmbedding,&controlRef);
				if( err != noErr )   _ACErrorNum("CreateUserPaneControl() returned error: ",err,this);
				//位置等設定
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
AView用UserPaneを生成
*/
void	CWindowImp::CreatePaneForAView( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, 
		const AControlID inParentControlID, const AControlID inSiblingControlID, //#688
		const AIndex inTabIndex, const ABool inFront, const ABool inSupportInputMethod )//#688 win 080618
{
	//#688
	if( true )
	{
		//ウインドウ内の生成位置frameを計算
		AWindowRect	wrect = {0};
		wrect.left	= inPoint.x;
		wrect.top	= inPoint.y;
		wrect.right	= inPoint.x + inWidth;
		wrect.bottom= inPoint.y + inHeight;
		NSRect	rect = ConvertFromWindowRectToNSRect(CocoaObjects->GetContentView(),wrect);
		//user pane生成
		CocoaUserPaneView*	view = nil;
		if( inSupportInputMethod == false )
		{
			//TextInputClientなしのviewを生成
			view = [[CocoaUserPaneView alloc] initWithFrame:rect windowImp:this controlID:inID];
		}
		else
		{
			//TextInputClient: Mac OS X 10.5以降
			//#0 Mac OS X 10.5未満は未対応とする if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
			{
				//TextInputClientありのviewを生成
				view = [[CocoaUserPaneTextInputClientView alloc] initWithFrame:rect windowImp:this controlID:inID];
			}
			/*
			else
			{
				//TextInputありのviewを生成
				view = [[CocoaUserPaneTextInputView alloc] initWithFrame:rect windowImp:this controlID:inID];
			}
			*/
			//
			[view setNeedsPanelToBecomeKey:true];
		}
		//autoresizeフラグを設定
		//★resize mask設定すると、ツールバーのボタン等がおおきくなってしまう[view setAutoresizingMask:(NSViewWidthSizable|NSViewHeightSizable)];
		
		//親viewへ登録
		if( inParentControlID != kControlID_Invalid )
		{
			//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
			NSView*	parentView = CocoaObjects->FindViewByTag(inParentControlID);
			
			//引数に指定された親ビューへ登録
			[parentView addSubview:view positioned:NSWindowAbove relativeTo:nil];
		}
		else if( inSiblingControlID != kControlID_Invalid )
		{
			//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
			NSView*	siblingView = CocoaObjects->FindViewByTag(inSiblingControlID);
			
			//引数に指定された親ビューへ登録
			[[siblingView superview] addSubview:view positioned:NSWindowAbove relativeTo:nil];
		}
		else
		{
			//contentViewを親ビューとして登録
			[CocoaObjects->GetContentView() addSubview:view positioned:NSWindowAbove relativeTo:nil];
		}
		//コントロールIDキャッシュに記憶（control id, viewがわかっているので、ここで登録した方が高速）
		CocoaObjects->AddToControlIDCache(inID,view);
		//
		SetControlPosition(inID,inPoint);
		
		//viewのポインタを保持するので、アプリ側で1つretainした状態(allocによるretain)のままにする。
		//mCocoaUserPaneViewArray_UserPaneViewからDeleteするときに、viewをreleaseする。
		CocoaObjects->RegisterUserPaneView(inID,view);
		//CocoaObjects側でretainするので、こちらはreleaseする。
		[view release];
		
		//UserPaneArray生成、登録
		CPaneForAViewFactory	factory(this,*this,inID);
		mUserPaneArray.AddNewObject(factory);
		mUserPaneArray_ControlID.Add(inID);
		mUserPaneArray_Type.Add(kUserPaneType_PaneForAView);
		//#688 mUserPaneArray_ControlRef.Add(NULL);
		
		//Overlay/Floatingウインドウ上に作成する場合は、最初のマウスクリックを処理するよう設定する
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
		
		//#291 UserPaneにSupportFocusフラグを設定
		//#360 GetUserPane(inID).SetSupportFocus(inSupportFocus);
	}
#endif
}

#if SUPPORT_CARBON
/**
UserPaneの値が変更されたときのCUserPaneからのコールバック
*/
void	CWindowImp::CB_UserPaneValueChanged( const AControlID inID )
{
	GetAWin().EVT_ValueChanged(inID);
}

//B0322
/**
UserPaneでText入力されたときのCUserPaneからのコールバック
*/
void	CWindowImp::CB_UserPaneTextInputted( const AControlID inID )
{
	GetAWin().EVT_TextInputted(inID);
}
#endif

//#205 #if USE_IMP_TABLEVIEW
#pragma mark ===========================

#pragma mark ---テーブルView管理

#if SUPPORT_CARBON
/**
TableView取得
*/
CTableView&	CWindowImp::GetTableViewByID( const AControlID inControlID )
{
	if( GetUserPaneType(inControlID) != kUserPaneType_TableView )   _ACThrow("not table view",this);
	return dynamic_cast<CTableView&>(GetUserPane(inControlID));
}

/**
TableView取得
*/
const CTableView&	CWindowImp::GetTableViewConstByID( const AControlID inControlID ) const
{
	if( GetUserPaneType(inControlID) != kUserPaneType_TableView )   _ACThrow("not table view",this);
	return dynamic_cast<const CTableView&>(GetUserPaneConst(inControlID));
}
#endif

/**
TableViewかどうか
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
TableView登録
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
列を登録
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

//テーブルデータ全設定

/**
設定開始時に呼ぶ
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
列データを設定(Text)
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
列データを設定(Bool)
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
列データを設定(Number)
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
列データを設定(Color)
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

//列データを設定(IsContainer)
/*階層モード実装途中void	CWindowImp::SetTable_IsContainer( const AControlID inControlID, const ABoolArray& inIsContaner )
{
	GetTableViewByID(inControlID).SetTable_IsContainer(inIsContaner);
}*/

/**
全列設定完了時に呼ぶ
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

//行追加

/**
行追加開始時に呼ぶ
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
行（1セル）追加(Text)
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
行（1セル）追加(Number)
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

//行（1セル）追加(IsContainer)
/*階層モード実装途中void	CWindowImp::InsertRow_IsContainer( const AControlID inControlID, const AIndex inIndex, const ABool inIsContainer )
{
	GetTableViewByID(inControlID).InsertRow_IsContainer(inIndex,inIsContainer);
}*/

/**
行追加完了時に呼ぶ
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
TableView更新
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
TableViewセル編集モード
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
アイコン設定（列単位）
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
テキスト設定（列単位）
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
Bool設定（列単位）
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
Number設定（列単位）
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
色設定（列単位）
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
列の幅設定
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
列の幅取得
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
TableViewフォント設定
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
TableViewに0文字入力を禁止
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
TableViewのスクロール位置設定
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
TableViewのスクロール位置取得
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
TableViewを編集可能かどうか設定
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
TableViewの行の値変更時のコールバック
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
TableViewの行ダブルクリック時のコールバック
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
TableViewのコラム幅の変更やSort順の変更などの状態変更時のコールバック
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
TableViewの選択項目変更時のコールバック
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

#pragma mark ---ポップアップメニュー管理

/**
ポップアップメニューにATextArrayの内容を設定
*/
void	CWindowImp::SetMenuItemsFromTextArray( const AControlID inID, const ATextArray& inTextArray )
{
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSPopUpButtonの場合、TextArrayの内容をメニュー項目として追加
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			
			//項目挿入位置を初期化
			AIndex	insertIndex = 0;
			//
			if( [popup pullsDown] == YES )
			{
				//==================pull downメニューの場合==================
				if( AMenuWrapper::GetMenuItemCount([popup menu]) == 0 )
				{
					//最初の項目（表示タイトル）が存在しない場合は、最初の項目を追加
					AText	text;
					AMenuWrapper::AddMenuItem([popup menu],text,kMenuItemID_Invalid,0,0);
				}
				else
				{
					//最初の項目以外を削除（最初のメニュー項目には、ツールバーの場合、プルダウンメニューのタイトルアイコンが設定されている場合がある）
					while( AMenuWrapper::GetMenuItemCount([popup menu]) >= 2 )
					{
						AMenuWrapper::DeleteMenuItem([popup menu],AMenuWrapper::GetMenuItemCount([popup menu])-1);
					}
				}
				//項目挿入位置は1から
				insertIndex = 1;
			}
			else
			{
				//==================pull downではないメニューの場合==================
				//メニュー項目全削除
				AMenuWrapper::DeleteAllMenuItems([popup menu]);
			}
			
			for( AIndex i = 0; i < inTextArray.GetItemCount(); i++ )
			{
				AText	text;
				inTextArray.Get(i,text);
				//メニュー項目追加
				//（NSPopUpButtonのaddItemWithTitleは、同じテキストがあるとそれを削除して追加するので、NSMenuのメソッドを使う）
				/*
				AStCreateNSStringFromAText	str(text);
				[[popup menu] addItemWithTitle:str.GetNSString() action:NULL keyEquivalent:@""];
				*/
				
				//0x00で始まるテキストの場合は、disable項目にする
				ABool	enable = true;
				if( text.GetItemCount() > 0 )
				{
					if( text.Get(0) == 0x00 )
					{
						enable = false;
						text.Delete1(0);
					}
				}
				//項目挿入
				AMenuWrapper::InsertMenuItem([popup menu],insertIndex,text,kMenuItemID_Invalid,0,0);
				//enable/disable設定
				AMenuWrapper::SetEnableMenuItem([popup menu],insertIndex,enable);
				//項目挿入位置を次へ
				insertIndex++;
			}
		}
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		ACarbonMenuWrapper::SetMenuItemsFromTextArray(GetMenuRef(inID),inTextArray);
		::SetControl32BitMaximum(GetControlRef(inID),ACarbonMenuWrapper::GetMenuItemCount(GetMenuRef(inID)));
	}
#endif
}

//win
/**
ポップアップメニュー項目追加
*/
void	CWindowImp::InsertMenuItem( const AControlID inID, const AIndex inMenuItemIndex, const AText& inText )
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSPopUpButtonの場合、メニュー項目追加
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			
			//メニュー項目追加
			//（NSPopUpButtonのaddItemWithTitleは、同じテキストがあるとそれを削除して追加するので、NSMenuのメソッドを使う）
			/*
			AStCreateNSStringFromAText	str(inText);
			[[popup menu] insertItemWithTitle:str.GetNSString() action:NULL keyEquivalent:@"" atIndex:inMenuItemIndex];
			*/
			
			//メニュー項目追加
			AIndex	itemIndex = inMenuItemIndex;
			if( [popup pullsDown] == YES )
			{
				itemIndex++;
			}
			//テキスト取得
			AText	text;
			text.SetText(inText);
			//0x00で始まるテキストの場合は、disable項目にする
			ABool	enable = true;
			if( text.GetItemCount() > 0 )
			{
				if( text.Get(0) == 0x00 )
				{
					enable = false;
					text.Delete1(0);
				}
			}
			//項目挿入
			AMenuWrapper::InsertMenuItem([popup menu],itemIndex,text,kMenuItemID_Invalid,0,0);
			//enable/disable設定
			AMenuWrapper::SetEnableMenuItem([popup menu],itemIndex,enable);
		}
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		ACarbonMenuWrapper::InsertMenuItem(GetMenuRef(inID),inMenuItemIndex,inText,0,0,0);
		::SetControl32BitMaximum(GetControlRef(inID),ACarbonMenuWrapper::GetMenuItemCount(GetMenuRef(inID)));
	}
#endif
}

//win
/**
ポップアップメニュー項目削除
*/
void	CWindowImp::DeleteMenuItem( const AControlID inID, const AIndex inMenuItemIndex )
{
	//#688 Cocoa対応
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//NSPopUpButtonの場合、メニュー項目削除
		if( [view isKindOfClass:[NSPopUpButton class]] == YES )
		{
			NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
			//メニュー項目削除
			AIndex	itemIndex = inMenuItemIndex;
			if( [popup pullsDown] == YES )
			{
				itemIndex++;
			}
			[[popup menu] removeItemAtIndex:itemIndex];
		}
		//上記以外はエラー
		else
		{
			_ACError("",NULL);
		}
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		ACarbonMenuWrapper::DeleteMenuItem(GetMenuRef(inID),inMenuItemIndex);
		::SetControl32BitMaximum(GetControlRef(inID),ACarbonMenuWrapper::GetMenuItemCount(GetMenuRef(inID)));
	}
#endif
}

/**
ポップアップメニューにフォント一覧を設定
*/
void	CWindowImp::RegisterFontMenu( const AControlID inID, const ABool inEnableDefaultFontItem )//#375
{
	if( true )
	{
		//Cocoaウインドウの場合
		
		//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
		NSView*	view = CocoaObjects->FindViewByTag(inID);
		
		//
		//★未実装
	}
#if SUPPORT_CARBON
	else
	{
		AApplication::GetApplication().NVI_GetMenuManager().RegisterFontMenu(GetMenuRef(inID));//win ,GetControlRef(inID));
		//#375
		if( inEnableDefaultFontItem == true )
		{
			//mValueIndependentMenuArray_ControlIDに登録（メニュー表示内容と、GetText()/SetText()での値が異なるため）
			AIndex	valueIndependentMenuArrayIndex = mValueIndependentMenuArray_ControlID.Add(inID);
			mValueIndependentMenuArray_ValueArray.AddNewObject();
			//「（デフォルトのフォント）」のメニュー項目を追加
			AText	defaultFontTitle;
			defaultFontTitle.SetLocalizedText("DefaultFontTitle");
			ACarbonMenuWrapper::InsertMenuItem(GetMenuRef(inID),0,defaultFontTitle,0,0,0);
			AText	defaultFontValue("default");
			mValueIndependentMenuArray_ValueArray.GetObject(valueIndependentMenuArrayIndex).Add(defaultFontValue);
			//各フォントのメニュー項目をmValueIndependentMenuArray_ValueArrayに追加
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
ポップアップメニューに指定IDのTextArrayMenuIDを設定

@param inControlID ポップアップメニューボタンControl
@param inTextArrayMenuID ATextArrayMenuManagerへ登録したID
*/
void	CWindowImp::RegisterTextArrayMenu( const AControlID inControlID, const ATextArrayMenuID inTextArrayMenuID )
{
	//インスタンス内配列へ登録
	mTextArrayMenuArray_ControlID.Add(inControlID);
	/*#232
	mTextArrayMenuArray_TextArrayMenuID.Add(inTextArrayMenuID);
	//static配列へ登録
	sTextArrayMenu_MenuRef.Add(GetMenuRef(inControlID));
	sTextArrayMenu_ControlRef.Add(GetControlRef(inControlID));
	sTextArrayMenu_TextArrayMenuID.Add(inTextArrayMenuID);
	//メニューへ現在のDBのTextArrayを反映
	ACarbonMenuWrapper::SetMenuItemsFromTextArray(GetMenuRef(inControlID),sTextArrayMenuDB.GetData_ConstTextArrayRef(inTextArrayMenuID));
	::SetControl32BitMaximum(GetControlRef(inControlID),sTextArrayMenuDB.GetItemCount_Array(inTextArrayMenuID));
	*/
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().RegisterControl(GetAWin().GetObjectID(),inControlID,inTextArrayMenuID);
}

/**
ポップアップメニューのTextArrayMenuID設定を解除

@param inControlID ポップアップメニューControl
*/
void	CWindowImp::UnregisterTextArrayMenu( const AControlID inControlID )
{
	/*#232
	//win対応
	//AMenuMangerから移動
	AIndex	staticarrayindex = sTextArrayMenu_MenuRef.Find(GetMenuRef(inControlID));
	if( staticarrayindex == kIndex_Invalid )   {_ACError("",this);return;}
	//static配列から削除
	sTextArrayMenu_MenuRef.Delete1(staticarrayindex);
	sTextArrayMenu_ControlRef.Delete1(staticarrayindex);
	sTextArrayMenu_TextArrayMenuID.Delete1(staticarrayindex);
	*/
	//インスタンス配列から削除
	AIndex	index = mTextArrayMenuArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )   {_ACError("",this);return;}
	mTextArrayMenuArray_ControlID.Delete1(index);
	//#232 mTextArrayMenuArray_TextArrayMenuID.Delete1(index);
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().UnregisterControl(GetAWin().GetObjectID(),inControlID);
}

/*#232 
//TextArrayの更新(static)
void	CWindowImp::UpdateTextArrayMenu( const ATextArrayMenuID inTextArrayMenuID, const ATextArray& inTextArray )
{
	//DBに反映
	sTextArrayMenuDB.SetData_TextArray(inTextArrayMenuID,inTextArray);
	
	//登録中のメニュー全てに反映
	for( AIndex index = 0; index < sTextArrayMenu_MenuRef.GetItemCount(); index++ )
	{
		if( sTextArrayMenu_TextArrayMenuID.Get(index) == inTextArrayMenuID )
		{
			ACarbonMenuWrapper::SetMenuItemsFromTextArray(sTextArrayMenu_MenuRef.Get(index),inTextArray);
			::SetControl32BitMaximum(sTextArrayMenu_ControlRef.Get(index),sTextArrayMenuDB.GetItemCount_Array(inTextArrayMenuID));
		}
	}
}

//TextArrayメニュー登録(DB追加)(static)
void	CWindowImp::RegisterTextArrayMenuID( const ATextArrayMenuID inTextArrayMenuID )
{
	sTextArrayMenuDB.CreateData_TextArray(inTextArrayMenuID,"","");
}
*/

/*#349
#pragma mark ===========================

#pragma mark --- ラジオボタングループ

**
ラジオボタングループ生成
*
void	CWindowImp::RegisterRadioGroup()
{
	mRadioGroup.AddNewObject();
}

**
最後に生成したラジオボタングループにコントロールを追加
*
void	CWindowImp::AddToLastRegisteredRadioGroup( const AControlID inID )
{
	if( mRadioGroup.GetItemCount() == 0 )   return;
	mRadioGroup.GetObject(mRadioGroup.GetItemCount()-1).Add(inID);
}

**
ラジオボタングループのUpdate（指定したコントロール以外、falseに設定する）
*
void	CWindowImp::UpdateRadioGroup( const AControlID inControlID )
{
	//指定Controlが含まれるラジオボタングループを検索
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
	//ラジオブループが見つからなければリターン
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
ラジオグループに設定されたボタンの場合、Onでdisableにされたら、次のボタンをOnにする。
*
void	CWindowImp::UpdateRadioButtonDisabled( const AControlID inControlID )
{
	//指定Controlが含まれるラジオボタングループを検索
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
	//ラジオブループが見つからなければリターン
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

#pragma mark ---イベントハンドラ(Window)
//#688

/**
ウインドウframe変更時の処理実行
*/
void	CWindowImp::APICB_CocoaWindowBoundsChanged( const AGlobalRect& inNewFrame )
{
	GetAWin().EVT_WindowBoundsChanged(mCurrentWindowBounds,inNewFrame);
	mCurrentWindowBounds = inNewFrame;
}

/**
ウインドウresize完了時の処理実行
*/
void	CWindowImp::APICB_CocoaWindowResizeCompleted()
{
	//Live Resize中かどうかの判定をfalseにするためのフラグON
	mDoingResizeCompleted = true;
	//resize完了イベント
	GetAWin().EVT_WindowResizeCompleted();
	//Live Resize中かどうかの判定をfalseにするためのフラグOFF
	mDoingResizeCompleted = false;
}

/**
ボタンsentAction処理実行
*/
void	CWindowImp::APICB_CocoaDoButtonAction( const AControlID inControlID )
{
	//EVT_Clicked()実行
	if( GetAWin().EVT_Clicked(inControlID,0) == false )
	{
		//EVT_Clicked()で未処理ならEVT_ValueChanged()で処理
		GetAWin().EVT_ValueChanged(inControlID);
	}
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
ポップアップメニューsentAction処理実行
*/
void	CWindowImp::APICB_CocoaDoPopupMenuAction( const AControlID inControlID )
{
	//EVT_Clicked()実行
	if( GetAWin().EVT_Clicked(inControlID,0) == false )
	{
		//EVT_Clicked()で未処理ならEVT_ValueChanged()で処理
		GetAWin().EVT_ValueChanged(inControlID);
	}
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
TextField sentAction処理実行（TextFieldで文字入力完了（リターンキー等）時）
*/
void	CWindowImp::APICB_CocoaDoTextFieldAction( const AControlID inControlID )
{
	//EVT_ValueChanged()実行
	GetAWin().EVT_ValueChanged(inControlID);
	//EVT_TextInputted()実行
	GetAWin().EVT_TextInputted(inControlID);
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
TextField 文字入力時処理（一文字ごとにコールされる）
*/
void	CWindowImp::APICB_CocoaDoTextFieldTextChanged( const AControlID inControlID )
{
	//EVT_TextInputted()実行
	GetAWin().EVT_TextInputted(inControlID);
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

//#688
/**
FontField フォント変更(changeFont)時処理
*/
void	CWindowImp::APICB_CocoaDoFontFieldChanged( const AControlID inControlID )
{
	//EVT_ValueChanged()実行
	GetAWin().EVT_ValueChanged(inControlID);
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

//#688
/**
FontPanel フォント変更(changeFont)時処理
*/
void	CWindowImp::APICB_CocoaDoFontPanelFontChanged()
{
	//EVT_ValueChanged()実行
	GetAWin().EVT_ValueChanged(mFontPanelVirtualControlID);
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
ツールバー項目のsentAction処理
*/
void	CWindowImp::APICB_CocoaToolbarItemAction( const AControlID inControlID )
{
	//タブコントロールのタブ選択用ツールバー項目選択時は、タブ選択処理を行う
	if( inControlID >= kControlID_FirstTabSelectorToolbarItem && inControlID <= kControlID_MaxTabSelectorToolbarItem )
	{
		AIndex	tabIndex = inControlID-kControlID_FirstTabSelectorToolbarItem;
		SetTabControlValue(tabIndex);
	}
	
	//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
	NSView*	view = CocoaObjects->FindViewByTag(inControlID);
	
	//pull downメニューの場合、選択項目のテキストをメニュータイトル（0番目の項目）に設定
	if( [view isKindOfClass:[NSPopUpButton class]] == YES )
	{
		NSPopUpButton*	popup = ACocoa::CastNSPopUpButtonFromId(view);
		if( [popup pullsDown] == YES )
		{
			AText	text;
			ACocoa::SetTextFromNSString([popup titleOfSelectedItem],text);
			AMenuWrapper::SetMenuItemText([popup menu],0,text);
		}
		
		//pull downメニューの場合、選択された項目にチェックマークを付ける
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
	
	//EVT_Clicked()実行
	GetAWin().EVT_Clicked(inControlID,0);
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
ツールバー項目追加時処理実行
*/
void	CWindowImp::APICB_CocoaDoToolbarItemAddedAction( const AControlID inControlID )
{
	//CWindowImpCocoaObjects::CreateToolbar()実行中ではない場合のみ、
	//EVT_ToolbarItemAdded()を実行する。
	//（CWindowImpCocoaObjects::CreateToolbar()実行中だと、Mac OS X 10.4において、EVT_ToolbarItemAdded()から[toolbar items]をコールしたときに、再度ここがコールされて無限ループになる）
	if( CocoaObjects->IsCreatingToolbar() == false )
	{
		GetAWin().EVT_ToolbarItemAdded(inControlID);
	}
}

/**
ウインドウがMainWindowになったときの処理
*/
void	CWindowImp::APICB_CocoaBecomeMainWindow()
{
	//EVT_WindowActivated()実行
	GetAWin().EVT_WindowActivated();
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
ウインドウがMainWindowでなくなったときの処理
*/
void	CWindowImp::APICB_CocoaResignMainWindow()
{
	//EVT_WindowDeactivated()実行
	GetAWin().EVT_WindowDeactivated();
}

/**
ウインドウがMainWindowになったときの処理
*/
void	CWindowImp::APICB_CocoaBecomeKeyWindow()
{
	//EVT_WindowFocusActivated()実行
	GetAWin().EVT_WindowFocusActivated();
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
ウインドウがMainWindowでなくなったときの処理
*/
void	CWindowImp::APICB_CocoaResignKeyWindow()
{
	//EVT_WindowFocusDeactivated()実行
	GetAWin().EVT_WindowFocusDeactivated();
}

/**
ウインドウをDock化したときの処理
*/
void	CWindowImp::APICB_CocoaMiniaturize()
{
	//EVT_WindowCollapsed()実行
	GetAWin().EVT_WindowCollapsed();
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
キャンセル（ESCキー押下時等）処理
*/
void	CWindowImp::APICB_CocoaDoCancelOperation( const AModifierKeys inModifierKeys )
{
	if( HasCloseButton() == true && mCloseByEscapeKey == true )//#986
	{
		//クローズボタンがあれば、EVT_DoCloseButton()を実行
		GetAWin().EVT_DoCloseButton();
	}
	else
	{
		//デフォルトキャンセルボタン設定時は、デフォルトキャンセルボタンのEVT_Clicked()実行
		if( mDefaultCancelButton != kControlID_Invalid )
		{
			GetAWin().EVT_Clicked(mDefaultCancelButton,inModifierKeys);
		}
	}
}

/**
ウインドウを閉じようとするときの処理
*/
void	CWindowImp::APICB_CocoaDoWindowShouldClose()
{
	//AWindow::EVT_DoCloseButton()実行
	GetAWin().EVT_DoCloseButton();
}

#pragma mark ===========================

#pragma mark ---イベントハンドラ(View)

/*
CocoaUserPaneViewの各イベントコールバックから、こちらのメソッドがコールされる。
（CUserPane側にメソッドが存在するものについても、全て、一旦こちらのメソッドを介している。）
*/

/**
マウスダウン
*/
void	CWindowImp::APICB_CocoaMouseDown( const AControlID inControlID,
			const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//コントロールがVisibleでなければ何もしない
	if( IsControlVisible(inControlID) == false )   return;
	
	//#558
	//縦書きモードの場合、座標変換
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoMouseDown()を実行
	if( GetAWin().EVT_DoMouseDown(inControlID,localPoint,inModifierKeys,inClickCount) == false )//#232 返り値がfalseの場合だけEVT_Clicked()実行
	{
		//クリック回数によって、それぞれEVT_Clicked(), EVT_DoubleClicked()を実行
		if( inClickCount == 1 )
		{
			GetAWin().EVT_Clicked(inControlID,inModifierKeys);
		}
		else if( inClickCount == 2 )
		{
			GetAWin().EVT_Clicked(inControlID,inModifierKeys);//B0303 ウインドウアクティベートクリックが１回目になってしまい、EVT_Clicked()を呼べないことがあるので
			GetAWin().EVT_DoubleClicked(inControlID);
		}
	}
	//AWindow::EVT_GetCursorType()を実行
	ACursorWrapper::SetCursor(GetAWin().EVT_GetCursorType(inControlID,localPoint,inModifierKeys));
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
右マウスダウン
*/
void	CWindowImp::APICB_CocoaRightMouseDown( const AControlID inControlID,
			const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//コントロールがVisibleでなければ何もしない
	if( IsControlVisible(inControlID) == false )   return;
	
	//#558
	//縦書きモードの場合、座標変換
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoContextMenu()を実行
	GetAWin().EVT_DoContextMenu(inControlID,localPoint,inModifierKeys,inClickCount);
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
マウスdrag
*/
void	CWindowImp::APICB_CocoaMouseDragged( const AControlID inControlID,
			const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//コントロールがVisibleでなければ何もしない
	if( IsControlVisible(inControlID) == false )   return;
	
	//#558
	//縦書きモードの場合、座標変換
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoMouseTracking()を実行
	GetAWin().EVT_DoMouseTracking(inControlID,localPoint,inModifierKeys);
	
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
マウスup
*/
void	CWindowImp::APICB_CocoaMouseUp( const AControlID inControlID, const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//コントロールがVisibleでなければ何もしない
	if( IsControlVisible(inControlID) == false )   return;
	
	//#558
	//縦書きモードの場合、座標変換
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoMouseTrackEnd()を実行
	GetAWin().EVT_DoMouseTrackEnd(inControlID,localPoint,inModifierKeys);
	
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
マウスホイール
*/
void	CWindowImp::APICB_CocoaMouseWheel( const AControlID inControlID,
			const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
			const ALocalPoint inLocalPoint )
{
	//コントロールがVisibleでなければ何もしない
	if( IsControlVisible(inControlID) == false )   return;
	
	//#558
	//縦書きモードの場合、座標変換
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoMouseWheel()を実行
	GetAWin().EVT_DoMouseWheel(inControlID,inDeltaX,inDeltaY,inModifierKeys,localPoint);
}

/**
マウスmoved
*/
void	CWindowImp::APICB_CocoaMouseMoved( const AControlID inControlID,
			const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//コントロールがVisibleでなければ何もしない
	if( IsControlVisible(inControlID) == false )   return;
	/*
	//
	CWindowImp::ProcessMouseExitToAnotherControl(this,inControlID,inLocalPoint);
	
	//AWindow::EVT_DoMouseMoved()を実行
	GetAWin().EVT_DoMouseMoved(inControlID,inLocalPoint,inModifierKeys);
	
	//
	CWindowImp::SetMouseTracking(this,inControlID);
	*/
	
	//#558
	//縦書きモードの場合、座標変換
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
		//AWindow::EVT_DoMouseLeft()を実行
		GetAWin().EVT_DoMouseLeft(mMouseMovedTrackingControlID,localPoint);
	}
	if( inControlID != kControlID_Invalid )
	{
		//AWindow::EVT_DoMouseMoved()を実行
		GetAWin().EVT_DoMouseMoved(inControlID,localPoint,inModifierKeys);
	}
	//
	mMouseMovedTrackingControlID = inControlID;
	
	//ウインドウを対象としてmouse moved処理
	AWindowPoint	wpt = {0};
	GetWindowPointFromControlLocalPoint(inControlID,localPoint,wpt);
	GetAWin().EVT_DoMouseMoved(wpt,inModifierKeys);
	
	//カーソル設定
	ACursorWrapper::SetCursor(GetAWin().EVT_GetCursorType(inControlID,localPoint,inModifierKeys));
}

/**
マウスexited
*/
void	CWindowImp::APICB_CocoaMouseExited( const AControlID inControlID, const ALocalPoint inLocalPoint )
{
	//#558
	//縦書きモードの場合、座標変換
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	if( mMouseMovedTrackingControlID != kControlID_Invalid )
	{
		//AWindow::EVT_DoMouseLeft()を実行
		GetAWin().EVT_DoMouseLeft(mMouseMovedTrackingControlID,localPoint);
	}
	//
	mMouseMovedTrackingControlID = kControlID_Invalid;
}

/**
dragがviewに入った
*/
void	CWindowImp::APICB_CocoaDraggingEntered( const AControlID inControlID, const ADragRef inDragRef,
		const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//#558
	//縦書きモードの場合、座標変換
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//新たに入ったControlIDでAWindow::EVT_DoDragEnter()を実行
	GetAWin().EVT_DoDragEnter(inControlID,inDragRef,localPoint,inModifierKeys);
	//mDragTrackingControlを更新
	mDragTrackingControl = inControlID;
}

/**
drag tracking中
*/
void	CWindowImp::APICB_CocoaDraggingUpdated( const AControlID inControlID, const ADragRef inDragRef, 
		const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys, ABool& outIsCopyOperation )
{
	//#558
	//縦書きモードの場合、座標変換
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//新しいControlIDが、Tracking中のControlID内と違う場合は、現在Tracking中のControlへDoDragLeaveを送信 (Carbon時代の#236の対策を踏襲）
	if( mDragTrackingControl != kControlID_Invalid && mDragTrackingControl != inControlID )
	{
		//Tracking中のControlIDでAWindow::EVT_DoDragLeave()を実行
		GetAWin().EVT_DoDragLeave(mDragTrackingControl,inDragRef,localPoint,inModifierKeys);
		//mDragTrackingControlをクリア
		mDragTrackingControl = kControlID_Invalid;
	}
	//mDragTrackingControlが今回のControlIDと違う場合は、新たに入ったControlIDでAWindow::EVT_DoDragEnter()を実行
	if( mDragTrackingControl != inControlID )
	{
		GetAWin().EVT_DoDragEnter(inControlID,inDragRef,localPoint,inModifierKeys);
	}
	//AWindow::EVT_DoDragTracking()を実行
	GetAWin().EVT_DoDragTracking(inControlID,inDragRef,localPoint,inModifierKeys,outIsCopyOperation);
	//mDragTrackingControlを更新
	mDragTrackingControl = inControlID;
}

/**
dragがviewから出た
*/
void	CWindowImp::APICB_CocoaDraggingExited( const AControlID inControlID, const ADragRef inDragRef,
		const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//#558
	//縦書きモードの場合、座標変換
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//現在tracking中のcontrolについて、AWindow::EVT_DoDragLeave()を実行
	if( mDragTrackingControl != kControlID_Invalid )
	{
		GetAWin().EVT_DoDragLeave(mDragTrackingControl,inDragRef,localPoint,inModifierKeys);
	}
	//mDragTrackingControlをクリア
	mDragTrackingControl = kControlID_Invalid;
}

/**
drop時
*/
void	CWindowImp::APICB_CocoaPerformDragOperation( const AControlID inControlID, const ADragRef inDragRef, 
		const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	//#558
	//縦書きモードの場合、座標変換
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoDragReceive()実行
	GetAWin().EVT_DoDragReceive(inControlID,inDragRef,localPoint,inModifierKeys);
	//Drop済みフラグを設定（Drag元で使用する）
	CUserPane::SetDropped(true);
	
	//ファイルDrop
	AFileAcc	file;
	if( AScrapWrapper::GetFileDragData(inDragRef,file) == true )
	{
		GetAWin().EVT_DoFileDropped(inControlID,file);
	}
	
	//メニュー更新
	AApplication::GetApplication().NVI_UpdateMenu();
}

/**
スクロールバー操作時のCallback
*/
void	CWindowImp::APICB_DoScrollBarAction( const AControlID inID, const AScrollBarPart inPart )
{
	GetAWin().EVT_DoScrollBarAction(inID,inPart);
}

/**
描画
*/
void	CWindowImp::APICB_CocoaDrawRect( const AControlID inControlID, const ALocalRect& inDirtyRect )
{
	GetUserPane(inControlID).APICB_CocoaDrawRect(inDirtyRect);
}

/**
key down時処理（raw keyイベント）
*/
ABool	CWindowImp::APICB_CocoaDoKeyDown( const AControlID inControlID, const AText& inText, const AModifierKeys inModifierKeys )//#1080
{
	//AWindow::EVT_DoKeyDown()実行。実際に処理が行われたときはtrueが返る。
	//こちらはraw keyイベントなのでどうしても割り込む必要がある場合以外は、EVT_DoKeyDown()は処理を行わない。
	if( GetAWin().EVT_DoKeyDown(inControlID,inText,inModifierKeys) == true )
	{
		//メニュー更新
		AApplication::GetApplication().NVI_UpdateMenu();
		//処理済みなのでtrueを返す
		return true;
	}
	//未処理なのでfalseを返す
	return false;
}

/**
テキスト入力時処理
*/
ABool	CWindowImp::APICB_CocoaDoTextInput( const AControlID inControlID, const AText& inText,
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction )
{
	ABool	updateMenu = false;
	//AWindow::EVT_DoTextInput()を実行
	ABool	result = GetAWin().EVT_DoTextInput(inControlID,inText,inKeyBindKey,inModifierKeys,inKeyBindAction,updateMenu);
	if( updateMenu == true )
	{
		//メニュー更新
		AApplication::GetApplication().NVI_UpdateMenu();
	}
	return result;
}

/**
InlineInputテキスト更新
*/
void	CWindowImp::APICB_CocoaDoInlineInput( const AControlID inControlID, 
		const AText& inText, const AItemCount inFixLen,
		const AArray<AIndex>& inHiliteLineStyleIndex, 
		const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
		const AItemCount inReplaceLengthInUTF16 )
{
	ABool	updateMenu = false;
	//AWindow::EVT_DoInlineInput()を実行
	GetAWin().EVT_DoInlineInput(inControlID,inText,inFixLen,
				inHiliteLineStyleIndex,inHiliteStartPos,inHiliteEndPos,inReplaceLengthInUTF16,updateMenu);
	if( updateMenu == true )
	{
		//メニュー更新
		AApplication::GetApplication().NVI_UpdateMenu();
	}
}

/**
現在の選択テキストを取得(InlineInput用)
*/
void	CWindowImp::APICB_CocoaDoInlineInputGetSelectedText( const AControlID inControlID, AText& outText ) const
{
	outText.DeleteAll();
	GetAWinConst().EVT_DoInlineInputGetSelectedText(inControlID,outText);
}

/**
テキストのindexに対応する座標を取得(InlineInput用)
*/
void	CWindowImp::APICB_CocoaFirstRectForCharacterRange( const AControlID inControlID, 
		const AIndex inStart, const AIndex inEnd, AGlobalRect& outRect ) 
{
	outRect.left	= 0;
	outRect.top		= 0;
	outRect.right	= 0;
	outRect.bottom	= 0;
	//AWindow::EVT_DoInlineInputOffsetToPos()実行
	ALocalPoint	localStartPoint = {0}, localEndPoint = {0};
	if( GetAWinConst().EVT_DoInlineInputOffsetToPos(inControlID,inStart,localStartPoint) == true &&
				GetAWinConst().EVT_DoInlineInputOffsetToPos(inControlID,inEnd,localEndPoint) == true )
	{
		//行の高さを取得 #1124
		ANumber	lineHeight = 0, lineAscent = 0;
		GetUserPane(inControlID).GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
		//縦書きモード時は座標変換 #558
		if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
		{
			localStartPoint = ConvertVerticalCoordinateFromAppToImp(inControlID,localStartPoint);
			localEndPoint = ConvertVerticalCoordinateFromAppToImp(inControlID,localEndPoint);
		}
		//グローバル座標に変換
		AGlobalPoint	globalStartPoint = {0}, globalEndPoint = {0};
		GetGlobalPointFromControlLocalPoint(inControlID,localStartPoint,globalStartPoint);
		GetGlobalPointFromControlLocalPoint(inControlID,localEndPoint,globalEndPoint);
		//GlobalRect設定。top/botomは-2, +2を設定。
		outRect.left	= globalStartPoint.x;
		outRect.top		= globalStartPoint.y-lineHeight-2;//#1124
		outRect.right	= globalEndPoint.x;
		outRect.bottom	= globalEndPoint.y+lineAscent+2;//#1124
	}
}

/**
座標に対するテキストのindexを取得(InlineInput用)
*/
void	CWindowImp::APICB_CocoaCharacterIndexForPoint( const AControlID inControlID, 
		const AGlobalPoint& inGlobalPoint, AIndex& outOffsetUTF8 ) const
{
	outOffsetUTF8 = 0;
	//LocalPointへ変換
	AGlobalRect	windowbounds = {0};
	GetWindowBounds(windowbounds);
	AWindowPoint	wpt = {0};
	wpt.x = inGlobalPoint.x - windowbounds.left;
	wpt.y = inGlobalPoint.y - windowbounds.top;
	ALocalPoint	localPoint = {0};
	GetControlLocalPointFromWindowPoint(inControlID,wpt,localPoint);
	//AWindow::EVT_DoInlineInputPosToOffset()を実行
	GetAWinConst().EVT_DoInlineInputPosToOffset(inControlID,localPoint,outOffsetUTF8);
}

/**
Viewのframe変更時処理
*/
void	CWindowImp::APICB_CocoaViewFrameChanged( const AControlID inControlID )
{
	//AWindow::EVT_DoControlBoundsChanged()を実行
	GetAWin().EVT_DoControlBoundsChanged(inControlID);
}

//フォーカス移動実行中のフラグ
//selectNextKeyView, selectPreviousKeyView実行中にフラグONになる
ABool	gSwitchingFocusNow = false;

/**
フォーカス移動実行中のフラグを設定
*/
void	CWindowImp::SetSwitchingFocusNow( const ABool inSwitchingFocusNow )
{
	gSwitchingFocusNow = inSwitchingFocusNow;
}

//
//無限ループ防止のためのフラグ
//resignFirstResponder等は実際にfocusが外れる前にコールされるので、EVT_DoViewFocusDeactivated()等の中から
//SwitchFocusされると、無限ループ発生する。（例：AWindow::NVI_SetControlEnable()での同じ値設定チェックをはずした場合、
//AView_EditBoxからValue changedがコールされ、update control status経由で、switch focus nextされる。）
ABool	gDoingBecomeOrResignFirstResponder = false;

/**
ViewがFirstResponderになったときの処理
*/
void	CWindowImp::APICB_CocoaViewBecomeFirstResponder( const AControlID inControlID )
{
	//無限ループ防止
	if( gDoingBecomeOrResignFirstResponder == true )
	{
		_ACError("",NULL);
		return;
	}
	gDoingBecomeOrResignFirstResponder = true;
	
	//AWindow::EVT_DoViewFocusActivated()を実行
	GetAWin().EVT_DoViewFocusActivated(inControlID);
	
	//フォーカス移動実行中のフラグ設定中なら、ビューの内容を自動選択する。（タブキーによるフォーカス移動時にテキストを全選択する）
	if( gSwitchingFocusNow == true )
	{
		GetAWin().NVI_AutomaticSelectBySwitchFocus(inControlID);
	}
	
	//無限ループ防止
	gDoingBecomeOrResignFirstResponder = false;
}

/**
ViewがFirstResponderでなくなったときの処理
*/
void	CWindowImp::APICB_CocoaViewResignFirstResponder( const AControlID inControlID )
{
	//無限ループ防止
	if( gDoingBecomeOrResignFirstResponder == true )
	{
		_ACError("",NULL);
		return;
	}
	gDoingBecomeOrResignFirstResponder = true;
	
	//AWindow::EVT_DoViewFocusDeactivated()を実行
	GetAWin().EVT_DoViewFocusDeactivated(inControlID);
	
	//無限ループ防止
	gDoingBecomeOrResignFirstResponder = false;
}

/**
MouseTracking中状態を設定する

対象ウインドウからMouseが出たことを検知するため。
*/
void	CWindowImp::SetMouseTracking( CWindowImp* inWindowImp , const AControlID inControlID )
{
	sMouseTrackingMode = true;
	sMouseTrackingWindowImp = inWindowImp;
	sMouseTrackingControlID = inControlID;
}

//#310
/**
WindowでのMouseMovedイベント発生時にコールされる。他のコントロールでTracking中だったら、そのコントロールに対してDoMouseExited()をとばす。
CWindowImp::APICB_CocoaMouseMoved()から毎回コールされる。
こちらを使用するかどうか検討中。
*/
void	CWindowImp::ProcessMouseExitToAnotherControl( CWindowImp* inWindowImp, 
		const AControlID inControlID, const ALocalPoint inLocalPoint )
{
	//Tracking中の場合のみ処理
	if( sMouseTrackingMode == true )
	{
		if( inWindowImp != sMouseTrackingWindowImp || inControlID != sMouseTrackingControlID )
		{
			//現在のTracking中Controlと一致しなければ、そのControlに対してDoMouseExitedをコールする。
			if( CWindowImp::ExistWindowImp(sMouseTrackingWindowImp) == true )
			{
				//AWindow::EVT_DoMouseLeft()を実行
				sMouseTrackingWindowImp->GetAWin().EVT_DoMouseLeft(sMouseTrackingControlID,inLocalPoint);
			}
			//Tracking解除
			sMouseTrackingMode = false;
		}
	}
}

//MouseTrackingデータ
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
	//コントロールがVisibleでなければ何もしない
	if( IsControlVisible(inControlID) == false )   return;
	
	//#558
	//縦書きモードの場合、座標変換
	ALocalPoint	localPoint = inLocalPoint;
	if( GetUserPaneConst(inControlID).GetVerticalMode() == true )
	{
		localPoint = ConvertVerticalCoordinateFromImpToApp(inControlID,inLocalPoint);
	}
	
	//AWindow::EVT_DoMouseTrackEnd()を実行
	GetAWin().EVT_QuickLook(inControlID,localPoint,inModifierKeys);
}

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---イベントハンドラ(Carbon)

/**
コントロールマウス押下Tracking中のイベントハンドラ
*/
ABool	CWindowImp::APICB_DoControlTrack( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//OS10.3バグ対策
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
マウスクリックイベントハンドラ
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
		//コントロールがEnableなら処理を行う
		//（この判定を外す場合、コンテキストメニューの選択時処理が必ずフォーカスの当たっているviewに対して処理される
		//（AWindow::EVT_DoMenuItemSelected()を参照）問題を解決する必要有り #601調査時追加のコメント）
		if( IsControlEnabled(controlID) == true )
		{
			//#291 クリックしたコントロールがFocus受け付けない場合はフォーカス移動しないようにする
			/*#360
			ABool	supportFocus = true;
			if( IsUserPane(controlID) == true )
			{
				supportFocus = GetUserPane(controlID).GetSupportFocus();
			}
			*/
			//#391 現在のフォーカス位置と違うコントロールをクリックした場合は、フォーカスコントロールへValueChangedを通知
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
					/*#391 上の処理へ変更 #360の影響？でこの中を通らない場合が出てきたため
					//マウスクリックによりフォーカスが移動したときに旧フォーカスの値変更を通知する
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
			//UserPaneかどうかを判定
			if( IsUserPane(controlID) == true )
			{
				//B0353 他アプリクリック時にmiのドキュメントウインドウがアクティブのままとなる問題対策
				ProcessSerialNumber psn = {0, kCurrentProcess};
				err = ::SetFrontProcessWithOptions(&psn,kSetFrontProcessFrontWindowOnly);
				if( err != noErr )   _ACErrorNum("SetFrontProcessWithOptions() returned error: ",err,this);
				if( IsOverlayWindow() == false )//#291 この処理があるとOverlayウインドウのControl(SupportFocusでない)をクリックしたときにそのウインドウへフォーカス移動してしまう
				{
					err = ::SetUserFocusWindow(mWindowRef);//Drawerからフォーカスを移すため
					if( err != noErr )   _ACErrorNum("SetUserFocusWindow() returned error: ",err,this);
				}
				//UserPaneならClickメソッドをコール
				return GetUserPane(controlID).DoMouseDown(inEventRef);
			}
		}
	}
	/*B0353 他アプリクリック時にmiのドキュメントウインドウがアクティブのままとなる問題対策（miアプリdeactivate後にマウスダウンイベントがくるのかもしれない？）
	else
	{
		Select();
		ProcessSerialNumber psn = {0, kCurrentProcess};
		//B0346 ::SetFrontProcess(&psn);
		::SetFrontProcessWithOptions(&psn,kSetFrontProcessFrontWindowOnly);
		return true;
	}*/
	//B0000 モード設定ウインドウを後ろにまわして「キーバインド」のDataBrowserの項目をクリックすると、モード設定ウインドウが前面に来ない
	//これを追加すると解決するが、B0353問題が復活するかどうかが不明
	else
	{
		if( mIsOverlayWindow == true && mOverlayParentWindowRef != NULL )//#669
		{
			//Overlayの子ウインドウの場合は、親ウインドウを選択する
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
マウスWheelイベントハンドラ
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
		//#601 コントロールdisableでもホイール可能にする if( IsControlEnabled(controlID) == true )
		{
			//UserPaneかどうかを判定
			if( IsUserPane(controlID) == true )
			{
				//UserPaneならDoMouseWheelメソッドをコール
				return GetUserPane(controlID).DoMouseWheel(inEventRef);
			}
		}
	}
	return false;
}

/**
マウス移動イベントハンドラ
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
		//B0000 080810 ウインドウ内で他のコントロール上にマウスが移動したときはDoMouseExit()を実行して、CAppImpでのtrackingを解除
		/*#310
		AControlID	trackingControlID =  CAppImp::GetMouseTrackingControlID(mWindowRef);
		if( trackingControlID != kControlID_Invalid && trackingControlID != controlID )
		{
			//UserPaneかどうかを判定
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
			//UserPaneかどうかを判定
			if( IsUserPane(controlID) == true )
			{
				//UserPaneならDoMouseWheelメソッドをコール
				return GetUserPane(controlID).DoMouseMoved(inEventRef);
			}
		}
	}
	return false;
}

/**
コマンド実行イベントハンドラ
*/
ABool	CWindowImp::APICB_DoCommand( const EventHandlerCallRef inCallRef, const EventRef inEventRef, const HICommand inCommand )
{
	OSStatus	err = noErr;
	
	if( inCommand.commandID == 'swin' )   return false;
	//タブView更新
	if( inCommand.commandID == 'ttab' )
	{
		UpdateTab();
		return true;
	}
	//メニュー更新
	if( inCommand.commandID == kCommandID_UpdateCommandStatus )
	{
		if( IsActive() == false )   return false;//B0411
		if( /*#1034 mNavSheet != NULL ||*/ mHasSheet == true )   return false;
		//B0418 TableViewがText編集モードのときは、そちら優先
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
		//UserPaneにFocusがあるならまずUserPaneでメニュー更新（例えば、MLTEPaneの「コピー」等）
		if( mCurrentFocus != kControlID_Invalid )
		{
			if( IsUserPane(mCurrentFocus) == true && tableViewFocus == false/*B0418*/ )
			{
				GetUserPane(mCurrentFocus).UpdateMenu();
			}
		}
		
		//Absでメニュー更新
		GetAWin().EVT_UpdateMenu();
		
		//コマンドチェーン上のオブジェクト全てに処理させたいのでfalseで返す
		return false;
	}
	/*#539
	//ヘルプ
	AIndex	helpindex = mHelpAnchor_Number.Find(inCommand.commandID);
	if( helpindex != kIndex_Invalid )
	{
		DisplayHelpPage(inCommand.commandID);
		return true;
	}
	*/
//#205 #if USE_IMP_TABLEVIEW
	//テーブルRow Up, Down, Top, Bottomボタン
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
	
	//NavSheet表示中の閉じるは無視する
	//#1034 if( mNavSheet != NULL )
	if( mHasSheet == true )//#1034
	{
		if( inCommand.commandID == 'clos' )   return true;
	}
	
	//キー取得
	UInt32	modifiers = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	
	//
	AText	actiontext;
	AApplication::GetApplication().NVI_GetMenuManager().GetMenuActionText(inCommand.commandID,inCommand.menu.menuRef,inCommand.menu.menuItemIndex-1,actiontext);
	
//#205 #if USE_IMP_TABLEVIEW
	//B0418 TableViewがText編集モードのときは、そちら優先
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
	//UserPaneにFocusがあるならUserPaneに処理させてみる
	if( mCurrentFocus != kControlID_Invalid )
	{
		if( IsUserPane(mCurrentFocus) == true )
		{
			//コマンド処理
			if( GetUserPane(mCurrentFocus).DoMenuItemSelected(inCommand.commandID,actiontext,modifiers) == true )
			{
				return true;
			}
		}
	}
	
	/*#349
	//検討
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
	
	//inCommandからコントロールの種類を得ることが出来ないのでとにかく２つとも呼ぶ
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
キー押下イベントハンドラ
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
文字入力イベントハンドラ
*/
ABool	CWindowImp::APICB_DoTextInput( const EventHandlerCallRef inCallRef, const EventRef inEventRef, ABool& outUpdateMenu )
{
	OSStatus	err = noErr;
	
	Boolean	done = false;
	outUpdateMenu = true;
	
	//現在のフォーカスのControlIDを取得
	AControlID	controlID = mCurrentFocus;
	
	//入力文字取得
	UniChar	unicodeStr[4096];
	UInt32	len = 0;
	err = ::GetEventParameter(inEventRef,kEventParamTextInputSendText,typeUnicodeText,NULL,4096,&len,&(unicodeStr[0]));
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	//キー取得
	//B0429 UInt32	modifiers;
	//B0429 ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	UInt32	modifiers = AKeyWrapper::GetEventKeyModifiers(inEventRef);//B0429 ここはkEventTextInputUnicodeForKeyEventのイベントハンドラなので、kEventParamKeyModifiersは取得できない。rawKeyEventに変換してからでないと、modifiersは取得できない
	
	//テキスト入力処理（ウインドウ／ビューで全て処理する場合）
	if( controlID != kControlID_Invalid )
	{
		AText	text;
		text.InsertFromUTF16TextPtr(0,&(unicodeStr[0]),len);
		if( GetAWin().EVT_DoTextInput(controlID,text,inEventRef,outUpdateMenu) == true )   return true;
	}
	
	//ESCキー入力の場合、かつ、閉じるボタン有りの場合
	ABool	closeshortcurt = ( (len == 2 && unicodeStr[0] == 0x1B) || (len == 2 && unicodeStr[0] == '.' && AKeyWrapper::IsCommandKeyPressed(modifiers) == true ) );//B0430
	if( /*B0430 len == 2 && unicodeStr[0] == 0x1B*/closeshortcurt == true && HasCloseButton() )
	{
		GetAWin().EVT_DoCloseButton();
		done = true;
	}
	//ESCキー入力の場合//B0285
	else if( /*B0430 len == 2 && unicodeStr[0] == 0x1B*/closeshortcurt == true )
	{
		if( mDefaultCancelButton != kControlID_Invalid )
		{
			GetAWin().EVT_Clicked(mDefaultCancelButton,modifiers);
			done = true;
		}
	}
	
	//リターンキー、もしくは、エンターキー入力の場合
	else if( len == 2 && (unicodeStr[0] == '\r' || unicodeStr[0] == 3) )//B0285 3(enter)も追加
	{
		//対象コントロールがリターンキーを処理するかどうかを判定。
		ABool	controlCatchReturn = false;
		if( controlID != kControlID_Invalid )
		{
			controlCatchReturn = IsCatchReturn(controlID);
		}
		if( controlCatchReturn == false )
		{
			//対象コントロールがリターンキーを処理しない（ウインドウがリターンキーを処理する）場合
			
			if( controlID != kControlID_Invalid )
			{
				//コントロール値変更を通知
				GetAWin().EVT_ValueChanged(controlID);
				//検討
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
			
			//done=trueにしておかないと、::CallNextEventHandler()により、MLTEにリターンが渡されてしまう
			done = true;
		}
	}
	//タブキー入力の場合
	else if( len == 2 && unicodeStr[0] == '\t' )
	{
		//対象コントロールがリターンキーを処理するかどうかを判定。
		ABool	controlCatchTab = false;
		if( controlID != kControlID_Invalid )
		{
			controlCatchTab = IsCatchTab(controlID);
		}
		if( controlCatchTab == false )
		{
			//対象コントロールがリターンキーを処理しない（ウインドウがリターンキーを処理する）場合
			
			if( controlID != kControlID_Invalid )
			{
				//コントロール値変更を通知
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
			
			//検討
			done = true;
		}
	}
	//\キー入力、かつ、バックスラッシュ変換モードの場合
	else if( len == 2 && unicodeStr[0] == 0x00A5 && IsInputBackslashByYenKeyMode() == true )
	{
		AText	backslash;
		backslash.Add(0x5C);//backslash(UTF-8)
		InputText(controlID,backslash);
		done = true;
		//Text入力を通知
		GetAWin().EVT_TextInputted(controlID);
	}
	//カーソルキー #353
	else if(	(len == 2 && unicodeStr[0] == 0x001C) || (len == 2 && unicodeStr[0] == 0x001D) ||
				(len == 2 && unicodeStr[0] == 0x001E) || (len == 2 && unicodeStr[0] == 0x001F) )
	{
		if( GetAWin().EVT_ArrowKey(controlID,unicodeStr[0]) == true )
		{
			done = true;
		}
	}
	//スペースキー #353
	else if( len == 2 && unicodeStr[0] == 0x0020 )
	{
		AIndex	controlIndex = mUserPaneArray_ControlID.Find(controlID);
		if( controlIndex != kIndex_Invalid )
		{
			//ColorPickerPaneの場合、mousedownと同じ動作をさせる
			if( mUserPaneArray_Type.Get(controlIndex) == kUserPaneType_ColorPickerPane )
			{
				GetUserPane(controlID).DoMouseDown(inEventRef);
				done = true;
			}
		}
	}
	if( done == false )
	{
		//データタイプに従った、入力可能文字判定（キーフィルター）
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
		//if( err != noErr )   _ACErrorNum("CallNextEventHandler() returned error: ",err,this);CallNextEventHandler()は結局何もしなかった場合notHandledを返す
		done = true;
		if( IsWindowCreated() == true )//B0000 CallNextEventHandler()でウインドウが閉じられる可能性があるため
		{
			//Text入力を通知
			if( (	(len == 2 && unicodeStr[0] == 0x001C) || (len == 2 && unicodeStr[0] == 0x001D) ||
					(len == 2 && unicodeStr[0] == 0x001E) || (len == 2 && unicodeStr[0] == 0x001F) ) == false )//#7 カーソルキーだけで編集とみなされる問題対応
			GetAWin().EVT_TextInputted(controlID);
		}
	}
	
	return done;
}

/**
TSM入力イベントハンドラ
*/
ABool	CWindowImp::APICB_DoTextInputUpdateActiveInputArea( const EventHandlerCallRef inCallRef, const EventRef inEventRef, ABool& outUpdateMenu )
{
	OSStatus	err = noErr;
	
	Boolean	done = false;
	outUpdateMenu = true;
	
	//テキスト取得
	UInt32	actSize = 0;
	err = ::GetEventParameter(inEventRef,kEventParamTextInputSendText,typeUnicodeText,NULL,0,&actSize,NULL);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	AText	inputUTF16Text;
	if( actSize > 0 )//#182 データが存在する場合のみ取得
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
	
	//確定文字長(UTF-16文字の文字数）取得
	SInt32	fixlen = 0;
	err = ::GetEventParameter(inEventRef,kEventParamTextInputSendFixLen,typeLongInteger,NULL,sizeof(fixlen),NULL,&(fixlen));
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	if( fixlen >= inputUTF16Text.GetItemCount()/sizeof(UniChar) )   fixlen = inputUTF16Text.GetItemCount()/sizeof(UniChar);
	
	//InlineInputハイライト下線情報取得 
	actSize = 0;
	err = ::GetEventParameter(inEventRef,kEventParamTextInputSendHiliteRng,typeTextRangeArray,NULL,0,&actSize,NULL);
	AArray<AIndex>	hiliteLineStyleIndex;
	AArray<AIndex>	hiliteStartPos;
	AArray<AIndex>	hiliteEndPos;
	if( actSize > 0 )//#182 hiliteデータが存在する場合のみ取得するようにする
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
	
	//現在のフォーカスのControlIDを取得
	AControlID	controlID = mCurrentFocus;
	
	//テキスト入力処理（ウインドウ／ビューで全て処理する場合）
	if( controlID != kControlID_Invalid )
	{
		//
		if( GetAWin().EVT_DoInlineInput(controlID,inputUTF8Text,mCurrentInlineInputUTF8PosIndexArray.Get(fixlen),
				hiliteLineStyleIndex,hiliteStartPos,hiliteEndPos,outUpdateMenu) == true )   return true;
	}
	
	//\キー入力、かつ、バックスラッシュ変換モードの場合 B0332
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
	
	//Text入力を通知
	GetAWin().EVT_TextInputted(controlID);
	
	return done;
}

/**
TSMイベントハンドラ
*/
ABool	CWindowImp::APICB_DoTextInputOffsetToPos( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//現在のフォーカスのControlIDを取得
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
TSMイベントハンドラ
*/
ABool	CWindowImp::APICB_DoTextInputPosToOffset( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//現在のフォーカスのControlIDを取得
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
TSMイベントハンドラ
*/
ABool	CWindowImp::APICB_DoTextInputGetSelectedText( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//現在のフォーカスのControlIDを取得
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
WindowActivatedイベントハンドラ
*/
ABool	CWindowImp::APICB_DoWindowFocusAcquiredHandler( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//フローティングウインドウの場合はUserFocusをフローティングウインドウではない最前面のウインドウへ移動させる
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
WindowActivatedイベントハンドラ
*/
ABool	CWindowImp::APICB_DoWindowActivated( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//最初にCallNextEventHandler+HIViewRenderをしておかないと、OSX10.3で、タイトルバーの表示が乱れる（インアクティブになる）
	//EVT_WindowActivated()のなかでHIViewRenderを行うのが影響している？
	err = ::CallNextEventHandler(inCallRef,inEventRef);
	if( err != noErr )   _ACErrorNum("CallNextEventHandler() returned error: ",err,this);
	err = ::HIViewRender(::HIViewGetRoot(mWindowRef));
	if( err != noErr )   _ACErrorNum("HIViewRender() returned error: ",err,this);
	
	GetAWin().EVT_WindowActivated();
	
	//UserPaneへActivated通知
	for( AIndex i = 0; i < mUserPaneArray.GetItemCount(); i++ )
	{
		mUserPaneArray.GetObject(i).DoWindowActivated();
	}
	
	//フォーカスを復元
	if( mLatentFocus != kControlID_Invalid )
	{
		SetFocus(mLatentFocus);
		//#0 mLatentFocus = kControlID_Invalid;
	}
	/*#424 WindowClassがkWindowClass_Documentの場合のみTSM制御する（overlayウインドウで大量にactivateすると異常に時間がかかる）
	if( IsFloatingWindow() == true )//#138
	{
		//★フローティングウインドウの場合のTSM制御は未対応
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
WindowDeactivatedイベントハンドラ
*/
ABool	CWindowImp::APICB_DoWindowDeactivated( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	/*#424 WindowClassがkWindowClass_Documentの場合のみTSM制御する（overlayウインドウで大量にactivateすると異常に時間がかかる）
	if( IsFloatingWindow() == true )//#138
	{
		//★フローティングウインドウの場合のTSM制御は未対応
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
	
	//現在のフォーカスを保存して、フォーカスをクリア
	if( mCurrentFocus != kControlID_Invalid )//#42 既にFocusがクリアされているときはフォーカスクリア、Latent設定をしない
	{
		mLatentFocus = mCurrentFocus;
		ClearFocus(false);//#212
	}
	
	//UserPaneへDeactivated通知
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
WindowCollapsedイベントハンドラ
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
閉じるボタンクリック時イベントハンドラ
*/
ABool	CWindowImp::APICB_DoWindowCloseButton( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	GetAWin().EVT_DoCloseButton();
	return true;
}

/**
ウインドウ領域変更後イベントハンドラ
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
ウインドウリサイズ変更済みイベントハンドラ
*/
ABool	CWindowImp::APICB_DoWindowResizeCompleted( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	GetAWin().EVT_WindowResizeCompleted();
	return false;
}

/**
カーソル変更イベントハンドラ
*/
ABool	CWindowImp::APICB_DoWindowCursorChange( const EventHandlerCallRef inCallRef, const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//イベント対象のコントロールのControlIDを取得
	HIViewRef	view;
	err = ::HIViewGetViewForMouseEvent(::HIViewGetRoot(mWindowRef),inEventRef,&view);
	if( err != noErr )   _ACErrorNum("HIViewGetViewForMouseEvent() returned error: ",err,this);
	
	if( view == NULL )   return false;
	
	AControlID	controlID = GetControlID(view);
	
	//コントロール種別を取得
	ControlKind	kind;
	err = ::GetControlKind(view,&kind);
	if( err != noErr )   _ACErrorNum("GetControlKind() returned error: ",err,this);
	
	//NavSheet表示中は矢印カーソル
	//#1034 if( mNavSheet != NULL )
	if( mHasSheet == true )//#1034
	{
		err = ::SetThemeCursor(kThemeArrowCursor);
		if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,this);
		return true;
	}
	//UserPaneの場合はUserPaneのメソッドをコール
	if( IsUserPane(controlID) == true )
	{
		return GetUserPane(controlID).DoAdjustCursor(inEventRef);
	}
	//テキスト系のコントロールの場合はIビームカーソルに変更
	else if( IsActive() && 
			(kind.kind == kControlKindEditText || kind.kind == kControlKindHIComboBox ||
			kind.kind == kControlKindEditUnicodeText) )
	{
		err = ::SetThemeCursor(kThemeIBeamCursor);
		if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,this);
	}
	//それ以外は矢印カーソルに変更
	else
	{
		err = ::SetThemeCursor(kThemeArrowCursor);
		if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,this);
	}
	return true;
}

/**
Drag Tracking中イベントハンドラ
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
	//#236 Drop可能なControl内からDrop先Controlを探す
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
		kDragTrackingEnterWindow, kDragTrackingInWindow, kDragTrackingLeaveWindowはあくまで
		Windowに入った・Tracking・出たときに通知されるイベントなので、Control単位に変換する。
		*/
	  case kDragTrackingEnterWindow:
		{
			/*#236
			if( IsUserPane(controlID) == true )
			{
				GetUserPane(controlID).DoDragEnter(inDragRef,localPoint,modifiers);
			}
			*/
			//#236　新たに入ったControlへDoDragEnterを送信
			if( controlID != kControlID_Invalid )
			{
				ALocalPoint	localPoint;
				GetControlLocalPointFromWindowPoint(controlID,windowPoint,localPoint);
				GetUserPane(controlID).DoDragEnter(inDragRef,localPoint,modifiers);
			}
			//mDragTrackingControlを更新
			//入った場所に登録Controlが無い場合はmDragTrackingControlにInvalid設定される
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
			//#236 現在のマウス位置が、Tracking中のControlID内に無い場合は、現在Tracking中のControlへDoDragLeaveを送信
			if( mDragTrackingControl != kControlID_Invalid && mDragTrackingControl != controlID )
			{
				ALocalPoint	localPoint;
				GetControlLocalPointFromWindowPoint(mDragTrackingControl,windowPoint,localPoint);
				GetUserPane(mDragTrackingControl).DoDragLeave(inDragRef,localPoint,modifiers);
				mDragTrackingControl = kControlID_Invalid;
			}
			//#236　マウス位置のControlへDoDragEnter, DoDragTrackingを送信
			//DoDragEnterは新たにこのControlに入った場合のみ送信する。
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
			//mDragTrackingControlを更新
			//入った場所に登録Controlが無い場合はmDragTrackingControlにInvalid設定される
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
			//#236　現在Tracking中のControlへDoDragLeaveを送信
			if( mDragTrackingControl != kControlID_Invalid )
			{
				if( ExistControl(mDragTrackingControl) == true )//#0 Drag&Drop後にそのViewをdeleteした場合、その後leaveが来たときにはViewが削除されているためチェック必要
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
			//処理無し
			break;
		}
	}
}

/**
Drag Receiveイベントハンドラ
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
	//#236 Drop可能なControl内からDrop先Controlを探す
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
	//ファイルDrop
	AFileAcc	file;
	if( AScrapWrapper::GetFileDragData(inDragRef,file) == true )
	{
		GetAWin().EVT_DoFileDropped(controlID,file);
	}
}

//#236
/**
DropされたWindowPointからControlを検索する
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
GetTypesサービスイベントハンドラ
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
Copyサービスイベントハンドラ
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
Pasteサービスイベントハンドラ
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
CAppImpからコールされるMouseExitedイベントハンドラ

mouse movedイベントがWindowで処理されなかった場合に、sMouseTrackingWindowRefのウインドウに対してコールされる
*/
void	CWindowImp::DoMouseExited( const AControlID inID, const EventRef inEventRef )//R0228
{
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).DoMouseExited(inEventRef);//R0228
	}
}
#endif

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---コントロールID/Ref変換
/**
ControlRef(HIViewRef)の取得
*/
AControlRef	CWindowImp::GetControlRef( const AControlID inID ) const
{
	OSStatus	err = noErr;
	
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACThrow("Window not created",this);return NULL;}
	
	//#353 -2をtabcontrolに割り当てる。tabcontrolへのフォーカス設定のため。
	if( inID == kControlID_TabControl )
	{
		return GetControlRef('ttab',0);
	}
	
	//#379 HIViewFindByID()重いので、高速化（ユーザーペインならそのクラスから取得）
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
ControlRef(HIViewRef)の取得（基本的にsignatureは0固定だが、Tab等特殊なもののため）
*/
AControlRef	CWindowImp::GetControlRef( const OSType inSignature, const AControlID inID ) const
{
	OSStatus	err = noErr;
	
	//ウインドウ生成済みチェック
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
コントロールIDの取得
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
メニューRefの取得
*/
ACarbonMenuRef	CWindowImp::GetMenuRef( const AControlID inID ) const
{
	//Carbonウインドウの場合
	
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

#pragma mark ---ファイル／フォルダ選択

//フォルダ選択シート表示
//inVirtualControlID: フォルダ選択完了時にAWindowへコールバックされるときに通知される（仮想的な）ControlID
void	CWindowImp::ShowChooseFolderWindow( const AFileAcc& inDefaultFolder, const AText& inMessage,
		const AControlID inVirtualControlID, const ABool inSheet )//#551
{
	//#1034
	//virtual control ID設定
	mChooseFolderVirtualControlID = inVirtualControlID;
	//デフォルトフォルダパス設定
	AText	dirpathtext;
	inDefaultFolder.GetPath(dirpathtext);
	NSString*	dirpath = nil;
	if( dirpathtext.GetItemCount() > 0 )
	{
		AStCreateNSStringFromAText	dirpathstr(dirpathtext);
		dirpath = [[[NSString alloc] initWithString:dirpathstr.GetNSString()] autorelease];
	}
	//メッセージ設定
	NSString*	message = nil;
	if( inMessage.GetItemCount() > 0 )
	{
		AStCreateNSStringFromAText	messagestr(inMessage);
		message = [[[NSString alloc] initWithString:messagestr.GetNSString()] autorelease];
	}
	//シートモードならシートフラグ設定
	if( inSheet == true )
	{
		mHasSheet = true;
	}
	//フォルダ選択シート／ダイアログ表示
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
フォルダ選択シートコールバック
（キャンセル時は引数空文字でコールされる）
*/
void	CWindowImp::APICB_ShowChooseFolderWindowAction( const AText& inPath )
{
	//#1034
	//シートフラグ解除
	mHasSheet = false;
	//フォルダ選択時処理実行
	if( inPath.GetItemCount() > 0 )
	{
		AFileAcc	file;
		file.Specify(inPath);
		GetAWin().EVT_FolderChoosen(mChooseFolderVirtualControlID,file);
	}
	//virtual control ID初期化
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
		//デフォルトフォルダ #551
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
フォルダ選択シートコールバック（OSコールバック用static）
*/
/*#1034
pascal void	CWindowImp::STATIC_NavChooseFolderCB( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD )
{
	//イベントトランザクション前処理／後処理 #0 ここにこれがないと、Refresh等経由でOSからイベントCB発生したときに、そこでsEventDepth==1となってしまい、
	//Window等のオブジェクトに対してDoObjectArrayItemTrashDelete()が発生し、メンバー変数へのメモリアクセスが発生し、落ちてしまう。
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
ファイル選択シート表示
@param inVirtualControlID フォルダ選択完了時にAWindowへコールバックされるときに通知される（仮想的な）ControlID
*/
void	CWindowImp::ShowChooseFileWindow( const AFileAcc& inDefaultFolder, const AText& inMessage,
		const AControlID inVirtualControlID, const ABool inOnlyApp, const ABool inSheet )
{
	//#1034
	//virtual control ID設定
	mChooseFileVirtualControlID = inVirtualControlID;
	//デフォルトフォルダパス設定
	AText	dirpathtext;
	inDefaultFolder.GetPath(dirpathtext);
	NSString*	dirpath = nil;
	if( dirpathtext.GetItemCount() > 0 )
	{
		AStCreateNSStringFromAText	dirpathstr(dirpathtext);
		dirpath = [[[NSString alloc] initWithString:dirpathstr.GetNSString()] autorelease];
	}
	//メッセージ設定
	NSString*	message = nil;
	if( inMessage.GetItemCount() > 0 )
	{
		AStCreateNSStringFromAText	messagestr(inMessage);
		message = [[[NSString alloc] initWithString:messagestr.GetNSString()] autorelease];
	}
	//シートモードならシートフラグ設定
	if( inSheet == true )
	{
		mHasSheet = true;
	}
	//ファイル選択シート／ダイアログ表示
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
ファイル選択シート表示
@param inVirtualControlID フォルダ選択完了時にAWindowへコールバックされるときに通知される（仮想的な）ControlID
*/
void	CWindowImp::ShowChooseFileWindowWithFileFilter( const AFileAcc& inDefaultFile, const AControlID inVirtualControlID, const ABool inSheet,
		const AFileAttribute inFilterAttribute )
{
	//★未実装
	
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
ファイル選択シートコールバック
（キャンセル時は引数空文字でコールされる）
*/
void	CWindowImp::APICB_ShowChooseFileWindowAction( const AText& inPath )
{
	//#1034
	//シートフラグ解除
	mHasSheet = false;
	//フォルダ選択時処理実行
	if( inPath.GetItemCount() > 0 )
	{
		AFileAcc	file;
		file.Specify(inPath);
		GetAWin().EVT_FileChoosen(mChooseFileVirtualControlID,file);
	}
	//virtual control ID初期化
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
		//デフォルトファイル
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
ファイル選択シートコールバック（OSコールバック用static）
*/
/*#1034
pascal void	CWindowImp::STATIC_NavChooseFileCB( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD )
{
	//イベントトランザクション前処理／後処理 #0 ここにこれがないと、Refresh等経由でOSからイベントCB発生したときに、そこでsEventDepth==1となってしまい、
	//Window等のオブジェクトに対してDoObjectArrayItemTrashDelete()が発生し、メンバー変数へのメモリアクセスが発生し、落ちてしまう。
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
アプリケーションファイルのみを選択可能にするためのフィルタコールバック
*/
/*#1034
Boolean	CWindowImp::STATIC_NavFilterCB_ApplicationFile( AEDesc * inDesc, void * inInfo, void * inCallBackUD, NavFilterModes inFilterMode )
{
	//イベントトランザクション前処理／後処理 #0 ここにこれがないと、Refresh等経由でOSからイベントCB発生したときに、そこでsEventDepth==1となってしまい、
	//Window等のオブジェクトに対してDoObjectArrayItemTrashDelete()が発生し、メンバー変数へのメモリアクセスが発生し、落ちてしまう。
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
指定Creator/Typeファイルのみを選択可能にするためのフィルタコールバック
*/
/*#1034
Boolean	CWindowImp::STATIC_NavFilterCB_CreatorTypeFilter( AEDesc * inDesc, void * inInfo, void * inCallBackUD, NavFilterModes inFilterMode )
{
	//イベントトランザクション前処理／後処理 #0 ここにこれがないと、Refresh等経由でOSからイベントCB発生したときに、そこでsEventDepth==1となってしまい、
	//Window等のオブジェクトに対してDoObjectArrayItemTrashDelete()が発生し、メンバー変数へのメモリアクセスが発生し、落ちてしまう。
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
AskSaveChangesダイアログ表示
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
AskSaveChangesダイアログコールバック（OSコールバック用static）
*/
pascal void	CWindowImp::STATIC_NavAskSaveChangesCB( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD )
{
	//イベントトランザクション前処理／後処理 #0 ここにこれがないと、Refresh等経由でOSからイベントCB発生したときに、そこでsEventDepth==1となってしまい、
	//Window等のオブジェクトに対してDoObjectArrayItemTrashDelete()が発生し、メンバー変数へのメモリアクセスが発生し、落ちてしまう。
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
AskSaveChangesダイアログコールバック
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
				::NavDialogDispose(mNavSheet);//もう用済みなのでkNavCBTerminateを待たずに殺す
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

//AskSaveChanges強制クローズ B0000
/*OSX10.4でクラッシュするのでやめる。NavDialogDispose()がNG？
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
保存ダイアログ表示
*/
void	CWindowImp::ShowSaveWindow( const AText& inFileName, const ADocumentID inDocumentID, 
		const AFileAttribute inFileAttr, const AText& inFilter, const AText& inRefText, const AFileAcc& inDefaultFolder )
{
	//★inFilter未実装
	
	//#1034
	//対象ドキュメント設定
	mSaveWindowDocumentID = inDocumentID;
	//保存タイプ参照テキスト設定
	mSaveWindowRefText.SetText(inRefText);
	//保存ファイル属性設定
	mSaveWindowFileAttribute = inFileAttr;
	//デフォルトファイル名設定
	NSString*	filename = nil;
	if( inFileName.GetItemCount() > 0 )
	{
		AStCreateNSStringFromAText	filenamestr(inFileName);
		filename = [[[NSString alloc] initWithString:filenamestr.GetNSString()] autorelease];
	}
	//デフォルトフォルダパス設定
	AText	dirpathtext;
	inDefaultFolder.GetPath(dirpathtext);
	NSString*	dirpath = nil;
	if( dirpathtext.GetItemCount() > 0 )
	{
		AStCreateNSStringFromAText	dirpathstr(dirpathtext);
		dirpath = [[[NSString alloc] initWithString:dirpathstr.GetNSString()] autorelease];
	}
	//シートフラグ設定
	mHasSheet = true;
	//ファイル保存シート表示
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
保存ダイアログコールバック（OSコールバック用static）
（キャンセル時は引数空文字でコールされる）
*/
void	CWindowImp::APICB_ShowSaveWindowAction( const AText& inPath )
{
	//#1034
	//シートフラグ解除
	mHasSheet = false;
	//保存ボタンクリック処理
	if( inPath.GetItemCount() > 0 )
	{
		AFileAcc	file;
		file.Specify(inPath);
		if( file.Exist() == true )
		{
			//ファイル置き換えの場合は、旧ファイルを置き換えることができるか試す。
			file.DeleteFile();
			if( file.Exist() == true )
			{
				//不要かも？
				AText	mes1, mes2;
				mes1.SetLocalizedText("SaveFailed_ReplaceError1");
				mes2.SetLocalizedText("SaveFailed_ReplaceError2");
				AApplication::GetApplication().NVI_ShowModalAlertWindow(mes1,mes2);//#291
				return;
			}
		}
		//ファイル生成
		file.CreateFile();
		file.SetFileAttribute(mSaveWindowFileAttribute);
		//ファイル保存処理実行
		GetAWin().EVT_SaveWindowReply(mSaveWindowDocumentID,file,mSaveWindowRefText);
	}
}
/*#1034
pascal void	CWindowImp::STATIC_NavPutDialogCB( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD )
{
	//イベントトランザクション前処理／後処理 #0 ここにこれがないと、Refresh等経由でOSからイベントCB発生したときに、そこでsEventDepth==1となってしまい、
	//Window等のオブジェクトに対してDoObjectArrayItemTrashDelete()が発生し、メンバー変数へのメモリアクセスが発生し、落ちてしまう。
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
保存ダイアログコールバック
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
									//ファイル置き換えの場合は、旧ファイルを置き換えることができるか試す。
									file.DeleteFile();
									if( file.Exist() == true )
									{
										//不要かも？
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
					//closingSaveModeをfalseにする？
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

#pragma mark ---子ウインドウ管理

/*#291
//B0244
**
子Sheetウインドウ表示（OKボタンのみ）
*
void	CWindowImp::ShowChildWindow_OK( const AText& inMessage1, const AText& inMessage2 )
{
	//既に開いている子シートウインドウがあったらリターン
	if( mChildSheetWindow.GetItemCount() > 0 )
	{
		_AError("sheet window double open",this);
		return;
	}
	//子Sheetを生成、設定、表示
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
子Sheetウインドウ表示（OKボタン+Cancelボタン）
*
void	CWindowImp::ShowChildWindow_OKCancel( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
		const AControlID inOKButtonVirtualControlID )
{
	//既に開いている子シートウインドウがあったらリターン
	if( mChildSheetWindow.GetItemCount() > 0 )
	{
		_AError("sheet window double open",this);
		return;
	}
	//子Sheetを生成、設定、表示
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
子Sheetウインドウ表示（OKボタン+Cancelボタン+Discardボタン）
*
void	CWindowImp::ShowChildWindow_OKCancelDiscard( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
		const AControlID inOKButtonVirtualControlID, const AControlID inDiscardButtonVirtualControlID )
{
	//既に開いている子シートウインドウがあったらリターン
	if( mChildSheetWindow.GetItemCount() > 0 )
	{
		_AError("sheet window double open",this);
		return;
	}
	//子Sheetを生成、設定、表示
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

//B0244 B0285 CloseAlert->HideAlert（オブジェクトのdeleteはコール側で実施する。Callback実行途中のdeleteを防止するため）
**
子Sheetウインドウクローズ 
*
void	CWindowImp::CloseChildWindow()
{
	//子シートウインドウが無ければリターン
	if( mChildSheetWindow.GetItemCount() == 0 )
	{
		_AError("no sheet window",this);
		return;
	}
	//削除
	mChildSheetWindow.GetObject(0).Hide();
	mChildSheetWindow.GetObject(0).CloseWindow();
	mChildSheetWindow.DeleteAll();
}
*/

/**
子ウインドウ表示中かどうかを取得
*/
ABool	CWindowImp::ChildWindowVisible() const
{
	//#1034 if( mNavSheet != NULL /*#291|| mChildSheetWindow.GetItemCount() > 0*/ )   return true;
	if( mHasSheet == true )   return true;//#1034
	else return false;
}

#pragma mark ===========================

#pragma mark ---背景

//#688
AArray<NSImage*>	gBackgroundImageArray;
AObjectArray<AFileAcc>	gBackgroundImageArray_File;

/**
背景をOffscreenに保存
*/
AIndex	CWindowImp::RegisterBackground( const AFileAcc& inBackgroundImageFile )
{
	//背景画像ファイルが存在しないときはリターンする #1273
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
	
	//ファイルパス取得
	AText	filepath;
	inBackgroundImageFile.GetPath(filepath);
	AStCreateNSStringFromAText	filepathstr(filepath);
	//NSImage生成
	NSImage*	image = [[NSImage alloc] initByReferencingFile:filepathstr.GetNSString()];//ポインタを保持するのでretainCount=1のまま(initによる)にする
	//Arrayに格納
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
inDrawRectで指定する領域を背景画像で描画する
*/
void	CWindowImp::DrawBackgroundImage( const AControlID inControlID, const ALocalRect& inEraseRect, const AImageRect& inImageFrameRect, 
		const ANumber inLeftOffset, const AIndex inBackgroundImageIndex, const AFloatNumber inAlpha )
{
	//NSImage取得
	NSImage*	image = gBackgroundImageArray.Get(inBackgroundImageIndex);
	ANumber	imagewidth = [image size].width;
	ANumber	imageheight = [image size].height;
	//width or heightが0なら何もせず終了（0除算チェック）
	if( imagewidth == 0 || imageheight == 0 )   return;
	
	//inImageFrameRectを含む、直近の、imageの開始x,yを取得（タイルの区切り位置）
	ANumber	startx = inImageFrameRect.left/imagewidth;
	startx *= imagewidth;
	ANumber	starty = inImageFrameRect.top/imageheight;
	starty *= imageheight;
	for( ANumber x = startx; x < inImageFrameRect.right + inLeftOffset; x += imagewidth )
	{
		for( ANumber y = starty; y < inImageFrameRect.bottom; y += imageheight )
		{
			//Imageを貼る位置をImage座標系で取得
			AImageRect	pictimage = {0};
			pictimage.left 		= x - inLeftOffset;
			pictimage.top		= y;
			pictimage.right		= x - inLeftOffset + imagewidth;
			pictimage.bottom	= y + imageheight;
			//★未レビュー
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
背景Offscreenを取得
*/
/*#688
void	CWindowImp::GetBackgroundData( const AIndex inBackgroundImageIndex, GWorldPtr& outGWorldPtr, Rect& outBoundRect ) const
{
	outGWorldPtr = sBackgroundImageArray.Get(inBackgroundImageIndex);
	outBoundRect = sBackgroundImageArray_Bound.Get(inBackgroundImageIndex);
}
*/

//背景Offscreenデータ
/*#688
AArray<GWorldPtr>	CWindowImp::sBackgroundImageArray;
AObjectArray<AFileAcc>	CWindowImp::sBackgroundImageArray_File;
AArray<Rect>	CWindowImp::sBackgroundImageArray_Bound;
*/

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---イメージ

/**
イメージ登録
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
		/*サイズ縮小テスト
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
登録イメージ取得
*/
PicHandle	CWindowImp::GetImage( const AIndex inImageIndex )
{
	return sImagePicHandle.Get(inImageIndex);
}

//登録イメージデータ
AArray<PicHandle>	CWindowImp::sImagePicHandle;
#endif

#pragma mark ===========================

#pragma mark --- アイコン

/**
アイコン登録(static)
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
アイコン登録（リソース(CIcon)から登録・AIconIDは自動割り当て）
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
アイコン登録（リソース(CIcon)から登録・AIconIDは指定）
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
//デバッグ中
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
		/*文字書き込みテスト::MoveTo(0,100);
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
アイコン登録(static)
*/
AIconID	CWindowImp::RegisterDynamicIconFromFile( const AFileAcc& inFile, const ABool inLoadEvenIfNotCustomIcon )//#232
{
	OSStatus	err = noErr;
	
	FSSpec	fsspec;
	inFile.GetFSSpec(fsspec);
	AIconID	iconID = AssignDynamicIconID();
	//
	ABool	loaded = false;
	//#232 .icnsファイルの存在チェック
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
		//B0307 カスタムアイコン存在チェック追加
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
	//B0307 優先順位変更（カスタムアイコンが先）
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
	//B0307 それでもなければカスタムアイコンなしでもファイル・フォルダのアイコンを取得
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
アイコン登録解除
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
アイコン名前からIconID取得
*/
AIconID	CWindowImp::GetIconIDByName( const AText& inName )
{
	AIndex	index = sIconNameArray.Find(inName);
	if( index == kIndex_Invalid )   return kIconID_NoIcon;
	return sIconIDArray.Get(index);
}

//#232
/**
アイコンをファイルへコピー

現状、ファイルからロードした場合のみ対応。
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
アイコンファイル拡張子取得
*/
void	CWindowImp::GetIconFileSuffix( AText& outSuffix )
{
	outSuffix.SetCstring(".icns");
}

/**
動的割当IconIDを割り当てる
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
アイコン取得(static)
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
アイコン取得(static)
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
動的割り当てIconIDかどうかを取得
*/
ABool	CWindowImp::IsDynamicIcon( const AIconID inIconID )
{
	return ((inIconID>=kDynamicIconIDStart)&&(inIconID<=kDynamicIconIDEnd));
}

//データ
AHashArray<AIconID>	CWindowImp::sIconIDArray;
AArray<IconRef>	CWindowImp::sIconRefArray;
//#1034 AArray<CIconHandle>	CWindowImp::sCIconHandleArray;
AObjectArray<AFileAcc>	CWindowImp::sIconFileArray;//#232
AHashTextArray	CWindowImp::sIconNameArray;//#232
AIconID	CWindowImp::sNextDynamicIconID = kDynamicIconIDStart;

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---イベントハンドラ（OSコールバック用static）

/**
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_MouseDownHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-MDH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//マウスクリック位置のチェック（ウインドウ外は処理しない）
	WindowPartCode	partcode = 0;
	err = ::GetEventParameter(inEventRef,'wpar'/*kEventParamWindowPartCode*/,'wpar'/*typeWindowPartCode*/,NULL,sizeof(partcode),NULL,&partcode);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,NULL);
	if( partcode != inContent )   return result;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_MouseWheelHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-MWH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//マウスWheel位置のチェック（ウインドウ外は処理しない）
	WindowPartCode	partcode = 0;
	err = ::GetEventParameter(inEventRef,'wpar'/*kEventParamWindowPartCode*/,'wpar'/*typeWindowPartCode*/,NULL,sizeof(partcode),NULL,&partcode);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,NULL);
	if( partcode != inContent )   return result;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_DoMouseMoved( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-MM)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//マウスWheel位置のチェック（ウインドウ外は処理しない）
	WindowPartCode	partcode = 0;
	err = ::GetEventParameter(inEventRef,'wpar'/*kEventParamWindowPartCode*/,'wpar'/*typeWindowPartCode*/,NULL,sizeof(partcode),NULL,&partcode);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,NULL);
	if( partcode != inContent )   return result;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_CommandHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-CmH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	//コマンド取得
	HICommand	command = {0,0};
	err = ::GetEventParameter(inEventRef,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&command);
	//if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,NULL);
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_KeyDownHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-KDH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_TextInputHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TIH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_TextInputUpdateActiveInputAreaHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TIUAIAH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_TextInputOffsetToPosHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TIOTPH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_TextInputPosToOffsetHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TIPTOH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_TextInputGetSelectedText( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TIGST)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）B330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessLockDocument( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-Lock)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）B330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessUnlockDocument( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-Unlock)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）B330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessGetLength( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-GetLength)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）B330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessGetSelectedRange( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-GetSelectedRange)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）B330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessGetCharacters( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-GetCharacters)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）B330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessGetFont( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-GetFont)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）B330
*/
pascal OSStatus CWindowImp::STATIC_APICB_TSMDocumentAccessGetGlyphInfo( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-TSMDA-GetGlyphInfo)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowActivatedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-AH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowDeactivatedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-DH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowCollapsedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-CoH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowBoundsChangedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-BCH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(true);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowResizeCompletedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-RCH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowZoomedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-WZH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowCursorChangeHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-CCH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(true);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowFocusAcquiredHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-FAH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_ControlTrackHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-CTH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CWindowImp::STATIC_APICB_WindowCloseHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-ClH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
//イベントハンドラ（OSコールバック用static）
pascal OSStatus CWindowImp::STATIC_APICB_ContextMenuHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	fprintf(stderr,"TEST ");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
OSErr	CWindowImp::STATIC_APICB_DragTracking( DragTrackingMessage inMessage, AWindowRef inWindowRef, void * inHandlerRefCon, DragRef inDragRef )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-DrT)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inHandlerRefCon;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;
	
	//B0000 OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static）
*/
OSErr	CWindowImp::STATIC_APICB_DragReceive( AWindowRef inWindowRef, void * inHandlerRefCon, DragRef inDragRef )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-DrR)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inHandlerRefCon;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;
	
	//B0000 OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static） B0370
*/
pascal OSStatus CWindowImp::STATIC_APICB_ServiceGetTypes( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-SeGT)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static） B0370
*/
pascal OSStatus CWindowImp::STATIC_APICB_ServiceCopy( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-SeC)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static） B0370
*/
pascal OSStatus CWindowImp::STATIC_APICB_ServicePaste( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-SeP)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;//R-abs
	
	OSStatus	result = eventNotHandledErr;
	
	//CWindowImpオブジェクトのイベントハンドラ実行
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
イベントハンドラ（OSコールバック用static） B0370
*/
pascal OSStatus	CWindowImp::STATIC_APICB_ControlHelpTagCallback( ControlRef inControl,
			Point inGlobalMouse, HMContentRequest inRequest,
			HMContentProvidedType *outContentProvided,
			HMHelpContentPtr ioHelpContent )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-HelpTag)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクト取得
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
			
			//mouseポイントをLocalに変換
			StSetPort	s(window.GetWindowRef());
			Point	pt = inGlobalMouse;
			::GlobalToLocal(&pt);
			AWindowPoint	windowPoint;
			windowPoint.x = pt.h;
			windowPoint.y = pt.v;
			ALocalPoint	localPoint;
			window.GetControlLocalPointFromWindowPoint(controlID,windowPoint,localPoint);
			
			//rectは、ヘルプタグ描画位置（デフォルトでコントロールのrectを入れておく。EVT_DoGetHelpTag()で値が設定される）
			ALocalRect	rect;
			rect.left		= 0;
			rect.top		= 0;
			rect.right		= window.GetControlWidth(controlID);
			rect.bottom		= window.GetControlHeight(controlID);
			AText	text1, text2;
			//AWindow, AViewオブジェクトから、現在のマウスポインタに対応するヘルプタグ情報を取得
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
				
				//ヘルプタグ描画位置設定
				localPoint.x = rect.left;
				localPoint.y = rect.top;
				window.GetWindowPointFromControlLocalPoint(controlID,localPoint,windowPoint);
				pt.h = windowPoint.x;
				pt.v = windowPoint.y;
				::LocalToGlobal(&pt);
				ioHelpContent->absHotRect.left = pt.h;
				ioHelpContent->absHotRect.top = pt.v;
				
				//ヘルプタグ描画位置設定
				localPoint.x = rect.right;
				localPoint.y = rect.bottom;
				window.GetWindowPointFromControlLocalPoint(controlID,localPoint,windowPoint);
				pt.h = windowPoint.x;
				pt.v = windowPoint.y;
				::LocalToGlobal(&pt);
				ioHelpContent->absHotRect.right = pt.h;
				ioHelpContent->absHotRect.bottom = pt.v;
				
				//テキスト設定
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
			//テキストCFStringオブジェクト解放
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
イベントハンドラ登録
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
	kEventWindowContextualMenuSelectは、compositeではないウインドウ(StandardWindow Handler)用
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
イベントハンドラ削除
*/
void	CWindowImp::UninstallEventHandler()
{
	OSStatus	err = noErr;
	
	//イベントハンドラーの削除
	for( AIndex i = 0; i < mInstalledEventHandler.GetItemCount(); i++ )
	{
		err = ::RemoveEventHandler(mInstalledEventHandler.Get(i));
		if( err != noErr )   _ACErrorNum("RemoveEventHandler() returned error: ",err,this);
	}
	mInstalledEventHandler.DeleteAll();
}
#endif
#pragma mark ===========================

#pragma mark ---ユーティリティ

/**
現在のマウス位置取得
@note イベント受信時ではない、現在のマウス位置。イベント受信時だと、イベント受信後にウインドウ移動した場合などに問題発生するので、この関数を使う。
*/
void	CWindowImp::GetCurrentMouseLocation( AGlobalPoint& outGlobalPoint )
{
	//マウス位置取得
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	NSPoint	mouseScreenPoint = [NSEvent mouseLocation];
	outGlobalPoint.x = mouseScreenPoint.x;
	outGlobalPoint.y = firstScreenFrame.size.height - mouseScreenPoint.y;
}


#pragma mark ===========================

#pragma mark ---画面サイズ取得

//win AWindowWrapperから移動
/**
ウインドウ配置可能な領域を取得（Dock, Menubar等に重ならない領域）（(0,0)が存在するスクリーン内）
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
ウインドウ配置可能な領域を取得（Dock, Menubar等に重ならない領域）（ウインドウが存在するスクリーン内）
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
ウインドウ配置可能な領域を取得（Dock, Menubar等に重ならない領域）（指定スクリーン内）
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
指定ポイントからscreen indexを取得
*/
AIndex	CWindowImp::FindScreenIndexFromPoint( const APoint inPoint )
{
	//スクリーン毎のループ
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	AItemCount	screenCount = [[NSScreen screens] count];
	for( AIndex i = 0; i < screenCount; i++ )
	{
		//ウインドウ配置可能領域取得
		NSRect	screenVisibleFrame = [[[NSScreen screens] objectAtIndex:i] visibleFrame];
		ARect	screenBounds = {0};
		screenBounds.left		= screenVisibleFrame.origin.x;
		screenBounds.top		= firstScreenFrame.size.height - screenVisibleFrame.origin.y - screenVisibleFrame.size.height;
		screenBounds.right		= screenVisibleFrame.origin.x + screenVisibleFrame.size.width;
		screenBounds.bottom		= firstScreenFrame.size.height - screenVisibleFrame.origin.y;
		//ポイントが範囲内かどうかの判定
		if( inPoint.x >= screenBounds.left && inPoint.x <= screenBounds.right &&
					inPoint.y >= screenBounds.top && inPoint.y <= screenBounds.bottom )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

#pragma mark ===========================

#pragma mark ---LineBreak取得

/**
行Break位置の計算
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
		//処理無し
	}
	err = ::ATSUDisposeTextLayout(textLayout);
	if( err != noErr )   _ACErrorNum("ATSUDisposeTextLayout() returned error: ",err,NULL);
	err = ::ATSUDisposeStyle(atsuStyle);
	if( err != noErr )   _ACErrorNum("ATSUDisposeStyle() returned error: ",err,NULL);
	return endUniPos;
}
*/

//B0000 行折り返し計算高速化
/**
行Break位置の計算
*/
AItemCount	CWindowImp::GetLineBreak( CTextDrawData& ioTextDrawData, AItemCount inStartOffset, 
									  const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAlias,
									  const ANumber inViewWidth, const ABool inLetterWrap )//#1113
{
	OSStatus	err = noErr;
	
	/*#1034
	//ATSUStyle設定
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
	//TextLayout生成
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
		//表示テキスト取得
		AStCreateNSStringFromUTF16Text	str(ioTextDrawData.UTF16DrawText);
		//表示テキストのNSMutableAttributedString取得
		NSMutableAttributedString *attrString = [[NSMutableAttributedString alloc] initWithString:str.GetNSString()];
		//attrStringはイベントループ終了で解放。CTLineでコピーしているか参照しているかは不明だが、参照しているならretain()しているはず。
		[attrString autorelease];
		//NSMutableAttributedStringの編集開始
		[attrString beginEditing];
		
		//通常フォント取得
		/*#1090
		AStCreateNSStringFromAText	fontname(inFontName);
		NSFontDescriptor*	fontDescriptor = [NSFontDescriptor fontDescriptorWithName: fontname.GetNSString() 
																				 size: inFontSize];
		NSFont*	normalfont = [NSFont fontWithDescriptor:fontDescriptor size:inFontSize];
		if( normalfont == NULL )
		{
			//フォント取得できないときはシステムフォントを使用する
			normalfont = [NSFont systemFontOfSize:inFontSize];
		}
		*/
		NSFont*	normalfont = GetNSFont(inFontName,inFontSize,kTextStyle_Normal);
		// ==================== テキスト属性設定 ====================
		//range設定
		NSRange	range = NSMakeRange(0,[attrString length]);
		//フォント設定
		[attrString addAttribute:NSFontAttributeName value:normalfont range:range];
		
		//NSMutableAttributedStringの編集終了
		[attrString endEditing];
		
		//CTTypesetter取得
		ioTextDrawData.SetCTTypesetterRef(::CTTypesetterCreateWithAttributedString((CFAttributedStringRef)attrString));
	}
	
	//幅が小さすぎるとバイナリファイルの行送り計算で正体不明のバグが起こるので、一文字は表示できる程度の幅を確保する
	ANumber	minwidth = 24;
	ANumber	width = inViewWidth;
	if( width <= minwidth )   width = minwidth;
	
	//行折り返し位置取得
	CFIndex start = inStartOffset;
	CFIndex count = 0;
	if( inLetterWrap == false )
	{
		count = ::CTTypesetterSuggestLineBreak(ioTextDrawData.GetCTTypesetterRef(), start, width);
	}
	//#1113 文字ごと行折り返し
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
		//処理無し
	}
	*/
	//fprintf(stderr,"%d,%d  ",ioTextDrawData.mTextLayout,status);
	//ATSUStyle, TextLayoutは再利用し、CTextDrawDataの削除・Init時に解放
	/*::ATSUDisposeTextLayout(ioTextDrawData.mTextLayout);
	ioTextDrawData.mTextLayout = NULL;
	::ATSUDisposeStyle(ioTextDrawData.mATSUStyle);
	ioTextDrawData.mATSUStyle = NULL;*/
	
	return endUniPos;
}

//#853
/**
テキストの行折り返し計算（改行コード対応・UTF8でのlinestart, lengthを取得）
*/
void	CWindowImp::CalcLineBreaks( const AText& inText, 
									const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAlias,
									const ANumber inViewWidth, const ABool inLetterWrap, //#1113
									AArray<AIndex>& outUTF8LineStartArray, AArray<AItemCount>& outUTF8LineLengthArray )
{
	//結果初期化
	outUTF8LineStartArray.DeleteAll();
	outUTF8LineLengthArray.DeleteAll();
	//テキスト各文字毎のループ
	AItemCount	len = inText.GetItemCount();
	for( AIndex pos = 0; pos < len; )
	{
		//段落開始位置をparaStartに記憶
		AIndex	paraStart = pos;
		//段落取得
		AText	paraText;
		inText.GetLine(pos,paraText);
		//段落のtext draw dataを取得
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(paraText,1,0,true,true,false,0,paraText.GetItemCount());
		//折り返し計算（UTF16でのindexを取得）
		AArray<AIndex>	startOffsetArray, endOffsetArray;
		CalcLineBreaks(textDrawData,0,inFontName,inFontSize,inAntiAlias,inViewWidth,inLetterWrap,startOffsetArray,endOffsetArray);//#1113
		//テキストの最初からの、UTF8のposに変換して、結果格納
		for( AIndex i = 0; i < startOffsetArray.GetItemCount(); i++ )
		{
			//開始位置
			outUTF8LineStartArray.Add(paraStart + textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(startOffsetArray.Get(i)));
			//レングス
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
テキストの行折り返し計算（改行コード未対応・UTF16でのlineStart, lengthを取得）
*/
void	CWindowImp::CalcLineBreaks( CTextDrawData& ioTextDrawData, AItemCount inStartOffset, 
									const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAlias,
									const ANumber inViewWidth, const ABool inLetterWrap, //#1113
									AArray<AIndex>& outUTF16StartOffsetArray, AArray<AIndex>& outUTF16EndOffsetArray )
{
	//結果初期化
	outUTF16StartOffsetArray.DeleteAll();
	outUTF16EndOffsetArray.DeleteAll();
	//各行毎ループ
	AIndex	startOffset = inStartOffset;
	while( ioTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - startOffset > 0 )
	{
		//行折り返し位置取得
		AItemCount	endOffset = CWindowImp::GetLineBreak(ioTextDrawData,startOffset,inFontName,inFontSize,inAntiAlias,inViewWidth,inLetterWrap);//#1113
		if( endOffset <= startOffset )   endOffset = startOffset+1;//無限ループ防止
		
		//結果格納
		outUTF16StartOffsetArray.Add(startOffset);
		outUTF16EndOffsetArray.Add(endOffset);
		startOffset = endOffset;
	}
	//結果無しの場合の処理
	if( outUTF16StartOffsetArray.GetItemCount() == 0 )
	{
		outUTF16StartOffsetArray.Add(0);
		outUTF16EndOffsetArray.Add(0);
	}
}

//#1090
/**
NSFontキャッシュ
*/
AHashTextArray	CWindowImp::sNSFontCache_Key;
AArray<NSFont*>	CWindowImp::sNSFontCache_NSFont;
//最後に使ったフォントをキャッシュしておく #1275
NSFont*			CWindowImp::sLastNSFont = nil;
AText			CWindowImp::sLastNSFont_FontName;
AFloatNumber	CWindowImp::sLastNSFont_FontSize = 0;
ATextStyle		CWindowImp::sLastNSFont_TextStyle = kTextStyle_Normal;

//#1090
/**
NSFont取得
*/
NSFont*	CWindowImp::GetNSFont( const AText& inFontName, const AFloatNumber inFontSize, const ATextStyle inTextStyle )
{
	//最後に使ったキャッシュと同じフォントならそれを返す（sLast～アクセスがスレッドセーフではないのでメインスレッドの場合のみ） #1275
	if( ABaseFunction::InMainThread() == true )
	{
		if( sLastNSFont != nil && inFontName.Compare(sLastNSFont_FontName) == true && inFontSize == sLastNSFont_FontSize && inTextStyle == sLastNSFont_TextStyle )
				{
			return sLastNSFont;
		}
	}
	//フォント名・フォントサイズ・スタイルからkey文字列生成
	AText	key;
	key.SetText(inFontName);
	key.Add('\t');
	key.AddNumber((ANumber)(inFontSize*10.0));
	key.Add('\t');
	key.AddNumber((ANumber)(inTextStyle));
	//キャッシュから検索
	AIndex	cacheIndex = sNSFontCache_Key.Find(key);
	if( cacheIndex != kIndex_Invalid )
	{
		//キャッシュにヒットしたら、それを返す
		//キャッシュからフォント取得
		NSFont* font = sNSFontCache_NSFont.Get(cacheIndex);
		//最後に使ったキャッシュを保存する（sLast～アクセスがスレッドセーフではないのでメインスレッドの場合のみ） #1275
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
		//返り値
		NSFont*	font;
		//通常フォント取得
		AStCreateNSStringFromAText	fontname(inFontName);
		NSFontDescriptor*	fontDescriptor = [NSFontDescriptor fontDescriptorWithName: fontname.GetNSString() size:inFontSize];
		NSFont*	normalfont = [NSFont fontWithDescriptor:fontDescriptor size:inFontSize];
		if( normalfont == NULL )
		{
			//フォント取得できないときはシステムフォントを使用する
			normalfont = [NSFont systemFontOfSize:inFontSize];
		}
		if( inTextStyle == kTextStyle_Normal )
		{
			//通常フォント
			font = normalfont;
		}
		else
		{
			//bold, italicフォント
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
			//bold,italicフォント取得
			font = [[NSFontManager sharedFontManager] convertFont:normalfont toHaveTrait:fontTraitMask];
			//他に下記のような取得方法も試したが、どれも同じ（Monacoのboldは取得できない）
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
				//boldフォント取得できない場合、または、通常フォントと同じときはHelveticaのbold/italicを取得する
				font = [[NSFontManager sharedFontManager] fontWithFamily:@"Helvetica" traits:fontTraitMask 
				weight:weight size:inFontSize];
				if( font == NULL ) 
				{
					//フォント取得できないときは通常フォントを使用する
					font = normalfont;
				}
			}
		}
		//キャッシュに登録
		[font retain];//これがないと、タブありのウインドウを閉じて、再度、ウインドウを開いたときに落ちる。（fontオブジェクトはreleaseされるらしい。）
		sNSFontCache_Key.Add(key);
		sNSFontCache_NSFont.Add(font);
		//
		return font;
	}
}

#pragma mark ===========================

#pragma mark ---インライン入力

/**
インライン入力を強制確定
*/
void	CWindowImp::FixInlineInput( const AControlID inControlID )
{
	if( true )
	{
		//if( inControlID != kControlID_Invalid )
		{
			//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
			NSView*	view = CocoaObjects->FindViewByTag(inControlID);
			
			//viewのfixInlineInput実行
			[view fixInlineInput];
		}
		/*
		else
		{
			AItemCount	itemCount = mUserPaneArray_ControlID.GetItemCount();
			for( AIndex i = 0; i < itemCount; i++ )
			{
				AControlID	controlID = mUserPaneArray_ControlID.Get(i);
				
				//AControlID(=NSViewにtagとして設定されている)から、NSViewを検索
				NSView*	view = CocoaObjects->FindViewByTag(controlID);
				
				//viewのfixInlineInput実行
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

#pragma mark ---ATSUユーティリティ

/**
ATSUStyleを設定
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
	//フォント名からATSUFontID取得
	//１．まずデフォルトフォント指定ならデフォルトフォントを取得
	if( inFontName.GetItemCount() == 0 || inFontName.Compare("default") == true )
	{
		AText	defaultFontName;
		AFontWrapper::GetAppDefaultFontName(defaultFontName);
		fontID = AFontWrapper::GetATSUFontIDByName(defaultFontName);
	}
	//２．デフォルトフォント指定でない場合は、フォント名に従ってフォント取得
	if( fontID == 0 )
	{
		fontID = AFontWrapper::GetATSUFontIDByName(inFontName);
	}
	//３．フォント取得失敗の場合は、アプリケーションのデフォルトフォントを設定
	if( fontID == 0 )
	{
		AText	defaultFontName;
		AFontWrapper::GetAppDefaultFontName(defaultFontName);
		fontID = AFontWrapper::GetATSUFontIDByName(defaultFontName);
	}
	//４．アプリケーションデフォルトフォント取得失敗の場合は、dialog fontを設定（dialog fontはシステムで必ず有効なフォントが取得できる）
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
	if( inAntiAliasing )//B0183  アンチエイリアスOFF時はtracking設定しない。
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
	
	//B0000 高速化 
	//color, styleもmATSUStyleに設定できるようにする。DrawText()のcolor, style指定無し版で、mATSUStyleを使えばデフォルトcolor, styleが使用されるようにする。
	//（以下、基本的にDrawText()からコピー）
	
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
	
	//色データを追加
	tagValuePtr[0] = &color;
	err = ::ATSUSetAttributes(ioATSUStyle,1,attrTag,tagValueSize,tagValuePtr);
	if( err != noErr )   _ACErrorNum("ATSUSetAttributes() returned error: ",err,NULL);
	
	//スタイルデータを追加
	//DrawText()のrunstyle設定で、設定しなければデフォルト、とするために、bがtrueでもfalseでも値を設定する。
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

//一行に表示する文字数の最大 #408 （無制限にすると、改行データがないときに異常に遅くなる）#408
//#572 使用していないので const ANumber	kLineTextDrawMax = 100000;

/**
TextLayout生成
@note 生成したTextLayoutを解放するまで、inUTF16Textは解放不可。
*/
ATSUTextLayout	CWindowImp::CreateTextLayout( const ATSUStyle inATSUStyle, const AText& inUTF16Text, const ABool inAntiAliasing, 
				const ABool inUseFontFallback, ADrawContextRef inContextRef )//#703
{
	OSStatus	err = noErr;
	
	ATSUTextLayout	textLayout = NULL;
	{
		AItemCount	unilen = inUTF16Text.GetItemCount()/sizeof(UniChar);//#408
		//行折り返しありの場合、1段落が長くなりうるのでやはり制限は不可 if( unilen > kLineTextDrawMax )   unilen = kLineTextDrawMax;//#408
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
	//フォントフォールバック使用
	err = ::ATSUSetTransientFontMatching(textLayout,inUseFontFallback);//true);
	if( err != noErr )   _ACErrorNum("ATSUSetTransientFontMatching() returned error: ",err,NULL);
	//描画オプション設定
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
	//Font Fallback設定
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
	//CGContextを設定
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

#pragma mark ---ヘルプ

/**
Helpアンカー設定
*/
void	CWindowImp::RegisterHelpAnchor( const ANumber inHelpNumber, AConstCstringPtr inAnchor )
{
	AText	text;
	text.SetCstring(inAnchor);
	mHelpAnchor_Number.Add(inHelpNumber);
	mHelpAnchor_Anchor.Add(text);
}

/**
Helpページ表示
*/
void	CWindowImp::DisplayHelpPage( const ANumber inHelpNumber )
{
	AIndex	index = mHelpAnchor_Number.Find(inHelpNumber);
	if( index == kIndex_Invalid ) 
	{
		_AError("no help page registered",this);
		return;
	}
	//#539 CFBundleHelpBookNameはnet.mimikaki.mi.helpに変更したので、Apple Titleと一致しなくなった。
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
フルキーボードアクセス設定かどうかを判定する
*/
ABool	CWindowImp::STATIC_CheckIsFocusGroup2ndPattern()
{
	OSStatus	err = noErr;
	//チェック用ダミーウインドウ取得
	WindowRef	windowRef;
	IBNibRef	nibRef;
	err = ::CreateNibReference(CFSTR("AbsFrameworkCommonResource"),&nibRef);
	if( err != noErr )   {_ACErrorNum("CreateNibReference() returned error: ",err,NULL);return false;}
	err = ::CreateWindowFromNib(nibRef,CFSTR("CheckFullKeyboardAccess"),&windowRef);
	if( err != noErr )   {_ACErrorNum("CreateWindowFromNib() returned error: ",err,NULL);return false;}
	::DisposeNibReference(nibRef);
	//ID=1のコントロールにフォーカス設定
	HIViewID	viewid;
	viewid.signature = 0;
	viewid.id = 1;
	HIViewRef	controlRef = NULL;
	err = ::HIViewFindByID(::HIViewGetRoot(windowRef), viewid, &controlRef);
	err = ::SetKeyboardFocus(windowRef,controlRef,kControlFocusNextPart);
	//次のコントロールに移動
	err = ::HIViewAdvanceFocus(::HIViewGetRoot(windowRef),0);
	HIViewRef	nextControlRef = NULL;
	//現在のコントロール取得
	err = ::GetKeyboardFocus(windowRef,&nextControlRef);
	//ダミーウインドウ削除
	::DisposeWindow(windowRef);
	sIsFocusGroup2ndPattern =  (controlRef!=nextControlRef);
	if( nextControlRef == 0 )   sIsFocusGroup2ndPattern = false;//#494
	return sIsFocusGroup2ndPattern;
}

ABool	CWindowImp::sIsFocusGroup2ndPattern = false;
#endif

#pragma mark ===========================

#pragma mark ---ウインドウ取得
//win AWindowWrapperから移動

/**
最前面ウインドウ取得
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
		//orderedWindows取得
		NSArray*	windowsArray = [NSApp orderedWindows];
		if( windowsArray != nil )
		{
			//各ウインドウのループ
			NSUInteger	count = [windowsArray count];
			for( AIndex i = 0; i < count; i++ )
			{
				//ウインドウ取得
				NSWindow*	window = [windowsArray objectAtIndex:i];
				if( [window level] != NSFloatingWindowLevel )
				{
					//floating window以外で、最初に見つかったウインドウのwindowRefを返す
					AWindowRef	windowRef = [window windowNumber];//#1275 (AWindowRef)[window windowRef];
					return windowRef;
				}
			}
		}
		return NULL;
	}
}

/**
次のウインドウ取得
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
		//orderedWindows取得
		NSArray*	windowsArray = [NSApp orderedWindows];
		if( windowsArray != nil )
		{
			//各ウインドウのループ
			NSUInteger	count = [windowsArray count];
			for( AIndex i = 0; i < count; i++ )
			{
				//ウインドウ取得
				NSWindow*	window = [windowsArray objectAtIndex:i];
				//引数のウインドウとwindowRefが一致したら、次のウインドウを取得して、そのwindowRefを返す
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
N番目のウインドウ取得
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
		//orderedWindows取得
		NSArray*	windowsArray = [NSApp orderedWindows];
		if( windowsArray != nil )
		{
			//指定番目のウインドウのwindowRefを返す
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

#pragma mark ---縦書き用座標変換
//#558

/**
縦書き用座標変換（App->Imp）
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
縦書き用座標変換（App->Imp）
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
縦書き用座標変換（Imp->App）
*/
ALocalPoint	CWindowImp::ConvertVerticalCoordinateFromImpToApp( const AControlID inControlID, const ALocalPoint& inImpPoint ) const
{
	ALocalPoint	localPoint = {0};
	localPoint.x = inImpPoint.y;
	localPoint.y = GetControlWidth(inControlID) - inImpPoint.x;
	return localPoint;
}

/**
縦書き用座標変換（Imp->App）
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


