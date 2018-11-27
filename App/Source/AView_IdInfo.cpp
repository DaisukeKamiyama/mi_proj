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

AView_IdInfo

*/

#include "stdafx.h"

#include "AView_IdInfo.h"
#include "AApp.h"

//�|�b�v�A�b�vid info��hide�^�C�}�[�l
const ANumber	kTimerForHidePopupIdInfo = 70;//#1354 80��70
const ANumber	kTimerForHidePopupIdInfo_AfterArgIndexChanged = 150;//#1354 160��150

//�p�ۂ̔��a #1079
const AFloatNumber	kRoundedRectRad = 3.0f;

#pragma mark ===========================
#pragma mark [�N���X]AView_IdInfo
#pragma mark ===========================
//���E�C���h�E�̃��C��View

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_IdInfo::AView_IdInfo( /* #199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID ) 
	: AView(/*#199 inParent,inWindow*/inWindowID,inID), mCurrentModeIndex(kIndex_Invalid), mArgIndex(kIndex_Invalid), mCurrentHoverCursorIndex(kIndex_Invalid)
	,mFixDisplay(false)//#238
//#725	,mTransparency(1.0)//#291
	,mContextMenuItemID(kIndex_Invalid)//#442
,mCurrentSelectionIndex(kIndex_Invalid)//#853
,mMouseTrackStartLocalPoint(kLocalPoint_00), mMouseDownResultIsDrag(false)
,mHideCounter(0),mAlpha(1.0), mDisableAutoHide(false)
{
	NVMC_SetOffscreenMode(true);//win
	//#92 ��������NVIDO_Init()�ֈړ�
	
	//�f�[�^�S�폜
	SPI_DeleteAllIdInfo();
}

//�f�X�g���N�^
AView_IdInfo::~AView_IdInfo()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_IdInfo::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_IdInfo::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/*#853
#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kFileNameFontSize = 9.0;
const AFloatNumber	kCategoryNameFontSize = 8.0;
const AFloatNumber	kInfoTextFontSize = 8.5;
const AFloatNumber	kCommentTextFontSize = 8;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const AFloatNumber	kFileNameFontSize = 7.8;
const AFloatNumber	kCategoryNameFontSize = 7.8;
const AFloatNumber	kInfoTextFontSize = 8.0;
const AFloatNumber	kCommentTextFontSize = 8.0;
#endif
*/
const ANumber	kHeaderLeftMargin = 6;
const ANumber	kHeaderRightMargin = 6;
const ANumber	kHeaderTopMargin = 2;
const ANumber	kHeaderBottomMargin = 2;
const ANumber	kInfoTextLeftMargin = 8;
const ANumber	kInfoTextLeftMargin2 = 9;
const ANumber	kInfoTextRightMargin = 3;
const ANumber	kInfoTextTopMargin = 2;
const ANumber	kInfoTextBottomMargin = 1;
const ANumber	kCommentTextLeftMargin = 8;
const ANumber	kCommentTextRightMargin = 3;
const ANumber	kCommentTextTopMargin = 2;
const ANumber	kCommentTextBottomMargin = 1;
//const ANumber	kFilenameRightMargin = 16;

//#643
const ANumber	kItemBoxLeftMargin = 5;
const ANumber	kItemBoxRightMargin = 5;
const ANumber	kItemBoxTopMargin = 2;
const ANumber	kItemBoxBottomMargin = 1;

const ANumber	kVScrollUnit = 8;

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_IdInfo::EVTDO_DoDraw()
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	
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
	
	//==================�F�擾==================
	
	//�`��F�ݒ�
	AColor	letterColor = kColor_Black;
	AColor	dropShadowColor = kColor_White;
	AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
	GetApp().SPI_GetSubWindowBoxColor(NVM_GetWindow().GetObjectID(),letterColor,dropShadowColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
	NVMC_SetDropShadowColor(dropShadowColor);
	
	//=========================�e���ږ����[�v=========================
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		//����rect�擾
		AImageRect	itemImageRect = {0};
		GetItemBoxImageRect(i,itemImageRect);//#643
		ALocalRect	itemLocalRect = {0};
		NVM_GetLocalRectFromImageRect(itemImageRect,itemLocalRect);
		
		//updateRect���ɂȂ��ꍇ��continue
		if( NVMC_IsRectInDrawUpdateRegion(itemLocalRect) == false )   continue;
		
		//�L�[���[�h�擾
		AText	keywordText;
		mKeywordTextArray.Get(i,keywordText);
		
		//�Ō�̍��ڈȊO�́A���Ԃ�w�i�F�i��:0.2�j�ŕ`��
		if( i != mInfoTextArray.GetItemCount() -1 )
		{
			ALocalRect	gapLocalRect = {0};
			gapLocalRect.left		= itemLocalRect.left + 5;
			gapLocalRect.top		= itemLocalRect.bottom;
			gapLocalRect.right		= itemLocalRect.right - 5;
			gapLocalRect.bottom		= itemLocalRect.bottom + kItemBoxTopMargin + kItemBoxBottomMargin;
			NVMC_PaintRect(gapLocalRect,/*kColor_White*/boxBaseColor2,0.2);
		}
		/*test
		if( mCurrentHoverCursorIndex == i )
		{
			{
				ALocalRect	shadowLocalRect = itemLocalRect;
				shadowLocalRect.bottom		= shadowLocalRect.top;
				shadowLocalRect.left		-= 5;
				shadowLocalRect.top			-= 5;
				shadowLocalRect.right		+= 5;
				NVMC_PaintRoundedRect(shadowLocalRect,kColor_White,kColor_White,kGradientType_Vertical,0.0,0.8,
									  5,true,true,false,false);
			}
			{
				ALocalRect	shadowLocalRect = itemLocalRect;
				shadowLocalRect.top			= shadowLocalRect.bottom;
				shadowLocalRect.bottom		+= 5;
				shadowLocalRect.left		-= 5;
				shadowLocalRect.right		+= 5;
				NVMC_PaintRoundedRect(shadowLocalRect,kColor_White,kColor_White,kGradientType_Vertical,0.8,0.0,
									  5,false,false,true,true);
			}
			{
				ALocalRect	shadowLocalRect = itemLocalRect;
				shadowLocalRect.right		= shadowLocalRect.left;
				shadowLocalRect.left		-= 5;
				NVMC_PaintRoundedRect(shadowLocalRect,kColor_White,kColor_White,kGradientType_Horizontal,0.0,0.8,
									  5,false,false,false,false);
			}
			{
				ALocalRect	shadowLocalRect = itemLocalRect;
				shadowLocalRect.left		= shadowLocalRect.right;
				shadowLocalRect.right		+= 5;
				NVMC_PaintRoundedRect(shadowLocalRect,kColor_White,kColor_White,kGradientType_Horizontal,0.8,0.0,
									  5,false,false,false,false);
			}
		}
		*/
		//�{�b�N�X�`��#643
		NVMC_PaintRoundedRect(itemLocalRect,
							  //kColor_White,kColor_Gray95Percent
							  boxBaseColor1,boxBaseColor3,
							  kGradientType_Vertical,backgroundAlpha*mAlpha,backgroundAlpha*mAlpha,
							  kRoundedRectRad,true,true,true,true);
		NVMC_FrameRoundedRect(itemLocalRect,
							  //kColor_Gray20Percent
							  kColor_Gray50Percent,
							  mAlpha,kRoundedRectRad,true,true,true,true);//,true,true,true,true,1.0);
		//�z�o�[�`��
		if( mCurrentHoverCursorIndex == i )
		{
			/*
			AColor	selcolor;
			AColorWrapper::GetHighlightColor(selcolor);
			NVMC_PaintRoundedRect(itemLocalRect,selcolor,selcolor,kGradientType_Vertical,0.1,0.1,5,true,true,true,true);
			NVMC_FrameRoundedRect(itemLocalRect,selcolor,1.0,5,true,true,true,true);
			*/
			//�z�o�[�F��rounded rect�ň͂�
			AColor	hoverColor = GetApp().SPI_GetSubWindowRoundedRectBoxHoverColor();
			NVMC_FrameRoundedRect(itemLocalRect,hoverColor,0.3,kRoundedRectRad,true,true,true,true,true,true,true,true,1.0);
		}
		//�I��`��
		if( mCurrentSelectionIndex == i )
		{
			/*
			AColor	selcolor = kColor_Blue;
			AColorWrapper::GetHighlightColor(selcolor);
			NVMC_PaintRoundedRect(itemLocalRect,selcolor,selcolor,kGradientType_Vertical,0.3,0.3,5,true,true,true,true);
			NVMC_FrameRoundedRect(itemLocalRect,selcolor,1.0,5,true,true,true,true);
			*/
			//�I��F��rounded rect�ň͂�
			AColor	boxFrameColor = GetApp().SPI_GetSubWindowRoundedRectBoxSelectionColor(NVM_GetWindow().GetObjectID());
			NVMC_FrameRoundedRect(itemLocalRect,boxFrameColor,1.0,kRoundedRectRad,true,true,true,true,true,true,true,true,1.0);
		}
		
		//=========================�w�b�_�����`��=========================
		
		//���[�h�ݒ�X�V�ƃX���b�h���s�Ƃ̂���Ⴂ�̏ꍇ�ɃJ�e�S��index���I�[�o�[����\��������̂ŁA�O�̂��߁B
		AIndex	categoryIndex = mCategoryIndexArray.Get(i);
		if( categoryIndex < 0 || categoryIndex >= GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryCount() )
		{
			categoryIndex = 0;
		}
		
		//�w�b�_�F�擾
		AColor	headercolor;
		AText	filename;
		switch(mScopeTypeArray.Get(i))
		{
		  case kScopeType_Local:
			{
				filename.SetLocalizedText("Scope_Local");
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryLocalColor(categoryIndex,headercolor);
				break;
			}
		  case kScopeType_Global:
			{
				mFilePathArray.GetTextConst(i).GetFilename(filename);
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryImportColor(categoryIndex,headercolor);
				break;
			}
		  case kScopeType_Import:
			{
				mFilePathArray.GetTextConst(i).GetFilename(filename);
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryImportColor(categoryIndex,headercolor);
				break;
			}
		  default://#147
			{
				filename.SetText(keywordText);
				//�L�[���[�h�ɃJ���[�X���b�g�w�肪����΂��̐F���擾
				AIndex	colorSlotIndex = mColorSlotIndexArray.Get(i);
				if( colorSlotIndex != kIndex_Invalid )
				{
					ATextStyle	style = kTextStyle_Normal;
					GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetColorSlotData(colorSlotIndex,headercolor,style);
				}
				//�J���[�X���b�g�w�肪�Ȃ���΃J�e�S���[�F���擾
				else
				{
					GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryColor(categoryIndex,headercolor);
				}
				break;
			}
		}
		//�w�b�_rect�擾
		AImageRect	headerImageRect = itemImageRect;//#643
		headerImageRect.bottom = itemImageRect.top + mDrawData_HeaderHeight.Get(i);//#643
		ALocalRect	headerLocalRect = {0};
		NVM_GetLocalRectFromImageRect(headerImageRect,headerLocalRect);
		//�w�b�_rect�`��
		AColor	headercolor_start = AColorWrapper::ChangeHSV(headercolor,0,0.8,1.0);//#643
		AColor	headercolor_end = AColorWrapper::ChangeHSV(headercolor,0,1.2,1.0);//#643
		NVMC_PaintRoundedRect(headerLocalRect,headercolor_start,headercolor_end,kGradientType_Vertical,0.15*mAlpha,0.15*mAlpha,
							  kRoundedRectRad,true,true,true,true);
		
		//=========================�J�e�S�����E���ڔԍ��e�L�X�g�`��=========================
		
		//�t�@�C�����t�H���g�擾
		NVMC_SetDefaultTextProperty(labelfontname,fontsize-0.5,/*kColor_Black*/letterColor,kTextStyle_Normal,true,mAlpha);
		ANumber	labelfontheight = 9 ,labelfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(labelfontheight,labelfontascent);
		//�J�e�S���[���t�H���g�擾
		NVMC_SetDefaultTextProperty(labelfontname,fontsize-1.0,/*kColor_Black*/letterColor,kTextStyle_Normal,true,mAlpha);//#1354 0.5��1.0
		ANumber	categoryfontheight = 9, categoryfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(categoryfontheight,categoryfontascent);
		//�J�e�S���e�L�X�g�擾
		AText	categoryname;
		GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryName(categoryIndex,categoryname);
		//���ڔԍ��`��
		categoryname.AddFormattedCstring("  (%d/%d)",i+1,mInfoTextArray.GetItemCount());
		//�J�e�S�����`��rect�擾
		AImageRect	categorynameImageRect = itemImageRect;//#643
		categorynameImageRect.right		= itemImageRect.right - kHeaderRightMargin;
		categorynameImageRect.top 		= itemImageRect.top + labelfontascent - categoryfontascent +kHeaderTopMargin;//#643
		categorynameImageRect.bottom 	= itemImageRect.top + labelfontheight +kHeaderTopMargin;//#643
		ALocalRect	categorynameLocalRect = {0};
		NVM_GetLocalRectFromImageRect(categorynameImageRect,categorynameLocalRect);
		//�J�e�S�����`��
		NVMC_DrawText(categorynameLocalRect,categoryname,/*kColor_Black*/letterColor,kTextStyle_Normal,kJustification_Right);
		
		//=========================�t�@�C�����e�L�X�g�`��=========================
		
		//�t�@�C�����`��rect�擾
		AImageRect	filenameImageRect = itemImageRect;//#643
		filenameImageRect.left		= itemImageRect.left + kHeaderLeftMargin;//#643
		filenameImageRect.bottom 	= itemImageRect.top + labelfontheight +kHeaderTopMargin;//#643
		filenameImageRect.top		+= kHeaderTopMargin;
		filenameImageRect.right		-= NVMC_GetDrawTextWidth(categoryname) + 4;//#1354 32��4
		ALocalRect	filenameLocalRect = {0};
		NVM_GetLocalRectFromImageRect(filenameImageRect,filenameLocalRect);
		//filename����̏ꍇ�̓L�[���[�h���i�[
		if( filename.GetItemCount() == 0 )
		{
			filename.SetText(mKeywordTextArray.GetTextConst(i));
		}
		//�E���́u�J�e�S�����v�{�u���ڔԍ��v�������������̈���Ńt�@�C������`��i�`�悵����Ȃ��ꍇ��...�ŏȗ��j
		AText	ellipsisFilename;
		NVI_GetEllipsisTextWithFixedLastCharacters(filename,filenameLocalRect.right-filenameLocalRect.left,5,ellipsisFilename);
		//�t�@�C�����e�L�X�g�`��
		if( filename.GetItemCount() > 0 )
		{
			NVMC_DrawText(filenameLocalRect,ellipsisFilename,/*kColor_Black*/letterColor,kTextStyle_Bold/*|kTextStyle_DropShadow*/,kJustification_Left);
		}
		
		//�N���X���\�� (���N���X��)
		AText	parentKeyword;
		mParentKeywordArray.Get(i,parentKeyword);
		if( parentKeyword.GetItemCount() > 0 )
		{
			ALocalRect	classNameLocalRect = filenameLocalRect;
			classNameLocalRect.left += NVMC_GetDrawTextWidth(ellipsisFilename) +16;
			AText	classname;
			classname.SetCstring("(");
			AText	t;
			if( mRequestedParentKeyword.Find(parentKeyword) != kIndex_Invalid )
			{
				//��v�\���i���j
				t.SetLocalizedText("Letter_ParentKeyword_Matched");
			}
			else
			{
				//�s��v�\���i���j
				t.SetLocalizedText("Letter_ParentKeyword");
			}
			classname.AddText(t);
			classname.AddText(parentKeyword);
			classname.AddCstring(")");
			AText	ellipsisClassname;
			NVI_GetEllipsisTextWithFixedLastCharacters(classname,classNameLocalRect.right-classNameLocalRect.left,5,ellipsisClassname);
			NVMC_DrawText(classNameLocalRect,ellipsisClassname,/*kColor_Black*/letterColor,kTextStyle_Bold/*|kTextStyle_DropShadow*/,kJustification_Left);
		}
		
		
		//=========================�R�����g�i@note�j�����`��=========================
		
		//CommentText
		if( mDrawData_CommentTextLineStart.GetObject(i).GetItemCount() > 0 )
		{
			//�t�H���g�ݒ�
			NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,kColor_Red,kTextStyle_Normal,true,mAlpha);
			ANumber	commenttextfontheight = 9 ,commenttextfontascent = 7;
			NVMC_GetMetricsForDefaultTextProperty(commenttextfontheight,commenttextfontascent);
			//�e�s�`��
			for( AIndex lineIndex = 0; lineIndex < mDrawData_CommentTextLineStart.GetObjectConst(i).GetItemCount(); lineIndex++ )
			{
				//�s�f�[�^�擾
				AIndex	start 	= mDrawData_CommentTextLineStart .GetObjectConst(i).Get(lineIndex);
				AItemCount	len = mDrawData_CommentTextLineLength.GetObjectConst(i).Get(lineIndex);
				AText	text;
				mCommentTextArray.GetTextConst(i).GetText(start,len,text);
				//textdata�ݒ�
				CTextDrawData	textDrawData(false);
				textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
				//�`��rect�擾
				AImageRect	lineImageRect = itemImageRect;//#643
				lineImageRect.left		= itemImageRect.left + kCommentTextLeftMargin;//#643
				lineImageRect.top 		= itemImageRect.top + mDrawData_HeaderHeight.Get(i) 
						+ mDrawData_InfoTextHeight.Get(i)
						+ lineIndex*commenttextfontheight + kInfoTextTopMargin;//#643
				lineImageRect.bottom	= lineImageRect.top + commenttextfontheight + kInfoTextTopMargin;//#643
				ALocalRect	lineLocalRect = {0};
				NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
				NVMC_DrawText(lineLocalRect,text,kColor_Red,kTextStyle_Bold);//#835 test textDrawData);
			}
		}
		
		//=========================InfoText�����i�{�̕����j�`��=========================
		
		//�J�e�S���F���Â����čʓx���グ���F���擾�i�|�b�v�A�b�v�E�C���h�E�ł͂Ȃ��ꍇ�̂݁j
		AColor	keywordColor = headercolor;
		if( GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()) != kSubWindowLocationType_Popup )
		{
			keywordColor = AColorWrapper::ChangeHSV(headercolor,0,3.0,0.7);
		}
		
		//InfoText�t�H���g�ݒ�
		NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,/*kColor_Black*/letterColor,kTextStyle_Normal,true,mAlpha);
		ANumber	infotextfontheight = 9, infotextfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(infotextfontheight,infotextfontascent);
		//
		ANumber	lineleftmargin = kInfoTextLeftMargin;
		ABool	argmode = false;
		ABool	argstartdone = false;
		AIndex	currentargindex = kIndex_Invalid;
		//�e�s���ƃ��[�v
		for( AIndex lineIndex = 0; lineIndex < mDrawData_InfoTextLineStart.GetObjectConst(i).GetItemCount(); lineIndex++ )
		{
			//�s�f�[�^�擾
			AIndex	start 	= mDrawData_InfoTextLineStart .GetObjectConst(i).Get(lineIndex);
			AItemCount	len = mDrawData_InfoTextLineLength.GetObjectConst(i).Get(lineIndex);
			AText	text;
			mInfoTextArray.GetTextConst(i).GetText(start,len,text);
			//text draw data�擾
			CTextDrawData	textDrawData(false);
			textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
			//�s�̍ŏ���attr�ݒ�i�O�s����A���_�[���C���p���Ȃ�A���_�[���C���ɂ���j
			//#853if( currentargindex != mArgIndex || mArgIndex == kIndex_Invalid )
			{
				textDrawData.attrPos.Add(0);
				textDrawData.attrColor.Add(letterColor);//kColor_Black);
				textDrawData.attrStyle.Add(kTextStyle_Normal);
			}
			/*#853 �Ώۈ����n�C���C�g��
			else
			{
				textDrawData.attrPos.Add(0);
				textDrawData.attrColor.Add(letterColor);//kColor_Black);
				textDrawData.attrStyle.Add(kTextStyle_Underline);
			}
			*/
			//�O�s����n�C���C�g�p���Ȃ�A�ŏ�����n�C���C�g
			AIndex	argStartX = kIndex_Invalid, argEndX = kIndex_Invalid;
			if( currentargindex == mArgIndex && mArgIndex != kIndex_Invalid )
			{
				//�n�C���C�g�J�n�ʒu�ݒ�
				argStartX = 0;
			}
			//
			AItemCount	textlen = text.GetItemCount();
			for( AIndex pos = 0; pos < textlen; )
			{
				if( text.SkipTabSpace(pos,textlen) == false )   break;
				AText	token;
				AIndex	tokenspos = pos;
				text.GetToken(pos,token);
				//�L�[���[�h�������J�e�S���F�ŕ`��
				if( token.Compare(keywordText) == true )
				{
					textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos));
					textDrawData.attrColor.Add(keywordColor);//categoryColorDraken);//headercolor);
					textDrawData.attrStyle.Add(kTextStyle_Bold/*|kTextStyle_DropShadow*/);
					textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(pos));
					textDrawData.attrColor.Add(letterColor);//kColor_Black);
					textDrawData.attrStyle.Add(kTextStyle_Normal);
				}
				//�����̃A���_�[���C���`��
				else if( token.GetItemCount() == 1 )
				{
					AUChar	ch = token.Get(0);
					if( GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetSyntaxDefinition().IsIdInfoStartChar(ch) == true && argstartdone == false )
					{
						argmode = true;
						argstartdone = true;
						currentargindex = 0;
						if( mArgIndex == currentargindex )
						{
							/*#853 �Ώۈ����n�C���C�g��
							textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(pos));
							textDrawData.attrColor.Add(letterColor);//kColor_Black);
							textDrawData.attrStyle.Add(kTextStyle_Underline);
							*/
							//�n�C���C�g�J�n�ʒu�ݒ�
							argStartX = pos;
						}
					}
					if( GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetSyntaxDefinition().IsIdInfoEndChar(ch) == true && argmode == true )
					{
					
						argmode = false;
						if( mArgIndex == currentargindex )
						{
							/*#853 �Ώۈ����n�C���C�g��
							textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos));
							textDrawData.attrColor.Add(letterColor);//kColor_Black);
							textDrawData.attrStyle.Add(kTextStyle_Normal);
							*/
							//�n�C���C�g�I���ʒu�ݒ�
							argEndX = tokenspos;
						}
						currentargindex = kIndex_Invalid;
					}
					if( GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetSyntaxDefinition().IsIdInfoDelimiterChar(ch) == true && argmode == true )
					{
						if( mArgIndex == currentargindex )
						{
							/*#853
							textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos));
							textDrawData.attrColor.Add(letterColor);//kColor_Black);
							textDrawData.attrStyle.Add(kTextStyle_Normal);
							*/
							//�n�C���C�g�I���ʒu�ݒ�
							argEndX = tokenspos;
						}
						currentargindex++;
						if( mArgIndex == currentargindex )
						{
							/*#853
							textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(pos));
							textDrawData.attrColor.Add(letterColor);//kColor_Black);
							textDrawData.attrStyle.Add(kTextStyle_Underline);
							*/
							//�n�C���C�g�J�n�ʒu�ݒ�
							argStartX = pos;
						}
					}
				}
			}
			//�����n�C���C�g
			if( argStartX != kIndex_Invalid )
			{
				//�I���ʒu���ݒ�Ȃ�s�Ō�܂�
				if( argEndX == kIndex_Invalid )
				{
					argEndX = textlen;
				}
				//�`��rect�擾
				AImageRect	imageRect = itemImageRect;
				imageRect.left			= itemImageRect.left + lineleftmargin + NVMC_GetImageXByTextPosition(textDrawData,argStartX);
				imageRect.right			= itemImageRect.left + lineleftmargin + NVMC_GetImageXByTextPosition(textDrawData,argEndX);
				imageRect.top 			= itemImageRect.top + mDrawData_HeaderHeight.Get(i) +
										lineIndex*infotextfontheight + kInfoTextTopMargin;//#643
				imageRect.bottom		= imageRect.top + infotextfontheight + kInfoTextTopMargin;//#643
				ALocalRect	localRect = {0};
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				//�`��
				NVMC_PaintRect(localRect,kColor_Yellow,0.3);
			}
			//�s�`��
			AImageRect	lineImageRect = itemImageRect;//#643
			lineImageRect.left		= itemImageRect.left + lineleftmargin;//#643
			lineImageRect.top 		= itemImageRect.top + mDrawData_HeaderHeight.Get(i) +
									lineIndex*infotextfontheight + kInfoTextTopMargin;//#643
			lineImageRect.bottom	= lineImageRect.top + infotextfontheight + kInfoTextTopMargin;//#643
			ALocalRect	lineLocalRect = {0};
			NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
			NVMC_DrawText(lineLocalRect,textDrawData);
			//
			//#147
			if( mScopeTypeArray.Get(i) != kScopeType_ModeKeyword )
			{
				lineleftmargin = kInfoTextLeftMargin2;
			}
		}
	}
}

//#643
/**
�eItemBox��imagerect�擾
*/
void	AView_IdInfo::GetItemBoxImageRect( const AIndex inIndex, AImageRect& outImageRect ) const
{
	outImageRect.left		= kItemBoxLeftMargin;
	outImageRect.top		= mDrawData_Y.Get(inIndex) + kItemBoxTopMargin;
	outImageRect.right		= NVI_GetViewWidth() - kItemBoxRightMargin;
	outImageRect.bottom		= outImageRect.top + mDrawData_Height.Get(inIndex);
}

//DrawData�v�Z
void	AView_IdInfo::CalcDrawData()
{
	//
	mDrawData_Y.DeleteAll();
	mDrawData_Height.DeleteAll();
	mDrawData_HeaderHeight.DeleteAll();
	mDrawData_InfoTextHeight.DeleteAll();
	mDrawData_InfoTextLineStart.DeleteAll();
	mDrawData_InfoTextLineLength.DeleteAll();
	mDrawData_CommentTextHeight.DeleteAll();
	mDrawData_CommentTextLineStart.DeleteAll();
	mDrawData_CommentTextLineLength.DeleteAll();
	
	//=========================�`��f�[�^�擾=========================
	//�t�H���g�擾
	//���e�����t�H���g
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//�w�b�_�����t�H���g
	AText	labelfontname;
	AFontWrapper::GetDialogDefaultFontName(labelfontname);
	//
	ANumber	viewwidth = NVI_GetViewWidth();
	
	//=========================���ږ����[�v=========================
	ANumber	y = 0;
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		mDrawData_Y.Add(y);
		mDrawData_InfoTextLineStart .AddNewObject();
		mDrawData_InfoTextLineLength.AddNewObject();
		mDrawData_CommentTextLineStart .AddNewObject();
		mDrawData_CommentTextLineLength.AddNewObject();
		
		//=========================�w�b�_�����v�Z=========================
		NVMC_SetDefaultTextProperty(labelfontname,fontsize,kColor_Black,kTextStyle_Bold,true);
		ANumber	headerfontheight = 9, headerfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(headerfontheight,headerfontascent);
		mDrawData_HeaderHeight.Add(kHeaderTopMargin+headerfontheight+kHeaderBottomMargin);
		
		//=========================InfoText�����v�Z=========================
		//InfoText�擾
		AText	infoText;
		mInfoTextArray.Get(i,infoText);
		//InfoText�s�܂�Ԃ��v�Z
		CWindowImp::CalcLineBreaks(infoText,fontname,fontsize-0.5,true,
								   viewwidth-kItemBoxLeftMargin-kItemBoxRightMargin -kInfoTextLeftMargin2 - kInfoTextRightMargin,
								   false,//#1113
								   mDrawData_InfoTextLineStart.GetObject(i),mDrawData_InfoTextLineLength.GetObject(i));
		//�t�H���g�����擾
		NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,kColor_Black,kTextStyle_Normal,true);
		ANumber	infotextfontheight = 9, infotextfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(infotextfontheight,infotextfontascent);
		//InfoText�S�̂̍����v�Z
		mDrawData_InfoTextHeight.Add(kInfoTextTopMargin+
					infotextfontheight*mDrawData_InfoTextLineStart.GetObject(i).GetItemCount() +
					kInfoTextBottomMargin);
		
		//=========================comment text���v�Z=========================
		AText	commentText;
		mCommentTextArray.Get(i,commentText);
		if( commentText.GetItemCount() > 0 )//#844 && GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetData_Bool(AModePrefDB::kIdInfoWindowDisplayComment) == true )
		{
			//�t�H���g�����擾
			NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,kColor_Black,kTextStyle_Normal,true);
			ANumber	commenttextfontheight = 9, commenttextfontascent = 7;
			NVMC_GetMetricsForDefaultTextProperty(commenttextfontheight,commenttextfontascent);
			//�s�܂�Ԃ��v�Z
			CWindowImp::CalcLineBreaks(commentText,fontname,fontsize-0.5,true,
									   viewwidth-kItemBoxLeftMargin-kItemBoxRightMargin -kCommentTextLeftMargin - kCommentTextRightMargin,
									   false,//#1113
									   mDrawData_CommentTextLineStart.GetObject(i),mDrawData_CommentTextLineLength.GetObject(i));
			//comment text�S�̂̍����v�Z
			mDrawData_CommentTextHeight.Add(kCommentTextTopMargin+commenttextfontheight*mDrawData_CommentTextLineStart.GetObject(i).GetItemCount()+kCommentTextBottomMargin);
		}
		else
		{
			//comment text�����Ȃ�Acomment text�̍�����0��ݒ�
			mDrawData_CommentTextHeight.Add(0);
		}
		
		//���ڑS�̂̍����ݒ�
		mDrawData_Height.Add(mDrawData_HeaderHeight.Get(i) + mDrawData_InfoTextHeight.Get(i) + mDrawData_CommentTextHeight.Get(i) + 2);
		//���̍��ڂ�y�ʒu���v�Z
		y += mDrawData_Height.Get(i);
		y += kItemBoxTopMargin + kItemBoxBottomMargin;//#643
	}
	//ImageSize�ݒ�
	NVM_SetImageSize(viewwidth,y);
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(0,kVScrollUnit,0,rect.bottom-rect.top-kVScrollUnit);
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
}

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_IdInfo::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	/*
	AImagePoint	imagept;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		if( imagept.y > mDrawData_Y.Get(i) && imagept.y < mDrawData_Y.Get(i) + mDrawData_Height.Get(i) )
		{
			SPI_OpenItem(i,inModifierKeys);//#853
			break;
		}
	}
	return false;
	*/
	//tracking�J�n���}�E�X�ʒu�L��
	mMouseTrackStartLocalPoint = inLocalPoint;
	mMouseDownResultIsDrag = false;
	
	//Mouse tracking�J�n
	NVM_SetMouseTrackingMode(true);
	return false;
}

/**
�}�E�X�{�^��������Tracking��OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_IdInfo::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	/*IdInfo���h���b�O������ʃE�C���h�E�ŊJ���悤�ɂ��悤�Ƃ������A�g�����肪���܂�悭���������Ȃ̂ł�߂�
	IdInfo��ʃr���[�Ƀh���b�O�����炻�̃r���[�ŊJ���@�\�͂��肩������Ȃ��̂ŁA������͕ʓr�����B
	//tracking�J�n���ƈႤ�}�E�X�ʒu�ɂȂ�����drag�J�n
	if( inLocalPoint.x != mMouseTrackStartLocalPoint.x || inLocalPoint.y != mMouseTrackStartLocalPoint.y )
	{
		//�h���b�O�J�n
		mMouseDownResultIsDrag = true;
		StartDrag(mMouseTrackStartLocalPoint,inModifierKeys);
		//TrackingMode����
		NVM_SetMouseTrackingMode(false);
	}
	NVI_Refresh();
	*/
}

/**
�}�E�X�{�^��������Tracking�I������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_IdInfo::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mMouseDownResultIsDrag == false )
	{
		//Drag�J�n���Ă��Ȃ��ꍇ�́A���ڂ��J��
		OpenItem(inLocalPoint,inModifierKeys);
		
		//Tracking����
		NVM_SetMouseTrackingMode(false);
		
		//�|�b�v�A�b�v�E�C���h�E�̏ꍇ��id info�E�C���h�E��hide
		if( GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()) == kSubWindowLocationType_Popup )
		{
			NVM_GetWindow().NVI_Hide();
		}
	}
}

/**
Drag�J�n
*/
/*IdInfo���h���b�O������ʃE�C���h�E�ŊJ���悤�ɂ��悤�Ƃ������A�g�����肪���܂�悭���������Ȃ̂ł�߂�
void	AView_IdInfo::StartDrag( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AIndex	itemIndex = FindItem(inLocalPoint);
	if( itemIndex == kIndex_Invalid )   return;
	//�t���[���擾
	ALocalRect	frame = {0};
	NVM_GetLocalViewRect(frame);
	//Scrap�ݒ�i�\��Index��n���j
	AArray<AScrapType>	scrapType;
	ATextArray	data;
	scrapType.Add(kScrapType_IdInfoItem);
	//Drag�f�[�^����
	AText	filepath;
	mFileArray.GetObjectConst(itemIndex).GetPath(filepath);
	AText	text;
	text.SetText(filepath);
	text.Add(kUChar_Tab);
	text.AddNumber(mStartIndexArray.Get(itemIndex));
	text.Add(kUChar_Tab);
	text.AddNumber(mEndIndexArray.Get(itemIndex));
	data.Add(text);
	//Drag�C���[�W�͈͎擾
	AImageRect	boxImageRect = {0};
	GetItemBoxImageRect(itemIndex,boxImageRect);
	ALocalRect	boxLocalRect = {0};
	NVM_GetLocalRectFromImageRect(boxImageRect,boxLocalRect);
	AWindowRect	boxWindowRect = {0};
	NVM_GetWindow().NVI_GetWindowRectFromControlLocalRect(NVI_GetControlID(),boxLocalRect,boxWindowRect);
	//Drag���s
	if( MVMC_DragTextWithWindowImage(inLocalPoint,scrapType,data,boxWindowRect,
					boxWindowRect.right-boxWindowRect.left,boxWindowRect.bottom-boxWindowRect.top) == false )
	{
		//Drop����Ȃ������Ƃ��́A�V�K�E�C���h�E�ō��ڊJ��
		
		AGlobalPoint	mouseGlobalPoint = {0};
		AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
		//
		OpenItem(inLocalPoint,inModifierKeys,true);
	}
}
*/

/**
Loacal point���獀�ڌ���
*/
AIndex	AView_IdInfo::FindItem( const ALocalPoint inLocalPoint )
{
	AImagePoint	imagept = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		if( imagept.y > mDrawData_Y.Get(i) && imagept.y < mDrawData_Y.Get(i) + mDrawData_Height.Get(i) )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

/**
���ڂ��J���iLocal point�w��j
*/
void	AView_IdInfo::OpenItem( const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	AIndex	index = FindItem(inLocalPoint);
	if( index != kIndex_Invalid )
	{
		SPI_OpenItem(index,inModifierKeys);
	}
}

//#853
/**
���ڂɕ\�����ꂽ�L�[���[�h�̒�`�ʒu���I�[�v������
*/
void	AView_IdInfo::SPI_OpenItem( const AIndex inIndex, const AModifierKeys inModifierKeys )
{
	if( inIndex == kIndex_Invalid )   return;
	//���݂̈ʒu��Navigate�o�^���� #146
	GetApp().SPI_RegisterNavigationPosition();
	//�t�@�C���擾
	AText	filepath;
	mFilePathArray.Get(inIndex,filepath);
	//���ڃf�[�^�擾
	AText	keywordText;
	mKeywordTextArray.Get(inIndex,keywordText);
	AText	parentKeywordText;
	mParentKeywordArray.Get(inIndex,parentKeywordText);
	AIndex	spos = 0, epos = 0;
	spos = mStartIndexArray.Get(inIndex);
	epos = mEndIndexArray.Get(inIndex);
	//Call Graf�֒ʒm
	GetApp().SPI_NotifyToCallGrafByIdInfoJump(mCallerFuncIdText,filepath,parentKeywordText,keywordText,spos,epos);
	//���ڂ��J��
	GetApp().SPI_OpenOrRevealTextDocument(filepath,spos,epos,kObjectID_Invalid,
				(AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true),
				kAdjustScrollType_Top);
	/*
	//
	AObjectID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,file);
	if( docID == kObjectID_Invalid )
	{
		docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file,GetEmptyText(),true,spos,epos,false);
	}
	if( docID != kObjectID_Invalid )
	{
		//#856 test GetApp().SPI_CreateNewWindowForExistingTextDocument(docID,spos,epos);
		AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
		if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
		{
			AIndex	tabIndex = GetApp().SPI_GetTextWindowByID(winID).NVI_GetTabIndexByDocumentID(docID);
			//�T�u�y�C���J�����\�����Ȃ�T�u�y�C�����ɕ\������ #212
			if( GetApp().SPI_GetTextWindowByID(winID).SPI_IsLeftSideBarDisplayed() == true &&
						AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == false )
			{
				//�T�u�y�C�����ɂ��̃h�L�������g��\��
				GetApp().SPI_GetTextWindowByID(winID).SPI_DisplayInSubText(tabIndex,false,kIndex_Invalid,true);
			}
			else
			{
				//�^�u�I�����ăr���[�����i���̃r���[�Ƀt�H�[�J�X���ڂ�j
				GetApp().SPI_GetTextWindowByID(winID).NVI_SelectTab(tabIndex);
				GetApp().SPI_GetTextWindowByID(winID).SPI_SplitView(false);
			}
		}
		//�I��
		GetApp().SPI_GetTextDocumentByID(docID).SPI_SelectText(spos,epos-spos);
		//
		GetApp().SPI_NotifyToCallGrafByIdInfoJump(mCallerFuncIdText,filepath,parentKeywordText,keywordText,spos,epos);
	}
	*/
	/*if( mScopeTypeArray.Get(i) != kScopeType_Import )
	{
	ADocumentID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,mFileArray.GetObjectConst(i));
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true )
	{
	GetApp().SPI_GetTextWindowByID(winID).SPI_SplitView(false);
	}
	GetApp().SPI_GetTextWindowByID(winID).SPI_SetSelect(docID,spos,epos);
	GetApp().SPI_GetTextWindowByID(winID).SPI_AdjustScroll_Center(docID);
	GetApp().SPI_GetTextWindowByID(winID).SPI_SelectText();
	}
	else
	{
	GetApp().SPI_SelectTextDocument(mFileArray.GetObjectConst(i),spos,epos-spos);
	}*/
}

//
void	AView_IdInfo::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
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
	//Hover�X�V
	EVTDO_DoMouseMoved(inLocalPoint,inModifierKeys);
}

ABool	AView_IdInfo::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	return false;
}

//�J�[�\��
ABool	AView_IdInfo::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�z�o�[�ݒ�
	AImagePoint	imagept;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		if( imagept.y > mDrawData_Y.Get(i) && imagept.y < mDrawData_Y.Get(i) + mDrawData_Height.Get(i) )
		{
			if( mCurrentHoverCursorIndex != i )
			{
				mCurrentHoverCursorIndex = i;
				NVI_Refresh();
			}
			return true;
		}
	}
	ClearHoverCursor();
	return true;
}

void	AView_IdInfo::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearHoverCursor();
}

void	AView_IdInfo::ClearHoverCursor()
{
	if( mCurrentHoverCursorIndex != kIndex_Invalid )
	{
		mCurrentHoverCursorIndex = kIndex_Invalid;
		NVI_Refresh();
	}
}

//#442
/**
�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
*/
ABool	AView_IdInfo::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	//#688 AApplication::GetApplication().NVI_GetMenuManager().
	//#688 ShowContextMenu(mContextMenuItemID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
	NVMC_ShowContextMenu(mContextMenuItemID,globalPoint);//#688
	return true;
}

//#507
/**
�w���v�^�O
*/
ABool	AView_IdInfo::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	//#688 NVM_GetWindowConst().NVI_HideHelpTag();//#643
	outTagSide = kHelpTagSide_Right;//#643
	
	ANumber	viewwidth = NVI_GetViewWidth();
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		//
		AImageRect	itemImageRect;
		ANumber	y = mDrawData_Y.Get(i);
		itemImageRect.left = 0;
		itemImageRect.right = viewwidth;
		itemImageRect.top = y;
		itemImageRect.bottom = y + mDrawData_Height.Get(i);
		ALocalRect	itemLocalRect;
		NVM_GetLocalRectFromImageRect(itemImageRect,itemLocalRect);
		//
		if( inPoint.x >= itemLocalRect.left && inPoint.x <= itemLocalRect.right &&
					inPoint.y >= itemLocalRect.top && inPoint.y <= itemLocalRect.bottom )
		{
			AText	text;
			AText	path;
			mFilePathArray.Get(i,path);
			//�J�e�S���擾
			AIndex	categoryIndex = mCategoryIndexArray.Get(i);
			//���[�h�ݒ�X�V�ƃX���b�h���s�Ƃ̂���Ⴂ�̏ꍇ�ɃJ�e�S��index���I�[�o�[����\��������̂ŁA�O�̂��߃J�e�S��index�l�`�F�b�N�B
			if( categoryIndex < 0 || categoryIndex >= GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryCount() )
			{
				categoryIndex = 0;
			}
			//�J�e�S�����擾
			AText	categoryname;
			GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryName(categoryIndex,categoryname);
			//�w���v�^�O�e�L�X�g�ݒ�
			text.SetLocalizedText("IdInfoHelpTag");
			text.ReplaceParamText('0',mKeywordTextArray.GetTextConst(i));//���ʎq
			text.ReplaceParamText('1',path);//�t�@�C��
			text.ReplaceParamText('2',categoryname);//�J�e�S���[
			text.ReplaceParamText('3',mParentKeywordArray.GetTextConst(i));//�e�L�[���[�h
			text.ReplaceParamText('4',mRawCommentTextArray.GetTextConst(i));//�R�����g
			text.ReplaceParamText('5',mInfoTextArray.GetTextConst(i));//���e�L�X�g
			text.ReplaceParamText('6',mTypeTextArray.GetTextConst(i));
			switch(mScopeTypeArray.Get(i))
			{
			  case kScopeType_Global:
				{
					text.ReplaceParamText('7',"(Global)");
					break;
				}
			  case kScopeType_Import:
				{
					text.ReplaceParamText('7',"(Import)");
					break;
				}
			  case kScopeType_ModeKeyword:
				{
					text.ReplaceParamText('7',"(Keyword)");
					break;
				}
			  default:
				{
					text.ReplaceParamText('7',"");
					break;
				}
			}
			outText1.AddText(text);
			outText2.AddText(text);
			outRect = itemLocalRect;
			return true;
		}
	}
	return false;
}

/**
�J�[�\���^�C�v�擾
*/
ACursorType	AView_IdInfo::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mCurrentHoverCursorIndex != kIndex_Invalid )
	{
		return kCursorType_PointingHand;
	}
	return kCursorType_Arrow;
}
#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================


const AItemCount kHistoryMax	= 256;

#if 0
void	AView_IdInfo::SPI_SetIdInfo( const AText& inIdText, const ADocumentID inTextDocumentID )
/*#238
		const AModeIndex inModeIndex, 
		const ATextArray& inInfoTextArray, const AObjectArray<AFileAcc>& inFileArray,
		const ATextArray& inCommentTextArray, const AArray<AIndex>& inCategoryIndexArray,
		const AArray<AScopeType>& inScopeTypeArray, const AArray<AIndex>& inStartIndexArray, const AArray<AIndex>& inEndIndexArray,
		const ATextArray& inParentKeywordArray )
*/
{
	if( mFixDisplay == true )   return;//#238
	
	//#238
	//Document���݃`�F�b�N
	if( GetApp().NVI_IsDocumentValid(inTextDocumentID) == false )   return;
	if( GetApp().NVI_GetDocumentTypeByID(inTextDocumentID) != kDocumentType_Text )   return;
	
	//#238 mCurrentModeIndex = inModeIndex;
	mInfoTextArray.DeleteAll();
	mCommentTextArray.DeleteAll();
	mFileArray.DeleteAll();
	mCategoryIndexArray.DeleteAll();
	mScopeTypeArray.DeleteAll();
	mStartIndexArray.DeleteAll();
	mEndIndexArray.DeleteAll();
	mParentKeywordArray.DeleteAll();
	mIdText.SetText(inIdText);
	//#238
	mCurrentModeIndex = GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetModeIndex();
	GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetIdInfoArray(mIdText,
			mInfoTextArray,mFileArray,mCommentTextArray,mCategoryIndexArray,mScopeTypeArray,mStartIndexArray,mEndIndexArray,mParentKeywordArray);
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		if( mInfoTextArray.GetTextConst(i).GetItemCount() == 0 )
		{
			mInfoTextArray.Set(i,mIdText);
		}
	}
	/*#238
	//
	for( AIndex i = 0; i < inInfoTextArray.GetItemCount(); i++ )
	{
		AText	infoText;
		infoText.SetText(inInfoTextArray.GetTextConst(i));
		if( infoText.GetItemCount() == 0 )
		{
			infoText.SetText(inIdText);
		}
		mInfoTextArray.Add(infoText);
		//
		AText	commentText;
		commentText.SetText(inCommentTextArray.GetTextConst(i));
		mCommentTextArray.Add(commentText);
		//
		mFileArray.GetObject(mFileArray.AddNewObject()).CopyFrom(inFileArray.GetObjectConst(i));
		//
		mCategoryIndexArray.Add(inCategoryIndexArray.Get(i));
		//
		mScopeTypeArray.Add(inScopeTypeArray.Get(i));
		//
		mStartIndexArray.Add(inStartIndexArray.Get(i));
		mEndIndexArray.Add(inEndIndexArray.Get(i));
		//
		mParentKeywordArray.Add(inParentKeywordArray.GetTextConst(i));
	}
	*/
	mArgIndex = kIndex_Invalid;
	mCurrentHoverCursorIndex = kIndex_Invalid;
	CalcDrawData();
	NVI_Refresh();
	
	//#238 ����ǉ�
	if( mInfoTextArray.GetItemCount() > 0 )
	{
		mIdTextHistory.Insert1(0,inIdText);
		AFileAcc	file;
		GetApp().NVI_GetDocumentByID(inTextDocumentID).NVI_GetFile(file);
		AText	path;
		file.GetPath(path);
		path.Add(kUChar_LineEnd);
		path.AddText(inIdText);
		mPathHistory.Insert1(0,path);
		//�������ڂ���������폜
		for( AIndex i = 1; i < mPathHistory.GetItemCount(); i++ )
		{
			if( mPathHistory.GetTextConst(i).Compare(path) == true )
			{
				mIdTextHistory.Delete1(i);
				mPathHistory.Delete1(i);
				break;
			}
		}
		//����max��葽����Έ�ԍŌ�̂��폜
		if( mIdTextHistory.GetItemCount() >= kHistoryMax )
		{
			mIdTextHistory.Delete1(mIdTextHistory.GetItemCount()-1);
			mPathHistory.Delete1(mPathHistory.GetItemCount()-1);
		}
		//IdInfoHistory���j���[�X�V
		GetApp().NVI_GetMenuManager().SetContextMenuTextArray(kContextMenuID_IdInfoHistory,mIdTextHistory,mPathHistory);
	}
	
	//#238 �����{�^���X�V
	GetApp().SPI_GetIdInfoWindow().SPI_UpdateHistoryButton();
}
#endif

/**
�f�[�^�ݒ�
@note note1
@warning warning1
@note note2
@caution caution3
*/
void	AView_IdInfo::SPI_SetIdInfo( const AText& inIdText, const ATextArray& inParentKeyword,
		const ADocumentID inTextDocumentID, 
		const ATextArray& inKeywordTextArray, const ATextArray& inParentKeywordTextArray,
		const ATextArray& inTypeTextArray, const ATextArray& inInfoTextArray, const ATextArray& inCommentTextArray, 
		const AArray<AIndex>& inCategoryIndexArray, const AArray<AIndex>& inColorSlotIndexArray,
		const AArray<AScopeType>& inScopeArray,
		const AArray<AIndex>& inStartIndexArray, const AArray<AIndex>& inEndIndexArray,
		const ATextArray& inFilePathArray, const AIndex inArgIndex, const AText& inCallerFuncIdText )
{
	//���b�N���Ȃ牽�����Ȃ�
	if( mFixDisplay == true )   return;//#238
	
	//���s�����݂̂Ȃ牽�����Ȃ� #0 �ΏǗÖ@�����AOriginalTextArray_UnicodeOffset��index�G���[������������̑΍�
	if( inIdText.Compare("\r") == true )   return;
	
	//#238
	//Document���݃`�F�b�N
	if( GetApp().NVI_IsDocumentValid(inTextDocumentID) == false )   return;
	if( GetApp().NVI_GetDocumentTypeByID(inTextDocumentID) != kDocumentType_Text )   return;
	
	//==================���ݕ\������id text�Ɠ����Ȃ�id info�X�V���Ȃ�==================
	if( inIdText.Compare(mIdText) == true )
	{
		if( mArgIndex != inArgIndex )
		{
			//------------------�L�[���[�h�͓����ň���index�݈̂قȂ�ꍇ------------------
			//����index�X�V
			mArgIndex = inArgIndex;
			//hide�J�E���^�ݒ�
			ResetHideCounter(kTimerForHidePopupIdInfo_AfterArgIndexChanged);
		}
		else
		{
			//hide�J�E���^�ݒ�
			ResetHideCounter(kTimerForHidePopupIdInfo);
		}
		NVI_Refresh();
		return;
	}
	
	//==================�e��p�����[�^�ݒ�==================
	
	//���݈����X�V
	mArgIndex = inArgIndex;
	//����hide��enable
	mDisableAutoHide = false;
	//hide�J�E���^�ݒ�
	ResetHideCounter(kTimerForHidePopupIdInfo);
	
	//==================�f�[�^�ݒ�==================
	mIdText.SetText(inIdText);
	mRequestedParentKeyword.SetFromTextArray(inParentKeyword);
	mKeywordTextArray.SetFromTextArray(inKeywordTextArray);
	mParentKeywordArray.SetFromTextArray(inParentKeywordTextArray);
	mTypeTextArray.SetFromTextArray(inTypeTextArray);
	mInfoTextArray.SetFromTextArray(inInfoTextArray);
	mCommentTextArray.SetFromTextArray(inCommentTextArray);
	mRawCommentTextArray.SetFromTextArray(inCommentTextArray);
	mCategoryIndexArray.SetFromObject(inCategoryIndexArray);
	mColorSlotIndexArray.SetFromObject(inColorSlotIndexArray);
	mScopeTypeArray.SetFromObject(inScopeArray);
	mStartIndexArray.SetFromObject(inStartIndexArray);
	mEndIndexArray.SetFromObject(inEndIndexArray);
	mFilePathArray.SetFromTextArray(inFilePathArray);
	//FuncId�L��
	mCallerFuncIdText.SetText(inCallerFuncIdText);
	//���[�hindex�擾
	mCurrentModeIndex = GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetModeIndex();
	//infotext�̕␳
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		//infotext�擾
		AText	infoText;
		mInfoTextArray.Get(i,infoText);
		//infotext����̏ꍇ�̓L�[���[�h��ݒ肵�Ă���
		if( infoText.GetItemCount() == 0 )
		{
			mInfoTextArray.Set(i,inIdText);
		}
		//infotext�̃T�C�Y�������l�ȏ�̂Ƃ��́A�����l�ȍ~���폜����
		if( infoText.GetItemCount() > kLimit_IdInfo_InfoTextLength )
		{
			AIndex	p = infoText.GetCurrentCharPos(kLimit_IdInfo_InfoTextLength);
			infoText.DeleteAfter(p);
			mInfoTextArray.Set(i,infoText);
		}
	}
	//#835 @note��
	//���X��comment text����A@note���̍s�𔲂��o���āAcomment text�ɍĐݒ�
	//�ݒ�f�[�^�擾�Atext array�ɓW�J
	AText	notePrefText;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kIdInfoAtNoteText,notePrefText);
	ATextArray	noteTextArray;
	noteTextArray.ExplodeFromText(notePrefText,kUChar_LineEnd);
	AItemCount	noteTextArrayItemCount = noteTextArray.GetItemCount();
	//�eIdInfo���ږ��̃��[�v
	for( AIndex i = 0; i < mCommentTextArray.GetItemCount(); i++ )
	{
		//���ڂ̌��X��comment text�擾
		AText	originalCommentText;
		mCommentTextArray.Get(i,originalCommentText);
		//comment text
		AText	commentText;
		//�e@note�ݒ�text array���̃��[�v
		for( AIndex j = 0; j < noteTextArrayItemCount; j++ )
		{
			//note text�擾
			AText	noteText;
			noteTextArray.Get(j,noteText);
			//comment text������@note�ȍ~������
			AItemCount	len = originalCommentText.GetItemCount();
			for( AIndex pos = 0; pos < len; )
			{
				if( originalCommentText.FindText(pos,noteText,pos) == true )
				{
					//@note����s���܂ł��擾
					AText	t;
					originalCommentText.GetLine(pos,t);
					commentText.AddText(t);
					commentText.Add(kUChar_LineEnd);
				}
				else
				{
					break;
				}
			}
		}
		//�����o����������commentText�Ƃ��Đݒ�
		mCommentTextArray.Set(i,commentText);
	}
	//���݈����ʒu����������
	mCurrentHoverCursorIndex = kIndex_Invalid;
	mCurrentSelectionIndex = kIndex_Invalid;
	//�`��f�[�^�X�V
	CalcDrawData();
	NVI_Refresh();
}

//#725
/**
�S�f�[�^�폜
*/
void	AView_IdInfo::SPI_DeleteAllIdInfo()
{
	//id info�����X���b�h�̌��݂̗v���̒��f
	GetApp().SPI_GetIdInfoFinderThread().AbortCurrentRequest();
	
	//�e�f�[�^�폜
	mKeywordTextArray.DeleteAll();
	mTypeTextArray.DeleteAll();
	mInfoTextArray.DeleteAll();
	mCommentTextArray.DeleteAll();
	mRawCommentTextArray.DeleteAll();
	mFilePathArray.DeleteAll();
	mCategoryIndexArray.DeleteAll();
	mColorSlotIndexArray.DeleteAll();
	mScopeTypeArray.DeleteAll();
	mStartIndexArray.DeleteAll();
	mEndIndexArray.DeleteAll();
	mParentKeywordArray.DeleteAll();
	mCallerFuncIdText.DeleteAll();
	mArgIndex = kIndex_Invalid;
	mCurrentModeIndex = kIndex_Invalid;
	mIdText.DeleteAll();
	mCurrentHoverCursorIndex = kIndex_Invalid;
	mCurrentSelectionIndex = kIndex_Invalid;
	mMouseTrackStartLocalPoint = kLocalPoint_00;
	mMouseDownResultIsDrag = false;
	mFixDisplay = false;
	//�`��f�[�^�X�V
	CalcDrawData();
}

//#853
/**
���ڂ̍ő卂�����擾
*/
ANumber	AView_IdInfo::SPI_GetMaxItemHeight() const
{
	//�ő卂��
	ANumber	maxHeight = 0;
	//�e���ږ��̃��[�v
	AItemCount	itemCount = mDrawData_Height.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		if( maxHeight < mDrawData_Height.Get(i) )
		{
			maxHeight = mDrawData_Height.Get(i);
		}
	}
	return maxHeight + kItemBoxTopMargin + kItemBoxBottomMargin;
}

/**
�I���ʒu�ݒ�
*/
void	AView_IdInfo::SPI_SetSelect( const AIndex inIndex )
{
	if( inIndex >= 0 && inIndex < mKeywordTextArray.GetItemCount() )
	{
		//�I��ݒ�
		mCurrentSelectionIndex = inIndex;
		//�`��X�V
		NVI_Refresh();
	}
}

/**
����I��
*/
void	AView_IdInfo::SPI_SelectNext()
{
	//���ږ����Ȃ烊�^�[��
	if( mKeywordTextArray.GetItemCount() == 0 )   return;
	//���ݖ��I���Ȃ�ŏ���I��
	if( mCurrentSelectionIndex == kIndex_Invalid )
	{
		SPI_SetSelect(0);
	}
	//����I��
	else
	{
		if( mCurrentSelectionIndex+1 < mKeywordTextArray.GetItemCount() )
		{
			mCurrentSelectionIndex++;
		}
		/*
		else
		{
			mCurrentSelectionIndex = 0;
		}
		*/
	}
	//�X�N���[������
	AdjustScroll();
	//�`��X�V
	NVI_Refresh();
}

/**
�O��I��
*/
void	AView_IdInfo::SPI_SelectPrev()
{
	//���ږ����Ȃ烊�^�[��
	if( mKeywordTextArray.GetItemCount() == 0 )   return;
	//���ݖ��I���Ȃ�Ō��I��
	if( mCurrentSelectionIndex == kIndex_Invalid )
	{
		SPI_SetSelect(mKeywordTextArray.GetItemCount()-1);
	}
	//�O��I��
	else
	{
		if( mCurrentSelectionIndex-1 >= 0 )
		{
			mCurrentSelectionIndex--;
		}
		/*
		else
		{
			mCurrentSelectionIndex = mKeywordTextArray.GetItemCount()-1;
		}
		*/
	}
	//�X�N���[������
	AdjustScroll();
	//�`��X�V
	NVI_Refresh();
}

/**
�X�N���[������
*/
void	AView_IdInfo::AdjustScroll()
{
	//�I�𖳂��Ȃ�ŏ��ɃX�N���[��
	if( mCurrentSelectionIndex == kIndex_Invalid )
	{
		AImagePoint	pt = {0,0};
		NVI_ScrollTo(pt);
		return;
	}
	//�I�����ڂ�rect���擾
	AImageRect	itemImageRect = {0};
	GetItemBoxImageRect(mCurrentSelectionIndex,itemImageRect);
	AImageRect	frameImageRect = {0};
	NVM_GetImageViewRect(frameImageRect);
	//�I�����ڂ���ʊO�Ȃ�A�I�����ڂ������ɗ���悤�ɃX�N���[��
	if( (itemImageRect.top-kItemBoxTopMargin) < frameImageRect.top || 
		(itemImageRect.bottom+kItemBoxBottomMargin) > frameImageRect.bottom )
	{
		AImagePoint	pt = {0,0};
		pt.x = 0;
		pt.y = ((itemImageRect.top-kItemBoxTopMargin)+(itemImageRect.bottom+kItemBoxBottomMargin))/2 
				- (itemImageRect.bottom-itemImageRect.top)/2;
		NVI_ScrollTo(pt);
	}
}

//#853
/**
�n�C���C�g����index�ݒ�i�e�L�X�g�r���[�ň����ʒu�Ń}�E�X�z�o�[�����ꍇ�ȂǂɃR�[�������j
*/
ABool	AView_IdInfo::SPI_HighlightIdInfoArgIndex( const AText& inKeyword, const AIndex inArgIndex )
{
	//hide�J�E���^�ݒ�
	//#1340 �����ʒu�ȊO�ł��}�E�X�𓮂����Ƃ����ɗ���̂ł����Ń^�C�}�[���Z�b�g���ׂ��łȂ� ResetHideCounter(kTimerForHidePopupIdInfo_AfterArgIndexChanged);
	//
	if( mIdText.Compare(inKeyword) == true )
	{
		//�����ʒu����������Ƃ��ɁAhide�J�E���^�[�����Z�b�g���� #1340
		if( mArgIndex != inArgIndex )
		{
			ResetHideCounter(kTimerForHidePopupIdInfo_AfterArgIndexChanged);
		}
		//
		mArgIndex = inArgIndex;
		NVI_Refresh();
		return true;
	}
	else
	{
		return false;
	}
}

/**
*/
void	AView_IdInfo::SPI_UpdateDisplayComment()
{
	CalcDrawData();
	NVI_Refresh();
}

/**
*/
void	AView_IdInfo::SPI_UpdateDrawProperty()
{
	CalcDrawData();
	NVI_Refresh();
}

/**
����hide��disable��Ԃɂ���i�|�b�v�A�b�v�E�C���h�E�}�E�X�z�o�[���j
*/
void	AView_IdInfo::SPI_SetDisableAutoHide()
{
	//disable�t���OON
	mDisableAutoHide = true;
	//�^�C�}�[��max�ݒ�
	mHideCounter = kNumber_MaxNumber;
	//���l�N���A
	mAlpha = 1.0;
	//�`��X�V
	NVI_Refresh();
}

/**
�^�C�}�[�ݒ�
*/
void	AView_IdInfo::ResetHideCounter( const ANumber inCounter )
{
	//�|�b�v�A�b�v�E�C���h�E�łȂ��ꍇ�̓^�C�}�[��max�ݒ�
	if( GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()) != kSubWindowLocationType_Popup )
	{
		mHideCounter = kNumber_MaxNumber;
	}
	//���ݒ�Ŏ���hide enable�A���A����hide disable��ԂłȂ���΃^�C�}�[�ݒ�
	else if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kHideIdInfoPopupByIdle) == true && mDisableAutoHide == false )
	{
		mHideCounter = inCounter;
	}
	//����hide disable�Ȃ�^�C�}�[��max�ݒ�
	else
	{
		mHideCounter = kNumber_MaxNumber;
	}
	
	//���l�N���A
	mAlpha = 1.0;
}

/**
tick�^�C�}�[������
*/
void	AView_IdInfo::SPI_DoTickTimer()
{
	//�|�b�v�A�b�v�E�C���h�E�ȊO�͉������Ȃ�
	if( GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()) != kSubWindowLocationType_Popup )
	{
		return;
	}
	//�^�C�}�[max���͉������Ȃ�
	if( mHideCounter == kNumber_MaxNumber )
	{
		return;
	}
	
	//�^�C�}�[�J�E���^�f�N�������g
	mHideCounter--;
	//�^�C�}�[�J�E���^��16�ȉ��Ȃ烿�l����
	if( mHideCounter <= 10 )//#1354 16��10
	{
		mAlpha = 0.9*mHideCounter/10;//#1354 16��10
		NVI_Refresh();
		NVM_GetWindow().NVI_UpdateWindow();
	}
	//�^�C�}�[�J�E���^��0�ɂȂ�����E�C���h�Ehide
	if( mHideCounter <= 0 )
	{
		NVM_GetWindow().NVI_Hide();
	}
}


