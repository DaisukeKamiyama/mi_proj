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

AView_KeyToolList
R0073
*/

#include "stdafx.h"

#include "AView_KeyToolList.h"
#include "AApp.h"

const ANumber	kLeftMargin = 8;
const ANumber	kRightMargin = 8;
const ANumber	kFileColumnWidth = 120;

const AFloatNumber	kFontSize = 9.0;

#pragma mark ===========================
#pragma mark [�N���X]AView_KeyToolList
#pragma mark ===========================
//���E�C���h�E�̃��C��View

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_KeyToolList::AView_KeyToolList( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID ) 
	: AView(/*#199 inParent,inWindow*/inWindowID,inID), mLineHeight(12), mCurrentHoverCursorIndex(kIndex_Invalid)
{
	NVMC_SetOffscreenMode(true);//win
	//#92 ��������NVIDO_Init()�ֈړ�
}

//�f�X�g���N�^
AView_KeyToolList::~AView_KeyToolList()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_KeyToolList::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_KeyToolList::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>


#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_KeyToolList::EVTDO_DoDraw()
{
	ALocalRect	frameRect;
	NVM_GetLocalViewRect(frameRect);
	
	//=========================�w�i���擾�i����view�ł͔w�i�͕`�悵�Ȃ��j=========================
	//�w�i���擾
	AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
	AFloatNumber	backgroundAlpha = 1.0;
	switch(GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()))
	{
	  case kSubWindowLocationType_Popup:
		{
			backgroundAlpha = GetApp().SPI_GetPopupWindowAlpha();
			//NVMC_EraseRect(frameRect);
			break;
		}
	  case kSubWindowLocationType_Floating:
		{
			//�t���[�e�B���O�̎w�蓧�ߗ��Ŕw�i����
			backgroundAlpha = GetApp().SPI_GetFloatingWindowAlpha();
			//NVMC_PaintRect(frameRect,backgroundColor,backgroundAlpha);
			break;
		}
	  default:
		{
			//NVMC_PaintRect(frameRect,backgroundColor);
			break;
		}
	}
	
	//=========================�`��f�[�^�擾=========================
	//�t�H���g�擾
	//���e�����t�H���g
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//#1316
	AColor	letterColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
	if( AApplication::GetApplication().NVI_IsDarkMode() == true )
	{
		letterColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
	}
	//
	ANumber	viewwidth = NVI_GetViewWidth();
	NVMC_SetDefaultTextProperty(fontname,fontsize,letterColor,kTextStyle_Normal,true);//#1316
	
	//=========================�e���ږ����[�v=========================
	//
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		//�srect�擾�Aupdate�͈͂��ǂ����𔻒�
		AImageRect	imagerect;
		imagerect.left		= 0;
		imagerect.top		= i*mLineHeight;
		imagerect.right		= viewwidth;
		imagerect.bottom	= (i+1)*mLineHeight;
		ALocalRect	localrect;
		NVM_GetLocalRectFromImageRect(imagerect,localrect);
		if( NVMC_IsRectInDrawUpdateRegion(localrect) == false )   continue;
		
		//�z�o�[�\��
		if( i == mCurrentHoverCursorIndex )
		{
			AColor	color;
			AColorWrapper::GetHighlightColor(color);
			NVMC_PaintRect(localrect,color,0.4);
		}
		const AText&	infotext = mInfoTextArray.GetTextConst(i);
		ATextStyle	textstyle = kTextStyle_Normal;
		if( mIsUserToolArray.Get(i) == true )
		{
			textstyle = kTextStyle_Bold;
		}
		/*
		//�`��f�[�^�쐬
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(infotext,4,0,true,true,false,0,infotext.GetItemCount());
		*/
		//�e�L�X�g�`��rect����
		ALocalRect	drawRect = localrect;
		drawRect.left	= localrect.left + kLeftMargin;
		drawRect.right	= localrect.right - kRightMargin;;
		ALocalRect	textRect = drawRect;
		//�e�L�X�g�`��
		//NVMC_DrawText(textRect,drawRect,textDrawData);
		NVMC_DrawText(textRect,infotext,letterColor,textstyle);//#1316
	}
}

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_KeyToolList::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	AImagePoint	imagept;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	AIndex	index = imagept.y / mLineHeight;
	if( index >= 0 && index < mInfoTextArray.GetItemCount() )
	{
		//TextWindow��keytool���͂�`����
		GetApp().SPI_GetTextWindowByID(mCurrentTextWindowID).SPI_KeyTool(mCurrentTextViewControlID,mInfoTextArray.GetTextConst(index).Get(0));
	}
	return false;
}

//�}�E�X�z�C�[��
void	AView_KeyToolList::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
										const ALocalPoint inLocalPoint )//win 080706
{
	//�X�N���[������ݒ肩��擾����
	ANumber	scrollPercent;
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true 
				|| AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )//win 080702
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd);
	}
	else
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent);
	}
	//�X�N���[�����s
	NVI_Scroll(0,inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100);
}

ABool	AView_KeyToolList::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	return false;
}

//�J�[�\��
ABool	AView_KeyToolList::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	imagept;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	AIndex	index = imagept.y / mLineHeight;
	if( index >= 0 && index < mInfoTextArray.GetItemCount() )
	{
		mCurrentHoverCursorIndex = index;
		NVI_Refresh();
		return true;
	}
	ClearHoverCursor();
	return true;
}

void	AView_KeyToolList::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearHoverCursor();
}

void	AView_KeyToolList::ClearHoverCursor()
{
	if( mCurrentHoverCursorIndex != kIndex_Invalid )
	{
		mCurrentHoverCursorIndex = kIndex_Invalid;
		NVI_Refresh();
	}
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

//
void	AView_KeyToolList::SPI_SetInfo( const AWindowID inTextWindowID, const AControlID inTextViewControlID,
									   const ATextArray& inInfoTextArray, const ABoolArray& inIsUserToolArray )
{
	mCurrentTextWindowID = inTextWindowID;
	mCurrentTextViewControlID = inTextViewControlID;
	mInfoTextArray.DeleteAll();
	mIsUserToolArray.DeleteAll();
	for( AIndex i = 0; i < inInfoTextArray.GetItemCount(); i++ )
	{
		AText	infoText;
		infoText.SetText(inInfoTextArray.GetTextConst(i));
		infoText.ReplaceChar(kUChar_CR,kUChar_Space);
		mInfoTextArray.Add(infoText);
		mIsUserToolArray.Add(inIsUserToolArray.Get(i));
	}
	CalcDrawData();
	//�|�b�v�A�b�v�\�����́A�E�C���h�E�T�C�Y��view��image size�ɍ��킹�čX�V
	if( GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()) == kSubWindowLocationType_Popup )
	{
		ARect	windowBounds = {0};
		NVM_GetWindow().NVI_GetWindowBounds(windowBounds);
		windowBounds.right = windowBounds.left + NVI_GetImageWidth() + 15;
		windowBounds.bottom = windowBounds.top + NVI_GetImageHeight() + 15;
		NVM_GetWindow().NVI_SetWindowBounds(windowBounds);
	}
	//
	NVM_GetWindow().NVI_UpdateProperty();
	NVI_Refresh();
}

void	AView_KeyToolList::AdjustScroll()
{
	/*ALocalRect	frameRect;
	NVM_GetLocalViewRect(frameRect);
	AImageRect	imagerect;
	NVM_GetImageRectFromLocalRect(frameRect,imagerect);
	AIndex	startIndex = imagerect.top/mLineHeight;
	AIndex	endIndex = imagerect.bottom/mLineHeight;
	if( mIndex <= startIndex || endIndex <= mIndex )
	{
		AImagePoint	pt = {0,0};
		pt.y = mIndex*mLineHeight - (frameRect.bottom - frameRect.top)/2;
		if( pt.y < 0 )   pt.y = 0;
		NVI_ScrollTo(pt);
	}*/
}

//�e��`��f�[�^�iimagesize���j�̐ݒ�
void	AView_KeyToolList::CalcDrawData()
{
	//�t�H���g�Alineheight���̐ݒ�
	//ANumber	viewwidth = NVI_GetViewWidth();
	AText	fixwidthfontname;
	AFontWrapper::GetAppDefaultFontName(fixwidthfontname);//#375
	NVMC_SetDefaultTextProperty(fixwidthfontname,kFontSize,kColor_Black,kTextStyle_Normal,true);
	ANumber	fontheight, fontascent;
	NVMC_GetMetricsForDefaultTextProperty(fontheight,fontascent);
	fontheight += 4;
	mLineHeight = fontheight;
	//�C���[�W�T�C�Y�̕��́A���X�g�̃e�L�X�g�̂����̍ő�̕��ɂ���
	ANumber	maxWidth = 0;
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		AText	infoText;
		ANumber	w = NVMC_GetDrawTextWidth(mInfoTextArray.GetTextConst(i));
		if( w > maxWidth )
		{
			maxWidth = w;
		}
	}
	//ImageSize�ݒ�
	NVM_SetImageSize(maxWidth+25,mInfoTextArray.GetItemCount()*fontheight);
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(0,fontheight,0,rect.bottom-rect.top-fontheight);
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
}

void	AView_KeyToolList::SPI_UpdateDrawProperty()
{
	CalcDrawData();
	NVI_Refresh();
}



