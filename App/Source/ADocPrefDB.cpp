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

#include "stdafx.h"

#include "ADocPrefDB.h"
#include "AApp.h"

ADocPrefDB::ADocPrefDB() : ADataBase(NULL), mIsTextEncodingFixed(false)
{
	/*#375
	�f�[�^�ǉ����̃f�[�^�����e�X�g�p�R�[�h
	�V�K�f�[�^�͋��o�[�W�����ł�ADataBase::LoadFromPrefTextFile()�ɂĖ������ēǂށB���̃e�X�g�p�B�����Ńt�@�C���ۑ���A�ēx�R�����g�A�E�g�A�r���h���ēǂށB
	CreateData_Bool(100001,					"100001",				true);
	CreateData_Number(100002,				"100002",				0,				0,kNumber_MaxNumber);
	CreateData_FloatNumber(100003,			"100003",				0,				0,kNumber_MaxNumber);
	CreateData_Point(100004,				"100004",				0,0,			0,0,0,0);
	CreateData_Rect(100005,					"100005",				0,0,0,0,		0,0,0,0);
	CreateData_Color(100006,				"100006",				kColor_Black);
	CreateData_Text(100007,					"100007",				"");
	CreateTableStart();
	CreateData_BoolArray(100008,			"100008",				true);
	CreateData_NumberArray(100009,			"100009",				0,				0,kNumber_MaxNumber,true);
	CreateData_ColorArray(100010,			"100010",				kColor_Black);
	CreateData_TextArray(100011,			"100011",				"");
	CreateTableEnd();
	Add_TextArray(100011,"test");
	*/
	//				�f�[�^ID									�t�@�C���pKey							�����l			(�ŏ�,�ő�)			(�ŏ�,�ő�)
	//#844 CreateData_Text(kFilePath,									"FilePath",							"");//#241
	CreateData_Text(kModeName,									"ModeName",							"");
	
	//#844 CreateData_Text(kTextFontNameOld,							"TextFontName",						"Monaco");//#375 ���o�[�W�����݊����̂���
	CreateData_FloatNumber(kTextFontSize,						"TextFontSize",						9.0,			6.0,256.0);//B0398 �ŏ��l�ύX
	CreateData_Bool(kCaretMode,									"CaretMode",						true);
	CreateData_Number(kCaretPointX,								"CaretPoint X",						0,				0,kNumber_MaxNumber);
	CreateData_Number(kCaretPointY,								"CaretPoint Y",						0,				0,kNumber_MaxNumber);
	CreateData_Number(kSelectPointX,							"SelectPoint X",					0,				0,kNumber_MaxNumber);
	CreateData_Number(kSelectPointY,							"SelectPoint Y",					0,				0,kNumber_MaxNumber);
	CreateData_Number(kReturnCode,								"ReturnCode",						0,				0,5);//#1472 �ő�l2��5
	//#844 CreateData_Number(kOldWrapMode,								"WrapMode",							0,				0,2);//EWrapMode
	//#844 CreateData_Bool(kOldWrapModeLCWrap,							"WrapWithLetterCount",				false);
	CreateData_Number(kWrapMode,								"DocWrapMode",						0,				0,4);//AWrapMode #1113
	//#844 CreateData_Number(kOldLCWrapLetterCount,					"LetterCount",						80,				5,9999);
	CreateData_Number(kWrapMode_LetterCount,					"WrapWithLetterCount LetterCount",	80,				5,9999);
	//#844 CreateData_Number(kTabWidth,								"TabWidth",							4,				1,128);
	//#844 CreateData_Number(kIndentWidth,								"IndentWidth",						4,				1,128);
	
	/*#844
	CreateData_Bool(kPrintOption_UsePrintFont,					"PrintForm_UsePrintFont",			false);
	CreateData_Bool(kPrintOption_PrintPageNumber,				"PrintForm_PrintPageNumber",		true);
	CreateData_Bool(kPrintOption_PrintFileName,					"PrintForm_PrintFileName",			true);
	CreateData_Bool(kPrintOption_PrintLineNumber,				"PrintForm_PrintLineNumber",		true);
	CreateData_Bool(kPrintOption_PrintSeparateLine,				"PrintForm_PrintSeparateLine",		true);
	CreateData_Bool(kPrintOption_ForceWordWrap,					"PrintForm_ForceWordWrap",			true);
	//win CreateData_Font(kPrintOption_PrintFont,						"PrintForm_PrintFont",				AFontWrapper::GetDefaultFont());
	CreateData_Text(kPrintOption_PrintFontNameOld,				"PrintForm_PrintFontName",			"Monaco");//#375 ���o�[�W�����݊����̂���
	CreateData_FloatNumber(kPrintOption_PrintFontSize,			"PrintForm_PrintFontSize",			12.0,			3.0,256.0);
	//win CreateData_Font(kPrintOption_PageNumberFont,				"PrintForm_PageFont",				AFontWrapper::GetDefaultFont());
	CreateData_Text(kPrintOption_PageNumberFontNameOld,			"PrintForm_PageFontName",			"Monaco");//#375 ���o�[�W�����݊����̂���
	CreateData_FloatNumber(kPrintOption_PageNumberFontSize,		"PrintForm_PageFontSize",			12.0,			3.0,256.0);
	//win CreateData_Font(kPrintOption_FileNameFont,					"PrintForm_FileFont",				AFontWrapper::GetDefaultFont());
	CreateData_Text(kPrintOption_FileNameFontNameOld,			"PrintForm_FileFontName",			"Monaco");//#375 ���o�[�W�����݊����̂���
	CreateData_FloatNumber(kPrintOption_FileNameFontSize,		"PrintForm_FileFontSize",			12.0,			3.0,256.0);
	//win CreateData_Font(kPrintOption_LineNumberFont,				"PrintForm_LineFont",				AFontWrapper::GetDefaultFont());
	CreateData_Text(kPrintOption_LineNumberFontNameOld,			"PrintForm_LineFontName",			"Monaco");//#375 ���o�[�W�����݊����̂���
	CreateData_Number(kPrintOption_LineMargin,					"PrintForm_LineMargin",				0,				0,64);
	CreateData_Number(kPrintOption_OldWrapMode,					"PrintForm_WrapMode",				0,				0,2);
	*/
	CreateData_Number(kWindowWidth,								"WindowWidthExclSideBar",			640,			50,5000);
	CreateData_Number(kWindowHeight,							"WindowHeightExclSideBar",			480,			50,5000);
	CreateData_Point(kWindowPosition,							"WindowPositionExclSideBar",		100,100,		-5000,5000,	-5000,5000);//B0376
	//#844 CreateData_Number(kAntiAliasMode,							"AntiAliasMode",					0,				0,2);
	CreateData_Text(kTextEncodingName,							"TextEncoding",						"UTF-8");
	CreateData_Text(kTextEncodingNameCocoa,						"TextEncodingCocoa",				"");//#1040
	
	CreateData_Text(kTextFontName,								"TextFontNameUni",					"default");//#375 defaultfont);
	/*#844
	CreateData_Text(kPrintOption_PrintFontName,					"PrintForm_PrintFontNameUni",		"default");//#375 defaultfont);
	CreateData_Text(kPrintOption_PageNumberFontName,			"PrintForm_PageFontNameUni",		"default");//#375 defaultfont);
	CreateData_Text(kPrintOption_FileNameFontName,				"PrintForm_FileFontNameUni",		"default");//#375 defaultfont);
	CreateData_Text(kPrintOption_LineNumberFontName,			"PrintForm_LineFontNameUni",		"default");//#375 defaultfont);
	
	//#668
	CreateData_Bool(kDisplaySubPane,							"DisplaySubPane",					false);
	CreateData_Number(kSubPaneWidth,							"SubPaneWidth",						350,			100,5000);
	*/
	//#450
	CreateData_NumberArray(kFolding_CollapsedLineIndex,			"Folding_CollapsedLineIndex",		0,				0,kNumber_MaxNumber);
	
	//#842
	CreateData_NumberArray(kLineCheckedDate_LineIndex,			"LineCheckedDate_LineIndex",		0,				0,kNumber_MaxNumber);
	CreateData_NumberArray(kLineCheckedDate_Date,				"LineCheckedDate_Date",				0,				0,kNumber_MaxNumber);
	
	//#868
	CreateData_Bool(kUseModeFont,								"UseModeFont",						true);
	
	//�h�L�������g�t���O
	CreateData_Bool(kDocumentFlag,								"DocumentFlag",						false);
	
	//�E�C���h�E�f�t�H���g�T�C�Y�f�t�H���g�ݒ�
	//#844 �E�C���h�E�f�t�H���g�T�C�Y�����[�h�ݒ肩����ݒ�Ɉړ������B
	SetData_Number(kWindowWidth,GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kDefaultWindowWidth));
	SetData_Number(kWindowHeight,GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kDefaultWindowHeight));
}

/**
���[�h�ɏ]���ăf�[�^������
*/
void	ADocPrefDB::InitPref( const AModeIndex inModeIndex, const ABool inReInit )//#1429
{
	//���[�h��
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(inModeIndex).GetModeRawName(text);
		SetData_Text(ADocPrefDB::kModeName,text);
	}
	//�t�H���g��
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(inModeIndex).GetModeData_Text(AModePrefDB::kDefaultFontName,text);
		SetData_Text(kTextFontName,text);
	}
	//�t�H���g�T�C�Y
	{
		SetData_FloatNumber(kTextFontSize,GetApp().SPI_GetModePrefDB(inModeIndex).GetModeData_FloatNumber(AModePrefDB::kDefaultFontSize));
	}
	//�e�L�X�g�G���R�[�f�B���O
	if( inReInit == false )//#1429
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(inModeIndex).GetModeData_Text(AModePrefDB::kDefaultTextEncoding,text);
		//#1040 SetData_Text(kTextEncodingName,text);
		SetTextEncoding(text);//#1040 
	}
	//�s�܂�Ԃ�
	{
		SetData_Number(kWrapMode,GetApp().SPI_GetModePrefDB(inModeIndex).GetModeData_Number(AModePrefDB::kDefaultWrapMode));
	}
	//�s�܂�Ԃ�������
	{
		SetData_Number(kWrapMode_LetterCount,GetApp().SPI_GetModePrefDB(inModeIndex).GetModeData_Number(AModePrefDB::kDefaultWrapMode_LetterCount));
	}
	//���s�R�[�h
	if( inReInit == false )//#1429
	{
		SetData_Number(kReturnCode,GetApp().SPI_GetModePrefDB(inModeIndex).GetModeData_Number(AModePrefDB::kDefaultReturnCode));
	}
}

//��thread-safe�ɂ���K�v�L��
/**
*/
/*#902
AModeIndex	ADocPrefDB::LoadPref( const AText& inTextFilePath )
{
	APrintPagePrefData	dummy;
	return LoadPref(inTextFilePath,false,dummy);
}
*/
AModeIndex	ADocPrefDB::LoadPref( const AText& inTextFilePath, ABool& outLoadedFromFile )//#902 , const ABool inLoadPrintPref, APrintPagePrefData& outPrintPref ) #1429
{
	outLoadedFromFile = false;//#1429
	AModeIndex	modeIndex = kIndex_Invalid;
	
	//�w��t�@�C���ɑΉ�����doc pref�p�X���擾
	//�i�A�v���P�[�V�����f�[�^�t�H���_�̒��́uDocumentData�v�t�H���_�̉��́A�t�@�C���̃p�X�ɑΉ������t�H���_�̒���docpref.mi��doc pref�t�@�C���Ƃ���B
	//�����o�b�N�A�b�v�ƕ����͓��������A�t�H���_�͕ʂ̃t�H���_�ɂ��Ă���B�j
	AFileAcc	docPrefRootFolder;
	GetApp().SPI_GetDocPrefRootFolder(docPrefRootFolder);
	AFileAcc	docPrefFolder;
	docPrefFolder.SpecifyChild(docPrefRootFolder,inTextFilePath);
	AFileAcc	docPrefFile;
	docPrefFile.SpecifyChild(docPrefFolder,"docpref.mi");
	
	//doc pref�t�@�C�������݂��邩�ǂ����ɏ]���ď�������
	if( docPrefFile.Exist() == false )
	{
		//==================Doc Pref�t�@�C�������݂��Ȃ��ꍇ==================
		
		//�e�L�X�g�t�@�C���̃p�X�ɏ]���ă��[�h����
		modeIndex = GetApp().SPI_FindModeIDByTextFilePath(inTextFilePath);
		if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
		
		//���[�h�ɏ]���ăf�[�^������
		InitPref(modeIndex);
	}
	else
	{
		//==================Doc Pref�t�@�C�������݂���ꍇ==================
		
		//doc pref�t�@�C���ǂݍ���
		LoadFromPrefTextFile(docPrefFile);
		//�e�L�X�g�G���R�[�f�B���O�m��ς݃t���O��ON
		mIsTextEncodingFixed = true;
		//�Ԃ�l�̃��[�h�ς݃t���O��ON
		outLoadedFromFile = true;
		
		//�ǂݍ��񂾃f�[�^����mode���擾
		AText	modename;
		GetData_Text(ADocPrefDB::kModeName,modename);
		modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modename);
		//�ǂݍ��񂾃��[�h���ɑΉ����郂�[�h�̑��݃`�F�b�N�A����сA���[�h��enable���ǂ����̃`�F�b�N
		if( modeIndex == kIndex_Invalid || GetApp().SPI_GetModePrefDB(modeIndex,false).IsModeEnabled() == false )
		{
			//DocPref�̃��[�h������mode���擾�ł��Ȃ��Ƃ��i���������[�h�������݂��Ȃ��Ȃ����j�́A�e�L�X�g�t�@�C���̃p�X�ɏ]���ă��[�h����
			modeIndex = GetApp().SPI_FindModeIDByTextFilePath(inTextFilePath);
			if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
			//���[�h����doc pref�ɐݒ�
			GetApp().SPI_GetModePrefDB(modeIndex).GetModeRawName(modename);
			SetData_Text(ADocPrefDB::kModeName,modename);
		}
		
		/*#902
		//�v�����g�ݒ�ǂݍ���
		if( inLoadPrintPref == true )
		{
#if IMPLEMENTATION_FOR_MACOSX
			//��MacOS���\�[�X�^�C�v�̃f�[�^�ǂݍ���
			//Print data�̂݃��\�[�X����ǂݍ���(#882)
			try
			{
				LoadMacLegacy(docPrefFile,outPrintPref);
			}
			catch(...)
			{
				//�t�@�C���̔j�󓙂ɂ��G���[
				_ACError("ADocPrefDB::LoadMacLegacy() caught exception.",NULL);//#199
			}
#endif
		}
		*/
	}
	
	//2.1�ȑO�̃f�[�^�͓ǂݍ��܂Ȃ��B�idoc pref�ꏊ���ς���Ă��邵�Adoc pref�f�[�^�͂���قǈ����p���d�v�ł͂Ȃ��̂Łj
#if 0
	//#241
	//���̃h�L�������g�̃p�X���擾
	AText	path;
	inFile.GetPath(path);
	//ExternalDocPref����擾
	AFileAcc	docpreffile;
	if( GetApp().SPI_GetExternalDocPrefFile(inFile,docpreffile,false) == true )
	{
		LoadFromDocPrefFile(docpreffile,path);
	}
	
	//PrefFile����f�[�^�ǂݍ���
#if IMPLEMENTATION_FOR_MACOSX
	LoadFromPrefFile(inFile,'MMKE');
	/*#241 ��ֈړ����AMac�̏ꍇ�����s����B���\�[�X�̕�������΂������D�悷��B
#else
	//ExternalDocPref����擾
	AFileAcc	docpreffile;
	if( GetApp().SPI_GetExternalDocPrefFile(inFile,docpreffile,false) == true )
	{
		LoadFromPrefTextFile(docpreffile);
	}
	*/
#endif
	
#if IMPLEMENTATION_FOR_MACOSX
	//���f�[�^����V�f�[�^�ֈڍs
	//�����A�VID�Ńf�[�^�������l�̂܂܁A���A��ID�������l�ł͂Ȃ��Ȃ�A��ID�̃f�[�^��VID�փR�s�[����
	if( IsStillDefault(kWrapMode) == true && IsStillDefault(kOldWrapMode) == false )
	{
		if( GetData_Number(kOldWrapMode) == wrapMode_NoWrap )
		{
			SetData_Number(kWrapMode,static_cast<ANumber>(kWrapMode_NoWrap));
		}
		else
		{
			if( GetData_Bool(kOldWrapModeLCWrap) == true )
			{
				SetData_Number(kWrapMode,static_cast<ANumber>(kWrapMode_WordWrapByLetterCount));
			}
			else
			{
				SetData_Number(kWrapMode,static_cast<ANumber>(kWrapMode_WordWrap));
			}
		}
	}
	if( IsStillDefault(kWrapMode_LetterCount) == true && IsStillDefault(kOldLCWrapLetterCount) == false )
	{
		SetData_Number(kWrapMode_LetterCount,GetData_Number(kOldLCWrapLetterCount)-2);
	}
	if( IsStillDefault(kPrintOption_ForceWordWrap) == true && IsStillDefault(kPrintOption_OldWrapMode) == false )
	{
		SetData_Bool(kPrintOption_ForceWordWrap,(GetData_Number(kPrintOption_OldWrapMode)!=static_cast<ANumber>(wrapMode_NoWrap)));
	}
	//2.1.10�ȑO�̃o�[�W��������̃f�[�^�����p���p
	//#375
	if( IsStillDefault(kTextFontName) == true && IsStillDefault(kTextFontNameOld) == false )
	{
		AText	fontname;
		GetData_Text(kTextFontNameOld,fontname);
		SetData_Text(kTextFontName,fontname);
	}
	/*#844
	//#375
	if( IsStillDefault(kPrintOption_PrintFontName) == true && IsStillDefault(kPrintOption_PrintFontNameOld) == false )
	{
		AText	fontname;
		GetData_Text(kPrintOption_PrintFontNameOld,fontname);
		SetData_Text(kPrintOption_PrintFontName,fontname);
	}
	//#375
	if( IsStillDefault(kPrintOption_PageNumberFontName) == true && IsStillDefault(kPrintOption_PageNumberFontNameOld) == false )
	{
		AText	fontname;
		GetData_Text(kPrintOption_PageNumberFontNameOld,fontname);
		SetData_Text(kPrintOption_PageNumberFontName,fontname);
	}
	//#375
	if( IsStillDefault(kPrintOption_FileNameFontName) == true && IsStillDefault(kPrintOption_FileNameFontNameOld) == false )
	{
		AText	fontname;
		GetData_Text(kPrintOption_FileNameFontNameOld,fontname);
		SetData_Text(kPrintOption_FileNameFontName,fontname);
	}
	//#375
	if( IsStillDefault(kPrintOption_LineNumberFontName) == true && IsStillDefault(kPrintOption_LineNumberFontNameOld) == false )
	{
		AText	fontname;
		GetData_Text(kPrintOption_LineNumberFontNameOld,fontname);
		SetData_Text(kPrintOption_LineNumberFontName,fontname);
	}
	*/
	/*#688
	//2.1.8a6�ȑO�̃o�[�W��������̃f�[�^�����p���p
	//win
	if( IsStillDefault(kTextFontName) == true &&  IsStillDefault(kTextFont_compat) == false )
	{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kTextFont_compat),fontname) == true )
		{
			SetData_Text(kTextFontName,fontname);
		}
	}
	if( IsStillDefault(kPrintOption_PrintFontName) == true && IsStillDefault(kPrintOption_PrintFont_compat) == false )
	{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kPrintOption_PrintFont_compat),fontname) == true )
		{
			SetData_Text(kPrintOption_PrintFontName,fontname);
		}
	}
	if( IsStillDefault(kPrintOption_PageNumberFontName) == true && IsStillDefault(kPrintOption_PageNumberFont_compat) == false )
	{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kPrintOption_PageNumberFont_compat),fontname) == true )
		{
			SetData_Text(kPrintOption_PageNumberFontName,fontname);
		}
	}
	if( IsStillDefault(kPrintOption_FileNameFontName) == true && IsStillDefault(kPrintOption_FileNameFont_compat) == false )
	{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kPrintOption_FileNameFont_compat),fontname) == true )
		{
			SetData_Text(kPrintOption_FileNameFontName,fontname);
		}
	}
	if( IsStillDefault(kPrintOption_LineNumberFontName) == true && IsStillDefault(kPrintOption_LineNumberFont_compat) == false )
	{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kPrintOption_LineNumberFont_compat),fontname) == true )
		{
			SetData_Text(kPrintOption_LineNumberFontName,fontname);
		}
	}
	*/
#endif
#endif
	
	//#521 �g�p�o���Ȃ��t�H���g�̏ꍇ��default�t�H���g�ɕ␳����
	AdjustFontName(kTextFontName);
	
	return modeIndex;
}

/**
doc pref�ۑ�
*/
/*#902
void	ADocPrefDB::SavePref( const AText& inTextFilePath )
{
	APrintPagePrefData	dummy;
	SavePref(inTextFilePath,false,dummy);
}
*/
void	ADocPrefDB::SavePref( const AText& inTextFilePath )//#902, const ABool inSavePrintPref, APrintPagePrefData& inPrintPref )
{
	//#898
	//doc pref�t�@�C���擾
	AFileAcc	docPrefRootFolder;
	GetApp().SPI_GetDocPrefRootFolder(docPrefRootFolder);
	AFileAcc	docPrefFolder;
	docPrefFolder.SpecifyChild(docPrefRootFolder,inTextFilePath);
	docPrefFolder.CreateFolderRecursive();
	AFileAcc	docPrefFile;
	docPrefFile.SpecifyChild(docPrefFolder,"docpref.mi");
	//�t�@�C����������ΐ���
	docPrefFile.CreateFile();
	
	//doc pref�t�@�C����������
	WriteToPrefTextFile(docPrefFile);
	
	/*#902
	//�v�����g�ݒ菑������
	if( inSavePrintPref == true )
	{
#if IMPLEMENTATION_FOR_MACOSX
		try
		{
			WriteMacLegacy(docPrefFile,inPrintPref);
		}
		catch(...)
		{
			//�t�@�C���̔j�󓙂ɂ��G���[
			_ACError("ADocPrefDB::WriteMacLegacy() caught exception.",NULL);//#199
		}
#endif
	}
	*/
#if 0
	/*#688
#if IMPLEMENTATION_FOR_MACOSX
	//win
	//���f�[�^���փR�s�[
	AText	text;
	AFontNumber	font;
	//
	GetData_Text(kTextFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kTextFont_compat,font);
	//
	GetData_Text(kPrintOption_PrintFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kPrintOption_PrintFont_compat,font);
	//
	GetData_Text(kPrintOption_PageNumberFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kPrintOption_PageNumberFont_compat,font);
	//
	GetData_Text(kPrintOption_FileNameFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kPrintOption_FileNameFont_compat,font);
	//
	GetData_Text(kPrintOption_LineNumberFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kPrintOption_LineNumberFont_compat,font);
#endif
	*/
	//�t�@�C���p�X�ۑ� #241
	AText	filepath;
	inFile.GetPath(filepath);
	SetData_Text(kFilePath,filepath);
	
	//
	AText	modename;
	GetData_Text(kModeName,modename);
	AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeName(modename);
	if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
#if IMPLEMENTATION_FOR_MACOSX
//#334	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_Bool(AModePrefDB::kDontSaveResouceFork) == false //R0002
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_Bool(AModePrefDB::kSaveDocumentPrefToResource) == true //#332
				|| inForceAttachResource == true ) //B0406
	{
		WriteToPrefFile(inFile,'MMKE');
		WriteMacLegacy(inFile,inPrintPref);
	}
	else
	{
		RemoveMacLegacyResouces(inFile);
	}
#endif
	//#241 #else
	//#241 Mac�̏ꍇ�����s
	//ExternalDocPref�֕ۑ�
	AFileAcc	docpreffile;
	if( GetApp().SPI_GetExternalDocPrefFile(inFile,docpreffile,true) == true )
	{
		if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_Bool(AModePrefDB::kSaveDocumentPrefToAppPref) == true )//#332
		{
			WriteToPrefTextFile(docpreffile);
		}
		//#332
		else
		{
			docpreffile.WriteFile(GetEmptyText());
		}
	}
	//#241 #endif
#endif
}

#if 0
//#241
/**
DocPrefFile����f�[�^�ǂݍ���
DocPrefFile���̃f�[�^��̃p�X�f�[�^��inPath����v�����ꍇ�̂ݓǂݍ���
@param inDocPrefFile DocPrefFile
@param inDocPath ��v���ׂ��p�X
*/
void	ADocPrefDB::LoadFromDocPrefFile( const AFileAcc& inDocPrefFile, const AText& inDocPath )
{
	//����DocPref�̃f�[�^��tmp�փR�s�[
	ADocPrefDB	tmp;
	tmp.CopyDataFromOtherDB(*this);
	//ExternalDocPref�t�@�C������tmp�ɓǂݏo��
	tmp.LoadFromPrefTextFile(inDocPrefFile);
	//ExternalDocPref�̃p�X���擾
	AText	pathindb;
	tmp.GetData_Text(ADocPrefDB::kFilePath,pathindb);
	//�p�X����v���Ă����炱��DocPref�փf�[�^���R�s�[
	if( pathindb.Compare(inDocPath) == true )
	{
		CopyDataFromOtherDB(tmp);
	}
}

//#241
void	ADocPrefDB::LoadForRemoteFile( const AText& inFileURL )
{
	//ExternalDocPref����擾
	AFileAcc	docpreffile;
	if( GetApp().SPI_GetExternalDocPrefFile(inFileURL,docpreffile,false) == true )
	{
		LoadFromDocPrefFile(docpreffile,inFileURL);
	}
}

//#241
void	ADocPrefDB::SaveForRemoteFile( const AText& inFileURL )
{
	//�t�@�C���p�X�ۑ� #241
	SetData_Text(kFilePath,inFileURL);
	
	//ExternalDocPref�֕ۑ�
	AFileAcc	docpreffile;
	if( GetApp().SPI_GetExternalDocPrefFile(inFileURL,docpreffile,true) == true )
	{
		WriteToPrefTextFile(docpreffile);
	}
	}

//�f�[�^�������l�̂܂܂Ȃ�A���[�h�ݒ�̃f�t�H���g�l��ݒ肷��iAModePrefDB����l���R�s�[����j
void	ADocPrefDB::CopyFromModePrefDBIfStillDefault()
{
	/*#844
	CopyFromModePrefDBIfStillDefault(kPrintOption_UsePrintFont,			AModePrefDB::kDefaultPrintOption_UsePrintFont);
	CopyFromModePrefDBIfStillDefault(kPrintOption_PrintPageNumber,		AModePrefDB::kDefaultPrintOption_PrintPageNumber);
	CopyFromModePrefDBIfStillDefault(kPrintOption_PrintFileName,		AModePrefDB::kDefaultPrintOption_PrintFileName);
	CopyFromModePrefDBIfStillDefault(kPrintOption_PrintLineNumber,		AModePrefDB::kDefaultPrintOption_PrintLineNumber);
	CopyFromModePrefDBIfStillDefault(kPrintOption_PrintSeparateLine,	AModePrefDB::kDefaultPrintOption_PrintSeparateLine);
	CopyFromModePrefDBIfStillDefault(kPrintOption_ForceWordWrap,		AModePrefDB::kDefaultPrintOption_ForceWordWrap);
	*/
	//win CopyFromModePrefDBIfStillDefault(kTextFont,							AModePrefDB::kDefaultFont);
	CopyFromModePrefDBIfStillDefault(kTextFontName,						AModePrefDB::kDefaultFontName);//win
	CopyFromModePrefDBIfStillDefault(kTextFontSize,						AModePrefDB::kDefaultFontSize);
	CopyFromModePrefDBIfStillDefault(kTabWidth,							AModePrefDB::kDefaultTabWidth);
	//#868 CopyFromModePrefDBIfStillDefault(kIndentWidth,						AModePrefDB::kIndentWidth);
	/*#844
	CopyFromModePrefDBIfStillDefault(kWindowWidth,						AModePrefDB::kDefaultWindowWidth);
	CopyFromModePrefDBIfStillDefault(kWindowHeight,						AModePrefDB::kDefaultWindowHeight);
	CopyFromModePrefDBIfStillDefault(kPrintOption_PrintFontName,		AModePrefDB::kDefaultPrintOption_PrintFontName);//win
	CopyFromModePrefDBIfStillDefault(kPrintOption_PrintFontSize,		AModePrefDB::kDefaultPrintOption_PrintFontSize);
	CopyFromModePrefDBIfStillDefault(kPrintOption_PageNumberFontName,	AModePrefDB::kDefaultPrintOption_PageNumberFontName);//win
	CopyFromModePrefDBIfStillDefault(kPrintOption_PageNumberFontSize,	AModePrefDB::kDefaultPrintOption_PageNumberFontSize);
	CopyFromModePrefDBIfStillDefault(kPrintOption_FileNameFontName,		AModePrefDB::kDefaultPrintOption_FileNameFontName);//win
	CopyFromModePrefDBIfStillDefault(kPrintOption_FileNameFontSize,		AModePrefDB::kDefaultPrintOption_FileNameFontSize);
	CopyFromModePrefDBIfStillDefault(kPrintOption_LineNumberFontName,	AModePrefDB::kDefaultPrintOption_LineNumberFontName);//win
	CopyFromModePrefDBIfStillDefault(kPrintOption_LineMargin,			AModePrefDB::kDefaultPrintOption_LineMargin);
	*/
	/*#868
	CopyFromModePrefDBIfStillDefault(kTextEncodingName,					AModePrefDB::kDefaultTextEncoding);
	CopyFromModePrefDBIfStillDefault(kWrapMode,							AModePrefDB::kDefaultWrapMode);
	CopyFromModePrefDBIfStillDefault(kWrapMode_LetterCount,				AModePrefDB::kDefaultWrapMode_LetterCount);
	CopyFromModePrefDBIfStillDefault(kReturnCode,						AModePrefDB::kDefaultReturnCode);
	*/
	//#868
	AText	modename;
	GetData_Text(kModeName,modename);
	AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeName(modename);
	if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
	if( IsStillDefault(kTextEncodingName) == true )
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Text(AModePrefDB::kDefaultTextEncoding,text);
		SetData_Text(kTextEncodingName,text);
	}
	if( IsStillDefault(kWrapMode) == true )
	{
		SetData_Number(kWrapMode,GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Number(AModePrefDB::kDefaultWrapMode));
	}
	if( IsStillDefault(kWrapMode_LetterCount) == true )
	{
		SetData_Number(kWrapMode_LetterCount,GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Number(AModePrefDB::kDefaultWrapMode_LetterCount));
	}
	if( IsStillDefault(kReturnCode) == true )
	{
		SetData_Number(kReturnCode,GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Number(AModePrefDB::kDefaultReturnCode));
	}
}

void	ADocPrefDB::CopyFromModePrefDBIfStillDefault( ADataID inDocPrefID, ADataID inModePrefID )
{
	if( IsStillDefault(inDocPrefID) == false )   return;
	AText	modename;
	GetData_Text(kModeName,modename);
	AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeName(modename);
	if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
	CopyDataFromOtherDB(inDocPrefID,GetApp().SPI_GetModePrefDB(modeIndex),inModePrefID);
}

#endif

//#1040
/**
�e�L�X�g�G���R�[�f�B���O�ݒ�
*/
void	ADocPrefDB::SetTextEncoding( const AText& inTextEncodingName )
{
	//�e�L�X�g�G���R�[�f�B���O���̂̃m�[�}���C�Y #1040
	AText	name(inTextEncodingName);
	GetApp().GetAppPref().ConvertToNormalizedTextEncodingName(name);
	//Cocoa�ŗp�ݒ�ۑ� #1040
	SetData_Text(ADocPrefDB::kTextEncodingNameCocoa,name);
	//Carbon�ŗp�ݒ�ۑ�
	ATextEncodingWrapper::ConvertToCarbonTextEncodingName(name);
	SetData_Text(ADocPrefDB::kTextEncodingName,name);
}



