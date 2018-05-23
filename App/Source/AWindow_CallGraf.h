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

AWindow_CallGraf
#723

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AView_SubWindowTitlebar.h"

class AView_CallGraf;
class AView_CallGrafHeader;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_CallGraf

/**
*/
class AWindow_CallGraf : public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_CallGraf();
	~AWindow_CallGraf();
	
	//<�֘A�I�u�W�F�N�g�擾>
  public:
	AView_CallGraf&			SPI_GetCallGrafView();
	AView_CallGrafHeader&	SPI_GetCallGrafHeaderView();
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
	void					NVIDO_Hide();
	void					NVIDO_UpdateProperty();
	
  public:
	void					SPI_ShowHideImportFileProgress( const ABool inShow );
  private:
	ABool								mAllFilesSearched;
	ABool								mShowImportFileProgress;
	
  public:
  private:
	void					UpdateViewBounds();
	
	//���x�����E�ړ��{�^��
	//�i���ۂ̃{�^���͑��݂����Acall graf header��̎w��ʒu���N���b�N������A����control id���ʒm�����j
  public:
	static const AControlID	kControlID_LeftLevelButton = 103;
	static const AControlID	kControlID_RightLevelButton = 104;
	
	//#725
	/** �E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾 */
  public:
	ANumber					SPI_GetSubWindowMinHeight() const;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_CallGraf; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_CallGraf"; }
};

