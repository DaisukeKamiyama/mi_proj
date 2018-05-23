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

AView_Index

*/

#include "stdafx.h"

#include "AView_Index.h"
#include "ADocument_IndexWindow.h"
#include "AApp.h"
#include "AWindow_FindResult.h"

//��ŏ���
const ANumber	kColumnMinWidth = 16;

#pragma mark ===========================
#pragma mark [�N���X]AView_Index
#pragma mark ===========================
//�C���f�b�N�X�E�C���h�E�̃��C��View

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_Index::AView_Index( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID, const AObjectID inIndexDocumentID ) 
	: AView(/*#199 inParent,inWindow*/inWindowID,inID), mIndexDocumentID(inIndexDocumentID), mSelectedRowIndex(kIndex_Invalid), /*#199mWindow(inWindow),*/ mFileColumnWidth(50)
	, mCursorRowDisplayIndex(kIndex_Invalid)//#92
	,mMouseTrackingMode(kMouseTrackingMode_None)//#688 , mMouseTrackByLoop(true)//win
	,mContextMenuSelectedRowIndex(kIndex_Invalid)//#465
,mOneColumnMode(false), mLineHeight(9)//#725
{
	NVMC_SetOffscreenMode(true);//win
	//#92 ��������NVIDO_Init()�ֈړ�
}

//�f�X�g���N�^
AView_Index::~AView_Index()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_Index::NVIDO_Init()
{
	/*#688
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
	SPI_UpdateTextDrawProperty();
	SPI_UpdateImageSize();
	//#92
	GetIndexDocument().NVI_RegisterView(GetObjectID());
	//#92
	NVMC_EnableMouseLeaveEvent();
}

/**
�폜���ɕK���R�[�������i�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s�����Ɓj
*/
void	AView_Index::NVIDO_DoDeleted()
{
	GetIndexDocument().NVI_UnregisterView(GetObjectID());
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

//IndexDocument�擾
ADocument_IndexWindow&	AView_Index::GetIndexDocument()
{
	return GetApp().SPI_GetIndexWindowDocumentByID(mIndexDocumentID);
}
const ADocument_IndexWindow&	AView_Index::GetIndexDocumentConst() const
{
	return GetApp().SPI_GetIndexWindowDocumentConstByID(mIndexDocumentID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

const ANumber	kCellLeftRightPadding = 2;
const ANumber	kFoldingIconWidth = 16;

//���\�����[�h���]��
const ANumber	kOneColumnMode_BoxLeftMargin = 3;
const ANumber	kOneColumnMode_BoxRightMargin = 3;
const ANumber	kOneColumnMode_BoxTopMargin = 2;
const ANumber	kOneColumnMode_BoxBottomMargin = 1;

//�s��
const ANumber	kLineMargin = 1;
const ANumber	kOneColumnMode_LineMargin = 2;

//���\�����[�h���p�f�B���O
const ANumber	kOneColumnMode_LeftPadding1stLine = 5;
const ANumber	kOneColumnMode_LeftPadding2ndLine = 8;

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_Index::EVTDO_DoDraw()
{
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	
	//=========================�`��f�[�^�擾=========================
	//�t�H���g�擾
	//���e�����t�H���g
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//�w�b�_�����t�H���g
	AText	labelfontname;
	AFontWrapper::GetDialogDefaultFontName(labelfontname);
	
	//=========================�w�i���擾�i����view�ł͔w�i�͕`�悵�Ȃ��j=========================
	//�w�i���擾
	AColor	backgroundColor = kColor_White;
	AFloatNumber	backgroundAlpha = 1.0;
	//�T�u�E�C���h�E�̏ꍇ�̔w�i�F�E���擾
	if( GetApp().SPI_IsSubWindow(NVM_GetWindow().GetObjectID()) == true )
	{
		backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
		if( NVM_GetWindow().NVI_GetOverlayMode() == false )
		{
			//�t���[�e�B���O�̎w�蓧�ߗ��Ŕw�i����
			backgroundAlpha = GetApp().SPI_GetFloatingWindowAlpha();
			//NVMC_PaintRect(localFrameRect,backgroundColor,backgroundAlpha);
		}
		else
		{
			//NVMC_PaintRect(localFrameRect,backgroundColor);
		}
	}
	
	//=========================�e�s�`��=========================
	AColor	normalcolor = GetApp().SPI_GetSubWindowLetterColor();
	//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kIndexWinNormalColor,normalcolor);
	AColor	groupcolor = kColor_Red;
	//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kIndexWinGroupColor,groupcolor);
	//
	NVMC_SetDefaultTextProperty(fontname,fontsize,normalcolor,kTextStyle_Normal,true);
	
	AItemCount	rowCount = GetIndexDocument().SPI_GetDisplayRowCount();
	AIndex	startRow = imageFrameRect.top / GetRowHeight();
	AIndex	endRow = imageFrameRect.bottom / GetRowHeight();
	for( AIndex rowIndex = startRow; rowIndex <= endRow; rowIndex++ )
	{
		if( rowIndex >= rowCount )   break;
		AImageRect	rowImageRect;
		GetRowImageRect(rowIndex,rowIndex+1,rowImageRect);
		ALocalRect	rowLocalRect;
		NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
		if( NVMC_IsRectInDrawUpdateRegion(rowLocalRect) == false )   continue;
		
		//�s�f�[�^�̎擾
		AText	path, prehit, hit, posthit, position;
		ANumber	start, length, paragraph;
		ABool	isGroup;
		AIndex	groupIndex;
		AText	comment;
		GetIndexDocument().SPI_GetDisplayRowData(rowIndex,path,hit,prehit,posthit,position,start,length,paragraph,groupIndex,isGroup,comment);
		
		//�F�̐ݒ�
		AColor	color;
		if( isGroup == true )
		{
			color = groupcolor;
		}
		else
		{
			color = normalcolor;
		}
		
		if( mOneColumnMode == true )
		{
			//==================���\�����[�h==================
			rowLocalRect.left 		+= kOneColumnMode_BoxLeftMargin;
			rowLocalRect.right		-= kOneColumnMode_BoxRightMargin;
			rowLocalRect.top		+= kOneColumnMode_BoxTopMargin;
			rowLocalRect.bottom		-= kOneColumnMode_BoxBottomMargin;
			
			//�{�b�N�X�`��#643
			if( rowIndex == mSelectedRowIndex )
			{
				AColor	selectionColor = kColor_Blue;
				AColorWrapper::GetHighlightColor(selectionColor);//#1034 GetListHighlightColor()��GetHighlightColor()
				AColor	color2 = AColorWrapper::ChangeHSV(selectionColor,0,0.6,1.0);
				NVMC_PaintRoundedRect(rowLocalRect,color2,selectionColor,kGradientType_Vertical,backgroundAlpha,backgroundAlpha,
									  5,true,true,true,true);
				NVMC_FrameRoundedRect(rowLocalRect,kColor_Gray20Percent,1.0,5,true,true,true,true);
				
				//
				color = kColor_White;
			}
			else
			{
				NVMC_PaintRoundedRect(rowLocalRect,
									  kColor_White,kColor_Gray95Percent,kGradientType_Vertical,backgroundAlpha,backgroundAlpha,
									  5,true,true,true,true);
				NVMC_FrameRoundedRect(rowLocalRect,kColor_Gray20Percent,1.0,5,true,true,true,true);
			}
		}
		else
		{
			//==================���\�����[�h==================
			//Selection�`��
			if( rowIndex == mSelectedRowIndex )
			{
				/*
				AColor	selectionColor;
				if( NVM_GetWindow().GetObjectID() == GetApp().SPI_GetActiveSubWindowForItemSelector() )
				{
					AColorWrapper::GetHighlightColor(selectionColor);
				}
				else
				{
					selectionColor = kColor_Gray80Percent;
				}
				NVMC_PaintRect(rowLocalRect,selectionColor,0.8);
				*/
				
				//------------------�I���s------------------
				
				//�I��F�擾
				AColor	selColor1 = kColor_Blue, selColor2 = kColor_Blue, selColor3 = kColor_Blue;
				if( NVM_GetWindow().NVI_IsWindowActive() == true ) 
				{
					//activated��
					selColor1 = AColorWrapper::GetColorByHTMLFormatColor("5c91d2");
					selColor2 = AColorWrapper::GetColorByHTMLFormatColor("71a2dc");
					selColor3 = AColorWrapper::GetColorByHTMLFormatColor("2966b9");
				}
				else
				{
					//deactivated��
					selColor1 = AColorWrapper::GetColorByHTMLFormatColor("a8a8a8");
					selColor2 = AColorWrapper::GetColorByHTMLFormatColor("c1c1c1");
					selColor3 = AColorWrapper::GetColorByHTMLFormatColor("9c9c9c");
				}
				NVMC_PaintRectWithVerticalGradient(rowLocalRect,selColor2,selColor3,1.0,1.0);
				//�I�𕔕��̍ŏ㕔1pixel��`��
				ALocalRect	rect = rowLocalRect;
				rect.bottom = rect.top+1;
				NVMC_PaintRect(rect,selColor1,1.0);
				
				//�����F�͔��F�ɂ���
				color = kColor_White;
			}
			//#92
			else if( rowIndex == mCursorRowDisplayIndex )
			{
				/*
				AColor	selectionColor;
				AColorWrapper::GetListHighlightColor(selectionColor);
				NVMC_PaintRect(rowLocalRect,selectionColor,0.2);
				NVMC_FrameRect(rowLocalRect,selectionColor,1.0);
				*/
				//------------------�z�o�[�s------------------
				ALocalRect	rect = rowLocalRect;
				rect.left		+= 2;
				rect.top		+= 2;
				rect.right		-= 2;
				rect.bottom		-= 2;
				NVMC_FrameRoundedRect(rect,kColor_Blue,0.3,3,true,true,true,true,true,true,true,true,1.0);
			}
		}
		
		//==================�t�@�C�����i�w�b�_�j�\��==================
		//
		if( mOneColumnMode == true )
		{
			ALocalRect	headerLocalRect = rowLocalRect;
			headerLocalRect.bottom = headerLocalRect.top + mLineHeight;
			//�w�b�_�F�擾
			AColor	headercolor = kColor_Blue;
			/*
			if( mClickedArray.Get(i) == true )
			{
			headercolor = kColor_Gray;
			}
			*/
			//�w�b�_rect�`��
			AColor	headercolor_start = AColorWrapper::ChangeHSV(headercolor,0,0.8,1.0);
			AColor	headercolor_end = AColorWrapper::ChangeHSV(headercolor,0,1.2,1.0);
			NVMC_PaintRoundedRect(headerLocalRect,headercolor_start,headercolor_end,kGradientType_Vertical,0.15,0.15,
								  5,true,true,true,true);
		}
		//
		ALocalRect	rect = rowLocalRect;
		if( mOneColumnMode == false )
		{
			rect.left += kCellLeftRightPadding;
			rect.right = mFileColumnWidth - kCellLeftRightPadding;
		}
		else
		{
			rect.left += kOneColumnMode_LeftPadding1stLine;
			rect.right -= kCellLeftRightPadding;
			rect.bottom = rect.top + mLineHeight;
		}
		//
		if( isGroup == true )
		{
			ALocalRect	iconrect;
			iconrect.left		= rect.left;
			iconrect.right		= rect.left + 16;
			iconrect.top		= (rect.top+rect.bottom)/2 -8;
			iconrect.bottom		= (rect.top+rect.bottom)/2 +8;
			//#688 AIconID	iconID = kIconID_Mi_ArrowDown;
			AImageID	iconID = kImageID_barSwCursorDown;
			if( GetIndexDocument().SPI_IsGroupExpanded(groupIndex) == false )
			{
				iconID = kImageID_barSwCursorRight;//#688 kIconID_Mi_ArrowRight;
			}
			//#688 NVMC_DrawIcon(iconrect,iconID,true,true);//win 080722
			ALocalPoint	pt = {0,0};
			pt.x = iconrect.left;
			pt.y = iconrect.top;
			NVMC_DrawImage(iconID,pt);
			//
			rect.left += kFoldingIconWidth;
			AText	text;
			text.SetText(path);
			//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Normal,true);
			NVMC_SetDefaultTextProperty(color,kTextStyle_Normal);//win
			NVMC_DrawText(rect,text);
		}
		else
		{
			AText	text;
			if( mOneColumnMode == false )
			{
				rect.left += kFoldingIconWidth;
			}
			if( path.GetItemCount() > 0 )//#220
			{
				if( path.Get(0) == kUChar_Tab )//#221
				{
					//#221 �V�K�h�L�������g�̏ꍇ�́A�t�@�C���p�X�Ƀ^�u�R�[�h+DocumentID���i�[���邱�Ƃɂ���
					ADocumentID	docID;
					AIndex	pos = 1;
					AUniqID	docUniqID;//#693
					path.ParseIntegerNumber(pos,docUniqID.val,false);
					docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
					if( GetApp().NVI_IsDocumentValid(docID) == true )
					{
						if( GetApp().NVI_GetDocumentTypeByID(docID) == kDocumentType_Text )
						{
							GetApp().NVI_GetDocumentByID(docID).NVI_GetTitle(text);
						}
					}
				}
				else
				{
					path.GetFilename(text);
				}
				//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Bold,true);
				NVMC_SetDefaultTextProperty(color,kTextStyle_Bold);//win
				ANumber	w = static_cast<ANumber>(NVMC_GetDrawTextWidth(text));
				NVMC_DrawText(rect,text);
				rect.left += w;
				
				text.SetCstring("   ");
				text.AddText(position);
				if( paragraph != kIndex_Invalid )
				{
					text.AddCstring("  ");
					AText	paratext;
					paratext.SetLocalizedText("FindResult_Paragraph");
					text.AddText(paratext);
					text.AddFormattedCstring("%d",paragraph+1);
				}
			}
			//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Normal,true);
			NVMC_SetDefaultTextProperty(color,kTextStyle_Normal);//win
			NVMC_DrawText(rect,text);
		}
		
		//==================���e���\��==================
		rect = rowLocalRect;
		if( mOneColumnMode == false )
		{
			//------------------�����J�����\�����[�h------------------
			rect.left = mFileColumnWidth + kCellLeftRightPadding;
			if( comment.GetItemCount() == 0 )
			{
				if( prehit.GetItemCount() > 0 || posthit.GetItemCount() > 0 )
				{
					//
					prehit.ReplaceChar(kUChar_LineEnd,kUChar_Space);
					posthit.ReplaceChar(kUChar_LineEnd,kUChar_Space);
					//
					NVMC_SetDefaultTextProperty(kColor_Black,kTextStyle_Normal);//win
					ANumber	w1 = static_cast<ANumber>(NVMC_GetDrawTextWidth(prehit));
					NVMC_SetDefaultTextProperty(kColor_Black,kTextStyle_Bold);//win
					ANumber	w2 = static_cast<ANumber>(NVMC_GetDrawTextWidth(hit));
					ANumber	w = w1 + w2/2;
					ANumber	drawStartX = rect.left - (w-(rect.right-rect.left)/2);
					ALocalRect	drawRect;
					drawRect = rect;
					drawRect.left = drawStartX;
					//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Normal,true);
					NVMC_SetDefaultTextProperty(color,kTextStyle_Normal);//win
					NVMC_DrawText(drawRect,rect,prehit);
					drawRect.left += w1;
					//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Bold,true);
					NVMC_SetDefaultTextProperty(color,kTextStyle_Bold);//win
					NVMC_DrawText(drawRect,rect,hit);
					drawRect.left += w2;
					//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Normal,true);
					NVMC_SetDefaultTextProperty(color,kTextStyle_Normal);//win
					NVMC_DrawText(drawRect,rect,posthit);
				}
				//R0238
				else
				{
					NVMC_SetDefaultTextProperty(color,kTextStyle_Bold);
					NVMC_DrawText(rect,rect,hit);
				}
			}
			else
			{
				//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Normal,true);
				NVMC_SetDefaultTextProperty(color,kTextStyle_Normal);//win
				NVMC_DrawText(rect,comment);
			}
			//��؂���\��
			ALocalPoint	spt, ept;
			spt.x = ept.x = mFileColumnWidth;
			spt.y = rect.top;
			ept.y = rect.bottom;
			NVMC_DrawLine(spt,ept,normalcolor,1.0,1.0);
		}
		else
		{
			//------------------�P�J�����\�����[�h------------------
			//
			rect.left += kOneColumnMode_LeftPadding2ndLine;
			rect.top = rect.top + mLineHeight;
			//
			//������
		}
		rect.right -= kCellLeftRightPadding;
	}
}

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_Index::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	AIndex	rowIndex = clickImagePoint.y/GetRowHeight();
	if( rowIndex >= GetIndexDocument().SPI_GetDisplayRowCount() )   return false;
	
	mSelectedRowIndex = rowIndex;
	NVMC_RefreshControl();
	
	AText	path, prehit, hit, posthit, position;
	ANumber	start, length, paragraph;
	ABool	isGroup;
	AIndex	groupIndex;
	AText	comment;
	GetIndexDocumentConst().SPI_GetDisplayRowData(rowIndex,path,hit,prehit,posthit,position,start,length,paragraph,groupIndex,isGroup,comment);
	
	if( isGroup == true && inLocalPoint.x < 16 )
	{
		GetIndexDocument().SPI_ExpandCollapse(groupIndex);
		return false;
	}
	
	ANumber	w = mFileColumnWidth;
	if( inLocalPoint.x >= w-kColumnWidthChangeAreaWidth && inLocalPoint.x <= w+kColumnWidthChangeAreaWidth )
	{
		DragChangeColumnWidth(inLocalPoint);//win
		return false;
	}
	
	//�_�u���N���b�N
	//#92 if( inClickCount == 2 )
	{
		if( isGroup == true )
		{
			GetIndexDocument().SPI_ExpandCollapse(groupIndex);
		}
		else
		{
			SPI_OpenFromRow(rowIndex);
		}
	}
	return false;
}

void	AView_Index::DragChangeColumnWidth( const ALocalPoint inLocalPoint )//win
{
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	
	//�����ň�U�C�x���g�R�[���o�b�N�𔲂���BMouseTrack�͕ʂ̃C�x���g�ōs���B
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	/*
	ALocalPoint	spt, ept;
	spt.x = inLocalPoint.x;
	ept.x = spt.x;
	spt.y = localFrameRect.top;
	ept.y = localFrameRect.bottom;
	NVMC_DrawXorCaretLine(spt,ept,true,false,1);
	*/
	//TrackingMode�ݒ�
	mMouseTrackingMode = kMouseTrackingMode_SingleClick;
	mMouseTrackStartPoint = inLocalPoint;
	mMouseTrackPoint = inLocalPoint;
	NVM_SetMouseTrackingMode(true);
	return;
	//#688 }
	/*#688
	//�ŏ��̃}�E�X�ʒu�擾�i���̎��_�Ń}�E�X�{�^����������Ă����烊�^�[���BTrackMouseDown()�Ń}�E�X�{�^�������[�X�̃C�x���g���擾������A���͂������Ȃ��BB0260�j
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	if( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == false )   return;
	
	//Local���W�ɕϊ�
	ALocalPoint	mouseLocalPoint;
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
	//
	ALocalPoint	spt, ept;
	spt.x = mouseLocalPoint.x;
	ept.x = spt.x;
	spt.y = localFrameRect.top;
	ept.y = localFrameRect.bottom;
	NVMC_DrawXorCaretLine(spt,ept,true,1);
	
	//�O��}�E�X�ʒu�̕ۑ�
	ALocalPoint	previousLocalPoint = mouseLocalPoint;
	//�}�E�X�{�^�����������܂Ń��[�v
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//Local���W�ɕϊ�
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
		if( mouseLocalPoint.x == previousLocalPoint.x && mouseLocalPoint.y == previousLocalPoint.y )
		{
			continue;
		}
		//
		NVMC_DrawXorCaretLine(spt,ept,true,1);
		spt.x = mouseLocalPoint.x;
		ept.x = spt.x;
		spt.y = localFrameRect.top;
		ept.y = localFrameRect.bottom;
		NVMC_DrawXorCaretLine(spt,ept,true,1);
		
	}
	//
	NVMC_DrawXorCaretLine(spt,ept,true,1);
	
	//
	SPI_SetFileColumnWidth(spt.x-localFrameRect.left);
	*/
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
//Mouse Tracking
void	AView_Index::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( inLocalPoint.x == mMouseTrackPoint.x && inLocalPoint.y == mMouseTrackPoint.y )   return;
	
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	
	/*
	ALocalPoint	spt, ept;
	spt.x = mMouseTrackPoint.x;
	ept.x = spt.x;
	spt.y = localFrameRect.top;
	ept.y = localFrameRect.bottom;
	NVMC_DrawXorCaretLine(spt,ept,true,false,1);
	*/
	mMouseTrackPoint = inLocalPoint;
	/*
	spt.x = mMouseTrackPoint.x;
	ept.x = spt.x;
	spt.y = localFrameRect.top;
	ept.y = localFrameRect.bottom;
	NVMC_DrawXorCaretLine(spt,ept,true,false,1);
	*/
	SPI_SetFileColumnWidth(inLocalPoint.x-localFrameRect.left);
}

//Mouse Tracking�I���i�}�E�X�{�^��Up�j
void	AView_Index::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	
	/*
	ALocalPoint	spt, ept;
	spt.x = mMouseTrackPoint.x;
	ept.x = spt.x;
	spt.y = localFrameRect.top;
	ept.y = localFrameRect.bottom;
	NVMC_DrawXorCaretLine(spt,ept,true,false,1);
	*/
	//
	SPI_SetFileColumnWidth(inLocalPoint.x-localFrameRect.left);
	
	//TrackingMode����
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}
//#688 #endif

//
void	AView_Index::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
		const ALocalPoint inLocalPoint )//win 080706
{
	//���������z�C�[���֎~�Ȃ���deltaX��0�ɂ���
	ANumber	deltaX = inDeltaX;
	/*
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisableHorizontalWheelScroll) == true )
	{
		deltaX = 0;
	}
	*/
	//Y�ω��ʂ�ݒ�icmd�L�[�Ȃ��Ȃ��ɂP�i�K���j win
	ANumber	yscroll = 0;
	ANumber	scrollPercent = 100;
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true 
				|| AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )//win
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd);
		yscroll = inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100;
	}
	else
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent);
		if( inDeltaY > 0 )
		{
			yscroll = NVI_GetVScrollBarUnit();
		}
		else if( inDeltaY < 0 )//�i#956 inDeltaY��0�̂Ƃ���yscroll��0�̂܂܂ɂ���j
		{
			yscroll = -NVI_GetVScrollBarUnit();
		}
	}
	//�X�N���[�����s
	NVI_Scroll(deltaX*NVI_GetHScrollBarUnit()*scrollPercent/100,yscroll);
	
	//#92
	UpdateCursorRow(inLocalPoint);
}

//�J�[�\��
ACursorType	AView_Index::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ANumber	w = mFileColumnWidth;
	if( inLocalPoint.x >= w-kColumnWidthChangeAreaWidth && inLocalPoint.x <= w+kColumnWidthChangeAreaWidth )
	{
		return kCursorType_ResizeLeftRight;
	}
	return kCursorType_Arrow;
}

//
ABool	AView_Index::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	//#688
	outUpdateMenu = false;
	//
	ABool	result = false;
	if( inText.GetItemCount() == 1 )
	{
		if( mSelectedRowIndex != kIndex_Invalid )
		{
			AText	path, prehit, hit, posthit, position;
			ANumber	start, length, paragraph;
			ABool	isGroup;
			AIndex	groupIndex;
			AText	comment;
			GetIndexDocumentConst().SPI_GetDisplayRowData(mSelectedRowIndex,path,hit,prehit,posthit,position,start,length,paragraph,groupIndex,isGroup,comment);
			
			AUChar	ch = inText.Get(0);
			switch(ch)
			{
			  case kUChar_CR:
				{
					if( isGroup == true )
					{
						GetIndexDocument().SPI_ExpandCollapse(groupIndex);
					}
					else
					{
						SPI_OpenFromRow(mSelectedRowIndex);
					}
					result = true;
					break;
				}
			  case 0x1E://up
				{
					if( AKeyWrapper::IsCommandKeyPressed(/*#688 AKeyWrapper::GetEventKeyModifiers(inOSKeyEvent)*/inModifierKeys) == true )
					{
						SPI_SelectRow(0);
					}
					else
					{
						if( mSelectedRowIndex > 0 )
						{
							SPI_SelectRow(mSelectedRowIndex-1);
						}
					}
					result = true;
					break;
				}
			  case 0x1F://down
				{
					if( AKeyWrapper::IsCommandKeyPressed(/*#688 AKeyWrapper::GetEventKeyModifiers(inOSKeyEvent)*/inModifierKeys) == true )
					{
						SPI_SelectRow(GetIndexDocumentConst().SPI_GetDisplayRowCount()-1);
					}
					else
					{
						if( mSelectedRowIndex < GetIndexDocumentConst().SPI_GetDisplayRowCount()-1 )
						{
							SPI_SelectRow(mSelectedRowIndex+1);
						}
					}
					result = true;
					break;
				}
			}
		}
	}
	return result;
}

//#92
/**
�}�E�X�ړ��C�x���g
*/
ABool	AView_Index::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	UpdateCursorRow(inLocalPoint);
	return true;
}

//#92
/**
�}�E�X�̈�O�ړ��C�x���g
*/
void	AView_Index::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearCursorRow();
}

//#92
/**
�J�[�\��Hover�s�X�V
*/
void	AView_Index::UpdateCursorRow( const ALocalPoint& inLocalPoint )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	
	AIndex newCursorIndex = clickImagePoint.y/GetRowHeight();
	if( newCursorIndex != mCursorRowDisplayIndex )
	{
		AIndex	svRowIndex = mCursorRowDisplayIndex;
		mCursorRowDisplayIndex = newCursorIndex;
		SPI_RefreshRow(svRowIndex);
		SPI_RefreshRow(mCursorRowDisplayIndex);
	}
}

//#92
/**
�J�[�\��Hover�s�N���A
*/
void	AView_Index::ClearCursorRow()
{
	if( mCursorRowDisplayIndex == kIndex_Invalid )   return;
	AIndex	svRowIndex = mCursorRowDisplayIndex;
	mCursorRowDisplayIndex = kIndex_Invalid;
	SPI_RefreshRow(svRowIndex);
}

//#92
/**
View��deactivate���ꂽ�Ƃ��ɃR�[�������
*/
void	AView_Index::NVMDO_DoViewDeactivated()
{
	ClearCursorRow();
}

/**
View�̃T�C�Y�ύX���ɃR�[�������
*/
void	AView_Index::NVIDO_SetSize( const ANumber inWidth, const ANumber inHeight )
{
	mFileColumnWidth = inWidth * GetApp().GetAppPref().GetData_Number(AAppPrefDB::kIndexWindowSeparateLine)/100;
	SPI_UpdateImageSize();//#291
}

//#465
//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_Index::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	AIndex	rowIndex = clickImagePoint.y/GetRowHeight();
	if( rowIndex >= GetIndexDocument().SPI_GetDisplayRowCount() )   return false;
	
	mContextMenuSelectedRowIndex = rowIndex;
	
	AGlobalPoint	globalPoint = {0};
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	//#688 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(kContextMenuID_IndexView,
	//#688 globalPoint,NVM_GetWindow().NVI_GetWindowRef());
	NVMC_ShowContextMenu(kContextMenuID_IndexView,globalPoint);//#688
	return true;
}

//#541
/**
�w���v�^�O�e�L�X�g�擾
*/
ABool	AView_Index::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	outTagSide = kHelpTagSide_Default;//#643
	//#688 NVM_GetWindowConst().NVI_HideHelpTag();
	
	//�\���sindex�擾
	AImagePoint	clickImagePoint = {0,0};
	NVM_GetImagePointFromLocalPoint(inPoint,clickImagePoint);
	if( clickImagePoint.x > mFileColumnWidth )   return false;//�t�@�C�����̂ݑΉ�
	AIndex	rowIndex = clickImagePoint.y/GetRowHeight();
	if( rowIndex >= GetIndexDocumentConst().SPI_GetDisplayRowCount() )   return false;
	
	//�s�f�[�^�擾
	AText	path, prehit, hit, posthit, position;
	ANumber	start = 0, length = 0, paragraph = 0;
	ABool	isGroup = false;
	AIndex	groupIndex = kIndex_Invalid;
	AText	comment;
	GetIndexDocumentConst().SPI_GetDisplayRowData(rowIndex,path,hit,prehit,posthit,position,start,length,paragraph,groupIndex,isGroup,comment);
	
	//text�ݒ�
	outText1.SetText(path);
	outText1.Add(kUChar_LineEnd);
	outText1.AddText(position);
	outText2.SetText(outText1);
	//�srect�擾
	AImageRect	imagerect = {0,0,0,0};
	GetRowImageRect(rowIndex,rowIndex+1,imagerect);
	NVM_GetLocalRectFromImageRect(imagerect,outRect);
	return true;
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

//
void	AView_Index::SPI_UpdateRows( const AIndex inStartRowIndex, const AIndex inEndRowIndex ) 
{
	//�\���t���[����Image���W�Ŏ擾
	AImageRect	frameImageRect;
	NVM_GetImageViewRect(frameImageRect);
	//
	AImageRect	imageRect;
	GetRowImageRect(inStartRowIndex,inEndRowIndex,imageRect);
	if( frameImageRect.top < imageRect.bottom || frameImageRect.bottom > imageRect.top )
	{
		ALocalRect	localRect;
		NVM_GetLocalRectFromImageRect(imageRect,localRect);
		NVMC_RefreshRect(localRect);
	}
}

//
void	AView_Index::SPI_UpdateImageSize()
{
	ANumber	width = NVI_GetViewWidth();
	ANumber	height = GetIndexDocumentConst().SPI_GetDisplayRowCount() * GetRowHeight();
	NVM_SetImageSize(width,height);
}

//
void	AView_Index::SPI_UpdateTextDrawProperty()
{
	AColor	color = kColor_Black;
	//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kIndexWinNormalColor,color);
	/*#844
	AText	fontname;//win
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kIndexWindowFontName,fontname);//win
	*/
	AFloatNumber	fontsize = GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kSubWindowsFontSize);//#844
	AText	fontname;
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kSubWindowsFontName,fontname);//#844
	NVMC_SetDefaultTextProperty(fontname,fontsize,color,kTextStyle_Normal,true);
	NVMC_GetMetricsForDefaultTextProperty(mLineHeight,mAscent);
	if( mOneColumnMode == false )
	{
		mLineHeight += kLineMargin;
	}
	else
	{
		mLineHeight += kOneColumnMode_LineMargin;
	}
	
	SPI_UpdateScrollBarUnit();
}

//
void	AView_Index::SPI_UpdateScrollBarUnit()
{
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(kHScrollBarUnit,GetRowHeight(),rect.right-rect.left-kHScrollBarUnit*5,rect.bottom-rect.top-GetRowHeight()*5);
}

//
ANumber	AView_Index::GetRowHeight() const
{
	if( mOneColumnMode == false )
	{
		return mLineHeight;
	}
	else
	{
		return mLineHeight*2 + kOneColumnMode_BoxTopMargin + kOneColumnMode_BoxBottomMargin;
	}
}

//
void	AView_Index::GetRowImageRect( const AIndex inStartRowIndex, const AIndex inEndRowIndex, AImageRect& outRowImageRect ) const
{
	outRowImageRect.left	= 0;
	outRowImageRect.right	= NVM_GetImageWidth();
	outRowImageRect.top		= inStartRowIndex*GetRowHeight();
	outRowImageRect.bottom	= inEndRowIndex*GetRowHeight();
}

void	AView_Index::SPI_OpenFromRow( const AIndex inRowIndex ) const
{
	//���ڑI���A�N�e�B�u�T�u�E�C���h�E�ɐݒ�
	GetApp().SPI_SetActiveSubWindowForItemSelector(NVM_GetWindowConst().GetObjectID());
	//
	GetApp().SPI_SleepForAWhileMultiFileFind();
	//�s�f�[�^�̎擾
	AText	path, prehit, hit, posthit, position;
	ANumber	start = 0, length = 0, paragraph = 0;
	ABool	isGroup = false;
	AIndex	groupIndex = 0;
	AText	comment;
	GetIndexDocumentConst().SPI_GetDisplayRowData(inRowIndex,path,hit,prehit,posthit,position,start,length,paragraph,groupIndex,isGroup,comment);
	if( path.GetItemCount() == 0 )   return;//#221
	if( isGroup == true )   return;//#960 �O���[�v���ڂ̏ꍇ�̓t�@�C���I�[�v���������s��Ȃ�
	//���݂̈ʒu��Navigate�o�^���� #146
	GetApp().SPI_RegisterNavigationPosition();
	//
	if( path.Get(0) == kUChar_Tab )//#221
	{
		//==================�V�K�h�L�������g�̏ꍇ==================
		//#221 �V�K�h�L�������g�̏ꍇ�́A�t�@�C���p�X�Ƀ^�u�R�[�h+DocumentID���i�[���邱�Ƃɂ���
		ADocumentID	docID;
		AIndex	pos = 1;
		AUniqID	docUniqID;//#693
		path.ParseIntegerNumber(pos,docUniqID.val,false);
		docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		if( GetApp().NVI_IsDocumentValid(docID) == true )
		{
			if( GetApp().NVI_GetDocumentTypeByID(docID) == kDocumentType_Text )
			{
				GetApp().SPI_GetTextDocumentByID(docID).SPI_SelectText(start,length,true);
			}
		}
	}
	else
	{
		//==================�ʏ�t�@�C���̏ꍇ==================
		/*#725
		AFileAcc	file;
		file.Specify(path);
		GetApp().SPI_SelectTextDocument(file,start,length,true);
		*/
		//�t�@�C���̎w��ꏊ���J��
		GetApp().SPI_OpenOrRevealTextDocument(path,start,start+length,kObjectID_Invalid,false,kAdjustScrollType_Center);
	}
}

void	AView_Index::SPI_ScrollToHome()
{
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
}

void	AView_Index::SPI_SelectNextRow()
{
	AIndex	old = mSelectedRowIndex;
	if( mSelectedRowIndex == kIndex_Invalid )
	{
		mSelectedRowIndex = 0;
	}
	else
	{
		if( mSelectedRowIndex < GetIndexDocumentConst().SPI_GetDisplayRowCount()-1 )
		{
			mSelectedRowIndex++;
		}
		else return;
	}
	SPI_RefreshRow(old);
	SPI_RefreshRow(mSelectedRowIndex);
	SPI_OpenFromRow(mSelectedRowIndex);
	SPI_AdjustScroll();
}

void	AView_Index::SPI_SelectPreviousRow()
{
	AIndex	old = mSelectedRowIndex;
	if( mSelectedRowIndex == kIndex_Invalid )
	{
		mSelectedRowIndex = GetIndexDocumentConst().SPI_GetDisplayRowCount()-1;
	}
	else
	{
		if( mSelectedRowIndex > 0 )
		{
			mSelectedRowIndex--;
		}
		else return;
	}
	SPI_RefreshRow(old);
	SPI_RefreshRow(mSelectedRowIndex);
	SPI_OpenFromRow(mSelectedRowIndex);
	SPI_AdjustScroll();
}

void	AView_Index::SPI_SelectRow( const AIndex inDisplayRowIndex )
{
	AIndex	old = mSelectedRowIndex;
	mSelectedRowIndex = inDisplayRowIndex;
	SPI_RefreshRow(old);
	SPI_RefreshRow(mSelectedRowIndex);
	SPI_AdjustScroll();
}

void	AView_Index::SPI_AdjustScroll()
{
	if( mSelectedRowIndex == kIndex_Invalid )
	{
		AImagePoint	pt = {0,0};
		NVI_ScrollTo(pt);
		return;
	}
	AImageRect	rect;
	NVM_GetImageViewRect(rect);
	AIndex	startRow = rect.top / GetRowHeight();
	AIndex	endRow = rect.bottom / GetRowHeight();
	if( mSelectedRowIndex < startRow || mSelectedRowIndex >= endRow )
	{
		AImagePoint	pt;
		pt.x = 0;
		pt.y = (mSelectedRowIndex-(endRow-startRow)/2)*GetRowHeight();
		NVI_ScrollTo(pt);
	}
}

void	AView_Index::SPI_RefreshRow( const AIndex inDisplayRowIndex )
{
	if( inDisplayRowIndex == kIndex_Invalid )   return;
	AImageRect	rowImageRect;
	GetRowImageRect(inDisplayRowIndex,inDisplayRowIndex+1,rowImageRect);
	ALocalRect	rowLocalRect;
	NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
	NVMC_RefreshRect(rowLocalRect);
}

void	AView_Index::SPI_SetOnlyFileColumnWidth( const ANumber inNewWidth )
{
	mFileColumnWidth = inNewWidth;
}

void	AView_Index::SPI_SetInfoColumnWidth( const ANumber inNewWidth )
{
#if 0
	//�J�������ύX���ɂ̓E�C���h�E�T�C�Y���ύX����d�l���������A�s�v�i���܂�ǂ��Ȃ��d�l�j�Ǝv����̂ŁA�ʏ�̃C���^�[�t�F�C�X�ɂ���
	if( NVI_IsBorrowerView() == false )//#92
	{
		ANumber	oldWidth = NVI_GetViewWidth()-mFileColumnWidth;
		ARect	rect;
		/*#199 mWindow*/NVM_GetWindowConst().NVI_GetWindowBounds(rect);
		ANumber	windowOldWidth = rect.right-rect.left;
		ANumber	windowNewWidth = windowOldWidth + inNewWidth - oldWidth;
		rect.right = rect.left + windowNewWidth;
		/*#199 mWindow*/NVM_GetWindow().NVI_SetWindowBounds(rect);
		//B0415 mWindow.EVT_WindowResizeCompleted();
		//
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIndexWindowSeparateLine,mFileColumnWidth*100/windowNewWidth);
	}
	else
#endif
	{
		//#92
		mFileColumnWidth = NVI_GetViewWidth() - inNewWidth;
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIndexWindowSeparateLine,mFileColumnWidth*100/NVI_GetViewWidth());
	}
	NVI_Refresh();
}

void	AView_Index::SPI_SetFileColumnWidth( const ANumber inNewWidth )
{
#if 0
	//�J�������ύX���ɂ̓E�C���h�E�T�C�Y���ύX����d�l���������A�s�v�i���܂�ǂ��Ȃ��d�l�j�Ǝv����̂ŁA�ʏ�̃C���^�[�t�F�C�X�ɂ���
	if( NVI_IsBorrowerView() == false )//#92
	{
		ANumber	oldWidth = mFileColumnWidth;
		mFileColumnWidth = inNewWidth;
		ARect	rect;
		/*#199 mWindow*/NVM_GetWindowConst().NVI_GetWindowBounds(rect);
		ANumber	windowOldWidth = rect.right-rect.left;
		ANumber	windowNewWidth = windowOldWidth + inNewWidth - oldWidth;
		rect.right = rect.left + windowNewWidth;
		/*#199 mWindow*/NVM_GetWindow().NVI_SetWindowBounds(rect);
		//B0415 mWindow.EVT_WindowResizeCompleted();
		//
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIndexWindowSeparateLine,inNewWidth*100/windowNewWidth);
	}
	else
#endif
	{
		//#92
		mFileColumnWidth = inNewWidth;
		//
		if( mFileColumnWidth < kColumnMinWidth )
		{
			mFileColumnWidth = kColumnMinWidth;
		}
		if( NVI_GetViewWidth()-mFileColumnWidth < kColumnMinWidth )
		{
			mFileColumnWidth = NVI_GetViewWidth() - kColumnMinWidth;
		}
		//
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIndexWindowSeparateLine,mFileColumnWidth*100/NVI_GetViewWidth());
	}
	NVI_Refresh();
}

void	AView_Index::SPI_GetColumnWidth( ANumber& outFileColumnWidth, ANumber& outInfoColumnWidth ) const
{
	outFileColumnWidth = mFileColumnWidth;
	outInfoColumnWidth = NVI_GetViewWidth()-mFileColumnWidth;
}

//#92
/**
�ۑ��_�C�A���O��View�̐e�E�C���h�E��ɕ\������
*/
void	AView_Index::SPI_ShowSaveWindow()
{
	if( GetApp().NVI_GetWindowByID(NVM_GetWindowID()).NVI_GetWindowType() == kWindowType_FindResult )
	{
		GetApp().SPI_GetFindResultWindow(NVM_GetWindowID()).SPI_ShowSaveWindow();
	}
}

//#465
AIndex	AView_Index::SPI_GetContextMenuSelectedRowDBIndex() const
{
	return mContextMenuSelectedRowIndex;
}

//#465
void	AView_Index::SPI_ExportToNewDocument( const AIndex inRowIndex ) const
{
	AText	path, prehit, hit, posthit, position;
	ANumber	start, length, paragraph;
	ABool	isGroup;
	AIndex	groupIndex;
	AText	comment;
	AText	paratext;
	GetIndexDocumentConst().SPI_GetDisplayRowData(inRowIndex,path,hit,prehit,posthit,position,
				start,length,paragraph,groupIndex,isGroup,comment);
	//�O���[�v�f�[�^�擾
	AFileAcc	baseFolder;
	GetIndexDocumentConst().SPI_GetGroupBaseFolder(groupIndex,baseFolder);
	//
	AText	text;
	//
	AText	title1, title2;
	GetIndexDocumentConst().SPI_GetGroupTitle(groupIndex,title1,title2);
	text.AddText(title1);
	text.Add(kUChar_LineEnd);
	text.AddText(title2);
	text.Add(kUChar_LineEnd);
	//
	for( AIndex index = 0; index < GetIndexDocumentConst().SPI_GetItemCountInGroup(groupIndex); index++ )
	{
		GetIndexDocumentConst().SPI_GetItemData(groupIndex,index,
					path,hit,prehit,posthit,position,paratext,
					start,length,paragraph,comment);
		//
		if( path.GetItemCount() > 0 )
		{
			AFileAcc	file;
			file.Specify(path);
			AText	relativePath;
			file.GetPartialPath(baseFolder,relativePath);
			text.AddText(relativePath);
			text.Add('(');
			text.AddNumber(paragraph+1);
			text.Add(')');
			//
			if( position.GetItemCount() > 0 )
			{
				text.Add('[');
				text.AddText(position);
				text.AddCstring("]: ");
			}
			else
			{
				text.AddCstring(": ");
			}
			//
			if( paratext.GetItemCount() > 0 )
			{
				if( paratext.Get(paratext.GetItemCount()-1) == kUChar_LineEnd )
				{
					//�Ō�̉��s�R�[�h�͍폜
					paratext.Delete1(paratext.GetItemCount()-1);
				}
			}
			text.AddText(paratext);
		}
		else
		{
			text.AddText(hit);
		}
		text.Add(kUChar_LineEnd);
	}
	//
	ADocumentID	docID = GetApp().SPNVI_CreateNewTextDocument();
	if( docID == kObjectID_Invalid )   return;//win
	GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,text);
}


