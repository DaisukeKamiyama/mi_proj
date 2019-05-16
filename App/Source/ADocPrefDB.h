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

ADocPrefDB

*/

#pragma once

#include "../../AbsFramework/Frame.h"

//#902 class APrintPagePrefData;

class ADocPrefDB : public ADataBase
{
  public:
	ADocPrefDB();
	
	void	InitPref( const AModeIndex inModeIndex );
	AModeIndex	LoadPref( const AText& inTextFilePath, ABool& outLoadedFromFile );//#1429
	//#902 AModeIndex	LoadPref( const AText& inTextFilePath, const ABool inLoadPrintPref, APrintPagePrefData& outPrintPref );
	void	SavePref( const AText& inTextFilePath );
	//#902 void	SavePref( const AText& inTextFilePath, const ABool inSavePrintPref, APrintPagePrefData& inPrintPref );
	//#898 void	LoadForRemoteFile( const AText& inFileURL );//#241
	//#898 void	SaveForRemoteFile( const AText& inFileURL );//#241
	//#902 void	LoadMacLegacy( const AFileAcc& inFile );//#902, APrintPagePrefData& outPrintPref );
	//#902 void	WriteMacLegacy( const AFileAcc& inFile );//#902 , APrintPagePrefData& inPrintPref );
	void	RemoveMacLegacyResouces( const AFileAcc& inFile );
	void	LoadFromDocPrefFile( const AFileAcc& inDocPrefFile, const AText& inDocPath );
	void	SetTextEncoding( const AText& inText );//#1040
	
	//#898
	/** doc pref fileからデータを読み込んだかどうかのフラグ */
  public:
	ABool	IsTextEncodingFixed() const { return mIsTextEncodingFixed; }
	void	SetTextEncodingFixed( const ABool inFixed ) { mIsTextEncodingFixed = inFixed; }
  private:
	ABool	mIsTextEncodingFixed;
	
  public:
	const static APrefID	kModeName						= 101;
	const static APrefID	kTextFontSize					= 103;
	const static APrefID	kCaretMode						= 104;
	const static APrefID	kCaretPointX					= 105;
	const static APrefID	kCaretPointY					= 106;
	const static APrefID	kSelectPointX					= 107;
	const static APrefID	kSelectPointY					= 108;
	const static APrefID	kReturnCode						= 109;
	const static APrefID	kWrapMode						= 112;
	const static APrefID	kWrapMode_LetterCount			= 114;
	const static APrefID	kWindowWidth					= 130;
	const static APrefID	kWindowHeight					= 131;
	const static APrefID	kWindowPosition					= 132;
	const static APrefID	kTextEncodingName				= 134;
	const static APrefID	kTextFontName					= 137;//win
	const static APrefID	kFolding_CollapsedLineIndex		= 149;//#450
	const static APrefID	kLineCheckedDate_LineIndex		= 150;//#842
	const static APrefID	kLineCheckedDate_Date			= 151;//#842
	const static APrefID	kUseModeFont					= 152;//#868
	const static APrefID	kDocumentFlag					= 153;//#832
	const static APrefID	kTextEncodingNameCocoa			= 154;//#1040
	
	/*
	//#844 #688 #898
	//バージョン3.0でobsoleteにした設定
  private:
	//
	const static APrefID	kTabWidth						= 115;
	const static APrefID	kAntiAliasMode					= 133;
	const static APrefID	kIndentWidth					= 136;
	const static APrefID	kDisplaySubPane					= 147;//#668
	const static APrefID	kSubPaneWidth					= 148;//#668
	const static APrefID	kFilePath						= 100;//#241
	const static APrefID	kTextFont_compat				= 102;
	const static APrefID	kOldLCWrapLetterCount			= 113;
	const static APrefID	kOldWrapMode					= 110;
	const static APrefID	kOldWrapModeLCWrap				= 111;
	//#688 const static APrefID	kPrintOption_PrintFont_compat	= 122;//win
	const static APrefID	kPrintOption_PrintFontSize		= 123;
	//#688 const static APrefID	kPrintOption_PageNumberFont_compat= 124;//win
	const static APrefID	kPrintOption_PageNumberFontSize	= 125;
	//#688 const static APrefID	kPrintOption_FileNameFont_compat= 126;//win
	const static APrefID	kPrintOption_FileNameFontSize	= 127;
	//#688 const static APrefID	kPrintOption_LineNumberFont_compat= 128;//win
	const static APrefID	kPrintOption_LineMargin			= 129;
	const static APrefID	kPrintOption_UsePrintFont		= 116;
	const static APrefID	kPrintOption_PrintPageNumber	= 117;
	const static APrefID	kPrintOption_PrintFileName		= 118;
	const static APrefID	kPrintOption_PrintLineNumber	= 119;
	const static APrefID	kPrintOption_PrintSeparateLine	= 120;
	const static APrefID	kPrintOption_ForceWordWrap		= 121;
	const static APrefID	kPrintOption_OldWrapMode		= 135;
	const static APrefID	kPrintOption_PrintFontName		= 138;//win
	const static APrefID	kPrintOption_PageNumberFontName	= 139;//win
	const static APrefID	kPrintOption_FileNameFontName	= 140;//win
	const static APrefID	kPrintOption_LineNumberFontName	= 141;//win
	const static APrefID	kTextFontNameOld					= 142;//win
	const static APrefID	kPrintOption_PrintFontNameOld		= 143;//win
	const static APrefID	kPrintOption_PageNumberFontNameOld	= 144;//win
	const static APrefID	kPrintOption_FileNameFontNameOld	= 145;//win
	const static APrefID	kPrintOption_LineNumberFontNameOld	= 146;//win
	
	*/
};

