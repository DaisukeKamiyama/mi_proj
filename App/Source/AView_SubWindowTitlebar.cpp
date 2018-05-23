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

AView_SubWindowTitlebar

*/

#include "stdafx.h"

#include "AView_SubWindowTitlebar.h"
#include "AApp.h"


//
const ANumber	kWidth_LeftMargin_Overlay = 4;
const ANumber	kWidth_FoldingTriangleIcon = 15;
//
const ANumber	kWidth_CloseButton = 19;

#pragma mark ===========================
#pragma mark [�N���X]AView_SubWindowTitlebar
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^

@param inWindowID �e�E�C���h�E��ID
@param inID View��ControlID
*/
AView_SubWindowTitlebar::AView_SubWindowTitlebar( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), /*mDragReceiving(false),*/ mMouseTrackStartLocalPoint(kLocalPoint_00)
,mDraggingOverlayWindow(false),mTitleIconImageID(kImageID_Invalid)
,mMouseHoverCloseButton(false),mMouseHoverFoldingButton(false)
,mMousePressingFoldingButton(false),mMousePressingCloseButton(false)
{
}

/**
�f�X�g���N�^
*/
AView_SubWindowTitlebar::~AView_SubWindowTitlebar()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_SubWindowTitlebar::NVIDO_Init()
{
	/*
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_SubWindowTitlebar);
	NVMC_EnableDrop(scrapTypeArray);
	*/
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_SubWindowTitlebar::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_SubWindowTitlebar::EVTDO_DoDraw()
{
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	
	//---------------�^�C�g���o�[�C���[�W�`��---------------
	ALocalPoint	pt = {0,0};
	if( IsOverlay() == true )
	{
		//---------------�I�[�o�[���C��---------------
		
		//�^�C�g���o�[Image�`��
		NVMC_DrawImageFlexibleWidth(kImageID_SubWindowTitlebar_Overlay0,kImageID_SubWindowTitlebar_Overlay1,kImageID_SubWindowTitlebar_Overlay2,
					pt,localFrameRect.right-localFrameRect.left);
		
		//�܂肽���ݏ�Ԏ擾
		ABool	collapsed = false;
		AWindowID	textWindowID = NVM_GetWindow().NVI_GetOverlayParentWindowID();
		if( textWindowID != kObjectID_Invalid )
		{
			collapsed = GetApp().SPI_GetTextWindowByID(textWindowID).SPI_IsCollapsedSubWindow(NVM_GetWindow().GetObjectID());
		}
		
		//---------------�܂肽���݃C���[�W�`��---------------
		{
			ALocalPoint	pt = {0};
			pt.x = localFrameRect.left+kWidth_LeftMargin_Overlay;
			pt.y = localFrameRect.top+2;
			if( collapsed == true )
			{
				//�܂肽���ݎ��g���C�A���O���A�C�R��
				if( mMousePressingFoldingButton == true )
				{
					//������
					NVMC_DrawImage(kImageID_barSwCursorRight_p,pt);
				}
				else if( mMouseHoverFoldingButton == false )
				{
					//�ʏ펞
					NVMC_DrawImage(kImageID_barSwCursorRight,pt);
				}
				else
				{
					//�z�o�[��
					NVMC_DrawImage(kImageID_barSwCursorRight_h,pt);
				}
			}
			else
			{
				//�W�J���g���C�A���O���A�C�R��
				if( mMousePressingFoldingButton == true )
				{
					//������
					NVMC_DrawImage(kImageID_barSwCursorDown_p,pt);
				}
				else if( mMouseHoverFoldingButton == false )
				{
					//�ʏ펞
					NVMC_DrawImage(kImageID_barSwCursorDown,pt);
				}
				else
				{
					//�z�o�[��
					NVMC_DrawImage(kImageID_barSwCursorDown_h,pt);
				}
			}
		}
	}
	else
	{
		//---------------�t���[�e�B���O�^�|�b�v�A�b�v��---------------
		
		NVMC_DrawImageFlexibleWidth(kImageID_SubWindowTitlebar_Floating0,kImageID_SubWindowTitlebar_Floating1,kImageID_SubWindowTitlebar_Floating2,
					pt,localFrameRect.right-localFrameRect.left);
		
		//---------------�N���[�Y�{�^���C���[�W�`��---------------
		{
			ALocalPoint	pt = {0};
			pt.x = localFrameRect.left+2;
			pt.y = localFrameRect.top+2;
			if( mMousePressingCloseButton == true )
			{
				//������
				NVMC_DrawImage(kImageID_barSwCloseButton_p,pt);
			}
			else if( mMouseHoverCloseButton == false )
			{
				//�ʏ펞
				NVMC_DrawImage(kImageID_barSwCloseButton_1,pt);
			}
			else
			{
				//�z�o�[��
				NVMC_DrawImage(kImageID_barSwCloseButton_h,pt);
			}
		}
	}
	
	//�t�H���g�ݒ�i�_�C�A���O�p�W���t�H���g�j
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);
	NVMC_SetDefaultTextProperty(fontname,9.0,kColor_Gray90Percent,kTextStyle_Bold,true);
	
	//�^�C�g���o�[�e�L�X�g�`��
	ALocalRect	rect =	localFrameRect;
	rect.top = 3;
	rect.bottom = 17;
	AColor	color = GetApp().SPI_GetSubWindowTitlebarTextColor();
	NVMC_SetDropShadowColor(GetApp().SPI_GetSubWindowTitlebarTextDropShadowColor());
	AJustification	justification = kJustification_Center;
	ABool	titleJustificationLeft = (IsOverlay() == true );
	if( titleJustificationLeft == true )
	{
		rect.left = 38;
		justification = kJustification_Left;
	}
	NVMC_DrawText(rect,mTitleText,color,kTextStyle_Bold|kTextStyle_DropShadow,justification);
	
	//�^�C�g���o�[�A�C�R���`��
	if( mTitleIconImageID != kImageID_Invalid )
	{
		ALocalPoint	pt = {0};
		pt.x = localFrameRect.left + kWidth_LeftMargin_Overlay + kWidth_FoldingTriangleIcon;
		pt.y = localFrameRect.top+3;
		if( titleJustificationLeft == false )
		{
			pt.x = (localFrameRect.left+localFrameRect.right)/2-NVMC_GetDrawTextWidth(mTitleText)/2 - 18;
		}
		NVMC_DrawImage(mTitleIconImageID,pt);
	}
}

/**
�}�E�X�{�^���_�E��������
*/
ABool	AView_SubWindowTitlebar::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//tracking�J�n���}�E�X�ʒu�L��
	mMouseTrackStartLocalPoint = inLocalPoint;
	
	//�N���[�Y�{�^���^�܂肽���݃A�C�R���N���b�N����
	mMousePressingFoldingButton = false;
	mMousePressingCloseButton = false;
	if( IsPointInCloseButton(mMouseTrackStartLocalPoint) == true )
	{
		if( IsPointInCloseButton(inLocalPoint) == true )
		{
			mMousePressingCloseButton = true;
		}
		NVI_Refresh();
	}
	if( IsPointInFoldingButton(mMouseTrackStartLocalPoint) == true )
	{
		if( IsPointInFoldingButton(inLocalPoint) == true )
		{
			mMousePressingFoldingButton = true;
		}
		NVI_Refresh();
	}
	
	//Mouse tracking�J�n
	NVM_SetMouseTrackingMode(true);
	NVI_Refresh();
	return true;
}

/**
�}�E�X�{�^��������Tracking��OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_SubWindowTitlebar::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�N���[�Y�{�^���^�܂肽���݃A�C�R���N���b�N����
	mMousePressingFoldingButton = false;
	mMousePressingCloseButton = false;
	if( IsPointInCloseButton(mMouseTrackStartLocalPoint) == true )
	{
		if( IsPointInCloseButton(inLocalPoint) == true )
		{
			mMousePressingCloseButton = true;
		}
		NVI_Refresh();
		return;
	}
	if( IsPointInFoldingButton(mMouseTrackStartLocalPoint) == true )
	{
		if( IsPointInFoldingButton(inLocalPoint) == true )
		{
			mMousePressingFoldingButton = true;
		}
		NVI_Refresh();
		return;
	}
	
	//==================�^�C�g���o�[Drag==================
	//tracking�J�n���ƈႤ�}�E�X�ʒu�ɂȂ�����drag�J�n
	if( inLocalPoint.x != mMouseTrackStartLocalPoint.x || inLocalPoint.y != mMouseTrackStartLocalPoint.y )
	{
		//Drag�����t���OON
		if( mDraggingOverlayWindow == false )
		{
			mDraggingOverlayWindow = true;
		}
	}
	//�E�C���h�EDrag�ړ��E�T�C�h�o�[Dock
	if( mDraggingOverlayWindow == true )
	{
		//�}�E�X�ʒu�擾
		AGlobalPoint	mouseGlobalPoint = {0};
		AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
		//�E�C���h�Ebounds�擾
		ARect	rect = {0};
		NVM_GetWindow().NVI_GetWindowBounds(rect);
		ANumber	w = rect.right - rect.left;
		ANumber	h = rect.bottom - rect.top;
		//�}�E�X�ʒu�ɉ������V�K�E�C���h�E����|�C���g���v�Z���A�X�N���[�����ɕ␳
		APoint	pt = {0};
		pt.x = mouseGlobalPoint.x - mMouseTrackStartLocalPoint.x;
		pt.y = mouseGlobalPoint.y - mMouseTrackStartLocalPoint.y;
		NVM_GetWindow().NVI_ConstrainWindowPosition(true,pt);
		//�V�K�E�C���h�Ebounds�擾
		rect.left = pt.x;
		rect.top = pt.y;
		rect.right = rect.left + w;
		rect.bottom = rect.top + h;
		//�t���[�e�B���O�E�C���h�E�Ԃ̋z���␳
		GetApp().SPI_DockSubWindowsToSubWindows(rect);
		//�E�C���h�Ebounds�ݒ�
		NVM_GetWindow().NVI_SetWindowBounds(rect);
		//�őO�ʂ̃e�L�X�g�E�C���h�E�̃T�u�E�C���h�Edock����
		AWindowID	textWindowID = NVM_GetWindow().NVI_GetOverlayParentWindowID();
		if( textWindowID == kObjectID_Invalid )
		{
			textWindowID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
		}
		if( textWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextWindowByID(textWindowID).SPI_DragSubWindow(NVM_GetWindow().GetObjectID(),mouseGlobalPoint);
		}
	}
	NVI_Refresh();
}

/**
�}�E�X�{�^��������Tracking�I������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_SubWindowTitlebar::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�N���[�Y�{�^���^�܂肽���݃A�C�R���N���b�N����
	mMousePressingFoldingButton = false;
	mMousePressingCloseButton = false;
	NVI_Refresh();
	//
	if( IsPointInCloseButton(mMouseTrackStartLocalPoint) == true )
	{
		if( IsPointInCloseButton(inLocalPoint) == true )
		{
			NVM_GetWindow().EVT_DoCloseButton();
		}
		return;
	}
	if( IsPointInFoldingButton(mMouseTrackStartLocalPoint) == true )
	{
		if( IsPointInFoldingButton(inLocalPoint) == true )
		{
			AWindowID	textWindowID = NVM_GetWindow().NVI_GetOverlayParentWindowID();
			if( textWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(textWindowID).SPI_ExpandCollapseSubWindow(NVM_GetWindow().GetObjectID());
				return;
			}
		}
		return;
	}
	
	//�őO�ʂ̃e�L�X�g�E�C���h�E�̃T�u�E�C���h�Edock��������
	AWindowID	textWindowID = NVM_GetWindow().NVI_GetOverlayParentWindowID();
	if( textWindowID == kObjectID_Invalid )
	{
		textWindowID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
	}
	if( textWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(textWindowID).SPI_EndDragSubWindow(NVM_GetWindow().GetObjectID());
	}
	//Drag���Ȃ������ꍇ�́A�E�C���h�E�ɃN���b�N��ʒm
	if( mDraggingOverlayWindow == false )
	{
		//�E�C���h�E�ɃN���b�N��ʒm
		NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
	}
	//Drag�����t���OOFF
	mDraggingOverlayWindow = false;
	
	//Tracking����
	NVM_SetMouseTrackingMode(false);
}

/**
�w��|�C���g���N���b�N�{�^�������ǂ����̔���
*/
ABool	AView_SubWindowTitlebar::IsPointInCloseButton( const ALocalPoint& inLocalPoint ) const
{
	if( IsOverlay() == true )
	{
		return false;
	}
	//
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	return (inLocalPoint.x >= localFrameRect.left && inLocalPoint.x <= localFrameRect.left + kWidth_CloseButton &&
		inLocalPoint.y >= localFrameRect.top && inLocalPoint.y <= localFrameRect.bottom );
}

/**
�w��|�C���g���܂肽���݃}�[�N�����ǂ����̔���
*/
ABool	AView_SubWindowTitlebar::IsPointInFoldingButton( const ALocalPoint& inLocalPoint ) const
{
	if( IsOverlay() == false )
	{
		return false;
	}
	//
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	return (inLocalPoint.x >= localFrameRect.left && inLocalPoint.x <= localFrameRect.left + kWidth_LeftMargin_Overlay+kWidth_FoldingTriangleIcon &&
		inLocalPoint.y >= localFrameRect.top && inLocalPoint.y <= localFrameRect.bottom );
}

/**
�}�E�X�ړ�����OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�J�[�\���̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
ABool	AView_SubWindowTitlebar::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�N���[�Y�{�^���^�܂肽���݃A�C�R���z�o�[����
	mMouseHoverCloseButton = false;
	mMouseHoverFoldingButton = false;
	if( IsPointInCloseButton(inLocalPoint) == true )
	{
		mMouseHoverCloseButton = true;
	}
	if( IsPointInFoldingButton(inLocalPoint) == true )
	{
		mMouseHoverFoldingButton = true;
	}
	NVI_Refresh();
	return true;
}

/**
�}�E�X��View�O�ֈړ���������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�J�[�\���̏ꏊ�i�R���g���[�����[�J�����W�j
*/
void	AView_SubWindowTitlebar::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	//�z�o�[����
	mMouseHoverCloseButton = false;
	mMouseHoverFoldingButton = false;
	NVI_Refresh();
}

#if 0
/**
Drag&Drop�����iDrag���j
*/
void	AView_SubWindowTitlebar::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )
{
}

/**
Drag&Drop�����iDrag��View�ɓ������j
*/
void	AView_SubWindowTitlebar::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_SubWindowTitlebar) == true )
	{
		mDragReceiving = true;
		NVI_Refresh();
	}
}

/**
Drag&Drop�����iDrag��View����o���j
*/
void	AView_SubWindowTitlebar::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_SubWindowTitlebar) == true )
	{
		mDragReceiving = false;
		NVI_Refresh();
	}
}

/**
Drag&Drop�����iDrop�j
*/
void	AView_SubWindowTitlebar::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_SubWindowTitlebar) == true )
	{
		AText	text;
		AScrapWrapper::GetDragData(inDragRef,kScrapType_SubWindowTitlebar,text);
		
		//
		ASubWindowLocationType	subWindowLocationType = kSubWindowLocationType_None;
		AIndex	subWindowLocationIndex = kIndex_Invalid;
		GetApp().SPI_GetSubWindowProperty(NVM_GetWindow().GetObjectID(),subWindowLocationType,subWindowLocationIndex);
		//
		ANumber	num = text.GetNumber();
		ASubWindowLocationType	droppedType = (ASubWindowLocationType)(num/100);
		AIndex	droppedIndex = (num%100);
		if( droppedIndex == 99 )
		{
			GetApp().SPI_AddOverlaySubWindow(subWindowLocationType,subWindowLocationIndex,(ASubWindowType)(num/100));
		}
		else
		{
			GetApp().SPI_MoveOverlaySubWindow(droppedType,droppedIndex,subWindowLocationType,subWindowLocationIndex);
		}
	}
}
#endif
/**
�J�[�\���^�C�v�擾
*/
ACursorType	AView_SubWindowTitlebar::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	return kCursorType_Arrow;
}

/**
*/
ABool	AView_SubWindowTitlebar::IsOverlay() const
{
	return (NVM_GetWindowConst().NVI_GetOverlayParentWindowID() != kObjectID_Invalid);
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

/**
�^�C�g���o�[�e�L�X�g�ƃA�C�R���ݒ�
*/
void	AView_SubWindowTitlebar::SPI_SetTitleTextAndIconImageID( const AText& inText, const AImageID inIconImageID )
{
	mTitleText.SetText(inText);
	mTitleIconImageID = inIconImageID;
	NVI_Refresh();
}


#pragma mark ===========================
#pragma mark [�N���X]AView_SubWindowBackground
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^

@param inWindowID �e�E�C���h�E��ID
@param inID View��ControlID
*/
AView_SubWindowBackground::AView_SubWindowBackground( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID)
{
}

/**
�f�X�g���N�^
*/
AView_SubWindowBackground::~AView_SubWindowBackground()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_SubWindowBackground::NVIDO_Init()
{
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_SubWindowBackground::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_SubWindowBackground::EVTDO_DoDraw()
{
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	
	//==================���P�[�V�����^�C�v���̏���==================
	ASubWindowLocationType	locationType = GetApp().SPI_GetSubWindowLocationType(NVM_GetWindowConst().GetObjectID());
	switch(locationType)
	{
		//�|�b�v�A�b�v�p�`��
	  case kSubWindowLocationType_Popup:
		{
			//==================�F�擾==================
			
			//�`��F�ݒ�
			AColor	letterColor = kColor_Black;
			AColor	dropShadowColor = kColor_White;
			AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
			GetApp().SPI_GetSubWindowBoxColor(NVM_GetWindow().GetObjectID(),letterColor,dropShadowColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
			
			//
			switch( GetApp().SPI_GetSubWindowType(NVM_GetWindow().GetObjectID()) )
			{
				//�⊮��⃊�X�g�̏ꍇ�Arounded rect��`��
			  case kSubWindowType_CandidateList:
				{
					//�F�E���擾
					AColor	backgroundColor = kColor_White;
					AFloatNumber	backgroundAlpha = GetApp().SPI_GetPopupWindowAlpha()+0.1;
					//�`��rect�擾�iframe����1pixel inset����j
					ALocalRect	rect = localFrameRect;
					rect.top		+= 1;
					rect.left		+= 1;
					rect.right		-= 1;
					rect.bottom		-= kHeight_CandidateListStatusBar-1;
					//�`��
					NVMC_PaintRoundedRect(rect,
										  //kColor_White,kColor_Gray95Percent
										  boxBaseColor1,boxBaseColor2,
										  kGradientType_Vertical,backgroundAlpha,backgroundAlpha,
										  kRadius_PopupCandidateList,true,true,false,false);
					NVMC_FrameRoundedRect(rect,
										  //kColor_Gray20Percent
										  //kColor_Gray50Percent,
										  letterColor,
										  0.5,kRadius_PopupCandidateList,true,true,false,false,
										  true,true,true,false,kLineWidth_PopupCandidateList);
					break;
				}
				//
			  case kSubWindowType_KeyToolList:
				{
					//�F�E���擾
					AColor	backgroundColor = kColor_White;
					AFloatNumber	backgroundAlpha = GetApp().SPI_GetPopupWindowAlpha() +0.1;
					//�`��rect�擾�iframe����1pixel inset����j
					ALocalRect	rect = localFrameRect;
					rect.top		+= 1;
					rect.left		+= 1;
					rect.right		-= 1;
					rect.bottom		-= 1;
					//�`��
					NVMC_PaintRoundedRect(rect,
										  //kColor_White,kColor_Gray95Percent,
										  boxBaseColor1,boxBaseColor2,
										  kGradientType_Vertical,backgroundAlpha,backgroundAlpha,	
										  kRadius_PopupKeyToolList,true,true,true,true);
					NVMC_FrameRoundedRect(rect,
										  //kColor_Gray20Percent,
										  //kColor_Gray50Percent,
										  letterColor,
										  0.5,kRadius_PopupKeyToolList,true,true,true,true,
										  true,true,true,true,kLineWidth_PopupKeyToolList);
					break;
				}
				//����ȊO�̏ꍇ
			  default:
				{
					AColor	color = kColor_Gray80Percent;
					NVMC_FrameRect(localFrameRect,color);
					break;
				}
			}
			break;
		}
		//�t���[�e�B���O�E�C���h�E
	  case kSubWindowLocationType_Floating:
		{
			//�t���[�e�B���O�E�C���h�E�w�i�F�E���擾
			AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
			AFloatNumber	backgroundAlpha = GetApp().SPI_GetFloatingWindowAlpha();
			//�S�̂ɍ����A�E��R=5.0��rounded rect��`��
			NVMC_PaintRoundedRect(localFrameRect,
								  backgroundColor,backgroundColor,kGradientType_Horizontal,backgroundAlpha,backgroundAlpha,
								  5.0,false,false,true,true);
			break;
		}
		//�E�T�C�h�o�[
	  case kSubWindowLocationType_RightSideBar:
		{
			//�S�̂��T�u�E�C���h�E�w�i�F�Ńy�C���g
			AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
			NVMC_PaintRect(localFrameRect,backgroundColor);
			//�E�[�ȊO���T�C�h�o�[�t���[���F�ŕ`��
			AColor	color = GetApp().SPI_GetSideBarFrameColor();
			ABool	drawBottom = !(GetApp().SPI_IsSubWindowSideBarBottom(NVM_GetWindow().GetObjectID()));
			NVMC_FrameRect(localFrameRect,color,1.0,true,true,false,drawBottom,1.0);
			break;
		}
		//���T�C�h�o�[
	  case kSubWindowLocationType_LeftSideBar:
		{
			//�S�̂��T�u�E�C���h�E�w�i�F�Ńy�C���g
			AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
			NVMC_PaintRect(localFrameRect,backgroundColor);
			//���[�ȊO���T�C�h�o�[�t���[���F�ŕ`��
			AColor	color = GetApp().SPI_GetSideBarFrameColor();
			ABool	drawBottom = !(GetApp().SPI_IsSubWindowSideBarBottom(NVM_GetWindow().GetObjectID()));
			NVMC_FrameRect(localFrameRect,color,1.0,false,true,true,drawBottom,1.0);
			break;
		}
	}
	/*
	//
	ALocalRect	r = localFrameRect;
	r.bottom += 3;
	NVMC_FrameRect(r,color);
	//
	ALocalPoint	spt = {localFrameRect.left,localFrameRect.bottom-2};
	ALocalPoint	ept = {localFrameRect.right,localFrameRect.bottom-2};
	NVMC_DrawLine(spt,ept,color);
	*/
}



