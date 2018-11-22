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

AWindow_AddToolButton

*/

#include "stdafx.h"

#include "AWindow_AddToolButton.h"
#include "AApp.h"

//コントロールID
/*#724
const AControlID	kIcon0					= 5100;
const AControlID	kIcon29					= 5129;
const AControlID	kRadioUser				= 5300;
const AControlID	kRadioBuiltin			= 5301;
const AControlID	kRadioFolder			= 5302;
*/
const AControlID	kToolName				= 5200;
/*#724
const AControlID	kIconSelect				= 5201;
const AControlID	kBuiltinPopup			= 5202;
const AControlID	kFolderPath				= 5203;
const AControlID	kFolderSelect			= 5204;
const AControlID	kIconScroll				= 5205;
const AControlID	kRadioStandard			= 5303;//#232
const AControlID	kStandardToolList		= 5207;//#232
const AControlID	kStandardToolListScroll	= 5208;//#232
*/
const AControlID	kOK						= 5001;
const AControlID	kCancel					= 5002;

const ANumber	kButtonWidth = 24;//win
const ANumber	kButtonHeight = 24;//win

#pragma mark ===========================
#pragma mark [クラス]AWindow_AddToolButton
#pragma mark ===========================
//環境設定ウインドウ

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow_AddToolButton::AWindow_AddToolButton(/*#199  AWindow& inWindow */ const AWindowID inParentWindowID ) 
		: AWindow(/*#175NULL*/), /*#199 mWindow(inWindow), */
		mParentWindowID(inParentWindowID),
		/*#724 mCurrentIconIndexOffset(0),*/ mModeIndex(kStandardModeIndex), mEditWithTextWindow(false)
{
	/*#724
	//
	AText	text;
	//#248
	text.SetLocalizedText("StandardToolButton_NewDocument");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<NEWDOCUMENT-SAMEMODE TOOLBARICON(icon_doc)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Doc);
	//
	text.SetLocalizedText("StandardToolButton_Save");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<SAVE TOOLBARICON(icon_save)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Save);
	//
	text.SetLocalizedText("StandardToolButton_Undo");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<UNDO TOOLBARICON(icon_undo)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Undo);
	//
	text.SetLocalizedText("StandardToolButton_Redo");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<REDO TOOLBARICON(icon_redo)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Redo);
	//
	text.SetLocalizedText("StandardToolButton_FindSelected");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<FIND-DIALOG TOOLBARICON(icon_find)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Find);
	//
	text.SetLocalizedText("StandardToolButton_FindNext");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<FIND-SELECTED-NEXT TOOLBARICON(icon_find_next)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_FindNext);
	//
	text.SetLocalizedText("StandardToolButton_FindPrev");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<FIND-SELECTED-PREV TOOLBARICON(icon_find_prev)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_FindPrev);
	//
	text.SetLocalizedText("StandardToolButton_Highlight");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<HIGHLIGHT-SELECTED TOOLBARICON(icon_highlight)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Highlight);
	//
	text.SetLocalizedText("StandardToolButton_MultiFileFindOnlyVisible");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<MULTIFILEFIND-SELECTED(ONLYVISIBLEFILE) TOOLBARICON(icon_find_folder)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_FindInFolder);
	//
	text.SetLocalizedText("StandardToolButton_MultiFileFindInProjectOnlyVisible");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<MULTIFILEFIND-SELECTED-INPROJECT(ONLYVISIBLEFILE) TOOLBARICON(icon_find_folder)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_FindInFolder);
	//win
	text.SetLocalizedText("StandardToolButton_SplitView");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<SPLITVIEW TOOLBARICON(icon_splitview)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_SplitView);
	//
	text.SetLocalizedText("StandardToolButton_IdInfo");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<IDINFO TOOLBARICON(icon_info)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Info);
	//
	text.SetLocalizedText("StandardToolButton_Indent");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<INDENT TOOLBARICON(icon_indent)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Indent);
	//
	text.SetLocalizedText("StandardToolButton_TimeStamp6");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<DATE-6 TOOLBARICON(icon_stamp)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Stamp);
	//#146
	text.SetLocalizedText("StandardToolButton_NavPrev");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<NAV-PREV TOOLBARICON(icon_nav_prev)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_NavPrev);
	//#146
	text.SetLocalizedText("StandardToolButton_NavNext");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<NAV-NEXT TOOLBARICON(icon_nav_next)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_NavNext);
	//
	text.SetLocalizedText("StandardToolButton_IndexWindowNext");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<INDEXWINDOW-NEXT TOOLBARICON(icon_tree_next)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_TreeNext);
	//
	text.SetLocalizedText("StandardToolButton_IndexWindowPrev");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<INDEXWINDOW-PREV TOOLBARICON(icon_tree_prev)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_TreePrev);
	//
	text.SetLocalizedText("StandardToolButton_SwitchCrossCaret");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<SHOWHIDE-CROSSCARET TOOLBARICON(icon_guide)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Guide);
	//#340
	text.SetLocalizedText("StandardToolButton_ShowHideLeftSideBarColumn");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<SHOWHIDE-SUBPANECOLUMN TOOLBARICON(icon_subpane)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_SubPane);
	//#354
	text.SetLocalizedText("StandardToolButton_ChangeFontToModeDefault");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<CHANGEFONT-MODEDEFAULT TOOLBARICON(icon_fontsize)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_FontSize);
	//#354
	text.SetLocalizedText("StandardToolButton_RevertFont");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<REVERTFONT TOOLBARICON(icon_fontsize)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_FontSize);
	//#390
	text.SetLocalizedText("StandardToolButton_KeyRecStartWithWindow");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<KEYRECORD-START-WITHWINDOW TOOLBARICON(icon_record)");
	mStandardToolButtonArray_IconID.Add(kIconID_Record);
	//#390
	text.SetLocalizedText("StandardToolButton_KeyRecStart");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<KEYRECORD-START TOOLBARICON(icon_record)");
	mStandardToolButtonArray_IconID.Add(kIconID_Record);
	//#390
	text.SetLocalizedText("StandardToolButton_KeyRecStop");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<KEYRECORD-STOP TOOLBARICON(icon_stop)");
	mStandardToolButtonArray_IconID.Add(kIconID_Stop);
	//#390
	text.SetLocalizedText("StandardToolButton_KeyRecPlay");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<KEYRECORD-PLAY TOOLBARICON(icon_play)");
	mStandardToolButtonArray_IconID.Add(kIconID_Play);
	//#404
	text.SetLocalizedText("StandardToolButton_FullScreen");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<FULLSCREEN TOOLBARICON(icon_fullscreen)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_FullScreen);
	//#455
	text.SetLocalizedText("StandardToolButton_Commit");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<SCM-COMMIT TOOLBARICON(icon_commit)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Commit);
	//#379
	text.SetLocalizedText("StandardToolButton_DiffRepository");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<DIFF-REPOSITORY TOOLBARICON(icon_diff)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Diff);
	//#379
	text.SetLocalizedText("StandardToolButton_PrevDiff");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<MOVECARET-PREVDIFF TOOLBARICON(icon_up)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Up);
	//#379
	text.SetLocalizedText("StandardToolButton_NextDiff");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<MOVECARET-NEXTDIFF TOOLBARICON(icon_down)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Down);
	//#606
	text.SetLocalizedText("StandardToolButton_HandTool");
	mStandardToolButtonArray_Title.Add(text);
	mStandardToolButtonArray_Content.Add("<<<HANDTOOL TOOLBARICON(icon_hand)");
	mStandardToolButtonArray_IconID.Add(kIconID_Tool_Hand);
	*/
}
//デストラクタ
AWindow_AddToolButton::~AWindow_AddToolButton()
{
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

ADataBase&	AWindow_AddToolButton::NVMDO_GetDB()
{
	return GetApp().GetAppPref();
}

/**
親Window取得
*/
AWindow&	AWindow_AddToolButton::GetParentWindow()
{
	return AApplication::GetApplication().NVI_GetWindowByID(mParentWindowID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

ABool	AWindow_AddToolButton::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	/*#724
	if( inID >= kIcon0 && inID <= kIcon29 )
	{
		for( AControlID i = kIcon0; i <= kIcon29; i++ )
		{
			if( i != inID )
			{
				NVI_SetControlBool(i,false);
			}
		}
		return true;
	}
	 */
	switch(inID)
	{
	  case kOK:
		{
			/*#724
			AIconID	iconID = GetApp().SPI_GetToolbarIconArray().Get(0);
			for( AControlID i = kIcon0; i <= kIcon29; i++ )
			{
				if( NVI_GetControlBool(i) == true )
				{
					if( mCurrentIconIndexOffset+i-kIcon0 < GetApp().SPI_GetToolbarIconArray().GetItemCount() )
					{
						iconID = GetApp().SPI_GetToolbarIconArray().Get(mCurrentIconIndexOffset+i-kIcon0);
						break;
					}
				}
			}
			if( NVI_GetControlBool(kRadioUser) == true )
			*/
			{
				//ツールフォルダ取得
				AFileAcc	toolbarFolder;
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarFolder(/*#844 false,*/toolbarFolder);
				//UIからファイル名取得
				AText	filename;
				NVI_GetControlText(kToolName,filename);
				//ツールファイル生成
				AFileAcc	file;
				file.SpecifyChild(toolbarFolder,filename);
				file.CreateFile();
				//#724
				//ツールテキスト書き込み
				file.WriteFile(mToolText);
				//#724 GetApp().SPI_CopyToolbarIconResource(iconID,file);
				//win file.SetCreatorType('MMKE','TEXT');
				AFileAttribute	attr;
				GetApp().SPI_GetToolFileAttribute(attr);
				file.SetFileAttribute(attr);
				//ツール追加
				GetApp().SPI_GetModePrefDB(mModeIndex).AddToolbarItem(kToolbarItemType_File,file,false);//#427
				if( mEditWithTextWindow == true )
				{
					//------------------ツールを編集するドキュメントを開く------------------
					NVI_Close();//B0410 SPNVI_CreateDocumentFromLocalFile()の前にクローズ（シート表示中は開いたドキュメントを前面に持ってこないようにしたため）
					ADocumentID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file);
					//#265
					if( docID != kObjectID_Invalid )
					{
						//ファイル名に対応するモードを取得
						AText	filepath;
						file.GetPath(filepath);
						AModeIndex	modeIndex = GetApp().SPI_FindModeIDByTextFilePath(filepath);
						//ファイル名に対応するモードが標準モードの時は、ツールコマンドモードを設定
						if( modeIndex == kStandardModeIndex )
						{
							AText	toolcommandModeName("TOOLCOMMAND");
							modeIndex = GetApp().SPI_FindModeIndexByModeRawName(toolcommandModeName);
						}
						//モード設定
						if( modeIndex != kIndex_Invalid )
						{
							GetApp().SPI_GetTextDocumentByID(docID).SPI_SetMode(modeIndex);
						}
					}
				}
			}
			/*#724
			else if( NVI_GetControlBool(kRadioBuiltin) == true )
			{
				AToolbarItemType	type;
				switch(NVI_GetControlNumber(kBuiltinPopup))
				{
					case 0:
					{
						type = kToolbarItemType_TextEncoding;
						break;
					}
				  case 1:
					{
						type = kToolbarItemType_ReturnCode;
						break;
					}
				  case 2:
					{
						type = kToolbarItemType_WrapMode;
						break;
					}
				  case 3:
					{
						type = kToolbarItemType_Mode;
						break;
					}
				}
				AFileAcc	file;
				GetApp().SPI_GetModePrefDB(mModeIndex).AddToolbarItem(type,file,false);//#427
			}
			else if( NVI_GetControlBool(kRadioFolder) == true )
			{
				AFileAcc	toolbarFolder;
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarFolder(false,toolbarFolder);
				AText	name;
				mFolder.GetFilename(name);
				AFileAcc	dstfolder;
				dstfolder.SpecifyUniqChildFile(toolbarFolder,name);
				
				//R0000 security
				AIndex	rowIndex = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarItemCount();
				AText	filelist, report;
				if( GetApp().SPI_ScreenModeExecutable(mFolder,filelist,report) == true )
				{
					GetApp().SPI_GetModePrefWindow(mModeIndex).SPI_ShowModeExecutableAlertWindow_Toolbar(mModeIndex,rowIndex,mFolder,dstfolder,filelist,report);
					NVI_Close();
					break;
				}
				
				//R0000 ファイルも対象にする
				if( mFolder.IsFolder() == false )
				{
					mFolder.CopyFileTo(dstfolder,true);
					GetApp().SPI_GetModePrefDB(mModeIndex).InsertToolbarItem(rowIndex,kToolbarItemType_File,dstfolder,false);//#427
				}
				else
				{
					mFolder.CopyFolderTo(dstfolder,true,true);
					//R0137
					if( dstfolder.IsBundleFolder() == true )
					{
						GetApp().SPI_GetModePrefDB(mModeIndex).InsertToolbarItem(rowIndex,kToolbarItemType_File,dstfolder,false);//#427
					}
					else
					GetApp().SPI_GetModePrefDB(mModeIndex).InsertToolbarItem(rowIndex,kToolbarItemType_Folder,dstfolder,false);//#427
				}
			}
			//#232 標準ツールボタン追加
			else if( NVI_GetControlBool(kRadioStandard) == true )
			{
				AddStandardIcon();
			}
			*/
			GetApp().SPI_GetModePrefWindow(mModeIndex).NVI_UpdateProperty();
			NVI_Close();
			break;
		}
	  case kCancel:
		{
			NVI_Close();
			break;
		}
		/*#724
	  case kFolderSelect:
		{
			NVI_ShowChooseFolderWindow(mFolder,kFolderSelect,false);//#551
			break;
		}
	  case kRadioFolder:
		{
			if( mFolder.IsSpecified() == false )
			{
				AFileAcc	folder;//#551
				AFileWrapper::GetDesktopFolder(folder);//#551
				NVI_ShowChooseFolderWindow(folder,kFolderSelect,false);//#551
			}
			break;
		}
		//#232
	  case kStandardToolList:
		{
			NVI_SetControlBool(kRadioStandard,true);
			break;
		}
		*/
	}
	return result;
}

ABool	AWindow_AddToolButton::EVTDO_ValueChanged( const AControlID inID )
{
	ABool	result = false;
	switch(inID)
	{
	}
	return result;
}

ABool	AWindow_AddToolButton::EVTDO_DoubleClicked( const AControlID inID )
{
	ABool	result = false;
	switch(inID)
	{
		/*#724
		//#232
	  case kStandardToolList:
		{
			AddStandardIcon();
			GetApp().SPI_GetModePrefWindow(mModeIndex).NVI_UpdateProperty();
			NVI_Close();
			result = true;
			break;
		}
		*/
	}
	return result;
}

void	AWindow_AddToolButton::EVTDO_FileChoosen( const AControlID inControlID, const AFileAcc& inFile )
{
	switch(inControlID)
	{
	}
}

void	AWindow_AddToolButton::EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder )
{
	switch(inControlID)
	{
		/*#724
	  case kFolderSelect:
		{
			mFolder.CopyFrom(inFolder);
			NVM_UpdateControlStatus();
			break;
		}
		*/
	}
}

void	AWindow_AddToolButton::EVTDO_TextInputted( const AControlID inID )
{
	switch(inID)
	{
	  case kToolName:
		{
			NVM_UpdateControlStatus();
			break;
		}
	}
}

void	AWindow_AddToolButton::EVTDO_DoScrollBarAction( const AControlID inID, const AScrollBarPart inPart )
{
	switch(inID)
	{
		/*#724
	  case kIconScroll:
		{
			switch(inPart)
			{
			  case kScrollBarPart_UpButton:
				{
					NVI_SetControlNumber(kIconScroll,NVI_GetControlNumber(kIconScroll)-1);
					break;
				}
			  case kScrollBarPart_DownButton:
				{
					NVI_SetControlNumber(kIconScroll,NVI_GetControlNumber(kIconScroll)+1);
					break;
				}
			}
		}
		mCurrentIconIndexOffset = NVI_GetControlNumber(kIconScroll)*10;
		if( mCurrentIconIndexOffset < 0 )   mCurrentIconIndexOffset = 0;
		//#232 NVI_UpdateProperty();
		UpdateIconList();
		break;
		*/
	}
}

//#232
/**
標準アイコン追加
*/
/*#724
void	AWindow_AddToolButton::AddStandardIcon()
{
	//#237 AIndex	selected = NVI_GetListView(kStandardToolList).SPI_GetSelectedDBIndex();
	//#237 if( selected != kIndex_Invalid )
	AArray<AIndex>	selectedDBIndexArray;
	NVI_GetListView(kStandardToolList).SPI_GetSelectedDBIndexArray(selectedDBIndexArray);//#237
	for( AIndex i = 0; i < selectedDBIndexArray.GetItemCount(); i++ )//#237
	{
		AIndex	selected = selectedDBIndexArray.Get(i);//#237
		
		AFileAcc	toolbarFolder;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarFolder(false,toolbarFolder);
		//ファイル生成
		AText	title;
		title.SetText(mStandardToolButtonArray_Title.GetTextConst(selected));
		AFileAcc	destfile;
		destfile.SpecifyChild(toolbarFolder,title);
		destfile.CreateFile();
		destfile.WriteFile(mStandardToolButtonArray_Content.GetTextConst(selected));
		*
		//アイコンファイルコピー
		AText	iconfiletitle;
		iconfiletitle.SetText(title);
		AText	iconfilesuffix;
		CWindowImp::GetIconFileSuffix(iconfilesuffix);
		iconfiletitle.AddText(iconfilesuffix);
		AFileAcc	dsticonfile;
		dsticonfile.SpecifyChild(toolbarFolder,iconfiletitle);
		CWindowImp::CopyIconToFile(mStandardToolButtonArray_IconID.Get(selected),dsticonfile);
		*
		//ModePrefに登録
		AFileAttribute	attr;
		GetApp().SPI_GetToolFileAttribute(attr);
		destfile.SetFileAttribute(attr);
		GetApp().SPI_GetModePrefDB(mModeIndex).AddToolbarItem(kToolbarItemType_File,destfile,false);//#427
	}
}
 */


//win
/**
ウインドウ閉じるボタン
*/
void	AWindow_AddToolButton::EVTDO_DoCloseButton()
{
	NVI_Close();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void AWindow_AddToolButton::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVI_SetFixTitle(true);//win 080726
	NVM_CreateWindow("main/AddToolButton");
	
	/*#724
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadioUser);
	NVI_AddToLastRegisteredRadioGroup(kRadioStandard);//#232
	NVI_AddToLastRegisteredRadioGroup(kRadioBuiltin);
	NVI_AddToLastRegisteredRadioGroup(kRadioFolder);
	*/
	//#688 NVI_CreateEditBoxView(kToolName);
	NVI_RegisterToFocusGroup(kToolName);
	/*#724
	NVI_RegisterToFocusGroup(kStandardToolList,true);//#353
	NVI_RegisterToFocusGroup(kBuiltinPopup,true);//#353
	NVI_RegisterToFocusGroup(kFolderSelect,true);//#353
	NVI_RegisterToFocusGroup(kCancel,true);//#353
	NVI_RegisterToFocusGroup(kOK,true);//#353
	
	//win
	NVI_RegisterRadioGroup();
	AWindowPoint	spt;
	NVI_GetControlPosition(5999,spt);
	spt.x += 63;
	for( AControlID i = kIcon0; i <= kIcon29; i++ )
	{
		AWindowPoint	pt = spt;
		pt.x += ((i-kIcon0)%10)*(kButtonWidth+2);
		pt.y += ((i-kIcon0)/10)*(kButtonHeight+2);
		NVI_CreateButtonView(i,pt,kButtonWidth,kButtonHeight,kControlID_Invalid);
		NVI_GetButtonViewByControlID(i).SPI_SetAlwaysActiveColors(true);
		NVI_GetButtonViewByControlID(i).SPI_SetToggleMode(true);
		NVI_SetControlBindings(i,true,true,false,false,false,false);
		NVI_AddToLastRegisteredRadioGroup(i);
	}
	*/
	NVI_SetWindowStyleToSheet(GetParentWindow().GetObjectID());
	/*#724
	NVI_SetControlBool(kIcon0,true);
	NVI_RegisterScrollBar(kIconScroll);
	NVI_SetMinMax(kIconScroll,0,(GetApp().SPI_GetToolbarIconArray().GetItemCount()+9)/10-2);
	*/
	NVI_SetDefaultOKButton(kOK);
	NVI_SetDefaultCancelButton(kCancel);
	/*#724
	mFolder.Unspecify();
	
	//#232
	//標準ボタンListView作成
	NVI_CreateListViewFromResource(kStandardToolList);
	NVI_GetListView(kStandardToolList).SPI_CreateFrame();
	NVI_GetListView(kStandardToolList).NVI_SetScrollBarControlID(kControlID_Invalid,kStandardToolListScroll);
	NVI_GetListView(kStandardToolList).SPI_RegisterColumn_Text(0,0,"",false);
	NVI_GetListView(kStandardToolList).SPI_SetRowMargin(5);
	NVI_GetListView(kStandardToolList).SPI_SetMultiSelectEnabled(true);//#237
	
	//
	mCurrentIconIndexOffset = 0;
	UpdateIconList();
	
	//初期選択設定 win
	NVI_SetControlBool(kRadioUser,true);
	NVI_SetControlNumber(kBuiltinPopup,0);
	
	//ヘルプボタン登録
	NVI_RegisterHelpAnchor(90001,"topic.htm#toolbar");
	*/
	//ヘルプボタン登録
	NVI_RegisterHelpAnchor(90001,"topic.htm#yokutsukau");
}

void	AWindow_AddToolButton::SPI_SetMode( const AModeIndex inModeIndex, const ABool inEditWithTextWindow,
		const AText& inInitialTitle, const AText& inToolText )
{
	mModeIndex = inModeIndex;
	mEditWithTextWindow = inEditWithTextWindow;
	//ツール内容テキスト記憶
	mToolText.SetText(inToolText);
	//初期ツール名をUIに設定
	NVI_SetControlText(kToolName,inInitialTitle);
	//表示更新
	NVM_UpdateControlStatus();
	
	/*#724
	//#232
	//標準ツールボタンの名称存在チェック
	ABoolArray	enablearray;
	for( AIndex i = 0; i < mStandardToolButtonArray_Title.GetItemCount(); i++ )
	{
		AFileAcc	toolbarFolder;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarFolder(false,toolbarFolder);
		AFileAcc	file;
		file.SpecifyChild(toolbarFolder,mStandardToolButtonArray_Title.GetTextConst(i));
		enablearray.Add(file.Exist()==false);
	}
	//標準ボタンListViewのデータ設定
	NVI_GetListView(kStandardToolList).SPI_SetEnableIcon(true);
	NVI_GetListView(kStandardToolList).SPI_BeginSetTable();
	NVI_GetListView(kStandardToolList).SPI_SetColumn_Text(0,mStandardToolButtonArray_Title);
	NVI_GetListView(kStandardToolList).SPI_SetTable_Icon(mStandardToolButtonArray_IconID);
	NVI_GetListView(kStandardToolList).SPI_SetTable_Enabled(enablearray);
	NVI_GetListView(kStandardToolList).SPI_EndSetTable();
	*/
}

#pragma mark ===========================

#pragma mark <汎化メソッド特化部分>

#pragma mark ===========================

//コントロール状態（Enable/Disable等）を更新
//NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()からコールされる
//クリックやアクティベートでコールされるので、あまり重い処理をNVMDO_UpdateControlStatus()には入れないこと。
void	AWindow_AddToolButton::NVMDO_UpdateControlStatus()
{
	/*#724
	ABool	b1 = NVI_GetControlBool(kRadioUser);
	if( NVI_IsControlEnable(kToolName) != b1 )//SetEnableControlとMLTEPaneの問題から、SetEnableControlで判定できないため
	{
		NVI_SetControlEnable(kToolName,b1);
	}
	//
	AItemCount	count = GetApp().SPI_GetToolbarIconArray().GetItemCount();
	for( AControlID i = kIcon0; i <= kIcon29; i++ )
	{
		AIndex	iconIndex = mCurrentIconIndexOffset + i-kIcon0;
		if( iconIndex < count )
		{
			//#232 処理高速化のためAIconID	iconID = GetApp().SPI_GetToolbarIconArray().Get(iconIndex);
			//#232 処理高速化のためNVI_SetControlIcon(i,iconID);
			NVI_SetControlEnable(i,b1);
		}
		else
		{
			//#232 処理高速化のためNVI_SetControlIcon(i,GetApp().SPI_GetToolbarIconArray().Get(0));
			NVI_SetControlEnable(i,false);
		}
	}
	//
	ABool	b2 =  NVI_GetControlBool(kRadioBuiltin);
	NVI_SetControlEnable(kBuiltinPopup,b2);
	ABool	b3 =  NVI_GetControlBool(kRadioFolder);
	NVI_SetControlEnable(kFolderPath,b3);
	NVI_SetControlEnable(kFolderSelect,b3);
	//#232
	//標準ボタンのコントロールEnable/Disable設定
	ABool	b0 = NVI_GetControlBool(kRadioStandard);
	//NVI_SetControlEnable(kStandardToolList,b0);
	//NVI_SetControlEnable(kStandardToolListScroll,b0);
	
	if( b1 == true )
	*/
	{
		AText	filename;
		NVI_GetControlText(kToolName,filename);
		if( filename.GetItemCount() == 0 )
		{
			NVI_SetControlEnable(kOK,false);
		}
		else
		{
			AFileAcc	toolbarFolder;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarFolder(/*#844 false,*/toolbarFolder);
			AFileAcc	file;
			file.SpecifyChild(toolbarFolder,filename);
			NVI_SetControlEnable(kOK,(file.Exist()==false));
		}
	}
	/*#724
	else if( b2 == true )
	{
		NVI_SetControlEnable(kOK,true);
	}
	else if( b3 == true )
	{
		NVI_SetControlEnable(kOK,(mFolder.IsSpecified()));
	}
	//#232
	//標準ボタンの場合、行選択ありなら追加ボタンをEnableにする
	else if( b0 == true )
	{
		NVI_SetControlEnable(kOK,(NVI_GetListView(kStandardToolList).SPI_GetSelectedDBIndex() != kIndex_Invalid));
	}
	//
	if( mFolder.IsSpecified() == true )
	{
		AText	path;
		//B0389 OK mFolder.GetPath(path,kFilePathType_1);
		GetApp().GetAppPref().GetFilePathForDisplay(mFolder,path);//B0389
		NVI_SetControlText(kFolderPath,path);
	}
	*/
}

//#232
/**
アイコンリスト表示を更新
*/
/*#724
void	AWindow_AddToolButton::UpdateIconList()
{
	ABool	b1 = NVI_GetControlBool(kRadioUser);
	AItemCount	count = GetApp().SPI_GetToolbarIconArray().GetItemCount();
	for( AControlID i = kIcon0; i <= kIcon29; i++ )
	{
		AIndex	iconIndex = mCurrentIconIndexOffset + i-kIcon0;
		if( iconIndex < count )
		{
			AIconID	iconID = GetApp().SPI_GetToolbarIconArray().Get(iconIndex);
			//win NVI_SetControlIcon(i,iconID);
			NVI_GetButtonViewByControlID(i).SPI_SetIcon(iconID,4,4);//win
			NVI_SetControlEnable(i,b1);
		}
		else
		{
			//win NVI_SetControlIcon(i,GetApp().SPI_GetToolbarIconArray().Get(0));
			NVI_GetButtonViewByControlID(i).SPI_SetIcon(kIconID_NoIcon,4,4);//win
			NVI_SetControlEnable(i,false);
		}
	}
}
*/

