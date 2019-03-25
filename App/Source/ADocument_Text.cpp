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

ADocument_Text

*/

#include "stdafx.h"

#include "ADocument_Text.h"
#include "ADocument_IndexWindow.h"
#include "AApp.h"
#include "ASyntaxDefinition.h"
#include "AView_Text.h"
//#858 #include "AView_List_FusenList.h"
#include "AView_LineNumber.h"
#include "AThread_SyntaxRecognizer.h"
#include "AWindow_Notification.h"
#include "AView_Notification.h"
#include "AThread_DocumentFileScreening.h"

#pragma mark ===========================
#pragma mark [�N���X]ADocument_Text 
#pragma mark ===========================
//�e�L�X�g�h�L�������g�̃N���X
#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
ADocument_Text::ADocument_Text( AObjectArrayItem* inParent, const AObjectID inDocImpID ) : ADocument(inParent,inDocImpID),//#1034
		/*#379 ���g�p�̂��߃R�����g�A�E�g mInitDone(false),*/ mUndoer(*this), 
		/*#361 mFTPMode(false),*/ mTextInfo(this), mTextInfoForPrint(this), mBookmarkLineIndex(kIndex_Invalid), //#182NVM_GetPrintImp()(GetObjectID()),
#if IMPLEMENTATION_FOR_MACOSX
		/*#1034 mASImp(&(GetApp().SPI_GetASImp()),GetObjectID()),*/ mCreator(NULL), mType(NULL), 
#endif
		/*#698 ���g�p�̂��ߍ폜mSyntaxDirty(false), */mImportFileDataDirty(false),
		mFindHighlightSet(false), 
		//#893 mCurrentLocalIdentifierStartLineIndex(kIndex_Invalid), mCurrentLocalIdentifierEndLineIndex(kIndex_Invalid),
		mDiffDisplayWaitFlag(false), //#379 mDiffDisplayMode(kDiffDisplayMode_None),//R0006
		mDrawArrowToDef(false),
		//win 080729
		mPrintLineNumberWidthMM(14.0)/*40(pix)/72(dpi)*25.4=14.111...*/,
		mPrintLineNumberRightMarginMM(2.5),
		mPrintFileNameHeightMM(10.0),
		mPrintSeparateLineHeightMM(1.0)
		,mProjectObjectID(kObjectID_Invalid)//RC3
		,mFindHighlightSet2(false)//R0244
		,mCreatedAutomatically(false)//B0441
		,mDisableInlineInputHilite(false)//#187
		,mOriginalFontSize(9.0)//#354
		,mLastSavedUndoPosition(kIndex_Invalid)//#267
		,mLastSelectedJumpListItemIndex(kIndex_Invalid)//#454
		,mShouldShowCorrectEncodingWindow(false)//#379
		,mDiffTargetMode(kDiffTargetMode_None)//#379
		,mDiffWorkingDocumentID(kObjectID_Invalid)//#379
		,mDiffTargetFileDocumentID(kObjectID_Invalid)//#379
		,mRepositoryDocumentID(kObjectID_Invalid)//#379
//#361 mTextDocumentType���g��,mIsDiffTargetDocument(false)//#379
		,mPrintStartParagraphIndex(kIndex_Invalid),mPrintEndParagraphIndex(kIndex_Invalid)//#524
//#0 ���g�p�̂��ߍ폜		,mDiffInited(true)//#379
//#699,mViewDataInited(false)//#379
		,mIsOwnedDocument(false)//#379
		,mShouldShowCorrectEncodingWindow_FallbackType(kTextEncodingFallbackType_None)//#473
		,mLuaConsoleMode(false),mLuaPromptTextIndex(kIndex_Invalid)//#567
		,mSCMFileState(kSCMFileState_NotSCMFolder)//#568
,mLastModifiedDateTime(0)//#653
,mSyntaxRecognizer_ThreadRunningFlag(false),mSyntaxRecognizer_ThreadUnpausedFlag(false)//#698
,mSyntaxRecognizer_RestartFlag(false)//#698
,mSyntaxRecognizer_StartLineIndex(kIndex_Invalid),mSyntaxRecognizer_EndLineIndex(kIndex_Invalid)//#698
,mSyntaxRecognizerThreadID(kObjectID_Invalid)//#698
,mWrapCalculator_ThreadUnpausedFlag(false),mWrapCalculator_CurrentLineIndex(kIndex_Invalid)//#699
,mWrapCalculatorThreadID(kObjectID_Invalid)//#699
,mCacheForGetJumpMenuItemIndexByLineIndex(kIndex_Invalid)//#&95
,mDocumentInitState(kDocumentInitState_Initial)//#699
,mWrapCalculatorWorkingByInsertText(false)//#699
,mWrapCalculatorProgressTotalTextLength(kIndex_Invalid),mWrapCalculatorProgressRemainedTextLength(0)//#699
,mRemoteConnectionObjectID(kObjectID_Invalid)//#361
,mTextDocumentType(kTextDocumentType_Normal)//#361
,mTextEncodingIsDirectedByInitArgument(false)//#830
,mLoaded(false)//#831
,mCurrentWordFindHighlightSet(false)//#750
,mSuspectBinaryData(false)//#703
,mAutoSaveDirty(false)
,mDiffTextRangeStartParagraph(0),mDiffTextRangeEndParagraph(0),mDiffTextRangeStart(kIndex_Invalid),mDiffTextRangeEnd(kIndex_Invalid)//#192
,mCompareMode(kCompareMode_NoCompare)//#81
,mPreviewCurrentLineIndex(kIndex_Invalid)//#734
,mGuessedIndnetCharacterType(kIndentCharacterType_Unknown)//#912
,mForMultiFileReplace(false)
,mCalculateWrapInThread(true)
,mFileScreenError(kFileScreenError_NoError)
,mDuplicatedDocumentID(kObjectID_Invalid)
,mTemporaryFontSize(0)//#966
,mPrintRect(kLocalRect_0000),mPrintPaperWidth(0),mPrintPaperHeight(0)//#558
,mIsReadMeFile(false)//#1351
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::ADocument_Text() started.",this);
	
	//������
	//#764 mTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
	
	//mWindow_SimpleCVSCommit.Setup(*this);
	//mWindow_SimpleCVSAdd.Setup(*this);
	//mPropertyWindow.Setup(this);
	mJumpMenuObjectID = GetApp().NVI_GetMenuManager().CreateDynamicMenu(kMenuItemID_Jump,true);
	
#if IMPLEMENTATION_FOR_MACOSX
	mODBMode = false;
#endif
	
	//#81
	//���ۑ��f�[�^�ۑ��t�H���_����
	CreateUnsavedDataBackupFolder();
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::ADocument_Text() ended.",this);
}

//�f�X�g���N�^
ADocument_Text::~ADocument_Text()
{
	//���X���b�h�ł�mText���Q�Ƃ�������ɃI�u�W�F�N�g�폜���邱�Ƃ�ۏ؂��邽�߁B
	//�iADocument_Text��AApp�ɓo�^���ɃX���b�h����Q�Ƃ���AmText���Q�ƒ��ɁAADocument_Text��Delete���ꂽ�ꍇ�̂��߁B���X���b�h�Q�Ƃ��I������܂�Lock()�̓u���b�N����B�j
	mTextMutex.Lock();
	mTextMutex.Unlock();
#if IMPLEMENTATION_FOR_MACOSX
	if( mODBMode == true )
	{
		ALaunchWrapper::SendODBClosed(mODBSender,mODBToken,mODBFile);
	}
#endif
	
	//#361
	//�����[�g�ڑ��I�u�W�F�N�g�폜
	if( mRemoteConnectionObjectID != kObjectID_Invalid )
	{
		GetApp().SPI_DeleteRemoteConnection(mRemoteConnectionObjectID);
		mRemoteConnectionObjectID = kObjectID_Invalid;
	}
	
	//#1229
	//���[�h���A����сA�Ō�̃Z�[�u���̃t�@�C���̃R�s�[��ۑ�����t�@�C�����폜
	AFileAcc	lastLoadOrSaveFile;
	GetLastLoadOrSaveCopyFile(lastLoadOrSaveFile);
	lastLoadOrSaveFile.DeleteFile();
}

//#379
/**
Document�폜������
*/
void	ADocument_Text::NVIDO_DocumentWillBeDeleted()
{
	//#890
	//�C���|�[�g�t�@�C���f�[�^��S�폜
	SPI_DeleteAllImportFileData();
	//#81
	//diff�p�e���|�����t�H���_���폜
	if( mDiffTempFolder.IsSpecified() == true )
	{
		mDiffTempFolder.DeleteFileOrFolderRecursive();
	}
	//#81
	//���ۑ��f�[�^�ۑ��t�H���_�폜
	DeleteUnsavedDataBackupFolder();
	
	//Working����Diff�Ώۃh�L�������g������ꂽ���Ƃ�ʒm
	if( mDiffWorkingDocumentID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextDocumentByID(mDiffWorkingDocumentID).SPI_DoDiffTargetDocumentClosed(GetObjectID());
	}
	//Diff�Ώۑ���Working�h�L�������g������ꂽ���Ƃ�ʒm
	if( mDiffTargetFileDocumentID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextDocumentByID(mDiffTargetFileDocumentID).SPI_DoWorkingDocumentClosed(GetObjectID());
	}
	if( mRepositoryDocumentID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).SPI_DoWorkingDocumentClosed(GetObjectID());
	}
	//���@�F���X���b�h�폜#698
	if( mSyntaxRecognizerThreadID != kObjectID_Invalid )
	{
		//�X���b�h�폜�i�X���b�h�I��������NVI_DeleteThread()����߂��Ă���B�j
		GetApp().NVI_DeleteThread(mSyntaxRecognizerThreadID);
		mSyntaxRecognizerThreadID = kObjectID_Invalid;
	}
	//�s�v�Z�X���b�h�폜#699
	if( mWrapCalculatorThreadID != kObjectID_Invalid )
	{
		//�X���b�h�폜�i�X���b�h�I��������NVI_DeleteThread()����߂��Ă���B�j
		GetApp().NVI_DeleteThread(mWrapCalculatorThreadID);
		mWrapCalculatorThreadID = kObjectID_Invalid;
	}
	//#890
	//�e�L�X�g�S�폜
	{
		AStMutexLocker	locker(mTextMutex);
		mText.DeleteAll();
	}
	//#896
	//�h�L�������g������ꂽ���Ƃ�ʒm�i�v���W�F�N�g�̃h�L�������g��S�ĕ������ɃC���|�[�g���ʎq�f�[�^��purge����Ȃǂ̂��߁j
	if( NVI_IsFileSpecified() == true )
	{
		AFileAcc	file;
		NVI_GetFile(file);
		GetApp().SPI_DoTextDocumentClosed(file,mProjectFolder);
	}
	/*
	//
	AText	t;
	NVI_GetTitle(t);
	t.AddCstring(" closed.\r");
	t.OutputToStderr();
	*/
}

#pragma mark ===========================

#pragma mark ---�����ݒ�
//ADocument_Text������A�ȉ��̂����ꂩ��Setup���R�[�������

//#361
/**
�A�N�Z�X�v���O�C��
@param inDocumentText �e�L�X�g�G���R�[�f�B���O���Ԋ҂̃����[�g�e�L�X�g
*/
void	ADocument_Text::SPI_Init_RemoteFile( const AObjectID inAccessPuginObjectID, 
		const AText& inFileURL, const AText& inDocumentText )
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::SPI_Init_RemoteFile() started.",this);
	
	//�h�L�������g�^�C�v�ݒ�
	mTextDocumentType = kTextDocumentType_RemoteFile;
	
	//�A�N�Z�X�v���O�C���f�[�^�ݒ�
	mRemoteConnectionObjectID = inAccessPuginObjectID;
	mRemoteFileURL.SetText(inFileURL);
	
	//=======================
	// �h�L�������g�ݒ�
	//=======================
	
	//#241 DocPref�ǂݍ��݁imRemoteFileURL�ɏ]�����f�[�^�ۑ��t�H���_����doc pref��ǂ݂��ށj
	//#898 mDocPrefDB.LoadForRemoteFile(inFileURL);
	AModeIndex	modeIndex = LoadDocPref();//#898
	
	//mModeIndex��ݒ�
	SetModeIndex(modeIndex);//#890 #898
	
	/*#898
	//ModeIndex����i�t�@�C�������猈��j
	SetModeIndex(GetApp().SPI_FindModeIDByTextFilePath(inFileURL));//R0210 #890
	AText	text;
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeName(text);
	GetDocPrefDB().SetData_Text(ADocPrefDB::kModeName,text);
	*/
	
	//=======================
	// �e�L�X�g�ݒ�
	//=======================
	//�e�L�X�g�ݒ�
	{
		AStMutexLocker	locker(mTextMutex);//#890
		mText.SetText(inDocumentText);
	}
	//�T�[�o�[��e�L�X�g���L��
	mTextOnRemoteServer.SetText(inDocumentText);
	
	//�����ݒ苤�ʏ���
	Init_Common();
	
	//#81
	//�����o�b�N�A�b�v�t�@�C�����X�g�X�V
	SPI_UpdateAutoBackupArray();
}

/*#361
//FTP�h�L�������gSetup
void	ADocument_Text::SPI_Init_FTP( const AText& inFTPURL, const AText& inDocumentText )
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::SPI_Init_FTP() started.",this);
	//=======================
	// �h�L�������g�ݒ�
	//=======================
	
	//#241 DocPref�ǂݍ���
	mDocPrefDB.LoadForRemoteFile(inFTPURL);
	
	//ModeIndex����i�t�@�C�������猈��j
	mModeIndex = GetApp().SPI_FindModeIDByTextFilePath(inFTPURL);//R0210
	AText	text;
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeName(text);
	GetDocPrefDB().SetData_Text(ADocPrefDB::kModeName,text);
	
	//=======================
	// FTP�f�[�^�ݒ�
	//=======================
	mFTPMode = true;
	mFTPURL.SetText(inFTPURL);
	mFTPTextOnServer.SetText(inDocumentText);
	
	//=======================
	// �e�L�X�g�ݒ�
	//=======================
	mText.SetText(inDocumentText);
	
	Init_Common();
}
*/

//���[�J���t�@�C���h�L�������gSetup
void	ADocument_Text::SPI_Init_LocalFile( const AFileAcc& inFileAcc, const AText& inTextEncodingName )//win 080715 #212
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::SPI_Init_LocalFile() started.",this);
	
	//#361
	//�h�L�������g�^�C�v�ݒ�
	mTextDocumentType = kTextDocumentType_Normal;
	
	//�t�@�C�����݃`�F�b�N
	//#932 �t�@�C�����݃`�F�b�N��SPI_ScreenDocumentFile()�Ŏ��s if( inFileAcc.Exist() == false ) _ACThrow("file not exist",this);
	/*#932 �N���G�[�^�^�C�v�ǂݍ��݂�SPI_ScreenDocumentFile()�Ŏ��s�Bttro�͑Ή����Ȃ��B
#if IMPLEMENTATION_FOR_MACOSX
	//�N���G�[�^�^�^�C�v�ǂݍ���
	inFileAcc.GetCreatorType(mCreator,mType);
	
	if( mType == 'ttro' )
	{
		NVI_SetReadOnly(true);
	}
#endif
	*/
	//�t�@�C���ݒ�i���X�N���[�j���O�Ƃ��Đݒ�j
	//#932 NVI_SpecifyFile(inFileAcc);
	NVI_SpecifyFileUnscreened(inFileAcc);
	
	//=======================
	// �h�L�������g�ݒ�
	//=======================
	
	//DocPref�ǂݍ���
	/*#898
	APrintPagePrefData	printpref;
	mDocPrefDB.Load(inFileAcc,printpref);
	NVM_GetPrintImp().SetPrintPagePrefData(printpref);
	*/
	AModeIndex	modeIndex = LoadDocPref();//#898
	
	//mModeIndex��ݒ�
	SetModeIndex(modeIndex);//#890 #898
	
	//inTextEncodingName���w�肳��Ă���ꍇ�iAppleScript�o�R�ɂ��I�[�v�����j�͂������D�悳����
	if( inTextEncodingName.GetLength() > 0 )
	{
		//DocPref��TextEncoding�ݒ�
		//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,inTextEncodingName);
		GetDocPrefDB().SetTextEncoding(inTextEncodingName);//#1040
		//�e�L�X�g�G���R�[�f�B���O�w��t���OON
		mTextEncodingIsDirectedByInitArgument = true;//#830
	}
	
	Init_Common();//#212
}

//�V�K�t�@�C��Setup
void	ADocument_Text::SPI_Init_NewFile( const AModeIndex inModeIndex, const AText& inTextEncodingName, const AText& inDocumentText )
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::SPI_Init_NewFile() started.",this);
	
	//#361
	//�h�L�������g�^�C�v�ݒ�
	mTextDocumentType = kTextDocumentType_Normal;
	
	//���^�C�g���ݒ�
	AText	title;
	title.SetLocalizedText("UntitledDocumentName");
	ANumber	num = GetApp().SPI_GetAndIncrementUntitledDocumentNumber();
	if( num >= 2 )
	{
		title.Add(kUChar_Space);//B0000 �킩������
		title.AddFormattedCstring("%d",num);
	}
	SPI_SetTemporaryTitle(title);
	
	AText	text;
	
	//=======================
	// �h�L�������g�ݒ�
	//=======================
	
	//mModeIndex�ݒ�
	SetModeIndex(inModeIndex);//#890
	
	//DocPref������ #898
	InitDocPref(inModeIndex);
	
	/*#898
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeName(text);
	GetDocPrefDB().SetData_Text(ADocPrefDB::kModeName,text);
	
	//DocPref��ModePref����R�s�[����
	mDocPrefDB.CopyFromModePrefDBIfStillDefault();
	*/
	
	if( inTextEncodingName.GetLength() > 0 ) 
	{
		//DocPref��TextEncoding�ݒ�
		//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,inTextEncodingName);
		GetDocPrefDB().SetTextEncoding(inTextEncodingName);//#1040
		//�e�L�X�g�G���R�[�f�B���O�w��t���OON
		mTextEncodingIsDirectedByInitArgument = true;//#830
	}
	
#if IMPLEMENTATION_FOR_MACOSX
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kSetDefaultCreator) == true )
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Text(AModePrefDB::kDefaultCreator,text);
		mCreator = text.GetOSTypeFromText();//AUtil::GetOSTypeFromAText(text);
		mType = 'TEXT';
	}
#endif
	
	//=======================
	// �e�L�X�g�ݒ�
	//=======================
	//�e�L�X�g�ݒ�
	{
		AStMutexLocker	locker(mTextMutex);//#890
		mText.SetText(inDocumentText);
	}
	
	Init_Common();
}

//#379
/**
Diff�Ώۃh�L�������g�𐶐��i�e�L�X�g���琶���j
*/
void	ADocument_Text::SPI_Init_DiffTargetDocument( const AText& inTitle, const AText& inDocumentText,
		const AModeIndex inModeIndex, const AText& inTextEncodingName )
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::SPI_Init_DiffTargetDocument() started.",this);
	
	//#361
	//�h�L�������g�^�C�v�ݒ�
	mTextDocumentType = kTextDocumentType_DiffTarget;
	
	//ReadOnly
	NVI_SetReadOnly(true);
	
	//�^�C�g���ݒ�
	SPI_SetTemporaryTitle(inTitle);
	
	//=======================
	// �h�L�������g�ݒ�
	//=======================
	
	//mModeIndex�ݒ�
	SetModeIndex(inModeIndex);//#890
	
	//DocPref������ #898
	InitDocPref(inModeIndex);
	
	//TextEncoding
	if( inTextEncodingName.GetLength() > 0 ) 
	{
		//DocPref��TextEncoding�ݒ�
		//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,inTextEncodingName);
		GetDocPrefDB().SetTextEncoding(inTextEncodingName);//#1040
		//�e�L�X�g�G���R�[�f�B���O�w��t���OON
		mTextEncodingIsDirectedByInitArgument = true;//#830
	}
	
	//�e�L�X�g�ݒ�
	{
		AStMutexLocker	locker(mTextMutex);//#890
		mText.SetText(inDocumentText);
	}
	
	//���ʐݒ�
	Init_Common();
}
/**
Diff�Ώۃh�L�������g�𐶐��i�t�@�C�����琶���j
*/
void	ADocument_Text::SPI_Init_DiffTargetDocument( const AText& inTitle, const AFileAcc& inFile,
													const AModeIndex inModeIndex, const AText& inTextEncodingName )
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::SPI_Init_DiffTargetDocument() started.",this);
	
	//#361
	//�h�L�������g�^�C�v�ݒ�
	mTextDocumentType = kTextDocumentType_DiffTarget;
	
	//ReadOnly
	NVI_SetReadOnly(true);
	
	//�^�C�g���ݒ�
	SPI_SetTemporaryTitle(inTitle);
	
	//�t�@�C���ݒ�
	mFileForDiffTargetDocument = inFile;
	
	//=======================
	// �h�L�������g�ݒ�
	//=======================
	
	//DocPref�ǂݍ���
	AModeIndex	modeIndex = LoadDocPref();//#898
	
	//mModeIndex�ݒ�
	SetModeIndex(inModeIndex);//#890
	
	//inTextEncodingName���w�肳��Ă���ꍇ�iAppleScript�o�R�ɂ��I�[�v�����j�͂������D�悳����
	if( inTextEncodingName.GetLength() > 0 )
	{
		//DocPref��TextEncoding�ݒ�
		//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,inTextEncodingName);
		GetDocPrefDB().SetTextEncoding(inTextEncodingName);//#1040
		//�e�L�X�g�G���R�[�f�B���O�w��t���OON
		mTextEncodingIsDirectedByInitArgument = true;//#830
	}
	
	//���ʐݒ�
	Init_Common();
	
	//�t�@�C���ǂݍ���
	SPI_ScreenAndLoadDocumentFile();
}

//Init���ʕ���
void	ADocument_Text::Init_Common()//#212
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::Init_Common() started.",this);
	
	/*#898
	//�����ɂ����Ƃ��ɁA�܂�DocPref�Ŏw�肳��Ă��Ȃ��f�[�^��ModePref����R�s�[����
	GetDocPrefDB().CopyFromModePrefDBIfStillDefault();
	*/
	
	/*#699
	//�s���v�Z�i�E�C���h�E�����O�ɂƂ肠����kWrapMode_NoWrap�ōs���v�Z����j
	mTextInfo.CalcLineInfoAllWithoutView(mText);//win
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("Document text info calculated.",this);
	*/
#if IMPLEMENTATION_FOR_MACOSX
	//�N���G�[�^�^�^�C�v�����ύX
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kForceMiTextFile) == true )
	{
		mCreator = 'MMKE';
		mType = 'TEXT';
	}
#endif
	
	//#354
	//�h�L�������g�������̃t�H���g���L��
	SPI_GetFontName(mOriginalFontName);
	mOriginalFontSize = SPI_GetFontSize();
	
	//#379
	//Diff target document�̏ꍇ�̍s�܂�Ԃ��ݒ�
	if( /*#361 mIsDiffTargetDocument*/SPI_IsDiffTargetDocument() == true && 
	   GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowDiffTargetWithWordWrap) == true )
	{
		GetDocPrefDB().SetData_Number(ADocPrefDB::kWrapMode,kWrapMode_WordWrap);
	}
	
	//�s�v�Z�X���b�h������#699
	GetWrapCalculatorThread().SPI_Init(GetObjectID());
	
	//���@�F���X���b�h������#698
	GetSyntaxRecognizerThread().SPI_Init(GetObjectID());
	
	//���̃I�u�W�F�N�g�Ƃ̐������̂��߁A�܂���s�ǉ����Ă���#699
	mTextInfo.AddLineInfo(0,mText.GetItemCount(),false);
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::Init_Common() ended.",this);
}

/*
�y�ʏ탍�[�h���z
�Eview���ŏ���activate�ɂȂ������ɁASPI_DoViewActivating()�o�R�ŁAStartScreeningForLoadDocumentFile()���R�[�������
�EStartScreeningForLoadDocumentFile()����A�X�N���[�j���O�X���b�h���N������
�E�X�N���[�j���O�X���b�h����SPI_ScreenDocumentFile()�����s����B������A���C���X���b�h�ɃC�x���g�ʒm�����B
�E���C���X���b�h��EVTDO_DoInternalEvent()����ASPI_TransitInitState_ViewActivated()���R�[�������B
�ESPI_TransitInitState_ViewActivated()�ŁA���[�h�A�s�܂�Ԃ��v�Z�A��ԑ@�ۂ����s�B

�ydiff target�h�L�������g�i�t�@�C���j���[�h���A����сA�}���`�t�@�C���u�����z
�ESPI_ScreenAndLoadDocumentFile()���R�[�������B
�ESPI_ScreenAndLoadDocumentFile()���ŁA�X�N���[�j���O�A���[�h�����s����B
�Ediff taret�h�L�������g�̏ꍇ�F���̌�SPI_DoViewActivating()���R�[�����ꂽ��A
 ����SPI_TransitInitState_ViewActivated()���R�[������B
�E�}���`�t�@�C���u���̏ꍇ�Fview��activate����Ȃ��B
*/

/**
@return �ǂݍ��ݐ���������true
@note diff target�h�L�������g���[�h���A����сA�}���`�t�@�C���u�����ɃR�[�������B�X�N���[�j���O�ƃ��[�h�𓯊����s����B
*/
ABool	ADocument_Text::SPI_ScreenAndLoadDocumentFile()
{
	//==================�X�N���[�j���O���s�i�������s�j==================
	SPI_ScreenDocumentFile();
	
	//==================���[�h���s==================
	LoadDocumentFile();
	
	//�X�N���[�j���O�G���[�����Ȃ�true��Ԃ�
	if( mFileScreenError == kFileScreenError_NoError )
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
�h�L�������g�t�@�C���X�N���[�j���O�X���b�h���s�J�n����B
@note �ʏ탍�[�h���ɁASPI_DoViewActivating()����R�[�������B
�T�u�X���b�h����SPI_ScreenDocumentFile()�����s��A���C���X���b�h�Ɋ����ʒm�𑗐M����B
�����ʒm�ɂ��ASPI_TransitInitState_ViewActivated()���R�[������A��ԑ@�ہi���[�h�E�s�܂�Ԃ��v�Z�j�����s�����B
*/
void	ADocument_Text::StartScreeningForLoadDocumentFile()
{
	//==================�h�L�������g������ԑJ��==================
	//�X���b�h�ł̃X�N���[����
	SPI_SetDocumentInitState(kDocumentInitState_Initial_FileScreeningInThread);
	
	//==================�X���b�h�N��==================
	AThread_DocumentFileScreeningFactory	factory(NULL);
	AObjectID	threadObjectID = GetApp().NVI_CreateThread(factory);
	(dynamic_cast<AThread_DocumentFileScreening&>(GetApp().NVI_GetThreadByID(threadObjectID))).SPNVI_Run(GetObjectID());
	//�ő�3�b�E�G�C�g�i���̑O�ɃX�N���[�j���O�X���b�h���I��������A�X���[�v���������j
	for( AIndex i = 0; i < 3; i++ )
	{
		if( GetApp().NVI_GetThreadByID(threadObjectID).NVI_IsThreadWorking() == false )   break;
		GetApp().NVI_SleepWithTimer(1);
	}
	//�X�N���[�j���O�X���b�h�������Ă��āA�G���[��������΁A��ԑJ�ڂ������ōs���B�i����ɂ��A�����\������Ă��Ȃ���Ԃ���u�\�������̂�h���j
	//�G���[������ꍇ�́A�h�L�������g�������Ă��܂��ASPI_DoViewActivating()�̌�̏����ɉe��������̂ŁA��ԑJ�ڂ͏������Ȃ��B�i����EVT_DoInternalEvent()�ŏ����j
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		if( mFileScreenError == kFileScreenError_NoError )
		{
			SPI_TransitInitState_ViewActivated();
		}
	}
}

/**
�h�L�������g�t�@�C���̃X�N���[�j���O�����s����
�ʏ탍�[�h���F�T�u�X���b�h����񓯊����s�����
diff target�h�L�������g���[�h���A����сA�}���`�t�@�C���u�����F���C���X���b�h���瓯�����s�����
*/
void	ADocument_Text::SPI_ScreenDocumentFile()
{
	//==================�h�L�������g������ԑJ��==================
	//�X�N���[���ς�
	SPI_SetDocumentInitState(kDocumentInitState_Initial_FileScreened);
	
	//==================�t�@�C����specify�i�V�K�h�L�������g���j�Ȃ牽�������I��==================
	if( mFileAcc.IsSpecified() == false )
	{
		return;
	}
	//==================�G�C���A�X����==================
	AFileAcc	file;
	file.ResolveAnyLink(mFileAcc);
	//mFileAcc�̃t�@�C�����񑶍݂̂Ƃ���file��specify����Ȃ��̂ŁA�����Ńt�@�C���񑶍݃G���[�ŏI���B#962
	if( file.IsSpecified() == false )
	{
		mFileScreenError = kFileScreenError_FileNotExist;
		return;
	}
	//�G�C���A�X���������ǂ����𔻒�
	if( file.Compare(mFileAcc) == false )
	{
		//�G�C���A�X�������ꍇ�A�G�C���A�X������̃t�@�C�������łɊJ����Ă��Ȃ����ǂ����𔻒�
		mDuplicatedDocumentID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,file);
		if( mDuplicatedDocumentID != kObjectID_Invalid )
		{
			//���łɓ����t�@�C�����J����Ă���Ƃ��́A�G���[�B
			mFileScreenError = kFileScreenError_DocumentDuplicatedAfterLinkResolved;
			return;
		}
		//�G�C���A�X������̃t�@�C����ݒ�
		NVI_SpecifyFileUnscreened(file);
	}
	
	//==================�t�@�C�����݃`�F�b�N==================
	if( file.Exist() == false )
	{
		mFileScreenError = kFileScreenError_FileNotExist;
		return;
	}
	
	//==================�t�H���_�łȂ����Ƃ̃`�F�b�N==================
	if( file.IsFolder() == true )
	{
		mFileScreenError = kFileScreenError_FileIsFolder;
		return;
	}
	
	/*#1116 �t�@�C���T�C�Y��������
	//==================�t�@�C���T�C�Y�`�F�b�N==================
	//�t�@�C���T�C�Y��100MB�𒴂���t�@�C���̏ꍇ�́A���[�_���G���[�\�����āA�t�@�C�����J���Ȃ�
	AByteCount	filesize = file.GetFileSize();
	if( filesize > kLimit_OpenTextFileSize )
	{
		mFileScreenError = kFileScreenError_FileSizeOver;
		return;
	}
	//�g�[�^���̃t�@�C���T�C�Y��200MB�𒴂���ꍇ�́A���[�_���G���[�\�����āA�t�@�C�����J���Ȃ�
	if( GetApp().SPI_GetTotalTextDocumentMemoryByteCount() + filesize > kLimit_TotalOpenTextFileSize )
	{
		mFileScreenError = kFileScreenError_TotalFileSizeOver;
		return;
	}
	*/
	//==================�N���G�[�^�^�^�C�v�ǂݍ���==================
#if IMPLEMENTATION_FOR_MACOSX
	//�N���G�[�^�^�^�C�v�ǂݍ���
	file.GetCreatorType(mCreator,mType);
#endif
}

//#831
/**
���[�h�iSPI_TransitInitState_ViewActivated()����R�[�������B�V�K�h�L�������g�̏ꍇ���܂݁A�S�Ẵh�L�������g�^�C�v�ŃR�[�������j
*/
void	ADocument_Text::LoadDocumentFile()
{
	//�t�@�C�����X�N���[���ς݂Ƃ��Đݒ�
	if( NVI_IsFileSpecified() == true )
	{
		AFileAcc	file;
		NVI_GetFile(file);
		NVI_SpecifyFile(file);
	}
	
	//==================TextInfo�Ƀ��[�h�ݒ�==================
	//TextInfo�փ��[�h�ݒ�
	mTextInfo.SetMode(mModeIndex);
	
	//==================�t�@�C���ǂݍ���==================
	//�t�@�C���ǂݍ��݂��邩�ǂ����̃t���O
	ABool	shouldLoadFile = false;
	AFileAcc	loadFile;
	//�t�@�C���w�肳��Ă���ꍇ�͂��̃t�@�C����ǂݍ��ނ��Ƃɂ���
	if( NVI_IsFileSpecified() == true )
	{
		//�t�@�C���擾
		NVI_GetFile(loadFile);
		shouldLoadFile = true;
	}
	//diff�^�[�Q�b�g�h�L�������g�̏ꍇ�ŁA�t�@�C���w�肳��Ă���ꍇ�́A���̃t�@�C����ǂݍ��ނ��Ƃɂ���
	else if( mTextDocumentType == kTextDocumentType_DiffTarget && mFileForDiffTargetDocument.IsSpecified() == true )
	{
		//�t�@�C���擾
		loadFile = mFileForDiffTargetDocument;
		shouldLoadFile = true;
	}
	//�t�@�C���ǂݍ��݂��邩�ǂ����̃t���O��ON�̂Ƃ��́A�t�@�C����ǂݍ���
	if( shouldLoadFile == true )
	{
		//�t�@�C���ǂݍ���
		{
			AStMutexLocker	locker(mTextMutex);//#890
			loadFile.ReadTo(mText);
		}
		
		//�ŏI�ҏW�����L��
		loadFile.GetLastModifiedDataTime(mLastModifiedDateTime);
		//#653 �ǂݍ��ݎ��̃t�@�C���f�[�^���L���i�e�L�X�g�G���R�[�f�B���O���ϊ��j���A�v���ł̕ҏW���m�Ɏg�p
		//#693 mLastReadOrWriteFileRawData.SetText(mText);
		//�ǂݍ��ݎ��̃t�@�C���f�[�^���t�@�C���Ƃ��ċL������B
		AFileAcc	lastLoadOrSaveFile;
		GetLastLoadOrSaveCopyFile(lastLoadOrSaveFile);
		lastLoadOrSaveFile.CreateFile();
		lastLoadOrSaveFile.WriteFile(mText);
	}
	
	/*��O�����ƕ�����Â炭�Ȃ�̂ŋ����I��NoWrap�ɂ��鏈���͍폜�B�i�s�܂�Ԃ��́A�h�L�������g�ݒ肪����΂���ɏ]���A������΃��[�h�ݒ�i�u�ҏW�v�^�u�j�ɏ]���B�T�C�Y���傫���Ă��B�܂�Ԃ��ݒ�̓X���b�h�����Ă���̂ŁA�܂�Ԃ��v�Z�Ɏ��Ԃ�������ꍇ�́A�s�܂�Ԃ������ɓr���ŕύX���邱�Ƃ��\�j
	//R0208 1MB���z�����狭���I��NoWrap
	if( mText.GetItemCount() > 1024*1024 )
	{
		GetDocPrefDB().SetData_Number(ADocPrefDB::kWrapMode,kWrapMode_NoWrap);
	}
	*/
	//�e�L�X�g��100MB�𒴂���ꍇ�͍s�܂�Ԃ��Ȃ��Ƃ��� #1118 �T�C�Y���傫���Ƃ��ɋ����I��NoWrap�ɂ��鏈���͂���ς蕜���B����s�܂�Ԃ���ύX���鑀�삪�ʓ|���������߁B
	if( mText.GetItemCount() > 100*1024*1024 )
	{
		GetDocPrefDB().SetData_Number(ADocPrefDB::kWrapMode,kWrapMode_NoWrap);
	}
	
	//==================�e�L�X�g�G���R�[�f�B���O�ϊ�==================
	
	//UTF8�֕ϊ�
	{
		AStMutexLocker	locker(mTextMutex);//#890
		ConvertToUTF8(mText);
	}
	
#if 0
	//TextEncoding�����F��
	ABool	kakuteiByPref = false;//#473
	ABool	kakuteiByGuess = false;//#473
	ATextEncoding	guessTextEncoding = SPI_GetTextEncoding();//#764 mTextEncoding;//#473
	if( mText.GetItemCount() > 0 )
	{
		//#473 kakutei = GuessTextEncoding();//TextEncoding�����F����ADocPrefDB::kTextEncodingName���f�t�H���g�̂܂܂̏ꍇ�̂ݎ��s
		//����TextEncoding���擾
		AText	tecname;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Text(AModePrefDB::kDefaultTextEncoding,tecname);
		ATextEncodingWrapper::GetTextEncodingFromName(tecname,guessTextEncoding);
		
		kakuteiByGuess = mText.GuessTextEncoding(GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSniffTextEncoding_BOM),
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSniffTextEncoding_Charset),
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSniffTextEncoding_Legacy),
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSetDefaultTextEncodingWhenNotDecided),//B0390
				guessTextEncoding);
		//DocPref���f�t�H���g�̂܂܂Ȃ琄��TextEncoding��ݒ�
		/* ����ɂ��DocPref�Ɛ���TextEncoding�͓����l�ɂȂ�B�m��t���O�͐����̌��ʂƂȂ�B */
		//#830 if( GetDocPrefDB().IsStillDefault(ADocPrefDB::kTextEncodingName) == true )
		if( mTextEncodingIsDirectedByInitArgument == false )//#830
		{
			//#764 mTextEncoding = guessTextEncoding;
			AText	text;
			ATextEncodingWrapper::GetTextEncodingName(guessTextEncoding,text);
			GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,text);
		}
		//DocPref��TextEncoding�ݒ�ς݂Ȃ�m��t���OOn�ɂ���
		/* DocPref��TextEncoding�ݒ�ς݂ƂȂ�P�[�X
		1. AppleScript�ɂ��TextEncoding�w�肳��Ċ����t�@�C�����J�����ꍇ
		2. Xattr�t���̊����t�@�C�����J�����ꍇ
		3. �V�K�t�@�C���EDiffTarget�h�L�������g�i���Ȃ��Ƃ����[�h�ݒ肩��̃R�s�[���������삷��j
		*/
		else
		{
			kakuteiByPref = true;
		}
	}
	else//#570
	{
		kakuteiByPref = true;
		kakuteiByGuess = true;
	}
	
	//B0343 TextEncoding����Ή��̂Ƃ���UTF-8�ɂ��Ă���
	AText	text;
	GetDocPrefDB().GetData_Text(ADocPrefDB::kTextEncodingName,text);
	if( ATextEncodingWrapper::CheckTextEncodingAvailability(text) == false )
	{
		text.SetCstring("UTF-8");
		GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,text);
	}
	
	//�����ɂ����Ƃ��ɁA�e�L�X�g�G���R�[�f�B���O���m��A���A���[�h�ݒ�Łu�e�L�X�g�G���R�[�f�B���O���m��ł��Ȃ��Ƃ��̓_�C�A���O��\������v��On�ɂ����ꍇ��
	//�G���R�[�f�B���O�C���_�C�A���O�\���t���OOn
	//#379 ABool	showCorrectEncodingWindow = false;
	if( kakuteiByPref == false && kakuteiByGuess == false &&//#473 docpref�ł��A�����ł��m��ł��Ȃ��ꍇ
	GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kShowDialogWhenTextEncodingNotDecided) == true )
	{
		//#379 showCorrectEncodingWindow = true;
		mShouldShowCorrectEncodingWindow = true;//#379
		mShouldShowCorrectEncodingWindow_FallbackType = kTextEncodingFallbackType_None;//#473
	}
	
	//mTextEncoding�̐ݒ�
	GetDocPrefDB().GetData_Text(ADocPrefDB::kTextEncodingName,text);
	ATextEncodingWrapper::GetTextEncodingFromName(text,mTextEncoding);
	
	//TextInfo�փ��[�h�ݒ�
	mTextInfo.SetMode(mModeIndex);
	
	//�e�L�X�g������`���֕ϊ�
	if( mText.GetItemCount() > 0 )
	{
		//UTF8�֕ϊ�
		ATextEncoding	actualTec = mTextEncoding;
		AReturnCode	actualReturnCode = static_cast<AReturnCode>(GetDocPrefDB().GetData_Number(ADocPrefDB::kReturnCode));//#307
		//B0400 if( AUtil::ConvertToUTF8CRAnyway(mText,mTextEncoding,actualTec) == false )
		//�e�L�X�g�G���R�[�f�B���O�ϊ����s
		//Shift_JIS���X���X�t�H�[���o�b�N�͊m��t���OON�̂Ƃ��̂ݍs���B�i���m��ōs����UTF-8�𖳗��SJIS�ϊ������肷�邽�߁j
		{
			AStMutexLocker	locker(mTextMutex);//#890
			if( GetApp().NVI_ConvertToUTF8CRAnyway(mText,mTextEncoding,guessTextEncoding,
							true,/*#392 docpref�ɂ��m�莞�̓t�H�[���o�b�N���s���� false*/kakuteiByPref,
							((kakuteiByPref||kakuteiByGuess)&&
								GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kUseSJISLosslessFallback)),
							false,actualTec,actualReturnCode,mShouldShowCorrectEncodingWindow_FallbackType) == false )//B0400 #182 #307 #473
			{
				//�ϊ��G���[�����̏ꍇ
				//�G���R�[�f�B���O�C���_�C�A���O�\���t���OOn
				//���[�h�ݒ�u�e�L�X�g�G���R�[�f�B���O���m��ł��Ȃ��Ƃ��̓_�C�A���O��\������v�Ɋւ�炸�_�C�A���O�\������
				//#379 showCorrectEncodingWindow = true;
				mShouldShowCorrectEncodingWindow = true;//#379
			}
		}
		//
		Convert5CToA5WhenOpenJIS();
		
		//���ۂ̃e�L�X�g�G���R�[�f�B���O��PrefDB�ƁAmTextEncoding�ɕۑ�
		AText	text;
		ATextEncodingWrapper::GetTextEncodingName(actualTec,text);
		GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,text);
		mTextEncoding = actualTec;
		
		//���s�R�[�h��ۑ� #307
		GetDocPrefDB().SetData_Number(ADocPrefDB::kReturnCode,actualReturnCode);
		
		/*#844
		//DOSEOF(1A)�폜
		if( mText.GetLastChar() == kUChar_DOSEOF )
		{
			AStMutexLocker	locker(mTextMutex);//#890
			mText.Delete1(mText.GetItemCount()-1);
		}
		*/
	}
#endif
	
	//==================�o�C�i���t�@�C������========================
	//#703
	mSuspectBinaryData = mText.SuspectBinaryData();
	
	//========================�e�L�X�g�G���R�[�f�B���O�m��ς݂�ݒ�========================
	//doc pref�̊m��ς݃t���O��ON�ɂ���B
	mDocPrefDB.SetTextEncodingFixed(true);
	
	//#81
	//�Ō�Ƀ��[�h�^�Z�[�u�������̃e�L�X�g�G���R�[�f�B���O�i���t�@�C���̌��݂̃e�L�X�g�G���R�[�f�B���O�j���L���������o�b�N�A�b�v���ɕۑ�
	SPI_GetTextEncoding(mTextEncodingWhenLastLoadOrSave);
	
	//==================���[�h�ς݃t���OON========================
	//#831
	//���[�h�ς݃t���OON
	mLoaded = true;
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("Document text has converted to UTF-8.",this);
}

//#764
/**
�e�L�X�g�G���R�[�f�B���O������`��UTF8�֕ϊ��iLoad(), SPI_ReloadEditByOtherApp()����R�[�������j
*/
void	ADocument_Text::ConvertToUTF8( AText& ioText )
{
	//==================�e�L�X�g�G���R�[�f�B���O�ϊ�==================
	const ATextEncoding	origtec = SPI_GetTextEncoding();
	ATextEncoding	resultTec = origtec;
	AReturnCode	resultReturnCode = static_cast<AReturnCode>(GetDocPrefDB().GetData_Number(ADocPrefDB::kReturnCode));//#307
	ATextEncodingFallbackType	resultFallbackType = kTextEncodingFallbackType_None;
	ABool	convertSuccessful = true;
	ABool	notMixed = true;//#995
	//------------------Init�֐��̈����Ńe�L�X�g�G���R�[�f�B���O���w�肳��Ă����ꍇ�́A����tec�ł̂ݕϊ�����------------------
	if( mTextEncodingIsDirectedByInitArgument == true )
	{
		//�w��tec�ŕϊ�
		//�w��tec�ł̕ϊ��Ȃ̂ŁA�t�H�[���o�b�N�ASJIS���X���X�t�H�[���o�b�N�͗����Ƃ��L��ɂ���B
		//�Ԃ�lconvertSuccessful�̓t�H�[���o�b�N���g���Ăł��ϊ����ł����Ƃ���true�ɂȂ�B
		convertSuccessful = GetApp().
				SPI_ConvertToUTF8CR(ioText,origtec,true,
									true,//�t�H�[���o�b�N
									false,//#844 SJIS���X���X�t�H�[���o�b�N��drop GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kUseSJISLosslessFallback),//SJIS���X���X
									resultReturnCode,resultTec,resultReturnCode,resultFallbackType,notMixed);//#995
	}
	//------------------Init�֐��̈����Ńe�L�X�g�G���R�[�f�B���O���w�肳��Ă��Ȃ��ꍇ�́Adoc pref�ɏ]���ĕϊ�����------------------
	else
	{
		//doc pref�ɏ]���ĕϊ��B
		//doc pref�̊m��ς݃t���O��OFF�Ȃ琄�����s���B
		//�Ԃ�lconvertSuccessful�̓t�H�[���o�b�N���g���Ăł��ϊ����ł����Ƃ���true�ɂȂ�B
		AText	filepath;;
		NVI_GetFilePath(filepath);
		ALoadTextPurposeType	purposeType = kLoadTextPurposeType_LoadDocument;
		if( mForMultiFileReplace == true )
		{
			purposeType = kLoadTextPurposeType_LoadDocument_ForMultiFileReplace;
		}
		convertSuccessful = GetApp().SPI_ConvertToUTF8CRUsingDocPref(purposeType,
																	 ioText,mDocPrefDB,filepath,
																	 resultTec,resultReturnCode,resultFallbackType,notMixed);//#995
	}
	
	//#995
	//�����̉��s�R�[�h���������Ă���ꍇ�̓G���[��\������
	if( notMixed == false )
	{
		AText	notificationTitle,notificationText;
		notificationTitle.SetLocalizedText("Notification_ReturnCodeMixedError_Title");
		notificationText.SetLocalizedText("Notification_ReturnCodeMixedError");
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
				SPI_SetNotification_General(notificationTitle,notificationText,GetEmptyText());
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_ShowNotificationPopupWindow(true);
	}
	
	//==================�e�L�X�g�G���R�[�f�B���O�E���s�R�[�h���ʂ�ۑ�==================
	//
	//���ۂ̃e�L�X�g�G���R�[�f�B���O��PrefDB�ɕۑ�
	//���ʂ��Ⴄ�ꍇ�A�܂��́A���݂�tec�������j���[���ɑ��݂��Ȃ��̂ݐݒ肷��
	//�iATextEncodingWrapper::GetTextEncodingFromName()�ɂāAtec��ISO-8859-1�ɑ΂���tec:0x500���擾����邪�A
	//0x500�͖{��Windows-1252�Ȃ̂ŁA���XISO-885-1���������̂�Windows-1252�ɕω����Ă��܂����(#934)�̑Ή��̂��߁j
	AText	tecName;
	GetDocPrefDB().GetData_Text(ADocPrefDB::kTextEncodingNameCocoa,tecName);//#1040
	if( origtec != resultTec || GetApp().GetAppPref().IsTextEncodingAvailable(tecName) == false )
	{
		AText	text;
		ATextEncodingWrapper::GetTextEncodingName(resultTec,text);
		//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,text);
		GetDocPrefDB().SetTextEncoding(text);//#1040
	}
	
	//���s�R�[�h��ۑ� #307
	GetDocPrefDB().SetData_Number(ADocPrefDB::kReturnCode,resultReturnCode);
	
	//==================�e�L�X�g�G���R�[�f�B���O�t�H�[���o�b�N�g�p�_�C�A���O�^notification==================
	
	if( convertSuccessful == true && resultFallbackType != kTextEncodingFallbackType_None )
	{
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowDialogWhenTextEncodingFallback) == true )
		{
			//�e�L�X�g�E�C���h�E�擾
			AWindowID	firstWindowID = SPI_GetFirstWindowID();
			if( firstWindowID != kObjectID_Invalid )
			{
				//�e�L�X�g�E�C���h�E�����ς݂ł���΁A�_�C�A���O��\��
				GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_ShowTextEncodingCorrectWindow(false,resultFallbackType);//#473
			}
		}
		else
		{
			AText	notificationTitle,notificationText;
			notificationTitle.SetLocalizedText("Notification_TextEncodingFallback_Title");
			notificationText.SetLocalizedText("Notification_TextEncodingFallback");
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
					SPI_SetNotification_General(notificationTitle,notificationText,GetEmptyText());
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_ShowNotificationPopupWindow(true);
		}
	}
	
	//==================�e�L�X�g�G���R�[�f�B���ONG�_�C�A���O==================
	
	if( convertSuccessful == false )
	{
		//�e�L�X�g�E�C���h�E�擾
		AWindowID	firstWindowID = SPI_GetFirstWindowID();
		if( firstWindowID != kObjectID_Invalid )
		{
			//�e�L�X�g�E�C���h�E�����ς݂ł���΁A�_�C�A���O��\��
			GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_ShowTextEncodingCorrectWindow(false,kTextEncodingFallbackType_None);//#473
		}
		else
		{
			//�e�L�X�g�E�C���h�E�������ł���΁A�_�C�A���O�\���\��
			mShouldShowCorrectEncodingWindow = true;
			mShouldShowCorrectEncodingWindow_FallbackType = kTextEncodingFallbackType_None;
		}
	}
}

/**
5c��a5�ɕϊ��i�o�b�N�X���b�V����\�ŕ\������j
*/
/*#940 ���[�h����5c->a5�ϊ��͂��Ȃ��悤�ɂ���B�\������a5�ɕϊ�����B
void	ADocument_Text::Convert5CToA5WhenOpenJIS( AText& ioText )
{
	if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kConvert5CToA5WhenOpenJIS) == false )   return;
	if( SPI_IsJISTextEncoding() == true )
	{
		AText	yen;
		yen.SetLocalizedText("Text_Yen");
		for( ATextIndex pos = 0; pos < ioText.GetItemCount(); pos = ioText.GetNextCharPos(pos) )
		{
			if( ioText.Get(pos) == kUChar_Backslash )
			{
				ioText.Delete1(pos);
				ioText.InsertText(pos,yen);
			}
		}
	}
}
*/

/**
JIS�n�e�L�X�g�G���R�[�f�B���O���ǂ������擾
*/
ABool	ADocument_Text::SPI_IsJISTextEncoding() const 
{
	ATextEncoding	tec = SPI_GetTextEncoding();//#764
	return ( ATextEncodingWrapper::IsSJISSeriesTextEncoding(tec) ||//#1040
		tec == ATextEncodingWrapper::GetJISTextEncoding() ||
		tec == ATextEncodingWrapper::GetEUCTextEncoding() );
}

//#1300
/**
�R�s�[����5c��a5�֕ϊ����邩�ǂ�����Ԃ��B
JIS�n�A���AAModePrefDB::kConvert5CToA5WhenOpenJIS��true�̏ꍇ�A�����R�[�h5c�i�o�b�N�X���b�V���j�ŁA�\����a5�i���p���j�ɂ��Ă���̂ŁA
�R�s�[����Ƃ��́Aa5�i���p���j�ŏo�͂���悤�ɂ��邽�߁B
*/
ABool	ADocument_Text::SPI_ShouldConvertFrom5CToA5ForCopy() const
{
	return ( SPI_IsJISTextEncoding() == true &&
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kConvert5CToA5WhenOpenJIS) == true );
}

//#65
/**
�}���`�t�@�C���u���p���[�h�E�s�܂�Ԃ��v�Z
*/
ABool	ADocument_Text::SPI_LoadForMultiFileReplace()
{
	//������ԈȊO�ŃR�[�����ꂽ��G���[
	if( SPI_GetDocumentInitState() != kDocumentInitState_Initial )
	{
		_ACError("",NULL);
		return false;
	}
	
	//�}���`�t�@�C���u���p�ł��邱�Ƃ������t���OON
	mForMultiFileReplace = true;
	//�X�N���[�������[�h
	if( SPI_ScreenAndLoadDocumentFile() == true )
	{
		//�s���S�폜
		mTextInfo.DeleteLineInfoAll();
		//��s�ɑS�f�[�^���i�[
		mTextInfo.AddLineInfo(0,mText.GetItemCount(),false);
		//
		return true;
	}
	else
	{
		return false;
	}
}


//#699
/**
�s��񏉊���
*/
void	ADocument_Text::InitLineInfoData()
{
	//=========================�e��f�[�^������=========================
	//arrowtodef�f�[�^�N���A
	ClearArrowToDef();
	//diff�\���N���A
	ClearDiffDisplay();
	//�s���폜
	mTextInfo.DeleteLineInfoAll();
	//�W�����v���ڑS�폜
	DeleteAllJumpItems();
	//���[�J�����ʎq�폜
	SPI_DeleteAllLocalIdentifiers();
	//�C���|�[�g�t�@�C���f�[�^�폜
	SPI_DeleteAllImportFileData();
}

//#699
/**
�s�܂�Ԃ�����Čv�Z����
*/
void	ADocument_Text::ReTransitInitState_Recalculate()
{
	//������Ԃ̏ꍇ�͉������Ȃ��B�i�܂�view activate����Ă��Ȃ��̂ŁA����activate���ꂽ�Ƃ��ɒʏ탋�[�g�ōs�܂�Ԃ��v�Z�����j
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial )
	{
		return;
	}
	
	//�s�܂�Ԃ��v�Z�X���b�h�����쒆�Ȃ�A�s�܂�Ԃ��v�Z�X���b�h��~
	if( mWrapCalculator_ThreadUnpausedFlag == true )
	{
		mWrapCalculator_ThreadUnpausedFlag = false;
		mWrapCalculator_CurrentLineIndex = kIndex_Invalid;
		mWrapCalculatorWorkingByInsertText = false;
		mWrapCalculatorProgressTotalTextLength = kIndex_Invalid;
		mWrapCalculatorProgressRemainedTextLength = 0;
		GetWrapCalculatorThread().NVI_AbortThread();
		GetWrapCalculatorThread().NVI_UnpauseIfPaused();
		GetWrapCalculatorThread().NVI_WaitThreadEnd();
		GetWrapCalculatorThread().ClearData();
	}
	//�s�܂�Ԃ��X���b�h���瑗�t���ꂽ�܂�Ԃ��v�Z�I���C�x���g���A�L���[����폜����
	ABase::RemoveEventFromMainInternalEventQueue(kInternalEvent_WrapCalculatorPaused,GetWrapCalculatorThread().GetObjectID());
	
	//���@�F���X���b�h�����쒆�Ȃ�A���@�F���X���b�h���~����
	SPI_StopSyntaxRecognizerThread();
	
	//�s�֘A�f�[�^������
	InitLineInfoData();
	
	//TextView�̑I��ݒ�̂��߁A�܂���s�ǉ����Ă���#699
	mTextInfo.AddLineInfo(0,mText.GetItemCount(),false);
	
	//�e�r���[�̑I���������i0,0��I���j����B
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_InitSelect();
	}
	
	//��Ԃ�������ԁi�X�N���[���ς݁j�ɂ���
	SPI_SetDocumentInitState(kDocumentInitState_Initial_FileScreened);
	
	//��ԑJ�ځi�܂�Ԃ��v�Z�J�n�j
	mCalculateWrapInThread = true;
	SPI_TransitInitState_ViewActivated();
}

//#962
/**
screen�G���[��\�����A�h�L�������g�����
�iSPI_TransitInitState_ViewActivated()�ɂăG���[���m�����Ƃ��ɁAInternal Event�����t����A
�@EVTDO_DoInternalEvent()�o�R�ł����ɗ���B�j
*/
void	ADocument_Text::SPI_ShowScreenErrorAndClose()
{
	//�t�@�C���擾
	AFileAcc	file;
	NVI_GetFile(file);
	AText	filename;
	file.GetFilename(filename);
	switch(mFileScreenError)
	{
		//�t�@�C����������Ȃ��ꍇ
	  case kFileScreenError_FileNotExist:
		{
			AText	message1, message2;
			message1.SetLocalizedText("CantOpen_Title");
			message2.SetLocalizedText("CantOpen_FileNotExist");
			message2.ReplaceParamText('0',filename);
			GetApp().NVI_ShowModalAlertWindow(message1,message2);
			break;
		}
		//�t�H���_�̏ꍇ
	  case kFileScreenError_FileIsFolder:
		{
			AText	message1, message2;
			message1.SetLocalizedText("CantOpen_Title");
			message2.SetLocalizedText("CantOpen_FileIsFolder");
			message2.ReplaceParamText('0',filename);
			GetApp().NVI_ShowModalAlertWindow(message1,message2);
			break;
		}
		//�t�@�C���T�C�Y��100MB�𒴂���t�@�C���̏ꍇ�́A���[�_���G���[�\�����āA�t�@�C�����J���Ȃ�
	  case kFileScreenError_FileSizeOver:
		{
			AText	message1, message2;
			message1.SetLocalizedText("CantOpen_Title");
			message2.SetLocalizedText("CantOpen_FileSize");
			message2.ReplaceParamText('0',filename);
			GetApp().NVI_ShowModalAlertWindow(message1,message2);
			break;
		}
		//�g�[�^���̃t�@�C���T�C�Y��200MB�𒴂���ꍇ�́A���[�_���G���[�\�����āA�t�@�C�����J���Ȃ�
	  case kFileScreenError_TotalFileSizeOver:
		{
			AText	message1, message2;
			message1.SetLocalizedText("CantOpen_Title");
			message2.SetLocalizedText("CantOpen_TotalFileSize");
			message2.ReplaceParamText('0',filename);
			GetApp().NVI_ShowModalAlertWindow(message1,message2);
			break;
		}
		//�G�C���A�X������A���łɓ����t�@�C���̃h�L�������g�����݂��Ă����ꍇ�́A���̃h�L�������g��I��
	  case kFileScreenError_DocumentDuplicatedAfterLinkResolved:
		{
			GetApp().SPI_GetTextDocumentByID(mDuplicatedDocumentID).NVI_RevealDocumentWithAView();
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//�h�L�������g�����
	GetApp().SPI_ExecuteCloseTextDocument(GetObjectID());
}

//#699
/**
�h�L�������g��������ԑJ�ځiView��activate���ꂽ���j
@param inCalculateWrapInThread false�̂Ƃ��́A��Ƀ��C���X���b�h���Ōv�Z����idiff�h�L�������g�p�j
*/
void	ADocument_Text::SPI_TransitInitState_ViewActivated()
{
	//������ԁi�X�N���[���ς݁j�łȂ��ꍇ�̓G���[
	if( SPI_GetDocumentInitState() != kDocumentInitState_Initial_FileScreened )
	{
		_ACError("",NULL);
		return;
	}
	
	//==================�X�N���[�j���O���ʃ`�F�b�N==================
	if( mFileScreenError != kFileScreenError_NoError )
	{
		//#962
		//���̊֐��̓E�C���h�E��NVI_Create()������R�[�������̂ŁA�����Ńh�L�������g���폜����ƁA
		//���̌�̏����̐�����������Ȃ��Ȃ�B���̂��߁A�G���[�������Ă��A��Ԃ�ω������Ȃ��ŁA���̂܂܏I������B
		//�i�����C�x���g�𑗐M���A���̃C�x���g�����ŃG���[�_�C�A���O��\�����AOK�������ꂽ��h�L�������g�폜����B�j
		
		//�����C�x���g���M
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(GetObjectID());
		ABase::PostToMainInternalEventQueue(kInternalEvent_DocumentFileScreenError,
											GetObjectID(),0,GetEmptyText(),objectIDArray);
		//�I��
		return;
	}
	//#831
	//���[�h���s
	if( mLoaded == false )
	{
		LoadDocumentFile();
	}
	
	//�s�֘A�f�[�^������
	InitLineInfoData();
	
	//=========================�܂�Ԃ��v�Z=========================
	//�s�܂�Ԃ��v�Z�i�ŏ���200�s�̂݌v�Z����B�c�肪����΁A�X���b�h�Ɍv�Z�˗��j
	AText	fontname;
	SPI_GetFontName(fontname);
	AItemCount	limitLineCount = kIndex_Invalid;//mCalculateWrapInThread��false�̂Ƃ��́A�S�s�ɂ��āA���Ő܂�Ԃ��v�Z����B
	if( mCalculateWrapInThread == true )
	{
		//mCalculateWrapInThread��true�̂Ƃ��́AkMainThreadWrapCalculateLineCount�s�܂ł��A�܂�Ԃ��v�Z����B�c��̓X���b�h�B
		limitLineCount = kMainThreadWrapCalculateLineCount;
	}
	ANumber	viewWidth = AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetTextViewWidth();
	AItemCount	completedTextLen = mTextInfo.CalcLineInfoAll(mText,true,limitLineCount,
				fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
				SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),viewWidth,
				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters));
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("Document wrap 200 lines calculated.",this);
	
	//�S�e�L�X�g�v�Z�ς݂��ǂ����𔻒�
	if( completedTextLen < mText.GetItemCount() )
	{
		//==================�s�܂�Ԃ��v�Z���������i200�s�ȏ�j==================
		
		//�e�L�X�g�̍Ō�ɉ��s�R�[�h�����邩�ǂ������擾�i�X���b�h�v�Z�Ώۍs�ɐݒ肷�邽�߁j
		ABool	existLineEnd = false;
		if( mText.GetItemCount()-1 >= 0 )
		{
			existLineEnd = (mText.Get(mText.GetItemCount()-1)==kUChar_LineEnd);
		}
		//�X���b�h�v�Z�Ώۍs����
		mTextInfo.AddLineInfo(completedTextLen,mText.GetItemCount()-completedTextLen,existLineEnd);
		//View�̍s�f�[�^������
		SPI_InitLineImageInfo();
		//�X���b�h�Ɍv�Z�w��
		StartWrapCalculator(mTextInfo.GetLineCount()-1);
		//�s�܂�Ԃ��v�Z����Ԃ֏�ԑJ��
		SPI_SetDocumentInitState(kDocumentInitState_WrapCalculating);
	}
	else
	{
		//==================�s�܂�Ԃ��v�Z�������i200�s�����j==================
		
		//View�̍s�f�[�^������
		SPI_InitLineImageInfo();
		//�s�܂�Ԃ��v�Z��������ԑJ��
		SPI_TransitInitState_WrapCalculateCompleted();
	}
	
	//�`��X�V
	SPI_RefreshTextViews();
	
	//#750
	//�e�L�X�g�}�[�J�[������
	SPI_ApplyTextMarker();
	
}

//#699
/**
�h�L�������g��������ԑJ�ځi�s�܂�Ԃ��v�Z�������j
*/
void	ADocument_Text::SPI_TransitInitState_WrapCalculateCompleted()
{
	//�s�`�F�b�N�f�[�^�𔽉f #842
	ApplyDocPref_LineCheckedDate();
	//���@�F���J�n
	StartSyntaxRecognizer(0);//#698
	//���@�F������Ԃ֏�ԑJ��
	SPI_SetDocumentInitState(kDocumentInitState_SyntaxRecognizing);
	
	//TextView�ɍs�܂�Ԃ��v�Z������ʒm�i�\�񂵂��I���ʒu�����f�����j
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_DoWrapCalculated();
	}
	//#142
	//�e�L�X�g�J�E���g�E�C���h�E�X�V
	UpdateDocInfoWindows();
	
	//�C�x���g���X�i�[���s #994
	SPI_ExecuteEventListener("onOpened",GetEmptyText());
}

//#699
/**
�h�L�������g��������ԑJ�ځi���@�F���������j
*/
void	ADocument_Text::SPI_TransitInitState_SyntaxRecognizeCompleted()
{
	//ImportFileData�X�V�iSPI_DoSyntaxRecognizerPaused()����CopySyntaxRecognizerResult()���R�[���������ɁAmImportFileDataDirty�t���O���ݒ肳��Ă���B�j
	UpdateImportFileData();
	mImportFileDataDirty = false;
	//View�̍s���X�V
	SPI_InitLineImageInfo();//#450
	//�܂肽���݃f�[�^��Textview�ɔ��f #450
	ApplyDocPref_CollapsedLine();
	
	//#664 SPI_DoSCMStatusUpdated()�������InitViewData()���R�[�����ꂽ�Ƃ��́A
	//mSCMFileState��kDiffTargetMode_None�̂܂܂ɂȂ��Ă���B
	//SPI_UpdateDiffDisplayData()�����s���邽�߂ɂ́A������SCMFileState���擾���Ă����K�v������B
	//�������ASCM�t�H���_��update status���܂��̂Ƃ��́ASPI_DoSCMStatusUpdated()���R�[�����ꂽ�^�C�~���O�ŁA
	//mSCMFileState��ݒ肵�ASPI_UpdateDiffDisplayData()�����s�����B
	AFileAcc	file;
	NVI_GetFile(file);
	mSCMFileState = GetApp().SPI_GetSCMFolderManager().GetFileState(file);
	
	//#81
	//�f�t�H���g��Diff���[�h��ݒ�
	//�D�揇��
	//�P�D��r��t�H���_���ݒ肳��Ă���A��r��t�@�C�������݂���ꍇ
	//�Q�D���|�W�g��
	if( mDiffTargetMode == kDiffTargetMode_None )
	{
		SetDefaultDiffMode();
	}
	
	//Diff�\���f�[�^�X�V
	SPI_UpdateDiffDisplayData();
	
	//#379 �s�܂�Ԃ��\�����f
	SPI_RefreshTextViews();
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::InitViewData() ended.",this);
	
	//������������Ԃ֏�ԑJ��
	SPI_SetDocumentInitState(kDocumentInitState_Completed);
	
	//#912
	//�C���f���g���������ݒ�
	SPI_GuessIndentCharacterType();
	
	//
	//GetApp().SPI_ReserveTimedExecution(kTimedExecutionType_TransitNextDocumentToWrapCalculatingState,3000);
}

//#379
/**
View�o�^��ɃR�[�������
*/
void	ADocument_Text::NVIDO_ViewRegistered( const AViewID inViewID )
{
	//���[�J�����ʎq�f�[�^�ǉ��i���[�J�����ʎq�f�[�^��view���j
	AStMutexLocker	locker(mCurrentLocalIdentifierMutex);
	mCurrentLocalIdentifierListArray.AddNewObject();
	mCurrentLocalIdentifierStartLineIndexArray.Add(kIndex_Invalid);
	mCurrentLocalIdentifierEndLineIndexArray.Add(kIndex_Invalid);
}

//#893
/**
View�o�^������ɃR�[�������
*/
void	ADocument_Text::NVIDO_ViewUnregistered( const AViewID inViewID )
{
	AIndex	index = mViewIDArray.Find(inViewID);
	
	//���[�J�����ʎq�f�[�^�폜�i���[�J�����ʎq�f�[�^��view���j
	AStMutexLocker	locker(mCurrentLocalIdentifierMutex);
	mCurrentLocalIdentifierListArray.Delete1Object(index);
	mCurrentLocalIdentifierStartLineIndexArray.Delete1(index);
	mCurrentLocalIdentifierEndLineIndexArray.Delete1(index);
}

//#379
/**
�r���[���A�N�e�B�u�ɂȂ�Ƃ��ɃR�[�������
@param inCalculateWrapInThread false�̂Ƃ��́A��Ƀ��C���X���b�h���Ōv�Z����idiff�h�L�������g�p�j
*/
void	ADocument_Text::SPI_DoViewActivating( const ABool inCalculateWrapInThread )
{
	/*#699
	if( mViewDataInited == false )
	{
		InitViewData(inViewID);
	}
	*/
	//��Ԃ�initial��screened�̏ꍇ�̂ݎ��s����Bscreening���̓X���b�h��screening���ł���_�u����screen���ԑJ�ڂ����s���Ȃ����߂Ɏ��s���Ȃ��悤�ɂ���B
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial || SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		//==================�h�L�������g��������ԑJ��==================
		//#932 SPI_TransitInitState_ViewActivated(inCalculateWrapInThread);
		mCalculateWrapInThread = inCalculateWrapInThread;
		if( SPI_GetDocumentInitState() == kDocumentInitState_Initial )
		{
			//------------------���X�N���[���̏ꍇ���ʏ탍�[�h���A����сAdiff target�h�L�������g�i���|�W�g���j��------------------
			//�X�N���[�����s�v���i�X���b�h�ł̃X�N���[���������SPI_TransitInitState_ViewActivated()���s�B
			StartScreeningForLoadDocumentFile();
		}
		else
		{
			//------------------�X�N���[���ς݂̏ꍇ��diff target�h�L�������g�i�t�@�C���j��------------------
			//diff target�h�L�������g�̏ꍇ�́A����SPI_ScreenAndLoadDocumentFile()�o�R�ŃX�N���[�j���O���s�ς݂Ȃ̂ŁA
			//���ڏ�ԑJ�ڎ��s����B
			SPI_TransitInitState_ViewActivated();
		}
		
		//==================�t�H���_���x���\��==================
		//�t�@�C���p�X�擾
		AText	filepath;
		NVI_GetFilePath(filepath);
		if( filepath.GetItemCount() > 0 )
		{
			//�h�L�������g�̃t�@�C���p�X�ɑΉ�����A�t�H���_���x���擾
			ATextArray	folderpathArray, labeltextArray;
			GetApp().SPI_FindFolderLabel(filepath,folderpathArray,labeltextArray);
			AItemCount	itemCount = folderpathArray.GetItemCount();
			for( AIndex i = 0; i < itemCount; i++ )
			{
				//�ŏ��̃r���[��notification�|�b�v�A�b�v�\��
				AText	folderpath, labeltext;
				folderpathArray.Get(i,folderpath);
				labeltextArray.Get(i,labeltext);
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetPopupNotificationWindow().
						SPI_GetNotificationView().SPI_SetNotification_FolderLabel(folderpath,labeltext);
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_ShowNotificationPopupWindow(true);
			}
		}
		
	}
}

//#450
/**
�܂肽���ݏ�Ԃ�DocPref����ǂݍ��݁Atext view�֔��f
*/
void	ADocument_Text::ApplyDocPref_CollapsedLine()
{
	if( mViewIDArray.GetItemCount() > 0 )
	{
		//�܂肽���݂�K�p�idoc pref�A����сA�f�t�H���g�K�p�j
		ANumberArray	collapsedLines;
		collapsedLines.SetFromObject(mDocPrefDB.GetData_NumberArrayRef(ADocPrefDB::kFolding_CollapsedLineIndex));
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_CollapseLines(collapsedLines,true);
	}
}

//#450
/**
�܂肽���ݏ�Ԃ�DocPref�֕ۑ�
*/
void	ADocument_Text::SaveToDocPref_CollapsedLine()
{
	if( mViewIDArray.GetItemCount() > 0 )
	{
		ANumberArray	collapsedLines;
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetFoldingCollapsedLines(collapsedLines);
		mDocPrefDB.SetData_NumberArray(ADocPrefDB::kFolding_CollapsedLineIndex,collapsedLines);
	}
}

//#842
/**
�s�`�F�b�N������DocPref����ǂݍ��݁A���f
*/
void	ADocument_Text::ApplyDocPref_LineCheckedDate()
{
	const ANumberArray&	lineCheckedDateArray_LineIndex = mDocPrefDB.GetData_NumberArrayRef(ADocPrefDB::kLineCheckedDate_LineIndex);
	const ANumberArray&	lineCheckedDateArray_Date = mDocPrefDB.GetData_NumberArrayRef(ADocPrefDB::kLineCheckedDate_Date);
	AItemCount	lineCheckedDateArrayCount = lineCheckedDateArray_LineIndex.GetItemCount();
	for( AIndex index = 0; index < lineCheckedDateArrayCount; index++ )
	{
		SPI_SetLineCheckedDate(lineCheckedDateArray_LineIndex.Get(index),lineCheckedDateArray_Date.Get(index));
	}
}

//#842
/**
�s�`�F�b�N������DocPref�֕ۑ�
*/
void	ADocument_Text::SaveToDocPref_LineCheckedDate()
{
	mDocPrefDB.DeleteAll_NumberArray(ADocPrefDB::kLineCheckedDate_LineIndex);
	mDocPrefDB.DeleteAll_NumberArray(ADocPrefDB::kLineCheckedDate_Date);
	AItemCount	lineCount = SPI_GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		ANumber	date = SPI_GetLineCheckedDate(lineIndex);
		if( date != 0 )
		{
			mDocPrefDB.Add_NumberArray(ADocPrefDB::kLineCheckedDate_LineIndex,lineIndex);
			mDocPrefDB.Add_NumberArray(ADocPrefDB::kLineCheckedDate_Date,date);
		}
	}
}

#if IMPLEMENTATION_FOR_MACOSX
void	ADocument_Text::SPI_SetODBMode( const OSType inODBSender, const AText& inODBToken, const AText& inODBCustomPath )
{
	mODBMode = true;
	mODBSender = inODBSender;
	mODBToken.SetText(inODBToken);
	mODBCustomPath.SetText(inODBCustomPath);
	AFileAcc	file;
	NVI_GetFile(file);
	mODBFile.CopyFrom(file);
	//#379 UpdateInfoHeader();
	//#725 GetApp().SPI_UpdateInfoHeader(GetObjectID());//#379
}
#endif

void	ADocument_Text::SPI_SetTemporaryTitle( const AText& inText )
{
	mTemporaryTitle.SetText(inText);
	//
	//#922 GetApp().SPI_UpdateWindowMenu();
	//�t�@�C�����X�g�E�C���h�E�X�V
	//#725 GetApp().SPI_UpdateFileListWindow_DocumentOpened(GetObjectID());
	GetApp().SPI_UpdateFileListWindows(kFileListUpdateType_DocumentOpened,GetObjectID());//#725
}

//#379
/**
�w��E�C���h�E�ȊO�̃E�C���h�E�����݂��邩�ǂ�����Ԃ�
TryClose()������R�[�������i�ق��ɑ��݂��Ȃ��ꍇ�̂�AskSave���̏����j
*/
ABool	ADocument_Text::SPI_ExistAnotherWindow( const AWindowID inWindowID )
{
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AWindowID	textWindowID = AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetTextWindowID();
		if( textWindowID != kObjectID_Invalid )
		{
			if( textWindowID != inWindowID )   return true;
		}
	}
	return false;
}

#pragma mark ===========================

#pragma mark ---DocPref
//#898

/**
doc pref�̏������i���[�h�ɏ]���������l��ݒ�j
*/
void	ADocument_Text::InitDocPref( const AModeIndex inModeIndex )
{
	mDocPrefDB.InitPref(inModeIndex);
}

/**
doc pref�����[�h
*/
AModeIndex	ADocument_Text::LoadDocPref()
{
	//autobackup/docpref�p�̃p�X�e�L�X�g���擾
	AText	docpathtext;
	GetPathTextForAutoBackupAndDocPref(docpathtext);
	
	//doc pref�����[�h
	//#902 APrintPagePrefData	printpref;
	AModeIndex	modeIndex = mDocPrefDB.LoadPref(docpathtext);//#902 ,true,printpref);
	//#902 NVM_GetPrintImp().SetPrintPagePrefData(printpref);
	return modeIndex;
}

/**
doc pref�֕ۑ�
*/
void	ADocument_Text::SaveDocPref()
{
	//#450
	//------------------�s�܂肽���ݏ�Ԃ�DocPref�֕ۑ�------------------
	SaveToDocPref_CollapsedLine();
	//#842
	//------------------�s�`�F�b�N������DocPref�֕ۑ�------------------
	SaveToDocPref_LineCheckedDate();
	
	/*#902
	//------------------����ݒ���擾------------------
	APrintPagePrefData	printpref;
	NVM_GetPrintImp().GetPrintPagePrefData(printpref);
	*/
	
	//------------------DocPref���t�@�C���ۑ�------------------
	//autobackup/docpref�p�̃p�X�e�L�X�g���擾
	AText	docpathtext;
	GetPathTextForAutoBackupAndDocPref(docpathtext);
	//doc pref��ۑ�
	mDocPrefDB.SavePref(docpathtext);//#902 ,true,printpref);
	
}

/**
���[�h�ς݂Ȃ�doc pref�ۑ�
*/
void	ADocument_Text::SPI_SaveDocPrefIfLoaded()
{
	if( mLoaded == true )
	{
		//�E�C���h�E�擾
		AWindowID	firstWindowID = SPI_GetFirstWindowID();
		if( firstWindowID != kObjectID_Invalid )
		{
			//�ۑ��O����
			GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_SavePreProcess(GetObjectID());
			//doc pref�ۑ�
			SaveDocPref();
		}
	}
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//
void	ADocument_Text::EVTDO_StartPrintMode( const ALocalRect& inRect, const ANumber inPaperWidth, const ANumber inPaperHeight )//#558
{
	mPrintRect = inRect;
	mPrintPaperWidth = inPaperWidth;//#558
	mPrintPaperHeight = inPaperHeight;//#558
	ALocalRect	textRect;
	GetPrintRect_Text(textRect);
	//win AFont	font;
	AText	fontname;//win
	AFloatNumber	fontsize;
	GetPrintFont_Text(fontname,fontsize);//win
	AWrapMode	wrapMode = SPI_GetWrapMode();
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_ForceWordWrap) == true && wrapMode == kWrapMode_NoWrap )//#1161
	{
		wrapMode = kWrapMode_WordWrap;
	}
	mTextInfoForPrint.SetMode(SPI_GetModeIndex());
	AArray<AUniqID>	addedIdentifier;
	AArray<AIndex>		addedIdentifierLineIndex;
	//#558
	//��������擾
	ANumber	width = textRect.right-textRect.left;
	if( SPI_GetVerticalMode() == true )
	{
		width = textRect.bottom-textRect.top;
	}
	//�s�܂�Ԃ��v�Z
	mTextInfoForPrint.CalcLineInfoAll(mText,true,kIndex_Invalid,fontname,fontsize,true,//win #699
			SPI_GetTabWidth(),SPI_GetIndentWidth(),wrapMode,SPI_GetWrapLetterCount(),width,
			GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters));
	SPI_InitLineImageInfo();//#450
	
	ABool	importChanged;
	AFileAcc	file;//R0000
	NVI_GetFile(file);//R0000
	ABool	abortFlag = false;
	AText	url;//#998
	SPI_GetURL(url);//#998
	mTextInfoForPrint.RecognizeSyntaxAll(mText,url,
										 addedIdentifier,addedIdentifierLineIndex,importChanged,abortFlag);//R0000 #698 #998
}

//
void	ADocument_Text::EVTDO_EndPrintMode()
{
	//#695 AArray<AUniqID>	deletedIdentifier;
	mTextInfoForPrint.DeleteLineInfoAll(/*#695 deletedIdentifier*/);
}

//
AItemCount	ADocument_Text::EVTDO_GetPrintPageCount() 
{
	ALocalRect	textRect;
	GetPrintRect_Text(textRect);
	//win AFont	font;
	AText	fontname;//win
	AFloatNumber	fontsize;
	GetPrintFont_Text(fontname,fontsize);//win
	NVM_GetPrintImp().SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);//win
	NVM_GetPrintImp().SetVerticalMode(SPI_GetVerticalMode());//#558
	ANumber	lineHeight, lineAscent;
	NVM_GetPrintImp().GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
	lineHeight += GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kPrintOption_LineMargin);
	//�y�[�W���s���v�Z
	AItemCount	lineCountInPage = (textRect.bottom-textRect.top)/lineHeight;
	if( SPI_GetVerticalMode() == true )//#558
	{
		//�c�����̏ꍇ�̃y�[�W���s���v�Z
		lineCountInPage = (textRect.right-textRect.left)/lineHeight;
	}
	//
	if( mPrintStartParagraphIndex == kIndex_Invalid )//#524
	{
		return (mTextInfoForPrint.GetLineCount()+lineCountInPage-1)/lineCountInPage;
	}
	else//#524
	{
		AIndex	printRangeStartLineIndex = mTextInfoForPrint.GetLineIndexByParagraphIndex(mPrintStartParagraphIndex);
		AIndex	printRangeEndLineIndex = mTextInfoForPrint.GetLineIndexByParagraphIndex(mPrintEndParagraphIndex);
		return (printRangeEndLineIndex-printRangeStartLineIndex+lineCountInPage-1)/lineCountInPage;
	}
}

//
void	ADocument_Text::EVTDO_PrintPage( const AIndex inPageIndex ) 
{
	//#1316
	//������J���[�X�L�[�����[�h�ɂ���
	GetApp().GetAppPref().SetPrintColorSchemeMode(true);
	
	//#524
	AIndex	printRangeStartLineIndex = 0;
	AIndex	printRangeEndLineIndex = mTextInfoForPrint.GetLineCount();
	if( mPrintStartParagraphIndex != kIndex_Invalid )
	{
		printRangeStartLineIndex = mTextInfoForPrint.GetLineIndexByParagraphIndex(mPrintStartParagraphIndex);
		printRangeEndLineIndex = mTextInfoForPrint.GetLineIndexByParagraphIndex(mPrintEndParagraphIndex);
	}
	//
	ANumber	lineHeight, lineAscent;
	//Text
	{
		//�c�������[�h�ݒ� #558
		NVM_GetPrintImp().SetVerticalMode(SPI_GetVerticalMode());
		//
		ALocalRect	textRect;
		GetPrintRect_Text(textRect);
		//win AFont	font;
		AText	fontname;//win
		AFloatNumber	fontsize;
		GetPrintFont_Text(fontname,fontsize);//win
		NVM_GetPrintImp().SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);//win
		//#1131
		//�S�p�X�y�[�X���擾�i���e�p���}�X�ڗp�j
		AText	zenkakuSpaceText;
		zenkakuSpaceText.SetLocalizedText("Text_ZenkakuSpace");
		ANumber	zenkakuSpaceWidth = static_cast<ANumber>(NVM_GetPrintImp().GetDrawTextWidth(zenkakuSpaceText));
		//
		NVM_GetPrintImp().GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
		ANumber	letterHeight = lineHeight;//#1131
		lineHeight += GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kPrintOption_LineMargin);
		//�y�[�W���s���v�Z
		AItemCount	lineCountInPage = (textRect.bottom-textRect.top)/lineHeight;
		if( SPI_GetVerticalMode() == true )//#558
		{
			//�c�����̏ꍇ�̃y�[�W���s���v�Z
			lineCountInPage = (textRect.right-textRect.left)/lineHeight;
		}
		//
		AIndex	startLineIndex = inPageIndex * lineCountInPage + printRangeStartLineIndex;//#524
		AIndex	endLineIndex = (inPageIndex+1) * lineCountInPage + printRangeStartLineIndex;//#524
		if( endLineIndex > mTextInfoForPrint.GetLineCount() )   endLineIndex = mTextInfoForPrint.GetLineCount();
		for( AIndex lineIndex = startLineIndex; lineIndex < endLineIndex; lineIndex++ )
		{
			//#524
			if( lineIndex >= printRangeEndLineIndex )   break;//#524
			//
			CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
			ATextPoint	tmp = {0,0};
			AAnalyzeDrawData	tmp2;//RC1
			GetTextDrawDataWithStyle(mText,mTextInfoForPrint,lineIndex,textDrawData,false,tmp,tmp,tmp2,kObjectID_Invalid);//#695
			//�������ɕϊ�
			if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kPrintColored) == true && 
			GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kPrintBlackForWhite) == true )
			{
				for( AIndex i = 0; i < textDrawData.attrColor.GetItemCount(); i++ )
				{
					AColor	color = textDrawData.attrColor.Get(i);
					if( AColorWrapper::ChangeBlackIfWhite(color) == true )
					{
						textDrawData.attrColor.Set(i,color);
					}
				}
			}
			//�F�����ɕϊ�
			if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kPrintColored) == false )
			{
				for( AIndex i = 0; i < textDrawData.attrColor.GetItemCount(); i++ )
				{
					textDrawData.attrColor.Set(i,kColor_Black);
				}
			}
			//�e�L�X�g�`��rect�v�Z
			ALocalRect	drawRect = textRect;
			if( SPI_GetVerticalMode() == false )
			{
				drawRect.top		= textRect.top + (lineIndex-startLineIndex) * lineHeight;
				drawRect.bottom		= textRect.top + (lineIndex-startLineIndex+1) * lineHeight;
			}
			else
			{
				//�c�����̏ꍇ�̃e�L�X�g�`��rect�v�Z #558
				drawRect.top		= (lineIndex-startLineIndex+1) * lineHeight;
				drawRect.bottom		= (lineIndex-startLineIndex) * lineHeight;
				drawRect.left		= textRect.top;
				drawRect.right		= textRect.bottom;
			}
			//#1131
			//���e�p���}�X�ڈ��
			ABool	drawMasume = (GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kDisplayGenkoyoshi) == true);
			if( drawMasume == true && false )//#1171
			{
				AIndex	start 	= 0;
				AIndex	end 	= (drawRect.right-drawRect.left) / zenkakuSpaceWidth;
				AWrapMode	wrapMode = SPI_GetWrapMode();
				if( wrapMode == kWrapMode_WordWrapByLetterCount || wrapMode == kWrapMode_LetterWrapByLetterCount )//#1113
				{
					end = SPI_GetWrapLetterCount()/2;
				}
				ALocalPoint	spt, ept;
				for( AIndex i = start; i <= end; i++ )
				{
					spt.x = drawRect.left + zenkakuSpaceWidth * i;
					spt.y = drawRect.top;
					ept.x = spt.x;
					ept.y = spt.y + letterHeight -1;
					NVM_GetPrintImp().DrawLine(spt,ept,kColor_Gray80Percent);
				}
				//
				spt.x = drawRect.left;
				spt.y = drawRect.top;
				ept.x = drawRect.left + zenkakuSpaceWidth * end;
				ept.y = drawRect.top;
				NVM_GetPrintImp().DrawLine(spt,ept,kColor_Gray80Percent);
				//
				spt.y += letterHeight;
				ept.y += letterHeight;
				NVM_GetPrintImp().DrawLine(spt,ept,kColor_Gray80Percent);
			}
			//�e�L�X�g���
			NVM_GetPrintImp().DrawText(drawRect,textDrawData);
		}
	}
	//PageNumber
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_PrintPageNumber) == true )
	{
		//�c�������[�h���� #558
		NVM_GetPrintImp().SetVerticalMode(false);
		//
		ALocalRect	pageNumberRect;
		GetPrintRect_PageNumber(pageNumberRect);
		//win AFont	font;
		AText	fontname;//win
		AFloatNumber	fontsize;
		GetPrintFont_PageNumber(fontname,fontsize);//win
		NVM_GetPrintImp().SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);//win
		AText	text;
		text.SetFormattedCstring("%d",inPageIndex+1);
		NVM_GetPrintImp().DrawText(pageNumberRect,text,kJustification_Right);
	}
	//LineNumber
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_PrintLineNumber) == true )
	{
		//�c�������[�h�ݒ� #558
		NVM_GetPrintImp().SetVerticalMode(SPI_GetVerticalMode());
		//
		ALocalRect	rect;
		GetPrintRect_LineNumber(rect);
		//win AFont	font;
		AText	fontname;//win
		AFloatNumber	fontsize;
		GetPrintFont_LineNumber(fontname,fontsize);//win
		NVM_GetPrintImp().SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);//win
		ANumber	lineNumberHeight, lineNumberAscent;
		NVM_GetPrintImp().GetMetricsForDefaultTextProperty(lineNumberHeight,lineNumberAscent);
		//�y�[�W���s���v�Z
		AItemCount	lineCountInPage = (rect.bottom-rect.top)/lineHeight;
		if( SPI_GetVerticalMode() == true )
		{
			//�c�����̏ꍇ�̃y�[�W���s���v�Z #558
			lineCountInPage = (rect.right-rect.left)/lineHeight;
		}
		//
		AIndex	startLineIndex = inPageIndex * lineCountInPage + printRangeStartLineIndex;//#524
		AIndex	endLineIndex = (inPageIndex+1) * lineCountInPage + printRangeStartLineIndex;//#524
		if( endLineIndex > mTextInfoForPrint.GetLineCount() )   endLineIndex = mTextInfoForPrint.GetLineCount();
		for( AIndex lineIndex = startLineIndex; lineIndex < endLineIndex; lineIndex++ )
		{
			//#524
			if( lineIndex >= printRangeEndLineIndex )   break;//#524
			//#334
			ABool	shouldPrint = true;
			AIndex	printLineIndex = lineIndex;
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintEachParagraphAlways) == true ||
						GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kDisplayEachLineNumber_AsParagraph) == true )
			{
				//�i���ԍ�����̏ꍇ
				shouldPrint = (mTextInfoForPrint.GetCurrentParagraphStartLineIndex(lineIndex) == lineIndex);
				printLineIndex = mTextInfoForPrint.GetParagraphIndexByLineIndex(printLineIndex);
			}
			//
			if( shouldPrint == true )//#334
			{
				//�s�ԍ��\��rect�v�Z
				ALocalRect	drawRect = rect;
				if( SPI_GetVerticalMode() == false )
				{
					drawRect.top		= rect.top + (lineIndex-startLineIndex) * lineHeight - lineNumberAscent + lineAscent;//�x�[�X���C�����킹��
					drawRect.bottom		= rect.top + (lineIndex-startLineIndex+1) * lineHeight - lineNumberAscent + lineAscent;
					drawRect.right		-= NVM_GetPrintImp().GetXInterfaceSizeByMM(mPrintLineNumberRightMarginMM);//win 080729 kPrintLineNumberRightMargin;
				}
				else
				{
					//�c�����̏ꍇ�̍s�ԍ��\��rect�v�Z #558
					drawRect.top		= (mPrintPaperWidth-rect.right) + (lineIndex-startLineIndex) * lineHeight - lineNumberAscent + lineAscent;//�x�[�X���C�����킹��
					drawRect.bottom		= (mPrintPaperWidth-rect.right) + (lineIndex-startLineIndex+1) * lineHeight - lineNumberAscent + lineAscent;
					drawRect.left		= rect.top;
					drawRect.right		= rect.bottom - NVM_GetPrintImp().GetXInterfaceSizeByMM(mPrintLineNumberRightMarginMM);//win 080729 kPrintLineNumberRightMargin;
				}
				AText	text;
				text.SetFormattedCstring("%d",printLineIndex+1);//#334
				NVM_GetPrintImp().DrawText(drawRect,text,kJustification_Right);
			}
		}
	}
	//FileName
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_PrintFileName) == true )
	{
		//�c�������[�h���� #558
		NVM_GetPrintImp().SetVerticalMode(false);
		//
		ALocalRect	rect;
		GetPrintRect_FileName(rect);
		//win AFont	font;
		AText	fontname;//win
		AFloatNumber	fontsize;
		GetPrintFont_FileName(fontname,fontsize);//win
		NVM_GetPrintImp().SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);//win
		AText	text;
		NVI_GetTitle(text);
		NVM_GetPrintImp().DrawText(rect,text,kJustification_Center);
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_PrintSeparateLine) == true )
		{
			ALocalPoint	spt, ept;
			spt.x = rect.left;
			spt.y = rect.top - NVM_GetPrintImp().GetYInterfaceSizeByMM(mPrintSeparateLineHeightMM);
			ept.x = rect.right;
			ept.y = spt.y;
			NVM_GetPrintImp().DrawLine(spt,ept,kColor_Black,1.0,0.0,0.25);
		}
	}
	//#1393
	else
	{
		//�c�������[�h����
		NVM_GetPrintImp().SetVerticalMode(false);
	}
	
	//�e�X�g
	/*ALocalPoint	spt, ept;
	spt.x = mPrintRect.left;
	spt.y = mPrintRect.top;
	ept.x = mPrintRect.left;
	ept.y = mPrintRect.bottom;
	NVM_GetPrintImp().DrawLine(spt,ept,kColor_Black,1.0,0.0,0.25);
	spt.x = mPrintRect.right;
	spt.y = mPrintRect.top;
	ept.x = mPrintRect.right;
	ept.y = mPrintRect.bottom;
	NVM_GetPrintImp().DrawLine(spt,ept,kColor_Black,1.0,0.0,0.25);
	1mm���炢�덷���o�邪�A�܂��悵�Ƃ���
	72dpi���W�n�ɕϊ����Ă��邩��H
	*/
	
	//#1316
	//������J���[�X�L�[�����[�h����������
	GetApp().GetAppPref().SetPrintColorSchemeMode(false);
}

//�e�L�X�g����t�H���g���擾
void	ADocument_Text::GetPrintFont_Text( /*win AFont& outFont*/AText& outFontName, AFloatNumber& outFontSize )
{
	//win outFont = SPI_GetFont();
	SPI_GetFontName(outFontName);//win
	outFontSize = SPI_GetFontSize();
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_UsePrintFont) == true )
	{
		//win outFont = mDocPrefDB.GetData_Font(AAppPrefDB::kPrintOption_PrintFont_compat);
		GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kPrintOption_PrintFontName,outFontName);//win
		outFontSize = GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintOption_PrintFontSize);
	}
}

//�e�L�X�g����G���A��Rect���擾
void	ADocument_Text::GetPrintRect_Text( ALocalRect& outRect )
{
	outRect = mPrintRect;
	/*win 080729 outRect.left += kPrintLeftMargin;
	outRect.top += kPrintTopMargin;
	outRect.right -= kPrintRightMargin;
	outRect.bottom -= kPrintBottomMargin;
	if( mDocPrefDB.GetData_Bool(AAppPrefDB::kPrintOption_PrintPageNumber) == true )
	{
		outRect.top += kPrintPageNumberHeight;
	}*/
	//�e�s�s�ԍ����������ꍇ�́A�s�ԍ��G���A�̕�����left�����炷
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_PrintLineNumber) == true )
	{
		if( SPI_GetVerticalMode() == false )
		{
			outRect.left += NVM_GetPrintImp().GetXInterfaceSizeByMM(mPrintLineNumberWidthMM);//win 080729 kPrintLineNumberWidth;
		}
		else
		{
			//�c�������[�h�̏ꍇ #558
			outRect.top += NVM_GetPrintImp().GetXInterfaceSizeByMM(mPrintLineNumberWidthMM);
		}
	}
	//�t�@�C�������������ꍇ�́A�t�@�C�����G���A�{�������̍�������bottom�����炷
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_PrintFileName) == true )
	{
		outRect.bottom -= NVM_GetPrintImp().GetYInterfaceSizeByMM(mPrintFileNameHeightMM) 
				+ NVM_GetPrintImp().GetYInterfaceSizeByMM(mPrintSeparateLineHeightMM);//win 080729 kPrintFileNameHeight + kPrintSeparateLineHeight;
	}
}

//�y�[�W�ԍ��̃t�H���g���擾
void	ADocument_Text::GetPrintFont_PageNumber( /*win AFont& outFont*/AText& outFontName, AFloatNumber& outFontSize )
{
	//win outFont = mDocPrefDB.GetData_Font(AAppPrefDB::kPrintOption_PageNumberFont);
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kPrintOption_PageNumberFontName,outFontName);//win
	outFontSize = GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintOption_PageNumberFontSize);
}

//�y�[�W�ԍ��G���A��Rect���擾
void	ADocument_Text::GetPrintRect_PageNumber( ALocalRect& outRect )
{
	outRect = mPrintRect;
	/*win 080729 outRect.left += kPrintLeftMargin;
	outRect.top += kPrintTopMargin;
	outRect.right -= kPrintRightMargin;
	outRect.bottom -= kPrintBottomMargin;*/
}

//�e�s�s�ԍ�����̃t�H���g���擾
void	ADocument_Text::GetPrintFont_LineNumber( /*win AFont& outFont*/AText& outFontName, AFloatNumber& outFontSize )
{
	GetPrintFont_Text(outFontName,outFontSize);//win
	//win outFont = mDocPrefDB.GetData_Font(AAppPrefDB::kPrintOption_LineNumberFont_compat);
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kPrintOption_LineNumberFontName,outFontName);//win
	outFontSize = GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintOption_LineNumberFontSize);//#1401
}

//�e�s�s�ԍ�����G���A��Rect���擾
void	ADocument_Text::GetPrintRect_LineNumber( ALocalRect& outRect )
{
	GetPrintRect_Text(outRect);
	if( SPI_GetVerticalMode() == false )
	{
		outRect.left = mPrintRect.left;//win 080729  + kPrintLeftMargin;
		outRect.right = mPrintRect.left + NVM_GetPrintImp().GetXInterfaceSizeByMM(mPrintLineNumberWidthMM);//win 080729 kPrintLineNumberWidth;
	}
	else
	{
		//�c�������[�h�̏ꍇ #558
		outRect.top = mPrintRect.top;
		outRect.bottom = mPrintRect.top + NVM_GetPrintImp().GetXInterfaceSizeByMM(mPrintLineNumberWidthMM);
	}
}

//�t�@�C��������̃t�H���g���擾
void	ADocument_Text::GetPrintFont_FileName( /*win AFont& outFont*/AText& outFontName, AFloatNumber& outFontSize )
{
	//win outFont = mDocPrefDB.GetData_Font(AAppPrefDB::kPrintOption_FileNameFont_compat);
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kPrintOption_FileNameFontName,outFontName);//win
	outFontSize = GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintOption_FileNameFontSize);
}

//�t�@�C��������G���A��Rect���擾
void	ADocument_Text::GetPrintRect_FileName( ALocalRect& outRect )
{
	outRect = mPrintRect;
	/*win 080729 outRect.left += kPrintLeftMargin;
	outRect.right -= kPrintRightMargin;
	outRect.bottom -= kPrintBottomMargin;
	outRect.top = outRect.bottom - kPrintFileNameHeight;*/
	outRect.top = outRect.bottom - NVM_GetPrintImp().GetYInterfaceSizeByMM(mPrintFileNameHeightMM);
}

//#524
/**
���
*/
void	ADocument_Text::SPI_Print()
{
	SPI_PrintWithRange(kIndex_Invalid,kIndex_Invalid);
}

//#524
/**
����͈͎w����
*/
void	ADocument_Text::SPI_PrintWithRange( const AIndex inStartParagraph, const AIndex inEndParagraph )
{
	//����͈͐ݒ�
	mPrintStartParagraphIndex = inStartParagraph;
	mPrintEndParagraphIndex = inEndParagraph;
	//���
	AText	title;
	NVI_GetTitle(title);
	NVI_Print(SPI_GetFirstWindowID(),title);
}

/*#379
#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//�E�C���h�E�擾
AWindow_Text&	ADocument_Text::GetTextWindowByIndex( const AIndex inIndex )
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mWindowIDArray.Get(inIndex));
	//#199 return dynamic_cast<AWindow_Text&>(GetApp().NVI_GetWindowByID(mWindowIDArray.Get(inIndex)));
}
*/

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�e�L�X�g�ҏW

//�e�L�X�g�}��
AItemCount	ADocument_Text::SPI_InsertText( const ATextIndex inInsertIndex, AText& ioInsertText,
										   const ABool inRecognizeSyntaxAlwaysInThread, const ABool inDontRedraw, 
										   const ABool inDontUpdateSubWindows )
{
	//#65
	//�}���`�t�@�C���u���p�Ƀh�L�������g���J�����ꍇ�̏���
	if( mForMultiFileReplace == true )
	{
		//�r������
		AStMutexLocker	locker(mTextMutex);
		//�e�L�X�g�֑}��
		mText.InsertText(inInsertIndex,ioInsertText);
		//���s���ύX�s�v�H
		//dirty�ݒ�
		NVI_SetDirty(true);
		return 0;
	}
	
	//
	ATextPoint	textpoint = {0,0};
	SPI_GetTextPointFromTextIndex(inInsertIndex,textpoint);
	return SPI_InsertText(textpoint,ioInsertText,inRecognizeSyntaxAlwaysInThread,inDontRedraw,inDontUpdateSubWindows);
}
AItemCount	ADocument_Text::SPI_InsertText( const ATextPoint& inInsertPoint, AText& ioInsertText,
										   const ABool inRecognizeSyntaxAlwaysInThread, const ABool inDontRedraw, 
										   const ABool inDontUpdateSubWindows )
{
	//#65
	//�}���`�t�@�C���u���p�Ƀh�L�������g���J�����ꍇ�̏����i�����ɂ͗��Ȃ��j
	if( mForMultiFileReplace == true )
	{
		_ACError("",NULL);
		return 0;
	}
	
	//�s�܂�Ԃ��v�Z���͕ҏW�s�� #699
	if( SPI_IsWrapCalculating() == true )
	{
		_ACError("",NULL);
		return 0;
	}
	
	//#897
	//LF��������s�R�[�hCR�ɕϊ��i�������͂̏ꍇ�u�������傤�v��LF�����邽�߁j
	ioInsertText.ReplaceChar(0x0A,kUChar_LineEnd);
	
	//JIS�n�e�L�X�g�G���R�[�f�B���O�̏ꍇ�A���p����0x5C�ɕϊ��B�i�����R�[�h�Ƃ��Ă͏��0x5C�ɂ���B�j
	if( SPI_IsJISTextEncoding() == true )
	{
		AText	yen;
		yen.Add(0xC2);
		yen.Add(0xA5);
		AText	backslash;
		backslash.Add(0x5C);
		ioInsertText.ReplaceText(yen,backslash);
	}
	
	//R0006
	mDisableInlineInputHilite = true;//#187
	ClearDiffDisplay();
	ClearArrowToDef();//RC1
	mDisableInlineInputHilite = false;//#187
	
	//�}���i�������擾 #695
	AItemCount	insertedParagraphCount = ioInsertText.GetCountOfChar(kUChar_LineEnd,0,ioInsertText.GetItemCount());
	
	//#695 �e�L�X�g���̉��s������LineInfo�̃��������蓖�đ����X�e�b�v�ɐݒ�i�s�܂�Ԃ�����̏ꍇ�͂�����������s���ǉ�����邪�A�����X�e�b�v�Ƃ��Ă̐ݒ�Ȃ̂Ō����ɍ����K�v�͂Ȃ��B�j
	//�i������ALineInfo�ɂ�kLineInfoAllocationStep��菬�����͐ݒ肳��Ȃ��j
	mTextInfo.SetLineReallocateStep(insertedParagraphCount);
	
	//R0208
	ABool	containLineEnd = ioInsertText.IsCharContained(kUChar_LineEnd,0,ioInsertText.GetItemCount());
	
	{
		
		//
		ATextPoint	insertPoint = inInsertPoint;
		ATextIndex	insertIndex = mTextInfo.GetTextIndexFromTextPoint(inInsertPoint);
		
		//�}���i���ԍ��擾 #992
		AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(insertPoint.y);
		
		//selection�C���̂���
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_EditPreProcess(insertIndex,ioInsertText.GetItemCount());
		}
		
		//Undo�L�^
		mUndoer.RecordInsertText(insertIndex,ioInsertText.GetItemCount());
		
		//���̒i���J�n�s���擾
		AIndex	nextParagraphStartLineIndex = mTextInfo.GetNextParagraphStartLineIndex(insertPoint.y);
		
		//Dirty�ɂ���B������ړ�#688
		NVI_SetDirty(true);
		
		//================== �e�L�X�g�}�� ==================
		
		{
			AStMutexLocker	locker(mTextMutex);//#598
			//#598 mTextMutex.Lock();
			mText.InsertText(insertIndex,ioInsertText);
			//#598 mTextMutex.Unlock();
		}
		
		//==================�q���g�e�L�X�g�X�V==================
		mTextInfo.UpdateHintTextByInsertText(insertIndex,ioInsertText.GetItemCount());
		
		//================== �s���X�V ==================
		
		//�}���s����̍s��LineStart��}�������������炷
		mTextInfo.ShiftLineStarts(insertPoint.y+1,ioInsertText.GetItemCount());
		
		//�}���s����̍s�̒i��index�����炷#695
		if( insertedParagraphCount > 0 )
		{
			mTextInfo.ShiftParagraphIndex(insertPoint.y+1,insertedParagraphCount);
		}
		
		AIndex	updateStartLineIndex = insertPoint.y;
		//�����ɂ���āA�O�s��LineInfo�v�Z
		//�i�}���s�̑O�s�ɉ��s�R�[�h�������Ƃ��́A���b�v�̊֌W�ŁA�O�s��LineInfo�ɍX�V���K�v�ȏꍇ������j
		if( insertPoint.y > 0 )
		{
			if( SPI_GetLineExistLineEnd(insertPoint.y-1) == false )
			{
				//B0000 �s�܂�Ԃ��v�Z������
				CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
				AIndex	textDrawDataStartOffset = kIndex_Invalid;
				
				AItemCount	oldLen = SPI_GetLineLengthWithLineEnd(insertPoint.y-1);
				AText	fontname;//win
				SPI_GetFontName(fontname);//win
				mTextInfo.CalcLineInfo(mText,insertPoint.y-1,/*win SPI_GetFont()*/fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
							SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),//#117
							GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters),
							textDrawData,textDrawDataStartOffset);//B0000 �s�܂�Ԃ��v�Z������
				if( oldLen != SPI_GetLineLengthWithLineEnd(insertPoint.y-1) )
				{
					updateStartLineIndex = insertPoint.y-1;
				}
			}
		}
		
		//�s���X�V�A�\���X�V
		AIndex	updateEndLineIndex;
		AItemCount insertedLineCount = AdjustLineInfo(/* insertPoint.y*/updateStartLineIndex,nextParagraphStartLineIndex,updateEndLineIndex);
		
		//#546 ���łɍs���X�V����Ă���̂ŁA�ҏW�J�n�s�ԍ����ς���Ă���\��������i�ŏI�s�ŋ֑��������͂Ȃǂ̏ꍇ�j�i#546 �ȍ~��spt��editStartTextPoint�ɒu�����܂����j
		ATextPoint	editStartTextPoint = {0,0};
		SPI_GetTextPointFromTextIndex(insertIndex,editStartTextPoint);
		
		/*#695��ֈړ�
		//R0208
		if( containLineEnd == true )
		{
			mTextInfo.AdjustParagraphIndex();
		}
		*/
		
		//================== �e��͈͕␳ ==================
		
		//#138
		//#858 AdjustExternalCommentArray(editStartTextPoint.y,insertedLineCount);
		
		//������␳
		if( mBookmarkLineIndex > editStartTextPoint.y )
		{
			mBookmarkLineIndex += insertedLineCount;
		}
		
		//���[�J���͈͕␳ #502 ������ړ��{������␳�Ɠ��l�̏����ɏC�� #893
		//�e�r���[���̃��[�J���͈͂��Ƃ̃��[�v�i�r���[���Ƀ��[�J���͈͂��Ⴄ���߁j
		for( AIndex i = 0; i < mCurrentLocalIdentifierStartLineIndexArray.GetItemCount(); i++ )
		{
			//���[�J���J�n�s���ҏW�J�n�s���ゾ������A�}���s���������炷
			AIndex	localRangeStartLineIndex = mCurrentLocalIdentifierStartLineIndexArray.Get(i);
			if( localRangeStartLineIndex > editStartTextPoint.y && localRangeStartLineIndex != kIndex_Invalid )
			{
				mCurrentLocalIdentifierStartLineIndexArray.Set(i,localRangeStartLineIndex+insertedLineCount);
			}
			//���[�J���I���s���ҏW�I���s���ゾ������A�}���s���������炷
			AIndex	localRangeEndLineIndex = mCurrentLocalIdentifierEndLineIndexArray.Get(i);
			if( localRangeEndLineIndex > editStartTextPoint.y && localRangeEndLineIndex != kIndex_Invalid )
			{
				mCurrentLocalIdentifierEndLineIndexArray.Set(i,localRangeEndLineIndex+insertedLineCount);
			}
		}
		
		//���@�`�F�b�J�[���[�j���O�̒i���ԍ���␳ #992
		for( AIndex i = 0; i < mCheckerPlugin_ParagraphNumber.GetItemCount(); i++ )
		{
			AIndex	p = mCheckerPlugin_ParagraphNumber.Get(i);
			if( p >= paraIndex )
			{
				mCheckerPlugin_ParagraphNumber.Set(i,p+insertedParagraphCount);
			}
		}
		
		//================== ���@�F�� ==================
		
		//#695if( true )
		{
			//�ύX�I���s�̏�Ԃ�ۑ�����
			AIndex	svNextLineStateIndex = 0, svNextLinePushedStateIndex = 0;
			if( updateEndLineIndex < SPI_GetLineCount() )
			{
				mTextInfo.GetLineStateIndex(updateEndLineIndex,svNextLineStateIndex,svNextLinePushedStateIndex);
			}
			//#467 �ύX�s�̎��̍s��directive level, directive disabled��Ԃ�ۑ�
			AIndex	svDirectiveLevel = 0;
			AIndex	svDisabledDirectiveLevel = kIndex_Invalid;
			if( editStartTextPoint.y+1 < SPI_GetLineCount() )
			{
				svDirectiveLevel = mTextInfo.GetDirectiveLevel(editStartTextPoint.y+1);
				svDisabledDirectiveLevel = mTextInfo.GetDisabledDirectiveLevel(editStartTextPoint.y+1);
			}
			
			//���@�F�����s
			AIndex	recognizeEndLineIndex = StartSyntaxRecognizerByEdit(editStartTextPoint.y,insertedLineCount,inRecognizeSyntaxAlwaysInThread);//#698 RecognizeSyntax(editStartTextPoint);
			
			//�ύX�I���s�̏�Ԃ��擾���A���@�F���O�ƕς���Ă�����Aredraw�𕶖@�F���I���s�܂łɂ���B�i��ɕ��@�F���I���s�ɂ���Ƒ��s�i���ҏW���x���Ȃ邽�߁j
			AIndex	nextLineStateIndex = 0, nextLinePushedStateIndex = 0;
			if( updateEndLineIndex < SPI_GetLineCount() )
			{
				mTextInfo.GetLineStateIndex(updateEndLineIndex,nextLineStateIndex,nextLinePushedStateIndex);
			}
			//fprintf(stderr,"%d,",updateEndLineIndex);
			if( svNextLineStateIndex != nextLineStateIndex )
			{
				AColor	color;
				ABool	valid1, valid2;
				ATextStyle	textstyle = kTextStyle_Normal;
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetLetterColorForState(svNextLineStateIndex,color,textstyle,valid1);
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetLetterColorForState(nextLineStateIndex,color,textstyle,valid2);
				if( valid1 == true || valid2 == true )
				{
					if( recognizeEndLineIndex > updateEndLineIndex )   updateEndLineIndex = recognizeEndLineIndex;
				}
			}
			//#467 �ύX�s�̎��̍s��directive level, directive disabled��ԂɕύX������΁Aredraw�𕶖@�F���I���s�܂łɂ���B
			AIndex	directiveLevel = 0;
			AIndex	disabledDirectiveLevel = kIndex_Invalid;
			if( editStartTextPoint.y+1 < SPI_GetLineCount() )
			{
				directiveLevel = mTextInfo.GetDirectiveLevel(editStartTextPoint.y+1);
				disabledDirectiveLevel = mTextInfo.GetDisabledDirectiveLevel(editStartTextPoint.y+1);
			}
			if( svDirectiveLevel != directiveLevel || svDisabledDirectiveLevel != disabledDirectiveLevel )
			{
				if( recognizeEndLineIndex > updateEndLineIndex )   updateEndLineIndex = recognizeEndLineIndex;
			}
			//fprintf(stderr,"%d ",updateEndLineIndex);
		}
		/*#695
		else
		{
			ClearSyntaxData(updateStartLineIndex,updateEndLineIndex);
			//
			//#698 ���g�p�̂��ߍ폜mSyntaxDirty = true;
		}
		*/
		//================== �`��X�V ==================
		
		//redraw
		UpdateWindowText(insertPoint.y,updateStartLineIndex,updateEndLineIndex,insertedLineCount,containLineEnd,inDontRedraw);//#627 R0208(SPI_GetParagraphCount()!=svParagraphCount)); #450
		
		//================== ���̑����� ==================
		
		//IndexWindow���֕ύX�ʒm
		AFileAcc	file;
		NVI_GetFile(file);
		GetApp().SPI_DoTextDocumentEdited(file,insertIndex,ioInsertText.GetItemCount());
		
		/*#699 ��ֈړ�
		//
		NVI_SetDirty(true);
		*/
		
		//selection�C���̂���
		/*#199
		for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
		{
			GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_EditPostProcess(GetObjectID(),insertIndex,ioInsertText.GetItemCount());
		}
		*/
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_EditPostProcess(insertIndex,ioInsertText.GetItemCount(),(inDontRedraw==false));//#846
		}
		
		if( inDontRedraw == false && inDontUpdateSubWindows == false )
		{
			//#142
			//�e�L�X�g�J�E���g�E�C���h�E�X�V
			UpdateDocInfoWindows();
			
			//#723 CallGraf�X�V
			SPI_SetCallGrafEditFlag(SPI_GetFirstWindowID(),inInsertPoint.y);
			
			//�^�C�g���o�[�e�L�X�g�X�V
			if( insertedLineCount != 0 )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_UpdateWindowTitleBarText();
			}
		}
		
		return insertedLineCount;
	}
}

//�e�L�X�g�폜
AItemCount	ADocument_Text::SPI_DeleteText( const ATextIndex inStart, const ATextIndex inEnd, 
										   const ABool inRecognizeSyntaxAlwaysInThread, 
										   const ABool inDontRedraw,
										   const ABool inDontUpdateSubWindows )
{
	//#65
	//�}���`�t�@�C���u���p�Ƀh�L�������g���J�����ꍇ�̏���
	if( mForMultiFileReplace == true )
	{
		//�r������
		AStMutexLocker	locker(mTextMutex);
		//�e�L�X�g�폜
		//���s���ύX�s�v�H
		mText.Delete(inStart,inEnd-inStart);
		//dirty�ݒ�
		NVI_SetDirty(true);
		return 0;
	}
	
	//
	//�s�܂�Ԃ��v�Z���͕ҏW�s�� #699
	if( SPI_IsWrapCalculating() == true )
	{
		_ACError("",NULL);
		return 0;
	}
	
	//R0006
	mDisableInlineInputHilite = true;//#187
	ClearDiffDisplay();
	ClearArrowToDef();//RC1
	mDisableInlineInputHilite = false;//#187
	
	if( inStart > inEnd )   {_ACError("delete inStart>inEnd",this); return 0; }
	
	//selection�C���̂���
	/*#199
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_EditPreProcess(GetObjectID(),inStart,-(inEnd-inStart));
	}
	*/
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_EditPreProcess(inStart,-(inEnd-inStart));
	}
	
	//�폜�i�������擾#695
	AItemCount	deletedParagraphCount = mText.GetCountOfChar(kUChar_LineEnd,inStart,(inEnd-inStart));
	
	//R0208 AItemCount	svParagraphCount = SPI_GetParagraphCount();
	//R0208
	ABool	containLineEnd = mText.IsCharContained(kUChar_LineEnd,inStart,inEnd);
	
	//
	ATextPoint	spt, ept;
	SPI_GetTextPointFromTextIndex(inStart,spt,true);//B0374
	SPI_GetTextPointFromTextIndex(inEnd,ept,true);//B0374
	//B0374 ����
	//�s�܂�Ԃ��i���̍ŏI�s�̍s���ɂĎ������폜���s�������ʁA�O�s�̍ŏI�������A�ŏI�s�̍ŏ��ɗ���ꍇ�A�i���Ƃ��΁A�ŏI�s���u���B�v�ŁA�u���v���폜�j
	//�ŏI�s���ĕ`�悳��Ȃ���肪���������B
	//����́A��Lspt���O�s�̍Ō�ɂȂ��Ă���AAdjustLineInfo()�́uInsert/Delete�O��linestart���Ɠ����ɂȂ�����I���v���锻��ŁA
	//�O�s�ŏI���Ɣ��肳��Ă��܂����߁B�i���Ƃ��΁A�ŏI�s���u���B�v�ƂȂ��Ă��܂����߁A�ŏI�s��linestart���́A�폜�O�Ɠ����i���炵�l���j�ƂȂ��Ă��܂��j
	//��L�̂悤�ɁA���sprefer�ɂ��Ă����΁A�ŏI�s�͕K���ĕ`�悳���B�܂��A�O�s�́A���L�́u�����ɂ���āA�O�s��LineInfo�v�Z�v�̑ΏۂƂȂ�̂ōĕ`�悳���B
	
	//�}���i���ԍ��擾 #992
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(spt.y);
	
	//Undo�L�^
	mUndoer.RecordDeleteText(inStart,inEnd);
	
	//���̒i���J�n�s���擾
	AIndex	nextParagraphStartLineIndex = mTextInfo.GetNextParagraphStartLineIndex(ept.y);
	
	//#399
	//dirty�ɂ���i��r�\���X�N���[�����������Ŕ��肷�邽��SPI_EditPostProcess()���O�Ɏ��s���邽�߁A�܂��ASPI_InsertText()�ƍ��킹�邽�߁A���̈ʒu�Ŏ��s�j
	NVI_SetDirty(true);
	
	//==================�e�L�X�g�폜==================
	{
		AStMutexLocker	locker(mTextMutex);//#598
		//#598 mTextMutex.Lock();
		mText.Delete(inStart,inEnd-inStart);
		//#598 mTextMutex.Unlock();
	}
	
	//==================�q���g�e�L�X�g�X�V==================
	mTextInfo.UpdateHintTextByDeleteText(inStart,inEnd);
	
	//�폜�s����̍s��LineStart���폜�����������炷
	mTextInfo.ShiftLineStarts(spt.y+1,-(inEnd-inStart));
	
	//�폜�J�n�s��肠�Ƃ̍s�̒i��index�����炷#695
	if( deletedParagraphCount > 0 )
	{
		mTextInfo.ShiftParagraphIndex(spt.y+1,-deletedParagraphCount);
	}
	
	AIndex	updateStartLineIndex = spt.y;
	//�����ɂ���āA�O�s��LineInfo�v�Z
	//�i�}���s�̑O�s�ɉ��s�R�[�h�������Ƃ��́A���b�v�̊֌W�ŁA�O�s��LineInfo�ɍX�V���K�v�ȏꍇ������j
	if( spt.y > 0 )
	{
		if( SPI_GetLineExistLineEnd(spt.y-1) == false )
		{
			//B0000 �s�܂�Ԃ��v�Z������
			CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
			AIndex	textDrawDataStartOffset = kIndex_Invalid;
			
			AItemCount	oldLen = SPI_GetLineLengthWithLineEnd(spt.y-1);
			AText	fontname;//win
			SPI_GetFontName(fontname);//win
			mTextInfo.CalcLineInfo(mText,spt.y-1,/*win SPI_GetFont()*/fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
					SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),
					GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters),
					textDrawData,textDrawDataStartOffset);//B0000 �s�܂�Ԃ��v�Z������
			if( oldLen != SPI_GetLineLengthWithLineEnd(spt.y-1) )
			{
				updateStartLineIndex = spt.y-1;
			}
		}
	}
	
	//�s���X�V�A�\���X�V
	AIndex	updateEndLineIndex;
	AItemCount insertedLineCount = AdjustLineInfo(/* spt.y*/updateStartLineIndex,nextParagraphStartLineIndex,updateEndLineIndex);
	
	//#546 ���łɍs���X�V����Ă���̂ŁA�ҏW�J�n�s�ԍ����ς���Ă���\��������i�ŏI�s�ŋ֑��������͂Ȃǂ̏ꍇ�j�i#546 �ȍ~��spt��editStartTextPoint�ɒu�����܂����j
	ATextPoint	editStartTextPoint = {0,0};
	SPI_GetTextPointFromTextIndex(inStart,editStartTextPoint);
	
	/*#695��ֈړ�
	//R0208
	if( containLineEnd == true )
	{
		mTextInfo.AdjustParagraphIndex();
	}
	*/
	
	//================== �e��͈͕␳ ==================
	
	//#138
	//#858 AdjustExternalCommentArray(editStartTextPoint.y+1,insertedLineCount);
	
	//������␳
	if( mBookmarkLineIndex > editStartTextPoint.y && mBookmarkLineIndex <= editStartTextPoint.y-insertedLineCount )
	{
		mBookmarkLineIndex = editStartTextPoint.y;
	}
	else if( mBookmarkLineIndex > editStartTextPoint.y-insertedLineCount )
	{
		mBookmarkLineIndex += insertedLineCount;
	}
	
	//���[�J���͈͕␳ #502 ������ړ��{������␳�Ɠ��l�̏����ɏC��
	//�e�r���[���̃��[�J���͈͂��Ƃ̃��[�v�i�r���[���Ƀ��[�J���͈͂��Ⴄ���߁j
	for( AIndex i = 0; i < mCurrentLocalIdentifierStartLineIndexArray.GetItemCount(); i++ )
	{
		//���[�J���͈͊J�n�s���A�ҏW�J�n�s�̎��̍s�`�폜�����s�̊Ԃɂ���Ƃ��́A���[�J���͈͊J�n�s�͕ҏW�J�n�s�ɂ���
		AIndex	localRangeStartLineIndex = mCurrentLocalIdentifierStartLineIndexArray.Get(i);
		if( localRangeStartLineIndex != kIndex_Invalid )
		{
			if( localRangeStartLineIndex > editStartTextPoint.y && localRangeStartLineIndex <= editStartTextPoint.y-insertedLineCount )
			{
				mCurrentLocalIdentifierStartLineIndexArray.Set(i,editStartTextPoint.y);
			}
			//���[�J���͈͊J�n�s���A�폜���ꂽ�s����ɂ���Ƃ��́A���[�J���͈͊J�n�s�����炷�i�폜�s�������O�ɂ���j
			else if( localRangeStartLineIndex > editStartTextPoint.y-insertedLineCount )
			{
				mCurrentLocalIdentifierStartLineIndexArray.Set(i,localRangeStartLineIndex+insertedLineCount);
			}
		}
		//���[�J���͈͏I���s���A�ҏW�J�n�s�̎��̍s�`�폜�����s�̊Ԃɂ���Ƃ��́A���[�J���͈͏I���s�͕ҏW�J�n�s�ɂ���
		AIndex	localRangeEndLineIndex = mCurrentLocalIdentifierEndLineIndexArray.Get(i);
		if( localRangeEndLineIndex != kIndex_Invalid )
		{
			if( localRangeEndLineIndex > editStartTextPoint.y && localRangeEndLineIndex <= editStartTextPoint.y-insertedLineCount )
			{
				mCurrentLocalIdentifierEndLineIndexArray.Set(i,editStartTextPoint.y);
			}
			//���[�J���͈͏I���s���A�폜���ꂽ�s����ɂ���Ƃ��́A���[�J���͈͏I���s�����炷�i�폜�s�������O�ɂ���j
			else if( localRangeEndLineIndex > editStartTextPoint.y-insertedLineCount )
			{
				mCurrentLocalIdentifierEndLineIndexArray.Set(i,localRangeEndLineIndex+insertedLineCount);
			}
		}
	}
	
	//���@�`�F�b�J�[���[�j���O�̒i���ԍ���␳ #992
	for( AIndex i = 0; i < mCheckerPlugin_ParagraphNumber.GetItemCount();  )
	{
		AIndex	p = mCheckerPlugin_ParagraphNumber.Get(i);
		if( p < paraIndex )
		{
			i++;
		}
		else if( p >= paraIndex && p < paraIndex+deletedParagraphCount )
		{
			mCheckerPlugin_ParagraphNumber.Delete1(i);
			mCheckerPlugin_ColIndex.Delete1(i);
			mCheckerPlugin_TitleText.Delete1(i);
			mCheckerPlugin_DetailText.Delete1(i);
			mCheckerPlugin_TypeIndex.Delete1(i);
			mCheckerPlugin_DisplayInTextView.Delete1(i);
		}
		else
		{
			mCheckerPlugin_ParagraphNumber.Set(i,p-deletedParagraphCount);
			i++;
		}
	}
	
	//================== ���@�F�� ==================
	
	//if( inDontRecognize == false )
	{
		//#695 if( true )
		{
			//�ύX�I���s�̏�Ԃ�ۑ�����
			AIndex	svNextLineStateIndex = 0, svNextLinePushedStateIndex = 0;
			if( updateEndLineIndex < SPI_GetLineCount() )
			{
				mTextInfo.GetLineStateIndex(updateEndLineIndex,svNextLineStateIndex,svNextLinePushedStateIndex);
			}
			//#467 �ύX�s�̎��̍s��directive level, directive disabled��Ԃ�ۑ�
			AIndex	svDirectiveLevel = 0;
			AIndex	svDisabledDirectiveLevel = kIndex_Invalid;
			if( editStartTextPoint.y+1 < SPI_GetLineCount() )
			{
				svDirectiveLevel = mTextInfo.GetDirectiveLevel(editStartTextPoint.y+1);
				svDisabledDirectiveLevel = mTextInfo.GetDisabledDirectiveLevel(editStartTextPoint.y+1);
			}
			
			//���@�F�����s
			AIndex	recognizeEndLineIndex = StartSyntaxRecognizerByEdit(editStartTextPoint.y,insertedLineCount,inRecognizeSyntaxAlwaysInThread);//#698 RecognizeSyntax(editStartTextPoint);
			
			//================== ���@�F���ɂ��`��X�V�͈͂̍X�V ==================
			
			//�ύX�I���s�̏�Ԃ��擾���A���@�F���O�ƕς���Ă�����Aredraw�𕶖@�F���I���s�܂łɂ���B�i��ɕ��@�F���I���s�ɂ���Ƒ��s�i���ҏW���x���Ȃ邽�߁j
			AIndex	nextLineStateIndex = 0, nextLinePushedStateIndex = 0;
			if( updateEndLineIndex < SPI_GetLineCount() )
			{
				mTextInfo.GetLineStateIndex(updateEndLineIndex,nextLineStateIndex,nextLinePushedStateIndex);
			}
			//fprintf(stderr,"%d,",updateEndLineIndex);
			if( svNextLineStateIndex != nextLineStateIndex )
			{
				AColor	color;
				ABool	valid1, valid2;
				ATextStyle	textstyle = kTextStyle_Normal;
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetLetterColorForState(svNextLineStateIndex,color,textstyle,valid1);
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetLetterColorForState(nextLineStateIndex,color,textstyle,valid2);
				if( valid1 == true || valid2 == true )
				{
					if( recognizeEndLineIndex > updateEndLineIndex )   updateEndLineIndex = recognizeEndLineIndex;
				}
			}
			//#467 �ύX�s�̎��̍s��directive level, directive disabled��ԂɕύX������΁Aredraw�𕶖@�F���I���s�܂łɂ���B
			AIndex	directiveLevel = 0;
			AIndex	disabledDirectiveLevel = kIndex_Invalid;
			if( editStartTextPoint.y+1 < SPI_GetLineCount() )
			{
				directiveLevel = mTextInfo.GetDirectiveLevel(editStartTextPoint.y+1);
				disabledDirectiveLevel = mTextInfo.GetDisabledDirectiveLevel(editStartTextPoint.y+1);
			}
			if( svDirectiveLevel != directiveLevel || svDisabledDirectiveLevel != disabledDirectiveLevel )
			{
				if( recognizeEndLineIndex > updateEndLineIndex )   updateEndLineIndex = recognizeEndLineIndex;
			}
			//fprintf(stderr,"updateEnd:%d ",updateEndLineIndex);
		}
		/*#695
		else
		{
			ClearSyntaxData(updateStartLineIndex,updateEndLineIndex);
			//
			//#698 ���g�p�̂��ߍ폜mSyntaxDirty = true;
		}
		*/
	}
	
	//================== �`��X�V ==================
	
	//redraw
	//#627 inDontRedraw��false�ł�LineImageInfo�̍X�V�͍s���K�v������ if( inDontRedraw == false )
	//#627 {
	UpdateWindowText(editStartTextPoint.y,updateStartLineIndex,updateEndLineIndex,insertedLineCount,containLineEnd,
				inDontRedraw);//#627 R0208(SPI_GetParagraphCount()!=svParagraphCount)); #450
	//#627 }
	
	//================== ���̑����� ==================
	
	//IndexWindow���֕ύX�ʒm
	AFileAcc	file;
	NVI_GetFile(file);
	GetApp().SPI_DoTextDocumentEdited(file,inStart,-(inEnd-inStart));
	
	//
	//#399 ���ֈړ��i���̓�������DoInlineInputGetSelectedText()���R�[�������\�������邽�߁j NVI_SetDirty(true);
	
	//selection�C���̂���
	/*#199
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
	GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_EditPostProcess(GetObjectID(),inStart,-(inEnd-inStart));
	}
	*/
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_EditPostProcess(inStart,-(inEnd-inStart),(inDontRedraw==false));
	}
	
	
	if( inDontRedraw == false && inDontUpdateSubWindows == false  )
	{
		//#142
		//�e�L�X�g�E�C���h�E�X�V
		UpdateDocInfoWindows();
		//#723 CallGraf�X�V
		SPI_SetCallGrafEditFlag(SPI_GetFirstWindowID(),spt.y);
		
		//�^�C�g���o�[�e�L�X�g�X�V
		if( insertedLineCount != 0 )
		{
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_UpdateWindowTitleBarText();
		}
	}
	
	return -insertedLineCount;
}

//�e�L�X�g�폜
AItemCount	ADocument_Text::SPI_DeleteText( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, 
										   const ABool inRecognizeSyntaxAlwaysInThread, 
										   const ABool inDontRedraw,
										   const ABool inDontUpdateSubWindows )
{
	//#65
	//�}���`�t�@�C���u���p�Ƀh�L�������g���J�����ꍇ�̏����i�����ɂ͗��Ȃ��j
	if( mForMultiFileReplace == true )
	{
		_ACError("",NULL);
		return 0;
	}
	
	return SPI_DeleteText(SPI_GetTextIndexFromTextPoint(inStartPoint),
		SPI_GetTextIndexFromTextPoint(inEndPoint),
		inRecognizeSyntaxAlwaysInThread,inDontRedraw,inDontUpdateSubWindows);
}

//�e�L�X�g�ҏW(Insert/Delete)���ɁA�e���s��LineInfo���X�V����B
//Insert��
//xxxxxxxx [�}��������(���s�R�[�h�܂�)] xxxxxxx��inStartLineIndex
//xxxxxxxxxxxxxxxxxxxxx
//xxxxxxxx<CR>
//xxxxxxxxxxxxxxxxxxxxx��inEndLineIndex
//��endTextIndex
//lineStart�́A���炵�ς݁B
//����āAinEndLineIndex��lineStart�܂Ōv�Z����Ί����ƂȂ�B
AItemCount	ADocument_Text::AdjustLineInfo( const AIndex inStartLineIndex, const AIndex inEndLineIndex, AIndex& outUpdateEndLineIndex )
{
	ABool	jumpMenuChanged = false;
	ABool	imageSizeChanged = false;//#695
	AItemCount	insertedLineCount = 0;
	//�ǂ�TextIndex�܂Ōv�Z����Ί������A��endTextIndex�Ɋi�[����
	AIndex	endTextIndex = SPI_GetLineStart(inEndLineIndex);
	//B0000 �s�܂�Ԃ��v�Z������
	CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
	AIndex	textDrawDataStartOffset = kIndex_Invalid;
	//
	AText	fontname;//win
	SPI_GetFontName(fontname);//win
	AIndex	lineIndex;
	for( lineIndex = inStartLineIndex; lineIndex < inEndLineIndex; lineIndex++ )
	{
		//�s���v�Z
		mTextInfo.CalcLineInfo(mText,lineIndex,/*win SPI_GetFont()*/fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
				SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),
				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters),
				textDrawData,textDrawDataStartOffset);//B0000 �s�܂�Ԃ��v�Z������
		//�v�Z�����s�̍s��񂩂�A���̍s��LineStart���v�Z����
		AIndex	nextLineStart = SPI_GetLineStart(lineIndex)+SPI_GetLineLengthWithLineEnd(lineIndex);
		if( nextLineStart >= endTextIndex )   break;
		//R0108 Insert/Delete�O��linestart���Ɠ����ɂȂ����炱���ŏI�����Ă悢�i�������j
		if( lineIndex+1 < inEndLineIndex )
		{
			if( nextLineStart == SPI_GetLineStart(lineIndex+1) )
			{
				outUpdateEndLineIndex = lineIndex+1;
				return insertedLineCount;
			}
		}
	}
	outUpdateEndLineIndex = lineIndex+1;
	
	//�܂��ǉ��e�L�X�g�����̍s���v�Z���I�����Ă��Ȃ��ꍇ�i���s���܂ރe�L�X�g�}�����j
	if( lineIndex >= inEndLineIndex )
	{
		//B0000 �s�܂�Ԃ��v�Z������
		CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
		AIndex	textDrawDataStartOffset = kIndex_Invalid;
		//
		for( ; SPI_GetLineStart(lineIndex-1) + mTextInfo.GetLineLengthWithLineEnd(lineIndex-1) < SPI_GetLineStart(lineIndex); lineIndex++ )
		{
			//�ǉ��s����臒l�ȏ�ɂȂ����ꍇ�A�c��̓X���b�h�Ōv�Z����#699
			if( insertedLineCount >= kThreashold_LineCountToUseWrapCalculatorThread )
			{
				//�c��e�L�X�g�v�Z�i���for���̏����Ɠ����v�Z���j
				AIndex	currentLineStart = SPI_GetLineStart(lineIndex-1) + mTextInfo.GetLineLengthWithLineEnd(lineIndex-1);
				AIndex	endLineStart = SPI_GetLineStart(lineIndex);
				//�X���b�h�v�Z�Ώۍs�̍Ōオ���s���ǂ����𔻒肷��B
				//�i�����Ő������ݒ肵�Ă����Ȃ��ƁA���̌��AddEmptyEOFLineIfNeeded()�ŁA���s���Ō�ɑ��݂��Ă��Ȃ��̂ɂ��ւ�炸
				//EOF�s���ǉ�����Ă��܂��B�iEOF�s�ǉ��͍ŏI�s��ExistLineEnd��ON�̏ꍇ�ɒǉ�����邽�߁j�j
				ABool	existLineEnd = false;
				if( endLineStart-1 >= 0 )
				{
					existLineEnd = (mText.Get(endLineStart-1) == kUChar_LineEnd);
				}
				//�X���b�h�v�Z�Ώۍs�����i���̍s�Ɏc��e�L�X�g���߂���ł����āA�X���b�h�Ōv�Z���Ă���A�v�Z�I����ɓW�J����j
				mTextInfo.InsertLineInfo(lineIndex,currentLineStart,endLineStart-currentLineStart,existLineEnd);
				//fprintf(stderr,"currentLineStart:%d len:%d lineindex:%d\n",currentLineStart,endLineStart-currentLineStart,
				//			lineIndex);
				//�X���b�h�Ɍv�Z�w��
				StartWrapCalculator(lineIndex);
				//�s�v�Z���t���Oon
				mWrapCalculatorWorkingByInsertText = true;
				//�ǉ��s���C���N�������g�i�X���b�h�v�Z�Ώۍs�̕��j
				insertedLineCount++;
				break;
			}
			//�s���ǉ�
			mTextInfo.InsertLineInfo(lineIndex);
			insertedLineCount++;
			mTextInfo.CalcLineInfo(mText,lineIndex,/*win SPI_GetFont()*/fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
					SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),
					GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters),
						textDrawData,textDrawDataStartOffset);//B0000 �s�܂�Ԃ��v�Z������
		}
		if( lineIndex > inEndLineIndex )
		{
			//UpdateWindowTextByScroll(lineIndex,lineIndex-inEndLineIndex);
			outUpdateEndLineIndex = SPI_GetLineCount();
			
			//#379UpdateWindowViewImageSize();
			/*�t���O�����āA���ɏ������܂Ƃ߂�#695
			//ImageSize�X�V��TextView(+LineNumberView)�֒ʒm
			for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
			}
			*/
			imageSizeChanged = true;//#695
		}
	}
	//�s���v�Z�������������A�܂��]���ȍs���f�[�^���c���Ă���ꍇ�i���s���܂ރe�L�X�g�폜���j
	else if( lineIndex < inEndLineIndex )
	{
		lineIndex++;
		//#695 �������̂��߁A���[�v�ł͂Ȃ�DeleteLineInfos()���ł܂Ƃ߂č폜���� for( AIndex i = 0; i < inEndLineIndex-lineIndex; i++ )
		{
			//�s���폜
			AArray<AUniqID>	deletedIdentifier;
			ABool	importChanged = false;
			mTextInfo.DeleteLineInfos(lineIndex,inEndLineIndex-lineIndex,deletedIdentifier,importChanged);//#695
			if( importChanged == true )
			{
				//UpdateImportFileData();
				mImportFileDataDirty = true;
			}
			insertedLineCount -= inEndLineIndex-lineIndex;//#695
			if( DeleteFromJumpMenu(deletedIdentifier) == true )
			{
				jumpMenuChanged = true;
			}
		}
		if( inEndLineIndex > lineIndex )
		{
			//UpdateWindowTextByScroll(lineIndex,lineIndex-inEndLineIndex);
			outUpdateEndLineIndex = SPI_GetLineCount();
			
			/*�t���O�����āA���ɏ������܂Ƃ߂�#695
			//#379UpdateWindowViewImageSize();
			//ImageSize�X�V��TextView(+LineNumberView)�֒ʒm
			for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
			}
			*/
			imageSizeChanged = true;//#695
		}
	}
	//�ŏI�s�ŏI�������ꍇ
	/*#699 AddEmptyEOFLineIfNeeded()�Ƀ��W���[����
	if( lineIndex == SPI_GetLineCount() )
	{
		//B0000 �s�܂�Ԃ��v�Z������
		CTextDrawData	textDrawData;
		AIndex	textDrawDataStartOffset = kIndex_Invalid;
		//
		if( SPI_GetLineExistLineEnd(lineIndex-1) == true )
		{
			mTextInfo.InsertLineInfo(lineIndex);
			insertedLineCount++;
			mTextInfo.CalcLineInfo(mText,lineIndex,fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
					SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),
					GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters),
					textDrawData,textDrawDataStartOffset);//B0000 �s�܂�Ԃ��v�Z������
			//#379UpdateWindowViewImageSize();
			//ImageSize�X�V��TextView(+LineNumberView)�֒ʒm
			for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
			}
		}
	}
	*/
	if( mTextInfo.AddEmptyEOFLineIfNeeded() == true )
	{
		insertedLineCount++;
		imageSizeChanged = true;
	}
	//TextView�̃C���[�W�T�C�Y�X�V
	if( imageSizeChanged == true )
	{
		//ImageSize�X�V��TextView(+LineNumberView)�֒ʒm
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
		}
	}
	//�W�����v���X�g�X�V
	if( jumpMenuChanged == true )
	{
		//#695 DeleteFromJumpMenu()���Ń��X�g���̃f�[�^�͍X�V�ς� SPI_UpdateJumpList();//#291
		SPI_RefreshJumpList();//#695
	}
	return insertedLineCount;
}

//
void	ADocument_Text::SPI_RecordUndoActionTag( const AUndoTag inTag )
{
	mUndoer.RecordActionTag(inTag);
}

//
void	ADocument_Text::SPI_InsertLineEndAtWrap( const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	if( inStartLineIndex >= inEndLineIndex )   return;
	SPI_RecordUndoActionTag(undoTag_ReturnAtWrap);
	AText	lineendText;
	lineendText.Add(kUChar_LineEnd);
	for( AIndex lineIndex = inStartLineIndex; lineIndex < inEndLineIndex; lineIndex++ )
	{
		if( SPI_GetLineExistLineEnd(lineIndex) == false )
		{
			ATextPoint	textpoint;
			textpoint.x = SPI_GetLineLengthWithoutLineEnd(lineIndex);
			textpoint.y = lineIndex;
			SPI_InsertText(textpoint,lineendText);
		}
	}
}

/**
InsertText/DeleteText�ł̕`��X�V�}���̌�̕`��܂Ƃ߂čX�V
*/
void	ADocument_Text::SPI_DeferredRefreshAfterInsertDeleteText()
{
	//text view�X�V
	SPI_RefreshTextViews();
}

/**
InsertText/DeleteText�ł̃T�u�E�C���h�E�`��X�V�}���̌�̃T�u�E�C���h�E�`��܂Ƃ߂čX�V
*/
void	ADocument_Text::SPI_DeferredRefreshSubWindowsAfterInsertDeleteText()
{
	//�e�L�X�g�J�E���g�E�C���h�E�X�V
	UpdateDocInfoWindows();
	
	//�^�C�g���o�[�e�L�X�g�X�V
	AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_UpdateWindowTitleBarText();
}

#pragma mark ===========================

#pragma mark ---�e�L�X�g�ɑ΂���e�푀��

void	ADocument_Text::SPI_Undo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	if( mUndoer.Undo(outSelectTextIndex,outSelectTextLength) == true )
	{
		//�Ԃ�l��true�́Aundo�ɂ��action�񐔂������i�u�����j���߁ASPI_InsertText()/SPI_DeleteText()��inDontRedraw=true�ŃR�[�����Ă��邱�Ƃ�
		//�����Ă���B�Ȃ̂ŁA������redraw���s���B
		SPI_DeferredRefreshAfterInsertDeleteText();
	}
	//#267
	UpdateDirtyByUndoPosition();
}

void	ADocument_Text::SPI_Redo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	if( mUndoer.Redo(outSelectTextIndex,outSelectTextLength) == true )
	{
		//�Ԃ�l��true�́Aundo�ɂ��action�񐔂������i�u�����j���߁ASPI_InsertText()/SPI_DeleteText()��inDontRedraw=true�ŃR�[�����Ă��邱�Ƃ�
		//�����Ă���B�Ȃ̂ŁA������redraw���s���B
		SPI_DeferredRefreshAfterInsertDeleteText();
	}
	//#267
	UpdateDirtyByUndoPosition();
}

//#267
/**
*/
void	ADocument_Text::UpdateDirtyByUndoPosition()
{
	AIndex	currentUndoPos = mUndoer.GetCurrentLastEditRecordPosition();
	if( mLastSavedUndoPosition == currentUndoPos )
	{
		NVI_SetDirty(false);
		//#485 Diff�f�[�^�X�V�i�t�@�C���Ɠ����Ȃ̂ŁADiff�f�[�^�\���\��ԂɂȂ�jsvn diff�͎��s�����A����diff�ɂ����̂ŁAsvn�ւ̕��ׂ̐S�z���Ȃ��Ȃ������ߑΉ��B
		SPI_UpdateDiffDisplayData();
	}
	else
	{
		NVI_SetDirty(true);
	}
}

//
ABool	ADocument_Text::SPI_GetUndoText( AText& outText ) const
{
	return mUndoer.GetUndoMenuText(outText); 
}

//
ABool	ADocument_Text::SPI_GetRedoText( AText& outText ) const
{
	return mUndoer.GetRedoMenuText(outText);
}

//�ۑ�
ABool	ADocument_Text::SPI_Save( const ABool inSaveAs )
{
	//���ꖢ���[�h�ł����ɗ����ꍇ�̓��^�[���i�ۑ����Ă��܂��Ƌ�̃e�L�X�g��ۑ����Ă��܂��̂Ŗ���ɂ��ۑ����Ă͂����Ȃ��B�j
	//���������Ċm�F
	if( mLoaded == false )
	{
		_ACError("",NULL);
		return false;
	}
	
	//#81
	//==================�����o�b�N�A�b�v���s==================
	SPI_AutoSave_FileBeforeSave(mForMultiFileReplace);
	
	//==================�q���g�e�L�X�g�S�폜==================
	SPI_DeleteAllHintText();
	
	//==================�f�o�b�O�p����==================
	//�ۑ����ɁA�f�[�^�������`�F�b�N#699
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )
	{
		mTextInfo.CheckLineInfoDataForDebug();
	}
	
	//==================�}�E�X�J�[�\���\��==================
	//win
	ACursorWrapper::ShowCursor();
	//
	ABool	saveOK = false;
	//�ۑ��O����
	//�i�E�C���h�E�ʒu��A�L�����b�g�ʒu���AWindow, View�I�u�W�F�N�g���ێ�����f�[�^���A�h�L�������g��PrefDB�֊i�[���鏈�����j
	/*#379
	if( mWindowIDArray.GetItemCount() > 0 )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_SavePreProcess(GetObjectID());
	}
	*/
	//#379
	//==================�E�C���h�E���̕ۑ��O����==================
	AWindowID	firstWindowID = SPI_GetFirstWindowID();
	if( firstWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_SavePreProcess(GetObjectID());
	}
	
	//==================�t�@�C���ۑ��_�C�A���O�\��==================
	//File������A�������́ASaveAs�Ȃ�A�t�@�C���ۑ��_�C�A���O��\�����ďI��
	if( ((NVI_IsFileSpecified()==false)&&(SPI_IsRemoteFileMode()==false)) || inSaveAs == true )//#361
	{
		/*#379
		if( mWindowIDArray.GetItemCount() > 0 )
		{
			GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_ShowSaveWindow();
		}
		*/
		if( firstWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_ShowSaveWindow();
		}
		//��U�I���B�t�@�C���ۑ��_�C�A���O����OK�̏ꍇ�AFile�����A��������ēxSPI_Save(false)���R�[�������B
		return false;
	}
	
	//==================�t�@�C���ۑ��O�����i���[�J���t�@�C���̏ꍇ�̂݁j==================
	//���[�J���t�@�C���̏ꍇ�̂ݏ�������B
	//���݂̃t�@�C�����e��miLastSaveOldText�t�@�C���֕ۑ��A�ҏW���Ƀt�@�C�����ړ����ꂽ�ꍇ�̑΍�Ȃǂ��s��
	
	//AFileAcc	folder;
	AText	oldText;
	ABool	oldTextOK = false;
	if( SPI_IsRemoteFileMode() == false )//#361
	{
		//
		AFileAcc	file;
		NVI_GetFile(file);
		
		//�t�@�C�����b�N��Specify�Ȃ�Specify����
		if( mFileWriteLocker.IsSpecified() == false )
		{
			mFileWriteLocker.Specify(file);
		}
		
		/*#666 mFileWriteLocker.Lock()�̒��g��open(,O_NONBLOCK|O_SHLOCK)�Ȃ̂ŁA
		O_SHLOCK�ł��Ȃ��悤�ȃl�b�g���[�N�t�@�C���T�[�o�[���ƁA���false���Ԃ�\��������B
		������Lock����K�v���͂Ȃ��A�������݌����`�F�b�N���Ȃ��Ă��A���WriteFile()�ŃG���[���m�ł���̂ŁA
		�����̃`�F�b�N�͊O���B
		//�t�@�C�����b�N���Ă��Ȃ��ꍇ�͏������݃p�[�~�V�����`�F�b�N�i���b�N�j
		if( mFileWriteLocker.IsLocked() == false )
		{
			if( mFileWriteLocker.Lock() == false )
			{
				AText	text1, text2;
				text1.SetLocalizedText("FileSaveError_WritePermision1");
				text2.SetLocalizedText("FileSaveError_WritePermision2");
				GetApp().SPI_GetTextWindowByID(SPI_GetFirstWindowID()).SPI_ShowChildErrorWindow(GetObjectID(),text1,text2);
				return;
			}
		}
		*/
		
		//���݂̃t�@�C�����e��oldText�֊i�[�i�f�B�X�N�e�ʃG���[���ŕۑ����s�����ꍇ�ɁA���̃t�@�C�����e�ɖ߂����߁j
		oldTextOK = file.ReadTo(oldText);
		
		//�ҏW���Ƀt�@�C�����ړ����ꂽ�ꍇ�̑΍�
		if( file.Exist() == false )
		{
			file.CreateFile();
		}
		//folder.SpecifyParent(file);
	}
	/*
	else
	{
		AFileWrapper::GetAppPrefFolder(folder);
	}
	*/
	
	//���݂̃t�@�C�����e��"miSaving"�t�@�C���ɕۑ����Asave�I����AmiSaving���폜����B�i�����A�r���ŋ����I��������AmiSaving�Ɏc���Ă���j
	//B0446 AFileAcc	miSavingFile;
	//B0446 miSavingFile.SpecifyUniqChildFile(folder,"miSaving");
	//B0446 miSavingFile.CreateFile();//win 080702
	//B0446 miSavingFile.WriteFile(oldText);
	GetApp().SPI_SaveTemporaryText(oldText);//B0446
	
	try//B0446
	{
		//==================�e�L�X�g�G���R�[�f�B���O�ϊ�==================
		
		//UTF-8, CR����A�e�e�L�X�g�G���R�[�f�B���O�֕ϊ�
		AText	buffer;
		ABool	converted = false;
		/*#844 SJIS���X���X�t�H�[���o�b�N��drop
		//#473 SJIS���X���X�t�H�[���o�b�N
		if( SPI_GetTextEncoding() == ATextEncodingWrapper::GetSJISTextEncoding() &&
					GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kUseSJISLosslessFallback) == true )
		{
			if( ConvertFromUTF8ToSJISUsingLosslessFallback(buffer) == true )
			{
				converted = true;
			}
		}
		*/
		//
		if( converted == false )//#473
		{
			//Canonical Comp�ϊ��ݒ�ON�Ȃ�Canonical Comp�֕ϊ����� #1044
			ABool	convertToCanonical = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp);
			//�ۑ��p�e�L�X�g�G���R�[�f�B���O�֕ϊ�
			if( mText.ConvertFromUTF8CR(SPI_GetTextEncoding(),SPI_GetReturnCode(),buffer,convertToCanonical) == false )
			{
				//------------------�e�L�X�g�G���R�[�f�B���O�ϊ��Ɏ��s�����ꍇ------------------
				
				//#65
				//�E�C���h�E���Ȃ��ꍇ�i���}���`�t�@�C���u���ɂ��I�[�v�����j�̏ꍇ�A�����Ń��^�[���i�Ԃ�l�F�G���[�j����
				if( SPI_GetFirstWindowID() == kObjectID_Invalid )
				{
					return false;
				}
				/*#926
				//�_�C�A���O�\��
				AText	text1, text2;
				text1.SetLocalizedText("FileSave_ConvertedToUTF8_1");
				text2.SetLocalizedText("FileSave_ConvertedToUTF8_2");
				GetApp().SPI_GetTextWindowByID(SPI_GetFirstWindowID()).SPI_ShowChildErrorWindow(GetObjectID(),text1,text2);
				*/
				AObjectIDArray	objectIDArray;
				objectIDArray.Add(SPI_GetFirstWindowID());
				ABase::PostToMainInternalEventQueue(kInternalEvent_SaveResultDialog_ForcedToUTF8,
													GetObjectID(),0,GetEmptyText(),objectIDArray);
				//UTF-8�ɕύX
				//#934 SPI_SetTextEncoding(ATextEncodingWrapper::GetUTF8TextEncoding());
				AText	tecname("UTF-8");
				SPI_SetTextEncoding(tecname);
				mText.ConvertReturnCodeFromCR(SPI_GetReturnCode(),buffer);
			}
		}
		
		/*#844
		//DOSEOF(1A)�t��
		if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSaveDOSEOF) == true )
		{
			buffer.Add(kUChar_DOSEOF);
		}
		*/
		
		//------------------BOM�t������------------------
		//����UTF16��BOM(FEFF)�ǉ�
		//win if( SPI_GetTextEncoding() == ATextEncodingWrapper::GetUTF16TextEncoding() )
		//Encoding�̒l��UTF-16���A���O��UTF-16LE, UTF-16BE�ȊO�̏ꍇ��BOM�ǉ��iWindows�ł͒l��UTF-16��UTF-16LE�������̂��߁j
		AText	encname;//win
		SPI_GetTextEncoding(encname);//win
		if( SPI_GetTextEncoding() == ATextEncodingWrapper::GetUTF16TextEncoding() &&
					encname.Compare("UTF-16LE") == false && encname.Compare("UTF-16BE") == false )//win
		{
			if( ATextEncodingWrapper::UTF16IsLE() == false )
			{
				//Big Endian
				buffer.Insert1(0,0xFE);
				buffer.Insert1(1,0xFF);
			}
			else
			{
				//Little Endian
				buffer.Insert1(0,0xFF);
				buffer.Insert1(1,0xFE);
			}
		}
		// UTF8 BOM�ǉ� EF BB BF   B0016
		if( SPI_GetTextEncoding() == ATextEncodingWrapper::GetUTF8TextEncoding() &&
					GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kSaveUTF8BOM) == true )
		{
			buffer.Insert1(0,0xEF);
			buffer.Insert1(1,0xBB);
			buffer.Insert1(2,0xBF);
		}
		
		if( SPI_IsRemoteFileMode() == false )
		{
			//==================�t�@�C���������ݏ����i���[�J���t�@�C���̏ꍇ�j==================
			
			//
			AFileAcc	file;
			NVI_GetFile(file);
			
			//�t�@�C�����b�N����
			mFileWriteLocker.Unlock();
			
			//�t�@�C����������
			AFileError	err;
			if( file.WriteFile(buffer,err) == false )
			{
				//#1034 if( err == dskFulErr )//B0224
				if( err == kFileError_DiskFull )//#1034
				{
					if( SPI_GetFirstWindowID() != kObjectID_Invalid )//#65
					{
						AText	text1, text2;
						text1.SetLocalizedText("FileSaveError_DiskFull_A");
						text2.SetLocalizedText("FileSaveError_DiskFull_B");
						GetApp().SPI_GetTextWindowByID(SPI_GetFirstWindowID()).SPI_ShowChildErrorWindow(GetObjectID(),text1,text2,true);
					}
					if( oldTextOK == true )//B0290
					{
						file.WriteFile(oldText);
					}
				}
				else
				{
					if( SPI_GetFirstWindowID() != kObjectID_Invalid )//#65
					{
						AText	text1, text2;
						text1.SetLocalizedText("FileSaveError_Other_A");
						text2.SetLocalizedText("FileSaveError_Other_B");
						GetApp().SPI_GetTextWindowByID(SPI_GetFirstWindowID()).SPI_ShowChildErrorWindow(GetObjectID(),text1,text2,true);
					}
					if( oldTextOK == true )//B0290
					{
						file.WriteFile(oldText);
					}
				}
				//�t�@�C�����b�N
				if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kLockWhileEdit) == true )//B0355
				{
					mFileWriteLocker.Lock();
				}
				//B0446 miSavingFile.DeleteFile();//win 080702
				return false;
			}
			else
			{
				saveOK = true;
			}
			
			//==================DocPref�ۑ�==================
			
			if( SPI_GetFirstWindowID() != kObjectID_Invalid )//#65 �E�C���h�E������ꍇ�̂ݎ��s
			{
				SaveDocPref();//#898
			}
			
#if IMPLEMENTATION_FOR_MACOSX
			//==================Mac�Ǝ��f�[�^�ۑ�==================
			if( SPI_GetFirstWindowID() != kObjectID_Invalid )//#65 �E�C���h�E������ꍇ�̂ݎ��s
			{
				//------------------�N���G�[�^�E�^�C�v��ۑ�------------------
				AFileAttribute	attr;
				attr.creator = mCreator;
				attr.type = mType;
				//win AFileWrapper::SetFileAttribute(file,attr);
				file.SetFileAttribute(attr);
				
				//------------------usro���\�[�X�ۑ�------------------
				//#311 Mac OS X 10.6�Ń_�u���N���b�N�Ńt�@�C�����J���Ȃ����΍�Busro���\�[�X�������Œǉ��B
				if( mCreator == 'MMKE' )
				{
					//FSRef�擾
					FSRef	ref;
					file.GetFSRef(ref);
					//���\�[�X�t�H�[�N�ǉ�
					{
						HFSUniStr255	forkName;
						::FSGetResourceForkName(&forkName);
						::FSCreateResourceFork(&ref,forkName.length,forkName.unicode,0);
					}
					//���\�[�X�t�H�[�N���J��
					short	rRefNum = ::FSOpenResFile(&ref,fsWrPerm);
					if( rRefNum != -1 )
					{
						//���X��usro���\�[�X���폜
						Handle	res = ::Get1Resource('usro',0);
						if( res != NULL ) 
						{
							::RemoveResource(res);
							::DisposeHandle(res);
						}
						//�n���h��alloc
						Handle	h = ::NewHandle(0x404);
						::HLock(h);
						for( AIndex i = 0; i < 0x404; i++ )
						{
							*((*h)+i) = 0x00;
						}
						//�A�v���P�[�V�����p�X�擾
						AFileAcc	appFile;
						AFileWrapper::GetAppFile(appFile);
						AText	appPath;
						appFile.GetPath(appPath);
						AItemCount	len = appPath.GetItemCount();
						if( len <= 0x400 )
						{
							//�����O�X��������
							UInt32*	p = (UInt32*)(*h);
							(*p) = len;//�Ȃ���host endian�̂܂܏������܂Ȃ��ƍŏI�I�Ƀ��\�[�X�ɏ������܂��f�[�^���t�ɂȂ�
							//�A�v���p�X��������
							for( AIndex i = 0; i < len; i++ )
							{
								*((*h)+4+i) = appPath.Get(i);
							}
						}
						::HUnlock(h);
						//���\�[�X�������݁E�N���[�Y
						::AddResource(h,'usro',0,"\p");
						::WriteResource(h);
						::ReleaseResource(h);
						::CloseResFile(rRefNum);
					}
				}
				
				//R0230
				//------------------�g������------------------
				//#844 if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSaveTextEncodingToEA) == true )
				if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 ) //#844 �ݒ�폜�B10.4�ȏ�Ȃ��ɐݒ�B�i10.3�͖��Ή��B�j
				{
					AText	tecname;
					SPI_GetTextEncoding(tecname);
					//#1040
					//IANA���̂��擾�i��Ŏ擾�������̂͂����܂�mi�p�ݒ薼�́j
					ATextEncoding	tec = ATextEncodingWrapper::GetTextEncodingFromName(tecname);
					ATextEncodingWrapper::GetTextEncodingIANAName(tec,tecname);
					//
					file.SetTextEncodingByXattr(tecname);
				}
				else
				{
					file.SetTextEncodingByXattr(GetEmptyText());
				}
			}
#endif
			
			//==================�t�@�C�����b�N==================
			if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kLockWhileEdit) == true )//B0355
			{
				mFileWriteLocker.Lock();
			}
			
			//==================OS�փt�@�C���ۑ��ʒm==================
			ALaunchWrapper::NotifyFileChanged(file);
			
#if IMPLEMENTATION_FOR_MACOSX
			//==================ODB���M==================
			//ODBModified���M(Mac only)
			if( mODBMode == true )
			{
				ABool	fileChanged = false;
				if( mODBFile.Compare(file) == false )
				{
					fileChanged = true;
				}
				ALaunchWrapper::SendODBModified(mODBSender,mODBToken,mODBFile,fileChanged,file);
				mODBFile.CopyFrom(file);
			}
#endif
			
			//#976
			//�Ō�ɊJ�����t�@�C����ۑ��i�V�K�ۑ����̃f�t�H���g�t�H���_�Ɏg�p�j
			GetApp().SPI_SetLastOpenedTextFile(file);
		}
		else
		{
			//==================�t�@�C���������ݏ����i�����[�g�t�@�C���̏ꍇ�j==================
			/*#844 ���[�J���o�b�N�A�b�v�@�\�폜
			//------------------�o�b�N�A�b�v�ۑ�------------------
			//�o�b�N�A�b�v�ۑ�
			AText	server, user, path;
			mRemoteFileURL.GetFTPURLServer(server);
			mRemoteFileURL.GetFTPURLUser(user);
			mRemoteFileURL.GetFTPURLPath(path);//B0369
			AIndex	index = GetApp().GetAppPref().FindFTPAccountPrefIndex(server,user);
			if( index != kIndex_Invalid )
			{
				if( GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFTPAccount_Backup,index) == true )
				{
					AText	text;
					GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kFTPAccount_BackupFolderPath,index,text);
					AFileAcc	folder, file;
					folder.SpecifyWithAnyFilePathType(text);//B0389 OK
					if( folder.Exist() == false )
					{
						AText	text1, text2;
						text1.SetLocalizedText("FileSaveError_FTPLocalBackupError1");
						text2.SetLocalizedText("FileSaveError_FTPLocalBackupError2");
						GetApp().SPI_GetTextWindowByID(SPI_GetFirstWindowID()).SPI_ShowChildErrorWindow(GetObjectID(),text1,text2);
					}
					else
					{
						//B0369 FTP�p�X�ƑΉ����郍�[�J����̃t�H���_�������i�����j
						ATextArray	foldernamearray;
						foldernamearray.ExplodeFromText(path,'/');
						for( AIndex i = 0; i < foldernamearray.GetItemCount()-1; i++ )
						{
							if( foldernamearray.GetTextConst(i).GetItemCount() == 0 )   continue;
							AFileAcc	f;
							f.SpecifyChild(folder,foldernamearray.GetTextConst(i));
							folder.CopyFrom(f);
							if( folder.Exist() == false )
							{
								folder.CreateFolder();
							}
						}
						
						AText	filename;
						mRemoteFileURL.GetFilename(filename);
						//R0207
						if( GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFTPAccount_BackupWithDateTime,index) == true )
						{
							AText	suffix;
							filename.GetSuffix(suffix);
							AText	text;
							ADateTimeWrapper::Get6LettersDateText(text);
							text.Insert1(0,'_');
							filename.InsertText(filename.GetItemCount()-suffix.GetItemCount(),text,0,text.GetItemCount());
							ADateTimeWrapper::GetTimeText(text);
							text.Insert1(0,'_');
							text.ReplaceChar(':','_');
							text.ReplaceChar(' ','_');
							filename.InsertText(filename.GetItemCount()-suffix.GetItemCount(),text,0,text.GetItemCount());
						}
						file.SpecifyChild(folder,filename);
						file.CreateFile();
						file.WriteFile(buffer);
					}
				}
			}
			*/
			//------------------�����[�g�ۑ�------------------
			/*#361
			GetApp().SPI_GetFTPConnection().SPNVI_Run_PutFile(mFTPURL,buffer);
			saveOK = GetApp().SPI_GetFTPConnection().SPI_GetLastPutFileOK();
			*/
			//�v���O�C���Đڑ��i�h�L�������g���J�������Ɠ����v���O�C���ɐڑ�����j
			//#193 GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).ReconnectPlugin();
			//�����[�g�ڑ��I�u�W�F�N�g���g�p���ĕۑ��iPUT�R�}���h���M�j #361
			//����A�X���b�h���g�p�����A�����ۑ����Ă���
			if( GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
			   ExecutePUT(kRemoteConnectionRequestType_PUTfromTextDocument,mRemoteFileURL,buffer) == false )
			{
				//���ۑ��s�i�R�}���h���s���j������
			}
			//#361
			//�t�@�C�����X�g�̃t�H���_���X�V
			AText	parentFolderURL;
			mRemoteFileURL.GetParentURL(parentFolderURL);
			GetApp().SPI_UpdateFileListRemoteFolder(parentFolderURL);
			
			//
			saveOK = true;
			
			//#241
			//#898 mDocPrefDB.SaveForRemoteFile(mRemoteFileURL);
			SaveDocPref();//#898
			
			//#361
			mTextOnRemoteServer.SetText(buffer);
		}
		
		//==================�ۑ�OK���A�ۑ��㏈��==================
		if( saveOK == true )
		{
			//#81
			//------------------unsaved data�t�@�C���̍폜------------------
			AutoSave_RemoveUnsavedDataFile();
			
			//#734
			//------------------�v���r���[�X�V------------------
			SPI_ReloadPrevew();
			
			//------------------Dirty����------------------
			NVI_SetDirty(false);
			
			//------------------�ۑ���undo position�L��------------------
			//#267 UndoPosition�ۑ�
			mLastSavedUndoPosition = mUndoer.GetCurrentLastEditRecordPosition();
			mUndoer.SetInhibitConcat();//�����h�~�t���O�ݒ�i����Undo�ƌ�������ƕۑ��ʒu�ւ�Undo�ɂȂ�Ȃ��Ȃ�j
			
			//------------------charset�`�F�b�N------------------
			//charset�`�F�b�N R0045
			if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kCheckCharsetWhenSave) == true )
			{
				ATextEncoding	charsetEnc;
				if( mText.GuessTextEncodingFromCharset(charsetEnc) == true && SPI_GetFirstWindowID() != kObjectID_Invalid )//#65
				{
					//SJIS�n�̏ꍇ�́A�S�������łȂ��Ă��A�ǂ����SJIS�n�Ȃ瓯���Ƃ݂Ȃ��B#1331
					if( ATextEncodingWrapper::IsSJISSeriesTextEncoding(charsetEnc) == true &&
						ATextEncodingWrapper::IsSJISSeriesTextEncoding(SPI_GetTextEncoding()) == true )
					{
						//SJIS�n�͓����Ƃ݂Ȃ��B�����Ȃ��B
					}
					//
					else if( charsetEnc != SPI_GetTextEncoding() )
					{
						/*#926
						AText	text1, text2;
						text1.SetLocalizedText("CharsetCheck_Error1");
						text2.SetLocalizedText("CharsetCheck_Error2");
						GetApp().SPI_GetTextWindowByID(SPI_GetFirstWindowID()).SPI_ShowChildErrorWindow(GetObjectID(),text1,text2);
						*/
						AObjectIDArray	objectIDArray;
						objectIDArray.Add(SPI_GetFirstWindowID());
						ABase::PostToMainInternalEventQueue(kInternalEvent_SaveResultDialog_CharsetError,
															GetObjectID(),0,GetEmptyText(),objectIDArray);
						
					}
				}
			}
			
			//==================���[�J���t�@�C���̏ꍇ�̂ݕۑ��㏈��==================
			if( SPI_IsRemoteFileMode() == false )
			{
				//------------------App�Ƀh�L�������g�ۑ���ʒm------------------
				//
				AFileAcc	file;
				NVI_GetFile(file);
				//
				GetApp().SPI_DoTextDocumentSaved(file);
				
				//------------------�Ō�ɕۑ������t�@�C���f�[�^���L��------------------
				file.GetLastModifiedDataTime(mLastModifiedDateTime);
				//#653 �ۑ������t�@�C���f�[�^�i�t�@�C���ɏ��������f�[�^�j���t�@�C���ɋL���i���A�v���ł̕ҏW���m�Ɏg�p�j
				//#693 mLastReadOrWriteFileRawData.SetText(buffer);
				AFileAcc	lastLoadOrSaveFile;
				GetLastLoadOrSaveCopyFile(lastLoadOrSaveFile);
				lastLoadOrSaveFile.CreateFile();
				lastLoadOrSaveFile.WriteFile(buffer);
				//
				//#379 UpdateInfoHeader();
				//#725 GetApp().SPI_UpdateInfoHeader(GetObjectID());//#379
			}
		}
		//#81
		//�Ō�Ƀ��[�h�^�Z�[�u�������̃e�L�X�g�G���R�[�f�B���O�i���t�@�C���̌��݂̃e�L�X�g�G���R�[�f�B���O�j���L���������o�b�N�A�b�v���ɕۑ�
		SPI_GetTextEncoding(mTextEncodingWhenLastLoadOrSave);
		
		//------------------Diff�f�[�^�X�V------------------
		//#379if( mDiffTargetMode == kDiffTargetMode_File || GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCompareWithLatestWhenSave) == true )//#379
		//#379{
		SPI_UpdateDiffDisplayData();
		//#379}
		
		//------------------text view�X�V------------------
		//SPI_ReRecognizeSyntax();//B0000
		//#212 RefreshWindow();
		SPI_RefreshTextViews();//#212
		
		//#688
		//------------------�^�C�g���o�[�X�V�i�]��InfoHeader�ɕ\�����Ă������e���^�C�g���o�[�ɕ\�����邽�߁j------------------
		GetApp().NVI_UpdateTitleBar();
		
		//�C�x���g���X�i�[���s #992
		SPI_ExecuteEventListener("onSaved",GetEmptyText());
	}
	catch(...)//B0446
	{
		_ACError("ADocument_Text::SPI_Save() caught exception.",NULL);//#199
	}
	//
	//B0446 miSavingFile.DeleteFile();//win 080702
	return true;
}

//#844 SJIS���X���X�t�H�[���o�b�N��drop
#if 0
/**
SJIS���X���X�t�H�[���o�b�N�ϊ�
@return �ُ핶�������݂��A���A�t�H�[���o�b�N�ϊ�����������true
*/
ABool	ADocument_Text::ConvertFromUTF8ToSJISUsingLosslessFallback( AText& outText ) const
{
	//���ʃe�L�X�g�폜
	outText.DeleteAll();
	//�ُ핶�������݂������ǂ����̃t���O
	ABool	abnormalCharExist = false;
	//
	AArray<AUChar>	escape01Array;
	AText	text;
	//UTF-8�e�L�X�g���������iU+105CXX/0x01�����݂����escape01Array�֊i�[���Ă����j
	AItemCount	textlen = mText.GetItemCount();
	for( AIndex pos = 0; pos < textlen; pos++ )
	{
		AUChar	ch = mText.Get(pos);
		if( ch == 0x01 )
		{
			//0x01
			escape01Array.Add(0x01);
			text.Add(0x01);
		}
		else if( ch == 0xF4 )
		{
			if( mText.Get(pos+1) == 0x85 )
			{
				//U+105CXX SJIS2�o�C�g�ڕs�݃G���[
				AUChar	escapeCh = ((mText.Get(pos+2)&0x03)<<6)+(mText.Get(pos+3)&0x3F);
				escape01Array.Add(escapeCh);
				text.Add(0x01);
				abnormalCharExist = true;
				pos += 3;
			}
			else
			{
				//���ʂ�1byte�ǉ�
				text.Add(ch);
			}
		}
		else
		{
			//���ʂ�1byte�ǉ�
			text.Add(ch);
		}
	}
	//�ُ핶�������݂��Ȃ����return
	if( abnormalCharExist == false )   return false;
	
	//SJIS�֕ϊ�
	if( text.ConvertFromUTF8CR(ATextEncodingWrapper::GetSJISTextEncoding(),SPI_GetReturnCode(),outText) == false )
	{
		return false;
	}
	
	//0x01��escape01Array�ɏ]���ĕϊ�
	AItemCount	len = outText.GetItemCount();
	AIndex	escape01pos = 0;
	for( AIndex pos = 0; pos < len; pos++ )
	{
		//0x01���ǂ����𔻒�
		AUChar	ch = outText.Get(pos);
		if( ch == 0x01 )
		{
			//escape01Array�ɏ]���ĕ����ϊ�
			AUChar	escapeCh = escape01Array.Get(escape01pos);
			escape01pos++;
			if( escapeCh != 0x01 )
			{
				outText.Set(pos,escapeCh);
			}
		}
	}
	return true;
}
#endif

//#81
/**
dirty��ԕύX������
*/
void	ADocument_Text::NVIDO_SetDirty( const ABool inDirty )
{
	//���ۑ��f�[�^�p�̃t���O�i�O�񎩓��ۑ�����̕ҏW�����邩�ǂ����̃t���O�j��ݒ�
	mAutoSaveDirty = inDirty;
}

#pragma mark ===========================

#pragma mark ---�����o�b�N�A�b�v
//#81

//�����o�b�N�A�b�v����
const AItemCount	kAutoBackup_MaxGenerations_Monthly = 24;
const AItemCount	kAutoBackup_MaxGenerations_Daily = 10;
const AItemCount	kAutoBackup_MaxGenerations_Hourly = 10;
const AItemCount	kAutoBackup_MaxGenerations_Minutely = 6;

/**
�����o�b�N�A�b�v���ꂽ�t�@�C���̃��X�g���擾
*/
void	ADocument_Text::SPI_UpdateAutoBackupArray()
{
	//�����o�b�N�A�b�v�t�H���_���擾
	AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
	if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
	{
		//�e�����o�b�N�A�b�v�t�H���_���̃t�@�C���̃��X�g���擾
		GetAutoBackupNumberArray(monthlyFolder,mAutoBackupMonthlyArray_Number);
		GetAutoBackupNumberArray(dailyFolder,mAutoBackupDailyArray_Number);
		GetAutoBackupNumberArray(hourlyFolder,mAutoBackupHourlyArray_Number);
		GetAutoBackupNumberArray(minutelyFolder,mAutoBackupMinutelyArray_Number);
	}
}

/**
�㏑���ۑ����̕ۑ��O�t�@�C���̎����o�b�N�A�b�v
*/
void	ADocument_Text::SPI_AutoSave_FileBeforeSave( const ABool inAlways )
{
	//�����o�b�N�A�b�vdisable�Ȃ牽�����Ȃ�
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kEnableAutoBackup) == false )
	{
		return;
	}
	
	//���݂̓����e�L�X�g���擾
	AText	currentDateTimeText;
	GetAutoSaveCurrentDateTimeText(currentDateTimeText);
	ANumber64bit	currentNumber = currentDateTimeText.GetNumber64bit();
	
	//===============Minutely�o�b�N�A�b�v===============
	//�����ƃo�b�N�A�b�v�̃t�@�C�����X�g����ŐV�̂��̂̓����e�L�X�g���擾
	ANumber64bit	latestMinutelyNumber = 0;
	if( mAutoBackupMinutelyArray_Number.GetItemCount() > 0 )
	{
		latestMinutelyNumber = mAutoBackupMinutelyArray_Number.Get(mAutoBackupMinutelyArray_Number.GetItemCount()-1);
	}
	//���݂̓����e�L�X�g�ƍŐV�̓����e�L�X�g���r���āA10���̌����オ�����Ă���΁A�o�b�N�A�b�v���s��
	if( currentNumber/10 > latestMinutelyNumber/10 || inAlways == true )
	{
		//�����o�b�N�A�b�v�t�H���_�擾
		AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
		if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
		{
			//�o�b�N�A�b�v�t�@�C���擾
			AFileAcc	backupfile;
			backupfile.SpecifyChild(minutelyFolder,currentDateTimeText);
			switch( mTextDocumentType )
			{
				//���[�J���t�@�C���̏ꍇ�́A�t�@�C���������o�b�N�A�b�v�t�@�C���ɃR�s�[����
			  case kTextDocumentType_Normal:
				{
					AFileAcc	srcfile;
					NVI_GetFile(srcfile);
					srcfile.CopyFileTo(backupfile,true);
					break;
				}
				//�����[�g�t�@�C���̏ꍇ�́A���[�h���ɓǂݍ��񂾁i���邢�́A�ۑ����ɋL�������j�e�L�X�g�������o�b�N�A�b�v�t�@�C���ɏ�������
			  case kTextDocumentType_RemoteFile:
				{
					backupfile.CreateFile();
					backupfile.WriteFile(mTextOnRemoteServer);
					break;
				}
				//
			  case kTextDocumentType_DiffTarget:
				{
					//�����ɂ͗��Ȃ��̂ŏ�������
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			//�o�b�N�A�b�v�t�@�C���̃e�L�X�g�G���R�[�f�B���O���L��
			if( mForMultiFileReplace == false )
			{
				AText	tecfilename;
				tecfilename.SetText(currentDateTimeText);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(minutelyFolder,tecfilename);
				tecfile.CreateFile();
				tecfile.WriteFile(mTextEncodingWhenLastLoadOrSave);
			}
			//�ő吢�㐔-1�ɂȂ�܂ŁA�ł��Â��t�@�C�����폜����
			while( mAutoBackupMinutelyArray_Number.GetItemCount() > kAutoBackup_MaxGenerations_Minutely-1 )
			{
				//�t�@�C�����X�g�̍ŏ��̓����e�L�X�g���擾
				AText	deletefilename;
				deletefilename.SetNumber64bit(mAutoBackupMinutelyArray_Number.Get(0));
				//�폜�t�@�C�����擾
				AFileAcc	deletefile;
				deletefile.SpecifyChild(minutelyFolder,deletefilename);
				//�t�@�C�����폜
				deletefile.DeleteFile();
				//�t�@�C�����X�g�̍��ڂ��폜
				mAutoBackupMinutelyArray_Number.Delete1(0);
				//�e�L�X�g�G���R�[�f�B���O�L���t�@�C�����폜
				AText	tecfilename;
				tecfilename.SetText(deletefilename);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(minutelyFolder,tecfilename);
				tecfile.DeleteFile();
			}
			//�t�@�C�����X�g���X�V
			GetAutoBackupNumberArray(minutelyFolder,mAutoBackupMinutelyArray_Number);
			//�u�����o�b�N�A�b�v�Ɣ�r�v���j���[���X�V
			SPI_UpdateComopareWithAutoBackupMenu();
		}
	}
	//===============Hourly�o�b�N�A�b�v===============
	//�����ƃo�b�N�A�b�v�̃t�@�C�����X�g����ŐV�̂��̂̓����e�L�X�g���擾
	ANumber64bit	latestHourlyNumber = 0;
	if( mAutoBackupHourlyArray_Number.GetItemCount() > 0 )
	{
		latestHourlyNumber = mAutoBackupHourlyArray_Number.Get(mAutoBackupHourlyArray_Number.GetItemCount()-1);
	}
	//���݂̓����e�L�X�g�ƍŐV�̓����e�L�X�g���r���āA���̌����オ�����Ă���΁A�o�b�N�A�b�v���s��
	if( currentNumber/100 > latestHourlyNumber/100 || inAlways == true )
	{
		//�����o�b�N�A�b�v�t�H���_�擾
		AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
		if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
		{
			//�o�b�N�A�b�v�t�@�C���擾
			AFileAcc	backupfile;
			backupfile.SpecifyChild(hourlyFolder,currentDateTimeText);
			switch( mTextDocumentType )
			{
				//���[�J���t�@�C���̏ꍇ�́A�t�@�C���������o�b�N�A�b�v�t�@�C���ɃR�s�[����
			  case kTextDocumentType_Normal:
				{
					AFileAcc	srcfile;
					NVI_GetFile(srcfile);
					srcfile.CopyFileTo(backupfile,true);
					break;
				}
				//�����[�g�t�@�C���̏ꍇ�́A���[�h���ɓǂݍ��񂾁i���邢�́A�ۑ����ɋL�������j�e�L�X�g�������o�b�N�A�b�v�t�@�C���ɏ�������
			  case kTextDocumentType_RemoteFile:
				{
					backupfile.CreateFile();
					backupfile.WriteFile(mTextOnRemoteServer);
					break;
				}
				//
			  case kTextDocumentType_DiffTarget:
				{
					//�����ɂ͗��Ȃ��̂ŏ�������
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			//�o�b�N�A�b�v�t�@�C���̃e�L�X�g�G���R�[�f�B���O���L��
			if( mForMultiFileReplace == false )
			{
				AText	tecfilename;
				tecfilename.SetText(currentDateTimeText);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(hourlyFolder,tecfilename);
				tecfile.CreateFile();
				tecfile.WriteFile(mTextEncodingWhenLastLoadOrSave);
			}
			//�ő吢�㐔-1�ɂȂ�܂ŁA�ł��Â��t�@�C�����폜����
			while( mAutoBackupHourlyArray_Number.GetItemCount() > kAutoBackup_MaxGenerations_Hourly-1 )
			{
				//�t�@�C�����X�g�̍ŏ��̓����e�L�X�g���擾
				AText	deletefilename;
				deletefilename.SetNumber64bit(mAutoBackupHourlyArray_Number.Get(0));
				//�폜�t�@�C�����擾
				AFileAcc	deletefile;
				deletefile.SpecifyChild(hourlyFolder,deletefilename);
				//�t�@�C�����폜
				deletefile.DeleteFile();
				//�t�@�C�����X�g�̍��ڂ��폜
				mAutoBackupHourlyArray_Number.Delete1(0);
				//�e�L�X�g�G���R�[�f�B���O�L���t�@�C�����폜
				AText	tecfilename;
				tecfilename.SetText(deletefilename);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(hourlyFolder,tecfilename);
				tecfile.DeleteFile();
			}
			//�t�@�C�����X�g���X�V
			GetAutoBackupNumberArray(hourlyFolder,mAutoBackupHourlyArray_Number);
			//�u�����o�b�N�A�b�v�Ɣ�r�v���j���[���X�V
			SPI_UpdateComopareWithAutoBackupMenu();
		}
	}
	//===============Daily�o�b�N�A�b�v===============
	//�����ƃo�b�N�A�b�v�̃t�@�C�����X�g����ŐV�̂��̂̓����e�L�X�g���擾
	ANumber64bit	latestDailyNumber = 0;
	if( mAutoBackupDailyArray_Number.GetItemCount() > 0 )
	{
		latestDailyNumber = mAutoBackupDailyArray_Number.Get(mAutoBackupDailyArray_Number.GetItemCount()-1);
	}
	//���݂̓����e�L�X�g�ƍŐV�̓����e�L�X�g���r���āA���̌����オ�����Ă���΁A�o�b�N�A�b�v���s��
	if( currentNumber/10000 > latestDailyNumber/10000 || inAlways == true )
	{
		//�����o�b�N�A�b�v�t�H���_�擾
		AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
		if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
		{
			//�o�b�N�A�b�v�t�@�C���擾
			AFileAcc	backupfile;
			backupfile.SpecifyChild(dailyFolder,currentDateTimeText);
			switch( mTextDocumentType )
			{
				//���[�J���t�@�C���̏ꍇ�́A�t�@�C���������o�b�N�A�b�v�t�@�C���ɃR�s�[����
			  case kTextDocumentType_Normal:
				{
					AFileAcc	srcfile;
					NVI_GetFile(srcfile);
					srcfile.CopyFileTo(backupfile,true);
					break;
				}
				//�����[�g�t�@�C���̏ꍇ�́A���[�h���ɓǂݍ��񂾁i���邢�́A�ۑ����ɋL�������j�e�L�X�g�������o�b�N�A�b�v�t�@�C���ɏ�������
			  case kTextDocumentType_RemoteFile:
				{
					backupfile.CreateFile();
					backupfile.WriteFile(mTextOnRemoteServer);
					break;
				}
				//
			  case kTextDocumentType_DiffTarget:
				{
					//�����ɂ͗��Ȃ��̂ŏ�������
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			//�o�b�N�A�b�v�t�@�C���̃e�L�X�g�G���R�[�f�B���O���L��
			if( mForMultiFileReplace == false )
			{
				AText	tecfilename;
				tecfilename.SetText(currentDateTimeText);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(dailyFolder,tecfilename);
				tecfile.CreateFile();
				tecfile.WriteFile(mTextEncodingWhenLastLoadOrSave);
			}
			//�ő吢�㐔-1�ɂȂ�܂ŁA�ł��Â��t�@�C�����폜����
			while( mAutoBackupDailyArray_Number.GetItemCount() > kAutoBackup_MaxGenerations_Daily-1 )
			{
				//�t�@�C�����X�g�̍ŏ��̓����e�L�X�g���擾
				AText	deletefilename;
				deletefilename.SetNumber64bit(mAutoBackupDailyArray_Number.Get(0));
				//�폜�t�@�C�����擾
				AFileAcc	deletefile;
				deletefile.SpecifyChild(dailyFolder,deletefilename);
				//�t�@�C�����폜
				deletefile.DeleteFile();
				//�t�@�C�����X�g�̍��ڂ��폜
				mAutoBackupDailyArray_Number.Delete1(0);
				//�e�L�X�g�G���R�[�f�B���O�L���t�@�C�����폜
				AText	tecfilename;
				tecfilename.SetText(deletefilename);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(dailyFolder,tecfilename);
				tecfile.DeleteFile();
			}
			//�t�@�C�����X�g���X�V
			GetAutoBackupNumberArray(dailyFolder,mAutoBackupDailyArray_Number);
			//�u�����o�b�N�A�b�v�Ɣ�r�v���j���[���X�V
			SPI_UpdateComopareWithAutoBackupMenu();
		}
	}
	//===============Monthly�o�b�N�A�b�v===============
	//�����ƃo�b�N�A�b�v�̃t�@�C�����X�g����ŐV�̂��̂̓����e�L�X�g���擾
	ANumber64bit	latestMonthlyNumber = 0;
	if( mAutoBackupMonthlyArray_Number.GetItemCount() > 0 )
	{
		latestMonthlyNumber = mAutoBackupMonthlyArray_Number.Get(mAutoBackupMonthlyArray_Number.GetItemCount()-1);
	}
	//���݂̓����e�L�X�g�ƍŐV�̓����e�L�X�g���r���āA���̌����オ�����Ă���΁A�o�b�N�A�b�v���s��
	if( currentNumber/1000000 > latestMonthlyNumber/1000000 || inAlways == true )
	{
		//�����o�b�N�A�b�v�t�H���_�擾
		AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
		if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
		{
			//�o�b�N�A�b�v�t�@�C���擾
			AFileAcc	backupfile;
			backupfile.SpecifyChild(monthlyFolder,currentDateTimeText);
			switch( mTextDocumentType )
			{
				//���[�J���t�@�C���̏ꍇ�́A�t�@�C���������o�b�N�A�b�v�t�@�C���ɃR�s�[����
			  case kTextDocumentType_Normal:
				{
					AFileAcc	srcfile;
					NVI_GetFile(srcfile);
					srcfile.CopyFileTo(backupfile,true);
					break;
				}
				//�����[�g�t�@�C���̏ꍇ�́A���[�h���ɓǂݍ��񂾁i���邢�́A�ۑ����ɋL�������j�e�L�X�g�������o�b�N�A�b�v�t�@�C���ɏ�������
			  case kTextDocumentType_RemoteFile:
				{
					backupfile.CreateFile();
					backupfile.WriteFile(mTextOnRemoteServer);
					break;
				}
				//
			  case kTextDocumentType_DiffTarget:
				{
					//�����ɂ͗��Ȃ��̂ŏ�������
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			//�o�b�N�A�b�v�t�@�C���̃e�L�X�g�G���R�[�f�B���O���L��
			if( mForMultiFileReplace == false )
			{
				AText	tecfilename;
				tecfilename.SetText(currentDateTimeText);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(monthlyFolder,tecfilename);
				tecfile.CreateFile();
				tecfile.WriteFile(mTextEncodingWhenLastLoadOrSave);
			}
			//�ő吢�㐔-1�ɂȂ�܂ŁA�ł��Â��t�@�C�����폜����
			while( mAutoBackupMonthlyArray_Number.GetItemCount() > kAutoBackup_MaxGenerations_Monthly-1 )
			{
				//�t�@�C�����X�g�̍ŏ��̓����e�L�X�g���擾
				AText	deletefilename;
				deletefilename.SetNumber64bit(mAutoBackupMonthlyArray_Number.Get(0));
				//�폜�t�@�C�����擾
				AFileAcc	deletefile;
				deletefile.SpecifyChild(monthlyFolder,deletefilename);
				//�t�@�C�����폜
				deletefile.DeleteFile();
				//�t�@�C�����X�g�̍��ڂ��폜
				mAutoBackupMonthlyArray_Number.Delete1(0);
				//�e�L�X�g�G���R�[�f�B���O�L���t�@�C�����폜
				AText	tecfilename;
				tecfilename.SetText(deletefilename);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(monthlyFolder,tecfilename);
				tecfile.DeleteFile();
			}
			//�t�@�C�����X�g���X�V
			GetAutoBackupNumberArray(monthlyFolder,mAutoBackupMonthlyArray_Number);
			//�u�����o�b�N�A�b�v�Ɣ�r�v���j���[���X�V
			SPI_UpdateComopareWithAutoBackupMenu();
		}
	}
}

/**
auto backup/doc pref�p�̃p�X�e�L�X�g���擾
*/
void	ADocument_Text::GetPathTextForAutoBackupAndDocPref( AText& outPath ) const
{
	//==================
	//���ʏ�����
	outPath.DeleteAll();
	switch( mTextDocumentType )
	{
		//------------------���[�J���t�@�C���̏ꍇ------------------
	  case kTextDocumentType_Normal:
		{
			//�t�@�C���p�X���擾
			NVI_GetFilePath(outPath);
			break;
		}
		//------------------diff�^�[�Q�b�g�h�L�������g�̏ꍇ------------------
	  case kTextDocumentType_DiffTarget:
		{
			//�t�@�C���p�X���擾
			mFileForDiffTargetDocument.GetPath(outPath);
			break;
		}
		//------------------�����[�g�t�@�C���̏ꍇ------------------
	  case kTextDocumentType_RemoteFile:
		{
			//URL�̂����T�[�o�[�e�L�X�g�����̃R�����A�X���b�V�����A���_�[�o�[�ɒu�����������̂��擾����B
			
			//�����[�g�t�@�C����URL���擾
			outPath.SetText(mRemoteFileURL);
			//"://"�̈ʒu���擾
			AIndex	foundpos = kIndex_Invalid;
			if( mRemoteFileURL.FindCstring(0,"://",foundpos) == true )
			{
				if( mRemoteFileURL.FindCstring(foundpos+3,"/",foundpos) == true )
				{
					//"://"����̍ŏ���"/"�̈ʒu��pos�Ɋi�[
					AIndex	pos = foundpos;
					//�T�[�o�[�e�L�X�g�����A�p�X�e�L�X�g�����ɕ����ăe�L�X�g���擾
					AText	servertext, pathtext;
					mRemoteFileURL.GetText(0,pos,servertext);
					mRemoteFileURL.GetText(pos,mRemoteFileURL.GetItemCount()-pos,pathtext);
					//�T�[�o�[�e�L�X�g�����̃R�����A�X���b�V�����A���_�[�o�[�ɒu��������
					servertext.ReplaceChar(':','_');
					servertext.ReplaceChar('/','_');
					//�T�[�o�[�e�L�X�g�����A�p�X�e�L�X�g����������
					outPath.SetText(servertext);
					outPath.AddText(pathtext);
				}
			}
			break;
		}
	  default:
		{
			return;
			break;
		}
	}
}

/**
�o�b�N�A�b�v�t�H���_�E�h�L�������g�ۑ���t�H���_�擾
*/
ABool	ADocument_Text::GetAutoBackupFolder( AFileAcc& outMonthlyFolder, AFileAcc& outDailyFolder,
		AFileAcc& outHourlyFolder, AFileAcc& outMinutelyFolder ) const
{
	//�����o�b�N�A�b�v�p�p�X�e�L�X�g�擾
	AText	docfilepath;
	GetPathTextForAutoBackupAndDocPref(docfilepath);
	if( docfilepath.GetItemCount() == 0 )
	{
		//�t�@�C���p�X����i��specify�j�̏ꍇ�́A�擾�s��
		return false;
	}
	else
	{
		//�����o�b�N�A�b�v�p���[�g�t�H���_�擾
		AFileAcc	autoBackupRootFolder;
		GetApp().SPI_GetAutoBackupRootFolder(autoBackupRootFolder);
		//���̃h�L�������g�̎����o�b�N�A�b�v�ۑ��t�H���_�擾
		AFileAcc	autoSaveFolder;
		autoSaveFolder.SpecifyChild(autoBackupRootFolder,docfilepath);
		autoSaveFolder.CreateFolderRecursive();
		//�e�o�b�N�A�b�v�t�H���_����
		outMonthlyFolder.SpecifyChild(autoSaveFolder,"Monthly");
		outMonthlyFolder.CreateFolder();
		outDailyFolder.SpecifyChild(autoSaveFolder,"Daily");
		outDailyFolder.CreateFolder();
		outHourlyFolder.SpecifyChild(autoSaveFolder,"Hourly");
		outHourlyFolder.CreateFolder();
		outMinutelyFolder.SpecifyChild(autoSaveFolder,"Minutely");
		outMinutelyFolder.CreateFolder();
		return true;
	}
}

/**
�����o�b�N�A�b�v�t�H���_�̃t�@�C���̓����e�L�X�g���X�g���擾
*/
void	ADocument_Text::GetAutoBackupNumberArray( const AFileAcc& inFolder, AArray<ANumber64bit>& outArray ) 
{
	//���ʏ�����
	outArray.DeleteAll();
	//���݂̓����e�L�X�g���擾
	AText	currentDateTimeText;
	GetAutoSaveCurrentDateTimeText(currentDateTimeText);
	ANumber64bit	currentNumber = currentDateTimeText.GetNumber64bit();
	//�t�H���_�̃t�@�C�����X�g���擾
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	//�e�t�@�C���ɂ��Ẵ��[�v
	AItemCount	itemCount = children.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//�t�@�C�����擾
		AText	filename;
		children.GetObjectConst(i).GetFilename(filename);
		//�t�@�C�����𐔒l��
		ANumber64bit	num = filename.GetNumber64bit();
		//���m�������e�L�X�g�̃t�H�[�}�b�g�Ɉ�v���Ă��Ȃ��A�܂��́A���݂�����̏ꍇ�́A�������Ȃ��B
		if( filename.GetItemCount() != 12 || num < 200001010000LL || num > currentNumber )
		{
			continue;
		}
		//���X�g�ɒǉ�
		outArray.Add(num);
	}
	//���X�g���\�[�g
	outArray.Sort(true);
}

/**
�����o�b�N�A�b�v�p ���݂̓����e�L�X�g���擾
*/
void	ADocument_Text::GetAutoSaveCurrentDateTimeText( AText& outText ) 
{
	outText.SetCstring("20");
	AText	t;
	ADateTimeWrapper::Get6LettersDateText(t);
	outText.AddText(t);
	ADateTimeWrapper::Get2LettersHourText(t);
	outText.AddText(t);
	ADateTimeWrapper::Get2LettersMinuteText(t);
	outText.AddText(t);
}

/**
�����o�b�N�A�b�v�Ƃ̔�r���j���[���X�V
*/
void	ADocument_Text::SPI_UpdateComopareWithAutoBackupMenu()
{
	{
		//���j���[���ڑS�폜
		GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_CompareWithAutoBackup_Monthly);
		//�����t�@�C�����X�g���ږ��̃��[�v
		AItemCount	itemCount = mAutoBackupMonthlyArray_Number.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//�t�@�C����(action text)�A���j���[�e�L�X�g�擾
			ANumber64bit	num = mAutoBackupMonthlyArray_Number.Get(i);
			AText	filename;
			filename.SetNumber64bit(num);
			AText	menutext;
			GetAutoBackupCompareMenuText(num,menutext);
			//���j���[�ɒǉ�
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_CompareWithAutoBackup_Monthly,0,menutext,filename,kObjectID_Invalid,NULL,0,false);
		}
	}
	{
		//���j���[���ڑS�폜
		GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_CompareWithAutoBackup_Daily);
		//�����t�@�C�����X�g���ږ��̃��[�v
		AItemCount	itemCount = mAutoBackupDailyArray_Number.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//�t�@�C����(action text)�A���j���[�e�L�X�g�擾
			ANumber64bit	num = mAutoBackupDailyArray_Number.Get(i);
			AText	filename;
			filename.SetNumber64bit(num);
			AText	menutext;
			GetAutoBackupCompareMenuText(num,menutext);
			//���j���[�ɒǉ�
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_CompareWithAutoBackup_Daily,0,menutext,filename,kObjectID_Invalid,NULL,0,false);
		}
	}
	{
		//���j���[���ڑS�폜
		GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_CompareWithAutoBackup_Hourly);
		//�����t�@�C�����X�g���ږ��̃��[�v
		AItemCount	itemCount = mAutoBackupHourlyArray_Number.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//�t�@�C����(action text)�A���j���[�e�L�X�g�擾
			ANumber64bit	num = mAutoBackupHourlyArray_Number.Get(i);
			AText	filename;
			filename.SetNumber64bit(num);
			AText	menutext;
			GetAutoBackupCompareMenuText(num,menutext);
			//���j���[�ɒǉ�
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_CompareWithAutoBackup_Hourly,0,menutext,filename,kObjectID_Invalid,NULL,0,false);
		}
	}
	{
		//���j���[���ڑS�폜
		GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_CompareWithAutoBackup_Minutely);
		//�����t�@�C�����X�g���ږ��̃��[�v
		AItemCount	itemCount = mAutoBackupMinutelyArray_Number.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//�t�@�C����(action text)�A���j���[�e�L�X�g�擾
			ANumber64bit	num = mAutoBackupMinutelyArray_Number.Get(i);
			AText	filename;
			filename.SetNumber64bit(num);
			AText	menutext;
			GetAutoBackupCompareMenuText(num,menutext);
			//���j���[�ɒǉ�
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_CompareWithAutoBackup_Minutely,0,menutext,filename,kObjectID_Invalid,NULL,0,false);
		}
	}
}

/**
�u�����o�b�N�A�b�v�Ɣ�r�v���j���[�̃��j���[�\���e�L�X�g���擾
*/
void	ADocument_Text::GetAutoBackupCompareMenuText( const ANumber64bit inNumber, AText& outText ) const
{
	//inNumber��N���������ɕ�����
	ANumber	year = (inNumber/100000000LL)%10000LL;
	ANumber	month = (inNumber/1000000LL)%100LL;
	ANumber	day = (inNumber/10000LL)%100LL;
	ANumber	hour = (inNumber/100LL)%100LL;
	ANumber	minute = (inNumber)%100LL;
	//inNumber�ɑΉ�����date time�e�L�X�g���擾�i�b�͍폜�j
	ADateTime	datetime = ADateTimeWrapper::GetDateTime(year,month,day,hour,minute,0);
	ADateTimeWrapper::GetDateTimeText(datetime,outText);
	outText.Delete(outText.GetItemCount()-3,3);
	//inNumber�ɑΉ�����date time�ƌ��ݓ����Ƃ̍������擾
	ANumber	years = 0, months = 0, days = 0, hours = 0, minutes = 0, seconds = 0;
	ADateTimeWrapper::GetDateTimeDiff(ADateTimeWrapper::GetCurrentTime(),datetime,
				years,months,days,hours,minutes,seconds);
	//�������ԃe�L�X�g��ǉ�
	outText.AddCstring("  ( ");
	AText	t;
	if( years > 0 )
	{
		t.SetLocalizedText("Year");
		t.ReplaceParamText('1',years);
		outText.AddText(t);
	}
	if( months > 0 )
	{
		t.SetLocalizedText("Month");
		t.ReplaceParamText('1',months);
		outText.AddText(t);
	}
	if( days > 0 )
	{
		t.SetLocalizedText("Day");
		t.ReplaceParamText('1',days);
		outText.AddText(t);
	}
	if( hours > 0 )
	{
		t.SetLocalizedText("Hour");
		t.ReplaceParamText('1',hours);
		outText.AddText(t);
	}
	if( minutes > 0 || (years==0&&months==0&&days==0&&hours==0))
	{
		t.SetLocalizedText("Minute");
		t.ReplaceParamText('1',minutes);
		outText.AddText(t);
	}
	{
		t.SetLocalizedText("Before");
		outText.AddText(t);
	}
	outText.AddCstring(" )");
}

/**
�����ƃo�b�N�A�b�v�t�@�C�����擾
*/
ABool	ADocument_Text::SPI_GetAutoBackupMinutelyFile( const AText& inFilename, AFileAcc& outFile )
{
	//�����o�b�N�A�b�v�t�H���_���擾
	AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
	if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
	{
		//�o�b�N�A�b�v�t�@�C�����擾
		outFile.SpecifyChild(minutelyFolder,inFilename);
		return true;
	}
	else
	{
		return false;
	}
}

/**
���Ԃ��ƃo�b�N�A�b�v�t�@�C�����擾
*/
ABool	ADocument_Text::SPI_GetAutoBackupHourlyFile( const AText& inFilename, AFileAcc& outFile )
{
	//�����o�b�N�A�b�v�t�H���_���擾
	AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
	if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
	{
		//�o�b�N�A�b�v�t�@�C�����擾
		outFile.SpecifyChild(hourlyFolder,inFilename);
		return true;
	}
	else
	{
		return false;
	}
}

/**
�����ƃo�b�N�A�b�v�t�@�C�����擾
*/
ABool	ADocument_Text::SPI_GetAutoBackupDailyFile( const AText& inFilename, AFileAcc& outFile )
{
	//�����o�b�N�A�b�v�t�H���_���擾
	AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
	if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
	{
		//�o�b�N�A�b�v�t�@�C�����擾
		outFile.SpecifyChild(dailyFolder,inFilename);
		return true;
	}
	else
	{
		return false;
	}
}

/**
�����ƃo�b�N�A�b�v�t�@�C�����擾
*/
ABool	ADocument_Text::SPI_GetAutoBackupMonthlyFile( const AText& inFilename, AFileAcc& outFile )
{
	//�����o�b�N�A�b�v�t�H���_���擾
	AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
	if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
	{
		//�o�b�N�A�b�v�t�@�C�����擾
		outFile.SpecifyChild(monthlyFolder,inFilename);
		return true;
	}
	else
	{
		return false;
	}
}

#pragma mark ===========================

#pragma mark ---���ۑ��o�b�N�A�b�v
//#81

//���ۑ��o�b�N�A�b�v�̃C���^�[�o���^�C�}�[
const ANumber	kAutoBackup_UnsavedTextIntervalMinute = 5;

/**
���ۑ��f�[�^�̎����ۑ��i�r���[deactivate���A�^�C�}�[�o�ߎ��ɃR�[�������j
*/
void	ADocument_Text::SPI_AutoSave_Unsaved()
{
	//�O�񎩓��ۑ�����̕ҏW�����邩�ǂ����̃t���O�𔻒肵�A�Ȃ���Ή������Ȃ�
	if( mAutoSaveDirty == false )
	{
		return;
	}
	
	//���݂̓����e�L�X�g���擾
	AText	currentDateTimeText;
	GetAutoSaveCurrentDateTimeText(currentDateTimeText);
	ANumber64bit	currentNumber = currentDateTimeText.GetNumber64bit();
	//���ۑ��o�b�N�A�b�v�̃t�@�C�����X�g����ŐV�̂��̂̓����e�L�X�g���擾
	ANumber64bit	latestUnsavedTextNumber = 0;
	if( mAutoBackupUnsavedDataArray_Number.GetItemCount() > 0 )
	{
		latestUnsavedTextNumber = mAutoBackupUnsavedDataArray_Number.Get(mAutoBackupUnsavedDataArray_Number.GetItemCount()-1);
	}
	//���݂̓����e�L�X�g�ƍŐV�̓����e�L�X�g���r���āA�C���^�[�o���^�C�}�[��؂�𒴂��Ă���΁A�o�b�N�A�b�v���s��
	if( currentNumber/kAutoBackup_UnsavedTextIntervalMinute > latestUnsavedTextNumber/kAutoBackup_UnsavedTextIntervalMinute )
	{
		//�o�b�N�A�b�v�t�@�C���擾
		AFileAcc	backupfile;
		backupfile.SpecifyChild(mAutoBackupUnsavedDataFolder,currentDateTimeText);
		backupfile.CreateFile();
		//�e�L�X�g���t�@�C���ɕۑ�
		backupfile.WriteFile(mText);
		//�t�@�C���p�X�擾���A"filepath"�t�@�C���ɕۑ�
		AText	filepath;
		switch( mTextDocumentType )
		{
		  case kTextDocumentType_Normal:
			{
				NVI_GetFilePath(filepath);
				break;
			}
		  case kTextDocumentType_RemoteFile:
			{
				filepath.SetText(mRemoteFileURL);
				break;
			}
			//
		  case kTextDocumentType_DiffTarget:
			{
				//�����ɂ͗��Ȃ��̂ŏ�������
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		AFileAcc	filepathfile;
		filepathfile.SpecifyChild(mAutoBackupUnsavedDataFolder,"filepath");
		filepathfile.CreateFile();
		filepathfile.WriteFile(filepath);
		
		//-----------------������unsaved file�͑S�폜-----------------
		//�imAutoBackupUnsavedDataArray_Number���X�g�Ƃ��̃t�@�C���������B��ō쐬�����t�@�C���͊܂܂�Ȃ��j
		AutoSave_RemoveUnsavedDataFile();
		
		//���X�g���X�V
		GetAutoBackupNumberArray(mAutoBackupUnsavedDataFolder,mAutoBackupUnsavedDataArray_Number);
		
		//�O�񎩓��ۑ�����̕ҏW�����邩�ǂ����̃t���O��OFF�ɂ���
		//�i����NVI_SetDirty()���R�[��������mAutoSaveDirty��true�ɂȂ�j
		mAutoSaveDirty = false;
	}
}

/**
unsaved file��S�폜����
@note �ۑ����ɂ��R�[�������B�ۑ�����unsaved���폜���Ă����Ȃ��ƁAsaved�t�@�C���̓��e��unsaved�����V�����Ȃ��Ă��܂��B
*/
void	ADocument_Text::AutoSave_RemoveUnsavedDataFile()
{
	//���X�g���̃f�[�^�������Ȃ�܂Ń��[�v
	while( mAutoBackupUnsavedDataArray_Number.GetItemCount() > 0 )
	{
		//���X�g�̍ŏ��̃e�L�X�g���擾
		AText	deletefilename;
		deletefilename.SetNumber64bit(mAutoBackupUnsavedDataArray_Number.Get(0));
		//�폜�t�@�C�����擾
		AFileAcc	deletefile;
		deletefile.SpecifyChild(mAutoBackupUnsavedDataFolder,deletefilename);
		//�t�@�C���폜
		deletefile.DeleteFile();
		//���X�g���ڍ폜
		mAutoBackupUnsavedDataArray_Number.Delete1(0);
	}
}

/**
���ۑ��f�[�^�o�b�N�A�b�v�t�H���_����
*/
void	ADocument_Text::CreateUnsavedDataBackupFolder()
{
	//���ۑ��f�[�^�o�b�N�A�b�v���[�g�t�H���_�擾
	AFileAcc	unsavedDataRootFolder;
	GetApp().SPI_GetUnsavedDataFolder(unsavedDataRootFolder);
	//"UnisavedData-x"�t�H���_���擾
	mAutoBackupUnsavedDataFolder.SpecifyUniqChildFile(unsavedDataRootFolder,"UnsavedData");
	mAutoBackupUnsavedDataFolder.CreateFolder();
	//���X�g�擾
	GetAutoBackupNumberArray(mAutoBackupUnsavedDataFolder,mAutoBackupUnsavedDataArray_Number);
}

/**
���ۑ��f�[�^�o�b�N�A�b�v�t�H���_�폜
*/
void	ADocument_Text::DeleteUnsavedDataBackupFolder()
{
	//"UnsavedData-x"�t�H���_���폜
	mAutoBackupUnsavedDataFolder.DeleteFileOrFolderRecursive();
}

#pragma mark ===========================

#pragma mark ---�e�L�X�g�`��p�f�[�^�擾

//�e�L�X�g�`��p�f�[�^�擾
//UTF16���A�^�u�̃X�y�[�X���A���䕶���̉��������s���B
//CTextDrawData�N���X�̂����A���L��ݒ肷��B
//UTF16DrawText: UTF16�ł̕`��p�e�L�X�g
//UnicodeDrawTextIndexArray: �C���f�b�N�X�F�h�L�������g���ێ����Ă��錳�̃e�L�X�g�̃C���f�b�N�X�@�l�F�`��e�L�X�g��Unicode�����P�ʂł̃C���f�b�N�X
void	ADocument_Text::SPI_GetTextDrawDataWithoutStyle( const AIndex inLineIndex, CTextDrawData& outTextDrawData ) const
{
	AStMutexLocker	locker(mTextMutex);
	GetTextDrawDataWithoutStyle(mText,mTextInfo,inLineIndex,outTextDrawData);
}
void	ADocument_Text::GetTextDrawDataWithoutStyle( const AText& inText, const ATextInfo& inTextInfo, const AIndex inLineIndex, CTextDrawData& outTextDrawData ) const//#695
{
	//#703
	outTextDrawData.SetFontFallbackEnabled(!mSuspectBinaryData);
	//
	inTextInfo.GetTextDrawDataWithoutStyle(/*#695 mText*/inText,SPI_GetTabWidth(),SPI_GetIndentWidth(),0,inLineIndex,outTextDrawData,true,//#117
										   GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters),false,true,
										   (SPI_IsJISTextEncoding() == true && GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kConvert5CToA5WhenOpenJIS) == true ));
}

//�e�L�X�g�`��p�f�[�^�擾
void	ADocument_Text::SPI_GetTextDrawDataWithStyle( const AIndex inLineIndex, CTextDrawData& outTextDrawData, 
		const ABool inSelectMode, const ATextPoint& inSelectTextPoint1, const ATextPoint& inSelectTextPoint2,
		AAnalyzeDrawData& ioAnalyzeDrawData, const AViewID inViewID ) const //RC1 #893
{
	AStMutexLocker	locker(mTextMutex);
	GetTextDrawDataWithStyle(mText,mTextInfo,inLineIndex,outTextDrawData,
							 inSelectMode,inSelectTextPoint1,inSelectTextPoint2,ioAnalyzeDrawData,inViewID);
}
void	ADocument_Text::GetTextDrawDataWithStyle( const AText& inText, const ATextInfoForDocument& inTextInfo, //#695
		const AIndex inLineIndex, CTextDrawData& outTextDrawData, 
		const ABool inSelectMode, const ATextPoint& inSelectTextPoint1, const ATextPoint& inSelectTextPoint2,
		AAnalyzeDrawData& ioAnalyzeDrawData, const AViewID inViewID ) const //RC1 #893
{
	//style�f�[�^�����̃f�[�^���擾
	GetTextDrawDataWithoutStyle(inText,inTextInfo,inLineIndex,outTextDrawData);//#695
	
	//�s�܂�Ԃ����v�Z�̏ꍇ�́Astyle�f�[�^�͕t�������ɏI��
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial || 
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened ||
	   SPI_GetDocumentInitState() == kDocumentInitState_WrapCalculating )
	{
		return;
	}
	
	//#893
	//text view��view id�w�肪����΁Aview index���擾
	AIndex	viewIndex = kIndex_Invalid;
	if( inViewID != kObjectID_Invalid )
	{
		viewIndex = mViewIDArray.Find(inViewID);
	}
	
	//R0199
	ABool	enableSpellCheck = false;
	
#if IMPLEMENTATION_FOR_MACOSX
	//�X�y���`�F�b�N���邩�ǂ������擾
	enableSpellCheck = ( /*#688 AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 && */
			GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kDisplaySpellCheck) == true);
#endif

	/*
	//�s�ɐF�w�肪����ꍇ�͂�����D��
	AColor	lineColor;
	if( inTextInfo.GetLineColor(inLineIndex,lineColor) == true )
	{
		outTextDrawData.attrPos.Add(0);
		outTextDrawData.attrColor.Add(lineColor);
		outTextDrawData.attrStyle.Add(kTextStyle_Normal);
	}
	else
	*/
	{
		AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex());
		ASyntaxDefinition&	syntaxDefinition = modePrefDB.GetSyntaxDefinition();//R0199
		//�s���擾
		AIndex	lineInfo_Start = inTextInfo.GetLineStart(inLineIndex);
		//#695 �I���ʒu��GetTextDrawDataWithoutStyle()�Ő������ꂽ�����ɏ]���悤�ɂ���AItemCount	lineInfo_LengthWithoutCR = inTextInfo.GetLineLengthWithoutLineEnd(inLineIndex);
		AIndex	stateIndex, pushedStateIndex;
		inTextInfo.GetLineStateIndex(inLineIndex,stateIndex,pushedStateIndex);
		
		//state color�L���t���O��false�ɏ����ݒ肷��
		ABool	stateColorValid = false;
		//�s����ԂɑΉ������F���擾�B��Ԃ��F�������Ȃ��ꍇ�́A���[�h�̕����F���擾�B
		AColor	normalColor;
		ATextStyle	normalTextStyle = kTextStyle_Normal;//#844
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetLetterColorForState(stateIndex,normalColor,normalTextStyle,stateColorValid);
		//#603 ���o�����K�\����v�O���[�v�̐F�Â�
		AColor	jumpsetupRegExpGroupColor = kColor_Black;
		AIndex	jumpsetupRegExpGroupColorSpos = kIndex_Invalid;
		AIndex	jumpsetupRegExpGroupColorEpos = kIndex_Invalid;
		inTextInfo.GetLineRegExpGroupColor(inLineIndex,jumpsetupRegExpGroupColor,
					jumpsetupRegExpGroupColorSpos,jumpsetupRegExpGroupColorEpos);
		//B0436
		ABool	lineColorValid = false;
		AColor	lineColor;
		if( inTextInfo.GetLineColor(inLineIndex,lineColor) == true )
		{
			normalColor = lineColor;
			lineColorValid = true;
		}
		//
		outTextDrawData.attrPos.Add(0);
		outTextDrawData.attrColor.Add(normalColor);
		outTextDrawData.attrStyle.Add(normalTextStyle);//#844
		//
		AIndex	skipKeywordRecognition = 0;//#592
		//�F�t�����v�Z�I���ʒu�擾
		//#695 �I���ʒu��GetTextDrawDataWithoutStyle()�Ő������ꂽ�����ɏ]���悤�ɂ���
		AItemCount	drawDataTextLength = 0;
		if( outTextDrawData.UTF16DrawTextArray_OriginalTextIndex.GetItemCount() > 0 )
		{
			//UTF16DrawTextArray_OriginalTextIndex�̍Ō�̗v�f�ɁA���̃e�L�X�g��end index�������Ă���
			drawDataTextLength = outTextDrawData.UTF16DrawTextArray_OriginalTextIndex.
					Get(outTextDrawData.UTF16DrawTextArray_OriginalTextIndex.GetItemCount()-1);
		}
		AIndex	endpos = lineInfo_Start + drawDataTextLength;//#695 lineInfo_LengthWithoutCR;
		//
		for( ATextIndex pos = lineInfo_Start; pos < endpos; )
		{
			//#603 ���o�����K�\����v�O���[�v�̐F�Â�
			if( jumpsetupRegExpGroupColorSpos != kIndex_Invalid )
			{
				//���݈ʒupos�����o�����K�\����v�O���[�v�F�Â��J�n�ʒu�����߂Ē������Ƃ��AattrPos/Color/Style��ǉ�
				if( pos > lineInfo_Start+jumpsetupRegExpGroupColorSpos )
				{
					outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.
								Get(jumpsetupRegExpGroupColorSpos));
					outTextDrawData.attrColor.Add(jumpsetupRegExpGroupColor);
					outTextDrawData.attrStyle.Add(kTextStyle_Normal);
					jumpsetupRegExpGroupColorSpos = kIndex_Invalid;
				}
			}
			if( jumpsetupRegExpGroupColorEpos != kIndex_Invalid )
			{
				//���݈ʒupos�����o�����K�\����v�O���[�v�F�Â��I���ʒu�����߂Ē������Ƃ��AattrPos/Color/Style��ǉ�
				if( pos > lineInfo_Start+jumpsetupRegExpGroupColorEpos )
				{
					outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.
								Get(jumpsetupRegExpGroupColorEpos));
					outTextDrawData.attrColor.Add(normalColor);//�ʏ핶���F�ɖ߂�
					outTextDrawData.attrStyle.Add(normalTextStyle);//#844 
					jumpsetupRegExpGroupColorEpos = kIndex_Invalid;
				}
			}
			//pos�ʒu����1�g�[�N�����
			AIndex	nextStateIndex;
			ABool	changeStateFromNextChar;
			ATextIndex	nextPos;
			ABool	seqFound;
			AText	token;
			ATextIndex	tokenspos;
			AItemCount	currentIndent = 0, nextIndent = 0;
			inTextInfo.RecognizeStateAndIndent(/*#695 mText*/inText,stateIndex,pushedStateIndex,
					pos,endpos,nextStateIndex,changeStateFromNextChar,nextPos,seqFound,token,tokenspos,0,0,currentIndent,nextIndent);
			
			AIndex	currentStateIndex = stateIndex;//R0000
			ABool	nextStateColorValid = stateColorValid;//#274
			ATextIndex	stateChangePos = kIndex_Invalid;//#435 ���݈ʒu�ȊO�����ԑJ�ڂ̏ꍇ�ɂ��̒l��ݒ肷�邱�Ƃɂ���
			//�A�N�V�����V�[�P���X�����������ꍇ
			if( seqFound == true )
			{
				//��ԑJ��
				if( stateIndex != nextStateIndex )
				{
					//��ԑJ��
					stateIndex = nextStateIndex;
					//��ԐF�擾�i��ԐF���Ȃ��ꍇ�́A���[�h�̕����F�擾�j
					if( lineColorValid == false )//B0436
					{
						GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetLetterColorForState(stateIndex,normalColor,normalTextStyle,nextStateColorValid);//#274 #844
					}
					//
					//#435 ATextIndex	stateChangePos = pos;
					if( changeStateFromNextChar == true )
					{
						stateChangePos = nextPos;
					}
					//#14 ���݂̕����񂩂��ԕύX�̏ꍇ�́AcurrentStateIndex��V�K��ԂɕύX����
					//</script>�̐F�Â��̂��߁B/script��javascirpt��Ԃ���AHTML�̃^�O����Ԃ֖߂��B
					else
					{
						currentStateIndex = nextStateIndex;
						stateColorValid = nextStateColorValid;//#274
					}
					//#435 changeStateFromNextChar�̏ꍇ�Awin�L�[���[�h��attr�ǉ���ɁA�ǉ��ʒu���f���Ēǉ����邱�Ƃɂ���
					//#435 if( stateChangePos <= endpos )
					if( stateChangePos == kIndex_Invalid && pos <= endpos )//#435
					{
						//���݈ʒu�����ԑJ��
						outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(pos-lineInfo_Start));
						outTextDrawData.attrColor.Add(normalColor);
						outTextDrawData.attrStyle.Add(normalTextStyle);//#844
					}
				}
			}
			//R0195 if( stateColorValid == false )
			if( tokenspos >= skipKeywordRecognition )//#633
			{
				//���@�p�[�gindex���擾
				AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
				//
				ATextIndex	p = tokenspos;
				GetKeywordToken(p,endpos,token,tokenspos,syntaxPartIndex);
				ABool	found = false;
				
				AColor	color;
				ATextStyle	textstyle;
				ABool	priorToOtherColor;//R0195
				//���ʎq����
				AObjectID	objectID = kObjectID_Invalid;
				if( viewIndex != kIndex_Invalid )//#893
				{
					//�w��s�����[�J���͈͓��Ȃ烍�[�J�����ʎq������
					if( inLineIndex >= mCurrentLocalIdentifierStartLineIndexArray.Get(viewIndex) && inLineIndex < mCurrentLocalIdentifierEndLineIndexArray.Get(viewIndex) )
					{
						objectID = mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).FindObjectID(token,0,token.GetItemCount());
					}
				}
				if( objectID != kObjectID_Invalid )
				{
					if( (stateColorValid == false && lineColorValid == false) )//R0195 B0436
					{
						AIndex	categoryIndex = mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).GetObjectConstByID(objectID).GetCategoryIndex();
						modePrefDB.GetCategoryLocalColor(categoryIndex,color);
						outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
						outTextDrawData.attrColor.Add(color);
						outTextDrawData.attrStyle.Add(/*R0195 kTextStyle_Normal*/modePrefDB.GetCategoryTextStyle(categoryIndex));
						outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
						outTextDrawData.attrColor.Add(normalColor);
						outTextDrawData.attrStyle.Add(normalTextStyle);//#844
						found = true;
					}
					
					//RC1
					if( ioAnalyzeDrawData.flagArrowToDef == true )
					{
						AIndex	defspos = mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).GetObjectConstByID(objectID).GetStartIndex();
						AIndex	defepos = mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).GetObjectConstByID(objectID).GetEndIndex();
						AIndex	refspos = tokenspos;
						AIndex	refepos = p;
						ATextPoint	defspt, defept, refspt, refept;
						SPI_GetTextPointFromTextIndex(defspos,defspt);
						SPI_GetTextPointFromTextIndex(defepos,defept);
						SPI_GetTextPointFromTextIndex(refspos,refspt);
						SPI_GetTextPointFromTextIndex(refepos,refept);
						ioAnalyzeDrawData.defStartArrowToDef.Add(defspt);
						ioAnalyzeDrawData.defEndArrowToDef.Add(defept);
						ioAnalyzeDrawData.refStartArrowToDef.Add(refspt);
						ioAnalyzeDrawData.refEndArrowToDef.Add(refept);
					}
				}
				else
				{
					//�O���[�o�����ʎq����
					AIndex	categoryIndex = inTextInfo.GetGlobalIdentifierCategoryIndexByKeywordText(token);
					if( categoryIndex != kIndex_Invalid )
					{
						if( (stateColorValid == false && lineColorValid == false) )//R0195 B0436
						{
							modePrefDB.GetCategoryColor(categoryIndex,color);
							outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
							outTextDrawData.attrColor.Add(color);
							outTextDrawData.attrStyle.Add(/*R0195 kTextStyle_Normal*/modePrefDB.GetCategoryTextStyle(categoryIndex));
							outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
							outTextDrawData.attrColor.Add(normalColor);
							outTextDrawData.attrStyle.Add(normalTextStyle);//#844
							found = true;
						}
					}
					//�L�[���[�h����
					if( found == false )
					{
						if( modePrefDB.FindKeyword(token,color,textstyle,priorToOtherColor,currentStateIndex) == true )//R0000
						{
							if( (stateColorValid == false && lineColorValid == false) || priorToOtherColor == true )//R0195 B0436
							{
								outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
								outTextDrawData.attrColor.Add(color);
								outTextDrawData.attrStyle.Add(textstyle);
								outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
								outTextDrawData.attrColor.Add(normalColor);
								outTextDrawData.attrStyle.Add(normalTextStyle);//#844
								found = true;
							}
						}
					}
					//�C���|�[�g�t�@�C���L�[���[�h����
					if( found == false )
					{
						if( mImportIdentifierLinkList.FindKeyword(token,categoryIndex) == true )
						{
							if( categoryIndex != kIndex_Invalid )
							{
								if( (stateColorValid == false && lineColorValid == false) )//R0195 B0436
								{
									modePrefDB.GetCategoryImportColor(categoryIndex,color);
									outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
									outTextDrawData.attrColor.Add(color);
									outTextDrawData.attrStyle.Add(/*R0195 kTextStyle_Normal*/modePrefDB.GetCategoryTextStyle(categoryIndex));
									outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
									outTextDrawData.attrColor.Add(normalColor);
									outTextDrawData.attrStyle.Add(normalTextStyle);//#844
									found = true;
								}
							}
						}
					}
					//�V�X�e���w�b�_�L�[���[�h���� #253
					if( found == false )
					{
						if( modePrefDB.GetSystemHeaderIdentifierLinkList().FindKeyword(token,categoryIndex) == true )
						{
							if( categoryIndex != kIndex_Invalid )
							{
								if( (stateColorValid == false && lineColorValid == false) )//R0195 B0436
								{
									modePrefDB.GetCategoryImportColor(categoryIndex,color);
									outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
									outTextDrawData.attrColor.Add(color);
									outTextDrawData.attrStyle.Add(/*R0195 kTextStyle_Normal*/modePrefDB.GetCategoryTextStyle(categoryIndex));
									outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
									outTextDrawData.attrColor.Add(normalColor);
									outTextDrawData.attrStyle.Add(normalTextStyle);//#844
									found = true;
								}
							}
						}
					}
					//RegExp�L�[���[�h���� 
					if( found == false )
					{
						categoryIndex = mTextInfo.MatchKeywordRegExp(token);
						if( categoryIndex != kIndex_Invalid )
						{
							if( modePrefDB.GetCategoryDisabled(stateIndex,categoryIndex) == false )
							{
								if( (stateColorValid == false && lineColorValid == false) || modePrefDB.GetCategoryPriorToOtherColor(categoryIndex) == true )//R0195 B0436
								{
									modePrefDB.GetCategoryColor(categoryIndex,color);
									textstyle = modePrefDB.GetCategoryTextStyle(categoryIndex);
									outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
									outTextDrawData.attrColor.Add(color);
									outTextDrawData.attrStyle.Add(textstyle);
									outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
									outTextDrawData.attrColor.Add(normalColor);
									outTextDrawData.attrStyle.Add(normalTextStyle);//#844
									found = true;
								}
							}
						}
					}
					/*�F�e�L�X�g�����̐F�ŕ\����drop �F�ɂ���Ă̓e�L�X�g�������Ȃ��E�E�E
					if( found == false )
					{
						if( token.GetItemCount() == 6 )
						{
							ABool	isHTMLColor = true;
							for( AIndex i = 0; i < 6; i++ )
							{
								AUChar	ch = token.Get(i);
								if( ch >= '0' && ch <= '9' )
								{
								}
								else if( ch >= 'A' && ch <= 'F' )
								{
								}
								else if( ch >= 'a' && ch <= 'f' )
								{
								}
								else
								{
									isHTMLColor = false;
									break;
								}
							}
							//
							if( isHTMLColor == true )
							{
								outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
								outTextDrawData.attrColor.Add(AColorWrapper::GetColorByHTMLFormatColor(token));
								outTextDrawData.attrStyle.Add(kTextStyle_Normal);
								outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
								outTextDrawData.attrColor.Add(normalColor);
								outTextDrawData.attrStyle.Add(normalTextStyle);
								found = true;
							}
						}
					}
					*/
				}
				
				//�����ł̊e�ϐ��̏�ԁFnextPos:���@��͂ɂ�鎟�̈ʒu p:1token�ǂݍ��݌�̈ʒu
				
				//�L�[���[�h��v������A���̓ǂݍ��݈ʒu�́A�L�[���[�h�̎��̕����ɂ���B
				//�L�[���[�h��v���Ȃ��ꍇ�́A���̓ǂݍ��݈ʒu�́A�i��{�I�ɂ́j���@��͂Ō��肵�������̈ʒu
				if( found == true )
				{
					//B0427 �G�X�P�[�v�������o�b�N�X���b�V���ŁA�L�[���[�h�ɂ��o�b�N�X���b�V�������݂���ꍇ�A
					//�u\"�v�Ƃ���������ɂ��āA���@��͂ł�nextPos��"�̂��Ƃɂ��邪�A�L�[���[�h�����݂��邱�Ƃɂ��\�̂��ƂɂȂ��Ă��܂�
					//���@��͌��ʂ̈ʒu�����炷�K�v�͂Ȃ��Ǝv����̂ŁA���̏����͍폜
					/*
					nextPos = p;
					*/
					//R0199
					//pos�̓��[�v�Ō��nextPos�ōX�V�����
					//nextPos: ���@���(RecognizeStateAndIndent)�̌��ʈʒu
					//p: �L�[���[�htoken�擾�̌��ʈʒu
					//�L�[���[�h��v�̏ꍇ�́A�L�[���[�h�̌��pos�������Ă��Ȃ��ƁA
					//���Ƃ��΁Atex��"\samepage"�ɂāApos��"\"�̌�ɂȂ��Ă��܂��A���̃��[�v��samepage���~�X�X�y���ΏۂɂȂ��Ă��܂��B
					//nextPos��p���ׂāA����̂ق��̈ʒu��pos�X�V�������B
					//nextPos�̕�����Ȃ炻�̂܂܂�OK�AnextPos�̕�����Ȃ�p�ōX�V����i�L�[���[�htoken�̌�̈ʒu���̗p����B�j
					/*#592
					������nextPos���X�V���Ă��܂��ƁA���@��͂����������s���Ȃ��B�i�����𔲂����ĕ��@�F�����Ă��܂��j
					if( nextPos < p )
					{
						nextPos = p;
					}
					*/
					//skipKeywordRecognition: ���ȍ~�̃��[�v�ŁA���̈ʒu�𒴂���܂ŁA�L�[���[�h�F�������͍s��Ȃ�
					//nextPos�́A���@�F���ɂ�鎟�ʒu�ł��邽�߁A1token�����O�ɂȂ邱�Ƃ�����B
					//���̏ꍇ�A���@�F���𐳂����s���A���A�r������L�[���[�h�������Ȃ��悤�ɂ���K�v������B
					skipKeywordRecognition = p;//#592 #633 skipKeywordRecognition�𒴂���܂ł̓L�[���[�h�F���𖳌��ɂ��� #633
				}
				//R0194
				else
				{
					//�}���`�o�C�g�����̏ꍇ�́AnextPos�́Atoken�̍Ō�ɂ���B
					if( token.GetItemCount() > 0 )
					{
						AUChar	ch = token.Get(0);
						if( token.IsUTF8MultiByte(ch) == true )
						{
							nextPos = p;
						}
					}
				}
#if IMPLEMENTATION_FOR_MACOSX
				//R0199 �����X�y���`�F�b�N
				//�L�[���[�h�Ȃ��A���A�ݒ�On�A���Atoken�̕�������2�ȏ�i1�����̓`�F�b�N�ΏۊO�j�̏ꍇ�Ɏ��{
				if( found == false && enableSpellCheck == true && token.GetItemCount() > 1 &&
							syntaxDefinition.GetSyntaxDefinitionState(currentStateIndex).IsNoSpellCheck() == false )
				{
					//U+2000�����̕����Ŏn�܂�P��̂݃`�F�b�N�Ώ�
					if( outTextDrawData.UTF16DrawText.GetUTF16CharFromUTF16Text(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start)*sizeof(AUTF16Char)) < 0x2000 )
					{
						//#371 �A�|�X�g���t�B�ŃX�y���G���[�ɂȂ�����C��
						//�P��̒���ɃA�|�X�g���t�B�{�A���t�@�x�b�g�����݂��Ă����炻����ǉ����ăX�y���`�F�b�N
						if( p < endpos )
						{
							ATextIndex	p2 = p, tokenspos2;
							AText	token2;
							GetKeywordToken(p2,endpos,token2,tokenspos2,syntaxPartIndex);
							if( token2.Compare("'") == true )
							{
								if( p2 < endpos )
								{
									ATextIndex	p3 = p2, tokenspos3;
									AText	token3;
									GetKeywordToken(p3,endpos,token3,tokenspos3,syntaxPartIndex);
									{
										if( token3.GetItemCount() > 0 )
										{
											if( GetApp().SPI_GetModePrefDB(mModeIndex).IsAlphabet(token3.Get(0),syntaxPartIndex) == true )
											{
												p = p3;
												token.AddText(token2);
												token.AddText(token3);
											}
										}
									}
								}
							}
						}
						//
						ABool	misspell = false;
						//�~�X�X�y���L���b�V���ɂ���΃~�X�X�y���Ɣ��f
						if( mMisspellCacheArray.Find(token) != kIndex_Invalid )
						{
							misspell = true;
						}
						//�������X�y���L���b�V���ɂ���΃~�X�X�y���łȂ��Ɣ��f
						else if( mCorrectspellCacheArray.Find(token) != kIndex_Invalid )
						{
							misspell = false;
						}
						//�L���b�V���Ŕ��f�ł��Ȃ��ꍇ�̂݃X�y���`�F�b�N���{
						else
						{
							//�X�y���`�F�b�N
							misspell = (ASpellCheckerWrapper::CheckSpell(outTextDrawData.UTF16DrawText,
									outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start),
									outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start),
									modePrefDB.GetModeData_Number(AModePrefDB::kSpellCheckLanguage)) == false);
							//�L���b�V��
							if( misspell == true )
							{
								//�L���b�V���ǉ�
								mMisspellCacheArray.Add(token);
								//�L���b�V���T�C�Y�I�[�o�[�Ȃ�ŏ��̕����폜
								if( mMisspellCacheArray.GetItemCount() > kMisspellCacheSize )
								{
									while( mMisspellCacheArray.GetItemCount() > kMisspellCacheSize-kMisspellCacheDeleteSize )
									{
										mMisspellCacheArray.Delete1(0);
									}
								}
							}
							else
							{
								//�L���b�V���ǉ�
								mCorrectspellCacheArray.Add(token);
								//�L���b�V���T�C�Y�I�[�o�[�Ȃ�ŏ��̕����폜
								if( mCorrectspellCacheArray.GetItemCount() > kMisspellCacheSize )
								{
									while( mCorrectspellCacheArray.GetItemCount() > kMisspellCacheSize-kMisspellCacheDeleteSize )
									{
										mCorrectspellCacheArray.Delete1(0);
									}
								}
							}
							//fprintf(stderr,"%d,%d\n",mMisspellCacheArray.GetItemCount(),mCorrectspellCacheArray.GetItemCount());
						}
						//�`��f�[�^
						if( misspell == true )
						{
							outTextDrawData.misspellStartArray.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
							outTextDrawData.misspellEndArray.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
						}
					}
				}
#endif
			}
			//#435 changeStateFromNextChar�̏ꍇ�́A��ԑJ��attr�ǉ�
			if( stateChangePos != kIndex_Invalid )
			{
				AIndex	utf8pos = stateChangePos-lineInfo_Start;
				if( utf8pos >= outTextDrawData.OriginalTextArray_UnicodeOffset.GetItemCount() )
				{
					//��ԕω��ʒu�����̍s�ɂȂ�ꍇ������̂ŁA�l�␳
					utf8pos = outTextDrawData.OriginalTextArray_UnicodeOffset.GetItemCount()-1;
				}
				outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(utf8pos));
				outTextDrawData.attrColor.Add(normalColor);
				outTextDrawData.attrStyle.Add(normalTextStyle);//#844
			}
			//#274
			stateColorValid = nextStateColorValid;
			//pos�X�V
			pos = nextPos;
		}
		//#603
		if( jumpsetupRegExpGroupColorEpos != kIndex_Invalid )
		{
			//���݈ʒupos�����o�����K�\����v�O���[�v�F�Â��I���ʒu�����߂Ē������Ƃ��AattrPos/Color/Style��ǉ�
			outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.
						Get(jumpsetupRegExpGroupColorEpos));
			outTextDrawData.attrColor.Add(normalColor);//�ʏ핶���F�ɖ߂�
			outTextDrawData.attrStyle.Add(normalTextStyle);//#844
			jumpsetupRegExpGroupColorEpos = kIndex_Invalid;
		}
	}
	//�I��\��
	outTextDrawData.drawSelection = false;
	if( inSelectMode == true )
	{
		//�ق���View�ł̃e�L�X�g�ҏW�𔽉f������ꍇ���A�ǂ����Ă��I��͈͂��C���[�K���ɂȂ邱�Ƃ����肤��̂ŁA
		//�C���[�K���̏ꍇ�͑I��͈͕\���͂����Ȃ��悤�ɂ���B
		if( IsTextPointValid(inSelectTextPoint1) == true && IsTextPointValid(inSelectTextPoint2) == true )
		{
			ATextPoint	spt, ept;
			ADocument_Text::OrderTextPoint(inSelectTextPoint1,inSelectTextPoint2,spt,ept);
			if( spt.y == inLineIndex && ept.y == inLineIndex )
			{
				outTextDrawData.drawSelection = true;
				outTextDrawData.selectionStart = outTextDrawData.OriginalTextArray_UnicodeOffset.Get(spt.x);
				outTextDrawData.selectionEnd = outTextDrawData.OriginalTextArray_UnicodeOffset.Get(ept.x);
			}
			else if( spt.y < inLineIndex && ept.y == inLineIndex )
			{
				outTextDrawData.drawSelection = true;
				outTextDrawData.selectionStart = kIndex_Invalid;
				outTextDrawData.selectionEnd = outTextDrawData.OriginalTextArray_UnicodeOffset.Get(ept.x);
			}
			else if( spt.y < inLineIndex && ept.y > inLineIndex )
			{
				outTextDrawData.drawSelection = true;
				outTextDrawData.selectionStart = outTextDrawData.OriginalTextArray_UnicodeOffset.Get(0);
				outTextDrawData.selectionEnd = kIndex_Invalid;
			}
			else if( spt.y == inLineIndex && ept.y > inLineIndex )
			{
				outTextDrawData.drawSelection = true;
				outTextDrawData.selectionStart = outTextDrawData.OriginalTextArray_UnicodeOffset.Get(spt.x);
				outTextDrawData.selectionEnd = kIndex_Invalid;
			}
		}
	}
	//ModuleComment RC3
	/*#138
	if( ioAnalyzeDrawData.flagModuleComment == true )
	{
		ioAnalyzeDrawData.moduleComment.DeleteAll();
		AIndex	identifierLineIndex = kIndex_Invalid;
		AObjectID	objectID = mTextInfo.GetMenuIdentifierByLineIndex(inLineIndex,identifierLineIndex);
		if( objectID != kIndex_Invalid )
		{
			AText	moduleName;
			//#138 mTextInfo.GetGlobalIdentifierKeywordText(objectID,moduleName);
			//#138 if( moduleName.GetItemCount() == 0 )
			//#138 {
			mTextInfo.GetGlobalIdentifierMenuText(objectID,moduleName);
			//#138 }
			SPI_GetExternalComment(moduleName,inLineIndex-identifierLineIndex,ioAnalyzeDrawData.moduleComment);
		}
	}
	*/
}
//
ABool	ADocument_Text::GetKeywordToken( AIndex& ioPos, const AIndex inEndPos, AText& outToken, AIndex& outTokenStartPos,
		const AIndex inSyntaxPartIndex ) const
{
	outToken.DeleteAll();
	
	if( mText.SkipTabSpace(ioPos,inEndPos) == false )   return false;
	
	outTokenStartPos = ioPos;
	
	//�P�ꌟ���ioutTokenStartPos��������ɂ̂݌����A�A���t�@�x�b�g����̓L�[���[�h�p�̔���i�����[�h�ݒ�orSDF���g���j�j
	ATextIndex	start = 0, end = 0;
	ATextInfo::FindWord(mText,mModeIndex,true,outTokenStartPos,start,end,true,inSyntaxPartIndex);
	//FindWord()�͍s�I�����l�������ɒP��I���ʒu���擾����̂ŁAend��inEndPos����Ȃ�AinEndPos���I���ʒu�ɂ���
	if( end > inEndPos )
	{
		end = inEndPos;
	}
	//�I���ʒu��ioPos�Ɋi�[
	ioPos = end;
	//�e�L�X�g�擾
	mText.GetText(outTokenStartPos,end-outTokenStartPos,outToken);
	return true;
	
	/*#R0199 
	AUChar	ch = mText.Get(ioPos);
	ioPos++;
	outToken.Add(ch);
	*/
	/*#�P��I�����W�b�N�̋��ʉ�
	//
	const AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(mModeIndex);
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();//R0199
	
	//=====================�ŏ��̕����̔���=====================
	
	//R0199 �A�X�L�[�O�A���t�@�x�b�g�͖������ŃA���t�@�x�b�g����
	//�ǂݍ���
	AUTF16Char	utf16char;
	AItemCount	bc = mText.Convert1CharToUTF16(ioPos,utf16char);
	AUChar	ch = mText.Get(ioPos);
	//�A���t�@�x�b�g����
	AUnicodeCharCategory	category = unicodeData.GetUnicodeCharCategory(utf16char);
	if( bc == 1 )
	{
		//=====================�ŏ��̕������A�X�L�[���A���t�@�x�b�g�̏ꍇ=====================
		
		//�A�X�L�[���̏ꍇ�F���@��`�̃A���t�@�x�b�g�ȊO�Ȃ炱���i�A���t�@�x�b�g�ȊO�̕����̑O�j�ŏI��
		if( (modePrefDB.IsAlphabet(ch) == false) )
		{
			outToken.Add(ch);//#371
			ioPos++;//#371
			return true;
		}
	}
	else if( category == kUnicodeCharCategory_LatinAndNumber )
	{
		//=====================�ŏ��̕������A�X�L�[�O�A���t�@�x�b�g�̏ꍇ=====================
		
		//�A�X�L�[�O��LatinAndNumber�̏ꍇ�A�m�[�`�F�b�N
	}
	else
	{
		//=====================�ŏ��̕������A�X�L�[�O�m���A���t�@�x�b�g�����̏ꍇ=====================
		
		//�A�X�L�[�O�̏ꍇ�F
		//R0194
		AIndex	spos = ioPos;//R0199 -1;
		AUTF16Char	utf16char;
		ioPos += mText.Convert1CharToUTF16(spos,utf16char);//R0199 -1;
		AUnicodeCharCategory	firstCharCategory = unicodeData.GetUnicodeCharCategory(utf16char);
		if( firstCharCategory != kUnicodeCharCategory_Others )
		{
			AItemCount	uniCharCount = 1;
			for( ; ioPos < inEndPos;  )
			{
				AItemCount	bc = mText.Convert1CharToUTF16(ioPos,utf16char);
				AUnicodeCharCategory	category = unicodeData.GetUnicodeCharCategory(utf16char);
				if( firstCharCategory != category )
				{
					if( (firstCharCategory==kUnicodeCharCategory_ZenkakuHiragana||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) &&
								category == kUnicodeCharCategory_ZenkakuOtohiki )
					{
						//�p��
					}
					else
					{
						break;
					}
				}
				ioPos += bc;
				uniCharCount++;
				if( uniCharCount == 32 )   break;
			}
		}
		if( ioPos > inEndPos )   ioPos = inEndPos;
		mText.GetText(spos,ioPos-spos,outToken);
		return true;
	}
	//Unicode 1�����i�[
	for( AIndex i = 0; i < bc; i++ )
	{
		if( ioPos >= mText.GetItemCount() )   break;//�}���`�t�@�C���������Ńo�C�i���t�@�C����ǂ񂾏ꍇ�Ȃǂ̑΍�
		outToken.Add(mText.Get(ioPos));
		ioPos++;
	}
	
	//B0444 tail����
	if( bc == 1 )
	{
		if( (modePrefDB.IsTailAlphabet(ch) == true) )   return true;
	}
	
	//
	while( ioPos < inEndPos )
	{
		//�ǂݍ���
		bc = mText.Convert1CharToUTF16(ioPos,utf16char);
		ch = mText.Get(ioPos);
		category = unicodeData.GetUnicodeCharCategory(utf16char);
		//�A���t�@�x�b�g����
		if( bc == 1 )
		{
			//�A�X�L�[�������̏ꍇ�F���@��`�̃A���t�@�x�b�g�ȊO�Ȃ炱���i�A���t�@�x�b�g�ȊO�̕����̑O�j�ŏI��
			if( modePrefDB.IsContinuousAlphabet(ch) == false )   return true;
		}
		else if( category == kUnicodeCharCategory_LatinAndNumber )
		{
			//�A�X�L�[�O��LatinAndNumber�̏ꍇ�A�m�[�`�F�b�N
		}
		else
		{
			//��L�ȊO�̃}���`�o�C�g�����F�}���`�o�C�g�����̑O�ŏI��
			return true;
		}
		//Unicode 1�����i�[
		for( AIndex i = 0; i < bc; i++ )
		{
			if( ioPos >= mText.GetItemCount() )   break;//�}���`�t�@�C���������Ńo�C�i���t�@�C����ǂ񂾏ꍇ�Ȃǂ̑΍�
			outToken.Add(mText.Get(ioPos));
			ioPos++;
		}
		//tail����
		if( bc == 1 )
		{
			if( (modePrefDB.IsTailAlphabet(ch) == true) )   return true;
		}
	}
	return true;
	*/
	/*#
	//UTF8�}���`�o�C�g�����̏ꍇ�́AUnicode�P�������ǂ�Ń��^�[��
	if( mText.IsUTF8MultiByte(ch) == true )
	{
		//R0194
		AIndex	spos = ioPos-1;
		AUTF16Char	utf16char;
		ioPos += mText.Convert1CharToUTF16(spos,utf16char) -1;
		const AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
		AUnicodeCharCategory	firstCharCategory = unicodeData.GetUnicodeCharCategory(utf16char);
		if( firstCharCategory != kUnicodeCharCategory_Others )
		{
			AItemCount	uniCharCount = 1;
			for( ; ioPos < inEndPos;  )
			{
				AItemCount	bc = mText.Convert1CharToUTF16(ioPos,utf16char);
				AUnicodeCharCategory	category = unicodeData.GetUnicodeCharCategory(utf16char);
				if( firstCharCategory != category )
				{
					if( (firstCharCategory==kUnicodeCharCategory_ZenkakuHiragana||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) &&
								category == kUnicodeCharCategory_ZenkakuOtohiki )
					{
						//�p��
					}
					else
					{
						break;
					}
				}
				ioPos += bc;
				uniCharCount++;
				if( uniCharCount == 32 )   break;
			}
		}
		if( ioPos > inEndPos )   ioPos = inEndPos;
		mText.GetText(spos,ioPos-spos,outToken);
		//outToken.OutputToStderr();
		//fprintf(stderr, "   ");
		*AItemCount	bc = mText.GetUTF8ByteCount(ch);
		for( AIndex i = 0; i < bc-1; i++ )
		{
			outToken.Add(mText.Get(ioPos));
			ioPos++;
		}*
		return true;
	}
	
	//
	const AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(mModeIndex);
	//
	if( (modePrefDB.IsAlphabet(ch) == false) )   return true;
	
	while( ioPos < inEndPos )
	{
		ch = mText.Get(ioPos);
		if( modePrefDB.IsContinuousAlphabet(ch) == false )   return true;
		
		ioPos++;
		outToken.Add(ch);
		if( (modePrefDB.IsTailAlphabet(ch) == true) )   return true;
	}
	return true;
	*/
}


#pragma mark ===========================

#pragma mark ---�e�L�X�g���擾

//TextPoint���L�����ǂ�����Ԃ�
ABool	ADocument_Text::IsTextPointValid( const ATextPoint& inTextPoint ) const
{
	if( inTextPoint.y < 0 || inTextPoint.y >= SPI_GetLineCount() )   return false;
	if( inTextPoint.x < 0 || inTextPoint.x > SPI_GetLineLengthWithoutLineEnd(inTextPoint.y) )   return false;
	return true;
}

//�e�L�X�g�擾�iinStart>inEnd�ł��j
void	ADocument_Text::GetText( const ATextIndex inStart, const ATextIndex inEnd, AText& outText ) const
{
	ATextIndex	start = inStart;
	ATextIndex	end = inEnd;
	if( inEnd < inStart )
	{
		start = inEnd;
		end = inStart;
	}
	{
		mText.GetText(start,end-start,outText);
	}
}
/**
�e�L�X�g�擾
@note thread-safe
*/
void	ADocument_Text::SPI_GetText( const ATextIndex inStart, const ATextIndex inEnd, AText& outText ) const
{
	AStMutexLocker	locker(mTextMutex);//#598
	GetText(inStart,inEnd,outText);
}
void	ADocument_Text::SPI_GetText( const ATextPoint& inStart, const ATextPoint& inEnd, AText& outText ) const
{
	AStMutexLocker	locker(mTextMutex);//#598
	GetText(SPI_GetTextIndexFromTextPoint(inStart),SPI_GetTextIndexFromTextPoint(inEnd),outText);
}
void	ADocument_Text::SPI_GetText( AText& outText ) const
{
	AStMutexLocker	locker(mTextMutex);//#598
	GetText(0,mText.GetItemCount(),outText);
}

//#450
/**
�w��s�̃e�L�X�g���擾
@note ���s�R�[�h�܂ށB
*/
void	ADocument_Text::SPI_GetLineText( const AIndex inLineIndex, AText& outText ) const
{
	AIndex	spos = SPI_GetLineStart(inLineIndex);
	AIndex	epos = spos + SPI_GetLineLengthWithLineEnd(inLineIndex);
	SPI_GetText(spos,epos,outText);
}

//#750
/**
�w��text index�ȍ~�̍ŏ��̉��s�ʒutext index���擾
*/
AIndex	ADocument_Text::SPI_GetNextLineEndCharIndex( const AIndex inStartTextIndex ) const
{
	AItemCount	len = mText.GetItemCount();
	for( AIndex pos = inStartTextIndex; pos < len; pos++ )
	{
		if( mText.Get(pos) == kUChar_LineEnd )
		{
			return pos;
		}
	}
	return len;
}

/**
�O��unicode�����̊J�n�ʒu��text index���擾
@param inSkipCanonicalDecomp decomp�������X�L�b�v���邩�ǂ����i�f�t�H���g�G�X�L�b�v����j
*/
AIndex	ADocument_Text::SPI_GetPrevCharTextIndex( const ATextIndex inIndex, const ABool inSkipCanonicalDecomp,
												 const ABool inReturnMinusWhen0 ) const
{
	if( inSkipCanonicalDecomp == true )
	{
		//#863
		//Unicode��`�ɏ]���Adecomp�����̓X�L�b�v���āA�O�̕�����text index���擾
		return GetApp().NVI_GetUnicodeData().GetPrevCharPosSkipCanonicalDecomp(mText,inIndex,inReturnMinusWhen0);
	}
	else
	{
		return mText.GetPrevCharPos(inIndex,inReturnMinusWhen0);
	}
}

/**
����unicode�����̊J�n�ʒu��text index���擾
@param inSkipCanonicalDecomp decomp�������X�L�b�v���邩�ǂ����i�f�t�H���g�G�X�L�b�v����j
*/
AIndex	ADocument_Text::SPI_GetNextCharTextIndex( const ATextIndex inIndex, const ABool inSkipCanonicalDecomp ) const
{
	if( inSkipCanonicalDecomp == true )
	{
		//#863
		//Unicode��`�ɏ]���Adecomp�����̓X�L�b�v���āA���̕�����text index���擾
		return GetApp().NVI_GetUnicodeData().GetNextCharPosSkipCanonicalDecomp(mText,inIndex);
	}
	else
	{
		return mText.GetNextCharPos(inIndex);
	}
}

//
AIndex	ADocument_Text::SPI_GetCurrentCharTextIndex( const ATextIndex inIndex ) const
{
	return mText.GetCurrentCharPos(inIndex);
}

//
ATextIndex	ADocument_Text::SPI_GetParagraphStartTextIndexFromLineIndex( const AIndex inLineIndex ) const
{
	return SPI_GetLineStart(mTextInfo.GetCurrentParagraphStartLineIndex(inLineIndex));
}

//
AIndex	ADocument_Text::SPI_GetCurrentParagraphStartLineIndex( const AIndex inLineIndex ) const
{
	return mTextInfo.GetCurrentParagraphStartLineIndex(inLineIndex);
}

//
AIndex	ADocument_Text::SPI_GetNextParagraphStartLineIndex( const AIndex inLineIndex ) const
{
	return mTextInfo.GetNextParagraphStartLineIndex(inLineIndex);
}

ATextIndex	ADocument_Text::SPI_GetParagraphStartTextIndex( const AIndex inParagraphIndex ) const
{
	return mTextInfo.GetParagraphStartTextIndex(inParagraphIndex);
}

//�sIndex����i��Index���擾
AIndex	ADocument_Text::SPI_GetParagraphIndexByLineIndex( const AIndex inLineIndex ) const
{
	return mTextInfo.GetParagraphIndexByLineIndex(inLineIndex);
}

//�i��Index����sIndex���擾
AIndex	ADocument_Text::SPI_GetLineIndexByParagraphIndex( const AIndex inParagraphIndex ) const
{
	return mTextInfo.GetLineIndexByParagraphIndex(inParagraphIndex);
}

//#465
/**
*/
void	ADocument_Text::SPI_GetParagraphTextByLineIndex( const AIndex inLineIndex, AText& outText ) const
{
	AIndex	startLineIndex = SPI_GetCurrentParagraphStartLineIndex(inLineIndex);
	AIndex	endLineIndex = startLineIndex;
	for( ; endLineIndex < SPI_GetLineCount(); endLineIndex++ )
	{
		if( SPI_GetLineExistLineEnd(endLineIndex) == true )
		{
			endLineIndex++;
			break;
		}
	}
	ATextPoint	spt = {0};
	spt.x = 0;
	spt.y = startLineIndex;
	ATextPoint	ept = {0};
	ept.x = SPI_GetLineLengthWithLineEnd(endLineIndex-1);//#0 �ŏI�s�̏ꍇ���l�����A(0,endLineIndex+1)�Ƃ͂����ɁA���L�̂悤�ɂ��Ă���
	ept.y = endLineIndex-1;
	SPI_GetText(spt,ept,outText);
}

//#379
/**
�w��p���O���t�̃e�L�X�g���擾
*/
void	ADocument_Text::SPI_GetParagraphText( const AIndex inParagraphIndex, AText& outText ) const
{
	AIndex	startLineIndex = SPI_GetLineIndexByParagraphIndex(inParagraphIndex);
	AIndex	endLineIndex = startLineIndex;
	for( ; endLineIndex < SPI_GetLineCount(); endLineIndex++ )
	{
		if( SPI_GetLineExistLineEnd(endLineIndex) == true )
		{
			endLineIndex++;
			break;
		}
	}
	ATextPoint	spt = {0};
	spt.x = 0;
	spt.y = startLineIndex;
	ATextPoint	ept = {0};
	ept.x = SPI_GetLineLengthWithLineEnd(endLineIndex-1);//#0 �ŏI�s�̏ꍇ���l�����A(0,endLineIndex+1)�Ƃ͂����ɁA���L�̂悤�ɂ��Ă���
	ept.y = endLineIndex-1;
	SPI_GetText(spt,ept,outText);
}

//#996
/**
�i���̒����擾�i���s�R�[�h�܂܂��j
*/
AItemCount	ADocument_Text::SPI_GetParagraphLengthWithoutLineEnd( const AIndex inParagraphIndex ) const
{
	AIndex	startLineIndex = SPI_GetLineIndexByParagraphIndex(inParagraphIndex);
	AIndex	endLineIndex = startLineIndex;
	for( ; endLineIndex < SPI_GetLineCount(); endLineIndex++ )
	{
		if( SPI_GetLineExistLineEnd(endLineIndex) == true )
		{
			endLineIndex++;
			break;
		}
	}
	ATextPoint	spt = {0};
	spt.x = 0;
	spt.y = startLineIndex;
	ATextPoint	ept = {0};
	ept.x = SPI_GetLineLengthWithLineEnd(endLineIndex-1);//#0 �ŏI�s�̏ꍇ���l�����A(0,endLineIndex+1)�Ƃ͂����ɁA���L�̂悤�ɂ��Ă���
	ept.y = endLineIndex-1;
	return SPI_GetTextIndexFromTextPoint(ept) - SPI_GetTextIndexFromTextPoint(spt);
}

//inTextIndex����AUTF8����inCharOffset�������i�񂾈ʒu��TextIndex��Ԃ�
ATextIndex	ADocument_Text::SPI_GetTextIndexByUTF8CharOffset( const ATextIndex inTextIndex, const AItemCount inCharOffset ) const
{
	ATextIndex pos = inTextIndex;
	AItemCount	charOffset = inCharOffset;
	while( charOffset > 0 && pos < mText.GetItemCount() )
	{
		pos = mText.GetNextCharPos(pos);
		charOffset--;
	}
	return pos;
}

//�i�����擾
AItemCount	ADocument_Text::SPI_GetParagraphCount() const
{
	return SPI_GetParagraphIndexByLineIndex(SPI_GetLineCount()-1)+1;
}

/**
�i���J�n�s���ǂ������擾
*/
ABool	ADocument_Text::SPI_IsParagraphStartLine( const AIndex inLineIndex ) const
{
	ABool	isParagraphStart = true;
	if( inLineIndex > 0 )
	{
		isParagraphStart = mTextInfo.GetLineExistLineEnd(inLineIndex-1);
	}
	return isParagraphStart;
}

ABool	ADocument_Text::SPI_IsUTF8Multibyte( const ATextIndex inIndex ) const
{
	return mText.IsUTF8MultiByte(inIndex);
}

AUChar	ADocument_Text::SPI_GetTextChar( const ATextIndex inIndex ) const
{
	return mText.Get(inIndex);
}

void	ADocument_Text::SPI_GetText1CharText( const ATextIndex inIndex, AText& outText ) const
{
	ATextIndex	epos = mText.GetNextCharPos(inIndex);
	mText.GetText(inIndex,epos-inIndex,outText);
}

//#142
/**
�w��text index��unicode�������擾
*/
AUCS4Char	ADocument_Text::SPI_GetText1UCS4Char( const ATextIndex inIndex ) const
{
	AUCS4Char	ch = 0;
	mText.Convert1CharToUCS4(inIndex,ch);
	return ch;
}

//#142
/**
�w��text index��unicode�������擾�i����decomp�����������ꍇ�́A���̕������擾�j
*/
void	ADocument_Text::SPI_GetText1UCS4Char( const ATextIndex inIndex, AUCS4Char& outChar, AUCS4Char& outCanonicalDecomp2ndChar ) const
{
	//���ʏ�����
	outChar = 0;
	outCanonicalDecomp2ndChar = 0;
	//unicode�����擾
	AItemCount	bc = mText.Convert1CharToUCS4(inIndex,outChar);
	//���̕������e�L�X�g�����̔���
	if( inIndex+bc < mText.GetItemCount() )
	{
		//���̕������擾
		AUCS4Char	nextchar = 0;
		mText.Convert1CharToUCS4(inIndex+bc,nextchar);
		//decomp�����Ȃ�outCanonicalDecomp2ndChar�Ɋi�[
		if( GetApp().NVI_GetUnicodeData().IsCanonicalDecomp2ndChar(nextchar) == true )
		{
			outCanonicalDecomp2ndChar = nextchar;
		}
	}
}

//B0424
/**
�P�ꌟ���i���[�h�ݒ�ɂ���ăL�[���[�h�P��ɂ��邩�ǂ���������j
@note �P�ꂪ�A���t�@�x�b�g���}���`�o�C�g�Ȃ�true��Ԃ�
*/
ABool	ADocument_Text::SPI_FindWord( const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd ) const
{
	//�w��text index�̕��@�p�[�g�̎擾
	ATextPoint	spt = {0};
	SPI_GetTextPointFromTextIndex(inStartPos,spt);
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSyntaxDefinition();
	AIndex	stateIndex = SPI_GetCurrentStateIndex(spt);//SPI_GetStateIndexByLineIndex(spt.y);
	AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
	//�P�ꌟ���i�O�����܂ށB�A���t�@�x�b�g���肪���[�h�ݒ�orSDF�ɏ]�����Aunicode�̔���݂̂ɏ]�����A�̓��[�h�ݒ�ɏ]���j
	return ATextInfo::FindWord(mText,mModeIndex,false,inStartPos,outStart,outEnd,
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kApplyCharsForWordToDoubleClick),
		syntaxPartIndex);
}

//B0424
/**
�P�ꌟ���i�L�[���[�h�P��j
@note �P�ꂪ�A���t�@�x�b�g���}���`�o�C�g�Ȃ�true��Ԃ�
*/
ABool	ADocument_Text::SPI_FindWordForKeyword( const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd ) const
{
	//�w��text index�̕��@�p�[�g�̎擾
	ATextPoint	spt = {0};
	SPI_GetTextPointFromTextIndex(inStartPos,spt);
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSyntaxDefinition();
	AIndex	stateIndex = SPI_GetCurrentStateIndex(spt);//SPI_GetStateIndexByLineIndex(spt.y);
	AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
	//�P�ꌟ���i������̂݌����B�A���t�@�x�b�g����̓��[�h�ݒ�orSDF�ɏ]���B�j
	return ATextInfo::FindWord(mText,mModeIndex,false,inStartPos,outStart,outEnd,true,syntaxPartIndex);
}

void	ADocument_Text::SPI_FindQuadWord( const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd ) const
{
	AText	text;
	GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Text(AModePrefDB::kQuadClickText,text);
	for( outStart = mText.GetPrevCharPos(inStartPos); outStart > 0; outStart = mText.GetPrevCharPos(outStart) )
	{
		ABool	found = false;
		for( AIndex i = 0; i < text.GetItemCount();  )
		{
			AText	t;
			text.Get1UTF8Char(i,t);
			if( outStart+t.GetItemCount() > mText.GetItemCount() )   continue;
			if( t.Compare(mText,outStart,t.GetItemCount()) == true )
			{
				found = true;
				break;
			}
		}
		if( found == true )
		{
			outStart = mText.GetNextCharPos(outStart);
			break;
		}
	}
	if( outStart < 0 )   outStart = 0;
	for( outEnd = inStartPos; outEnd < mText.GetItemCount(); outEnd = mText.GetNextCharPos(outEnd) )
	{
		ABool	found = false;
		for( AIndex i = 0; i < text.GetItemCount();  )
		{
			AText	t;
			text.Get1UTF8Char(i,t);
			if( outEnd+t.GetItemCount() > mText.GetItemCount() )   continue;
			if( t.Compare(mText,outEnd,t.GetItemCount()) == true )
			{
				found = true;
				break;
			}
		}
		if( found == true )   break;
	}
}

ABool	ADocument_Text::SPI_IsAsciiAlphabet( const ATextIndex inPos ) const
{
	AUChar ch = mText.Get(inPos);
	return Macro_IsAlphabet(ch);
	/*#416 �_�u���N���b�N���삪�ς���Ă��܂��̂ŁA�Ƃ肠�����A���L�͕ۗ�
	AIndex	curPos = mText.GetCurrentCharPos(inPos);//inPos��UTF-8�̓r���ŃR�[�����Ă���ӏ�������̂ňʒu�␳
	AUCS4Char	ch = 0;
	mText.Convert1CharToUCS4(curPos,ch);
	return GetApp().NVI_GetUnicodeData().IsAlphabetOrNumber(ch);
	*/
}

AItemCount	ADocument_Text::SPI_GetLineCount() const
{
	return mTextInfo.GetLineCount();
}

AIndex	ADocument_Text::SPI_GetLineLengthWithoutLineEnd( const AIndex inLineIndex ) const
{
	return mTextInfo.GetLineLengthWithoutLineEnd(inLineIndex);
}

AIndex	ADocument_Text::SPI_GetLineLengthWithLineEnd( const AIndex inLineIndex ) const
{
	return mTextInfo.GetLineLengthWithLineEnd(inLineIndex);
}

ABool	ADocument_Text::SPI_GetLineExistLineEnd( const AIndex inLineIndex ) const
{
	return mTextInfo.GetLineExistLineEnd(inLineIndex);
}

ATextIndex	ADocument_Text::SPI_GetLineStart( const AIndex inLineIndex ) const
{
	if( inLineIndex < SPI_GetLineCount() )
	{
		return mTextInfo.GetLineStart(inLineIndex);
	}
	else
	{
		return SPI_GetTextLength();
	}
}

void	ADocument_Text::SPI_GetGlobalIdentifierRange( const AUniqID inIdentifierUniqID, ATextPoint& outStart, ATextPoint& outEnd ) const
{
	mTextInfo.GetGlobalIdentifierRange(inIdentifierUniqID,outStart,outEnd);
}

//
void	ADocument_Text::SPI_UpdateCurrentIdentifierDefinitionList( const AText& inText )
{
	mIdentifierDefinitionList_InfoText.DeleteAll();
	mIdentifierDefinitionList_File.DeleteAll();
	mIdentifierDefinitionList_StartTextIndex.DeleteAll();
	mIdentifierDefinitionList_Length.DeleteAll();
	//global
	AArray<AUniqID>	UniqIDArray;
	mTextInfo.GetGlobalIdentifierListByKeyword(inText,UniqIDArray);
	AFileAcc	file;
	NVI_GetFile(file);
	for( AIndex i = 0; i < UniqIDArray.GetItemCount(); i++ )
	{
		AUniqID	UniqID = UniqIDArray.Get(i);
		AText	categoryName;
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(mTextInfo.GetGlobalIdentifierCategoryIndex(UniqID),categoryName);
		AText	infotext;
		infotext.SetCstring("[");
		infotext.AddText(categoryName);
		infotext.AddCstring("] ");
		AText	info;
		mTextInfo.GetGlobalIdentifierInfoText(UniqID,info);
		if( info.GetItemCount() > 0 )
		{
			infotext.AddText(info);
		}
		else//R0241
		{
			infotext.AddText(inText);
		}
		ATextPoint	spt, ept;
		mTextInfo.GetGlobalIdentifierRange(UniqID,spt,ept);
		ATextIndex	spos = SPI_GetTextIndexFromTextPoint(spt);
		ATextIndex	epos = SPI_GetTextIndexFromTextPoint(ept);
		mIdentifierDefinitionList_InfoText.Add(infotext);
		mIdentifierDefinitionList_File.GetObject(mIdentifierDefinitionList_File.AddNewObject()).CopyFrom(file);
		mIdentifierDefinitionList_StartTextIndex.Add(spos);
		mIdentifierDefinitionList_Length.Add(epos-spos);
	}
	//import
	AArray<AIndex>	categoryArray;
	ATextArray	infotextArray, commentArray, parentArray;
	AObjectArray<AFileAcc>	fileArray;
	AArray<ATextIndex>	sposArray, eposArray;
	mImportIdentifierLinkList.GetIdentifierListByKeyword(
				inText,categoryArray,infotextArray,fileArray,sposArray,eposArray,commentArray,parentArray);
	GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSystemHeaderIdentifierLinkList().GetIdentifierListByKeyword(
			inText,categoryArray,infotextArray,fileArray,sposArray,eposArray,commentArray,parentArray);//#253
	for( AIndex i = 0; i < categoryArray.GetItemCount(); i++ )
	{
		if( categoryArray.Get(i) == kIndex_Invalid )   continue;//�v�����H
		if( fileArray.GetObjectConst(i).Compare(file) == true )   continue;//win 080722 ���t�@�C���̎��ʎq��global�Œǉ��ς݂̂͂��Ȃ̂Ńp�X
		AText	categoryName;
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryArray.Get(i),categoryName);
		AText	infotext;
		AText	filename;
		fileArray.GetObjectConst(i).GetFilename(filename);
		infotext.SetCstring("(");
		infotext.AddText(filename);
		infotext.AddCstring(") ");
		infotext.AddCstring("[");
		infotext.AddText(categoryName);
		infotext.AddCstring("] ");
		AText	info;
		infotextArray.Get(i,info);
		if( info.GetItemCount() > 0 )
		{
			infotext.AddText(info);
		}
		else//R0241
		{
			infotext.AddText(inText);
		}
		ATextIndex	spos = sposArray.Get(i);
		ATextIndex	epos = eposArray.Get(i);
		mIdentifierDefinitionList_InfoText.Add(infotext);
		mIdentifierDefinitionList_File.GetObject(mIdentifierDefinitionList_File.AddNewObject()).CopyFrom(fileArray.GetObjectConst(i));
		mIdentifierDefinitionList_StartTextIndex.Add(spos);
		mIdentifierDefinitionList_Length.Add(epos-spos);
	}
	/*#893
	//local
	mTextInfo.GetLocalIdentifierListByKeyword(inText,UniqIDArray);
	for( AIndex i = 0; i < UniqIDArray.GetItemCount(); i++ )
	{
		AUniqID	UniqID = UniqIDArray.Get(i);
		AText	categoryName;
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(mTextInfo.GetLocalIdentifierCategoryIndex(UniqID),categoryName);
		AText	infotext;
		infotext.SetCstring("(local) ");
		AText	info;
		mTextInfo.GetLocalIdentifierInfoText(UniqID,info);
		if( info.GetItemCount() > 0 )
		{
			infotext.AddText(info);
		}
		else//R0241
		{
			infotext.AddText(inText);
		}
		ATextPoint	spt, ept;
		mTextInfo.GetLocalIdentifierRange(UniqID,spt,ept);
		if( spt.y >= mCurrentLocalIdentifierStartLineIndex && ept.y < mCurrentLocalIdentifierEndLineIndex )
		{
			ATextIndex	spos = SPI_GetTextIndexFromTextPoint(spt);
			ATextIndex	epos = SPI_GetTextIndexFromTextPoint(ept);
			mIdentifierDefinitionList_InfoText.Add(infotext);
			mIdentifierDefinitionList_File.GetObject(mIdentifierDefinitionList_File.AddNewObject()).CopyFrom(file);
			mIdentifierDefinitionList_StartTextIndex.Add(spos);
			mIdentifierDefinitionList_Length.Add(epos-spos);
		}
	}
	*/
	
	//���݂̃��j���[���ڂ��폜
	GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_MoveToDefinition);
	//��`�ꏊ���j���[�X�V
	for( AIndex i = 0; i < mIdentifierDefinitionList_InfoText.GetItemCount(); i++ )
	{
		AText	menutext;
		mIdentifierDefinitionList_InfoText.Get(i,menutext);
		AText	actiontext;
		actiontext.SetFormattedCstring("%d",i);
		GetApp().NVI_GetMenuManager().AddDynamicMenuItemByMenuItemID(kMenuItemID_MoveToDefinition,menutext,actiontext,kObjectID_Invalid,NULL,0,false);
	}
}

//
void	ADocument_Text::SPI_GetCurrentIdentifierDefinitionRange( const AIndex inIndex, ATextIndex& outStart, ATextIndex& outEnd,
		ABool& outImport, AFileAcc& outFile ) const
{
	outFile.CopyFrom(mIdentifierDefinitionList_File.GetObjectConst(inIndex));
	AFileAcc	file;
	NVI_GetFile(file);
	outImport = ( file.Compare(outFile) == false );
	outStart = mIdentifierDefinitionList_StartTextIndex.Get(inIndex);
	outEnd = outStart + mIdentifierDefinitionList_Length.Get(inIndex);
}

AItemCount	ADocument_Text::SPI_GetIdentifierDefinitionListCount() const
{
	return mIdentifierDefinitionList_File.GetItemCount();
}

#if 0
//#717 �폜

//inText�Ŏn�܂镶����Ɋւ��ďȗ����͌�⃊�X�g���擾����
//RC2 �S�̓I�ɏC��
void	ADocument_Text::SPI_GetAbbrevCandidate( const AText& inText, ATextArray& outAbbrevCandidateArray,
		ATextArray& outInfoTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
		AObjectArray<AFileAcc>& outFileArray ) const
{
	//�o�͔z���S����
	outAbbrevCandidateArray.DeleteAll();
	outInfoTextArray.DeleteAll();
	outCategoryIndexArray.DeleteAll();
	outScopeArray.DeleteAll();
	outFileArray.DeleteAll();
	//
	AArray<AUniqID>	identifierIDArray;
	//���[�J��
	AItemCount	itemCount = mCurrentLocalIdentifier.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		const AText&	keywordText = mCurrentLocalIdentifier.GetObjectConst(i).GetKeywordText();
		if( keywordText.GetItemCount() >= inText.GetItemCount() )
		{
			if( keywordText.CompareMin(inText) == true )
			{
				if( outAbbrevCandidateArray.Find(keywordText) == kIndex_Invalid )//�d������L�[���[�h�͕\�����Ȃ�
				{
					outAbbrevCandidateArray.Add(keywordText);
					AText	infotext;
					infotext.SetText(mCurrentLocalIdentifier.GetObjectConst(i).GetInfoText());
					outInfoTextArray.Add(infotext);
					outCategoryIndexArray.Add(mCurrentLocalIdentifier.GetObjectConst(i).GetCategoryIndex());
					outScopeArray.Add(kScopeType_Local);
				}
			}
		}
	}
	//
	//�O���[�o��
	mTextInfo.GetAbbrevCandidateGlobal(inText,outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray);
	//outFileArray�𖄂߂�
	AFileAcc	file;
	NVI_GetFile(file);
	for( AIndex i = 0; i < outAbbrevCandidateArray.GetItemCount(); i++ )
	{
		outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(file);
	}
	//�C���|�[�g
	mImportIdentifierLinkList.GetAbbrevCandidate(inText,file,
			outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray,outFileArray);
	//�V�X�e���w�b�_�t�@�C�� #253
	GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSystemHeaderIdentifierLinkList().
			GetAbbrevCandidate(inText,file,
			outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray,outFileArray);
	
	//���[�h�̃L�[���[�h
	GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetAbbrevCandidate(inText,
			outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray);
	//outFileArray�𖄂߂�
	for( AIndex i = outFileArray.GetItemCount(); i < outAbbrevCandidateArray.GetItemCount(); i++ )
	{
		outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(file);
	}
	//mImportIdentifierLinkList.GetAbbrevCandidate(inText,outAbbrevCandidateArray);//win 080714
	//GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetAbbrevCandidate(inText,outAbbrevCandidateArray,identifierIDArray);
	//GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_GetInfoWindow().SPI_SetAbbrevCandidate(outAbbrevCandidateArray);//���[�g����
	/*AText	text;
	if( identifierIDArray.GetItemCount() > 0 )
	{
		AUniqID	identifierID = identifierIDArray.Get(0);
		AIndex	lineIndex = mTextInfo.GetLineIndexByIdentifier(identifierID);
		ATextPoint	spt, ept;
		spt.x = 0;
		spt.y = lineIndex;
		ept.x = 0;
		ept.y = lineIndex+20;
		if( ept.y > SPI_GetLineCount() )   ept.y = SPI_GetLineCount();
		SPI_GetText(spt,ept,text);
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_GetInfoWindow().SPI_SetInfoText(text);
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_Test(lineIndex);
	}*/
}

//R0243
//ParentKeyword�ȗ����͌�⃊�X�g���擾����
void	ADocument_Text::SPI_GetAbbrevCandidateByParentKeyword( const AText& inParentKeyword, ATextArray& outAbbrevCandidateArray,
		ATextArray& outInfoTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
		AObjectArray<AFileAcc>& outFileArray ) const
{
	//�o�͔z���S����
	outAbbrevCandidateArray.DeleteAll();
	outInfoTextArray.DeleteAll();
	outCategoryIndexArray.DeleteAll();
	outScopeArray.DeleteAll();
	outFileArray.DeleteAll();
	//���[�J��
	//���[�J���͑Ή����Ȃ��B�iParentKeyword�i�N���X�j��`�̓O���[�o�����C���|�[�g�j
	//�O���[�o��
	mTextInfo.GetAbbrevCandidateGlobalByParentKeyword(inParentKeyword,outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray);
	//outFileArray�𖄂߂�
	AFileAcc	file;
	NVI_GetFile(file);
	for( AIndex i = 0; i < outAbbrevCandidateArray.GetItemCount(); i++ )
	{
		outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(file);
	}
	//�C���|�[�g
	mImportIdentifierLinkList.GetAbbrevCandidateByParentKeyword(inParentKeyword,
			outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray,outFileArray);
	//�V�X�e���w�b�_�t�@�C�� #253
	GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSystemHeaderIdentifierLinkList().
			GetAbbrevCandidateByParentKeyword(inParentKeyword,
			outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray,outFileArray);
}
#endif

//R0243
//
ABool	ADocument_Text::SPI_GetTypeOfKeyword( const AText& inText, AText& outType, const AViewID inViewID ) const
{
	outType.DeleteAll();
	//#893
	//�w��view ID����view index���擾
	AIndex	viewIndex = kIndex_Invalid;
	if( inViewID != kObjectID_Invalid )
	{
		viewIndex = mViewIDArray.Find(inViewID);
	}
	//���[�J��
	if( viewIndex != kIndex_Invalid )
	{
		//�w��view�̃��[�J���͈͂̎��ʎq������
		AObjectID	objectID = mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).FindObjectID(inText,0,inText.GetItemCount());
		if( objectID != kObjectID_Invalid )
		{
			outType.SetText(mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).GetObjectConstByID(objectID).GetTypeText());
			if( outType.GetItemCount() > 0 )
			{
				return true;
			}
		}
	}
	//�O���[�o��
	if( mTextInfo.GetGlobalIdentifierTypeTextByKeywordText(inText,outType) == true )
	{
		if( outType.GetItemCount() > 0 )
		{
			return true;
		}
	}
	//�C���|�[�g
	if( mImportIdentifierLinkList.FindKeywordType(inText,outType) == true )
	{
		if( outType.GetItemCount() > 0 )
		{
			return true;
		}
	}
	//�V�X�e���w�b�_�t�@�C�����猟�� #253
	if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSystemHeaderIdentifierLinkList().FindKeywordType(inText,outType) == true )
	{
		if( outType.GetItemCount() > 0 )
		{
			return true;
		}
	}
	//�L�[���[�h���X�g���猟��
	if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).FindKeywordTypeFromKeywordList(inText,outType) == true )
	{
		if( outType.GetItemCount() > 0 )
		{
			return true;
		}
	}
	//��v�Ȃ�
	return false;
}

/*
//�_�u���N���b�N�����ɃL�[���[�h�̏���\������@�\�p
void	ADocument_Text::SPI_GetInfoText( const AText& inText, AText& outInfoText ) const
{
	outInfoText.DeleteAll();
	//#147 �L�[���[�h������\���i��ԗD��j
	{
		ATextArray	explanationArray;
		AArray<AIndex>	categoryIndexArray;
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).FindKeywordExplanation(inText,explanationArray,categoryIndexArray);
		//
		if( explanationArray.GetItemCount() > 0 )
		{
			AText	categoryName;
			GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndexArray.Get(0),categoryName);
			outInfoText.AddCstring("[");
			outInfoText.AddText(categoryName);
			outInfoText.AddCstring("] ");
			outInfoText.AddText(explanationArray.GetTextConst(0));
			return;
		}
	}
	//
	AUniqID identifierID = mTextInfo.GetLocalIdentifierByKeywordText(inText);
	if( identifierID != kUniqID_Invalid )
	{
		AText	infotext;
		mTextInfo.GetLocalIdentifierInfoText(identifierID,infotext);
		if( infotext.GetItemCount() == 0 )//R0241
		{
			infotext.SetText(inText);
		}
		AIndex	categoryIndex = mTextInfo.GetLocalIdentifierCategoryIndex(identifierID);
		if( categoryIndex != kIndex_Invalid )
		{
			outInfoText.SetCstring("(Local) ");
			AText	categoryName;
			GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndex,categoryName);
			outInfoText.AddCstring("[");
			outInfoText.AddText(categoryName);
			outInfoText.AddCstring("] ");
			outInfoText.AddText(infotext);
		}
	}
	else
	{
		AUniqID identifierID = mTextInfo.GetGlobalIdentifierByKeywordText(inText);
		if( identifierID != kUniqID_Invalid )
		{
			AText	infotext;
			mTextInfo.GetGlobalIdentifierInfoText(identifierID,infotext);
			if( infotext.GetItemCount() == 0 )//R0241
			{
				infotext.SetText(inText);
			}
			AIndex	categoryIndex = mTextInfo.GetGlobalIdentifierCategoryIndex(identifierID);
			if( categoryIndex != kIndex_Invalid )
			{
				AText	categoryName;
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndex,categoryName);
				outInfoText.SetCstring("[");
				outInfoText.AddText(categoryName);
				outInfoText.AddCstring("] ");
				outInfoText.AddText(infotext);
			}
		}
		else
		{
			AFileAcc	file;
			AIndex	categoryIndex;
			AText	infotext;
			if( mImportIdentifierLinkList.FindKeyword(inText,file,categoryIndex,infotext) == true )
			{
				if( infotext.GetItemCount() == 0 )//R0241
				{
					infotext.SetText(inText);
				}
				AText	filename;
				file.GetFilename(filename);
				outInfoText.SetCstring("(");
				outInfoText.AddText(filename);
				outInfoText.AddCstring(") ");
				if( categoryIndex != kIndex_Invalid )
				{
					AText	categoryName;
					GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndex,categoryName);
					outInfoText.AddCstring("[");
					outInfoText.AddText(categoryName);
					outInfoText.AddCstring("] ");
					outInfoText.AddText(infotext);
				}
			}
			//#253 �V�X�e���w�b�_�t�@�C��
			else if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSystemHeaderIdentifierLinkList().
						FindKeyword(inText,file,categoryIndex,infotext) == true )
			{
				if( infotext.GetItemCount() == 0 )//R0241
				{
					infotext.SetText(inText);
				}
				AText	filename;
				file.GetFilename(filename);
				outInfoText.SetCstring("(");
				outInfoText.AddText(filename);
				outInfoText.AddCstring(") ");
				if( categoryIndex != kIndex_Invalid )
				{
					AText	categoryName;
					GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndex,categoryName);
					outInfoText.AddCstring("[");
					outInfoText.AddText(categoryName);
					outInfoText.AddCstring("] ");
					outInfoText.AddText(infotext);
				}
			}
		}
	}
}
*/

//RC2
//���ʎq���擾
void	ADocument_Text::SPI_GetIdInfoArray( const AText& inText, ATextArray& outInfoTextArray, AObjectArray<AFileAcc>& outFileArray,
		ATextArray& outCommentTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
		AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
		ATextArray& outParentKeywordArray ) const
{
	AFileAcc	docfile;
	NVI_GetFile(docfile);
	//�o�͔z��S�폜
	outInfoTextArray.DeleteAll();
	outFileArray.DeleteAll();
	outCommentTextArray.DeleteAll();
	outCategoryIndexArray.DeleteAll();
	outScopeArray.DeleteAll();
	outStartIndexArray.DeleteAll();
	outEndIndexArray.DeleteAll();
	outParentKeywordArray.DeleteAll();
	//���[�J��
	/*���[�J�����ʎq�͕\�����Ȃ����Ƃɂ���
	mTextInfo�Ɋi�[����Ă��郍�[�J�����ʎq�́A�t�@�C���S�̂̃��[�J�����ʎq�ł���B
	����āA���L�̕��@�Ŏ��ʎq���擾����ƁA���݂̃��[�J���͈͂𖳎����āA�S�Ă̎��ʎq�̏�񂪎擾����Ă��܂�
	����AmCurrentLocalIdentifier�ɂ́A�L�[���[�h�ƃJ�e�S���[�̏�񂵂��܂܂�Ă��Ȃ��B
	���������A���[�J�����ʎq�̎��ʎq�����擾����K�v���́A���݂̎d�l�ł͏��Ȃ��Ǝv����B
	{
		AArray<AUniqID>	UniqIDArray;
		mTextInfo.GetLocalIdentifierListByKeywordText(inText,UniqIDArray);
		for( AIndex i = 0; i < UniqIDArray.GetItemCount(); i++ )
		{
			AUniqID	identifierID = UniqIDArray.Get(i);
			//
			AText	infotext;
			mTextInfo.GetLocalIdentifierInfoText(identifierID,infotext);
			AIndex	categoryIndex = mTextInfo.GetLocalIdentifierCategoryIndex(identifierID);
			if( categoryIndex != kIndex_Invalid )
			{
				AText	categoryName;
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndex,categoryName);
				//
				outInfoTextArray.Add(infotext);
				outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(docfile);
				outCommentTextArray.Add(GetEmptyText());
				outCategoryIndexArray.Add(categoryIndex);
				outScopeArray.Add(kScopeType_Local);
				ATextPoint	spt, ept;
				mTextInfo.GetLocalIdentifierRange(identifierID,spt,ept);
				ATextIndex	spos = SPI_GetTextIndexFromTextPoint(spt);
				ATextIndex	epos = SPI_GetTextIndexFromTextPoint(ept);
				outStartIndexArray.Add(spos);
				outEndIndexArray.Add(epos);
				outParentKeywordArray.Add(GetEmptyText());
			}
		}
	}*/
	//#147 �L�[���[�h����
	{
		ATextArray	explanationArray;
		AArray<AIndex>	categoryIndexArray;
		ATextArray	parentKeywordArray;
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).FindKeywordIdInfo(inText,explanationArray,categoryIndexArray,parentKeywordArray);
		//
		for( AIndex i = 0; i < explanationArray.GetItemCount(); i++ )
		{
			//�e���ݒ�
			outInfoTextArray.Add(explanationArray.GetTextConst(i));
			outFileArray.AddNewObject();
			outCommentTextArray.Add(GetEmptyText());
			outCategoryIndexArray.Add(categoryIndexArray.Get(i));
			outScopeArray.Add(kScopeType_ModeKeyword);
			//���ʎq�L�[���[�h�͈̔͂��擾���Đݒ�
			outStartIndexArray.Add(0);
			outEndIndexArray.Add(0);
			outParentKeywordArray.Add(parentKeywordArray.GetTextConst(i));
		}
	}
	//�O���[�o��
	{
		//�L�[���[�h�ɑΉ�����O���[�o�����ʎq��S�Ď擾
		AArray<AUniqID>	UniqIDArray;
		mTextInfo.GetGlobalIdentifierListByKeywordText(inText,UniqIDArray);
		//�e���ʎq���ɏ���
		for( AIndex i = 0; i < UniqIDArray.GetItemCount(); i++ )
		{
			//���ʎqUniqID�擾
			AUniqID	identifierID = UniqIDArray.Get(i);
			//�e���擾
			AText	infotext;
			mTextInfo.GetGlobalIdentifierInfoText(identifierID,infotext);
			AText	comment;
			mTextInfo.GetGlobalIdentifierCommentText(identifierID,comment);
			AText	parent;
			mTextInfo.GetGlobalIdentifierParentKeywordText(identifierID,parent);
			AIndex	categoryIndex = mTextInfo.GetGlobalIdentifierCategoryIndex(identifierID);
			if( categoryIndex != kIndex_Invalid )
			{
				AText	categoryName;
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndex,categoryName);
				//�e���ݒ�
				outInfoTextArray.Add(infotext);
				outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(docfile);
				outCommentTextArray.Add(comment);
				outCategoryIndexArray.Add(categoryIndex);
				outScopeArray.Add(kScopeType_Global);
				//���ʎq�L�[���[�h�͈̔͂��擾���Đݒ�
				ATextPoint	spt, ept;
				mTextInfo.GetGlobalIdentifierRange(identifierID,spt,ept);
				ATextIndex	spos = SPI_GetTextIndexFromTextPoint(spt);
				ATextIndex	epos = SPI_GetTextIndexFromTextPoint(ept);
				outStartIndexArray.Add(spos);
				outEndIndexArray.Add(epos);
				outParentKeywordArray.Add(parent);
			}
		}
	}
	//�C���|�[�g
	{
		//�L�[���[�h�ɑΉ�����C���|�[�g���ʎq��S�Ď擾
		ATextArray	infoTextArray, commentTextArray, parentTextArray;
		AArray<AIndex>	categoryIndexArray;
		AObjectArray<AFileAcc>	fileArray;
		AArray<ATextIndex>	startArray;
		AArray<ATextIndex>	endArray;
		mImportIdentifierLinkList.GetIdentifierListByKeyword(inText,categoryIndexArray,infoTextArray,fileArray,startArray,endArray,
					commentTextArray,parentTextArray);
		//�V�X�e���w�b�_�t�@�C�� #253
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSystemHeaderIdentifierLinkList().
				GetIdentifierListByKeyword(inText,categoryIndexArray,infoTextArray,fileArray,startArray,endArray,
				commentTextArray,parentTextArray);
		//�e���ʎq���Ƃɏ���
		for( AIndex i = 0; i < infoTextArray.GetItemCount(); i++ )
		{
			//�e���ݒ�
			outInfoTextArray.Add(infoTextArray.GetTextConst(i));
			outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(fileArray.GetObjectConst(i));
			outCommentTextArray.Add(commentTextArray.GetTextConst(i));
			outCategoryIndexArray.Add(categoryIndexArray.Get(i));
			outScopeArray.Add(kScopeType_Import);
			outStartIndexArray.Add(startArray.Get(i));
			outEndIndexArray.Add(endArray.Get(i));
			outParentKeywordArray.Add(parentTextArray.GetTextConst(i));
		}
	}
}

//
ABool	ADocument_Text::SPI_IsCodeChar( const ATextPoint& inPoint ) const
{
	AArray<ABool>	isCode;
	mTextInfo.GetIsCodeArray(mText,inPoint.y,isCode);
	return isCode.Get(inPoint.x);
}

/**
���ʓ��͈͎擾
@note inLimitSearchRange��true���w�肷��ƃT�[�`�͈͂͌��肳���B
*/
ABool	ADocument_Text::SPI_GetBraceSelection( ATextPoint& ioStart, ATextPoint& ioEnd, const ABool inLimitSearchRange ) const
{
	ABool	result = true;
	ANumber	braceLev = 0;
	ANumber	bracketLev = 0;
	ANumber	kakkoLev = 0;
	
	//#853
	//�����J�n�O�̈ʒu���L��
	ATextPoint	originalStart = ioStart;
	ATextPoint	originalEnd = ioEnd;
	
	//���ʊJ�n�ʒu�v�Z
	ABool	loopend = false;
	ATextIndex	spos = SPI_GetTextIndexFromTextPoint(ioStart);
	ATextPoint	spt;
	SPI_GetTextPointFromTextIndex(spos,spt);
	spt.x--;
	spos--;
	AArray<ABool>	isCode;
	while(true)
	{
		//inLimitSearchRange��true�̏ꍇ�A�T�[�`�͈͌��肷�� #853
		if( inLimitSearchRange == true )
		{
			if( originalStart.y - spt.y > kLimit_BraseSelectionSearchRange )
			{
				//�߂肷������AioStart, ioEnd�����ɖ߂��āA���^�[��
				ioStart = originalStart;
				ioEnd = originalEnd;
				return false;
			}
		}
		//
		mTextInfo.GetIsCodeArray(mText,spt.y,isCode);
		for( ; spt.x >= 0; spt.x--, spos-- )
		{
			if( isCode.Get(spt.x) == false )
			{
				continue;
			}
			AUChar	ch = mText.Get(spos);
			switch(ch)
			{
			  case '{':
				{
					braceLev--;
					break;
				}
			  case '}':
				{
					braceLev++;
					break;
				}
			  case '[':
				{
					bracketLev--;
					break;
				}
			  case ']':
				{
					bracketLev++;
					break;
				}
			  case '(':
				{
					kakkoLev--;
					break;
				}
			  case ')':
				{
					kakkoLev++;
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			if( braceLev < 0 || bracketLev < 0 || kakkoLev < 0 )
			{
				spos++;
				loopend = true;
				break;
			}
		}
		if( loopend == true )   break;
		spt.y--;
		if( spt.y < 0 )
		{
			result = false;
			spos = 0;
			break;
		}
		spt.x = SPI_GetLineLengthWithoutLineEnd(spt.y)-1;
		spos = SPI_GetTextIndexFromTextPoint(spt);
	}
	
	//���ʏI���ʒu�v�Z
	loopend = false;
	braceLev = 0;
	bracketLev = 0;
	kakkoLev = 0;
	ATextPoint	ept = ioEnd;
	ATextIndex	epos = SPI_GetTextIndexFromTextPoint(ept);
	while(true)
	{
		//inLimitSearchRange��true�̏ꍇ�A�T�[�`�͈͌��肷��
		if( inLimitSearchRange == true )
		{
			if( ept.y - originalEnd.y > kLimit_BraseSelectionSearchRange )
			{
				//�i�݂�������AioStart, ioEnd�����ɖ߂��āA���^�[��
				ioStart = originalStart;
				ioEnd = originalEnd;
				return false;
			}
		}
		//
		mTextInfo.GetIsCodeArray(mText,ept.y,isCode);
		AItemCount	lineLength = SPI_GetLineLengthWithoutLineEnd(ept.y);
		for( ; ept.x < lineLength; ept.x++, epos++ )
		{
			if( isCode.Get(ept.x) == false )
			{
				continue;
			}
			AUChar	ch = mText.Get(epos);
			switch(ch)
			{
			  case '{':
					{
						braceLev--;
						break;
					}
			  case '}':
				{
					braceLev++;
					break;
				}
			  case '[':
				{
					bracketLev--;
					break;
				}
			  case ']':
				{
					bracketLev++;
					break;
				}
			  case '(':
				{
					kakkoLev--;
					break;
				}
			  case ')':
				{
					kakkoLev++;
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			if( braceLev > 0 || bracketLev > 0 || kakkoLev > 0 )
			{
				loopend = true;
				break;
			}
		}
		if( loopend == true )   break;
		ept.y++;
		if( ept.y >= SPI_GetLineCount() )
		{
			result = false;
			epos = SPI_GetTextLength();
			break;
		}
		ept.x = 0;
		epos = SPI_GetTextIndexFromTextPoint(ept);
	}
	//
	SPI_GetTextPointFromTextIndex(spos,ioStart);
	SPI_GetTextPointFromTextIndex(epos,ioEnd);
	return result;
}

//
ABool	ADocument_Text::SPI_CheckBrace( const ATextPoint& inCaretTextPoint, ATextPoint& outBraceTextPoint, ABool& outCheckOK ) const
{
	//
	ABoolArray	isCode;
	mTextInfo.GetIsCodeArray(mText,inCaretTextPoint.y,isCode);
	if( inCaretTextPoint.x-1 >= 0 )
	{
		if( isCode.Get(inCaretTextPoint.x-1) == false )   return false;
	}
	
	//
	ATextIndex	spos = SPI_GetTextIndexFromTextPoint(inCaretTextPoint);
	spos--;
	if( spos-1 < 0 )   return false;
	ATextPoint	pt;
	SPI_GetTextPointFromTextIndex(spos-1,pt);
	ABool	found = false;
	ABool	end = false;
	AUChar	out = mText.Get(spos);
	AUChar	in = 0;
	ANumber	braceLev = 0;
	ANumber	bracketLev = 0;
	ANumber	kakkoLev = 0;
	switch(out)
	{
	  case '}':
		{
			in = '{';
			braceLev = 1;
			break;
		}
	  case ')':
		{
			in = '(';
			kakkoLev = 1;
			break;
		}
	  case ']':
		{
			in = '[';
			bracketLev = 1;
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	if( in == 0 )   return false;
	for( ; pt.y >= 0; pt.y-- )
	{
		isCode.DeleteAll();
		mTextInfo.GetIsCodeArray(mText,pt.y,isCode);
		AIndex	lineStart = SPI_GetLineStart(pt.y);
		for( ; pt.x >= 0; pt.x-- )
		{
			if( pt.x >= isCode.GetItemCount() )   continue;
			if( isCode.Get(pt.x) == false )   continue;
			AUChar	ch = mText.Get(lineStart+pt.x);
			switch(ch)
			{
			  case '{':
				{
					braceLev--;
					break;
				}
			  case '}':
				{
					braceLev++;
					break;
				}
			  case '[':
				{
					bracketLev--;
					break;
				}
			  case ']':
				{
					bracketLev++;
					break;
				}
			  case '(':
				{
					kakkoLev--;
					break;
				}
			  case ')':
				{
					kakkoLev++;
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			switch(in)
			{
			  case '{':
				{
					if( braceLev == 0 )
					{
						if( bracketLev == 0 && kakkoLev == 0 )    found = true;
						end = true;
					}
					break;
				}
			  case '(':
				{
					if( kakkoLev == 0 )
					{
						if( braceLev == 0 && bracketLev == 0 )   found = true;
						end = true;
					}
					break;
				}
			  case '[':
				{
					if( bracketLev == 0 )
					{
						if( braceLev == 0 && kakkoLev == 0 )   found = true;
						end = true;
					}
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			if( end == true )   break;
		}
		if( end == true )   break;
		if( pt.y == 0 )   break;
		pt.x = SPI_GetLineLengthWithoutLineEnd(pt.y-1);
	}
	outBraceTextPoint = pt;
	outCheckOK = found;
	return true;
}

//�Ή����������
ABool	ADocument_Text::SPI_GetCorrespondText( const ATextPoint& inCaretTextPoint, AText& outText, ATextPoint& outStart, ATextPoint& outEnd ) const//R0000
{
	return CalcCorrespond(inCaretTextPoint,inCaretTextPoint,true,outText,outStart,outEnd);
#if 0
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSyntaxDefinition();
	AIndex	stateIndex = SPI_GetStateIndexByLineIndex(inCaretTextPoint.y);
	AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
	
	//#12
	//�I��������̕����񒷂���������D�悵�Ĉ�v������
	//startTextArray, endTextArray�ɏI�������񒷂�������i�[����
	//abc
	//def
	//abcd
	//�Ȃ�
	//abcd
	//abc
	//def
	//�̏��ɂ���B
	ATextArray	startTextArray, endTextArray;
	//startTextArray, endTextArray�ɒǉ����Ă���
	AItemCount	itemCountInModePref = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetItemCount_TextArray(AModePrefDB::kCorrespond_StartText);
	for( AIndex i = 0; i < itemCountInModePref; i++ )
	{
		//�ǉ��ς݂̔z�����납�猟�����āA�ǉ����悤�Ƃ��Ă��镶����ȏ�̒����̕����񂪂͂��߂Č��������ʒu�́A���̈ʒu�ɒǉ�����
		//����ɂ�蕶���񒷂��������ɂȂ�B���A���������Ȃ猳�̏��Ԃ��ۂ����B
		AItemCount	len = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_StartText).GetTextConst(i).GetItemCount();
		AIndex j = startTextArray.GetItemCount()-1;
		for( ; j >= 0 ; j-- )
		{
			if( startTextArray.GetTextConst(j).GetItemCount() >= len )
			{
				startTextArray.Insert1(j+1, GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_StartText).GetTextConst(i));
				endTextArray.Insert1(j+1, GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_EndText).GetTextConst(i));
				break;
			}
		}
		//�ǉ����悤�Ƃ��Ă��镶����ȏ�̒����̕����񂪌�����Ȃ��ꍇ�͍ŏ��ɒǉ�����B
		if( j < 0 )
		{
			startTextArray.Insert1(0, GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_StartText).GetTextConst(i));
			endTextArray.Insert1(0, GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_EndText).GetTextConst(i));
		}
	}
	//
	if( syntaxPartIndex != kIndex_Invalid )
	{
		const ATextArray&	sdfStartTextArray = syntaxDefinition.GetCorrespondStartArray(syntaxPartIndex);
		const ATextArray&	sdfEndTextArray = syntaxDefinition.GetCorrespondEndArray(syntaxPartIndex);
		AItemCount	itemCountInSDF = sdfStartTextArray.GetItemCount();
		for( AIndex i = 0; i < itemCountInSDF; i++ )
		{
			//�ǉ��ς݂̔z�����납�猟�����āA�ǉ����悤�Ƃ��Ă��镶����ȏ�̒����̕����񂪂͂��߂Č��������ʒu�́A���̈ʒu�ɒǉ�����
			//����ɂ�蕶���񒷂��������ɂȂ�B���A���������Ȃ猳�̏��Ԃ��ۂ����B
			AItemCount	len = sdfStartTextArray.GetTextConst(i).GetItemCount();
			AIndex j = startTextArray.GetItemCount()-1;
			for( ; j >= 0 ; j-- )
			{
				if( startTextArray.GetTextConst(j).GetItemCount() >= len )
				{
					startTextArray.Insert1(j+1, sdfStartTextArray.GetTextConst(i));
					endTextArray.Insert1(j+1, sdfEndTextArray.GetTextConst(i));
					break;
				}
			}
			//�ǉ����悤�Ƃ��Ă��镶����ȏ�̒����̕����񂪌�����Ȃ��ꍇ�͍ŏ��ɒǉ�����B
			if( j < 0 )
			{
				startTextArray.Insert1(0, sdfStartTextArray.GetTextConst(i));
				endTextArray.Insert1(0, sdfEndTextArray.GetTextConst(i));
			}
		}
	}
	
	//
	AItemCount	startTextArrayCount = startTextArray.GetItemCount();
	if( startTextArrayCount == 0 )   return false;
	//
	ANumberArray	levelArray;
	for( AIndex i = 0; i < startTextArrayCount; i++ )
	{
		levelArray.Add(0);
	}
	
	//B0443 �S�̓I�ɏ�������
	//�y�A���S���Y���z
	//�L�����b�g�ʒu����߂��Ă��� pos�Ɉʒu�������Ă���
	//levelArray�ɁA�Ή�������ݒ�̊e���ڂ��ƂɁA���x����񂪓����Ă���B�ŏ��͑S��0]]
	
	ATextIndex	spos = SPI_GetTextIndexFromTextPoint(inCaretTextPoint);
	//���[�_���Z�b�V����
	AStEditProgressModalSession	modalSession(kEditProgressType_Correspond,true);
	try
	{
		AStTextPtr	textptr(mText,0,mText.GetItemCount());
		AUChar*	p = textptr.GetPtr();
		AItemCount	textlen = mText.GetItemCount();
		
		for( ATextIndex pos = spos; pos > 0; )
		{
			//���[�_���Z�b�V�����p������
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Correspond,0,true,
																	  spos-pos,
																	  spos) == false )
			{
				break;
			}
			//
			AText	st, et;
			//�Ή�������ݒ�̊e���ږ��Ɋm�F
			ABool	found = false;//B0000 �ŏ��Ɍ����������̂���level�ύX����
			for( AIndex i = 0; i < startTextArrayCount; i++ )
			{
				const AText&	startText = startTextArray.GetTextConst(i);
				const AText&	endText = endTextArray.GetTextConst(i);
				//pos�ȑO�ɏI�������������level�ύX
				if( pos - endText.GetItemCount() >= 0 && endText.GetItemCount() > 0/*#11*/ )
				{
					if( endText.Compare(mText,pos - endText.GetItemCount(),endText.GetItemCount()) == true )
					{
						levelArray.Set(i,levelArray.Get(i)+1);
						found = true;//B0000 �ŏ��Ɍ����������̂���level�ύX����
						pos -= endText.GetItemCount();
					}
				}
				//�I��������D��
				if( found == false )
				//pos�ȑO�ɊJ�n�����������level�ύX
				if( pos - startText.GetItemCount() >= 0 && startText.GetItemCount() > 0/*#11*/ )
				{
					if( startText.Compare(mText,pos - startText.GetItemCount(),startText.GetItemCount()) == true )
					{
						levelArray.Set(i,levelArray.Get(i)-1);
						found = true;//B0000 �ŏ��Ɍ����������̂���level�ύX����
						pos -= startText.GetItemCount();
					}
				}
				//level���}�C�i�X�ɂȂ�����I��
				if( levelArray.Get(i) < 0 && endText.GetItemCount() > 0 )
				{
					outText.SetText(endText);
					SPI_GetTextPointFromTextIndex(pos,outStart);
					SPI_GetTextPointFromTextIndex(pos+startText.GetItemCount(),outEnd);
					return true;
				}
				if( found == true )   break;//B0000 �ŏ��Ɍ����������̂���level�ύX����
			}
			if( found == false )
			{
				pos = SPI_GetPrevCharTextIndex(pos);
			}
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	
#if 0
	//�y�A���S���Y���z
	//�L�����b�g�ʒu����߂��Ă��� pt�Ɉʒu�������Ă���
	//levelArray�ɁA�Ή�������ݒ�̊e���ڂ��ƂɁA���x����񂪓����Ă���B�ŏ��͑S��0
	
	ATextIndex	spos = SPI_GetTextIndexFromTextPoint(inCaretTextPoint);
	//B0443 spos--;
	if( spos < 0 )   return false;
	//B0443 ATextPoint	pt;
	//B0443 SPI_GetTextPointFromTextIndex(spos,pt);
	//ABoolArray	isCode;
	//B0000������AText	startText, endText;
	//B0443 for( ; pt.y >= 0; pt.y-- )
	for( ATextIndex pos = spos; pos > 0; )
	{
		//isCode.DeleteAll();
		//mTextInfo.GetIsCodeArray(mText,SPI_GetModeIndex(),pt.y,isCode);
		//B0443 AIndex	lineStart = SPI_GetLineStart(pt.y);
		//B0443 for( ; pt.x >= 0; pt.x-- )
		//{
			//if( isCode.Get(pt.x) == false )   continue;
			/*B0443 AUChar	ch = mText.Get(lineStart+pt.x);
			if( mText.IsUTF8MultiByte(ch) == true )
			{
				if( mText.IsUTF8FirstByteInMultiByte(ch) == false )   continue;
			}*/
			//�Ή�������ݒ�̊e���ږ��Ɋm�F
			ABool	found = false;//B0000 �ŏ��Ɍ����������̂���level�ύX����
			for( AIndex i = 0; i < taiocount; i++ )
			{
				//B0000������GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArray(AModePrefDB::kCorrespond_StartText,i,startText);
				//B0000������GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArray(AModePrefDB::kCorrespond_EndText,i,endText);
				const AText&	startText = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_StartText).GetTextConst(i);
				const AText&	endText   = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_EndText).GetTextConst(i);
				//pt�ȑO�ɏI�������������level�ύX
				//B0443 if( lineStart+pt.x+endText.GetItemCount() <= mText.GetItemCount() )
				if( pos - endText.GetItemCount() >= 0 )
				{
					//B0443 if( endText.Compare(mText,lineStart+pt.x,endText.GetItemCount()) == true )
					if( endText.Compare(mText,pos - endText.GetItemCount(),endText.GetItemCount()) == true )
					{
						levelArray.Set(i,levelArray.Get(i)+1);
						found = true;//B0000 �ŏ��Ɍ����������̂���level�ύX����
						pos -= endText.GetItemCount();//B0443
					}
				}
				//�I��������D��
				if( found == false )
				//pt�ȑO�ɊJ�n�����������level�ύX
				//B0443 if( lineStart+pt.x+startText.GetItemCount() <= mText.GetItemCount() )
				if( pos - startText.GetItemCount() >= 0 )
				{
					//B0443 if( startText.Compare(mText,lineStart+pt.x,startText.GetItemCount()) == true )
					if( startText.Compare(mText,pos - startText.GetItemCount(),startText.GetItemCount()) == true )
					{
						levelArray.Set(i,levelArray.Get(i)-1);
						found = true;//B0000 �ŏ��Ɍ����������̂���level�ύX����
						pos -= startText.GetItemCount();//B0443
					}
				}
				/*B0443
				if( lineStart+pt.x+endText.GetItemCount() <= mText.GetItemCount() )
				{
					if( endText.Compare(mText,lineStart+pt.x,endText.GetItemCount()) == true )
					{
						levelArray.Set(i,levelArray.Get(i)+1);
						found = true;//B0000 �ŏ��Ɍ����������̂���level�ύX����
					}
				}
				*/
				//level���}�C�i�X�ɂȂ�����I��
				if( levelArray.Get(i) < 0 )
				{
					outText.SetText(endText);
					//R0000
					/*B0443
					outStart = pt;
					outEnd = pt;
					outEnd.x += startText.GetItemCount();
					*/
					SPI_GetTextPointFromTextIndex(pos,outStart);
					SPI_GetTextPointFromTextIndex(pos+startText.GetItemCount(),outEnd);
					return true;
				}
				if( found == true )   break;//B0000 �ŏ��Ɍ����������̂���level�ύX����
			}
		//}
		//B0443 if( pt.y == 0 )   break;
		//B0443 pt.x = SPI_GetLineLengthWithoutLineEnd(pt.y-1);
		//B0443
		if( found == false )
		{
			pos = SPI_GetPrevCharTextIndex(pos);
		}
	}
#endif
	return false;
#endif
}

//#359 #419(������S�̓I�Ɍ�����)
/**
*/
ABool	ADocument_Text::SPI_GetCorrespondenceSelection( const ATextPoint& inStart, const ATextPoint& inEnd,
		ATextPoint& outStart, ATextPoint& outEnd ) const
{
	AText	correspondText;
	return CalcCorrespond(inStart,inEnd,false,correspondText,outStart,outEnd);
}
ABool	ADocument_Text::CalcCorrespond( const ATextPoint& inStart, const ATextPoint& inEnd, 
									   const ABool inFindForCorrespondInput, AText& outCorrespondText,
									   ATextPoint& outStart, ATextPoint& outEnd ) const
{
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSyntaxDefinition();
	AIndex	stateIndex = SPI_GetCurrentStateIndex(inStart);//SPI_GetStateIndexByLineIndex(inStart.y);
	AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
	
	ABool	result = true;
	outStart = inStart;
	outEnd = inEnd;
	
	AObjectArray<AText>	startTextArray, endTextArray;
	
	//==================���[�h�ݒ肩��ǉ�==================
	{
		AItemCount	itemCountInModePref = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetItemCount_TextArray(AModePrefDB::kCorrespond_StartText);
		//���[�h�ݒ�̃f�[�^��AObjectArray<AText>�փR�s�[
		AObjectArray<AText>	prefStartTextArray, prefEndTextArray;
		for( AIndex i = 0; i < itemCountInModePref; i++ )
		{
			prefStartTextArray.AddNewObject();
			prefEndTextArray.AddNewObject();
			prefStartTextArray.GetObject(i).SetText(GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).
													GetData_TextArrayRef(AModePrefDB::kCorrespond_StartText).GetTextConst(i));
			prefEndTextArray.GetObject(i).SetText(GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).
												  GetData_TextArrayRef(AModePrefDB::kCorrespond_EndText).GetTextConst(i));
		}
		//startTextArray, endTextArray�ɒǉ����Ă���
		for( AIndex i = 0; i < itemCountInModePref; i++ )
		{
			//�ǉ��ς݂̔z�����납�猟�����āA�ǉ����悤�Ƃ��Ă��镶����ȏ�̒����̕����񂪂͂��߂Č��������ʒu�́A���̈ʒu�ɒǉ�����
			//����ɂ�蕶���񒷂��������ɂȂ�B���A���������Ȃ猳�̏��Ԃ��ۂ����B
			AItemCount	len = prefStartTextArray.GetObjectConst(i).GetItemCount();
			AIndex j = startTextArray.GetItemCount()-1;
			for( ; j >= 0 ; j-- )
			{
				if( startTextArray.GetObjectConst(j).GetItemCount() >= len )
				{
					startTextArray.InsertNew1Object(j+1);
					endTextArray.InsertNew1Object(j+1);
					startTextArray.GetObject(j+1).SetText(prefStartTextArray.GetObjectConst(i));
					endTextArray.GetObject(j+1).SetText(prefEndTextArray.GetObjectConst(i));
					break;
				}
			}
			//�ǉ����悤�Ƃ��Ă��镶����ȏ�̒����̕����񂪌�����Ȃ��ꍇ�͍ŏ��ɒǉ�����B
			if( j < 0 )
			{
				startTextArray.InsertNew1Object(0);
				endTextArray.InsertNew1Object(0);
				startTextArray.GetObject(0).SetText(prefStartTextArray.GetObjectConst(i));
				endTextArray.GetObject(0).SetText(prefEndTextArray.GetObjectConst(i));
			}
		}
	}
	//==================SDF����擾==================
	//
	if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kEnableSDFCorrespond) == true )
	{
		if( syntaxPartIndex != kIndex_Invalid )
		{
			AItemCount	itemCountInSDF = syntaxDefinition.GetCorrespondStartArray(syntaxPartIndex).GetItemCount();
			//SDF�̃f�[�^��AObjectArray<AText>�փR�s�[
			AObjectArray<AText>	sdfStartTextArray, sdfEndTextArray;
			for( AIndex i = 0; i < itemCountInSDF; i++ )
			{
				sdfStartTextArray.AddNewObject();
				sdfEndTextArray.AddNewObject();
				sdfStartTextArray.GetObject(i).SetText(syntaxDefinition.GetCorrespondStartArray(syntaxPartIndex).GetTextConst(i));
				sdfEndTextArray.GetObject(i).SetText(syntaxDefinition.GetCorrespondEndArray(syntaxPartIndex).GetTextConst(i));
			}
			//
			for( AIndex i = 0; i < itemCountInSDF; i++ )
			{
				//�ǉ��ς݂̔z�����납�猟�����āA�ǉ����悤�Ƃ��Ă��镶����ȏ�̒����̕����񂪂͂��߂Č��������ʒu�́A���̈ʒu�ɒǉ�����
				//����ɂ�蕶���񒷂��������ɂȂ�B���A���������Ȃ猳�̏��Ԃ��ۂ����B
				AItemCount	len = sdfStartTextArray.GetObjectConst(i).GetItemCount();
				AIndex j = startTextArray.GetItemCount()-1;
				for( ; j >= 0 ; j-- )
				{
					if( startTextArray.GetObjectConst(j).GetItemCount() >= len )
					{
						startTextArray.InsertNew1Object(j+1);
						endTextArray.InsertNew1Object(j+1);
						startTextArray.GetObject(j+1).SetText(sdfStartTextArray.GetObjectConst(i));
						endTextArray.GetObject(j+1).SetText(sdfEndTextArray.GetObjectConst(i));
						break;
					}
				}
				//�ǉ����悤�Ƃ��Ă��镶����ȏ�̒����̕����񂪌�����Ȃ��ꍇ�͍ŏ��ɒǉ�����B
				if( j < 0 )
				{
					startTextArray.InsertNew1Object(0);
					endTextArray.InsertNew1Object(0);
					startTextArray.GetObject(0).SetText(sdfStartTextArray.GetObjectConst(i));
					endTextArray.GetObject(0).SetText(sdfEndTextArray.GetObjectConst(i));
				}
			}
		}
	}
	
	//�J�n������ƏI�������񂪓����ꍇ��A�ǂ��炩����̏ꍇ�͑ΏۊO�ɂ���
	for( AIndex i = 0; i < startTextArray.GetItemCount(); )
	{
		if( startTextArray.GetObjectConst(i).GetItemCount() == 0 ||
		   //endTextArray.GetObjectConst(i).GetItemCount() == 0 ||
		   startTextArray.GetObjectConst(i).Compare(endTextArray.GetObjectConst(i)) == true )
		{
			startTextArray.Delete1Object(i);
			endTextArray.Delete1Object(i);
		}
		else
		{
			i++;
		}
	}
	
	//
	AItemCount	startTextArrayCount = startTextArray.GetItemCount();
	if( startTextArrayCount == 0 )   return false;
	
	AStTextPtr	textptr(mText,0,mText.GetItemCount());
	AUChar*	p = textptr.GetPtr();
	AItemCount	textlen = mText.GetItemCount();
	
	//�J�n�ʒu�A�Ή�������C���f�b�N�X����
	AArray<ABool>	isCodeArray;
	AIndex	isCodeArray_LineIndex = inStart.y;
	AIndex	isCodeArray_LineStart = SPI_GetLineStart(isCodeArray_LineIndex);
	mTextInfo.GetIsCodeArray(mText,isCodeArray_LineIndex,isCodeArray);
	ATextIndex	spos = SPI_GetTextIndexFromTextPoint(inStart);
	spos = SPI_GetPrevCharTextIndex(spos);
	//
	AIndex	resultspos = spos;
	AIndex	resultepos = spos;
	//==================�J�n�ʒu����==================
	AIndex	taioStartIndex = kIndex_Invalid;
	//���[�_���Z�b�V����
	AStEditProgressModalSession	modalSession(kEditProgressType_Correspond,true,false,true);
	try
	{
		AArray<AIndex>	taioEndIndexStack;
		for( ; spos >= 0; spos = SPI_GetPrevCharTextIndex(spos,true,true) )
		{
			//���[�_���Z�b�V�����p������
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Correspond,0,true,
																	  (resultspos-spos)/2,resultspos) == false )
			{
				taioStartIndex = kIndex_Invalid;
				result = false;
				break;
			}
			//Code�������ǂ����̔���
			if( spos < isCodeArray_LineStart )
			{
				ATextPoint	pt;
				SPI_GetTextPointFromTextIndex(spos,pt,false);
				isCodeArray_LineIndex = pt.y;
				isCodeArray_LineStart = SPI_GetLineStart(isCodeArray_LineIndex);
				mTextInfo.GetIsCodeArray(mText,isCodeArray_LineIndex,isCodeArray);
			}
			if( isCodeArray.Get(spos-isCodeArray_LineStart) == false )   continue;
			//
			AUChar	ch = p[spos];
			if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_LineEnd )
			{
				continue;
			}
			//
			AText	st, et;
			//�e�Ή������񃋁[�v
			for( AIndex i = 0; i < startTextArrayCount; i++ )
			{
				const AText&	startText = startTextArray.GetObjectConst(i);
				const AText&	endText = endTextArray.GetObjectConst(i);
				//------------------�J�n�����񌟍�------------------
				//
				if( st.GetItemCount() != startText.GetItemCount() )
				{
					AText::GetCharactersOtherThanSpaces(p,textlen,spos,startText.GetItemCount(),st);
				}
				//
				if( startText.Compare(st) == true )
				{
					//
					if( endText.GetItemCount() == 0 )
					{
						break;
					}
					//
					if( taioEndIndexStack.GetItemCount() == 0 )
					{
						//�I��������X�^�b�N���Ȃ��ꍇ�́A���̊J�n�����ȍ~��I������
						taioStartIndex = i;
						resultspos = spos;
						spos += startText.GetItemCount();
					}
					else
					{
						if( taioEndIndexStack.Get(taioEndIndexStack.GetItemCount()-1) == i )
						{
							//�J�n�ƏI����index����v���Ă���΁A�I��������X�^�b�N�����菜��
							taioEndIndexStack.Delete1(taioEndIndexStack.GetItemCount()-1);
						}
						else
						{
							//��v���Ă��Ȃ��ꍇ�́A���̊J�n������ȍ~��I������
							result = false;
							taioStartIndex = i;
							resultspos = spos;
							spos += startText.GetItemCount();
						}
					}
					break;
				}
				//�I�������񌟍�
				if( endText.GetItemCount() > 0 )
				{
					//
					if( et.GetItemCount() != endText.GetItemCount() )
					{
						AText::GetCharactersOtherThanSpaces(p,textlen,spos,endText.GetItemCount(),et);
					}
					//
					if( endText.Compare(et) == true )
					{
						//�I��������X�^�b�N�֒ǉ�
						taioEndIndexStack.Add(i);
						break;
					}
				}
			}
			if( taioStartIndex != kIndex_Invalid )   break;
		}
		//
		if( spos < 0 )   spos = 0;
		//
		if( taioStartIndex == kIndex_Invalid )
		{
			return false;
		}
		
		if( inFindForCorrespondInput == true )
		{
			outCorrespondText.SetText(endTextArray.GetObjectConst(taioStartIndex));
			SPI_GetTextPointFromTextIndex(resultspos,outStart);
			outEnd = inEnd;
			return true;
		}
		
		//==================�I���ʒu����==================
		ATextIndex	epos = spos;
		//
		AIndex	taioEndIndex = kIndex_Invalid;
		AArray<AIndex>	taioIndexStack;
		taioIndexStack.Add(taioStartIndex);
		for( ; epos < textlen; epos = SPI_GetNextCharTextIndex(epos) )
		{
			//���[�_���Z�b�V�����p������
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Correspond,0,true,
																	  textlen/2+epos,textlen) == false )
			{
				taioEndIndex = kIndex_Invalid;
				break;
			}
			//Code�������ǂ����̔���
			if( epos >= isCodeArray_LineStart+isCodeArray.GetItemCount() )
			{
				ATextPoint	pt;
				SPI_GetTextPointFromTextIndex(epos,pt,true);
				isCodeArray_LineIndex = pt.y;
				isCodeArray_LineStart = SPI_GetLineStart(isCodeArray_LineIndex);
				mTextInfo.GetIsCodeArray(mText,isCodeArray_LineIndex,isCodeArray);
			}
			if( isCodeArray.Get(epos-isCodeArray_LineStart) == false )   continue;
			//
			AUChar	ch = p[epos];
			if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_LineEnd )
			{
				continue;
			}
			//
			AText	st, et;
			AIndex	et_pos = kIndex_Invalid;
			//�e�Ή������񃋁[�v
			for( AIndex i = 0; i < startTextArrayCount; i++ )
			{
				const AText&	startText = startTextArray.GetObjectConst(i);
				const AText&	endText = endTextArray.GetObjectConst(i);
				//------------------�J�n�����񌟍�------------------
				//
				if( st.GetItemCount() != startText.GetItemCount() )
				{
					AText::GetCharactersOtherThanSpaces(p,textlen,epos,startText.GetItemCount(),st);
				}
				//
				if( startText.Compare(st) == true )
				{
					//
					if( endText.GetItemCount() > 0 )
					{
						//�X�^�b�N�֒ǉ�
						taioIndexStack.Add(i);
					}
					break;
				}
				//------------------�I�������񌟍�------------------
				if( endText.GetItemCount() > 0 )
				{
					//
					if( et.GetItemCount() != endText.GetItemCount() )
					{
						et_pos = AText::GetCharactersOtherThanSpaces(p,textlen,epos,endText.GetItemCount(),et);
					}
					//
					if( endText.Compare(et) == true )
					{
						if( taioIndexStack.Get(taioIndexStack.GetItemCount()-1) == i )
						{
							//�X�^�b�N�̍ŏ��ƈ�v�����ꍇ�̓X�^�b�N�����菜��
							taioIndexStack.Delete1(taioIndexStack.GetItemCount()-1);
							//�X�^�b�N������ɂȂ�����I��
							if( taioIndexStack.GetItemCount() == 0 )
							{
								taioEndIndex = i;
							}
						}
						else
						{
							//��v���Ă��Ȃ��ꍇ�́A���̏I��������ȑO��I������
							result = false;
							taioEndIndex = i;
						}
						//
						resultepos = et_pos;
						break;
					}
				}
				
			}
			if( taioEndIndex != kIndex_Invalid )   break;
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	//
	SPI_GetTextPointFromTextIndex(resultspos,outStart);
	SPI_GetTextPointFromTextIndex(resultepos,outEnd);
	return result;
}

const AItemCount	kPreHitTextLength = 100;
const AItemCount	kPostHitTextLength = 100;

//
//
void	ADocument_Text::SPI_ReportTextEncodingErrors()
{
	AText	returntext;
	returntext.SetLocalizedText("ReturnCodeText");
	AText	wintitle("TextEncoding Errors");
	ADocumentID	docID = GetApp().SPI_GetOrCreateFindResultWindowDocument();//#725
	AText	grouptitle1, grouptitle2;
	grouptitle1.SetLocalizedText("IndexWindow_Title_TextEncodingErrors");
	AFileAcc	basefolder;//#465
	NVI_GetParentFolder(basefolder);//#465
	GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_InsertGroup(0,grouptitle1,grouptitle2,basefolder);//#465
	//#208
	ABool	checkKishuIson = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCheckEncodingError_KishuIzon);
	ABool	checkHankakuKana = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCheckEncodingError_HankakuKana);
	//
	AItemCount	count = mText.GetItemCount();
	for( ATextIndex pos = 0; pos < count; )
	{
		ATextIndex	spos = pos;
		AText	ch;
		mText.Get1UTF8Char(pos,ch);
		ABool	convertError = false;//#208
		ABool	kishuIzon = false;//#208
		ABool	hankakuKana = false;//#208
		//#473 SJIS�G���[����
		ABool	shiftJISNo2ndByte = false;
		if( ch.GetItemCount() == 4 )
		{
			if( ch.Get(0) == 0xF4 && ch.Get(1) == 0x85 )
			{
				shiftJISNo2ndByte = true;
			}
		}
		//
		AText	tmp;
		if( ch.ConvertFromUTF8CR(SPI_GetTextEncoding(),returnCode_CR,tmp) == false )
		{
			convertError = true;//#208
		}
		//#208
		else
		{
			ATextEncodingWrapper::CheckJISX0208(SPI_GetTextEncoding(),tmp,kishuIzon,hankakuKana);
		}
		if( convertError == true || 
					(kishuIzon == true && checkKishuIson == true) || 
					(hankakuKana == true && checkHankakuKana == true) || 
					(shiftJISNo2ndByte == true) )//#473
		{
			ATextPoint	textpoint;
			SPI_GetTextPointFromTextIndex(spos,textpoint);
			//
			AFileAcc	file;
			NVI_GetFile(file);
			AText	filepath;
			file.GetPath(filepath);
			//
			AText	prehit;
			ATextIndex	p = spos - kPreHitTextLength;
			if( p < 0 )   p = 0;
			p = mText.GetCurrentCharPos(p);
			mText.GetText(p,spos-p,prehit);
			//
			AText	posthit;
			p = pos + kPostHitTextLength;
			if( p >= mText.GetItemCount() )   p = mText.GetItemCount();
			p = mText.GetCurrentCharPos(p);
			mText.GetText(pos,p-pos,posthit);
			prehit.ReplaceChar(kUChar_CR,returntext);
			posthit.ReplaceChar(kUChar_CR,returntext);
			//�ꏊ���擾
			AText	position;
			SPI_GetPositionTextByLineIndex(textpoint.y,position);
			//#208
			AText	comment;
			//#473
			if( shiftJISNo2ndByte == true )
			{
				comment.SetLocalizedText("TextEncodingError_ShiftJIS_No2ndByte");
			}
			//
			else if( kishuIzon == true )
			{
				comment.SetLocalizedText("TextEncodingError_KishuIzon");
				comment.AddText(ch);
			}
			else if( hankakuKana == true )
			{
				comment.SetLocalizedText("TextEncodingError_HankakuKana");
				comment.AddText(ch);
			}
			else
			{
				comment.SetLocalizedText("TextEncodingError_CannotConvert");
				comment.AddText(ch);
			}
			//#465
			AText	paratext;
			SPI_GetParagraphTextByLineIndex(textpoint.y,paratext);
			//
			GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_AddItem_TextPosition(0,filepath,ch,prehit,posthit,position,
					paratext,//#465
					spos,pos-spos,SPI_GetParagraphIndexByLineIndex(textpoint.y),comment);
		}
	}
}

//R0199
//�X�y���`�F�b�N�ꗗ�\��
void	ADocument_Text::SPI_ReportSpellCheck()
{
	AText	returntext;
	returntext.SetLocalizedText("ReturnCodeText");
	AText	wintitle("TextEncoding Errors");
	ADocumentID	docID = GetApp().SPI_GetOrCreateFindResultWindowDocument();//#725
	AText	grouptitle1, grouptitle2;
	grouptitle1.SetLocalizedText("IndexWindow_Title_Misspell");
	AFileAcc	basefolder;//#465
	NVI_GetParentFolder(basefolder);//#465
	GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_InsertGroup(0,grouptitle1,grouptitle2,basefolder);//#465
	for( AIndex lineIndex = 0; lineIndex < mTextInfo.GetLineCount(); lineIndex++ )
	{
		//
		CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
		ATextPoint	tmp1, tmp2;
		AAnalyzeDrawData	analyze;
		GetTextDrawDataWithStyle(mText,mTextInfo,lineIndex,textDrawData,false,tmp1,tmp2,analyze,kObjectID_Invalid);//#695
		//
		for( AIndex index = 0; index < textDrawData.misspellStartArray.GetItemCount(); index++ )
		{
			ATextPoint	textspt;
			textspt.x = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(textDrawData.misspellStartArray.Get(index));
			textspt.y = lineIndex;
			ATextPoint	textept;
			textept.x = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(textDrawData.misspellEndArray.Get(index));
			textept.y = lineIndex;
			ATextIndex	spos = SPI_GetTextIndexFromTextPoint(textspt);
			ATextIndex	epos = SPI_GetTextIndexFromTextPoint(textept);
			//
			AFileAcc	file;
			NVI_GetFile(file);
			AText	filepath;
			file.GetPath(filepath);
			//
			AText	prehit;
			ATextIndex	p = spos - kPreHitTextLength;
			if( p < 0 )   p = 0;
			p = mText.GetCurrentCharPos(p);
			mText.GetText(p,spos-p,prehit);
			//
			AText	posthit;
			p = epos + kPostHitTextLength;
			if( p >= mText.GetItemCount() )   p = mText.GetItemCount();
			p = mText.GetCurrentCharPos(p);
			mText.GetText(epos,p-epos,posthit);
			prehit.ReplaceChar(kUChar_CR,returntext);
			posthit.ReplaceChar(kUChar_CR,returntext);
			//�ꏊ���擾
			AText	position;
			SPI_GetPositionTextByLineIndex(lineIndex,position);
			//#465
			AText	paratext;
			SPI_GetParagraphTextByLineIndex(textspt.y,paratext);
			//
			AText	text;
			SPI_GetText(spos,epos,text);
			GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_AddItem_TextPosition(0,filepath,text,prehit,posthit,position,
					paratext,//#465
					spos,epos-spos,SPI_GetParagraphIndexByLineIndex(lineIndex),GetEmptyText());
		}
	}
}

/**
�w��s�ɑΉ�����menu identifier�̃e�L�X�g���擾
*/
void	ADocument_Text::SPI_GetPositionTextByLineIndex( const AIndex inLineIndex, AText& outText ) const
{
	outText.DeleteAll();
	//#695 AUniqID	identifierID = mTextInfo.GetMenuIdentifierByLineIndex(inLineIndex);
	//#695 if( identifierID != kUniqID_Invalid )
	AIndex	jumpIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);//#695
	if( jumpIndex != kIndex_Invalid )//#695
	{
		AUniqID identifierID = mJump_IdentifierObjectArray.Get(jumpIndex);//#695
		mTextInfo.GetGlobalIdentifierMenuText(identifierID,outText);
	}
}

void	ADocument_Text::SPI_GetTextCount( ATextCountData& ioTextCount, const AArray<ATextIndex>& inStart, const AArray<ATextIndex>& inEnd )
{
	//�������␧���������p�r�I�ɍ������͕s�v�Ǝv���邽�߁A�o�[�W����2.1�����̃��W�b�N�Ƃ���B#1328
	//#695
	ABool	supportComment = true;
	if( SPI_GetLineCount() > kLimit_TextCount_MaxLineCountForSyntaxRecognize )
	{
		supportComment = false;
	}
	//�i����
	ioTextCount.paragraphCount = SPI_GetParagraphCount();
	//
	ATextInfoForDocument	textInfo(this);
	textInfo.SetMode(SPI_GetModeIndex());
	//#695 AArray<AUniqID>	deletedIdentifier;
	AArray<AUniqID>	addedIdentifier;
	AArray<AIndex>		addedIdentifierLineIndex;
	AText	fontname;//win
	SPI_GetFontName(fontname);//win
	textInfo.CalcLineInfoAll(mText,true,kIndex_Invalid,/*win SPI_GetFont()*/fontname,SPI_GetFontSize(),true,SPI_GetTabWidth(),SPI_GetIndentWidth(),kWrapMode_WordWrapByLetterCount,//#699
				ioTextCount.conditionLetterCount,0,/*#695 deletedIdentifier,*/ioTextCount.countAs2Letters);
	SPI_InitLineImageInfo();//#450
	//
	ioTextCount.lineCountWithCondition = textInfo.GetLineCount();
	//�������J�E���g
	ioTextCount.letterCount = 0;
	ioTextCount.commentLetterCount = 0;
	ioTextCount.letterCountInSelect = 0;
	ioTextCount.commentLetterCountInSelect = 0;
	ioTextCount.paragraphCountInSelect = 0;
	//#695 AIndex	startLineIndex, endLineIndex;
	ABool	importChanged = false;
	AFileAcc	file;//R0000
	NVI_GetFile(file);//R0000
	//
	if( supportComment == true )//#695
	{
		ABool	abortFlag = false;
		AText	url;//#998
		SPI_GetURL(url);//#998
		textInfo.RecognizeSyntaxAll(mText,url,addedIdentifier,addedIdentifierLineIndex,importChanged,abortFlag);//R0000 #698 #998
	}
	AArray<ABool>	isCommentArray;
	AItemCount	selArrayCount = inStart.GetItemCount();
	ABool	selparaflag = false;
	for( AIndex lineIndex = 0; lineIndex < textInfo.GetLineCount(); lineIndex++ )
	{
		if( supportComment == true )//#695
		{
			textInfo.GetIsCommentArray(mText,lineIndex,isCommentArray);
		}
		ATextIndex	spos = textInfo.GetLineStart(lineIndex);
		ATextIndex	epos = spos + textInfo.GetLineLengthWithLineEnd(lineIndex);
		for( ATextIndex pos = spos; pos < epos;  )
		{
			AItemCount	bc = mText.GetUTF8ByteCount(pos);
			AUChar	ch = mText.Get(pos);
			ABool	isComment = false;//#695
			if( supportComment == true )//#695
			{
				isComment = isCommentArray.Get(pos-spos);
			}
			ABool	inSelect = false;
			for( AIndex i = 0; i < selArrayCount; i++ )
			{
				if( inStart.Get(i) <= pos && pos < inEnd.Get(i) )
				{
					inSelect = true;
					break;
				}
			}
			if( inSelect )
			{
				if( ch == kUChar_LineEnd )
				{
					ioTextCount.paragraphCountInSelect++;
					selparaflag = false;
				}
				else
				{
					selparaflag = true;
				}
			}
			if( ioTextCount.countAs2Letters == true && bc >= 3 )
			{
				ioTextCount.letterCount += 2;
				if( isComment == true )
				{
					ioTextCount.commentLetterCount += 2;
				}
				if( inSelect == true )
				{
					ioTextCount.letterCountInSelect += 2;
					if( isComment == true )
					{
						ioTextCount.commentLetterCountInSelect += 2;
					}
				}
			}
			else
			{
				if( (ch==kUChar_Space||ch==kUChar_Tab) && ioTextCount.countSpaceTab == false )
				{
					//�J�E���g���Ȃ�
				}
				else if( ch == kUChar_LineEnd && ioTextCount.countReturn == false )
				{
					//�J�E���g���Ȃ�
				}
				else
				{
					ioTextCount.letterCount++;
					if( isComment == true )
					{
						ioTextCount.commentLetterCount++;
					}
					if( inSelect == true )
					{
						ioTextCount.letterCountInSelect ++;
						if( isComment == true )
						{
							ioTextCount.commentLetterCountInSelect ++;
						}
					}
				}
			}
			pos += bc;
		}
	}
	if( selparaflag == true )   ioTextCount.paragraphCountInSelect++;
	//#695
	if( supportComment == false )
	{
		ioTextCount.commentLetterCount = kIndex_Invalid;
		ioTextCount.commentLetterCountInSelect = kIndex_Invalid;
	}
}

void	ADocument_Text::SPI_GetTextForOpenSelected( const ATextPoint& inSelectStart, const ATextPoint& inSelectEnd, AText& outPathText ) const
{
	outPathText.DeleteAll();
	ATextIndex	spos = SPI_GetTextIndexFromTextPoint(inSelectStart);
	for( ATextIndex	pos = SPI_GetPrevCharTextIndex(spos); pos > 0; pos = SPI_GetPrevCharTextIndex(pos) )
	{
		AUChar	ch = mText.Get(pos);
		if( SPI_IsAsciiAlphabet(pos) == false && ch != '.' && ch != '/' && ch != ':' && ch != '%' && ch != '~' &&
				ch != '?' && ch != '(' && ch != ')' && ch != '-' && ch != '=' )
		{
			spos = SPI_GetNextCharTextIndex(pos);
			break;
		}
	}
	ATextIndex	epos = SPI_GetTextIndexFromTextPoint(inSelectEnd);
	for( ATextIndex	pos = epos; pos < mText.GetItemCount(); pos = SPI_GetNextCharTextIndex(pos) )
	{
		AUChar	ch = mText.Get(pos);
		if( SPI_IsAsciiAlphabet(pos) == false && ch != '.' && ch != '/' && ch != ':' && ch != '%' && ch != '~' &&
				ch != '?' && ch != '(' && ch != ')' && ch != '-' && ch != '=' )
		{
			epos = pos;
			break;
		}
	}
	if( epos-spos <= 0 )   return;
	mText.GetText(spos,epos-spos,outPathText);
}

//#142
/**
�������E�P�ꐔ�擾�i�w��͈́j
*/
void	ADocument_Text::SPI_GetWordCount( const ATextPoint& inStartTextPoint, const ATextPoint& inEndTextPoint,
		AItemCount& outCharCount, AItemCount& outWordCount, AItemCount& outParagraphCount ) const
{
	AIndex	spos = SPI_GetTextIndexFromTextPoint(inStartTextPoint);
	AIndex	epos = SPI_GetTextIndexFromTextPoint(inEndTextPoint);
	mTextInfo.GetWordCount(mText,spos,epos-spos,outCharCount,outWordCount,outParagraphCount);
}

/**
SyntaxWarning�擾
*/
ABool	ADocument_Text::SPI_GetSyntaxWarning( const AIndex inLineIndex ) const
{
	return mTextInfo.GetSyntaxWarning(inLineIndex);
}

/**
SyntaxError�擾
*/
ABool	ADocument_Text::SPI_GetSyntaxError( const AIndex inLineIndex ) const
{
	return mTextInfo.GetSyntaxError(inLineIndex);
}

//#992
/**
���@�`�F�b�J�[�f�[�^�S�폜
*/
void	ADocument_Text::SPI_ClearCheckerPluginWarnings()
{
	mCheckerPlugin_ParagraphNumber.DeleteAll();
	mCheckerPlugin_ColIndex.DeleteAll();
	mCheckerPlugin_TitleText.DeleteAll();
	mCheckerPlugin_DetailText.DeleteAll();
	mCheckerPlugin_TypeIndex.DeleteAll();
	mCheckerPlugin_DisplayInTextView.DeleteAll();
	//�`��X�V
	SPI_RefreshTextViews();
}

//#992
/**
���@�`�F�b�J�[�f�[�^�ǉ�
*/
void	ADocument_Text::SPI_AddCheckerPluginWarning( const ANumber inParagraphNumber, const AIndex inColIndex,
													 const AText& inTitleText, const AText& inDetailText,
													 const AIndex inTypeIndex, const ABool inDisplayInTextView )
{
	mCheckerPlugin_ParagraphNumber.Add(inParagraphNumber);
	mCheckerPlugin_ColIndex.Add(inColIndex);
	mCheckerPlugin_TitleText.Add(inTitleText);
	mCheckerPlugin_DetailText.Add(inDetailText);
	mCheckerPlugin_TypeIndex.Add(inTypeIndex);
	mCheckerPlugin_DisplayInTextView.Add(inDisplayInTextView);
	//�`��X�V
	AIndex	lineIndex = SPI_GetLineIndexByParagraphIndex(inParagraphNumber);
	SPI_RefreshTextViews(lineIndex,lineIndex+1);
}

//#992
/**
���@�`�F�b�J�[�f�[�^�擾
*/
void	ADocument_Text::SPI_GetCheckerPluginWarnings( const ANumber inParagraphNumber, 
													  ANumberArray& outColIndexArray, 
													  ATextArray& outTitleTextArray, ATextArray& outDetailTextArray,
													  AColorArray& outColorArray, ABoolArray& outDisplayInTextViewArray ) const
{
	//���ʑO�폜
	outColIndexArray.DeleteAll();
	outTitleTextArray.DeleteAll();
	outDetailTextArray.DeleteAll();
	outColorArray.DeleteAll();
	outDisplayInTextViewArray.DeleteAll();
	//�w��i���̃f�[�^���擾
	AArray<AIndex>	indexArray;
	mCheckerPlugin_ParagraphNumber.FindAll(inParagraphNumber,indexArray);
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		//index�擾
		AIndex	index = indexArray.Get(i);
		//���[�j���O�^�C�v�擾
		AIndex	typeIndex = mCheckerPlugin_TypeIndex.Get(index);
		//�^�C�v�����̂Ƃ��������[�j���O�ɒǉ�
		if( typeIndex > 0 )
		{
			//�^�C�v�ɏ]���ĐF�擾
			AColor	color = kColor_White;
			switch(typeIndex)
			{
			  case 1:
			  default:
				{
					color = kColor_White;
					break;
				}
			  case 2:
				{
					color = kColor_Yellow;
					break;
				}
			  case 3:
				{
					color = kColor_Red;
					break;
				}
			}
			//�f�[�^�擾
			outColIndexArray.Add(mCheckerPlugin_ColIndex.Get(index));
			outTitleTextArray.Add(mCheckerPlugin_TitleText.GetTextConst(index));
			outDetailTextArray.Add(mCheckerPlugin_DetailText.GetTextConst(index));
			outColorArray.Add(color);
			outDisplayInTextViewArray.Add(mCheckerPlugin_DisplayInTextView.Get(index));
		}
	}
}

#pragma mark ===========================

#pragma mark ---TextPoint/TextIndex�ϊ�

//TextPoint����Index�擾
AIndex	ADocument_Text::SPI_GetTextIndexFromTextPoint( const ATextPoint& inPoint ) const
{
	//�s�v�Z���܂��i�s��񂪑S���Ȃ��ꍇ�j�̏ꍇ�p�B
	//(0,0)�̏ꍇ�́A�K��0��Ԃ��B
	if( inPoint.x == 0 && inPoint.y == 0 )
	{
		return 0;
	}
	//
	return mTextInfo.GetTextIndexFromTextPoint(inPoint);
}

//Index����TextPoint�擾
void	ADocument_Text::SPI_GetTextPointFromTextIndex( const AIndex inTextPosition, ATextPoint& outPoint, const ABool inPreferNextLine ) const
{
	mTextInfo.GetTextPointFromTextIndex(inTextPosition,outPoint,inPreferNextLine);
}

#pragma mark ===========================

#pragma mark ---�h�L�������g���ݒ�^�擾

//URL�擾 #994
void	ADocument_Text::SPI_GetURL( AText& outURL ) const
{
	if( SPI_IsRemoteFileMode() == true )
	{
		SPI_GetRemoteFileURL(outURL);
	}
	else if( NVI_IsFileSpecified() == false )
	{
		SPI_GetTemporaryTitle(outURL);
	}
	else
	{
		AFileAcc	file;
		NVI_GetFile(file);
		file.GetPath(outURL);
		outURL.InsertCstring(0,"file://");
	}
}

//�^�C�g���擾
void	ADocument_Text::NVIDO_GetTitle( AText& outTitle ) const
{
	if( SPI_IsRemoteFileMode() == true )
	{
		//URL���ƒ����ă^�u�ɕ\��������Ȃ��̂ŁA�t�@�C�����{(�v���g�R����)�݂̂ɂ��� #1247
		AText	url;
		SPI_GetRemoteFileURL(url);
		url.GetFilename(outTitle);
		AText	protocol;
		url.GetFTPURLProtocol(protocol);
		outTitle.AddCstring(" (");
		outTitle.AddText(protocol);
		outTitle.AddCstring(")");
	}
	else if( NVI_IsFileSpecified() == false )
	{
		SPI_GetTemporaryTitle(outTitle);
	}
	else
	{
		AFileAcc	file;
		NVI_GetFile(file);
		file.GetFilename(outTitle);
	}
}

//#699
/**
�������݉۔��菈��
*/
ABool	ADocument_Text::NVIDO_IsReadOnly() const
{
	//ADocument��mReadOnly�t���O��on�Ȃ�read only
	if( mReadOnly == true )   return true;
	//�s�܂�Ԃ��v�Z���Ȃ�read only
	if( SPI_GetDocumentInitState() == kDocumentInitState_WrapCalculating || mWrapCalculatorWorkingByInsertText == true )
	{
		return true;
	}
	//��L�ȊO�͏������݉�
	return false;
}

//R0242
void	ADocument_Text::NVIDO_GetPrintMargin( AFloatNumber& outLeftMargin, AFloatNumber& outRightMargin, AFloatNumber& outTopMargin, AFloatNumber& outBottomMargin ) const
{
	outLeftMargin	= GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintMargin_Left);
	outRightMargin	= GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintMargin_Right);
	outTopMargin	= GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintMargin_Top);
	outBottomMargin	= GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintMargin_Bottom);
}

//�t�@�C�����擾
void	ADocument_Text::SPI_GetFilename( AText& outFilename ) const
{
	if( SPI_IsRemoteFileMode() == true )
	{
		AText	text;
		SPI_GetRemoteFileURL(text);
		text.GetFilename(outFilename);
	}
	else if( NVI_IsFileSpecified() == false )
	{
		SPI_GetTemporaryTitle(outFilename);
	}
	else
	{
		AFileAcc	file;
		NVI_GetFile(file);
		file.GetFilename(outFilename);
	}
}

//TextEncoding�擾
ATextEncoding	ADocument_Text::SPI_GetTextEncoding() const
{
	//#764 return mTextEncoding;
	AText	tecname;
	GetDocPrefDBConst().GetData_Text(ADocPrefDB::kTextEncodingNameCocoa,tecname);//#1040
	return ATextEncodingWrapper::GetTextEncodingFromName(tecname);
}

//TextEncoding���擾
void	ADocument_Text::SPI_GetTextEncoding( AText& outTextEncodingName ) const
{
	GetDocPrefDBConst().GetData_Text(ADocPrefDB::kTextEncodingNameCocoa,outTextEncodingName);//#1040
}

/*#934
//TextEncoding�ݒ�iATextEncoding����ݒ�j
void	ADocument_Text::SPI_SetTextEncoding( const ATextEncoding inTextEncoding )
{
	AText	text;
	ATextEncodingWrapper::GetTextEncodingName(inTextEncoding,text);
	SPI_SetTextEncoding(text);
}
*/

//TextEncoding�ݒ�iTextEncoding������ݒ�j
void	ADocument_Text::SPI_SetTextEncoding( const AText& inTextEncodingName )
{
	//ReadOnly�̏ꍇ�͉����������^�[��
	if( NVI_IsReadOnly() == true )   return;
	//DB�ɐݒ�
	//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,inTextEncodingName);
	GetDocPrefDB().SetTextEncoding(inTextEncodingName);//#1040
	/*#764
	//mTextEncoding�ɐݒ�
	if( ATextEncodingWrapper::GetTextEncodingFromName(inTextEncodingName,mTextEncoding) == false )
	{
		//TextEncoding�̃`�F�b�N�͂��̊֐����R�[������O�ɍs��Ȃ���΂Ȃ�Ȃ��B
		_ACError("text encoding not found",this);
	}
	*/
	//�E�C���h�E�̃c�[���o�[���X�V
	/*#379
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateToolbarItemValue(GetUniqID());
	}
	*/
	GetApp().SPI_UpdateToolbarItemValue(GetObjectID());//#379
	//SetDirty
	NVI_SetDirty(true);
}

//TextEncoding�C��
void	ADocument_Text::SPI_ModifyTextEncoding( const AText& inTextEncodingName, ATextEncodingFallbackType& outResultFallbackType ) //#473
{
	outResultFallbackType = kTextEncodingFallbackType_None;//#473
	
	//Dirty�̏ꍇ�̓G���[
	if( NVI_IsDirty() == true )   { _ACError("",this); return; }
	
	//�e�L�X�g�G���R�[�f�B���O�ݒ�
	//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,inTextEncodingName);
	SPI_SetTextEncoding(inTextEncodingName);//#1040
	//#764 ATextEncodingWrapper::GetTextEncodingFromName(inTextEncodingName,mTextEncoding);
	
	//��r���폜
	ClearDiffDisplay();
	
	//�s���Aidentifier���N���A
	//#695 AArray<AUniqID>	deletedIdentifier;
	mTextInfo.DeleteLineInfoAll(/*#695 deletedIdentifier*/);
	//#695 DeleteFromJumpMenu(deletedIdentifier);
	//�W�����v���ڑS�폜
	DeleteAllJumpItems();
	//�C���|�[�g�t�@�C���f�[�^����
	//#349 UpdateImportFileData();
	SPI_DeleteAllImportFileData();//#349
	
	//#81
	//�Ō�Ƀ��[�h�^�Z�[�u�������̃e�L�X�g�G���R�[�f�B���O�i���t�@�C���̌��݂̃e�L�X�g�G���R�[�f�B���O�j���L���������o�b�N�A�b�v���ɕۑ�
	SPI_GetTextEncoding(mTextEncodingWhenLastLoadOrSave);
	
	//�e�L�X�g�ēǍ�
	{
		AStMutexLocker	locker(mTextMutex);//#890
		mText.DeleteAll();
		if( SPI_IsRemoteFileMode() == true )
		{
			mText.SetText(mTextOnRemoteServer);
		}
		else
		{
			if( NVI_IsFileSpecified() == true )
			{
				AFileAcc	file;
				NVI_GetFile(file);
				file.ReadTo(mText);
			}
		}
	}
	
	//�e�L�X�g������`���֕ϊ�
	if( mText.GetItemCount() > 0 )
	{
		//UTF8�֕ϊ�
		ATextEncoding	tec = SPI_GetTextEncoding();
		ATextEncoding	resultTec = tec;
		AReturnCode	resultReturnCode = static_cast<AReturnCode>(GetDocPrefDB().GetData_Number(ADocPrefDB::kReturnCode));//#307
		ABool	convertSuccessful = true;
		ABool	notMixed = true;//#995
		{
			AStMutexLocker	locker(mTextMutex);//#890
			//�w��tec�ł̕ϊ��Ȃ̂ŁA�t�H�[���o�b�N�ASJIS���X���X�t�H�[���o�b�N�͗����Ƃ��L��ɂ���B
			//�Ԃ�lconvertSuccessful�̓t�H�[���o�b�N���g���Ăł��ϊ����ł����Ƃ���true�ɂȂ�B
			convertSuccessful = GetApp().
					SPI_ConvertToUTF8CR(mText,tec,true,
										true,//�t�H�[���o�b�N
										false,//#844 SJIS���X���X�t�H�[���o�b�N��dropGetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kUseSJISLosslessFallback),
										resultReturnCode,resultTec,resultReturnCode,outResultFallbackType,notMixed);//#995
			//5c��a5�ɕϊ��i�o�b�N�X���b�V����\�ŕ\������j
			//#940 Convert5CToA5WhenOpenJIS(mText);
		}
	}
	
	/*#699
	//�s���v�Z
	AText	fontname;//win
	SPI_GetFontName(fontname);//win
	mTextInfo.CalcLineInfoAll(mText,true,kIndex_Invalid,fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),//#699
				SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),
				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters));
	//�W�����v���ڑS�폜
	DeleteAllJumpItems();
	//
	SPI_InitLineImageInfo();//#450
	//�܂肽���݃f�[�^��Textview�ɔ��f #450
	ApplyDocPref_CollapsedLine();
	//�s�`�F�b�N�f�[�^�𔽉f #842
	ApplyDocPref_LineCheckedDate();
	//���@���
	StartSyntaxRecognizer(0);
	*/
	
	//�h�L�������g��ԑJ�ځB�s�܂�Ԃ�����Čv�Z����
	//�i�s�܂�Ԃ��v�Z��ԂɑJ�ڂ��A�X���b�h�ōs�v�Z���s���B�j
	ReTransitInitState_Recalculate();
	
	//�E�C���h�E�̃c�[���o�[���X�V
	/*#379
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateToolbarItemValue(GetObjectID());
	}
	*/
	GetApp().SPI_UpdateToolbarItemValue(GetObjectID());//#379
	//
	//#212 RefreshWindow();
	SPI_RefreshTextViews();//#212
	
	//#378
	SPI_UpdateJumpList();
	
	//#413
	SPI_CorrectCaretIfInvalid();
	
	//#1040
	NVI_SetDirty(false);
}

//�s���Čv�Z
void	ADocument_Text::SPI_DoWrapRelatedDataChanged( const ABool inForceRecalc )
{
	//R006 �s��񂪕ς��̂ŕύX�ӏ�������������
	//#379 Diff�f�[�^�͒i���P�ʂȂ̂ŕω����Ȃ� ClearDiffDisplay();
	ClearArrowToDef();//RC1
	
	//selection�C���̂���
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_EditPreProcess(0,0);
	}
	//���[�h�f�[�^�ύX��ɌĂ΂�邱�Ƃ������̂ŁA������regexp�̍č\���Ȃǂ���Ă��܂�
	mTextInfo.SetMode(mModeIndex);
	
	if( SPI_GetWrapMode() == kWrapMode_WordWrap || SPI_GetWrapMode() == kWrapMode_LetterWrap || //#1113
		inForceRecalc == true )
	{
		/*#699
		//�s���Čv�Z
		AText	fontname;//win
		SPI_GetFontName(fontname);//win
		mTextInfo.CalcLineInfoAll(mText,true,kIndex_Invalid,fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),//#699
					SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),
					GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters));
		//�W�����v���ڑS�폜
		DeleteAllJumpItems();
		//�C���|�[�g�t�@�C���f�[�^����
		//#349 UpdateImportFileData();
		SPI_DeleteAllImportFileData();//#349
		//���@���
		StartSyntaxRecognizer(0);
		*/
		//
		
		//�h�L�������g��ԑJ�ځB�s�܂�Ԃ�����Čv�Z����
		//�i�s�܂�Ԃ��v�Z��ԂɑJ�ڂ��A�X���b�h�ōs�v�Z���s���B�j
		ReTransitInitState_Recalculate();
	}
	
	//#450 �t�H���g�T�C�Y�ύX�����ōs�������X�V����
	SPI_InitLineImageInfo();
	
	//selection�C���̂���
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_EditPostProcess(0,0,false);
	}
	
	//#218 diff���X�V
	//#379 LineInfo�X�V���Ă�Diff�f�[�^�͍X�V�s�v�Ȃ��߃R�����g�A�E�g�iDiff���͒i���P�ʁj SPI_UpdateDiffDisplayData();
}

/**
�t�H���g�ݒ�
@note �t�H���g��ݒ肵�A���[�h�t�H���g�͎g�p���Ȃ��ݒ�ɂ���
*/
void	ADocument_Text::SPI_SetFontName( const AText& inText )
{
	AFloatNumber	fontsize = GetDocPrefDB().GetData_FloatNumber(ADocPrefDB::kTextFontSize);
	SPI_SetFontNameAndSize(inText,fontsize);
}

/**
�t�H���g���̎擾
@note ���[�h�t�H���g���g�p����ݒ�̏ꍇ�́A���[�h�t�H���g���擾
*/
void	ADocument_Text::SPI_GetFontName( AText& outText ) const
{
	if( GetDocPrefDBConst().GetData_Bool(ADocPrefDB::kUseModeFont) == true )
	{
		//���[�h�̃t�H���g���g���ꍇ
		GetApp().SPI_GetModePrefDB(mModeIndex).GetModeData_Text(AModePrefDB::kDefaultFontName,outText);
	}
	else
	{
		//�h�L�������g�̃t�H���g���g���ꍇ
		GetDocPrefDBConst().GetData_Text(ADocPrefDB::kTextFontName,outText);
	}
}

/**
�t�H���g�T�C�Y�̐ݒ�
@note �t�H���g�T�C�Y��ݒ肵�A���[�h�t�H���g�͎g�p���Ȃ��ݒ�ɂ���
*/
void	ADocument_Text::SPI_SetFontSize( const AFloatNumber inFontSize )
{
	AText	fontname;
	GetDocPrefDB().GetData_Text(ADocPrefDB::kTextFontName,fontname);
	SPI_SetFontNameAndSize(fontname,inFontSize);
}

/**
�t�H���g�T�C�Y�̎擾
@note ���[�h�t�H���g���g�p����ݒ�̏ꍇ�́A���[�h�t�H���g�T�C�Y���擾
*/
AFloatNumber	ADocument_Text::SPI_GetFontSize() const
{
	//#966
	//�ꎞ�t�H���g�T�C�Y���ݒ肳��Ă����炻���Ԃ�
	if( mTemporaryFontSize > 0 )
	{
		return mTemporaryFontSize;
	}
	
	//
	if( GetDocPrefDBConst().GetData_Bool(ADocPrefDB::kUseModeFont) == true )
	{
		//���[�h�̃t�H���g���g���ꍇ
		return GetApp().SPI_GetModePrefDB(mModeIndex).GetModeData_FloatNumber(AModePrefDB::kDefaultFontSize);
	}
	else
	{
		//�h�L�������g�̃t�H���g���g���ꍇ
		return GetDocPrefDBConst().GetData_FloatNumber(ADocPrefDB::kTextFontSize);
	}
}

//#354
/**
�t�H���g�E�t�H���g�T�C�Y�̐ݒ�
@note �t�H���g�E�t�H���g�T�C�Y��ݒ肵�A���[�h�t�H���g�͎g�p���Ȃ��ݒ�ɂ���
*/
void	ADocument_Text::SPI_SetFontNameAndSize( const AText& inFontName, const AFloatNumber inFontSize )
{
	//�ꎞ�t�H���g�T�C�Y�N���A #966
	mTemporaryFontSize = 0;
	
	//==================�t�H���g���E�T�C�Y�␳==================
	//#521 �g�p�o���Ȃ��t�H���g�̏ꍇ��default�t�H���g�ɕ␳����
	AText	fontname;
	fontname.SetText(inFontName);
	if( AFontWrapper::IsFontEnabled(fontname) == false )
	{
		AFontWrapper::GetAppDefaultFontName(fontname);//#868
	}
	
	//�t�H���g�T�C�Y������������ꍇ�␳
	AFloatNumber	fontsize = inFontSize;
	if( fontsize < 6.0 )   fontsize = 6.0;//B0398 �ŏ��l�ύX
	
	//#868
	//�ύX�����̏ꍇ�A�����������^�[��
	AText	origfontname;
	GetDocPrefDB().GetData_Text(ADocPrefDB::kTextFontName,origfontname);
	AFloatNumber	origfontsize = GetDocPrefDB().GetData_FloatNumber(ADocPrefDB::kTextFontSize);
	ABool	origUseModeFont = GetDocPrefDB().GetData_Bool(ADocPrefDB::kUseModeFont);
	if( fontname.Compare(origfontname) == true && fontsize == origfontsize && origUseModeFont == false )
	{
		return;
	}
	
	//DB�ݒ�
	GetDocPrefDB().SetData_Text(ADocPrefDB::kTextFontName,fontname);
	GetDocPrefDB().SetData_FloatNumber(ADocPrefDB::kTextFontSize,fontsize);
	
	//���[�h�̃t�H���g�͎g�p���Ȃ��悤�ɐݒ�
	GetDocPrefDB().SetData_Bool(ADocPrefDB::kUseModeFont,false);
	
	//�t�H���g�ύX���h�L�������g�E�r���[�f�[�^�ɔ��f
	UpdateDocumentByFontChange();
}

//#354
/**
�t�H���g�E�T�C�Y��Revert����
*/
void	ADocument_Text::SPI_RevertFontNameAndSize()
{
	SPI_SetFontNameAndSize(mOriginalFontName,mOriginalFontSize);
}

//#868
/**
���[�h�̃t�H���g���g�����ǂ����̐ݒ���s��
*/
void	ADocument_Text::SPI_SetUseModeFont( const ABool inUseModeFont )
{
	//�ύX�����̏ꍇ�A���������I��
	if( GetDocPrefDB().GetData_Bool(ADocPrefDB::kUseModeFont) == inUseModeFont )
	{
		return;
	}
	//���[�h�̃t�H���g���g�����ǂ�����ݒ�
	GetDocPrefDB().SetData_Bool(ADocPrefDB::kUseModeFont,inUseModeFont);
	//�t�H���g�ύX���h�L�������g�E�r���[�f�[�^�ɔ��f
	UpdateDocumentByFontChange();
}

//#868
/**
���[�h�̃t�H���g���g�����ǂ������擾
*/
ABool	ADocument_Text::SPI_GetUseModeFont() const
{
	return GetDocPrefDBConst().GetData_Bool(ADocPrefDB::kUseModeFont);
}

//#966
/**
�ꎞ�t�H���g�T�C�Y�ݒ�
*/
void	ADocument_Text::SPI_SetTemporaryFontSize( const AFloatNumber inFontSize )
{
	//�ꎞ�t�H���g�T�C�Y�ݒ�
	mTemporaryFontSize = inFontSize;
	//�t�H���g�ύX���h�L�������g�E�r���[�f�[�^�ɔ��f
	UpdateDocumentByFontChange();
}

//#868
/**
���[�h�ݒ�̃e�L�X�g�t�H���g���ύX���ꂽ�Ƃ��̃h�L�������g�ւ̔��f����
*/
void	ADocument_Text::SPI_TextFontSetInModeIsUpdated( const AIndex inUpdatedModeIndex )
{
	//���[�h�̃t�H���g���g�p����ݒ�ł͂Ȃ��ꍇ�́A���������I��
	if( SPI_GetUseModeFont() == false )
	{
		return;
	}
	
	//���̃h�L�������g�ɔ��f���邩�ǂ����̃t���O
	ABool	related = false;
	if( inUpdatedModeIndex == SPI_GetModeIndex() )
	{
		//���[�h�������Ȃ甽�f�t���OON
		related = true;
	}
	if( inUpdatedModeIndex == kStandardModeIndex )
	{
		if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSameAsNormalMode_Font) == true )
		{
			//�ύX���ꂽ���[�h���W�����[�h�ŁA���A���̃h�L�������g�̃��[�h���u�W�����[�h�Ɠ����v�ݒ�Ȃ�A���f�t���OON
			related = true;
		}
	}
	//�t�H���g�ύX���h�L�������g�E�r���[�f�[�^�ɔ��f
	if( related == true )
	{
		UpdateDocumentByFontChange();
	}
}

//#868
/**
�t�H���g�ύX���h�L�������g�E�r���[�f�[�^�ɔ��f
*/
void	ADocument_Text::UpdateDocumentByFontChange()
{
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial ||
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		return;
	}
	
	//�s���Čv�Z
	SPI_DoWrapRelatedDataChanged(false);
	
	//�E�C���h�E�A����сAPrintImp�̕`��v���p�e�B�X�V
	GetApp().SPI_UpdateViewBounds(GetObjectID());//�s�ԍ��̕��X�V�A�X�N���[���o�[�̕\���X�V�Ȃ�
	GetApp().SPI_UpdateTextDrawProperty(GetObjectID());//#379
	
	//�e�r���[��line image info���X�V
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateLineImageInfoAllByCollapsedParameter();
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_RefreshTextView();
	}
}

//���s�R�[�h�̐ݒ�
void	ADocument_Text::SPI_SetReturnCode( const AReturnCode inReturnCode )
{
	//ReadOnly�̏ꍇ�͉����������^�[��
	if( NVI_IsReadOnly() == true )   return;
	//DB�ݒ�
	GetDocPrefDB().SetData_Number(ADocPrefDB::kReturnCode,inReturnCode);
	//�E�C���h�E�̃c�[���o�[���X�V
	/*#379
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateToolbarItemValue(GetObjectID());
	}
	*/
	GetApp().SPI_UpdateToolbarItemValue(GetObjectID());//#379
	//SetDirty
	NVI_SetDirty(true);
}

//���s�R�[�h�̎擾
AReturnCode	ADocument_Text::SPI_GetReturnCode() const
{
	return static_cast<AReturnCode>(GetDocPrefDBConst().GetData_Number(ADocPrefDB::kReturnCode));
}

//�s���胂�[�h�ݒ�
void	ADocument_Text::SPI_SetWrapMode( const AWrapMode inWrapMode, const ANumber inLetterCount )
{
	//�ݒ肪�ω����Ȃ��ꍇ�̓��^�[��
	if( inWrapMode == kWrapMode_WordWrapByLetterCount || inWrapMode == kWrapMode_LetterWrapByLetterCount )//#1113
	{
		if( static_cast<AWrapMode>(GetDocPrefDBConst().GetData_Number(ADocPrefDB::kWrapMode)) == inWrapMode && 
		GetDocPrefDBConst().GetData_Number(ADocPrefDB::kWrapMode_LetterCount) == inLetterCount )   return;
	}
	else
	{
		if( static_cast<AWrapMode>(GetDocPrefDBConst().GetData_Number(ADocPrefDB::kWrapMode)) == inWrapMode )   return;
	}
	//DB�ݒ�
	GetDocPrefDB().SetData_Number(ADocPrefDB::kWrapMode,inWrapMode);
	if( inWrapMode == kWrapMode_WordWrapByLetterCount || inWrapMode == kWrapMode_LetterWrapByLetterCount )//#1113
	{
		GetDocPrefDB().SetData_Number(ADocPrefDB::kWrapMode_LetterCount,inLetterCount);
	}
	
	//�֘A����E�C���h�E��LineInfo���ύX����邱�Ƃ�ʒm
	/*#199
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_DoLineInfoChangeStart(GetObjectID());
	}
	*/
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_DoLineInfoChangeStart();
	}
	
	//�s���Čv�Z
	SPI_DoWrapRelatedDataChanged(true);
	
	//�֘A����E�C���h�E�֒ʒm
	/*#379
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		//#199 GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_DoLineInfoChangeEnd(GetObjectID());
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateToolbarItemValue(GetObjectID());
	}
	*/
	GetApp().SPI_UpdateToolbarItemValue(GetObjectID());//#379
	//#199
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_DoLineInfoChangeEnd();
	}
	
	//���X�N���[���o�[�\���L���X�V #1068
	GetApp().SPI_UpdateViewBounds(GetObjectID());
	
	//�E�C���h�E�A����сAPrintImp�̕`��v���p�e�B�X�V
	//#379 UpdateTextDrawProperty();
	GetApp().SPI_UpdateTextDrawProperty(GetObjectID());//#379
	
	/*#1192 �s�܂�Ԃ��̂ݕύX�̏ꍇ��dirty�ɂ��Ȃ��B�iExecuteClose()����SPI_SaveDocPrefIfLoaded()���R�[�����Ă���̂�doc pref�͕ۑ������j
	//SetDirty
	if( NVI_IsReadOnly() == false )
	{
		NVI_SetDirty(true);
	}
	*/
}

//�s���胂�[�h�擾
AWrapMode	ADocument_Text::SPI_GetWrapMode() const
{
	return static_cast<AWrapMode>(GetDocPrefDBConst().GetData_Number(ADocPrefDB::kWrapMode));
}

//���������[�h���b�v�̕������擾
ANumber	ADocument_Text::SPI_GetWrapLetterCount() const
{
	return GetDocPrefDBConst().GetData_Number(ADocPrefDB::kWrapMode_LetterCount);
}

/*#844
//�^�u���ݒ�
void	ADocument_Text::SPI_SetTabWidth( const ANumber inTabWidth )
{
	//DB�ݒ�
	GetDocPrefDB().SetData_Number(ADocPrefDB::kTabWidth,inTabWidth);
	
	//�s���Čv�Z
	SPI_DoWrapRelatedDataChanged(false);
	
	//�E�C���h�E�A����сAPrintImp�̕`��v���p�e�B�X�V
	//#379 UpdateTextDrawProperty();
	GetApp().SPI_UpdateTextDrawProperty(GetObjectID());//#379
	
	//SetDirty
	if( NVI_IsReadOnly() == false )
	{
		NVI_SetDirty(true);
	}
}
*/

//�^�u���擾
ANumber	ADocument_Text::SPI_GetTabWidth() const
{
	//#868 return GetDocPrefDBConst().GetData_Number(ADocPrefDB::kTabWidth);
	return GetApp().SPI_GetModePrefDB(mModeIndex).GetModeData_Number(AModePrefDB::kDefaultTabWidth);//#868
}

AItemCount	ADocument_Text::SPI_GetIndentWidth() const
{
	//#868 return GetDocPrefDBConst().GetData_Number(ADocPrefDB::kIndentWidth);
	//#912
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == true )
	{
		//�C���f���g�����F�X�y�[�X�̏ꍇ�́A�C���f���g���ݒ�̒l��Ԃ�
		return GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Number(AModePrefDB::kIndentWidth);//#868
	}
	else
	{
		//�C���f���g�����F�^�u�̏ꍇ�́A��Ƀ^�u�\�����̒l��Ԃ��B
		//�i�^�u�̏ꍇ�ɁA�C���f���g����ݒ肷��Ƃ����̂��A������Â炢���߁B�C���f���g���͂����܂ŃX�y�[�X�̂Ƃ��̃X�y�[�X������̐ݒ�ɂ���B�j#912
		return GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Number(AModePrefDB::kDefaultTabWidth);
	}
}

/*#844
void	ADocument_Text::SPI_SetIndentWidth( const AItemCount inIndentWidth )
{
	return GetDocPrefDB().SetData_Number(ADocPrefDB::kIndentWidth,inIndentWidth);
}

void	ADocument_Text::SPI_SetAntiAliasMode( const ANumber inMode )
{
	//DB�ݒ�
	GetDocPrefDB().SetData_Number(ADocPrefDB::kAntiAliasMode,inMode);
	
	//�s���Čv�Z
	SPI_DoWrapRelatedDataChanged(false);
	
	//�E�C���h�E�A����сAPrintImp�̕`��v���p�e�B�X�V
	//#379 UpdateTextDrawProperty();
	GetApp().SPI_UpdateTextDrawProperty(GetObjectID());//#379
	
	//SetDirty
	if( NVI_IsReadOnly() == false )
	{
		NVI_SetDirty(true);
	}
}

AAntiAliasMode	ADocument_Text::SPI_GetAntiAliasMode() const
{
	return static_cast<AAntiAliasMode>(GetDocPrefDBConst().GetData_Number(ADocPrefDB::kAntiAliasMode));
}
*/

/**
�A���`�G�C���A�X�g�p���邩�ǂ����̔���
*/
ABool	ADocument_Text::SPI_IsAntiAlias() const
{
	/*#844
	switch(SPI_GetAntiAliasMode())
	{
	  case kAntiAliasMode_Default:
		{
			return GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kDefaultEnableAntiAlias);
		}
	  case kAntiAliasMode_On:
		{
			return true;
		}
	  case kAntiAliasMode_Off:
		{
			return false;
		}
	  default:
		{
			return true;
		}
	}
	*/
	return GetApp().NVI_GetAppPrefDBConst().GetData_Bool(AAppPrefDB::kEnableAntiAlias);//#844
}

/**
���[�h�ݒ�i���[�h�����烂�[�h�ݒ�j
*/
void	ADocument_Text::SPI_SetModeByRawName( const AText& inModeName )
{
	AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(inModeName);
	if( modeIndex != kIndex_Invalid )
	{
		SPI_SetMode(modeIndex);
	}
}

/**
���[�h�ݒ�i���[�h�\�������烂�[�h�ݒ�j
*/
void	ADocument_Text::SPI_SetModeByDisplayName( const AText& inModeName )
{
	AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeDisplayName(inModeName);
	if( modeIndex != kIndex_Invalid )
	{
		SPI_SetMode(modeIndex);
	}
}

/**
���[�h�ݒ�
*/
void	ADocument_Text::SPI_SetMode( const AModeIndex inModeIndex )
{
	//���[�h��disable���Ȃ牽�����Ȃ�
	if( GetApp().SPI_GetModePrefDB(inModeIndex,false).IsModeEnabled() == false )
	{
		return;
	}
	
	//�s���N���A��AReTransitInitState_Recalculate()���ŃL�����b�g������(SPI_InitSelect())����O�ɁA
	//���[�h�̃��[�h�������ŁAtextview�̑I���֘A���������삵�Ă��܂�Ȃ��悤�ɁA���[�h�̃��[�h�������ōs���Ă����B
	GetApp().SPI_GetModePrefDB(inModeIndex);
	
	//��r���폜
	ClearDiffDisplay();
	
	//B0071 identifier���N���A���Ă��烂�[�h�؂�ւ�����
	//�s���Aidentifier���N���A
	//#695 AArray<AUniqID>	deletedIdentifier;
	mTextInfo.DeleteLineInfoAll(/*#695 deletedIdentifier*/);
	//#695 DeleteFromJumpMenu(deletedIdentifier);
	//�W�����v���ڑS�폜
	DeleteAllJumpItems();
	/*
	//
	{
		AStMutexLocker	locker(mCurrentLocalIdentifierMutex);//#717
		mCurrentLocalIdentifier.DeleteAll();
	}
	mCurrentLocalIdentifierStartLineIndex = kIndex_Invalid;
	mCurrentLocalIdentifierEndLineIndex = kIndex_Invalid;
	*/
	//#699
	//���[�J�����ʎq�f�[�^�S�폜
	SPI_DeleteAllLocalIdentifiers();
	
	//�C���|�[�g�t�@�C���f�[�^����
	//#349 UpdateImportFileData();
	SPI_DeleteAllImportFileData();//#349
	
	//���[�h�ݒ�
	AModeIndex	modeIndex = inModeIndex;
	if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
	SetModeIndex(modeIndex);//#890
	
	//���[�h����doc pref�ɐݒ�
	AText	text;
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeRawName(text);
	GetDocPrefDB().SetData_Text(ADocPrefDB::kModeName,text);
	
	//text info�Ƀ��[�h�ݒ�
	mTextInfo.SetMode(mModeIndex);
	
	/*#699
	//�s���v�Z
	AText	fontname;//win
	SPI_GetFontName(fontname);//win
	//#695 deletedIdentifier.DeleteAll();//#571
	mTextInfo.CalcLineInfoAll(mText,true,kIndex_Invalid,fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),//#699
				SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),//#695 deletedIdentifier,
				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters));
	//#695 DeleteFromJumpMenu(deletedIdentifier);//#571
	//�W�����v���ڑS�폜
	DeleteAllJumpItems();
	//
	SPI_InitLineImageInfo();//#450
	//�܂肽���݃f�[�^��Textview�ɔ��f #450
	ApplyDocPref_CollapsedLine();
	//�s�`�F�b�N�f�[�^�𔽉f #842
	ApplyDocPref_LineCheckedDate();
	//���@���
	StartSyntaxRecognizer(0);
	*/
	
	//�h�L�������g��ԑJ�ځB�s�܂�Ԃ�����Čv�Z����
	//�i�s�܂�Ԃ��v�Z��ԂɑJ�ڂ��A�X���b�h�ōs�v�Z���s���B�j
	ReTransitInitState_Recalculate();
	
	//�E�C���h�E�A����сAPrintImp�̕`��v���p�e�B�X�V
	//#379 UpdateTextDrawProperty();
	GetApp().SPI_UpdateTextDrawProperty(GetObjectID());//#379
	
	/*#379
	//B0341
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateViewBounds();
	}
	*/
	GetApp().SPI_UpdateViewBounds(GetObjectID());//#379
	
	//#378
	SPI_UpdateJumpList();
	
	//�c�[���o�[�l�X�V
	GetApp().SPI_UpdateToolbarItemValue(GetObjectID());//#379
	
	//���[�h���j���[�̃V���[�g�J�b�g�i���݂̃h�L�������g�̃��[�h�j�ݒ�
	GetApp().SPI_UpdateModeMenuShortcut();
}

//#683
/**
TextInfo���Ŏ����Ă���RegExp�I�u�W�F�N�g���X�V����iRegExp�̐ݒ��ύX���ɃR�[���K�v�j
*/
void	ADocument_Text::SPI_UpdateTextInfoRegExp()
{
	//purge�̂ݍs���Bpurge����΁A����Atext info���Ŏg�p���悤�Ƃ���Ƃ��ɁAupdate�����B
	
	//text info��reg exp��purge����
	mTextInfo.PurgeRegExpObject();
	//���@�F���X���b�h����text info��reg exp��purge����
	GetSyntaxRecognizerThread().SPI_PurgeTextinfoRegExp();
}

/**
���[�h���i���[�h�t�@�C�����j�擾
*/
void	ADocument_Text::SPI_GetModeRawName( AText& outText ) const
{
	//#914 GetDocPrefDBConst().GetData_Text(ADocPrefDB::kModeName,outText);
	AStMutexLocker	locker(mModeIndexMutex);
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeRawName(outText);
}

//#914
/**
���[�h�\�����擾
*/
void	ADocument_Text::SPI_GetModeDisplayName( AText& outText ) const
{
	AStMutexLocker	locker(mModeIndexMutex);
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeDisplayName(outText);
}

ABool	ADocument_Text::NVIDO_IsSpecified() const
{
	if( SPI_IsRemoteFileMode() == true )
	{
		return true;
	}
	else
	{
		return true;//file.IsSpecified();
	}
}

/**
�h�L�������g�^�C�v�������[�g�t�@�C�����ǂ������擾
*/
ABool ADocument_Text::SPI_IsRemoteFileMode() const
{
	//#361 return mFTPMode;
	return (mTextDocumentType==kTextDocumentType_RemoteFile);//#361
}

ANumber	ADocument_Text::SPI_GetLineMargin() const
{
	ANumber	lineMargin = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Number(AModePrefDB::kLineMargin);
	return lineMargin;
}

ABool	ADocument_Text::SPI_GetParentFolder( AFileAcc& outFolder ) const
{
	AFileAcc	file;
	if( NVI_GetFile(file) == true )
	{
		outFolder.SpecifyParent(file);
		return true;
	}
	else
	{
		return false;
	}
}

void	ADocument_Text::SPI_GetRemoteFileURL( AText& outRemoteFileURL ) const
{
	outRemoteFileURL.SetText(mRemoteFileURL);
}


void	ADocument_Text::NVIDO_SpecifyFile( const AFileAcc& inFileAcc )
{
	//FTP���[�h����SaveAs�Œʏ�t�@�C���ۑ������ꍇ
	if( SPI_IsRemoteFileMode() == true )
	{
		//#361 mFTPMode = false;
		mTextDocumentType = kTextDocumentType_Normal;//#361
		//��
		//�����[�g�ڑ��I�u�W�F�N�g����H
	}
	
	mFileWriteLocker.Unlock();
	mFileWriteLocker.Specify(inFileAcc);
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kLockWhileEdit) == true && NVI_IsReadOnly() == false )
	{
		mFileWriteLocker.Lock();
	}
	
	//mCVSProperty.Specify(mFileAcc);//R0006
	/*#379
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).NVI_UpdateTitleBar();
	}
	*/
	GetApp().NVI_UpdateTitleBar();//#379
	
	//ProjectFolder�ݒ�
	GetApp().SPI_FindProjectFolder(inFileAcc,mProjectFolder,mProjectObjectID,mProjectRelativePath);//RC3
	//�v���W�F�N�g�t�H���_��o�^
	GetApp().SPI_RegisterProjectFolder(mProjectFolder);
	//
	//#922 GetApp().SPI_UpdateWindowMenu();
	//�t�@�C�����X�g�E�C���h�E�X�V
	//#725 GetApp().SPI_UpdateFileListWindow_DocumentOpened(GetObjectID());
	GetApp().SPI_UpdateFileListWindows(kFileListUpdateType_DocumentOpened,GetObjectID());//#725
	
	//#81
	//�����o�b�N�A�b�v�t�@�C�����X�g�X�V
	SPI_UpdateAutoBackupArray();
}

//RC3
//�h�L�������g�I�[�v�����Ƀv���W�F�N�g�ݒ肳�ꂽ�ꍇ�ɃR�[�������
void	ADocument_Text::SPI_UpdateProjectFolder()
{
	if( NVI_IsFileSpecified() == false )   return;
	if( SPI_IsRemoteFileMode() == true )   return;
	
	AFileAcc	file;
	NVI_GetFile(file);
	GetApp().SPI_FindProjectFolder(file,mProjectFolder,mProjectObjectID,mProjectRelativePath);
	//�v���W�F�N�g�t�H���_�o�^
	GetApp().SPI_RegisterProjectFolder(mProjectFolder);
}

void	ADocument_Text::SPI_GetFileAttribute( AFileAttribute& outFileAttribute ) const
{
#if IMPLEMENTATION_FOR_MACOSX
	outFileAttribute.creator = mCreator;
	outFileAttribute.type = mType;
#endif
}

void	ADocument_Text::SPI_CorrectTextPoint( ATextPoint& ioTextPoint ) const
{
	if( ioTextPoint.y >= SPI_GetLineCount() )
	{
		ioTextPoint.y = SPI_GetLineCount()-1;
	}
	if( ioTextPoint.x > SPI_GetLineLengthWithoutLineEnd(ioTextPoint.y) )
	{
		ioTextPoint.x = SPI_GetLineLengthWithoutLineEnd(ioTextPoint.y);
	}
	ATextIndex	textindex = SPI_GetTextIndexFromTextPoint(ioTextPoint);
	textindex = mText.GetCurrentCharPos(textindex);
	SPI_GetTextPointFromTextIndex(textindex,ioTextPoint);
}

//R0000
void	ADocument_Text::SPI_UpdateFileAttribute()
{
	//ReadMe�t�@�C���̏ꍇ�͉����������^�[���ireadonly���ێ����邽�߁j #1351
	if( mIsReadMeFile == true )   return;
	
	AFileAcc	file;
	if( NVI_GetFile(file) == true )
	{
		//�h�L�������g������ReadOnly�ł���A�t�@�C����ReadOnly�Ŗ����Ȃ����ꍇ�́A�h�L�������g��ReadOnly������
		if( NVI_IsReadOnly() == true )
		{
			if( file.IsReadOnly() == false )
			{
				NVI_SetReadOnly(false);
			}
		}
	}
}

//win
//�h�L�������g���V�K�h�L�������g�����ҏW�ł����true��Ԃ�
ABool	ADocument_Text::SPI_IsNewAndUneditted()
{
	if( NVI_IsFileSpecified() == true )   return false;
	if( SPI_IsRemoteFileMode() == true )   return false;
	if( NVI_IsDirty() == true )   return false;
	AText	text;
	if( mUndoer.GetUndoMenuText(text) == true )   return false;
	if( mUndoer.GetRedoMenuText(text) == true )   return false;
	return true;
}

//#850
/**
���I�[�v���p�̃t�@�C���p�X�e�L�X�g���擾
*/
void	ADocument_Text::SPI_GetReopenFilePath( AText& outPath, ABool& outIsRemoteFile ) const
{
	//���ʏ�����
	outPath.DeleteAll();
	outIsRemoteFile = false;
	switch( mTextDocumentType )
	{
		//���[�J���t�@�C���̏ꍇ�A�t�@�C���p�X���擾
	  case kTextDocumentType_Normal:
		{
			if( NVI_IsFileSpecified() == true )
			{
				NVI_GetFilePath(outPath);
			}
			break;
		}
		//�����[�g�t�@�C���̏ꍇ�A�����[�g�t�@�C����URL���擾
	  case kTextDocumentType_RemoteFile:
		{
			outPath.SetText(mRemoteFileURL);
			outIsRemoteFile = true;
			break;
		}
		//
	  case kTextDocumentType_DiffTarget:
		{
			//���������i�Ԃ�l�͋�ɂ���j
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

//#912
/**
�C���f���g�����̐���
*/
void	ADocument_Text::SPI_GuessIndentCharacterType()
{
	//==================�w�b�_�[��������ɂ�鐄��==================
	//�w�b�_�[��������ɂ�鐄���t���O �i#957 �r���������SPI_UpdateIndentCharacterTypeModePref()�����s���Ȃ��悤�ɂ���j
	ABool	indentGuessedByHeader = false;
	{
		//�r������
		AStMutexLocker	locker(mTextMutex);
		//"tab-width:"�e�L�X�g������΂���ɏ]��
		AText	firstLineText;
		AIndex	pos = 0;
		mText.GetLine(pos,firstLineText);
		pos = 0;
		if( firstLineText.FindCstring(0,"tab-width:",pos) == true )
		{
			pos += 10;
			mText.SkipTabSpace(pos,firstLineText.GetItemCount());
			//
			ANumber	tabwidth = 0;
			mText.ParseIntegerNumber(pos,tabwidth);
			switch(tabwidth)
			{
			  case 2:
				{
					mGuessedIndnetCharacterType = kIndentCharacterType_2Spaces;
					indentGuessedByHeader = true;
					break;
				}
			  case 4:
				{
					mGuessedIndnetCharacterType = kIndentCharacterType_4Spaces;
					indentGuessedByHeader = true;
					break;
				}
			  case 8:
				{
					mGuessedIndnetCharacterType = kIndentCharacterType_8Spaces;
					indentGuessedByHeader = true;
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
		}
	}
	//�w�b�_�[��������ɂ�鐄���t���OON�̂Ƃ��́A�ݒ���X�V���ďI�� #957
	if( indentGuessedByHeader == true )
	{
		SPI_UpdateIndentCharacterTypeModePref();
		return;
	}
	
	//==================�e�s�̃^�u�^�X�y�[�X�����琄��==================
	{
		//�r������i#957 �r���������SPI_UpdateIndentCharacterTypeModePref()�����s���Ȃ��悤�ɂ���j
		AStMutexLocker	locker(mTextMutex);
		//�|�C���^�擾
		AStTextPtr	textptr(mText,0,mText.GetItemCount());
		AUChar*	p = textptr.GetPtr();
		//�e�^�C�v���Ƃ̃J�E���g��������
		AItemCount	space2Count = 0;
		AItemCount	space4Count = 0;
		AItemCount	space6Count = 0;
		AItemCount	space8Count = 0;
		AItemCount	tabCount = 0;
		//�s���Ƃ̃��[�v�i�������A�ő��1000�s�܂Łj
		AItemCount  lineCount = SPI_GetLineCount();
		if( lineCount > 1000 )   lineCount = 1000;
		for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
		{
			//�i���J�n�s���ǂ������擾
			ABool	isParagraphStart = true;
			if( lineIndex > 0 )
			{
				isParagraphStart = mTextInfo.GetLineExistLineEnd(lineIndex);
			}
			//�i���J�n�s�̏ꍇ�A���A�s�����R�[�h���̏ꍇ�̂݁A��������
			if( isParagraphStart == true && mTextInfo.GetLineStartIsCode(lineIndex) == true )//#1003
			{
				//�s�����^�u���A�X�y�[�X���i�X�y�[�X�̏ꍇ�̓X�y�[�X�����j�𔻒�
				AIndex	lineStart = mTextInfo.GetLineStart(lineIndex);
				AItemCount	len = mTextInfo.GetLineLengthWithoutLineEnd(lineIndex);
				AItemCount	spaceCount = 0;
				ABool	tab = false;
				for( AIndex pos = 0; pos < len; pos++ )
				{
					AUChar	ch = p[lineStart+pos];
					if( ch == kUChar_Tab )
					{
						tab = true;
						break;
					}
					else if( ch == kUChar_Space )
					{
						spaceCount++;
					}
					else
					{
						break;
					}
				}
				//�e�^�C�v�̃J�E���^���C���N�������g
				if( tab == true )
				{
					tabCount++;
				}
				switch( spaceCount )
				{
				  case 2:
					{
						space2Count++;
						break;
					}
				  case 4:
					{
						space4Count++;
						break;
					}
				  case 6:
					{
						space6Count++;
						break;
					}
				  case 8:
					{
						space8Count++;
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
			}
		}
		//------------------�e�^�C�v�̃J�E���^���ɏ]���āA����------------------
		//�S�̂�1%��臒l�Ƃ��āA�^�u�A�X�y�[�X2�����A4�����A8�����A����s�̏��ɁA�ŏ���臒l�𒴂�����̂�I������
		AItemCount	threashold = 0;//#1003 tabCount+space2Count+space4Count+space6Count+space8Count;
		//#1003 threashold /= 100;
		//#1003 threashold += 10;//#1003 
		//#1003 ���ǂ������l��0�i���傫���j���x�^�[�B Python�ȂǂŃN���X���� def�s��1�s�����Ȃ��ꍇ�Ȃǂ́A�ŏ��C���f���g��1�s�����Ȃ����Ƃ����蓾��B
		//
		if( tabCount > threashold )
		{
			mGuessedIndnetCharacterType = kIndentCharacterType_Tab;
		}
		else if( space2Count > threashold )
		{
			mGuessedIndnetCharacterType = kIndentCharacterType_2Spaces;
		}
		else if( space4Count > threashold )
		{
			mGuessedIndnetCharacterType = kIndentCharacterType_4Spaces;
		}
		else if( space8Count > threashold )
		{
			mGuessedIndnetCharacterType = kIndentCharacterType_8Spaces;
		}
		else
		{
			mGuessedIndnetCharacterType = kIndentCharacterType_Unknown;
		}
	}
	//------------------���[�h�ݒ���X�V------------------
	SPI_UpdateIndentCharacterTypeModePref();
}

/**
�����C���f���g�����ݒ�
�h�L�������g�Ő�������Ă���C���f���g�����^�C�v�����[�h�ݒ�ɐݒ肷��
*/
ABool	ADocument_Text::SPI_UpdateIndentCharacterTypeModePref()
{
	//�����C���f���g�����ݒ肪OFF�Ȃ牽�����Ȃ��ŏI��
	if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kIndentAutoDetect) == false )
	{
		return false;
	}
	//�����C���f���g�������s���Ȃ牽�����Ȃ��ŏI��
	if( mGuessedIndnetCharacterType == kIndentCharacterType_Unknown )
	{
		return false;
	}
	
	//�����^�C�v�ɏ]���āA�C���f���g�����ƃC���f���g�����擾
	AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex());
	ANumber	indentWidth = SPI_GetTabWidth();
	ABool	spaceIndent = false;
	switch(mGuessedIndnetCharacterType)
	{
	  case kIndentCharacterType_Tab:
		{
			break;
		}
	  case kIndentCharacterType_2Spaces:
		{
			indentWidth = 2;
			spaceIndent = true;
			break;
		}
	  case kIndentCharacterType_4Spaces:
		{
			indentWidth = 4;
			spaceIndent = true;
			break;
		}
	  case kIndentCharacterType_8Spaces:
		{
			indentWidth = 8;
			spaceIndent = true;
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//�f�[�^�ݒ�
	ABool	changed = false;
	if( modePrefDB.GetData_Number(AModePrefDB::kIndentWidth) != indentWidth )
	{
		modePrefDB.SetData_Number(AModePrefDB::kIndentWidth,indentWidth);
		changed = true;
	}
	if( modePrefDB.GetData_Bool(AModePrefDB::kInputSpacesByTabKey) != spaceIndent )
	{
		modePrefDB.SetData_Bool(AModePrefDB::kInputSpacesByTabKey,spaceIndent);
		changed = true;
	}
	//�f�[�^�ɕύX���������ꍇ�́A���[�h�ݒ�E�C���h�E�ɔ��f���Anotification��\������
	if( changed == true )
	{
		//���[�h�ݒ�E�C���h�E�\�����Ȃ�A���[�h�ݒ�E�C���h�E�ɕ\�����f
		if( GetApp().SPI_GetModePrefWindow(SPI_GetModeIndex()).NVI_IsWindowVisible() == true )
		{
			GetApp().SPI_GetModePrefWindow(SPI_GetModeIndex()).NVI_UpdateProperty();
		}
		//���[�h��ʂ��J��URL�X�L�[���쐬 #1003
		AText	modeName;
		SPI_GetModeRawName(modeName);
		AText	modeprefURI;
		modeprefURI.SetCstring("net.mimikaki.mi://modepref/");
		modeprefURI.AddText(modeName);
		modeprefURI.AddCstring("/6");
		//notification�E�C���h�E���|�b�v�A�b�v�\��
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetPopupNotificationWindow().
				SPI_GetNotificationView().SPI_SetNotification_IndentCharacterChanged(SPI_GetModeIndex(),modeprefURI);
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_ShowNotificationPopupWindow(true);
	}
	return changed;
}

//#558
/**
�c�������[�h���ǂ������擾
*/
ABool	ADocument_Text::SPI_GetVerticalMode() const
{
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
	{
		return GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kVerticalMode);
	}
	else
	{
		//10.6�ȉ��͏c�������Ή�
		return false;
	}
}

//#858
#if 0
#pragma mark ===========================

#pragma mark ---�O���R�����g�i�tⳎ��j

//#138
/**
�O���R�����g�f�[�^�i�tⳎ��f�[�^�j�z����\��
*/
void	ADocument_Text::SPI_UpdateExternalCommentArray()
{
	//�܂��S�폜
	mExternalCommentArray_LineIndex.DeleteAll();
	mExternalCommentArray_Comment.DeleteAll();
	mExternalCommentArray_ModuleName.DeleteAll();
	mExternalCommentArray_Offset.DeleteAll();
	
	//�v���W�F�N�g���ݒ�Ȃ牽�������I��
	if( mProjectObjectID == kObjectID_Invalid )   return;
	
	//Project����A���̃h�L�������g�̑S�f�[�^���擾����
	ATextArray	moduleNameArray, commentArray;
	ANumberArray	offsetArray;
	GetApp().SPI_GetExternalCommentArrayFromProjectDB(mProjectObjectID,mProjectRelativePath,moduleNameArray,offsetArray,commentArray);
	//�擾�����f�[�^���Ƃɍs�ԍ��E�R�����g�̃��R�[�h�ɕϊ����āA�z����\������
	for( AIndex i = 0; i < moduleNameArray.GetItemCount(); i++ )
	{
		//���W���[�����E�I�t�Z�b�g����s�ԍ�����
		AIndex	lineIndex = FindLineIndexByModuleNameAndOffset(moduleNameArray.GetTextConst(i),offsetArray.Get(i));
		//�s�ԍ��␳
		if( lineIndex < 0 )   lineIndex = 0;
		if( lineIndex >= SPI_GetLineCount() )   lineIndex = SPI_GetLineCount()-1;
		//�\���s�ƃR�����g��z��ɒǉ�
		mExternalCommentArray_LineIndex.Add(lineIndex);
		mExternalCommentArray_Comment.Add(commentArray.GetTextConst(i));
		mExternalCommentArray_ModuleName.Add(moduleNameArray.GetTextConst(i));
		mExternalCommentArray_Offset.Add(offsetArray.Get(i));
	}
	
	//TextView�֔��f
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateFusen(true);
	}
	
	//�tⳎ����X�g�E�C���h�E�����ς݂Ȃ�e�[�u���X�V
	if( GetApp().SPI_GetFusenListWindow().NVI_IsWindowCreated() == true )
	{
		//View�ɑ΂��ă��X�g�X�V���w��
		GetApp().SPI_GetFusenListWindow().SPI_GetListView().SPI_UpdateTable(mProjectObjectID,mProjectRelativePath);
	}
}

//#138
/**
���W���[�����E�I�t�Z�b�g����s�ԍ��擾

@param inModuleName ���W���[����
@param inOffset �I�t�Z�b�g
@return �s�ԍ�
*/
AIndex	ADocument_Text::FindLineIndexByModuleNameAndOffset( const AText& inModuleName, const ANumber inOffset ) const
{
	//inModuleName�Ɉ�v���郁�j���[�e�L�X�g���������A���̍s�ԍ��𓾂�
	AIndex	lineIndex = mTextInfo.FindLineIndexByMenuText(inModuleName);
	if( lineIndex == kIndex_Invalid )
	{
		//���j���[�e�L�X�g�ň�v���Ȃ��ꍇ�́A�L�[���[�h�Ō�������
		lineIndex = mTextInfo.FindLineIndexByMenuKeywordText(inModuleName);
		if( lineIndex == kIndex_Invalid )
		{
			//������Ȃ���΍ŏ��̍s�ɂ��Ă���
			lineIndex = 0;
		}
	}
	//�s�ԍ��␳
	AIndex	result = lineIndex+inOffset;
	if( result < 0 )   result = 0;
	if( result >= SPI_GetLineCount() )   result = SPI_GetLineCount()-1;
	return result;
}

//#138
/**
�s�ԍ��ɑΉ�����O���R�����g��Index���擾

@param inLineIndex �s�ԍ�
@return �R�����gIndex
*/
AIndex	ADocument_Text::SPI_FindExternalCommentIndexByLineIndex( const AIndex inLineIndex ) const
{
	return mExternalCommentArray_LineIndex.Find(inLineIndex);
}

//#138
/**
�O���R�����gText�擾

@param inCommentIndex �R�����gIndex
@return �R�����gText
*/
const AText	ADocument_Text::SPI_GetExternalCommentText( const AIndex inCommentIndex ) const
{
	return mExternalCommentArray_Comment.GetTextConst(inCommentIndex);
}

//#138
/**
�O���R�����glineIndex�擾

@param inCommentIndex �R�����gIndex
@return �s�ԍ�
*/
AIndex	ADocument_Text::SPI_GetExternalCommentLineIndex( const AIndex inCommentIndex ) const
{
	return mExternalCommentArray_LineIndex.Get(inCommentIndex);
}

//#138
/**
�O���R�����gmoduleName�擾

@param inCommentIndex �R�����gIndex
@return ���W���[����
*/
const AText	ADocument_Text::SPI_GetExternalCommentModuleName( const AIndex inCommentIndex ) const
{
	return mExternalCommentArray_ModuleName.GetTextConst(inCommentIndex);
}

//#138
/**
�O���R�����glineIndex�擾

@param inCommentIndex �R�����gIndex
@return �I�t�Z�b�g
*/
AIndex	ADocument_Text::SPI_GetExternalCommentOffset( const AIndex inCommentIndex ) const
{
	return mExternalCommentArray_Offset.Get(inCommentIndex);
}

//#138
/**
�e�L�X�g�̒ǉ��E�폜���ɁA�O���R�����g�z��̍s�ԍ���Adjust����

@param inStarLineIndex �X�V�J�n�s�ԍ�
@param inDelta �ǉ��E�폜�s���i�폜�̓}�C�i�X�j
*/
void	ADocument_Text::AdjustExternalCommentArray( const AIndex inStartLineIndex, const AIndex inDelta )
{
	//�z����f�[�^�����ꂼ�꒲�ׂāAinStartLineIndex�ȍ~�̍s�̂��̂́AinDelta�������炷
	AItemCount	itemCount = mExternalCommentArray_LineIndex.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AIndex	lineIndex = mExternalCommentArray_LineIndex.Get(i);
		if( lineIndex >= inStartLineIndex )
		{
			mExternalCommentArray_LineIndex.Set(i,lineIndex+inDelta);
		}
	}
}

//�O���R�����g�擾 RC3
/*#138
void	ADocument_Text::SPI_GetExternalComment( const AText& inModuleName, const AIndex inOffset, AText& outComment ) const
{
	if( mProjectObjectID != kObjectID_Invalid )
	{
		GetApp().SPI_GetExternalCommentFromProjectDB(mProjectObjectID,mProjectRelativePath,inModuleName,inOffset,outComment);
	}
}
*/

//RC3 #138
/**
�s�ɑΉ����郂�W���[�����E�I�t�Z�b�g���擾

�Ή����郂�W���[���������݂��Ȃ��ꍇ�́AoutText�͋�AoutOffset�͍s�ԍ��ƂȂ�B
@param inLineIndex �s�ԍ�(in)
@param outText ���W���[����(out)
@param outOffset �I�t�Z�b�g(out)
*/
void	ADocument_Text::SPI_GetModuleNameAndOffsetByLineIndex( const AIndex inLineIndex, AText& outText, AIndex& outOffset ) const
{
	//���v�e�X�g
	
	//���W���[�����E�I�t�Z�b�g������
	outText.DeleteAll();
	outOffset = kIndex_Invalid;
	//�s�ԍ����烁�j���[�e�L�X�g�擾
	AIndex	identifierLineIndex = 0;
	//#695 AUniqID	UniqID = mTextInfo.GetMenuIdentifierByLineIndex(inLineIndex,identifierLineIndex);
	//#695 if( UniqID != kUniqID_Invalid )//#216
	AIndex	jumpIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);//#695
	if( jumpIndex != kIndex_Invalid )//#695
	{
		AUniqID	UniqID = mJump_IdentifierObjectArray.Get(jumpIndex);//#695
		identifierLineIndex = mTextInfo.GetLineIndexByGlobalIdentifier(UniqID);//#695
		outOffset = inLineIndex - identifierLineIndex;
		mTextInfo.GetGlobalIdentifierMenuText(UniqID,outText);
	}
	else
	{
		outOffset = inLineIndex;
	}
}

/**
���W���[�����E�I�t�Z�b�g�ɑΉ�����s�̍ŏ���I������
*/
void	ADocument_Text::SPI_SelectTextByModuleNameAndOffset( const AText& inModuleName, const ANumber inOffset )
{
	//���W���[�����E�I�t�Z�b�g����s�ԍ�������
	AIndex	lineIndex = FindLineIndexByModuleNameAndOffset(inModuleName,inOffset);
	//#199 if( mWindowIDArray.GetItemCount() == 0 )   return;
	if( mViewIDArray.GetItemCount() == 0 )   return;
	//�s�ԍ��␳
	if( lineIndex < 0 )   lineIndex = 0;
	if( lineIndex > SPI_GetLineCount() )   lineIndex = SPI_GetLineCount();
	//�e�L�X�g��I��
	//#199 GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_SelectText(GetObjectID(),SPI_GetLineStart(lineIndex),0);
	SPI_SelectText(SPI_GetLineStart(lineIndex),0);
}

/**
�O���R�����g�z��f�[�^�擾
*/
void	ADocument_Text::SPI_GetExternalCommentArray( ATextArray& outModuleNameArray, ANumberArray& outOffsetArray, ATextArray& outCommentArray, ANumberArray& outLineIndexArray ) const
{
	//������
	outModuleNameArray.DeleteAll();
	outOffsetArray.DeleteAll();
	outCommentArray.DeleteAll();
	outLineIndexArray.DeleteAll();
	//�i�[
	for( AIndex i = 0; i < mExternalCommentArray_ModuleName.GetItemCount(); i++ )
	{
		outModuleNameArray.Add(mExternalCommentArray_ModuleName.GetTextConst(i));
		outOffsetArray.Add(mExternalCommentArray_Offset.Get(i));
		outCommentArray.Add(mExternalCommentArray_Comment.GetTextConst(i));
		outLineIndexArray.Add(mExternalCommentArray_LineIndex.Get(i));
	}
}
#endif

#pragma mark ===========================

#pragma mark ---����

//����
ABool	ADocument_Text::SPI_Find( const AFindParameter& inParam, const ABool inBackward,//#789
								 const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos, 
								 const ABool inUseModalSession, ABool& outModalSessionAborted )
{
	ABool	abortflag = false;//B0359
	AFindParameter	param;
	param.Set(inParam);
	//JIS�n�e�L�X�g�G���R�[�f�B���O�̏ꍇ�́A��Ɂ��^�o�b�N�X���b�V�������ɂ���B #940
	if( SPI_IsJISTextEncoding() == true )
	{
		param.ignoreBackslashYen = true;
	}
	//�������s
	return mTextFinder.ExecuteFind(mText,param,inBackward,inSpos,inEpos,outSpos,outEpos,abortflag,inUseModalSession,outModalSessionAborted);
}

void	ADocument_Text::SPI_ChangeReplaceText( const AText& inReplaceText, AText& outText ) const
{
	mTextFinder.ChangeReplaceText(mText,inReplaceText,outText);
}

ABool	ADocument_Text::SPI_FindForHighlight( const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos ) 
{
	//�n�C���C�g������
	AIndex	epos = inEpos;
	if( inEpos-inSpos > kLimit_HighlightTargetTextLength )
	{
		epos = SPI_GetCurrentCharTextIndex(inSpos+kLimit_HighlightTargetTextLength);
	}
	//
	if( mFindHighlightSet == false )   return false;
	ABool	abortflag = false;//B0359
	ABool	modalSessionAborted = false;//#695
	return mTextFinderForHighlight.ExecuteFind(mText,mFindHighlightParam,false,inSpos,epos,outSpos,outEpos,abortflag,false,modalSessionAborted);
}

//R0244
ABool	ADocument_Text::SPI_FindForHighlight2( const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos ) 
{
	//�n�C���C�g������
	AIndex	epos = inEpos;
	if( inEpos-inSpos > kLimit_HighlightTargetTextLength )
	{
		epos = SPI_GetCurrentCharTextIndex(inSpos+kLimit_HighlightTargetTextLength);
	}
	//
	if( mFindHighlightSet2 == false )   return false;
	ABool	abortflag = false;//B0359
	ABool	modalSessionAborted = false;//#695
	return mTextFinderForHighlight2.ExecuteFind(mText,mFindHighlightParam2,false,inSpos,epos,outSpos,outEpos,abortflag,false,modalSessionAborted);
}

void	ADocument_Text::SPI_SetFindHighlight( const AFindParameter& inParam )
{
	if( mFindHighlightSet == true )
	{
		if( 	inParam.findText.Compare(mFindHighlightParam.findText) == true &&
				inParam.ignoreCase == mFindHighlightParam.ignoreCase &&
				inParam.wholeWord == mFindHighlightParam.wholeWord &&
				inParam.fuzzy == mFindHighlightParam.fuzzy &&
				inParam.regExp == mFindHighlightParam.regExp &&
				inParam.ignoreBackslashYen == mFindHighlightParam.ignoreBackslashYen &&
				inParam.ignoreSpaces == mFindHighlightParam.ignoreSpaces )//#165
		{
			return;
		}
		//R0244
		mFindHighlightParam2.Set(mFindHighlightParam);
		mFindHighlightSet2 = true;
	}
	mFindHighlightParam.Set(inParam);
	mFindHighlightSet = true;
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayFindHighlight) == true )
	{
		//TextView�\���X�V
		//win 080727 RefreshWindow();
		SPI_RefreshTextViews();//win 080727
	}
}

ABool	ADocument_Text::SPI_IsFindHighlightSet() const
{
	return mFindHighlightSet;
}

void	ADocument_Text::SPI_ClearFindHighlight()
{
	mFindHighlightSet = false;
	mFindHighlightSet2 = false;//R0244
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayFindHighlight) == true )
	{
		//TextView�\���X�V
		//win 080727 RefreshWindow();
		SPI_RefreshTextViews();//win 080727
	}
}

//#65
/**
@note abort/error���͕Ԃ�l��false
*/
ABool	ADocument_Text::SPI_ReplaceText( const AFindParameter& inParam, 
													const ATextIndex inStart, const ATextIndex inEnd, 
		AItemCount& outReplacedCount, AItemCount& outOffset, AIndex& ioSelectSpos, AIndex& ioSelectEpos )//#245
{
	if( inStart > inEnd )
	{
		_ACError("",NULL);
		return false;
	}
	
	//���ʏ�����
	outOffset = 0;//#245
	outReplacedCount = 0;
	
	ATextIndex	startPos = inStart;
	ATextIndex	endPos = inEnd;
	
	//Undo�A�N�V�����^�O�L�^
	SPI_RecordUndoActionTag(undoTag_Replace);
	
	//#846
	//���[�_���Z�b�V����
	AStEditProgressModalSession	modalSession(kEditProgressType_Replace,true,false,true);
	ABool	result = true;
	try
	{
		
		//�P�ڂ̌���
		ATextIndex	replaceStart1, replaceEnd1;
		ABool	modalSessionAborted = false;
		if( SPI_Find(inParam,false,startPos,endPos,replaceStart1,replaceEnd1,true,modalSessionAborted) == false )
		{
			//modalSessionAborted�t���OON�Ȃ�A�Ԃ�l��true
			result = (modalSessionAborted==false);
		}
		else
		{
			//�u��������쐬
			AText	replaceText1;
			if( inParam.regExp == true )
			{
				SPI_ChangeReplaceText(inParam.replaceText,replaceText1);
			}
			else
			{
				replaceText1.SetText(inParam.replaceText);
			}
			//���̌����J�n�ʒu
			if( replaceStart1 == replaceEnd1 )
			{
				if( replaceEnd1 >= endPos )
				{
					startPos = endPos+1;//���܂ł�������I��点��
				}
				else
				{
					startPos = SPI_GetNextCharTextIndex(replaceEnd1);
				}
			}
			else
			{
				startPos = replaceEnd1;
			}
			
			//�Q�ڈȍ~�̌���
			AText	replaceText2;
			while(true) 
			{
				//���[�_���Z�b�V�����p������
				if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Replace,outReplacedCount,true,startPos-inStart,inEnd-inStart) == false )
				{
					result = false;
					break;
				}
				
				//���̌���
				ABool	found = false;
				ATextIndex	replaceStart2, replaceEnd2;
				if( startPos <= endPos )
				{
					found = SPI_Find(inParam,false,startPos,endPos,replaceStart2,replaceEnd2,true,modalSessionAborted);
					//modalSessionAborted�t���OON�Ȃ�A�Ԃ�l��true
					if( modalSessionAborted == true )
					{
						result = false;
						break;
					}
					//���̌����J�n�ʒu
					if( replaceStart2 == replaceEnd2 )
					{
						if( replaceEnd2 >= endPos )
						{
							startPos = endPos+1;//���܂ł�������I��点��
						}
						else
						{
							startPos = SPI_GetNextCharTextIndex(replaceEnd2);
						}
					}
					else
					{
						startPos = replaceEnd2;
					}
				}
				else
				{
					replaceStart2 = replaceEnd2 = startPos;
				}
				
				//B0182 �܂�replaceText2�Ɏ��̒u����������쐬���ĕۑ�����
				//���̒u��������̍쐬
				replaceText2.DeleteAll();
				if( found == true )
				{
					if( inParam.regExp == true )
					{
						SPI_ChangeReplaceText(inParam.replaceText,replaceText2);
					}
					else
					{
						replaceText2.SetText(inParam.replaceText);
					}
				}
				
				//�P�O�Ɍ������������̒u��
				//DeleteTextFromDocument(replaceStart1,replaceEnd1,true);//dont redraw
				SPI_DeleteText(replaceStart1,replaceEnd1,true,true);
				if( replaceText1.GetItemCount() > 0 )
				{
					//InsertTextToDocument(replaceStart1,replaceText1,true);//#846
					SPI_InsertText(replaceStart1,replaceText1,true,true);
				}
				outReplacedCount++;
				//�u���ɂ�錟���͈͂̈ړ�
				AItemCount	offset = replaceText1.GetItemCount() - (replaceEnd1-replaceStart1);
				startPos += offset;
				endPos += offset;
				outOffset += offset;//#245
				
				if( replaceStart1 < ioSelectSpos ) 	ioSelectSpos += offset;
				if( replaceEnd1 <= ioSelectEpos )  	ioSelectEpos += offset;
				
				//replaceStart1/replaceEnd1�̍X�V
				replaceStart1 	= replaceStart2 + offset;
				replaceEnd1 	= replaceEnd2 + offset;
				
				if( found == false )   break;
				
				//���̒u���������replaceText1�ɃR�s�[
				replaceText1.SetText(replaceText2);
			}
			
			//SPI_DeleteText(), SPI_InsertText()��dont redraw�w�肵�Ă���̂ŁA�����ŕ`����s��
			SPI_RefreshTextViews();
			UpdateDocInfoWindows();
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	//
	return result;
}

#pragma mark ===========================

#pragma mark ---�e�L�X�g�}�[�J�[
//#750

//#750
/**
�e�L�X�g�}�[�J�[���������������
*/
ABool	ADocument_Text::SPI_FindForTextMarker( const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos ) 
{
	//�n�C���C�g������
	AIndex	epos = inEpos;
	if( inEpos-inSpos > kLimit_HighlightTargetTextLength )
	{
		epos = SPI_GetCurrentCharTextIndex(inSpos+kLimit_HighlightTargetTextLength);
	}
	//
	//�����p�����[�^�ݒ�
	AFindParameter	param;
	param.Set(mTextMarkerParam);
	//JIS�n�e�L�X�g�G���R�[�f�B���O�̏ꍇ�́A��Ɂ��^�o�b�N�X���b�V�������ɂ���B #940
	if( SPI_IsJISTextEncoding() == true )
	{
		param.ignoreBackslashYen = true;
	}
	//�ŏ��̈�v�ʒu���L�����邽�߂̕ϐ�
	ABool	matched = false;
	AIndex	firstMatchedSpos = epos;
	AIndex	firstMatchedEpos = epos;
	//�e�L�X�g�}�[�J�[�e�����񖈂̃��[�v
	for( AIndex index = 0; index < mTextMarkerArray_TextFinder.GetItemCount(); index++ )
	{
		//�e�L�X�g�}�[�J�[��������擾���A�����p�����[�^�ɐݒ�
		mTextMarkerArray_FindText.Get(index,param.findText);
		//��������������
		param.findText.LimitLength(0,kLimit_FindTextLength);
		//�������s
		ABool	abortflag = false;
		ABool	modalSessionAborted = false;//#695
		AIndex	spos = kIndex_Invalid, epos = kIndex_Invalid;
		if( mTextMarkerArray_TextFinder.GetObject(index).
		   ExecuteFind(mText,param,false,inSpos,inEpos,spos,epos,abortflag,false,modalSessionAborted) == true )
		{
			//�ŏ��̈�v�ʒu���L��
			if( spos < firstMatchedSpos )
			{
				firstMatchedSpos = spos;
				firstMatchedEpos = epos;
			}
			//��v�t���OON
			matched = true;
		}
	}
	//�ŏ��̈�v�ʒu�����ʂɕԂ�
	outSpos = firstMatchedSpos;
	outEpos = firstMatchedEpos;
	return matched;
}

//#750
/**
�e�L�X�g�}�[�J�[��K�p����
*/
void	ADocument_Text::SPI_ApplyTextMarker()
{
	//������Ԃ̏ꍇ�͉������Ȃ�
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial ||
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		return;
	}
	
	//���݂̃e�L�X�g�}�[�J�[�̃f�[�^���擾
	AText	title, text;
	GetApp().SPI_GetCurrentTextMarkerData(title,text);
	
	//���ݒ肳��Ă�����̂Ɠ����Ȃ牽���������^�[��
	if( mTextMarkerText.Compare(text) == true )
	{
		return;
	}
	mTextMarkerText.SetText(text);
	
	//�����p�����[�^�ݒ�
	mTextMarkerParam.ignoreCase 		= true;
	mTextMarkerParam.wholeWord 			= false;
	mTextMarkerParam.fuzzy				= false;
	mTextMarkerParam.regExp 			= false;
	mTextMarkerParam.loop 				= false;
	mTextMarkerParam.ignoreBackslashYen = true;
	mTextMarkerParam.ignoreSpaces		= true;
	
	//�f�[�^������
	mTextMarkerArray_TextFinder.DeleteAll();
	mTextMarkerArray_FindText.DeleteAll();
	
	//�w�肳�ꂽ�e�L�X�g��explode���āA�etext���̃��[�v
	ATextArray	textArray;
	textArray.ExplodeFromText(text,kUChar_LineEnd);
	for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
	{
		//�e�L�X�g�擾
		AText	findText;
		textArray.Get(i,findText);
		if( findText.GetItemCount() == 0 )   continue;
		
		//�etext���ɁATextFinder����
		mTextMarkerArray_TextFinder.AddNewObject();
		//text�ݒ�
		mTextMarkerArray_FindText.Add(findText);
	}
	
	//�`��X�V
	SPI_RefreshTextViews();
}

#pragma mark ===========================

#pragma mark ---���ݒP��}�[�J�[
//#823

/**
@return ���refresh���K�v���ǂ���
*/
ABool	ADocument_Text::SPI_SetCurrentWordForHighlight( const AText& inWord, const ABool inWholeWord, const ABool inRedraw )
{
	//�w��e�L�X�g�̉��s�R�[�h�܂ł�����������Ƃ���B
	AText	firstlineText;
	AIndex	p = 0;
	inWord.GetLine(p,firstlineText);
	//�ŏ��̉��s�R�[�h�܂łɕ�����������΁A�n�C���C�g����
	if( firstlineText.GetItemCount() == 0 )
	{
		return SPI_ClearCurrentWordForHighlight(true);
	}
	
	//���݂̐ݒ�f�[�^�Ɠ����Ȃ烊�^�[��
	if( mCurrentWordFindHighlightSet == true && mCurrentWordFindHighlightParam.findText.Compare(inWord) == true )
	{
		return false;
	}
	
	//�ĕ`�悪�K�v�ȍsindex���L��
	AObjectArray< AArray<ANumber> >	redrawLineIndexArray;
	if( inRedraw == true )
	{
		//�eview���̃��[�v
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			//view����number array�I�u�W�F�N�g����
			redrawLineIndexArray.AddNewObject();
			//view�̌��݂̕\���s�͈͂��擾
			AIndex	startLineIndex = 0, endLineIndex = 0;
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetDrawLineRange(startLineIndex,endLineIndex);
			if( endLineIndex >= SPI_GetLineCount() )   endLineIndex = SPI_GetLineCount()-1;
			//�e�s���Ƃ̃��[�v
			for( AIndex lineIndex = startLineIndex; lineIndex <= endLineIndex; lineIndex++ )
			{
				//�s���܂ޒi�������������āA�Ώۃe�L�X�g������������A�sindex��ǉ�
				//�i���S�̂���������Ǝ��Ԃ�������̂őO�̍s���玟�̍s�܂ł�����
				AIndex	findStart = 0, findEnd = 0;
				AIndex	paragraphStartLineIndex = SPI_GetCurrentParagraphStartLineIndex(lineIndex);
				if( paragraphStartLineIndex < lineIndex && lineIndex-1 >= 0 )
				{
					findStart = SPI_GetLineStart(lineIndex-1);
				}
				else
				{
					findStart = SPI_GetLineStart(lineIndex);
				}
				AIndex	paragraphEndLineIndex = SPI_GetNextParagraphStartLineIndex(lineIndex);
				if( paragraphEndLineIndex > lineIndex+1 && lineIndex+2 < SPI_GetLineCount() )
				{
					findEnd = SPI_GetLineStart(lineIndex+2);
				}
				else
				{
					findEnd = findStart + SPI_GetLineLengthWithLineEnd(lineIndex);
				}
				//
				ATextIndex	spos = 0, epos = 0;
				if( SPI_FindForCurrentWordHighlight(findStart,findEnd,spos,epos) == true )
				{
					redrawLineIndexArray.GetObject(i).Add(lineIndex);
				}
			}
		}
	}
	
	//�����p�����[�^�ݒ�
	mCurrentWordFindHighlightParam.findText.SetText(firstlineText);
	mCurrentWordFindHighlightParam.ignoreCase 			= false;
	mCurrentWordFindHighlightParam.wholeWord 			= inWholeWord;
	mCurrentWordFindHighlightParam.fuzzy				= false;
	mCurrentWordFindHighlightParam.regExp 				= false;
	mCurrentWordFindHighlightParam.loop 				= false;
	mCurrentWordFindHighlightParam.ignoreBackslashYen 	= false;
	mCurrentWordFindHighlightParam.ignoreSpaces			= false;
	//��������������
	mCurrentWordFindHighlightParam.findText.LimitLength(0,kLimit_FindTextLength);
	//�t���OON
	mCurrentWordFindHighlightSet = true;
	
	//�`��X�V
	if( inRedraw == true )
	{
		//�eview���̃��[�v
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			//view�̌��݂̕\���s�͈͂��擾
			AIndex	startLineIndex = 0, endLineIndex = 0;
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetDrawLineRange(startLineIndex,endLineIndex);
			if( endLineIndex >= SPI_GetLineCount() )   endLineIndex = SPI_GetLineCount()-1;
			//�e�s���Ƃ̃��[�v
			for( AIndex lineIndex = startLineIndex; lineIndex <= endLineIndex; lineIndex++ )
			{
				//�s���܂ޒi�������������āA�Ώۃe�L�X�g������������A�sindex��ǉ�
				//�i���S�̂���������Ǝ��Ԃ�������̂őO�̍s���玟�̍s�܂ł�����
				AIndex	findStart = 0, findEnd = 0;
				AIndex	paragraphStartLineIndex = SPI_GetCurrentParagraphStartLineIndex(lineIndex);
				if( paragraphStartLineIndex < lineIndex && lineIndex-1 >= 0 )
				{
					findStart = SPI_GetLineStart(lineIndex-1);
				}
				else
				{
					findStart = SPI_GetLineStart(lineIndex);
				}
				AIndex	paragraphEndLineIndex = SPI_GetNextParagraphStartLineIndex(lineIndex);
				if( paragraphEndLineIndex > lineIndex+1 && lineIndex+2 < SPI_GetLineCount() )
				{
					findEnd = SPI_GetLineStart(lineIndex+2);
				}
				else
				{
					findEnd = SPI_GetLineStart(lineIndex) + SPI_GetLineLengthWithLineEnd(lineIndex);
				}
				//
				ATextIndex	spos = 0, epos = 0;
				if( SPI_FindForCurrentWordHighlight(findStart,findEnd,spos,epos) == true )
				{
					if( redrawLineIndexArray.GetObjectConst(i).Find(lineIndex) == kIndex_Invalid )
					{
						redrawLineIndexArray.GetObject(i).Add(lineIndex);
					}
				}
			}
			
			//�`��X�V���s
			for( AIndex j = 0; j < redrawLineIndexArray.GetObjectConst(i).GetItemCount(); j++ )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_RefreshLine(redrawLineIndexArray.GetObjectConst(i).Get(j));
			}
		}
	}
	return true;
}

/**
���ݒP��n�C���C�g�N���A
*/
ABool	ADocument_Text::SPI_ClearCurrentWordForHighlight( const ABool inRedraw )
{
	//���݃n�C���C�g�ݒ薳���Ȃ牽���������^�[��
	if( mCurrentWordFindHighlightSet == false )
	{
		return false;
	}
	
	//�ĕ`�悪�K�v�ȍsindex���L��
	AObjectArray< AArray<ANumber> >	redrawLineIndexArray;
	if( inRedraw == true )
	{
		//�eview���̃��[�v
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			//view����number array�I�u�W�F�N�g����
			redrawLineIndexArray.AddNewObject();
			//view�̌��݂̕\���s�͈͂��擾
			AIndex	startLineIndex = 0, endLineIndex = 0;
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetDrawLineRange(startLineIndex,endLineIndex);
			if( endLineIndex >= SPI_GetLineCount() )   endLineIndex = SPI_GetLineCount()-1;
			//�e�s���Ƃ̃��[�v
			for( AIndex lineIndex = startLineIndex; lineIndex <= endLineIndex; lineIndex++ )
			{
				//�s���܂ޒi�������������āA�Ώۃe�L�X�g������������A�sindex��ǉ�
				//�i���S�̂���������Ǝ��Ԃ�������̂őO�̍s���玟�̍s�܂ł�����
				AIndex	findStart = 0, findEnd = 0;
				AIndex	paragraphStartLineIndex = SPI_GetCurrentParagraphStartLineIndex(lineIndex);
				if( paragraphStartLineIndex < lineIndex && lineIndex-1 >= 0 )
				{
					findStart = SPI_GetLineStart(lineIndex-1);
				}
				else
				{
					findStart = SPI_GetLineStart(lineIndex);
				}
				AIndex	paragraphEndLineIndex = SPI_GetNextParagraphStartLineIndex(lineIndex);
				if( paragraphEndLineIndex > lineIndex+1 && lineIndex+2 < SPI_GetLineCount() )
				{
					findEnd = SPI_GetLineStart(lineIndex+2);
				}
				else
				{
					findEnd = findStart + SPI_GetLineLengthWithLineEnd(lineIndex);
				}
				//
				ATextIndex	spos = 0, epos = 0;
				if( SPI_FindForCurrentWordHighlight(findStart,findEnd,spos,epos) == true )
				{
					redrawLineIndexArray.GetObject(i).Add(lineIndex);
				}
			}
		}
	}
	
	//����������N���A
	mCurrentWordFindHighlightParam.findText.DeleteAll();
	mCurrentWordFindHighlightSet = false;
	
	if( inRedraw == true )
	{
		//�eview���̃��[�v
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			//�`��X�V���s
			for( AIndex j = 0; j < redrawLineIndexArray.GetObjectConst(i).GetItemCount(); j++ )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_RefreshLine(redrawLineIndexArray.GetObjectConst(i).Get(j));
			}
		}
	}
	return true;
}

/**
���ݒP��n�C���C�g�p����
*/
ABool	ADocument_Text::SPI_FindForCurrentWordHighlight( const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos )
{
	if( mCurrentWordFindHighlightSet == false )   return false;
	//�n�C���C�g��������
	AIndex	epos = inEpos;
	if( inEpos-inSpos > kLimit_HighlightTargetTextLength )
	{
		epos = SPI_GetCurrentCharTextIndex(inSpos+kLimit_HighlightTargetTextLength);
	}
	//
	ABool	abortflag = false;
	ABool	modalSessionAborted = false;//#695
	return mTextFinderForCurrentWordHighlight.ExecuteFind(mText,mCurrentWordFindHighlightParam,false,inSpos,epos,outSpos,outEpos,abortflag,false,modalSessionAborted);
}

#pragma mark ===========================

#pragma mark ---Diff���[�h����

//#81
/**
�f�t�H���g��Diff���[�h��ݒ肷��
*/
void	ADocument_Text::SetDefaultDiffMode()
{
	//�f�t�H���g��Diff���[�h��ݒ肷��B
	//�D�揇��
	//�P�D��r��t�H���_���ݒ肳��Ă���A��r��t�@�C�������݂���ꍇ
	//�Q�D���|�W�g��
	
	//��r��t�H���_���ݒ肳��Ă���A��r��t�@�C�������݂���ꍇ�́A���̃t�@�C���Ɣ�r
	AFileAcc	file;
	if( GetDiffFileInDiffSourceFolder(file) == true )
	{
		SPI_SetDiffMode_File(file,kCompareMode_File);
		return;
	}
	//���|�W�g���Ɣ�r
	if( SPI_CanDiffWithRepositiory() == true )
	{
		SPI_SetDiffMode_Repository();
		return;
	}
}

/**
���|�W�g����r�Ώۃt�@�C�����ǂ����iSCM�t�@�C�����ǂ����j�𔻒�
*/
ABool	ADocument_Text::SPI_CanDiffWithRepositiory() const
{
	if( NVI_IsFileSpecified() == true )
	{
		if( mSCMFileState != kSCMFileState_NotSCMFolder && mSCMFileState != kSCMFileState_NotEntried )//#568
		{
			return true;
		}
	}
	return false;
}

/**
��r��t�H���_���ݒ肳��Ă���A��r��t�@�C�������݂��邩�ǂ����𔻒肵�A��r��t�@�C�����擾
*/
ABool	ADocument_Text::GetDiffFileInDiffSourceFolder( AFileAcc& outFile ) const
{
	ABool	result = false;
	//
	if( NVI_IsFileSpecified() == true )
	{
		//�t�@�C���p�X�擾
		AFileAcc	file;
		NVI_GetFile(file);
		AText	path;
		file.GetPath(path);
		//Diff��r��t�H���_�ݒ�擾
		AText	diffSourceFolderWorkingPath;
		GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffSourceFolder_Working,diffSourceFolderWorkingPath);
		//���̃h�L�������g�̃t�@�C����Diff��r��t�H���_���̃t�@�C�����ǂ����𔻒�
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDiffSourceFolder_Enable) == true &&
					diffSourceFolderWorkingPath.GetItemCount() > 0 && path.GetItemCount() > 0 && 
					path.GetItemCount() > diffSourceFolderWorkingPath.GetItemCount() && 
					diffSourceFolderWorkingPath.CompareMin(path) == true )
		{
			//���΃p�X���擾
			AText	relativePath;
			relativePath.InsertText(0,path,diffSourceFolderWorkingPath.GetItemCount(),
						path.GetItemCount()-diffSourceFolderWorkingPath.GetItemCount());
			//Diff��r���t�H���_�擾
			AText	diffSourceFolderPath;
			GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffSourceFolder_Source,diffSourceFolderPath);
			AFileAcc	diffSourceFolder;
			diffSourceFolder.Specify(diffSourceFolderPath);
			//Diff��r���t�@�C���擾
			AFileAcc	file;
			file.SpecifyChild(diffSourceFolder,relativePath);
			//�t�@�C�������݂��Ă��邩�ǂ�������
			if( file.Exist() == true )
			{
				outFile.CopyFrom(file);
				result = true;
			}
		}
	}
	return result;
}

//#379 
/**
Diff���[�h��ݒ�i���|�W�g���Ƃ̔�r��ݒ�j
*/
void	ADocument_Text::SPI_SetDiffMode_Repository()
{
	//���Ƀ��|�W�g���Ƃ̔�r���[�h�ł���΁A���������I��
	if( mDiffTargetMode == kDiffTargetMode_RepositoryLatest )
	{
		return;
	}
	
	//compare���[�h�i���ڍׂȔ�r���ʁj�ݒ�
	mCompareMode = kCompareMode_Repository;
	//diff���[�h�ݒ�
	AFileAcc	dummy;
	SetDiffMode(kDiffTargetMode_RepositoryLatest,dummy,kIndex_Invalid,kIndex_Invalid);
}

//#379
/**
Diff���[�h��ݒ�i�t�@�C���Ƃ̔�r��ݒ�j
*/
void	ADocument_Text::SPI_SetDiffMode_File( const AFileAcc& inDiffTargetFile, const ACompareMode inCompareMode )
{
	/*��r���t�@�C����ύX���A�ēx��r���t�@�C����I�����Ĕ�r�o����悤�ɂ��邽�߁A�����t�@�C�����ǂ����̃`�F�b�N�͍s��Ȃ�
	//
	if( mDiffTargetMode == kDiffTargetMode_File && mDiffTargetFile.Compare(inDiffTargetFile) == true )
	{
		return;
	}
	*/
	
	//compare���[�h�i���ڍׂȔ�r���ʁj�ݒ�
	mCompareMode = inCompareMode;
	//diff���[�h�ݒ�
	SetDiffMode(kDiffTargetMode_File,inDiffTargetFile,kIndex_Invalid,kIndex_Invalid);
}

/**
Diff���[�h�ݒ����
*/
void	ADocument_Text::SPI_SetDiffMode_None()
{
	//����diff���[�h�ݒ�������Ă���΁A���������I��
	if( mDiffTargetMode == kDiffTargetMode_None )
	{
		return;
	}
	
	//compare���[�h�i���ڍׂȔ�r���ʁj�ݒ�
	mCompareMode = kCompareMode_NoCompare;
	//diff���[�h�ݒ�
	AFileAcc	dummy;
	SetDiffMode(kDiffTargetMode_None,dummy,kIndex_Invalid,kIndex_Invalid);
}

//#192
/**
Diff���[�h��ݒ�i�w��e�L�X�g�͈͂Ɣ�r�j
*/
void	ADocument_Text::SPI_SetDiffMode_TextWithRange( const AText& inTargetText, 
		const AIndex inDiffTextRangeStart, const AIndex inDiffTextRangeEnd,
		const ACompareMode inCompareMode )
{
	//diff��r�p�e���|�����t�H���_�������Ȃ琶���A�擾����
	if( mDiffTempFolder.IsSpecified() == false || mDiffTempFolder.Exist() == false )
	{
		GetApp().SPI_GetUniqTempFolder(mDiffTempFolder);
		mDiffTempFolder.CreateFolder();
	}
	//inTargetText��UTF8/LF�Ńe���|�����t�@�C���ɕۑ�����
	AFileAcc	diffTargetFile;
	diffTargetFile.SpecifyChild(mDiffTempFolder,"difftargetfile");
	diffTargetFile.CreateFile();
	AText	text;
	text.SetText(inTargetText);
	text.ConvertLineEndToLF();
	diffTargetFile.WriteFile(text);
	//compare���[�h�i���ڍׂȔ�r���ʁj�ݒ�
	mCompareMode = inCompareMode;
	//diff���[�h�ݒ�
	SetDiffMode(kDiffTargetMode_TextWithRange,diffTargetFile,inDiffTextRangeStart,inDiffTextRangeEnd);
}

/**
Diff���[�h�ݒ�i���ʏ����j
*/
void	ADocument_Text::SetDiffMode( const ADiffTargetMode inMode, const AFileAcc& inDiffTargetFile,
		const AIndex inDiffTextRangeStart, const AIndex inDiffTextRangeEnd )
{
	//Diff���S�폜
	ClearDiffDisplay();
	//������diff target�h�L�������g�����
	CloseCurrentDiffTargetDocument();
	//diff target mode��ݒ�
	mDiffTargetMode = inMode;
	//DiffTargetFile�ݒ�
	mDiffTargetFile.CopyFrom(inDiffTargetFile);
	//diff�e�L�X�g�͈͂�ݒ�
	mDiffTextRangeStart = inDiffTextRangeStart;
	mDiffTextRangeEnd = inDiffTextRangeEnd;
	//diff�e�L�X�g�͈͊J�n�i����������
	//�i�K�v�ȏꍇ(=kDiffTargetMode_TextWithRange)�AExecuteDiff()�Œl���ݒ肳���j
	mDiffTextRangeStartParagraph = 0;
	//diff�\���f�[�^�X�V
	SPI_UpdateDiffDisplayData();
}

//
/**
������diff target�h�L�������g�����
*/
void	ADocument_Text::CloseCurrentDiffTargetDocument()
{
	if( mDiffTargetFileDocumentID != kObjectID_Invalid )
	{
		//�h�L�������g�i�ɑΉ�����E�C���h�E�j�����
		GetApp().SPI_ExecuteCloseTextDocument(mDiffTargetFileDocumentID);
		//diff target document id���N���A
		mDiffTargetFileDocumentID = kObjectID_Invalid;
	}
}

//R0006
/**
Diff�\���f�[�^�̍X�V���s���BOpen/Save�����ɃR�[�������B
���Ɣ�r���邩��mDiffTargetMdoe�Ō��܂�
*/
void	ADocument_Text::SPI_UpdateDiffDisplayData( const ABool inReloadRepository )
{
	//#379 NVIDO_ViewRegistered()�Ŕ���if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == false )   return;
	if( NVI_IsDirty() == true )   return;
	//if( SPI_IsRemoteFileMode() == true || NVI_IsFileSpecified() == false )  return;
	
	switch(mDiffTargetMode)
	{
	  case kDiffTargetMode_RepositoryLatest:
		{
			if( SPI_IsRemoteFileMode() == true )  return;
			if( NVI_IsFileSpecified() == false )   return;
			//Diff�f�[�^�폜
			mDiffDisplayArray_DiffType.DeleteAll();
			mDiffDisplayArray_StartParagraphIndex.DeleteAll();
			mDiffDisplayArray_EndParagraphIndex.DeleteAll();
			mDiffDisplayArray_DeletedText.DeleteAll();//#379
			mDiffParagraphIndexArray.DeleteAll();//#379
			//#568 ���݂�SCM�t�@�C����ԃ`�F�b�N
			if( mSCMFileState == kSCMFileState_NotSCMFolder || mSCMFileState == kSCMFileState_NotEntried )
			{
				return;
			}
			//#379 mDiffDisplayMode = kDiffDisplayMode_None;
			//#379 Diff�f�[�^�X�V�i���|�W�g���e�L�X�g�����{Diff�̏ꍇ�͔񓯊����s�ADiff�݂̂̏ꍇ�͓������s�j
			if( mRepositoryDocumentID == kObjectID_Invalid || inReloadRepository == true )
			{
				//SCM�c�[���̔�r�R�}���h���s�i���s������ASPI_DoSCMCompareResult()���R�[�������j
				AFileAcc	docfile;
				NVI_GetFile(docfile);
				mDiffDisplayWaitFlag = true;
				GetApp().SPI_GetSCMFolderManager().CompareWithLatest(docfile,GetObjectID());//�񓯊����s
			}
			else
			{
				//diff�R�}���h���s
				ExecuteDiff();
			}
			break;
		} 
	  case kDiffTargetMode_File:
	  case kDiffTargetMode_TextWithRange://#192
		{
			//if( SPI_IsRemoteFileMode() == false && NVI_IsFileSpecified() == false )   return;
			//
			mDiffDisplayArray_DiffType.DeleteAll();
			mDiffDisplayArray_StartParagraphIndex.DeleteAll();
			mDiffDisplayArray_EndParagraphIndex.DeleteAll();
			mDiffDisplayArray_DeletedText.DeleteAll();
			mDiffParagraphIndexArray.DeleteAll();
			//
			if( mDiffTargetFileDocumentID == kObjectID_Invalid )
			{
				//diff target�h�L�������g����
				//�iSetDiffMode()�o�R�̏ꍇ�́A�K��������ɂȂ�BSetDiffMode()�Ŋ�����diff target document����Ă��邽�߁j
				
				//mDiffTargetFile�́A�t�@�C���Ƃ̔�r���[�h�̏ꍇ�͔�r��t�@�C���A�e�L�X�g�͈͔�r���[�h�̏ꍇ�͔�r�e�L�X�g��ۑ������e���|�����t�@�C��
				
				//�^�C�g���擾
				AText	title;
				mDiffTargetFile.GetPath(title);
				title.AddCstring(":Diff");
				AText	tec;
				if( mDiffTargetMode == kDiffTargetMode_TextWithRange )
				{
					//�e�L�X�g�͈͔�r���[�h�̏ꍇ�́A�K��UTF8�Ȃ̂ŁA�e�L�X�g�G���R�[�f�B���O���w�肵�ăh�L�������g����
					tec.SetCstring("UTF-8");
				}
				else if( mDiffTargetMode == kDiffTargetMode_File )
				{
					//�t�@�C����r���[�h�̏ꍇ�́A��r��t�@�C����TEC�͗l�X�B
					
					//�����o�b�N�A�b�v�Ƃ̔�r�̏ꍇ�́A�����o�b�N�A�b�v�t�@�C�����{_attr�t�@�C���Ƀe�L�X�g�G���R�[�f�B���O���ۑ�����Ă���̂ŁA
					//������w��
					switch(mCompareMode)
					{
					  case kCompareMode_AutoBackupMinutely:
					  case kCompareMode_AutoBackupHourly:
					  case kCompareMode_AutoBackupDaily:
					  case kCompareMode_AutoBackupMonthly:
						{
							AFileAcc	parentFolder;
							parentFolder.SpecifyParent(mDiffTargetFile);
							AText	filename;
							mDiffTargetFile.GetFilename(filename);
							filename.AddCstring("_attr");
							AFileAcc	tecfile;
							tecfile.SpecifyChild(parentFolder,filename);
							if( tecfile.Exist() == true )
							{
								tecfile.ReadTo(tec);
							}
							else
							{
								//_attr�t�@�C�����񑶍݂̏ꍇ�́A���h�L�������g�̃e�L�X�g�G���R�[�f�B���O�ɂ���
								SPI_GetTextEncoding(tec);
							}
							break;
						}
					  default:
						{
							//�����Ȃ�
							break;
						}
					}
				}
				//�E�C���h�E���������Ƀh�L�������g����
				ATextDocumentFactory	factory(this,kObjectID_Invalid);
				mDiffTargetFileDocumentID = GetApp().SPNVI_CreateDiffTargetDocument(title,mDiffTargetFile,mModeIndex,tec,
						kIndex_Invalid,kIndex_Invalid,false,false,SPI_GetFirstWindowID());
				GetApp().SPI_GetTextDocumentByID(mDiffTargetFileDocumentID).mDiffWorkingDocumentID = GetObjectID();
				GetApp().SPI_GetTextDocumentByID(mDiffTargetFileDocumentID).SPI_SetOwnedDocument();
				//�s�܂�Ԃ��v�Z�i���C���X���b�h���Ōv�Z�j
				GetApp().SPI_GetTextDocumentByID(mDiffTargetFileDocumentID).SPI_DoViewActivating(false);
			}
			else
			{
				//��r���t�@�C�����ύX����Ă���ꍇ�ɁAdiff target document�̃f�[�^���X�V����B
				//�idiff target document��dirty�ɂ͂Ȃ�Ȃ��̂ŁA�_�C�A���O�����Ń����[�h�����j
				GetApp().SPI_GetTextDocumentByID(mDiffTargetFileDocumentID).SPI_CheckEditByOtherApp();
			}
			//Diff�R�}���h���s
			ExecuteDiff();
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

//#379
/**
���݂�Diff���[�h�ɉ�����Diff�Ώۃh�L�������g��Ԃ�
*/
ADocumentID	ADocument_Text::SPI_GetDiffTargetDocumentID() const
{
	ADocumentID	diffTargetDocumentID = kObjectID_Invalid;
	switch(mDiffTargetMode)
	{
		//
	  case kDiffTargetMode_RepositoryLatest:
		{
			diffTargetDocumentID = mRepositoryDocumentID;
			break;
		}
	  case kDiffTargetMode_File:
	  case kDiffTargetMode_TextWithRange://#192
		{
			diffTargetDocumentID = mDiffTargetFileDocumentID;
			break;
		}
	  case kDiffTargetMode_None:
		{
			//Diff�Ώۃh�L�������g�̏ꍇ�́A�Ή�����working�h�L�������g��Ԃ�
			if( mDiffWorkingDocumentID != kObjectID_Invalid )
			{
				diffTargetDocumentID = mDiffWorkingDocumentID;
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return diffTargetDocumentID;
}

/**
diff��r���[�h�idiff���[�h���ڍׂȔ�r��j�A����сA��r��t�@�C�����擾
*/
ACompareMode	ADocument_Text::SPI_GetCompareMode( AText& outFilename ) const
{
	outFilename.DeleteAll();
	if( mDiffTargetMode == kDiffTargetMode_File )
	{
		mDiffTargetFile.GetFilename(outFilename);
	}
	return mCompareMode;
}

#pragma mark ===========================

#pragma mark ---Diff�\���f�[�^

//#379
/**
DiffArray�ɍ����f�[�^��ݒ肷��
*/
void	ADocument_Text::SetDiffArray( const AArray<ADiffType>& inDiffTypeArray, 
		const AArray<AIndex>& inStartParagraphIndexArray, const AArray<AIndex>& inEndParagraphIndexArray,
		const AArray<AIndex>& inOldStartParagraphIndexArray, const AArray<AIndex>& inOldEndParagraphIndexArray,//#379
		const ATextArray& inDeletedText, const ABool inSetParagraphIndexArray )
{
	//�F�擾
	AColor	subWindowNormalColor = kColor_List_Normal;//#1316 GetApp().SPI_GetSubWindowLetterColor();
	AColor	jumpListDiffColor = kColor_List_Red;//#1316 GetApp().SPI_GetJumpListDiffColor();
	
	//#304 ��L�t���O�̑O�ɂ����������������Ɉړ�
	mDiffDisplayArray_DiffType.DeleteAll();
	mDiffDisplayArray_StartParagraphIndex.DeleteAll();
	mDiffDisplayArray_EndParagraphIndex.DeleteAll();
	mDiffDisplayArray_DeletedText.DeleteAll();//#379
	mDiffParagraphIndexArray.DeleteAll();//#379
	
	if( inDiffTypeArray.GetItemCount() > 0 )
	{
		//Diff�f�[�^�ǉ�
		for( AIndex i = 0; i < inDiffTypeArray.GetItemCount(); i++ )
		{
			mDiffDisplayArray_DiffType.Add(inDiffTypeArray.Get(i));
			mDiffDisplayArray_StartParagraphIndex.Add(inStartParagraphIndexArray.Get(i));//#379 SPI_GetLineIndexByParagraphIndex(inStartParagraphIndexArray.Get(i)));
			mDiffDisplayArray_EndParagraphIndex.Add(inEndParagraphIndexArray.Get(i));//#379 SPI_GetLineIndexByParagraphIndex(inEndParagraphIndexArray.Get(i)));
			mDiffDisplayArray_DeletedText.Add(inDeletedText.GetTextConst(i));//#379
		}
		//#379 mDiffDisplayMode = kDiffDisplayMode_RepositoryLatest;
		
		//�W�����v���X�g�X�V
		//#844 AColor	diffcolor = kColor_DarkBlue;
		//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_JumpList,diffcolor);
		//�e�W�����v���X�g���ږ��̃��[�v
		for( AIndex i = 0; i < mJump_IdentifierObjectArray.GetItemCount(); i++ )
		{
			/*spt,ept�ł͂Ȃ�����paragraph���擾����悤�ɕύX#695
			ATextPoint	spt,ept;
			SPI_GetGlobalIdentifierRange(mJump_IdentifierObjectArray.Get(i),spt,ept);
			//�b�� mJump_IdentifierObjectArray�ɑ��݂���Identifier���ATextInfo��mGlobalIdentifierList�ɑ��݂��Ȃ����Ƃ�����B
			//���R�����������A�Ƃ肠�����A��蔭������break���邱�Ƃɂ���B#571
			*/
			AIndex	startParagraphIndex = mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(i));//#695
			if( /*#695 spt.y == kIndex_Invalid || ept.y == kIndex_Invalid*/startParagraphIndex == kIndex_Invalid )
			{
				_ACError("Object in mJump_IdentifierObjectArray does not exist.",this);
				break;
			}
			//
			AIndex	endParagraphIndex = SPI_GetParagraphCount();//#695
			if( i+1 < mJump_IdentifierObjectArray.GetItemCount() )
			{
				/*#695
				ATextPoint	s,e;
				SPI_GetGlobalIdentifierRange(mJump_IdentifierObjectArray.Get(i+1),s,e);
				ept = e;
				*/
				endParagraphIndex = mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(i+1));//#695
			}
			/*#695
			else
			{
				ept.x = 0;
				ept.y = SPI_GetLineCount()-1;//#379
			}
			*/
			//�W�����v���X�g���ڔ͈͓���diff�͈͂����݂��邩�ǂ����𔻒�
			for( AIndex j = 0; j < mDiffDisplayArray_StartParagraphIndex.GetItemCount(); j++ )
			{
				if( ( /*#695 SPI_GetParagraphIndexByLineIndex(ept.y)*/
								endParagraphIndex <= mDiffDisplayArray_StartParagraphIndex.Get(j) || 
								/*#695 SPI_GetParagraphIndexByLineIndex(spt.y)*/
								startParagraphIndex >= mDiffDisplayArray_EndParagraphIndex.Get(j) ) == false )//#379
				{
					mJump_MenuColorArray.Set(i,jumpListDiffColor);
				}
				//else fprintf(stderr,"s:%d e:%d sd:%d ed:%d   ",spt.y,ept.y,mDiffDisplayArray_StartParagraphIndex.Get(j),mDiffDisplayArray_EndParagraphIndex.Get(j));
			}
		}
	}
	else
	{
		for( AIndex i = 0; i < mJump_MenuColorArray.GetItemCount(); i++ )
		{
			mJump_MenuColorArray.Set(i,subWindowNormalColor);
		}
	}
	//#212 RefreshWindow();
	//�֐��O���ŃR�[������SPI_RefreshTextViews();//#212
	SPI_UpdateJumpList();//#695 true);//#291
	
	if( inSetParagraphIndexArray == true )
	{
		/*#737
		//Repository�h�L�������g�̑Ή�����i���C���f�b�N�X�f�[�^mDiffParagraphIndexArray���쐬
		AIndex	lastWorkingParagraphIndex = 0;
		AIndex	lastRepositoryParagraphIndex = 0;
		//�eDiffPart���Ƃ̃��[�v
		for( AIndex i = 0; i < mDiffDisplayArray_DiffType.GetItemCount(); i++ )
		{
			//�O���DiffPart�J�n�ʒu�܂ł̑Ή��s��ǉ�
			AIndex	diffStart = mDiffDisplayArray_StartParagraphIndex.Get(i);
			for( ; lastWorkingParagraphIndex < diffStart; lastWorkingParagraphIndex++ )
			{
				mDiffParagraphIndexArray.Add(lastRepositoryParagraphIndex);
				lastRepositoryParagraphIndex++;
			}
			//DiffPart���̑Ή��s��ǉ�
			AIndex	diffEnd = mDiffDisplayArray_EndParagraphIndex.Get(i);
			for( ; lastWorkingParagraphIndex < diffEnd; lastWorkingParagraphIndex++ )
			{
				mDiffParagraphIndexArray.Add(inOldStartParagraphIndexArray.Get(i));
			}
			//����Repository���J�n�ʒu��ݒ�
			if( mDiffDisplayArray_DiffType.Get(i) == kDiffType_Added )
			{
				lastRepositoryParagraphIndex = inOldStartParagraphIndexArray.Get(i);
			}
			else
			{
				lastRepositoryParagraphIndex = inOldEndParagraphIndexArray.Get(i);
			}
		}
		//�Ō��DiffPart�ȍ~�̑Ή��s��ǉ�
		AItemCount	paraCount = SPI_GetParagraphCount();
		for( ; lastWorkingParagraphIndex < paraCount; lastWorkingParagraphIndex++ )
		{
			mDiffParagraphIndexArray.Add(lastRepositoryParagraphIndex);
			lastRepositoryParagraphIndex++;
		}
		*/
		SPI_GetDiffParagraphIndexArray(SPI_GetParagraphCount(),mDiffDisplayArray_DiffType,
					mDiffDisplayArray_StartParagraphIndex,mDiffDisplayArray_EndParagraphIndex,
					inOldStartParagraphIndexArray,inOldEndParagraphIndexArray,mDiffParagraphIndexArray);
	}
}

/**
diff target document�̒i���Ƃ̑Ή��e�[�u�����擾
*/
void	ADocument_Text::SPI_GetDiffParagraphIndexArray( const AItemCount inParagraphCount,
		const AArray<ADiffType>& inDiffTypeArray, 
		const AHashArray<AIndex>& inStartParagraphIndexArray, const AHashArray<AIndex>& inEndParagraphIndexArray,
		const AArray<AIndex>& inOldStartParagraphIndexArray, const AArray<AIndex>& inOldEndParagraphIndexArray,
		AArray<AIndex>& outDiffParagraphIndexArray )
{
	outDiffParagraphIndexArray.DeleteAll();
	//Repository�h�L�������g�̑Ή�����i���C���f�b�N�X�f�[�^mDiffParagraphIndexArray���쐬
	AIndex	lastWorkingParagraphIndex = 0;
	AIndex	lastRepositoryParagraphIndex = 0;
	//�eDiffPart���Ƃ̃��[�v
	for( AIndex i = 0; i < inDiffTypeArray.GetItemCount(); i++ )
	{
		//�O���DiffPart�J�n�ʒu�܂ł̑Ή��s��ǉ�
		AIndex	diffStart = inStartParagraphIndexArray.Get(i);
		for( ; lastWorkingParagraphIndex < diffStart; lastWorkingParagraphIndex++ )
		{
			outDiffParagraphIndexArray.Add(lastRepositoryParagraphIndex);
			lastRepositoryParagraphIndex++;
		}
		//DiffPart���̑Ή��s��ǉ�
		AIndex	diffEnd = inEndParagraphIndexArray.Get(i);
		for( ; lastWorkingParagraphIndex < diffEnd; lastWorkingParagraphIndex++ )
		{
			outDiffParagraphIndexArray.Add(inOldStartParagraphIndexArray.Get(i));
		}
		//����Repository���J�n�ʒu��ݒ�
		if( inDiffTypeArray.Get(i) == kDiffType_Added )
		{
			lastRepositoryParagraphIndex = inOldStartParagraphIndexArray.Get(i);
		}
		else
		{
			lastRepositoryParagraphIndex = inOldEndParagraphIndexArray.Get(i);
		}
	}
	//�Ō��DiffPart�ȍ~�̑Ή��s��ǉ�
	for( ; lastWorkingParagraphIndex < inParagraphCount; lastWorkingParagraphIndex++ )
	{
		outDiffParagraphIndexArray.Add(lastRepositoryParagraphIndex);
		lastRepositoryParagraphIndex++;
	}
	
}

//#450
/**
�w��s���܂ފ֐����ɍ��������݂��邩�ǂ�����Ԃ�
*/
/*
ABool	ADocument_Text::SPI_DiffExistInCurrentFunction( const AIndex inLineIndex ) const
{
	//�w��s���܂ފ֐���jump item index���擾
	AIndex	jumpItemIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);
	
	//�֐��̊J�n�i��index�擾
	AIndex	startParagraphIndex = mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(jumpItemIndex));//#695
	if( startParagraphIndex == kIndex_Invalid )
	{
		_ACError("Object in mJump_IdentifierObjectArray does not exist.",this);
		return false;
	}
	//���̊֐��̊J�n�i��index�擾
	AIndex	endParagraphIndex = SPI_GetParagraphCount();//#695
	if( jumpItemIndex+1 < mJump_IdentifierObjectArray.GetItemCount() )
	{
		endParagraphIndex = mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(jumpItemIndex+1));//#695
	}
	//�W�����v���X�g���ڔ͈͓���diff�͈͂����݂��邩�ǂ����𔻒�
	for( AIndex j = 0; j < mDiffDisplayArray_StartParagraphIndex.GetItemCount(); j++ )
	{
		if( ( endParagraphIndex <= mDiffDisplayArray_StartParagraphIndex.Get(j) || 
						startParagraphIndex >= mDiffDisplayArray_EndParagraphIndex.Get(j) ) == false )//#379
		{
			return true;
		}
	}
	return false;
}
*/

/**
�w��͈͓���diff target�h�L�������g�Ƃ̍��������邩�ǂ������擾
*/
ABool	ADocument_Text::SPI_DiffExistInRange( const AIndex inStartLineIndex, const AIndex inEndLineIndex ) const
{
	//�w��͈͂̒i��index���擾
	AIndex	startParagraphIndex = SPI_GetParagraphIndexByLineIndex(inStartLineIndex);
	AIndex	endParagraphIndex = SPI_GetParagraphIndexByLineIndex(inEndLineIndex);
	//�W�����v���X�g���ڔ͈͓���diff�͈͂����݂��邩�ǂ����𔻒�
	for( AIndex j = 0; j < mDiffDisplayArray_StartParagraphIndex.GetItemCount(); j++ )
	{
		if( ( endParagraphIndex <= mDiffDisplayArray_StartParagraphIndex.Get(j) || 
						startParagraphIndex >= mDiffDisplayArray_EndParagraphIndex.Get(j) ) == false )//#379
		{
			return true;
		}
	}
	return false;
}

//#379
/**
���̃h�L�������g�̒i��index�ɑΉ�����ADiff�Ώۃh�L�������g�̒i��index���擾
*/
AIndex	ADocument_Text::SPI_GetDiffTargetParagraphIndexFromThisDocumentParagraphIndex( const AIndex inParagraphIndex ) const
{
	//
	if( inParagraphIndex >= mDiffParagraphIndexArray.GetItemCount() )
	{
		return kIndex_Invalid;
	}
	//mDiffParagraphIndexArray�̎w��index�̒l��Ԃ�
	return mDiffParagraphIndexArray.Get(inParagraphIndex);
}

//#379
/**
���L���̃h�L�������g��ID��Ԃ�
*/
void	ADocument_Text::SPI_GetOwnDocIDArray( AArray<ADocumentID>& outOwnDocArray ) const
{
	//RepositoryDocument����Ȃ炻���ǉ�
	if( mRepositoryDocumentID != kObjectID_Invalid )
	{
		outOwnDocArray.Add(mRepositoryDocumentID);
	}
	//DiffTargetFileDocument����Ȃ炻���ǉ�
	if( mDiffTargetFileDocumentID != kObjectID_Invalid )
	{
		outOwnDocArray.Add(mDiffTargetFileDocumentID);
	}
}

//#379
/**
DiffPart�̃C���f�b�N�X����DiffType���擾
*/
ADiffType	ADocument_Text::SPI_GetDiffTypeByDiffIndex( const AIndex inDiffIndex ) const
{
	return mDiffDisplayArray_DiffType.Get(inDiffIndex);
}

/**
�sIndex����DiffType���擾
*/
ADiffType	ADocument_Text::SPI_GetDiffTypeByIncludingLineIndex( const AIndex inLineIndex ) const
{
	//�i��index���擾
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);//#379
	//�e�L�X�g�͈͔�r���[�h�̏ꍇ�A�e�L�X�g�͈͊O�ł���΁A�����ŏI���i�w��s�̓O���[�\�������j
	if( mDiffTargetMode == kDiffTargetMode_TextWithRange )
	{
		if( paraIndex < mDiffTextRangeStartParagraph || paraIndex > mDiffTextRangeEndParagraph )
		{
			return kDiffType_OutOfDiffRange;
		}
	}
	//
	AItemCount	itemcount = mDiffDisplayArray_StartParagraphIndex.GetItemCount();
	for( AIndex i = 0; i < itemcount; i++ )
	{
		if( /*#379 inLineIndex*/paraIndex >= mDiffDisplayArray_StartParagraphIndex.Get(i) &&
					/*#379 inLineIndex*/paraIndex < mDiffDisplayArray_EndParagraphIndex.Get(i) )
		{
			return mDiffDisplayArray_DiffType.Get(i);
		}
	}
	return kDiffType_None;
}

//#379
/**
DiffPart�̃C���f�b�N�X���炻��Part�̊J�n�i�����擾
*/
AIndex	ADocument_Text::SPI_GetDiffStartParagraphIndexByDiffIndex( const AIndex inDiffIndex ) const
{
	return mDiffDisplayArray_StartParagraphIndex.Get(inDiffIndex);
}

//#379
/**
DiffPart�̃C���f�b�N�X���炻��Part�̊J�n�s���擾
*/
AIndex	ADocument_Text::SPI_GetDiffStartLineIndexByDiffIndex( const AIndex inDiffIndex ) const
{
	AIndex	paraIndex = mDiffDisplayArray_StartParagraphIndex.Get(inDiffIndex);
	return SPI_GetLineIndexByParagraphIndex(paraIndex);
}

//#379
/**
DiffPart�̃C���f�b�N�X���炻��Part�̏I���s���擾
*/
AIndex	ADocument_Text::SPI_GetDiffEndLineIndexByDiffIndex( const AIndex inDiffIndex ) const
{
	AIndex	paraIndex = mDiffDisplayArray_EndParagraphIndex.Get(inDiffIndex);
	return SPI_GetLineIndexByParagraphIndex(paraIndex);
}

//#379
/**
�sIndex����DiffPart�C���f�b�N�X���擾
*/
AIndex	ADocument_Text::SPI_GetDiffIndexByIncludingLineIndex( const AIndex inLineIndex ) const
{
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);
	AItemCount	itemcount = mDiffDisplayArray_StartParagraphIndex.GetItemCount();
	for( AIndex i = 0; i < itemcount; i++ )
	{
		if( paraIndex >= mDiffDisplayArray_StartParagraphIndex.Get(i) &&
					paraIndex < mDiffDisplayArray_EndParagraphIndex.Get(i) )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

//#379
/**
DiffPart�J�n�i������DiffPart�C���f�b�N�X���擾
*/
AIndex	ADocument_Text::SPI_GetDiffIndexByStartParagraphIndex( const AIndex inParagraphIndex ) const
{
	return mDiffDisplayArray_StartParagraphIndex.Find(inParagraphIndex);
}

//#379
/**
DiffPart�I���s����DiffPart�C���f�b�N�X���擾
*/
AIndex	ADocument_Text::SPI_GetDiffIndexByEndParagraphIndex( const AIndex inParagraphIndex ) const
{
	return mDiffDisplayArray_EndParagraphIndex.Find(inParagraphIndex);
}

//#379
/**
DiffPart�J�n�s����DiffPart�C���f�b�N�X���擾
*/
AIndex	ADocument_Text::SPI_GetDiffIndexByStartLineIndex( const AIndex inLineIndex ) const
{
	//�i���J�n�s�ȊO�Ȃ猟�����Ȃ�
	if( inLineIndex > 0 )
	{
		if( SPI_GetLineExistLineEnd(inLineIndex-1) == false )   return kIndex_Invalid;
	}
	//�w��s����n�܂�DiffIndex������
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);
	return mDiffDisplayArray_StartParagraphIndex.Find(paraIndex);
}

//#379
/**
DiffPart�I���s����DiffPart�C���f�b�N�X���擾
*/
AIndex	ADocument_Text::SPI_GetDiffIndexByEndLineIndex( const AIndex inLineIndex ) const
{
	//�i���J�n�s�ȊO�Ȃ猟�����Ȃ�
	if( inLineIndex > 0 )
	{
		if( SPI_GetLineExistLineEnd(inLineIndex-1) == false )   return kIndex_Invalid;
	}
	//�w��s�ŏI������DiffIndex������
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);
	return mDiffDisplayArray_EndParagraphIndex.Find(paraIndex);
}

/**
diff���葤�h�L�������g��text point����A�������̑Ή�����text point���擾����B
*/
ABool	ADocument_Text::SPI_GetCorrespondingTextPointByDiffTargetTextPoint( const ATextPoint& inDiffTargetTextPoint,
		ATextPoint& outTextPoint ) const
{
	//���ʏ�����
	outTextPoint.x = outTextPoint.y = 0;
	
	//���葤�h�L�������g�擾
	if( SPI_GetDiffTargetDocumentID() == kObjectID_Invalid )   return false;
	const ADocument_Text&	otherDocument = GetApp().SPI_GetTextDocumentByID(SPI_GetDiffTargetDocumentID());
	
	//����inDiffTargetTextPoint.y�̍s�ɑΉ�����i��index���擾
	AIndex	otherDocParaIndex = otherDocument.SPI_GetParagraphIndexByLineIndex(inDiffTargetTextPoint.y);
	
	//����inLineIndex�̍s���܂�ł���diff�p�[�gindex���擾
	AIndex	diffIndex = kIndex_Invalid;
	ABool	insideDiffPart = false;
	AItemCount	diffitemcount = otherDocument.mDiffDisplayArray_StartParagraphIndex.GetItemCount();
	for( AIndex i = 0; i < diffitemcount; i++ )
	{
		if( otherDocParaIndex >= otherDocument.mDiffDisplayArray_StartParagraphIndex.Get(i) &&
					otherDocParaIndex < otherDocument.mDiffDisplayArray_EndParagraphIndex.Get(i) )
		{
			diffIndex = i;
			insideDiffPart = true;
			break;
		}
		if( otherDocParaIndex < otherDocument.mDiffDisplayArray_StartParagraphIndex.Get(i) )
		{
			diffIndex = i;
			insideDiffPart = false;
			break;
		}
	}
	//�������̑Ή�����i��index�i�[�ϐ�
	AIndex	thisDocParaIndex = kIndex_Invalid;
	if( diffIndex == kIndex_Invalid )
	{
		//diff�p�[�g��������Ȃ��ꍇ�i���Ō��diff�p�[�g����j�A�ŏI�i������̒i���������킹��
		AIndex	otherDocDiffRangeStartParaIndex = 0, otherDocDiffRangeEndParaIndex = 0;
		otherDocument.SPI_GetDiffTextRangeParagraphIndex(otherDocDiffRangeStartParaIndex,otherDocDiffRangeEndParaIndex);
		thisDocParaIndex = SPI_GetParagraphCount() - (otherDocDiffRangeEndParaIndex - otherDocParaIndex);
	}
	else
	{
		//�����瑤��Diff��񂪖��ݒ莞�ɂ��̊֐����R�[�������ꍇ������̂ŁA���̏ꍇ�͂����ŏI��
		if( diffIndex >= mDiffDisplayArray_StartParagraphIndex.GetItemCount() )   return false;
		
		//�����瑤diff�p�[�g�̊J�n�i���E�I���i�����擾
		ATextIndex	thisDocStartParaIndex = mDiffDisplayArray_StartParagraphIndex.Get(diffIndex);
		ATextIndex	thisDocEndParaIndex = mDiffDisplayArray_EndParagraphIndex.Get(diffIndex);
		
		//���葤diff�p�[�g�̊J�n�i���E�I���i�����擾
		ATextIndex	otherDocStartParaIndex = otherDocument.mDiffDisplayArray_StartParagraphIndex.Get(diffIndex);
		ATextIndex	otherDocEndParaIndex = otherDocument.mDiffDisplayArray_EndParagraphIndex.Get(diffIndex);
		
		//diff�p�[�g�����ǂ����̔���
		if( insideDiffPart == true )
		{
			//------------------diff�p�[�g���̏ꍇ------------------
			//�����瑤�̑Ή�����i��index���擾
			thisDocParaIndex = thisDocStartParaIndex + (otherDocParaIndex-otherDocStartParaIndex);
			//�����瑤�ɑΉ�����s���Ȃ��idiff part�͈͊O�j�ꍇ��diff�����̍ŏI�s�ɂ���
			if( thisDocParaIndex > thisDocEndParaIndex-1 )
			{
				thisDocParaIndex = thisDocEndParaIndex-1;
			}
		}
		else
		{
			//------------------diff�p�[�g�O�̏ꍇ------------------
			//�����瑤�̑Ή�����i��index���擾
			thisDocParaIndex = thisDocStartParaIndex - (otherDocStartParaIndex-otherDocParaIndex);
		}
	}
	
	//�������̑Ή�����i��index���s���̏ꍇ��false�ŏI��
	if( thisDocParaIndex < 0 || thisDocParaIndex >= SPI_GetParagraphCount() )
	{
		return false;
	}
	
	//�������̑Ή�����i��index����Atext point�ɕϊ�
	AIndex	thisDocLineIndex = SPI_GetLineIndexByParagraphIndex(thisDocParaIndex);
	outTextPoint.x = 0;
	outTextPoint.y = thisDocLineIndex;
	return true;
}

//#0
/**
diff�̑Ώ۔͈͒i�����擾����
*/
void	ADocument_Text::SPI_GetDiffTextRangeParagraphIndex( AIndex& outStartParagraphIndex, AIndex& outEndParagraphIndex ) const
{
	if( mDiffTargetMode == kDiffTargetMode_TextWithRange )
	{
		outStartParagraphIndex = mDiffTextRangeStartParagraph;
		outEndParagraphIndex = mDiffTextRangeEndParagraph;
	}
	else
	{
		outStartParagraphIndex = 0;
		outEndParagraphIndex = SPI_GetParagraphCount();
	}
}

//#379
/**
�������Ɣ�r���s
@param inLineIndex ��r����s
@param outDiffStart ���������J�n�ʒu�i�s����index)
@param outDiffEnd ���������I���ʒu�i�s����index)
*/
void	ADocument_Text::SPI_GetCharDiffInfo( const AIndex inLineIndex, 
		AArray<AIndex>& outDiffStart, AArray<AIndex>& outDiffEnd )
{
	//���ʏ�����
	outDiffStart.DeleteAll();
	outDiffEnd.DeleteAll();
	
	//���葤�h�L�������g�擾
	if( SPI_GetDiffTargetDocumentID() == kObjectID_Invalid )   return;
	const ADocument_Text&	otherDocument = GetApp().SPI_GetTextDocumentByID(SPI_GetDiffTargetDocumentID());
	
	//����inLineIndex�̍s�ɑΉ�����i��index���擾
	AIndex	thisDocParaIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);
	
	//����inLineIndex�̍s���܂�ł���diff�p�[�gindex���擾
	AIndex	diffIndex = SPI_GetDiffIndexByIncludingLineIndex(inLineIndex);
	if( diffIndex == kIndex_Invalid )   return;
	
	//���葤��Diff��񂪖��ݒ莞�ɂ��̊֐����R�[�������ꍇ������̂ŁA���̏ꍇ�͂����ŏI��
	if( diffIndex >= otherDocument.mDiffDisplayArray_StartParagraphIndex.GetItemCount() )   return;
	
	//�����瑤diff�p�[�g�̊J�n�i���E�I���i�����擾
	ATextIndex	thisDocStartParaIndex = mDiffDisplayArray_StartParagraphIndex.Get(diffIndex);
	
	//���葤diff�p�[�g�̊J�n�i���E�I���i�����擾
	ATextIndex	otherDocStartParaIndex = otherDocument.mDiffDisplayArray_StartParagraphIndex.Get(diffIndex);
	ATextIndex	otherDocEndParaIndex = otherDocument.mDiffDisplayArray_EndParagraphIndex.Get(diffIndex);
	
	//���葤�ɑΉ�����s���Ȃ��ꍇ�͏I��
	if( thisDocParaIndex-thisDocStartParaIndex >= otherDocEndParaIndex-otherDocStartParaIndex )   return;
	
	//���葤�̑Ή�����i��index���擾
	AIndex	otherDocParaIndex = otherDocStartParaIndex + (thisDocParaIndex-thisDocStartParaIndex);
	
	//------------------�����܂łœ����J�n�i���E�I���i����thidDoc/otherDoc���ƂȂ̂ŁAworking/diffTarget�ɕϊ�����------------------
	AIndex	workingParaIndex = kIndex_Invalid;
	AIndex	diffParaIndex = kIndex_Invalid;
	ADocumentID	workingDocumentID = kObjectID_Invalid;
	ADocumentID	diffTargetDocumentID = kObjectID_Invalid;
	if( mTextDocumentType == kTextDocumentType_DiffTarget )
	{
		//DiffTarget�h�L�������g���̏ꍇ
		workingParaIndex = otherDocParaIndex;
		diffParaIndex = thisDocParaIndex;
		workingDocumentID = SPI_GetDiffTargetDocumentID();
		diffTargetDocumentID = GetObjectID();
	}
	else
	{
		//�ʏ�h�L�������g���̏ꍇ
		workingParaIndex = thisDocParaIndex;
		diffParaIndex = otherDocParaIndex;
		workingDocumentID = GetObjectID();
		diffTargetDocumentID = SPI_GetDiffTargetDocumentID();
	}
	const ADocument_Text&	diffTargetDocument = GetApp().SPI_GetTextDocumentByID(diffTargetDocumentID);
	const ADocument_Text&	workingDocument = GetApp().SPI_GetTextDocumentByID(workingDocumentID);
	
	//working���̒i���J�nTextIndex�A�s�̒������擾
	ATextIndex	workingParaStartTextIndex = workingDocument.SPI_GetParagraphStartTextIndex(workingParaIndex);
	ATextIndex	workingParaEndTextIndex = workingDocument.SPI_GetParagraphStartTextIndex(workingParaIndex+1);// -1;
	//Diff�Ώۑ��̒i���J�nTextIndex���擾
	ATextIndex	diffParaStartTextIndex = diffTargetDocument.SPI_GetParagraphStartTextIndex(diffParaIndex);
	ATextIndex	diffParaEndTextIndex = diffTargetDocument.SPI_GetParagraphStartTextIndex(diffParaIndex+1);// -1;
	//working, diff���ꂼ��̌��݈ʒu
	AIndex	workingPos = workingParaStartTextIndex;
	AIndex	diffPos = diffParaStartTextIndex;
	//------------------���[�v�i��{�I��working���̑Ώۍs�̊e�����ɂ��Ĕ�r���s���s��v�̂Ƃ��͎��ɓ�������ꏊ�������j------------------
	AStTextPtr	workingTextPtr(workingDocument.mText,0,workingDocument.mText.GetItemCount());
	AStTextPtr	diffTextPtr(diffTargetDocument.mText,0,diffTargetDocument.mText.GetItemCount());
	for( ;workingPos < workingParaEndTextIndex; )
	{
		//Diff���ɑΉ��������Ȃ���ΏI��
		if( diffPos >= diffTargetDocument.mText.GetItemCount() )   break;
		//working�������擾
		AUCS4Char	workingChar = 0;
		ATextIndex	s = workingPos;
		ATextIndex	e = workingPos + AText::Convert1CharToUCS4(workingTextPtr.GetPtr(),workingTextPtr.GetByteCount(),workingPos,workingChar);
		//Diff�������ʒu�擾
		AUCS4Char	diffChar = 0;
		ATextIndex	ds = diffPos;
		ATextIndex	de = diffPos + AText::Convert1CharToUCS4(diffTextPtr.GetPtr(),diffTextPtr.GetByteCount(),diffPos,diffChar);
		//��r
		if( workingChar != diffChar )
		{
			//------------------�����s��v��------------------
			//����̕����ȍ~�ŁA���ɓ�������ꏊ����������
			ABool	resync = false;
			for( AIndex pos = s; pos < workingParaEndTextIndex; )
			{
				//
				AIndex	epos = pos + 8;
				if( epos > workingParaEndTextIndex )
				{
					epos = workingParaEndTextIndex;
				}
				epos = workingDocument.mText.GetCurrentCharPos(epos);
				AText	text;
				workingDocument.SPI_GetText(pos,epos,text);
				//
				AIndex	diffResyncPos = kIndex_Invalid;
				if( diffTargetDocument.mText.FindText(ds,text,diffResyncPos,diffParaEndTextIndex) == true )
				{
					if( mTextDocumentType == kTextDocumentType_DiffTarget )
					{
						outDiffStart.Add(ds);
						outDiffEnd.Add(diffResyncPos);
					}
					else
					{
						outDiffStart.Add(s);
						outDiffEnd.Add(pos);
					}
					//���̈ʒu
					workingPos = pos;
					diffPos = diffResyncPos;
					//�O�̂���
					if( workingPos <= s && diffPos <= ds )
					{
						workingPos = e;
						diffPos = de;
					}
					//
					resync = true;
					break;
				}
				//
				pos = workingDocument.mText.GetNextCharPos(pos);
			}
			//
			if( resync == false )
			{
				if( mTextDocumentType == kTextDocumentType_DiffTarget )
				{
					outDiffStart.Add(ds);
					outDiffEnd.Add(diffParaEndTextIndex);
				}
				else
				{
					outDiffStart.Add(s);
					outDiffEnd.Add(workingParaEndTextIndex);
				}
				break;
			}
		}
		else
		{
			//------------------������v��------------------
			//���̈ʒu
			workingPos = e;
			diffPos = de;
		}
	}
}

//#379
/**
Diff���s
*/
void	ADocument_Text::ExecuteDiff()
{
	//Diff�p�X�E�I�v�V�����擾
	AText	diffExeFilePath;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffPath,diffExeFilePath);
	AText	diffOption;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffOption,diffOption);
	
#if IMPLEMENTATION_FOR_WINDOWS
	//���b�� Windows�ł�Diff�p�X�Œ�i�A�v���ɕt����diff.exe�����s�j
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	AFileAcc	diffExeFile;
	diffExeFile.SpecifySister(appFile,"diff\\diff.exe");
	diffExeFile.GetPath(diffExeFilePath);
#endif
	
	//diff�R�}���h�p�X
	AText	command;
	command.SetText(diffExeFilePath);
	//����
	ATextArray	argArray;
	argArray.Add(command);
	//Unified Diff�ݒ�
	AText	t;
	t.SetCstring("-u");
	argArray.Add(t);
	//Diff�I�v�V����
	if( diffOption.GetItemCount() > 0 )
	{
		argArray.Add(diffOption);
	}
	//�^�[�Q�b�g���[�h���̏���
	//������̏ꍇ��diff��r�e���|�����t�H���_�𐶐����Adiff�Ώۃh�L�������g�̃t�@�C���ƁA���h�L�������g�̃t�@�C���ɏ�������Ńt�@�C���ԂŔ�r
	AText	dirpath;
	switch(mDiffTargetMode)
	{
	  case kDiffTargetMode_RepositoryLatest:
		{
			//==================���|�W�g���Ɣ�r==================
			//diff��r�e���|�����t�H���_�������Ȃ琶��
			if( mDiffTempFolder.IsSpecified() == false || mDiffTempFolder.Exist() == false )
			{
				GetApp().SPI_GetUniqTempFolder(mDiffTempFolder);
				mDiffTempFolder.CreateFolder();
			}
			//------------Diff�Ώۃt�@�C���𐶐��A�����ɐݒ�------------
			//diff�Ώۃh�L�������g����e�L�X�g���擾���ALF�֕ϊ�����
			AText	diffTargetText;
			GetApp().SPI_GetTextDocumentConstByID(mRepositoryDocumentID).SPI_GetText(diffTargetText);
			diffTargetText.ConvertLineEndToLF();
			//�e���|�����t�@�C���ɕۑ�
			AFileAcc	targetFile;
			targetFile.SpecifyChild(mDiffTempFolder,"difftargetfile");
			targetFile.CreateFile();
			targetFile.WriteFile(diffTargetText);
			//�����ɐݒ�
			AText	targetFilePath;
			targetFile.GetPath(targetFilePath);
			argArray.Add(targetFilePath);
			//------------���̃h�L�������g�̃t�@�C��------------
			//���h�L�������g�̃e�L�X�g���擾���ALF�֕ϊ�
			AText	text;
			text.SetText(mText);
			text.ConvertLineEndToLF();
			//�e���|�����t�@�C���ɕۑ�
			AFileAcc	workingFile;
			workingFile.SpecifyChild(mDiffTempFolder,"workingfile");
			workingFile.CreateFile();
			workingFile.WriteFile(text);
			//�����ɐݒ�
			AText	workingFilePath;
			workingFile.GetPath(workingFilePath);
			argArray.Add(workingFilePath);
			//------------�J�����g�f�B���N�g��------------
			AText	dirpath;
			mDiffTempFolder.GetNormalizedPath(dirpath);
			break;
		}
	  case kDiffTargetMode_File:
		{
			//diff��r�e���|�����t�H���_�������Ȃ琶��
			if( mDiffTempFolder.IsSpecified() == false || mDiffTempFolder.Exist() == false )
			{
				GetApp().SPI_GetUniqTempFolder(mDiffTempFolder);
				mDiffTempFolder.CreateFolder();
			}
			//------------Diff�Ώۃt�@�C��------------
			//diff�Ώۃh�L�������g����e�L�X�g���擾���ALF�֕ϊ�����
			AText	diffTargetText;
			GetApp().SPI_GetTextDocumentConstByID(mDiffTargetFileDocumentID).SPI_GetText(diffTargetText);
			diffTargetText.ConvertLineEndToLF();
			//�e���|�����t�@�C���ɕۑ�
			AFileAcc	targetFile;
			targetFile.SpecifyChild(mDiffTempFolder,"difftargetfile");
			targetFile.CreateFile();
			targetFile.WriteFile(diffTargetText);
			//�����ɐݒ�
			AText	targetFilePath;
			targetFile.GetPath(targetFilePath);
			argArray.Add(targetFilePath);
			//------------���̃h�L�������g�̃t�@�C��------------
			//���h�L�������g�̃e�L�X�g���擾���ALF�֕ϊ�
			AText	text;
			text.SetText(mText);
			text.ConvertLineEndToLF();
			//�e���|�����t�@�C���ɕۑ�
			AFileAcc	workingFile;
			workingFile.SpecifyChild(mDiffTempFolder,"workingfile");
			workingFile.CreateFile();
			workingFile.WriteFile(text);
			//�����ɐݒ�
			AText	workingFilePath;
			workingFile.GetPath(workingFilePath);
			argArray.Add(workingFilePath);
			//------------�J�����g�f�B���N�g��------------
			AText	dirpath;
			mDiffTempFolder.GetNormalizedPath(dirpath);
			break;
		}
		//#192
	  case kDiffTargetMode_TextWithRange:
		{
			//diff��r�e���|�����t�H���_�������Ȃ琶��
			if( mDiffTempFolder.IsSpecified() == false || mDiffTempFolder.Exist() == false )
			{
				GetApp().SPI_GetUniqTempFolder(mDiffTempFolder);
				mDiffTempFolder.CreateFolder();
			}
			//------------Diff�Ώۃt�@�C��------------
			//diff�Ώۃh�L�������g����e�L�X�g���擾���ALF�֕ϊ�����
			AText	diffTargetText;
			GetApp().SPI_GetTextDocumentConstByID(mDiffTargetFileDocumentID).SPI_GetText(diffTargetText);
			diffTargetText.ConvertLineEndToLF();
			//�e���|�����t�@�C���ɕۑ�
			AFileAcc	targetFile;
			targetFile.SpecifyChild(mDiffTempFolder,"difftargetfile");
			targetFile.CreateFile();
			targetFile.WriteFile(diffTargetText);
			//�����ɐݒ�
			AText	targetFilePath;
			targetFile.GetPath(targetFilePath);
			argArray.Add(targetFilePath);
			//------------���̃h�L�������g�̃t�@�C��------------
			//�J�n�E�I��text index�擾
			AIndex	start = mDiffTextRangeStart;
			if( start > mText.GetItemCount() )
			{
				start = mText.GetItemCount();
			}
			start = mText.GetCurrentCharPos(start);
			AIndex	end = mDiffTextRangeEnd;
			if( end > mText.GetItemCount() )
			{
				end = mText.GetItemCount();
			}
			end = mText.GetCurrentCharPos(end);
			//�J�n�i��index�擾
			ATextPoint	startpt = {0};
			SPI_GetTextPointFromTextIndex(start,startpt);
			mDiffTextRangeStartParagraph = SPI_GetParagraphIndexByLineIndex(startpt.y);
			//�I���i��index�擾
			ATextPoint	endpt = {0};
			SPI_GetTextPointFromTextIndex(end,endpt);
			mDiffTextRangeEndParagraph = SPI_GetParagraphIndexByLineIndex(endpt.y);
			//���h�L�������g�̃e�L�X�g���擾���ALF�֕ϊ�
			AText	text;
			mText.GetText(start,end-start,text);
			text.ConvertLineEndToLF();
			//�e���|�����t�@�C���ɕۑ�
			AFileAcc	workingFile;
			workingFile.SpecifyChild(mDiffTempFolder,"workingfile");
			workingFile.CreateFile();
			workingFile.WriteFile(text);
			//�����ɐݒ�
			AText	workingFilePath;
			workingFile.GetPath(workingFilePath);
			argArray.Add(workingFilePath);
			//------------�J�����g�f�B���N�g��------------
			AText	dirpath;
			mDiffTempFolder.GetNormalizedPath(dirpath);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//���s
	GetApp().SPI_GetChildProcessManager().ExecuteGeneralAsyncCommand(command,argArray,dirpath,
				GetObjectID(),kGeneralAsyncCommandType_Diff);
}

//#379
/**
�񓯊��R�}���h���ʎ��s
*/
void	ADocument_Text::NVIDO_DoGeneralAsyncCommandResult( const AGeneralAsyncCommandType inType, const AText& inText )
{
	switch(inType)
	{
	  case kGeneralAsyncCommandType_Diff:
		{
			AText	resulttext;
			resulttext.SetText(inText);
			
			//CR�֕ϊ��i�e�L�X�g�G���R�[�f�B���O��UTF8�Œ�B�iExecuteDiff()�ŁAUTF8/LF�Ŕ�r�p�e���|�����t�@�C���ɏ�������ł��邽�߁B�j�j�j
			resulttext.ConvertReturnCodeToCR();
			
			//
			AArray<ADiffType>	diffTypeArray;
			AArray<AIndex>	startParagraphIndexArray;
			AArray<AIndex>	endParagraphIndexArray;
			AArray<AIndex>	oldStartParagraphIndexArray;
			AArray<AIndex>	oldEndParagraphIndexArray;
			ATextArray	deletedTextArray;
			
			//DiffText���
			ParseDiffText(resulttext,diffTypeArray,startParagraphIndexArray,endParagraphIndexArray,
						oldStartParagraphIndexArray,oldEndParagraphIndexArray,deletedTextArray);
			
			//#192
			//�e�L�X�g�͈͔�r�̏ꍇ�́A�i��index��␳
			if( mDiffTargetMode == kDiffTargetMode_TextWithRange )
			{
				AItemCount	itemCount = startParagraphIndexArray.GetItemCount();
				for( AIndex i = 0; i < itemCount; i++ )
				{
					startParagraphIndexArray.Set(i,startParagraphIndexArray.Get(i)+mDiffTextRangeStartParagraph);
					endParagraphIndexArray.Set(i,endParagraphIndexArray.Get(i)+mDiffTextRangeStartParagraph);
				}
			}
			
			//DiffArray�̐ݒ菈�����s���i#379 �ȑO�����ɂ�����������SPI_SetDiffArray()�ֈړ��j
			SetDiffArray(diffTypeArray,startParagraphIndexArray,endParagraphIndexArray,
						oldStartParagraphIndexArray,oldEndParagraphIndexArray,deletedTextArray,true);
			
			//���]Diff�ݒ�
			ADocumentID	targetDocID = kObjectID_Invalid;
			switch(mDiffTargetMode)
			{
				//
			  case kDiffTargetMode_RepositoryLatest:
				{
					targetDocID = mRepositoryDocumentID;
					break;
				}
			  case kDiffTargetMode_File:
			  case kDiffTargetMode_TextWithRange://#192
				{
					targetDocID = mDiffTargetFileDocumentID;
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			if( targetDocID != kObjectID_Invalid )
			{
				SetReverseDiffToDiffTargetDocument(targetDocID,diffTypeArray,startParagraphIndexArray,endParagraphIndexArray,
												   oldStartParagraphIndexArray,oldEndParagraphIndexArray);
			}
			
			//�e�E�C���h�E��Diff�\�����X�V
			GetApp().SPI_UpdateDiffDisplay(GetObjectID());
			
			//Diff���̃f�[�^�쐬��łȂ��ƁA��������Diff���쐬�ł��Ȃ��̂ŁA������xTextView���X�V����B
			SPI_RefreshTextViews();
			
			//���j���[�X�V�i�C�x���g���痈�Ă��Ȃ��̂ł����Ŏ��s����K�v������j
			AApplication::GetApplication().NVI_UpdateMenu();
			
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

//#379
/**
Diff��ԃe�L�X�g�擾�i�T�u�e�L�X�g�̏�̃{�^���ɕ\���j
*/
void	ADocument_Text::SPI_GetDiffStatusText( AText& outText ) const
{
	switch(mCompareMode)
	{
		//
	  case kCompareMode_Repository:
		{
			outText.SetLocalizedText("SubPaneDiffButton_Repository");
			AText	diffCountText;
			diffCountText.SetNumber(SPI_GetDiffCount());
			outText.ReplaceParamText('0',diffCountText);
			break;
		}
	  case kCompareMode_File:
		{
			outText.SetLocalizedText("SubPaneDiffButton_File");
			AText	path;
			mDiffTargetFile.GetPath(path);
			outText.ReplaceParamText('0',path);
			AText	diffCountText;
			diffCountText.SetNumber(SPI_GetDiffCount());
			outText.ReplaceParamText('1',diffCountText);
			break;
		}
	  case kCompareMode_AutoBackupMinutely:
	  case kCompareMode_AutoBackupDaily:
	  case kCompareMode_AutoBackupHourly:
	  case kCompareMode_AutoBackupMonthly:
		{
			//�����o�b�N�A�b�v�Ƃ̔�r�̏ꍇ
			outText.SetLocalizedText("SubPaneDiffButton_AutoBackup");
			AText	filename;
			mDiffTargetFile.GetFilename(filename);
			AText	menutext;
			GetAutoBackupCompareMenuText(filename.GetNumber64bit(),menutext);
			outText.ReplaceParamText('0',menutext);
			AText	diffCountText;
			diffCountText.SetNumber(SPI_GetDiffCount());
			outText.ReplaceParamText('1',diffCountText);
			break;
		}
	  case kCompareMode_Clipboard:
		{
			//�N���b�v�{�[�h�Ƃ̔�r�̏ꍇ
			outText.SetLocalizedText("SubPaneDiffButton_Clipboard");
			AText	diffCountText;
			diffCountText.SetNumber(SPI_GetDiffCount());
			outText.ReplaceParamText('0',diffCountText);
			break;
		}
	  case kCompareMode_UnsavedData:
		{
			//���ۑ��f�[�^�Ƃ̔�r�̏ꍇ
			outText.SetLocalizedText("SubPaneDiffButton_UnsavedData");
			AText	diffCountText;
			diffCountText.SetNumber(SPI_GetDiffCount());
			outText.ReplaceParamText('0',diffCountText);
			break;
		}
	  case kCompareMode_NoCompare:
		{
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

//#379
/**
�e���[�h���Ƃ�Diff�ێ擾
*/
/*#81
ABool	ADocument_Text::SPI_GetDiffAvailability( const ADiffTargetMode inDiffTargetMode, AFileAcc& outFile ) const
{
	ABool	result = false;
	switch(inDiffTargetMode)
	{
		//
	  case kDiffTargetMode_RepositoryLatest:
		{
			if( NVI_IsFileSpecified() == true )
			{
				AFileAcc	file;
				NVI_GetFile(file);
				//#568 if( GetApp().SPI_GetSCMFolderManager().GetFileState(file) != kSCMFileState_NotSCMFolder )
				if( mSCMFileState != kSCMFileState_NotSCMFolder && mSCMFileState != kSCMFileState_NotEntried )//#568
				{
					result = true;
				}
			}
			break;
		}
	  case kDiffTargetMode_File:
		{
			AText	path;
			if( NVI_IsFileSpecified() == true )
			{
				AFileAcc	file;
				NVI_GetFile(file);
				file.GetPath(path);
				//Diff��r��t�H���_�ݒ�擾
				AText	diffSourceFolderWorkingPath;
				GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffSourceFolder_Working,diffSourceFolderWorkingPath);
				//���̃h�L�������g�̃t�@�C����Diff��r��t�H���_���̃t�@�C�����ǂ����𔻒�
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDiffSourceFolder_Enable) == true &&
							diffSourceFolderWorkingPath.GetItemCount() > 0 && path.GetItemCount() > 0 && 
							path.GetItemCount() > diffSourceFolderWorkingPath.GetItemCount() && 
							diffSourceFolderWorkingPath.CompareMin(path) == true )
				{
					//���΃p�X���擾
					AText	relativePath;
					relativePath.InsertText(0,path,diffSourceFolderWorkingPath.GetItemCount(),
							path.GetItemCount()-diffSourceFolderWorkingPath.GetItemCount());
					//Diff��r���t�H���_�擾
					AText	diffSourceFolderPath;
					GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffSourceFolder_Source,diffSourceFolderPath);
					AFileAcc	diffSourceFolder;
					diffSourceFolder.Specify(diffSourceFolderPath);
					//Diff��r���t�@�C���擾
					outFile.SpecifyChild(diffSourceFolder,relativePath);
					//�t�@�C�������݂��Ă��邩�ǂ�������
					if( outFile.Exist() == true )
					{
						result = true;
					}
				}
			}
			break;
		}
	}
	return result;
}
*/
//#379
/**
Diff�Ώۃt�@�C�����X�V�iDiff�ݒ�E�C���h�E�Ńt�H���_�ݒ�X�V���j
*/
/*#81
ABool	ADocument_Text::SPI_UpdateDiffFileTarget()
{
	ABool	result = false;
	AFileAcc	diffFile;
	if( SPI_GetDiffAvailability(kDiffTargetMode_File,diffFile) == true )
	{
		//�t�@�C����r���[�h
		mDiffTargetMode = kDiffTargetMode_File;
		//Diff�Ώۃt�@�C���ݒ�
		SPI_SetDiffTargetFile(diffFile);
		//
		result = true;
	}
	else if( SPI_GetDiffAvailability(kDiffTargetMode_RepositoryLatest,diffFile) == true )
	{
		//���|�W�g����r���[�h
		mDiffTargetMode = kDiffTargetMode_RepositoryLatest;
		//Diff�f�[�^�X�V
		SPI_UpdateDiffDisplayData();
		//
		result = true;
	}
	//
	else
	{
		ClearDiffDisplay();
		//
		mDiffTargetMode = kDiffTargetMode_None;
	}
	return result;
}
*/
/*
UnifiedDiff�`��
Index: AApp.cpp
===================================================================
--- AApp.cpp	(revision 2247)
+++ AApp.cpp	(working copy)
@@ -5632,7 +5649,7 @@
 	SPI_GetMultiFileFindWindow().NVI_SwitchFocusToFirst();
 }
 
-//�u�ŋߎg����?�v�ɒǉ�
+//�u�ŋߎg����?�v�ɒǉ�
 void	AApp::SPI_AddRecentlyUsed( const AText& inFindText, const AFileAcc& inFolder )
 {
 	SPI_GetMultiFileFindWindow().SPI_AddRecentlyUsedFindText(inFindText);

��diff�`��
Index: AView_EditBox.cpp
===================================================================
70a71
> 	GetTextDocument().NVI_RegisterView(GetObjectID());
141c142,143
< 	AApplication::GetApplication().NVI_DeleteDocument(mDocumentID);
---
> 	//#379 AApplication::GetApplication().NVI_DeleteDocument(mDocumentID);
> 	GetTextDocument().NVI_UnregisterView(GetObjectID());
*/
//#379
/**
*/
void	ADocument_Text::ParseDiffText( const AText& inText,
		AArray<ADiffType>& diffTypeArray,
		AArray<AIndex>& startParagraphIndexArray, AArray<AIndex>& endParagraphIndexArray,
		AArray<AIndex>& oldStartParagraphIndexArray, AArray<AIndex>& oldEndParagraphIndexArray,
		ATextArray& deletedTextArray )
{
	//#379 �����e�L�X�g���e�Ō`�����f����悤�ɕύXif( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseExternalDiffForSubversion) == false )//#335
	ABool	unifiedDiffFormat = false;
	AIndex	pos = 0;
	AText	line;
	AItemCount	lineCount = 0;
	for( ; pos < inText.GetItemCount(); )
	{
		inText.GetLine(pos,line);
		if( line.GetItemCount() == 0 )   continue;
		if( line.Get(0) == '@' )
		{
			//@�Ŏn�܂�s�������UnifiedDiff�`���Ƃ݂Ȃ�
			unifiedDiffFormat = true;
			break;
		}
		//�ŏ�����16�s�ڂ܂ł��`�F�b�N
		lineCount++;
		if( lineCount > 16 )   break;
	}
	//�`���ʂɔ�r���e�擾
	if( unifiedDiffFormat == true )
	{
		//Unified Diff�`�� #335
		
		//@�Ŏn�܂�ŏ��̍s������
		AIndex	pos = 0;
		AText	line;
		for( ; pos < inText.GetItemCount(); )
		{
			inText.GetLine(pos,line);
			if( line.GetItemCount() == 0 )   continue;
			if( line.Get(0) == '@' )   break;
		}
		//�e�ύX�_���Ƃ̃��[�v
		for( ; pos < inText.GetItemCount(); )
		{
			//�����ɂ����Ƃ��ɂ́A���Ȃ炸@@�s��line�ɓǂݍ��񂾎��_�i@@�s�̎��̍s��pos������j
			AIndex	new_startlineindex = 0;//�ύX�㑤�̊J�n�s
			AIndex	new_linecount = 1;//�ύX�㑤�̍s��
			AIndex	old_startlineindex = 0;//�ύX�O���̊J�n�s #379
			AIndex	old_linecount = 1;//�ύX�O���̍s�� #379
			//@@�s�̉��
			//��F@@ -5632,7 +5649,7 @@
			for( AIndex linepos = 1; linepos < line.GetItemCount(); linepos++ )
			{
				AUChar	ch = line.Get(linepos);
				//�ύX�O���J�n�s�E�s���擾 #379
				if( ch == '-' )
				{
					linepos++;
					if( line.ParseIntegerNumber(linepos,old_startlineindex,false) == false )   {_ACError("",NULL);return;}
					old_startlineindex--;//�s�ԍ���1�͂��܂�Ȃ̂ŁA0�͂��܂�ɂ���
					if( old_startlineindex < 0 )   old_startlineindex = 0;//��h�L�������g����0,0�ɂȂ邽�߁B
					if( line.Get(linepos) == ',' )
					{
						linepos++;
						line.ParseIntegerNumber(linepos,old_linecount,false);
					}
				}
				//�ύX�㑤�J�n�s�E�s���擾
				if( ch == '+' )
				{
					linepos++;
					if( line.ParseIntegerNumber(linepos,new_startlineindex,false) == false )   {_ACError("",NULL);return;}
					new_startlineindex--;//�s�ԍ���1�͂��܂�Ȃ̂ŁA0�͂��܂�ɂ���
					if( new_startlineindex < 0 )   new_startlineindex = 0;//��h�L�������g����0,0�ɂȂ邽�߁B
					if( line.Get(linepos) == ',' )
					{
						linepos++;
						line.ParseIntegerNumber(linepos,new_linecount,false);
					}
					break;
				}
			}
			//�ύX���e�����̉��
			//+�̃u���b�N�A-�̃u���b�N����ڍs����Ƃ��ɁA�����s�i�[�������s��
			AIndex	new_lineindex = new_startlineindex;//working���̌��ݍs���i�[
			AIndex	old_lineindex = old_startlineindex;//#379 repos���̌��ݍs���i�[
			AItemCount	pluscount = 0;
			AItemCount	minuscount = 0;
			AText	deletedText;//#379
			for( ; pos < inText.GetItemCount(); )
			{
				//1�s�擾
				inText.GetLine(pos,line);
                if( line.GetItemCount() == 0 )   {_ACError("",NULL);return;}
				AUChar	ch = line.Get(0);
				//@�s�Ȃ炱�̕ύX���e�������[�v�I��
				if( ch == '@' )   break;
				//+�u���b�N�I��
				if( ch != '+' && pluscount > 0 )
				{
					if( minuscount > 0 )
					{
						//-�u���b�N����A+�u���b�N�I���Ȃ̂ŁA�u�ύX�v
						diffTypeArray.Add(kDiffType_Changed);
						startParagraphIndexArray.Add(new_lineindex-pluscount);
						endParagraphIndexArray.Add(new_lineindex);
						//#379 ���|�W�g������Start,End�i���ݒ�
						oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
						oldEndParagraphIndexArray.Add(old_lineindex);
						//�폜�e�L�X�g�ǉ�
						deletedTextArray.Add(deletedText);//#379
					}
					else
					{
						//-�u���b�N�Ȃ��A+�u���b�N�I���Ȃ̂ŁA�u�ǉ��v
						diffTypeArray.Add(kDiffType_Added);
						startParagraphIndexArray.Add(new_lineindex-pluscount);
						endParagraphIndexArray.Add(new_lineindex);
						//#379 ���|�W�g������Start,End�i���ݒ�
						oldStartParagraphIndexArray.Add(old_lineindex);
						oldEndParagraphIndexArray.Add(old_lineindex+1);
						//�폜�e�L�X�g�ǉ�
						deletedTextArray.Add(GetEmptyText());//#379
					}
					pluscount = 0;
					minuscount = 0;
					deletedText.DeleteAll();
				}
				//-�u���b�N�I���i�������A����+�̏ꍇ�́Achanged�Ȃ̂ŉ������Ȃ��B
				if( ch == ' ' && minuscount > 0 )
				{
					//-�u���b�N�I�����ύX�Ȃ��u���b�N�Ȃ̂ŁA�u�폜�v
					diffTypeArray.Add(kDiffType_Deleted);
					startParagraphIndexArray.Add(new_lineindex);
					endParagraphIndexArray.Add(new_lineindex+1);
					//#379 ���|�W�g������Start,End�i���ݒ�
					oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
					oldEndParagraphIndexArray.Add(old_lineindex);
					//�폜�e�L�X�g�ǉ�
					deletedTextArray.Add(deletedText);//#379
					minuscount = 0;
					deletedText.DeleteAll();
				}
				//�폜�e�L�X�g�L�� #379
				if( ch == '-' )
				{
					AText	text;
					text.SetText(line);
					text.Delete(0,1);
					text.Add(kUChar_LineEnd);
					deletedText.AddText(text);
				}
				//+, -�J�E���g�A�s���C���N�������g
				switch(ch)
				{
				  case '+':
					{
						pluscount++;
						new_lineindex++;
						break;
					}
				  case '-':
					{
						minuscount++;
						old_lineindex++;//#379
						break;
					}
				  case ' ':
					{
						new_lineindex++;
						old_lineindex++;//#379
						break;
					}
				  default:
					{
						//���Ƃ肠�����R�����g�A�E�g���Ă����B�����ɗ���ꍇ�̑΍��K�v�B�i�h�L�������g�Ō�Ŕ������邱�Ƃ�����H�j_ACError("",NULL);
						break;
					}
				}
			}
			//�ύX���e�������[�v�I������
			//+, -�̃J�E���g�c�肪����Ώ�������
			if( pluscount > 0 )
			{
				if( minuscount > 0 )
				{
					//-�u���b�N����A+�u���b�N�I���Ȃ̂ŁA�u�ύX�v
					diffTypeArray.Add(kDiffType_Changed);
					startParagraphIndexArray.Add(new_lineindex-pluscount);
					endParagraphIndexArray.Add(new_lineindex);
					//#379 ���|�W�g������Start,End�i���ݒ�
					oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
					oldEndParagraphIndexArray.Add(old_lineindex);
					//�폜�e�L�X�g�ǉ�
					deletedTextArray.Add(deletedText);//#379
				}
				else
				{
					//-�u���b�N�Ȃ��A+�u���b�N�I���Ȃ̂ŁA�u�ǉ��v
					diffTypeArray.Add(kDiffType_Added);
					startParagraphIndexArray.Add(new_lineindex-pluscount);
					endParagraphIndexArray.Add(new_lineindex);
					//#379 ���|�W�g������Start,End�i���ݒ�
					oldStartParagraphIndexArray.Add(old_lineindex);
					oldEndParagraphIndexArray.Add(old_lineindex+1);
					//�폜�e�L�X�g�ǉ�
					deletedTextArray.Add(GetEmptyText());//#379
				}
				pluscount = 0;
				minuscount = 0;
			}
			if( minuscount > 0 )
			{
				//-�u���b�N�Ȃ��I���Ȃ̂ŁA�u�폜�v
				diffTypeArray.Add(kDiffType_Deleted);
				startParagraphIndexArray.Add(new_lineindex);
				endParagraphIndexArray.Add(new_lineindex+1);
				//#379 ���|�W�g������Start,End�i���ݒ�
				oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
				oldEndParagraphIndexArray.Add(old_lineindex);
				//�폜�e�L�X�g�ǉ�
				deletedTextArray.Add(deletedText);//#379
				minuscount = 0;
			}
		}
	}
	else
	{
		//��diff�`��
		for( AIndex pos = 0; pos < inText.GetItemCount(); )
		{
			AText	line;
			inText.GetLine(pos,line);
			if( line.GetItemCount() == 0 )   continue;
			AUChar	ch = line.Get(0);
			if( ch >= '0' && ch <= '9' )
			{
				AIndex	linepos = 0;
				ANumber	oldsnum = 0, oldenum = 0;
				if( line.ParseIntegerNumber(linepos,oldsnum,false) == false )   continue;
				if( linepos >= line.GetItemCount() )   continue;
				ch = line.Get(linepos);
				linepos++;
				if( ch == ',' )
				{
					if( line.ParseIntegerNumber(linepos,oldenum,false) == false )   continue;
					if( linepos >= line.GetItemCount() )   continue;
					ch = line.Get(linepos);
					linepos++;
				}
				if( oldenum == 0 )   oldenum = oldsnum;//#379
				//
				switch(ch)
				{
				  case 'a':
					{
						ANumber	newsnum = 0, newenum = 0;
						if( line.ParseIntegerNumber(linepos,newsnum,false) == false )   continue;
						if( linepos < line.GetItemCount() )
						{
							if( line.Get(linepos) == ',' )
							{
								linepos++;
								line.ParseIntegerNumber(linepos,newenum,false);
							}
						}
						if( newenum == 0 )   newenum = newsnum;
						
						diffTypeArray.Add(kDiffType_Added);
						startParagraphIndexArray.Add(newsnum-1);
						endParagraphIndexArray.Add(newenum+1-1);
						//#379 ���|�W�g������Start,End�i���ݒ�
						oldStartParagraphIndexArray.Add(oldsnum-1+1);
						oldEndParagraphIndexArray.Add(oldenum+1-1+1);
						//#379 �ǉ��s�Ȃ̂ō폜�e�L�X�g�͂Ȃ�
						deletedTextArray.Add(GetEmptyText());
						break;
					}
				  case 'c':
					{
						ANumber	newsnum = 0, newenum = 0;
						if( line.ParseIntegerNumber(linepos,newsnum,false) == false )   continue;
						if( linepos < line.GetItemCount() )
						{
							if( line.Get(linepos) == ',' )
							{
								linepos++;
								line.ParseIntegerNumber(linepos,newenum,false);
							}
						}
						if( newenum == 0 )   newenum = newsnum;
						
						diffTypeArray.Add(kDiffType_Changed);
						startParagraphIndexArray.Add(newsnum-1);
						endParagraphIndexArray.Add(newenum+1-1);
						//#379 ���|�W�g������Start,End�i���ݒ�
						oldStartParagraphIndexArray.Add(oldsnum-1);
						oldEndParagraphIndexArray.Add(oldenum+1-1);
						//#379 �폜�e�L�X�g�擾
						AText	deletedText;
						for( AIndex p = pos; p < inText.GetItemCount(); )
						{
							AText	l;
							inText.GetLine(p,l);
							if( l.GetItemCount() < 2 )   continue;
							if( l.Get(0) != '<' )   break;
							l.Delete(0,2);
							deletedText.AddText(l);
							deletedText.Add(kUChar_LineEnd);
						}
						deletedTextArray.Add(deletedText);
						break;
					}
				  case 'd':
					{
						ANumber	newsnum = 0, newenum = 0;
						if( line.ParseIntegerNumber(linepos,newsnum,false) == false )   continue;
						if( linepos < line.GetItemCount() )
						{
							if( line.Get(linepos) == ',' )
							{
								linepos++;
								line.ParseIntegerNumber(linepos,newenum,false);
							}
						}
						if( newenum == 0 )   newenum = newsnum;
						
						diffTypeArray.Add(kDiffType_Deleted);
						//'d'�̏ꍇ�A�\�����ꂽ�i���ԍ��̌�ɍ폜�s�����邱�Ƃ��Ӗ�����B
						//newsnum==0�����肤��B'a'��'c'�Ɠ�����newsnum-1���i�[���Ă��܂��ƁA-1���i�[����āA�����̓s���������B
						//����āA�i�[���ꂽ�i���ԍ��̑O�ɍ폜�s�����邱�Ƃ��Ӗ�����悤�ɂ���B
						startParagraphIndexArray.Add(newsnum-1+1);
						endParagraphIndexArray.Add(newenum+1-1+1);
						//#379 ���|�W�g������Start,End�i���ݒ�
						oldStartParagraphIndexArray.Add(oldsnum-1);
						oldEndParagraphIndexArray.Add(oldenum+1-1);
						//#379 �폜�e�L�X�g�擾
						AText	deletedText;
						for( AIndex p = pos; p < inText.GetItemCount(); )
						{
							AText	l;
							inText.GetLine(p,l);
							if( l.GetItemCount() < 2 )   continue;
							if( l.Get(0) != '<' )   break;
							l.Delete(0,2);
							deletedText.AddText(l);
							deletedText.Add(kUChar_LineEnd);
						}
						deletedTextArray.Add(deletedText);
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
			}
		}
	}
}

//#732
/**
*/
/*#drop
void	ADocument_Text::GetDiffDataFor2Texts( const AFileAcc& inDiffTempFolder,
		const AText& inText1, const AText& inText2, 
		AArray<ADiffType>& diffTypeArray,
		AArray<AIndex>& startParagraphIndexArray, AArray<AIndex>& endParagraphIndexArray,
		AArray<AIndex>& oldStartParagraphIndexArray, AArray<AIndex>& oldEndParagraphIndexArray,
		ATextArray& deletedTextArray )
{
	//
	diffTypeArray.DeleteAll();
	startParagraphIndexArray.DeleteAll();
	endParagraphIndexArray.DeleteAll();
	oldStartParagraphIndexArray.DeleteAll();
	oldEndParagraphIndexArray.DeleteAll();
	deletedTextArray.DeleteAll();
	//
	AText	text1;
	inText1.ConvertReturnCodeFromCR(returnCode_LF,text1);
	AFileAcc	file1;
	file1.SpecifyChild(inDiffTempFolder,"text1");
	file1.CreateFile();
	file1.WriteFile(text1);
	AText	file1path;
	file1.GetPath(file1path);
	//
	AText	text2;
	inText2.ConvertReturnCodeFromCR(returnCode_LF,text2);
	AFileAcc	file2;
	file2.SpecifyChild(inDiffTempFolder,"text2");
	file2.CreateFile();
	file2.WriteFile(text2);
	AText	file2path;
	file2.GetPath(file2path);
	
	//
	//Diff�p�X�E�I�v�V�����擾
	AText	diffExeFilePath;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffPath,diffExeFilePath);
	AText	diffOption;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffOption,diffOption);
	
#if IMPLEMENTATION_FOR_WINDOWS
	//���b�� Windows�ł�Diff�p�X�Œ�i�A�v���ɕt����diff.exe�����s�j
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	AFileAcc	diffExeFile;
	diffExeFile.SpecifySister(appFile,"diff\\diff.exe");
	diffExeFile.GetPath(diffExeFilePath);
#endif
	
	//diff�R�}���h�p�X
	AText	command;
	command.SetText(diffExeFilePath);
	//����
	ATextArray	argArray;
	argArray.Add(command);
	//Unified Diff�ݒ�
	AText	t;
	t.SetCstring("-u");
	argArray.Add(t);
	//Diff�I�v�V����
	if( diffOption.GetItemCount() > 0 )
	{
		argArray.Add(diffOption);
	}
	//
	//file1path.Insert1(0,'\"');
	//file1path.Add('\"');
	argArray.Add(file1path);
	//
	//file2path.Insert1(0,'\"');
	//file2path.Add('\"');
	argArray.Add(file2path);
	
	//
	AText	tmpFolderPath;
	inDiffTempFolder.GetPath(tmpFolderPath);
	//���s
	AText	resulttext;
	GetApp().SPI_GetChildProcessManager().ExecuteGeneralSyncCommand(command,argArray,tmpFolderPath,resulttext);
	//
	ParseDiffText(resulttext,diffTypeArray,startParagraphIndexArray,endParagraphIndexArray,
				oldStartParagraphIndexArray,oldEndParagraphIndexArray,deletedTextArray);
	resulttext.OutputToStderr();
}
*/

//#379
/**
���]Diff���w��h�L�������g�ɐݒ�
*/
void	ADocument_Text::SetReverseDiffToDiffTargetDocument( const ADocumentID inDiffTargetDocumentID,
		const AArray<ADiffType>& inDiffTypeArray,
		const AArray<AIndex>& inStartParagraphIndexArray, const AArray<AIndex>& inEndParagraphIndexArray,
		const AArray<AIndex>& inOldStartParagraphIndexArray, const AArray<AIndex>& inOldEndParagraphIndexArray )
{
	//���|�W�g���\���p�h�L�������g�p��diff�f�[�^�i���]diff�j�쐬
	AArray<ADiffType>	reverseDiffArray_DiffType;
	ATextArray	reverseDiffArray_DeletedText;
	for( AIndex i = 0; i < inDiffTypeArray.GetItemCount(); i++ )
	{
		switch(inDiffTypeArray.Get(i))
		{
			//�ǉ��̏ꍇ
		  case kDiffType_Added:
			{
				reverseDiffArray_DiffType.Add(kDiffType_Deleted);
				reverseDiffArray_DeletedText.Add(GetEmptyText());
				break;
			}
			//�폜�̏ꍇ�ireposText�ւ͍폜���ꂽ�e�L�X�g��ǉ��j
		  case kDiffType_Deleted:
			{
				reverseDiffArray_DiffType.Add(kDiffType_Added);
				reverseDiffArray_DeletedText.Add(GetEmptyText());
				break;
			}
			//�ύX�̏ꍇ�ireposText�ւ͍폜���ꂽ�e�L�X�g��ǉ��j
		  case kDiffType_Changed:
			{
				reverseDiffArray_DiffType.Add(kDiffType_Changed);
				reverseDiffArray_DeletedText.Add(GetEmptyText());
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
	//���|�W�g���\���p�h�L�������g�ɔ��]diff�ݒ�
	GetApp().SPI_GetTextDocumentByID(inDiffTargetDocumentID).SetDiffArray(reverseDiffArray_DiffType,
		inOldStartParagraphIndexArray,inOldEndParagraphIndexArray,
				inStartParagraphIndexArray,inEndParagraphIndexArray,reverseDiffArray_DeletedText,false);//#379
	GetApp().SPI_GetTextDocumentByID(inDiffTargetDocumentID).SPI_RefreshTextViews();//#379
}

//#379
/**
Diff�Ώۑ��̃h�L�������g������ꂽ�Ƃ��ɃR�[�������
*/
void	ADocument_Text::SPI_DoDiffTargetDocumentClosed( const ADocumentID inDiffTargetDocumentID )
{
	if( mDiffTargetFileDocumentID == inDiffTargetDocumentID )
	{
		mDiffTargetFileDocumentID = kObjectID_Invalid;
	}
	else if( mRepositoryDocumentID == inDiffTargetDocumentID )
	{
		mRepositoryDocumentID = kObjectID_Invalid;
		//���|�W�g���e�L�X�g�t�@�C�����폜
		mRepositoryTempFile.DeleteFile();
	}
	else
	{
		_ACError("",this);
	}
}

//#379
/**
Woking���̃h�L�������g������ꂽ�Ƃ��ɃR�[�������
*/
void	ADocument_Text::SPI_DoWorkingDocumentClosed( const ADocumentID inWorkingDocumentID )
{
	if( mDiffWorkingDocumentID == inWorkingDocumentID )
	{
		mDiffWorkingDocumentID = kObjectID_Invalid;
	}
	else
	{
		_ACError("",this);
	}
}

#pragma mark ===========================

#pragma mark ---SCM

/**
SCM��r�R�}���h���s�������̏���
*/
void	ADocument_Text::SPI_DoSCMCompareResult(/*#379 const AArray<ADiffType>& inDiffTypeArray, 
		const AArray<AIndex>& inStartParagraphIndexArray, const AArray<AIndex>& inEndParagraphIndexArray,
		const AArray<AIndex>& inOldStartParagraphIndexArray, const AArray<AIndex>& inOldEndParagraphIndexArray,//#379
		const ATextArray& inDeletedText*/ const AText& inDiffText )
{
	if( mDiffDisplayWaitFlag == false )   return;
	mDiffDisplayWaitFlag = false;
	
	//#379 diff���ʃe�L�X�g��́A���ʂ�z��Ɋi�[
	AArray<ADiffType>	diffTypeArray;
	AArray<AIndex>	startParagraphIndexArray;
	AArray<AIndex>	endParagraphIndexArray;
	AArray<AIndex>	oldStartParagraphIndexArray;
	AArray<AIndex>	oldEndParagraphIndexArray;
	ATextArray	deletedTextArray;
	ParseDiffText(inDiffText,diffTypeArray,startParagraphIndexArray,endParagraphIndexArray,
			oldStartParagraphIndexArray,oldEndParagraphIndexArray,deletedTextArray);
	
	//==================���|�W�g���e�L�X�g����==================
	
	AText	reposText;
	//��reposText��UTF8�ɂ��鏈�����m�F����B
	//�������́ACompareWithLatest()�ł�svn diff���s�ɑ΂��錋�ʏ����Bsvn diff�łȂ��ق���������������Ȃ��B
	
	//�{�h�L�������g�̒i���J�n�ʒu���X�g���擾
	AArray<AIndex>	paraStartIndex;
	for( AIndex lineIndex = 0; lineIndex < mTextInfo.GetLineCount();  )
	{
		paraStartIndex.Add(mTextInfo.GetLineStart(lineIndex));
		while( lineIndex < mTextInfo.GetLineCount() )
		{
			if( mTextInfo.GetLineExistLineEnd(lineIndex) == true )   break;
			lineIndex++;
		}
		lineIndex++;
	}
	
	//�{�h�L�������g���̎��̊J�n�i��index
	AIndex	nextStartParagraphIndex = 0;
	//
	for( AIndex i = 0; i < diffTypeArray.GetItemCount(); i++ )
	{
		//reposText�փe�L�X�g�ǉ�
		//diff�ӏ��̑O�̕����̊J�n�ʒu�擾
		if( nextStartParagraphIndex >= paraStartIndex.GetItemCount() )
		{
			//nextStartParagraphIndex����Ŏ擾�����i���J�n�ʒu���X�g�ȍ~�̏ꍇ�́A�Ō�̒i���ɂ���
			//��UTF-16�̏ꍇ���A����ɒi���擾�ł��Ȃ������ꍇ�̕␳�����B
			nextStartParagraphIndex = paraStartIndex.GetItemCount() -1;
		}
		AIndex	spos = paraStartIndex.Get(nextStartParagraphIndex);
		//diff�ӏ��̑O�̕����̏I���ʒu�擾
		AIndex	startParagraphIndex = startParagraphIndexArray.Get(i);
		if( startParagraphIndex >= paraStartIndex.GetItemCount() )
		{
			//startParagraphIndex����Ŏ擾�����i���J�n�ʒu���X�g�ȍ~�̏ꍇ�́A�Ō�̒i���ɂ���
			//��UTF-16�̏ꍇ���A����ɒi���擾�ł��Ȃ������ꍇ�̕␳�����B
			startParagraphIndex = paraStartIndex.GetItemCount()-1;
		}
		AIndex	epos = paraStartIndex.Get(startParagraphIndex);
		//
		reposText.InsertText(reposText.GetItemCount(),mText,spos,epos-spos);
		//���̊J�n�i��index��ݒ�
		if( diffTypeArray.Get(i) == kDiffType_Deleted )
		{
			nextStartParagraphIndex = startParagraphIndexArray.Get(i);
		}
		else
		{
			nextStartParagraphIndex = endParagraphIndexArray.Get(i);
		}
		//�폜�e�L�X�g���擾����UTF8�֕ϊ�
		AText	deletedTextUTF8;
		deletedTextUTF8.SetText(deletedTextArray.GetTextConst(i));
		deletedTextUTF8.ConvertToUTF8(SPI_GetTextEncoding());
		//reposText�֍폜�e�L�X�g��ǉ�
		switch(diffTypeArray.Get(i))
		{
			//�ǉ��̏ꍇ
		  case kDiffType_Added:
			{
				//��������
				break;
			}
			//�폜�̏ꍇ�ireposText�ւ͍폜���ꂽ�e�L�X�g��ǉ��j
		  case kDiffType_Deleted:
			{
				//reposText�փe�L�X�g�ǉ��i�폜���ꂽ�e�L�X�g�̒ǉ��j
				reposText.AddText(deletedTextUTF8);
				break;
			}
			//�ύX�̏ꍇ�ireposText�ւ͍폜���ꂽ�e�L�X�g��ǉ��j
		  case kDiffType_Changed:
			{
				//reposText�փe�L�X�g�ǉ��i�폜���ꂽ�e�L�X�g�̒ǉ��j
				reposText.AddText(deletedTextUTF8);
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
	//�Ō�̕����̃e�L�X�g�ǉ�
	if( nextStartParagraphIndex < paraStartIndex.GetItemCount() )
	{
		AIndex	spos = paraStartIndex.Get(nextStartParagraphIndex);
		AIndex	epos = mText.GetItemCount();
		reposText.InsertText(reposText.GetItemCount(),mText,spos,epos-spos);
	}
	
	//���|�W�g���\���p�̃h�L�������g�𐶐�
	if( mRepositoryDocumentID == kObjectID_Invalid )
	{
		//�h�L�������g����
		AText	title;
		NVI_GetTitle(title);
		title.AddCstring(":Repository");
		AText	tecName;
		//SPI_GetTextEncoding(tecName);
		ATextEncodingWrapper::GetTextEncodingName(ATextEncodingWrapper::GetUTF8TextEncoding(),tecName);
		//�E�C���h�E���������Ƀh�L�������g����
		ATextDocumentFactory	factory(this,kObjectID_Invalid);
		mRepositoryDocumentID = GetApp().SPNVI_CreateDiffTargetDocument(title,reposText,mModeIndex,tecName,
					kIndex_Invalid,kIndex_Invalid,false,false,SPI_GetFirstWindowID());
		GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).mDiffWorkingDocumentID = GetObjectID();
		GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).SPI_SetOwnedDocument();
		//View activate���̏����i�s�v�Z���s���j#699 diff target�h�L�������g�̓R�~�b�g���܂�view����x��active�ɂȂ�Ȃ��\��������̂ŁA�����ŏ�������
		GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).SPI_DoViewActivating(false);
		
		//���|�W�g���e�L�X�g�i�[�p�t�@�C�������iDiff���s�p�j
		AFileAcc	appPrefFolder;
		AFileWrapper::GetAppPrefFolder(appPrefFolder);
		AFileAcc	tmpFolder;
		//#427 tmpFolder.SpecifyChild(appPrefFolder,"temp");
		//#427 tmpFolder.CreateFolder();
		GetApp().SPI_GetTempFolder(tmpFolder);//#427
		mRepositoryTempFile.SpecifyUniqChildFile(tmpFolder,"temprepos");
		mRepositoryTempFile.CreateFile();
	}
	else
	{
		//�S�폜
		GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).SPI_DeleteText(0,
				GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).SPI_GetTextLength());
		//�e�L�X�g�}��
		GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).SPI_InsertText(0,reposText);
	}
	
	//���|�W�g���e�L�X�g���t�@�C���ۑ��iDiff���s�p�j
	AText	tmpText;
	reposText.ConvertFromUTF8CR(SPI_GetTextEncoding(),SPI_GetReturnCode(),tmpText);
	mRepositoryTempFile.WriteFile(tmpText);
	
	//�t�@�C�����������|�W�g���e�L�X�g�Ƃ�Diff���s
	ExecuteDiff();
}

void	ADocument_Text::SPI_GetNextDiffLineIndex( const AIndex inLineIndex, AIndex& outStart, AIndex& outEnd ) const//#512
{
	//#379
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);
	//#512
	outStart = outEnd = kIndex_Invalid;
	//
	for( AIndex i = 0; i < mDiffDisplayArray_StartParagraphIndex.GetItemCount(); i++ )
	{
		if( /*#379 inLineIndex*/paraIndex < mDiffDisplayArray_StartParagraphIndex.Get(i) )
		{
			outStart = SPI_GetLineIndexByParagraphIndex(mDiffDisplayArray_StartParagraphIndex.Get(i));//#512
			outEnd = SPI_GetLineIndexByParagraphIndex(mDiffDisplayArray_EndParagraphIndex.Get(i));//#512
			//���|�W�g���Ɣ�r���čŏI�s�i��s�j���폜����A�ŏI�s�����s�݂̂ł͂Ȃ��ꍇ�AoutStart/outEnd�͍ŏI�s�̌�ɂȂ�B
			//�ŏI�s�̌�͑I���ł��Ȃ��̂ŁA�ŏI�s�ɕ␳����B#0
			if( outStart >= SPI_GetLineCount() )   outStart = SPI_GetLineCount()-1;
			if( outEnd >= SPI_GetLineCount() )   outEnd = SPI_GetLineCount()-1;
			return;//#512
		}
	}
}

void	ADocument_Text::SPI_GetPrevDiffLineIndex( const AIndex inLineIndex, AIndex& outStart, AIndex& outEnd ) const//#512
{
	//#379
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);
	//#512
	outStart = outEnd = kIndex_Invalid;
	//
	for( AIndex i = mDiffDisplayArray_StartParagraphIndex.GetItemCount()-1; i >= 0; i-- )
	{
		if( /*#379 inLineIndex*/paraIndex >= mDiffDisplayArray_EndParagraphIndex.Get(i) )
		{
			outStart = SPI_GetLineIndexByParagraphIndex(mDiffDisplayArray_StartParagraphIndex.Get(i));//#512
			outEnd = SPI_GetLineIndexByParagraphIndex(mDiffDisplayArray_EndParagraphIndex.Get(i));//#512
			//���|�W�g���Ɣ�r���čŏI�s�i��s�j���폜����A�ŏI�s�����s�݂̂ł͂Ȃ��ꍇ�AoutStart/outEnd�͍ŏI�s�̌�ɂȂ�B
			//�ŏI�s�̌�͑I���ł��Ȃ��̂ŁA�ŏI�s�ɕ␳����B#0
			if( outStart >= SPI_GetLineCount() )   outStart = SPI_GetLineCount()-1;
			if( outEnd >= SPI_GetLineCount() )   outEnd = SPI_GetLineCount()-1;
			return;//#512
		}
	}
}

void	ADocument_Text::ClearDiffDisplay()
{
	if( mTextDocumentType == kTextDocumentType_DiffTarget )
	{
		//#0
		//diff target����diff display data�́Aworking����SetReverseDiffToDiffTargetDocument()����̂ݐݒ肷��
		//�i���[�h�ݒ�̃t�H���g�ύX���ȂǂŁA�������R�[������邪�A�����ŃN���A����ƁA���working document�̂ق���diff���ʎ擾�������Ă����ꍇ�A
		//����diff target���̌��ʂ��i�[����Ă���̂��������邱�ƂɂȂ��Ă��܂��B�j
		//����L���ʓ��̏ꍇ�A�r����diff���͑��݂��邪�A�s��񂪑��݂��Ȃ��^�C�~���O�����݂�����B���̏ꍇ�ɁAthrow���������Ȃ��悤�ɂ������B
		return;
	}
	
	mDiffDisplayWaitFlag = false;
	//#379 mDiffDisplayMode = kDiffDisplayMode_None;
	if( mDiffDisplayArray_DiffType.GetItemCount() == 0 )   return;
	mDiffDisplayArray_DiffType.DeleteAll();
	mDiffDisplayArray_StartParagraphIndex.DeleteAll();
	mDiffDisplayArray_EndParagraphIndex.DeleteAll();
	mDiffDisplayArray_DeletedText.DeleteAll();//#379
	mDiffParagraphIndexArray.DeleteAll();//#379
	//#212 RefreshWindow();
	SPI_RefreshTextViews();//#212
	
	//�F�擾
	AColor	subWindowNormalColor = kColor_List_Normal;//#1316 GetApp().SPI_GetSubWindowLetterColor();
	//
	for( AIndex i = 0; i < mJump_MenuColorArray.GetItemCount(); i++ )
	{
		mJump_MenuColorArray.Set(i,subWindowNormalColor);
	}
	SPI_UpdateJumpList();//#695 true);//#291
}

/**
Repository�̃f�[�^�X�V���̏���
*/
void	ADocument_Text::SPI_DoRepositoryUpdated()
{
	/*#379
	switch(mDiffDisplayMode)
	{
	  case kDiffDisplayMode_RepositoryLatest:
		{
			SPI_CompareWithRepository();
			break;
		}
	  default:
		{
			//��������
			break;
		}
		
	}
	*/
	//#379 ���|�W�g���e�L�X�g�{Diff�f�[�^�̍X�V���s��
	SPI_UpdateDiffDisplayData(true);
}

//#455
/**
�R�~�b�g�@�\���p�\���ǂ�����Ԃ�
*/
ABool	ADocument_Text::SPI_CanCommit() const
{
	ABool	canCommit = false;
	AFileAcc	file;
	NVI_GetFile(file);
	ASCMFolderType	scmFolderType = GetApp().SPI_GetSCMFolderManager().GetSCMFolderType(file);//#589
	if( scmFolderType == kSCMFolderType_SVN || scmFolderType == kSCMFolderType_Git )//#589
	{
		switch( /*#568 GetApp().SPI_GetSCMFolderManager().GetFileState(file)*/mSCMFileState )
		{
		  case kSCMFileState_Modified:
		  case kSCMFileState_Added:
			{
				canCommit = true;
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
	return canCommit;
}

//#379
/**
SCM��status update�R�}���h���s������ɃR�[�������i�h�L�������g��Diff���[�h�X�V�̂��߁j
*/
void	ADocument_Text::SPI_DoSCMStatusUpdated( const AFileAcc& inFolder )
{
	//#568 mSCMFileState���X�V
	AFileAcc	file;
	NVI_GetFile(file);
	mSCMFileState = GetApp().SPI_GetSCMFolderManager().GetFileState(file);
	//status update�R�}���h�̌��ʂ̕����A�h�L�������g������ԑJ�ڊ��������x�������ꍇ�A
	//�h�L�������g������ԑJ�ڊ�������SCM�Ώۂ��ǂ��������ʂł��Ȃ��̂ŁA���|�W�g����r���[�h�ɂł��Ȃ��B
	//���̂��߁A�����Ŕ�r���[�h�����A���A�h�L�������g������ԑJ�ڊ����ς݂ł���΁A�ēx���|�W�g����r���[�h�ɂ��邩�ǂ����̔�����s���B
	if( mDiffTargetMode == kDiffTargetMode_None && //#699 mViewDataInited == true )//#664 �������̂��߂܂�ViewData���������Ă��Ȃ��i�K�i�^�u���܂��J���Ă��Ȃ��j�ł̓��|�W�g���擾���Ȃ��悤�ɂ���
	   SPI_GetDocumentInitState() == kDocumentInitState_Completed )//#699
	{
		/*#81
		AFileAcc	diffFile;
		if( SPI_GetDiffAvailability(kDiffTargetMode_RepositoryLatest,diffFile) == true )
		{
			//���|�W�g����r���[�h
			mDiffTargetMode = kDiffTargetMode_RepositoryLatest;
			//Diff�f�[�^�X�V
			SPI_UpdateDiffDisplayData();
		}
		*/
		//���|�W�g����r�\�ł���΁A���|�W�g����r���[�h�ɂ���
		if( SPI_CanDiffWithRepositiory() == true )
		{
			SPI_SetDiffMode_Repository();
		}
	}
	
	//#688
	//�^�C�g���o�[�X�V�iinfoheader�p�~�̂��߁A�^�C�g���o�[��svn���\���j
	GetApp().NVI_UpdateTitleBar();
}

#pragma mark ===========================

#pragma mark ---���A�v���ɂ��ҏW

//R0232
//���A�v���ɂ��ҏW�L���̊m�F�i�ҏW�L��Ȃ甽�f�m�F�_�C�A���O�\���j
void	ADocument_Text::SPI_CheckEditByOtherApp()
{
	//�t�@�C���擾
	AFileAcc	file;
	switch( mTextDocumentType )
	{
	  case kTextDocumentType_Normal:
		{
			//------------------�ʏ탍�[�J���t�@�C���̏ꍇ------------------
			//�t�@�C����specify�ł���Ή������Ȃ�
			if( NVI_IsFileSpecified() == false )
			{
				return;
			}
			//�t�@�C���擾
			NVI_GetFile(file);
			break;
		}
	  case kTextDocumentType_DiffTarget:
		{
			//------------------diff target�t�@�C���̏ꍇ------------------
			//�t�@�C����specify�ł���Ή������Ȃ�
			if( mFileForDiffTargetDocument.IsSpecified() == false )
			{
				return;
			}
			//�t�@�C���擾
			file = mFileForDiffTargetDocument;
			break;
		}
	  default:
		return;
		break;
	}
	
	//#831
	//�����[�h�Ȃ牽�����Ȃ�
	if( mLoaded == false )   return;
	
	/*
	���L�̏����𗼕��������ꍇ�A���A�v���ɂ��ҏW���f�_�C�A���O��\������
	1. �O��I�[�v���^�ۑ����ƍŏI�X�V�������قȂ�Ƃ��A�������́A�ŏI�X�V�������擾�ł��Ȃ�
	2. �O��I�[�v���^�ۑ����ƃt�@�C�����e���قȂ�
	*/
	
	//�ŏI�X�V�����m�F
	if( file.Exist() == false )
	{
		//�t�@�C���폜�����ꂽ�ꍇ�́A�`�F�b�N���Ȃ��i���ɏ㏑���ۑ�����΁A�V���Ƀt�@�C���쐬�����B�j
		return;
	}
	ADateTime	datetime = -1;
	ABool	lastModifiedDateObtained = file.GetLastModifiedDataTime(datetime);//#653
	//�O��I�[�v���^�ۑ����ƍŏI�X�V�������قȂ�Ƃ��A�������́A�ŏI�X�V�������擾�ł��Ȃ�������A
	//�t�@�C�����e��r�������s���B
	if( datetime != mLastModifiedDateTime || lastModifiedDateObtained == false )//��lastModifiedDateObtained�擾�ł��Ȃ��ꍇ����̂��Č����B�A�v���؂�ւ����̃{�g���l�b�N�̉\���H#1267
	{
		//���݂̃t�@�C�����e�ƁA�O��I�[�v���^�ۑ����̃t�@�C�����e(mLastReadOrWriteFileRawData)���r���� #653
		//���b�N�����A���b�N���s���B�i���b�N�K�v���ǂ����킩��Ȃ���SPI_ReloadEditByOtherApp()�̏����ɍ��킹�Ă����B�j
		//���b�N����
		mFileWriteLocker.Unlock();
		//�ǂݍ��݁i�e�L�X�g�G���R�[�f�B���O�ύX���Ȃ��B���f�[�^�Ŕ�r�j
		AText	text;
		file.ReadTo(text);
		//���b�N
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kLockWhileEdit) == true && NVI_IsReadOnly() == false )
		{
			mFileWriteLocker.Lock();
		}
		//�Ō�Ƀ��[�h�E�Z�[�u�����Ƃ��̃t�@�C�����e�Ƃ̔�r�i���A�v���ł̕ҏW�̗L�����`�F�b�N�j
		//#693 if( text.Compare(mLastReadOrWriteFileRawData) == true )
		AFileAcc	lastLoadOrSaveFile;
		GetLastLoadOrSaveCopyFile(lastLoadOrSaveFile);
		AText	lastLoadOrSaveText;
		lastLoadOrSaveFile.ReadTo(lastLoadOrSaveText);
		if( text.Compare(lastLoadOrSaveText) == true || lastLoadOrSaveFile.Exist() == false )
		{
			//�t�@�C�����e�������Ƃ��́A�ŏI�X�V�����̂ݍX�V���āA�������Ȃ��B�i�_�C�A���O�\�����Ȃ��j
			if( lastModifiedDateObtained == true )
			{
				mLastModifiedDateTime = datetime;
			}
		}
		else
		{
			//#379 GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_ShowAskApplyEditByOtherApp(GetObjectID());
			/*#653 kDontShowEditByOhterAppDialog�ݒ�͍폜 if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontShowEditByOhterAppDialog) == true )
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontShowEditByOhterAppDialog) == true )
			{
				//kDontShowEditByOhterAppDialog��ON�Ȃ�A��Ƀ����[�h���� #545
				SPI_ReloadEditByOtherApp(true);
			}
			else
			{
			*/
			if( NVI_IsDirty() == false )
			{
				//dirty�ł͂Ȃ��idiff target document�̏ꍇ�܂ށj�̏ꍇ�́A��ɕύX��ǂݍ���
				SPI_ReloadEditByOtherApp(true);
			}
			else
			{
				//dirty�̂Ƃ��́A�ǂݍ��ނ��ǂ����̃_�C�A���O��\��
				AWindowID	firstWindowID = SPI_GetFirstWindowID();
				if( firstWindowID != kObjectID_Invalid )
				{
					GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_ShowAskApplyEditByOtherApp(GetObjectID());
				}
			}
			//#653}
		}
	}
}

//R0232
//���A�v���̕ҏW���ʂ̓Ǎ�
//inReload: true�Ȃ�ǂݍ��݂���  false�Ȃ�ǂݍ��݂����ɍŏI�X�V�����̂�update����i����app�A�N�e�B�x�[�g���ɍēx�_�C�A���O��\�����Ȃ��悤�Ɂj
void	ADocument_Text::SPI_ReloadEditByOtherApp( const ABool inReload )
{
	//�t�@�C���擾
	AFileAcc	file;
	switch( mTextDocumentType )
	{
	  case kTextDocumentType_Normal:
		{
			//------------------�ʏ탍�[�J���t�@�C���̏ꍇ------------------
			//�t�@�C����specify�ł���Ή������Ȃ�
			if( NVI_IsFileSpecified() == false )
			{
				return;
			}
			//�t�@�C���擾
			NVI_GetFile(file);
			break;
		}
		/*
		//------------------diff target�t�@�C���̏ꍇ------------------
		�Ƃ肠�����Ή����Ȃ��B�Ή�����ޗǁA���L�̑Ή����K�v�B
		�Emi�Ŕ�r���t�@�C����ύX�����ꍇ�A�A�v���P�[�V�����ؑֈȊO�ł������ɗ���K�v������B
		�E���[�L���O�t�@�C���ƂƂ���diff data���X�V����K�v������B
	  case kTextDocumentType_DiffTarget:
		{
			if( mFileForDiffTargetDocument.IsSpecified() == false )
			{
				return;
			}
			file = mFileForDiffTargetDocument;
			break;
		}
		*/
	  default:
		return;
		break;
	}
	
	//Reload���Ȃ��ꍇ�ł��A���b�N�����A���b�N���s���B
	//�������Ȃ��ƁASave���ɁA���t�@�C���f�[�^�����J�o���p�ɓǂݍ��ނƂ��ɁA�t�@�C���T�C�Y�̈Ⴄ�G���[���N����B
	//�i���Ԃ�Ami�Ńt�@�C�����J�����܂܁A���t�@�C���f�[�^��ǂݍ��ނ̂ŁA�t�@�C���̒�����ǂނƂ��ɋ��f�[�^�̒������K�p�����B�j
	//���b�N����
	mFileWriteLocker.Unlock();
	//�ǂݍ���#653
	//#693 file.ReadTo(mLastReadOrWriteFileRawData);
	AText	fileText;
	file.ReadTo(fileText);
	//Reload���Ȃ��ꍇ�ł�lastloadorsave�t�@�C���ɂ͋L������BReload���Ȃ����Ƃ�I�������Ƃ��̃f�[�^���L�����邽�߁B
	AFileAcc	lastLoadOrSaveFile;
	GetLastLoadOrSaveCopyFile(lastLoadOrSaveFile);
	lastLoadOrSaveFile.CreateFile();
	lastLoadOrSaveFile.WriteFile(fileText);
	//�ŏI�X�V�����X�V
	file.GetLastModifiedDataTime(mLastModifiedDateTime);
	//���b�N
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kLockWhileEdit) == true && NVI_IsReadOnly() == false )
	{
		mFileWriteLocker.Lock();
	}
	//
	if( inReload == true )
	{
		//�eview�̑I��͈͂��L��
		AArray<ATextPoint>	sptArray, eptArray;
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			ATextPoint	spt = {0}, ept = {0};
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetSelect(spt,ept);
			sptArray.Add(spt);
			eptArray.Add(ept);
		}
		//Undo�A�N�V�����^�O�L�^
		SPI_RecordUndoActionTag(undoTag_EditByOtherApp);
		//�S�폜
		SPI_DeleteText(0,SPI_GetTextLength(),true,false);
		//UTF8/CR�ɕϊ�
		ConvertToUTF8(fileText);//#764
		//�e�L�X�g�}��
		SPI_InsertText(0,fileText,true,false);
		//�t�@�C�����擾
		AText	fileName;
		file.GetFilename(fileName);
		//�eview�̑I��͈͂𕜌��Anotification�\��
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			//�I��͈͕���
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_SetSelect(sptArray.Get(i),eptArray.Get(i));
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_AdjustScroll_Center();
			//notification�\��
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetPopupNotificationWindow().
					SPI_GetNotificationView().SPI_SetNotification_ReloadEditByOtherApp(fileName);
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_ShowNotificationPopupWindow(true);
		}
		//���Xdirty�������ꍇ�ł��A�t�@�C������ǂݍ��񂾂̂ŁAnot dirty�ɂ���̂��Ó��B
		NVI_SetDirty(false);
		//------------------Diff�f�[�^�X�V------------------
		SPI_UpdateDiffDisplayData();
	}
}

//#693
/**
���[�h���A����сA�Ō�̃Z�[�u���̃t�@�C���̃R�s�[��ۑ�����t�@�C�����擾
*/
void	ADocument_Text::GetLastLoadOrSaveCopyFile( AFileAcc& outFile ) const
{
	//�t�@�C���p�X�擾
	AText	filepath;
	NVI_GetFilePath(filepath);
	//doc pref�t�H���_�擾
	AFileAcc	docPrefRootFolder;
	GetApp().SPI_GetDocPrefRootFolder(docPrefRootFolder);
	AFileAcc	docPrefFolder;
	docPrefFolder.SpecifyChild(docPrefRootFolder,filepath);
	docPrefFolder.CreateFolderRecursive();
	//doc pref�t�H���_����lastloadorsave�t�@�C�����擾
	outFile.SpecifyChild(docPrefFolder,"lastloadorsave");
}

#pragma mark ===========================

/*#92
void	ADocument_Text::SPI_SelectFirstWindow()
{
	if( mWindowIDArray.GetItemCount() == 0 )   return;
	GetApp().NVI_GetWindowByID(mWindowIDArray.Get(0)).NVI_SelectTabByDocumentID(GetObjectID());
	GetApp().NVI_GetWindowByID(mWindowIDArray.Get(0)).NVI_SelectWindow();
}
*/

//#199
/**
�ŏ���View��Ŏw��͈͂�I�����A�őO�ʂɎ����Ă���
*/
void	ADocument_Text::SPI_SelectText( const AIndex inStartIndex, const AItemCount inLength, const ABool inByFind )
{
	/*#199
	if( mWindowIDArray.GetItemCount() == 0 )   return;
	GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_SelectText(GetObjectID(),inStartIndex,inLength);
	*/
	if( mViewIDArray.GetItemCount() == 0 )   return;
	//#312 �t�H�[�J�X����Ă���View������΂����D��i�Ȃ���΁A�ŏ���View�j
	AViewID	viewID = mViewIDArray.Get(0);
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		if( AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).NVI_IsFocusActive() == true ||
					AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).NVI_IsFocusInTab() == true )
		{
			viewID = mViewIDArray.Get(i);
			break;
		}
	}
	//�I��
	AView_Text::GetTextViewByViewID(viewID).SPI_SelectText(inStartIndex,inLength,inByFind);
	//�\��
	AView_Text::GetTextViewByViewID(viewID).NVI_RevealView();
}

AWindowID	ADocument_Text::SPI_GetFirstWindowID() const
{
	/* #379 mViewIDArray����WindowID���擾����悤�ɕύX
	if( mWindowIDArray.GetItemCount() == 0 )   return kObjectID_Invalid;
	return mWindowIDArray.Get(0);
	*/
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AWindowID	textWindowID = AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetTextWindowID();
		if( textWindowID != kObjectID_Invalid )
		{
			return textWindowID;
		}
	}
	return kObjectID_Invalid;
}

#pragma mark ===========================

#pragma mark ---�s�܂�Ԃ��v�Z
//#699

/*
�X���b�h�ɂ��s�܂�Ԃ��v�Z�̗���
1. �X���b�h�ɂ��s�܂�Ԃ������s����Ƃ��́A���O�ɁA1�s�ɍs�܂�Ԃ��v�Z�������e�L�X�g���S�ē����Ă�����
�i����ȊO�̍s�f�[�^�͐������Ƃ�Ă����ԁj�ŁA���s����B���̍s���u���ݍs�v�Ƃ��ċL���B
�X���b�h�ւ��̍s�̃e�L�X�g��n���B
2. �X���b�h��k�s��܂�Ԃ��v�Z���āA���C���X���b�h�ɖ߂��B�i�e�L�X�g�̂ǂ��܂Ŏ��s�������̓X���b�h�ŋL���j
3. ���C���X���b�h�́A�u���ݍs�v�ȍ~�ɃX���b�h�Ōv�Z���ꂽk�s���̍s�f�[�^���R�s�[����B�ik�s�ǉ��j
4. ���C���X���b�h�́A�u���ݍs�v�Ƃ��āA�R�s�[�������̍s�ɁA�c��̃e�L�X�g���S�ē����Ă����Ԃɂ��Ă����B
5. �X���b�h�Ɍp�����s���w������B
6. �܂�Ԃ��v�Z���ׂ��e�L�X�g�������Ȃ�܂ŌJ��Ԃ��B
�i�܂�A��Ɂu���ݍs�vmWrapCalculator_CurrentLineIndex���X���b�h���W�J���Ă����`�ɂ���B�j
*/

/**
�X���b�h�ɍs�܂�Ԃ��v�Z���˗�����
@param inLineIndex �܂�Ԃ����v�Z����Ώۍs
*/
void	ADocument_Text::StartWrapCalculator( const AIndex inLineIndex )
{
	if( mWrapCalculator_ThreadUnpausedFlag == false )
	{
		//�X���b�h���쒆�łȂ��ꍇ
		
		//�w��s�̎��ʎq���폜
		AArray<AUniqID>	deletedIdentifier;
		ABool	importChanged = false;
		mTextInfo.DeleteLineIdentifiers(inLineIndex,deletedIdentifier,importChanged);
		DeleteFromJumpMenu(deletedIdentifier);
		if( importChanged == true )
		{
			mImportFileDataDirty = true;
		}
		
		//mWrapCalculator_CurrentLineIndex�ɐ܂�Ԃ��v�Z�Ώۍs��ݒ�
		mWrapCalculator_CurrentLineIndex = inLineIndex;
		//�܂�Ԃ��v�Z�Ώۍs�i���v�Z�Ώۃe�L�X�g���߂��܂�Ă���j��LineStart, len�擾
		AIndex	lineStart = mTextInfo.GetLineStart(inLineIndex);
		AItemCount	len = mTextInfo.GetLineLengthWithLineEnd(inLineIndex);
		//Progress�\���p�ɁA�v�Z����S�̂̃����O�X���L��
		mWrapCalculatorProgressTotalTextLength = len;
		mWrapCalculatorProgressRemainedTextLength = len;
		//�v�Z�Ώۃe�L�X�g���X���b�h�ɐݒ�
		GetWrapCalculatorThread().SPI_SetText(mText,lineStart,len);
		//���������蓖�ăX�e�b�v��ݒ�i���C���X���b�h��mTextInfo�j
		mTextInfo.SetLineReallocateStep(mText.GetCountOfChar(kUChar_LineEnd,lineStart,len));
		//�X���b�h�ɍs�܂�Ԃ��p�����[�^��ݒ�
		AText	fontname;
		SPI_GetFontName(fontname);
		ANumber	viewWidth = AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetTextViewWidth();
		GetWrapCalculatorThread().SPI_SetWrapParameters(fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
					SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),
					viewWidth,GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters));
		//�X���b�h�N���i�N����A�X���b�h��pause���ꂽ��ԂɂȂ�BContinueWrapCalculator()��pause�����B�j
		GetWrapCalculatorThread().NVI_Run(true);//�ŏ���pause���
		//�X���b�hunpause
		ContinueWrapCalculator();
	}
	else
	{
		//�X���b�h���쒆�̏ꍇ
		
		//�s�܂�Ԃ��v�Z���͕ҏW�s�Ƃ��邽�߁A�����ɂ͗��Ȃ��͂�
		_ACError("",NULL);
	}
}

/**
�s�܂�Ԃ��v�Z�X���b�hpause������
*/
ABool	ADocument_Text::SPI_DoWrapCalculatorPaused()
{
	//�X���b�h���쒆�̃`�F�b�N
	if( mWrapCalculator_ThreadUnpausedFlag == false )
	{
		//�����ɗ���Ƃ��̓X���b�h���쒆�̂͂�
		_ACError("",NULL);
		return false;
	}
	
	//�X���b�h���쒆�t���Ooff
	mWrapCalculator_ThreadUnpausedFlag = false;
	
	//�X���b�h�̌v�Z���ʂ�mTextInfo�փR�s�[
	//�X���b�h���̍s�f�[�^���S��mWrapCalculator_CurrentLineIndex�ȍ~�ɃR�s�[�ǉ������
	//�s�v�Z�����̏ꍇ�́Atrue��Ԃ��BEOF��s���K�v�ȏꍇ�́ACopyFromCalculatorResult()�ɂĒǉ��ς݁B
	ABool	completed = mTextInfo.CopyFromCalculatorResult(mWrapCalculator_CurrentLineIndex,
			GetWrapCalculatorThread().SPI_GetTextInfo());
	
	//TextView��LineImageInfo�X�V
	AItemCount	insertedLineCount = GetWrapCalculatorThread().SPI_GetTextInfo().GetLineCount();
	if( completed == true )
	{
		insertedLineCount--;
	}
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateLineImageInfo(mWrapCalculator_CurrentLineIndex,
			insertedLineCount,mWrapCalculator_CurrentLineIndex);
	}
	
	//TextView�ɍs�v�Z����ʒm�i�s�ԍ��{�^���X�V�̂��߁j
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_DoWrapCalculating();
	}
	
	//�`��X�V
	SPI_RefreshTextViews(mWrapCalculator_CurrentLineIndex,SPI_GetLineCount());
	
	//�s�܂�Ԃ��v�Z��������
	if( completed == false )
	{
		//�������̏ꍇ�́A�X���b�h�s�v�Z�Ώۍs���v�Z�ςݍs�̌�ɓ����Ă���B
		mWrapCalculator_CurrentLineIndex += GetWrapCalculatorThread().SPI_GetTextInfo().GetLineCount();
		
		//Progress�\���p�ɁA�c���Ă���e�L�X�g�̗ʂ��擾
		mWrapCalculatorProgressRemainedTextLength = 
				mTextInfo.GetLineLengthWithLineEnd(mWrapCalculator_CurrentLineIndex);
		//�X���b�hunpause
		ContinueWrapCalculator();
	}
	else
	{
		//�s�܂�Ԃ�����
		
		//Progress�\���p�f�[�^������
		mWrapCalculatorProgressTotalTextLength = kIndex_Invalid;
		mWrapCalculatorProgressRemainedTextLength = 0;
		
		//�X���b�h��~
		GetWrapCalculatorThread().NVI_AbortThread();
		GetWrapCalculatorThread().NVI_Unpause();
		GetWrapCalculatorThread().NVI_WaitThreadEnd();
		GetWrapCalculatorThread().ClearData();
		
		//�s�܂�Ԃ��v�Z����������
		if( SPI_GetDocumentInitState() == kDocumentInitState_WrapCalculating )
		{
			//���������̏ꍇ
			
			//��ԑJ��
			SPI_TransitInitState_WrapCalculateCompleted();
		}
		else if( mWrapCalculatorWorkingByInsertText == true )
		{
			//�ҏW�ɂ��s�܂�Ԃ��v�Z���̏ꍇ
			
			//�ҏW�ɂ��s�܂�Ԃ��v�Z���t���Ooff
			mWrapCalculatorWorkingByInsertText = false;
			//���@�F�����F���̍s���當�@�F��
			AIndex	unrecognizedLineIndex = mTextInfo.FindFirstUnrecognizedLine();
			if( unrecognizedLineIndex != kIndex_Invalid )
			{
				StartSyntaxRecognizer(unrecognizedLineIndex);
			}
			//TextView�ɍs�܂�Ԃ��v�Z������ʒm�i�\�񂵂��I���ʒu�����f�����j
			for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_DoWrapCalculated();
			}
			//#142
			//�e�L�X�g�J�E���g�E�C���h�E�X�V
			UpdateDocInfoWindows();
		}
		else
		{
			_ACError("",NULL);
		}
	}
	//�^�C�g���o�[�\���X�V
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateWindowTitleBarText();
	}
	//���^�[��
	return completed;
}

/**
�s�܂�Ԃ��v�Z�X���b�hunpause
*/
void	ADocument_Text::ContinueWrapCalculator()
{
	mWrapCalculator_ThreadUnpausedFlag = true;
	//�X���b�h���s�ĊJ�i�X���b�hpause��ԉ����j
	GetWrapCalculatorThread().NVI_Unpause();
}

/**
�s�܂�Ԃ��v�Z�X���b�h�擾
*/
AThread_WrapCalculator&	ADocument_Text::GetWrapCalculatorThread()
{
	if( mWrapCalculatorThreadID == kObjectID_Invalid )
	{
		AThread_WrapCalculatorFactory	factory(this);
		mWrapCalculatorThreadID = GetApp().NVI_CreateThread(factory);
	}
	return (dynamic_cast<AThread_WrapCalculator&>(GetApp().NVI_GetThreadByID(mWrapCalculatorThreadID)));
}

/**
�s�܂�Ԃ��v�Z�̐i���擾
*/
ANumber	ADocument_Text::SPI_GetWrapCalculatorProgressPercent()
{
	if( SPI_GetDocumentInitState() == kDocumentInitState_WrapCalculating || mWrapCalculatorWorkingByInsertText == true )
	{
		//�e�L�X�g�̃T�C�Y���������ꍇ�ɁA�h�L�������g���J��������ɁA�u�܂�Ԃ��v�Z���v�u�������݋֎~�v�ǂ�����\�������Ȃ�����
		if( mWrapCalculatorProgressTotalTextLength < 1024*1024 )
		{
			return 0;
		}
		//0���Z�h�~�i�O�̂��߁j
		if( mWrapCalculatorProgressTotalTextLength == 0 )   return 100;
		//�p�[�Z���g�v�Z
		AFloatNumber	result = mWrapCalculatorProgressTotalTextLength-mWrapCalculatorProgressRemainedTextLength;
		result *= 100;
		result /= mWrapCalculatorProgressTotalTextLength;
		return result;
	}
	else
	{
		return kIndex_Invalid;
	}
}

#pragma mark ===========================

#pragma mark ---���@�F��
//#698

//#905
/**
unrecognized�ȍs���T�[�`���đS�āi�X���b�h�Łj�F������
*/
void	ADocument_Text::SPI_RecognizeSyntaxUnrecognizedLines()
{
	//���@�F���������Ȃ牽�������I�� #0
	if( SPI_GetDocumentInitState() != kDocumentInitState_Completed )
	{
		return;
	}
	//
	AIndex	unrecognizedLineIndex = mTextInfo.FindFirstUnrecognizedLine();
	if( unrecognizedLineIndex != kIndex_Invalid )
	{
		StartSyntaxRecognizer(unrecognizedLineIndex);
	}
}

/**
�X���b�h�ɂ�镶�@�F���J�n
���ɃX���b�h���쒆�̏ꍇ�́A����X���b�h����߂��Ă����Ƃ��ɃX���b�h���@�F�����ĊJ����
�iinLineIndex�����܂̔F���J�n�s������������΁A����F���J�n�s���X�V����j
@param inLineIndex �F���J�n�s�i0�s�ڈȊO�́A���ɍs�̊J�n��ԓ����ݒ�ς݂̍s���w�肷�邱�ƁB�j
*/
void	ADocument_Text::StartSyntaxRecognizer( const AIndex inLineIndex )
{
	//�s�܂�Ԃ��v�Z���͉����������^�[��
	if( mWrapCalculatorWorkingByInsertText == true )
	{
		return;
	}
	
	//�X���b�h���N���Ȃ�X���b�h�N��
	if( mSyntaxRecognizer_ThreadRunningFlag == false )
	{
		//�X���b�h�N���i�N����A�X���b�h��pause���ꂽ��ԂɂȂ�BContinueWrapCalculator()��pause�����B�j
		GetSyntaxRecognizerThread().NVI_Run(true);//�ŏ���pause���
		mSyntaxRecognizer_ThreadRunningFlag = true;
	}
	
	//�X���b�h���쒆����
	if( mSyntaxRecognizer_ThreadUnpausedFlag == false )
	{
		//�X���b�h���쒆�Ŗ����ꍇ
		
		// =============== �X���b�h�J�n���� ===============
		
		//���@�F���O�����i�R���e�L�X�g�쐬�j
		AText	url;//#998
		SPI_GetURL(url);//#998
		mSyntaxRecognizer_StartLineIndex = mTextInfo.PrepareRecognizeSyntaxContext(mText,inLineIndex,url);//#998
		
		//�X���b�h�ɃR���e�L�X�g���R�s�[
		GetSyntaxRecognizerThread().SPI_SetContext(mModeIndex,mTextInfo,mSyntaxRecognizer_StartLineIndex);
		
		//���@�F���I���s�ݒ�i������s���j
		mSyntaxRecognizer_EndLineIndex = mSyntaxRecognizer_StartLineIndex 
				+ kSyntaxRecognizerThreadPauseLineCount;//���̎��s��
		if( mSyntaxRecognizer_EndLineIndex >= SPI_GetLineCount() )
		{
			mSyntaxRecognizer_EndLineIndex = SPI_GetLineCount();
		}
		
		//�X���b�h�ɁA������s���̑Ώۃe�L�X�g��TextInfo��ݒ�
		AIndex	startLine_LineStart = SPI_GetLineStart(mSyntaxRecognizer_StartLineIndex);
		AIndex	endLine_LineStart = SPI_GetLineStart(mSyntaxRecognizer_EndLineIndex);
		GetSyntaxRecognizerThread().SPI_SetText(mText,startLine_LineStart,endLine_LineStart-startLine_LineStart);
		GetSyntaxRecognizerThread().SPI_SetTextInfo(mTextInfo,
					mSyntaxRecognizer_StartLineIndex,mSyntaxRecognizer_EndLineIndex);
		//�X���b�h���쒆�t���Oon
		mSyntaxRecognizer_ThreadUnpausedFlag = true;
		//�X���b�h���X�^�[�g�t���O�������ioff�j
		mSyntaxRecognizer_RestartFlag = false;
		//�X���b�hunpause
		GetSyntaxRecognizerThread().NVI_Unpause();
		//
		if( AApplication::NVI_GetEnableDebugTraceMode() == true )
		{
			fprintf(stderr,"SyntaxRecognizerStart:%ld-%ld\n",
						mSyntaxRecognizer_StartLineIndex,mSyntaxRecognizer_EndLineIndex);
		}
	}
	else
	{
		//�X���b�h���쒆�̏ꍇ
		
		//�X���b�h���X�^�[�g�t���Oon�B����X���b�h����߂��Ă����Ƃ��ɁA����̌��ʂ͔j�����ă��X�^�[�g���s��
		mSyntaxRecognizer_RestartFlag = true;
		//inLineIndex���A���܂̔F���J�n�s������������΁A����F���J�n�s���X�V����
		if( inLineIndex < mSyntaxRecognizer_StartLineIndex )
		{
			mSyntaxRecognizer_StartLineIndex = inLineIndex;
		}
	}
}

/**
�e�L�X�g�ҏW�iInsertText/DeleteText�j�ɂ�镶�@�ĔF���J�n
@return ���@�F���I���s�iInsertText/DeleteText��TextView�̕`��X�V�̂��߂Ɏg���j
*/
AIndex	ADocument_Text::StartSyntaxRecognizerByEdit( const AIndex inLineIndex, const AItemCount inInsertedLineCount,
		const ABool inRecognizeSyntaxAlwaysInThread )
{
	//#905 �e�L�X�g�ҏW���ɂ͕��@�F���͍s��Ȃ��B
	//�ҏW��A���Ԍo�߂���s�Ȃǂ̃^�C�~���O��SPI_RecognizeSyntaxUnrecognizedLines()���R�[������A�X���b�h�ŕ��@�F�������B
	return inLineIndex;
	/*#905
	//�s�܂�Ԃ��v�Z���͉����������^�[��
	if( mWrapCalculatorWorkingByInsertText == true )
	{
		return inLineIndex;
	}
	
	//�X���b�h���쒆�łȂ��ꍇ�F臒l�����Ȃ烁�C���X���b�h���ŔF���A臒l�ȏ�Ȃ�F���X���b�h�ɔF���˗�
	//�X���b�h���쒆�̏ꍇ�F����X���b�h����߂����Ƃ��ɁAinLineIndex����ĊJ
	
	//recognizeEndLineIndex: �Ԃ�l�i���@�F���I���s�j
	//�X���b�h���s�ɂ���ꍇ�́A�J�n�s�iinLineIndex�j��Ԃ��悤�ɂ���
	//�i�Ԃ�l�́A���@�F���ɂ��`��X�V�͈͂��L���邽�߂Ɏg�p����Ă���̂ŁA
	//inLineIndex��Ԃ��΁A�`��X�V�͈͍͂L����Ȃ����ƂɂȂ�B�i�`��X�V�̓X���b�h�I�����ɏ������ׂ��Ȃ̂ŁB�j�j
	AIndex	recognizeEndLineIndex = inLineIndex;
	
	if( inRecognizeSyntaxAlwaysInThread == true )
	{
		//�X���b�h�ŔF���J�n
		StartSyntaxRecognizer(inLineIndex);
		//
		return inLineIndex;
	}
	
	//�X���b�h���쒆����
	if( mSyntaxRecognizer_ThreadUnpausedFlag == false )
	{
		//�X���b�h���쒆�łȂ��ꍇ
		if( inInsertedLineCount >= kThreashold_LineCountToUseSyntaxRecognizerThreadByEdit )
		{
			//�}���s����臒l�ȏ�̏ꍇ
			
			//�X���b�h�ŔF���J�n
			StartSyntaxRecognizer(inLineIndex);
		}
		else
		{
			//�}���s����臒l�����̏ꍇ
			
			//���C���X���b�h���ŔF������
			recognizeEndLineIndex = RecognizeSyntaxInMainThread(inLineIndex,kIndex_Invalid);
		}
	}
	else
	{
		//�X���b�h���쒆�̏ꍇ
		
		//�X���b�h�ŔF���J�n
		StartSyntaxRecognizer(inLineIndex);
	}
	return recognizeEndLineIndex;
	*/
}

/**
���@�F���X���b�hpause������
*/
void	ADocument_Text::SPI_DoSyntaxRecognizerPaused()
{
	//�s�܂�Ԃ��v�Z���͉����������^�[��
	if( mSyntaxRecognizer_ThreadUnpausedFlag == false )
	{
		_ACError("",NULL);
		return;
	}
	
	//�X���b�h���쒆�t���Ooff
	mSyntaxRecognizer_ThreadUnpausedFlag = false;
	
	//���X�^�[�g�t���O����
	if( mSyntaxRecognizer_RestartFlag == true )
	{
		//Restart�t���O��On�̂Ƃ��́A����̃X���b�h���@�F�����ʂ�j�����āA
		//�w�肳�ꂽ�s�imSyntaxRecognizer_StartLineIndex�j����X���b�h���@�F�������X�^�[�g����B
		StartSyntaxRecognizer(mSyntaxRecognizer_StartLineIndex);
	}
	else
	{
		//����̃X���b�h���@�F�����ʂ��A�X���b�h����{TextInfo�փR�s�[����B
		CopySyntaxRecognizerResult();
		
		//���@�F���I������
		//�F�����W���[���ŕ��@�F���I���Ɣ��f���ꂽ�ꍇ�i��ExecuteRecognizeSyntax()���r���̍s�ŏI�������ꍇ�j�A�܂��́A
		//�X���b�h�ɔF���w�������ŏI�s���e�L�X�g�̍ŏI�s�̏ꍇ�́A�F���I���ƌ��Ȃ��B
		//����ȊO�̏ꍇ�́A����X���b�h�ɔF���w�����Ă����ŏI�s���J�n�s�Ƃ��ăX���b�h�p�����s����B
		if( GetSyntaxRecognizerThread().SPI_RecognizerJudgedAsEnd() == false && 
					mSyntaxRecognizer_EndLineIndex < SPI_GetLineCount() )
		{
			//���@�F��������
			
			//����X���b�h�ɔF���w�����Ă����s�̍Ō�̍s���J�n�s�Ƃ��ăX���b�h�p�����s
			//ContinueSyntaxRecognizer(mSyntaxRecognizer_EndLineIndex);
			AIndex	lineIndex = mSyntaxRecognizer_EndLineIndex;
			if( lineIndex > 0 )
			{
				//�Ō�̔F���ςݍs���J�n�s�Ƃ��Ďw������
				//�i�F���J�n�s�́A�s�J�n���_�ł̃C���f���g��񓙂�v���邽�߁B�j
				lineIndex--;
			}
			//StartSyntaxRecognizer()�́Astable state�܂ł����̂ڂ��ĊJ�n����B
			//����́A�e��ϐ���token stack��spos���͕ێ��ł��Ȃ��ispos��point�ϊ����邽�߂ɂ�text info�̕ێ����K�v�j���߁A
			//����A�R���e�L�X�g�͍�蒼���A�ϐ��Etoken stack�͏��������邽�߁Bstable state�܂ł����̂ڂ�΁A
			//�ϐ��Etoken stack�������̉e���͏��Ȃ��i�]���̒ʏ�ҏW���Ɠ����ɂȂ�j�B
			StartSyntaxRecognizer(lineIndex);
		}
		else
		{
			//���@�F������
			
			//�e�L�X�g�̍ŏ����當�@�F�����������̍s������
			//�i�X���b�h�F�����ɔF���ς݂̉ӏ���ҏW�����ꍇ�ȂǁA���F�����F���ς݁����F���̏�ԂɂȂ�B
			//���̂Ƃ��A�ŏ��̖��F���̏I���ӏ��ŔF�����W���[���ŕ��@�F���I���Ɣ��f�����̂ŁA
			//���F���̉ӏ�����F���ĊJ������K�v������B�j
			AIndex	unrecognizedLineIndex = mTextInfo.FindFirstUnrecognizedLine();
			if( unrecognizedLineIndex != kIndex_Invalid )
			{
				//�������̍s������΂��̍s���當�@�F�������X�^�[�g
				if( unrecognizedLineIndex > 0 )
				{
					//�Ō�̔F���ςݍs���J�n�s�Ƃ��Ďw������
					//�i�F���J�n�s�́A�s�J�n���_�ł̃C���f���g��񓙂�v���邽�߁B�j
					unrecognizedLineIndex--;
				}
				StartSyntaxRecognizer(unrecognizedLineIndex);
			}
			else
			{
				//���@�F���������̍s�����i���@�F�������j
				
				//�X���b�h�I��
				mSyntaxRecognizer_ThreadRunningFlag = false;
				GetSyntaxRecognizerThread().NVI_AbortThread();
				GetSyntaxRecognizerThread().NVI_Unpause();
				GetSyntaxRecognizerThread().NVI_WaitThreadEnd();
				GetSyntaxRecognizerThread().ClearData();
				mSyntaxRecognizer_StartLineIndex = SPI_GetLineCount();
				mSyntaxRecognizer_EndLineIndex = SPI_GetLineCount();
				//��ԑJ��
				if( SPI_GetDocumentInitState() == kDocumentInitState_SyntaxRecognizing )
				{
					SPI_TransitInitState_SyntaxRecognizeCompleted();
				}
			}
		}
	}
}

/**
�X���b�h���@�F�����ʂ����C���X���b�h��TextInfo�փR�s�[
*/
void	ADocument_Text::CopySyntaxRecognizerResult()
{
	AArray<AUniqID>	deletedIdentifier;
	AArray<AUniqID>	addedGlobalIdentifier;
	AArray<AIndex>	addedGlobalIdentifierLineIndex;
	ABool	importChanged = false;
	//�R�s�[��̍s���擾
	AIndex	dstStartLineIndex = mSyntaxRecognizer_StartLineIndex;
	AIndex	dstEndLineIndex = mSyntaxRecognizer_StartLineIndex 
			+ GetSyntaxRecognizerThread().SPI_GetRecognizeEndLineIndex();
	//���@�F�����ʃR�s�[���s
	mTextInfo.CopyFromSyntaxRecognizerResult(dstStartLineIndex,dstEndLineIndex,
				GetSyntaxRecognizerThread().SPI_GetTextInfo(),
				deletedIdentifier,addedGlobalIdentifier,addedGlobalIdentifierLineIndex,importChanged);
	//���@�F���ɂ��import�X�V�t���O�ݒ�
	if( importChanged == true )
	{
		mImportFileDataDirty = true;
	}
	//���@�F���ɂ��폜���ꂽIdentifier�ɑΉ����郁�j���[���ڂ��폜����
	ABool	jumpMenuChanged = DeleteFromJumpMenu(deletedIdentifier);
	//���@�F���ɂ��ǉ����ꂽIdentifier�����j���[�ɓo�^
	if( addedGlobalIdentifier.GetItemCount() > 0 )
	{
		AddToJumpMenu(addedGlobalIdentifier,addedGlobalIdentifierLineIndex);
		jumpMenuChanged = true;
	}
	//�W�����v���X�g�X�V
	if( jumpMenuChanged == true )
	{
		//�W�����v���X�g�`��X�V
		SPI_RefreshJumpList();
		//�W�����v���X�g�I���ʒu�X�V
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			AView_Text&	textview = AView_Text::GetTextViewByViewID(mViewIDArray.Get(i));
			if( textview.NVI_IsFocusActive() == true )
			{
				ATextPoint	spt = {0,0}, ept = {0,0};
				textview.SPI_GetSelect(spt,ept);
				SPI_UpdateJumpListSelection(spt.y,false);//�L�����b�g�I���ɂ��X�V�ł͂Ȃ��̂ŃX�N���[����adjust���Ȃ�
			}
		}
	}
	
	//#905
	//���@�f�[�^���X�V�����͈͂��A�e�r���[�̃��[�J���͈͂Əd�Ȃ�ꍇ�́A���[�J�����ʎq���X�g���X�V����
	for( AIndex i = 0; i < mCurrentLocalIdentifierStartLineIndexArray.GetItemCount(); i++ )
	{
		AIndex	localStartLineIndex = mCurrentLocalIdentifierStartLineIndexArray.Get(i);
		AIndex	localEndLineIndex = mCurrentLocalIdentifierEndLineIndexArray.Get(i);
		if( localStartLineIndex != kIndex_Invalid && localEndLineIndex != kIndex_Invalid )
		{
			if( ! (dstEndLineIndex < localStartLineIndex || localEndLineIndex <= dstStartLineIndex) )
			{
				UpdateLocalIdentifierList(i,false);
			}
		}
	}
	
	//�e�L�X�g�r���[�X�V
	SPI_RefreshTextViews(dstStartLineIndex,dstEndLineIndex);
	
	//#905�i�^�C�~���O�ύX�j
	//Import File Data�X�V
	SPI_UpdateImportFileDataIfDirty();
}

/**
���@�F���X���b�h�擾
*/
AThread_SyntaxRecognizer&	ADocument_Text::GetSyntaxRecognizerThread()
{
	if( mSyntaxRecognizerThreadID == kObjectID_Invalid )
	{
		AThread_SyntaxRecognizerFactory	factory(this);
		mSyntaxRecognizerThreadID = GetApp().NVI_CreateThread(factory);
	}
	return (dynamic_cast<AThread_SyntaxRecognizer&>(GetApp().NVI_GetThreadByID(mSyntaxRecognizerThreadID)));
}

//���@�F��
AIndex	ADocument_Text::RecognizeSyntaxInMainThread( const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	ABool	jumpMenuChanged = false;
	//���@�F�����s
	AArray<AUniqID>	deletedIdentifier;
	AArray<AUniqID>	addedGlobalIdentifier;
	AArray<AIndex>	addedGlobalIdentifierLineIndex;
	//#698 AIndex	recognizedStartLineIndex, recognizedEndLineIndex;
	ABool	importChanged = false;
	AText	url;//#998
	SPI_GetURL(url);//#998
	mTextInfo.PrepareRecognizeSyntaxContext(mText,inStartLineIndex,url);//#698 #998
	ABool	abortFlag = false;
	AIndex	recognizedEndLineIndex = mTextInfo.ExecuteRecognizeSyntax(mText,inEndLineIndex,deletedIdentifier,
				addedGlobalIdentifier,addedGlobalIdentifierLineIndex,importChanged,abortFlag);//#698
	mTextInfo.ClearRecognizeSyntaxContext();//#698
	//fprintf(stderr,"%d-%d   ",recognizedStartLineIndex,recognizedEndLineIndex);
	if( importChanged == true )
	{
		//UpdateImportFileData();
		mImportFileDataDirty = true;
	}
	//���@�F���ɂ��폜���ꂽIdentifier�ɑΉ����郁�j���[���ڂ��폜����
	jumpMenuChanged = DeleteFromJumpMenu(deletedIdentifier);
	//if( jumpMenuChanged == true )  fprintf(stderr,"deleted:%d ",deletedIdentifier.GetItemCount());
	//���@�F���ɂ��ǉ����ꂽIdentifier�����j���[�ɓo�^
	/*#695
	for( AIndex i = 0; i < addedGlobalIdentifier.GetItemCount(); i++ )
	{
		if( mTextInfo.IsMenuIdentifier(addedGlobalIdentifier.Get(i)) == true )
		{
			AddToJumpMenu(addedGlobalIdentifier.Get(i),addedGlobalIdentifierLineIndex.Get(i));
			jumpMenuChanged = true;
		}
		//fprintf(stderr,"added:%d ",addedIdentifierLineIndex.Get(i));
	}
	*/
	if( addedGlobalIdentifier.GetItemCount() > 0 )
	{
		AddToJumpMenu(addedGlobalIdentifier,addedGlobalIdentifierLineIndex);
		jumpMenuChanged = true;
	}
	//�W�����v���X�g�X�V
	if( jumpMenuChanged == true )
	{
		//#695 SPI_UpdateJumpList();//#291
		SPI_RefreshJumpList();//#695
	}
	return recognizedEndLineIndex;
}

/* #695 ���������āAClearSyntaxDataForAllLines�ֈړ�
void	ADocument_Text::ClearSyntaxData( const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	ABool	jumpMenuChanged = false;
	AArray<AUniqID>	deletedIdentifier;//#695
	for( AIndex lineIndex = inEndLineIndex-1; lineIndex >= inStartLineIndex; lineIndex-- )//#695
	{
		//�Ώۍs��Identifier���폜
		ABool	importChanged = false;
		mTextInfo.DeleteLineIdentifiers(lineIndex,deletedIdentifier,importChanged);
		if( importChanged == true )
		{
			//UpdateImportFileData();
			mImportFileDataDirty = true;
		}
		mTextInfo.SetLineRecognized(lineIndex,false);
	}
	//���@�F���ɂ��폜���ꂽIdentifier�ɑΉ����郁�j���[���ڂ��폜���� #695 ���[�v������ړ�
	if( DeleteFromJumpMenu(deletedIdentifier) == true )
	{
		jumpMenuChanged = true;
	}
	//�W�����v���X�g�X�V
	if( jumpMenuChanged == true )
	{
		//#695 SPI_UpdateJumpList();//#291  
		SPI_RefreshJumpList();//#695
	}
}
*/

/**
���@�ĔF��
*/
void	ADocument_Text::SPI_ReRecognizeSyntax()
{
	//#695 ClearSyntaxData(0,SPI_GetLineCount());
	/*#698
	ATextPoint	pt = {0,0};
	RecognizeSyntax(pt);
	*/
	
	//#695
	//������Ԃł���΁A���������I��
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial || 
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened ||
	   SPI_GetDocumentInitState() == kDocumentInitState_WrapCalculating )
	{
		return;
	}
	
	//�S���@�F���f�[�^�폜
	mTextInfo.ClearSyntaxDataForAllLines();
	//�W�����v���ڑS�폜
	DeleteAllJumpItems();
	
	//#890
	//���@�F���X���b�h�����쒆�Ȃ�A���@�F���X���b�h���~����
	SPI_StopSyntaxRecognizerThread();
	
	//���@�F���J�n
	StartSyntaxRecognizer(0);
	
	//#554
	//�`��X�V
	SPI_RefreshTextViews();
}

//#844
/**
���@�F���f�[�^�S�폜�A�S�s���ʎq�폜�E�F���ς݃t���OOFF
*/
void	ADocument_Text::SPI_ClearSyntaxDataForAllLines()
{
	//�S���@�F���f�[�^�폜�A�S�s���ʎq�폜�E�F���ς݃t���OOFF
	mTextInfo.ClearSyntaxDataForAllLines();
	//�W�����v���ڑS�폜
	DeleteAllJumpItems();
}

/**
���@�F���X���b�h���~����
*/
void	ADocument_Text::SPI_StopSyntaxRecognizerThread()
{
	if( mSyntaxRecognizer_ThreadRunningFlag == true )
	{
		//�X���b�h��~
		mSyntaxRecognizer_ThreadUnpausedFlag = false;
		mSyntaxRecognizer_ThreadRunningFlag = false;
		mSyntaxRecognizer_RestartFlag = false;
		mSyntaxRecognizer_StartLineIndex = kIndex_Invalid;
		mSyntaxRecognizer_EndLineIndex = kIndex_Invalid;
		GetSyntaxRecognizerThread().NVI_AbortThread();
		GetSyntaxRecognizerThread().NVI_UnpauseIfPaused();
		GetSyntaxRecognizerThread().NVI_WaitThreadEnd();
		GetSyntaxRecognizerThread().ClearData();
	}
	//�C�x���g�L���[����A���̃h�L�������g�̕��@�F���X���b�h���瑗�M�������@�F�������C�x���g���폜
	ABase::RemoveEventFromMainInternalEventQueue(kInternalEvent_SyntaxRecognizerPaused,GetSyntaxRecognizerThread().GetObjectID());
}


void	ADocument_Text::UpdateWindowText( const AIndex inEditStartLineIndex, 
			const AIndex inRedrawStartLineIndex, const AIndex inRedrawEndLineIndex, const AItemCount inInsertedLineCount, const ABool inParagraphCountChanged,
			const ABool inDontRedraw )//#627
{
	/*#199 View�̃��\�b�h�𒼐ڃR�[������悤�ɏC��
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateText(GetObjectID(),inStartLineIndex,inEndLineIndex,inInsertedLineCount,inParagraphCountChanged);
	}
	*/
	//textview
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		//#450
		AIndex	updateLineImageInfoStart = inRedrawStartLineIndex;
		if( mCurrentLocalIdentifierStartLineIndexArray.Get(i) != kIndex_Invalid && mCurrentLocalIdentifierStartLineIndexArray.Get(i) < updateLineImageInfoStart )
		{
			updateLineImageInfoStart = mCurrentLocalIdentifierStartLineIndexArray.Get(i);
			//��#695 lineViewInfo�̌v�Z�J�n�ʒu���߂肷���Ȃ��悤�ɂ���
			if( updateLineImageInfoStart < inRedrawStartLineIndex-10 )
			{
				updateLineImageInfoStart = inRedrawStartLineIndex-10;
			}
		}
		//text view�擾
		AView_Text&	textview = AView_Text::GetTextViewByViewID(mViewIDArray.Get(i));
		//view��origin point�擾
		AImagePoint	frameOriginPoint = {0};
		textview.NVI_GetOriginOfLocalFrame(frameOriginPoint);
		//line image info���X�V
		ANumber	imageYDelta = textview.SPI_UpdateLineImageInfo(inEditStartLineIndex,inInsertedLineCount,updateLineImageInfoStart);//#627
		if( inDontRedraw == false )//#627
		{
			//==================�`��X�V==================
			if( imageYDelta > 0 )//#627
			{
				//------------------text view��imageY�ύX���������Ă���ꍇ------------------
				
				//�ҏW�I���s���擾
				AIndex	editEndLineIndex = inEditStartLineIndex;
				if( inInsertedLineCount > 0 )
				{
					editEndLineIndex += inInsertedLineCount;
				}
				if( editEndLineIndex < textview.SPI_GetOriginLineIndex() )
				{
					//------------------�ҏW�I���s���Aview��frame�J�n�s���O�̏ꍇ------------------
					//�i������view�ŕҏW���������Ă���ꍇ�j
					//view frame���̊J�n�s���ω����Ȃ��悤�ɂ���
					frameOriginPoint.y += imageYDelta;
					textview.NVI_SetOriginOfLocalFrame(frameOriginPoint);
					//�w�i�\������̏ꍇ�̂ݕ`��X�V
					if( GetApp().SPI_GetModePrefDB(mModeIndex).GetModeData_Bool(AModePrefDB::kDisplayBackgroundImage) == true )
					{
						textview.NVI_Refresh();
					}
					//�s�ԍ�view��origin point�X�V�A�`��X�V
					frameOriginPoint.x = 0;
					textview.SPI_GetLineNumberView().NVI_SetOriginOfLocalFrame(frameOriginPoint);
					textview.SPI_GetLineNumberView().NVI_Refresh();
				}
				else
				{
					//------------------�ҏW�I���s���Aview��frame�J�n�s�ȍ~�̏ꍇ------------------
					//�ҏW�s�ȍ~��S�čX�V����
					//#688 AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_RefreshTextView();//#450 NVI_Refresh();
					textview.SPI_RefreshLines(inRedrawStartLineIndex,SPI_GetLineCount());
				}
			}
			else
			{
				//------------------text view��imageY�ύX���������Ă��Ȃ��ꍇ------------------
				//textview��ImageY�ύX���������Ă��Ȃ���Βʏ�X�V
				textview.SPI_UpdateText(inRedrawStartLineIndex,inRedrawEndLineIndex,inInsertedLineCount,inParagraphCountChanged);//#450
			}
		}
	}
	/*#450
	//�s�ԍ�view
	for( AIndex i = 0; i < mLineNumberViewIDArray.GetItemCount(); i++ )
	{
		AView_LineNumber::GetLineNumberViewByViewID(mLineNumberViewIDArray.Get(i)).
				SPI_UpdateText(inStartLineIndex,inEndLineIndex,inInsertedLineCount,inParagraphCountChanged);
	}
	*/
}

/*#379
void	ADocument_Text::UpdateWindowViewImageSize()
{
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateViewImageSize(GetObjectID());
	}
}
*/
/*#291
void	ADocument_Text::UpdateWindowJumpList()
{
	for( AIndex index = 0; index < mWindowIDArray.GetItemCount(); index++ )
	{
		GetTextWindowByIndex(index).SPI_UpdateJumpList(GetObjectID());
	}
}

//R0006
void	ADocument_Text::UpdateWindowJumpListWithColor()
{
	for( AIndex index = 0; index < mWindowIDArray.GetItemCount(); index++ )
	{
		GetTextWindowByIndex(index).SPI_UpdateJumpListWithColor(GetObjectID());
	}
}
*/
/*#379
void	ADocument_Text::UpdateInfoHeader()
{
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_SetInfoHeaderText(GetEmptyText());
	}
}
*/
/*#212
void	ADocument_Text::SPI_RefreshWindow()
{
	RefreshWindow();
}

void	ADocument_Text::RefreshWindow()
{
	for( AIndex index = 0; index < mWindowIDArray.GetItemCount(); index++ )
	{
		if( GetTextWindowByIndex(index).SPI_IsDocumentVisible(GetObjectID()) == true )
		{
			GetTextWindowByIndex(index).NVI_RefreshWindow();
		}
	}
}
*/

//win 080727
//TextView�\���X�V
void	ADocument_Text::SPI_RefreshTextViews()
{
	/*#199 View�̃��\�b�h�𒼐ڃR�[������悤�ɏC��
	for( AIndex index = 0; index < mWindowIDArray.GetItemCount(); index++ )
	{
		if( GetTextWindowByIndex(index).SPI_IsDocumentVisible(GetObjectID()) == true )
		{
			GetTextWindowByIndex(index).SPI_RefreshTextViews(GetObjectID());
		}
	}
	*/
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_RefreshTextView();//TextView��LineNumberView�𗼕��X�V #379
	}
}

//#695
/**
TextView�\���X�V�i�s�͈͎w��j
*/
void	ADocument_Text::SPI_RefreshTextViews( const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_RefreshLines(inStartLineIndex,inEndLineIndex);//TextView��LineNumberView�𗼕��X�V #379
	}
}

//#413
/**
�L�����b�g�ʒu�␳
*/
void	ADocument_Text::SPI_CorrectCaretIfInvalid()
{
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_CorrectCaretIfInvalid();
	}
}

/*#379
void	ADocument_Text::SPI_TryClose()
{
	for( AIndex index = 0; index < mWindowIDArray.GetItemCount(); index++ )
	{
		GetTextWindowByIndex(index).SPI_TryCloseDocument(GetObjectID());
	}
}

void	ADocument_Text::SPI_ExecuteClose()
{
	for( AIndex index = 0; index < mWindowIDArray.GetItemCount(); index++ )
	{
		GetTextWindowByIndex(index).SPI_ExecuteCloseDocument(GetObjectID());
	}
}
*/
ANumber	ADocument_Text::GetTextViewWidth()
{
	/*#379
	if( mWindowIDArray.GetItemCount() > 0 )
	{
		return GetTextWindowByIndex(0).SPI_GetTextViewWidth(GetObjectID());
	}
	*/
	AWindowID	firstWindowID = SPI_GetFirstWindowID();
	if( firstWindowID != kObjectID_Invalid )
	{
		return GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_GetTextViewWidth(GetObjectID());
	}
	//
	return 500;//zantei
}

#pragma mark ===========================

#pragma mark ---�⊮����
//#717

#if 0
/**
@note thread-safe
*/
void	ADocument_Text::SPI_GetImportIdentifierAbbrevCandidate( const AText& inText, const AText& inParentWord,
		ATextArray& outAbbrevCandidateArray,
		ATextArray& outInfoTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
		ATextArray& outFilePathArray ) const
{
	//
	AFileAcc	file;
	NVI_GetFile(file);
	//
	AStMutexLocker	locker(mImportIdentifierLinkListMutex);
	//
	mImportIdentifierLinkList.GetAbbrevCandidate(inText,inParentWord,file,
				outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray,outFilePathArray);
}

/**
���݂̃��[�J�����ʎq����⊮��⌟��
@note thread-safe
*/
void	ADocument_Text::SPI_GetLocalIdentifierAbbrevCandidate( const AText& inText, const AText& inParentWord,
		ATextArray& outAbbrevCandidateArray,
		ATextArray& outInfoTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
		ATextArray& outFilePathArray ) const
{
	AFileAcc	file;
	NVI_GetFile(file);
	AText	path;
	file.GetPath(path);
	//
	AStMutexLocker	locker(mCurrentLocalIdentifierMutex);
	//
	AItemCount	itemCount = mCurrentLocalIdentifier.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		const AText&	keywordText = mCurrentLocalIdentifier.GetObjectConst(i).GetKeywordText();
		if( keywordText.GetItemCount() >= inText.GetItemCount() )
		{
			if( keywordText.CompareMin(inText) == true )
			//test AIndex	pos = 0;
			//test if( keywordText.FindText(0,inText,pos) == true )
			{
				if( outAbbrevCandidateArray.Find(keywordText) == kIndex_Invalid )//�d������L�[���[�h�͕\�����Ȃ�
				{
					outAbbrevCandidateArray.Add(keywordText);
					AText	infotext;
					infotext.SetText(mCurrentLocalIdentifier.GetObjectConst(i).GetInfoText());
					outInfoTextArray.Add(infotext);
					outCategoryIndexArray.Add(mCurrentLocalIdentifier.GetObjectConst(i).GetCategoryIndex());
					outScopeArray.Add(kScopeType_Local);
					outFilePathArray.Add(path);
				}
			}
		}
	}
}
#endif
//#853
/**
�e��L�[���[�h�����X���b�h�p�����L�[���[�h��������(Global���ʎq)
@note �n�b�V�����g��Ȃ��̂Œᑬ�Ȃ��߁A�X���b�h�g�p���K�{�����A���낢��Ȍ������@���ł���Bthread-safe
*/
ABool	ADocument_Text::SPI_SearchKeywordGlobal( const AText& inWord, const AHashTextArray& inParentWord, 
												const AKeywordSearchOption inOption,
												const AIndex inCurrentStateIndex,
												AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
												ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
												ATextArray& outCommentTextArray,
												ATextArray& outCompletionTextArray, ATextArray& outURLArray,
												AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
												AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
												AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray,
												AArray<AItemCount>& outMatchedCountArray,
		const ABool& inAbort ) const
{
	//�L�[���[�h�������s
	ABool	result = mTextInfo.SearchKeywordGlobal(inWord,inParentWord,inOption,
												   inCurrentStateIndex,
												   outKeywordArray,outParentKeywordArray,
												   outTypeTextArray,outInfoTextArray,outCommentTextArray,
												   outCompletionTextArray,outURLArray,
												   outCategoryIndexArray,outColorSlotIndexArray,
												   outStartIndexArray,outEndIndexArray,outScopeArray,outMatchedCountArray,
												   inAbort);
	//outFileArray�𖄂߂�
	AFileAcc	file;
	NVI_GetFile(file);
	AText	path;
	file.GetPath(path);
	if( mTextDocumentType == kTextDocumentType_RemoteFile )
	{
		path.SetText(mRemoteFileURL);
	}
	while( outFilePathArray.GetItemCount() < outKeywordArray.GetItemCount() )
	{
		outFilePathArray.Add(path);
	}
	return result;
}

//#853
/**
�e��L�[���[�h�����X���b�h�p�����L�[���[�h��������(���[�J�����ʎq)
@note �n�b�V�����g��Ȃ��̂Œᑬ�Ȃ��߁A�X���b�h�g�p���K�{�����A���낢��Ȍ������@���ł���Bthread-safe
*/
ABool	ADocument_Text::SPI_SearchKeywordLocal( const AText& inWord, const AHashTextArray& inParentWord, 
											   const AKeywordSearchOption inOption,
											   const AIndex inCurrentStateIndex,
											   AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
											   ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
											   ATextArray& outCommentTextArray,
											   ATextArray& outCompletionTextArray, ATextArray& outURLArray,
											   AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
											   AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
											   AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray, 
											   AArray<AItemCount>& outMatchedCountArray,
											   const AViewID inViewID, 
		const ABool& inAbort ) const
{
	//#893
	//view��index���擾
	AIndex	viewIndex = kIndex_Invalid;
	if( inViewID != kObjectID_Invalid )
	{
		viewIndex = mViewIDArray.Find(inViewID);
	}
	if( viewIndex == kIndex_Invalid )
	{
		//view index���擾�ł��Ȃ��ꍇ�́A���������I��
		return true;
	}
	
	//���[�J���͈͂����ݒ�Ȃ牽���������^�[��
	if( mCurrentLocalIdentifierStartLineIndexArray.Get(viewIndex) == kIndex_Invalid )   return true;
	
	//�L�[���[�h�������s
	ABool	result = mTextInfo.SearchKeywordLocal(inWord,inParentWord,inOption,inCurrentStateIndex,
												  mCurrentLocalIdentifierStartLineIndexArray.Get(viewIndex),
												  mCurrentLocalIdentifierEndLineIndexArray.Get(viewIndex),
												  outKeywordArray,outParentKeywordArray,
												  outTypeTextArray,outInfoTextArray,outCommentTextArray,
												  outCompletionTextArray,outURLArray,
												  outCategoryIndexArray,outColorSlotIndexArray,
												  outStartIndexArray,outEndIndexArray,outScopeArray,outMatchedCountArray,
												  inAbort);
	//outFileArray�𖄂߂�
	AFileAcc	file;
	NVI_GetFile(file);
	AText	path;
	file.GetPath(path);
	if( mTextDocumentType == kTextDocumentType_RemoteFile )
	{
		path.SetText(mRemoteFileURL);
	}
	while( outFilePathArray.GetItemCount() < outKeywordArray.GetItemCount() )
	{
		outFilePathArray.Add(path);
	}
	return result;
}

//#853
/**
�e��L�[���[�h�����X���b�h�p�����L�[���[�h��������(Import���ʎq)
@note �n�b�V�����g��Ȃ��̂Œᑬ�Ȃ��߁A�X���b�h�g�p���K�{�����A���낢��Ȍ������@���ł���Bthread-safe
*/
ABool	ADocument_Text::SPI_SearchKeywordImport( const AText& inWord, const AHashTextArray& inParentWord, 
												const AKeywordSearchOption inOption,
												const AIndex inCurrentStateIndex,
												AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
												ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
												ATextArray& outCommentTextArray,
												ATextArray& outCompletionTextArray, ATextArray& outURLArray,
												AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
												AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
												AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray,
												AArray<AItemCount>& outMatchedCountArray,
												const ABool& inAbort ) const
{
	//ImportIdentifierLinkList�r������
	AStMutexLocker	locker(mImportIdentifierLinkListMutex);
	//�C���|�[�g�L�[���[�h�������s
	return mImportIdentifierLinkList.SearchKeywordImport(inWord,inParentWord,inOption,inCurrentStateIndex,
		outKeywordArray,outParentKeywordArray,
		outTypeTextArray,outInfoTextArray,outCommentTextArray,
		outCompletionTextArray,outURLArray,
		outCategoryIndexArray,outColorSlotIndexArray,
		outStartIndexArray,outEndIndexArray,outScopeArray,outFilePathArray,outMatchedCountArray,
		inAbort);
}

#pragma mark ===========================

#pragma mark ---Import�t�@�C��

//�w�b�_�t�@�C�����̃C���|�[�g�t�@�C���̃f�[�^���X�V����B�i#include�����X�V���ꂽ�ꍇ��A�ŏ��̕��@��͂̂Ƃ��A����сA�C���|�[�g�t�@�C���̉�̓X���b�h�������ɌĂ΂��B�j
void	ADocument_Text::UpdateImportFileData()
{
	if( NVI_IsFileSpecified() == false )   return;
	
	//#349 SDF���g�p���̓C���|�[�g�t�@�C������
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kUseBuiltinSyntaxDefinitionFile) == false &&
	   GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kUseUserDefinedSDF) == false )
	{
		mImportFileArray.DeleteAll();
		AStMutexLocker	locker(mImportIdentifierLinkListMutex);//#717
		mImportIdentifierLinkList.DeleteAll();
		return;
	}
	
	//�C���|�[�g���ʎq�����N���X�g�S�폜
	mImportFileArray.DeleteAll();
	{
		AStMutexLocker	locker(mImportIdentifierLinkListMutex);
		mImportIdentifierLinkList.DeleteAll();
	}
	AFileAcc	file;
	NVI_GetFile(file);
	AFileAcc	folder;
	folder.SpecifyParent(file);
	//win 080722 mTextInfo.GetImportFileArray(folder,mImportFileArray);
	//win 080722
	//SDF��ImportLinkToProjectAll�̐ݒ肪����Ƃ��́A�v���W�F�N�g���̑S�Ẵt�@�C�����C���|�[�g��������
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetImportLinkToProjectAll() == true )
	{
		//�v���W�F�N�g�t�H���_�ݒ�ς݂̏ꍇ�́A�v���W�F�N�g�t�H���_���̓������[�h�̃t�@�C����S�ăC���|�[�g�Ώ�
		AIndex	projectindex = GetApp().SPI_GetSameProjectArrayIndex(mProjectFolder);
		if( projectindex != kIndex_Invalid )
		{
			const ATextArray&	pathnamearray = GetApp().SPI_GetSameProjectPathnameArray(projectindex);
			for( AIndex i = 0; i < pathnamearray.GetItemCount(); i++ )
			{
				const AText&	path = pathnamearray.GetTextConst(i);
				if( GetApp().SPI_GetModePrefDB(mModeIndex).IsThisModeSuffix(path) == true )
				{
					AFileAcc	file;
					file.Specify(path);
					mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(file);
				}
			}
		}
		//�v���W�F�N�g�t�H���_���ݒ�̏ꍇ�́A�����t�H���_�̓������[�h�̃t�@�C�����C���|�[�g�Ώ�
		else
		{
			ATextArray	samefolder;
			GetApp().SPI_GetSameFolderFilenameTextArray(samefolder);
			for( AIndex i = 0; i < samefolder.GetItemCount(); i++ )
			{
				const AText&	filename = samefolder.GetTextConst(i);
				if( GetApp().SPI_GetModePrefDB(mModeIndex).IsThisModeSuffix(filename) == true )
				{
					AFileAcc	file;
					file.SpecifyChild(folder,filename);
					mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(file);
				}
			}
		}
	}
	else
	{
		//���@�F���ɂ���ē���ꂽ�C���|�[�g�t�@�C�����擾
		mTextInfo.GetImportFileArray(folder,mImportFileArray);
		//�h�L�������g�̃t�@�C���Ɠ����t�H���_�̓����ʊg���q�̃t�@�C����ǉ�����i���Ƃ��΁AApp.h�̃h�L�������g����App.cpp�̒�`���Q�Ƃ��邽�߁j
		AText	origfn;
		file.GetFilename(origfn);
		origfn.DeleteAfter(origfn.GetSuffixStartPos());
		//�����t�H���_���̑S�Ẵt�@�C���ɂ��ă��[�h�������A���A�g���q�ʂ��̖��O���ꏏ�Ȃ��̂���������
		ATextArray	samefolder;
		GetApp().SPI_GetSameFolderFilenameTextArray(samefolder);
		for( AIndex i = 0; i < samefolder.GetItemCount(); i++ )
		{
			const AText&	filename = samefolder.GetTextConst(i);
			if( GetApp().SPI_GetModePrefDB(mModeIndex).IsThisModeSuffix(filename) == true )
			{
				AText	fn;
				fn.SetText(filename);
				fn.DeleteAfter(fn.GetSuffixStartPos());
				if( origfn.Compare(fn) == true )
				{
					AFileAcc	folder;
					folder.SpecifyParent(file);
					AFileAcc	f;
					f.SpecifyChild(folder,filename);
					if( file.Compare(f) == false )
					{
						mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(f);
					}
				}
			}
		}
	}
	//test
	/*
	{
		AFileAcc	folder;
		folder.Specify("C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\Include\\");
		AObjectArray<AFileAcc>	children;
		folder.GetChildren(children);
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			AText	filename;
			children.GetObjectConst(i).GetFilename(filename);
			AText	suffix;
			filename.GetSuffix(suffix);
			if( suffix.Compare(".h") == true )
			{
				mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(children.GetObjectConst(i));
			}
		}
	}
	*/
	/*{
		AFileAcc	file;
		file.Specify("C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\Include\\Windows.h");
		mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(file);
	}*/
	/*
	//#253
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kImportSystemHeaderFiles) == true )
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Text(AModePrefDB::kSystemHeaderFiles,text);
		for( AIndex pos = 0; pos < text.GetItemCount();  )
		{
			AText	line;
			text.GetLine(pos,line);
			AFileAcc	folder;
			folder.Specify(line);
			if( folder.IsFolder() == true )
			{
				AObjectArray<AFileAcc>	children;
				folder.GetChildren(children);
				for( AIndex i = 0; i < children.GetItemCount(); i++ )
				{
					AText	filename;
					children.GetObjectConst(i).GetFilename(filename);
					if( GetApp().SPI_GetModePrefDB(mModeIndex).IsThisModeSuffix(filename) == true )
					{
						mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(children.GetObjectConst(i));
					}
				}
			}
			else
			{
				mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(folder);
			}
		}
	}
	*/
	
	//
	//#423 mImportIdentifierLinkList.DeleteAll();//�����K�v�@�K�v�ȕ��������X�V�ł��Ȃ����H
	if( mImportFileArray.GetItemCount() > 0 )
	{
		AStMutexLocker	locker(mImportIdentifierLinkListMutex);//#717
		//�C���|�[�g�t�@�C���ւ̃����N�f�[�^�擾�i����͂�����Ή�͗v���j
		GetApp().SPI_GetImportFileManager().GetImportIdentifierLinkList(GetObjectID(),
			mImportFileArray,SPI_GetModeIndex(),mImportIdentifierLinkList,false);
	}
	else //#423
	{
		AStMutexLocker	locker(mImportIdentifierLinkListMutex);//#717
		mImportIdentifierLinkList.DeleteAll();
	}
	
	//�u�֘A�t�@�C���v���j���[�̍X�V
	GetApp().SPI_UpdateImportFileMenu(SPI_GetImportFileArray());
}

//
void	ADocument_Text::SPI_UpdateImportFileDataIfDirty()
{
	if( mImportFileDataDirty == true )
	{
		UpdateImportFileData();
		//#212 RefreshWindow();
		SPI_RefreshTextViews();//#212
		mImportFileDataDirty = false;
	}
}

/**
�C���|�[�g�t�@�C���̕��@�F����������import file manager����R�[�������
*/
void	ADocument_Text::SPI_DoImportFileRecognized()
{
	UpdateImportFileData();
	//win 080727 RefreshWindow();
	SPI_RefreshTextViews();//win 080727
}

//
const AObjectArray<AFileAcc>&	ADocument_Text::SPI_GetImportFileArray() const
{
	return mImportFileArray;
}

/**
�C���|�[�g���ʎq�����N���X�g����w��t�@�C���̃f�[�^���폜����
*/
ABool	ADocument_Text::SPI_DeleteImportFileData( const AObjectID inImportFileDataObjectID, const AFileAcc& inFile )
{
	//�C���|�[�g���ʎq�����N���X�g����w��t�@�C���̃f�[�^���폜
	//�i�������̂��߁AmRevisionData_ImportFileDataID���猟�����āA��v���Ȃ���΁A�n�b�V�����͌������Ȃ��Ƃ����΍�ς݁j
	AStMutexLocker	locker(mImportIdentifierLinkListMutex);//#717
	return mImportIdentifierLinkList.Delete(inImportFileDataObjectID);
}

//#349
/**
�h�L�������g����Import�t�@�C���f�[�^��S�č폜
AImportFileManager::DeleteAll()����R�[�������B
AImportIdentifierLinkList����AImportFileData�ւ̃����N���\���Ă���̂ŁA
AImportFileData��S�폜����O�ɁA�����N�f�[�^��S�č폜����K�v�����邽�߁B
*/
void	ADocument_Text::SPI_DeleteAllImportFileData()
{
	mImportFileArray.DeleteAll();
	{
		AStMutexLocker	locker(mImportIdentifierLinkListMutex);//#717
		mImportIdentifierLinkList.DeleteAll();
	}
}

#pragma mark ===========================

#pragma mark ---���[�J�����ʎq

void	ADocument_Text::SPI_SetCurrentLocalTextPoint( const ATextPoint& inTextPoint, const AViewID inViewID )
{
	//���@��͊����܂ł́A���[�J���͈͂������Ȃ肷����\��������̂ŁA�������Ȃ� #0
	if( SPI_GetDocumentInitState() != kDocumentInitState_Completed )
	{
		return;
	}
	//#893
	//view��index���擾
	AIndex	viewIndex = kIndex_Invalid;
	if( inViewID != kObjectID_Invalid )
	{
		viewIndex = mViewIDArray.Find(inViewID);
	}
	if( viewIndex == kIndex_Invalid )   return;
	
	//#867 ������
	//�w��e�L�X�g�|�C���g�����݂̃��[�J���͈͓��Ȃ牽���������^�[��
	//AIndex	textIndex = SPI_GetTextIndexFromTextPoint(inTextPoint);
	if( inTextPoint.y >= mCurrentLocalIdentifierStartLineIndexArray.Get(viewIndex) && inTextPoint.y < mCurrentLocalIdentifierEndLineIndexArray.Get(viewIndex) )
	{
		return;
	}
	//
	AIndex	start, end;
	mTextInfo.GetLocalRangeByLineIndex(inTextPoint.y,start,end);
	//���[�J���͈͍X�V
	AIndex	oldStart = mCurrentLocalIdentifierStartLineIndexArray.Get(viewIndex);
	mCurrentLocalIdentifierStartLineIndexArray.Set(viewIndex,start);
	mCurrentLocalIdentifierEndLineIndexArray.Set(viewIndex,end);
	//���[�J�����ʎq���X�g�X�V�i���[�J���J�n�ʒu�ɕύX���������redraw�͂��Ȃ��j
	UpdateLocalIdentifierList(viewIndex,(oldStart != start));
}

/**
���[�J�����ʎq���X�g�X�V
*/
void	ADocument_Text::UpdateLocalIdentifierList( const AIndex viewIndex, const ABool inAlwaysRefresh )
{
	//���[�J���͈͎擾
	AIndex	localStartLineIndex = mCurrentLocalIdentifierStartLineIndexArray.Get(viewIndex);
	AIndex	localEndLineIndex = mCurrentLocalIdentifierEndLineIndexArray.Get(viewIndex);
	//���[�J���͈͖��ݒ莞�͉��������I�� #0
	if( localStartLineIndex == kIndex_Invalid || localEndLineIndex == kIndex_Invalid )
	{
		return;
	}
	//
	AItemCount	oldCount = mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).GetItemCount();
	{
		//local identifier list�r������
		AStMutexLocker	locker(mCurrentLocalIdentifierMutex);//#717
		//local identifier list�擾
		mTextInfo.GetLocalRangeIdentifier(localStartLineIndex,localEndLineIndex,
					this,mCurrentLocalIdentifierListArray.GetObject(viewIndex));
	}
	//���ʎq���ɕω��A�܂��́AinAlwaysRefresh��true�Ȃ�`��X�V
	if( mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).GetItemCount() != oldCount || inAlwaysRefresh == true )
	{
		//RC1
		SPI_UpdateArrowToDef(false);
		//win 080727 RefreshWindow();
		SPI_RefreshTextViews();
	}
}

/**
�w��s���܂ރ��[�J���͈͂��擾
*/
void	ADocument_Text::SPI_GetLocalRangeForLineIndex( const AIndex inLineIndex, AIndex& outStartLineIndex, AIndex& outEndLineIndex ) const
{
	//���@��͊����܂ł́A���[�J���͈͂������Ȃ肷����\��������̂ŁAinvalid��Ԃ� #0
	if( mDocumentInitState != kDocumentInitState_Completed )
	{
		outStartLineIndex = kIndex_Invalid;
		outEndLineIndex = kIndex_Invalid;
		return;
	}
	//
	outStartLineIndex = 0;
	outEndLineIndex = SPI_GetLineCount();
	mTextInfo.GetLocalRangeByLineIndex(inLineIndex,outStartLineIndex,outEndLineIndex);
}

//RC1
void	ADocument_Text::SPI_UpdateArrowToDef( const ABool inRefresh )
{
	//��
	/*
	mCurrentLocalIdentifierAnalyzeDrawData.Init();
	if( mDrawArrowToDef == true )
	{
		mCurrentLocalIdentifierAnalyzeDrawData.flagArrowToDef = true;
		for( AIndex lineIndex = mCurrentLocalIdentifierStartLineIndex; lineIndex < mCurrentLocalIdentifierEndLineIndex; lineIndex++ )
		{
			CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
			ATextPoint	tmp1, tmp2;
			GetTextDrawDataWithStyle(mText,mTextInfo,lineIndex,textDrawData,false,tmp1,tmp2,mCurrentLocalIdentifierAnalyzeDrawData);//#695
		}
	}
	if( inRefresh == true )
	{
		SPI_RefreshTextViews();
	}
	*/
}

//RC1
void	ADocument_Text::ClearArrowToDef()
{
	//��
	/*
	if( mCurrentLocalIdentifierAnalyzeDrawData.defStartArrowToDef.GetItemCount() > 0 )
	{
		mCurrentLocalIdentifierAnalyzeDrawData.Init();
		//#212 RefreshWindow();
		SPI_RefreshTextViews();//#212
	}
	*/
}

//#699
/**
���[�J�����ʎq�����[�J�����ʎq���X�g����S�폜���A���݂̃��[�J���͈͂�����
*/
void	ADocument_Text::SPI_DeleteAllLocalIdentifiers()
{
	for( AIndex viewIndex = 0; viewIndex < mViewIDArray.GetItemCount(); viewIndex++ )
	{
		//���[�J�����ʎq�폜
		AStMutexLocker	locker(mCurrentLocalIdentifierMutex);//#717
		mCurrentLocalIdentifierListArray.GetObject(viewIndex).DeleteAll();
		//���݂̃��[�J���͈͂�����
		mCurrentLocalIdentifierStartLineIndexArray.Set(viewIndex,kIndex_Invalid);
		mCurrentLocalIdentifierEndLineIndexArray.Set(viewIndex,kIndex_Invalid);
	}
}

//#717
/**
���݂̃��[�J���J�n���ʎq�̐e�L�[���[�h�擾�i���T�^�I�ɂ͌��݂̊֐��̃N���X�j
*/
void	ADocument_Text::SPI_GetCurrentLocalStartIdentifierParentKeyword( const AIndex inLineIndex ,AText& outParentKeyword ) const
{
	mTextInfo.GetCurrentLocalStartIdentifierParentKeyword(inLineIndex,outParentKeyword);
}

//#723
/**
�w��s���܂ރ��[�J���͈͂̃��[�J���J�n���ʎq�̃f�[�^���擾
*/
void	ADocument_Text::SPI_GetLocalStartIdentifierDataByLineIndex( const AIndex inLineIndex, 
		AText& outFunctionName, AText& outParentKeywordName, 
		AIndex& outCategoryIndex, ATextPoint& outStart, ATextPoint& outEnd ) const
{
	AIndex	lineIndex = inLineIndex;
	if( lineIndex > SPI_GetLineCount() )   lineIndex = SPI_GetLineCount()-1;
	//�sIndex�ɑΉ����郍�[�J���J�n���ʎq��ObjectID�𓾂�
	AUniqID	identifierUniqID = mTextInfo.GetLocalStartIdentifierByLineIndex(lineIndex);
	if( identifierUniqID != kUniqID_Invalid )
	{
		//���ʎq�̊e�f�[�^�擾
		if( mTextInfo.GetGlobalIdentifierKeywordText(identifierUniqID,outFunctionName) == false )
		{
			//�L�[���[�h�e�L�X�g�������ꍇ�́A���j���[�e�L�X�g���擾
			mTextInfo.GetGlobalIdentifierMenuText(identifierUniqID,outFunctionName);
		}
		//�e�L�[���[�h�擾
		mTextInfo.GetGlobalIdentifierParentKeywordText(identifierUniqID,outParentKeywordName);
		//�J�e�S���擾
		outCategoryIndex = mTextInfo.GetGlobalIdentifierCategoryIndex(identifierUniqID);
		//���ʎq�̈ʒu�擾
		SPI_GetGlobalIdentifierRange(identifierUniqID,outStart,outEnd);
	}
}

#pragma mark ===========================

#pragma mark ---�W�����v���j���[�Ǘ�

/**
�W�����v���j���[�ւ̎��ʎq�ǉ�
@param inIdentifierUniqIDArray �ǉ����ʎq�̔z��i�s���ɕ���ł��邱�ƑO��j
*/
void	ADocument_Text::AddToJumpMenu( const AArray<AUniqID>& inIdentifierUniqIDArray, const AArray<AIndex>& inLineIndexArray )//#695
{
	//�ǉ����ʎq�̌����擾#695
	AItemCount	itemCount = inIdentifierUniqIDArray.GetItemCount();
	//�ǉ������Ȃ牽���������^�[��
	if( itemCount == 0 )   return;
	
	//�ŏ��̍��ڂ̈ʒu�ɁA�S���ڂ�ǉ�����B�i���̂��߁AinIdentifierUniqIDArray�͍s���ɕ���ł���K�v������j
	//�ŏ��̍��ڂ̍sindex�擾
	AIndex	firstItemLineIndex = inLineIndexArray.Get(0);
	//�}���ʒu����
	AIndex	firstItemMenuIndex = 0;
	if( firstItemLineIndex > 0 )
	{
		//�O�̍s��jump index�̎��ɑ}��
		firstItemMenuIndex = SPI_GetJumpMenuItemIndexByLineIndex(firstItemLineIndex-1) + 1;
	}
	
	//�����f�[�^Reserve
	mJump_MenuTextArray.Reserve(firstItemMenuIndex,itemCount);
	mJump_MenuTextStyleArray.Reserve(firstItemMenuIndex,itemCount);
	mJump_MenuColorArray.Reserve(firstItemMenuIndex,itemCount);
	mJump_MenuOutlineLevelArray.Reserve(firstItemMenuIndex,itemCount);//#130
	
	//�F�擾
	AColor	subWindowNormalColor = kColor_List_Normal;//#1316 GetApp().SPI_GetSubWindowLetterColor();
	
	//�e���ڒǉ�
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//uniq id�擾
		AUniqID	identifierUniqID = inIdentifierUniqIDArray.Get(i);
		//���j���[�e�L�X�g�擾
		AText	menutext;
		mTextInfo.GetGlobalIdentifierMenuText(identifierUniqID,menutext);
		AText	actiontext;
		actiontext.SetFormattedCstring("%d",identifierUniqID.val);
		AIndex	categoryIndex = mTextInfo.GetGlobalIdentifierCategoryIndex(identifierUniqID);
		ATextStyle	style = GetApp().SPI_GetModePrefDB(mModeIndex).GetCategoryMenuTextStyle(categoryIndex);
		//#130 �A�E�g���C�����x���擾
		AIndex	outlineLevel = mTextInfo.GetGlobalIdentifierOutlineLevel(identifierUniqID);
		
		//���j���[�}��
		//�}���ʒu�������l���O�̏ꍇ�̂݃��j���[�֑}������ #695
		//����ɂ��̃��[�v�̉��Ń��j���[�̐����l�ȏ�̍��ڂ��폜���Ă���̂ŁA���j���[���ڂ̑����͏�ɐ����l�܂łɐ��������
		//���j���[�I�����A�폜���́AUniqID���L�[�ɂ��Č������Ă���B�폜�́A���݂���ꍇ�̂ݍ��ڍ폜���Ă���B
		if( firstItemMenuIndex + i < kLimit_MaxJumpMenuItemCount )//#695
		{
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByObjectID(
						mJumpMenuObjectID,firstItemMenuIndex + i,menutext,actiontext,kObjectID_Invalid,0,0,false);
		}
		
		//�����f�[�^�ɑ}��
		mJump_IdentifierObjectArray.Insert1(firstItemMenuIndex + i,identifierUniqID);
		//�����f�[�^�ɒl�ݒ�
		mJump_MenuTextArray.Set(firstItemMenuIndex + i,menutext);
		mJump_MenuTextStyleArray.Set(firstItemMenuIndex + i,style);
		mJump_MenuColorArray.Set(firstItemMenuIndex + i,subWindowNormalColor);
		mJump_MenuOutlineLevelArray.Set(firstItemMenuIndex + i,outlineLevel);//#130
	}
	//���j���[�̐����l�ȏ�̍��ڂ��폜 #695
	while( GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(mJumpMenuObjectID) > kLimit_MaxJumpMenuItemCount )
	{
		GetApp().NVI_GetMenuManager().DeleteDynamicMenuItemByObjectID(mJumpMenuObjectID,kLimit_MaxJumpMenuItemCount);
	}
	//�W�����v���X�g�}���X�V#695
	SPI_UpdateJumpList_Inserted(firstItemMenuIndex,itemCount);
}

/**
�W�����v���j���[����w��Identifier�ɑΉ����鍀�ڂ��폜
@param inIdentifierUniqIDArray �폜���ׂ�Identifier��UniqID�i���łɍ폜�ς݂ł��ǂ��B�܂��Amenu�\���ΏۂłȂ��Ă��ǂ��j
@note �����s�܂Ƃ߂č폜����ꍇ�́A�e�s���ƂɌĂԂ̂ł͂Ȃ��A�܂Ƃ߂ăR�[�����邱�ƁB�i���̊֐�����MakeHash()�𖈉���s���邽�߁j
*/
ABool	ADocument_Text::DeleteFromJumpMenu( const AArray<AUniqID>& inIdentifierUniqIDArray )
{
	ABool	jumpMenuChanged = false;
	//deleteMenuItemIndexArray�ɍ폜���鍀�ڂ�jump index���擾
	AArray<AIndex>	deleteMenuItemIndexArray;//#695
	AItemCount	identifierItemCount = inIdentifierUniqIDArray.GetItemCount();
	for( AIndex i = 0; i < identifierItemCount; i++ )
	{
		AUniqID	UniqID = inIdentifierUniqIDArray.Get(i);
		AIndex	menuIndex = mJump_IdentifierObjectArray.Find(UniqID);
		if( menuIndex != kIndex_Invalid )
		{
			deleteMenuItemIndexArray.Add(menuIndex);//#695
		}
	}
	//�~���\�[�g�i���̃��[�v�Ŋe�z��̌��̍��ڂ���폜���Ă������߁j
	deleteMenuItemIndexArray.Sort(false);
	//mJump_IdentifierObjectArray��Hash���ꎞ�I��disable�i�������̂��߁j
	mJump_IdentifierObjectArray.SetEnableHash(false);//#695
	//�e���ڍ폜
	AItemCount	deleteMenuItemIndexArrayCount = deleteMenuItemIndexArray.GetItemCount();//#695
	for( AIndex i = 0; i < deleteMenuItemIndexArrayCount; i++ )//#695
	{
		AIndex	menuIndex = deleteMenuItemIndexArray.Get(i);//#695
		//#695 GetApp().NVI_GetMenuManager().DeleteDynamicMenuItemByObjectID(mJumpMenuObjectID,menuIndex);
		//�W�����v���j���[����폜
		AText	actiontext;
		actiontext.SetNumber(mJump_IdentifierObjectArray.Get(menuIndex).val);
		AIndex	jumpMenuItemIndex = GetApp().NVI_GetMenuManager().
				FindDynamicMenuItemIndexFromActionText(mJumpMenuObjectID,actiontext);
		if( jumpMenuItemIndex != kIndex_Invalid )
		{
			GetApp().NVI_GetMenuManager().DeleteDynamicMenuItemByObjectID(mJumpMenuObjectID,jumpMenuItemIndex);
		}
		//�����f�[�^����폜
		mJump_MenuTextArray.Delete1(menuIndex);
		mJump_MenuTextStyleArray.Delete1(menuIndex);
		mJump_IdentifierObjectArray.Delete1(menuIndex);
		//#699 ���g�p�̂��ߍ폜mJump_ParagraphTextArray.Delete1(menuIndex);
		mJump_MenuColorArray.Delete1(menuIndex);//R0006
		mJump_MenuOutlineLevelArray.Delete1(menuIndex);//#130
		//�W�����v���X�g�폜#695
		SPI_UpdateJumpList_Deleted(menuIndex);
		//�ύX�t���Oon
		jumpMenuChanged = true;
	}
	//mJump_IdentifierObjectArray��Hash�̈ꎞ�Idisable���������A�ēxMakeHash
	mJump_IdentifierObjectArray.SetEnableHash(true);//#695
	mJump_IdentifierObjectArray.MakeHash();//#695
	return jumpMenuChanged;
}

//#695
/**
�W�����v���ڑS�폜
*/
void	ADocument_Text::DeleteAllJumpItems()
{
	//���j���[�S�폜
	GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByObjectID(mJumpMenuObjectID);
	//�����f�[�^�S�폜
	mJump_MenuTextArray.DeleteAll();
	mJump_MenuTextStyleArray.DeleteAll();
	mJump_IdentifierObjectArray.DeleteAll();
	mJump_MenuColorArray.DeleteAll();
	mJump_MenuOutlineLevelArray.DeleteAll();//#130
	//�W�����v���X�g�X�V
	SPI_UpdateJumpList();//#695 true);
}

//�sIndex����W�����v���j���[����Index���擾
AIndex	ADocument_Text::SPI_GetJumpMenuItemIndexByLineIndex( const AIndex inLineIndex ) const
{
	//�������s���l�Ȃ�A�Y�����j���[���ڂȂ��ŕԂ�
	if( inLineIndex >= SPI_GetLineCount() || inLineIndex < 0 )   return kIndex_Invalid;
	
	//�i���擾
	AIndex	paragraphIndex = mTextInfo.GetParagraphIndexByLineIndex(inLineIndex);
	AItemCount	paragraphCount = SPI_GetParagraphCount();
	
	//�W�����v���ڐ��擾
	AItemCount	jumpItemCount = mJump_IdentifierObjectArray.GetItemCount();
	if( jumpItemCount == 0 )
	{
		//�Y���W�����v���ږ���
		return kIndex_Invalid;
	}
	
	//�ŏ��̃W�����v���ڂ����O�Ȃ�Y���W�����v���ږ���
	if( paragraphIndex < mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(0)) )
	{
		return kIndex_Invalid;
	}
	
	//�L���b�V������
	//�O��̃q�b�g�ʒu�imCacheForGetJumpMenuItemIndexByLineIndex�j�ƁA���̂P�オ�Y������Ȃ�A�����Ԃ�
	if( mCacheForGetJumpMenuItemIndexByLineIndex != kIndex_Invalid )
	{
		for( AIndex jumpIndex = mCacheForGetJumpMenuItemIndexByLineIndex; 
					((jumpIndex < jumpItemCount)&&(jumpIndex <= mCacheForGetJumpMenuItemIndexByLineIndex+1));
					jumpIndex++ )
		{
			//jumpIndex��jump���ڂ̊J�n�i���擾
			AIndex	currentMenuParagraphIndex = 
					mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(jumpIndex));
			//jumpIndex�̎���jump���ڂ̊J�n�i���擾
			AIndex	nextMenuParagraphIndex = paragraphCount;
			if( jumpIndex+1 < jumpItemCount )
			{
				nextMenuParagraphIndex = 
						mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(jumpIndex+1));
			}
			//�͈͓��Ɍ����Ώےi���������Ă���Ȃ炻��jump���ڂ�Ԃ��B
			if( paragraphIndex >= currentMenuParagraphIndex && paragraphIndex < nextMenuParagraphIndex ) 
			{
				mCacheForGetJumpMenuItemIndexByLineIndex = jumpIndex;
				return jumpIndex;
			}
		}
	}
	
	//2���@
	AIndex	startJumpIndex = 0;
	AIndex	endJumpIndex = jumpItemCount;
	AIndex	jumpIndex = (startJumpIndex+endJumpIndex)/2;
	for( AIndex i = 0; i < jumpItemCount*2; i++ )//�������[�v�h�~�i�����Ƃ��s����̃��[�v�ŏI���͂��j
	{
		//jumpIndex��jump���ڂ̊J�n�i���擾
		AIndex	currentMenuParagraphIndex = mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(jumpIndex));
		//jumpIndex�̎���jump���ڂ̊J�n�i���擾
		AIndex	nextMenuParagraphIndex = paragraphCount;
		if( jumpIndex+1 < jumpItemCount )
		{
			nextMenuParagraphIndex = mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(jumpIndex+1));
		}
		//�͈͓��Ɍ����Ώےi���������Ă���Ȃ炻��jump���ڂ�Ԃ��B
		if( paragraphIndex >= currentMenuParagraphIndex && paragraphIndex < nextMenuParagraphIndex ) 
		{
			mCacheForGetJumpMenuItemIndexByLineIndex = jumpIndex;
			return jumpIndex;
		}
		//�����Ώےi����jumpIndex�̊J�n�i�����O�Ȃ�O�͈̔͂ōċA�I�Ɍ���
		if( paragraphIndex < currentMenuParagraphIndex )
		{
			endJumpIndex = jumpIndex;
			jumpIndex = (startJumpIndex+endJumpIndex)/2;
		}
		//�����Ώےi�������̊J�n�i���ȍ~�Ȃ���͈̔͂ōċA�I�Ɍ���
		else if( paragraphIndex >= nextMenuParagraphIndex )
		{
			startJumpIndex = jumpIndex+1;
			jumpIndex = (startJumpIndex+endJumpIndex)/2;
		}
		//start��end������ւ�����ꍇ�̏����i�������Ȃ��͂��j
		if( startJumpIndex >= endJumpIndex )
		{
			_ACError("",NULL);
			if( jumpIndex >= jumpItemCount )
			{
				jumpIndex = jumpItemCount-1;
			}
			mCacheForGetJumpMenuItemIndexByLineIndex = jumpIndex;
			return jumpIndex;
		}
	}
	_ACError("cannot find jump menu index",this);
	return kIndex_Invalid;
	/*#695 �������̂���mJump_IdentifierObjectArray����2���@�ŊY������menu identifier�������邱�Ƃɂ���
	AIndex	lineIndex = inLineIndex;
	if( lineIndex > SPI_GetLineCount() )   lineIndex = SPI_GetLineCount()-1;
	//�sIndex�ɑΉ����郁�j���[�o�^Identifier��UniqID�𓾂�
	AUniqID	identifierUniqID = mTextInfo.GetMenuIdentifierByLineIndex(lineIndex);
	if( identifierUniqID == kUniqID_Invalid )   return kIndex_Invalid;
	
	*#698
	//
	for( AIndex itemindex = 0; itemindex < mJump_IdentifierObjectArray.GetItemCount(); itemindex++ )
	{
		if( mJump_IdentifierObjectArray.Get(itemindex) == identifierUniqID )   return itemindex;
	}
	return kIndex_Invalid;
	*
	return mJump_IdentifierObjectArray.Find(identifierUniqID);
	*/
}

void	ADocument_Text::SPI_GetJumpMenuTextByMenuItemIndex( const AIndex inMenuItemIndex, AText& outText ) const
{
	mJump_MenuTextArray.Get(inMenuItemIndex,outText);
}

//R0212
void	ADocument_Text::SPI_GetJumpMenuItemInfoTextByLineIndex( const AIndex inLineIndex, AText& outText ) const
{
	AIndex	lineIndex = inLineIndex;
	if( lineIndex > SPI_GetLineCount() )   lineIndex = SPI_GetLineCount()-1;
	//�sIndex�ɑΉ����郁�j���[�o�^Identifier��UniqID�𓾂�
	//#695 AUniqID	identifierUniqID = mTextInfo.GetMenuIdentifierByLineIndex(lineIndex);
	//#695 if( identifierUniqID == kUniqID_Invalid )   return;
	AIndex	jumpIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);//#695
	if( jumpIndex != kIndex_Invalid )//#695
	{
		AUniqID identifierUniqID = mJump_IdentifierObjectArray.Get(jumpIndex);//#695
		mTextInfo.GetGlobalIdentifierInfoText(identifierUniqID,outText);
	}
}

//#454
/**
*/
void	ADocument_Text::SPI_RevealFromJumpMenuText( const AText& inText ) const
{
	AIndex	index = mJump_MenuTextArray.Find(inText);
	if( index == kIndex_Invalid )   return;
	
	if( mViewIDArray.GetItemCount() > 0 )
	{
		ATextPoint	spt,ept;
		SPI_GetGlobalIdentifierRange(mJump_IdentifierObjectArray.Get(index),spt,ept);
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_SetSelect(spt,ept);
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_AdjustScroll_Top();
	}
}

//
AObjectID	ADocument_Text::SPI_GetJumpMenuObjectID() const
{
	return mJumpMenuObjectID;
}

//
AUniqID	ADocument_Text::SPI_GetJumpMenuIdentifierUniqID( const AIndex inIndex ) const
{
	return mJump_IdentifierObjectArray.Get(inIndex);
}

//#723
/**
�w��s���܂�jump menu���ڂ̃f�[�^���擾
*/
void	ADocument_Text::SPI_GetJumpMenuItemKeywordTextByLineIndex( const AIndex inLineIndex, 
		AText& outFunctionName, AText& outParentKeywordName, 
		AIndex& outCategoryIndex, ATextPoint& outStart, ATextPoint& outEnd ) const
{
	AIndex	lineIndex = inLineIndex;
	if( lineIndex > SPI_GetLineCount() )   lineIndex = SPI_GetLineCount()-1;
	//�sIndex�ɑΉ����郁�j���[�o�^Identifier��ObjectID�𓾂�
	AUniqID	identifierUniqID = mTextInfo.GetMenuIdentifierByLineIndex(lineIndex);
	if( identifierUniqID != kUniqID_Invalid )
	{
		//���ʎq�̊e�f�[�^�擾
		if( mTextInfo.GetGlobalIdentifierKeywordText(identifierUniqID,outFunctionName) == false )
		{
			//�L�[���[�h�e�L�X�g��������΁A���j���[�e�L�X�g�擾
			mTextInfo.GetGlobalIdentifierMenuText(identifierUniqID,outFunctionName);
		}
		//�e�L�[���[�h�擾
		mTextInfo.GetGlobalIdentifierParentKeywordText(identifierUniqID,outParentKeywordName);
		//�J�e�S���擾
		outCategoryIndex = mTextInfo.GetGlobalIdentifierCategoryIndex(identifierUniqID);
		//���ʎq�ʒu�擾
		SPI_GetGlobalIdentifierRange(identifierUniqID,outStart,outEnd);
	}
}

//#906
/**
���o���p�X�擾
*/
void	ADocument_Text::SPI_GetHeaderPathArray( const AIndex inLineIndex, 
		ATextArray& outHeaderPathTextArray, AArray<AIndex>& outHeaderParagraphIndexArray ) const
{
	//���ʏ�����
	outHeaderPathTextArray.DeleteAll();
	outHeaderParagraphIndexArray.DeleteAll();
	//�w��s�ɑΉ����錩�o��index�擾
	AIndex	jumpItemIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);
	if( jumpItemIndex == kIndex_Invalid )
	{
		return;
	}
	//���o���e�L�X�g�擾
	AText	headerText;
	mJump_MenuTextArray.Get(jumpItemIndex,headerText);
	outHeaderPathTextArray.Add(headerText);
	//���o�����ʎq�ʒu�擾
	AUniqID	uniqID = mJump_IdentifierObjectArray.Get(jumpItemIndex);
	ATextPoint	spt = {0}, ept = {0};
	SPI_GetGlobalIdentifierRange(uniqID,spt,ept);
	outHeaderParagraphIndexArray.Add(SPI_GetParagraphIndexByLineIndex(spt.y));
	//�A�E�g���C�����x���擾
	AIndex	currentOutlineLevel = mJump_MenuOutlineLevelArray.Get(jumpItemIndex);
	//���o����O�����Ɍ���
	for( AIndex i = jumpItemIndex -1; i >= 0; i-- )
	{
		//�A�E�g���C�����x���擾
		AIndex	outlineLevel = mJump_MenuOutlineLevelArray.Get(i);
		//�A�E�g���C�����x�����������Ȃ�����A���ʂɒǉ�
		if( outlineLevel < currentOutlineLevel )
		{
			//���o���e�L�X�g�擾
			mJump_MenuTextArray.Get(i,headerText);
			outHeaderPathTextArray.Add(headerText);
			//���ʎq�ʒu�擾
			AUniqID	uniqID = mJump_IdentifierObjectArray.Get(i);
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetGlobalIdentifierRange(uniqID,spt,ept);
			outHeaderParagraphIndexArray.Add(SPI_GetParagraphIndexByLineIndex(spt.y));
			//���݂̃A�E�g���C�����x���X�V
			currentOutlineLevel = outlineLevel;
		}
	}
}

//#875
/**
�w��s���܂ތ��o���̃A�E�g���C�����x���擾
*/
AIndex	ADocument_Text::SPI_GetOutlineLevel( const AIndex inLineIndex ) const
{
	//�w��s�ɑΉ����錩�o��index�擾
	AIndex	jumpItemIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);
	if( jumpItemIndex == kIndex_Invalid )
	{
		return 0;
	}
	//�A�E�g���C�����x���擾
	return mJump_MenuOutlineLevelArray.Get(jumpItemIndex);
}

#pragma mark ===========================

#pragma mark --- �W�����v���X�g�Ǘ�
//#291

//�J����ID
const ADataID			kColumn_Jump					= 0;

/**
�W�����v���X�g��ListView��o�^
*/
void	ADocument_Text::SPI_RegisterJumpListView( const AViewID inJumpListViewID )
{
	mJumpListArray_ViewID.Add(inJumpListViewID);
	mJumpListArray_Sort.Add(false);
	//�W�����v���X�g�����ݒ�
	GetJumpListView(inJumpListViewID).SPI_RegisterColumn_Text(kColumn_Jump,0,"",false);
	GetJumpListView(inJumpListViewID).SPI_SetEnableCursorRow();
	//#725 UpdateJumpListProperty(inJumpListViewID);
	//�W�����v���X�g���e�쐬
	SPI_UpdateJumpList();//#695 true);
}

/**
�W�����v���X�g��ListView��o�^����
*/
void	ADocument_Text::SPI_UnregisterJumpListView( const AViewID inJumpListViewID )
{
	AIndex	index = mJumpListArray_ViewID.Find(inJumpListViewID);
	if( index == kIndex_Invalid )   { _ACError("",this); return; }
	
	mJumpListArray_ViewID.Delete1(index);
	mJumpListArray_Sort.Delete1(index);
}

/**
�W�����v���X�g��ListView�擾
*/
AView_List&	ADocument_Text::GetJumpListView( const AViewID inViewID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_List,kViewType_List,inViewID);
}

//#695
/**
�W�����v���X�g�X�V�i�s�폜�j
*/
void	ADocument_Text::SPI_UpdateJumpList_Deleted( const AIndex inIndex )
{
	//�\�[�g�{�^���X�V�K�v�t���O
	ABool	sortButtonShouldBeUpdated = false;
	//�e�W�����v���X�g�̍X�V
	for( AIndex i = 0; i < mJumpListArray_ViewID.GetItemCount(); i++ )
	{
		AViewID	viewID = mJumpListArray_ViewID.Get(i);
		//�W�����v���X�g�̍s�폜
		GetJumpListView(viewID).SPI_DeleteTableRow(inIndex);
		//�����f�[�^�̃\�[�g��ԉ����iSPI_DeleteTableRow()���Ń\�[�g��������邽�߁B�j
		if( mJumpListArray_Sort.Get(i) == true )
		{
			mJumpListArray_Sort.Set(i,false);
			sortButtonShouldBeUpdated = true;
		}
	}
	//�\�[�g�{�^����ԍX�V�i�\�[�g��ԉ����j
	if( sortButtonShouldBeUpdated == true )
	{
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			AWindowID	textWindowID = AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetTextWindowID();
			if( textWindowID != kObjectID_Invalid )
			{
				//��#725 GetApp().SPI_GetTextWindowByID(textWindowID).SPI_GetJumpListWindow().SPI_UpdateSortButtonStatus(GetObjectID(),false);
			}
		}
	}
	
	//�I����SPI_DeleteTableRow()�ŉ�������邪�A
	//�ҏW���̓L�����b�g�ݒ莞��NVIDO_SelectionChanged()����SPI_UpdateJumpListSelection()���R�[������邱�Ƃɂ��A
	//�X���b�h���@�F�����́ACopySyntaxRecognizerResult()����SPI_UpdateJumpListSelection()���R�[������邱�Ƃɂ��A
	//���݂̃L�����b�g�ʒu���ēx�I�������B
}

//#695
/**
�W�����v���X�g�X�V�i�s�}���j
*/
void	ADocument_Text::SPI_UpdateJumpList_Inserted( const AIndex inIndex, const AItemCount inCount )
{
	//�F�擾
	AColor	subWindowNormalColor = kColor_List_Normal;//#1316 GetApp().SPI_GetSubWindowLetterColor();
	
	//�\�[�g�{�^���X�V�K�v�t���O
	ABool	sortButtonShouldBeUpdated = false;
	//�e�W�����v���X�g�̍X�V
	for( AIndex i = 0; i < mJumpListArray_ViewID.GetItemCount(); i++ )
	{
		AViewID	viewID = mJumpListArray_ViewID.Get(i);
		//�W�����v���X�g�̍s�}���E�ݒ�
		GetJumpListView(viewID).SPI_InsertTableRows(inIndex,inCount,
													kTextStyle_Normal,subWindowNormalColor,false,kColor_White,false);
		for( AIndex index = inIndex; index < inIndex + inCount; index++ )
		{
			AText	menutext;
			mJump_MenuTextArray.Get(index,menutext);
			GetJumpListView(viewID).SPI_SetTableRow_Text(kColumn_Jump,index,
						menutext,mJump_MenuTextStyleArray.Get(index));
			GetJumpListView(viewID).SPI_SetTableRow_OutlineFoldingLevel(index,mJump_MenuOutlineLevelArray.Get(index));//#130
		}
		//�����f�[�^�̃\�[�g��ԉ����iSPI_InsertTableRows()���Ń\�[�g��������邽�߁j
		if( mJumpListArray_Sort.Get(i) == true )
		{
			mJumpListArray_Sort.Set(i,false);
			sortButtonShouldBeUpdated = true;
		}
	}
	//�\�[�g�{�^����ԍX�V�i�\�[�g��ԉ����j
	if( sortButtonShouldBeUpdated == true )
	{
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			AWindowID	textWindowID = AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetTextWindowID();
			if( textWindowID != kObjectID_Invalid )
			{
				//��#725 GetApp().SPI_GetTextWindowByID(textWindowID).SPI_GetJumpListWindow().SPI_UpdateSortButtonStatus(GetObjectID(),false);
			}
		}
	}
	
	//�I����SPI_InsertTableRows()�ŉ�������邪�A
	//�ҏW���̓L�����b�g�ݒ莞��NVIDO_SelectionChanged()����SPI_UpdateJumpListSelection()���R�[������邱�Ƃɂ��A
	//�X���b�h���@�F�����́ACopySyntaxRecognizerResult()����SPI_UpdateJumpListSelection()���R�[������邱�Ƃɂ��A
	//���݂̃L�����b�g�ʒu���ēx�I�������B
}

//#695
/**
�W�����v���X�g�̕`��X�V
*/
void	ADocument_Text::SPI_RefreshJumpList()
{
	for( AIndex i = 0; i < mJumpListArray_ViewID.GetItemCount(); i++ )
	{
		AViewID	viewID = mJumpListArray_ViewID.Get(i);
		GetJumpListView(viewID).NVI_Refresh();
	}
}

/**
�W�����v���X�g�X�V
*/
void	ADocument_Text::SPI_UpdateJumpList()//#695  const ABool inForceSetAll )
{
	for( AIndex i = 0; i < mJumpListArray_ViewID.GetItemCount(); i++ )
	{
		AViewID	viewID = mJumpListArray_ViewID.Get(i);
		/*#695
		AIndex	svSelected = GetJumpListView(viewID).SPI_GetSelectedDBIndex();
		const ATextArray&	oldTextArray = GetJumpListView(viewID).SPI_GetColumn_Text(kColumn_Jump);
		if( mJump_MenuTextArray.GetItemCount() == oldTextArray.GetItemCount() && inForceSetAll == false )//R0006
		{
			//�O�ƃ��X�g�̌��������ŁA���A����inForceSetAll��false�̏ꍇ�́AText�̈Ⴄ�ӏ������X�V����
			AItemCount	count = mJump_MenuTextArray.GetItemCount();
			for( AIndex i = 0; i < count; i++ )
			{ 
				if( mJump_MenuTextArray.GetTextConst(i).Compare(oldTextArray.GetTextConst(i)) == false )
				{
					GetJumpListView(viewID).SPI_SetTableRow_Text(kColumn_Jump,i,mJump_MenuTextArray.GetTextConst(i),mJump_MenuTextStyleArray.Get(i));
					GetJumpListView(viewID).SPI_SetTableRow_OutlineFoldingLevel(i,mJump_MenuOutlineLevelArray.Get(i));//#130
				}
			}
		}
		else
		*/
		{
			//��L�ȊO�̏ꍇ�͑S�čX�V
			GetJumpListView(viewID).SPI_SetSelect(kIndex_Invalid);
			GetJumpListView(viewID).SPI_BeginSetTable();
			GetJumpListView(viewID).SPI_SetColumn_Text(kColumn_Jump,mJump_MenuTextArray);
			GetJumpListView(viewID).SPI_SetTable_TextStyle(mJump_MenuTextStyleArray);
			GetJumpListView(viewID).SPI_SetTable_Color(mJump_MenuColorArray);//R0006
			GetJumpListView(viewID).SPI_SetTable_OutlineFoldingLevel(mJump_MenuOutlineLevelArray);//#130
			GetJumpListView(viewID).SPI_EndSetTable();
		}
		//�\�[�g�X�V
		if( mJumpListArray_Sort.Get(i) == true )
		{
			GetJumpListView(viewID).SPI_Sort(kColumn_Jump,true);
		}
		/*#695
		//�I�𕜌�
		if( mJump_MenuTextArray.GetItemCount() == oldTextArray.GetItemCount() )
		{
			GetJumpListView(viewID).SPI_SetSelectByDBIndex(svSelected);
		}
		*/
		//�W�����v���X�g�̑I���X�V
		AWindowID	winID = SPI_GetFirstWindowID();
		if( winID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextWindowByID(winID).SPI_UpdateJumpListSelection(true);
		}
		//�`��X�V
		GetJumpListView(viewID).NVI_Refresh();//win
	}
}

/**
�W�����v���X�g�I���X�V
*/
void	ADocument_Text::SPI_UpdateJumpListSelection( const AIndex inLineIndex, const ABool inAdjustScroll )//#698
{
	AIndex	newIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);//#454
	for( AIndex i = 0; i < mJumpListArray_ViewID.GetItemCount(); i++ )
	{
		AViewID	viewID = mJumpListArray_ViewID.Get(i);
		//#454 AIndex	newIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);
		if( newIndex != GetJumpListView(viewID).SPI_GetSelectedDBIndex() )
		{
			GetJumpListView(viewID).SPI_SetSelectByDBIndex(newIndex,inAdjustScroll);//#698
		}
		//#212
		//���ꂪ����ƁAfilter����ɂ����Ƃ��Ƀt�H�[�J�X��filter��������Ă��܂����A�K�v�Ǝv���Ȃ��̂ō폜GetJumpListView(viewID).NVI_RevealView(false);
	}
	//#454 �W�����v���X�g����
	//�O��̑I���ƕύX������Η����X�V
	if( newIndex != mLastSelectedJumpListItemIndex )
	{
		mLastSelectedJumpListItemIndex = newIndex;
		AFileAcc	file;
		if( newIndex != kIndex_Invalid && NVI_GetFile(file) == true )
		{
			GetApp().SPI_UpdateJumpListHistory(mJump_MenuTextArray.GetTextConst(newIndex),file);
		}
	}
}

/**
�W�����v���X�g�\�[�g���[�h�ݒ�
*/
void	ADocument_Text::SPI_SetJumpListSortMode( const AViewID inViewID, const ABool inSort )
{
	AIndex	index = mJumpListArray_ViewID.Find(inViewID);
	if( index != kIndex_Invalid )
	{
		if( mJumpListArray_Sort.Get(index) != inSort )
		{
			if( inSort == true )
			{
				GetJumpListView(inViewID).SPI_Sort(kColumn_Jump,true);
			}
			else
			{
				GetJumpListView(inViewID).SPI_SortOff();
			}
		}
		mJumpListArray_Sort.Set(index,inSort);
	}
}

/**
�W�����v���X�g�t�H���g���ݒ�i�S�āj
*/
/*#725
void	ADocument_Text::SPI_UpdateJumpListProperty()
{
	for( AIndex i = 0; i < mJumpListArray_ViewID.GetItemCount(); i++ )
	{
		AViewID	viewID = mJumpListArray_ViewID.Get(i);
		UpdateJumpListProperty(viewID);
	}
}
*/

/**
�W�����v���X�g�t�H���g���ݒ�i�ʁj
*/
/*#725
void	ADocument_Text::UpdateJumpListProperty( const AViewID inViewID )
{
	AText	fontname;
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kJumpListFontName,fontname);
	GetJumpListView(inViewID).SPI_SetTextDrawProperty(fontname,
			GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kJumpListFontSize),kColor_Black);
	GetJumpListView(inViewID).SPI_SetWheelScrollPercent(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent),
			GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd));
	GetJumpListView(inViewID).NVI_Refresh();
}
*/

#pragma mark ===========================

#pragma mark ---�C���f���g�擾

//
AItemCount	ADocument_Text::SPI_CalcIndentCount( const AIndex inLineIndex, const AItemCount inPrevParagraphIndent, 
			AItemCount& outNextParagraphIndent,
			const AIndentTriggerType inIndentTriggerType )//#650
{
	//�w��i���̊J�n�s���擾
	AIndex	currentParagraphStartLineIndex = mTextInfo.GetCurrentParagraphStartLineIndex(inLineIndex);
	//�w��i���̃C���f���g�ʎ擾
	AItemCount	currentIndentCount = SPI_GetCurrentIndentCount(currentParagraphStartLineIndex);
	//���i���̊J�n�s���擾
	AIndex	nextParagraphStartLineIndex = mTextInfo.GetNextParagraphStartLineIndex(inLineIndex);
	
	//�C���f���g���s�s�̕��@���F���̏ꍇ�́A�����Łi���C���X���b�h���Łj�F������B
	//�i#905�ɂ��ҏW���̕��@�F�������Ȃ��Ȃ����̂ŁA���s�ɂ��V�K�s�̃C���f���g�ȂǁA���@�F�����܂��̏ꍇ�����肤�邽�߁B�j
	if( SPI_GetLineRecognized(inLineIndex) == false )
	{
		RecognizeSyntaxInMainThread(inLineIndex,inLineIndex+1);
		//inLineIndex+1�܂Ō���ŕ��@�F������̂ŁA#endif+���s�ǉ������A����ȍ~�̍s�̏�ԕύX������Ƃ��ɂ́A�ゾ�����ƁA��ԕύX����Ȃ��B
		//���̂��߁A���L�̂悤�ɁAinLineIndex+1�̔F���ς݃t���O�𗎂Ƃ��Ă����B�^�C�}�[�����㓙�ɁA�X���b�h�ŔF�������B
		if( inLineIndex+1 < SPI_GetLineCount() )
		{
			mTextInfo.ClearRecognizeFlags(inLineIndex+1);
		}
	}
	
	//�w��s�J�n�ʒu���R�����g��Ԃ��ǂ����̔���
	AIndex	stateIndex = 0;
	AIndex	pushedStateIndex = 0;
	mTextInfo.GetLineStateIndex(inLineIndex,stateIndex,pushedStateIndex);
	ABool	isDisableIndentLine = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSyntaxDefinition().
			GetSyntaxDefinitionState(stateIndex).IsDisableIndent();
	ABool	isCommentLine = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSyntaxDefinition().
			GetSyntaxDefinitionState(stateIndex).IsCommentState();
	
	AIndentType	indentType = SPI_GetIndentType();
	
	//==================�C���f���g����==================
	//���̃C���f���g���ێ�����
	if( indentType == kIndentType_NoIndent || isDisableIndentLine == true )
	{
		//���̃C���f���g�Ɠ����C���f���g�i�C���f���g�ێ��j
		return currentIndentCount;
	}
	
	//==================�O�s�Ɠ����C���f���g==================
	//�w��i���̍��̃C���f���g�ʂɂ���ĉ��L�̃C���f���g�ƂȂ�
	//�E0�̏ꍇ�F�O�s�Ɠ����C���f���g��
	//�E0�ł͂Ȃ��ꍇ�F���̃C���f���g�Ɠ����C���f���g�i�C���f���g�ێ��j
	//���L�̂����ꂩ�̏ꍇ�ɓK�p�����
	//�E���W�I�{�^���Łu�O�s�Ɠ����C���f���g�ɂ���v��I�������ꍇ
	//�E�f�B���N�e�B�u�ɂ��disable�s
	//�E�R�����g�u���b�N�̒��̍s�i�R�����g��ԂŎn�܂�s�j
	if( indentType == kIndentType_SameAsPreviousParagraph || 
	   isCommentLine == true ||
	   mTextInfo.GetDisabledDirectiveLevel(inLineIndex) != kIndex_Invalid  )
	{
		if( inIndentTriggerType == kIndentTriggerType_ReturnKey_CurrentLine || 
		   inIndentTriggerType == kIndentTriggerType_AutoIndentCharacter )
		{
			//���s���̌��ݍs�C���f���g�܂��͎����C���f���g�����̏ꍇ�A���̃C���f���g�Ɠ����C���f���g�i�C���f���g�ێ��j
			return currentIndentCount;
		}
		else if( currentIndentCount == 0 )
		{
			//------------------�w��i���̍��̃C���f���g�ʂ�0�̏ꍇ------------------
			//�O�s�Ɠ����C���f���g��
			if( currentParagraphStartLineIndex > 0 )
			{
				AIndex	prevParaLineIndex = mTextInfo.GetCurrentParagraphStartLineIndex(currentParagraphStartLineIndex-1);
				return SPI_GetCurrentIndentCount(prevParaLineIndex);
			}
			else
			{
				//�ŏ��̒i���̏ꍇ�́A���̃C���f���g�Ɠ����C���f���g�i�C���f���g�ێ��j
				return currentIndentCount;
			}
		}
		else
		{
			//------------------�O�s�Ɠ����C���f���g�ʂ�0�ł͂Ȃ��ꍇ------------------
			//���̃C���f���g�Ɠ����C���f���g�i�C���f���g�ێ��j
			return currentIndentCount;
		}
	}
	
	//==================���@��`�t�@�C���C���f���g==================
	if( indentType == kIndentType_SDFIndent )
	{
		//�C���f���g�͊e�s�̍s���Ɋ܂܂�邪�A�ҏW�㕶�@��͂����C���f���g�ƐV�C���f���g����v����s�܂Ŏ��{����킯�ɂ͂����Ȃ�
		//�i���Ƃ���{���͂ōŏI�s�܂ŕ��@��͕K�v�ɂȂ��Ă��܂��B�j�̂ŁA�C���f���g�v�Z�͓K���ȂƂ���܂Ŗ߂��āA��������v�Z����B
		
		//�C���f���g�v�Z�J�n�s������
		AIndex	startLineIndex = inLineIndex;
		AItemCount	indentCount = inPrevParagraphIndent;
		if( inPrevParagraphIndent == kIndex_Invalid )
 		{
			startLineIndex = currentParagraphStartLineIndex-1;
			if( startLineIndex < 0 )   startLineIndex = 0;
			//#695 AUniqID	menuIdentifierUniqID = mTextInfo.GetMenuIdentifierByLineIndex(inLineIndex);
			//#695 if( menuIdentifierUniqID != kUniqID_Invalid )
			AIndex	jumpIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);//#695
			if( jumpIndex != kIndex_Invalid )//#695
			{
				AUniqID menuIdentifierUniqID = mJump_IdentifierObjectArray.Get(jumpIndex);//#695
				startLineIndex = mTextInfo.GetLineIndexByGlobalIdentifier(menuIdentifierUniqID);
			}
			//B0340
			else
			{
				startLineIndex = GetPrevIndent0LineIndex(inLineIndex);
			}
			//#695 �߂肷����ƃX���[�I�y���[�V�����ɂȂ�̂ŁA�߂肷���h�~
			if( startLineIndex < inLineIndex - kLimit_SyntaxDefinitionIndent_MaxLineCountToGoBack )
			{
				startLineIndex = mTextInfo.GetPrevStableStateLineIndex(inLineIndex-kLimit_SyntaxDefinitionIndent_MaxLineCountToGoBack);
			}
			
			startLineIndex = mTextInfo.GetCurrentParagraphStartLineIndex(startLineIndex);
			
			//�����C���f���g���ݒ�
			//#695 �����̏����͊Ԉ���Ă��邽�߉��̏����ɏC���B�e�L�X�g���̌��ݍs�̃C���f���g�͌��ݍs�F�����ʂ����ɔ��f����Ă��邽�߁B
			//#695 indentCount = SPI_GetCurrentIndentCount(startLineIndex);
			indentCount = mTextInfo.GetIndentCount(startLineIndex) * SPI_GetIndentWidth();//#695
		}
		
		//fprintf(stderr,"s:%d,%d ",indentCount,startLineIndex);
		
		AIndex	lineIndex = startLineIndex;
		AItemCount	nextIndentCount = indentCount;
		//�s���̃��[�v
		for( ; lineIndex < nextParagraphStartLineIndex; lineIndex++ )
		{
			//fprintf(stderr,"lineIndex:%d indent:%d\n",lineIndex,indentCount);
			//#467
			if( mTextInfo.GetDisabledDirectiveLevel(lineIndex) == kIndex_Invalid )
			{
				//
				AIndex	stateIndex = 0;
				AIndex	pushedStateIndex = 0;
				mTextInfo.GetLineStateIndex(lineIndex,stateIndex,pushedStateIndex);
				
				//fprintf(stderr,"l:%d,%d,%d,%d ",indentCount,nextIndentCount,lineIndex,stateIndex);
				ATextIndex	spos = SPI_GetLineStart(lineIndex);
				AIndex	endpos = spos + SPI_GetLineLengthWithLineEnd(lineIndex);
				for( AIndex pos = spos; pos < endpos; )
				{
					//pos�ʒu����1�g�[�N�����
					AIndex	nextStateIndex;
					ABool	changeStateFromNextChar;
					ATextIndex	nextPos;
					ABool	seqFound;
					AText	token;
					ATextIndex	tokenspos;
					mTextInfo.RecognizeStateAndIndent(	mText,stateIndex,pushedStateIndex,
														pos,endpos,nextStateIndex,changeStateFromNextChar,nextPos,seqFound,token,tokenspos,
														SPI_GetIndentWidth(),
														GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Number(AModePrefDB::kIndentForLabel),
														indentCount,nextIndentCount);
					//��ԑJ��
					stateIndex = nextStateIndex;
					//pos�X�V
					pos = nextPos;
				}
				//���K�\���C���f���g
				//mTextInfo.CalcIndentByIndentRegExp(mText,spos,endpos,indentCount,nextIndentCount);
				//
			}
			
			if( lineIndex == nextParagraphStartLineIndex-1 )
			{
				outNextParagraphIndent = nextIndentCount;
				break;
			}
			//
			if( mTextInfo.GetLineExistLineEnd(lineIndex) == true )
			{
				indentCount = nextIndentCount;
			}
		}
		
		//fprintf(stderr,"e:%d ",indentCount);
		//==================statement�r������==================
		AIndex	stateIndex = 0;
		AIndex	pushedStateIndex = 0;
		mTextInfo.GetLineStateIndex(currentParagraphStartLineIndex,stateIndex,pushedStateIndex);
		if( GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetSyntaxDefinitionState(stateIndex).IsIndentOnTheWay() == true )
		{
			indentCount += GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Number(AModePrefDB::kIndentOnTheWay);
			//------------------�����̍ŏ��̍s���擾------------------
			AIndex	statementFirstLineIndex = currentParagraphStartLineIndex-1;
			for( ; statementFirstLineIndex >= 0; statementFirstLineIndex-- )
			{
				mTextInfo.GetLineStateIndex(statementFirstLineIndex,stateIndex,pushedStateIndex);
				if(	mTextInfo.GetCurrentParagraphStartLineIndex(statementFirstLineIndex) == statementFirstLineIndex &&
					GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().
					GetSyntaxDefinitionState(stateIndex).IsIndentOnTheWay() == false )
				{
					break;
				}
			}
			if( statementFirstLineIndex >= 0 )
			{
				//==================���̍ŏ��̍s��2�Ԗڂ̎������ʒu�ɃC���f���g==================
				if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kIndentOnTheWaySecondIndent) == true )
				{
					//------------------second indent�擾------------------
					ATextIndex	spos = SPI_GetLineStart(statementFirstLineIndex);
					ATextIndex	epos = spos + SPI_GetLineLengthWithoutLineEnd(statementFirstLineIndex);
					ATextIndex pos = spos;
					//�ŏ���tab/space��ǂݔ�΂�
					for( ; pos < epos; pos++ )
					{
						AUChar	ch = mText.Get(pos);
						if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
						else break;
					}
					//tab�ȊO��ǂݔ�΂�
					for( ; pos < epos; pos++ )
					{
						AUChar	ch = mText.Get(pos);
						if( ch == kUChar_Tab )   break;
					}
					for( ; pos < epos; pos++ )
					{
						AUChar	ch = mText.Get(pos);
						if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
						else
						{
							//tab/space�̈ʒu�̃C���f���g���擾����
							CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
							GetTextDrawDataWithoutStyle(mText,mTextInfo,statementFirstLineIndex,textDrawData);//#695
							indentCount = textDrawData.OriginalTextArray_TabLetters.Get(pos-spos);
							break;
						}
					}
				}
				//==================���ʊJ�n�ʒu�ɃC���f���g==================
				if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kIndentOnTheWay_ParenthesisStartPos) == true )
				{
					//���ʔ͈͎擾
					ATextPoint	spt = {0}, ept = {0};
					spt.x = 0;
					spt.y = inLineIndex;
					ept = spt;
					if( SPI_GetBraceSelection(spt,ept,true) == true )
					{
						//���ʊJ�n�ʒu���A�����̊J�n�s�ȍ~�ł���΁A���ʊJ�n�ʒu���C���f���g�ʂɂ���
						if( spt.y >= statementFirstLineIndex )
						{
							CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
							GetTextDrawDataWithoutStyle(mText,mTextInfo,spt.y,textDrawData);//#695
							indentCount = textDrawData.OriginalTextArray_TabLetters.Get(spt.x);
							//���ʂ̌�ɃX�y�[�X������΃X�y�[�X�̌�ɂ���
							for( ATextIndex	textIndex = SPI_GetTextIndexFromTextPoint(spt); textIndex < SPI_GetTextLength(); textIndex++ )
							{
								if( SPI_GetTextChar(textIndex) == kUChar_Space )
								{
									indentCount++;
								}
								else
								{
									break;
								}
							}
						}
					}
				}
				//==================���̍ŏ��̍s�̒P��̃R�����̈ʒu�����킹��==================
				if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kIndentOnTheWay_SingleColon) == true )
				{
					AIndex	statementFirstLineColonPos = kIndex_Invalid;
					{
						//�����J�n�s�̃R�����ʒu���擾
						ATextIndex	spos = SPI_GetLineStart(statementFirstLineIndex);
						ATextIndex	epos = spos + SPI_GetLineLengthWithoutLineEnd(statementFirstLineIndex);
						ATextIndex pos = spos;
						for( ; pos < epos; pos++ )
						{
							AUChar	ch = mText.Get(pos);
							AUChar	ch2 = 0;
							if( pos+1 < mText.GetItemCount() )
							{
								ch2 = mText.Get(pos+1);
							}
							if( ch == ':' )
							{
								//���̕������R�����ł͂Ȃ��i�P��R�����j���ǂ����𔻒�
								if( ch2 != ':' )
								{
									//':'�̈ʒu���擾����
									CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
									GetTextDrawDataWithoutStyle(mText,mTextInfo,statementFirstLineIndex,textDrawData);//#695
									statementFirstLineColonPos = textDrawData.OriginalTextArray_TabLetters.Get(pos-spos) +1;
									break;
								}
								//�R�����ȊO�̕����܂œǂݐi��
								for( ; pos < epos; pos++ )
								{
									if( mText.Get(pos) != ':' )
									{
										//pos�̓R�����ȊO�̕����̈ʒu�ɂȂ�B���̎��̕�������O���̃��[�v�p�������B
										break;
									}
								}
							}
						}
					}
					if( statementFirstLineColonPos != kIndex_Invalid )
					{
						AIndex	currentLineColonPos = kIndex_Invalid;
						//�w��s�̃R�����ʒu���擾
						ATextIndex	spos = SPI_GetLineStart(inLineIndex);
						ATextIndex	epos = spos + SPI_GetLineLengthWithoutLineEnd(inLineIndex);
						ATextIndex pos = spos;
						for( ; pos < epos; pos++ )
						{
							AUChar	ch = mText.Get(pos);
							AUChar	ch2 = 0;
							if( pos+1 < mText.GetItemCount() )
							{
								ch2 = mText.Get(pos+1);
							}
							if( ch == ':' )
							{
								//���̕������R�����ł͂Ȃ��i�P��R�����j���ǂ����𔻒�
								if( ch2 != ':' )
								{
									//':'�̈ʒu���擾����
									CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
									GetTextDrawDataWithoutStyle(mText,mTextInfo,inLineIndex,textDrawData);//#695
									currentLineColonPos = textDrawData.OriginalTextArray_TabLetters.Get(pos-spos) +1;
									break;
								}
								//�R�����ȊO�̕����܂œǂݐi��
								for( ; pos < epos; pos++ )
								{
									if( mText.Get(pos) != ':' )
									{
										//pos�̓R�����ȊO�̕����̈ʒu�ɂȂ�B���̎��̕�������O���̃��[�v�p�������B
										break;
									}
								}
							}
						}
						//�R�����ʒu�����킹��悤�ȃC���f���g�ʂɂ���
						if( currentLineColonPos != kIndex_Invalid )
						{
							AItemCount	lenBeforeColon = currentLineColonPos - SPI_GetCurrentIndentCount(inLineIndex);
							indentCount = statementFirstLineColonPos - lenBeforeColon;
						}
					}
				}
			}
		}
		return indentCount;
	}
	//���K�\���C���f���g
	else
	{
		AItemCount indentCount = 0;
		AItemCount	nextIndentCount = 0;
		/*#650
		if( inUseCurrentParagraphIndentForRegExp == true )//B0363
		{
			nextIndentCount = SPI_GetCurrentIndentCount(currentParagraphStartLineIndex);
			indentCount = nextIndentCount;
		}
		else 
		*/
		//#650 ���K�\���C���f���g�́u���s���̌��ݍs�C���f���g�ŁA���ݍs�����K�\����v���Ȃ������ꍇ�v�ȊO�͑S�āA�K�p����B
		//#650�Ή��ȑO�́A���ݍs�����K�\����v���Ȃ��ꍇ�͐��K�\���C���f���g���Ȃ��悤�ɂ��Ă������A
		//���ꂾ�ƁA�����s��I�����ăC���f���g���s����ꍇ�ɁA�C���f���g�K�p����Ȃ��B�O�s�����ݍs�����K�\����v���Ȃ��s���܂߂�
		//�C���f���g����ɂ́A�S�Ă̍s�Ɂu�i�O�s�Ɠ����C���f���g�j�{���i���K�\����v�������̃C���f���g�j�v��K�p����K�v������B
		//�i���K�\����v���Ȃ��s�̓��̕����͂O�ɂȂ�̂ŁA�O�s�Ɠ����C���f���g�ƂȂ�B�j
		if( currentParagraphStartLineIndex > 0 )
		{
			/*R0208 GetCurrentParagraphStartLineIndex()�d���̂�
			AIndex	prevParagraphIndex = mTextInfo.GetParagraphIndexByLineIndex(currentParagraphStartLineIndex-1);
			AIndex	prevParagraphStartLineIndex = mTextInfo.GetLineIndexByParagraphIndex(prevParagraphIndex);*/
			AIndex	prevParagraphStartLineIndex = mTextInfo.GetCurrentParagraphStartLineIndex(currentParagraphStartLineIndex-1);
			nextIndentCount = SPI_GetCurrentIndentCount(prevParagraphStartLineIndex);
			indentCount = nextIndentCount;//B0324
			//fprintf(stderr,"prevpara:%d,%d ",prevParagraphIndex,indentCount);
			{
				ATextIndex	spos = SPI_GetLineStart(prevParagraphStartLineIndex);
				AIndex	endpos = SPI_GetLineStart(currentParagraphStartLineIndex);
				mTextInfo.CalcIndentByIndentRegExp(mText,spos,endpos,indentCount,nextIndentCount);
				indentCount = nextIndentCount;
			}
		}
		{
			//fprintf(stderr,"start:%d,%d ",currentParagraphStartLineIndex,indentCount);
			ATextIndex	spos = SPI_GetLineStart(currentParagraphStartLineIndex);
			AIndex	endpos = SPI_GetLineStart(nextParagraphStartLineIndex);
			AItemCount	svIndentCount = indentCount;//#650 ���ݍs�̐��K�\���ɂ��C���f���g���������邩�ǂ����̔���̂��߁A�C���f���g�L��
			/*#650 ABool	regExpFound = */
			mTextInfo.CalcIndentByIndentRegExp(mText,spos,endpos,indentCount,outNextParagraphIndent);//#441
			//fprintf(stderr,"end:%d ",indentCount);
			//#650 AItemCount	currentLineLength = SPI_GetLineLengthWithoutLineEnd(currentParagraphStartLineIndex);//#441
			AItemCount	currentIndentCount = SPI_GetCurrentIndentCount(currentParagraphStartLineIndex);//#441
			//#650 if( currentLineLength == 0 || regExpFound == true )//#441
			if( (inIndentTriggerType == kIndentTriggerType_ReturnKey_CurrentLine || 
				 inIndentTriggerType == kIndentTriggerType_AutoIndentCharacter ) 
			   && (svIndentCount==indentCount) )//#650 ���ݍs�̐��K�\���ɂ��C���f���g���������邩�ǂ����̔���
			{
				//���s���̌��ݍs�C���f���g�܂��͎����C���f���g�����̏ꍇ�A���ݍs�̐��K�\���ɂ��C���f���g�����������ꍇ�́A�C���f���g�ێ� #650
				//�Ⴆ��</pre>�̃C���f���g��0�ŁA���̍s�Ɏ蓮�ŃC���f���g�쐬���A<p>��ł������ꍇ�A����C���f���g�ێ�����K�v������B
				return currentIndentCount;
			}
			else
			//if( inIndentTriggerType != kIndentTriggerType_ReturnKey_CurrentLine || regExpFound == true )//#650
			{
				//�C���f���g�K�p
				return indentCount;
			}
		}
	}
}

//B0340
//�w��s�ȑO�̍s�i�i���j�ŁA�C���f���g��0�̍s�̃C���f�b�N�X��Ԃ�
AIndex	ADocument_Text::GetPrevIndent0LineIndex( const AIndex inLineIndex ) const
{
	for( AIndex lineIndex = inLineIndex-1; lineIndex > 0; lineIndex-- )
	{
		if( mTextInfo.GetLineExistLineEnd(lineIndex-1) == true )
		{
			if( SPI_GetCurrentIndentCount(lineIndex) <= 0 )   return lineIndex;
		}
	}
	return 0;
}

//
AItemCount	ADocument_Text::SPI_GetCurrentIndentCount( const AIndex inLineIndex ) const
{
	//#318
	AText	zenkakuSpaceText;
	zenkakuSpaceText.SetZenkakuSpace();
	ABool	zenkakuSpaceEnable = (GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == true &&
			GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kZenkakuSpaceIndent) == true);
	//
	ATextIndex	spos = SPI_GetLineStart(inLineIndex);
	ATextIndex	epos = spos + SPI_GetLineLengthWithoutLineEnd(inLineIndex);
	AItemCount	indentCount = 0;
	for( ATextIndex pos = spos; pos < epos; pos++ )
	{
		//#318
		if( zenkakuSpaceEnable == true )
		{
			AText	text;
			SPI_GetText1CharText(pos,text);
			if( text.Compare(zenkakuSpaceText) == true )
			{
				pos += text.GetItemCount()-1;
				indentCount += 2;
				continue;
			}
		}
		//
		AUChar	ch = SPI_GetTextChar(pos);
		if( ch == kUChar_Space )
		{
			indentCount++;
		}
		else if( ch == kUChar_Tab )
		{
			indentCount += SPI_GetTabWidth();
		}
		else break;
	}
	return indentCount;
}

//
void	ADocument_Text::GetIndentText( const AItemCount inIndentCount, AText& outText, const ABool inInputSpaces ) const//#249
{
	outText.DeleteAll();
	//#249 if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == false )
	if( inInputSpaces == false )//#249
	{
		AItemCount	tabCount = (inIndentCount/SPI_GetTabWidth());
		AItemCount	spaceCount = inIndentCount - (tabCount*SPI_GetTabWidth());
		for( AIndex i = 0; i < tabCount; i++ )
		{
			outText.Add(kUChar_Tab);
		}
		for( AIndex i = 0; i < spaceCount; i++ )
		{
			outText.Add(kUChar_Space);
		}
	}
	else
	{
		//#318
		if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kZenkakuSpaceIndent) == true )
		{
			AText	zenkakuSpaceText;
			zenkakuSpaceText.SetZenkakuSpace();
			AItemCount	zenkakuSpaceCount = inIndentCount/2;
			AItemCount	spaceCount = inIndentCount-zenkakuSpaceCount*2;
			for( AIndex i = 0; i < zenkakuSpaceCount; i++ )
			{
				outText.AddText(zenkakuSpaceText);
			}
			for( AIndex i = 0; i < spaceCount; i++ )
			{
				outText.Add(kUChar_Space);
			}
		}
		else
		{
			for( AIndex i = 0; i < inIndentCount; i++ )
			{
				outText.Add(kUChar_Space);
			}
		}
	}
}

//
AItemCount	ADocument_Text::SPI_Indent( const AIndex inLineIndex, const AItemCount inIndentCount, const ABool inInputSpaces,
									   const ABool inDontRedrawSubWindows )//#249
{
	AText	newIndentText;
	GetIndentText(inIndentCount,newIndentText,inInputSpaces);//#249
	AItemCount	insertedTextCount = 0;
	//�ŏ��̃^�u�E�X�y�[�X�̕�����spos�`epos�ɓ����
	ATextIndex	spos = SPI_GetLineStart(inLineIndex);
	ATextIndex	epos = spos;
	//#318
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == true &&
					GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kZenkakuSpaceIndent) == true )
	{
		mText.SkipTabSpaceIncludingZenkakuSpace(epos,spos+mTextInfo.GetLineLengthWithoutLineEnd(inLineIndex));
	}
	else
	{
		mText.SkipTabSpace(epos,spos+mTextInfo.GetLineLengthWithoutLineEnd(inLineIndex));
	}
	//�ŏ��̃^�u�E�X�y�[�X�̕����̕s��v�ӏ���T��
	ATextIndex pos = spos;
	for( ; pos < epos; pos++ )
	{
		if( pos-spos >= newIndentText.GetItemCount() )   break;
		AUChar	ch = SPI_GetTextChar(pos);
		if( ch != newIndentText.Get(pos-spos) )
		{
			break;
		}
	}
	//�s��v�ӏ��ȍ~���폜
	if( pos < epos )
	{
		SPI_DeleteText(pos,epos,true,false,inDontRedrawSubWindows);
		insertedTextCount -= (epos-pos);
	}
	//�s��v�ӏ��ȍ~��}��
	newIndentText.Delete(0,pos-spos);
	if( newIndentText.GetItemCount() > 0 )
	{
		SPI_InsertText(pos,newIndentText,true,false,inDontRedrawSubWindows);
		insertedTextCount += newIndentText.GetItemCount();
	}
	return insertedTextCount;
}

//B0381
/**
�C���f���g�̃^�C�v���擾����
*/
AIndentType	ADocument_Text::SPI_GetIndentType() const
{
	//���@��`�C���f���g
	//�����@���蓮�ݒ�̂Ƃ��́A���@��`�C���f���g�ΏۊO
	//�@�i�u���b�N�J�n�A�I���ɏ]���ăC���f���g�������������A��������ƁA�u���b�N�J�n�A�I���ȊO�̕����̃C���f���g���Œ肳��Ă��܂��̂ŁA
	//�@�@�������̒�`���ł��Ȃ��Ǝ��p�I�ł͂Ȃ��B�]���A�蓮���@�ݒ�ŃC���f���g�𕶖@��`�ɏ]���悤�ɐݒ肵�Ă������[�U�[�̍����������邽�߁A����ʂ�ɂ���j
	if( (GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kUseBuiltinSyntaxDefinitionFile) == true ||
		 GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kUseUserDefinedSDF) == true ) && 
				GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kUseSyntaxDefinitionIndent) == true )
	{
		return kIndentType_SDFIndent;
	}
	//���K�\���C���f���g
	else if( (GetApp().SPI_GetModePrefDB(mModeIndex).GetItemCount_TextArray(AModePrefDB::kIndent_RegExp)>0) &&
				(GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kUseRegExpIndent) == true) )
	{
		return kIndentType_RegExpIndent;
	}
	//�C���f���g����
	else if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kNoIndent) == true )
	{
		return kIndentType_NoIndent;
	}
	//�O�s�Ɠ����C���f���g
	else
	{
		return kIndentType_SameAsPreviousParagraph;
	}
}

//B0000
AItemCount	ADocument_Text::SPI_GetTextInfoIndentCount( const AIndex inLineIndex ) const
{
	return mTextInfo.GetIndentCount(inLineIndex);
}

#pragma mark ===========================

#pragma mark ---�܂肽����
//#450

/**
*/
void	ADocument_Text::SPI_InitLineImageInfo()
{
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_InitLineImageInfo();
	}
}

/**
*/
AFoldingLevel	ADocument_Text::SPI_GetFoldingLevel( const AIndex inLineIndex ) const
{
	if( SPI_IsParagraphStartLine(inLineIndex) == true )
	{
		return mTextInfo.GetFoldingLevel(inLineIndex);
	}
	else
	{
		return kFoldingLevel_None;
	}
}

//#493
/**
�s�̃t�H���g�T�C�Y�{���擾
*/
AFloatNumber	ADocument_Text::SPI_GetLineFontSizeMultiply( const AIndex inLineIndex ) const
{
	AFloatNumber	result = 1.0;
	if( SPI_GetFoldingLevel(inLineIndex) == kFoldingLevel_Header )
	{
		result *= mTextInfo.GetLineMultiply(inLineIndex);
		result /= 100;
	}
	return result;
}

/*#379
#pragma mark ===========================

#pragma mark <���L�N���X(AWindow_Text)�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�����ʒm�R�[���o�b�N
//�E�C���h�E������ꂽ���ɁA�E�C���h�E����R�[�������
//mWindowIDArray����Ή��E�C���h�E��ID���폜���A�E�C���h�E��0�ɂȂ�����A�h�L�������g���폜����
void	ADocument_Text::OWICB_DoWindowClosed( const AWindowID inWindowID )
{
	AIndex	index = mWindowIDArray.Find(inWindowID);
	if( index == kIndex_Invalid )
	{
		_ACThrow("closed window not found",this);
	}
	mWindowIDArray.Delete1(index);
	if( mWindowIDArray.GetItemCount() == 0 )
	{
		GetApp().NVI_DeleteDocument(GetObjectID());
	}
}
*/

#pragma mark ===========================

#pragma mark ---CallGraf

//#723
/**
CallGraf�E�C���h�E�Ɍ���function��ݒ�
*/
void	ADocument_Text::SPI_SetCallGrafCurrentFunction( const AWindowID inTextWindowID, const AIndex inLineIndex, 
		const ABool inByEdit )
{
	//diff target�h�L�������g�ł���Ή������Ȃ�
	if( SPI_IsDiffTargetDocument() == true )
	{
		return;
	}
	
	//���[�J���͈͊J�n���ʎq�̃f�[�^���擾
	AText	funcname, classname;
	ATextPoint	menuidentifierSpt = kTextPoint_00, menuidentifierEpt = kTextPoint_00;
	AIndex	categoryIndex = kIndex_Invalid;
	SPI_GetLocalStartIdentifierDataByLineIndex(inLineIndex,funcname,classname,categoryIndex,menuidentifierSpt,menuidentifierEpt);
	if( funcname.GetItemCount() > 0 )
	{
		//�t�@�C���p�X�擾
		AText	filepath;
		NVI_GetFilePath(filepath);
		//CallGraf�Ɍ���function�ݒ�
		GetApp().SPI_SetCallGrafCurrentFunction(GetObjectID(),
					mProjectFolder,mModeIndex,categoryIndex,filepath,classname,funcname,
					SPI_GetTextIndexFromTextPoint(menuidentifierSpt),
					SPI_GetTextIndexFromTextPoint(menuidentifierEpt),
					inByEdit);
	}
}

/**
�R�[���O���t�̍��ڂ̕ҏW�L��t���O��ݒ�
*/
void	ADocument_Text::SPI_SetCallGrafEditFlag( const AWindowID inTextWindowID, const AIndex inLineIndex )
{
	//diff target�h�L�������g�ł���Ή������Ȃ�
	if( SPI_IsDiffTargetDocument() == true )
	{
		return;
	}
	
	//���[�J���͈͊J�n���ʎq�̃f�[�^���擾
	AText	funcname, classname;
	ATextPoint	menuidentifierSpt = kTextPoint_00, menuidentifierEpt = kTextPoint_00;
	AIndex	categoryIndex = kIndex_Invalid;
	SPI_GetLocalStartIdentifierDataByLineIndex(inLineIndex,funcname,classname,categoryIndex,menuidentifierSpt,menuidentifierEpt);
	if( funcname.GetItemCount() > 0 )
	{
		//�t�@�C���p�X�擾
		AText	filepath;
		NVI_GetFilePath(filepath);
		//CallGraf��edit�t���O�X�V
		GetApp().SPI_SetCallGrafEditFlag(GetObjectID(),filepath,classname,funcname);
	}
}


#pragma mark ===========================

#pragma mark ---�}�\���[�h
//#478

//�������r���[
/**
�w��text point�̂܂��ɑ��݂���e�[�u���̏����擾
*/
ABool	ADocument_Text::SPI_GetSurroundingTable( const ATextPoint& inTextPoint, 
		AArray<AIndex>& outRowArray_LineIndex, AArray<ABool>& outRowArray_IsRuledLine,
		AArray<AIndex>& outRowArray_ColumnEndX ) const
{
	//
	outRowArray_LineIndex.DeleteAll();
	outRowArray_IsRuledLine.DeleteAll();
	outRowArray_ColumnEndX.DeleteAll();
	
	//�w��textpoint�̒i���̊J�n�s���擾
	AIndex	targetParagraphStartLineIndex = SPI_GetCurrentParagraphStartLineIndex(inTextPoint.y);
	//�J�n�s�̍ŏ���|�̈ʒu���擾
	AIndex	startXpos = kIndex_Invalid;
	AIndex	lineStart = SPI_GetLineStart(targetParagraphStartLineIndex);
	for( AIndex pos = lineStart; 
				pos < mText.GetItemCount(); 
				pos++ )
	{
		AUChar	ch = mText.Get(pos);
		if( ch == '|' )
		{
			startXpos = pos - lineStart;
			break;
		}
		if( ch == kUChar_LineEnd )
		{
			break;
		}
	}
	if( startXpos == kIndex_Invalid )
	{
		return false;
	}
	
	AItemCount	lineCount = SPI_GetLineCount();
	//��[������
	for( AIndex lineIndex = targetParagraphStartLineIndex; 
				lineIndex > 0;
				lineIndex = SPI_GetCurrentParagraphStartLineIndex(lineIndex-1) )//�O�̒i���̊J�n�s���擾
	{
		//
		if( startXpos >= SPI_GetLineLengthWithoutLineEnd(lineIndex) )
		{
			break;
		}
		else
		{
			AIndex	pos = SPI_GetLineStart(lineIndex) + startXpos;
			AUChar	ch = mText.Get(pos);
			if( ch == '|' || ch == '+' )
			{
				outRowArray_LineIndex.Insert1(0,lineIndex);
			}
			else
			{
				break;
			}
		}
	}
	
	//���[������
	for( AIndex lineIndex = SPI_GetNextParagraphStartLineIndex(targetParagraphStartLineIndex);
				lineIndex < lineCount;
				lineIndex = SPI_GetNextParagraphStartLineIndex(lineIndex) )
	{
		//
		if( startXpos >= SPI_GetLineLengthWithoutLineEnd(lineIndex) )
		{
			break;
		}
		else
		{
			AIndex	pos = SPI_GetLineStart(lineIndex) + startXpos;
			AUChar	ch = mText.Get(pos);
			if( ch == '|' || ch == '+' )
			{
				outRowArray_LineIndex.Add(lineIndex);
			}
			else
			{
				break;
			}
		}
	}
	
	//�e�e�[�u���\���s�́AinTextPoint���E���̃J������؂�ʒu���擾
	for( AIndex index = 0; index < outRowArray_LineIndex.GetItemCount(); index++ )
	{
		//
		AIndex	lineIndex = outRowArray_LineIndex.Get(index);
		//
		AIndex	lineStart = SPI_GetLineStart(lineIndex);
		AItemCount	len = SPI_GetLineLengthWithoutLineEnd(lineIndex);
		//
		if( inTextPoint.x >= len )
		{
			outRowArray_IsRuledLine.Add(false);
			outRowArray_ColumnEndX.Add(kIndex_Invalid);
		}
		else
		{
			AUChar	ch = mText.Get(lineStart + inTextPoint.x);
			if( ch == '-' || ch == '+' )
			{
				outRowArray_IsRuledLine.Add(true);
				outRowArray_ColumnEndX.Add(inTextPoint.x+1);
			}
			else
			{
				outRowArray_IsRuledLine.Add(false);
				//
				AIndex p = inTextPoint.x+1;
				for( ; p < len; p++ )
				{
					AUChar	ch = mText.Get(lineStart + p);
					if( ch == '|' )
					{
						break;
					}
				}
				outRowArray_ColumnEndX.Add(p);
			}
		}
	}
	
	//
	/*
	+------------+------+
	|  aa  aaa   |      |
	+------------+------+
	*/
	
	//
	if( outRowArray_LineIndex.GetItemCount() >= 2 )
	{
		return true;
	}
	else
	{
		outRowArray_LineIndex.DeleteAll();
		outRowArray_IsRuledLine.DeleteAll();
		outRowArray_ColumnEndX.DeleteAll();
		return false;
	}
	/*
	
	
	
	
	//
	AIndex	startLineCellStartPos = kIndex_Invalid;
	for( AIndex pos = GetTextIndexFromTextPoint(inTextPoint); pos >= 0; pos = mText.GetPrevCharPos(pos) )
	{
		AUChar	ch = mText.Get(pos);
		if( ch == '|' )
		{
			startLineCellStartPos = pos;
			break;
		}
		if( ch == kUChar_LineEnd )
		{
			break;
		}
	}
	if( startLineCellStartPos == kIndex_Invalid )
	{
		return;
	}
	ATextPoint	startLineCellStartTextPoint = {0};
	GetTextPointFromTextIndex(startLineCellStartPos,startLineCellStartTextPoint);
	//
	
	//
	AIndex	endLineCellStartPos = kIndex_Invalid;
	for( AIndex pos = GetTextIndexFromTextPoint(inTextPoint); pos < mText.GetItemCount(); pos = mText.GetNextCharPos(pos) )
	{
		AUChar	ch = mText.Get(pos);
		if( ch == '|' )
		{
			endLineCellStartPos = pos;
			break;
		}
		if( ch == kUChar_LineEnd )
		{
			return;
		}
	}
	if( endLineCellStartPos == kIndex_Invalid )
	{
		return;
	}
	ATextPoint	endLineCellStartTextPoint = {0};
	GetTextPointFromTextIndex(endLineCellStartPos,endLineCellStartTextPoint);
	//
	*/
}

#pragma mark ===========================

#pragma mark ---DocInfo�E�C���h�E
//#142

/**
*/
void	ADocument_Text::UpdateDocInfoWindows()
{
	//
	AItemCount	totalCharCount = 0, totalWordCount = 0;
	mTextInfo.GetTotalWordCount(totalCharCount,totalWordCount);
	//
	GetApp().SPI_UpdateDocInfoWindows_TotalCount(GetObjectID(),totalCharCount,totalWordCount);
	//#1149
	//�I�𕶎����A���ݕ����̕\���̍X�V
	if( mViewIDArray.GetItemCount() > 0 )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_UpdateTextCountWindows();
	}
}

#pragma mark ===========================

#pragma mark ---�q���g�e�L�X�g
//

/**
�q���g�e�L�X�g�ǉ�
*/
AIndex	ADocument_Text::SPI_AddHintText( const ATextIndex inTextIndex, const AText& inHintText )
{
	return mTextInfo.AddHintText(inTextIndex,inHintText);
}

/**
�q���g�e�L�X�g�폜
*/
void	ADocument_Text::SPI_ClearHintText()
{
	mTextInfo.ClearHintText();
}

/**
�q���g�e�L�X�g�擾
*/
void	ADocument_Text::SPI_GetHintText( const ATextIndex inTextIndex, AText& outHintText ) const
{
	mTextInfo.GetHintText(inTextIndex,outHintText);
}

/**
�w��ʒu�̃q���g�e�L�X�g��index���擾
*/
AIndex	ADocument_Text::SPI_FindHintIndexFromTextIndex( const ATextIndex inHintTextPos ) const
{
	return mTextInfo.FindHintIndexFromTextIndex(inHintTextPos);
}

/**
�q���g�e�L�X�g�S�폜�i"?"���폜�j
*/
void	ADocument_Text::SPI_DeleteAllHintText()
{
	AItemCount	itemCount = mTextInfo.GetHintTextCount();
	for( AIndex i = itemCount-1; i >= 0; i-- )//��{�I�ɍ��ڍ폜�����̂Ō�납��
	{
		ATextIndex	textIndex = mTextInfo.GetHintTextPos(i);
		if( SPI_GetTextChar(textIndex) == kHintTextChar )
		{
			SPI_DeleteText(textIndex,textIndex+1);
		}
	}
}

#pragma mark ===========================

#pragma mark ---AppleScript�p�h�L�������g�f�[�^�擾

ABool	ADocument_Text::SPI_ASGetParagraph( const ATextIndex inStartTextIndex, const AIndex inParagraphOffsetIndex, 
		ATextIndex& outStart, ATextIndex& outEnd ) const
{
	ATextPoint	pt;
	SPI_GetTextPointFromTextIndex(inStartTextIndex,pt);
	AIndex	paragraphIndex = SPI_GetParagraphIndexByLineIndex(pt.y) + inParagraphOffsetIndex;
	
	if( SPI_GetLineIndexByParagraphIndex(paragraphIndex) >= SPI_GetLineCount() )   return false;
	
	outStart = SPI_GetParagraphStartTextIndex(paragraphIndex);
	outEnd = SPI_GetParagraphStartTextIndex(paragraphIndex+1);
	return true;
}

ABool	ADocument_Text::SPI_ASGetWord( const ATextIndex inStartTextIndex, const AIndex inWordOffsetIndex, 
		ATextIndex& outStart, ATextIndex& outEnd ) const
{
	AIndex	wordIndex = 0;
	AItemCount	textlength = SPI_GetTextLength();
	for( ATextIndex pos = inStartTextIndex; pos < textlength; )
	{
		if( mText.SkipTabSpaceLineEnd(pos,textlength) == false )   break;
		ATextIndex	start, end;
		SPI_FindWord(pos,start,end);
		if( wordIndex == inWordOffsetIndex )
		{
			outStart = start;
			outEnd = end;
			return true;
		}
		wordIndex++;
		pos = end;
	}
	return false;
}

ABool	ADocument_Text::SPI_ASGetChar( const ATextIndex inStartTextIndex, const AIndex inCharOffsetIndex, 
		ATextIndex& outStart, ATextIndex& outEnd ) const
{
	AIndex	charIndex = 0;
	AItemCount	textlength = SPI_GetTextLength();
	for( ATextIndex pos = inStartTextIndex; pos < textlength; )
	{
		if( charIndex == inCharOffsetIndex )
		{
			outStart = pos;
			outEnd = mText.GetNextCharPos(pos);
			return true;
		}
		pos = mText.GetNextCharPos(pos);
		charIndex++;
	}
	return false;
}

//B0331
ABool	ADocument_Text::SPI_ASGetInsertionPoint( const ATextIndex inStartTextIndex, const AIndex inOffsetIndex, ATextIndex& outPos ) const
{
	AIndex	insertionpointIndex = 0;
	AItemCount	textlength = SPI_GetTextLength();
	for( ATextIndex pos = inStartTextIndex; pos <= textlength; )
	{
		if( insertionpointIndex == inOffsetIndex )
		{
			outPos = pos;
			return true;
		}
		pos = mText.GetNextCharPos(pos);
		insertionpointIndex++;
	}
	return false;
}

//inStartTextIndex�`inEndTextIndex�̒i���̌��𐔂���
//��{�I�ɂ�inStartTextIndex�`inEndTextIndex�̉��s�R�[�h�̐��𐔂���
/*
inStartTextIndex���i���̍ŏ��̏ꍇ�́AinEndTextIndex�̈ʒu�ɂ���āA�Ԃ�l�͈ȉ��̂悤�ɂȂ�
0111111111111111
1222222222222222
inStartTextIndex���i���̓r���̏ꍇ�́AinEndTextIndex�̈ʒu�ɂ���āA�Ԃ�l�͈ȉ��̂悤�ɂȂ�
      0111111111
1222222222222222
inStartTextIndex == inEndTextIndex�̏ꍇ�́A�Ԃ�l��0�ƂȂ�̂ŁAcount of paragraph�̏ꍇ�̂݃R�[�����łȂ�Ƃ����邱�ƁB
index of paragraph�ł��g�p����̂ŁA��L�̂悤�Ȏd�l�ɂ�����𓾂Ȃ��B
*/
AItemCount	ADocument_Text::SPI_ASGetParagraphCount( const ATextIndex inStartTextIndex, const ATextIndex inEndTextIndex ) const
{
	AItemCount	count = 0;
	ATextIndex	currentParagraphStartPos = inStartTextIndex;//B0304(B0331) 071123����ς茳�ɖ߂� kIndex_Invalid;//B0331 �I��͈͂���̏ꍇ��1�ɂ���inStartTextIndex;
	ATextIndex pos = inStartTextIndex;
	for( ; pos < inEndTextIndex; pos++ )
	{
		if( mText.Get(pos) == kUChar_LineEnd )
		{
			count++;
			currentParagraphStartPos = pos+1;
		}
	}
	//���[�v���i���̍ŏ��ŏI�������ꍇ�ȊO�͂��̕������P�i���Ɋ܂߂�
	if( pos != currentParagraphStartPos )
	{
		count++;
	}
	return count;
}

AItemCount	ADocument_Text::SPI_ASGetWordCount( const ATextIndex inStartTextIndex, const ATextIndex inEndTextIndex ) const
{
	AItemCount	count = 0;
	for( ATextIndex pos = inStartTextIndex; pos < inEndTextIndex; )
	{
		if( mText.SkipTabSpaceLineEnd(pos,inEndTextIndex) == false )   break;
		ATextIndex	start, end;
		SPI_FindWord(pos,start,end);
		count++;
		pos = end;
	}
	return count;
}

AItemCount	ADocument_Text::SPI_ASGetCharCount( const ATextIndex inStartTextIndex, const ATextIndex inEndTextIndex ) const
{
	AItemCount	count = 0;
	for( ATextIndex pos = inStartTextIndex; pos < inEndTextIndex; )
	{
		pos = mText.GetNextCharPos(pos);
		count++;
	}
	return count;
}

void	ADocument_Text::SPI_GetCurrentStateIndexAndName( const ATextPoint& inTextPoint, 
		AIndex& outStateIndex, AIndex& outPushedStateIndex, 
		AText& outStateText, AText& outPushedStateText ) const
{
	mTextInfo.GetCurrentStateIndexAndName(mText,inTextPoint,outStateIndex,outPushedStateIndex,
				outStateText,outPushedStateText);
}

/**
�w��e�L�X�g�|�C���g�̏�Ԃ��擾
*/
AIndex	ADocument_Text::SPI_GetCurrentStateIndex( const ATextPoint& inTextPoint ) const
{
	AIndex	stateIndex = kIndex_Invalid;
	AIndex	pushedStateIndex = kIndex_Invalid;
	AText	stateName, pushedStateName;
	mTextInfo.GetCurrentStateIndexAndName(mText,inTextPoint,stateIndex,pushedStateIndex,stateName,pushedStateName);
	return stateIndex;
}

#pragma mark ===========================

#pragma mark --- Previewer
//#734

/**
�v���r���[���X�V
*/
void	ADocument_Text::SPI_UpdatePreview( const AIndex inLineIndex )
{
	AText	previewPluginText;
	if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetPreviewPlugin(previewPluginText) == true )
	{
		//==================�v���r���[�v���O�C�����s==================
		
		//���Ή�
	}
	else
	{
		/*#1257����
		AText	modename;
		SPI_GetModeRawName(modename);
		if( modename.Compare("Markdown") == true )
		{
			AFileAcc	docfile;
			NVI_GetFile(docfile);
			AText	docpath;
			docfile.GetPath(docpath);
			//
			AFileAcc	tmpFolder;
			GetApp().SPI_GetTempFolder(tmpFolder);
			AFileAcc	tmpFile;
			tmpFile.SpecifyChildFile(tmpFolder,"markdown_preview.html");
			tmpFile.CreateFile();
			AText	tmpFilePath;
			tmpFile.GetPath(tmpFilePath);
			//
			{
				AText	command;
				command.AddCstring("/usr/local/bin/pandoc \"");
				command.AddText(docpath);
				command.AddCstring("\" -s -o \"");
				command.AddText(tmpFilePath);
				command.AddCstring("\"");
				AStCreateCstringFromAText	cstr(command);
				system(cstr.GetPtr());//async�ɂ��Ȃ��ƒx��
			}
			
			//���[�h
			GetApp().SPI_RequestLoadToPreviewerWindow(SPI_GetFirstWindowID(),tmpFilePath);
			return;
		}
		*/
		/*
		AText	modename;
		SPI_GetModeRawName(modename);
		if( modename.Compare("TEX") == true )
		{
			
			AFileAcc	docfile;
			NVI_GetFile(docfile);
			AText	docpath;
			docfile.GetPath(docpath);
			//
			AText	filename;
			docfile.GetFilename(filename);
			AText	origsuffix;
			filename.GetSuffix(origsuffix);
			AText	filenamewithoutsuffix;
			filenamewithoutsuffix.SetText(filename);
			filenamewithoutsuffix.Delete(filename.GetItemCount()-origsuffix.GetItemCount(),origsuffix.GetItemCount());
			
			//
			AFileAcc	folder;
			folder.SpecifyParent(docfile);
			AText	folderpath;
			folder.GetPath(folderpath);
			//
			AStCreateCstringFromAText	dirPath(folderpath);
			chdir(dirPath.GetPtr());
			
			//
			{
				AText	command;
				GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kTexTypesetPath,command);
				command.AddCstring(" \"");
				command.AddText(docpath);
				command.AddCstring("\"");
				AStCreateCstringFromAText	cstr(command);
				system(cstr.GetPtr());
			}
			
			//
			AText	dvifilename;
			dvifilename.SetText(filenamewithoutsuffix);
			dvifilename.AddCstring(".dvi");
			AFileAcc	dvifile;
			dvifile.SpecifySister(docfile,dvifilename);
			//
			{
				AText	command;
				GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kTexDviToPdfPath,command);
				command.AddCstring(" \"");
				command.AddText(dvifilename);
				command.AddCstring("\"");
				AStCreateCstringFromAText	cstr(command);
				system(cstr.GetPtr());
			}
			//
			AText	pdffilename;
			pdffilename.SetText(filenamewithoutsuffix);
			pdffilename.AddCstring(".pdf");
			AFileAcc	pdffile;
			pdffile.SpecifyChild(folder,pdffilename);
			
			AText	url;
			pdffile.GetPath(url);
			//���[�h
			GetApp().SPI_RequestLoadToPreviewerWindow(SPI_GetFirstWindowID(),url);
			return;
		}
		*/
		//==================�W���v���r���[����==================
		
		//���ݍs�ɕύX����������A���݃A���J�[����������
		if( mPreviewCurrentLineIndex != inLineIndex )
		{
			//���݃A���J�[������
			mPreviewCurrentAnchor.DeleteAll();
			//���߂̃A���J�[�̃^�C�g�����擾
			AUniqID	anchorIdentifier = mTextInfo.GetAnchorIdentifierByLineIndex(inLineIndex);
			if( anchorIdentifier != kUniqID_Invalid )
			{
				mTextInfo.GetGlobalIdentifierKeywordText(anchorIdentifier,mPreviewCurrentAnchor);
			}
			//���ݍs�X�V
			mPreviewCurrentLineIndex = inLineIndex;
		}
		
		//URL�擾
		AText	url;
		if( SPI_IsRemoteFileMode() == false )
		{
			//���[�J���t�@�C����URL
			AFileAcc	file;
			NVI_GetFile(file);
			file.GetPath(url);
		}
		else
		{
			//�����[�g�t�@�C����URL�ihttp��URL�ɕϊ��j
			GetApp().GetAppPref().GetHTTPURLFromFTPURL(mRemoteFileURL,url);
		}
		//�A���J�[�ݒ�
		if( mPreviewCurrentAnchor.GetItemCount() > 0 )
		{
			url.AddCstring("#");
			url.AddText(mPreviewCurrentAnchor);
		}
		//���[�h
		GetApp().SPI_RequestLoadToPreviewerWindow(SPI_GetFirstWindowID(),url);
	}
}

/**
�v���r���[�������[�h
*/
void	ADocument_Text::SPI_ReloadPrevew()
{
	//�����[�h
	GetApp().SPI_RequestReloadToPreviewerWindow(SPI_GetFirstWindowID());
}

#pragma mark ===========================

#pragma mark --- LuaConsole
//#567

/**
Lua print�֐��o��
*/
void	ADocument_Text::SPI_Lua_Output( const AText& inText )
{
	//���s�R�[�h�擾
	AText	lineEnd;
	lineEnd.Add(kUChar_LineEnd);
	//ViewID�擾
	AViewID	viewID = mViewIDArray.Get(0);
	
	//�e�L�X�g�Ō��pt�擾
	ATextPoint	pt = {0,0};
	SPI_GetTextPointFromTextIndex(mText.GetItemCount(),pt);
	//�Ō��pt���s�̓r���Ȃ�A���s�ǉ�
	if( pt.x > 0 )
	{
		SPI_InsertText(mText.GetItemCount(),lineEnd);
	}
	//inText���h�L�������g�̍Ō�ɒǉ�
	AText	t(inText);
	SPI_InsertText(mText.GetItemCount(),t);
	
	//�L�����b�g�ʒu���Ō���ɐݒ�
	SPI_GetTextPointFromTextIndex(mText.GetItemCount(),pt);
	AView_Text::GetTextViewByViewID(viewID).SPI_SetSelect(pt,pt);
	AView_Text::GetTextViewByViewID(viewID).SPI_AdjustScroll_Center();
}

/**
Lua�v�����v�g�\��
*/
void	ADocument_Text::SPI_Lua_Prompt()
{
	//���݂̃v�����v�g�ʒu���h�L�������g�Ō�Ȃ炻�̂܂�
	if( mLuaPromptTextIndex == mText.GetItemCount() )   return;
	
	//���s�R�[�h�擾
	AText	lineEnd;
	lineEnd.Add(kUChar_LineEnd);
	//ViewID�擾
	AViewID	viewID = mViewIDArray.Get(0);
	
	//�e�L�X�g�Ō��pt�擾
	ATextPoint	pt = {0,0};
	SPI_GetTextPointFromTextIndex(mText.GetItemCount(),pt);
	//�Ō��pt���s�̓r���Ȃ�A���s�ǉ�
	if( pt.x > 0 )
	{
		SPI_InsertText(mText.GetItemCount(),lineEnd);
	}
	//�v�����v�g���h�L�������g�̍Ō�ɒǉ�
	AText	prompt(">");
	SPI_InsertText(mText.GetItemCount(),prompt);
	
	//�L�����b�g�ʒu���Ō���ɐݒ�
	SPI_GetTextPointFromTextIndex(mText.GetItemCount(),pt);
	AView_Text::GetTextViewByViewID(viewID).SPI_SetSelect(pt,pt);
	AView_Text::GetTextViewByViewID(viewID).SPI_AdjustScroll_Center();
	
	//���݂̃v�����v�g�ʒu�X�V
	mLuaPromptTextIndex = mText.GetItemCount();
}

#pragma mark ===========================

#pragma mark ---�n�b�V�������p��r����

//inTargetText��inTargetIndex, inTargetItemCount�̕����Ɣ�r���Ĉ�v���邩�ǂ�����Ԃ�
ABool	ALocalIdentifierData::Compare( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	return GetKeywordText().Compare(inTargetText,inTargetIndex,inTargetItemCount);
}

//�n�b�V���֐�
template<> unsigned long	AHashObjectArray<ALocalIdentifierData,AText>::GetHashValue( const AText& inData, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	AItemCount	targetItemCount = inTargetItemCount;
	if( targetItemCount == kIndex_Invalid )
	{
		targetItemCount = inData.GetItemCount();
	}
	/*win 080726 if( targetItemCount == 0 )
	{
		return 12345;
	}
	else if( targetItemCount == 1 )
	{
		return inData.Get(inTargetIndex)*111;
	}
	else if( targetItemCount == 2 )
	{
		return inData.Get(inTargetIndex)*111 + inData.Get(inTargetIndex+1)*222;
	}
	else 
	{
		return inData.Get(inTargetIndex)*111 + inData.Get(inTargetIndex+1)*222 + inData.Get(inTargetIndex+2)*333 + targetItemCount*999;
	}*/
	if( targetItemCount == 0 )
	{
		return 1234567;
	}
	else if( targetItemCount == 1 )
	{
		return inData.Get(inTargetIndex)*67890123;
	}
	else if( targetItemCount == 2 )
	{
		return inData.Get(inTargetIndex)*78901234 + inData.Get(inTargetIndex+1)*891012345;
	}
	else 
	{
		return inData.Get(inTargetIndex)*8901234 + inData.Get(inTargetIndex+1)*9012345 
			+ inData.Get(inTargetIndex+targetItemCount-2)*1234567 + inData.Get(inTargetIndex+targetItemCount-1) + targetItemCount*4567;
	}
}

#pragma mark ===========================

#pragma mark ---

/**
�������`�F�b�N
*/
void	ADocument_Text::SPI_CheckLineInfoDataForDebug()
{
	mTextInfo.CheckLineInfoDataForDebug();
}

void	ADocument_Text::OrderTextPoint( const ATextPoint& inSelectTextPoint1, const ATextPoint& inSelectTextPoint2, 
		ATextPoint& outStartTextPoint, ATextPoint& outEndTextPoint ) 
{
	if( inSelectTextPoint1.y < inSelectTextPoint2.y ) 
	{
		outStartTextPoint = inSelectTextPoint1;
		outEndTextPoint = inSelectTextPoint2;
	}
	else if( inSelectTextPoint1.y == inSelectTextPoint2.y )
	{
		if( inSelectTextPoint1.x < inSelectTextPoint2.x )
		{
			outStartTextPoint = inSelectTextPoint1;
			outEndTextPoint = inSelectTextPoint2;
		}
		else
		{
			outStartTextPoint = inSelectTextPoint2;
			outEndTextPoint = inSelectTextPoint1;
		}
	}
	else
	{
		outStartTextPoint = inSelectTextPoint2;
		outEndTextPoint = inSelectTextPoint1;
	}
}

#pragma mark ===========================

#pragma mark ---JavaScript�p�֐�
//#904

/**
text index����Unicode�����ʒu(JavaScript�ł�text index)�ɕϊ�
*/
AIndex	ADocument_Text::SPI_GetUnicodeTextIndexFromTextIndex( const AIndex inTextIndex ) const
{
	AIndex	unicodeTextIndex = 0;
	//�|�C���^�擾
	AStMutexLocker	locker(mTextMutex);
	AStTextPtr	textptr(mText,0,mText.GetItemCount());
	AUChar*	p = textptr.GetPtr();
	//�w��text index�܂ŁAUnicode��������unicodeTextIndex���C���N�������g
	for( AIndex pos = 0; pos < inTextIndex; pos++ )
	{
		//UTF8�J�n�����Ȃ�unicodeTextIndex���C���N�������g
		//Mac OS X 10.6.8�Ńe�X�g�������AMac��JS�G���W����decomposition�����͂��ꂼ����ꕶ���Ƃ��Đ�����̂ŁAdecomp����͕s�v
		if( (p[pos]&0xC0) != 0x80 )
		{
			unicodeTextIndex++;
		}
	}
	return unicodeTextIndex;
}

/**
Unicode�����ʒu(JavaScript�ł�text index)����text index�ɕϊ�
*/
AIndex	ADocument_Text::SPI_GetTextIndexFromUnicodeTextIndex( const AIndex inUnicodeTextIndex ) const
{
	AIndex	unicodeTextIndex = 0;
	//�|�C���^�擾
	AStMutexLocker	locker(mTextMutex);
	AStTextPtr	textptr(mText,0,mText.GetItemCount());
	AUChar*	p = textptr.GetPtr();
	//�w��Unicode�����ʒu�܂ŁAUnicode��������unicodeTextIndex���C���N�������g
	AItemCount	len = textptr.GetByteCount();
	for( AIndex pos = 0; pos < len; pos++ )
	{
		//UTF8�J�n�����Ȃ�unicodeTextIndex���C���N�������g���A�����ƈ�v������A���݂�pos��Ԃ�
		if( (p[pos]&0xC0) != 0x80 )
		{
			//������Unicode�����ʒu�Ƃ���v������A���݂�pos��Ԃ�
			if( unicodeTextIndex == inUnicodeTextIndex )
			{
				return pos;
			}
			//unicodeTextIndex���C���N�������g
			unicodeTextIndex++;
		}
	}
	return len;
}

#pragma mark ===========================

#pragma mark ---�v���O�C��

/**
�C�x���g���X�i�[���s #994
*/
void	ADocument_Text::SPI_ExecuteEventListener( AConstCstringPtr inPtr, const AText& inParameter ) const
{
	//�t�H���_�͋�Ŏ��s�i���X�i�[���ł̓��W���[�����[�h�͂ł��Ȃ��Ƃ����d�l�Ƃ���B�j
	AFileAcc	folder;
	//���̃h�L�������g��UniqID�������Ƃ��ăC�x���g���X�i�[���s
	AText	script;
	script.SetCstring("mi.executeEventListeners(mi.getDocumentByID(\"");
	script.AddNumber(GetApp().NVI_GetDocumentUniqID(GetObjectID()).val);
	script.AddCstring("\"),\"");
	script.AddCstring(inPtr);
	script.AddCstring("\",\"");
	script.AddText(inParameter);
	script.AddCstring("\");");
	GetApp().SPI_ExecuteJavaScript(script,folder);
}

#pragma mark ===========================

#pragma mark ---

/**
�e�L�X�g�o�C�g���擾
*/
AByteCount	ADocument_Text::SPI_GetTextDocumentMemoryByteCount() const
{
	if( mLoaded == true )
	{
		//�t�@�C�����[�h��̓e�L�X�g�̃T�C�Y��Ԃ�
		return mText.GetItemCount();
	}
	else
	{
		//�t�@�C�������[�h����0��Ԃ�
		
		/*
		if( NVI_IsFileSpecified() == true )
		{
			AFileAcc	file;
			NVI_GetFile(file);
			return file.GetFileSize();
		}
		
		else
		 */
		{
			return 0;
		}
	}
}

/**
�e�L�X�g�G���R�[�f�B���O�����e�X�g
*/
void	ADocument_Text::TestGuessTextEncoding()
{
	//
	ATextEncoding	tec;
	AText	text;
	
	//UTF8
	{
		text.SetText(mText);
		//
		AFloatNumber	utf8err = 0, eucerr = 0, sjiserr = 0;
		text.CalcUTF8ErrorRate(1024*1024,utf8err);
		text.CalcEUCErrorRate(1024*1024,eucerr);
		text.CalcSJISErrorRate(1024*1024,sjiserr);
		fprintf(stderr,"UTF8:UTF8Err:%.3f EUC:%.3f SJISErr:%.3f Result:",utf8err,eucerr,sjiserr);
		if( text.GuessTextEncodingByAnalysis(tec) == true )
		{
			AText	tecname;
			ATextEncodingWrapper::GetTextEncodingName(tec,tecname);
			tecname.OutputToStderr();
		}
		else
		{
			fprintf(stderr,"???");
		}
		fprintf(stderr,"\n");
	}
	
	//EUC
	{
		text.SetText(mText);
		text.ConvertFromUTF8(ATextEncodingWrapper::GetEUCTextEncoding());
		//
		AFloatNumber	utf8err = 0, eucerr = 0, sjiserr = 0;
		text.CalcUTF8ErrorRate(1024*1024,utf8err);
		text.CalcEUCErrorRate(1024*1024,eucerr);
		text.CalcSJISErrorRate(1024*1024,sjiserr);
		fprintf(stderr,"EUC:UTF8Err:%.3f EUC:%.3f SJISErr:%.3f Result:",utf8err,eucerr,sjiserr);
		if( text.GuessTextEncodingByAnalysis(tec) == true )
		{
			AText	tecname;
			ATextEncodingWrapper::GetTextEncodingName(tec,tecname);
			tecname.OutputToStderr();
		}
		else
		{
			fprintf(stderr,"???");
		}
		fprintf(stderr,"\n");
	}
	
	//SJIS
	{
		text.SetText(mText);
		text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
		//
		AFloatNumber	utf8err = 0, eucerr = 0, sjiserr = 0;
		text.CalcUTF8ErrorRate(1024*1024,utf8err);
		text.CalcEUCErrorRate(1024*1024,eucerr);
		text.CalcSJISErrorRate(1024*1024,sjiserr);
		fprintf(stderr,"SJIS:UTF8Err:%.3f EUC:%.3f SJISErr:%.3f Result:",utf8err,eucerr,sjiserr);
		if( text.GuessTextEncodingByAnalysis(tec) == true )
		{
			AText	tecname;
			ATextEncodingWrapper::GetTextEncodingName(tec,tecname);
			tecname.OutputToStderr();
		}
		else
		{
			fprintf(stderr,"???");
		}
		fprintf(stderr,"\n");
	}
}


