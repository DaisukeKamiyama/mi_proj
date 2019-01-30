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

AView_CandidateList

*/

#include "stdafx.h"

#include "AView_CandidateList.h"
#include "AApp.h"

const ANumber	kLeftMargin = 4;
const ANumber	kRightMargin = 4;

/*#717
const ANumber	kFileColumnWidth = 120;

#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kFontSize = 9.0;
#else
const AFloatNumber	kFontSize = 8.0;
#endif
*/

#pragma mark ===========================
#pragma mark [�N���X]AView_CandidateList
#pragma mark ===========================
//���E�C���h�E�̃��C��View

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_CandidateList::AView_CandidateList( /* #199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID ) 
	: AView(/* #199 inParent,inWindow*/inWindowID,inID), mIndex(kIndex_Invalid), mLineHeight(12), mCurrentHoverCursorIndex(kIndex_Invalid)
,mCurrentModeIndex(kIndex_Invalid), mCurrentTextViewControlID(kControlID_Invalid), mCurrentTextWindowID(kObjectID_Invalid)//#717
,mKeywordStartX(0), mTransparency(1.0)
{
	NVMC_SetOffscreenMode(true);//win
	//#92 ��������NVIDO_Init()�ֈړ�
	
	//�f�[�^�S�폜
	SPI_DeleteAllData();
}

//�f�X�g���N�^
AView_CandidateList::~AView_CandidateList()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_CandidateList::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_CandidateList::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>


#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_CandidateList::EVTDO_DoDraw()
{
	//�t���[��rect�擾
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	//=========================�`��f�[�^���擾=========================
	//�t�@�C���񕝎擾
	ANumber fileColumnWidth = (frameRect.right-frameRect.left)/4;
	//�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	NVMC_SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);
	//�ԍ��\�����擾
	AText	digit("0");
	ANumber	numColumnWidth = NVMC_GetDrawTextWidth(digit)*3 + kLeftMargin + kRightMargin;
	
	//=========================�w�i���擾�i����view�ł͔w�i�͕`�悵�Ȃ��j=========================
	//�w�i���擾
	//�T�u�E�C���h�E�p�w�i�F�擾
	AColor	backgroundColor = kColor_White;//GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
	AFloatNumber	backgroundAlpha = 1.0;
	//�T�u�E�C���h�ELocationType�ɂ���āA���ꂼ��̔w�i�`��
	switch(GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()))
	{
	  case kSubWindowLocationType_Popup:
		{
			//�|�b�v�A�b�v�̎w�蓧�ߗ��Ŕw�i����
			//�|�b�v�A�b�v�\�����̃��T�C�Y�ɑΉ����邽�߁Apaint�ɂ���B(erase���ƕ`�悵���̈�ȊO�����ɂȂ��Ă��܂��jNVMC_EraseRect(frameRect);
			backgroundAlpha = GetApp().SPI_GetPopupWindowAlpha();
			break;
		}
	  case kSubWindowLocationType_Floating:
		{
			//�t���[�e�B���O�̎w�蓧�ߗ��Ŕw�i����
			backgroundAlpha = GetApp().SPI_GetFloatingWindowAlpha();
			break;
		}
	  default:
		{
			//�|�b�v�A�b�v�ȊO�Ȃ�T�u�E�C���h�E�w�i�F�ŏ���
			break;
		}
	}
	//�w�ipaint
	//NVMC_PaintRect(frameRect,backgroundColor,backgroundAlpha);
	
	//�f�[�^���ݒ�Ȃ炱���ŏI��
	if( mCurrentModeIndex == kIndex_Invalid )   return;
	
	//==================�F�擾==================
	
	//�`��F�ݒ�
	AColor	letterColor = AColorWrapper::GetColorByHTMLFormatColor("303030");//#1316
	//#1316
	if( GetApp().NVI_IsDarkMode() == true )
	{
		letterColor = AColorWrapper::GetColorByHTMLFormatColor("F0F0F0");
	}
	//#1316 AColor	dropShadowColor = kColor_White;
	/*#1316
	AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
	GetApp().SPI_GetSubWindowBoxColor(NVM_GetWindow().GetObjectID(),letterColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
	*/
    //#1316 NVMC_SetDropShadowColor(dropShadowColor);
	
	//=========================��⍀�ږ��̃��[�v=========================
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		//�L�[���[�h�����̃X�^�C��
		ATextStyle	keywordTextStyle = kTextStyle_Bold;//kTextStyle_Normal;
		//�������\�����邩�ǂ����̃t���O�i���ʎq�ȊO�i�����[�h�ݒ�̃L�[���[�h�j�̏ꍇ�͑S�č������ɂ���j
		ABool	justificationleft = false;//#147
		
		//���ڂ̕`��rect�擾
		AImageRect	imagerect = {0};
		GetItemImageRect(i,imagerect);
		ALocalRect	localItemRect = {0};
		NVM_GetLocalRectFromImageRect(imagerect,localItemRect);
		
		//���ڂ��`��̈�ɓ����Ă��邩�̔���
		if( NVMC_IsRectInDrawUpdateRegion(localItemRect) == false )   continue;
		
		/*#1316 �X�R�[�v�ɂ��F�����̓_�[�N���[�h�ȂǂŌ����Â炭�Ȃ�̂ŁA�K�v���������̂ō폜����B
		//=========================���ڂ̔w�i�`��=========================
		//���ڂ̔w�i�F�擾
		AColor	bgcolor = kColor_White;//#725
		//�D�揇�ʂɂ���Ĕw�i�F�ύX
		ABool	priorityColor = true;
		switch( mPriorityArray.Get(i) )
		{
		  case kCandidatePriority_Local:
			{
				bgcolor = kColor_Red;
				break;
			}
		  case kCandidatePriority_ParentKeywordWithHistory:
			{
				bgcolor = kColor_Blue;
				break;
			}
		  case kCandidatePriority_ParentKeyword:
			{
				bgcolor = kColor_Blue;
				break;
			}
		  case kCandidatePriority_History:
			{
				bgcolor = kColor_Yellow;
				break;
			}
		  default:
			{
				priorityColor = false;
				break;
			}
		}
		//���ڂ̔w�i�`��
		//����ݒ胿�l�ŕ`��i�|�b�v�A�b�v�̏ꍇ�͏��EraseRect()���Ă���̂ŁA�����߂ƂȂ�j
		//NVMC_PaintRect(localItemRect,kColor_White,backgroundAlpha/2);
		//�X�R�[�v���Ƃ̐F�ŕ`��
		if( priorityColor == true )
		{
			NVMC_PaintRect(localItemRect,bgcolor,0.05);
		}
		*/
		
		//=========================�I��`��=========================
		//�I��`��
		/*�I��`��͉��F�ŃL�[���[�h�n�C���C�g���邱�Ƃɂ���
		if( i == mIndex )
		{
			
			AColor	color = kColor_Blue;
			AColorWrapper::GetHighlightColor(color);
			AColor	color2 = AColorWrapper::ChangeHSV(color,0,0.3,1.0);
			NVMC_PaintRectWithVerticalGradient(localItemRect,color2,color,1.0,1.0);
		}
		*/
		//�z�o�[�`��
		if( i == mCurrentHoverCursorIndex )
		{
			AColor	color = kColor_Blue;
			AColorWrapper::GetHighlightColor(color);
			NVMC_PaintRect(localItemRect,color,0.4);
		}
		
		//=========================���ڕ`��f�[�^�擾=========================
		//InfoText�܂���KeywordText��TextDrawData����
		CTextDrawData	textDrawData(false);
		//const AText&	infotext = ((mInfoTextArray.GetTextConst(i).GetItemCount()>0)?(mInfoTextArray.GetTextConst(i)):(mKeywordTextArray.GetTextConst(i)));;
		//�e��f�[�^�擾
		AScopeType	scopeType = mScopeTypeArray.Get(i);
		AText	keywordText;
		mKeywordTextArray.Get(i,keywordText);
		AText	originalInforText;
		mInfoTextArray.Get(i,originalInforText);
		//infotext�ݒ�
		AText	infotext;
		if( originalInforText.GetItemCount() > 0 )
		{
			//���ʎq��InfoText�����݂���ꍇ
			if( scopeType == kScopeType_ModeKeyword )
			{
				//scope:���[�h�L�[���[�h�̏ꍇ�Akeyword : infotext�ɂ���
				infotext.SetText(keywordText);
				infotext.AddCstring(" : ");
				infotext.AddText(originalInforText);
			}
			else
			{
				//infotext��ݒ�
				infotext.SetText(originalInforText);
			}
		}
		else
		{
			//���ʎq��InfoText�����݂��Ȃ��ꍇkeyword��ݒ�
			infotext.SetText(keywordText);
		}
		textDrawData.MakeTextDrawDataWithoutStyle(infotext,1,0,true,true,false,0,infotext.GetItemCount());
		//���[�h�ݒ�X�V�ƃX���b�h���s�Ƃ̂���Ⴂ�̏ꍇ�ɃJ�e�S��index���I�[�o�[����\��������̂ŁA�O�̂��߁B
		AIndex	categoryIndex = mCategoryIndexArray.Get(i);
		if( categoryIndex < 0 || categoryIndex >= GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryCount() )
		{
			categoryIndex = 0;
		}
		//
		//�J�e�S���[�J���[�擾
		//�i�J�e�S��=-1��SPI_SetInfo()�ɂĔr���ς݁j
		AColor	categorycolor = letterColor;
		switch(mScopeTypeArray.Get(i))
		{
		  case kScopeType_Local:
			{
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryLocalColor(categoryIndex,categorycolor);
				break;
			}
		  case kScopeType_Global:
			{
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryColor(categoryIndex,categorycolor);
				break;
			}
		  case kScopeType_Import:
		  case kScopeType_Import_Near:
			{
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryImportColor(categoryIndex,categorycolor);
				break;
			}
		  case kScopeType_ModeKeyword:
		  case kScopeType_ModeSystemHeader:
			{
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryColor(categoryIndex,categorycolor);//��colorslot�Ή�
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		//�J�e�S���F���Â����čʓx���グ���F���擾
		//AColor	categoryColorDrarken = AColorWrapper::ChangeHSV(categorycolor,0,3.0,0.5);
		
		//==================�X�R�[�v�ɉ������f�[�^�擾==================
		AText	filename;
		switch(mScopeTypeArray.Get(i))
		{
		  case kScopeType_Local:
			{
				//"���[�J��"��\��
				filename.SetLocalizedText("Scope_Local");
				break;
			}
		  case kScopeType_Global:
			{
				//filename.SetLocalizedText("Scope_Global");
				//#717 mFileArray.GetObjectConst(i).GetFilename(filename);
				AText	path;
				mFilePathArray.Get(i,path);
				path.GetFilename(filename);
				break;
			}
		  case kScopeType_Import:
		  case kScopeType_Import_Near:
			{
				//#717 mFileArray.GetObjectConst(i).GetFilename(filename);
				AText	path;
				mFilePathArray.Get(i,path);
				path.GetFilename(filename);
				break;
			}
			//�L�[���[�h
		  default:
			{
				//���[�h�ݒ�X�V�ƃX���b�h���s�Ƃ̂���Ⴂ�̏ꍇ�ɃJ�e�S��index���I�[�o�[����\��������̂ŁA�O�̂��߁B
				AIndex	categoryIndex = mCategoryIndexArray.Get(i);
				if( categoryIndex < 0 || categoryIndex >= GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryCount() )
				{
					categoryIndex = 0;
				}
				//
				//�Œ�L�[���[�h�̏ꍇ�A�t�@�C�����̗��ɂ̓J�e�S���[����ݒ肷��
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryName(categoryIndex,filename);
				//�������\��
				justificationleft = true;
				break;
			}
		}
		//==================�`��e�L�X�g�擾==================
		AIndex	idtextspos = 0;
		const AText&	idtext = mKeywordTextArray.GetTextConst(i);
		AItemCount	textlen = infotext.GetItemCount();
		//KeywordText==InfoText���ǂ����̔���
		if( idtext.Compare(infotext) == true )
		{
			//InfoText���݂��Ȃ��̂ŁA�L�[���[�h�݂̂�\��
			textDrawData.attrPos.Add(0);
			textDrawData.attrColor.Add(categorycolor);//categoryColorDrarken);
			textDrawData.attrStyle.Add(keywordTextStyle);
		}
		else
		{
			//InfoText����Ȃ̂ŁAInfoText��\��
			textDrawData.attrPos.Add(0);
			textDrawData.attrColor.Add(letterColor);//kColor_Black);
			textDrawData.attrStyle.Add(kTextStyle_Normal);
			for( AIndex pos = 0; pos < textlen; )
			{
				if( infotext.SkipTabSpace(pos,textlen) == false )   break;
				AText	token;
				AIndex	tokenspos = pos;
				infotext.GetToken(pos,token);//���L��B���̕��@�ł́A���Ƃ���TeX��\�Ŏn�܂�L�[���[�h�ɐF�Â��ł��Ȃ�
				if( token.Compare(idtext) == true )
				{
					idtextspos = tokenspos;
					//�L�[���[�h�������J�e�S���J���[�ŕ`��
					textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos));
					textDrawData.attrColor.Add(categorycolor);//categoryColorDrarken);
					textDrawData.attrStyle.Add(keywordTextStyle);
					textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(pos));
					textDrawData.attrColor.Add(letterColor);//kColor_Black);
					textDrawData.attrStyle.Add(kTextStyle_Normal);
				}
			}
		}
		//=========================�t�@�C�����`��=========================
		//�t�@�C�����̗��Ɂu���v��\��
		if( mMatchedCountArray.Get(i) > 1 )
		{
			AText	andsoon;
			andsoon.SetLocalizedText("CandidateList_AndSoOn");
			filename.AddCstring(" ");
			filename.AddText(andsoon);
		}
		//�D�揇�ʂɂ���Ă̓t�@�C�����̗��ɐe�L�[���[�h����\��
		switch( mPriorityArray.Get(i) )
		{
		  case kCandidatePriority_ParentKeyword:
		  case kCandidatePriority_ParentKeywordWithHistory:
			{
				filename.SetLocalizedText("Letter_ParentKeyword_Matched");
				//
				filename.AddText(mParentKeywordArray.GetTextConst(i));
				AText	t;
				t.SetLocalizedText("CandidateList_ParentKeywordMatched");
				filename.AddText(t);
				break;
			}
		  case kCandidatePriority_History:
			{
				AText	t;
				t.SetLocalizedText("CandidateList_FoundInHistory");
				filename.AddText(t);
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		//�t�@�C�����`��i1�|�C���g�����������ŕ`��j
		ALocalRect	drawRect = localItemRect;
		drawRect.left = localItemRect.right - fileColumnWidth + kLeftMargin;
		drawRect.right = localItemRect.right - kRightMargin;
		drawRect.top += 1;
		NVMC_SetDefaultTextProperty(fontname,fontsize-1.0,letterColor,kTextStyle_Normal,true);
		NVMC_DrawText(drawRect,filename);
		
		//=========================�������`��=========================
		//������
		ALocalPoint	spt = {0}, ept = {0};
		spt.x = localItemRect.right - fileColumnWidth;
		spt.y = drawRect.top;
		ept.x = localItemRect.right - fileColumnWidth;
		ept.y = drawRect.bottom;
		NVMC_DrawLine(spt,ept,kColor_LightGray,1.0,1.0);
		
		//=========================�e�L�X�g�`��=========================
		//
		NVMC_SetDefaultTextProperty(fontname,fontsize,letterColor,kTextStyle_Normal,true);
		//�`��̈�v�Z
		drawRect = localItemRect;
		drawRect.left = localItemRect.left + numColumnWidth + kLeftMargin;
		drawRect.right = localItemRect.right - fileColumnWidth - kRightMargin;
		//
		mKeywordStartX = drawRect.left;
		//
		ALocalRect	textRect = drawRect;
		textRect.top += 1;
		if( justificationleft == false )//#147
		{
			//==================���ʎq�palign�i���ʎq�����̓������킹�Ē����t�߂ɕ\���j==================
			//
			mKeywordStartX = drawRect.left + (drawRect.right-drawRect.left)/5;
			//
			AText	t;
			infotext.GetText(0,idtextspos,t);
			ANumber	tw = static_cast<ANumber>(NVMC_GetDrawTextWidth(t));
			textRect.left = mKeywordStartX - tw;
		}
		//�I�����ڂ̃L�[���[�h���������F�n�C���C�g
		if( i == mIndex )
		{
			//�J���[�X���b�g�̑I��F�s�����x���擾���A����ɏ]���ăn�C���C�g�Z�x�ݒ�
			AFloatNumber	selectionOpacity = GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetModeData_Number(AModePrefDB::kSelectionOpacity);
			selectionOpacity /= 100;
			//
			ALocalRect	rect = textRect;
			rect.left = mKeywordStartX;
			rect.right = mKeywordStartX + NVMC_GetDrawTextWidth(keywordText);
			AFloatNumber alpha = 0.3*selectionOpacity;//#1316
			if( GetApp().NVI_IsDarkMode() == false )
			{
				//�_�[�N���[�h�ł͂Ȃ��ꍇ�̂݁A�A���t�@��+0.2����
				alpha += 0.2;
			}
			NVMC_PaintRect(rect,kColor_Yellow,alpha);
		}
		//�e�L�X�g�`��
		NVMC_DrawText(textRect,drawRect,textDrawData);
		
		//=========================�������`��=========================
		//������
		spt.x = localItemRect.left + numColumnWidth;
		spt.y = drawRect.top;
		ept.x = localItemRect.left + numColumnWidth;
		ept.y = drawRect.bottom;
		NVMC_DrawLine(spt,ept,kColor_LightGray,1.0,1.0);
		
		//==================�ԍ��`��==================
		drawRect = localItemRect;
		drawRect.left = localItemRect.left + kLeftMargin;
		drawRect.right = localItemRect.left + numColumnWidth - kRightMargin;
		drawRect.top += 1;
		NVMC_SetDefaultTextProperty(fontname,fontsize-1.0,letterColor,kTextStyle_Normal,true);
		AText	numtext;
		numtext.SetNumber(i+1);
		NVMC_DrawText(drawRect,numtext,kJustification_Right);
	}
	/*
	for( AIndex lineIndex = 0; lineIndex < mDrawData_LineStart.GetItemCount(); lineIndex++ )
	{
		AIndex	start 	= mDrawData_LineStart .Get(lineIndex);
		AItemCount	len = mDrawData_LineLength.Get(lineIndex);
		AText	text;
		mText.GetText(start,len,text);
		ALocalRect	rect;
		rect.left		= kLeftMargin;
		rect.top		= lineIndex*fontheight;
		rect.right		= viewwidth;
		rect.bottom		= lineIndex*fontheight + fontheight;
		//
		NVMC_DrawText(rect,text);
	}
	*/
}

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_CandidateList::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//TextWindowID, TextView��ControlID��invalid�Ȃ牽�����Ȃ�
	if( mCurrentTextWindowID == kObjectID_Invalid || mCurrentTextViewControlID == kControlID_Invalid )
	{
		return false;
	}
	//�E�C���h�E���񑶍݂Ȃ牽�����Ȃ�
	if( GetApp().NVI_ExistWindow(mCurrentTextWindowID) == false )
	{
		return false;
	}
	//
	AImagePoint	imagept;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	AIndex	index = imagept.y / mLineHeight;
	if( index >= 0 && index < mInfoTextArray.GetItemCount() )
	{
		GetApp().SPI_GetTextWindowByID(mCurrentTextWindowID).SPI_SetCandidateIndex(mCurrentTextViewControlID,index);
	}
	return true;
}

//
void	AView_CandidateList::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
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

ABool	AView_CandidateList::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	return false;
}

//�J�[�\��
ABool	AView_CandidateList::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
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

void	AView_CandidateList::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearHoverCursor();
}

void	AView_CandidateList::ClearHoverCursor()
{
	if( mCurrentHoverCursorIndex != kIndex_Invalid )
	{
		mCurrentHoverCursorIndex = kIndex_Invalid;
		NVI_Refresh();
	}
}

/**
���ڂ�imageRect���擾
*/
void	AView_CandidateList::GetItemImageRect( const AIndex inIndex, AImageRect& outImageRect ) const
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	outImageRect.left		= 0;
	outImageRect.top		= inIndex*mLineHeight;
	outImageRect.right		= frameRect.right - frameRect.left;
	outImageRect.bottom		= (inIndex+1)*mLineHeight;
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

void	AView_CandidateList::SPI_SetInfo( const AModeIndex inModeIndex,
		const AWindowID inTextWindowID, const AControlID inTextViewControlID,
		const ATextArray& inIdTextArray, const ATextArray& inInfoTextArray,
		const AArray<AIndex>& inCategoryIndexArray, const AArray<AScopeType>& inScopeTypeArray,
		const ATextArray& inFilePathArray, const AArray<ACandidatePriority>& inPriorityArray,
		const ATextArray& inParentKeywordArray,
		const AArray<AItemCount>& inMatchedCountArray )//#717
{
	//inCandidateArray.Implode(kUChar_Space,mText);
	
	NVMC_SetRedirectTextInput(inTextWindowID,inTextViewControlID);
	
	mCurrentModeIndex = inModeIndex;
	mCurrentTextWindowID = inTextWindowID;
	mCurrentTextViewControlID = inTextViewControlID;
	
	mInfoTextArray.DeleteAll();
	mKeywordTextArray.DeleteAll();
	mScopeTypeArray.DeleteAll();
	mCategoryIndexArray.DeleteAll();
	//#717 mFileArray.DeleteAll();
	mFilePathArray.DeleteAll();//#717
	mPriorityArray.DeleteAll();//#717
	mMatchedCountArray.DeleteAll();
	mParentKeywordArray.DeleteAll();
	mInfoTextIsEmpty = true;
	for( AIndex i = 0; i < inInfoTextArray.GetItemCount(); i++ )
	{
		//�J�e�S��=-1�̏ꍇ(ImportIdentifier(#include��)�j�͒ǉ����Ȃ�
		if( inCategoryIndexArray.Get(i) == kIndex_Invalid )
		{
			continue;
		}
		//info text�擾
		AText	infoText;
		inInfoTextArray.Get(i,infoText);
		infoText.ReplaceChar(kUChar_CR,kUChar_Space);
		infoText.ReplaceChar(kUChar_Tab,kUChar_Space);
		mInfoTextArray.Add(infoText);
		if( infoText.GetItemCount() > 0 )   mInfoTextIsEmpty = false;
		//keyword text�擾
		AText	keywordText;
		inIdTextArray.Get(i,keywordText);
		keywordText.ReplaceChar(kUChar_CR,kUChar_Space);
		keywordText.ReplaceChar(kUChar_Tab,kUChar_Space);
		mKeywordTextArray.Add(keywordText);
		//scope, category, file path�擾
		mScopeTypeArray.Add(inScopeTypeArray.Get(i));
		mCategoryIndexArray.Add(inCategoryIndexArray.Get(i));
		//#717 mFileArray.GetObject(mFileArray.AddNewObject()).CopyFrom(inFileArray.GetObjectConst(i));
		mFilePathArray.Add(inFilePathArray.GetTextConst(i));//#717
		mPriorityArray.Add(inPriorityArray.Get(i));//#717
		mMatchedCountArray.Add(inMatchedCountArray.Get(i));
		mParentKeywordArray.Add(inParentKeywordArray.GetTextConst(i));//#717
	}
	mIndex = kIndex_Invalid;//#717
	CalcDrawData();
	NVI_Refresh();
}

/**
�f�[�^�S�폜
*/
void	AView_CandidateList::SPI_DeleteAllData()
{
	//���X���b�h�ł̌������f�K�v�H
	
	mIndex = kIndex_Invalid;
	mCurrentModeIndex = kIndex_Invalid;
	mCurrentTextWindowID = kObjectID_Invalid;
	mCurrentTextViewControlID = kControlID_Invalid;
	
	mInfoTextArray.DeleteAll();
	mKeywordTextArray.DeleteAll();
	mScopeTypeArray.DeleteAll();
	mCategoryIndexArray.DeleteAll();
	mFilePathArray.DeleteAll();//#717
	mPriorityArray.DeleteAll();//#717
	mMatchedCountArray.DeleteAll();
	mParentKeywordArray.DeleteAll();//#717
	mInfoTextIsEmpty = true;
	
	mCurrentHoverCursorIndex = kIndex_Invalid;
	
	CalcDrawData();
	NVI_Refresh();
}

//
void	AView_CandidateList::SPI_SetIndex( const AIndex inIndex )
{
	mIndex = inIndex;
	AdjustScroll();
	NVI_Refresh();
	NVMC_FlushControl();//win
	//�X�e�[�^�X�o�[�X�V
	GetApp().SPI_GetCandidateListWindow(NVM_GetWindow().GetObjectID()).SPI_UpdateStatusBar();
}

void	AView_CandidateList::AdjustScroll()
{
	ALocalRect	frameRect;
	NVM_GetLocalViewRect(frameRect);
	AImageRect	imagerect;
	NVM_GetImageRectFromLocalRect(frameRect,imagerect);
	AIndex	startIndex = imagerect.top/mLineHeight;
	AIndex	endIndex = imagerect.bottom/mLineHeight;
	if( mIndex != kIndex_Invalid )
	{
		if( mIndex <= startIndex || endIndex <= mIndex )
		{
			AImagePoint	pt = {0,0};
			pt.y = mIndex*mLineHeight - (frameRect.bottom - frameRect.top)/2;
			if( pt.y < 0 )   pt.y = 0;
			NVI_ScrollTo(pt);
		}
	}
}

void	AView_CandidateList::CalcDrawData()
{
	/*mDrawData_LineStart.DeleteAll();
	mDrawData_LineLength.DeleteAll();
	//
	AText	fixwidthfontname;
	AFontWrapper::GetFixWidthFontName(fixwidthfontname);
	CTextDrawData	textDrawData;
	textDrawData.MakeTextDrawDataWithoutStyle(mText,
			4,
			0,true,true,false,0,mText.GetItemCount());
	NVMC_SetDefaultTextProperty(fixwidthfontname,kFontSize,kColor_Black,kTextStyle_Normal,true);
	ANumber	fontheight, fontascent;
	NVMC_GetMetricsForDefaultTextProperty(fontheight,fontascent);
	ANumber	viewwidth = NVI_GetViewWidth();
	//�s�܂�Ԃ��v�Z
	AIndex	startOffset = 0;
	ANumber	lineleftmargin = kLeftMargin;
	AItemCount	lineCount = 0;
	while( textDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - startOffset > 0 )
	{
		AItemCount	endOffset = CWindowImp::GetLineBreak(textDrawData,startOffset,fixwidthfontname,kFontSize,true,viewwidth-lineleftmargin);
		if( endOffset <= startOffset )   endOffset = startOffset+1;
		mDrawData_LineStart .Add(textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(startOffset));
		mDrawData_LineLength.Add(textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(endOffset)-textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(startOffset));
		startOffset = endOffset;
		lineCount++;
	}*/
	//�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	
	ANumber	viewwidth = NVI_GetViewWidth();
	NVMC_SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);
	ANumber	fontheight, fontascent;
	NVMC_GetMetricsForDefaultTextProperty(fontheight,fontascent);
	fontheight += 1;
	mLineHeight = fontheight;
	//ImageSize�ݒ�
	NVM_SetImageSize(viewwidth,mInfoTextArray.GetItemCount()*fontheight+5);
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(0,fontheight,0,rect.bottom-rect.top-fontheight);
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
}

void	AView_CandidateList::SPI_UpdateDrawProperty()
{
	CalcDrawData();
	NVI_Refresh();
}



