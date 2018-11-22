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

AWindow_ChangeToolButton

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AWindow_Text;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_ChangeToolButton
//���ݒ�E�C���h�E
class AWindow_ChangeToolButton: public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_ChangeToolButton(const AWindowID inParentWindowID);
	virtual ~AWindow_ChangeToolButton();
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	AWindow&				GetParentWindow();
	AWindowID							mParentWindowID;
	
	//�f�[�^
  private:
	AModeIndex							mModeIndex;
	AFileAcc							mFile;
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_TextInputted( const AControlID inID );
	void					EVTDO_DoCloseButton();//win
	
	//<�C���^�[�t�F�C�X>
	
	//�E�C���h�E����
  public:
	void					SPI_SetMode( const AModeIndex inModeIndex, const AFileAcc& inFile );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//<�ĉ����\�b�h��������>
	
  private:
	void					NVMDO_UpdateControlStatus();
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_ChangeToolButton; }
};

