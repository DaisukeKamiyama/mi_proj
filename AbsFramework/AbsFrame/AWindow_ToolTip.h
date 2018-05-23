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

AWindow_ToolTip

*/

#pragma once

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_ToolTip
/**
HSperator�\���I�[�o�[���C�E�C���h�E
*/
class AWindow_ToolTip : public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_ToolTip();
	~AWindow_ToolTip();
	
	//<�C�x���g����>
	
	//�C�x���g�R�[���o�b�N
  private:
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
  private:
	void					UpdateViewBounds();
	
	//�e�L�X�g�ݒ�
  public:
	void					SPI_SetToolTipText( const AText& inToolTipText );
  private:
	void					AdjustWindowSize();
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_ToolTip; }
};

