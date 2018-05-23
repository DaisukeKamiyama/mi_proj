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

AWindow_AddNewMode

*/

#include "stdafx.h"

#include "AWindow_AddNewMode.h"
#include "AApp.h"

const AControlID	kModeName			= 5100;
const AControlID	kRadioNew			= 5101;
const AControlID	kRadioImport		= 5102;
const AControlID	kFilePathText		= 5103;
const AControlID	kFileSelectButton	= 5104;
const AControlID	kAddButton			= 5001;
const AControlID	kCancelButton		= 5002;
const AControlID	kRadioCopyMode		= 5105;
const AControlID	kMenuCopyMode		= 5106;
//#844 const AControlID	kRadioCopyDefaultMode		= 5107;//#463
//#844 const AControlID	kMenuCopyDefaultMode		= 5108;//#463
const AControlID	kRadioGetFromWeb			= 5109;//#427
const AControlID	kURLForGetFromWeb			= 5110;//#427
//#844 const AControlID	kMenuFroGetFromWeb			= 5111;//#427
const AControlID	kLabelDuplicatedName		= 5112;//#427
 
#pragma mark ===========================
#pragma mark [�N���X]AWindow_AddNewMode
#pragma mark ===========================
//���[�h�ǉ��E�C���h�E

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_AddNewMode::AWindow_AddNewMode():AWindow()
{
	//�E�C���h�E�ʒu�f�[�^
	NVM_SetWindowPositionDataID(AAppPrefDB::kAddNewModeWindowPosition);
	//�^�C�}�[�Ώ�
	NVI_AddToTimerActionWindowArray();
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

ABool	AWindow_AddNewMode::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	switch(inID)
	{
		//���[�h�t�H���_����C���|�[�g���W�I�{�^��
	  case kRadioImport:
		{
			if( mImportFolder.IsSpecified() == false )
			{
				AFileAcc	desktop;
				AFileWrapper::GetDesktopFolder(desktop);
				/*#605
				AFileAttribute	attr;
				GetApp().SPI_GetModeFileAttribute(attr);//win
				NVI_ShowChooseFileWindowWithFileFilter(desktop,kRadioImport,false,attr);
				*/
				AText	message;
				message.SetLocalizedText("ChooseFolderMessage_SelectModeFolder");
				NVI_ShowChooseFolderWindow(desktop,message,kRadioImport,true);
				break;
			}
			break;
		}
		//�C���|�[�g�����[�h�t�H���_�I���{�^��
	  case kFileSelectButton:
		{
			AFileAcc	desktop;
			AFileWrapper::GetDesktopFolder(desktop);
			/*#350
			AFileAttribute	attr;
			GetApp().SPI_GetModeFileAttribute(attr);//win
			NVI_ShowChooseFileWindowWithFileFilter(desktop,kRadioImport,false,attr);
			*/
			AText	message;
			message.SetLocalizedText("ChooseFolderMessage_SelectModeFolder");
			NVI_ShowChooseFolderWindow(desktop,message,kRadioImport,true);
			break;
		}
		//���[�h�ǉ��{�^��
	  case kAddButton:
		{
			AText	modename;
			NVI_GetControlText(kModeName,modename);
			//==================�V�K==================
			if( NVI_GetControlBool(kRadioNew) == true )
			{
				AModeIndex	modeIndex = GetApp().SPI_AddNewMode(modename);
				if( modeIndex != kIndex_Invalid )
				{
					GetApp().SPI_ShowModePrefWindow(modeIndex);//#844
				}
			}
			//==================�������[�h����R�s�[==================
			else if( NVI_GetControlBool(kRadioCopyMode) == true )//R0000 ���[�h�R�s�[
			{
				AText	originalmodename;
				NVI_GetControlText(kMenuCopyMode,originalmodename);
				AModeIndex originalModeIndex = GetApp().SPI_FindModeIndexByModeRawName(originalmodename);
				if( originalModeIndex != kIndex_Invalid )
				{
					AFileAcc	originalModeFolder;
					GetApp().SPI_GetModePrefDB(originalModeIndex).GetModeFolder(originalModeFolder);
					AModeIndex	modeIndex = GetApp().SPI_AddNewModeImportFromFolder(modename,originalModeFolder,true,false);
					if( modeIndex != kIndex_Invalid )
					{
						GetApp().SPI_ShowModePrefWindow(modeIndex);//#844
					}
				}
			}
			/*#844
			//#463
			else if( NVI_GetControlBool(kRadioCopyDefaultMode) == true )
			{
				AText	originalModeFileName;
				originalModeFileName.SetText(mDefaultModeFolderNameArray.GetTextConst(NVI_GetControlNumber(kMenuCopyDefaultMode)));
				//�f�t�H���g���[�h�t�@�C���擾
				AFileAcc	defaultModeFolder;
				GetApp().SPI_GetModeDefaultFolder(defaultModeFolder);
				AFileAcc	originalModeFolder;
				originalModeFolder.SpecifyChild(defaultModeFolder,originalModeFileName);
				AFileAcc	originalModeFile;
				originalModeFile.SpecifyChild(originalModeFolder,originalModeFileName);
				//���[�h�ǉ�
				AModeIndex	modeIndex = GetApp().SPI_AddNewModeImport(modename,originalModeFile,true,true);
				if( modeIndex != kIndex_Invalid )
				{
					GetApp().SPI_ShowModePrefWindow(modeIndex);//#844
				}
			}
			*/
			//#427 Web����擾�̃��W�I�{�^��ON�̏ꍇ
			//==================Web����擾==================
			else if( NVI_GetControlBool(kRadioGetFromWeb) == true )
			{
				//Web���烂�[�h�ǉ�
				AText	url;
				NVI_GetControlText(kURLForGetFromWeb,url);
				GetApp().SPI_AddNewModeFromWeb(modename,url);
			}
			//==================���[�h�t�H���_����C���|�[�g==================
			else
			{
				NVI_Close();//R0000
				AModeIndex	modeIndex = GetApp().SPI_AddNewModeImportFromFolder(modename,mImportFolder,false,false);//R0000 security
				if( modeIndex != kIndex_Invalid )
				{
					GetApp().SPI_ShowModePrefWindow(modeIndex);//#844
				}
			}
			NVI_Close();
			break;
		}
	  case kCancelButton:
		{
			NVI_Close();
			break;
		}
	  default:
		{
			return false;
		}
	}
	return true;
}

void	AWindow_AddNewMode::EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder )
{
	switch(inControlID)
	{
	  case kRadioImport:
		{
			SPI_SetImportFolder(inFolder);
			break;
		}
	}
}

void	AWindow_AddNewMode::EVTDO_TextInputted( const AControlID inID )
{
	NVM_UpdateControlStatus();
}

//R0000
ABool	AWindow_AddNewMode::EVTDO_ValueChanged( const AControlID inControlID )
{
	ABool	result = true;
	switch(inControlID)
	{
	  case kMenuCopyMode:
		{
			NVI_SetControlBool(kRadioCopyMode,true);
			NVM_UpdateControlStatus();
			//���[�h���ݒ� #463
			AText	modename;
			NVI_GetControlText(kMenuCopyMode,modename);
			NVI_SetControlText(kModeName,modename);
			break;
		}
		/*#844
		//#463
	  case kMenuCopyDefaultMode:
		{
			NVI_SetControlBool(kRadioCopyDefaultMode,true);
			NVM_UpdateControlStatus();
			//���[�h�����ݒ�Ȃ�R�s�[���Đݒ�
			AText	modename;
			NVI_GetControlText(kMenuCopyDefaultMode,modename);
			NVI_SetControlText(kModeName,modename);
			break;
		}
		//#427
	  case kMenuFroGetFromWeb:
		{
			AText	url;
			GetApp().SPI_GetModeUpdateURL(NVI_GetControlNumber(kMenuFroGetFromWeb),url);
			NVI_SetControlText(kURLForGetFromWeb,url);
			NVI_SetControlBool(kRadioGetFromWeb,true);
			break;
		}
		*/
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

//win
/**
�E�C���h�E����{�^��
*/
void	AWindow_AddNewMode::EVTDO_DoCloseButton()
{
	NVI_Close();
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_AddNewMode::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVI_SetFixTitle(true);//win 080726
	NVM_CreateWindow("main/AddNewMode");
	NVI_CreateEditBoxView(kModeName);
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadioNew);
	NVI_AddToLastRegisteredRadioGroup(kRadioCopyMode);//R0000
	NVI_AddToLastRegisteredRadioGroup(kRadioImport);
	NVI_AddToLastRegisteredRadioGroup(kRadioGetFromWeb);//#427
	//#844 NVI_AddToLastRegisteredRadioGroup(kRadioCopyDefaultMode);//#463
	//#688 NVI_CreateEditBoxView(kURLForGetFromWeb);//#427
	NVI_RegisterToFocusGroup(kFileSelectButton,true);//#353
	NVI_RegisterToFocusGroup(kMenuCopyMode,true);//#353
	//#844 NVI_RegisterToFocusGroup(kMenuFroGetFromWeb,true);//#427
	NVI_RegisterToFocusGroup(kCancelButton,true);//#353
	NVI_RegisterToFocusGroup(kAddButton,true);//#353
	
	//#844 NVI_SetWindowStyleToSheet(/*#199 mModePrefWindow*/GetModePrefWindow().GetObjectID());
	NVI_SetDefaultCancelButton(kCancelButton);//B0000
	mImportFolder.Unspecify();
	NVI_SetControlBool(kRadioNew,true);//win
	//R0000 ���[�h�R�s�[
	NVI_RegisterTextArrayMenu(kMenuCopyMode,kTextArrayMenuID_AllMode);//#914
	
	/*#844
	//#463 �f�t�H���g���[�h���j���[�쐬
	mDefaultModeFolderNameArray.DeleteAll();
	AFileAcc	defaultModeFolder;
	GetApp().SPI_GetModeDefaultFolder(defaultModeFolder);
	AObjectArray<AFileAcc>	modeFolderArray;
	defaultModeFolder.GetChildren(modeFolderArray);
	for( AIndex i = 0; i < modeFolderArray.GetItemCount(); i++ )
	{
		AText	filename;
		modeFolderArray.GetObjectConst(i).GetFilename(filename,false);
		mDefaultModeFolderNameArray.Add(filename);
	}
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_DefaultMode,mDefaultModeFolderNameArray);
	NVI_RegisterTextArrayMenu(kMenuCopyDefaultMode,kTextArrayMenuID_DefaultMode);
	*/
	//#427
	//#844 NVI_RegisterTextArrayMenu(kMenuFroGetFromWeb,kTextArrayMenuID_ModeUpdateURL);
	
#if IMPLEMENTATION_FOR_MACOSX
	//#427
	if( AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_4 )
	{
		NVI_SetControlEnable(kRadioGetFromWeb,false);
		NVI_SetControlEnable(kURLForGetFromWeb,false);
		//#844 NVI_SetControlEnable(kMenuFroGetFromWeb,false);
	}
#endif
}

void	AWindow_AddNewMode::NVMDO_UpdateControlStatus()
{
	NVI_SetControlEnable(kAddButton,ModeNameValid());
	
	//#427 ���[�h���d���x�����x���\��
	AText	text;
	NVI_GetControlText(kModeName,text);
	NVI_SetShowControl(kLabelDuplicatedName,((ModeNameValid()==false)&&(text.GetItemCount()>0)));
}

ABool	AWindow_AddNewMode::ModeNameValid()
{
	AText	text;
	NVI_GetControlText(kModeName,text);
	if( text.GetItemCount() == 0 )   return false;
	/*#427 ���L���Ƒ啶����������ʂ��Ă��܂��̂�SPI_CheckModeNameUnique()���g���悤�ɏC��
	AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeName(text);
	return (modeIndex==kIndex_Invalid);
	*/
	return (GetApp().SPI_CheckModeRawNameUnique(text)==false);
}

void	AWindow_AddNewMode::SPI_SetImportFolder( const AFileAcc& inImportFolder )
{
	mImportFolder.CopyFrom(inImportFolder);
	AText	path;
	//B0389 OK inImportFile.GetPath(path,kFilePathType_1);
	GetApp().GetAppPref().GetFilePathForDisplay(inImportFolder,path);//B0389
	NVI_SetControlText(kFilePathText,path);
	AText	name;
	path.GetFilename(name);
	NVI_SetControlText(kModeName,name);
	NVI_SetControlBool(kRadioImport,true);
	NVM_UpdateControlStatus();
}

