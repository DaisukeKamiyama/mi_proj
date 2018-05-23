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

AWindow_SaveCancelDiscard

*/

#pragma once

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_SaveCancelDiscard
/**
SaveCancelDiscard�E�C���h�E
*/
class AWindow_SaveCancelDiscard : public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_SaveCancelDiscard();
	~AWindow_SaveCancelDiscard();
  private:
	ABool								mModal;
	
	//<�֘A�I�u�W�F�N�g>
  private:
	AWindowID							mParentWindowID;
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	ADocumentID							mAskSaveChangesDocumentID;
	void					EVTDO_DoCloseButton();
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  public:
	void					SPNVI_Show( const ADocumentID inAskSaveChangesDocumentID, const AWindowID inParentWindowID, 
										const AText& inMessage1, const AText& inMessage2, const ABool inModal );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_Close();
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_SaveCancelDiscard; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_SaveCancelDiscard"; }
};

