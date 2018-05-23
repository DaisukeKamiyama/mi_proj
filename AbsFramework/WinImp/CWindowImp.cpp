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
//#353 ABaseTypes.h�ɂĒ�`����悤�ɕύX const AControlID	kControlID_TabControl = 99999;

extern const wchar_t*	kIdForDropFromAnother;

AArray<CWindowImp*>	CWindowImp::sAliveWindowArray;
AArray<HWND>		CWindowImp::sAliveWindowArray_HWnd;
AArray<HWND>		CWindowImp::sAliveWindowArray_HWnd_OverlayBackgroundWindow;
//AArray<HWND>		CWindowImp::sFloatingWindowArray;

#pragma mark ===========================
#pragma mark [�N���X]CWindowImp
#pragma mark ===========================

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^

//�R���X�g���N�^
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

//�f�X�g���N�^
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

#pragma mark --- AWindow���擾
//AWindow�擾
AWindow&	CWindowImp::GetAWin()
{
	return mAWin;
}
//AWindow�擾(const)
AWindow&	CWindowImp::GetAWinConst() const
{
	return mAWin;
}

#pragma mark ===========================

#pragma mark --- �E�C���h�E����

//�E�C���h�E����
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
					WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_TOPMOST,//�t���X�N���[���Ń��C���E�C���h�E������ɂ��邽�߂�WS_EX_TOPMOST���ݒ�
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
					L"dialogs",L"",/*�����ݒ肷��ƃ��C���E�C���h�E��deact����Ă��܂�WS_VISIBLE|*/WS_POPUP,
					CW_USEDEFAULT,0,CW_USEDEFAULT,0,inParentWindowRef,NULL,CAppImp::GetHInstance(),NULL);
			//���ݒ�
			//�����F�L�[�J���[��mOverlayBackgroundColor�Őݒ�
			::SetLayeredWindowAttributes(mHWnd,mOverlayBackgroundColor,0,/*LWA_ALPHA|*/LWA_COLORKEY);
			//
			mNoActivate = true;
			break;
		}
	  case kWindowClass_Overlay:
		{
			/*
			Overlay�E�C���h�E
			�E�ʃE�C���h�E���̃y�C���̂悤�Ɍ�����E�C���h�E�B���ۂɂ͕ʃE�C���h�E�B�i�Ɨ�����AWindow�œ��삷��j
			
			�E�w�i�E�C���h�E(mHWndOverlayBackgroundWindow)�ƁA�{�E�C���h�E(HWND)���쐬����
			�w�i�E�C���h�E�ɂ͔w�i�F�𔼓����ɓh��Ԃ�
			�{�E�C���h�E�ɂ͕s�����ŕ`����s��
			
			*/
			//�{�E�C���h�E
			//WS_EX_LAYERED: ���C���[�h�E�C���h�E�𐶐�
			//WS_EX_TRANSPARENT: �}�E�X�C�x���g��w�i�E�C���h�E�֑S�ē��߂���i��ɔw�i�E�C���h�E����{�E�C���h�E�փ��_�C���N�g������j
			// ��MOUSELEAVE�C�x���g����ɔw�i�E�C���h�E�̕��Ŏ�M����悤�ɂ���B���߂��Ȃ��ƁA�����łȂ������Ɠ���������leave�̂��������ς��̂ł������
			//WS_VISIBLE|WS_POPUP: ������Ԃ�show�AWS_POPUP: �E�C���h�E�g�Ȃ�
			mHWnd = ::CreateWindowExW(
					WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW,
					L"dialogs",L"",/*�����ݒ肷��ƃ��C���E�C���h�E��deact����Ă��܂�WS_VISIBLE|*/WS_POPUP,
					CW_USEDEFAULT,0,CW_USEDEFAULT,0,inParentWindowRef,NULL,CAppImp::GetHInstance(),NULL);
			//���ݒ�
			//�{�E�C���h�E�̃A���t�@�͂Ȃ�
			//�����F�L�[�J���[��mOverlayBackgroundColor��ݒ�
			//#379 ����color key�𗼕��g�p����悤�ɂ��� ::SetLayeredWindowAttributes(mHWnd,mOverlayBackgroundColor,0,/*LWA_ALPHA|*/LWA_COLORKEY);
			::SetLayeredWindowAttributes(mHWnd,mOverlayBackgroundColor,mAlpha,LWA_ALPHA|LWA_COLORKEY);
			
			//�w�i�E�C���h�E
			mHWndOverlayBackgroundWindow = ::CreateWindowExW(
					WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW,
					L"overlaybackground",L"",/*�����ݒ肷��ƃ��C���E�C���h�E��deact����Ă��܂�WS_VISIBLE|*/WS_POPUP,
					CW_USEDEFAULT,0,CW_USEDEFAULT,0,inParentWindowRef,NULL,CAppImp::GetHInstance(),NULL);
			//���ݒ�
			//#379 ��=0�ɐݒ� ::SetLayeredWindowAttributes(mHWndOverlayBackgroundWindow,0,mAlpha,LWA_ALPHA);
			::SetLayeredWindowAttributes(mHWndOverlayBackgroundWindow,0,0,LWA_ALPHA);
			
			//mParentWindowRef��ۑ�
			mParentWindowRef = inParentWindowRef;
			//�e�E�C���h�E�֓o�^
			if( mParentWindowRef != NULL )
			{
				AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mParentWindowRef);
				if( parentWindowIndex == kIndex_Invalid )   { _ACError("",this); }
				else
				{
					//�e�E�C���h�E�֓o�^
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
	
	//�t�@�C��Drop����
	::DragAcceptFiles(mHWnd,TRUE);
	
	//
	GetWindowBounds(mCurrentWindowBounds);
	mUnzoomedBounds = mCurrentWindowBounds;
	mCurrentWindowSize_Width = mCurrentWindowBounds.right - mCurrentWindowBounds.left;
	mCurrentWindowSize_Height = mCurrentWindowBounds.bottom - mCurrentWindowBounds.top;
	//�w�iWindow�X�V
	UpdateOverlayBackgroundWindow();
	
	//#404 �ʏ��ԃE�C���h�E�T�C�Y�L��
	GetWindowBounds(mWindowBoundsInStandardState);
	//#404 �t���X�N���[����ԉ���
	mFullScreenMode = false;
} 

void	CWindowImp::Create( AConstCharPtr inWindowName, const AWindowClass inWindowClass,
		const ABool inTabWindow, const AItemCount inTabCount, const ABool inCocoa )
{
	mWindowClass = inWindowClass;//���b�� kWindowClass_Document�ȊO�͂܂Ƃ��ɓ��삵�Ȃ�
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
	
	//�^�u
	if( inTabWindow == true )
	{
		//ID=65534�Ƀ^�u�R���g���[���̈ʒu���������߂̃_�~�[�R���g���[����u���Ă���
		HWND	dummycontrol = ::GetDlgItem(mHWnd,kControlID_TabControl);
		RECT	bounds;
		::GetWindowRect(dummycontrol,&bounds);
		HWND	parent = ::GetParent(dummycontrol);
		ARect	parentbounds;
		GetWindowBounds(parentbounds);
		//�_�~�[�R���g���[���폜
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
	
	//�R���{�{�b�N�X�֌Œ蕶����ݒ�
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
	
	//�t�@�C��Drop����
	::DragAcceptFiles(mHWnd,TRUE);
	
	//
	mNoActivate = false;
	
	//#404 �ʏ��ԃE�C���h�E�T�C�Y�L��
	GetWindowBounds(mWindowBoundsInStandardState);
	//#404 �t���X�N���[����ԉ���
	mFullScreenMode = false;
}

BOOL CALLBACK CWindowImp::STATIC_APICB_EnumChildProc( HWND hwnd, LPARAM lParam )
{
	AIndex	index = sAliveWindowArray_HWnd.Find((HWND)lParam);
	if( index == kIndex_Invalid )  {_ACError("",NULL);return FALSE;}
	//�o�^
	(sAliveWindowArray.Get(index))->RegisterControl(hwnd);
	return TRUE;
}

void	CWindowImp::RegisterControl( const HWND hwnd )
{
	AControlID	controlID = ::GetWindowLong(hwnd,GWL_ID);
	//�e�E�C���h�E�̃N���X��main, dialogs�ł͂Ȃ��ꍇ��controlID��0�Ƃ��ēo�^
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
	//�R���g���[���̃N���X�擾
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
�R���g���[���̃T�u�N���X
*/
LRESULT	CWindowImp::APICB_ControlSubclassProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	//
	AIndex	index = mControlArray_HWnd.Find(hwnd);
	if( index == kIndex_Invalid )  { _ACError("",this); return 0L; }
	WNDPROC	originalProc = mControlArray_OriginalProc.Get(index);//���L�������s�ɂ��mControlArray_OriginalProc���폜�����\�������邽�ߐ�Ɏ擾
	//ControlID�擾�@�Ȃ��AWinAPI�����o�^�R���g���[����controlID��0�ɂȂ�iSTATIC_APICB_EnumChildProc���Q�Ɓj
	AControlID	controlID = mControlArray_ID.Get(index);
	//���݂̃t�H�[�J�X�ʒu�ƈႤ�R���g���[����focus�����ꍇ�́A�t�H�[�J�X�R���g���[����ValueChanged��ʒm
	if( message == WM_SETFOCUS )
	{
		AControlID	oldFocus = GetCurrentFocus();
		if( oldFocus != kControlID_Invalid && oldFocus != controlID )
		{
			GetAWin().EVT_ValueChanged(oldFocus);
		}
	}
	//WindowModal�̐e�E�C���h�E�̏ꍇ�A�}�E�X�N���b�N�𖳌��ɂ���
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
				//�L�[����
				//Tab��WM_KEYDOWN�ŏ������悤�Ƃ���ƁAsysbeep���邱�Ƃ�����iWM_CHAR�œ��͕s�Ƃ��Đ�ɔ��f����Ă���H�j
			  case WM_CHAR:
				{
					switch(wParam)
					{
						//���^�[���L�[
					  case kUChar_CR:
						{
							if( mControlArray_CatchReturn.Get(index) == false )
							{
								//
								if( mCurrentFocus != kControlID_Invalid )
								{
									//�R���g���[���ύX��ʒm
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
						//�^�u�L�[
					  case kUChar_Tab:
						{
							if( mControlArray_CatchTab.Get(index) == false )
							{
								//
								if( mCurrentFocus != kControlID_Invalid )
								{
									//�R���g���[���ύX��ʒm
									GetAWin().EVT_ValueChanged(mCurrentFocus);
									//
									//SetSelectionAll(mCurrentFocus);
								}
								//�t�H�[�J�X�ړ�
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
						//ESC�L�[
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
						//controlID���o�^�̏ꍇ�͐e�������Ă݂�
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
								//�^�u�L�[
							  case kUChar_Tab:
								{
									if( mControlArray_CatchTab.Get(index) == false )
									{
										//�t�H�[�J�X�ړ�
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
					//�t�H�[�J�X�j���`��
					//���WindowProc�����s���Ă���A�Ǝ��ɔj����`�悷��
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
										//�`�F�b�N�{�b�N�X�E���W�I�{�^���̏ꍇ�F�����_���ň͂�
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
										//��L�ȊO�̃{�^���̏ꍇ�F������_���ň͂�
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
										//���X�g�{�b�N�X�̏ꍇ�F������_���ň͂�
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
										//�^�u�R���g���[���̏ꍇ
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
				//�t�H�[�J�X�ݒ�
			  case WM_SETFOCUS:
				{
					GetAWin().NVI_SwitchFocusTo(controlID);
					break;
				}
				//�L�[����
			  case WM_KEYDOWN:
				{
					switch(wParam)
					{
						//�^�u�L�[
					  case VK_TAB:
						{
							//�t�H�[�J�X�ړ�
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
						//�J�[�\���L�[�ɂ��t�H�[�J�X�ړ�
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
						//�f�t�H���gCancel�{�^��
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
						//�f�t�H���gOK�{�^��
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
				//�}�E�X�ړ�
			  case WM_MOUSEMOVE:
				{
					ACursorWrapper::ShowCursor();
					//�T�C�Y�{�b�N�X�̃}�E�X�J�[�\���ݒ�
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
	//�C�x���g�g�����U�N�V�����O�����^�㏈�� #0 �����ɂ��ꂪ�Ȃ��ƁARefresh���o�R��OS����C�x���gCB���������Ƃ��ɁA������sEventDepth==1�ƂȂ��Ă��܂��A
	//Window���̃I�u�W�F�N�g�ɑ΂���DoObjectArrayItemTrashDelete()���������A�����o�[�ϐ��ւ̃������A�N�Z�X���������A�����Ă��܂��B
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
Window��Drawer�ɐݒ�
@param inParent �e�E�C���h�E
*/
void	CWindowImp::SetWindowStyleToDrawer( CWindowImp& inParent )
{
	//���b�� Windows��drawer�Ȃ�
}

/**
Window��Sheet�ɐݒ�
@param inParent �e�E�C���h�E
*/
void	CWindowImp::SetWindowStyleToSheet( CWindowImp& inParent )
{
	//TOPMOST�ɂ���
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
	//WindowModal�̐e�E�C���h�E��WindowModalParentMode��ݒ�
	mWindowModalParentWindowRef = parent;
	inParent.SetWindowModalParentMode(true);
}

//
void	CWindowImp::Show( const int nCmdShow )
{
	int	cmdShow = nCmdShow;
	//��\������Zoom()�R�[������Ă�����ASW_SHOW��SW_SHOWMAXIMIZED���ɒu�������ĕ\��
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
	//�E�C���h�E���A�N�e�B�u�ɂ��Ȃ��Ȃ�ASW_SHOW��SW_SHOWNA�ɒu�������ĕ\��
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
	//�E�C���h�E��\��
	::ShowWindow(mHWnd,cmdShow);
	//
	DWORD err = ::GetLastError();
	::UpdateWindow(mHWnd);
	//�t�H�[�J�X�𕜌�
	if( mLatentFocus != kControlID_Invalid )
	{
		SetFocus(mLatentFocus);
		mLatentFocus = kControlID_Invalid;
	}
	
	//WindowModal�̐e�E�C���h�E��WindowModalParentMode��ݒ�
	if( mWindowModalParentWindowRef != NULL )
	{
		AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mWindowModalParentWindowRef);
		if( parentWindowIndex == kIndex_Invalid )   { _ACError("",this); }
		else
		{
			//�e�E�C���h�E���[�_����ԉ���
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
	
	//WindowModal�̐e�E�C���h�E��WindowModalParentMode������
	if( mWindowModalParentWindowRef != NULL )
	{
		AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mWindowModalParentWindowRef);
		if( parentWindowIndex == kIndex_Invalid )   { _ACError("",this); }
		else
		{
			//�e�E�C���h�E���[�_����ԉ���
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
	//::SetWindowPos(mHWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);���ǂ����������H
	::SetActiveWindow(mHWnd);
}

//
void	CWindowImp::SendBehind( CWindowImp& inBeforeWindow )
{
	//mHWnd�����̃E�C���h�E�������ɂ������Ƃ�����΍�Ƃ��āAinBeforeWindow�̂ق����őO�ʂɎ����Ă��� #379
	::SetWindowPos(inBeforeWindow.GetHWnd(),HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	//
	::SetWindowPos(mHWnd,inBeforeWindow.GetHWnd(),0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

//#567
/**
�E�C���h�E���Ŕw�ʂɈړ�
*/
void	CWindowImp::SendBehindAll()
{
	::SetWindowPos(mHWnd,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

/*�g��Ȃ����A�g���Ȃ�kWindowClass���`�������B
//
void	CWindowImp::SetTopMost()
{
	::SetWindowPos(mHWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}
*/

//
void	CWindowImp::CloseWindow()
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	/*
	//
	AIndex	floatingindex = sFloatingWindowArray.Find(mHWnd);
	if( floatingindex != kIndex_Invalid )
	{
		sFloatingWindowArray.Delete1(floatingindex);
	}
	*/
	
	//child window�N���[�Y
	//#291�ɂ��q�E�C���h�E�@�\�폜
	
	//TextArrayMenu�o�^����
	while( mTextArrayMenuArray_ControlID.GetItemCount() > 0 )
	{
		UnregisterTextArrayMenu(mTextArrayMenuArray_ControlID.Get(0));
	}
	
	//�t�H�[�J�X�O���[�v�폜
	//#135 mFocusGroup.DeleteAll();
	
	//�t�H�[�J�X�ݒ����
	mLatentFocus = kControlID_Invalid;
	mCurrentFocus = kControlID_Invalid;
	
	//radiogroup�폜
	//#349 mRadioGroup.DeleteAll();
	
	//�R���g���[���폜
	while(mControlArray_ID.GetItemCount()>0)
	{
		DeleteControl(mControlArray_ID.Get(0));
	}
	
	//UserPane�폜
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
	
	//Overlay�E�C���h�E�̏ꍇ�A�e�E�C���h�E�̓o�^����������
	if( mWindowClass == kWindowClass_Overlay && mParentWindowRef != NULL )
	{
		AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mParentWindowRef);
		if( parentWindowIndex == kIndex_Invalid )   { _ACError("",this); }
		else
		{
			//�e�E�C���h�E�֓o�^
			(sAliveWindowArray.Get(parentWindowIndex))->UnregisterOverlayWindow(mHWnd);
		}
	}
	
	//
	mParentWindowRef = NULL;
	
	//WindowModal�̐e�E�C���h�E��WindowModalParentMode������
	if( mWindowModalParentWindowRef != NULL )
	{
		AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mWindowModalParentWindowRef);
		if( parentWindowIndex == kIndex_Invalid )   { _ACError("",this); }
		else
		{
			//�e�E�C���h�E���[�_����ԉ���
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane��Offscreen�̂����\�����̂��̂�S�čĕ`�悷��
	for( AIndex index = 0; index < mUserPaneArray.GetItemCount(); index++ )
	{
		if( mUserPaneArray.GetObjectConst(index).IsControlVisible() == true )
		{
			mUserPaneArray.GetObject(index).RefreshControl();
		}
	}
	//�E�C���h�E�S�̂ɑ΂���WM_PAINT�C�x���g�𔭐�������
	::InvalidateRect(mHWnd,NULL,FALSE);
}

/*#688
//
void	CWindowImp::RefreshWindowRect( const AWindowRect& inWindowRect )
{
	//�w��̈����UserPane��Offscreen�̂����\�����̂��̂�S�čĕ`�悷��
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
	//�w��̈�ɑ΂���WM_PAINT�C�x���g�𔭐�������
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
	//���b�� Windows�̈���̓��[�_���Ȃ̂ł��Ԃ�s�v�ifalse�Œ��OK�j
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
	wndclass.hbrBackground	= NULL;//NULL�ɂ��Ȃ��ƁA���T�C�Y�����ɁAOS�ɑS�̏�������A������B(HBRUSH)(COLOR_WINDOW+1);
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
�I�[�o�[���C�E�C���h�E��o�^����i�e���ŃR�[���j
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
�I�[�o�[���C�E�C���h�E�o�^����������i�e���ŃR�[���j
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
�����i�I�[�o�[���C�E�C���h�E�j��������WindowGroup��MoveTogether������On/Off����i�q���ŃR�[���j
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
�����i�I�[�o�[���C�E�C���h�E�j��������WindowGroup��kWindowGroupAttrLayerTogether������On/Off����i�q���ŃR�[���j
*/
void	CWindowImp::SetOverlayLayerTogether( const ABool inLayerTogether )
{
	//���b�� �����炭�Ή��s�v
}

/**
Overlay�E�C���h�E�̔w�iWindow�`��
*/
void	CWindowImp::UpdateOverlayBackgroundWindow()
{
	//Overlay�E�C���h�E�p�w�iWindow�����݂���ꍇ�́A�����ʒu�E�T�C�Y�ɕύX���A�w�i�`��
	if( mHWndOverlayBackgroundWindow != NULL )
	{
		//�{Overlay�E�C���h�E��rect�擾
		RECT	rect;
		::GetWindowRect(mHWnd,&rect);
		//�{Overlay�E�C���h�E�̔w��E�����ʒu�E�T�C�Y�ɐݒ�
		::SetWindowPos(mHWndOverlayBackgroundWindow,mHWnd,
				rect.left,
				rect.top,
				rect.right-rect.left,
				rect.bottom-rect.top,
				SWP_NOACTIVATE);
		//�w�i�`��i�w�i�F�h��Ԃ��j
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
�E�C���h�E�𖾎��I�Ɂi�����j�X�V�iWM_PAINT��WndProc�����ڃR�[������A�E�C���h�E�����X�V�����j
*/
void	CWindowImp::UpdateWindow()
{
	::UpdateWindow(mHWnd);
}

//#404
/**
�t���X�N���[�����[�h
*/
void	CWindowImp::SetFullScreenMode( const ABool inFullScreenMode )
{
	if( inFullScreenMode == true && mFullScreenMode == false )
	{
		//�E�C���h�E�̈�L��
		GetWindowBounds(mWindowBoundsInStandardState);
		//�t���X�N���[���ݒ�
		//ChangeDisplaySettingsW(NULL,CDS_FULLSCREEN);
		::SetWindowPos(mHWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		//#476 �t���X�N���[�����ɃE�C���h�E�T�C�Y���L�����Ȃ��悤�ɐ�Ƀt���X�N���[�����[�h�ݒ�
		mFullScreenMode = inFullScreenMode;
		//�E�C���h�E�̈�ݒ�
		ARect	rect;
		rect.left = 0;
		rect.top = ::GetSystemMetrics(SM_CYMENU);
		rect.right = ::GetSystemMetrics(SM_CXSCREEN);
		rect.bottom = ::GetSystemMetrics(SM_CYSCREEN);
		SetWindowBounds(rect);
	}
	else if( inFullScreenMode == false && mFullScreenMode == true )
	{
		//�t���X�N���[������
		//ChangeDisplaySettingsW(NULL,0);
		::SetWindowPos(mHWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		//#476 �t���X�N���[�����ɃE�C���h�E�T�C�Y���L�����Ȃ��悤�ɐ�Ƀt���X�N���[�����[�h�ݒ�
		mFullScreenMode = inFullScreenMode;
		//�E�C���h�E�̈�����ɖ߂�
		SetWindowBounds(mWindowBoundsInStandardState);
	}
	//#476 mFullScreenMode = inFullScreenMode;
}

//#505
/**
�E�C���h�E�̍ŏ��T�C�Y�ݒ�
*/
void	CWindowImp::SetWindowMinimumSize( const ANumber inWidth, const ANumber inHeight )
{
	mWindowMinWidth = inWidth;
	mWindowMinHeight = inHeight;
}

#pragma mark ===========================

#pragma mark --- �E�C���h�E�����ݒ�

//
void	CWindowImp::SetTitle( const AText& inText )
{
	//�E�C���h�E�����ς݃`�F�b�N
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
	//Windows�ł͑Ή����鏈������
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
	//Windows�ł͑Ή����鏈������
}

//
void	CWindowImp::ClearWindowFile()
{
	//Windows�ł͑Ή����鏈������
}

/*
//
void	CWindowImp::SetFloating()
{
	sFloatingWindowArray.Add(mHWnd);
	//�E�C���h�E���A�N�e�B�u�ɂ��Ȃ�
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

#pragma mark --- �E�C���h�E���擾

//
ABool	CWindowImp::ExistControl( const AControlID inID ) const
{
	return (mControlArray_ID.Find(inID)!=kIndex_Invalid);
}

//
void	CWindowImp::GetTitle( AText& outText ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	//
	outText.DeleteAll();
	int	len = ::GetWindowTextLengthW(mHWnd)+1;
	outText.Reserve(0,len*sizeof(wchar_t));
	{
		AStTextPtr	textptr(outText,0,outText.GetItemCount());
		::GetWindowTextW(mHWnd,(LPWSTR)(textptr.GetPtr()),textptr.GetByteCount()/sizeof(wchar_t));
	}
	//�k�������ȍ~���폜
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
	return (::IsWindowVisible(mHWnd)==true)&&(::GetActiveWindow()==mHWnd);//���b��@�t���[�e�B���O�E�C���h�E�������K�v
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
	//���b�� ���L�ȊO�̃��[�_���Ή�
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
	//���b��
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

//WindowRef�擾
AWindowRef	CWindowImp::GetWindowRef() const
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACThrow("Window not created",this);return NULL;}
	
	return mHWnd;
}

#pragma mark ===========================

#pragma mark --- �E�C���h�E�T�C�Y�^�ʒuGet/Set

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
	//�w�iWindow�X�V
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
		//�T�C�Y�������ꍇ�iSetWindowPos���ŏ����œK�����邩������Ȃ����A�O�̂��߁j
		::SetWindowPos(inWindowRef,HWND_TOP,
				inRect.left	- (clientRect.left - bounds.left),
				inRect.top	- (clientRect.top - bounds.top),
				0,0,
				SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
	}
	else
	{
		//�T�C�Y�E�ʒu�Ƃ��Ⴄ�ꍇ
		::SetWindowPos(inWindowRef,HWND_TOP,
				inRect.left	- (clientRect.left - bounds.left),
				inRect.top	- (clientRect.top - bounds.top),
				inRect.right - inRect.left + (clientRect.left - bounds.left) + (bounds.right - clientRect.right),
				inRect.bottom - inRect.top + (clientRect.top - bounds.top) + (bounds.bottom - clientRect.bottom),
				SWP_NOZORDER|SWP_NOACTIVATE);
	}
	//#476 �t���X�N���[�����[�h�łȂ��Ƃ��̃E�C���h�E�T�C�Y���L��
	if( mFullScreenMode == false )
	{
		GetWindowBounds(mWindowBoundsInStandardState);
	}
}
void	CWindowImp::SetWindowBounds( const AGlobalRect& inRect )
{
	SetWindowBounds(mHWnd,inRect);
	//�w�iWindow�X�V
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
		//�T�C�Y�������ꍇ�iSetWindowPos���ŏ����œK�����邩������Ȃ����A�O�̂��߁j
		::SetWindowPos(inWindowRef,HWND_TOP,
				inRect.left	- (clientRect.left - bounds.left),
				inRect.top	- (clientRect.top - bounds.top),
				0,0,
				SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
	}
	else
	{
		//�T�C�Y�E�ʒu�Ƃ��Ⴄ�ꍇ
		::SetWindowPos(inWindowRef,HWND_TOP,
				inRect.left	- (clientRect.left - bounds.left),
				inRect.top	- (clientRect.top - bounds.top),
				inRect.right - inRect.left + (clientRect.left - bounds.left) + (bounds.right - clientRect.right),
				inRect.bottom - inRect.top + (clientRect.top - bounds.top) + (bounds.bottom - clientRect.bottom),
				SWP_NOZORDER|SWP_NOACTIVATE);
	}
	//�w�iWindow�X�V
	UpdateOverlayBackgroundWindow();
	//#476 �t���X�N���[�����[�h�łȂ��Ƃ��̃E�C���h�E�T�C�Y���L��
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

//�E�C���h�E�T�C�Y�ݒ�
void	CWindowImp::SetWindowSize( const ANumber inWidth, const ANumber inHeight )
{
	//�E�C���h�E�����ς݃`�F�b�N
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
	//�w�iWindow�X�V
	UpdateOverlayBackgroundWindow();
	//#476 �t���X�N���[�����[�h�łȂ��Ƃ��̃E�C���h�E�T�C�Y���L��
	if( mFullScreenMode == false )
	{
		GetWindowBounds(mWindowBoundsInStandardState);
	}
}

//
void	CWindowImp::ConstrainWindowPosition( const ABool inAllowPartial )
{
	//#404 �t���X�N���[�����͈ʒu�␳���Ȃ�
	if( mFullScreenMode == true )   return;
	
	int	cx = ::GetSystemMetrics(SM_CXSCREEN);
	int cy = ::GetSystemMetrics(SM_CYSCREEN);
	AGlobalRect	rect;
	GetWindowBounds(rect);
	if( rect.right < 0 || rect.bottom < 0 || rect.left > cx || rect.top > cy )
	{
		//�E�C���h�E���܂������X�N���[�����ɕ\������Ă��Ȃ��ꍇ�A�Œ�ʒu(50,50)�ɕ\������
		APoint	pt = {50,50};
		SetWindowPosition(pt);
	}
	//���b�� inAllowPartila���Ή� �i���allowPartial�j
}

//#385
/**
�E�C���h�E�T�C�Y����ʂɍ��킹�ĕ␳
*/
void	CWindowImp::ConstrainWindowSize()
{
	//#404 �t���X�N���[�����͈ʒu�␳���Ȃ�
	if( mFullScreenMode == true )   return;
	
	//��ʃT�C�Y�擾
	AGlobalRect	globalRect;
	CWindowImp::GetAvailableWindowPositioningBounds(globalRect);
	//
	RECT	winrect;
	::GetWindowRect(mHWnd,&winrect);
	//�E�C���h�E�T�C�Y����ʃT�C�Y���傫����΁A�T�C�Y�␳
	if( (winrect.bottom-winrect.top) > (globalRect.bottom-globalRect.top) )
	{
		//
		ARect	clientrect;
		GetWindowBounds(clientrect);
		//�T�C�Y�ݒ�
		ANumber	delta = (globalRect.bottom-globalRect.top) - (winrect.bottom-winrect.top);
		clientrect.bottom += delta;
		SetWindowBounds(clientrect);
		//����������܂�悤�ɁAallowPartial=false�ňʒu�␳
		ConstrainWindowPosition(false);
	}
}

//
ABool	CWindowImp::IsWindowCollapsed() const
{
	//���b��_ACError("not implemented",NULL);
	return false;
}

#pragma mark ===========================

#pragma mark --- �R���g���[������

//
void	CWindowImp::DrawControl( const AControlID inID )
{
	//UserPane��Offscreen���ĕ`�悷��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).RefreshControl();
	}
	//WM_PAINT�C�x���g�𔭐�������
	::InvalidateRect(GetHWndByControlID(inID),NULL,FALSE);
}

#pragma mark ===========================

#pragma mark --- �R���g���[�������ݒ�C���^�[�t�F�C�X

//
void	CWindowImp::SetEnableControl( const AControlID inID, const ABool inEnable )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetEnableControl(inEnable);
		UpdateFocus();
		DrawControl(inID);
		return;
	}
	//�ʏ�R���g���[���̏ꍇ
	if( inEnable == true )
	{
		::EnableWindow(GetHWndByControlID(inID),TRUE);
	}
	else
	{
		::EnableWindow(GetHWndByControlID(inID),FALSE);
		/*#349 ���W�I�O���[�v�̑S�Ẵ{�^����dissable�ɂ���Ƃ���On���ړ�����̂ł��̏����̓A�v���w�ŕK�v�ȂƂ��ɍs�����Ƃɂ���
		//���W�I�O���[�v�ɐݒ肳�ꂽ�{�^���̏ꍇ�AOn��disable�ɂ��ꂽ��A���̃{�^���̂ǂꂩ��On�ɂ���B 
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsControlEnabled();
	}
	//�ʏ�R���g���[���̏ꍇ
	return (::IsWindowEnabled(GetHWndByControlID(inID))==TRUE);
}

//
void	CWindowImp::SetShowControl( const AControlID inID, const ABool inShow )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetShowControl(inShow);
		return;
	}
	//�ʏ�R���g���[���̏ꍇ
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		return GetUserPaneConst(inID).IsControlVisible();
	}
	//�ʏ�R���g���[���̏ꍇ
	return (::IsWindowVisible(GetHWndByControlID(inID))==TRUE);
}

//
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

//
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inControlID) == true )
	{
		GetUserPane(inControlID).SetTextJustification(inJustification);
		return;
	}
	
	//�ʏ�R���g���[���̏ꍇ
	//��������
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
	//�g�b�v�E�C���h�E���W�n(AWindowPoint)����e�E�C���h�E�̍��W�n�֕ϊ�
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
	//�s�v�H���������ƁA���s���ɐe�E�C���h�E���A�N�e�B�x�[�g����邽�߃R�����g�A�E�g�B::SetWindowPos(htooltip, HWND_TOPMOST,0, 0, 0, 0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	RECT	rect;
	::GetClientRect(GetHWndByControlID(inID),&rect);
	AStCreateWcharStringFromAText	str(inText);
	TOOLINFOW	ti;
	ti.cbSize = sizeof(ti);// 0x501�ȏゾ��Windows2000�œ��삵�Ȃ��炵���H
	ti.hwnd		= GetHWndByControlID(inID);
	ti.rect		= rect;
	ti.hinst	= CAppImp::GetHInstance();
	ti.uFlags	= TTF_SUBCLASS;
	ti.uId		= inID;
	ti.lpszText	= (wchar_t*)str.GetPtr();//�L���X�g�FTTM_ADDTOOL�Ŏg���ꍇ�́AlpszText�ɒl���Z�b�g����ċA���Ă��邱�Ƃ͖����͂�
	::SendMessageW(htooltip,TTM_ADDTOOLW,0,(LPARAM)&ti);
	*/
}

//R0246
void	CWindowImp::HideHelpTag() const
{
	//���b�� ���Ή�
}

//R0240
void	CWindowImp::EnableHelpTagCallback( const AControlID inID, const ABool inShowOnlyWhenNarrow )
{
	/*
	TTN_NEEDTEXT�̕����ł́A�R���g���[�����Ń}�E�X�ړ������Ƃ��Ƀc�[���`�b�v���e���X�V�ł��Ȃ��̂ŁA
	Tracking Tooltip�����iTTF_TRACK�t���O��On�ɂ���j���g�����Ƃɂ���B
	HWND	htooltip = ::CreateWindowEx(0,TOOLTIPS_CLASS,NULL,TTS_ALWAYSTIP|TTS_NOPREFIX,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
			GetHWndByControlID(inID),NULL,CAppImp::GetHInstance(),NULL);
	RECT	rect;
	::GetClientRect(GetHWndByControlID(inID),&rect);
	TOOLINFOW	ti;
	ti.cbSize = sizeof(ti);// 0x501�ȏゾ��Windows2000�œ��삵�Ȃ��炵���H
	ti.hwnd		= GetHWndByControlID(inID);
	ti.rect		= rect;
	ti.hinst	= CAppImp::GetHInstance();
	ti.uFlags	= TTF_SUBCLASS|TTF_IDISHWND;//TTN_NEEDTEXT���g�����߂ɂ�TTF_IDISHWND�K�{���ۂ�
	ti.uId		= (UINT_PTR) GetHWndByControlID(inID);
	ti.lpszText	= LPSTR_TEXTCALLBACKW;
	::SendMessageW(htooltip,TTM_ADDTOOLW,0,(LPARAM)&ti);
	*/
	//UserPane���Őݒ�
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
	//�E�C���h�E�����ς݃`�F�b�N
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
Drop�\��Control��o�^����

DeleteControl()�œo�^���������
*/
void	CWindowImp::RegisterDropEnableControl( const AControlID inControlID )
{
	//���b��@���Ή�
}

//#291
/**
�R���g���[����Zorder��ݒ�
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
�R���g���[����FileDrop�\�ɐݒ�
*/
void	CWindowImp::EnableFileDrop( const AControlID inControlID )
{
	//���������iWin�ł͏��FileDrop�\�j
}

#pragma mark ===========================

#pragma mark --- �R���g���[�����擾

//
void	CWindowImp::GetControlPosition( const AControlID inID, AWindowPoint& outPoint ) const
{
	HWND	hwnd = GetHWndByControlID(inID);
	RECT	rect;
	::GetWindowRect(hwnd,&rect);//�X�N���[�����W
	
	//�{�E�C���h�E���̑��΍��W���擾
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
		::GetWindowRect(GetHWndByControlID(mUserPaneArray_ControlID.Get(i)),&rect);//�X�N���[�����W
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
	//�k�������ȍ~���폜
	outText.DeleteAfterUTF16NullChar();
	outText.ConvertToUTF8FromAPIUnicode();
}

#pragma mark ===========================

#pragma mark --- �R���g���[���l��Set/Get�C���^�[�t�F�C�X

//
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
	//
	outText.DeleteAll();
	int	len = ::GetWindowTextLengthW(GetHWndByControlID(inID))+1;
	outText.Reserve(0,len*sizeof(wchar_t));
	{
		AStTextPtr	textptr(outText,0,outText.GetItemCount());
		::GetWindowTextW(GetHWndByControlID(inID),(LPWSTR)(textptr.GetPtr()),textptr.GetByteCount()/sizeof(wchar_t));
	}
	//�k�������ȍ~���폜
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetText(inText);
		return;
	}
	
	//���݂̒l�Ɣ�r���ē����Ȃ牽�����Ȃ�
	AText	text;
	GetText(inID,text);
	if( inText.Compare(text) == true )   return;
	
	//
	//�ʏ�R���g���[���̏ꍇ
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
			//DropList�̏ꍇ�A��L�ł͐ݒ�ł��Ȃ��̂ŁA���X�g�̃e�L�X�g�Ɣ�r���āAindex��ݒ肷��
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).AddText(inText);
		return;
	}
	//�ʏ�R���g���[���̏ꍇ
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	outBool = false;
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return false;}
	
	ABool	result = false;
	GetBool(inID,result);
	return result;
}

//
void	CWindowImp::SetBool( const AControlID inID, const ABool inBool )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//���݂̒l�Ɣ�r���ē����Ȃ牽�����Ȃ�
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
		//�|�b�v�A�b�v�{�^���̏ꍇ��false�Ȃ�1�ԖځAtrue�Ȃ�2�Ԗڂ̍��ڂ�I������
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPaneConst(inID).GetColor(outColor);
		return;
	}
	
	//�ʏ�R���g���[���̏ꍇ
	//��������
}

//
void	CWindowImp::SetColor( const AControlID inID, const AColor& inColor )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//���݂̒l�Ɣ�r���ē����Ȃ牽�����Ȃ�
	AColor	color;
	GetColor(inID,color);
	if( AColorWrapper::CompareColor(color,inColor) == true )   return;
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inID) == true )
	{
		GetUserPane(inID).SetColor(inColor);
		return;
	}
	
	//�ʏ�R���g���[���̏ꍇ
	//��������
}

//
void	CWindowImp::GetNumber( const AControlID inID, ANumber& outNumber ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return 0;}
	
	ANumber	num = 0;
	GetNumber(inID,num);
	return num;
}

//
void	CWindowImp::GetFloatNumber( const AControlID inID, AFloatNumber& outNumber ) const
{
	//�E�C���h�E�����ς݃`�F�b�N
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//���݂̒l�Ɣ�r���ē����Ȃ牽�����Ȃ�
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//���݂̒l�Ɣ�r���ē����Ȃ牽�����Ȃ�
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inControlID) == true )
	{
		GetUserPane(inControlID).SetIcon(inIconID,inDefaultSize,inLeftOffset,inTopOffset);
		return;
	}
	
	//�ʏ�R���g���[���̏ꍇ
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

#pragma mark --- �R���g���[�����I���C���^�[�t�F�C�X

//
void	CWindowImp::CreateBevelButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, 
		const AText& inTitle, const ABool inEnableMenu )
{
	DWORD	winstyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;
	HWND	hwnd = ::CreateWindowW(L"button",NULL,winstyle,
			inPoint.x,inPoint.y,inWidth,inHeight,
			mHWnd,
			(HMENU)inID,//�q�E�C���h�EID
			CAppImp::GetHInstance(),NULL);
	::SetWindowPos(hwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	RegisterControl(hwnd);
}

/* AView_Button����������ߕs�v 
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
	SetControlPosition(inID,inPoint);//AWindowPoint�̓g�b�v�E�C���h�E���W�n�Ȃ̂ŕϊ�
	
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
			(HMENU)inID,//�q�E�C���h�EID
			CAppImp::GetHInstance(),NULL);
	::SetWindowPos(hwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	RegisterControl(hwnd);
	SetControlPosition(inID,inPoint);//AWindowPoint�̓g�b�v�E�C���h�E���W�n�Ȃ̂ŕϊ�
}

//
void	CWindowImp::RegisterScrollBar( const AControlID inID )
{
	//��������
}

/**
�T�C�Y�{�b�N�X����
*/
void	CWindowImp::CreateSizeBox( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	HWND	hwnd = ::CreateWindowW(L"scrollbar",NULL,WS_CHILD | WS_VISIBLE | SBS_SIZEBOX,
			inPoint.x,inPoint.y,inWidth,inHeight,
			mHWnd,
			(HMENU)inID,//�q�E�C���h�EID
			CAppImp::GetHInstance(),NULL);
	RegisterControl(hwnd);
	//�X�N���[���o�[�S�ẴJ�[�\�����ς���Ă��܂��̂ŉE�L��NG::SetClassLong(hwnd,GCL_HCURSOR,(LONG)::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENWSE)));
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
	//���b�� �f�o�b�O�r���i�T�C�Y�{�b�N�X�\������Ȃ��j
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

//���b�� �e�X�g����
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
			(HMENU)5999,//�q�E�C���h�EID
			CAppImp::GetHInstance(),NULL);
	RegisterControl(hb);
	::CreateWindowW(L"button",NULL,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			28,2,24,24,
			mUserPaneArray.GetObject(index).GetHWnd(),
			(HMENU)5998,//�q�E�C���h�EID
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
	
	//���b�� �Ή��K�v? textarraymenu
	
	/*#135
	//FocusGroup����폜
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
		//UserPane�̏ꍇ�iCUserPane��DestroyWindow()�����s�j
		//DestroyUserPane()��WM_DESTROY������s���邱�Ƃɂ���GetUserPane(inID).DestroyUserPane();
		::DestroyWindow(GetHWndByControlID(inID));
		//
		mUserPaneArray.Delete1Object(userpaneindex);
		mUserPaneArray_ControlID.Delete1(userpaneindex);
		mUserPaneArray_Type.Delete1(userpaneindex);
	}
	else
	{
		//�ʏ�R���g���[���̏ꍇ
		::DestroyWindow(GetHWndByControlID(inID));
	}
	//�R���g���[���o�^�폜
	UnregisterControl(inID);
	//Bindings�ݒ�폜
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

#pragma mark --- �t�H�[�J�X�Ǘ�

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
	//�E�C���h�E���A�N�e�B�u�ɂ��Ȃ��Ƃ���Focus���ݒ肵�Ȃ��B�i::SetFocus()�̓E�C���h�E���A�N�e�B�u�ɂȂ�B�j
	if( mNoActivate == true )   return;
	
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}

	//char	str[256];
	//sprintf(str,"SetFocus()S:(current:%d new:%d) \n",mCurrentFocus,inControlID);
	//OutputDebugStringA(str);
	
	//
	if( inControlID == mCurrentFocus )   return;
	
	//
	//#135 if( mFocusGroup.Find(inControlID) == kControlID_Invalid )   return;
	
	//���t�H�[�J�X���L������
	//�i������EVT_ValueChanged()���s�͂��Ȃ��BEVT_ValueChanged()����SetFocus()���R�[�������\��������̂ŁA�����ċA�Ăяo���ɂȂ�B
	//EVT_ValueChanged()���R�[������̂́AmCurrentFocus��ύX��B����Ȃ�A��̏����ł͂�����Ď��s����Ȃ��B
	AControlID	oldFocus = GetCurrentFocus();
	
	//���݂̃t�H�[�J�X���N���A����
	ClearFocus();
	
	//
	HWND hwnd = ::SetFocus(GetHWndByControlID(inControlID));
	//BS_NOTIFY��ݒ肷��i���ꂪ�Ȃ���Win7�̏ꍇ�Ƀt�H�[�J�X�j�����\������Ȃ����ԈႢ�H
	//HWND	hwnd = GetHWndByControlID(inControlID);
	//::SetWindowLong(hwnd,GWL_STYLE,
	//		::GetWindowLong(hwnd,GWL_STYLE)|BS_NOTIFY);
	/*
	if( hwnd == NULL )
	{
		return;
	}
	*/
	
	//mCurrentFocus��ݒ�
	mCurrentFocus = inControlID;
	mLatentFocus = inControlID;//#0 kControlID_Invalid;
	
	//���݂̃t�H�[�J�X�ʒu�ƈႤ�R���g���[����focus�����ꍇ�́A�t�H�[�J�X�R���g���[����ValueChanged��ʒm
	if( oldFocus != kControlID_Invalid && oldFocus != inControlID )
	{
		GetAWin().EVT_ValueChanged(oldFocus);
	}
	
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( IsUserPane(inControlID) == true )
	{
		//UserPane��SetFocus()���s
		GetUserPane(inControlID).SetFocus();
	}
	//OutputDebugStringA("SetFocus()E  \n");
}

/**
�t�H�[�J�X���N���A
*/
void	CWindowImp::ClearFocus( const ABool inAlsoClearLatentFocus )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}

	//char	str[256];
	//sprintf(str,"ClearFocus()S:%d ",mCurrentFocus);
	//OutputDebugStringA(str);
	//UserPane�̏ꍇ�͑Ή����\�b�h���R�[��
	if( mCurrentFocus != kControlID_Invalid )
	{
		if( IsUserPane(mCurrentFocus) == true )
		{
			GetUserPane(mCurrentFocus).ResetFocus();
		}
	}
	//���Ȃ��Ƃ�Overlay���͂����Őݒ肷��ƃ��C���E�C���h�E�̃t�H�[�J�X���O���
	//::SetFocus(mHWnd);
	//mCurrentFocus���N���A
	mCurrentFocus = kControlID_Invalid;
	//inAlsoClearLatentFocus��true�Ȃ�latent���N���A�i�f�t�H���g��latent���N���A�B�������Adeactivated����R�[�������ꍇ��latent�͎c���j #212
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
				focusGroupIndex++;
				if( focusGroupIndex >= mFocusGroup.GetItemCount() )
				{
					focusGroupIndex = 0;
				}
			}
		}
	}
	//Enable����Visible�ȃR���g���[�����Ȃ��ꍇ�̓t�H�[�J�X���N���A
	ClearFocus();
}

//
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
*/
//�t�H�[�J�X�̃R���g���[����Disable���ꂽ�ꍇ�A�܂��́A�t�H�[�J�X�����̏�Ԃ���t�H�[�J�X�\�ȃR���g���[����Enable���ꂽ�ꍇ�̏���
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

#pragma mark --- �^�uView�Ǘ�

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
�^�uControl�I��
*/
void	CWindowImp::SetTabControlValue( const AIndex inTabIndex )
{
	if( mTabControlHWnd == NULL )   {_ACError("",this);return;}
	TabCtrl_SetCurSel(mTabControlHWnd,inTabIndex);
}

#pragma mark ===========================

#pragma mark --- UserPane�Ǘ�

/*#688
//
void	CWindowImp::RegisterTextEditPane( const AControlID inID, const ABool inWrapMode, 
		const ABool inVScrollbar, const ABool inHScrollBar, const ABool inHasFrame )
{
	//�L�[�{�[�h�t�H�[�J�X�O���[�v�֓o�^
	//#135 mFocusGroup.Add(inID);
	
	//�����Ȃ�
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
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	HWND	dummycontrol = GetHWndByControlID(inID);
	RECT	bounds;
	::GetWindowRect(dummycontrol,&bounds);
	HWND	parent = ::GetParent(dummycontrol);
	RECT	parentbounds;
	::GetWindowRect(parent,&parentbounds);
	//�_�~�[�R���g���[���폜
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
	//�R���g���[���o�^
	RegisterControl(mUserPaneArray.GetObject(index).GetHWnd());
	
}

//
void	CWindowImp::RegisterPaneForAView( const AControlID inID/*#135, const ABool inSupportFocus*/ )
{
	//�E�C���h�E�����ς݃`�F�b�N
	if( IsWindowCreated() == false ) {_ACError("Window not created",this);return;}
	
	HWND	dummycontrol = GetHWndByControlID(inID);
	RECT	bounds;
	::GetWindowRect(dummycontrol,&bounds);
	HWND	parent = ::GetParent(dummycontrol);
	RECT	parentbounds;
	//::GetWindowRect(parent,&parentbounds);GetWindowRect()����parent��Window�̂Ƃ��ɁA�^�C�g���o�[���܂񂾗̈�ɂȂ�̂ŁAGetWindowInfo()�֕ύX
	WINDOWINFO	parentwininfo;
	::GetWindowInfo(parent,&parentwininfo);
	parentbounds = parentwininfo.rcClient;
	//�_�~�[�R���g���[���폜
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
	//�R���g���[���o�^
	RegisterControl(mUserPaneArray.GetObject(index).GetHWnd());
	/*#135
	//
	if( inSupportFocus == true )
	{
		RegisterFocusGroup(inID);
	}
	*/
	//Offscreen�ݒ�iWM_PAINT�O��GetDrawTextWidth()���R�[���ł���悤�Ɂj�B���̃^�C�~���O�ł͕`��͂����AWM_PAINT�ŕ`�悳���
	if( width > 0 && height > 0 )
	{
		GetUserPane(inID).PrepareOffscreen();
	}
}

//AView�pUserPane�𐶐�
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
	SetControlPosition(inID,inPoint);//AWindowPoint�̓g�b�v�E�C���h�E���W�n�Ȃ̂ŕϊ�
	
	/*#135
	//
	if( inSupportFocus == true )
	{
		RegisterFocusGroup(inID);
	}
	*/
	
	//#291 UserPane��SupportFocus�t���O��ݒ�
	//#360 GetUserPane(inID).SetSupportFocus(inSupportFocus);
	//Offscreen�ݒ�iWM_PAINT�O��GetDrawTextWidth()���R�[���ł���悤�Ɂj�B���̃^�C�~���O�ł͕`��͂����AWM_PAINT�ŕ`�悳���
	if( inWidth > 0 && inHeight > 0 )
	{
		GetUserPane(inID).PrepareOffscreen();
	}
}

/**
UserPane�̒l���ύX���ꂽ�Ƃ���CUserPane����̃R�[���o�b�N
*/
void	CWindowImp::CB_UserPaneValueChanged( const AControlID inID )
{
	GetAWin().EVT_ValueChanged(inID);
}

/**
UserPane��Text���͂��ꂽ�Ƃ���CUserPane����̃R�[���o�b�N
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

//�w��ControlID�̃R���g���[����UserPane���ǂ����𔻒�
ABool	CWindowImp::IsUserPane( const AControlID inControlID ) const
{
	AIndex	index = mUserPaneArray_ControlID.Find(inControlID);
	return (index != kIndex_Invalid);
}

#pragma mark ===========================

#pragma mark ---OSControl�̃e�[�u��View�Ǘ�
//Windows�̏ꍇ�͖��Ή�

//TableView�o�^
void	CWindowImp::RegisterTableView( const AControlID inControlID, const AControlID inUp, const AControlID inDown,
		const AControlID inTop, const AControlID inBottom )
{
	_ACError("not implemented",this);
}

//���o�^
void	CWindowImp::RegisterTableViewColumn( const AControlID inControlID, const APrefID inColumnID, const ANumber inDataType )
{
	_ACError("not implemented",this);
}

//�e�[�u���f�[�^�S�ݒ�

//�ݒ�J�n���ɌĂ�
void	CWindowImp::BeginSetTable( const AControlID inControlID )
{
	_ACError("not implemented",this);
}

//��f�[�^��ݒ�(Text)
void	CWindowImp::SetTable_Text( const AControlID inControlID, const APrefID inColumnID, const ATextArray& inTextArray )
{
	_ACError("not implemented",this);
}

//��f�[�^��ݒ�(Bool)
void	CWindowImp::SetTable_Bool( const AControlID inControlID, const APrefID inColumnID, const ABoolArray& inBoolArray )
{
	_ACError("not implemented",this);
}

//��f�[�^��ݒ�(Number)
void	CWindowImp::SetTable_Number( const AControlID inControlID, const APrefID inColumnID, const ANumberArray& inNumberArray )
{
	_ACError("not implemented",this);
}

//��f�[�^��ݒ�(Color)
void	CWindowImp::SetTable_Color( const AControlID inControlID, const APrefID inColumnID, const AColorArray& inColorArray )
{
	_ACError("not implemented",this);
}

//�S��ݒ芮�����ɌĂ�
void	CWindowImp::EndSetTable( const AControlID inControlID )
{
	_ACError("not implemented",this);
}

//�s�ǉ�

//�s�ǉ��J�n���ɌĂ�
void	CWindowImp::BeginInsertRows( const AControlID inControlID )
{
	_ACError("not implemented",this);
}

//�s�i1�Z���j�ǉ�(Text)
void	CWindowImp::InsertRow_Text( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, const AText& inText )
{
	_ACError("not implemented",this);
}
void	CWindowImp::InsertRow_Text_SwapContent( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, AText& ioText )
{
	_ACError("not implemented",this);
}

//�s�i1�Z���j�ǉ�(Number)
void	CWindowImp::InsertRow_Number( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, const ANumber inNumber )
{
	_ACError("not implemented",this);
}

//�s�ǉ��������ɌĂ�
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

#pragma mark --- �|�b�v�A�b�v���j���[�{�^���Ǘ�

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
	//�S�폜
	while( ::SendMessageW(inHWnd,CB_GETCOUNT,0,0) > 0 )
	{
		::SendMessageW(inHWnd,CB_DELETESTRING,0,0);
	}
	//textarray�ǉ�
	for( AIndex i = 0; i < inTextArray.GetItemCount(); i++ )
	{
		AStCreateWcharStringFromAText	str(inTextArray.GetTextConst(i));
		::SendMessageW(inHWnd,CB_INSERTSTRING,-1,(LPARAM)(str.GetPtr()));
	}
	//�ŏ��̍��ڂ�I������
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
		//mValueIndependentMenuArray_ControlID�ɓo�^�i���j���[�\�����e�ƁAGetText()/SetText()�ł̒l���قȂ邽�߁j
		AIndex	valueIndependentMenuArrayIndex = mValueIndependentMenuArray_ControlID.Add(inID);
		mValueIndependentMenuArray_ValueArray.AddNewObject();
		mValueIndependentMenuArray_TitleArray.AddNewObject();
		//�u�i�f�t�H���g�̃t�H���g�j�v�̃��j���[���ڂ�ǉ�
		AText	defaultFontTitle;
		defaultFontTitle.SetLocalizedText("DefaultFontTitle");
		AStCreateWcharStringFromAText	str(defaultFontTitle);
		::SendMessageW(GetHWndByControlID(inID),CB_INSERTSTRING,0,(LPARAM)(str.GetPtr()));
		AText	defaultFontValue("default");
		mValueIndependentMenuArray_TitleArray.GetObject(valueIndependentMenuArrayIndex).Add(defaultFontTitle);
		mValueIndependentMenuArray_ValueArray.GetObject(valueIndependentMenuArrayIndex).Add(defaultFontValue);
		//�e�t�H���g�̃��j���[���ڂ�mValueIndependentMenuArray_ValueArray�ɒǉ�
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
		//�t�H���g�ꗗ�쐬 
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
	//�C�x���g�g�����U�N�V�����O�����^�㏈�� #0 �����ɂ��ꂪ�Ȃ��ƁARefresh���o�R��OS����C�x���gCB���������Ƃ��ɁA������sEventDepth==1�ƂȂ��Ă��܂��A
	//Window���̃I�u�W�F�N�g�ɑ΂���DoObjectArrayItemTrashDelete()���������A�����o�[�ϐ��ւ̃������A�N�Z�X���������A�����Ă��܂��B
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

//TextArrayMenu�ւ̓o�^
void	CWindowImp::RegisterTextArrayMenu( const AControlID inControlID, const ATextArrayMenuID inTextArrayMenuID )
{
	//�C���X�^���X���z��֓o�^
	mTextArrayMenuArray_ControlID.Add(inControlID);
	/*#232
	mTextArrayMenuArray_TextArrayMenuID.Add(inTextArrayMenuID);
	//static�z��֓o�^
	sTextArrayMenu_HWnd.Add(GetHWndByControlID(inControlID));
	sTextArrayMenu_TextArrayMenuID.Add(inTextArrayMenuID);
	//�R���{�{�b�N�X��DB��TextArray�𔽉f
	SetComboStringFromTextArray(GetHWndByControlID(inControlID),sTextArrayMenuDB.GetData_TextArrayRef(inTextArrayMenuID));
	*/
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().RegisterControl(GetAWin().GetObjectID(),inControlID,inTextArrayMenuID);
}

//TextArrayMenu�o�^����
void	CWindowImp::UnregisterTextArrayMenu( const AControlID inControlID )
{
	/*#232
	//static�z�񂩂�폜
	AIndex	staticindex = sTextArrayMenu_HWnd.Find(GetHWndByControlID(inControlID));
	if( staticindex == kIndex_Invalid )   {_ACError("",this);return;}
	sTextArrayMenu_HWnd.Delete1(staticindex);
	sTextArrayMenu_TextArrayMenuID.Delete1(staticindex);
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
	//DB�֔��f
	sTextArrayMenuDB.SetData_TextArray(inTextArrayMenuID,inTextArray);
	
	//�o�^���̃��j���[�S�Ăɔ��f
	for( AIndex index = 0; index < sTextArrayMenu_HWnd.GetItemCount(); index++ )
	{
		if( sTextArrayMenu_TextArrayMenuID.Get(index) == inTextArrayMenuID )
		{
			SetComboStringFromTextArray(sTextArrayMenu_HWnd.Get(index),inTextArray);
		}
	}
}

//TextArray���j���[�o�^(DB�ǉ�)(static)
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

#pragma mark --- ���W�I�{�^���O���[�v

//���W�I�{�^���O���[�v����
void	CWindowImp::RegisterRadioGroup()
{
	mRadioGroup.AddNewObject();
}

//�Ō�ɐ����������W�I�{�^���O���[�v�ɃR���g���[����ǉ�
void	CWindowImp::AddToLastRegisteredRadioGroup( const AControlID inID )
{
	if( mRadioGroup.GetItemCount() == 0 )   return;
	mRadioGroup.GetObject(mRadioGroup.GetItemCount()-1).Add(inID);
}

//���W�I�{�^���O���[�v��Update�i�w�肵���R���g���[���ȊO�Afalse�ɐݒ肷��j
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

//���W�I�O���[�v�ɐݒ肳�ꂽ�{�^���̏ꍇ�AOn��disable�ɂ��ꂽ��A���̃{�^����On�ɂ���B
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

#pragma mark --- �t�@�C���^�t�H���_�I��

//
void	CWindowImp::ShowChooseFolderWindow( const AFileAcc& inDefaultFolder, 
		const AControlID inVirtualControlID, const ABool inSheet )//#551
{
	//��inDefaultFolder�Ή��K�v
	const wchar_t	title[] = L"Select a directory.";//���b�� �f�B���N�g���I�𕶎���Ή��K�v
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
	//��inDefaultFile�Ή��K�v
	//���b��iinOnlyApp���Ή��K�v�j
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
	//���b�� ���Ή�
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
	//�f�t�H���g�t�@�C������path�֐ݒ�ilpstrFile�ɂ͌��ʂ��i�[�����̂ŁAAStCreateWcharStringFromAText�͎g���Ȃ��j
	AText	filename;
	filename.SetText(inFileName);
	filename.ConvertFromUTF8ToAPIUnicode();
	filename.Add(0);//UTF16 NULL�����ǉ�
	filename.Add(0);
	filename.CopyToTextPtr((AUCharPtr)path,kPathMaxLength*sizeof(wchar_t),0,filename.GetItemCount());
	//
	title[0] = 0;
	//
	AStCreateWcharStringFromAText	filtertextptr(inFilter);
	//�����f�B���N�g���ilpstrInitialDir��ReadOnly�����o�[�Ȃ̂ŁAAStCreateWcharStringFromAText�g�p�\�j
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

//Windows�̏ꍇ��HWND���K�v�Ȃ̂Ńt�@�C���I�[�v���_�C�A���O��CWindowImp�Ŏ��s����
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

#pragma mark --- �q�E�C���h�E�Ǘ�
/* #291�ɂ��AWindow_OK���֕ύX
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

//�q�E�C���h�E�\���iOK�{�^��+Cancel�{�^��+Discard�{�^���j#8
void	CWindowImp::ShowChildWindow_OKCancelDiscard( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
		const AControlID inOKButtonVirtualControlID, const AControlID inDiscardButtonVirtualControlID )
{
}

//
void	CWindowImp::CloseChildWindow()
{
	//Windows�̏ꍇ�AMessageBox()�ŁAmodal�Ȃ̂ŕs�v�i�R�[������Ȃ��j
}
*/
//
ABool	CWindowImp::ChildWindowVisible() const
{
	//Windows�̏ꍇ�AMessageBox()�ŁAmodal�Ȃ̂ŕs�v�i�R�[������Ȃ��j
	return false;
}

#pragma mark ===========================

#pragma mark --- �w�i

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
	//�f�[�^�ǂݏo���A�|�C���^�ݒ�
	/*
	AText	imagedata;
	inBackgroundImageFile.ReadTo(imagedata);
	AStTextPtr	fileHeaderPtr(imagedata,0,sizeof(BITMAPFILEHEADER));
	LPBITMAPFILEHEADER	lpFileHeader = (LPBITMAPFILEHEADER)(fileHeaderPtr.GetPtr());
	DWORD	offBits = lpFileHeader->bfOffBits;//Bitmap�f�[�^�̃t�@�C���̍ŏ������offset
	AStTextPtr	infoHeaderPtr(imagedata,sizeof(BITMAPFILEHEADER),sizeof(BITMAPINFOHEADER));
	LPBITMAPINFOHEADER	lpInfoHeader = (LPBITMAPINFOHEADER)(infoHeaderPtr.GetPtr());
	AStTextPtr	bmpPtr(imagedata,offBits,imagedata.GetItemCount()-offBits);
	*/
	//
	AStCreateWcharStringFromAText	pathcstr(path);
	Gdiplus::Image	image(pathcstr.GetPtr());
	
	//�r�b�g�}�b�v�쐬�pHDC���擾
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
			0,0,//�]������W
			lpInfoHeader->biWidth,lpInfoHeader->biHeight,//���A����
			0,0,//�]�������W
			0,lpInfoHeader->biHeight,//�����J�n�ԍ��A�������̖{��
			(char*)(bmpPtr.GetPtr()),//bmp�f�[�^�J�n�A�h���X
			(const BITMAPINFO*)(infoHeaderPtr.GetPtr()),
			DIB_RGB_COLORS);
	*/
	//
	//HBITMAP	hbitmap = ::LoadBitmap(CAppImp::GetHInstance(),MAKEINTRESOURCE(IDB_BITMAP1));//zantei
	AIndex index = sBackgroundImageArray_File.AddNewObject();
	sBackgroundImageArray_File.GetObject(index).CopyFrom(inBackgroundImageFile);
	sBackgroundImageArray_HBitmap.Add(hbitmap);
	//�r�b�g�}�b�v�쐬�pHDC���폜
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
	//LineBreak�v�Z�pOffscreen����
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
		//Font���쐬�A�܂��́A�O��ƈႤ�ꍇ�́A�쐬����
		if(	sLineBreakHFont == NULL ||
			sLineBreakHFont_FontName.Compare(inFontName) == false ||
			sLineBreakHFont_FontSize != inFontSize ||
			sLineBreakHFont_AntiAlias != antiAlias )
		{
			//�t�H���g�쐬
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
			//�t�H���g���ݒ�
			::SelectObject(sLineBreakOffscreenDC,sLineBreakHFont);
			::SetTextColor(sLineBreakOffscreenDC,kColor_Black);
			::SetBkMode(sLineBreakOffscreenDC,TRANSPARENT);
		}
		//���pa�̃T�C�Y�擾
		SIZE	atextsize;
		wchar_t	atext[1];
		atext[0] = 'a';
		::GetTextExtentPoint32W(sLineBreakOffscreenDC,&(atext[0]),1,&atextsize);
		int	awidth = atextsize.cx;
		if( awidth == 0 )   awidth = 5;
		//�S�p�u���v�̃T�C�Y�擾
		SIZE	zatextsize;
		wchar_t	zatext[1];
		zatext[0] = 0x3042;//�u���v
		::GetTextExtentPoint32W(sLineBreakOffscreenDC,&(zatext[0]),1,&zatextsize);
		int zawidth = zatextsize.cx;
		if( zawidth == 0 )   zawidth = 5;
		//�s�܂�Ԃ��ʒu���ς�����s��
		//0x3000�ȏ�Ȃ�S�p�u���v�̕��A�����Ȃ甼�pa�̕��𑫂��Ă����AinViewWidth�𒴂��Ȃ��ő�̕��������擾��estimateCount
		ANumber	estimateCount = 0;
		ANumber	estimateWidth = 0;
		ANumber	estimateCountMax = (ioTextDrawData.UTF16DrawText.GetItemCount())/sizeof(AUTF16Char)-inStartOffset;
		for( ; estimateCount < estimateCountMax; estimateCount++ )
		{
			//�����ɉ����ĕ��𑫂��Ă���
			AUTF16Char	uniChar = ioTextDrawData.UTF16DrawText.GetUTF16CharFromUTF16Text((inStartOffset+estimateCount)*sizeof(AUTF16Char));
			if( uniChar >= 0x3000 )
			{
				estimateWidth += zawidth;
			}
			else
			{
				estimateWidth += awidth;
			}
			//inViewWidth�𒴂�����I��
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
			//���ς���l�ŕ��v�Z
			SIZE	firsttextsize;
			::GetTextExtentPoint32W(sLineBreakOffscreenDC,
						(LPCWSTR)(utf16textptr.GetPtr()),(pos-inStartOffset),&firsttextsize);
			if( firsttextsize.cx < inViewWidth )
			{
				//���₵�Ă����āA�ŏ���textsize.cx > inViewWidth�ƂȂ����Ƃ���ŏI��
				while( pos < ioTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) )
				{
					SIZE	textsize;
					::GetTextExtentPoint32W(sLineBreakOffscreenDC,
								(LPCWSTR)(utf16textptr.GetPtr()),(pos-inStartOffset),&textsize);
					if( textsize.cx > inViewWidth )
					{
						//inViewWidth�����������Ƃ��낪�ŏI����
						pos--;
						break;
					}
					pos++;
				}
			}
			else
			{
				//���炵�Ă����āA�ŏ���textsize.cx < inViewWidth�ƂȂ����Ƃ���ŏI��
				while( pos > inStartOffset+2 )
				{
					SIZE	textsize;
					::GetTextExtentPoint32W(sLineBreakOffscreenDC,
								(LPCWSTR)(utf16textptr.GetPtr()),(pos-inStartOffset),&textsize);
					if( textsize.cx < inViewWidth )
					{
						//inViewWidth�����������Ƃ��낪�ŏI����
						break;
					}
					pos--;
				}
			}
		}
		//���[�h���b�v����
		if( pos > inStartOffset && pos < ioTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) -1 )
		{
			//���b�� UCS4���K�v
			AUTF16Char	curChar = ioTextDrawData.UTF16DrawText.
					GetUTF16CharFromUTF16Text(pos*sizeof(AUTF16Char));
			AUTF16Char	prevChar = ioTextDrawData.UTF16DrawText.
					GetUTF16CharFromUTF16Text((pos-1)*sizeof(AUTF16Char));
			if( AApplication::GetApplication().NVI_GetUnicodeData().IsAlphabetOrNumber(prevChar) == true && 
						AApplication::GetApplication().NVI_GetUnicodeData().IsAlphabetOrNumber(curChar) == true )
			{
				//�P�������O�ɖ߂��āA�A���t�@�x�b�g�ȊO�ɂȂ�ӏ���T��
				AIndex	newpos = pos-1;
				for( ; newpos > inStartOffset; newpos-- )
				{
					AUTF16Char	ch = ioTextDrawData.UTF16DrawText.
							GetUTF16CharFromUTF16Text(newpos*sizeof(AUTF16Char));
					if( AApplication::GetApplication().NVI_GetUnicodeData().IsAlphabetOrNumber(ch) == false )
					{
						//�A���t�@�x�b�g�ȊO�̈ʒu��break
						break;
					}
				}
				if( newpos > inStartOffset )
				{
					//pos���A���t�@�x�b�g�ȊO�̕����̒���ɐݒ�
					pos = newpos+1;
				}
			}
		}
		return pos;
	}
	else if( false )
	{
		/*
		DrawTextEx()���t�H���g�ɂ���Ă͔��ɏd��(Consolas��)
		*/
		//Font���쐬�A�܂��́A�O��ƈႤ�ꍇ�́A�쐬����
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
		���i�b��jLineBreak�v�Z����
		�E�x��
		�Eword wrap�ł��Ă��Ȃ�
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

#pragma mark --- �C�����C������

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

#pragma mark --- �w���v

void	CWindowImp::RegisterHelpAnchor( const ANumber inHelpNumber, AConstCstringPtr inAnchor )
{
	//���b�� ���Ή�
}
*/
#pragma mark ===========================

#pragma mark --- �A�C�R���o�^

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
	//Windows�͏����Ȃ�
	return kIconID_NoIcon;
}

void	CWindowImp::RegisterIconFromResouce( const AIconID inIconID, const AIndex inResID )
{
	//Windows�͏����Ȃ�
}

/*#1012
AIconID	CWindowImp::RegisterDynamicIconFromImageFile( const AFileAcc& inFile )
{
	return kIconID_NoIcon;
}
*/

AIconID	CWindowImp::RegisterDynamicIconFromFile( const AFileAcc& inIconFile, const ABool inLoadEvenIfNotCustomIcon )
{
	//".ico"��t�����t�@�C�����̃t�@�C�����A�C�R���t�@�C���Ƃ���
	AText	path;
	inIconFile.GetNormalizedPath(path);
	path.AddCstring(".ico");
	AStCreateWcharStringFromAText	pathstring(path);
	//�A�C�R���ǂݍ��݁A�o�^
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
�A�C�R�����t�@�C���փR�s�[
*/
void	CWindowImp::CopyIconToFile( const AIconID inIconID, AFileAcc& inDestIconFile )
{
	//���b�� �Ή��K�v
}

//#232
/**
�A�C�R���t�@�C���g���q�擾
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
�A�C�R�����O����IconID�擾
*/
AIconID	CWindowImp::GetIconIDByName( const AText& inName )
{
	AIndex	index = sIconNameArray.Find(inName);
	if( index == kIndex_Invalid )   return kIconID_NoIcon;
	return sIconArray_ID.Get(index);
}


#pragma mark ===========================

#pragma mark --- API�R�[���o�b�N

ABool	CWindowImp::APICB_WndProc( UINT message, WPARAM wParam, LPARAM lParam, LRESULT& outResult )
{
	outResult = 0;
	ABool	handled = true;
	switch(message)
	{
	  case WM_PAINT:
		{
			/*������s���ƃE�C���h�E�ĕ`�掞��Window�ł̔��h��Ԃ���View�ł̃I�t�X�N���[���R�s�[���������A������Ă��܂�
			//View�̂Ȃ��ꏊ�ł����F�œh��Ԃ��悤�ɁAWindow�S�̂�FillRect()����
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
			//CUserPane�ł��`����s�����߁Afalse��Ԃ�
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
			//���j���[�I���iMNS_NOTIFYBYPOS��ݒ肵�Ă���̂�WM_MENUCOMMAND�ł���j
			//actiontext�擾�̂��߁Amenuref��index���~�����̂ŁAWM_MENUCOMMAND���g��
			
			AMenuRef	menuref = (AMenuRef)lParam;
			AIndex	menuIndex = (AIndex)wParam;
			AMenuItemID	menuItemID = AMenuWrapper::GetMenuItemID(menuref,menuIndex);
			//actiontext�擾
			AText	actiontext;
			AApplication::GetApplication().NVI_GetMenuManager().GetMenuActionText(menuItemID,menuref,menuIndex,actiontext);
			//Win�Ŏ��s���s
			if( GetAWin().EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
			{
				AApplication::GetApplication().NVI_UpdateMenu();
				::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
				break;
			}
			//�eWindow�Ŏ��s���s
			if( mParentWindowRef != NULL )
			{
				AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mParentWindowRef);
				if( parentWindowIndex != kIndex_Invalid )
				{
					if( (sAliveWindowArray.Get(parentWindowIndex))->GetAWin().
								EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
					{
						AApplication::GetApplication().NVI_UpdateMenu();
						::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
						break;
					}
				}
			}
			//App�Ŏ��s���s
			else if( AApplication::GetApplication().EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
			{
				AApplication::GetApplication().NVI_UpdateMenu();
				::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
				break;
			}
			break;
		}
	  case WM_COMMAND:
		{
			if( lParam == 0 )
			{
				//�A�N�Z�����[�^�L�[�ɂ�郁�j���[�I���i�ʏ�̃��j���[�I����MNS_NOTIFYBYPOS��ݒ肵�Ă���̂�WM_MENUCOMMAND�ł���j
				
				int wmId, wmEvent;
				wmId    = LOWORD(wParam);
				wmEvent = HIWORD(wParam);
				AText	actiontext;
				AMenuItemID	menuItemID = wmId;
				//actiontext�̓��j���[�e�L�X�g�����̂܂܎g��
				AApplication::GetApplication().NVI_GetMenuManager().GetMenuItemText(menuItemID,actiontext);
				//Win�Ŏ��s���s
				if( GetAWin().EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
				{
					AApplication::GetApplication().NVI_UpdateMenu();
					::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
					break;
				}
				//�eWindow�Ŏ��s���s
				if( mParentWindowRef != NULL )
				{
					AIndex	parentWindowIndex = sAliveWindowArray_HWnd.Find(mParentWindowRef);
					if( parentWindowIndex != kIndex_Invalid )
					{
						if( (sAliveWindowArray.Get(parentWindowIndex))->GetAWin().
									EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
						{
							AApplication::GetApplication().NVI_UpdateMenu();
							::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
							break;
						}
					}
				}
				//App�Ŏ��s���s
				else if( AApplication::GetApplication().EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
				{
					AApplication::GetApplication().NVI_UpdateMenu();
					::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
					break;
				}
			}
			else
			{
				//�q�R���g���[�������Notification���b�Z�[�W
				//BS_NOTIFY��ݒ肵�Ȃ��ꍇ��BN_CLICKED�̂݁A�ݒ肵���ꍇ��BN_DBLCLK, BN_SETFOCUS, BN_KILLFOCUS���B
				//��{�I�ɃR���g���[���̃T�u�N���X�őΉ�����̂ŁABN_SETFOCUS���͏������Ȃ��B
				
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
							//CBN_SELCHANGE�̓e�L�X�g�{�b�N�X�ύX�O�ɒʒm�����̂ŁA
							//EVT_ValueChanged()������GetText()���R�[�������Ƃ��ɑI����̃e�L�X�g���I�������悤�ɁA
							//�����Ńe�L�X�g�{�b�N�X���X�V���Ă����B
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
					//Edit�R���g���[�����́A�ҏW����EVT_ValueChanged()�������s����ƁA�l�␳���������肵�Ă��܂�
				  default:
					{
						//��������
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
		/*#688 ���ڎ��s�ɕύX
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
					/*��������*/
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
					/*��������*/
					break;
				}
			}
			GetAWin().EVT_DoScrollBarAction(controlID,scrollBarpart);
			break;
		}
		//�}�E�X�z�C�[��
		//�ʃE�C���h�E��ł̃}�E�X�z�C�[���͂��̃E�C���h�E��ł̃X�N���[���ɂ��邽�߁A
		//WM_APP_MOUSEWHEEL�C�x���g�ɕϊ����ă��_�C���N�g����B
	  case WM_MOUSEWHEEL:
		{
			AGlobalPoint	globalPoint;
			globalPoint.x = GET_X_LPARAM(lParam);
			globalPoint.y = GET_Y_LPARAM(lParam);
			//�q�I�[�o�[���C�E�C���h�E�����͂��߂Ɍ���
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
					//�ʃE�C���h�E��ł̃z�C�[��
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
				/*wParam����ctrl, shift�ȊO�͎擾�ł��Ȃ��̂ŁAAKeyWrapper::GetEventKeyModifiers()���g�����Ƃɂ���
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
					//Bindings�K�p
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
					//Bindings�K�p
					ApplyBindings(prev,current);
					//
					GetAWin().EVT_WindowBoundsChanged(prev,current);
					//
					mUnzoomedBounds = mCurrentWindowBounds;
					break;
				}
			  default:
				{
					//�����Ȃ�
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
			//Bindings�K�p
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
			
			//Minimize��������
			if( mCurrentWindowBounds.left == -32000 && mCurrentWindowBounds.top == -32000 )
			{
				//Minimize������ʒm
				GetAWin().EVT_WindowMinimizeRestored();
				//Overlay�E�C���h�E����ɕ`�悳��Ă��̌��Minimize�����A�j���[�V��������������̂�h��
				::UpdateWindow(mHWnd);//��ʕ`��
				::Sleep(250);//Minimize�����A�j���[�V�������s�҂�
			}
			
			AGlobalRect	prev, current;
			prev = mCurrentWindowBounds;
			GetWindowBounds(mCurrentWindowBounds);
			current = mCurrentWindowBounds;
			//Bindings�K�p
			ApplyBindings(prev,current);
			
			//
			RECT	newRect = {0,0,0,0};
			::GetWindowRect(mHWnd,&newRect);
			//�E�C���h�E�����ړ��J�n
			HDWP	hdwp = ::BeginDeferWindowPos(mOverlayWindowGroup_WindowRef.GetItemCount()*2);
			//�eOverlay�E�C���h�E�ړ�
			for( AIndex i = 0; i < mOverlayWindowGroup_WindowRef.GetItemCount(); i++ )
			{
				//Overlay�E�C���h�E�ړ�
				::DeferWindowPos(hdwp,mOverlayWindowGroup_WindowRef.Get(i),HWND_TOP,
						newRect.left	+ mOverlayWindowGroup_XOffset.Get(i),
						newRect.top		+ mOverlayWindowGroup_YOffset.Get(i),
						0,
						0,
						SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
				//OverlayBackground�E�C���h�E�ړ�
				::DeferWindowPos(hdwp,mOverlayWindowGroup_BackgroundWindowRef.Get(i),HWND_TOP,
						newRect.left	+ mOverlayWindowGroup_XOffset.Get(i),
						newRect.top		+ mOverlayWindowGroup_YOffset.Get(i),
						0,
						0,
						SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
			}
			//�E�C���h�E�����ړ����s
			::EndDeferWindowPos(hdwp);
			
			//Minimize���ǂ����̔���
			if( mCurrentWindowBounds.left == -32000 && mCurrentWindowBounds.top == -32000 )
			{
				//Minimize�̏ꍇ�AMinimize�ʒm
				GetAWin().EVT_WindowMinimized();
				//�t�H�[�J�X���Ȃ��ɐݒ肷��BMinimize�������iActivate���j�ɁASetFocus()�����邽�߁B
				mCurrentFocus = kIndex_Invalid;
			}
			else
			{
				//Minimize�ȊO�̏ꍇ�AWindowBoundsChanged�ʒm
				GetAWin().EVT_WindowBoundsChanged(prev,current);
			}
			break;
		}
	  case WM_SIZING:
	  case WM_WINDOWPOSCHANGING:
		{
			//�����Ȃ�
			//�T�C�Y�ύX���ɁAWM_SIZE���A�����đ��t�����B
			//Windows�̐ݒ�ɂ���邪�A�f�t�H���g�̓T�C�Y�ύX���ɂ�WM_SIZE���Ă΂��B
			//�����炭�AWindows95?�����͕ύX���ɂ�WM_SIZING�݂̂������Ǝv���邪�A���I�ύX�̂��߂Ɏd�l���ύX���ꂽ�Ǝv����B
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
			//Overlay�E�C���h�E���ꏏ�Ɉړ�����
			//�ړ���E�C���h�E�ʒu�擾
			RECT	newRect = *((LPRECT)lParam);
			//sprintf(str,"WM_MOVING(new) left:%d right:%d \n",newRect.left,newRect.right);
			//OutputDebugStringA(str);
			
			//Windows7�ōő剻�z�����������ADeferWindowPos�Ŏ��E�C���h�E�ړ�����ƁA�z�������A�z�����J��Ԃ��Ă��܂��̂ŁA���̉����B
			if( newRect.left == 0 )   break;
			
			//�E�C���h�E�����ړ��J�n
			HDWP	hdwp = ::BeginDeferWindowPos(1+mOverlayWindowGroup_WindowRef.GetItemCount()*2);
			//�����ړ�
			::DeferWindowPos(hdwp,mHWnd,HWND_TOP,
					newRect.left,
					newRect.top,
					newRect.right-newRect.left,
					newRect.bottom-newRect.top,
					SWP_NOZORDER|SWP_NOACTIVATE);
			//�eOverlay�E�C���h�E�ړ�
			for( AIndex i = 0; i < mOverlayWindowGroup_WindowRef.GetItemCount(); i++ )
			{
				//Overlay�E�C���h�E�ړ�
				::DeferWindowPos(hdwp,mOverlayWindowGroup_WindowRef.Get(i),HWND_TOP,
						newRect.left	+ mOverlayWindowGroup_XOffset.Get(i),
						newRect.top		+ mOverlayWindowGroup_YOffset.Get(i),
						0,
						0,
						SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
				//OverlayBackground�E�C���h�E�ړ�
				::DeferWindowPos(hdwp,mOverlayWindowGroup_BackgroundWindowRef.Get(i),HWND_TOP,
						newRect.left	+ mOverlayWindowGroup_XOffset.Get(i),
						newRect.top		+ mOverlayWindowGroup_YOffset.Get(i),
						0,
						0,
						SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
			}
			//�E�C���h�E�����ړ����s
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
					//UserPane��Activated�ʒm
					for( AIndex i = 0; i < mUserPaneArray.GetItemCount(); i++ )
					{
						mUserPaneArray.GetObject(i).DoWindowActivated();
					}
					//�t�H�[�J�X�𕜌�
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
						//�������j���[(RootMenu)�𕡐��E�C���h�E�Ɋ��蓖�ĂĂ����Ȃ�����
						//���̏ꍇ�ASetMenu()���E�C���h�E�؂�ւ��̂��тɓ��삵�Ȃ��̂Ń��j���[�̂��������������
						if( ::GetMenu(mHWnd) != rootmenu )
						{
							::SetMenu(mHWnd,rootmenu);
						}
						AApplication::GetApplication().NVI_UpdateMenu();
					}
					//#404 �t���X�N���[��TOPMOST�ݒ�
					if( mFullScreenMode == true && mWindowModalParentMode == false )
					{
						::SetWindowPos(mHWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
					}
					break;
				}
			  case WA_INACTIVE:
				{
					//���݂̃t�H�[�J�X��ۑ����āA�t�H�[�J�X���N���A
					if( /*#212 mLatentFocus == kControlID_Invalid &&*/ mCurrentFocus != kControlID_Invalid )
					{
						mLatentFocus = mCurrentFocus;
						ClearFocus(false);//#212
					}
					//UserPane��Deactivated�ʒm
					for( AIndex i = 0; i < mUserPaneArray.GetItemCount(); i++ )
					{
						mUserPaneArray.GetObject(i).DoWindowDeactivated();
					}
					//
					GetAWin().EVT_WindowDeactivated();
					//menu
					/*if( mEachMenu != NULL )
					{
						::SetMenu(mHWnd,mEachMenu);������SetMenu����ƃ^�C�g���o�[���A�N�e�B�u�ɂȂ�
					}*/
					//#404 �t���X�N���[��TOPMOST����
					if( mFullScreenMode == true && mWindowModalParentMode == false )
					{
						::SetWindowPos(mHWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
					}
					break;
				}
			  default:
				{
					//��������
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
		/*���̏����̓_�C�A���O�N���b�N���Ƀ��C���E�C���h�E���A�N�e�B�u�ɂȂ�Ȃǂ̖�肪����A����ɕK�v�Ȃ��Ǝv���邽�߃R�����g�A�E�g
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
				::ClientToScreen(mHWnd,&pt);//global point�֕ϊ�
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
		
		//�}�E�X�֘A�����iOverlayWindow�ւ̃��_�C���N�g�̂݁j�i�qWND�̖����ӏ��݂̂������Ă΂��B�qWND�������ꍇ�A���鏈����Overlay�ւ�redirect�̂݁j
		//�}�E�X�N���b�N
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
			//globalPoint�擾
			AGlobalPoint	globalPoint;
			::ClientToScreen(mHWnd,&pt);
			globalPoint.x = pt.x;
			globalPoint.y = pt.y;
			//OverlayWindow�ւ̃��_�C���N�g���s���s�i�������s�����Ȃ�true�ŕԂ�j
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
OverlayWindow�ւ̃��_�C���N�g���s���s�i�e�������E�f�B�X�p�b�`�����j
*/
ABool	CWindowImp::RedirectToOverlayWindow( UINT message, WPARAM wParam, LPARAM lParam, const AGlobalPoint inGlobalPoint )
{
	//�q�I�[�o�[���C�E�C���h�E�������i�}�E�X�ʒu�Ƀq�b�g����E�C���h�E�j���A�ŏ��Ƀq�b�g�����E�C���h�E�Ƀf�B�X�p�b�`
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
OverlayWindow�ւ̃��_�C���N�g���s�iOverlayWindow�����s�����j
*/
ABool	CWindowImp::DoOverlayWindowRedirect( UINT message, WPARAM wParam, LPARAM lParam, const AGlobalPoint inGlobalPoint )
{
	AControlID	controlID = FindControl(inGlobalPoint);
	if( IsUserPane(controlID) == false )//   return false;
	{
		//UserPane�ȊO�̃R���g���[���͂����Ŏ��s
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
	
	//ControlLocal�擾
	POINT	pt;
	pt.x = inGlobalPoint.x;
	pt.y = inGlobalPoint.y;
	::ScreenToClient(GetUserPane(controlID).GetHWnd(),&pt);
	ALocalPoint	localPoint;
	localPoint.x = pt.x;
	localPoint.y = pt.y;
	
	//UserPane�ł̏������s
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
�E�C���h�E���ɁATracking����UserPane������΁ADoMouseLeft()����
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
�E�C���h�E���ɁATracking����UserPane������΁ADoMouseLeft()����iinExceptControlID�ȊO�j
*/
void	CWindowImp::TryMouseLeaveForAllUserPanesExceptFor( const AControlID inExceptControlID )
{
	//�E�C���h�E���̕ʂ�control��Tracking���Ȃ�ADoMouseLeft()����
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
���̃E�C���h�E�̑S�Ă�UserPane��DoMouseLeft()����iinExceptControlID�ȊO�j
*/
void	CWindowImp::TryMouseLeaveForAllWindowsExceptFor( const AControlID inExceptControlID )
{
	//���E�C���h�E��inExceptControlID�ȊO��UserPane��DoMouseLeft()
	TryMouseLeaveForAllUserPanesExceptFor(inExceptControlID);
	//���̃E�C���h�E�̑S�Ă�UserPane��DoMouseLeft()
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
Bindings�K�p
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
			//�e�R���g���[�����E�C���h�E�̏ꍇ
			
			//X����
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
			//Y����
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
			//�e�R���g���[�����R���g���[���̏ꍇ
			
			//�e������
			AIndex	pi = mControlArray_HWnd.Find(parent);
			if( pi != kIndex_Invalid )
			{
				//�e��Bindings���擾
				AControlID	parentControlID = mControlArray_ID.Get(pi);
				AIndex	parentBindingsIndex = mBindings_ControlID.Find(parentControlID);
				if( parentBindingsIndex != kIndex_Invalid )
				{
					//�e��Bindings�Ɋ֘A���ē��삷��B
					//�e�̃T�C�Y�ύX����������ꍇ�̂݁A�q���ړ��^�T�C�Y�ύX����������
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

#pragma mark --- API�R�[���o�b�N(static)

LRESULT CALLBACK CWindowImp::STATIC_APICB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-PRC)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return 0;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	
	//hWnd�ɑΉ�����CWindowImp�I�u�W�F�N�g������
	AIndex	index = sAliveWindowArray_HWnd.Find(hWnd);
	if( index == kIndex_Invalid )
	{
		//==================================================
		//���[�g�E�C���h�E�ł͂Ȃ��E�C���h�E�i�^�u��"dialogs"���j�p
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
					//�R���g���[�����b�Z�[�W�����[�g�E�C���h�E�֑��M
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
			//�N�����ɃV���[�g�J�b�g���ւ̃t�@�C���h���b�v�iWinMain()����MappedFile�Ƀt�@�C���p�X���������܂��j
		  case WM_APP_DROPFROMANOTHER:
			{
				HANDLE	hmap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,kIdForDropFromAnother);
				if( hmap == NULL )   {_ACError("",NULL);break;}
				wchar_t*	ptr = (wchar_t*)MapViewOfFile(hmap,FILE_MAP_ALL_ACCESS,0,0,0);
				if( ptr == NULL )   {::CloseHandle(hmap);_ACError("",NULL);break;}
				AText	cmdline;
				cmdline.SetFromWcharString(ptr,4096);//miedit.cpp�Ŏw�肵���}�b�N�X�T�C�Y
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
	//�C���X�^���X�̃R�[���o�b�N���\�b�h���Ăяo��
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
Overlay�w�i�E�C���h�E�pWindow�v���V�[�W��
*/
LRESULT CALLBACK CWindowImp::STATIC_APICB_WndProcOverlayBackground( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-PRC)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return 0;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	
	//
	ABool	handled = false;
	LRESULT	result = 0;
	try
	{
		switch(message)
		{
			//�}�E�X�C�x���g��{Overlay�E�C���h�E�փ��_�C���N�g����
		  case WM_MOUSEWHEEL://MouseWheel��SetFocus()���Ȃ��ƃC�x���g�L���b�`�ł��Ȃ�
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
					//�w�i�`��i�w�i�F�h��Ԃ��j
					//UpdateOverlayBackgroundWindow()�ł��w�i�`�悵�Ă��邪�A
					//�����ł��������Ȃ��ƁA�Ƃ��ǂ�Overlay�̔w�i�F���s��ɂȂ錻�ۂ���������
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
		//DefWindowProc()���Ȃ��ƃE�C���h�E�����ɂ����s����
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
}
	
//�Q�l�FPetzold section 17.3 EZFONT.c
//�֐��R�[������HFONT����K�v
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
	//��VERTRES, VERTSIZE����v�Z������@�Ƃǂ��炪�������H
	
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
	lf.lfCharSet		= DEFAULT_CHARSET;//���b�� ���̒l�͖���`������\������̂Ɏg����H
	lf.lfOutPrecision	= 0;
	lf.lfClipPrecision	= 0;
	if( inAntiAlias == true )
	{
		lf.lfQuality		= PROOF_QUALITY;
		//Windows7�ł̌��،���
		//ANTIALIASED_QUALITY: �������������ƂԂ�ĉ����Ȃ�
		//PROOF_QUALITY: ��Ԃ��ꂢ
		//CLEARTYPE_QUALITY: PROOF_QUALITY�Ɠ������H
	}
	else
	{
		lf.lfQuality		= NONANTIALIASED_QUALITY;
	}
	lf.lfPitchAndFamily	= 0;//������FIXED_PITCH�ɂ��Ă��v���|�[�V���i���𓙕��ɂ��邱�Ƃ͂ł��Ȃ�����
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

#pragma mark ---���[�e�B���e�B

#pragma mark ===========================

#pragma mark ---��ʃT�C�Y�擾

/**
���C���f�o�C�X�̃X�N���[���̈���擾
*/
void	CWindowImp::GetMainDeviceScreenBounds( AGlobalRect& outBounds )
{
	outBounds.left 		= 0;
	outBounds.top		= 0;
	outBounds.right		= ::GetSystemMetrics(SM_CXSCREEN);
	outBounds.bottom	= ::GetSystemMetrics(SM_CYSCREEN);
}

/**
�E�C���h�E�z�u�\�ȗ̈���擾�iDock, Menubar���ɏd�Ȃ�Ȃ��̈�j
*/
void	CWindowImp::GetAvailableWindowPositioningBounds( AGlobalRect& outBounds )
{
	RECT	rect;
	::SystemParametersInfoW(SPI_GETWORKAREA,0,&rect,0);
	outBounds.left 		= rect.left;
	outBounds.top		= rect.top;
	outBounds.right		= rect.right;
	outBounds.bottom	= rect.bottom;
	
	//��menubar�ɏd�Ȃ�Ȃ��悤�ɏC���K�v
}

#pragma mark ===========================

#pragma mark ---�E�C���h�E�擾

/**
�őO�ʃE�C���h�E�擾�ikWindowClass_Document�E�C���h�E�̒����猟���j
��GetForegroundWindow()��Windows�S�̂̍őO�ʂ��擾����̂ŃA�v�����őO�ʎ擾�ɂ͎g���Ȃ�
*/
AWindowRef	CWindowImp::GetMostFrontWindow()
{
	//document�N���X�E�C���h�E���P������
	HWND	hwnd = NULL;
	for( AIndex i = 0; i < sAliveWindowArray.GetItemCount(); i++ )
	{
		if( (sAliveWindowArray.Get(i))->mWindowClass == kWindowClass_Document )
		{
			hwnd = sAliveWindowArray_HWnd.Get(i);
			break;
		}
		
	}
	//�Ȃ����NULL��Ԃ�
	if( hwnd == NULL )   return NULL;
	
	//����top-level�̃E�C���h�E�̍őO�ʃE�C���h�E��������
	hwnd = ::GetWindow(hwnd,GW_HWNDFIRST);
	//kWindowClass_Document�̃E�C���h�E�̒��ōőO�ʂ̂��̂�����
	while( hwnd != NULL )
	{
		//kWindowClass_Document�̃E�C���h�E�������炻���Ԃ�
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
		//����z-order�̃E�C���h�E���擾
		hwnd = ::GetNextWindow(hwnd,GW_HWNDNEXT);//GetWindow()/GetNextWindow()�Ƀ��X�N������̂̓��[�v���ɃE�C���h�E���䂪����ꍇ���Ǝv����̂ŁA����͖��Ȃ��ł��傤
	}
	return NULL;
}

/**
���̃E�C���h�E�擾
*/
AWindowRef	CWindowImp::GetNextOrderWindow( const AWindowRef inWindowRef )
{
	return ::GetNextWindow(inWindowRef,GW_HWNDNEXT);
}

/**
N�Ԗڂ̃E�C���h�E�擾
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

