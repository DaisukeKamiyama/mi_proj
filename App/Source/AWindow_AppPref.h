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

AWindow_AppPref

*/

#pragma once

#include "../../AbsFramework/Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_AppPref
//���ݒ�E�C���h�E
class AWindow_AppPref: public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_AppPref();
	virtual ~AWindow_AppPref();
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	ADataBase&				NVMDO_GetDB();
	
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	ABool					EVTDO_ValueChanged( const AControlID inID );
	ABool					EVTDO_DoubleClicked( const AControlID inID );
	void					EVTDO_FileChoosen( const AControlID inControlID, const AFileAcc& inFile );
	void					EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder );
	
	
	//<�C���^�[�t�F�C�X>
	
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	//#725 void					UpdatePaneLayoutMenus();//#291
	
	//<�ĉ����\�b�h��������>
	
  private:
	void					NVMDO_UpdateControlStatus();
	void					NVIDO_UpdateProperty();
	void					NVMDO_UpdateTableView( const AControlID inTableControlID, const ADataID inColumnID );
	void					NVMDO_OpenSpecializedContentView( const AControlID inTableControlID, const AIndex inRowIndex );//B0406
	void					NVMDO_SaveSpecializedContentView( const AControlID inTableControlID, const AIndex inRowIndex );//B0406
	void					NVMDO_NotifyDataChanged( const AControlID inControlID, const AModificationType inModificationType, const AIndex inIndex,
							const AControlID inTriggerControlID/*B0406*/ );
	void					NVIDO_ListViewFileDropped( const AControlID inControlID, const AIndex inRowIndex, const AFileAcc& inFile );
	void					NVMDO_NotifyDataChangedForSave();//#1239
	
	//#1373
  public:
	static const AControlID	kFontControl_NormalFont				 = 7011;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_AppPref; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_AppPref"; }
};

