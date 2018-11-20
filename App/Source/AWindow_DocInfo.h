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

AWindow_DocInfo
#142

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AView_SubWindowTitlebar.h"

class AView_DocInfo;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_DocInfo

/**
DocInfo�E�C���h�E
*/
class AWindow_DocInfo : public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_DocInfo();
	~AWindow_DocInfo();
	
	//<�֘A�I�u�W�F�N�g�擾>
  public:
	AView_DocInfo&			GetDocInfoView();
	AView_SubWindowTitlebar&	GetTitlebarViewByControlID( const AControlID inControlID );//#725
	
	//<�C�x���g����>
	
	//�C�x���g�R�[���o�b�N
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	void					EVTDO_DoCloseButton();
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
	void					NVIDO_Show();//#1312
	void					NVIDO_Hide();
	void					NVIDO_UpdateProperty();
	
	//
  public:
  private:
	void					UpdateViewBounds();
	
	//DocInfo�X�V
  public:
	void					SPI_UpdateDocInfo_TotalCount( 
							const AItemCount inTotalCharCount, const AItemCount inTotalWordCount,
							const AItemCount inTotalParagraphCount );
	void					SPI_UpdateDocInfo_SelectedCount( 
							const AItemCount inSelectedCharCount, const AItemCount inSelectedWordCount,
															const AItemCount inSelectedParagraphCount,
															const AText& inSelectedText );
	void					SPI_UpdateDocInfo_CurrentChar( const AUCS4Char inChar, const AUCS4Char inCanonicalDecomp, 
							const ATextArray& inHeaderTextArray, const AArray<AIndex>& inHeaderParagraphIndexArray, const AText& inDebugText );
	void					SPI_UpdateDocInfo_PluginText( const AText& inPluginText );
	
	//#725
	/** �E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾 */
  public:
	ANumber					SPI_GetSubWindowMinHeight() const;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_DocInfo; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_DocInfo"; }
};

