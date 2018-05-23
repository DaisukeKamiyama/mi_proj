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

AWindow_ProjectFolderAlert

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_ProjectFolderAlert
//

class AWindow_ProjectFolderAlert : public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_ProjectFolderAlert();
	~AWindow_ProjectFolderAlert();
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	
	//<�C���^�[�t�F�C�X>
	
	//�E�C���h�E����
  public:
	void					SPI_SetFolder( const AFileAcc& inDefaultFolder );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	AFileAcc							mDefaultFolder;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_ProjectFolderAlert; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_ProjectFolderAlert"; }
};




