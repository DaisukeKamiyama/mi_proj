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

	CWindowImp (Windows)

*/

#include "stdafx.h"

#include "CWindowImp.h"
#include "CAppImp.h"
#include "../Frame.h"
#include "Commdlg.h"
#include <windowsx.h>
#include <commctrl.h>
#include <windows.h>
#include <strsafe.h>
#include <shellapi.h>
#include <shlobj.h>
#include "CRebarContent.h"
#include "gdiplus.h"
//#353 ABaseTypes.hにて定義するように変更 const AControlID	kControlID_TabControl = 99999;

extern const wchar_t*	kIdForDropFromAnother;

AArray<CWindowImp*>	CWindowImp::sAliveWindowArray;
AArray<HWND>		CWindowImp::sAliveWindowArray_HWnd;
AArray<HWND>		CWindowImp::sAliveWindowArray_HWnd_OverlayBackgroundWindow;
//AArray<HWND>		CWindowImp::sFloatingWindowArray;

#pragma mark ===========================
#pragma mark [クラス]CWindowImp
#pragma mark ===========================

#pragma mark --- コンストラクタ／デストラクタ

//コンストラクタ
CWindowImp::CWindowImp( AObjectArrayItem* inParent, AWindow& inAWin ) 
: AObjectArrayItem(inParent), mAWin(inAWin), mHWnd(NULL), mCurrentFocus(kControlID_Invalid), mLatentFocus(kControlID_Invalid), mEachMenu(NULL),
mCurrentTabIndex(kIndex_Invalid),mDefaultOKButton(kControlID_Invalid),mDefaultCancelButton(kControlID_Invalid)
,mParentWindowRef(NULL)
,mOverlayBackgroundColor(kColor_White), mHWndOverlayBackgroundWindow(NULL), mAlpha(255)
,mWindowClass(kWindowClass_Document)
,mNoActivate(false)
,mCurrentWindowSize_Width(-1),mCurrentWindowSize_Height(-1)
,mInputBackslashByYenKeyMode(false)
,mZoomed(false)
,mWindowModalParentWindowRef(NULL), mWindowModalParentMode(false)
,mFullScreenMode(false)//#404
,mTabControlHWnd(NULL)
,mWindowMinWidth(100),mWindowMinHeight(100)//#505
{
	//
	sAliveWindowArray.Add(this);
	sAliveWindowArray_HWnd.Add(0);
	sAliveWindowArray_HWnd_OverlayBackgroundWindow.Add(NULL);
}

//デストラクタ
CWindowImp::~CWindowImp()
{
	//
	AIndex	index = sAliveWindowArray.Find(this);
	if( index != kIndex_Invalid )
	{
		sAliveWindowArray.Delete1(index);
		sAliveWindowArray_HWnd.Delete1(index);
		sAliveWindowArray_HWnd_OverlayBackgroundWindow.Delete1(index);
	}
}

#pragma mark ===========================

#pragma mark --- AWindowを取得
//AWindow取得
AWindow&	CWindowImp::GetAWin()
{
	return mAWin;
}
//AWindow取得(const)
AWindow&	CWindowImp::GetAWinConst() const
{
	return mAWin;
}

#pragma mark ===========================

#pragma mark --- ウインドウ制御

//ウインドウ生成
void	CWindowImp::Create( const AWindowClass inWindowClass,
		const ABool inHasCloseButton, const ABool inHasCollapseButton, const ABool inHasZoomButton, const ABool inHasGrowBox,
		const ABool inIgnoreClick,//#379
		const AWindowRef inParentWindowRef, const ABool inCocoa )
{
	mWindowClass = inWindowClass;
	switch(inWindowClass)
	{
	  case kWindowClass_Document:
	  default:
		{
			mEachMenu = ::LoadMenuW(CAppImp::GetHResourceInstance(),MAKEINTRESOURCEW(IDC_APPMENU));//satDLL
			mHWnd = ::CreateWindowW(L"main",L"",WS_OVERLAPPEDWINDOW|WS_SIZEBOX,
			CW_USEDEFAULT,0,CW_USEDEFAULT,0,NULL,mEachMenu,CAppImp::GetHInstance(),NULL);
			//
			mNoActivate = false;
			break;
		}
	  case kWindowClass_Floating:
		{
			mHWnd = ::CreateWindowExW(
					WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_TOPMOST,//フルスクリーンでメインウインドウよりも上にするためにWS_EX_TOPMOSTも設定
					L"dialogs",L"",WS_SIZEBOX|WS_SYSMENU,
					CW_USEDEFAULT,0,CW_USEDEFAULT,0,NULL,NULL,CAppImp::GetHInstance(),NULL);
			//
			mNoActivate = true;
			break;
		}
	  case kWindowClass_Toolbar:
		{
			mHWnd = ::CreateWindowExW(
					WS_EX_LAYERED|WS_EX_TOOLWINDOW,
					L"dialogs",L"",/*これを設定するとメインウインドウがdeactされてしまうWS_VISIBLE|*/WS_POPUP,
					CW_USEDEFAULT,0,CW_USEDEFAULT,0,inParentWindowRef,NULL,CAppImp::GetHInstance(),NULL);
			//α設定
			//透明色キーカラーはmOverlayBackgroundColorで設定
			::SetLayeredWindowAttributes(mHWnd,mOverlayBackgroundColor,0,/*LWA_ALPHA|*/LWA_COLORKEY);
			//
			mNoActivate = true;
			break;
		}
	  case kWindowClass_Overlay:
		{
			/*
			Overlayウインドウ
			・別ウインドウ内のペインのように見えるウインドウ。実際には別ウインドウ。（独立したAWindowで動作する）
			
			・背景ウインドウ(mHWndOverlayBackgroundWindow)と、本ウインドウ(HWND)を作成する
			背景ウインドウには背景色を半透明に塗りつぶす
			本ウインドウには不透明で描画を行う
			
			*/
			//本ウインドウ
			//WS_EX_LAYERED: レイヤードウインドウを生成
			//WS_EX_TRANSPARENT: マウスイベントを背景ウインドウへ全て透過する（常に背景ウインドウから本ウインドウへリダイレクトさせる）
			// →MOUSELEAVEイベントを常に背景ウインドウの方で受信するようにする。透過しないと、透明でない部分と透明部分でleaveのきかたが変わるのでやっかい
			//WS_VISIBLE|WS_POPUP: 初期状態でshow、WS_POPUP: ウインドウ枠なし
			mHWnd = ::CreateWindowExW(
					WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW,
					L"dialogs",L"",/*これを設定するとメインウインドウがdeactされてしまうWS_VISIBLE|*/WS_POPUP,
					CW_USEDEFAULT,0,CW_USEDEFAULT,0,inParentWindowRef,NULL,CAppImp::GetHInstance(),NULL);
			//α設定
			//本ウインドウのアルファはなし
			//透明色キーカラーはmOverlayBackgroundColorを設定
			//#379 αとcolor keyを両方使用するようにする ::SetLayeredWindowAttributes(mHWnd,mOverlayBackgroundColor,0,/*LWA_ALPHA|*/LWA_COLORKEY);
			::SetLayeredWindowAttributes(mHWnd,mOverlayBackgroundColor,mAlpha,LWA_ALPHA|LWA_COLORKEY);
			
			//背景ウインドウ
			mHWndOverlayBackgroundWindow = ::CreateWindowExW(
					WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW,
					L"overlaybackground",L"",/*これを設定するとメインウインドウがdeactされてしまうWS_VISIBLE|*/WS_POPUP,
					CW_USEDEFAULT,0,CW_USEDEFAULT,0,inParentWindowRef,NULL,CAppImp::GetHInstance(),NULL);
			//α設定
			//#379 α=0に設定 ::SetLayeredWindowAttributes(mHWndOverlayBackgroundWindow,0,mAlpha,LWA_ALPHA);
			::SetLayeredWindowAttributes(mHWndOverlayBackgroundWindow,0,0,LWA_ALPHA);
			
			//mParentWindowRefを保存
			mParentWindowRef = inParentWindowRef;
			//親ウインドウへ登録
			if( mParentWindowRef != NULL )
			{
				AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mParentWindowRef);
				if( parentWindowIndex == kIndex_Invalid )   { _ACError("",this); }
				else
				{
					//親ウインドウへ登録
					(sAliveWindowArray.Get(parentWindowIndex))->RegisterOverlayWindow(mHWnd,mHWndOverlayBackgroundWindow);
				}
				/*
				::SetWindowPos(mHWndOverlayBackgroundWindow,mHWnd,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				::SetWindowPos(mParentWindowRef,mHWndOverlayBackgroundWindow,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				*/
			}
			//
			mNoActivate = true;
			break;
		}
	}
	DWORD	err = GetLastError();
	if( mHWnd == 0 )
	{
		_ACThrow("CreateWindow() failed",this);
	}
	
	AIndex	index = sAliveWindowArray.Find(this);
	if( index == kIndex_Invalid )
	{
		_ACThrow("",this);
	}
	sAliveWindowArray_HWnd.Set(index,mHWnd);
	sAliveWindowArray_HWnd_OverlayBackgroundWindow.Set(index,mHWndOverlayBackgroundWindow);
	
	//ファイルDrop許可
	::DragAcceptFiles(mHWnd,TRUE);
	
	//
	GetWindowBounds(mCurrentWindowBounds);
	mUnzoomedBounds = mCurrentWindowBounds;
	mCurrentWindowSize_Width = mCurrentWindowBounds.right - mCurrentWindowBounds.left;
	mCurrentWindowSize_Height = mCurrentWindowBounds.bottom - mCurrentWindowBounds.top;
	//背景Window更新
	UpdateOverlayBackgroundWindow();
	
	//#404 通常状態ウインドウサイズ記憶
	GetWindowBounds(mWindowBoundsInStandardState);
	//#404 フルスクリーン状態解除
	mFullScreenMode = false;
} 

void	CWindowImp::Create( AConstCharPtr inWindowName, const AWindowClass inWindowClass,
		const ABool inTabWindow, const AItemCount inTabCount, const ABool inCocoa )
{
	mWindowClass = inWindowClass;//★暫定 kWindowClass_Document以外はまともに動作しない
	//
	AText	name;
	//CAppImp::GetLangText(name);
	name.SetCstring("jp_");
	name.AddCstring(inWindowName);
	name.ReplaceChar('/','_');
	AStCreateWcharStringFromAText	windowname(name);
	mHWnd = ::CreateDialogW(CAppImp::GetHResourceInstance(),windowname.GetPtr(),0,NULL);//satDLL
	DWORD	err = GetLastError();
	if( mHWnd == 0 )
	{
		_ACError("CreateDialog() failed",this);
		return;
	}
	
	AIndex	index = sAliveWindowArray.Find(this);
	if( index == kIndex_Invalid )
	{
		_ACThrow("",this);
	}
	sAliveWindowArray_HWnd.Set(index,mHWnd);
	sAliveWindowArray_HWnd_OverlayBackgroundWindow.Set(index,NULL);
	
	//タブ
	if( inTabWindow == true )
	{
		//ID=65534にタブコントロールの位置を示すためのダミーコントロールを置いておく
		HWND	dummycontrol = ::GetDlgItem(mHWnd,kControlID_TabControl);
		RECT	bounds;
		::GetWindowRect(dummycontrol,&bounds);
		HWND	parent = ::GetParent(dummycontrol);
		ARect	parentbounds;
		GetWindowBounds(parentbounds);
		//ダミーコントロール削除
		::DestroyWindow(dummycontrol);
		//
		mTabControlHWnd = ::CreateWindowExW(0,WC_TABCONTROLW,NULL,WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
				bounds.left - parentbounds.left, bounds.top - parentbounds.top,
				bounds.right - bounds.left, bounds.bottom - bounds.top,
				mHWnd,(HMENU)kControlID_TabControl,CAppImp::GetHInstance(),NULL);
		::SendMessage(mTabControlHWnd,WM_SETFONT,(LPARAM)::GetStockObject(DEFAULT_GUI_FONT),(WPARAM)FALSE);
		for( AIndex i = 0; i < inTabCount; i++ )
		{
			AText	pagenum;
			pagenum.SetFormattedCstring("%d",i);
			AText	pagename;
			pagename.SetCstring(inWindowName);
			pagename.ReplaceChar('/','_');
			pagename.AddText(pagenum);
			AText	tabtitle;
			tabtitle.SetLocalizedText(pagename);
			AStCreateWcharStringFromAText	tabtitlestr(tabtitle);
			TC_ITEMW	tc;
			tc.mask = TCIF_TEXT;
			tc.pszText = (LPWSTR)tabtitlestr.GetPtr();
			::SendMessageW(mTabControlHWnd,TCM_INSERTITEMW,(WPARAM)i,(LPARAM)&tc);
			AText	localizedPagename;
			//CAppImp::GetLangText(localizedPagename);
			localizedPagename.SetCstring("jp_");
			localizedPagename.AddText(pagename);
			AStCreateWcharStringFromAText	pagenamestr(localizedPagename);
			HWND	hpage = ::CreateDialogW(CAppImp::GetHResourceInstance(),pagenamestr.GetPtr(),mTabControlHWnd,NULL);//satDLL
			mTabControlArray.Add(hpage);
		}
		RECT	tabdisplay;
		::GetClientRect(mTabControlHWnd,&tabdisplay);
		TabCtrl_AdjustRect(mTabControlHWnd,FALSE,&tabdisplay);
		for( AIndex i = 0; i < inTabCount; i++ )
		{
			::MoveWindow(mTabControlArray.Get(i),tabdisplay.left,tabdisplay.top,
				tabdisplay.right-tabdisplay.left,tabdisplay.bottom-tabdisplay.top,FALSE);
		}
		mCurrentTabIndex = 0;
		UpdateTab();
	}
	
	::EnumChildWindows(mHWnd,STATIC_APICB_EnumChildProc,(LPARAM)mHWnd);
	
	//コンボボックスへ固定文字列設定
	for( AIndex i = 0; i < mControlArray_ID.GetItemCount(); i++ )
	{
		if( GetControlTypeByID(mControlArray_ID.Get(i)) == kControlType_ComboBox )
		{
			AText	text;
			text.SetCstring(inWindowName);
			text.ReplaceChar('/','_');
			text.Add('_');
			AText	idtext;
			idtext.SetNumber(mControlArray_ID.Get(i));
			text.AddText(idtext);
			text.Add('_');
			for( AIndex j = 0; ; j++ )
			{
				AText	numtext;
				numtext.SetNumber(j);
				AText	t;
				t.SetText(text);
				t.AddText(numtext);
				AText	combotext;
				if( CAppImp::ExistLocalizedText(t) == false )
				{
					break;
				}
				else
				{
					combotext.SetLocalizedText(t);
					AStCreateWcharStringFromAText	str(combotext);
					::SendMessageW(GetHWndByControlID(mControlArray_ID.Get(i)),CB_INSERTSTRING,-1,
							(LPARAM)(str.GetPtr()));
				}
			}
		}
	}
	
	//ファイルDrop許可
	::DragAcceptFiles(mHWnd,TRUE);
	
	//
	mNoActivate = false;
	
	//#404 通常状態ウインドウサイズ記憶
	GetWindowBounds(mWindowBoundsInStandardState);
	//#404 フルスクリーン状態解除
	mFullScreenMode = false;
}

BOOL CALLBACK CWindowImp::STATIC_APICB_EnumChildProc( HWND hwnd, LPARAM lParam )
{
	AIndex	index = sAliveWindowArray_HWnd.Find((HWND)lParam);
	if( index == kIndex_Invalid )  {_ACError("",NULL);return FALSE;}
	//登録
	(sAliveWindowArray.Get(index))->RegisterControl(hwnd);
	return TRUE;
}

void	CWindowImp::RegisterControl( const HWND hwnd )
{
	AControlID	controlID = ::GetWindowLong(hwnd,GWL_ID);
	//親ウインドウのクラスがmain, dialogsではない場合はcontrolIDは0として登録
	HWND	parent = ::GetParent(hwnd);
	AText	parentname;
	if( parent != NULL )
	{
		char	namestr[256];
		::GetClassNameA(parent,namestr,256);
		parentname.SetCstring(namestr);
	}
	if( parentname.Compare("main") != true && parentname.Compare("dialogs") != true &&
		parentname.Compare("ReBarWindow32") != true && parentname.Compare("userpane") != true )
	{
		controlID = 0;
	}
	mControlArray_ID.Add(controlID);
	mControlArray_HWnd.Add(hwnd);
	mControlArray_HFont.Add(NULL);
	mControlArray_DataType.Add(kDataType_NotFound);
	mControlArray_OriginalProc.Add((WNDPROC)::GetWindowLong(hwnd,GWL_WNDPROC));
	::SetWindowLong(hwnd,GWL_WNDPROC,(LONG)STATIC_APICB_ControlSubclassProc);
	//コントロールのクラス取得
	char	namestr[256];
	::GetClassNameA(hwnd,namestr,256);
	AText	name;
	name.SetCstring(namestr);
	AIndex	index = sControlWindowClass_Name.Find(name);
	ABool	autoselectall = false;
	if( index == kIndex_Invalid )
	{
		mControlArray_ControlType.Add(kControlType_Invalid);
	}
	else
	{
		mControlArray_ControlType.Add(sControlWindowClass_Type.Get(index));
	if( sControlWindowClass_Type.Get(index) == kControlType_Edit )
	{
		autoselectall = true;
	}
	}
	mControlArray_CatchReturn.Add(false);
	mControlArray_CatchTab.Add(false);
	mControlArray_AutoSelectAllBySwitchFocus.Add(autoselectall);
}

void	CWindowImp::UnregisterControl( const AControlID inID )
{
	AIndex	index = mControlArray_ID.Find(inID);
	mControlArray_ID.Delete1(index);
	mControlArray_HWnd.Delete1(index);
	HFONT	hfont = mControlArray_HFont.Get(index);
	if( hfont != NULL )
	{
		::DeleteObject(hfont);
	}
	mControlArray_HFont.Delete1(index);
	mControlArray_DataType.Delete1(index);
	mControlArray_OriginalProc.Delete1(index);
	mControlArray_ControlType.Delete1(index);
	mControlArray_CatchReturn.Delete1(index);
	mControlArray_CatchTab.Delete1(index);
	mControlArray_AutoSelectAllBySwitchFocus.Delete1(index);
}

/**
コントロールのサブクラス
*/
LRESULT	CWindowImp::APICB_ControlSubclassProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	//
	AIndex	index = mControlArray_HWnd.Find(hwnd);
	if( index == kIndex_Invalid )  { _ACError("",this); return 0L; }
	WNDPROC	originalProc = mControlArray_OriginalProc.Get(index);//下記処理実行によりmControlArray_OriginalProcが削除される可能性があるため先に取得
	//ControlID取得　なお、WinAPI自動登録コントロールのcontrolIDは0になる（STATIC_APICB_EnumChildProcを参照）
	AControlID	controlID = mControlArray_ID.Get(index);
	//現在のフォーカス位置と違うコントロールをfocusした場合は、フォーカスコントロールへValueChangedを通知
	if( message == WM_SETFOCUS )
	{
		AControlID	oldFocus = GetCurrentFocus();
		if( oldFocus != kControlID_Invalid && oldFocus != controlID )
		{
			GetAWin().EVT_ValueChanged(oldFocus);
		}
	}
	//WindowModalの親ウインドウの場合、マウスクリックを無効にする
	if( mWindowModalParentMode == true )
	{
		switch(message)
		{
		  case WM_LBUTTONDOWN:
		  case WM_RBUTTONDOWN:
		  case WM_LBUTTONDBLCLK:
		  case WM_RBUTTONDBLCLK:
		  case WM_LBUTTONUP:
		  case WM_RBUTTONUP:
		  case WM_KEYDOWN:
		  case WM_CHAR:
		  case WM_IME_COMPOSITION:
			{
				return 0;
			}
		}
	}
	//
	switch(mControlArray_ControlType.Get(index))
	{
	  case kControlType_Edit:
		{
			switch(message)
			{
			  case WM_COMMAND:
				{
					if( lParam == 0 )
					{
						int wmId, wmEvent;
						wmId    = LOWORD(wParam);
						wmEvent = HIWORD(wParam);
						switch( wmId )
						{
						  case kMenuItemID_Copy:
							{
								::SendMessage(hwnd,WM_COPY,0,0);
								return 0L;
							}
						  case kMenuItemID_Cut:
							{
								::SendMessage(hwnd,WM_CUT,0,0);
								return 0L;
							}
						  case kMenuItemID_Paste:
							{
								::SendMessage(hwnd,WM_PASTE,0,0);
								return 0L;
							}
						  case kMenuItemID_Clear:
							{
								::SendMessage(hwnd,WM_CLEAR,0,0);
								return 0L;
							}
						  case kMenuItemID_SelectAll:
							{
								::SendMessage(hwnd,EM_SETSEL,0,-1);
								return 0L;
							}
						  case kMenuItemID_Undo:
							{
								::SendMessageW(hwnd,EM_UNDO,0,0);
								return 0L;
							}
						  case kMenuItemID_Redo:
							{
								::SendMessageW(hwnd,EM_UNDO,0,0);
								return 0L;
							}
						}
						break;
					}
					break;
				}
				//キー入力
				//Tab等WM_KEYDOWNで処理しようとすると、sysbeepが鳴ることがある（WM_CHARで入力不可として先に判断されている？）
			  case WM_CHAR:
				{
					switch(wParam)
					{
						//リターンキー
					  case kUChar_CR:
						{
							if( mControlArray_CatchReturn.Get(index) == false )
							{
								//
								if( mCurrentFocus != kControlID_Invalid )
								{
									//コントロール変更を通知
									GetAWin().EVT_ValueChanged(mCurrentFocus);
									//
									SetSelectionAll(mCurrentFocus);
								}
								//
								if( mDefaultOKButton != kControlID_Invalid )
								{
									if( IsControlEnabled(mDefaultOKButton) == true )
									{
										GetAWin().EVT_Clicked(mDefaultOKButton,0);
									}
								}
								break;
							}
							break;
						}
						//タブキー
					  case kUChar_Tab:
						{
							if( mControlArray_CatchTab.Get(index) == false )
							{
								//
								if( mCurrentFocus != kControlID_Invalid )
								{
									//コントロール変更を通知
									GetAWin().EVT_ValueChanged(mCurrentFocus);
									//
									//SetSelectionAll(mCurrentFocus);
								}
								//フォーカス移動
								AModifierKeys	modifiers = AKeyWrapper::GetEventKeyModifiers();
								if( AKeyWrapper::IsShiftKeyPressed(modifiers) == false )
								{
									GetAWin().NVI_SwitchFocusToNext();
								}
								else
								{
									GetAWin().NVI_SwitchFocusToPrev();
								}
								return 0L;
							}
							break;
						}
						//ESCキー
					  case kUChar_Escape:
						{
							if( mDefaultCancelButton != kControlID_Invalid )
							{
								if( IsControlEnabled(mDefaultCancelButton) == true )
								{
									GetAWin().EVT_Clicked(mDefaultCancelButton,0);
								}
							}
							break;
						}
					}
					break;
				}
			  case WM_SETFOCUS:
				{
					if( controlID != kIndex_Invalid && controlID != 0 )
					{
						SetFocus(controlID);
					}
					else
					{
						//controlID未登録の場合は親を試してみる
						AIndex	parentindex = mControlArray_HWnd.Find(::GetParent(hwnd));
						if( parentindex != kIndex_Invalid )
						{
							controlID = mControlArray_ID.Get(parentindex);
							if( controlID != kIndex_Invalid && controlID != 0 )
							{
								SetFocus(controlID);
							}
						}
					}
					break;
				}
			  default:
				{
					break;
				}
			}
			break;
		}
	  case kControlType_UserPane:
		{
			AIndex	index = mUserPaneArray_ControlID.Find(controlID);
			if( index == kIndex_Invalid )   break;
			switch( mUserPaneArray_Type.Get(index) )
			{
			  case kUserPaneType_ColorPickerPane:
				{
					switch(message)
					{
					  case WM_CHAR:
						{
							switch(wParam)
							{
								//タブキー
							  case kUChar_Tab:
								{
									if( mControlArray_CatchTab.Get(index) == false )
									{
										//フォーカス移動
										AModifierKeys	modifiers = AKeyWrapper::GetEventKeyModifiers();
										if( AKeyWrapper::IsShiftKeyPressed(modifiers) == false )
										{
											GetAWin().NVI_SwitchFocusToNext();
										}
										else
										{
											GetAWin().NVI_SwitchFocusToPrev();
										}
										return 0L;
									}
									break;
								}
							}
							break;
						}
					}
				}
				break;
			}
			break;
		}
	  default:
		{
			switch(message)
			{
			  case WM_PAINT:
				{
					//フォーカス破線描画
					//先にWindowProcを実行してから、独自に破線を描画する
					if( mCurrentFocus != kControlID_Invalid && AEnvWrapper::GetOSVersion() >= kOSVersion_WindowsVista )
					{
						if( GetHWndByControlID(mCurrentFocus) == hwnd )
						{
							switch(mControlArray_ControlType.Get(index))
							{
							  case kControlType_Button:
								{
									LONG	buttontype = (::GetWindowLongW(
											mControlArray_HWnd.Get(index),GWL_STYLE)&BS_TYPEMASK);
									if( buttontype == BS_CHECKBOX || 
												buttontype == BS_RADIOBUTTON ||
												buttontype == BS_AUTOCHECKBOX || 
												buttontype == BS_AUTORADIOBUTTON )
									{
										//チェックボックス・ラジオボタンの場合：周りを点線で囲む
										RECT	rect;
										::GetWindowRect(hwnd,&rect);
										LRESULT	lret = ::CallWindowProcW(originalProc,hwnd,message,wParam,lParam);
										RECT	r;
										r.left		= 0;
										r.top		= 0;
										r.right		= rect.right-rect.left;
										r.bottom	= rect.bottom-rect.top;
										HDC	hdc = ::GetDC(hwnd);
										::DrawFocusRect(hdc,&r);
										::ReleaseDC(hwnd,hdc);
										return lret;
									}
									else
									{
										//上記以外のボタンの場合：内部を点線で囲む
										RECT	rect;
										::GetWindowRect(hwnd,&rect);
										LRESULT	lret = ::CallWindowProcW(originalProc,hwnd,message,wParam,lParam);
										RECT	r;
										r.left		= 3;
										r.top		= 3;
										r.right		= rect.right-rect.left-3;
										r.bottom	= rect.bottom-rect.top-3;
										HDC	hdc = ::GetDC(hwnd);
										::DrawFocusRect(hdc,&r);
										::ReleaseDC(hwnd,hdc);
										return lret;
									}
									break;
								}
							  case kControlType_ComboBox:
								{
									if( (::GetWindowLongW(mControlArray_HWnd.Get(index),GWL_STYLE)&CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST )
									{
										//リストボックスの場合：内部を点線で囲む
										RECT	rect;
										::GetWindowRect(hwnd,&rect);
										LRESULT	lret = ::CallWindowProcW(originalProc,hwnd,message,wParam,lParam);
										RECT	r;
										r.left		= 3;
										r.top		= 3;
										r.right		= rect.right-rect.left-3;
										r.bottom	= rect.bottom-rect.top-3;
										HDC	hdc = ::GetDC(hwnd);
										::DrawFocusRect(hdc,&r);
										::ReleaseDC(hwnd,hdc);
										return lret;
									}
									break;
								}
							  default:
								{
									if( mCurrentFocus == kControlID_TabControl )
									{
										//タブコントロールの場合
										LRESULT	lret = ::CallWindowProcW(originalProc,hwnd,message,wParam,lParam);
										RECT	rect;
										TabCtrl_GetItemRect(hwnd,TabCtrl_GetCurSel(hwnd),&rect);
										HDC	hdc = ::GetDC(hwnd);
										::DrawFocusRect(hdc,&rect);
										::ReleaseDC(hwnd,hdc);
										return lret;
									}
									break;
								}
							}
						}
					}
					break;
				}
				//フォーカス設定
			  case WM_SETFOCUS:
				{
					GetAWin().NVI_SwitchFocusTo(controlID);
					break;
				}
				//キー入力
			  case WM_KEYDOWN:
				{
					switch(wParam)
					{
						//タブキー
					  case VK_TAB:
						{
							//フォーカス移動
							AModifierKeys	modifiers = AKeyWrapper::GetEventKeyModifiers();
							if( AKeyWrapper::IsShiftKeyPressed(modifiers) == false )
							{
								GetAWin().NVI_SwitchFocusToNext();
							}
							else
							{
								GetAWin().NVI_SwitchFocusToPrev();
							}
							return 0L;
							break;
						}
						//カーソルキーによるフォーカス移動
					  case VK_UP:
						{
							GetAWin().EVT_ArrowKey(controlID,kUChar_Up);
							break;
						}
					  case VK_DOWN:
						{
							GetAWin().EVT_ArrowKey(controlID,kUChar_Down);
							break;
						}
					  case VK_LEFT:
						{
							GetAWin().EVT_ArrowKey(controlID,kUChar_Left);
							break;
						}
					  case VK_RIGHT:
						{
							GetAWin().EVT_ArrowKey(controlID,kUChar_Right);
							break;
						}
						//デフォルトCancelボタン
					  case VK_ESCAPE:
						{
							if( mDefaultCancelButton != kControlID_Invalid )
							{
								if( IsControlEnabled(mDefaultCancelButton) == true )
								{
									GetAWin().EVT_Clicked(mDefaultCancelButton,0);
								}
							}
							break;
						}
						//デフォルトOKボタン
					  case VK_RETURN:
						{
							if( mDefaultOKButton != kControlID_Invalid )
							{
								if( IsControlEnabled(mDefaultOKButton) == true )
								{
									GetAWin().EVT_Clicked(mDefaultOKButton,0);
								}
							}
							break;
						}
					}
					break;
				}
				//マウス移動
			  case WM_MOUSEMOVE:
				{
					ACursorWrapper::ShowCursor();
					//サイズボックスのマウスカーソル設定
					if( mControlArray_ControlType.Get(index) == kControlType_ScrollBar )
					{
						if( (GetWindowLong(hwnd,GWL_STYLE)&SBS_SIZEBOX) != 0 )
						{
							ACursorWrapper::SetCursor(kCursorType_ResizeNWSE);
						}
					}
					break;
				}
			  default:
				{
					break;
				}
			}
		}
	}
	return ::CallWindowProcW(originalProc,hwnd,message,wParam,lParam);
}

LRESULT CALLBACK CWindowImp::STATIC_APICB_ControlSubclassProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	//イベントトランザクション前処理／後処理 #0 ここにこれがないと、Refresh等経由でOSからイベントCB発生したときに、そこでsEventDepth==1となってしまい、
	//Window等のオブジェクトに対してDoObjectArrayItemTrashDelete()が発生し、メンバー変数へのメモリアクセスが発生し、落ちてしまう。
	AStEventTransactionPrePostProcess	oneevent(false);
	
	AIndex	index = sAliveWindowArray_HWnd.Find((HWND)::GetParent(hwnd));
	if( index == kIndex_Invalid )
	{
		index = sAliveWindowArray_HWnd.Find((HWND)::GetAncestor(hwnd,GA_ROOT));
	}
	if( index == kIndex_Invalid )  {_ACError("",NULL);return FALSE;}
	return (sAliveWindowArray.Get(index))->APICB_ControlSubclassProc(hwnd,msg,wp,lp);
}

/**
WindowをDrawerに設定
@param inParent 親ウインドウ
*/
void	CWindowImp::SetWindowStyleToDrawer( CWindowImp& inParent )
{
	//★暫定 Windowsにdrawerなし
}

/**
WindowをSheetに設定
@param inParent 親ウインドウ
*/
void	CWindowImp::SetWindowStyleToSheet( CWindowImp& inParent )
{
	//TOPMOSTにする
	HWND	parent = inParent.GetHWnd();
	RECT	parentrect;
	::GetWindowRect(parent,&parentrect);
	RECT	rect;
	::GetWindowRect(mHWnd,&rect);
	::SetWindowPos(mHWnd,
			HWND_TOPMOST,
			parentrect.left + (parentrect.right-parentrect.left)/2 - (rect.right-rect.left)/2,
			parentrect.top + 64,
			0,0,SWP_NOSIZE);
	//WindowModalの親ウインドウにWindowModalParentModeを設定
	mWindowModalParentWindowRef = parent;
	inParent.SetWindowModalParentMode(true);
}

//
void	CWindowImp::Show( const int nCmdShow )
{
	int	cmdShow = nCmdShow;
	//非表示中にZoom()コールされていたら、SW_SHOWをSW_SHOWMAXIMIZED等に置き換えて表示
	if( nCmdShow == SW_SHOW && mZoomed == true )
	{
		if( mNoActivate == true )
		{
			cmdShow = SW_MAXIMIZE;
		}
		else
		{
			cmdShow = SW_SHOWMAXIMIZED;
		}
	}
	//ウインドウをアクティブにしないなら、SW_SHOWはSW_SHOWNAに置き換えて表示
	if( nCmdShow == SW_SHOW && mNoActivate == true )
	{
		cmdShow = SW_SHOWNA;
	}
	//
	if( mHWndOverlayBackgroundWindow != NULL )
	{
		int	cmdShowBackground = cmdShow;
		if( cmdShowBackground == SW_SHOW )   cmdShowBackground = SW_SHOWNA;
		::ShowWindow(mHWndOverlayBackgroundWindow,cmdShowBackground);
	}
	//ウインドウを表示
	::ShowWindow(mHWnd,cmdShow);
	//
	DWORD err = ::GetLastError();
	::UpdateWindow(mHWnd);
	//フォーカスを復元
	if( mLatentFocus != kControlID_Invalid )
	{
		SetFocus(mLatentFocus);
		mLatentFocus = kControlID_Invalid;
	}
	
	//WindowModalの親ウインドウのWindowModalParentModeを設定
	if( mWindowModalParentWindowRef != NULL )
	{
		AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mWindowModalParentWindowRef);
		if( parentWindowIndex == kIndex_Invalid )   { _ACError("",this); }
		else
		{
			//親ウインドウモーダル状態解除
			(sAliveWindowArray.Get(parentWindowIndex))->SetWindowModalParentMode(true);
		}
	}
}

//
void	CWindowImp::Hide()
{
	//
	if( mHWndOverlayBackgroundWindow != NULL )
	{
		::ShowWindow(mHWndOverlayBackgroundWindow,SW_HIDE);
	}
	::ShowWindow(mHWnd,SW_HIDE);
	
	//WindowModalの親ウインドウのWindowModalParentModeを解除
	if( mWindowModalParentWindowRef != NULL )
	{
		AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mWindowModalParentWindowRef);
		if( parentWindowIndex == kIndex_Invalid )   { _ACError("",this); }
		else
		{
			//親ウインドウモーダル状態解除
			(sAliveWindowArray.Get(parentWindowIndex))->SetWindowModalParentMode(false);
		}
	}
}

//
void	CWindowImp::Zoom()
{
	if( IsVisible() == true )
	{
		Show(SW_MAXIMIZE);
	}
	mZoomed = true;
}

//
void	CWindowImp::Select()
{
	//::SetWindowPos(mHWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);★どっちが正解？
	::SetActiveWindow(mHWnd);
}

//
void	CWindowImp::SendBehind( CWindowImp& inBeforeWindow )
{
	//mHWndが他のウインドウよりも後ろにいくことがある対策として、inBeforeWindowのほうを最前面に持ってくる #379
	::SetWindowPos(inBeforeWindow.GetHWnd(),HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	//
	::SetWindowPos(mHWnd,inBeforeWindow.GetHWnd(),0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

//#567
/**
ウインドウを最背面に移動
*/
void	CWindowImp::SendBehindAll()
{
	::SetWindowPos(mHWnd,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

/*使わないし、使うならkWindowClassを定義したい。
//
void	CWindowImp::SetTopMost()
{
	::SetWindowPos(mHWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}
*/

//
void	CWindowImp::CloseWindow()
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	/*
	//
	AIndex	floatingindex = sFloatingWindowArray.Find(mHWnd);
	if( floatingindex != kIndex_Invalid )
	{
		sFloatingWindowArray.Delete1(floatingindex);
	}
	*/
	
	//child windowクローズ
	//#291により子ウインドウ機構削除
	
	//TextArrayMenu登録解除
	while( mTextArrayMenuArray_ControlID.GetItemCount() > 0 )
	{
		UnregisterTextArrayMenu(mTextArrayMenuArray_ControlID.Get(0));
	}
	
	//フォーカスグループ削除
	//#135 mFocusGroup.DeleteAll();
	
	//フォーカス設定解除
	mLatentFocus = kControlID_Invalid;
	mCurrentFocus = kControlID_Invalid;
	
	//radiogroup削除
	//#349 mRadioGroup.DeleteAll();
	
	//コントロール削除
	while(mControlArray_ID.GetItemCount()>0)
	{
		DeleteControl(mControlArray_ID.Get(0));
	}
	
	//UserPane削除
	mUserPaneArray.DeleteAll();
	mUserPaneArray_ControlID.DeleteAll();
	mUserPaneArray_Type.DeleteAll();
	
	//
	if( mEachMenu != NULL )
	{
		AMenuWrapper::DeleteMenu(mEachMenu);
	}
	
	//
	if( mHWndOverlayBackgroundWindow != NULL )
	{
		::DestroyWindow(mHWndOverlayBackgroundWindow);
		mHWndOverlayBackgroundWindow = NULL;
	}
	
	//Overlayウインドウの場合、親ウインドウの登録を解除する
	if( mWindowClass == kWindowClass_Overlay && mParentWindowRef != NULL )
	{
		AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mParentWindowRef);
		if( parentWindowIndex == kIndex_Invalid )   { _ACError("",this); }
		else
		{
			//親ウインドウへ登録
			(sAliveWindowArray.Get(parentWindowIndex))->UnregisterOverlayWindow(mHWnd);
		}
	}
	
	//
	mParentWindowRef = NULL;
	
	//WindowModalの親ウインドウのWindowModalParentModeを解除
	if( mWindowModalParentWindowRef != NULL )
	{
		AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mWindowModalParentWindowRef);
		if( parentWindowIndex == kIndex_Invalid )   { _ACError("",this); }
		else
		{
			//親ウインドウモーダル状態解除
			(sAliveWindowArray.Get(parentWindowIndex))->SetWindowModalParentMode(false);
		}
	}
	//
	mWindowModalParentWindowRef = NULL;
	
	//
	mTabControlHWnd = NULL;
	
	//
	::DestroyWindow(mHWnd);
	mHWnd = NULL;
	
	/*
	//
	mControlArray_ID.DeleteAll();
	mControlArray_HWnd.DeleteAll();
	mControlArray_HFont.DeleteAll();
	mControlArray_DataType.DeleteAll();
	mControlArray_OriginalProc.DeleteAll();
	mControlArray_ControlType.DeleteAll();
	mControlArray_CatchReturn.DeleteAll();
	mControlArray_CatchTab.DeleteAll();
	mControlArray_AutoSelectAllBySwitchFocus.DeleteAll();
	*/
	//
	mTabControlArray.DeleteAll();
	//#291
	mOverlayWindowGroup_WindowRef.DeleteAll();
	mOverlayWindowGroup_BackgroundWindowRef.DeleteAll();
	mOverlayWindowGroup_XOffset.DeleteAll();
	mOverlayWindowGroup_YOffset.DeleteAll();
	//win
	mEventCatchDisabledControlArray.DeleteAll();
	
	//#404
	mFullScreenMode = false;
}

//
void	CWindowImp::RefreshWindow()
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneのOffscreenのうち表示中のものを全て再描画する
	for( AIndex index = 0; index < mUserPaneArray.GetItemCount(); index++ )
	{
		if( mUserPaneArray.GetObjectConst(index).IsControlVisible() == true )
		{
			mUserPaneArray.GetObject(index).RefreshControl();
		}
	}
	//ウインドウ全体に対してWM_PAINTイベントを発生させる
	::InvalidateRect(mHWnd,NULL,FALSE);
}

/*#688
//
void	CWindowImp::RefreshWindowRect( const AWindowRect& inWindowRect )
{
	//指定領域内のUserPaneのOffscreenのうち表示中のものを全て再描画する
	for( AIndex index = 0; index < mUserPaneArray.GetItemCount(); index++ )
	{
		if( mUserPaneArray.GetObject(index).IsControlVisible() == true )
		{
			RECT	winrect, panerect, intersect;
			::GetWindowRect(mHWnd,&winrect);
			::GetWindowRect(GetHWndByControlID(mUserPaneArray_ControlID.Get(index)),&panerect);
			::IntersectRect(&intersect,&winrect,&panerect);
			if( ::IsRectEmpty(&intersect) == FALSE )
			{
				mUserPaneArray.GetObject(index).RefreshControl();
			}
		}
	}
	//指定領域に対してWM_PAINTイベントを発生させる
	RECT	rect;
	rect.left		= inWindowRect.left;
	rect.top		= inWindowRect.top;
	rect.right		= inWindowRect.right;
	rect.bottom		= inWindowRect.bottom;
	::InvalidateRect(mHWnd,&rect,FALSE);
}
*/
//
ABool	CWindowImp::IsPrintMode() const
{
	//★暫定 Windowsの印刷はモーダルなのでたぶん不要（false固定でOK）
	return false;
}

//
void	CWindowImp::CollapseWindow( const ABool inCollapse )
{
	if( inCollapse == true )
	{
		::ShowWindow(mHWnd,SW_MINIMIZE);
	}
	else
	{
		::ShowWindow(mHWnd,SW_RESTORE);
	}
}

//
AHashTextArray	CWindowImp::sControlWindowClass_Name;
AArray<AControlType>	CWindowImp::sControlWindowClass_Type;

//
void	CWindowImp::InitWindowClass()
{
	WNDCLASSEXW	wndclass;
	wndclass.cbSize			= sizeof(WNDCLASSEXW);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= CWindowImp::STATIC_APICB_WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= CAppImp::GetHResourceInstance();//CAppImp::GetHInstance();
	wndclass.hIcon			= ::LoadIconW(CAppImp::GetHResourceInstance(),MAKEINTRESOURCE(IDI_APPLICATION));
	wndclass.hCursor		= ::LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground	= NULL;//NULLにしないと、リサイズ時等に、OSに全体消去され、ちらつく。(HBRUSH)(COLOR_WINDOW+1);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= L"main";
	wndclass.hIconSm		= ::LoadIcon(CAppImp::GetHResourceInstance(),MAKEINTRESOURCE(IDI_APPLICATION));
	
	if( RegisterClassExW(&wndclass) == 0 )
	{
		_ACError("RegisterClass() failed",NULL);
	}
	
	wndclass.cbSize			= sizeof(WNDCLASSEXW);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= CWindowImp::STATIC_APICB_WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= DLGWINDOWEXTRA;
	wndclass.hInstance		=CAppImp::GetHResourceInstance();//CAppImp::GetHInstance();
	wndclass.hIcon			= ::LoadIconW(CAppImp::GetHResourceInstance(),MAKEINTRESOURCE(IDI_APPLICATION));
	wndclass.hCursor		= ::LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= L"dialogs";
	wndclass.hIconSm		= ::LoadIcon(CAppImp::GetHResourceInstance(),MAKEINTRESOURCE(IDI_APPLICATION));
	
	if( RegisterClassExW(&wndclass) == 0 )
	{
		DWORD	err = GetLastError();
		_ACError("RegisterClass() failed",NULL);
	}
	
	wndclass.cbSize			= sizeof(WNDCLASSEXW);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= CWindowImp::STATIC_APICB_WndProcOverlayBackground;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= DLGWINDOWEXTRA;
	wndclass.hInstance		= CAppImp::GetHResourceInstance();//CAppImp::GetHInstance();
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= ::LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= L"overlaybackground";
	wndclass.hIconSm		= NULL;
	
	if( RegisterClassExW(&wndclass) == 0 )
	{
		DWORD	err = GetLastError();
		_ACError("RegisterClass() failed",NULL);
	}
	
	//userpane class
	//CUserPane::RegisterUserPaneWindow();
	
	//message-only window
	wndclass.cbSize			= sizeof(WNDCLASSEXW);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= CWindowImp::STATIC_APICB_WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= CAppImp::GetHResourceInstance();//CAppImp::GetHInstance();
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= NULL;
	wndclass.hbrBackground	= NULL;
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= kIdForDropFromAnother;
	wndclass.hIconSm		= NULL;
	if( RegisterClassExW(&wndclass) == 0 )
	{
		DWORD	err = GetLastError();
		_ACError("RegisterClass() failed",NULL);
	}
	
	AText	name;
	name.SetCstring("userpane");
	sControlWindowClass_Name.Add(name);
	sControlWindowClass_Type.Add(kControlType_UserPane);
	name.SetCstring("ScrollBar");
	sControlWindowClass_Name.Add(name);
	sControlWindowClass_Type.Add(kControlType_ScrollBar);
	name.SetCstring("Button");
	sControlWindowClass_Name.Add(name);
	sControlWindowClass_Type.Add(kControlType_Button);
	name.SetCstring("Edit");
	sControlWindowClass_Name.Add(name);
	sControlWindowClass_Type.Add(kControlType_Edit);
	name.SetCstring("ComboBox");
	sControlWindowClass_Name.Add(name);
	sControlWindowClass_Type.Add(kControlType_ComboBox);
	name.SetCstring("Static");
	sControlWindowClass_Name.Add(name);
	sControlWindowClass_Type.Add(kControlType_Static);
	name.SetCstring("msctls_trackbar32");
	sControlWindowClass_Name.Add(name);
	sControlWindowClass_Type.Add(kControlType_Slider);
	name.SetCstring("msctls_progress32");
	sControlWindowClass_Name.Add(name);
	sControlWindowClass_Type.Add(kControlType_ProgressBar);

	INITCOMMONCONTROLSEX	icc;
	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_STANDARD_CLASSES | ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_COOL_CLASSES ;
	::InitCommonControlsEx(&icc);
	
}

//#291
/**
オーバーレイウインドウを登録する（親側でコール）
*/
void	CWindowImp::RegisterOverlayWindow( const AWindowRef inChildWindow, const AWindowRef inChildBackgroundWindow )
{
	mOverlayWindowGroup_WindowRef.Add(inChildWindow);
	mOverlayWindowGroup_BackgroundWindowRef.Add(inChildBackgroundWindow);
	mOverlayWindowGroup_XOffset.Add(0);
	mOverlayWindowGroup_YOffset.Add(0);
	SetOverlayWindowOffset(inChildWindow);
}

/**
オーバーレイウインドウ登録を解除する（親側でコール）
*/
void	CWindowImp::UnregisterOverlayWindow( const AWindowRef inChildWindow )
{
	AIndex	index = mOverlayWindowGroup_WindowRef.Find(inChildWindow);
	if( index != kIndex_Invalid )
	{
		mOverlayWindowGroup_WindowRef.Delete1(index);
		mOverlayWindowGroup_BackgroundWindowRef.Delete1(index);
		mOverlayWindowGroup_XOffset.Delete1(index);
		mOverlayWindowGroup_YOffset.Delete1(index);
	}
}

/**
*/
void	CWindowImp::SetOverlayWindowOffset( const AWindowRef inChildWindow )
{
	AIndex	index = mOverlayWindowGroup_WindowRef.Find(inChildWindow);
	if( index == kIndex_Invalid )   { _ACError("",this); return; }
	
	RECT	parentrect, childrect;
	::GetWindowRect(mHWnd,&parentrect);
	::GetWindowRect(inChildWindow,&childrect);
	mOverlayWindowGroup_XOffset.Set(index,childrect.left - parentrect.left);
	mOverlayWindowGroup_YOffset.Set(index,childrect.top - parentrect.top);
}

//#291
/**
自分（オーバーレイウインドウ）が属するWindowGroupのMoveTogether属性をOn/Offする（子側でコール）
*/
void	CWindowImp::SetOverlayWindowMoveTogether( const ABool inMoveTogether )
{
	if( inMoveTogether == true && mParentWindowRef != NULL )
	{
		AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mParentWindowRef);
		if( parentWindowIndex == kIndex_Invalid )   { _ACError("",this); return; }
		//
		(sAliveWindowArray.Get(parentWindowIndex))->SetOverlayWindowOffset(mHWnd);
	}
}

//#320
/**
自分（オーバーレイウインドウ）が属するWindowGroupのkWindowGroupAttrLayerTogether属性をOn/Offする（子側でコール）
*/
void	CWindowImp::SetOverlayLayerTogether( const ABool inLayerTogether )
{
	//★暫定 おそらく対応不要
}

/**
Overlayウインドウの背景Window描画
*/
void	CWindowImp::UpdateOverlayBackgroundWindow()
{
	//Overlayウインドウ用背景Windowが存在する場合は、同じ位置・サイズに変更し、背景描画
	if( mHWndOverlayBackgroundWindow != NULL )
	{
		//本Overlayウインドウのrect取得
		RECT	rect;
		::GetWindowRect(mHWnd,&rect);
		//本Overlayウインドウの背後・同じ位置・サイズに設定
		::SetWindowPos(mHWndOverlayBackgroundWindow,mHWnd,
				rect.left,
				rect.top,
				rect.right-rect.left,
				rect.bottom-rect.top,
				SWP_NOACTIVATE);
		//背景描画（背景色塗りつぶし）
		HDC	hdc = ::GetDC(mHWndOverlayBackgroundWindow);
		if( hdc == NULL )   {_ACError("",NULL);return;}
		HBRUSH	hbr = ::CreateSolidBrush(mOverlayBackgroundColor);
		if( hbr == NULL )   {_ACError("",NULL);return;}
		RECT	drect;
		drect.left		= 0;
		drect.top		= 0;
		drect.right		= rect.right-rect.left;
		drect.bottom		= rect.bottom-rect.top;
		::FillRect(hdc,&drect,hbr);
		::DeleteObject(hbr);
		::ReleaseDC(mHWndOverlayBackgroundWindow,hdc);
	}
}

/**
*/
void	CWindowImp::PaintRect( const AWindowRect& inRect, const AColor& inColor )
{
	HDC	hdc = ::GetDC(mHWnd);
	if( hdc == NULL )   {_ACError("",NULL);return;}
	HBRUSH	hbr = ::CreateSolidBrush(inColor);
	if( hbr == NULL )   {_ACError("",NULL);return;}
	WINDOWINFO	wininfo;
	::GetWindowInfo(mHWnd,&wininfo);
	RECT	rect;
	rect.left		= inRect.left;
	rect.top		= inRect.top;
	rect.right		= inRect.right;
	rect.bottom		= inRect.bottom;
	::FillRect(hdc,&rect,hbr);
	::DeleteObject(hbr);
	::ReleaseDC(mHWnd,hdc);
}

//#409
/**
ウインドウを明示的に（即時）更新（WM_PAINTでWndProcが直接コールされ、ウインドウ即時更新される）
*/
void	CWindowImp::UpdateWindow()
{
	::UpdateWindow(mHWnd);
}

//#404
/**
フルスクリーンモード
*/
void	CWindowImp::SetFullScreenMode( const ABool inFullScreenMode )
{
	if( inFullScreenMode == true && mFullScreenMode == false )
	{
		//ウインドウ領域記憶
		GetWindowBounds(mWindowBoundsInStandardState);
		//フルスクリーン設定
		//ChangeDisplaySettingsW(NULL,CDS_FULLSCREEN);
		::SetWindowPos(mHWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		//#476 フルスクリーン時にウインドウサイズを記憶しないように先にフルスクリーンモード設定
		mFullScreenMode = inFullScreenMode;
		//ウインドウ領域設定
		ARect	rect;
		rect.left = 0;
		rect.top = ::GetSystemMetrics(SM_CYMENU);
		rect.right = ::GetSystemMetrics(SM_CXSCREEN);
		rect.bottom = ::GetSystemMetrics(SM_CYSCREEN);
		SetWindowBounds(rect);
	}
	else if( inFullScreenMode == false && mFullScreenMode == true )
	{
		//フルスクリーン解除
		//ChangeDisplaySettingsW(NULL,0);
		::SetWindowPos(mHWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		//#476 フルスクリーン時にウインドウサイズを記憶しないように先にフルスクリーンモード設定
		mFullScreenMode = inFullScreenMode;
		//ウインドウ領域を元に戻す
		SetWindowBounds(mWindowBoundsInStandardState);
	}
	//#476 mFullScreenMode = inFullScreenMode;
}

//#505
/**
ウインドウの最小サイズ設定
*/
void	CWindowImp::SetWindowMinimumSize( const ANumber inWidth, const ANumber inHeight )
{
	mWindowMinWidth = inWidth;
	mWindowMinHeight = inHeight;
}

#pragma mark ===========================

#pragma mark --- ウインドウ属性設定

//
void	CWindowImp::SetTitle( const AText& inText )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	AStCreateWcharStringFromAText	str(inText);
	::SetWindowTextW(mHWnd,str.GetPtr());
}

//
void	CWindowImp::SetWindowTransparency( const AFloatNumber inTransparency )
{
	//
	mAlpha = static_cast<BYTE>(255*inTransparency);
	//
	if( mHWnd != NULL )
	{
		if( mWindowClass == kWindowClass_Overlay )
		{
			/*
			if( mAlpha < 255 )
			{
				::SetWindowLong(mHWndOverlayBackgroundWindow,GWL_EXSTYLE,
						::GetWindowLong(mHWndOverlayBackgroundWindow,GWL_EXSTYLE)|WS_EX_LAYERED);
			}
			else
			{
				::SetWindowLong(mHWndOverlayBackgroundWindow,GWL_EXSTYLE,
						::GetWindowLong(mHWndOverlayBackgroundWindow,GWL_EXSTYLE)&~WS_EX_LAYERED);
			}
			*/
			::SetLayeredWindowAttributes(mHWndOverlayBackgroundWindow,0,mAlpha,LWA_ALPHA);
		}
		else
		{
			if( mAlpha < 255 )
			{
				::SetWindowLong(mHWnd,GWL_EXSTYLE,::GetWindowLong(mHWnd,GWL_EXSTYLE)|WS_EX_LAYERED);
			}
			else
			{
				::SetWindowLong(mHWnd,GWL_EXSTYLE,::GetWindowLong(mHWnd,GWL_EXSTYLE)&~WS_EX_LAYERED);
			}
			::SetLayeredWindowAttributes(mHWnd,0,mAlpha,LWA_ALPHA);
		}
	}
}

//
void	CWindowImp::SetModified( const ABool inModified )
{
	//Windowsでは対応する処理無し
}

//
void	CWindowImp::SetDefaultOKButton( const AControlID inID )
{
	mDefaultOKButton = inID;
}

//
void	CWindowImp::SetDefaultCancelButton( const AControlID inID )
{
	mDefaultCancelButton = inID;
}
//
void	CWindowImp::SetWindowFile( const AFileAcc& inFile )
{
	//Windowsでは対応する処理無し
}

//
void	CWindowImp::ClearWindowFile()
{
	//Windowsでは対応する処理無し
}

/*
//
void	CWindowImp::SetFloating()
{
	sFloatingWindowArray.Add(mHWnd);
	//ウインドウをアクティブにしない
	mNoActivate = true;
}
*/

/**
*/
HBRUSH	CWindowImp::GetBackgroundBlash() const
{
	return (HBRUSH)::GetClassLong(mHWnd,GCL_HBRBACKGROUND);
}

#pragma mark ===========================

#pragma mark --- ウインドウ情報取得

//
ABool	CWindowImp::ExistControl( const AControlID inID ) const
{
	return (mControlArray_ID.Find(inID)!=kIndex_Invalid);
}

//
void	CWindowImp::GetTitle( AText& outText ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//
	outText.DeleteAll();
	int	len = ::GetWindowTextLengthW(mHWnd)+1;
	outText.Reserve(0,len*sizeof(wchar_t));
	{
		AStTextPtr	textptr(outText,0,outText.GetItemCount());
		::GetWindowTextW(mHWnd,(LPWSTR)(textptr.GetPtr()),textptr.GetByteCount()/sizeof(wchar_t));
	}
	//ヌル文字以降を削除
	outText.DeleteAfterUTF16NullChar();
	outText.ConvertToUTF8FromAPIUnicode();
}

//
ABool	CWindowImp::IsWindowCreated() const
{
	return (mHWnd!=0);
}

//
ABool	CWindowImp::IsVisible() const
{
	return ::IsWindowVisible(mHWnd);
}

//
ABool	CWindowImp::IsActive() const
{
	return (::IsWindowVisible(mHWnd)==true)&&(::GetActiveWindow()==mHWnd);//★暫定　フローティングウインドウ等検討必要
}

//
ABool	CWindowImp::HasCloseButton() const
{
	DWORD	cs = ::GetClassLong(mHWnd,GCL_STYLE);
	if( (cs&CS_NOCLOSE) == 0 )   return true;
	else return false;
}

//
ABool	CWindowImp::IsFloatingWindow() const
{
	/*
	AIndex	floatingindex = sFloatingWindowArray.Find(mHWnd);
	return (floatingindex!=kIndex_Invalid);
	*/
	return (mWindowClass==kWindowClass_Floating);
}

//
ABool	CWindowImp::HasTitleBar() const
{
	LONG	s = ::GetWindowLong(mHWnd,GWL_STYLE);
	if( (s&WS_CAPTION ) != 0 )   return true;
	else return false;
}

//
ABool	CWindowImp::IsModalWindow() const
{
	//★暫定 下記以外のモーダル対応
	if( mWindowModalParentMode == true )   return true;
	return false;
}

//
ABool	CWindowImp::IsResizable() const
{
	LONG	s = ::GetWindowLong(mHWnd,GWL_STYLE);
	if( (s&WS_SIZEBOX  ) != 0 )   return true;
	else return false;
}

//
ABool	CWindowImp::IsZoomable() const
{
	//★暫定
	return false;
}

//
ABool	CWindowImp::IsZoomed() const
{
	return (::IsZoomed(mHWnd)==TRUE);
}

/**
*/
void	CWindowImp::GetWindowBoundsInStandardState( ARect& outBounds ) const
{
	outBounds.left		= mUnzoomedBounds.left;
	outBounds.top		= mUnzoomedBounds.top;
	outBounds.right		= mUnzoomedBounds.right;
	outBounds.bottom	= mUnzoomedBounds.bottom;
}

//WindowRef取得
AWindowRef	CWindowImp::GetWindowRef() const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACThrow("Window not created",this);return NULL;}
	
	return mHWnd;
}

#pragma mark ===========================

#pragma mark --- ウインドウサイズ／位置Get/Set

//
void	CWindowImp::GetWindowBounds( ARect& outRect ) const
{
	WINDOWINFO	wininfo;
	::GetWindowInfo(mHWnd,&wininfo);
	outRect.left		= wininfo.rcClient.left;
	outRect.top			= wininfo.rcClient.top;
	outRect.right		= wininfo.rcClient.right;
	outRect.bottom		= wininfo.rcClient.bottom;
}

/**
*/
void	CWindowImp::GetWindowBounds( AGlobalRect& outRect ) const
{
	WINDOWINFO	wininfo;
	::GetWindowInfo(mHWnd,&wininfo);
	outRect.left		= wininfo.rcClient.left;
	outRect.top			= wininfo.rcClient.top;
	outRect.right		= wininfo.rcClient.right;
	outRect.bottom		= wininfo.rcClient.bottom;
}

//
void	CWindowImp::SetWindowBounds( const ARect& inRect )
{
	SetWindowBounds(mHWnd,inRect);
	//背景Window更新
	UpdateOverlayBackgroundWindow();
}
void	CWindowImp::SetWindowBounds( const AWindowRef inWindowRef, const ARect& inRect )
{
	ARect	clientRect;
	GetWindowBounds(clientRect);
	RECT	bounds;
	::GetWindowRect(inWindowRef,&bounds);
	
	if( ((inRect.right - inRect.left) == (clientRect.right - clientRect.left)) &&
				((inRect.bottom - inRect.top) == (clientRect.bottom - clientRect.top)) )
	{
		//サイズが同じ場合（SetWindowPos内で処理最適化するかもしれないが、念のため）
		::SetWindowPos(inWindowRef,HWND_TOP,
				inRect.left	- (clientRect.left - bounds.left),
				inRect.top	- (clientRect.top - bounds.top),
				0,0,
				SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
	}
	else
	{
		//サイズ・位置とも違う場合
		::SetWindowPos(inWindowRef,HWND_TOP,
				inRect.left	- (clientRect.left - bounds.left),
				inRect.top	- (clientRect.top - bounds.top),
				inRect.right - inRect.left + (clientRect.left - bounds.left) + (bounds.right - clientRect.right),
				inRect.bottom - inRect.top + (clientRect.top - bounds.top) + (bounds.bottom - clientRect.bottom),
				SWP_NOZORDER|SWP_NOACTIVATE);
	}
	//#476 フルスクリーンモードでないときのウインドウサイズを記憶
	if( mFullScreenMode == false )
	{
		GetWindowBounds(mWindowBoundsInStandardState);
	}
}
void	CWindowImp::SetWindowBounds( const AGlobalRect& inRect )
{
	SetWindowBounds(mHWnd,inRect);
	//背景Window更新
	UpdateOverlayBackgroundWindow();
}
void	CWindowImp::SetWindowBounds( const AWindowRef inWindowRef, const AGlobalRect& inRect )
{
	ARect	clientRect;
	GetWindowBounds(clientRect);
	RECT	bounds;
	::GetWindowRect(inWindowRef,&bounds);
	
	if( ((inRect.right - inRect.left) == (clientRect.right - clientRect.left)) &&
				((inRect.bottom - inRect.top) == (clientRect.bottom - clientRect.top)) )
	{
		//サイズが同じ場合（SetWindowPos内で処理最適化するかもしれないが、念のため）
		::SetWindowPos(inWindowRef,HWND_TOP,
				inRect.left	- (clientRect.left - bounds.left),
				inRect.top	- (clientRect.top - bounds.top),
				0,0,
				SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
	}
	else
	{
		//サイズ・位置とも違う場合
		::SetWindowPos(inWindowRef,HWND_TOP,
				inRect.left	- (clientRect.left - bounds.left),
				inRect.top	- (clientRect.top - bounds.top),
				inRect.right - inRect.left + (clientRect.left - bounds.left) + (bounds.right - clientRect.right),
				inRect.bottom - inRect.top + (clientRect.top - bounds.top) + (bounds.bottom - clientRect.bottom),
				SWP_NOZORDER|SWP_NOACTIVATE);
	}
	//背景Window更新
	UpdateOverlayBackgroundWindow();
	//#476 フルスクリーンモードでないときのウインドウサイズを記憶
	if( mFullScreenMode == false )
	{
		GetWindowBounds(mWindowBoundsInStandardState);
	}
}

//
ANumber	CWindowImp::GetWindowWidth() const
{
	ARect	rect;
	GetWindowBounds(rect);
	return rect.right - rect.left;
}

//
void	CWindowImp::SetWindowWidth( const ANumber inWidth )
{
	ARect	rect;
	GetWindowBounds(rect);
	rect.right = rect.left + inWidth;
	SetWindowBounds(rect);
}

//
ANumber	CWindowImp::GetWindowHeight() const
{
	ARect	rect;
	GetWindowBounds(rect);
	return rect.bottom - rect.top;
}

//
void	CWindowImp::SetWindowHeight( const ANumber& inHeight )
{
	ARect	rect;
	GetWindowBounds(rect);
	rect.bottom = rect.top + inHeight;
	SetWindowBounds(rect);
}

//ウインドウサイズ設定
void	CWindowImp::SetWindowSize( const ANumber inWidth, const ANumber inHeight )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	ARect	rect;
	GetWindowBounds(rect);
	rect.right	= rect.left + inWidth;
	rect.bottom	= rect.top + inHeight;
	SetWindowBounds(rect);
}

//
void	CWindowImp::GetWindowPosition( APoint& outPoint ) const
{
	ARect	rect;
	GetWindowBounds(rect);
	outPoint.x = rect.left;
	outPoint.y = rect.top;
}

//
void	CWindowImp::SetWindowPosition( const APoint& inPoint )
{
	/*
	ANumber	w = GetWindowWidth();
	ANumber	h = GetWindowHeight();
	ARect	rect;
	rect.left		= inPoint.x;
	rect.top		= inPoint.y;
	rect.right		= inPoint.x + w;
	rect.bottom		= inPoint.y + h;
	SetWindowBounds(rect);
	*/
	ARect	clientRect;
	GetWindowBounds(clientRect);
	RECT	bounds;
	::GetWindowRect(mHWnd,&bounds);
	
	::SetWindowPos(mHWnd,HWND_TOP,
			inPoint.x	- (clientRect.left - bounds.left),
			inPoint.y	- (clientRect.top - bounds.top),
			0,
			0,
			SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
	//背景Window更新
	UpdateOverlayBackgroundWindow();
	//#476 フルスクリーンモードでないときのウインドウサイズを記憶
	if( mFullScreenMode == false )
	{
		GetWindowBounds(mWindowBoundsInStandardState);
	}
}

//
void	CWindowImp::ConstrainWindowPosition( const ABool inAllowPartial )
{
	//#404 フルスクリーン中は位置補正しない
	if( mFullScreenMode == true )   return;
	
	int	cx = ::GetSystemMetrics(SM_CXSCREEN);
	int cy = ::GetSystemMetrics(SM_CYSCREEN);
	AGlobalRect	rect;
	GetWindowBounds(rect);
	if( rect.right < 0 || rect.bottom < 0 || rect.left > cx || rect.top > cy )
	{
		//ウインドウがまったくスクリーン内に表示されていない場合、固定位置(50,50)に表示する
		APoint	pt = {50,50};
		SetWindowPosition(pt);
	}
	//★暫定 inAllowPartila未対応 （常にallowPartial）
}

//#385
/**
ウインドウサイズを画面に合わせて補正
*/
void	CWindowImp::ConstrainWindowSize()
{
	//#404 フルスクリーン中は位置補正しない
	if( mFullScreenMode == true )   return;
	
	//画面サイズ取得
	AGlobalRect	globalRect;
	CWindowImp::GetAvailableWindowPositioningBounds(globalRect);
	//
	RECT	winrect;
	::GetWindowRect(mHWnd,&winrect);
	//ウインドウサイズが画面サイズより大きければ、サイズ補正
	if( (winrect.bottom-winrect.top) > (globalRect.bottom-globalRect.top) )
	{
		//
		ARect	clientrect;
		GetWindowBounds(clientrect);
		//サイズ設定
		ANumber	delta = (globalRect.bottom-globalRect.top) - (winrect.bottom-winrect.top);
		clientrect.bottom += delta;
		SetWindowBounds(clientrect);
		//きっちり収まるように、allowPartial=falseで位置補正
		ConstrainWindowPosition(false);
	}
}

//
ABool	CWindowImp::IsWindowCollapsed() const
{
	//★暫定_ACError("not implemented",NULL);
	return false;
}

#pragma mark ===========================

#pragma mark --- コントロール制御

//
void	CWindowImp::DrawControl( const AControlID inID )
{
	//UserPaneのOffscreenを再描画する
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).RefreshControl();
	}
	//WM_PAINTイベントを発生させる
	::InvalidateRect(GetHWndByControlID(inID),NULL,FALSE);
}

#pragma mark ===========================

#pragma mark --- コントロール属性設定インターフェイス

//
void	CWindowImp::SetEnableControl( const AControlID inID, const ABool inEnable )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetEnableControl(inEnable);
		UpdateFocus();
		DrawControl(inID);
		return;
	}
	//通常コントロールの場合
	if( inEnable == true )
	{
		::EnableWindow(GetHWndByControlID(inID),TRUE);
	}
	else
	{
		::EnableWindow(GetHWndByControlID(inID),FALSE);
		/*#349 ラジオグループの全てのボタンをdissableにするときにOnが移動するのでこの処理はアプリ層で必要なときに行うことにする
		//ラジオグループに設定されたボタンの場合、Onでdisableにされたら、他のボタンのどれかをOnにする。 
		if( GetControlTypeByID(inID) == kControlType_Button )
		{
		if( GetBool(inID) == true )
		{
			UpdateRadioButtonDisabled(inID);
		}
		}
		*/
	}
	UpdateFocus();
}

//
ABool	CWindowImp::IsControlEnabled( const AControlID inID ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsControlEnabled();
	}
	//通常コントロールの場合
	return (::IsWindowEnabled(GetHWndByControlID(inID))==TRUE);
}

//
void	CWindowImp::SetShowControl( const AControlID inID, const ABool inShow )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetShowControl(inShow);
		return;
	}
	//通常コントロールの場合
	if( inShow == true )
	{
		::ShowWindow(GetHWndByControlID(inID),SW_SHOWNORMAL);
	}
	else
	{
		::ShowWindow(GetHWndByControlID(inID),SW_HIDE);
	}
}

//
ABool	CWindowImp::IsControlVisible( const AControlID inID ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsControlVisible();
	}
	//通常コントロールの場合
	return (::IsWindowVisible(GetHWndByControlID(inID))==TRUE);
}

//
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

//
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

	HDC	hdc = ::GetDC(mHWnd);
	if( hdc == NULL )   {_ACError("",NULL);return;}
	HFONT	hfont = CreateFont_(hdc,inFontName,inFontSize,kTextStyle_Normal,true);
	if( hfont == NULL )   {_ACError("",NULL);return;}
	::SendMessage(GetHWndByControlID(inID),WM_SETFONT,(LPARAM)hfont,(WPARAM)TRUE);
	::DeleteObject(hfont);
	::ReleaseDC(mHWnd,hdc);
}

void	CWindowImp::SetTextJustification( const AControlID inControlID, const AJustification inJustification )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inControlID) == true )
	{
		GetUserPane(inControlID).SetTextJustification(inJustification);
		return;
	}
	
	//通常コントロールの場合
	//処理無し
}

//
void	CWindowImp::SetCatchReturn( const AControlID inID, const ABool inCatchReturn )
{
	AIndex	index = mControlArray_ID.Find(inID);
	mControlArray_CatchReturn.Set(index,inCatchReturn);
}

//
ABool	CWindowImp::IsCatchReturn( const AControlID inID ) const
{
	AIndex	index = mControlArray_ID.Find(inID);
	return mControlArray_CatchReturn.Get(index);
}

//
void	CWindowImp::SetCatchTab( const AControlID inID, const ABool inCatchTab )
{
	AIndex	index = mControlArray_ID.Find(inID);
	mControlArray_CatchTab.Set(index,inCatchTab);
}

//
ABool	CWindowImp::IsCatchTab( const AControlID inID ) const
{
	AIndex	index = mControlArray_ID.Find(inID);
	return mControlArray_CatchTab.Get(index);
}

//
void	CWindowImp::SetAutoSelectAllBySwitchFocus( const AControlID inID, ABool inAutoSelectAll )
{
	AIndex	index = mControlArray_ID.Find(inID);
	mControlArray_AutoSelectAllBySwitchFocus.Set(index,inAutoSelectAll);
}

//
ABool	CWindowImp::IsAutoSelectAllBySwitchFocus( const AControlID inID ) const
{
	AIndex	index = mControlArray_ID.Find(inID);
	return mControlArray_AutoSelectAllBySwitchFocus.Get(index);
}

//
void	CWindowImp::SetDataType( const AControlID inControlID, const ADataType inDataType )
{
	AIndex	index = mControlArray_ID.Find(inControlID);
	if( index == kIndex_Invalid )   {_ACError("",this);return;}
	//
	mControlArray_DataType.Set(index,inDataType);
}

//
void	CWindowImp::SetControlPosition( const AControlID inID, const AWindowPoint& inPoint )
{
	//トップウインドウ座標系(AWindowPoint)から親ウインドウの座標系へ変換
	POINT	pt;
	pt.x = inPoint.x;
	pt.y = inPoint.y;
	::ClientToScreen(mHWnd,&pt);
	HWND	parent = ::GetParent(GetHWndByControlID(inID));
	::ScreenToClient(parent,&pt);
	//
	::SetWindowPos(GetHWndByControlID(inID),HWND_TOP,pt.x,pt.y,0,0,SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
}

//
void	CWindowImp::SetControlSize( const AControlID inID, const ANumber inWidth, const ANumber inHeight )
{
	::SetWindowPos(GetHWndByControlID(inID),HWND_TOP,0,0,inWidth,inHeight,SWP_NOZORDER | SWP_NOMOVE|SWP_NOACTIVATE);
}

//
void	CWindowImp::SetHelpTag( const AControlID inID, const AText& inText, const AHelpTagSide inTagSide )//#643
{
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetFixedTextTooltip(inText,inTagSide);//#643
	}
	else
	{
		_ACError("",this);
	}
	/*
	HWND	htooltip = ::CreateWindowEx(0,TOOLTIPS_CLASS,NULL,TTS_ALWAYSTIP|TTS_NOPREFIX,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
			GetHWndByControlID(inID),NULL,CAppImp::GetHInstance(),NULL);
	//不要？これを入れると、実行時に親ウインドウがアクティベートされるためコメントアウト。::SetWindowPos(htooltip, HWND_TOPMOST,0, 0, 0, 0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	RECT	rect;
	::GetClientRect(GetHWndByControlID(inID),&rect);
	AStCreateWcharStringFromAText	str(inText);
	TOOLINFOW	ti;
	ti.cbSize = sizeof(ti);// 0x501以上だとWindows2000で動作しないらしい？
	ti.hwnd		= GetHWndByControlID(inID);
	ti.rect		= rect;
	ti.hinst	= CAppImp::GetHInstance();
	ti.uFlags	= TTF_SUBCLASS;
	ti.uId		= inID;
	ti.lpszText	= (wchar_t*)str.GetPtr();//キャスト：TTM_ADDTOOLで使う場合は、lpszTextに値がセットされて帰ってくることは無いはず
	::SendMessageW(htooltip,TTM_ADDTOOLW,0,(LPARAM)&ti);
	*/
}

//R0246
void	CWindowImp::HideHelpTag() const
{
	//★暫定 未対応
}

//R0240
void	CWindowImp::EnableHelpTagCallback( const AControlID inID, const ABool inShowOnlyWhenNarrow )
{
	/*
	TTN_NEEDTEXTの方式では、コントロール内でマウス移動したときにツールチップ内容を更新できないので、
	Tracking Tooltip方式（TTF_TRACKフラグをOnにする）を使うことにする。
	HWND	htooltip = ::CreateWindowEx(0,TOOLTIPS_CLASS,NULL,TTS_ALWAYSTIP|TTS_NOPREFIX,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
			GetHWndByControlID(inID),NULL,CAppImp::GetHInstance(),NULL);
	RECT	rect;
	::GetClientRect(GetHWndByControlID(inID),&rect);
	TOOLINFOW	ti;
	ti.cbSize = sizeof(ti);// 0x501以上だとWindows2000で動作しないらしい？
	ti.hwnd		= GetHWndByControlID(inID);
	ti.rect		= rect;
	ti.hinst	= CAppImp::GetHInstance();
	ti.uFlags	= TTF_SUBCLASS|TTF_IDISHWND;//TTN_NEEDTEXTを使うためにはTTF_IDISHWND必須っぽい
	ti.uId		= (UINT_PTR) GetHWndByControlID(inID);
	ti.lpszText	= LPSTR_TEXTCALLBACKW;
	::SendMessageW(htooltip,TTM_ADDTOOLW,0,(LPARAM)&ti);
	*/
	//UserPane内で設定
	if( IsUserPane(inID) == false )   { _ACError("",this);return; }
	GetUserPane(inID).EnableTrackingToopTip(inShowOnlyWhenNarrow);//#507
}

void	CWindowImp::EmbedControl( const AControlID inParent, const AControlID inChild )
{
	AWindowPoint	pt;
	GetControlPosition(inChild,pt);
	::SetParent(GetHWndByControlID(inChild),GetHWndByControlID(inParent));
	SetControlPosition(inChild,pt);
	::SetWindowPos(GetHWndByControlID(inChild),HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void	CWindowImp::SetControlZOrder( const AControlID inID, const ABool inFront )
{
	if( inFront == true )
	{
		::SetWindowPos(GetHWndByControlID(inID),HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
	else
	{
		::SetWindowPos(GetHWndByControlID(inID),HWND_BOTTOM,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
}

void	CWindowImp::SetSelectionAll( const AControlID inID )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	AControlType	controlType = GetControlTypeByID(inID);
	HWND	hwnd = GetHWndByControlID(inID);
	switch(controlType)
	{
	  case kControlType_Edit:
		{
			::SendMessage(GetHWndByControlID(inID),EM_SETSEL,0,-1);
			break;
		}
	  default:
		{
			break;
		}
	}
}

//#236
/**
Drop可能なControlを登録する

DeleteControl()で登録解除される
*/
void	CWindowImp::RegisterDropEnableControl( const AControlID inControlID )
{
	//★暫定　未対応
}

//#291
/**
コントロールのZorderを設定
*/
void	CWindowImp::SetControlZOrderToTop( const AControlID inControlID )
{
	SetControlZOrder(inControlID,true);
}

//#291
void	CWindowImp::SetControlBindings( const AControlID inControlID,
		const ABool inLeft, const ABool inTop, const ABool inRight, const ABool inBottom )
{
	AIndex	bindingsIndex = mBindings_ControlID.Find(inControlID);
	if( bindingsIndex != kIndex_Invalid )   { _ACError("",this);return; }
	mBindings_ControlID.Add(inControlID);
	mBindings_Left.Add(inLeft);
	mBindings_Top.Add(inTop);
	mBindings_Right.Add(inRight);
	mBindings_Bottom.Add(inBottom);
}

//win
/**
*/
void	CWindowImp::DisableEventCatch( const AControlID inControlID )
{
	mEventCatchDisabledControlArray.Add(inControlID);
}

//#565
/**
コントロールをFileDrop可能に設定
*/
void	CWindowImp::EnableFileDrop( const AControlID inControlID )
{
	//処理無し（Winでは常にFileDrop可能）
}

#pragma mark ===========================

#pragma mark --- コントロール情報取得

//
void	CWindowImp::GetControlPosition( const AControlID inID, AWindowPoint& outPoint ) const
{
	HWND	hwnd = GetHWndByControlID(inID);
	RECT	rect;
	::GetWindowRect(hwnd,&rect);//スクリーン座標
	
	//本ウインドウ内の相対座標を取得
	POINT	pt;
	pt.x = rect.left;
	pt.y = rect.top;
	::ScreenToClient(mHWnd,&pt);
	
	outPoint.x = pt.x;
	outPoint.y = pt.y;
}

//
ANumber	CWindowImp::GetControlWidth( const AControlID inID ) const
{
	HWND	hwnd = GetHWndByControlID(inID);
	RECT	rect;
	::GetWindowRect(hwnd,&rect);
	return rect.right - rect.left;
}

//
ANumber	CWindowImp::GetControlHeight( const AControlID inID ) const
{
	HWND	hwnd = GetHWndByControlID(inID);
	RECT	rect;
	::GetWindowRect(hwnd,&rect);
	return rect.bottom - rect.top;
}

//
void	CWindowImp::GetWindowPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AWindowPoint& outWindowPoint ) const
{
	GetControlPosition(inID,outWindowPoint);
	outWindowPoint.x += inLocalPoint.x;
	outWindowPoint.y += inLocalPoint.y;
}

//
void	CWindowImp::GetWindowRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AWindowRect& outWindowRect ) const
{
	ALocalPoint	localpoint;
	AWindowPoint	windowpoint;
	//
	localpoint.x = inLocalRect.left;
	localpoint.y = inLocalRect.top;
	GetWindowPointFromControlLocalPoint(inID,localpoint,windowpoint);
	outWindowRect.left	= windowpoint.x;
	outWindowRect.top	= windowpoint.y;
	//
	localpoint.x = inLocalRect.right;
	localpoint.y = inLocalRect.bottom;
	GetWindowPointFromControlLocalPoint(inID,localpoint,windowpoint);
	outWindowRect.right	= windowpoint.x;
	outWindowRect.bottom= windowpoint.y;
}

//
void	CWindowImp::GetControlLocalPointFromWindowPoint( const AControlID inID, const AWindowPoint& inWindowPoint, ALocalPoint& outLocalPoint ) const
{
	AWindowPoint	controlpos;
	GetControlPosition(inID,controlpos);
	outLocalPoint.x = inWindowPoint.x - controlpos.x;
	outLocalPoint.y = inWindowPoint.y - controlpos.y;
}

//
void	CWindowImp::GetControlLocalRectFromWindowRect( const AControlID inID, const AWindowRect& inWindowRect, ALocalRect& outLocalRect ) const
{
	AWindowPoint	windowpoint;
	ALocalPoint	localpoint;
	//
	windowpoint.x = inWindowRect.left;
	windowpoint.y = inWindowRect.top;
	GetControlLocalPointFromWindowPoint(inID,windowpoint,localpoint);
	outLocalRect.left	= localpoint.x;
	outLocalRect.top	= localpoint.y;
	//
	windowpoint.x = inWindowRect.right;
	windowpoint.y = inWindowRect.bottom;
	GetControlLocalPointFromWindowPoint(inID,windowpoint,localpoint);
	outLocalRect.right	= localpoint.x;
	outLocalRect.bottom	= localpoint.y;
}

//
void	CWindowImp::GetGlobalPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AGlobalPoint& outGlobalPoint ) const
{
	HWND	hwnd = GetHWndByControlID(inID);
	if( hwnd == 0 )   { _ACError("",NULL); return; }
	
	POINT	pt;
	pt.x = inLocalPoint.x;
	pt.y = inLocalPoint.y;
	::ClientToScreen(hwnd,&pt);
	outGlobalPoint.x = pt.x;
	outGlobalPoint.y = pt.y;
}

//
void	CWindowImp::GetGlobalRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AGlobalRect& outGlobalRect ) const
{
	HWND	hwnd = GetHWndByControlID(inID);
	if( hwnd == 0 )   { _ACError("",NULL); return; }
	
	POINT	pt;
	pt.x= inLocalRect.left;
	pt.y = inLocalRect.top;
	::ClientToScreen(hwnd,&pt);
	outGlobalRect.left		= pt.x;
	outGlobalRect.top		= pt.y;
	
	pt.x = inLocalRect.right;
	pt.y = inLocalRect.bottom;
	::ClientToScreen(hwnd,&pt);
	outGlobalRect.right		= pt.x;
	outGlobalRect.bottom	= pt.y;
}

//
AControlID	CWindowImp::FindControl( const AGlobalPoint& inGlobalPoint ) const
{
	AControlID	controlID = kControlID_Invalid;
	for( AIndex i = 0; i < mUserPaneArray_ControlID.GetItemCount(); i++ )
	{
		RECT	rect;
		::GetWindowRect(GetHWndByControlID(mUserPaneArray_ControlID.Get(i)),&rect);//スクリーン座標
		if(	rect.top <= inGlobalPoint.y && rect.bottom >= inGlobalPoint.y &&
			rect.left <= inGlobalPoint.x && rect.right >= inGlobalPoint.x &&
			GetUserPaneConst(mUserPaneArray_ControlID.Get(i)).IsControlVisible() == true )
		{
			controlID = mUserPaneArray_ControlID.Get(i);
			//win
			if( mEventCatchDisabledControlArray.Find(controlID) != kIndex_Invalid )
			{
				continue;
			}
			//
			break;
		}
	}
	return controlID;
}

HWND	CWindowImp::GetHWndByControlID( const AControlID inID ) const
{
	AIndex	index = mControlArray_ID.Find(inID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return 0;}
	return mControlArray_HWnd.Get(index);
}

AControlType	CWindowImp::GetControlTypeByID( const AControlID inID ) const
{
	AIndex	index = mControlArray_ID.Find(inID);
	if( index == kIndex_Invalid )   return kControlType_Invalid;
	return mControlArray_ControlType.Get(index);
}

AControlID	CWindowImp::GetControlIDByHWnd( const HWND inHWnd ) const
{
	return ::GetWindowLong(inHWnd,GWL_ID);
}

AIndex	CWindowImp::GetControlEmbeddedTabIndex( const AControlID inID ) const
{
	AIndex	result = kIndex_Invalid;
	HWND	hwnd = GetHWndByControlID(inID);
	for( AIndex i = 0; i < mTabControlArray.GetItemCount(); i++ )
	{
		if( ::IsChild(mTabControlArray.Get(i),hwnd) == TRUE )
		{
			result = i;
			break;
		}
	}
	return result;
}

void	CWindowImp::GetComboListText( const AControlID inID, const AIndex inIndex, AText& outText ) const
{
	AItemCount	len = ::SendMessageW(GetHWndByControlID(inID),CB_GETLBTEXTLEN,inIndex,0)+1;
	outText.Reserve(0,len*sizeof(wchar_t));
	{
		AStTextPtr	textptr(outText,0,outText.GetItemCount());
		::SendMessageW(GetHWndByControlID(inID),CB_GETLBTEXT,inIndex,(LPARAM)(textptr.GetPtr()));
	}
	//ヌル文字以降を削除
	outText.DeleteAfterUTF16NullChar();
	outText.ConvertToUTF8FromAPIUnicode();
}

#pragma mark ===========================

#pragma mark --- コントロール値のSet/Getインターフェイス

//
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
	//
	outText.DeleteAll();
	int	len = ::GetWindowTextLengthW(GetHWndByControlID(inID))+1;
	outText.Reserve(0,len*sizeof(wchar_t));
	{
		AStTextPtr	textptr(outText,0,outText.GetItemCount());
		::GetWindowTextW(GetHWndByControlID(inID),(LPWSTR)(textptr.GetPtr()),textptr.GetByteCount()/sizeof(wchar_t));
	}
	//ヌル文字以降を削除
	outText.DeleteAfterUTF16NullChar();
	outText.ConvertToUTF8FromAPIUnicode();
	outText.ConvertReturnCodeToCR();
	//#375
	AIndex	valueIndependentMenuIndex = mValueIndependentMenuArray_ControlID.Find(inID);
	if( valueIndependentMenuIndex != kIndex_Invalid )
	{
		AIndex	index = mValueIndependentMenuArray_TitleArray.GetObjectConst(valueIndependentMenuIndex).Find(outText);
		if( index != kIndex_Invalid )
		{
			mValueIndependentMenuArray_ValueArray.GetObjectConst(valueIndependentMenuIndex).Get(index,outText);
		}
	}
}

//
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
	
	//現在の値と比較して同じなら何もしない
	AText	text;
	GetText(inID,text);
	if( inText.Compare(text) == true )   return;
	
	//
	//通常コントロールの場合
	switch(GetControlTypeByID(inID))
	{
	  case kControlType_ComboBox:
		{
			//#375
			AText	title;
			title.SetText(inText);
			AIndex	valueIndependentMenuIndex = mValueIndependentMenuArray_ControlID.Find(inID);
			if( valueIndependentMenuIndex != kIndex_Invalid )
			{
				AIndex	index = mValueIndependentMenuArray_ValueArray.GetObjectConst(valueIndependentMenuIndex).Find(inText);
				if( index != kIndex_Invalid )
				{
					//mValueIndependentMenuArray_TitleArray.GetObjectConst(valueIndependentMenuIndex).Get(index,title);
					::SendMessageW(GetHWndByControlID(inID),CB_SETCURSEL,(WPARAM)index,0);
					break;
				}
			}
			//
			AStCreateWcharStringFromAText	str(inText);
			::SetWindowTextW(GetHWndByControlID(inID),str.GetPtr());
			//DropListの場合、上記では設定できないので、リストのテキストと比較して、indexを設定する
			AItemCount	count = ::SendMessageW(GetHWndByControlID(inID),CB_GETCOUNT,0,0);
			ABool	selected = false;
			for( AIndex i = 0; i < count; i++ )
			{
				AText	text;
				GetComboListText(inID,i,text);
				if( inText.Compare(text) == true )
				{
					::SendMessageW(GetHWndByControlID(inID),CB_SETCURSEL,(WPARAM)i,0);
					selected = true;
					break;
				}
			}
			if( selected == false )
			{
				::SendMessageW(GetHWndByControlID(inID),CB_SETCURSEL,(WPARAM)(-1),0);
			}
			break;
		}
	  default:
		{
			AText	text;
			text.SetText(inText);
			AText	crlf;
			crlf.Add(13);
			crlf.Add(10);
			text.ReplaceChar('\r',crlf);
			AStCreateWcharStringFromAText	str(text);
			::SetWindowTextW(GetHWndByControlID(inID),str.GetPtr());
			break;
		}
	}
}

//
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
	switch(GetControlTypeByID(inID))
	{
	  case kControlType_Edit:
		{
			AStCreateWcharStringFromAText	str(inText);
			::SendMessage(GetHWndByControlID(inID),EM_REPLACESEL,0,(LPARAM)(str.GetPtr()));
			break;
		}
	  default:
		{
			_ACError("",NULL);
			break;
		}
	}
}

//
void	CWindowImp::GetBool( const AControlID inID, ABool& outBool ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	outBool = false;
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPaneConst(inID).GetBool(outBool);
		return;
	}
	
	switch(GetControlTypeByID(inID))
	{
	  case kControlType_Button:
		{
			outBool = ::SendMessage(GetHWndByControlID(inID),BM_GETCHECK,0,0);
			break;
		}
	  case kControlType_ComboBox:
		{
			if( ::SendMessage(GetHWndByControlID(inID),CB_GETCURSEL,0,0)== 0 )
			{
				outBool = false;
			}
			else
			{
				outBool = true;
			}
			break;
		}
	  default:
		{
			_ACError("",NULL);
			break;
		}
	}
}

//
ABool	CWindowImp::GetBool( const AControlID inID ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	ABool	result = false;
	GetBool(inID,result);
	return result;
}

//
void	CWindowImp::SetBool( const AControlID inID, const ABool inBool )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//現在の値と比較して同じなら何もしない
	if( GetControlTypeByID(inID) == kControlType_ComboBox )
	{
		int val = ::SendMessage(GetHWndByControlID(inID),CB_GETCURSEL,0,0);
		if( val == 0 && inBool == false )   return;
		if( val == 1 && inBool == true )   return;
	}
	else
	{
		if( GetBool(inID) == inBool )   return;
	}
	
	/*#349
	//
	if( inBool == true )
	{
		UpdateRadioGroup(inID);
	}
	*/
	switch(GetControlTypeByID(inID))
	{
	  case kControlType_Button:
		{
			::SendMessage(GetHWndByControlID(inID),BM_SETCHECK,inBool,0);
			break;
		}
		//ポップアップボタンの場合はfalseなら1番目、trueなら2番目の項目を選択する
	  case kControlType_ComboBox:
		{
			if( inBool == false )
			{
				::SendMessageW(GetHWndByControlID(inID),CB_SETCURSEL,(WPARAM)0,0);
			}
			else
			{
				::SendMessageW(GetHWndByControlID(inID),CB_SETCURSEL,(WPARAM)1,0);
			}
			break;
		}
	  default:
		{
			_ACError("",NULL);
			break;
		}
	}
}

//
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
	
	//通常コントロールの場合
	//処理無し
}

//
void	CWindowImp::SetColor( const AControlID inID, const AColor& inColor )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//現在の値と比較して同じなら何もしない
	AColor	color;
	GetColor(inID,color);
	if( AColorWrapper::CompareColor(color,inColor) == true )   return;
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetColor(inColor);
		return;
	}
	
	//通常コントロールの場合
	//処理無し
}

//
void	CWindowImp::GetNumber( const AControlID inID, ANumber& outNumber ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	AControlType	controlType = GetControlTypeByID(inID);
	HWND	hwnd = GetHWndByControlID(inID);
	switch(controlType)
	{
	  case kControlType_ScrollBar:
		{
			SCROLLINFO	si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS;
			::GetScrollInfo(hwnd,SB_CTL,&si);
			outNumber = si.nPos;
			break;
		}
	  case kControlType_Edit:
		{
			AText	text;
			GetText(inID,text);
			outNumber = text.GetNumber();
			break;
		}
	  case kControlType_ComboBox:
		{
			outNumber = static_cast<ANumber>(::SendMessage(hwnd,CB_GETCURSEL,0,0));
			break;
		}
	  case kControlType_Slider:
		{
			outNumber = static_cast<ANumber>(::SendMessageW(GetHWndByControlID(inID),TBM_GETPOS,0,0));
			break;
		}
	  case kControlType_ProgressBar:
		{
			outNumber = static_cast<ANumber>(::SendMessageW(GetHWndByControlID(inID),PBM_GETPOS,0,0));
			break;
		}
	  default:
		{
			_ACError("not implemented",NULL);
			break;
		}
	}
}

//
ANumber	CWindowImp::GetNumber( const AControlID inID ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return 0;}
	
	ANumber	num = 0;
	GetNumber(inID,num);
	return num;
}

//
void	CWindowImp::GetFloatNumber( const AControlID inID, AFloatNumber& outNumber ) const
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	AControlType	controlType = GetControlTypeByID(inID);
	HWND	hwnd = GetHWndByControlID(inID);
	switch(controlType)
	{
	  case kControlType_Edit:
	  case kControlType_ComboBox:
		{
			AText	text;
			GetText(inID,text);
			outNumber = text.GetFloatNumber();
			break;
		}
	  default:
		{
			_ACError("not implemented",NULL);
			break;
		}
	}
}

//
void	CWindowImp::SetNumber( const AControlID inID, const ANumber inNumber )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//現在の値と比較して同じなら何もしない
	if( GetNumber(inID) == inNumber )   return;
	
	AControlType	controlType = GetControlTypeByID(inID);
	HWND	hwnd = GetHWndByControlID(inID);
	switch(controlType)
	{
	  case kControlType_ScrollBar:
		{
			SCROLLINFO	si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS;
			::GetScrollInfo(hwnd,SB_CTL,&si);
			si.nPos = inNumber;
			::SetScrollInfo(hwnd,SB_CTL,&si,TRUE);
			break;
		}
	  case kControlType_Edit:
		{
			AText	text;
			text.SetNumber(inNumber);
			SetText(inID,text);
			break;
		}
	  case kControlType_ComboBox:
		{
			::SendMessageW(GetHWndByControlID(inID),CB_SETCURSEL,(WPARAM)inNumber,0);
			break;
		}
	  case kControlType_Slider:
		{
			::SendMessageW(GetHWndByControlID(inID),TBM_SETRANGE,0,(LPARAM)MAKELPARAM(0,100));
			::SendMessageW(GetHWndByControlID(inID),TBM_SETPOS,(WPARAM)TRUE,(LPARAM)inNumber);
			break;
		}
	  case kControlType_ProgressBar:
		{
			::SendMessageW(GetHWndByControlID(inID),PBM_SETRANGE,0,(LPARAM)MAKELPARAM(0,100));
			::SendMessageW(GetHWndByControlID(inID),PBM_SETPOS,(WPARAM)inNumber,0);
			break;
		}
	  default:
		{
			break;
		}
	}
}

//
void	CWindowImp::SetFloatNumber( const AControlID inID, const AFloatNumber inNumber )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//現在の値と比較して同じなら何もしない
	AFloatNumber	number;
	GetFloatNumber(inID,number);
	if( number == inNumber )   return;
	
	AControlType	controlType = GetControlTypeByID(inID);
	HWND	hwnd = GetHWndByControlID(inID);
	switch(controlType)
	{
	  case kControlType_Edit:
	  case kControlType_ComboBox:
		{
			AText	text;
			text.SetFormattedCstring("%g",inNumber);
			SetText(inID,text);
			break;
		}
	  default:
		{
			break;
		}
	}
}

//
void	CWindowImp::SetIcon( const AControlID inControlID, const AIconID inIconID, 
		const ABool inDefaultSize, const ANumber inLeftOffset, const ANumber inTopOffset )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inControlID) == true )
	{
		GetUserPane(inControlID).SetIcon(inIconID,inDefaultSize,inLeftOffset,inTopOffset);
		return;
	}
	
	//通常コントロールの場合
	switch(GetControlTypeByID(inControlID))
	{
	  case kControlType_Button:
		{
			::SendMessageW(GetHWndByControlID(inControlID),BM_SETIMAGE,(WPARAM)IMAGE_ICON,(LPARAM)GetHIconByIconID(inIconID));
			break;
		}
	  default:
		{
			_ACError("",NULL);
			break;
		}
	}
}

//
void	CWindowImp::SetMinMax( const AControlID inControlID, const ANumber inMin, const ANumber inMax )
{
	AControlType	controlType = GetControlTypeByID(inControlID);
	HWND	hwnd = GetHWndByControlID(inControlID);
	switch(controlType)
	{
	  case kControlType_ScrollBar:
		{
			::SetScrollRange(hwnd,SB_CTL,inMin,inMax,TRUE);
			break;
		}
	  default:
		{
			_ACError("not implemented",NULL);
			break;
		}
	}
}

void	CWindowImp::SetScrollBarPageSize( const AControlID inControlID, const ANumber inPageSize )
{
	SCROLLINFO	si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE;
	si.nPage = inPageSize;
	::SetScrollInfo(GetHWndByControlID(inControlID),SB_CTL,&si,TRUE);
}

#pragma mark ===========================

#pragma mark --- コントロール内選択インターフェイス

//
void	CWindowImp::CreateBevelButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, 
		const AText& inTitle, const ABool inEnableMenu )
{
	DWORD	winstyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;
	HWND	hwnd = ::CreateWindowW(L"button",NULL,winstyle,
			inPoint.x,inPoint.y,inWidth,inHeight,
			mHWnd,
			(HMENU)inID,//子ウインドウID
			CAppImp::GetHInstance(),NULL);
	::SetWindowPos(hwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	RegisterControl(hwnd);
}

/* AView_Buttonを作ったため不要 
void	CWindowImp::CreateCustomButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, 
		const AText& inTitle, const ABool inEnableMenu, const AControlID inParentID, const ABool inToggle )
{
	CCustomButtonFactory	factory(this,*this);
	AIndex	index = mUserPaneArray.AddNewObject(factory);
	mUserPaneArray_ControlID.Add(inID);
	mUserPaneArray_Type.Add(kUserPaneType_PaneForAView);
	
	HWND	hwnd = mHWnd;
	if( inParentID != kControlID_Invalid )
	{
		hwnd = GetHWndByControlID(inParentID);
	}
	APoint	pt;
	pt.x = inPoint.x;
	pt.y = inPoint.y;
	mUserPaneArray.GetObject(index).CreateUserPane(hwnd,inID,pt,inWidth,inHeight,true);
	RegisterControl(mUserPaneArray.GetObject(index).GetHWnd());
	SetControlPosition(inID,inPoint);//AWindowPointはトップウインドウ座標系なので変換
	
	if( inToggle == true )
	{
		GetUserPane(inID).SetToggleEnable(inToggle);
	}
}
*/

//
void	CWindowImp::CreateScrollBar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex )
{
	DWORD	winstyle = WS_CHILD | WS_VISIBLE;
	if( inWidth > inHeight )
	{
		winstyle |= SBS_HORZ;
	}
	else
	{
		winstyle |= SBS_VERT;
	}
	HWND	parent = mHWnd;
	if( inTabIndex != kIndex_Invalid )
	{
		parent = mTabControlArray.Get(inTabIndex);
	}
	HWND	hwnd = ::CreateWindowW(L"scrollbar",NULL,winstyle,
			inPoint.x,inPoint.y,inWidth,inHeight,
			parent,
			(HMENU)inID,//子ウインドウID
			CAppImp::GetHInstance(),NULL);
	::SetWindowPos(hwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	RegisterControl(hwnd);
	SetControlPosition(inID,inPoint);//AWindowPointはトップウインドウ座標系なので変換
}

//
void	CWindowImp::RegisterScrollBar( const AControlID inID )
{
	//処理無し
}

/**
サイズボックス生成
*/
void	CWindowImp::CreateSizeBox( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	HWND	hwnd = ::CreateWindowW(L"scrollbar",NULL,WS_CHILD | WS_VISIBLE | SBS_SIZEBOX,
			inPoint.x,inPoint.y,inWidth,inHeight,
			mHWnd,
			(HMENU)inID,//子ウインドウID
			CAppImp::GetHInstance(),NULL);
	RegisterControl(hwnd);
	//スクロールバー全てのカーソルが変わってしまうので右記はNG::SetClassLong(hwnd,GCL_HCURSOR,(LONG)::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENWSE)));
}

//
void	CWindowImp::CreateStatusBar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	HWND	hwnd = ::CreateWindowExW(0,STATUSCLASSNAME,NULL,
			WS_CHILD | SBARS_SIZEGRIP | WS_VISIBLE,
			inPoint.x,inPoint.y,inWidth,inHeight,
			mHWnd,(HMENU)inID,CAppImp::GetHInstance(),NULL);
	RegisterControl(hwnd);
	::SendMessage(hwnd,SB_SIMPLE,TRUE,0L);
	//★暫定 デバッグ途中（サイズボックス表示されない）
}

//
void	CWindowImp::CreateToolbar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	HWND	htoolbar = ::CreateWindowExW(0,TOOLBARCLASSNAME,NULL,
		WS_CHILD | TBSTYLE_WRAPABLE,inPoint.x,inPoint.y,0,0,mHWnd,NULL,CAppImp::GetHInstance(),NULL);
	//HWND	htoolbar = ::CreateToolbarEx(mHWnd,WS_CHILD | WS_VISIBLE,inID,2,CAppImp::GetHInstance(),
	//		IDB_BITMAP1,tbb,2,0,0,16,15,sizeof(TBBUTTON));
	
	DWORD	err = GetLastError();
	
	// Create the imagelist.
	HIMAGELIST hImageList = ImageList_Create(
			24, 24,   // Dimensions of individual bitmaps.
			ILC_COLOR32 | ILC_MASK,   // Ensures transparent background.
			2, 0);
	
	// Set the image list.
	SendMessage(htoolbar, TB_SETIMAGELIST, (WPARAM)0, 
			(LPARAM)hImageList);
	
	// Load the button images.
	SendMessage(htoolbar, TB_LOADIMAGES, (WPARAM)IDB_VIEW_LARGE_COLOR , 
			(LPARAM)HINST_COMMCTRL);
	
	UINT	uToolStyle = (UINT)GetWindowLong(htoolbar, GWL_STYLE);
	uToolStyle |= TBSTYLE_FLAT;
	SetWindowLong(htoolbar, GWL_STYLE, (LONG)uToolStyle); 
	
	TBBUTTON 	tbb[] = {
				{MAKELONG(STD_FILENEW,0), IDM_Open,TBSTATE_ENABLED,BTNS_AUTOSIZE,{0},0,0},
				{MAKELONG(VIEW_PARENTFOLDER,0),IDM_Open,TBSTATE_ENABLED,BTNS_AUTOSIZE,{0},0,0}};
	::SendMessage(htoolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	::SendMessage(htoolbar, TB_ADDBUTTONS, (WPARAM)2, (LPARAM)&tbb);
	
	TBBUTTON tbSPACE = {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0L};
	//SendMessage(htoolbar, TB_INSERTBUTTON, 0, (LPARAM)&tbSPACE);
	
	//SendMessage(htoolbar,TB_SETPADDING,0,(LPARAM)MAKELPARAM(32,0));
	
	TBMETRICS	tbm;
	tbm.cbSize = sizeof(tbm);
	tbm.dwMask = TBMF_PAD;
	tbm.cxPad = 32;
	tbm.cyPad = 0;
	::SendMessage(htoolbar,TB_SETMETRICS,0,(LPARAM)&tbm);
	
	::SendMessage(htoolbar, TB_AUTOSIZE, 0, 0); 
	::ShowWindow(htoolbar, TRUE);

}

//★暫定 テスト実装
void	CWindowImp::CreateRebar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	HWND	hrebar = ::CreateWindowExW(0,REBARCLASSNAME,NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | RBS_BANDBORDERS | WS_CLIPCHILDREN | CCS_NODIVIDER ,
			inPoint.x,inPoint.y,inWidth,inHeight,mHWnd,(HMENU)inID,CAppImp::GetHInstance(),NULL);
	//HWND	htoolbar = ::CreateToolbarEx(mHWnd,WS_CHILD | WS_VISIBLE,inID,2,CAppImp::GetHInstance(),
	//		IDB_BITMAP1,tbb,2,0,0,16,15,sizeof(TBBUTTON));
	RegisterControl(hrebar);
	
	REBARINFO	rbi;
	rbi.cbSize = sizeof(rbi);
	rbi.fMask = 0;
	::SendMessage(hrebar,RB_SETBARINFO,0,(LPARAM)&rbi); 
	
	/*CRebarContentFactory	factory(this,*this);
	AIndex	index = mUserPaneArray.AddNewObject(factory);
	mUserPaneArray_ControlID.Add(5900);
	mUserPaneArray_Type.Add(kUserPaneType_RebarContent);
	APoint	pt = {0,0};
	mUserPaneArray.GetObject(index).CreateUserPane(hrebar,5900,pt,64,24,true);
	HWND	hc = mUserPaneArray.GetObject(index).GetHWnd();
	RegisterControl(hc);
	
	HWND hb = ::CreateWindowW(L"button",NULL,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
			0,0,24,24,
			mUserPaneArray.GetObject(index).GetHWnd(),
			(HMENU)5999,//子ウインドウID
			CAppImp::GetHInstance(),NULL);
	RegisterControl(hb);
	::CreateWindowW(L"button",NULL,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			28,2,24,24,
			mUserPaneArray.GetObject(index).GetHWnd(),
			(HMENU)5998,//子ウインドウID
			CAppImp::GetHInstance(),NULL);
	RegisterControl(hb);
	
	REBARBANDINFO rbbi;
	
	HWND	hbutton = CreateWindow(L"BUTTON", L"test", 
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 24, 24, 
			hrebar, (HMENU)0, CAppImp::GetHInstance() ,NULL);
	rbbi.cbSize = sizeof(rbbi);
	rbbi.fMask = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE;
	rbbi.fStyle = RBBS_CHILDEDGE;
	rbbi.cxMinChild = 0;
	rbbi.cyMinChild = 25; 
	rbbi.lpText = L"test";
	rbbi.hwndChild =hbutton;
	rbbi.cx = 80;
	::SendMessage(hrebar,RB_INSERTBAND,(WPARAM)-1,(LPARAM)&rbbi);

	
	rbbi.cbSize = sizeof(rbbi);
	rbbi.fMask = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE;
	rbbi.fStyle = RBBS_CHILDEDGE;
	rbbi.cxMinChild = 0;
	rbbi.cyMinChild = 25; 
	rbbi.lpText = L"test";
	rbbi.hwndChild = mUserPaneArray.GetObject(index).GetHWnd();
	rbbi.cx = 80;
	::SendMessage(hrebar,RB_INSERTBAND,(WPARAM)-1,(LPARAM)&rbbi);
	//::SendMessage(hrebar,RB_INSERTBAND,(WPARAM)-1,(LPARAM)&rbbi); 
	*/
	HWND	htoolbar = ::CreateWindowExW(0,TOOLBARCLASSNAME,NULL,
		WS_CHILD | TBSTYLE_WRAPABLE,inPoint.x,inPoint.y,0,0,hrebar,NULL,CAppImp::GetHInstance(),NULL);
	//HWND	htoolbar = ::CreateToolbarEx(mHWnd,WS_CHILD | WS_VISIBLE,inID,2,CAppImp::GetHInstance(),
	//		IDB_BITMAP1,tbb,2,0,0,16,15,sizeof(TBBUTTON));
	
	DWORD	err = GetLastError();
	
	// Create the imagelist.
	HIMAGELIST hImageList = ImageList_Create(
			24, 24,   // Dimensions of individual bitmaps.
			ILC_COLOR32 | ILC_MASK,   // Ensures transparent background.
			2, 0);
	
	// Set the image list.
	SendMessage(htoolbar, TB_SETIMAGELIST, (WPARAM)0, 
			(LPARAM)hImageList);
	
	// Load the button images.
	SendMessage(htoolbar, TB_LOADIMAGES, (WPARAM)IDB_VIEW_LARGE_COLOR , 
			(LPARAM)HINST_COMMCTRL);
	
	UINT	uToolStyle = (UINT)GetWindowLong(htoolbar, GWL_STYLE);
	uToolStyle |= TBSTYLE_FLAT;
	SetWindowLong(htoolbar, GWL_STYLE, (LONG)uToolStyle); 
	
	TBBUTTON 	tbb[] = {
				{MAKELONG(STD_FILENEW,0), IDM_Open,TBSTATE_ENABLED,BTNS_AUTOSIZE,{0},0,0},
				{MAKELONG(VIEW_PARENTFOLDER,0),IDM_Open,TBSTATE_ENABLED,BTNS_AUTOSIZE,{0},0,0}};
	::SendMessage(htoolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	::SendMessage(htoolbar, TB_ADDBUTTONS, (WPARAM)2, (LPARAM)&tbb);
	
	TBBUTTON tbSPACE = {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0L};
	//SendMessage(htoolbar, TB_INSERTBUTTON, 0, (LPARAM)&tbSPACE);
	
	//SendMessage(htoolbar,TB_SETPADDING,0,(LPARAM)MAKELPARAM(32,0));
	
	TBMETRICS	tbm;
	tbm.cbSize = sizeof(tbm);
	tbm.dwMask = TBMF_PAD;
	tbm.cxPad = 32;
	tbm.cyPad = 0;
	::SendMessage(htoolbar,TB_SETMETRICS,0,(LPARAM)&tbm);
	
	::SendMessage(htoolbar, TB_AUTOSIZE, 0, 0); 
	
	
	REBARBANDINFO rbbi;
	
	//::ShowWindow(htoolbar, TRUE);
	
	HWND	hbutton = CreateWindow(L"BUTTON", L"test", 
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 24, 24, 
			hrebar, (HMENU)0, CAppImp::GetHInstance() ,NULL);
	//REBARBANDINFO rbbi;
	rbbi.cbSize = sizeof(rbbi);
	rbbi.fMask = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE;
	rbbi.fStyle = RBBS_CHILDEDGE;
	rbbi.cxMinChild = 0;
	rbbi.cyMinChild = 25; 
	rbbi.lpText = L"test";
	rbbi.hwndChild =hbutton;
	rbbi.cx = 80;
	::SendMessage(hrebar,RB_INSERTBAND,(WPARAM)-1,(LPARAM)&rbbi);
	::SendMessage(hrebar,RB_INSERTBAND,(WPARAM)-1,(LPARAM)&rbbi);
	
	rbbi.cbSize = sizeof(rbbi);
	rbbi.fMask = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE;
	rbbi.fStyle = RBBS_CHILDEDGE;
	rbbi.cxMinChild = 0;
	rbbi.cyMinChild = 25; 
	rbbi.lpText = L"test";
	rbbi.hwndChild = htoolbar;
	rbbi.cx = 80;
	::SendMessage(hrebar,RB_INSERTBAND,(WPARAM)-1,(LPARAM)&rbbi);

	::ShowWindow(hrebar, TRUE);
}

//
void	CWindowImp::DeleteControl( const AControlID inID )
{
	//#212
	if( mCurrentFocus == inID )
	{
		ClearFocus();
	}
	
	//★暫定 対応必要? textarraymenu
	
	/*#135
	//FocusGroupから削除
	AIndex	focusgroupindex = mFocusGroup.Find(inID);
	if( focusgroupindex != kIndex_Invalid )
	{
		mFocusGroup.Delete1(focusgroupindex);
	}
	*/
	//
	AIndex	userpaneindex = mUserPaneArray_ControlID.Find(inID);
	if( userpaneindex != kIndex_Invalid )
	{
		//UserPaneの場合（CUserPaneでDestroyWindow()等実行）
		//DestroyUserPane()はWM_DESTROYから実行することにするGetUserPane(inID).DestroyUserPane();
		::DestroyWindow(GetHWndByControlID(inID));
		//
		mUserPaneArray.Delete1Object(userpaneindex);
		mUserPaneArray_ControlID.Delete1(userpaneindex);
		mUserPaneArray_Type.Delete1(userpaneindex);
	}
	else
	{
		//通常コントロールの場合
		::DestroyWindow(GetHWndByControlID(inID));
	}
	//コントロール登録削除
	UnregisterControl(inID);
	//Bindings設定削除
	AIndex	bindingsIndex = mBindings_ControlID.Find(inID);
	if( bindingsIndex != kIndex_Invalid )
	{
		mBindings_ControlID.Delete1(bindingsIndex);
		mBindings_Left.Delete1(bindingsIndex);
		mBindings_Top.Delete1(bindingsIndex);
		mBindings_Right.Delete1(bindingsIndex);
		mBindings_Bottom.Delete1(bindingsIndex);
	}
}

#pragma mark ===========================

#pragma mark --- フォーカス管理

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
	//ウインドウをアクティブにしないときはFocusも設定しない。（::SetFocus()はウインドウがアクティブになる。）
	if( mNoActivate == true )   return;
	
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}

	//char	str[256];
	//sprintf(str,"SetFocus()S:(current:%d new:%d) \n",mCurrentFocus,inControlID);
	//OutputDebugStringA(str);
	
	//
	if( inControlID == mCurrentFocus )   return;
	
	//
	//#135 if( mFocusGroup.Find(inControlID) == kControlID_Invalid )   return;
	
	//旧フォーカスを記憶する
	//（ここでEVT_ValueChanged()実行はしない。EVT_ValueChanged()からSetFocus()をコールされる可能性があるので、無限再帰呼び出しになる。
	//EVT_ValueChanged()をコールするのは、mCurrentFocusを変更後。これなら、上の条件ではじかれて実行されない。
	AControlID	oldFocus = GetCurrentFocus();
	
	//現在のフォーカスをクリアする
	ClearFocus();
	
	//
	HWND hwnd = ::SetFocus(GetHWndByControlID(inControlID));
	//BS_NOTIFYを設定する（これがないとWin7の場合にフォーカス破線が表示されない→間違い？
	//HWND	hwnd = GetHWndByControlID(inControlID);
	//::SetWindowLong(hwnd,GWL_STYLE,
	//		::GetWindowLong(hwnd,GWL_STYLE)|BS_NOTIFY);
	/*
	if( hwnd == NULL )
	{
		return;
	}
	*/
	
	//mCurrentFocusを設定
	mCurrentFocus = inControlID;
	mLatentFocus = inControlID;//#0 kControlID_Invalid;
	
	//現在のフォーカス位置と違うコントロールをfocusした場合は、フォーカスコントロールへValueChangedを通知
	if( oldFocus != kControlID_Invalid && oldFocus != inControlID )
	{
		GetAWin().EVT_ValueChanged(oldFocus);
	}
	
	//UserPaneの場合は対応メソッドをコール
	if( IsUserPane(inControlID) == true )
	{
		//UserPaneのSetFocus()実行
		GetUserPane(inControlID).SetFocus();
	}
	//OutputDebugStringA("SetFocus()E  \n");
}

/**
フォーカスをクリア
*/
void	CWindowImp::ClearFocus( const ABool inAlsoClearLatentFocus )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}

	//char	str[256];
	//sprintf(str,"ClearFocus()S:%d ",mCurrentFocus);
	//OutputDebugStringA(str);
	//UserPaneの場合は対応メソッドをコール
	if( mCurrentFocus != kControlID_Invalid )
	{
		if( IsUserPane(mCurrentFocus) == true )
		{
			GetUserPane(mCurrentFocus).ResetFocus();
		}
	}
	//少なくともOverlay時はここで設定するとメインウインドウのフォーカスが外れる
	//::SetFocus(mHWnd);
	//mCurrentFocusをクリア
	mCurrentFocus = kControlID_Invalid;
	//inAlsoClearLatentFocusがtrueならlatentもクリア（デフォルトはlatentもクリア。ただし、deactivatedからコールされる場合はlatentは残す） #212
	if( inAlsoClearLatentFocus == true )
	{
		mLatentFocus = kControlID_Invalid;
	}
	//OutputDebugStringA("ClearFocus()E  \n");
}

/*#135
//
void	CWindowImp::SwitchFocusTo( const AControlID inControlID )
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
				focusGroupIndex++;
				if( focusGroupIndex >= mFocusGroup.GetItemCount() )
				{
					focusGroupIndex = 0;
				}
			}
		}
	}
	//EnableかつVisibleなコントロールがない場合はフォーカスをクリア
	ClearFocus();
}

//
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
*/
//フォーカスのコントロールがDisableされた場合、または、フォーカス無しの状態からフォーカス可能なコントロールがEnableされた場合の処理
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

AControlID	CWindowImp::GetCurrentFocus() const
{
	if( IsActive() == true )
	{
		return mCurrentFocus;
	}
	else
	{
		return mLatentFocus;
	}
}

#pragma mark ===========================

#pragma mark --- タブView管理

//
void	CWindowImp::UpdateTab()
{
	for( AIndex i = 0; i < mTabControlArray.GetItemCount(); i++ )
	{
		if( i == mCurrentTabIndex )
		{
			::ShowWindow(mTabControlArray.Get(i),SW_SHOW);
		}
		else
		{
			::ShowWindow(mTabControlArray.Get(i),SW_HIDE);
		}
	}
	RefreshWindow();//#427
	::UpdateWindow(mHWnd);
}

/**
タブControl選択
*/
void	CWindowImp::SetTabControlValue( const AIndex inTabIndex )
{
	if( mTabControlHWnd == NULL )   {_ACError("",this);return;}
	TabCtrl_SetCurSel(mTabControlHWnd,inTabIndex);
}

#pragma mark ===========================

#pragma mark --- UserPane管理

/*#688
//
void	CWindowImp::RegisterTextEditPane( const AControlID inID, const ABool inWrapMode, 
		const ABool inVScrollbar, const ABool inHScrollBar, const ABool inHasFrame )
{
	//キーボードフォーカスグループへ登録
	//#135 mFocusGroup.Add(inID);
	
	//処理なし
}
*/

//
void	CWindowImp::ResetUndoStack( const AControlID inID )
{
	switch(GetControlTypeByID(inID))
	{
	  case kControlType_Edit:
		{
			::SendMessageW(GetHWndByControlID(inID),EM_EMPTYUNDOBUFFER,0,0);
			break;
		}
	}
}

//
void	CWindowImp::RegisterColorPickerPane( const AControlID inID )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	HWND	dummycontrol = GetHWndByControlID(inID);
	RECT	bounds;
	::GetWindowRect(dummycontrol,&bounds);
	HWND	parent = ::GetParent(dummycontrol);
	RECT	parentbounds;
	::GetWindowRect(parent,&parentbounds);
	//ダミーコントロール削除
	::DestroyWindow(dummycontrol);
	UnregisterControl(inID);
	
	CColorPickerPaneFactory	factory(this,*this);
	AIndex	index = mUserPaneArray.AddNewObject(factory);
	mUserPaneArray_ControlID.Add(inID);
	mUserPaneArray_Type.Add(kUserPaneType_ColorPickerPane);
	
	APoint	pt;
	pt.x = bounds.left - parentbounds.left;
	pt.y = bounds.top - parentbounds.top;
	ANumber	width = bounds.right - bounds.left;
	ANumber height = bounds.bottom - bounds.top;
	mUserPaneArray.GetObject(index).CreateUserPane(parent,inID,pt,width,height,true);
	//コントロール登録
	RegisterControl(mUserPaneArray.GetObject(index).GetHWnd());
	
}

//
void	CWindowImp::RegisterPaneForAView( const AControlID inID/*#135, const ABool inSupportFocus*/ )
{
	//ウインドウ生成済みチェック
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	HWND	dummycontrol = GetHWndByControlID(inID);
	RECT	bounds;
	::GetWindowRect(dummycontrol,&bounds);
	HWND	parent = ::GetParent(dummycontrol);
	RECT	parentbounds;
	//::GetWindowRect(parent,&parentbounds);GetWindowRect()だとparentがWindowのときに、タイトルバーを含んだ領域になるので、GetWindowInfo()へ変更
	WINDOWINFO	parentwininfo;
	::GetWindowInfo(parent,&parentwininfo);
	parentbounds = parentwininfo.rcClient;
	//ダミーコントロール削除
	::DestroyWindow(dummycontrol);
	UnregisterControl(inID);
	
	CPaneForAViewFactory	factory(this,*this);
	AIndex	index = mUserPaneArray.AddNewObject(factory);
	mUserPaneArray_ControlID.Add(inID);
	mUserPaneArray_Type.Add(kUserPaneType_PaneForAView);
	
	APoint	pt;
	pt.x = bounds.left - parentbounds.left;
	pt.y = bounds.top - parentbounds.top;
	ANumber	width = bounds.right - bounds.left;
	ANumber height = bounds.bottom - bounds.top;
	mUserPaneArray.GetObject(index).CreateUserPane(parent,inID,pt,width,height,true);
	//コントロール登録
	RegisterControl(mUserPaneArray.GetObject(index).GetHWnd());
	/*#135
	//
	if( inSupportFocus == true )
	{
		RegisterFocusGroup(inID);
	}
	*/
	//Offscreen設定（WM_PAINT前にGetDrawTextWidth()等コールできるように）。このタイミングでは描画はせず、WM_PAINTで描画される
	if( width > 0 && height > 0 )
	{
		GetUserPane(inID).PrepareOffscreen();
	}
}

//AView用UserPaneを生成
void	CWindowImp::CreatePaneForAView( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, 
		/*#360 const ABool inSupportFocus,*/ const AIndex inTabIndex, const ABool inFront )
{
	CPaneForAViewFactory	factory(this,*this);
	AIndex	index = mUserPaneArray.AddNewObject(factory);
	mUserPaneArray_ControlID.Add(inID);
	mUserPaneArray_Type.Add(kUserPaneType_PaneForAView);
	
	HWND	hwnd = mHWnd;
	if( inTabIndex != kIndex_Invalid )
	{
		hwnd = mTabControlArray.Get(inTabIndex);
	}
	APoint	pt;
	pt.x = inPoint.x;
	pt.y = inPoint.y;
	mUserPaneArray.GetObject(index).CreateUserPane(hwnd,inID,pt,inWidth,inHeight,inFront);
	RegisterControl(mUserPaneArray.GetObject(index).GetHWnd());
	SetControlPosition(inID,inPoint);//AWindowPointはトップウインドウ座標系なので変換
	
	/*#135
	//
	if( inSupportFocus == true )
	{
		RegisterFocusGroup(inID);
	}
	*/
	
	//#291 UserPaneにSupportFocusフラグを設定
	//#360 GetUserPane(inID).SetSupportFocus(inSupportFocus);
	//Offscreen設定（WM_PAINT前にGetDrawTextWidth()等コールできるように）。このタイミングでは描画はせず、WM_PAINTで描画される
	if( inWidth > 0 && inHeight > 0 )
	{
		GetUserPane(inID).PrepareOffscreen();
	}
}

/**
UserPaneの値が変更されたときのCUserPaneからのコールバック
*/
void	CWindowImp::CB_UserPaneValueChanged( const AControlID inID )
{
	GetAWin().EVT_ValueChanged(inID);
}

/**
UserPaneでText入力されたときのCUserPaneからのコールバック
*/
void	CWindowImp::CB_UserPaneTextInputted( const AControlID inID )
{
	GetAWin().EVT_TextInputted(inID);
}

//
CUserPane&	CWindowImp::GetUserPane( const AControlID inControlID )
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )   _ACThrow("no UserPane",this);
	return mUserPaneArray.GetObject(index);
}

//
const CUserPane&	CWindowImp::GetUserPaneConst( const AControlID inControlID ) const
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )   _ACThrow("no UserPane",this);
	return mUserPaneArray.GetObjectConst(index);
}

//
AUserPaneType	CWindowImp::GetUserPaneType( const AControlID inControlID ) const
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )   return kUserPaneType_Invalid;
	return mUserPaneArray_Type.Get(index);
}

//指定ControlIDのコントロールがUserPaneかどうかを判定
ABool	CWindowImp::IsUserPane( const AControlID inControlID ) const
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	return (index != kIndex_Invalid);
}

#pragma mark ===========================

#pragma mark ---OSControlのテーブルView管理
//Windowsの場合は未対応

//TableView登録
void	CWindowImp::RegisterTableView( const AControlID inControlID, const AControlID inUp, const AControlID inDown,
		const AControlID inTop, const AControlID inBottom )
{
	_ACError("not implemented",this);
}

//列を登録
void	CWindowImp::RegisterTableViewColumn( const AControlID inControlID, const APrefID inColumnID, const ANumber inDataType )
{
	_ACError("not implemented",this);
}

//テーブルデータ全設定

//設定開始時に呼ぶ
void	CWindowImp::BeginSetTable( const AControlID inControlID )
{
	_ACError("not implemented",this);
}

//列データを設定(Text)
void	CWindowImp::SetTable_Text( const AControlID inControlID, const APrefID inColumnID, const ATextArray& inTextArray )
{
	_ACError("not implemented",this);
}

//列データを設定(Bool)
void	CWindowImp::SetTable_Bool( const AControlID inControlID, const APrefID inColumnID, const ABoolArray& inBoolArray )
{
	_ACError("not implemented",this);
}

//列データを設定(Number)
void	CWindowImp::SetTable_Number( const AControlID inControlID, const APrefID inColumnID, const ANumberArray& inNumberArray )
{
	_ACError("not implemented",this);
}

//列データを設定(Color)
void	CWindowImp::SetTable_Color( const AControlID inControlID, const APrefID inColumnID, const AColorArray& inColorArray )
{
	_ACError("not implemented",this);
}

//全列設定完了時に呼ぶ
void	CWindowImp::EndSetTable( const AControlID inControlID )
{
	_ACError("not implemented",this);
}

//行追加

//行追加開始時に呼ぶ
void	CWindowImp::BeginInsertRows( const AControlID inControlID )
{
	_ACError("not implemented",this);
}

//行（1セル）追加(Text)
void	CWindowImp::InsertRow_Text( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, const AText& inText )
{
	_ACError("not implemented",this);
}
void	CWindowImp::InsertRow_Text_SwapContent( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, AText& ioText )
{
	_ACError("not implemented",this);
}

//行（1セル）追加(Number)
void	CWindowImp::InsertRow_Number( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, const ANumber inNumber )
{
	_ACError("not implemented",this);
}

//行追加完了時に呼ぶ
void	CWindowImp::EndInsertRows( const AControlID inControlID, const AIndex inRowIndex, const AIndex inRowCount )
{
	_ACError("not implemented",this);
}

void	CWindowImp::UpdateTableView( const AControlID inControlID )
{
	_ACError("not implemented",this);
}


void	CWindowImp::EnterColumnEditMode( const AControlID inControlID, const ADataID inColumnID )
{
	_ACError("not implemented",this);
}

void	CWindowImp::SetColumnIcon( const AControlID inControlID, const ADataID inColumnID, const ANumberArray& inIconIDArray )
{
	_ACError("not implemented",this);
}

const ATextArray&	CWindowImp::GetColumn_TextArray( const AControlID inControlID, const APrefID inColumnID ) const
{
	_ACError("not implemented",this);
	return GetEmptyTextArray();
}

const ABoolArray&	CWindowImp::GetColumn_BoolArray( const AControlID inControlID, const APrefID inColumnID ) const
{
	_ACError("not implemented",this);
	return GetEmptyBoolArray();
}

const ANumberArray&	CWindowImp::GetColumn_NumberArray( const AControlID inControlID, const APrefID inColumnID ) const
{
	_ACError("not implemented",this);
	return GetEmptyNumberArray();
}

const AColorArray&	CWindowImp::GetColumn_ColorArray( const AControlID inControlID, const ADataID inColumnID ) const
{
	_ACError("not implemented",this);
	return GetEmptyColorArray();
}

void	CWindowImp::SetColumnWidth( const AControlID inControlID, const APrefID inColumnID, const ANumber inWidth )
{
	_ACError("not implemented",this);
}

ANumber	CWindowImp::GetColumnWidth( const AControlID inControlID, const APrefID inColumnID ) const
{
	_ACError("not implemented",this);
	return 0;
}

void	CWindowImp::SetTableFont( const AControlID inControlID, const AText& inFontName, const AFloatNumber inFontSize )
{
	_ACError("not implemented",this);
}

void	CWindowImp::SetInhibit0LengthForTable( const AControlID inControlID, const ABool inInhibit0Length )
{
	_ACError("not implemented",this);
}

void	CWindowImp::SetTableScrollPosition( const AControlID inControlID, const APoint& inPoint )
{
	_ACError("not implemented",this);
}

void	CWindowImp::GetTableScrollPosition( const AControlID inControlID, APoint& outPoint ) const
{
	_ACError("not implemented",this);
}

void	CWindowImp::SetTableEditable( const AControlID inControlID, const ABool inEditable )
{
	_ACError("not implemented",this);
}

#pragma mark ===========================

#pragma mark --- ポップアップメニューボタン管理

//
void	CWindowImp::SetMenuItemsFromTextArray( const AControlID inID, const ATextArray& inTextArray )
{
	if( GetControlTypeByID(inID) != kControlType_ComboBox )   {_ACError("",this);return;}
	
	SetComboStringFromTextArray(GetHWndByControlID(inID),inTextArray);
}

/**
*/
void	CWindowImp::InsertMenuItem( const AControlID inID, const AIndex inMenuItemIndex, const AText& inText )
{
	if( GetControlTypeByID(inID) != kControlType_ComboBox )   {_ACError("",this);return;}
	
	AStCreateWcharStringFromAText	str(inText);
	::SendMessageW(GetHWndByControlID(inID),CB_INSERTSTRING,inMenuItemIndex,(LPARAM)(str.GetPtr()));
}

/**
*/
void	CWindowImp::DeleteMenuItem( const AControlID inID, const AIndex inMenuItemIndex )
{
	if( GetControlTypeByID(inID) != kControlType_ComboBox )   {_ACError("",this);return;}
	
	::SendMessageW(GetHWndByControlID(inID),CB_DELETESTRING,inMenuItemIndex,0);
}

void	CWindowImp::SetComboStringFromTextArray( const HWND inHWnd, const ATextArray& inTextArray )
{
	//全削除
	while( ::SendMessageW(inHWnd,CB_GETCOUNT,0,0) > 0 )
	{
		::SendMessageW(inHWnd,CB_DELETESTRING,0,0);
	}
	//textarray追加
	for( AIndex i = 0; i < inTextArray.GetItemCount(); i++ )
	{
		AStCreateWcharStringFromAText	str(inTextArray.GetTextConst(i));
		::SendMessageW(inHWnd,CB_INSERTSTRING,-1,(LPARAM)(str.GetPtr()));
	}
	//最初の項目を選択する
	if( inTextArray.GetItemCount() > 0 )
	{
		::SendMessageW(inHWnd,CB_SETCURSEL,(WPARAM)0,0);
	}
}

//
void	CWindowImp::RegisterFontMenu( const AControlID inID, const ABool inEnableDefaultFontItem )//#375
{
	if( GetControlTypeByID(inID) != kControlType_ComboBox )   {_ACError("",this);return;}
	
	const ATextArray&	fontnamearray = GetFontNameArray();
	for( AIndex i = 0; i < fontnamearray.GetItemCount(); i++ )
	{
		AText	combotext;
		fontnamearray.Get(i,combotext);
		AStCreateWcharStringFromAText	str(combotext);
		::SendMessageW(GetHWndByControlID(inID),CB_INSERTSTRING,-1,(LPARAM)(str.GetPtr()));
	}
	//#375
	if( inEnableDefaultFontItem == true )
	{
		//mValueIndependentMenuArray_ControlIDに登録（メニュー表示内容と、GetText()/SetText()での値が異なるため）
		AIndex	valueIndependentMenuArrayIndex = mValueIndependentMenuArray_ControlID.Add(inID);
		mValueIndependentMenuArray_ValueArray.AddNewObject();
		mValueIndependentMenuArray_TitleArray.AddNewObject();
		//「（デフォルトのフォント）」のメニュー項目を追加
		AText	defaultFontTitle;
		defaultFontTitle.SetLocalizedText("DefaultFontTitle");
		AStCreateWcharStringFromAText	str(defaultFontTitle);
		::SendMessageW(GetHWndByControlID(inID),CB_INSERTSTRING,0,(LPARAM)(str.GetPtr()));
		AText	defaultFontValue("default");
		mValueIndependentMenuArray_TitleArray.GetObject(valueIndependentMenuArrayIndex).Add(defaultFontTitle);
		mValueIndependentMenuArray_ValueArray.GetObject(valueIndependentMenuArrayIndex).Add(defaultFontValue);
		//各フォントのメニュー項目をmValueIndependentMenuArray_ValueArrayに追加
		for( AIndex i = 0; i < fontnamearray.GetItemCount(); i++ )
		{
			AText	text;
			fontnamearray.Get(i,text);
			mValueIndependentMenuArray_TitleArray.GetObject(valueIndependentMenuArrayIndex).Add(text);
			mValueIndependentMenuArray_ValueArray.GetObject(valueIndependentMenuArrayIndex).Add(text);
		}
	}
}

const ATextArray& CWindowImp::GetFontNameArray()
{
	if( sFontNameArray.GetItemCount() == 0 )
	{
		//フォント一覧作成 
		AText	name;
		//CAppImp::GetLangText(name);
		name.SetCstring("jp_main_About");
		AStCreateWcharStringFromAText	namestr(name);
		HWND	tmphwnd = ::CreateDialogW(CAppImp::GetHResourceInstance(),namestr.GetPtr(),0,NULL);//satDLL
		HDC	hdc = ::GetDC(tmphwnd);
		if( hdc == NULL )   {_ACError("",NULL);return sFontNameArray;}
		LOGFONTW	logfont;
		logfont.lfCharSet = DEFAULT_CHARSET;
		logfont.lfFaceName[0] = 0;
		logfont.lfPitchAndFamily = 0;
		::EnumFontFamiliesEx(hdc,&logfont,(FONTENUMPROC)STATIC_APICB_EnumFontFamExProc,0,0);
		::ReleaseDC(tmphwnd,hdc);
		::DestroyWindow(tmphwnd);
		sFontNameArray.Sort(true);
	}
	return sFontNameArray;
}

int CALLBACK	CWindowImp::STATIC_APICB_EnumFontFamExProc( ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam )
{
	//イベントトランザクション前処理／後処理 #0 ここにこれがないと、Refresh等経由でOSからイベントCB発生したときに、そこでsEventDepth==1となってしまい、
	//Window等のオブジェクトに対してDoObjectArrayItemTrashDelete()が発生し、メンバー変数へのメモリアクセスが発生し、落ちてしまう。
	AStEventTransactionPrePostProcess	oneevent(false);
	
	AText	fontname;
	fontname.SetFromWcharString(lpelfe->elfLogFont.lfFaceName,
			sizeof(lpelfe->elfLogFont.lfFaceName)/sizeof(lpelfe->elfLogFont.lfFaceName[0]));
	if( fontname.GetItemCount() > 0 )
	{
		if( sFontNameArray.Find(fontname) == kIndex_Invalid )
		{
			if( fontname.Get(0) != '@' )
			{
				sFontNameArray.Add(fontname);
			}
		}
	}
	return 1;
}

ATextArray	CWindowImp::sFontNameArray;

//TextArrayMenuへの登録
void	CWindowImp::RegisterTextArrayMenu( const AControlID inControlID, const ATextArrayMenuID inTextArrayMenuID )
{
	//インスタンス内配列へ登録
	mTextArrayMenuArray_ControlID.Add(inControlID);
	/*#232
	mTextArrayMenuArray_TextArrayMenuID.Add(inTextArrayMenuID);
	//static配列へ登録
	sTextArrayMenu_HWnd.Add(GetHWndByControlID(inControlID));
	sTextArrayMenu_TextArrayMenuID.Add(inTextArrayMenuID);
	//コンボボックスへDBのTextArrayを反映
	SetComboStringFromTextArray(GetHWndByControlID(inControlID),sTextArrayMenuDB.GetData_TextArrayRef(inTextArrayMenuID));
	*/
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().RegisterControl(GetAWin().GetObjectID(),inControlID,inTextArrayMenuID);
}

//TextArrayMenu登録解除
void	CWindowImp::UnregisterTextArrayMenu( const AControlID inControlID )
{
	/*#232
	//static配列から削除
	AIndex	staticindex = sTextArrayMenu_HWnd.Find(GetHWndByControlID(inControlID));
	if( staticindex == kIndex_Invalid )   {_ACError("",this);return;}
	sTextArrayMenu_HWnd.Delete1(staticindex);
	sTextArrayMenu_TextArrayMenuID.Delete1(staticindex);
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
	//DBへ反映
	sTextArrayMenuDB.SetData_TextArray(inTextArrayMenuID,inTextArray);
	
	//登録中のメニュー全てに反映
	for( AIndex index = 0; index < sTextArrayMenu_HWnd.GetItemCount(); index++ )
	{
		if( sTextArrayMenu_TextArrayMenuID.Get(index) == inTextArrayMenuID )
		{
			SetComboStringFromTextArray(sTextArrayMenu_HWnd.Get(index),inTextArray);
		}
	}
}

//TextArrayメニュー登録(DB追加)(static)
void	CWindowImp::RegisterTextArrayMenuID( const ATextArrayMenuID inTextArrayMenuID )
{
	sTextArrayMenuDB.CreateData_TextArray(inTextArrayMenuID,"","");
}
*/

/*#232
AArray<HWND>					CWindowImp::sTextArrayMenu_HWnd;
AArray<ATextArrayMenuID>		CWindowImp::sTextArrayMenu_TextArrayMenuID;
ADataBase						CWindowImp::sTextArrayMenuDB;
*/

/*#349
#pragma mark ===========================

#pragma mark --- ラジオボタングループ

//ラジオボタングループ生成
void	CWindowImp::RegisterRadioGroup()
{
	mRadioGroup.AddNewObject();
}

//最後に生成したラジオボタングループにコントロールを追加
void	CWindowImp::AddToLastRegisteredRadioGroup( const AControlID inID )
{
	if( mRadioGroup.GetItemCount() == 0 )   return;
	mRadioGroup.GetObject(mRadioGroup.GetItemCount()-1).Add(inID);
}

//ラジオボタングループのUpdate（指定したコントロール以外、falseに設定する）
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

//ラジオグループに設定されたボタンの場合、Onでdisableにされたら、次のボタンをOnにする。
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

#pragma mark --- ファイル／フォルダ選択

//
void	CWindowImp::ShowChooseFolderWindow( const AFileAcc& inDefaultFolder, 
		const AControlID inVirtualControlID, const ABool inSheet )//#551
{
	//★inDefaultFolder対応必要
	const wchar_t	title[] = L"Select a directory.";//★暫定 ディレクトリ選択文字列対応必要
	wchar_t	path[kPathMaxLength];
	path[0] = 0;
	BROWSEINFO	bi;
	bi.hwndOwner		= mHWnd;
	bi.pidlRoot			= NULL;
	bi.pszDisplayName	= path;
	bi.lpszTitle		= title;
	bi.ulFlags			= BIF_USENEWUI;
	bi.lpfn				= NULL;
	bi.lParam			= 0;
	bi.iImage			= 0;
	PIDLIST_ABSOLUTE	pidl = ::SHBrowseForFolder(&bi);
	if( pidl == NULL )   return;
	if( ::SHGetPathFromIDList(pidl,path) == FALSE )   return;
	::CoTaskMemFree(pidl);
	AText	p;
	p.SetFromWcharString(path,sizeof(path)/sizeof(path[0]));
	AFileAcc	file;
	file.Specify(p);
	GetAWin().EVT_FolderChoosen(inVirtualControlID,file);
}

//
void	CWindowImp::ShowChooseFileWindow( const AFileAcc& inDefaultFile, const AControlID inVirtualControlID, const ABool inOnlyApp, 
		const ABool inSheet )
{
	//★inDefaultFile対応必要
	//★暫定（inOnlyApp等対応必要）
	AText	filter;
	filter.SetCstring("All Files (*.*)");
	filter.Add(0);
	filter.AddCstring("*.*");
	filter.Add(0);
	filter.Add(0);
	wchar_t	path[kPathMaxLength], title[kPathMaxLength];
	path[0] = 0;
	title[0] = 0;
	AStCreateWcharStringFromAText	textptr(filter);
	OPENFILENAMEW	ofn;
	ofn.lStructSize				= sizeof(OPENFILENAMEW);
	ofn.hwndOwner				= mHWnd;
	ofn.hInstance				= NULL;
	ofn.lpstrFilter				= textptr.GetPtr();
	ofn.lpstrCustomFilter		= NULL;
	ofn.nMaxCustFilter			= 0;
	ofn.nFilterIndex			= 0;
	ofn.lpstrFile				= path;
	ofn.nMaxFile				= kPathMaxLength;
	ofn.lpstrFileTitle			= title;
	ofn.nMaxFileTitle			= kPathMaxLength;
	ofn.lpstrInitialDir			= NULL;
	ofn.lpstrTitle				= NULL;
	ofn.Flags					= OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	ofn.nFileOffset				= 0;
	ofn.nFileExtension			= 0;
	ofn.lpstrDefExt				= NULL;
	ofn.lCustData				= 0L;
	ofn.lpfnHook				= NULL;
	ofn.lpTemplateName			= NULL;
	if( GetOpenFileNameW(&ofn) == TRUE )
	{
		AText	text;
		text.SetFromWcharString(path,sizeof(path)/sizeof(path[0]));
		AFileAcc	file;
		file.Specify(text);
		GetAWin().EVT_FileChoosen(inVirtualControlID,file);
	}
}

//
void	CWindowImp::ShowChooseFileWindowWithFileFilter( const AFileAcc& inDefaultFile, const AControlID inVirtualControlID, 
		const ABool inSheet, const AFileAttribute inFilterAttribute )
{
	//★暫定 未対応
	ShowChooseFileWindow(inDefaultFile,inVirtualControlID,false,inSheet);
}

//
void	CWindowImp::ShowAskSaveChangesWindow( const AText& inFileName, const AObjectID inDocumentID )
{
	AText	message;
	message.SetLocalizedText("AskSave");
	message.ReplaceParamText('0',inFileName);
	AStCreateWcharStringFromAText	msgtextptr(message);
	AText	appname;
	GetApplicationName(appname);
	AStCreateWcharStringFromAText	captiontextptr(appname);
	AAskSaveChangesResult	askSaveChanges = kAskSaveChangesResult_Cancel;
	switch(::MessageBox(mHWnd,msgtextptr.GetPtr(),captiontextptr.GetPtr(),
					MB_YESNOCANCEL|MB_APPLMODAL|MB_ICONWARNING) )
	{
	  case IDYES:
		{
			askSaveChanges = kAskSaveChangesResult_Save;
			break;
		}
	  case IDNO:
		{
			askSaveChanges = kAskSaveChangesResult_DontSave;
			break;
		}
	  default:
		{
			askSaveChanges = kAskSaveChangesResult_Cancel;
			break;
		}
	}
	GetAWin().EVT_AskSaveChangesReply(inDocumentID,askSaveChanges);
	AApplication::GetApplication().NVI_UpdateMenu();
}

//
void	CWindowImp::ShowSaveWindow( const AText& inFileName, const ADocumentID inDocumentID, 
		const AFileAttribute inFileAttr, const AText& inFilter, const AText& inRefText, const AFileAcc& inDefaultFolder )
{
	//
	wchar_t	path[kPathMaxLength], title[kPathMaxLength];
	//デフォルトファイル名をpathへ設定（lpstrFileには結果が格納されるので、AStCreateWcharStringFromATextは使えない）
	AText	filename;
	filename.SetText(inFileName);
	filename.ConvertFromUTF8ToAPIUnicode();
	filename.Add(0);//UTF16 NULL文字追加
	filename.Add(0);
	filename.CopyToTextPtr((AUCharPtr)path,kPathMaxLength*sizeof(wchar_t),0,filename.GetItemCount());
	//
	title[0] = 0;
	//
	AStCreateWcharStringFromAText	filtertextptr(inFilter);
	//初期ディレクトリ（lpstrInitialDirはReadOnlyメンバーなので、AStCreateWcharStringFromAText使用可能）
	AText	dirpath;
	inDefaultFolder.GetPath(dirpath);
	AStCreateWcharStringFromAText	dirpathtextptr(dirpath);
	//
	OPENFILENAMEW	ofn;
	ofn.lStructSize				= sizeof(OPENFILENAMEW);
	ofn.hwndOwner				= mHWnd;
	ofn.hInstance				= NULL;
	ofn.lpstrFilter				= filtertextptr.GetPtr();
	ofn.lpstrCustomFilter		= NULL;
	ofn.nMaxCustFilter			= 0;
	ofn.nFilterIndex			= 0;
	ofn.lpstrFile				= path;
	ofn.nMaxFile				= kPathMaxLength;
	ofn.lpstrFileTitle			= title;
	ofn.nMaxFileTitle			= kPathMaxLength;
	ofn.lpstrInitialDir			= dirpathtextptr.GetPtr();;
	ofn.lpstrTitle				= NULL;
	ofn.Flags					= OFN_OVERWRITEPROMPT;
	ofn.nFileOffset				= 0;
	ofn.nFileExtension			= 0;
	ofn.lpstrDefExt				= NULL;
	ofn.lCustData				= 0L;
	ofn.lpfnHook				= NULL;
	ofn.lpTemplateName			= NULL;
	if( GetSaveFileNameW(&ofn) == TRUE )
	{
		AText	text;
		text.SetFromWcharString(path,sizeof(path)/sizeof(path[0]));
		AFileAcc	file;
		file.Specify(text);
		file.CreateFile();
		GetAWin().EVT_SaveWindowReply(inDocumentID,file,inRefText);
	}
}

//Windowsの場合はHWNDが必要なのでファイルオープンダイアログもCWindowImpで実行する
void	CWindowImp::ShowFileOpenWindow( const AText& inFilter, const ABool inShowInvisibleFile )
{
	wchar_t	path[kPathMaxLength], title[kPathMaxLength];
	path[0] = 0;
	title[0] = 0;
	AStCreateWcharStringFromAText	textptr(inFilter);
	OPENFILENAMEW	ofn = {0};
	ofn.lStructSize				= sizeof(OPENFILENAMEW);
	ofn.hwndOwner				= mHWnd;
	ofn.hInstance				= NULL;
	ofn.lpstrFilter				= textptr.GetPtr();
	ofn.lpstrCustomFilter		= NULL;
	ofn.nMaxCustFilter			= 0;
	ofn.nFilterIndex			= 0;
	ofn.lpstrFile				= path;
	ofn.nMaxFile				= kPathMaxLength;
	ofn.lpstrFileTitle			= title;
	ofn.nMaxFileTitle			= kPathMaxLength;
	ofn.lpstrInitialDir			= NULL;
	ofn.lpstrTitle				= NULL;
	ofn.Flags					= OFN_CREATEPROMPT;
	ofn.nFileOffset				= 0;
	ofn.nFileExtension			= 0;
	ofn.lpstrDefExt				= NULL;
	ofn.lCustData				= 0L;
	ofn.lpfnHook				= NULL;
	ofn.lpTemplateName			= NULL;
	ofn.FlagsEx = 0;
	if( GetOpenFileNameW(&ofn) == TRUE )
	{
		AText	text;
		text.SetFromWcharString(path,sizeof(path)/sizeof(path[0]));
		AFileAcc	file;
		file.Specify(text);
		AApplication::GetApplication().NVI_OpenFile(file,mHWnd);
	}
	//DWORD	err = CommDlgExtendedError();
}

#pragma mark ===========================

#pragma mark --- 子ウインドウ管理
/* #291によりAWindow_OK等へ変更
//
void	CWindowImp::ShowChildWindow_OK( const AText& inMessage1, const AText& inMessage2 )
{
	AStCreateWcharStringFromAText	msgtextptr(inMessage2);
	AStCreateWcharStringFromAText	captiontextptr(inMessage1);
	::MessageBox(mHWnd,msgtextptr.GetPtr(),captiontextptr.GetPtr(),MB_OK|MB_APPLMODAL|MB_ICONWARNING);
}

//
void	CWindowImp::ShowChildWindow_OKCancel( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
		const AControlID inOKButtonVirtualControlID )
{
	AStCreateWcharStringFromAText	msgtextptr(inMessage2);
	AStCreateWcharStringFromAText	captiontextptr(inMessage1);
	if( ::MessageBox(mHWnd,msgtextptr.GetPtr(),captiontextptr.GetPtr(),
					MB_OKCANCEL|MB_APPLMODAL|MB_ICONWARNING) == IDOK )
	{
		GetAWin().EVT_Clicked(inOKButtonVirtualControlID,0);
	}
}

//子ウインドウ表示（OKボタン+Cancelボタン+Discardボタン）#8
void	CWindowImp::ShowChildWindow_OKCancelDiscard( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
		const AControlID inOKButtonVirtualControlID, const AControlID inDiscardButtonVirtualControlID )
{
}

//
void	CWindowImp::CloseChildWindow()
{
	//Windowsの場合、MessageBox()で、modalなので不要（コールされない）
}
*/
//
ABool	CWindowImp::ChildWindowVisible() const
{
	//Windowsの場合、MessageBox()で、modalなので不要（コールされない）
	return false;
}

#pragma mark ===========================

#pragma mark --- 背景

AIndex	CWindowImp::RegisterBackground( const AFileAcc& inBackgroundImageFile )
{
	if( inBackgroundImageFile.Exist() == false )   return kIndex_Invalid;
	//
	AText	path;
	inBackgroundImageFile.GetPath(path);
	/*
	AText	suffix;
	path.GetSuffix(suffix);
	if( suffix.Compare(".bmp") == false )   return kIndex_Invalid;
	*/
	//
	for( AIndex i = 0; i < sBackgroundImageArray_File.GetItemCount(); i++ )
	{
		if( sBackgroundImageArray_File.GetObject(i).Compare(inBackgroundImageFile) == true )
		{
			return i;
		}
	}
	//データ読み出し、ポインタ設定
	/*
	AText	imagedata;
	inBackgroundImageFile.ReadTo(imagedata);
	AStTextPtr	fileHeaderPtr(imagedata,0,sizeof(BITMAPFILEHEADER));
	LPBITMAPFILEHEADER	lpFileHeader = (LPBITMAPFILEHEADER)(fileHeaderPtr.GetPtr());
	DWORD	offBits = lpFileHeader->bfOffBits;//Bitmapデータのファイルの最初からのoffset
	AStTextPtr	infoHeaderPtr(imagedata,sizeof(BITMAPFILEHEADER),sizeof(BITMAPINFOHEADER));
	LPBITMAPINFOHEADER	lpInfoHeader = (LPBITMAPINFOHEADER)(infoHeaderPtr.GetPtr());
	AStTextPtr	bmpPtr(imagedata,offBits,imagedata.GetItemCount()-offBits);
	*/
	//
	AStCreateWcharStringFromAText	pathcstr(path);
	Gdiplus::Image	image(pathcstr.GetPtr());
	
	//ビットマップ作成用HDC等取得
	HWND	tmphwnd = ::CreateWindowW(L"main",L"",WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,0,CW_USEDEFAULT,0,NULL,NULL,CAppImp::GetHInstance(),NULL);
	HDC	tmphdc = ::GetDC(tmphwnd);
	HDC	hdc = ::CreateCompatibleDC(tmphdc);
	HBITMAP	hbitmap = ::CreateCompatibleBitmap(tmphdc,image.GetWidth(),image.GetHeight());
	::SelectObject(hdc,hbitmap);
	//
	Gdiplus::Graphics	graphics(hdc);
	graphics.DrawImage(&image,0,0,image.GetWidth(),image.GetHeight());
	/*
	::SetDIBitsToDevice(hdc,
			0,0,//転送先座標
			lpInfoHeader->biWidth,lpInfoHeader->biHeight,//幅、高さ
			0,0,//転送元座標
			0,lpInfoHeader->biHeight,//走査開始番号、走査線の本数
			(char*)(bmpPtr.GetPtr()),//bmpデータ開始アドレス
			(const BITMAPINFO*)(infoHeaderPtr.GetPtr()),
			DIB_RGB_COLORS);
	*/
	//
	//HBITMAP	hbitmap = ::LoadBitmap(CAppImp::GetHInstance(),MAKEINTRESOURCE(IDB_BITMAP1));//zantei
	AIndex index = sBackgroundImageArray_File.AddNewObject();
	sBackgroundImageArray_File.GetObject(index).CopyFrom(inBackgroundImageFile);
	sBackgroundImageArray_HBitmap.Add(hbitmap);
	//ビットマップ作成用HDC等削除
	::DeleteDC(hdc);
	::ReleaseDC(tmphwnd,tmphdc);
	::DestroyWindow(tmphwnd);
	return index;
}

HBITMAP	CWindowImp::GetBackgoundBitmap( const AIndex inBackgroundImageIndex )
{
	return sBackgroundImageArray_HBitmap.Get(inBackgroundImageIndex);
}

AObjectArray<AFileAcc>		CWindowImp::sBackgroundImageArray_File;
AArray<HBITMAP>				CWindowImp::sBackgroundImageArray_HBitmap;

#pragma mark ===========================

#pragma mark --- Line Break

HDC			CWindowImp::sLineBreakOffscreenDC = NULL;
HBITMAP		CWindowImp::sLineBreakOffscreenBitmap = NULL;
HFONT		CWindowImp::sLineBreakHFont = NULL;
AText		CWindowImp::sLineBreakHFont_FontName;
AFloatNumber	CWindowImp::sLineBreakHFont_FontSize = 9.0;
ABool		CWindowImp::sLineBreakHFont_AntiAlias = true;

/**
*/
void	CWindowImp::InitLineBreakOffscreen()
{
	//LineBreak計算用Offscreen生成
	HWND	hwnd = ::CreateWindowW(L"main",L"",WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,0,100,100,NULL,NULL,CAppImp::GetHInstance(),NULL);
	HDC	hdc = ::GetDC(hwnd);
	sLineBreakOffscreenDC = ::CreateCompatibleDC(hdc);
	sLineBreakOffscreenBitmap = ::CreateCompatibleBitmap(hdc,100,100);
	::SelectObject(sLineBreakOffscreenDC,sLineBreakOffscreenBitmap);
	sLineBreakHFont = NULL;
	sLineBreakHFont_FontName.DeleteAll();
	sLineBreakHFont_FontSize = 9.0;
	sLineBreakHFont_AntiAlias = true;
	::ReleaseDC(hwnd,hdc);
	::DestroyWindow(hwnd);
}

/**
*/
AItemCount	CWindowImp::GetLineBreak( CTextDrawData& ioTextDrawData, AItemCount inStartOffset, 
			const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAlias,
			const ANumber inViewWidth )
{
	if( true )
	{
		ABool	antiAlias = false;//inAntiAlias
		//Font未作成、または、前回と違う場合は、作成する
		if(	sLineBreakHFont == NULL ||
			sLineBreakHFont_FontName.Compare(inFontName) == false ||
			sLineBreakHFont_FontSize != inFontSize ||
			sLineBreakHFont_AntiAlias != antiAlias )
		{
			//フォント作成
			if( sLineBreakHFont != NULL )
			{
				::DeleteObject(sLineBreakHFont);
				sLineBreakHFont = NULL;
			}
			sLineBreakHFont = CreateFont_(sLineBreakOffscreenDC,
					inFontName,inFontSize,kTextStyle_Normal,antiAlias);
			sLineBreakHFont_FontName.SetText(inFontName);
			sLineBreakHFont_FontSize = inFontSize;
			sLineBreakHFont_AntiAlias = antiAlias;
			//フォント等設定
			::SelectObject(sLineBreakOffscreenDC,sLineBreakHFont);
			::SetTextColor(sLineBreakOffscreenDC,kColor_Black);
			::SetBkMode(sLineBreakOffscreenDC,TRANSPARENT);
		}
		//半角aのサイズ取得
		SIZE	atextsize;
		wchar_t	atext[1];
		atext[0] = 'a';
		::GetTextExtentPoint32W(sLineBreakOffscreenDC,&(atext[0]),1,&atextsize);
		int	awidth = atextsize.cx;
		if( awidth == 0 )   awidth = 5;
		//全角「あ」のサイズ取得
		SIZE	zatextsize;
		wchar_t	zatext[1];
		zatext[0] = 0x3042;//「あ」
		::GetTextExtentPoint32W(sLineBreakOffscreenDC,&(zatext[0]),1,&zatextsize);
		int zawidth = zatextsize.cx;
		if( zawidth == 0 )   zawidth = 5;
		//行折り返し位置見積もりを行う
		//0x3000以上なら全角「あ」の幅、未満なら半角aの幅を足していき、inViewWidthを超えない最大の文字数を取得→estimateCount
		ANumber	estimateCount = 0;
		ANumber	estimateWidth = 0;
		ANumber	estimateCountMax = (ioTextDrawData.UTF16DrawText.GetItemCount())/sizeof(AUTF16Char)-inStartOffset;
		for( ; estimateCount < estimateCountMax; estimateCount++ )
		{
			//文字に応じて幅を足していく
			AUTF16Char	uniChar = ioTextDrawData.UTF16DrawText.GetUTF16CharFromUTF16Text((inStartOffset+estimateCount)*sizeof(AUTF16Char));
			if( uniChar >= 0x3000 )
			{
				estimateWidth += zawidth;
			}
			else
			{
				estimateWidth += awidth;
			}
			//inViewWidthを超えたら終了
			if( estimateWidth > inViewWidth )
			{
				estimateCount--;
				break;
			}
		}
		//
		AIndex pos = inStartOffset+estimateCount;
		{
			AStTextPtr	utf16textptr(ioTextDrawData.UTF16DrawText,
						inStartOffset*sizeof(AUTF16Char),
						ioTextDrawData.UTF16DrawText.GetItemCount()-inStartOffset*sizeof(AUTF16Char));
			//見積もり値で幅計算
			SIZE	firsttextsize;
			::GetTextExtentPoint32W(sLineBreakOffscreenDC,
						(LPCWSTR)(utf16textptr.GetPtr()),(pos-inStartOffset),&firsttextsize);
			if( firsttextsize.cx < inViewWidth )
			{
				//増やしていって、最初にtextsize.cx > inViewWidthとなったところで終了
				while( pos < ioTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) )
				{
					SIZE	textsize;
					::GetTextExtentPoint32W(sLineBreakOffscreenDC,
								(LPCWSTR)(utf16textptr.GetPtr()),(pos-inStartOffset),&textsize);
					if( textsize.cx > inViewWidth )
					{
						//inViewWidthよりも小さいところが最終結果
						pos--;
						break;
					}
					pos++;
				}
			}
			else
			{
				//減らしていって、最初にtextsize.cx < inViewWidthとなったところで終了
				while( pos > inStartOffset+2 )
				{
					SIZE	textsize;
					::GetTextExtentPoint32W(sLineBreakOffscreenDC,
								(LPCWSTR)(utf16textptr.GetPtr()),(pos-inStartOffset),&textsize);
					if( textsize.cx < inViewWidth )
					{
						//inViewWidthよりも小さいところが最終結果
						break;
					}
					pos--;
				}
			}
		}
		//ワードラップ処理
		if( pos > inStartOffset && pos < ioTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) -1 )
		{
			//★暫定 UCS4化必要
			AUTF16Char	curChar = ioTextDrawData.UTF16DrawText.
					GetUTF16CharFromUTF16Text(pos*sizeof(AUTF16Char));
			AUTF16Char	prevChar = ioTextDrawData.UTF16DrawText.
					GetUTF16CharFromUTF16Text((pos-1)*sizeof(AUTF16Char));
			if( AApplication::GetApplication().NVI_GetUnicodeData().IsAlphabetOrNumber(prevChar) == true && 
						AApplication::GetApplication().NVI_GetUnicodeData().IsAlphabetOrNumber(curChar) == true )
			{
				//１文字ずつ前に戻って、アルファベット以外になる箇所を探す
				AIndex	newpos = pos-1;
				for( ; newpos > inStartOffset; newpos-- )
				{
					AUTF16Char	ch = ioTextDrawData.UTF16DrawText.
							GetUTF16CharFromUTF16Text(newpos*sizeof(AUTF16Char));
					if( AApplication::GetApplication().NVI_GetUnicodeData().IsAlphabetOrNumber(ch) == false )
					{
						//アルファベット以外の位置でbreak
						break;
					}
				}
				if( newpos > inStartOffset )
				{
					//posをアルファベット以外の文字の直後に設定
					pos = newpos+1;
				}
			}
		}
		return pos;
	}
	else if( false )
	{
		/*
		DrawTextEx()がフォントによっては非常に重い(Consolas等)
		*/
		//Font未作成、または、前回と違う場合は、作成する
		if(	sLineBreakHFont == NULL ||
			sLineBreakHFont_FontName.Compare(inFontName) == false ||
			sLineBreakHFont_FontSize != inFontSize ||
			sLineBreakHFont_AntiAlias != inAntiAlias )
		{
			//
			if( sLineBreakHFont != NULL )
			{
				::DeleteObject(sLineBreakHFont);
				sLineBreakHFont = NULL;
			}
			sLineBreakHFont = CreateFont_(sLineBreakOffscreenDC,
					inFontName,inFontSize,kTextStyle_Normal,inAntiAlias);
			sLineBreakHFont_FontName.SetText(inFontName);
			sLineBreakHFont_FontSize = inFontSize;
			sLineBreakHFont_AntiAlias = inAntiAlias;
			//
			::SelectObject(sLineBreakOffscreenDC,sLineBreakHFont);
			::SetTextColor(sLineBreakOffscreenDC,kColor_Black);
			::SetBkMode(sLineBreakOffscreenDC,TRANSPARENT);
		}
		//
		AIndex	result = inStartOffset+1;
		AStTextPtr	utf16textptr(ioTextDrawData.UTF16DrawText,
					inStartOffset*sizeof(AUTF16Char),
					ioTextDrawData.UTF16DrawText.GetItemCount()-inStartOffset*sizeof(AUTF16Char));
		DRAWTEXTPARAMS	dtp = {0};
		dtp.cbSize = sizeof(dtp);
		dtp.iTabLength 		= 8;
		dtp.iLeftMargin 	= 0;
		dtp.iRightMargin 	= 0;
		RECT	rect;
		rect.left 		= 0;
		rect.top 		= 0;
		rect.right 		= inViewWidth;
		rect.bottom 	= 6;
		if( ::DrawTextExW(sLineBreakOffscreenDC,
						(LPWSTR)(utf16textptr.GetPtr()),
						utf16textptr.GetByteCount()/sizeof(AUTF16Char),
						&rect,DT_WORDBREAK|DT_LEFT,&dtp) != 0 )
		{
			result = inStartOffset+dtp.uiLengthDrawn;
		}
		return result;
	}
	else
	{
		/*
		旧（暫定）LineBreak計算処理
		・遅い
		・word wrapできていない
		*/
		if( sAliveWindowArray_HWnd.GetItemCount() == 0 )   {_ACError("",NULL);return ioTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char);}
		HDC	hdc = ::GetDC(sAliveWindowArray_HWnd.Get(0));
		if( hdc == NULL )   {_ACError("",NULL);return ioTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char);}
		
		HFONT	hfont = CreateFont_(hdc,inFontName,inFontSize,kTextStyle_Normal,inAntiAlias);
		::SelectObject(hdc,hfont);
		AIndex	result = inStartOffset+1;
		AIndex pos = inStartOffset+2;
		AStTextPtr	utf16textptr(ioTextDrawData.UTF16DrawText,
					inStartOffset*sizeof(AUTF16Char),ioTextDrawData.UTF16DrawText.GetItemCount()-inStartOffset*sizeof(AUTF16Char));
		while( pos <= ioTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) )
		{
			SIZE	textsize;
			::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),(pos-inStartOffset),&textsize);
			if( textsize.cx > inViewWidth )
			{
				break;
			}
			result = pos;
			pos++;
		}
		::DeleteObject(hfont);
		::ReleaseDC(sAliveWindowArray_HWnd.Get(0),hdc);
		return result;
	}
}

#pragma mark ===========================

#pragma mark --- インライン入力

void	CWindowImp::FixInlineInput( const AControlID inControlID )
{
	AControlID	controlID = GetCurrentFocus();
	if( controlID != kControlID_Invalid )
	{
		if( IsUserPane(controlID) == true )
		{
			GetUserPane(controlID).FixInlineInput(inControlID);
		}
	}
}

//#448
/**
*/
ABool	CWindowImp::GetInlineInputMode()
{
	HIMC	hImc = ::ImmGetContext(mHWnd);
	BOOL	status = ::ImmGetOpenStatus(hImc);
	::ImmReleaseContext(mHWnd,hImc);
	return (status==TRUE);
}

/*#539
#pragma mark ===========================

#pragma mark --- ヘルプ

void	CWindowImp::RegisterHelpAnchor( const ANumber inHelpNumber, AConstCstringPtr inAnchor )
{
	//★暫定 未対応
}
*/
#pragma mark ===========================

#pragma mark --- アイコン登録

AArray<AIconID>		CWindowImp::sIconArray_ID;
AArray<HICON>		CWindowImp::sIconArray_HIcon;
AArray<HICON>		CWindowImp::sIconArray_HIcon_Gray;
AIconID				CWindowImp::sNextDynamicIconID = kDynamicIconIDStart;
AHashTextArray		CWindowImp::sIconNameArray;//#232

void	CWindowImp::RegisterIcon( const AIconID inIconID, LPCTSTR inIconName, const ACstringPtr inName )
{
	sIconArray_ID.Add(inIconID);
	sIconArray_HIcon.Add(::LoadIcon(CAppImp::GetHResourceInstance(),inIconName));//satDLL
	//gray icon
	{
		AText	grayiconname;
		grayiconname.SetFromWcharString(inIconName,32);//
		grayiconname.AddCstring("_GRAY");
		AStCreateWcharStringFromAText	grayiconnamestr(grayiconname);
		sIconArray_HIcon_Gray.Add(::LoadIcon(CAppImp::GetHResourceInstance(),grayiconnamestr.GetPtr()));//satDLL
	}
	//
	AText	name;//#232
	name.SetCstring(inName);//#232
	sIconNameArray.Add(name);//#232
}

AIconID	CWindowImp::RegisterIconFromResouce( const AIndex inResID )
{
	//Windowsは処理なし
	return kIconID_NoIcon;
}

void	CWindowImp::RegisterIconFromResouce( const AIconID inIconID, const AIndex inResID )
{
	//Windowsは処理なし
}

/*#1012
AIconID	CWindowImp::RegisterDynamicIconFromImageFile( const AFileAcc& inFile )
{
	return kIconID_NoIcon;
}
*/

AIconID	CWindowImp::RegisterDynamicIconFromFile( const AFileAcc& inIconFile, const ABool inLoadEvenIfNotCustomIcon )
{
	//".ico"を付けたファイル名のファイルをアイコンファイルとする
	AText	path;
	inIconFile.GetNormalizedPath(path);
	path.AddCstring(".ico");
	AStCreateWcharStringFromAText	pathstring(path);
	//アイコン読み込み、登録
	AIconID	iconID = kIconID_NoIcon;
	HICON	hIcon = (HICON)::LoadImage(NULL,pathstring.GetPtr(),IMAGE_ICON,0,0,LR_LOADFROMFILE);
	if( hIcon != NULL )
	{
		iconID = AssignDynamicIconID();
		sIconArray_ID.Add(iconID);
		sIconArray_HIcon.Add(hIcon);
		sIconArray_HIcon_Gray.Add(NULL);
		AText	name;//#232
		sIconNameArray.Add(name);//#232
	}
	return iconID;
}

void	CWindowImp::UnregisterDynamicIcon( const AIconID inIconID )
{
}

//#232
/**
アイコンをファイルへコピー
*/
void	CWindowImp::CopyIconToFile( const AIconID inIconID, AFileAcc& inDestIconFile )
{
	//★暫定 対応必要
}

//#232
/**
アイコンファイル拡張子取得
*/
void	CWindowImp::GetIconFileSuffix( AText& outSuffix )
{
	outSuffix.SetCstring(".ico");
}

AIconID	CWindowImp::AssignDynamicIconID()
{
	while( sIconArray_ID.Find(sNextDynamicIconID) != kIndex_Invalid )
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

//
HICON	CWindowImp::GetHIconByIconID( const AIconID inIconID, const ABool inGray )
{
	AIndex	index = sIconArray_ID.Find(inIconID);
	if( index == kIndex_Invalid )
	{
		//_AError("icon not registered",NULL);
		return ::LoadIcon(CAppImp::GetHResourceInstance(),MAKEINTRESOURCE(IDI_SMALL));//satDLL
	}
	if( inGray == false )
	{
		return sIconArray_HIcon.Get(index);
	}
	else
	{
		return sIconArray_HIcon_Gray.Get(index);
	}
}

/**
*/
ABool	CWindowImp::IsDynamicIcon( const AIconID inIconID )
{
	return ((inIconID>=kDynamicIconIDStart)&&(inIconID<=kDynamicIconIDEnd));
}

/**
アイコン名前からIconID取得
*/
AIconID	CWindowImp::GetIconIDByName( const AText& inName )
{
	AIndex	index = sIconNameArray.Find(inName);
	if( index == kIndex_Invalid )   return kIconID_NoIcon;
	return sIconArray_ID.Get(index);
}


#pragma mark ===========================

#pragma mark --- APIコールバック

ABool	CWindowImp::APICB_WndProc( UINT message, WPARAM wParam, LPARAM lParam, LRESULT& outResult )
{
	outResult = 0;
	ABool	handled = true;
	switch(message)
	{
	  case WM_PAINT:
		{
			/*これを行うとウインドウ再描画時にWindowでの白塗りつぶし→Viewでのオフスクリーンコピーが発生し、ちらついてしまう
			//Viewのない場所でも白色で塗りつぶすように、Window全体をFillRect()する
			PAINTSTRUCT ps;
			HDC	hdc = ::BeginPaint(mHWnd,&ps);
			if( hdc == NULL )   {_ACError("",NULL);break;}
			HBRUSH	hbr = ::CreateSolidBrush(kColor_White);
			if( hbr == NULL )   {_ACError("",NULL);break;}
			WINDOWINFO	wininfo;
			::GetWindowInfo(mHWnd,&wininfo);
			RECT	rect;
			rect.left		= 0;
			rect.top		= 0;
			rect.right		= wininfo.rcClient.right - wininfo.rcClient.left;
			rect.bottom		= wininfo.rcClient.bottom - wininfo.rcClient.top;
			::FillRect(hdc,&rect,hbr);
			::DeleteObject(hbr);
			::EndPaint(mHWnd,&ps);
			//CUserPaneでも描画を行うため、falseを返す
			*/
			handled = false;
			break;
		}
	  case WM_CREATE:
		{
			/*
			HDC	hdc = ::GetDC(mHWnd);
			if( hdc == NULL )   {_ACError("",NULL);break;}
			HBRUSH	hbr = ::CreateSolidBrush(kColor_White);
			if( hbr == NULL )   {_ACError("",NULL);break;}
			WINDOWINFO	wininfo;
			::GetWindowInfo(mHWnd,&wininfo);
			RECT	rect;
			rect.left		= 0;
			rect.top		= 0;
			rect.right		= wininfo.rcClient.right - wininfo.rcClient.left;
			rect.bottom		= wininfo.rcClient.bottom - wininfo.rcClient.top;
			::FillRect(hdc,&rect,hbr);
			::DeleteObject(hbr);
			::ReleaseDC(mHWnd,hdc);
			*/
			handled = false;
			break;
		}
	  case WM_MENUCOMMAND:
		{
			//メニュー選択（MNS_NOTIFYBYPOSを設定しているのでWM_MENUCOMMANDでくる）
			//actiontext取得のため、menurefとindexが欲しいので、WM_MENUCOMMANDを使う
			
			AMenuRef	menuref = (AMenuRef)lParam;
			AIndex	menuIndex = (AIndex)wParam;
			AMenuItemID	menuItemID = AMenuWrapper::GetMenuItemID(menuref,menuIndex);
			//actiontext取得
			AText	actiontext;
			AApplication::GetApplication().NVI_GetMenuManager().GetMenuActionText(menuItemID,menuref,menuIndex,actiontext);
			//Winで実行試行
			if( GetAWin().EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
			{
				AApplication::GetApplication().NVI_UpdateMenu();
				::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
				break;
			}
			//親Windowで実行試行
			if( mParentWindowRef != NULL )
			{
				AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mParentWindowRef);
				if( parentWindowIndex != kIndex_Invalid )
				{
					if( (sAliveWindowArray.Get(parentWindowIndex))->GetAWin().
								EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
					{
						AApplication::GetApplication().NVI_UpdateMenu();
						::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
						break;
					}
				}
			}
			//Appで実行試行
			else if( AApplication::GetApplication().EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
			{
				AApplication::GetApplication().NVI_UpdateMenu();
				::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
				break;
			}
			break;
		}
	  case WM_COMMAND:
		{
			if( lParam == 0 )
			{
				//アクセラレータキーによるメニュー選択（通常のメニュー選択はMNS_NOTIFYBYPOSを設定しているのでWM_MENUCOMMANDでくる）
				
				int wmId, wmEvent;
				wmId    = LOWORD(wParam);
				wmEvent = HIWORD(wParam);
				AText	actiontext;
				AMenuItemID	menuItemID = wmId;
				//actiontextはメニューテキストをそのまま使う
				AApplication::GetApplication().NVI_GetMenuManager().GetMenuItemText(menuItemID,actiontext);
				//Winで実行試行
				if( GetAWin().EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
				{
					AApplication::GetApplication().NVI_UpdateMenu();
					::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
					break;
				}
				//親Windowで実行試行
				if( mParentWindowRef != NULL )
				{
					AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mParentWindowRef);
					if( parentWindowIndex != kIndex_Invalid )
					{
						if( (sAliveWindowArray.Get(parentWindowIndex))->GetAWin().
									EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
						{
							AApplication::GetApplication().NVI_UpdateMenu();
							::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
							break;
						}
					}
				}
				//Appで実行試行
				else if( AApplication::GetApplication().EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
				{
					AApplication::GetApplication().NVI_UpdateMenu();
					::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
					break;
				}
			}
			else
			{
				//子コントロールからのNotificationメッセージ
				//BS_NOTIFYを設定しない場合はBN_CLICKEDのみ、設定した場合はBN_DBLCLK, BN_SETFOCUS, BN_KILLFOCUSも。
				//基本的にコントロールのサブクラスで対応するので、BN_SETFOCUS等は処理しない。
				
				ABool	changed = false;
				AControlID	controlID = LOWORD(wParam);
				WORD	notificationCode = HIWORD(wParam);
				switch( GetControlTypeByID(controlID) )
				{
				  case kControlType_Button:
				  case kControlType_Slider:
					{
						if( notificationCode == BN_CLICKED )
						{
							changed = true;
						}
						break;
					}
				  case kControlType_ComboBox:
					{
						if( notificationCode == CBN_SELCHANGE )
						{
							//CBN_SELCHANGEはテキストボックス変更前に通知されるので、
							//EVT_ValueChanged()内からGetText()をコールしたときに選択後のテキストが選択されるように、
							//ここでテキストボックスを更新しておく。
							LRESULT	sel = ::SendMessageW(GetHWndByControlID(controlID),CB_GETCURSEL,0,0);
							LRESULT	len = ::SendMessageW(GetHWndByControlID(controlID),CB_GETLBTEXTLEN,sel,0);
							AText	text;
							text.Reserve(0,len*sizeof(wchar_t));
							{
								AStTextPtr	textptr(text,0,text.GetItemCount());
								::SendMessageW(GetHWndByControlID(controlID),CB_GETLBTEXT,sel,
										(LPARAM)(textptr.GetPtr()));
								::SetWindowTextW(GetHWndByControlID(controlID),(LPCWSTR)textptr.GetPtr());
							}
							changed = true;
						}
						break;
					}
				  case kControlType_Edit:
					{
						if( notificationCode == EN_CHANGE )
						{
							GetAWin().EVT_TextInputted(controlID);
						}
						break;
					}
					//Editコントロール等は、編集中にEVT_ValueChanged()等を実行すると、値補正が入ったりしてしまう
				  default:
					{
						//処理無し
						break;
					}
				}
				if( changed == true )
				{
					/*#349
					//
					UpdateRadioGroup(controlID);
					*/
					//
					if( GetAWin().EVT_Clicked(controlID,AKeyWrapper::GetEventKeyModifiers()) == true )
					{
						break;
					}
					if( GetAWin().EVT_ValueChanged(controlID) == true )
					{
						break;
					}
				}
			}
			break;
		}
		/*#688 直接実行に変更
	  case WM_APP_UPDATEMENU:
		{
			GetAWin().EVT_UpdateMenu();
			AApplication::GetApplication().EVT_UpdateMenu();
			break;
		}
		*/
	  case WM_VSCROLL:
		{
			AControlID	controlID = GetControlIDByHWnd((HWND)lParam);
			AScrollBarPart	scrollBarpart = kScrollBarPart_None;
			switch(LOWORD(wParam))
			{
			  case SB_PAGEDOWN:
				{
					scrollBarpart = kScrollBarPart_PageDownButton;
					break;
				}
			  case SB_LINEDOWN:
				{
					scrollBarpart = kScrollBarPart_DownButton;
					break;
				}
			  case SB_PAGEUP:
				{
					scrollBarpart = kScrollBarPart_PageUpButton;
					break;
				}
			  case SB_LINEUP:
				{
					scrollBarpart = kScrollBarPart_UpButton;
					break;
				}
			  case SB_THUMBTRACK:
				{
					SCROLLINFO si;
					::ZeroMemory(&si,sizeof(si));
					si.cbSize = sizeof(si);
					si.fMask = SIF_TRACKPOS;
					::GetScrollInfo((HWND)lParam,SB_CTL,&si);
					SetNumber(controlID,si.nTrackPos);
					scrollBarpart = kScrollBarPart_ThumbTracking;
					break;
				}
			  default:
				{
					/*処理無し*/
					break;
				}
			}
			GetAWin().EVT_DoScrollBarAction(controlID,scrollBarpart);
			break;
		}
	  case WM_HSCROLL:
		{
			AControlID	controlID = GetControlIDByHWnd((HWND)lParam);
			AScrollBarPart	scrollBarpart = kScrollBarPart_None;
			switch(LOWORD(wParam))
			{
			  case SB_PAGEDOWN:
				{
					scrollBarpart = kScrollBarPart_PageDownButton;
					break;
				}
			  case SB_LINEDOWN:
				{
					scrollBarpart = kScrollBarPart_DownButton;
					break;
				}
			  case SB_PAGEUP:
				{
					scrollBarpart = kScrollBarPart_PageUpButton;
					break;
				}
			  case SB_LINEUP:
				{
					scrollBarpart = kScrollBarPart_UpButton;
					break;
				}
			  case SB_THUMBTRACK:
				{
					SCROLLINFO si;
					::ZeroMemory(&si,sizeof(si));
					si.cbSize = sizeof(si);
					si.fMask = SIF_TRACKPOS;
					::GetScrollInfo((HWND)lParam,SB_CTL,&si);
					SetNumber(controlID,si.nTrackPos);
					scrollBarpart = kScrollBarPart_ThumbTracking;
					break;
				}
			  default:
				{
					/*処理無し*/
					break;
				}
			}
			GetAWin().EVT_DoScrollBarAction(controlID,scrollBarpart);
			break;
		}
		//マウスホイール
		//別ウインドウ上でのマウスホイールはそのウインドウ上でのスクロールにするため、
		//WM_APP_MOUSEWHEELイベントに変換してリダイレクトする。
	  case WM_MOUSEWHEEL:
		{
			AGlobalPoint	globalPoint;
			globalPoint.x = GET_X_LPARAM(lParam);
			globalPoint.y = GET_Y_LPARAM(lParam);
			//子オーバーレイウインドウ内をはじめに検索
			ABool	found = false;
			for( AIndex i = 0; i < mOverlayWindowGroup_WindowRef.GetItemCount(); i++ )
			{
				RECT	rect;
				::GetWindowRect(mOverlayWindowGroup_WindowRef.Get(i),&rect);
				if( (globalPoint.x >= rect.left && globalPoint.x <= rect.right) &&
							(globalPoint.y >= rect.top && globalPoint.y <= rect.bottom) )
				{
					//
					::SendMessageW(mOverlayWindowGroup_WindowRef.Get(i),WM_APP_MOUSEWHEEL,wParam,lParam);
					found = true;
				}
			}
			if( found == false )
			{
				//
				RECT	rect;
				::GetWindowRect(mHWnd,&rect);
				if( (globalPoint.x >= rect.left && globalPoint.x <= rect.right) &&
							(globalPoint.y >= rect.top && globalPoint.y <= rect.bottom) )
				{
					//
					::SendMessageW(mHWnd,WM_APP_MOUSEWHEEL,wParam,lParam);
				}
				else
				{
					//別ウインドウ上でのホイール
					HWND	hwnd = mHWnd;
					while( hwnd != NULL )
					{
						hwnd = ::GetNextWindow(hwnd,GW_HWNDNEXT);
						if( hwnd == NULL )   break;
						::GetWindowRect(hwnd,&rect);
						if( (globalPoint.x >= rect.left && globalPoint.x <= rect.right) &&
									(globalPoint.y >= rect.top && globalPoint.y <= rect.bottom) )
						{
							::SendMessageW(hwnd,WM_APP_MOUSEWHEEL,wParam,lParam);
							break;
						}
					}
				}
			}
			break;
		}
	  case WM_APP_MOUSEWHEEL:
		{
			AGlobalPoint	globalPoint;
			globalPoint.x = GET_X_LPARAM(lParam);
			globalPoint.y = GET_Y_LPARAM(lParam);
			AControlID	controlID = FindControl(globalPoint);
			if( IsUserPane(controlID) == true )
			{
				AFloatNumber	delta = -(signed short)HIWORD(wParam)/50;
				/*wParamだとctrl, shift以外は取得できないので、AKeyWrapper::GetEventKeyModifiers()を使うことにする
				AModifierKeys	modifiers = 0;
				if( (GET_KEYSTATE_WPARAM(wParam)&MK_CONTROL) != 0 )
				{
					modifiers |= kModifierKeysMask_Control;
				}
				if( (GET_KEYSTATE_WPARAM(wParam)&MK_SHIFT) != 0 )
				{
					modifiers |= kModifierKeysMask_Shift;
				}
				*/
				AModifierKeys	modifiers = AKeyWrapper::GetEventKeyModifiers();
				POINT	pt;
				pt.x = globalPoint.x;
				pt.y = globalPoint.y;
				::ScreenToClient(GetUserPane(controlID).GetHWnd(),&pt);
				ALocalPoint	localPoint;
				localPoint.x = pt.x;
				localPoint.y = pt.y;
				GetUserPane(controlID).DoMouseWheel(delta,modifiers,localPoint);
			}
			break;
		}
	  case WM_SIZE:
		{
			//AGlobalRect	rect;
			//GetWindowBounds(rect);
			//char	str[256];
			//sprintf(str,"WM_SIZE hwnd:%X msg:%d left:%d right:%d \n",mHWnd,wParam,rect.left,rect.right);
			//OutputDebugStringA(str);
			
			switch(wParam)
			{
			  case SIZE_MAXIMIZED:
				{
					//
					mZoomed = true;
					//
					AGlobalRect	prev, current;
					prev = mCurrentWindowBounds;
					GetWindowBounds(mCurrentWindowBounds);
					current = mCurrentWindowBounds;
					//Bindings適用
					ApplyBindings(prev,current);
					//
					if(	mCurrentWindowSize_Width != current.right-current.left ||
						mCurrentWindowSize_Height != current.bottom-current.top )
					{
						GetAWin().EVT_WindowResizeCompleted();
						mCurrentWindowSize_Width = current.right-current.left;
						mCurrentWindowSize_Height = current.bottom-current.top;
					}
					//
					GetAWin().EVT_WindowBoundsChanged(prev,current);
					break;
				}
			  case SIZE_RESTORED:
				{
					//
					mZoomed = false;
					//
					AGlobalRect	prev, current;
					prev = mCurrentWindowBounds;
					GetWindowBounds(mCurrentWindowBounds);
					current = mCurrentWindowBounds;
					//Bindings適用
					ApplyBindings(prev,current);
					//
					GetAWin().EVT_WindowBoundsChanged(prev,current);
					//
					mUnzoomedBounds = mCurrentWindowBounds;
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			break;
		}
	  case WM_EXITSIZEMOVE:
		{
			AGlobalRect	prev, current;
			prev = mCurrentWindowBounds;
			GetWindowBounds(mCurrentWindowBounds);
			current = mCurrentWindowBounds;
			//Bindings適用
			ApplyBindings(prev,current);
			//
			if(	mCurrentWindowSize_Width != current.right-current.left ||
				mCurrentWindowSize_Height != current.bottom-current.top )
			{
				GetAWin().EVT_WindowResizeCompleted();
				mCurrentWindowSize_Width = current.right-current.left;
				mCurrentWindowSize_Height = current.bottom-current.top;
			}
			//
			GetAWin().EVT_WindowBoundsChanged(prev,current);
			break;
		}
	  case WM_MOVE:
		{
			//char	str[256];
			//sprintf(str,"WM_MOVE hwnd:%X\n",mHWnd);
			//OutputDebugStringA(str);
			
			//Minimize解除判定
			if( mCurrentWindowBounds.left == -32000 && mCurrentWindowBounds.top == -32000 )
			{
				//Minimize解除を通知
				GetAWin().EVT_WindowMinimizeRestored();
				//Overlayウインドウが先に描画されてその後にMinimize解除アニメーションが発生するのを防ぐ
				::UpdateWindow(mHWnd);//画面描画
				::Sleep(250);//Minimize解除アニメーション実行待ち
			}
			
			AGlobalRect	prev, current;
			prev = mCurrentWindowBounds;
			GetWindowBounds(mCurrentWindowBounds);
			current = mCurrentWindowBounds;
			//Bindings適用
			ApplyBindings(prev,current);
			
			//
			RECT	newRect = {0,0,0,0};
			::GetWindowRect(mHWnd,&newRect);
			//ウインドウ同時移動開始
			HDWP	hdwp = ::BeginDeferWindowPos(mOverlayWindowGroup_WindowRef.GetItemCount()*2);
			//各Overlayウインドウ移動
			for( AIndex i = 0; i < mOverlayWindowGroup_WindowRef.GetItemCount(); i++ )
			{
				//Overlayウインドウ移動
				::DeferWindowPos(hdwp,mOverlayWindowGroup_WindowRef.Get(i),HWND_TOP,
						newRect.left	+ mOverlayWindowGroup_XOffset.Get(i),
						newRect.top		+ mOverlayWindowGroup_YOffset.Get(i),
						0,
						0,
						SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
				//OverlayBackgroundウインドウ移動
				::DeferWindowPos(hdwp,mOverlayWindowGroup_BackgroundWindowRef.Get(i),HWND_TOP,
						newRect.left	+ mOverlayWindowGroup_XOffset.Get(i),
						newRect.top		+ mOverlayWindowGroup_YOffset.Get(i),
						0,
						0,
						SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
			}
			//ウインドウ同時移動実行
			::EndDeferWindowPos(hdwp);
			
			//Minimizeかどうかの判定
			if( mCurrentWindowBounds.left == -32000 && mCurrentWindowBounds.top == -32000 )
			{
				//Minimizeの場合、Minimize通知
				GetAWin().EVT_WindowMinimized();
				//フォーカスをなしに設定する。Minimize解除時（Activate時）に、SetFocus()させるため。
				mCurrentFocus = kIndex_Invalid;
			}
			else
			{
				//Minimize以外の場合、WindowBoundsChanged通知
				GetAWin().EVT_WindowBoundsChanged(prev,current);
			}
			break;
		}
	  case WM_SIZING:
	  case WM_WINDOWPOSCHANGING:
		{
			//処理なし
			//サイズ変更中に、WM_SIZEも連続して送付される。
			//Windowsの設定にもよるが、デフォルトはサイズ変更中にもWM_SIZEが呼ばれる。
			//おそらく、Windows95?当初は変更中にはWM_SIZINGのみだったと思われるが、動的変更のために仕様が変更されたと思われる。
			break;
		}
	  case WM_MOVING:
		{
			//AGlobalRect	rect;
			//GetWindowBounds(rect);
			//char	str[256];
			//sprintf(str,"WM_MOVING hwnd:%X msg:%d left:%d right:%d \n",mHWnd,wParam,rect.left,rect.right);
			//OutputDebugStringA(str);
			
			AGlobalRect	prev, current;
			prev = mCurrentWindowBounds;
			//Overlayウインドウを一緒に移動する
			//移動先ウインドウ位置取得
			RECT	newRect = *((LPRECT)lParam);
			//sprintf(str,"WM_MOVING(new) left:%d right:%d \n",newRect.left,newRect.right);
			//OutputDebugStringA(str);
			
			//Windows7で最大化吸着したさい、DeferWindowPosで自ウインドウ移動すると、吸着解除、吸着を繰り返してしまうので、その回避策。
			if( newRect.left == 0 )   break;
			
			//ウインドウ同時移動開始
			HDWP	hdwp = ::BeginDeferWindowPos(1+mOverlayWindowGroup_WindowRef.GetItemCount()*2);
			//自分移動
			::DeferWindowPos(hdwp,mHWnd,HWND_TOP,
					newRect.left,
					newRect.top,
					newRect.right-newRect.left,
					newRect.bottom-newRect.top,
					SWP_NOZORDER|SWP_NOACTIVATE);
			//各Overlayウインドウ移動
			for( AIndex i = 0; i < mOverlayWindowGroup_WindowRef.GetItemCount(); i++ )
			{
				//Overlayウインドウ移動
				::DeferWindowPos(hdwp,mOverlayWindowGroup_WindowRef.Get(i),HWND_TOP,
						newRect.left	+ mOverlayWindowGroup_XOffset.Get(i),
						newRect.top		+ mOverlayWindowGroup_YOffset.Get(i),
						0,
						0,
						SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
				//OverlayBackgroundウインドウ移動
				::DeferWindowPos(hdwp,mOverlayWindowGroup_BackgroundWindowRef.Get(i),HWND_TOP,
						newRect.left	+ mOverlayWindowGroup_XOffset.Get(i),
						newRect.top		+ mOverlayWindowGroup_YOffset.Get(i),
						0,
						0,
						SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
			}
			//ウインドウ同時移動実行
			::EndDeferWindowPos(hdwp);
			//
			GetWindowBounds(mCurrentWindowBounds);
			current = mCurrentWindowBounds;
			GetAWin().EVT_WindowBoundsChanged(prev,current);
			break;
		}
	  case WM_ACTIVATE:
		{
			switch(wParam)
			{
			  case WA_ACTIVE:
			  case WA_CLICKACTIVE:
				{
					//
					GetAWin().EVT_WindowActivated();
					//UserPaneへActivated通知
					for( AIndex i = 0; i < mUserPaneArray.GetItemCount(); i++ )
					{
						mUserPaneArray.GetObject(i).DoWindowActivated();
					}
					//フォーカスを復元
					if( mLatentFocus != kControlID_Invalid && mCurrentFocus == kControlID_Invalid )
					{
						SetFocus(mLatentFocus);
						//#0 mLatentFocus = kControlID_Invalid;
					}
					//menu
					if( mEachMenu != NULL )
					{
						AMenuRef	rootmenu = AMenuWrapper::GetRootMenu();
						//AMenuRef	m = ::GetMenu(mHWnd);
						//同じメニュー(RootMenu)を複数ウインドウに割り当てても問題なさそう
						//その場合、SetMenu()がウインドウ切り替えのたびに動作しないのでメニューのちらつきが減少する
						if( ::GetMenu(mHWnd) != rootmenu )
						{
							::SetMenu(mHWnd,rootmenu);
						}
						AApplication::GetApplication().NVI_UpdateMenu();
					}
					//#404 フルスクリーンTOPMOST設定
					if( mFullScreenMode == true && mWindowModalParentMode == false )
					{
						::SetWindowPos(mHWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
					}
					break;
				}
			  case WA_INACTIVE:
				{
					//現在のフォーカスを保存して、フォーカスをクリア
					if( /*#212 mLatentFocus == kControlID_Invalid &&*/ mCurrentFocus != kControlID_Invalid )
					{
						mLatentFocus = mCurrentFocus;
						ClearFocus(false);//#212
					}
					//UserPaneへDeactivated通知
					for( AIndex i = 0; i < mUserPaneArray.GetItemCount(); i++ )
					{
						mUserPaneArray.GetObject(i).DoWindowDeactivated();
					}
					//
					GetAWin().EVT_WindowDeactivated();
					//menu
					/*if( mEachMenu != NULL )
					{
						::SetMenu(mHWnd,mEachMenu);ここでSetMenuするとタイトルバーがアクティブになる
					}*/
					//#404 フルスクリーンTOPMOST解除
					if( mFullScreenMode == true && mWindowModalParentMode == false )
					{
						::SetWindowPos(mHWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
					}
					break;
				}
			  default:
				{
					//処理無し
					break;
				}
			}
			break;
		}
	  case WM_CLOSE:
		{
			GetAWin().EVT_DoCloseButton();
			break;
		}
	  case WM_NOTIFY:
		{
			AControlID	controlID = ((NMHDR*)lParam)->idFrom;
			UINT	code = ((NMHDR*)lParam)->code;
			switch(code)
			{
			  case TCN_SELCHANGE:
				{
					if( controlID == kControlID_TabControl )
					{
						mCurrentTabIndex = TabCtrl_GetCurSel(((NMHDR*)lParam)->hwndFrom);
						UpdateTab();
					}
					break;
				}
			}
			break;
		}
		/*この処理はダイアログクリック時にメインウインドウがアクティブになるなどの問題があり、さらに必要ないと思われるためコメントアウト
	  case WM_ACTIVATEAPP:
		{
			if( ((BOOL)(wParam)) == TRUE )
			{
				//for( AIndex i = 0; i < sFloatingWindowArray.GetItemCount(); i++ )
				if( IsFloatingWindow() == true )
				{
					::SetWindowPos(mHWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
				}
			}
			break;
		}
		*/
	  case WM_DROPFILES:
		{
			HDROP	hdrop = (HDROP)wParam;
			UINT	count = ::DragQueryFile(hdrop,0xFFFFFFFF,NULL,0);
			for( UINT i = 0; i < count; i++ )
			{
				wchar_t	name[4096];
				::DragQueryFile(hdrop,i,name,4096);
				AText	path;
				path.SetFromWcharString(name,sizeof(name)/sizeof(name[0]));
				AFileAcc	file;
				file.Specify(path);
				POINT	pt;
				::DragQueryPoint(hdrop,&pt);
				::ClientToScreen(mHWnd,&pt);//global pointへ変換
				AGlobalPoint	gpt;
				gpt.x = pt.x;
				gpt.y = pt.y;
				AControlID	controlID = FindControl(gpt);
				GetAWin().EVT_DoFileDropped(controlID,file);
			}
			::DragFinish(hdrop);
			break;
		}
	  case WM_DRAWITEM:
		{
			DoOwnerDraw((UINT)wParam,(LPDRAWITEMSTRUCT)lParam);
			break;
		}
		
		//マウス関連処理（OverlayWindowへのリダイレクトのみ）（子WNDの無い箇所のみここが呼ばれる。子WNDが無い場合、する処理はOverlayへのredirectのみ）
		//マウスクリック
	  case WM_LBUTTONDOWN:
	  case WM_RBUTTONDOWN:
	  case WM_LBUTTONDBLCLK:
	  case WM_RBUTTONDBLCLK:
	  case WM_LBUTTONUP:
	  case WM_RBUTTONUP:
	  case WM_MOUSEMOVE:
		{
			POINT	pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			//globalPoint取得
			AGlobalPoint	globalPoint;
			::ClientToScreen(mHWnd,&pt);
			globalPoint.x = pt.x;
			globalPoint.y = pt.y;
			//OverlayWindowへのリダイレクト実行試行（もし実行したならtrueで返る）
			if( RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
			{
				handled = true;
				break;
			}
			//
			//ACursorWrapper::ShowCursor();
			break;
		}
		//#505
	  case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO	lpmmi = (LPMINMAXINFO)lParam;
			lpmmi->ptMinTrackSize.x = mWindowMinWidth;
			lpmmi->ptMinTrackSize.y = mWindowMinHeight;
			break;
		}
	  default:
		{
			handled = false;
			break;
		}
	}
	return handled;
}

/**
OverlayWindowへのリダイレクト実行試行（親側検索・ディスパッチ処理）
*/
ABool	CWindowImp::RedirectToOverlayWindow( UINT message, WPARAM wParam, LPARAM lParam, const AGlobalPoint inGlobalPoint )
{
	//子オーバーレイウインドウを検索（マウス位置にヒットするウインドウ）し、最初にヒットしたウインドウにディスパッチ
	for( AIndex i = 0; i < mOverlayWindowGroup_WindowRef.GetItemCount(); i++ )
	{
		RECT	rect;
		::GetWindowRect(mOverlayWindowGroup_WindowRef.Get(i),&rect);
		if( (inGlobalPoint.x >= rect.left && inGlobalPoint.x <= rect.right) &&
					(inGlobalPoint.y >= rect.top && inGlobalPoint.y <= rect.bottom) )
		{
			AIndex	windowIndex = sAliveWindowArray_HWnd.Find(mOverlayWindowGroup_WindowRef.Get(i));
			if( windowIndex == kIndex_Invalid )   { _ACError("",this); break; }
			if( (sAliveWindowArray.Get(windowIndex))->DoOverlayWindowRedirect(message,wParam,lParam,inGlobalPoint) == true )
			{
				return true;
			}
		}
	}
	return false;
}

/**
OverlayWindowへのリダイレクト実行（OverlayWindow側実行処理）
*/
ABool	CWindowImp::DoOverlayWindowRedirect( UINT message, WPARAM wParam, LPARAM lParam, const AGlobalPoint inGlobalPoint )
{
	AControlID	controlID = FindControl(inGlobalPoint);
	if( IsUserPane(controlID) == false )//   return false;
	{
		//UserPane以外のコントロールはここで実行
		for( AIndex i = 0; i < mControlArray_HWnd.GetItemCount(); i++ )
		{
			POINT	pt;
			pt.x = inGlobalPoint.x;
			pt.y = inGlobalPoint.y;
			HWND	controlHWnd = mControlArray_HWnd.Get(i);
			if( ::IsWindowVisible(controlHWnd) == true )
			{
				WINDOWINFO	wininfo = {0};
				::GetWindowInfo(controlHWnd,&wininfo);
				if( pt.x >= wininfo.rcClient.left && pt.x <= wininfo.rcClient.right &&
							pt.y >= wininfo.rcClient.top && pt.y <= wininfo.rcClient.bottom )
				{
					::ScreenToClient(controlHWnd,&pt);
					APICB_ControlSubclassProc(controlHWnd,message,wParam,MAKELPARAM(pt.x,pt.y));
					return true;
				}
			}
		}
		return false;
	}
	
	//ControlLocal取得
	POINT	pt;
	pt.x = inGlobalPoint.x;
	pt.y = inGlobalPoint.y;
	::ScreenToClient(GetUserPane(controlID).GetHWnd(),&pt);
	ALocalPoint	localPoint;
	localPoint.x = pt.x;
	localPoint.y = pt.y;
	
	//UserPaneでの処理実行
	ABool	handled = false;
	switch(message)
	{
	  case WM_LBUTTONDOWN:
		{
			if( GetUserPane(controlID).DoMouseDown(localPoint,AKeyWrapper::GetEventKeyModifiers(),1,false) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_RBUTTONDOWN:
		{
			if( GetUserPane(controlID).DoMouseDown(localPoint,AKeyWrapper::GetEventKeyModifiers(),1,true) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_LBUTTONDBLCLK:
		{
			if( GetUserPane(controlID).DoMouseDown(localPoint,AKeyWrapper::GetEventKeyModifiers(),2,false) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_RBUTTONDBLCLK:
		{
			if( GetUserPane(controlID).DoMouseDown(localPoint,AKeyWrapper::GetEventKeyModifiers(),2,true) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_LBUTTONUP:
		{
			if( GetUserPane(controlID).DoMouseUp(localPoint,AKeyWrapper::GetEventKeyModifiers(),1,false) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_RBUTTONUP:
		{
			if( GetUserPane(controlID).DoMouseUp(localPoint,AKeyWrapper::GetEventKeyModifiers(),1,true) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_MOUSEMOVE:
		{
			if( GetUserPane(controlID).DoMouseMoved(localPoint,AKeyWrapper::GetEventKeyModifiers()) == true )
			{
				handled = true;
			}
			break;
		}
	}
	return handled;
}

/**
ウインドウ内に、Tracking中のUserPaneがあれば、DoMouseLeft()する
*/
void	CWindowImp::TryMouseLeaveForAllUserPanes()
{
	for( AIndex i = 0; i < mUserPaneArray_ControlID.GetItemCount(); i++ )
	{
		AControlID	controlID = mUserPaneArray_ControlID.Get(i);
		if( GetUserPaneConst(controlID).IsMouseLeaveEventTracking() == true )
		{
			GetUserPane(controlID).DoMouseLeft();
		}
	}
}

/**
ウインドウ内に、Tracking中のUserPaneがあれば、DoMouseLeft()する（inExceptControlID以外）
*/
void	CWindowImp::TryMouseLeaveForAllUserPanesExceptFor( const AControlID inExceptControlID )
{
	//ウインドウ内の別のcontrolがTracking中なら、DoMouseLeft()する
	for( AIndex i = 0; i < mUserPaneArray_ControlID.GetItemCount(); i++ )
	{
		AControlID	controlID = mUserPaneArray_ControlID.Get(i);
		if( controlID != inExceptControlID )
		{
			if( GetUserPaneConst(controlID).IsMouseLeaveEventTracking() == true )
			{
				GetUserPane(controlID).DoMouseLeft();
			}
		}
	}
}

/**
他のウインドウの全てのUserPaneをDoMouseLeft()する（inExceptControlID以外）
*/
void	CWindowImp::TryMouseLeaveForAllWindowsExceptFor( const AControlID inExceptControlID )
{
	//自ウインドウのinExceptControlID以外のUserPaneをDoMouseLeft()
	TryMouseLeaveForAllUserPanesExceptFor(inExceptControlID);
	//他のウインドウの全てのUserPaneをDoMouseLeft()
	for( AIndex i = 0; i < sAliveWindowArray_HWnd.GetItemCount(); i++ )
	{
		HWND	hWnd = sAliveWindowArray_HWnd.Get(i);
		if( hWnd != mHWnd )
		{
			(sAliveWindowArray.Get(i))->TryMouseLeaveForAllUserPanes();
		}
	}
}

//
void	CWindowImp::DoOwnerDraw( const AControlID inID, const LPDRAWITEMSTRUCT drawitem )
{
	HWND	hwnd = GetHWndByControlID(inID);
	HDC	hdc = ::GetDC(hwnd);
	if( hdc == NULL )   {_ACError("",NULL);return;}
	
	//test
	HBRUSH	hbr = ::CreateSolidBrush(kColor_Gray70Percent);
	HBRUSH	hbr2 = ::CreateSolidBrush(kColor_Gray92Percent);
	if( hbr == NULL )   {_ACError("",NULL);return;}
	RECT	rect;
	::GetClientRect(hwnd,&rect);
	::FillRect(hdc,&rect,hbr2);
	::FrameRect(hdc,&rect,hbr);
	::DeleteObject(hbr);
	::DeleteObject(hbr2);
	
	::ReleaseDC(hwnd,hdc);
}

/**
Bindings適用
*/
void	CWindowImp::ApplyBindings( const AGlobalRect& prev, const AGlobalRect& current )
{
	//
	ANumber	prevwidth = prev.right-prev.left;
	ANumber	prevheight = prev.bottom-prev.top;
	ANumber	curwidth = current.right-current.left;
	ANumber	curheight = current.bottom-current.top;
	for( AIndex i = 0; i < mBindings_ControlID.GetItemCount(); i++ )
	{
		AControlID	controlID = mBindings_ControlID.Get(i);
		ABool	left 		= mBindings_Left.Get(i);
		ABool	right 		= mBindings_Right.Get(i);
		ABool	top 		= mBindings_Top.Get(i);
		ABool	bottom 		= mBindings_Bottom.Get(i);
		AWindowPoint	pt;
		GetControlPosition(controlID,pt);
		ANumber	w = GetControlWidth(controlID);
		ANumber	h = GetControlHeight(controlID);
		HWND	parent = ::GetParent(GetHWndByControlID(controlID));
		if( parent == mHWnd )
		{
			//親コントロールがウインドウの場合
			
			//X方向
			if( prevwidth != curwidth )
			{
				if( left == false && right == true )
				{
					pt.x += (curwidth-prevwidth);
					SetControlPosition(controlID,pt);
				}
				else if( left == true && right == true )
				{
					w += (curwidth-prevwidth);
					SetControlSize(controlID,w,h);
				}
			}
			//Y方向
			if( prevheight != curheight )
			{
				if( top == false && bottom == true )
				{
					pt.y += (curheight-prevheight);
					SetControlPosition(controlID,pt);
				}
				else if( top == true && bottom == true )
				{
					h += (curheight-prevheight);
					SetControlSize(controlID,w,h);
				}
			}
		}
		else
		{
			//親コントロールがコントロールの場合
			
			//親を検索
			AIndex	pi = mControlArray_HWnd.Find(parent);
			if( pi != kIndex_Invalid )
			{
				//親のBindingsを取得
				AControlID	parentControlID = mControlArray_ID.Get(pi);
				AIndex	parentBindingsIndex = mBindings_ControlID.Find(parentControlID);
				if( parentBindingsIndex != kIndex_Invalid )
				{
					//親のBindingsに関連して動作する。
					//親のサイズ変更が発生する場合のみ、子も移動／サイズ変更が発生する
					ABool	pleft 		= mBindings_Left.Get(parentBindingsIndex);
					ABool	pright 		= mBindings_Right.Get(parentBindingsIndex);
					ABool	ptop 		= mBindings_Top.Get(parentBindingsIndex);
					ABool	pbottom		= mBindings_Bottom.Get(parentBindingsIndex);
					if( prevwidth != curwidth )
					{
						if( pleft == true && pright == true )
						{
							if( left == false && right == true )
							{
								pt.x += (curwidth-prevwidth);
								SetControlPosition(controlID,pt);
							}
							else if( left == true && right == true )
							{
								w += (curwidth-prevwidth);
								SetControlSize(controlID,w,h);
							}
						}
					}
					if( prevheight != curheight )
					{
						if( ptop == true && pbottom == true )
						{
							if( top == false && bottom == true )
							{
								pt.y += (curheight-prevheight);
								SetControlPosition(controlID,pt);
							}
							else if( top == true && bottom == true )
							{
								h += (curheight-prevheight);
								SetControlSize(controlID,w,h);
							}
						}
					}
				}
			}
		}
	}
}

#pragma mark ===========================

#pragma mark --- APIコールバック(static)

LRESULT CALLBACK CWindowImp::STATIC_APICB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-PRC)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return 0;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	
	//hWndに対応するCWindowImpオブジェクトを検索
	AIndex	index = sAliveWindowArray_HWnd.Find(hWnd);
	if( index == kIndex_Invalid )
	{
		//==================================================
		//ルートウインドウではないウインドウ（タブ内"dialogs"等）用
		//==================================================
		switch(message)
		{
		  case WM_APP_INTERNALEVENT:
			{
				ABool	updateMenu = false;//#698
				AApplication::GetApplication().EVT_DoInternalEvent(updateMenu);//#698
				if( updateMenu == true )//#698
				{
					AApplication::GetApplication().NVI_UpdateMenu();
				}
				break;
			}
		  case WM_COMMAND:
			{
				if( lParam != 0 )
				{
					//コントロールメッセージをルートウインドウへ送信
					::PostMessage(::GetAncestor(hWnd,GA_ROOT), message, wParam, lParam);
					return 0L;
				}
				break;
			}
		  case WM_HSCROLL:
			{
				::PostMessage(::GetAncestor(hWnd,GA_ROOT), message, wParam, lParam);
				return 0L;
				break;
			}
		  case WM_VSCROLL:
			{
				::PostMessage(::GetAncestor(hWnd,GA_ROOT), message, wParam, lParam);
				return 0L;
				break;
			}
			//起動中にショートカット等へのファイルドロップ（WinMain()からMappedFileにファイルパスが書き込まれる）
		  case WM_APP_DROPFROMANOTHER:
			{
				HANDLE	hmap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,kIdForDropFromAnother);
				if( hmap == NULL )   {_ACError("",NULL);break;}
				wchar_t*	ptr = (wchar_t*)MapViewOfFile(hmap,FILE_MAP_ALL_ACCESS,0,0,0);
				if( ptr == NULL )   {::CloseHandle(hmap);_ACError("",NULL);break;}
				AText	cmdline;
				cmdline.SetFromWcharString(ptr,4096);//miedit.cppで指定したマックスサイズ
				::CloseHandle(hmap);
				ATextArray	argArray;//#263
				CAppImp::GetCmdLineArgTextArray(cmdline,argArray);//#263
				AApplication::GetApplication().NVI_ExecCommandLineArgs(argArray);//#263
				::SetForegroundWindow(::GetActiveWindow());
				break;
			}
		  default:
			{
				break;
			}
		}
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	//インスタンスのコールバックメソッドを呼び出す
	LRESULT	result = 0;
	try
	{
		if( (sAliveWindowArray.Get(index))->APICB_WndProc(message,wParam,lParam,result) == false )
		{
			result = ::DefWindowProc(hWnd, message, wParam, lParam);
		}
		else
		{
			switch(message)
			{
			  case WM_MOVING:
			  case WM_SIZING:
				{
					result = TRUE;
					break;
				}
			  default:
				{
					result = 0;
				}
			}
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_WndProc() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-PRC]");
	return result;
}

/**
Overlay背景ウインドウ用Windowプロシージャ
*/
LRESULT CALLBACK CWindowImp::STATIC_APICB_WndProcOverlayBackground( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-PRC)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return 0;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	
	//
	ABool	handled = false;
	LRESULT	result = 0;
	try
	{
		switch(message)
		{
			//マウスイベントを本Overlayウインドウへリダイレクトする
		  case WM_MOUSEWHEEL://MouseWheelはSetFocus()しないとイベントキャッチできない
		  case WM_LBUTTONDOWN:
		  case WM_RBUTTONDOWN:
		  case WM_LBUTTONDBLCLK:
		  case WM_RBUTTONDBLCLK:
		  case WM_LBUTTONUP:
		  case WM_RBUTTONUP:
		  case WM_MOUSEMOVE:
		  case WM_MOUSELEAVE:
			{
				AIndex	index = sAliveWindowArray_HWnd_OverlayBackgroundWindow.Find(hWnd);
				if( index != kIndex_Invalid )
				{
					(sAliveWindowArray.Get(index))->APICB_WndProc(message,wParam,lParam,result);
					handled = true;
				}
				break;
			}
		  case WM_PAINT:
			{
				AIndex	index = sAliveWindowArray_HWnd_OverlayBackgroundWindow.Find(hWnd);
				if( index != kIndex_Invalid )
				{
					//背景描画（背景色塗りつぶし）
					//UpdateOverlayBackgroundWindow()でも背景描画しているが、
					//ここでも消去しないと、ときどきOverlayの背景色が不定になる現象が発生する
					AColor	color = (sAliveWindowArray.Get(index))->mOverlayBackgroundColor;
					RECT	rect;
					::GetWindowRect(hWnd,&rect);
					PAINTSTRUCT ps;
					HDC	hdc = ::BeginPaint(hWnd,&ps);
					if( hdc == NULL )   {_ACError("",NULL);break;}
					HBRUSH	hbr = ::CreateSolidBrush(color);
					if( hbr == NULL )   {_ACError("",NULL);break;}
					RECT	drect;
					drect.left		= 0;
					drect.top		= 0;
					drect.right		= rect.right-rect.left;
					drect.bottom	= rect.bottom-rect.top;
					::FillRect(hdc,&drect,hbr);
					::DeleteObject(hbr);
					::EndPaint(hWnd,&ps);
					handled = true;
				}
				break;
			}
			/*
		  case WM_NCACTIVATE:
			{
				if((BOOL)wParam == TRUE)
				{
					AIndex	index = sAliveWindowArray_HWnd_OverlayBackgroundWindow.Find(hWnd);
					if( index != kIndex_Invalid )
					{
						::SetActiveWindow((sAliveWindowArray.Get(index))->mParentWindowRef);
					}
					return FALSE;
				}
				break;
			}
			*/
		}
	}
	catch(...)
	{
		_ACError("CWindowImp::STATIC_APICB_WndProcOverlayBackground() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-PRC]");
	if( handled == true )
	{
		return 0;
	}
	else
	{
		//DefWindowProc()がないとウインドウ生成にも失敗する
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
}
	
//参考：Petzold section 17.3 EZFONT.c
//関数コール側でHFONT解放必要
HFONT	CWindowImp::CreateFont_( const HDC inHDC, const AText& inFontName, const AFloatNumber inFontSize,
		ATextStyle inStyle, const ABool inAntiAlias )
{
	::SaveDC(inHDC);
	
	::SetGraphicsMode(inHDC,GM_ADVANCED);
	::ModifyWorldTransform(inHDC,NULL,MWT_IDENTITY);
	::SetViewportOrgEx(inHDC,0,0,NULL);
	::SetWindowOrgEx(inHDC,0,0,NULL);
	
	int	decipoint = inFontSize*10;
	FLOAT	cyDpi = (FLOAT)GetDeviceCaps(inHDC,LOGPIXELSY);
	POINT	pt;
	pt.x = 0;
	pt.y = (int)(decipoint*cyDpi/72);
	DPtoLP(inHDC,&pt,1);
	//★VERTRES, VERTSIZEから計算する方法とどちらが正しい？
	
	FLOAT	h = pt.y;
	h /= 10.0;
	h += 0.5;
	LOGFONT	lf;
	lf.lfHeight			= -(int)(h);;
	lf.lfWidth			= 0;
	lf.lfEscapement		= 0;
	lf.lfOrientation	= 0;
	lf.lfWeight			= 0;
	if( (inStyle & kTextStyle_Bold) != 0 )
	{
		lf.lfWeight = FW_BOLD;
	}
	lf.lfItalic			= 0;
	if( (inStyle & kTextStyle_Italic) != 0 )
	{
		lf.lfItalic = TRUE;
	}
	lf.lfUnderline		= 0;
	if( (inStyle & kTextStyle_Underline) != 0 )
	{
		lf.lfUnderline = TRUE;
	}
	lf.lfStrikeOut		= 0;
	lf.lfCharSet		= DEFAULT_CHARSET;//★暫定 この値は未定義文字を表示するのに使われる？
	lf.lfOutPrecision	= 0;
	lf.lfClipPrecision	= 0;
	if( inAntiAlias == true )
	{
		lf.lfQuality		= PROOF_QUALITY;
		//Windows7での検証結果
		//ANTIALIASED_QUALITY: 文字が小さいとつぶれて汚くなる
		//PROOF_QUALITY: 一番きれい
		//CLEARTYPE_QUALITY: PROOF_QUALITYと同じか？
	}
	else
	{
		lf.lfQuality		= NONANTIALIASED_QUALITY;
	}
	lf.lfPitchAndFamily	= 0;//ここをFIXED_PITCHにしてもプロポーショナルを等幅にすることはできなかった
	{
		//#375
		AText	fontname(inFontName);
		if( fontname.Compare("default") == true )
		{
			AFontWrapper::GetAppDefaultFontName(fontname);
		}
		//
		AStCreateWcharStringFromAText	fontnamecstr(fontname);
		::StringCchCopyW(lf.lfFaceName,LF_FACESIZE,fontnamecstr.GetPtr());
	}
	HFONT	font = ::CreateFontIndirect(&lf);
	::RestoreDC(inHDC,-1);
	return font;
}


#pragma mark ===========================

#pragma mark ---ユーティリティ

#pragma mark ===========================

#pragma mark ---画面サイズ取得

/**
メインデバイスのスクリーン領域を取得
*/
void	CWindowImp::GetMainDeviceScreenBounds( AGlobalRect& outBounds )
{
	outBounds.left 		= 0;
	outBounds.top		= 0;
	outBounds.right		= ::GetSystemMetrics(SM_CXSCREEN);
	outBounds.bottom	= ::GetSystemMetrics(SM_CYSCREEN);
}

/**
ウインドウ配置可能な領域を取得（Dock, Menubar等に重ならない領域）
*/
void	CWindowImp::GetAvailableWindowPositioningBounds( AGlobalRect& outBounds )
{
	RECT	rect;
	::SystemParametersInfoW(SPI_GETWORKAREA,0,&rect,0);
	outBounds.left 		= rect.left;
	outBounds.top		= rect.top;
	outBounds.right		= rect.right;
	outBounds.bottom	= rect.bottom;
	
	//★menubarに重ならないように修正必要
}

#pragma mark ===========================

#pragma mark ---ウインドウ取得

/**
最前面ウインドウ取得（kWindowClass_Documentウインドウの中から検索）
※GetForegroundWindow()はWindows全体の最前面を取得するのでアプリ内最前面取得には使えない
*/
AWindowRef	CWindowImp::GetMostFrontWindow()
{
	//documentクラスウインドウを１つ見つける
	HWND	hwnd = NULL;
	for( AIndex i = 0; i < sAliveWindowArray.GetItemCount(); i++ )
	{
		if( (sAliveWindowArray.Get(i))->mWindowClass == kWindowClass_Document )
		{
			hwnd = sAliveWindowArray_HWnd.Get(i);
			break;
		}
		
	}
	//なければNULLを返す
	if( hwnd == NULL )   return NULL;
	
	//同じtop-levelのウインドウの最前面ウインドウを見つける
	hwnd = ::GetWindow(hwnd,GW_HWNDFIRST);
	//kWindowClass_Documentのウインドウの中で最前面のものを検索
	while( hwnd != NULL )
	{
		//kWindowClass_Documentのウインドウだったらそれを返す
		if( ::IsWindowVisible(hwnd) == TRUE )
		{
			AIndex	index = sAliveWindowArray_HWnd.Find(hwnd);
			if( index != kIndex_Invalid )
			{
				if( (sAliveWindowArray.Get(index))->mWindowClass == kWindowClass_Document )
				{
					return hwnd;
				}
			}
		}
		//次のz-orderのウインドウを取得
		hwnd = ::GetNextWindow(hwnd,GW_HWNDNEXT);//GetWindow()/GetNextWindow()にリスクがあるのはループ内にウインドウ制御がある場合だと思われるので、これは問題ないでしょう
	}
	return NULL;
}

/**
次のウインドウ取得
*/
AWindowRef	CWindowImp::GetNextOrderWindow( const AWindowRef inWindowRef )
{
	return ::GetNextWindow(inWindowRef,GW_HWNDNEXT);
}

/**
N番目のウインドウ取得
*/
AWindowRef	CWindowImp::GetNthWindow( const ANumber inNumber )
{
	if( inNumber <= 0 )   return NULL;
	ANumber	num = 0;
	for( AWindowRef windowRef = GetMostFrontWindow(); windowRef != NULL; 
				windowRef = GetNextOrderWindow(windowRef) )
	{
		num++;
		if( num == inNumber )   return windowRef;
	}
	return NULL;
}

