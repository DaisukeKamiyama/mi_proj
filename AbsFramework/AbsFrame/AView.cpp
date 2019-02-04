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

AView

*/

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AView
#pragma mark ===========================
//View

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView::AView( const AWindowID inWindowID, const AControlID inID, const AImagePoint& inOriginOfLocalFrame ) 
		: AObjectArrayItem(/*#199 inParent*/&(AApplication::GetApplication())), /*#199mWindow(inWindow),*/
		mWindowID(inWindowID), mControlID(inID), 
		mControlID_VScrollBar(kControlID_Invalid), mControlID_HScrollBar(kControlID_Invalid),
		mImageWidth(100), mImageHeight(100), mHScrollBarUnit(1), mVScrollBarUnit(1), mHScrollBarPageUnit(1), mVScrollBarPageUnit(1),
		/*#688 mFocusActive(false),*/ mOriginOfLocalFrame(inOriginOfLocalFrame)
		,mMouseWheelEventRedirectViewID(kObjectID_Invalid)
		,mBorrowerView(false)//#92
		,mAutomaticSelectBySwitchFocus(false)//#135
		,mCatchReturn(false)//#137
		,mCatchTab(false)//#137
		,mInputBackslashByYenKeyMode(false)//#182
		,mDataType(kDataType_Unknown)//#182
		,mViewImp(AApplication::GetApplication().NVI_GetWindowByID(inWindowID).NVI_GetViewImp(inID))//#271
		,mWindow(AApplication::GetApplication().NVI_GetWindowByID(inWindowID))//#271
		,mFrameViewControlID(kControlID_Invalid), mFrameLeftMargin(0), mFrameRightMargin(0), mFrameTopMargin(0), mFrameBottomMargin(0)//#135
		,mVScrollBarControlID(kControlID_Invalid),mHScrollBarControlID(kControlID_Invalid)//#135
		,mVScrollBarWidth(15), mHScrollBarHeight(15)
		,mMouseTrackingMode(false)//#688
,mImageYMinMargin(0), mImageYMaxMargin(0)//#621
,mDefaultCursor(kCursorType_Arrow)//#725
,mBackgroundTransparency(1.0), mBackgroundActiveColor(kColor_White), mBackgroundDeactiveColor(kColor_White)//#725
,mEnableHelpTag(true)
,mRefreshing(false)//#1034
,mContextMenuItemID(0)//#1380
{
}

//�f�X�g���N�^
AView::~AView()
{
}

//#92
/**
AObjectArrayItem��AObjectArray����폜��������ɃR�[�������B
�i�f�X�g���N�^��GC���Ȃ̂ŁA�^�C�~���O����ł��Ȃ��B����āA�e��폜������DoDeleted()�ōs���ׂ��B�j
*/
void	AView::DoDeleted()
{
	//FrameView�폜 #135
	if( ExistFrameView() == true )
	{
		DeleteFrameView();
	}
	//
	NVIDO_DoDeleted();
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
*/
ABool	AView::EVT_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	//#390
	if( AWindow::STATIC_NVI_GetTextInputRedirect_WindowID() != kObjectID_Invalid && 
				AWindow::STATIC_NVI_GetTextInputRedirect_WindowID() != mWindowID )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(AWindow::STATIC_NVI_GetTextInputRedirect_WindowID()).
		NVI_GetViewByControlID(AWindow::STATIC_NVI_GetTextInputRedirect_ControlID()).
		EVT_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys);
	}
	//
	return EVTDO_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys);
}

/**
*/
void	AView::EVT_UpdateMenu()
{
	//#688 �I����������UpdateMenu���Ȃ��悤�ɂ���
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return;
	
	//#390
	if( AWindow::STATIC_NVI_GetTextInputRedirect_WindowID() != kObjectID_Invalid && 
				AWindow::STATIC_NVI_GetTextInputRedirect_WindowID() != mWindowID )
	{
		AApplication::GetApplication().NVI_GetWindowByID(AWindow::STATIC_NVI_GetTextInputRedirect_WindowID()).
				NVI_GetViewByControlID(AWindow::STATIC_NVI_GetTextInputRedirect_ControlID()).EVT_UpdateMenu();
		return;
	}
	//
	EVTDO_UpdateMenu();
}

/**
�`��
��\������DoDraw()���s���Ȃ��B
*/
void	AView::EVT_DoDraw()
{
	//#1034
	//���t���b�V�����idraw�҂��j�t���OOFF
	//NVMC_RefreshRect(), NVMC_RefreshControl()��ON�ɂ����B�L�����b�g�`�掞�ɁAcontext flush�̗v�ۂ𔻒f���邽�ߎg�p����B
	mRefreshing = false;
	//win ��\�����ł�Offscreen�ւ̕`��\�ɂ��� if( NVI_IsVisible() == false )   return;//#0 ��\������draw����K�v���Ȃ�
	EVTDO_DrawPreProcess();
	EVTDO_DoDraw();
	NVMC_TransferOffscreen();
	EVTDO_DrawPostProcess();
}

//�X�N���[���o�[����
void	AView::EVT_DoScrollBarAction( const ABool inV, const AScrollBarPart inPart )
{
	if( inV )
	{
		switch(inPart)
		{
		  case kScrollBarPart_UpButton:
			{
				if( mViewImp.GetVerticalMode() == false )
				{
					NVI_Scroll(0,-NVI_GetVScrollBarUnit());
				}
				else
				{
					//�c�����̏ꍇ�͋t���� #558
					NVI_Scroll(0,NVI_GetVScrollBarUnit());
				}
				break;
			}
		  case kScrollBarPart_DownButton:
			{
				if( mViewImp.GetVerticalMode() == false )
				{
					NVI_Scroll(0,NVI_GetVScrollBarUnit());
				}
				else
				{
					//�c�����̏ꍇ�͋t���� #558
					NVI_Scroll(0,-NVI_GetVScrollBarUnit());
				}
				break;
			}
		  case kScrollBarPart_PageUpButton:
			{
				if( mViewImp.GetVerticalMode() == false )
				{
					NVI_Scroll(0,-NVI_GetVScrollBarPageUnit());
				}
				else
				{
					//�c�����̏ꍇ�͋t���� #558
					NVI_Scroll(0,NVI_GetVScrollBarPageUnit());
				}
				break;
			}
		  case kScrollBarPart_PageDownButton:
			{
				if( mViewImp.GetVerticalMode() == false )
				{
					NVI_Scroll(0,NVI_GetVScrollBarPageUnit());
				}
				else
				{
					//�c�����̏ꍇ�͋t���� #558
					NVI_Scroll(0,-NVI_GetVScrollBarPageUnit());
				}
				break;
			}
		  case kScrollBarPart_ThumbTracking:
			{
				AImagePoint	pt;
				NVI_GetOriginOfLocalFrame(pt);
				if( mViewImp.GetVerticalMode() == false )
				{
					pt.y = NVM_GetWindow().NVI_GetControlNumber(mControlID_VScrollBar);
				}
				else
				{
					//�c�����̏ꍇ�͋t���� #558
					pt.y = mImageHeight-NVI_GetViewHeight()-NVM_GetWindow().NVI_GetControlNumber(mControlID_VScrollBar);
				}
				NVI_ScrollTo(pt,true,true,kScrollTrigger_ScrollBar);//#1031
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
	else
	{
		switch(inPart)
		{
		  case kScrollBarPart_UpButton:
			{
				NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
				break;
			}
		  case kScrollBarPart_DownButton:
			{
				NVI_Scroll(NVI_GetHScrollBarUnit(),0);
				break;
			}
		  case kScrollBarPart_PageUpButton:
			{
				NVI_Scroll(-NVI_GetHScrollBarPageUnit(),0);
				break;
			}
		  case kScrollBarPart_PageDownButton:
			{
				NVI_Scroll(NVI_GetHScrollBarPageUnit(),0);
				break;
			}
		  case kScrollBarPart_ThumbTracking:
			{
				AImagePoint	pt;
				NVI_GetOriginOfLocalFrame(pt);
				pt.x = NVM_GetWindow().NVI_GetControlNumber(mControlID_HScrollBar);
				NVI_ScrollTo(pt,true,true,kScrollTrigger_ScrollBar);//#1031
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
}

//
void	AView::EVT_DoControlBoundsChanged()
{
	//
	mHScrollBarPageUnit = NVI_GetViewWidth()  - NVI_GetHScrollBarUnit();
	mVScrollBarPageUnit = NVI_GetViewHeight() - NVI_GetVScrollBarUnit();
	//
	EVTDO_DoControlBoundsChanged();
}

/**
�}�E�XWheel����OS�C�x���g�R�[���o�b�N
*/
void	AView::EVT_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
		const ALocalPoint inLocalPoint )
{
	//�C�x���gRedirect���w�肳��Ă���ꍇ�́ARedirect��View��EVT_���\�b�h���R�[������
	if( mMouseWheelEventRedirectViewID != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetViewByID(mMouseWheelEventRedirectViewID).
		EVT_DoMouseWheel(inDeltaX,inDeltaY,inModifierKeys,inLocalPoint);
		return;
	}
	//
	EVTDO_DoMouseWheel(inDeltaX,inDeltaY,inModifierKeys,inLocalPoint);
}

//#1380
/**
�E�N���b�N���j���[�N���b�N���̃f�t�H���g����
*/
ABool	AView::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( mContextMenuItemID > 0 )
	{
		AImagePoint	clickImagePoint = {0};
		NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
		if( NVM_IsImagePointInViewRect(clickImagePoint) == true )
		{
			AGlobalPoint	globalPoint = {0};
			NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
			NVMC_ShowContextMenu(mContextMenuItemID,globalPoint);
		}
		return true;
	}
	else
	{
		return false;
	}
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�\������
//#137

/**
Show/Hide
*/
void	AView::NVI_SetShow( const ABool inShow )
{
	NVMC_SetShowControl(inShow);
	NVIDO_SetShow(inShow);
	//FrameView��Show/Hide��ݒ� #135
	if( ExistFrameView() == true )
	{
		GetFrameView().NVI_SetShow(inShow);
	}
}

/**
Enable/Disable
*/
void	AView::NVI_SetEnabled( const ABool inEnable )
{
	//#530
	if( NVMC_IsControlEnabled() == inEnable )   return;
	//
	NVMC_SetEnable(inEnable);
	NVIDO_SetEnabled(inEnable);
	//FrameView��Enable/Disable��ݒ�
	if( ExistFrameView() == true )
	{
		GetFrameView().NVI_SetEnabled(inEnable);
	}
	NVMC_RefreshControl();//win
}

#pragma mark ===========================

#pragma mark ---�X�N���[��

//�X�N���[��
void	AView::NVI_Scroll( const ANumber inDeltaX, const ANumber inDeltaY, const ABool inRedraw, const ABool inConstrainToImageSize,
						   const AScrollTrigger inScrollTrigger )//#1031
{
	if( inDeltaX == 0 && inDeltaY == 0 )   return;
	ScrollCore(inDeltaX,inDeltaY,inRedraw,inConstrainToImageSize,inScrollTrigger);//#1031
}

void	AView::ScrollCore( const ANumber inDeltaX, const ANumber inDeltaY, const ABool inRedraw, const ABool inConstrainToImageSize,
						   const AScrollTrigger inScrollTrigger )//#1031
{
	//fprintf(stderr,"%16X ScrollCore() - Start\n",(int)(GetObjectID().val));
	
	if( inRedraw == true )
	{
		//#688
		//�X�N���[���O�ɁA���܂ł̕`�悷�ׂ��̈���A�������`��(EVT_DoDraw())����
		NVMC_ExecuteDoDrawImmediately();
	}
	
	//�X�N���[���O����
	NVIDO_ScrollPreProcess(inDeltaX,inDeltaY,inRedraw,inConstrainToImageSize,inScrollTrigger);//#138 #1031
	
	AImagePoint	newOrigin = mOriginOfLocalFrame;
	newOrigin.x += inDeltaX;
	if( inConstrainToImageSize == true )
	{
		//�C���[�W�T�C�Y�O�ɃX�N���[�����Ȃ��悤�ɕ␳
		if( newOrigin.x > mImageWidth - NVI_GetViewWidth() )//#558 NVM_GetWindowConst().NVI_GetControlWidth(mControlID)��NVI_GetViewWidth()
		{
			newOrigin.x = mImageWidth - NVI_GetViewWidth();//#558 NVM_GetWindowConst().NVI_GetControlWidth(mControlID)��NVI_GetViewWidth()
		}
		if( newOrigin.x < 0 )
		{
			newOrigin.x = 0;
		}
	}
	newOrigin.y += inDeltaY;
	if( inConstrainToImageSize == true )
	{
		//�C���[�W�T�C�Y�O�ɃX�N���[�����Ȃ��悤�ɕ␳
		if( newOrigin.y > mImageHeight + mImageYMaxMargin - NVI_GetViewHeight() )//#621 #558 NVM_GetWindowConst().NVI_GetControlHeight(mControlID)��NVI_GetViewHeight()
		{
			newOrigin.y = mImageHeight + mImageYMaxMargin - NVI_GetViewHeight();//#621 #558 NVM_GetWindowConst().NVI_GetControlHeight(mControlID)��NVI_GetViewHeight()
		}
		if( newOrigin.y < -mImageYMinMargin )//#621
		{
			newOrigin.y = -mImageYMinMargin;//#621
		}
	}
	ANumber	deltaX = newOrigin.x - mOriginOfLocalFrame.x;
	ANumber	deltaY = newOrigin.y - mOriginOfLocalFrame.y;
	mOriginOfLocalFrame = newOrigin;
	if( inRedraw == true )
	{
		NVMC_Scroll(-deltaX,-deltaY);
	}
	
	//�X�N���[���o�[�̌��ݒl�ݒ�
	if( mControlID_HScrollBar != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_SetControlNumber(mControlID_HScrollBar,mOriginOfLocalFrame.x);
	}
	if( mControlID_VScrollBar != kControlID_Invalid )
	{
		if( mViewImp.GetVerticalMode() == false )
		{
			NVM_GetWindow().NVI_SetControlNumber(mControlID_VScrollBar,mOriginOfLocalFrame.y);
		}
		else
		{
			//�c�������[�h���͉E����0�ɂ��� #558
			NVM_GetWindow().NVI_SetControlNumber(mControlID_VScrollBar,mImageHeight-NVI_GetViewHeight()-mOriginOfLocalFrame.y);
		}
	}
	
	//AWindow�I�u�W�F�N�g��View���X�N���[�����ꂽ���Ƃ�`����
	//#1031 NVIDO_ScrollPostProcess()�ֈړ� NVM_GetWindow().OWICB_ViewScrolled(mControlID,inDeltaX,inDeltaY,inRedraw,inConstrainToImageSize);
	
	//�X�N���[���㏈��
	NVIDO_ScrollPostProcess(inDeltaX,inDeltaY,inRedraw,inConstrainToImageSize,inScrollTrigger);//#138 #1031
	
	//fprintf(stderr,"%16X ScrollCore() - End\n",(int)(GetObjectID().val));
}

//�X�N���[��
void	AView::NVI_ScrollTo( const AImagePoint& inNewOrigin, const ABool inRedraw, const ABool inConstrainToImageSize,
							 const AScrollTrigger inScrollTrigger )//#1031
{
	NVI_Scroll(inNewOrigin.x-mOriginOfLocalFrame.x,inNewOrigin.y-mOriginOfLocalFrame.y,inRedraw,inConstrainToImageSize,inScrollTrigger);//#1031
}

/**
�X�N���[���ʒu��Image size�͈̔͊O�ɂȂ��Ă�����A����
*/
void	AView::NVI_ConstrainScrollToImageSize( const ABool inRedraw )
{
	ScrollCore(0,0,inRedraw,true);
}

void	AView::NVI_SetScrollBarControlID( const AControlID inHScrollBar, const AControlID inVScrollBar )
{
	if( mViewImp.GetVerticalMode() == false )
	{
		mControlID_HScrollBar = inHScrollBar;
		mControlID_VScrollBar = inVScrollBar;
	}
	else
	{
		//�c�������[�h����H,V����ւ� #558
		mControlID_HScrollBar = inVScrollBar;
		mControlID_VScrollBar = inHScrollBar;
	}
	//Scrollbar�l�\���X�V #558
	NVI_UpdateScrollBar();
}

/**
�X�N���[���o�[unit�ݒ�
*/
void	AView::NVI_SetScrollBarUnit( const ANumber inHScrollUnit, const ANumber inVScrollUnit, 
		const ANumber inHScrollPageUnit, const ANumber inVScrollPageUnit )
{
	mHScrollBarUnit = inHScrollUnit;
	mVScrollBarUnit = inVScrollUnit;
	mHScrollBarPageUnit = inHScrollPageUnit;
	mVScrollBarPageUnit = inVScrollPageUnit;
	//#725
	//page unit�ݒ�
	if( mControlID_VScrollBar != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_SetScrollBarPageUnit(mControlID_VScrollBar,mVScrollBarPageUnit);
	}
	if( mControlID_HScrollBar != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_SetScrollBarPageUnit(mControlID_HScrollBar,mHScrollBarPageUnit);
	}
}


#pragma mark ===========================

#pragma mark ---�t�H�[�J�X����

/**
Focus��Activate���ꂽ�Ƃ��ɃR�[�������
*/
void	AView::EVT_DoViewFocusActivated()
{
	//�t�H�[�J�X�ݒ�
	//#688 mFocusActive = true;
	EVTDO_DoViewFocusActivated();
	//FrameView��Focus�`��p��Ԃ�ݒ� #135
	if( ExistFrameView() == true )
	{
		GetFrameView().SPI_SetFocused(true);
	}
	//#688
	//mAutomaticSelectBySwitchFocus��true�Ȃ�A�t�H�[�J�X��ԂɂȂ����Ƃ��ɑS�I������
	//�iCocoa�Ή���tab�L�[�ɂ��focus�����OS�C���ɂ����̂ŁA�����ŕ߂܂���K�v������B�j
	//#688 CWindowImp::APICB_CocoaViewBecomeFirstResponder()�ɂāA�^�u�L�[�ɂ��ړ��̏ꍇ�̂ݎ��s����悤�ɂ����B NVI_AutomaticSelectBySwitchFocus();
}

/**
Focus��Deactivate���ꂽ�Ƃ��ɃR�[�������
*/
void	AView::EVT_DoViewFocusDeactivated()
{
	//#688 mFocusActive = false;
	EVTDO_DoViewFocusDeactivated();
	//FrameView��Focus�`��p��Ԃ�ݒ� #135
	if( ExistFrameView() == true )
	{
		GetFrameView().SPI_SetFocused(false);
	}
}

//#312
/**
�E�C���h�E��latent��focus���ǂ�����Ԃ�
*/
ABool	AView::NVI_IsLatentFocus() const
{
	return (NVI_GetControlID() == NVM_GetWindowConst().NVI_GetLatentFocus());
}

/**
�r���[��Focus����Ă��邩�ǂ�����Ԃ�
*/
ABool	AView::NVI_IsFocusActive() const
{
	return (/*#688 mFocusActive*/NVI_GetControlID() == NVM_GetWindowConst().NVI_GetCurrentFocus() &&
		NVM_GetWindowConst().NVI_IsWindowFocusActive());
}

//#312
/**
�^�u����latent��focus���ǂ�����Ԃ�
*/
ABool	AView::NVI_IsFocusInTab() const
{
	return NVM_GetWindowConst().NVI_IsFocusInTab(NVI_GetControlID());
}

#pragma mark ===========================

#pragma mark ---�ʒu�E�T�C�Y

/**
�ʒu�ݒ�i�t���������s���B�ʏ��NVMC_SetPosition()�ł͂Ȃ���������Ăяo���B�j
*/
void	AView::NVI_SetPosition( const AWindowPoint& inWindowPoint )
{
	if( ExistFrameView() == false )
	{
		//FrameView�����݂��Ȃ��ꍇ
		//�ʒu�ݒ�
		NVMC_SetControlPosition(inWindowPoint);//#182
	}
	else
	{
		//FrameView�����݂���ꍇ
		/*
		//Frame�������ʒu�����炷
		AWindowPoint	insetPoint;
		insetPoint.x = inWindowPoint.x + mFrameLeftMargin;
		insetPoint.y = inWindowPoint.y + mFrameTopMargin;
		NVMC_SetControlPosition(insetPoint);
		//Frame�T�C�Y�ύX
		UpdateFrameViewBounds();
		*/
		//����view���̂�frame view�̗��[��bind����悤�ɂ���
		NVMC_SetControlBindings(true,true,true,true,false,false);
		//frame view�̃T�C�Y�擾
		ANumber	frameWidth = NVI_GetWidth() + mFrameLeftMargin + mFrameRightMargin;
		ANumber	frameHeight = NVI_GetHeight() + mFrameTopMargin + mFrameBottomMargin;
		//Frame�̈ʒu�E�T�C�Y�ύX
		GetFrameView().SPI_SetRealFramePositionAndSize(inWindowPoint,frameWidth,frameHeight);
	}
	//�h���N���X����
	NVIDO_SetPosition(inWindowPoint);
}

/**
�T�C�Y�ݒ�i�t���������s���B�ʏ��NVI_SetSize()�ł͂Ȃ���������Ăяo���B�j
*/
void	AView::NVI_SetSize( const ANumber inWidth, const ANumber inHeight )
{
	if( ExistFrameView() == false )
	{
		//FrameView�����݂��Ȃ��ꍇ
		//�T�C�Y�ݒ�
		NVMC_SetControlSize(inWidth,inHeight);//#182
	}
	else
	{
		//FrameView�����݂���ꍇ
		/*
		//Frame�������T�C�Y������������
		ANumber	insetWidth = inWidth - mFrameLeftMargin - mFrameRightMargin;
		ANumber	insetHeight = inHeight - mFrameTopMargin - mFrameBottomMargin;
		NVMC_SetControlSize(insetWidth,insetHeight);
		//Frame�T�C�Y�ύX
		UpdateFrameViewBounds();
		*/
		//����view���̂�frame view�̗��[��bind����悤�ɂ���
		NVMC_SetControlBindings(true,true,true,true,false,false);
		//frame view�̈ʒu�擾
		AWindowPoint	editBoxPoint = NVI_GetPosition();
		AWindowPoint	framePoint = {0};
		framePoint.x = editBoxPoint.x - mFrameLeftMargin;
		framePoint.y = editBoxPoint.y - mFrameTopMargin;
		//Frame�̈ʒu�E�T�C�Y�ύX
		GetFrameView().SPI_SetRealFramePositionAndSize(framePoint,inWidth,inHeight);
		//����view���̂̈ʒu�ݒ�
		NVMC_SetControlSize(inWidth - mFrameLeftMargin - mFrameRightMargin,inHeight - mFrameTopMargin - mFrameBottomMargin);
	}
	//�h���N���X����
	NVIDO_SetSize(inWidth,inHeight);
	//�X�N���[���o�[�ő�ŏ����ݒl�X�V#688
	NVI_UpdateScrollBar();
}

//�ʒu�擾
AWindowPoint	AView::NVI_GetPosition() const
{
	AWindowPoint	wpt;
	NVM_GetWindowConst().NVI_GetControlPosition(NVI_GetControlID(),wpt);
	return wpt;
}

//���擾
ANumber	AView::NVI_GetWidth() const
{
	return NVM_GetWindowConst().NVI_GetControlWidth(NVI_GetControlID());
}

//�����擾
ANumber	AView::NVI_GetHeight() const
{
	return NVM_GetWindowConst().NVI_GetControlHeight(NVI_GetControlID());
}

//#455
/**
*/
void	AView::NVI_SetControlBindings( const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
		const ABool inWidthFixed, const ABool inHeightFixed )
{
	if( ExistFrameView() == true )
	{
		//Frame view�̕���Bind�ݒ肷��
		GetFrameView().NVMC_SetControlBindings(inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
		//���g�̓t���[���r���[�̍��E�㉺��binding����B�i�T�C�Y�̓t���L�V�u���j
		NVMC_SetControlBindings(true,true,true,true,false,false);
	}
	else
	{
		NVMC_SetControlBindings(inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
	}
}

#pragma mark ===========================

#pragma mark ---

void	AView::NVI_Refresh()
{
	if( NVMC_IsControlVisible() == true )
	{
		NVMC_RefreshControl();
	}
}

ABool	AView::NVI_IsVisible() const
{
	return NVMC_IsControlVisible();
}

//#92
/**
View���őO�ʂɎ����Ă���
*/
void	AView::NVI_RevealView( const ABool inSelectWindow, const ABool inSwitchFocus )//#291 #492
{
	//�E�C���h�E��I��
	if( inSelectWindow == true )//#291
	{
		NVM_GetWindow().NVI_SelectWindow();
	}
	//Tab��I��
	if( NVI_IsVisible() == false )//#212 ����View���\�����Ȃ�Tab�I��
	{
		NVM_GetWindow().NVI_SelectTabByIncludingControlID(NVI_GetControlID());
	}
	//�t�H�[�J�X�ݒ� #313
	if( inSwitchFocus == true )//#492
	{
		NVM_GetWindow().NVI_SwitchFocusTo(NVI_GetControlID());
	}
}

/**
����View���܂���Window��TryClose����iBorrowerView�łȂ��ꍇ�̂݁j
*/
void	AView::NVI_TryCloseWindow()
{
	if( mBorrowerView == false )
	{
		NVM_GetWindow().NVI_TryClose();
	}
}

/**
����View���܂���Window��ExecuteClose����iBorrowerView�łȂ��ꍇ�̂݁j
*/
void	AView::NVI_ExecuteCloseWindow()
{
	if( mBorrowerView == false )
	{
		NVM_GetWindow().NVI_ExecuteClose();
	}
}

/**
����View���܂���Window�����̂��֎~����iBorrowerView�łȂ��ꍇ�̂݁j
*/
void	AView::NVI_SetInhibitCloseWindow( const ABool inInhibit )
{
	if( mBorrowerView == false )
	{
		NVM_GetWindow().NVI_SetInhibitClose(inInhibit);
	}
}

/**
����View���܂���Window�̃^�C�g���o�[���X�V����iBorrowerView�łȂ��ꍇ�̂݁j
*/
void	AView::NVI_UpdateWindowTitleBar()
{
	NVM_GetWindow().NVI_UpdateTitleBar();
}

//#717
/**
Frame Rect��global���W�Ŏ擾
*/
void	AView::NVI_GetGlobalViewRect( AGlobalRect& outGlobalRect ) const
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	NVM_GetWindowConst().NVI_GetGlobalRectFromControlLocalRect(NVI_GetControlID(),frameRect,outGlobalRect);
}

#pragma mark ===========================

#pragma mark <�ĉ����\�b�h>

#pragma mark ===========================

#pragma mark ---Image�T�C�Y

//Image�T�C�Y�ݒ�
void	AView::NVM_SetImageSize( const ANumber inWidth, const ANumber inHeight )
{
	//�ω������Ȃ牽�������I��
	if( mImageWidth == inWidth && mImageHeight == inHeight )
	{
		return;
	}
	//
	mImageWidth = inWidth;
	mImageHeight = inHeight;
	
	//#519 ���\�b�h��
	NVI_UpdateScrollBar();
}

//#519
/**
�X�N���[���o�[�̍ő�ŏ��E���ݒl�ݒ�
*/
void	AView::NVI_UpdateScrollBar()
{
	//�X�N���[���o�[�̍ő�ŏ��l�ݒ�
	if( mControlID_VScrollBar != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_SetMinMax(mControlID_VScrollBar,0,mImageHeight-NVI_GetViewHeight());//#558 NVM_GetWindowConst().NVI_GetControlHeight(mControlID)��NVI_GetViewHeight()
	}
	if( mControlID_HScrollBar != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_SetMinMax(mControlID_HScrollBar,0,mImageWidth-NVI_GetViewWidth());//#558 NVM_GetWindowConst().NVI_GetControlWidth(mControlID)��NVI_GetViewWidth()
	}
	//�X�N���[���o�[�̌��ݒl�ݒ�
	if( mControlID_HScrollBar != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_SetControlNumber(mControlID_HScrollBar,mOriginOfLocalFrame.x);
	}
	if( mControlID_VScrollBar != kControlID_Invalid )
	{
		if( mViewImp.GetVerticalMode() == false )
		{
			NVM_GetWindow().NVI_SetControlNumber(mControlID_VScrollBar,mOriginOfLocalFrame.y);
		}
		else
		{
			//�c�������[�h���͉E����0�ɂ��� #558
			NVM_GetWindow().NVI_SetControlNumber(mControlID_VScrollBar,mImageHeight-NVI_GetViewHeight()-mOriginOfLocalFrame.y);
		}
	}
}

//#688
/**
���T�C�Y�����ǂ����𔻒�
*/
ABool	AView::NVI_IsInLiveResize() const
{
	return ((mViewImp.IsInLiveResize()==true)||(NVM_GetWindowConst().NVI_GetVirtualLiveResizing()==true));
}

#pragma mark ===========================

#pragma mark ---�C�x���gRedirect

//#135
/**
MouseWheel�C�x���g��Redirect���ݒ肷��

MouseWheel�C�x���g�́A�����Őݒ肵��ViewID�֓]�������
*/
void	AView::NVI_SetMouseWheelEventRedirectViewID( const AViewID inViewID )
{
	mMouseWheelEventRedirectViewID = inViewID;
	NVIDO_SetMouseWheelEventRedirectViewID(inViewID);
	//FrameView�����l�ɐݒ�
	if( ExistFrameView() == true )
	{
		GetFrameView().NVI_SetMouseWheelEventRedirectViewID(inViewID);
	}
}

#pragma mark ===========================

#pragma mark ---Frame View
//#135

/**
FrameView�𐶐�
*/
void	AView::NVI_CreateFrameView( const ANumber inLeftMargin, const ANumber inRightMargin, const ANumber inTopMargin, const ANumber inBottomMargin )
{
	//Frame�Ƃ̂������̗]���T�C�Y��ݒ�
	mFrameLeftMargin 	= inLeftMargin;
	mFrameRightMargin 	= inRightMargin;
	mFrameTopMargin 	= inTopMargin;
	mFrameBottomMargin 	= inBottomMargin;
	//����View�̌��̈ʒu�E�T�C�Y���擾���A����View�̃T�C�Y�͏k������B
	AWindowPoint	origPoint = NVI_GetPosition();
	ANumber	origWidth = NVI_GetWidth();
	ANumber	origHeight = NVI_GetHeight();
	AWindowPoint	insetPoint;
	insetPoint.x = origPoint.x + mFrameLeftMargin;
	insetPoint.y = origPoint.y + mFrameTopMargin;
	ANumber	insetWidth = origWidth - mFrameLeftMargin - mFrameRightMargin;
	ANumber	insetHeight = origHeight - mFrameTopMargin - mFrameBottomMargin;
	NVMC_SetControlPosition(insetPoint);
	NVMC_SetControlSize(insetWidth,insetHeight);
	//EditBox�����݂���TabControl��Index���擾
	AIndex	tabControlIndex = NVM_GetWindow().NVI_GetControlEmbeddedTabControlIndex(NVI_GetControlID());
	//Frame����
	mFrameViewControlID = NVM_GetWindow().NVM_AssignDynamicControlID();//#271
	AFrameViewFactory	frameViewFactory(NVM_GetWindowID(),mFrameViewControlID,NVI_GetControlID());//#92
	NVM_GetWindow().NVI_CreateView(mFrameViewControlID,origPoint,origWidth,origHeight,kControlID_Invalid,NVI_GetControlID(),false,frameViewFactory,tabControlIndex,true);//#92 #137 �Ō��false->true
	GetFrameView().SPI_SetRealFramePositionAndSize(origPoint,origWidth,origHeight);//#135
	NVM_GetWindow().NVI_EmbedControl(mFrameViewControlID,NVI_GetControlID());//#182
}

/*#688
//FrameView�̈ʒu�E�T�C�Y���X�V
void	AView::UpdateFrameViewBounds()
{
	//EditBox�̈ʒu�E�T�C�Y���擾���AFrame�̈ʒu�E�T�C�Y���v�Z����
	AWindowPoint	editBoxPoint = NVI_GetPosition();
	ANumber	editBoxWidth = NVI_GetWidth();
	ANumber	editBoxHeight = NVI_GetHeight();
	AWindowPoint	framePoint;
	framePoint.x = editBoxPoint.x - mFrameLeftMargin;//#135
	framePoint.y = editBoxPoint.y - mFrameTopMargin;//#135
	ANumber	frameWidth = editBoxWidth + mFrameLeftMargin + mFrameRightMargin;//#135
	ANumber	frameHeight = editBoxHeight + mFrameTopMargin + mFrameBottomMargin;//#135
	//Frame�̈ʒu�E�T�C�Y�ύX
	GetFrameView().SPI_SetRealFramePositionAndSize(framePoint,frameWidth,frameHeight);//#135
	//EditBox�̈ʒu���ύX�����̂ŁA�����Ō��̈ʒu�iWindowPoint�j���Đݒ肷�� #390
	NVMC_SetControlPosition(editBoxPoint);
}
*/

//FrameView���폜
void	AView::DeleteFrameView()
{
	if( NVM_GetWindow().NVI_ExistView(mFrameViewControlID) == true )//#137 NVI_DeleteTabAndView()�ɂ���ɍ폜����Ă���ꍇ������̂Ń`�F�b�N�ǉ�
	{
		NVM_GetWindow().NVI_DeleteControl(mFrameViewControlID);//win NVI_DeleteView()��mTabDataArray_ControlIDs����̍폜�����s��Ȃ��̂�NG
	}
	mFrameViewControlID = kControlID_Invalid;
}

/**
FrameView�I�u�W�F�N�g�擾
*/
AView_Frame&	AView::GetFrameView()
{
	return dynamic_cast<AView_Frame&>(NVM_GetWindow().NVI_GetViewByControlID(mFrameViewControlID));
}

#pragma mark ===========================

#pragma mark ---ScrollBar
//#135

/**
VScrollBar����
*/
void	AView::NVI_CreateVScrollBar( const ANumber inWidth, const ABool inHasHScrollbar )
{
	//
	mVScrollBarWidth = inWidth;
	//����View�̌��̈ʒu�E�T�C�Y���擾���A����View�̃T�C�Y�͏k������B
	AWindowPoint	origPoint = NVI_GetPosition();
	ANumber	origWidth = NVI_GetWidth();
	ANumber	origHeight = NVI_GetHeight();
	ANumber	insetWidth = origWidth - mVScrollBarWidth;
	ANumber	insetHeight = origHeight;
	NVMC_SetControlSize(insetWidth,insetHeight);
	//EditBox�����݂���TabControl��Index���擾
	AIndex	tabControlIndex = NVM_GetWindow().NVI_GetControlEmbeddedTabControlIndex(NVI_GetControlID());
	//ScrollBar����
	mVScrollBarControlID = NVM_GetWindow().NVM_AssignDynamicControlID();
	if( ExistFrameView() == true )
	{
		//
		AWindowRect	frameRealBounds;
		GetFrameView().SPI_GetRealFrameBounds(frameRealBounds);
		//
		AWindowPoint	pt;
		pt.x = frameRealBounds.right - mVScrollBarWidth -1;
		pt.y = frameRealBounds.top +1;
		ANumber	h = frameRealBounds.bottom-frameRealBounds.top-2;
		if( inHasHScrollbar == true )
		{
			h -= 15;
		}
		NVM_GetWindow().NVI_CreateScrollBar(mVScrollBarControlID,pt,mVScrollBarWidth,h,tabControlIndex);
		NVM_GetWindow().NVI_EmbedControl(mFrameViewControlID,mVScrollBarControlID);
	}
	else
	{
		AWindowPoint	pt;
		pt.x = origPoint.x + insetWidth;
		pt.y = origPoint.y;
		NVM_GetWindow().NVI_CreateScrollBar(mVScrollBarControlID,pt,mVScrollBarWidth,insetHeight,tabControlIndex);
	}
	//
	NVM_GetWindow().NVI_SetControlBindings(mVScrollBarControlID,false,true,true,true,true,false);//#455 #688
	//
	mControlID_VScrollBar = mVScrollBarControlID;
}

/**
HScrollBar����
*/
void	AView::NVI_CreateHScrollBar( const ANumber inHeight, const ABool inHasVScrollbar )
{
	//
	mHScrollBarHeight = inHeight;
	//����View�̌��̈ʒu�E�T�C�Y���擾���A����View�̃T�C�Y�͏k������B
	AWindowPoint	origPoint = NVI_GetPosition();
	ANumber	origWidth = NVI_GetWidth();
	ANumber	origHeight = NVI_GetHeight();
	ANumber	insetWidth = origWidth;
	ANumber	insetHeight = origHeight - mHScrollBarHeight;
	NVMC_SetControlSize(insetWidth,insetHeight);
	//EditBox�����݂���TabControl��Index���擾
	AIndex	tabControlIndex = NVM_GetWindow().NVI_GetControlEmbeddedTabControlIndex(NVI_GetControlID());
	//ScrollBar����
	mHScrollBarControlID = NVM_GetWindow().NVM_AssignDynamicControlID();
	if( ExistFrameView() == true )
	{
		//
		AWindowRect	frameRealBounds;
		GetFrameView().SPI_GetRealFrameBounds(frameRealBounds);
		//
		AWindowPoint	pt;
		pt.x = frameRealBounds.left +1;
		pt.y = frameRealBounds.bottom - mHScrollBarHeight -1;
		ANumber	w = frameRealBounds.right-frameRealBounds.left-2;
		if( inHasVScrollbar == true )
		{
			w -= 15;
		}
		NVM_GetWindow().NVI_CreateScrollBar(mHScrollBarControlID,pt,w,mHScrollBarHeight,tabControlIndex);
		NVM_GetWindow().NVI_EmbedControl(mFrameViewControlID,mHScrollBarControlID);
	}
	else
	{
		AWindowPoint	pt;
		pt.x = origPoint.x;
		pt.y = origPoint.y + insetHeight;
		NVM_GetWindow().NVI_CreateScrollBar(mHScrollBarControlID,pt,insetWidth,mHScrollBarHeight,tabControlIndex);
	}
	//
	NVM_GetWindow().NVI_SetControlBindings(mHScrollBarControlID,true,false,true,true,false,true);//#455 #688
	//
	mControlID_HScrollBar = mHScrollBarControlID;
}


#pragma mark ===========================

#pragma mark ---�}�E�X�N���b�NTracking���[�h�ݒ�
//#688

/**
�}�E�X�N���b�NTracking���[�h��ݒ肷��
*/
void	AView::NVM_SetMouseTrackingMode( const ABool inSet )
{
	//�����o�ϐ��ݒ�itrue�̂Ƃ��̂�EVTDO_DoMouseTracking()/EVT_DoMouseTrackEnd()�����s����j
	mMouseTrackingMode = inSet;
	//ViewImp�Ƀ��[�h�ݒ�
	mViewImp.SetMouseTrackingMode(inSet);
}

#pragma mark ===========================

#pragma mark ---�e�L�X�g�`��
//#725

/**
�Ō�̕���...�ŏȗ������e�L�X�g���擾
*/
void	AView::NVI_GetEllipsisTextWithFixedLastCharacters( const AText& inText, CTextDrawData& inTextDrawData, const ANumber inWidth,
		const AItemCount inLastCharCount, AText& outText, const ABool inEllipsisRepeatChar ) 
{
	//�ȗ��O�̃e�L�X�g�\�������擾
	ANumber	totalWidth = NVMC_GetDrawTextWidth(inTextDrawData);
	//���XinWidth�Ɏ��܂��Ă���΁A���̂܂܂̃e�L�X�g���i�[���ďI���B
	if( totalWidth < inWidth )
	{
		outText.SetText(inText);
		return;
	}
	//...�̕\�������擾
	AText	ellipsisText("...");
	ANumber	ellipsisTextWidth = NVMC_GetDrawTextWidth(ellipsisText);
	//�Ōォ��inLastCharCount�����̈ʒu���擾
	AIndex	lastCharsStartIndex = inText.GetItemCount() - inLastCharCount;
	if( lastCharsStartIndex < 0 )   lastCharsStartIndex = 0;
	lastCharsStartIndex = inText.GetCurrentCharPos(lastCharsStartIndex);
	//�Ōォ��inLastCharCount�������̕\�������擾
	ANumber	lastCharsWidth = totalWidth - NVMC_GetImageXByTextPosition(inTextDrawData,lastCharsStartIndex);
	//inWidth����...�ȍ~�̕������������\�������v�Z
	ANumber	remainTextWidth = inWidth - 3 - ellipsisTextWidth - lastCharsWidth;
	if( inEllipsisRepeatChar == true )
	{
		//------------------�J��Ԃ��������ȗ�����ꍇ------------------
		//�J��Ԃ����������邩�ǂ����𔻒肵�A����΂��̕������ȗ�����B
		//�i�J��Ԃ��������Ȃ���΁A�ʏ폈�����s���j
		
		//inLastCharCount�����̈ʒu�܂ł̃e�L�X�g���擾
		AText	text;
		inText.GetText(0,lastCharsStartIndex,text);
		//�J��Ԃ������̈ʒu���擾
		AUChar	prevCh = 0;
		AItemCount	repeatCount = 0;
		AIndex	repeatSpos = kIndex_Invalid;
		AIndex	repeatEpos = kIndex_Invalid;
		for( AIndex p = 0; p < text.GetItemCount(); p++ )
		{
			AUChar	ch = text.Get(p);
			if( prevCh == ch )
			{
				//�O�̕����Ɠ����ꍇ
				//�J��Ԃ��J�n�ʒu���L��
				if( repeatSpos == kIndex_Invalid )
				{
					repeatSpos = p-1;
				}
				//�J��Ԃ����C���N�������g
				repeatCount++;
			}
			else
			{
				//�O�̕����ƈႤ�ꍇ
				if( repeatCount >= 6 )
				{
					//�J��Ԃ��I������6�����ȏ�̏ꍇ�́A�I���ʒu��ݒ肵�āA���[�v�I��
					repeatEpos = p;
					break;
				}
				//�J��Ԃ����Z�b�g
				repeatSpos = kIndex_Invalid;
				repeatCount = 0;
			}
			//�O��̕������L��
			prevCh = ch;
		}
		if( repeatEpos != kIndex_Invalid )
		{
			//------------------�J��Ԃ����������ꍇ------------------
			
			//�J��Ԃ������폜
			text.Delete(repeatSpos+1,repeatEpos-repeatSpos-2);
			//...�}��
			text.InsertCstring(repeatSpos+1,"...");
			//�uinWidth����...�ȍ~�̕������������\�����v���ɕ\���ł���e�L�X�g���擾
			NVI_GetTextInWidth(text,remainTextWidth,outText);
			//��ɂȂ��āu�Ō�̕����̃e�L�X�g�v������outText�Ɋi�[
			outText.AddCstring("...");
			AText	lastText;
			inText.GetText(lastCharsStartIndex,inText.GetItemCount()-lastCharsStartIndex,lastText);
			outText.AddText(lastText);
			return;
		}
	}
	//inWidth����...�ȍ~�̕������������\�����ɕ\���ł��镶�������擾
	AIndex	remainTextEndIndex = NVMC_GetTextPositionByImageX(inTextDrawData,remainTextWidth);
	//�uinWidth����...�ȍ~�̕������������\�����ɕ\���ł��镶�������̃e�L�X�g�v�u...�v�u�Ō�̕����̃e�L�X�g�v������outText�Ɋi�[
	inText.GetText(0,remainTextEndIndex,outText);
	outText.AddCstring("...");
	AText	lastText;
	inText.GetText(lastCharsStartIndex,inText.GetItemCount()-lastCharsStartIndex,lastText);
	outText.AddText(lastText);
}

/**
�Ō�̕���...�ŏȗ������e�L�X�g���擾
*/
void	AView::NVI_GetEllipsisTextWithFixedLastCharacters( const AText& inText, const ANumber inWidth,
														  const AItemCount inLastCharCount, AText& outText, 
														  const ABool inEllipsisRepeatChar ) 
{
	CTextDrawData	textDrawData(false);
	textDrawData.MakeTextDrawDataWithoutStyle(inText);
	NVI_GetEllipsisTextWithFixedLastCharacters(inText,textDrawData,inWidth,inLastCharCount,outText,inEllipsisRepeatChar);
}

/**
�ŏ��̕���...�ŏȗ������e�L�X�g���擾
*/
void	AView::NVI_GetEllipsisTextWithFixedFirstCharacters( const AText& inText, CTextDrawData& inTextDrawData, const ANumber inWidth,
		const AItemCount inFirstCharCount, AText& outText ) 
{
	//�ȗ��O�̃e�L�X�g�\�������擾
	ANumber	totalWidth = NVMC_GetDrawTextWidth(inTextDrawData);
	//���XinWidth�Ɏ��܂��Ă���΁A���̂܂܂̃e�L�X�g���i�[���ďI���B
	if( totalWidth < inWidth )
	{
		outText.SetText(inText);
		return;
	}
	//...�̕\�������擾
	AText	ellipsisText("...");
	ANumber	ellipsisTextWidth = NVMC_GetDrawTextWidth(ellipsisText);
	//�ŏ�����inFirstCharCount�����̈ʒu���擾
	AIndex	firstCharsEndIndex = inFirstCharCount;
	if( firstCharsEndIndex >= inText.GetItemCount() )   firstCharsEndIndex = inText.GetItemCount();
	firstCharsEndIndex = inText.GetCurrentCharPos(firstCharsEndIndex);
	//�ŏ�����inFirstCharCount�������̕\�������擾
	ANumber	firstCharsWidth = NVMC_GetImageXByTextPosition(inTextDrawData,firstCharsEndIndex);
	//inWidth����...�ȑO�̕������������\�������v�Z
	ANumber	remainTextWidth = inWidth -3 - ellipsisTextWidth - firstCharsWidth;
	//inWidth����...�ȑO�̕������������\�����ɕ\���ł��镶�������擾
	AIndex	remainTextStartIndex = NVMC_GetTextPositionByImageX(inTextDrawData,totalWidth-remainTextWidth);
	//inWidth����...�ȑO�̕������������\�����ɕ\���ł��镶�������̃e�L�X�g�A...�A�Ō�̕����̃e�L�X�g������outText�Ɋi�[
	inText.GetText(0,firstCharsEndIndex,outText);
	outText.AddCstring("...");
	AText	remainText;
	inText.GetText(remainTextStartIndex,inText.GetItemCount()-remainTextStartIndex,remainText);
	outText.AddText(remainText);
}

/**
�ŏ��̕���...�ŏȗ������e�L�X�g���擾
*/
void	AView::NVI_GetEllipsisTextWithFixedFirstCharacters( const AText& inText, const ANumber inWidth,
		const AItemCount inLastCharCount, AText& outText ) 
{
	CTextDrawData	textDrawData(false);
	textDrawData.MakeTextDrawDataWithoutStyle(inText);
	NVI_GetEllipsisTextWithFixedFirstCharacters(inText,textDrawData,inWidth,inLastCharCount,outText);
}

/**
�w�蕝�܂ł̃e�L�X�g���擾
*/
void	AView::NVI_GetTextInWidth( const AText& inText, const ANumber inWidth, AText& outText ) 
{
	CTextDrawData	textDrawData(false);
	textDrawData.MakeTextDrawDataWithoutStyle(inText);
	AIndex	index = NVMC_GetTextPositionByImageX(textDrawData,inWidth);
	inText.GetText(0,index,outText);
}

/**
�w��rect���ɕ����s�i���[�h���b�v�j�Ƀe�L�X�g�\������
*/
void	AView::NVI_DrawTextMultiLine( const ALocalRect& inLocalRect, const AText& inText,
		const AText& inFontName, const AFloatNumber inFontSize, 
		const AColor inColor, const ATextStyle inStyle, const ABool inAntiAlias )
{
	NVMC_SetDefaultTextProperty(inFontName,inFontSize,inColor,inStyle,true);
	ANumber	fontheight = 9 ,fontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(fontheight,fontascent);
	//���[�h���b�v�v�Z
	AArray<AIndex>	lineStartArray;
	AArray<AItemCount>	lengthArray;
	//inText.CalcParagraphBreaks(lineStartArray,lengthArray);
	CWindowImp::CalcLineBreaks(inText,inFontName,inFontSize,inAntiAlias,inLocalRect.right-inLocalRect.left,false,
							   lineStartArray,lengthArray);
	//�e�s���Ƃ̏���
	for( AIndex lineIndex = 0; lineIndex < lineStartArray.GetItemCount(); lineIndex++ )
	{
		//�s�f�[�^�擾
		AIndex	start 	= lineStartArray.Get(lineIndex);
		AItemCount	len = lengthArray.Get(lineIndex);
		AText	text;
		inText.GetText(start,len,text);
		//�srect�擾
		ALocalRect	rect = inLocalRect;
		rect.top		+= lineIndex*fontheight;
		rect.bottom		+= rect.top + fontheight;
		//�s�`��
		NVMC_DrawText(rect,text,kJustification_Left);
	}
}

