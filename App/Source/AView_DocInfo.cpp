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

AView_DocInfo
#142

*/

#include "stdafx.h"

#include "AView_DocInfo.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_DocInfo
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^

@param inWindowID �e�E�C���h�E��ID
@param inID View��ControlID
*/
AView_DocInfo::AView_DocInfo( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID)
,mCurrentChar(0),mCurrentCanonicalDecomp(0),mSelectedColor(kColor_Black)
{
	mTemplateText_Total1.SetLocalizedText("DocInfo_Total1");
	mTemplateText_Total2.SetLocalizedText("DocInfo_Total2");
	mTemplateText_Total3.SetLocalizedText("DocInfo_Total3");
	mTemplateText_Total4.SetLocalizedText("DocInfo_Total4");
	mTemplateText_Selected1.SetLocalizedText("DocInfo_Selected1");
	mTemplateText_Selected2.SetLocalizedText("DocInfo_Selected2");
	mTemplateText_Selected3.SetLocalizedText("DocInfo_Selected3");
	mTemplateText_Selected4.SetLocalizedText("DocInfo_Selected4");
	mTemplateText_CurrentCharReturnCode.SetLocalizedText("DocInfo_CurrentCharReturnCode");
	mTemplateText_CurrentChar.SetLocalizedText("DocInfo_CurrentChar");
	mTemplateText_HeaderPath.SetLocalizedText("DocInfo_HeaderPath");
	mTemplateText_HeaderPathParagraph.SetLocalizedText("DocInfo_HeaderPathParagraph");
	mTemplateText_HeaderPathTriangle.SetLocalizedText("DocInfo_HeaderPathTriangle");
	mTemplateText_SelectedTextColor.SetLocalizedText("DocInfo_SelectedTextColor");
	mTemplateText_SelectedTextNumber.SetLocalizedText("DocInfo_SelectedTextNumber");
}

/**
�f�X�g���N�^
*/
AView_DocInfo::~AView_DocInfo()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_DocInfo::NVIDO_Init()
{
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_DocInfo::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�]���f�[�^
const ANumber	kLeftMargin = 8;
const ANumber	kTopMargin = 5;
const ANumber	kRightMargin = 0;
const ANumber	kLineMargin = 2;

/**
�`��v�����̃R�[���o�b�N(AView�p)
*/
void	AView_DocInfo::EVTDO_DoDraw()
{
	//�����F�擾
	AColor	subwindowNormalColor = GetApp().SPI_GetSubWindowLetterColor();
	
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	//=========================�w�i���擾�i����view�ł͔w�i�͕`�悵�Ȃ��j=========================
	//�w�i���擾
	AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
	//�T�u�E�C���h�ELocationType�ɂ���āA���ꂼ��̔w�i�`��
	switch(GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()))
	{
	  case kSubWindowLocationType_Floating:
		{
			//�t���[�e�B���O�̎w�蓧�ߗ��Ŕw�i����
			//NVMC_PaintRect(localFrameRect,backgroundColor,GetApp().SPI_GetFloatingWindowAlpha());
			break;
		}
	  default:
		{
			//�|�b�v�A�b�v�ȊO�Ȃ�T�u�E�C���h�E�w�i�F�ŏ���
			//NVMC_PaintRect(localFrameRect,backgroundColor);
			break;
		}
	}
	
	
	//�T�u�E�C���h�E�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//
	NVMC_SetDefaultTextProperty(fontname,fontsize,subwindowNormalColor,kTextStyle_Normal,true,1.0);
	ANumber	lineHeight = 9, lineAscent = 3;
	NVMC_GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
	//
	ANumber	y = kTopMargin;
	
	//==================�g�[�^�����==================
	{
		//�e�L�X�g�ݒ�
		AText	text;
		text.AddText(mTemplateText_Total1);
		AIndex	charCountStart = text.GetItemCount();
		text.AddText(mTotalCharCountText);
		AIndex	charCountEnd = text.GetItemCount();
		text.AddText(mTemplateText_Total2);
		AIndex	wordCountStart = text.GetItemCount();
		text.AddText(mTotalWordCountText);
		AIndex	wordCountEnd = text.GetItemCount();
		text.AddText(mTemplateText_Total3);
		AIndex	paraCountStart = text.GetItemCount();
		text.AddText(mTotalParagraphCountText);
		AIndex	paraCountEnd = text.GetItemCount();
		text.AddText(mTemplateText_Total4);
		
		//text draw data�擾
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//�ʏ핶��
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//����
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(charCountStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//�ʏ핶��
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(charCountEnd));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//����
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(wordCountStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//�ʏ핶��
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(wordCountEnd));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//����
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(paraCountStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//�ʏ핶��
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(paraCountEnd));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//�e�L�X�g�`��
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//����y
		y += lineHeight + kLineMargin;
	}
	
	//==================�I��͈͏��==================
	{
		//�e�L�X�g�ݒ�
		AText	text;
		text.AddText(mTemplateText_Selected1);
		AIndex	charCountStart = text.GetItemCount();
		text.AddText(mSelectedCharCountText);
		AIndex	charCountEnd = text.GetItemCount();
		text.AddText(mTemplateText_Selected2);
		AIndex	wordCountStart = text.GetItemCount();
		text.AddText(mSelectedWordCountText);
		AIndex	wordCountEnd = text.GetItemCount();
		text.AddText(mTemplateText_Selected3);
		AIndex	paraCountStart = text.GetItemCount();
		text.AddText(mSelectedParagraphCountText);
		AIndex	paraCountEnd = text.GetItemCount();
		text.AddText(mTemplateText_Selected4);
		
		//text draw data�擾
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//�ʏ핶��
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//����
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(charCountStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//�ʏ핶��
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(charCountEnd));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//����
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(wordCountStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//�ʏ핶��
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(wordCountEnd));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//����
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(paraCountStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//�ʏ핶��
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(paraCountEnd));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//�e�L�X�g�`��
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//����y
		y += lineHeight + kLineMargin;
	}
	
	//==================���݂̕���==================
	//���ݕ��������s�̏ꍇ
	if( mCurrentChar == kUChar_LineEnd )
	{
		//�e�L�X�g�ݒ�
		AText	text;
		text.SetText(mTemplateText_CurrentCharReturnCode);
		
		//text draw data�擾
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//�ʏ핶��
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		
		//�e�L�X�g�`��
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//����y
		y += lineHeight + kLineMargin;
	}
	//���ݕ��������s�ȊO�̏ꍇ
	else
	{
		//�e�L�X�g�ݒ�
		AText	text;
		text.AddText(mTemplateText_CurrentChar);
		AIndex	currentCharStart = text.GetItemCount();
		text.AddText(mCurrentCharText);
		
		//text draw data�擾
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//�ʏ핶��
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//����
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(currentCharStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//�e�L�X�g�`��
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//����y
		y += lineHeight + kLineMargin;
	}
	
	//==================�F==================
	if( mSelectedColorText.GetItemCount() > 0 )
	{
		AText	text;
		text.AddText(mTemplateText_SelectedTextColor);
		text.AddText(mSelectedColorText);
		
		//text draw data�擾
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//�ʏ핶��
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(mSelectedColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//�e�L�X�g�`��
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//����y
		y += lineHeight + kLineMargin;
		
		//
		ALocalRect	colorRect = rect;
		colorRect.left	= rect.left + NVMC_GetDrawTextWidth(text) + 16;
		colorRect.right = colorRect.left + 64;
		NVMC_PaintRect(colorRect,AColorWrapper::GetColorByHTMLFormatColor(mSelectedColorText));
	}
	
	//==================���l==================
	if( mSelectedNumberText.GetItemCount() > 0 )
	{
		AText	text;
		text.AddText(mTemplateText_SelectedTextNumber);
		text.AddText(mSelectedNumberText);
		
		//text draw data�擾
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//�ʏ핶��
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(mSelectedColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//�e�L�X�g�`��
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//����y
		y += lineHeight + kLineMargin;
	}
	
	//==================���o���p�X==================
	{
		AText	text;
		text.AddText(mTemplateText_HeaderPath);
		
		//text draw data�擾
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//�ʏ핶��
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//�e�L�X�g�`��
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//����y
		y += lineHeight + kLineMargin;
	}
	for( AIndex i = mHeaderTextArray.GetItemCount()-1; i >= 0 ; i-- )
	{
		AText	text;
		//���o��index�������X�y�[�X������O�ɂ���
		AText	spaceText;
		spaceText.SetRepeatedCstring(" ",mHeaderTextArray.GetItemCount()-i);
		text.AddText(spaceText);
		//"�i���F"
		text.AddText(mTemplateText_HeaderPathParagraph);
		//�i���ԍ�
		AText	t;
		t.SetNumber(mHeaderParagraphIndexArray.Get(i)+1);
		text.AddText(t);
		text.AddCstring(" ");
		//�E�O�p
		text.AddText(mTemplateText_HeaderPathTriangle);
		text.AddCstring(" ");
		//���o���e�L�X�g
		AText	headerText;
		mHeaderTextArray.Get(i,headerText);
		text.AddText(headerText);
		
		//text draw data�擾
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//�ʏ핶��
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		
		//�e�L�X�g�`��
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//����y
		y += lineHeight + kLineMargin;
	}
	
	//==================�f�o�b�O���==================
	for( AIndex pos = 0; pos < mDebugText.GetItemCount(); )
	{
		AText	text;
		mDebugText.GetLine(pos,text);
		
		//text draw data�擾
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//�ʏ핶��
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		
		//�e�L�X�g�`��
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//����y
		y += lineHeight + kLineMargin;
	}
	
	//==================�v���O�C���e�L�X�g���==================
	for( AIndex pos = 0; pos < mPluginText.GetItemCount(); )
	{
		AText	text;
		mPluginText.GetLine(pos,text);
		
		//text draw data�擾
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//�ʏ핶��
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		
		//�e�L�X�g�`��
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//����y
		y += lineHeight + kLineMargin;
	}
	
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

/**
DocInfo�X�V�itotal�j
*/
void	AView_DocInfo::SPI_UpdateDocInfo_TotalCount( const AItemCount inTotalCharCount, 
													const AItemCount inTotalWordCount,
													const AItemCount inTotalParagraphCount )
{
	//�g�[�^�����X�V
	mTotalCharCountText.SetNumber(inTotalCharCount);
	mTotalWordCountText.SetNumber(inTotalWordCount);
	mTotalParagraphCountText.SetNumber(inTotalParagraphCount);
	//�`��X�V
	NVI_Refresh();
}

/**
DocInfo�X�V�i�I��͈́j
*/
void	AView_DocInfo::SPI_UpdateDocInfo_SelectedCount( const AItemCount inSelectedCharCount, 
													   const AItemCount inSelectedWordCount,
													   const AItemCount inSelectedParagraphCount, 
													   const AText& inSelectedText )
{
	//�I��͈͏��X�V
	mSelectedCharCountText.SetNumber(inSelectedCharCount);
	mSelectedWordCountText.SetNumber(inSelectedWordCount);
	mSelectedParagraphCountText.SetNumber(inSelectedParagraphCount);
	//�I���e�L�X�g�Ŏ������F
	mSelectedColorText.DeleteAll();
	mSelectedColor = kColor_Black;
	//HTML�t�H�[�}�b�g�ŏ���#����� #1095
	AText	HTMLColorText(inSelectedText);
	if( HTMLColorText.GetItemCount() == 7 )
	{
		if( HTMLColorText.Get(0) == '#' )
		{
			HTMLColorText.Delete(0,1);
		}
	}
	//HTML�t�H�[�}�b�g�̏ꍇ�A�I���e�L�X�g�F�ݒ�
	if( HTMLColorText.GetItemCount() == 6 )
	{
		ABool	isHTMLColor = true;
		for( AIndex i = 0; i < 6; i++ )
		{
			AUChar	ch = HTMLColorText.Get(i);
			if( ch >= '0' && ch <= '9' )
			{
			}
			else if( ch >= 'A' && ch <= 'F' )
			{
			}
			else if( ch >= 'a' && ch <= 'f' )
			{
			}
			else
			{
				isHTMLColor = false;
				break;
			}
		}
		if( isHTMLColor == true )
		{
			mSelectedColorText.SetText(HTMLColorText);
			mSelectedColor = AColorWrapper::GetColorByHTMLFormatColor(HTMLColorText);
		}
	}
	//�I���e�L�X�g�Ŏ�����鐔�l
	mSelectedNumberText.DeleteAll();
	{
		ANumber	num = inSelectedText.GetNumber();
		if( num != 0 )
		{
			mSelectedNumberText.SetFormattedCstring("%d 0x%X",num,num);
			//
			if( num >= 0x20 && num <= 0x10FFFF )
			{
				mSelectedNumberText.AddCstring(" '");
				mSelectedNumberText.AddFromUCS4Char(num);
				mSelectedNumberText.AddCstring("'");
			}
		}
	}
	//0x
	if( inSelectedText.GetItemCount() > 2 )
	{
		if( inSelectedText.Get(0) == '0' && inSelectedText.Get(1) == 'x' )
		{
			AText	hextext;
			inSelectedText.GetText(2,inSelectedText.GetItemCount()-2,hextext);
			ANumber	num = hextext.GetIntegerFromHexText();
			mSelectedNumberText.SetFormattedCstring("%d 0x%X",num,num);
			//
			if( num >= 0x20 && num < 0x200000 )
			{
				mSelectedNumberText.AddCstring(" '");
				mSelectedNumberText.AddFromUCS4Char(num);
				mSelectedNumberText.AddCstring("'");
			}
		}
	}
	//�`��X�V
	NVI_Refresh();
}

/**
DocInfo�X�V�i���ݕ����j
*/
void	AView_DocInfo::SPI_UpdateDocInfo_CurrentChar( const AUCS4Char inChar, 
													 const AUCS4Char inCanonicalDecomp, 
													 const ATextArray& inHeaderTextArray, 
													 const AArray<AIndex>& inHeaderParagraphIndexArray, 
													 const AText& inDebugText )
{
	//==================���ݕ������X�V==================
	mCurrentChar = inChar;
	mCurrentCanonicalDecomp = inCanonicalDecomp;
	//���ݕ��������s�ȊO�̏ꍇ
	if( mCurrentChar != kUChar_LineEnd )
	{
		//���ݕ����e�L�X�g�X�V
		mCurrentCharText.SetText(mTemplateText_CurrentChar);
		//------------------U+�`��ݒ�------------------
		AText	t;
		if( mCurrentChar >= 0x10000 )
		{
			t.SetFormattedCstring("U+%05X",mCurrentChar);
		}
		else
		{
			t.SetFormattedCstring("U+%04X",mCurrentChar);
		}
		//canonical decomp����������Ƃ��́A",U+�`"��ݒ�
		if( mCurrentCanonicalDecomp != 0 )
		{
			if( mCurrentCanonicalDecomp >= 0x10000 )
			{
				t.AddFormattedCstring(",U+%05X (Canonical Decomposition)",mCurrentCanonicalDecomp);
			}
			else
			{
				t.AddFormattedCstring(",U+%04X (Canonical Decomposition)",mCurrentCanonicalDecomp);
			}
		}
		mCurrentCharText.SetText(t);
		//------------------���ݕ�����ݒ�------------------
		if( mCurrentChar >= 0x20 )
		{
			AText	chtext;
			chtext.AddFromUCS4Char(mCurrentChar);
			if( mCurrentCanonicalDecomp != 0 )
			{
				//canonical decomp����������ꍇ�͂��̕������ǉ�
				chtext.AddFromUCS4Char(mCurrentCanonicalDecomp);
			}
			mCurrentCharText.AddCstring("  '");
			mCurrentCharText.AddText(chtext);
			mCurrentCharText.AddCstring("'");
		}
	}
	
	//==================�f�o�b�O���==================
	//�f�o�b�O���e�L�X�g�ݒ�
	mDebugText.SetText(inDebugText);
	
	//==================���o���p�X==================
	//���o���p�X�f�[�^�ۑ�
	mHeaderTextArray.SetFromTextArray(inHeaderTextArray);
	mHeaderParagraphIndexArray.SetFromObject(inHeaderParagraphIndexArray);
	
	//�`��X�V
	NVI_Refresh();
}

/**
JavaScript�v���O�C������̃e�L�X�g�ݒ�
*/
void	AView_DocInfo::SPI_UpdateDocInfo_PluginText( const AText& inPluginText )
{
	mPluginText.SetText(inPluginText);
	//�`��X�V
	NVI_Refresh();
}


