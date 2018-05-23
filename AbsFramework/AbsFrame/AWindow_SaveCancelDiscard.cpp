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

AWindow_SaveCancelDiscard

*/

#include "stdafx.h"

#include "AWindow_SaveCancelDiscard.h"

const AControlID	kSaveButton		= 10001;
const AControlID	kCancelButton	= 10002;
const AControlID	kMessage1		= 10003;
const AControlID	kMessage2		= 10004;
const AControlID	kDiscardButton	= 10005;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_SaveCancelDiscard
#pragma mark ===========================
//SaveCancelDiscard�q�E�C���h�E

/**
�R���X�g���N�^
*/
AWindow_SaveCancelDiscard::AWindow_SaveCancelDiscard() :
		AWindow(), mParentWindowID(kObjectID_Invalid), 
		mAskSaveChangesDocumentID(kObjectID_Invalid),
		mModal(false)
{
}

/**
�f�X�g���N�^
*/
AWindow_SaveCancelDiscard::~AWindow_SaveCancelDiscard()
{
}

/**
�E�C���h�E����
*/
void	AWindow_SaveCancelDiscard::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVI_SetFixTitle(true);
	NVM_CreateWindow("AbsFrameworkCommonResource/SheetSaveCancelDiscard");
	NVI_SetDefaultCancelButton(kCancelButton);
	NVI_RegisterToFocusGroup(kDiscardButton,true);//#353
	NVI_RegisterToFocusGroup(kCancelButton,true);//#353
	NVI_RegisterToFocusGroup(kSaveButton,true);//#353
}

/**
�E�C���h�E������ꂽ
*/
void	AWindow_SaveCancelDiscard::NVIDO_Close()
{
	AApplication::GetApplication().NVI_GetWindowByID(mParentWindowID).NVI_DoChildWindowClosed_SaveCancelDiscard();
}

/**
���j���[�I��������
*/
ABool	AWindow_SaveCancelDiscard::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	return false;
}

/**
���j���[�X�V����
*/
void	AWindow_SaveCancelDiscard::EVTDO_UpdateMenu()
{
}

/**
�ݒ�{�E�C���h�E�\��
*/
void	AWindow_SaveCancelDiscard::SPNVI_Show( const ADocumentID inAskSaveChangesDocumentID, const AWindowID inParentWindowID, 
											   const AText& inMessage1, const AText& inMessage2, const ABool inModal )
{
	//�Ώۃh�L�������g�L��
	mAskSaveChangesDocumentID = inAskSaveChangesDocumentID;
	//�e�h�L�������g�L��
	mParentWindowID = inParentWindowID;
	//���[�_�����ǂ����L��
	mModal = inModal;
	//���[�_���ȊO�Ȃ�A�V�[�g�Ƃ��Đݒ�
	if( mModal == false )
	{
		NVI_SetWindowStyleToSheet(mParentWindowID);
	}
	//���b�Z�[�W�ݒ�
	NVI_SetControlText(kMessage1,inMessage1);
	NVI_SetControlText(kMessage2,inMessage2);
	
	//���[�_���Ȃ炱���ŁA�{�^�����������܂Ńu���b�N
	if( mModal == true )
	{
		//���[�_���J�n�i���[�_���I���܂Ńu���b�N����j�iNVI_RunModalWindow()���ŃE�C���h�E�\�������j
		NVI_RunModalWindow();
	}
	//���[�_���łȂ���΁A�E�C���h�E�\���������Ė߂�
	else
	{
		//�E�C���h�E�\��
		NVI_Show();
	}
}

/**
�R���g���[���̃N���b�N���̃R�[���o�b�N
*/
ABool	AWindow_SaveCancelDiscard::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	AAskSaveChangesResult	askSaveChanges = kAskSaveChangesResult_Cancel;
	switch(inID)
	{
	  case kSaveButton:
		{
			askSaveChanges = kAskSaveChangesResult_Save;
			NVI_Close();
			result = true;
			break;
		}
	  case kDiscardButton:
		{
			askSaveChanges = kAskSaveChangesResult_DontSave;
			NVI_Close();
			result = true;
			break;
		}
	  case kCancelButton:
		{
			NVI_Close();
			result = true;
			break;
		}
	}
	//�e�E�C���h�E�̃R�[���o�b�N���s
	AApplication::GetApplication().NVI_GetWindowByID(mParentWindowID).EVT_AskSaveChangesReply(mAskSaveChangesDocumentID,askSaveChanges);
	AApplication::GetApplication().NVI_UpdateMenu();
	
	//���[�_���Ȃ烂�[�_���I��
	if( mModal == true )
	{
		NVI_StopModalSession();
	}
	return result;
}

/**
�E�C���h�E����{�^��
*/
void	AWindow_SaveCancelDiscard::EVTDO_DoCloseButton()
{
}

