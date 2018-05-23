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

	CUserPane (Windows)

*/

#include "stdafx.h"

#include "CUserPane.h"
#include "../Frame.h"
#include <math.h>
#include <strsafe.h>
#include <windowsx.h>
#include <commctrl.h>
#include <windows.h>
#include <strsafe.h>
#include <shellapi.h>
#include <shlobj.h>

AArray<CUserPane*>	CUserPane::sAliveUserPaneArray;
AArray<HWND>		CUserPane::sAliveUserPaneArray_HWnd;

#pragma mark ===========================
#pragma mark [�N���X]CUserPane
#pragma mark ===========================

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^

//�R���X�g���N�^
CUserPane::CUserPane( AObjectArrayItem* inParent, CWindowImp& inWindowImp ) 
: AObjectArrayItem(inParent), mWindowImp(inWindowImp), mHWnd(0), mControlID(0), 
		mOffscreenDC(NULL), mOffscreenBitmap(NULL), mColor(kColor_Black), mStyle(kTextStyle_Normal),
		mMouseTrackingMode(false), mInWMPaint(false), mDrawLinePen(NULL), mDropTarget(NULL), 
		mMouseLeaveEventEnable(false), mMouseLeaveEventTracking(false), 
		mRedirectTextInput(false)
		,mDefaultCursorType(kCursorType_Arrow)
		//#360,mSupportFocus(true)//#291
		,mOffscreenWidth(0),mOffscreenHeight(0)
		,mPreparingOffscreen(false)
		,mFontSize(9.0), mAntiAliasing(true)
		,mOffscreenMode(false), mPaintDC(NULL)
{
	//
	sAliveUserPaneArray.Add(this);
	sAliveUserPaneArray_HWnd.Add(0);
	//
	for( AIndex i = 0; i < 8; i++ )
	{
		mHDefaultFont[i] = NULL;
	}
}

//�f�X�g���N�^
CUserPane::~CUserPane()
{
	//�����ɂ���Ƃ���DestroyUserPane()�ł��łɃE�C���h�E�폜�ς݂łȂ��Ă͂Ȃ�Ȃ��B
	if( mHWnd != 0 )
	{
		_ACError("",this);
	}
	//�f�X�g���N�^��DestroyWindow()���͍s��Ȃ��BDestroyUserPane()���ĂԂ��ƁB
	//�f�X�g���N�^�̎��s�̓C�x���g�g�����U�N�V�����̍Ō�ɂȂ�̂ŁA������UI�������s���ƁA
	//�폜�V�[�P���X�������iAPICB_ControlSubProc()���R�[������AControlID��������Ȃ��j
	//
	AIndex	index = sAliveUserPaneArray.Find(this);
	if( index != kIndex_Invalid )
	{
		sAliveUserPaneArray.Delete1(index);
		sAliveUserPaneArray_HWnd.Delete1(index);
	}
}

//AWindow�擾
AWindow&	CUserPane::GetAWin()
{
	return mWindowImp.GetAWin();
}

#pragma mark ===========================

#pragma mark ---�R���g���[�����擾

//������ControlID�擾
AControlID	CUserPane::GetControlID() const
{
	return mControlID;
}

//�R���g���[���̈ʒu�擾
void	CUserPane::GetControlPosition( AWindowPoint& outPoint ) const
{
	GetWinConst().GetControlPosition(GetControlID(),outPoint);
}

//�R���g���[���S�̂�Rect�擾
void	CUserPane::GetControlLocalFrameRect( ALocalRect& outRect ) const
{
	outRect.left 	= 0;
	outRect.top 	= 0;
	outRect.right 	= GetWinConst().GetControlWidth(GetControlID());
	outRect.bottom 	= GetWinConst().GetControlHeight(GetControlID());
}

#pragma mark ===========================

#pragma mark --- �E�C���h�E����

//Wnd����
void	CUserPane::CreateUserPane( const HWND inParent, const AControlID inControlID, const APoint& inPoint, 
			const ANumber inWidth, const ANumber inHeight, const ABool inFront )
{
	mControlID = inControlID;
	mHWnd = ::CreateWindowW(L"userpane",NULL,WS_CHILDWINDOW|WS_VISIBLE,
			inPoint.x,inPoint.y,inWidth,inHeight,
			inParent,
			(HMENU)inControlID,//�q�E�C���h�EID
			CAppImp::GetHInstance(),NULL);
	DWORD	err = GetLastError();
	if( mHWnd == 0 )
	{
		_ACThrow("CreateUserPane() failed",this);
	}
	if( inFront == true )
	{
		::SetWindowPos(mHWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	else
	{
		::SetWindowPos(mHWnd,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	
	//HWND�o�^
	AIndex	index = sAliveUserPaneArray.Find(this);
	if( index == kIndex_Invalid )
	{
		_ACThrow("",this);
	}
	sAliveUserPaneArray_HWnd.Set(index,mHWnd);
}

void	CUserPane::DestroyUserPane()
{
	//
	if( mDrawLinePen != NULL )
	{
		::DeleteObject(mDrawLinePen);
		mDrawLinePen = NULL;
	}
	//DropTarget�폜
	if( mDropTarget != NULL )
	{
		//Drag&Drop�o�^�폜
		::RevokeDragDrop(mHWnd);
		//CDropTarget�폜�i�Q�Ɖ񐔌��Z�j
		mDropTarget->Release();
		//
		mDropTarget = NULL;
	}
	//Offscreen�폜
	DisposeOffscreen();
	//
	for( AIndex i = 0; i < 8; i++ )
	{
		if( mHDefaultFont[i] != NULL )
		{
			::DeleteObject(mHDefaultFont[i]);
			mHDefaultFont[i] = NULL;
		}
	}
	//WND�폜
	//WM_DESTROY����DestroyUserPane()���R�[������悤�ɂ����̂ł����̓R�����g�A�E�g::DestroyWindow(mHWnd);
	mHWnd = 0;
}

void	CUserPane::EnableDrop( const AArray<AScrapType>& inScrapType )
{
	if( mHWnd == 0 )   {_ACError("",this);return;}
	if( mDropTarget == NULL )
	{
		mDropTarget = new CDropTarget(*this);
		::RegisterDragDrop(mHWnd,mDropTarget);
	}
	for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
	{
		mDropTarget->AddAcceptableType(inScrapType.Get(i));
	}
}

//
void	CUserPane::SetMouseTrackingMode( const ABool inSet )
{
	if( inSet == true )
	{
		::SetCapture(mHWnd);
		mMouseTrackingMode = true;
	}
	else
	{
		::ReleaseCapture();
		mMouseTrackingMode = false;
	}
}

//
void	CUserPane::EnableMouseLeaveEvent()
{
	mMouseLeaveEventEnable = true;
}

//
void	CUserPane::RegisterUserPaneWindow()
{
	WNDCLASSEXW	wndclass;
	wndclass.cbSize			= sizeof(WNDCLASSEXW);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc	= CUserPane::STATIC_APICB_WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= CAppImp::GetHInstance();
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= ::LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground	= NULL;//(HBRUSH)(COLOR_WINDOW+1);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= L"userpane";
	wndclass.hIconSm		= NULL;
	
	if( RegisterClassExW(&wndclass) == false )
	{
		_ACError("RegisterClass() failed",NULL);
	}
}

#pragma mark ===========================

#pragma mark ---���j���[�R�}���h����

//���j���[�R�}���h����
//virtual, public
ABool	CUserPane::DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	//�f�t�H���g����F��������
	return false;
}

void	CUserPane::UpdateMenu()
{
	//�f�t�H���g����F��������
	return;
}

#pragma mark ===========================

#pragma mark ---�R���g���[���t�H�[�J�X����

//�t�H�[�J�X�ɐݒ�
//virtual, public
void	CUserPane::SetFocus() 
{
	//�f�t�H���g����F��������
}

//�t�H�[�J�X����
//virtual, public
void	CUserPane::ResetFocus()
{
	//�f�t�H���g����F��������
}

#pragma mark ===========================

#pragma mark --- �R���g���[�������ݒ�C���^�[�t�F�C�X

//
void	CUserPane::SetEnableControl( const ABool inEnable )
{
	if( IsControlEnabled() == inEnable )   return;//#530
	::EnableWindow(mHWnd,inEnable);
}

//
ABool	CUserPane::IsControlEnabled() const
{
	return (::IsWindowEnabled(mHWnd)==TRUE);
}

//
void	CUserPane::SetShowControl( const ABool inShow )
{
	if( inShow == true )
	{
		::ShowWindow(mHWnd,SW_SHOWNORMAL);
		//������UpdateWindow()���Ȃ��ƁA�r���Ŕ��F�i�D�F�j�`�攭������ꍇ������B���̌�̉��炩�̃^�C�~���O��offscreen�p�ӑO�ɃE�C���h�E�`�攭�����邽�߁H
		::UpdateWindow(mHWnd);
	}
	else
	{
		::ShowWindow(mHWnd,SW_HIDE);
	}
}

//
ABool	CUserPane::IsControlVisible() const
{
	return (::IsWindowVisible(mHWnd)==TRUE);
}

//
ABool	CUserPane::IsControlActive() const
{
	return (GetWinConst().IsActive());
}

void	CUserPane::SetDefaultCursor( const ACursorType inCursorType )
{
	/*���̕��@�ł̓N���X�S�ẴJ�[�\�����ς���Ă��܂��̂ŏ������e�ύX
	switch(inCursorType)
	{
	  case kCursorType_IBeam:
		{
			::SetClassLong(mHWnd,GCL_HCURSOR,(LONG)::LoadCursor(NULL,MAKEINTRESOURCE(IDC_IBEAM)));
			break;
		}
	  case kCursorType_Arrow:
	  default:
		{
			::SetClassLong(mHWnd,GCL_HCURSOR,(LONG)::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));
			break;
		}
	}*/
	mDefaultCursorType = inCursorType;
}

//#282
/**
Control�T�C�Y�ݒ�
*/
void	CUserPane::SetControlSize( const ANumber inWidth, const ANumber inHeight )
{
	GetWin().SetControlSize(GetControlID(),inWidth,inHeight);
}

//#282
/**
Control�ʒu�ݒ�
*/
void	CUserPane::SetControlPosition( const AWindowPoint& inWindowPoint )
{
	GetWin().SetControlPosition(GetControlID(),inWindowPoint);
}

//#455
/**
Binding�ݒ�
*/
void	CUserPane::SetControlBindings( const ABool inLeft, const ABool inTop, const ABool inRight, const ABool inBottom )
{
	GetWin().SetControlBindings(GetControlID(),inLeft,inTop,inRight,inBottom);
}

#pragma mark ===========================

#pragma mark --- 

void	CUserPane::TellChangedToWindow()
{
	GetWin().CB_UserPaneValueChanged(GetControlID());
}

#pragma mark ===========================

#pragma mark ---Offscreen

/*
�`��͏�ɂ܂�Offscreen�ɑ΂��čs����B
�yOffscreen�ւ̕`��z
�@�ŏ���Offscreen���쐬�����Ƃ�
mUpdateRect��frame�S�̂ɂ���DoDraw()���R�[�������
�AScroll
mUpdateRect��Scroll�ɂ���ĐV���ɕ`�悷�ׂ��͈͂ɂ���DoDraw()���R�[�������

�Ȃ��ADoDraw()����Abs����EVT_DoDraw()���R�[������A���̒�����DrawText()���̕`�惋�[�`�����R�[�������B

OS�����WM_PAINT�C�x���g����M�����Ƃ��AOffscreen����E�C���h�E�ւ�BitBlt�]�����s����B
BitBlt�]����UserPane�t���[���S�̂ɑ΂��čs��
�E�C���h�E�ւ̒��ڕ`��́A��{�I�ɂ́A��LBitBlt�����ł���B
*/

//Offscreen�쐬�E�`��
void	CUserPane::MakeOffscreen( HDC inHDC )
{
	if( mOffscreenMode == false )   return;
	//char	str[256];
	//sprintf(str,"MakeOffscreen()-s hwnd:%X\n",mHWnd);
	//OutputDebugStringA(str);
	
	if( mOffscreenDC != NULL )
	{
		DisposeOffscreen();
	}
	//�t���[���擾
	ALocalRect	localFrame;
	GetControlLocalFrameRect(localFrame);
	//Offscreen����
	mOffscreenDC = ::CreateCompatibleDC(inHDC);
	mOffscreenWidth = localFrame.right-localFrame.left;
	mOffscreenHeight = localFrame.bottom-localFrame.top;
	mOffscreenBitmap = ::CreateCompatibleBitmap(inHDC,mOffscreenWidth,mOffscreenHeight);
	::SelectObject(mOffscreenDC,mOffscreenBitmap);
	//����
	::SelectObject(mOffscreenDC,::GetStockObject(WHITE_PEN));
	::PatBlt(mOffscreenDC,localFrame.left,localFrame.top,localFrame.right,localFrame.bottom,PATCOPY);
	//Offscreen�ւ̕`��
	mUpdateRect = localFrame;
	DoDraw();
	
	//sprintf(str,"MakeOffscreen()-e hwnd:%X\n",mHWnd);
	//OutputDebugStringA(str);
}

/**
Offscreen�쐬
���̃��\�b�h�ł͕`��͂����AWM_PAINT�ŕ`�悳���
*/
void	CUserPane::PrepareOffscreen()
{
	if( mOffscreenMode == false )   return;
	if( mOffscreenDC != NULL )   return;
	
	HDC	hdc = ::GetDC(GetHWnd());
	//�t���[���擾
	ALocalRect	localFrame;
	GetControlLocalFrameRect(localFrame);
	//Offscreen����
	mOffscreenDC = ::CreateCompatibleDC(hdc);
	mOffscreenWidth = localFrame.right-localFrame.left;
	mOffscreenHeight = localFrame.bottom-localFrame.top;
	mOffscreenBitmap = ::CreateCompatibleBitmap(hdc,mOffscreenWidth,mOffscreenHeight);
	::SelectObject(mOffscreenDC,mOffscreenBitmap);
	//����
	::SelectObject(mOffscreenDC,::GetStockObject(WHITE_PEN));
	::PatBlt(mOffscreenDC,localFrame.left,localFrame.top,localFrame.right,localFrame.bottom,PATCOPY);
	//
	::ReleaseDC(GetHWnd(),hdc);
	//
	mPreparingOffscreen = true;
}

//Offscreen�폜
void	CUserPane::DisposeOffscreen()
{
	if( mOffscreenDC != NULL )
	{
		::DeleteObject(mOffscreenBitmap);
		::DeleteDC(mOffscreenDC);
		mOffscreenDC = NULL;
		mOffscreenWidth = 0;
		mOffscreenHeight = 0;
	}
}

void	CUserPane::TransferOffscreen()
{
	if( IsControlVisible() == false )   return;
	if( mInWMPaint == true )   return;//WM_PAINT���œ]������邽��
	if( mOffscreenMode == false )   return;
	//Offscreen���E�C���h�E�֓]�ʂ���imUpdateRect��Ώہj
	RECT	rect;
	if( true )
	{
		rect.left		= mUpdateRect.left;
		rect.top		= mUpdateRect.top;
		rect.right		= mUpdateRect.right;
		rect.bottom		= mUpdateRect.bottom;
	}
	else
	{
		//�f�o�b�O�p�i��ɑS�̈���E�C���h�E�֓]�ʁj
		rect.left = 0;
		rect.top = 0;
		rect.right = mOffscreenWidth;
		rect.bottom = mOffscreenHeight;
	}
	::InvalidateRect(mHWnd,&rect,FALSE);
	//::UpdateWindow(mHWnd);����������Ŏ��s����Ƃ��Ƃ��Ε⊮���͂ō폜�����͂̉ߒ���������i������j�C�x���g�g�����U�N�V�������s��Ɉ�C��paint���ׂ��Ȃ̂ł�����UpdateWindow()�����s���Ȃ��ق����������ۂ�
}

/**
*/
void	CUserPane::SetOffscreenMode( const ABool inOffscreenMode )
{
	mOffscreenMode = inOffscreenMode;
}

#pragma mark ===========================

#pragma mark ---�`�摮���ݒ�

//�e�L�X�g�f�t�H���g�����ݒ�
void	CUserPane::SetDefaultTextProperty( const AText& inFontName, const AFloatNumber inFontSize, 
		const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing )
{
	//�ǂꂩ�̃p�����[�^���Ⴄ�Ƃ������ݒ肵�ăt�H���g�L���b�V������ #450
	if( mFontName.Compare(inFontName) == false || mFontSize != inFontSize || 
				mColor != inColor || mStyle != inStyle || mAntiAliasing != inAntiAliasing )
	{
		//
		mFontName.SetText(inFontName);
		mFontSize = inFontSize;
		mColor = inColor;
		mStyle = inStyle;
		mAntiAliasing = inAntiAliasing;
		//�t�H���g�L���b�V������
		for( AIndex i = 0; i < 8; i++ )
		{
			if( mHDefaultFont[i] != NULL )
			{
				::DeleteObject(mHDefaultFont[i]);
				mHDefaultFont[i] = NULL;
			}
		}
	}
}
void	CUserPane::SetDefaultTextProperty( const AColor& inColor, const ATextStyle inStyle )
{
	mColor = inColor;
	mStyle = inStyle;
}

/**
*/
HFONT	CUserPane::GetHFont( const ATextStyle inStyle ) const
{
	if( inStyle < 0 || inStyle >= 8 )  {_ACError("",NULL);return NULL;}
	
	//���łɎ擾�ς݂Ȃ炻���Ԃ�
	if( mHDefaultFont[inStyle] != NULL )   return mHDefaultFont[inStyle];
	
	//���擾�Ȃ�擾���ĕԂ�
	HDC	hdc = ::GetDC(mHWnd);
	if( hdc == NULL )   {_ACError("",NULL);return NULL;}
	mHDefaultFont[inStyle] = CWindowImp::CreateFont_(hdc,mFontName,mFontSize,inStyle,mAntiAliasing);
	::ReleaseDC(mHWnd,hdc);
	return mHDefaultFont[inStyle];
}

#pragma mark ===========================

#pragma mark ---�`�惋�[�`��

//�e�L�X�g�\��
void	CUserPane::DrawText( const ALocalRect& inDrawRect, const AText& inText, const AJustification inJustification )
{
	DrawText(inDrawRect,inDrawRect,inText,mColor,mStyle,inJustification);
}

void	CUserPane::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AText& inText, const AJustification inJustification )
{
	DrawText(inDrawRect,inClipRect,inText,mColor,mStyle,inJustification);
}

void	CUserPane::DrawText( const ALocalRect& inDrawRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification )
{
	DrawText(inDrawRect,inDrawRect,inText,inColor,inStyle,inJustification);
}

void	CUserPane::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification )
{
	CTextDrawData	textDrawData(false);
	textDrawData.UTF16DrawText.SetText(inText);
	textDrawData.UTF16DrawText.ConvertFromUTF8ToAPIUnicode();
	textDrawData.attrPos.Add(0);
	textDrawData.attrColor.Add(inColor);
	textDrawData.attrStyle.Add(inStyle);
	DrawText(inDrawRect,inClipRect,textDrawData,inJustification);
}

void	CUserPane::DrawText( const ALocalRect& inDrawRect, CTextDrawData& inTextDrawData, const AJustification inJustification )
{
	DrawText(inDrawRect,inDrawRect,inTextDrawData,inJustification);
}

void	CUserPane::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
		CTextDrawData& inTextDrawData, const AJustification inJustification )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	::SelectObject(hdc,GetHFont(0));
	::SetBkMode(hdc,TRANSPARENT);
	
	ANumber	textWidth = 1;
	ANumber	x = inDrawRect.left;
	ANumber	y = inDrawRect.top;//#450
	{
		AStTextPtr	utf16textptr(inTextDrawData.UTF16DrawText,0,inTextDrawData.UTF16DrawText.GetItemCount());
		
		SIZE	textsize;
		::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),utf16textptr.GetByteCount()/sizeof(AUTF16Char),&textsize);
		textWidth = textsize.cx;
		TEXTMETRIC	tm = {0};//#450
		::GetTextMetrics(hdc,&tm);//#450
		{
			switch(inJustification)
			{
			  case kJustification_Right:
				{
					x = inDrawRect.right - textWidth;
					break;
				}
			  case kJustification_RightCenter://#450
				{
					x = inDrawRect.right - textWidth;
					y = inDrawRect.top + (inDrawRect.bottom-inDrawRect.top)/2 - tm.tmHeight/2;
					break;
				}
			  case kJustification_Center:
				{
					x = (inDrawRect.left+inDrawRect.right)/2 - textWidth/2;
					break;
				}
			  case kJustification_LeftTruncated://#315
				{
					if( textWidth > inDrawRect.right-inDrawRect.left )
					{
						x = inDrawRect.right - textWidth;
					}
					break;
				}
			  case kJustification_CenterTruncated://#315
				{
					if( textWidth > inDrawRect.right-inDrawRect.left )
					{
						x = inDrawRect.right - textWidth;
					}
					else
					{
						x = (inDrawRect.left+inDrawRect.right)/2 - textWidth/2;
					}
					break;
				}
			}
		}
		
		if( inTextDrawData.drawSelection == true )
		{
			ALocalRect	rect = inDrawRect;
			//SIZE	ts;
			if( inTextDrawData.selectionStart != kIndex_Invalid )
			{
				//::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),inTextDrawData.selectionStart,&ts);
				//rect.left = inDrawRect.left + ts.cx;
				rect.left = inDrawRect.left +
						GetImageXByTextPosition(inTextDrawData,
						inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(inTextDrawData.selectionStart));
			}
			if( inTextDrawData.selectionEnd != kIndex_Invalid )
			{
				//::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),inTextDrawData.selectionEnd,&ts);
				//rect.right = inDrawRect.left + ts.cx;
				rect.right = inDrawRect.left +
						GetImageXByTextPosition(inTextDrawData,
						inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(inTextDrawData.selectionEnd));
			}
			PaintRect(rect,inTextDrawData.selectionColor,0.5);
		}
		
		//clips
		RECT	cliprect;
		cliprect.left		= inClipRect.left;
		cliprect.top		= inClipRect.top;
		cliprect.right		= inClipRect.right;
		cliprect.bottom		= inClipRect.bottom;
		RECT	updaterect;
		updaterect.left		= mUpdateRect.left;
		updaterect.top		= mUpdateRect.top;
		updaterect.right	= mUpdateRect.right;
		updaterect.bottom	= mUpdateRect.bottom;
		RECT	totalclip;
		::IntersectRect(&totalclip,&cliprect,&updaterect);
		//HRGN	hrgn = ::CreateRectRgn(inClipRect.left,inClipRect.top,inClipRect.right,inClipRect.bottom);
		HRGN	hrgn = ::CreateRectRgn(totalclip.left,totalclip.top,totalclip.right,totalclip.bottom);
		::SelectClipRgn(hdc,hrgn);
		
		::SetTextAlign(hdc,TA_UPDATECP);
		::MoveToEx(hdc,x,/*#450 inDrawRect.top*/y,NULL);
		if( inTextDrawData.attrPos.GetItemCount() > 0 )
		{
			//attrPos, curPos, len��Unicode�����P��
			//UTF16DrawText��AText(�o�C�g�P��)
			AIndex	curPos = 0;//#435 attrPos�̏��Ԃ�����ւ�����Ƃ��̖h�~�̂��߁AcurPos�łǂ��܂ŕ\�����������Ǘ�����
			for( AIndex i = 0; i < inTextDrawData.attrPos.GetItemCount(); i++ )
			{
				//len�ɁA��������RunLength�́AUniChar�ł̒������i�[����B
				AItemCount	len;
				if( i+1 < inTextDrawData.attrPos.GetItemCount() )
				{
					len = inTextDrawData.attrPos.Get(i+1) - curPos;//#435 inTextDrawData.attrPos.Get(i);
				}
				else
				{
					//for���[�v�Ō�Ńe�L�X�g�̍Ō�܂ł�K���\�����邱�Ƃ�ۏ؂��邽��
					len = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - curPos;//#435 inTextDrawData.attrPos.Get(i);
				}
				//len���e�L�X�g�̒������������Ȃ�Ȃ��悤�ɕ␳ #435
				if( curPos + len > inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) )
				{
					len = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - curPos;
				}
				//
				if( len <= 0 )   continue;
				
				AColor	color = inTextDrawData.attrColor.Get(i);
				::SetTextColor(hdc,color);
				ATextStyle	style = (inTextDrawData.attrStyle.Get(i)&0x07);
				::SelectObject(hdc,GetHFont(style));
/*				switch(style)
				{
				  case kTextStyle_Bold:
					{
						::SelectObject(hdc,mHDefaultFont_Bold);
						break;
					}
				  case kTextStyle_Normal:
				  default:
					{
						::SelectObject(hdc,mHDefaultFont_Normal);
						break;
					}
				}*/
				::TextOutW(hdc,0,0,
						(LPCWSTR)(utf16textptr.GetPtr()+/*#435 inTextDrawData.attrPos.Get(i)*/
						curPos*sizeof(AUTF16Char)),len);
				
				//curPos�ړ� #435
				curPos += len;
			}
		}
		else
		{
			::SetTextColor(hdc,mColor);
			::TextOutW(hdc,0,0,(LPCWSTR)(utf16textptr.GetPtr()),utf16textptr.GetByteCount()/sizeof(AUTF16Char));
		}
		
		::DeleteObject(hrgn);
		::SelectClipRgn(hdc,NULL);
	}
}

void	CUserPane::DrawIcon( const ALocalRect& inRect, const AIconID inIconID, const ABool inEnabledColor, const ABool inDefaultSize ) const
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	HICON	hicon = CWindowImp::GetHIconByIconID(inIconID);
	if( inEnabledColor == false )
	{
		HICON	hi = CWindowImp::GetHIconByIconID(inIconID,true);
		if( hi != NULL )   hicon = hi;
	}
	UINT	diFlags = DI_NORMAL;
	//win::DrawIconEx(hdc,inRect.left,inRect.top,hicon,0,0,0,NULL,diFlags);
	::DrawIconEx(hdc,inRect.left,inRect.top,hicon,16,16,0,NULL,diFlags);//win 16*16�̃T�C�Y�ŕ\��
	/*��L�Ō��̃A�C�R���̃T�C�Y���̂܂܂ŕ\���ł���E�E�E�͂�
	if( inDefaultSize == true )
	{
		diFlags |= DI_DEFAULTSIZE;
	}
	::DrawIconEx(hdc,inRect.left,inRect.top,hicon,inRect.right-inRect.left,inRect.bottom-inRect.top,0,NULL,diFlags);
	*/
}

void	CUserPane::DrawIconFromFile( const ALocalRect& inRect, const AFileAcc& inFile, const ABool inEnabledColor ) const
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	//���b�� 
	HICON	hicon = NULL;
	if( inFile.IsFolder() == false )
	{
		hicon = CWindowImp::GetHIconByIconID(kIconID_Abs_Document);
	}
	else
	{
		hicon = CWindowImp::GetHIconByIconID(kIconID_Abs_Folder);
	}
	UINT	diFlags = DI_NORMAL;
	//win ::DrawIconEx(hdc,inRect.left,inRect.top,hicon,0,0,0,NULL,diFlags);
	::DrawIconEx(hdc,inRect.left,inRect.top,hicon,16,16,0,NULL,diFlags);//win 16*16�̃T�C�Y�ŕ\��
}

//#379
/**
���p�`�`��
*/
void	CUserPane::PaintPoly( const AArray<ALocalPoint>& inPath, const AColor& inColor, const AFloatNumber inAlpha )
{
	if( inPath.GetItemCount() == 0 )   return;
	
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	//
	HRGN	hcliprgn = ::CreateRectRgn(mUpdateRect.left,mUpdateRect.top,mUpdateRect.right,mUpdateRect.bottom);
	if( hcliprgn == NULL )   return;
	HBRUSH	hbr = ::CreateSolidBrush(inColor);
	if( hbr == NULL )   {_ACError("",NULL);return;}
	::SelectObject(hdc,hbr);
	::SelectClipRgn(hdc,hcliprgn);
	
	//
	AItemCount	pointCount = inPath.GetItemCount();
	POINT*	pointArray = (POINT*)malloc((pointCount+1)*sizeof(POINT));
	if( pointArray == NULL )   {_ACThrow("",this);}
	for( AIndex i = 0; i < pointCount; i++ )
	{
		pointArray[i].x = inPath.Get(i).x;
		pointArray[i].y = inPath.Get(i).y;
	}
	pointArray[pointCount].x = inPath.Get(0).x;
	pointArray[pointCount].y = inPath.Get(0).y;
	//
	//�A���t�@���Ή� if( inAlpha == 1.0 )
	{
		::Polygon(hdc,pointArray,pointCount);
	}
	/*�A���t�@���Ή�
	{
		//�A���t�@�i���߁j�̏ꍇ
		//�I�t�X�N���[���r�b�g�}�b�v�Ɏw��F�ׂ��h�肵�āAAlphaBlend�ŃA���t�@�d�˓h�肷��
		//
		ANumber	w = 500;
		ANumber	h = 2000;
		//
		HDC	alphahdc = ::CreateCompatibleDC(hdc);
		HBITMAP alphahbitmap = ::CreateCompatibleBitmap(hdc,w,h);
		::SelectObject(alphahdc,alphahbitmap);
		//
		RECT	r;
		r.left		= 0;
		r.top		= 0;
		r.right		= w;
		r.bottom	= h;
		HBRUSH	backhbr = ::CreateSolidBrush(kColor_Black);
		if( backhbr == NULL )   {_ACError("",NULL);return;}
		::FillRect(alphahdc,&r,backhbr);
		//
		::SelectObject(alphahdc,hbr);
		::Polygon(alphahdc,pointArray,pointCount);
		//
		BLENDFUNCTION	ftn;
		ftn.BlendOp = AC_SRC_OVER;
		ftn.BlendFlags = 0;
		ftn.SourceConstantAlpha = 256*inAlpha;
		ftn.AlphaFormat = AC_SRC_ALPHA;
		::AlphaBlend(hdc,0,0,w,h,alphahdc,0,0,w,h,ftn);
		//
		::DeleteDC(alphahdc);
		::DeleteObject(alphahbitmap);
		::DeleteObject(backhbr);
	}
	*/
	//
	::free(pointArray);
	//
	::SelectClipRgn(hdc,NULL);
	::DeleteObject(hcliprgn);
	::DeleteObject(hbr);
}

/**
Rect�h��Ԃ�
*/
void	CUserPane::PaintRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha ) const
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	HRGN	hcliprgn = ::CreateRectRgn(mUpdateRect.left,mUpdateRect.top,mUpdateRect.right,mUpdateRect.bottom);
	if( hcliprgn == NULL )   return;
	HBRUSH	hbr = ::CreateSolidBrush(inColor);
	if( hbr == NULL )   {_ACError("",NULL);return;}
	::SelectClipRgn(hdc,hcliprgn);
	RECT	rect;
	rect.left		= inRect.left;
	rect.top		= inRect.top;
	rect.right		= inRect.right;
	rect.bottom		= inRect.bottom;
	
	if( inAlpha == 1.0 )
	{
		::FillRect(hdc,&rect,hbr);
	}
	else
	{
		//�A���t�@�i���߁j�̏ꍇ
		//�I�t�X�N���[���r�b�g�}�b�v�Ɏw��F�ׂ��h�肵�āAAlphaBlend�ŃA���t�@�d�˓h�肷��
		//
		ANumber	w = rect.right-rect.left;
		ANumber	h = rect.bottom-rect.top;
		//
		HDC	alphahdc = ::CreateCompatibleDC(hdc);
		HBITMAP alphahbitmap = ::CreateCompatibleBitmap(hdc,w,h);
		::SelectObject(alphahdc,alphahbitmap);
		//
		RECT	r;
		r.left		= 0;
		r.top		= 0;
		r.right		= w;
		r.bottom	= h;
		::FillRect(alphahdc,&r,hbr);
		//
		BLENDFUNCTION	ftn;
		ftn.BlendOp = AC_SRC_OVER;
		ftn.BlendFlags = 0;
		ftn.SourceConstantAlpha = 256*inAlpha;
		ftn.AlphaFormat = 0;
		::AlphaBlend(hdc,rect.left,rect.top,w,h,alphahdc,0,0,w,h,ftn);
		//
		::DeleteDC(alphahdc);
		::DeleteObject(alphahbitmap);
	}
	::SelectClipRgn(hdc,NULL);
	::DeleteObject(hcliprgn);
	::DeleteObject(hbr);
}

//#634
/**
���������O���f�[�V������PaintRect
*/
void	CUserPane::PaintRectWithVerticalGradient( const ALocalRect& inRect, 
		const AColor& inTopColor, const AColor& inBottomColor, const AFloatNumber inAlpha, const ABool inDrawUsingLines )
{
	if( inDrawUsingLines == true )
	{
		//���𕡐��������ƂŎ�������
		//�������s���������Ə������Ԃ�������
		AFloatNumber	percent = 0;
		AFloatNumber	h = (inRect.bottom-inRect.top-1);
		AFloatNumber	percentInc = 100.0/h;
		for( ANumber y = inRect.top; y < inRect.bottom; y++ )
		{
			AColor	color = AColorWrapper::GetGradientColor(inTopColor,inBottomColor,percent);
			ALocalPoint	spt = {inRect.left,y};
			ALocalPoint	ept = {inRect.right-1,y};
			DrawLine(spt,ept,color,inAlpha);
			percent += percentInc;
		}
	}
	else
	{
		AColor	color = AColorWrapper::GetGradientColor(inTopColor,inBottomColor,50.0);
		PaintRect(inRect,color,inAlpha);
		//�Ή��K�v
	}
}

//#634
/**
���������O���f�[�V������PaintRect
*/
void	CUserPane::PaintRectWithHorizontalGradient( const ALocalRect& inRect, 
		const AColor& inTopColor, const AColor& inBottomColor, const AFloatNumber inAlpha )
{
	AColor	color = AColorWrapper::GetGradientColor(inTopColor,inBottomColor,50.0);
	PaintRect(inRect,color,inAlpha);
	//�Ή��K�v
}

/**
�E�C���h�E��w�i�F�^�����F�ŏ���
�iOverlay�E�C���h�E�̏ꍇ��Rect�𓧖��F�œh��Ԃ��j
*/
void	CUserPane::EraseRect( const ALocalRect& inRect )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	if( GetWin().IsOverlayWindow() == true )
	{
		PaintRect(inRect,GetWin().GetOverlayBackgroundColor(),1.0);
	}
	else
	{
		HRGN	hcliprgn = ::CreateRectRgn(mUpdateRect.left,mUpdateRect.top,mUpdateRect.right,mUpdateRect.bottom);
		if( hcliprgn == NULL )   return;
		::SelectClipRgn(hdc,hcliprgn);
		RECT	rect;
		rect.left		= inRect.left;
		rect.top		= inRect.top;
		rect.right		= inRect.right;
		rect.bottom		= inRect.bottom;
		
		//�E�C���h�E�w�i�u���V�ŏ���
		::FillRect(hdc,&rect,GetWin().GetBackgroundBlash());
		
		::SelectClipRgn(hdc,NULL);
		::DeleteObject(hcliprgn);
	}
}

void	CUserPane::FrameRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	HBRUSH	hbr = ::CreateSolidBrush(inColor);
	if( hbr == NULL )   {_ACError("",NULL);return;}
	HRGN	hcliprgn = ::CreateRectRgn(mUpdateRect.left,mUpdateRect.top,mUpdateRect.right,mUpdateRect.bottom);
	if( hcliprgn == NULL )   {_ACError("",NULL);return;}
	::SelectClipRgn(hdc,hcliprgn);
	RECT	rect;
	rect.left		= inRect.left;
	rect.top		= inRect.top;
	rect.right		= inRect.right;
	rect.bottom		= inRect.bottom;
	::FrameRect(hdc,&rect,hbr);
	::SelectClipRgn(hdc,NULL);
	::DeleteObject(hcliprgn);
	::DeleteObject(hbr);
	//���b�� �A���t�@���Ή�
}

/**
*/
void	CUserPane::DrawFocusFrame( const ALocalRect& inRect )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	HRGN	hcliprgn = ::CreateRectRgn(mUpdateRect.left,mUpdateRect.top,mUpdateRect.right,mUpdateRect.bottom);
	if( hcliprgn == NULL )   {_ACError("",NULL);return;}
	::SelectClipRgn(hdc,hcliprgn);
	RECT	rect;
	rect.left		= inRect.left;
	rect.top		= inRect.top;
	rect.right		= inRect.right;
	rect.bottom		= inRect.bottom;
	
	//::DrawFocusRect(hdc,&rect);����͓_���g�ɂȂ�̂ňႤ
	//
	AColor	focuscolor;
	AColorWrapper::GetHighlightColor(focuscolor);
	ALocalRect	localrect = inRect;
	
	localrect.left++;
	localrect.top++;
	localrect.right--;
	localrect.bottom--;
	
	FrameRect(localrect,focuscolor);
	
	::SelectClipRgn(hdc,NULL);
	::DeleteObject(hcliprgn);
}

void	CUserPane::DrawBackgroundImage( const ALocalRect& inEraseRect, const AImageRect& inImageFrameRect, 
		const ANumber inLeftOffset, const AIndex inBackgroundImageIndex, const AFloatNumber inAlpha )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	//
	RECT	eraserect;
	eraserect.left		= inEraseRect.left;
	eraserect.top		= inEraseRect.top;
	eraserect.right		= inEraseRect.right;
	eraserect.bottom	= inEraseRect.bottom;
	//mUpdateRect��eraserect�Ƃ̏d��������clip����
	RECT	updaterect;
	updaterect.left		= mUpdateRect.left;
	updaterect.top		= mUpdateRect.top;
	updaterect.right	= mUpdateRect.right;
	updaterect.bottom	= mUpdateRect.bottom;
	RECT	cliprect;
	::IntersectRect(&cliprect,&eraserect,&updaterect);
	HRGN	hcliprgn = ::CreateRectRgn(mUpdateRect.left,mUpdateRect.top,mUpdateRect.right,mUpdateRect.bottom);
	if( hcliprgn == NULL )   return;
	::SelectClipRgn(hdc,hcliprgn);
	//
	HBITMAP	hbitmap = CWindowImp::GetBackgoundBitmap(inBackgroundImageIndex);
	BITMAP	bmp;
	::GetObject(hbitmap,sizeof(BITMAP),&bmp);
	HDC	bitmaphdc = ::CreateCompatibleDC(hdc);
	::SelectObject(bitmaphdc,hbitmap);
	//
	ANumber	picwidth = bmp.bmWidth;
	ANumber	picheight = bmp.bmHeight;
	if( picwidth == 0 )   picwidth = 1;
	//
	ANumber	startx = inImageFrameRect.left/picwidth;
	ANumber	starty = inImageFrameRect.top/picheight;
	for( ANumber x = startx; x < inImageFrameRect.right + inLeftOffset; x += picwidth )
	{
		for( ANumber y = starty; y < inImageFrameRect.bottom; y += picheight )
		{
			//Image��\��ʒu��Image���W�n�Ŏ擾
			AImageRect	pictimage;
			pictimage.left		= x - inLeftOffset;
			pictimage.top		= y;
			pictimage.right		= x - inLeftOffset + picwidth;
			pictimage.bottom	= y + picheight;
			//Local���W�n�ɕϊ�
			RECT	rect;
			rect.left		= pictimage.left		- inImageFrameRect.left;
			rect.top		= pictimage.top			- inImageFrameRect.top;
			rect.right		= pictimage.right		- inImageFrameRect.left;
			rect.bottom		= pictimage.bottom		- inImageFrameRect.top;
			//
			RECT	tmp;
			if( ::IntersectRect(&tmp,&eraserect,&rect) )
			{
				::BitBlt(hdc,rect.left,rect.top,picwidth,picheight,bitmaphdc,0,0,SRCCOPY);
			}
		}
	}
	::DeleteDC(bitmaphdc);
	::SelectClipRgn(hdc,NULL);
	::DeleteObject(hcliprgn);
	if( true )
	{
		PaintRect(inEraseRect,kColor_White,inAlpha);
	}
}

void	CUserPane::DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
		const AFloatNumber inAlpha, const AFloatNumber inLineDash, const AFloatNumber inLineWidth )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	HPEN	hpen = NULL;
	if( inLineDash == 0.0 )
	{
		hpen = ::CreatePen(PS_SOLID,static_cast<int>(inLineWidth),inColor);
	}
	else
	{
		//���b��
		hpen = ::CreatePen(PS_DOT,1,inColor);
	} 
	if( hpen == NULL )   {_ACError("",NULL);return;}
	::SetBkMode(hdc,TRANSPARENT);
	::SelectObject(hdc,hpen);
	::MoveToEx(hdc,inStartPoint.x,inStartPoint.y,NULL);
	::LineTo(hdc,inEndPoint.x,inEndPoint.y);
	::DeleteObject(hpen);
	//���b��@�A���t�@���Ή�
}

void	CUserPane::SetDrawLineMode( const AColor& inColor, const AFloatNumber inAlpha, 
		const AFloatNumber inLineDash, const AFloatNumber inLineWidth )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	if( mDrawLinePen != NULL )   ::DeleteObject(mDrawLinePen);
	mDrawLinePen = NULL;
	
	if( inLineDash == 0.0 )
	{
		mDrawLinePen = ::CreatePen(PS_SOLID,static_cast<int>(inLineWidth),inColor);
	}
	else
	{
		//���b��
		mDrawLinePen = ::CreatePen(PS_DOT,1,inColor);
	} 
	if( mDrawLinePen == NULL )   {_ACError("",NULL);return;}
	::SetBkMode(hdc,TRANSPARENT);
	::SelectObject(hdc,mDrawLinePen);
	//���b��@�A���t�@���Ή�
}

void	CUserPane::RestoreDrawLineMode()
{
	if( mDrawLinePen != NULL )   ::DeleteObject(mDrawLinePen);
	mDrawLinePen = NULL;
}

void	CUserPane::DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	::MoveToEx(hdc,inStartPoint.x,inStartPoint.y,NULL);
	::LineTo(hdc,inEndPoint.x,inEndPoint.y);
}

void	CUserPane::DrawXorCaretLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const ABool inClipToFrame, 
		const ABool inDash, const ANumber inPenSize )
{
	APoint	spt,ept;
	HWND	hwnd = mHWnd;
	if( inClipToFrame == false )
	{
		//UserPane����clip���Ȃ���UserPane�O�ւ��`�悷��ꍇ
		//Windows�ł́AWnd���R���g���[�����ƂȂ̂ŁA�O�֕`�悷��Ƃ��́A�g�b�v�E�C���h�E�ɕ`�悷��K�v������B
		
		//HWnd
		hwnd = GetWin().GetHWnd();
		
		//�E�C���h�E���W�ϊ�
		AWindowPoint	wspt, wept;
		GetWin().GetWindowPointFromControlLocalPoint(GetControlID(),inStartPoint,wspt);
		GetWin().GetWindowPointFromControlLocalPoint(GetControlID(),inEndPoint,wept);
		spt.x = wspt.x;
		spt.y = wspt.y;
		ept.x = wept.x;
		ept.y = wept.y;
	}
	else
	{
		//UserPane����clip����ꍇ
		//Wnd�O�ւ͕`�悳��Ȃ��̂ŁASelectClipRgn�͕s�v
		
		//
		spt.x = inStartPoint.x;
		spt.y = inStartPoint.y;
		ept.x = inEndPoint.x;
		ept.y = inEndPoint.y;
	}
	/*HRGN	hrgn = NULL;
	if( inClipToFrame == true )
	{
	ALocalRect	localFrame;
	GetControlLocalFrameRect(localFrame);
	hrgn = ::CreateRectRgn(localFrame.left,localFrame.top,localFrame.right,localFrame.bottom);
	::SelectClipRgn(hdc,hrgn);
	}*/
	
	//HDC�擾
	HDC	hdc = ::GetDC(hwnd);
	if( hdc == NULL )   {_ACError("",NULL);return;}
	
	HPEN	hpen = NULL;
	if( inDash == false )
	{
		hpen = ::CreatePen(PS_SOLID,inPenSize,kColor_White);
	}
	else
	{
		//���b��
		hpen = ::CreatePen(PS_SOLID,inPenSize,kColor_Gray30Percent);
		//hpen = ::CreatePen(PS_DOT,1,kColor_Black);
	} 
	if( hpen == NULL )   {_ACError("",NULL);return;}//����F�uhpen���擾�ł��Ȃ��悤�ȏ󋵂ł͂܂Ƃ��ɓ����Ȃ��i�̏Ⴉ�������s���j���܂Ƃ��ɓ����Ȃ���Ԃōׂ������[�N�΍􂷂�͖̂��Ӗ��v���|���V�[�B�ł��邱�Ƃ́A�f�[�^���ً}�ۑ����āA���[�U�[�Ɋ댯��Ԃ�ʒm���邱�Ƃ����B
	::SetBkMode(hdc,TRANSPARENT);
	::SetROP2(hdc,R2_XORPEN);
	::SelectObject(hdc,hpen);
	::MoveToEx(hdc,spt.x,spt.y,NULL);
	::LineTo(hdc,ept.x,ept.y);
	::DeleteObject(hpen);
	::SetROP2(hdc,R2_COPYPEN);
	
	/*if( hrgn != NULL )
	{
	::DeleteObject(hrgn);
	::SelectClipRgn(hdc,NULL);
	}*/
	
	::ReleaseDC(hwnd,hdc);
}

void	CUserPane::DrawXorCaretRect( const ALocalRect& inLocalRect, const ABool inClipToFrame )
{
	ARect	rect;
	HWND	hwnd = mHWnd;
	if( inClipToFrame == false )
	{
		//UserPane����clip���Ȃ���UserPane�O�ւ��`�悷��ꍇ
		//Windows�ł́AWnd���R���g���[�����ƂȂ̂ŁA�O�֕`�悷��Ƃ��́A�g�b�v�E�C���h�E�ɕ`�悷��K�v������B
		
		//HWnd
		hwnd = GetWin().GetHWnd();
		
		//�E�C���h�E���W�ϊ�
		AWindowRect	wrect;
		GetWin().GetWindowRectFromControlLocalRect(GetControlID(),inLocalRect,wrect);
		rect.left		= wrect.left;
		rect.top		= wrect.top;
		rect.right		= wrect.right;
		rect.bottom		= wrect.bottom;
	}
	else
	{
		//UserPane����clip����ꍇ
		//Wnd�O�ւ͕`�悳��Ȃ��̂ŁASelectClipRgn�͕s�v
		
		//
		rect.left		= inLocalRect.left;
		rect.top		= inLocalRect.top;
		rect.right		= inLocalRect.right;
		rect.bottom		= inLocalRect.bottom;
	}
	
	HDC	hdc = ::GetDC(hwnd);
	if( hdc == NULL )   {_ACError("",NULL);return;}
	
	HRGN	hrgn = NULL;
	if( inClipToFrame == true )
	{
		ALocalRect	localFrame;
		GetControlLocalFrameRect(localFrame);
		hrgn = ::CreateRectRgn(localFrame.left,localFrame.top,localFrame.right,localFrame.bottom);
		::SelectClipRgn(hdc,hrgn);
	}
	
	HPEN	hpen = ::CreatePen(PS_SOLID,1,kColor_White);
	if( hpen == NULL )   {_ACError("",NULL);return;}
	::SetBkMode(hdc,TRANSPARENT);
	::SetROP2(hdc,R2_XORPEN);
	::SelectObject(hdc,hpen);
	::MoveToEx(hdc,rect.left,rect.top,NULL);
	::LineTo(hdc,rect.right,rect.top);
	::LineTo(hdc,rect.right,rect.bottom);
	::LineTo(hdc,rect.left,rect.bottom);
	::LineTo(hdc,rect.left,rect.top);
	::DeleteObject(hpen);
	::SetROP2(hdc,R2_COPYPEN);
	
	if( hrgn != NULL )
	{
		::DeleteObject(hrgn);
		::SelectClipRgn(hdc,NULL);
	}
	
	::ReleaseDC(hwnd,hdc);
}

#pragma mark ===========================

#pragma mark ---Refresh

void	CUserPane::RefreshRect( const ALocalRect& inLocalRect )
{
	ALocalRect	localFrameRect;
	GetControlLocalFrameRect(localFrameRect);
	//R0108������ �t���[���T�C�Y�g���~���O�̌��ʁAlocalRect.top > localRect.bottom �ƂȂ�ƁADoDraw����UpdateRegion����ŁA�S�͈͑ΏۂɂȂ��Ă��܂��H�̂�
	//RefreshRect���t���[���O�̂Ƃ��͉������Ȃ��悤�ɂ���
	if( inLocalRect.bottom < localFrameRect.top )   return;
	if( inLocalRect.top > localFrameRect.bottom )   return;
	if( inLocalRect.right < localFrameRect.left )   return;
	if( inLocalRect.left > localFrameRect.right )   return;
	//�w��̈��`��iMacOSX�ł�OS��Update���w��(HIViewRender())����OS�o�R�ŕ`�悵�Ă������AWindows�ł�Offscreen��CUserPane�Ǘ��Ȃ̂Œ��ڕ`��j
	if( mOffscreenMode == true )
	{
		//
		if( mOffscreenDC == NULL )
		{
			PrepareOffscreen();
		}
		//
		mUpdateRect = inLocalRect;
		DoDraw();
		//WM_PAINT�C�x���g�𔭐������āAOffscreen���E�C���h�E�֓]�ʂ���
		/* EVT_DoDraw()������TransferOffscreen()���Ă�
		RECT	rect;
		rect.left		= inLocalRect.left;
		rect.top		= inLocalRect.top;
		rect.right		= inLocalRect.right;
		rect.bottom		= inLocalRect.bottom;
		::InvalidateRect(mHWnd,&rect,FALSE);
		::UpdateWindow(mHWnd);
		*/
	}
	else
	{
		RECT	rect;
		rect.left		= inLocalRect.left;
		rect.top		= inLocalRect.top;
		rect.right		= inLocalRect.right;
		rect.bottom		= inLocalRect.bottom;
		::InvalidateRect(mHWnd,&rect,FALSE);
		//::UpdateWindow(mHWnd);
	}
}

void	CUserPane::RefreshControl()
{
	ALocalRect	rect;
	GetControlLocalFrameRect(rect);
	RefreshRect(rect);
}

/**
�w��rect�ɂ���Offscreen�������ʂ�Flush����
*/
void	CUserPane::FlushRect( const ALocalRect& inLocalRect )
{
	RECT	rect;
	rect.left		= inLocalRect.left;
	rect.top		= inLocalRect.top;
	rect.right		= inLocalRect.right;
	rect.bottom		= inLocalRect.bottom;
	::InvalidateRect(mHWnd,&rect,FALSE);
	//UpdateWindow()����WM_PAINT�����������iWM_PAINT������AUpdateWindow()���A���Ă���j
	::UpdateWindow(mHWnd);
}

/**
�R���g���[���S�̂�Offscreen�������ʂ�Flush����
*/
void	CUserPane::FlushControl()
{
	ALocalRect	rect;
	GetControlLocalFrameRect(rect);
	FlushRect(rect);
}

#pragma mark ===========================

#pragma mark ---�`��p���擾

//�e�L�X�g�̈ʒu����`�掞��X���W�𓾂�
ANumber	CUserPane::GetImageXByTextPosition( CTextDrawData& inTextDrawData, const AIndex inTextPosition )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = ::GetDC(GetHWnd());
	}
	else
	{
		if( mOffscreenDC == NULL )
		{
			PrepareOffscreen();
		}
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return 0;}
	
	//�t�H���g�ݒ�
	::SelectObject(hdc,GetHFont(0));
	::SetBkMode(hdc,TRANSPARENT);
	::SetTextAlign(hdc,TA_UPDATECP);
	//Unicode����offset�擾
	AItemCount	offset = 0;
	if( inTextPosition < inTextDrawData.OriginalTextArray_UnicodeOffset.GetItemCount() )
	{
		offset = inTextDrawData.OriginalTextArray_UnicodeOffset.Get(inTextPosition);
	}
	else
	{
		offset = inTextDrawData.OriginalTextArray_UnicodeOffset.Get(inTextDrawData.OriginalTextArray_UnicodeOffset.GetItemCount()-1);
	}
	ANumber	textWidth = 0;
	{
		AStTextPtr	utf16textptr(inTextDrawData.UTF16DrawText,0,inTextDrawData.UTF16DrawText.GetItemCount());
		if( inTextDrawData.attrPos.GetItemCount() > 0 )
		{
			//attrPos, curPos, len��Unicode�����P��
			//UTF16DrawText��AText(�o�C�g�P��)
			AIndex	curPos = 0;//#435 attrPos�̏��Ԃ�����ւ�����Ƃ��̖h�~�̂��߁AcurPos�łǂ��܂ŕ\�����������Ǘ�����
			for( AIndex i = 0; i < inTextDrawData.attrPos.GetItemCount(); i++ )
			{
				//�I�������Foffset�܂ł̕������i�̘a�j���v�Z����
				if( curPos >= offset )   break;
				
				//len�ɁA��������RunLength�́AUniChar�ł̒������i�[����B
				AItemCount	len;
				if( i+1 < inTextDrawData.attrPos.GetItemCount() )
				{
					len = inTextDrawData.attrPos.Get(i+1) - curPos;//#435 inTextDrawData.attrPos.Get(i);
				}
				else
				{
					//for���[�v�Ō�Ńe�L�X�g�̍Ō�܂ł�K���\�����邱�Ƃ�ۏ؂��邽��
					len = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - curPos;//#435 inTextDrawData.attrPos.Get(i);
				}
				//len���e�L�X�g�̒������������Ȃ�Ȃ��悤�ɕ␳ #435
				if( curPos + len > inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) )
				{
					len = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - curPos;
				}
				//
				if( len <= 0 )   continue;
				
				//���̈ʒu��offset�ȍ~�Ȃ�offset�ʒu�܂ł̕��������v�Z����悤�ɂ���
				if( curPos+len >= offset )   len = offset-curPos;
				
				//style�w��
				ATextStyle	style = (inTextDrawData.attrStyle.Get(i)&0x07);
				::SelectObject(hdc,GetHFont(style));
				//�������擾(curPos����len�̒������j
				SIZE	textsize = {1,1};
				::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()+curPos*sizeof(AUTF16Char)),len,&textsize);
				textWidth += textsize.cx;
				//curPos�ړ� #435
				curPos += len;
			}
		}
		else
		{
			SIZE	textsize = {1,1};
			::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),offset,&textsize);
			textWidth = textsize.cx;
		}
	}
	//
	if( mOffscreenMode == false )
	{
		::ReleaseDC(GetHWnd(),hdc);
	}
	return textWidth;
}

//�`�掞��X���W����e�L�X�g�̈ʒu�𓾂�
AIndex	CUserPane::GetTextPositionByImageX( CTextDrawData& inTextDrawData, const ANumber inImageX )
{
	//AStTextPtr	utf16textptr(inTextDrawData.UTF16DrawText,0,inTextDrawData.UTF16DrawText.GetItemCount());
	AItemCount	count = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char);
	if( count == 0 )   return 0;
	
	ANumber	prevw = 0;
	for( AIndex i = 1; i <= count; i++ )
	{
		ANumber w = GetImageXByTextPosition(inTextDrawData,inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(i));
		if( w > inImageX )
		{
			if( (prevw+w)/2 < inImageX )
			{
				return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(i);
			}
			else
			{
				return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(i-1);
			}
		}
		prevw = w;
	}
	return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(count);
	/*
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = ::GetDC(GetHWnd());
	}
	else
	{
		if( mOffscreenDC == NULL )
		{
			PrepareOffscreen();
		}
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return 0;}
	
	//�t�H���g�ݒ�
	::SelectObject(hdc,GetHFont(0));
	//
	AStTextPtr	utf16textptr(inTextDrawData.UTF16DrawText,0,inTextDrawData.UTF16DrawText.GetItemCount());
	AItemCount	count = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char);
	if( count == 0 )   return 0;
	
	ANumber	prevw = 0;
	for( AIndex i = 1; i <= count; i++ )
	{
		SIZE	textsize = {1,1};
		::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),i,&textsize);
		ANumber	w = textsize.cx;
		if( w > inImageX )
		{
			if( (prevw+w)/2 < inImageX )
			{
				return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(i);
			}
			else
			{
				return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(i-1);
			}
		}
		prevw = w;
	}
	//
	if( mOffscreenMode == false )
	{
		::ReleaseDC(GetHWnd(),hdc);
	}
	return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(count);
	*/
}

AFloatNumber	CUserPane::GetDrawTextWidth( const AText& inText )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = ::GetDC(GetHWnd());
	}
	else
	{
		if( mOffscreenDC == NULL )
		{
			PrepareOffscreen();
		}
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return 0;}
	
	//�t�H���g�ݒ�
	::SelectObject(hdc,GetHFont(mStyle));
	//
	ANumber	textWidth = 1;
	AText	utf16text;
	utf16text.SetText(inText);
	utf16text.ConvertFromUTF8ToAPIUnicode();
	{
		AStTextPtr	utf16textptr(utf16text,0,utf16text.GetItemCount());
		SIZE	textsize = {1,1};
		::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),static_cast<int>(utf16textptr.GetByteCount()/sizeof(AUTF16Char)),&textsize);
		textWidth = textsize.cx;
	}
	//
	if( mOffscreenMode == false )
	{
		::ReleaseDC(GetHWnd(),hdc);
	}
	return textWidth;
}

AFloatNumber	CUserPane::GetDrawTextWidth( CTextDrawData& inTextDrawData )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = ::GetDC(GetHWnd());
	}
	else
	{
		if( mOffscreenDC == NULL )
		{
			PrepareOffscreen();
		}
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return 0;}
	
	//�t�H���g�ݒ�
	::SelectObject(hdc,GetHFont(mStyle));
	//
	ANumber	textWidth = 1;
	{
		AStTextPtr	utf16textptr(inTextDrawData.UTF16DrawText,0,inTextDrawData.UTF16DrawText.GetItemCount());
		SIZE	textsize = {1,1};
		::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),static_cast<int>(utf16textptr.GetByteCount()/sizeof(AUTF16Char)),&textsize);
		textWidth = textsize.cx;
	}
	//
	if( mOffscreenMode == false )
	{
		::ReleaseDC(GetHWnd(),hdc);
	}
	return textWidth;
}

//Draw���ׂ��̈�����ǂ����̔���
//mUpdateRect��CUserPane�������Őݒ肷��AOffscreen���̕`��X�V���ׂ��̈�BOS��WM_PAINT�C�x���g�̕`��̈�Ƃ͊֌W�Ȃ��B
ABool	CUserPane::IsRectInDrawUpdateRegion( const ALocalRect& inLocalRect ) const
{
	if(	mUpdateRect.right < inLocalRect.left  ||
		mUpdateRect.left  > inLocalRect.right ||
		mUpdateRect.bottom< inLocalRect.top   ||
		mUpdateRect.top   > inLocalRect.bottom )
	{
		return false;
	}
	else
	{
		return true;
	}
}

//
void	CUserPane::GetMetricsForDefaultTextProperty( ANumber& outLineHeight, ANumber& outLineAscent ) const
{
	HDC	hdc = ::GetDC(mHWnd);
	if( hdc == NULL )   {_ACError("",NULL);return;}
	
	::SelectObject(hdc,GetHFont(0));
	TEXTMETRIC	tm;
	::GetTextMetrics(hdc,&tm);
	outLineHeight = tm.tmHeight;
	outLineAscent = tm.tmAscent;
	
	::ReleaseDC(mHWnd,hdc);
}

#pragma mark ===========================

#pragma mark ---�X�N���[��

//Pane�S�̂��X�N���[������
void	CUserPane::Scroll( const ANumber inDeltaX, const ANumber inDeltaY )
{
	if( inDeltaX == 0 && inDeltaY == 0 )   return;

	ALocalRect	frame;
	GetControlLocalFrameRect(frame);
	
	//
	RECT	framerect, invalRect;
	framerect.left		= frame.left;
	framerect.top		= frame.top;
	framerect.right		= frame.right;
	framerect.bottom	= frame.bottom;
	if( mOffscreenMode == true )
	{
		if( mOffscreenDC == NULL )
		{
			PrepareOffscreen();
		}
		::ScrollDC(mOffscreenDC,inDeltaX,inDeltaY,&framerect,NULL,NULL,&invalRect);
		//���ۂ̃E�C���h�E��ʂ��X�N���[��
		HDC	hdc = ::GetDC(mHWnd);
		if( hdc != NULL )
		{
			RECT	updaterect;
			::ScrollDC(hdc,inDeltaX,inDeltaY,&framerect,NULL,NULL,&updaterect);
			::BitBlt(hdc,updaterect.left,updaterect.top,
					updaterect.right-updaterect.left,updaterect.bottom-updaterect.top,
					mOffscreenDC,updaterect.left,updaterect.top,SRCCOPY);
			::ReleaseDC(mHWnd,hdc);
		}
		else _ACError("",NULL);
		
		::SelectObject(mOffscreenDC,::GetStockObject(WHITE_PEN));
		::PatBlt(mOffscreenDC,invalRect.left,invalRect.top,invalRect.right-invalRect.left,invalRect.bottom-invalRect.top,PATCOPY);//��
		
		//�X�N���[���ŐV���ɕ`�悷�ׂ��ꏊ��`��
		mUpdateRect.left	= invalRect.left;
		mUpdateRect.top		= invalRect.top;
		mUpdateRect.right	= invalRect.right;
		mUpdateRect.bottom	= invalRect.bottom;
		DoDraw();
		
		//WM_PAINT�C�x���g�𔭐������āAOffscreen�S�Ă��E�C���h�E�֓]�ʂ���
		/* EVT_DoDraw()������TransferOffscreen()���Ă�
		::InvalidateRect(mHWnd,&framerect,FALSE);
		::UpdateWindow(mHWnd);*/
	}
	else
	{
		mPaintDC = ::GetDC(mHWnd);
		if( mPaintDC == NULL )   {_ACError("",this);return;}
		//
		RECT	updaterect;
		::ScrollDC(mPaintDC,inDeltaX,inDeltaY,&framerect,NULL,NULL,&invalRect);
		//
		mUpdateRect.left	= invalRect.left;
		mUpdateRect.top		= invalRect.top;
		mUpdateRect.right	= invalRect.right;
		mUpdateRect.bottom	= invalRect.bottom;
		DoDraw();
		//
		::ReleaseDC(mHWnd,mPaintDC);
	}
}

#pragma mark ===========================

#pragma mark ---Drag

void	CUserPane::DragText( const ALocalPoint& inMousePoint, const AArray<ALocalRect>& inDragRect, const AArray<AScrapType>& inScrapType, const ATextArray& inData )
{
	IDataObject*	pdataobject = new CDataObject();
	for( AIndex index = 0; index < inScrapType.GetItemCount(); index++ )
	{
		AScrapType	scrapType = inScrapType.Get(index);
		//��Unicode�ȊO�̃e�L�X�g�Ή������i�����Ń��K�V�[�ϊ����Ċi�[�H�j
		/*#688
		switch(scrapType)
		{
		  case kScrapType_UnicodeText:
		{
		*/
		FORMATETC	fmt;
		fmt.cfFormat = scrapType;
		fmt.ptd = NULL;
		fmt.dwAspect = DVASPECT_CONTENT;
		fmt.lindex = -1;
		fmt.tymed = TYMED_HGLOBAL;
		STGMEDIUM	stg;
		stg.tymed = TYMED_HGLOBAL;
		{
			//�e�L�X�g�G���R�[�f�B���O�^���s�R�[�h�ϊ��i���̊֐��ɂ����Ƃ��ɂ�UTF-8�j#688
			AText	utf16Text;
			utf16Text.SetText(inData.GetTextConst(index));
			utf16Text.ConvertFromUTF8ToUTF16();//#688 
			for( AIndex pos = 0; pos < utf16Text.GetItemCount(); pos += sizeof(AUTF16Char) )
			{
				AUTF16Char	ch = utf16Text.GetUTF16CharFromUTF16Text(pos);
				if( ch == 0x000D )
				{
					utf16Text.InsertUTF16Char(pos+sizeof(AUTF16Char),0x000A);
					pos += sizeof(AUTF16Char);
				}
			}
			//�ݒ�
			AStTextPtr	textptr(utf16Text,0,utf16Text.GetItemCount());
			stg.hGlobal = ::GlobalAlloc(GHND|GMEM_SHARE,textptr.GetByteCount()+sizeof(AUTF16Char));//�������NULL�I�[�����
			AUCharPtr	ptr = (AUCharPtr)::GlobalLock(stg.hGlobal);
			for( AIndex i = 0; i < textptr.GetByteCount(); i++ )
			{
				ptr[i] = (textptr.GetPtr())[i];
			}
			::GlobalUnlock(stg.hGlobal);
		}
		stg.pUnkForRelease = NULL;
		pdataobject->SetData(&fmt,&stg,TRUE);//���L����CDataObject�ֈړ�
			/*#688
				break;
			}
		  case kScrapType_Text:
			  {
				FORMATETC	fmt;
				fmt.cfFormat = scrapType;
				fmt.ptd = NULL;
				fmt.dwAspect = DVASPECT_CONTENT;
				fmt.lindex = -1;
				fmt.tymed = TYMED_HGLOBAL;
				STGMEDIUM	stg;
				stg.tymed = TYMED_HGLOBAL;
				{
					//���s�R�[�h�ϊ�
					AText	text;
					text.SetText(inData.GetTextConst(index));
					for( AIndex pos = 0; pos < text.GetItemCount(); pos++ )
					{
						AUChar	ch = text.Get(pos);
						if( ch == kUChar_CR )
						{
							text.Insert1(pos+1,kUChar_LF);
							pos++;
						}
					}
					//�ݒ�
					AStTextPtr	textptr(text,0,text.GetItemCount());
					stg.hGlobal = ::GlobalAlloc(GHND|GMEM_SHARE,textptr.GetByteCount()+sizeof(AUTF16Char));//�������NULL�I�[�����
					AUCharPtr	ptr = (AUCharPtr)::GlobalLock(stg.hGlobal);
					for( AIndex i = 0; i < textptr.GetByteCount(); i++ )
					{
						ptr[i] = (textptr.GetPtr())[i];
					}
					::GlobalUnlock(stg.hGlobal);
				}
				stg.pUnkForRelease = NULL;
				pdataobject->SetData(&fmt,&stg,TRUE);//���L����CDataObject�ֈړ�
				break;
			}
		  default:
			  {
				FORMATETC	fmt;
				fmt.cfFormat = scrapType;
				fmt.ptd = NULL;
				fmt.dwAspect = DVASPECT_CONTENT;
				fmt.lindex = -1;
				fmt.tymed = TYMED_HGLOBAL;
				STGMEDIUM	stg;
				stg.tymed = TYMED_HGLOBAL;
				{
					//�ݒ�
					AStTextPtr	textptr(inData.GetTextConst(index),0,inData.GetTextConst(index).GetItemCount());
					stg.hGlobal = ::GlobalAlloc(GHND|GMEM_SHARE,textptr.GetByteCount()+sizeof(AUTF16Char));//�������NULL�I�[�����
					AUCharPtr	ptr = (AUCharPtr)::GlobalLock(stg.hGlobal);
					for( AIndex i = 0; i < textptr.GetByteCount(); i++ )
					{
						ptr[i] = (textptr.GetPtr())[i];
					}
					::GlobalUnlock(stg.hGlobal);
				}
				stg.pUnkForRelease = NULL;
				pdataobject->SetData(&fmt,&stg,TRUE);//���L����CDataObject�ֈړ�
				break;
			  }
		}
	*/
	}
	IDropSource*	pdropsource = new CDropSource();
	//
	DWORD	pdweffect = 0;
	::DoDragDrop(pdataobject,pdropsource,DROPEFFECT_COPY|DROPEFFECT_MOVE,&pdweffect);
	//�I�u�W�F�N�g����i�Q�Ɖ񐔌��Z��delte�j
	pdataobject->Release();
	pdropsource->Release();
}

ABool	CUserPane::DragEnter( IDataObject* pDataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	//�p���N���X�Ŏ���
	return true;
}

ABool	CUserPane::DragOver( DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	//�p���N���X�Ŏ���
	return true;
}

ABool	CUserPane::DragLeave()
{
	//�p���N���X�Ŏ���
	return true;
}

ABool	CUserPane::Drop( IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	//�p���N���X�Ŏ���
	return true;
}

#pragma mark ===========================

#pragma mark --- �J�[�\��

/**
�J�[�\�����i����}�E�X�𓮂����܂Łj����
���݂̃J�[�\���ʒu���E�C���h�E���̏ꍇ�̂ݓK�p�����B
�E�C���h�E�̃^�C�g���o�[��Ȃǂɂ������ꍇ�ɁA�ēx�\������^�C�~���O���Ȃ����߁B
*/
void	CUserPane::ObscureCursor()
{
	POINT	pt;
	::GetCursorPos(&pt);
	ARect	rect;
	GetWin().GetWindowBounds(rect);
	if( pt.x >= rect.left && pt.x <= rect.right && pt.y >= rect.top && pt.y <= rect.bottom )
	{
		ACursorWrapper::ObscureCursor();
	}
}

#pragma mark ===========================

#pragma mark --- API�R�[���o�b�N

ABool	CUserPane::APICB_WndProc( UINT message, WPARAM wParam, LPARAM lParam, LRESULT& outResult )
{
	outResult = 0;
	ABool	handled = false;
	switch(message)
	{
		//�R�}���h���s
	  case WM_COMMAND:
		{
			if( lParam == 0 && mRedirectTextInput == true )
			{
				HWND	hwnd = AApplication::GetApplication().NVI_GetWindowByID(mRedirectWindowID).NVI_GetWindowRef();
						//win NVM_GetImp().GetHWnd();
				::PostMessage(hwnd, message, wParam, lParam);
				break;
			}
			//�R���g���[�����b�Z�[�W�����[�g�E�C���h�E�֑��M
			::PostMessage(::GetAncestor(mHWnd,GA_ROOT), message, wParam, lParam);
			handled = true;
			break;
		}
		//�E�C���h�E�`��
	  case WM_PAINT:
		{
			//OutputDebugStringA("StartWM_PAINTproc\n");
			mInWMPaint = true;
			AControlID	controlID = GetControlID();
			//Offscreen���R�s�[����i�t���[���S�̂��R�s�[�j
			//Offscreen���`��ς݂̏ꍇ�́A���������DoDraw()�̓R�[������Ȃ��B
			ALocalRect	frame;
			GetControlLocalFrameRect(frame);
			PAINTSTRUCT ps;
			mPaintDC = ::BeginPaint(mHWnd,&ps);
			if( mPaintDC == NULL )   {_ACError("",NULL);break;}
			if( mOffscreenMode == true )
			{
				if( mOffscreenDC == NULL )
				{
					MakeOffscreen(mPaintDC);
				}
				else if( mPreparingOffscreen == true )
				{
					//Offscreen�ւ̕`��
					ALocalRect	localFrame;
					GetControlLocalFrameRect(localFrame);
					mUpdateRect = localFrame;
					DoDraw();
					//Prepare��ԉ���
					mPreparingOffscreen = false;
				}
				//char	str[256];
				//sprintf(str,"WM_PAINT(UserPane) hwnd:%X left:%d right:%d \n",mHWnd,ps.rcPaint.left,ps.rcPaint.right);
				//OutputDebugStringA(str);
				
				RECT	framerect;
				framerect.left		= frame.left;
				framerect.top		= frame.top;
				framerect.right		= frame.right;
				framerect.bottom	= frame.bottom;
				//::BitBlt(hdc,framerect.left,framerect.top,framerect.right-framerect.left,framerect.bottom-framerect.top,
				//		mOffscreenDC,framerect.left,framerect.top,SRCCOPY);
				DoDrawPreProcess();
				
				//�ŏ��ɏ����i�j
				/*HBRUSH	hbr = ::CreateSolidBrush(GetWin().GetOverlayBackgroundColor());
				if( hbr == NULL )   {_ACError("",NULL);break;}
				::FillRect(hdc,&framerect,hbr);
				::DeleteObject(hbr);
				*/
				::BitBlt(mPaintDC,ps.rcPaint.left,ps.rcPaint.top,ps.rcPaint.right-ps.rcPaint.left,ps.rcPaint.bottom-ps.rcPaint.top,
						mOffscreenDC,ps.rcPaint.left,ps.rcPaint.top,SRCCOPY);
				DoDrawPostProcess();
			}
			else
			{
				ALocalRect	localFrame;
				GetControlLocalFrameRect(localFrame);
				mUpdateRect = localFrame;
				DoDrawPreProcess();
				DoDraw();
				DoDrawPostProcess();
			}
			::EndPaint(mHWnd,&ps);
			mInWMPaint = false;
			handled = true;
			//OutputDebugStringA("EndWM_PAINTproc\n");
			break;
		}
	  case WM_TIMER:
		{
			switch(wParam)
			{
			  case kTimer_ShowTooltip:
				{
					//�^�C�}�[�폜
					::KillTimer(GetHWnd(),kTimer_ShowTooltip);
					//�c�[���`�b�v�\��
					ShowTooltip();
					break;
				}
			}
			break;
		}
		//
	  case WM_DESTROY:
		{
			DestroyUserPane();
			break;
		}
		//�}�E�X�N���b�N
	  case WM_LBUTTONDOWN:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint�擾
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			//OverlayWindow�ւ̃��_�C���N�g���s���s�i�������s�����Ȃ�true�ŕԂ�j
			if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
			{
				handled = true;
				break;
			}
			//
			if( DoMouseDown(localPoint,AKeyWrapper::GetEventKeyModifiers(),1,false) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_RBUTTONDOWN:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint�擾
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			//OverlayWindow�ւ̃��_�C���N�g���s���s�i�������s�����Ȃ�true�ŕԂ�j
			if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
			{
				handled = true;
				break;
			}
			//
			if( DoMouseDown(localPoint,AKeyWrapper::GetEventKeyModifiers(),1,true) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_LBUTTONDBLCLK:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint�擾
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			//OverlayWindow�ւ̃��_�C���N�g���s���s�i�������s�����Ȃ�true�ŕԂ�j
			if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
			{
				handled = true;
				break;
			}
			//
			if( DoMouseDown(localPoint,AKeyWrapper::GetEventKeyModifiers(),2,false) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_RBUTTONDBLCLK:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint�擾
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			//OverlayWindow�ւ̃��_�C���N�g���s���s�i�������s�����Ȃ�true�ŕԂ�j
			if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
			{
				handled = true;
				break;
			}
			//
			if( DoMouseDown(localPoint,AKeyWrapper::GetEventKeyModifiers(),2,true) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_LBUTTONUP:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint�擾
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			if( mMouseTrackingMode == false )
			{
				//OverlayWindow�ւ̃��_�C���N�g���s���s�i�������s�����Ȃ�true�ŕԂ�j
				if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
				{
					handled = true;
					break;
				}
			}
			//
			if( DoMouseUp(localPoint,AKeyWrapper::GetEventKeyModifiers(),1,false) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_RBUTTONUP:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint�擾
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			if( mMouseTrackingMode == false )
			{
				//OverlayWindow�ւ̃��_�C���N�g���s���s�i�������s�����Ȃ�true�ŕԂ�j
				if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
				{
					handled = true;
					break;
				}
			}
			//
			if( DoMouseUp(localPoint,AKeyWrapper::GetEventKeyModifiers(),1,true) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
		//�}�E�X�ړ�
	  case WM_MOUSEMOVE:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint�擾
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			if( mMouseTrackingMode == false )
			{
				//OverlayWindow�ւ̃��_�C���N�g���s���s�i�������s�����Ȃ�true�ŕԂ�j
				if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
				{
					handled = true;
					break;
				}
			}
			//
			if( DoMouseMoved(localPoint,AKeyWrapper::GetEventKeyModifiers()) == true )
			{
				handled = true;
			}
			//
			//ACursorWrapper::ShowCursor();
			break;
		}
		//�}�E�X��client�̈悩��͂��ꂽ
	  case WM_MOUSELEAVE:
		{
			DoMouseLeft();
			ACursorWrapper::ShowCursor();
			break;
		}
		//�}�E�X�z�C�[����CWindowImp�ɗ���B
		//��������
	  case WM_CHAR:
		{
			AOSKeyEvent	keyevent;
			keyevent.message = message;
			keyevent.wParam = wParam;
			keyevent.lParam = lParam;
			//#688
			AKeyBindKey	keyBindKey = AKeyWrapper::GetKeyBindKey(keyevent);
			AModifierKeys	modifiers = AKeyWrapper::GetEventKeyModifiers(keyevent);
			//
			AText	text;
			AUTF16Char	ch = wParam;
			//���䕶���`�F�b�N
			if( ch >= 0x00 && ch <= 0x1F )
			{
				ABool	canhandle = false;
				switch(ch)
				{
				  case kUChar_Tab:
				  case kUChar_Escape:
				  case '\r':
				  case 8:
					/*�����L�[��WM_CHAR�����Ȃ�
				  case 0x1C:
				  case 0x1D:
				  case 0x1E:
				  case 0x1F:
					*/
					{
						canhandle = true;
						break;
					}
				}
				if( canhandle == false )   break;
			}
			//
			if( ch == kUChar_Backslash )
			{
				if( GetWin().IsInputBackslashByYenKeyMode() == false )
				{
					ch = 0x00A5;
				}
			}
			//text�ݒ�
			text.InsertFromUTF16TextPtr(0,&ch,sizeof(AUTF16Char));
			ABool	updatemenu = false;
			if( mRedirectTextInput == true )
			{
				if( AApplication::GetApplication().NVI_GetWindowByID(mRedirectWindowID).
							EVT_DoTextInput(mRedirectControlID,text,/*#688 keyevent*/keyBindKey,modifiers,updatemenu) == true )
				{
					::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
					handled = true;
					AApplication::GetApplication().NVI_UpdateMenu();
					break;
				}
			}
			else
			{
				if( GetAWin().EVT_DoTextInput(GetControlID(),text,/*#688 keyevent*/keyBindKey,modifiers,updatemenu) == true )
				{
					::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
					handled = true;
					AApplication::GetApplication().NVI_UpdateMenu();
					break;
				}
			}
			break;
		}
		//�L�[����
		//WM_KEYDOWN�̂ق���raw�C�x���g�B
		//�ꕔ�L�[�����A���̌�ɁAWM_CHAR������BWM_CHAR�����邱�Ȃ��́AWM_KEYDOWN�̏����̎d���͉e�����Ȃ��B
		//�iWM_KEYDOWN�̕Ԃ�l��0�ɂ��āADefWindowProc()���R�[�����Ȃ��ꍇ�ł��AWM_CHAR�͗���B�j
	  case WM_KEYDOWN:
		{
			AOSKeyEvent	keyevent;
			keyevent.message = message;
			keyevent.wParam = wParam;
			keyevent.lParam = lParam;
			//���j���[�V���[�g�J�b�g�iWindows�̏ꍇ�A���j���[�ɃV���[�g�J�b�g��o�^�ł��Ȃ��i�\���̂݁j�BAccelerator���Œ�B����āA���I���j���[�̃V���[�g�J�b�g�L�[�̓\�t�g���䂷��B�j
			ANumber	shortcut = 0;
			switch(wParam)
			{
			  case 0x41:
			  case 0x42:
			  case 0x43:
			  case 0x44:
			  case 0x45:
			  case 0x46:
			  case 0x47:
			  case 0x48:
			  case 0x49:
			  case 0x4A:
			  case 0x4B:
			  case 0x4C:
			  case 0x4D:
			  case 0x4E:
			  case 0x4F:
			  case 0x50:
			  case 0x51:
			  case 0x52:
			  case 0x53:
			  case 0x54:
			  case 0x55:
			  case 0x56:
			  case 0x57:
			  case 0x58:
			  case 0x59:
			  case 0x5A:
				{
					shortcut = wParam;
					break;
				}
			  case '0':
			  case '1':
			  case '2':
			  case '3':
			  case '4':
			  case '5':
			  case '6':
			  case '7':
			  case '8':
			  case '9':
				{
					shortcut = wParam;
					break;
				}
			  case VK_OEM_1://0xBA
				{
					shortcut = ':';
					break;
				}
			  case VK_OEM_PLUS://0xBB
				{
					shortcut = ';';
					break;
				}
			  case VK_OEM_MINUS://0xBD
				{
					shortcut = '-';
					break;
				}
			  case VK_OEM_7://0xDE
				{
					shortcut = '^';
					break;
				}
			  case VK_OEM_5://0xDC
				{
					shortcut = '\\';
					break;
				}
			  case VK_OEM_3://0xC0
				{
					shortcut = '@';
					break;
				}
			  case VK_OEM_4://0xDB
				{
					shortcut = '[';
					break;
				}
			  case VK_OEM_6://0xDD
				{
					shortcut = ']';
					break;
				}
			  case VK_OEM_COMMA://0xBC
				{
					shortcut = ',';
					break;
				}
			  case VK_OEM_PERIOD://0xBE
				{
					shortcut = '.';
					break;
				}
			  case VK_OEM_2://0xBF
				{
					shortcut = '/';
					break;
				}
			}
			if( shortcut != 0 )
			{
				//���I���j���[�̃V���[�g�J�b�g����
				AModifierKeys	modifiers = AKeyWrapper::GetEventKeyModifiers(keyevent);
				AMenuShortcutModifierKeys	menumodifiers = AKeyWrapper::MakeMenuShortcutModifierKeys(
											(AKeyWrapper::IsControlKeyPressed(modifiers) == true),
											(AKeyWrapper::IsCommandKeyPressed(modifiers) == true),
											(AKeyWrapper::IsShiftKeyPressed(modifiers) == true));
				AMenuItemID	menuItemID = 0;
				AText	actiontext;
				if( ADynamicMenu::FindShortcut(shortcut,menumodifiers,menuItemID,actiontext) == true )
				{
					//Win�Ŏ��s���s
					if( GetAWin().EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
					{
						AApplication::GetApplication().NVI_UpdateMenu();
						::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
						break;
					}
					//App�Ŏ��s���s
					else if( AApplication::GetApplication().EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
					{
						AApplication::GetApplication().NVI_UpdateMenu();
						::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
						break;
					}
				}
			}
			//
			if( AKeyWrapper::GetKeyBindKey(keyevent) != kKeyBindKey_Invalid )
			{
				AText	text;
				//�e�L�X�g�ݒ�
				//�J�[�\���L�[��WM_CHAR�ɂ��Ȃ��̂ŁAkeyevent�ł͂Ȃ�text�ŏ�������view�ɑ΂���(AView_EditBox���j�Atext��ݒ肷��K�v������B
				//delete�L�[�����l
				switch(wParam)
				{
				  case VK_UP:
					{
						text.Add(0x1E);
						break;
					}
				  case VK_DOWN:
					{
						text.Add(0x1F);
						break;
					}
				  case VK_LEFT:
					{
						text.Add(0x1C);
						break;
					}
				  case VK_RIGHT:
					{
						text.Add(0x1D);
						break;
					}
				  case VK_DELETE:
					{
						text.Add(0x7F);
						break;
					}
				}
				//
				ABool	updatemenu = false;
				if( mRedirectTextInput == true )
				{
					if( AApplication::GetApplication().NVI_GetWindowByID(mRedirectWindowID).
								EVT_DoTextInput(mRedirectControlID,text,/*#688 keyevent*/keyBindKey,modifiers,updatemenu) == true )
					{
						::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
						handled = true;
						AApplication::GetApplication().NVI_UpdateMenu();
					}
					else
					{
						handled = false;
					}
				}
				else
				{
					if( GetAWin().EVT_DoTextInput(GetControlID(),text,/*#688 keyevent*/keyBindKey,modifiers,updatemenu) == true )
					{
						::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
						handled = true;
						AApplication::GetApplication().NVI_UpdateMenu();
					}
					else
					{
						handled = false;
					}
				}
			}
			//handle�����ꍇ�AWM_CHAR���s���Ȃ��悤�ɂ���
			if( handled == true )
			{
				CAppImp::DisableNextTranslateMessage();
			}
			break;
		}
		//IME
	  case WM_IME_COMPOSITION:
		{
			char	buf[4096];
			HIMC	hImc = ::ImmGetContext(mHWnd);
			AText	comp;
			//if( (lParam&GCS_COMPSTR) != 0 )
			{
				LONG	bytecount = ::ImmGetCompositionStringW(hImc,GCS_COMPSTR,buf,4096);
				comp.InsertFromUTF16TextPtr(0,(AUTF16CharPtr)buf,bytecount);
			}
			AText	fix;
			//if( (lParam&GCS_RESULTSTR) != 0 )
			{
				LONG	bytecount = ::ImmGetCompositionStringW(hImc,GCS_RESULTSTR,buf,4096);
				fix.InsertFromUTF16TextPtr(0,(AUTF16CharPtr)buf,bytecount);
			}
			AText	inputUTF8Text;
			inputUTF8Text.AddText(fix);
			inputUTF8Text.AddText(comp);
			AItemCount	fixlen = fix.GetItemCount();
			comp.GetUTF16TextIndexInfo(mCurrentInlineInputUTF16PosIndexArray,mCurrentInlineInputUTF8PosIndexArray);
			AArray<AIndex>	hiliteLineStyleIndex;
			AArray<AIndex>	hiliteStartPos;
			AArray<AIndex>	hiliteEndPos;
			AArray<AColor>	hiliteColor;
			{
				LONG	clause[1024];
				LONG	clausebytecount = ::ImmGetCompositionStringW(hImc,GCS_COMPCLAUSE,clause,1024*sizeof(clause));
				char	attr[4096];
				LONG	attrbytecount = ::ImmGetCompositionStringW(hImc,GCS_COMPATTR,attr,4096);
				AItemCount	inputing = false;
				AItemCount	clausecount = clausebytecount/sizeof(LONG);
				for( AIndex i = 0; i < clausecount-1; i++ )
				{
					AIndex	spos = clause[i];
					AIndex	epos = clause[i+1];
					if( spos >= 4096 || epos >= 4096 )   break;
					AIndex	style = 0;
					switch(attr[spos])
					{
					  case ATTR_INPUT:
						{
							style = 1;
							inputing = true;
							break;
						}
					  case ATTR_TARGET_CONVERTED:
						{
							style = 0;
							break;
						}
					  case ATTR_CONVERTED:
						{
							style = 2;
							break;
						}
					  case ATTR_TARGET_NOTCONVERTED:
						{
							style = 3;
							break;
						}
					  default:
						{
							style = 4;
							break;
						}
					}
					/*
					AText	t;
					t.SetFormattedCstring("(%d)",style);
					t.OutputToStderr();
					*/
					hiliteLineStyleIndex.Add(style);
					hiliteStartPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(spos)+fixlen);
					hiliteEndPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(epos)+fixlen);
					//������+fixlen�ɂ��āF
					//Windows�̏ꍇ�A�ϊ��シ���Ɏ��̕������͂����ꍇ�ɁAfixlen=�O��̕ϊ������̒����A
					//spos,epos=����̕ϊ��J�n����̃I�t�Z�b�g�ŗ���B
					//Mac�̏ꍇ�Ə��������킹�邽�߂ɁA�����Œ�������
				}
				//�L�����b�g�ʒu�ݒ�
				if( inputing == true )
				{
					LONG	utf16pos = ::ImmGetCompositionStringW(hImc,GCS_CURSORPOS,NULL,0);
					/*
					AText	t;
					t.SetFormattedCstring("%X %d %d %d\n",lParam,wParam,utf16pos,
							::ImmGetCompositionStringW(hImc,GCS_DELTASTART,NULL,0));
					t.OutputToStderr();
					*/
					if( utf16pos < mCurrentInlineInputUTF8PosIndexArray.GetItemCount() )
					{
						hiliteLineStyleIndex.Add(kIndex_Invalid);
						hiliteStartPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(utf16pos)+fixlen);
						hiliteEndPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(utf16pos)+fixlen);
					}
				}
			}
			::ImmReleaseContext(mHWnd,hImc);
			ABool	updateMenu = false;
			if( GetAWin().EVT_DoInlineInput(GetControlID(),inputUTF8Text,fixlen,
							hiliteLineStyleIndex,hiliteColor,hiliteStartPos,hiliteEndPos,0,updateMenu) == true )
			{
				::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
			}
			if( updateMenu == true )
			{
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			handled = true;
			break;
		}
	  case WM_IME_STARTCOMPOSITION:
		{
			//���b�� �^�C�~���O�����K�v
			ALocalPoint	localPoint;
			if( GetAWin().EVT_DoInlineInputOffsetToPos(GetControlID(),0,localPoint) == true )
			{
				HIMC	hImc = ::ImmGetContext(mHWnd);
				CANDIDATEFORM	cf;
				cf.dwIndex = 0;//���b�� �v����
				cf.dwStyle = CFS_CANDIDATEPOS;
				cf.ptCurrentPos.x = localPoint.x;
				cf.ptCurrentPos.y = localPoint.y+3;
				::ImmSetCandidateWindow(hImc,&cf);
				::ImmReleaseContext(mHWnd,hImc);
			}
			handled = true;
			break;
		}
	  case WM_IME_ENDCOMPOSITION:
		{
			handled = true;
			break;
		}
		//IME����
		/*case WM_IME_CHAR:
		{
		AOSKeyEvent	keyevent;
		keyevent.message = message;
		keyevent.wParam = wParam;
		keyevent.lParam = lParam;
		AText	text;
		AUTF16Char	ch = wParam;
		text.InsertFromUTF16TextPtr(0,&ch,sizeof(AUTF16Char));
		ABool	updatemenu = false;
		if( GetAWin().EVT_DoTextInput(GetControlID(),text,keyevent,updatemenu) == true )
		{
		handled = true;
		AApplication::GetApplication().NVI_UpdateMenu();
		break;
		}
		break;
		}*/
		//
	  case WM_SIZE:
		{
			//debug
			//char	str[256];
			//sprintf(str,"WM_SIZE(UserPane) hwnd:%X wParam:%d x:%d y:%d\n",
			//		mHWnd,wParam,GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
			//OutputDebugStringA(str);
			if( mOffscreenMode == true )
			{
				//�V�K�T�C�Y�擾
				ANumber	newWidth =  GET_X_LPARAM(lParam);
				ANumber	newHeight = GET_Y_LPARAM(lParam);
				if( newWidth == 0 )   break;
				if( newHeight == 0 )   break;
				//���݂�offscreen�̃T�C�Y�Ɠ����Ȃ牽�����Ȃ�
				if( newWidth == mOffscreenWidth && newHeight == mOffscreenHeight )   break;
				
				//
				DoControlBoundsChanged();
				//�V�K�T�C�Y��offscreen�č쐬
				if( mPreparingOffscreen == true )
				{
					DisposeOffscreen();
					PrepareOffscreen();
				}
				else
				{
					HDC	hdc = ::GetDC(mHWnd);
					if( hdc == NULL )   {_ACError("",NULL);return false;}
					if( mOffscreenDC != NULL )
					{
						if( IsControlVisible() == true )
						{
							MakeOffscreen(hdc);
						}
						else
						{
							//��\�����Ȃ�Offscreen���폜�B������WM_PAINT�ōX�V�B
							//MakeOffscreen()��Offscreen���쐬���Ȃ����iEVT_DoDraw()���Ă΂��j���A
							//EVT_DoDraw()�̓R���g���[���\�����ȊO�͉������Ȃ��̂ŁB
							DisposeOffscreen();
						}
					}
					::ReleaseDC(mHWnd,hdc);
				}
			}
			break;
		}
	  case WM_HSCROLL:
		{
			::PostMessage(::GetAncestor(mHWnd,GA_ROOT), message, wParam, lParam);
			return 0L;
			break;
		}
	  case WM_VSCROLL:
		{
			::PostMessage(::GetAncestor(mHWnd,GA_ROOT), message, wParam, lParam);
			return 0L;
			break;
		}
	  case WM_SETCURSOR:
		{
			/*Tracking Tooltip�Ή���AAView_List�ŃJ�[�\������������ۂ����������̂ŁA�����̓R�����g�A�E�g�B
			WM_SETCURSOR��handled=true�ŕԂ��Ȃ��ƁA�e�L�X�g�G���A�ł����
			::ACursorWrapper::SetCursor(mDefaultCursorType);
			*/
			handled = true;
			break;
		}
	  case WM_KILLFOCUS:
		{
			FixInlineInput();
			ACursorWrapper::ShowCursor();
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

#pragma mark ===========================

#pragma mark --- IME

/**
IME�̋����m��
*/
void	CUserPane::FixInlineInput()
{
	//AView���̖��m��e�L�X�g�����ׂĊm�肳����
	AArray<AIndex>	hiliteLineStyleIndex;
	AArray<AIndex>	hiliteStartPos;
	AArray<AIndex>	hiliteEndPos;
	ABool	updateMenu = false;
	if( GetAWin().EVT_DoInlineInput(GetControlID(),GetEmptyText(),kIndex_Invalid,
					hiliteLineStyleIndex,hiliteStartPos,hiliteEndPos,updateMenu) == true )
	{
		//::UpdateWindow(mHWnd);//���ꂪ�Ȃ��ƃL�[�{�[�h���͑��x�i���s�[�g�����j���������Ԃ̂Ƃ��ɁA�`��ł��Ȃ��Ȃ�
	}
	if( updateMenu == true )
	{
		AApplication::GetApplication().NVI_UpdateMenu();
	}
	//OS����IME���͂��L�����Z��
	HIMC	hImc = ::ImmGetContext(mHWnd);
	::ImmNotifyIME(hImc,NI_COMPOSITIONSTR,CPS_CANCEL,0);
	::ImmReleaseContext(mHWnd,hImc);
}

#pragma mark ===========================

#pragma mark --- API�R�[���o�b�N(static)

LRESULT CALLBACK CUserPane::STATIC_APICB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-PRC)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return 0;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	
	//hWnd�ɑΉ�����CWindowImp�I�u�W�F�N�g������
	AIndex	index = sAliveUserPaneArray_HWnd.Find(hWnd);
	if( index == kIndex_Invalid )
	{
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	//�C���X�^���X�̃R�[���o�b�N���\�b�h���Ăяo��
	LRESULT	result = 0;
	try
	{
		if( (sAliveUserPaneArray.Get(index))->APICB_WndProc(message,wParam,lParam,result) == false )
		{
			result = ::DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_WndProc() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-PRC]");
	return result;
}


#pragma mark ===========================
#pragma mark [�N���X]CDropTarget
#pragma mark ===========================
//COM Drag&Drop�����N���X �i�h���b�O��I�u�W�F�N�g�j
//�Q�l�Fhttp://hp.vector.co.jp/authors/VA016117/oledragdrop2.html 

CDropTarget::CDropTarget( CUserPane& inUserPane ): mRefCount(1), mUserPane(inUserPane)
{
}

CDropTarget::~CDropTarget()
{
}

HRESULT __stdcall CDropTarget::QueryInterface( const IID& iid, void** ppv )
{
	HRESULT hr;
	
	if( iid == IID_IDropTarget || iid == IID_IUnknown )
	{
		hr = S_OK;
		*ppv = (void*)this;
		AddRef();
	}
	else
	{
		hr = E_NOINTERFACE;
		*ppv = 0;
	}
	return hr;
}

ULONG __stdcall CDropTarget::AddRef()
{
	::InterlockedIncrement(&mRefCount);
	return (ULONG)mRefCount;
}

ULONG __stdcall CDropTarget::Release()
{
	ULONG ret = (ULONG)::InterlockedDecrement(&mRefCount);
	if( ret == 0 )
	{
		delete this;
	}
	return (ULONG)mRefCount;
}

HRESULT __stdcall CDropTarget::DragEnter( IDataObject* pDataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	//��M�\�f�[�^���܂܂�Ă��邩�ǂ����̃`�F�b�N
	if( ExistAcceptableData(pDataObject) == false )
	{
		*pdwEffect = DROPEFFECT_NONE;
		return E_UNEXPECTED;
	}
	
	//
	if( mUserPane.DragEnter(pDataObject,grfKeyState,ptl,pdwEffect) == true )
	{
		return S_OK;
	}
	else
	{
		return E_UNEXPECTED;
	}
}

HRESULT __stdcall CDropTarget::DragOver( DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	if( mUserPane.DragOver(grfKeyState,ptl,pdwEffect) == true )
	{
		return S_OK;
	}
	else
	{
		return E_UNEXPECTED;
	}
}

HRESULT __stdcall CDropTarget::DragLeave()
{
	if( mUserPane.DragLeave() == true )
	{
		return S_OK;
	}
	else
	{
		return E_UNEXPECTED;
	}
}

HRESULT __stdcall CDropTarget::Drop( IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	//��M�\�f�[�^���܂܂�Ă��邩�ǂ����̃`�F�b�N
	if( ExistAcceptableData(pDataObject) == false )
	{
		*pdwEffect = DROPEFFECT_NONE;
		return E_UNEXPECTED;
	}
	
	//
	if( mUserPane.Drop(pDataObject,grfKeyState,pt,pdwEffect) == true )
	{
		return S_OK;
	}
	else
	{
		return E_UNEXPECTED;
	}
}

void	CDropTarget::AddAcceptableType( const AScrapType inAcceptableType )
{
	mAcceptableType.Add(inAcceptableType);
}

ABool	CDropTarget::ExistAcceptableData( IDataObject* inDataObject ) const
{
	for( AIndex i = 0; i < mAcceptableType.GetItemCount(); i++ )
	{
		FORMATETC	fmt;
		fmt.cfFormat = mAcceptableType.Get(i);
		fmt.ptd = NULL;
		fmt.dwAspect = DVASPECT_CONTENT;
		fmt.lindex = -1;
		fmt.tymed = TYMED_HGLOBAL;
		if( inDataObject->QueryGetData(&fmt) == S_OK )
		{
			return true;
		}
	}
	return false;
}

#pragma mark ===========================
#pragma mark [�N���X]CDropSource
#pragma mark ===========================
//COM Drag&Drop�����N���X �i�h���b�O���I�u�W�F�N�g�j
//�Q�l�Fhttp://hp.vector.co.jp/authors/VA016117/oledragdrop2.html 

CDropSource::CDropSource() : mRefCount(1)
{
}

CDropSource::~CDropSource()
{
}

HRESULT __stdcall CDropSource::QueryInterface( const IID& iid, void** ppv )
{
	HRESULT hr;
	
	if( iid == IID_IDropSource || iid == IID_IUnknown )
	{
		hr = S_OK;
		*ppv = (void*)this;
		AddRef();
	}
	else
	{
		hr = E_NOINTERFACE;
		*ppv = 0;
	}
	return hr;
}

ULONG __stdcall CDropSource::AddRef()
{
	::InterlockedIncrement(&mRefCount);
	return (ULONG)mRefCount;
}

ULONG __stdcall CDropSource::Release()
{
	ULONG ret = (ULONG)::InterlockedDecrement(&mRefCount);
	if( ret == 0 )
	{
		delete this;
	}
	return (ULONG)mRefCount;
}

HRESULT __stdcall CDropSource::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
	if( fEscapePressed == TRUE )
	{
		return DRAGDROP_S_CANCEL;
	}
	
	if( (grfKeyState & (MK_LBUTTON | MK_RBUTTON)) == 0 )
	{
		return DRAGDROP_S_DROP;
	}
	return S_OK;
}

HRESULT __stdcall CDropSource::GiveFeedback( DWORD dwEffect )
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

#pragma mark ===========================
#pragma mark [�N���X]CDataObject
#pragma mark ===========================
//COM Drag&Drop�����N���X�@�i�f�[�^���e�j

CDataObject::CDataObject() : mRefCount(1),mDataCount(0)
{
}

CDataObject::~CDataObject()
{
	for( AIndex i = 0; i < mDataCount; i++ )
	{
		::ReleaseStgMedium(&(mMedium[i]));
	}
}

HRESULT __stdcall CDataObject::QueryInterface( const IID& iid, void** ppv )
{
	HRESULT hr;
	
	if( iid == IID_IDataObject || iid == IID_IUnknown )
	{
		hr = S_OK;
		*ppv = (void*)this;
		AddRef();
	}
	else
	{
		hr = E_NOINTERFACE;
		*ppv = 0;
	}
	return hr;
}

ULONG __stdcall CDataObject::AddRef()
{
	::InterlockedIncrement(&mRefCount);
	return (ULONG)mRefCount;
}

ULONG __stdcall CDataObject::Release()
{
	ULONG ret = (ULONG)::InterlockedDecrement(&mRefCount);
	if(ret == 0)
	{
		delete this;
	}
	return (ULONG)mRefCount;
}

//�w��t�H�[�}�b�g�̃f�[�^�擾
HRESULT __stdcall	CDataObject::GetData( FORMATETC *pFormatetc, STGMEDIUM *pMedium )
{
	if( pFormatetc == NULL || pMedium == NULL ) return E_INVALIDARG;
	
	if( (pFormatetc->dwAspect&DVASPECT_CONTENT) == 0 )   return DV_E_DVASPECT;
	
	for( AIndex i = 0; i < mDataCount; i++ )
	{
		if( pFormatetc->cfFormat == mFormat[i].cfFormat &&
					(pFormatetc->tymed & mFormat[i].tymed) != 0 )
		{
			if( DuplicateMedium(pMedium,&(mFormat[i]),&(mMedium[i])) == false )
			{
				return E_OUTOFMEMORY;
			}
			else
			{
				return S_OK;
			}
		}
	}
	
	return DV_E_FORMATETC;
}

//�w��t�H�[�}�b�g�̃f�[�^���݃`�F�b�N
HRESULT __stdcall	CDataObject::QueryGetData( FORMATETC *pFormatetc )
{
	if( pFormatetc == NULL ) return E_INVALIDARG;
	
	if( (pFormatetc->dwAspect&DVASPECT_CONTENT) == 0 )   return DV_E_DVASPECT;
	
	for( AIndex i = 0; i < mDataCount; i++ )
	{
		if( pFormatetc->cfFormat == mFormat[i].cfFormat &&
					(pFormatetc->tymed & mFormat[i].tymed) != 0 )
		{
			return S_OK;
		}
	}
	
	return DV_E_FORMATETC;
}

//
HRESULT __stdcall	CDataObject::EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC **ppenumFormatetc )
{
	return S_OK;
}

//�f�[�^�ǉ�
HRESULT __stdcall	CDataObject::SetData( FORMATETC *pFormatetc, STGMEDIUM *pMedium, BOOL fRelease )
{
	//�����`�F�b�N
	if( pFormatetc == NULL || pMedium == NULL ) return E_INVALIDARG;
	
	//�f�[�^���`�F�b�N
	if( mDataCount >= kDataCountMax )   return E_OUTOFMEMORY;
	
	if( fRelease == TRUE )
	{
		//���L���͖{�C���X�^���X�Ɉړ�
		mFormat[mDataCount] = *pFormatetc;
		mMedium[mDataCount] = *pMedium;
	}
	else
	{
		//������medium�̓R�[���������L���ێ��A�{�C���X�^���X�փR�s�[
		mFormat[mDataCount] = *pFormatetc;
		DuplicateMedium(&(mMedium[mDataCount]),pFormatetc,pMedium);
	}
	mDataCount++;
	
	return S_OK;
}

HRESULT __stdcall	CDataObject::GetDataHere( FORMATETC *pFormatetc, STGMEDIUM *pMedium )
{
	return E_NOTIMPL;
}

HRESULT __stdcall	CDataObject::GetCanonicalFormatEtc( FORMATETC *pFormatetcIn, FORMATETC *pFormatetcOut )
{
	return E_NOTIMPL;
}

HRESULT __stdcall	CDataObject::DAdvise( FORMATETC *pFormatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection )
{
    return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT __stdcall	CDataObject::DUnadvise( DWORD dwConnection )
{
    return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT __stdcall	CDataObject::EnumDAdvise( IEnumSTATDATA **ppEnumAdvise )
{
    return OLE_E_ADVISENOTSUPPORTED;
}

//medium�f�[�^�R�s�[
ABool	CDataObject::DuplicateMedium( STGMEDIUM *pdest, const FORMATETC* pFormatetc, const STGMEDIUM *pMedium )
{
	switch(pMedium->tymed)
	{
	  case TYMED_HGLOBAL:
		{
			pdest->hGlobal = (HGLOBAL)::OleDuplicateData(pMedium->hGlobal, pFormatetc->cfFormat, (UINT)NULL);
			break;
		}
	  case TYMED_GDI:
		{
			pdest->hBitmap = (HBITMAP)::OleDuplicateData(pMedium->hBitmap, pFormatetc->cfFormat, (UINT)NULL);
			break;
		}
	  case TYMED_MFPICT:
		{
			pdest->hMetaFilePict = (HMETAFILEPICT)::OleDuplicateData(pMedium->hMetaFilePict, pFormatetc->cfFormat, (UINT)NULL);
			break;
		}
	  case TYMED_ENHMF:
		{
			pdest->hEnhMetaFile = (HENHMETAFILE)::OleDuplicateData(pMedium->hEnhMetaFile, pFormatetc->cfFormat, (UINT)NULL);
			break;
		}
	  case TYMED_FILE:
		{
			pdest->lpszFileName = (LPOLESTR)::OleDuplicateData(pMedium->lpszFileName, pFormatetc->cfFormat, (UINT)NULL);
			break;
		}
	  case TYMED_NULL:
		{
			break;
		}
	  case TYMED_ISTREAM:
	  case TYMED_ISTORAGE:
	  default:
		{
			return false;
			break;
		}
	}
	pdest->tymed = pMedium->tymed;
	pdest->pUnkForRelease = pMedium->pUnkForRelease;
	if( pMedium->pUnkForRelease != NULL )   pMedium->pUnkForRelease->AddRef();
	return true;
}





