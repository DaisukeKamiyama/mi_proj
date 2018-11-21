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

AWindow_AppPref

*/

#include "stdafx.h"

#include "AAppPrefDB.h"
#include "AApp.h"

//�R���g���[��ID�iAAppPrefDB�Ŗ���`�̂��́BListView�S�̂�ControlID���j
const AControlID	kTable_TextEncodingMenu				= 126;
const AControlID	kTable_LaunchApp					= 146;
const AControlID	kPopup_TextEncodingMenu				= 154;
const AControlID	kTable_FTPAccount					= 153;
const AControlID	kTable_RecentlyOpendFileComment		= 166;
const AControlID	kText_FTPAccount_Server				= 168;
const AControlID	kText_FTPAccount_User				= 169;
const AControlID	kText_FTPAccount_Password			= 170;
const AControlID	kText_FTPAccount_RootPath			= 171;
const AControlID	kText_FTPAccount_HTTPURL			= 172;
const AControlID	kPopup_FTPAccount_ProxyMode			= 173;
const AControlID	kText_FTPAccount_ProxyServer		= 174;
const AControlID	kText_FTPAccount_ProxyUser			= 175;
const AControlID	kText_FTPAccount_ProxyPassword		= 176;
const AControlID	kCheck_FTPAccount_Backup			= 177;
const AControlID	kText_FTPAccount_BackupFolderPath	= 178;
const AControlID	kButton_FTPAccount_BackupFolderPathSelect	= 179;
const AControlID	kCheck_FTPAccount_Passive			= 180;
const AControlID	kGroup_FTPAccount					= 181;
const AControlID	kText_FTPAccount_Port				= 182;
const AControlID	kButton_FTPAccount_Add				= 183;
const AControlID	kButton_FTPAccount_Remove			= 184;
const AControlID	kCheck_FTPAccount_BinaryMode		= 185;
const AControlID	kButton_FTPAccount_RemoveOK			= 186;
const AControlID	kButton_LaunchApp_Add				= 187;
const AControlID	kButton_LaunchApp_Remove			= 188;
const AControlID	kButton_LaunchApp_Up				= 189;
const AControlID	kButton_LaunchApp_Down				= 190;
const AControlID	kButton_LaunchApp_RemoveOK			= 191;
const AControlID	kButton_TextEncodingUp				= 192;
const AControlID	kButton_TextEncodingDown			= 193;
const AControlID	kButton_TextEncodingTop				= 194;
const AControlID	kButton_TextEncodingBottom			= 195;
const AControlID	kButton_TextEncodingAddNew			= 196;
const AControlID	kButton_TextEncodingDelete			= 197;
const AControlID	kButton_TextEncodingDeleteOK		= 198;
const AControlID	kButton_RecentlyOpenedFileCommentAddNew		= 199;
const AControlID	kButton_RecentlyOpenedFileCommentDelete		= 200;
const AControlID	kButton_RecentlyOpenedFileCommentUp			= 201;
const AControlID	kButton_RecentlyOpenedFileCommentDown		= 202;
const AControlID	kButton_RecentlyOpenedFileCommentDeleteOK	= 203;
const AControlID	kButton_KinsokuDefault				= 204;
const AControlID	kButton_Registration				= 205;
const AControlID	kButton_DeleteAntiAliasDefaulltData	= 206;
const AControlID	kButton_FTPFolder					= 208;
const AControlID	kText_TexPreviewerPath 				= 213;
const AControlID	kButton_SelectTexPreviewer			= 214;
const AControlID	kButton_OpenIANAURL					= 215;
const AControlID	kPopup_TextWindowTabSize			= 220;//R0177
const AControlID	kPopup_SubPane1						= 270;//#291
const AControlID	kPopup_SubPane2						= 271;//#291
const AControlID	kPopup_SubPane3						= 272;//#291
const AControlID	kPopup_InfoPane1					= 280;//#291
const AControlID	kPopup_InfoPane2					= 281;//#291
const AControlID	kPopup_InfoPane3					= 282;//#291
const AControlID	kButton_PaneLayoutUpdate			= 290;//#291

const AControlID	kButton_EditTextEncodingMenu		= 500;
const AControlID	kButton_Edit_Recently_Path			= 501;
const AControlID	kButton_Edit_Recently_Comment		= 502;
const AControlID	kButton_Edit_Launch_Suffix			= 503;
const AControlID	kButton_Edit_Launch_Creator			= 504;
const AControlID	kButton_Edit_Launch_Type			= 505;
const AControlID	kCheck_FTPAccount_BackupWithDateTime	= 506;//R0207
const AControlID	kCheck_RegisterToSendToMenu			= 507;//win 080722
//const AControlID	kLabel_kLineFolding					= 508;//#450

//======================== �u��ʁv �^�u ========================
//
const AControlID	kButton_CurrentWindowSize			= 7001;
const AControlID	kRadio_NormalCaret					= 7002;
const AControlID	kRadio_NoneForCurrentLine			= 7003;
const AControlID	kButton_OpenModePref				= 7004;
const AControlID	kButton_AutoBackupSetDefault		= 7005;
const AControlID	kButton_AutoBackupSelectFolder		= 7006;
const AControlID	kButton_BinaryFileDefinitionSetDefault = 7007;
const AControlID	kRadio_OpenDocumentWithWindow		 = 7008;
const AControlID	kRadio_SelectLineByTripleClick		 = 7009;
const AControlID	kRadio_SelectLineStartByClickLineNumber	 = 7010;

//======================== �u�T�u�E�C���h�E�v �^�u ========================
//
const AControlID	kFontControl_SubWindowsFont			= 7101;//#725
const AControlID	kRadio_NoPopupCandidate				= 7102;

//======================== �u�e�L�X�g�v �^�u ========================
//
const AControlID	kRadio_KinsokuNoBurasage			= 7201;

//======================== �u�����v �^�u ========================
//
const AControlID	kFontControl_FindFont				= 7301;//#688

//======================== �u�����[�g�v �^�u ========================
//
const AControlID	kPopup_FTPAccount_ConnectionType	= 7401;//#361
const AControlID	kRadio_FTPAccount_UseDefaultPort	= 7403;//#193
const AControlID	kRadio_FTPAccount_SpecifyPort		= 7404;//#193
const AControlID	kText_FTPAccount_DisplayName		= 7405;//#193
const AControlID	kCheck_FTPAccount_UsePrivateKey		= 7406;//#1231
const AControlID	kText_FTPAccount_PrivateKeyFilePath	= 7407;//#1231
const AControlID	kButton_FTPAccount_SelectPrivateKey	= 7408;//#1231
const AControlID	kStaticText_FTPAccount_PrivateKeyPass= 7409;//#1231
const AControlID	kCheck_FTPAccount_DontCheckServerCert= 7410;//#1231

//======================== �u��r�v �^�u ========================
//
const AControlID	kButton_DiffWorkingSelectFolder		= 7501;
const AControlID	kButton_DiffSourceSelectFolder		= 7502;

//======================== �u����v �^�u ========================
//
const AControlID	kFontControl_PrintFont				= 7601;//#868
const AControlID	kFontControl_PrintPageNumberFont	= 7602;//#868
const AControlID	kFontControl_PrintFileNameFont		= 7603;//#868
const AControlID	kFontControl_PrintLineNumberFont	= 7604;//#868

//======================== �u�@�\�v �^�u ========================
//
const AControlID	kButton_UpdateSkinMenu				= 7701;

//�e�[�u���r���[ColumnID
const ADataID	kColumn_TextEncodingMenu			= 0;

const ADataID	kColumn_FTPAccount_Server			= 0;

const ADataID	kColumn_LaunchApp_CheckExtension	= 0;
const ADataID	kColumn_LaunchApp_Extension			= 1;
const ADataID	kColumn_LaunchApp_CheckCreator		= 2;
const ADataID	kColumn_LaunchApp_Creator			= 3;
const ADataID	kColumn_LaunchApp_CheckType			= 4;
const ADataID	kColumn_LaunchApp_Type				= 5;
const ADataID	kColumn_LaunchApp_ShiftKey			= 6;
const ADataID	kColumn_LaunchApp_App_Name			= 7;
const ADataID	kColumn_LaunchApp_App_Path			= 8;

const ADataID	kColumn_RecentlyOpenedFileComment		= 0;
const ADataID	kColumn_RecentlyOpenedFileCommentPath	= 1;


#pragma mark ===========================
#pragma mark [�N���X]AWindow_AppPref
#pragma mark ===========================
//���ݒ�E�C���h�E

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_AppPref::AWindow_AppPref() : AWindow(/*#175NULL*/)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kPreferenceWindowPosition);
	//#175
	NVI_AddToRotateArray();
	//#138
	NVI_AddToTimerActionWindowArray();
}
//�f�X�g���N�^
AWindow_AppPref::~AWindow_AppPref()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

ADataBase&	AWindow_AppPref::NVMDO_GetDB()
{
	return GetApp().GetAppPref();
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

ABool	AWindow_AppPref::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
		//#868
		//���݂̃E�C���h�E�T�C�Y��ݒ肷��{�^��
	  case kButton_CurrentWindowSize:
		{
			//�őO�ʂ̃e�L�X�g�E�C���h�E���擾
			AWindowID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID != kObjectID_Invalid )
			{
				//�E�C���h�Erect�i���C���J���������j�擾
				ARect	rect = {0};
				GetApp().SPI_GetTextWindowByID(winID).SPI_GetWindowMainColumnBounds(rect);
				NVI_SetControlNumber(AAppPrefDB::kDefaultWindowWidth,rect.right-rect.left);
				NVI_SetControlNumber(AAppPrefDB::kDefaultWindowHeight,rect.bottom-rect.top);
				NVI_DoValueChanged(AAppPrefDB::kDefaultWindowWidth);
				NVI_DoValueChanged(AAppPrefDB::kDefaultWindowHeight);
			}
			break;
		}
		//#81
		//�����o�b�N�A�b�v�t�H���_�ݒ�i�f�t�H���g��ݒ�j
	  case kButton_AutoBackupSetDefault:
		{
			//�f�t�H���g�̃o�b�N�A�b�v�t�H���_���擾���Đݒ�
			AFileAcc	folder;
			GetApp().SPI_GetDefaultAutoBackupRootFolder(folder);
			AText	path;
			folder.GetPath(path);
			NVI_SetControlText(AAppPrefDB::kAutoBackupRootFolder,path);
			//DB�ւ̕ۑ��̂��߁A�R���g���[���l�ύX�����������s����
			NVI_DoValueChanged(AAppPrefDB::kAutoBackupRootFolder);
			//�o�b�N�A�b�v�t�H���_�ύX������
			GetApp().SPI_DoAutoBackupFolderChangedAll();
			break;
		}
		//�����o�b�N�A�b�v�t�H���_�ݒ�i�t�H���_�I���j�ۂ���
	  case kButton_AutoBackupSelectFolder:
		{
			//���ݐݒ肳��Ă���t�H���_�p�X���擾���A�t�H���_�I���_�C�A���O�̃f�t�H���g�p�X�Ƃ���
			AText	path;
			NVI_GetControlText(AAppPrefDB::kAutoBackupRootFolder,path);
			AFileAcc	folder;
			folder.Specify(path);
			//�t�H���_�I�����b�Z�[�W�擾
			AText	message;
			message.SetLocalizedText("ChooseFolderMessage_AutoBackupFolder");
			//�t�H���_�I���_�C�A���O�\��
			NVI_ShowChooseFolderWindow(folder,message,kButton_AutoBackupSelectFolder,true);
			result = true;
			break;
		}
		//��e�L�X�g�t�@�C����`�f�t�H���g�ݒ�{�^��
	  case kButton_BinaryFileDefinitionSetDefault:
		{
			//DB�f�[�^���f�t�H���g�l�ɐݒ�
			GetApp().NVI_GetAppPrefDB().SetDefault_Text(AAppPrefDB::kBinaryFileDefinition);
			//DB�f�[�^���擾���āA�R���g���[���ɐݒ�
			AText	text;
			GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kBinaryFileDefinition,text);
			NVI_SetControlText(AAppPrefDB::kBinaryFileDefinition,text);
			break;
		}
		//#160
		//�����t�H���_��r working�t�H���_�I���{�^��
	  case kButton_DiffWorkingSelectFolder:
		{
			//���ݐݒ肳��Ă���working�t�H���_�p�X���擾���A�t�H���_�I���_�C�A���O�̃f�t�H���g�p�X�Ƃ���
			AText	path;
			NVI_GetControlText(AAppPrefDB::kDiffSourceFolder_Working,path);
			AFileAcc	folder;
			folder.Specify(path);
			//�t�H���_�I�����b�Z�[�W�擾
			AText	message;
			message.SetLocalizedText("ChooseFolderMessage_AutoDiffWorkingFolder");
			//�t�H���_�I���_�C�A���O�\��
			NVI_ShowChooseFolderWindow(folder,message,kButton_DiffWorkingSelectFolder,true);
			result = true;
			break;
		}
		//�����t�H���_��r �\�[�X�t�H���_�I���{�^��
	  case kButton_DiffSourceSelectFolder:
		{
			//���ݐݒ肳��Ă���\�[�X�t�H���_�p�X���擾���A�t�H���_�I���_�C�A���O�̃f�t�H���g�p�X�Ƃ���
			AText	path;
			NVI_GetControlText(AAppPrefDB::kDiffSourceFolder_Source,path);
			AFileAcc	folder;
			folder.Specify(path);
			//�t�H���_�I�����b�Z�[�W�擾
			AText	message;
			message.SetLocalizedText("ChooseFolderMessage_AutoDiffSourceFolder");
			//�t�H���_�I���_�C�A���O�\��
			NVI_ShowChooseFolderWindow(folder,message,kButton_DiffSourceSelectFolder,true);
			result = true;
			break;
		}
		//�����[�g�t�@�C�� �閧���I���{�^�� #1231
	  case kButton_FTPAccount_SelectPrivateKey:
		{
			//~/.ssh���f�t�H���g�p�X�Ƃ���
			AFileAcc	userFolder;
			AFileWrapper::GetUserFolder(userFolder);
			AFileAcc	folder;
			folder.SpecifyChild(userFolder,".ssh");
			//�t�H���_�I�����b�Z�[�W�擾
			AText	message;
			message.SetLocalizedText("ChooseFileMessage_SelectPrivateKeyFile");
			//�t�H���_�I���_�C�A���O�\��
			NVI_ShowChooseFileWindow(folder,message,kButton_FTPAccount_SelectPrivateKey);
			result = true;
			break;
		}
		//���[�h�ݒ�i�W�����[�h�j���J���{�^��
	  case kButton_OpenModePref:
		{
			GetApp().SPI_ShowModePrefWindow(kStandardModeIndex);
			break;
		}
		//skin���j���[
	  case kButton_UpdateSkinMenu:
		{
			GetApp().SPI_UpdateSkinMenu();
			break;
		}
		//
	  case kButton_FTPAccount_BackupFolderPathSelect:
		{
			//#551 ���݃e�L�X�g�{�b�N�X�Ɏw�肳��Ă���t�H���_���f�t�H���g�t�H���_�ɂ���
			AText	path;
			NVI_GetControlText(kText_FTPAccount_BackupFolderPath,path);
			AFileAcc	defaultfolder;
			defaultfolder.Specify(path);
			//
			AText	message;
			message.SetLocalizedText("ChooseFolderMessage_FTPBackup");
			NVI_ShowChooseFolderWindow(defaultfolder,message,kButton_FTPAccount_BackupFolderPathSelect,true);//#551
			result = true;
			break;
		}
	  case kButton_KinsokuDefault:
		{
			AText	text;
			text.SetLocalizedText("Kinsoku_Default");
			NVI_SetControlText(AAppPrefDB::kKinsokuLetters,text);
			result = true;
			break;
		}
	  case kButton_Registration:
		{
			AText	text;
			NVI_GetControlText(AAppPrefDB::kUserID,text);
			ABool	checkOK = false;
			if( text.GetItemCount() == 11 )
			{
				if( false )//���T�[�o�[�ł̃`�F�b�N����������
				{
					checkOK = true;
				}
			}
			
			{
				if( checkOK == true )
				{
					AFileAcc	folder;
					AFileWrapper::GetAppPrefFolder(folder);
					AFileAcc	file;
					file.SpecifyChild(folder,"userid");
					file.CreateFile();
					file.WriteFile(text);
					NVI_SetShowControl(AAppPrefDB::kThanks,true);
				}
				else
				{
					NVI_SetControlText(AAppPrefDB::kUserID,GetEmptyText());
				}
			}
			result = true;
			break;
		}
		//B0266
	  case kButton_DeleteAntiAliasDefaulltData:
		{
			char	cmd[256];
			strcpy(cmd,"/usr/bin/defaults delete mi AppleAntiAliasingThreshold");
			system(cmd);
			result = true;
			break;
		}
		/*#193
	  case kButton_FTPFolder:
		{
			GetApp().SPI_ShowFTPFolderWindow();
			result = true;
			break;
		}
		*/
	  case kButton_SelectTexPreviewer:
		{
			AFileAcc	defaultFile;
			defaultFile.Specify("/Applications");
			AText	message;
			NVI_ShowChooseFileWindow(defaultFile,message,kButton_SelectTexPreviewer,true);
			result = true;
			break;
		}
	  case kButton_OpenIANAURL:
		{
			ALaunchWrapper::OpenURL("http://www.iana.org/assignments/character-sets");
			break;
		}
#if IMPLEMENTATION_FOR_WINDOWS
		//win 080723
	  case kCheck_RegisterToSendToMenu:
		{
			AFileAcc	sendtofolder;
			AFileWrapper::GetSendToFolder(sendtofolder);
			AText	shortcutfilename;
			GetApplicationName(shortcutfilename);
			shortcutfilename.AddCstring(".lnk");
			AFileAcc	linkfile;
			linkfile.SpecifyChild(sendtofolder,shortcutfilename);
			AFileAcc	appfile;
			AFileWrapper::GetAppFile(appfile);
			if( NVI_GetControlBool(kCheck_RegisterToSendToMenu) == true )
			{
				if( linkfile.Exist() == false )
				{
					appfile.CreateShellLink(linkfile);
				}
			}
			else
			{
				if( linkfile.Exist() == true )
				{
					linkfile.DeleteFile();
				}
			}
			break;
		}
#endif
	}
	return result;
}

ABool	AWindow_AppPref::EVTDO_ValueChanged( const AControlID inID )
{
	ADataBase& apppref = GetApp().NVI_GetAppPrefDB();
	
	ABool	result = false;
	switch(inID)
	{
		//#725
		//�T�u�E�C���h�E�t�H���g�ύX������
	  case kFontControl_SubWindowsFont:
		{
			//�t�H���g�R���g���[������t�H���g���E�T�C�Y�擾
			AText	fontname;
			AFloatNumber	fontsize = 9.0;
			NVI_GetControlFont(kFontControl_SubWindowsFont,fontname,fontsize);
			//DB�ɐݒ�
			apppref.SetData_Text(AAppPrefDB::kSubWindowsFontName,fontname);
			apppref.SetData_FloatNumber(AAppPrefDB::kSubWindowsFontSize,fontsize);
			//�T�u�E�C���h�E�v���p�e�B�X�V������
			GetApp().SPI_UpdateSubWindowsProperty();
			break;
		}
		//#688
		//�����E�C���h�E�t�H���g�ύX������
	  case kFontControl_FindFont:
		{
			//�t�H���g�R���g���[������t�H���g���E�T�C�Y�擾
			AText	fontname;
			AFloatNumber	fontsize = 9.0;
			NVI_GetControlFont(kFontControl_FindFont,fontname,fontsize);
			//DB�ɐݒ�
			apppref.SetData_Text(AAppPrefDB::kFindDialogFontName,fontname);
			apppref.SetData_FloatNumber(AAppPrefDB::kFind_FontSize,fontsize);
			//�����E�C���h�E�X�V����
			GetApp().SPI_UpdateFindWindow();
			break;
		}
		//#868
		//����t�H���g�ύX������
	  case kFontControl_PrintFont:
		{
			//�t�H���g�R���g���[������t�H���g���E�T�C�Y�擾
			AText	fontname;
			AFloatNumber	fontsize = 9.0;
			NVI_GetControlFont(kFontControl_PrintFont,fontname,fontsize);
			//DB�ɐݒ�
			apppref.SetData_Text(AAppPrefDB::kPrintOption_PrintFontName,fontname);
			apppref.SetData_FloatNumber(AAppPrefDB::kPrintOption_PrintFontSize,fontsize);
			break;
		}
		//#868
		//����y�[�W�ԍ��t�H���g�ύX������
	  case kFontControl_PrintPageNumberFont:
		{
			//�t�H���g�R���g���[������t�H���g���E�T�C�Y�擾
			AText	fontname;
			AFloatNumber	fontsize = 9.0;
			NVI_GetControlFont(kFontControl_PrintPageNumberFont,fontname,fontsize);
			//DB�ɐݒ�
			apppref.SetData_Text(AAppPrefDB::kPrintOption_PageNumberFontName,fontname);
			apppref.SetData_FloatNumber(AAppPrefDB::kPrintOption_PageNumberFontSize,fontsize);
			break;
		}
		//#868
		//����t�@�C�����t�H���g�ύX������
	  case kFontControl_PrintFileNameFont:
		{
			//�t�H���g�R���g���[������t�H���g���E�T�C�Y�擾
			AText	fontname;
			AFloatNumber	fontsize = 9.0;
			NVI_GetControlFont(kFontControl_PrintFileNameFont,fontname,fontsize);
			//DB�ɐݒ�
			apppref.SetData_Text(AAppPrefDB::kPrintOption_FileNameFontName,fontname);
			apppref.SetData_FloatNumber(AAppPrefDB::kPrintOption_FileNameFontSize,fontsize);
			break;
		}
		//#868
		//����s�ԍ��t�H���g�ύX������
	  case kFontControl_PrintLineNumberFont:
		{
			//�t�H���g�R���g���[������t�H���g���E�T�C�Y�擾
			AText	fontname;
			AFloatNumber	fontsize = 9.0;
			NVI_GetControlFont(kFontControl_PrintLineNumberFont,fontname,fontsize);
			//DB�ɐݒ�
			apppref.SetData_Text(AAppPrefDB::kPrintOption_LineNumberFontName,fontname);
			apppref.SetData_FloatNumber(AAppPrefDB::kPrintOption_LineNumberFontSize,fontsize);
			break;
		}
		/*#844
	  case kPopup_TextEncodingMenu:
		{
			AText	text;
			//B0343
			if( NVI_GetControlNumber(kPopup_TextEncodingMenu) == 0 )
			{
				text.SetCstring("-");
			}
			else
			{
				NVI_GetControlText(kPopup_TextEncodingMenu,text);
			}
			NVI_SetControlNumber(kPopup_TextEncodingMenu,0);
			AIndex	index = NVI_GetControlNumber(kTable_TextEncodingMenu);
			if( index != kIndex_Invalid )
			{
				GetApp().GetAppPref().SetData_TextArray(AAppPrefDB::kTextEncodingMenu,index,text);
				NVM_UpdateDBTableView(kTable_TextEncodingMenu);
			}
			GetApp().SPI_UpdateTextEncodingMenu();
			result = true;
			break;
		}
		*/
		//�����[�g�A�J�E���g�̃T�[�o�[���i�T�[�o�[����URL����͂����ꍇ�A�T�[�o�[���A���[�U�[�����ɕ������Đݒ肷��j
	  case kText_FTPAccount_Server:
		{
			//�ݒ肵���T�[�o�[���擾
			AText	text;
			NVI_GetControlText(kText_FTPAccount_Server,text);
			//URL���ǂ����̔���
			AIndex	foundpos = 0;
			if( text.FindCstring(0,"://",foundpos) == true )
			{
				//�v���g�R���A�T�[�o�[���A���[�U�[�A�p�X�A�|�[�g�ԍ��ɕ���
				AText	protocol, server, user, path;
				text.GetFTPURLProtocol(protocol);
				protocol.ChangeCaseLower();
				text.GetFTPURLServer(server);
				text.GetFTPURLUser(user);
				text.GetFTPURLPath(path);
				APortNumber	portnumber = text.GetFTPURLPortNumber();
				//�T�[�o�[���擾
				NVI_SetControlText(kText_FTPAccount_Server,server);
				//�v���g�R������R�l�N�V�����^�C�v�I��
				//�R�l�N�V�����^�C�v���Ƃ̃��[�v
				for( AIndex i = 0; i < GetApp().SPI_GetAccessPluginMenuTextArray().GetItemCount(); i++ )
				{
					//�R�l�N�V�����^�C�v�擾
					AText	accessPluginName;
					GetApp().SPI_GetAccessPluginMenuTextArray().Get(i,accessPluginName);
					//���ʈȍ~���폜
					AIndex	foundpos = 0;
					if( accessPluginName.FindCstring(0,"(",foundpos) == true )
					{
						accessPluginName.DeleteAfter(foundpos);
					}
					accessPluginName.ChangeCaseLower();
					//�v���g�R����v�����炻�̃R�l�N�V�����^�C�v���|�b�v�A�b�v���j���[�ɐݒ�
					if( protocol.Compare(accessPluginName) == true )
					{
						//
						NVI_SetControlNumber(kPopup_FTPAccount_ConnectionType,i);
					}
				}
				//���[�U�[
				if( user.GetItemCount() > 0 )
				{
					NVI_SetControlText(kText_FTPAccount_User,user);
				}
				//�p�X
				if( path.GetItemCount() > 0 )
				{
					NVI_SetControlText(kText_FTPAccount_RootPath,path);
				}
				//�|�[�g�ԍ�
				if( portnumber > 0 )
				{
					AText	portnumbertext;
					portnumbertext.SetNumber(portnumber);
					NVI_SetControlBool(kRadio_FTPAccount_SpecifyPort,true);
					NVI_SetControlText(kText_FTPAccount_Port,portnumbertext);
				}
			}
			break;
		}
	}
	return result;
}

ABool	AWindow_AppPref::EVTDO_DoubleClicked( const AControlID inID )
{
	ABool	result = false;
	switch(inID)
	{
	  case kTable_LaunchApp:
		{
			ADataID	columnID = NVI_GetTableViewSelectionColumnID(inID);
			if( columnID == kColumn_LaunchApp_App_Name || columnID == kDataID_Invalid )
			{
				AFileAcc	defaultFile;
				defaultFile.Specify("/Applications");
				AText	message;
				NVI_ShowChooseFileWindow(defaultFile,message,kTable_LaunchApp,true);
				result = true;
			}
			/*#205
#if USE_IMP_TABLEVIEW
			AFileAcc	defaultFile;
			defaultFile.Specify("/Applications");
			NVI_ShowChooseFileWindow(defaultFile,kTable_LaunchApp,true);
			result = true;
#else
			if( NVI_GetListView(kTable_LaunchApp).SPI_GetLastClickedColumnID() == kColumn_LaunchApp_App_Name )//win 080618
			{
				AFileAcc	defaultFile;
				defaultFile.Specify("/Applications");
				NVI_ShowChooseFileWindow(defaultFile,kTable_LaunchApp,true);
				result = true;
			}
#endif
			*/
			break;
		}
	}
	return result;
}

void	AWindow_AppPref::EVTDO_FileChoosen( const AControlID inControlID, const AFileAcc& inFile )
{
	switch(inControlID)
	{
		//�閧���t�@�C���I�������� #1231
	  case kButton_FTPAccount_SelectPrivateKey:
		{
			AText	text;
			inFile.GetPath(text);
			NVI_SetControlText(kText_FTPAccount_PrivateKeyFilePath,text);
			break;
		}
			/*#844
	  case kTable_LaunchApp:
		{
			AIndex	index = NVI_GetControlNumber(kTable_LaunchApp);
			if( index != kIndex_Invalid )
			{
				AText	path;
				inFile.GetPath(path);
				GetApp().GetAppPref().SetData_TextArray(AAppPrefDB::kLaunchApp_App,index,path);
				NVM_UpdateDBTableView(kTable_LaunchApp);
			}
			break;
		}
			 */
		/*#844
	  case kButton_SelectTexPreviewer:
		{
			AText	path;
			inFile.GetPath(path);
			GetApp().GetAppPref().SetData_Text(AAppPrefDB::kTexPreviewerPath,path);
			NVM_UpdateControlStatus();
			break;
		}
		*/
	}
}

void	AWindow_AppPref::EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder )
{
	switch(inControlID)
	{
	  case kButton_FTPAccount_BackupFolderPathSelect:
		{
			AText	text;
			inFolder.GetPath(text);//,kFilePathType_1);B0389 OK �ݒ��UNIX�p�X�ɂ���
			NVI_SetControlText(kText_FTPAccount_BackupFolderPath,text);
			break;
		}
		//#81
		//�����o�b�N�A�b�v�t�H���_�I��������
	  case kButton_AutoBackupSelectFolder:
		{
			//�I���t�H���_�̃p�X���擾���A�R���g���[���ɐݒ�
			AText	text;
			inFolder.GetPath(text);
			NVI_SetControlText(AAppPrefDB::kAutoBackupRootFolder,text);
			//DB�ւ̕ۑ��̂��߁A�R���g���[���l�ύX�����������s����
			NVI_DoValueChanged(AAppPrefDB::kAutoBackupRootFolder);
			//�����o�b�N�A�b�v�t�H���_�ύX������
			GetApp().SPI_DoAutoBackupFolderChangedAll();
			break;
		}
		//#160
		//�����t�H���_��r working�t�H���_�I��������
	  case kButton_DiffWorkingSelectFolder:
		{
			//�I���t�H���_�̃p�X���擾���A�R���g���[���ɐݒ�
			AText	text;
			inFolder.GetPath(text);
			NVI_SetControlText(AAppPrefDB::kDiffSourceFolder_Working,text);
			//DB�ւ̕ۑ��̂��߁A�R���g���[���l�ύX�����������s����
			NVI_DoValueChanged(AAppPrefDB::kDiffSourceFolder_Working);
			break;
		}
		//�����t�H���_��r �\�[�X�t�H���_�I��������
	  case kButton_DiffSourceSelectFolder:
		{
			//�I���t�H���_�̃p�X���擾���A�R���g���[���ɐݒ�
			AText	text;
			inFolder.GetPath(text);
			NVI_SetControlText(AAppPrefDB::kDiffSourceFolder_Source,text);
			//DB�ւ̕ۑ��̂��߁A�R���g���[���l�ύX�����������s����
			NVI_DoValueChanged(AAppPrefDB::kDiffSourceFolder_Source);
			break;
		}
	}
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void AWindow_AppPref::NVIDO_Create( const ADocumentID inDocumentID )
{
	//==================�E�C���h�E����==================
	NVM_CreateWindow("apppref/Preference",kWindowClass_Document,true,11);//#0
	//�c�[���o�[����
	NVI_CreateToolbar(1);
	
	//======================== �u��ʁv �^�u ========================
	
	//�L�����b�g�����ݍs���W�I�O���[�v
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_NormalCaret);
	NVI_AddToLastRegisteredRadioGroup(AAppPrefDB::kDrawVerticalLineAsCaret);
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_NoneForCurrentLine);
	NVI_AddToLastRegisteredRadioGroup(AAppPrefDB::kDrawLineForCurrentLine);
	NVI_AddToLastRegisteredRadioGroup(AAppPrefDB::kPaintLineForCurrentLine);
	
	//�L�����b�g�A���ݍs
	NVM_RegisterDBData(AAppPrefDB::kDrawVerticalLineAsCaret,		true);
	NVM_RegisterDBData(AAppPrefDB::kDrawLineForCurrentLine,			true);
	NVM_RegisterDBData(AAppPrefDB::kPaintLineForCurrentLine,		true);
	NVM_RegisterDBData(AAppPrefDB::kCurrentLineColor,				true);
	
	//#1031
	NVM_RegisterDBData(AAppPrefDB::kFixCaretLocalPoint,				false);
	
	//�^�u�^�E�C���h�E���W�I�O���[�v
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(AAppPrefDB::kCreateTabInsteadOfCreateWindow);
	NVI_AddToLastRegisteredRadioGroup(kRadio_OpenDocumentWithWindow);
	
	//�^�u�^�E�C���h�E
	NVM_RegisterDBData(AAppPrefDB::kCreateTabInsteadOfCreateWindow,	true);
	
	//�s�ԍ��N���b�N���̍s�I�𓮍�I�����W�I�O���[�v
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_SelectLineStartByClickLineNumber);
	NVI_AddToLastRegisteredRadioGroup(AAppPrefDB::kSelectLineByClickLineNumber);
	
	//�s�ԍ��N���b�N���̍s�I�𓮍�I��
	NVM_RegisterDBData(AAppPrefDB::kSelectLineByClickLineNumber,	false);
	
	//�g���v���N���b�N���W�I�O���[�v
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_SelectLineByTripleClick);
	NVI_AddToLastRegisteredRadioGroup(AAppPrefDB::kSelectParagraphByTripleClick);
	
	//�g���v���N���b�N
	NVM_RegisterDBData(AAppPrefDB::kSelectParagraphByTripleClick,	false);
	
	//�����o�b�N�A�b�v
	NVM_RegisterDBData(AAppPrefDB::kEnableAutoBackup,				false);
	NVM_RegisterDBData(AAppPrefDB::kAutoBackupRootFolder,			true);
	
	//���C�A�E�g #457
	NVM_RegisterDBData(AAppPrefDB::kShowHScrollBar,					true);
	//NVM_RegisterDBData(AAppPrefDB::kShowVScrollBar,					true);
	NVM_RegisterDBData(AAppPrefDB::kShowMacroBar,					true);
	
	//��e�L�X�g�t�@�C����`
	NVM_RegisterDBData(AAppPrefDB::kBinaryFileDefinition,			false);
	
	//�f�t�H���g�̃��[�h
	NVI_RegisterTextArrayMenu(AAppPrefDB::kCmdNModeName,kTextArrayMenuID_EnabledMode);
	NVM_RegisterDBData(AAppPrefDB::kCmdNModeName,					true);
	
	//Sparkle�����X�V���x�� #1102
	NVM_RegisterDBData(AAppPrefDB::kSparkleUpdateLevel,				true);
	
	//�E�C���h�E�^�C�g���t���p�X�\�� #1182
	NVM_RegisterDBData(AAppPrefDB::kDisplayFullPathTitleBar,		true);

	//======================== �u�T�u�E�C���h�E�v �^�u ========================
	
	//�E�C���h�E�����x
	NVM_RegisterDBData(AAppPrefDB::kSubWindowsAlpha_Floating,		true);
	NVM_RegisterDBData(AAppPrefDB::kSubWindowsAlpha_Popup,			true);
	
	//�⊮���|�b�v�A�b�v�ʒu���W�I�{�^���O���[�v�o�^
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_NoPopupCandidate);
	NVI_AddToLastRegisteredRadioGroup(AAppPrefDB::kPopupCandidateNearInputText);
	NVI_AddToLastRegisteredRadioGroup(AAppPrefDB::kPopupCandidateBelowInputText);
	
	//�⊮���|�b�v�A�b�v�ʒu
	NVM_RegisterDBData(AAppPrefDB::kPopupCandidateNearInputText,		false);
	NVM_RegisterDBData(AAppPrefDB::kPopupCandidateBelowInputText,		false);
	
	/*#1160
	//�����^�O���L�[
	NVM_RegisterDBData(AAppPrefDB::kAbbrevNextKey,					false);
	NVM_RegisterDBData(AAppPrefDB::kAbbrevPrevKey,					false);
	*/
	
	//���L�[�ŕ⊮�|�b�v�A�b�v
	NVM_RegisterDBData(AAppPrefDB::kAbbrevInputByArrowKeyAfterPopup,	false);
	
	//�L�[���[�h���|�b�v�A�b�v�ݒ�
	NVM_RegisterDBData(AAppPrefDB::kPopupIdInfo,					false);
	NVM_RegisterDBData(AAppPrefDB::kHideIdInfoPopupByIdle,			false);
	
	//@note
	NVM_RegisterDBData(AAppPrefDB::kIdInfoAtNoteText,				false);
	
	//======================== �u�e�L�X�g�v �^�u ========================
	
	//�֑�����
	NVM_RegisterDBData(AAppPrefDB::kKinsokuLetters,					true);
	
	//�Ԃ牺����֑��ݒ胉�W�I�{�^���O���[�v
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(AAppPrefDB::kKinsokuBuraasge);
	NVI_AddToLastRegisteredRadioGroup(kRadio_KinsokuNoBurasage);
	
	//�Ԃ牺����֑��ݒ�
	NVM_RegisterDBData(AAppPrefDB::kKinsokuBuraasge,				true);
	
	//�e�L�X�g�G���R�[�f�B���O����
	NVM_RegisterDBData(AAppPrefDB::kUseGuessTextEncoding,			false);
	
	//#830
	NVM_RegisterDBData(AAppPrefDB::kUseFallbackForTextEncodingConversion,false);
	NVM_RegisterDBData(AAppPrefDB::kShowDialogWhenTextEncodingFallback,false);
	
	//�G���R�[�f�B���O���|�[�g�ݒ�
	NVM_RegisterDBData(AAppPrefDB::kCheckEncodingError_KishuIzon,	false);
	NVM_RegisterDBData(AAppPrefDB::kCheckEncodingError_HankakuKana,	false);
	
	//SJIS���X���X�t�H�[���o�b�N
	//#844 SJIS���X���X�t�H�[���o�b�N��drop NVM_RegisterDBData(AAppPrefDB::kUseSJISLosslessFallback,		false);
	
	//======================== �u�����v �^�u ========================
	
	//�����I�v�V�����f�t�H���g
	NVM_RegisterDBData(AAppPrefDB::kFindDefaultIgnoreCase,			false);
	NVM_RegisterDBData(AAppPrefDB::kFindDefaultWholeWord,			false);
	NVM_RegisterDBData(AAppPrefDB::kFindDefaultIgnoreSpaces,		false);//#165
	NVM_RegisterDBData(AAppPrefDB::kFindDefaultIgnoreBackslashYen,	false,kControlID_Invalid,kControlID_Invalid,true);
	NVM_RegisterDBData(AAppPrefDB::kFindDefaultRegExp,				false);
	NVM_RegisterDBData(AAppPrefDB::kFindDefaultLoop,				false);
	NVM_RegisterDBData(AAppPrefDB::kFindDefaultAimai,				false);
	NVM_RegisterDBData(AAppPrefDB::kFindDefaultRecursive,			false);
	NVM_RegisterDBData(AAppPrefDB::kFindDefaultDisplayPosition,		false);
	NVM_RegisterDBData(AAppPrefDB::kFindDefaultOnlyVisibleFile,		false);//B0313
	NVM_RegisterDBData(AAppPrefDB::kFindDefaultOnlyTextFile,		false);//B0313
	
	//�B�������I�v�V����
	NVM_RegisterDBData(AAppPrefDB::kFuzzyFind_NFKD,					true);//#166
	NVM_RegisterDBData(AAppPrefDB::kFuzzyFind_KanaType,				true);//#166
	NVM_RegisterDBData(AAppPrefDB::kFuzzyFind_Onbiki,				true);//#166
	NVM_RegisterDBData(AAppPrefDB::kFuzzyFind_KanaYuragi,			true);//#166
	
	//�����n�C���C�g
	NVM_RegisterDBData(AAppPrefDB::kDisplayFindHighlight,			true);
	NVM_RegisterDBData(AAppPrefDB::kDisplayFindHighlight2,			true);
	NVM_RegisterDBData(AAppPrefDB::kDisplayCurrentWordHighlight,	false);
	
	//�t�H�[�J�X����
	NVM_RegisterDBData(AAppPrefDB::kSelectTextWindowAfterFind,		false);
	NVM_RegisterDBData(AAppPrefDB::kFindShortcutIsFocusSearchBox,	false);
	
	//======================== �u�����[�g�t�@�C���v �^�u ========================
	
	//#193
	//�R�l�N�V�����^�C�v
	NVI_RegisterTextArrayMenu(kPopup_FTPAccount_ConnectionType,kTextArrayMenuID_AccessPlugIn);
	
	//�|�[�g�ԍ�
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_FTPAccount_UseDefaultPort);
	NVI_AddToLastRegisteredRadioGroup(kRadio_FTPAccount_SpecifyPort);
	
	//�����[�g�t�@�C���A�J�E���g�@�e�[�u���̐ݒ�
	NVM_RegisterDBTable(kTable_FTPAccount,AAppPrefDB::kFTPAccount_Server,
				kButton_FTPAccount_Add,kButton_FTPAccount_Remove,kButton_FTPAccount_RemoveOK,
				kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
				kControlID_Invalid,false,true,false,true,false);
	NVM_RegisterDBTableColumn(	kTable_FTPAccount,kColumn_FTPAccount_Server,			AAppPrefDB::kFTPAccount_Server,
								kControlID_Invalid,					0,"",false);//win 080618
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kText_FTPAccount_Server,		AAppPrefDB::kFTPAccount_Server);
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kText_FTPAccount_User,			AAppPrefDB::kFTPAccount_User);
	NVM_RegisterDBTableSpecializedContentView(kTable_FTPAccount,kText_FTPAccount_Password);
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kCheck_FTPAccount_Passive,		AAppPrefDB::kFTPAccount_Passive);
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kText_FTPAccount_Port,			AAppPrefDB::kFTPAccount_Port);
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kCheck_FTPAccount_BinaryMode,	AAppPrefDB::kFTPAccount_BinaryMode);
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kText_FTPAccount_RootPath,		AAppPrefDB::kFTPAccount_RootPath);
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kText_FTPAccount_HTTPURL,		AAppPrefDB::kFTPAccount_HTTPURL);
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kPopup_FTPAccount_ProxyMode,	AAppPrefDB::kFTPAccount_ProxyMode);
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kText_FTPAccount_ProxyServer,	AAppPrefDB::kFTPAccount_ProxyServer);
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kText_FTPAccount_ProxyUser,		AAppPrefDB::kFTPAccount_ProxyUser);
	NVM_RegisterDBTableSpecializedContentView(kTable_FTPAccount,kRadio_FTPAccount_UseDefaultPort);
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kText_FTPAccount_DisplayName,	AAppPrefDB::kFTPAccount_DisplayName);
	NVM_RegisterDBTableSpecializedContentView(kTable_FTPAccount,kText_FTPAccount_ProxyPassword);
	//NVM_RegisterDBTableContentView(kTable_FTPAccount,kCheck_FTPAccount_Backup,		AAppPrefDB::kFTPAccount_Backup);
	//NVM_RegisterDBTableContentView(kTable_FTPAccount,kText_FTPAccount_BackupFolderPath,AAppPrefDB::kFTPAccount_BackupFolderPath);
	//NVM_RegisterDBTableContentView(kTable_FTPAccount,kCheck_FTPAccount_BackupWithDateTime,AAppPrefDB::kFTPAccount_BackupWithDateTime);//R0207
	NVM_RegisterDBTableSpecializedContentView(kTable_FTPAccount,kPopup_FTPAccount_ConnectionType);//#361
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kCheck_FTPAccount_UsePrivateKey,	AAppPrefDB::kFTPAccount_UsePrivateKey);//#1231
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kText_FTPAccount_PrivateKeyFilePath,AAppPrefDB::kFTPAccount_PrivateKeyFilePath);//#1231
	NVM_RegisterDBTableContentView(kTable_FTPAccount,kCheck_FTPAccount_DontCheckServerCert,AAppPrefDB::kFTPAccount_DontCheckServerCert);//#1231
	
	//======================== �u��r�v �^�u ========================
	
	//��r�\���̐F
	NVM_RegisterDBData(AAppPrefDB::kDiffColor_Changed,				true);
	NVM_RegisterDBData(AAppPrefDB::kDiffColor_Added,				true);
	NVM_RegisterDBData(AAppPrefDB::kDiffColor_Deleted,				true);
	
	//diff target�̍s�܂�Ԃ�
	NVM_RegisterDBData(AAppPrefDB::kShowDiffTargetWithWordWrap,		true);
	
	//SCM�g�p�ݒ�
	NVM_RegisterDBData(AAppPrefDB::kUseSCM,							true);
	
	//�p�X�A�I�v�V����
	NVM_RegisterDBData(AAppPrefDB::kSVNPath,						false);
	NVM_RegisterDBData(AAppPrefDB::kDiffPath,						false);
	NVM_RegisterDBData(AAppPrefDB::kDiffOption,						false);
	NVM_RegisterDBData(AAppPrefDB::kGitPath,						false);//#589
	
	//#160
	//�����t�H���_��r
	NVM_RegisterDBData(AAppPrefDB::kDiffSourceFolder_Enable,		false);
	NVM_RegisterDBData(AAppPrefDB::kDiffSourceFolder_Source,		false);
	NVM_RegisterDBData(AAppPrefDB::kDiffSourceFolder_Working,		false);
	
	//======================== �u����v �^�u ========================
	
	//����I�v�V����
	NVM_RegisterDBData(AAppPrefDB::kPrintColored,					false);
	NVM_RegisterDBData(AAppPrefDB::kPrintBlackForWhite,				false);
	NVM_RegisterDBData(AAppPrefDB::kPrintEachParagraphAlways,		false);
	
	//����v���p�e�B
	NVM_RegisterDBData(AAppPrefDB::kPrintOption_UsePrintFont,		false);
	NVM_RegisterDBData(AAppPrefDB::kPrintOption_PrintPageNumber,	false);
	NVM_RegisterDBData(AAppPrefDB::kPrintOption_PrintFileName,		false);
	NVM_RegisterDBData(AAppPrefDB::kPrintOption_PrintSeparateLine,	false);
	NVM_RegisterDBData(AAppPrefDB::kPrintOption_PrintLineNumber,	false);
	NVM_RegisterDBData(AAppPrefDB::kPrintOption_LineMargin,			false);
	NVM_RegisterDBData(AAppPrefDB::kPrintOption_ForceWordWrap,		false);
	
	//����]��
	NVM_RegisterDBData(AAppPrefDB::kPrintMargin_Left,				false);
	NVM_RegisterDBData(AAppPrefDB::kPrintMargin_Right,				false);
	NVM_RegisterDBData(AAppPrefDB::kPrintMargin_Top,				false);
	NVM_RegisterDBData(AAppPrefDB::kPrintMargin_Bottom,				false);
	
	//======================== �u�@�\�v �^�u ========================
	
	//�V�F���X�N���v�g
	NVM_RegisterDBData(AAppPrefDB::kUseShellScript,					false);
	NVI_RegisterTextArrayMenu(AAppPrefDB::kStdOutModeName,kTextArrayMenuID_EnabledMode);
	NVM_RegisterDBData(AAppPrefDB::kStdOutModeName,					false);
	
	//�t�H���_���x���i�ŋߊJ�����t�@�C���ɒ��߂�����j�@�e�[�u���̐ݒ�
	NVM_RegisterDBTable(kTable_RecentlyOpendFileComment,AAppPrefDB::kRecentlyOpenedFileComment,
						kButton_RecentlyOpenedFileCommentAddNew,kButton_RecentlyOpenedFileCommentDelete,kButton_RecentlyOpenedFileCommentDeleteOK,
						kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
						kControlID_Invalid,true,true,true,true,false);
	NVM_RegisterDBTableColumn(	kTable_RecentlyOpendFileComment,kColumn_RecentlyOpenedFileCommentPath,AAppPrefDB::kRecentlyOpenedFileCommentPath,
							  kButton_Edit_Recently_Path,			290,"AppPref_RecentlyOpenedFileComment_ListViewTitle0",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_RecentlyOpendFileComment,kColumn_RecentlyOpenedFileComment,	AAppPrefDB::kRecentlyOpenedFileComment,
							  kButton_Edit_Recently_Comment,		0,"AppPref_RecentlyOpenedFileComment_ListViewTitle1",true);//win 080618
	
	//======================== �u�ڍׁv �^�u ========================
	
	//�z�C�[���X�N���[��
	NVM_RegisterDBData(AAppPrefDB::kWheelScrollPercent,				false);
	NVM_RegisterDBData(AAppPrefDB::kWheelScrollPercentCmd,			false);
	NVM_RegisterDBData(AAppPrefDB::kDisableHorizontalWheelScroll,	false);
	
	//#621
	NVM_RegisterDBData(AAppPrefDB::kBottomScrollAt25Percent,		false);
	
	//�_�u���N���b�N�ɂ�镶�ߑI��
	NVM_RegisterDBData(AAppPrefDB::kBunsetsuSelect,					false);
	
	//�E�N���b�N
	NVM_RegisterDBData(AAppPrefDB::kSelectWordByContextMenu,		false);//B0432
	NVM_RegisterDBData(AAppPrefDB::kCMMouseButton,					false);
	NVM_RegisterDBData(AAppPrefDB::kCMMouseButtonTick,				false,AAppPrefDB::kCMMouseButton);
	
	//�^�u����
	NVM_RegisterDBData(AAppPrefDB::kCloseAllTabsWhenCloseWindow,	false);
	NVM_RegisterDBData(AAppPrefDB::kTabToWindowPositionOffset,		false);
	
	//���I�[�v��
	NVM_RegisterDBData(AAppPrefDB::kReopenFile,						false);
	
	//�����L��
	NVM_RegisterDBData(AAppPrefDB::kDontRememberAnyHistory,			false);
	
	//�r������
	NVM_RegisterDBData(AAppPrefDB::kLockWhileEdit,					false);
	
	//�f�t�H���g�E�C���h�E�T�C�Y
	NVM_RegisterDBData(AAppPrefDB::kDefaultWindowWidth,				false);
	NVM_RegisterDBData(AAppPrefDB::kDefaultWindowHeight,			false);
	
	//�X�e�[�^�X�o�[
	NVM_RegisterDBData(AAppPrefDB::kDisplayStatusBar,				true);
	
	//����R�[�h�\��
	NVM_RegisterDBData(AAppPrefDB::kDisplayControlCode,				true);
	
	//#863
	//Unicode decomposition
	NVM_RegisterDBData(AAppPrefDB::kDisplayEachCanonicalDecompChar,	true);
	//NVM_RegisterDBData(AAppPrefDB::kFindDistinguishCanonicalCompDecomp,false);
	
	//�S�p�����J�E���g���@
	NVM_RegisterDBData(AAppPrefDB::kCountAs2Letters,				true);
	
	//�L�����b�g
	NVM_RegisterDBData(AAppPrefDB::kBigCaret,						false);//R0184
	
	//�A���`�G�C���A�X
	NVM_RegisterDBData(AAppPrefDB::kEnableAntiAlias,				true);
	
	//�A���`�G�C���A�X
	NVM_RegisterDBData(AAppPrefDB::kTexTypesetPath,					false);
	NVM_RegisterDBData(AAppPrefDB::kTexDviToPdfPath,				false);
	
	//skin
	NVI_RegisterTextArrayMenu(AAppPrefDB::kSkinName,kTextArrayMenuID_Skin);
	NVM_RegisterDBData(AAppPrefDB::kSkinName,						true);
	
	//�h�b�g�P�ʃX�N���[�� #891
	NVM_RegisterDBData(AAppPrefDB::kUseWheelScrollFloatValue,		true);
	
	//#1044
	//NFC�����ϊ��ۑ��E�R�s�[
	NVM_RegisterDBData(AAppPrefDB::kAutoConvertToCanonicalComp,		false);
	
	//�e�L�X�g�̏�ł����Ă�I�r�[���J�[�\���ɂ��Ȃ��ݒ� #1208
	NVM_RegisterDBData(AAppPrefDB::kInhibitIbeamCursor,				false);
	
	//���E�]�� #1186
	NVM_RegisterDBData(AAppPrefDB::kDisplayLeftRightMargin,			true);
	NVM_RegisterDBData(AAppPrefDB::kTextWidthForLeftRightMargin,	true,AAppPrefDB::kDisplayLeftRightMargin);
	
	//�������E�C���h�E#1255
	NVM_RegisterDBData(AAppPrefDB::kAlphaWindowModeV3,				true);
	NVM_RegisterDBData(AAppPrefDB::kAlphaWindowPercent1V3,			true,AAppPrefDB::kAlphaWindowModeV3);
	NVM_RegisterDBData(AAppPrefDB::kAlphaWindowPercent2V3,			true,AAppPrefDB::kAlphaWindowModeV3);
	
	//�^�u�ɐe�t�H���_���\�� #1334
	NVM_RegisterDBData(AAppPrefDB::kTabShowParentFolder,			true);
	
	//�������ʃ|�b�v�A�b�v���Ȃ��I�v�V���� #1322
	NVM_RegisterDBData(AAppPrefDB::kDontShowFindResultPopup,		false);
	
	//�^�u�� #1349
	NVM_RegisterDBData(AAppPrefDB::kTabWidth,						true);
	
	/*#844
	//�N���A�v���P�[�V�����@�e�[�u���̐ݒ�
	NVM_RegisterDBTable(kTable_LaunchApp,AAppPrefDB::kLaunchApp_CheckExtension,
			kButton_LaunchApp_Add,kButton_LaunchApp_Remove,kButton_LaunchApp_RemoveOK,
			kButton_LaunchApp_Up,kButton_LaunchApp_Down,kControlID_Invalid,kControlID_Invalid,
			kControlID_Invalid,false,
			true,true,true,false);//win 080618
	NVM_RegisterDBTableColumn(	kTable_LaunchApp,kColumn_LaunchApp_CheckExtension,	AAppPrefDB::kLaunchApp_CheckExtension,
								kControlID_Invalid,					30,"",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_LaunchApp,kColumn_LaunchApp_Extension,			AAppPrefDB::kLaunchApp_Extension,
								kButton_Edit_Launch_Suffix,			60,"AppPref_LaunchApp_ListViewTitle1",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_LaunchApp,kColumn_LaunchApp_CheckCreator,		AAppPrefDB::kLaunchApp_CheckCreator,
								kControlID_Invalid,					30,"",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_LaunchApp,kColumn_LaunchApp_Creator,			AAppPrefDB::kLaunchApp_Creator,
								kButton_Edit_Launch_Creator,		60,"AppPref_LaunchApp_ListViewTitle3",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_LaunchApp,kColumn_LaunchApp_CheckType,			AAppPrefDB::kLaunchApp_CheckType,
								kControlID_Invalid,					30,"",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_LaunchApp,kColumn_LaunchApp_Type,				AAppPrefDB::kLaunchApp_Type,
								kButton_Edit_Launch_Type,			60,"AppPref_LaunchApp_ListViewTitle5",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_LaunchApp,kColumn_LaunchApp_ShiftKey,			AAppPrefDB::kLaunchApp_ShiftKey,
								kControlID_Invalid,					50,"AppPref_LaunchApp_ListViewTitle6",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_LaunchApp,kColumn_LaunchApp_App_Name,			kDataType_TextArray,
								kControlID_Invalid,					200,"AppPref_LaunchApp_ListViewTitle7",false);//win 080618
	NVM_RegisterDBTableColumn(	kTable_LaunchApp,kColumn_LaunchApp_App_Path,			AAppPrefDB::kLaunchApp_App,
								kControlID_Invalid,					0,"",false);//win 080618
	*/
	
	//======================== ���̑����� ========================
	
	//
#if IMPLEMENTATION_FOR_WINDOWS
	AFileAcc	sendtofolder;
	AFileWrapper::GetSendToFolder(sendtofolder);
	AText	shortcutfilename;
	GetApplicationName(shortcutfilename);
	shortcutfilename.AddCstring(".lnk");
	AFileAcc	linkfile;
	linkfile.SpecifyChild(sendtofolder,shortcutfilename);
	NVI_SetControlBool(kCheck_RegisterToSendToMenu,linkfile.Exist());
#endif
	
	//�^�u�R���g���[���̍ŏ��̃^�u��I��
	NVI_SelectTabControl(0);
	
	/*
#if IMPLEMENTATION_FOR_WINDOWS
	AFileAcc	sendtofolder;
	AFileWrapper::GetSendToFolder(sendtofolder);
	AText	shortcutfilename;
	GetApplicationName(shortcutfilename);
	shortcutfilename.AddCstring(".lnk");
	AFileAcc	linkfile;
	linkfile.SpecifyChild(sendtofolder,shortcutfilename);
	NVI_SetControlBool(kCheck_RegisterToSendToMenu,linkfile.Exist());
#endif
	
	//�w���v�{�^���o�^
	NVI_RegisterHelpAnchor(90001,"pref.htm#apppref_hyouji");
	NVI_RegisterHelpAnchor(90002,"pref.htm#apppref_sousa");
	NVI_RegisterHelpAnchor(90003,"pref.htm#apppref_tabview");
	NVI_RegisterHelpAnchor(90004,"pref.htm#apppref_kensaku");
	NVI_RegisterHelpAnchor(90005,"pref.htm#apppref_menu");
	NVI_RegisterHelpAnchor(90006,"pref.htm#apppref_text");
	NVI_RegisterHelpAnchor(90007,"pref.htm#apppref_ftp");
	NVI_RegisterHelpAnchor(90008,"pref.htm#apppref_tool");
	NVI_RegisterHelpAnchor(90009,"pref.htm#apppref_sonota");
	NVI_RegisterHelpAnchor(90010,"pref.htm#apppref_touroku");
	NVI_RegisterHelpAnchor(90011,"pref.htm#apppref_scm");
	*/
}

#pragma mark ===========================

#pragma mark <�ĉ����\�b�h��������>

#pragma mark ===========================

//�R���g���[����ԁiEnable/Disable���j���X�V
//NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()����R�[�������
//�N���b�N��A�N�e�B�x�[�g�ŃR�[�������̂ŁA���܂�d��������NVMDO_UpdateControlStatus()�ɂ͓���Ȃ����ƁB
void	AWindow_AppPref::NVMDO_UpdateControlStatus()
{
	const ADataBase& apppref = GetApp().NVI_GetAppPrefDB();
	
	AText	text;
	
	//�e�t�H���g�R���g���[���X�V
	//#725
	apppref.GetData_Text(AAppPrefDB::kSubWindowsFontName,text);
	NVI_SetControlFont(kFontControl_SubWindowsFont,text,apppref.GetData_FloatNumber(AAppPrefDB::kSubWindowsFontSize));
	//#688
	apppref.GetData_Text(AAppPrefDB::kFindDialogFontName,text);
	NVI_SetControlFont(kFontControl_FindFont,text,apppref.GetData_FloatNumber(AAppPrefDB::kFind_FontSize));
	//#868
	apppref.GetData_Text(AAppPrefDB::kPrintOption_PrintFontName,text);
	NVI_SetControlFont(kFontControl_PrintFont,text,apppref.GetData_FloatNumber(AAppPrefDB::kPrintOption_PrintFontSize));
	//#868
	apppref.GetData_Text(AAppPrefDB::kPrintOption_PageNumberFontName,text);
	NVI_SetControlFont(kFontControl_PrintPageNumberFont,text,apppref.GetData_FloatNumber(AAppPrefDB::kPrintOption_PageNumberFontSize));
	//#868
	apppref.GetData_Text(AAppPrefDB::kPrintOption_FileNameFontName,text);
	NVI_SetControlFont(kFontControl_PrintFileNameFont,text,apppref.GetData_FloatNumber(AAppPrefDB::kPrintOption_FileNameFontSize));
	//#868
	apppref.GetData_Text(AAppPrefDB::kPrintOption_LineNumberFontName,text);
	NVI_SetControlFont(kFontControl_PrintLineNumberFont,text,apppref.GetData_FloatNumber(AAppPrefDB::kPrintOption_LineNumberFontSize));
	
	//#81
	//�����o�b�N�A�b�v�`�F�b�N�{�b�N�X�A��
	{
		ABool	b = NVI_GetControlBool(AAppPrefDB::kEnableAutoBackup);
		NVI_SetControlEnable(AAppPrefDB::kAutoBackupRootFolder,b);
		NVI_SetControlEnable(kButton_AutoBackupSetDefault,b);
		NVI_SetControlEnable(kButton_AutoBackupSelectFolder,b);
	}
	//#160
	//�����t�H���_��r�`�F�b�N�{�b�N�X�A��
	{
		ABool	b = NVI_GetControlBool(AAppPrefDB::kDiffSourceFolder_Enable);
		NVI_SetControlEnable(AAppPrefDB::kDiffSourceFolder_Source,b);
		NVI_SetControlEnable(AAppPrefDB::kDiffSourceFolder_Working,b);
		NVI_SetControlEnable(kButton_DiffSourceSelectFolder,b);
		NVI_SetControlEnable(kButton_DiffWorkingSelectFolder,b);
	}
	//�����[�g�A�J�E���g
	{
		AIndex	rowIndex = NVI_GetListView(kTable_FTPAccount).SPI_GetSelectedDBIndex();
		ABool	b = (rowIndex != kIndex_Invalid);
		//
		NVI_SetControlEnable(kPopup_FTPAccount_ConnectionType,b);
		NVI_SetControlEnable(kText_FTPAccount_Password,b);
		NVI_SetControlEnable(kText_FTPAccount_ProxyPassword,b);
		NVI_SetControlEnable(kRadio_FTPAccount_UseDefaultPort,b);
		NVI_SetControlEnable(kRadio_FTPAccount_SpecifyPort,b);
		//#1231
		AText	connectionType;
		NVI_GetControlText(kPopup_FTPAccount_ConnectionType,connectionType);
		if( connectionType.Compare("FTP") == true )
		{
			NVI_SetControlEnable(kCheck_FTPAccount_UsePrivateKey,false);
			NVI_SetControlEnable(kText_FTPAccount_PrivateKeyFilePath,false);
			NVI_SetControlEnable(kButton_FTPAccount_SelectPrivateKey,false);
			NVI_SetControlEnable(kStaticText_FTPAccount_PrivateKeyPass,false);
		}
		else
		{
			NVI_SetControlEnable(kCheck_FTPAccount_UsePrivateKey,b);
			ABool	usePrivateKey = NVI_GetControlBool(kCheck_FTPAccount_UsePrivateKey);
			NVI_SetControlEnable(kText_FTPAccount_PrivateKeyFilePath,b&&usePrivateKey);
			NVI_SetControlEnable(kButton_FTPAccount_SelectPrivateKey,b&&usePrivateKey);
			NVI_SetControlEnable(kStaticText_FTPAccount_PrivateKeyPass,b&&usePrivateKey);
		}
		//
		if( b == true )
		{
			//#1231
			if( connectionType.Compare("FTP") == true )
			{
				NVI_SetControlEnable(kPopup_FTPAccount_ProxyMode,true);
				//
				b = (NVI_GetControlNumber(kPopup_FTPAccount_ProxyMode) != AAppPrefDB::kProxyMode_NoProxy);
				NVI_SetControlEnable(kText_FTPAccount_ProxyServer,b);
				b = (NVI_GetControlNumber(kPopup_FTPAccount_ProxyMode) == AAppPrefDB::kProxyMode_FTPProxy2);
				NVI_SetControlEnable(kText_FTPAccount_ProxyUser,b);
				NVI_SetControlEnable(kText_FTPAccount_ProxyPassword,b);
			}
			else
			{
				NVI_SetControlEnable(kPopup_FTPAccount_ProxyMode,false);
				NVI_SetControlEnable(kText_FTPAccount_ProxyServer,false);
				NVI_SetControlEnable(kText_FTPAccount_ProxyUser,false);
				NVI_SetControlEnable(kText_FTPAccount_ProxyPassword,false);
			}
		}
	}
	/*
#if IMPLEMENTATION_FOR_MACOSX
	{
		AText	text;
		GetApp().GetAppPref().GetData_Text(AAppPrefDB::kTexPreviewerPath,text);
		AFileAcc	file;
		file.Specify(text);
		AText	path;
		//B0389 OK file.GetPath(path,kFilePathType_1);
		GetApp().GetAppPref().GetFilePathForDisplay(file,path);//B0389
		NVI_SetControlText(kText_TexPreviewerPath,path);
	}
#endif
	*/
	//#1102
	NVI_SetControlEnable(AAppPrefDB::kSparkleUpdateLevel,(AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7));
}

/**
�\���X�V
*/
void	AWindow_AppPref::NVIDO_UpdateProperty()
{
	//�^�u�^�E�C���h�E���W�I�{�^���X�V
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == false )
	{
		NVI_SetControlBool(kRadio_OpenDocumentWithWindow,true);
	}
}

//�e�[�u���̕\����DB�̃f�[�^�ɍ��킹�čX�V
void	AWindow_AppPref::NVMDO_UpdateTableView( const AControlID inTableControlID, const ADataID inColumnID )
{
	switch(inTableControlID)
	{
			/*#844
	  case kTable_LaunchApp:
		{
			switch(inColumnID)
			{
			  case kColumn_LaunchApp_App_Name:
				{
					ATextArray	textArray;
					for( AIndex i = 0; i < GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kLaunchApp_App); i++ )
					{
						AText	path;
						GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kLaunchApp_App,i,path);
						AText	filename;
						path.GetFilename(filename);
						textArray.Add(filename);
					}
					//�e�[�u���ɐݒ�(BeginSetTable/EndSetTable��AWindow���Ŏ��s�����)
					NVI_SetTableColumn_Text(kTable_LaunchApp,kColumn_LaunchApp_App_Name,textArray);//#205
					break;
				}
			}
			break;
		}
			 */
		/*#844
	  case kTable_TextEncodingMenu:
		{
			switch(inColumnID)
			{
			  case kColumn_TextEncodingMenu:
				{
					ANumberArray	iconIDArray;
					AItemCount	count = GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kTextEncodingMenu);
					for( AIndex i = 0; i < count; i++ )
					{
						AText	text;
						GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kTextEncodingMenu,i,text);
						if( text.Compare("-") == true || ATextEncodingWrapper::CheckTextEncodingAvailability(text) == true )
						{
							iconIDArray.Add(kIndex_Invalid);
						}
						else
						{
							iconIDArray.Add(kIconID_Mi_Batsu);
						}
					}
					//�e�[�u���ɐݒ�(BeginSetTable/EndSetTable��AWindow���Ŏ��s�����)
					NVI_SetTableColumn_Icon(kTable_TextEncodingMenu,kColumn_TextEncodingMenu,iconIDArray);//#205
					break;
				}
			}
			break;
		}
		*/
	}
}

//AWindow�Ŏ����Ǘ�����Ȃ�ContentView���ڂ�\��
void	AWindow_AppPref::NVMDO_OpenSpecializedContentView( const AControlID inTableControlID, const AIndex inRowIndex )
{
	switch(inTableControlID)
	{
#if IMPLEMENTATION_FOR_MACOSX
	  case kTable_FTPAccount:
		{
			AText	text;
			if( inRowIndex != kIndex_Invalid )
			{
				/*
				AText	server, user, password;
				GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_Server,inRowIndex,server);
				GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_Server,inRowIndex,user);
				ASecurityWrapper::GetInternetPassword(server,user,password);
				if( password.GetItemCount() > 0 )
				*/
				{
					text.SetCstring("\t\t\t\t\t\t\t\t");
				}
			}
			NVI_SetControlText(kText_FTPAccount_Password,text);
			NVI_SetControlText(kText_FTPAccount_ProxyPassword,text);
			//#361
			//�ڑ��^�C�v�|�b�v�A�b�v�R���g���[���̍X�V
			if( inRowIndex != kIndex_Invalid )
			{
				//�ڑ��^�C�v��DB����擾
				AText	connectionTypeText;
				GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_ConnectionType,inRowIndex,connectionTypeText);
				if( connectionTypeText.GetItemCount() == 0 )
				{
					connectionTypeText.SetCstring("SFTP");
				}
				//�|�b�v�A�b�v�R���g���[���Ƀ��j���[index�ݒ�
				NVI_SetControlText(kPopup_FTPAccount_ConnectionType,connectionTypeText);
				//�|�[�g�ԍ����W�I�{�^��
				if( GetApp().NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kFTPAccount_UseDefaultPort,inRowIndex) == true )
				{
					NVI_SetControlBool(kRadio_FTPAccount_UseDefaultPort,true);
				}
				else
				{
					NVI_SetControlBool(kRadio_FTPAccount_SpecifyPort,true);
				}
			}
			break;
		}
#endif
	}
}


//AWindow�Ŏ����Ǘ�����Ȃ�ContentView���ڂ��f�[�^�x�[�X�֐ݒ� B0406
void	AWindow_AppPref::NVMDO_SaveSpecializedContentView( const AControlID inTableControlID, const AIndex inRowIndex )
{
	switch(inTableControlID)
	{
#if IMPLEMENTATION_FOR_MACOSX
	  case kTable_FTPAccount:
		{
			AText	server, user, pass;
			NVI_GetControlText(kText_FTPAccount_Server,server);
			NVI_GetControlText(kText_FTPAccount_User,user);
			NVI_GetControlText(kText_FTPAccount_Password,pass);
			if( pass.Compare("\t\t\t\t\t\t\t\t") == false )
			{
				ASecurityWrapper::SetInternetPassword(server,user,pass);
			}
			
			NVI_GetControlText(kText_FTPAccount_ProxyServer,server);
			NVI_GetControlText(kText_FTPAccount_ProxyUser,user);
			NVI_GetControlText(kText_FTPAccount_ProxyPassword,pass);
			if( pass.Compare("\t\t\t\t\t\t\t\t") == false )
			{
				ASecurityWrapper::SetInternetPassword(server,user,pass);
			}
			//�Z�L�����e�B��A�p�X���[�h���A�v���������ԕێ�����̂͗ǂ������̂ŖY���B
			pass.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
			//#361
			//�ڑ��^�C�v�|�b�v�A�b�v��DB�ւ̕ۑ�
			//�|�b�v�A�b�v�R���g���[���̃��j���[index����ڑ��^�C�v�e�L�X�g�ւ̕ϊ�
			AText	connectionTypeText;
			NVI_GetControlText(kPopup_FTPAccount_ConnectionType,connectionTypeText);
			//�ڑ��^�C�v�e�L�X�g��DB�ɕۑ�
			GetApp().NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kFTPAccount_ConnectionType,inRowIndex,connectionTypeText);
			//�|�[�g�ԍ����W�I�{�^��
			GetApp().NVI_GetAppPrefDB().SetData_BoolArray(AAppPrefDB::kFTPAccount_UseDefaultPort,inRowIndex,NVI_GetControlBool(kRadio_FTPAccount_UseDefaultPort));
			break;
		}
#endif
	}
}
//�u�e�[�u�����eView�v�̒l���e�[�u��DB�֐ݒ肵�A�e�[�u���\���X�V
/*B0406 void	AWindow_AppPref::NVMDO_SetDBTableDataFromContentView( const AControlID inTableControlID, const AControlID inContentControlID, const AIndex inRowIndex )
{
	switch(inTableControlID)
	{
	  case kTable_FTPAccount:
		{
			switch(inContentControlID)
			{
			  case kText_FTPAccount_Password:
				{
					AText	server, user, pass;
					NVI_GetControlText(kText_FTPAccount_Server,server);
					NVI_GetControlText(kText_FTPAccount_User,user);
					NVI_GetControlText(kText_FTPAccount_Password,pass);
					if( pass.Compare("\t\t\t\t\t\t\t\t") == false )
					{
						ASecurityWrapper::SetInternetPassword(server,user,pass);
					}
					//�Z�L�����e�B��A�p�X���[�h���A�v���������ԕێ�����̂͗ǂ������̂ŖY���B
					pass.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
					break;
				}
			  case kText_FTPAccount_ProxyPassword:
				{
					AText	server, user, pass;
					NVI_GetControlText(kText_FTPAccount_ProxyServer,server);
					NVI_GetControlText(kText_FTPAccount_ProxyUser,user);
					NVI_GetControlText(kText_FTPAccount_ProxyPassword,pass);
					if( pass.Compare("\t\t\t\t\t\t\t\t") == false )
					{
						ASecurityWrapper::SetInternetPassword(server,user,pass);
					}
					//�Z�L�����e�B��A�p�X���[�h���A�v���������ԕێ�����̂͗ǂ������̂ŖY���B
					pass.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
					break;
				}
			}
			break;
		}
	}
}*/

/**
���X�gview�ւ̃t�@�C���h���b�v������
*/
void	AWindow_AppPref::NVIDO_ListViewFileDropped( const AControlID inControlID, const AIndex inRowIndex, const AFileAcc& inFile )
{
	switch(inControlID)
	{
	  case kTable_RecentlyOpendFileComment:
		{
			//�V�K�f�[�^�ǉ�
			AText	filepath;
			inFile.GetPath(filepath);
			AText	comment;
			filepath.GetFilename(comment);
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kRecentlyOpenedFileCommentPath,filepath);
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kRecentlyOpenedFileComment,comment);
			//�e�[�u���X�V
			NVM_UpdateDBTableView(kTable_RecentlyOpendFileComment);
			break;
		}
	}
}

//
void	AWindow_AppPref::NVMDO_NotifyDataChanged( const AControlID inControlID, const AModificationType inModificationType, const AIndex inIndex,
		const AControlID inTriggerControlID/*B0406*/ )
{
	switch(inControlID)
	{
		//#868
	  case AAppPrefDB::kEnableAntiAlias:
	  case AAppPrefDB::kFuzzyFind_NFKD:
	  case AAppPrefDB::kFuzzyFind_KanaType:
	  case AAppPrefDB::kFuzzyFind_Onbiki:
	  case AAppPrefDB::kFuzzyFind_KanaYuragi:
		{
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		//#868
	  case AAppPrefDB::kSubWindowsAlpha_Floating:
	  case AAppPrefDB::kSubWindowsAlpha_Popup:
		{
			GetApp().SPI_UpdateSubWindowsProperty();
			break;
		}
		//#844
	  case AAppPrefDB::kDrawVerticalLineAsCaret:
	  case AAppPrefDB::kDrawLineForCurrentLine:
	  case AAppPrefDB::kPaintLineForCurrentLine:
	  case AAppPrefDB::kCurrentLineColor:
		{
			GetApp().SPI_RefreshCaretAll();
			GetApp().NVI_RefreshAllWindow();
			break;
		}
	  case AAppPrefDB::kKinsokuBuraasge:
		{
			GetApp().SPI_RecalcWordWrapAll(kIndex_Invalid);
			break;
		}
		//���C�A�E�g #457
	  case AAppPrefDB::kDisplayStatusBar:
	  case AAppPrefDB::kShowHScrollBar:
	  case AAppPrefDB::kShowVScrollBar:
	  case AAppPrefDB::kShowMacroBar:
		{
			GetApp().SPI_UpdateTextWindowViewBoundsAll();
			break;
		}
		/*#844
		//
	  case AAppPrefDB::kAppDefaultFont://#375
		{
			AText	text;
			NVI_GetControlText(AAppPrefDB::kAppDefaultFont,text);
			AFontWrapper::SetAppDefaultFontName(text);
			GetApp().NVI_UpdateAll();
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		*/
	  case AAppPrefDB::kDisplayControlCode:
	  case AAppPrefDB::kCountAs2Letters:
	  case AAppPrefDB::kDisplayEachCanonicalDecompChar://#863
		{
			GetApp().SPI_RecalcAll(kIndex_Invalid);
			break;
		}
	  case AAppPrefDB::kKinsokuLetters:
		{
			GetApp().GetAppPref().UpdateKinsokuLettersHash();
			GetApp().SPI_RecalcWordWrapAll(kIndex_Invalid);
			break;
		}
	  /*B0000 case AAppPrefDB::kInsertLineMarginForAntiAlias:
		{
			GetApp().SPI_UpdateTextDrawPropertyAll(kIndex_Invalid);
			break;
		}*/
			/*#844
	  case AAppPrefDB::kWindowShortcut:
		{
			GetApp().SPI_UpdateWindowMenu();
			break;
		}*/
		/*#844
	  case AAppPrefDB::kCommandMIsMultiFileFind:
		{
			GetApp().SPI_UpdateFixedMenuShortcut();
			break;
		}
		*/
	  case AAppPrefDB::kCmdNModeName:
		{
			GetApp().SPI_UpdateNewMenu();
			break;
		}
	  //win case AAppPrefDB::kFindDialogFont:
	  case AAppPrefDB::kFindDialogFontName://win
	  case AAppPrefDB::kFind_FontSize:
		{
			GetApp().SPI_UpdateFindWindow();
			GetApp().SPI_UpdateMultiFileFindWindow();
			break;
		}
	  case AAppPrefDB::kDisplayFindHighlight:
	  case AAppPrefDB::kDisplayFindHighlight2://R0244
		{
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		/*#725
	  case AAppPrefDB::kOverlayWindowsAlpha://#291
		{
			GetApp().SPI_UpdateOverlayWindowsAlpha();
			break;
		}
		*/
	  case kTable_TextEncodingMenu:
		{
			GetApp().SPI_UpdateTextEncodingMenu();
			break;
		}
		/*#844
	  //win case AAppPrefDB::kJumpListFont:
		//#844 case AAppPrefDB::kJumpListFontName://win
		//#844 case AAppPrefDB::kJumpListFontSize:
		//#844 case AAppPrefDB::kJumpListAlphaPercent:
	  case AAppPrefDB::kJumpListPosition:
		{
			//#725 GetApp().SPI_UpdatePropertyJumpListAll();
			GetApp().SPI_UpdateFileListWindowsProperty();//#725
			break;
		}
		*/
		/*#844
		case AAppPrefDB::kFileListFontName://win
		case AAppPrefDB::kFileListFontSize:
	  case AAppPrefDB::kWindowListAlphaPercent:
		{
			//#725 GetApp().SPI_UpdateFileListProperty();
			GetApp().SPI_UpdateFileListWindowsProperty();//#725
			break;
		}
		*/
	  //win case AAppPrefDB::kIndexWindowFont:
		//#844 case AAppPrefDB::kIndexWindowFontName://win
		//#844 case AAppPrefDB::kIndexWindowFontSize:
		/*#844
	  case AAppPrefDB::kIndexWinNormalColor:
	  case AAppPrefDB::kIndexWinBackColor:
	  case AAppPrefDB::kIndexWinCommentColor:
	  case AAppPrefDB::kIndexWinURLColor:
	  case AAppPrefDB::kIndexWinGroupColor:
		{
			GetApp().SPI_UpdateIndexWindowProperty();
			break;
		}
		*/
		/*#844
	  case AAppPrefDB::kMultipleRowTab:
		{
			GetApp().SPI_UpdateTextWindowViewBoundsAll(kIndex_Invalid);
			break;
		}
		*/
	  case kTable_RecentlyOpendFileComment:
		{
			GetApp().SPI_UpdateRecentlyOpenedFile(false);
			break;
		}
		/*#844
	  case AAppPrefDB::kDisplayLeftsideConcatViewButton:
		{
			GetApp().SPI_UpdateTextWindowViewBoundsAll(kIndex_Invalid);
			break;
		}
		*/
		/*#844
		//B0333
	  case AAppPrefDB::kAlphaWindowMode:
	  case AAppPrefDB::kAlphaWindowPercent1:
	  case AAppPrefDB::kAlphaWindowPercent2:
		{
			GetApp().SPI_UpdateWindowAlphaAll();
			break;
		}
		*/
		/*#844
		//B0389 OK
	  case AAppPrefDB::kFilePathDisplayType:
		{
			GetApp().SPI_UpdateFilePathDisplayAll();
			break;
		}
		*/
		//R0006
	  case AAppPrefDB::kUseSCM:
		//#844 case AAppPrefDB::kDisplaySCMStateInFileList:
		//#844 case AAppPrefDB::kDisplaySCMStateInFileList_Color:
		//#844 case AAppPrefDB::kDisplaySCMStateInFileList_NotEntriedColor:
		//#844 case AAppPrefDB::kDisplaySCMStateInFileList_OthersColor:
		{
			GetApp().SPI_UpdateSCMDisplayAll();
			break;
		}
	  case AAppPrefDB::kDiffColor_Changed:
	  case AAppPrefDB::kDiffColor_Added:
	  case AAppPrefDB::kDiffColor_Deleted:
		//#844 case AAppPrefDB::kDiffColor_JumpList:
		{
			GetApp().NVI_RefreshAllWindow();
			//�W�����v���X�g�̕ύX���ڂɑΉ�����ɂ̓h�L�������g���ێ����Ă���F���X�g��ύX����K�v������B
			break;
		}
		/*#844
	  case AAppPrefDB::kIdInfoWindowAlphaPercent:
		{
			GetApp().SPI_GetIdInfoWindow().NVI_UpdateProperty();
			GetApp().SPI_GetKeyRecordWindow().NVI_UpdateProperty();//#677
			break;
		}
		*/
		//win 080702
	  case AAppPrefDB::kFileListDontDisplayInvisibleFile:
		{
			GetApp().SPI_UpdateAllSameFolderData();
			GetApp().SPI_UpdateAllSameProjectData();
			break;
		}
		/*#844
		//#137
	  case AAppPrefDB::kDisplaySearchBox:
		{
			GetApp().SPI_UpdateTextWindowViewBoundsAll(kIndex_Invalid);
			break;
		}
		//amazon
	  case AAppPrefDB::kDisplayAmazonButton:
		{
			GetApp().SPI_RemakeToolButtonsAll(kIndex_Invalid);
			break;
		}
		//#450
	  case AAppPrefDB::kLineFolding:
		{
			GetApp().SPI_UpdateLineImageInfo(kIndex_Invalid);
			break;
		}
		//#164
	  case AAppPrefDB::kShowTabCloseButton:
		{
			GetApp().SPI_UpdateWindowMenu();
			break;
		}
		*/
		/*#drop diff info�̕\���X�V�����ʓ|�Ȃ̂Ŋ��ɊJ���Ă���h�L�������g�ɂ͓K�p���Ȃ����Ƃɂ���B
		//#379
	  case AAppPrefDB::kShowDiffTargetWithWordWrap:
		{
			GetApp().SPI_UpdateDiffTargetDocumentWrap();
			break;
		}
		*/
		//#634
	  case AAppPrefDB::kInfoPaneSideBarMode:
	  case AAppPrefDB::kInfoPaneSideBarModeColor:
		{
			GetApp().SPI_UpdateTextWindowViewBoundsAll();
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		//#81
	  case AAppPrefDB::kEnableAutoBackup:
	  case AAppPrefDB::kAutoBackupRootFolder:
		{
			GetApp().SPI_DoAutoBackupFolderChangedAll();
			break;
		}
		//skin�X�V
	  case AAppPrefDB::kSkinName:
		{
			GetApp().SPI_LoadImageFiles(true);
			GetApp().NVI_UpdatePropertyAllWindow();
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		//�h�b�g�P�ʃX�N���[�� #891
	  case AAppPrefDB::kUseWheelScrollFloatValue:
		{
			CAppImp::SetUseWheelScrollFloatValue(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kUseWheelScrollFloatValue));
			break;
		}
		//�����[�g�A�J�E���g #193
	  case kTable_FTPAccount:
		{
			GetApp().SPI_UpdateFileListWindows(kFileListUpdateType_RemoteFileAccountDataChanged,kObjectID_Invalid);
			break;
		}
		//�����X�V���x�� #1102
	  case AAppPrefDB::kSparkleUpdateLevel:
		{
			GetApp().GetAppPref().SPI_UpdateSparkleUpdateLevel();
			break;
		}
		//�E�C���h�E�^�C�g���t���p�X�\�� #1182
	  case AAppPrefDB::kDisplayFullPathTitleBar:
		{
			GetApp().NVI_UpdateTitleBar();
			break;
		}
		//���E�]�� #1186
	  case AAppPrefDB::kDisplayLeftRightMargin:
	  case AAppPrefDB::kTextWidthForLeftRightMargin:
		{
			GetApp().SPI_UpdateTextWindowViewBoundsAll();
			GetApp().SPI_RecalcWordWrapAll(kIndex_Invalid);
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		//�������E�C���h�E#1255
	  case AAppPrefDB::kAlphaWindowModeV3:
	  case AAppPrefDB::kAlphaWindowPercent1V3:
	  case AAppPrefDB::kAlphaWindowPercent2V3:
		{
			GetApp().SPI_UpdateWindowAlphaAll();
			break;
		}
		//�^�u�ɐe�t�H���_���\�� #1334
	  case AAppPrefDB::kTabShowParentFolder:
		{
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		//�^�u�� #1349
	  case AAppPrefDB::kTabWidth:
		{
			GetApp().SPI_UpdateTextWindowViewBoundsAll();
			break;
		}
	  default:
		{
			//��������
			break;
		}
	}
}

//#1239
/**
�t�@�C���ɕۑ�
*/
void	AWindow_AppPref::NVMDO_NotifyDataChangedForSave()
{
	GetApp().NVI_GetAppPrefDB().SaveToFile();
}


