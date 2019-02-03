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

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "../Imp.h"
//#include "CAppImp.h"
#include "AbsFrame.h"
#include "AThread.h"
#include "AWindow.h"
#include "ADocument.h"
#include "AMenuManager.h"
#include "AUnicodeData.h"
#include "ATextArrayMenuManager.h"

//#567 Lua #1170
extern "C"{
#include "../../lua/src/lualib.h"
#include "../../lua/src/lauxlib.h"
}

typedef ANumber	ADocumentType;

class CWindowImp;
class AFileOpenCustomData;
class AWindow_ToolTip;

#pragma mark ===========================
#pragma mark [クラス]AApplication
/**
アプリケーションの基底クラス
*/
class AApplication : public AObjectArrayItem
{
	//コンストラクタ、デストラクタ
  public:
	AApplication();
	virtual ~AApplication();
	
	//Imp
  public:
	CAppImp&				GetImp() {return mAppImp;}//#688
	const CAppImp&			GetImpConst() const  {return mAppImp;}//#1316
  private:
	CAppImp								mAppImp;
	
	//<イベント処理>
  public:
	ABool					EVT_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVT_UpdateMenu() { return EVTDO_UpdateMenu(); }
	void					EVT_DoTickTimerAction();//#138 { return EVTDO_DoTickTimerAction(); }
	//#688 ABool					EVT_DoMouseMoved() { return EVTDO_DoMouseMoved(); }
	void					EVT_DoAppActivated() { mIsActive = true; mAppImp.RestartTimer(); EVTDO_DoAppActivated(); }//B0442 #288
	void					EVT_DoAppDeactivated() { mIsActive = false; mAppImp.StopTimer(); EVTDO_DoAppDeactivated(); }//B0442 #288
	void					EVT_OpenUntitledFile() { EVTDO_OpenUntitledFile(); }//#1056
	void					EVT_DoInternalEvent()
	{
		//EVT_DoInternalEvent()の中から再帰的にEVT_DoInternalEvent()をコールするのは禁止する
		if( mNowDoingInternalEvent == true )
		{
			return;
		}
		//EVT_DoInternalEvent()実行中フラグON
		mNowDoingInternalEvent = true;
		//EVTDO_DoInternalEvent()実行
		ABool	updateMenu = false;
		EVTDO_DoInternalEvent(updateMenu); 
		//EVT_DoInternalEvent()実行中フラグOFF
		mNowDoingInternalEvent = false;
		//メニュー更新
		if( updateMenu == true )
		{
			NVI_UpdateMenu();
		}
	}//#698
	void					EVT_WindowActivated() { EVTDO_WindowActivated(); }//#959
  private:
	virtual ABool			EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys ) {return false;}
	virtual void			EVTDO_UpdateMenu() {}
	virtual void			EVTDO_DoTickTimerAction() {}
	//#688 virtual ABool			EVTDO_DoMouseMoved() { return false; }
	virtual void			EVTDO_DoAppActivated() {}
	virtual void			EVTDO_DoAppDeactivated() {}//B0442
	virtual void			EVTDO_OpenUntitledFile() {}//#1056
	virtual void			EVTDO_DoInternalEvent( ABool& outUpdateMenu );//#182 #698
	virtual void			EVTDO_WindowActivated() {}//#959
	ABool								mNowDoingInternalEvent;
	
	//<インターフェイス>
  public:
	int						NVI_Run();//#182
	void					NVI_TryQuit();//#1102
	ABool					NVI_Quit(/*#1102 const ABool inContinuing = false */);//#182 { NVIDO_Quit(); }//win 080702 #433
	void					NVI_OpenFile( const AFileAcc& inFile, const AFileOpenDialogRef inRef ) //R0198
	{ NVIDO_OpenFile(inFile,inRef); }
	void					NVI_UpdateAll();
	//#0 ABool					NVI_DeleteAllDocumentForQuit();
  private:
	virtual void			NVIDO_UpdateAll() {}
	virtual void			NVIDO_OpenFile( const AFileAcc& inFile, const AFileOpenDialogRef inRef ) {}//R0198
	virtual ABool			NVIDO_Run() { return false; }//#182
	virtual ABool			NVIDO_Quit(/*#1102 const ABool inContinuing */) { return true; }//win 080702 #433
#if IMPLEMENTATION_FOR_MACOSX
  public:
	void					NVI_MakeCustomControlInFileOpenWindow( const AFileOpenDialogRef inRef )//R0198
	{ NVIDO_MakeCustomControlInFileOpenWindow(inRef); }
	/*#1034
	void					NVI_GetCustomControlRectInFileOpenWindow( ARect& ioRect )//R0198
	{ NVIDO_GetCustomControlRectInFileOpenWindow(ioRect); }
	*/
  private:
	virtual void			NVIDO_MakeCustomControlInFileOpenWindow( const AFileOpenDialogRef inRef ) {}//R0198
	//#1034 virtual void			NVIDO_GetCustomControlRectInFileOpenWindow( ARect& ioRect ) {}//R0198
#endif
//#263 #if IMPLEMENTATION_FOR_WINDOWS
public:
	ABool					NVI_ExecCommandLineArgs( const ATextArray& inArgs ) {return NVIDO_ExecCommandLineArgs(inArgs);}//#263
private:
	virtual ABool			NVIDO_ExecCommandLineArgs( const ATextArray& inArgs ) {return false;}//#263
//#263 #endif
	//メニュー更新
  public:
	void					NVI_UpdateMenu();//win { NVIDO_UpdateMenu(); }
	
	//フレーム共通内部イベント処理 win
  protected:
	ABool					NVI_DoInternalEvent_FrameCommon( const AInternalEventType inType, const AObjectID inSenderObjectID,
							const ANumber inNumber, const AText& inText, const AObjectIDArray& inObjectIDArray );
	
	//FileOpenDialog #182
  public:
	void					NVI_ShowFileOpenWindow( const ABool inShowInvisibleFile );
	
	//ModalAlert #291
  public:
	void					NVI_ShowModalAlertWindow( const AText& inMessage1, const AText& inMessage2 );
	void					NVI_ShowModalAlertWindow( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText );//#1217
	AWindowID							mModalAlertWindowID;
	
	//ModalCancelAlert #1217
  public:
	ABool					NVI_ShowModalCancelAlertWindow( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText, const AText& inCancelButtonText );
	AWindowID							mModalCancelAlertWindowID;
	
	//Tooltip #688
  public:
	AWindow_ToolTip&		NVI_GetToolTipWindow();
	void					NVI_HideToolTip();
  private:
	AWindowID							mToolTipWindowID;
	
	//アプリケーション情報取得 #182
  public:
	void					NVI_GetVersion( AText& outText ) const;
	void					NVI_GetBuildNumber( AText& outText ) const;//#1096
	ALanguage				NVI_GetLanguage() const;//#305
	void					NVI_GetLanguageName( AText& outText ) const;
	ABool					NVI_IsDarkMode() const;//#1316
	void					NVI_SetAppearanceType( const AAppearanceType inAppearanceType );//#1316
	
	//メインスレッド制御
  public:
	void					NVI_Sleep();
	ABool					NVI_SleepWithTimer( const ANumber inTimerSecond );
	void					NVI_WakeTrigger();
  private:
  private:
	ABool								mWakeFlag;
	AThreadCondition					mWakeFlagCondition;
	AThreadMutex						mWakeFlagMutex;
	
	//アプリがアクティブかどうかB0442
  public:
	ABool					NVI_IsActive() const { return mIsActive; }
  private:
	ABool								mIsActive;
	
	//<所有クラス(AUnicodeData)生成／削除／取得> win 080618
  public:
	AUnicodeData&		NVI_GetUnicodeData() {return mUnicodeData;}
  private:
	AUnicodeData						mUnicodeData;
	
	//<所有クラス(ADocument)生成／削除／取得>
	
	//ドキュメント生成／削除
  public:
	ADocumentID				NVI_CreateDocument( ADocumentFactory& inFactory );
	void					NVI_DeleteDocument( const ADocumentID inDocID );
	void					NVI_DeleteAllDocument();
	//ドキュメント取得
  public:
	//#272 const ADocument&		NVI_GetDocumentByIDConst( const ADocumentID inDocID ) const;
	ADocument&				NVI_GetDocumentByID( const ADocumentID inDocID );
	AItemCount				NVI_GetDocumentCount( const ADocumentType inDocType ) const;
	ADocumentID				NVI_GetFirstDocumentID( const ADocumentType inDocType ) const;
	ADocumentID				NVI_GetNextDocumentID( const ADocumentID inDocID ) const;
	const ADocument&		NVI_GetDocumentConstByID( const ADocumentID inDocID ) const;//#272
	ADocumentType			NVI_GetDocumentTypeByID( const ADocumentID inDocID ) const;
	ADocumentID				NVI_GetDocumentIDByTitle( const ADocumentType inDocType, const AText& inTitle ) const;
	ADocumentID				NVI_GetDocumentIDByFile( const ADocumentType inDocType, const AFileAcc& inFile ) const;
	ADocumentID				NVI_GetNthDocumentID( const ADocumentType inDocType, const ANumber inNumber ) const;
	void					NVI_GetOrderedDocumentIDArray( AArray<ADocumentID>& outDocumentIDArray, const ADocumentType inDocType, const ABool inOnlyDocWithDocImp ) const;//#1034
	AIndex					NVI_GetOrderIndexOfDocument( const ADocumentID inDocumentID, const ADocumentType inDocType, const ABool inOnlyDocWithDocImp ) const;//#1034
	ADocumentID				NVI_GetMostFrontDocumentID( const ADocumentType inDocType ) const;
	ADocumentID				NVI_GetDocumentIDOfMostFrontWindow() const;
	ABool					NVI_IsDocumentValid( const ADocumentID inDocID ) const;
	AUniqID					NVI_GetDocumentUniqID( const ADocumentID inDocID ) const;//#693
	ADocumentID				NVI_GetDocumentIDByUniqID( const AUniqID inDocUniqID ) const;//#693
	ADocument*				NVI_GetDocumentAndRetain( const ADocumentID inDocID ) 
	{ return mDocumentArray.GetObjectAndRetain(inDocID); }//#717
  protected:
	AObjectArrayIDIndexed<ADocument>		mDocumentArray;
	AUniqIDArray							mDocumentUniqIDArray;//#693
	mutable AThreadMutex					mDocumentArrayMutex;//#890
  private:
	ADocumentID				GetFirstDocumentID( const ADocumentType inDocType ) const;//#890
	ADocumentID				GetNextDocumentID( const AObjectID inDocID ) const;//#890
	
	//<所有クラス(AWindow)生成／削除／取得>
	
	//動的・固定生成ウインドウ生成／削除
  public:
	AWindowID				NVI_CreateWindow( AWindowFactory& inFactory );
	void					NVI_DeleteWindow( const AWindowID inWindowID );
	AWindow&				NVI_GetWindowByID( const AWindowID inWindowID );
	const AWindow&			NVI_GetWindowConstByID( const AWindowID inWindowID ) const;
	AItemCount				NVI_GetWindowCount( const AWindowType inWinType ) const;
	AWindowID				NVI_GetNthWindowID( const AWindowType inWinType, const ANumber inNumber ) const;
	void					NVI_GetOrderedWindowIDArray( AArray<AWindowID>& outArray, const AWindowType inWindowType );//#1034
	AWindowID				NVI_GetMostFrontWindowID( const AWindowType inWinType,
													  const ABool inIncludeFloating = true ) const;//#959
	AWindowID				NVI_GetMostFrontWindowID( const AWindowType inWinType, 
							const AWindowID inExceptWindowID ) const;//#567
	AWindowID				NVI_GetWindowIDByWindowRef( const AWindowRef inWindowRef ) const;
	AWindowID				NVI_GetFirstWindowID( const AWindowType inWinType ) const;
	AWindowID				NVI_GetNextWindowID( const AWindowID inWinType ) const;
	void					NVI_RefreshAllWindow();
	void					NVI_UpdatePropertyAllWindow();
	void					NVI_UpdateTitleBar();//#379
	ABool					NVI_ExistWindow( const AWindowID inWindowID )
	{ return mWindowArray.ExistObject(inWindowID); }//#853
	AWindowID				NVI_GetFocusWindowID() const;//#688
	ABool					NVI_ExistWindow( const AViewID inWindowID ) const;//#1075
  protected:
	AObjectArrayIDIndexed<AWindow>			mWindowArray;//#693
  private:
	virtual void			NVIDO_GetOrderedWindowIDArray( AArray<AWindowID>& outArray, const AWindowType inWindowType ) {}//#1034
	
	//ウインドウ巡回 #175
  public:
	void					NVI_AddToWindowRotateArray( const AWindowID inWindowID );
	void					NVI_DeleteFromWindowRotateArray( const AWindowID inWindowID );
	void					NVI_RotateWindowActivated( const AWindowID inWindowID );
	void					NVI_RotateWindow( const ABool inNext = true );//#552
  private:
	AArray<AWindowID>					mWindowRotateArray;
	ABool								mWindowRotating;
	AWindowID							mRotateActiveWindowID;
	
	//AllClose #199
  public:
	ABool					NVI_CloseAllWindow();
	void					NVI_AddToAllCloseTargetWindowArray( const AWindowID inWindowID );
	void					NVI_DeleteFromAllCloseTargetWindowArray( const AWindowID inWindowID );
  private:
	AArray<AWindowID>					mAllCloseTargetWindowArray;
	
	//タイマー処理 #138
  public:
	void					NVI_AddToTimerActionWindowArray( const AWindowID inWindowID );
	void					NVI_DeleteFromTimerActionWindowArray( const AWindowID inWindowID );
  private:
	AArray<AWindowID>					mTimerActionWindowArray;
	
	//<所有クラス(AView)生成／削除／取得>
	//#199
	
	//ビュー生成／削除
	//ビューの削除責任者は（基本的には）ウインドウ
  public:
	AViewID					NVI_CreateView( AViewFactory& inFactory );
	void					NVI_DeleteView( const AViewID inViewID );
	AView&					NVI_GetViewByID( const AViewID inViewID );
	const AView&			NVI_GetViewConstByID( const AViewID inViewID ) const;
	ABool					NVI_ExistView( const AViewID inViewID ) const;
	AUniqID					NVI_GetViewUniqID( const AViewID inViewID ) const;//#693
	AViewID					NVI_GetViewIDByUniqID( const AUniqID inDocUniqID ) const;//#693
  protected:
	AObjectArrayIDIndexed<AView>				mViewArray;//#693
	AUniqIDArray								mViewUniqIDArray;//#693
	
	//<所有クラス(AThread)生成／削除／取得>
  public:
	AObjectID				NVI_CreateThread( AThreadFactory& inFactory );
	void					NVI_DeleteThread( const AObjectID inThreadID, const ABool inWaitAbortThread = true );//#402
	AThread&				NVI_GetThreadByID( const AObjectID inThreadID );
	const AThread&			NVI_GetThreadByIDConst( const AObjectID inThreadID ) const;
	void					NVI_RunThread( const AObjectID inThreadID );
  protected:
	AObjectArrayIDIndexed<AThread>				mThreadArray;//#693
	
	//<所有クラス(AMenuManager)生成／削除／取得>
  public:
	AMenuManager&			NVI_GetMenuManager() { return mMenuManager; }
  private:
	AMenuManager						mMenuManager;
	
	//<所有クラス(ATextArrayMenuManager)生成／削除／取得> #232
  public:
	ATextArrayMenuManager&	NVI_GetTextArrayMenuManager() { return mTextArrayMenuManager; }
  private:
	ATextArrayMenuManager				mTextArrayMenuManager;
	
	//アプリ設定DB取得
  public:
	ADataBase&				NVI_GetAppPrefDB() { return NVIDO_GetAppPrefDB(); }
	const ADataBase&		NVI_GetAppPrefDBConst() const { return NVIDO_GetAppPrefDBConst(); }
  private:
	virtual ADataBase&		NVIDO_GetAppPrefDB() = 0;
	virtual const ADataBase&	NVIDO_GetAppPrefDBConst() const = 0;
	
	/*#688
	//#182
	//ユーティリティ
  public:
	ABool					NVI_ConvertToUTF8CRAnyway( AText& ioText, 
							const ATextEncoding& inPrefTextEncoding, const ATextEncoding& inGuessTextEncoding,
							const ABool inFromExternalData, const ABool inUseFallback, const ABool inUseLosslessFallback,
							const ABool inOnlyPrefTextEncoding, 
							ATextEncoding& outResultEncoding,
							AReturnCode& outResultReturnCode,
							ATextEncodingFallbackType& outResultFallbackType ) const//B0400 #307 #473
	{
		return NVIDO_ConvertToUTF8CRAnyway(ioText,inPrefTextEncoding,inGuessTextEncoding,
		inFromExternalData,inUseFallback,inUseLosslessFallback,inOnlyPrefTextEncoding,
		outResultEncoding,outResultReturnCode,outResultFallbackType);
	}
	ABool					NVI_ConvertToUTF8CRAnyway( AText& ioText, const ATextEncoding& inPreferTextEncoding,
							const ABool inFromExternalData = false ) const//#307
	{
		ATextEncoding	resultTextEncoding;
		AReturnCode	resultReturnCode = returnCode_CR;
		ATextEncodingFallbackType	resultFallbackType = kTextEncodingFallbackType_None;//#473
		return NVI_ConvertToUTF8CRAnyway(ioText,inPreferTextEncoding,inPreferTextEncoding,
		inFromExternalData,true,false,false,resultTextEncoding,resultReturnCode,resultFallbackType);//#473
	}
  private:
	virtual ABool			NVIDO_ConvertToUTF8CRAnyway( AText& ioText, 
							const ATextEncoding& inPrefTextEncoding, const ATextEncoding& inGuessTextEncoding,
							const ABool inFromExternalData, const ABool inUseFallback, const ABool inUseLosslessFallback,
							const ABool inOnlyPrefTextEncoding, 
							ATextEncoding& outResultEncoding,
							AReturnCode& outResultReturnCode,
							ATextEncodingFallbackType& outResultFallbackType ) const {_ACError("",this);return false;}//#307 #473
	*/
	//Applicationオブジェクトの取得
  public:
	static AApplication&	GetApplication()
	{
		if( sApplication != NULL )
		{
			return *sApplication;
		}
		else
		{
			fprintf(stderr,"no application created yet");
			throw 0;
		}
	}
  private:
	static AApplication*	sApplication;
	
	//アプリケーション定常動作中フラグ
  public:
	ABool					NVI_IsWorking() { return mWorking; }
  protected:
	void					NVM_SetWorking( const ABool inWorking ) { mWorking = inWorking; }
  private:
	ABool								mWorking;
	
	//致命的エラー発生時のデータバックアップ #182
  public:
	void					NVI_BackupDataForCriticalError() { NVIDO_BackupDataForCriticalError(); }
  private:
	virtual void			NVIDO_BackupDataForCriticalError() {}
	
	//イベントtransaction終了時処理 #182
  public:
	void					NVI_EventTransactionPostProcess( const ABool inPeriodical ) { NVIDO_EventTransactionPostProcess(inPeriodical); }
  private:
	virtual void			NVIDO_EventTransactionPostProcess( const ABool inPeriodical ) {}
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AApplication"; }
	
	//#1003
	//URIスキーム
  public:
	void					NVI_ExecuteURIScheme( const AText& inURI ) { NVIDO_ExecuteURIScheme(inURI); }
  private:
	virtual void			NVIDO_ExecuteURIScheme( const AText& inURI ) {}
	
	//#851
	//Speech
  public:
	void					NVI_SpeakText( const AText& inText );
	void					NVI_StopSpeaking();
	ABool					NVI_IsSpeaking() const;
	
	//#567 #1170
	
	//Lua
  public:
	lua_State*				NVI_Lua_GetVM() const { return mLuaVM; }
	int						NVI_Lua_dofile( const AFileAcc& inFile, AText& outErrorText );
	int						NVI_Lua_dostring( const AText& inString, AText& outErrorText );
	
  public:
	void					Lua_CheckExistDocument( lua_State* L, const ADocumentID inDocID, const ADocumentType inType ) const;
	void					Lua_CheckExistView( lua_State* L, const AViewID inViewID, const AViewType inType ) const;
	static void				Lua_Error( lua_State* L, const char* inStringPtr );
	
	//Luaスタック制御
  public:
	static AItemCount		Lua_GetStackCount( lua_State* L );
	static void				Lua_ClearStack( lua_State* L );
	//Luaスタック制御（引数チェック・エラー処理有り）
  public:
	static void				Lua_PushInteger( lua_State* L, const lua_Integer inInteger );
	static void				Lua_PushNumber( lua_State* L, const lua_Number inNumber );
	static void				Lua_PushText( lua_State* L, const AText& inText );
	static void				Lua_PushNewTable( lua_State* L );
	static void				Lua_SetTableFieldInteger( lua_State* L, const char* inFieldName, const lua_Integer inValue );
	static void				Lua_PopText( lua_State* L, AText& outText );
	static lua_Integer		Lua_PopArgumentInteger( lua_State* L );
	static void				Lua_PopArgumentText( lua_State* L, AText& outText );
	static AUniqID			Lua_PopArgumentUniqID( lua_State* L );//#693
	static void				Lua_CheckArgumentEnd( lua_State* L );
  private:
	lua_State*						mLuaVM;
	
	//#904
	//JavaScript
  public:
	ABool					NVI_DoJavaScript( const AText& inScript, const AFileAcc& inFolder );
	//ABool					NVI_IsExecutingJavaScript();
	//void					NVI_CancelExecutingJavaScript();
	void					NVI_LoadJavaScriptModule( const AText& inModuleName );
  private:
	AFileAcc							mCurrentJavaScriptFolder;
	
	//デバッグ
	
	//デバッグメニュー表示するかどうか
  public:
	ABool					NVI_GetEnableDebugMenu() const { return mEnableDebugMenu; }
	void					NVI_SetEnableDebugMenu( const ABool inEnableDebugMenu ) { mEnableDebugMenu = inEnableDebugMenu; }
  private:
	ABool							mEnableDebugMenu;
	
	//トレースログウインドウ自動表示するかどうか
  public:
	ABool					NVI_GetEnableTraceLog() const { return mEnableTraceLog; }
	void					NVI_SetEnableTraceLog( const ABool inEnableTraceLog ) { mEnableTraceLog = inEnableTraceLog; }
  private:
	ABool							mEnableTraceLog;
	
	//API callbackトレースを表示するかどうか
  public:
	static ABool			NVI_GetEnableAPICBTraceMode() { return sEnableAPICBTraceMode; }
	static void				NVI_SetEnableAPICBTraceMode( const ABool inEnable ) { sEnableAPICBTraceMode = inEnable; }
  private:
	static ABool					sEnableAPICBTraceMode;
	
	//各種Infoをトレースログ出力するかどうか
  public:
	static ABool			NVI_GetEnableDebugTraceMode() { return sEnableDebugTraceMode; }
	static void				NVI_SetEnableDebugTraceMode( const ABool inEnable ) { sEnableDebugTraceMode = inEnable; }
  private:
	static ABool					sEnableDebugTraceMode;
	
};

extern void	GetApplicationName( AText& outName );

