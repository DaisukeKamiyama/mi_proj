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

#include "stdafx.h"

#include "AWindow_FileList.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowHeader.h"
#include "AView_SubWindowSizeBox.h"

const AControlID		kButton_ModeChange				= 102;
const AControlID		kButton_Sort					= 103;

//VScroll bar
const AControlID		kControlID_VScrollBar			= 104;
const ANumber			kVScrollBarWidth = 11;

//#798
//フィルタ
const AControlID		kControlID_Filter				= 201;
const ANumber			kHeight_FilterTopBottomMargin	= 8;

const ADataID			kColumn_FileName				= 0;

//#725
//SubWindow用タイトルバーview
const AControlID	kControlID_TitleBar = 901;

//#725
//SubWindow用セパレータview
const AControlID	kControlID_Separator = 902;

//#725
//サイズボックス
const AControlID	kControlID_SizeBox = 903;
const ANumber	kWidth_SizeBox = 15;
const ANumber	kHeight_SizeBox = 15;

//#725
//背景色描画用view
const AControlID	kControlID_Background = 905;

//ヘッダview
const AControlID	kControlID_Header = 906;
const ANumber	kHeight_Header1Line = 18;
const ANumber	kHeight_Header2Lines = 36;

//ヘッダ上のテキスト
const AControlID	kControlID_FolderPath = 701;
const AControlID	kControlID_ProjectModeFilter = 702;
const ANumber		kHeight_HeaderButton = 15;
const ANumber		kLeftMargin_HeaderButton = 5;
const ANumber		kRightMargin_HeaderButton = 5;

//リモートファイルプログレス表示 #1236
const AControlID	kControlID_RemoteAccessProgressIndicator = 750;
const ANumber	kHeight_ProgressIndicator = 16;
const ANumber	kWidth_ProgressIndicator = 16;

#pragma mark ===========================
#pragma mark [クラス]AWindow_FileList
#pragma mark ===========================
//ファイルリストウインドウ

#pragma mark --- コンストラクタ／デストラクタ
//コンストラクタ
AWindow_FileList::AWindow_FileList():AWindow(/*#175NULL*/), mCurrentSameFolderIndex(kIndex_Invalid), mCurrentSameProjectIndex(kIndex_Invalid),
		mListViewControlID(kControlID_Invalid), mRecentSortMode(false), 
		mSelectionIndex_Recent(kIndex_Invalid), mSelectionIndex_RemoteConnection(kIndex_Invalid), 
		mSelectionIndex_SameFolder(kIndex_Invalid), mSelectionIndex_SameProject(kIndex_Invalid)
		,mCurrentModeIndex(kIndex_Invalid)//#533
		,mRemoteConnectionCurrentLevel(0),mRemoteConnectionObjectID(kObjectID_Invalid)//#361
,mProjectFilterMode(kFileListProjectFilter_SameModeFiles)//#873
,mRemoteConnectionCurrentLevel_Request(0)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kFileListPosition);
	//#138 デフォルト範囲(10000～)を使う限り設定不要 NVM_SetDynamicControlIDRange(kDynamicControlIDStart,kDynamicControlIDEnd);
	mMode = kFileListMode_RecentlyOpened;
	
	//#361
	//アクセスプラグイン接続オブジェクト生成
	mRemoteConnectionObjectID = GetApp().SPI_CreateRemoteConnection(kRemoteConnectionType_FileListWindow,GetObjectID());
	
	//#725 タイマー対象にする
	NVI_AddToTimerActionWindowArray();
}

//デストラクタ
AWindow_FileList::~AWindow_FileList()
{
	//#361
	//リモート接続オブジェクト削除
	GetApp().SPI_DeleteRemoteConnection(mRemoteConnectionObjectID);
	mRemoteConnectionObjectID = kObjectID_Invalid;
}

/**
ウインドウの通常時（collapseしていない時）の最小高さ取得
*/
ANumber	AWindow_FileList::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 20 + kHeight_FilterTopBottomMargin + kHeight_Header2Lines + 10;
}

//#725
/**
SubWindow用タイトルバー取得
*/
AView_SubWindowTitlebar&	AWindow_FileList::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
メニュー選択時処理
*/
ABool	AWindow_FileList::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
		//クローズ
	  case kMenuItemID_Close:
		{
			//フローティングウインドウの場合のみウインドウクローズする
			if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
			{
				GetApp().SPI_CloseSubWindow(GetObjectID());
			}
			result = true;
			break;
		}
		//同じプロジェクト時のフィルタ：全て
	  case kMenuItemID_FileList_ProjectModeFileter_AllFiles:
		{
			mProjectFilterMode = kFileListProjectFilter_AllFiles;
			SPI_UpdateTable();
			NVI_UpdateProperty();
			result = true;
			break;
		}
		//同じプロジェクト時のフィルタ：同じモード
	  case kMenuItemID_FileList_ProjectModeFileter_SameMode:
		{
			mProjectFilterMode = kFileListProjectFilter_SameModeFiles;
			SPI_UpdateTable();
			NVI_UpdateProperty();
			result = true;
			break;
		}
		//同じプロジェクト時のフィルタ：差分のあるファイル
	  case kMenuItemID_FileList_ProjectModeFileter_DiffFiles:
		{
			mProjectFilterMode = kFileListProjectFilter_DiffFiles;
			SPI_UpdateTable();
			NVI_UpdateProperty();
			result = true;
			break;
		}
		//ピン設定
	  case kMenuItemID_FixRecentlyOpenedItem:
		{
			//#394 ピン未実装
			result = true;
			break;
		}
		//最近開いたファイルの項目を削除
	  case kMenuItemID_CM_ClearItemFromRecentOpen:
		{
			GetApp().SPI_DeleteRecentlyOpenedFileItem(SPI_GetContextMenuSelectedRowDBIndex());
			break;
		}
		//見つからない項目を履歴から削除 #1301
	  case kMenuItemID_CM_ClearNotFoundItemsFromRecentOpen:
		{
			GetApp().SPI_DeleteRecentlyOpenedFileNotFound();
			break;
		}
		//その他のmenu item IDの場合、このクラスで処理せず、次のコマンド対象で処理する
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

/**
メニュー更新
*/
void	AWindow_FileList::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

//コントロールのクリック時のコールバック
ABool	AWindow_FileList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	if( inID == mListViewControlID )
	{
		AIndex	index = GetListView().SPI_GetSelectedDBIndex();
		switch(mMode)
		{
			//==================リモートファイル==================
		  case kFileListMode_RemoteFile:
			{
				/*
				//リモート接続オブジェクトへ何らかの要求中には何もせずにbreak
				if( GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).SPI_GetCurrentRequestType() != kRemoteConnectionRequestType_None )
				{
					break;
				}
				*/
				if( index != kIndex_Invalid )
				{
					//アカウントリスト表示中 or アカウント毎のディレクトリ表示中の分岐
					if( mRemoteConnectionCurrentLevel == 0 )
					{
						//================リモートアカウントリスト表示をクリック時================
						
						//選択解除
						GetListView().SPI_SetSelect(kIndex_Invalid);
						
						//レベルインクリメント
						mRemoteConnectionCurrentLevel_Request = mRemoteConnectionCurrentLevel +1;
						
						//アカウントデータをDBから取得
						AText	server, user, path;
						GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_Server,index,server);
						GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_User,index,user);
						GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_RootPath,index,path);
						/*#1231 ポート番号はFTPアカウント設定から取得
						ANumber	portNumber = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFTPAccount_Port,index);//#193
						if( GetApp().NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kFTPAccount_UseDefaultPort,index) == true )
						{
							portNumber = 0;
						}
						*/
						//接続タイプ（プラグイン）をDBから取得
						AText	connectionType;
						GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_ConnectionType,index,connectionType);//#1231
						//
						mRemoteConnectionRootFolderURL_Request.SetFTPURL(connectionType,server,user,path,0);//#1231 portNumber);//#193
						mRemoteConnectionCurrentFolderURL_Request.SetText(mRemoteConnectionRootFolderURL_Request);
						mRemoteConnectionCurrentFolderURL_Request.AddLastPathDelimiter();
						
						/*#1231
						//接続タイプ設定 #193
						if( GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).SetConnectionType(mRemoteConnectionConnectionType_Request) == true )
						{
						*/
						//リモート接続オブジェクトにLIST実行要求
						GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
								ExecuteLIST(kRemoteConnectionRequestType_LISTfromFileListWindow,mRemoteConnectionCurrentFolderURL_Request,GetEmptyText());
						//プログレス表示 #1236
						ShowRemoteAccessProgress(true);
						
						//→LIST応答受信時、SPI_SetRemoteConnectionResult()がリモート接続オブジェクトからコールされ、
						//そこでリスト更新される。
						/*#1231
						}
						else
						{
							//★プラグインが無い場合
							
						}
						*/
					}
					else
					{
						//================リモートファイルリスト表示をクリック時================
						
						if( index == 0 )
						{
							//==================親フォルダに移動==================
							
							//選択解除
							GetListView().SPI_SetSelect(kIndex_Invalid);
							
							//レベルによる分岐
							if( mRemoteConnectionCurrentLevel <= 1 )
							{
								//リモートアカウントリスト表示状態に戻る
								mRemoteConnectionCurrentLevel = 0;
								//現在URL等消去
								mRemoteConnectionCurrentFolderURL.DeleteAll();
								mRemoteConnectionRootFolderURL.DeleteAll();
								//#1231 mRemoteConnectionConnectionType.DeleteAll();
								//テーブル更新
								SPI_UpdateTable();
							}
							else
							{
								//レベルデクリメント
								mRemoteConnectionCurrentLevel_Request = mRemoteConnectionCurrentLevel-1;
								//現在URLを親URLへ移動
								AText	parentURL;
								mRemoteConnectionCurrentFolderURL.GetParentURL(parentURL);
								mRemoteConnectionCurrentFolderURL_Request.SetText(parentURL);
								mRemoteConnectionCurrentFolderURL_Request.AddLastPathDelimiter();
								
								//リモート接続オブジェクトにLIST実行要求
								GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
										ExecuteLIST(kRemoteConnectionRequestType_LISTfromFileListWindow,
													mRemoteConnectionCurrentFolderURL_Request,GetEmptyText());
								//プログレス表示 #1236
								ShowRemoteAccessProgress(true);
								
								//→LIST応答受信時、SPI_SetRemoteConnectionResult()がリモート接続オブジェクトからコールされ、
								//そこでリスト更新される。
							}
						}
						else if( index == 1 )
						{
							//==================新規ファイル生成==================
							//新規ファイル生成ダイアログ
							GetApp().SPI_GetNewFTPFileWindow().NVI_CreateAndShow();
							GetApp().SPI_GetNewFTPFileWindow().SPI_SetFolder(mRemoteConnectionCurrentFolderURL);
							GetApp().SPI_GetNewFTPFileWindow().NVI_SwitchFocusToFirst();
							//モーダル開始
							GetApp().SPI_GetNewFTPFileWindow().NVI_RunModalWindow();
							//ウインドウを閉じる
							GetApp().SPI_GetNewFTPFileWindow().NVI_Close();
						}
						else
						{
							//==================ファイルまたはフォルダ選択時==================
							//ファイル／フォルダ名取得
							AText	filename;
							mRemoteConnectionFilenameArray.Get(index,filename);
							
							//ファイル／フォルダURL取得
							AText	fileURL;
							fileURL.SetText(mRemoteConnectionCurrentFolderURL);
							fileURL.AddPathDelimiter('/');
							fileURL.AddText(filename);
							
							//ファイル／フォルダ分岐
							if( mRemoteConnectionIsDiractoryArray.Get(index) == false )
							{
								//==================ファイル==================
								//選択解除
								GetListView().SPI_SetSelect(kIndex_Invalid);
								//リモートファイルドキュメントを開く要求
								GetApp().SPI_RequestOpenFileToRemoteConnection(/*#1231 mRemoteConnectionConnectionType,*/fileURL,false);
							}
							else
							{
								//==================フォルダ==================
								//レベルインクリメント
								mRemoteConnectionCurrentLevel_Request = mRemoteConnectionCurrentLevel +1;
								
								//URL更新
								mRemoteConnectionCurrentFolderURL_Request.SetText(fileURL);
								mRemoteConnectionCurrentFolderURL_Request.AddLastPathDelimiter();
								
								//リモート接続オブジェクトにLIST実行要求
								GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
										ExecuteLIST(kRemoteConnectionRequestType_LISTfromFileListWindow,
													mRemoteConnectionCurrentFolderURL_Request,GetEmptyText());
								//プログレス表示 #1236
								ShowRemoteAccessProgress(true);
							}
						}
					}
				}
				//選択項目更新
				mSelectionIndex_RemoteConnection = index;
				break;
			}
			//==================最近開いたファイル==================
		  case kFileListMode_RecentlyOpened:
			{
				if( index != kIndex_Invalid )
				{
					//現在の位置をNavigate登録する #146
					GetApp().SPI_RegisterNavigationPosition();
					//
					AText	text;
					GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFile).Get(index,text);
					//#235
					AIndex	p = 0;
					if( text.FindCstring(0,"://",p) == true )
					{
						//------------------リモートファイル------------------
						//"://"を含んでいるなら、リモートファイルとして、ドキュメントを開く
						//#361 GetApp().SPI_StartOpenFTPDocument(text,false);
						GetApp().SPI_RequestOpenFileToRemoteConnection(text,false);//#361
					}
					else
					{
						//------------------ローカルファイル------------------
						//B0389 OK GetApp().SPNVI_CreateDocumentByPathForDisplay(text);
						AFileAcc	file;
						file.Specify(text);//#910 ,kFilePathType_1);//B0389 ここはデータ互換性のため旧タイプのまま。
						//B0421 GetApp().SPNVI_CreateDocumentFromLocalFile(file);
						GetApp().GetAppPref().LaunchAppWithFile(file,inModifierKeys);//B0421
					}
				}
				//選択項目更新
				mSelectionIndex_Recent = index;
				break;
			}
			//==================同じフォルダ==================
		  case kFileListMode_SameFolder:
			{
				if( index != kIndex_Invalid )
				{
					//現在の位置をNavigate登録する #146
					GetApp().SPI_RegisterNavigationPosition();
					//
					AFileAcc	file;
					GetApp().SPI_GetSameFolderFile(index,file);
					//B0421 GetApp().SPNVI_CreateDocumentFromLocalFile(file);
					GetApp().GetAppPref().LaunchAppWithFile(file,inModifierKeys);//B0421
				}
				//選択項目更新
				mSelectionIndex_SameFolder = index;
				break;
			}
			//==================同じプロジェクト==================
		  case kFileListMode_SameProject:
			{
				if( index != kIndex_Invalid )
				{
					//現在の位置をNavigate登録する #146
					GetApp().SPI_RegisterNavigationPosition();
					//
					AFileAcc	file;
					file.Specify(mActionTextArray.GetTextConst(index));//#892
					GetApp().GetAppPref().LaunchAppWithFile(file,inModifierKeys);//B0421
				}
				//選択項目更新
				mSelectionIndex_SameProject = index;
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		return true;
	}
	switch(inID)
	{
	  case kButton_ModeChange:
	  case kControlID_TitleBar://#725
		{
			switch(mMode)
			{
			  case kFileListMode_RemoteFile:
				{
					SPI_ChangeMode(kFileListMode_RecentlyOpened);
					break;
				}
			  case kFileListMode_RecentlyOpened:
				{
					SPI_ChangeMode(kFileListMode_SameFolder);
					break;
				}
			  case kFileListMode_SameFolder:
				{
					SPI_ChangeMode(kFileListMode_SameProject);
					break;
				}
			  case kFileListMode_SameProject:
				{
					SPI_ChangeMode(kFileListMode_RemoteFile);
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			GetListView().SPI_AdjustScroll();
			result = true;
			break;
		}
	  case kButton_Sort:
		{
			mRecentSortMode = NVI_GetButtonViewByControlID(kButton_Sort).SPI_GetToggleOn();//#232 NVI_GetControlBool(kButton_Sort);
			if( mRecentSortMode == true )
			{
				GetListView().SPI_Sort(kColumn_FileName,true);
			}
			else
			{
				GetListView().SPI_SortOff();
			}
			result = true;
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return result;
}

//ウインドウサイズ変更通知時のコールバック
void	AWindow_FileList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

//#291
void	AWindow_FileList::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}

//#798
/**
コントロールでtext入力時処理
*/
void	AWindow_FileList::EVTDO_TextInputted( const AControlID inID )
{
	//フィルタで入力時
	if( inID == kControlID_Filter )
	{
		//フィルタテキスト取得
		AText	filter;
		NVI_GetControlText(kControlID_Filter,filter);
		filter.ChangeCaseLowerFast();
		
		//list viewにフィルタ設定
		GetListView().SPI_SetFilterText(kColumn_FileName,filter);
	}
}

//#798
/**
コントロールでリターンキー押下時処理
*/
ABool	AWindow_FileList::NVIDO_ViewReturnKeyPressed( const AControlID inControlID )
{
	//フィルタで入力時
	if( inControlID == kControlID_Filter )
	{
		//list viewでのクリック時と同じ処理を実行
		EVTDO_Clicked(mListViewControlID,kModifierKeysMask_None);
		
		//フィルタテキストを空にする
		NVI_SetControlText(kControlID_Filter,GetEmptyText());
		
		//list viewにフィルタ設定
		GetListView().SPI_SetFilterText(0,GetEmptyText());
	}
	return true;
}

//#798
/**
コントロールでESCキー押下時処理
*/
ABool	AWindow_FileList::NVIDO_ViewEscapeKeyPressed( const AControlID inControlID )
{
	//フィルタで入力時
	if( inControlID == kControlID_Filter )
	{
		//フィルタテキストを空にする
		NVI_SetControlText(kControlID_Filter,GetEmptyText());
		
		//list viewにフィルタ設定
		GetListView().SPI_SetFilterText(0,GetEmptyText());
		
		//テキストウインドウへフォーカス移動
		if( NVM_GetOverlayMode() == true )
		{
			GetApp().NVI_GetWindowByID(NVI_GetOverlayParentWindowID()).NVI_SelectWindow();
		}
		else
		{
			AWindowID	frontWinID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( frontWinID != kObjectID_Invalid )
			{
				GetApp().NVI_GetWindowByID(frontWinID).NVI_SelectWindow();
			}
		}
	}
	return true;
}

//#798
/**
コントロールで上キー押下時処理
*/
ABool	AWindow_FileList::NVIDO_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//フィルタで入力時
	if( inControlID == kControlID_Filter )
	{
		//list viewで前を選択
		GetListView().SPI_SetSelectPreviousDisplayRow(inModifers);
	}
	return true;
}

//#798
/**
コントロールで下キー押下時処理
*/
ABool	AWindow_FileList::NVIDO_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//フィルタで入力時
	if( inControlID == kControlID_Filter )
	{
		//list viewで次を選択
		GetListView().SPI_SetSelectNextDisplayRow(inModifers);
	}
	return true;
}

//#892
/**
リストで折りたたみ・展開実行後処理
*/
void	AWindow_FileList::NVIDO_ListViewExpandedCollapsed( const AControlID inControlID )
{
	//テキストウインドウへフォーカス移動
	if( NVM_GetOverlayMode() == true )
	{
		GetApp().NVI_GetWindowByID(NVI_GetOverlayParentWindowID()).NVI_SelectWindow();
	}
	else
	{
		AWindowID	frontWinID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
		if( frontWinID != kObjectID_Invalid )
		{
			GetApp().NVI_GetWindowByID(frontWinID).NVI_SelectWindow();
		}
	}
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_FileList::NVIDO_Create( const ADocumentID inDocumentID )
{
	//サブウインドウ用フォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	
	//ウインドウ生成
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//サイドバー
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Bottom,false,false,false,false,false,false,false);
			break;
		}
		//フローティング
	  default:
		{
			NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_None,false,false,false,false,false,true,false);
			break;
		}
	}
	
	//モードの復元
	mMode = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWindowListMode);
	//ウインドウ幅設定
	if( NVM_GetOverlayMode() == false )//#291
	{
		NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWindowListSizeX));
		NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWindowListSizeY));
	}
	
	//背景色描画用view生成
	{
		AViewDefaultFactory<AView_SubWindowBackground>	viewFactory(GetObjectID(),kControlID_Background);
		AWindowPoint	pt = {0,0};
		NVM_CreateView(kControlID_Background,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Background,true);
		NVI_SetControlBindings(kControlID_Background,true,true,true,true,false,false);
	}
	
	//ヘッダview生成
	{
		AViewDefaultFactory<AView_SubWindowHeader>	viewFactory(GetObjectID(),kControlID_Header);
		AWindowPoint	pt = {0,0};
		NVM_CreateView(kControlID_Header,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Header,true);
		NVI_SetControlBindings(kControlID_Header,true,true,true,false,false,true);
	}
	
	//フォルダーパスボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_FolderPath,pt,10,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_FolderPath,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_FolderPath).SPI_SetButtonViewType(kButtonViewType_TextWithNoHoverWithFixedWidth);
		NVI_GetButtonViewByControlID(kControlID_FolderPath).SPI_SetEllipsisMode(kEllipsisMode_FixedLastCharacters,10);
	}
	
	//プロジェクトモードフィルタボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_ProjectModeFilter,pt,10,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_ProjectModeFilter,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
		NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).SPI_SetEllipsisMode(kEllipsisMode_FixedFirstCharacters,10);
		//
		NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).SPI_SetMenu(kContextMenuID_SameProjectHeader,kMenuItemID_Invalid);
	}
	
	//SubWindow用タイトルバー生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("FileListWindow_RecentFile");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwList);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//検索ボックスを生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_Filter,pt,100,13,kControlID_Background,kIndex_Invalid,true,false,false,true,kEditBoxType_FilterBox);
		NVI_GetEditBoxView(kControlID_Filter).NVI_SetTextFont(fontname,fontsize);
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetTextColor(
					AColorWrapper::GetColorByHTMLFormatColor("1574cf"),
					AColorWrapper::GetColorByHTMLFormatColor("1574cf"));
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetBackgroundColor(
					kColor_White,kColor_Gray70Percent,kColor_Gray70Percent);
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetEnableFocusRing(false);
		NVI_SetAutomaticSelectBySwitchFocus(kControlID_Filter,true);
		NVI_SetControlBindings(kControlID_Filter,true,true,true,false,false,true);
	}
	//SizeBox生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
	
	//プログレスindicator #1236
	{
		//progress indicator生成
		AWindowPoint	pt = {16,64};
		NVI_CreateProgressIndicator(kControlID_RemoteAccessProgressIndicator,pt,kWidth_ProgressIndicator,kHeight_ProgressIndicator);
		NVI_SetControlBindings(kControlID_RemoteAccessProgressIndicator,true,true,false,false,true,true);
		ShowRemoteAccessProgress(false);
	}
}

//新規タブ生成
//引数：生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
void	AWindow_FileList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	AWindowPoint	pt = {0,0};
	//ListView生成
	mListViewControlID = NVM_AssignDynamicControlID();
	AListViewFactory	listViewFactory(/* #199 this,*this*/GetObjectID(),mListViewControlID);
	NVM_CreateView(mListViewControlID,pt,10,10,kControlID_Background,kControlID_Invalid,false,listViewFactory);
	/*#725
	if( NVM_GetOverlayMode() == true )//#291
	{
		NVI_GetListView(mListViewControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetListView(mListViewControlID).SPI_SetAlwaysActiveColors(true);
	}
	*/
	NVI_GetListView(mListViewControlID).SPI_SetAlwaysActiveColors(true);
	NVI_GetListView(mListViewControlID).SPI_SetBackgroundColor(
				GetApp().SPI_GetSubWindowBackgroundColor(true),
				GetApp().SPI_GetSubWindowBackgroundColor(false));
	//#725
	NVI_GetListView(mListViewControlID).SPI_SetEnableImageIcon(true);
	//#291 サイズbinding設定（区切り線変更時のちらつき防止）
	NVI_SetControlBindings(mListViewControlID,true,true,true,true,false,false);
	//#328 HelpTag
	NVI_EnableHelpTagCallback(mListViewControlID);
	
	//Vスクロールバー生成
	NVI_CreateScrollBar(kControlID_VScrollBar,pt,kVScrollBarWidth,kVScrollBarWidth*2);
	GetListView().NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_VScrollBar);
	//#291 サイズbinding設定（区切り線変更時のちらつき防止）
	NVI_SetControlBindings(kControlID_VScrollBar,false,true,true,true,true,false);
	
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);
	
	//SubWindow用セパレータ生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,mListViewControlID,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	/*#725
	//モード変更ボタン
	NVI_CreateButtonView(kButton_ModeChange,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);
	if( NVM_GetOverlayMode() == true )//#291
	{
		NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetAlwaysActiveColors(true);
	}
	NVI_EnableHelpTagCallback(kButton_ModeChange);
	//#291 サイズbinding設定（区切り線変更時のちらつき防止）
	NVI_SetControlBindings(kButton_ModeChange,true,true,true,false,false,true);
	//
	NVI_EmbedControl(kControlID_HeaderBar,kButton_ModeChange);
	
	//ソートボタン
	NVI_CreateButtonView(kButton_Sort,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);
	AText	t;
	t.SetLocalizedText("Sort");
	NVI_GetButtonViewByControlID(kButton_Sort).NVI_SetText(t);
	NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetToggleMode(true);
	if( NVM_GetOverlayMode() == true )//#291
	{
		NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetAlwaysActiveColors(true);
	}
	//#291 サイズbinding設定（区切り線変更時のちらつき防止）
	NVI_SetControlBindings(kButton_Sort,false,true,true,false,true,true);
	//
	NVI_EmbedControl(kControlID_HeaderBar,kButton_Sort);
	*/
	
	//view bounds更新
	UpdateViewBounds();
	
	GetListView().SPI_RegisterColumn_Text(kColumn_FileName,0,"",false);
	GetListView().SPI_SetEnableCursorRow();
	//GetListView().SPI_SetRowMargin(3);
	
	SPI_UpdateTable();
	
	//win
	NVI_UpdateProperty();
	
	//
	outInitialFocusControlID = mListViewControlID;
}

//#798
/**
Show()派生処理
*/
void	AWindow_FileList::NVIDO_Show()
{
	//ファイルリストウインドウ表示時に、現在の最前面ドキュメントに従ってテーブル設定する（これがないと、新規にファイルリストを表示した時に、モードが設定されない）
	ADocumentID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID != kObjectID_Invalid )
	{
		SPI_Update_DocumentActivated(docID);
	}
}

//Hide()時等にウインドウのデータを保存する
void	AWindow_FileList::NVIDO_Hide()
{
	//折りたたみデータをDBに保存
	SaveCollapseData();
	//
	if( NVM_GetOverlayMode() == false )//#291 Overlayモード時には保存しない
	{
		//ウインドウ幅の保存
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kWindowListSizeX,NVI_GetWindowWidth());
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kWindowListSizeY,NVI_GetWindowHeight());
	}
	//モードの保存
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kWindowListMode,static_cast<ANumber>(mMode));
}

//各種コントロールの状態をアップデートする
void	AWindow_FileList::NVMDO_UpdateControlStatus()
{
	switch(mMode)
	{
	  case kFileListMode_RecentlyOpened:
		{
			//フォルダパス更新
			AText	text;
			NVI_GetViewByControlID(kControlID_FolderPath).NVI_SetText(text);
			break;
		}
	  case kFileListMode_SameFolder:
		{
			//フォルダパス更新
			AText	text;
			GetApp().SPI_GetCurrentFolderPathForDisplay(text);
			NVI_GetViewByControlID(kControlID_FolderPath).NVI_SetText(text);
			break;
		}
	  case kFileListMode_RemoteFile:
		{
			//フォルダパス更新
			AText	text;
			text.SetText(mRemoteConnectionCurrentFolderURL);
			if( text.GetItemCount() == 0 )
			{
				//「アカウントを選択」を表示
				text.SetLocalizedText("FileList_Remote_SelectServer");
			}
			NVI_GetViewByControlID(kControlID_FolderPath).NVI_SetText(text);
			break;
		}
	  case kFileListMode_SameProject:
		{
			//フォルダパス更新
			AText	text;
			GetApp().SPI_GetCurrentProjectPathForDisplay(text);
			NVI_GetViewByControlID(kControlID_FolderPath).NVI_SetText(text);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
#if 0
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);//#375
	AText	text;
	switch(mMode)
	{
	  case kFileListMode_RecentlyOpened:
		{
			//#232 NVI_SetControlIcon(kButton_ModeChange,kIconID_Mi_FileList_RecentlyOpened,true);//win 080721
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetIcon(kIconID_Mi_FileList_RecentlyOpened,3,3);//#232
			text.SetLocalizedText("FileListWindow_RecentFile");
			//#232 NVI_SetControlText(kButton_ModeChange,text);
			//#232 NVI_SetControlTextJustification(kButton_ModeChange,kJustification_Left);//win 080721
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);//#232
			NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetText(text);//#232
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetHelpTag(text,text);//#661
			break;
		}
	  case kFileListMode_SameFolder:
		{
			//#232 NVI_SetControlIcon(kButton_ModeChange,kIconID_Mi_FileList_SameFolder,true);//win 080721
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetIcon(kIconID_Mi_FileList_SameFolder,3,3);//#232
			GetApp().SPI_GetCurrentFolderPathForDisplay(text);
			if( text.GetLength() == 0 )
			{
				text.SetLocalizedText("FileListWindow_SameFolder");
			}
			/*#232
			NVI_SetControlText(kButton_ModeChange,text);
			NVI_SetControlTextJustification(kButton_ModeChange,kJustification_Right);//win 080721
			*/
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_CenterTruncated);//#232 #315
			NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetText(text);//#232
			//#661
			AText	helpTagText;
			helpTagText.SetLocalizedText("HelpTag_FileListSameFolder");
			helpTagText.ReplaceParamText('0',text);
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetHelpTag(helpTagText,helpTagText);
			break;
		}
	  case kFileListMode_RemoteFile:
		{
			//#232 NVI_SetControlIcon(kButton_ModeChange,kIconID_Mi_FileList_Window,true);//win 080721
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetIcon(kIconID_Mi_FileList_Window,3,3);//#232
			text.SetLocalizedText("FileListWindow_Window");
			/*#232
			NVI_SetControlText(kButton_ModeChange,text);
			NVI_SetControlTextJustification(kButton_ModeChange,kJustification_Left);//win 080721
			*/
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);//#232
			NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetText(text);//#232
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetHelpTag(text,text);//#661
			break;
		}
	  case kFileListMode_SameProject:
		{
			//#232 NVI_SetControlIcon(kButton_ModeChange,kIconID_Mi_FileList_Project,true);//win 080721
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetIcon(kIconID_Mi_FileList_Project,3,3);//#232
			GetApp().SPI_GetCurrentProjectPathForDisplay(text);
			//#661
			AText	helpTagText;
			helpTagText.SetLocalizedText("HelpTag_FileListSameProject");
			//B0403
			if( text.GetItemCount() == 0 )
			{
				text.SetLocalizedText("FileListWindow_ProjectFolderNotDefined");
				helpTagText.SetLocalizedText("HelpTag_FileListSameProject_NotDefined");//#661
			}
			else
			{
				helpTagText.ReplaceParamText('0',text);
			}
			/*#232
			NVI_SetControlText(kButton_ModeChange,text);
			NVI_SetControlTextJustification(kButton_ModeChange,kJustification_Right);//win 080721
			*/
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_CenterTruncated);//#232 #315
			NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetText(text);//#232
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetHelpTag(helpTagText,helpTagText);//#661
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
#endif
	//#725
	//タイトルテキスト更新
	UpdateTitleText();
}

void	AWindow_FileList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	/*#725
	AText	fontname;//win
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kFileListFontName,fontname);//win
	GetListView().SPI_SetTextDrawProperty(
			fontname,GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kFileListFontSize),kColor_Black);
	*/
	 GetListView().SPI_SetWheelScrollPercent(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent),
				GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd));//R0000
	/*#725
	if( NVM_GetOverlayMode() == false )//#291
	{
		NVI_SetWindowTransparency((static_cast<AFloatNumber>(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWindowListAlphaPercent)))/100);
	}
	else
	{
		//Overlay時は情報ペインOverlay透明度を適用（ただし、MacOSXでは実際には何もしない。View側で塗りつぶし時に透明度適用。）
		NVI_SetWindowTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
	}
	*/
	//サブウインドウフォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//Filterのフォント設定
	NVI_GetEditBoxView(kControlID_Filter).NVI_SetTextFont(fontname,fontsize);
	AText	filtertext;
	filtertext.SetLocalizedText("Filter");
	//Filterに表示するショートカットキーバインドを取得
	AText	shortcuttext;
	switch(mMode)
	{
	  case kFileListMode_SameProject:
		{
			GetApp().NVI_GetMenuManager().GetMenuItemShortcutDisplayText(kMenuItemID_SearchInProjectFolder,shortcuttext);
			break;
		}
	  case kFileListMode_SameFolder:
		{
			GetApp().NVI_GetMenuManager().GetMenuItemShortcutDisplayText(kMenuItemID_SearchInSameFolder,shortcuttext);
			break;
		}
	  case kFileListMode_RecentlyOpened:
		{
			GetApp().NVI_GetMenuManager().GetMenuItemShortcutDisplayText(kMenuItemID_SearchInRecentlyOpenFiles,shortcuttext);
			break;
		}
	  default:
		{
			//処理無し
			break;
		}
	}
	//
	NVI_GetEditBoxView(kControlID_Filter).SPI_SetTextForEmptyState(filtertext,shortcuttext);
	//list viewのフォント設定
	GetListView().SPI_SetTextDrawProperty(fontname,fontsize,GetApp().SPI_GetSubWindowLetterColor());
	//ホイールスクロール率設定
	GetListView().SPI_SetWheelScrollPercent(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent),
				GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd));
	//Filterへのα設定
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		GetListView().SPI_SetTransparency(GetApp().SPI_GetFloatingWindowAlpha());
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetTransparency(GetApp().SPI_GetFloatingWindowAlpha());
	}
	else
	{
		GetListView().SPI_SetTransparency(1.0);
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetTransparency(1.0);
	}
	//「同じプロジェクト」のフィルターモード選択ボタンのテキスト設定
	switch(mProjectFilterMode)
	{
	  case kFileListProjectFilter_SameModeFiles:
		{
			AText	text;
			text.SetLocalizedText("FileListHeader_ProjectFilter_SameMode");
			AText	modename;
			if( mCurrentModeIndex != kIndex_Invalid )
			{
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetModeDisplayName(modename);
			}
			text.ReplaceParamText('0',modename);
			NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).NVI_SetText(text);
			break;
		}
	  case kFileListProjectFilter_AllFiles:
		{
			AText	text;
			text.SetLocalizedText("FileListHeader_ProjectFilter_AllFiles");
			NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).NVI_SetText(text);
			break;
		}
	  case kFileListProjectFilter_DiffFiles:
		{
			AText	text;
			text.SetLocalizedText("FileListHeader_ProjectFilter_DiffFiles");
			NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).NVI_SetText(text);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	NVI_GetEditBoxView(kControlID_Filter).SPI_SetBackgroundColorForEmptyState(
																			  GetApp().SPI_GetSubWindowBackgroundColor(true),
																			  GetApp().SPI_GetSubWindowBackgroundColor(false));
	
	//ヘッダ部分フォント
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	//ヘッダ色取得
	AColor	headerlettercolor = GetApp().SPI_GetSubWindowHeaderLetterColor();
	//ヘッダ部制御ボタン色設定
	NVI_GetButtonViewByControlID(kControlID_ProjectModeFilter).SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,
																				   kTextStyle_Bold,headerlettercolor,headerlettercolor);
	NVI_GetButtonViewByControlID(kControlID_FolderPath).SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,
																			kTextStyle_Bold,headerlettercolor,headerlettercolor);
	
	//view bounds更新
	UpdateViewBounds();
	//ウインドウrefresh
	NVI_RefreshWindow();
}

//#798
/**
Filterボックスにフォーカス移動
*/
void	AWindow_FileList::SPI_SwitchFocusToSearchBox()
{
	NVI_SelectWindow();
	NVI_SwitchFocusTo(kControlID_Filter);
	NVI_GetEditBoxView(kControlID_Filter).NVI_SetSelectAll();
}


#pragma mark ===========================

#pragma mark ---表示更新

//#725
/**
ファイルリスト各種更新
*/
void	AWindow_FileList::SPI_UpdateFileList( const AFileListUpdateType inType, const ADocumentID inDocumentID )
{
	switch(inType)
	{
		//最近開いたファイルデータの更新時
	  case kFileListUpdateType_RecentlyOpenedFileUpdated:
		{
			SPI_Update_RecentlyOpendFileUpdated();
			break;
		}
		//プロジェクトフォルダ読み込み完了時
	  case kFileListUpdateType_ProjectFolderLoaded:
		{
			if( mMode == kFileListMode_SameProject )
			{
				SPI_UpdateTable();
			}
			break;
		}
		//同じフォルダデータの更新時
	  case kFileListUpdateType_SameFolderDataUpdated:
		{
			if( mMode == kFileListMode_SameFolder )
			{
				SPI_UpdateTable();
			}
			break;
		}
		//同じプロジェクトデータの更新時
	  case kFileListUpdateType_SameProjectDataUpdated:
		{
			if( mMode == kFileListMode_SameProject )
			{
				SPI_UpdateTable();
			}
			break;
		}
		//SCMデータの更新時
	  case kFileListUpdateType_SCMDataUpdated:
		{
			SPI_UpdateTable();
			break;
		}
		//ドキュメントactivate時
	  case kFileListUpdateType_DocumentActivated:
		{
			SPI_Update_DocumentActivated(inDocumentID);
			break;
		}
		//ドキュメントopen時
	  case kFileListUpdateType_DocumentOpened:
		{
			SPI_Update_DocumentOpened(inDocumentID);
			break;
		}
		//ドキュメントclose時
	  case kFileListUpdateType_DocumentClosed:
		{
			break;
		}
		//リモートファイルアカウント設定変更時 #193
	  case kFileListUpdateType_RemoteFileAccountDataChanged:
		{
			SPI_UpdateTable();
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

void	AWindow_FileList::SPI_UpdateTable() 
{
	if( NVI_IsWindowCreated() == false )   return;
	
	//通常文字色取得
	AColor	subwindowNormalColor = GetApp().SPI_GetSubWindowLetterColor();
	
	AIndex	sel = kIndex_Invalid;
	GetListView().SPI_BeginSetTable();
	switch(mMode)
	{
		//==================最近開いたファイル==================
	  case kFileListMode_RecentlyOpened:
		{
			GetListView().SPI_SetColumn_Text(kColumn_FileName,GetApp().SPI_GetRecentlyOpenedFileDisplayTextArray());
			//#725
			//全ての項目にドキュメントアイコンを設定
			AArray<AImageID>	imageIconIDArray;
			AArray<AColor>	colorarray;
			AItemCount	itemCount = GetApp().SPI_GetRecentlyOpenedFileDisplayTextArray().GetItemCount();
			for( AIndex i = 0; i < itemCount; i++ )
			{
				colorarray.Add(subwindowNormalColor);
				imageIconIDArray.Add(kImageID_iconSwlDocument);
			}
			GetListView().SPI_SetTable_ImageIconID(imageIconIDArray);
			GetListView().SPI_SetTable_Color(colorarray);
			//最近開いたファイル用のコンテキストメニュー設定
			GetListView().SPI_SetEnableContextMenu(true,kContextMenuID_RecentOpen);//R0186
			sel = mSelectionIndex_Recent;
			break;
		}
		//==================同じフォルダ==================
	  case kFileListMode_SameFolder:
		{
			ATextArray	samefolder;
			GetApp().SPI_GetSameFolderFilenameTextArray(samefolder);
			GetListView().SPI_SetColumn_Text(kColumn_FileName,samefolder);
			//#725
			//全ての項目にドキュメントアイコンを設定
			AArray<AImageID>	imageIconIDArray;
			AItemCount	itemCount = samefolder.GetItemCount();
			for( AIndex i = 0; i < itemCount; i++ )
			{
				imageIconIDArray.Add(kImageID_iconSwlDocument);
			}
			GetListView().SPI_SetTable_ImageIconID(imageIconIDArray);
			
			//R0006
			if(	GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true /*#844&& 
				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplaySCMStateInFileList) == true*/ )
			{
				/*AArray<ASCMFileState>	scmfilestate;
				GetApp().SPI_GetSameFolderSCMStateArray(scmfilestate);
				AArray<ATextStyle>	stylearray;
				AItemCount	count = scmfilestate.GetItemCount();
				if( count > 0 )
				{
				for( AIndex i = 0; i < count; i++ )
				{
				ATextStyle	style = kTextStyle_Normal;
				if( scmfilestate.Get(i) != kSCMFileState_None )
				{
				style = kTextStyle_Italic;
				}
				stylearray.Add(style);
				}
				}
				GetListView().SPI_SetTable_TextStyle(stylearray);*/
				AArray<ASCMFileState>	scmfilestate;
				GetApp().SPI_GetSameFolderSCMStateArray(scmfilestate);
				AArray<AColor>	colorarray;
				AArray<ATextStyle>	stylearray;//#458
				AItemCount	count = scmfilestate.GetItemCount();
				if( count > 0 )
				{
					for( AIndex i = 0; i < count; i++ )
					{
						AColor	color = subwindowNormalColor;
						ATextStyle	style = kTextStyle_Normal;//#458
						ASCMFileState	filestate = scmfilestate.Get(i);
						if( filestate == kSCMFileState_NotEntried )
						{
							//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDisplaySCMStateInFileList_NotEntriedColor,color);
							color = kColor_Gray;//#844
						}
						else if( filestate == kSCMFileState_Others )
						{
							//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDisplaySCMStateInFileList_OthersColor,color);
							color = kColor_Pink;//#844
						}
						else if( filestate != kSCMFileState_UpToDate && filestate != /*kSCMFolderType_NotSCMFolder*/kSCMFileState_NotSCMFolder )
						{
							//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDisplaySCMStateInFileList_Color,color);
							color = GetApp().SPI_GetFileListDiffColor();//#844
							style = kTextStyle_Bold;//#458
						}
						colorarray.Add(color);
						stylearray.Add(style);//#458
					}
				}
				GetListView().SPI_SetTable_Color(colorarray);
				GetListView().SPI_SetTable_TextStyle(stylearray);//#458
			}
			
			GetListView().SPI_SetEnableContextMenu(true,kContextMenuID_FileList);//R0186 #442
			sel = mSelectionIndex_SameFolder;
			break;
		}
		//#361
		//==================リモートファイル==================
	  case kFileListMode_RemoteFile:
		{
			if( mRemoteConnectionCurrentLevel == 0 )
			{
				//================リモートファイルアカウントリスト表示================
				
				//各アカウント毎にサーバー名を取得し、textArrayに格納する
				ATextArray	textArray;
				AArray<AColor>	colorarray;
				AItemCount	itemCount = GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kFTPAccount_Server);
				for( AIndex i = 0; i < itemCount; i++ )
				{
					colorarray.Add(subwindowNormalColor);
					//アカウント表示名取得
					AText	server;
					GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_Server,i,server);
					AText	text;
					GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_DisplayName,i,text);
					if( text.GetItemCount() > 0 )
					{
						text.AddCstring(" (");
						text.AddText(server);
						text.AddCstring(")");
					}
					else
					{
						text.SetText(server);
					}
					text.AddCstring(" ");
					//
					AText	existSubMenuText;
					existSubMenuText.SetLocalizedText("ExistSubMenu");
					text.AddText(existSubMenuText);
					textArray.Add(text);
				}
				//textArrayをテーブル列に設定
				GetListView().SPI_SetColumn_Text(kColumn_FileName,textArray);
				//全ての項目にフォルダアイコン設定
				AArray<AImageID>	imageIconIDArray;
				for( AIndex i = 0; i < itemCount; i++ )
				{
					imageIconIDArray.Add(kImageID_iconSwlFolder);
				}
				GetListView().SPI_SetTable_ImageIconID(imageIconIDArray);
				GetListView().SPI_SetTable_Color(colorarray);
			}
			else
			{
				//================リモートファイルリスト表示================
				
				//各ファイル／フォルダ毎のループ
				ATextArray	textArray;
				AArray<AColor>	colorarray;
				AArray<AImageID>	imageIconIDArray;
				AItemCount	itemCount = mRemoteConnectionFilenameArray.GetItemCount();
				for( AIndex i = 0; i < itemCount; i++ )
				{
					colorarray.Add(subwindowNormalColor);
					//ファイル／フォルダ名取得
					AText	text;
					mRemoteConnectionFilenameArray.Get(i,text);
					if( i == 0 )
					{
						//==================1番目の項目（親フォルダへ移動）の場合==================
						//フォルダ名＋右三角
						text.AddCstring(" ");
						AText	existSubMenuText;
						existSubMenuText.SetLocalizedText("ExistParentMenu");
						text.InsertText(0,existSubMenuText);
						//親フォルダ移動アイコン
						imageIconIDArray.Add(kImageID_iconSwlParentPath);
					}
					else if( i == 1 )
					{
						//==================2番目の項目（新規ファイル生成）の場合==================
						//ドキュメントアイコン
						imageIconIDArray.Add(kImageID_iconSwlDocument);
					}
					else if( mRemoteConnectionIsDiractoryArray.Get(i) == true )
					{
						//==================フォルダ項目の場合==================
						//フォルダ名＋右三角
						text.AddCstring(" ");
						AText	existSubMenuText;
						existSubMenuText.SetLocalizedText("ExistSubMenu");
						text.AddText(existSubMenuText);
						//フォルダアイコン
						imageIconIDArray.Add(kImageID_iconSwlFolder);
					}
					else
					{
						//==================ファイル項目の場合==================
						//ドキュメントアイコン
						imageIconIDArray.Add(kImageID_iconSwlDocument);
					}
					textArray.Add(text);
				}
				//テキスト、アイコン設定
				GetListView().SPI_SetColumn_Text(kColumn_FileName,textArray);
				GetListView().SPI_SetTable_ImageIconID(imageIconIDArray);
				GetListView().SPI_SetTable_Color(colorarray);
			}
			//選択項目設定
			sel = mSelectionIndex_RemoteConnection;
			break;
		}
		//==================同じプロジェクト==================
	  case kFileListMode_SameProject:
		{
			//#892
			
			//SCM使用可否取得
			ABool	useSCM = (GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true /*#844&& 
					GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplaySCMStateInFileList) == true*/ );
			//同じプロジェクトのファイルパス、および、モードindexを取得
			ATextArray	projData_FullPathArray;//#533
			AArray<AModeIndex>	projData_ModeIndexArray;//#533
			ABoolArray	projData_IsFolderArray;
			AText	projFolderPath;
			GetApp().SPI_GetCurrentSameProjectData(projFolderPath,projData_FullPathArray,projData_ModeIndexArray,projData_IsFolderArray);//#533
			//表示データ初期化
			mActionTextArray.DeleteAll();
			ATextArray	textArray;
			AArray<AImageID>	imageIconIDArray;
			AArray<AColor>	colorarray;
			AArray<ATextStyle>	stylearray;//#458
			AArray<AIndex>	outlinearray;
			
			//#533 現在のモードの項目だけを表示データに追加する。
			//各ファイル／フォルダ毎のループ
			for( AIndex i = 0; i < projData_FullPathArray.GetItemCount(); i++ )
			{
				//パス取得
				AText	path;
				projData_FullPathArray.Get(i,path);
				//ファイル取得
				AFileAcc	file;
				file.Specify(path);
				//プロジェクト内相対パス取得
				AText	partialPath;
				path.GetText(projFolderPath.GetItemCount(),path.GetItemCount()-projFolderPath.GetItemCount(),partialPath);
				//ファイル名、モードindex取得
				AText	filename;
				partialPath.GetFilename(filename);
				AColor	color = subwindowNormalColor;
				ATextStyle	style = kTextStyle_Normal;//#458
				AModeIndex	modeIndex = projData_ModeIndexArray.Get(i);
				ABool	isFolder = projData_IsFolderArray.Get(i);
				//SCM状態に応じた文字色取得
				ASCMFileState	filestate = kSCMFileState_NotSCMFolder;
				if( useSCM == true )
				{
					filestate = GetApp().SPI_GetSCMFolderManager().GetFileState(path);//GetFileState(file);
					if( filestate == kSCMFileState_NotEntried )
					{
						color = kColor_Gray;//#844
					}
					else if( filestate == kSCMFileState_Others )
					{
						color = kColor_Pink;//#844
					}
					else if( filestate != kSCMFileState_UpToDate && filestate != kSCMFileState_NotSCMFolder )
					{
						color = kColor_DarkBlue;//#844
						style = kTextStyle_Bold;//#458
					}
				}
				//プロジェクト内ファイルフィルターモードに従って、表示対象ファイルかどうかを判定
				ABool	filterMatchedItem = false;
				switch(mProjectFilterMode)
				{
					//フィルタ：全てのファイル
				  case kFileListProjectFilter_AllFiles:
					{
						filterMatchedItem = true;
						break;
					}
					//フィルタ：同じモード
				  case kFileListProjectFilter_SameModeFiles:
					{
						if( modeIndex == kIndex_Invalid || modeIndex == mCurrentModeIndex )
						{
							filterMatchedItem = true;
						}
						break;
					}
					//フィルタ：変更有りファイル
				  case kFileListProjectFilter_DiffFiles:
					{
						//
						if( isFolder == false )
						{
							switch(filestate)
							{
							  case kSCMFileState_Modified:
							  case kSCMFileState_Added:
							  case kSCMFileState_Others:
							  case kSCMFileState_NotEntried:
								{
									filterMatchedItem = true;
									break;
								}
							  default:
								{
									//処理なし
									break;
								}
							}
						}
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
				//フィルタ後、対象ファイルなら、表示データに追加
				if( filterMatchedItem == true )//#873
				{
					if( isFolder == true )
					{
						//------------------フォルダの場合------------------
						mActionTextArray.Add(GetEmptyText());
						textArray.Add(partialPath);
						imageIconIDArray.Add(kImageID_iconSwlFolder);
						colorarray.Add(subwindowNormalColor);
						stylearray.Add(kTextStyle_Normal);
						partialPath.RemoveLastPathDelimiter();
					}
					else
					{
						//------------------ファイルの場合------------------
						mActionTextArray.Add(path);
						textArray.Add(filename);
						imageIconIDArray.Add(kImageID_iconSwlDocument);
						colorarray.Add(color);
						stylearray.Add(style);//#458
					}
					//パス内の'/'の数に従ってアウトラインレベル取得
					AIndex	outlineLevel = 0;
					switch(mProjectFilterMode)
					{
					  case kFileListProjectFilter_AllFiles:
					  case kFileListProjectFilter_SameModeFiles:
						{
							outlineLevel = partialPath.GetCountOfChar('/');
							break;
						}
					  default:
						{
							//処理なし
							break;
						}
					}
					outlinearray.Add(outlineLevel*12);
				}
			}
			//テーブルに表示データ設定
			GetListView().SPI_SetColumn_Text(kColumn_FileName,textArray);
			GetListView().SPI_SetTable_ImageIconID(imageIconIDArray);
			GetListView().SPI_SetTable_Color(colorarray);
			GetListView().SPI_SetTable_TextStyle(stylearray);//#458
			GetListView().SPI_SetTable_OutlineFoldingLevel(outlinearray);
			//折りたたみ状態を復元
			if( GetApp().SPI_GetCurrentSameProjectIndex() != kIndex_Invalid )
			{
				switch(mProjectFilterMode)
				{
				  case kFileListProjectFilter_AllFiles:
				  case kFileListProjectFilter_SameModeFiles:
					{
						ATextArray	collapsedata;
						GetApp().SPI_GetProjectFolderCollapseData(GetApp().SPI_GetCurrentSameProjectIndex(),collapsedata);
						GetListView().SPI_ApplyCollapseRowsByTextArray(kColumn_FileName,collapsedata);
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
			}
			//コンテキストメニュー設定
			GetListView().SPI_SetEnableContextMenu(true,kContextMenuID_FileList);//R0186 #442
			//選択項目設定
			sel = mSelectionIndex_SameProject;
			mCurrentSameProjectIndex = GetApp().SPI_GetCurrentSameProjectIndex();
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	GetListView().SPI_EndSetTable();
	GetListView().SPI_SetSelect(sel);
	if( mMode == kFileListMode_RecentlyOpened && mRecentSortMode == true )
	{
		GetListView().SPI_Sort(kColumn_FileName,true);
	}
	NVM_UpdateControlStatus();
}

void	AWindow_FileList::SPI_Update_DocumentOpened( const ADocumentID inTextDocumentID )
{
	if( NVI_IsWindowCreated() == false )   return;
	
	switch(mMode)
	{
	  case kFileListMode_RemoteFile:
		{
			SPI_UpdateTable();
			break;
		}
	  case kFileListMode_SameFolder:
	  case kFileListMode_SameProject:
	  case kFileListMode_RecentlyOpened:
		{
			//処理なし
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

void	AWindow_FileList::SPI_Update_DocumentActivated( const ADocumentID inTextDocumentID )
{
	if( NVI_IsWindowCreated() == false )   return;
	
	//#533
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetModeIndex();
	//
	switch(mMode)
	{
	  case kFileListMode_SameFolder:
		{
			AFileAcc	file;
			GetApp().SPI_GetTextDocumentByID(inTextDocumentID).NVI_GetFile(file);
			AFileAcc	parent;
			parent.SpecifyParent(file);
			AIndex	index = GetApp().SPI_GetSameFolderArrayIndex(parent);
			if( mCurrentSameFolderIndex != index )
			{
				mSelectionIndex_SameFolder = kIndex_Invalid;
				mCurrentSameFolderIndex = index;
				SPI_UpdateTable();
				GetListView().SPI_AdjustScroll();
				//
				NVI_UpdateProperty();
			}
			break;
		}
	  case kFileListMode_SameProject:
		{
			//
			AFileAcc	projectFolder;
			GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetProjectFolder(projectFolder);
			AIndex	index = GetApp().SPI_GetSameProjectArrayIndex(projectFolder);
			//現在のプロジェクトと違う、または、（フィルタ：同じモードで）モードindexが違う場合、テーブル更新
			if( mCurrentSameProjectIndex != index || 
						(mCurrentModeIndex != modeIndex && mProjectFilterMode == kFileListProjectFilter_SameModeFiles) ) //#533 #873
			{
				//#892
				//折りたたみ状態保存
				SaveCollapseData();
				//テーブル更新
				mSelectionIndex_SameProject = kIndex_Invalid;
				mCurrentSameProjectIndex = index;
				mCurrentModeIndex = modeIndex;//#533
				SPI_UpdateTable();
				GetListView().SPI_AdjustScroll();
				//
				NVI_UpdateProperty();
			}
			break;
		}
	  case kFileListMode_RemoteFile:
	  case kFileListMode_RecentlyOpened:
		{
			//処理なし
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//#533
	mCurrentModeIndex = modeIndex;
}

void	AWindow_FileList::SPI_Update_RecentlyOpendFileUpdated()
{
	if( NVI_IsWindowCreated() == false )   return;
	
	switch(mMode)
	{
	  case kFileListMode_RecentlyOpened:
		{
			SPI_UpdateTable();
			GetListView().SPI_SetSelect(0);
			mSelectionIndex_Recent = 0;
			GetListView().SPI_AdjustScroll();
			break;
		}
	  case kFileListMode_SameFolder:
	  case kFileListMode_SameProject:
	  case kFileListMode_RemoteFile:
		{
			//処理なし
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

void	AWindow_FileList::SPI_ChangeMode( const ANumber& inMode )
{
	if( inMode < kFileListMode_Min || inMode > kFileListMode_Max )   return;
	//#892
	SaveCollapseData();
	//
	mMode = inMode;
	UpdateViewBounds();
	SPI_UpdateTable();
	NVM_UpdateControlStatus();
	NVI_UpdateProperty();//#798
	NVI_RefreshWindow();
	//最前面のウインドウで、サイドバーのデータをapp pref dbに保存する（次に開くウインドウに、今回のモード変更を適用させるため）
	AWindowID	frontWinID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
	if( frontWinID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(frontWinID).SPI_SaveSideBarData();
	}
}

/*#725
void	AWindow_FileList::SPI_UpdateSelection_Window( const AIndex& inSelection )
{
	if( NVI_IsWindowCreated() == false )   return;
	
	GetListView().SPI_SetSelect(inSelection);
	mSelectionIndex_RemoteConnection = inSelection;
}
*/

/**
タイトルテキスト更新
*/
void	AWindow_FileList::UpdateTitleText()
{
	switch(mMode)
	{
	  case kFileListMode_RecentlyOpened:
		{
			AText	text;
			text.SetLocalizedText("FileListWindow_RecentFile");
			GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwBox_R);
			break;
		}
	  case kFileListMode_SameFolder:
		{
			AText	text;
			text.SetLocalizedText("FileListWindow_SameFolder");
			GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwBox_S);
			break;
		}
	  case kFileListMode_SameProject:
		{
			AText	text;
			text.SetLocalizedText("FileListWindow_SameProject");
			GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwBox_P);
			break;
		}
	  case kFileListMode_RemoteFile:
		{
			AText	text;
			text.SetLocalizedText("FileListWindow_RemoteFile");
			GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwBox_R);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

/*B0000 080810 void	AWindow_FileList::SPI_ClearCursorRow()
{
	GetListView().SPI_ClearCursorRow();
}
*/
//R0186
AIndex	AWindow_FileList::SPI_GetContextMenuSelectedRowDBIndex() const
{
	return GetListViewConst().SPI_GetContextMenuSelectedRowDBIndex();
}

//#892
/**
折りたたみ状態を保存
*/
void	AWindow_FileList::SaveCollapseData()
{
	switch(mMode)
	{
	  case kFileListMode_SameProject:
		{
			switch(mProjectFilterMode)
			{
			  case kFileListProjectFilter_AllFiles:
			  case kFileListProjectFilter_SameModeFiles:
				{
					if( mActionTextArray.GetItemCount() > 0 )
					{
						//折りたたみ行のテキストリストを取得
						ATextArray	textarray;
						GetListView().SPI_GetCollapseRowsTextArray(kColumn_FileName,textarray);
						//同じプロジェクトのデータに保存
						GetApp().SPI_SaveProjectFolderCollapseData(mCurrentSameProjectIndex,textarray);
					}
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//#361
/**
現在のパスを取得
*/
void	AWindow_FileList::SPI_GetCurrentPath( AText& outText ) const
{
	//結果初期化
	outText.DeleteAll();
	
	//履歴記憶しない場合は何もせず終了
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )
	{
		return;
	}
	
	switch(mMode)
	{
	  case kFileListMode_RemoteFile:
		{
			outText.SetNumber(mRemoteConnectionCurrentLevel);
			outText.AddCstring("\r");
			outText.AddText(mRemoteConnectionRootFolderURL);
			outText.AddCstring("\r");
			outText.AddText(mRemoteConnectionCurrentFolderURL);
			outText.AddCstring("\r");
            //#1231 outText.AddText(mRemoteConnectionConnectionType);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

/**
現在のパスを設定
*/
void	AWindow_FileList::SPI_SetCurrentPath( const AText& inText )
{
	switch(mMode)
	{
	  case kFileListMode_RemoteFile:
		{
			//アカウントリスト表示にする（LIST要求の応答が無かった時に、アカウントクリックにより別のアカウントへのアクセスに変更できるようにするため）
			mRemoteConnectionCurrentLevel = 0;
			//#1231 mRemoteConnectionConnectionType.DeleteAll();
			mRemoteConnectionCurrentFolderURL.DeleteAll();
			mRemoteConnectionRootFolderURL.DeleteAll();
			SPI_UpdateTable();
			//LIST要求データ取得
			AIndex	p = 0;
			AText	levelText;
			inText.GetLine(p,levelText);
			mRemoteConnectionCurrentLevel_Request = levelText.GetNumber();
			inText.GetLine(p,mRemoteConnectionRootFolderURL_Request);
			inText.GetLine(p,mRemoteConnectionCurrentFolderURL_Request);
			//#1231 inText.GetLine(p,mRemoteConnectionConnectionType_Request);
			mRemoteConnectionCurrentFolderURL_Request.AddLastPathDelimiter();
			
			if( mRemoteConnectionCurrentLevel_Request > 0 )
			{
				//リモート接続オブジェクトでプラグイン接続
				/*#1231
				if( GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
					SetConnectionType(mRemoteConnectionConnectionType_Request) == true )
				{
				*/
				//リモート接続オブジェクトにLIST実行要求
				GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
						ExecuteLIST(kRemoteConnectionRequestType_LISTfromFileListWindow,
									mRemoteConnectionCurrentFolderURL_Request,GetEmptyText());
				//プログレス表示 #1236
				ShowRemoteAccessProgress(true);
				
				//→LIST応答受信時、SPI_SetRemoteConnectionResult()がリモート接続オブジェクトからコールされ、
				//そこでリスト更新される。
				/*#1231
				}
				else
				{
					//★プラグインが無い場合
				}
				*/
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

#if 0
//#725 削除
//#291
/**
オーバーレイモード時の透明度設定
*/
void	AWindow_FileList::SPI_UpdateOverlayWindowAlpha()
{
	if( NVI_GetOverlayMode() == true )
	{
		NVI_GetListView(mListViewControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
	}
}

//#634
/**
サイドバーモードの透過率・背景色に切り替え
*/
void	AWindow_FileList::SPI_SetSideBarMode( const ABool inSideBarMode, const AColor inSideBarColor )
{
	if( NVI_GetOverlayMode() == true )
	{
		if( inSideBarMode == true )
		{
			//できる限り文字をきれいに表示させるために不透過率はできるだけ高めに設定する。（背景のグラデーションがうっすら透過する程度）
			NVI_GetListView(mListViewControlID).SPI_SetBackgroundColor(inSideBarColor,kColor_Gray97Percent);
			NVI_GetListView(mListViewControlID).SPI_SetTransparency(0.8);
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTransparency(0.8);
			NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetTransparency(0.8);
		}
		else
		{
			NVI_GetListView(mListViewControlID).SPI_SetBackgroundColor(kColor_White,kColor_White);
			NVI_GetListView(mListViewControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
			NVI_GetButtonViewByControlID(kButton_ModeChange).SPI_SetTransparency(1);
			NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetTransparency(1);
		}
	}
}
#endif

#pragma mark ===========================

#pragma mark ---リモート接続
//#361

/**
リモート接続オブジェクトへのLIST要求に対する応答受信時処理
*/
void	AWindow_FileList::SPI_SetRemoteConnectionResult( const ATextArray& inFileNameArray, const ABoolArray& inIsDirectoryArray )
{
	//プログレス非表示 #1236
	ShowRemoteAccessProgress(false);
	
	//LIST結果が全くない場合は何もせずリターン（エラーでない場合は.と..が存在するはず）
	if( inFileNameArray.GetItemCount() == 0 )
	{
		return;
	}
	
	//要求データを、現在データに設定
	mRemoteConnectionCurrentLevel = mRemoteConnectionCurrentLevel_Request;
    //#1231 mRemoteConnectionConnectionType.SetText(mRemoteConnectionConnectionType_Request);
	mRemoteConnectionCurrentFolderURL.SetText(mRemoteConnectionCurrentFolderURL_Request);
	mRemoteConnectionRootFolderURL.SetText(mRemoteConnectionRootFolderURL_Request);
	//受信データを設定
	mRemoteConnectionFilenameArray.SetFromTextArray(inFileNameArray);
	mRemoteConnectionIsDiractoryArray.SetFromObject(inIsDirectoryArray);
	//".", ".."の項目は削除する
	for( AIndex i = 0; i < mRemoteConnectionFilenameArray.GetItemCount(); )
	{
		AText	filename;
		mRemoteConnectionFilenameArray.Get(i,filename);
		if( filename.Compare(".") == true || filename.Compare("..") == true )
		{
			mRemoteConnectionFilenameArray.Delete1(i);
			mRemoteConnectionIsDiractoryArray.Delete1(i);
		}
		else
		{
			i++;
		}
	}
	//1番目の項目を親フォルダへの移動項目にする
	AText	parentFolderTitle;
	parentFolderTitle.SetLocalizedText("RemoteFileParentFolder");
	mRemoteConnectionFilenameArray.Insert1(0,parentFolderTitle);
	mRemoteConnectionIsDiractoryArray.Insert1(0,true);
	//2番目の項目を新規ファイル生成用項目にする
	AText	newFile;
	newFile.SetLocalizedText("RemoteFileNewFile");
	mRemoteConnectionFilenameArray.Insert1(1,newFile);
	mRemoteConnectionIsDiractoryArray.Insert1(1,false);
	//テーブル更新
	SPI_UpdateTable();
	//選択解除
	GetListView().SPI_SetSelect(kIndex_Invalid);
	//スクロール調整
	GetListView().SPI_AdjustScroll();
}

/**
リモートファイルモード時、現在のフォルダを更新（ドキュメント保存時に実行される）
*/
void	AWindow_FileList::SPI_UpdateRemoteFolder( const AText& inFolderURL )
{
	if( mMode == kFileListMode_RemoteFile )
	{
		AText	url1, url2;
		url1.SetText(mRemoteConnectionCurrentFolderURL);
		url1.AddPathDelimiter(kUChar_Slash);
		url2.SetText(inFolderURL);
		url2.AddPathDelimiter(kUChar_Slash);
		if( url1.Compare(url2) == true )
		{
			//現在のフォルダ内容で更新
			mRemoteConnectionCurrentLevel_Request = mRemoteConnectionCurrentLevel;
			mRemoteConnectionRootFolderURL_Request.SetText(mRemoteConnectionRootFolderURL);
			mRemoteConnectionCurrentFolderURL_Request.SetText(mRemoteConnectionCurrentFolderURL);
			//#1231 mRemoteConnectionConnectionType_Request.SetText(mRemoteConnectionConnectionType);
			mRemoteConnectionCurrentFolderURL_Request.AddLastPathDelimiter();
			
			//リモート接続オブジェクトでプラグイン接続
			/*#1231 
			if( GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
				SetConnectionType(mRemoteConnectionConnectionType_Request) == true )
			{
			*/
			//リモート接続オブジェクトにLIST実行要求
			GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
					ExecuteLIST(kRemoteConnectionRequestType_LISTfromFileListWindow,
								mRemoteConnectionCurrentFolderURL_Request,GetEmptyText());
			//プログレス表示 #1236
			ShowRemoteAccessProgress(true);
			
			//→LIST応答受信時、SPI_SetRemoteConnectionResult()がリモート接続オブジェクトからコールされ、
			//そこでリスト更新される。
			/*#1231
			}
			*/
		}
	}
}

#pragma mark ===========================

#pragma mark ---

AView_List&	AWindow_FileList::GetListView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_List,kViewType_List,mListViewControlID);
	//#199 return dynamic_cast<AView_List&>(NVI_GetViewByControlID(mListViewControlID));
}
//R0186
const AView_List&	AWindow_FileList::GetListViewConst() const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_List,kViewType_List,mListViewControlID);
	//#199 return dynamic_cast<const AView_List&>(NVI_GetViewConstByControlID(mListViewControlID));
}

void	AWindow_FileList::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//タイルバー、セパレータ高さ取得
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	ANumber	height_Filter = NVI_GetEditBoxView(kControlID_Filter).SPI_GetLineHeightWithMargin() + kHeight_FilterTopBottomMargin;
	ANumber	height_Header = kHeight_Header1Line;
	switch( mMode )
	{
	  case kFileListMode_RecentlyOpened:
		{
			height_Header = 0;
			break;
		}
	  case kFileListMode_SameProject:
		{
			height_Header = kHeight_Header2Lines;
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	
	//
	ABool	isSideBarBottom = GetApp().SPI_IsSubWindowSideBarBottom(GetObjectID());
	
	//背景色描画view調整
	ANumber	leftMargin = 1;
	ANumber	rightMargin = 1;
	ANumber	bottomMargin = 1;
	AWindowPoint	backgroundPt = {0,height_Titlebar};
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			leftMargin = 0;
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			rightMargin = 0;
			break;
		}
	  default:
		{
			leftMargin = 0;
			rightMargin = 0;
			bottomMargin = 0;
			break;
		}
	}
	if( isSideBarBottom == true )
	{
		bottomMargin = 0;
	}
	
	//タイトルバー、セパレータ表示フラグ
	ABool	showTitleBar = false;
	ABool	showSeparator = false;
	ABool	showSizeBox = false;
	//list viewサイズ
	AWindowPoint	listViewPoint = {leftMargin,0};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top  -bottomMargin;
	//スクロールバー幅
	ANumber	vscrollbarWidth = kVScrollBarWidth;
	//
	ANumber	sizeboxHeight = 0;
	//右サイドバーの最終項目の場合、かつ、ステータスバー未表示時以外はサイズボックスの対象は自ウインドウ
	NVI_SetSizeBoxTargetWindowID(GetObjectID());
	//レイアウト計算
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//サイドバーの場合
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//タイトルバー、セパレータ表示フラグ
			showTitleBar = true;
			showSeparator = true;
			//list viewサイズ
			listViewPoint.y += height_Titlebar + height_Filter + height_Header;
			listViewHeight -= height_Titlebar + height_Filter + height_Header;// + height_Separator;
			/*
			if( isSideBarBottom == true )
			{
				showSeparator = false;
				//右サイドバーの最終項目の場合、かつ、ステータスバー未表示時はサイズボックスを表示する
				if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_RightSideBar &&
							GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayStatusBar) == false )
				{
					showSizeBox = true;
					//サイズボックス対象をテキストウインドウにする
					NVI_SetSizeBoxTargetWindowID(NVI_GetOverlayParentWindowID());
				}
			}
			*/
			break;
		}
		//フローティングの場合
	  default:
		{
			//タイトルバー、セパレータ表示フラグ
			showTitleBar = true;
			//list viewサイズ
			listViewPoint.y += height_Titlebar + height_Filter + height_Header;
			listViewHeight -= height_Titlebar + height_Filter + height_Header;
			//
			showSizeBox = true;
			sizeboxHeight = kHeight_SizeBox;
			break;
		}
	}
	
	//タイトルバー配置
	if( showTitleBar == true )
	{
		//タイトルバー配置
		AWindowPoint	pt = {-1+leftMargin,0};
		NVI_SetControlPosition(kControlID_TitleBar,pt);
		NVI_SetControlSize(kControlID_TitleBar,windowBounds.right - windowBounds.left +2-leftMargin-rightMargin,height_Titlebar);
		//タイトルバー表示
		NVI_SetShowControl(kControlID_TitleBar,true);
	}
	else
	{
		//タイトルバー非表示
		NVI_SetShowControl(kControlID_TitleBar,false);
	}
	
	//背景描画view配置
	{
		//背景描画view配置
		NVI_SetControlPosition(kControlID_Background,backgroundPt);
		NVI_SetControlSize(kControlID_Background,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top - backgroundPt.y);
		//背景描画view表示
		NVI_SetShowControl(kControlID_Background,true);
	}
	
	//セパレータ配置
	if( showSeparator == true )
	{
		//セパレータ配置
		AWindowPoint	pt = {0,windowBounds.bottom-windowBounds.top-height_Separator};
		NVI_SetControlPosition(kControlID_Separator,pt);
		NVI_SetControlSize(kControlID_Separator,windowBounds.right-windowBounds.left,height_Separator);
		//セパレータ表示
		NVI_SetShowControl(kControlID_Separator,true);
	}
	else
	{
		//セパレータ非表示
		NVI_SetShowControl(kControlID_Separator,false);
	}
	
	//フィルタ配置
	{
		AWindowPoint	pt = {leftMargin,height_Titlebar + height_Header};
		NVI_SetControlPosition(kControlID_Filter,pt);
		NVI_SetControlSize(kControlID_Filter,windowBounds.right-windowBounds.left - leftMargin - rightMargin,height_Filter);
	}
	
	//ヘッダ配置
	if( height_Header > 0 )
	{
		AWindowPoint	pt = {leftMargin,height_Titlebar};
		NVI_SetControlPosition(kControlID_Header,pt);
		NVI_SetControlSize(kControlID_Header,windowBounds.right-windowBounds.left - leftMargin - rightMargin,height_Header);
		NVI_SetShowControl(kControlID_Header,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_Header,false);
	}
	
	//フォルダーパスボタン配置
	if( height_Header > 0 )
	{
		AWindowPoint	pt = {kLeftMargin_HeaderButton,height_Titlebar + 2};
		NVI_SetControlPosition(kControlID_FolderPath,pt);
		NVI_SetControlSize(kControlID_FolderPath,windowBounds.right-windowBounds.left-kLeftMargin_HeaderButton-kRightMargin_HeaderButton,kHeight_HeaderButton);
		NVI_SetShowControl(kControlID_FolderPath,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_FolderPath,false);
	}
	
	//プロジェクトモードフィルタボタン配置
	if( mMode == kFileListMode_SameProject )
	{
		AWindowPoint	pt = {kLeftMargin_HeaderButton,height_Titlebar + 19};
		NVI_SetControlPosition(kControlID_ProjectModeFilter,pt);
		NVI_SetControlSize(kControlID_ProjectModeFilter,windowBounds.right-windowBounds.left-kLeftMargin_HeaderButton-kRightMargin_HeaderButton,kHeight_HeaderButton);
		NVI_SetShowControl(kControlID_ProjectModeFilter,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_ProjectModeFilter,false);
	}
	
	//FileListView bounds設定
	{
		NVI_SetControlPosition(mListViewControlID,listViewPoint);
		NVI_SetControlSize(mListViewControlID,listViewWidth - vscrollbarWidth,listViewHeight);
	}
	//Vスクロールバー配置
	if( vscrollbarWidth > 0 && NVI_IsControlEnable(kControlID_VScrollBar) == true )
	{
		AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
		NVI_SetControlPosition(kControlID_VScrollBar,pt);
		NVI_SetControlSize(kControlID_VScrollBar,vscrollbarWidth,listViewHeight - sizeboxHeight);
		NVI_SetShowControl(kControlID_VScrollBar,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_VScrollBar,false);
	}
	
	//サイズボックス配置
	if( showSizeBox == true )
	{
		AWindowPoint	pt = {windowBounds.right-windowBounds.left-kWidth_SizeBox - rightMargin,
		windowBounds.bottom-windowBounds.top-kHeight_SizeBox - bottomMargin};
		NVI_SetControlPosition(kControlID_SizeBox,pt);
		NVI_SetControlSize(kControlID_SizeBox,kWidth_SizeBox,kHeight_SizeBox);
		NVI_SetShowControl(kControlID_SizeBox,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_SizeBox,false);
	}
	
#if 0
	if( mMode == kFileListMode_RecentlyOpened )
	{
		AWindowPoint	pt;
		pt.x = 4;
		pt.y = kTitleBarHeight + 4;
		ANumber	w = windowBounds.right - windowBounds.left -kSortButtonWidth -16;
		ANumber	h = kModeChangeButtonHeight;
#if IMPLEMENTATION_FOR_MACOSX
		//pt.x--;
		//pt.y--;
		//w += 2;
		w++;
		//h++;
#endif
		/*#232
		NVI_SetControlPosition(kButton_ModeChange,pt);
		NVI_SetControlSize(kButton_ModeChange,w,h);
		*/
		NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetPosition(pt);
		NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetSize(w,h);
		pt.x = windowBounds.right - windowBounds.left -kSortButtonWidth;
		pt.y = kTitleBarHeight + 4;
		w = kSortButtonWidth;
		h = kModeChangeButtonHeight;
# if IMPLEMENTATION_FOR_MACOSX
		//pt.y--;
		//h++;
#endif
		/*#232
		NVI_SetControlPosition(kButton_Sort,pt);
		NVI_SetShowControl(kButton_Sort,true);
		NVI_SetControlSize(kButton_Sort,w,h);
		NVI_SetControlBool(kButton_Sort,mRecentSortMode);
		*/
		NVI_GetButtonViewByControlID(kButton_Sort).NVI_SetPosition(pt);
		NVI_GetButtonViewByControlID(kButton_Sort).NVI_SetSize(w,h);
		NVI_GetButtonViewByControlID(kButton_Sort).SPI_SetToogleOn(mRecentSortMode);
		NVI_GetButtonViewByControlID(kButton_Sort).NVI_SetShow(true);
	}
	else
	{
		AWindowPoint	pt;
		pt.x = 4;
		pt.y = kTitleBarHeight + 4;
#if IMPLEMENTATION_FOR_MACOSX
		//pt.x--;
		//pt.y--;
#endif
		//#232 NVI_SetControlPosition(kButton_ModeChange,pt);
		NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetPosition(pt);
		ANumber	w = windowBounds.right - windowBounds.left -16;
		ANumber	h = kModeChangeButtonHeight;
#if IMPLEMENTATION_FOR_MACOSX
		//w += 2;
		//h++;
#endif
		//#232 NVI_SetControlSize(kButton_ModeChange,w,h);
		NVI_GetButtonViewByControlID(kButton_ModeChange).NVI_SetSize(w,h);
		//#232 NVI_SetShowControl(kButton_Sort,false);
		NVI_GetButtonViewByControlID(kButton_Sort).NVI_SetShow(false);
	}
	#endif
}

//#1236
/**
リモートアクセスプログレス表示制御
*/
void AWindow_FileList::ShowRemoteAccessProgress( const ABool inShow )
{
	//
	NVI_SetShowControl(kControlID_RemoteAccessProgressIndicator,inShow);
	NVI_SetControlBool(kControlID_RemoteAccessProgressIndicator,inShow);
}





