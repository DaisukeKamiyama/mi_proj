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

CAppImp.h

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "../AbsFrame/AbsFrame.h"
//#include "../Imp.h"

//#1034
typedef NSOpenPanel* AFileOpenDialogRef;

//#1078
#pragma mark ===========================
#pragma mark [クラス]CDocImp
#pragma mark ===========================
/**
 CDocImpクラス（OS APIを直接コールし、OS間の違いを吸収する）
*/
class CDocImp: public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	CDocImp( AObjectArrayItem*	inParent );
	~CDocImp();
	
  public:
	void					SetNSDocument( NSDocument* inNSDocument ) { mNSDocument = inNSDocument; }
	NSDocument*				GetNSDocument() { return mNSDocument; }//#1034
	void					SetDocumentID( const ADocumentID inDocumentID );//#1034
  private:
	NSDocument*							mNSDocument;
};

#pragma mark ===========================
#pragma mark [クラス]CDocImpFactory
/**
CDocImp生成用Factoryクラス
*/
class CDocImpFactory : public AAbstractFactoryForObjectArray<CDocImp>
{
  public:
	CDocImpFactory( AObjectArrayItem*	inParent ) : mParent(inParent)
	{
	}
	CDocImp*	Create()
	{
		return new CDocImp(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};


#pragma mark ===========================
#pragma mark [クラス]CAppImp
#pragma mark ===========================
/**
AppImpクラス（OS APIを直接コールし、OS間の違いを吸収する）
*/
class CAppImp: public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	CAppImp();
	
	//#263
	//アプリ起動時設定
  public:
	static void				Setup( int argc, const char **argv );
	static void				GetCmdLineArgTextArray( ATextArray& outTextArray );
  private:
	static int							sArgc;
	static const char**					sArgv;
	
	//AppleScript
  public:
	static ComponentInstance	GetScriptingComponent() { return sScriptingComponent; }
  private:
	static ComponentInstance	sScriptingComponent;
	
#if SUPPORT_CARBON
	//NibRef
  public:
	static IBNibRef			GetNibRef();
	static IBNibRef			GetMenuNibRef();
	static IBNibRef						sNibRef;
	static IBNibRef						sMenuNibRef;//#0
#endif
	
	//Run/Quit
  public:
	int						Run();
	void					Quit();
	
	//#688
	//標準動作
  public:
	void					HideThisApplication();
	void					HideOtherApplications();
	void					ShowAllApplications();
	void					BringAllToFront();
	
	//#851
	//Speech
  public:
	void					SpeakText( const AText& inText );
	void					StopSpeaking();
	ABool					IsSpeaking() const;
	
	//タイマー
  public:
	void					StopTimer();//#288
	void					RestartTimer();//#288
  private:
	void					StartTimer();//B0000
	
	//イベントハンドラ
  public:
	void					APICB_CocoaDoMenuAction( const AMenuItemID inMenuItemID, const AModifierKeys inModifierKeys,
							const AMenuRef inMenuRef, const AIndex inMenuItemIndex );
	void					APICB_CocoaDoTickTimerAction();
	void					APICB_CocoaDoBecomeActive();
	void					APICB_CocoaDoResignActive();
#if SUPPORT_CARBON
	static pascal OSStatus	STATIC_APICB_DoAppDeactivated( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_DoAppActivated( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_DoCommand( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_DoCommandUpdateStatus( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_DoKeyDown( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_DoTextInput( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_DoMouseMoved( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_DoAppleEvent( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	//static pascal OSErr	STATIC_APICB_DoAEOpenDocument( const AppleEvent* inAppleEvent, AppleEvent* outReply, long inRefcon );
	static pascal OSStatus	STATIC_APICB_DoInternalEvent( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal void		STATIC_APICB_DoTickTimerAction( EventLoopTimerRef inTimer, void *inUserData );
	void					InstallEventHandler();
	EventLoopTimerRef					mTickTimerRef;
#endif
	
	//ファイル選択ウインドウ
  public:
	void					ShowFileOpenWindow( const ABool inShowInvisibleFile );
	//#1034 static pascal void		STATIC_APICB_FileOpenWindow( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD );
  private:
	//#1034 NavDialogRef						mOpenDialog;
	
	//情報取得
  public:
	void					GetVersion( AText& outText ) const;
	void					GetBuildNumber( AText& outText ) const;//#1096
	ALanguage				GetLanguage() const;//#305
  private:
	ALanguage						mLanguage;//#305
	AText							mApplicationVersionText;//#645
	AText                           mApplicationBuildNumberText;//#1096
	
	//context menuのターゲットウインドウ／コントロールを設定
  public:
	static void				SetContextMenuTarget( const AWindowID inWindowID, const AControlID inControlID );
  private:
	static AWindowID				sContextMenuWindowID;
	static AControlID				sContextMenuViewControlID;
	
	/*#1034
	//#852
	//デフォルトATSUStyleを取得
  public:
	static ATSUStyle		GetDefaultATSUStyle();
  private:
	static ATSUStyle				sDefaultATSUStyle;
	*/
	
	//#703
	//Font Fallback
  public:
	static ATSUFontFallbacks	GetATSUFontFallbacks() { return sFontFallbacks; }
  private:
	static ATSUFontFallbacks		sFontFallbacks;
	
	//JavaScript #904
  public:
	void					DoJavaScript( const AText& inScript );
	//ABool					IsExecutingJavaScript();
	//void					CancelExecutingJavaScript();
	
	/*#688
	//B0000 080810
	//Mouse Tracking
  public:
	static void				SetMouseTracking( const AWindowRef inWindowRef, const AControlID inControlID );
	//#310 static void				ClearMouseTracking();
	//#310 static AControlID		GetMouseTrackingControlID( const AWindowRef inWindowRef );
	static void				ProcessMouseExitToAnotherControl( const EventRef inEventRef, const AWindowRef inWindowRef, const AControlID inControlID );
  private:
	static ABool						sMouseTrackingMode;
	static AWindowRef					sMouseTrackingWindowRef;
	static AControlID					sMouseTrackingControlID;
	*/
	
	//#891
	//ドット単位スクロール
  public:
	static ABool			GetUseWheelScrollFloatValue() { return sUseWheelScrollFloatValue; }
	static void				SetUseWheelScrollFloatValue( const ABool inUseWheelScrollFloatValue ) 
	{ sUseWheelScrollFloatValue = inUseWheelScrollFloatValue; }
  private:
	static ABool						sUseWheelScrollFloatValue;
	
	//#1078
	//CDocImp管理
  public:
	AObjectID				CreateDocImp();
	void					DeleteDocImp( const AObjectID inDocImpID );
	CDocImp&				GetDocImpByID( const AObjectID inDocImpID );
  private:
	AObjectArrayIDIndexed<CDocImp>		mDocImpArray;
};

