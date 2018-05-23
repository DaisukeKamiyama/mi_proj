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

CPaneForAView

*/

#pragma once

#include "CUserPane.h"

#pragma mark ===========================
#pragma mark [�N���X]CPaneForAView
/**
AView�p�R���g���[��Imp�N���X
*/
class CPaneForAView: public CUserPane
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	CPaneForAView( AObjectArrayItem* inParent, CWindowImp& inWindow, const AControlID inControlID );//#688 const AControlRef inControlRef );
	~CPaneForAView();
	
#if SUPPORT_CARBON
	//�C�x���g�n���h��
  public:
	ABool	APICB_DoDraw();
	ABool	APICB_DoControlBoundsChanged( const EventRef inEventRef );
	ABool	DoMouseDown( const EventRef inEventRef );
	ABool	DoMouseWheel( const EventRef inEventRef );
	ABool	DoAdjustCursor( const EventRef inEventRef );
	void	DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys );
	void	DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys );
	void	DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys );
	void	DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys );
	ABool	DoMouseMoved( const EventRef inEventRef );
	ABool	DoMouseExited( const EventRef inEventRef );//B0000 080810
#endif
	void	PreProcess_SetShowControl( const ABool inShow );//#138 #688
	
#if SUPPORT_CARBON
	//�R���g���[���t�H�[�J�X����#135
  public:
	void	SetFocus();
	void	ResetFocus();
#endif
	
	//���� R0231
  public:
	void	ShowDictionary( const AText& inWord, const ALocalPoint inLocalPoint ) const;
	
};


#pragma mark ===========================
#pragma mark [�N���X]CPaneForAViewFactory
/**
ColorPicker��ObjectArray�Ő������邽�߂̃N���X
*/
class CPaneForAViewFactory : public CUserPaneFactory
{
  public:
	CPaneForAViewFactory( AObjectArrayItem* inParent, CWindowImp& inWindow, AControlID inControlID )//#688const AControlRef inControlRef )
	: mParent(inParent), mWindow(inWindow), mControlID(inControlID)//#688mControlRef(inControlRef)
	{
	}
	CPaneForAView*	Create() 
	{
		return new CPaneForAView(mParent,mWindow,mControlID);//#688mControlRef);
	}
  private:
	AObjectArrayItem* mParent;
	CWindowImp&	mWindow;
	//#688 AControlRef	mControlRef;
	AControlID	mControlID;//#688
};


