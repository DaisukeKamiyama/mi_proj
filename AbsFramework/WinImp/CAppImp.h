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

	CAppImp (Windows)

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "CWindowImp.h"

#pragma mark ===========================
#pragma mark [クラス]CAppImp
#pragma mark ===========================

/**
アプリケーション実装クラス
*/
class CAppImp
{
	//コンストラクタ／デストラクタ
  public:
	CAppImp();
	~CAppImp();
	
	//Run/Quit
  public:
	int						Run();
	void					Quit();
	static void				Setup( HINSTANCE hInstance, LPTSTR inCmdLine, int nCmdShow );
	//#263 static void				GetCmdLine( AText& outText );
	static void				GetCmdLineArgTextArray( ATextArray& outTextArray );//#263
	static void				GetCmdLineArgTextArray( const AText& cmdline ,ATextArray& outTextArray );//#263
	static void				DisableNextTranslateMessage() { sDisableNextTranslateMessage = true; }
  private:
	static ABool			sDisableNextTranslateMessage;
	
	//タイマー
  public:
	void					StopTimer();//#288
	void					RestartTimer();//#288
	
	//
  private:
	static VOID CALLBACK	STATIC_APICB_DoTickTimerAction( HWND inHWnd, UINT inMessage, UINT inTimerID, DWORD inTime );
	
public:
	void					ShowFileOpenWindow( const ABool inShowInvisibleFile );
	void					GetVersion( AText& outText ) const;

	//
  public:
	static HINSTANCE		GetHInstance() {return sHInstance;}
	static HWND				GetMessageOnlyWindow() { return sMessageOnlyWindow; }
  private:
	static HINSTANCE					sHInstance;
	static LPTSTR						sCmdLine;
	static int							sCmdShow;
	static HWND							sMessageOnlyWindow;
	
	//Satelite DLL
  public:
	static HINSTANCE		GetHResourceInstance() {return sHResourceInstance;}
  private:
	static HINSTANCE					sHResourceInstance;
	
	//GDI+
  private:
	static ULONG_PTR					sGdiToken;
	
	//LocalizedText
  public:
	static void	GetLocalizedText( const AText& inKey, AText& outText );
	static ABool	ExistLocalizedText( const AText& inKey );
	static void	GetLangText( AText& outText );
  private:
	static void	LoadLocalizedText();
	
	//システムリソース
public:
	static ABool			SystemResourceCheck_GDIObject();
	
	//言語
public:
	static void				SetupLanguageSateliteDLL( const ALanguage inLanguage );
	ALanguage				GetLanguage() const { return sLanguage; }
  private:
	static BOOL CALLBACK	EnumLangProc(HANDLE, LPCTSTR, LPCTSTR,
							WORD wIDLanguage, LONG_PTR lParam);
	static LANGID			GetNTDLLNativeLangID();
	static BOOL 			IsHongKongVersion();
	static LANGID			DetectLanguage();
	static ALanguage					sLanguage;
	
	//#645 アプリケーションバージョン
  private:
	AText								mApplicationVersionText;
	AText								mApplicationBuildNumberText;//#1096
	
  private:
	static AHashTextArray	sLocalizedTextArray_Key;
	static ATextArray	sLocalizedTextArray_Text;
	
};



