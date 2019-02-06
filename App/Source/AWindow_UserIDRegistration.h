/*

AWindow_UserIDRegistration
(C) 2019 Daisuke Kamiyama, All Rights Reserved.

*/


#pragma once

#include "Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_UserIDRegistration
//モード追加ウインドウ
class AWindow_UserIDRegistration : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_UserIDRegistration();
  private:
	
	//<関連オブジェクト取得>
  public:
	
	//<イベント処理>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();

	//<インターフェイス>
	
	//ウインドウ制御
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



