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

AView_Ruler

*/

#include "stdafx.h"

#include "AView_Ruler.h"
#include "AWindow_Text.h"
#include "ADocument_Text.h"
#include "AApp.h"

#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kFontSize = 8.0;
#else
const AFloatNumber	kFontSize = 7.2;
#endif

#pragma mark ===========================
#pragma mark [�N���X]AView_Ruler
#pragma mark ===========================
//InfoHeader

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_Ruler::AView_Ruler( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID )
 : AView(/*#199 inParent,inWindow*/inWindowID,inID), mCaretImageX(0), mSelectImageX(0), mRulerScaleCount(5), mCaretDrawn(false)
,mLetterWidth(10)//#0 ����̏����������EVTDO_DoDraw()��end�����ɑ傫�Ȓl�ɂȂ邱�Ƃ�������
{
	NVMC_SetOffscreenMode(true);//win
	
	//view��s�����ɐݒ� #1090
	NVMC_SetOpaque(true);
}

//�f�X�g���N�^
AView_Ruler::~AView_Ruler()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_Ruler::NVIDO_Init()
{
	//win
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_File);
	NVMC_EnableDrop(scrapTypeArray);
	//ImageSize�ݒ� #1070
	SPI_UpdateImageSize();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_Ruler::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_Ruler::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	return false;
}

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_Ruler::EVTDO_DoDraw()
{
	//NSLog(@"AView_Ruler::EVTDO_DoDraw");
	/* win EVTDO_DrawPreProcess()�ֈړ�
	if( mCaretDrawn == true )
	{
		XorCaret();
	}
	*/
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	
	//�X�V�̈�擾
	ALocalRect	updateRect = NVMC_GetDrawUpdateRect();
	
	//�^�u�I���r���[�S�̂̕\��
	//NVMC_PaintRect(viewRect,kColor_Gray95Percent,1.0);
	//�F #1316
	AColor	backgroundStartColor = AColorWrapper::GetColorByHTMLFormatColor("D9D9D9");
	AColor	backgroundEndColor = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
	if( NVI_GetWindow().NVI_IsWindowActive() == false )
	{
		backgroundStartColor = backgroundEndColor = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
	}
	AColor	borderColor = AColorWrapper::GetColorByHTMLFormatColor("CCCCCC");
	AColor	caretColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
	//�_�[�N���[�h
	if( AApplication::GetApplication().NVI_IsDarkMode() == true )
	{
		backgroundStartColor = AColorWrapper::GetColorByHTMLFormatColor("282828");
		backgroundEndColor = AColorWrapper::GetColorByHTMLFormatColor("181818");
		borderColor = AColorWrapper::GetColorByHTMLFormatColor("4C4C4C");
		caretColor = AColorWrapper::GetColorByHTMLFormatColor("C0C0C0");
	}
	//�w�i�`��
	NVMC_PaintRectWithVerticalGradient(viewRect,backgroundStartColor,backgroundEndColor,1.0,1.0);
	//���E��
	ALocalPoint	spt,ept;
	spt.x = viewRect.left;
	spt.y = viewRect.bottom-1;//win 080618
	ept.x = viewRect.right;
	ept.y = viewRect.bottom-1;//win 080618
	NVMC_DrawLine(spt,ept,borderColor,1.0);
	
	//
	AText	fontname;//win
	AFontWrapper::GetDialogDefaultFontName(fontname);//win #375
	NVMC_SetDefaultTextProperty(fontname,kFontSize,kColor_Gray50Percent,kTextStyle_Normal,true);
	
	//
	/*
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	ANumber	start = static_cast<ANumber>((imageFrameRect.left-mLeftMargin)/mLetterWidth);//B0000
	*/
	AImageRect	imageUpdateRect = {0};
	NVM_GetImageRectFromLocalRect(updateRect,imageUpdateRect);
	ANumber	start = static_cast<ANumber>((imageUpdateRect.left-mLeftMargin)/mLetterWidth);//B0000
	if( start < 0 )   start = 0;//B0000
	ANumber	end = static_cast<ANumber>((imageUpdateRect.right-mLeftMargin)/mLetterWidth);
	ANumber	lastTextX = -1000;
	for( AIndex i = start; i <= end; i++ )
	{
		AImagePoint	imagespt, imageept;
		imagespt.x = static_cast<ANumber>(viewRect.left + mLeftMargin + i*mLetterWidth);
		imagespt.y = viewRect.bottom - 3;
		if( i%mRulerScaleCount == 0 )
		{
			imagespt.y -= 2;
		}
		imageept.x = imagespt.x;
		imageept.y = viewRect.bottom -1;
		NVM_GetLocalPointFromImagePoint(imagespt,spt);
		NVM_GetLocalPointFromImagePoint(imageept,ept);
		NVMC_DrawLine(spt,ept,kColor_Gray50Percent,1.0);
		if( i%mRulerScaleCount == 0 )
		{
			AText	text;
			text.SetFormattedCstring("%d",i);
			ANumber	w = static_cast<ANumber>(NVMC_GetDrawTextWidth(text));
			ALocalRect	localrect;
			localrect.left		= spt.x - w/2;
			localrect.top		= viewRect.top + 5;
			localrect.right		= localrect.left + w+1;
			localrect.bottom	= viewRect.bottom;
			if( lastTextX+3 < localrect.left )
			{
				NVMC_DrawText(localrect,text);
				lastTextX = localrect.right;
			}
		}
	}
	//�L�����b�g�ʒu�`��
	{
		AImagePoint	imagept = {mCaretImageX+mLeftMargin,0};
		ALocalPoint	pt = {0};
		NVM_GetLocalPointFromImagePoint(imagept,pt);
		ALocalPoint	spt = pt, ept = pt;
		spt.y = viewRect.top;
		ept.y = viewRect.bottom;
		NVMC_DrawLine(spt,ept,caretColor,1.0,0,2.0);
	}
	if( mCaretImageX != mSelectImageX )
	{
		AImagePoint	imagept = {mSelectImageX+mLeftMargin,0};
		ALocalPoint	pt = {0};
		NVM_GetLocalPointFromImagePoint(imagept,pt);
		ALocalPoint	spt = pt, ept = pt;
		spt.y = viewRect.top;
		ept.y = viewRect.bottom;
		NVMC_DrawLine(spt,ept,caretColor,1.0,0,2.0);
	}
	
	/*
	//#725
	//ALocalPoint	spt, ept;
	AImagePoint	imagespt, imageept;
	imagespt.x = viewRect.left + mLeftMargin + mCaretImageX;
	imagespt.y = viewRect.top;
	imageept.x = imagespt.x;
	imageept.y = viewRect.bottom;
	NVM_GetLocalPointFromImagePoint(imagespt,spt);
	NVM_GetLocalPointFromImagePoint(imageept,ept);
	NVMC_DrawLine(spt,ept,kColor_Black,1.0,0.0,2.0);
	*/
	//
	/* AImagePoint	imagespt, imageept;
	imagespt.x = viewRect.left + mLeftMargin + mCaretImageX;
	imagespt.y = viewRect.top;
	imageept.x = imagespt.x;
	imageept.y = viewRect.bottom;
	NVM_GetLocalPointFromImagePoint(imagespt,spt);
	NVM_GetLocalPointFromImagePoint(imageept,ept);
	NVMC_DrawLine(spt,ept,kColor_Black,3.0);
	//
	imagespt.x = viewRect.left + mLeftMargin + mSelectImageX;
	imagespt.y = viewRect.top;
	imageept.x = imagespt.x;
	imageept.y = viewRect.bottom;
	NVM_GetLocalPointFromImagePoint(imagespt,spt);
	NVM_GetLocalPointFromImagePoint(imageept,ept);
	NVMC_DrawLine(spt,ept,kColor_Black,1.0,0.0,2.0);*/
	/* win EVTDO_DrawPostProcess()�ֈړ�
	if( mCaretDrawn == true )
	{
		XorCaret();
	}
	*/
}

//#575
/**
Drag&Drop�����iDrag���j
*/
void	AView_Ruler::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, ABool& outIsCopyOperation )
{
	ACursorWrapper::SetCursor(kCursorType_ArrowCopy);
}

//Drag&Drop�����iDrop�j win
void	AView_Ruler::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AObjectArray<AFileAcc>	fileArray;
	AScrapWrapper::GetFileDragData(inDragRef,fileArray);
	for( AIndex i = 0; i < fileArray.GetItemCount(); i++ )
	{
		GetApp().SPNVI_CreateDocumentFromLocalFile(fileArray.GetObjectConst(i));
	}
}

//win
/**
�`��O����
*/
void	AView_Ruler::EVTDO_DrawPreProcess()
{
	/*
#if !USE_OVERLAYCARET
	if( mCaretDrawn == true )
	{
		XorCaret();
	}
#endif
	*/
}

//win
/**
�`��O����
*/
void	AView_Ruler::EVTDO_DrawPostProcess()
{
	/*
#if !USE_OVERLAYCARET
	if( mCaretDrawn == true )
	{
		XorCaret();
	}
#endif
	*/
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---Update

//�t�H���g����Text�`��v���p�e�B���ύX���ꂽ�ꍇ�̏���
void	AView_Ruler::SPI_UpdateProperty( const ADocumentID inTextDocumentID, const ANumber inLeftMargin, const AItemCount inRulerScaleCount )
{
	//win
	AText	fontname;
	GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetFontName(fontname);
	
	NVMC_SetDefaultTextProperty(
			fontname,//win
			GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetFontSize(),
			kColor_Black,
			kTextStyle_Normal,
			GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_IsAntiAlias());
	AText	rulerBaseText;
	GetApp().SPI_GetModePrefDB(GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetModeIndex()).
			GetModeData_Text(AModePrefDB::kRulerBaseLetter,rulerBaseText);
	mLetterWidth = NVMC_GetDrawTextWidth(rulerBaseText);
	mLeftMargin = inLeftMargin;
	mRulerScaleCount = inRulerScaleCount;
	NVI_Refresh();
}

//
void	AView_Ruler::SPI_UpdateCaretImageX( const ANumber inCaretImageX, const ANumber inSelectImageX )
{
	/*
#if !USE_OVERLAYCARET
	if( mCaretDrawn == true )
	{
		XorCaret();
	}
#endif
	*/
	
	//���݂̃L�����b�g�`��rect�擾
	ALocalRect	oldUpdateRect1 = GetCaretUpdateRect(mCaretImageX);
	ALocalRect	oldUpdateRect2 = GetCaretUpdateRect(mSelectImageX);
	ABool	oldCaretMode = (mCaretImageX == mSelectImageX);
	
	//�L�����b�g�`��ʒu�X�V
	mCaretImageX = inCaretImageX;
	mSelectImageX = inSelectImageX;
	
	//NVI_Refresh();
	//XorCaret();
	mCaretDrawn = true;
	
	//���L�����b�g�ʒu�`��폜
	NVMC_RefreshRect(oldUpdateRect1);
	if( oldCaretMode == false )
	{
		NVMC_RefreshRect(oldUpdateRect2);
	}
	
	//�`��X�V
	NVMC_RefreshRect(GetCaretUpdateRect(mCaretImageX));
	if( mCaretImageX != mSelectImageX )
	{
		NVMC_RefreshRect(GetCaretUpdateRect(mSelectImageX));
	}
}

/**
�L�����b�g�`��͈͎擾
*/
ALocalRect	AView_Ruler::GetCaretUpdateRect( const ANumber inCaretImageX ) const
{
	ALocalRect	viewRect = {0};
	NVM_GetLocalViewRect(viewRect);
	AImagePoint	imagept = {inCaretImageX+mLeftMargin,0};
	ALocalPoint	pt = {0};
	NVM_GetLocalPointFromImagePoint(imagept,pt);
	ALocalRect	updateRect = {0};
	updateRect.left		= pt.x-2;
	updateRect.top		= viewRect.top;
	updateRect.right	= pt.x+2;
	updateRect.bottom	= viewRect.bottom;
	return updateRect;
}

void	AView_Ruler::XorCaret()
{
	if( NVI_IsVisible() == false )   return;
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	ALocalPoint	spt, ept;
	AImagePoint	imagespt, imageept;
	imagespt.x = viewRect.left + mLeftMargin + mCaretImageX;
	imagespt.y = viewRect.top;
	imageept.x = imagespt.x;
	imageept.y = viewRect.bottom;
	NVM_GetLocalPointFromImagePoint(imagespt,spt);
	NVM_GetLocalPointFromImagePoint(imageept,ept);
	NVMC_DrawXorCaretLine(spt,ept,true,false,2);
}

//
void	AView_Ruler::SPI_UpdateImageSize()
{
	ANumber	width = 100000;//#1069
	ANumber	height = NVI_GetViewHeight();
	NVM_SetImageSize(width,height);
}


