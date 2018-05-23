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

AWindow_FileList

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AView_SubWindowTitlebar.h"

/**
ファイルリスト更新タイプ
*/
enum AFileListUpdateType
{
	kFileListUpdateType_RecentlyOpenedFileUpdated,
	kFileListUpdateType_ProjectFolderLoaded,
	kFileListUpdateType_SameFolderDataUpdated,
	kFileListUpdateType_SameProjectDataUpdated,
	kFileListUpdateType_SCMDataUpdated,
	kFileListUpdateType_DocumentActivated,
	kFileListUpdateType_DocumentOpened,
	kFileListUpdateType_DocumentClosed,
	kFileListUpdateType_RemoteFileAccountDataChanged,
};

/**
同じプロジェクトのフィルタ
*/
enum AFileListProjectFilter
{
	kFileListProjectFilter_SameModeFiles = 0,
	kFileListProjectFilter_AllFiles,
	kFileListProjectFilter_DiffFiles,
};

/**
ファイルリストモード
*/
const ANumber				kFileListMode_Min						= 0;
const ANumber				kFileListMode_RecentlyOpened			= 0;
const ANumber				kFileListMode_SameFolder				= 1;
const ANumber				kFileListMode_SameProject				= 2;
const ANumber				kFileListMode_RemoteFile				= 3;
const ANumber				kFileListMode_Max						= 3;

#pragma mark ===========================
#pragma mark [クラス]AWindow_FileList
//ファイルリストウインドウ

class AWindow_FileList : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_FileList();
	~AWindow_FileList();
	
	//#725
  private:
	AView_SubWindowTitlebar&	GetTitlebarViewByControlID( const AControlID inControlID );//#725
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	void					EVTDO_DoCloseButton();//#291
	void					EVTDO_TextInputted( const AControlID inID );//#798
	ABool					NVIDO_ViewReturnKeyPressed( const AControlID inControlID );
	ABool					NVIDO_ViewEscapeKeyPressed( const AControlID inControlID );//#798
	ABool					NVIDO_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers );
	ABool					NVIDO_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers );
	void					NVIDO_ListViewExpandedCollapsed( const AControlID inControlID );//#892
	
	//<インターフェイス>
	
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
	void					NVIDO_Show();//#798
	void					NVIDO_Hide();
	void					NVMDO_UpdateControlStatus();
	void					NVIDO_UpdateProperty();
	
	//表示更新
  public:
	void					SPI_UpdateFileList( const AFileListUpdateType inType, const ADocumentID inDocumentID );//#725
	void					SPI_UpdateTable();
	void					SPI_Update_DocumentOpened( const ADocumentID inTextDocumentID );
	void					SPI_Update_DocumentActivated( const ADocumentID inTextDocumentID );
	void					SPI_Update_RecentlyOpendFileUpdated();
	//#725 void					SPI_UpdateSelection_Window( const AIndex& inSelection );
	void					SPI_ChangeMode( const ANumber& inMode );
	//B0000 080810 void					SPI_ClearCursorRow();
	AIndex					SPI_GetContextMenuSelectedRowDBIndex() const;//R0186
	//#725 void					SPI_UpdateOverlayWindowAlpha();//#291
	//#725 void					SPI_SetSideBarMode( const ABool inSideBarMode, const AColor inSideBarColor );//#634
	//
	void					SPI_SwitchFocusToSearchBox();//#798
	void					SPI_GetCurrentPath( AText& outText ) const;//#361
	void					SPI_SetCurrentPath( const AText& inText );//#361
  private:
	void					SaveCollapseData();//#892
	AView_List&				GetListView();
	const AView_List&		GetListViewConst() const;//R0186
	void					UpdateViewBounds();
	void					UpdateTitleText();
	
	//リモート接続 #361
  public:
	void					SPI_SetRemoteConnectionResult( const ATextArray& inFileNameArray, const ABoolArray& inIsDirectoryArray );
	void					SPI_UpdateRemoteFolder( const AText& inFolderURL );
	
	//プログレス表示 #1236
  private:
	void 					ShowRemoteAccessProgress( const ABool inShow );
	
	//データ
  public:
	ANumber					SPI_GetCurrentMode() const { return mMode; }
  private:
	ANumber								mMode;
	AIndex								mCurrentSameFolderIndex;
	AIndex								mCurrentSameProjectIndex;
	AControlID							mListViewControlID;
	ABool								mRecentSortMode;
	AIndex								mSelectionIndex_Recent;
	AIndex								mSelectionIndex_RemoteConnection;
	AIndex								mSelectionIndex_SameFolder;
	AIndex								mSelectionIndex_SameProject;
	AFileListProjectFilter				mProjectFilterMode;//#873
	
	//#533
  private:
	AModeIndex							mCurrentModeIndex;
	ATextArray							mActionTextArray;//#892
	
	//#725 static const ANumber						kModeChangeButtonHeight = 21;
	//#725 static const ANumber						kDragBoxHeight = 8;
	//#725 static const ANumber						kSortButtonWidth = 70;
	
	//リモート接続 #361
  private:
	//リモート接続オブジェクト
	AObjectID							mRemoteConnectionObjectID;
	//現在データ
	//#1231 AText								mRemoteConnectionConnectionType;//#193
	AIndex								mRemoteConnectionCurrentLevel;
	AText								mRemoteConnectionCurrentFolderURL;
	AText								mRemoteConnectionRootFolderURL;
	//要求時データ
	//#1231 AText								mRemoteConnectionConnectionType_Request;//#193
	AIndex								mRemoteConnectionCurrentLevel_Request;
	AText								mRemoteConnectionCurrentFolderURL_Request;
	AText								mRemoteConnectionRootFolderURL_Request;
	//ディレクトリデータ
	ATextArray							mRemoteConnectionFilenameArray;
	ABoolArray							mRemoteConnectionIsDiractoryArray;
	
	
	//#725
	/** ウインドウの通常時（collapseしていない時）の最小高さ取得 */
  public:
	ANumber					SPI_GetSubWindowMinHeight() const;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_FileList; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_FileList"; }
};




