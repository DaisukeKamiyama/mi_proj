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

AView_CallGraf
#723

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AThread_WordsListFinder.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_CallGraf

/**
�R�[���O���t�r���[
*/
class AView_CallGraf : public AView
{
	//�R���X�g���N�^�A�f�X�g���N�^
  public:
	AView_CallGraf( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_CallGraf();
  private:
	void					NVIDO_Init();
	void					NVIDO_DoDeleted();
	
	//<�֘A�I�u�W�F�N�g�擾>
  public:
  private:
	AThread_WordsListFinder&	GetWordsListFinderThread();
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_DoTextInput( const AText& inText, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu );
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
							const ALocalPoint inLocalPoint );
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint );
	void					ClearHoverCursor();
	ABool					EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	ABool					EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) ;
	void					GetBoxRect( const AIndex inColumnIndex, const AIndex inItemIndex, AImageRect& outRect ) const;
	void					FindBoxIndexByPoint( const AImagePoint& inPoint, AIndex& outLevelIndex, AIndex& outItemIndex ) const;
	void					DrawItem( const ALocalRect& inLocalRect, 
							const AIndex inModeIndex, const AIndex inCategoryIndex,
							const AText& inFilePath, const AText& inClassName, const AText& inFunctionName,
							const ABool inCurrent, const ABool inHover, const ABool inVisited, const ABool inEdited,
							const AFloatNumber inAlpha );
	AIndex					GetLevelFromColumn( const AIndex inColumnIndex ) const;
	void					UpdateDrawProperty();
	void					GetItemData( const AIndex inLevelIndex, const AIndex inItemIndex, 
							AText& outFilePath, AIndex& outStartIndex, AIndex& outEndIndex,
							AIndex& outModeIndex, AIndex& outCategoryIndex ) const;
	ACursorType				EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	
	//<�C���^�[�t�F�C�X>
  public:
	void					SPI_OpenItem( const AIndex inLevel, const AIndex inItemIndex, const AModifierKeys inModifierKeys );
	void					SPI_SetCurrentFunction( const AFileAcc& inProjectFolder, 
							const AIndex inModeIndex, const AIndex inCategoryIndex,
							const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
												   const AIndex inStartIndex, const AIndex inEndIndex, const ABool inByEdit );
	void					SPI_SetEditFlag( const AText& inFilePath, const AText& inClassName, const AText& inFunctionName );
	void					SPI_DoWordsListFinderPaused();
	void					SPI_NotifyToCallGrafByIdInfoJump( const AText& inCallerFuncIdText, 
															 const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
							const AIndex inStartIndex, const AIndex inEndIndex );
	void					SPI_NotifyToCallGrafByTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount );
	void					SPI_ReturnHome();
	void					SPI_GoLeftLevel();
	void					SPI_GoRightLevel();
	void					SPI_DeleteAllData();
	void					SPI_AbortCurrentWordsListFinderRequest();
	void					SPI_SelectNextItem();
	void					SPI_SelectPreviousItem();
	void					SPI_AdjustScroll();
  private:
	void					AddToVisited( const AText& inFuncIdText );
	void					UpdateArray( const AIndex inLevelIndex, const AIndex inModeIndex, const AText& inFunctionName );
	void					UpdateHeaderView();
	void					UpdateImageSize();
	void					UpdateCallerArrayForTheLatestHistory();
	//void					SetSelect( const AIndex inLevel, const AIndex inItemIndex );
	
	//FuncId�擾
  public:
	void					SPI_GetFuncIdText( const AIndex inLevel, const AIndex inItemIndex, AText& outFuncIdText ) const;
	static void				SPI_GetFuncIdText( const AText& inFilePath, const AText& inClassName, const AText& inFuncName,
							AText& outFuncIdText );
	
	//�������s���t���O�i�����v���O���X��\�����邽�߂Ɏg�p����j
  public:
	ABool					SPI_IsFinding() const { return mIsFinding; }
  private:
	ABool								mIsFinding;
	
	//�f�[�^
  private:
	//���݂̑I������
	AIndex								mCurrentSelection_LevelIndex;
	AIndex								mCurrentSelection_ItemIndex;
	
	//�z�o�[����
	AIndex								mCurrentHover_LevelIndex;
	AIndex								mCurrentHover_ItemIndex;
	
	//���[�ɕ`�悳���level index
	AIndex								mCurrentDisplayStartLevel;
	
	//���݂̃v���W�F�N�g�t�H���_
	AFileAcc							mCurrentProjectFolder;
	
	//����array
	AObjectArray<AHashTextArray>		mCallerArray_FilePath;
	AObjectArray<ATextArray>			mCallerArray_FunctionName;
	AObjectArray<ATextArray>			mCallerArray_ClassName;
	AObjectArray< AArray<AIndex> >		mCallerArray_StartIndex;
	AObjectArray< AArray<AIndex> >		mCallerArray_EndIndex;
	AObjectArray< AArray<AIndex> >		mCallerArray_ModeIndex;
	AObjectArray< AArray<AIndex> >		mCallerArray_CategoryIndex;
	AObjectArray<ABoolArray>			mCallerArray_EdittedFlag;
	AObjectArray<ABoolArray>			mCallerArray_ClickedFlag;
	AObjectArray<ATextArray>			mCallerArray_CallerFuncId;
	
	//array���̌���index
	AArray<AIndex>						mCallerArray_CalleeItemIndex;
	
	//level0��FuncId�i�����(=level0)���瓯��function���������邽�߂Ɏg�p����j
	AHashTextArray						mLevel0FuncIdTextArray;
	
	//�S�t�@�C���T�[�`�ς݃t���O
	ABool								mAllFilesSearched;
	
	//�earray�̌��ݑI��word�i�w�b�_�\���̂��߂Ɏg�p����j
	ATextArray							mCurrentWordArray;
	
	//�K��ς�FuncId
	AHashTextArray						mVisitedFuncIdTextArray;
	
	//id info����̃W�����v�̒ʒm���L�^
	AText								mIdInfoJumpMemory_FilePath;
	AText								mIdInfoJumpMemory_ClassName;
	AText								mIdInfoJumpMemory_FunctionName;
	AText								mIdInfoJumpMemory_CallerFuncId;
	
	//�Ō�̗v���f�[�^
	AFileAcc							mLastSetCurrentFunction_ProjectFolder;
	AIndex								mLastSetCurrentFunction_ModeIndex;
	AIndex								mLastSetCurrentFunction_CategoryIndex;
	AText								mLastSetCurrentFunction_FilePath;
	AText								mLastSetCurrentFunction_ClassName;
	AText								mLastSetCurrentFunction_FunctionName;
	AIndex								mLastSetCurrentFunction_StartIndex;
	AIndex								mLastSetCurrentFunction_EndIndex;
	ABool								mLastSetCurrentFunction_ByEdit;
	
	//�`��f�[�^
	ANumber								mContentFontHeight;
	ANumber								mHeaderFontHeight;
	ANumber								mBoxWidth;
	ANumber								mBoxHeight;
	ANumber								mContentHeight;
	ANumber								mHeaderHeight;
	
	//�X���b�h
  private:
	AObjectID							mWordsListFinderThreadID;
	
	//���擾
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_CallGraf;}
};

