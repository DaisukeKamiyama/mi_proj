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

AWindow_DevTools
//#436

*/

#include "stdafx.h"

#include "AWindow_DevTools.h"
#include "AApp.h"

const AControlID	kControlID_DmgMaker_Version 	= 200;
const AControlID	kControlID_DmgMaker_Folder	 	= 201;
const AControlID	kControlID_DmgMaker_Master 		= 202;
const AControlID	kControlID_DmgMaker_Make 		= 203;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_DevTools
#pragma mark ===========================
//

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_DevTools::AWindow_DevTools():AWindow()
{
}
//�f�X�g���N�^
AWindow_DevTools::~AWindow_DevTools()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�R���g���[���̃N���b�N���̃R�[���o�b�N
ABool	AWindow_DevTools::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kControlID_DmgMaker_Make:
		{
			AText	ver, dir, masterfile;
			NVI_GetControlText(kControlID_DmgMaker_Version,ver);
			NVI_GetControlText(kControlID_DmgMaker_Folder,dir);
			NVI_GetControlText(kControlID_DmgMaker_Master,masterfile);
			//cd
			ALaunchWrapper::ChangeDirectory(dir);
			//rm
			AText	command;
			command.SetCstring("rm ");
			command.AddText(ver);
			command.AddCstring(".dmg");
			ALaunchWrapper::ExecCommand(command);
			//hdiutil
			command.SetCstring("hdiutil convert ");
			command.AddText(masterfile);
			command.AddCstring(" -format UDCO -o ");
			command.AddText(ver);
			command.AddCstring(".dmg");
			ALaunchWrapper::ExecCommand(command);
			break;
		}
	}
	return result;
}

//Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
void	AWindow_DevTools::NVIDO_Hide()
{
	AText	text;
	NVI_GetControlText(kControlID_DmgMaker_Version,text);
	GetApp().NVI_GetAppPrefDB().SetData_Text(AAppPrefDB::kDevTool_DmgMaker_Version,text);
	NVI_GetControlText(kControlID_DmgMaker_Folder,text);
	GetApp().NVI_GetAppPrefDB().SetData_Text(AAppPrefDB::kDevTool_DmgMaker_Folder,text);
	NVI_GetControlText(kControlID_DmgMaker_Master,text);
	GetApp().NVI_GetAppPrefDB().SetData_Text(AAppPrefDB::kDevTool_DmgMaker_Master,text);
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_DevTools::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/DevTools");
	NVI_CreateEditBoxView(200);
	NVI_CreateEditBoxView(201);
	NVI_CreateEditBoxView(202);
	
	AText	text;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDevTool_DmgMaker_Version,text);
	NVI_SetControlText(kControlID_DmgMaker_Version,text);
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDevTool_DmgMaker_Folder,text);
	NVI_SetControlText(kControlID_DmgMaker_Folder,text);
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDevTool_DmgMaker_Master,text);
	NVI_SetControlText(kControlID_DmgMaker_Master,text);
}

