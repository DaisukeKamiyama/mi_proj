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

#include "stdafx.h"

#include "AView_CallGraf.h"
#include "AView_CallGrafHeader.h"
#include "AApp.h"

//�]�����T�C�Y
const ANumber	kLeftMargin_Box = 5;
const ANumber	kRightMargin_Box = 3;
const ANumber	kTopMargin_Box = 3;
const ANumber	kBottomMargin_Box = 3;
const ANumber	kCallerCalleeLineWidth = 8;
const ANumber	kTopPadding_Header = 3;
const ANumber	kBottomPadding_Header = 0;
const ANumber	kTopPadding_Content = 3;
const ANumber	kBottomPadding_Content = 0;

//Import File Progress�\���G���A����
const ANumber	kProgressTextAreaHeight = 16;

#pragma mark ===========================
#pragma mark [�N���X]AView_CallGraf
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AView_CallGraf::AView_CallGraf( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mCurrentSelection_LevelIndex(kIndex_Invalid), mCurrentSelection_ItemIndex(kIndex_Invalid),
		mCurrentHover_LevelIndex(kIndex_Invalid), mCurrentHover_ItemIndex(kIndex_Invalid),
		mAllFilesSearched(false), mCurrentDisplayStartLevel(1),
		mContentFontHeight(9), mHeaderFontHeight(9), mBoxWidth(100), mContentHeight(30), mHeaderHeight(10), mBoxHeight(40)
,mWordsListFinderThreadID(kObjectID_Invalid),mIsFinding(false)
{
	NVMC_SetOffscreenMode(true);
	
	//level0 array�ǉ�
	mCallerArray_FilePath.AddNewObject();
	mCallerArray_FunctionName.AddNewObject();
	mCallerArray_ClassName.AddNewObject();
	mCallerArray_StartIndex.AddNewObject();
	mCallerArray_EndIndex.AddNewObject();
	mCallerArray_ModeIndex.AddNewObject();
	mCallerArray_CategoryIndex.AddNewObject();
	mCallerArray_EdittedFlag.AddNewObject();
	mCallerArray_ClickedFlag.AddNewObject();
	mCallerArray_CallerFuncId.AddNewObject();
	//array�̌��ݑI��index�i�R�[����item�j
	mCallerArray_CalleeItemIndex.Add(kIndex_Invalid);
	//array�̌��ݒP��
	mCurrentWordArray.Add(GetEmptyText());
	
	//���[�h���X�g�����X���b�h�����E�N��
	AThread_WordsListFinderFactory	factory(this);
	mWordsListFinderThreadID = GetApp().NVI_CreateThread(factory);
	GetWordsListFinderThread().SetWordsListFinderType(kWordsListFinderType_ForCallGraf);
	GetWordsListFinderThread().NVI_Run(true);
	
	//�S�f�[�^�폜
	SPI_DeleteAllData();
	
	//�����f�[�^��ǂ݂���
	AItemCount	itemCount = GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kCallGrafHistory_FilePath);
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AText	filepath, classname, functionname;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kCallGrafHistory_FilePath,i,filepath);
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kCallGrafHistory_ClassName,i,classname);
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kCallGrafHistory_FunctionName,i,functionname);
		AIndex	startIndex = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kCallGrafHistory_StartIndex,i);
		AIndex	endIndex = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kCallGrafHistory_EndIndex,i);
		AText	modename;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kCallGrafHistory_ModeName,i,modename);
		AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modename);
		if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
		//ABool	edittedFlag = GetApp().NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kCallGrafHistory_EdittedFlag,i);
		//funcID�擾
		AText	funcIdText;
		AView_CallGraf::SPI_GetFuncIdText(filepath,classname,functionname,funcIdText);
		//����ǉ�
		mCallerArray_FilePath.GetObject(0).Add(filepath);
		mCallerArray_FunctionName.GetObject(0).Add(functionname);
		mCallerArray_ClassName.GetObject(0).Add(classname);
		mCallerArray_StartIndex.GetObject(0).Add(startIndex);
		mCallerArray_EndIndex.GetObject(0).Add(endIndex);
		mCallerArray_ModeIndex.GetObject(0).Add(modeIndex);
		mCallerArray_CategoryIndex.GetObject(0).Add(kIndex_Invalid);//category index�͕ς���Ă���\��������̂ŁA�L�������A�����Ȃ�
		mCallerArray_EdittedFlag.GetObject(0).Add(false);//edittedFlag);
		mCallerArray_ClickedFlag.GetObject(0).Add(true);
		mCallerArray_CallerFuncId.GetObject(0).Add(GetEmptyText());
		mLevel0FuncIdTextArray.Add(funcIdText);
	}
	//Image size�X�V
	UpdateImageSize();
}

/**
�f�X�g���N�^
*/
AView_CallGraf::~AView_CallGraf()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_CallGraf::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_CallGraf::NVIDO_DoDeleted()
{
	//���[�h���X�g�����X���b�h�I���A�폜
	if( mWordsListFinderThreadID != kObjectID_Invalid )
	{
		GetApp().NVI_DeleteThread(mWordsListFinderThreadID);
		mWordsListFinderThreadID = kObjectID_Invalid;
	}
	//�����̋L��
	GetApp().NVI_GetAppPrefDB().DeleteAllRows_Table(AAppPrefDB::kCallGrafHistory_FilePath);
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == false )
	{
		for( AIndex i = 0; i < mCallerArray_FilePath.GetObjectConst(0).GetItemCount(); i++ )
		{
			AText	filepath, classname, functionname;
			mCallerArray_FilePath.GetObjectConst(0).Get(i,filepath);
			mCallerArray_ClassName.GetObjectConst(0).Get(i,classname);
			mCallerArray_FunctionName.GetObjectConst(0).Get(i,functionname);
			ANumber	startIndex = mCallerArray_StartIndex.GetObjectConst(0).Get(i);
			ANumber	endIndex = mCallerArray_EndIndex.GetObjectConst(0).Get(i);
			AIndex	modeIndex = mCallerArray_ModeIndex.GetObjectConst(0).Get(i);
			AText	modename;
			GetApp().SPI_GetModePrefDB(modeIndex,false).GetModeRawName(modename);
			ABool	edittedFlag = mCallerArray_EdittedFlag.GetObjectConst(0).Get(i);
			//
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kCallGrafHistory_FilePath,filepath);
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kCallGrafHistory_ClassName,classname);
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kCallGrafHistory_FunctionName,functionname);
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kCallGrafHistory_StartIndex,startIndex);
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kCallGrafHistory_EndIndex,endIndex);
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kCallGrafHistory_ModeName,modename);
			GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kCallGrafHistory_EdittedFlag,edittedFlag);
		}
	}
}

/**
���[�h���X�g�����X���b�h�擾
*/
AThread_WordsListFinder&	AView_CallGraf::GetWordsListFinderThread()
{
	return (dynamic_cast<AThread_WordsListFinder&>(GetApp().NVI_GetThreadByID(mWordsListFinderThreadID)));
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�`��v�����̃R�[���o�b�N(AView�p)
*/
void	AView_CallGraf::EVTDO_DoDraw()
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	
	//�`��f�[�^�X�V
	UpdateDrawProperty();
	
	//=========================�`��f�[�^�擾=========================
	//�t�H���g�擾
	//���e�����t�H���g
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//�w�b�_�����t�H���g
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	
	//=========================�w�i���擾�i����view�ł͔w�i�͕`�悵�Ȃ��j=========================
	//�w�i���擾
	AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
	AFloatNumber	backgroundAlpha = 1.0;
	switch(GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()))
	{
	  case kSubWindowLocationType_Floating:
		{
			backgroundAlpha = GetApp().SPI_GetFloatingWindowAlpha();
			break;
		}
	  case kSubWindowLocationType_Popup:
		{
			backgroundAlpha = GetApp().SPI_GetPopupWindowAlpha();
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//
	//NVMC_PaintRect(frameRect,backgroundColor,backgroundAlpha);
	
	//=========================�e���ڕ`��=========================
	//�e�J���������[�v
	for( AIndex columnIndex = 0; columnIndex <= 1; columnIndex++ )
	{
		//�J��������level�擾
		AIndex	level = GetLevelFromColumn(columnIndex);
		
		//�Ώ�level�̃f�[�^�������Ȃ牽�����Ȃ�
		if( level >= mCallerArray_FilePath.GetItemCount() )   continue;
		
		//�e���ږ����[�v
		for( AIndex index = 0; index < mCallerArray_FilePath.GetObjectConst(level).GetItemCount(); index++ )
		{
			//box�擾
			AImageRect	box = {0};
			GetBoxRect(columnIndex,index,box);
			ALocalRect	localBox = {0};
			NVM_GetLocalRectFromImageRect(box,localBox);
			
			//Update�̈攻��i�P���box�܂ŕ`�悷��B�i���`��̂��߁j
			ALocalRect	updateBox = localBox;
			updateBox.top -= (localBox.bottom - localBox.top)*2;
			ABool	insideUpdateRegion = NVMC_IsRectInDrawUpdateRegion(updateBox);
			
			//update�̈���̏ꍇ�̕`�揈��
			if( insideUpdateRegion == true )
			{
				//���ڃf�[�^�擾
				AText	filepath;
				mCallerArray_FilePath.GetObjectConst(level).Get(index,filepath);
				AText	classname;
				mCallerArray_ClassName.GetObjectConst(level).Get(index,classname);
				AText	funcname;
				mCallerArray_FunctionName.GetObjectConst(level).Get(index,funcname);
				AIndex	modeIndex = mCallerArray_ModeIndex.GetObjectConst(level).Get(index);
				AIndex	categoryIndex = mCallerArray_CategoryIndex.GetObjectConst(level).Get(index);
				//FuncId�擾
				AText	funcIdText;
				SPI_GetFuncIdText(level,index,funcIdText);
				
				//���ڕ`��
				DrawItem(localBox,modeIndex,categoryIndex,filepath,classname,funcname,
						 //(level == mCurrentSelection_LevelIndex && index == mCurrentSelection_ItemIndex),
						 (mCallerArray_CalleeItemIndex.Get(level) == index ),//�I�����ڂ��ǂ���
							(level == mCurrentHover_LevelIndex && index == mCurrentHover_ItemIndex),//�z�o�[
							//(mVisitedFuncIdTextArray.Find(funcIdText) != kIndex_Invalid),
							(mCallerArray_ClickedFlag.GetObjectConst(level).Get(index)==true&&level!=0),//�K��ς݃t���O
							mCallerArray_EdittedFlag.GetObjectConst(level).Get(index),//�ҏW�L��t���O
							backgroundAlpha);
				
				//==================�L�[���[�h���ɂ��W�����v��==================
				//Id info�N���b�N�ɂ��ړ��̏ꍇ�ɁA�ړ���ƈړ����̊֐�����Ō��ԁi���x��0���j
				if( level == 0 && index+1 < mCallerArray_FilePath.GetObjectConst(level).GetItemCount() )
				{
					AText	nextItemFuncId;
					SPI_GetFuncIdText(level,index+1,nextItemFuncId);
					if( mCallerArray_CallerFuncId.GetObjectConst(level).Compare(index,nextItemFuncId) == true )
					{
						ALocalPoint	spt = {0}, ept = {0};
						spt.x = (localBox.left+localBox.right)/2 -3;
						spt.y = localBox.bottom;
						ept = spt;
						ept.y += 5;
						NVMC_DrawLine(spt,ept,kColor_Gray20Percent,0.7,0.0,3.0);
					}
				}
			}
			//Callee�֍Œ�P�{�̐���\�����邽�߂�index==0�̍��ڂ�update�̈�͈͊O�ł���ɕ\������悤�ɂ���
			if( insideUpdateRegion == true || index == 0 )
			{
				//==================Caller->Callee���`��i�J����index��0�̂Ƃ��̂݁j==================
				if( columnIndex == 0 )
				{
					AColor	lineColor = kColor_Gray70Percent;
					AFloatNumber	lineWidth = 1.0;
					AFloatNumber	lineAlpha = 1.0;
					/*
					if( mCallerArray_CalleeItemIndex.Get(level) == index )
					{
						lineColor = GetApp().SPI_GetSubWindowRoundedRectBoxSelectionColor();
						lineWidth = 2.0;
						lineAlpha = 1.0;
					}
					*/
					//�Ă΂��box�擾
					AImageRect	calleeBox = {0};
					GetBoxRect(columnIndex+1,mCallerArray_CalleeItemIndex.Get(GetLevelFromColumn(columnIndex+1)),calleeBox);
					ALocalRect	localCalleeBox = {0};
					NVM_GetLocalRectFromImageRect(calleeBox,localCalleeBox);
					//���`��
					ALocalPoint	spt = {0}, ept = {0};
					spt.x = localBox.right;
					spt.y = (localBox.top+localBox.bottom)/2;
					ept.x = localCalleeBox.left;
					ept.y = (localCalleeBox.top+localCalleeBox.bottom)/2;
					NVMC_DrawCurvedLine(spt,ept,lineColor,lineAlpha,0.0,lineWidth);
				}
			}
		}
	}
	//==================�I�𒆂̏ꍇ�A�I�����ڂƌĂ΂�����ŕ`��==================
	//�Y��Ȑŕ`�悷�邽�߂ɁA���̐���S�ď�������ɁA��=1.0�ŕ`�悷��
	if( mCurrentSelection_LevelIndex > 0 )
	{
		AIndex	columnIndex = 0;
		AIndex	level = GetLevelFromColumn(columnIndex);
		if( mCallerArray_CalleeItemIndex.GetItemCount() > level )
		{
			AIndex	index = mCallerArray_CalleeItemIndex.Get(level);
			if( index != kIndex_Invalid )
			{
				//==================�I�����ڂƌĂ΂�����ŕ`��==================
				//�Ăь�box�擾
				AImageRect	box = {0};
				GetBoxRect(columnIndex,index,box);
				ALocalRect	localBox = {0};
				NVM_GetLocalRectFromImageRect(box,localBox);
				//�Ă΂��box�擾
				AImageRect	calleeBox = {0};
				GetBoxRect(columnIndex+1,mCallerArray_CalleeItemIndex.Get(GetLevelFromColumn(columnIndex+1)),calleeBox);
				ALocalRect	localCalleeBox = {0};
				NVM_GetLocalRectFromImageRect(calleeBox,localCalleeBox);
				//�I��F�擾
				AColor	selColor = GetApp().SPI_GetSubWindowRoundedRectBoxSelectionColor(NVM_GetWindow().GetObjectID());
				//���`��
				ALocalPoint	spt = {0}, ept = {0};
				spt.x = localBox.right;
				spt.y = (localBox.top+localBox.bottom)/2;
				ept.x = localCalleeBox.left;
				ept.y = (localCalleeBox.top+localCalleeBox.bottom)/2;
				NVMC_DrawCurvedLine(spt,ept,selColor,1.0,0.0,2.0);
				
				//==================�I�����ڂ̍��ɐ���`��==================
				spt.x = frameRect.left;
				spt.y = (localBox.top+localBox.bottom)/2;
				ept.x = localBox.left;
				ept.y = spt.y;
				NVMC_DrawCurvedLine(spt,ept,kColor_Gray70Percent,1.0,0.0,2.0);
			}
		}
	}
	//==================�E��I�����ڂ̉E�ɐ���`��==================
	{
		AIndex	columnIndex = 1;
		AIndex	level = GetLevelFromColumn(columnIndex);
		if( level > 0 && mCallerArray_CalleeItemIndex.GetItemCount() > level )
		{
			AIndex	index = mCallerArray_CalleeItemIndex.Get(level);
			if( index != kIndex_Invalid )
			{
				//�I��F�擾
				AColor	selColor = GetApp().SPI_GetSubWindowRoundedRectBoxSelectionColor(NVM_GetWindow().GetObjectID());
				//�Ăь�box�擾
				AImageRect	box = {0};
				GetBoxRect(columnIndex,index,box);
				ALocalRect	localBox = {0};
				NVM_GetLocalRectFromImageRect(box,localBox);
				//���`��
				ALocalPoint	spt = {0}, ept = {0};
				spt.x = localBox.right;
				spt.y = (localBox.top+localBox.bottom)/2;
				ept.x = frameRect.right;
				ept.y = spt.y;
				NVMC_DrawCurvedLine(spt,ept,selColor,1.0,0.0,2.0);
			}
		}
	}
}

/**
�J����index����level idnex�擾
*/
AIndex	AView_CallGraf::GetLevelFromColumn( const AIndex inColumnIndex ) const
{
	return (mCurrentDisplayStartLevel - inColumnIndex);
}

/**
*/
void	AView_CallGraf::UpdateDrawProperty()
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//�t�H���g�̍������擾
	NVMC_SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);
	ANumber	fontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(mContentFontHeight,fontascent);
	//�w�b�_�����t�H���g
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	//�w�b�_�����t�H���g�����擾
	NVMC_SetDefaultTextProperty(headerfontname,fontsize,kColor_Black,kTextStyle_Normal,true);
	ANumber	headerfontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(mHeaderFontHeight,headerfontascent);
	
	//�w�b�_�E�R���e���g�����̍���
	mHeaderHeight = kTopPadding_Header + mHeaderFontHeight + kBottomPadding_Header;
	mContentHeight = kTopPadding_Content + mContentFontHeight*2 + kBottomPadding_Content;
	
	//box�̕��E����
	mBoxWidth = (frameRect.right - frameRect.left - (kLeftMargin_Box + kRightMargin_Box + kCallerCalleeLineWidth)) /2;
	mBoxHeight = mHeaderHeight + mContentHeight;
}

/**
���ڕ`��
*/
void	AView_CallGraf::DrawItem( const ALocalRect& inLocalBoxRect, 
		const AIndex inModeIndex, const AIndex inCategoryIndex,
		const AText& inFilePath, const AText& inClassName, const AText& inFunctionName,
		const ABool inCurrent, const ABool inHover, const ABool inVisited, const ABool inEdited, const AFloatNumber inAlpha ) 
{
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	fontsize -= 0.5;
	//�t�H���g�̍������擾
	NVMC_SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);
	ANumber	fontheight = 9, fontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(fontheight,fontascent);
	//�w�b�_�����t�H���g
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	//�w�b�_�����t�H���g�����擾
	NVMC_SetDefaultTextProperty(headerfontname,fontsize,kColor_Black,kTextStyle_Normal,true);
	ANumber	headerfontheight = 9, headerfontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(headerfontheight,headerfontascent);
	
	//==================�F�擾==================
	
	//�`��F�ݒ�
	AColor	letterColor = kColor_Black;
	AColor	dropShadowColor = kColor_White;
	AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
	GetApp().SPI_GetSubWindowBoxColor(NVM_GetWindow().GetObjectID(),letterColor,dropShadowColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
	NVMC_SetDropShadowColor(dropShadowColor);
	
	//�J�e�S���F�擾
	AColor	categoryColor = kColor_Black;//#1316 ���@�\��������Ȃ�_�[�N���[�h�Ή��K�vGetApp().SPI_GetSubWindowsBoxHeaderColor();//�w�i���̏ꍇ���܂߂��F�݌v������̂ŁA�Œ�F kColor_Black;
	/*
	if( inCategoryIndex != kIndex_Invalid )
	{
		GetApp().SPI_GetModePrefDB(inModeIndex).GetCategoryColor(inCategoryIndex,categoryColor);
	}
	*/
	
	//�n�C���C�g�F�擾
	AColor	highlightColor = kColor_Blue;
	AColorWrapper::GetHighlightColor(highlightColor);
	
	//===================�S�� box�`��===================
	
	//content�����F�擾
	AColor	contentColor_start = kColor_White;
	AColor	contentColor_end = kColor_Gray95Percent;
	//�I�����ڂȂ�n�C���C�g�F�ŕ`��
	/*
	if( inCurrent == true )
	{
		//�A�N�e�B�u�ȃT�u�E�C���h�E���ǂ����őI��\���F��ς���
		if( NVM_GetWindow().GetObjectID() == GetApp().SPI_GetActiveSubWindowForItemSelector() )
		{
			contentColor_end = highlightColor;
		}
		else
		{
			contentColor_end = kColor_Gray80Percent;
		}
	}
	*/
	//box�`��
	NVMC_PaintRoundedRect(inLocalBoxRect,
						  //contentColor_start,contentColor_end,
						  boxBaseColor1,boxBaseColor3,
						  kGradientType_Vertical,inAlpha,inAlpha,
						  5,true,true,true,true);
	
	//==================�z�o�[���`��==================
	if( inHover == true )
	{
		/*
		NVMC_PaintRoundedRect(inLocalBoxRect,highlightColor,highlightColor,kGradientType_None,0.3,0.3,
							  5,true,true,true,true);
							  */
		NVMC_FrameRoundedRect(inLocalBoxRect,kColor_Blue,0.3,5,true,true,true,true,true,true,true,true,1.0);
	}
	//===================header box�`��===================
	
	//header box rect�擾
	ALocalRect	headerRect = inLocalBoxRect;
	headerRect.bottom = inLocalBoxRect.top + mHeaderHeight;
	
	//�ҏW����Ȃ�w�b�_�[�F��ς��ĕ\��
	AFloatNumber	headerAlpha = 0.15;
	if( inEdited == true )
	{
		//categoryColor = kColor_Red;
		headerAlpha = 0.3;
	}
	//header�����F�擾
	AColor	headerColor_start = AColorWrapper::ChangeHSV(categoryColor,0,0.8,1.0);
	AColor	headerColor_end = AColorWrapper::ChangeHSV(categoryColor,0,1.2,1.0);
	//�K��ς݂̏ꍇ�̐F�擾
	if( inVisited == true )
	{
		headerColor_start = AColorWrapper::ChangeHSV(categoryColor,0,0.2,0.5);
		headerColor_end = AColorWrapper::ChangeHSV(categoryColor,0,0.4,0.5);
	}
	
	//box�`��
	NVMC_PaintRoundedRect(headerRect,headerColor_start,headerColor_end,kGradientType_Vertical,headerAlpha,headerAlpha,
						  5,true,true,true,true);
	
	//�t�H���g�ݒ�
	NVMC_SetDefaultTextProperty(headerfontname,fontsize,letterColor,kTextStyle_Normal,true);
	//�t�@�C�����`��
	ALocalRect	fileNameRect = headerRect;
	fileNameRect.left += 5;
	fileNameRect.right -= 1;
	fileNameRect.top += 1;
	AText	filename;
	inFilePath.GetFilename(filename);
	NVMC_DrawText(fileNameRect,filename,letterColor,kTextStyle_Bold|kTextStyle_DropShadow,kJustification_Left);
	
	//===================content �`��===================
	
	//content����rect�擾
	ALocalRect	contentRect = inLocalBoxRect;
	contentRect.top = inLocalBoxRect.top + mHeaderHeight;
	
	//�t�H���g�ݒ�
	NVMC_SetDefaultTextProperty(fontname,fontsize,letterColor,kTextStyle_Normal,true);
	
	//class���`��
	if( inClassName.GetItemCount() > 0 )
	{
		ALocalRect	classNameRect = contentRect;
		classNameRect.left += 5;
		classNameRect.right -= 1;
		classNameRect.top += 0;
		AText	classname;
		classname.SetText(inClassName);
		AText	t;
		t.SetLocalizedText("Letter_ParentKeyword");
		classname.InsertText(0,t);
		NVMC_DrawText(classNameRect,classname,letterColor,kTextStyle_DropShadow);
	}
	
	//�J�e�S���F���Â����čʓx���グ���F���擾
	AColor	categoryColorDraken = kColor_Black;//#1316 ���@�\��������Ȃ�_�[�N���[�h�Ή��K�vGetApp().SPI_GetSubWindowBoxFunctionNameColor();//�w�i���̏ꍇ���܂߂��F�݌v������̂ŁA�Œ�F AColorWrapper::ChangeHSV(categoryColor,0,3.0,0.3);
	//func���`��
	ALocalRect	funcNameRect = contentRect;
	funcNameRect.left += 8;
	funcNameRect.top += mContentFontHeight;
	AText	funcname;
	funcname.SetText(inFunctionName);
	NVMC_DrawText(funcNameRect,funcname,categoryColorDraken,kTextStyle_Bold);
	
	//==================�t���[���`��i�I�����ڂ͑I��F�ň͂ށj==================
	if( inCurrent == true )
	{
		AColor	selColor = GetApp().SPI_GetSubWindowRoundedRectBoxSelectionColor(NVM_GetWindow().GetObjectID());
		NVMC_FrameRoundedRect(inLocalBoxRect,selColor,1.0,5,true,true,true,true,true,true,true,true,1.0);
	}
	else
	{
		NVMC_FrameRoundedRect(inLocalBoxRect,kColor_Gray50Percent,1.0,5,true,true,true,true);
	}
}

/**
�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
*/
ABool	AView_CallGraf::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	AImagePoint	imagept = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	
	//�N���b�N����level, item���v�Z
	AIndex	level = kIndex_Invalid;
	AIndex	itemIndex = kIndex_Invalid;
	FindBoxIndexByPoint(imagept,level,itemIndex);
	
	//���ڂ��J��
	SPI_OpenItem(level,itemIndex,inModifierKeys);
	
	return true;
}

/**
���ڂ��J��
*/
void	AView_CallGraf::SPI_OpenItem( const AIndex inLevel, const AIndex inItemIndex, const AModifierKeys inModifierKeys )
{
	AIndex	level = inLevel;
	AIndex	itemIndex = inItemIndex;
	
	//���ڃN���b�N
	if( level != kIndex_Invalid && itemIndex != kIndex_Invalid )
	{
		//�N���b�N�ς݃t���OON
		mCallerArray_ClickedFlag.GetObject(level).Set(itemIndex,true);
		
		if( level == 0 )//&& itemIndex == 0 )
		{
			//------------------���x��0�̏ꍇ�́A�ŐV���ڂ֏�������ւ�------------------
			
			//�I������
			mCurrentSelection_LevelIndex = kIndex_Invalid;
			mCurrentSelection_ItemIndex = kIndex_Invalid;
			
			if( itemIndex != 0 )
			{
				//�ŐV���ڂֈړ�
				mCallerArray_FilePath.GetObject(0).MoveObject(itemIndex,0);
				mCallerArray_FunctionName.GetObject(0).MoveObject(itemIndex,0);
				mCallerArray_ClassName.GetObject(0).MoveObject(itemIndex,0);
				mCallerArray_StartIndex.GetObject(0).Move(itemIndex,0);
				mCallerArray_EndIndex.GetObject(0).Move(itemIndex,0);
				mCallerArray_ModeIndex.GetObject(0).Move(itemIndex,0);
				mCallerArray_CategoryIndex.GetObject(0).Move(itemIndex,0);
				mCallerArray_EdittedFlag.GetObject(0).Move(itemIndex,0);
				mCallerArray_ClickedFlag.GetObject(0).Move(itemIndex,0);
				mCallerArray_CallerFuncId.GetObject(0).MoveObject(itemIndex,0);
				//
				mLevel0FuncIdTextArray.MoveObject(itemIndex,0);
				//(0,0)�ɃX�N���[��
				AImagePoint	originpt = {0,0};
				NVI_ScrollTo(originpt);
				//
				itemIndex = 0;
			}
		}
		else
		{
			//------------------���x��0�ȊO�̏ꍇ�́A�I�����ڐݒ�------------------
			mCurrentSelection_LevelIndex = level;
			mCurrentSelection_ItemIndex = itemIndex;
			
			//�A�N�e�B�u�ȃT�u�E�C���h�E�Ƃ��Đݒ�icmd+option+�����̑ΏۂƂȂ�j
			GetApp().SPI_SetActiveSubWindowForItemSelector(NVM_GetWindow().GetObjectID());
		}
		
		//�elevel���̑I�����L��
		mCallerArray_CalleeItemIndex.Set(level,itemIndex);
		
		//�N���b�N���ڂ̃f�[�^���擾
		AText	filepath;
		mCallerArray_FilePath.GetObjectConst(level).Get(itemIndex,filepath);
		AText	classname;
		mCallerArray_ClassName.GetObjectConst(level).Get(itemIndex,classname);
		AText	funcname;
		mCallerArray_FunctionName.GetObjectConst(level).Get(itemIndex,funcname);
		AIndex	spos = mCallerArray_StartIndex.GetObjectConst(level).Get(itemIndex);
		AIndex	epos = mCallerArray_EndIndex.GetObjectConst(level).Get(itemIndex);
		AIndex	modeIndex = mCallerArray_ModeIndex.GetObjectConst(level).Get(itemIndex);
		//AIndex	categoryIndex = mCallerArray_CategoryIndex.GetObjectConst(level).Get(itemIndex);
		
		//FuncId�擾
		AText	funcIdText;
		SPI_GetFuncIdText(level,itemIndex,funcIdText);
		//�K��ς݂ɒǉ�
		AddToVisited(funcIdText);
		
		//==================���ڂ��J��==================
		//���ڂ��J��
		GetApp().SPI_OpenOrRevealTextDocument(filepath,spos,epos,kObjectID_Invalid,
					(AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true),
					kAdjustScrollType_Center);
		
		//==================�R�[�����\�����X�V==================
		UpdateArray(level+1,modeIndex,funcname);
		
	}
	//�`��X�V
	NVI_Refresh();
}

/**
�}�E�X�z�C�[��������
*/
void	AView_CallGraf::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
							const ALocalPoint inLocalPoint )
{
	//�X�N���[������ݒ肩��擾����
	ANumber	scrollPercent;
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true 
				|| AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd);
	}
	else
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent);
	}
	//�X�N���[�����s
	NVI_Scroll(0,inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100);
	//Hover�X�V
	EVTDO_DoMouseMoved(inLocalPoint,inModifierKeys);
}

/**
�}�E�X�z�o�[������
*/
ABool	AView_CallGraf::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�}�E�X�ʒu��image point�擾
	AImagePoint	imagept = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	
	//�z�o�[����level, item�擾
	AIndex	index = kIndex_Invalid;
	AIndex	level = kIndex_Invalid;
	FindBoxIndexByPoint(imagept,level,index);
	//�z�o�[���ڍX�V
	if( mCurrentHover_LevelIndex != level || mCurrentHover_ItemIndex != index )
	{
		mCurrentHover_LevelIndex = level;
		mCurrentHover_ItemIndex = index;
		NVI_Refresh();
	}
	//��v���鍀�ڂ�������΃z�o�[����
	else if( index == kIndex_Invalid )
	{
		ClearHoverCursor();
	}
	return true;
}

/**
�}�E�Xleave������
*/
void	AView_CallGraf::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearHoverCursor();
}

/**
�z�o�[����
*/
void	AView_CallGraf::ClearHoverCursor()
{
	mCurrentHover_LevelIndex = kIndex_Invalid;
	mCurrentHover_ItemIndex = kIndex_Invalid;
	NVI_Refresh();
}

/**
�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
*/
ABool	AView_CallGraf::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	/*
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	AApplication::GetApplication().NVI_GetMenuManager().
	ShowContextMenu(mContextMenuItemID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
	*/
	return true;
}

/**
�w���v�^�O
*/
ABool	AView_CallGraf::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	return false;
}

/**
�e�L�X�g���͎�����
*/
ABool	AView_CallGraf::EVTDO_DoTextInput( const AText& inText, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	return false;
}

/**
�e���ڂ�box rect���擾
*/
void	AView_CallGraf::GetBoxRect( const AIndex inColumnIndex, const AIndex inItemIndex, AImageRect& outRect ) const
{
	//
	outRect.left	= kLeftMargin_Box + inColumnIndex*(mBoxWidth + kRightMargin_Box + kCallerCalleeLineWidth);
	outRect.top		= kTopMargin_Box + inItemIndex*(kTopMargin_Box + mBoxHeight + kBottomMargin_Box);
	outRect.right	= outRect.left + mBoxWidth - kRightMargin_Box;
	outRect.bottom	= outRect.top + mBoxHeight;
}

/**
�N���b�N�ʒu���獀��index������
*/
void	AView_CallGraf::FindBoxIndexByPoint( const AImagePoint& inPoint, AIndex& outLevelIndex, AIndex& outItemIndex ) const
{
	outLevelIndex = kIndex_Invalid;
	outItemIndex = kIndex_Invalid;
	//�e�J�������̃��[�v
	for( AIndex columnIndex = 0; columnIndex <= 1; columnIndex++ )
	{
		//�J��������level���擾
		AIndex	level = GetLevelFromColumn(columnIndex);
		//�f�[�^���ݒ�level�Ȃ�continue
		if( level >= mCallerArray_FilePath.GetItemCount() )
		{
			continue;
		}
		//�e���ږ��̃��[�v
		for( AIndex index = 0; index < mCallerArray_FilePath.GetObjectConst(level).GetItemCount(); index++ )
		{
			//box rect����point������΁A�����Ԃ��B
			AImageRect	box = {0,0,0,0};
			GetBoxRect(columnIndex,index,box);
			if( inPoint.x >= box.left && inPoint.x <= box.right &&
						inPoint.y >= box.top && inPoint.y <= box.bottom )
			{
				outLevelIndex = level;
				outItemIndex = index;
				return;
			}
		}
	}
}

/**
���ڂ̃f�[�^���擾
*/
void	AView_CallGraf::GetItemData( const AIndex inLevelIndex, const AIndex inItemIndex,
		AText& outFilePath, AIndex& outStartIndex, AIndex& outEndIndex,
		AIndex& outModeIndex, AIndex& outCategoryIndex ) const
{
	outFilePath.DeleteAll();
	outStartIndex = kIndex_Invalid;
	outEndIndex = kIndex_Invalid;
	outModeIndex = kIndex_Invalid;
	outCategoryIndex = kIndex_Invalid;
	//�I�����ڂ̃f�[�^���擾
	mCallerArray_FilePath.GetObjectConst(inLevelIndex).Get(inItemIndex,outFilePath);
	outStartIndex = mCallerArray_StartIndex.GetObjectConst(inLevelIndex).
			Get(inItemIndex);
	outEndIndex = mCallerArray_EndIndex.GetObjectConst(inLevelIndex).
			Get(inItemIndex);
	outModeIndex = mCallerArray_ModeIndex.GetObjectConst(inLevelIndex).
			Get(inItemIndex);
	outCategoryIndex = mCallerArray_CategoryIndex.GetObjectConst(inLevelIndex).
			Get(inItemIndex);
}

/**
�}�E�X�J�[�\���ݒ�
*/
ACursorType	AView_CallGraf::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mCurrentHover_ItemIndex != kIndex_Invalid )
	{
		return kCursorType_PointingHand;
	}
	return kCursorType_Arrow;
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

/**
�e�L�X�g�ł̌��ݑI�����ꂽfuncion��ݒ�
@param inProjectFolder �����Ώۃt�H���_�i�v���W�F�N�g�t�H���_�łȂ��ꍇ�́A���̃t�H���_�����ċA�I�Ɍ����j
@param inStartIndex �֐��̒�`�J�n�ʒu
@param inEndIndex �֐��̒�`�I���ʒu
*/
void	AView_CallGraf::SPI_SetCurrentFunction( const AFileAcc& inProjectFolder, 
		const AIndex inModeIndex, const AIndex inCategoryIndex, 
		const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
		const AIndex inStartIndex, const AIndex inEndIndex, const ABool inByEdit )
{
	//Id Info�N���b�N�ɂ��W�����v�̏ꍇ�ɂ́AcallerFuncId�Ɉړ����֐���func id������
	//�iId Info�N���b�N���ɁASPI_NotifyToCallGrafByIdInfoJump()�ɂāA�ړ���֐����L������Ă���̂ŁA
	// ��v����΁A���̂Ƃ��ɋL�������ړ����֐���callerFuncId�Ɋi�[����j
	AText	callerFuncId;
	if( mIdInfoJumpMemory_FilePath.Compare(inFilePath) == true &&
				mIdInfoJumpMemory_ClassName.Compare(inClassName) == true &&
				mIdInfoJumpMemory_FunctionName.Compare(inFunctionName) == true )
	{
		callerFuncId.SetText(mIdInfoJumpMemory_CallerFuncId);
	}
	
	//�����funcID���擾
	AText	funcIdText;
	AView_CallGraf::SPI_GetFuncIdText(inFilePath,inClassName,inFunctionName,funcIdText);
	
	//�ҏW�t���O���ύX����Ă���΁A�ҏW�t���OON��ݒ�
	AIndex	foundIndexInLevel0 = mLevel0FuncIdTextArray.Find(funcIdText);
	if( inByEdit == true && foundIndexInLevel0 != kIndex_Invalid )
	{
		if( mCallerArray_EdittedFlag.GetObject(0).Get(foundIndexInLevel0) == false )
		{
			mCallerArray_EdittedFlag.GetObject(0).Set(foundIndexInLevel0,true);
			NVI_Refresh();
		}
	}
	
	//==================�����X�V���邩�ǂ����̔���==================
	
	//�����̍Ō�̍��ځi��level=0, item index=0�j�Ɠ����֐��Ȃ牽���������^�[��
	AText	filepath;
	AIndex	spos = 0, epos = 0;
	AIndex	modeIndex = kIndex_Invalid, categoryIndex = kIndex_Invalid;
	if( mCallerArray_FilePath.GetObjectConst(0).GetItemCount() > 0 )
	{
		GetItemData(0,0,filepath,spos,epos,modeIndex,categoryIndex);
		//�t�@�C���p�X�ƊJ�n�ʒu�������Ȃ瓯���֐��Ɣ��f�i�I���ʒu�̓`�F�b�N���Ȃ��B����ɂ��֐����̕�����ҏW���Ă���Ƃ��ɗ������ǂ�ǂ�ǉ�����邱�Ƃ�h�~�j
		if( filepath.Compare(inFilePath) == true  && spos == inStartIndex )
		{
			return;
		}
	}
	
	//�Ō�ɐݒ肵���v���f�[�^���L���i���b�N�������ɍēx�v�����邽�߂Ɏg�p����j
	mLastSetCurrentFunction_ProjectFolder = inProjectFolder;
	mLastSetCurrentFunction_ModeIndex = inModeIndex;
	mLastSetCurrentFunction_CategoryIndex = inCategoryIndex;
	mLastSetCurrentFunction_FilePath.SetText(inFilePath);
	mLastSetCurrentFunction_ClassName.SetText(inClassName);
	mLastSetCurrentFunction_FunctionName.SetText(inFunctionName);
	mLastSetCurrentFunction_StartIndex = inStartIndex;
	mLastSetCurrentFunction_EndIndex = inEndIndex;
	mLastSetCurrentFunction_ByEdit = inByEdit;
	
	//���݂̑I�����ڂ�level1�ȏ�̏ꍇ�i�����b�N���j�͉����������^�[���i����I���I�v�V�����iATypes.h�ɂĐݒ�jON�̏ꍇ�̂݁j
	if( kOption_CallGraf_DontUpdateHistoryWhenItemSelected == true )
	{
		if( mCurrentSelection_LevelIndex > 0 )
		{
			return;
		}
	}
	
	//�����i���x��0�j�}���ʒu
	AIndex	historyItemInsertIndex = 0;
	
	//����I�v�V�������A���b�N���A�ŏ��̗������ڂ������b�N����Ƃ����I�v�V�����ɂȂ��Ă���ꍇ�A
	//index:1�̈ʒu�ɑ}������
	if( kOption_CallGraf_FixFirstHistoryItem == true )
	{
		if( mCurrentSelection_LevelIndex > 0 )
		{
			if( mCallerArray_FilePath.GetObject(0).GetItemCount() > 0 )
			{
				historyItemInsertIndex++;
			}
		}
	}
	
	//���݂̑I�����x�����ɓ������ڂ�����Ή��������Ƀ��^�[��
	if( inByEdit == false && mCurrentSelection_LevelIndex != kIndex_Invalid )
	{
		AItemCount	itemCount = mCallerArray_FilePath.GetObjectConst(mCurrentSelection_LevelIndex).GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			AText	currentFuncIdText;
			SPI_GetFuncIdText(mCurrentSelection_LevelIndex,i,currentFuncIdText);
			if( funcIdText.Compare(currentFuncIdText) == true )
			{
				return;
			}
		}
	}
	
	/*
	//���݂̑I�����ڂƓ����Ȃ牽�����Ȃ�
	if( mCurrentSelection_LevelIndex != kIndex_Invalid && mCurrentSelection_ItemIndex != kIndex_Invalid )
	{
		AText	currentFuncIdText;
		SPI_GetFuncIdText(mCurrentSelection_LevelIndex,mCurrentSelection_ItemIndex,currentFuncIdText);
		if( funcIdText.Compare(currentFuncIdText) == true )
		{
			return;
		}
	}
	*/
	
	//==================�����X�V==================
	
	//���݂�projectfolder, mode index, category index��ݒ�
	mCurrentProjectFolder.CopyFrom(inProjectFolder);
	
	//level0���̓������e��index���������A���݂��Ă����炻�̍��ڂ͍폜�ispos, epos�f�[�^���X�V���Ă��������̂ŁA�ړ��ł͂Ȃ��A�폜�E�ǉ��ɂ���j
	ABool	edited = false;
	if( foundIndexInLevel0 != kIndex_Invalid )
	{
		//------------------�폜------------------
		//�ҏW�ς݃t���O�L��
		edited = mCallerArray_EdittedFlag.GetObject(0).Get(foundIndexInLevel0);
		//���ڍ폜
		mCallerArray_FilePath.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_FunctionName.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_ClassName.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_StartIndex.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_EndIndex.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_ModeIndex.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_CategoryIndex.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_EdittedFlag.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_ClickedFlag.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_CallerFuncId.GetObject(0).Delete1(foundIndexInLevel0);
		//
		mLevel0FuncIdTextArray.Delete1(foundIndexInLevel0);
	}
	//------------------�����̍ŏ��ɒǉ�------------------
	mCallerArray_FilePath.GetObject(0).Insert1(historyItemInsertIndex,inFilePath);
	mCallerArray_FunctionName.GetObject(0).Insert1(historyItemInsertIndex,inFunctionName);
	mCallerArray_ClassName.GetObject(0).Insert1(historyItemInsertIndex,inClassName);
	mCallerArray_StartIndex.GetObject(0).Insert1(historyItemInsertIndex,inStartIndex);
	mCallerArray_EndIndex.GetObject(0).Insert1(historyItemInsertIndex,inEndIndex);
	mCallerArray_ModeIndex.GetObject(0).Insert1(historyItemInsertIndex,inModeIndex);
	mCallerArray_CategoryIndex.GetObject(0).Insert1(historyItemInsertIndex,inCategoryIndex);
	mCallerArray_EdittedFlag.GetObject(0).Insert1(historyItemInsertIndex,(inByEdit||edited));
	mCallerArray_ClickedFlag.GetObject(0).Insert1(historyItemInsertIndex,false);
	mCallerArray_CallerFuncId.GetObject(0).Insert1(historyItemInsertIndex,callerFuncId);
	//
	mLevel0FuncIdTextArray.Insert1(historyItemInsertIndex,funcIdText);
	
	//level0��callee item index�i�����̂Ȃ����Ă���Ƃ���j���X�V����
	//�i�������A��I����ԂȂ�A���̉��̕����ŁA�R�[�����\���͎����X�V�����̂ŁAcallee item index��0�ɍX�V�����B�j
	if( historyItemInsertIndex == 0 )//���b�N���ŁA����2�Ԗڂɍ��ڒǉ������ꍇ�́Acallee item�͍���1�ԖڂȂ̂ŕω����Ȃ��B
	{
		AIndex	origCurrentIndex = mCallerArray_CalleeItemIndex.Get(0);
		if( origCurrentIndex != kIndex_Invalid )
		{
			if( foundIndexInLevel0 == kIndex_Invalid )
			{
				//������V�K�ǉ������ꍇ
				//callee item index��+1
				mCallerArray_CalleeItemIndex.Set(0,origCurrentIndex+1);
			}
			else if( foundIndexInLevel0 == origCurrentIndex )
			{
				//�������ړ����A���A���̍��ڂ�callee item�̏ꍇ
				//callee item index��0
				mCallerArray_CalleeItemIndex.Set(0,0);
			}
			else if( foundIndexInLevel0 > origCurrentIndex )
			{
				//�������ړ����A���A���̍��ڂ�callee item���ゾ�����ꍇ
				//callee item index��+1
				mCallerArray_CalleeItemIndex.Set(0,origCurrentIndex+1);
			}
		}
	}
	
	//�������I�[�o�[�Ȃ�Ō�̍��ڂ��폜
	AItemCount	historyLimitCount = kLimit_CallGrafLevel0ItemCount;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )
	{
		historyLimitCount = 1;
	}
	while( mCallerArray_FilePath.GetObject(0).GetItemCount() > historyLimitCount )
	{
		mCallerArray_FilePath.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_FunctionName.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_ClassName.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_StartIndex.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_EndIndex.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_ModeIndex.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_CategoryIndex.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_EdittedFlag.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_ClickedFlag.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_CallerFuncId.GetObject(0).Delete1(historyLimitCount);
		//
		mLevel0FuncIdTextArray.Delete1(historyLimitCount);
	}
	
	//�K��ς݂ɒǉ�
	AddToVisited(funcIdText);
	
	//��I����Ԃ̏ꍇ�Ɍ���A�R�[�����\�����A�����I�ɗ����ŐV�̃R�[�����\���ɍX�V����B
	if( mCurrentSelection_LevelIndex == kIndex_Invalid || mCurrentSelection_ItemIndex == kIndex_Invalid )
	{
		UpdateCallerArrayForTheLatestHistory();
	}
	//Image size�X�V
	UpdateImageSize();
	
	//�`��X�V
	NVI_Refresh();
}

/**
�ҏW�L��t���O��ݒ�i���ɒǉ��ς݂̗������ڂ��猟�����āA���݂��Ă���΂��̍��ڂ̕ҏW�t���O��ON�ɂ���j
*/
void	AView_CallGraf::SPI_SetEditFlag( const AText& inFilePath, const AText& inClassName, const AText& inFunctionName )
{
	//�����funcID���擾
	AText	funcIdText;
	AView_CallGraf::SPI_GetFuncIdText(inFilePath,inClassName,inFunctionName,funcIdText);
	
	//�ҏW�t���O���ύX����Ă���΁A�ҏW�t���OON��ݒ�
	AIndex	foundIndexInLevel0 = mLevel0FuncIdTextArray.Find(funcIdText);
	if( foundIndexInLevel0 != kIndex_Invalid )
	{
		if( mCallerArray_EdittedFlag.GetObject(0).Get(foundIndexInLevel0) == false )
		{
			//�ҏW�t���OON
			mCallerArray_EdittedFlag.GetObject(0).Set(foundIndexInLevel0,true);
			//�`��X�V
			NVI_Refresh();
		}
	}
}

/**
�����ŐV���ڂɂ���caller�\������
*/
void	AView_CallGraf::UpdateCallerArrayForTheLatestHistory()
{
	//���x��0�̌��ݍ��ڂ��ŐV�̍��ڂɐݒ肷��
	mCallerArray_CalleeItemIndex.Set(0,0);
	
	//(0,0)�ɃX�N���[��
	AImagePoint	originpt = {0,0};
	NVI_ScrollTo(originpt);
	
	//�ŐV�����̍��ڂ̃f�[�^���擾
	AIndex	modeIndex = mCallerArray_ModeIndex.GetObjectConst(0).Get(0);
	AText	functionname;
	mCallerArray_FunctionName.GetObjectConst(0).Get(0,functionname);
	//���x��1��array���X�V�i���mCallerArray_CalleeItemIndex��0��ݒ肵�Ă���̂ŁA�����ŐV����R�[�����\�������B�j
	UpdateArray(1,modeIndex,functionname);
}

/**
�w�背�x����array���X�V�i�w��function���g�p����ꏊ�̈ꗗ���쐬�j
*/
void	AView_CallGraf::UpdateArray( const AIndex inLevelIndex, const AIndex inModeIndex, const AText& inFunctionName )
{
	//�w�背�x����array�����݂��Ă��Ȃ���΁Aarray�𐶐�
	while( inLevelIndex >= mCallerArray_FilePath.GetItemCount() )
	{
		//�earray
		mCallerArray_FilePath.AddNewObject();
		mCallerArray_FunctionName.AddNewObject();
		mCallerArray_ClassName.AddNewObject();
		mCallerArray_StartIndex.AddNewObject();
		mCallerArray_EndIndex.AddNewObject();
		mCallerArray_ModeIndex.AddNewObject();
		mCallerArray_CategoryIndex.AddNewObject();
		mCallerArray_EdittedFlag.AddNewObject();
		mCallerArray_ClickedFlag.AddNewObject();
		mCallerArray_CallerFuncId.AddNewObject();
		//array���I��index
		mCallerArray_CalleeItemIndex.Add(0);
		//array�̌��ݒP��
		mCurrentWordArray.Add(GetEmptyText());
	}
	//�w��level�ȏ�̑S���ڂ��폜
	for( AIndex i = inLevelIndex; i < mCallerArray_FilePath.GetItemCount(); i++ )
	{
		mCallerArray_FilePath.GetObject(i).DeleteAll();
		mCallerArray_FunctionName.GetObject(i).DeleteAll();
		mCallerArray_ClassName.GetObject(i).DeleteAll();
		mCallerArray_StartIndex.GetObject(i).DeleteAll();
		mCallerArray_EndIndex.GetObject(i).DeleteAll();
		mCallerArray_ModeIndex.GetObject(i).DeleteAll();
		mCallerArray_CategoryIndex.GetObject(i).DeleteAll();
		mCallerArray_EdittedFlag.GetObject(i).DeleteAll();
		mCallerArray_ClickedFlag.GetObject(i).DeleteAll();
		mCallerArray_CallerFuncId.GetObject(i).DeleteAll();
		//�Ă΂��index���N���A
		mCallerArray_CalleeItemIndex.Set(i,kIndex_Invalid);
		//
		mCurrentWordArray.Set(i,GetEmptyText());
	}
	//array�̌��ݒP��ݒ�
	mCurrentWordArray.Set(inLevelIndex-1,inFunctionName);
	
	//==================�X���b�h�Ƀ��[�Y���X�g�����v��==================
	{
		//���[�h���X�g�X���b�h�Ɍ����v��
		//mCurrentProjectFolder���v���W�F�N�g�t�H���_�Ŗ����ꍇ�A�X���b�h���i�Ƃ�����SPI_GetImportFileDataIDArrayForWordsList()�ɂāA
		//�����t�H���_��ΏۂɌ������s���B
		GetWordsListFinderThread().SetRequestDataForCallGraf(mCurrentProjectFolder,
					inModeIndex,NVM_GetWindowConst().GetObjectID(),inFunctionName,inLevelIndex);
		//�X���b�hunpause
		GetWordsListFinderThread().NVI_UnpauseIfPaused();
		//�������t���OON�A�E�C���h�E�`��X�V
		mIsFinding = true;
		NVM_GetWindow().NVI_UpdateProperty();
	}
	
	//�w�b�_�r���[�X�V
	UpdateHeaderView();
}

/**
�������f
*/
void	AView_CallGraf::SPI_AbortCurrentWordsListFinderRequest()
{
	GetWordsListFinderThread().AbortCurrentRequest();
}

/**
�f�[�^�S�폜
*/
void	AView_CallGraf::SPI_DeleteAllData()
{
	//���݂̌����𒆒f����
	SPI_AbortCurrentWordsListFinderRequest();
	
	//�������t���OOFF
	mIsFinding = false;
	//���݂̑I���E�z�o�[���N���A
	mCurrentSelection_LevelIndex = kIndex_Invalid;
	mCurrentSelection_ItemIndex = kIndex_Invalid;
	mCurrentHover_LevelIndex = kIndex_Invalid;
	mCurrentHover_ItemIndex = kIndex_Invalid;
	//�\���J�n�J������1�ɐݒ�
	mCurrentDisplayStartLevel = 1;
	//���݂̃v���W�F�N�g�t�H���_�N���A
	mCurrentProjectFolder.Unspecify();
	//���x��1�ȏ�̃f�[�^���폜�i���x��0�͗����Ȃ̂Łi�J�e�S���f�[�^�ȊO�j�N���A���Ȃ��B�j
	AItemCount	levelCount = mCallerArray_FilePath.GetItemCount();
	for( AIndex i = 1; i < levelCount; i++ )
	{
		//���ڃN���A
		mCallerArray_FilePath.GetObject(i).DeleteAll();
		mCallerArray_FunctionName.GetObject(i).DeleteAll();
		mCallerArray_ClassName.GetObject(i).DeleteAll();
		mCallerArray_StartIndex.GetObject(i).DeleteAll();
		mCallerArray_EndIndex.GetObject(i).DeleteAll();
		mCallerArray_ModeIndex.GetObject(i).DeleteAll();
		mCallerArray_CategoryIndex.GetObject(i).DeleteAll();
		mCallerArray_EdittedFlag.GetObject(i).DeleteAll();
		mCallerArray_ClickedFlag.GetObject(i).DeleteAll();
		mCallerArray_CallerFuncId.GetObject(i).DeleteAll();
		//callee�N���A
		mCallerArray_CalleeItemIndex.Set(i,kIndex_Invalid);
		//���ݒP��N���A
		mCurrentWordArray.Set(i,GetEmptyText());
	}
	//�S�����ς݃t���O�N���A
	mAllFilesSearched = false;
	//�K��ς�func id�N���A
	mVisitedFuncIdTextArray.DeleteAll();
	
	//mLevel0FuncIdTextArray�̓N���A���Ȃ��B�i���x��0�Ȃ̂Łj
	
	//���x��0�̃J�e�S��index���N���A
	AItemCount	itemCount = mCallerArray_CategoryIndex.GetObjectConst(0).GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		mCallerArray_CategoryIndex.GetObject(0).Set(i,kIndex_Invalid);
	}
	
	//id info����̒ʒm�L�^�N���A
	mIdInfoJumpMemory_FilePath.DeleteAll();
	mIdInfoJumpMemory_ClassName.DeleteAll();
	mIdInfoJumpMemory_FunctionName.DeleteAll();
	mIdInfoJumpMemory_CallerFuncId.DeleteAll();
	
	//Image size�X�V
	UpdateImageSize();
}

/**
�X���b�h�̃��[�Y���X�g�������ʔ��f
*/
void	AView_CallGraf::SPI_DoWordsListFinderPaused()
{
	//���x��0�̃f�[�^���Ȃ��Ȃ牽���������^�[��
	if( mCallerArray_FilePath.GetObjectConst(0).GetItemCount() == 0 )
	{
		return;
	}
	
	//�������ʎ擾
	AWindowID	callGrafWindowID;
	AIndex	requestLevelIndex = kIndex_Invalid;
	ATextArray	filePathArray;
	ATextArray	classNameArray;
	ATextArray	functionNameArray;
	AArray<AIndex>	startArray;
	AArray<AIndex>	endArray;
	ABool	allFilesSearched = GetWordsListFinderThread().GetResultForCallGraf(callGrafWindowID,requestLevelIndex,filePathArray,classNameArray,functionNameArray,startArray,endArray);
	//�R�[���捀�ڂ̃f�[�^�擾
	AText	currentSelectedItem_FilePath;
	AIndex	currentSelectedItem_StartIndex = 0;
	AIndex	currentSelectedItem_EndIndex = 0;
	AIndex	currentSelectedItem_ModeIndex = kIndex_Invalid;
	AIndex	currentSelectedItem_CategoryIndex = kIndex_Invalid;
	//�R�[���捀�ځi���v�����x���̂P�E�̃��x���̌��ݍ��ځj�̃f�[�^�擾
	GetItemData(requestLevelIndex-1,mCallerArray_CalleeItemIndex.Get(requestLevelIndex-1),
				currentSelectedItem_FilePath,currentSelectedItem_StartIndex,currentSelectedItem_EndIndex,
				currentSelectedItem_ModeIndex,currentSelectedItem_CategoryIndex);
	//�X���b�h�������ʂ�ǉ�
	for( AIndex i = 0; i < filePathArray.GetItemCount(); i++ )
	{
		//�X���b�h�������ʃf�[�^�擾
		AText	filepath;
		filePathArray.Get(i,filepath);
		AIndex	spos = startArray.Get(i);
		AIndex	epos = endArray.Get(i);
		//���݂̑I�����ڂ̃f�[�^�Ɠ������ڂ́Aarray�ɒǉ����Ȃ�
		if( filepath.Compare(currentSelectedItem_FilePath) == true &&
					spos == currentSelectedItem_StartIndex &&
					epos == currentSelectedItem_EndIndex )
		{
			continue;
		}
		//array�ɒǉ�
		mCallerArray_FilePath.GetObject(requestLevelIndex).Add(filepath);
		mCallerArray_ClassName.GetObject(requestLevelIndex).Add(classNameArray.GetTextConst(i));
		mCallerArray_FunctionName.GetObject(requestLevelIndex).Add(functionNameArray.GetTextConst(i));
		mCallerArray_StartIndex.GetObject(requestLevelIndex).Add(spos);
		mCallerArray_EndIndex.GetObject(requestLevelIndex).Add(epos);
		mCallerArray_ModeIndex.GetObject(requestLevelIndex).Add(currentSelectedItem_ModeIndex);
		mCallerArray_CategoryIndex.GetObject(requestLevelIndex).Add(currentSelectedItem_CategoryIndex);
		mCallerArray_EdittedFlag.GetObject(requestLevelIndex).Add(false);
		mCallerArray_ClickedFlag.GetObject(requestLevelIndex).Add(false);
		mCallerArray_CallerFuncId.GetObject(requestLevelIndex).Add(GetEmptyText());
	}
	//���������ڂ��Ȃ����ǂ����̃t���O�iimportFileData�������́Afalse�ɂȂ�j
	mAllFilesSearched = allFilesSearched;
	//Image size�X�V
	UpdateImageSize();
	//�������t���OOFF�A�v���O���X�\���X�V
	mIsFinding = false;
	NVM_GetWindow().NVI_UpdateProperty();
	//�`��X�V
	NVI_Refresh();
}

/**
IdInfo�N���b�N�ɂ��W�����v���ɃR�[�������
*/
void	AView_CallGraf::SPI_NotifyToCallGrafByIdInfoJump( const AText& inCallerFuncIdText, 
		const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
		const AIndex inStartIndex, const AIndex inEndIndex )
{
	mIdInfoJumpMemory_FilePath.SetText(inFilePath);
	mIdInfoJumpMemory_ClassName.SetText(inClassName);
	mIdInfoJumpMemory_FunctionName.SetText(inFunctionName);
	mIdInfoJumpMemory_CallerFuncId.SetText(inCallerFuncIdText);
}

/**
�h�L�������g�ҏW���ɃR�[�������
*/
void	AView_CallGraf::SPI_NotifyToCallGrafByTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	//�X�V�t�@�C�����擾
	AText	filepath;
	inFile.GetPath(filepath);
	//�e���x�����̃��[�v
	AItemCount	levelCount = mCallerArray_FilePath.GetItemCount();
	for( AIndex level = 0; level < levelCount; level++ )
	{
		//�X�V�t�@�C���Ɉ�v���鍀�ڂ�index���擾
		AArray<AIndex>	indexArray;
		mCallerArray_FilePath.GetObjectConst(level).FindAll(filepath,0,filepath.GetItemCount(),indexArray);
		//�e���ږ��̃��[�v
		AItemCount	itemCount = indexArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			AIndex	index = indexArray.Get(i);
			//start index���X�V�ʒu�ȍ~�̏ꍇ�́Astart index, end index���X�V
			AIndex	spos = mCallerArray_StartIndex.GetObjectConst(level).Get(index);
			if( spos > inTextIndex )
			{
				mCallerArray_StartIndex.GetObject(level).Set(index,spos+inInsertedCount);
				mCallerArray_EndIndex.GetObject(level).Set(index,
							mCallerArray_EndIndex.GetObjectConst(level).Get(index)+inInsertedCount);
			}
		}
	}
}

/**
�w�b�_�[�r���[�̕`��X�V
*/
void	AView_CallGraf::UpdateHeaderView()
{
	//�w�b�_�r���[�̕`��X�V
	GetApp().SPI_GetCallGrafWindow(NVM_GetWindow().GetObjectID()).SPI_GetCallGrafHeaderView().
			SPI_SetCurrentStatus(mCurrentDisplayStartLevel,mCurrentWordArray);
}

/**
���ݍ��ڐݒ�
*/
/*
void	AView_CallGraf::SetSelect( const AIndex inLevel, const AIndex inItemIndex )
{
	//���݂̑I���ʒu���L��
	mCurrentSelection_LevelIndex = inLevel;
	mCurrentSelection_ItemIndex = inItemIndex;
	
	//�elevel���̑I�����L��
	mCallerArray_CalleeItemIndex.Set(inLevel,inItemIndex);
}
*/

/**
�z�[���ֈړ�����L�[���N���b�N������
*/
void	AView_CallGraf::SPI_ReturnHome()
{
	//�I������
	mCurrentSelection_LevelIndex = kIndex_Invalid;
	mCurrentSelection_ItemIndex = kIndex_Invalid;
	//�\���J����������
	mCurrentDisplayStartLevel = 1;
	//�����ŐV���ڂɂ��ăR�[�����\��
	UpdateCallerArrayForTheLatestHistory();
	//�C���[�W�T�C�Y�X�V
	UpdateImageSize();
	//�`��X�V
	NVI_Refresh();
	//�w�b�_�r���[�X�V
	UpdateHeaderView();
	//�Ō�̗v���f�[�^�i�����b�N���Ȃ̂Ŏ��s����Ȃ������j���A�v������
	if( kOption_CallGraf_DontUpdateHistoryWhenItemSelected == true )
	{
		AFileAcc	projectFolder = mLastSetCurrentFunction_ProjectFolder;
		AIndex	modeIndex = mLastSetCurrentFunction_ModeIndex;
		AIndex	categoryIndex = mLastSetCurrentFunction_CategoryIndex;
		AText	filePath = mLastSetCurrentFunction_FilePath;
		AText	className = mLastSetCurrentFunction_ClassName;
		AText	functionName = mLastSetCurrentFunction_FunctionName;
		AIndex	startIndex = mLastSetCurrentFunction_StartIndex;
		AIndex	endIndex = mLastSetCurrentFunction_EndIndex;
		ABool	byEdit = mLastSetCurrentFunction_ByEdit;
		SPI_SetCurrentFunction(projectFolder,modeIndex,categoryIndex,filePath,className,functionName,startIndex,endIndex,byEdit);
	}
}

/**
�����x���X�N���[��
*/
void	AView_CallGraf::SPI_GoLeftLevel()
{
	//�J�������x���C���N�������g
	mCurrentDisplayStartLevel++;
	//�C���[�W�T�C�Y�X�V
	UpdateImageSize();
	//�`��X�V
	NVI_Refresh();
	//�w�b�_�r���[�X�V
	UpdateHeaderView();
}

/**
�E���x���X�N���[��
*/
void	AView_CallGraf::SPI_GoRightLevel()
{
	if( mCurrentDisplayStartLevel >= 2 )
	{
		//�J�������x���f�N�������g
		mCurrentDisplayStartLevel--;
		//�C���[�W�T�C�Y�X�V
		UpdateImageSize();
		//�`��X�V
		NVI_Refresh();
		//�w�b�_�r���[�X�V
		UpdateHeaderView();
	}
}

/**
�K��ς݂ɒǉ�
*/
void	AView_CallGraf::AddToVisited( const AText& inFuncIdText )
{
	if( mVisitedFuncIdTextArray.Find(inFuncIdText) == kIndex_Invalid )
	{
		mVisitedFuncIdTextArray.Add(inFuncIdText);
	}
}

/**
�C���[�W�T�C�Y�X�V
*/
void	AView_CallGraf::UpdateImageSize()
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	//image����
	ANumber	imageHeight = 0;
	//�e�J���������[�v
	for( AIndex columnIndex = 0; columnIndex <= 1; columnIndex++ )
	{
		//�J��������level�擾
		AIndex	level = GetLevelFromColumn(columnIndex);
		
		//�Ώ�level�̃f�[�^�������Ȃ牽�����Ȃ�
		if( level >= mCallerArray_FilePath.GetItemCount() )   continue;
		
		//���ڐ��擾
		AItemCount	itemCount = mCallerArray_FilePath.GetObjectConst(level).GetItemCount();
		if( itemCount == 0 )   continue;
		
		//�Ō�̍��ڂ�box rect�擾
		AImageRect	box = {0};
		GetBoxRect(columnIndex,itemCount-1,box);
		//image�����X�V
		ANumber	columnImageHeight = box.bottom + kBottomMargin_Box;
		if( imageHeight < columnImageHeight )
		{
			imageHeight = columnImageHeight;
		}
	}
	
	//�C���[�W�T�C�Y�ݒ�
	NVM_SetImageSize(frameRect.right - frameRect.left,imageHeight);
	
	//scroll�ݒ�
	ALocalRect	rect = {0};
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(16,16,rect.right-rect.left-16,rect.bottom-rect.top-16);
}

/**
FuncId�擾
*/
void	AView_CallGraf::SPI_GetFuncIdText( const AIndex inLevel, const AIndex inItemIndex, AText& outFuncIdText ) const
{
	AText	filepath;
	mCallerArray_FilePath.GetObjectConst(inLevel).Get(inItemIndex,filepath);
	AText	classname;
	mCallerArray_ClassName.GetObjectConst(inLevel).Get(inItemIndex,classname);
	AText	funcname;
	mCallerArray_FunctionName.GetObjectConst(inLevel).Get(inItemIndex,funcname);
	//FuncId�擾
	AView_CallGraf::SPI_GetFuncIdText(filepath,classname,funcname,outFuncIdText);
}

/**
FuncId�擾
*/
void	AView_CallGraf::SPI_GetFuncIdText( const AText& inFilePath, const AText& inClassName, const AText& inFuncName,
		AText& outFuncIdText )
{
	outFuncIdText.SetText(inFilePath);
	outFuncIdText.Add(kUChar_Tab);
	outFuncIdText.AddText(inClassName);
	outFuncIdText.Add(kUChar_Tab);
	outFuncIdText.AddText(inFuncName);
}

/**
���̍��ڂ�I��
*/
void	AView_CallGraf::SPI_SelectNextItem()
{
	if( mCurrentSelection_LevelIndex != kIndex_Invalid )
	{
		if( mCurrentSelection_ItemIndex >= 0 && mCurrentSelection_ItemIndex+1 < mCallerArray_FilePath.GetObject(mCurrentSelection_LevelIndex).GetItemCount() )
		{
			//���ݑI�����ڃC���f�b�N�X���C���N�������g
			mCurrentSelection_ItemIndex++;
			//���ڂ��J��
			SPI_OpenItem(mCurrentSelection_LevelIndex,mCurrentSelection_ItemIndex,0);
			//�X�N���[������
			SPI_AdjustScroll();
		}
	}
}

/**
�O�̍��ڂ�I��
*/
void	AView_CallGraf::SPI_SelectPreviousItem()
{
	if( mCurrentSelection_LevelIndex != kIndex_Invalid )
	{
		if( mCurrentSelection_ItemIndex-1 >= 0 && mCurrentSelection_ItemIndex < mCallerArray_FilePath.GetObject(mCurrentSelection_LevelIndex).GetItemCount() )
		{
			//���ݑI�����ڃC���f�b�N�X���f�N�������g
			mCurrentSelection_ItemIndex--;
			//���ڂ��J��
			SPI_OpenItem(mCurrentSelection_LevelIndex,mCurrentSelection_ItemIndex,0);
			//�X�N���[������
			SPI_AdjustScroll();
		}
	}
}

/**
�X�N���[������
*/
void	AView_CallGraf::SPI_AdjustScroll()
{
	if( mCurrentSelection_LevelIndex != kIndex_Invalid && mCurrentSelection_ItemIndex != kIndex_Invalid )
	{
		//���ڂ�rect���擾
		AImageRect	box = {0};
		GetBoxRect(mCurrentSelection_LevelIndex,mCurrentSelection_ItemIndex,box);
		//�t���[����image rect���擾
		AImageRect	imageFrameRect = {0};
		NVM_GetImageViewRect(imageFrameRect);
		//frame�������ɂ���Ƃ�
		if( box.bottom > imageFrameRect.bottom )
		{
			AImagePoint	imagept = {imageFrameRect.left,box.top-(imageFrameRect.bottom-imageFrameRect.top)/2};
			if( imagept.y < 0 )   imagept.y = 0;
			NVI_ScrollTo(imagept);
		}
		//frame������ɂ���Ƃ�
		if( box.top < imageFrameRect.top )
		{
			AImagePoint	imagept = {imageFrameRect.left,box.top-(imageFrameRect.bottom-imageFrameRect.top)/2};
			if( imagept.y < 0 )   imagept.y = 0;
			NVI_ScrollTo(imagept);
		}
	}
}


