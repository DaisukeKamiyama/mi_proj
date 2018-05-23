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

AModePref

*/

#pragma once

#include "../../AbsFramework/Frame.h"
//#844 #include "AWindow_AddNewMode.h"
#include "AWindow_AddToolButton.h"
#include "AWindow_ChangeToolButtonIcon.h"
#include "AWindow_ModeExecutableAlert.h"//R0000

#pragma mark ===========================
#pragma mark [クラス]AKeywordEditManager
//キーワード入力エリア管理 B0406
class AWindow_ModePref;
class AKeywordEditManager
{
  public:
	AKeywordEditManager( AWindow_ModePref& inModePrefWindow );
	~AKeywordEditManager();
	
	void					Open( const AIndex inModeIndex, const AIndex inCategoryIndex);//#427 const AObjectID inCategoryObjectID );//#216 const AIndex inCategoryIndex );
	ABool					TryClose( const ABool inForceSave = false );
	void					SetDirty( const ABool inDirty );
	void					Save();
	void					SaveUseCSV();
	void					Sort();
	void					Revert();//#8
	
	ABool					IsDirty() const { return mDirty; }
	
  private:
	ABool								mOpened;
	ABool								mDirty;
	AIndex								mCurrentModeIndex;
	//#427 AObjectID							mCurrentCategoryObjectID;
	AIndex								mCurrentCategoryIndex;//#427
	AWindow_ModePref&					mModePrefWindow;
};

#pragma mark ===========================
#pragma mark [クラス]AToolEditManager
class AToolEditManager
{
  public:
	AToolEditManager( AWindow_ModePref& inModePrefWindow );
	~AToolEditManager();
	
	void					Open( const AModeIndex inModeIndex, //#868 const ABool inToolbar, //#427 const AFileAcc& inFile )
							const AObjectID inToolMenuObjectID, const AIndex inToolIndex );//#427
	void					SetDirty( const ABool inDirty );
	ABool					TryClose( const ABool inForceSave = false );
	void					SaveToolName();
	void					SaveShortcut();
	void					SaveText();
	void					RevertText();//#8
	void					SetAutoUpdate( const AArray<AIndex>& inSelectedDBIndexArray, const ABool inAutoUpdate );//#427
	void					OpenToolWithApp();//#868
	
	ABool					IsDirty() const { return mDirty; }
	
  private:
	void					Open_ToolText();
	ABool								mOpened;
	ABool								mDirty;
	AModeIndex							mCurrentModeIndex;
	ABool								mCurrentIsToobar;
	AFileAcc							mCurrentFile;
	AWindow_ModePref&					mModePrefWindow;
	AObjectID							mCurrentToolMenuObjectID;//#427
	AIndex								mCurrentToolIndex;//#427
};

#pragma mark ===========================
#pragma mark [クラス]AWindow_ModePref
//モード設定ウインドウ
class AWindow_ModePref : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_ModePref();
	~AWindow_ModePref();
  private:
	void					NVIDO_DoDeleted();//#92
	
	//<関連オブジェクト取得>
  private:
	ADataBase&				NVMDO_GetDB();
	
	
	//<イベント処理>
  public:
	void					SPI_SelectMode( const AModeIndex inModeIndex );
	void					ShowSaveKeywordAlertSheet();
	void					ShowSaveToolAlertSheet();
  private:
	ABool					SwitchMode( const AModeIndex inModeIndex );
	void					DoToolListRowSelected();
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_TextInputted( const AControlID inID );
	void					EVTDO_FileChoosen( const AControlID inControlID, const AFileAcc& inFile );;
	ABool					EVTDO_ValueChanged( const AControlID inID );
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	void					EVTDO_SaveWindowReply( const AObjectID inDocumentID, const AFileAcc& inFile, const AText& inRefText );
	void					EVTDO_DoCloseButton();
	ABool					EVTDO_DoubleClicked( const AControlID inID );
	void					EVTDO_WindowDeactivated();
	ABool					TryCloseCurrentMode();
	void					SetToolTextDirty( const ABool inDirty );
	void					MakeOldDataCopyInTrash( const AConstCharPtr inFileName, const AText& inExportText );//R0000
	void					EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder );//#427
	
	//<インターフェイス>
	
	//ウインドウ制御
  public:
	//B0303 void					SPNVI_Show( const AModeIndex inModeIndex );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVMDO_UpdateControlStatus();
	void					NVMDO_UpdateTableView( const AControlID inTableControlID, const ADataID inColumnID );
	//B0406 void					NVMDO_UpdateTableContentView( const AControlID inTableControlID, const AControlID inContentControlID );
	void					NVMDO_OpenSpecializedContentView( const AControlID inTableControlID, const AIndex inRowIndex );//B0406
	void					NVIDO_UpdateProperty();
	void					NVIDO_Hide();
	
	//コントロール状態制御 #427
  public:
	void					SPI_UpdateToolContentViewControlGrayout( 
							const ABool inEnableToolName, const ABool inEnableToolContent,
							const ABool inEnableShortcut, const ABool inEnableGrayout, const ABool inEnableDisplay, 
																	const ABool inEnableCopyToUserTool );
	void					SPI_ClearToolContentViewControl();
	void					SPI_UpdateKeywordContentViewControlGrayout();
	void					SPI_UpdateJumpSetupContentViewControlGrayout();
  private:
	void					UpdateValidSyntaxPartMenu();
	
	//自動更新 #427
  public:
	ABool					SPI_StartAutoUpdate();
	void					SPI_EndAutoUpdate();
	void					SPI_ShowAutoUpdateError();
	
	//<汎化メソッド特化部分>
	
	//「テーブル内容View」の現在値をテーブルDBへ設定
  private:
	void					NVMDO_SaveSpecializedContentView( const AControlID inTableControlID, const AIndex inRowIndex );//B0406
	
	//DB連動データが変更されたことを派生クラスへ通知するためのメソッド
  private:
	void					NVMDO_NotifyDataChanged( const AControlID inControlID, const AModificationType inModificationType, const AIndex inIndex,
							const AControlID inTriggerControlID/*B0406*/ );
	void					UpdateColors();
	void					NVMDO_NotifyDataChangedForSave();//#1239
	
	void					NVIDO_ListViewRowMoved( const AControlID inControlID, const AIndex inOldIndex, const AIndex inNewIndex );
	void					NVIDO_ListViewFileDropped( const AControlID inControlID, const AIndex inRowIndex, const AFileAcc& inFile );
	
	//未分類
	
  public:
	void	SPI_ShowAddNewModeImportWindow( const AFileAcc& inFile );
	ABool	AttemptQuit();
	//#199 AWindow_AddToolButton&	SPI_GetAddToolButtonWindow() { return mAddToolButtonWindow; }
	
	/*#625 NVMDO_NotifyDataChanged()と役割重複のため、こちらは削除
  private:
	void					NVIDO_AddTableRow( const AControlID inControlID );
	void					NVIDO_RemoveTableRow( const AControlID inControlID, const AIndex inRowIndex );
	void					NVIDO_MoveTableRow( const AControlID inControlID, const AIndex inDeleteItemIndex, const AIndex inNewItemIndex );//R0183
	*/
	//B0406 ABool					SwitchToolContent();//B0303
	//B0406 void					ApplyToolName();//B0303
	//B0406 void					ApplyToolShortcut();//B0303
	void					SelectToolListRow( const AIndex inIndex );//B0303
	void					UpdateToolContent();//B0321
	
	void	TestRecursive( AFileAcc& inOld, AFileAcc& inNew, AText& outText );
	void	Test();
	
	AIndex	mCurrentKeywordCategoryIndex;
	
	AObjectID	mCurrentToolMenuObjectID;
	AArray<AObjectID>	mToolMenuObjectIDArray;
	ATextArray			mToolMenuPathArray;
	ATextArray			mToolMenuDisplayPathArray;
	
	//#868
	//モードindex設定
  public:
	void					SPI_SetModeIndex( const AIndex inModeIndex )
	{ mModeIndex = inModeIndex; }
  private:
	AModeID	mModeIndex;
	
	//#868 ABool	mToolbarMode;
	//B0406 ABool	mToolTextDirty;
	//B0406 AIndex	mCurrentToolSelectIndex;
	
	//B0406 ABool	mKeywordTextDirty;
	
	ATextArray	mKeyBindKeyText;
	ATextArray	mKeyBindActionMenuText;
	AArray<AKeyBindAction>	mKeybindMenuOrder;//B0000
	AModifierKeys	mCurrentKeyBindContentViewModifiers;//#688 別のkeyへの値save禁止
	
	/*#844
	//AddToolButtonWindow #199
 public:
	AWindow_AddToolButton&	SPI_GetAddToolButtonWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_AddToolButton,kWindowType_AddToolButton,mAddToolButtonWindowID);
	const AWindow_AddToolButton&	SPI_GetAddToolButtonWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_AddToolButton,kWindowType_AddToolButton,mAddToolButtonWindowID);
  private:
	AWindowID							mAddToolButtonWindowID;
	
	//AddNewModeWindow #199
  public:
	AWindow_AddNewMode&		SPI_GetAddNewModeWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_AddNewMode,kWindowType_AddNewMode,mAddNewModeWindowID);
	const AWindow_AddNewMode&	SPI_GetAddNewModeWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_AddNewMode,kWindowType_AddNewMode,mAddNewModeWindowID);
  private:
	AWindowID							mAddNewModeWindowID;
	
	//ChangeToolButtonIconWindow #199
  public:
	AWindow_ChangeToolButtonIcon&		SPI_GetChangeToolButtonIconWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_ChangeToolButtonIcon,kWindowType_ChangeToolButtonIcon,mChangeToolButtonIconWindowID);
	const AWindow_ChangeToolButtonIcon&	SPI_GetChangeToolButtonIconWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_ChangeToolButtonIcon,kWindowType_ChangeToolButtonIcon,mChangeToolButtonIconWindowID);
  private:
	AWindowID							mChangeToolButtonIconWindowID;
	*/
	
	
	//B0406
	AKeywordEditManager	mKeywordEditManager;
	AToolEditManager	mToolEditManager;
	
	//多言語処理
	//#305
  private:
	void					SaveMultiLanguageToolName();
	void					DisplayMultiLanguageToolName();
	void					SaveMultiLanguageModeName();
	void					DisplayMultiLanguageModeName();
	ALanguage							mCurrentToolNameLanguage;
	ALanguage							mCurrentModeNameLanguage;
  private:
	ALanguage				GetLaunguageFromMenuItemIndex( const AIndex inIndex ) const;
	AIndex					GetMenuItemIndexFromLanguage( const ALanguage inLanguage ) const;
	
	//フォント設定コントロール
  public:
	static const AControlID	kFontControl_DefaultFont					= 7101;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_ModePref; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_ModePref"; }
};

