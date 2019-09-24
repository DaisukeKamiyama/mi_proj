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

AApplication
アプリケーション

*/

#include "stdafx.h"

#include "../Frame.h"
//#1159 #include <curl/curl.h>

//デバッグモード
ABool	AApplication::sEnableAPICBTraceMode = false;
ABool	AApplication::sEnableDebugTraceMode = false;

//アプリケーションオブジェクトへのポインタ
AApplication*	AApplication::sApplication = NULL;

#pragma mark ===========================
#pragma mark [クラス]AApplication
#pragma mark ===========================
//アプリケーション

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AApplication::AApplication() : AObjectArrayItem(NULL), mEnableDebugMenu(false), mEnableTraceLog(false)
		,mIsActive(true)//B0442
		,mWindowRotating(false), mRotateActiveWindowID(kObjectID_Invalid)//#175
		,mModalAlertWindowID(kObjectID_Invalid)//#291
		,mModalCancelAlertWindowID(kObjectID_Invalid)//#1217
,mDocumentArray(NULL)//#693,true)//#417
,mWindowArray(NULL)//#693 ,true)//#417
,mViewArray(NULL)//#693 ,true)//#417
,mThreadArray(NULL)//#693 ,true)//#417
,mLuaVM(NULL)//#567 #1170
,mToolTipWindowID(kObjectID_Invalid)//#688
,mNowDoingInternalEvent(false)
{
	//cURL初期化 #427
	//#1159 curl_global_init(CURL_GLOBAL_ALL);
	
	sApplication = this;
	//Wrapper Init B0000 高速化
	AFontWrapper::Init();//win ABase()から移動（LocalizedTextを使用するため)Windows用のLocalizedText初期化はCAppImp::Setup()内
	AColorWrapper::Init();
	AScrapWrapper::Init();//win
	
	//AWindow Init
	AWindow::STATIC_Init();//win
	
	//Unicode Data   win 080618
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	AFileAcc	unicodeDataFile, widthFile;
#if IMPLEMENTATION_FOR_MACOSX
	unicodeDataFile.SpecifyChild(appFile,"Contents/Resources/UnicodeData.txt");
	widthFile.SpecifyChild(appFile,"Contents/Resources/EastAsianWidth.txt");//#415
#else
	unicodeDataFile.SpecifySister(appFile,"UnicodeData.txt");
	widthFile.SpecifySister(appFile,"EastAsianWidth.txt");//#415
#endif
	mUnicodeData.Init(unicodeDataFile,widthFile);//#852
	
	//#1012
	AText	name;
	name.SetCstring("icon_CheckBoxOn.png");
	CUserPane::RegisterImageByName(kImageID_iconChecked,name);
	name.SetCstring("icon_CheckBoxOff.png");
	CUserPane::RegisterImageByName(kImageID_iconUnchecked,name);
	
	/*#1012
	//アイコン登録
	AFileAcc	resFolder;
	resFolder.SpecifyChild(appFile,"Contents/Resources");
	AFileAcc	iconfile;
	
	iconfile.SpecifyChild(resFolder,"icon_CheckBoxOn.icns");
	CWindowImp::RegisterIcon(kIconID_CheckBoxOn,iconfile);
	iconfile.SpecifyChild(resFolder,"icon_CheckBoxOff.icns");
	CWindowImp::RegisterIcon(kIconID_CheckBoxOff,iconfile);
	*/
	//iconfile.SpecifyChild(resFolder,"icon_menutriangle.icns");//#232
	//CWindowImp::RegisterIcon(kIconID_Abs_MenuTriangle,iconfile);//#232
	/*
#if IMPLEMENTATION_FOR_MACOSX
	iconfile.SpecifyChild(resFolder,"icon_NoIcon.icns");//B0000 ダミーアイコン（真っ白）
	CWindowImp::RegisterIcon(kIconID_NoIcon,iconfile);//B0000
	iconfile.SpecifyChild(resFolder,"icon_RowMoveUp.icns");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveUp,iconfile);
	iconfile.SpecifyChild(resFolder,"icon_RowMoveDown.icns");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveDown,iconfile);
	iconfile.SpecifyChild(resFolder,"icon_RowMoveTop.icns");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveTop,iconfile);
	iconfile.SpecifyChild(resFolder,"icon_RowMoveBottom.icns");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveBottom,iconfile);
	iconfile.SpecifyChild(resFolder,"doc.icns");//win
	CWindowImp::RegisterIcon(kIconID_Abs_Document,iconfile,"icon_doc");//win
	iconfile.SpecifyChild(resFolder,"folder.icns");//win
	CWindowImp::RegisterIcon(kIconID_Abs_Folder,iconfile,"icon_folder");//win
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	CWindowImp::RegisterIcon(kIconID_NoIcon,L"NoIcon");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveUp,L"abs_rowmoveup");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveDown,L"abs_rowmovedown");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveTop,L"abs_rowmovetop");
	CWindowImp::RegisterIcon(kIconID_Abs_RowMoveBottom,L"abs_rowmovebottom");
	CWindowImp::RegisterIcon(kIconID_Abs_MenuTriangle,L"abs_menutriangle");
	CWindowImp::RegisterIcon(kIconID_Abs_Document,L"abs_document","icon_doc");//win
	CWindowImp::RegisterIcon(kIconID_Abs_Folder,L"abs_folder","icon_folder");//win
#endif
	*/
	NVI_GetMenuManager().RegisterContextMenu(kContextMenuID_ButtonView);//#232 #0
	NVI_GetMenuManager().RegisterContextMenu("AbsFrameworkCommonResource/ContextMenu_EditBox",kContextMenuID_EditBox);//#135
	
	
	//EditBoxViewのデフォルトフォント・サイズを設定 win
	AText	defaultFontName;
	AFontWrapper::GetAppDefaultFontName(defaultFontName);//#375
	AView_EditBox::SPI_SetDefaultFont(defaultFontName,9.0);
	
#if IMPLEMENTATION_FOR_WINDOWS
	//行折り返し計算用Offscreen生成
	CWindowImp::InitLineBreakOffscreen();
#endif
	//#567 Lua VMを開く #1170
    mLuaVM = ::lua_open();
	::luaL_openlibs(mLuaVM);
}
/**
デストラクタ
*/
AApplication::~AApplication()
{
	//#693
	//tempフォルダ全体を削除（ファイル・フォルダすべて）
	AFileAcc	tempFolder;
	AFileWrapper::GetTempFolder(tempFolder);
	tempFolder.DeleteFileOrFolderRecursive();
	
	//
	//#567 Lua VMを閉じる #1170
	::lua_close(mLuaVM);
	
	//sApplication = NULL;
	//cURL終了処理 #427
	//#1159 curl_global_cleanup();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---Run/Quit

//#182
/**
mainからコールされる
*/
int	AApplication::NVI_Run()
{
	_AInfo("AApp::NVI_Run() started.",this);
	
	//アプリ実行状態へ移行（実行状態でないと、画面描画されない）#0 NVIDO_Runの前に実行するように修正 #0
	NVM_SetWorking(true);
	
	if( NVIDO_Run() == true )
	{
		//#291
		AWindowDefaultFactory<AWindow_ModalAlert>	modalAlertWindowFactory;
		mModalAlertWindowID = NVI_CreateWindow(modalAlertWindowFactory);
		
		//モーダルCancelボタン付きAlert #1217
		AWindowDefaultFactory<AWindow_ModalCancelAlert>	modalCancelAlertWindowFactory;
		mModalCancelAlertWindowID = NVI_CreateWindow(modalCancelAlertWindowFactory);
		
		//#688
		//ツールチップウインドウ生成
		AWindowDefaultFactory<AWindow_ToolTip>	tooltipWindowFactory;
		mToolTipWindowID = NVI_CreateWindow(tooltipWindowFactory);
		NVI_GetToolTipWindow().NVI_Create(kObjectID_Invalid);
		
		//win 080728
		NVI_UpdateMenu();
		
		//B0000
		ATrace::StartPostInternalEvent();
		
		_AInfo("Before calling mAppImp.Run().",this);
		
		//B0372
		//SPI_CreateNewWindowAutomatic();//AppleEvent AppOpenを受信時にコールするよう修正（これだと、書類ダブルクリックとともに起動する時に問題）
		
		//Windows: mAppImp.Run()はアプリ終了まで返ってこない
		//Mac OS X: mAppImp.Run()はすぐに終了する（#688によりawakeFromNibからNVI_Run()をコールするようにしたため
		int result = mAppImp.Run();
		_AInfo("After calling mAppImp.Run().",this);
		return result;
	}
	else
	{
		return 0;
	}
}

//#1102
/**
終了を試行
（メニュー等からの終了はこの処理をコールする。）
*/
void	AApplication::NVI_TryQuit()
{
	//OSに対して終了を指示。OSからNVI_Quit()がコールされる。
	GetImp().Quit();
}

//#182
/**
OSの終了前処理（mAppImp.Quit()の後）からコールされる
*/
ABool	AApplication::NVI_Quit(/*#1102 const ABool inContinuing */)//#433
{
	//派生クラスでの終了処理
	if( NVIDO_Quit(/*#1102 inContinuing*/) == true )//#433
	{
		//#291
		NVI_DeleteWindow(mModalAlertWindowID);
		mModalAlertWindowID = kObjectID_Invalid;
		
		//モーダルCancelボタン付きAlert #1217
		NVI_DeleteWindow(mModalCancelAlertWindowID);
		mModalCancelAlertWindowID = kObjectID_Invalid;
		
		//#688
		//ツールチップウインドウ削除
		NVI_DeleteWindow(mToolTipWindowID);
		mToolTipWindowID = kObjectID_Invalid;
		
		//NVIDO_Quit()内で削除されなかったウインドウ全削除 #688 ここで削除しないとAApplicationオブジェクト解放時にviewとの順序不定で削除されてしまう
		while( mWindowArray.GetItemCount() > 0 )
		{
			AWindowID	winID = mWindowArray.GetObject(0).GetObjectID();
			mWindowArray.GetObject(0).NVI_Close();
			NVI_DeleteWindow(winID);
		}
		
		//ドキュメント全削除 #688 AApp::NVIDO_Quit()から移動
		NVI_DeleteAllDocument();
		
		//NVIDO_Quit()内で削除されなかったスレッドを全削除
		while( mThreadArray.GetItemCount() > 0 )
		{
			NVI_DeleteThread(mThreadArray.GetObject(0).GetObjectID());
		}
		
		//Imp側を終了（これにより、本イベントトランザクション終了後、Run()が終了する。）
		//#1102 mAppImp.Quit();
		
		return true;
	}
	else
	{
		return false;
	}
}

#pragma mark ===========================

/**
全ウインドウ、ドキュメントのUpdateを行う
（環境設定の反映等のために用いる）
*/
void	AApplication::NVI_UpdateAll()
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		mWindowArray.GetObject(index).NVI_UpdateProperty();
	}
	/*#182
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		mDocumentArray.GetObject(index).NVI_UpdateProperty();
	}
	*/
	NVIDO_UpdateAll();
}

/*#199
//全てのウインドウを閉じる。
//NVI_TryCloseForQuit()をコールし、１つでもfalseがあれば、その時点で削除処理を終了し、falseを返す
ABool	AApplication::NVI_CloseAllWindow( const ABool inQuit )
{
	while( mWindowArray.GetItemCount() > 0 )
	{
		AWindowID	winID = mWindowArray.GetObject(0).GetObjectID();
		//ウインドウ削除実行（削除不可の場合はfalseで返ってくる）
		if( NVI_GetWindowByID(winID).NVI_TryCloseForAllClose(inQuit) == false )
		{
			return false;
		}
	}
	return true;
}
*/

#pragma mark ===========================

#pragma mark ---メインスレッド制御

/**
NVI_WakeTrigger()で起こされるまでスリープする
（mWakeFlagがtrueになるまでブロック状態にする）
*/
void	AApplication::NVI_Sleep()
{
	AStMutexLocker	mutexlocker(mWakeFlagMutex);
	mWakeFlag = false;
	while( mWakeFlag == false )
	{
		//mWakeFlagConditionにシグナルが送られるまでブロック、かつ、ブロック中はmutexはアンロックされる
		mWakeFlagCondition.WaitForCondition(mWakeFlagMutex);
	}
	mWakeFlag = false;
}

/**
NVI_WakeTrigger()で起こされるまでスリープする
（mWakeFlagがtrueになるまでブロック状態にする）
*/
ABool	AApplication::NVI_SleepWithTimer( const float inTimerSecond )//#1483 ANumber→float
{
	ABool	result = true;
	AStMutexLocker	mutexlocker(mWakeFlagMutex);
	mWakeFlag = false;
	{
		//mWakeFlagConditionにシグナルが送られるまでブロック、かつ、ブロック中はmutexはアンロックされる
		//ただし、タイマー満了したらそこでブロック解除
		result = mWakeFlagCondition.WaitForConditionWithTimer(mWakeFlagMutex,inTimerSecond);
	}
	mWakeFlag = false;
	return result;
}

/**
スリープ状態のスレッドを起こす
*/
void	AApplication::NVI_WakeTrigger()
{
	AStMutexLocker	mutexlocker(mWakeFlagMutex);
	if( mWakeFlag == false )//B0314
	{
		mWakeFlag = true;
		mWakeFlagCondition.SignalWithCondition();
	}
}

#pragma mark ===========================

#pragma mark ---メニュー更新

/**
メニュー更新
*/
void	AApplication::NVI_UpdateMenu()
{
	//#688
	//現在と状態が違う項目のみenable/disable設定するために、バッファーモードを設定する。
	//下のEndEnableMenuItemBufferredMode()でバッファーモードを終了する。（終了時に実際のメニュー項目へのenable/disable設定がなされる）
	AApplication::GetApplication().NVI_GetMenuManager().StartEnableMenuItemBufferredMode();
	
	//まず、全てのメニューをDisableにする
	AApplication::GetApplication().NVI_GetMenuManager().DisableAllMenuBarMenuItems();
	//#688 終了処理中にUpdateMenuを実行しないようにする
	if( NVI_IsWorking() == false )   return;
	
	//#688 EVT_UpdateMenu()を直接実行（以前はMenuUpdateイベントを自身に向かって投げていたが、必要なさそうなのと、イベントすれ違い等の可能性をなくすため、直接実行に変更。）
	//フォーカスウインドウについてEVT_UpdateMenu()を実行
	AWindowID	winID = AApplication::GetApplication().NVI_GetFocusWindowID();
	if( winID != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(winID).EVT_UpdateMenu();
	}
	
	//アプリケーションについてEVT_UpdateMenu()を実行
	AApplication::GetApplication().EVT_UpdateMenu();
	
	//#688
	//Mac(Cocoa)の場合は常にmenuのenable/disableはバッファーモードで動かす。
	//OS側のautomatic validation処理により、AppDelegateのvalidateMenuItem:から、バッファーを読み出して、更新される。
#if !IMPLEMENTATION_FOR_MACOSX
	AApplication::GetApplication().NVI_GetMenuManager().EndEnableMenuItemBufferredMode();
#endif
	
	/*#688 直接実行に変更（menuがenableになるまでタイムラグができると、選択後コピー等のとき、コピーのショートカットが効かないタイミングがあるかもしれない）
#if IMPLEMENTATION_FOR_MACOSX
	//MenuUpdateイベントを自身に向かって投げる（メニューコマンド処理と同じルートを通す）
	HICommand	command;
	command.attributes = NULL;
	command.commandID = kCommandID_UpdateCommandStatus;
	command.menu.menuRef = NULL;
	command.menu.menuItemIndex = 0;
	::ProcessHICommand(&command);
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	::PostMessage(CWindowImp::GetMostFrontWindow(),WM_APP_UPDATEMENU,0,0);
#endif
	*/
}

#pragma mark ===========================

#pragma mark ---イベント処理

//#182
/**
メニュー項目選択時処理
*/
ABool	AApplication::EVT_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	/*#513 やはり派生クラスで処理
	//QuitのみAApplicationで処理する
	switch(inMenuItemID)
	{
	  case kMenuItemID_Quit:
		{
			NVI_Quit();
			return true;
		}
	}
	*/
	//派生クラス内で処理
	return EVTDO_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys);
}

//#182
/**
内部イベント処理

派生クラスでオーバーライドする。
ただ、オーバーライドしなくても、_ACThrow()等のログダンプができるように、デフォルト処理を用意した。
*/
void	AApplication::EVTDO_DoInternalEvent( ABool& outUpdateMenu )
{
	outUpdateMenu = false;
	
	//ここに来た時点でキューにたまっているイベント数だけを処理する。
	//これにより、（内部イベントが存在している限りループさせる場合と違って）
	//マウスイベントや表示更新イベントをOSイベントキューにためたままEVT_DoInternalEvent()内でぐるぐる回り続けることはなく、
	//queueCount分が終了したら、OSへ処理を返すことができる。
	AItemCount	queueCount = ABase::GetMainInternalEventQueueCount();
	
	AInternalEventType	type;
	AObjectID	senderObjectID = kObjectID_Invalid;
	ANumber	num;
	AText	text;
	AObjectIDArray	objectIDArray;
	for( AIndex i = 0; i < queueCount; i++ )
	{
		if( ABase::DequeueFromMainInternalEventQueue(type,senderObjectID,num,text,objectIDArray) == true )
		{
			if( NVI_DoInternalEvent_FrameCommon(type,senderObjectID,num,text,objectIDArray) == false )//win
			switch(type)
			{
			  case kInternalEvent_TraceLog:
				{
					AStCreateCstringFromAText	cstr(text);
					fprintf(stderr,"%s",cstr.GetPtr());
					break;
				}
			}
		}
	}
}

#pragma mark ===========================

#pragma mark ---フレーム共通内部イベント処理
//win

/**
フレーム共通内部イベント処理
*/
ABool	AApplication::NVI_DoInternalEvent_FrameCommon( const AInternalEventType inType, const AObjectID inSenderObjectID, 
		const ANumber inNumber, const AText& inText, const AObjectIDArray& inObjectIDArray )
{
	ABool	result = false;
	switch(inType)
	{
	  case kInternalEvent_Quit:
		{
			NVI_TryQuit();//#1102
			result = true;
			break;
		}
	  default:
		{
			//処理無し
			//派生クラスで処理すること
			break;
		}
	}
	return result;
}

#pragma mark ===========================

#pragma mark ---FileOpenDialog
//#182

/**
FileOpenDialogを開く
*/
void	AApplication::NVI_ShowFileOpenWindow( const ABool inShowInvisibleFile )
{
	mAppImp.ShowFileOpenWindow(inShowInvisibleFile);
}

#pragma mark ===========================

#pragma mark ---ModalAlert
//#291

/**
モーダルな警告ウインドウを表示する
*/
void	AApplication::NVI_ShowModalAlertWindow( const AText& inMessage1, const AText& inMessage2 )
{
	NVI_GetWindowByID(mModalAlertWindowID).NVI_Create(kObjectID_Invalid);
	(dynamic_cast<AWindow_ModalAlert&>(NVI_GetWindowByID(mModalAlertWindowID))).SPNVI_Show(inMessage1,inMessage2);
}

//#1217
/**
モーダルな警告ウインドウを表示する
*/
void	AApplication::NVI_ShowModalAlertWindow( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText )
{
	NVI_GetWindowByID(mModalAlertWindowID).NVI_Create(kObjectID_Invalid);
	(dynamic_cast<AWindow_ModalAlert&>(NVI_GetWindowByID(mModalAlertWindowID))).SPNVI_Show(inMessage1,inMessage2,inOKButtonText);
}

#pragma mark ===========================

#pragma mark ---ModalCancelAlert

//#1217
/**
モーダルな警告ウインドウ（Cancelボタン付き）を表示する
*/
ABool	AApplication::NVI_ShowModalCancelAlertWindow( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText, const AText& inCancelButtonText )
{
	NVI_GetWindowByID(mModalCancelAlertWindowID).NVI_Create(kObjectID_Invalid);
	return (dynamic_cast<AWindow_ModalCancelAlert&>(NVI_GetWindowByID(mModalCancelAlertWindowID))).SPNVI_Show(inMessage1,inMessage2,inOKButtonText,inCancelButtonText);
}

#pragma mark ===========================

#pragma mark ---Tooltip

/**
ツールチップ初どう取得
*/
AWindow_ToolTip&	AApplication::NVI_GetToolTipWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_ToolTip,kWindowType_ToolTip,mToolTipWindowID);
}

/**
ツールチップウインドウ非表示
*/
void	AApplication::NVI_HideToolTip()
{
	if( mToolTipWindowID == kObjectID_Invalid )
	{
		return;
	}
	
	if( NVI_GetToolTipWindow().NVI_IsWindowVisible() == true )
	{
		NVI_GetToolTipWindow().NVI_Hide();
	}
}


#pragma mark ===========================

#pragma mark ---アプリケーション情報取得
//#182

/**
バージョン取得
*/
void	AApplication::NVI_GetVersion( AText& outText ) const
{
	mAppImp.GetVersion(outText);
}

//#1096
/**
ビルド番号取得
*/
void	AApplication::NVI_GetBuildNumber( AText& outText ) const
{
	mAppImp.GetBuildNumber(outText);
}

//#305
/**
現在の言語取得
*/
ALanguage	AApplication::NVI_GetLanguage() const
{
	return mAppImp.GetLanguage();
}

/**
言語毎フォルダ名取得
*/
void	AApplication::NVI_GetLanguageName( AText& outText ) const
{
	switch(NVI_GetLanguage())
	{
	  case kLanguage_Japanese:
		{
			outText.SetCstring("ja");
			break;
		}
	  case kLanguage_English:
	  default:
		{
			outText.SetCstring("en");
			break;
		}
	}
}

//#1316
/**
ダークモードかどうかの判定
*/
ABool	AApplication::NVI_IsDarkMode() const
{
	return GetImpConst().IsDarkMode();
}

//#1316
/**
アピアランスタイプ設定
*/
void	AApplication::NVI_SetAppearanceType( const AAppearanceType inAppearanceType )
{
	GetImp().SetAppearanceType(inAppearanceType);
}

#pragma mark ===========================

#pragma mark <所有クラス(ADocument)生成／削除／取得>

#pragma mark ===========================

#pragma mark ---ドキュメント生成／削除

/**
ドキュメントの生成
@note thread-safe
*/
AObjectID	AApplication::NVI_CreateDocument( ADocumentFactory& inFactory )
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	AIndex	docIndex = mDocumentArray.AddNewObject(inFactory);
	mDocumentUniqIDArray.AddItem();//#693
	return mDocumentArray.GetObject(docIndex).GetObjectID();
}

/**
ドキュメントの削除
@note thread-safe
*/
void	AApplication::NVI_DeleteDocument( const AObjectID inDocID )
{
	//#896
	//ドキュメント削除時処理
	NVI_GetDocumentByID(inDocID).NVI_DocumentWillBeDeleted();
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	AIndex	docIndex = mDocumentArray.GetIndexByID(inDocID);
	mDocumentArray.Delete1Object(docIndex);
	mDocumentUniqIDArray.Delete1Item(docIndex);//#693
}

/**
ドキュメントの全削除
@note thread-safe
*/
void	AApplication::NVI_DeleteAllDocument()
{
	//#896
	//ドキュメント削除時処理
	for( AIndex i = 0; i < mDocumentArray.GetItemCount(); i++ )
	{
		mDocumentArray.GetObject(i).NVI_DocumentWillBeDeleted();
	}
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//ドキュメント全削除
	mDocumentArray.DeleteAll();
	mDocumentUniqIDArray.DeleteAll();
}


#pragma mark ===========================

#pragma mark ---ドキュメント取得

/**
DocumentIDからdocumentオブジェクト取得
@note thread-safe
*/
ADocument&	AApplication::NVI_GetDocumentByID( const ADocumentID inDocID )
{
	/*#272
	return mDocumentArray.GetObject(mDocumentArray.GetIndexByID(inDocID));
	*/
	return mDocumentArray.GetObjectByID(inDocID);
}

/**
DocumentIDからdocumentオブジェクト取得(const)
@note thread-safe
*/
const ADocument&	AApplication::NVI_GetDocumentConstByID( const ADocumentID inDocID ) const
{
	/*#272
	return mDocumentArray.GetObjectConst(mDocumentArray.GetIndexByID(inDocID));
	*/
	return mDocumentArray.GetObjectConstByID(inDocID);
}

/**
指定DocumentTypeのドキュメント数を取得
@note thread-safe
*/
AItemCount	AApplication::NVI_GetDocumentCount( const ADocumentType inDocType ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	AItemCount	result = 0;
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObjectConst(index).NVI_GetDocumentType() == inDocType )
		{
			result++;
		}
	}
	return result;
}

/**
指定DocumentTypeの最初のドキュメントのIDを取得
@note thread-safe
*/
ADocumentID	AApplication::NVI_GetFirstDocumentID( const ADocumentType inDocType ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	return GetFirstDocumentID(inDocType);
}
ADocumentID	AApplication::GetFirstDocumentID( const ADocumentType inDocType ) const
{
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObjectConst(index).NVI_GetDocumentType() == inDocType )
		{
			return mDocumentArray.GetObjectConst(index).GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

/**
指定DocIDの次のドキュメント（DocumentTypeが同じもの）のIDを取得
@note thread-safe
*/
ADocumentID	AApplication::NVI_GetNextDocumentID( const AObjectID inDocID ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	return GetNextDocumentID(inDocID);
}
ADocumentID	AApplication::GetNextDocumentID( const AObjectID inDocID ) const
{
	if( mDocumentArray.GetItemCount() == 0 )   return kObjectID_Invalid;
	AIndex	index = mDocumentArray.GetIndexByID(inDocID);
	if( index == kIndex_Invalid )
	{
		_ACError("document not found",this);
		return kObjectID_Invalid;
	}
	ADocumentType	docType = mDocumentArray.GetObjectConst(index).NVI_GetDocumentType();
	for( index++; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObjectConst(index).NVI_GetDocumentType() == docType )
		{
			return mDocumentArray.GetObjectConst(index).GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

/*#272
const ADocument&	AApplication::NVI_GetDocumentByIDConst( const ADocumentID inDocID ) const
{
	return mDocumentArray.GetObjectConst(mDocumentArray.GetIndexByID(inDocID));
}
*/

/**
DocumentIDからdocumentタイプを取得
@note thread-safe
*/
ADocumentType	AApplication::NVI_GetDocumentTypeByID( const ADocumentID inDocID ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	/*#272
	return mDocumentArray.GetObjectConst(mDocumentArray.GetIndexByID(inDocID)).NVI_GetDocumentType();
	*/
	return mDocumentArray.GetObjectConstByID(inDocID).NVI_GetDocumentType();
}

/**
タイトルからdocumentを検索
*/
ADocumentID	AApplication::NVI_GetDocumentIDByTitle( const ADocumentType inDocType, const AText& inTitle ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	for( AObjectID docID = GetFirstDocumentID(inDocType); docID != kObjectID_Invalid; docID = GetNextDocumentID(docID) )
	{
		AText	title;
		NVI_GetDocumentConstByID(docID).NVI_GetTitle(title);//#272
		if( inTitle.Compare(title) == true )
		{
			return docID;
		}
	}
	return kObjectID_Invalid;
}

/**
ファイルからdocumentを検索
*/
ADocumentID	AApplication::NVI_GetDocumentIDByFile( const ADocumentType inDocType, const AFileAcc& inFile ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	for( AObjectID docID = GetFirstDocumentID(inDocType); docID != kObjectID_Invalid; docID = GetNextDocumentID(docID) )
	{
		AFileAcc	file;
		if( NVI_GetDocumentConstByID(docID).NVI_GetFile(file) == true )//#272
		{
			if( inFile.Compare(file) == true )
			{
				return docID;
			}
		}
	}
	return kObjectID_Invalid;
}

/**
指定DocumentType内で、ウインドウ順番N番目のドキュメントのIDを取得
*/
ADocumentID	AApplication::NVI_GetNthDocumentID( const ADocumentType inDocType, const ANumber inNumber ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	ANumber	num = 0;
	//最前面のウインドウから順にループ
	for( AWindowRef	windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID != kObjectID_Invalid )
		{
			if( NVI_GetWindowConstByID(winID).NVI_IsDocumentWindow() == false)   continue;//#673 フローティングウインドウ等は対象外にする
			for( AIndex zorderIndex = 0; 
						zorderIndex <  NVI_GetWindowConstByID(winID).NVI_GetSelectableTabCount();//#673NVI_GetTabCount(); 
						zorderIndex++ )
			{
				AIndex	tabIndex = NVI_GetWindowConstByID(winID).NVI_GetTabIndexByZOrderIndex(zorderIndex);
				ADocumentID	docID = NVI_GetWindowConstByID(winID).NVI_GetDocumentIDByTabIndex(tabIndex);
				if( docID == kObjectID_Invalid )   continue;//#92
				if( NVI_GetDocumentConstByID(docID).NVI_GetDocumentType() == inDocType )
				{
					num++;
					if( num == inNumber )
					{
						return docID;
					}
				}
			}
		}
	}
	return kObjectID_Invalid;
}

//#1034
/**
テキストドキュメントをウインドウorder順に取得
（AppleScriptから使用される。）
*/
void	AApplication::NVI_GetOrderedDocumentIDArray( AArray<ADocumentID>& outDocumentIDArray, const ADocumentType inDocType, const ABool inOnlyDocWithDocImp ) const
{
	//結果格納用array初期化
	outDocumentIDArray.DeleteAll();
	//document arrayをロック
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	ANumber	num = 0;
	//最前面のウインドウから順にループ
	for( AWindowRef	windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
		 windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID != kObjectID_Invalid )
		{
			if( NVI_GetWindowConstByID(winID).NVI_IsDocumentWindow() == false)   continue;//#673 フローティングウインドウ等は対象外にする
			for( AIndex zorderIndex = 0; 
				 zorderIndex <  NVI_GetWindowConstByID(winID).NVI_GetSelectableTabCount();//#673NVI_GetTabCount(); 
				 zorderIndex++ )
			{
				AIndex	tabIndex = NVI_GetWindowConstByID(winID).NVI_GetTabIndexByZOrderIndex(zorderIndex);
				ADocumentID	docID = NVI_GetWindowConstByID(winID).NVI_GetDocumentIDByTabIndex(tabIndex);
				if( docID == kObjectID_Invalid )   continue;//#92
				if( NVI_GetDocumentConstByID(docID).NVI_GetDocumentType() == inDocType )
				{
					AObjectID	docImpID = NVI_GetDocumentConstByID(docID).NVI_GetDocImpID();
					if( inOnlyDocWithDocImp == false || docImpID != kObjectID_Invalid )
					{
						outDocumentIDArray.Add(docID);
					}
				}
			}
		}
	}
}

//#1034
/**
テキストドキュメントのウインドウorder内でのindexを返す
（AppleScriptから使用される。）
*/
AIndex	AApplication::NVI_GetOrderIndexOfDocument( const ADocumentID inDocumentID, const ADocumentType inDocType, const ABool inOnlyDocWithDocImp ) const
{
	AArray<ADocumentID>	docIDArray;
	NVI_GetOrderedDocumentIDArray(docIDArray,inDocType,inOnlyDocWithDocImp);
	return docIDArray.Find(inDocumentID);
}

/**
指定DocumentType内で、ウインドウ順番1番目のドキュメントのIDを取得
*/
ADocumentID	AApplication::NVI_GetMostFrontDocumentID( const ADocumentType inDocType ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	//最前面のウインドウから順にループ
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID != kObjectID_Invalid )//#216
		{
			ADocumentID	currentDocID = NVI_GetWindowConstByID(winID).NVI_GetCurrentDocumentID();
			if( currentDocID != kObjectID_Invalid )
			{
				if( NVI_GetDocumentConstByID(currentDocID).NVI_GetDocumentType() == inDocType )
				{
					return currentDocID;
				}
			}
		}
	}
	return kObjectID_Invalid;
}

/**
全ドキュメントから、ウインドウ順番1番目のドキュメントのIDを取得（最前面がドキュメントウインドウでない場合は、kObjectID_Invalidを返す）
*/
AObjectID	AApplication::NVI_GetDocumentIDOfMostFrontWindow() const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	//最前面のウインドウから順にループ
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID != kObjectID_Invalid )//#216
		{
			ADocumentID	currentDocID = NVI_GetWindowConstByID(winID).NVI_GetCurrentDocumentID();
			if( currentDocID != kObjectID_Invalid )
			{
				return currentDocID;
			}
		}
	}
	return kObjectID_Invalid;
}

/**
指定DocumentIDのドキュメントが存在するかどうかを返す
@note thread-safe
*/
ABool	AApplication::NVI_IsDocumentValid( const AObjectID inDocID ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	/*#272
	return (mDocumentArray.GetIndexByID(inDocID)!=kIndex_Invalid);
	*/
	return mDocumentArray.ExistObject(inDocID);
}

//#693
/**
ドキュメントUniqIDを取得
*/
AUniqID	AApplication::NVI_GetDocumentUniqID( const ADocumentID inDocID ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	return mDocumentUniqIDArray.Get(mDocumentArray.GetIndexByID(inDocID));
}

//#693
/**
ドキュメントUniqIDからDocumentID(ObjectID)を取得
*/
ADocumentID	AApplication::NVI_GetDocumentIDByUniqID( const AUniqID inDocUniqID ) const
{
	//#890
	AStMutexLocker	locker(mDocumentArrayMutex);
	//
	AIndex	index = mDocumentUniqIDArray.Find(inDocUniqID);
	if( index == kIndex_Invalid )   return kObjectID_Invalid;
	return mDocumentArray.GetObjectConst(index).GetObjectID();
}

#pragma mark ===========================

#pragma mark <所有クラス(AWindow)生成／削除／取得>

#pragma mark ===========================

#pragma mark ---動的生成ウインドウ生成／削除

/**
動的生成ウインドウの生成
*/
AObjectID	AApplication::NVI_CreateWindow( AWindowFactory& inFactory )
{
	AIndex	winIndex = mWindowArray.AddNewObject(inFactory);
	return mWindowArray.GetObject(winIndex).GetObjectID();
}

/**
動的生成ウインドウの削除
*/
void	AApplication::NVI_DeleteWindow( const AWindowID inWindowID )
{
	AIndex	winIndex = mWindowArray.GetIndexByID(inWindowID);
	mWindowArray.Delete1Object(winIndex);
}

#pragma mark ===========================

#pragma mark ---動的・固定生成ウインドウ取得

/**
WindowIDからウインドウ取得
*/
AWindow&	AApplication::NVI_GetWindowByID( const AWindowID inWindowID )
{
	/*#272
	AIndex	winIndex = mWindowArray.GetIndexByID(inWindowID);
	return mWindowArray.GetObject(winIndex);
	*/
	return mWindowArray.GetObjectByID(inWindowID);
}

/**
WindowIDからウインドウ取得
*/
const AWindow&	AApplication::NVI_GetWindowConstByID( const AWindowID inWindowID ) const
{
	/*#272
	AIndex	winIndex = mWindowArray.GetIndexByID(inWindowID);
	return mWindowArray.GetObjectConst(winIndex);
	*/
	return mWindowArray.GetObjectConstByID(inWindowID);
}

/**
指定WindowTypeのドキュメント数を取得
*/
AItemCount	AApplication::NVI_GetWindowCount( const AWindowType inWinType ) const
{
	AItemCount	result = 0;
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 高速化
	for( AIndex index = 0; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		if( mWindowArray.GetObjectConst(index).NVI_GetWindowType() == inWinType || inWinType == kWindowType_Invalid )
		{
			result++;
		}
	}
	return result;
}

/**
指定WindowType内で、ウインドウ順番N番目のウインドウのIDを取得
*/
AWindowID	AApplication::NVI_GetNthWindowID( const AWindowType inWinType, const ANumber inNumber ) const
{
	//#654 高速化 NVI_GetWindowIDByWindowRef()での各WindowオブジェクトからのWindowRef検索が遅い（ループ内ループ）ので改善
	
	//条件該当するWindowのWindowRef, WindowIDを配列に格納する
	AHashArray<AWindowRef>	winRefArray;
	AArray<AWindowID>	winIDArray;
	AItemCount	windowCount = mWindowArray.GetItemCount();
	for( AIndex i = 0; i < windowCount; i++ )
	{
		const AWindow&	window = mWindowArray.GetObjectConst(i);
		if( window.NVI_IsWindowVisible() == false )   continue;//#212 非表示ウインドウはカウントしない
		if( window.NVI_GetOverlayMode() == true )   continue;//#212 Overlayウインドウはカウントしない
		if( window.NVI_GetWindowType() == inWinType || inWinType == kWindowType_Invalid )
		{
			winRefArray.Add(window.NVI_GetWindowRef());
			winIDArray.Add(window.GetObjectID());
		}
	}
	//条件該当ウインドウなければここでリターン
	if( winRefArray.GetItemCount() == 0 )   return kObjectID_Invalid;
	
	//最前面のウインドウから順にループ
	ANumber	num = 0;
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		//上で作成した配列から検索
		AIndex	index = winRefArray.Find(windowRef);
		if( index != kIndex_Invalid )
		{
			//WindowRefが一致したらカウンタインクリメントし、引数Nthに一致したら、そのWindowIDを返す
			num++;
			if( num == inNumber )
			{
				return winIDArray.Get(index);
			}
		}
	}
	return kObjectID_Invalid;
	
	/*#654
	ANumber	num = 0;
	//最前面のウインドウから順にループ
	for( AWindowRef	windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID != kObjectID_Invalid )
		{
			if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == false )   continue;//#212 非表示ウインドウはカウントしない
			if( NVI_GetWindowConstByID(winID).NVI_GetOverlayMode() == true )   continue;//#212 Overlayウインドウはカウントしない
			if( NVI_GetWindowConstByID(winID).NVI_GetWindowType() == inWinType || inWinType == kWindowType_Invalid )
			{
				num++;
				if( num == inNumber )
				{
					return winID;
				}
			}
		}
	}
	return kObjectID_Invalid;
	*/
}

//#1034
/**
ウインドウをウインドウorder順に取得
（AppleScriptから使用される。）
*/
void	AApplication::NVI_GetOrderedWindowIDArray( AArray<AWindowID>& outArray, const AWindowType inWindowType )
{
	//結果格納用array初期化
	outArray.DeleteAll();
	//条件該当するWindowのWindowRef, WindowIDを配列に格納する
	AHashArray<AWindowRef>	winRefArray;
	AArray<AWindowID>	winIDArray;
	AItemCount	windowCount = mWindowArray.GetItemCount();
	for( AIndex i = 0; i < windowCount; i++ )
	{
		const AWindow&	window = mWindowArray.GetObjectConst(i);
		if( window.NVI_IsWindowVisible() == false )   continue;//#212 非表示ウインドウはカウントしない
		if( window.NVI_GetOverlayMode() == true )   continue;//#212 Overlayウインドウはカウントしない
		if( window.NVI_GetWindowType() == inWindowType || inWindowType == kWindowType_Invalid )
		{
			winRefArray.Add(window.NVI_GetWindowRef());
			winIDArray.Add(window.GetObjectID());
		}
	}
	//条件該当ウインドウなければここでリターン
	if( winRefArray.GetItemCount() == 0 )   return;
	
	//最前面のウインドウから順にループ
	ANumber	num = 0;
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
		 windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		//上で作成した配列から検索
		AIndex	index = winRefArray.Find(windowRef);
		if( index != kIndex_Invalid )
		{
			outArray.Add(winIDArray.Get(index));
		}
	}
	//派生クラスでウインドウ追加
	NVIDO_GetOrderedWindowIDArray(outArray,inWindowType);
}

/**
指定WindowType内で、ウインドウ順番1番目のドキュメントのIDを取得
*/
AWindowID	AApplication::NVI_GetMostFrontWindowID( const AWindowType inWinType, const ABool inIncludeFloating ) const
{
	//#654 高速化 NVI_GetWindowIDByWindowRef()での各WindowオブジェクトからのWindowRef検索が遅い（ループ内ループ）ので改善
	
	//条件該当するWindowのWindowRef, WindowIDを配列に格納する
	AHashArray<AWindowRef>	winRefArray;
	AArray<AWindowID>	winIDArray;
	AItemCount	windowCount = mWindowArray.GetItemCount();
	for( AIndex i = 0; i < windowCount; i++ )
	{
		const AWindow&	window = mWindowArray.GetObjectConst(i);
		if( window.NVI_IsWindowVisible() == false )   continue;//#212 非表示ウインドウはカウントしない
		if( window.NVI_GetOverlayMode() == true )   continue;//#212 Overlayウインドウはカウントしない
		//引数inIncludeFloatingがfalseの場合は、フローティングウインドウは対象ウインドウとしてカウントしない #959
		if( inIncludeFloating == false )
		{
			if( window.NVI_IsFloating() == true )   continue;
		}
		//
		if( window.NVI_GetWindowType() == inWinType || inWinType == kWindowType_Invalid )//#688
		{
			winRefArray.Add(window.NVI_GetWindowRef());
			winIDArray.Add(window.GetObjectID());
		}
	}
	//条件該当ウインドウなければここでリターン
	if( winRefArray.GetItemCount() == 0 )   return kObjectID_Invalid;
	//最前面のウインドウから順にループ
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		//上で作成した配列から検索
		AIndex	index = winRefArray.Find(windowRef);
		if( index != kIndex_Invalid )
		{
			//WindowRefが一致したら、そのWindowIDを返す
			return winIDArray.Get(index);
		}
	}
	return kObjectID_Invalid;
	
	/*#654
	//最前面のウインドウから順にループ
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID == kObjectID_Invalid )   continue;
		if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == false )   continue;//#212 非表示ウインドウはカウントしない
		if( NVI_GetWindowConstByID(winID).NVI_GetOverlayMode() == true )   continue;//#212 Overlayウインドウはカウントしない
		if( NVI_GetWindowConstByID(winID).NVI_GetWindowType() == inWinType )
		{
			return winID;
		}
	}
	return kObjectID_Invalid;
	*/
}

//#567
/**
指定WindowType内で、ウインドウ順番1番目のドキュメントのIDを取得（除外ウインドウ指定有り）
*/
AWindowID	AApplication::NVI_GetMostFrontWindowID( const AWindowType inWinType, 
			const AWindowID inExceptWindowID ) const
{
	//#654 高速化 NVI_GetWindowIDByWindowRef()での各WindowオブジェクトからのWindowRef検索が遅い（ループ内ループ）ので改善
	
	//条件該当するWindowのWindowRef, WindowIDを配列に格納する
	AHashArray<AWindowRef>	winRefArray;
	AArray<AWindowID>	winIDArray;
	AItemCount	windowCount = mWindowArray.GetItemCount();
	for( AIndex i = 0; i < windowCount; i++ )
	{
		const AWindow&	window = mWindowArray.GetObjectConst(i);
		if( window.NVI_IsWindowVisible() == false )   continue;//#212 非表示ウインドウはカウントしない
		if( window.NVI_GetOverlayMode() == true )   continue;//#212 Overlayウインドウはカウントしない
		if( window.GetObjectID() == inExceptWindowID )   continue;
		if( window.NVI_GetWindowType() == inWinType || inWinType == kWindowType_Invalid )//#688
		{
			winRefArray.Add(window.NVI_GetWindowRef());
			winIDArray.Add(window.GetObjectID());
		}
	}
	//条件該当ウインドウなければここでリターン
	if( winRefArray.GetItemCount() == 0 )   return kObjectID_Invalid;
	//最前面のウインドウから順にループ
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		//上で作成した配列から検索
		AIndex	index = winRefArray.Find(windowRef);
		if( index != kIndex_Invalid )
		{
			//WindowRefが一致したら、そのWindowIDを返す
			return winIDArray.Get(index);
		}
	}
	return kObjectID_Invalid;
	
	/*#654
	//最前面のウインドウから順にループ
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; 
				windowRef = CWindowImp::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID == kObjectID_Invalid )   continue;
		if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == false )   continue;//#212 非表示ウインドウはカウントしない
		if( NVI_GetWindowConstByID(winID).NVI_GetOverlayMode() == true )   continue;//#212 Overlayウインドウはカウントしない
		if( winID == inExceptWindowID )   continue;
		if( NVI_GetWindowConstByID(winID).NVI_GetWindowType() == inWinType )
		{
			return winID;
		}
	}
	return kObjectID_Invalid;
	*/
}

/**
AWindowRefから動的生成ウインドウのObjectIDを検索
*/
AWindowID	AApplication::NVI_GetWindowIDByWindowRef( const AWindowRef inWindowRef ) const
{
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 高速化
	for( AIndex index = 0; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		const AWindow&	window = mWindowArray.GetObjectConst(index);//#654
		if( /*#654 mWindowArray.GetObjectConst(index)*/window.NVI_IsWindowCreated() == false )   continue;//#199
		if( /*#654 mWindowArray.GetObjectConst(index)*/window.NVI_GetWindowRef() == inWindowRef )
		{
			return /*#654 mWindowArray.GetObjectConst(index)*/window.GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

/**
指定WindowTypeの最初のウインドウのIDを取得
*/
AWindowID	AApplication::NVI_GetFirstWindowID( const AWindowType inWinType ) const
{
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 高速化
	for( AIndex index = 0; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		if( mWindowArray.GetObjectConst(index).NVI_GetWindowType() == inWinType )
		{
			return mWindowArray.GetObjectConst(index).GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

/**
指定WindowIDの次のドキュメント（WindowTypeが同じもの）のIDを取得
*/
AWindowID	AApplication::NVI_GetNextWindowID( const AWindowID inWinType ) const
{
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 高速化
	if( /*#654 mWindowArray.GetItemCount()*/windowCount == 0 )   return kObjectID_Invalid;
	AIndex	index = mWindowArray.GetIndexByID(inWinType);
	if( index == kIndex_Invalid )
	{
		_ACError("window not found",this);
		return kObjectID_Invalid;
	}
	AWindowType	winType = mWindowArray.GetObjectConst(index).NVI_GetWindowType();
	for( index++; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		if( mWindowArray.GetObjectConst(index).NVI_GetWindowType() == winType )
		{
			return mWindowArray.GetObjectConst(index).GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

/**
全てのウインドウをRefreshする
*/
void	AApplication::NVI_RefreshAllWindow()
{
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 高速化
	for( AIndex index = 0; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		mWindowArray.GetObject(index).NVI_RefreshWindow();
	}
}

/**
全てのウインドウをupdate propertyする
*/
void	AApplication::NVI_UpdatePropertyAllWindow()
{
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 高速化
	for( AIndex index = 0; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		mWindowArray.GetObject(index).NVI_UpdateProperty();
	}
}

//#379
/**
各ウインドウのTitleバー更新
*/
void	AApplication::NVI_UpdateTitleBar()
{
	AItemCount	windowCount = mWindowArray.GetItemCount();//#654 高速化
	for( AIndex index = 0; index < windowCount/*#654 mWindowArray.GetItemCount()*/; index++ )
	{
		mWindowArray.GetObject(index).NVI_UpdateTitleBar();
	}
}

//#688
/**
現在のフォーカスウインドウ（key window）を検索
*/
AWindowID	AApplication::NVI_GetFocusWindowID() const
{
	AItemCount	windowCount = mWindowArray.GetItemCount();
	for( AIndex index = 0; index < windowCount; index++ )
	{
		const AWindow&	window = mWindowArray.GetObjectConst(index);
		if( window.NVI_IsWindowCreated() == false )   continue;
		if( window.NVI_IsWindowFocusActive() == true )
		{
			return window.GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

//#1075
/**
ViewIDのViewが存在しているかどうかを返す
*/
ABool	AApplication::NVI_ExistWindow( const AViewID inWindowID ) const
{
	return mWindowArray.ExistObject(inWindowID);
}

#pragma mark ===========================

#pragma mark ---ウインドウ巡回

//#175
/**
巡回対象ウインドウに追加
*/
void	AApplication::NVI_AddToWindowRotateArray( const AWindowID inWindowID )
{
	mWindowRotateArray.Insert1(0,inWindowID);
}

//#175
/**
巡回対象ウインドウから削除
*/
void	AApplication::NVI_DeleteFromWindowRotateArray( const AWindowID inWindowID )
{
	AIndex	index = mWindowRotateArray.Find(inWindowID);
	if( index != kIndex_Invalid )
	{
		mWindowRotateArray.Delete1(index);
	}
	//#196 (#175バグ修正) マルチ検索結果ウインドウだけを表示した状態でコマンド+^でthrow発生を修正
	//mRotateActiveWindowIDが削除対象ウインドウなら、mRotateActiveWindowIDはInvalidにする
	if( mRotateActiveWindowID == inWindowID )
	{
		mRotateActiveWindowID = kObjectID_Invalid;
	}
}

//#175
/**
Activateされたらコールされる
巡回対象ウインドウなら、ウインドウ巡回順を更新する
*/
void	AApplication::NVI_RotateWindowActivated( const AWindowID inWindowID )
{
	if( mWindowRotating == true )   return;
	
	AIndex	index = mWindowRotateArray.Find(inWindowID);
	if( index != kIndex_Invalid )
	{
		mWindowRotateArray.Delete1(index);
		mWindowRotateArray.Insert1(0,inWindowID);
		mRotateActiveWindowID = inWindowID;
	}
}

//#175
/**
ウインドウ巡回
*/
void	AApplication::NVI_RotateWindow( const ABool inNext )
{
	if( mWindowRotateArray.GetItemCount() <= 1 )   return;
	
	if( inNext == true )
	{
		//タブ巡回
		if( mRotateActiveWindowID != kObjectID_Invalid )
		{
			/*#559
			AItemCount	tabCount = NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetTabCount();
			AIndex	tabIndex = NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetCurrentTabIndex();
			if( tabIndex != kIndex_Invalid && tabIndex+1 < tabCount )
			{
				//現在のタブが最後のタブ以外なら、次のタブに切り替えてモジュール終了
				//#379 選択可能なタブを検索
				tabIndex++;
				while(tabIndex<tabCount)
				{
					if( NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetTabSelectable(tabIndex) == true )   break;
					tabIndex++;
				}
				if( tabIndex < tabCount )
				{
					//
					NVI_GetWindowByID(mRotateActiveWindowID).NVI_SelectTab(tabIndex);
					return;
				}
			}
			*/
			AIndex	tabIndex = NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetCurrentTabIndex();
			if( tabIndex != kIndex_Invalid )
			{
				if( NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetNextDisplayTab(tabIndex,false) == true )
				{
					NVI_GetWindowByID(mRotateActiveWindowID).NVI_SelectTab(tabIndex);
					return;
				}
			}
		}
		
		//巡回処理中フラグOn
		mWindowRotating = true;
		//
		AIndex	startindex = mWindowRotateArray.Find(mRotateActiveWindowID);
		if( startindex != kIndex_Invalid )
		{
			AIndex	index = startindex;
			//次のウインドウへ
			index++;
			if( index >= mWindowRotateArray.GetItemCount() )
			{
				index = 0;
			}
			//mWindowRotateArrayのなかで、表示中のウインドウをサーチしていき、見つかったらそれを選択
			for( ; index != startindex; )
			{
				//
				AWindowID	winID = mWindowRotateArray.Get(index);
				if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true && NVI_GetWindowByID(winID).NVI_IsWindowCollapsed() == false)
				{
					NVI_GetWindowByID(winID).NVI_SelectWindow();
					NVI_GetWindowByID(winID).NVI_SelectTab(NVI_GetWindowByID(winID).NVI_GetFirstDisplayTab());//#559 0);
					mRotateActiveWindowID = winID;
					break;
				}
				//次のウインドウへ
				index++;
				if( index >= mWindowRotateArray.GetItemCount() )
				{
					index = 0;
				}
			}
			//#196 (#175バグ修正)
			//表示中のウインドウが他に無かった場合、現在のウインドウの最初のタブに切り替える
			if( index == startindex )
			{
				NVI_GetWindowByID(mRotateActiveWindowID).NVI_SelectTab(//#559 0);
							NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetFirstDisplayTab());
			}
		}
		//巡回処理中フラグOff
		mWindowRotating = false;
	}
	else//#552
	{
		//タブ巡回
		if( mRotateActiveWindowID != kObjectID_Invalid )
		{
			AIndex	tabIndex = NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetCurrentTabIndex();
			if( tabIndex != kIndex_Invalid )
			{
				if( NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetPrevDisplayTab(tabIndex,false) == true )
				{
					NVI_GetWindowByID(mRotateActiveWindowID).NVI_SelectTab(tabIndex);
					return;
				}
			}
		}
		
		//巡回処理中フラグOn
		mWindowRotating = true;
		//
		AIndex	startindex = mWindowRotateArray.Find(mRotateActiveWindowID);
		if( startindex != kIndex_Invalid )
		{
			AIndex	index = startindex;
			//前のウインドウへ
			index--;
			if( index < 0 )
			{
				index = mWindowRotateArray.GetItemCount()-1;
			}
			//mWindowRotateArrayのなかで、表示中のウインドウをサーチしていき、見つかったらそれを選択
			for( ; index != startindex; )
			{
				//
				AWindowID	winID = mWindowRotateArray.Get(index);
				if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true && NVI_GetWindowByID(winID).NVI_IsWindowCollapsed() == false)
				{
					NVI_GetWindowByID(winID).NVI_SelectWindow();
					NVI_GetWindowByID(winID).NVI_SelectTab(NVI_GetWindowByID(winID).NVI_GetLastDisplayTab());
					mRotateActiveWindowID = winID;
					break;
				}
				//前のウインドウへ
				index--;
				if( index < 0 )
				{
					index = mWindowRotateArray.GetItemCount()-1;
				}
			}
			//#196 (#175バグ修正)
			//表示中のウインドウが他に無かった場合、現在のウインドウの最後のタブに切り替える
			if( index == startindex )
			{
				NVI_GetWindowByID(mRotateActiveWindowID).NVI_SelectTab(
							NVI_GetWindowByID(mRotateActiveWindowID).NVI_GetLastDisplayTab());
			}
		}
		//巡回処理中フラグOff
		mWindowRotating = false;
	}
}

#pragma mark ===========================

#pragma mark --- AllClose
//#199

/**
AllClose対象ウインドウを追加
*/
void	AApplication::NVI_AddToAllCloseTargetWindowArray( const AWindowID inWindowID )
{
	mAllCloseTargetWindowArray.Add(inWindowID);
}

/**
AllClose対象ウインドウを削除
*/
void	AApplication::NVI_DeleteFromAllCloseTargetWindowArray( const AWindowID inWindowID )
{
	AIndex	index = mAllCloseTargetWindowArray.Find(inWindowID);
	if( index == kIndex_Invalid )   return;
	
	mAllCloseTargetWindowArray.Delete1(index);
}

/**
全てのウインドウを閉じる。
まず、全てのウインドウについて、dirtyなウインドウ／タブについて、保存／保存しない／キャンセル確認ダイアログを表示し、キャンセルされたら「１つも閉じずに」終了
*/
ABool	AApplication::NVI_CloseAllWindow()
{
	//最初にwinArrayへ対象ウインドウの配列をコピー
	AArray<AWindowID>	winArray;
	for( AIndex i = 0; i < mAllCloseTargetWindowArray.GetItemCount(); i++ )
	{
		winArray.Add(mAllCloseTargetWindowArray.Get(i));
	}
	//コピーした配列について、各ウインドウについて、保存／保存しない／キャンセル確認ダイアログを表示する
	for( AIndex i = 0; i < winArray.GetItemCount(); i++ )
	{
		//保存／保存しない／キャンセル確認ダイアログを表示
		//ダイアログはモーダル
		//保存の場合：保存実行。返り値はtrue
		//保存しない場合：何もしない。返り値はtrue
		//キャンセルの場合：何もしない。返り値はfalse
		if( NVI_GetWindowByID(winArray.Get(i)).NVI_AskSaveForAllTabs() == false )
		{
			//キャンセルされたらそこで終了
			return false;
		}
	}
	//保存／保存しない／キャンセル確認ダイアログでキャンセルがなければ、ウインドウを全て強制クローズ
	for( AIndex i = 0; i < winArray.GetItemCount(); i++ )
	{
		//ウインドウ削除実行（強制クローズ）
		NVI_GetWindowByID(winArray.Get(i)).NVI_ExecuteClose();
	}
	return true;
}

#pragma mark ===========================

#pragma mark ---タイマー処理
//#138

/**
タイマー処理対象ウインドウを追加
*/
void	AApplication::NVI_AddToTimerActionWindowArray( const AWindowID inWindowID )
{
	mTimerActionWindowArray.Add(inWindowID);
}

/**
タイマー処理対象ウインドウを削除
*/
void	AApplication::NVI_DeleteFromTimerActionWindowArray( const AWindowID inWindowID )
{
	AIndex	index = mTimerActionWindowArray.Find(inWindowID);
	if( index == kIndex_Invalid )   return;
	
	mTimerActionWindowArray.Delete1(index);
}

/**
タイマー処理
*/
void	AApplication::EVT_DoTickTimerAction()
{
	for( AIndex i = 0; i < mTimerActionWindowArray.GetItemCount(); i++ )
	{
		AWindow&	win = NVI_GetWindowByID(mTimerActionWindowArray.Get(i));
		if( win.NVI_IsWindowCreated() == true )//#402
		{
			if( win.NVI_IsWindowActive() == true || win.NVI_IsFloating() == true )
			{
				win.EVT_DoTickTimer();
			}
		}
	}
	EVTDO_DoTickTimerAction();
}

#pragma mark ===========================

#pragma mark <所有クラス(AView)生成／削除／取得>
//#199
#pragma mark ===========================

#pragma mark ---ビュー生成／削除

/**
動的生成ウインドウの生成
*/
AViewID	AApplication::NVI_CreateView( AViewFactory& inFactory )
{
	AIndex	viewIndex = mViewArray.AddNewObject(inFactory);
	mViewUniqIDArray.AddItem();//#693
	return mViewArray.GetObject(viewIndex).GetObjectID();
}

/**
動的生成ウインドウの削除
*/
void	AApplication::NVI_DeleteView( const AViewID inViewID )
{
	AIndex	viewIndex = mViewArray.GetIndexByID(inViewID);
	mViewArray.Delete1Object(viewIndex);
	mViewUniqIDArray.Delete1Item(viewIndex);//#693
}

#pragma mark ===========================

#pragma mark ---ビュー取得

/**
ViewIDからウインドウ取得
*/
AView&	AApplication::NVI_GetViewByID( const AViewID inViewID )
{
	/*#272
	AIndex	viewIndex = mViewArray.GetIndexByID(inViewID);
	return mViewArray.GetObject(viewIndex);
	*/
	return mViewArray.GetObjectByID(inViewID);
}

/**
ViewIDからウインドウ取得
*/
const AView&	AApplication::NVI_GetViewConstByID( const AViewID inViewID ) const
{
	/*#272
	AIndex	viewIndex = mViewArray.GetIndexByID(inViewID);
	return mViewArray.GetObjectConst(viewIndex);
	*/
	return mViewArray.GetObjectConstByID(inViewID);
}

/**
ViewIDのViewが存在しているかどうかを返す
*/
ABool	AApplication::NVI_ExistView( const AViewID inViewID ) const
{
	/*#272
	return (mViewArray.GetIndexByID(inViewID)!=kIndex_Invalid);
	*/
	return mViewArray.ExistObject(inViewID);
}

//#693
/**
ビューUniqID取得
*/
AUniqID	AApplication::NVI_GetViewUniqID( const AViewID inViewID ) const
{
	return mViewUniqIDArray.Get(mViewArray.GetIndexByID(inViewID));
}

//#693
/**
ビューUniqIDからViewID(ObjectID)を取得
*/
AViewID	AApplication::NVI_GetViewIDByUniqID( const AUniqID inDocUniqID ) const
{
	AIndex	index = mViewUniqIDArray.Find(inDocUniqID);
	if( index == kIndex_Invalid )   return kObjectID_Invalid;
	return mViewArray.GetObjectConst(index).GetObjectID();
}

#pragma mark ===========================

#pragma mark <所有クラス(AThread)生成／削除／取得>

#pragma mark ===========================

#pragma mark ---スレッド生成／削除

/**
スレッド生成
*/
AObjectID	AApplication::NVI_CreateThread( AThreadFactory& inFactory )
{
	AIndex	index = mThreadArray.AddNewObject(inFactory);
	return mThreadArray.GetObject(index).GetObjectID();
}

//#402
/**
スレッド削除
*/
void	AApplication::NVI_DeleteThread( const AObjectID inThreadID, const ABool inWaitAbortThread )
{
	if( inWaitAbortThread == true )
	{
		//スレッド停止
		mThreadArray.GetObjectByID(inThreadID).NVI_WaitAbortThread();
	}
	//スレッドオブジェクト削除
	AIndex	index = mThreadArray.GetIndexByID(inThreadID);
	mThreadArray.Delete1Object(index);
}

/**
スレッド取得
*/
AThread&	AApplication::NVI_GetThreadByID( const AObjectID inThreadID )
{
	/*#272
	AIndex	index = mThreadArray.GetIndexByID(inThreadID);
	return mThreadArray.GetObject(index);
	*/
	return mThreadArray.GetObjectByID(inThreadID);
}
/**
スレッド取得(const)
*/
const AThread&	AApplication::NVI_GetThreadByIDConst( const AObjectID inThreadID ) const
{
	/*#272
	AIndex	index = mThreadArray.GetIndexByID(inThreadID);
	return mThreadArray.GetObjectConst(index);
	*/
	return mThreadArray.GetObjectConstByID(inThreadID);
}

/**
スレッド起動
*/
void	AApplication::NVI_RunThread( const AObjectID inThreadID )
{
	NVI_GetThreadByID(inThreadID).NVI_Run();
}

//#699
/*
AObjectID	AApplication::NVI_GetCurrentThreadObjectID() const
{
	AItemCount	itemCount = mThreadArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		if( mThreadArray.GetObjectConst(i).NVI_GetThraedID() == AThreadWrapper::GetCurrentThreadID() )
		{
			return mThreadArray.GetObjectConst(i).GetObjectID();
		}
	}
	return kObjectID_Invalid;
}
*/
#pragma mark ===========================

#pragma mark ---Speech
//#851

/**
テキスト読み上げ開始
*/
void	AApplication::NVI_SpeakText( const AText& inText )
{
	mAppImp.SpeakText(inText);
}

/**
テキスト読み上げ停止
*/
void	AApplication::NVI_StopSpeaking()
{
	mAppImp.StopSpeaking();
}

/**
テキスト読み上げ中かどうかを返す
*/
ABool	AApplication::NVI_IsSpeaking() const
{
	return mAppImp.IsSpeaking();
}

#pragma mark ===========================

#pragma mark ---Lua
//#567

/**
Luaファイル実行
*/
int	AApplication::NVI_Lua_dofile( const AFileAcc& inFile, AText& outErrorText )
{
	AText	path;
	inFile.GetPath(path);
	AStCreateCstringFromAText	pathcstr(path);
	int	result = luaL_dofile(mLuaVM,pathcstr.GetPtr());
	if( result != 0 )
	{
		Lua_PopText(mLuaVM,outErrorText);
	}
	return result;
}

/**
Luaテキスト実行
*/
int	AApplication::NVI_Lua_dostring( const AText& inString, AText& outErrorText )
{
	AStCreateCstringFromAText	cstr(inString);
	int	result = luaL_dostring(mLuaVM,cstr.GetPtr());
	if( result != 0 )
	{
		Lua_PopText(mLuaVM,outErrorText);
	}
	return result;
}

/**
Luaエラー処理（longjmp発生）
*/
void	AApplication::Lua_Error( lua_State* L, const char* inStringPtr )
{
	luaL_error(L,inStringPtr);
}

/**
Luaドキュメント存在チェック
*/
void	AApplication::Lua_CheckExistDocument( lua_State* L, const ADocumentID inDocID, const ADocumentType inType ) const
{
	if( NVI_IsDocumentValid(inDocID) == false )
	{
		Lua_Error(L,"(AB001) bad object id (document)");
	}
	if( NVI_GetDocumentTypeByID(inDocID) != inType )
	{
		Lua_Error(L,"(AB001) bad object id (document)");
	}
}

/**
Luaビュー存在チェック
*/
void	AApplication::Lua_CheckExistView( lua_State* L, const AViewID inViewID, const AViewType inType ) const
{
	if( NVI_ExistView(inViewID) == false )
	{
		Lua_Error(L,"(AB002) bad object id (view)");
	}
	if( NVI_GetViewConstByID(inViewID).NVI_GetViewType() != inType )
	{
		Lua_Error(L,"(AB002) bad object id (view)");
	}
}

#pragma mark ===========================

#pragma mark ---Luaスタック制御

/**
スタック要素数取得
*/
AItemCount	AApplication::Lua_GetStackCount( lua_State* L )
{
	return lua_gettop(L);
}

/**
スタックを全てクリア
*/
void	AApplication::Lua_ClearStack( lua_State* L )
{
	lua_settop(L,0);
}

#pragma mark ===========================

#pragma mark ---Luaスタック制御（引数チェック・エラー処理有り）

/**
Push(integer)
*/
void	AApplication::Lua_PushInteger( lua_State* L, const lua_Integer inInteger )
{
	lua_pushinteger(L,inInteger);
}

/**
Push(number)
*/
void	AApplication::Lua_PushNumber( lua_State* L, const lua_Number inNumber )
{
	lua_pushnumber(L,inNumber);
}

/**
Push(text)
*/
void	AApplication::Lua_PushText( lua_State* L, const AText& inText )
{
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	luaL_Buffer	b;
	luaL_buffinit(L,&b);
	luaL_addlstring(&b,(const char*)textptr.GetPtr(),textptr.GetByteCount());
	luaL_pushresult(&b);
}

/**
Push(table)
*/
void	AApplication::Lua_PushNewTable( lua_State* L )
{
	lua_newtable(L);
}

/**
テーブルのフィールドに設定(integer)
*/
void	AApplication::Lua_SetTableFieldInteger( lua_State* L, const char* inFieldName, const lua_Integer inValue )
{
	lua_pushinteger(L,inValue);
	lua_setfield(L,-2,inFieldName);
}

/**
Pop(text)
*/
void	AApplication::Lua_PopText( lua_State* L, AText& outText )
{
	size_t	len = 0;
	AConstUCharPtr	textCstr = (AConstUCharPtr)(lua_tolstring(L,-1,&len));
	outText.SetFromTextPtr(textCstr,len);
	lua_pop(L,1);
}

/**
引数取得(integer)
*/
lua_Integer	AApplication::Lua_PopArgumentInteger( lua_State* L )
{
	lua_Integer	result = luaL_checkinteger(L,-1);
	lua_pop(L,1);
	return result;
}

/**
引数取得(text)
*/
void	AApplication::Lua_PopArgumentText( lua_State* L, AText& outText )
{
	size_t	len = 0;
	AConstUCharPtr	textCstr = (AConstUCharPtr)(luaL_checklstring(L,-1,&len));
	outText.SetFromTextPtr(textCstr,len);
	lua_pop(L,1);
}

/**
引数取得(UniqID)
*/
AUniqID	AApplication::Lua_PopArgumentUniqID( lua_State* L )//#693
{
	AUniqID	result = kUniqID_Invalid;
	result.val = Lua_PopArgumentInteger(L);
	if( result.val == -1 )
	{
		result = kUniqID_Invalid;
	}
	return result;
}

/**
引数終了チェック（既にスタックが空であることをチェック）
*/
void	AApplication::Lua_CheckArgumentEnd( lua_State* L )
{
	if( Lua_GetStackCount(L) != 0 )
	{
		luaL_error(L,"too many arguments");
	}
}

#pragma mark ===========================

#pragma mark ---JavaScript
//#904

/**
JavaScript実行
*/
ABool	AApplication::NVI_DoJavaScript( const AText& inScript, const AFileAcc& inFolder )
{
	//JavaScript実行
	mCurrentJavaScriptFolder = inFolder;
	GetImp().DoJavaScript(inScript);
	mCurrentJavaScriptFolder.Unspecify();
	return true;
}

/*NSOperationQueue使用テスト
ABool	AApplication::NVI_IsExecutingJavaScript()
{
	return GetImp().IsExecutingJavaScript();
}

void	AApplication::NVI_CancelExecutingJavaScript()
{
	GetImp().CancelExecutingJavaScript();
}
*/

/**
JavaScriptモジュールロード
*/
void	AApplication::NVI_LoadJavaScriptModule( const AText& inModuleName )
{
	//現在実行中のJavaScriptファイルのフォルダ内の、inModuleName+.jsをファイル名とするファイルを読み込んで実行する
	AText	modulename;
	modulename.SetText(inModuleName);
	AFileAcc	file;
	file.SpecifyChild(mCurrentJavaScriptFolder,modulename);
	if( file.Exist() == false )
	{
		modulename.AddCstring(".js");
		file.SpecifyChild(mCurrentJavaScriptFolder,modulename);
	}
	AText	script;
	file.ReadTo(script);
	GetImp().DoJavaScript(script);
}

#pragma mark ===========================

#pragma mark ---致命的エラー発生時のデータバックアップ
//#182

/**
致命的エラー発生時のデータバックアップ
*/
void	BackupDataForCriticalError()
{
	try
	{
		AApplication::GetApplication().NVI_BackupDataForCriticalError();
	}
	catch(...)
	{
	}
}

#pragma mark ===========================

#pragma mark ---イベントtransaction終了時処理
//#182

/**
イベントtransaction終了時処理
*/
void	EventTransactionPostProcess( const ABool inPeriodical )
{
	try
	{
		AApplication::GetApplication().NVI_EventTransactionPostProcess(inPeriodical);
	}
	catch(...)
	{
	}
}

