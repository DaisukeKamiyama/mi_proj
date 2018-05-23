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

AView_Notification
#725

*/

#include "stdafx.h"

#include "AView_Notification.h"
#include "AApp.h"

//�e����
const ANumber	kHeight_ItemBox = 38;
const ANumber	kHeight_Header = 18;
const ANumber	kHeight_ItemMargin = 4;
const ANumber	kHeight_ItemBoxWithMargin = kHeight_ItemBox + kHeight_ItemMargin;

//�w�b�_�[�����̏㉺���E�}�[�W��
const ANumber	kLeftMargin_HeaderText = 12;
const ANumber	kRightMargin_HeaderText = 12;
const ANumber	kTopMargin_HeaderText = 2;
const ANumber	kBottomMargin_HeaderText = 2;
//���e�����̏㉺���E�}�[�W��
const ANumber	kLeftMargin_ContentText = 14;
const ANumber	kRightMargin_ContentText = 14;
const ANumber	kTopMargin_ContentText = 2;
const ANumber	kBottomMargin_ContentText = 2;

//�p�ۂ̔��a #1079
const AFloatNumber	kRoundedRectRad = 3.0f;

#pragma mark ===========================
#pragma mark [�N���X]AView_Notification
#pragma mark ===========================
/*
�e�탁�b�Z�[�W�\���p�|�b�v�A�b�v
�etext view����AWindow_Notification�|�b�v�A�b�v�E�C���h�E�������A���̃E�C���h�E����AView_Notification������
*/

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_Notification::AView_Notification( const AWindowID inWindowID, const AControlID inID ) : AView(inWindowID,inID)
{
	NVMC_SetOffscreenMode(true);//win
}

//�f�X�g���N�^
AView_Notification::~AView_Notification()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_Notification::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_Notification::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_Notification::EVTDO_DoDraw()
{
	//�t�H���g�擾
	//�w�b�_�����t�H���g
	AText	labelfontname;
	AFontWrapper::GetDialogDefaultFontName(labelfontname);
	
	//frame rect�擾
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	//�\��alpha�擾
	//AFloatNumber	backgroundAlpha = GetApp().SPI_GetPopupWindowAlpha();
	
	//view�S�̏���
	NVMC_EraseRect(frameRect);
	
	//==================�F�擾==================
	
	//�`��F�ݒ�
	AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
	boxBaseColor1 = AColorWrapper::ChangeHSV(kColor_White,0,1.0,1.2);
	boxBaseColor2 = AColorWrapper::ChangeHSV(kColor_White,0,1.0,0.95);
	boxBaseColor3 = AColorWrapper::ChangeHSV(kColor_White,0,1.0,0.93);
	
	//�e���ږ��̃��[�v
	for( AIndex index = 0; index < mArray_Text.GetItemCount(); index++ )
	{
		//���ڂ�rect�擾
		ALocalRect	itemLocalRect = {0};
		GetItemBox(index,itemLocalRect);
		
		//���ږ��̃��l�擾
		AFloatNumber	alpha = mArray_Alpha.Get(index);
		
		//���ږ��̐F�擾
		AColor	color = mArray_Color.Get(index);
		
		//==================�S��rounded rect�`��==================
		//round rect�`��
		NVMC_PaintRoundedRect(itemLocalRect,
							  //kColor_White,kColor_Gray95Percent,
							  boxBaseColor1,boxBaseColor3,
							  kGradientType_Vertical,alpha,alpha,
							  kRoundedRectRad,true,true,true,true);
		
		//==================�w�b�_�`��==================
		//�t�H���g�ݒ�
		NVMC_SetDefaultTextProperty(labelfontname,9.5,kColor_Black,kTextStyle_DropShadow,true,alpha);
		//�w�b�_rect�擾
		ALocalRect	headerLocalRect = itemLocalRect;
		headerLocalRect.bottom = headerLocalRect.top + kHeight_Header;
		//�w�b�_�`��
		AColor	headercolor_start = AColorWrapper::ChangeHSV(color,0,0.8,1.0);
		AColor	headercolor_end = AColorWrapper::ChangeHSV(color,0,1.2,1.0);
		NVMC_PaintRoundedRect(headerLocalRect,headercolor_start,headercolor_end,kGradientType_Vertical,0.15,0.15,
							  kRoundedRectRad,true,true,true,true);
		//�w�b�_�^�C�g���擾
		AText	title;
		mArray_Title.Get(index,title);
		//�w�b�_�^�C�g���`��̈�擾
		ALocalRect	headerTextLocalRect = headerLocalRect;
		headerTextLocalRect.left 	+= kLeftMargin_HeaderText;
		headerTextLocalRect.right 	-= kRightMargin_HeaderText;
		headerTextLocalRect.top 	+= kTopMargin_HeaderText;
		headerTextLocalRect.bottom 	-= kBottomMargin_HeaderText;
		//�w�b�_�^�C�g���`��
		AText	ellipsisTitle;
		NVI_GetEllipsisTextWithFixedLastCharacters(title,headerTextLocalRect.right-headerTextLocalRect.left,5,ellipsisTitle);
		NVMC_DrawText(headerTextLocalRect,ellipsisTitle);
		
		//==================content�`��==================
		//�t�H���g�ݒ�
		NVMC_SetDefaultTextProperty(labelfontname,10.0,kColor_Gray20Percent,kTextStyle_DropShadow,true,alpha);
		//���e�e�L�X�g�擾
		AText	text;
		mArray_Text.Get(index,text);
		//���erect�擾
		ALocalRect	contentLocalRect = itemLocalRect;
		contentLocalRect.top += kHeight_Header;
		//���e�e�L�X�g�`��̈�擾
		ALocalRect	contentTextLocalRect = contentLocalRect;
		contentTextLocalRect.left 	+= kLeftMargin_ContentText;
		contentTextLocalRect.right 	-= kRightMargin_ContentText;
		contentTextLocalRect.top 	+= kTopMargin_ContentText;
		contentTextLocalRect.bottom -= kBottomMargin_ContentText;
		//���e�e�L�X�g�`��
		AText	ellipsisText;
		NVI_GetEllipsisTextWithFixedLastCharacters(text,contentTextLocalRect.right-contentTextLocalRect.left,5,ellipsisText);
		NVMC_DrawText(contentTextLocalRect,ellipsisText);
		
		//==================�t���[���`��==================
		//�t���[���`��
		NVMC_FrameRoundedRect(itemLocalRect,kColor_Gray50Percent,1.0,kRoundedRectRad,true,true,true,true);
	}
}

/**
���ڂ�rect�擾
*/
void	AView_Notification::GetItemBox( const AIndex inIndex, ALocalRect& outRect ) const
{
	//frame rect�擾
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	//����rect�擾
	outRect = frameRect;
	outRect.top		= inIndex*kHeight_ItemBoxWithMargin;
	outRect.bottom 	= outRect.top + kHeight_ItemBox;
}

/**
local point���獀�ڂ�index������
*/
AIndex	AView_Notification::FindItemBox( const ALocalPoint& inLocalPoint ) const
{
	AItemCount	itemCount = mArray_Text.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//���ڂ�rect���擾
		ALocalRect	boxrect = {0};
		GetItemBox(i,boxrect);
		//�w��point��rect���Ȃ猻�݂�index��Ԃ��B
		if( IsPointInRect(inLocalPoint,boxrect) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_Notification::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//���ڂ��N���b�N������A���ڂ����
	AIndex	index = FindItemBox(inLocalPoint);
	if( index != kIndex_Invalid )
	{
		//URL�X�L�[���������URL�X�L�[�����s #1003
		AText	uri;
		mArray_URI.Get(index,uri);
		if( uri.GetItemCount() > 0 )
		{
			GetApp().NVI_ExecuteURIScheme(uri);
		}
		//����
		CloseBox(index);
	}
	return true;
}

//
void	AView_Notification::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
		const ALocalPoint inLocalPoint )
{
}

ABool	AView_Notification::EVTDO_DoTextInput( const AText& inText, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	return false;
}

//�J�[�\��
ABool	AView_Notification::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	return true;
}

void	AView_Notification::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
}

/**
�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
*/
ABool	AView_Notification::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	return true;
}

//#507
/**
�w���v�^�O
*/
ABool	AView_Notification::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	return false;
}

/**
�J�[�\���^�C�v�擾
*/
ACursorType	AView_Notification::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	return kCursorType_Arrow;
}

/**
tick�^�C�}�[
*/
void	AView_Notification::SPI_DoTickTimer()
{
	//�e���ږ��̃��[�v
	for( AIndex i = 0; i < mArray_HideCounter.GetItemCount(); )
	{
		//�J�E���^�f�N�������g�i�������Amax�l���ݒ肳��Ă��鍀�ڂ̓f�N�������g���Ȃ��j
		ANumber	count = mArray_HideCounter.Get(i);
		if( count == kNumber_MaxNumber )
		{
			i++;
			continue;
		}
		count--;
		mArray_HideCounter.Set(i,count);
		//�J�E���^16�ȉ��ɂȂ����烿�l�������Ă���
		if( count <= 16 )
		{
			mArray_Alpha.Set(i,0.9*count/16);
			NVI_Refresh();
			NVM_GetWindow().NVI_UpdateWindow();
		}
		//�J�E���^0�ɂȂ����獀�ڂ����
		if( count <= 0 )
		{
			CloseBox(i);
		}
		else
		{
			i++;
		}
	}
}

/**
�E�C���h�Ehide������
*/
void	AView_Notification::SPI_DoWindowHide()
{
	//�^�C�}�[�ɂ���ĕ���ׂ����ڂ̂ݑS�ĕ���
	//�e���ږ��̃��[�v
	for( AIndex i = 0; i < mArray_HideCounter.GetItemCount(); )
	{
		ANumber	count = mArray_HideCounter.Get(i);
		if( count != kNumber_MaxNumber )
		{
			CloseBox(i);
		}
		else
		{
			i++;
		}
	}
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

/**
�u�����ʂ�\��
*/
void	AView_Notification::SPI_SetNotification_ReplaceResult( const ABool inNotAborted, const ANumber inCount,
		const AText& inFindText, const AText& inReplaceText )
{
	//�e�L�X�g�擾
	AText	text, title;
	AColor	color = kColor_Blue;
	if( inNotAborted == true )
	{
		//�u��OK��
		text.SetLocalizedText("Notification_ReplaceResult_OK");
		title.SetLocalizedText("Notification_ReplaceResult_OK_Title");
	}
	else
	{
		//�u��abort��
		text.SetLocalizedText("Notification_ReplaceResult_Aborted");
		title.SetLocalizedText("Notification_ReplaceResult_Aborted_Title");
		color = kColor_Red;
	}
	//�u����
	text.ReplaceParamText('0',inCount);
	//�����e�L�X�g
	AText	findtext;
	findtext.SetText(inFindText);
	findtext.ReplaceChar(kUChar_LineEnd,kUChar_Space);
	findtext.ReplaceChar(kUChar_Tab,kUChar_Space);
	//�u���e�L�X�g
	AText	replacetext;
	replacetext.SetText(inReplaceText);
	replacetext.ReplaceChar(kUChar_LineEnd,kUChar_Space);
	replacetext.ReplaceChar(kUChar_Tab,kUChar_Space);
	//�����E�u���e�L�X�g���w�b�_�^�C�g���ɂ���
	title.AddCstring(" (");
	title.AddText(findtext);
	title.AddCstring("->");
	title.AddText(replacetext);
	title.AddCstring(")");
	//���ڒǉ�
	mArray_Text.Add(text);
	mArray_Title.Add(title);
	mArray_HideCounter.Add(80);
	mArray_Color.Add(color);
	mArray_Alpha.Add(1.0);
	mArray_Type.Add(kNotificationType_ReplaceResult);
	mArray_URI.Add(GetEmptyText());//#1003
	//�`��X�V
	NVI_Refresh();
	//�E�C���h�E�����X�V
	SPI_UpdateWindowHeight();
}

/**
�������ʂ�\��
*/
void	AView_Notification::SPI_SetNotification_FindResult( const ABool inFoundInNext, const ABool inFoundInPrev,
		const AText& inFindText )
{
	//�e�L�X�g�擾
	AText	text, title;
	AColor	color = kColor_Gray70Percent;
	if( inFoundInNext == false && inFoundInPrev == false )
	{
		//�������ʂȂ��i�O�ɂ���ɂ��Ȃ��j
		text.SetLocalizedText("Notification_FindResult_NotFoundBoth");
		title.SetLocalizedText("Notification_FindResult_NotFoundBoth_Title");
	}
	else if( inFoundInNext == false && inFoundInPrev == true )
	{
		//�������ʂȂ��i��ɂ͂Ȃ����O�ɂ͂���j
		text.SetLocalizedText("Notification_FindResult_NotFoundNext");
		title.SetLocalizedText("Notification_FindResult_NotFoundNext_Title");
		color = kColor_Blue;
	}
	else if( inFoundInNext == true && inFoundInPrev == false )
	{
		//�������ʂȂ��i�O�ɂ͂Ȃ�����ɂ͂���j
		text.SetLocalizedText("Notification_FindResult_NotFoundPrev");
		title.SetLocalizedText("Notification_FindResult_NotFoundPrev_Title");
		color = kColor_Blue;
	}
	//�����e�L�X�g�擾�A�^�C�g���ɕ\��
	AText	findtext;
	findtext.SetText(inFindText);
	findtext.ReplaceChar(kUChar_LineEnd,kUChar_Space);
	findtext.ReplaceChar(kUChar_Tab,kUChar_Space);
	title.AddCstring(" (");
	title.AddText(findtext);
	title.AddCstring(")");
	//�������e�̍��ڂ��Ȃ���΁A���ڒǉ� �i�^�C�g���Ɩ{�������ꂩ������Ă���΁A���ڒǉ� #967�j
	//#967 AIndex	index = mArray_Text.Find(text);
	//#967 if( index == kIndex_Invalid )
	if( mArray_Text.Find(text) == kIndex_Invalid || mArray_Title.Find(title) == kIndex_Invalid )//#967
	{
		//���ڒǉ�
		mArray_Text.Add(text);
		mArray_Title.Add(title);
		mArray_HideCounter.Add(60);
		mArray_Color.Add(color);
		mArray_Alpha.Add(1.0);
		mArray_Type.Add(kNotificationType_FindResult);
		mArray_URI.Add(GetEmptyText());//#1003
	}
	//�`��X�V
	NVI_Refresh();
	//�E�C���h�E�����X�V
	SPI_UpdateWindowHeight();
}

/**
���ۑ��f�[�^������Ƃ��̕\��
*/
void	AView_Notification::SPI_SetNotification_UnsavedFile()
{
	//�e�L�X�g�擾
	AText	text,title;
	text.SetLocalizedText("Notification_UnsavedFile");
	title.SetLocalizedText("Notification_UnsavedFile_Title");
	//���ۑ��f�[�^��notification���\���̏ꍇ�̂݁A���ڒǉ�����
	AIndex	index = mArray_Type.Find(kNotificationType_UnsavedFile);
	if( index == kIndex_Invalid )
	{
		mArray_Text.Add(text);
		mArray_Title.Add(title);
		mArray_HideCounter.Add(kNumber_MaxNumber);
		mArray_Color.Add(kColor_Red);
		mArray_Alpha.Add(1.0);
		mArray_Type.Add(kNotificationType_UnsavedFile);
		mArray_URI.Add(GetEmptyText());//#1003
	}
	//�`��X�V
	NVI_Refresh();
	//�E�C���h�E�����X�V
	SPI_UpdateWindowHeight();
}

/**
�t�H���_���x���i���ݒ�́u�t�H���_���x���v�Őݒ肵�����x���j�\��
*/
void	AView_Notification::SPI_SetNotification_FolderLabel( const AText& inFolderPath, const AText& inLabelText )
{
	//�e�L�X�g�擾
	AText	text;
	text.SetLocalizedText("Notification_FolderLabel");
	text.ReplaceParamText('0',inFolderPath);
	//���ڒǉ�
	mArray_Text.Add(text);
	mArray_Title.Add(inLabelText);
	mArray_HideCounter.Add(kNumber_MaxNumber);
	mArray_Color.Add(kColor_Orange);
	mArray_Alpha.Add(1.0);
	mArray_Type.Add(kNotificationType_FolderLabel);
	mArray_URI.Add(GetEmptyText());//#1003
	//�`��X�V
	NVI_Refresh();
	//�E�C���h�E�����X�V
	SPI_UpdateWindowHeight();
}

//#912
/**
�C���f���g�����^�C�v�ݒ�X�V�\��
*/
void	AView_Notification::SPI_SetNotification_IndentCharacterChanged( const AModeIndex inModeIndex,
																		const AText& inURI )//#1003
{
	AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(inModeIndex);
	//�e�L�X�g�擾
	AText	text;
	if( modePrefDB.GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == false )
	{
		text.SetLocalizedText("Notification_IndentCharacterChanged_Tab");
	}
	else
	{
		text.SetLocalizedText("Notification_IndentCharacterChanged_Space");
		text.ReplaceParamText('0',modePrefDB.GetData_Number(AModePrefDB::kIndentWidth));
	}
	//
	AText	title;
	title.SetLocalizedText("Notification_IndentCharacterChanged_Title");
	//���ڒǉ�
	mArray_Text.Add(text);
	mArray_Title.Add(title);
	mArray_HideCounter.Add(60);
	mArray_Color.Add(kColor_Yellow);
	mArray_Alpha.Add(1.0);
	mArray_Type.Add(kNotificationType_IndentCharacterChanged);
	mArray_URI.Add(inURI);//#1003
	//�`��X�V
	NVI_Refresh();
	//�E�C���h�E�����X�V
	SPI_UpdateWindowHeight();
}

//#912
/**
���A�v���ҏW���ʓǂݍ��ݕ\��
*/
void	AView_Notification::SPI_SetNotification_ReloadEditByOtherApp( const AText& inFileName )
{
	//�e�L�X�g�擾
	AText	text;
	text.SetLocalizedText("Notification_ReloadEditByOtherApp");
	text.ReplaceParamText('0',inFileName);
	//
	AText	title;
	title.SetLocalizedText("Notification_ReloadEditByOtherApp_Title");
	//���ڒǉ�
	mArray_Text.Add(text);
	mArray_Title.Add(title);
	mArray_HideCounter.Add(60);
	mArray_Color.Add(kColor_Blue);
	mArray_Alpha.Add(1.0);
	mArray_Type.Add(kNotificationType_ReloadEditByOtherApp);
	mArray_URI.Add(GetEmptyText());//#1003
	//�`��X�V
	NVI_Refresh();
	//�E�C���h�E�����X�V
	SPI_UpdateWindowHeight();
}

/**
�ėpnotification
*/
void	AView_Notification::SPI_SetNotification_General( const AText& inTitle, const AText& inText,
														 const AText& inURI )//#1003
{
	//���ڒǉ�
	mArray_Text.Add(inText);
	mArray_Title.Add(inTitle);
	mArray_HideCounter.Add(60);
	mArray_Color.Add(kColor_Blue);
	mArray_Alpha.Add(1.0);
	mArray_Type.Add(kNotificationType_General);
	mArray_URI.Add(inURI);//#1003
	//�`��X�V
	NVI_Refresh();
	//�E�C���h�E�����X�V
	SPI_UpdateWindowHeight();
}

/**
���ڂ����i�폜�j
*/
void	AView_Notification::CloseBox( const AIndex inIndex )
{
	//���ڍ폜
	mArray_Text.Delete1(inIndex);
	mArray_Title.Delete1(inIndex);
	mArray_HideCounter.Delete1(inIndex);
	mArray_Color.Delete1(inIndex);
	mArray_Alpha.Delete1(inIndex);
	mArray_Type.Delete1(inIndex);
	mArray_URI.Add(GetEmptyText());//#1003
	//�`��X�V
	NVI_Refresh();
	//�E�C���h�E�����X�V
	SPI_UpdateWindowHeight();
}

/**
�E�C���h�E�����擾
*/
ANumber	AView_Notification::SPI_GetWindowHeight() const
{
	return mArray_Text.GetItemCount() * kHeight_ItemBoxWithMargin;
}

/**
�E�C���h�E�����X�V
*/
void	AView_Notification::SPI_UpdateWindowHeight()
{
	ANumber	height = SPI_GetWindowHeight();
	ARect	windowbounds = {0};
	NVM_GetWindow().NVI_GetWindowBounds(windowbounds);
	windowbounds.bottom = windowbounds.top + height;
	NVM_GetWindow().NVI_SetWindowBounds(windowbounds);
}


