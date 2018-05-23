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

ADynamicMenu

*/

#pragma once

#include "../AbsBase/ABase.h"

class AMenuManager;

#pragma mark ===========================
#pragma mark [�N���X]ADynamicMenu
/**
���I���j���[���ڂ��Ǘ�����N���X
*/
class ADynamicMenu : public AObjectArrayItem
{
	//�R���X�g���N�^�^�f�X�g���N�^ 
  public:
	ADynamicMenu( AObjectArrayItem* inParent, AMenuManager& inMenuManager, const AMenuItemID inItemID, const AMenuRef inMenuRef );
	~ADynamicMenu();
  private:
	AMenuManager&					mMenuManager;
	AMenuItemID						mMenuItemID;
	ABool							mIsRoot;
	AMenuRef						mMenuRef;
	//#857 ABool							mIsRealized;
	
	//�֘A�I�u�W�F�N�g�擾 
  private:
	AMenuManager&		GetMenuManager() {return mMenuManager;}
	AMenuManager&		GetMenuManagerConst() const {return mMenuManager;}
	
	//���擾�^�ݒ� 
  public:
	AItemCount			GetMenuItemCount() const;
	AMenuRef			GetMenuRef() const {return mMenuRef;}
	//#857 ABool				IsRealized() const { return mIsRealized; }
	void				GetActionTextByRealMenuItemIndex( const AIndex inRealMenuItemIndex, AText& outText ) const;
	void				GetActionTextByMenuItemArrayIndex( const AIndex inIndex, AText& outText ) const;
	void				SetActionTextByMenuItemArrayIndex( const AIndex inIndex, const AText& inText );
	void				GetMenuTextByMenuItemArrayIndex( const AIndex inIndex, AText& outText ) const;
	void				SetMenuTextByMenuItemArrayIndex( const AIndex inIndex, const AText& inText );
	AObjectID			GetSubMenuObjectIDByMenuItemArrayIndex( const AIndex inIndex ) const;
	void				GetShortcutByMenuItemArrayIndex( const AIndex inIndex, ANumber& outShortcut, AMenuShortcutModifierKeys& outModifiers ) const;
	void				SetShortcutByMenuItemArrayIndex( const AIndex inIndex, const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers );
	AMenuItemID			GetMenuItemID() const {return mMenuItemID;}
	
	//���j���[���ڒǉ��^�폜 
  public:
	void				InsertMenuItem( const AIndex inItemIndex, const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuObjectID,
						const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
						const ABool inEnabled );//#129
	void				AddMenuItem( const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
						const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
						const ABool inEnabled );//#129
	void				SetMenuItem( const ATextArray& inMenuTextArray, const ATextArray& inActionTextArray );
	void				DeleteMenuItem( const AIndex inItemIndex );
	void				DeleteAllMenuItems();
	void				MoveMenuItem( const AIndex inOldIndex, const AIndex inNewIndex );//R0173
	AIndex				GetRealMenuItemIndex( const AIndex inMenuItemArrayIndex );
	void				SetAsFontMenu( const ABool inEnableDefaultFontItem );//#375
	void				SetCheckMarkWithActionText( const AText& inActionText );
	void				SetCheckMark( const AIndex inIndex );//B0308
	void				SetEnableMenu( const ABool inEnable );
	AIndex				FindMenuItemIndexFromActionText( const AText& inActionText ) const;//#695
	
	void				ReOrderByNewActionText( const ATextArray& inActionTextArray);
	
	//���̉��^����̉� 
  public:
	void				Realize();
	void				Unrealize();
	void				RealizeMenuItem( const AIndex inItemIndex );
	void				SetSubMenu( const AIndex inItemIndex, const AObjectID inSubMenuObjectID );
	ABool				IsRealized() const;//#857
  private:
	void				ReRealize();
	void				SetRealized( const ABool inRealized );//#857
	AIndex				GetCurrentRealizedMenuArrayIndex() const;//#857
	static AArray<AMenuItemID>		sCurrentRealizedMenuArray_MenuItemID;//#857
	static AArray<AObjectID>		sCurrentRealizedMenuArray_MenuObjectID;//#857
	
	//Enable/Disable�X�V
  public:
	void				UpdateEnableItems();
	void				UpdateEnableItem( const AIndex inItemIndex );
	void				GetDynamicMenuEnableMenuItem( const AIndex inRealMenuItemIndex, ABool& isEnabled ) const;//#688
	
	//���j���[���ڔz�� 
  private:
	//���j���[�\���e�L�X�g 
	ATextArray						mMenuItemArray_MenuText;
	//���j���[�I�����̃A�N�V�����ɕK�v�ȃe�L�X�g�f�[�^ 
	AHashTextArray					mMenuItemArray_SelectActionText;//#695
	//�T�u���j���[ 
	AArray<AObjectID>				mMenuItemArray_SubMenuObjectID;
	//Enable/Disable 
	AArray<ABool>					mMenuItemArray_Enabled;
	//�V���[�g�J�b�g 
	AArray<ANumber>					mMenuItemArray_ShortcutKey;
	//�V���[�g�J�b�gModifierKey 
	AArray<AMenuShortcutModifierKeys>	mMenuItemArray_ShortcutModifierKeys;
	//
	AArray<ABool>					mMenuItemArray_Separator;
	
	//���j���[�V���[�g�J�b�g�iWindows�p�j
  public:
	static ABool		FindShortcut( const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers,
						AMenuItemID& outMenuItemID, AText& outText );
  private:
	static void			AddToShortcutArray( const AMenuItemID inMenuItemID, const AText& inActionText,
						const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers );
	static void			RemoveFromShortcutArray( const AMenuItemID inMenuItemID, const AText& inActionText );
	static AHashNumberArray			sMenuShortcutArray_Key;
	static AArray<AMenuShortcutModifierKeys>	sMenuShortcutArray_ModifierKeys;
	static AArray<AMenuItemID>		sMenuShortcutArray_MenuItemID;
	static ATextArray				sMenuShortcutArray_ActionText;
	
	//Object���擾 
  public:
	virtual AConstCharPtr	GetClassName() const { return "ADynamicMenu"; }
	
};

#pragma mark ===========================
#pragma mark [�N���X]ADynamicMenuFactory
/**
ADynamicMenu�����pFactory�N���X
*/
class ADynamicMenuFactory : public AAbstractFactoryForObjectArray<ADynamicMenu>
{
  public:
	ADynamicMenuFactory( AObjectArrayItem* inParent, AMenuManager& inMenuManager, const AMenuItemID inItemID, const AMenuRef inMenuRef ) 
	: mParent(inParent), mMenuManager(inMenuManager), mMenuItemID(inItemID), mMenuRef(inMenuRef)
	{
	}
	ADynamicMenu*	Create() 
	{
		return new ADynamicMenu(mParent,mMenuManager,mMenuItemID,mMenuRef);
	}
  private:
	AObjectArrayItem* mParent;
	AMenuManager&	mMenuManager;
	AMenuItemID	mMenuItemID;
	AMenuRef	mMenuRef;
};

