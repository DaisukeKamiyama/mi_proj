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

AView_WordsList
#723

*/

#include "stdafx.h"

#include "AView_WordsList.h"
#include "AApp.h"

//�`��p�f�[�^
//���ړ��e�����̃}�[�W��
const ANumber	kHeaderLeftMargin = 6;
const ANumber	kHeaderRightMargin = 6;
const ANumber	kHeaderTopMargin = 2;
const ANumber	kHeaderBottomMargin = 2;
const ANumber	kInfoTextLeftMargin_Header = 8;
const ANumber	kInfoTextLeftMargin = 12;
const ANumber	kInfoTextRightMargin = 3;
const ANumber	kInfoTextTopMargin = 2;
const ANumber	kInfoTextBottomMargin = 1;
const ANumber	kCommentTextLeftMargin = 8;
const ANumber	kCommentTextRightMargin = 3;
const ANumber	kCommentTextTopMargin = 2;
const ANumber	kCommentTextBottomMargin = 1;
//�T�}���[�\�����̃}�[�W��
const ANumber	kSummaryLeftMargin = 8;
const ANumber	kSummaryTopMargin = 4;
const ANumber	kSummaryHeight = 100;
//item box�̃}�[�W��
const ANumber	kItemBoxLeftMargin = 5;
const ANumber	kItemBoxRightMargin = 5;
const ANumber	kItemBoxTopMargin = 2;
const ANumber	kItemBoxBottomMargin = 1;

const ANumber	kVScrollUnit = 8;

#pragma mark ===========================
#pragma mark [�N���X]AView_WordsList
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AView_WordsList::AView_WordsList( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mAllFilesSearched(false), mWordsListFinderThreadID(kObjectID_Invalid)
,mCurrentHoverCursorIndex(kIndex_Invalid),mCurrentSelectionIndex(kIndex_Invalid), mMouseTrackStartLocalPoint(kLocalPoint_00)
,mModeIndex(kIndex_Invalid),mIsFinding(false),mLockedMode(false), mDrawDataAllCalculated(false), mSearchInProjectFolder(false)
,mLastRequest_ModeIndex(kIndex_Invalid),mMouseClickCount(0)
{
	NVMC_SetOffscreenMode(true);
	
	//���[�h���X�g�����X���b�h�����A�N��
	AThread_WordsListFinderFactory	factory(this);
	mWordsListFinderThreadID = GetApp().NVI_CreateThread(factory);
	GetWordsListFinderThread().SetWordsListFinderType(kWordsListFinderType_ForWordsList);
	GetWordsListFinderThread().NVI_Run(true);
	
	//�S�f�[�^�폜
	SPI_DeleteAllData();
}

/**
�f�X�g���N�^
*/
AView_WordsList::~AView_WordsList()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_WordsList::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_WordsList::NVIDO_DoDeleted()
{
	//���[�h���X�g�����X���b�h�I���A�폜
	if( mWordsListFinderThreadID != kObjectID_Invalid )
	{
		GetApp().NVI_DeleteThread(mWordsListFinderThreadID);
	}
}

/**
���[�h���X�g�����X���b�h�擾
*/
AThread_WordsListFinder&	AView_WordsList::GetWordsListFinderThread()
{
	return (dynamic_cast<AThread_WordsListFinder&>(GetApp().NVI_GetThreadByID(mWordsListFinderThreadID)));
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�`��v�����̃R�[���o�b�N(AView�p)
*/
void	AView_WordsList::EVTDO_DoDraw()
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	
	//==================�F�擾==================
	
	//�`��F�ݒ�
	AColor	letterColor = kColor_Black;
	AColor	dropShadowColor = kColor_White;
	AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
	GetApp().SPI_GetSubWindowBoxColor(NVM_GetWindow().GetObjectID(),letterColor,dropShadowColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
	NVMC_SetDropShadowColor(dropShadowColor);
	
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
	  case kSubWindowLocationType_Floating:
		{
			//�t���[�e�B���O�̎w�蓧�ߗ��Ŕw�i����
			backgroundAlpha = GetApp().SPI_GetFloatingWindowAlpha();
			//NVMC_PaintRect(frameRect,backgroundColor,backgroundAlpha);
			break;
		}
	  default:
		{
			//�s���߂Ŕw�i����
			//NVMC_PaintRect(frameRect,backgroundColor);
			break;
		}
	} 
	
	//=========================�e���ږ����[�v=========================
	for( AIndex i = 0; i < mContentTextArray.GetItemCount(); i++ )
	{
		//����rect�擾
		AImageRect	itemImageRect = {0};
		GetItemBoxImageRect(i,itemImageRect);//#643
		ALocalRect	itemLocalRect = {0};
		NVM_GetLocalRectFromImageRect(itemImageRect,itemLocalRect);
		
		//updateRect���ɂȂ��ꍇ��continue
		if( NVMC_IsRectInDrawUpdateRegion(itemLocalRect) == false )   continue;
		
		//index:32�ȍ~�̍��ڂ́A�������̂��߂ɁASPI_DoWordsListFinderPaused()�̒i�K�ł�CalcDrawData()���Ȃ��̂ŁA
		//�v�Z�ς݃t���OmDrawDataAllCalculated��false�̂Ƃ��́A�����Ōv�Z����B
		if( i >= 32 && mDrawDataAllCalculated == false )
		{
			//index:32�ȍ~�̍��ڂ̍s�v�Z
			CalcDrawData(true);
			//����rect�̍Ď擾
			GetItemBoxImageRect(i,itemImageRect);
			NVM_GetLocalRectFromImageRect(itemImageRect,itemLocalRect);
		}
		
		//�Ō�̍��ڈȊO�́A���Ԃ𔒁i��:0.2�j�ŕ`��i�|�b�v�A�b�v�E�C���h�E�̏ꍇ�ɁA�z�C�[���X�N���[�������Ԃł��L���ɂ��邽�߁B�������ƃz�C�[�����������Ȃ��B�j����A���[�h���X�g�Ƀ|�b�v�A�b�v�͂Ȃ����A�����Ή����邩������Ȃ��̂Ŏc���B
		if( i != mContentTextArray.GetItemCount() -1 )
		{
			ALocalRect	gapLocalRect = {0};
			gapLocalRect.left		= itemLocalRect.left + 5;
			gapLocalRect.top		= itemLocalRect.bottom;
			gapLocalRect.right		= itemLocalRect.right - 5;
			gapLocalRect.bottom		= itemLocalRect.bottom + kItemBoxTopMargin + kItemBoxBottomMargin;
			NVMC_PaintRect(gapLocalRect,boxBaseColor2,0.2);
		}
		
		//�{�b�N�X�`��#643
		//content�����F�擾
		AColor	contentColor_start = kColor_White;
		AColor	contentColor_end = kColor_Gray95Percent;
		/*�I�����ڂ͑I��F�ň͂ނ��Ƃɂ���
		if( mCurrentSelectionIndex == i )
		{
			//���݃A�N�e�B�u�ȃT�u�E�C���h�E���ǂ����icmd+option+�����Ώۂ��j�ɂ���āA�F��ς���
			if( NVM_GetWindow().GetObjectID() == GetApp().SPI_GetActiveSubWindowForItemSelector() )
			{
				//�A�N�e�B�u�ȃT�u�E�C���h�E
				AColorWrapper::GetHighlightColor(contentColor_end);
			}
			else
			{
				//��A�N�e�B�u�ȃT�u�E�C���h�E
				contentColor_start = kColor_Gray90Percent;
				contentColor_end = kColor_Gray90Percent;
			}
		}
		*/
		//�{�b�N�X�`��
		NVMC_PaintRoundedRect(itemLocalRect,
							  //contentColor_start,contentColor_end,
							  boxBaseColor1,boxBaseColor3,
							  kGradientType_Vertical,backgroundAlpha,backgroundAlpha,
							  5,true,true,true,true);
		//==================�t���[���\���i�I�����ڂ̏ꍇ�A�I��F�ň͂ݕ\���j==================
		if( mCurrentSelectionIndex == i )
		{
			//�I��F�ň͂ݕ\��
			AColor	boxFrameColor = GetApp().SPI_GetSubWindowRoundedRectBoxSelectionColor(NVM_GetWindow().GetObjectID());
			NVMC_FrameRoundedRect(itemLocalRect,boxFrameColor,1.0,5,true,true,true,true,true,true,true,true,1.0);
		}
		else
		{
			//�ʏ�F�͂�
			NVMC_FrameRoundedRect(itemLocalRect,kColor_Gray50Percent,1.0,5,true,true,true,true);
		}
		
		//�z�o�[�`��
		if( mCurrentHoverCursorIndex == i )
		{
			/*
			AColor	selcolor;
			AColorWrapper::GetHighlightColor(selcolor);
			NVMC_PaintRect(itemLocalRect,selcolor,0.1);
			NVMC_FrameRect(itemLocalRect,selcolor,1.0);
			*/
			//�z�o�[�F�ň͂ݕ\��
			AColor	hoverColor = GetApp().SPI_GetSubWindowRoundedRectBoxHoverColor();
			NVMC_FrameRoundedRect(itemLocalRect,hoverColor,0.3,5,true,true,true,true,true,true,true,true,1.0);
		}
		
		//=========================�w�b�_�����`��=========================
		
		//�w�b�_�����F
		AColor	headerLetterColor = letterColor;
		//�w�b�_�F�擾
		AColor	headercolor = kColor_Blue;//#1316 ���@�\��������Ȃ�_�[�N���[�h�Ή��K�v GetApp().SPI_GetSubWindowsBoxHeaderColor();;//�F�Œ�i�J�e�S���F�ɂ���̂́A�w�i���n�̐F�ȂǂŐF�ʐ݌v������̂Łj mHeaderColorArray.Get(i);
		if( mClickedArray.Get(i) == true )
		{
			headercolor = kColor_Gray;
		}
		//�w�b�_rect�擾
		AImageRect	headerImageRect = itemImageRect;//#643
		headerImageRect.bottom = itemImageRect.top + mDrawData_HeaderHeight.Get(i);//#643
		ALocalRect	headerLocalRect = {0};
		NVM_GetLocalRectFromImageRect(headerImageRect,headerLocalRect);
		//�w�b�_rect�`��
		AColor	headercolor_start = AColorWrapper::ChangeHSV(headercolor,0,0.8,1.0);//#643
		AColor	headercolor_end = AColorWrapper::ChangeHSV(headercolor,0,1.2,1.0);//#643
		//
		AFloatNumber	headerAlpha = 0.15;
		/*test
		if( mCurrentSelectionIndex == i )
		{
			AColorWrapper::GetListHighlightColor(headercolor_end);
			headercolor_start = AColorWrapper::ChangeHSV(headercolor_end,0,0.6,1.0);
			headerLetterColor = kColor_White;
			headerAlpha = 1.0;
		}
		*/
		//
		NVMC_PaintRoundedRect(headerLocalRect,headercolor_start,headercolor_end,kGradientType_Vertical,headerAlpha,headerAlpha,
							  5,true,true,true,true);
		
		//==================�ԍ��`��==================
		//�t�@�C�����t�H���g�擾
		NVMC_SetDefaultTextProperty(labelfontname,fontsize-0.5,headerLetterColor,kTextStyle_Normal,true);
		ANumber	labelfontheight = 9 ,labelfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(labelfontheight,labelfontascent);
		//�w�b�_rect�擾
		AImageRect	labelImageRect = itemImageRect;//#643
		labelImageRect.left		= itemImageRect.left + kHeaderLeftMargin;//#643
		labelImageRect.right	= itemImageRect.right - kHeaderRightMargin;
		labelImageRect.bottom 	= itemImageRect.top + labelfontheight +kHeaderTopMargin;//#643
		labelImageRect.top		+= kHeaderTopMargin;
		ALocalRect	labelLocalRect = {0};
		NVM_GetLocalRectFromImageRect(labelImageRect,labelLocalRect);
		//���ڔԍ��`��
		AText	itemnumtext;
		itemnumtext.AddFormattedCstring("(%d/%d)",i+1,mContentTextArray.GetItemCount());
		NVMC_DrawText(labelLocalRect,itemnumtext,headerLetterColor,kTextStyle_Normal,kJustification_Right);
		
		//=========================�t�@�C�����e�L�X�g�`��=========================
		
		//
		ALocalRect	filenameLocalRect = labelLocalRect;
		filenameLocalRect.right -= NVMC_GetDrawTextWidth(itemnumtext);
		//�t�@�C�����擾
		AText	filename;
		mFilePathArray.GetTextConst(i).GetFilename(filename);
		//�t�@�C�����e�L�X�g�`��
		if( filename.GetItemCount() > 0 )
		{
			NVMC_DrawText(filenameLocalRect,filename,headerLetterColor,kTextStyle_Bold|kTextStyle_DropShadow,kJustification_Left);
		}
		
		//==================�N���X���e�L�X�g�`��==================
		
		//�e�L�[���[�h�擾
		AText	parentKeyword;
		mClassNameArray.Get(i,parentKeyword);
		if( parentKeyword.GetItemCount() > 0 )
		{
			ALocalRect	classLocalRect = filenameLocalRect;
			classLocalRect.left += NVMC_GetDrawTextWidth(filename) +8;
			//�N���X���e�L�X�g
			AText	classnameText;
			classnameText.SetCstring("(");
			classnameText.AddLocalizedText("Letter_ParentKeyword");
			classnameText.AddText(parentKeyword);
			classnameText.AddCstring(")");
			//�N���X���e�L�X�g�`��
			NVMC_DrawText(classLocalRect,classnameText,headerLetterColor,kTextStyle_Bold|kTextStyle_DropShadow,kJustification_Left);
		}
		
		//=========================InfoText�����i���o�������j�`��=========================
		
		//InfoText�i���o���j�t�H���g�ݒ�
		{
			NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,kColor_Black,kTextStyle_Bold,true);//#1316 ���@�\��������Ȃ�_�[�N���[�h�Ή��K�v 
			ANumber	infotextfontheight = 9, infotextfontascent = 7;
			NVMC_GetMetricsForDefaultTextProperty(infotextfontheight,infotextfontascent);
			//�֐����擾
			AText	headerText;
			mFunctionNameArray.Get(i,headerText);
			//���o���`��
			AImageRect	lineImageRect = itemImageRect;
			lineImageRect.left		= itemImageRect.left + kInfoTextLeftMargin_Header;
			lineImageRect.top 		= itemImageRect.top + mDrawData_HeaderHeight.Get(i) + kInfoTextTopMargin;
			lineImageRect.bottom	= lineImageRect.top + infotextfontheight;
			ALocalRect	lineLocalRect = {0};
			NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
			NVMC_DrawText(lineLocalRect,headerText);
		}
		
		//=========================InfoText�����i�{�̕����j�`��=========================
		
		//InfoText�t�H���g�ݒ�
		NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,letterColor,kTextStyle_Normal,true);
		ANumber	infotextfontheight = 9, infotextfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(infotextfontheight,infotextfontascent);
		//
		ANumber	lineleftmargin = kInfoTextLeftMargin;
		//�e�s���ƃ��[�v
		for( AIndex lineIndex = 0; lineIndex < mDrawData_InfoTextLineStart.GetObjectConst(i).GetItemCount(); lineIndex++ )
		{
			//�s�f�[�^�擾
			AIndex	start 	= mDrawData_InfoTextLineStart .GetObjectConst(i).Get(lineIndex);
			AItemCount	len = mDrawData_InfoTextLineLength.GetObjectConst(i).Get(lineIndex);
			AText	text;
			mContentTextArray.GetTextConst(i).GetText(start,len,text);
			//text draw data�擾
			CTextDrawData	textDrawData(false);
			textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
			//�s�̍ŏ���attr�ݒ�
			{
				textDrawData.attrPos.Add(0);
				textDrawData.attrColor.Add(letterColor);
				textDrawData.attrStyle.Add(kTextStyle_Normal);
			}
			//���[�h��v�ӏ���ԕ����\������
			AIndex	contentTextSpos = mContentTextStartIndexArray.Get(i);//���Ӄe�L�X�g�̊J�ntext index
			if( contentTextSpos != kIndex_Invalid )
			{
				//���[�h��v�ӏ��́A���ݍs�ł�x�ʒu���擾
				AIndex	s = mStartArray.Get(i)-contentTextSpos - start;
				AIndex	e = mEndArray.Get(i)-contentTextSpos - start;
				if( s >= 0 && s < len && e >= 0 && e < len )
				{
					//���[�h��v�ӏ������ݍs�̒��ɂ���΁A�ԐF�ɂ���
					textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(s));
					textDrawData.attrColor.Add(kColor_Red);//#1316 ���@�\��������Ȃ�_�[�N���[�h�Ή��K�vGetApp().SPI_GetSubWindowBoxMatchedColor());
					textDrawData.attrStyle.Add(kTextStyle_Bold);
					textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(e));
					textDrawData.attrColor.Add(letterColor);
					textDrawData.attrStyle.Add(kTextStyle_Normal);
				}
			}
			//�s�`��
			AImageRect	lineImageRect = itemImageRect;//#643
			lineImageRect.left		= itemImageRect.left + lineleftmargin;//#643
			lineImageRect.top 		= itemImageRect.top + mDrawData_HeaderHeight.Get(i) +
									infotextfontheight +
									lineIndex*infotextfontheight + kInfoTextTopMargin;//#643
			lineImageRect.bottom	= lineImageRect.top + infotextfontheight + kInfoTextTopMargin;//#643
			ALocalRect	lineLocalRect = {0};
			NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
			NVMC_DrawText(lineLocalRect,textDrawData);
		}
	}
	
	//==================�T�}���\��==================
	//�T�}���\��rect�擾
	AImageRect	summaryRect = {0};
	summaryRect.left	= kSummaryLeftMargin;
	summaryRect.top		= kSummaryTopMargin;
	if( mDrawData_Y.GetItemCount() > 0 )
	{
		summaryRect.top += mDrawData_Y.Get(mDrawData_Y.GetItemCount()-1) + mDrawData_Height.Get(mDrawData_Height.GetItemCount()-1);
	}
	summaryRect.right	= NVI_GetViewWidth();
	summaryRect.bottom	= summaryRect.top + kSummaryHeight;
	ALocalRect	summaryLocalRect = {0};
	NVM_GetLocalRectFromImageRect(summaryRect,summaryLocalRect);
	//�T�}���e�L�X�g�擾
	AText	summaryText;
	if( mContentTextArray.GetItemCount() == 0 )
	{
		summaryText.SetLocalizedText("WordsListSummary_NoResult");
	}
	else
	{
		if( mSearchInProjectFolder == true )
		{
			summaryText.SetLocalizedText("WordsListSummary_ProjectFolder");
		}
		else
		{
			summaryText.SetLocalizedText("WordsListSummary_SameFolder");
		}
		AText	t;
		t.SetLocalizedText("WordsListSummary_WholeWord");
		summaryText.AddText(t);
		//
		if( mContentTextArray.GetItemCount() >= kLimit_WordsListResultCount )
		{
			t.SetLocalizedText("WordsListSummary_TooManyItems");
			summaryText.AddText(t);
		}
		else if( GetApp().SPI_GetWordsListWindow(NVM_GetWindow().GetObjectID()).SPI_GetShowImportFileProgress() == true )
		{
			t.SetLocalizedText("WordsListSummary_ImportFileProgress");
			summaryText.AddText(t);
		}
	}
	//�T�}���`��i�}���`���C���j
	AColor	summaryColor = GetApp().SPI_GetSubWindowHeaderLetterColor();
	NVI_DrawTextMultiLine(summaryLocalRect,summaryText,labelfontname,fontsize,summaryColor,kTextStyle_Bold,true);
}

/**
�eItemBox��imagerect�擾
*/
void	AView_WordsList::GetItemBoxImageRect( const AIndex inIndex, AImageRect& outImageRect ) const
{
	outImageRect.left		= kItemBoxLeftMargin;
	outImageRect.top		= mDrawData_Y.Get(inIndex) + kItemBoxTopMargin;
	outImageRect.right		= NVI_GetViewWidth() - kItemBoxRightMargin;
	outImageRect.bottom		= outImageRect.top + mDrawData_Height.Get(inIndex);
}

//DrawData�v�Z
void	AView_WordsList::CalcDrawData( const ABool inCalcAll )
{
	//�`��f�[�^�폜
	mDrawData_Y.DeleteAll();
	mDrawData_Height.DeleteAll();
	mDrawData_HeaderHeight.DeleteAll();
	mDrawData_InfoTextHeight.DeleteAll();
	mDrawData_InfoTextLineStart.DeleteAll();
	mDrawData_InfoTextLineLength.DeleteAll();
	mDrawDataAllCalculated = inCalcAll;
	
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
	
	//=========================�w�b�_�����P�s�̍����v�Z=========================
	NVMC_SetDefaultTextProperty(labelfontname,fontsize,kColor_Black,kTextStyle_Bold,true);
	ANumber	headerfontheight = 9, headerfontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(headerfontheight,headerfontascent);
	
	//=========================InfoText�����P�s�̍����v�Z�Atext property�ۑ�=========================
	//�t�H���g�����擾
	NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,kColor_Black,kTextStyle_Normal,true);
	ANumber	infotextfontheight = 9, infotextfontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(infotextfontheight,infotextfontascent);
	
	//=========================���ږ����[�v=========================
	ANumber	y = 0;
	for( AIndex i = 0; i < mContentTextArray.GetItemCount(); i++ )
	{
		mDrawData_Y.Add(y);
		mDrawData_InfoTextLineStart .AddNewObject();
		mDrawData_InfoTextLineLength.AddNewObject();
		
		//=========================�w�b�_�����f�[�^�ۑ�=========================
		mDrawData_HeaderHeight.Add(kHeaderTopMargin+headerfontheight+kHeaderBottomMargin);
		
		//=========================InfoText�����v�Z=========================
		//InfoText�擾
		AText	infoText;
		mContentTextArray.Get(i,infoText);
		//InfoText�s�܂�Ԃ��v�Z
		if( inCalcAll == true || i < 32 )
		{
			/*
			CWindowImp::CalcLineBreaks(infoText,fontname,fontsize-0.5,true,
						viewwidth-kItemBoxLeftMargin-kItemBoxRightMargin -kInfoTextLeftMargin2 - kInfoTextRightMargin,
						mDrawData_InfoTextLineStart.GetObject(i),mDrawData_InfoTextLineLength.GetObject(i));
			*/
			//�s�܂�Ԃ������Ōv�Z����
			infoText.CalcParagraphBreaks(mDrawData_InfoTextLineStart.GetObject(i),mDrawData_InfoTextLineLength.GetObject(i));
			//�ŏI�s�͋�s�Ȃ̂ō폜����
			AItemCount	itemCount = mDrawData_InfoTextLineStart.GetObject(i).GetItemCount();
			if( itemCount > 0 )
			{
				mDrawData_InfoTextLineStart.GetObject(i).DeleteAfter(itemCount-1);
				mDrawData_InfoTextLineLength.GetObject(i).DeleteAfter(itemCount-1);
			}
		}
		else
		{
			//inCalcAll��false�̂Ƃ��́Aindex:32�����̍s�̂݌v�Z����i�`�掞�ɕK�v�ɉ�����index:32�ȍ~���v�Z����j
			mDrawData_InfoTextLineStart.GetObject(i).Add(0);
			mDrawData_InfoTextLineLength.GetObject(i).Add(0);
		}
		//
		//InfoText�S�̂̍����v�Z
		mDrawData_InfoTextHeight.Add(kInfoTextTopMargin+
					infotextfontheight*mDrawData_InfoTextLineStart.GetObject(i).GetItemCount() +
					kInfoTextBottomMargin);
		
		//���ڑS�̂̍����ݒ�
		mDrawData_Height.Add(mDrawData_HeaderHeight.Get(i) + mDrawData_InfoTextHeight.Get(i) + 2 + infotextfontheight);
		//���̍��ڂ�y�ʒu���v�Z
		y += mDrawData_Height.Get(i);
		y += kItemBoxTopMargin + kItemBoxBottomMargin;//#643
	}
	//ImageSize�ݒ�
	NVM_SetImageSize(viewwidth,y + kSummaryHeight);
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(0,kVScrollUnit,0,rect.bottom-rect.top-kVScrollUnit);
}

/**
�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
*/
ABool	AView_WordsList::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//tracking�J�n���}�E�X�ʒu�L��
	mMouseTrackStartLocalPoint = inLocalPoint;
	mMouseClickCount = inClickCount;
	
	//Mouse tracking�J�n
	NVM_SetMouseTrackingMode(true);
	return false;
}

/**
�}�E�X�{�^��������Tracking��OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_WordsList::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
}

/**
�}�E�X�{�^��������Tracking�I������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_WordsList::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�N���b�N�ʒu�̍���index�擾
	AIndex	index = FindItem(inLocalPoint);
	if( index != kIndex_Invalid )
	{
		//���b�N���[�h�ݒ�
		GetApp().SPI_GetWordsListWindow(NVM_GetWindow().GetObjectID()).SPI_SetLockedMode(mMouseClickCount==1);
		//�N���b�N�������ڂ��J��
		SPI_OpenItem(index,inModifierKeys);
	}
	//Tracking����
	NVM_SetMouseTrackingMode(false);
}

/**
Loacal point���獀�ڌ���
*/
AIndex	AView_WordsList::FindItem( const ALocalPoint inLocalPoint ) const
{
	AImagePoint	imagept = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	for( AIndex i = 0; i < mContentTextArray.GetItemCount(); i++ )
	{
		if( imagept.y > mDrawData_Y.Get(i) && imagept.y < mDrawData_Y.Get(i) + mDrawData_Height.Get(i) )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

/**
���̍��ڂ�I��
*/
void	AView_WordsList::SPI_SelectNextItem()
{
	if( mCurrentSelectionIndex >= 0 && mCurrentSelectionIndex+1 < mContentTextArray.GetItemCount() )
	{
		//���ݑI���ʒu�����ɂ���
		mCurrentSelectionIndex++;
		//���ڂ��J��
		SPI_OpenItem(mCurrentSelectionIndex,0);
		//�X�N���[������
		SPI_AdjustScroll();
	}
}

/**
�O�̍��ڂ�I��
*/
void	AView_WordsList::SPI_SelectPreviousItem()
{
	if( mCurrentSelectionIndex-1 >= 0 && mCurrentSelectionIndex < mContentTextArray.GetItemCount() )
	{
		//���ݑI���ʒu��O�ɂ���
		mCurrentSelectionIndex--;
		//���ڂ��J��
		SPI_OpenItem(mCurrentSelectionIndex,0);
		//�X�N���[������
		SPI_AdjustScroll();
	}
}

/**
���ڑI����ԉ���
*/
void	AView_WordsList::SPI_CancelSelect()
{
	mCurrentSelectionIndex = kIndex_Invalid;
	NVI_Refresh();
}

//#853
/**
���ڂɕ\�����ꂽ�L�[���[�h�̒�`�ʒu���I�[�v������
*/
void	AView_WordsList::SPI_OpenItem( const AIndex inIndex, const AModifierKeys inModifierKeys )
{
	//App�ɂ��̃T�u�E�C���h�E���A�N�e�B�u�T�u�E�C���h�E�ł��邱�Ƃ�ݒ�
	GetApp().SPI_SetActiveSubWindowForItemSelector(NVM_GetWindow().GetObjectID());
	//���ݑI���ʒu��ݒ�
	mCurrentSelectionIndex = inIndex;
	//kIndex_Invalid�Ȃ�I��
	if( inIndex == kIndex_Invalid )
	{
		return;
	}
	//�N���b�N�ς݂��L��
	mClickedArray.Set(inIndex,true);
	//�`��
	NVI_Refresh();
	
	//���݂̈ʒu��Navigate�o�^���� #146
	GetApp().SPI_RegisterNavigationPosition();
	//�t�@�C���擾
	AText	filepath;
	mFilePathArray.Get(inIndex,filepath);
	//���ڃf�[�^�擾
	AIndex	spos = 0, epos = 0;
	spos = mStartArray.Get(inIndex);
	epos = mEndArray.Get(inIndex);
	//���ڂ��J��
	GetApp().SPI_OpenOrRevealTextDocument(filepath,spos,epos,kObjectID_Invalid,
				(AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true),
				kAdjustScrollType_Center);
}

/**
�}�E�X�z�C�[��������
*/
void	AView_WordsList::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
		const ALocalPoint inLocalPoint )
{
	//�X�N���[������ݒ肩��擾����
	ANumber	scrollPercent;
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true 
				|| AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd);
	}
	else
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent);
	}
	//�X�N���[�����s
	NVI_Scroll(0,inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100);
	//�z�o�[�X�V�O�̍���index���擾
	AIndex	svHoverIndex = mCurrentHoverCursorIndex;
	//Hover�X�V
	EVTDO_DoMouseMoved(inLocalPoint,inModifierKeys);
	//�z�o�[����index�ɕω�������ꍇ�́A�w���v�^�O���X�V
	if( svHoverIndex != mCurrentHoverCursorIndex )
	{
		NVM_GetWindow().NVI_UpdateHelpTag(NVI_GetControlID(),inLocalPoint,inModifierKeys,true);
	}
}

/**
�}�E�X�z�o�[������
*/
ABool	AView_WordsList::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�z�o�[�ݒ�
	AImagePoint	imagept;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	for( AIndex i = 0; i < mContentTextArray.GetItemCount(); i++ )
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

/**
�}�E�Xleave������
*/
void	AView_WordsList::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearHoverCursor();
}

/**
�z�o�[����
*/
void	AView_WordsList::ClearHoverCursor()
{
	if( mCurrentHoverCursorIndex != kIndex_Invalid )
	{
		mCurrentHoverCursorIndex = kIndex_Invalid;
		NVI_Refresh();
	}
}

/**
�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
*/
ABool	AView_WordsList::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	/*
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	AApplication::GetApplication().NVI_GetMenuManager().
	ShowContextMenu(mContextMenuItemID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
	*/
	return true;
}

/**
�w���v�^�O
*/
ABool	AView_WordsList::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	AIndex	index = FindItem(inPoint);
	if( index != kIndex_Invalid )
	{
		outTagSide = kHelpTagSide_Right;
		mContentTextArray.Get(index,outText1);
		//�w���v�^�O�e�L�X�g�ݒ�
		outText1.SetLocalizedText("WordsListHelpTag");
		outText1.ReplaceParamText('1',mFilePathArray.GetTextConst(index));//�t�@�C��
		outText1.ReplaceParamText('2',mClassNameArray.GetTextConst(index));//�e�L�[���[�h
		outText1.ReplaceParamText('3',mFunctionNameArray.GetTextConst(index));//���o��
		outText1.ReplaceParamText('4',mContentTextArray.GetTextConst(index));//�e�L�X�g
		AImageRect	imageRect = {0};
		imageRect.left		= 0;
		imageRect.right		= NVI_GetViewWidth();
		imageRect.top		= mDrawData_Y.Get(index);
		imageRect.bottom	= imageRect.top + 10;//mDrawData_Height.Get(index);
		NVM_GetLocalRectFromImageRect(imageRect,outRect);
		return true;
	}
	else
	{
		return false;
	}
}

/**
�e�L�X�g���͎�����
*/
ABool	AView_WordsList::EVTDO_DoTextInput( const AText& inText, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	return false;
}

/**
�}�E�X�J�[�\���ݒ�
*/
ACursorType	AView_WordsList::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
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

/**
���[�h���X�g�����v��
*/
void	AView_WordsList::SPI_RequestWordsList( const AFileAcc& inProjectFolder, const AIndex inModeIndex, const AText& inWord )
{
	//�O�񌟍����[�h�Ɠ����A���A�v���W�F�N�g�������Ȃ牽�����Ȃ�
	if( mWord.Compare(inWord) == true && mCurrentProjectFolder.Compare(inProjectFolder) == true )
	{
		return;
	}
	
	//�Ō�̗v���f�[�^�L��
	mLastRequest_ProjectFolder = inProjectFolder;
	mLastRequest_ModeIndex = inModeIndex;
	mLastRequest_Word.SetText(inWord);
	
	//���b�N���[�h���͉������Ȃ�
	if( mLockedMode == true )
	{
		return;
	}
	
	//�������ʃf�[�^�S�폜
	SPI_DeleteAllData();
	
	//�X�N���[������
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
	//�ĕ`��
	NVI_Refresh();
	
	//�����p�����[�^�L��
	mCurrentProjectFolder.CopyFrom(inProjectFolder);
	mModeIndex = inModeIndex;
	mWord.SetText(inWord);
	//�v���W�F�N�g�t�H���_�̌������ǂ������L��
	mSearchInProjectFolder = (GetApp().SPI_GetSameProjectArrayIndex(inProjectFolder)!=kIndex_Invalid);
	
	//�X���b�h�Ƀ��[�Y���X�g�����v��
	if( inWord.GetItemCount() > 0 )
	{
		GetWordsListFinderThread().SetRequestDataForWordsList(mCurrentProjectFolder,inModeIndex,NVM_GetWindowConst().GetObjectID(),inWord);
		//�X���b�hunpause
		GetWordsListFinderThread().NVI_UnpauseIfPaused();
		
		//�������t���OON
		mIsFinding = true;
	}
	
	//�w�b�_�r���[�X�V
	AText	path;
	mCurrentProjectFolder.GetPath(path);
	GetApp().SPI_GetWordsListWindow(NVM_GetWindow().GetObjectID()).SPI_SetHeaderText(mWord,path,mModeIndex);
}

/**
�������f
*/
void	AView_WordsList::SPI_AbortCurrentWordsListFinderRequest()
{
	GetWordsListFinderThread().AbortCurrentRequest();
}

/**
�������ʑS�폜
*/
void	AView_WordsList::SPI_DeleteAllData()
{
	//�X���b�h�ł̌��݂̌����𒆒f
	SPI_AbortCurrentWordsListFinderRequest();
	
	//���ݑI�����ړ��N���A
	mCurrentHoverCursorIndex = kIndex_Invalid;
	mCurrentSelectionIndex = kIndex_Invalid;
	mMouseTrackStartLocalPoint = kLocalPoint_00;
	//�v���f�[�^�N���A
	mWord.DeleteAll();
	mCurrentProjectFolder.Unspecify();
	mModeIndex = kIndex_Invalid;
	mSearchInProjectFolder = false;
	//��ԃN���A
	mLockedMode = false;
	mIsFinding = false;
	//�������ʑS�폜
	mFilePathArray.DeleteAll();
	mFunctionNameArray.DeleteAll();
	mClassNameArray.DeleteAll();
	mClassNameArray.DeleteAll();
	mStartArray.DeleteAll();
	mEndArray.DeleteAll();
	mHeaderColorArray.DeleteAll();
	mContentTextArray.DeleteAll();
	mContentTextStartIndexArray.DeleteAll();
	mClickedArray.DeleteAll();
	mAllFilesSearched = false;
	//�`��f�[�^�N���A
	CalcDrawData(false);
}

/**
�X���b�h�̃��[�Y���X�g�������ʔ��f
*/
void	AView_WordsList::SPI_DoWordsListFinderPaused()
{
	//�������ʎ擾
	ABool	allFilesSearched = GetWordsListFinderThread().GetResultForWordsList(mFilePathArray,mClassNameArray,mFunctionNameArray,mStartArray,mEndArray,mHeaderColorArray,mContentTextArray,mContentTextStartIndexArray);
	//���ݑI�����ڃN���A
	mCurrentSelectionIndex = kIndex_Invalid;
	//�N���b�N�ς݃t���O�N���A
	for( AIndex i = 0; i < mFilePathArray.GetItemCount(); i++ )
	{
		mClickedArray.Add(false);
	}
	//�S�t�@�C�������ς݃t���O�L��
	mAllFilesSearched = allFilesSearched;
	//�`��f�[�^�v�Z�iindex:32�����܂ŁB�������̂��߁B����ȍ~�͕`�掞�ɕK�v�ɉ����ĕ`��j
	CalcDrawData(false);
	//�X�N���[������
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
	//�������t���OOFF
	mIsFinding = false;
	//�E�C���h�E�̕`��v���p�e�B�X�V
	NVM_GetWindow().NVI_UpdateProperty();
	//�ĕ`��
	NVI_Refresh();
}

/**
�h�L�������g�ҏW���ɃR�[�������B�e�L�X�g�ʒu�̍X�V�B
*/
void	AView_WordsList::SPI_NotifyToWordsListByTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	//�X�V�t�@�C�����擾
	AText	filepath;
	inFile.GetPath(filepath);
	//�X�V�t�@�C���Ɉ�v���鍀�ڂ�index���擾
	AArray<AIndex>	indexArray;
	mFilePathArray.FindAll(filepath,0,filepath.GetItemCount(),indexArray);
	//�e���ږ��̃��[�v
	AItemCount	itemCount = indexArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AIndex	index = indexArray.Get(i);
		//start index���X�V�ʒu�ȍ~�̏ꍇ�́Astart index, end index���X�V
		AIndex	spos = mStartArray.Get(index);
		if( spos > inTextIndex )
		{
			mStartArray.Set(index,spos+inInsertedCount);
			mEndArray.Set(index,mEndArray.Get(index)+inInsertedCount);
		}
	}
}

/**
�`��f�[�^�X�V�i�E�C���h�Ebounds�ύX�����j
*/
void	AView_WordsList::SPI_UpdateDrawProperty()
{
	CalcDrawData(true);
	NVI_Refresh();
}

/**
�X�N���[������
*/
void	AView_WordsList::SPI_AdjustScroll()
{
	//���ݑI�����ڂ������ɕ\�������悤�ɃX�N���[������
	if( mCurrentSelectionIndex != kIndex_Invalid )
	{
		//���ݑI�����ڂ�rect���擾
		AImageRect	box = {0};
		GetItemBoxImageRect(mCurrentSelectionIndex,box);
		AImageRect	imageFrameRect = {0};
		NVM_GetImageViewRect(imageFrameRect);
		//frame��艺�ɍ��ڂ�����ꍇ
		if( box.bottom > imageFrameRect.bottom )
		{
			AImagePoint	imagept = {imageFrameRect.left,box.top-(imageFrameRect.bottom-imageFrameRect.top)/2};
			if( imagept.y < 0 )   imagept.y = 0;
			NVI_ScrollTo(imagept);
		}
		//frame����ɍ��ڂ�����ꍇ
		if( box.top < imageFrameRect.top )
		{
			AImagePoint	imagept = {imageFrameRect.left,box.top-(imageFrameRect.bottom-imageFrameRect.top)/2};
			if( imagept.y < 0 )   imagept.y = 0;
			NVI_ScrollTo(imagept);
		}
	}
}

/**
���b�N���[�h�ɐݒ�
*/
void	AView_WordsList::SPI_SetLockedMode( const ABool inLocked )
{
	mLockedMode = inLocked;
	SPI_CancelSelect();
	//�������́A�Ō�̗v����K�p����
	if( inLocked == false && mLastRequest_Word.GetItemCount() > 0 )
	{
		AFileAcc	projectFolder = mLastRequest_ProjectFolder;
		AIndex	modeIndex = mLastRequest_ModeIndex;
		AText	word = mLastRequest_Word;
		SPI_RequestWordsList(projectFolder,modeIndex,word);
	}
}

