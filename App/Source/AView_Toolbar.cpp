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

AView_Toolbar

*/

#include "stdafx.h"

#include "AView_Toolbar.h"
#include "AWindow_Text.h"
#include "ADocument_Text.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_Toolbar
#pragma mark ===========================
//InfoHeader

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_Toolbar::AView_Toolbar( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID ) : AView(/*#199 inParent,inWindow*/inWindowID,inID)
/*#724,mDragMode(false)*/,mDragCurrentIndex(kIndex_Invalid),mModeIndex(kIndex_Invalid)//#232
,mMouseTrackStartLocalPoint(kLocalPoint_00),mDraggingWindow(true)//#725
{
	NVMC_SetOffscreenMode(true);//win
}

//�f�X�g���N�^
AView_Toolbar::~AView_Toolbar()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_Toolbar::NVIDO_Init()
{
	//#236
	AArray<AScrapType>	scrapTypeArray;
	//scrapTypeArray.Add(kScrapType_File);
	scrapTypeArray.Add(kScrapType_ButtonView);
	scrapTypeArray.Add(kScrapType_UnicodeText);//#358
	scrapTypeArray.Add(kScrapType_File);//win
	NVMC_EnableDrop(scrapTypeArray);
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_Toolbar::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_Toolbar::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//�_�u���N���b�N���̓}�N�����ڒǉ�
	if( inClickCount == 2 )
	{
		GetApp().SPI_GetTextWindowByID(NVM_GetWindowID()).SPI_ShowAddToolButtonWindow();
		return true;
	}
	
	//tracking�J�n���}�E�X�ʒu�L��
	mMouseTrackStartLocalPoint = inLocalPoint;
	
	//Mouse tracking�J�n
	NVM_SetMouseTrackingMode(true);
	NVI_Refresh();
	return true;
}

/**
�}�E�X�{�^��������Tracking��OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_Toolbar::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�E�C���h�E�h���b�O
	//tracking�J�n���ƈႤ�}�E�X�ʒu�ɂȂ�����drag�J�n
	if( IsDragStart(inLocalPoint,mMouseTrackStartLocalPoint) == true )
	{
		if( mDraggingWindow == false )
		{
			mDraggingWindow = true;
		}
	}
	//
	if( mDraggingWindow == true )
	{
		//�E�C���h�E���h���b�O�ړ�
		AGlobalPoint	mouseGlobalPoint = {0};
		AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
		ARect	rect = {0};
		NVM_GetWindow().NVI_GetWindowBounds(rect);
		ANumber	w = rect.right - rect.left;
		ANumber	h = rect.bottom - rect.top;
		rect.left = mouseGlobalPoint.x - mMouseTrackStartLocalPoint.x;
		rect.top = mouseGlobalPoint.y - mMouseTrackStartLocalPoint.y;
		/*�ʏ�h�L�������g�E�C���h�E�͂��Ԃ�s�v�BOS���������Ă����B
		//
		AGlobalRect	screenBounds = {0};
		AWindow::NVI_GetAvailableWindowPositioningBounds(screenBounds);
		//
		if( rect.left < screenBounds.left )
		{
			rect.left = screenBounds.left;
		}
		if( rect.top < screenBounds.left )
		{
			rect.top = screenBounds.top;
		}
		*/
		{
			//
			rect.right = rect.left + w;
			rect.bottom = rect.top + h;
			NVM_GetWindow().NVI_SetWindowBounds(rect);
		}
	}
}

/**
�}�E�X�{�^��������Tracking�I������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_Toolbar::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//
	if( mDraggingWindow == false )
	{
		//�E�C���h�E�ɃN���b�N��ʒm
		NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
	}
	//�E�C���h�E�h���b�O���t���O����
	mDraggingWindow = false;
	
	//Tracking����
	NVM_SetMouseTrackingMode(false);
}

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_Toolbar::EVTDO_DoDraw()
{
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	//NVMC_FrameRect(viewRect,kColor_Red);
	/*#724
	//�^�u�I���r���[�S�̂̕\��
	NVMC_PaintRect(viewRect,kColor_Gray92Percent,1.0);
	ALocalPoint	spt,ept;
	spt.x = viewRect.left;
	spt.y = viewRect.bottom-1;//win 080618
	ept.x = viewRect.right;
	ept.y = viewRect.bottom-1;//win 080618
	NVMC_DrawLine(spt,ept,kColor_Gray60Percent,1.0);
	*/
	/*
	//#597 gradient�\��
	AColor	startColor = kColor_Gray92Percent;
	AColor	endColor = kColor_Gray85Percent;
	ALocalRect	gradientRect = viewRect;
	gradientRect.bottom -= 1;
	gradientRect.top = gradientRect.bottom - 30;
	NVMC_PaintRectWithVerticalGradient(gradientRect,startColor,endColor,1.0,1.0);
	*/
	/*
	//
	ALocalPoint	pt = {0,0};
	NVMC_DrawImageFlexibleWidthAndHeight(kImageID_bg32MacroBar_1,kImageID_bg32MacroBar_2,kImageID_bg32MacroBar_3,
				kImageID_bg32MacroBar_4,kImageID_bg32MacroBar_5,kImageID_bg32MacroBar_6,
				kImageID_bg32MacroBar_7,kImageID_bg32MacroBar_8,kImageID_bg32MacroBar_9,
				pt,viewRect.right-viewRect.left,viewRect.bottom-viewRect.top);
	*/
	
	//�c�[���o�[�}�N�����w�i�F�`��iMojave�̃c�[���o�[�F�ɍ��킹��j#1347
	AColor	backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("C8C8C8");
	if( NVI_GetWindow().NVI_IsWindowActive() == false )
	{
		backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("F6F6F6");
	}
	//�_�[�N���[�h #1316
	if( GetApp().NVI_IsDarkMode() == true )
	{
		backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("363636");
		if( NVI_GetWindow().NVI_IsWindowActive() == false )
		{
			backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("2C2C2C");
		}
	}
	//
	NVMC_PaintRect(viewRect,backgroundColor,1.0);
	
	//==================��؂���`��==================
	ALocalPoint	spt = {viewRect.left,viewRect.bottom-1}, ept = {viewRect.right,viewRect.bottom-1};
	AColor	color = kColor_Gray70Percent;
	if( NVI_GetWindow().NVI_IsWindowActive() == false )
	{
		color = kColor_Gray90Percent;
	}
	//�_�[�N���[�h #1316
	if( GetApp().NVI_IsDarkMode() == true )
	{
		color = AColorWrapper::GetColorByHTMLFormatColor("000000");
	}
	//
	NVMC_DrawLine(spt,ept,/*#1079 AColorWrapper::GetColorByHTMLFormatColor("4c4b4b")*/color,1.0);
	
}

//#232
/**
Drag&Drop�����iDrag���j
*/
void	AView_Toolbar::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, ABool& outIsCopyOperation )
{
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_ButtonView) == true )
	{
		//==================�}�N���o�[���ڃh���b�O==================
		AText	text;
		AScrapWrapper::GetDragData(inDragRef,kScrapType_ButtonView,text);
		AUniqID	viewUniqID = kUniqID_Invalid;
		viewUniqID.val = text.GetNumber();
		AViewID	viewID = GetApp().NVI_GetViewIDByUniqID(viewUniqID);
		AWindowPoint	wpt = {0};
		NVM_GetWindow().NVI_GetWindowPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,wpt);
		GetApp().SPI_GetTextWindowByID(NVM_GetWindowID()).SPI_MacroBarItemDragging(viewID,wpt);
		//
		ACursorWrapper::SetCursor(kCursorType_Arrow);
	}
	else
	{
		ACursorWrapper::SetCursor(kCursorType_ArrowCopy);
	}
	/*#850
	if( mDragMode == true )
	{
		XorDragLine(mDragCurrentIndex);
	}
	mDragMode = false;
	//
	//Windows�łł�Tracking����inDragRef��NULL if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_ButtonView) == true ||
	//Windows�łł�Tracking����inDragRef��NULL 			AScrapWrapper::ExistURLDragData(inDragRef) == true )//#358
	{
		//#575
		if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_ButtonView) == true )
		{
			ACursorWrapper::SetCursor(kCursorType_Arrow);
		}
		else
		{
			ACursorWrapper::SetCursor(kCursorType_ArrowCopy);
		}
		//
		mDragMode = true;
		mDragCurrentIndex = FindDragIndex(inLocalPoint);
		XorDragLine(mDragCurrentIndex);
	}
	*/
}

/**
Drag&Drop�����iDrag��View�ɓ������j
*/
void	AView_Toolbar::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	/*#724
	if( mDragMode == true )
	{
		XorDragLine(mDragCurrentIndex);
	}
	mDragMode = false;
	*/
}

/**
Drag&Drop�����iDrag��View����o���j
*/
void	AView_Toolbar::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	/*#724
	if( mDragMode == true )
	{
		XorDragLine(mDragCurrentIndex);
	}
	mDragMode = false;
	*/
	//
	GetApp().SPI_GetTextWindowByID(NVM_GetWindowID()).SPI_MacroBarItemStopDragging();
}

/**
Drag&Drop�����iDrop�j
*/
void	AView_Toolbar::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	/*#724
	if( mDragMode == true )
	{
		XorDragLine(mDragCurrentIndex);
	}
	mDragMode = false;
	*/
	//#358 Web����c�[���{�^���_�E�����[�h
	AText	url, title;
	if( AScrapWrapper::GetURLDragData(inDragRef,url,title) == true )//#688
	{
		//==================URL�h���b�v==================
		AFileAcc	toolbarFolder;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarFolder(/*#844 false,*/toolbarFolder);
		AFileAcc	file;
		file.SpecifyChild(toolbarFolder,title);
		file.CreateFile();
		url.Delete(0,7);//"http://"���폜�ihttp://�̓_�~�[�B���΃p�X�Ƃ݂Ȃ����̂�������邽�߁j
		url.ConvertFromURLEscape();
		file.WriteFile(url);
		AFileAttribute	attr;
		GetApp().SPI_GetToolFileAttribute(attr);
		file.SetFileAttribute(attr);
		GetApp().SPI_GetModePrefDB(mModeIndex).InsertToolbarItem(mDragCurrentIndex,kToolbarItemType_File,file,false);//#427
		GetApp().SPI_GetModePrefWindow(mModeIndex).NVI_UpdateProperty();
		return;
	}
	//
	AFileAcc	file;
	AText	text;
	if( AScrapWrapper::GetFileDragData(inDragRef,file) == true )
	{
		//==================�t�@�C���h���b�v==================
		//#575
		AObjectArray<AFileAcc>	fileArray;
		AScrapWrapper::GetFileDragData(inDragRef,fileArray);
		for( AIndex i = 0; i < fileArray.GetItemCount(); i++ )
		{
			GetApp().SPNVI_CreateDocumentFromLocalFile(fileArray.GetObjectConst(i));
		}
	}
	else if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_ButtonView) == true )
	{
		//==================�}�N���o�[���ڃh���b�v==================
		AText	text;
		AScrapWrapper::GetDragData(inDragRef,kScrapType_ButtonView,text);
		AUniqID	viewUniqID = kUniqID_Invalid;
		viewUniqID.val = text.GetNumber();
		AViewID	viewID = GetApp().NVI_GetViewIDByUniqID(viewUniqID);
		AWindowPoint	wpt = {0};
		NVM_GetWindow().NVI_GetWindowPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,wpt);
		GetApp().SPI_GetTextWindowByID(NVM_GetWindowID()).SPI_MacroBarItemDrop(viewID,wpt);
	}
	else if( AScrapWrapper::ExistTextDragData(inDragRef) == true )
	{
		//==================�e�L�X�g�h���b�v==================
		AText	text;
		AScrapWrapper::GetTextDragData(inDragRef,text);
		AText	title;
		title.SetText(text);
		if( title.GetItemCount() > kLimit_MacroTitleLength )
		{
			AIndex	p = title.GetCurrentCharPos(kLimit_MacroTitleLength);
			if( p < title.GetItemCount() )
			{
				title.DeleteAfter(p);
				title.AddCstring("...");
			}
		}
		GetApp().SPI_GetModePrefDB(mModeIndex).CreateNewToolbar(title,text);
	}
}

//#619
/**
�E�N���b�N��������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
@param inClickCout �N���b�N��
*/
ABool	AView_Toolbar::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//ContextMenu�\��
	AGlobalPoint	globalPoint = {0,0};
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	//#688 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(kContextMenuID_ToolBar,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
	NVMC_ShowContextMenu(kContextMenuID_ToolBar,globalPoint);//#688
	return true;
}

/**
*/
/*
AIndex	AView_Toolbar::FindDragIndex( const ALocalPoint& inLocalPoint ) const
{
	AIndex	index = 0;
	for( ; index < mToolButtonPositionArray.GetItemCount()-1; index++ )
	{
		if( (mToolButtonXPositionArray.Get(index)+mToolButtonXPositionArray.Get(index+1))/2 > inLocalPoint.x )   break;
	}
	return index;
}
*/

/**
*/
/*
void	AView_Toolbar::XorDragLine( const AIndex inDragIndex )
{
	ALocalRect	frame;
	NVM_GetLocalViewRect(frame);
	ALocalPoint	start, end;
	start.x = mToolButtonXPositionArray.Get(inDragIndex)-2;
	start.y = frame.top+2;
	end.x = start.x;
	end.y = frame.bottom-4;
	NVMC_DrawXorCaretLine(start,end,true,false,2);
}
*/

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---

/**
*/
void	AView_Toolbar::SPI_SetInfo( const AModeIndex inModeIndex )
{
	mModeIndex = inModeIndex;
	/*
	mToolButtonPositionArray.DeleteAll();
	for( AIndex i = 0; i < inToolButtonPositionArray.GetItemCount(); i++ )
	{
		mToolButtonPositionArray.Add(inToolButtonPositionArray.Get(i));
	}
	*/
}

