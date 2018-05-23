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

AWindow_JumpList

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AView_SubWindowTitlebar.h"

class AWindow_Text;
class AView_List;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_JumpList
/**
�W�����v���X�g�E�C���h�E�̃N���X
*/
class AWindow_JumpList : public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_JumpList( const AWindowID inTextWindowID );
	~AWindow_JumpList();
	
	//<�֘A�h�L�������g�擾>
  private:
	AWindow_Text&			GetTextWindow();
	AWindowID							mTextWindowID;//#199
	AView_SubWindowTitlebar&	GetTitlebarViewByControlID( const AControlID inControlID );
	
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_TableViewStateChanged( const AControlID inID );
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	void					EVTDO_DoCloseButton();
	void					EVTDO_TextInputted( const AControlID inID );//#798
	ABool					NVIDO_ViewReturnKeyPressed( const AControlID inControlID );
	ABool					NVIDO_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers );
	ABool					NVIDO_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers );
	ABool					NVIDO_ViewEscapeKeyPressed( const AControlID inControlID );//#798
	ABool					NVIDO_ViewTabKeyPressed( const AControlID inControlI );
	void					NVIDO_ListViewExpandedCollapsed( const AControlID inControlID );//#892
	void					NVIDO_ListViewHoverUpdated( const AControlID inControlID, 
													   const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex );
	
	//<�C���^�[�t�F�C�X>
	
	//�E�C���h�E����
  public:
	void					SPI_SwitchFocusToSearchBox();//#798
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
	void					NVIDO_DeleteTab( const AIndex inTabIndex );
	void					NVIDO_Hide();
	void					NVIDO_Show();
	void					NVIDO_UpdateProperty();
	void					UpdateViewBounds();
	/*#291
  public:
	void					SPI_SetScrollPosition( const APoint& inPoint );
	void					SPI_GetScrollPosition( APoint& outPoint ) const;
	*/
	
	//�\���X�V
  public:
	void					SPI_UpdateOverlayWindowAlpha();
	void					SPI_UpdateTable( const AIndex inLineIndex, const ABool inForceSetAll );//R0006
	void					SPI_UpdatePosition();
	void					SPI_UpdateSelection( const AIndex inLineIndex );
	void					SPI_SetSideBarMode( const ABool inSideBarMode, const AColor inSideBarColor );//#634
	void					SPI_UpdateSortButtonStatus( const ADocumentID inDocumentID, const ABool inSortOn );//#695
	void					SPI_SetSubWindowTitle( const AText& inTitle );
  private:
	void					SaveWindowPosition();
	
	//ListView�擾
  private:
	AView_List&				GetListView( const AIndex inTabIndex );
	const AView_List&		GetListViewConst( const AIndex inTabIndex ) const;
	
	//Tab���̃f�[�^ #291
  private:
	AArray<AControlID>					mListViewControlIDArray;
	AArray<AViewID>						mListViewIDArray;
	AArray<AControlID>					mVScrollBarControlIDArray;
	AArray<AControlID>					mSortButtonControlIDArray;
	AArray<AControlID>					mJumpListButtonControlIDArray;//#446
	AArray<AControlID>					mSeparatorControlIDArray;//#725
	
	static const ANumber						kVScrollBarWidth = 11;
	static const ANumber						kDragBoxHeight = 8;
	static const ANumber						kButtonHeight = 19;
	static const ANumber						kSortButtonWidth = 70;//#446
	
	//#725
	/** �E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾 */
  public:
	ANumber					SPI_GetSubWindowMinHeight() const;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_JumpList; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_JumpList"; }
};




