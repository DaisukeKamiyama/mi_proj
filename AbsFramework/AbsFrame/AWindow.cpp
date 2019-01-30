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

AWindow
�e��E�C���h�E�̊��N���X

#175: AWindow�I�u�W�F�N�g�ɂ͑S��ObjectID�����蓖�Ă�悤�ɏC��

*/

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow
#pragma mark ===========================
//�E�C���h�E���N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow::AWindow(/*#175 AObjectArrayItem* inParent */) : AObjectArrayItem(/*#175 inParent*/&(AApplication::GetApplication())), mWindowImp(NULL,*this), mCurrentTabIndex(kIndex_Invalid), 
		mDynamicControlIDRangeStart(/*win kControlID_Invalid*//*10000*/kDefaultDynamicControlIDRangeStart), 
		mDynamicControlIDRangeEnd(/*win 080618 kControlID_Invalid*/kNumber_MaxNumber), 
		mNextDynamicControlID(/*win 080618 kControlID_Invalid*//*10000*/kDefaultDynamicControlIDRangeStart),
		/*#135 mCurrentFocus(kControlID_Invalid),*/ mVisibleMode(false), mFixTitle(false)//win 080726
		,mParentWindowIDForStickyWindow(kObjectID_Invalid)
		,mInhibitClose(false)//#92
		,mLastClickedButtonControlID(kControlID_Invalid)//#232
		,mDefaultCancelButton(kControlID_Invalid)//#135
		,mDefaultOKButton(kControlID_Invalid)//#135
		,mLatentFocus(kControlID_Invalid)//#135
		,mOverlayMode(false), mOverlayParentWindowID(kObjectID_Invalid)//#291
		,mOKWindow(kObjectID_Invalid), mOKCancelWindow(kObjectID_Invalid), mOKCancelDiscardWindow(kObjectID_Invalid)//#291
		,mOverlayCurrentOffset(kWindowPoint_00),mOverlayCurrentWidth(0),mOverlayCurrentHeight(0)//#379
,mInitialWindowBoundsIsSet(false),mInitialWindowBounds(kRect_0000)//#850
,mCurrentWindowWidth(0),mCurrentWindowHeight(0)//#688
,mSizeBoxTargetWindowID(kObjectID_Invalid)
,mVirtualLiveResizing(false)
,mAskSaveChanges_IsModal(false),mAskSaveChanges_Result(kAskSaveChangesResult_None)
,mSaveCancelDiscardWindow(kObjectID_Invalid)//#1034
{
	mWindowPositionDataID = kDataID_Invalid;
}
//�f�X�g���N�^
AWindow::~AWindow()
{
	/*#92 DoDeleted()�ֈړ�
	NVI_Close();
	//#175
	//����E�C���h�E����폜
	AApplication::GetApplication().NVI_DeleteFromWindowRotateArray(GetObjectID());
	*/
}

//#138
/**
AObjectArrayItem��AObjectArray����폜��������ɃR�[�������B
�i�f�X�g���N�^��GC���Ȃ̂ŁA�^�C�~���O����ł��Ȃ��B����āA�e��폜������DoDeleted()�ōs���ׂ��B�j
*/
void	AWindow::DoDeleted()
{
	//#138 StickyWindow�̏ꍇ�A�e�E�C���h�E�̃E�C���h�E�O���[�v����A�������폜����
	if( mParentWindowIDForStickyWindow != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mParentWindowIDForStickyWindow).NVI_UnregisterChildStickyWindow(GetObjectID());
		mParentWindowIDForStickyWindow = kObjectID_Invalid;
	}
	//�^�C�}�[�ΏۃE�C���h�E����폜
	AApplication::GetApplication().NVI_DeleteFromTimerActionWindowArray(GetObjectID());
	//AllClose�ΏۃE�C���h�E����폜 #199
	AApplication::GetApplication().NVI_DeleteFromAllCloseTargetWindowArray(GetObjectID());
	//�E�C���h�E��Hide����
	NVI_Hide();
	//#92 �f�X�g���N�^����ړ�
	//
	NVI_Close();
	//#175
	//����E�C���h�E����폜
	AApplication::GetApplication().NVI_DeleteFromWindowRotateArray(GetObjectID());
	//#92
	NVIDO_DoDeleted();
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//���j���[�I�����̃R�[���o�b�N�i�������s������true��Ԃ��j
ABool	AWindow::EVT_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
				EVT_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys);
	}
	//
	//���݂�focus view��AView�ł���ꍇ�́AAView�Ŏ��s�����݂�
	AControlID	focusViewID = NVI_GetCurrentFocus();
	if( focusViewID != kControlID_Invalid )
	{
		if( IsView(focusViewID) == true )
		{
			if( NVI_GetViewByControlID(focusViewID).NVI_IsFocusActive() == true )//#725
			{
				if( NVI_GetViewByControlID(focusViewID).EVT_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys) == true )
				{
					return true;
				}
			}
		}
	}
	//�T�u�N���X�Ŏ��s�����݂�
	if( EVTDO_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys) == true )
	{
		return true;
	}
	if( inMenuItemID == kMenuItemID_Close )
	{
		NVI_Hide();
		return true;
	}
	
	//#725
	//�I�[�o�[���C�E�C���h�E�̏ꍇ�A�e�E�C���h�E�ł̎��s�����݂�
	if( NVI_GetOverlayParentWindowID() != kObjectID_Invalid )
	{
		if( AApplication::GetApplication().NVI_GetWindowByID(NVI_GetOverlayParentWindowID()).
					EVT_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys) == true )
		{
			return true;
		}
	}
	return false;
}

//���j���[���ڂ̃A�b�v�f�[�g�v�����̃R�[���o�b�N
void	AWindow::EVT_UpdateMenu()
{
	//#688 �I����������UpdateMenu�����s���Ȃ��悤�ɂ���
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return;
	
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).EVT_UpdateMenu();
		return;
	}
	//
	//���݂�focus view��AView�ł���ꍇ�́AAView��update
	AControlID	focusViewID = NVI_GetCurrentFocus();
	if( focusViewID != kControlID_Invalid )
	{
		if( IsView(focusViewID) == true )
		{
			if( NVI_GetViewByControlID(focusViewID).NVI_IsFocusActive() == true )//#725
			{
				NVI_GetViewByControlID(focusViewID).EVT_UpdateMenu();
			}
		}
	}
	//
	if( NVI_HasCloseButton() == true && NVI_IsChildWindowVisible() == false/*B0411*/ )
	{
		AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,true);
	}
	//�T�u�N���X�ł�update
	EVTDO_UpdateMenu();
	
	//#725
	//�I�[�o�[���C�E�C���h�E�̏ꍇ�A�e�E�C���h�E�ł�update
	if( NVI_GetOverlayParentWindowID() != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(NVI_GetOverlayParentWindowID()).EVT_UpdateMenu();
	}
}

//�e�L�X�g���͎��̃R�[���o�b�N
void AWindow::EVT_TextInputted( const AControlID inID )
{
	EVTDO_TextInputted(inID);
}

//�R���g���[���̃N���b�N���̃R�[���o�b�N
ABool	AWindow::EVT_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	//���W�I�O���[�v�X�V
	UpdateRadioGroup(inID);
	
	//#539
	AIndex	helpAnchorIndex = mHelpAnchorArray_ControlID.Find(inID);
	if( helpAnchorIndex != kIndex_Invalid )
	{
		AFileAcc	helpFolder;
		AFileWrapper::GetHelpFolder("ja",helpFolder);
		AText	url;
		helpFolder.GetURL(url);
		if( url.GetItemCount() > 0 )
		{
			if( url.Get(url.GetItemCount()-1) != '/' )
			{
				url.Add('/');
			}
			url.AddText(mHelpAnchorArray_Anchor.GetTextConst(helpAnchorIndex));
			ALaunchWrapper::OpenURLWithDefaultBrowser(url);
		}
		return true;
	}
	//
	ABool	result = false;
	if( EVTDO_Clicked(inID,inModifierKeys) == true )   result = true;
	
	//EVTDO_Clicked���ŃE�C���h�E������ꍇ
	if( NVI_IsWindowCreated() == false )   return true;
	
	if( result == false )
	{
		//DB�A������
		if( IsDBDataTableExist() == true )
		{
			if( DoClicked_DBTable(inID) == true )   result = true;
		}
	}
	
	//#349 ���W�I�O���[�v�X�V
	//#791 ��ֈړ� UpdateRadioGroup(inID);
	
	//DB�f�[�^�ۑ����� #1239
	NVMDO_NotifyDataChangedForSave();
	
	//
	NVM_UpdateControlStatus();
	return result;
}

//�R���g���[���̃_�u���N���b�N���̃R�[���o�b�N
ABool	AWindow::EVT_DoubleClicked( const AControlID inID )
{
	return EVTDO_DoubleClicked(inID);
}

//�R���g���[���l�ύX�ʒm���̃R�[���o�b�N�i�������s������true��Ԃ��j
ABool	AWindow::EVT_ValueChanged( const AControlID inID )
{
	ABool	result = false;
	if( EVTDO_ValueChanged(inID) == true )   result = true;
	
	if( result == false )
	{
		//DB�A������
		if( IsDBDataTableExist() == true )
		{
			if( DoValueChanged_DBData(inID) == true )   result = true;
			if( DoValueChanged_DBTable(inID) == true )   result = true;
			if( DoValueChanged_DBTableContentView(inID) == true )   result = true;
		}
	}
	
	//DB�f�[�^�ۑ����� #1239
	NVMDO_NotifyDataChangedForSave();
	
	NVM_UpdateControlStatus();
	return result;
}

//#688
/**
�c�[���o�[���ڒǉ��^�폜�������i�폜����inID=kControlID_Invalid)
*/
void	AWindow::EVT_ToolbarItemAdded( const AControlID inID )
{
	EVTDO_ToolbarItemAdded(inID);
}

//WindowActivated���̃R�[���o�b�N
void	AWindow::EVT_WindowActivated()
{
	EVTDO_WindowActivated();
	NVM_UpdateControlStatus();
	//Tab Activated
	NVM_DoTabActivated(mCurrentTabIndex,false);//win 080712
	//�E�C���h�E���̊e��Control�̕\�����X�V���� #137
	//#725 �����������̂��߁A������refresh�͂��Ȃ��BNVI_RefreshWindow();
	//#175
	//�E�C���h�E���񏇂��X�V����
	AApplication::GetApplication().NVI_RotateWindowActivated(GetObjectID());
	//�A�v���P�[�V�������ʂ�Window�A�N�e�B�x�[�g���������s #959
	AApplication::GetApplication().EVT_WindowActivated();
}

//WindowDeactivated���̃R�[���o�b�N
void	AWindow::EVT_WindowDeactivated()
{
	EVTDO_WindowDeactivated();
	//Tab Deactivated
	if( mCurrentTabIndex != kIndex_Invalid )//win
	{
		NVM_DoTabDeactivated(mCurrentTabIndex);
	}
	//�E�C���h�E���̊e��Control�̕\�����X�V���� #137
	//#725 �����������̂��߁A������refresh�͂��Ȃ��BNVI_RefreshWindow();
}

//#688
/**
�E�C���h�E�t�H�[�J�Xactivate������
*/
void	AWindow::EVT_WindowFocusActivated()
{
	//�h���N���X������
	EVTDO_WindowFocusActivated();
	//���݂̃t�H�[�J�Xview�ł̃t�H�[�J�Xactivated����
	if( mLatentFocus != kControlID_Invalid )
	{
		EVT_DoViewFocusActivated(mLatentFocus);
	}
}

//#688
/**
�E�C���h�E�t�H�[�J�Xdeactivate������
*/
void	AWindow::EVT_WindowFocusDeactivated()
{
	//�h���N���X������
	EVTDO_WindowFocusDeactivated();
	//���݂̃t�H�[�J�Xview�ł̃t�H�[�J�Xdeactivated����
	if( mLatentFocus != kControlID_Invalid )
	{
		EVT_DoViewFocusDeactivated(mLatentFocus);
	}
}

//�N���[�Y�{�^���N���b�N���̓���
void	AWindow::EVT_DoCloseButton()
{
	EVTDO_DoCloseButton();
	
	//DB�f�[�^�ۑ����� #1239
	NVMDO_NotifyDataChangedForSave();
}

//�t�H���_�I���_�C�A���O�Ńt�H���_���I�����ꂽ�Ƃ��̃R�[���o�b�N
void	AWindow::EVT_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder )
{
	EVTDO_FolderChoosen(inControlID,inFolder);
	
	//DB�f�[�^�ۑ����� #1239
	NVMDO_NotifyDataChangedForSave();
}

//�t�@�C���I���_�C�A���O�Ńt�@�C�����I�����ꂽ�Ƃ��̃R�[���o�b�N
void	AWindow::EVT_FileChoosen( const AControlID inControlID, const AFileAcc& inFile )
{
	EVTDO_FileChoosen(inControlID,inFile);
	
	//DB�f�[�^�ۑ����� #1239
	NVMDO_NotifyDataChangedForSave();
}

//�t�@�C���I���_�C�A���O�Ńt�@�C�����I�����ꂽ�Ƃ��̃R�[���o�b�N
void	AWindow::EVT_AskSaveChangesReply( const AObjectID inDocumentID, const AAskSaveChangesResult inAskSaveChangesReply )
{
	if( mAskSaveChanges_IsModal == true )
	{
		//���[�_���_�C�A���O�Ŏ��s�����ꍇ�̓����o�ϐ��ɋL��
		mAskSaveChanges_Result = inAskSaveChangesReply;
	}
	else
	{
		//�V�[�g�Ŏ��s�����ꍇ�͔h���N���X�ł̏������s
		EVTDO_AskSaveChangesReply(inDocumentID,inAskSaveChangesReply);
	}
}

//�ۑ��_�C�A���O�Ńt�@�C�����I�����ꂽ�Ƃ��̃R�[���o�b�N
void	AWindow::EVT_SaveWindowReply( const AObjectID inDocumentID, const AFileAcc& inFile, const AText& inRefText )
{
	EVTDO_SaveWindowReply(inDocumentID,inFile,inRefText);
}

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow::EVT_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )//win 080618
{
	if( NVI_IsWindowCreated() == false )   return;
	if( NVI_GetTabCount() == 0 )   return;//win Windows�̏ꍇ�^�u�����O��WM_SIZE������
	
	EVTDO_WindowBoundsChanged(inPrevBounds,inCurrentBounds);
	//win 080618
	//List�t���[����X�N���[���o�[�̈ʒu���AListView�̈ʒu�ɍ��킹�čX�V����iUSE_IMP_TABLEVIEW�̏ꍇ�͎��ۂɂ͏��������j
	/*#205 
	for( AIndex i = 0; i < mListFrameViewControlIDArray.GetItemCount(); i++ )
	{
		NVI_GetListFrameView(mListFrameViewControlIDArray.Get(i)).SPI_UpdateBounds();
	}
	*/
	//#138 StickyWindow�ʒu�X�V
	UpdateStickyWindowOffset();
}

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow::EVT_WindowResizeCompleted()
{
	if( NVI_IsWindowCreated() == false )   return;
	if( NVI_GetTabCount() == 0 )   return;//win Windows�̏ꍇ�^�u�����O��WM_SIZE������
	
	//#688
	//�O��̃T�C�Y�Ɠ������ǂ����̔���
	ARect	bounds = {0};
	NVI_GetWindowBounds(bounds);
	ABool	resized = ( mCurrentWindowWidth != bounds.right-bounds.left || mCurrentWindowHeight != bounds.bottom-bounds.top);
	//�h���N���X�ł̏���
	EVTDO_WindowResizeCompleted(resized);
	//����̃T�C�Y���L��
	mCurrentWindowWidth = bounds.right-bounds.left;
	mCurrentWindowHeight = bounds.bottom-bounds.top;
}

//win
/**
�E�C���h�EMinimize���̃R�[���o�b�N
*/
void	AWindow::EVT_WindowMinimized()
{
	EVTDO_WindowMinimized();
}

//win
/**
�E�C���h�EMinimize�������̃R�[���o�b�N
*/
void	AWindow::EVT_WindowMinimizeRestored()
{
	EVTDO_WindowMinimizeRestored();
}

//TableView�̃R�������̕ύX��Sort���̕ύX�Ȃǂ̏�ԕύX���̃R�[���o�b�N
void	AWindow::EVT_TableViewStateChanged( const AControlID inID )
{
	EVTDO_TableViewStateChanged(inID);
}

//#135
/**
Focus��Activate���ꂽ�Ƃ��ɃR�[�������
*/
void	AWindow::EVT_DoViewFocusActivated( const AControlID inID )
{
	//#312
	mLatentFocus = inID;
	//�^�u���̃A�N�e�B�u�t�H�[�J�X���X�V
	UpdateFocusInTab(inID);
	//�h���N���X�ł̏���
	EVTDO_DoViewFocusActivated(inID);
	//View�ł̏���
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoViewFocusActivated();
}

//#135
/**
Focus��Deactivate���ꂽ�Ƃ��ɃR�[�������
*/
void	AWindow::EVT_DoViewFocusDeactivated( const AControlID inID )
{
	//�h���N���X�ł̏���
	EVTDO_DoViewFocusDeactivated(inID);
	//View�ł̏���
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoViewFocusDeactivated();
}

//#353
/**
�J�[�\���L�[����
*/
ABool	AWindow::EVT_ArrowKey( const AControlID inID, const AUTF16Char inChar )
{
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	const ABoolArray&	focusGroup_IsGroup = GetFocusGroup_IsGroup();
	
	AIndex	focusIndex = focusGroup.Find(inID);
	if( focusIndex != kIndex_Invalid )
	{
		if( focusGroup_IsGroup.Get(focusIndex) == true )
		{
			switch(inChar)
			{
			  case 0x001C://left
			  case 0x001E://up
				{
					for( AIndex	index = focusIndex-1; index >= 0; index-- )
					{
						if( focusGroup_IsGroup.Get(index) == false )   break;
						
						AControlID	controlID = focusGroup.Get(index);
						if( IsFocusableControl(controlID) == true )
						{
							NVI_SwitchFocusTo(controlID);
							NVI_SetControlBool(controlID,true);
							return true;
							break;
						}
					}
					break;
				}
			  case 0x001D://right
			  case 0x001F://down
				{
					for( AIndex	index = focusIndex+1; index < focusGroup.GetItemCount(); index++ )
					{
						if( focusGroup_IsGroup.Get(index) == false )   break;
						
						AControlID	controlID = focusGroup.Get(index);
						if( IsFocusableControl(controlID) == true )
						{
							NVI_SwitchFocusTo(controlID);
							NVI_SetControlBool(controlID,true);
							return true;
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
	}
	return false;
}

#pragma mark ---View�s��

//Inline Input
ABool	AWindow::EVT_DoInlineInput( const AControlID inID, const AText& inText, const AItemCount inFixLen, 
		const AArray<AIndex>& inHiliteLineStyleIndex, const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
		const AItemCount inReplaceLengthInUTF16,//#688
		ABool& outUpdateMenu )
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
		EVT_DoInlineInput(sTextInputRedirect_ControlID,inText,
			inFixLen,inHiliteLineStyleIndex,inHiliteStartPos,inHiliteEndPos,inReplaceLengthInUTF16,outUpdateMenu);
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).
	EVT_DoInlineInput(inText,inFixLen,inHiliteLineStyleIndex,inHiliteStartPos,inHiliteEndPos,inReplaceLengthInUTF16,outUpdateMenu);
}

ABool	AWindow::EVT_DoInlineInputOffsetToPos( const AControlID inID, const AIndex inStartOffset, const AIndex inEndOffset, ALocalRect& outRect )//#1305
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
		EVT_DoInlineInputOffsetToPos(sTextInputRedirect_ControlID,inStartOffset,inEndOffset,outRect);//#1305
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoInlineInputOffsetToPos(inStartOffset,inEndOffset,outRect);//#1305
}

ABool	AWindow::EVT_DoInlineInputPosToOffset( const AControlID inID, const ALocalPoint& inPos, AIndex& outOffset )
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
				EVT_DoInlineInputPosToOffset(sTextInputRedirect_ControlID,inPos,outOffset);
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoInlineInputPosToOffset(inPos,outOffset);
}

ABool	AWindow::EVT_DoInlineInputGetSelectedText( const AControlID inID, AText& outText )
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
				EVT_DoInlineInputGetSelectedText(sTextInputRedirect_ControlID,outText);
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoInlineInputGetSelectedText(outText);
}

//�e�L�X�g����
ABool	AWindow::EVT_DoTextInput( const AControlID inID, const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction, 
		ABool& outUpdateMenu )
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
				EVT_DoTextInput(sTextInputRedirect_ControlID,inText,/*#688 inOSKeyEvent*/inKeyBindKey,inModifierKeys,inKeyBindAction,outUpdateMenu);
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoTextInput(inText,/*#688 inOSKeyEvent*/inKeyBindKey,inModifierKeys,inKeyBindAction,outUpdateMenu);
}

//�L�[����(raw)
ABool	AWindow::EVT_DoKeyDown( const AControlID inID, /*#688 const AUChar inChar*/ const AText& inText, const AModifierKeys inModifierKeys )//#1080
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
				EVT_DoKeyDown(sTextInputRedirect_ControlID,/*#688 inChar*/inText,inModifierKeys);
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoKeyDown(/*#688 inChar*/inText,inModifierKeys);
}

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AWindow::EVT_DoMouseDown( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).NVI_SwitchFocusTo(kControlID_Invalid);
		sTextInputRedirect_WindowID = kObjectID_Invalid;
		sTextInputRedirect_ControlID = kControlID_Invalid;
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoMouseDown(inLocalPoint,inModifierKeys,inClickCount);
}

//�}�E�X�{�^�������i�R���e�N�X�g���j���[�j���̃R�[���o�b�N(AView�p)
ABool	AWindow::EVT_DoContextMenu( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoContextMenu(inLocalPoint,inModifierKeys,inClickCount);
}

//�}�E�X�ړ����̃R�[���o�b�N(AView�p)
ABool	AWindow::EVT_DoMouseMoved( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//���݂̃}�E�X�ʒu�ƈقȂ�Ƃ��͉������Ȃ�
	AGlobalPoint	currentMouseGlobalPoint = {0};
	AWindow::NVI_GetCurrentMouseLocation(currentMouseGlobalPoint);
	AGlobalPoint	gpt = {0};
	NVI_GetGlobalPointFromControlLocalPoint(inID,inLocalPoint,gpt);
	if( gpt.x != currentMouseGlobalPoint.x || gpt.y != currentMouseGlobalPoint.y )
	{
		return true;
	}
	
	//�w���v�^�O�X�V
	NVI_UpdateHelpTag(inID,inLocalPoint,inModifierKeys,false);
	
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoMouseMoved(inLocalPoint,inModifierKeys);
}

//�c�[���`�b�v�E�C���h�E��\�����Ă���ΏۃR���g���[�����L�����邽�߂̃O���[�o���ϐ�
AGlobalRect		gToolTipControlAreaGlobalRect = {0};
AModifierKeys	gToolTipModifierKeys = kModifierKeysMask_None;

/**
*/
void	AWindow::NVI_UpdateHelpTag( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
								  const ABool inUpdateAlways )
{
	//==================�w���v�^�O�\��==================
	//rect�́A�w���v�^�O�`��ʒu�i�f�t�H���g�ŃR���g���[����rect�����Ă����BEVT_DoGetHelpTag()�Œl���ݒ肳���j
	ALocalRect	controlAreaLocalRect = {0};
	controlAreaLocalRect.right = NVI_GetControlWidth(inID);
	controlAreaLocalRect.bottom = NVI_GetControlHeight(inID);
	AText	text1, text2;
	//AWindow, AView�I�u�W�F�N�g����A���݂̃}�E�X�|�C���^�ɑΉ�����w���v�^�O�����擾
	AHelpTagSide	tagside = kHelpTagSide_Default;//#643
	ABool	showToolTip = EVT_DoGetHelpTag(inID,inLocalPoint,text1,text2,controlAreaLocalRect,tagside);
	if( showToolTip == false )
	{
		//AWindow, AView�I�u�W�F�N�g����EVT_DoGetHelpTag()�Ńw���v�^�O�擾�ł��Ȃ��ꍇ�́AAWindow::NVI_SetHelpTag()�ɂ��ݒ肳�ꂽ�f�[�^����擾
		AIndex	index = mToolTipTextArray_ControlID.Find(inID);
		if( index != kIndex_Invalid )
		{
			showToolTip = true;
			mToolTipTextArray_ToolTipText.Get(index,text1);
			text2.SetText(text1);
			tagside = mToolTipTextArray_HelpTagSide.Get(index);
		}
	}
	//�r���[��help tag��disable�̏ꍇ�́A�w���v�^�O��\���ɂ���
	if( IsView(inID) == true )
	{
		if( NVI_GetViewByControlID(inID).NVI_GetEnableHelpTag() == false )
		{
			showToolTip = false;
		}
	}
	//
	if( showToolTip == true )
	{
		//�R���g���[����global rect���擾
		AGlobalRect	controlAreaGlobalRect = {0};
		GetImp().GetGlobalRectFromControlLocalRect(inID,controlAreaLocalRect,controlAreaGlobalRect);
		//�����ꂩ��modifier�L�[�������Ă��邩�ǂ����Ńe�L�X�g��ς���
		AText	tooltipText;
		if( inModifierKeys == kModifierKeysMask_None )
		{
			tooltipText.SetText(text1);
		}
		else
		{
			tooltipText.SetText(text2);
		}
		
		if( tooltipText.GetItemCount() > 0 )
		{
			//------------------�w���v�^�O�\������ꍇ------------------
			//�c�[���`�b�v�E�C���h�E�Ƀe�L�X�g�ݒ�i����уT�C�Y�ݒ�j
			AApplication::GetApplication().NVI_GetToolTipWindow().SPI_SetToolTipText(tooltipText);
			//�c�[���`�b�v�E�C���h�E�̃T�C�Y�擾
			ANumber	w = AApplication::GetApplication().NVI_GetToolTipWindow().NVI_GetWindowWidth();
			ANumber	h = AApplication::GetApplication().NVI_GetToolTipWindow().NVI_GetWindowHeight();
			//�}�E�X�ʒu�擾
			AGlobalPoint	mouseGlobalPoint = {0};
			GetImp().GetGlobalPointFromControlLocalPoint(inID,inLocalPoint,mouseGlobalPoint);
			//�ΏۃR���g���[�����O��ƈႤ���ǂ����̔���
			if(	gToolTipControlAreaGlobalRect.left != controlAreaGlobalRect.left ||
			   gToolTipControlAreaGlobalRect.right != controlAreaGlobalRect.right ||
			   gToolTipControlAreaGlobalRect.top != controlAreaGlobalRect.top ||
			   gToolTipControlAreaGlobalRect.bottom != controlAreaGlobalRect.bottom ||
			   gToolTipModifierKeys != inModifierKeys ||
			   AApplication::GetApplication().NVI_GetToolTipWindow().NVI_IsWindowVisible() == false ||
			   inUpdateAlways == true )
			{
				//------------------�ΏۃR���g���[�����O��ƈႤ�̂ŁA�w���v�^�O�ʒu����------------------
				//�X�N���[���͈͎擾
				AGlobalRect	screenRect = {0};
				NVI_GetAvailableWindowBoundsInSameScreen(screenRect);
				//�c�[���`�b�v�E�C���h�E�ʒu�v�Z
				ARect	tooltipRect = {0};
				switch(tagside)
				{
				  case kHelpTagSide_Default:
					{
						//�R���g���[���̗̈������
						tooltipRect.left		= mouseGlobalPoint.x;
						tooltipRect.top			= controlAreaGlobalRect.bottom + 3;
						tooltipRect.right		= tooltipRect.left + w;
						tooltipRect.bottom		= tooltipRect.top + h;
						//
						APoint	pt = {tooltipRect.right,tooltipRect.bottom};
						if( PointInRect(pt,screenRect) == false )
						{
							//�R���g���[���̗̈������
							tooltipRect.left		= mouseGlobalPoint.x;
							tooltipRect.right		= tooltipRect.left + w;
							tooltipRect.bottom		= controlAreaGlobalRect.top - 3;
							tooltipRect.top			= tooltipRect.bottom - h;
						}
						break;
					}
				  case kHelpTagSide_Right:
					{
						//�R���g���[���̗̈�����E
						tooltipRect.left		= controlAreaGlobalRect.right + 3;
						tooltipRect.top			= controlAreaGlobalRect.bottom + 3;
						tooltipRect.right		= tooltipRect.left + w;
						tooltipRect.bottom		= tooltipRect.top + h;
						//
						APoint	pt = {tooltipRect.right,tooltipRect.bottom};
						if( PointInRect(pt,screenRect) == false )
						{
							pt.x = tooltipRect.right;
							pt.y = tooltipRect.top;
							if( PointInRect(pt,screenRect) == true )
							{
								//�E�オ�X�N���[�����Ȃ�A�R���g���[���̈�̏�ɕ\������
								tooltipRect.left		= mouseGlobalPoint.x;
								tooltipRect.right		= tooltipRect.left + w;
								tooltipRect.bottom		= controlAreaGlobalRect.top - 3;
								tooltipRect.top			= tooltipRect.bottom - h;
							}
							else
							{
								//�R���g���[���̗̈�̍��ɕ\������
								tooltipRect.right		= controlAreaGlobalRect.left - 3;
								tooltipRect.top			= controlAreaGlobalRect.bottom + 3;
								tooltipRect.left		= tooltipRect.right - w;
								tooltipRect.bottom		= tooltipRect.top + h;
							}
						}
						break;
					}
				  case kHelpTagSide_Top:
					{
						tooltipRect.left		= mouseGlobalPoint.x;
						tooltipRect.right		= tooltipRect.left + w;
						tooltipRect.bottom		= controlAreaGlobalRect.top - 3;
						tooltipRect.top			= tooltipRect.bottom - h;
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
				//�z�u�E�\��
				AApplication::GetApplication().NVI_GetToolTipWindow().NVI_SetWindowBounds(tooltipRect);
				AApplication::GetApplication().NVI_GetToolTipWindow().NVI_ConstrainWindowPosition(false);//�����܂����삵�Ă��Ȃ�
				AApplication::GetApplication().NVI_GetToolTipWindow().NVI_Show(false);
			}
		}
		else
		{
			//------------------�w���v�^�O�\�����Ȃ��ꍇ------------------
			//�w���v�^�O����
			AApplication::GetApplication().NVI_HideToolTip();
		}
		//����̑ΏۃR���g���[�����L��
		gToolTipControlAreaGlobalRect = controlAreaGlobalRect;
		gToolTipModifierKeys = inModifierKeys;
	}
}

//#688
/**
�E�C���h�E�ΏۂƂ���mouse moved����
*/
ABool	AWindow::EVT_DoMouseMoved( const AWindowPoint& inWindowPoint, const AModifierKeys inModifierKeys )
{
	return EVTDO_DoMouseMoved(inWindowPoint,inModifierKeys);
}

//�}�E�X�����ꂽ(AView�p)win 080712
void	AWindow::EVT_DoMouseLeft( const AControlID inID, const ALocalPoint& inLocalPoint )
{
	//�w���v�^�O����
	AApplication::GetApplication().NVI_HideToolTip();
	//
	if( IsView(inID) == false )   return;
	return NVI_GetViewByControlID(inID).EVT_DoMouseLeft(inLocalPoint);
}

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ACursorType	AWindow::EVT_GetCursorType( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return kCursorType_Arrow;
	return NVI_GetViewByControlID(inID).EVT_GetCursorType(inLocalPoint,inModifierKeys);
}

//�`��v�����̃R�[���o�b�N(AView�p)
void	AWindow::EVT_DoDraw( const AControlID inID )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoDraw();
}

//win 080712
//�`��O���������s����B�i�L�����b�g�ꎞ�����Ȃǁj
//EVT_DrawPreProcess()�́AWindows��WM_PAINT�C�x���g�ł̕`�掞�ɃR�[�������B
//�Ȃ��ANVI�̒��g�����ł���EVTDO_DrawPreProcess()�́A���̃��[�g�̂ق��ɁAEVT_DoDraw()������R�[�������B
void	AWindow::EVT_DrawPreProcess( const AControlID inID )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DrawPreProcess();
}

//win 080712
//�`��㏈�������s����B�i�L�����b�g�����Ȃǁj
//EVT_DrawPostProcess()�́AWindows��WM_PAINT�C�x���g�ł̕`�掞�ɃR�[�������B
//�Ȃ��ANVI�̒��g�����ł���EVTDO_DrawPreProcess()�́A���̃��[�g�̂ق��ɁAEVT_DoDraw()������R�[�������B
void	AWindow::EVT_DrawPostProcess( const AControlID inID )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DrawPostProcess();
}

//#138
//View�̕\���^��\���؂�ւ�
void	AWindow::EVT_PreProcess_SetShowControl( const AControlID inID, const ABool inVisible )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_PreProcess_SetShowControl(inVisible);
}

//�}�E�X�z�C�[���̃R�[���o�b�N(AView�p)
void	AWindow::EVT_DoMouseWheel( const AControlID inID, const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
			const ALocalPoint inLocalPoint )//win 080706
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoMouseWheel(inDeltaX,inDeltaY,inModifierKeys,inLocalPoint);
}

//�}�E�X�g���b�N�i�h���b�O�I�𓙁j�̃R�[���o�b�N(AView�p)
void	AWindow::EVT_DoMouseTracking( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoMouseTracking(inLocalPoint,inModifierKeys);
}

//�}�E�X�g���b�N�I���̃R�[���o�b�N(AView�p)
void	AWindow::EVT_DoMouseTrackEnd( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoMouseTrackEnd(inLocalPoint,inModifierKeys);
}

//
void	AWindow::EVT_DoControlBoundsChanged( const AControlID inID )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoControlBoundsChanged();
}

//#1026
//QuickLook�R�[���o�b�N(AView�p)
void	AWindow::EVT_QuickLook( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_QuickLook(inLocalPoint,inModifierKeys);
}

//Drag&Drop�R�[���o�b�N(AView�p)
void	AWindow::EVT_DoDragTracking( const AControlID inID, const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoDragTracking(inDragRef,inLocalPoint,inModifierKeys,outIsCopyOperation);
}
void	AWindow::EVT_DoDragEnter( const AControlID inID, const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoDragEnter(inDragRef,inLocalPoint,inModifierKeys);
}
void	AWindow::EVT_DoDragLeave( const AControlID inID, const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoDragLeave(inDragRef,inLocalPoint,inModifierKeys);
}
void	AWindow::EVT_DoDragReceive( const AControlID inID, const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoDragReceive(inDragRef,inLocalPoint,inModifierKeys);
}

//R240
//�w���v�^�O
ABool	AWindow::EVT_DoGetHelpTag( const AControlID inID, const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide )
{
	//#602 AWindow_�ł�helptag�ݒ�ɑΉ�
	if( EVTDO_DoGetHelpTag(inID,inPoint,outText1,outText2,outRect,outTagSide) == true )   return true;
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoGetHelpTag(inPoint,outText1,outText2,outRect,outTagSide);
}

//�X�N���[���o�[
void	AWindow::EVT_DoScrollBarAction( const AControlID inID, const AScrollBarPart inPart )
{
	/* #199 �I�u�W�F�N�g�擾��1��ł��܂���悤�ɕύX
	for( AIndex i = 0; i < mViewArray.GetItemCount(); i++ )
	{
		if( inID == mViewArray.GetObject(i).NVI_GetVScrollBarControlID() )
		{
			if( mViewArray.GetObject(i).NVI_IsVisible() == true )
			{
				mViewArray.GetObject(i).EVT_DoScrollBarAction(true,inPart);
			}
		}
		else if( inID == mViewArray.GetObject(i).NVI_GetHScrollBarControlID() )
		{
			if( mViewArray.GetObject(i).NVI_IsVisible() == true )
			{
				mViewArray.GetObject(i).EVT_DoScrollBarAction(false,inPart);
			}
		}
	}
	*/
	//�eView�Ƀ����N����V, H�X�N���[�����������AControlID����v������AView��ScrollBarAction�����s����B
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView&	view = AApplication::GetApplication().NVI_GetViewByID(mViewIDArray.Get(i));
		//�����X�N���[���o�[
		if( inID == view.NVI_GetVScrollBarControlID() )
		{
			if( view.NVI_IsVisible() == true )
			{
				view.EVT_DoScrollBarAction(true,inPart);
			}
		}
		//�����X�N���[���o�[
		else if( inID == view.NVI_GetHScrollBarControlID() )
		{
			if( view.NVI_IsVisible() == true )
			{
				view.EVT_DoScrollBarAction(false,inPart);
			}
		}
	}
	//�h���N���X�Ǝ��̏������s
	EVTDO_DoScrollBarAction(inID,inPart);
}

//
void	AWindow::EVT_DoTickTimer()
{
	if( NVI_IsWindowActive() == false )   return;//win 080618
	AControlID	focus = NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		if( IsView(focus) == true )//#135
		{
			NVI_GetViewByControlID(focus).EVT_DoTickTimer();
		}
	}
	//�h���N���X������
	EVTDO_DoTickTimerAction();
}

//#390
AWindowID	AWindow::sTextInputRedirect_WindowID = kObjectID_Invalid;
AControlID	AWindow::sTextInputRedirect_ControlID = kControlID_Invalid;

//#390
/**
*/
void	AWindow::STATIC_NVI_SetTextInputRedirect( const AWindowID inWindowID, const AControlID inControlID )
{
	sTextInputRedirect_WindowID = inWindowID;
	sTextInputRedirect_ControlID = inControlID;
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����
//�E�C���h�E����
void	AWindow::NVI_Create( const ADocumentID inDocumentID )
{
	//�E�C���h�E�������Ȃ琶������
	if( NVI_IsWindowCreated() == false )
	{
		//�E�C���h�E�����A�����l�ݒ�
		NVIDO_Create(inDocumentID);
		AText	initialTitle;
		GetImp().GetTitle(initialTitle);
		mWindowDefaultTitle.SetText(initialTitle);
		//�ŏ��́i�������͗B��́j�^�u����
		NVI_CreateTab(inDocumentID);
		//�^�u�I��
		NVI_SelectTab(0);
		//�E�C���h�E�ʒu����
		if( mWindowPositionDataID != kDataID_Invalid && NVM_GetOverlayMode() == false )//#291
		{
			APoint	pt;
			AApplication::GetApplication().NVI_GetAppPrefDB().GetData_Point(mWindowPositionDataID,pt);
			NVI_SetWindowPosition(pt);
			NVI_ConstrainWindowPosition(true);//#370 �����I�ɉB��Ă�OK���w��i#370�Ή��ȑO�Ɠ����j
		}
	}
}

//#182
/**
�E�C���h�E���̐���
*/
void	AWindow::NVM_CreateWindow( AConstCharPtr inWindowName, 
		const AWindowClass inWindowClass,//win
		const ABool inTabWindow, const AItemCount inTabCount )
{
	GetImp().Create(inWindowName,inWindowClass,inTabWindow,inTabCount);
}

//#182
/**
�E�C���h�E���̐����i��{�I��NVIDO_Create()����R�[�������j
*/
void	AWindow::NVM_CreateWindow( const AWindowClass inWindowClass, 
		const AOverlayWindowLayer inOverlayWindowLayer,//#688
		const ABool inHasCloseButton, const ABool inHasCollapseButton, 
		const ABool inHasZoomButton, const ABool inHasGrowBox, //#219
		const ABool inIgnoreClick, const ABool inHasShadow, const ABool inHasTitlebar )//#379 #688
{
	//#291 �e�E�C���h�E��ݒ�
	//�e�E�C���h�E�Ƃ̊֘A�Â����@��OS�ɂ���ĈقȂ�̂ŁACreate()���ōs��
	AWindowRef	parentWindowRef = NULL;
	if( NVM_GetOverlayMode() == true )
	{
		parentWindowRef = AApplication::GetApplication().NVI_GetWindowByID(mOverlayParentWindowID).NVI_GetWindowRef();
	}
	//����
	GetImp().Create(inWindowClass,inHasCloseButton,inHasCollapseButton,
				inHasZoomButton,inHasGrowBox,inIgnoreClick,inHasShadow,inHasTitlebar,parentWindowRef,inOverlayWindowLayer);//#291 #379 #688
}

//�E�C���h�E�\��
void	AWindow::NVI_Show( const ABool inSelect )
{
	//�E�C���h�E�������Ȃ烊�^�[��
	if( NVI_IsWindowCreated() == false )   return;
	
	if( NVI_IsWindowVisible() == false )
	{
		//�h���E�C���h�E���Ƃ̏���
		NVIDO_Show();
		//
		NVI_UpdateProperty();
		//�^�C�g���o�[�X�V
		NVI_UpdateTitleBar();
		//�E�C���h�E�\��
		GetImp().Show();
		//
		NVM_UpdateControlStatus();
		//#135
		NVI_SwitchFocusTo(mLatentFocus);
		//
		//#852 NVI_RefreshWindow();//win
	}
	//�I��
	if( inSelect == true )
	{
		NVI_SelectWindow();
	}
	mVisibleMode = true;//B0404
}

//�E�C���h�E�𐶐����ĕ\��
//inSelect: �E�C���h�E��I���i�őO�ʁj���邩�ǂ���
void	AWindow::NVI_CreateAndShow( const ABool inSelect )
{
	//�E�C���h�E�������Ȃ琶������
	if( NVI_IsWindowCreated() == false )
	{
		NVI_Create(kObjectID_Invalid);
	}
	NVI_Show(inSelect);
}

//�E�C���h�E��\��
void	AWindow::NVI_Hide()
{
	//�E�C���h�E�������Ȃ烊�^�[��
	if( NVI_IsWindowCreated() == false )   return;
	
	if( NVI_IsWindowVisible() == true )
	{
		//�E�C���h�E�ʒu�̕ۑ�
		if( mWindowPositionDataID != kDataID_Invalid && NVM_GetOverlayMode() == false )//#291
		{
			APoint	pt;
			NVI_GetWindowPosition(pt);
			AApplication::GetApplication().NVI_GetAppPrefDB().SetData_Point(mWindowPositionDataID,pt);
		}
		
		//�h���E�C���h�E���Ƃ̏���
		NVIDO_Hide();
		
		//�E�C���h�E��\��
		GetImp().Hide();
		
		//DB�A������
		if( IsDBDataTableExist() == true )
		{
			NVM_StoreToDBAll();
			//DB�̃f�[�^���t�@�C���֏�������
			NVM_GetDB().SaveToFile();
		}
	}
	//B0000 �A�v���P�[�V�����f�A�N�e�B�x�[�g���ɃR�[�������ƕ���Ȃ��̂ŁAFloating�E�C���h�E�͓��ꏈ��
	//�i���L������if�O�̋��ʏ����ɂ��Ă��ǂ��C�����邪�A�e���x���s���Ȃ̂œ��ʂ���ŁE�E�E�j
	else if( NVI_IsFloating() == true )
	{
		//�h���E�C���h�E���Ƃ̏���
		NVIDO_Hide();
		
		//�E�C���h�E��\��
		GetImp().Hide();
	}
	mVisibleMode = false;//B0404
	//�w���v�^�O����
	AApplication::GetApplication().NVI_HideToolTip();
}

//�E�C���h�E��\���{�폜
void	AWindow::NVI_Close()
{
	//�E�C���h�E�������Ȃ烊�^�[��
	if( NVI_IsWindowCreated() == false )   return;
	
	//�q�E�C���h�E��S�ĕ��� #291
	NVI_CloseChildWindow();
	//�E�C���h�E��\��
	NVI_Hide();
	//�^�u/View/Control�̍폜 #92
	while( NVI_GetTabCount() > 0 )
	{
		NVI_DeleteTabAndView(0);
	}
	//�c���View��S�č폜���� #92
	while( mViewControlIDArray.GetItemCount() > 0 )
	{
		NVI_DeleteControl(mViewControlIDArray.Get(0));//win NVI_DeleteView()��mTabDataArray_ControlIDs����̍폜�����s��Ȃ��̂�NG
	}
	//�E�C���h�E�폜
	GetImp().CloseWindow();
	//#135
	mFocusGroup.DeleteAll();
	mFocusGroup_IsGroup.DeleteAll();//#353
	//#688 mFocusGroup2nd.DeleteAll();//#353
	//#688 mFocusGroup2nd_IsGroup.DeleteAll();//#353
	//#349
	mRadioGroup.DeleteAll();
	mRadioGroup_All.DeleteAll();
	//
	NVIDO_Close();
	//�^�u�f�[�^�̍폜 B0000
	/*#92 mTabDataArray_ControlIDs�ɃZ�b�g���ꂽView/Control���폜���ׂ��Ȃ̂ŁANVI_DeleteTabAndView()���R�[��������@�ɕύX
	mTabDataArray_ControlIDs.DeleteAll();
	mTabDataArray_mWindowMenuShortcut.DeleteAll();
	mTabDataArray_DocumentID.DeleteAll();
	mTabDataArray_LatentFocus.DeleteAll();
	mTabZOrder.DeleteAll();
	mCurrentTabIndex = kIndex_Invalid;
	*/
	//B0000
	//#135 mCurrentFocus = kControlID_Invalid;
	mLatentFocus = kControlID_Invalid;//#135
}

//�E�C���h�E�I��
void	AWindow::NVI_SelectWindow()
{
	GetImp().Select();
}

/**
�E�C���h�E��w��Ɉړ�
@param inWindowID ��O�̃E�C���h�E�BkObjectID_Invalid�Ȃ�Ŕw�ʂɈړ�
*/
void	AWindow::NVI_SendBehind( const AWindowID inWindowID )
{
	if( inWindowID == kObjectID_Invalid )
	{
		GetImp().SendBehindAll();//#567
	}
	else
	{
		GetImp().SendBehind(AApplication::GetApplication().NVI_GetWindowByID(inWindowID).GetImp());
	}
}

//#688
/**
�E�C���h�E��O�ʂɈړ�
@param inWindowID ��O�̃E�C���h�E�BkObjectID_Invalid�Ȃ�Ŕw�ʂɈړ�
*/
void	AWindow::NVI_SendAbove( const AWindowID inWindowID )
{
	if( inWindowID == kObjectID_Invalid )
	{
		//�őO�ʂɈړ�
		GetImp().SendAboveAll();
	}
	else
	{
		//inWindowID�̑O�ʂɈړ�
		GetImp().SendAbove(AApplication::GetApplication().NVI_GetWindowByID(inWindowID).GetImp());
	}
}

//#688
/**
�q�I�[�o�[���C�E�C���h�E��z�������E�C���h�E�������Ɏw�肵��AOverlayWindowLayer�ɏ]���čĔz�u
*/
void	AWindow::NVI_ReorderOverlayChildWindows()
{
	GetImp().ReorderOverlayChildWindows();
}

//�E�C���h�E�S�̂̕\�����X�V
void	AWindow::NVI_UpdateProperty()
{
	//�E�C���h�E�������Ȃ烊�^�[�� #402
	if( NVI_IsWindowCreated() == false )   return;
	
	if( IsDBDataTableExist() == true )
	{
		UpdateDBDataAndTable();
	}
	NVIDO_UpdateProperty();
	NVM_UpdateControlStatus();
}

//�E�C���h�E���t���b�V��
void	AWindow::NVI_RefreshWindow()
{
	if( NVI_IsWindowCreated() == false )   return;//#0 �E�C���h�E���������̓��t���b�V���ł��Ȃ��̂ŉ����������^�[��
	GetImp().RefreshWindow();
}

void	AWindow::NVI_SetWindowBounds( const ARect& inBounds, const ABool inAnimate )//#688
{
	//���݂�bounds�Ɠ����Ȃ牽�������I��
	ARect	oldBounds = {0};
	GetImp().GetWindowBounds(oldBounds);
	if( inBounds.left == oldBounds.left && inBounds.top == oldBounds.top && 
				inBounds.right == oldBounds.right && inBounds.bottom == oldBounds.bottom )
	{
		return;
	}
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(false);
	}
	//
	GetImp().SetWindowBounds(inBounds,inAnimate);//#688
	//�T�C�Y�ύX�r���łȂ���΁A���T�C�Y�������������s�iAView_SizeBox�N���X������NVI_SetVirtualLiveResizing()�ɂ��T�C�Y�ύX���s���ݒ肳���j
	if( mVirtualLiveResizing == false )
	{
		EVT_WindowResizeCompleted();//B0415
	}
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(true);
	}
}

void	AWindow::NVI_GetWindowBounds( ARect& outBounds ) const
{
	GetImpConst().GetWindowBounds(outBounds);
}

void	AWindow::NVI_SetWindowPosition( const APoint& inPoint )
{
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(false);
	}
	//
	GetImp().SetWindowPosition(inPoint);
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(true);
	}
}

void	AWindow::NVI_GetWindowPosition( APoint& outPoint ) const
{
	GetImpConst().GetWindowPosition(outPoint);
}

//�E�C���h�E�T�C�Y�ݒ�
void	AWindow::NVI_SetWindowSize( const ANumber inWidth, const ANumber inHeight )
{
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(false);
	}
	//
	GetImp().SetWindowSize(inWidth,inHeight);
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(true);
	}
}

//#182
/**
�E�C���h�E���ݒ�
*/
void	AWindow::NVI_SetWindowWidth( const ANumber inWidth )
{
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(false);
	}
	//
	GetImp().SetWindowWidth(inWidth);
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(true);
	}
}

//#182
/**
�E�C���h�E�����ݒ�
*/
void	AWindow::NVI_SetWindowHeight( const ANumber inHeight )
{
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(false);
	}
	//
	GetImp().SetWindowHeight(inHeight);
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(true);
	}
}

//�E�C���h�E���擾
ANumber	AWindow::NVI_GetWindowWidth() const
{
	return GetImpConst().GetWindowWidth();
}

//�E�C���h�E�����擾
ANumber	AWindow::NVI_GetWindowHeight() const
{
	return GetImpConst().GetWindowHeight();
}

//B0395
void	AWindow::NVI_CollapseWindow( const ABool inCollapse )
{
	GetImp().CollapseWindow(inCollapse);
}

//#92
/**

*/
void	AWindow::NVI_TryClose( const AIndex inTabIndex )//#164
{
	if( mInhibitClose == true )   return;
	
	NVIDO_TryClose(inTabIndex);//#164
}

/**

*/
void	AWindow::NVI_ExecuteClose()
{
	if( mInhibitClose == true )   return;
	
	NVIDO_ExecuteClose();
}

/**
�e�^�u�ɂ��āA�ۑ��m�F�_�C�A���O��\��
*/
ABool	AWindow::NVI_AskSaveForAllTabs()
{
	return NVIDO_AskSaveForAllTabs();
}

//#182
/**
�E�C���h�E��Sheet�E�C���h�E�ɐݒ肷��
*/
void	AWindow::NVI_SetWindowStyleToSheet( const AWindowID inParentWindowID )
{
	GetImp().SetWindowStyleToSheet(AApplication::GetApplication().NVI_GetWindowByID(inParentWindowID).mWindowImp);
}

//#182
/**
�E�C���h�E��Drawer�E�C���h�E�ɐݒ肷��
*/
/*#688
void	AWindow::NVI_SetWindowStyleToDrawer( const AWindowID inParentWindowID )
{
	GetImp().SetWindowStyleToDrawer(AApplication::GetApplication().NVI_GetWindowByID(inParentWindowID).mWindowImp);
}
*/

/*win
//#182
**
�E�C���h�E���t���[�e�B���O�ɂ���
*
void	AWindow::NVI_SetFloating()
{
	GetImp().SetFloating();
}
*/

//#182
/**
�E�C���h�E���f�X�N�g�b�v���Ɏ��܂�悤�ɔz�u����
*/
void	AWindow::NVI_ConstrainWindowPosition( const ABool inAllowPartial )//#370
{
	GetImp().ConstrainWindowPosition(inAllowPartial);//#370
}

/**
�w��|�C���g���f�X�N�g�b�v�����ǂ����𔻒�
*/
void	AWindow::NVI_ConstrainWindowPosition( const ABool inAllowPartial, APoint& ioPoint ) const
{
	GetImpConst().ConstrainWindowPosition(inAllowPartial,ioPoint);
}

//#385
/**
�E�C���h�E���f�X�N�g�b�v���Ɏ��܂�悤�Ƀ��T�C�Y����
*/
void	AWindow::NVI_ConstrainWindowSize()
{
	GetImp().ConstrainWindowSize();
}

#if IMPLEMENTATION_FOR_WINDOWS
//#182
/**
�T�C�Y�{�b�N�X����
*/
void	AWindow::NVI_CreateSizeBox( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	GetImp().CreateSizeBox(inID,inPoint,inWidth,inHeight);
}

//#182
/**
FileOpenDialog��\������iWindows�p�j
*/
void	AWindow::NVI_ShowFileOpenWindow( const AText& inFilter, const ABool inShowInvisibleFile )
{
	GetImp().ShowFileOpenWindow(inFilter,inShowInvisibleFile);
}
#endif

//#291
/**
Overlay���[�h�łȂ���΃E�C���h�E��Show
*/
void	AWindow::NVI_ShowIfNotOverlayMode()
{
	if( NVM_GetOverlayMode() == false )
	{
		NVI_Show(false);
	}
}

//#291
/**
Overlay���[�h�łȂ���΃E�C���h�E��Hide
*/
void	AWindow::NVI_HideIfNotOverlayMode()
{
	if( NVM_GetOverlayMode() == false )
	{
		NVI_Hide();
	}
}

//#409
/**
�E�C���h�E�𖾎��I�Ɂi�����j�X�V
*/
void	AWindow::NVI_UpdateWindow()
{
	GetImp().UpdateWindow();
}

//win
/**
*/
void	AWindow::NVI_Zoom()
{
	GetImp().Zoom();
}

//#404
/**
�t���X�N���[�����[�h�ݒ�
*/
void	AWindow::NVI_SetFullScreenMode( const ABool inFullScreenMode )
{
	GetImp().SetFullScreenMode(inFullScreenMode);
}

//#404
/**
�t���X�N���[�����[�h��Ԏ擾
*/
ABool	AWindow::NVI_GetFullScreenMode() const
{
	return GetImpConst().GetFullScreenMode();
}

//#505
/**
�E�C���h�E�̍ŏ��T�C�Y�ݒ�
*/
void	AWindow::NVI_SetWindowMinimumSize( const ANumber inWidth, const ANumber inHeight )
{
	GetImp().SetWindowMinimumSize(inWidth,inHeight);
}

//#505
/**
�E�C���h�E�̍ő�T�C�Y�ݒ�
*/
void	AWindow::NVI_SetWindowMaximumSize( const ANumber inWidth, const ANumber inHeight )
{
	GetImp().SetWindowMaximumSize(inWidth,inHeight);
}

#pragma mark ===========================

#pragma mark ---�^�u����

//�^�u����
void	AWindow::NVI_CreateTab( const ADocumentID inDocumentID, const ABool inSelectableTab )
{
	//#291 �E�C���h�E�������Ȃ琶��
	if( NVI_IsWindowCreated() == false )
	{
		NVI_Create(inDocumentID);//���̒���NVI_CreateTab()���R�[�������
		return;
	}
	
	AIndex	tabIndex = mTabDataArray_ControlIDs.AddNewObject();
	mTabDataArray_mWindowMenuShortcut.Add(kIndex_Invalid);
	mTabDataArray_DocumentID.Add(inDocumentID);
	mTabDataArray_LatentFocus.Add(kControlID_Invalid);
	mTabDataArray_LatentVisible.AddNewObject();//#212
	mTabDataArray_TabSelectable.Add(inSelectableTab);//#379
	if( inSelectableTab == true )//#379
	{
		if( mTabZOrder.GetItemCount() > 0 )
		{
			//B0411 �ŏ��̃^�u�ȊO�̏ꍇ�́A2�Ԗڂ�Zorder�փ^�u��}������
			mTabZOrder.Insert1(1,tabIndex);
		}
		else
		{
			//�őO�ʂɃ^�u��}��
			mTabZOrder.Insert1(0,tabIndex);
		}
		//���݃^�u���ݒ�Ȃ�A���������^�u�����݃^�u�ɐݒ肷��B
		//�{����mCurrentTabIndex�́ANVI_SelectTab()�ɐݒ肳���ׂ������A
		//NVIDO_CreateTab()���ɂ�NVI_GetCurrentDocumentID()�����g�p�����critical throw���ɂȂ��Ă��܂��̂ŁA
		//�����Ō��݃^�u��ݒ肷������x�^�[�BmTabDataArray_XXX�͐ݒ�ς݂Ȃ̂ŁA��{�I�ɖ��͖����͂��B #963
		if( mCurrentTabIndex == kIndex_Invalid )
		{
			mCurrentTabIndex = tabIndex;
		}
	}
	//�h���N���X���Ƃ̏���
	AControlID	initialFocus = kControlID_Invalid;
	NVIDO_CreateTab(tabIndex,initialFocus);
	//B0411 �ŏ���tab�łȂ��ꍇ��invisible�ɂ��Ă���
	if( mTabDataArray_ControlIDs.GetItemCount() > 1 )
	{
		for( AIndex viewIndex = 0; viewIndex < mTabDataArray_ControlIDs.GetObject(tabIndex).GetItemCount(); viewIndex++ )
		{
			NVI_SetShowControl(mTabDataArray_ControlIDs.GetObject(tabIndex).Get(viewIndex),false);
		}
	}
	//�����t�H�[�J�X�̐ݒ�i���������^�u��SelectTab()���ꂽ�Ƃ��ɃE�C���h�E���t�H�[�J�X�ɂȂ�B�j
	mTabDataArray_LatentFocus.Set(tabIndex,initialFocus);
	//�ŏ��̃^�u�����̏ꍇ�Ainitial�t�H�[�J�X�Ƀt�H�[�J�X�ړ�
	if( inSelectableTab == true )
	{
		if( mTabZOrder.GetItemCount() == 1 )
		{
			//�t�H�[�J�X�ݒ�
			NVI_SwitchFocusTo(initialFocus);
		}
	}
}

//�^�u�A����сA�^�u�������r���[�̍폜
void	AWindow::NVI_DeleteTabAndView( const AIndex inTabIndex )
{
	//�t�H�[�J�X����������iNVI_SelectTab()�܂ł̊ԂɁA�t�H�[�J�XView���擾����邱�ƂŁA���łɍ폜����View���g�p���邱�ƂɂȂ��Ă��܂��̂�h�����߁j
	//�^�u�؂�ւ��ɂȂ�ꍇ�́ANVI_SelectTab()�ŐV�����t�H�[�J�X���ݒ肳���B
	NVI_SwitchFocusTo(kControlID_Invalid);
	//�h���N���X���Ƃ̏���
	NVIDO_DeleteTab(inTabIndex);
	//����r���[�̍폜
	/*#243 NVI_DeleteControl()������mTabDataArray_ControlIDs����̓o�^����������悤�ɕύX�����̂ŁA�����������Ȃ��悤�A��UControlID�����[�J���Ɉڂ�
	for( AIndex index = 0; index < mTabDataArray_ControlIDs.GetObject(inTabIndex).GetItemCount(); index++ )
	{
		NVI_DeleteControl(mTabDataArray_ControlIDs.GetObject(inTabIndex).Get(index));
	}
	*/
	//�^�u���̃R���g���[��ID��S��controlIDArray�փR�s�[����B�i���Ŏ��ۂɍ폜����j
	AArray<AControlID>	controlIDArray;
	for( AIndex index = 0; index < mTabDataArray_ControlIDs.GetObject(inTabIndex).GetItemCount(); index++ )
	{
		controlIDArray.Add(mTabDataArray_ControlIDs.GetObject(inTabIndex).Get(index));
	}
	/*#688 ���ֈړ��i���݂̃^�uindex�A�^�u�f�[�^���폜��̐���ȏ�Ԃɂ�����ŁA�R���g���[���̍폜���s���j
	for( AIndex i = 0; i < controlIDArray.GetItemCount(); i++ )
	{
		AControlID	controlID = controlIDArray.Get(i);
		//���łɂق���Control�폜���ɓ����폜����Ă���\��������̂ŁA���݃`�F�b�N���s���Ă���폜
		if( mTabDataArray_ControlIDs.GetObjectConst(inTabIndex).Find(controlID) != kIndex_Invalid )
		{
			NVI_DeleteControl(controlID);
		}
	}
	*/
	
	//#725
	//�o�^�I�[�o�[���C�E�C���h�E�̂����A���Ytab�ɑ�������̂��폜����
	AArray<AWindowID>	deletedOverlayWindowIDArray;
	for( AIndex i = 0; i < mChildOverlayWindowArray_TabIndex.GetItemCount(); i++ )
	{
		AIndex	tabIndex = mChildOverlayWindowArray_TabIndex.Get(i);
		if( tabIndex == inTabIndex )
		{
			//���Ytab���Ȃ�A���̃E�C���h�E���폜����B
			AWindowID	overlayWindowID = mChildOverlayWindowArray_WindowID.Get(i);
			AApplication::GetApplication().NVI_DeleteWindow(overlayWindowID);
			deletedOverlayWindowIDArray.Add(overlayWindowID);
		}
		else if( tabIndex > inTabIndex )
		{
			//���Ytab�O�ŁA�폜tab�ȍ~�Ȃ�AtabIndex���f�N�������g����
			mChildOverlayWindowArray_TabIndex.Set(i,tabIndex-1);
		}
	}
	//��ō폜�������̂��q�I�[�o�[���C�E�C���h�E�o�^����o�^��������
	for( AIndex i = 0; i < deletedOverlayWindowIDArray.GetItemCount(); i++ )
	{
		NVM_UnregisterOverlayWindow(deletedOverlayWindowIDArray.Get(i));
	}
	
	//�^�u�f�[�^�̍폜
	mTabDataArray_ControlIDs.Delete1Object(inTabIndex);
	mTabDataArray_mWindowMenuShortcut.Delete1(inTabIndex);
	mTabDataArray_DocumentID.Delete1(inTabIndex);
	mTabDataArray_LatentFocus.Delete1(inTabIndex);
	mTabDataArray_LatentVisible.Delete1Object(inTabIndex);//#212
	mTabDataArray_TabSelectable.Delete1(inTabIndex);//#379
	AIndex	tabZOrderIndex = mTabZOrder.Find(inTabIndex);
	if( tabZOrderIndex != kIndex_Invalid )//#379
	{
		mTabZOrder.Delete1(tabZOrderIndex);
	}
	//TabZOrder�z��ɂ��āA����폜�����^�u�ȍ~��Index���P�O�ɂ��炷
	for( AIndex i = 0; i < mTabZOrder.GetItemCount(); i++ )
	{
		if( mTabZOrder.Get(i) > inTabIndex )
		{
			mTabZOrder.Set(i,mTabZOrder.Get(i)-1);
		}
	}
	
	//#663 ���݃A�N�e�B�u�ȃ^�u���폜�����ꍇ�̂݁A�V���ȃ^�u��I��������
	if( inTabIndex == mCurrentTabIndex )
	{
		mCurrentTabIndex = kIndex_Invalid;
		
		//�^�u�I���iZOrder�őO�ʂ̃^�u�j
		if( NVI_GetSelectableTabCount() > 0 )
		{
			NVI_SelectTab(mTabZOrder.Get(0));
		}
	}
	else
	{
		//�A�N�e�B�u�łȂ��^�u���폜�����ꍇ�̏���#663
		//mCurrentTabIndex���폜�����^�u����Ȃ�A�C���f�b�N�X�l�̕␳������
		if( mCurrentTabIndex > inTabIndex )
		{
			mCurrentTabIndex--;
		}
	}
	//#688 �ォ��ړ��i���݂̃^�uindex�A�^�u�f�[�^���폜��̐���ȏ�Ԃɂ�����ŁA�R���g���[���̍폜���s���j
	//�^�u���̃R���g���[�����폜
	for( AIndex i = 0; i < controlIDArray.GetItemCount(); i++ )
	{
		AControlID	controlID = controlIDArray.Get(i);
		//���łɂق���Control�폜���ɓ����폜����Ă���\��������iframe view�̏ꍇ�A���C��view�폜���ɓ����폜�����j�̂ŁA
		//���݃`�F�b�N���s���Ă���폜
		if( GetImp().ExistControl(controlID) == true )
		{
			NVI_DeleteControl(controlID);
		}
	}
	//#663 �^�u�폜��̊e�E�C���h�E����
	if( NVI_GetTabCount() > 0 )
	{
		NVIDO_TabDeleted();
	}
}

//�E�C���h�E�I���{�^�u�I��
void	AWindow::NVI_SelectTab( const AIndex inTabIndex )
{
	if( inTabIndex == mCurrentTabIndex/*B0415 && inForRedraw == false*/ )   return;
	//���݂�Tab��Deactivated��ʒm
	if( mCurrentTabIndex != kIndex_Invalid )
	{
		NVM_DoTabDeactivated(mCurrentTabIndex);
	}
	//����Tab�X�V
	mCurrentTabIndex = inTabIndex;
	//ZOrder�X�V
	mTabZOrder.Delete1(mTabZOrder.Find(mCurrentTabIndex));
	mTabZOrder.Insert1(0,mCurrentTabIndex);
	//Tab�����ꂩ��Activate����邱�Ƃ�ʒm win ShowControl()�����O�̏�������(SPI_DoViewActivated()�����s������)
	NVM_DoTabActivating(inTabIndex);
	//#1091
	//�^�u���t�H�[�J�X���ݒ�ς݂Ȃ�A������t�H�[�J�X�ɐݒ肷��B�i�^�u����View�̂����A�Ō�Ƀt�H�[�J�X����Ă������̂��A�^�u���t�H�[�J�X�Ƃ��Đݒ肳��Ă���j
	//�i�ȑO�̓^�uView�̕\������������Ȃ����̂��Ƀt�H�[�J�X�ݒ肵�Ă������AMac OS X 10.10�ł́A���݂̃t�H�[�J�X��hide����ƁA
	//�t�H�[�J�X������Ɍ����t�B�[���h�Ɉړ�����H�Ǝv���邽�߁Ahide����O�ɁA����t�H�[�J�X�ɂȂ�ׂ�view�͐��show����B�j
	AControlID	tabFocus = NVI_GetFocusInTab(mCurrentTabIndex);
	if( tabFocus != kControlID_Invalid )
	{
		//View��\������
		NVI_SetShowControl(tabFocus,true);
		//�t�H�[�J�X��ݒ肷��
		NVI_SwitchFocusTo(tabFocus);
	}
	//�^�uView��\������
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray_ControlIDs.GetItemCount(); tabIndex++ )
	{
		for( AIndex viewIndex = 0; viewIndex < mTabDataArray_ControlIDs.GetObject(tabIndex).GetItemCount(); viewIndex++ )
		{
			ABool	visible = (tabIndex==mCurrentTabIndex);
			//#212 LatentVisible�Ŗ����ꍇ�͕\�����Ȃ�
			if( mTabDataArray_LatentVisible.GetObjectConst(tabIndex).Get(viewIndex) == false )
			{
				visible = false;
			}
			//�\���ݒ�
			NVI_SetShowControl(mTabDataArray_ControlIDs.GetObject(tabIndex).Get(viewIndex),visible);
		}
	}
	//#725
	//�o�^�q�I�[�o�[���C�E�C���h�E��show/hide����
	for( AIndex windowIndex = 0; windowIndex < mChildOverlayWindowArray_WindowID.GetItemCount(); windowIndex++ )
	{
		AIndex	tabIndex = mChildOverlayWindowArray_TabIndex.Get(windowIndex);
		//�^�u�ɑ����Ȃ��ꍇ�͉������Ȃ��B
		if( tabIndex == kIndex_Invalid )
		{
			continue;
		}
		//����I�����ꂽ�^�u�ɑ�����I�[�o�[���C�E�C���h�E�̏ꍇ�A���Alatent visible�̏ꍇ�Ashow����B����ȊO��hide�B
		AWindowID	winID = mChildOverlayWindowArray_WindowID.Get(windowIndex);
		ABool	latentVisible = mChildOverlayWindowArray_LatentVisible.Get(windowIndex);
		if( tabIndex == mCurrentTabIndex && latentVisible == true )
		{
			AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_Show(false);
		}
		else
		{
			AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_Hide();
		}
	}
	//�^�u���t�H�[�J�X���ݒ�ς݂Ȃ�A������t�H�[�J�X�ɐݒ肷��B�i�^�u����View�̂����A�Ō�Ƀt�H�[�J�X����Ă������̂��A�^�u���t�H�[�J�X�Ƃ��Đݒ肳��Ă���j
	//�i#1091�ɂ���Ńt�H�[�J�X�ݒ肵�Ă��邪�A�ēx�ݒ肵�Ă�����p�͂Ȃ��͂��Ȃ̂ŁA������ł��O�̂��ߐݒ肷��j
	tabFocus = NVI_GetFocusInTab(mCurrentTabIndex);
	if( tabFocus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(tabFocus);
	}
	//#725 ��������ȊO��view��show/hide�Ȃǂ̂��߁A�h���N���X�ŏ��������s
	NVIDO_SelectTab(inTabIndex);
	//�V�KTab��Activated��ʒm
	NVM_DoTabActivated(inTabIndex,true);//win 080712
}

void	AWindow::NVI_SetTabZOrder( const AIndex inTabIndex, const AIndex inZOrder )
{
	if( inZOrder == 0 ) 
	{
		NVI_SelectTab(inTabIndex);
	}
	else
	{
		//ZOrder�X�V
		AIndex	oldZOrder = mTabZOrder.Find(inTabIndex);
		mTabZOrder.Delete1(oldZOrder);
		mTabZOrder.Insert1(inZOrder,inTabIndex);
		//
		if( oldZOrder == 0 )
		{
			NVI_SelectTab(mTabZOrder.Get(0));
		}
		//
		NVI_RefreshWindow();
	}
}

AIndex	AWindow::NVI_GetTabZOrder( const AIndex inTabIndex ) const
{
	return mTabZOrder.Find(inTabIndex);
}

//TabIndex����h�L�������gID���擾
ADocumentID	AWindow::NVI_GetDocumentIDByTabIndex( const AIndex inTabIndex ) const
{
	return mTabDataArray_DocumentID.Get(inTabIndex);
}

//�h�L�������gID����TabIndex���擾
AIndex	AWindow::NVI_GetTabIndexByDocumentID( const AObjectID inDocumentID ) const
{
	return mTabDataArray_DocumentID.Find(inDocumentID);
}

//
void	AWindow::NVI_SelectTabByDocumentID( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	if( tabIndex != kIndex_Invalid )
	{
		NVI_SelectTab(tabIndex);
	}
}

//#92
/**
�w��Control������Tab��I������
*/
void	AWindow::NVI_SelectTabByIncludingControlID( const AControlID inControlID )
{
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray_ControlIDs.GetItemCount(); tabIndex++ )
	{
		if( mTabDataArray_ControlIDs.GetObject(tabIndex).Find(inControlID) != kIndex_Invalid )
		{
			NVI_SelectTab(tabIndex);
			return;
		}
	}
}

//Tab��View�ǉ�
void	AWindow::NVM_RegisterViewInTab( const AIndex inTabIndex, const AControlID inControlID )
{
	mTabDataArray_ControlIDs.GetObject(inTabIndex).Add(inControlID);
	mTabDataArray_LatentVisible.GetObject(inTabIndex).Add(true);//#212
	//View��FrameView�����݂���ꍇ��FrameView������Tab�Ɋi�[���� #135
	AControlID	frameControlID = kControlID_Invalid;
	if( IsView(inControlID) == true )
	{
		frameControlID = NVI_GetViewByControlID(inControlID).NVI_GetFrameViewControlID();
	}
	if( frameControlID != kControlID_Invalid )
	{
		mTabDataArray_ControlIDs.GetObject(inTabIndex).Add(frameControlID);
		mTabDataArray_LatentVisible.GetObject(inTabIndex).Add(true);//#212
	}
	//win
	if( inTabIndex != mCurrentTabIndex )
	{
		NVI_SetShowControl(inControlID,false);
	}
}

//Tab��View�ǉ�
void	AWindow::NVM_UnregisterViewInTab( const AIndex inTabIndex, const AControlID inControlID )
{
	AIndex	index = mTabDataArray_ControlIDs.GetObject(inTabIndex).Find(inControlID);
	if( index == kIndex_Invalid )   _ACThrow("not found controlID",this);
	mTabDataArray_ControlIDs.GetObject(inTabIndex).Delete1(index);
	mTabDataArray_LatentVisible.GetObject(inTabIndex).Delete1(index);//#212
}

//#212
/**
Tab�\�������Ƃ���Control�\�����邩�ǂ�����ݒ肷��(LatentVisible)
*/
void	AWindow::NVI_SetLatentVisible( const AIndex inTabIndex, const AControlID inControlID, const ABool inLatentVisible )
{
	AIndex	index = mTabDataArray_ControlIDs.GetObject(inTabIndex).Find(inControlID);
	if( index == kIndex_Invalid )   _ACThrow("not found controlID",this);
	mTabDataArray_LatentVisible.GetObject(inTabIndex).Set(index,inLatentVisible);
}

//win
/**
Tab Activate��������
*/
void	AWindow::NVM_DoTabActivating( const AIndex inTabIndex )
{
	NVMDO_DoTabActivating(inTabIndex);
}

//Tab Activated���̏���
//inTabSwitched: true: �^�u���؂�ւ����  false: �^�u�͐؂�ւ�炸�E�C���h�E��activate���ꂽ
void	AWindow::NVM_DoTabActivated( const AIndex inTabIndex, const ABool inTabSwitched )//win 080712
{
	if( NVI_IsWindowVisible() == true )
	{
		//
		NVMDO_DoTabActivated(inTabIndex,inTabSwitched);//win 080712
		/*#135 DoViewFocusActivated()��NVI_SelectTab()����NVI_SwitchFocusTo()�o�R�Ŏ��s����悤�ɕύX
		//
		AControlID	focus = mTabDataArray_LatentFocus.Get(inTabIndex);
		if( ExistView(focus) == true )
		{
			NVI_GetViewByControlID(focus).NVM_DoViewFocusActivated();
		}
		*/
		//
		NVI_UpdateTitleBar();
	}
}

//Tab Deactivated
void	AWindow::NVM_DoTabDeactivated( const AIndex inTabIndex )
{
	if( NVI_IsWindowVisible() == true )
	{
		//
		NVMDO_DoTabDeactivated(inTabIndex);
		/*#135 DoViewFocusActivated()��NVI_SelectTab()����NVI_SwitchFocusTo()�o�R�Ŏ��s����悤�ɕύX
		//
		AControlID	focus = mTabDataArray_LatentFocus.Get(inTabIndex);
		if( ExistView(focus) == true )
		{
			NVI_GetViewByControlID(focus).NVM_DoViewFocusDeactivated();
		}
		*/
	}
}

//�Ĕw�ʂɂ���^�u�̃C���f�b�N�X���擾
AIndex	AWindow::NVI_GetMostBackTabIndex() const
{
	return mTabZOrder.Get(mTabZOrder.GetItemCount()-1);
}

//
AIndex	AWindow::NVI_GetTabIndexByZOrderIndex( const AIndex inZOrderIndex ) const
{
	return mTabZOrder.Get(inZOrderIndex);
}

//#379
/**
�^�u�I���\���ǂ����i�\���L���j��ݒ肷��
*/
ABool	AWindow::NVI_GetTabSelectable( const AIndex inTabIndex ) const
{
	return mTabDataArray_TabSelectable.Get(inTabIndex);
}

//#559
/**
���̕\���^�u���擾
*/
ABool	AWindow::NVIDO_GetNextDisplayTab( AIndex& ioIndex, const ABool inLoop ) const
{
	//
	AIndex	tabIndex = ioIndex;
	tabIndex++;
	for( ; tabIndex != ioIndex; tabIndex++ )
	{
		if( tabIndex >= NVI_GetTabCount() )
		{
			if( inLoop == true )
			{
				tabIndex = 0;
			}
			else
			{
				return false;
			}
		}
		if( NVI_GetTabSelectable(tabIndex) == true )   break;
	}
	//
	ABool	changed = (tabIndex != ioIndex);
	ioIndex = tabIndex;
	return changed;
}

//#559
/**
�O�̕\���^�u���擾
*/
ABool	AWindow::NVIDO_GetPrevDisplayTab( AIndex& ioIndex, const ABool inLoop ) const
{
	//
	AIndex	tabIndex = ioIndex;
	tabIndex--;
	for( ; tabIndex != ioIndex; tabIndex-- )
	{
		if( tabIndex < 0 )
		{
			if( inLoop == true )
			{
				tabIndex = NVI_GetTabCount()-1;
			}
			else
			{
				return false;
			}
		}
		if( NVI_GetTabSelectable(tabIndex) == true )   break;
	}
	//
	ABool	changed = (tabIndex != ioIndex);
	ioIndex = tabIndex;
	return changed;
}

//#559
/**
�ŏ��̕\���^�u���擾
*/
AIndex	AWindow::NVI_GetFirstDisplayTab() const
{
	AIndex	tabIndex = 0;
	NVI_GetPrevDisplayTab(tabIndex,true);
	NVI_GetNextDisplayTab(tabIndex,true);
	return tabIndex;
}

//#559
/**
�Ō�̕\���^�u���擾
*/
AIndex	AWindow::NVI_GetLastDisplayTab() const
{
	AIndex	tabIndex = 0;
	NVI_GetPrevDisplayTab(tabIndex,true);
	return tabIndex;
}

#pragma mark ===========================

#pragma mark ---�E�C���h�E���ݒ�^�擾

//�E�C���h�E�\���������擾
ABool	AWindow::NVI_IsWindowVisible() const
{
	if( NVI_IsWindowCreated() == false )   return false;//#402
	return GetImpConst().IsVisible();
}

//�E�C���h�E�쐬�ς݂����擾
ABool	AWindow::NVI_IsWindowCreated() const
{
	return GetImpConst().IsWindowCreated();
}

//�E�C���h�E���A�N�e�B�u�����擾
ABool	AWindow::NVI_IsWindowActive() const
{
	if( NVI_IsWindowCreated() == false )   return false;//#402
	//#634 �I�[�o�[���C���[�h�̂Ƃ��́A�e�E�C���h�E��active��Ԃ�Ԃ�
	if( NVM_GetOverlayMode() == true )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(mOverlayParentWindowID).NVI_IsWindowActive();
	}
	//
	return GetImpConst().IsActive();
}

/**
�E�C���h�E��Focus�A�N�e�B�u(Key window)�����擾
*/
ABool	AWindow::NVI_IsWindowFocusActive() const
{
	if( NVI_IsWindowCreated() == false )   return false;//#402
	//
	return GetImpConst().IsFocusActive();
}

//WindowRef�擾
AWindowRef	AWindow::NVI_GetWindowRef() const
{
	return GetImpConst().GetWindowRef();
}

//���݂̃^�u�ɑΉ�����DocumentID���擾
ADocumentID	AWindow::NVI_GetCurrentDocumentID() const
{
	AIndex	currentTabIndex = NVI_GetCurrentTabIndex();
	if( currentTabIndex == kIndex_Invalid )   return kObjectID_Invalid;//�A�v���I�����̃^�C�~���O�ɂ���Ă�currentTabIndex��Invalid�̏ꍇ������
	return NVI_GetDocumentIDByTabIndex(currentTabIndex);
}

ABool	AWindow::NVI_HasCloseButton() const
{
	return GetImpConst().HasCloseButton();
}

ABool	AWindow::NVI_HasTitleBar() const
{
	return GetImpConst().HasTitleBar();
}

ABool	AWindow::NVI_IsFloating() const
{
	return GetImpConst().IsFloatingWindow();
}

ABool	AWindow::NVI_IsModal() const
{
	return GetImpConst().IsModalWindow();
}

ABool	AWindow::NVI_IsResizable() const
{
	return GetImpConst().IsResizable();
}

ABool	AWindow::NVI_IsZoomable() const
{
	return GetImpConst().IsZoomable();
}

ABool	AWindow::NVI_IsZoomed() const
{
	return GetImpConst().IsZoomed();
}

//#673
/**
�h�L�������g�E�C���h�E���ǂ������擾
�i���t���[�e�B���O�ł͂Ȃ��A���A�I�[�o�[���C�ł͂Ȃ��j
*/
ABool	AWindow::NVI_IsDocumentWindow() const
{
	//�h���N���X�Ńh�L�������g�E�C���h�E�ƒ�`����Ă���ꍇ�͂������D�� #688
	if( NVIDO_IsDocumentWindow() == true )
	{
		return true;
	}
	//�t���[�e�B���O�ł͂Ȃ��A���A�I�[�o�[���C�ł͂Ȃ��Ȃ�true��Ԃ�
	return ((NVI_IsFloating()==false)&&(NVM_GetOverlayMode()==false));
}

//win
/**
*/
void	AWindow::NVI_GetWindowBoundsInStandardState( ARect& outBounds ) const
{
	GetImpConst().GetWindowBoundsInStandardState(outBounds);
}

//B0395
ABool	AWindow::NVI_IsWindowCollapsed() const
{
	return GetImpConst().IsWindowCollapsed();
}

/*
#291 �q�E�C���h�E�̃Z�N�V�����Ɉړ�
//B0411
ABool	AWindow::NVI_IsChildWindowVisible() const
{
	if( GetImpConst().ChildWindowVisible() == true )   return true;
	if( NVI_IsPrintMode() == true )   return true;
	if( NVIDO_IsChildWindowVisible() == true )   return true;
	return false;
}
*/

//#182
/**
��������ǂ������擾
*/
ABool	AWindow::NVI_IsPrintMode() const
{
	return GetImpConst().IsPrintMode();
}

//#175
//�E�C���h�E������\�ɂ���
void	AWindow::NVI_AddToRotateArray()
{
	AApplication::GetApplication().NVI_AddToWindowRotateArray(GetObjectID());
}

/**
�E�C���h�E���^�C�}�[�A�N�V�����Ώۂɂ���
*/
void	AWindow::NVI_AddToTimerActionWindowArray()
{
	AApplication::GetApplication().NVI_AddToTimerActionWindowArray(GetObjectID());
}

//#199
/**
�E�C���h�E��AllClose�Ώۂɂ���
*/
void	AWindow::NVI_AddToAllCloseTargetWindowArray()
{
	AApplication::GetApplication().NVI_AddToAllCloseTargetWindowArray(GetObjectID());
}

//#182
/**
�E�C���h�E�����x�ݒ�
*/
void	AWindow::NVI_SetWindowTransparency( const AFloatNumber inTransparency )
{
	if( NVM_GetOverlayMode() == false )
	{
		GetImp().SetWindowTransparency(inTransparency);
	}
	else
	{
#if IMPLEMENTATION_FOR_WINDOWS
		//win Windows�̏ꍇ�AOverlay�E�C���h�E�̓E�C���h�E�S�́ibackground�E�C���h�E�j�ɓ����x��ݒ�ł���悤�ɂ���
		//Mac�̏ꍇ�́AView��Paint����Alpha�`�悷��
		if( mOverlayTransparent == true )//false�̏ꍇ�͏�ɔ񓧉�
		{
			GetImp().SetWindowTransparency(inTransparency);
		}
#else
		//#1255
		GetImp().SetWindowTransparency(inTransparency);
#endif
	}
}

//#182
/**
�E�C���h�EDirty�ݒ�
*/
void	AWindow::NVI_SetModified( const ABool inModified )
{
	GetImp().SetModified(inModified);
}

//#182
/**
�E�C���h�E���̑SControl�����L�[��backslash�ɕϊ�����ǂ�����ݒ�
*/
void	AWindow::NVI_SetInputBackslashByYenKeyMode( const ABool inMode )
{
	GetImp().SetInputBackslashByYenKeyMode(inMode);
	for( AIndex i = 0; i < mViewControlIDArray.GetItemCount(); i++ )
	{
		NVI_GetViewByControlID(mViewControlIDArray.Get(i)).NVI_SetInputBackslashByYenKeyMode(inMode);
	}
}

//#182
/**
TabControl�̒l��ݒ�
*/
void	AWindow::NVI_SelectTabControl( const AIndex inIndex )
{
	GetImp().SetTabControlValue(inIndex);
}

//win
/**
�E�C���h�E�̎w��̈��h��Ԃ�
*/
void	AWindow::NVI_PaintRect( const AWindowRect& inRect, const AColor& inColor )
{
	GetImp().PaintRect(inRect,inColor);
}

#pragma mark ===========================

#pragma mark ---Control����
//#137

/**
Control��Show/Hide
*/
void	AWindow::NVI_SetShowControl( const AControlID inControlID, const ABool inShow )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetShowControl(inControlID,inShow);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetShow(inShow);
	}
}

//#246
/**
Contol��Show/Hide�i�������AinTabIndex�����݂̃^�u�̏ꍇ�̂ݏ������s���j
*/
void	AWindow::NVI_SetShowControlIfCurrentTab( const AIndex inTabIndex, const AControlID inControlID, const ABool inShow )
{
	if( inTabIndex == NVI_GetCurrentTabIndex() )
	{
		NVI_SetShowControl(inControlID,inShow);
	}
}

//#246
/**
Control�̈ʒu�ݒ�
*/
void	AWindow::NVI_SetControlPosition( const AControlID inControlID, const AWindowPoint& inPoint )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetControlPosition(inControlID,inPoint);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetPosition(inPoint);
	}
}

//#246
/**
Control�̃T�C�Y�ݒ�
*/
void	AWindow::NVI_SetControlPositionIfCurrentTab( const AIndex inTabIndex, const AControlID inControlID, const AWindowPoint& inPoint )
{
	if( inTabIndex == NVI_GetCurrentTabIndex() )
	{
		NVI_SetControlPosition(inControlID,inPoint);
	}
}

//#246
/**
Control�̃T�C�Y�ݒ�
*/
void	AWindow::NVI_SetControlSize( const AControlID inControlID, const ANumber inWidth, const ANumber inHeight )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetControlSize(inControlID,inWidth,inHeight);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetSize(inWidth,inHeight);
	}
}

//#246
/**
Control�̃T�C�Y�ݒ�
*/
void	AWindow::NVI_SetControlSizeIfCurrentTab( const AIndex inTabIndex, const AControlID inControlID, const ANumber inWidth, const ANumber inHeight )
{
	if( inTabIndex == NVI_GetCurrentTabIndex() )
	{
		NVI_SetControlSize(inControlID,inWidth,inHeight);
	}
}

//#135
/**
Control�̒��g��S�I��
*/
void	AWindow::NVI_SetControlSelectAll( const AControlID inControlID )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetSelectionAll(inControlID);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetSelectAll();
	}
}

//#182
/**
�R���g���[��Enable/Disable
*/
void	AWindow::NVI_SetControlEnable( const AControlID inControlID, const ABool inEnable )
{
	//enable/disable�ɕύX��������΁A�������Ȃ�
	if( NVI_IsControlEnable(inControlID) == inEnable )
	{
		return;
	}
	//
	if( IsView(inControlID) == false )
	{
		GetImp().SetEnableControl(inControlID,inEnable);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetEnabled(inEnable);
	}
	//#349
	if( NVI_GetCurrentFocus() == inControlID && inEnable == false )
	{
		NVI_SwitchFocusToNext();
	}
}

//#182
/**
Control��Rerturn�L�[���������邩�ǂ�����ݒ�
*/
void	AWindow::NVI_SetCatchReturn( const AControlID inControlID, const ABool inCatch )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetCatchReturn(inControlID,inCatch);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetCatchReturn(inCatch);
	}
}

//#182
/**
Control��Tab�L�[���������邩�ǂ�����ݒ�
*/
void	AWindow::NVI_SetCatchTab( const AControlID inControlID, const ABool inCatch )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetCatchTab(inControlID,inCatch);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetCatchTab(inCatch);
	}
}

//#182
/**
Control�̈ʒu�擾
*/
void	AWindow::NVI_GetControlPosition( const AControlID inID, AWindowPoint& outPoint ) const
{
	GetImpConst().GetControlPosition(inID,outPoint);
}

//#182
/**
Control�̕��擾
*/
ANumber	AWindow::NVI_GetControlWidth( const AControlID inID ) const
{
	return GetImpConst().GetControlWidth(inID);
}

//#182
/**
Control�̍����擾
*/
ANumber	AWindow::NVI_GetControlHeight( const AControlID inID ) const
{
	return GetImpConst().GetControlHeight(inID);
}

//#182
/**
Control��Visible���ǂ������擾
*/
ABool	AWindow::NVI_IsControlVisible( const AControlID inControlID ) const
{
	return GetImpConst().IsControlVisible(inControlID);
}

//#182
/**
Control��Enable���ǂ������擾
*/
ABool	AWindow::NVI_IsControlEnable( const AControlID inControlID ) const
{
	return GetImpConst().IsControlEnabled(inControlID);
}

//#182
/**
Control���܂܂��TabControl��Index���擾
*/
AIndex	AWindow::NVI_GetControlEmbeddedTabControlIndex( const AControlID inID ) const
{
	return GetImpConst().GetControlEmbeddedTabIndex(inID);
}

//#182
/**
�t�H�[�J�X�ړ��őS�I�����邩�ǂ�����ݒ�
*/
void	AWindow::NVI_SetAutomaticSelectBySwitchFocus( const AControlID inControlID, const ABool inSelect )
{
	if( IsView(inControlID) == true )
	{
		NVI_GetViewByControlID(inControlID).NVI_SetAutomaticSelectBySwitchFocus(inSelect);
	}
	else
	{
		GetImp().SetAutoSelectAllBySwitchFocus(inControlID,inSelect);
	}
}

/**
�r���[�̓��e�����I�������s
*/
void	AWindow::NVI_AutomaticSelectBySwitchFocus( const AControlID inControlID )
{
	if( IsView(inControlID) == true )
	{
		NVI_GetViewByControlID(inControlID).NVI_AutomaticSelectBySwitchFocus();
	}
}

//#182
/**
Control�̃A�C�R����ݒ肷��
*/
void	AWindow::NVI_SetControlIcon( const AControlID inControlID, const AIconID inIconID,
		const ABool inDefaultSize, const ANumber inLeftOffset, const ANumber inTopOffset )
{
	GetImp().SetIcon(inControlID,inIconID,inDefaultSize,inLeftOffset,inTopOffset);
}

//#182
/**
Control�̃��j���[��ݒ肷��
*/
void	AWindow::NVI_SetMenuItemsFromTextArray( const AControlID inID, const ATextArray& inTextArray )
{
	//���ݑI�𒆂̍��ڂ̃e�L�X�g���L�� #1316
	AText	text;
	NVI_GetControlText(inID,text);
	//
	GetImp().SetMenuItemsFromTextArray(inID,inTextArray);
	//�L�����Ă����e�L�X�g��ݒ� #1316
	NVI_SetControlText(inID,text);
}

//win
/**
Control�̃��j���[�ɍ��ڂ�ǉ�����
*/
void	AWindow::NVI_InsertMenuItem( const AControlID inID, const AIndex inMenuItemIndex, const AText& inText )
{
	GetImp().InsertMenuItem(inID,inMenuItemIndex,inText);
}

//win
/**
Control�̃��j���[�̍��ڂ��폜����
*/
void	AWindow::NVI_DeleteMenuItem( const AControlID inID, const AIndex inMenuItemIndex )
{
	GetImp().DeleteMenuItem(inID,inMenuItemIndex);
}

//#182
/**
Control�\���X�V
*/
void	AWindow::NVI_DrawControl( const AControlID inID )
{
	GetImp().DrawControl(inID);
}

//#182
/**
Control�e�q�ݒ�
*/
void	AWindow::NVI_EmbedControl( const AControlID inParent, const AControlID inChild )
{
	//View��FrameView�����݂���ꍇ��FrameView�̂ق���inParent��embed���� #135
	AControlID	frameControlID = kControlID_Invalid;
	if( IsView(inChild) == true )
	{
		frameControlID = NVI_GetViewByControlID(inChild).NVI_GetFrameViewControlID();
	}
	if( frameControlID != kControlID_Invalid )
	{
		if( frameControlID != inParent )
		{
			GetImp().EmbedControl(inParent,frameControlID);
		}
		else
		{
			GetImp().EmbedControl(inParent,inChild);
		}
	}
	else
	{
		GetImp().EmbedControl(inParent,inChild);
	}
}

//#182
/**
Control�̍ŏ��ő�l�̐ݒ�
*/
void	AWindow::NVI_SetMinMax( const AControlID inControlID, const ANumber inMin, const ANumber inMax )
{
	GetImp().SetMinMax(inControlID,inMin,inMax);
}

//#182
/**
FontMenu�̓o�^
*/
void	AWindow::NVI_RegisterFontMenu( const AControlID inID, const ABool inEnableDefaultFontItem )//#375
{
	GetImp().RegisterFontMenu(inID,inEnableDefaultFontItem);//#375
}

//#182
/**
FontMenu�̓o�^
*/
void	AWindow::NVI_RegisterTextArrayMenu( const AControlID inControlID, const ATextArrayMenuID inTextArrayMenuID )
{
	GetImp().RegisterTextArrayMenu(inControlID,inTextArrayMenuID);
}

//#182
/**
FontMenu�̓o�^
*/
void	AWindow::NVI_UnregisterTextArrayMenu( const AControlID inControlID )
{
	GetImp().UnregisterTextArrayMenu(inControlID);
}

//#182
/**
Control��Help�^�O/Tooltip�ݒ�
*/
void	AWindow::NVI_SetHelpTag( const AControlID inID, const AText& inText, const AHelpTagSide inTagSide )//#634
{
	//#688 GetImp().SetHelpTag(inID,inText,inTagSide);//#634
	AIndex	index = mToolTipTextArray_ControlID.Find(inID);
	if( index == kIndex_Invalid )
	{
		mToolTipTextArray_ControlID.Add(inID);
		mToolTipTextArray_ToolTipText.Add(inText);
		mToolTipTextArray_HelpTagSide.Add(inTagSide);
	}
	else
	{
		mToolTipTextArray_ControlID.Set(index,inID);
		mToolTipTextArray_ToolTipText.Set(index,inText);
		mToolTipTextArray_HelpTagSide.Set(index,inTagSide);
	}
}

//#182
/**
Control��Help�^�O�}�E�X�I�[�o�[����Callback�֐����ĂԂ悤�ɂ���i�G���A���ɕ\�����e��ς���ꍇ�p�j
*/
void	AWindow::NVI_EnableHelpTagCallback( const AControlID inID, const ABool inShowOnlyWhenNarrow )//#507
{
	//#688 GetImp().EnableHelpTagCallback(inID,inShowOnlyWhenNarrow);//#507
}

//#182
/**
Help�^�O��Hide
*/
void	AWindow::NVI_HideHelpTag() const
{
	//#688 GetImpConst().HideHelpTag();
	//�c�[���`�b�v�E�C���h�E��\��
	AApplication::GetApplication().NVI_GetToolTipWindow().NVI_Hide();
}

//#182
/**
Undo�����N���A
*/
void	AWindow::NVI_ClearControlUndoInfo( const AControlID inControlID )
{
	if( IsView(inControlID) == true )
	{
		NVI_GetViewByControlID(inControlID).NVI_ClearUndoInfo();
	}
	else
	{
		GetImp().ResetUndoStack(inControlID);
	}
}

//#291
/**
�R���g���[����Z�ʒu��Top�ɐݒ肷��
*/
void	AWindow::NVI_SetControlZOrderToTop( const AControlID inID )
{
	GetImp().SetControlZOrderToTop(inID);
}

//#291
/**
Bindings�ݒ�
*/
void	AWindow::NVI_SetControlBindings( const AControlID inControlID,
		const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
		const ABool inWidthFixed, const ABool inHeightFixed )//#688
{
	if( IsView(inControlID) == true )//#455
	{
		NVI_GetViewByControlID(inControlID).NVI_SetControlBindings(
					inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
	}
	else
	{
		GetImp().SetControlBindings(inControlID,
					inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
	}
}

//win
/**
*/
void	AWindow::NVI_DisableEventCatch( const AControlID inControlID )
{
	GetImp().DisableEventCatch(inControlID);
}

//�R���g���[���̍폜
void	AWindow::NVI_DeleteControl( const AControlID inControlID )
{
	//#243 �^�u�ւ̓o�^�imTabDataArray_ControlIDs�j���폜�Ȃ�A�o�^�폜����
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray_ControlIDs.GetItemCount(); tabIndex++ )
	{
		if( mTabDataArray_ControlIDs.GetObject(tabIndex).Find(inControlID) != kIndex_Invalid )
		{
			NVM_UnregisterViewInTab(tabIndex,inControlID);
			break;
		}
	}
	//#135 �t�H�[�J�X�O���[�v����폜     win ����IsView()���������ړ�
	NVI_UnregisterFromFocusGroup(inControlID);
	//
	if( IsView(inControlID) == false )
	{
		//AView�͑��݂��Ȃ��ꍇ
		GetImp().DeleteControl(inControlID);
	}
	else
	{
		//AView���݂���ꍇ
		DeleteView(inControlID);
	}
	//LatentFocus�N���A
	if( mLatentFocus == inControlID )
	{
		mLatentFocus = kControlID_Invalid;
	}
}

//#565
/**
�R���g���[����FileDrop�\�ɐݒ�
*/
void	AWindow::NVI_EnableFileDrop( const AControlID inControlID )
{
	GetImp().EnableFileDrop(inControlID);
}

//#182
/**
ViewImp�擾
*/
CPaneForAView&	AWindow::NVI_GetViewImp( const AControlID inControlID )
{
	return dynamic_cast<CPaneForAView&>(GetImp().GetUserPane(inControlID));
}

#pragma mark ===========================

#pragma mark ---���W�ϊ�
//#182

/**
LocalPoint��WindowPoint
*/
void	AWindow::NVI_GetWindowPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AWindowPoint& outWindowPoint ) const
{
	GetImpConst().GetWindowPointFromControlLocalPoint(inID,inLocalPoint,outWindowPoint);
}

/**
LocalRect��WindowRect
*/
void	AWindow::NVI_GetWindowRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AWindowRect& outWindowRect ) const
{
	GetImpConst().GetWindowRectFromControlLocalRect(inID,inLocalRect,outWindowRect);
}

/**
WindowPoint��LocalPoint
*/
void	AWindow::NVI_GetControlLocalPointFromWindowPoint( const AControlID inID, const AWindowPoint& inWindowPoint, ALocalPoint& outLocalPoint ) const
{
	GetImpConst().GetControlLocalPointFromWindowPoint(inID,inWindowPoint,outLocalPoint);
}

/**
WindowRect��LocalRect
*/
void	AWindow::NVI_GetControlLocalRectFromWindowRect( const AControlID inID, const AWindowRect& inWindowRect, ALocalRect& outLocalRect ) const
{
	GetImpConst().GetControlLocalRectFromWindowRect(inID,inWindowRect,outLocalRect);
}

/**
LocalPoint��GlobalPoint
*/
void	AWindow::NVI_GetGlobalPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AGlobalPoint& outGlobalPoint ) const
{
	GetImpConst().GetGlobalPointFromControlLocalPoint(inID,inLocalPoint,outGlobalPoint);
}

/**
LocalRect��GlobalRect
*/
void	AWindow::NVI_GetGlobalRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AGlobalRect& outGlobalRect ) const
{
	GetImpConst().GetGlobalRectFromControlLocalRect(inID,inLocalRect,outGlobalRect);
}

//#182
/**
*/
void	AWindow::NVI_DragWindow( const AControlID inControlID, const ALocalPoint& inMouseLocalPoint )
{
	GetImp().DragWindow(inControlID,inMouseLocalPoint);
}

#pragma mark ===========================

#pragma mark ---StickyWindow����
//#138

//#138
/**
StickyWindow�ɂ���i�q������R�[���j
*/
void	AWindow::NVI_SetAsStickyWindow( const AWindowID inParent )
{
	mParentWindowIDForStickyWindow = inParent;
	//�e�E�C���h�E�ɓo�^
	AApplication::GetApplication().NVI_GetWindowByID(mParentWindowIDForStickyWindow).NVI_RegisterChildStickyWindow(GetObjectID());
}

//#138
/**
�qStickyWindow��o�^����
*/
void	AWindow::NVI_RegisterChildStickyWindow( const AWindowID inChild )
{
	//�q�E�C���h�E��WindowID, Offset��o�^
	mChildStickyWindowArray_WindowID.Add(inChild);
	AWindowPoint	pt = {0,0};
	mChildStickyWindowArray_Offset.Add(pt);
}

//#138
/**
�qStickyWindow�̓o�^����������
*/
void	AWindow::NVI_UnregisterChildStickyWindow( const AWindowID inChild )
{
	AIndex	index = mChildStickyWindowArray_WindowID.Find(inChild);
	if( index == kIndex_Invalid )   return;
	
	//�q�E�C���h�E�̓o�^���폜
	mChildStickyWindowArray_WindowID.Delete1(index);
	mChildStickyWindowArray_Offset.Delete1(index);
}

//#138
/**
�qSticky�E�C���h�E�̈ʒu�ݒ�

@param inChild �q�E�C���h�E��WindowID
@param inOffset �q�E�C���h�E�̃I�t�Z�b�g�ʒu�i�e�E�C���h�E�̍��ォ��̑��Έʒu�j
*/
void	AWindow::NVI_SetChildStickyWindowOffset( const AWindowID inChild, const AWindowPoint& inOffset )
{
	AIndex	index = mChildStickyWindowArray_WindowID.Find(inChild);
	if( index == kIndex_Invalid )   { _ACError("child sticky window not found",this); return; }
	
	//�q�E�C���h�E�̈ʒu�����X�V
	mChildStickyWindowArray_Offset.Set(index,inOffset);
	
	//�q�E�C���h�E�̈ʒu�ݒ�
	APoint	gpt;
	NVI_GetWindowPosition(gpt);
	gpt.x += inOffset.x;
	gpt.y += inOffset.y;
	AApplication::GetApplication().NVI_GetWindowByID(inChild).NVI_SetWindowPosition(gpt);
}

//#138
/**
�S�Ă̎qSticky�E�C���h�E�̈ʒu���X�V
*/
void	AWindow::UpdateStickyWindowOffset()
{
	//�e�E�C���h�E�̈ʒu�ɒǐ����āA�S�Ă̎q�E�C���h�E���ړ�
	APoint	parentgpt;
	NVI_GetWindowPosition(parentgpt);
	for( AIndex i = 0; i < mChildStickyWindowArray_WindowID.GetItemCount(); i++ )
	{
		//�I�t�Z�b�g�ʒu�擾
		AWindowPoint	offset = mChildStickyWindowArray_Offset.Get(i);
		//�e�E�C���h�E�̍��ォ��̑��Έʒu���擾
		APoint	childgpt = parentgpt;
		childgpt.x += offset.x;
		childgpt.y += offset.y;
		//�q�E�C���h�E�̈ʒu�ݒ�
		AApplication::GetApplication().NVI_GetWindowByID(mChildStickyWindowArray_WindowID.Get(i)).NVI_SetWindowPosition(childgpt);
	}
}

#pragma mark ===========================

#pragma mark ---�I�[�o�[���C�E�C���h�E����
//#291

/**
�I�[�o�[���C���[�h�ɐݒ肷��
*/
void	AWindow::NVI_ChangeToOverlay( const AWindowID inParentWindowID, const ABool inTransparent )//win
{
	//
	mOverlayParentWindowID = inParentWindowID;
	mOverlayTransparent = inTransparent;//win
	//
	mOverlayMode = true;
	//#379
	//�e�E�C���h�E���ʒu�f�[�^������
	mOverlayCurrentOffset.x = 0;
	mOverlayCurrentOffset.y = 0;
	mOverlayCurrentWidth = 0;
	mOverlayCurrentHeight = 0;
	//�e�E�C���h�E�擾
	AWindowRef	parentWindowRef = NULL;
	if( mOverlayParentWindowID != kObjectID_Invalid )
	{
		parentWindowRef = AApplication::GetApplication().NVI_GetWindowByID(mOverlayParentWindowID).NVI_GetWindowRef();
	}
	//�I�[�o�[���C���[�h�ɐݒ�
	if( NVI_IsWindowCreated() == true )
	{
		GetImp().ChangeToOverlayWindow(parentWindowRef);
	}
	//�E�C���h�E�^�C�v�ɂ���ăA���t�@�l��������肷��̂ŁA����𔽉f������
	NVI_UpdateProperty();
	/*
	//
	if( NVM_GetOverlayMode() == false )
	{
		//
		SetOverlayMode(true);
		//�I�[�o�[���C�E�C���h�E�o�^��NVI_Create�Ŏ��s�����
	}
	*/
}

/**
�I�[�o�[���C���[�h����������
*/
void	AWindow::NVI_ChangeToFloating()
{
	//
	mOverlayParentWindowID = kObjectID_Invalid;
	mOverlayTransparent = false;//win
	//
	mOverlayMode = false;
	//#379
	//�e�E�C���h�E���ʒu�f�[�^������
	mOverlayCurrentOffset.x = 0;
	mOverlayCurrentOffset.y = 0;
	mOverlayCurrentWidth = 0;
	mOverlayCurrentHeight = 0;
	//�t���[�e�B���O�E�C���h�E���[�h�ɐݒ�
	if( NVI_IsWindowCreated() == true )
	{
		GetImp().ChangeToFloatingWindow();
	}
	//�E�C���h�E�^�C�v�ɂ���ăA���t�@�l��������肷��̂ŁA����𔽉f������
	NVI_UpdateProperty();
	/*
	if( NVM_GetOverlayMode() == true )
	{
		SetOverlayMode(false);
		//�I�[�o�[���C�E�C���h�E�o�^��SetOverlayMode()���ł̃E�C���h�E�폜�ɂ�莩���I�ɉ��������
	}
	*/
}

/**
�I�[�o�[���C���[�h��ݒ�
*/
void	AWindow::SetOverlayMode( const ABool inOverlayMode )
{
	if( mOverlayMode != inOverlayMode )
	{
		//#379
		mOverlayCurrentOffset.x = 0;
		mOverlayCurrentOffset.y = 0;
		mOverlayCurrentWidth = 0;
		mOverlayCurrentHeight = 0;
		//
		if( NVI_IsWindowCreated() == true )
		{
			//Tab�f�[�^���L��
			AArray<ADocumentID>	svTabDataArray_DocumentID;
			for( AIndex i = 0; i < mTabDataArray_DocumentID.GetItemCount(); i++ )
			{
				svTabDataArray_DocumentID.Add(mTabDataArray_DocumentID.Get(i));
			}
			//�\����Ԃ��L��
			//ABool	visible = NVI_IsWindowVisible();
			//�E�C���h�E�����
			NVI_Close();
			//�I�[�o�[���C���[�h�ύX
			mOverlayMode = inOverlayMode;
			//�eTab�𐶐�
			//NVI_Create()�͍ŏ���NVI_CreateTab()���ŃR�[�������
			for( AIndex i = 0; i < svTabDataArray_DocumentID.GetItemCount(); i++ )
			{
				NVI_CreateTab(svTabDataArray_DocumentID.Get(i));
			}
			//�\����Ԃ�߂�
			/*if( visible == true )
			{
				NVI_Show();
			}*/
		}
		else
		{
			mOverlayMode = inOverlayMode;
		}
	}
}

/**
�I�[�o�[���C�E�C���h�E�̐e�E�C���h�E���ł̃I�t�Z�b�g�ʒu��ݒ�
*/
void	AWindow::NVI_SetOffsetOfOverlayWindow( const AWindowID inParentWindowID, const AWindowPoint& inOffset )
{
	if( NVM_GetOverlayMode() == false )   { _ACError("not in OverlayMode",this); return; }
	//#379 �����������B�O�Ɠ����ʒu�Ȃ牽�����Ȃ�
	if( inOffset.x == mOverlayCurrentOffset.x && inOffset.y == mOverlayCurrentOffset.y )   return;
	//
	GetImp().SetOverlayWindowMoveTogether(false);
	//
	APoint	gpt;
	AApplication::GetApplication().NVI_GetWindowByID(inParentWindowID).NVI_GetWindowPosition(gpt);
	gpt.x += inOffset.x;
	gpt.y += inOffset.y;
	NVI_SetWindowPosition(gpt);
	//
	GetImp().SetOverlayWindowMoveTogether(true);
	//#379
	mOverlayCurrentOffset = inOffset;
}

/**
�I�[�o�[���C�E�C���h�E�̐e�E�C���h�E���ł̃I�t�Z�b�g�ʒu��ݒ�
*/
void	AWindow::NVI_SetOffsetOfOverlayWindowAndSize( const AWindowID inParentWindowID, 
		const AWindowPoint& inOffset, const ANumber inWidth, const ANumber inHeight, const ABool inAnimate )
{
	if( NVM_GetOverlayMode() == false )   { _ACError("not in OverlayMode",this); return; }
	//
	//#379 �����������B�O�Ɠ����ʒu�E�T�C�Y�Ȃ牽�����Ȃ�
	//#688 if( inOffset.x == mOverlayCurrentOffset.x && inOffset.y == mOverlayCurrentOffset.y &&
	//#688inWidth == mOverlayCurrentWidth && inHeight == mOverlayCurrentHeight )   return;
	GetImp().SetOverlayWindowMoveTogether(false);
	//
	APoint	gpt;
	AApplication::GetApplication().NVI_GetWindowByID(inParentWindowID).NVI_GetWindowPosition(gpt);
	gpt.x += inOffset.x;
	gpt.y += inOffset.y;
	ARect	grect;
	grect.left		= gpt.x;
	grect.top		= gpt.y;
	grect.right		= gpt.x + inWidth;
	grect.bottom	= gpt.y + inHeight;
	GetImp().SetWindowBounds(grect,inAnimate);
	//
	GetImp().SetOverlayWindowMoveTogether(true);
	//#379
	mOverlayCurrentOffset = inOffset;
	mOverlayCurrentWidth = inWidth;
	mOverlayCurrentHeight = inHeight;
}

//#320
/**
�I�[�o�[���C�E�C���h�E���w��E�C���h�E�̔w��Ɉړ�
*/
void	AWindow::NVI_SendOverlayWindowBehind( const AWindowID inWindowID )
{
	if( NVM_GetOverlayMode() == false )   { _ACError("not in OverlayMode",this); return; }
	//Layer�Œ����
	GetImp().SetOverlayLayerTogether(false);
	//�w��Ɉړ�
	NVI_SendBehind(inWindowID);
	//Layer�Œ�
	GetImp().SetOverlayLayerTogether(true);
}

//#688
/**
�I�[�o�[���C�E�C���h�E���w��E�C���h�E�̑O�ʂɈړ�
*/
void	AWindow::NVI_SendOverlayWindowAbove( const AWindowID inWindowID )
{
	if( NVM_GetOverlayMode() == false )   { _ACError("not in OverlayMode",this); return; }
	//Layer�Œ����
	GetImp().SetOverlayLayerTogether(false);
	//�w��Ɉړ�
	NVI_SendAbove(inWindowID);
	//Layer�Œ�
	GetImp().SetOverlayLayerTogether(true);
}

//#422
/**
Spaces���΍��p
*/
void	AWindow::NVI_SelectWindowForSpacesProblemCountermeasure()
{
	//Group����
	GetImp().SetOverlayWindowMoveTogether(false);
	//�E�C���h�E�I��
	NVI_SelectWindow();
	//Group�ݒ�
	GetImp().SetOverlayWindowMoveTogether(true);
}

#pragma mark ===========================

#pragma mark ---�q�I�[�o�[���C�E�C���h�E�Ǘ�
//#725

/**
�q�I�[�o�[���C�E�C���h�E�o�^
*/
void	AWindow::NVM_RegisterOverlayWindow( const AWindowID inWindowID, const AIndex inTabIndex, const ABool inLatentVisible )
{
	//�f�[�^�ǉ�
	mChildOverlayWindowArray_WindowID.Add(inWindowID);
	mChildOverlayWindowArray_TabIndex.Add(inTabIndex);
	mChildOverlayWindowArray_LatentVisible.Add(inLatentVisible);
	//show/hide����
	NVI_SetOverlayWindowVisibility(inWindowID,inLatentVisible);
}

//#725
/**
�q�I�[�o�[���C�E�C���h�E�o�^����
*/
void	AWindow::NVM_UnregisterOverlayWindow( const AWindowID inWindowID )
{
	//����
	AIndex	index = mChildOverlayWindowArray_WindowID.Find(inWindowID);
	if( index == kIndex_Invalid )   _ACThrow("",this);
	
	//�f�[�^�폜
	mChildOverlayWindowArray_WindowID.Delete1(index);
	mChildOverlayWindowArray_TabIndex.Delete1(index);
	mChildOverlayWindowArray_LatentVisible.Delete1(index);
}

//#725
/**
�q�I�[�o�[���C�E�C���h�Eshow/hide
*/
void	AWindow::NVI_SetOverlayWindowVisibility( const AWindowID inWindowID, const ABool inShow )
{
	//����
	AIndex	index = mChildOverlayWindowArray_WindowID.Find(inWindowID);
	if( index == kIndex_Invalid )   _ACThrow("",this);
	
	//���݂̕\���^�u�ƈ�v���Ă��āAinShow��true�Ȃ�Ashow����B����ȊO��hide�B
	AWindowID	winID = mChildOverlayWindowArray_WindowID.Get(index);
	AIndex	tabIndex = mChildOverlayWindowArray_TabIndex.Get(index);
	if( inShow == true && (tabIndex == kIndex_Invalid || tabIndex == NVI_GetCurrentTabIndex()) )
	{
		AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_Show(false);
	}
	else
	{
		AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_Hide();
	}
	//latent visible��Ԃ�ݒ�
	mChildOverlayWindowArray_LatentVisible.Set(index,inShow);
}

#pragma mark ===========================

#pragma mark ---�E�C���h�E�^�C�g���ݒ�^�擾

//�E�C���h�E�^�C�g���ݒ�i�Ή�����h�L�������g�����݂��Ȃ��ꍇ�p�j
void	AWindow::NVI_SetDefaultTitle( const AText& inText )
{
	mWindowDefaultTitle.SetText(inText);
	GetImp().SetTitle(inText);
}

//�E�C���h�E�^�C�g���擾
void	AWindow::NVI_GetTitle( const AIndex inTabIndex, AText& outTitle ) const
{
	ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	if( docID == kObjectID_Invalid )
	{
		outTitle.SetText(mWindowDefaultTitle);
	}
	else
	{
		AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_GetTitle(outTitle);
	}
}

//���݂̃E�C���h�E�^�C�g���擾
void	AWindow::NVI_GetCurrentTitle( AText& outTitle ) const
{
	NVI_GetTitle(NVI_GetCurrentTabIndex(),outTitle);
}

//�^�C�g���o�[�\���̍X�V
void	AWindow::NVIDO_UpdateTitleBar()//#699
{
	//win 080726
	//mFixTitle��true�̂Ƃ��͍ŏ��Ɍ��߂��^�C�g���̂܂܂ɂ���i�h�L�������g�^�C�g���ɍ��킹�Ȃ��j
	if( mFixTitle == true )   return;
	if( NVI_GetCurrentTabIndex() == kIndex_Invalid )   return;//#379
	//
	AText	text;
	NVI_GetTitle(NVI_GetCurrentTabIndex(),text);
	GetImp().SetTitle(text);
	
	ADocumentID	docID = NVI_GetCurrentDocumentID();
	if( docID == kObjectID_Invalid )   return;
	
	//ReadOnly
	if( AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_IsReadOnly() == true )
	{
		AText	text;
		NVI_GetTitle(NVI_GetCurrentTabIndex(),text);
		AText	ro;
		ro.SetLocalizedText("ReadOnly");
		text.AddText(ro);
		GetImp().SetTitle(text);
	}
	
	//�t�@�C���ݒ�ς݁A���A�X�N���[�j���O�����ς݂Ȃ�E�C���h�E�t�@�C���ݒ�
	if( AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_IsFileSpecifiedAndScreened() == true )
	{
		AFileAcc	file;
		AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_GetFile(file);
		GetImp().SetWindowFile(file);
	}
	else
	{
		GetImp().ClearWindowFile();
	}
	NVI_SetModified(AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_IsDirty());
	//#699 NVIDO_UpdateTitleBar();
}

#pragma mark ===========================

#pragma mark ���ʃV���[�g�J�b�g�L�[�p�ݒ�

#pragma mark ===========================

//#135
/**
�f�t�H���g�L�����Z���{�^���ݒ�iESC�L�[��������Click�Ƃ݂Ȃ����Control�j
*/
void	AWindow::NVI_SetDefaultCancelButton( const AControlID inControlID )
{
	mDefaultCancelButton = inControlID;
	GetImp().SetDefaultCancelButton(inControlID);
}

//#135
/**
�f�t�H���gOK�{�^���ݒ�iretrun�L�[��������Click�Ƃ݂Ȃ����Control�j
*/
void	AWindow::NVI_SetDefaultOKButton( const AControlID inControlID )
{
	mDefaultOKButton = inControlID;
	GetImp().SetDefaultOKButton(inControlID);
}

#pragma mark ===========================

#pragma mark <���L�N���X(AView)�����^�폜�^�擾>

#pragma mark ===========================

/*#271 View��������ControlID���m�肵�Ă��Ȃ���mViewImp��ݒ�ł��Ȃ��̂ł��̃��\�b�h�ɂ��View�����͂��Ȃ����Ƃɂ���
//#92
**
View�����i�܂����\�[�X���View�����݂��Ă��Ȃ��ꍇ�p�BControlID�̓��W���[�����Ő�������j

*
AViewID	AWindow::NVI_CreateView( AViewFactory& inFactory, 
		const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight, const ABool inSupportFocus, 
		const AIndex inTabControlIndex *�y���Ӂz"TabControl"��Index*, const ABool inFront )
{
	//ControlID����
	AControlID	controlID = NVM_AssignDynamicControlID();
	//View�ɑΉ�������̂𐶐�
	GetImp().CreatePaneForAView(controlID,inPosition,inWidth,inHeight,inSupportFocus,inTabControlIndex,inFront);
	//#135
	if( inSupportFocus == true )
	{
		NVI_RegisterToFocusGroup(controlID);
	}
	//View����
	AViewID	viewID = AApplication::GetApplication().NVI_CreateView(inFactory);
	mViewIDArray.Add(viewID);
	mViewControlIDArray.Add(controlID);
	//View�ւ�ControlID�ݒ�ƁA������
	AApplication::GetApplication().NVI_GetViewByID(viewID).NVI_SetControlID(controlID);
	AApplication::GetApplication().NVI_GetViewByID(viewID).NVI_Init();
	return viewID;
}
*/

/*win
void	AWindow::NVI_DeleteViewByViewID( const AViewID inViewID )
{
	//ViewID���猟��
	AIndex	viewIndex = mViewIDArray.Find(inViewID);
	if( viewIndex == kIndex_Invalid )   _ACThrow("no view found",this);
	AControlID	controlID = mViewControlIDArray.Get(viewIndex);
	NVM_DeleteView(controlID);//#135
}
*/

//View����
//�܂����\�[�X���View�����݂��Ă��Ȃ��ꍇ�p
//inTabControlIndex: OS API�Ő�������^�u�R���g���[���̂ǂ̃^�u�ɓ���邩�iAWindow�̃^�u�ł͂Ȃ��̂Œ��Ӂj
//inFront: ��������View���őO�ʂɂ��邩�ǂ���
AViewID	AWindow::NVM_CreateView( const AControlID inControlID, 
		const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight, 
		const AControlID inParentControlID, const AControlID inSiblingControlID,//#688
		const ABool /*#688inSupportFocus*/inSupportInputMethod, AViewFactory& inFactory,
		const AIndex inTabControlIndex /*�y���Ӂz"TabControl"��Index*/, const ABool inFront )//win 080618
{
	GetImp().CreatePaneForAView(inControlID,inPosition,inWidth,inHeight,inParentControlID,inSiblingControlID,
				inTabControlIndex,inFront,inSupportInputMethod);//#688
	//#135
	if( /*#688 inSupportFocus*/inSupportInputMethod == true )
	{
		NVI_RegisterToFocusGroup(inControlID);
	}
	//win 080707 ViewControlIDArray.Add(inControlID);
	//#199 mViewArray.AddNewObject(inFactory);
	AViewID	viewID = AApplication::GetApplication().NVI_CreateView(inFactory);//#199
	mViewIDArray.Add(viewID);//#199
	//win 080618 mViewControlIDArray.Add(inControlID);��ֈړ�
	//�iAView_xxx�̃R���X�g���N�^����DoDraw���Ă΂��
	//��mViewControlIDArray�ɓo�^���Ȃ��ƃr���[��DoDraw()���s����Ȃ�
	//��Offscreen�ɕ`�悳��Ȃ�
	//��Windows�łł�Offscreen�̕`��`�����X�͍ŏ���WM_PAINT���A�����IRefresh�ȊO�Ȃ�
	//win 080707
	//�X�ɕύX
	//mViewArray�ɓo�^����Ȃ���Windows����̃R�[���o�b�N��������view�Q�Ƃ������Ȃ��B
	//AView_xxx��init�������킯�āAmViewArray, mViewControlIDArray�̗����֓o�^���init���s����
	mViewControlIDArray.Add(inControlID);
	NVI_GetViewByControlID(inControlID).NVI_Init();
	return viewID;//#199
}

//View����
//���Ƀ��\�[�X���View�����݂��Ă���ꍇ�p
AViewID	AWindow::NVM_CreateView( const AControlID inControlID, const ABool /*#688inSupportFocus*/inSupportInputMethod, AViewFactory& inFactory )
{
	GetImp().RegisterPaneForAView(inControlID,inSupportInputMethod/*#135,inSupportFocus*/);//#688
	//#135
	if( /*#688 inSupportFocus*/inSupportInputMethod == true )
	{
		NVI_RegisterToFocusGroup(inControlID);
	}
	//#199 mViewArray.AddNewObject(inFactory);
	AViewID	viewID = AApplication::GetApplication().NVI_CreateView(inFactory);//#199
	mViewIDArray.Add(viewID);//#199
	mViewControlIDArray.Add(inControlID);
	NVI_GetViewByControlID(inControlID).NVI_Init();
	return viewID;//#199
}

//View�폜
void	AWindow::DeleteView( const AControlID inControlID )
{
	AIndex	viewIndex = mViewControlIDArray.Find(inControlID);
	if( viewIndex == kIndex_Invalid )   _ACThrow("no view found",this);
	//
	//#199 mViewArray.Delete1Object(viewIndex);
	AApplication::GetApplication().NVI_DeleteView(mViewIDArray.Get(viewIndex));//#199
	mViewIDArray.Delete1(viewIndex);//#199
	mViewControlIDArray.Delete1(viewIndex);
	//#135 �t�H�[�J�X�O���[�v����폜
	NVI_UnregisterFromFocusGroup(inControlID);
	//View�ɑΉ�������̂��폜
	GetImp().DeleteControl(inControlID);
	/*#135
	//win 080618
	//�t�H�[�J�X�̂���r���[���폜�����ꍇ�́A�t�H�[�J�X���ړ�
	if( mCurrentFocus == inControlID )
	{
		mCurrentFocus = kControlID_Invalid;
		NVI_SwitchFocusToFirst();
	}
	*/
	//LatentFocus�N���A
	if( mLatentFocus == inControlID )
	{
		mLatentFocus = kControlID_Invalid;
	}
}

//View�擾
AView&	AWindow::NVI_GetViewByControlID( const AControlID inControlID )
{
	AIndex	viewIndex = mViewControlIDArray.Find(inControlID);
	if( viewIndex == kIndex_Invalid )   _ACThrow("no view found",this);
	//#199 return mViewArray.GetObject(viewIndex);
	return AApplication::GetApplication().NVI_GetViewByID(mViewIDArray.Get(viewIndex));//#199
}
const AView&	AWindow::NVI_GetViewConstByControlID( const AControlID inControlID ) const
{
	AIndex	viewIndex = mViewControlIDArray.Find(inControlID);
	if( viewIndex == kIndex_Invalid )   _ACThrow("no view found",this);
	//#199 return mViewArray.GetObjectConst(viewIndex);
	return AApplication::GetApplication().NVI_GetViewConstByID(mViewIDArray.Get(viewIndex));//#199
}

/*#243 ���IControlID��10000�`�Œ�ɂ���
//���I����ControlID�͈̔͂�ݒ�
void	AWindow::NVM_SetDynamicControlIDRange( const AControlID inStart, const AControlID inEnd )
{
	mDynamicControlIDRangeStart = inStart;
	mDynamicControlIDRangeEnd = inEnd;
	mNextDynamicControlID = inStart;
}
*/

//���I����ControlID�����蓖�Ă�
AControlID	AWindow::NVM_AssignDynamicControlID()
{
	if( mDynamicControlIDRangeStart == kControlID_Invalid || mDynamicControlIDRangeEnd == kControlID_Invalid )
	{
		_ACThrow("dynamic control ID range is not set",this);
	}
	while( mViewControlIDArray.Find(mNextDynamicControlID) != kIndex_Invalid )
	{
		mNextDynamicControlID++;
		if( mNextDynamicControlID >= mDynamicControlIDRangeEnd )
		{
			mNextDynamicControlID = mDynamicControlIDRangeStart;
		}
	}
	AControlID	id = mNextDynamicControlID;
	mNextDynamicControlID++;
	return id;
}

/*#182
ABool	AWindow::ExistView( const AControlID inControlID )
{
	if( inControlID == kControlID_Invalid )   return false;
	return ( mViewControlIDArray.Find(inControlID) != kIndex_Invalid );
}
*/

#pragma mark ===========================

#pragma mark ---�t�H�[�J�X����

//#135
/**
�t�H�[�J�X�O���[�v��ݒ�
*/
void	AWindow::NVI_RegisterToFocusGroup( const AControlID inFocusControlID, 
		const ABool inOnly2ndPattern, const ABool inGroup )//#353
{
	//�^�u�t�H�[�J�X�ړ�1st�p�^�[���֓o�^
	//#688 if( inOnly2ndPattern == false )//#353 �t�H�[�J�X2nd�p�^�[��
	{
		mFocusGroup.Add(inFocusControlID);
		mFocusGroup_IsGroup.Add(false);
	}
	//�^�u�t�H�[�J�X�ړ���2nd�p�^�[���֓o�^�i�|�b�v�A�b�v���j���[�E�{�^�������t�H�[�J�X����j #353
	//#688 mFocusGroup2nd.Add(inFocusControlID);
	//#688 mFocusGroup2nd_IsGroup.Add(inGroup);
}

//#135
/**
�t�H�[�J�X�O���[�v����폜
*/
void	AWindow::NVI_UnregisterFromFocusGroup( const AControlID inFocusControlID )
{
	//���݂̃t�H�[�J�X���擾
	AControlID	oldFocus = NVI_GetCurrentFocus();
	//�폜
	AIndex	focusindex = mFocusGroup.Find(inFocusControlID);
	if( focusindex != kIndex_Invalid )
	{
		mFocusGroup.Delete1(focusindex);
		mFocusGroup_IsGroup.Delete1(focusindex);
	}
	/*#688
	//2nd�p�^�[���폜 #353
	AIndex	focusindex2nd = mFocusGroup2nd.Find(inFocusControlID);
	if( focusindex2nd != kIndex_Invalid )
	{
		mFocusGroup2nd.Delete1(focusindex2nd);
		mFocusGroup2nd_IsGroup.Delete1(focusindex2nd);
	}
	*/
	//�t�H�[�J�X�̂���r���[���폜�����ꍇ�́A�t�H�[�J�X������
	/*#688
	if( inFocusControlID == oldFocus )
	{
		//#688 NVI_SwitchFocusToFirst();
		NVI_SwitchFocusTo(kControlID_Invalid);//#688
	}
	 */
}

//�t�H�[�J�X��ݒ肷��
void	AWindow::NVI_SwitchFocusTo( const AControlID inFocusControlID, const ABool inReverseSearch )//#135 #688
{
	if( inFocusControlID == /*#135 mCurrentFocus*/NVI_GetCurrentFocus() )   return;
	
	//#353
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	
	//#135 
	//�A�N�e�B�u�ȃ^�u���łȂ���΁A�^�u���t�H�[�J�X�݂̂�ݒ肵�ďI������ #212 �I���͂��Ȃ��B�^�u���t�H�[�J�X��ݒ肵�ď����p���B
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray_ControlIDs.GetItemCount(); tabIndex++ )
	{
		AIndex	index = mTabDataArray_ControlIDs.GetObject(tabIndex).Find(inFocusControlID);
		if( index != kIndex_Invalid )
		{
			mTabDataArray_LatentFocus.Set(tabIndex,inFocusControlID);//#212
			break;//#212
		}
	}
	//
	mLatentFocus = inFocusControlID;
	if( NVI_IsWindowVisible() == false )
	{
		return;
	}
	//�t�H�[�J�X�ݒ�
	GetImp().SetFocus(inFocusControlID);
	
	//�r���[�̓��e�����I�������s
	NVI_AutomaticSelectBySwitchFocus(inFocusControlID);
	return;
	
#if 0
	//
	if( inFocusControlID != kControlID_Invalid )
	{
		AIndex	focusGroupIndex = focusGroup.Find(inFocusControlID);
		if( focusGroupIndex != kIndex_Invalid )
		{
			//inControlID���珇�ɁAEnable�������^�u�̃R���g���[����������܂Ō������A���������炻����focus��ݒ�
			for( AIndex i = 0; i < focusGroup.GetItemCount(); i++ )
			{
				AControlID	controlID = focusGroup.Get(focusGroupIndex);
				if( NVI_IsControlEnable(controlID) == true && NVI_IsControlVisible(controlID) == true )
				{
					//�t�H�[�J�X�ݒ�
					GetImp().SetFocus(controlID);
					//
					/*#135 if( IsView(controlID) == true )
					{
						NVI_GetViewByControlID(controlID).NVI_AutomaticSelectBySwitchFocus();
					}
					else
					{
						if( GetImp().IsAutoSelectAllBySwitchFocus(controlID) == true )
						{
							GetImp().SetSelectionAll(controlID);
						}
					}*/
					mLatentFocus = controlID;
					return;
				}
				if( inReverseSearch == false )
				{
					focusGroupIndex++;
					if( focusGroupIndex >= focusGroup.GetItemCount() )
					{
						focusGroupIndex = 0;
					}
				}
				else//B0429 �t�H�[�J�X��O�֖߂��ꍇ�́A�L���ȃR���g���[����O�����ɒT���K�v������
				{
					focusGroupIndex--;
					if( focusGroupIndex < 0 )
					{
						focusGroupIndex = focusGroup.GetItemCount()-1;
					}
				}
			}
		}
	}
	//Enable����Visible�ȃR���g���[�����Ȃ��ꍇ�̓t�H�[�J�X���N���A
	GetImp().ClearFocus();
#endif
}

//#135
/**
�t�H�[�J�X���ŏ��̃R���g���[���Ɉړ�����B
*/
void	AWindow::NVI_SwitchFocusToFirst()
{
	//#353
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	
	//
	NVI_SwitchFocusTo(kControlID_Invalid);
	if( focusGroup.GetItemCount() > 0 )
	{
		NVI_SwitchFocusTo(focusGroup.Get(0));
	}
	/*#688
	//#135
	if( IsView(NVI_GetCurrentFocus()) == true )
	{
		NVI_GetViewByControlID(NVI_GetCurrentFocus()).NVI_AutomaticSelectBySwitchFocus();
	}
	else
	{
		if( GetImp().IsAutoSelectAllBySwitchFocus(NVI_GetCurrentFocus()) == true )
		{
			GetImp().SetSelectionAll(NVI_GetCurrentFocus());
		}
	}
	*/
}

//#135
/**
�^�u���̃A�N�e�B�u�t�H�[�J�X���X�V�i�^�u��activate���ꂽ�Ƃ��ɁA�t�H�[�J�X�����j
*/
void	AWindow::UpdateFocusInTab( const AControlID inFocusControlID )
{
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray_ControlIDs.GetItemCount(); tabIndex++ )
	{
		AIndex	index = mTabDataArray_ControlIDs.GetObject(tabIndex).Find(inFocusControlID);
		if( index != kIndex_Invalid )
		{
			mTabDataArray_LatentFocus.Set(tabIndex,inFocusControlID);
			break;
		}
	}
}

//���݂̃t�H�[�J�X�̂���View���擾
AControlID	AWindow::NVI_GetCurrentFocus() const
{
	//#135 return mCurrentFocus;
	return GetImpConst().GetCurrentFocus();//#135
}

//�^�u���t�H�[�J�X���擾
AControlID	AWindow::NVI_GetFocusInTab( const AIndex inTabIndex ) const
{
	return mTabDataArray_LatentFocus.Get(inTabIndex);
}

//#567
/**
���݂̃t�H�[�J�X�̂���View��ViewID���擾
*/
AViewID	AWindow::NVI_GetCurrentFocusViewID() const
{
	AControlID	focusControlID = NVI_GetCurrentFocus();
	if( IsView(focusControlID) == true )
	{
		return NVI_GetViewConstByControlID(focusControlID).GetObjectID();
	}
	else
	{
		return kObjectID_Invalid;
	}
}

//#135
/**
���̃t�H�[�J�X�ֈړ�
*/
void	AWindow::NVI_SwitchFocusToNext()
{
	GetImp().SetFocusNext();
	
	/*#688
	NVI_SwitchFocusTo(GetNextFocusControlID(NVI_GetCurrentFocus()));
	//#135
	if( IsView(NVI_GetCurrentFocus()) == true )
	{
		NVI_GetViewByControlID(NVI_GetCurrentFocus()).NVI_AutomaticSelectBySwitchFocus();
	}
	else
	{
		if( GetImp().IsAutoSelectAllBySwitchFocus(NVI_GetCurrentFocus()) == true )
		{
			GetImp().SetSelectionAll(NVI_GetCurrentFocus());
		}
	}
	*/
	/*#353
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	
	//
	if( focusGroup.GetItemCount() == 0 )   return;
	
	//����Focus����focusIndex�ɓ����
	//����Focus���I���̏ꍇ��Focus����0
	AIndex	focusIndex = 0;
	if( NVI_GetCurrentFocus() != kControlID_Invalid )
	{
		//���݂�Focus�̃C���f�b�N�X������
		focusIndex = focusGroup.Find(NVI_GetCurrentFocus());
		
		if( focusIndex == kIndex_Invalid )
		{
			//���݂�Focus�̃C���f�b�N�X��������Ȃ��ꍇ�i�ӂ��͂��肦�Ȃ��j
			_AError("focus not found",this);
			focusIndex = 0;
		}
		else
		{
			//focusIndex�����ɐi�߂�
			focusIndex++;
			if( focusIndex >= focusGroup.GetItemCount() )
			{
				focusIndex = 0;
			}
		}
	}
	NVI_SwitchFocusTo(focusGroup.Get(focusIndex));
	*/
}

//#135
/**
���̃t�H�[�J�X�ֈړ�
*/
void	AWindow::NVI_SwitchFocusToPrev()
{
	GetImp().SetFocusPrev();
	
	/*#688
	NVI_SwitchFocusTo(GetPreviousFocusControlID(NVI_GetCurrentFocus()));
	//#135
	if( IsView(NVI_GetCurrentFocus()) == true )
	{
		NVI_GetViewByControlID(NVI_GetCurrentFocus()).NVI_AutomaticSelectBySwitchFocus();
	}
	else
	{
		if( GetImp().IsAutoSelectAllBySwitchFocus(NVI_GetCurrentFocus()) == true )
		{
			GetImp().SetSelectionAll(NVI_GetCurrentFocus());
		}
	}
	*/
	/*#353
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	
	if( focusGroup.GetItemCount() == 0 )   return;
	
	//�O��Focus����focusIndex�ɓ����
	//����Focus���I���̏ꍇ��Focus����mFocusGroup.GetItemCount()-1
	AIndex	focusIndex = focusGroup.GetItemCount()-1;
	if( NVI_GetCurrentFocus() != kControlID_Invalid )
	{
		//���݂�Focus�̃C���f�b�N�X������
		focusIndex = focusGroup.Find(NVI_GetCurrentFocus());
		
		if( focusIndex == kIndex_Invalid )
		{
			//���݂�Focus�̃C���f�b�N�X��������Ȃ��ꍇ�i�ӂ��͂��肦�Ȃ��j
			_AError("focus not found",this);
			focusIndex = 0;
		}
		else
		{
			//focusIndex��O�֖߂�
			focusIndex--;
			if( focusIndex < 0 )
			{
				focusIndex = focusGroup.GetItemCount()-1;
			}
		}
	}
	NVI_SwitchFocusTo(focusGroup.Get(focusIndex),true);
	*/
}

//#312
/**
�w��ControlID���^�u���t�H�[�J�X���ǂ�����Ԃ�
*/
ABool	AWindow::NVI_IsFocusInTab( const AControlID inControlID ) const
{
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray_LatentFocus.GetItemCount(); tabIndex++ )
	{
		if( mTabDataArray_LatentFocus.Get(tabIndex) == inControlID )
		{
			return true;
		}
	}
	return false;
}

//#353
/**
���̃t�H�[�J�X�R���g���[�����擾
*/
AControlID	AWindow::GetNextFocusControlID( const AControlID inControlID ) const
{
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	const ABoolArray&	focusGroup_IsGroup = GetFocusGroup_IsGroup();
	
	//������kControlID_Invalid�̏ꍇ�͍ŏ��̃R���g���[����Ԃ�
	if( inControlID == kControlID_Invalid )
	{
		if( focusGroup.GetItemCount() > 0 )
		{
			return focusGroup.Get(0);
		}
		else
		{
			return kControlID_Invalid;
		}
	}
	
	//���݂�index���擾
	AIndex	currentFocusIndex = focusGroup.Find(inControlID);
	if( currentFocusIndex == kIndex_Invalid )   return inControlID;
	
	//���݂̏ꏊ����n�߂�
	AIndex focusIndex = currentFocusIndex;
	//�ŏ���focusable��control��������
	while( true )
	{
		//���Ɉړ�
		//���݈ʒu���O���[�v�����ǂ����ɂ���Ď��̈ʒu���ߕ��@��ς���
		if( focusGroup_IsGroup.Get(focusIndex) == false )
		{
			//���݈ʒu���O���[�v���ł͖�����΁A�P����+1
			focusIndex++;
		}
		else
		{
			//���݈ʒu���O���[�v���Ȃ�A���ɃO���[�v�O�ɂȂ�ӏ��Ɉړ�
			for( ; focusIndex < focusGroup_IsGroup.GetItemCount(); focusIndex++ )
			{
				if( focusGroup_IsGroup.Get(focusIndex) == false )   break;
			}
		}
		//�Ō�ɂȂ�����0�ɂ���
		if( focusIndex == focusGroup.GetItemCount() )
		{
			focusIndex = 0;
		}
		//���ɓ������ꏊ���O���[�v���������ꍇ�Atrue�̏ꏊ�܂ňړ�
		if( focusGroup_IsGroup.Get(focusIndex) == true )
		{
			while( NVI_GetControlBool(focusGroup.Get(focusIndex)) == false )
			{
				focusIndex++;
				if( focusIndex == focusGroup.GetItemCount() )
				{
					focusIndex = 0;
				}
				//�������[�v�h�~
				if( focusIndex == currentFocusIndex )    break;
			}
		}
		//focusable�Ȃ�return
		AControlID	controlID = focusGroup.Get(focusIndex);
		if( IsFocusableControl(controlID) == true )   return controlID;
		
		//�������[�v�h�~
		if( focusIndex == currentFocusIndex )    break;
	}
	return inControlID;
}

//#353
/**
�O�̃t�H�[�J�X�R���g���[�����擾
*/
AControlID	AWindow::GetPreviousFocusControlID( const AControlID inControlID ) const
{
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	const ABoolArray&	focusGroup_IsGroup = GetFocusGroup_IsGroup();
	
	//������kControlID_Invalid�̏ꍇ�͍Ō�̃R���g���[����Ԃ�
	if( inControlID == kControlID_Invalid )
	{
		if( focusGroup.GetItemCount() > 0 )
		{
			return focusGroup.Get(focusGroup.GetItemCount()-1);
		}
		else
		{
			return kControlID_Invalid;
		}
	}
	
	//���݂�index���擾
	AIndex	currentFocusIndex = focusGroup.Find(inControlID);
	if( currentFocusIndex == kIndex_Invalid )   return inControlID;
	
	//���݂̏ꏊ����n�߂�
	AIndex focusIndex = currentFocusIndex;
	//�ŏ���focusable��control��������
	while( true )
	{
		//�O�Ɉړ�
		//���݈ʒu���O���[�v�����ǂ����ɂ���đO�̈ʒu���ߕ��@��ς���
		if( focusGroup_IsGroup.Get(focusIndex) == false )
		{
			//���݈ʒu���O���[�v���ł͖�����΁A�P����-1
			focusIndex--;
		}
		else
		{
			//���݈ʒu���O���[�v���Ȃ�A���ɃO���[�v�O�ɂȂ�ӏ��Ɉړ�
			for( ; focusIndex >= 0; focusIndex-- )
			{
				if( focusGroup_IsGroup.Get(focusIndex) == false )   break;
			}
		}
		//-1�ɂȂ�����Ō�ɂ���
		if( focusIndex < 0 )
		{
			focusIndex = focusGroup.GetItemCount()-1;
		}
		//���ɓ������ꏊ���O���[�v���������ꍇ�Atrue�̏ꏊ�܂ňړ�
		if( focusGroup_IsGroup.Get(focusIndex) == true )
		{
			while( NVI_GetControlBool(focusGroup.Get(focusIndex)) == false )
			{
				focusIndex--;
				if( focusIndex < 0 )
				{
					focusIndex = focusGroup.GetItemCount()-1;
				}
				//�������[�v�h�~
				if( focusIndex == currentFocusIndex )    break;
			}
		}
		//focusable�Ȃ�return
		AControlID	controlID = focusGroup.Get(focusIndex);
		if( IsFocusableControl(controlID) == true )   return controlID;
		
		//�������[�v�h�~
		if( focusIndex == currentFocusIndex )    break;
	}
	return inControlID;
}

//#353
/**
�t�H�[�J�X�\���ǂ�����Ԃ�
*/
ABool	AWindow::IsFocusableControl( const AControlID inControlID ) const
{
	return ( NVI_IsControlEnable(inControlID) == true && NVI_IsControlVisible(inControlID) == true );
}

//#360
/**
�R���g���[����Focus�Ώۂ��ǂ�����Ԃ�
*/
ABool	AWindow::NVI_GetControlSupportFocus( const AControlID inControlID ) const
{
	return (GetFocusGroup().Find(inControlID)!=kIndex_Invalid);
}

#pragma mark ===========================

#pragma mark ---�e��Control/Menu�������^�o�^
//#182

/**
TextEditPane�̓o�^
*/
/*#688
void	AWindow::NVI_RegisterTextEditPane( const AControlID inID, const ABool inWrapMode , const ABool inVScrollbar, const ABool inHScrollBar , const ABool inHasFrame )
{
	GetImp().RegisterTextEditPane(inID,inWrapMode,inVScrollbar,inHScrollBar,inHasFrame);
	NVI_RegisterToFocusGroup(inID);
}
*/

/**
ColorPicker�̓o�^
*/
void	AWindow::NVI_RegisterColorPickerPane( const AControlID inID )
{
	GetImp().RegisterColorPickerPane(inID);
}

/**
�w���v�̓o�^
*/
void	AWindow::NVI_RegisterHelpAnchor( const AControlID inID, AConstCstringPtr inAnchor )
{
	//#539 GetImp().RegisterHelpAnchor(inHelpNumber,inAnchor);
	mHelpAnchorArray_ControlID.Add(inID);
	AText	text;
	text.SetCstring(inAnchor);
	mHelpAnchorArray_Anchor.Add(text);
}

/**
���W�I�O���[�v�o�^
*/
void	AWindow::NVI_RegisterRadioGroup()
{
	//#349 GetImp().RegisterRadioGroup();
	//#349
	mRadioGroup.AddNewObject();
}

/**
���W�I�O���[�v�ւ̒ǉ�
*/
void	AWindow::NVI_AddToLastRegisteredRadioGroup( const AControlID inID )
{
	//#349 GetImp().AddToLastRegisteredRadioGroup(inID);
	//#349
	if( mRadioGroup.GetItemCount() == 0 )   return;
	mRadioGroup.GetObject(mRadioGroup.GetItemCount()-1).Add(inID);
	mRadioGroup_All.Add(inID);
	//#353
	NVI_RegisterToFocusGroup(inID,true,true);
}

//#349
/**
���W�I�{�^���O���[�v��Update�i�w�肵���R���g���[���ȊO�Afalse�ɐݒ肷��j
*/
void	AWindow::UpdateRadioGroup( const AControlID inControlID )
{
	//���W�I�O���[�v�ȊO�Ȃ烊�^�[��
	if( mRadioGroup_All.Find(inControlID) == kIndex_Invalid )   return;
	
	//�w��Control���܂܂�郉�W�I�{�^���O���[�v������
	AIndex	foundRadioGroup = kIndex_Invalid;
	AItemCount	radioGroupCount = mRadioGroup.GetItemCount();
	for( AIndex i = 0; i < radioGroupCount; i++ )
	{
		if( mRadioGroup.GetObjectConst(i).Find(inControlID) != kIndex_Invalid )
		{
			foundRadioGroup = i;
			break;
		}
	}
	//���W�I�u���[�v��������Ȃ���΃��^�[��
	if( foundRadioGroup == kIndex_Invalid )   return;
	
	//inControlID��true�̏ꍇ�A���̃O���[�v��control��false�ɂ���
	if( NVI_GetControlBool(inControlID) == true )
	{
		//
		AItemCount	radioButtonCount = mRadioGroup.GetObject(foundRadioGroup).GetItemCount();
		for( AIndex j = 0; j < radioButtonCount; j++ )
		{
			AControlID	controlID = mRadioGroup.GetObject(foundRadioGroup).Get(j);
			if( controlID != inControlID )
			{
				//���̃O���[�v��control��false�ɂ��āA�f�[�^�X�V����
				NVI_SetControlBool(controlID,false);
				DoValueChanged_DBData(controlID);
			}
		}
	}
}

/**
�X�N���[���o�[����
*/
void	AWindow::NVI_CreateScrollBar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex )
{
	GetImp().CreateScrollBar(inID,inPoint,inWidth,inHeight,inTabIndex);
}

/**
�X�N���[���o�[�o�^
*/
void	AWindow::NVI_RegisterScrollBar( const AControlID inID )
{
	GetImp().RegisterScrollBar(inID);
}

#pragma mark ---WebView
//#734

/**
WebView����
*/
void	AWindow::NVI_CreateWebView( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	GetImp().CreateWebView(inID,inPoint,inWidth,inHeight);
}

/**
WebView��Load�v��
*/
void	AWindow::NVI_LoadURLToWebView( const AControlID inID, const AText& inURL )
{
	GetImp().LoadURLToWebView(inID,inURL);
}

/**
WebView��Reload�v��
*/
void	AWindow::NVI_ReloadWebView( const AControlID inID )
{
	GetImp().ReloadWebView(inID);
}

/**
WebView�̌��݂�URL�擾
*/
void	AWindow::NVI_GetCurrentWebViewURL( const AControlID inID, AText& outURL ) const
{
	GetImpConst().GetCurrentWebViewURL(inID,outURL);
}

/**
WebView��JavaScript���s
*/
void	AWindow::NVI_ExecuteJavaScriptInWebView( const AControlID inID, const AText& inText )
{
	GetImp().ExecuteJavaScriptInWebView(inID,inText);
}

/**
WebView�̕\���{���ݒ�
*/
void	AWindow::NVI_SetWebViewFontMultiplier( const AControlID inID, const AFloatNumber inMultiplier )
{
	GetImp().SetWebViewFontMultiplier(inID,inMultiplier);
}

#pragma mark ===========================

#pragma mark ---�t�@�C���^�t�H���_�I��
//#182

/**
�t�H���_�I��
*/
void	AWindow::NVI_ShowChooseFolderWindow( const AFileAcc& inDefaultFolder, const AText& inMessage,
		const AControlID inVirtualControlID, const ABool inSheet )//#551
{
	GetImp().ShowChooseFolderWindow(inDefaultFolder,inMessage,inVirtualControlID,inSheet);//#551
}

/**
�t�@�C���I��
*/
void	AWindow::NVI_ShowChooseFileWindow( const AFileAcc& inDefaultFile, const AText& inMessage,
		const AControlID inVirtualControlID, const ABool inOnlyApp, 
		const ABool inSheet )
{
	GetImp().ShowChooseFileWindow(inDefaultFile,inMessage,inVirtualControlID,inOnlyApp,inSheet);
}

/**
�t�@�C���I��
*/
void	AWindow::NVI_ShowChooseFileWindowWithFileFilter( const AFileAcc& inDefaultFile, const AControlID inVirtualControlID, const ABool inSheet,
		const AFileAttribute inFilterAttribute )
{
	GetImp().ShowChooseFileWindowWithFileFilter(inDefaultFile,inVirtualControlID,inSheet,inFilterAttribute);
}

/**
�ۑ��E�C���h�E
*/
void	AWindow::NVI_ShowSaveWindow( const AText& inFileName, const ADocumentID inDocumentID, 
		const AFileAttribute inFileAttr, const AText& inFilter, const AText& inRefText, const AFileAcc& inDefaultFolder )
{
	GetImp().ShowSaveWindow(inFileName,inDocumentID,inFileAttr,inFilter,inRefText,inDefaultFolder);
}


#pragma mark ===========================

#pragma mark ---�q�E�C���h�E
//#182

/**
�q�E�C���h�E���OK�_�C�A���O��\��
*/
void	AWindow::NVI_ShowChildWindow_OK( const AText& inMessage1, const AText& inMessage2, const ABool inLongDialog )
{
	//#291 GetImp().ShowChildWindow_OK(inMessage1,inMessage2);
	
	//���ɊJ���Ă���q�V�[�g�E�C���h�E���������烊�^�[��
	if( NVI_IsChildWindowVisible() == true )   return;
	
	//OKCancel�E�C���h�E�\��
	AWindowDefaultFactory<AWindow_OK>	childWindowFactory;
	mOKWindow = AApplication::GetApplication().NVI_CreateWindow(childWindowFactory);
	(dynamic_cast<AWindow_OK&>(AApplication::GetApplication().NVI_GetWindowByID(mOKWindow))).
			SPNVI_CreateAndShow(GetObjectID(),inMessage1,inMessage2,inLongDialog);
}

//#427
/**
�qOK�_�C�A���O�����
*/
void	AWindow::NVI_CloseChildWindow_OK()
{
	(dynamic_cast<AWindow_OK&>(AApplication::GetApplication().NVI_GetWindowByID(mOKWindow))).
			NVI_Close();
	//Close��������ƁAAWindow_OK����NVI_DoChildWindowClosed_OK()���R�[�������B
}

//#427
/**
OK�q�E�C���h�E���OK�{�^����Enable/Disable����
*/
void	AWindow::NVI_SetChildWindow_OK_EnableOKButton( const ABool inEnable )
{
	(dynamic_cast<AWindow_OK&>(AApplication::GetApplication().NVI_GetWindowByID(mOKWindow))).
			SPI_SetEnableOKButton(inEnable);
}

//#427
/**
OK�q�E�C���h�E���Text���b�Z�[�W�ݒ�
*/
void	AWindow::NVI_SetChildWindow_OK_SetText( const AText& inMessage1, const AText& inMessage2 )
{
	(dynamic_cast<AWindow_OK&>(AApplication::GetApplication().NVI_GetWindowByID(mOKWindow))).
			SPI_SetText(inMessage1,inMessage2);
}

/**
OK/Cancel�q�E�C���h�E������ꂽ�Ƃ��̏���
*/
void	AWindow::NVI_DoChildWindowClosed_OK()
{
	AApplication::GetApplication().NVI_DeleteWindow(mOKWindow);
	mOKWindow = kObjectID_Invalid;
}

/**
�q�E�C���h�E���OK/Cancel�_�C�A���O��\��
*/
void	AWindow::NVI_ShowChildWindow_OKCancel( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
		const AControlID inOKButtonVirtualControlID )
{
	//#291 GetImp().ShowChildWindow_OKCancel(inMessage1,inMessage2,inOKButtonMessage,inOKButtonVirtualControlID);
	
	//���ɊJ���Ă���q�V�[�g�E�C���h�E���������烊�^�[��
	if( NVI_IsChildWindowVisible() == true )   return;
	
	//OKCancel�E�C���h�E�\��
	AWindowDefaultFactory<AWindow_OKCancel>	childWindowFactory;
	mOKCancelWindow = AApplication::GetApplication().NVI_CreateWindow(childWindowFactory);
	AApplication::GetApplication().NVI_GetWindowByID(mOKCancelWindow).NVI_Create(kObjectID_Invalid);
	(dynamic_cast<AWindow_OKCancel&>(AApplication::GetApplication().NVI_GetWindowByID(mOKCancelWindow))).
			SPNVI_Show(GetObjectID(),inMessage1,inMessage2,inOKButtonMessage,inOKButtonVirtualControlID);
}

/**
OK/Cancel�q�E�C���h�E������ꂽ�Ƃ��̏���
*/
void	AWindow::NVI_DoChildWindowClosed_OKCancel()
{
	AApplication::GetApplication().NVI_DeleteWindow(mOKCancelWindow);
	mOKCancelWindow = kObjectID_Invalid;
}

/**
�q�E�C���h�E���OK/Cancel/Discard�_�C�A���O��\��
*/
void	AWindow::NVI_ShowChildWindow_OKCancelDiscard( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
		const AControlID inOKButtonVirtualControlID, const AControlID inDiscardButtonVirtualControlID )
{
	//#291 GetImp().ShowChildWindow_OKCancelDiscard(inMessage1,inMessage2,inOKButtonMessage,inOKButtonVirtualControlID,inDiscardButtonVirtualControlID);
	
	//���ɊJ���Ă���q�V�[�g�E�C���h�E���������烊�^�[��
	if( NVI_IsChildWindowVisible() == true )   return;
	
	//OKCancel�E�C���h�E�\��
	AWindowDefaultFactory<AWindow_OKCancelDiscard>	childWindowFactory;
	mOKCancelDiscardWindow = AApplication::GetApplication().NVI_CreateWindow(childWindowFactory);
	AApplication::GetApplication().NVI_GetWindowByID(mOKCancelDiscardWindow).NVI_Create(kObjectID_Invalid);
	(dynamic_cast<AWindow_OKCancelDiscard&>(AApplication::GetApplication().NVI_GetWindowByID(mOKCancelDiscardWindow))).
			SPNVI_Show(GetObjectID(),inMessage1,inMessage2,inOKButtonMessage,inOKButtonVirtualControlID,inDiscardButtonVirtualControlID);
}

/**
OK/Cancel/Discard�q�E�C���h�E������ꂽ�Ƃ��̏���
*/
void	AWindow::NVI_DoChildWindowClosed_OKCancelDiscard()
{
	AApplication::GetApplication().NVI_DeleteWindow(mOKCancelDiscardWindow);
	mOKCancelDiscardWindow = kObjectID_Invalid;
}

//#1034
/**
AskSaveChanges�E�C���h�E
*/
AAskSaveChangesResult	AWindow::NVI_ShowAskSaveChangesWindow( const AText& inFileName, const AObjectID inDocumentID, 
															   const ABool inModal )
{
	//���[�_�����ǂ������L��
	mAskSaveChanges_IsModal = inModal;
	//���[�_���̏ꍇ�̌��ʕۑ��p�����o�ϐ�������
	mAskSaveChanges_Result = kAskSaveChangesResult_None;
	
	//SaveCancelDiscard�E�C���h�E�\��
	AWindowDefaultFactory<AWindow_SaveCancelDiscard>	childWindowFactory;
	mSaveCancelDiscardWindow = AApplication::GetApplication().NVI_CreateWindow(childWindowFactory);
	AApplication::GetApplication().NVI_GetWindowByID(mSaveCancelDiscardWindow).NVI_Create(kObjectID_Invalid);
	AText	message1, message2;
	message1.SetLocalizedText("SaveCancelDiscard_Message1");
	message1.ReplaceParamText('0',inFileName);
	message2.SetLocalizedText("SaveCancelDiscard_Message2");
	//�ۑ��m�F�_�C�A���O�\��
	//inModal=false�̂Ƃ��́A�����ɕԂ��Ă���B
	//inModal=true�̂Ƃ��́A�{�^�����N���b�N����܂ł����Ńu���b�N����BmAskSaveChanges_Result�Ɍ��ʂ�����B
	(dynamic_cast<AWindow_SaveCancelDiscard&>(AApplication::GetApplication().NVI_GetWindowByID(mSaveCancelDiscardWindow))).
			SPNVI_Show(inDocumentID,GetObjectID(),message1,message2,inModal);
	
	return mAskSaveChanges_Result;
	/*#1034
	//���[�_�����ǂ������L��
	mAskSaveChanges_IsModal = inModal;
	//���[�_���̏ꍇ�̌��ʕۑ��p�����o�ϐ�������
	mAskSaveChanges_Result = kAskSaveChangesResult_None;
	//�ۑ��m�F�_�C�A���O�\��
	//inModal=false�̂Ƃ��́A�����ɕԂ��Ă���B�Ԃ�l��kAskSaveChangesResult_None�Œ�B
	//inModal=true�̂Ƃ��́A�{�^�����N���b�N����܂ł����Ńu���b�N����B�Ԃ�l�Ɍ��ʂ�Ԃ��B
	GetImp().ShowAskSaveChangesWindow(inFileName,inDocumentID,inModal);
	return mAskSaveChanges_Result;
	*/
}

/**
OK/Cancel/Discard�q�E�C���h�E������ꂽ�Ƃ��̏���
*/
void	AWindow::NVI_DoChildWindowClosed_SaveCancelDiscard()
{
	AApplication::GetApplication().NVI_DeleteWindow(mSaveCancelDiscardWindow);
	mSaveCancelDiscardWindow = kObjectID_Invalid;
}

/**
�q�E�C���h�E�����
*/
void	AWindow::NVI_CloseChildWindow()
{
	//#291 GetImp().CloseChildWindow();
	//#291
	if( mOKWindow != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mOKWindow).NVI_Close();
	}
	if( mOKCancelWindow != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mOKCancelWindow).NVI_Close();
	}
	if( mOKCancelDiscardWindow != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mOKCancelDiscardWindow).NVI_Close();
	}
	//#1034
	if( mSaveCancelDiscardWindow != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mSaveCancelDiscardWindow).NVI_Close();
	}
}

/* #291
NVI_IsChildWindowVisible()�ɓ���
ABool	AWindow::NVI_ChildWindowVisible() const
{
	return GetImpConst().ChildWindowVisible();
}
*/

//B0411
/**
�q�E�C���h�E��visible���ǂ������擾
*/
ABool	AWindow::NVI_IsChildWindowVisible() const
{
	if( GetImpConst().ChildWindowVisible() == true )   return true;
	if( NVI_IsPrintMode() == true )   return true;
	//#694 �o�^���ꂽ�q�E�C���h�E���\�������ǂ����𒲂ׂ�
	for( AIndex i = 0; i < mChildWindowIDArray.GetItemCount(); i++ )
	{
		AWindowID	winID = mChildWindowIDArray.Get(i);
		if( AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			return true;
		}
	}
	//
	if( NVIDO_IsChildWindowVisible() == true )   return true;
	if( mOKWindow != kObjectID_Invalid )   return true;//#291
	if( mOKCancelWindow != kObjectID_Invalid )   return true;//#291
	if( mOKCancelDiscardWindow != kObjectID_Invalid )   return true;//#291
	if( mSaveCancelDiscardWindow != kObjectID_Invalid )   return true;//#1034
	return false;
}

//#694
/**
�q�E�C���h�E�o�^
*/
void	AWindow::NVI_RegisterChildWindow( const AWindowID inChildWindowID )
{
	mChildWindowIDArray.Add(inChildWindowID);
}

//#694
/**
�q�E�C���h�E�o�^����
*/
void	AWindow::NVI_UnregisterChildWindow( const AWindowID inChildWindowID )
{
	AIndex	index = mChildWindowIDArray.Find(inChildWindowID);
	if( index != kIndex_Invalid )
	{
		mChildWindowIDArray.Delete1(index);
	}
}


#pragma mark ===========================

#pragma mark <���L�N���X(AView)�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�����ʒm�R�[���o�b�N

/*#1031
//View���X�N���[�����ꂽ
void	AWindow::OWICB_ViewScrolled( const AControlID inControlID, const ANumber inDeltaX, const ANumber inDeltaY,
									const ABool inRedraw, const ABool inConstrainToImageSize )
{
	NVIDO_ViewScrolled(inControlID,inDeltaX,inDeltaY,inRedraw,inConstrainToImageSize);
}
*/

//View���̑I��͈͂��ύX���ꂽ
void	AWindow::OWICB_SelectionChanged( const AControlID inControlID )
{
	NVIDO_SelectionChanged(inControlID);
}

#pragma mark ===========================

#pragma mark ---View���ł̃L�[�������ʏ���

//#135
/**
EditBoxView/ListView������ESC�L�[�������ꂽ�ꍇ��EditBox����̃R�[���o�b�N�i�R�}���h+.���܂ށj
*/
void	AWindow::OWICB_ViewEscapeKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//�h���N���X�ł̏����i�Ԃ�ltrue�Ȃ�h���N���X�̏���������return�j
	if( NVIDO_ViewEscapeKeyPressed(inControlID) == true )
	{
		return;
	}
	//�f�t�H���g�L�����Z���{�^���ݒ�ς݂Ȃ�A�f�t�H���g�L�����Z���{�^���N���b�N���̓���
	if( mDefaultCancelButton != kControlID_Invalid )
	{
		EVT_Clicked(mDefaultCancelButton,inModifers);
	}
	//��L�ȊO�ŃE�C���h�E�ɕ���{�^��������ꍇ�͕���{�^���N���b�N���̓���
	else if( NVI_HasCloseButton() == true )
	{
		EVT_DoCloseButton();
	}
}

//#135
/**
EditBoxView/ListView������return�L�[�������ꂽ�ꍇ��EditBox����̃R�[���o�b�N
*/
void	AWindow::OWICB_ViewReturnKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//�h���N���X�ł̏����i�Ԃ�ltrue�Ȃ�h���N���X�̏���������return�j
	if( NVIDO_ViewReturnKeyPressed(inControlID) == true )
	{
		return;
	}
	//EditBox�̒l���ύX���ꂽ���Ƃ��E�C���h�E�i�����j�ɒʒm���A�S�I��
	if( inControlID != kControlID_Invalid )
	{
		EVT_ValueChanged(inControlID);
		NVI_GetViewByControlID(inControlID).NVI_SetSelectAll();
	}
	//�f�t�H���gOK�{�^���ݒ�ς݂Ȃ�A�f�t�H���gOK�{�^���N���b�N���̓���
	if( mDefaultOKButton != kControlID_Invalid )
	{
		if( NVI_IsControlEnable(mDefaultOKButton) == true )
		{
			EVT_Clicked(mDefaultOKButton,inModifers);
		}
	}
}

//#135
/**
EditBoxView/ListView������tab�L�[�������ꂽ�ꍇ��EditBox����̃R�[���o�b�N
*/
void	AWindow::OWICB_ViewTabKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//�h���N���X�ł̏����i�Ԃ�ltrue�Ȃ�h���N���X�̏���������return�j
	if( NVIDO_ViewTabKeyPressed(inControlID) == true )
	{
		return;
	}
	//EditBox�̒l���ύX���ꂽ���Ƃ��E�C���h�E�i�����j�ɒʒm
	if( inControlID != kControlID_Invalid )
	{
		EVT_ValueChanged(inControlID);
	}
	//�t�H�[�J�X�ړ�
	if( AKeyWrapper::IsShiftKeyPressed(inModifers) == true )
	{
		NVI_SwitchFocusToPrev();
	}
	else
	{
		NVI_SwitchFocusToNext();
	}
}

//#798
/**
View�ł́��L�[����������
*/
ABool	AWindow::OWICB_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//�h���N���X�ł̏����i�Ԃ�ltrue�Ȃ�h���N���X�̏���������return�j
	if( NVIDO_ViewArrowUpKeyPressed(inControlID,inModifers) == true )
	{
		return true;
	}
	//
	return false;
}

//#798
/**
View�ł́��L�[����������
*/
ABool	AWindow::OWICB_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//�h���N���X�ł̏����i�Ԃ�ltrue�Ȃ�h���N���X�̏���������return�j
	if( NVIDO_ViewArrowDownKeyPressed(inControlID,inModifers) == true )
	{
		return true;
	}
	//
	return false;
}

//#892
/**
ListView�ł̐܂肽���ݏ�ԕύX������
*/
void	AWindow::OWICB_ListViewExpandedCollapsed( const AControlID inControlID )
{
	NVIDO_ListViewExpandedCollapsed(inControlID);
}

/**
�{�^��view�h���b�O�L�����Z��������
*/
void	AWindow::OWICB_ButtonViewDragCanceled( const AControlID inControlID )
{
	NVIDO_ButtonViewDragCanceled(inControlID);
}

/**
ListView�̃z�o�[�͈͍X�V������
*/
void	AWindow::OWICB_ListViewHoverUpdated( const AControlID inControlID, 
											const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex )
{
	NVIDO_ListViewHoverUpdated(inControlID,inHoverStartDBIndex,inHoverEndDBIndex);
}

#pragma mark ===========================

#pragma mark ---ListView����̏����ʒm

/**
ListView�ł̍s�ړ��������iListView�ł̍s�ړ����AAWindow���f�[�^�ɔ��f������j
@note �I��������Ԃł��邱�ƑO��BListView���őI���������AAWindow�ɂ����f��������ԂŃR�[�������B
*/
void	AWindow::OWICB_ListViewRowMoved( const AControlID inTableControlID, const AIndex inOldRowIndex, const AIndex inNewRowIndex )
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	if( tableIndex != kIndex_Invalid )
	{
		//------------------�f�[�^�̈ړ�------------------
		ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
		NVM_GetDB().MoveRow_Table(firstDataID,inOldRowIndex,inNewRowIndex);
		//------------------�R���g���[���\�����f------------------
		NVM_UpdateControlStatus();
		//------------------�f�[�^�ύX�ʒm------------------
		if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
		{
			//���j���[�̍X�V���̏������ȒP�ɂ��邽�߁Adeleted, added�������g��
			NVMDO_NotifyDataChanged(inTableControlID,kModificationType_RowDeleted,inOldRowIndex,inTableControlID);
			NVMDO_NotifyDataChanged(inTableControlID,kModificationType_RowAdded,inNewRowIndex,inTableControlID);
		}
		//#625 �����I��Ή� �s�ړ��{�^���ɂ��s�ړ��������������Ƃ�ʒm
		NVMDO_NotifyDataChanged(inTableControlID,kModificationType_TransactionCompleted_MoveRow,kIndex_Invalid,inTableControlID);
	}
	//�s�ړ�������
	NVIDO_ListViewRowMoved(inTableControlID,inOldRowIndex,inNewRowIndex);
}

#pragma mark ===========================

#pragma mark ---�R���g���[���̒l�ݒ�^�擾
//#135 #182

/**
�e�L�X�g�ݒ�
*/
void	AWindow::NVI_SetControlText( const AControlID inControlID, const AText& inText )
{
	//#0 �����e�L�X�g�̏ꍇ�A���������Ƀ��^�[��
	AText	text;
	NVI_GetControlText(inControlID,text);
	if( text.Compare(inText) == true )   return;
	//
	if( IsView(inControlID) == false )
	{
		GetImp().SetText(inControlID,inText);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetText(inText);
	}
}

/**
�e�L�X�g�ݒ�
*/
void	AWindow::NVI_AddControlText( const AControlID inControlID, const AText& inText )
{
	if( IsView(inControlID) == false )
	{
		GetImp().AddText(inControlID,inText);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_AddText(inText);
	}
}

/**
�e�L�X�g�擾
*/
void	AWindow::NVI_GetControlText( const AControlID inControlID, AText& outText ) const
{
	if( IsView(inControlID) == false )
	{
		GetImpConst().GetText(inControlID,outText);
	}
	else
	{
		NVI_GetViewConstByControlID(inControlID).NVI_GetText(outText);
	}
}

/**
���l�ݒ�
*/
void	AWindow::NVI_SetControlNumber( const AControlID inControlID, const ANumber inNumber )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetNumber(inControlID,inNumber);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetNumber(inNumber);
	}
}

/**
���l�擾
*/
ANumber	AWindow::NVI_GetControlNumber( const AControlID inControlID ) const
{
	if( IsView(inControlID) == false )
	{
		return GetImpConst().GetNumber(inControlID);
	}
	else
	{
		return NVI_GetViewConstByControlID(inControlID).NVI_GetNumber();
	}
}

/**
���l�擾
*/
void	AWindow::NVI_GetControlNumber( const AControlID inControlID, ANumber& outNumber ) const
{
	if( IsView(inControlID) == false )
	{
		outNumber = GetImpConst().GetNumber(inControlID);
	}
	else
	{
		outNumber = NVI_GetViewConstByControlID(inControlID).NVI_GetNumber();
	}
}

/**
Bool�ݒ�
*/
void	AWindow::NVI_SetControlBool( const AControlID inControlID, const ABool inBool )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetBool(inControlID,inBool);
		
		//#349 ���W�I�O���[�v�X�V����
		UpdateRadioGroup(inControlID);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetBool(inBool);
	}
}

/**
Bool�擾
*/
void	AWindow::NVI_GetControlBool( const AControlID inControlID, ABool& outBool ) const
{
	if( IsView(inControlID) == false )
	{
		outBool = GetImpConst().GetBool(inControlID);
	}
	else
	{
		outBool = NVI_GetViewConstByControlID(inControlID).NVI_GetBool();
	}
}

/**
Bool�擾
*/
ABool	AWindow::NVI_GetControlBool( const AControlID inControlID ) const
{
	if( IsView(inControlID) == false )
	{
		return GetImpConst().GetBool(inControlID);
	}
	else
	{
		return NVI_GetViewConstByControlID(inControlID).NVI_GetBool();
	}
}

/**
Color�ݒ�
*/
void	AWindow::NVI_SetControlColor( const AControlID inControlID, const AColor& inColor )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetColor(inControlID,inColor);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetColor(inColor);
	}
}

/**
Color�擾
*/
void	AWindow::NVI_GetControlColor( const AControlID inControlID, AColor& outColor ) const
{
	if( IsView(inControlID) == false )
	{
		GetImpConst().GetColor(inControlID,outColor);
	}
	else
	{
		NVI_GetViewConstByControlID(inControlID).NVI_GetColor(outColor);
	}
}

/**
Float���l�ݒ�
*/
void	AWindow::NVI_SetControlFloatNumber( const AControlID inControlID, const AFloatNumber inNumber )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetFloatNumber(inControlID,inNumber);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetFloatNumber(inNumber);
	}
}

/**
Float���l�擾
*/
void	AWindow::NVI_GetControlFloatNumber( const AControlID inControlID, AFloatNumber& outNumber ) const
{
	if( IsView(inControlID) == false )
	{
		 GetImpConst().GetFloatNumber(inControlID,outNumber);
	}
	else
	{
		outNumber = NVI_GetViewConstByControlID(inControlID).NVI_GetFloatNumber();
	}
}

/**
�e�L�X�g�t�H���g�ݒ�
*/
void	AWindow::NVI_SetControlTextFont( const AControlID inControlID, const AText& inFontName, const AFloatNumber inFontSize )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetTextFont(inControlID,inFontName,inFontSize);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetTextFont(inFontName,inFontSize);
	}
}

//#688
/**
Progress indicator��progress�l�ݒ�
*/
void	AWindow::NVI_SetProgressIndicatorProgress( const AIndex inProgressIndex, const ANumber inProgress, const ABool inShow )
{
	GetImp().SetProgressIndicatorProgress(inProgressIndex,inProgress,inShow);
}

//#688
/*
void	AWindow::NVI_SetMouseTrackingRect( const AControlID inControlID, const AArray<ALocalRect>& inLocalRectArray )
{
	GetImp().SetMouseTrackingRect(inControlID,inLocalRectArray);
}
*/

//#688
/**
control��font�ݒ�
*/
void	AWindow::NVI_SetControlFont( const AControlID inID, const AText& inFontName, const AFloatNumber inFontSize )
{
	GetImp().SetFont(inID,inFontName,inFontSize);
}

//#688
/**
control��font�擾
*/
void	AWindow::NVI_GetControlFont( const AControlID inID, AText& outFontName, AFloatNumber& outFontSize ) const
{
	GetImpConst().GetFont(inID,outFontName,outFontSize);
}

#pragma mark ===========================

#pragma mark ---�C�����C������
//#182

/**
�C�����C�����͊m��
*/
void	AWindow::NVI_FixInlineInput( const AControlID inControlID )
{
	GetImp().FixInlineInput(inControlID);
}

#pragma mark ===========================

#pragma mark ---�R���g���[����Ԃ̍X�V

//�R���g���[����ԁiEnable/Disable���j���X�V
//NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()����R�[�������
//�N���b�N��A�N�e�B�x�[�g�ŃR�[�������̂ŁA���܂�d��������NVMDO_UpdateControlStatus()�ɂ͓���Ȃ����ƁB
void	AWindow::NVM_UpdateControlStatus()
{
	if( NVI_IsWindowCreated() == false )   return;//#0 ���[�h�ݒ�u���@�v�Ńh�L�������g�ɔ��f�{�^����������CError������������C��
	
	//#427 ���ֈړ��iNVMDO_UpdateControlStatus()��Grayout��Ԃ��X�V���邽�߁B�jNVMDO_UpdateControlStatus();
	
	//DB�A������
	if( IsDBDataTableExist() == true )
	{
		UpdateDBControlStatus();
	}
	
	//#427
	NVMDO_UpdateControlStatus();
}

#pragma mark ===========================

#pragma mark ---DB�A���f�[�^�̓o�^

//�f�[�^�̓o�^�iControlID��DataID�̒l������̏ꍇ�p�j
//DB��̃f�[�^�ƁAUI��̃R���g���[���̑Ή��t��
void	AWindow::NVM_RegisterDBData( const ADataID inDataID, const ABool inNotifyDataChanged, 
		const AControlID inEnablerControlID, const AControlID inDisablerControlID, const ABool inReverseBool )
{
	NVM_RegisterDBData_(inDataID,inDataID,inNotifyDataChanged,inEnablerControlID,inDisablerControlID,inReverseBool);
}

//�f�[�^�̓o�^
//DB��̃f�[�^�ƁAUI��̃R���g���[���̑Ή��t��
//inControlID: UI��̃R���g���[����CotrolID
//inDataID: DB��̃f�[�^��DataID
//inNotifyDataChanged: �R���g���[���̒l�ύX���ɁA�ݒ�ύX���f���������s�����߂�NVICB_NotifyDataChanged()���R�[�����邩�ǂ���
//inEnablerControlID: ���̃R���g���[����Enable/Disable���鑼�̃R���g���[����ControlID�i�����ꍇ��kControlID_Invalid���w��j
void	AWindow::NVM_RegisterDBData_( const AControlID inControlID, const ADataID inDataID, const ABool inNotifyDataChanged, 
		const AControlID inEnablerControlID, const AControlID inDisablerControlID, const ABool inReverseBool )
{
	mDBData_ControlID.Add(inControlID);
	mDBData_DataID.Add(inDataID);
	mDBData_NotifyDataChanged.Add(inNotifyDataChanged);
	mDBData_EnablerControlID.Add(inEnablerControlID);
	mDBData_DisablerControlID.Add(inDisablerControlID);
	mDBData_ReverseBool.Add(inReverseBool);
}

#pragma mark ===========================

#pragma mark ---�f�[�^�^�C�v�ݒ�iDB�A���R���g���[���p�j
//#182

/**
Control�̃f�[�^�^�C�v�ݒ�iDB�A���R���g���[���p�j
*/
void	AWindow::NVM_SetControlDataType( const AControlID inControlID, const ADataType inDataType )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetDataType(inControlID,inDataType);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetDataType(inDataType);
	}
}

#pragma mark ===========================

#pragma mark ---�e�[�u���I��

/**
�e�[�u��View�̍s��I������iContentView�̕ۑ���A���ݑI���s�̍X�V���s���j

@param inControlID �e�[�u��View��ControlID
@param inTableRowDBIndex �s��DBIndex
*/
void	AWindow::NVM_SelectTableRow( const AControlID inControlID, const AIndex inTableRowDBIndex )
{
	/*#205
#if USE_IMP_TABLEVIEW
	NVI_SetControlNumber(inControlID,inTableRow);
#else
	NVI_GetListView(inControlID).SPI_SetSelectByDBIndex(inTableRow);
#endif
	*/
	//�s�I��
	SetTableViewSelectionByDBIndex(inControlID,inTableRowDBIndex);
	//ContentView�̕ۑ�
	NVM_SaveContentView(inControlID);
	//���ݑI���s���̍X�V
	AIndex	tableIndex = mDBTable_ControlID.Find(inControlID);
	mDBTable_CurrentSelect.Set(tableIndex,inTableRowDBIndex);
}

//#205
/**
�e�[�u��View�̍s��I������iContentView�̕ۑ���A���ݑI���s�̍X�V�͂��Ȃ��j

@param inControlID �e�[�u��View��ControlID
@param inTableRowDBIndex �s��DBIndex
*/
void	AWindow::SetTableViewSelectionByDBIndex( const AControlID inControlID, const AIndex inTableRowDBIndex )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inControlID))
	{
	  case kTableViewType_OSControl:
		{
			NVI_SetControlNumber(inControlID,inTableRowDBIndex);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inControlID).SPI_SetSelectByDBIndex(inTableRowDBIndex);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

//#205
/**
DB�A���e�[�u���Ō��ݑI�𒆂̍s��(DB)Index��Ԃ��i�����I���̎���kIndex_Invalid��Ԃ��j
*/
AIndex	AWindow::NVI_GetTableViewSelectionDBIndex( const AControlID inTableControlID ) const
{
	AIndex	rowIndex = kIndex_Invalid;
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			rowIndex = NVI_GetControlNumber(inTableControlID);
			break;
		}
	  case kTableViewType_ListView:
		{
			rowIndex = NVI_GetListViewConst(inTableControlID).SPI_GetSelectedDBIndex();
			//#625 �����I�����\�ɂ���B�����I���̏ꍇ�́A��{�I�ɂ́i�폜�{�^�����ȊO�́j�A���I�����Ɠ��������ɂ���B
			AArray<AIndex>	DBIndexArray;
			SPI_GetTableViewSelectionDBIndexArray(inTableControlID,DBIndexArray);
			if( DBIndexArray.GetItemCount() >= 2 )
			{
				//���I������
				rowIndex = kIndex_Invalid;
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return rowIndex;
}

//#625
/**
�I�����ڂ��擾�i�����I���̎��͕����̍��ڂ��擾����j
*/
void	AWindow::SPI_GetTableViewSelectionDBIndexArray( const AControlID inTableControlID, AArray<AIndex>& outDBIndexArray ) const
{
	outDBIndexArray.DeleteAll();
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			AIndex	rowIndex = NVI_GetControlNumber(inTableControlID);
			if( rowIndex != kIndex_Invalid )
			{
				outDBIndexArray.Add(rowIndex);
			}
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListViewConst(inTableControlID).SPI_GetSelectedDBIndexArray(outDBIndexArray);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

//#205
/**
DB�A���e�[�u���Ō��ݑI�𒆂̗��DataID��Ԃ�
����AkDataID_Invalid�Œ�A�������́A�Ō�ɃN���b�N�������Ԃ��i��I�𖢑Ή��j
*/
ADataID	AWindow::NVI_GetTableViewSelectionColumnID( const AControlID inTableControlID ) const
{
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			return kDataID_Invalid;
		}
	  case kTableViewType_ListView:
		{
			return NVI_GetListViewConst(inTableControlID).SPI_GetLastClickedColumnID();
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return kDataID_Invalid;
}

#pragma mark ===========================

#pragma mark ---�e�[�u���ҏW
//#205

/**
DB�A���e�[�u���̕ҏW�J�n�i�s�͌��ݑI�𒆂̍s�j
*/
void	AWindow::NVI_StartTableColumnEditMode( const AControlID inTableControlID, const ADataID inColumnID )
{
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().EnterColumnEditMode(inTableControlID,inColumnID);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_StartEditMode(inColumnID);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

#pragma mark ===========================

#pragma mark ---DB�A���e�[�u���f�[�^�̓o�^

//�e�[�u���̓o�^
//inTableControlID: �e�[�u���R���g���[����ControlID
//inFirstDataID: 
void	AWindow::NVM_RegisterDBTable( const AControlID inTableControlID, const ADataID inFirstDataID,
		const AControlID inAddRowControlID, const AControlID inDeleteRowControlID, const AControlID inDeleteRowOKControlID,
		const AControlID inUpID, const AControlID inDownID, const AControlID inTopID, const AControlID inBottomID,
		const AControlID inContentGroupControlID, const ABool inNotifyDataChanged,
		const ABool inFrame, const ABool inHeader, const ABool inVScrollBar, const ABool inHScrollBar, 
		const ATableViewType inTableViewType  )//win 080618
{
	//#205
	//inTableViewType�ɂ���Đ�������R���g���[����؂�ւ�
	switch(inTableViewType)
	{
	  case kTableViewType_OSControl:
		{
			GetImp().RegisterTableView(inTableControlID,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid);//R0183
			break;
		}
	  case kTableViewType_ListView:
		{
			//ListView����
			AListViewFactory	listViewFactory(GetObjectID(),inTableControlID);
			NVM_CreateView(inTableControlID,false,listViewFactory);
			
			//�t���[������
			if( inFrame == true )
			{
				NVI_GetListView(inTableControlID).SPI_CreateFrame(inHeader,inVScrollBar,inHScrollBar);
			}
			
			//#625 ListView�̕����I�����\�ɂ���
			NVI_GetListView(inTableControlID).SPI_SetMultiSelectEnabled(true);
			//
			NVI_GetListView(inTableControlID).SPI_SetEnableDragDrop(true);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	
	mDBTable_ControlID.Add(inTableControlID);
	mDBTable_FirstDataID.Add(inFirstDataID);
	mDBTable_AddRow_ControlID.Add(inAddRowControlID);
	mDBTable_DeleteRow_ControlID.Add(inDeleteRowControlID);
	mDBTable_DeleteRowOK_ControlID.Add(inDeleteRowOKControlID);
	mDBTable_Up_ControlID.Add(inUpID);
	mDBTable_Down_ControlID.Add(inDownID);
	mDBTable_Top_ControlID.Add(inTopID);
	mDBTable_Bottom_ControlID.Add(inBottomID);
	mDBTable_ColumnIDArray.AddNewObject();
	mDBTable_DataIDArray.AddNewObject();
	mDBTable_ColumnEditButtonIDArray.AddNewObject();
	mDBTable_ContentIDArray.AddNewObject();
	mDBTable_ContentReverseBool.AddNewObject();//#1032
	mDBTable_SpecializedContentIDArray.AddNewObject();//B0406
	mDBTable_ContentDataIDArray.AddNewObject();
	mDBTable_ContentGroupControlID.Add(inContentGroupControlID);
	mDBTable_NotifyDataChanged.Add(inNotifyDataChanged);
	mDBTable_CurrentSelect.Add(kIndex_Invalid);
	mDBTable_CurrentContentViewRowIndex.Add(kIndex_Invalid);//B0406
	mDBTable_CurrentContentViewDBID.Add(kObjectID_Invalid);//B0406
	mDBTable_ListViewType.Add(inTableViewType);//#205
	
	//���ԕύX�{�^��
	/*#688
	if( inUpID != kControlID_Invalid )
	{
		NVI_SetControlIcon(inUpID,kIconID_Abs_RowMoveUp);
	}
	if( inDownID != kControlID_Invalid )
	{
		NVI_SetControlIcon(inDownID,kIconID_Abs_RowMoveDown);
	}
	if( inTopID != kControlID_Invalid )
	{
		NVI_SetControlIcon(inTopID,kIconID_Abs_RowMoveTop);
	}
	if( inBottomID != kControlID_Invalid )
	{
		NVI_SetControlIcon(inBottomID,kIconID_Abs_RowMoveBottom);
	}*/
	
	//content�O���[�v
	if( inContentGroupControlID != kControlID_Invalid )//win 080707
	{
		NVI_SetControlEnable(inContentGroupControlID,false);
	}
}

//��̓o�^
void	AWindow::NVM_RegisterDBTableColumn( const AControlID inTableControlID, const ADataID inColumnID, const ADataID inDataID, 
		const AControlID inEditButtonControlID, const ANumber inColumnWidth, const AConstCharPtr inTitle, const ABool inEditable )
{
	//#205 TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().RegisterTableViewColumn(inTableControlID,inColumnID,NVM_GetDB().GetDataType(inDataID));
			break;
		}
	  case kTableViewType_ListView:
		{
			if( inDataID != kDataID_Invalid )
			{
				switch(NVM_GetDB().GetDataType(inDataID))
				{
				  case kDataType_BoolArray:
					{
						NVI_GetListView(inTableControlID).SPI_RegisterColumn_Bool(inColumnID,inColumnWidth,inTitle,inEditable);
						break;
					}
				  case kDataType_NumberArray:
					{
						NVI_GetListView(inTableControlID).SPI_RegisterColumn_Number(inColumnID,inColumnWidth,inTitle,inEditable);
						break;
					}
				  case kDataType_TextArray:
					{
						NVI_GetListView(inTableControlID).SPI_RegisterColumn_Text(inColumnID,inColumnWidth,inTitle,inEditable);
						break;
					}
				  case kDataType_ColorArray:
					{
						//NVI_GetListView(inTableControlID).SPI_RegisterColumn_Color(inColumnID);
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
	}
	
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	mDBTable_ColumnIDArray.GetObject(tableIndex).Add(inColumnID);
	mDBTable_DataIDArray.GetObject(tableIndex).Add(inDataID);
	mDBTable_ColumnEditButtonIDArray.GetObject(tableIndex).Add(inEditButtonControlID);
}

//��̓o�^
void	AWindow::NVM_RegisterDBTableColumn( const AControlID inTableControlID, const ADataID inColumnID, const ADataType inDataType,
		const AControlID inEditButtonControlID, const ANumber inColumnWidth, const AConstCharPtr inTitle, const ABool inEditable )
{
	//#205 TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().RegisterTableViewColumn(inTableControlID,inColumnID,inDataType);
			break;
		}
	  case kTableViewType_ListView:
		{
			switch(inDataType)
			{
			  case kDataType_BoolArray:
				{
					NVI_GetListView(inTableControlID).SPI_RegisterColumn_Bool(inColumnID,inColumnWidth,inTitle,inEditable);
					break;
				}
			  case kDataType_NumberArray:
				{
					NVI_GetListView(inTableControlID).SPI_RegisterColumn_Number(inColumnID,inColumnWidth,inTitle,inEditable);
					break;
				}
			  case kDataType_TextArray:
				{
					NVI_GetListView(inTableControlID).SPI_RegisterColumn_Text(inColumnID,inColumnWidth,inTitle,inEditable);
					break;
				}
			  case kDataType_ColorArray:
				{
					//NVI_GetListView(inTableControlID).SPI_RegisterColumn_Color(inColumnID);
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
	
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	mDBTable_ColumnIDArray.GetObject(tableIndex).Add(inColumnID);
	mDBTable_DataIDArray.GetObject(tableIndex).Add(kDataID_Invalid);
	mDBTable_ColumnEditButtonIDArray.GetObject(tableIndex).Add(inEditButtonControlID);
}

//#205
/**
TableViewType���擾
*/
ATableViewType	AWindow::GetTableViewType( const AControlID inTableControlID ) const
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	return mDBTable_ListViewType.Get(tableIndex);
}

#pragma mark ===========================

#pragma mark --- DB�A���e�[�u��View�\���f�[�^�̐ݒ�
//#205

/**
�e�[�u��View�\���f�[�^�ݒ�J�n
*/
void	AWindow::NVI_BeginSetTable( const AControlID inTableControlID )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().BeginSetTable(inTableControlID);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_BeginSetTable();
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
�e�[�u��View�\���f�[�^�ݒ�(Bool)
*/
void	AWindow::NVI_SetTableColumn_Bool( const AControlID inTableControlID, const ADataID inColumnID, const ABoolArray& inArray )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetTable_Bool(inTableControlID,inColumnID,inArray);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetColumn_Bool(inColumnID,inArray);
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
�e�[�u��View�\���f�[�^�ݒ�(Number)
*/
void	AWindow::NVI_SetTableColumn_Number( const AControlID inTableControlID, const ADataID inColumnID, const ANumberArray& inArray )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetTable_Number(inTableControlID,inColumnID,inArray);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetColumn_Number(inColumnID,inArray);
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
�e�[�u��View�\���f�[�^�ݒ�(Text)
*/
void	AWindow::NVI_SetTableColumn_Text( const AControlID inTableControlID, const ADataID inColumnID, const ATextArray& inArray )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetTable_Text(inTableControlID,inColumnID,inArray);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetColumn_Text(inColumnID,inArray);
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
�e�[�u��View�\���f�[�^�ݒ�(Color)
*/
void	AWindow::NVI_SetTableColumn_Color( const AControlID inTableControlID, const ADataID inColumnID, const AColorArray& inArray )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetTable_Color(inTableControlID,inColumnID,inArray);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetColumn_Color(inColumnID,inArray);
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
�e�[�u��View�\���f�[�^�ݒ�(Icon)
*/
/*#1012
void	AWindow::NVI_SetTableColumn_Icon( const AControlID inTableControlID, const ADataID inColumnID, const ANumberArray& inArray )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetColumnIcon(inTableControlID,inColumnID,inArray);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetColumnIcon(inColumnID,inArray);
			break;
		}
	}
}
*/

/**
�e�[�u��View�\���f�[�^�ݒ�I��
*/
void	AWindow::NVI_EndSetTable( const AControlID inTableControlID )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().EndSetTable(inTableControlID);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_EndSetTable();
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

//#205
/**
�e�[�u���ւ̃����O�X0�̃e�L�X�g���͂��֎~����
*/
void	AWindow::NVM_SetInhibit0LengthForTable( const AControlID inTableControlID, const ABool inInhibit0Length )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetInhibit0LengthForTable(inTableControlID,inInhibit0Length);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetInhibit0Length(inInhibit0Length);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

#pragma mark ===========================

#pragma mark --- DB�A���e�[�u��View�\���f�[�^�̎擾
//#205

/**
�e�[�u��View�\���f�[�^�ݒ�(Bool)
*/
const ABoolArray&	AWindow::NVI_GetTableColumn_Bool( const AControlID inTableControlID, const ADataID inColumnID )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			return GetImp().GetColumn_BoolArray(inTableControlID,inColumnID);
		}
	  case kTableViewType_ListView:
		{
			return NVI_GetListView(inTableControlID).SPI_GetColumn_Bool(inColumnID);
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return GetEmptyBoolArray();
}

/**
�e�[�u��View�\���f�[�^�ݒ�(Number)
*/
const ANumberArray&	AWindow::NVI_GetTableColumn_Number( const AControlID inTableControlID, const ADataID inColumnID )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			return GetImp().GetColumn_NumberArray(inTableControlID,inColumnID);
		}
	  case kTableViewType_ListView:
		{
			return NVI_GetListView(inTableControlID).SPI_GetColumn_Number(inColumnID);
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return GetEmptyNumberArray();
}

/**
�e�[�u��View�\���f�[�^�ݒ�(Text)
*/
const ATextArray&	AWindow::NVI_GetTableColumn_Text( const AControlID inTableControlID, const ADataID inColumnID )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			return GetImp().GetColumn_TextArray(inTableControlID,inColumnID);
		}
	  case kTableViewType_ListView:
		{
			return NVI_GetListView(inTableControlID).SPI_GetColumn_Text(inColumnID);
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return GetEmptyTextArray();
}

/**
�e�[�u��View�\���f�[�^�ݒ�(Color)
*/
const AColorArray&	AWindow::NVI_GetTableColumn_Color( const AControlID inTableControlID, const ADataID inColumnID )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			return GetImp().GetColumn_ColorArray(inTableControlID,inColumnID);
		}
	  case kTableViewType_ListView:
		{
			return NVI_GetListView(inTableControlID).SPI_GetColumn_Color(inColumnID);
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return GetEmptyColorArray();
}

#pragma mark ===========================

#pragma mark --- DB�A���e�[�u��View��̕��ݒ�^�擾
//#205

/**
�e�[�u��View��̕��ݒ�
*/
void	AWindow::NVI_SetTableColumnWidth( const AControlID inTableControlID, const ADataID inColumnID, const ANumber inWidth )
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetColumnWidth(inTableControlID,inColumnID,inWidth);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetColumnWidth(inColumnID,inWidth);
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
�e�[�u��View��̕��擾
*/
ANumber	AWindow::NVI_GetTableColumnWidth( const AControlID inTableControlID, const ADataID inColumnID ) const
{
	//TableViewType�ɂ���ď����؂�ւ�
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			return GetImpConst().GetColumnWidth(inTableControlID,inColumnID);
		}
	  case kTableViewType_ListView:
		{
			return NVI_GetListViewConst(inTableControlID).SPI_GetColumnWidth(inColumnID);
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return 100;
}

#pragma mark ===========================

#pragma mark --- DB�A���u�e�[�u�����e�\��View�v�̓o�^

//�u�e�[�u�����e�\��View�v�̓o�^
void	AWindow::NVM_RegisterDBTableContentView( const AControlID inTableControlID, const AControlID inContentControlID, const ADataID inDataID,
												 const ABool inReverseBool )//#1032
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	mDBTable_ContentIDArray.GetObject(tableIndex).Add(inContentControlID);
	mDBTable_ContentDataIDArray.GetObject(tableIndex).Add(inDataID);
	mDBTable_ContentReverseBool.GetObject(tableIndex).Add(inReverseBool);//#1032
}

//B0406
//ContentView �̃R���g���[���̂����AAWindow�������Ǘ������A�p���N���X�ŊǗ�������̂�o�^����B
//�����œo�^�����R���g���[����AWindow���ύX���邱�Ƃ͂Ȃ��B
//���Y�R���g���[�����ύX���ꂽ�Ƃ��ɁA�Ή�����table�������āANVM_SaveContentView()���R�[�����邽�߂����Ɏg�p����B
void	AWindow::NVM_RegisterDBTableSpecializedContentView( const AControlID inTableControlID, const AControlID inContentControlID )
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	mDBTable_SpecializedContentIDArray.GetObject(tableIndex).Add(inContentControlID);
}

#pragma mark ===========================

#pragma mark --- ContentView�Ǘ�

//�u�e�[�u�����eView�v�̒l���e�[�u��DB�֐ݒ肵�A�e�[�u���\���X�V B0406 ������NVM_SetDBTableDataFromContentView()���烁�\�b�h���ύX
void	AWindow::NVM_SaveContentView( const AControlID inTableControlID )
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	if( tableIndex == kIndex_Invalid )   return;
	AIndex	rowIndex = mDBTable_CurrentContentViewRowIndex.Get(tableIndex);//B0406 mDBTable_CurrentSelect.Get(tableIndex);
	if( rowIndex == kIndex_Invalid )   return;
	//B0406 Open����DB�Ɠ���DB�ɕۑ����邱�Ƃ�ۏ؂��邽��
	if( NVM_GetDB()./*#693GetDBID()*/GetObjectID() != mDBTable_CurrentContentViewDBID.Get(tableIndex) )
	{
		_ACError("DB changed",this);
		return;
	}
	
	for( AIndex contentIndex = 0; contentIndex < mDBTable_ContentIDArray.GetObject(tableIndex).GetItemCount(); contentIndex++ )
	{
		AControlID	contentControlID = mDBTable_ContentIDArray.GetObject(tableIndex).Get(contentIndex);
		ADataID	dataID = mDBTable_ContentDataIDArray.GetObject(tableIndex).Get(contentIndex);
		ABool	reverseBool = mDBTable_ContentReverseBool.GetObject(tableIndex).Get(contentIndex);//#1032
		if( dataID != kDataID_Invalid )
		{
			switch(NVM_GetDB().GetDataType(dataID))
			{
			  case kDataType_BoolArray:
				{
					ABool	data;
					NVI_GetControlBool(contentControlID,data);
					//#1032
					if( reverseBool == true )
					{
						data = !data;
					}
					//
					NVM_GetDB().SetData_BoolArray(dataID,rowIndex,data);
					break;
				}
			  case kDataType_NumberArray:
				{
					ANumber	data;
					NVI_GetControlNumber(contentControlID,data);
					NVM_GetDB().SetData_NumberArray(dataID,rowIndex,data);
					//�f�[�^���񔽉f
					data = NVM_GetDB().GetData_NumberArray(dataID,rowIndex);
					NVI_SetControlNumber(contentControlID,data);
					break;
				}
			  case kDataType_TextArray:
				{
					AText	data;
					NVI_GetControlText(contentControlID,data);
					NVM_GetDB().SetData_TextArray(dataID,rowIndex,data);
					//�f�[�^���񔽉f
					NVM_GetDB().GetData_TextArray(dataID,rowIndex,data);
					NVI_SetControlText(contentControlID,data);
					break;
				}
			  case kDataType_ColorArray:
				{
					AColor	data;
					NVI_GetControlColor(contentControlID,data);
					NVM_GetDB().SetData_ColorArray(dataID,rowIndex,data);
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
		}
		//B0406 NVMDO_SetDBTableDataFromContentView(inTableControlID,contentControlID,rowIndex);
	}
	NVMDO_SaveSpecializedContentView(inTableControlID,rowIndex);//B0406
	NVM_UpdateDBTableView(inTableControlID);
}

//�u�e�[�u�����eView�v�̕\�����A���݂̃e�[�u���I���s�ƁADB�̃f�[�^�ɍ��킹�čX�V
void	AWindow::OpenContentView( const AControlID inTableControlID )
{
	/*#205
#if USE_IMP_TABLEVIEW
	AIndex	rowIndex = NVI_GetControlNumber(inTableControlID);
#else
	AIndex	rowIndex = NVI_GetListView(inTableControlID).SPI_GetSelectedDBIndex();
	#endif
	*/
	AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(inTableControlID);//#205
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	if( tableIndex != kIndex_Invalid )
	{
		//#391
		//AView_EditBox::EVTDO_DoViewFocusDeactivated()�ɂ�EVT_ValueChanged()���R�[�����邱�Ƃɂ������߁A
		//���ɂ���NVI_SetControlEnable()����ResetFocus()���R�[������A
		//mDBTable_CurrentContentViewRowIndex, mDBTable_CurrentContentViewDBID���O�̒l�̂܂�
		//EVT_ValueChanged()�o�R��NVM_SaveContentView()���R�[������A
		//�ύX�r���̃f�[�^���O�̃C���f�b�N�X�ɕۑ�������肪���������B
		//�����h�����߂ɁA���mDBTable_CurrentContentViewRowIndex, mDBTable_CurrentContentViewDBID���N���A���Ă���
		mDBTable_CurrentContentViewRowIndex.Set(tableIndex,kIndex_Invalid);
		mDBTable_CurrentContentViewDBID.Set(tableIndex,kObjectID_Invalid);
		
		//Content�O���[�v��Enable/Disable�̐ݒ�
		AControlID	contentGroupControlID = mDBTable_ContentGroupControlID.Get(tableIndex);
		if( contentGroupControlID != kControlID_Invalid )
		{
			NVI_SetControlEnable(contentGroupControlID,(rowIndex!=kIndex_Invalid));
		}
		//
		for( AIndex contentItemIndex = 0; contentItemIndex < mDBTable_ContentIDArray.GetObject(tableIndex).GetItemCount(); contentItemIndex++ )
		{
			AControlID	contentControlID = mDBTable_ContentIDArray.GetObject(tableIndex).Get(contentItemIndex);
			if( rowIndex == kIndex_Invalid )
			{
				NVI_SetControlEnable(contentControlID,false);
				//#182 GetImp().SetEmptyValue(contentControlID);
				ADataID	dataID = mDBTable_ContentDataIDArray.GetObject(tableIndex).Get(contentItemIndex);
				if( dataID != kDataID_Invalid )
				{
					switch(NVM_GetDB().GetDataType(dataID))
					{
					  case kDataType_BoolArray:
						{
							NVI_SetControlBool(contentControlID,false);
							break;
						}
					  case kDataType_NumberArray:
						{
							NVI_SetControlNumber(contentControlID,0);
							break;
						}
					  case kDataType_TextArray:
						{
							NVI_SetControlText(contentControlID,GetEmptyText());
							break;
						}
					  case kDataType_ColorArray:
						{
							NVI_SetControlColor(contentControlID,kColor_Gray);
							break;
						}
					  default:
						{
							//�����Ȃ�
							break;
						}
					}
				}
			}
			else
			{
				NVI_SetControlEnable(contentControlID,true);
				ADataID	dataID = mDBTable_ContentDataIDArray.GetObject(tableIndex).Get(contentItemIndex);
				ABool	reverseBool = mDBTable_ContentReverseBool.GetObject(tableIndex).Get(contentItemIndex);//#1032
				if( dataID != kDataID_Invalid )
				{
					switch(NVM_GetDB().GetDataType(dataID))
					{
					  case kDataType_BoolArray:
						{
							ABool	data = NVM_GetDB().GetData_BoolArray(dataID,rowIndex);
							//#1032
							if( reverseBool == true )
							{
								data = !data;
							}
							//
							NVI_SetControlBool(contentControlID,data);
							break;
						}
					  case kDataType_NumberArray:
						{
							ANumber	data = NVM_GetDB().GetData_NumberArray(dataID,rowIndex);
							NVI_SetControlNumber(contentControlID,data);
							break;
						}
					  case kDataType_TextArray:
						{
							AText	text;
							NVM_GetDB().GetData_TextArray(dataID,rowIndex,text);
							NVI_SetControlText(contentControlID,text);
							break;
						}
					  case kDataType_ColorArray:
						{
							AColor	data;
							NVM_GetDB().GetData_ColorArray(dataID,rowIndex,data);
							NVI_SetControlColor(contentControlID,data);
							break;
						}
					  default:
						{
							//�����Ȃ�
							break;
						}
					}
				}
				//B0406 NVMDO_UpdateTableContentView(inTableControlID,contentControlID);
				NVI_ClearControlUndoInfo(contentControlID);//B0323
				/*#688
				//ContentView���̍ŏ��̍��ڂփt�H�[�J�X�ړ�
				if( contentItemIndex == 0 && sIsFocusGroup2ndPattern == false )//#353 �t���L�[�{�[�h�A�N�Z�X�̏ꍇ�̓t�H�[�J�X�ړ����Ȃ�
				{
					NVI_SwitchFocusTo(contentControlID);
				}
				*/
			}
		}
		NVMDO_OpenSpecializedContentView(inTableControlID,rowIndex);//B0406
		//B0406
		mDBTable_CurrentContentViewRowIndex.Set(tableIndex,rowIndex);
		mDBTable_CurrentContentViewDBID.Set(tableIndex,NVM_GetDB()./*#693GetDBID()*/GetObjectID());
	}
}


//
/**
�u�e�[�u�����eView�v�̒��̃R���g���[���̒l���ύX���ꂽ�Ƃ��̏���
*/
ABool	AWindow::DoValueChanged_DBTableContentView( const AControlID inControlID )
{
	for( AIndex tableIndex = 0; tableIndex < mDBTable_ControlID.GetItemCount(); tableIndex++ )
	{
		for( AIndex contentIndex = 0; contentIndex < mDBTable_ContentIDArray.GetObject(tableIndex).GetItemCount(); contentIndex++ )
		{
			AControlID	contentControlID = mDBTable_ContentIDArray.GetObject(tableIndex).Get(contentIndex);
			if( inControlID == contentControlID )
			{
				AControlID	tableControlID = mDBTable_ControlID.Get(tableIndex);
				NVM_SaveContentView(tableControlID);
				if( mDBTable_NotifyDataChanged.Get(tableIndex) == true && mDBTable_CurrentSelect.Get(tableIndex) != kIndex_Invalid )
				{
					NVMDO_NotifyDataChanged(tableControlID,kModificationType_ValueChanged,mDBTable_CurrentSelect.Get(tableIndex),inControlID/*B0406*/);
				}
				return true;
			}
		}
		//B0406
		for( AIndex contentIndex = 0; contentIndex < mDBTable_SpecializedContentIDArray.GetObject(tableIndex).GetItemCount(); contentIndex++ )
		{
			AControlID	contentControlID = mDBTable_SpecializedContentIDArray.GetObject(tableIndex).Get(contentIndex);
			if( inControlID == contentControlID )
			{
				AControlID	tableControlID = mDBTable_ControlID.Get(tableIndex);
				NVM_SaveContentView(tableControlID);
				if( mDBTable_NotifyDataChanged.Get(tableIndex) == true && mDBTable_CurrentSelect.Get(tableIndex) != kIndex_Invalid )
				{
					NVMDO_NotifyDataChanged(tableControlID,kModificationType_ValueChanged,mDBTable_CurrentSelect.Get(tableIndex),inControlID/*B0406*/);
				}
				return true;
			}
		}
	}
	return false;
}

#pragma mark ===========================

#pragma mark ---DB�A���e�[�u����Update

//�e�[�u���̕\����DB�̃f�[�^�ɍ��킹�čX�V
void	AWindow::NVM_UpdateDBTableView( const AControlID inTableControlID )
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	if( tableIndex != kIndex_Invalid )
	{
		//�e�[�u���f�[�^�ݒ�J�n
		NVI_BeginSetTable(inTableControlID);
		//�e�񂲂�
		for( AIndex columnIndex = 0; columnIndex < mDBTable_ColumnIDArray.GetObject(tableIndex).GetItemCount(); columnIndex++ )
		{
			ADataID	columnID = mDBTable_ColumnIDArray.GetObject(tableIndex).Get(columnIndex);
			AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
			ADataID	dataID = mDBTable_DataIDArray.GetObject(tableIndex).Get(columnIndex);
			if( dataID != kDataID_Invalid )
			{
				switch(NVM_GetDB().GetDataType(dataID))
				{
				  case kDataType_BoolArray:	
					{
						NVI_SetTableColumn_Bool(inTableControlID,columnID,NVM_GetDB().GetData_ConstBoolArrayRef(dataID));
						break;
					}
				  case kDataType_NumberArray:
					{
						NVI_SetTableColumn_Number(inTableControlID,columnID,NVM_GetDB().GetData_ConstNumberArrayRef(dataID));
						break;
					}
				  case kDataType_TextArray:
					{
						NVI_SetTableColumn_Text(inTableControlID,columnID,NVM_GetDB().GetData_ConstTextArrayRef(dataID));
						break;
					}
				  case kDataType_ColorArray:
					{
						NVI_SetTableColumn_Color(inTableControlID,columnID,NVM_GetDB().GetData_ConstColorArrayRef(dataID));
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
			}
			//�h���N���X�ł̏���
			NVMDO_UpdateTableView(inTableControlID,columnID);
		}
		//�e�[�u���f�[�^�ݒ�I��
		NVI_EndSetTable(inTableControlID);
/*#205
#if USE_IMP_TABLEVIEW
		GetImp().BeginSetTable(inTableControlID);
		for( AIndex columnIndex = 0; columnIndex < mDBTable_ColumnIDArray.GetObject(tableIndex).GetItemCount(); columnIndex++ )
		{
			ADataID	columnID = mDBTable_ColumnIDArray.GetObject(tableIndex).Get(columnIndex);
			
			AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
			//B0000 for�̒��Œ�`�ς�AIndex	columnIndex = mDBTable_ColumnIDArray.GetObject(tableIndex).Find(columnID);
			ADataID	dataID = mDBTable_DataIDArray.GetObject(tableIndex).Get(columnIndex);
			if( dataID != kDataID_Invalid )
			{
				switch(NVM_GetDB().GetDataType(dataID))
				{
				  case kDataType_BoolArray:
					{
						GetImp().SetTable_Bool(inTableControlID,columnID,NVM_GetDB().GetData_ConstBoolArrayRef(dataID));
						break;
					}
				  case kDataType_NumberArray:
					{
						GetImp().SetTable_Number(inTableControlID,columnID,NVM_GetDB().GetData_ConstNumberArrayRef(dataID));
						break;
					}
				  case kDataType_TextArray:
					{
						GetImp().SetTable_Text(inTableControlID,columnID,NVM_GetDB().GetData_ConstTextArrayRef(dataID));
						break;
					}
				  case kDataType_ColorArray:
					{
						GetImp().SetTable_Color(inTableControlID,columnID,NVM_GetDB().GetData_ConstColorArrayRef(dataID));
						break;
					}
				}
			}
			NVMDO_UpdateTableView(inTableControlID,columnID);
		}
		GetImp().EndSetTable(inTableControlID);
#else
		NVI_GetListView(inTableControlID).SPI_BeginSetTable();
		for( AIndex columnIndex = 0; columnIndex < mDBTable_ColumnIDArray.GetObject(tableIndex).GetItemCount(); columnIndex++ )
		{
			ADataID	columnID = mDBTable_ColumnIDArray.GetObject(tableIndex).Get(columnIndex);
			
			AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
			//B0000 for�̒��Œ�`�ς�AIndex	columnIndex = mDBTable_ColumnIDArray.GetObject(tableIndex).Find(columnID);
			ADataID	dataID = mDBTable_DataIDArray.GetObject(tableIndex).Get(columnIndex);
			if( dataID != kDataID_Invalid )
			{
				switch(NVM_GetDB().GetDataType(dataID))
				{
				  case kDataType_BoolArray:
					{
						NVI_GetListView(inTableControlID).SPI_SetColumn_Bool(columnID,NVM_GetDB().GetData_ConstBoolArrayRef(dataID));
						break;
					}
				  case kDataType_NumberArray:
					{
						NVI_GetListView(inTableControlID).SPI_SetColumn_Number(columnID,NVM_GetDB().GetData_ConstNumberArrayRef(dataID));
						break;
					}
				  case kDataType_TextArray:
					{
						NVI_GetListView(inTableControlID).SPI_SetColumn_Text(columnID,NVM_GetDB().GetData_ConstTextArrayRef(dataID));
						break;
					}
				  case kDataType_ColorArray:
					{
						NVI_GetListView(inTableControlID).SPI_SetColumn_Color(columnID,NVM_GetDB().GetData_ConstColorArrayRef(dataID));
						break;
					}
				}
			}
			NVMDO_UpdateTableView(inTableControlID,columnID);
		}
		NVI_GetListView(inTableControlID).SPI_EndSetTable();
#endif
*/
	}
}

#pragma mark ===========================

#pragma mark <�������W���[��>

#pragma mark ===========================

#pragma mark ---DB�A���f�[�^����H

ABool	AWindow::IsDBDataTableExist()
{
	return ((mDBData_ControlID.GetItemCount()>0)||(mDBTable_ControlID.GetItemCount()>0));
}

#pragma mark ===========================

#pragma mark ---DB�A���f�[�^��Update

void	AWindow::UpdateDBDataAndTable()
{
	//�o�^�f�[�^�̒l��DB����R���g���[���֔��f
	for( AIndex index = 0; index < mDBData_ControlID.GetItemCount(); index++ )
	{
		AControlID	controlID = mDBData_ControlID.Get(index);
		ADataID	dataID = mDBData_DataID.Get(index);
		
		ADataType	datatype = NVM_GetDB().GetDataType(dataID);
		NVM_SetControlDataType(controlID,datatype);
		switch(datatype)
		{
		  case kDataType_Bool:
			{
				if( mDBData_ReverseBool.Get(index) == false )
				{
					NVI_SetControlBool(controlID,NVM_GetDB().GetData_Bool(dataID));
				}
				else
				{
					NVI_SetControlBool(controlID,(NVM_GetDB().GetData_Bool(dataID)==false));
				}
				break;
			}
		  case kDataType_Number:
			{
				NVI_SetControlNumber(controlID,NVM_GetDB().GetData_Number(dataID));
				break;
			}
		  case kDataType_FloatNumber:
			{
				NVI_SetControlFloatNumber(controlID,NVM_GetDB().GetData_FloatNumber(dataID));
				break;
			}
		  case kDataType_Color:
			{
				AColor	color;
				NVM_GetDB().GetData_Color(dataID,color);
				NVI_SetControlColor(controlID,color);
				break;
			}
		  case kDataType_Text:
			{
				AText	text;
				NVM_GetDB().GetData_Text(dataID,text);
				NVI_SetControlText(controlID,text);
				break;
			}
		  /*win case kDataType_Font:
			{
				GetImp().SetFont(controlID,NVM_GetDB().GetData_Font(dataID));//�b��
				break;
			}*/
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
	//�o�^�e�[�u���f�[�^�̒l��DB����R���g���[���֔��f
	for( AIndex index = 0; index < mDBTable_ControlID.GetItemCount(); index++ )
	{
		//�e�[�u���\���X�V
		NVM_UpdateDBTableView(mDBTable_ControlID.Get(index));
		//�u�e�[�u�����eView�v�\���X�V
		OpenContentView(mDBTable_ControlID.Get(index));
	}
	//�R���g���[����ԍX�V
	UpdateDBControlStatus();
}

//�R���g���[����ԍX�V
void	AWindow::UpdateDBControlStatus()
{
	for( AIndex	dataIndex = 0; dataIndex < mDBData_DataID.GetItemCount(); dataIndex++ )
	{
		AControlID	enablerControlID = mDBData_EnablerControlID.Get(dataIndex);
		if( enablerControlID != kControlID_Invalid )
		{
			ABool	enabled = NVI_GetControlBool(enablerControlID);//#349
			if( GetImpConst().IsControlEnabled(enablerControlID) == false )   enabled = false;//#349 enabler��grayout���Ȃ���grayout�ɂ���
			NVI_SetControlEnable(/*#243 mDBData_DataID*/mDBData_ControlID.Get(dataIndex),enabled);//#349 NVI_GetControlBool(enablerControlID));
		}
		AControlID	disablerControlID = mDBData_DisablerControlID.Get(dataIndex);
		if( disablerControlID != kControlID_Invalid )
		{
			ABool	enabled = (NVI_GetControlBool(disablerControlID)==false);//#349
			if( GetImpConst().IsControlEnabled(disablerControlID) == false )   enabled = false;//#349 enabler��grayout���Ȃ���grayout�ɂ���
			NVI_SetControlEnable(/*#243 mDBData_DataID*/mDBData_ControlID.Get(dataIndex),enabled);//#349 (NVI_GetControlBool(disablerControlID)==false));
		}
	}
	
	for( AIndex	tableIndex = 0; tableIndex < mDBTable_ControlID.GetItemCount(); tableIndex++ )
	{
		AControlID	tableControlID = mDBTable_ControlID.Get(tableIndex);
		
		/** #205
#if USE_IMP_TABLEVIEW
		AIndex	rowIndex = NVI_GetControlNumber(tableControlID);
#else
		AIndex	rowIndex = NVI_GetListView(tableControlID).SPI_GetSelectedDBIndex();
#endif
		*/
		AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(tableControlID);//#205
		ABool	rowSelected = ( rowIndex != kIndex_Invalid);
		AControlID	id = mDBTable_DeleteRow_ControlID.Get(tableIndex);
		if( id != kControlID_Invalid )
		{
			NVI_SetControlEnable(id,rowSelected);
		}
		/* win 080703 id = mDBTable_DeleteRowOK_ControlID.Get(tableIndex);
		if( id != kControlID_Invalid )
		{
			NVI_SetControlEnable(id,rowSelected);
		}*/
		id = mDBTable_Up_ControlID.Get(tableIndex);
		if( id != kControlID_Invalid )
		{
			NVI_SetControlEnable(id,rowSelected);
		}
		id = mDBTable_Down_ControlID.Get(tableIndex);
		if( id != kControlID_Invalid )
		{
			NVI_SetControlEnable(id,rowSelected);
		}
		id = mDBTable_Top_ControlID.Get(tableIndex);
		if( id != kControlID_Invalid )
		{
			NVI_SetControlEnable(id,rowSelected);
		}
		id = mDBTable_Bottom_ControlID.Get(tableIndex);
		if( id != kControlID_Invalid )
		{
			NVI_SetControlEnable(id,rowSelected);
		}
		for( AIndex i = 0; i < mDBTable_ColumnEditButtonIDArray.GetObject(tableIndex).GetItemCount(); i++ )
		{
			id = mDBTable_ColumnEditButtonIDArray.GetObject(tableIndex).Get(i);
			if( id != kControlID_Invalid )
			{
				NVI_SetControlEnable(id,rowSelected);
			}
		}
	}
}

#pragma mark ===========================

#pragma mark ---DB�A���f�[�^�̃N���b�N������

//�e�[�u���N���b�N����
ABool	AWindow::DoClicked_DBTable( const AControlID inControlID )
{
	//�e�[�u���I��
	AIndex	tableIndex = mDBTable_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		NVM_SaveContentView(inControlID);
		//CurrentSelect��ݒ�B�����I���̏ꍇ�ANVI_GetTableViewSelectionDBIndex()��kIndex_Invalid��Ԃ��B
		mDBTable_CurrentSelect.Set(tableIndex,NVI_GetTableViewSelectionDBIndex(inControlID));//#205
		//
		OpenContentView(inControlID);
		return true;
	}
	
	//�e�[�u���s�ǉ��{�^��
	tableIndex = mDBTable_AddRow_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		//�f�[�^�̒ǉ�
		ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
		NVM_GetDB().AddRow_Table(firstDataID);
		//#625 NVMDO_NotifyDataChanged()�Ɩ����d���̂��߁A������͍폜 NVI_AddTableRow(tableID);
		NVM_UpdateDBTableView(tableID);
		//�ŏI�s��I��
		SetTableViewSelectionByDBIndex(tableID,NVM_GetDB().GetRowCount_Table(firstDataID)-1);//#205
		mDBTable_CurrentSelect.Set(tableIndex,NVI_GetTableViewSelectionDBIndex(tableID));//#205
		//
		OpenContentView(tableID);
		NVM_UpdateControlStatus();
		//�f�[�^�ύX�ʒm
		if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
		{
			NVMDO_NotifyDataChanged(tableID,kModificationType_RowAdded,NVI_GetTableViewSelectionDBIndex(tableID),inControlID);//#205
		}
		//#625 �����I��Ή� �s�ǉ��{�^���ɂ��s�ǉ��������������Ƃ�ʒm
		NVMDO_NotifyDataChanged(tableID,kModificationType_TransactionCompleted_AddNew,kIndex_Invalid,inControlID);
		return true;
	}
	//�e�[�u���s�폜�{�^��
	tableIndex = mDBTable_DeleteRow_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		AArray<AIndex>	rowIndexArray;//#625
		SPI_GetTableViewSelectionDBIndexArray(tableID,rowIndexArray);//#625
		if( rowIndexArray.GetItemCount() > 0 )//#625
		{
			AText	mes1, mes2, mes3;
			mes1.SetLocalizedText("ModePref_DeleteGeneral1");
			mes2.SetLocalizedText("ModePref_DeleteGeneral2");
			mes3.SetLocalizedText("ModePref_DeleteGeneral3");
			NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,mDBTable_DeleteRowOK_ControlID.Get(tableIndex));
		}
		return true;
	}
	//�e�[�u���s�폜OK�{�^��
	tableIndex = mDBTable_DeleteRowOK_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		AArray<AIndex>	rowIndexArray;//#625
		SPI_GetTableViewSelectionDBIndexArray(tableID,rowIndexArray);//#625
		SetTableViewSelectionByDBIndex(tableID,kIndex_Invalid);//�I������ #625
		rowIndexArray.Sort(false);//DB��̌��̃f�[�^�̂ق�����폜���邽�߂ɍ~���\�[�g����#625
		for( AIndex i = 0; i < rowIndexArray.GetItemCount(); i++ )//#625
		{
			AIndex	rowIndex = rowIndexArray.Get(i);//#625
			//�f�[�^�̍폜
			ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
			NVM_GetDB().DeleteRow_Table(firstDataID,rowIndex);
			mDBTable_CurrentSelect.Set(tableIndex,kIndex_Invalid);
			NVM_UpdateDBTableView(tableID);
			OpenContentView(tableID);
			NVM_UpdateControlStatus();
			//�f�[�^�ύX�ʒm
			if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
			{
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowDeleted,rowIndex,inControlID/*B0406*/);
			}
		}
		//#625 �����I��Ή� �S�Ă̑I���s�̍폜�������������Ƃ�ʒm
		NVMDO_NotifyDataChanged(tableID,kModificationType_TransactionCompleted_DeleteRows,kIndex_Invalid,inControlID);
		return true;
	}
	//�e�[�u���sUp�{�^�� R0183
	//#205 //��DBIndex�x�[�X�ł̏����ɂȂ��Ă���̂ŁA�\�[�g�\������ꍇ�A�\���s�x�[�X�ɑΉ��K�v
	tableIndex = mDBTable_Up_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(tableID);//#205
		if( rowIndex != kIndex_Invalid && rowIndex > 0)
		{
			//�f�[�^�̈ړ�
			ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
			NVM_GetDB().MoveRow_Table(firstDataID,rowIndex,rowIndex-1);
			NVM_UpdateDBTableView(tableID);
			//�ړ���̍s��I��
			SetTableViewSelectionByDBIndex(tableID,rowIndex-1);//#205
			mDBTable_CurrentSelect.Set(tableIndex,NVI_GetTableViewSelectionDBIndex(tableID));//#205
			//
			OpenContentView(tableID);
			NVM_UpdateControlStatus();
			//�f�[�^�ύX�ʒm
			if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
			{
				//���j���[�̍X�V���̏������ȒP�ɂ��邽�߁Adeleted, added�������g��
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowDeleted,rowIndex,inControlID/*B0406*/);
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowAdded,NVI_GetTableViewSelectionDBIndex(tableID),inControlID);//#205
			}
		}
		//#625 �����I��Ή� �s�ړ��{�^���ɂ��s�ړ��������������Ƃ�ʒm
		NVMDO_NotifyDataChanged(tableID,kModificationType_TransactionCompleted_MoveRow,kIndex_Invalid,inControlID);
		return true;
	}
	//�e�[�u���sDown�{�^�� R0183
	tableIndex = mDBTable_Down_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
		AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(tableID);//#205
		if( rowIndex != kIndex_Invalid && rowIndex < NVM_GetDB().GetItemCount_Array(firstDataID)-1 )
		{
			//�f�[�^�̈ړ�
			NVM_GetDB().MoveRow_Table(firstDataID,rowIndex,rowIndex+1);
			NVM_UpdateDBTableView(tableID);
			//�ړ���̍s��I��
			SetTableViewSelectionByDBIndex(tableID,rowIndex+1);//#205
			mDBTable_CurrentSelect.Set(tableIndex,NVI_GetTableViewSelectionDBIndex(tableID));//#205
			//
			OpenContentView(tableID);
			NVM_UpdateControlStatus();
			//�f�[�^�ύX�ʒm
			if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
			{
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowDeleted,rowIndex,inControlID/*B0406*/);
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowAdded,NVI_GetTableViewSelectionDBIndex(tableID),inControlID);//#205
			}
		}
		//#625 �����I��Ή� �s�ړ��{�^���ɂ��s�ړ��������������Ƃ�ʒm
		NVMDO_NotifyDataChanged(tableID,kModificationType_TransactionCompleted_MoveRow,kIndex_Invalid,inControlID);
		return true;
	}
	//�e�[�u���sTop�{�^�� R0183
	tableIndex = mDBTable_Top_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(tableID);//#205
		if( rowIndex != kIndex_Invalid && rowIndex > 0 )
		{
			//�f�[�^�̈ړ�
			ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
			NVM_GetDB().MoveRow_Table(firstDataID,rowIndex,0);
			NVM_UpdateDBTableView(tableID);
			//�ړ���̍s��I��
			SetTableViewSelectionByDBIndex(tableID,0);//#205
			mDBTable_CurrentSelect.Set(tableIndex,NVI_GetTableViewSelectionDBIndex(tableID));//#205
			//
			OpenContentView(tableID);
			NVM_UpdateControlStatus();
			//�f�[�^�ύX�ʒm
			if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
			{
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowDeleted,rowIndex,inControlID/*B0406*/);
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowAdded,NVI_GetTableViewSelectionDBIndex(tableID),inControlID);//#205
			}
		}
		//#625 �����I��Ή� �s�ړ��{�^���ɂ��s�ړ��������������Ƃ�ʒm
		NVMDO_NotifyDataChanged(tableID,kModificationType_TransactionCompleted_MoveRow,kIndex_Invalid,inControlID);
		return true;
	}
	//�e�[�u���sBottom�{�^�� R0183
	tableIndex = mDBTable_Bottom_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
		AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(tableID);//#205
		if( rowIndex != kIndex_Invalid && rowIndex < NVM_GetDB().GetItemCount_Array(firstDataID)-1 )
		{
			//�f�[�^�̈ړ�
			NVM_GetDB().MoveRow_Table(firstDataID,rowIndex,NVM_GetDB().GetItemCount_Array(firstDataID)-1);
			NVM_UpdateDBTableView(tableID);
			//�ړ���̍s��I��
			SetTableViewSelectionByDBIndex(tableID,NVM_GetDB().GetItemCount_Array(firstDataID)-1);//#205
			mDBTable_CurrentSelect.Set(tableIndex,NVI_GetTableViewSelectionDBIndex(tableID));//#205
			//
			OpenContentView(tableID);
			NVM_UpdateControlStatus();
			//�f�[�^�ύX�ʒm
			if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
			{
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowDeleted,rowIndex,inControlID/*B0406*/);
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowAdded,NVI_GetTableViewSelectionDBIndex(tableID),inControlID);//#205
			}
		}
		//#625 �����I��Ή� �s�ړ��{�^���ɂ��s�ړ��������������Ƃ�ʒm
		NVMDO_NotifyDataChanged(tableID,kModificationType_TransactionCompleted_MoveRow,kIndex_Invalid,inControlID);
		return true;
	}
	//�ҏW�{�^��
	for( tableIndex = 0; tableIndex < mDBTable_ControlID.GetItemCount(); tableIndex++ )
	{
		AIndex	index = mDBTable_ColumnEditButtonIDArray.GetObject(tableIndex).Find(inControlID);
		if( index != kIndex_Invalid )
		{
			NVI_StartTableColumnEditMode(mDBTable_ControlID.Get(tableIndex),mDBTable_ColumnIDArray.GetObject(tableIndex).Get(index));
			return true;
		}
	}
	return false;
}

#pragma mark ===========================

#pragma mark ---DB�A���f�[�^�̒l�ύX������

//DBData�l�ύX������
ABool	AWindow::DoValueChanged_DBData( const AControlID inControlID )
{
	AIndex	dataIndex = mDBData_ControlID.Find(inControlID);
	if( dataIndex != kIndex_Invalid )
	{
		ABool	changed = false;
		ADataID	dataID = mDBData_DataID.Get(dataIndex);
		switch(NVM_GetDB().GetDataType(dataID))
		{
		  case kDataType_Bool:
			{
				ABool	data;
				NVI_GetControlBool(inControlID,data);
				if( mDBData_ReverseBool.Get(dataIndex) == true )
				{
					data = (data==false);
				}
				if( NVM_GetDB().GetData_Bool(dataID) != data )
				{
					NVM_GetDB().SetData_Bool(dataID,data);
					changed = true;
				}
				break;
			}
		  case kDataType_Number:
			{
				ANumber	data;
				NVI_GetControlNumber(inControlID,data);
				if( NVM_GetDB().GetData_Number(dataID) != data )
				{
					NVM_GetDB().SetData_Number(dataID,data);
					//DB��̒l�����̌��ʂ�UI�֔��f������
					NVI_SetControlNumber(inControlID,NVM_GetDB().GetData_Number(dataID));
					changed = true;
				}
				break;
			}
		  case kDataType_FloatNumber:
			{
				AFloatNumber	data;
				NVI_GetControlFloatNumber(inControlID,data);
				if( NVM_GetDB().GetData_FloatNumber(dataID) != data )
				{
					NVM_GetDB().SetData_FloatNumber(dataID,data);
					//DB��̒l�����̌��ʂ�UI�֔��f������
					NVI_SetControlFloatNumber(inControlID,NVM_GetDB().GetData_FloatNumber(dataID));
					changed = true;
				}
				break;
			}
		  case kDataType_Color:
			{
				AColor	data;
				NVI_GetControlColor(inControlID,data);
				AColor	olddata;
				NVM_GetDB().GetData_Color(dataID,olddata);
				if( AColorWrapper::CompareColor(olddata,data) == false )
				{
					NVM_GetDB().SetData_Color(dataID,data);
					changed = true;
				}
				break;
			}
		  case kDataType_Text:
			{
				AText	data;
				NVI_GetControlText(inControlID,data);
				AText	olddata;
				NVM_GetDB().GetData_Text(dataID,olddata);
				if( data.Compare(olddata) == false )
				{
					NVM_GetDB().SetData_Text(dataID,data);
					changed = true;
				}
				break;
			}
		  /*win case kDataType_Font:
			{
				AFont	data;
				GetImp().GetFont(inControlID,data);//�b��
				if( NVM_GetDB().GetData_Font(dataID) != data )
				{
					NVM_GetDB().SetData_Font(dataID,data);
					changed = true;
				}
				break;
			}*/
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		if( mDBData_NotifyDataChanged.Get(dataIndex) == true && changed == true )
		{
			NVMDO_NotifyDataChanged(inControlID,kModificationType_ValueChanged,kIndex_Invalid,inControlID/*B0406*/);
		}
		return true;
	}
	return false;
}

//UI��DB
//�e�[�u��(UI)������������ꂽ�Ƃ��A�R�[�������B
//�e�[�u��(UI)����f�[�^�i�z��j���擾���ADB�փR�s�[����B���̌�ADB�ォ��AUI�֋t�R�s�[���邱�ƂŁADB��̒l�����̌��ʂ�UI�֔��f������B
ABool	AWindow::DoValueChanged_DBTable( const AControlID inTableControlID )
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	if( tableIndex != kIndex_Invalid )
	{
		for( AIndex columnIndex = 0; columnIndex < mDBTable_ColumnIDArray.GetObject(tableIndex).GetItemCount(); columnIndex++ )
		{
			ADataID	columnID = mDBTable_ColumnIDArray.GetObject(tableIndex).Get(columnIndex);
			ADataID	dataID = mDBTable_DataIDArray.GetObject(tableIndex).Get(columnIndex);
			if( dataID == kIndex_Invalid )   continue;
			
			//
			switch(NVM_GetDB().GetDataType(dataID))
			{
			  case kDataType_BoolArray:
				{
					NVM_GetDB().SetData_BoolArray(dataID,NVI_GetTableColumn_Bool(inTableControlID,columnID));
					break;
				}
			  case kDataType_NumberArray:
				{
					NVM_GetDB().SetData_NumberArray(dataID,NVI_GetTableColumn_Number(inTableControlID,columnID));
					break;
				}
			  case kDataType_TextArray:
				{
					NVM_GetDB().SetData_TextArray(dataID,NVI_GetTableColumn_Text(inTableControlID,columnID));
					break;
				}
			  case kDataType_ColorArray:
				{
					NVM_GetDB().SetData_ColorArray(dataID,NVI_GetTableColumn_Color(inTableControlID,columnID));
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			/*#205
#if USE_IMP_TABLEVIEW
			switch(NVM_GetDB().GetDataType(dataID))
			{
			  case kDataType_BoolArray:
				{
					NVM_GetDB().SetData_BoolArray(dataID,GetImp().GetColumn_BoolArray(inTableControlID,columnID));
					break;
				}
			  case kDataType_NumberArray:
				{
					NVM_GetDB().SetData_NumberArray(dataID,GetImp().GetColumn_NumberArray(inTableControlID,columnID));
					break;
				}
			  case kDataType_TextArray:
				{
					NVM_GetDB().SetData_TextArray(dataID,GetImp().GetColumn_TextArray(inTableControlID,columnID));
					break;
				}
			  case kDataType_ColorArray:
				{
					NVM_GetDB().SetData_ColorArray(dataID,GetImp().GetColumn_ColorArray(inTableControlID,columnID));
					break;
				}
			}
#else
			switch(NVM_GetDB().GetDataType(dataID))
			{
			  case kDataType_BoolArray:
				{
					NVM_GetDB().SetData_BoolArray(dataID,NVI_GetListView(inTableControlID).SPI_GetColumn_Bool(columnID));
					break;
				}
			  case kDataType_NumberArray:
				{
					NVM_GetDB().SetData_NumberArray(dataID,NVI_GetListView(inTableControlID).SPI_GetColumn_Number(columnID));
					break;
				}
			  case kDataType_TextArray:
				{
					NVM_GetDB().SetData_TextArray(dataID,NVI_GetListView(inTableControlID).SPI_GetColumn_Text(columnID));
					break;
				}
			  case kDataType_ColorArray:
				{
					NVM_GetDB().SetData_ColorArray(dataID,NVI_GetListView(inTableControlID).SPI_GetColumn_Color(columnID));
					break;
				}
			}
#endif
			*/
		}
		if( mDBTable_NotifyDataChanged.Get(tableIndex) == true && mDBTable_CurrentSelect.Get(tableIndex) != kIndex_Invalid )
		{
			NVMDO_NotifyDataChanged(inTableControlID,kModificationType_ValueChanged,mDBTable_CurrentSelect.Get(tableIndex),inTableControlID/*B0406*/);
		}
		//DB��̒l�����̌��ʂ�UI�֔��f������
		NVM_UpdateDBTableView(inTableControlID);
		return true;
	}
	return false;
}
void	AWindow::NVI_DoValueChanged( const AControlID inID )
{
	if( IsDBDataTableExist() == true )
	{
		if( DoValueChanged_DBData(inID) == true )   return;
		if( DoValueChanged_DBTable(inID) == true )   return;
		if( DoValueChanged_DBTableContentView(inID) == true )   return;
	}
}

void	AWindow::NVM_StoreToDBAll()
{
	for( AIndex i = 0; i < mDBData_ControlID.GetItemCount(); i++ )
	{
		NVI_DoValueChanged(mDBData_ControlID.Get(i));
	}
	for( AIndex i = 0; i < mDBTable_ContentIDArray.GetItemCount(); i++ )
	{
		for( AIndex j = 0; j < mDBTable_ContentIDArray.GetObject(i).GetItemCount(); j++ )
		{
			NVI_DoValueChanged(mDBTable_ContentIDArray.GetObject(i).Get(j));
		}
	}
	//��mDBTable_SpecializedContentIDArray���K�v�H
	//��������ContentView�̃R���g���[�����ɑ΂���DoValueChanged���s����̂͂��������i���������Ȃ��j�H
	for( AIndex i = 0; i < mDBTable_ControlID.GetItemCount(); i++ )
	{
		NVI_DoValueChanged(mDBTable_ControlID.Get(i));
	}
}

#pragma mark ===========================

#pragma mark ---ListView

//ListView�����i�t���[���A�X�N���[���o�[�����\�[�X��ɒ�`����Ă���ꍇ�p�j
//inListViewControlID: ListView��ControlID
//inListViewFrameControlID: �t���[����ControlID�i�t���[�������̏ꍇ��kControlID_Invalid)
//inScrollBarControlID: VScrollBar��ControlID�iVScrollBar�����̏ꍇ��kControlID_Invalid)
/*#205
void	AWindow::NVI_CreateListView( const AControlID inListViewControlID, const AControlID inListViewFrameControlID,
		const AControlID inScrollBarControlID )
{
	if( inListViewFrameControlID != kControlID_Invalid )
	{
		AListFrameViewFactory	listFrameViewFactory(GetObjectID(),inListViewFrameControlID,inListViewControlID,false,kControlID_Invalid,kControlID_Invalid);
		NVM_CreateView(inListViewFrameControlID,false,listFrameViewFactory);
		mListFrameViewControlIDArray.Add(inListViewFrameControlID);
	}
	
	//ListView����
	AListViewFactory	listViewFactory(GetObjectID(),inListViewControlID);
	NVM_CreateView(inListViewControlID,false,listViewFactory);
	mListViewControlIDArray.Add(inListViewControlID);
	
	NVI_GetListView(inListViewControlID).NVI_SetScrollBarControlID(kControlID_Invalid,inScrollBarControlID);
}
*/

//#205
/**
ListView�����i�t���[���A�X�N���[���o�[�����\�[�X��ɒ�`����Ă���ꍇ�p�j

@param inListViewControlID ListView��ControlID�iNib��rsrc�̃��\�[�X�t�@�C���ɒ�`�ς݂�ControlID�j
*/
void	AWindow::NVI_CreateListViewFromResource( const AControlID inListViewControlID )
{
	AListViewFactory	listViewFactory(GetObjectID(),inListViewControlID);
	NVM_CreateView(inListViewControlID,false,listViewFactory);
	//#205 mListViewControlIDArray.Add(inListViewControlID);
}

//ListView�����i�t���[���A�X�N���[���o�[�̓��\�[�X��ɒ�`����Ă��炸�AListView�݂̂���`����Ă���ꍇ�p�j
//inListViewControlID: ListView��ControlID
//inListViewFrameControlID: �t���[����ControlID�i�t���[�������̏ꍇ��kControlID_Invalid)
//inScrollBarControlID: VScrollBar��ControlID�iVScrollBar�����̏ꍇ��kControlID_Invalid)
//win 080618 Frame�Ƀw�b�_�[�@�\�ƃX�N���[���o�[�@�\��ǉ��BListView�̊O���Ƀt���[���A�w�b�_�[�A�X�N���[���o�[���\������
/*#205
void	AWindow::NVI_CreateListView( const AControlID inListViewControlID, const ABool inFrame, const ABool inHeader, 
		const ABool inVScrollBar, const ABool inHScrollBar )
{
	AControlID	frameControlID = kControlID_Invalid;
	AControlID	vScrollbarControlID = kControlID_Invalid;
	AControlID	hScrollbarControlID = kControlID_Invalid;

#if USE_IMP_TABLEVIEW
#else
	if( inFrame == true )
	{
		AIndex	tabIndex = GetImp().GetControlEmbeddedTabIndex(inListViewControlID);
		frameControlID = NVM_AssignDynamicControlID();
		AWindowPoint	pt = {0,0};
		if( inVScrollBar == true )
		{
			vScrollbarControlID = NVM_AssignDynamicControlID();
			NVI_CreateScrollBar(vScrollbarControlID,pt,10,20,tabIndex);
		}
		if( inHScrollBar == true )
		{
			hScrollbarControlID = NVM_AssignDynamicControlID();
			NVI_CreateScrollBar(hScrollbarControlID,pt,20,10,tabIndex);
		}
		AListFrameViewFactory	listFrameViewFactory(this,*this,frameControlID,inListViewControlID,inHeader,vScrollbarControlID,hScrollbarControlID);
		NVM_CreateView(frameControlID,pt,10,10,false,listFrameViewFactory,tabIndex,false);
		mListFrameViewControlIDArray.Add(frameControlID);
		if( inVScrollBar == true )
		{
			NVI_EmbedControl(frameControlID,vScrollbarControlID);
		}
		if( inHScrollBar == true )
		{
			NVI_EmbedControl(frameControlID,hScrollbarControlID);
		}
	}
#endif
	
	//ListView����
	AListViewFactory	listViewFactory(GetObjectID(),inListViewControlID);
	NVM_CreateView(inListViewControlID,false,listViewFactory);
	mListViewControlIDArray.Add(inListViewControlID);
	
	//�t���[���̒���ListView������
	NVI_EmbedControl(frameControlID,inListViewControlID);
	
	//ListView�Ƀt���[���ƃX�N���[���o�[��ControlID��ݒ�
	NVI_GetListView(inListViewControlID).NVI_SetScrollBarControlID(hScrollbarControlID,vScrollbarControlID);
	NVI_GetListView(inListViewControlID).SPI_SetFrameControlID(frameControlID);
}
*/

AView_List&	AWindow::NVI_GetListView( const AControlID inListViewControlID )
{
	//#205 if( mListViewControlIDArray.Find(inListViewControlID) == kControlID_Invalid )   _ACThrow("no control ID",this);
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_List,kViewType_List,inListViewControlID);//#199
	//#199 return dynamic_cast<AView_List&>(NVI_GetViewByControlID(inListViewControlID));
}
const AView_List&	AWindow::NVI_GetListViewConst( const AControlID inListViewControlID ) const
{
	//#205 if( mListViewControlIDArray.Find(inListViewControlID) == kControlID_Invalid )   _ACThrow("no control ID",this);
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_List,kViewType_List,inListViewControlID);
}

/*#205
List�t���[����ListView����̂�ViewID�o�R�ŃA�N�Z�X����
//List�t���[�����擾
//win 080618
AView_ListFrame&	AWindow::NVI_GetListFrameView( const AControlID inListFrameViewControlID )
{
	if( mListFrameViewControlIDArray.Find(inListFrameViewControlID) == kControlID_Invalid )   _ACThrow("no control ID",this);
	return dynamic_cast<AView_ListFrame&>(NVI_GetViewByControlID(inListFrameViewControlID));
}
*/

#pragma mark ===========================

//win 080618

#pragma mark ---EditBoxView

/**
EditBox�����i���\�[�X��ɒ�`����Ă���ꍇ�p�j
*/
AViewID	AWindow::NVI_CreateEditBoxView( const AControlID inEditViewControlID, 
		const ABool inHasVScrollbar, const ABool inHasHScrollBar , const ABool inHasFrame, //#182
		const AEditBoxType inEditBoxType )//#725
{
	AEditBoxViewFactory	editViewFactory(GetObjectID(),inEditViewControlID);//#182
	AViewID	viewID = NVM_CreateView(inEditViewControlID,/*#688 false*/true,editViewFactory);
	NVI_GetEditBoxView(inEditViewControlID).SPI_SetEditBoxType(inEditBoxType);
	NVI_RegisterToFocusGroup(inEditViewControlID);
	if( inHasFrame == true )
	{
		//edit box��filter box�̏ꍇ�́A���]���𑽂߂ɂ���i���ዾ�A�C�R���\���p�j
		ANumber	frameLeftMargin = 4;
		if( inEditBoxType == kEditBoxType_FilterBox )
		{
			frameLeftMargin += 18;
		}
		//frame view�擾
		NVI_GetViewByControlID(inEditViewControlID).NVI_CreateFrameView(frameLeftMargin);
		//edit box��filter box�̏ꍇ�́A�t���[����filter box�^�C�v�ɐݒ�
		if( inEditBoxType == kEditBoxType_FilterBox )
		{
			NVI_GetViewByControlID(inEditViewControlID).NVI_GetFrameView().SPI_SetFrameViewType(kFrameViewType_FilterBox);
		}
		else
		{
			NVI_GetViewByControlID(inEditViewControlID).NVI_GetFrameView().SPI_SetFrameViewType(kFrameViewType_EditBox);
		}
	}
	if( inHasVScrollbar == true )
	{
		NVI_GetViewByControlID(inEditViewControlID).NVI_CreateVScrollBar(15,inHasHScrollBar);
	}
	if( inHasHScrollBar == true )
	{
		NVI_GetViewByControlID(inEditViewControlID).NVI_CreateHScrollBar(15,inHasVScrollbar);
	}
	return viewID;
}

//EditBox�����i���\�[�X��ɒ�`����Ă��Ȃ��ꍇ�p�j
AViewID	AWindow::NVI_CreateEditBoxView( const AControlID inEditViewControlID, 
		const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight, 
		const AControlID inParentControlID,//#688
		const AIndex inTabControlIndex, const ABool inFront,
		const ABool inHasVScrollbar, const ABool inHasHScrollBar , const ABool inHasFrame, //#182
		const AEditBoxType inEditBoxType )//#725
{
	AEditBoxViewFactory	editViewFactory(/* #199 this,*this*/GetObjectID(),inEditViewControlID);
	AViewID	viewID = NVM_CreateView(inEditViewControlID,inPosition,inWidth,inHeight,inParentControlID,kControlID_Invalid,
			true,editViewFactory,inTabControlIndex,inFront);
	NVI_GetEditBoxView(inEditViewControlID).SPI_SetEditBoxType(inEditBoxType);
	NVI_RegisterToFocusGroup(inEditViewControlID);
	if( inHasFrame == true )
	{
		//edit box��filter box�̏ꍇ�́A���]���𑽂߂ɂ���i���ዾ�A�C�R���\���p�j
		ANumber	frameLeftMargin = 4, frameRightMargin = 4;//#1316
		if( inEditBoxType == kEditBoxType_FilterBox )
		{
			frameLeftMargin += 22;//#1316 18;
			frameRightMargin += 8;//#1316
		}
		//frame view�擾
		NVI_GetViewByControlID(inEditViewControlID).NVI_CreateFrameView(frameLeftMargin,frameRightMargin);//#1316
		//�t���[���̕\���^�C�v��ݒ�
		AFrameViewType	frameViewType = kFrameViewType_EditBox;
		switch(inEditBoxType)
		{
		  case kEditBoxType_ToolTip://#1316 kEditBoxType_ThinFrame:
			{
				frameViewType = kFrameViewType_ToolTip;//#1316 kFrameViewType_Normal;
				break;
			}
		  case kEditBoxType_FilterBox:
			{
				frameViewType = kFrameViewType_FilterBox;
				break;
			}
		  case kEditBoxType_NoFrameDraw:
			{
				frameViewType = kFrameViewType_NoFrameDraw;
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		NVI_GetViewByControlID(inEditViewControlID).NVI_GetFrameView().SPI_SetFrameViewType(frameViewType);
	}
	if( inHasVScrollbar == true )
	{
		NVI_GetViewByControlID(inEditViewControlID).NVI_CreateVScrollBar();
	}
	if( inHasHScrollBar == true )
	{
		NVI_GetViewByControlID(inEditViewControlID).NVI_CreateHScrollBar();
	}
	return viewID;
}

//EditBox�폜
void	AWindow::NVI_DeleteEditBoxView( const AControlID inEditViewControlID )
{
	//win NVM_DeleteView(inEditViewControlID);
	NVI_DeleteControl(inEditViewControlID);//win NVI_DeleteView()��mTabDataArray_ControlIDs����̍폜�����s��Ȃ��̂�NG
	/*#199 AIndex	index = mEditBoxViewControlIDArray.Find(inEditViewControlID);
	if( index == kIndex_Invalid )   {_ACError("",this);return;}
	mEditBoxViewControlIDArray.Delete1(index);*/
}

//EditBox�擾
AView_EditBox&	AWindow::NVI_GetEditBoxView( const AControlID inEditViewControlID )
{
	//#199 if( mEditBoxViewControlIDArray.Find(inEditViewControlID) == kControlID_Invalid )   _ACThrow("no control ID",this);
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_EditBox,kViewType_EditBox,inEditViewControlID);
	/*#199
	return dynamic_cast<AView_EditBox&>(NVI_GetViewByControlID(inEditViewControlID));
	*/
}
const AView_EditBox&	AWindow::NVI_GetEditBoxViewConst( const AControlID inEditViewControlID ) const
{
	//#199 if( mEditBoxViewControlIDArray.Find(inEditViewControlID) == kControlID_Invalid )   _ACThrow("no control ID",this);
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_EditBox,kViewType_EditBox,inEditViewControlID);
	/*#199
	return dynamic_cast<const AView_EditBox&>(NVI_GetViewConstByControlID(inEditViewControlID));
	*/
}

#pragma mark ===========================

#pragma mark ---ButtonView
//#232

/**
Button����
*/
AViewID	AWindow::NVI_CreateButtonView( const AControlID inControlID, 
		const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight,
		const AControlID inParentControlID )//#688
{
	AViewDefaultFactory<AView_Button>	buttonFactory(GetObjectID(),inControlID);
	return NVM_CreateView(inControlID,inPosition,inWidth,inHeight,inParentControlID,kControlID_Invalid,false,buttonFactory);
}

//#427
/**
Button����(���\�[�X��ɒ�`����Ă���ꍇ�p)
*/
AViewID	AWindow::NVI_CreateButtonView( const AControlID inControlID )
{
	AViewDefaultFactory<AView_Button>	buttonFactory(GetObjectID(),inControlID);
	return NVM_CreateView(inControlID,false,buttonFactory);
}


/**
Button�擾
*/
AView_Button&	AWindow::NVI_GetButtonViewByControlID( const AControlID inButtonControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_Button,kViewType_Button,inButtonControlID);
}

/**
�Ō�ɃN���b�N����ButtonView��ControlID��ݒ肷��
*/
void	AWindow::NVI_SetLastClickedButtonControlID( const AControlID inLastClickedButtonControlID )
{
	mLastClickedButtonControlID = inLastClickedButtonControlID;
}

/**
�Ō�ɃN���b�N����ButtonView��ControlID���擾����
*/
AControlID	AWindow::NVI_GetLastClickedButtonControlID() const
{
	return mLastClickedButtonControlID;
}

#pragma mark ===========================

#pragma mark ---PlainView
//#634

/**
PlainView����
*/
AViewID	AWindow::NVI_CreatePlainView( const AControlID inControlID, const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight )
{
	AViewDefaultFactory<AView_Plain>	viewFactory(GetObjectID(),inControlID);
	return NVM_CreateView(inControlID,inPosition,inWidth,inHeight,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
}

/**
PlainView�擾
*/
AView_Plain&	AWindow::NVI_GetPlainViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_Plain,kViewType_Plain,inControlID);
}

/**
PlainView�F�ݒ�
*/
void	AWindow::NVI_SetPlainViewColor( const AControlID inControlID, const AColor inColor, const AColor inColorDeactive, 
									   const ABool inLeftBottomRounded, const ABool inRightBottomRounded,
									   const ABool inForLeftSideBarSeparator, const ABool inForRightSideBarSeparator, const AColor inSeparatorLineColor )
{
	NVI_GetPlainViewByControlID(inControlID).SPI_SetColor(inColor,inColorDeactive,inLeftBottomRounded,inRightBottomRounded,inForLeftSideBarSeparator,inForRightSideBarSeparator,inSeparatorLineColor);
}

#pragma mark ===========================

#pragma mark ---Separator
//#291

/**
VSpeparator OverlayWindow
*/
AWindow_VSeparatorOverlay&	AWindow::NVI_GetVSeparatorWindow( const AWindowID inSeparatorOverlayWindow ) 
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_VSeparatorOverlay,kWindowType_VSeparatorOverlay,inSeparatorOverlayWindow);
}

/**
HSpeparator OverlayWindow
*/
AWindow_HSeparatorOverlay&	AWindow::NVI_GetHSeparatorWindow( const AWindowID inSeparatorOverlayWindow ) 
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_HSeparatorOverlay,kWindowType_HSeparatorOverlay,inSeparatorOverlayWindow);
}

/**
VSeparator����
*/
void	AWindow::NVI_CreateVSeparatorView( const AControlID inControlID, const ASeparatorLinePosition inSeparatorLinePosition,
		const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight )
{
	AViewDefaultFactory<AView_VSeparator>	separatorFactory(GetObjectID(),inControlID);
	NVM_CreateView(inControlID,inPosition,inWidth,inHeight,kControlID_Invalid,kControlID_Invalid,false,separatorFactory);
	NVI_GetVSeparatorViewByControlID(inControlID).SPI_SetLinePosition(inSeparatorLinePosition);
}

/**
�Z�p���[�^�ړ������Ƃ��ɃR�[�������
*/
ANumber	AWindow::NVI_SeparatorMoved( const AWindowID inTargetWindowID, const AControlID inID, 
		const ANumber inDelta, const ABool inCompleted )//#409
{
	//�h���N���X�ł̏���
	return AApplication::GetApplication().NVI_GetWindowByID(inTargetWindowID).
			NVIDO_SeparatorMoved(GetObjectID(),inID,inDelta,inCompleted);//#409
}

/**
�Z�p���[�^�_�u���N���b�N���ɃR�[�������
*/
void	AWindow::NVI_SeparatorDoubleClicked( const AWindowID inTargetWindowID, const AControlID inID )
{
	//�h���N���X�ł̏���
	AApplication::GetApplication().NVI_GetWindowByID(inTargetWindowID).NVIDO_SeparatorDoubleClicked(GetObjectID(),inID);
}

/**
VSeparator�擾
*/
AView_VSeparator&	AWindow::NVI_GetVSeparatorViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_VSeparator,kViewType_VSeparator,inControlID);
}

/**
HSeparator����
*/
void	AWindow::NVI_CreateHSeparatorView( const AControlID inControlID, const ASeparatorLinePosition inSeparatorLinePosition,
		const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight )
{
	AViewDefaultFactory<AView_HSeparator>	separatorFactory(GetObjectID(),inControlID);
	NVM_CreateView(inControlID,inPosition,inWidth,inHeight,kControlID_Invalid,kControlID_Invalid,false,separatorFactory);
	NVI_GetHSeparatorViewByControlID(inControlID).SPI_SetLinePosition(inSeparatorLinePosition);
}

/**
HSeparator�擾
*/
AView_HSeparator&	AWindow::NVI_GetHSeparatorViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_HSeparator,kViewType_HSeparator,inControlID);
}

#pragma mark ===========================

#pragma mark ---ButtonOverlayWindow
//#291

/**
AWindow_ButtonOverlay
*/
AWindow_ButtonOverlay&	AWindow::NVI_GetButtonWindow( const AWindowID inButtonOverlayWindow )
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_ButtonOverlay,kWindowType_ButtonOverlay,inButtonOverlayWindow);
}

#pragma mark ===========================

#pragma mark ---modal����
//#846

//���[�_�������ǂ����̃t���O #946
ABool	AWindow::sInModal = false;

/**
���[�_���Z�b�V�����J�n
*/
void	AWindow::NVI_StartModalSession()
{
	GetImp().StartModalSession();
}

/**
���[�_���Z�b�V�����p���`�F�b�N
*/
ABool	AWindow::NVI_CheckContinueingModalSession()
{
	return GetImp().CheckContinueingModalSession();
}

/**
���[�_���Z�b�V�����I��
*/
void	AWindow::NVI_EndModalSession()
{
	GetImp().EndModalSession();
}

/**
���[�_���Z�b�V�������~
*/
void	AWindow::NVI_StopModalSession()
{
	GetImp().StopModalSession();
}

/**
���[�_���E�C���h�E���[�v�J�n�i�E�C���h�E���̃C�x���g������StopModalSession���R�[�������܂ŏI�����Ȃ��j
*/
void	AWindow::NVI_RunModalWindow()
{
	try
	{
		//���[�_�����t���OON #946
		sInModal = true;
		//���[�_�����s�imodal�I������܂Ńu���b�N�j
		GetImp().RunModalWindow();
		//���[�_�����t���OOFF #946
		sInModal = false;
	}
	catch(...)
	{
		_ACError("",this);
	}
}

#pragma mark ===========================

#pragma mark ---static�f�[�^������
//#353

//#688 ABool	AWindow::sIsFocusGroup2ndPattern = false;//#353

/**
static�f�[�^������
*/
void	AWindow::STATIC_Init()
{
	//#688 sIsFocusGroup2ndPattern = CWindowImp::STATIC_CheckIsFocusGroup2ndPattern();
}

