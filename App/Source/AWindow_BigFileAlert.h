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

AWindow_BigFileAlert
R0208
*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_BigFileAlert
//About�E�C���h�E
class AWindow_BigFileAlert : public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_BigFileAlert();
	
	//<�C�x���g����>
	
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	
	
	//<�C���^�[�t�F�C�X>
	
	//�E�C���h�E����
  public:
	void					SPI_SetFile( const AFileAcc& inFile );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//B0419 AFileAcc				mFile;
	AObjectArray<AFileAcc>				mFileArray;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_BigFileAlert; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_BigFileAlert"; }
};

