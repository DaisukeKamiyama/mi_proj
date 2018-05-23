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
#pragma mark [クラス]AWindow_FTPProgress
//FTPプログレスウインドウ
class AWindow_FTPProgress : public AWindow
{
	//コンストラクタ
  public:
	AWindow_FTPProgress();
	
	//<イベント処理>
  public:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	
	
	//<インターフェイス>
	
	//ウインドウ制御
  public:
	void					SPNVI_Show( const ABool inEnableCancel );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//プログレス表示
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
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_FTPProgress"; }
};

#pragma mark ===========================
#pragma mark [クラス]AWindow_FTPLog
//FTPログウインドウ
class AWindow_FTPLog : public AWindow
{
	//コンストラクタ
  public:
	AWindow_FTPLog();
	
	//<インターフェイス>
	
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//ログ追加
  public:
	void					SPI_AddLog( const AText& inText );
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_FTPLog; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_FTPLog"; }
};



