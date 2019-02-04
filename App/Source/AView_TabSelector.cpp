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

AView_TabSelector

*/

#include "stdafx.h"

#include "AView_TabSelector.h"
#include "AWindow_Text.h"
#include "ADocument_Text.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_TabSelector
#pragma mark ===========================
//�^�u�I���r���[

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_TabSelector::AView_TabSelector( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID ) 
: AView(/*#199 inParent,inWindow*/inWindowID,inID), mTabRowHeight(21), mTabColumnWidth(160), mFontSize(10.5), mHoverIndex(kIndex_Invalid)//R0177, B0000 080810
//#164 ,mCurrentCloseButtonIndex(kIndex_Invalid), mEnableTabCloseButton(false)//R0228
,mMouseTrackResultIsDrag(false),mMouseDown(false)//#688 mMouseTrackByLoop(true)//#364
//#850 ,mDragMode(false),mDragCurrentIndex(kIndex_Invalid),mDragOriginalDisplayIndex(kIndex_Invalid)
,mMouseTrackStartLocalPoint(kLocalPoint_00)//#364
,mHoverCloseButton(false)//#164
,mDragging(false),mDraggingCurrentDisplayIndex(kIndex_Invalid),mDraggingTabShouldHide(false)//#575 #850
,mMouseClickCount(0)
{
	NVMC_SetOffscreenMode(true);//win
	//#92 ��������NVIDO_Init()�ֈړ�
	
	//view��s�����ɐݒ� #1090
	NVMC_SetOpaque(true);
}

//�f�X�g���N�^
AView_TabSelector::~AView_TabSelector()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_TabSelector::NVIDO_Init()
{
	SPI_UpdateProperty();//R0177
	//B0000 080810
	NVMC_EnableMouseLeaveEvent();
	/*#164
	//R0228
	AWindowPoint	pt = {0,0};
	NVM_GetWindow().NVI_CreateButtonView(kTabCloseButtonControlID,pt,16,16);
	NVM_GetWindow().NVI_GetButtonViewByControlID(kTabCloseButtonControlID).SPI_SetIcon(kIconID_SplitView);//#182
	NVM_GetWindow().NVI_SetShowControl(kTabCloseButtonControlID,false);//#284
	*/
	//win
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_TabSelector);
	scrapTypeArray.Add(kScrapType_File);//#575
	NVMC_EnableDrop(scrapTypeArray);
	/*#688
	//#364
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_TabSelector::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_TabSelector::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//
	mMouseDown = true;//#364
	mMouseTrackStartLocalPoint = inLocalPoint;//#364
	
	AIndex	col = inLocalPoint.x / mTabColumnWidth;
	AIndex	row = inLocalPoint.y / mTabRowHeight;
	//#498 AIndex	index = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
	AIndex	displayIndex = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
	
	//R0225 �^�u�_�u���N���b�N�ŐV�K�h�L�������g
	if( displayIndex >= /*#498 NVM_GetWindow().NVI_GetTabCount()*/mDisplayOrderArray.GetItemCount() || 
				col >= GetTabCountInOneRow(NVI_GetViewWidth()) ) 
	{
		if( inClickCount == 2 )
		{
			AText	name;
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kCmdNModeName,name);
			AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(name);
			if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
			GetApp().SPNVI_CreateNewTextDocument(modeIndex);
		}
		return false;
	}
	
	//#364 �^�u��������ւ�
	//�����ň�U�C�x���g�R�[���o�b�N�𔲂���BMouseTrack�͕ʂ̃C�x���g�ōs���B
	//#688 if( mMouseTrackByLoop == false )
	//#688{
	//TrackingMode�ݒ�
	mMouseTrackResultIsDrag = false;
	mMouseClickCount = inClickCount;
	NVM_SetMouseTrackingMode(true);
	NVI_Refresh();
	return true;
	//#688}
	/*#688
	//Drag�J�n���菈��
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	mMouseTrackResultIsDrag = false;
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//�^�u�͈͎擾
		ALocalRect	tabrect = {0,0,0,0};
		GetTabRect(mMouseTrackStartLocalPoint,tabrect);
		//Drag�J�n���菈��
		ALocalPoint	mouseLocalPoint;
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		if(	mouseLocalPoint.x >= tabrect.left && mouseLocalPoint.x <= tabrect.right &&
			mouseLocalPoint.y >= tabrect.top && mouseLocalPoint.y <= tabrect.bottom )
		{
			//�^�u���̂܂�
			mMouseDown = true;
		}
		else
		{
			//�^�u�O�ֈړ�
			mMouseDown = false;
			//Drag�J�n
			mMouseTrackResultIsDrag = true;
			NVI_Refresh();
			break;
		}
		//#662 NVI_Refresh();
	}
	if( mMouseTrackResultIsDrag == true )
	{
		//Drag�J�n
		StartDrag(inLocalPoint,inModifierKeys);
		return true;
	}
	MouseClick(inLocalPoint);//#164
	return true;//#164 false->true�itrue�ɂ��Ȃ���EVTDO_Click()�����̌���s�����BTab�N���[�Y��Ɏ��s���ׂ��ł͂Ȃ��j
	*/
}

//#164
/**
�}�E�X�N���b�N�i�}�E�X�_�E�����}�E�X�A�b�v�j������
*/
void	AView_TabSelector::MouseClick( const ALocalPoint& inLocalPoint )
{
	AIndex	col = inLocalPoint.x / mTabColumnWidth;
	AIndex	row = inLocalPoint.y / mTabRowHeight;
	AIndex	displayIndex = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
	if( displayIndex < 0 || displayIndex >= mDisplayOrderArray.GetItemCount() )
	{
		return;
	}
	AIndex	index = mDisplayOrderArray.Get(displayIndex);
	//
	ALocalRect	closeButtonRect = {0,0,0,0};
	GetCloseButtonRect(displayIndex,closeButtonRect);
	if( //#844 GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowTabCloseButton) == true &&
				mMouseTrackStartLocalPoint.x > closeButtonRect.left && mMouseTrackStartLocalPoint.x < closeButtonRect.right &&
				mMouseTrackStartLocalPoint.y > closeButtonRect.top && mMouseTrackStartLocalPoint.y < closeButtonRect.bottom &&
				inLocalPoint.x > closeButtonRect.left && inLocalPoint.x < closeButtonRect.right &&
				inLocalPoint.y > closeButtonRect.top && inLocalPoint.y < closeButtonRect.bottom )
	{
		//�}�E�X�{�^���_�E���E�A�b�v�ӏ����N���[�Y�{�^�����̂Ƃ��A�^�u�����
		NVM_GetWindow().NVI_TryClose(index);
	}
	else
	{
		//Tab�I������
		NVM_GetWindow().NVI_SelectTab(index);
		//�_�u���N���b�N���̓t���O��t���� #832
		if( mMouseClickCount == 2 )
		{
			ADocumentID	tabDocID = NVM_GetWindow().NVI_GetDocumentIDByTabIndex(index);
			GetApp().SPI_GetTextDocumentByID(tabDocID).
					SPI_SetDocumentFlag(!GetApp().SPI_GetTextDocumentByID(tabDocID).SPI_GetDocumentFlag());
		}
	}
}

//#364
/**
Drag�J�n
*/
void	AView_TabSelector::StartDrag( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mMouseTrackResultIsDrag == true )
	{
		//�\��Index�擾
		AIndex	col = inLocalPoint.x / mTabColumnWidth;
		AIndex	row = inLocalPoint.y / mTabRowHeight;
		//#850 mDragOriginalDisplayIndex = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
		mDraggingCurrentDisplayIndex = col + row*GetTabCountInOneRow(NVI_GetViewWidth());//#850
		AIndex	tabIndex = mDisplayOrderArray.Get(mDraggingCurrentDisplayIndex);
		
		//Tab�͈͎擾
		ALocalRect	rect = {0,0,0,0};
		GetTabRect(inLocalPoint,rect);
		
		//Drag�̈�ݒ�
		AArray<ALocalRect>	dragRect;
		dragRect.Add(rect);
		//Scrap�ݒ�i�\��Index��n���j
		AArray<AScrapType>	scrapType;
		ATextArray	data;
		scrapType.Add(kScrapType_TabSelector);
		//#859 data.Add(GetEmptyText());
		//drag �f�[�^�ɂ̓h�L�������gID��ݒ�
		AText	tabIndexText;
		ADocumentID	docID = NVM_GetWindow().NVI_GetDocumentIDByTabIndex(tabIndex);
		tabIndexText.SetNumber(GetApp().NVI_GetDocumentUniqID(docID).val);//#850 mDragOriginalDisplayIndex);
		data.Add(tabIndexText);
		//Drag���s
		mDragging = true;//#575
		mDraggingTabShouldHide = false;//#850
		//
		AWindowRect	wrect = {0};
		NVM_GetWindow().NVI_GetWindowRectFromControlLocalRect(NVI_GetControlID(),rect,wrect);
		if( MVMC_DragTextWithWindowImage(inLocalPoint,scrapType,data,wrect,wrect.right-wrect.left,wrect.bottom-wrect.top) == false )
		{
			//�}�E�X�ʒu�擾
			AGlobalPoint	mouseGlobalPoint = {0};
			AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
			APoint	pt = {mouseGlobalPoint.x,mouseGlobalPoint.y};
			//��view��drop����Ȃ�������E�C���h�E������//��drag���ɃE�C���h�E�\���ɐ؂�ւ��悤�ɂ�����
			AWindowID	thisWinID = NVM_GetWindow().GetObjectID();
			GetApp().SPI_GetTextWindowByID(thisWinID).SPI_TabToWindow(tabIndex,true,pt);
		}
		mDragging = false;//#575
		mDraggingCurrentDisplayIndex = kIndex_Invalid;//#850
		mDraggingTabShouldHide = false;//#850
	}
}

//#364
/**
�^�u�͈͎擾
*/
void	AView_TabSelector::GetTabRect( const ALocalPoint& inLocalPoint, ALocalRect& outTabRect )
{
	//�s�^��擾
	AIndex	col = inLocalPoint.x / mTabColumnWidth;
	AIndex	row = inLocalPoint.y / mTabRowHeight;
	//�͈͌v�Z
	outTabRect.left = col*mTabColumnWidth;
	outTabRect.right = outTabRect.left + mTabColumnWidth;
	outTabRect.top = row*mTabRowHeight;
	outTabRect.bottom = outTabRect.top + mTabRowHeight;
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
/**
�}�E�X�{�^��������Tracking��OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_TabSelector::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//Drag�J�n����
	/*#850
	ALocalRect	tabrect = {0,0,0,0};
	GetTabRect(mMouseTrackStartLocalPoint,tabrect);
	if( inLocalPoint.x >= tabrect.left && inLocalPoint.x <= tabrect.right &&
	inLocalPoint.y >= tabrect.top && inLocalPoint.y <= tabrect.bottom )
	*/
	if( IsDragStart(mMouseTrackStartLocalPoint,inLocalPoint) == false )
	{
		//�^�u��
		mMouseDown = true;
	}
	else
	{
		//�^�u�O�ֈړ�
		mMouseDown = false;
		//
		mMouseTrackResultIsDrag = true;
		//�h���b�O�J�n
		StartDrag(mMouseTrackStartLocalPoint,inModifierKeys);
		//TrackingMode����
		NVM_SetMouseTrackingMode(false);
	}
	NVI_Refresh();
}

/**
�}�E�X�{�^��������Tracking�I������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_TabSelector::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	NVM_SetMouseTrackingMode(false);
	//#379
	if( mMouseTrackResultIsDrag == false )
	{
		MouseClick(inLocalPoint);//#164
	}
	//�N���[�Y�{�^���N���b�N���Ƀz�o�[�`����X�V���邽�߁Amoved��������B
	EVTDO_DoMouseMoved(inLocalPoint,inModifierKeys);
}
//#688 #endif

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_TabSelector::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	AIndex	col = inLocalPoint.x / mTabColumnWidth;
	AIndex	row = inLocalPoint.y / mTabRowHeight;
	//#498 AIndex	index = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
	AIndex	displayIndex = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
	if( displayIndex >= mDisplayOrderArray.GetItemCount() )   return true;
	AIndex	index = mDisplayOrderArray.Get(displayIndex);
	if( index >= NVM_GetWindow().NVI_GetTabCount() )   return true;
	mContextMenuSelectedTabIndex = index;
	//#688 GetApp().NVI_GetMenuManager().ShowContextMenu(kContextMenuID_Tab,globalPoint,NVM_GetWindow().NVI_GetWindowRef());//win 080712
	NVMC_ShowContextMenu(kContextMenuID_Tab,globalPoint);//#688
	return true;
}

//�{�^����]��
const ANumber	kButtonTopMargin = 0;

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_TabSelector::EVTDO_DoDraw()
{
	//NSLog(@"AView_TabSelector::EVTDO_DoDraw");
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	
	//�~�{�^���p������
	AText	closetext;
	closetext.SetLocalizedText("PopupClose");
	//�~�{�^���p�����t�H���g�ݒ�
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);
	NVMC_SetDefaultTextProperty(defaultfontname,16.0,kColor_Black,kTextStyle_Normal,true);
	//�~�{�^���p�\�����擾
	ANumber	closeTextWidth = NVMC_GetDrawTextWidth(closetext);
	
	//�A�N�e�B�u���F
	//�w�i
	AColor	backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("D8D8D8");
	//�g��
	AColor	tabFrameColor = AColorWrapper::GetColorByHTMLFormatColor("C0C0C0");
	//1�Ԗڃ^�u
	AColor	firstTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
	AColor	firstTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
	AColor	firstTabPaintColor_Hover = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
	//2�Ԗڃ^�u
	AColor	secondTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("F0F0F0");
	AColor	secondTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
	AColor	secondTabPaintColor_Hover = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
	//3�Ԗڈȍ~�^�u
	AColor	thirdTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("E8E8E8");
	AColor	thirdTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("333333");
	AColor	thirdTabPaintColor_Hover = AColorWrapper::GetColorByHTMLFormatColor("EDEDED");
	//�^�u�̈拫�E��
	AColor	boaderColor = AColorWrapper::GetColorByHTMLFormatColor("C0C0C0");
	//�N���[�Y�{�^��
	AColor	closeButtonColor = kColor_Black;
	//��A�N�e�B�u���F
	if( NVI_GetWindow().NVI_IsWindowActive() == false )
	{
		//�w�i
		backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
		//�g��
		tabFrameColor = AColorWrapper::GetColorByHTMLFormatColor("E6E6E6");
		//1�Ԗڃ^�u
		firstTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
		firstTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("B2B2B2");
		//2�Ԗڃ^�u
		secondTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
		secondTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("B2B2B2");
		//3�Ԗڃ^�u
		thirdTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
		thirdTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("B2B2B2");
		//�^�u�̈拫�E��
		boaderColor = AColorWrapper::GetColorByHTMLFormatColor("E6E6E6");
	}
	//�_�[�N���[�h #1316
	if( AApplication::GetApplication().NVI_IsDarkMode() == true )
	{
		//�w�i
		backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("222222");
		//�g��
		tabFrameColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
		//1�Ԗڃ^�u
		firstTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
		firstTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("F0F0F0");
		firstTabPaintColor_Hover = AColorWrapper::GetColorByHTMLFormatColor("383838");
		//�Q�Ԗڃ^�u
		secondTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
		secondTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("909090");
		secondTabPaintColor_Hover = AColorWrapper::GetColorByHTMLFormatColor("383838");
		//3�Ԗڃ^�u
		thirdTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
		thirdTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("707070");
		thirdTabPaintColor_Hover = AColorWrapper::GetColorByHTMLFormatColor("383838");
		//�^�u�̈拫�E��
		boaderColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
		//�N���[�Y�{�^��
		closeButtonColor = kColor_White;
	}
	
	//=====================�^�u�G���A�w�i�`��=====================
	ALocalRect	r = viewRect;
	r.top -= 1;
	NVMC_PaintRect(r,backgroundColor);
	
	AItemCount	tabCountInOneRow = GetTabCountInOneRow(NVI_GetViewWidth());
	//�P�̃^�u���̕\��
	for( AIndex index = 0; index < NVM_GetWindow().NVI_GetTabCount(); index++ )
	{
		//=====================�e�^�u�`��=====================
		ADocumentID	tabDocID = NVM_GetWindow().NVI_GetDocumentIDByTabIndex(index);
		
		//�\��index, �s�A��擾
		AIndex	displayIndex = mDisplayOrderArray.Find(index);
		if( displayIndex == kIndex_Invalid )   continue;
		
		//#850
		//�h���b�O���̃^�u�̓{�^����\���ɂ���
		if( mDragging == true && mDraggingTabShouldHide == true && displayIndex == mDraggingCurrentDisplayIndex )
		{
			continue;
		}
		
		//
		AIndex	column = displayIndex%tabCountInOneRow;
		AIndex	row = displayIndex/tabCountInOneRow;
		
		//�\��rect�擾
		ALocalRect	rect = viewRect;
		rect.left = column*mTabColumnWidth;
		rect.right = rect.left + mTabColumnWidth;
		rect.top = row*mTabRowHeight;
		rect.bottom = rect.top + mTabRowHeight;
		
		//�t���b�g�h��Ԃ��prect #1079
		ALocalRect	r = rect;
		r.top -= 1;
		if( column != 0 )
		{
			r.left -= 1;
		}
		//�~�{�^���p�����t�H���g�ݒ�
		AText	defaultfontname;
		AFontWrapper::GetDialogDefaultFontName(defaultfontname);
		NVMC_SetDefaultTextProperty(defaultfontname,16.0,closeButtonColor,kTextStyle_Normal,true);
		//�~�{�^���prect �� top���}�C�i�X�ɂ��Ȃ��ƈʒu������Ȃ��E�E�E
		ALocalRect	closetextrect = rect;
		closetextrect.top -= 2;
		closetextrect.left += 5;
		closetextrect.bottom -= 2;
		
		//�~�{�^���͂�rect
		ALocalRect	closeHoverRect = rect;
		closeHoverRect.left += 3;
		closeHoverRect.top += 3;
		closeHoverRect.bottom -= 3;
		closeHoverRect.right = closeHoverRect.left + closeTextWidth + 6;
		
		//=====================�e�^�u�{�^���C���[�W�`��=====================
		//
		AColor	tabPaintColor = thirdTabPaintColor;
		AColor	letterColor = thirdTabLetterColor;
		AColor	tabPaintColor_Hover = thirdTabPaintColor_Hover;
		//
		if( NVM_GetWindow().NVI_GetCurrentTabIndex() == index )
		{
			tabPaintColor = firstTabPaintColor;
			letterColor = firstTabLetterColor;
			tabPaintColor_Hover = firstTabPaintColor_Hover;
		}
		else if( NVM_GetWindow().NVI_GetTabCount() >= 2 )
		{
			if( NVM_GetWindow().NVI_GetTabIndexByZOrderIndex(1) == index )
			{
				tabPaintColor = secondTabPaintColor;
				letterColor = secondTabLetterColor;
				tabPaintColor_Hover = secondTabPaintColor_Hover;
			}
		}
		// -------------------- �^�uimage�`�� --------------------
		ALocalPoint	pt = {rect.left,rect.top+kButtonTopMargin};
		if( mHoverIndex != kIndex_Invalid && index == mHoverIndex )
		{
			//�z�o�[
			NVMC_PaintRect(r,tabPaintColor_Hover);
			//�~�{�^���͂�rounded rect�`��
			if( mHoverCloseButton == true )
			{
				NVMC_PaintRoundedRect(closeHoverRect,closeButtonColor,closeButtonColor,kGradientType_None,
									  0.1,0.1,2.0,true,true,true,true);
			}
			//�~�{�^���`��
			NVMC_DrawText(closetextrect,closetext);
		}
		else
		{
			//�ʏ�
			NVMC_PaintRect(r,tabPaintColor);
		}
		//�^�u�g���`��
		NVMC_FrameRect(r,tabFrameColor);
		
		//=====================�e�L�X�g�`��=====================
		ATextStyle	style = 0;
		//�����F�擾
		NVMC_SetDefaultTextProperty(mFontName,mFontSize,letterColor,style,true);
		ALocalRect	textRect = rect;
		textRect.left += kTextLeftMargin;
		textRect.right -= kTextRightMargin;
		textRect.bottom -= kTextBottomMargin;
		textRect.bottom += 1;
		textRect.top = textRect.bottom-mFontHeight;
		AText	title;
		NVM_GetWindow().NVI_GetTitle(index,title);
		//�e�t�H���_�����\�� #1334
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kTabShowParentFolder) == true )
		{
			AText	fileurl;
			GetApp().SPI_GetTextDocumentByID(tabDocID).SPI_GetURL(fileurl);
			ATextArray	fileurlArray;
			fileurl.Explode('/', fileurlArray);
			if( fileurlArray.GetItemCount() >= 2 )
			{
				AText	folderName;
				fileurlArray.Get(fileurlArray.GetItemCount()-2, folderName);
				title.InsertCstring(0,"/");
				title.InsertText(0,folderName);
			}
		}
		//
		AText	ellipsisTitle;
		NVI_GetEllipsisTextWithFixedLastCharacters(title,textRect.right-textRect.left-8,5,ellipsisTitle);
		//
		NVMC_DrawText(textRect,ellipsisTitle,letterColor,style,kJustification_Center);
		
		if( GetApp().NVI_GetDocumentByID(tabDocID).NVI_IsDirty() == true )
		{
			//
			ANumber	textWidth = NVMC_GetDrawTextWidth(ellipsisTitle);
			//�ύX�L��h�b�g�\��
			ALocalPoint	pt = {(textRect.left+textRect.right)/2 + textWidth/2 + 3, textRect.top};
			NVMC_DrawImage(kImageID_tabDot,pt);
		}
		
		//�t���O�L��\�� #832
		if( GetApp().SPI_GetTextDocumentByID(tabDocID).SPI_GetDocumentFlag() == true )
		{
			//�e�L�X�g�\�����擾
			ANumber	textWidth = NVMC_GetDrawTextWidth(ellipsisTitle);
			//�t���O�A�C�R���\��
			ALocalPoint	pt = {(textRect.left+textRect.right)/2 - textWidth/2 - 15, textRect.top};
			NVMC_DrawImage(kImageID_iconSwlFlag,pt);
		}
	}
	//������
	ALocalRect	borderRect = viewRect;
	borderRect.top = borderRect.bottom -1;
	NVMC_FrameRect(borderRect,boaderColor);
}

//B0000 080810
//�J�[�\��
ABool	AView_TabSelector::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	AItemCount	tabCountInOneRow = GetTabCountInOneRow(NVI_GetViewWidth());
	for( AIndex index = 0; index < NVM_GetWindow().NVI_GetTabCount(); index++ )
	{
		/*#498
		AIndex	column = index%tabCountInOneRow;
		AIndex	row = index/tabCountInOneRow;
		*/
		AIndex	displayIndex = mDisplayOrderArray.Find(index);
		if( displayIndex == kIndex_Invalid )   continue;
		AIndex	column = displayIndex%tabCountInOneRow;
		AIndex	row = displayIndex/tabCountInOneRow;
		//
		ALocalRect	rect = viewRect;
		rect.left = column*mTabColumnWidth;
		rect.right = rect.left + mTabColumnWidth;
		rect.top = row*mTabRowHeight;
		rect.bottom = rect.top + mTabRowHeight;
		//
		if( inLocalPoint.x > rect.left && inLocalPoint.x < rect.right &&
					inLocalPoint.y > rect.top && inLocalPoint.y < rect.bottom )
		{
			/*#164
			//R0228
			if( mEnableTabCloseButton == true )
			{
				ALocalPoint	lpt;
				lpt.x = rect.right - mTabRowHeight;
				lpt.y = rect.top + 2;
				AWindowPoint	wpt;
				NVM_GetWindow().NVI_GetWindowPointFromControlLocalPoint(NVI_GetControlID(),lpt,wpt);
				NVM_GetWindow().NVI_SetControlPosition(kTabCloseButtonControlID,wpt);
				NVM_GetWindow().NVI_SetControlSize(kTabCloseButtonControlID,mTabRowHeight-4,mTabRowHeight-4);
				NVM_GetWindow().NVI_SetShowControl(kTabCloseButtonControlID,true);
				mCurrentCloseButtonIndex = index;
			}
			*/
			//
			if( mHoverIndex != index )
			{
				mHoverIndex = index;
				NVI_Refresh();
			}
			//#164 �N���[�Y�{�^����Hover����
			//#844 if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowTabCloseButton) == true )
			{
				ALocalRect	closeButtonRect = {0,0,0,0};
				GetCloseButtonRect(displayIndex,closeButtonRect);
				if( inLocalPoint.x >= closeButtonRect.left && inLocalPoint.x <= closeButtonRect.right &&
							inLocalPoint.y >= closeButtonRect.top && inLocalPoint.y <= closeButtonRect.bottom )//#688 =������i������蓮������enter/exit���ɂ��傤�ǋ��E�Ɠ����l�ɂȂ�̂Łj
				{
					if( mHoverCloseButton == false )
					{
						mHoverCloseButton = true;
						NVI_Refresh();
					}
				}
				else
				{
					if( mHoverCloseButton == true )
					{
						mHoverCloseButton = false;
						NVI_Refresh();
					}
				}
			}
			return true;
		}
	}
	/*#164
	//R0228
	if( mEnableTabCloseButton == true )
	{
		NVM_GetWindow().NVI_SetShowControl(kTabCloseButtonControlID,false);
		mCurrentCloseButtonIndex = kIndex_Invalid;
	}
	*/
	//#164 �N���[�Y�{�^����Hover����
	if( mHoverCloseButton == true )
	{
		mHoverCloseButton = false;
		NVI_Refresh();
	}
	//
	ClearHover();
	return true;
}

//B0000 080810
void	AView_TabSelector::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	/*#164
	//R0228
	if( mEnableTabCloseButton == true )
	{
		ALocalRect	viewRect;
		NVM_GetLocalViewRect(viewRect);
		if( (inLocalPoint.x > viewRect.left && inLocalPoint.x < viewRect.right &&
						inLocalPoint.y > viewRect.top && inLocalPoint.y < viewRect.bottom) == false )
		{
			NVM_GetWindow().NVI_SetShowControl(kTabCloseButtonControlID,false);
			mCurrentCloseButtonIndex = kIndex_Invalid;
		}
	}
	*/
	//#1644 �N���[�Y�{�^����Hover����
	if( mHoverCloseButton == true )
	{
		mHoverCloseButton = false;
		NVI_Refresh();
	}
	//
	ClearHover();
}

//B0000 080810
void	AView_TabSelector::ClearHover()
{
	mHoverIndex = kIndex_Invalid;
	NVI_Refresh();
}

//R0240
ABool	AView_TabSelector::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	outTagSide = kHelpTagSide_Default;//#643
	//#688 NVM_GetWindowConst().NVI_HideHelpTag();
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	AItemCount	tabCountInOneRow = GetTabCountInOneRow(NVI_GetViewWidth());
	for( AIndex index = 0; index < NVM_GetWindowConst().NVI_GetTabCount(); index++ )
	{
		/*#498
		AIndex	column = index%tabCountInOneRow;
		AIndex	row = index/tabCountInOneRow;
		*/
		AIndex	displayIndex = mDisplayOrderArray.Find(index);
		if( displayIndex == kIndex_Invalid )   continue;
		AIndex	column = displayIndex%tabCountInOneRow;
		AIndex	row = displayIndex/tabCountInOneRow;
		//
		ALocalRect	rect = viewRect;
		rect.left = column*mTabColumnWidth;
		rect.right = rect.left + mTabColumnWidth;
		rect.top = row*mTabRowHeight;
		rect.bottom = rect.top + mTabRowHeight;
		//
		if( inPoint.x > rect.left && inPoint.x < rect.right &&
					inPoint.y > rect.top && inPoint.y < rect.bottom )
		{
			ADocumentID	docID = NVM_GetWindowConst().NVI_GetDocumentIDByTabIndex(index);
			//�E�C���h�E�^�C�g���o�[�̃e�L�X�g���擾
			AText	text;
			GetApp().SPI_GetTextWindowByID(NVM_GetWindowConst().GetObjectID()).SPI_GetTitleText(docID,text);
			//�p�X�ݒ�
			outText1.SetText(text);
			outText2.SetText(text);
			//�^�O�\���ʒu�ݒ�i���i�̏ꍇ�͈�ԉ��j
			outRect = rect;
			outRect.bottom = viewRect.bottom;
			return true;
		}
	}
	return false;
}

/**
Drag&Drop�����iDrag���j
*/
void	AView_TabSelector::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, ABool& outIsCopyOperation )
{
	//#575
	if( mDragging == false )
	{
		//==================���̃^�u����h���b�O���ł͂Ȃ��ꍇ==================
		//
		ACursorWrapper::SetCursor(kCursorType_ArrowCopy);
	}
	else
	{
		//==================���̃^�u����h���b�O���̏ꍇ==================
		//Drag���̃^�u�\���ʒu�X�V
		UpdateDragging(inLocalPoint);
		//
		ACursorWrapper::SetCursor(kCursorType_Arrow);
		//�\���X�V
		NVI_Refresh();
	}
}

/**
Drag&Drop�����iDrag��View�ɓ������j
*/
void	AView_TabSelector::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mDragging == true )
	{
		//==================���̃^�u����h���b�O���̏ꍇ==================
		//Drag���̃^�u�\���ʒu�X�V
		UpdateDragging(inLocalPoint);
		//�\���X�V
		NVI_Refresh();
	}
}

/**
Drag&Drop�����iDrag��View����o���j
*/
void	AView_TabSelector::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�\���X�V
	NVI_Refresh();
}

/**
Drag���̃^�u�\���ʒu�X�V
*/
void	AView_TabSelector::UpdateDragging( const ALocalPoint& inLocalPoint )
{
	//Drop�ʒu�擾
	AIndex	dropDisplayIndex = FindDragIndex(inLocalPoint);
	if( mDraggingCurrentDisplayIndex != dropDisplayIndex )
	{
		//�\���ʒu�ړ�
		mDisplayOrderArray.Move(mDraggingCurrentDisplayIndex,dropDisplayIndex);
		//���ݕ\���ʒu�X�V
		mDraggingCurrentDisplayIndex = dropDisplayIndex;
	}
	//�h���b�O�ʒu�̃^�u���\���ɂ���
	mDraggingTabShouldHide = true;
}

/**
Drag&Drop�����iDrop�j
*/
void	AView_TabSelector::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//==================�t�@�C��Drop��M����==================
	AObjectArray<AFileAcc>	fileArray;
	AScrapWrapper::GetFileDragData(inDragRef,fileArray);
	for( AIndex i = 0; i < fileArray.GetItemCount(); i++ )
	{
		GetApp().SPNVI_CreateDocumentFromLocalFile(fileArray.GetObjectConst(i));
	}
	//�\���X�V
	NVI_Refresh();
}

//#364
/**
LocalPoint����ADrop�C���f�b�N�X���擾
*/
AIndex	AView_TabSelector::FindDragIndex( const ALocalPoint& inLocalPoint ) const
{
	AIndex	col = (inLocalPoint.x/*#850 +(mTabColumnWidth/2)*/) / mTabColumnWidth;
	AIndex	row = inLocalPoint.y / mTabRowHeight;
	AIndex	index = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
	if( index < 0 )   index = 0;
	if( index >= mDisplayOrderArray.GetItemCount() )   index = mDisplayOrderArray.GetItemCount()-1;
	return index;
}

//#164
/**
�^�u�����{�^���̕\���̈�擾
*/
void	AView_TabSelector::GetCloseButtonRect( const AIndex inTabDisplayIndex, ALocalRect& outRect ) const
{
	AItemCount	tabCountInOneRow = GetTabCountInOneRow(NVI_GetViewWidth());
	AIndex	column = inTabDisplayIndex%tabCountInOneRow;
	AIndex	row = inTabDisplayIndex/tabCountInOneRow;
	
	outRect.left 	= (column)*mTabColumnWidth + 2;//(column+1)*mTabColumnWidth - 18;
	outRect.right 	= (column)*mTabColumnWidth +18;//(column+1)*mTabColumnWidth - 2;
	outRect.top 	= row*mTabRowHeight + 1;;
	ANumber	margin = (/*#844 GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kTextWindowTabHeight)*/mTabRowHeight -16) /2 +1;
	if( margin < 0 )    margin = 0;
	outRect.top += margin;
	outRect.bottom 	= outRect.top + 16;
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

//
ANumber	AView_TabSelector::SPI_GetHeight( const ANumber inViewWidth ) const
{
	return GetTabRowCount(inViewWidth)*mTabRowHeight;
}

//
AItemCount	AView_TabSelector::GetTabRowCount( const ANumber inViewWidth ) const
{
	AItemCount	tabCountInOneRow = GetTabCountInOneRow(inViewWidth);
	if( tabCountInOneRow == 0 )   return 1;
	//#498 return (NVM_GetWindowConst().NVI_GetTabCount()+tabCountInOneRow-1)/tabCountInOneRow;
	return (mDisplayOrderArray.GetItemCount()+tabCountInOneRow-1)/tabCountInOneRow;//#498
}

//
AItemCount	AView_TabSelector::GetTabCountInOneRow( const ANumber inViewWidth ) const
{
	/*#844
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kMultipleRowTab) == false )
	{
		return 99999;
	}
	else
	*/
	{
		//B0000 0���Z�΍� ���1�s��tab����1�ȏ�ɂ��� return inViewWidth/mTabColumnWidth;
		AItemCount	result = inViewWidth/mTabColumnWidth;
		if( result <= 0 )   result = 1;
		return result;
	}
}

//R0177
void	AView_TabSelector::SPI_UpdateProperty()
{
	/*#844
	mTabRowHeight = static_cast<ANumber>(GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kTextWindowTabHeight));
	mTabRowHeight = 21;
	mTabColumnWidth = static_cast<ANumber>(GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kTextWindowTabWidth));
	mTabColumnWidth = 140;
	mFontSize = GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kTextWindowTabFontSize);
#if IMPLEMENTATION_FOR_WINDOWS
	mFontSize *= 0.8;
	if( mFontSize < 7.8 )   mFontSize = 7.8;
#endif
	*/
	//�^�u���ݒ� #1349
	mTabColumnWidth = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kTabWidth);
	//
	AFontWrapper::GetDialogDefaultFontName(mFontName);//#375
	NVMC_SetDefaultTextProperty(mFontName,mFontSize,kColor_Black,/*#732 kTextStyle_Normal*/kTextStyle_DropShadow,true);
	NVMC_GetMetricsForDefaultTextProperty(mFontHeight,mFontAscent);
	/*#164
	//R0228
	if( false )
	{
		ALocalRect	viewRect;
		NVM_GetLocalViewRect(viewRect);
		ALocalPoint	lpt;
		lpt.x = viewRect.right - mTabRowHeight +2;
		lpt.y = viewRect.top + 2;
		AWindowPoint	wpt;
		NVM_GetWindow().NVI_GetWindowPointFromControlLocalPoint(NVI_GetControlID(),lpt,wpt);
		NVM_GetWindow().NVI_SetControlPosition(kTabCloseButtonControlID,wpt);
		NVM_GetWindow().NVI_SetControlSize(kTabCloseButtonControlID,mTabRowHeight-4,mTabRowHeight-4);
		NVM_GetWindow().NVI_SetShowControl(kTabCloseButtonControlID,true);
	}
	*/
	//
	NVI_Refresh();
}

/*#164
//R0228
AIndex	AView_TabSelector::SPI_GetCurrentCloseButtonIndex() const
{
	if( mEnableTabCloseButton == true )
	{
		return mCurrentCloseButtonIndex;
	}
	else
	{
		return NVM_GetWindowConst().NVI_GetCurrentTabIndex();
	}
}
*/

//#498
/**
Tab�ǉ�������
*/
void	AView_TabSelector::SPI_DoTabCreated( const AIndex inTabIndex )
{
	//�폜�^�u����̃C���f�b�N�X�����炷
	for( AIndex i = 0; i < mDisplayOrderArray.GetItemCount(); i++ )
	{
		if( mDisplayOrderArray.Get(i) >= inTabIndex )
		{
			mDisplayOrderArray.Set(i,mDisplayOrderArray.Get(i)+1);
		}
	}
	//�ǉ��i�I���\�^�u�̏ꍇ�̂݁j
	if( NVM_GetWindow().NVI_GetTabSelectable(inTabIndex) == true )
	{
		mDisplayOrderArray.Add(inTabIndex);
	}
	//�`��X�V
	NVI_Refresh();
	//#688 MouseTrackingRect�X�V���ŏI�I�ɂ͎g�p����
	//UpdateMouseTrackingRect();
}

//#498
/**
Tab�폜������
*/
void	AView_TabSelector::SPI_DoTabDeleted( const AIndex inTabIndex )
{
	//�폜
	AIndex	index = mDisplayOrderArray.Find(inTabIndex);
	if( index != kIndex_Invalid )
	{
		mDisplayOrderArray.Delete1(index);
		
	}
	//�폜�^�u����̃C���f�b�N�X�����炷
	for( AIndex i = 0; i < mDisplayOrderArray.GetItemCount(); i++ )
	{
		if( mDisplayOrderArray.Get(i) > inTabIndex )
		{
			mDisplayOrderArray.Set(i,mDisplayOrderArray.Get(i)-1);
		}
	}
	//�`��X�V
	NVI_Refresh();
	//#688 MouseTrackingRect�X�V���ŏI�I�ɂ͎g�p����
	//UpdateMouseTrackingRect();
}

//#850
/**
�^�u�\��index�ݒ�
*/
void	AView_TabSelector::SPI_SetDisplayTabIndex( const AIndex inTabIndex, const AIndex inDisplayTabIndex )
{
	AIndex	oldIndex = mDisplayOrderArray.Find(inTabIndex);
	mDisplayOrderArray.Move(oldIndex,inDisplayTabIndex);
	NVI_Refresh();
}

//#1062
/**
�^�u�\������ݒ肷��
*/
void	AView_TabSelector::SPI_SetDisplayOrderArray( const AArray<AIndex>& inTabIndexArray )
{
	for( AIndex i = 0; i < inTabIndexArray.GetItemCount(); i++ )
	{
		mDisplayOrderArray.Set(i,inTabIndexArray.Get(i));
	}
	NVI_Refresh();
}

/**
MouseTrackingRect�X�V���ŏI�I�ɂ͎g�p�����i�݌v�ύX�j
*/
/*
void	AView_TabSelector::UpdateMouseTrackingRect()
{
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	
	//
	AArray<ALocalRect>	rectarray;
	//
	AItemCount	tabCountInOneRow = GetTabCountInOneRow(NVI_GetViewWidth());
	for( AIndex index = 0; index < mDisplayOrderArray.GetItemCount(); index++ )
	{
		//
		AIndex	column = index%tabCountInOneRow;
		AIndex	row = index/tabCountInOneRow;
		
		//
		ALocalRect	rect = viewRect;
		rect.left 		= column*mTabColumnWidth;
		rect.right 		= rect.left + mTabColumnWidth;
		rect.top 		= row*mTabRowHeight;
		rect.bottom 	= rect.top + mTabRowHeight;	
		
		//
		rectarray.Add(rect);
		
		//
		ALocalRect	closeButtonRect = {0};
		GetCloseButtonRect(index,closeButtonRect);
		rectarray.Add(closeButtonRect);
	}
	//
	NVM_GetWindow().NVI_SetMouseTrackingRect(NVI_GetControlID(),rectarray);
}
*/
//��control bounds�ύX����UpdateMouseTrackingRect()

