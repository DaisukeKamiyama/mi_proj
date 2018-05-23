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

#pragma once

#include "CUserPane.h"

#pragma mark ===========================
#pragma mark [�N���X]CColorPickerPane
/**
ColorPicker�����N���X
*/
class CColorPickerPane: public CUserPane
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	CColorPickerPane( AObjectArrayItem* inParent, CWindowImp& inWindow );
	~CColorPickerPane();
	
	//�C�x���g�n���h��
  public:
	ABool	DoDraw();
	ABool	DoMouseDown( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers, 
		const ANumber inClickCount, const ABool inRightButton );
	
	//�R���g���[���lSet/Get�C���^�[�t�F�C�X
  public:
	void	GetColor( AColor& outColor ) const;
	void	SetColor( const AColor& inColor );
	
	//�R���g���[���t�H�[�J�X���� #353
  public:
	void	SetFocus();
	void	ResetFocus();
  private:
	ABool	mFocused;
	
  private:
	AColor	mColor;
	
};


#pragma mark ===========================
#pragma mark [�N���X]CColorPickerPaneFactory
//ColorPicker��ObjectArray�Ő������邽�߂̃N���X
class CColorPickerPaneFactory : public CUserPaneFactory
{
  public:
	CColorPickerPaneFactory( AObjectArrayItem* inParent, CWindowImp& inWindow )
			: mParent(inParent), mWindow(inWindow)
	{
	}
	CColorPickerPane*	Create() 
	{
		return new CColorPickerPane(mParent,mWindow);
	}
  private:
	AObjectArrayItem* mParent;
	CWindowImp&	mWindow;
};


