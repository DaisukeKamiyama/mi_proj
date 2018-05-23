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

AWindow_EditProgress
#846

*/

#include "stdafx.h"

#include "AWindow_EditProgress.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_EditProgress
#pragma mark ===========================
//�ҏW�v���O���X�E�C���h�E
const AControlID		kMessageText		= 101;
const AControlID		kProgressBar		= 102;
const AControlID		kAbortButton		= 103;
const AControlID		kTitleText			= 104;
const AControlID		kProgressText		= 105;

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^

/**
�R���X�g���N�^
*/
AWindow_EditProgress::AWindow_EditProgress():AWindow(), mErrorMode(false)
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�N���b�N������
*/
ABool	AWindow_EditProgress::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kAbortButton:
		{
			NVI_StopModalSession();
			break;
		}
	}
	return result;
}


#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_EditProgress::NVIDO_Create( const ADocumentID inDocumentID )
{
	//�E�C���h�E����
	NVM_CreateWindow("main/EditProgress");
	
}

#pragma mark ===========================

#pragma mark ---�v���O���X�\��

/**
�v���O���X�ݒ�
*/
void	AWindow_EditProgress::SPI_SetProgress( const ANumber inPercent )
{
	if( NVI_IsWindowCreated() == false )   return;
#if IMPLEMENTATION_FOR_MACOSX
	NVI_SetProgressIndicatorProgress(0,inPercent);
#else
	NVI_SetControlNumber(kProgressBar,inPercent);
#endif
}

/**
�e�e�L�X�g�ݒ�
*/
void	AWindow_EditProgress::SPI_SetText( const AText& inTitle, const AText& inMessage )
{
	if( NVI_IsWindowCreated() == false )   return;
	NVI_SetControlText(kTitleText,inTitle);
	NVI_SetControlText(kMessageText,inMessage);
	NVI_SetControlText(kProgressText,GetEmptyText());
}

/**
�v���O���X�e�L�X�g�ݒ�
*/
void	AWindow_EditProgress::SPI_SetProgressText( const AText& inText )
{
	NVI_SetControlText(kProgressText,inText);
}

/**
�L�����Z���{�^��enable/disable�ݒ�
*/
void	AWindow_EditProgress::SPI_SetEnableCancelButton( const ABool inEnable )
{
	NVI_SetControlEnable(kAbortButton,inEnable);
	NVI_RefreshWindow();
}

