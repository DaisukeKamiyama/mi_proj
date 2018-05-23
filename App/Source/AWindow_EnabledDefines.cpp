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

AWindow_EnabledDefines
#467

*/

#include "stdafx.h"

#include "AWindow_EnabledDefines.h"
#include "AApp.h"

//
const AControlID		kDefineList						= 100;
const AControlID		kDefineEditBox		 			= 101;
const AControlID		kValueEditBox					= 102;
const AControlID		kAddButton						= 103;
const AControlID		kDeleteButton					= 104;
const AControlID		kUpdateDocumentButton			= 105;

const AIndex			kColumn_DefineList = 0;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_EnabledDefines
#pragma mark ===========================
//�����E�C���h�E

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^

//�R���X�g���N�^
AWindow_EnabledDefines::AWindow_EnabledDefines():AWindow()
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kEnabledDefinesWindowPosition);
	//
	NVI_AddToRotateArray();
	//
	NVI_AddToTimerActionWindowArray();
}

//�f�X�g���N�^
AWindow_EnabledDefines::~AWindow_EnabledDefines()
{
}

/**
�폜������
*/
void	AWindow_EnabledDefines::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�N���b�N������
*/
ABool	AWindow_EnabledDefines::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kAddButton:
		{
			//define�G�f�B�b�g�{�b�N�X����e�L�X�g�擾�i�X�y�[�X�폜�j
			AText	text;
			NVI_GetControlText(kDefineEditBox,text);
			AText	defineText;
			text.GetTextWithoutSpaces(defineText);
			//define�l�G�f�B�b�g�{�b�N�X����e�L�X�g�擾�i�X�y�[�X�폜�j
			NVI_GetControlText(kValueEditBox,text);
			AText	valueText;
			text.GetTextWithoutSpaces(valueText);
			//���łɑ��݂���define�Ȃ�l�ύX�A�����łȂ���ΐV�K�ǉ�
			AIndex	index = GetApp().NVI_GetAppPrefDB().Find_TextArray(AAppPrefDB::kDirectiveEnabledDefine,defineText);
			if( index == kIndex_Invalid )
			{
				GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kDirectiveEnabledDefine,defineText);
				GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,valueText);
			}
			else
			{
				GetApp().NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kDirectiveEnabledDefine,index,defineText);
				GetApp().NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,index,valueText);
			}
			UpdateList();
			break;
		}
	  case kDeleteButton:
		{
			//�I�����擾
			AArray<AIndex>	selectedDBIndexArray;
			NVI_GetListView(kDefineList).SPI_GetSelectedDBIndexArray(selectedDBIndexArray);
			//���Ԃɕ��ׂ����ƂŁA������폜
			selectedDBIndexArray.Sort(true);
			for( AIndex i = selectedDBIndexArray.GetItemCount()-1; i >= 0; i-- )
			{
				GetApp().NVI_GetAppPrefDB().Delete1_TextArray(AAppPrefDB::kDirectiveEnabledDefine,selectedDBIndexArray.Get(i));
				GetApp().NVI_GetAppPrefDB().Delete1_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,selectedDBIndexArray.Get(i));
			}
			UpdateList();
			break;
		}
	  case kDefineList:
		{
			//�I�����擾
			AIndex	sel = NVI_GetListView(kDefineList).SPI_GetSelectedDBIndex();
			if( sel != kIndex_Invalid )
			{
				//�e�L�X�g�{�b�N�X�X�V
				AText	text;
				GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefine,sel,text);
				NVI_SetControlText(kDefineEditBox,text);
				GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,sel,text);
				NVI_SetControlText(kValueEditBox,text);
			}
			break;
		}
	  case kUpdateDocumentButton:
		{
			//���@�ĔF��
			GetApp().SPI_ReRecognizeSyntaxAll(kIndex_Invalid);
			break;
		}
	}
	return result;
}

//
/**
�R���g���[���l�ύX������
*/
ABool	AWindow_EnabledDefines::EVTDO_ValueChanged( const AControlID inControlID )
{
	ABool	result = true;
	switch(inControlID)
	{
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

/**
�R���g���[����ԁiEnable/Disable���j���X�V
NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()����R�[�������
�N���b�N��A�N�e�B�x�[�g�ŃR�[�������̂ŁA���܂�d��������NVMDO_UpdateControlStatus()�ɂ͓���Ȃ����ƁB
*/
void	AWindow_EnabledDefines::NVMDO_UpdateControlStatus()
{
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

/**
�E�C���h�E����
*/
void AWindow_EnabledDefines::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/EnabledDefines");
	
	NVI_CreateListViewFromResource(kDefineList);
	NVI_GetListView(kDefineList).SPI_CreateFrame();
	NVI_GetListView(kDefineList).NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_Invalid);
	NVI_GetListView(kDefineList).SPI_RegisterColumn_Text(kColumn_DefineList,0,"",false);
	NVI_GetListView(kDefineList).SPI_SetEnableDragDrop(true);
	NVI_GetListView(kDefineList).SPI_SetRowMargin(5);
	NVI_GetListView(kDefineList).SPI_SetMultiSelectEnabled(true);
	
	NVI_CreateEditBoxView(kDefineEditBox);
	NVI_CreateEditBoxView(kValueEditBox);
	
	NVI_SetDefaultOKButton(kAddButton);
	
	UpdateList();
}

/**
���X�g�X�V
*/
void	AWindow_EnabledDefines::UpdateList()
{
	//���X�g�̃e�L�X�g����
	ATextArray	textArray;
	for( AIndex i = 0; i < GetApp().NVI_GetAppPrefDB().GetItemCount_Array(AAppPrefDB::kDirectiveEnabledDefine); i++ )
	{
		AText	defineText;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefine,i,defineText);
		AText	valueText;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,i,valueText);
		AText	text;
		text.AddText(defineText);
		if( valueText.GetItemCount() > 0 )
		{
			text.AddCstring(" = ");
			text.AddText(valueText);
		}
		textArray.Add(text);
	}
	//���X�g�X�V
	//#1012 NVI_GetListView(kDefineList).SPI_SetEnableIcon(false);
	NVI_GetListView(kDefineList).SPI_BeginSetTable();
	NVI_GetListView(kDefineList).SPI_SetColumn_Text(kColumn_DefineList,textArray);
	NVI_GetListView(kDefineList).SPI_EndSetTable();
}

/**
define�G�f�B�b�g�{�b�N�X�Ƀe�L�X�g�ݒ�
*/
void	AWindow_EnabledDefines::SPI_SetDefineEditBox( const AText& inText )
{
	//�X�y�[�X�n�������폜
	AText	defineText;
	inText.GetTextWithoutSpaces(defineText);
	//���݂���define�Ȃ�l���擾�A�����łȂ����1���f�t�H���g�Ƃ��Đݒ�
	AIndex	index = GetApp().NVI_GetAppPrefDB().Find_TextArray(AAppPrefDB::kDirectiveEnabledDefine,defineText);
	AText	valueText("1");
	if( index != kIndex_Invalid )
	{
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,index,valueText);
	}
	NVI_SetControlText(kDefineEditBox,defineText);
	NVI_SetControlText(kValueEditBox,valueText);
	
	//�e�L�X�g�{�b�N�X��S�I�����ăt�H�[�J�X�ړ�
	NVI_GetEditBoxView(kValueEditBox).NVI_SetSelectAll();
	NVI_SwitchFocusTo(kValueEditBox);
}
