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

AWindow_FindResult
#725

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AView_SubWindowTitlebar.h"

class ADocument_IndexWindow;
class AView_Index;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_FindResult
//�C���f�b�N�X�E�C���h�E�̃N���X

class AWindow_FindResult : public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_FindResult( const ADocumentID inDocumentID );
	~AWindow_FindResult();
	
	//<�֘A�I�u�W�F�N�g�擾>
  public:
	ADocumentID				SPI_GetDocumentID() const { return mDocumentID; }
  private:
	ADocument_IndexWindow&	GetDocument();
	ADocumentID							mDocumentID;
	AView_SubWindowTitlebar&	GetTitlebarViewByControlID( const AControlID inControlID );
	
	//<�C�x���g����>
	
	//�C�x���g�R�[���o�b�N
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	void					EVTDO_DoCloseButton();
	void					EVTDO_WindowResizeCompleted( const ABool inResized );
	//drop void					EVTDO_SaveWindowReply( const AObjectID inDocumentID, const AFileAcc& inFile, const AText& inRefText );
	//drop void					EVTDO_AskSaveChangesReply( const AObjectID inDocumentID, const AAskSaveChanges inAskSaveChangesReply );
	void					NVIDO_TryClose( const AIndex inTabIndex );//#92
	void					NVIDO_ExecuteClose();//#92
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	
	//<�C���^�[�t�F�C�X>
	
	//�E�C���h�E����
  public:
	AViewID					SPI_GetIndexViewID() const { return mIndexViewID; }//#92
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_Hide();
	void					NVIDO_UpdateProperty();
	void					NVMDO_UpdateControlStatus();
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
	ABool					NVIDO_IsDocumentWindow() const { return true; }//#688
	AViewID								mIndexViewID;//#92
	AControlID							mVScrollBarControlID;
	
	void					UpdateViewBounds();
	
	//
  public:
	void					SPI_ShowSaveWindow();
	
	//�E�C���h�E���ݒ�^�擾
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_FindResult; }
	
	ABool								mQuitting;
	ABool								mAllClosing;
	//drop ABool								mAskingSaveChanges;
	
	static const ANumber	kVScrollBarWidth = 16;
	static const ANumber	kDragBoxHeight = 16;
	
	//#725
	/** �E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾 */
  public:
	ANumber					SPI_GetSubWindowMinHeight() const;
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_FindResult"; }
};

#pragma mark ===========================
#pragma mark [�N���X]AFindResultWindowFactory
class AFindResultWindowFactory : public AWindowFactory
{
  public:
	AFindResultWindowFactory( const ADocumentID inDocumentID )
		: mDocumentID(inDocumentID)
	{
	}
	AWindow_FindResult*	Create() 
	{
		return new AWindow_FindResult(mDocumentID);
	}
  private:
	ADocumentID	mDocumentID;
};




