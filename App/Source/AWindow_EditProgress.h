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


#846

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [�N���X]
//FTP�v���O���X�E�C���h�E
class  AWindow_EditProgress: public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_EditProgress();
	
	//<�C�x���g����>
  public:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	
	
	//<�C���^�[�t�F�C�X>
	
	//�E�C���h�E����
  public:
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//�v���O���X�\��
  public:
	void					SPI_SetProgress( const ANumber inPercent );
	void					SPI_SetText( const AText& inTitle, const AText& inMessage );
	void					SPI_SetProgressText( const AText& inText );
	void					SPI_SetEnableCancelButton( const ABool inEnable );
	
	//
  private:
	ABool								mEnableCancel;
	ABool								mErrorMode;
	
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_EditProgress; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return ""; }
};

