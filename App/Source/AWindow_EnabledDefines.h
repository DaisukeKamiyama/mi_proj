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

AWindow_EnabledDefines
#467

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_EnabledDefines
//�����E�C���h�E
class AWindow_EnabledDefines : public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_EnabledDefines();
	~AWindow_EnabledDefines();
  private:
	void	NVIDO_DoDeleted();
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	ABool					EVTDO_ValueChanged( const AControlID inControlID );
	void					NVMDO_UpdateControlStatus();
	
	//<�C���^�[�t�F�C�X>
	
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//
  public:
	void					SPI_SetDefineEditBox( const AText& inText );
  private:
	void					UpdateList();
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_EnabledDefines; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_EnabledDefines"; }
};





