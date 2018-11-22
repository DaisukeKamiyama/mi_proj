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

AWindow_ChangeToolButton

*/

#include "stdafx.h"

#include "AWindow_ChangeToolButton.h"
#include "AApp.h"

//�R���g���[��ID
const AControlID	kToolName				= 5200;
const AControlID	kOK						= 5001;
const AControlID	kCancel					= 5002;

const ANumber	kButtonWidth = 24;//win
const ANumber	kButtonHeight = 24;//win

#pragma mark ===========================
#pragma mark [�N���X]AWindow_ChangeToolButton
#pragma mark ===========================
//���ݒ�E�C���h�E

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_ChangeToolButton::AWindow_ChangeToolButton( const AWindowID inParentWindowID ) 
		: AWindow(),mParentWindowID(inParentWindowID),mModeIndex(kStandardModeIndex)
{
}
//�f�X�g���N�^
AWindow_ChangeToolButton::~AWindow_ChangeToolButton()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

/**
�eWindow�擾
*/
AWindow&	AWindow_ChangeToolButton::GetParentWindow()
{
	return AApplication::GetApplication().NVI_GetWindowByID(mParentWindowID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
*/
ABool	AWindow_ChangeToolButton::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kOK:
		{
			//�c�[���t�H���_�擾
			AFileAcc	toolbarFolder;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarFolder(toolbarFolder);
			//UI����t�@�C�����擾
			AText	filename;
			NVI_GetControlText(kToolName,filename);
			//�c�[���o�[���ږ��̕ύX
			AFileAcc	newfile;
			GetApp().SPI_GetModePrefDB(mModeIndex).SetToolbarName(mFile,filename,newfile);
			//����
			NVI_Close();
			break;
		}
	  case kCancel:
		{
			NVI_Close();
			break;
		}
	}
	return result;
}

/**
*/
void	AWindow_ChangeToolButton::EVTDO_TextInputted( const AControlID inID )
{
	switch(inID)
	{
	  case kToolName:
		{
			NVM_UpdateControlStatus();
			break;
		}
	}
}

/**
�E�C���h�E����{�^��
*/
void	AWindow_ChangeToolButton::EVTDO_DoCloseButton()
{
	NVI_Close();
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void AWindow_ChangeToolButton::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVI_SetFixTitle(true);
	NVM_CreateWindow("main/ChangeToolButton");
	
	NVI_RegisterToFocusGroup(kToolName);
	NVI_SetWindowStyleToSheet(GetParentWindow().GetObjectID());
	NVI_SetDefaultOKButton(kOK);
	NVI_SetDefaultCancelButton(kCancel);
}

/**
*/
void	AWindow_ChangeToolButton::SPI_SetMode( const AModeIndex inModeIndex, const AFileAcc& inFile )
{
	//
	mModeIndex = inModeIndex;
	mFile = inFile;
	//�t�@�C�����擾
	AText	title;
	mFile.GetFilename(title);
	//�����c�[������UI�ɐݒ�
	NVI_SetControlText(kToolName,title);
	//�\���X�V
	NVM_UpdateControlStatus();
}

#pragma mark ===========================

#pragma mark <�ĉ����\�b�h��������>

#pragma mark ===========================

//�R���g���[����ԁiEnable/Disable���j���X�V
//NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()����R�[�������
//�N���b�N��A�N�e�B�x�[�g�ŃR�[�������̂ŁA���܂�d��������NVMDO_UpdateControlStatus()�ɂ͓���Ȃ����ƁB
void	AWindow_ChangeToolButton::NVMDO_UpdateControlStatus()
{
	AText	filename;
	NVI_GetControlText(kToolName,filename);
	if( filename.GetItemCount() == 0 )
	{
		NVI_SetControlEnable(kOK,false);
	}
	else
	{
		AFileAcc	toolbarFolder;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarFolder(toolbarFolder);
		AFileAcc	file;
		file.SpecifyChild(toolbarFolder,filename);
		NVI_SetControlEnable(kOK,(file.Exist()==false));
	}
}

