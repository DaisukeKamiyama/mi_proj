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

AWindow_MultiFileFind

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "ATextFinder.h"

class AFindParameter;

#pragma mark ===========================
#pragma mark [クラス]AWindow_MultiFileFind
//マルチファイル検索ウインドウ
class AWindow_MultiFileFind : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_MultiFileFind();
	~AWindow_MultiFileFind();
  private:
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();
	ABool					EVTDO_ValueChanged( const AControlID inControlID );
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_TextInputted( const AControlID inID );
	void					EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder );
	void					EVTDO_WindowActivated();
	void					EVTDO_WindowDeactivated();
	ABool					CheckRegExp( const AFindParameter& inFindParam );//B0317
	void					EVTDO_DoFileDropped( const AControlID inControlID, const AFileAcc& inFile );//R0000
	ABool					EVTDO_DoubleClicked( const AControlID inID );
	
	//<インターフェイス>
	
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVMCB_Update();
	void					NVMDO_UpdateControlStatus();
	void					Execute( const ABool inExtractMatchedText );
	void					NVIDO_UpdateProperty();
	void					NVIDO_Show();
	void					NVIDO_Hide();
	void					UpdateFolderRadioButtons();
	void					NVIDO_TryClose( const AIndex inTabIndex );
	
	//外部からのパラメータ設定
  public:
	void					SPI_SetFindText( const AText& inFindText );
	
	//<内部モジュール>
  public:
	void					SPI_AddRecentlyUsedFindText( const AText& inText );
	void					SPI_AddRecentlyUsedFileFilter( const AText& inText );
	void					SPI_AddRecentlyUsedPath( const AFileAcc& inFolder );
  private:
	void					UpdateRecentlyUsedFindTextMenu();
	void					UpdateRecentlyUsedFileFilterMenu();
	void					UpdateRecentlyUsedPathMenu();
	
  private:
	AFileAcc							mActiveFolder;
	
  private:
	ABool					GetFindParam( AFindParameter& findParam ) const;
	
	//マルチファイル置換
  public:
	void					SPI_OpenCloseReplaceScreen( const ABool inReplaceMode );
	void					SPI_MultiFileReplace_FindResult( const ATextArray& inFilePathArray, const AText& inTextEncoding );
	void					SPI_MultiFileReplace_FindCompleted();
	ABool					SPI_ExecutingFindForMultiFileReplace() const { return mExecutingFindForReplace; }
	void					SPI_MultiFileReplace_Progress( const short inPercent );
	void					SPI_MultiFileReplace_SetProgressText( const AText& inText );
	ABool					SPI_ExecutingMultiFileReplace() const { return mExecutingReplace; }
  private:
	void					MultiFileReplace_ClearReplaceData();
	void					MultiFileReplace_UpdateTargetFileTables();
	void					MultiFileReplace_ExcludeButton();
	void					MultiFileReplace_IncludeButton();
	void					MultiFileReplace_ExecuteReplace();
	void					MultiFileReplace_Backup( const AFileAcc& inBaseFolder, const AText& inFindText, const AText& inReplaceText );
	ABool								mReplaceWindowMode;
	ABool								mExecutingFindForReplace;
	ABool								mExecutingReplace;
	ABool								mFindForReplaceFinished;
	ADocumentID							mFindResultDocumentID;
	AHashTextArray						mReplaceTargetFilePathArray;
	ATextArray							mReplaceTargetFilePathArray_TextEncoding;
	AHashTextArray						mReplaceExclusionFilePathArray;
	ATextArray							mReplaceExclusionFilePathArray_TextEncoding;
	AFileAcc							mReplaceBackupRootFolder;
	ABool								mFindForReplaceAborted;
	ABool								mOpenClosingReplaceScreen;
	AFindParameter						mFindParamerForReplace;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_MultiFileFind; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_MultiFileFind"; }
};





