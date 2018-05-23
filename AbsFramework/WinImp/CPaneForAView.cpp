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

CPaneForAView (Windows)

*/

#include "stdafx.h"

#include "CPaneForAView.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]CPaneForAView
#pragma mark ===========================
//AView�p�R���g���[��

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
CPaneForAView::CPaneForAView( AObjectArrayItem* inParent, CWindowImp& inWindow ) : CUserPane(inParent,inWindow)
		,mShowTooltipTimerWorking(false)
		,mEnableTrackingToolTip(false)
		,mHTooltip(NULL), mTooltipActive(false)
		,mShowToolTipOnlyWhenNarrow(true)//#507
		,mFixedTextTooltip_TagSide(kHelpTagSide_Default)//#643
{
	//DoMouseMoved�̈ړ��`�F�b�N�p
	mPreviousMousePoint.x = -1;
	mPreviousMousePoint.y = -1;
}

//�f�X�g���N�^
CPaneForAView::~CPaneForAView()
{
}

void	CPaneForAView::DestroyUserPane()
{
	//
	if( mHTooltip != NULL )
	{
		::DestroyWindow(mHTooltip);
		mHTooltip = NULL;
	}
	//
	CUserPane::DestroyUserPane();
}

#pragma mark ===========================

#pragma mark ---�C�x���g�n���h��

//�`��
ABool	CPaneForAView::DoDraw()
{
	GetAWin().EVT_DoDraw(GetControlID());
	return true;
}

void	CPaneForAView::DoDrawPreProcess()
{
	GetAWin().EVT_DrawPreProcess(GetControlID());
}

void	CPaneForAView::DoDrawPostProcess()
{
	GetAWin().EVT_DrawPostProcess(GetControlID());
}

//�}�E�X�z�C�[��
ABool	CPaneForAView::DoMouseWheel( const AFloatNumber inDeltaY, const AModifierKeys inModifiers, const ALocalPoint inLocalPoint )
{
//	if( IsControlVisible() == false )   return true;
	
	//�iAWindow���o�R���āj�Ή�����AView�̃C�x���g�n���h�������s 
	GetAWin().EVT_DoMouseWheel(GetControlID(),0,inDeltaY,inModifiers,inLocalPoint);
	//�\���X�V�iWM_PAINT�����j
	::UpdateWindow(GetHWnd());
	//Tooltip����
	if( GetEnableTrackingToolTip() == true )
	{
		//Tooltip�X�V
		UpdateTooltip(inLocalPoint);
	}
	return true;
}

//�}�E�X�N���b�N�ɑ΂��鏈��  
ABool	CPaneForAView::DoMouseDown( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers, 
		const ANumber inClickCount, const ABool inRightButton )
{
//�tⳎ������N���b�N����Ƃ�����false�ɂȂ�B������ɂ��Ă����̔���͕s�v�Ǝv����̂ŃR�����g�A�E�g�B	if( IsControlVisible() == false )   return true;
	
	//�E�N���b�N�Ȃ�AEVT_DoContextMenu()���R�[�����Ă݂� 
	if( inRightButton == true )
	{
		if( GetAWin().EVT_DoContextMenu(GetControlID(),inLocalPoint,inModifiers,inClickCount) == true )
		{
			return true;
		}
	}
	//�iAWindow���o�R���āj�Ή�����AView��EVT_DoMouseDown()�����s 
	if( GetAWin().EVT_DoMouseDown(GetControlID(),inLocalPoint,inModifiers,inClickCount) == false )
	{
		//
		if( inClickCount == 1 )
		{
			GetAWin().EVT_Clicked(GetControlID(),inModifiers);
		}
		else if( inClickCount == 2 )
		{
			GetAWin().EVT_Clicked(GetControlID(),inModifiers);//B0303 �E�C���h�E�A�N�e�B�x�[�g�N���b�N���P��ڂɂȂ��Ă��܂��AEVT_Clicked()���ĂׂȂ����Ƃ�����̂�
			GetAWin().EVT_DoubleClicked(GetControlID());
		}
		//char	str[256];
		//sprintf(str,"DoMouseDown:%d \n",GetControlID());
		//OutputDebugStringA(str);
	}
	//�iAWindow���o�R���āj�Ή�����AView��EVT_GetCursorType()�����s  
	ACursorWrapper::SetCursor(GetAWin().EVT_GetCursorType(GetControlID(),inLocalPoint,inModifiers));
	//�\���X�V�iWM_PAINT�����j
	::UpdateWindow(GetHWnd());
	return true;
}

ABool	CPaneForAView::DoMouseMoved( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers )
{
	if( IsControlVisible() == false )   return true;
	
	//�ړ��`�F�b�N�iTTF_TRACK��ݒ肵��Tooltip��\�����Ă���Ƃ��́A�}�E�X���ړ����Ă��Ȃ��Ă��R�[�������j
	if( mPreviousMousePoint.x == inLocalPoint.x && mPreviousMousePoint.y == inLocalPoint.y )
	{
		//�O��ƃ}�E�X�ʒu�����Ȃ牽���������^�[��
		return true;
	}
	mPreviousMousePoint.x = inLocalPoint.x;
	mPreviousMousePoint.y = inLocalPoint.y;
	
	//MouseTracking
	if( mMouseTrackingMode == true )
	{
		GetAWin().EVT_DoMouseTracking(GetControlID(),inLocalPoint,inModifiers);
		AApplication::GetApplication().NVI_UpdateMenu();
	}
	//��UserPane�ȊO�̑��̑S�Ă�Window�̑S�Ă�UserPane��DoMouseLeft()����B�itracking���Ȃ�j
	GetWin().TryMouseLeaveForAllWindowsExceptFor(GetControlID());
	//
	ABool	result = GetAWin().EVT_DoMouseMoved(GetControlID(),inLocalPoint,inModifiers);
	//�iAWindow���o�R���āj�Ή�����AView��EVT_GetCursorType()�����s 
	ACursorWrapper::SetCursor(GetAWin().EVT_GetCursorType(GetControlID(),inLocalPoint,inModifiers));
	//
	if( mMouseLeaveEventEnable == true && mMouseLeaveEventTracking == false )
	{
		mMouseLeaveEventTracking = true;
		//
		HWND	trackHWnd = GetHWnd();
		if( GetWin().IsOverlayWindow() == true )
		{
			//OverlayWindow�̏ꍇ�͐e�E�C���h�E����track����B�iOverlay�E�C���h�E�̓}�E�X���߂Ȃ̂ŁA��leave���Ă��܂��j
			trackHWnd = GetWin().GetParentWindowHWnd();
		}
		//
		TRACKMOUSEEVENT	trackmouse;
		trackmouse.cbSize = sizeof(trackmouse);
		trackmouse.dwFlags = TME_LEAVE;
		trackmouse.hwndTrack = trackHWnd;
		trackmouse.dwHoverTime = HOVER_DEFAULT;
		//TrackMouseEvent()���̂͂����ɔ�����B
		//����A�}�E�X���E�C���h�E�O�ֈړ�����ƁA��x�����AWM_MOUSELEAVE�����̃E�C���h�E�֑��M�����
		//�I�[�o�[���C�̏ꍇ�A���������̓E�C���h�E�O�Ɣ��肳���B
		//OutputDebugStringA("BeforeTrackMouseEvent\n");
		::TrackMouseEvent(&trackmouse);
		//OutputDebugStringA("AfterTrackMouseEvent\n");
	}
	//�}�E�X�J�[�\���\��
	ACursorWrapper::ShowCursor();
	//�\���X�V�iWM_PAINT�����j
	::UpdateWindow(GetHWnd());
	//Tooltip����
	if( GetEnableTrackingToolTip() == true )
	{
		//Tooltip�X�V
		UpdateTooltip(inLocalPoint);
	}
	return result;
}

void	CPaneForAView::DoMouseLeft()
{
	if( IsControlVisible() == false )   return;
	ALocalPoint	localPoint = {0,0};
	//
	GetAWin().EVT_DoMouseLeft(GetControlID(),localPoint);
	//Tooltip����
	if( GetEnableTrackingToolTip() == true )
	{
		//Tooltip��deactivate����
		HideTooltip();
	}
	//�ړ��`�F�b�N�����o�[�ϐ��N���A
	mPreviousMousePoint.x = -1;
	mPreviousMousePoint.y = -1;
	//
	mMouseLeaveEventTracking = false;
}

void	CPaneForAView::UpdateTooltip( const ALocalPoint inLocalPoint )
{
	//�e�L�X�g�擾
	ABool	show = true;
	AText	text;
	AText	text1, text2;
	ALocalRect	localRect;
	AHelpTagSide	tagSide = kHelpTagSide_Default;//#643
	if( mFixedTextTooltip.GetItemCount() > 0 )
	{
		text.SetText(mFixedTextTooltip);
		text2.SetText(text);
		GetControlLocalFrameRect(localRect);
	}
	else
	{
		GetAWin().EVT_DoGetHelpTag(GetControlID(),inLocalPoint,text1,text2,localRect,tagSide);
		text.SetText(text2);
		if( mShowToolTipOnlyWhenNarrow == true )//#507
		{
			ANumber	width = static_cast<ANumber>(GetDrawTextWidth(text));
			if( width < localRect.right-localRect.left )
			{
				show = false;
			}
		}
	}
	if( show == false )
	{
		HideTooltip();
	}
	else
	{
		//
		if( mCurrentTooltipText.Compare(text) == false ||
					mCurrentTooltipRect.left != localRect.left ||
					mCurrentTooltipRect.right != localRect.right ||
					mCurrentTooltipRect.top != localRect.top ||
					mCurrentTooltipRect.bottom != localRect.bottom )
		{
			//�c�[���`�b�v�E�C���h�E����
			if( mHTooltip == NULL )
			{
				mHTooltip = ::CreateWindowEx(0,TOOLTIPS_CLASS,NULL,TTS_ALWAYSTIP|TTS_NOPREFIX,
						CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
						GetHWnd(),NULL,CAppImp::GetHInstance(),NULL);
				//Tooltip�ݒ�
				//TTF_TRACK: Tracking���[�h
				//TTF_ABSOLUTE: �ݒ肵�Ȃ��ꍇ�R���g���[���O�ŕ\��
				RECT	rect;
				::GetClientRect(GetHWnd(),&rect);
				mTooltipToolinfo.cbSize = sizeof(mTooltipToolinfo);
				mTooltipToolinfo.hwnd		= GetHWnd();
				mTooltipToolinfo.rect		= rect;
				mTooltipToolinfo.hinst		= CAppImp::GetHResourceInstance();//satDLL
				mTooltipToolinfo.uFlags	= TTF_IDISHWND|TTF_TRACK|TTF_ABSOLUTE;
				mTooltipToolinfo.uId		= (UINT_PTR)GetHWnd();
				mTooltipToolinfo.lpszText	= L"";
				::SendMessageW(mHTooltip,TTM_ADDTOOLW,0,(LPARAM)&mTooltipToolinfo);
				mTooltipActive = false;
			}
			
			//�X�V�`�F�b�N�p�f�[�^�ݒ�
			mCurrentTooltipText.SetText(text);
			mCurrentTooltipRect = localRect;
			//�܂���\���ɂ���i�^�C�}�[�ŕ\�������邽�߁j
			::SendMessageW(mHTooltip,TTM_TRACKACTIVATE,(WPARAM)FALSE,(LPARAM)&mTooltipToolinfo);
			//�\���ʒu�擾
			//POINT	pt = {localRect.left,localRect.bottom+3};
			POINT	pt = { inLocalPoint.x+32, localRect.bottom+3};
			//#643
			switch(tagSide)
			{
			  case kHelpTagSide_Right:
				{
					pt.x = localRect.right + 3;
					pt.y = localRect.bottom + 3;
					break;
				}
			}
			//
			::ClientToScreen(GetHWnd(),&pt);
			//�ő啝�ݒ�
			AGlobalRect	screenBounds;
			CWindowImp::GetAvailableWindowPositioningBounds(screenBounds);
			::SendMessageW(mHTooltip,TTM_SETMAXTIPWIDTH,(WPARAM)0,
					(LPARAM)screenBounds.right-pt.x);
			//�t�H���g�ݒ�iUserPane�̃f�t�H���g�t�H���g��ݒ�j
			if( mFixedTextTooltip.GetItemCount() == 0 )//�Œ蕶����c�[���`�b�v�̂Ƃ��̓V�X�e���f�t�H���g�̃t�H���g���g�p����
			{
				::SendMessageW(mHTooltip,WM_SETFONT,(WPARAM)GetHFont(0),(LPARAM)FALSE);
			}
			//Tooltip�e�L�X�g�X�V
			AStCreateWcharStringFromAText	str(text2);
			mTooltipToolinfo.lpszText = (wchar_t*)str.GetPtr();
			::SendMessageW(mHTooltip,TTM_SETTOOLINFO,(WPARAM)0,(LPARAM)&mTooltipToolinfo);
			mTooltipToolinfo.lpszText = L"";
			//Tooltip�ʒu�ݒ�
			::SendMessageW(mHTooltip,TTM_TRACKPOSITION,0,(LPARAM)MAKELONG(pt.x,pt.y));
			//�\���^�C�}�[�N��
			::SetTimer(GetHWnd(),kTimer_ShowTooltip,750,NULL);
			mShowTooltipTimerWorking = true;
		}
	}
}

void	CPaneForAView::ShowTooltip()
{
	//if( mTooltipActive == true )   return;
	
	mTooltipToolinfo.lpszText = L"";//�O�̂���
	::SendMessageW(mHTooltip,TTM_TRACKACTIVATE,(WPARAM)TRUE,(LPARAM)&mTooltipToolinfo);
	mTooltipActive = true;
}

void	CPaneForAView::HideTooltip()
{
	//if( mTooltipActive == false )   return;
	
	mTooltipToolinfo.lpszText = L"";//�O�̂���
	::SendMessageW(mHTooltip,TTM_TRACKACTIVATE,(WPARAM)FALSE,(LPARAM)&mTooltipToolinfo);
	mTooltipActive = false;
	//�\���^�C�}�[����
	if( mShowTooltipTimerWorking == true )
	{
		//�^�C�}�[�폜
		::KillTimer(GetHWnd(),kTimer_ShowTooltip);
		mShowTooltipTimerWorking = false;
	}
	//�X�V�`�F�b�N�p�f�[�^�N���A
	mCurrentTooltipText.DeleteAll();
	mCurrentTooltipRect.left	= 0;
	mCurrentTooltipRect.top		= 0;
	mCurrentTooltipRect.right	= 0;
	mCurrentTooltipRect.bottom	= 0;
	//�c�[���`�b�v�E�C���h�E�폜
	if( mHTooltip != NULL )
	{
		::DestroyWindow(mHTooltip);
		mHTooltip = NULL;
	}
}

//�}�E�X�N���b�N�ɑ΂��鏈��  
ABool	CPaneForAView::DoMouseUp( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers, 
		const ANumber inClickCount, const ABool inRightButton )
{
	if( IsControlVisible() == false )   return true;
	
	//MouseTracking
	if( mMouseTrackingMode == true )
	{
		GetAWin().EVT_DoMouseTrackEnd(GetControlID(),inLocalPoint,inModifiers);
	}
	//�\���X�V�iWM_PAINT�����j
	::UpdateWindow(GetHWnd());
	return true;
}

void	CPaneForAView::DoControlBoundsChanged()
{
	GetAWin().EVT_DoControlBoundsChanged(GetControlID());
}

ABool	CPaneForAView::DragEnter( IDataObject* pDataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	POINT	pt;
	pt.x = ptl.x;
	pt.y = ptl.y;
	::ScreenToClient(GetHWnd(),&pt);
	ALocalPoint	localPoint;
	localPoint.x = pt.x;
	localPoint.y = pt.y;
	AModifierKeys	modifiers = 0;
	if( (grfKeyState&MK_SHIFT)==0 && (grfKeyState&MK_CONTROL)==0 && ((*pdwEffect)&DROPEFFECT_MOVE)!= 0 )
	{
		*pdwEffect = DROPEFFECT_MOVE;
	}
	else if( (grfKeyState&MK_SHIFT) != 0 )
	{
		modifiers |= kModifierKeysMask_Shift;
		*pdwEffect = DROPEFFECT_COPY;
	}
	else if( (grfKeyState&MK_CONTROL) != 0 )
	{
		modifiers |= kModifierKeysMask_Control;
		*pdwEffect = DROPEFFECT_COPY;
	}
	else
	{
		*pdwEffect = DROPEFFECT_COPY;
	}
	GetAWin().EVT_DoDragEnter(GetControlID(),NULL,localPoint,modifiers);
	return true;
}

ABool	CPaneForAView::DragOver( DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	POINT	pt;
	pt.x = ptl.x;
	pt.y = ptl.y;
	::ScreenToClient(GetHWnd(),&pt);
	ALocalPoint	localPoint;
	localPoint.x = pt.x;
	localPoint.y = pt.y;
	AModifierKeys	modifiers = 0;
	if( (grfKeyState&MK_SHIFT)==0 && (grfKeyState&MK_CONTROL)==0 && ((*pdwEffect)&DROPEFFECT_MOVE)!= 0 )
	{
		*pdwEffect = DROPEFFECT_MOVE;
	}
	else if( (grfKeyState&MK_SHIFT) != 0 )
	{
		modifiers |= kModifierKeysMask_Shift;
		*pdwEffect = DROPEFFECT_COPY;
	}
	else if( (grfKeyState&MK_CONTROL) != 0 )
	{
		modifiers |= kModifierKeysMask_Control;
		*pdwEffect = DROPEFFECT_COPY;
	}
	else
	{
		*pdwEffect = DROPEFFECT_COPY;
	}
	GetAWin().EVT_DoDragTracking(GetControlID(),NULL,localPoint,modifiers);
	return true;
}

ABool	CPaneForAView::DragLeave()
{
	ALocalPoint	localPoint;
	localPoint.x = 0;
	localPoint.y = 0;
	AModifierKeys	modifiers = 0;
	GetAWin().EVT_DoDragLeave(GetControlID(),NULL,localPoint,modifiers);
	return true;
}

ABool	CPaneForAView::Drop( IDataObject* pDataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	ADragRef	dummy = 0;
	POINT	pt;
	pt.x = ptl.x;
	pt.y = ptl.y;
	::ScreenToClient(GetHWnd(),&pt);
	ALocalPoint	localPoint;
	localPoint.x = pt.x;
	localPoint.y = pt.y;
	AModifierKeys	modifiers = 0;
	if( (grfKeyState&MK_SHIFT)==0 && (grfKeyState&MK_CONTROL)==0 && ((*pdwEffect)&DROPEFFECT_MOVE)!= 0 )
	{
		*pdwEffect = DROPEFFECT_MOVE;
	}
	else if( (grfKeyState&MK_SHIFT) != 0 )
	{
		modifiers |= kModifierKeysMask_Shift;
		*pdwEffect = DROPEFFECT_COPY;
	}
	else if( (grfKeyState&MK_CONTROL) != 0 )
	{
		modifiers |= kModifierKeysMask_Control;
		*pdwEffect = DROPEFFECT_COPY;
	}
	else
	{
		*pdwEffect = DROPEFFECT_COPY;
	}
	GetAWin().EVT_DoDragReceive(GetControlID(),pDataObject,localPoint,modifiers);
	return true;
}

#pragma mark ===========================

#pragma mark ---Tooltip

//#507
/**
*/
void	CPaneForAView::EnableTrackingToopTip( const ABool inShowToolTipOnlyWhenNarrow )
{
	mEnableTrackingToolTip = true;
	mShowToolTipOnlyWhenNarrow = inShowToolTipOnlyWhenNarrow;
}

void	CPaneForAView::SetFixedTextTooltip( const AText& inText, const AHelpTagSide inTagSide )//#643
{
	EnableTrackingToopTip();
	mFixedTextTooltip.SetText(inText);
	mFixedTextTooltip_TagSide = inTagSide;//#643
}

#pragma mark ===========================

#pragma mark ---�R���g���[���t�H�[�J�X����

//#135
/**
�t�H�[�J�X�ݒ莞��CWindowImp����R�[�������
*/
void	CPaneForAView::SetFocus()
{
	GetAWin().EVT_DoViewFocusActivated(GetControlID());
}

//#135
/**
�t�H�[�J�X����
*/
void	CPaneForAView::ResetFocus()
{
	GetAWin().EVT_DoViewFocusDeactivated(GetControlID());
}

