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

AWindow_TextSheet

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class ADocument_Text;
class AWindow_Text;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_TextSheet_CorrectEncoding
//�e�L�X�g�G���R�[�f�B���O�C��
class AWindow_TextSheet_CorrectEncoding : public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_TextSheet_CorrectEncoding(/*#199  AWindow_Text& inWindow */ const AWindowID inTextWindowID );
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	AWindow_Text&			GetTextWindow();//#199 
	AWindowID							mTextWindowID;
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();//win
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  public:
	void					SPNVI_CreateAndShow( const ADocumentID inDocumentID,
												const ABool inWithAlert, const ATextEncodingFallbackType inFallbackType );
  private:
	void					UpdateFallbackTypeMessage( const ATextEncodingFallbackType inFallbackType );//#473
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_Show();
	//ABool								mTextEncodingNotDecided;
	AText								mSavedTextEncodingName;
	ADocumentID							mTextDocumentID;
	ATextEncodingFallbackType			mFallbackType;//#473
	ABool								mWithAlert;
	
	//�R���g���[��ID
	static const AControlID	kOKButton 					= 5001;
	static const AControlID	kAlertTitle1				= 5002;
	static const AControlID	kAlertTitle2				= 5003;
	static const AControlID	kTextEncodingMenu 			= 5004;
	static const AControlID	kTextEncodingError 			= 5005;
	static const AControlID	kTextEncodingErrorIcon 		= 5006;
	static const AControlID	kCancelButton 				= 5010;
	static const AControlID	kErrorCharactersButton		= 5011;//#473
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_TextSheet_CorrectEncoding; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_TextSheet_CorrectEncoding"; }
};

#pragma mark ===========================
#pragma mark [�N���X]AWindow_TextSheet_MoveLine
//�s�ړ�
class AWindow_TextSheet_MoveLine : public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_TextSheet_MoveLine(/*#199  AWindow_Text& inTextWindow */const AWindowID inTextWindowID );
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	AWindow_Text&			GetTextWindow();//#199 {return mTextWindow;}
	//#199 AWindow_Text&						mTextWindow;
	AWindowID							mTextWindowID;
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();//win
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  public:
	void					SPNVI_Show( const AIndex inLineIndex, const AItemCount inLineCount, const ABool inParagraphMode );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	ABool								mParagraphMode;
	AItemCount							mLineCount;
	
	//�R���g���[��ID
	static const AControlID				kOKButton 			= 5001;
	static const AControlID				kNumber 			= 5002;
	static const AControlID				kTitle 				= 5003;
	static const AControlID				kMax 				= 5004;
	static const AControlID				kCancelButton 		= 5010;
	
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_TextSheet_MoveLine; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_TextSheet_MoveLine"; }
};

#pragma mark ===========================
#pragma mark [�N���X]AWindow_TextSheet_FontSize
//�t�H���g�T�C�Y
class AWindow_TextSheet_FontSize : public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_TextSheet_FontSize( /*#199 AWindow_Text& inTextWindow*/const AWindowID inTextWindowID );
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	AWindow_Text&			GetTextWindow();//#199 {return mTextWindow;}
	//#199 AWindow_Text&						mTextWindow;
	AWindowID							mTextWindowID;
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();//win
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_Show();
	ADocumentID							mTextDocumentID;
	
	//�R���g���[��ID
	static const AControlID	kOKButton 						= 5001;
	static const AControlID	kNumber 						= 5002;
	static const AControlID	kCancelButton 					= 5010;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_TextSheet_FontSize; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_TextSheet_FontSize"; }
};

#pragma mark ===========================
#pragma mark [�N���X]AWindow_TextSheet_WrapLetterCount
//�s���蕶����
class AWindow_TextSheet_WrapLetterCount : public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_TextSheet_WrapLetterCount( /*#199 AWindow_Text& inWindow*/const AWindowID inTextWindowID );
	void					SPI_SetWrapMode( const AWrapMode inWrapMode );//#1113
  private:
	AWrapMode							mWrapMode;//#1113
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	AWindow_Text&			GetTextWindow();//#199 {return mTextWindow;}
	//#199 AWindow_Text&						mTextWindow;
	AWindowID							mTextWindowID;
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();//win
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	ADocumentID							mTextDocumentID;
	
	//�R���g���[��ID
	static const AControlID	kButton_OK 						= 5001;
	static const AControlID	kNumber 						= 5002;
	static const AControlID	kButton_Cancel 					= 5010;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_TextSheet_WrapLetterCount; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_TextSheet_WrapLetterCount"; }
};

#if 0
#pragma mark ===========================
#pragma mark [�N���X]AWindow_TextSheet_Property
//�v���p�e�B
class AWindow_TextSheet_Property : public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_TextSheet_Property( /*#199 AWindow_Text& inTextWindow*/const AWindowID inTextWindowID );
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	AWindow_Text&			GetTextWindow();//#199 {return mTextWindow;}
	//#199 AWindow_Text&						mTextWindow;
	AWindowID							mTextWindowID;
	ADataBase&				NVMDO_GetDB() {return mLocalDataBase;}
	ADataBase							mLocalDataBase;
	ADataBase							mBackupDataBase;
	ADocumentID							mTextDocumentID;
	ABool								mSavedDirty;
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();//win
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					Adjust( const AControlID inControlID );
	void					NVMDO_NotifyDataChanged( const AControlID inControlID, const AModificationType inModificationType, const AIndex inIndex,
							const AControlID inTriggerControlID/*B0406*/ );
	void					NVMDO_UpdateControlStatus();
	
	//�R���g���[��ID
	static const AControlID	kButton_OK 						= 5001;
	static const AControlID	kButton_Cancel 					= 5002;
	static const AControlID	kCheck_SetCreatorType 			= 5006;
	static const AControlID	kText_Creator 					= 5007;
	static const AControlID	kText_Type 						= 5008;
	//#244
	static const AControlID	kPopup_TextFontName				= 5137;
	static const AControlID	kText_TextFontSize				= 5103;
	static const AControlID	kPopup_AntiAliasMode			= 5133;
	static const AControlID	kText_TabWidth					= 5115;
	static const AControlID	kText_IndentWidth				= 5136;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_TextSheet_Property; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_TextSheet_Property"; }
	};
#endif

#if 0
#pragma mark ===========================
#pragma mark [�N���X]AWindow_TextSheet_PrintOption
//����I�v�V����
class AWindow_TextSheet_PrintOption: public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_TextSheet_PrintOption( /*#199 AWindow_Text& inTextWindow*/const AWindowID inTextWindowID );
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	AWindow_Text&			GetTextWindow();//#199 {return mTextWindow;}
	//#199 AWindow_Text&						mTextWindow;
	AWindowID							mTextWindowID;
	ADataBase&				NVMDO_GetDB() {return mLocalDataBase;}
	ADataBase							mLocalDataBase;
	ABool								mSavedDirty;
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();//win
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	ADocumentID							mTextDocumentID;
	
	//�R���g���[��ID
	static const AControlID	kButton_OK 						= 5001;
	static const AControlID	kButton_Cancel 					= 5002;
	//#244
	const static APrefID	kControl_UsePrintFont			= 5116;
	const static APrefID	kControl_PrintPageNumber		= 5117;
	const static APrefID	kControl_PrintFileName			= 5118;
	const static APrefID	kControl_PrintLineNumber		= 5119;
	const static APrefID	kControl_PrintSeparateLine		= 5120;
	const static APrefID	kControl_ForceWordWrap			= 5121;
	const static APrefID	kControl_PrintFontSize			= 5123;
	const static APrefID	kControl_PageNumberFontSize		= 5125;
	const static APrefID	kControl_FileNameFontSize		= 5127;
	const static APrefID	kControl_LineMargin				= 5129;
	const static APrefID	kControl_PrintFontName			= 5138;//win
	const static APrefID	kControl_PageNumberFontName		= 5139;//win
	const static APrefID	kControl_FileNameFontName		= 5140;//win
	const static APrefID	kControl_LineNumberFontName		= 5141;//win
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_TextSheet_PrintOption; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_TextSheet_PrintOption"; }
};
#endif

#pragma mark ===========================
#pragma mark [�N���X]AWindow_TextSheet_DropWarning
//�s���蕶����
class AWindow_TextSheet_DropWarning : public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_TextSheet_DropWarning( /*#199 AWindow_Text& inWindow*/const AWindowID inTextWindowID );
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	AWindow_Text&			GetTextWindow();//#199 {return mTextWindow;}
	//#199 AWindow_Text&						mTextWindow;
	AWindowID							mTextWindowID;
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();//win
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  public:
	void					SPI_SetToolText( const AText& inText );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	ADocumentID							mTextDocumentID;
	AText								mToolText;
	
	//�R���g���[��ID
	static const AControlID	kButton_SaveExecute = 1;
	static const AControlID	kButton_DontSaveExecute = 3;
	static const AControlID	kButton_Cancel = 2;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_TextSheet_DropWarning; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_TextSheet_DropWarning"; }
};

#pragma mark ===========================
#pragma mark [�N���X]AWindow_TextSheet_EditByOtherApp
//���A�v���ł̕ҏW R0232
class AWindow_TextSheet_EditByOtherApp : public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_TextSheet_EditByOtherApp( /*#199 AWindow_Text& inWindow*/const AWindowID inTextWindowID );
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	AWindow_Text&			GetTextWindow();//#199 {return mTextWindow;}
	//#199 AWindow_Text&						mTextWindow;
	AWindowID							mTextWindowID;
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();//win
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  public:
	void					SPI_AddDocument( const ADocumentID inDocumentID );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					Update();
	
  private:
	AArray<ADocumentID>					mDocumentIDArray;
	
	//�R���g���[��ID
	static const AControlID	kStaticText = 100;
	//#688 static const AControlID	kCheck_ApplyAllDocuments = 101;
	static const AControlID	kButton_Reload = 200;
	static const AControlID	kButton_NoReload = 201;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_EditByOtherApp; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_TextSheet_EditByOtherApp"; }
};


#pragma mark ===========================
#pragma mark [�N���X]AWindow_TextSheet_SCMCommit
//#455
//�t�H���g�T�C�Y
class AWindow_TextSheet_SCMCommit : public AWindow
{
	//�R���X�g���N�^
  public:
	AWindow_TextSheet_SCMCommit( const AWindowID inTextWindowID );
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	AWindow_Text&			GetTextWindow();
	AWindowID							mTextWindowID;
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();
	
	//<�C���^�[�t�F�C�X>
	//�E�C���h�E����
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_Hide();
	ADocumentID							mTextDocumentID;
	
	//
  private:
	void					UpdateDiff();
	void					UpdateRecentMessageMenu();
	void					AddRecentMessage();
	void					Commit();
	ABool								mButtonIsCommit;
	
	//�R���g���[��ID
  private:
	static const AControlID	kPathText 						= 100;
	static const AControlID	kRecentMessageMenu 				= 101;
	static const AControlID	kMessageText 					= 102;
	static const AControlID	kAdditionalArgText 				= 103;
	static const AControlID	kResultText		 				= 104;
	static const AControlID	kDiffText		 				= 105;
	static const AControlID	kDiffCount		 				= 106;
	static const AControlID	kCommitButton		 			= 200;
	static const AControlID	kCancelButton	 				= 201;
	static const AControlID	kPrevDiff		 				= 202;
	static const AControlID	kNextDiff		 				= 203;
	static const AControlID	kOKButton			 			= 204;
	static const AControlID	kStaticText1	 				= 301;
	static const AControlID	kStaticText2	 				= 302;
	static const AControlID	kStaticText3	 				= 303;
	static const AControlID kTabViewSelect_Normal			= 350;
	static const AControlID kTabViewSelect_Result			= 351;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_TextSheet_SCMCommit; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_TextSheet_SCMCommit"; }
};

