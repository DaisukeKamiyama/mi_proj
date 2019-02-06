/*

AWindow_UserIDRegistration
(C) 2019 Daisuke Kamiyama, All Rights Reserved.

*/


#pragma once

#include "Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_UserIDRegistration
//���[�h�ǉ��E�C���h�E
class AWindow_UserIDRegistration : public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_UserIDRegistration();
  private:
	
	//<�֘A�I�u�W�F�N�g�擾>
  public:
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();

	//<�C���^�[�t�F�C�X>
	
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVMDO_UpdateControlStatus();
	
	//
  public:
	static ABool			CheckUserID( const AText& inText );
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_UserIDRegistration; }
};



