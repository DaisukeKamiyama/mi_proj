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

CColorPickerPane

*/

#include "stdafx.h"

#include "CColorPickerPane.h"
#include "../Frame.h"
#include <Commdlg.h>

#pragma mark ===========================
#pragma mark [�N���X]CColorPickerPane
#pragma mark ===========================
//�F�I���R���g���[��

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
CColorPickerPane::CColorPickerPane( AObjectArrayItem* inParent, CWindowImp& inWindow ) 
	: CUserPane(inParent,inWindow), mColor(kColor_Black)
	,mFocused(false)//#353
{
}

//�f�X�g���N�^
CColorPickerPane::~CColorPickerPane()
{
}

#pragma mark ===========================

#pragma mark ---�C�x���g�n���h��

//�`��
ABool	CColorPickerPane::DoDraw()
{
	ALocalRect	frame;
	GetControlLocalFrameRect(frame);
	ALocalRect	rect;
	if( IsControlEnabled() && IsControlActive() )
	{
		//�h���b�v�V���h�E
		rect.left		= frame.left+1;
		rect.top		= frame.top+1;
		rect.right		= frame.right;
		rect.bottom		= frame.bottom;
		PaintRect(rect,kColor_Gray);
		//�g
		rect.left		= frame.left;
		rect.top		= frame.top;
		rect.right		= frame.right-1;
		rect.bottom		= frame.bottom-1;
		PaintRect(rect,kColor_Black);
	}
	//���g
	rect.left		= frame.left+1;
	rect.top		= frame.top+1;
	rect.right		= frame.right-2;
	rect.bottom		= frame.bottom-2;
	PaintRect(rect,mColor);
	if( (IsControlEnabled() && IsControlActive()) == false )
	{
		PaintRect(rect,kColor_White,0.5);
	}
	//#353 �t�H�[�J�X
	if( mFocused == true )
	{
		rect = frame;
		rect.left	-= 2;
		rect.top	-= 2;
		rect.right	+= 2;
		rect.bottom	+= 2;
		DrawFocusFrame(frame);
	}
	return true;
}

//�}�E�X�N���b�N�ɑ΂��鏈�� 
ABool	CColorPickerPane::DoMouseDown( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers, 
									  const ANumber inClickCount, const ABool inRightButton )
{
	if( IsControlEnabled() == false )   return false;
	
	COLORREF	zantei[16];
	CHOOSECOLOR	cc;
	cc.lStructSize 		= sizeof(cc);
	cc.hwndOwner 		= GetWin().GetHWnd();
	cc.hInstance		= NULL;
	cc.rgbResult		= mColor;
	cc.lpCustColors		= zantei;
	cc.Flags			= CC_RGBINIT;
	cc.lCustData		= 0;
	cc.lpfnHook			= NULL;
	cc.lpTemplateName	= L"";
	if( ::ChooseColor(&cc) == TRUE )
	{
		SetColor(cc.rgbResult);
		TellChangedToWindow();
	}
	GetAWin().NVI_SwitchFocusTo(GetControlID());
	return true;
}

#pragma mark ===========================

#pragma mark --- �R���g���[���lSet/Get�C���^�[�t�F�C�X

//Color�擾 
void	CColorPickerPane::GetColor( AColor& outColor ) const
{
	outColor = mColor;
}

//Color�ݒ� 
void	CColorPickerPane::SetColor( const AColor& inColor )
{
	mColor = inColor;
	RefreshControl();
	GetWin().DrawControl(GetControlID());
}

#pragma mark ===========================

#pragma mark ---�R���g���[���t�H�[�J�X����
//#353

/**
�t�H�[�J�X�l��
*/
void	CColorPickerPane::SetFocus() 
{
	mFocused = true;
	RefreshControl();
	FlushControl();
}

/**
�t�H�[�J�X����
*/
void	CColorPickerPane::ResetFocus()
{
	mFocused = false;
	RefreshControl();
	FlushControl();
}

