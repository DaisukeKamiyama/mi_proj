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

AWindow_Find

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "ATextFinder.h"

/**
�u���͈̓^�C�v
*/
enum AReplaceRangeType
{
	kReplaceRangeType_Next,
	kReplaceRangeType_AfterCaret,
	kReplaceRangeType_InSelection,
	kReplaceRangeType_All,
	kReplaceRangeType_ReplaceSelectedTextOnly,
};

#pragma mark ===========================
#pragma mark [�N���X]AWindow_Find
//�����E�C���h�E
class AWindow_Find : public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_Find();
	~AWindow_Find();
  private:
	void	NVIDO_DoDeleted();//#92
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	ADataBase&				NVMDO_GetDB();
	
	//<�C���^�[�t�F�C�X>
	
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_Hide();
	void					NVIDO_Close();
	void					NVMDO_UpdateControlStatus();
	void					NVIDO_UpdateProperty();
	void					NVMDO_UpdateTableView( const AControlID inTableControlID, const ADataID inColumnID );
	void					NVMDO_NotifyDataChanged( const AControlID inControlID, const AModificationType inModificationType, const AIndex inIndex,
							const AControlID inTriggerControlID/*B0406*/ );
	
	
	//�������ݒ�^�擾
  public:
	//#693 void					SPI_SetFindText( const AText& inText );
	//#693 void					SPI_SetReplaceText( const AText& inText );
	void					SPI_UpdateUI_CurrentFindParam();//#693
	//#693 void					SPI_GetFindParam( AFindParameter& outParam );
	//ABool					SPI_CompareFindText( CTextHandler *inTextHandler );
	void					SPI_ShowReplaceResultSheet( const AItemCount inReplacedCount );
  private:
	void					UpdateData_CurrentFindParam() const;//#693
	
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	ABool					EVTDO_ValueChanged( const AControlID inControlID );
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	ABool					Execute( const AMenuItemID inMenuItemID );
	void					EVTDO_WindowActivated();
	void					EVTDO_WindowDeactivated();
	void					EVTDO_TextInputted( const AControlID inID );
	ABool					CheckRegExp( const AFindParameter& inFindParam );//B0317
	void					EVTDO_DoCloseButton();//#701
	ABool					EVTDO_DoubleClicked( const AControlID inID );
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	void					EVTDO_FileChoosen( const AControlID inControlID, const AFileAcc& inFile );
	
	//<�������W���[��>
	
	//�u�ŋߎg����������v
  public:
	void					SPI_AddRecentlyUsedFindText( const AFindParameter& inFindParam );
	void					SPI_AddRecentlyUsedReplaceText( const AFindParameter& inFindParam );
  private:
	void					MakeRecentlyUsedFindTextMenu();//win
	void					MakeRecentlyUsedReplaceTextMenu();//win
	AHashTextArray						mRecentlyUsedFindTextArray;//win
	AHashTextArray						mRecentlyUsedReplaceTextArray;//win
	
	//�����������[
  public:
	void					SPI_SetFromFindMemory( const AIndex inMemoryIndex );
	void					SPI_CopyToFindMemory( const AIndex inMemoryIndex );
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_Find; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_Find"; }
};





