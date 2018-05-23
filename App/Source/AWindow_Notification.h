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

AWindow_Notification
#725

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AView_Notification;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_Notification
/*
�e�탁�b�Z�[�W�\���p�|�b�v�A�b�v
�etext view����AWindow_Notification�|�b�v�A�b�v�E�C���h�E�������A���̃E�C���h�E����AView_Notification������
*/

class AWindow_Notification : public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_Notification();
	~AWindow_Notification();
	
	//<�֘A�I�u�W�F�N�g�擾>
  public:
	AView_Notification&		SPI_GetNotificationView() { return GetListView(); }
  private:
	AView_Notification&		GetListView();
	const AView_Notification&	GetListViewConst() const;
	
	//<�C�x���g����>
	
	//�C�x���g�R�[���o�b�N
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	void					EVTDO_DoCloseButton();
	void					EVTDO_DoTickTimerAction();
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
	void					NVIDO_Hide();
	void					NVIDO_UpdateProperty();
  private:
	void					UpdateViewBounds();
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_Notification; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_Notification"; }
};

