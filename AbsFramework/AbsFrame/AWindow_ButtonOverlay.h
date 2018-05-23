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

AWindow_ButtonOverlay

*/

#pragma once

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_ButtonOverlay
/**
HSperator�\���I�[�o�[���C�E�C���h�E
*/
class AWindow_ButtonOverlay : public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_ButtonOverlay();
	~AWindow_ButtonOverlay();
	
	//<�C�x���g����>
	
	//�C�x���g�R�[���o�b�N
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
	
	//Click�ʒm��E�C���h�E�ݒ�
  public:
	void					SPI_SetTargetWindowID( const AWindowID inTargetWindowID, const AControlID inVirtualControlID );
private:
	AWindowID							mClickEventTargetWindowID;
	AControlID							mClickEventVirtualControlID;
	
	//Icon�ݒ�
  public:
	void					SPI_SetIcon( const AImageID inIconID, const ANumber inLeftOffset = 0, const ANumber inTopOffset = 0,
							const ANumber inWidth = 16, const ANumber inHeight = 16, 
							const AImageID inHoverIconImageID = kImageID_Invalid,
							const AImageID inToggleOnImageID = kImageID_Invalid );
	
	//�{�^���^�C�v�ݒ�
  public:
	void					SPI_SetButtonViewType( const AButtonViewType inType );
	
	//HelpTag�ݒ�#661
  public:
	void					SPI_SetHelpTag( const AText& inText1, const AText& inText2, const AHelpTagSide inTagSide = kHelpTagSide_Default );
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_ButtonOverlay; }
};

