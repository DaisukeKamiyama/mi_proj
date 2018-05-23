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

AWindow_FTPProgress

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_FTPProgress
//FTP�v���O���X�E�C���h�E
class AWindow_FTPProgress : public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_FTPProgress();
	
	//<�C�x���g����>
  public:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	
	
	//<�C���^�[�t�F�C�X>
	
	//�E�C���h�E����
  public:
	void					SPNVI_Show( const ABool inEnableCancel );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//�v���O���X�\��
  public:
	void					SPI_SetProgress( const ANumber inPercent );
	void					SPI_SetProgressText( const AText& inText );
	void					SPI_SetTitle( const AText& inText );
	void					SPI_SetError( const AText& inText );
	
	//
  private:
	ABool								mEnableCancel;
	ABool								mErrorMode;
	
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_FTPProgress; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_FTPProgress"; }
};

#pragma mark ===========================
#pragma mark [�N���X]AWindow_FTPLog
//FTP���O�E�C���h�E
class AWindow_FTPLog : public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_FTPLog();
	
	//<�C���^�[�t�F�C�X>
	
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//���O�ǉ�
  public:
	void					SPI_AddLog( const AText& inText );
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_FTPLog; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_FTPLog"; }
};



