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

AWindow_ProjectFolderAlert

*/

#include "stdafx.h"

#include "AWindow_ProjectFolderAlert.h"
#include "AApp.h"

const AControlID	kControlID_SetButton = 1;
const AControlID	kControlID_Cancel = 2;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_ProjectFolderAlert
#pragma mark ===========================
//

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_ProjectFolderAlert::AWindow_ProjectFolderAlert():AWindow(/*#175NULL*/)
{
}
//�f�X�g���N�^
AWindow_ProjectFolderAlert::~AWindow_ProjectFolderAlert()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�R���g���[���̃N���b�N���̃R�[���o�b�N
ABool	AWindow_ProjectFolderAlert::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kControlID_SetButton:
		{
			GetApp().SPI_ShowSetProjectFolderWindow(mDefaultFolder);
			NVI_Close();
			break;
		}
	  case kControlID_Cancel:
		{
			NVI_Close();
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
void	AWindow_ProjectFolderAlert::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/ProjectFolderAlert");
	NVI_SetDefaultOKButton(kControlID_Cancel);
	NVI_SetDefaultCancelButton(kControlID_Cancel);
	
	NVI_RegisterToFocusGroup(kControlID_Cancel,true);//#353
	NVI_RegisterToFocusGroup(kControlID_SetButton,true);//#353
	NVI_RegisterToFocusGroup(90001,true);//#353
	//�w���v�{�^���o�^
	NVI_RegisterHelpAnchor(90001,"topic.htm#project");
}


//RC3
void	AWindow_ProjectFolderAlert::SPI_SetFolder( const AFileAcc& inDefaultFolder )
{
	mDefaultFolder = inDefaultFolder;
}



