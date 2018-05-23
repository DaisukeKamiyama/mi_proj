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

AWindow_TextCount

*/

#include "stdafx.h"

#include "AWindow_TextCount.h"
#include "AApp.h"
//#include "AUtil.h"
#include "AView_Text.h"

const AControlID	kControlID_UpdateButton = 1;
const AControlID	kControlID_LetterCount = 200;
const AControlID	kControlID_LineCount = 202;
const AControlID	kRadioButton_Z2H1 = 300;
const AControlID	kRadioButton_Z1H05 = 301;
const AControlID	kRadioButton_Z1H1 = 302;
const AControlID	kCheck_CountSpaceTab = 303;
const AControlID	kCheck_CountReturn = 304;

const AControlID	kLetterCount = 100;
const AControlID	kCommentPercent = 101;
const AControlID	kParagraphCount = 102;
const AControlID	kSelectedLetterCount = 103;
const AControlID	kSelectedCommentPercent = 104;
const AControlID	kSelectedParagraphCount = 105;
const AControlID	kDocumentTitle = 150;
const AControlID	kLineCountWithCondition = 201;
const AControlID	kPageCountWithCondition = 203;


#pragma mark ===========================
#pragma mark [�N���X]AWindow_TextCount
#pragma mark ===========================
//

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_TextCount::AWindow_TextCount():AWindow(/*#175NULL*/)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kTextCountWindowPosition);
}
//�f�X�g���N�^
AWindow_TextCount::~AWindow_TextCount()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�R���g���[���̃N���b�N���̃R�[���o�b�N
ABool	AWindow_TextCount::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kControlID_UpdateButton:
		{
			SPI_Update();
			break;
		}
	}
	return result;
}

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow_TextCount::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	if( NVI_IsWindowCreated() == false )   return;
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_TextCount::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/TextCount");
	//#688 NVI_CreateEditBoxView(kControlID_LetterCount);
	//#688 NVI_CreateEditBoxView(kControlID_LineCount);
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadioButton_Z2H1);
	NVI_AddToLastRegisteredRadioGroup(kRadioButton_Z1H05);
	NVI_AddToLastRegisteredRadioGroup(kRadioButton_Z1H1);
	NVI_SetDefaultOKButton(kControlID_UpdateButton);
	NVI_RegisterToFocusGroup(kCheck_CountSpaceTab,true);//#353
	NVI_RegisterToFocusGroup(kCheck_CountReturn,true);//#353
	NVI_RegisterToFocusGroup(kControlID_UpdateButton,true);//#353
	
	//
	AText	text;
	text.SetFormattedCstring("%d",GetApp().GetAppPref().GetData_Number(AAppPrefDB::kTextCountConditionLetterCount));
	NVI_SetControlText(kControlID_LetterCount,text);
	text.SetFormattedCstring("%d",GetApp().GetAppPref().GetData_Number(AAppPrefDB::kTextCountConditionLineCount));
	NVI_SetControlText(kControlID_LineCount,text);
	//
	switch(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kTextCountCountAs2Letters))
	{
	  case 0:
	  default:
		{
			NVI_SetControlBool(kRadioButton_Z2H1,true);
			break;
		}
	  case 1:
		{
			NVI_SetControlBool(kRadioButton_Z1H05,true);
			break;
		}
	  case 2:
		{
			NVI_SetControlBool(kRadioButton_Z1H1,true);
			break;
		}
	}
	//
	NVI_SetControlBool(kCheck_CountSpaceTab,GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kTextCountCountSpaceTab));
	NVI_SetControlBool(kCheck_CountReturn,GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kTextCountCountReturn));
}

//Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
void	AWindow_TextCount::NVIDO_Hide()
{
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kTextCountConditionLetterCount,NVI_GetControlNumber(kControlID_LetterCount));
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kTextCountConditionLineCount,NVI_GetControlNumber(kControlID_LineCount));
	if( NVI_GetControlBool(kRadioButton_Z2H1) == true )
	{
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kTextCountCountAs2Letters,0);
	}
	else if( NVI_GetControlBool(kRadioButton_Z1H05) == true )
	{
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kTextCountCountAs2Letters,1);
	}
	else
	{
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kTextCountCountAs2Letters,2);
	}
	GetApp().GetAppPref().SetData_Bool(AAppPrefDB::kTextCountCountSpaceTab,NVI_GetControlBool(kCheck_CountSpaceTab));
	GetApp().GetAppPref().SetData_Bool(AAppPrefDB::kTextCountCountReturn,NVI_GetControlBool(kCheck_CountReturn));
}

void	AWindow_TextCount::SPI_Update()
{
	ADocumentID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID == kObjectID_Invalid )   return;
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
	AArray<ATextIndex>	sposarray, eposarray;
	GetApp().SPI_GetTextWindowByID(winID).SPI_GetSelect(docID,sposarray,eposarray);
	
	AItemCount	ratio = 1;
	//
	ATextCountData	data;
	data.countSpaceTab = NVI_GetControlBool(kCheck_CountSpaceTab);
	data.countReturn = NVI_GetControlBool(kCheck_CountReturn);
	if( NVI_GetControlBool(kRadioButton_Z2H1) == true )
	{
		data.countAs2Letters = true;
	}
	else if( NVI_GetControlBool(kRadioButton_Z1H05) == true )
	{
		data.countAs2Letters = true;
		ratio = 2;
	}
	else
	{
		data.countAs2Letters = false;
	}
	data.conditionLetterCount = NVI_GetControlNumber(kControlID_LetterCount);
	if( data.conditionLetterCount < 5 )   data.conditionLetterCount = 5;
	NVI_SetControlNumber(kControlID_LetterCount,data.conditionLetterCount);
	data.conditionLetterCount *= ratio;
	AItemCount	lineCountInPage = NVI_GetControlNumber(kControlID_LineCount);
	//#497
	if( lineCountInPage < 1 )
	{
		lineCountInPage = 1;
		NVI_SetControlNumber(kControlID_LineCount,lineCountInPage);
	}
	//�J�E���g���s
	GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextCount(data,sposarray,eposarray);
	//
	AText	text;
	AFloatNumber	fnum;
	//�t�@�C����
	GetApp().SPI_GetTextDocumentByID(docID).NVI_GetTitle(text);
	NVI_SetControlText(kDocumentTitle,text);
	//������
	fnum = data.letterCount;
	fnum /= ratio;
	text.SetFormattedCstring("%g",fnum);
	NVI_SetControlText(kLetterCount,text);
	//�i����
	text.SetFormattedCstring("%d",data.paragraphCount);
	NVI_SetControlText(kParagraphCount,text);
	//�R�����g��
	fnum = data.commentLetterCount;
	if( data.letterCount == 0 )
	{
		text.SetCstring("-");
	}
	else if( data.commentLetterCount == kIndex_Invalid )//#695 //��
	{
		text.SetCstring("-");
	}
	else
	{
		fnum = fnum*100/data.letterCount;
		text.SetFormattedCstring("%.1f",fnum);
	}
	NVI_SetControlText(kCommentPercent,text);
	//�������i�I��͈́j
	fnum = data.letterCountInSelect;
	fnum /= ratio;
	text.SetFormattedCstring("%g",fnum);
	NVI_SetControlText(kSelectedLetterCount,text);
	//�i�����i�I��͈́j
	text.SetFormattedCstring("%d",data.paragraphCountInSelect);
	NVI_SetControlText(kSelectedParagraphCount,text);
	//�R�����g���i�I��͈́j
	fnum = data.commentLetterCountInSelect;
	if( data.letterCountInSelect == 0 )
	{
		text.SetCstring("-");
	}
	else if( data.commentLetterCountInSelect == kIndex_Invalid )//#695 //��
	{
		text.SetCstring("-");
	}
	else
	{
		fnum = fnum*100/data.letterCountInSelect;
		text.SetFormattedCstring("%.1f",fnum);
	}
	NVI_SetControlText(kSelectedCommentPercent,text);
	//�P�s�������Ȃ�`�s
	text.SetFormattedCstring("%d",data.lineCountWithCondition);
	NVI_SetControlText(kLineCountWithCondition,text);
	text.SetFormattedCstring("%d",(data.lineCountWithCondition + lineCountInPage-1) / lineCountInPage);
	NVI_SetControlText(kPageCountWithCondition,text);
}

