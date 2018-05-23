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

AWindow_SetProjectFolder

*/

#include "stdafx.h"

#include "AWindow_SetProjectFolder.h"
#include "AApp.h"
//#include "AUtil.h"
#include "AView_Text.h"

const AControlID	kControlID_SetButton = 1;
const AControlID	kControlID_Cancel = 2;
const AControlID	kFolderSelect = 200;
const AControlID	kPathText = 201;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_SetProjectFolder
#pragma mark ===========================
//

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_SetProjectFolder::AWindow_SetProjectFolder():AWindow(/*#175NULL*/)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kProjectFolderSetupPosition);
}
//�f�X�g���N�^
AWindow_SetProjectFolder::~AWindow_SetProjectFolder()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�R���g���[���̃N���b�N���̃R�[���o�b�N
ABool	AWindow_SetProjectFolder::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	//�e�L�X�g�{�b�N�X�ɋL�q���ꂽ�t�H���_�擾
	AText	path;
	NVI_GetControlText(kPathText,path);
	AFileAcc	folder;
	folder.Specify(path);
	//
	ABool	result = false;
	switch(inID)
	{
	  case kControlID_SetButton:
		{
			if( folder.Exist() == true )
			{
				AFileAcc	file;
				file.SpecifyChild(folder,"miproj");
				file.CreateFile();
				GetApp().SPI_UpdateProjectFolder();//RC3
			}
			NVI_Close();
			break;
		}
	  case kControlID_Cancel:
		{
			NVI_Close();
			break;
		}
	  case kFolderSelect:
		{
			AText	message;
			message.SetLocalizedText("ChooseFolderMessage_SelectProjectFolder");
			NVI_ShowChooseFolderWindow(folder,message,kFolderSelect,true);//#551
			break;
		}
	}
	return result;
}

void	AWindow_SetProjectFolder::EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder )
{
	switch(inControlID)
	{
	  case kFolderSelect:
		{
			AText	path;
			inFolder.GetPath(path);
			NVI_SetControlText(kPathText,path);
			NVM_UpdateControlStatus();
			break;
		}
	}
}

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow_SetProjectFolder::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	if( NVI_IsWindowCreated() == false )   return;
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_SetProjectFolder::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/SetProjectFolder");
	NVI_SetDefaultOKButton(kControlID_SetButton);
	
	NVI_RegisterToFocusGroup(kFolderSelect,true);//#353
	NVI_RegisterToFocusGroup(kControlID_Cancel,true);//#353
	NVI_RegisterToFocusGroup(kControlID_SetButton,true);//#353
	NVI_RegisterToFocusGroup(90001,true);//#353
	
	//�w���v�{�^���o�^
	NVI_RegisterHelpAnchor(90001,"topic.htm#project");
}

void	AWindow_SetProjectFolder::NVIDO_Show()
{
	//NVI_ShowChooseFolderWindow(kFolderSelect,true);
}

//Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
void	AWindow_SetProjectFolder::NVIDO_Hide()
{
	
}

void	AWindow_SetProjectFolder::NVMDO_UpdateControlStatus()
{
	/*
	AText	path;
	NVI_GetControlText(kPathText,path);
	AFileAcc	folder;
	folder.Specify(path);
	//
	if( folder.Exist() == true )
	{
		NVI_SetControlEnable(kControlID_SetButton,true);
	}
	else
	{
		NVI_SetControlEnable(kControlID_SetButton,false);
	}
	*/
}

//RC3
void	AWindow_SetProjectFolder::SPI_SetFolder( const AFileAcc& inDefaultFolder )
{
	AText	path;
	inDefaultFolder.GetPath(path);
	NVI_SetControlText(kPathText,path);
}



