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

AView_LineNumber

*/

#include "stdafx.h"

#include "AView_LineNumber.h"
#include "AWindow_Text.h"
#include "ADocument_Text.h"
#include "AView_Text.h"
#include "AApp.h"

//�s�ԍ��\���̃t�H���g�T�C�Y�ő�
const AFloatNumber	kMaxFontSizeForLineNumber = 16.0;

//�t�H���g�T�C�Y �e�L�X�g�{������̔{��
const AFloatNumber	kLineNumberFontMultiply = 0.9;


#pragma mark ===========================
#pragma mark [�N���X]AView_LineNumber
#pragma mark ===========================
//�s�ԍ��\��View
//AView_LineNumber��AView_Text�Ɉˑ����邪�AAView_Text��AView_LineNumber�ɂ͈ˑ����Ȃ��iAView_LineNumber�����ő��݂�����j
//�e�E�C���h�E��AWindow�ł���Ηǂ��AAWindow_Text�ȊO�ł��g�p��

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_LineNumber::AView_LineNumber( /*#199 AObjectArrayItem* inParent, AWindow& inWindow, */const AWindowID inWindowID,
		const AControlID inID, /*#199 const AControlID inControlID_TextView*/const AViewID inTextViewID, const AObjectID inTextDocumentID, const AImagePoint& inOriginOfLocalFrame ) 
		: AView(/*#199 inParent,inWindow*/inWindowID,inID,inOriginOfLocalFrame), /*#199 mControlID_TextView(inControlID_TextView)*/mTextViewID(inTextViewID), mTextDocumentID(inTextDocumentID)
		,mFoldingMarkMouseHoverLineIndex(kIndex_Invalid)//#450
		,mMouseHoverLineIndex(kIndex_Invalid)//#450
,mDiffDrawEnable(false)//#379
,mFoldingHoverStart(kIndex_Invalid),mFoldingHoverEnd(kIndex_Invalid)
,mLeftMargin(0)//#1186
{
	NVMC_SetOffscreenMode(true);//win
	/*win 080709 NVIDO_Init�ֈړ�
	SPI_UpdateTextDrawProperty();
	*/
	
	//view��s�����ɐݒ� #1090
	NVMC_SetOpaque(true);
}

//�f�X�g���N�^
AView_LineNumber::~AView_LineNumber()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_LineNumber::NVIDO_Init()
{
	//�c�������[�h�ݒ� #558
	NVI_SetVerticalMode(GetTextDocumentConst().SPI_GetVerticalMode());
	
	NVMC_EnableMouseLeaveEvent();//#450
	//#450 GetTextDocument().SPI_RegisterLineNumberView(GetObjectID());//#199
	SPI_UpdateTextDrawProperty();
	//#236 NVMC_EnableDrop(kScrapType_UnicodeText);
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_UnicodeText);
	scrapTypeArray.Add(kScrapType_File);//win
	NVMC_EnableDrop(scrapTypeArray);
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_LineNumber::NVIDO_DoDeleted()
{
	//#450 GetTextDocument().SPI_UnregisterLineNumberView(GetObjectID());//#199
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//TextDocument�擾
ADocument_Text&	AView_LineNumber::GetTextDocument()
{
	return GetApp().SPI_GetTextDocumentByID(mTextDocumentID);
}
const ADocument_Text&	AView_LineNumber::GetTextDocumentConst() const
{
	return GetApp().SPI_GetTextDocumentConstByID(mTextDocumentID);
}

//DocPrefDB�擾
ADocPrefDB&	AView_LineNumber::GetDocPrefDB()
{
	return GetTextDocument().GetDocPrefDB();
}

//ModePrefDB�擾
AModePrefDB&	AView_LineNumber::GetModePrefDB()
{
	return GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex());
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_LineNumber::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//���肽���݃N���b�N���� #450
	if( GetModePrefDB().GetData_Bool(AModePrefDB::kUseLineFolding) == true )
	{
		AImagePoint	clickImagePoint;
		NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
		ATextPoint	clickTextPoint;
		GetTextView().GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
		
		//�܂肽���݃}�[�Nrect�擾
		ALocalRect	foldingMarkRect = {0};
		GetFoldingMarkLocalRect(clickTextPoint.y,foldingMarkRect);
		if( inLocalPoint.x >= foldingMarkRect.left && inLocalPoint.x <= foldingMarkRect.right &&
					inLocalPoint.y >= foldingMarkRect.top && inLocalPoint.y <= foldingMarkRect.bottom )
		{
			//==================�܂肽���݃}�[�N���N���b�N�̏ꍇ==================
			if( GetTextDocument().SPI_GetFoldingLevel(clickTextPoint.y) != kFoldingLevel_None )
			{
				if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
				{
					//------------------�R�}���hor�R���g���[���L�[������------------------
					GetTextView().SPI_ExpandCollapseAllAtCurrentLevel(clickTextPoint.y,
								(GetTextView().SPI_IsCollapsedLine(clickTextPoint.y)==false),true);
				}
				else
				{
					//------------------�ʏ�N���b�N������------------------
					GetTextView().SPI_ExpandCollapse(clickTextPoint.y,kExpandCollapseType_Switch,true);
				}
			}
			else
			{
				//------------------folding�ُ��ԉ񕜗p------------------
				//FoldingLevel��None��collapsed�Ƃ����ꍇ���ُ��ԉ����ł���悤�ɂ��Ă���
				if( GetTextView().SPI_GetFoldingCollapsed(clickTextPoint.y) == true )
				{
					GetTextView().SPI_ExpandCollapse(clickTextPoint.y,kExpandCollapseType_ForceExpand,true);
				}
			}
			//diff info�E�C���h�E�`��X�V
			GetApp().SPI_GetTextWindowByID(NVM_GetWindow().GetObjectID()).SPI_RefreshDiffInfoWindow();
			//
			return true;
		}
		switch(inClickCount)
		{
			/*����ς�s�ԍ��_�u���N���b�N�ɏ������蓖�Ă���Ǝg���Â炢�B���Ƃ��΍s��{�_�u���N���b�N�������Ƃ��B
		  case 2:
			{
				if( GetTextDocument().SPI_GetFoldingLevel(clickTextPoint.y) != kFoldingLevel_None )
				{
					ALocalRect	foldingMarkRect;
					GetFoldingMarkLocalRect(clickTextPoint.y,foldingMarkRect);
					if( (inLocalPoint.x >= foldingMarkRect.left && inLocalPoint.x <= foldingMarkRect.right &&
									inLocalPoint.y >= foldingMarkRect.top && inLocalPoint.y <= foldingMarkRect.bottom))
					{
						//��������
						//Folding�}�[�N�̃_�u���N���b�N�ɂ͏������蓖�Ă��ׂ��łȂ��B
						//Collapse��Expand��f�������s����\�������邽�߁B
						break;
					}
				}
				//
				if( GetTextDocumentConst().SPI_GetLineCheckedDate(clickTextPoint.y) != 0 )
				{
					GetTextDocument().SPI_SetLineCheckedDate(clickTextPoint.y,0);
				}
				else
				{
					GetTextDocument().SPI_SetLineCheckedDate(clickTextPoint.y,1);
				}
				NVI_Refresh();
				return true;
				break;
			}
			*/
			/*
		  case 2:
			{
				GetTextView().SPI_SwitchFoldingExpandCollapse(clickTextPoint.y);
				return true;
			}
			*/
		}
	}
	
	//==================�ʏ�N���b�N==================
	//�t���[���擾
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	//
	ALocalPoint	pt = inLocalPoint;
	pt.x -= localFrameRect.right;
	if( pt.x < 0 )   pt.x = 0;
	//TextView�̃R���g���[��ID�Ŕ����������Ƃɂ���
	//#199 NVM_GetWindow().EVT_DoMouseDown(mControlID_TextView,pt,inModifierKeys,inClickCount);
	AItemCount	clickCount = inClickCount;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSelectLineByClickLineNumber) == true )
	{
		//�s�ԍ��G���A�V���O���N���b�N�ōs�I��
		clickCount = 3;
	}
	ABool	result =  GetTextView().SPI_DoMouseDown(pt,inModifierKeys,clickCount,true);
	NVM_SetMouseTrackingMode(GetTextView().NVI_GetMouseTrackingMode());//#688
	return result;
}

//#688
/**
Mouse Tracking
*/
void	AView_LineNumber::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�t���[���擾
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	//
	ALocalPoint	pt = inLocalPoint;
	pt.x -= localFrameRect.right;
	if( pt.x < 0 )   pt.x = 0;
	//TextView�̃R���g���[��ID�Ŕ����������Ƃɂ���
	GetTextView().SPI_DoMouseTracking(pt,inModifierKeys,true);
}

//#688
/**
Mouse Tracking�I���i�}�E�X�{�^��Up�j
*/
void	AView_LineNumber::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	//
	ALocalPoint	pt = inLocalPoint;
	pt.x -= localFrameRect.right;
	if( pt.x < 0 )   pt.x = 0;
	//TextView�̃R���g���[��ID�Ŕ����������Ƃɂ���
	GetTextView().EVT_DoMouseTrackEnd(pt,inModifierKeys);
	//#688
	NVM_SetMouseTrackingMode(GetTextView().NVI_GetMouseTrackingMode());
}

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_LineNumber::EVTDO_DoDraw()
{
	//
	const AView_Text&	textview = GetTextView();
	const ADocument_Text&	document = GetApp().SPI_GetTextDocumentByID(mTextDocumentID);
	const AModeIndex	modeIndex = document.SPI_GetModeIndex();
	const AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(modeIndex);
	
	//�����F�擾
	AColor	lettercolor = kColor_Black;
	GetModePrefDB().GetModeData_Color(AModePrefDB::kLetterColor,lettercolor);
	//#379 �s�ԍ��{�̗p�̃t�H���g�ݒ�
	{
		AFloatNumber	fontsize = GetTextDocumentConst().SPI_GetFontSize() *kLineNumberFontMultiply;//#868 GetDocPrefDB().GetData_FloatNumber(ADocPrefDB::kTextFontSize);
		if( fontsize > kMaxFontSizeForLineNumber )   fontsize = kMaxFontSizeForLineNumber;
		AText	fontname;
		//#868 GetDocPrefDB().GetData_Text(ADocPrefDB::kTextFontName,fontname);
		GetTextDocumentConst().SPI_GetFontName(fontname);//#868
		NVMC_SetDefaultTextProperty(fontname,fontsize,lettercolor,kTextStyle_Normal,document.SPI_IsAntiAlias(),0.7);
	}
	
	//
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	//#621
	if( imageFrameRect.top < 0 )
	{
		imageFrameRect.top = 0;
	}
	//
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	//==================�w�i�`��i�w�i��localFrame�S�́i�̂���UpdateRegion���j����C�ɕ`���B�j==================
	//------------------text view�Ɠ����w�i�`��------------------
	AIndex	backgroundImageIndex = modePrefDB.GetBackgroundImageIndex();
	if( modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayBackgroundImage) == true && backgroundImageIndex != kIndex_Invalid )
	{
		//------------------�w�i�摜�`��------------------
		AImagePoint	pt;
		textview.NVI_GetOriginOfLocalFrame(pt);
		AFloatNumber	alpha = modePrefDB.GetModeData_Number(AModePrefDB::kBackgroundImageTransparency);
		alpha /= 100;
		NVMC_DrawBackgroundImage(localFrameRect,imageFrameRect,pt.x,backgroundImageIndex,alpha);
	}
	else
	{
		//------------------�w�i�F�`��------------------
		AColor	color;
		modePrefDB.GetModeData_Color(AModePrefDB::kBackgroundColor,color);
		NVMC_PaintRect(localFrameRect,color,1.0);
	}
	
	//Initial��ԁi�X�N���[�j���O�������j���́A�`�悵�Ȃ�
	if( GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		return;
	}
	
	//#285
	if( modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayEachLineNumber) == false )   return;
	
	//------------------�s�ԍ������̔w�i�F�𓧖��F�ŏ㏑��------------------
	//#725
	AColor	lineNumberBackgroundColor = AColorWrapper::GetColorByHTMLFormatColor("d0d0d0");
	ALocalRect	localLineNumberRect = localFrameRect;
	localLineNumberRect.right -= kRightMargin;
	NVMC_PaintRect(localLineNumberRect,lineNumberBackgroundColor,0.05);
	//NVMC_PaintRectWithHorizontalGradient(localLineNumberRect,lineNumberBackgroundColor,lineNumberBackgroundColor,0.2,0.18);
	
	//------------------���E���`��------------------
	ALocalPoint	spt, ept;
	spt.x = ept.x = localFrameRect.right - kRightMargin;
	spt.y = localFrameRect.top-1;
	ept.y = localFrameRect.bottom;
	/*#725
	//
	AColor	color;
	modePrefDB.GetModeData_Color(AModePrefDB::kLetterColor,color);
	if( (imageFrameRect.top%2) == 1 )   spt.y--;//B0000 �_�����������C�� 080819 spt.y++����ύX�i�X�N���[�����ɓ_���̊Ԃ��J�����߁j
#if IMPLEMENTATION_FOR_WINDOWS
	NVMC_DrawLine(spt,ept,kColor_Gray60Percent);//B0365
#else
	NVMC_DrawLine(spt,ept,color,1.0,1.0);//B0365
	#endif
	*/
	AColor	separatorLineColor = AColorWrapper::GetColorByHTMLFormatColor("808080");
	NVMC_DrawLine(spt,ept,separatorLineColor,0.45);
	
	//==================�e�s�`��==================
	//
	ABool	drawParagraph = modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayEachLineNumber_AsParagraph);
	//R0208 �������i�s�܂�Ԃ������Ȃ�s�ԍ��ƒi���ԍ��͓����j
	if( document.SPI_GetWrapMode() == kWrapMode_NoWrap )
	{
		drawParagraph = false;
	}
	AIndex	startLineIndex = textview.GetLineIndexByImageY(imageFrameRect.top);
	//#450 AIndex	endLineIndex = imageFrameRect.bottom/textview.GetLineHeightWithMargin();//���݂��Ȃ��s�ɂ��E�C���h�E���͔w�i��\�������邽��textview.GetLineIndexByImageY(imageFrameRect.bottom);
	AIndex	lineCount = document.SPI_GetLineCount();//#450
	for( AIndex lineIndex = startLineIndex; lineIndex <= lineCount/*#450endLineIndex*/; lineIndex++ )
	{
		/*#450
		//�s�̕`��̈�v�Z
		AImageRect	textViewLineImageRect;
		textview.GetLineImageRect(lineIndex,textViewLineImageRect);
		AImageRect	lineImageRect;
		ALocalRect	lineLocalRect;
		lineImageRect = textViewLineImageRect;
		lineImageRect.left = imageFrameRect.left;
		lineImageRect.right = imageFrameRect.right;
		*/
		//�s�̕`��̈�擾 #450 �֐���
		AImageRect	lineImageRect;
		GetLineImageRect(lineIndex,lineImageRect);
		//#450 ��\���s����
		if( lineImageRect.top == lineImageRect.bottom )   continue;
		//#450 �I������i���\�����悤�Ƃ��Ă���s��imagerect��frame�O��������I��
		if( lineImageRect.top > imageFrameRect.bottom )   break;
		//
		ALocalRect	lineLocalRect;
		NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
		//UpdateRegion���łȂ��Ȃ�`�悵�Ȃ�
		if( NVMC_IsRectInDrawUpdateRegion(lineLocalRect) == false )   continue;
		
		//�w�i�`��
		/* AIndex	backgroundImageIndex = GetApp().SPI_GetModePrefDB(document.SPI_GetModeIndex()).GetBackgroundImageIndex();
		if( GetApp().SPI_GetModePrefDB(document.SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kDisplayBackgroundImage) == true &&
				backgroundImageIndex != kIndex_Invalid )
		{
			AImagePoint	pt;
			textview.NVI_GetOriginOfLocalFrame(pt);
			AFloatNumber	alpha = GetApp().SPI_GetModePrefDB(document.SPI_GetModeIndex()).GetData_Number(AModePrefDB::kBackgroundImageTransparency);
			alpha /= 100;
			NVMC_DrawBackgroundImage(lineLocalRect,imageFrameRect,pt.x,backgroundImageIndex,alpha);
		}
		else
		{
			AColor	color;
			GetApp().SPI_GetModePrefDB(document.SPI_GetModeIndex()).GetData_Color(AModePrefDB::kBackgroundColor,color);
			NVMC_PaintRect(lineLocalRect,color,1.0);
		}*/
		
		
		//���E���`��
		/* ALocalPoint	spt, ept;
		spt.x = ept.x = lineLocalRect.right +kSeparatorOffset;
		spt.y = lineLocalRect.top;
		ept.y = lineLocalRect.bottom;
		NVMC_DrawLine(spt,ept,kColor_Black,1.0,1.0);*/
		
		//�ŏI�s�ȍ~�͔w�i�A���E�������`��
		if( lineIndex >= document.SPI_GetLineCount() )   continue;
		
		//#844
		//���ݍs�\��
		{
			ATextPoint	spt = {0,0}, ept = {0,0};
			textview.SPI_GetSelect(spt,ept);
			AColor	color = kColor_LightGray;
			GetApp().NVI_GetAppPrefDB().GetData_Color(AAppPrefDB::kCurrentLineColor,color);
			ABool	currentLine = ((spt.y==ept.y)&&(lineIndex==spt.y));
			if( currentLine == true )
			{
				//�s�y�C���g
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPaintLineForCurrentLine) == true )
				{
					NVMC_PaintRect(lineLocalRect,color,0.3);
				}
				//#496 ���ݍs����
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDrawLineForCurrentLine) == true )//#844
				{
					/*
					ALocalPoint	start = {0,0}, end = {0,0};
					start.x = lineLocalRect.left;
					start.y = lineLocalRect.top + textview.GetLineHeightWithoutMargin(lineIndex)-1;
					end.x = lineLocalRect.right;
					end.y = start.y;
					NVMC_DrawLine(start,end,color,0.7);
					*/
					ALocalRect	r = lineLocalRect;
					r.top = r.bottom - 1;
					NVMC_PaintRect(r,color,0.7);
				}
			}
		}
		
		//fprintf(stderr,"lineNumberDraw:%d ",lineIndex);
		//#450
		//�܂肽���ݐ��\��
		if( textview.SPI_GetFoldingCollapsed(lineIndex) == true )
		{
			ALocalPoint	foldingLineSpt, foldingLineEpt;
			foldingLineSpt.x = lineLocalRect.left;
			foldingLineSpt.y = lineLocalRect.bottom - 3;
			foldingLineEpt.x = lineLocalRect.right;
			foldingLineEpt.y = lineLocalRect.bottom - 3;
			//NVMC_DrawLine(foldingLineSpt,foldingLineEpt,kColor_Orange,0.8,3.0);
			foldingLineSpt.x += 3;
			foldingLineSpt.y++;
			foldingLineEpt.y++;
			NVMC_DrawLine(foldingLineSpt,foldingLineEpt,kColor_Orange,0.8,3.0);
			foldingLineSpt.x -= 3;
			foldingLineSpt.y++;
			foldingLineEpt.y++;
			NVMC_DrawLine(foldingLineSpt,foldingLineEpt,kColor_Orange,0.8,3.0);
		}
		
		//#842
		if( GetTextDocumentConst().SPI_GetLineCheckedDate(lineIndex) != 0 )
		{
			ALocalPoint	pt = {lineLocalRect.left+16,lineLocalRect.top};
			NVMC_DrawImage(kImageID_iconSwlFlag,pt);
		}
		
		//�s�ԍ��`��
		AText	lineNumberText;
		if( drawParagraph == false )
		{
			lineNumberText.SetFormattedCstring("%d",lineIndex+1);
		}
		else
		{
			ABool	firstLineInParagraph = true;
			if( lineIndex > 0 )
			{
				firstLineInParagraph = document.SPI_GetLineExistLineEnd(lineIndex-1);
			}
			if( firstLineInParagraph == true )
			{
				AIndex	paraIndex = document.SPI_GetParagraphIndexByLineIndex(lineIndex);
				lineNumberText.SetFormattedCstring("%d",paraIndex+1);
			}
		}
		ABool	defaultStyle = true;//B0000 ������
		ATextStyle	style = kTextStyle_Normal;
		if( lineIndex == document.SPI_GetBookmarkLineIndex() )
		{
			style = kTextStyle_Underline;
			defaultStyle = false;//B0000 ������
		}
		AColor	c = lettercolor;
		if( GetApp().SPI_GetSDFDebugMode() == true )
		{
			if( document.SPI_GetFirstGlobalIdentifierUniqID(lineIndex) != kUniqID_Invalid )
			{
				c = kColor_Red;
				defaultStyle = false;//B0000 ������
			}
		}
		//#450
		if( mMouseHoverLineIndex == lineIndex )
		{
			AColorWrapper::GetHighlightColor(c);
			defaultStyle = false;
		}
		//�܂肽���ݔ͈̓z�o�[�\��
		if( lineIndex >= mFoldingHoverStart && lineIndex <= mFoldingHoverEnd )
		{
			ALocalRect	lineNumberTextRect = lineLocalRect;
			lineNumberTextRect.left = 0;
			lineNumberTextRect.right = lineLocalRect.right - kRightMargin;
			//�z�o�[�\��
			AColor	hovercolor = AColorWrapper::GetColorByHTMLFormatColor("007eff");
			NVMC_PaintRect(lineNumberTextRect,hovercolor,0.12);
			//
			//AColorWrapper::GetHighlightColor(c);
			//c = AColorWrapper::GetColorByHTMLFormatColor("003398");
			defaultStyle = false;
		}
		//R0006
		/*
		ADiffType	difftype = document.SPI_GetDiffType(lineIndex);
		switch(difftype)
		{
		  case kDiffType_Added:
			{
				c = kColor_Red;
				break;
			}
		}*/
		ALocalRect	lineNumberTextRect = lineLocalRect;
		lineNumberTextRect.left = 0;
		lineNumberTextRect.right = lineLocalRect.right - (kSeparatorOffset+kRightMargin);
		//#992
		//���@�`�F�b�J�[���[�j���O�ӏ��ɐF�Â�
		if( GetTextDocumentConst().SPI_IsParagraphStartLine(lineIndex) == true )
		{
			//�i���ԍ��擾
			AIndex	paraIndex = document.SPI_GetParagraphIndexByLineIndex(lineIndex);
			//���@�`�F�b�J�[���[�j���O�f�[�^�擾
			ANumberArray	colIndexArray;
			ATextArray	titleArray,detailArray;
			AColorArray	colorArray;
			ABoolArray	displayInTextViewArray;
			GetTextDocumentConst().SPI_GetCheckerPluginWarnings(paraIndex,colIndexArray,titleArray,detailArray,colorArray,displayInTextViewArray);
			//���@�`�F�b�J�[���[�j���O�����邩�̔���
			if( colIndexArray.GetItemCount() > 0 )
			{
				//�ŏ��̃��[�j���O�̐F���擾
				AColor	color = colorArray.Get(0);
				//�F�Â��͈�rect�擾
				ALocalRect	rect = lineNumberTextRect;
				rect.right += 2;
				rect.left = rect.right - NVMC_GetDrawTextWidth(lineNumberText) - 6;
				/*
				//�����̃��[�j���O������Ƃ��̕\��
				if( colorArray.GetItemCount() >= 2 )
				{
					ALocalRect	r = rect;
					r.left++;
					r.right++;
					r.top++;
					r.bottom++;
					NVMC_FrameRoundedRect(r,kColor_Gray50Percent,1.0,3,true,true,true,true);
				}
				*/
				NVMC_PaintRoundedRect(rect,color,color,kGradientType_None,0.8,0.8,3,true,true,true,true);//#1275 kGradientType_Horizontal��kGradientType_None
				NVMC_FrameRoundedRect(rect,kColor_Gray50Percent,1.0,3,true,true,true,true);
			}
		}
		//
		if( defaultStyle == true )//B0000 ������
		{
			NVMC_DrawText(lineNumberTextRect,lineNumberText,kJustification_RightCenter);//#450
		}
		else
		{
			NVMC_DrawText(lineNumberTextRect,lineNumberText,c,style,kJustification_RightCenter);//#450
		}
		//���@���[�j���O�\��
		if( GetTextDocumentConst().SPI_GetSyntaxError(lineIndex) == true )
		{
			ALocalRect	rect = lineLocalRect;
			rect.left = 2;
			rect.right = lineLocalRect.right-2;
			NVMC_PaintRoundedRect(rect,kColor_Red,kColor_Red,kGradientType_None,0.7,0.7,2,
								  true,true,true,true);//#1275 kGradientType_Horizontal��kGradientType_None
		}
		if( GetTextDocumentConst().SPI_GetSyntaxWarning(lineIndex) == true )
		{
			ALocalRect	rect = lineLocalRect;
			rect.left = 2;
			rect.right = lineLocalRect.right-2;
			NVMC_PaintRoundedRect(rect,kColor_Yellow,kColor_Yellow,kGradientType_None,0.7,0.7,2,
								  true,true,true,true);//#1275 kGradientType_Horizontal��kGradientType_None
		}
	}
	//#450 �s�܂肽���݃}�[�N�\��
	if( GetModePrefDB().GetData_Bool(AModePrefDB::kUseLineFolding) == true )
	{
		for( AIndex lineIndex = startLineIndex; lineIndex < lineCount; lineIndex++ )
		{
			//�s�̕`��̈�擾
			AImageRect	lineImageRect;
			GetLineImageRect(lineIndex,lineImageRect);
			//#450 ��\���s����
			if( lineImageRect.top == lineImageRect.bottom )   continue;
			//#450 �I������i���\�����悤�Ƃ��Ă���s��imagerect��frame�O��������I��
			if( lineImageRect.top > imageFrameRect.bottom )   break;
			//------------------�܂肽���݊J�n�ʒu�}�[�N------------------
			if( textview.SPI_IsFoldingStart(document.SPI_GetFoldingLevel(lineIndex)) == true ||
						textview.SPI_GetFoldingCollapsed(lineIndex) == true )
			{
				//�܂肽���݃}�[�Nrect�擾
				ALocalRect	foldingMarkRect;
				GetFoldingMarkLocalRect(lineIndex,foldingMarkRect);
				//�\���ʒu�擾
				ALocalPoint	pt = {0};
				pt.x = foldingMarkRect.left-2;
				pt.y = foldingMarkRect.top-2;
				if( textview.SPI_GetFoldingCollapsed(lineIndex) == true )
				{
					//------------------�܂肽���ݎ�------------------
					if( mFoldingHoverStart == lineIndex )
					{
						//�z�o�[
						NVMC_DrawImage(kImageID_btnFldngArrowRight_h,pt);
					}
					else
					{
						//�ʏ�
						NVMC_DrawImage(kImageID_btnFldngArrowRight,pt);
					}
				}
				else
				{
					//------------------�W�J��------------------
					if( mFoldingHoverStart == lineIndex )
					{
						//�z�o�[
						NVMC_DrawImage(kImageID_btnFldngArrowDown_h,pt);
					}
					else
					{
						//�ʏ�
						NVMC_DrawImage(kImageID_btnFldngArrowDown,pt);
					}
				}
			}
			//------------------�܂肽���ݏI���ʒu�}�[�N------------------
			if( textview.SPI_IsFoldingEnd(document.SPI_GetFoldingLevel(lineIndex)) == true )
			{
				//�܂肽���݃}�[�Nrect�擾
				ALocalRect	foldingMarkRect;
				GetFoldingMarkLocalRect(lineIndex,foldingMarkRect);
				//�\���ʒu�擾
				ALocalPoint	pt = {0};
				pt.x = foldingMarkRect.left-2;
				pt.y = foldingMarkRect.top-2;
				if( mFoldingHoverEnd == lineIndex )
				{
					//�z�o�[
					NVMC_DrawImage(kImageID_btnFldngArrowEnd_h,pt);
				}
				else
				{
					//�ʏ�
					NVMC_DrawImage(kImageID_btnFldngArrowEnd,pt);
				}
				/*
				//
				//NVMC_PaintRect(foldingMarkRect,kColor_White,1.0);//0.6);
				AColor	foldingMarkColor = kColor_Gray50Percent;
				if( mFoldingHoverEnd == lineIndex )
				{
					AColorWrapper::GetHighlightColor(foldingMarkColor);
				}
				//
				ANumber	foldingMarkSize = foldingMarkRect.bottom-foldingMarkRect.top;
				ALocalPoint	spt, ept;
				spt.x = foldingMarkRect.left + foldingMarkSize/2 + 1;
				spt.y = foldingMarkRect.top;
				ept.x = foldingMarkRect.left + foldingMarkSize/2 + 1;
				ept.y = foldingMarkRect.top + foldingMarkSize/2;
				NVMC_DrawLine(spt,ept,foldingMarkColor);//,1.0,0.0,1.0,true);
				spt.x = foldingMarkRect.left + foldingMarkSize/2 + 1;
				spt.y = foldingMarkRect.top + foldingMarkSize/2;
				ept.x = foldingMarkRect.right + 1;
				ept.y = foldingMarkRect.top + foldingMarkSize/2;
				NVMC_DrawLine(spt,ept,foldingMarkColor);//,1.0,0.0,1.0,true);
				*/
			}
		}
	}
	
	//Diff index�ԍ��`��
	AWindowID	textWindowID = textview.SPI_GetTextWindowID();
	if( textWindowID != kObjectID_Invalid )
	{
		//Diff���[�h�����ǂ����̔���
		if( mDiffDrawEnable == true && document.SPI_GetDiffTargetDocumentID() != kObjectID_Invalid && 
					GetApp().SPI_GetTextWindowByID(textWindowID).SPI_GetDiffDisplayMode() == true )
		{
			//Diff index�ԍ��͂Q�s�ɂ킽���ĕ\�������̂ŁA�P�O�̍s����`��J�n
			AIndex	sl = startLineIndex;
			if( sl > 0 ) sl--;
			//��ʓ��̍s���ƂɃ��[�v
			for( AIndex lineIndex = sl; lineIndex <= lineCount; lineIndex++ )
			{
				//�s�̕`��̈�擾
				AImageRect	lineImageRect;
				GetLineImageRect(lineIndex,lineImageRect);
				//#450 ��\���s����
				if( lineImageRect.top == lineImageRect.bottom )   continue;
				//#450 �I������i���\�����悤�Ƃ��Ă���s��imagerect��frame�O��������I��
				if( lineImageRect.top > imageFrameRect.bottom )   break;
				//
				ALocalRect	lineLocalRect;
				NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
				
				//lineIndex��DiffPart�̊J�n�s���ǂ����𔻒�
				AIndex	diffIndex = document.SPI_GetDiffIndexByStartLineIndex(lineIndex);
				if( diffIndex != kIndex_Invalid )
				{
					//�`����擾
					AFloatNumber	fontsize = GetTextDocumentConst().SPI_GetFontSize();//#868
					if( fontsize > 9.0 )   fontsize = 9.0;
					AText	fontname;
					AFontWrapper::GetAppDefaultFontName(fontname);
					NVMC_SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Italic|kTextStyle_Bold,document.SPI_IsAntiAlias());
					ANumber	fontheight = 0, fontascent = 0;
					NVMC_GetMetricsForDefaultTextProperty(fontheight,fontascent);
					//Diff index������ݒ�
					AText	diffIndexText;
					diffIndexText.SetNumber(diffIndex+1);
					//
					ANumber	textwidth = NVMC_GetDrawTextWidth(diffIndexText) +3;
					//
					ALocalRect	diffIndexRect;
					diffIndexRect.left		= lineLocalRect.right - textwidth - 12 -2;
					diffIndexRect.top		= lineLocalRect.top;
					diffIndexRect.right		= lineLocalRect.right -2;
					diffIndexRect.bottom	= lineLocalRect.top + fontheight + 4;
					NVMC_PaintRect(diffIndexRect,kColor_Gray95Percent,1.0);
					NVMC_FrameRect(diffIndexRect,kColor_Black);
					diffIndexRect.left += 8;
					diffIndexRect.top += 2;
					diffIndexRect.right -= 4;
					diffIndexRect.bottom -= 2;
					NVMC_DrawText(diffIndexRect,diffIndexText,kJustification_Left);
				}
			}
		}
	}
}

//#450
/**
*/
void	AView_LineNumber::GetFoldingMarkLocalRect( const AIndex inLineIndex, ALocalRect& outRect ) const
{
	//�Ώۍs�S�̂�localrect�擾
	AImageRect	lineImageRect;
	GetLineImageRect(inLineIndex,lineImageRect);
	ALocalRect	lineLocalRect;
	NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
	
	//
	AFoldingLevel	foldingLevel =  GetTextDocumentConst().SPI_GetFoldingLevel(inLineIndex);
	//
	ANumber	foldingMarkSize = kFoldingMarkSize;
	if( (foldingLevel&kFoldingLevel_Header) == 0 )
	{
		foldingMarkSize = kSmallFoldingMarkSize;
	}
	if( foldingMarkSize > (lineLocalRect.bottom - lineLocalRect.top) )
	{
		foldingMarkSize = lineLocalRect.bottom - lineLocalRect.top;
		foldingMarkSize /= 2;
		foldingMarkSize *= 2;
		foldingMarkSize++;
	}
	//
	outRect.left		= lineLocalRect.left + kFoldingMarkLeftMargin + mLeftMargin;//#1186 //lineLocalRect.right - foldingMarkSize - 4;
	outRect.top			= (lineLocalRect.top+lineLocalRect.bottom)/2 - foldingMarkSize/2;
	outRect.right		= outRect.left + foldingMarkSize;
	outRect.bottom		= outRect.top + foldingMarkSize;
	if( (foldingLevel&kFoldingLevel_Header) == 0 )
	{
		outRect.left += 2;
		outRect.right += 2;
	}
}

//#450
/**
�s��imagerect���擾
*/
void	AView_LineNumber::GetLineImageRect( const AIndex inLineIndex, AImageRect& outLineImageRect ) const
{
	GetLineImageRect(GetTextViewConst(),inLineIndex,outLineImageRect);
}
void	AView_LineNumber::GetLineImageRect( const AView_Text& inTextView, const AIndex inLineIndex, AImageRect& outLineImageRect ) const
{
	//TextView�̍simagerect���擾
	inTextView.GetLineImageRect(inLineIndex,outLineImageRect);
	//�s�ԍ�view��imagerect�֕ϊ�
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	outLineImageRect.left = imageFrameRect.left;
	outLineImageRect.right = imageFrameRect.right;
}

//
void	AView_LineNumber::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
			const ALocalPoint inLocalPoint )//win 080706
{
	//TextView�̃R���g���[��ID�Ŕ����������Ƃɂ���
	//�����𒼐ړn���ƁA0�ɂȂ��Ă��܂��igcc�o�O�H�j�̂ŁA���[�J���ϐ��Ɉڂ��Ă���n���B
	ANumber	deltaX = inDeltaX;
	ANumber	deltaY = inDeltaY;
	//#199 NVM_GetWindow().EVT_DoMouseWheel(mControlID_TextView,deltaX,deltaY,inModifierKeys,inLocalPoint);
	GetTextView().EVT_DoMouseWheel(deltaX,deltaY,inModifierKeys,inLocalPoint);
}

//Drag�C�x���g�iTextView�̃R���g���[��ID�Ŕ����������Ƃɂ���j
void	AView_LineNumber::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )
{
	ALocalPoint	pt = inLocalPoint;
	pt.x = 0;
	//#199 NVM_GetWindow().EVT_DoDragTracking(mControlID_TextView,inDragRef,pt,inModifierKeys);
	GetTextView().EVT_DoDragTracking(inDragRef,pt,inModifierKeys,outIsCopyOperation);
}
void	AView_LineNumber::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalPoint	pt = inLocalPoint;
	pt.x = 0;
	//#199 NVM_GetWindow().EVT_DoDragEnter(mControlID_TextView,inDragRef,pt,inModifierKeys);
	GetTextView().EVT_DoDragEnter(inDragRef,pt,inModifierKeys);
}
void	AView_LineNumber::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalPoint	pt = inLocalPoint;
	pt.x = 0;
	//#199 NVM_GetWindow().EVT_DoDragLeave(mControlID_TextView,inDragRef,pt,inModifierKeys);
	GetTextView().EVT_DoDragLeave(inDragRef,pt,inModifierKeys);
}
void	AView_LineNumber::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalPoint	pt = inLocalPoint;
	pt.x = 0;
	//#199 NVM_GetWindow().EVT_DoDragReceive(mControlID_TextView,inDragRef,pt,inModifierKeys);
	GetTextView().EVT_DoDragReceive(inDragRef,pt,inModifierKeys);
}

//#450
/**
�}�E�X�ړ�����OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�J�[�\���̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
ABool	AView_LineNumber::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//==================�z�o�[�sindex���擾==================
	ANumber	foldingMarkMouseHoverLineIndex = kIndex_Invalid;
	//
	AImagePoint	clickImagePoint = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint = {0};
	GetTextView().GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	
	//==================�z�o�[�s�̃t�H�[���f�B���O�f�[�^���擾==================
	AFoldingLevel	foldingLevel = GetTextDocument().SPI_GetFoldingLevel(clickTextPoint.y);
	if( foldingLevel != kFoldingLevel_None )
	{
		ALocalRect	foldingMarkRect = {0};
		GetFoldingMarkLocalRect(clickTextPoint.y,foldingMarkRect);
		if( inLocalPoint.x >= foldingMarkRect.left && inLocalPoint.x <= foldingMarkRect.right &&
					inLocalPoint.y >= foldingMarkRect.top && inLocalPoint.y <= foldingMarkRect.bottom )
		{
			foldingMarkMouseHoverLineIndex = clickTextPoint.y;
		}
	}
	//==================�ω�������΁A�t�H�[���f�B���O�f�[�^�X�V�A�`��X�V==================
	if( mFoldingMarkMouseHoverLineIndex != foldingMarkMouseHoverLineIndex || mMouseHoverLineIndex != clickTextPoint.y )
	{
		//folding�z�o�[�͈͎擾
		mFoldingMarkMouseHoverLineIndex = foldingMarkMouseHoverLineIndex;
		mMouseHoverLineIndex = clickTextPoint.y;
		mFoldingHoverStart = kIndex_Invalid;
		mFoldingHoverEnd = kIndex_Invalid;
		//
		if( mFoldingMarkMouseHoverLineIndex != kIndex_Invalid )
		{
			if( GetTextView().SPI_IsFoldingStart(foldingLevel) == true )
			{
				mFoldingHoverStart = mFoldingMarkMouseHoverLineIndex;
				mFoldingHoverEnd = GetTextView().SPI_FindFoldingEndLineIndex(mFoldingMarkMouseHoverLineIndex)-1;
				if( (foldingLevel&kFoldingLevel_Header) == 0 )
				{
					mFoldingHoverEnd++;
				}
			}
			else if( GetTextView().SPI_IsFoldingEnd(foldingLevel) == true )
			{
				mFoldingHoverStart = GetTextView().SPI_FindFoldingStartLineIndex(mFoldingMarkMouseHoverLineIndex);
				mFoldingHoverEnd = mFoldingMarkMouseHoverLineIndex;
			}
		}
		//
		NVI_Refresh();
	}
	return true;
}

//#450
/**
�}�E�X��View�O�ֈړ���������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�J�[�\���̏ꏊ�i�R���g���[�����[�J�����W�j
*/
void	AView_LineNumber::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	mFoldingMarkMouseHoverLineIndex = kIndex_Invalid;
	mMouseHoverLineIndex = kIndex_Invalid;
	mFoldingHoverStart = kIndex_Invalid;
	mFoldingHoverEnd = kIndex_Invalid;
	NVI_Refresh();
}

//#992
/**
�w���v�^�O
*/
ABool	AView_LineNumber::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	//�}�E�X�ʒu�̍s�ԍ����擾
	AImagePoint	clickImagePoint = {0};
	NVM_GetImagePointFromLocalPoint(inPoint,clickImagePoint);
	ATextPoint	clickTextPoint = {0};
	GetTextView().GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	//�i���ԍ��擾
	AIndex	paraIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(clickTextPoint.y);
	//�s�̕`��̈�擾
	AImageRect	lineImageRect = {0};
	GetLineImageRect(clickTextPoint.y,lineImageRect);
	//�w���v�^�O�\���̈�擾
	NVM_GetLocalRectFromImageRect(lineImageRect,outRect);
	//���[�j���O�f�[�^�擾
	ANumberArray	colIndexArray;
	ATextArray	titleArray,detailArray;
	AColorArray	colorArray;
	ABoolArray	displayInTextViewArray;
	GetTextDocumentConst().SPI_GetCheckerPluginWarnings(paraIndex,colIndexArray,titleArray,detailArray,colorArray,displayInTextViewArray);
	//���[�j���O�f�[�^���w���v�^�O�ɐݒ�
	if( colIndexArray.GetItemCount() > 0 )
	{
		//�w���v�^�O�e�L�X�g�擾
		AText	text;
		for( AIndex i = 0; i < detailArray.GetItemCount(); i++ )
		{
			text.AddText(detailArray.GetTextConst(i));
			text.AddCstring("\n");
		}
		outText1.SetText(text);
		outText2.SetText(text);
		outTagSide = kHelpTagSide_Default;
		return true;
	}
	else
	{
		return false;
	}
}
#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�e��X�V����

//
void	AView_LineNumber::SPI_UpdateText( const AIndex inStartLineIndex, const AIndex inEndLineIndex, 
		const AItemCount inInsertedLineCount, const ABool inParagraphCountChanged ) 
{
	//
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	//�h�L�������g�̍Ō�ŉ��s�����ꍇ
	AIndex	endLineIndex = inEndLineIndex;
	if( inInsertedLineCount > 0 && endLineIndex < GetTextDocument().SPI_GetLineCount() )
	{
		endLineIndex++;
	}
	//
	AImageRect	imageRect;
	GetTextView().GetLineImageRect(inStartLineIndex,endLineIndex,imageRect);
	ALocalRect	localRect;
	NVM_GetLocalRectFromImageRect(imageRect,localRect);
	//�X�V�ŏI�s���A�e�L�X�g�S�̂̍ŏI�s�A���A�s�폜�ɂ����̂ł������ꍇ�́A�r���[�̍ŉ����܂ōX�V
	if( inEndLineIndex >= GetTextDocument().SPI_GetLineCount() && inInsertedLineCount < 0 )
	{
		localRect.bottom = localFrameRect.bottom;
	}
	//�i���ԍ��\���̏ꍇ�͏�ɍŉ����܂ōX�V
	if( inParagraphCountChanged == true )
	{
		const ADocument_Text&	document = GetApp().SPI_GetTextDocumentByID(mTextDocumentID);
		const AModeIndex	modeIndex = document.SPI_GetModeIndex();
		const AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(modeIndex);
		if( modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayEachLineNumber_AsParagraph) == true )
		{
			localRect.bottom = localFrameRect.bottom;
		}
	}
	//�X�V
	NVMC_RefreshRect(localRect);
}

//#496
/**
�s�`��X�V
*/
void	AView_LineNumber::SPI_RefreshLine( const AIndex inLineIndex )
{
	AImageRect	imageLineRect;
	GetTextView().GetLineImageRect(inLineIndex,imageLineRect);
	ALocalRect	localLineRect;
	NVM_GetLocalRectFromImageRect(imageLineRect,localLineRect);
	NVMC_RefreshRect(localLineRect);
}

//#695
/**
�s�`��X�V
*/
void	AView_LineNumber::SPI_RefreshLines( const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	AImageRect	imageLineRect = {0,0,0,0};
	GetTextView().GetLineImageRect(inStartLineIndex,inEndLineIndex,imageLineRect);
	ALocalRect	localLineRect = {0,0,0,0};
	NVM_GetLocalRectFromImageRect(imageLineRect,localLineRect);
	NVMC_RefreshRect(localLineRect);
}

//�t�H���g����Text�`��v���p�e�B���ύX���ꂽ�ꍇ�̏���
void	AView_LineNumber::SPI_UpdateTextDrawProperty()
{
	AFloatNumber	fontsize = GetTextDocumentConst().SPI_GetFontSize()*kLineNumberFontMultiply;//#868 GetDocPrefDB().GetData_FloatNumber(ADocPrefDB::kTextFontSize);
	if( fontsize > kMaxFontSizeForLineNumber )   fontsize = kMaxFontSizeForLineNumber;
	AColor	color;
	GetModePrefDB().GetModeData_Color(AModePrefDB::kLetterColor,color);
	AText	fontname;//win
	GetTextDocumentConst().SPI_GetFontName(fontname);//#868
	NVMC_SetDefaultTextProperty(
				fontname,//win
				fontsize,
				color,
				kTextStyle_Normal,
				GetTextDocument().SPI_IsAntiAlias());
	/*
	const ADocument_Text&	document = GetApp().SPI_GetTextDocumentByID(mTextDocumentID);
	const AModeIndex	modeIndex = document.SPI_GetModeIndex();
	const AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(modeIndex);
	//
	AText	digits("000");
	mLineNumberTextWidth = kFoldingMarkLeftMargin + kFoldingMarkSize;
	if( modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayEachLineNumber) == true )
	{
		mLineNumberTextWidth += NVMC_GetDrawTextWidth(digits);
	}
	//
	ALocalRect	frame = {0};
	NVM_GetLocalViewRect(frame);
	ANumber	controlwidth = mLineNumberTextWidth + kSeparatorOffset + kRightMargin;
	NVM_GetWindow().NVI_SetControlSize(NVI_GetControlID(),controlwidth,frame.bottom-frame.top);
	*/
	//LineHeight���ύX���ꂽ�̂�ImageSize���ύX�K�v
	SPI_UpdateImageSize();
}

//#450
/**
�P���̕\�����擾
*/
ANumber	AView_LineNumber::SPI_Get1DigitWidth() const
{
	AText	digit("0");
	return NVMC_GetDrawTextWidth(digit);
}

//
void	AView_LineNumber::SPI_UpdateImageSize()
{
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	ANumber	width = rect.right-rect.left;
	ANumber	height = GetTextView().SPI_GetImageHeight();//#450 GetTextDocument().SPI_GetLineCount() * GetTextView().GetLineHeightWithMargin();
	NVM_SetImageSize(width,height);
}

//#379
/**
Diff�Ή��|���S���擾
*/
void	AView_LineNumber::SPI_GetDiffDrawData( AArray<ADiffType>& outDiffType, AArray<AIndex>& outDiffIndex,
		AArray<ALocalPoint>& outStartLeftPoint, AArray<ALocalPoint>& outStartRightPoint, 
		AArray<ALocalPoint>& outEndLeftPoint, AArray<ALocalPoint>& outEndRightPoint )
{
	//���ʏ�����
	outDiffType.DeleteAll();
	outDiffIndex.DeleteAll();
	outStartLeftPoint.DeleteAll();
	outStartRightPoint.DeleteAll();
	outEndLeftPoint.DeleteAll();
	outEndRightPoint.DeleteAll();
	//
	const ADocument_Text&	document = GetApp().SPI_GetTextDocumentByID(mTextDocumentID);
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	//��ʓ��J�n�s�擾
	AIndex	startLineIndex = GetTextView().GetLineIndexByImageY(imageFrameRect.top);
	AIndex	lineCount = document.SPI_GetLineCount();
	//
	AWindowID	textWindowID = GetTextView().SPI_GetTextWindowID();
	if( textWindowID != kObjectID_Invalid )
	{
		/*#drop
		if( GetApp().SPI_GetTextWindowByID(textWindowID).SPI_IsMainSubDiffMode(GetTextView().NVI_GetControlID()) == true )
		{
			AWindow_Text&	win = GetApp().SPI_GetTextWindowByID(textWindowID);
			AControlID	mainControlID = GetTextView().NVI_GetControlID();
			//���ݕ\�����͈͓̔���Diff���擾�istartDiffIndex, endDiffIndex�ցj����
			ADiffType	diffType = kDiffType_None;
			AIndex	startDiffIndex = win.SPI_MainSubDiff_GetDiffIndexByIncludingLineIndex(mainControlID,startLineIndex,diffType);
			AIndex	endDiffIndex = startDiffIndex;
			for( AIndex lineIndex = startLineIndex; lineIndex <= lineCount; lineIndex++ )
			{
				//�s�̕`��̈�擾
				AImageRect	lineImageRect;
				GetLineImageRect(lineIndex,lineImageRect);
				//#450 ��\���s����
				if( lineImageRect.top == lineImageRect.bottom )   continue;
				//#450 �I������i���\�����悤�Ƃ��Ă���s��imagerect��frame�O��������I��
				if( lineImageRect.top > imageFrameRect.bottom )   break;
				
				//lineIndex��DiffPart�̊J�n�s���ǂ����𔻒�
				AIndex	s = win.SPI_MainSubDiff_GetDiffIndexByStartLineIndex(lineIndex);
				if( s != kIndex_Invalid )
				{
					//lineIndex��DiffPart�J�n�s�ŁAstartDiffIndex���ݒ�Ȃ�ݒ�
					if( startDiffIndex == kIndex_Invalid )
					{
						startDiffIndex = s;
					}
					//lineIndex��DiffPart�I���s�Ȃ�AendDiffIndex�ɐݒ�
					endDiffIndex = s;
				}
				//lineIndex��DiffPart�̏I���s���ǂ����𔻒�
				AIndex	e = win.SPI_MainSubDiff_GetDiffIndexByEndLineIndex(lineIndex);
				if( e != kIndex_Invalid )
				{
					//lineIndex��DiffPart�I���s�ŁAstartDiffIndex���ݒ�Ȃ�ݒ�
					if( startDiffIndex == kIndex_Invalid )
					{
						startDiffIndex = e;
					}
					//lineIndex��DiffPart�I���s�Ȃ�AendDiffIndex�ɐݒ�
					endDiffIndex = e;
				}
			}
			//Diff�Ή��\���f�[�^�擾�istartDiffIndex�`endDiffIndex�j
			if( startDiffIndex != kIndex_Invalid && endDiffIndex != kIndex_Invalid )
			{
				for( AIndex diffIndex = startDiffIndex; diffIndex <= endDiffIndex; diffIndex++ )
				{
					//diffIndex�̃p�[�g�̊J�n�s�̏��擾
					AIndex	startLineIndex = win.SPI_MainSubDiff_GetDiffStartLineIndexByDiffIndex(diffIndex);
					AImageRect	startLineImageRect = {0,0,0,0};
					GetLineImageRect(startLineIndex,startLineImageRect);
					ALocalRect	startLineLocalRect = {0,0,0,0};
					NVM_GetLocalRectFromImageRect(startLineImageRect,startLineLocalRect);
					//�J�n�s��working�h�L�������g��LocalPoint�擾
					ALocalPoint	startRightPoint = {startLineLocalRect.right,startLineLocalRect.top-1};
					//�J�n�s��diff�h�L�������g��LocalPoint�擾
					ALocalPoint	startLeftPoint = {0,0};
					AIndex	diffStartParaIndex = win.SPI_MainSubDiff_GetDiffTargetParagraphIndexFromMainTextParagraphIndex(
							GetTextDocument().SPI_GetParagraphIndexByLineIndex(startLineIndex));
					if( diffStartParaIndex == kIndex_Invalid )   continue;
					AWindowPoint	diffstartwpt = {0,0};
					win.SPI_MainSubDiff_GetSubTextWindowPointByParagraph(
							diffStartParaIndex,diffstartwpt);
					GetApp().SPI_GetTextWindowByID(textWindowID).NVI_GetControlLocalPointFromWindowPoint(
							NVI_GetControlID(),diffstartwpt,startLeftPoint);
					startLeftPoint.x = 0;
					startLeftPoint.y--;
					
					//diffIndex�̃p�[�g�̏I���s�̏��擾
					AIndex	endLineIndex = win.SPI_MainSubDiff_GetDiffEndLineIndexByDiffIndex(diffIndex);
					AImageRect	endLineImageRect = {0,0,0,0};
					GetLineImageRect(endLineIndex,endLineImageRect);
					ALocalRect	endLineLocalRect = {0,0,0,0};
					NVM_GetLocalRectFromImageRect(endLineImageRect,endLineLocalRect);
					//�I���s��working�h�L�������g��LocalPoint�擾
					ALocalPoint	endRightPoint = {endLineLocalRect.right,endLineLocalRect.top-1};
					//�I���s��diff�h�L�������g��LocalPoint�擾
					ALocalPoint	endLeftPoint = {0,0};
					if( endLineIndex >= GetTextDocumentConst().SPI_GetLineCount() )   endLineIndex = GetTextDocumentConst().SPI_GetLineCount()-1;//#0 �폜�̏ꍇ��GetLineCount()�ȏ�ɂȂ邱�Ƃ�����
					AIndex	diffEndParaIndex = win.SPI_MainSubDiff_GetDiffTargetParagraphIndexFromMainTextParagraphIndex(
							GetTextDocument().SPI_GetParagraphIndexByLineIndex(endLineIndex));
					if( diffEndParaIndex == kIndex_Invalid )   continue;
					AWindowPoint	diffendwpt = {0,0};
					win.SPI_MainSubDiff_GetSubTextWindowPointByParagraph(
							diffEndParaIndex,diffendwpt);
					GetApp().SPI_GetTextWindowByID(textWindowID).NVI_GetControlLocalPointFromWindowPoint(
							NVI_GetControlID(),diffendwpt,endLeftPoint);
					endLeftPoint.x = 0;
					endLeftPoint.y--;
					
					//���ʊi�[
					outDiffIndex.Add(diffIndex);
					outDiffType.Add(win.SPI_MainSubDiff_GetDiffTypeByDiffIndex(diffIndex));
					outStartLeftPoint.Add(startLeftPoint);
					outStartRightPoint.Add(startRightPoint);
					outEndLeftPoint.Add(endLeftPoint);
					outEndRightPoint.Add(endRightPoint);
				}
			}
		}
		*/
		//Diff���[�h�����ǂ����̔���
		/*else */if( mDiffDrawEnable == true && GetTextDocument().SPI_GetDiffTargetDocumentID() != kObjectID_Invalid && 
					GetApp().SPI_GetTextWindowByID(textWindowID).SPI_GetDiffDisplayMode() == true )
		{
			//���ݕ\�����͈͓̔���Diff���擾�istartDiffIndex, endDiffIndex�ցj����
			AIndex	startDiffIndex = GetTextDocument().SPI_GetDiffIndexByIncludingLineIndex(startLineIndex);
			AIndex	endDiffIndex = startDiffIndex;
			for( AIndex lineIndex = startLineIndex; lineIndex <= lineCount; lineIndex++ )
			{
				//�s�̕`��̈�擾
				AImageRect	lineImageRect;
				GetLineImageRect(lineIndex,lineImageRect);
				//#450 ��\���s����
				if( lineImageRect.top == lineImageRect.bottom )   continue;
				//#450 �I������i���\�����悤�Ƃ��Ă���s��imagerect��frame�O��������I��
				if( lineImageRect.top > imageFrameRect.bottom )   break;
				
				//lineIndex��DiffPart�̊J�n�s���ǂ����𔻒�
				AIndex	s = GetTextDocument().SPI_GetDiffIndexByStartLineIndex(lineIndex);
				if( s != kIndex_Invalid )
				{
					//lineIndex��DiffPart�J�n�s�ŁAstartDiffIndex���ݒ�Ȃ�ݒ�
					if( startDiffIndex == kIndex_Invalid )
					{
						startDiffIndex = s;
					}
					//lineIndex��DiffPart�I���s�Ȃ�AendDiffIndex�ɐݒ�
					endDiffIndex = s;
				}
				//lineIndex��DiffPart�̏I���s���ǂ����𔻒�
				AIndex	e = GetTextDocument().SPI_GetDiffIndexByEndLineIndex(lineIndex);
				if( e != kIndex_Invalid )
				{
					//lineIndex��DiffPart�I���s�ŁAstartDiffIndex���ݒ�Ȃ�ݒ�
					if( startDiffIndex == kIndex_Invalid )
					{
						startDiffIndex = e;
					}
					//lineIndex��DiffPart�I���s�Ȃ�AendDiffIndex�ɐݒ�
					endDiffIndex = e;
				}
			}
			//Diff�Ή��\���f�[�^�擾�istartDiffIndex�`endDiffIndex�j
			if( startDiffIndex != kIndex_Invalid && endDiffIndex != kIndex_Invalid )
			{
				for( AIndex diffIndex = startDiffIndex; diffIndex <= endDiffIndex; diffIndex++ )
				{
					//diffIndex�̃p�[�g�̊J�n�s�̏��擾
					AIndex	startLineIndex = GetTextDocument().SPI_GetDiffStartLineIndexByDiffIndex(diffIndex);
					AImageRect	startLineImageRect = {0,0,0,0};
					GetLineImageRect(startLineIndex,startLineImageRect);
					ALocalRect	startLineLocalRect = {0,0,0,0};
					NVM_GetLocalRectFromImageRect(startLineImageRect,startLineLocalRect);
					//�J�n�s��working�h�L�������g��LocalPoint�擾
					ALocalPoint	startRightPoint = {startLineLocalRect.right,startLineLocalRect.top-1};
					//�J�n�s��diff�h�L�������g��LocalPoint�擾
					ALocalPoint	startLeftPoint = {0,0};
					AIndex	diffStartParaIndex = GetTextDocument().SPI_GetDiffTargetParagraphIndexFromThisDocumentParagraphIndex(
							GetTextDocument().SPI_GetParagraphIndexByLineIndex(startLineIndex));
					if( diffStartParaIndex == kIndex_Invalid )   continue;
					AWindowPoint	diffstartwpt = {0,0};
					GetApp().SPI_GetTextWindowByID(textWindowID).SPI_GetDiffViewWindowPointByParagraph(
							GetTextDocument().GetObjectID(),diffStartParaIndex,diffstartwpt);
					GetApp().SPI_GetTextWindowByID(textWindowID).NVI_GetControlLocalPointFromWindowPoint(
							NVI_GetControlID(),diffstartwpt,startLeftPoint);
					startLeftPoint.x = 0;
					startLeftPoint.y--;
					
					//diffIndex�̃p�[�g�̏I���s�̏��擾
					AIndex	endLineIndex = GetTextDocument().SPI_GetDiffEndLineIndexByDiffIndex(diffIndex);
					AImageRect	endLineImageRect = {0,0,0,0};
					GetLineImageRect(endLineIndex,endLineImageRect);
					ALocalRect	endLineLocalRect = {0,0,0,0};
					NVM_GetLocalRectFromImageRect(endLineImageRect,endLineLocalRect);
					//�I���s��working�h�L�������g��LocalPoint�擾
					ALocalPoint	endRightPoint = {endLineLocalRect.right,endLineLocalRect.top-1};
					//�I���s��diff�h�L�������g��LocalPoint�擾
					ALocalPoint	endLeftPoint = {0,0};
					if( endLineIndex >= GetTextDocumentConst().SPI_GetLineCount() )   endLineIndex = GetTextDocumentConst().SPI_GetLineCount()-1;//#0 �폜�̏ꍇ��GetLineCount()�ȏ�ɂȂ邱�Ƃ�����
					AIndex	diffEndParaIndex = GetTextDocument().SPI_GetDiffTargetParagraphIndexFromThisDocumentParagraphIndex(
							GetTextDocument().SPI_GetParagraphIndexByLineIndex(endLineIndex));
					if( diffEndParaIndex == kIndex_Invalid )   continue;
					/*
					if( GetTextDocumentConst().SPI_GetDiffTypeByDiffIndex(diffIndex) == kDiffType_Deleted && 
					   diffEndParaIndex > 0 )
					{
						diffEndParaIndex--;
					}
					*/
					//
					AWindowPoint	diffendwpt = {0,0};
					GetApp().SPI_GetTextWindowByID(textWindowID).SPI_GetDiffViewWindowPointByParagraph(
							GetTextDocument().GetObjectID(),diffEndParaIndex,diffendwpt);
					GetApp().SPI_GetTextWindowByID(textWindowID).NVI_GetControlLocalPointFromWindowPoint(
							NVI_GetControlID(),diffendwpt,endLeftPoint);
					endLeftPoint.x = 0;
					endLeftPoint.y--;
					
					//���ʊi�[
					outDiffIndex.Add(diffIndex);
					outDiffType.Add(GetTextDocumentConst().SPI_GetDiffTypeByDiffIndex(diffIndex));
					outStartLeftPoint.Add(startLeftPoint);
					outStartRightPoint.Add(startRightPoint);
					outEndLeftPoint.Add(endLeftPoint);
					outEndRightPoint.Add(endRightPoint);
				}
			}
		}
	}
}

/**
���o�����X�g�̃z�o�[�X�V������
*/
void	AView_LineNumber::SPI_DoListViewHoverUpdated( const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	mFoldingHoverStart = inStartLineIndex;
	mFoldingHoverEnd = inEndLineIndex;
	NVI_Refresh();
}

/**
�������L�����b�g�\��
*/
/*
void	AView_LineNumber::SPI_XorCrossCaretH( const ANumber inCaretLocalY )
{
	ALocalRect	localFrame = {0};
	NVM_GetLocalViewRect(localFrame);
	//������
	ALocalPoint	spt = {localFrame.left,inCaretLocalY};
	ALocalPoint	ept = {localFrame.right,inCaretLocalY};
	NVMC_DrawXorCaretLine(spt,ept,true,true,1);
}
*/

