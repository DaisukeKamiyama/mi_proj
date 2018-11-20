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

AWindow_FileList

*/

#include "stdafx.h"

#include "AWindow_FileList.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowHeader.h"
#include "AView_SubWindowSizeBox.h"

const AControlID		kButton_ModeChange				= 102;
const AControlID		kButton_Sort					= 103;

//VScroll bar
const AControlID		kControlID_VScrollBar			= 104;
const ANumber			kVScrollBarWidth = 11;

//#798
//�t�B���^
const AControlID		kControlID_Filter				= 201;
const ANumber			kHeight_FilterTopBottomMargin	= 8;

const ADataID			kColumn_FileName				= 0;

//#725
//SubWindow�p�^�C�g���o�[view
const AControlID	kControlID_TitleBar = 901;

//#725
//SubWindow�p�Z�p���[�^view
const AControlID	kControlID_Separator = 902;

//#725
//�T�C�Y�{�b�N�X
const AControlID	kControlID_SizeBox = 903;
const ANumber	kWidth_SizeBox = 15;
const ANumber	kHeight_SizeBox = 15;

//#725
//�w�i�F�`��pview
const AControlID	kControlID_Background = 905;

//�w�b�_view
const AControlID	kControlID_Header = 906;
const ANumber	kHeight_Header1Line = 18;
const ANumber	kHeight_Header2Lines = 36;

//�w�b�_��̃e�L�X�g
const AControlID	kControlID_FolderPath = 701;
const AControlID	kControlID_ProjectModeFilter = 702;
const ANumber		kHeight_HeaderButton = 15;
const ANumber		kLeftMargin_HeaderButton = 5;
const ANumber		kRightMargin_HeaderButton = 5;

//�����[�g�t�@�C���v���O���X�\�� #1236
const AControlID	kControlID_RemoteAccessProgressIndicator = 750;
const ANumber	kHeight_ProgressIndicator = 16;
const ANumber	kWidth_ProgressIndicator = 16;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_FileList
#pragma mark ===========================
//�t�@�C�����X�g�E�C���h�E

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_FileList::AWindow_FileList():AWindow(/*#175NULL*/), mCurrentSameFolderIndex(kIndex_Invalid), mCurrentSameProjectIndex(kIndex_Invalid),
		mListViewControlID(kControlID_Invalid), mRecentSortMode(false), 
		mSelectionIndex_Recent(kIndex_Invalid), mSelectionIndex_RemoteConnection(kIndex_Invalid), 
		mSelectionIndex_SameFolder(kIndex_Invalid), mSelectionIndex_SameProject(kIndex_Invalid)
		,mCurrentModeIndex(kIndex_Invalid)//#533
		,mRemoteConnectionCurrentLevel(0),mRemoteConnectionObjectID(kObjectID_Invalid)//#361
,mProjectFilterMode(kFileListProjectFilter_SameModeFiles)//#873
,mRemoteConnectionCurrentLevel_Request(0)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kFileListPosition);
	//#138 �f�t�H���g�͈�(10000�`)���g������ݒ�s�v NVM_SetDynamicControlIDRange(kDynamicControlIDStart,kDynamicControlIDEnd);
	mMode = kFileListMode_RecentlyOpened;
	
	//#361
	//�A�N�Z�X�v���O�C���ڑ��I�u�W�F�N�g����
	mRemoteConnectionObjectID = GetApp().SPI_CreateRemoteConnection(kRemoteConnectionType_FileListWindow,GetObjectID());
	
	//#725 �^�C�}�[�Ώۂɂ���
	NVI_AddToTimerActionWindowArray();
}

//�f�X�g���N�^
AWindow_FileList::~AWindow_FileList()
{
	//#361
	//�����[�g�ڑ��I�u�W�F�N�g�폜
	GetApp().SPI_DeleteRemoteConnection(mRemoteConnectionObjectID);
	mRemoteConnectionObjectID = kObjectID_Invalid;
}

/**
�E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾
*/
ANumber	AWindow_FileList::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 20 + kHeight_FilterTopBottomMargin + kHeight_Header2Lines + 10;
}

//#725
/**
SubWindow�p�^�C�g���o�[�擾
*/
AView_SubWindowTitlebar&	AWindow_FileList::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
���j���[�I��������
*/
ABool	AWindow_FileList::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
		//�N���[�Y
	  case kMenuItemID_Close:
		{
			//�t���[�e�B���O�E�C���h�E�̏ꍇ�̂݃E�C���h�E�N���[�Y����
			if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
			{
				GetApp().SPI_CloseSubWindow(GetObjectID());
			}
			result = true;
			break;
		}
		//�����v���W�F�N�g���̃t�B���^�F�S��
	  case kMenuItemID_FileList_ProjectModeFileter_AllFiles:
		{
			mProjectFilterMode = kFileListProjectFilter_AllFiles;
			SPI_UpdateTable();
			NVI_UpdateProperty();
			result = true;
			break;
		}
		//�����v���W�F�N�g���̃t�B���^�F�������[�h
	  case kMenuItemID_FileList_ProjectModeFileter_SameMode:
		{
			mProjectFilterMode = kFileListProjectFilter_SameModeFiles;
			SPI_UpdateTable();
			NVI_UpdateProperty();
			result = true;
			break;
		}
		//�����v���W�F�N�g���̃t�B���^�F�����̂���t�@�C��
	  case kMenuItemID_FileList_ProjectModeFileter_DiffFiles:
		{
			mProjectFilterMode = kFileListProjectFilter_DiffFiles;
			SPI_UpdateTable();
			NVI_UpdateProperty();
			result = true;
			break;
		}
		//�s���ݒ�
	  case kMenuItemID_FixRecentlyOpenedItem:
		{
			//#394 �s��������
			result = true;
			break;
		}
		//�ŋߊJ�����t�@�C���̍��ڂ��폜
	  case kMenuItemID_CM_ClearItemFromRecentOpen:
		{
			GetApp().SPI_DeleteRecentlyOpenedFileItem(SPI_GetContextMenuSelectedRowDBIndex());
			break;
		}
		//������Ȃ����ڂ𗚗�����폜 #1301
	  case kMenuItemID_CM_ClearNotFoundItemsFromRecentOpen:
		{
			GetApp().SPI_DeleteRecentlyOpenedFileNotFound();
			break;
		}
		//���̑���menu item ID�̏ꍇ�A���̃N���X�ŏ��������A���̃R�}���h�Ώۂŏ�������
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

/**
���j���[�X�V
*/
void	AWindow_FileList::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

//�R���g���[���̃N���b�N���̃R�[���o�b�N
ABool	AWindow_FileList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	if( inID == mListViewControlID )
	{
		AIndex	index = GetListView().SPI_GetSelectedDBIndex();
		switch(mMode)
		{
			//==================�����[�g�t�@�C��==================
		  case kFileListMode_RemoteFile:
			{
				/*
				//�����[�g�ڑ��I�u�W�F�N�g�։��炩�̗v�����ɂ͉���������break
				if( GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).SPI_GetCurrentRequestType() != kRemoteConnectionRequestType_None )
				{
					break;
				}
				*/
				if( index != kIndex_Invalid )
				{
					//�A�J�E���g���X�g�\���� or �A�J�E���g���̃f�B���N�g���\�����̕���
					if( mRemoteConnectionCurrentLevel == 0 )
					{
						//================�����[�g�A�J�E���g���X�g�\�����N���b�N��================
						
						//�I������
						GetListView().SPI_SetSelect(kIndex_Invalid);
						
						//���x���C���N�������g
						mRemoteConnectionCurrentLevel_Request = mRemoteConnectionCurrentLevel +1;
						
						//�A�J�E���g�f�[�^��DB����擾
						AText	server, user, path;
						GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_Server,index,server);
						GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_User,index,user);
						GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_RootPath,index,path);
						/*#1231 �|�[�g�ԍ���FTP�A�J�E���g�ݒ肩��擾
						ANumber	portNumber = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFTPAccount_Port,index);//#193
						if( GetApp().NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kFTPAccount_UseDefaultPort,index) == true )
						{
							portNumber = 0;
						}
						*/
						//�ڑ��^�C�v�i�v���O�C���j��DB����擾
						AText	connectionType;
						GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_ConnectionType,index,connectionType);//#1231
						//
						mRemoteConnectionRootFolderURL_Request.SetFTPURL(connectionType,server,user,path,0);//#1231 portNumber);//#193
						mRemoteConnectionCurrentFolderURL_Request.SetText(mRemoteConnectionRootFolderURL_Request);
						mRemoteConnectionCurrentFolderURL_Request.AddLastPathDelimiter();
						
						/*#1231
						//�ڑ��^�C�v�ݒ� #193
						if( GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).SetConnectionType(mRemoteConnectionConnectionType_Request) == true )
						{
						*/
						//�����[�g�ڑ��I�u�W�F�N�g��LIST���s�v��
						GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
								ExecuteLIST(kRemoteConnectionRequestType_LISTfromFileListWindow,mRemoteConnectionCurrentFolderURL_Request,GetEmptyText());
						//�v���O���X�\�� #1236
						ShowRemoteAccessProgress(true);
						
						//��LIST������M���ASPI_SetRemoteConnectionResult()�������[�g�ڑ��I�u�W�F�N�g����R�[������A
						//�����Ń��X�g�X�V�����B
						/*#1231
						}
						else
						{
							//���v���O�C���������ꍇ
							
						}
						*/
					}
					else
					{
						//================�����[�g�t�@�C�����X�g�\�����N���b�N��================
						
						if( index == 0 )
						{
							//==================�e�t�H���_�Ɉړ�==================
							
							//�I������
							GetListView().SPI_SetSelect(kIndex_Invalid);
							
							//���x���ɂ�镪��
							if( mRemoteConnectionCurrentLevel <= 1 )
							{
								//�����[�g�A�J�E���g���X�g�\����Ԃɖ߂�
								mRemoteConnectionCurrentLevel = 0;
								//����URL������
								mRemoteConnectionCurrentFolderURL.DeleteAll();
								mRemoteConnectionRootFolderURL.DeleteAll();
								//#1231 mRemoteConnectionConnectionType.DeleteAll();
								//�e�[�u���X�V
								SPI_UpdateTable();
							}
							else
							{
								//���x���f�N�������g
								mRemoteConnectionCurrentLevel_Request = mRemoteConnectionCurrentLevel-1;
								//����URL��eURL�ֈړ�
								AText	parentURL;
								mRemoteConnectionCurrentFolderURL.GetParentURL(parentURL);
								mRemoteConnectionCurrentFolderURL_Request.SetText(parentURL);
								mRemoteConnectionCurrentFolderURL_Request.AddLastPathDelimiter();
								
								//�����[�g�ڑ��I�u�W�F�N�g��LIST���s�v��
								GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
										ExecuteLIST(kRemoteConnectionRequestType_LISTfromFileListWindow,
													mRemoteConnectionCurrentFolderURL_Request,GetEmptyText());
								//�v���O���X�\�� #1236
								ShowRemoteAccessProgress(true);
								
								//��LIST������M���ASPI_SetRemoteConnectionResult()�������[�g�ڑ��I�u�W�F�N�g����R�[������A
								//�����Ń��X�g�X�V�����B
							}
						}
						else if( index == 1 )
						{
							//==================�V�K�t�@�C������==================
							//�V�K�t�@�C�������_�C�A���O
							GetApp().SPI_GetNewFTPFileWindow().NVI_CreateAndShow();
							GetApp().SPI_GetNewFTPFileWindow().SPI_SetFolder(mRemoteConnectionCurrentFolderURL);
							GetApp().SPI_GetNewFTPFileWindow().NVI_SwitchFocusToFirst();
							//���[�_���J�n
							GetApp().SPI_GetNewFTPFileWindow().NVI_RunModalWindow();
							//�E�C���h�E�����
							GetApp().SPI_GetNewFTPFileWindow().NVI_Close();
						}
						else
						{
							//==================�t�@�C���܂��̓t�H���_�I����==================
							//�t�@�C���^�t�H���_���擾
							AText	filename;
							mRemoteConnectionFilenameArray.Get(index,filename);
							
							//�t�@�C���^�t�H���_URL�擾
							AText	fileURL;
							fileURL.SetText(mRemoteConnectionCurrentFolderURL);
							fileURL.AddPathDelimiter('/');
							fileURL.AddText(filename);
							
							//�t�@�C���^�t�H���_����
							if( mRemoteConnectionIsDiractoryArray.Get(index) == false )
							{
								//==================�t�@�C��==================
								//�I������
								GetListView().SPI_SetSelect(kIndex_Invalid);
								//�����[�g�t�@�C���h�L�������g���J���v��
								GetApp().SPI_RequestOpenFileToRemoteConnection(/*#1231 mRemoteConnectionConnectionType,*/fileURL,false);
							}
							else
							{
								//==================�t�H���_==================
								//���x���C���N�������g
								mRemoteConnectionCurrentLevel_Request = mRemoteConnectionCurrentLevel +1;
								
								//URL�X�V
								mRemoteConnectionCurrentFolderURL_Request.SetText(fileURL);
								mRemoteConnectionCurrentFolderURL_Request.AddLastPathDelimiter();
								
								//�����[�g�ڑ��I�u�W�F�N�g��LIST���s�v��
								GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
										ExecuteLIST(kRemoteConnectionRequestType_LISTfromFileListWindow,
													mRemoteConnectionCurrentFolderURL_Request,GetEmptyText());
								//�v���O���X�\�� #1236
								ShowRemoteAccessProgress(true);
							}
						}
					}
				}
				//�I�����ڍX�V
				mSelectionIndex_RemoteConnection = index;
				break;
			}
			//==================�ŋߊJ�����t�@�C��==================
		  case kFileListMode_RecentlyOpened:
			{
				if( index != kIndex_Invalid )
				{
					//���݂̈ʒu��Navigate�o�^���� #146
					GetApp().SPI_RegisterNavigationPosition();
					//
					AText	text;
					GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFile).Get(index,text);
					//#235
					AIndex	p = 0;
					if( text.FindCstring(0,"://",p) == true )
					{
						//------------------�����[�g�t�@�C��------------------
						//"://"���܂�ł���Ȃ�A�����[�g�t�@�C���Ƃ��āA�h�L�������g���J��
						//#361 GetApp().SPI_StartOpenFTPDocument(text,false);
						GetApp().SPI_RequestOpenFileToRemoteConnection(text,false);//#361
					}
					else
					{
						//------------------���[�J���t�@�C��------------------
						//B0389 OK GetApp().SPNVI_CreateDocumentByPathForDisplay(text);
						AFileAcc	file;
						file.Specify(text);//#910 ,kFilePathType_1);//B0389 �����̓f�[�^�݊����̂��ߋ��^�C�v�̂܂܁B
						//B0421 GetApp().SPNVI_CreateDocumentFromLocalFile(file);
						GetApp().GetAppPref().LaunchAppWithFile(file,inModifierKeys);//B0421
					}
				}
				//�I�����ڍX�V
				mSelectionIndex_Recent = index;
				break;
			}
			//==================�����t�H���_==================
		  case kFileListMode_SameFolder:
			{
				if( index != kIndex_Invalid )
				{
					//���݂̈ʒu��Navigate�o�^���� #146
					GetApp().SPI_RegisterNavigationPosition();
					//
					AFileAcc	file;
					GetApp().SPI_GetSameFolderFile(index,file);
					//B0421 GetApp().SPNVI_CreateDocumentFromLocalFile(file);
					GetApp().GetAppPref().LaunchAppWithFile(file,inModifierKeys);//B0421
				}
				//�I�����ڍX�V
				mSelectionIndex_SameFolder = index;
				break;
			}
			//==================�����v���W�F�N�g==================
		  case kFileListMode_SameProject:
			{
				if( index != kIndex_Invalid )
				{
					//���݂̈ʒu��Navigate�o�^���� #146
					GetApp().SPI_RegisterNavigationPosition();
					//
					AFileAcc	file;
					file.Specify(mActionTextArray.GetTextConst(index));//#892
					GetApp().GetAppPref().LaunchAppWithFile(file,inModifierKeys);//B0421
				}
				//�I�����ڍX�V
				mSelectionIndex_SameProject = index;
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
	switch(inID)
	{
	  case kButton_ModeChange:
	  case kControlID_TitleBar://#725
		{
			switch(mMode)
			{
			  case kFileListMode_RemoteFile:
				{
					SPI_ChangeMode(kFileListMode_RecentlyOpened);
					break;
				}
			  case kFileListMode_RecentlyOpened:
				{
					SPI_ChangeMode(kFileListMode_SameFolder);
					break;
				}
			  case kFileListMode_SameFolder:
				{
					SPI_ChangeMode(kFileListMode_SameProject);
					break;
				}
			  case kFileListMode_SameProject:
				{
					SPI_ChangeMode(kFileListMode_RemoteFile);
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			GetListView().SPI_AdjustScroll();
			result = true;
			break;
		}
	  case kButton_Sort:
		{
			mRecentSortMode = NVI_GetButtonViewByControlID(kButton_Sort).SPI_GetToggleOn();//#232 NVI_GetControlBool(kButton_Sort);
			if( mRecentSortMode == true )
			{
				GetListView().SPI_Sort(kColumn_FileName,true);
			}
			else
			{
				GetListView().SPI_SortOff();
			}
			result = true;
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

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow_FileList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

//#291
void	AWindow_FileList::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}

//#798
/**
�R���g���[����text���͎�����
*/
void	AWindow_FileList::EVTDO_TextInputted( const AControlID inID )
{
	//�t�B���^�œ��͎�
	if( inID == kControlID_Filter )
	{
		//�t�B���^�e�L�X�g�擾
		AText	filter;
		NVI_GetControlText(kControlID_Filter,filter);
		filter.ChangeCaseLowerFast();
		
		//list view�Ƀt�B���^�ݒ�
		GetListView().SPI_SetFilterText(kColumn_FileName,filter);
	}
}

//#798
/**
�R���g���[���Ń��^�[���L�[����������
*/
ABool	AWindow_FileList::NVIDO_ViewReturnKeyPressed( const AControlID inControlID )
{
	//�t�B���^�œ��͎�
	if( inControlID == kControlID_Filter )
	{
		//list view�ł̃N���b�N���Ɠ������������s
		EVTDO_Clicked(mListViewControlID,kModifierKeysMask_None);
		
		//�t�B���^�e�L�X�g����ɂ���
		NVI_SetControlText(kControlID_Filter,GetEmptyText());
		
		//list view�Ƀt�B���^�ݒ�
		GetListView().SPI_SetFilterText(0,GetEmptyText());
	}
	return true;
}

//#798
/**
�R���g���[����ESC�L�[����������
*/
ABool	AWindow_FileList::NVIDO_ViewEscapeKeyPressed( const AControlID inControlID )
{
	//�t�B���^�œ��͎�
	if( inControlID == kControlID_Filter )
	{
		//�t�B���^�e�L�X�g����ɂ���
		NVI_SetControlText(kControlID_Filter,GetEmptyText());
		
		//list view�Ƀt�B���^�ݒ�
		GetListView().SPI_SetFilterText(0,GetEmptyText());
		
		//�e�L�X�g�E�C���h�E�փt�H�[�J�X�ړ�
		if( NVM_GetOverlayMode() == true )
		{
			GetApp().NVI_GetWindowByID(NVI_GetOverlayParentWindowID()).NVI_SelectWindow();
		}
		else
		{
			AWindowID	frontWinID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( frontWinID != kObjectID_Invalid )
			{
				GetApp().NVI_GetWindowByID(frontWinID).NVI_SelectWindow();
			}
		}
	}
	return true;
}

//#798
/**
�R���g���[���ŏ�L�[����������
*/
ABool	AWindow_FileList::NVIDO_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//�t�B���^�œ��͎�
	if( inControlID == kControlID_Filter )
	{
		//list view�őO��I��
		GetListView().SPI_SetSelectPreviousDisplayRow(inModifers);
	}
	return true;
}

//#798
/**
�R���g���[���ŉ��L�[����������
*/
ABool	AWindow_FileList::NVIDO_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//�t�B���^�œ��͎�
	if( inControlID == kControlID_Filter )
	{
		//list view�Ŏ���I��
		GetListView().SPI_SetSelectNextDisplayRow(inModifers);
	}
	return true;
}

//#892
/**
���X�g�Ő܂肽���݁E�W�J���s�㏈��
*/
void	AWindow_FileList::NVIDO_ListViewExpandedCollapsed( const AControlID inControlID )
{
	//�e�L�X�g�E�C���h�E�փt�H�[�J�X�ړ�
	if( NVM_GetOverlayMode() == true )
	{
		GetApp().NVI_GetWindowByID(NVI_GetOverlayParentWindowID()).NVI_SelectWindow();
	}
	else
	{
		AWindowID	frontWinID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
		if( frontWinID != kObjectID_Invalid )
		{
			GetApp().NVI_GetWindowByID(frontWinID).NVI_SelectWindow();
		}
	}
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_FileList::NVIDO_Create( const ADocumentID inDocumentID )
{
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	
	//�E�C���h�E����
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//�T�C�h�o�[
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Bottom,false,false,false,false,false,false,false);
			break;
		}
		//�t���[�e�B���O
	  default:
		{
			NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_None,false,false,false,false,false,true,false);
			break;
		}
	}
	
	//���[�h�̕���
	mMode = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWindowListMode);
	//�E�C���h�E���ݒ�
	if( NVM_GetOverlayMode() == false )//#291
	{
		NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWindowListSizeX));
		NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWindowListSizeY));
	}
	
	//�w�i�F�`��pview����
	{
		AViewDefaultFactory<AView_SubWindowBackground>	viewFactory(GetObjectID(),kControlID_Background);
		AWindowPoint	pt = {0,0};
		NVM_CreateView(kControlID_Background,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Background,true);
		NVI_SetControlBindings(kControlID_Background,true,true,true,true,false,false);
	}
	
	//�w�b�_view����
	{
		AViewDefaultFactory<AView_SubWindowHeader>	viewFactory(GetObjectID(),kControlID_Header);
		AWindowPoint	pt = {0,0};
		NVM_CreateView(kControlID_Header,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Header,true);
		NVI_SetControlBindings(kControlID_Header,true,true,true,false,false,true);
	}
	
	//�t�H���_�[�p�X�{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_FolderPath,pt,10,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_FolderPath,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_FolderPath).SPI_SetButtonViewType(kButtonViewType_TextWithNoHoverWithFixedWidth);
		NVI_GetButtonViewByControlID(kControlID_FolderPath).SPI_SetEllipsisMode(kEllipsisMode_FixedLastCharacters,10);
	}
	
	//�v���W�F�N�g���[�h�t�B���^�{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_ProjectModeFilter,pt,10,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_ProjectModeFilter,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
		NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).SPI_SetEllipsisMode(kEllipsisMode_FixedFirstCharacters,10);
		//
		NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).SPI_SetMenu(kContextMenuID_SameProjectHeader,kMenuItemID_Invalid);
	}
	
	//SubWindow�p�^�C�g���o�[����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("FileListWindow_RecentFile");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwList);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//�����{�b�N�X�𐶐�
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_Filter,pt,100,13,kControlID_Background,kIndex_Invalid,true,false,false,true,kEditBoxType_FilterBox);
		NVI_GetEditBoxView(kControlID_Filter).NVI_SetTextFont(fontname,fontsize);
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetTextColor(
					AColorWrapper::GetColorByHTMLFormatColor("1574cf"),
					AColorWrapper::GetColorByHTMLFormatColor("1574cf"));
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetBackgroundColor(
					kColor_White,kColor_Gray70Percent,kColor_Gray70Percent);
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetEnableFocusRing(false);
		NVI_SetAutomaticSelectBySwitchFocus(kControlID_Filter,true);
		NVI_SetControlBindings(kControlID_Filter,true,true,true,false,false,true);
	}
	//SizeBox����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
	
	//�v���O���Xindicator #1236
	{
		//progress indicator����
		AWindowPoint	pt = {16,64};
		NVI_CreateProgressIndicator(kControlID_RemoteAccessProgressIndicator,pt,kWidth_ProgressIndicator,kHeight_ProgressIndicator);
		NVI_SetControlBindings(kControlID_RemoteAccessProgressIndicator,true,true,false,false,true,true);
		ShowRemoteAccessProgress(false);
	}
}

//�V�K�^�u����
//�����F��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
void	AWindow_FileList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	AWindowPoint	pt = {0,0};
	//ListView����
	mListViewControlID = NVM_AssignDynamicControlID();
	AListViewFactory	listViewFactory(/* #199 this,*this*/GetObjectID(),mListViewControlID);
	NVM_CreateView(mListViewControlID,pt,10,10,kControlID_Background,kControlID_Invalid,false,listViewFactory);
	/*#725
	if( NVM_GetOverlayMode() == true )//#291
	{
		NVI_GetListView(mListViewControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetListView(mListViewControlID).SPI_SetAlwaysActiveColors(true);
	}
	*/
	NVI_GetListView(mListViewControlID).SPI_SetAlwaysActiveColors(true);
	NVI_GetListView(mListViewControlID).SPI_SetBackgroundColor(
				GetApp().SPI_GetSubWindowBackgroundColor(true),
				GetApp().SPI_GetSubWindowBackgroundColor(false));
	//#725
	NVI_GetListView(mListViewControlID).SPI_SetEnableImageIcon(true);
	//#291 �T�C�Ybinding�ݒ�i��؂���ύX���̂�����h�~�j
	NVI_SetControlBindings(mListViewControlID,true,true,true,true,false,false);
	//#328 HelpTag
	NVI_EnableHelpTagCallback(mListViewControlID);
	
	//V�X�N���[���o�[����
	NVI_CreateScrollBar(kControlID_VScrollBar,pt,kVScrollBarWidth,kVScrollBarWidth*2);
	GetListView().NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_VScrollBar);
	//#291 �T�C�Ybinding�ݒ�i��؂���ύX���̂�����h�~�j
	NVI_SetControlBindings(kControlID_VScrollBar,false,true,true,true,true,false);
	
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);
	
	//SubWindow�p�Z�p���[�^����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,mListViewControlID,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	/*#725
	//���[�h�ύX�{�^��
	NVI_CreateButtonView(kButton_ModeChange,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);
	if( NVM_GetOverlayMode() == true )//#291
	{
		NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetAlwaysActiveColors(true);
	}
	NVI_EnableHelpTagCallback(kButton_ModeChange);
	//#291 �T�C�Ybinding�ݒ�i��؂���ύX���̂�����h�~�j
	NVI_SetControlBindings(kButton_ModeChange,true,true,true,false,false,true);
	//
	NVI_EmbedControl(kControlID_HeaderBar,kButton_ModeChange);
	
	//�\�[�g�{�^��
	NVI_CreateButtonView(kButton_Sort,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);
	AText	t;
	t.SetLocalizedText("Sort");
	NVI_GetButtonViewByControlID(kButton_Sort).NVI_SetText(t);
	NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetToggleMode(true);
	if( NVM_GetOverlayMode() == true )//#291
	{
		NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetAlwaysActiveColors(true);
	}
	//#291 �T�C�Ybinding�ݒ�i��؂���ύX���̂�����h�~�j
	NVI_SetControlBindings(kButton_Sort,false,true,true,false,true,true);
	//
	NVI_EmbedControl(kControlID_HeaderBar,kButton_Sort);
	*/
	
	//view bounds�X�V
	UpdateViewBounds();
	
	GetListView().SPI_RegisterColumn_Text(kColumn_FileName,0,"",false);
	GetListView().SPI_SetEnableCursorRow();
	//GetListView().SPI_SetRowMargin(3);
	
	SPI_UpdateTable();
	
	//win
	NVI_UpdateProperty();
	
	//
	outInitialFocusControlID = mListViewControlID;
}

//#798
/**
Show()�h������
*/
void	AWindow_FileList::NVIDO_Show()
{
	//�t�@�C�����X�g�E�C���h�E�\�����ɁA���݂̍őO�ʃh�L�������g�ɏ]���ăe�[�u���ݒ肷��i���ꂪ�Ȃ��ƁA�V�K�Ƀt�@�C�����X�g��\���������ɁA���[�h���ݒ肳��Ȃ��j
	ADocumentID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID != kObjectID_Invalid )
	{
		SPI_Update_DocumentActivated(docID);
	}
}

//Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
void	AWindow_FileList::NVIDO_Hide()
{
	//�܂肽���݃f�[�^��DB�ɕۑ�
	SaveCollapseData();
	//
	if( NVM_GetOverlayMode() == false )//#291 Overlay���[�h���ɂ͕ۑ����Ȃ�
	{
		//�E�C���h�E���̕ۑ�
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kWindowListSizeX,NVI_GetWindowWidth());
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kWindowListSizeY,NVI_GetWindowHeight());
	}
	//���[�h�̕ۑ�
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kWindowListMode,static_cast<ANumber>(mMode));
}

//�e��R���g���[���̏�Ԃ��A�b�v�f�[�g����
void	AWindow_FileList::NVMDO_UpdateControlStatus()
{
	switch(mMode)
	{
	  case kFileListMode_RecentlyOpened:
		{
			//�t�H���_�p�X�X�V
			AText	text;
			NVI_GetViewByControlID(kControlID_FolderPath).NVI_SetText(text);
			break;
		}
	  case kFileListMode_SameFolder:
		{
			//�t�H���_�p�X�X�V
			AText	text;
			GetApp().SPI_GetCurrentFolderPathForDisplay(text);
			NVI_GetViewByControlID(kControlID_FolderPath).NVI_SetText(text);
			break;
		}
	  case kFileListMode_RemoteFile:
		{
			//�t�H���_�p�X�X�V
			AText	text;
			text.SetText(mRemoteConnectionCurrentFolderURL);
			if( text.GetItemCount() == 0 )
			{
				//�u�A�J�E���g��I���v��\��
				text.SetLocalizedText("FileList_Remote_SelectServer");
			}
			NVI_GetViewByControlID(kControlID_FolderPath).NVI_SetText(text);
			break;
		}
	  case kFileListMode_SameProject:
		{
			//�t�H���_�p�X�X�V
			AText	text;
			GetApp().SPI_GetCurrentProjectPathForDisplay(text);
			NVI_GetViewByControlID(kControlID_FolderPath).NVI_SetText(text);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
#if 0
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);//#375
	AText	text;
	switch(mMode)
	{
	  case kFileListMode_RecentlyOpened:
		{
			//#232 NVI_SetControlIcon(kButton_ModeChange,kIconID_Mi_FileList_RecentlyOpened,true);//win 080721
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetIcon(kIconID_Mi_FileList_RecentlyOpened,3,3);//#232
			text.SetLocalizedText("FileListWindow_RecentFile");
			//#232 NVI_SetControlText(kButton_ModeChange,text);
			//#232 NVI_SetControlTextJustification(kButton_ModeChange,kJustification_Left);//win 080721
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);//#232
			NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetText(text);//#232
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetHelpTag(text,text);//#661
			break;
		}
	  case kFileListMode_SameFolder:
		{
			//#232 NVI_SetControlIcon(kButton_ModeChange,kIconID_Mi_FileList_SameFolder,true);//win 080721
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetIcon(kIconID_Mi_FileList_SameFolder,3,3);//#232
			GetApp().SPI_GetCurrentFolderPathForDisplay(text);
			if( text.GetLength() == 0 )
			{
				text.SetLocalizedText("FileListWindow_SameFolder");
			}
			/*#232
			NVI_SetControlText(kButton_ModeChange,text);
			NVI_SetControlTextJustification(kButton_ModeChange,kJustification_Right);//win 080721
			*/
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_CenterTruncated);//#232 #315
			NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetText(text);//#232
			//#661
			AText	helpTagText;
			helpTagText.SetLocalizedText("HelpTag_FileListSameFolder");
			helpTagText.ReplaceParamText('0',text);
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetHelpTag(helpTagText,helpTagText);
			break;
		}
	  case kFileListMode_RemoteFile:
		{
			//#232 NVI_SetControlIcon(kButton_ModeChange,kIconID_Mi_FileList_Window,true);//win 080721
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetIcon(kIconID_Mi_FileList_Window,3,3);//#232
			text.SetLocalizedText("FileListWindow_Window");
			/*#232
			NVI_SetControlText(kButton_ModeChange,text);
			NVI_SetControlTextJustification(kButton_ModeChange,kJustification_Left);//win 080721
			*/
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);//#232
			NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetText(text);//#232
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetHelpTag(text,text);//#661
			break;
		}
	  case kFileListMode_SameProject:
		{
			//#232 NVI_SetControlIcon(kButton_ModeChange,kIconID_Mi_FileList_Project,true);//win 080721
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetIcon(kIconID_Mi_FileList_Project,3,3);//#232
			GetApp().SPI_GetCurrentProjectPathForDisplay(text);
			//#661
			AText	helpTagText;
			helpTagText.SetLocalizedText("HelpTag_FileListSameProject");
			//B0403
			if( text.GetItemCount() == 0 )
			{
				text.SetLocalizedText("FileListWindow_ProjectFolderNotDefined");
				helpTagText.SetLocalizedText("HelpTag_FileListSameProject_NotDefined");//#661
			}
			else
			{
				helpTagText.ReplaceParamText('0',text);
			}
			/*#232
			NVI_SetControlText(kButton_ModeChange,text);
			NVI_SetControlTextJustification(kButton_ModeChange,kJustification_Right);//win 080721
			*/
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_CenterTruncated);//#232 #315
			NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetText(text);//#232
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetHelpTag(helpTagText,helpTagText);//#661
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
#endif
	//#725
	//�^�C�g���e�L�X�g�X�V
	UpdateTitleText();
}

void	AWindow_FileList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	/*#725
	AText	fontname;//win
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kFileListFontName,fontname);//win
	GetListView().SPI_SetTextDrawProperty(
			fontname,GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kFileListFontSize),kColor_Black);
	*/
	 GetListView().SPI_SetWheelScrollPercent(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent),
				GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd));//R0000
	/*#725
	if( NVM_GetOverlayMode() == false )//#291
	{
		NVI_SetWindowTransparency((static_cast<AFloatNumber>(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWindowListAlphaPercent)))/100);
	}
	else
	{
		//Overlay���͏��y�C��Overlay�����x��K�p�i�������AMacOSX�ł͎��ۂɂ͉������Ȃ��BView���œh��Ԃ����ɓ����x�K�p�B�j
		NVI_SetWindowTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
	}
	*/
	//�T�u�E�C���h�E�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//Filter�̃t�H���g�ݒ�
	NVI_GetEditBoxView(kControlID_Filter).NVI_SetTextFont(fontname,fontsize);
	AText	filtertext;
	filtertext.SetLocalizedText("Filter");
	//Filter�ɕ\������V���[�g�J�b�g�L�[�o�C���h���擾
	AText	shortcuttext;
	switch(mMode)
	{
	  case kFileListMode_SameProject:
		{
			GetApp().NVI_GetMenuManager().GetMenuItemShortcutDisplayText(kMenuItemID_SearchInProjectFolder,shortcuttext);
			break;
		}
	  case kFileListMode_SameFolder:
		{
			GetApp().NVI_GetMenuManager().GetMenuItemShortcutDisplayText(kMenuItemID_SearchInSameFolder,shortcuttext);
			break;
		}
	  case kFileListMode_RecentlyOpened:
		{
			GetApp().NVI_GetMenuManager().GetMenuItemShortcutDisplayText(kMenuItemID_SearchInRecentlyOpenFiles,shortcuttext);
			break;
		}
	  default:
		{
			//��������
			break;
		}
	}
	//
	NVI_GetEditBoxView(kControlID_Filter).SPI_SetTextForEmptyState(filtertext,shortcuttext);
	//list view�̃t�H���g�ݒ�
	GetListView().SPI_SetTextDrawProperty(fontname,fontsize,GetApp().SPI_GetSubWindowLetterColor());
	//�z�C�[���X�N���[�����ݒ�
	GetListView().SPI_SetWheelScrollPercent(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent),
				GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd));
	//Filter�ւ̃��ݒ�
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		GetListView().SPI_SetTransparency(GetApp().SPI_GetFloatingWindowAlpha());
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetTransparency(GetApp().SPI_GetFloatingWindowAlpha());
	}
	else
	{
		GetListView().SPI_SetTransparency(1.0);
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetTransparency(1.0);
	}
	//�u�����v���W�F�N�g�v�̃t�B���^�[���[�h�I���{�^���̃e�L�X�g�ݒ�
	switch(mProjectFilterMode)
	{
	  case kFileListProjectFilter_SameModeFiles:
		{
			AText	text;
			text.SetLocalizedText("FileListHeader_ProjectFilter_SameMode");
			AText	modename;
			if( mCurrentModeIndex != kIndex_Invalid )
			{
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetModeDisplayName(modename);
			}
			text.ReplaceParamText('0',modename);
			NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).NVI_SetText(text);
			break;
		}
	  case kFileListProjectFilter_AllFiles:
		{
			AText	text;
			text.SetLocalizedText("FileListHeader_ProjectFilter_AllFiles");
			NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).NVI_SetText(text);
			break;
		}
	  case kFileListProjectFilter_DiffFiles:
		{
			AText	text;
			text.SetLocalizedText("FileListHeader_ProjectFilter_DiffFiles");
			NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).NVI_SetText(text);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	NVI_GetEditBoxView(kControlID_Filter).SPI_SetBackgroundColorForEmptyState(
																			  GetApp().SPI_GetSubWindowBackgroundColor(true),
																			  GetApp().SPI_GetSubWindowBackgroundColor(false));
	
	//�w�b�_�����t�H���g
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	//�w�b�_�F�擾
	AColor	headerlettercolor = GetApp().SPI_GetSubWindowHeaderLetterColor();
	//�w�b�_������{�^���F�ݒ�
	NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,
																				   kTextStyle_Bold,headerlettercolor,headerlettercolor);
	NVI_GetButtonViewByControlID(kControlID_FolderPath).SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,
																			kTextStyle_Bold,headerlettercolor,headerlettercolor);
	
	//view bounds�X�V
	UpdateViewBounds();
	//�E�C���h�Erefresh
	NVI_RefreshWindow();
}

//#798
/**
Filter�{�b�N�X�Ƀt�H�[�J�X�ړ�
*/
void	AWindow_FileList::SPI_SwitchFocusToSearchBox()
{
	NVI_SelectWindow();
	NVI_SwitchFocusTo(kControlID_Filter);
	NVI_GetEditBoxView(kControlID_Filter).NVI_SetSelectAll();
}


#pragma mark ===========================

#pragma mark ---�\���X�V

//#725
/**
�t�@�C�����X�g�e��X�V
*/
void	AWindow_FileList::SPI_UpdateFileList( const AFileListUpdateType inType, const ADocumentID inDocumentID )
{
	switch(inType)
	{
		//�ŋߊJ�����t�@�C���f�[�^�̍X�V��
	  case kFileListUpdateType_RecentlyOpenedFileUpdated:
		{
			SPI_Update_RecentlyOpendFileUpdated();
			break;
		}
		//�v���W�F�N�g�t�H���_�ǂݍ��݊�����
	  case kFileListUpdateType_ProjectFolderLoaded:
		{
			if( mMode == kFileListMode_SameProject )
			{
				SPI_UpdateTable();
			}
			break;
		}
		//�����t�H���_�f�[�^�̍X�V��
	  case kFileListUpdateType_SameFolderDataUpdated:
		{
			if( mMode == kFileListMode_SameFolder )
			{
				SPI_UpdateTable();
			}
			break;
		}
		//�����v���W�F�N�g�f�[�^�̍X�V��
	  case kFileListUpdateType_SameProjectDataUpdated:
		{
			if( mMode == kFileListMode_SameProject )
			{
				SPI_UpdateTable();
			}
			break;
		}
		//SCM�f�[�^�̍X�V��
	  case kFileListUpdateType_SCMDataUpdated:
		{
			SPI_UpdateTable();
			break;
		}
		//�h�L�������gactivate��
	  case kFileListUpdateType_DocumentActivated:
		{
			SPI_Update_DocumentActivated(inDocumentID);
			break;
		}
		//�h�L�������gopen��
	  case kFileListUpdateType_DocumentOpened:
		{
			SPI_Update_DocumentOpened(inDocumentID);
			break;
		}
		//�h�L�������gclose��
	  case kFileListUpdateType_DocumentClosed:
		{
			break;
		}
		//�����[�g�t�@�C���A�J�E���g�ݒ�ύX�� #193
	  case kFileListUpdateType_RemoteFileAccountDataChanged:
		{
			SPI_UpdateTable();
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

void	AWindow_FileList::SPI_UpdateTable() 
{
	if( NVI_IsWindowCreated() == false )   return;
	
	//�ʏ핶���F�擾
	AColor	subwindowNormalColor = GetApp().SPI_GetSubWindowLetterColor();
	
	AIndex	sel = kIndex_Invalid;
	GetListView().SPI_BeginSetTable();
	switch(mMode)
	{
		//==================�ŋߊJ�����t�@�C��==================
	  case kFileListMode_RecentlyOpened:
		{
			GetListView().SPI_SetColumn_Text(kColumn_FileName,GetApp().SPI_GetRecentlyOpenedFileDisplayTextArray());
			//#725
			//�S�Ă̍��ڂɃh�L�������g�A�C�R����ݒ�
			AArray<AImageID>	imageIconIDArray;
			AArray<AColor>	colorarray;
			AItemCount	itemCount = GetApp().SPI_GetRecentlyOpenedFileDisplayTextArray().GetItemCount();
			for( AIndex i = 0; i < itemCount; i++ )
			{
				colorarray.Add(subwindowNormalColor);
				imageIconIDArray.Add(kImageID_iconSwlDocument);
			}
			GetListView().SPI_SetTable_ImageIconID(imageIconIDArray);
			GetListView().SPI_SetTable_Color(colorarray);
			//�ŋߊJ�����t�@�C���p�̃R���e�L�X�g���j���[�ݒ�
			GetListView().SPI_SetEnableContextMenu(true,kContextMenuID_RecentOpen);//R0186
			sel = mSelectionIndex_Recent;
			break;
		}
		//==================�����t�H���_==================
	  case kFileListMode_SameFolder:
		{
			ATextArray	samefolder;
			GetApp().SPI_GetSameFolderFilenameTextArray(samefolder);
			GetListView().SPI_SetColumn_Text(kColumn_FileName,samefolder);
			//#725
			//�S�Ă̍��ڂɃh�L�������g�A�C�R����ݒ�
			AArray<AImageID>	imageIconIDArray;
			AItemCount	itemCount = samefolder.GetItemCount();
			for( AIndex i = 0; i < itemCount; i++ )
			{
				imageIconIDArray.Add(kImageID_iconSwlDocument);
			}
			GetListView().SPI_SetTable_ImageIconID(imageIconIDArray);
			
			//R0006
			if(	GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true /*#844&& 
				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplaySCMStateInFileList) == true*/ )
			{
				/*AArray<ASCMFileState>	scmfilestate;
				GetApp().SPI_GetSameFolderSCMStateArray(scmfilestate);
				AArray<ATextStyle>	stylearray;
				AItemCount	count = scmfilestate.GetItemCount();
				if( count > 0 )
				{
				for( AIndex i = 0; i < count; i++ )
				{
				ATextStyle	style = kTextStyle_Normal;
				if( scmfilestate.Get(i) != kSCMFileState_None )
				{
				style = kTextStyle_Italic;
				}
				stylearray.Add(style);
				}
				}
				GetListView().SPI_SetTable_TextStyle(stylearray);*/
				AArray<ASCMFileState>	scmfilestate;
				GetApp().SPI_GetSameFolderSCMStateArray(scmfilestate);
				AArray<AColor>	colorarray;
				AArray<ATextStyle>	stylearray;//#458
				AItemCount	count = scmfilestate.GetItemCount();
				if( count > 0 )
				{
					for( AIndex i = 0; i < count; i++ )
					{
						AColor	color = subwindowNormalColor;
						ATextStyle	style = kTextStyle_Normal;//#458
						ASCMFileState	filestate = scmfilestate.Get(i);
						if( filestate == kSCMFileState_NotEntried )
						{
							//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDisplaySCMStateInFileList_NotEntriedColor,color);
							color = kColor_Gray;//#844
						}
						else if( filestate == kSCMFileState_Others )
						{
							//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDisplaySCMStateInFileList_OthersColor,color);
							color = kColor_Pink;//#844
						}
						else if( filestate != kSCMFileState_UpToDate && filestate != /*kSCMFolderType_NotSCMFolder*/kSCMFileState_NotSCMFolder )
						{
							//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDisplaySCMStateInFileList_Color,color);
							color = GetApp().SPI_GetFileListDiffColor();//#844
							style = kTextStyle_Bold;//#458
						}
						colorarray.Add(color);
						stylearray.Add(style);//#458
					}
				}
				GetListView().SPI_SetTable_Color(colorarray);
				GetListView().SPI_SetTable_TextStyle(stylearray);//#458
			}
			
			GetListView().SPI_SetEnableContextMenu(true,kContextMenuID_FileList);//R0186 #442
			sel = mSelectionIndex_SameFolder;
			break;
		}
		//#361
		//==================�����[�g�t�@�C��==================
	  case kFileListMode_RemoteFile:
		{
			if( mRemoteConnectionCurrentLevel == 0 )
			{
				//================�����[�g�t�@�C���A�J�E���g���X�g�\��================
				
				//�e�A�J�E���g���ɃT�[�o�[�����擾���AtextArray�Ɋi�[����
				ATextArray	textArray;
				AArray<AColor>	colorarray;
				AItemCount	itemCount = GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kFTPAccount_Server);
				for( AIndex i = 0; i < itemCount; i++ )
				{
					colorarray.Add(subwindowNormalColor);
					//�A�J�E���g�\�����擾
					AText	server;
					GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_Server,i,server);
					AText	text;
					GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_DisplayName,i,text);
					if( text.GetItemCount() > 0 )
					{
						text.AddCstring(" (");
						text.AddText(server);
						text.AddCstring(")");
					}
					else
					{
						text.SetText(server);
					}
					text.AddCstring(" ");
					//
					AText	existSubMenuText;
					existSubMenuText.SetLocalizedText("ExistSubMenu");
					text.AddText(existSubMenuText);
					textArray.Add(text);
				}
				//textArray���e�[�u����ɐݒ�
				GetListView().SPI_SetColumn_Text(kColumn_FileName,textArray);
				//�S�Ă̍��ڂɃt�H���_�A�C�R���ݒ�
				AArray<AImageID>	imageIconIDArray;
				for( AIndex i = 0; i < itemCount; i++ )
				{
					imageIconIDArray.Add(kImageID_iconSwlFolder);
				}
				GetListView().SPI_SetTable_ImageIconID(imageIconIDArray);
				GetListView().SPI_SetTable_Color(colorarray);
			}
			else
			{
				//================�����[�g�t�@�C�����X�g�\��================
				
				//�e�t�@�C���^�t�H���_���̃��[�v
				ATextArray	textArray;
				AArray<AColor>	colorarray;
				AArray<AImageID>	imageIconIDArray;
				AItemCount	itemCount = mRemoteConnectionFilenameArray.GetItemCount();
				for( AIndex i = 0; i < itemCount; i++ )
				{
					colorarray.Add(subwindowNormalColor);
					//�t�@�C���^�t�H���_���擾
					AText	text;
					mRemoteConnectionFilenameArray.Get(i,text);
					if( i == 0 )
					{
						//==================1�Ԗڂ̍��ځi�e�t�H���_�ֈړ��j�̏ꍇ==================
						//�t�H���_���{�E�O�p
						text.AddCstring(" ");
						AText	existSubMenuText;
						existSubMenuText.SetLocalizedText("ExistParentMenu");
						text.InsertText(0,existSubMenuText);
						//�e�t�H���_�ړ��A�C�R��
						imageIconIDArray.Add(kImageID_iconSwlParentPath);
					}
					else if( i == 1 )
					{
						//==================2�Ԗڂ̍��ځi�V�K�t�@�C�������j�̏ꍇ==================
						//�h�L�������g�A�C�R��
						imageIconIDArray.Add(kImageID_iconSwlDocument);
					}
					else if( mRemoteConnectionIsDiractoryArray.Get(i) == true )
					{
						//==================�t�H���_���ڂ̏ꍇ==================
						//�t�H���_���{�E�O�p
						text.AddCstring(" ");
						AText	existSubMenuText;
						existSubMenuText.SetLocalizedText("ExistSubMenu");
						text.AddText(existSubMenuText);
						//�t�H���_�A�C�R��
						imageIconIDArray.Add(kImageID_iconSwlFolder);
					}
					else
					{
						//==================�t�@�C�����ڂ̏ꍇ==================
						//�h�L�������g�A�C�R��
						imageIconIDArray.Add(kImageID_iconSwlDocument);
					}
					textArray.Add(text);
				}
				//�e�L�X�g�A�A�C�R���ݒ�
				GetListView().SPI_SetColumn_Text(kColumn_FileName,textArray);
				GetListView().SPI_SetTable_ImageIconID(imageIconIDArray);
				GetListView().SPI_SetTable_Color(colorarray);
			}
			//�I�����ڐݒ�
			sel = mSelectionIndex_RemoteConnection;
			break;
		}
		//==================�����v���W�F�N�g==================
	  case kFileListMode_SameProject:
		{
			//#892
			
			//SCM�g�p�ێ擾
			ABool	useSCM = (GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true /*#844&& 
					GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplaySCMStateInFileList) == true*/ );
			//�����v���W�F�N�g�̃t�@�C���p�X�A����сA���[�hindex���擾
			ATextArray	projData_FullPathArray;//#533
			AArray<AModeIndex>	projData_ModeIndexArray;//#533
			ABoolArray	projData_IsFolderArray;
			AText	projFolderPath;
			GetApp().SPI_GetCurrentSameProjectData(projFolderPath,projData_FullPathArray,projData_ModeIndexArray,projData_IsFolderArray);//#533
			//�\���f�[�^������
			mActionTextArray.DeleteAll();
			ATextArray	textArray;
			AArray<AImageID>	imageIconIDArray;
			AArray<AColor>	colorarray;
			AArray<ATextStyle>	stylearray;//#458
			AArray<AIndex>	outlinearray;
			
			//#533 ���݂̃��[�h�̍��ڂ�����\���f�[�^�ɒǉ�����B
			//�e�t�@�C���^�t�H���_���̃��[�v
			for( AIndex i = 0; i < projData_FullPathArray.GetItemCount(); i++ )
			{
				//�p�X�擾
				AText	path;
				projData_FullPathArray.Get(i,path);
				//�t�@�C���擾
				AFileAcc	file;
				file.Specify(path);
				//�v���W�F�N�g�����΃p�X�擾
				AText	partialPath;
				path.GetText(projFolderPath.GetItemCount(),path.GetItemCount()-projFolderPath.GetItemCount(),partialPath);
				//�t�@�C�����A���[�hindex�擾
				AText	filename;
				partialPath.GetFilename(filename);
				AColor	color = subwindowNormalColor;
				ATextStyle	style = kTextStyle_Normal;//#458
				AModeIndex	modeIndex = projData_ModeIndexArray.Get(i);
				ABool	isFolder = projData_IsFolderArray.Get(i);
				//SCM��Ԃɉ����������F�擾
				ASCMFileState	filestate = kSCMFileState_NotSCMFolder;
				if( useSCM == true )
				{
					filestate = GetApp().SPI_GetSCMFolderManager().GetFileState(path);//GetFileState(file);
					if( filestate == kSCMFileState_NotEntried )
					{
						color = kColor_Gray;//#844
					}
					else if( filestate == kSCMFileState_Others )
					{
						color = kColor_Pink;//#844
					}
					else if( filestate != kSCMFileState_UpToDate && filestate != kSCMFileState_NotSCMFolder )
					{
						color = kColor_DarkBlue;//#844
						style = kTextStyle_Bold;//#458
					}
				}
				//�v���W�F�N�g���t�@�C���t�B���^�[���[�h�ɏ]���āA�\���Ώۃt�@�C�����ǂ����𔻒�
				ABool	filterMatchedItem = false;
				switch(mProjectFilterMode)
				{
					//�t�B���^�F�S�Ẵt�@�C��
				  case kFileListProjectFilter_AllFiles:
					{
						filterMatchedItem = true;
						break;
					}
					//�t�B���^�F�������[�h
				  case kFileListProjectFilter_SameModeFiles:
					{
						if( modeIndex == kIndex_Invalid || modeIndex == mCurrentModeIndex )
						{
							filterMatchedItem = true;
						}
						break;
					}
					//�t�B���^�F�ύX�L��t�@�C��
				  case kFileListProjectFilter_DiffFiles:
					{
						//
						if( isFolder == false )
						{
							switch(filestate)
							{
							  case kSCMFileState_Modified:
							  case kSCMFileState_Added:
							  case kSCMFileState_Others:
							  case kSCMFileState_NotEntried:
								{
									filterMatchedItem = true;
									break;
								}
							  default:
								{
									//�����Ȃ�
									break;
								}
							}
						}
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
				//�t�B���^��A�Ώۃt�@�C���Ȃ�A�\���f�[�^�ɒǉ�
				if( filterMatchedItem == true )//#873
				{
					if( isFolder == true )
					{
						//------------------�t�H���_�̏ꍇ------------------
						mActionTextArray.Add(GetEmptyText());
						textArray.Add(partialPath);
						imageIconIDArray.Add(kImageID_iconSwlFolder);
						colorarray.Add(subwindowNormalColor);
						stylearray.Add(kTextStyle_Normal);
						partialPath.RemoveLastPathDelimiter();
					}
					else
					{
						//------------------�t�@�C���̏ꍇ------------------
						mActionTextArray.Add(path);
						textArray.Add(filename);
						imageIconIDArray.Add(kImageID_iconSwlDocument);
						colorarray.Add(color);
						stylearray.Add(style);//#458
					}
					//�p�X����'/'�̐��ɏ]���ăA�E�g���C�����x���擾
					AIndex	outlineLevel = 0;
					switch(mProjectFilterMode)
					{
					  case kFileListProjectFilter_AllFiles:
					  case kFileListProjectFilter_SameModeFiles:
						{
							outlineLevel = partialPath.GetCountOfChar('/');
							break;
						}
					  default:
						{
							//�����Ȃ�
							break;
						}
					}
					outlinearray.Add(outlineLevel*12);
				}
			}
			//�e�[�u���ɕ\���f�[�^�ݒ�
			GetListView().SPI_SetColumn_Text(kColumn_FileName,textArray);
			GetListView().SPI_SetTable_ImageIconID(imageIconIDArray);
			GetListView().SPI_SetTable_Color(colorarray);
			GetListView().SPI_SetTable_TextStyle(stylearray);//#458
			GetListView().SPI_SetTable_OutlineFoldingLevel(outlinearray);
			//�܂肽���ݏ�Ԃ𕜌�
			if( GetApp().SPI_GetCurrentSameProjectIndex() != kIndex_Invalid )
			{
				switch(mProjectFilterMode)
				{
				  case kFileListProjectFilter_AllFiles:
				  case kFileListProjectFilter_SameModeFiles:
					{
						ATextArray	collapsedata;
						GetApp().SPI_GetProjectFolderCollapseData(GetApp().SPI_GetCurrentSameProjectIndex(),collapsedata);
						GetListView().SPI_ApplyCollapseRowsByTextArray(kColumn_FileName,collapsedata);
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
			}
			//�R���e�L�X�g���j���[�ݒ�
			GetListView().SPI_SetEnableContextMenu(true,kContextMenuID_FileList);//R0186 #442
			//�I�����ڐݒ�
			sel = mSelectionIndex_SameProject;
			mCurrentSameProjectIndex = GetApp().SPI_GetCurrentSameProjectIndex();
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	GetListView().SPI_EndSetTable();
	GetListView().SPI_SetSelect(sel);
	if( mMode == kFileListMode_RecentlyOpened && mRecentSortMode == true )
	{
		GetListView().SPI_Sort(kColumn_FileName,true);
	}
	NVM_UpdateControlStatus();
}

void	AWindow_FileList::SPI_Update_DocumentOpened( const ADocumentID inTextDocumentID )
{
	if( NVI_IsWindowCreated() == false )   return;
	
	switch(mMode)
	{
	  case kFileListMode_RemoteFile:
		{
			SPI_UpdateTable();
			break;
		}
	  case kFileListMode_SameFolder:
	  case kFileListMode_SameProject:
	  case kFileListMode_RecentlyOpened:
		{
			//�����Ȃ�
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

void	AWindow_FileList::SPI_Update_DocumentActivated( const ADocumentID inTextDocumentID )
{
	if( NVI_IsWindowCreated() == false )   return;
	
	//#533
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetModeIndex();
	//
	switch(mMode)
	{
	  case kFileListMode_SameFolder:
		{
			AFileAcc	file;
			GetApp().SPI_GetTextDocumentByID(inTextDocumentID).NVI_GetFile(file);
			AFileAcc	parent;
			parent.SpecifyParent(file);
			AIndex	index = GetApp().SPI_GetSameFolderArrayIndex(parent);
			if( mCurrentSameFolderIndex != index )
			{
				mSelectionIndex_SameFolder = kIndex_Invalid;
				mCurrentSameFolderIndex = index;
				SPI_UpdateTable();
				GetListView().SPI_AdjustScroll();
				//
				NVI_UpdateProperty();
			}
			break;
		}
	  case kFileListMode_SameProject:
		{
			//
			AFileAcc	projectFolder;
			GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetProjectFolder(projectFolder);
			AIndex	index = GetApp().SPI_GetSameProjectArrayIndex(projectFolder);
			//���݂̃v���W�F�N�g�ƈႤ�A�܂��́A�i�t�B���^�F�������[�h�Łj���[�hindex���Ⴄ�ꍇ�A�e�[�u���X�V
			if( mCurrentSameProjectIndex != index || 
						(mCurrentModeIndex != modeIndex && mProjectFilterMode == kFileListProjectFilter_SameModeFiles) ) //#533 #873
			{
				//#892
				//�܂肽���ݏ�ԕۑ�
				SaveCollapseData();
				//�e�[�u���X�V
				mSelectionIndex_SameProject = kIndex_Invalid;
				mCurrentSameProjectIndex = index;
				mCurrentModeIndex = modeIndex;//#533
				SPI_UpdateTable();
				GetListView().SPI_AdjustScroll();
				//
				NVI_UpdateProperty();
			}
			break;
		}
	  case kFileListMode_RemoteFile:
	  case kFileListMode_RecentlyOpened:
		{
			//�����Ȃ�
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//#533
	mCurrentModeIndex = modeIndex;
}

void	AWindow_FileList::SPI_Update_RecentlyOpendFileUpdated()
{
	if( NVI_IsWindowCreated() == false )   return;
	
	switch(mMode)
	{
	  case kFileListMode_RecentlyOpened:
		{
			SPI_UpdateTable();
			GetListView().SPI_SetSelect(0);
			mSelectionIndex_Recent = 0;
			GetListView().SPI_AdjustScroll();
			break;
		}
	  case kFileListMode_SameFolder:
	  case kFileListMode_SameProject:
	  case kFileListMode_RemoteFile:
		{
			//�����Ȃ�
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

void	AWindow_FileList::SPI_ChangeMode( const ANumber& inMode )
{
	if( inMode < kFileListMode_Min || inMode > kFileListMode_Max )   return;
	//#892
	SaveCollapseData();
	//
	mMode = inMode;
	UpdateViewBounds();
	SPI_UpdateTable();
	NVM_UpdateControlStatus();
	NVI_UpdateProperty();//#798
	NVI_RefreshWindow();
	//�őO�ʂ̃E�C���h�E�ŁA�T�C�h�o�[�̃f�[�^��app pref db�ɕۑ�����i���ɊJ���E�C���h�E�ɁA����̃��[�h�ύX��K�p�����邽�߁j
	AWindowID	frontWinID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
	if( frontWinID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(frontWinID).SPI_SaveSideBarData();
	}
}

/*#725
void	AWindow_FileList::SPI_UpdateSelection_Window( const AIndex& inSelection )
{
	if( NVI_IsWindowCreated() == false )   return;
	
	GetListView().SPI_SetSelect(inSelection);
	mSelectionIndex_RemoteConnection = inSelection;
}
*/

/**
�^�C�g���e�L�X�g�X�V
*/
void	AWindow_FileList::UpdateTitleText()
{
	switch(mMode)
	{
	  case kFileListMode_RecentlyOpened:
		{
			AText	text;
			text.SetLocalizedText("FileListWindow_RecentFile");
			GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwBox_R);
			break;
		}
	  case kFileListMode_SameFolder:
		{
			AText	text;
			text.SetLocalizedText("FileListWindow_SameFolder");
			GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwBox_S);
			break;
		}
	  case kFileListMode_SameProject:
		{
			AText	text;
			text.SetLocalizedText("FileListWindow_SameProject");
			GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwBox_P);
			break;
		}
	  case kFileListMode_RemoteFile:
		{
			AText	text;
			text.SetLocalizedText("FileListWindow_RemoteFile");
			GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwBox_R);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

/*B0000 080810 void	AWindow_FileList::SPI_ClearCursorRow()
{
	GetListView().SPI_ClearCursorRow();
}
*/
//R0186
AIndex	AWindow_FileList::SPI_GetContextMenuSelectedRowDBIndex() const
{
	return GetListViewConst().SPI_GetContextMenuSelectedRowDBIndex();
}

//#892
/**
�܂肽���ݏ�Ԃ�ۑ�
*/
void	AWindow_FileList::SaveCollapseData()
{
	switch(mMode)
	{
	  case kFileListMode_SameProject:
		{
			switch(mProjectFilterMode)
			{
			  case kFileListProjectFilter_AllFiles:
			  case kFileListProjectFilter_SameModeFiles:
				{
					if( mActionTextArray.GetItemCount() > 0 )
					{
						//�܂肽���ݍs�̃e�L�X�g���X�g���擾
						ATextArray	textarray;
						GetListView().SPI_GetCollapseRowsTextArray(kColumn_FileName,textarray);
						//�����v���W�F�N�g�̃f�[�^�ɕۑ�
						GetApp().SPI_SaveProjectFolderCollapseData(mCurrentSameProjectIndex,textarray);
					}
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

//#361
/**
���݂̃p�X���擾
*/
void	AWindow_FileList::SPI_GetCurrentPath( AText& outText ) const
{
	//���ʏ�����
	outText.DeleteAll();
	
	//�����L�����Ȃ��ꍇ�͉��������I��
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )
	{
		return;
	}
	
	switch(mMode)
	{
	  case kFileListMode_RemoteFile:
		{
			outText.SetNumber(mRemoteConnectionCurrentLevel);
			outText.AddCstring("\r");
			outText.AddText(mRemoteConnectionRootFolderURL);
			outText.AddCstring("\r");
			outText.AddText(mRemoteConnectionCurrentFolderURL);
			outText.AddCstring("\r");
            //#1231 outText.AddText(mRemoteConnectionConnectionType);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

/**
���݂̃p�X��ݒ�
*/
void	AWindow_FileList::SPI_SetCurrentPath( const AText& inText )
{
	switch(mMode)
	{
	  case kFileListMode_RemoteFile:
		{
			//�A�J�E���g���X�g�\���ɂ���iLIST�v���̉����������������ɁA�A�J�E���g�N���b�N�ɂ��ʂ̃A�J�E���g�ւ̃A�N�Z�X�ɕύX�ł���悤�ɂ��邽�߁j
			mRemoteConnectionCurrentLevel = 0;
			//#1231 mRemoteConnectionConnectionType.DeleteAll();
			mRemoteConnectionCurrentFolderURL.DeleteAll();
			mRemoteConnectionRootFolderURL.DeleteAll();
			SPI_UpdateTable();
			//LIST�v���f�[�^�擾
			AIndex	p = 0;
			AText	levelText;
			inText.GetLine(p,levelText);
			mRemoteConnectionCurrentLevel_Request = levelText.GetNumber();
			inText.GetLine(p,mRemoteConnectionRootFolderURL_Request);
			inText.GetLine(p,mRemoteConnectionCurrentFolderURL_Request);
			//#1231 inText.GetLine(p,mRemoteConnectionConnectionType_Request);
			mRemoteConnectionCurrentFolderURL_Request.AddLastPathDelimiter();
			
			if( mRemoteConnectionCurrentLevel_Request > 0 )
			{
				//�����[�g�ڑ��I�u�W�F�N�g�Ńv���O�C���ڑ�
				/*#1231
				if( GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
					SetConnectionType(mRemoteConnectionConnectionType_Request) == true )
				{
				*/
				//�����[�g�ڑ��I�u�W�F�N�g��LIST���s�v��
				GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
						ExecuteLIST(kRemoteConnectionRequestType_LISTfromFileListWindow,
									mRemoteConnectionCurrentFolderURL_Request,GetEmptyText());
				//�v���O���X�\�� #1236
				ShowRemoteAccessProgress(true);
				
				//��LIST������M���ASPI_SetRemoteConnectionResult()�������[�g�ڑ��I�u�W�F�N�g����R�[������A
				//�����Ń��X�g�X�V�����B
				/*#1231
				}
				else
				{
					//���v���O�C���������ꍇ
				}
				*/
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

#if 0
//#725 �폜
//#291
/**
�I�[�o�[���C���[�h���̓����x�ݒ�
*/
void	AWindow_FileList::SPI_UpdateOverlayWindowAlpha()
{
	if( NVI_GetOverlayMode() == true )
	{
		NVI_GetListView(mListViewControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
	}
}

//#634
/**
�T�C�h�o�[���[�h�̓��ߗ��E�w�i�F�ɐ؂�ւ�
*/
void	AWindow_FileList::SPI_SetSideBarMode( const ABool inSideBarMode, const AColor inSideBarColor )
{
	if( NVI_GetOverlayMode() == true )
	{
		if( inSideBarMode == true )
		{
			//�ł�����蕶�������ꂢ�ɕ\�������邽�߂ɕs���ߗ��͂ł��邾�����߂ɐݒ肷��B�i�w�i�̃O���f�[�V�������������瓧�߂�����x�j
			NVI_GetListView(mListViewControlID).SPI_SetBackgroundColor(inSideBarColor,kColor_Gray97Percent);
			NVI_GetListView(mListViewControlID).SPI_SetTransparency(0.8);
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTransparency(0.8);
			NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetTransparency(0.8);
		}
		else
		{
			NVI_GetListView(mListViewControlID).SPI_SetBackgroundColor(kColor_White,kColor_White);
			NVI_GetListView(mListViewControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTransparency(1);
			NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetTransparency(1);
		}
	}
}
#endif

#pragma mark ===========================

#pragma mark ---�����[�g�ڑ�
//#361

/**
�����[�g�ڑ��I�u�W�F�N�g�ւ�LIST�v���ɑ΂��鉞����M������
*/
void	AWindow_FileList::SPI_SetRemoteConnectionResult( const ATextArray& inFileNameArray, const ABoolArray& inIsDirectoryArray )
{
	//�v���O���X��\�� #1236
	ShowRemoteAccessProgress(false);
	
	//LIST���ʂ��S���Ȃ��ꍇ�͉����������^�[���i�G���[�łȂ��ꍇ��.��..�����݂���͂��j
	if( inFileNameArray.GetItemCount() == 0 )
	{
		return;
	}
	
	//�v���f�[�^���A���݃f�[�^�ɐݒ�
	mRemoteConnectionCurrentLevel = mRemoteConnectionCurrentLevel_Request;
    //#1231 mRemoteConnectionConnectionType.SetText(mRemoteConnectionConnectionType_Request);
	mRemoteConnectionCurrentFolderURL.SetText(mRemoteConnectionCurrentFolderURL_Request);
	mRemoteConnectionRootFolderURL.SetText(mRemoteConnectionRootFolderURL_Request);
	//��M�f�[�^��ݒ�
	mRemoteConnectionFilenameArray.SetFromTextArray(inFileNameArray);
	mRemoteConnectionIsDiractoryArray.SetFromObject(inIsDirectoryArray);
	//".", ".."�̍��ڂ͍폜����
	for( AIndex i = 0; i < mRemoteConnectionFilenameArray.GetItemCount(); )
	{
		AText	filename;
		mRemoteConnectionFilenameArray.Get(i,filename);
		if( filename.Compare(".") == true || filename.Compare("..") == true )
		{
			mRemoteConnectionFilenameArray.Delete1(i);
			mRemoteConnectionIsDiractoryArray.Delete1(i);
		}
		else
		{
			i++;
		}
	}
	//1�Ԗڂ̍��ڂ�e�t�H���_�ւ̈ړ����ڂɂ���
	AText	parentFolderTitle;
	parentFolderTitle.SetLocalizedText("RemoteFileParentFolder");
	mRemoteConnectionFilenameArray.Insert1(0,parentFolderTitle);
	mRemoteConnectionIsDiractoryArray.Insert1(0,true);
	//2�Ԗڂ̍��ڂ�V�K�t�@�C�������p���ڂɂ���
	AText	newFile;
	newFile.SetLocalizedText("RemoteFileNewFile");
	mRemoteConnectionFilenameArray.Insert1(1,newFile);
	mRemoteConnectionIsDiractoryArray.Insert1(1,false);
	//�e�[�u���X�V
	SPI_UpdateTable();
	//�I������
	GetListView().SPI_SetSelect(kIndex_Invalid);
	//�X�N���[������
	GetListView().SPI_AdjustScroll();
}

/**
�����[�g�t�@�C�����[�h���A���݂̃t�H���_���X�V�i�h�L�������g�ۑ����Ɏ��s�����j
*/
void	AWindow_FileList::SPI_UpdateRemoteFolder( const AText& inFolderURL )
{
	if( mMode == kFileListMode_RemoteFile )
	{
		AText	url1, url2;
		url1.SetText(mRemoteConnectionCurrentFolderURL);
		url1.AddPathDelimiter(kUChar_Slash);
		url2.SetText(inFolderURL);
		url2.AddPathDelimiter(kUChar_Slash);
		if( url1.Compare(url2) == true )
		{
			//���݂̃t�H���_���e�ōX�V
			mRemoteConnectionCurrentLevel_Request = mRemoteConnectionCurrentLevel;
			mRemoteConnectionRootFolderURL_Request.SetText(mRemoteConnectionRootFolderURL);
			mRemoteConnectionCurrentFolderURL_Request.SetText(mRemoteConnectionCurrentFolderURL);
			//#1231 mRemoteConnectionConnectionType_Request.SetText(mRemoteConnectionConnectionType);
			mRemoteConnectionCurrentFolderURL_Request.AddLastPathDelimiter();
			
			//�����[�g�ڑ��I�u�W�F�N�g�Ńv���O�C���ڑ�
			/*#1231 
			if( GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
				SetConnectionType(mRemoteConnectionConnectionType_Request) == true )
			{
			*/
			//�����[�g�ڑ��I�u�W�F�N�g��LIST���s�v��
			GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
					ExecuteLIST(kRemoteConnectionRequestType_LISTfromFileListWindow,
								mRemoteConnectionCurrentFolderURL_Request,GetEmptyText());
			//�v���O���X�\�� #1236
			ShowRemoteAccessProgress(true);
			
			//��LIST������M���ASPI_SetRemoteConnectionResult()�������[�g�ڑ��I�u�W�F�N�g����R�[������A
			//�����Ń��X�g�X�V�����B
			/*#1231
			}
			*/
		}
	}
}

#pragma mark ===========================

#pragma mark ---

AView_List&	AWindow_FileList::GetListView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_List,kViewType_List,mListViewControlID);
	//#199 return dynamic_cast<AView_List&>(NVI_GetViewByControlID(mListViewControlID));
}
//R0186
const AView_List&	AWindow_FileList::GetListViewConst() const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_List,kViewType_List,mListViewControlID);
	//#199 return dynamic_cast<const AView_List&>(NVI_GetViewConstByControlID(mListViewControlID));
}

void	AWindow_FileList::UpdateViewBounds()
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//�^�C���o�[�A�Z�p���[�^�����擾
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	ANumber	height_Filter = NVI_GetEditBoxView(kControlID_Filter).SPI_GetLineHeightWithMargin() + kHeight_FilterTopBottomMargin;
	ANumber	height_Header = kHeight_Header1Line;
	switch( mMode )
	{
	  case kFileListMode_RecentlyOpened:
		{
			height_Header = 0;
			break;
		}
	  case kFileListMode_SameProject:
		{
			height_Header = kHeight_Header2Lines;
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	
	//
	ABool	isSideBarBottom = GetApp().SPI_IsSubWindowSideBarBottom(GetObjectID());
	
	//�w�i�F�`��view����
	ANumber	leftMargin = 1;
	ANumber	rightMargin = 1;
	ANumber	bottomMargin = 1;
	AWindowPoint	backgroundPt = {0,height_Titlebar};
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			leftMargin = 0;
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			rightMargin = 0;
			break;
		}
	  default:
		{
			leftMargin = 0;
			rightMargin = 0;
			bottomMargin = 0;
			break;
		}
	}
	if( isSideBarBottom == true )
	{
		bottomMargin = 0;
	}
	
	//�^�C�g���o�[�A�Z�p���[�^�\���t���O
	ABool	showTitleBar = false;
	ABool	showSeparator = false;
	ABool	showSizeBox = false;
	//list view�T�C�Y
	AWindowPoint	listViewPoint = {leftMargin,0};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top  -bottomMargin;
	//�X�N���[���o�[��
	ANumber	vscrollbarWidth = kVScrollBarWidth;
	//
	ANumber	sizeboxHeight = 0;
	//�E�T�C�h�o�[�̍ŏI���ڂ̏ꍇ�A���A�X�e�[�^�X�o�[���\�����ȊO�̓T�C�Y�{�b�N�X�̑Ώۂ͎��E�C���h�E
	NVI_SetSizeBoxTargetWindowID(GetObjectID());
	//���C�A�E�g�v�Z
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//�T�C�h�o�[�̏ꍇ
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//�^�C�g���o�[�A�Z�p���[�^�\���t���O
			showTitleBar = true;
			showSeparator = true;
			//list view�T�C�Y
			listViewPoint.y += height_Titlebar + height_Filter + height_Header;
			listViewHeight -= height_Titlebar + height_Filter + height_Header;// + height_Separator;
			/*
			if( isSideBarBottom == true )
			{
				showSeparator = false;
				//�E�T�C�h�o�[�̍ŏI���ڂ̏ꍇ�A���A�X�e�[�^�X�o�[���\�����̓T�C�Y�{�b�N�X��\������
				if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_RightSideBar &&
							GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayStatusBar) == false )
				{
					showSizeBox = true;
					//�T�C�Y�{�b�N�X�Ώۂ��e�L�X�g�E�C���h�E�ɂ���
					NVI_SetSizeBoxTargetWindowID(NVI_GetOverlayParentWindowID());
				}
			}
			*/
			break;
		}
		//�t���[�e�B���O�̏ꍇ
	  default:
		{
			//�^�C�g���o�[�A�Z�p���[�^�\���t���O
			showTitleBar = true;
			//list view�T�C�Y
			listViewPoint.y += height_Titlebar + height_Filter + height_Header;
			listViewHeight -= height_Titlebar + height_Filter + height_Header;
			//
			showSizeBox = true;
			sizeboxHeight = kHeight_SizeBox;
			break;
		}
	}
	
	//�^�C�g���o�[�z�u
	if( showTitleBar == true )
	{
		//�^�C�g���o�[�z�u
		AWindowPoint	pt = {-1+leftMargin,0};
		NVI_SetControlPosition(kControlID_TitleBar,pt);
		NVI_SetControlSize(kControlID_TitleBar,windowBounds.right - windowBounds.left +2-leftMargin-rightMargin,height_Titlebar);
		//�^�C�g���o�[�\��
		NVI_SetShowControl(kControlID_TitleBar,true);
	}
	else
	{
		//�^�C�g���o�[��\��
		NVI_SetShowControl(kControlID_TitleBar,false);
	}
	
	//�w�i�`��view�z�u
	{
		//�w�i�`��view�z�u
		NVI_SetControlPosition(kControlID_Background,backgroundPt);
		NVI_SetControlSize(kControlID_Background,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top - backgroundPt.y);
		//�w�i�`��view�\��
		NVI_SetShowControl(kControlID_Background,true);
	}
	
	//�Z�p���[�^�z�u
	if( showSeparator == true )
	{
		//�Z�p���[�^�z�u
		AWindowPoint	pt = {0,windowBounds.bottom-windowBounds.top-height_Separator};
		NVI_SetControlPosition(kControlID_Separator,pt);
		NVI_SetControlSize(kControlID_Separator,windowBounds.right-windowBounds.left,height_Separator);
		//�Z�p���[�^�\��
		NVI_SetShowControl(kControlID_Separator,true);
	}
	else
	{
		//�Z�p���[�^��\��
		NVI_SetShowControl(kControlID_Separator,false);
	}
	
	//�t�B���^�z�u
	{
		AWindowPoint	pt = {leftMargin,height_Titlebar + height_Header};
		NVI_SetControlPosition(kControlID_Filter,pt);
		NVI_SetControlSize(kControlID_Filter,windowBounds.right-windowBounds.left - leftMargin - rightMargin,height_Filter);
	}
	
	//�w�b�_�z�u
	if( height_Header > 0 )
	{
		AWindowPoint	pt = {leftMargin,height_Titlebar};
		NVI_SetControlPosition(kControlID_Header,pt);
		NVI_SetControlSize(kControlID_Header,windowBounds.right-windowBounds.left - leftMargin - rightMargin,height_Header);
		NVI_SetShowControl(kControlID_Header,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_Header,false);
	}
	
	//�t�H���_�[�p�X�{�^���z�u
	if( height_Header > 0 )
	{
		AWindowPoint	pt = {kLeftMargin_HeaderButton,height_Titlebar + 2};
		NVI_SetControlPosition(kControlID_FolderPath,pt);
		NVI_SetControlSize(kControlID_FolderPath,windowBounds.right-windowBounds.left-kLeftMargin_HeaderButton-kRightMargin_HeaderButton,kHeight_HeaderButton);
		NVI_SetShowControl(kControlID_FolderPath,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_FolderPath,false);
	}
	
	//�v���W�F�N�g���[�h�t�B���^�{�^���z�u
	if( mMode == kFileListMode_SameProject )
	{
		AWindowPoint	pt = {kLeftMargin_HeaderButton,height_Titlebar + 19};
		NVI_SetControlPosition(kControlID_ProjectModeFilter,pt);
		NVI_SetControlSize(kControlID_ProjectModeFilter,windowBounds.right-windowBounds.left-kLeftMargin_HeaderButton-kRightMargin_HeaderButton,kHeight_HeaderButton);
		NVI_SetShowControl(kControlID_ProjectModeFilter,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_ProjectModeFilter,false);
	}
	
	//FileListView bounds�ݒ�
	{
		NVI_SetControlPosition(mListViewControlID,listViewPoint);
		NVI_SetControlSize(mListViewControlID,listViewWidth - vscrollbarWidth,listViewHeight);
	}
	//V�X�N���[���o�[�z�u
	if( vscrollbarWidth > 0 && NVI_IsControlEnable(kControlID_VScrollBar) == true )
	{
		AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
		NVI_SetControlPosition(kControlID_VScrollBar,pt);
		NVI_SetControlSize(kControlID_VScrollBar,vscrollbarWidth,listViewHeight - sizeboxHeight);
		NVI_SetShowControl(kControlID_VScrollBar,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_VScrollBar,false);
	}
	
	//�T�C�Y�{�b�N�X�z�u
	if( showSizeBox == true )
	{
		AWindowPoint	pt = {windowBounds.right-windowBounds.left-kWidth_SizeBox - rightMargin,
		windowBounds.bottom-windowBounds.top-kHeight_SizeBox - bottomMargin};
		NVI_SetControlPosition(kControlID_SizeBox,pt);
		NVI_SetControlSize(kControlID_SizeBox,kWidth_SizeBox,kHeight_SizeBox);
		NVI_SetShowControl(kControlID_SizeBox,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_SizeBox,false);
	}
	
#if 0
	if( mMode == kFileListMode_RecentlyOpened )
	{
		AWindowPoint	pt;
		pt.x = 4;
		pt.y = kTitleBarHeight + 4;
		ANumber	w = windowBounds.right - windowBounds.left -kSortButtonWidth -16;
		ANumber	h = kModeChangeButtonHeight;
#if IMPLEMENTATION_FOR_MACOSX
		//pt.x--;
		//pt.y--;
		//w += 2;
		w++;
		//h++;
#endif
		/*#232
		NVI_SetControlPosition(kButton_ModeChange,pt);
		NVI_SetControlSize(kButton_ModeChange,w,h);
		*/
		NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetPosition(pt);
		NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetSize(w,h);
		pt.x = windowBounds.right - windowBounds.left -kSortButtonWidth;
		pt.y = kTitleBarHeight + 4;
		w = kSortButtonWidth;
		h = kModeChangeButtonHeight;
# if IMPLEMENTATION_FOR_MACOSX
		//pt.y--;
		//h++;
#endif
		/*#232
		NVI_SetControlPosition(kButton_Sort,pt);
		NVI_SetShowControl(kButton_Sort,true);
		NVI_SetControlSize(kButton_Sort,w,h);
		NVI_SetControlBool(kButton_Sort,mRecentSortMode);
		*/
		NVI_GetButtonViewByControlID(kButton_Sort).NVI_SetPosition(pt);
		NVI_GetButtonViewByControlID(kButton_Sort).NVI_SetSize(w,h);
		NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetToogleOn(mRecentSortMode);
		NVI_GetButtonViewByControlID(kButton_Sort).NVI_SetShow(true);
	}
	else
	{
		AWindowPoint	pt;
		pt.x = 4;
		pt.y = kTitleBarHeight + 4;
#if IMPLEMENTATION_FOR_MACOSX
		//pt.x--;
		//pt.y--;
#endif
		//#232 NVI_SetControlPosition(kButton_ModeChange,pt);
		NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetPosition(pt);
		ANumber	w = windowBounds.right - windowBounds.left -16;
		ANumber	h = kModeChangeButtonHeight;
#if IMPLEMENTATION_FOR_MACOSX
		//w += 2;
		//h++;
#endif
		//#232 NVI_SetControlSize(kButton_ModeChange,w,h);
		NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetSize(w,h);
		//#232 NVI_SetShowControl(kButton_Sort,false);
		NVI_GetButtonViewByControlID(kButton_Sort).NVI_SetShow(false);
	}
	#endif
}

//#1236
/**
�����[�g�A�N�Z�X�v���O���X�\������
*/
void AWindow_FileList::ShowRemoteAccessProgress( const ABool inShow )
{
	//
	NVI_SetShowControl(kControlID_RemoteAccessProgressIndicator,inShow);
	NVI_SetControlBool(kControlID_RemoteAccessProgressIndicator,inShow);
}





