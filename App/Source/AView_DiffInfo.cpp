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

AView_DiffInfo
#379

*/

#include "stdafx.h"

#include "AView_DiffInfo.h"
#include "AView_LineNumber.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_DiffInfo
#pragma mark ===========================
//���E�C���h�E�̃��C��View

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_DiffInfo::AView_DiffInfo( const AWindowID inWindowID, const AControlID inID ) 
	: AView(inWindowID,inID), mTextWindowID(kObjectID_Invalid)
{
	NVMC_SetOffscreenMode(true);
}

//�f�X�g���N�^
AView_DiffInfo::~AView_DiffInfo()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================



#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_DiffInfo::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_DiffInfo::NVIDO_DoDeleted()
{
}


/**
�`��v�����̃R�[���o�b�N(AView�p)
*/
void	AView_DiffInfo::EVTDO_DoDraw()
{
	//
	ALocalRect	frameRect;
	NVM_GetLocalViewRect(frameRect);
#if IMPLEMENTATION_FOR_WINDOWS
	//MacOSX�̏ꍇ�́A�I�[�o�[���C�E�C���h�E�͂��Ƃ��Ɠ���
	//Windows�̏ꍇ�́A�����F�œh��Ԃ��B�����x�͔w�i�E�C���h�E�Őݒ肷��B
	//�i�I�[�o�[���C�̏ꍇ�j�����F�őS�̏���
	NVMC_EraseRect(frameRect);//win
#endif
	
	//
	if( mTextWindowID == kObjectID_Invalid )   return;
	if( GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_GetCurrentTabIndex() == kIndex_Invalid )   return;
	if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetCurrentViewCount() <= 0 )   return;
	
	//
	ADocumentID	docID = GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_GetCurrentDocumentID();
	if( docID == kObjectID_Invalid )   return;
	
	//
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	//
	//ALocalRect	localFrameRect;
	//NVM_GetLocalViewRect(localFrameRect);
	//NVMC_PaintRect(localFrameRect,kColor_Red,1);
	//�F�擾
	AColor	diffcolor_added, diffcolor_changed, diffcolor_deleted;
	GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Changed,diffcolor_changed);
	GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Added,diffcolor_added);
	GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Deleted,diffcolor_deleted);
	//LineNumberView����Diff�\�����擾
	AArray<ADiffType>	diffTypeArray;
	AArray<AIndex>	diffIndexArray;
	AArray<ALocalPoint>	diffStartLeftPointArray;
	AArray<ALocalPoint>	diffStartRightPointArray;
	AArray<ALocalPoint>	diffEndLeftPointArray;
	AArray<ALocalPoint>	diffEndRightPointArray;
	GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetCurrentLineNumberView(0).
			SPI_GetDiffDrawData(diffTypeArray,diffIndexArray,diffStartLeftPointArray,diffStartRightPointArray,
								diffEndLeftPointArray,diffEndRightPointArray);
	//���݂̃t�H�[�J�X�̃��[�h�擾
	AModeIndex	modeIndex = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetCurrentFocusTextDocument().SPI_GetModeIndex();
	//diff�\�����l�擾
	AFloatNumber	diffAlpha = GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Number(AModePrefDB::kDiffColorOpacity);
	diffAlpha /= 300;//0.0�`0.33�ɂ���
	//�Z���ݒ肪�傫���قǍʓx�������āA���x���グ��
	AFloatNumber	diffSChange = 1.0 - diffAlpha*2.0;
	AFloatNumber	diffVChange = 1.0 + diffAlpha*3.0;
	diffcolor_added = AColorWrapper::ChangeHSV(diffcolor_added,0.0,diffSChange,diffVChange);
	diffcolor_deleted = AColorWrapper::ChangeHSV(diffcolor_deleted,0.0,diffSChange,diffVChange);
	diffcolor_changed = AColorWrapper::ChangeHSV(diffcolor_changed,0.0,diffSChange,diffVChange);
	//�t���[���i�s�S�́A���������j�̃��{��
	ANumber	diffPartFrameLineAlphaMultiply = 6;
	//Diff�\���`��
	for( AIndex i = 0; i < diffTypeArray.GetItemCount(); i++ )
	{
		//
		ALocalPoint	startLeftPoint = diffStartLeftPointArray.Get(i);
		ALocalPoint	startRightPoint = diffStartRightPointArray.Get(i);
		ALocalPoint	endLeftPoint = diffEndLeftPointArray.Get(i);
		ALocalPoint	endRightPoint = diffEndRightPointArray.Get(i);
		if( IsPointInRect(startLeftPoint,frameRect) == true || IsPointInRect(startRightPoint,frameRect) == true ||
					IsPointInRect(endLeftPoint,frameRect) == true || IsPointInRect(endRightPoint,frameRect) == true )
		{
			AArray<ALocalPoint>	path;
			switch(diffTypeArray.Get(i))
			{
			  case kDiffType_Added:
				{
					endLeftPoint.y += 3;
					path.Add(startLeftPoint);
					path.Add(startRightPoint);
					path.Add(endRightPoint);
					path.Add(endLeftPoint);
					NVMC_PaintPoly(path,diffcolor_added,diffAlpha);
					//�㉺�̐���`��
					if( IsPointInRect(startLeftPoint,frameRect) == true ||
					   IsPointInRect(startRightPoint,frameRect) == true )
					{
						NVMC_DrawLine(startLeftPoint,startRightPoint,diffcolor_added,diffAlpha*diffPartFrameLineAlphaMultiply);
					}
					if( IsPointInRect(endLeftPoint,frameRect) == true ||
					   IsPointInRect(endRightPoint,frameRect) == true )
					{
						NVMC_DrawLine(endLeftPoint,endRightPoint,diffcolor_added,diffAlpha*diffPartFrameLineAlphaMultiply);
					}
					break;
				}
			  case kDiffType_Deleted:
				{
					endRightPoint.x = startRightPoint.x;
					endRightPoint.y = startRightPoint.y +3;
					path.Add(startLeftPoint);
					path.Add(startRightPoint);
					path.Add(endRightPoint);
					path.Add(endLeftPoint);
					NVMC_PaintPoly(path,diffcolor_deleted,diffAlpha);
					//�㉺�̐���`��
					if( IsPointInRect(startLeftPoint,frameRect) == true ||
					   IsPointInRect(startRightPoint,frameRect) == true )
					{
						NVMC_DrawLine(startLeftPoint,startRightPoint,diffcolor_deleted,diffAlpha*diffPartFrameLineAlphaMultiply);
					}
					if( IsPointInRect(endLeftPoint,frameRect) == true ||
					   IsPointInRect(endRightPoint,frameRect) == true )
					{
						NVMC_DrawLine(endLeftPoint,endRightPoint,diffcolor_deleted,diffAlpha*diffPartFrameLineAlphaMultiply);
					}
					break;
				}
			  case kDiffType_Changed:
				{
					path.Add(startLeftPoint);
					path.Add(startRightPoint);
					path.Add(endRightPoint);
					path.Add(endLeftPoint);
					NVMC_PaintPoly(path,diffcolor_changed,diffAlpha);
					//�㉺�̐���`��
					if( IsPointInRect(startLeftPoint,frameRect) == true ||
					   IsPointInRect(startRightPoint,frameRect) == true )
					{
						NVMC_DrawLine(startLeftPoint,startRightPoint,diffcolor_changed,diffAlpha*diffPartFrameLineAlphaMultiply);
					}
					if( IsPointInRect(endLeftPoint,frameRect) == true ||
					   IsPointInRect(endRightPoint,frameRect) == true )
					{
						NVMC_DrawLine(endLeftPoint,endRightPoint,diffcolor_changed,diffAlpha*diffPartFrameLineAlphaMultiply);
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
}

/**
�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
*/
ABool	AView_DiffInfo::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//LineNumberView�փ��_�C���N�g
	return GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetCurrentLineNumberView(0).
			EVTDO_DoMouseDown(inLocalPoint,inModifierKeys,inClickCount);
}


