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

AView_Notification
#725

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

/**
Notificationタイプ
*/
enum ANotificationType
{
	kNotificationType_ReplaceResult = 1,
	kNotificationType_FindResult,
	kNotificationType_UnsavedFile,
	kNotificationType_FolderLabel,
	kNotificationType_IndentCharacterChanged,
	kNotificationType_ReloadEditByOtherApp,
	kNotificationType_General,
};

#pragma mark ===========================
#pragma mark [クラス]AView_Notification

/*
各種メッセージ表示用ポップアップ
各text view毎にAWindow_Notificationポップアップウインドウを持ち、そのウインドウ内にAView_Notificationがある
*/
class AView_Notification : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_Notification( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_Notification();
  private:
	void					NVIDO_Init();
	void					NVIDO_DoDeleted();
	
	//
  public:
	void					SPI_DoTickTimer();
	void					SPI_DoWindowHide();
	
	//<イベント処理>
public:
	ANumber					SPI_GetWindowHeight() const;
	AItemCount				SPI_GetItemCount() { return mArray_Text.GetItemCount(); }
  private:
	ABool					EVTDO_DoTextInput( const AText& inText,
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu );
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, 
							const ALocalPoint inLocalPoint );
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint );
	ABool					EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	ABool					EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) ;
	ACursorType				EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					SPI_UpdateWindowHeight();
  private:
	void					CloseBox( const AIndex inIndex );
	void					GetItemBox( const AIndex inIndex, ALocalRect& outRect ) const;
	AIndex					FindItemBox( const ALocalPoint& inLocalPoint ) const;
	
	//<インターフェイス>
  public:
	void					SPI_SetNotification_ReplaceResult( const ABool inNotAborted, const ANumber inCount,
							const AText& inFindText, const AText& inReplaceText );
	void					SPI_SetNotification_FindResult( const ABool inFoundInNext, const ABool inFoundInPrev,
							const AText& inFindText );
	void					SPI_SetNotification_UnsavedFile();
	void					SPI_SetNotification_FolderLabel( const AText& inFolderPath, const AText& inLabelText );
	void					SPI_SetNotification_IndentCharacterChanged( const AModeIndex inModeIndex,
																		const AText& inURI );//#1003
	void					SPI_SetNotification_ReloadEditByOtherApp( const AText& inFileName );
	void					SPI_SetNotification_General( const AText& inTitle, const AText& inText,
														 const AText& inURI );//#1003
  private:
	ATextArray							mArray_Text;
	ATextArray							mArray_Title;
	ANumberArray						mArray_HideCounter;
	AArray<AColor>						mArray_Color;
	AArray<AFloatNumber>				mArray_Alpha;
	AArray<ANotificationType>			mArray_Type;
	ATextArray							mArray_URI;//#1003
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_Notification;}
};

