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

AView_Button

*/
//#232

#include "stdafx.h"

#include "../Frame.h"

//�A�C�R���ƃe�L�X�g�̊Ԃ̃X�y�[�X
const ANumber	kSpaceBetweenIconAndText = 2;

#pragma mark ===========================
#pragma mark [�N���X]AView_Button
#pragma mark ===========================
/**
�g�\���̂��߂�View
*/

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^

@param inWindowID �e�E�C���h�E��ID
@param inID View��ControlID
*/
AView_Button::AView_Button( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mIconImageID(kImageID_Invalid), mMenuID(kContextMenuID_Invalid), mToggleMode(false), mToggleOn(false),mContextMenuID(kContextMenuID_Invalid),
		mMouseHover(false), mMouseDown(false), mIconLeftOffset(4), mIconTopOffset(4),mIconWidth(16),mIconHeight(16),
		/*#688 mMouseTrackByLoop(true), */
		mTextJustification(kJustification_Left), mFontHeight(9), mFontAscent(7), mEnableDrag(false)
		,mTransparency(1.0), mAlwaysActiveColors(false)//#291
		,mFrameColor(kColor_Gray80Percent)//amazon
		,mMouseTrackResultIsDrag(false)//win
		,mColor(kColor_Gray10Percent),mColorDeactive(kColor_Gray60Percent)
,mButtonViewType(kButtonViewType_None)//kButtonViewType_Rect20)//#634
,mButtonBehaviorType(kButtonBehaviorType_Normal)//#634
,mPreviousMouseGlobalPoint(kGlobalPoint_00)//#634
,mButtonValidXRangeStart(-1),mButtonValidXRangeEnd(-1)//#634
,mHelpTagSide(kHelpTagSide_Default)//#661
,mFontSize(9.0), mTextStyle(kTextStyle_Normal)//#724
,mDisplayingContextMenu(false)//#724
,mDropShadowColor(kColor_White)//#724
,mEllipsisMode(kEllipsisMode_FixedLastCharacters),mEllipsisCharacterCount(5)//#725
,mDragging(false)//#850
,mTemporaryInvisible(false)//#724
,mMenuItemID(0)//#688
,mHoverIconImageID(kImageID_Invalid)
,mToggleOnIconImageID(kImageID_Invalid)
,mOvalHoverColor(kColor_Gray50Percent)
{
	NVMC_SetOffscreenMode(true);//win
}
/**
�f�X�g���N�^
*/
AView_Button::~AView_Button()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_Button::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
	/*#852
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);//#375
	SPI_SetTextProperty(defaultfontname,9.0,kJustification_Left);
	*/
	/*#688
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
}

//#852
/**
���������Ȃ珉��������i�������̂��߁A�{���ɕ`�悳��钼�O��SPI_SetTextProperty()�����s����j
*/
void	AView_Button::InitTextPropertyIfNotInited()
{
	if( mFontName.GetItemCount() > 0 )   return;
	
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);//#375
	SPI_SetTextProperty(defaultfontname,mFontSize,mTextJustification,mTextStyle,mColor,mColorDeactive);
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_Button::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�}�E�X�{�^����������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
@param inClickCout �N���b�N��
*/
ABool	AView_Button::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//�R���g���[��disable�Ȃ牽�����Ȃ�
	if( NVMC_IsControlEnabled() == false )
	{
		return false;
	}
	
	//#634 �{�^���L���͈�
	if( mButtonValidXRangeStart != -1 && mButtonValidXRangeEnd != -1 )
	{
		if( inLocalPoint.x < mButtonValidXRangeStart || inLocalPoint.x > mButtonValidXRangeEnd )
		{
			return false;
		}
	}
	
	//#634 VSeparator���[�h
	if( mButtonBehaviorType == kButtonBehaviorType_VSeparator )
	{
		return DoMouseDown_VSeparator(inLocalPoint,inModifierKeys,inClickCount);
	}
	
	ALocalRect	frame;
	NVM_GetLocalViewRect(frame);
	//#256 win������ړ�
	mMouseDown = true;
	
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	
	//TrackingMode�ݒ�
	mMouseTrackResultIsDrag = false;
	NVM_SetMouseTrackingMode(true);
	NVI_Refresh();//win
	return true;
	
	//#688 }
	/*#688
	//
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	mMouseTrackResultIsDrag = false;
	if( (mMenuID == kIndex_Invalid && mMenuTextArray.GetItemCount() == 0) || (AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true) )//#276
	{
		while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
		{
			ALocalPoint	mouseLocalPoint;
			NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
			if(	mouseLocalPoint.x >= frame.left && mouseLocalPoint.x <= frame.right &&
				mouseLocalPoint.y >= frame.top && mouseLocalPoint.y <= frame.bottom )
			{
				mMouseDown = true;
			}
			else
			{
				mMouseDown = false;
				if( mEnableDrag == true )
				{
					mMouseTrackResultIsDrag = true;
					NVI_Refresh();
					break;
				}
			}
			NVI_Refresh();
		}
	}
	if( mMouseTrackResultIsDrag == true )
	{
		StartDrag(inLocalPoint,inModifierKeys);//win
		return true;
	}
	//
	MouseDown(inModifierKeys);
	return true;
	*/
}

//win
void	AView_Button::StartDrag( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mMouseTrackResultIsDrag == true )
	{
		ALocalRect	frame;
		NVM_GetLocalViewRect(frame);
		//Drag�̈�ݒ�
		AArray<ALocalRect>	dragRect;
		dragRect.Add(frame);
		//Scrap�ݒ�
		AArray<AScrapType>	scrapType;
		ATextArray	data;
		scrapType.Add(kScrapType_ButtonView);
		AUniqID	viewUniqID = AApplication::GetApplication().NVI_GetViewUniqID(GetObjectID());//#693
		AText	text;
		text.AddNumber(viewUniqID.val);//#693
		data.Add(text);
		//Drag���s
		//#850 NVMC_DragText(inLocalPoint,dragRect,scrapType,data);
		AWindowRect	wrect = {0};
		NVM_GetWindow().NVI_GetWindowRectFromControlLocalRect(NVI_GetControlID(),frame,wrect);
		mDragging = true;
		if( MVMC_DragTextWithWindowImage(inLocalPoint,scrapType,data,wrect,wrect.right-wrect.left,wrect.bottom-wrect.top) == false )
		{
			NVM_GetWindow().OWICB_ButtonViewDragCanceled(NVI_GetControlID());
		}
		mDragging = false;
	}
}

/**
�}�E�X�{�^���_�E��������
*/
void	AView_Button::MouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalRect	frame;
	NVM_GetLocalViewRect(frame);
	
	//Toggle
	if( mMouseDown == true && mToggleMode == true )
	{
		mToggleOn = !mToggleOn;
	}
	//�{�^����ControlID�ۑ��iWindow��EVTDO_DoMenuItemSelected()�łǂ̃{�^���������ꂽ���𔻒肷�邽�߁j
	NVM_GetWindow().NVI_SetLastClickedButtonControlID(NVI_GetControlID());
	//���j���[�`���{�^���̏ꍇ�A���j���[��\������
	//���j���[�N���b�N��AWindow��EVTDO_DoMenuItemSelected()�ɓn�����
	//�y�����z���j���[�N���b�N��܂�ShowContextMenu()�͖߂��Ă��Ȃ��̂ŁA�����őI�����j���[���ڂ�����EVT_Clicked()�ŏ�������������������̂����A
	//Window��EVTDO_DoMenuItemSelected()�ŏ�����������ʏ탁�j���[�Ə������ʉ��ł��ėǂ����Ǝv�������߁B
	//ShowContextMenu()��������Ɖ�������Ώ�L�̕������T�|�[�g���邱�Ƃ͉\�B
	if( mMouseDown == true && mMenuID != kIndex_Invalid )
	{
		//TextArrayMenu�A�����j���[�̏ꍇ
		//ContextMenu�ݒ�iMenuItemID�ƃ`�F�b�N�}�[�N�j
		if( mMenuItemID != kMenuItemID_Invalid )
		{
			AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuMenuItemID(mMenuID,mMenuItemID);
		}
		AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuCheckMark(mMenuID,mText);
		//ContextMenu�\��
		ALocalPoint	localPoint;
		localPoint.x = 0;
		localPoint.y = frame.bottom+5;
		AGlobalPoint	globalPoint;
		NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),localPoint,globalPoint);
		//#724
		mDisplayingContextMenu = true;
		NVI_Refresh();
		//#688 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(mMenuID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
		NVMC_ShowContextMenu(mMenuID,globalPoint);//#688
		mMouseDown = false;
		//#724
		mDisplayingContextMenu = false;
		//�R���e�L�X�g���j���[�����ɂ��R���g���[�����폜����Ă���\��������̂ŁA���݃`�F�b�N���s��
		if( NVM_GetWindow().NVI_ExistControl(NVI_GetControlID()) == true )
		{
			//�`��X�V
			NVI_Refresh();
			//�E�C���h�E���̃N���b�N���������s
			NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
		}
		return;
	}
	if( mMouseDown == true && mMenuTextArray.GetItemCount() > 0 )
	{
		//�Œ胁�j���[�imMenuTextArray�Ŏw��j�̏ꍇ
		//ContextMenu�ݒ�iTextArray��MeuItemID�j
		AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuTextArray(kContextMenuID_ButtonView,mMenuTextArray,mActionTextArray);
		if( mMenuItemID != kMenuItemID_Invalid )
		{
			AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuMenuItemID(kContextMenuID_ButtonView,mMenuItemID);
		}
		AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuCheckMark(kContextMenuID_ButtonView,mText);
		//ContextMenu�\��
		ALocalPoint	localPoint;
		localPoint.x = 0;
		localPoint.y = frame.bottom+5;
		AGlobalPoint	globalPoint;
		NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),localPoint,globalPoint);
		//#688 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(kContextMenuID_ButtonView,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
		NVMC_ShowContextMenu(kContextMenuID_ButtonView,globalPoint);//#688
		mMouseDown = false;
		//�R���e�L�X�g���j���[�����ɂ��R���g���[�����폜����Ă���\��������̂ŁA���݃`�F�b�N���s��
		if( NVM_GetWindow().NVI_ExistControl(NVI_GetControlID()) == true )
		{
			//�E�C���h�E���̃N���b�N���������s
			NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
		}
		return;
	}
	//
	ABool	mousedown = mMouseDown;
	//��ɕ\���X�V�iClick�C�x���g�Ń{�^���������Ȃ�\�������邩��j
	mMouseDown = false;
	NVI_Refresh();
	//Click�C�x���g���s
	if( mousedown == true )
	{
		NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
	}
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
/**
�}�E�X�{�^��������Tracking��OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_Button::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//#634 VSeparator���[�h
	if( mButtonBehaviorType == kButtonBehaviorType_VSeparator )
	{
		DoMouseTrack_VSeparator(inLocalPoint);
		return;
	}
	
	ALocalRect	frame;
	NVM_GetLocalViewRect(frame);
	if(	inLocalPoint.x >= frame.left && inLocalPoint.x <= frame.right &&
		inLocalPoint.y >= frame.top && inLocalPoint.y <= frame.bottom )
	{
		mMouseDown = true;
	}
	else
	{
		mMouseDown = false;
		//
		if( mEnableDrag == true )
		{
			mMouseTrackResultIsDrag = true;
			//�h���b�O�J�n
			StartDrag(inLocalPoint,inModifierKeys);
			//TrackingMode����
			NVM_SetMouseTrackingMode(false);
		}
	}
	//Drag�����ɂ��R���g���[���폜����Ă���\��������̂ŁA���݃`�F�b�N���Ă���Arefresh���� #724
	if( NVM_GetWindow().NVI_ExistControl(NVI_GetControlID()) == true )
	{
		NVI_Refresh();
	}
}

/**
�}�E�X�{�^��������Tracking�I������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_Button::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//#634 VSeparator���[�h
	if( mButtonBehaviorType == kButtonBehaviorType_VSeparator )
	{
		DoMouseTrackEnd_VSeparator(inLocalPoint);
		return;
	}
	
	NVM_SetMouseTrackingMode(false);
	MouseDown(inLocalPoint,inModifierKeys);
}
//#688 #endif

//
const ANumber	kLeftPadding_TextWithOvalHover = 10;
const ANumber	kRightPadding_TextWithOvalHover = 10;
const ANumber	kTopPadding_TextWithOvalHover = 2;
const ANumber	kBottomPadding_TextWithOvalHover = 2;

//
const ANumber	kWidth_MenuTriangle = 8;
//


//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_Button::EVTDO_DoDraw()
{
	//���������Ȃ珉����
	InitTextPropertyIfNotInited();
	
	ALocalRect	frame;
	NVM_GetLocalViewRect(frame);
	//�i�I�[�o�[���C�̏ꍇ�́j�����F�őS�̏��� win
	NVMC_EraseRect(frame);
	
	ABool	active = (NVM_GetWindow().NVI_IsWindowActive() == true || NVM_GetWindow().NVI_IsFloating() == true);
	if( mAlwaysActiveColors == true )//#291
	{
		active = true;
	}
	if( NVMC_IsControlEnabled() == false )//#390
	{
		active = false;
	}
	if( (mTextStyle&kTextStyle_DropShadow) != 0 )
	{
		NVMC_SetDropShadowColor(mDropShadowColor);
	}
	
	//==================Oval�z�o�[�t���e�L�X�g�^�C�v�{�^��==================
	//Oval�z�o�[�t���e�L�X�g�{�^����p����
	if( mButtonViewType == kButtonViewType_TextWithOvalHover || 
				mButtonViewType == kButtonViewType_TextWithOvalHoverWithFixedWidth ||
				mButtonViewType == kButtonViewType_TextWithNoHoverWithFixedWidth )
	{
		//------------------�e�L�X�g�`��̈�v�Z------------------
		ALocalRect	textrect = frame;
		textrect.left	= frame.left + kLeftPadding_TextWithOvalHover -1;
		textrect.top	= (frame.top+frame.bottom)/2 - (mFontHeight+1)/2;
		textrect.right	= frame.right - kRightPadding_TextWithOvalHover +1;
		textrect.bottom	= (frame.top+frame.bottom)/2 + (mFontHeight+1)/2;
		
		//�����F
		AColor	lettercolor = mColor;
		if( active == false )
		{
			lettercolor = mColorDeactive;
		}
		
		//------------------���j���[���O�p�`�`�攻��------------------
		ABool	showMenuTriangle = false;
		if( mMenuID != kIndex_Invalid || mMenuTextArray.GetItemCount() > 0 )
		{
			showMenuTriangle = true;
			//�e�L�X�g�`��̈�ύX
			textrect.right -= kWidth_MenuTriangle;
		}
		
		//------------------�z�o�[�`��------------------
		if( mButtonViewType == kButtonViewType_TextWithOvalHover || 
					mButtonViewType == kButtonViewType_TextWithOvalHoverWithFixedWidth )
		{
			//�z�o�[���A�܂��́A�R���e�L�X�g���j���[�\�����ɁA�z�o�[�\������
			if( mMouseHover == true || mDisplayingContextMenu == true || mToggleOn == true )
			{
				//�`��rect�擾
				ALocalRect	ovalrect = {0};
				GetOvalRect(ovalrect);
				//�z�o�[�`��
				NVMC_PaintRoundedRect(ovalrect,mOvalHoverColor,mOvalHoverColor,kGradientType_None,1.0,1.0,
									  kNumber_MaxNumber,true,true,true,true);
				//�h���b�v�V���h�E�F�ݒ�
				if( active == true )
				{
					NVMC_SetDropShadowColor(kColor_Gray30Percent);
				}
				else
				{
					NVMC_SetDropShadowColor(kColor_White);
				}
				//�����F�ݒ�
				lettercolor = kColor_White;
			}
			/*#
			else
			{
			//�z�o�[���łȂ��ꍇ�̕����F�ݒ�
			if( active == false )
			{
			lettercolor = mColorDeactive;
			}
			}
			*/
		}
		
		//------------------�A�C�R���`��------------------
		//�A�C�R���C���[�W�����݂��Ă�����A�A�C�R����`�悷��
		AImageID	iconImageID = mIconImageID;
		if( mToggleOn == true && mToggleOnIconImageID != kImageID_Invalid )
		{
			iconImageID = mToggleOnIconImageID;
		}
		if( iconImageID != kImageID_Invalid )
		{
			//�A�C�R���`��
			ALocalPoint	pt = {0};
			pt.x = textrect.left;
			pt.y = (textrect.top+textrect.bottom)/2 - (NVMC_GetImageHeight(iconImageID))/2;
			NVMC_DrawImage(iconImageID,pt);
			//�e�L�X�g�`��̈�ύX
			textrect.left += NVMC_GetImageWidth(iconImageID) + kSpaceBetweenIconAndText;
		}
		
		//�e�L�X�g�擾
		AText	text;
		text.SetText(mText);
		//Canonical Comp�֕ϊ��i�t�@�C�����R���f�[�^�̏ꍇ�A���_�����������Ă���ꍇ������A���̂܂܂��ƕ����\������邽�߁j
		text.ConvertToCanonicalComp();
		//ellipsis�e�L�X�g�擾
		switch( mEllipsisMode )
		{
		  case kEllipsisMode_FixedFirstCharacters:
			{
				NVI_GetEllipsisTextWithFixedFirstCharacters(mText,textrect.right-textrect.left,mEllipsisCharacterCount,text);
				break;
			}
		  case kEllipsisMode_FixedLastCharacters:
			{
				NVI_GetEllipsisTextWithFixedLastCharacters(mText,textrect.right-textrect.left,mEllipsisCharacterCount,text);
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		//�w���v�^�O�ݒ�
		NVI_SetEnableHelpTag(mText.Compare(text)==false);
		//�ꎞ�I��\���̂Ƃ��͕\�����Ȃ�
		if( mTemporaryInvisible == false )
		{
			//------------------�e�L�X�g�`��------------------
			NVMC_DrawText(textrect,text,lettercolor,mTextStyle,mTextJustification);
			//NVMC_FrameRect(textrect,kColor_Red);
		}
		
		//------------------���j���[���O�p�`�̕`��------------------
		if( showMenuTriangle == true )
		{
			ANumber	drawntextwidth = NVMC_GetDrawTextWidth(text);
			//�`��
			ALocalPoint	pt = {textrect.left + drawntextwidth +5 , (textrect.top+textrect.bottom)/2 - 8};
			NVMC_DrawImage(kImageID_iconSwTriangles,pt);
		}
		
		return;
	}
	
	//#724 #725
	//==================�ʏ�{�^���i�z�o�[�{�^���ȊO�j==================
	switch(mButtonViewType)
	{
	  case kButtonViewType_Rect20:
		{
			ALocalPoint	pt = {frame.left,frame.top};
			if( mMouseDown == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn20Rect_p_1,kImageID_btn20Rect_p_2,kImageID_btn20Rect_p_3,pt,frame.right-frame.left);
			}
			else if( mMouseHover == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn20Rect_h_1,kImageID_btn20Rect_h_2,kImageID_btn20Rect_h_3,pt,frame.right-frame.left);
			}
			else
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn20Rect_1,kImageID_btn20Rect_2,kImageID_btn20Rect_3,pt,frame.right-frame.left);
			}
			break;
		}
	  case kButtonViewType_RoundedRect20:
		{
			ALocalPoint	pt = {frame.left,frame.top};
			if( mMouseDown == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn20RoundedRect_p_1,kImageID_btn20RoundedRect_p_2,kImageID_btn20RoundedRect_p_3,pt,frame.right-frame.left);
			}
			else if( mMouseHover == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn20RoundedRect_h_1,kImageID_btn20RoundedRect_h_2,kImageID_btn20RoundedRect_h_3,pt,frame.right-frame.left);
			}
			else
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn20RoundedRect_1,kImageID_btn20RoundedRect_2,kImageID_btn20RoundedRect_3,pt,frame.right-frame.left);
			}
			break;
		}
	  case kButtonViewType_Rect16:
		{
			ALocalPoint	pt = {frame.left,frame.top};
			if( mMouseDown == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn16Rect_p_1,kImageID_btn16Rect_p_2,kImageID_btn16Rect_p_3,pt,frame.right-frame.left);
			}
			else if( mMouseHover == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn16Rect_h_1,kImageID_btn16Rect_h_2,kImageID_btn16Rect_h_3,pt,frame.right-frame.left);
			}
			else
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn16Rect_1,kImageID_btn16Rect_2,kImageID_btn16Rect_3,pt,frame.right-frame.left);
			}
			break;
		}
	  case kButtonViewType_Rect16Footer:
		{
			ALocalPoint	pt = {frame.left,frame.top};
			if( mMouseDown == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_panel16Footer_1,kImageID_panel16Footer_2,kImageID_panel16Footer_3,pt,frame.right-frame.left);
			}
			else if( mMouseHover == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_panel16Footer_1,kImageID_panel16Footer_2,kImageID_panel16Footer_3,pt,frame.right-frame.left);
			}
			else
			{
				NVMC_DrawImageFlexibleWidth(kImageID_panel16Footer_1,kImageID_panel16Footer_2,kImageID_panel16Footer_3,pt,frame.right-frame.left);
			}
			break;
		}
	  case kButtonViewType_ScrollBar:
		{
			ALocalPoint	pt = {frame.left,frame.top};
			if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
			{
				if( mMouseDown == true )
				{
					NVMC_DrawImage(kImageID_frameScrlbarTop_107,pt);
				}
				else if( mMouseHover == true )
				{
					NVMC_DrawImage(kImageID_frameScrlbarTop_107,pt);
				}
				else
				{
					NVMC_DrawImage(kImageID_frameScrlbarTop_107,pt);
				}
			}
			else
			{
				if( mMouseDown == true )
				{
					NVMC_DrawImage(kImageID_frameScrlbarTop_106,pt);
				}
				else if( mMouseHover == true )
				{
					NVMC_DrawImage(kImageID_frameScrlbarTop_106,pt);
				}
				else
				{
					NVMC_DrawImage(kImageID_frameScrlbarTop_106,pt);
				}
			}
			break;
		}
	  case kButtonViewType_NoFrame:
	  case kButtonViewType_NoFrameWithTextHover:
		{
			//��������
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
#if 0
	//
	if( active == true )
	{
		if( mMouseHover == false || mToggleOn == true )
		{
			//�ʏ��� 
			if( mToggleOn == false )
			{
				//#597
				/*
				//�{�^���㔼���\��
				ALocalRect	f1 = frame;
				f1.bottom = f1.top + (frame.bottom-frame.top)/2;
				NVMC_PaintRect(f1,kColor_Gray94Percent,mTransparency);//#291
				//�{�^��������gradient�\��
				ALocalRect	f2 = frame;
				f2.top = f1.bottom;
				AColor	startColor = kColor_Gray93Percent;
				AColor	endColor = kColor_Gray90Percent;
				NVMC_PaintRectWithVerticalGradient(f2,startColor,endColor,mTransparency,true);
				*/
				{
					ALocalPoint	pt = {frame.left,frame.top};
					NVMC_DrawImageFlexibleWidth(300,301,302/*kImageID_RectButton*/,pt,frame.right-frame.left);
				}
				//�{�^���O���`��
				/*
				switch(mButtonViewType)
				{
				  case kButtonViewType_Rect:
					{
						NVMC_FrameRect(frame,mFrameColor);//amazon kColor_Gray80Percent);//#291 kColor_Gray70Percent);
						break;
					}
					//#634
				  case kButtonViewType_RoundedCorner:
					{
						ALocalPoint	spt, ept;
						spt.x = frame.left+1;
						spt.y = frame.top;
						ept.x = frame.right-1;
						ept.y = frame.top;
						NVMC_DrawLine(spt,ept,mFrameColor);
						spt.x = frame.left+1;
						spt.y = frame.bottom-1;
						ept.x = frame.right-1;
						ept.y = frame.bottom-1;
						NVMC_DrawLine(spt,ept,mFrameColor);
						spt.x = frame.left;
						spt.y = frame.top;
						ept.x = frame.left;
						ept.y = frame.bottom-2;
						NVMC_DrawLine(spt,ept,mFrameColor);
						spt.x = frame.right-1;
						spt.y = frame.top;
						ept.x = frame.right-1;
						ept.y = frame.bottom-2;
						NVMC_DrawLine(spt,ept,mFrameColor);
						break;
					}
				}
				
				ALocalPoint	spt, ept;
				spt.x = frame.left+1;
				spt.y = frame.top+1;
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_White);
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_White);
				spt.x = frame.right-2;
				spt.y = frame.bottom-2;
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
				*/
			}
			else
			{
				/*win �g�O��on���̕`��ύX
				NVMC_PaintRect(frame,kColor_Gray80Percent,mTransparency);//#291
				NVMC_FrameRect(frame,mFrameColor);//amazon kColor_Gray80Percent);//#291 kColor_Gray70Percent);
				ALocalPoint	spt, ept;
				spt.x = frame.left+1;
				spt.y = frame.top+1;
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_Gray70Percent);
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_Gray70Percent);
				spt.x = frame.right-2;
				spt.y = frame.bottom-2;
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_White);
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_White);
				*/
				AColor	hilightColor;
				AColorWrapper::GetHighlightColor(hilightColor);
				NVMC_PaintRect(frame,kColor_Gray92Percent);
				NVMC_PaintRect(frame,hilightColor,0.7);
				NVMC_FrameRect(frame,hilightColor);
				ALocalPoint	spt, ept;
				spt.x = frame.left+1;
				spt.y = frame.top+1;
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_White);
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_White);
				spt.x = frame.right-2;
				spt.y = frame.bottom-2;
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
			}
		}
		else
		{
			//�}�E�X�I�[�o�[
			if( mMouseDown == true )
			{
				//�}�E�X�_�E�����̃}�E�X�I�[�o�[ 
				AColor	hilightColor;
				AColorWrapper::GetHighlightColor(hilightColor);
				NVMC_PaintRect(frame,kColor_Gray92Percent);
				NVMC_PaintRect(frame,hilightColor,0.7);
				NVMC_FrameRect(frame,hilightColor);
				ALocalPoint	spt, ept;
				spt.x = frame.left+1;
				spt.y = frame.top+1;
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_White);
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_White);
				spt.x = frame.right-2;
				spt.y = frame.bottom-2;
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
			}
			else
			{
				//�ʏ�̃}�E�X�I�[�o�[ 
				AColor	hilightColor;
				AColorWrapper::GetHighlightColor(hilightColor);
				NVMC_PaintRect(frame,kColor_Gray92Percent);
				NVMC_PaintRect(frame,hilightColor,0.2);
				NVMC_FrameRect(frame,hilightColor);
				ALocalPoint	spt, ept;
				spt.x = frame.left+1;
				spt.y = frame.top+1;
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_White);
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_White);
				spt.x = frame.right-2;
				spt.y = frame.bottom-2;
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
			}
		}
	}
	//�E�C���h�E��deactive
	else
	{
		NVMC_PaintRect(frame,kColor_Gray92Percent);
		NVMC_FrameRect(frame,kColor_Gray80Percent);
		ALocalPoint	spt, ept;
		spt.x = frame.left+1;
		spt.y = frame.top+1;
		ept.x = frame.left+1;
		ept.y = frame.bottom-2;
		NVMC_DrawLine(spt,ept,kColor_White);
		ept.x = frame.right-2;
		ept.y = frame.top+1;
		NVMC_DrawLine(spt,ept,kColor_White);
		spt.x = frame.right-2;
		spt.y = frame.bottom-2;
		ept.x = frame.right-2;
		ept.y = frame.top+1;
		NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
		ept.x = frame.left+1;
		ept.y = frame.bottom-2;
		NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
	}
#endif
	
	//------------------�`��̈�v�Z------------------
	ALocalRect	drawrect = frame;
	drawrect.left += 3;
	drawrect.top = (frame.top+frame.bottom)/2 - mFontHeight/2 -1;
	drawrect.right -= 3;
	drawrect.bottom = (frame.top+frame.bottom)/2 + mFontHeight/2 +1;
	
	//------------------�A�C�R���\�����擾------------------
	ANumber	iconImageWidth = 0;
	if( mIconImageID != kImageID_Invalid )
	{
		iconImageWidth = mIconWidth+4;
	}
	
	//------------------���j���[���\�����擾------------------
	ANumber	menuTriangleWidth = 0;
	if( mMenuID != kIndex_Invalid || mMenuTextArray.GetItemCount() > 0 )
	{
		menuTriangleWidth = kWidth_MenuTriangle + 4;
	}
	
	//------------------�Z���^�����O�Ȃ�`��̈�𒆉��񂹂ɂ���------------------
	switch(mTextJustification)
	{
	  case kJustification_Center:
	  case kJustification_CenterTruncated:
		{
			ANumber	textwidth = NVMC_GetDrawTextWidth(mText);
			if( drawrect.right - drawrect.left > textwidth + iconImageWidth + menuTriangleWidth )
			{
				ANumber	m = ((drawrect.left+iconImageWidth)+(drawrect.right-menuTriangleWidth))/2;
				drawrect.left	= m - textwidth/2 - iconImageWidth -2;
				drawrect.right	= m + textwidth/2 + menuTriangleWidth +2;
				//NVMC_FrameRect(drawrect,kColor_Black);
			}
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	
	//------------------�A�C�R���`��------------------
	if( iconImageWidth > 0 )
	{
		switch(mTextJustification)
		{
		  case kJustification_Center:
		  case kJustification_CenterTruncated:
			{
				//
				ALocalPoint	pt = {drawrect.left,mIconTopOffset};
				if( mMouseHover == true && mHoverIconImageID != kImageID_Invalid )
				{
					NVMC_DrawImage(mHoverIconImageID,pt);
				}
				else
				{
					NVMC_DrawImage(mIconImageID,pt);
				}
				break;
			}
		  default:
			{
				//
				ALocalPoint	pt = {mIconLeftOffset,mIconTopOffset};
				if( mIconLeftOffset < 0 )
				{
					pt.x = frame.right+mIconLeftOffset;
				}
				if( mMouseHover == true && mHoverIconImageID != kImageID_Invalid )
				{
					NVMC_DrawImage(mHoverIconImageID,pt);
				}
				else
				{
					NVMC_DrawImage(mIconImageID,pt);
				}
				break;
			}
		}
	}
	//------------------Menu�p���`��------------------
	if( menuTriangleWidth > 0 )
	{
		ALocalPoint	pt = {drawrect.right - kWidth_MenuTriangle,(drawrect.top+drawrect.bottom)/2 - 7};
		NVMC_DrawImage(kImageID_iconSwTriangles,pt);
	}
	//------------------�e�L�X�g�`��------------------
	if( mText.GetItemCount() > 0 )
	{
		//
		ALocalRect	textrect = drawrect;
		textrect.left += iconImageWidth;
		textrect.right -= menuTriangleWidth;
		//�e�L�X�g�擾
		AText	text;
		text.SetText(mText);
		//Canonical Comp�֕ϊ��i�t�@�C�����R���f�[�^�̏ꍇ�A���_�����������Ă���ꍇ������A���̂܂܂��ƕ����\������邽�߁j
		text.ConvertToCanonicalComp();
		//ellipsis�e�L�X�g�擾
		switch( mEllipsisMode )
		{
		  case kEllipsisMode_FixedFirstCharacters:
			{
				NVI_GetEllipsisTextWithFixedFirstCharacters(mText,textrect.right-textrect.left,mEllipsisCharacterCount,text);
				break;
			}
		  case kEllipsisMode_FixedLastCharacters:
			{
				NVI_GetEllipsisTextWithFixedLastCharacters(mText,textrect.right-textrect.left,mEllipsisCharacterCount,text);
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		//�F�擾
		AColor	color = mColor;
		//
		if( mButtonViewType == kButtonViewType_NoFrameWithTextHover )
		{
			if( mMouseHover == true )
			{
				//AColorWrapper::GetHighlightColor(color);
				color = kColor_Blue;
			}
		}
		
		if( active == false )
		{
			color = mColorDeactive;
		}
		
		//
		switch(mButtonViewType)
		{
		  case kButtonViewType_Rect16:
			{
				if( active == true )
				{
					color = kColor_Gray10Percent;
				}
				else
				{
					color = kColor_Gray60Percent;
				}
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		
		//�e�L�X�g�X�^�C���擾�iactive���̓h���b�v�V���h�E�j
		ATextStyle	style = kTextStyle_DropShadow;
		if( active == false )
		{
			style = kTextStyle_Normal;
		}
		
		//�e�L�X�g�`��
		NVMC_DrawText(textrect,text,color,style,mTextJustification);
	}
	//fprintf(stderr,"AView_Button::EVTDO_DoDraw()");
}

/**
Oval�{�^���̏ꍇ��Oval�̈�擾
*/
void	AView_Button::GetOvalRect( ALocalRect& outOvalRect ) const
{
	ALocalRect	frame = {0};
	NVM_GetLocalViewRect(frame);
	
	//���~�����̕����v
	const ANumber	kWidth_OvalWidth = kLeftPadding_TextWithOvalHover + kRightPadding_TextWithOvalHover;
	
	//�e�L�X�g�\�����擾
	ANumber	textwidth = NVMC_GetDrawTextWidth(mText);
	
	//���j���[���\������
	ABool	showMenuTriangle = false;
	if( mMenuID != kIndex_Invalid || mMenuTextArray.GetItemCount() > 0 )
	{
		showMenuTriangle = true;
	}
	
	//
	outOvalRect = frame;
	//
	outOvalRect.top += 1;
	outOvalRect.bottom -= 1;
	//�Œ蕝�^�C�v�̏ꍇ�́A�e�L�X�g���ɍ��킹�ĕ��ݒ�
	if( mButtonViewType == kButtonViewType_TextWithOvalHoverWithFixedWidth ||
				mButtonViewType == kButtonViewType_TextWithNoHoverWithFixedWidth )
	{
		//���j���[���O�p�`�����̕��擾
		ANumber	iconsWidth = 0;
		if( showMenuTriangle == true )
		{
			iconsWidth = kWidth_MenuTriangle;
		}
		if( mIconImageID != kImageID_Invalid )
		{
			//�A�C�R���\����
			iconsWidth += mIconWidth;
		}
		//�e�L�X�g���{���j���[���O�p�`�������Aovalrect�����傫���Ƃ��́A�e�L�X�g���ɍ��킹�ĕ��ݒ肷��
		if( outOvalRect.right - outOvalRect.left > textwidth + iconsWidth + kWidth_OvalWidth )
		{
			//ovalrect�ݒ�
			outOvalRect.right = outOvalRect.left + textwidth + iconsWidth + kWidth_OvalWidth;
		}
	}
}

/**
�}�E�X�ړ�����OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�J�[�\���̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
ABool	AView_Button::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( NVMC_IsControlEnabled() == true )
	{
		//#634 �{�^���L���͈�
		if( mButtonValidXRangeStart != -1 && mButtonValidXRangeEnd != -1 )
		{
			if( inLocalPoint.x < mButtonValidXRangeStart || inLocalPoint.x > mButtonValidXRangeEnd )
			{
				return false;
			}
		}
		
		mMouseHover = true;
		NVI_Refresh();
	}
	return true;
}

/**
�}�E�X��View�O�ֈړ���������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�J�[�\���̏ꏊ�i�R���g���[�����[�J�����W�j
*/
void	AView_Button::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	mMouseHover = false;
	NVI_Refresh();
}

/**
�E�N���b�N��������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
@param inClickCout �N���b�N��
*/
ABool	AView_Button::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( mContextMenuID == kIndex_Invalid )   return false;
	mMouseHover = false;
	NVI_Refresh();
	//�{�^����ControlID�ۑ��iWindow��EVTDO_DoMenuItemSelected()�łǂ̃{�^���������ꂽ���𔻒肷�邽�߁j
	NVM_GetWindow().NVI_SetLastClickedButtonControlID(NVI_GetControlID());
	//ContextMenu Enable/Disable�ݒ�
	for( AIndex i = 0; i < mContextMenuEnableArray.GetItemCount(); i++ )
	{
		AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuEnableMenuItem(mContextMenuID,i,mContextMenuEnableArray.Get(i));
	}
	//ContextMenu�\��
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	//#724
	mDisplayingContextMenu = true;
	NVI_Refresh();
	//#688 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(mContextMenuID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
	NVMC_ShowContextMenu(mContextMenuID,globalPoint);//#688
	//#724
	mDisplayingContextMenu = false;
	//�R���e�L�X�g���j���[�̎��s�̌��ʁA�R���g���[�����폜����Ă���\��������̂ŁA�`�F�b�N����B
	if( NVM_GetWindow().NVI_ExistControl(NVI_GetControlID()) == true )
	{
		NVI_Refresh();
	}
	//
	return true;
}

//#379
/**
�w���v�^�O
*/
ABool	AView_Button::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	//Oval�{�^���A�t���[�������{�^���̂Ƃ��̓w���v�^�O�͕\�����Ȃ�
	switch(mButtonViewType)
	{
	  case kButtonViewType_TextWithOvalHover:
	  case kButtonViewType_NoFrame:
	  case kButtonViewType_NoFrameWithTextHover:
		{
			return false;
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//#661 SPI_SetHelpTag()�Őݒ肳�ꂽ�e�L�X�g������Ƃ��́A������D��
	if( mHelpTagText1.GetItemCount() > 0 )
	{
		outText1.SetText(mHelpTagText1);
		outText2.SetText(mHelpTagText2);
		outTagSide = mHelpTagSide;
	}
	else
	{
		outText1.SetText(mText);
		outText2.SetText(mText);
		outTagSide = kHelpTagSide_Default;//#643
	}
	NVM_GetLocalViewRect(outRect);
	return true;
}

//#634
/**
�}�E�X�J�[�\���^�C�v
*/
ACursorType	AView_Button::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�{�^���L���͈�
	if( mButtonValidXRangeStart != -1 && mButtonValidXRangeEnd != -1 )
	{
		if( inLocalPoint.x < mButtonValidXRangeStart || inLocalPoint.x > mButtonValidXRangeEnd )
		{
			return kCursorType_Arrow;
		}
	}
	
	//
	ACursorType	cursorType = kCursorType_Arrow;
	switch(mButtonBehaviorType)
	{
	  case kButtonBehaviorType_VSeparator:
		{
			cursorType = kCursorType_ResizeLeftRight;
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return cursorType;
}

/**
*/
void	AView_Button::NVIDO_SetShow( const ABool inShow )
{
	//mMouseHover = false;
}

//win
/**
*/
void	AView_Button::NVIDO_SetBool( const ABool inBool )
{
	if( mToggleMode == true )
	{
		mToggleOn = inBool;
		NVI_Refresh();
	}
}

//win
/**
*/
ABool	AView_Button::NVIDO_GetBool() const
{
	return mToggleOn;
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

/**
�A�C�R���ݒ�
*/
void	AView_Button::SPI_SetIcon( const AImageID inIconImageID, 
								  const ANumber inLeftOffset, const ANumber inTopOffset,
								  const ANumber inWidth, const ANumber inHeight, const ABool inRefresh, 
								  const AImageID inHoverIconImageID,//#530
								  const AImageID inToggleOnImageID )
{
	mIconImageID = inIconImageID;
	mHoverIconImageID = inHoverIconImageID;
	mToggleOnIconImageID = inToggleOnImageID;
	mIconLeftOffset = inLeftOffset;
	mIconTopOffset = inTopOffset;
	mIconWidth = inWidth;
	mIconHeight = inHeight;
	if( inRefresh == true )//#530
	{
		NVI_Refresh();
	}
}

/**
�A�C�R��Image�ݒ�
*/
void	AView_Button::SPI_SetIconImageID( AImageID inIconImageID, 
										 const AImageID inHoverIconImageID,
										 const AImageID inToggleOnImageID )
{
	mIconImageID = inIconImageID;
	mHoverIconImageID = inHoverIconImageID;
	mToggleOnIconImageID = inToggleOnImageID;
	mIconLeftOffset = 0;
	mIconTopOffset = 0;
	mIconWidth = NVMC_GetImageWidth(mIconImageID);
	mIconHeight = NVMC_GetImageHeight(mIconImageID);
}

/**
�e�L�X�g�����ݒ�
*/
void	AView_Button::SPI_SetTextProperty( const AText& inFontName, const AFloatNumber inFontSize, const AJustification inJustification, 
		const ATextStyle inTextStyle,//#724
		const AColor inColor, const AColor inColorDeactive, const ABool inRefresh )//#530
{
	mFontName.SetText(inFontName);
	mFontSize = inFontSize;
#if IMPLEMENTATION_FOR_WINDOWS
	mFontSize *= 0.8;
	if( mFontSize < 7.8 )   mFontSize = 7.8;
#endif
	mTextStyle = inTextStyle;//#724
	mColor = inColor;
	mColorDeactive = inColorDeactive;
	NVMC_SetDefaultTextProperty(inFontName,mFontSize,inColor,mTextStyle,true);
	NVMC_GetMetricsForDefaultTextProperty(mFontHeight,mFontAscent);
	mTextJustification = inJustification;
	//#724
	if( mButtonViewType == kButtonViewType_TextWithOvalHover )
	{
		SPI_SetWidthToFitTextWidth();
	}
	//
	if( inRefresh == true )//#530
	{
		NVI_Refresh();
	}
}

//#724
/**
�e�L�X�g���ɍ��킹�ă{�^�����ݒ�
*/
void	AView_Button::SPI_SetWidthToFitTextWidth( const ANumber inMaxWidth )
{
	ANumber	width = NVMC_GetDrawTextWidth(mText)+kLeftPadding_TextWithOvalHover+kRightPadding_TextWithOvalHover;
	if( mMenuID != kIndex_Invalid || mMenuTextArray.GetItemCount() > 0 )
	{
		width += kWidth_MenuTriangle;
	}
	if( mIconImageID != kImageID_Invalid )
	{
		//�A�C�R���\����
		width += NVMC_GetImageWidth(mIconImageID) + kSpaceBetweenIconAndText;
	}
	if( width > inMaxWidth )
	{
		width = inMaxWidth;
	}
	NVI_SetSize(width,NVI_GetHeight());
}

/**
�e�L�X�g�ݒ�
*/
void	AView_Button::NVIDO_SetText( const AText& inText )
{
	if( mText.Compare(inText) == false )
	{
		mText.SetText(inText);
		
		NVI_Refresh();
	}
}

//#379
/**
�e�L�X�g�擾
*/
void	AView_Button::NVIDO_GetText( AText& outText ) const
{
	outText.SetText(mText);
}

/**
�e�L�X�g�ݒ�
*/
void	AView_Button::SPI_SetTextFromMenuIndex( const AIndex inIndex )
{
	if( mMenuID == kIndex_Invalid )   {_ACError("",this);return;}
	AText	text;
	AApplication::GetApplication().NVI_GetMenuManager().GetContextMenuItemText(mMenuID,inIndex,text);
	NVI_SetText(text);
}

/**
���j���[�ݒ�iContextMenu��{�N���X�O�Őݒ肷��ꍇ�p�j

ContextMenu��TextArrayMenu��ݒ肵�Ă���ꍇ�ȂǂɎg��
*/
void	AView_Button::SPI_SetMenu( const AContextMenuID inContextMenuID, const AMenuItemID inMenuItemID )
{
	mMenuID = inContextMenuID;
	mMenuItemID = inMenuItemID;
}

/**
���j���[�ݒ�iTextArray��ݒ肷��ꍇ�p�j

�Œ��TextArray
*/
void	AView_Button::SPI_SetMenuTextArray( const ATextArray& inTextArray, const ATextArray& inActionTextArray, const AMenuItemID inMenuItemID )
{
	mMenuTextArray.SetFromTextArray(inTextArray);
	mActionTextArray.SetFromTextArray(inActionTextArray);
	mMenuItemID = inMenuItemID;
}

/**
�{�^�����g�O�����[�h�ɂ���
*/
void	AView_Button::SPI_SetToggleMode( const ABool inToggleMode )
{
	mToggleMode = inToggleMode;
	mToggleOn = false;
	NVI_Refresh();
}

/**
�{�^���̌��݂̃g�O����Ԃ��擾����
*/
void	AView_Button::SPI_SetToogleOn( const ABool inToggleOn )
{
	if( mToggleOn == inToggleOn )   return;//#695
	mToggleOn = inToggleOn;
	NVI_Refresh();
}

/**
�{�^�����g�O��On�̏�Ԃɐݒ肷��
*/
ABool	AView_Button::SPI_GetToggleOn() const
{
	return mToggleOn;
}

/**
�E�N���b�N���j���[�ݒ�
*/
void	AView_Button::SPI_SetContextMenu( const AContextMenuID inContextMenuID )
{
	mContextMenuID = inContextMenuID;
	mContextMenuEnableArray.DeleteAll();
	for( AIndex i = 0; i < AApplication::GetApplication().NVI_GetMenuManager().GetContextMenuItemCount(inContextMenuID); i++ )
	{
		mContextMenuEnableArray.Add(true);
	}
}

/**
�R���e�L�X�g���j���[��Enable/Disable��ݒ肷��
*/
void	AView_Button::SPI_SetContextMenuEnableItem( const AIndex inIndex, const ABool inEnable )
{
	mContextMenuEnableArray.Set(inIndex,inEnable);
}

//#634
/**
�{�^������^�C�v�ݒ�
*/
void	AView_Button::SPI_SetButtonBehaviorType( const AButtonBehaviorType inButtonType )
{
	mButtonBehaviorType = inButtonType;
}

//#634
/**
�{�^���L���͈͐ݒ�
*/
void	AView_Button::SPI_SetButtonValidXRange( const ANumber inStartX, const ANumber inEndX )
{
	mButtonValidXRangeStart = inStartX;
	mButtonValidXRangeEnd = inEndX;
}

//#661
/**
�w���v�^�O�ݒ�
*/
void	AView_Button::SPI_SetHelpTag( const AText& inText1, const AText& inText2, const AHelpTagSide inTagSide )
{
	switch(mButtonViewType)
	{
	  case kButtonViewType_TextWithOvalHover:
	  case kButtonViewType_NoFrame:
	  case kButtonViewType_NoFrameWithTextHover:
		{
			return;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	mHelpTagText1.SetText(inText1);
	mHelpTagText2.SetText(inText2);
	mHelpTagSide = inTagSide;
	NVM_GetWindow().NVI_EnableHelpTagCallback(NVI_GetControlID(),false);
}

//#724
/**
�{�^��view�^�C�v�ݒ�
*/
void	AView_Button::SPI_SetButtonViewType( const AButtonViewType inType )
{
	mButtonViewType = inType;
	switch(mButtonViewType)
	{
	  case kButtonViewType_TextWithOvalHover:
	  case kButtonViewType_NoFrame:
	  case kButtonViewType_NoFrameWithTextHover:
		{
			mHelpTagText1.DeleteAll();
			mHelpTagText2.DeleteAll();
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//
	NVI_Refresh();
}

#pragma mark ===========================

#pragma mark --- VSeprator type
//#634

/**
VSeparator�^�C�v�@�}�E�X�_�E��������
*/
ABool	AView_Button::DoMouseDown_VSeparator( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//�O��}�E�X�ʒu�̕ۑ�
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,mPreviousMouseGlobalPoint);
	
	//Windows�̏ꍇ�́A�����ň�U�C�x���g�R�[���o�b�N�𔲂���BMouseTrack�͕ʂ̃C�x���g�ōs���B
	//#688 if( mMouseTrackByLoop == false )
	{
		//TrackingMode�ݒ�
		NVM_SetMouseTrackingMode(true);
		return false;
	}
	/*#688
	//�}�E�X�{�^�����������܂Ń��[�v
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//���[�J�����W�ɕϊ�
		ALocalPoint	mouseLocalPoint;
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		//
		DoMouseTrack_VSeparator(mouseLocalPoint);
	}
	//�����ʒm #409
	NVM_GetWindow().NVI_SeparatorMoved(NVM_GetWindowID(),NVI_GetControlID(),0,true);
	return false;
	*/
}

/**
VSeparator�^�C�v�@�}�E�X�g���b�N������
*/
void	AView_Button::DoMouseTrack_VSeparator( const ALocalPoint& inLocalPoint )
{
	//�O���[�o�����W�ɕϊ�
	AGlobalPoint	mouseGlobalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,mouseGlobalPoint);
	//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
	if( mouseGlobalPoint.x == mPreviousMouseGlobalPoint.x && mouseGlobalPoint.y == mPreviousMouseGlobalPoint.y )
	{
		return;
	}
	//TargetWindow��Separator�ړ���ʒm
	ANumber	widthDelta = mouseGlobalPoint.x-mPreviousMouseGlobalPoint.x;
	ANumber	resultDelta = NVM_GetWindow().NVI_SeparatorMoved(NVM_GetWindowID(),NVI_GetControlID(),widthDelta,false);//#409
	mouseGlobalPoint.x = mPreviousMouseGlobalPoint.x + resultDelta;
	//�O��}�E�X�ʒu�̕ۑ�
	mPreviousMouseGlobalPoint = mouseGlobalPoint;
}

/**
VSeparator�^�C�v�@�}�E�X�g���b�N�I��������
*/
void	AView_Button::DoMouseTrackEnd_VSeparator( const ALocalPoint& inLocalPoint )
{
	//�����ʒm #409
	NVM_GetWindow().NVI_SeparatorMoved(NVM_GetWindowID(),NVI_GetControlID(),0,true);
	//TrackingMode�����iDoMouseTrackEnd()�̑O�ɂ��Ȃ��ƁADoMouseTrackEnd()��Separator�R���g���[�����폜����Ă��܂��̂Łj
	NVM_SetMouseTrackingMode(false);
}


