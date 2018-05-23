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

AView_List_FusenList
#138

*/

#include "stdafx.h"

#include "AView_List_FusenList.h"
#include "AApp.h"

const ADataID			kColumn_LineIndex				= 0;
const ADataID			kColumn_ModuleName				= 1;
const ADataID			kColumn_Comment					= 2;

#pragma mark ===========================
#pragma mark [�N���X]AView_List_FusenList
#pragma mark ===========================
//���E�C���h�E�̃��C��View

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AView_List_FusenList::AView_List_FusenList( const AWindowID inWindowID, const AControlID inID  ) 
	: AView_List(inWindowID,inID), mProjectObjectID(kObjectID_Invalid)
{
}

/**
������
*/
void	AView_List_FusenList::NVIDO_ListInit()
{
	//�J�[�\���ݒ�
	NVI_SetDefaultCursor(kCursorType_Arrow);
	//�t�H���g���ݒ�
	AText	fontname;
	AFontWrapper::GetAppDefaultFontName(fontname);//#375
	SPI_SetTextDrawProperty(fontname,10.0,kColor_Black);
	SPI_SetWheelScrollPercent(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent),
			GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd));
	//���X�g�ݒ�
	SPI_RegisterColumn_Number(kColumn_LineIndex,0,"",false);
	SPI_RegisterColumn_Text(kColumn_ModuleName,250,"",false);
	SPI_RegisterColumn_Text(kColumn_Comment,0,"",false);
	SPI_SetEnableCursorRow();
}

/**
�f�X�g���N�^
*/
AView_List_FusenList::~AView_List_FusenList()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_List_FusenList::EVTDO_ListClicked( const AIndex inClickedRowIndex, const AIndex inClickedRowDBIndex, const ANumber inClickCount )
{
	if( inClickedRowDBIndex == kIndex_Invalid )   return;
	
	GetApp().SPI_RevealExternalComment(mProjectObjectID,mRelativePath,
			mModuleNameArray.GetTextConst(inClickedRowDBIndex),mOffsetArray.Get(inClickedRowDBIndex));
	
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---

/**
�A�N�e�B�u�h�L�������g�ύX

�\�����ׂ��Ώۂ̃h�L�������g���ύX���ꂽ�Ƃ��ɃR�[�������B
@param inProjectObjectID �A�N�e�B�u�ɂȂ����h�L�������g��ProjectObjectID
@param inRelativePath �A�N�e�B�u�ɂȂ����h�L�������g�̃v���W�F�N�g���΃p�X
*/
void	AView_List_FusenList::SPI_ChangeActiveDocument( const AObjectID inProjectObjectID, const AText& inRelativePath )
{
	//���݂̃A�N�e�B�u�h�L�������g�Ɠ����Ȃ牽�����Ȃ�
	if( inProjectObjectID == mProjectObjectID && inRelativePath.Compare(mRelativePath) == true )   return;
	
	//�A�N�e�B�u�h�L�������g�X�V
	mProjectObjectID = inProjectObjectID;
	mRelativePath.SetText(inRelativePath);
	//�e�[�u���X�V
	UpdateTable();
}

/**
���X�g�X�V

�tⳎ��f�[�^���X�V���ꂽ�ꍇ�ȂǂɃR�[�������
@param inProjectObjectID �X�V�Ώۃh�L�������g��ProjectObjectID
@param inRelativePath �X�V�Ώۃh�L�������g�̃v���W�F�N�g���΃p�X
*/
void	AView_List_FusenList::SPI_UpdateTable( const AObjectID inProjectObjectID, const AText& inRelativePath )
{
	//�X�V�Ώۃh�L�������g���A���݂̃A�N�e�B�u�h�L�������g�Ɠ����ꍇ�̂݃e�[�u���X�V
	if( inProjectObjectID == mProjectObjectID && inRelativePath.Compare(mRelativePath) == true )
	{
		UpdateTable();
	}
}

/**
���X�g�X�V�i�������W���[���j
*/
void	AView_List_FusenList::UpdateTable()
{
	//�tⳎ��f�[�^�擾
	
	//DocID�擾
	ADocumentID	docID = GetApp().SPI_GetDocumentIDByProjectObjectIDAndPath(mProjectObjectID,mRelativePath,false);//�Y���Ȃ��Ȃ�V�Kdoc�쐬���Ȃ�
	if( docID == kObjectID_Invalid )   return;
	
	//�h�L�������g����f�[�^�擾
	ATextArray	commentArray;
	ANumberArray	lineIndexArray;
	GetApp().SPI_GetTextDocumentByID(docID).SPI_GetExternalCommentArray(mModuleNameArray,mOffsetArray,commentArray,lineIndexArray);
	
	//"���W���[�����{�I�t�Z�b�g"�̕�����z��𐶐�
	ATextArray	posArray;
	for( AIndex i = 0; i < mModuleNameArray.GetItemCount(); i++ )
	{
		AText	text;
		text.SetText(mModuleNameArray.GetTextConst(i));
		text.AddCstring(" + ");
		text.AddNumber(mOffsetArray.Get(i));
		posArray.Add(text);
	}
	
	//�e�[�u���X�V
	SPI_SetSelect(kIndex_Invalid);
	SPI_BeginSetTable();
	SPI_SetColumn_Number(kColumn_LineIndex,lineIndexArray);
	SPI_SetColumn_Text(kColumn_ModuleName,posArray);
	SPI_SetColumn_Text(kColumn_Comment,commentArray);
	SPI_EndSetTable();
	SPI_Sort(kColumn_LineIndex,true);
	
	//�\���X�V
	NVI_Refresh();
}

