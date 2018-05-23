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

CPaneForAView (Windows)

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "CUserPane.h"

#pragma mark ===========================
#pragma mark [�N���X]CPaneForAView
/**
AView�p�R���g���[�������N���X
*/
class CPaneForAView: public CUserPane
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	CPaneForAView( AObjectArrayItem* inParent, CWindowImp& inWindow );
	~CPaneForAView();
	void	DestroyUserPane();
	
	//�C�x���g�n���h��
  public:
	ABool	DoDraw();
	void	DoDrawPreProcess();
	void	DoDrawPostProcess();
	ABool	DoMouseDown( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers, 
		const ANumber inClickCount, const ABool inRightButton );
	ABool	DoMouseUp( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers, 
		const ANumber inClickCount, const ABool inRightButton );
	ABool	DoMouseWheel( const AFloatNumber inDeltaY, const AModifierKeys inModifiers, const ALocalPoint inLocalPoint );
	ABool	DoMouseMoved( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers );
	void	DoMouseLeft();
	void	DoControlBoundsChanged();
	ABool	DragEnter( IDataObject* pDataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect );
	ABool	DragOver( DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect );
	ABool	DragLeave();
	ABool	Drop( IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect );
  private:
	void	UpdateTooltip( const ALocalPoint inLocalPoint );
	void	ShowTooltip();
	void	HideTooltip();
	ALocalPoint			mPreviousMousePoint;//DoMouseMoved�̈ړ��`�F�b�N�p
	AText				mCurrentTooltipText;
	ALocalRect			mCurrentTooltipRect;
	ABool				mShowTooltipTimerWorking;
	
	//Tooltip
  public:
	void					EnableTrackingToopTip( const ABool inShowToolTipOnlyWhenNarrow = true );//#507
	ABool					GetEnableTrackingToolTip() { return mEnableTrackingToolTip; }
	void					SetFixedTextTooltip( const AText& inText, const AHelpTagSide inTagSide );//#643
  private:
	ABool								mEnableTrackingToolTip;
	AText								mFixedTextTooltip;
	AHelpTagSide						mFixedTextTooltip_TagSide;//#643
	HWND								mHTooltip;
	TOOLINFOW							mTooltipToolinfo;
	ABool								mTooltipActive;
	ABool								mShowToolTipOnlyWhenNarrow;//#507
	
	//�R���g���[���t�H�[�J�X����#135
  public:
	void	SetFocus();
	void	ResetFocus();
	
	//���� R0231
  public:
	void	ShowDictionary( const AText& inWord, const ALocalPoint inLocalPoint ) const;
	
};


#pragma mark ===========================
#pragma mark [�N���X]CPaneForAViewFactory
//ColorPicker��ObjectArray�Ő������邽�߂̃N���X
class CPaneForAViewFactory : public CUserPaneFactory
{
  public:
	CPaneForAViewFactory( AObjectArrayItem* inParent, CWindowImp& inWindow )
			: mParent(inParent), mWindow(inWindow)
	{
	}
	CPaneForAView*	Create() 
	{
		return new CPaneForAView(mParent,mWindow);
	}
  private:
	AObjectArrayItem* mParent;
	CWindowImp&	mWindow;
};


