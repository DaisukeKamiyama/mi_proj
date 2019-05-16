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

AView_List

*/

#include "stdafx.h"

#include "../Frame.h"

const AControlID	kEditBoxControlID = 9999;
const ANumber	kEditBoxBottomMargin = 8;

#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kListDefaultFontSize = 12.0;
const AFloatNumber	kListFrameDefaultFontSize = 10.0;
#else
const AFloatNumber	kListDefaultFontSize = 9.0;
const AFloatNumber	kListFrameDefaultFontSize = 9.0;
#endif

//���X�g���ڃA�C�R���̕��A�}�[�W��
const ANumber	kImageIconWidth = 16;
const ANumber	kImageIconLeftMargin = 4;
const ANumber	kImageIconTopMargin = 1;//#1316 0;

//�A�E�g���C���P���x���ӂ�̕�
const AFloatNumber	kOutlineWidthPerLevel = 0.5;

//�w�b�_����
const ANumber				kHeaderHeight = 18;

#pragma mark ===========================
#pragma mark [�N���X]AView_List
#pragma mark ===========================
//�C���f�b�N�X�E�C���h�E�̃��C��View

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_List::AView_List( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID ) 
: AView(/*#199 inParent,inWindow*/inWindowID,inID), mSelectedRowDisplayIndex(kIndex_Invalid), 
		mCursorRowStartDisplayIndex(kIndex_Invalid),mCursorRowEndDisplayIndex(kIndex_Invalid), mDragDrawn(false), 
		mEnableCursorRow(false), mEnableFileIcon(false), mEnableDragDrop(false), mRowMargin(kDefaultRowMargin), /*#1012 mEnableIcon(false),*/
		mEnableContextMenu(false), mWheelScrollPercent(100), mWheelScrollPercentHighSpeed(100),//R0186, R0000
		mEditMode(false), /*#205 mFrameControlID(kControlID_Invalid),*///win 080618
		mFrameViewID(kObjectID_Invalid),//#205
		/*#688 mMouseTrackByLoop(true),*/ mMouseTrackingMode(kMouseTrackingMode_None)//win 080728
		,mRowHeight(9)
		,mInhibit0Length(true)//#205
//#725,mBackGroundImageIndex(kIndex_Invalid), mBackGroundImageAlpha(0.5)//#291
		,mTransparency(1.0), mAlwaysActiveColors(false)//#291
		,mMultiSelectEnalbed(false)//#237
,mBackgroundColor(kColor_White),mBackgroundColorDeactivate(kColor_White)//#634
,mFontSize(9.0)
,mDisplayRowOrderMode(kDisplayRowOrderMode_Normal)//#871
,mSortMode(false), mSortAscendant(false)//#871
,mCursorRowHoverFolding(false)//#130
,mContextMenuSelectedRowDisplayIndex(kIndex_Invalid)//#892
,mOutlineLevelDataExist(false)//#130
,mVScrollBarControlID(kControlID_Invalid),mHScrollBarControlID(kControlID_Invalid)
,mFilterColumnID(kIndex_Invalid),mSortColumnID(kIndex_Invalid)
,mEnableImageIcon(false)
,mEllipsisFirstCharacters(false)
,mCursorRowLeftMargin(0)
{
	//win SPI_SetTextDrawProperty(AFontWrapper::GetDefaultFont(),12.0,kColor_Black);
	/*win 080707 Init�ֈړ�
	AText	fontname;
	AFontWrapper::GetDefaultFontName(fontname);
	SPI_SetTextDrawProperty(fontname,12.0,kColor_Black);
	mDataBase.CreateTableStart();
	mDataBase.CreateData_TextArray(kFilePathText,"","");
	mDataBase.CreateData_NumberArray(kIconID,"",0,-1,9999999,false);
	mIconDataBase.CreateTableStart();//win 080618
	*/
	NVMC_SetOffscreenMode(true);
}

//�f�X�g���N�^
AView_List::~AView_List()
{
}

//Init 080707
void	AView_List::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);//#375
	SPI_SetTextDrawProperty(fontname,/*win 080707 12.0*/kListDefaultFontSize,kColor_Black);
	mDataBase.CreateTableStart();
	//������B�����̗�͕`�悳��Ȃ��imColumnIDArray�ɒǉ�����Ȃ��̂ŕ`�悳��Ȃ��j
	mDataBase.CreateData_TextArray(kFilePathText,"","");
	mDataBase.CreateData_NumberArray(kIconID,"",0,-1,9999999);//#695 ,false);
	mDataBase.CreateData_BoolArray(kEnabled,"",true);//#232
	mDataBase.CreateData_NumberArray(kTextStyle,"",static_cast<ANumber>(kTextStyle_Normal),0,0xFFFF);//#695
	mDataBase.CreateData_ColorArray(kColor,"",kColor_Black);//#695
	mDataBase.CreateData_ColorArray(kBackgroundColor,"",mBackgroundColor);//#695
	mDataBase.CreateData_BoolArray(kBackgroundColorEnabled,"",false);//#695
	mDataBase.CreateData_BoolArray(kRowEditable,"",true);//#695
	mDataBase.CreateData_NumberArray(kOutlineFoldingLevel,"",0,0,9999);//#130
	mDataBase.CreateData_BoolArray(kOutlineFoldingCollapsed,"",false);//#130
	mDataBase.CreateData_NumberArray(kImageIconID,"",kImageID_Invalid,-1,9999999);//#725
	//
	//#1012 mIconDataBase.CreateTableStart();//win 080618
	//#138
	NVIDO_ListInit();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_List::NVIDO_DoDeleted()
{
	SPI_EndEditMode();
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

/**
List�t���[��View�擾
*/
AView_ListFrame&	AView_List::GetFrameView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_ListFrame,kViewType_ListFrame,mFrameViewID);
}

#pragma mark ===========================

void	AView_List::SPI_SetEnableCursorRow()
{
	mEnableCursorRow = true;
}

void	AView_List::SPI_SetEnableFileIcon( const ABool inEnable )
{
	mEnableFileIcon = inEnable;
}

/*#1012
void	AView_List::SPI_SetEnableIcon( const ABool inEnable )
{
	mEnableIcon = inEnable;
}
*/

/**
���ڃA�C�R��Enable/disable�ݒ�
*/
void	AView_List::SPI_SetEnableImageIcon( const ABool inEnable )
{
	mEnableImageIcon = inEnable; 
}

void	AView_List::SPI_SetEnableDragDrop( const ABool inEnable )//B0319
{
	mEnableDragDrop = inEnable;
	//#236
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_ListViewItem);
	scrapTypeArray.Add(kScrapType_File);//win
	NVMC_EnableDrop(scrapTypeArray);
}

void	AView_List::SPI_SetRowMargin( const ANumber inRowMargin )
{
	mRowMargin = inRowMargin;
	AText	fontname;//win
	fontname.SetText(mFontName);//win
	SPI_SetTextDrawProperty(/*win mFont*/fontname,mFontSize,mColor);
}

//R0186
void	AView_List::SPI_SetEnableContextMenu( const ABool inEnable, const AMenuItemID inMenuItemID )
{
	mEnableContextMenu = inEnable;
	mContextMenuItemID = inMenuItemID;
}

//R0000
void	AView_List::SPI_SetWheelScrollPercent( const ANumber inNormal, const ANumber inHighSpeed )
{
	mWheelScrollPercent = inNormal;
	mWheelScrollPercentHighSpeed = inHighSpeed;
}

/*#205
//win 080618
void	AView_List::SPI_SetFrameControlID( const AControlID inFrameControlID )
{
	mFrameControlID = inFrameControlID;
}
*/

//#205
/**
List�t���[����ViewID��ݒ肷��

�{�N���X�O��List�t���[���𐶐������ꍇ�p�B
AWindow��TableView�@�\�ł́AAWindow��List�t���[���𐶐����Ă���B
*/
void	AView_List::SPI_SetFrameViewID( const AViewID inViewID )
{
	mFrameViewID = inViewID;
}

//#205
/**
List�t���[���𐶐�����

�{ListView�̎���ɁAList�t���[���𐶐�����B
�{ListView�͎w��Margin�������������Ȃ�B
*/
void	AView_List::SPI_CreateFrame( const ABool inCreateHeader, const ABool inCreateVScrollBar, const ABool inCreateHScrollBar,
		const ANumber inLeftMargin, const ANumber inTopMargin, const ANumber inRightMargin, const ANumber inBottomMargin )
{
	//ListView�̌��̈ʒu�E�T�C�Y���擾���AListView�̃T�C�Y�͏k������B
	AWindowPoint	origPoint = NVI_GetPosition();
	ANumber	origWidth = NVI_GetWidth();
	ANumber	origHeight = NVI_GetHeight();
	AWindowPoint	insetPoint;
	insetPoint.x = origPoint.x + inLeftMargin;
	insetPoint.y = origPoint.y;
	ANumber	insetWidth = origWidth - inLeftMargin - inRightMargin;
	ANumber	insetHeight = origHeight - inBottomMargin;
	//�t���[���������A�����̃T�C�Y������������
	if( inCreateHeader == true )
	{
		insetPoint.y += kHeaderHeight;
		insetHeight -= kHeaderHeight;
	}
	else
	{
		insetPoint.y += inTopMargin;
		insetHeight -= inTopMargin;
	}
	NVM_GetWindow().NVI_SetControlPosition(NVI_GetControlID(),insetPoint);
	//
	ANumber	listviewWidth = insetWidth;
	ANumber	listviewHeight = insetHeight;
	//ListView�����݂���TabControl��Index���擾
	AIndex	tabControlIndex = NVM_GetWindow().NVI_GetControlEmbeddedTabControlIndex(NVI_GetControlID());
	//Frame����
	AControlID	frameControlID = NVM_GetWindow().NVM_AssignDynamicControlID();
	AListFrameViewFactory	frameViewFactory(NVM_GetWindowID(),frameControlID,NVI_GetControlID(),inCreateHeader,kControlID_Invalid,kControlID_Invalid);
	mFrameViewID = NVM_GetWindow().NVI_CreateView(frameControlID,origPoint,origWidth,origHeight,kControlID_Invalid,
				NVI_GetControlID(),false,frameViewFactory,tabControlIndex,false);
	//�t���[������embed���� win
	NVM_GetWindow().NVI_EmbedControl(frameControlID,NVI_GetControlID());
	//�����ɂ���ăX�N���[���o�[�𐶐�����
	if( inCreateVScrollBar == true )
	{
		//ListView�̌��̈ʒu�E�T�C�Y���擾���AListView�̃T�C�Y�͏k������B
		AWindowPoint	pt;
		pt.x = insetPoint.x + insetWidth - kVScrollBarWidth;
		pt.y = insetPoint.y;
		ANumber	width = kVScrollBarWidth;
		ANumber	height = insetHeight;
		if( inCreateHScrollBar == true )
		{
			height -= kHScrollBarHeight;
		}
		mVScrollBarControlID = NVM_GetWindow().NVM_AssignDynamicControlID();
		//V�X�N���[���o�[����
		NVM_GetWindow().NVI_CreateScrollBar(mVScrollBarControlID,pt,width,height,tabControlIndex);
		//V�X�N���[���o�[������listview�̃T�C�Y���k�߂�
		listviewWidth -= kVScrollBarWidth;
		//�t���[������embed���� win
		NVM_GetWindow().NVI_EmbedControl(frameControlID,mVScrollBarControlID);
	}
	if( inCreateHScrollBar == true )
	{
		//ListView�̌��̈ʒu�E�T�C�Y���擾���AListView�̃T�C�Y�͏k������B
		AWindowPoint	pt;
		pt.x = insetPoint.x;
		pt.y = insetPoint.y + insetHeight - kHScrollBarHeight;
		ANumber	width = insetWidth;
		if( inCreateVScrollBar == true )
		{
			width -= kVScrollBarWidth;
		}
		ANumber	height = kHScrollBarHeight;
		mHScrollBarControlID = NVM_GetWindow().NVM_AssignDynamicControlID();
		//H�X�N���[���o�[����
		NVM_GetWindow().NVI_CreateScrollBar(mHScrollBarControlID,pt,width,height,tabControlIndex);
		//H�X�N���[���o�[������listview�̃T�C�Y���k�߂�
		listviewHeight -= kHScrollBarHeight;
		//�t���[������embed���� win
		NVM_GetWindow().NVI_EmbedControl(frameControlID,mHScrollBarControlID);
	}
	//
	NVI_SetSize(listviewWidth,listviewHeight);
	//�X�N���[���o�[�ւ̃����N��ݒ�
	NVI_SetScrollBarControlID(mHScrollBarControlID,mVScrollBarControlID);
}

/**
bindings�ݒ�
*/
void	AView_List::SPI_SetControlBindings( const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
		const ABool inWidthFixed, const ABool inHeightFixed )
{
	if( mFrameViewID != kObjectID_Invalid )
	{
		//------------------�t���[���L��̏ꍇ------------------
		//Frame view�̕���Bind�ݒ肷��
		NVM_GetWindow().NVI_SetControlBindings(GetFrameView().NVI_GetControlID(),
					inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
		//H�X�N���[���o�[��bindings�ݒ�
		if( mHScrollBarControlID != kControlID_Invalid )
		{
			NVM_GetWindow().NVI_SetControlBindings(mHScrollBarControlID,true,false,true,true,false,true);
		}
		//V�X�N���[���o�[��bindings�ݒ�
		if( mVScrollBarControlID != kControlID_Invalid )
		{
			NVM_GetWindow().NVI_SetControlBindings(mVScrollBarControlID,false,true,true,true,true,false);
		}
		//���g�̓t���[���r���[�̍��E�㉺��binding����B�i�T�C�Y�̓t���L�V�u���j
		NVMC_SetControlBindings(true,true,true,true,false,false);
	}
	else
	{
		//------------------�t���[�������̏ꍇ------------------
		//���̂܂ܐݒ�
		NVMC_SetControlBindings(inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
	}
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_List::EVTDO_DoDraw()
{
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	ALocalRect	localFrame;
	NVM_GetLocalViewRect(localFrame);
	
	/*#1316 AView_List�ł͔w�i�`�悵�Ȃ��B�iFrame�L��̏ꍇ��Frame�Ŕw�i�`��A�����̏ꍇ�͓����j
	//==================�w�i�f�[�^�擾�i����view�ł͔w�i�`�悵�Ȃ��j==================
	
	//�w�i�F�擾
	AColor	backgroundColor = mBackgroundColor;
	if( NVM_GetWindow().NVI_IsWindowActive() == false )
	{
		backgroundColor = mBackgroundColorDeactivate;
	}
	*/
	//�w�i�`��
	//NVMC_EraseRect(localFrame);//Windows�p�i�����F�`��j
	//NVMC_PaintRect(localFrame,backgroundColor,mTransparency);//#291
	
	
	//==================�z�o�[�`��==================
	if( mEnableCursorRow == true &&
	   mCursorRowStartDisplayIndex != kIndex_Invalid && mCursorRowEndDisplayIndex != kIndex_Invalid )
	{
		AImageRect	hoverImageRect = {0};
		GetRowImageRect(mCursorRowStartDisplayIndex,mCursorRowEndDisplayIndex+1,hoverImageRect);
		ALocalRect	hoverLocalRect = {0};
		NVM_GetLocalRectFromImageRect(hoverImageRect,hoverLocalRect);
		hoverLocalRect.left		+= 2 + mCursorRowLeftMargin;
		hoverLocalRect.top		+= 2;
		hoverLocalRect.right	-= 2;
		hoverLocalRect.bottom	-= 2;
		NVMC_FrameRoundedRect(hoverLocalRect,AColorWrapper::GetControlAccentColor(),0.7,
							  3,true,true,true,true,true,true,true,true,1.0);
	}
	
	//==================�e�s�`��==================
	
	AImageRect	rect;
	NVM_GetImageViewRect(rect);
	AIndex	startRow = rect.top / GetRowHeight();
	AIndex	endRow = rect.bottom / GetRowHeight();
	
	//#232 ABool	enabled =  ((NVM_GetWindow().IsWindowActive() == true) || (NVM_GetWindow().NVI_IsFloatingWindow() == true))&&(NVMC_IsControlEnabled());
	AItemCount	rowDisplayCount = SPI_GetDisplayRowCount();//#798 GetRowCount();
	for( AIndex rowDisplayIndex = startRow; rowDisplayIndex <= endRow; rowDisplayIndex++ )
	{
		AImageRect	rowImageRect;
		GetRowImageRect(rowDisplayIndex,rowDisplayIndex+1,rowImageRect);
		ALocalRect	rowLocalRect;
		NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
		if( NVMC_IsRectInDrawUpdateRegion(rowLocalRect) == false )   continue;
		
		//�s�\���N���A
		//NVMC_PaintRect(rowLocalRect,kColor_White,1.0);
		
		if( rowDisplayIndex >= rowDisplayCount )   break;
		
		AIndex	DBRowIndex = mRowOrderDisplayToDBArray.Get(rowDisplayIndex);
		
		//#232 �s��Enable/Disable
		ABool	enabled =  ((NVM_GetWindow().NVI_IsWindowActive() == true) || 
				(NVM_GetWindow().NVI_IsFloating() == true))&&(NVMC_IsControlEnabled());
		if( mAlwaysActiveColors == true )//#291
		{
			enabled = true;
		}
		if( enabled == true )
		{
			enabled = mDataBase.GetData_BoolArray(kEnabled,DBRowIndex);;
		}
		
		//#237
		//�I���s���ǂ����̔���
		ABool	selected = (rowDisplayIndex==mSelectedRowDisplayIndex);
		if( mSelectedRowDisplayIndexArray.GetItemCount() > 0 )
		{
			if( mSelectedRowDisplayIndexArray.Find(rowDisplayIndex) != kIndex_Invalid )
			{
				selected = true;
			}
		}
		
		//==================Selection�`��==================
		if( selected == true )//#237
		{
			AFloatNumber	alpha = 0.7;
			if( AApplication::GetApplication().NVI_IsDarkMode() == true )
			{
				alpha = 0.4;
			}
			NVMC_PaintRect(rowLocalRect,AColorWrapper::GetControlAccentColor(),alpha);
			/*#1316
			//�I��F�擾
			AColor	selColor1 = kColor_Blue, selColor2 = kColor_Blue, selColor3 = kColor_Blue;
			if( NVM_GetWindow().NVI_IsWindowActive() == true ) 
			{
				//activated��
				selColor1 = AColorWrapper::GetColorByHTMLFormatColor("5c91d2");
				selColor2 = AColorWrapper::GetColorByHTMLFormatColor("71a2dc");
				selColor3 = AColorWrapper::GetColorByHTMLFormatColor("2966b9");
			}
			else
			{
				//deactivated��
				selColor1 = AColorWrapper::GetColorByHTMLFormatColor("a8a8a8");
				selColor2 = AColorWrapper::GetColorByHTMLFormatColor("c1c1c1");
				selColor3 = AColorWrapper::GetColorByHTMLFormatColor("9c9c9c");
			}
			NVMC_PaintRectWithVerticalGradient(rowLocalRect,selColor2,selColor3,1.0,1.0);
			//�I�𕔕��̍ŏ㕔1pixel��`��
			ALocalRect	rect = rowLocalRect;
			rect.bottom = rect.top+1;
			NVMC_PaintRect(rect,selColor,1.0);
			*/
		}
		
		/*
		//�w�i�F #427
		if( selected == false )
		{
			if( mDataBase.GetData_BoolArray(kBackgroundColorEnabled,DBRowIndex) == true )
			{
				AColor	c = mBackgroundColor;
				mDataBase.GetData_ColorArray(kBackgroundColor,DBRowIndex,c);
				NVMC_PaintRect(rowLocalRect,c,1.0);
			}
		}
		*/
		
		ANumber	imagex = kRowLeftMargin;
		
		//#130
		//==================folding�A�C�R���`��L������уA�E�g���C�����x���]���̈�擾==================
		ALocalRect	foldingRect = {0};
		ABool	hasFolding = GetOutlineFoldingAndOutlineMarginRect(rowDisplayIndex,foldingRect);
		
		//==================Folding�O�p�A�C�R���`��==================
		if( hasFolding == true )
		{
			//�\���ʒu�擾
			ALocalPoint	pt = {0};
			pt.x = foldingRect.right - 18;
			pt.y = (foldingRect.top+foldingRect.bottom)/2 - 6;
			//�܂肽���ݏ�Ԃɂ�镪��
			if( mDataBase.GetData_BoolArray(kOutlineFoldingCollapsed,DBRowIndex) == true )
			{
				//�܂肽���ݒ�
				if( rowDisplayIndex >= mCursorRowStartDisplayIndex && rowDisplayIndex <= mCursorRowEndDisplayIndex &&
				   mCursorRowHoverFolding == true )
				{
					//�z�o�[
					NVMC_DrawImage(kImageID_barSwCursorRight_h,pt);
				}
				else
				{
					//�ʏ�
					NVMC_DrawImage(kImageID_barSwCursorRight,pt);
				}
			}
			else
			{
				//�܂肽���݉�����
				if( rowDisplayIndex >= mCursorRowStartDisplayIndex && rowDisplayIndex <= mCursorRowEndDisplayIndex &&
				   mCursorRowHoverFolding == true )
				{
					//�z�o�[
					NVMC_DrawImage(kImageID_barSwCursorDown_h,pt);
				}
				else
				{
					//�ʏ�
					NVMC_DrawImage(kImageID_barSwCursorDown,pt);
				}
			}
		}
		//X�ʒu��folding�A�C�R���̉E��
		imagex += foldingRect.right - foldingRect.left - kCellLeftMargin - kCellRightMargin;
		//==================�t�@�C���A�C�R���\��==================
		if( mEnableFileIcon == true )
		{
			AText	path;
			mDataBase.GetData_TextArray(kFilePathText,DBRowIndex,path);
			AFileAcc	file;
			file.Specify(path);
			AImageRect	cellImageRect = rowImageRect;
			cellImageRect.left		= imagex;
			cellImageRect.right		= imagex + GetRowHeight() +kCellLeftMargin +kCellRightMargin;
			ALocalRect	drawRect = {0};
			NVM_GetLocalRectFromImageRect(cellImageRect,drawRect);
			drawRect.top += mRowMargin/2 -1;//#232
			drawRect.left += kCellLeftMargin;
			drawRect.right = drawRect.left + (drawRect.bottom-drawRect.top);
			if( file.Exist() == true )//#1316
			{
				NVMC_DrawIconFromFile(drawRect,file,enabled);
			}
			imagex = cellImageRect.right;
		}
		/*#1012
		//==================���K�V�[�A�C�R���\��==================
		if( mEnableIcon == true )
		{
			AIconID	iconID = mDataBase.GetData_NumberArray(kIconID,DBRowIndex);
			AImageRect	cellImageRect = rowImageRect;
			cellImageRect.left		= imagex;
			cellImageRect.right		= imagex + GetRowHeight() +kCellLeftMargin +kCellRightMargin;
			ALocalRect	drawRect = {0};
			NVM_GetLocalRectFromImageRect(cellImageRect,drawRect);
			drawRect.top += mRowMargin/2 -1;//#232
			drawRect.left += kCellLeftMargin;
			drawRect.right = drawRect.left + (drawRect.bottom-drawRect.top);
			NVMC_DrawIcon(drawRect,iconID,enabled);
			imagex = cellImageRect.right;
		}
		*/
		//#725
		//==================���ڃA�C�R���\��==================
		if( mEnableImageIcon == true )
		{
			AImageID	imageIconID = mDataBase.GetData_NumberArray(kImageIconID,DBRowIndex);
			if( imageIconID != kImageID_Invalid )
			{
				AImagePoint	imagept = {imagex+kImageIconLeftMargin,rowImageRect.top+kImageIconTopMargin};
				ALocalPoint	pt = {0};
				NVM_GetLocalPointFromImagePoint(imagept,pt);
				NVMC_DrawImage(imageIconID,pt);
				imagex = imagept.x + kImageIconWidth;
			}
		}
		//==================�e�񂲂Ƃ̃��[�v==================
		for( AIndex colIndex = 0; colIndex < mColumnIDArray.GetItemCount(); colIndex++ )
		{
			//
			APrefID	colID = mColumnIDArray.Get(colIndex);
			AImageRect	cellImageRect = rowImageRect;
			cellImageRect.left		= imagex;
			cellImageRect.right		= imagex + mColumnWidthArray.Get(colIndex);
			//�Ō�̗�͉E�[�܂�
			if( colIndex == mColumnIDArray.GetItemCount() - 1 )
			{
				cellImageRect.right = rowImageRect.right;
			}
			//�`��̈�擾
			ALocalRect	drawRect = {0};
			NVM_GetLocalRectFromImageRect(cellImageRect,drawRect);
			drawRect.top += mRowMargin/2 + 0.1;//#1316 -1;
			drawRect.left += kCellLeftMargin;
			drawRect.right -= kCellRightMargin;
			drawRect.bottom += 32;
			//
			switch(mDataBase.GetDataType(colID))
			{
				//==================�e�L�X�g��==================
			  case kDataType_TextArray:
				{
					//------------------�A�C�R���擾------------------
					/*#1012
					if( mIconDataBase.GetItemCount_NumberArray(colID) > 0 )
					{
						AIconID	iconID = mIconDataBase.GetData_NumberArray(colID,DBRowIndex);
						if( iconID != kIndex_Invalid )
						{
							//
							ALocalRect	iconDrawRect = {0};
							NVM_GetLocalRectFromImageRect(cellImageRect,iconDrawRect);
							iconDrawRect.top	+= mRowMargin/2 -1;//#232
							iconDrawRect.left	+= kCellLeftMargin;
							iconDrawRect.right	= drawRect.left + (iconDrawRect.bottom-iconDrawRect.top);
							NVMC_DrawIcon(iconDrawRect,iconID,true,true);//#232
							//
						}
						drawRect.left += GetRowHeight();
					}
					*/
					
					//------------------�e�L�X�g�擾------------------
					//�e�L�X�g�擾
					AText	text;
					mDataBase.GetData_TextArray(colID,DBRowIndex,text);
					//Canonical comp�ɕϊ�
					text.ConvertToCanonicalComp();
					/*
					AIndex	outlineLevel = mDataBase.GetData_NumberArray(kOutlineFoldingLevel,DBRowIndex);
					AText	t;
					t.SetNumber(outlineLevel);
					text.InsertText(0,t);
					*/
					//�F�擾
					AColor	color = kColor_List_Normal;//#1316 kColor_Black;
					mDataBase.GetData_ColorArray(kColor,DBRowIndex,color);//#695
					//#1316
					//kColor_List_�J���[���_�[�N���[�h���ǂ����ɏ]���āA���ۂ̐F�ɕϊ�
					if( AColorWrapper::CompareColor(color,kColor_List_Normal) == true )
					{
						if( selected == true )
						{
							color = kColor_White;
						}
						else
						{
							color = AColorWrapper::GetColorByHTMLFormatColor("303030");
							if( AApplication::GetApplication().NVI_IsDarkMode() == true )
							{
								color = AColorWrapper::GetColorByHTMLFormatColor("C0C0C0");
							}
						}
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Blue) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("0000FF");
						if( AApplication::GetApplication().NVI_IsDarkMode() == true )
						{
							color = AColorWrapper::GetColorByHTMLFormatColor("00C0FF");
						}
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Red) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("FF0000");
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Pink) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("FF00C3");
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Gray) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("757575");
					}
					
					//�X�^�C���擾
					ATextStyle	style = static_cast<ATextStyle>(mDataBase.GetData_NumberArray(kTextStyle,DBRowIndex));//#695
					//�I���s�̓{�[���h�� #1316
					if( selected == true )
					{
						style |= kTextStyle_Bold;
					}
					/*#1316
					if( selected == true )
					{
						//------------------�I���s�̏ꍇ------------------
						//�����F�����̏ꍇ�́A���ɂ���
						if( AColorWrapper::CompareColor(color,kColor_Black) == true )//R0206
						{
							color = kColor_White;
						}
						//�����F�����ȊO�̏ꍇ�́AS�������AV��������
						else
						{
							color = AColorWrapper::ChangeHSV(color,0,0.05,100000);
						}
						//�h���b�v�V���h�E�F�ݒ�
						NVMC_SetDropShadowColor(kColor_Gray30Percent);
						//bold�ɐݒ�
						//#1089 style |= kTextStyle_Bold;
					}
					*/
					//#1428
					//�ݒ�f�[�^�̕����񒷂�0�̂Ƃ��͑�փe�L�X�g��\������
					if( text.GetItemCount() == 0 )
					{
						text.SetText(m0LengthText);
						color = AColorWrapper::GetColorByHTMLFormatColor("808080");
						style = kTextStyle_Italic;
					}
					//�e�L�X�g�v���p�e�B�ݒ�
					NVMC_SetDefaultTextProperty(mFontName,mFontSize,color,style,true);
					//ellipsis�e�L�X�g�擾
					AText	ellipsisText;
					if( mEllipsisFirstCharacters == true )
					{
						NVI_GetEllipsisTextWithFixedFirstCharacters(text,drawRect.right-drawRect.left,3,ellipsisText);
					}
					else
					{
						NVI_GetEllipsisTextWithFixedLastCharacters(text,drawRect.right-drawRect.left,3,ellipsisText,true);
					}
					//�e�L�X�g�`��
					NVMC_DrawText(drawRect,ellipsisText,color,style);
					break;
				}
				//==================�ԍ���==================
			  case kDataType_NumberArray:
				{
					//�e�L�X�g�擾
					AText	text;
					text.SetNumber(mDataBase.GetData_NumberArray(colID,DBRowIndex));
					//�F�擾
					AColor	color = kColor_Black;
					mDataBase.GetData_ColorArray(kColor,DBRowIndex,color);//#695
					//#1316
					//kColor_List_�J���[���_�[�N���[�h���ǂ����ɏ]���āA���ۂ̐F�ɕϊ�
					if( AColorWrapper::CompareColor(color,kColor_List_Normal) == true )
					{
						if( selected == true )
						{
							color = kColor_White;
						}
						else
						{
							color = AColorWrapper::GetColorByHTMLFormatColor("303030");
							if( AApplication::GetApplication().NVI_IsDarkMode() == true )
							{
								color = AColorWrapper::GetColorByHTMLFormatColor("C0C0C0");
							}
						}
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Blue) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("0000FF");
						if( AApplication::GetApplication().NVI_IsDarkMode() == true )
						{
							color = AColorWrapper::GetColorByHTMLFormatColor("00C0FF");
						}
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Red) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("FF0000");
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Pink) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("FF00C3");
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Gray) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("757575");
					}
					
					//�X�^�C���擾
					ATextStyle	style = static_cast<ATextStyle>(mDataBase.GetData_NumberArray(kTextStyle,DBRowIndex));//#695
					//�I���s�̓{�[���h�� #1316
					if( selected == true )
					{
						style |= kTextStyle_Bold;
					}
					/*#1316
					if( selected == true )//#237
					{
						//------------------�I���s�̏ꍇ------------------
						//�����F�����̏ꍇ�́A���ɂ���
						if( AColorWrapper::CompareColor(color,kColor_Black) == true )//R0206
						{
							color = kColor_White;
						}
						//�����F�����ȊO�̏ꍇ�́AS�������AV��������
						else
						{
							color = AColorWrapper::ChangeHSV(color,0,0.05,100000);
						}
						//�h���b�v�V���h�E�F�ݒ�
						NVMC_SetDropShadowColor(kColor_Gray30Percent);
						//bold�ɐݒ�
						style |= kTextStyle_Bold;
					}
					*/
					//�e�L�X�g�`��
					NVMC_DrawText(drawRect,text,color,style);
					break;
				}
			  case kDataType_BoolArray:
				{
					//#1012 AIconID	iconID = kIconID_CheckBoxOn;
					AImageID	imageID = kImageID_iconChecked;//#1012
					if( mDataBase.GetData_BoolArray(colID,DBRowIndex) == false )
					{
						//#1012 iconID = kIconID_CheckBoxOff;
						imageID = kImageID_iconUnchecked;//#1012
					}
					ALocalRect	drawRect = {0};
					NVM_GetLocalRectFromImageRect(cellImageRect,drawRect);
					drawRect.top += (drawRect.bottom-drawRect.top)/2 - 8;//#1316 mRowMargin/2 -1;
					drawRect.left += kCellLeftMargin;
					drawRect.right = drawRect.left + (drawRect.bottom-drawRect.top);
					drawRect.bottom = drawRect.top + 16;//#1316
					//#1012 NVMC_DrawIcon(drawRect,iconID,enabled,true);//#232
					ALocalPoint	pt = {0};
					pt.x = drawRect.left;
					pt.y = drawRect.top;
					NVMC_DrawImage(imageID,pt);
					break;
				}
			  default:
				{
					_ACError("",NULL);
					break;
				}
			}
			imagex += mColumnWidthArray.Get(colIndex);
		}
	}
	//#871
	//==================Drop���`��==================
	if( mDragDrawn == true && mDragCaretRowDisplayIndex != mSelectedRowDisplayIndex && mDragCaretRowDisplayIndex != mSelectedRowDisplayIndex+1 )
	{
		//�F�擾
		AColor	dragColor = AColorWrapper::GetColorByHTMLFormatColor("526cdc");
		//���`��
		AImageRect	imageframe = {0};
		NVM_GetImageViewRect(imageframe);
		AImagePoint	caretImageStart, caretImageEnd;
		caretImageStart.y = caretImageEnd.y = mDragCaretRowDisplayIndex*GetRowHeight();
		caretImageStart.x = imageframe.left + 9;
		caretImageEnd.x = imageframe.right - 2;
		ALocalPoint	caretLocalStart = {0}, caretLocalEnd = {0};
		NVM_GetLocalPointFromImagePoint(caretImageStart,caretLocalStart);
		NVM_GetLocalPointFromImagePoint(caretImageEnd,caretLocalEnd);
		NVMC_DrawLine(caretLocalStart,caretLocalEnd,dragColor,1.0,0,2.0);
		//�ە`��
		ALocalRect	circleRect = {0};
		circleRect.left		= caretLocalStart.x -7;
		circleRect.top		= caretLocalStart.y - 3;
		circleRect.right	= caretLocalStart.x;
		circleRect.bottom	= caretLocalStart.y + 4;
		NVMC_DrawCircle(circleRect,dragColor,2);
	}
}

//#130
/**
folding�A�C�R���`��L������уA�E�g���C�����x���]���̈�擾
*/
ABool	AView_List::GetOutlineFoldingAndOutlineMarginRect( const AIndex inDisplayRowIndex, ALocalRect& outRect ) const
{
	//�srect�擾
	AImageRect	rowImageRect = {0};
	GetRowImageRect(inDisplayRowIndex,inDisplayRowIndex+1,rowImageRect);
	
	//�A�E�g���C���f�[�^�����̂Ƃ��́A���]���Ȃ�
	if( mOutlineLevelDataExist == false )
	{
		AImageRect	foldingImageRect = rowImageRect;
		foldingImageRect.right = kRowLeftMargin;
		NVM_GetLocalRectFromImageRect(foldingImageRect,outRect);
		return false;
	}
	
	//�A�E�g���C�����x���擾
	AIndex	DBRowIndex = mRowOrderDisplayToDBArray.Get(inDisplayRowIndex);
	AIndex	outlineLevel = mDataBase.GetData_NumberArray(kOutlineFoldingLevel,DBRowIndex);
	//�A�E�g���C�����x���]���̈�擾
	AImageRect	foldingImageRect = rowImageRect;
	foldingImageRect.right = kRowLeftMargin + 11 + outlineLevel*kOutlineWidthPerLevel;
	NVM_GetLocalRectFromImageRect(foldingImageRect,outRect);
	//folding�A�C�R���`��L������
	if( HasFoldingChild(DBRowIndex) == true )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_List::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//�R���g���[����disable�Ȃ牽�������I��
	if( NVMC_IsControlEnabled() == false )
	{
		return false;
	}
	
	//��View��Active�łȂ��ꍇ�́AWindow�փt�H�[�J�X�̈ړ����˗�����
	if( NVI_IsFocusActive() == false )
	{
		NVM_GetWindow().NVI_SwitchFocusTo(NVI_GetControlID());
	}
	
	//==================�N���b�N�ʒu�擾==================
	AImagePoint	clickImagePoint = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	//�N���b�N�s�擾
	AIndex	clickRowDisplayIndex = clickImagePoint.y/GetRowHeight();
	if( clickRowDisplayIndex >= SPI_GetDisplayRowCount() )
	{
		//�s�̍Ō������N���b�N�����ꍇ�́A�I������
		SPI_SetSelect(kIndex_Invalid);
		//AWindow�̃e�[�u���I����Ԃ��������邽�߂�EVT_Clicked()���R�[������
		NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
		return true;
	}
	
	//#130
	//==================Folding�A�C�R���N���b�N==================
	ALocalRect	foldingRect = {0};
	if( clickRowDisplayIndex >= 0 && clickRowDisplayIndex < SPI_GetDisplayRowCount() )
	{
		ABool	hasFolding = GetOutlineFoldingAndOutlineMarginRect(clickRowDisplayIndex,foldingRect);
		if( hasFolding == true )
		{
			if( IsPointInRect(inLocalPoint,foldingRect) == true )
			{
				//------------------Folding�N���b�N������------------------
				if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
				{
					//�������x����S�Đ܂肽���݁E�W�J
					SPI_ExpandCollapseAllForSameLevel(clickRowDisplayIndex);
				}
				else
				{
					//�܂肽���݁E�W�J
					SPI_ExpandCollapse(clickRowDisplayIndex,false);
				}
				//�z�o�[�X�V
				UpdateHover(inLocalPoint,inModifierKeys,true);
				//
				NVM_GetWindow().OWICB_ListViewExpandedCollapsed(NVI_GetControlID());
				return true;
			}
		}
	}
	
	//==================�I��==================
	//------------------�`��X�V�p�ɁA���݂̑I�����ڂ��L������B------------------
	//B0303�̂��Łi���X�g�I�������������j
	AIndex	svSelectedRowIndex = mSelectedRowDisplayIndex;
	AIndex	svCursorStartRowIndex = mCursorRowStartDisplayIndex;
	AIndex	svCursorEndRowIndex = mCursorRowEndDisplayIndex;
	//#237
	AArray<AIndex>	svSelectedRowArray;
	for( AIndex i = 0; i < mSelectedRowDisplayIndexArray.GetItemCount(); i++ )
	{
		svSelectedRowArray.Add(mSelectedRowDisplayIndexArray.Get(i));
	}
	//------------------�I�����ڂ��X�V�i�P�ꍀ�ځj------------------
	if( NVM_IsImagePointInViewRect(clickImagePoint) == true )
	{
		mSelectedRowDisplayIndex = clickRowDisplayIndex;
		if( mSelectedRowDisplayIndex < 0 || mSelectedRowDisplayIndex >= GetRowCount() )
		{
			mSelectedRowDisplayIndex = kIndex_Invalid;
		}
		//#232 �s��disabled�Ȃ�I����������
		if( mSelectedRowDisplayIndex != kIndex_Invalid )
		{
			if( mDataBase.GetData_BoolArray(kEnabled,mSelectedRowDisplayIndex) == false )
			{
				mSelectedRowDisplayIndex = kIndex_Invalid;
			}
		}
	}
	else
	{
		mSelectedRowDisplayIndex = kIndex_Invalid;
	}
	mCursorRowStartDisplayIndex = kIndex_Invalid;
	mCursorRowEndDisplayIndex = kIndex_Invalid;
	mCursorRowLeftMargin = 0;
	
	//------------------�I�����ڂ��X�V�i�������ځj------------------
	//#237 �����I��
	if( mMultiSelectEnalbed == true )
	{
		//Shift�L�[�ɂ�镡���I��
		if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true )
		{
			if( mSelectedRowDisplayIndexArray.GetItemCount() == 0 )
			{
				mSelectedRowDisplayIndexArray.Add(mSelectedRowDisplayIndex);
			}
			else
			{
				AIndex	first = mSelectedRowDisplayIndexArray.Get(0);
				AIndex	cur = mSelectedRowDisplayIndex;
				mSelectedRowDisplayIndexArray.DeleteAll();
				if( first <= cur )
				{
					for( AIndex i = first; i <= cur; i++ )
					{
						mSelectedRowDisplayIndexArray.Add(i);
					}
				}
				else
				{
					for( AIndex i = first; i >= cur; i-- )
					{
						mSelectedRowDisplayIndexArray.Add(i);
					}
				}
			}
		}
		//Ctrl�L�[�ɂ�镡���I��
		else if( AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
		{
			if( mSelectedRowDisplayIndexArray.Find(mSelectedRowDisplayIndex) == kIndex_Invalid )
			{
				mSelectedRowDisplayIndexArray.Add(mSelectedRowDisplayIndex);
			}
		}
		//�����I�������i�P�I���j
		else
		{
			mSelectedRowDisplayIndexArray.DeleteAll();
			mSelectedRowDisplayIndexArray.Add(mSelectedRowDisplayIndex);
		}
	}
	
	//win 080618
	//�N���b�N�����炻��܂ł̕ҏW���[�h�͉���
	if( mEditMode == true )
	{
		SPI_EndEditMode();
	}
	
	//==================�`��X�V==================
	//B0303�̂��Łi���X�g�I�������������jNVI_Refresh();
	//B0303�̂��Łi���X�g�I�������������j
	//
	SPI_RefreshRow(svSelectedRowIndex);
	SPI_RefreshRow(svCursorStartRowIndex);
	SPI_RefreshRow(svCursorEndRowIndex);
	if( svSelectedRowArray.GetItemCount() > 0 )//#237
	{
		for( AIndex i = 0; i < svSelectedRowArray.GetItemCount(); i++ )
		{
			SPI_RefreshRow(svSelectedRowArray.Get(i));
		}
	}
	//
	SPI_RefreshRow(mSelectedRowDisplayIndex);
	if( mSelectedRowDisplayIndexArray.GetItemCount() > 0 )//#237
	{
		for( AIndex i = 0; i < mSelectedRowDisplayIndexArray.GetItemCount(); i++ )
		{
			SPI_RefreshRow(mSelectedRowDisplayIndexArray.Get(i));
		}
	}
	
	//------------------AWindow�ł̏���------------------
	if( inClickCount == 1 )
	{
		NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
	}
	else if( inClickCount == 2 )
	{
		NVM_GetWindow().EVT_DoubleClicked(NVI_GetControlID());
	}
	
	//------------------�I������������------------------
	if( mSelectedRowDisplayIndex == kIndex_Invalid )
	{
		EVTDO_ListClicked(kIndex_Invalid,kIndex_Invalid,inClickCount);//#199
		return false;
	}
	
	//win 080618
	//�N���b�N���ꂽ���index���擾
	AIndex	DBRowIndex = mRowOrderDisplayToDBArray.Get(mSelectedRowDisplayIndex);
	
	EVTDO_ListClicked(mSelectedRowDisplayIndex,DBRowIndex,inClickCount);//#199
	//
	AIndex colIndex = 0;
	ANumber	w = 0;
	for( ; colIndex < mColumnIDArray.GetItemCount()-1; colIndex++ )
	{
		w += mColumnWidthArray.Get(colIndex);
		if( clickImagePoint.x < w )
		{
			break;
		}
	}
	//Edit
	//win 080618
	mLastClickedColumnID = mColumnIDArray.Get(colIndex);
	if( mColumnEditable.Get(colIndex) == true )
	{
		//�s�ҏW�\���� #427
		/*#695
		ABool	rowEditable = true;
		if( mSelectedRowDisplayIndex < mRowEditableArray.GetItemCount() )
		{
			if( mRowEditableArray.Get(mSelectedRowDisplayIndex) == false )
			{
				rowEditable = false;
			}
		}
		*/
		ABool	rowEditable = mDataBase.GetData_BoolArray(kRowEditable,mSelectedRowDisplayIndex);//#695
		if( rowEditable == true )
		{
			switch(mDataBase.GetDataType(mLastClickedColumnID))
			{
			  case kDataType_BoolArray:
				{
					//On/Off���g�O������
					mDataBase.SetData_BoolArray(mLastClickedColumnID,DBRowIndex,(mDataBase.GetData_BoolArray(mLastClickedColumnID,DBRowIndex)==false));
					NVM_GetWindow().EVT_ValueChanged(NVI_GetControlID());
					SPI_RefreshRow(mSelectedRowDisplayIndex);
					break;
				}
			  case kDataType_TextArray:
			  case kDataType_NumberArray://#868
				{
					//�ҏW���[�h�ֈڍs
					if( inClickCount == 2 )
					{
						SPI_StartEditMode(mColumnIDArray.Get(colIndex),mSelectedRowDisplayIndex);
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
	}
	
	if( mEnableDragDrop == true )
	{
		//�����ň�U�C�x���g�R�[���o�b�N�𔲂���BMouseTrack�͕ʂ̃C�x���g�ōs���B
		//TrackingMode�ݒ�
		mMouseTrackingMode = kMouseTrackingMode_SingleClick;
		mMouseTrackStartImagePoint = clickImagePoint;
		mMouseTrackResultIsDrag = false;
		NVM_SetMouseTrackingMode(true);
		return true;
	}
	return true;
}

void	AView_List::StartDrag( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mMouseTrackResultIsDrag == true )
	{
		//Drag�̈�ݒ�
		AArray<ALocalRect>	dragRect;
		AImageRect	rowImageRect;
		GetRowImageRect(mSelectedRowDisplayIndex,mSelectedRowDisplayIndex+1,rowImageRect);
		ALocalRect	rowLocalRect;
		NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
		dragRect.Add(rowLocalRect);
		//Scrap�ݒ�
		AArray<AScrapType>	scrapType;
		ATextArray	data;
		scrapType.Add(kScrapType_ListViewItem);
		AText	text;
		data.Add(text);
		//Drag���s
		NVMC_DragText(inLocalPoint,dragRect,scrapType,data);
		//
		NVI_Refresh();
	}
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
//Mouse Tracking
void	AView_List::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	switch(mMouseTrackingMode)
	{
	  case kMouseTrackingMode_SingleClick:
		{
			//#871
			//row order���[�h���ʏ�A���A�����I���ł͂Ȃ��ꍇ�̂݁i�\�[�g��t�B���^�[�ȊO�j�A�h���b�O�J�n
			if( mDisplayRowOrderMode == kDisplayRowOrderMode_Normal && mSelectedRowDisplayIndexArray.GetItemCount() <= 1 )
			{
				//
				if( mouseImagePoint.x != mMouseTrackStartImagePoint.x || mouseImagePoint.y != mMouseTrackStartImagePoint.y )
				{
					mMouseTrackResultIsDrag = true;
					//
					StartDrag(inLocalPoint,inModifierKeys);
					//TrackingMode����
					mMouseTrackingMode = kMouseTrackingMode_None;
					NVM_SetMouseTrackingMode(false);
				}
			}
			break;
		}
	  default:
		{
			_ACError("",this);
			break;
		}
	}
}

//Mouse Tracking�I���i�}�E�X�{�^��Up�j
void	AView_List::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	//TrackingMode����
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}
//#688 #endif

//R0186
//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_List::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( mEnableContextMenu == true )
	{
		AImagePoint	clickImagePoint;
		NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
		if( NVM_IsImagePointInViewRect(clickImagePoint) == true )
		{
			mContextMenuSelectedRowDisplayIndex = clickImagePoint.y/GetRowHeight();
			AGlobalPoint	globalPoint;
			NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
			//#&88 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(mContextMenuItemID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());//win 080712
			NVMC_ShowContextMenu(mContextMenuItemID,globalPoint);//#688
		}
		return true;
	}
	else
	{
		return false;
	}
}

//R0186
AIndex	AView_List::SPI_GetContextMenuSelectedRowDBIndex() const
{
	return mRowOrderDisplayToDBArray.Get(mContextMenuSelectedRowDisplayIndex);
}

//
void	AView_List::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
									   const ALocalPoint inLocalPoint )//win 080706
{
	//R0000
	ANumber	scrollPercent;
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true
				|| AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )//win 080702
	{
		scrollPercent = mWheelScrollPercentHighSpeed;
	}
	else
	{
		scrollPercent = mWheelScrollPercent;
	}
	//�X�N���[�����s
	NVI_Scroll(inDeltaX*NVI_GetHScrollBarUnit()*scrollPercent/100,inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100);//R0000
	
	//�i�}�E�X�ʒu�����݂̃z�o�[�͈͊O�ɂȂ�Ƃ��́j�z�o�[�X�V
	AImagePoint	clickImagePoint = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	AIndex newCursorDisplayIndex = clickImagePoint.y/GetRowHeight();
	if( newCursorDisplayIndex < mCursorRowStartDisplayIndex  || newCursorDisplayIndex > mCursorRowEndDisplayIndex )
	{
		UpdateHover(inLocalPoint,inModifierKeys,false);
	}
}

/**
�}�E�X�z�o�[����
*/
ABool	AView_List::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�z�o�[�X�V
	UpdateHover(inLocalPoint,inModifierKeys,false);
	//�J�[�\���ݒ�
	ACursorWrapper::SetCursor(kCursorType_Arrow);
	return true;
}

/**
�z�o�[�X�V
*/
void	AView_List::UpdateHover( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ABool inUpdateAlways )
{
	if( mEnableCursorRow == true )
	{
		AImagePoint	clickImagePoint;
		NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
		
		//==================�z�o�[�ʒu�擾==================
		AIndex newCursorDisplayIndex = clickImagePoint.y/GetRowHeight();
		
		//==================�z�o�[�ʒu���s�͈͊O�Ȃ�z�o�[�������ďI��==================
		if( newCursorDisplayIndex < 0 || newCursorDisplayIndex >= SPI_GetDisplayRowCount() )
		{
			ClearCursorRow();
			return;
		}
		
		//==================folding�A�C�R���ォ�ǂ����̔���==================
		ABool	newCursorRowHoverFolding = false;
		ALocalRect	foldingRect = {0};
		if( newCursorDisplayIndex >= 0 && newCursorDisplayIndex < SPI_GetDisplayRowCount() )
		{
			GetOutlineFoldingAndOutlineMarginRect(newCursorDisplayIndex,foldingRect);
			if( IsPointInRect(inLocalPoint,foldingRect) == true )
			{
				newCursorRowHoverFolding = true;
			}
		}
		
		//==================�z�o�[�X�V==================
		//�z�o�[�ʒu���ω��������Afolding�A�C�R���ォ�ǂ������ω��������AinUpdateAlways��true�̂Ƃ��̓z�o�[�X�V
		if( newCursorDisplayIndex != mCursorRowStartDisplayIndex ||
		   newCursorRowHoverFolding != mCursorRowHoverFolding ||
		   inUpdateAlways == true )
		{
			//�ύX�O�̃z�o�[�͈͂��L��
			AIndex	svStartRowDisplayIndex = mCursorRowStartDisplayIndex;
			AIndex	svEndRowDisplayIndex = mCursorRowEndDisplayIndex;
			//�܂肽���݃A�C�R�����z�o�[�����ǂ����������o�[�ϐ��ɋL��
			mCursorRowHoverFolding = newCursorRowHoverFolding;
			if( newCursorRowHoverFolding == true )
			{
				//==================�܂肽���ݔ͈͂��z�o�[�ݒ�==================
				//folding�J�n�ʒudb index�擾
				AIndex	foldingStartRowDBIndex = mRowOrderDisplayToDBArray.Get(newCursorDisplayIndex);
				AIndex	foldingEndRowDBIndex = foldingStartRowDBIndex;
				//���̍��ڂ̐܂肽���݃��x�����擾
				AIndex	collapseLevel = mDataBase.GetData_NumberArray(kOutlineFoldingLevel,foldingStartRowDBIndex);
				//���̍��ڂ�collapseLevel�ȉ��ƂȂ鍀�ڂ܂łƂ΂�
				for( ; foldingEndRowDBIndex+1 < GetRowCount(); foldingEndRowDBIndex++ )
				{
					if( mDataBase.GetData_NumberArray(kOutlineFoldingLevel,foldingEndRowDBIndex+1) <= collapseLevel )
					{
						break;
					}
				}
				//folding�͈͕\��index�擾
				mCursorRowStartDisplayIndex = newCursorDisplayIndex;
				mCursorRowEndDisplayIndex = SPI_GetDisplayRowIndexFromDBIndex(foldingEndRowDBIndex);
				if( mCursorRowEndDisplayIndex == kIndex_Invalid )
				{
					mCursorRowEndDisplayIndex = newCursorDisplayIndex;
				}
				mCursorRowLeftMargin = foldingRect.right - 20;
				if( mCursorRowLeftMargin < 0 )   mCursorRowLeftMargin = 0;
			}
			else
			{
				//==================���݂̃}�E�X�s���z�o�[�ݒ�==================
				mCursorRowStartDisplayIndex = mCursorRowEndDisplayIndex = newCursorDisplayIndex;
				mCursorRowLeftMargin = 0;
			}
			//�`��X�V
			SPI_RefreshRows(svStartRowDisplayIndex,svEndRowDisplayIndex);
			SPI_RefreshRows(mCursorRowStartDisplayIndex,mCursorRowEndDisplayIndex);
			//�z�o�[�X�V��ʒm
			AIndex	hoverStartDBIndex = kIndex_Invalid;
			AIndex	hoverEndDBIndex = kIndex_Invalid;
			if( mCursorRowStartDisplayIndex != kIndex_Invalid )
			{
				hoverStartDBIndex = mRowOrderDisplayToDBArray.Get(mCursorRowStartDisplayIndex);
			}
			if( mCursorRowEndDisplayIndex != kIndex_Invalid )
			{
				hoverEndDBIndex = mRowOrderDisplayToDBArray.Get(mCursorRowEndDisplayIndex);
			}
			NVM_GetWindow().OWICB_ListViewHoverUpdated(NVI_GetControlID(),hoverStartDBIndex,hoverEndDBIndex);
		}
	}
}

//win 080712
void	AView_List::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearCursorRow();
}

void	AView_List::EVTDO_DoControlBoundsChanged()
{
	SetImageSize();
}

//Drag&Drop�����iDrag���j
void	AView_List::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )
{
	if( mEnableDragDrop == false )   return;
	AIndex	newDragCaretDisplayRowIndex = CalcDragCaretDisplayRowIndex(inLocalPoint);
	if( newDragCaretDisplayRowIndex != mDragCaretRowDisplayIndex || mDragDrawn == false )
	{
		if( mDragDrawn == true )
		{
			//Erase
			mDragDrawn = false;
			XorDragCaret();
		}
		mDragCaretRowDisplayIndex = newDragCaretDisplayRowIndex;
		mDragDrawn = true;
		XorDragCaret();
	}
}

//Drag&Drop�����iDrag��View�ɓ������j
void	AView_List::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mEnableDragDrop == false )   return;
	mDragCaretRowDisplayIndex = CalcDragCaretDisplayRowIndex(inLocalPoint);
	mDragDrawn = true;
	XorDragCaret();
}

//Drag&Drop�����iDrag��View����o���j
void	AView_List::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mEnableDragDrop == false )   return;
	if( mDragDrawn == true )
	{
		//Erase
		mDragDrawn = false;
		XorDragCaret();
	}
	//#871
	mDragCaretRowDisplayIndex = kIndex_Invalid;
}

//Drag&Drop�����iDrop�j
void	AView_List::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mEnableDragDrop == false )   return;
	if( mDragDrawn == true )
	{
		//Erase
		mDragDrawn = false;
		XorDragCaret();
	}
	mDragCaretRowDisplayIndex = CalcDragCaretDisplayRowIndex(inLocalPoint);
	
	//==================�t�@�C��Drop==================
	AFileAcc	file;
	if( AScrapWrapper::GetFileDragData(inDragRef,file) == true )
	{
		NVM_GetWindow().OWICB_ListViewFileDropped(NVI_GetControlID(),mDragCaretRowDisplayIndex,file);
		return;
	}
	
	//==================ListView����Drop==================
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_ListViewItem) == true )
	{
		if( mDragCaretRowDisplayIndex == mSelectedRowDisplayIndex || mDragCaretRowDisplayIndex == mSelectedRowDisplayIndex+1 )   return;
		
		//�sindex�擾
		AIndex	newRowIndex = mDragCaretRowDisplayIndex;
		if( newRowIndex > mSelectedRowDisplayIndex )
		{
			newRowIndex--;
		}
		//�s�ړ�
		//�i�������ڈړ��A�q�ړ��ɂ͖��Ή��B
		//��������Drag�A�\�[�g�^�t�B���^�[����Drag��Drag�J�n���֎~���Ă���B�j
		MoveRow(mRowOrderDisplayToDBArray.Get(mSelectedRowDisplayIndex),mRowOrderDisplayToDBArray.Get(newRowIndex));
		//drop��̍s��I��
		SPI_SetSelect(newRowIndex);
		//AWindow�ł̑I��������
		NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),0);
	}
}

//
AIndex	AView_List::CalcDragCaretDisplayRowIndex( const ALocalPoint& inLocalPoint )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	AIndex	caretRowIndex = clickImagePoint.y/GetRowHeight();
	if( caretRowIndex < 0 )   caretRowIndex = 0;
	if( caretRowIndex >= GetRowCount() )   caretRowIndex = GetRowCount();
	return caretRowIndex;
}

void	AView_List::XorDragCaret()
{
	AImageRect	imageframe;
	NVM_GetImageViewRect(imageframe);
	AImagePoint	caretImageStart, caretImageEnd;
	caretImageStart.y = caretImageEnd.y = mDragCaretRowDisplayIndex*GetRowHeight();
	caretImageStart.x = imageframe.left;
	caretImageEnd.x = imageframe.right;
	ALocalPoint	caretLocalStart, caretLocalEnd;
	NVM_GetLocalPointFromImagePoint(caretImageStart,caretLocalStart);
	NVM_GetLocalPointFromImagePoint(caretImageEnd,caretLocalEnd);
	//#871 NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,false,2);
	ALocalRect	refreshRect = {0};
	refreshRect.left	= caretLocalStart.x;
	refreshRect.top		= caretLocalStart.y - 5;
	refreshRect.right	= caretLocalEnd.x;
	refreshRect.bottom	= caretLocalStart.y + 5;
	NVMC_RefreshRect(refreshRect);
}

/**
�s�ړ�
*/
void 	AView_List::MoveRow( const AIndex inDeleteRowDBIndex, const AIndex inNewRowDBIndex )
{
	//#695 SPI_BeginSetTable()�̓e�[�u����S�č폜����悤�ɂȂ�����mTextStyleArray���̓e�[�u���ɑg�ݍ��񂾂̂ŁA���̏����͍폜�B SPI_BeginSetTable();
	//�܂��I�������iOWICB_ListViewRowMoved()�͑I���������ɏ������邱�ƑO��j
	SPI_SetSelect(kIndex_Invalid);
	NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),0);
	//DB�̃f�[�^�ړ�
	mDataBase.MoveRow_Table(kFilePathText,inDeleteRowDBIndex,inNewRowDBIndex);
	//#695 SPI_EndSetTable();
	//AWindow�ł̍s�ړ�
	NVM_GetWindow().OWICB_ListViewRowMoved(NVI_GetControlID(),inDeleteRowDBIndex,inNewRowDBIndex);
}

//#328
/**
�w���v�^�O
*/
ABool	AView_List::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	outTagSide = kHelpTagSide_Right;//#644
	//#688 NVM_GetWindowConst().NVI_HideHelpTag();
	
	//���݂̕\���s�͈͎擾
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	AIndex	startRow = imageFrameRect.top / GetRowHeight();
	AIndex	endRow = imageFrameRect.bottom / GetRowHeight();
	//localviewrect�擾
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	
	//�\�����̊e�s���ɔ���
	AItemCount	rowCount = SPI_GetDisplayRowCount();
	for( AIndex rowIndex = startRow; rowIndex <= endRow; rowIndex++ )
	{
		if( rowIndex >= rowCount )   break;
		
		//�s��imageRect���擾
		AImageRect	imageRowRect = imageFrameRect;
		imageRowRect.top	= rowIndex*GetRowHeight();
		imageRowRect.bottom	= (rowIndex+1)*GetRowHeight();
		//localRect�ɕϊ�
		ALocalRect	rowRect;
		NVM_GetLocalRectFromImageRect(imageRowRect,rowRect);
		//�}�E�X�ʒu����
		if( inPoint.x > rowRect.left && inPoint.x < rowRect.right &&
					inPoint.y > rowRect.top && inPoint.y < rowRect.bottom )
		{
			//���̍s�Ƀ}�E�X�ʒu�����݂���Ȃ�A��̍ŏ�����Atextarray�����݂���������
			for( AIndex colIndex = 0; colIndex < mColumnIDArray.GetItemCount(); colIndex++ )
			{
				APrefID	colID = mColumnIDArray.Get(colIndex);
				switch(mDataBase.GetDataType(colID))
				{
				  case kDataType_TextArray:
					{
						//���̍s��DBIndex�擾
						AIndex	DBRowIndex = mRowOrderDisplayToDBArray.Get(rowIndex);
						//Text�擾
						AText	text;
						mDataBase.GetData_TextArray(colID,DBRowIndex,text);
						//�\�����擾
						ANumber	rowWidth = viewRect.right-viewRect.left;
						ALocalRect	foldingRect = {0};
						ABool	hasFolding = GetOutlineFoldingAndOutlineMarginRect(rowIndex,foldingRect);
						if( hasFolding == true )
						{
							rowWidth -= foldingRect.right-foldingRect.left;
						}
						//�^�O�\��������ݒ�
						if( NVMC_GetDrawTextWidth(text) >= rowWidth-40 )
						{
							outText1.SetText(text);
						}
						outText2.SetText(text);
						//�^�O�\���ʒu�ݒ�i���i�̏ꍇ�͈�ԉ��j
						outRect = rowRect;
						outRect.left += kRowLeftMargin+kCellLeftMargin+kCellRightMargin;//win
						return true;
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
	return false;
}

//win
ABool	AView_List::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	//#688
	outUpdateMenu = false;
	
	//
	if( inText.GetItemCount() == 1 )
	{
		AModifierKeys	modifiers = inModifierKeys;//#688 AKeyWrapper::GetEventKeyModifiers(inOSKeyEvent);
		//#0 ABool	shift = AKeyWrapper::IsShiftKeyPressed(modifiers);
		//#0 ABool	option = AKeyWrapper::IsOptionKeyPressed(modifiers);
		//#0 ABool	cmd = AKeyWrapper::IsCommandKeyPressed(modifiers);
		AUChar	ch = inText.Get(0);
		switch(ch)
		{
		  case kUChar_Tab:
			{
				NVM_GetWindow().OWICB_ViewTabKeyPressed(NVI_GetControlID(),modifiers);
				break;
			}
		  case kUChar_Escape:
			{
				NVM_GetWindow().OWICB_ViewEscapeKeyPressed(NVI_GetControlID(),modifiers);
				break;
			}
		  case kUChar_Up:
			{
				AIndex	sel = mSelectedRowDisplayIndex;
				if( sel == kIndex_Invalid )
				{
					sel = SPI_GetRowCount()-1;
				}
				else if( sel > 0 )
				{
					sel--;
				}
				SPI_SetSelect(sel);
				//
				NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),0);
				break;
			}
		  case kUChar_Down:
			{
				AIndex	sel = mSelectedRowDisplayIndex;
				if( sel == kIndex_Invalid )
				{
					sel = 0;
				}
				else if( sel < SPI_GetRowCount()-1 )
				{
					sel++;
				}
				SPI_SetSelect(sel);
				//
				NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),0);
				break;
			}
			//#353
		  case kUChar_LineEnd:
		  case kUChar_Space:
			{
				if( mSelectedRowDisplayIndex != kIndex_Invalid )
				{
					NVM_GetWindow().EVT_DoubleClicked(NVI_GetControlID());
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
	return true;
}

#pragma mark ===========================

#pragma mark ---�t�H�[�J�X
//#137

void	AView_List::EVTDO_DoViewFocusActivated()
{
	if( mFrameViewID != kObjectID_Invalid )
	{
		GetFrameView().SPI_SetFocused(true);
	}
}

void	AView_List::EVTDO_DoViewFocusDeactivated()
{
	if( mFrameViewID != kObjectID_Invalid )
	{
		GetFrameView().SPI_SetFocused(false);
	}
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

void	AView_List::SPI_SetSelect( const AIndex inSelect )
{
	//���݂̑I�����L��
	AIndex	oldSelectedRowIndex = mSelectedRowDisplayIndex;
	//�I���X�V
	mSelectedRowDisplayIndex = inSelect;
	mSelectedRowDisplayIndexArray.DeleteAll();
	//
	if( inSelect != kIndex_Invalid )
	{
		//#237
		if( mMultiSelectEnalbed == true )
		{
			mSelectedRowDisplayIndexArray.Add(inSelect);
		}
		//
		SPI_AdjustScroll();
	}
	SPI_RefreshRow(oldSelectedRowIndex);
	SPI_RefreshRow(mSelectedRowDisplayIndex);
}

//win
/**
*/
void	AView_List::NVIDO_SetNumber( const ANumber inNumber )
{
	SPI_SetSelect(inNumber);
}

//#798
/**
���̍��ڂ�I��
*/
void	AView_List::SPI_SetSelectNextDisplayRow( const AModifierKeys inModifers )
{
	//cmd�L�[�������Ȃ���̂Ƃ��͍Ō�̍��ڂ�I��
	if( AKeyWrapper::IsCommandKeyPressed(inModifers) == true )
	{
		if( SPI_GetDisplayRowCount() > 0 )
		{
			SPI_SetSelectByDisplayRowIndex(SPI_GetDisplayRowCount()-1);
		}
		return;
	}
	//���ݖ��I���Ȃ�ŏ��̍��ڂ�I��
	if( mSelectedRowDisplayIndex == kIndex_Invalid )
	{
		if( SPI_GetDisplayRowCount() > 0 )
		{
			SPI_SetSelectByDisplayRowIndex(0);
		}
	}
	//����I��
	else
	{
		if( mSelectedRowDisplayIndex+1 < SPI_GetDisplayRowCount() )
		{
			SPI_SetSelectByDisplayRowIndex(mSelectedRowDisplayIndex+1);
		}
		else
		{
			SPI_SetSelectByDisplayRowIndex(0);
		}
	}
}

//#798
/**
�O�̍��ڂ�I��
*/
void	AView_List::SPI_SetSelectPreviousDisplayRow( const AModifierKeys inModifers )
{
	//cmd�L�[�������Ȃ���̂Ƃ��͍ŏ��̍��ڂ�I��
	if( AKeyWrapper::IsCommandKeyPressed(inModifers) == true )
	{
		if( SPI_GetDisplayRowCount() > 0 )
		{
			SPI_SetSelectByDisplayRowIndex(0);
		}
		return;
	}
	//���ݖ��I���Ȃ�Ō�̍��ڂ�I��
	if( mSelectedRowDisplayIndex == kIndex_Invalid )
	{
		if( SPI_GetDisplayRowCount() > 0 )
		{
			SPI_SetSelectByDisplayRowIndex(SPI_GetDisplayRowCount()-1);
		}
	}
	//�O��I��
	else
	{
		if( mSelectedRowDisplayIndex-1 >= 0 )
		{
			SPI_SetSelectByDisplayRowIndex(mSelectedRowDisplayIndex-1);
		}
		else
		{
			SPI_SetSelectByDisplayRowIndex(SPI_GetDisplayRowCount()-1);
		}
	}
}

void	AView_List::SPI_SetSelectByDBIndex( const AIndex inSelectDBIndex, const ABool inAdjustScroll )//#698
{
	AIndex	oldSelectedRowIndex = mSelectedRowDisplayIndex;
	mSelectedRowDisplayIndex = mRowOrderDisplayToDBArray.Find(inSelectDBIndex);
	//���ݕ\������Ă��Ȃ����ڂ�select�����Ƃ��́ADBIndex�������̂ڂ��āA�ŏ��ɕ\������Ă��鍀�ڂ�select����
	if( mSelectedRowDisplayIndex == kIndex_Invalid && mDisplayRowOrderMode == kDisplayRowOrderMode_Normal )
	{
		for( AIndex dbindex = inSelectDBIndex-1; dbindex > 0; dbindex-- )
		{
			mSelectedRowDisplayIndex = mRowOrderDisplayToDBArray.Find(dbindex);
			if( mSelectedRowDisplayIndex != kIndex_Invalid )
			{
				break;
			}
		}
	}
	//#237
	if( mMultiSelectEnalbed == true )
	{
		mSelectedRowDisplayIndexArray.DeleteAll();
		mSelectedRowDisplayIndexArray.Add(mSelectedRowDisplayIndex);
	}
	//
	if( inAdjustScroll == true )//#698
	{
		SPI_AdjustScroll();
	}
	//
	SPI_RefreshRow(oldSelectedRowIndex);
	SPI_RefreshRow(mSelectedRowDisplayIndex);
}

//#237
/**
�����I�����ꂽ���ڂ�DBIndex���擾����

���O���[�̍��ڂ͓���Ȃ�
*/
void	AView_List::SPI_GetSelectedDBIndexArray( AArray<AIndex>& outDBIndexArray ) const
{
	outDBIndexArray.DeleteAll();
	if( mSelectedRowDisplayIndexArray.GetItemCount() > 0 )
	{
		for( AIndex i = 0; i < mSelectedRowDisplayIndexArray.GetItemCount(); i++ )
		{
			AIndex	selectedRowIndex = mSelectedRowDisplayIndexArray.Get(i);
			if( selectedRowIndex != kIndex_Invalid )
			{
				AIndex	dbIndex = mRowOrderDisplayToDBArray.Get(selectedRowIndex);
				if( mDataBase.GetData_BoolArray(kEnabled,dbIndex) == true )
				{
					outDBIndexArray.Add(dbIndex);
				}
			}
		}
	}
	else
	{
		if( mSelectedRowDisplayIndex != kIndex_Invalid )
		{
			AIndex	dbIndex = mRowOrderDisplayToDBArray.Get(mSelectedRowDisplayIndex);
			if( mDataBase.GetData_BoolArray(kEnabled,dbIndex) == true )
			{
				outDBIndexArray.Add(dbIndex);
			}
		}
	}
}

void	AView_List::SPI_AdjustScroll()
{
	SPI_AdjustScroll(mSelectedRowDisplayIndex);
}
void	AView_List::SPI_AdjustScroll( const AIndex inRowDisplayIndex )
{
	if( inRowDisplayIndex == kIndex_Invalid )
	{
		AImagePoint	pt = {0,0};
		NVI_ScrollTo(pt);
		return;
	}
	AImageRect	rect;
	NVM_GetImageViewRect(rect);
	AIndex	startRow = rect.top / GetRowHeight();
	AIndex	endRow = rect.bottom / GetRowHeight();
	if( inRowDisplayIndex < startRow || inRowDisplayIndex >= endRow )//B0321
	{
		AImagePoint	pt;
		pt.x = 0;
		pt.y = (inRowDisplayIndex-(endRow-startRow)/2)*GetRowHeight();
		NVI_ScrollTo(pt);
	}
}

void	AView_List::SPI_BeginSetTable()
{
	//R0108������������ς�߁BAWindow_JumpList::SPI_UpdateTable�̂ق��őΉ����邱�Ƃɂ���B
	/*NVI_GetOriginOfLocalFrame(mSavedOrigin);
	mSelectedRowDisplayIndex = kIndex_Invalid;
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);*/
	//R0006
	/*#695
	mTextStyleArray.DeleteAll();
	mColorArray.DeleteAll();
	mBackgroundColorArray.DeleteAll();//#427
	mRowEditableArray.DeleteAll();//#427
	*/
	mDataBase.DeleteAllRows_Table(kFilePathText);
}

void	AView_List::SPI_EndSetTable()
{
	while( mDataBase.GetItemCount_Array(kFilePathText) < GetRowCount() )
	{
		mDataBase.Add_TextArray(kFilePathText,GetEmptyText());
	}
	/*#695
	while( mDataBase.GetItemCount_Array(kIconID) < GetRowCount() )
	{
		mDataBase.Add_NumberArray(kIconID,kIconID_NoIcon);
	}
	//#232 �s��Enable/Disable
	while( mDataBase.GetItemCount_Array(kEnabled) < GetRowCount() )
	{
		mDataBase.Add_BoolArray(kEnabled,true);
	}
	*/
	mDataBase.CorrectTable();//#695
	//
	/*#871
	mRowOrderDisplayToDBArray.DeleteAll();
	for( AIndex i = 0; i < GetRowCount(); i++ )
	{
		mRowOrderDisplayToDBArray.Add(i);
	}
	*/
	//�s�\�����X�V
	UpdateRowOrderDisplay(false);
	/*UpdateRowOrderDisplay()�Ŏ��s
	//win 080618
	if( mSelectedRowDisplayIndex >= SPI_GetRowCount() )
	{
		mSelectedRowDisplayIndex = kIndex_Invalid;
	}
	*/
	//
	SetImageSize();
	
	//R0108������������ς�߁BAWindow_JumpList::SPI_UpdateTable�̂ق��őΉ����邱�Ƃɂ���B
	//NVI_ScrollTo(mSavedOrigin);
	
	NVI_Refresh();
}

#pragma mark ===========================

#pragma mark ---�e�[�u���S��

/**
�\�����Ă��鍀�ڂ̐����擾
*/
AItemCount	AView_List::SPI_GetDisplayRowCount() const
{
	return mRowOrderDisplayToDBArray.GetItemCount();
}

#pragma mark ===========================

#pragma mark ---��f�[�^�̓o�^�E�ݒ�E�擾

//��̓o�^(Text)
void	AView_List::SPI_RegisterColumn_Text( const APrefID inColumnID, const ANumber inWidth, const AConstCharPtr inTitle, const ABool inEditable )
{
	mDataBase.CreateData_TextArray(inColumnID,"","");
	mColumnIDArray.Add(inColumnID);
	mColumnWidthArray.Add(inWidth);
	mColumnEditable.Add(inEditable);
	//win 080618
	//�t���[���̕��A�^�C�g����ݒ�
	if( mFrameViewID != kObjectID_Invalid )
	{
		AText	title;
		if( inTitle[0] != 0 )
		{
			title.SetLocalizedText(inTitle);
		}
		GetFrameView().SPI_RegisterColumn(inColumnID,inWidth,title);
	}
	//�A�C�R��
	//#1012 mIconDataBase.CreateData_NumberArray(inColumnID,"",0,-1,kNumber_MaxNumber);//#695 ,false);
}

//��̓o�^(Bool) win 080618
void	AView_List::SPI_RegisterColumn_Bool( const APrefID inColumnID, const ANumber inWidth, const AConstCharPtr inTitle, const ABool inEditable )
{
	mDataBase.CreateData_BoolArray(inColumnID,"",false);
	mColumnIDArray.Add(inColumnID);
	mColumnWidthArray.Add(inWidth);
	mColumnEditable.Add(inEditable);
	//�t���[���̕��A�^�C�g����ݒ�
	if( mFrameViewID != kObjectID_Invalid )
	{
		AText	title;
		if( inTitle[0] != 0 )
		{
			title.SetLocalizedText(inTitle);
		}
		GetFrameView().SPI_RegisterColumn(inColumnID,inWidth,title);
	}
}

//��̓o�^(Number) win 080618
void	AView_List::SPI_RegisterColumn_Number( const APrefID inColumnID, const ANumber inWidth, const AConstCharPtr inTitle, const ABool inEditable )
{
	mDataBase.CreateData_NumberArray(inColumnID,"",0,kNumber_MinNumber,kNumber_MaxNumber);//#695 ,false);
	mColumnIDArray.Add(inColumnID);
	mColumnWidthArray.Add(inWidth);
	mColumnEditable.Add(inEditable);
	//�t���[���̕��A�^�C�g����ݒ�
	if( mFrameViewID != kObjectID_Invalid )
	{
		AText	title;
		if( inTitle[0] != 0 )
		{
			title.SetLocalizedText(inTitle);
		}
		GetFrameView().SPI_RegisterColumn(inColumnID,inWidth,title);
	}
}

//��f�[�^��ݒ�(Text)
void	AView_List::SPI_SetColumn_Text( const APrefID inColumnID, const ATextArray& inTextArray )
{
	mDataBase.SetData_TextArray(inColumnID,inTextArray);
}

//��f�[�^��ݒ�(Bool)
void	AView_List::SPI_SetColumn_Bool( const APrefID inColumnID, const ABoolArray& inBoolArray )
{
	mDataBase.SetData_BoolArray(inColumnID,inBoolArray);
}

//��f�[�^��ݒ�(Number)
void	AView_List::SPI_SetColumn_Number( const APrefID inColumnID, const ANumberArray& inNumberArray )
{
	mDataBase.SetData_NumberArray(inColumnID,inNumberArray);
}

//��f�[�^��ݒ�(Color)
void	AView_List::SPI_SetColumn_Color( const APrefID inColumnID, const AColorArray& inColorArray )
{
	mDataBase.SetData_ColorArray(inColumnID,inColorArray);
}

//��f�[�^���擾 
//R0108 ������
const ATextArray&	AView_List::SPI_GetColumn_Text( const APrefID inColumnID ) const
{
	return mDataBase.GetData_ConstTextArrayRef(inColumnID);
}

//��f�[�^���擾 win 080618
const ANumberArray&	AView_List::SPI_GetColumn_Number( const APrefID inColumnID ) const
{
	return mDataBase.GetData_ConstNumberArrayRef(inColumnID);
}

//��f�[�^���擾 win 080618
const ABoolArray&	AView_List::SPI_GetColumn_Bool( const APrefID inColumnID ) const
{
	return mDataBase.GetData_ConstBoolArrayRef(inColumnID);
}

//��f�[�^���擾 win 080618
const AColorArray&	AView_List::SPI_GetColumn_Color( const APrefID inColumnID ) const
{
	return mDataBase.GetData_ConstColorArrayRef(inColumnID);
}

#pragma mark ===========================

#pragma mark ---

void	AView_List::SPI_SetTable_File( const ATextArray& inTextArray )
{
	mDataBase.SetData_TextArray(kFilePathText,inTextArray);
}

void	AView_List::SPI_SetTable_TextStyle( const AArray<ATextStyle>& inTextStyleArray )
{
	mDataBase.DeleteAll_NumberArray(kTextStyle);
	AItemCount	rowCount = inTextStyleArray.GetItemCount();
	for( AIndex i = 0; i < rowCount; i++ )
	{
		mDataBase.Add_NumberArray(kTextStyle,static_cast<ANumber>(inTextStyleArray.Get(i)));
	}
	/*#695
	mTextStyleArray.DeleteAll();
	AItemCount	count = inTextStyleArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		mTextStyleArray.Add(inTextStyleArray.Get(i));
	}
	*/
}

//R0006
void	AView_List::SPI_SetTable_Color( const AArray<AColor>& inColorArray )
{
	mDataBase.SetData_ColorArray(kColor,inColorArray);
	/*#695
	mColorArray.DeleteAll();
	AItemCount	count = inColorArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		mColorArray.Add(inColorArray.Get(i));
	}
	*/
}

//#427
/**
�w�i�F�ݒ�
*/
void	AView_List::SPI_SetTable_BackgroundColor( const AArray<AColor>& inColorArray )
{
	mDataBase.SetData_ColorArray(kBackgroundColor,inColorArray);
	//
	mDataBase.DeleteAll_BoolArray(kBackgroundColorEnabled);
	AItemCount	itemCount = inColorArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		mDataBase.Add_BoolArray(kBackgroundColorEnabled,true);
	}
	/*#695
	mBackgroundColorArray.DeleteAll();
	AItemCount	count = inColorArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		mBackgroundColorArray.Add(inColorArray.Get(i));
	}
	*/
}

void	AView_List::SPI_SetTable_Icon( const AArray<AIconID>& inIconArray )
{
	mDataBase.DeleteAll_NumberArray(kIconID);
	AItemCount	count = inIconArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		mDataBase.Add_NumberArray(kIconID,inIconArray.Get(i));
	}
}

//#725
/**
ImageIconID��ݒ�
*/
void	AView_List::SPI_SetTable_ImageIconID( const AArray<AImageID>& inImageIconArray )
{
	mDataBase.DeleteAll_NumberArray(kImageIconID);
	AItemCount	count = inImageIconArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		mDataBase.Add_NumberArray(kImageIconID,inImageIconArray.Get(i));
	}
}

//#427
/**
�ҏW�ېݒ�
*/
void	AView_List::SPI_SetTable_RowEditable( const ABoolArray& inEditableArray )
{
	mDataBase.SetData_BoolArray(kRowEditable,inEditableArray);
	//#695 mRowEditableArray.SetFromObject(inEditableArray);
}

//#130
/**
Outline���x���ݒ�
*/
void	AView_List::SPI_SetTable_OutlineFoldingLevel( const AArray<AIndex>& inOutlineFoldinLevelArray )
{
	mDataBase.SetData_NumberArray(kOutlineFoldingLevel,inOutlineFoldinLevelArray);
	mOutlineLevelDataExist = true;
}

//#232
/**
�s��Enable/Disable�ݒ�
*/
void	AView_List::SPI_SetTable_Enabled( const ABoolArray& inEnabledArray )
{
	mDataBase.DeleteAll_BoolArray(kEnabled);
	AItemCount	count = inEnabledArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		mDataBase.Add_BoolArray(kEnabled,inEnabledArray.Get(i));
	}
}

//win 080618
void	AView_List::SPI_SetColumnWidth( const ADataID inColumnID, const ANumber inWidth )
{
	AIndex	index = mColumnIDArray.Find(inColumnID);
	if( index == kIndex_Invalid )   {_ACError("",this);return;}
	mColumnWidthArray.Set(index,inWidth);
	//win 080618
	//�t���[���̕��A�^�C�g����ݒ�
	if( /*#205 mFrameControlID != kControlID_Invalid*/mFrameViewID != kObjectID_Invalid )
	{
		/*#205 NVM_GetWindow().NVI_GetListFrameView(mFrameControlID)*/GetFrameView().SPI_SetColumnWidth(index,inWidth);
	}
}

//win 080618
ANumber	AView_List::SPI_GetColumnWidth( const ADataID inColumnID ) const
{
	AIndex	index = mColumnIDArray.Find(inColumnID);
	if( index == kIndex_Invalid )   {_ACError("",this);return 0;}
	return mColumnWidthArray.Get(index);
}

/* win 080618 ���g�p�i�������̂��ߎg�p����Ȃ��Ȃ����j�̂��ߍ폜
void	AView_List::SPI_GetTable_Text( const APrefID inColumnID, ATextArray& outTextArray ) const
{
	outTextArray.Set(mDataBase.GetData_ConstTextArrayRef(inColumnID));
}
*/

void	AView_List::SPI_GetTable_File( ATextArray& outTextArray ) const
{
	outTextArray.SetFromTextArray(mDataBase.GetData_ConstTextArrayRef(kFilePathText));
}

/*#695 ���g�p�̂��ߍ폜
const AArray<ATextStyle>&	AView_List::SPI_GetTable_TextStyle() const
{
	return mTextStyleArray;
}
*/

#pragma mark ===========================

#pragma mark ---�s��Set

//R0108 ������
void	AView_List::SPI_SetTableRow_Text( const APrefID inColumnID, 
		const AIndex inDBIndex, const AText& inText, const ATextStyle inTextStyle )
{
	mDataBase.SetData_TextArray(inColumnID,inDBIndex,inText);
	//#695 mTextStyleArray.Set(inDBIndex,inTextStyle);
	mDataBase.SetData_NumberArray(kTextStyle,inDBIndex,static_cast<ANumber>(inTextStyle));//#695
	//#695 �ʏ�Array��Find()�Ȃ̂Œx���i�R�[�����ł܂Ƃ߂�refresh����悤�ɂ���j SPI_RefreshRow(mRowOrderDisplayToDBArray.Find(inDBIndex));
}

//#130
/**
�A�E�g���C�����x���ݒ�
*/
void	AView_List::SPI_SetTableRow_OutlineFoldingLevel( const AIndex inDBIndex, const AIndex inOutlineFoldingLevel )
{
	mDataBase.SetData_NumberArray(kOutlineFoldingLevel,inDBIndex,inOutlineFoldingLevel);
	mOutlineLevelDataExist = true;
}

#pragma mark ===========================

#pragma mark ---�s��Insert/Delete

//#695
/**
�s�}���iinCount�̍s��}���j
*/
void	AView_List::SPI_InsertTableRows( const AIndex inDBIndex, const AItemCount inCount,
		const ATextStyle inTextStyle, const AColor inColor, 
		const ABool inEnableBackgroundColor, const AColor inBackgroundColor, 
		const ABool inRowEditable )
{
	//display row index���擾
	//�i�}��DBIndex�ȍ~�ōŏ��ɕ\������Ă��鍀�ڂ�display row index���擾�B�Ȃ���΍Ō�B�j
	AIndex	insertDisplayRowIndex = mRowOrderDisplayToDBArray.GetItemCount();
	for( AIndex dbIndex = inDBIndex; dbIndex < mDataBase.GetItemCount_Array(kFilePathText); dbIndex++ )
	{
		AIndex	displayIndex = mRowOrderDisplayToDBArray.Find(dbIndex);
		if( displayIndex != kIndex_Invalid )
		{
			insertDisplayRowIndex = displayIndex;
			break;
		}
	}
	//==================DB�Ƀf�[�^�}��==================
	mDataBase.ReserveRow_Table(kFilePathText,inDBIndex,inCount);
	for( AIndex index = inDBIndex; index < inDBIndex + inCount; index++ )
	{
		mDataBase.SetData_NumberArray(kIconID,index,0);
		mDataBase.SetData_BoolArray(kEnabled,index,true);
		mDataBase.SetData_NumberArray(kTextStyle,index,inTextStyle);
		mDataBase.SetData_ColorArray(kColor,index,inColor);
		mDataBase.SetData_ColorArray(kBackgroundColor,index,inBackgroundColor);
		mDataBase.SetData_BoolArray(kBackgroundColorEnabled,index,inEnableBackgroundColor);
		mDataBase.SetData_BoolArray(kRowEditable,index,inRowEditable);
		mDataBase.SetData_NumberArray(kImageIconID,index,kImageID_Invalid);//#725
	}
	/*#871
	//�\�[�g�f�[�^���ڑ}��
	mRowOrderDisplayToDBArray.Reserve(inDBIndex,inCount);
	//�\�[�g��Ԃ������i�s�̕\��index��db��index��S�ē����ɂ���j//�����x����
	{
		AItemCount	itemCount = mRowOrderDisplayToDBArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mRowOrderDisplayToDBArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			p[i] = i;
		}
	}
	*/
	//==================�\���sarray�̍X�V==================
	//mRowOrderDisplayToDBArray���́A�}��DBIndex�ȍ~��DBIndex�����炷
	{
		AItemCount	itemCount = mRowOrderDisplayToDBArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mRowOrderDisplayToDBArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			if( p[i] >= inDBIndex )
			{
				p[i] += inCount;
			}
		}
	}
	
	//mRowOrderDisplayToDBArray�ɍ���ǉ����̍��ڂ�}��
	//�܂�A�t�B���^���ł����Ă��A�ǉ����ڂ͕K���\�������B�i�\���ʒu�͑}��DBIndex�ȍ~�ōŏ��ɕ\������Ă��鍀�ڂ̑O�j
	mRowOrderDisplayToDBArray.Reserve(insertDisplayRowIndex,inCount);
	{
		AItemCount	itemCount = mRowOrderDisplayToDBArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mRowOrderDisplayToDBArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < inCount; i++ )
		{
			p[insertDisplayRowIndex+i] = inDBIndex+i;
		}
	}
	//�I������
	mSelectedRowDisplayIndex = kIndex_Invalid;
	mSelectedRowDisplayIndexArray.DeleteAll();
	//Image size�X�V
	SetImageSize();
}

//#695
/**
�s�폜�i��s�폜�j
*/
void	AView_List::SPI_DeleteTableRow( const AIndex inDBIndex )
{
	//==================DB�̍s�폜==================
	mDataBase.DeleteRow_Table(kFilePathText,inDBIndex);
	
	//==================�\���sarray�̍X�V==================
	AIndex	deleteDisplayRowIndex = mRowOrderDisplayToDBArray.Find(inDBIndex);
	if( deleteDisplayRowIndex != kIndex_Invalid )
	{
		mRowOrderDisplayToDBArray.Delete1(deleteDisplayRowIndex);
	}
	//mRowOrderDisplayToDBArray���́A�폜DBIndex�ȍ~��DBIndex�����炷
	{
		AItemCount	itemCount = mRowOrderDisplayToDBArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mRowOrderDisplayToDBArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			if( p[i] >= inDBIndex )
			{
				p[i] -= 1;
			}
		}
	}
	/*
	//�\�[�g�f�[�^���ڍ폜
	mRowOrderDisplayToDBArray.Delete1(0);
	//�\�[�g��Ԃ������i�s�̕\��index��db��index��S�ē����ɂ���j//�����x����
	{
		AItemCount	itemCount = mRowOrderDisplayToDBArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mRowOrderDisplayToDBArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			p[i] = i;
		}
	}
	*/
	//�I������
	mSelectedRowDisplayIndex = kIndex_Invalid;
	mSelectedRowDisplayIndexArray.DeleteAll();
	//Image size�X�V
	SetImageSize();
}

#pragma mark ===========================

#pragma mark ---�\���s����

/**
�\���s�X�V
*/
void	AView_List::UpdateRowOrderDisplay( const ABool inRedraw )
{
	//�\���s�f�[�^�S�폜
	mRowOrderDisplayToDBArray.DeleteAll();
	//
	if( mSortMode == true )
	{
		//==================�\�[�g���[�h==================
		
		for( AIndex i = 0; i < GetRowCount(); i++ )
		{
			mRowOrderDisplayToDBArray.Add(i);
		}
		//�N�C�b�N�\�[�g
		if( mDataBase.GetItemCount_Array(mSortColumnID) >= 2 )
		{
			QuickSort(mSortColumnID,0,mDataBase.GetItemCount_Array(mSortColumnID)-1,mSortAscendant);
		}
		//�\�[�g���[�h�ɐݒ�
		mDisplayRowOrderMode = kDisplayRowOrderMode_Sorted;
	}
	else if( mFilterText.GetItemCount() > 0 )
	{
		//==================�t�B���^���[�h==================
		
		const ATextArray&	textArray = SPI_GetColumn_Text(mFilterColumnID);
		//������v������̂�ǉ����Ă���
		for( AIndex i = 0; i < GetRowCount(); i++ )
		{
			AText	text;
			textArray.Get(i,text);
			text.ChangeCaseLowerFast();
			AIndex	pos = 0;
			if( text.FindText(0,mFilterText,pos) == true )
			{
				mRowOrderDisplayToDBArray.Add(i);
			}
		}
		//�t�B���^���[�h�ɐݒ�
		mDisplayRowOrderMode = kDisplayRowOrderMode_Filtered;
	}
	else
	{
		//==================�ʏ탂�[�h�i�܂肽���ݓK�p�j==================
		
		//�e�s�f�[�^���Ƃ̃��[�v
		for( AIndex i = 0; i < GetRowCount(); i++ )
		{
			//���̍��ڂ�mRowOrderDisplayToDBArray�ɒǉ��i���\������j
			mRowOrderDisplayToDBArray.Add(i);
			//���̍��ڂ̐܂肽���ݏ�Ԃ��擾
			ABool	collapsed = mDataBase.GetData_BoolArray(kOutlineFoldingCollapsed,i);
			if( collapsed == true )
			{
				//------------------�܂肽���݂̏ꍇ------------------
				//���̍��ڂ̐܂肽���݃��x�����擾
				AIndex	collapseLevel = mDataBase.GetData_NumberArray(kOutlineFoldingLevel,i);
				//���̍��ڂ�collapseLevel�ȉ��ƂȂ鍀�ڂ܂łƂ΂��i����\���ɂ���j�B
				for( ; i+1 < GetRowCount(); i++ )
				{
					if( mDataBase.GetData_NumberArray(kOutlineFoldingLevel,i+1) <= collapseLevel )
					{
						break;
					}
				}
			}
		}
		//�ʏ탂�[�h�ɐݒ�
		mDisplayRowOrderMode = kDisplayRowOrderMode_Normal;
	}
	
	//���ݑI���s���\���s�̐����I�[�o�[������A�I����������
	if( mSelectedRowDisplayIndex >= mRowOrderDisplayToDBArray.GetItemCount() )
	{
		mSelectedRowDisplayIndex = kIndex_Invalid;
	}
	//�����I���͖���������
	mSelectedRowDisplayIndexArray.DeleteAll();
	//�C���[�W�T�C�Y�X�V
	SetImageSize();
	
	if( inRedraw == true )
	{
		//�ŏ��ɃX�N���[��
		NVI_ScrollTo(kImagePoint_00);
		//�`��X�V
		NVI_Refresh();
	}
}

#pragma mark ===========================

#pragma mark ---�t�B���^

/**
�t�B���^�e�L�X�g�ݒ�
*/
void	AView_List::SPI_SetFilterText( const ADataID inColumnID, const AText& inFilterText )
{
	//���݂Ɠ����Ȃ牽�����Ȃ�
	if( mFilterText.Compare(inFilterText) == true )
	{
		return;
	}
	//�t�B���^�ݒ�
	mFilterText.SetText(inFilterText);
	mFilterColumnID = inColumnID;
	//�\���s�X�V
	UpdateRowOrderDisplay();
}

#pragma mark ===========================

#pragma mark ---�t�H�[���f�B���O

/**
�܂肽���܂�Ă��邩�ǂ������擾
*/
ABool	AView_List::SPI_IsCollapsed( const AIndex inDisplayRowIndex ) const
{
	AIndex	DBIndex = mRowOrderDisplayToDBArray.Get(inDisplayRowIndex);
	return mDataBase.GetData_BoolArray(kOutlineFoldingCollapsed,DBIndex);
}

/**
�܂肽���݁E�W�J
*/
void	AView_List::SPI_ExpandCollapse( const AIndex inDisplayRowIndex, const ABool inSelectRow )
{
	//DB�X�V
	AIndex	DBIndex = mRowOrderDisplayToDBArray.Get(inDisplayRowIndex);
	mDataBase.SetData_BoolArray(kOutlineFoldingCollapsed,DBIndex,
				!mDataBase.GetData_BoolArray(kOutlineFoldingCollapsed,DBIndex));
	
	if( inSelectRow == true )
	{
		//==================�s�I���X�V����ꍇ==================
		//�\���s�X�V
		UpdateRowOrderDisplay(false);
		//�s�I��
		SPI_SetSelectByDBIndex(DBIndex,true);
		//�`��X�V
		NVI_Refresh();
	}
	else
	{
		//==================�s�I���X�V���Ȃ��ꍇ==================
		//���X�̑I���s�擾
		AIndex	origSelectDBIndex = kIndex_Invalid;
		if( mSelectedRowDisplayIndex != kIndex_Invalid )
		{
			origSelectDBIndex = mRowOrderDisplayToDBArray.Get(mSelectedRowDisplayIndex);
		}
		AImagePoint	origin = {0};
		NVI_GetOriginOfLocalFrame(origin);
		//�\���s�X�V
		UpdateRowOrderDisplay(false);
		//�I���s����
		SPI_SetSelectByDBIndex(origSelectDBIndex,false);
		NVI_ScrollTo(origin,false);
		//�`��X�V
		NVI_Refresh();
	}
}

/**
�܂肽���݉\���ǂ����i�q�����݂��j���擾
*/
ABool	AView_List::HasFoldingChild( const AIndex inDBRowIndex ) const
{
	//���̍��ڂ̃A�E�g���C�����x�����[�����true��Ԃ�
	AIndex	collapseLevel = mDataBase.GetData_NumberArray(kOutlineFoldingLevel,inDBRowIndex);
	if( inDBRowIndex+1 < GetRowCount() )
	{
		if( mDataBase.GetData_NumberArray(kOutlineFoldingLevel,inDBRowIndex+1) > collapseLevel )
		{
			return true;
		}
	}
	return false;
}

/**
�S�ēW�J
*/
void	AView_List::SPI_ExpandAll()
{
	//�S��collapse����
	for( AIndex i = 0; i < GetRowCount(); i++ )
	{
		mDataBase.SetData_BoolArray(kOutlineFoldingCollapsed,i,false);
	}
	mDisplayRowOrderMode = kDisplayRowOrderMode_Normal;
	//�\���s�X�V
	UpdateRowOrderDisplay();
	//�X�N���[������
	SPI_AdjustScroll();
}

/**
�������x���̑S�Ă�܂肽���ށE�W�J���A�X�N���[���ʒu�𒲐��i�w��s�������ꏊ�ɕ\�������悤�ɂ���j
*/
void	AView_List::SPI_ExpandCollapseAllForSameLevel( const AIndex inDisplayRowIndex )
{
	//�w��s��rect���L��
	AImageRect	rowImageRect = {0};
	GetRowImageRect(inDisplayRowIndex,inDisplayRowIndex+1,rowImageRect);
	ALocalRect	rowLocalRect = {0};
	NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
	//�܂肽���݁A�W�J
	AIndex	DBRowIndex = mRowOrderDisplayToDBArray.Get(inDisplayRowIndex);
	AIndex	outlineLevel = SPI_GetOutlineFoldingLevel(DBRowIndex);
	if( SPI_IsCollapsed(inDisplayRowIndex) == false )
	{
		SPI_CollapseAllForLevel(outlineLevel);
	}
	else
	{
		SPI_ExpandAllForLevel(outlineLevel);
	}
	//�w��sinDisplayRowIndex��expand/collapse�O�Ɠ����ʒu�ɕ\�������悤�ɃX�N���[���ʒu�𒲐�����
	AIndex	rowDisplayIndex = mRowOrderDisplayToDBArray.Find(DBRowIndex);
	AImageRect	newRowImageRect = {0};
	GetRowImageRect(rowDisplayIndex,rowDisplayIndex+1,newRowImageRect);
	AImagePoint	imagept = {0};
	imagept.y = newRowImageRect.top - rowLocalRect.top;
	NVI_ScrollTo(imagept);
}

/**
�������x���̑S�Ă�܂肽����
*/
void	AView_List::SPI_CollapseAllForLevel( const ANumber inLevel )
{
	//�����A�E�g���C�����x����S�Đ܂肽����
	for( AIndex i = 0; i < GetRowCount(); i++ )
	{
		if( mDataBase.GetData_NumberArray(kOutlineFoldingLevel,i) == inLevel )
		{
			mDataBase.SetData_BoolArray(kOutlineFoldingCollapsed,i,true);
		}
	}
	mDisplayRowOrderMode = kDisplayRowOrderMode_Normal;
	//�\���s�X�V
	UpdateRowOrderDisplay();
}

/**
�������x���̑S�Ă�W�J
*/
void	AView_List::SPI_ExpandAllForLevel( const ANumber inLevel )
{
	//�����A�E�g���C�����x����S�ēW�J
	for( AIndex i = 0; i < GetRowCount(); i++ )
	{
		if( mDataBase.GetData_NumberArray(kOutlineFoldingLevel,i) == inLevel )
		{
			mDataBase.SetData_BoolArray(kOutlineFoldingCollapsed,i,false);
		}
	}
	mDisplayRowOrderMode = kDisplayRowOrderMode_Normal;
	//�\���s�X�V
	UpdateRowOrderDisplay();
}

/**
�A�E�g���C�����x���擾
*/
ANumber	AView_List::SPI_GetOutlineFoldingLevel( const AIndex inDBRowIndex ) const
{
	return mDataBase.GetData_NumberArray(kOutlineFoldingLevel,inDBRowIndex);
}

/**
�܂肽���ݍs�̃e�L�X�g���X�g���擾
*/
void	AView_List::SPI_GetCollapseRowsTextArray( const ADataID inColumnID, ATextArray& outTextArray ) const
{
	outTextArray.DeleteAll();
	for( AIndex i = 0; i < GetRowCount(); i++ )
	{
		if( mDataBase.GetData_BoolArray(kOutlineFoldingCollapsed,i) == true )
		{
			AText	text;
			mDataBase.GetData_TextArray(inColumnID,i,text);
			outTextArray.Add(text);
		}
	}
}

/**
�e�L�X�g���X�g�Ɉ�v����s�̂ݐ܂肽����
*/
void	AView_List::SPI_ApplyCollapseRowsByTextArray( const ADataID inColumnID, const ATextArray& inTextArray ) 
{
	//�S�ēW�J
	if( mDataBase.GetItemCount_BoolArray(kOutlineFoldingCollapsed) == 0 )
	{
		for( AIndex i = 0; i < GetRowCount(); i++ )
		{
			mDataBase.Add_BoolArray(kOutlineFoldingCollapsed,false);
		}
	}
	//hash text array�փR�s�[
	AHashTextArray	hashTextArray;
	hashTextArray.SetFromTextArray(inTextArray);
	//�e�L�X�g����v���鍀�ڂ�܂肽����
	for( AIndex i = 0; i < GetRowCount(); i++ )
	{
		AText	text;
		mDataBase.GetData_TextArray(inColumnID,i,text);
		if( hashTextArray.Find(text) != kIndex_Invalid )
		{
			mDataBase.SetData_BoolArray(kOutlineFoldingCollapsed,i,true);
		}
	}
}

#pragma mark ===========================

#pragma mark ---

/*#1012
//Icon�ݒ� win 080618
void	AView_List::SPI_SetColumnIcon( const ADataID inColumnID, const ANumberArray& inIconIDArray )
{
	mIconDataBase.SetData_NumberArray(inColumnID,inIconIDArray);
}
*/

//
void	AView_List::UpdateScrollBarUnit()
{
	const ANumber				kHScrollBarUnit = 30;
	
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(kHScrollBarUnit,GetRowHeight(),rect.right-rect.left-kHScrollBarUnit,rect.bottom-rect.top-GetRowHeight()*5);
}

//�t�H���g����Text�`��v���p�e�B���ύX���ꂽ�ꍇ�̏���
/*win void	AView_List::SPI_SetTextDrawProperty( const AFont inFont, const AFloatNumber inFontSize, const AColor& inColor )
{
	mFont = inFont;
	mFontSize = inFontSize;
	mColor = inColor;
	NVMC_SetDefaultTextProperty(mFont,mFontSize,mColor,kTextStyle_Normal,true);
	NVMC_GetMetricsForDefaultTextProperty(mRowHeight,mAscent);
	mRowHeight += mRowMargin;
	UpdateScrollBarUnit();
	//LineHeight���ύX���ꂽ�̂�ImageSize���ύX�K�v
	SetImageSize();
}*/
void	AView_List::SPI_SetTextDrawProperty( const AText& inFontName, const AFloatNumber inFontSize, const AColor& inColor )
{
	mFontName.SetText(inFontName);
	mFontSize = inFontSize;
	mColor = inColor;
	NVMC_SetDefaultTextProperty(mFontName,mFontSize,mColor,kTextStyle_Normal,true);
	NVMC_GetMetricsForDefaultTextProperty(mRowHeight,mAscent);
	mRowHeight += mRowMargin;
	UpdateScrollBarUnit();
	//LineHeight���ύX���ꂽ�̂�ImageSize���ύX�K�v
	SetImageSize();
}

void	AView_List::SPI_SetFontSize( const AFloatNumber inFontSize )
{
	AText	fontname(mFontName);
	SPI_SetTextDrawProperty(fontname,inFontSize,mColor);
}

//
ANumber	AView_List::GetRowHeight() const
{
	return mRowHeight;
}

AItemCount	AView_List::GetRowCount() const
{
	if( mColumnIDArray.GetItemCount() == 0 )   return 0;
	else
	{
		APrefID	colID = mColumnIDArray.Get(0);
		return mDataBase.GetItemCount_Array(colID);
	}
}

void	AView_List::SetImageSize()
{
	//win 080618
	//width���A�S�Ă̗�̕��̘a���AView�̕��̂ǂ��炩�傫�����ɐݒ�
	ANumber	width = 0;
	for( AIndex i = 0; i < mColumnWidthArray.GetItemCount(); i++ )
	{
		width += mColumnWidthArray.Get(i);
	}
	if( width < NVI_GetViewWidth() )
	{
		width = NVI_GetViewWidth();
	}
	
	ANumber	height = /*#789 GetRowCount()*/SPI_GetDisplayRowCount() * GetRowHeight() + 8;
	NVM_SetImageSize(width,height);
	
	//
	AImagePoint	origin = {0};
	NVI_GetOriginOfLocalFrame(origin);
	NVI_ScrollTo(origin);
}

//
void	AView_List::GetRowImageRect( const AIndex inStartRowIndex, const AIndex inEndRowIndex, AImageRect& outRowImageRect ) const
{
	outRowImageRect.left	= 0;
	outRowImageRect.right	= NVM_GetImageWidth();
	outRowImageRect.top		= inStartRowIndex*GetRowHeight();
	outRowImageRect.bottom	= inEndRowIndex*GetRowHeight();
}

//win 080618
//�Z����ImageRect���擾
void	AView_List::GetCellImageRect( const APrefID inColumnID, const AIndex inRowIndex, AImageRect& outRowImageRect ) const
{
	GetRowImageRect(inRowIndex,inRowIndex+1,outRowImageRect);
	AIndex	colIndex = mColumnIDArray.Find(inColumnID);
	if( colIndex == kIndex_Invalid )   {_ACError("",this);return;}
	for( AIndex i = 0; i < colIndex; i++ )
	{
		outRowImageRect.left += mColumnWidthArray.Get(i);
	}
	if( colIndex != mColumnIDArray.GetItemCount() - 1 )
	{
		outRowImageRect.right = outRowImageRect.left + mColumnWidthArray.Get(colIndex);
	}
}


void	AView_List::ClearCursorRow()
{
	if( mCursorRowStartDisplayIndex == kIndex_Invalid )   return;
	//�`��X�V
	AIndex	svStartRowDisplayIndex = mCursorRowStartDisplayIndex;
	AIndex	svEndRowDisplayIndex = mCursorRowEndDisplayIndex;
	mCursorRowStartDisplayIndex = mCursorRowEndDisplayIndex = kIndex_Invalid;
	mCursorRowLeftMargin = 0;
	SPI_RefreshRows(svStartRowDisplayIndex,svEndRowDisplayIndex);
}

void	AView_List::SPI_SortOff()
{
	mSortMode = false;
	UpdateRowOrderDisplay();
}

void	AView_List::SPI_Sort( const APrefID inColumnID, const ABool inAscendant )
{
	mSortMode = true;
	mSortColumnID = inColumnID;
	mSortAscendant = inAscendant;
	UpdateRowOrderDisplay();
}

void	AView_List::QuickSort( const ADataID inColumnID, AIndex first, AIndex last, const ABool inAscendant )
{
	switch(mDataBase.GetDataType(inColumnID))
	{
	  case kDataType_TextArray:
		{
			AText	axis;
			mDataBase.GetData_TextArray(inColumnID,mRowOrderDisplayToDBArray.Get((first+last)/2),axis);
			AIndex	i = first, j = last;
			for( ; ; )
			{
				for( ; ; )
				{
					AText	data;
					mDataBase.GetData_TextArray(inColumnID,mRowOrderDisplayToDBArray.Get(i),data);
					if( inAscendant == true )
					{
						if( data.IsLessThan(axis) == true )
						{
							i++;
							continue;
						}
					}
					else
					{
						if( axis.IsLessThan(data) == true )
						{
							i++;
							continue;
						}
					}
					break;
				}
				for( ; ; )
				{
					AText	data;
					mDataBase.GetData_TextArray(inColumnID,mRowOrderDisplayToDBArray.Get(j),data);
					if( inAscendant == true )
					{
						if( axis.IsLessThan(data) == true )
						{
							j--;
							continue;
						}
					}
					else
					{
						if( data.IsLessThan(axis) == true )
						{
							j--;
							continue;
						}
					}
					break;
				}
				if( i >= j )   break;
				AIndex	tmp = mRowOrderDisplayToDBArray.Get(i);
				mRowOrderDisplayToDBArray.Set(i,mRowOrderDisplayToDBArray.Get(j));
				mRowOrderDisplayToDBArray.Set(j,tmp);
				i++;
				j--;
			}
			if( first < i-1 )   QuickSort(inColumnID,first,i-1,inAscendant);
			if( j+1 < last  )   QuickSort(inColumnID,j+1,last,inAscendant);
			break;
		}
		//#138
	  case kDataType_NumberArray:
		{
			ANumber	axis = mDataBase.GetData_NumberArray(inColumnID,mRowOrderDisplayToDBArray.Get((first+last)/2));
			AIndex	i = first, j = last;
			for( ; ; )
			{
				for( ; ; )
				{
					ANumber	data = mDataBase.GetData_NumberArray(inColumnID,mRowOrderDisplayToDBArray.Get(i));
					if( inAscendant == true )
					{
						if( data < axis )
						{
							i++;
							continue;
						}
					}
					else
					{
						if( axis < data )
						{
							i++;
							continue;
						}
					}
					break;
				}
				for( ; ; )
				{
					ANumber	data = mDataBase.GetData_NumberArray(inColumnID,mRowOrderDisplayToDBArray.Get(j));
					if( inAscendant == true )
					{
						if( axis < data )
						{
							j--;
							continue;
						}
					}
					else
					{
						if( data < axis )
						{
							j--;
							continue;
						}
					}
					break;
				}
				if( i >= j )   break;
				AIndex	tmp = mRowOrderDisplayToDBArray.Get(i);
				mRowOrderDisplayToDBArray.Set(i,mRowOrderDisplayToDBArray.Get(j));
				mRowOrderDisplayToDBArray.Set(j,tmp);
				i++;
				j--;
			}
			if( first < i-1 )   QuickSort(inColumnID,first,i-1,inAscendant);
			if( j+1 < last  )   QuickSort(inColumnID,j+1,last,inAscendant);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

void	AView_List::SPI_RefreshRow( const AIndex inDisplayRowIndex )
{
	if( inDisplayRowIndex == kIndex_Invalid )   return;
	AImageRect	rowImageRect;
	GetRowImageRect(inDisplayRowIndex,inDisplayRowIndex+1,rowImageRect);
	ALocalRect	rowLocalRect;
	NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
	NVMC_RefreshRect(rowLocalRect);
}

/**
�`��X�V
*/
void	AView_List::SPI_RefreshRows( const AIndex inDisplayRowStartIndex, const AIndex inDisplayRowEndIndex )
{
	AImageRect	rowImageRect;
	GetRowImageRect(inDisplayRowStartIndex,inDisplayRowEndIndex+1,rowImageRect);
	ALocalRect	rowLocalRect;
	NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
	NVMC_RefreshRect(rowLocalRect);
}

//win 080618
//�X�N���[���㏈��
void	AView_List::NVIDO_ScrollPostProcess( const ANumber inDeltaX, const ANumber inDeltaY,
											 const ABool inRedraw, const ABool inConstrainToImageSize,
											 const AScrollTrigger inScrollTrigger )//#138 #1031
{
	//�t���[���̃X�N���[���ʒu�����킹��
	if( /*#205 mFrameControlID != kControlID_Invalid*/mFrameViewID != kObjectID_Invalid )
	{
		AImagePoint	pt;
		NVI_GetOriginOfLocalFrame(pt);
		/*#205 NVM_GetWindow().NVI_GetListFrameView(mFrameControlID)*/GetFrameView().SPI_SetHeaderOffset(pt.x);
	}
	//�`��X�V
	NVI_Refresh();
}

//#205
/**
�e�[�u���ւ̃����O�X0�̃e�L�X�g���͂��֎~����
*/
void	AView_List::SPI_SetInhibit0Length( const ABool inInhibit0Length )
{
	SPI_SetInhibit0Length(inInhibit0Length, GetEmptyText());
}
void	AView_List::SPI_SetInhibit0Length( const ABool inInhibit0Length, const AText& in0LengthText )//#1428
{
	mInhibit0Length = inInhibit0Length;
	m0LengthText.SetText(in0LengthText);//#1428
}

#pragma mark ===========================

#pragma mark ---Edit

//�ҏW���[�h�J�n
//inColumnID: ��ID
//inRowIndex: �s�C���f�b�N�X�ikIndex_Invalid�Ȃ�I���s�j
void	AView_List::SPI_StartEditMode( const APrefID inColumnID, const AIndex inRowIndex )
{
	//���ɕҏW���[�h�Ȃ�ҏW���[�h���I��������
	if( mEditMode == true )
	{
		SPI_EndEditMode();
	}
	
	//��C���f�b�N�X�擾
	AIndex	colIndex = mColumnIDArray.Find(inColumnID);
	if( colIndex == kIndex_Invalid )   {_ACError("",this);return;}
	
	//�s�C���f�b�N�X�擾
	AIndex	rowIndex = inRowIndex;
	if( rowIndex == kIndex_Invalid )
	{
		rowIndex = mSelectedRowDisplayIndex;
	}
	if( rowIndex == kIndex_Invalid )   return;
	
	//EditBox�R���g���[�������݂��Ă�����AEditBox�R���g���[�����폜����
	if( NVM_GetWindow().NVI_ExistView(kEditBoxControlID) == true )
	{
		NVM_GetWindow().NVI_DeleteEditBoxView(kEditBoxControlID);
	}
	
	//EditBox�R���g���[�������A�����ݒ�
	AIndex	tabIndex = NVM_GetWindow().NVI_GetControlEmbeddedTabControlIndex(NVI_GetControlID());
	AImageRect	imagerect;
	GetCellImageRect(inColumnID,rowIndex,imagerect);
	AImagePoint	imagept;
	imagept.x = imagerect.left;
	imagept.y = imagerect.top;
	ALocalPoint	localpt;
	NVM_GetLocalPointFromImagePoint(imagept,localpt);
	AWindowPoint	windowpt;
	NVM_GetWindow().NVI_GetWindowPointFromControlLocalPoint(NVI_GetControlID(),localpt,windowpt);
	AWindowPoint	pt;
	pt.x = windowpt.x;
	pt.y = windowpt.y;
	NVM_GetWindow().NVI_CreateEditBoxView(kEditBoxControlID,pt,imagerect.right-imagerect.left,imagerect.bottom-imagerect.top+kEditBoxBottomMargin,
				kControlID_Invalid,
				tabIndex,true);
	NVM_GetWindow().NVI_EmbedControl(NVI_GetControlID(),kEditBoxControlID);
	AText	text;
	switch(mDataBase.GetDataType(inColumnID))
	{
	  case kDataType_TextArray:
		{
			mDataBase.GetData_TextArray(inColumnID,rowIndex,text);
			break;
		}
	  case kDataType_NumberArray://#868
		{
			text.SetNumber(mDataBase.GetData_NumberArray(inColumnID,rowIndex));
			break;
		}
	  default:
		{
			_ACError("",NULL);
			break;
		}
	}
	NVM_GetWindow().NVI_GetEditBoxView(kEditBoxControlID).SPI_SetListViewID(NVI_GetControlID());
	NVM_GetWindow().NVI_SetControlText(kEditBoxControlID,text);
	NVM_GetWindow().NVI_GetEditBoxView(kEditBoxControlID).NVI_SetSelectAll();
	NVM_GetWindow().NVI_SwitchFocusTo(kEditBoxControlID);
	
	mEditMode = true;
	mEditModeColumnID = inColumnID;
	mEditModeRowIndex = rowIndex;
}

//�ҏW���[�h�I��
void	AView_List::SPI_EndEditMode()
{
	if( mEditMode == false )   return;
	if( NVM_GetWindow().NVI_ExistView(kEditBoxControlID) == false )
	{
		//����ListView��EditBox���J�����ꍇ
		mEditMode = false;
		return;
	}
	//EditBox�R���g���[���폜
	NVM_GetWindow().NVI_DeleteEditBoxView(kEditBoxControlID);
}

//EditBox�ŕ������͂��ꂽ�Ƃ��ɃR�[�������
//���s���͂ŁAListView�̕ҏW���[�h���I������
ABool	AView_List::SPI_EditBoxInput( const AUChar inChar )
{
	if( inChar == '\r' )
	{
		SPI_EndEditMode();
		return true;
	}
	return false;
}

//EditBox���폜���ꂽ�Ƃ��ɁAEditBox�̃f�X�g���N�^����R�[�������B
void	AView_List::SPI_DoEditBoxDeleted( const AText& inText )
{
	//#205
	ABool	ok = true;
	if( mInhibit0Length == true )
	{
		if( inText.GetItemCount() == 0 )   ok = false;
	}
	if( ok == true )
	{
		switch(mDataBase.GetDataType(mEditModeColumnID))
		{
		  case kDataType_TextArray:
			{
				mDataBase.SetData_TextArray(mEditModeColumnID,mEditModeRowIndex,inText);
				break;
			}
		  case kDataType_NumberArray://#868
			{
				mDataBase.SetData_NumberArray(mEditModeColumnID,mEditModeRowIndex,inText.GetNumber());
				break;
			}
		  default:
			{
				_ACError("",NULL);
				break;
			}
		}
	}
	
	mEditMode = false;
	NVM_GetWindow().EVT_ValueChanged(NVI_GetControlID());
}

#pragma mark ===========================
#pragma mark [�N���X]AView_ListFrame
#pragma mark ===========================
//

#if IMPLEMENTATION_FOR_WINDOWS
const AColor	kListFrameColor = kColor_Gray;
#endif
#if IMPLEMENTATION_FOR_MACOSX
const AColor	kListFrameColor = kColor_LightGray;
#endif

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_ListFrame::AView_ListFrame( const AWindowID inWindowID, const AControlID inID, const AControlID inListViewControlID,
			const ABool inHeader, const AControlID inVScrollBarControlID, const AControlID inHScrollBarControlID ) 
: AView(inWindowID,inID), 
		mHeaderEnable(inHeader), mVScrollBarControlID(inVScrollBarControlID), mHScrollBarControlID(inHScrollBarControlID)
		,mFocused(false), mListViewControlID(inListViewControlID)
,mMouseTrackingPreviousLocalPoint(kLocalPoint_00),mMouseTrackingColumnIndex(kIndex_Invalid), mHeaderOffset(0)
{
	/*win 080707 Init�ֈړ�
	AText	fontname;
	AFontWrapper::GetDefaultFontName(fontname);
	NVMC_SetDefaultTextProperty(fontname,10.5,kColor_Black,kTextStyle_Normal,true);
	//
	SPI_UpdateBounds();
	*/
}
//�f�X�g���N�^
AView_ListFrame::~AView_ListFrame()
{
}

//Init 080707
void	AView_ListFrame::NVIDO_Init()
{
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);//#375
	NVMC_SetDefaultTextProperty(fontname,kListFrameDefaultFontSize,kColor_Black,kTextStyle_Normal,true);
	//win
	NVM_GetWindow().NVI_DisableEventCatch(NVI_GetControlID());
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_ListFrame::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_ListFrame::EVTDO_DoDraw()
{
	//#1316
	AColor	frameColor = AColorWrapper::GetColorByHTMLFormatColor("D0D0D0");
	AColor	fillColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
	AColor	letterColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
	if( AApplication::GetApplication().NVI_IsDarkMode() == true )
	{
		frameColor = AColorWrapper::GetColorByHTMLFormatColor("505050");
		fillColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
		letterColor = AColorWrapper::GetColorByHTMLFormatColor("909090");
	}
	//
	ALocalRect	localFrame;
	NVM_GetLocalViewRect(localFrame);
	NVMC_PaintRect(localFrame,fillColor,1.0);
	//�w�b�_�`��
	if( mHeaderEnable == true )
	{
		ALocalRect	headerRect = localFrame;
		headerRect.bottom = headerRect.top + kHeaderHeight;
		/*#1316
		//Mac OS�̃��X�g�w�b�_�Ɏ�����gradient�`��
		ALocalRect	headerRect1 = headerRect;
		headerRect1.bottom = headerRect.top + kHeaderHeight/2;
		NVMC_PaintRectWithVerticalGradient(headerRect1,kColor_White,kColor_Gray90Percent,1.0,1.0);
		ALocalRect	headerRect2 = headerRect;
		headerRect2.top = headerRect.top + kHeaderHeight/2;
		NVMC_PaintRectWithVerticalGradient(headerRect2,kColor_Gray90Percent,kColor_White,1.0,1.0);
		*/
		AColor	backgroundStartColor = AColorWrapper::GetColorByHTMLFormatColor("ECECEC");//D9D9D9");
		AColor	backgroundEndColor = AColorWrapper::GetColorByHTMLFormatColor("ECECEC");//F7F7F7");
		if( AApplication::GetApplication().NVI_IsDarkMode() == true )
		{
			backgroundStartColor = AColorWrapper::GetColorByHTMLFormatColor("383838");//282828");
			backgroundEndColor = AColorWrapper::GetColorByHTMLFormatColor("303030");//"181818");
		}
		NVMC_PaintRectWithVerticalGradient(headerRect,backgroundStartColor,backgroundEndColor,1.0,1.0);
		//�t���[���`��
		NVMC_FrameRect(headerRect,frameColor);//#1316 kListFrameColor,1.0);
		//
		ANumber	x = 0;
		for( AIndex i = 0; i < mColumnWidth.GetItemCount(); i++ )
		{
			//�^�C�g��
			AText	title;
			mColumnTitle.Get(i,title);
			ALocalRect	drawRect;
			drawRect.left		= x + 7 - mHeaderOffset;
			drawRect.right		= x + mColumnWidth.Get(i) - mHeaderOffset;
			drawRect.top		= headerRect.top + 3;
			drawRect.bottom		= headerRect.bottom -1;
			//�Ō�̗�͉E�[�܂�
			if( i == mColumnWidth.GetItemCount() - 1 )
			{
				drawRect.right = headerRect.right-1;
			}
			//�^�C�g���`��
			/*#1316
			AColor	color = kColor_Black;
			if( NVM_GetWindow().NVI_IsWindowActive() == false )
			{
				color = kColor_Gray;
			}
			*/
			NVMC_DrawText(drawRect,title,letterColor,kTextStyle_Normal);
			
			//
			x += mColumnWidth.Get(i);
			//��؂��
			if( i < mColumnWidth.GetItemCount() - 1 )
			{
				ALocalPoint	spt, ept;
				spt.x = x - mHeaderOffset;
				spt.y = headerRect.top;
				ept.x = x - mHeaderOffset;
				ept.y = headerRect.bottom-1;//win 080618
				NVMC_DrawLine(spt,ept,frameColor);//#1316 kListFrameColor);
			}
		}
	}
	//
	AColor	color = frameColor;//#1316 kListFrameColor;
	/*#1316
	if( NVM_GetWindow().NVI_IsWindowActive() == false )
	{
		color = kColor_Gray80Percent;
	}
	*/
	if( mFocused == true )
	{
		AColorWrapper::GetHighlightColor(color);
		NVMC_FrameRect(localFrame,color,1.0);
		//NVMC_DrawFocusFrame(localFrame);
	}
	else
	{
		NVMC_FrameRect(localFrame,color,1.0);
	}
}

/**
�}�E�X�{�^���_�E��������
*/
ABool	AView_ListFrame::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//�O��}�E�X�ʒu�̕ۑ�
	mMouseTrackingPreviousLocalPoint = inLocalPoint;
	mMouseTrackingColumnIndex = kIndex_Invalid;
	ANumber	x = -mHeaderOffset;
	for( AIndex i = 0; i < mColumnWidth.GetItemCount()-1; i++ )
	{
		x += mColumnWidth.Get(i);
		if( inLocalPoint.x > x-5 && inLocalPoint.x < x+5 )
		{
			mMouseTrackingColumnIndex = i;
			break;
		}
	}
	if( mMouseTrackingColumnIndex == kIndex_Invalid )
	{
		return false;
	}
	
	//Tracking���[�h�ݒ�
	NVM_SetMouseTrackingMode(true);
	NVI_Refresh();
	return true;
}

/**
�}�E�X�{�^��������Tracking��OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_ListFrame::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
	if( inLocalPoint.x == mMouseTrackingPreviousLocalPoint.x && inLocalPoint.y == mMouseTrackingPreviousLocalPoint.y )
	{
		return;
	}
	//�J�������ύX
	ANumber	w = mColumnWidth.Get(mMouseTrackingColumnIndex);
	w += inLocalPoint.x - mMouseTrackingPreviousLocalPoint.x;
	mColumnWidth.Set(mMouseTrackingColumnIndex,w);
	NVM_GetWindow().NVI_GetListView(mListViewControlID).SPI_SetColumnWidth(mColumnID.Get(mMouseTrackingColumnIndex),w);
	//�`��X�V
	NVI_Refresh();
	NVM_GetWindow().NVI_GetListView(mListViewControlID).NVI_Refresh();
	//
	mMouseTrackingPreviousLocalPoint = inLocalPoint;
}

/**
�}�E�X�{�^��������Tracking�I������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_ListFrame::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	NVM_SetMouseTrackingMode(false);
	mMouseTrackingColumnIndex = kIndex_Invalid;
}

/**
�J�[�\���^�C�v�擾
*/
ACursorType	AView_ListFrame::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ABool	resize = false;
	ANumber	x = -mHeaderOffset;
	for( AIndex i = 0; i < mColumnWidth.GetItemCount()-1; i++ )
	{
		x += mColumnWidth.Get(i);
		if( inLocalPoint.x > x-5 && inLocalPoint.x < x+5 )
		{
			resize = true;
			break;
		}
	}
	if( resize == true )
	{
		return kCursorType_ResizeLeftRight;
	}
	else
	{
		return kCursorType_Arrow;
	}
}

//win
/**
�t�H�[�J�X�ݒ�
*/
void	AView_ListFrame::SPI_SetFocused( const ABool inFocused )
{
	mFocused = inFocused;
	NVI_Refresh();
}

//��o�^
void	AView_ListFrame::SPI_RegisterColumn( const ADataID inColumnID, const ANumber inWidth, const AText& inTitle )
{
	mColumnID.Add(inColumnID);
	mColumnWidth.Add(inWidth);
	mColumnTitle.Add(inTitle);
	NVI_Refresh();
}

//��̕��ݒ�
void	AView_ListFrame::SPI_SetColumnWidth( const AIndex inIndex, const ANumber inWidth )
{
	mColumnWidth.Set(inIndex,inWidth);
	NVI_Refresh();
}

//�w�b�_�̃X�N���[���ʒu�ݒ�
void	AView_ListFrame::SPI_SetHeaderOffset( const ANumber inHeaderOffset )
{
	mHeaderOffset = inHeaderOffset;
	NVI_Refresh();
}

