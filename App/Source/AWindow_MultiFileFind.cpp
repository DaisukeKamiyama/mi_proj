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

AWindow_MultiFileFind

*/

#include "stdafx.h"

#include "AWindow_MultiFileFind.h"
#include "AApp.h"
//#include "AUtil.h"
#include "ATextFinder.h"
#include "ADocument_IndexWindow.h"
#include "AView_Index.h"

#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kTextSize = 10.0;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const AFloatNumber	kTextSize = 9.0;
#endif

//�R���g���[��ID
const static AControlID		kPopup_RecentlyUsedFindText = 101;
const static AControlID		kText_FindText = 102;
const static AControlID		kPopup_RecentlyUsedFileFilter = 103;
const static AControlID		kText_FileFilter = 104;
const static AControlID		kButton_Find = 105;
//const static AControlID		kStaticText_ActiveFolder = 107;
const static AControlID		kPopup_RecentlyUsedPath = 109;
const static AControlID		kButton_SelectFolder = 110;
const static AControlID		kCheckBox_IgnoreCase = 112;
const static AControlID		kCheckBox_WholeWord = 113;
const static AControlID		kCheckBox_Aimai = 114;
const static AControlID		kCheckBox_BackslashYen = 115;
const static AControlID		kCheckBox_RegExp = 116;
//#688 const static AControlID		kCheckBox_AllowReturnTab = 117;
const static AControlID		kCheckBox_Recursive = 119;
const static AControlID		kCheckBox_DisplayPosition = 120;
const static AControlID		kCheckBox_OnlyVisibleFile = 121;
const static AControlID		kCheckBox_OnlyTextFile = 122;
const static AControlID		kCheckBox_IgnoreSpaces			= 123;//#165
const static AControlID		kButton_SetupFuzzySearch		= 124;//#166
const static AControlID		kButton_ExtractToNewDocument	= 125;
//const static AControlID		kPopup_ChooseKind = 200;
//const static AControlID		kCheckBox_UseFileFilter = 201;
//const static AControlID		kPopup_FilterKind = 202;
//
const static AControlID		kRadio_Folder_SameFolder = 301;
const static AControlID		kRadio_Folder_ParentFolder = 302;
const static AControlID		kRadio_Folder_SameProject = 303;
const static AControlID		kRadio_Folder_Select = 304;
const static AControlID		kText_Folder_Path = 305;
//
const static AControlID		kRadio_Target_AllFiles = 311;
const static AControlID		kRadio_Target_WildCard = 312;
const static AControlID		kRadio_Target_RegExp = 313;
const static AControlID		kPopup_FileFilterPreset = 314;//#617
//
const static AControlID		kButton_OpenCloseReplaceScreen = 401;
//
const AControlID		kTriangle_FindOptions			= 501;
const AControlID		kTriangleText_FindOptions		= 502;
const AControlID		kStaticText_BottomMarker		= 503;
const AControlID		kTriangle_AdvancedSearch		= 504;
const AControlID		kTriangleText_AdvancedSearch	= 505;

//
const static AControlID		kControlID_IndexView = 1002;
const static AControlID		kControlID_ReplaceTargetListView = 1003;
const static AControlID		kControlID_ReplaceExclusionListView = 1004;
const static AControlID		kButton_ExcludeFile = 1005;
const static AControlID		kButton_IncludeFile = 1006;
const static AControlID		kCheck_BackupFolder = 1007;
const static AControlID		kText_BackupFolder = 1008;
const static AControlID		kButton_CancelReplace = 1009;
const static AControlID		kButton_ExecuteReplace = 1010;
const static AControlID		kText_ReplaceText = 1011;
const static AControlID		kButton_ReplaceBasePath = 1013;
const static AControlID		kButton_ExecuteReplaceOK = 1999;
const static AControlID		kProgressText = 1014;
const static AControlID		kButton_FindAbort = 1015;
const static AControlID		kButton_SelectBackupFolder = 1017;
const static AControlID		kButton_DefaultBackupFolder = 1018;

//
const static AControlID		kVScroll_ReplaceTargetListView = 2001;
const static AControlID		kVScroll_ReplaceExclusionListView = 2001;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_MultiFileFind
#pragma mark ===========================
//�}���`�t�@�C�������E�C���h�E

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_MultiFileFind::AWindow_MultiFileFind():AWindow()
,mFindResultDocumentID(kObjectID_Invalid)
, mReplaceWindowMode(false), mExecutingFindForReplace(false), mFindForReplaceFinished(false), mFindForReplaceAborted(false)
,mOpenClosingReplaceScreen(false), mExecutingReplace(false)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kMultiFileFindWindowPosition);
	
	AText	path;
	path.SetCstring("/");
	mActiveFolder.Specify(path);
	//#175
	NVI_AddToRotateArray();
	//
	NVI_AddToTimerActionWindowArray();
}

//�f�X�g���N�^
AWindow_MultiFileFind::~AWindow_MultiFileFind()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
���j���[�I��������
*/
ABool	AWindow_MultiFileFind::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			//AWindow�ŃN���[�Y�\�t���O�`�F�b�N��ANVIDO_Close()���R�[�������
			NVI_TryClose();
			result = true;
			break;
		}
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

/**
�E�C���h�E�̕���{�^��
*/
void	AWindow_MultiFileFind::EVTDO_DoCloseButton()
{
	//AWindow�ŃN���[�Y�\�t���O�`�F�b�N��ANVIDO_Close()���R�[�������
	NVI_TryClose();//#92
}

//
ABool	AWindow_MultiFileFind::EVTDO_ValueChanged( const AControlID inControlID )
{
	ABool	result = true;
	switch(inControlID)
	{
		/*
	  case kPopup_ChooseKind:
		{
			NVI_SetControlBool(kRadio_ActiveFolder,true);
			NVM_UpdateControlStatus();
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

//�R���g���[���N���b�N������
ABool	AWindow_MultiFileFind::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	
	switch(inID)
	{
		//�������s
	  case kButton_Find:
		{
			Execute(false);
			result = true;
			break;
		}
		//�������s
	  case kButton_ExtractToNewDocument:
		{
			Execute(true);
			result = true;
			break;
		}
		//�ŋߎg���������e�L�X�g
	  case kPopup_RecentlyUsedFindText:
		{
			AIndex	index = NVI_GetControlNumber(kPopup_RecentlyUsedFindText);
			if( index-1 >= GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFindString) )
			{
				//���j���[������
				GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFindString);
				UpdateRecentlyUsedFindTextMenu();
			}
			else if( index > 0 )//B0000 �u�ŋߌ�������������v�͌���������ɐݒ肵�Ȃ�
			{
				//����������ɃR�s�[
				AText	text;
				NVI_GetControlText(kPopup_RecentlyUsedFindText,text);
				NVI_SetControlText(kText_FindText,text);
			}
			NVI_SetControlNumber(kPopup_RecentlyUsedFindText,0);
			break;
		}
		//�ŋߎg�����t�@�C���t�B���^
	  case kPopup_RecentlyUsedFileFilter:
		{
			AIndex	index = NVI_GetControlNumber(kPopup_RecentlyUsedFileFilter);
			if( index-1 >= GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter) )
			{
				//���j���[������
				GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter);
				UpdateRecentlyUsedFileFilterMenu();
			}
			else if( index > 0 )//B0000 �u�ŋߌ�������������v�͌���������ɐݒ肵�Ȃ�
			{
				//�t�@�C���t�B���^���ɃR�s�[
				AText	text;
				NVI_GetControlText(kPopup_RecentlyUsedFileFilter,text);
				NVI_SetControlText(kText_FileFilter,text);
			}
			NVI_SetControlNumber(kPopup_RecentlyUsedFileFilter,0);
			break;
		}
		//�ŋߎg�����t�@�C���p�X
	  case kPopup_RecentlyUsedPath:
		{
			AIndex	index = NVI_GetControlNumber(kPopup_RecentlyUsedPath);
			if( index-1 >= GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder) )
			{
				//���j���[������
				GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder);
				UpdateRecentlyUsedPathMenu();
			}
			else if( index > 0 )//B0000 �u�ŋߌ�������������v�͌���������ɐݒ肵�Ȃ�
			{
				//�t�@�C���p�X���ɃR�s�[
				AText	text;
				NVI_GetControlText(kPopup_RecentlyUsedPath,text);
				NVI_SetControlText(kText_Folder_Path,text);
				//
				NVI_SetControlBool(kRadio_Folder_Select,true);
			}
			NVI_SetControlNumber(kPopup_RecentlyUsedPath,0);
			break;
		}
		//�t�H���_�I��
	  case kButton_SelectFolder:
		{
			//#551 ���݃e�L�X�g�{�b�N�X�Ɏw�肳��Ă���t�H���_���f�t�H���g�t�H���_�ɂ���
			AText	path;
			NVI_GetControlText(kText_Folder_Path,path);
			AFileAcc	defaultfolder;
			defaultfolder.Specify(path);
			//�t�H���_�I���E�C���h�E��\��
			AText	message;
			message.SetLocalizedText("ChooseFolderMessage_MultiFileFindSelectFolder");
			NVI_ShowChooseFolderWindow(defaultfolder,message,kButton_SelectFolder,true);//#551
			result = true;
			break;
		}
		//�o�b�N�A�b�v�t�H���_�I�� #65
	  case kButton_SelectBackupFolder:
		{
			AText	path;
			NVI_GetControlText(kText_BackupFolder,path);
			AFileAcc	defaultfolder;
			defaultfolder.Specify(path);
			//�t�H���_�I���E�C���h�E�\��
			AText	message;
			message.SetLocalizedText("ChooseFolderMessage_MultiFileFindSelectBackupFolder");
			NVI_ShowChooseFolderWindow(defaultfolder,message,kButton_SelectBackupFolder,true);
			result = true;
			break;
		}
		//�f�t�H���g�̃o�b�N�A�b�v�t�H���_��ݒ�
	  case kButton_DefaultBackupFolder:
		{
			AFileAcc	backupFolder;
			GetApp().SPI_GetDefaultMultiFileReplaceBackupFolder(backupFolder);
			AText	path;
			backupFolder.GetPath(path);
			NVI_SetControlText(kText_BackupFolder,path);
			break;
		}
		//#166
	  case kButton_SetupFuzzySearch:
		{
			GetApp().SPI_GetAppPrefWindow().NVI_CreateAndShow();
			GetApp().SPI_GetAppPrefWindow().NVI_SelectTabControl(3);
			GetApp().SPI_GetAppPrefWindow().NVI_RefreshWindow();
			GetApp().SPI_GetAppPrefWindow().NVI_SwitchFocusToFirst();
			break;
		}
		//#617 �v���Z�b�g����t�@�C���t�B���^�ݒ�
	  case kPopup_FileFilterPreset:
		{
			AText	text;
			GetApp().SPI_GetFileFilterPreset(NVI_GetControlNumber(kPopup_FileFilterPreset),text);
			NVI_SetControlText(kText_FileFilter,text);
			NVI_SetControlBool(kRadio_Target_WildCard,true);
			NVI_SetControlNumber(kPopup_FileFilterPreset,0);
			break;
		}
		//�����I�v�V�����܂肽����
	  case kTriangle_FindOptions:
		{
			if( mReplaceWindowMode == false )
			{
				NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			}
			break;
		}
	  case kTriangleText_FindOptions:
		{
			if( mReplaceWindowMode == false )
			{
				NVI_SetControlBool(kTriangle_FindOptions,!(NVI_GetControlBool(kTriangle_FindOptions)));
				NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			}
			break;
		}
		//
	  case kTriangle_AdvancedSearch:
		{
			if( mReplaceWindowMode == false )
			{
				NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			}
			break;
		}
	  case kTriangleText_AdvancedSearch:
		{
			if( mReplaceWindowMode == false )
			{
				NVI_SetControlBool(kTriangle_AdvancedSearch,!(NVI_GetControlBool(kTriangle_AdvancedSearch)));
				NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			}
			break;
		}
		//�}���`�t�@�C���u�����open/close
	  case kButton_OpenCloseReplaceScreen:
		{
			SPI_OpenCloseReplaceScreen(!mReplaceWindowMode);
			result = true;
			break;
		}
		//�u���L�����Z��
	  case kButton_CancelReplace:
		{
			MultiFileReplace_ClearReplaceData();
			break;
		}
		//�u�����O�t�@�C���֒ǉ�
	  case kButton_ExcludeFile:
		{
			MultiFileReplace_ExcludeButton();
			break;
		}
		//�u���Ώۃt�@�C���֒ǉ�
	  case kButton_IncludeFile:
		{
			MultiFileReplace_IncludeButton();
			break;
		}
		//#65
		//�u�������s�{�^��
	  case kButton_ExecuteReplace:
		{
			AText	mes1, mes2, mes3;
			mes1.SetLocalizedText("MultiFileReplaceConfirmation1");
			mes2.SetLocalizedText("MultiFileReplaceConfirmation2");
			mes3.SetLocalizedText("MultiFileReplaceConfirmation3");
			mes2.ReplaceParamText('0',mFindParamerForReplace.folderPath);
			NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,kButton_ExecuteReplaceOK);
			break;
		}
		//�u�����sOK�{�^��
	  case kButton_ExecuteReplaceOK:
		{
			MultiFileReplace_ExecuteReplace();
			break;
		}
		//�������f�{�^��
	  case kButton_FindAbort:
		{
			//�}���`�t�@�C���������f
			GetApp().SPI_AbortMultiFileFind();
			//�v���O���X�e�L�X�g���f�e�L�X�g�ݒ�
			AText	text;
			text.SetLocalizedText("Progress_Aborting");
			NVI_SetControlText(kProgressText,text);
			mFindForReplaceAborted = true;
			//
			NVM_UpdateControlStatus();
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return result;
}

/**
�u���Ώۃt�@�C���_�u���N���b�N������
*/
ABool	AWindow_MultiFileFind::EVTDO_DoubleClicked( const AControlID inID )
{
	switch(inID)
	{
	  case kControlID_ReplaceTargetListView:
		{
			AIndex	index = NVI_GetListView(kControlID_ReplaceTargetListView).SPI_GetSelectedDBIndex();
			AText	path;
			mReplaceTargetFilePathArray.Get(index,path);
			AFileAcc	file;
			file.Specify(path);
			GetApp().GetAppPref().LaunchAppWithFile(file,0);
			break;
		}
	  case kControlID_ReplaceExclusionListView:
		{
			AIndex	index = NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_GetSelectedDBIndex();
			AText	path;
			mReplaceExclusionFilePathArray.Get(index,path);
			AFileAcc	file;
			file.Specify(path);
			GetApp().GetAppPref().LaunchAppWithFile(file,0);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return true;
}

//�������s
void	AWindow_MultiFileFind::Execute( const ABool inExtractMatchedText )
{
	if( GetApp().SPI_IsMultiFileFindWorking() == true )   return;
	
	//�����p�����[�^�擾
	AFindParameter	findParam;
	if( GetFindParam(findParam) == false )
	{
		return;
	}
	
	//���K�\���`�F�b�N
	if( CheckRegExp(findParam) == false )   return;//B0317
	
	//�ŋߎg���������e�L�X�g���̐ݒ�
	SPI_AddRecentlyUsedFindText(findParam.findText);
	AText	filterText;
	NVI_GetControlText(kText_FileFilter,filterText);
	SPI_AddRecentlyUsedFileFilter(filterText);//#437
	//�����ɒǉ�
	AFileAcc	folder;
	folder.Specify(findParam.folderPath);
	SPI_AddRecentlyUsedPath(folder);
	
	//�������s
	ADocumentID	indexDocumentID = kObjectID_Invalid;
	if( mReplaceWindowMode == true )
	{
		//------------------�u���p����------------------
		//�u���f�[�^�S�폜
		MultiFileReplace_ClearReplaceData();
		//�u���p�����p�����[�^���L��
		mFindParamerForReplace.Set(findParam);
		//Step 2�̌����t�H���_�\��
		NVI_SetControlText(kButton_ReplaceBasePath,mFindParamerForReplace.folderPath);
		//�������ʕ\����document�̓}���`�t�@�C���u���E�C���h�E���̌��ʕ\���p�h�L�������g
		indexDocumentID = mFindResultDocumentID;
		//��Ԑݒ�
		mExecutingFindForReplace = true;//�u���p�����Ftrue
		mFindForReplaceFinished = false;//�u���p�����I���Ffalse
		mFindForReplaceAborted = false;//�u���p�������~�Ffalse
		//�\���X�V
		NVM_UpdateControlStatus();
	}
	else
	{
		//------------------����------------------
		//�������ʃE�C���h�E�擾�i�܂��͐����j
		indexDocumentID = GetApp().SPI_GetOrCreateFindResultWindowDocument();
	}
	//�}���`�t�@�C���������s
	GetApp().SPI_StartMultiFileFind(findParam,indexDocumentID,mReplaceWindowMode,inExtractMatchedText);
}

/**
�����p�����[�^�擾
*/
ABool	AWindow_MultiFileFind::GetFindParam( AFindParameter& findParam ) const
{
	ABool	result = true;
	AFileAcc	folder;
	
	//����������擾
	NVI_GetControlText(kText_FindText,findParam.findText);
	if( findParam.findText.GetLength() == 0 )   result = false;
	
	//#437
	//�t�B���^������擾
	AText	filterText;
	NVI_GetControlText(kText_FileFilter,filterText);
	if( NVI_GetControlBool(kRadio_Target_WildCard) == true )//wildcard filter
	{
		ARegExp::ConvertToREFromWildCard(filterText,findParam.filterText);
	}
	else
	{
		findParam.filterText.SetText(filterText);
	}
	
	//�t�B���^��܂��͑S�t�@�C���Ώۃ��W�I�{�^��ON�Ȃ�.+��ݒ�
	if( findParam.filterText.GetLength() == 0 || NVI_GetControlBool(kRadio_Target_AllFiles) == true )
	{
		findParam.filterText.SetCstring(".+");
	}
	
	//�Ώۃt�H���_�擾
	//findParam.modeIndex = kIndex_Invalid;
	if( NVI_GetControlBool(kRadio_Folder_Select) == false )
	{
		AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
		if( docID == kObjectID_Invalid )   result = false;
		folder = mActiveFolder;
	}
	else
	{
		AText	pathtext;
		NVI_GetControlText(kText_Folder_Path,pathtext);
		folder.SpecifyWithAnyFilePathType(pathtext);//B0389
	}
	if( folder.Exist() == false )   result = false;
	folder.GetPath(findParam.folderPath);
	
	//�����I�v�V�����擾
	NVI_GetControlBool(kCheckBox_IgnoreCase,findParam.ignoreCase);
	NVI_GetControlBool(kCheckBox_WholeWord,findParam.wholeWord);
	NVI_GetControlBool(kCheckBox_Aimai,findParam.fuzzy);
	findParam.ignoreBackslashYen = !(NVI_GetControlBool(kCheckBox_BackslashYen));
	NVI_GetControlBool(kCheckBox_RegExp,findParam.regExp);
	NVI_GetControlBool(kCheckBox_Recursive,findParam.recursive);
	NVI_GetControlBool(kCheckBox_DisplayPosition,findParam.displayPosition);
	NVI_GetControlBool(kCheckBox_IgnoreSpaces,findParam.ignoreSpaces);//#165
	NVI_GetControlBool(kCheckBox_OnlyVisibleFile,findParam.onlyVisibleFile);
	NVI_GetControlBool(kCheckBox_OnlyTextFile,findParam.onlyTextFile);
	
	return result;
}

void	AWindow_MultiFileFind::EVTDO_TextInputted( const AControlID inID )
{
	switch(inID)
	{
	  case kText_Folder_Path:
		{
			NVI_SetControlBool(kRadio_Folder_Select,true);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	NVM_UpdateControlStatus();
}

void	AWindow_MultiFileFind::EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder )
{
	AText	path;
	inFolder.GetPath(path);
	switch(inControlID)
	{
	  case kButton_SelectFolder:
		{
			NVI_SetControlText(kText_Folder_Path,path);
			NVI_SetControlBool(kRadio_Folder_Select,true);
			//#1443 test inFolder.CreateSecurityScopedBookmark();
			break;
		}
	  case kButton_SelectBackupFolder:
		{
			NVI_SetControlText(kText_BackupFolder,path);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

//B0317
ABool	AWindow_MultiFileFind::CheckRegExp( const AFindParameter& inFindParam )
{
	if( inFindParam.regExp == true )
	{
		ARegExp	regexpcheck;
		if( regexpcheck.SetRE(inFindParam.findText) == false )
		{
			AText	message1, message2;
			message1.SetLocalizedText("RegExpError1");
			message2.SetLocalizedText("RegExpError2");
			message2.ReplaceParamText('0',inFindParam.findText);
			NVI_SelectWindow();
			NVI_ShowChildWindow_OK(message1,message2);
			return false;
		}
	}
	return true;
}

//R0000
void	AWindow_MultiFileFind::EVTDO_DoFileDropped( const AControlID inControlID, const AFileAcc& inFile )
{
	AFileAcc	folder;
	folder.CopyFrom(inFile);
	if( folder.IsFolder() == false)
	{
		folder.SpecifyParent(inFile);
	}
	AText	path;
	folder.GetPath(path);
	NVI_SetControlText(kText_Folder_Path,path);
	NVI_SetControlBool(kRadio_Folder_Select,true);
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void AWindow_MultiFileFind::NVIDO_Create( const ADocumentID inDocumentID )
{
	if( mReplaceWindowMode == true )
	{
		NVM_CreateWindow("main/MultiFileReplace");
	}
	else
	{
		NVM_CreateWindow("main/MultiFileFind");
	}
	NVI_CreateEditBoxView(kText_FindText);
	NVI_SetControlBindings(kText_FindText,true,true,true,false,false,true);
	NVI_GetEditBoxView(kText_FindText).SPI_SetCenterizeWhen1Line();
	NVI_RegisterToFocusGroup(kPopup_FileFilterPreset,true);//#617
	//NVI_CreateEditBoxView(kText_FileFilter);
	NVI_RegisterToFocusGroup(kPopup_RecentlyUsedFileFilter,true);//#353
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_Folder_SameFolder);
	NVI_AddToLastRegisteredRadioGroup(kRadio_Folder_ParentFolder);
	NVI_AddToLastRegisteredRadioGroup(kRadio_Folder_SameProject);
	NVI_AddToLastRegisteredRadioGroup(kRadio_Folder_Select);
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_Target_AllFiles);
	NVI_AddToLastRegisteredRadioGroup(kRadio_Target_WildCard);
	NVI_AddToLastRegisteredRadioGroup(kRadio_Target_RegExp);
	NVI_RegisterToFocusGroup(kButton_Find,true);//#353
	NVI_RegisterToFocusGroup(kPopup_RecentlyUsedPath,true);//#353
	NVI_RegisterToFocusGroup(kButton_SelectFolder,true);//#353
	//NVI_CreateEditBoxView(kText_Folder_Path);
	NVI_RegisterToFocusGroup(kCheckBox_IgnoreCase,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_WholeWord,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_IgnoreSpaces,true);//#165
	NVI_RegisterToFocusGroup(kCheckBox_BackslashYen,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_RegExp,true);//#353
	//#688 NVI_RegisterToFocusGroup(kCheckBox_AllowReturnTab,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_Recursive,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_DisplayPosition,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_OnlyVisibleFile,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_OnlyTextFile,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_Aimai,true);//#353
	NVI_RegisterToFocusGroup(kPopup_RecentlyUsedFindText,true);//#353
	
	
	AText	text;
	//win text.SetCstring(".+");
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kFindDefaultFileFilter,text);//win
	
	//�t�@�C������v��������
	//B0414
	if( GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter).GetItemCount() > 0 )
	{
		GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter).Get(0,text);
	}
	NVI_SetControlText(kText_FileFilter,text);
	
	//�ŋߌ��������t�H���_����
	if(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder).GetItemCount() > 0 )
	{
		GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder).Get(0,text);
	}
	
	//�p�X�e�L�X�g�ݒ�
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kMultiFileFind_FolderPathText,text);
	if( text.GetItemCount() == 0 )
	{
		//�ݒ�f�[�^���󔒃e�L�X�g�̏ꍇ�iver3�ł̍ŏ��̋N�����A�܂��́A�폜�����ꍇ�j�A�����̍ŏ��̃e�L�X�g��ݒ�
		if( GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder).GetItemCount() > 0 )
		{
			GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder).Get(0,text);
		}
	}
	NVI_SetControlText(kText_Folder_Path,text);
	
	//�t�B���^�e�L�X�g�ݒ�
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kMultiFileFind_FileFilterText,text);
	if( text.GetItemCount() == 0 )
	{
		//�ݒ�f�[�^���󔒃e�L�X�g�̏ꍇ�iver3�ł̍ŏ��̋N�����A�܂��́A�폜�����ꍇ�j�A�����̍ŏ��̃e�L�X�g��ݒ�
		if( GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter).GetItemCount() > 0 )
		{
			GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter).Get(0,text);
		}
	}
	if( text.GetItemCount() == 0 )
	{
		//��L�ł���ɋ󔒃e�L�X�g�̏ꍇ�A���C���h�J�[�h*.*��ݒ�
		text.SetCstring("*.*");
		GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kMultiFileFindFileFilterKind,1);
	}
	NVI_SetControlText(kText_FileFilter,text);
	
	//�t�H���_���W�I�{�^���X�V
	UpdateFolderRadioButtons();
	
	//�����I�v�V�����f�t�H���g�ݒ�
	NVI_SetControlBool(kCheckBox_IgnoreCase,		GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultIgnoreCase));
	NVI_SetControlBool(kCheckBox_WholeWord,		GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultWholeWord));
	NVI_SetControlBool(kCheckBox_Aimai,			GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultAimai));
	NVI_SetControlBool(kCheckBox_BackslashYen,	(GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultIgnoreBackslashYen)==false));
	NVI_SetControlBool(kCheckBox_RegExp,			GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultRegExp));
	//#688 NVI_SetControlBool(kCheckBox_AllowReturnTab,	GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultAllowReturn));
	NVI_SetControlBool(kCheckBox_Recursive,		GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultRecursive));
	NVI_SetControlBool(kCheckBox_DisplayPosition,	GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultDisplayPosition));
	NVI_SetControlBool(kCheckBox_IgnoreSpaces,		GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultIgnoreSpaces));//#165
	//B0313
	NVI_SetControlBool(kCheckBox_OnlyVisibleFile,	GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultOnlyVisibleFile));
	NVI_SetControlBool(kCheckBox_OnlyTextFile,	GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultOnlyTextFile));
	
	NVI_SetDefaultOKButton(kButton_Find);
	
	UpdateRecentlyUsedFindTextMenu();
	UpdateRecentlyUsedFileFilterMenu();
	UpdateRecentlyUsedPathMenu();
	
	//�Ώۃ��W�I�{�^���ݒ�
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kMultiFileFind_UseFileFilter) == false )
	{
		NVI_SetControlBool(kRadio_Target_AllFiles,true);
	}
	else if( GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kMultiFileFindFileFilterKind) == 0 )
	{
		NVI_SetControlBool(kRadio_Target_RegExp,true);
	}
	else if( GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kMultiFileFindFileFilterKind) == 1 )
	{
		NVI_SetControlBool(kRadio_Target_WildCard,true);
	}
	
	//#617
	NVI_RegisterTextArrayMenu(kPopup_FileFilterPreset,kTextArrayMenuID_FileFilterPreset);
	NVI_SetControlNumber(kPopup_FileFilterPreset,0);
	
	//DocImp���� #1034
	AObjectID	docImpObjectID = GetApp().SPI_CreateDocImp(kDocumentType_IndexWindow);
	//�������ʗpDocument����
	AIndexWindowDocumentFactory	docfactory(this,docImpObjectID);//#1034
	mFindResultDocumentID = GetApp().NVI_CreateDocument(docfactory);
	
	if( mReplaceWindowMode == true )
	{
		//==================�}���`�t�@�C���u���p���==================
		
		const ANumber	kIndexViewFileColumnWidth = 250;
		
		//�E�C���h�E�ŏ��T�C�Y�ݒ�
		ARect	bounds = {0};
		NVI_GetWindowBounds(bounds);
		NVI_SetWindowMinimumSize(bounds.right-bounds.left,bounds.bottom-bounds.top);
		NVI_SetWindowMaximumSize(bounds.right-bounds.left,bounds.bottom-bounds.top);
		
		//�u���e�L�X�g
		NVI_CreateEditBoxView(kText_ReplaceText);
		NVI_SetControlBindings(kText_ReplaceText,true,true,true,false,false,true);
		NVI_GetEditBoxView(kText_ReplaceText).SPI_SetCenterizeWhen1Line();
		
		//��������View����
		AIndexViewFactory	indexViewFactory(GetObjectID(),kControlID_IndexView,mFindResultDocumentID);
		AViewID	indexViewID = NVM_CreateView(kControlID_IndexView,false,indexViewFactory);
		AView_Index::GetIndexViewByViewID(indexViewID).NVI_CreateFrameView();
		AView_Index::GetIndexViewByViewID(indexViewID).NVI_CreateVScrollBar();
		AView_Index::GetIndexViewByViewID(indexViewID).SPI_SetFileColumnWidth(kIndexViewFileColumnWidth);
		
		//Replace Target File ListView����
		AListViewFactory	targetListViewFactory(GetObjectID(),kControlID_ReplaceTargetListView);
		NVM_CreateView(kControlID_ReplaceTargetListView,false,targetListViewFactory);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_CreateFrame(true,true,false);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetTextDrawProperty(GetEmptyText(),10.0,kColor_Black);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetEllipsisFirstCharacters(true);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_RegisterColumn_Text(0,200,"MultiFileReplace_FileListTitle_File",false);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_RegisterColumn_Text(1,0,"MultiFileReplace_FileListTitle_TextEncoding",false);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetMultiSelectEnabled(true);
		
		//Replace Exclusion File ListView����
		AListViewFactory	exclusionListViewFactory(GetObjectID(),kControlID_ReplaceExclusionListView);
		NVM_CreateView(kControlID_ReplaceExclusionListView,false,exclusionListViewFactory);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_CreateFrame(true,true,false);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetTextDrawProperty(GetEmptyText(),10.0,kColor_Black);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetEllipsisFirstCharacters(true);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_RegisterColumn_Text(0,200,"MultiFileReplace_FileListTitle_File",false);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_RegisterColumn_Text(1,0,"MultiFileReplace_FileListTitle_TextEncoding",false);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetMultiSelectEnabled(true);
		
		//�o�b�N�A�b�v�t�H���_�p�X�e�L�X�g�{�b�N�X
		AText	backupFolderPath;
		GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kMultiFileFind_BackupFolder,backupFolderPath);
		if( backupFolderPath.GetItemCount() == 0 )
		{
			//�o�b�N�A�b�v�t�H���_�̐ݒ肪��̏ꍇ�̓f�t�H���g��ݒ�
			AFileAcc	backupRootFolder;
			GetApp().SPI_GetDefaultMultiFileReplaceBackupFolder(backupRootFolder);
			backupRootFolder.GetPath(backupFolderPath);
		}
		NVI_SetControlText(kText_BackupFolder,backupFolderPath);
	}
	else
	{
		//==================�}���`�t�@�C�������p���==================
		
		const ANumber kWindowMinimumWidth = 280;
		const ANumber kWindowMaximumWidth = 100000;
		
		//�E�C���h�E�ŏ��T�C�Y�ݒ�
		ARect	bounds = {0};
		NVI_GetWindowBounds(bounds);
		NVI_SetWindowMinimumSize(kWindowMinimumWidth,bounds.bottom-bounds.top);
		NVI_SetWindowMaximumSize(kWindowMaximumWidth,bounds.bottom-bounds.top);
		
		//�E�C���h�E���ݒ�
		NVI_SetWindowWidth(GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kMultiFileFindWindow_WindowWidth));
		
		//#872
		//�����I�v�V�����܂肽���ݏ�Ԃɂ��E�C���h�E�T�C�Y�ݒ�
		NVI_SetControlBool(kTriangle_FindOptions,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kMultiFileFindWindow_FindOptionsExpanded));
		NVI_SetControlBool(kTriangle_AdvancedSearch,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kMultiFileFindWindow_AdvancedSearchExpanded));
		NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
		
		//�v���O���X������
		SPI_MultiFileReplace_SetProgressText(GetEmptyText());
		SPI_MultiFileReplace_Progress(0);
	}
	
	//#1373
	NVI_RegisterHelpAnchor(90001,"find.htm#option");
}

void	AWindow_MultiFileFind::NVIDO_UpdateProperty()
{
	//win
	AText	fontname;
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kFindDialogFontName,fontname);
	//
	//�t�H���g�ݒ�
	NVI_SetControlTextFont(kText_FindText,
			//win GetApp().GetAppPref().GetData_Font(AAppPrefDB::kFindDialogFont),
			fontname,
			GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kFind_FontSize));
	//R0227
	AFloatNumber	fontsize = GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kFind_FontSize);
	if( fontsize > 12.0 )   fontsize = 12.0;
	//NVI_SetControlTextFont(kText_Folder_Path,fontname,fontsize);
	//#616 AFontWrapper::GetDialogDefaultFontName(fontname);
	//NVI_SetControlTextFont(kText_FileFilter,fontname,kTextSize);//win
	//NVI_SetControlTextFont(kText_Folder_Path,fontname,kTextSize);//win
}

void	AWindow_MultiFileFind::NVIDO_Show()
{
	if( mOpenClosingReplaceScreen == false )
	{
		UpdateFolderRadioButtons();
	}
}

/**
�t�H���_���W�I�{�^���X�V
*/
void	AWindow_MultiFileFind::UpdateFolderRadioButtons()
{
	AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID != kObjectID_Invalid )
	{
		//==================�e�L�X�g�E�C���h�E���L��ꍇ==================
		//�ݒ�ɂ���āA���L�����ꂩ�̃��W�I�{�^����ON�ɂ���
		switch(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kMultiFileFind_ActiveFolderKind))
		{
		  case 0:
			{
				NVI_SetControlBool(kRadio_Folder_SameFolder,true);
				break;
			}
		  case 1:
			{
				NVI_SetControlBool(kRadio_Folder_ParentFolder,true);
				break;
			}
		  case 2:
			{
				NVI_SetControlBool(kRadio_Folder_SameProject,true);
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
	else
	{
		//==================�e�L�X�g�E�C���h�E�������ꍇ==================
		//�蓮�t�H���_�I�����W�I�{�^����ON�ɂ���
		NVI_SetControlBool(kRadio_Folder_Select,true);
	}
}

void	AWindow_MultiFileFind::NVIDO_Hide()
{
	//�ݒ��db�ɕۑ�
	if( NVI_GetControlBool(kRadio_Folder_SameFolder) == true )
	{
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kMultiFileFind_ActiveFolderKind,0);
	}
	else if( NVI_GetControlBool(kRadio_Folder_ParentFolder) == true )
	{
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kMultiFileFind_ActiveFolderKind,1);
	}
	else if( NVI_GetControlBool(kRadio_Folder_SameProject) == true )
	{
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kMultiFileFind_ActiveFolderKind,2);
	}
	
	if( NVI_GetControlBool(kRadio_Target_AllFiles) == true )
	{
		GetApp().GetAppPref().SetData_Bool(AAppPrefDB::kMultiFileFind_UseFileFilter,false);
	}
	else if( NVI_GetControlBool(kRadio_Target_WildCard) == true )
	{
		GetApp().GetAppPref().SetData_Bool(AAppPrefDB::kMultiFileFind_UseFileFilter,true);
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kMultiFileFindFileFilterKind,1);
	}
	else if( NVI_GetControlBool(kRadio_Target_RegExp) == true )
	{
		GetApp().GetAppPref().SetData_Bool(AAppPrefDB::kMultiFileFind_UseFileFilter,true);
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kMultiFileFindFileFilterKind,0);
	}
	//
	AText	text;
	NVI_GetControlText(kText_Folder_Path,text);
	GetApp().NVI_GetAppPrefDB().SetData_Text(AAppPrefDB::kMultiFileFind_FolderPathText,text);
	NVI_GetControlText(kText_FileFilter,text);
	GetApp().NVI_GetAppPrefDB().SetData_Text(AAppPrefDB::kMultiFileFind_FileFilterText,text);
	
	if( mReplaceWindowMode == false )
	{
		//==================�����p���==================
		//#872
		//�܂肽���ݏ�ԕۑ�
		GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kMultiFileFindWindow_FindOptionsExpanded,NVI_GetControlBool(kTriangle_FindOptions));
		GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kMultiFileFindWindow_AdvancedSearchExpanded,NVI_GetControlBool(kTriangle_AdvancedSearch));
		
		//�E�C���h�E���ۑ�
		GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kMultiFileFindWindow_WindowWidth,NVI_GetWindowWidth());
	}
	else
	{
		//==================�u���p���==================
		//�o�b�N�A�b�v�t�H���_�ۑ�
		AText	backupFolderPath;
		NVI_GetControlText(kText_BackupFolder,backupFolderPath);
		GetApp().NVI_GetAppPrefDB().SetData_Text(AAppPrefDB::kMultiFileFind_BackupFolder,backupFolderPath);
		
	}
}

//�R���g���[����ԁiEnable/Disable���j���X�V
//NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()����R�[�������
//�N���b�N��A�N�e�B�x�[�g�ŃR�[�������̂ŁA���܂�d��������NVMDO_UpdateControlStatus()�ɂ͓���Ȃ����ƁB
void	AWindow_MultiFileFind::NVMDO_UpdateControlStatus()
{
	//------------------�A�N�e�B�u�t�H���_�ݒ�------------------
	if( !(mReplaceWindowMode == true && (mFindForReplaceFinished == true || mExecutingFindForReplace == true) ) )
	{
		ADocumentID	docID = kObjectID_Invalid;
		//�őO�ʂ̃e�L�X�g�E�C���h�E�擾
		AWindowID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
		if( winID != kObjectID_Invalid )
		{
			//�őO�ʂ̃e�L�X�g�E�C���h�E������΁A���̃E�C���h�E�̌��݂̃h�L�������g���擾
			docID = GetApp().SPI_GetTextWindowByID(winID).SPI_GetCurrentFocusTextDocument().GetObjectID();
		}
		if( docID != kObjectID_Invalid )
		{
			//------------------�e�L�X�g�E�C���h�E������ꍇ------------------
			//���݂̃A�N�e�B�u�ȃe�L�X�g�E�C���h�E�̃t�H���_��ݒ肷��B
			//B0431
			if( GetApp().SPI_GetTextDocumentByID(docID).SPI_IsRemoteFileMode() == true || 
			   GetApp().SPI_GetTextDocumentByID(docID).NVI_IsFileSpecified() == false )
			{
				mActiveFolder.Unspecify();
			}
			else if( NVI_GetControlBool(kRadio_Folder_SameFolder) == true )
			{
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParentFolder(mActiveFolder);
				AText	text;
				GetApp().GetAppPref().GetFilePathForDisplay(mActiveFolder,text);//B0389
				NVI_SetControlText(kText_Folder_Path,text);
			}
			else if( NVI_GetControlBool(kRadio_Folder_ParentFolder) == true )
			{
				AFileAcc	folder;
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParentFolder(folder);
				mActiveFolder.SpecifyParent(folder);
				AText	text;
				GetApp().GetAppPref().GetFilePathForDisplay(mActiveFolder,text);//B0389
				NVI_SetControlText(kText_Folder_Path,text);
			}
			else if( NVI_GetControlBool(kRadio_Folder_SameProject) == true )
			{
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetProjectFolder(mActiveFolder);
				if( mActiveFolder.IsSpecified() == false )
				{
					GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParentFolder(mActiveFolder);
				}
				AText	text;
				GetApp().GetAppPref().GetFilePathForDisplay(mActiveFolder,text);//B0389
				NVI_SetControlText(kText_Folder_Path,text);
			}
			//���W�I�{�^��enable
			NVI_SetControlEnable(kRadio_Folder_SameFolder,true);
			NVI_SetControlEnable(kRadio_Folder_ParentFolder,true);
			NVI_SetControlEnable(kRadio_Folder_SameProject,true);
		}
		else
		{
			//------------------�e�L�X�g�E�C���h�E���Ȃ��ꍇ==================
			//���W�I�{�^��disable
			NVI_SetControlEnable(kRadio_Folder_SameFolder,false);
			NVI_SetControlEnable(kRadio_Folder_ParentFolder,false);
			NVI_SetControlEnable(kRadio_Folder_SameProject,false);
		}
	}
	
	//------------------�����I�v�V����------------------
	/*#688
	NVI_SetCatchTab(kText_FindText,NVI_GetControlBool(kCheckBox_AllowReturnTab));
	NVI_SetCatchReturn(kText_FindText,NVI_GetControlBool(kCheckBox_AllowReturnTab));
	*/
	//#930 NVI_SetControlEnable(kCheckBox_IgnoreCase,(NVI_GetControlBool(kCheckBox_RegExp)==false));
	NVI_SetControlEnable(kCheckBox_WholeWord,(NVI_GetControlBool(kCheckBox_RegExp)==false));
	NVI_SetControlEnable(kCheckBox_Aimai,(NVI_GetControlBool(kCheckBox_RegExp)==false));
	NVI_SetControlEnable(kCheckBox_BackslashYen,(NVI_GetControlBool(kCheckBox_RegExp)==false));
	NVI_SetControlEnable(kCheckBox_IgnoreSpaces,(NVI_GetControlBool(kCheckBox_RegExp)==false));//#165
	
	AText	text;
	NVI_GetControlText(kText_FindText,text);
	NVI_SetControlEnable(kButton_Find,(text.GetLength()>0)&&((GetApp().SPI_IsMultiFileFindWorking()==false)));
	NVI_SetControlEnable(kButton_OpenCloseReplaceScreen,(GetApp().SPI_IsMultiFileFindWorking()==false));
	//if( NVI_GetControlBool(kRadio_Path) == true )
	{
		AText	text;
		NVI_GetControlText(kText_Folder_Path,text);
		if( text.GetLength() == 0 )
		{
			NVI_SetControlEnable(kButton_Find,false);
		}
	}
	/*
	//B0431
	if( NVI_GetControlBool(kRadio_ActiveFolder) == true )
	{
		if( mActiveFolder.IsSpecified() == false )
		{
			NVI_SetControlEnable(kButton_Find,false);
		}
	}
	*/
	
	//���W�I�{�^���őΏہF�S�t�@�C����I�����̊e�R���g���[��disable
	NVI_SetControlEnable(kText_FileFilter,!NVI_GetControlBool(kRadio_Target_AllFiles));
	NVI_SetControlEnable(kPopup_RecentlyUsedFileFilter,!NVI_GetControlBool(kRadio_Target_AllFiles));
	NVI_SetControlEnable(kPopup_FileFilterPreset,!NVI_GetControlBool(kRadio_Target_AllFiles));//#617
	
	//�������~�{�^�� #1376
	if( mReplaceWindowMode == false )
	{
		NVI_SetControlEnable(kButton_FindAbort,(GetApp().SPI_IsMultiFileFindWorking()==true));
	}
	
	//==================�}���`�t�@�C���u���p==================
	
	if( mReplaceWindowMode == true )
	{
		//�u���p�������s�����ǂ����̔���
		if( mExecutingFindForReplace == true )
		{
			//�������s���Ȃ�v���O���X�E���f�R���g���[���\��
			NVI_SetShowControl(kProgressText,true);
			NVI_SetShowControl(kButton_FindAbort,true);
			NVI_SetShowControl(kProgressText,true);
		}
		else
		{
			//�������s���łȂ���΃v���O���X�E���f�R���g���[����\��
			NVI_SetProgressIndicatorProgress(0,0,false);
			NVI_SetShowControl(kProgressText,false);
			NVI_SetShowControl(kButton_FindAbort,false);
			NVI_SetShowControl(kProgressText,false);
		}
		//�u���e�L�X�g�擾
		AText	replaceText;
		NVI_GetControlText(kText_ReplaceText,replaceText);
		//�u���e�L�X�g�G���A�͉��s�\ #1060
		NVI_SetCatchTab(kText_ReplaceText,true);
		NVI_SetCatchReturn(kText_ReplaceText,true);
		//�u���p�����������ǂ����̔���
		if( mFindForReplaceFinished == true )
		{
			//�u���p���������Ȃ�u�����s�E�L�����Z���{�^��enable
			NVI_SetControlEnable(kButton_ExecuteReplace,true);//(replaceText.GetItemCount() > 0));
			NVI_SetControlEnable(kButton_CancelReplace,true);
		}
		else
		{
			//�u���p�����������Ȃ�u�����s�E�L�����Z���{�^��disable
			NVI_SetControlEnable(kButton_ExecuteReplace,false);
			NVI_SetControlEnable(kButton_CancelReplace,false);
		}
		//�u���p�������s���܂��͒u���p�����������ǂ����̔���
		if( mFindForReplaceFinished == true || mExecutingFindForReplace == true )
		{
			//�u���p�������s���܂��͒u���p���������Ȃ�e�����p�����[�^��disable
			NVI_SetControlEnable(kText_FindText,false);
			NVI_SetControlEnable(kRadio_Folder_SameFolder,false);
			NVI_SetControlEnable(kRadio_Folder_ParentFolder,false);
			NVI_SetControlEnable(kRadio_Folder_SameProject,false);
			NVI_SetControlEnable(kRadio_Folder_Select,false);
			NVI_SetControlEnable(kText_Folder_Path,false);
			NVI_SetControlEnable(kButton_SelectFolder,false);
			NVI_SetControlEnable(kPopup_RecentlyUsedFindText,false);
			NVI_SetControlEnable(kPopup_RecentlyUsedPath,false);
			NVI_SetControlEnable(kRadio_Target_AllFiles,false);
			NVI_SetControlEnable(kRadio_Target_WildCard,false);
			NVI_SetControlEnable(kRadio_Target_RegExp,false);
			NVI_SetControlEnable(kPopup_FileFilterPreset,false);
			NVI_SetControlEnable(kPopup_RecentlyUsedFileFilter,false);
			NVI_SetControlEnable(kText_FileFilter,false);
			NVI_SetControlEnable(kCheckBox_IgnoreCase,false);
			NVI_SetControlEnable(kCheckBox_WholeWord,false);
			NVI_SetControlEnable(kCheckBox_Aimai,false);
			NVI_SetControlEnable(kCheckBox_BackslashYen,false);
			NVI_SetControlEnable(kCheckBox_RegExp,false);
			NVI_SetControlEnable(kCheckBox_Recursive,false);
			NVI_SetControlEnable(kCheckBox_DisplayPosition,false);
			NVI_SetControlEnable(kCheckBox_OnlyVisibleFile,false);
			NVI_SetControlEnable(kCheckBox_OnlyTextFile,false);
			NVI_SetControlEnable(kCheckBox_IgnoreSpaces,false);
			NVI_SetControlEnable(kButton_SetupFuzzySearch,false);
		}
		else
		{
			//�u���p�������s���܂��͒u���p���������Ȃ�e�����p�����[�^��enable
			NVI_SetControlEnable(kText_FindText,true);
			//NVI_SetControlEnable(kRadio_Folder_SameFolder,true);
			//NVI_SetControlEnable(kRadio_Folder_ParentFolder,true);
			//NVI_SetControlEnable(kRadio_Folder_SameProject,true);
			NVI_SetControlEnable(kRadio_Folder_Select,true);
			NVI_SetControlEnable(kText_Folder_Path,true);
			NVI_SetControlEnable(kButton_SelectFolder,true);
			NVI_SetControlEnable(kPopup_RecentlyUsedFindText,true);
			NVI_SetControlEnable(kPopup_RecentlyUsedPath,true);
			NVI_SetControlEnable(kRadio_Target_AllFiles,true);
			NVI_SetControlEnable(kRadio_Target_WildCard,true);
			NVI_SetControlEnable(kRadio_Target_RegExp,true);
			//NVI_SetControlEnable(kPopup_FileFilterPreset,true);
			//NVI_SetControlEnable(kPopup_RecentlyUsedFileFilter,true);
			//NVI_SetControlEnable(kText_FileFilter,true);
			NVI_SetControlEnable(kCheckBox_IgnoreCase,true);//#0
			//NVI_SetControlEnable(kCheckBox_WholeWord,true);
			//NVI_SetControlEnable(kCheckBox_Aimai,true);
			//NVI_SetControlEnable(kCheckBox_BackslashYen,true);
			NVI_SetControlEnable(kCheckBox_RegExp,true);
			NVI_SetControlEnable(kCheckBox_Recursive,true);
			NVI_SetControlEnable(kCheckBox_DisplayPosition,true);
			NVI_SetControlEnable(kCheckBox_OnlyVisibleFile,true);
			NVI_SetControlEnable(kCheckBox_OnlyTextFile,true);
			//NVI_SetControlEnable(kCheckBox_IgnoreSpaces,true);
			NVI_SetControlEnable(kButton_SetupFuzzySearch,true);
		}
		//�u���Ώۃt�@�C�����O�{�^���X�V
		AArray<AIndex>	indexArray;
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_GetSelectedDBIndexArray(indexArray);
		NVI_SetControlEnable(kButton_ExcludeFile,(indexArray.GetItemCount()>0));
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_GetSelectedDBIndexArray(indexArray);
		NVI_SetControlEnable(kButton_IncludeFile,(indexArray.GetItemCount()>0));
		
		//�u���Ώۃt�@�C���ǉ��{�^���X�V
		NVI_SetControlEnable(kText_BackupFolder,NVI_GetControlBool(kCheck_BackupFolder));
		NVI_SetControlEnable(kButton_SelectBackupFolder,NVI_GetControlBool(kCheck_BackupFolder));
		NVI_SetControlEnable(kButton_DefaultBackupFolder,NVI_GetControlBool(kCheck_BackupFolder));
		
	}
	
}

//WindowActivated���̃R�[���o�b�N
void	AWindow_MultiFileFind::EVTDO_WindowActivated()
{
	AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID != kObjectID_Invalid )
	{
		NVI_SetInputBackslashByYenKeyMode(GetApp().SPI_GetModePrefDB(GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex()).
					GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey));
	}
	else
	{
		NVI_SetInputBackslashByYenKeyMode(false);
	}
}

//WindowDeactivated���̃R�[���o�b�N
void	AWindow_MultiFileFind::EVTDO_WindowDeactivated()
{
}

/**
�E�C���h�E�����{�^���E���郁�j���[�I���������iAWindow::NVI_TryClose()�ɂāAAWindow�̃N���[�Y�\�t���O�i���������̓N���[�Y�s�j�`�F�b�N�ς݁j
*/
void	AWindow_MultiFileFind::NVIDO_TryClose( const AIndex inTabIndex )
{
	NVI_Hide();
}

#pragma mark ===========================

#pragma mark ---�O������̃p�����[�^�ݒ�

//FindText�ݒ�
void	AWindow_MultiFileFind::SPI_SetFindText( const AText& inFindText )
{
	NVI_SetControlText(kText_FindText,inFindText);
	NVM_UpdateControlStatus();
}

#pragma mark ===========================

#pragma mark ---�}���`�t�@�C���u��

//#65
/**
�u���p���open/close
*/
void	AWindow_MultiFileFind::SPI_OpenCloseReplaceScreen( const ABool inReplaceMode )
{
	if( inReplaceMode != mReplaceWindowMode )
	{
		//�u���p���open/close���t���OON
		mOpenClosingReplaceScreen = true;
		//�p�����[�^���L��
		ABool	selectFolderRadioButton = NVI_GetControlBool(kRadio_Folder_Select);
		AFindParameter	findParam;
		GetFindParam(findParam);
		//�E�C���h�E�����
		NVI_Close();
		//�u���p��ʏ�ԕύX
		mReplaceWindowMode = inReplaceMode;
		//�E�C���h�E����
		NVI_Create(kObjectID_Invalid);
		//��ԕ���
		NVI_SetControlText(kText_FindText,findParam.findText);
		NVI_SetControlBool(kRadio_Folder_Select,selectFolderRadioButton);
		NVI_SetControlBool(kCheckBox_IgnoreCase,findParam.ignoreCase);
		NVI_SetControlBool(kCheckBox_WholeWord,findParam.wholeWord);
		NVI_SetControlBool(kCheckBox_Aimai,findParam.fuzzy);
		NVI_SetControlBool(kCheckBox_BackslashYen,!(findParam.ignoreBackslashYen));
		NVI_SetControlBool(kCheckBox_RegExp,findParam.regExp);
		NVI_SetControlBool(kCheckBox_Recursive,findParam.recursive);
		NVI_SetControlBool(kCheckBox_DisplayPosition,findParam.displayPosition);
		NVI_SetControlBool(kCheckBox_IgnoreSpaces,findParam.ignoreSpaces);//#165
		NVI_SetControlBool(kCheckBox_OnlyVisibleFile,findParam.onlyVisibleFile);
		NVI_SetControlBool(kCheckBox_OnlyTextFile,findParam.onlyTextFile);
		//�u����ԏ�����
		mExecutingFindForReplace = false;
		mFindForReplaceFinished = false;
		mFindForReplaceAborted = false;
		//�E�C���h�E�\��
		NVI_Show();
		//�u���p���open/close���t���OOFF
		mOpenClosingReplaceScreen = false;
	}
}

/**
�}���`�t�@�C����������InternalEvent��M������
*/
void	AWindow_MultiFileFind::SPI_MultiFileReplace_FindResult( const ATextArray& inFilePathArray, const AText& inTextEncoding )
{
	//�E�C���h�E���\������Ă��Ȃ�����A�u���p��ʂł͂Ȃ��Ƃ��͉������Ȃ�
	if( NVI_IsWindowVisible() == false || mReplaceWindowMode == false )   return;
	
	//���ʃt�@�C�����X�g���A�Ώۃt�@�C�����X�g�ɒǉ�
	AItemCount	fileCount = inFilePathArray.GetItemCount();
	for( AIndex i = 0; i < fileCount; i++ )
	{
		AText	filepath;
		inFilePathArray.Get(i,filepath);
		if( filepath.GetItemCount() > 0 &&
					mReplaceTargetFilePathArray.Find(filepath) == kIndex_Invalid && 
					mReplaceExclusionFilePathArray.Find(filepath) == kIndex_Invalid )
		{
			mReplaceTargetFilePathArray.Add(filepath);
			mReplaceTargetFilePathArray_TextEncoding.Add(inTextEncoding);
		}
	}
	//�Ώۃt�@�C���e�[�u�����X�V
	MultiFileReplace_UpdateTargetFileTables();
}

/**
�}���`�t�@�C����������InternalEvent��M������
*/
void	AWindow_MultiFileFind::SPI_MultiFileReplace_FindCompleted()
{
	//�E�C���h�E���\������Ă��Ȃ�����A�u���p��ʂł͂Ȃ��Ƃ��͉������Ȃ�
	if( NVI_IsWindowVisible() == false || mReplaceWindowMode == false )   return;
	
	//�������f����
	if( mFindForReplaceAborted == false )
	{
		//���f�łȂ���΁A�u���p���������t���OON
		mFindForReplaceFinished = true;
	}
	else
	{
		//���f�̏ꍇ�A�u���p���������t���OOFF
		mFindForReplaceFinished = false;
	}
	//�u���p�������s���t���OOFF
	mExecutingFindForReplace = false;
	//�v���O���X�e�L�X�g�N���A
	SPI_MultiFileReplace_SetProgressText(GetEmptyText());
	
	//�\���X�V
	NVM_UpdateControlStatus();
}

/**
�u���p�p�����[�^�f�[�^�S�폜
*/
void	AWindow_MultiFileFind::MultiFileReplace_ClearReplaceData()
{
	//�S�t���OOFF
	mExecutingFindForReplace = false;
	mFindForReplaceFinished = false;
	mFindForReplaceAborted = false;
	
	//�Ώۃt�@�C�����X�g�S�폜
	mReplaceTargetFilePathArray.DeleteAll();
	mReplaceTargetFilePathArray_TextEncoding.DeleteAll();
	mReplaceExclusionFilePathArray.DeleteAll();
	mReplaceExclusionFilePathArray_TextEncoding.DeleteAll();
	//�Ώۃt�@�C���e�[�u���X�V
	MultiFileReplace_UpdateTargetFileTables();
	//�������ʑS�폜
	GetApp().SPI_GetIndexWindowDocumentByID(mFindResultDocumentID).SPI_DeleteAllGroup();
	//�\���X�V
	NVM_UpdateControlStatus();
}

/**
�u���Ώۃt�@�C���e�[�u���X�V
*/
void	AWindow_MultiFileFind::MultiFileReplace_UpdateTargetFileTables()
{
	//�}���`�t�@�C���������s�t�H���_�̃p�X�̕������擾
	AText	baseFolderPath;
	baseFolderPath.SetText(mFindParamerForReplace.folderPath);
	baseFolderPath.AddPathDelimiter(kUChar_Slash);
	AItemCount	baseFolderPathLen = baseFolderPath.GetItemCount();
	//�u���Ώۃt�@�C���e�[�u���X�V
	{
		//���΃p�X�ɕϊ�
		ATextArray	partialPathArray;
		AItemCount	itemCount = mReplaceTargetFilePathArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			AText	path;
			mReplaceTargetFilePathArray.Get(i,path);
			path.Delete(0,baseFolderPathLen);
			partialPathArray.Add(path);
		}
		//�e�[�u���ɐݒ�
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_BeginSetTable();
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetColumn_Text(0,partialPathArray);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetColumn_Text(1,mReplaceTargetFilePathArray_TextEncoding);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_EndSetTable();
	}
	//�u�����O�t�@�C���e�[�u���X�V
	{
		//���΃p�X�ɕϊ�
		ATextArray	partialPathArray;
		AItemCount	itemCount = mReplaceExclusionFilePathArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			AText	path;
			mReplaceExclusionFilePathArray.Get(i,path);
			path.Delete(0,baseFolderPathLen);
			partialPathArray.Add(path);
		}
		//�e�[�u���ɐݒ�
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_BeginSetTable();
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetColumn_Text(0,partialPathArray);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetColumn_Text(1,mReplaceExclusionFilePathArray_TextEncoding);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_EndSetTable();
	}
	//�\���X�V
	NVM_UpdateControlStatus();
}

/**
���O�{�^���N���b�N������
*/
void	AWindow_MultiFileFind::MultiFileReplace_ExcludeButton()
{
	//�I������index�擾
	AArray<AIndex>	indexArray;
	NVI_GetListView(kControlID_ReplaceTargetListView).SPI_GetSelectedDBIndexArray(indexArray);
	//�I������
	NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetSelect(kIndex_Invalid);
	NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetSelect(kIndex_Invalid);
	//�Ώۃt�@�C�����X�g����I�����ڂ̃t�@�C���p�X���擾
	ATextArray	pathArray;
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		pathArray.Add(mReplaceTargetFilePathArray.GetTextConst(indexArray.Get(i)));
	}
	//�Ώۃt�@�C�����X�g����t�@�C���p�X���폜���A���O�t�@�C�����X�g�Ƀt�@�C���p�X��ǉ�
	AText	tecname;
	for( AIndex i = 0; i < pathArray.GetItemCount(); i++ )
	{
		AText	path;
		pathArray.Get(i,path);
		//���X�g����폜
		for( AIndex index = 0; index < mReplaceTargetFilePathArray.GetItemCount();  )
		{
			if( mReplaceTargetFilePathArray.GetTextConst(index).Compare(path) == true )
			{
				mReplaceTargetFilePathArray.Delete1(index);
				mReplaceTargetFilePathArray_TextEncoding.Get(index,tecname);
				mReplaceTargetFilePathArray_TextEncoding.Delete1(index);
			}
			else
			{
				index++;
			}
		}
		//
		mReplaceExclusionFilePathArray.Add(path);
		mReplaceExclusionFilePathArray_TextEncoding.Add(tecname);
	}
	//�e�[�u���X�V
	MultiFileReplace_UpdateTargetFileTables();
	//�\���X�V
	NVM_UpdateControlStatus();
}

/**
�Ώےǉ��{�^���N���b�N������
*/
void	AWindow_MultiFileFind::MultiFileReplace_IncludeButton()
{
	//�I������index�擾
	AArray<AIndex>	indexArray;
	NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_GetSelectedDBIndexArray(indexArray);
	//�I������
	NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetSelect(kIndex_Invalid);
	NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetSelect(kIndex_Invalid);
	//���O�t�@�C�����X�g����I�����ڂ̃t�@�C���p�X���擾
	ATextArray	pathArray;
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		pathArray.Add(mReplaceExclusionFilePathArray.GetTextConst(indexArray.Get(i)));
	}
	//���O�t�@�C�����X�g����t�@�C���p�X���폜���A�Ώۃt�@�C�����X�g�Ƀt�@�C���p�X��ǉ�
	AText	tecname;
	for( AIndex i = 0; i < pathArray.GetItemCount(); i++ )
	{
		AText	path;
		pathArray.Get(i,path);
		//���X�g����폜
		for( AIndex index = 0; index < mReplaceExclusionFilePathArray.GetItemCount();  )
		{
			if( mReplaceExclusionFilePathArray.GetTextConst(index).Compare(path) == true )
			{
				mReplaceExclusionFilePathArray.Delete1(index);
				mReplaceExclusionFilePathArray_TextEncoding.Get(index,tecname);
				mReplaceExclusionFilePathArray_TextEncoding.Delete1(index);
			}
			else
			{
				index++;
			}
		}
		//
		mReplaceTargetFilePathArray.Add(path);
		mReplaceTargetFilePathArray_TextEncoding.Add(tecname);
	}
	//�e�[�u���X�V
	MultiFileReplace_UpdateTargetFileTables();
	//�\���X�V
	NVM_UpdateControlStatus();
}

/**
�u�����s
*/
void	AWindow_MultiFileFind::MultiFileReplace_ExecuteReplace()
{
	//�u�������J�E���^
	AItemCount	totalReplacedCount = 0;
	
	//�u��������擾
	NVI_GetControlText(kText_ReplaceText,mFindParamerForReplace.replaceText);
	
	//�u�����s���t���OON
	mExecutingReplace = true;
	
	//#846
	//���[�_���Z�b�V����
	AStEditProgressModalSession	modalSession(kEditProgressType_MultiFileReplace,true,true,true);
	ABool	aborted = false;
	ATextArray	saveFailFilePathArray;
	try
	{
		
		//�u���v���O���X�E�C���h�E�\���i�o�b�N�A�b�v���s����\���j
		AText	title, message;
		title.SetLocalizedText("EditProgress_Title_MultiFileReplace");
		message.SetLocalizedText("EditProgress_Message_BackupForMultiFileReplace");
		GetApp().SPI_GetEditProgressWindow().SPI_SetText(title,message);
		GetApp().SPI_GetEditProgressWindow().NVI_RefreshWindow();
		//�o�b�N�A�b�vON�Ȃ�o�b�N�A�b�v���s
		if( NVI_GetControlBool(kCheck_BackupFolder) == true )
		{
			MultiFileReplace_Backup(mFindParamerForReplace.folderPath,mFindParamerForReplace.findText,mFindParamerForReplace.replaceText);
		}
		//�u���v���O���X�E�C���h�E�\���i�u������\���j
		title.SetLocalizedText("EditProgress_Title_MultiFileReplace");
		message.SetLocalizedText("EditProgress_Message_MultiFileReplace");
		GetApp().SPI_GetEditProgressWindow().SPI_SetText(title,message);
		GetApp().SPI_GetEditProgressWindow().NVI_RefreshWindow();
		
		//�u���Ώۃt�@�C�����Ƃ̃��[�v
		AItemCount	fileCount = mReplaceTargetFilePathArray.GetItemCount();
		for( AIndex i = 0; i < fileCount; i++ )
		{
			//�Z�b�V�����p�����f
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_MultiFileReplace,
																	  totalReplacedCount,true,i,fileCount) == false )
			{
				aborted = true;
				break;
			}
			//�Ώۃt�@�C���p�X�擾
			AText	filepath;
			mReplaceTargetFilePathArray.Get(i,filepath);
			//�Ώۃt�@�C���擾
			AFileAcc	file;
			file.Specify(filepath);
			//�Ώۃt�@�C�������݂��A���A�s�܂�Ԃ��������i�����łɕҏW�\���j�ǂ����̔���
			AObjectID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,file);
			ABool	docExistAndWrapCalculated = false;
			if( docID != kObjectID_Invalid )
			{
				switch(GetApp().SPI_GetTextDocumentByID(docID).SPI_GetDocumentInitState())
				{
				  case kDocumentInitState_SyntaxRecognizing:
				  case kDocumentInitState_Completed:
					{
						docExistAndWrapCalculated = true;
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
			}
			if( docExistAndWrapCalculated == false )
			{
				//------------------�Ώۃt�@�C���̃h�L�������g�����݂��Ȃ��A�܂��́A���݂��邪�s�܂�Ԃ��v�Z�����O�ꍇ------------------
				
				//�V�K�e�L�X�g�h�L�������g����
				ATextDocumentFactory	factory(this,kObjectID_Invalid);
				docID = GetApp().NVI_CreateDocument(factory);
				GetApp().SPI_GetTextDocumentByID(docID).SPI_Init_LocalFile(file,GetEmptyText());
				//�}���`�t�@�C���u���p�̃��[�h���s
				if( GetApp().SPI_GetTextDocumentByID(docID).SPI_LoadForMultiFileReplace() == true )
				{
					//==================�h�L�������g�t�@�C�����[�h���������ꍇ==================
					
					//�u�����s
					AItemCount	offset = 0;
					AItemCount	replacedCount = 0;
					AIndex	selectSpos = 0, selectEpos = 0;
					if( GetApp().SPI_GetTextDocumentByID(docID).SPI_ReplaceText(mFindParamerForReplace,0,GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength(),
																				replacedCount,offset,selectSpos,selectEpos) == false )
					{
						aborted = true;
						break;
					}
					//�u���g�[�^�����X�V
					totalReplacedCount += replacedCount;
					//�ۑ����s
					if( GetApp().SPI_GetTextDocumentByID(docID).SPI_Save(false) == false )
					{
						//�ۑ����s�̏ꍇ���X�g�ɒǉ�
						saveFailFilePathArray.Add(filepath);
					}
				}
				else
				{
					//==================�h�L�������g�t�@�C�����[�h���s�����ꍇ==================
					//�t�@�C�����폜���ꂽ�A�A���}�E���g���ꂽ�A�l�b�g���[�N�ؒf���ꂽ�Ȃ�
					
					//�ۑ����s�̏ꍇ���X�g�ɒǉ�
					saveFailFilePathArray.Add(filepath);
				}
				//�h�L�������g�����
				GetApp().NVI_DeleteDocument(docID);
			}
			else
			{
				//------------------�Ώۃt�@�C���̃h�L�������g�����݂��A���A�s�܂�Ԃ��v�Z������̏ꍇ------------------
				
				//���X��dirty���ǂ������L��
				ABool	origDirty = GetApp().SPI_GetTextDocumentByID(docID).NVI_IsDirty();
				//�u�����s
				AItemCount	offset = 0;
				AItemCount	replacedCount = 0;
				AIndex	selectSpos = 0, selectEpos = 0;
				if( GetApp().SPI_GetTextDocumentByID(docID).SPI_ReplaceText(mFindParamerForReplace,0,GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength(),
																		replacedCount,offset,selectSpos,selectEpos) == false )
				{
					aborted = true;
					break;
				}
				//�u���g�[�^�����X�V
				totalReplacedCount += replacedCount;
				//���Xdirty�ł͂Ȃ������ꍇ�́A�ۑ����s����
				if( origDirty == false )
				{
					if( GetApp().SPI_GetTextDocumentByID(docID).SPI_Save(false) == false )
					{
						//�ۑ����s�̏ꍇ���X�g�ɒǉ�
						saveFailFilePathArray.Add(filepath);
					}
				}
			}
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	
	//�u�����s�t���OOFF
	mExecutingReplace = false;
	
	//status�R�}���h���s�i�}���`�t�@�C���u�����s����status�R�}���h���s�}�����Ă���̂ŁA�����ł܂Ƃ߂Ď��s�j
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true )
	{
		AFileAcc	folder;
		folder.Specify(mFindParamerForReplace.folderPath);
		GetApp().SPI_GetSCMFolderManager().UpdateFolderStatus(folder);
	}
	
	if( aborted == false )
	{
		//------------------�u���r�����f�Ȃ��̏ꍇ------------------
		//�u�����ʐݒ�
		if( saveFailFilePathArray.GetItemCount() > 0 )
		{
			//------------------�ۑ����s�_�C�A���O�\��------------------
			//�ۑ����s�e�L�X�g
			AText	message1;
			AText	message2;
			message1.SetLocalizedText("MultiFileReplace_FailSave");
			message2.SetLocalizedText("MultiFileReplace_FailSaveMessage");
			//���s�t�@�C�����e�L�X�g��ݒ�
			AText	t;
			saveFailFilePathArray.Implode(kUChar_LineEnd,t);
			message2.AddText(t);
			//���ʃ_�C�A���O�\��
			NVI_ShowChildWindow_OK(message1,message2,true);
		}
		else
		{
			//------------------�u��OK�_�C�A���O�\��------------------
			AText	message1;
			AText	message2;
			message1.SetLocalizedText("MultiFileReplace_Result");
			AText	text;
			text.SetFormattedCstring("%d",totalReplacedCount);
			message1.ReplaceParamText('0',text);
			//���ʃ_�C�A���O�\��
			NVI_ShowChildWindow_OK(message1,message2);
		}
		
		//�u���f�[�^�S�폜
		MultiFileReplace_ClearReplaceData();
	}
	else
	{
		//------------------�u���r�����f�̏ꍇ------------------
		//���f�_�C�A���O�\��
		AText	message1;
		message1.SetLocalizedText("MultiFileReplace_Aborted");
		NVI_ShowChildWindow_OK(message1,GetEmptyText());
	}
}

/**
�u���O�o�b�N�A�b�v
*/
void	AWindow_MultiFileFind::MultiFileReplace_Backup( const AFileAcc& inSrcBaseFolder, const AText& inFindText, const AText& inReplaceText )
{
	//�o�b�N�A�b�v���[�g�t�H���_�擾
	AText	backupRootFolderPath;
	NVI_GetControlText(kText_BackupFolder,backupRootFolderPath);
	AFileAcc	backupRootFolder;
	backupRootFolder.Specify(backupRootFolderPath);
	
	//�o�b�N�A�b�v���[�g�t�H���_����
	backupRootFolder.CreateFolder();
	if( backupRootFolderPath.GetItemCount() == 0 || backupRootFolder.Exist() == false || backupRootFolder.IsFolder() == false )
	{
		//�o�b�N�A�b�v���[�g�t�H���_�������o���Ȃ������ꍇ
		//�f�t�H���g�̃o�b�N�A�b�v���[�g�t�H���_�擾
		GetApp().SPI_GetDefaultMultiFileReplaceBackupFolder(backupRootFolder);
		backupRootFolder.GetPath(backupRootFolderPath);
		NVI_SetControlText(kText_BackupFolder,backupRootFolderPath);
	}
	//�o�b�N�A�b�v�t�H���_���擾
	AText	foldername("MultiFileReplaceBackup_20");
	AText	t;
	ADateTimeWrapper::Get6LettersDateText(t);
	foldername.AddText(t);
	ADateTimeWrapper::Get2LettersHourText(t);
	foldername.AddText(t);
	ADateTimeWrapper::Get2LettersMinuteText(t);
	foldername.AddText(t);
	ADateTimeWrapper::Get2LettersSecondText(t);
	foldername.AddText(t);
	//�o�b�N�A�b�v�t�H���_�擾
	AFileAcc	backupFolder;
	backupFolder.SpecifyUniqChildFile(backupRootFolder,foldername);
	backupFolder.CreateFolder();
	//���O�e�L�X�g����
	AText	datetext;
	ADateTimeWrapper::GetDateTimeText(datetext);
	AText	log;
	log.AddCstring("Multi File Replace Backup\nDate: ");
	log.AddText(datetext);
	log.AddCstring("\nFolder: ");
	AText	folderpath;
	inSrcBaseFolder.GetPath(folderpath);
	log.AddText(folderpath);
	log.AddCstring("\nFind Text: ");
	log.AddText(inFindText);
	log.AddCstring("\nReplace Text: ");
	log.AddText(inReplaceText);
	log.AddCstring("\n");
	AFileAcc	logfile;
	logfile.SpecifyChild(backupFolder,"log.txt");
	logfile.CreateFile();
	logfile.WriteFile(log);
	//�Ώۃt�@�C�����̃��[�v
	for( AIndex i = 0; i < mReplaceTargetFilePathArray.GetItemCount(); i++ )
	{
		//�Ώۃt�@�C���̃t�@�C���p�X�擾
		AText	filepath;
		mReplaceTargetFilePathArray.Get(i,filepath);
		//�Ώۃt�@�C���擾
		AFileAcc	srcfile;
		srcfile.Specify(filepath);
		//���΃p�X�擾
		AText	relativePath;
		srcfile.GetPartialPath(inSrcBaseFolder,relativePath);
		//�o�b�N�A�b�v��t�@�C���擾
		AFileAcc	dstfile;
		dstfile.SpecifyChild(backupFolder,relativePath);
		//�t�@�C���R�s�[
		srcfile.CopyFileOrFolderTo(dstfile,false,false);
	}
}

/**
�v���O���X�e�L�X�g�ݒ�
*/
void	AWindow_MultiFileFind::SPI_MultiFileReplace_SetProgressText( const AText& inText )
{
	if( NVI_IsWindowVisible() == false )   return;
	NVI_SetControlText(kProgressText,inText);
}

/**
�v���O���X�ݒ�
*/
void	AWindow_MultiFileFind::SPI_MultiFileReplace_Progress( const short inPercent )
{
	if( NVI_IsWindowVisible() == false )   return;
#if IMPLEMENTATION_FOR_MACOSX
	NVI_SetProgressIndicatorProgress(0,inPercent);
#else
	NVI_SetControlNumber(kProgressBar,inPercent);
	#endif
}


#pragma mark ===========================

#pragma mark <�������W���[��>

#pragma mark ===========================

//�ŋߎg���������e�L�X�g���j���[�̍X�V
void	AWindow_MultiFileFind::UpdateRecentlyUsedFindTextMenu()
{
	ATextArray	textArray;
	textArray.SetFromTextArray(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFindString));
	AText	text;
	text.SetLocalizedText("MultiFileFind_RecentlyUsedFindString");
	textArray.Insert1(0,text);
	text.SetLocalizedText("RecentlyMenus_DeleteAll");
	textArray.Add(text);
	NVI_SetMenuItemsFromTextArray(kPopup_RecentlyUsedFindText,textArray);
}

//�ŋߎg���������e�L�X�g�̒ǉ�
void	AWindow_MultiFileFind::SPI_AddRecentlyUsedFindText( const AText& inText )
{
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )   return;//#352
	
	GetApp().GetAppPref().AddRecentlyUsed_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFindString,inText,128);
	UpdateRecentlyUsedFindTextMenu();
}

//�ŋߎg�����t�@�C���t�B���^�̍X�V
void	AWindow_MultiFileFind::UpdateRecentlyUsedFileFilterMenu()
{
	ATextArray	textArray;
	textArray.SetFromTextArray(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter));
	AText	text;
	text.SetLocalizedText("MultiFileFind_RecentlyUsedFileFilter");
	textArray.Insert1(0,text);
	text.SetLocalizedText("RecentlyMenus_DeleteAll");
	textArray.Add(text);
	NVI_SetMenuItemsFromTextArray(kPopup_RecentlyUsedFileFilter,textArray);
}

//�ŋߎg�����t�@�C���t�B���^�̒ǉ�
void	AWindow_MultiFileFind::SPI_AddRecentlyUsedFileFilter( const AText& inText )
{
	GetApp().GetAppPref().AddRecentlyUsed_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter,inText,128);
	UpdateRecentlyUsedFileFilterMenu();
}

//�ŋߎg�����t�@�C���p�X�̍X�V
void	AWindow_MultiFileFind::UpdateRecentlyUsedPathMenu()
{
	ATextArray	textArray;
	textArray.SetFromTextArray(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder));
	AText	text;
	text.SetLocalizedText("MultiFileFind_RecentlyUsedUsedFolder");
	textArray.Insert1(0,text);
	text.SetLocalizedText("RecentlyMenus_DeleteAll");
	textArray.Add(text);
	NVI_SetMenuItemsFromTextArray(kPopup_RecentlyUsedPath,textArray);
}

//�ŋߎg�����t�@�C���t�B���^�̒ǉ�
void	AWindow_MultiFileFind::SPI_AddRecentlyUsedPath( const AFileAcc& inFolder )
{
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )   return;//#352
	
	AText	pathtext;
	//B0389 OK inFolder.GetPath(pathtext,kFilePathType_1);
	GetApp().GetAppPref().GetFilePathForDisplay(inFolder,pathtext);//B0389
	GetApp().GetAppPref().AddRecentlyUsed_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder,pathtext,128);
	UpdateRecentlyUsedPathMenu();
}

