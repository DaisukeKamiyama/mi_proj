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

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]ADynamicMenu
#pragma mark ===========================
//���I���j���[���ڂ��Ǘ�����N���X

/*

���I���j���[�ɂ͉��L�̂Q��ނ�����B
1. ���[�g�i�R���X�g���N�^��inMenuRef�Ƀ��j���[��MenuRef��ݒ�j
�@���j���[�o�[�Ɋ��ɑ��݂��郁�j���[�ɁA���ڂ�ǉ�������́B��F�E�C���h�E���j���[
�@�����o�ϐ��̎g�����F
�@�@mMenuRef: ���j���[�o�[�̃��j���[��MenuRef
�@�@mIsRealized: ���j���[�o�[�̃��j���[�Ɏ��̉�����Ă��邩�ǂ���
2. �T�u���j���[�i�R���X�g���N�^��inMenuRef��NULL��ݒ�j
�@��L�̃��[�g���j���[�̃T�u���j���[�B
�@�����o�ϐ��̎g�����F
�@�@mMenuRef: �R���X�g���N�^�Ő��������Ǝ���MenuRef
�@�@mIsRealized: ���true�i���[�g���j���[��mIsRealized�Ƃ͘A�����Ȃ��j
�@�@����ʊK�w�̃��j���[���ڂ��폜���Ă��A���̃T�u���j���[�̎��̉��͉������ꂸ�A�I�u�W�F�N�g���폜����Ȃ��B

*/

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

/**
�R���X�g���N�^ 
*/
ADynamicMenu::ADynamicMenu( AObjectArrayItem* inParent, AMenuManager& inMenuManager, const AMenuItemID inItemID, const AMenuRef inMenuRef ) 
: AObjectArrayItem(inParent), mMenuManager(inMenuManager), mMenuItemID(inItemID), mIsRoot(inMenuRef!=NULL), mMenuRef(inMenuRef)//#857, mIsRealized(false)
{
	//�T�u���j���[�̏ꍇ�AMenuRef�𐶐�
	if( mIsRoot == false )
	{
		mMenuRef = AMenuWrapper::CreateMenu();
		//#857 mIsRealized = true;
	}
}

/**
�f�X�g���N�^
*/
ADynamicMenu::~ADynamicMenu()
{
	DeleteAllMenuItems();
	//�T�u���j���[�̏ꍇ�AMenuRef���폜
	if( mIsRoot == false )
	{
		AMenuWrapper::DeleteMenu(mMenuRef);
	}
}

#pragma mark ===========================

#pragma mark ---���擾�^�ݒ�

/**
���ڐ��擾
*/
AItemCount	ADynamicMenu::GetMenuItemCount() const
{
	return mMenuItemArray_MenuText.GetItemCount();
}

/**
RealMenuItemIndex����ActionText���擾
*/
void	ADynamicMenu::GetActionTextByRealMenuItemIndex( const AIndex inRealMenuItemIndex, AText& outText ) const
{
	AIndex	index = inRealMenuItemIndex;
	if( mIsRoot == true )
	{
		index = inRealMenuItemIndex - GetMenuManagerConst().GetRealMenuItemIndex(mMenuItemID);
	}
	mMenuItemArray_SelectActionText.Get(index,outText);
}

/**
ActionText�̎擾
*/
void	ADynamicMenu::GetActionTextByMenuItemArrayIndex( const AIndex inIndex, AText& outText ) const
{
	mMenuItemArray_SelectActionText.Get(inIndex,outText);
}
/**
ActionText�̐ݒ�
*/
void	ADynamicMenu::SetActionTextByMenuItemArrayIndex( const AIndex inIndex, const AText& inText ) 
{
	mMenuItemArray_SelectActionText.Set(inIndex,inText);
}

/**
MenuText�̎擾
*/
void	ADynamicMenu::GetMenuTextByMenuItemArrayIndex( const AIndex inIndex, AText& outText ) const
{
	mMenuItemArray_MenuText.Get(inIndex,outText);
}
/**
MenuText�̐ݒ�
*/
void	ADynamicMenu::SetMenuTextByMenuItemArrayIndex( const AIndex inIndex, const AText& inText ) 
{
	mMenuItemArray_MenuText.Set(inIndex,inText);
	//#0 ������ ReRealize();
	if( IsRealized() == true )
	{
		AIndex	realMenuItemIndex = GetRealMenuItemIndex(inIndex);
		AMenuWrapper::SetMenuItemText(mMenuRef,realMenuItemIndex,inText);
	}
}

/**
�T�u���j���[��ObjectID�̎擾
*/
AObjectID	ADynamicMenu::GetSubMenuObjectIDByMenuItemArrayIndex( const AIndex inIndex ) const
{
	return mMenuItemArray_SubMenuObjectID.Get(inIndex);
}

/**
�V���[�g�J�b�g�擾
*/
void	ADynamicMenu::GetShortcutByMenuItemArrayIndex( const AIndex inIndex, ANumber& outShortcut, AMenuShortcutModifierKeys& outModifiers ) const
{
	outShortcut = mMenuItemArray_ShortcutKey.Get(inIndex);
	outModifiers = mMenuItemArray_ShortcutModifierKeys.Get(inIndex);
}
/**
�V���[�g�J�b�g�ݒ�
*/
void	ADynamicMenu::SetShortcutByMenuItemArrayIndex( const AIndex inIndex, const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers )
{
	//�V���[�g�J�b�g�����e�[�u������폜 win
	RemoveFromShortcutArray(mMenuItemID,mMenuItemArray_SelectActionText.GetTextConst(inIndex));
	//
	mMenuItemArray_ShortcutKey.Set(inIndex,inShortcut);
	mMenuItemArray_ShortcutModifierKeys.Set(inIndex,inModifiers);
	//�V���[�g�J�b�g�����e�[�u���ɒǉ� win
	if( inShortcut != 0 )
	{
		AddToShortcutArray(mMenuItemID,mMenuItemArray_SelectActionText.GetTextConst(inIndex),
				inShortcut,inModifiers);
	}
	//
	ReRealize();
}

//�V���[�g�J�b�g�����e�[�u���iWindows�̓��j���[�ɓ��I�ɃV���[�g�J�b�g���蓖�Ăł��Ȃ��̂Łj win
AHashNumberArray			ADynamicMenu::sMenuShortcutArray_Key;
AArray<AMenuShortcutModifierKeys>	ADynamicMenu::sMenuShortcutArray_ModifierKeys;
AArray<AMenuItemID>			ADynamicMenu::sMenuShortcutArray_MenuItemID;
ATextArray					ADynamicMenu::sMenuShortcutArray_ActionText;

/**
�V���[�g�J�b�g�����iWindows�p�j
*/
ABool	ADynamicMenu::FindShortcut( const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers,
		AMenuItemID& outMenuItemID, AText& outText )
{
	AArray<AIndex>	indexArray;
	sMenuShortcutArray_Key.FindAll(inShortcut,indexArray);
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		if( sMenuShortcutArray_ModifierKeys.Get(indexArray.Get(i)) == inModifiers )
		{
			outMenuItemID = sMenuShortcutArray_MenuItemID.Get(indexArray.Get(i));
			outText.SetText(sMenuShortcutArray_ActionText.GetTextConst(indexArray.Get(i)));
			return true;
		}
	}
	return false;
}

/**
�V���[�g�J�b�g�����e�[�u���ɒǉ�
*/
void	ADynamicMenu::AddToShortcutArray( const AMenuItemID inMenuItemID, const AText& inActionText,
		const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers )
{
	sMenuShortcutArray_Key.Add(inShortcut);
	sMenuShortcutArray_ModifierKeys.Add(inModifiers);
	sMenuShortcutArray_MenuItemID.Add(inMenuItemID);
	sMenuShortcutArray_ActionText.Add(inActionText);
}

/**
�V���[�g�J�b�g�����e�[�u������폜
*/
void	ADynamicMenu::RemoveFromShortcutArray( const AMenuItemID inMenuItemID, const AText& inActionText )
{
	for( AIndex i = 0; i < sMenuShortcutArray_MenuItemID.GetItemCount(); i++ )
	{
		if( sMenuShortcutArray_MenuItemID.Get(i) == inMenuItemID &&
					sMenuShortcutArray_ActionText.GetTextConst(i).Compare(inActionText) == true )
		{
			sMenuShortcutArray_Key.Delete1(i);
			sMenuShortcutArray_ModifierKeys.Delete1(i);
			sMenuShortcutArray_MenuItemID.Delete1(i);
			sMenuShortcutArray_ActionText.Delete1(i);
			return;
		}
	}
}

#pragma mark ===========================

#pragma mark ---���j���[���ڒǉ��^�폜

/**
���j���[���ڒǉ�
*/
void	ADynamicMenu::InsertMenuItem( const AIndex inItemIndex, const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuObjectID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator, const ABool inEnabled )//#129
{
	//�����f�[�^�ւ̒ǉ�
	mMenuItemArray_MenuText.Insert1(inItemIndex,inMenuText);
	mMenuItemArray_SelectActionText.Insert1(inItemIndex,inActionText);
	mMenuItemArray_SubMenuObjectID.Insert1(inItemIndex,inSubMenuObjectID);
	mMenuItemArray_Enabled.Insert1(inItemIndex,inEnabled);//#129 true->inEnabled
	mMenuItemArray_ShortcutKey.Insert1(inItemIndex,inShortcutKey);
	mMenuItemArray_ShortcutModifierKeys.Insert1(inItemIndex,inShortcutModifierKeys);
	mMenuItemArray_Separator.Insert1(inItemIndex,inSeparator);
	//�V���[�g�J�b�g�����e�[�u���ɒǉ� win
	if( inShortcutKey != 0 )
	{
		AddToShortcutArray(mMenuItemID,mMenuItemArray_SelectActionText.GetTextConst(inItemIndex),
				inShortcutKey,inShortcutModifierKeys);
	}
	
	//���ۂ̃��j���[�Ɏ��̉�����Ă���ꍇ�́A�����j���[�̍X�V���s���B
	if( /*#857 mIsRealized*/IsRealized() == true )
	{
		//���ۂɃ��j���[�ɑ}������
		RealizeMenuItem(inItemIndex);
	}
}

/**
���j���[���ڒǉ�
*/
void	ADynamicMenu::AddMenuItem( const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator, const ABool inEnabled )//#129
{
	InsertMenuItem(GetMenuItemCount(),inMenuText,inActionText,inSubMenuID,inShortcutKey,inShortcutModifierKeys,inSeparator,inEnabled);//#129
}

/**
���j���[���ڐݒ�iArray����ݒ�j
*/
void	ADynamicMenu::SetMenuItem( const ATextArray& inMenuTextArray, const ATextArray& inActionTextArray )
{
	DeleteAllMenuItems();
	for( AIndex i = 0; i < inMenuTextArray.GetItemCount(); i++ )
	{
		AText	menuText, actionText;
		inMenuTextArray.Get(i,menuText);
		inActionTextArray.Get(i,actionText);
		AddMenuItem(menuText,actionText,kObjectID_Invalid,NULL,NULL,false,true);
	}
}

/**
���j���[���ڍ폜
*/
void	ADynamicMenu::DeleteMenuItem( const AIndex inItemIndex )
{
	//�V���[�g�J�b�g�����e�[�u������폜 win
	RemoveFromShortcutArray(mMenuItemID,mMenuItemArray_SelectActionText.GetTextConst(inItemIndex));
	//�����f�[�^�̍폜
	mMenuItemArray_MenuText.Delete1(inItemIndex);
	mMenuItemArray_SelectActionText.Delete1(inItemIndex);
	mMenuItemArray_SubMenuObjectID.Delete1(inItemIndex);
	mMenuItemArray_Enabled.Delete1(inItemIndex);
	mMenuItemArray_ShortcutKey.Delete1(inItemIndex);
	mMenuItemArray_ShortcutModifierKeys.Delete1(inItemIndex);
	mMenuItemArray_Separator.Delete1(inItemIndex);
	
	//���ۂ̃��j���[�Ɏ��̉�����Ă���ꍇ�́A�����j���[�̍X�V���s���B
	if( /*#857 mIsRealized*/IsRealized() == true )
	{
		//�폜
		AMenuWrapper::DeleteMenuItem(mMenuRef,GetRealMenuItemIndex(inItemIndex));
	}
}

//R0173
/**
���j���[���ڈړ�
*/
void	ADynamicMenu::MoveMenuItem( const AIndex inOldIndex, const AIndex inNewIndex )
{
	mMenuItemArray_MenuText.MoveObject(inOldIndex,inNewIndex);
	mMenuItemArray_SelectActionText.MoveObject(inOldIndex,inNewIndex);
	mMenuItemArray_SubMenuObjectID.Move(inOldIndex,inNewIndex);
	mMenuItemArray_Enabled.Move(inOldIndex,inNewIndex);
	mMenuItemArray_ShortcutKey.Move(inOldIndex,inNewIndex);
	mMenuItemArray_ShortcutModifierKeys.Move(inOldIndex,inNewIndex);
	mMenuItemArray_Separator.Move(inOldIndex,inNewIndex);
	//�Ď��̉�
	ReRealize();
}

/**
���j���[���ڑS�폜
*/
void	ADynamicMenu::DeleteAllMenuItems()
{
	while( GetMenuItemCount() > 0 )
	{
		DeleteMenuItem(0);
	}
}

/**
���j���[���ڑSEnable/Disable
*/
void	ADynamicMenu::SetEnableMenu( const ABool inEnable )
{
	/*#695 �������BGetRealMenuItemIndex()�����[�v���Ŏ��s����K�v�Ȃ��B
	for( AIndex i = 0; i < mMenuItemArray_MenuText.GetItemCount(); i++ )
	{
		AMenuWrapper::SetEnableMenuItem(mMenuRef,GetRealMenuItemIndex(i),inEnable);
	}
	*/
	//���ڐ��擾
	AItemCount	itemCount = mMenuItemArray_Enabled.GetItemCount();
	//���ڐ�0�Ȃ烊�^�[��
	if( itemCount == 0 )   return;
	//�ŏ��̍��ڂɂ���GetRealMenuItemIndex()�����s�i���[�v���Ŋe���ڂɃI�t�Z�b�g�l�Ƃ��đ����j
	AIndex	realMenuItemIndexOffset = GetRealMenuItemIndex(0);
	//�e���ڂɂ���SetEnableMenuItem()���s
	for( AIndex index = 0; index < itemCount; index++ )
	{
		//���R�����g�A�E�g DisableAllMenuBarMenuItems()��mMenuItemArray_Enabled���X�V���Ă��Ȃ��̂ŁB
		//if( mMenuItemArray_Enabled.Get(index) != inEnable )
		{
			AMenuWrapper::SetEnableMenuItem(mMenuRef,index+realMenuItemIndexOffset,inEnable);
			mMenuItemArray_Enabled.Set(index,inEnable);
		}
	}
}

/**
ADynamicMenu�I�u�W�F�N�g���̍���Index����RealMenuItemIndex�ւ̕ϊ�
*/
AIndex	ADynamicMenu::GetRealMenuItemIndex( const AIndex inMenuItemArrayIndex )
{
	AIndex	realMenuItemIndex = inMenuItemArrayIndex;
	//���I���j���[�̃��[�g�̏ꍇ�́A�Y�����j���[�̏ꏊ��T���K�v������B�i��ɌŒ胁�j���[�⑼�̓��I���j���[�����݂�����B�j
	if( mIsRoot == true )
	{
		realMenuItemIndex = GetMenuManager().GetRealMenuItemIndex(mMenuItemID) + inMenuItemArrayIndex;
	}
	return realMenuItemIndex;
}

/**
�t�H���g���j���[�Ƃ��Đݒ�
*/
void	ADynamicMenu::SetAsFontMenu( const ABool inEnableDefaultFontItem )//#375
{
	if( /*#857 mIsRealized*/IsRealized() == false )   Realize();
	AMenuWrapper::InsertFontMenu(mMenuRef,0,mMenuItemID);
	
	//�����j���[��������z����t�\��
	for( AIndex index = 0; index < AMenuWrapper::GetMenuItemCount(mMenuRef); index++ )
	{
		AText	text;
		AMenuWrapper::GetMenuItemText(mMenuRef,index,text);
		mMenuItemArray_MenuText.Insert1(index,text);
		/*win FMFontFamily	fontFamily;
		FMFontStyle	style;
		::GetFontFamilyFromMenuSelection(mMenuRef,1+index,&fontFamily,&style);
		text.SetFormattedCstring("%d",fontFamily);*/
		mMenuItemArray_SelectActionText.Insert1(index,text);
		mMenuItemArray_SubMenuObjectID.Insert1(index,kObjectID_Invalid);
		mMenuItemArray_Enabled.Insert1(index,true);
		mMenuItemArray_ShortcutKey.Insert1(index,0);
		mMenuItemArray_ShortcutModifierKeys.Insert1(index,0);
		mMenuItemArray_Separator.Insert1(index,0);
	}
	
	//#375
	if( inEnableDefaultFontItem == true )
	{
		AText	defaultFontTitle;
		defaultFontTitle.SetLocalizedText("DefaultFontTitle");
		AText	defaultFontValue("default");
		InsertMenuItem(0,defaultFontTitle,defaultFontValue,kObjectID_Invalid,0,0,false,true);
	}
}

/**
ActionText�ɑΉ����鍀�ڂɃ`�F�b�N�}�[�N������
*/
void	ADynamicMenu::SetCheckMarkWithActionText( const AText& inActionText )
{
	/*#695 �������BGetRealMenuItemIndex()�����[�v���Ŏ��s����K�v�Ȃ��B
	for( AIndex index = 0; index < mMenuItemArray_SelectActionText.GetItemCount(); index++ )
	{
		if( mMenuItemArray_SelectActionText.GetTextConst(index).Compare(inActionText) == true )
		{
			AMenuWrapper::SetCheckMark(mMenuRef,GetRealMenuItemIndex(index),true);
		}
		else
		{
			AMenuWrapper::SetCheckMark(mMenuRef,GetRealMenuItemIndex(index),false);
		}
	}
	*/
	//���ڐ��擾
	AItemCount	itemCount = mMenuItemArray_SelectActionText.GetItemCount();
	//���ڐ�0�Ȃ烊�^�[��
	if( itemCount == 0 )   return;
	//�ŏ��̍��ڂɂ���GetRealMenuItemIndex()�����s�i���[�v���Ŋe���ڂɃI�t�Z�b�g�l�Ƃ��đ����j
	AIndex	realMenuItemIndexOffset = GetRealMenuItemIndex(0);
	//�e���ڂɂ��ă`�F�b�Noff�ɂ���
	for( AIndex index = 0; index < itemCount; index++ )
	{
		AMenuWrapper::SetCheckMark(mMenuRef,index+realMenuItemIndexOffset,false);
	}
	//inActionText�ɑΉ����鍀�ڂ̃`�F�b�Non�ɂ���
	AIndex	checkItemIndex = mMenuItemArray_SelectActionText.Find(inActionText);
	if( checkItemIndex != kIndex_Invalid )
	{
		AMenuWrapper::SetCheckMark(mMenuRef,checkItemIndex+realMenuItemIndexOffset,true);
	}
}

//B0308
/**
�w��Index�̃��j���[���ڂɃ`�F�b�N�}�[�N������
*/
void	ADynamicMenu::SetCheckMark( const AIndex inIndex )
{
	/*#695 �������BGetRealMenuItemIndex()�����[�v���Ŏ��s����K�v�Ȃ��B
	AItemCount	itemCount = mMenuItemArray_SelectActionText.GetItemCount();
	for( AIndex index = 0; index < itemCount; index++ )
	{
		if( index == inIndex )
		{
			AMenuWrapper::SetCheckMark(mMenuRef,GetRealMenuItemIndex(index),true);
		}
		else
		{
			AMenuWrapper::SetCheckMark(mMenuRef,GetRealMenuItemIndex(index),false);
		}
	}
	*/
	//���ڐ��擾
	AItemCount	itemCount = mMenuItemArray_SelectActionText.GetItemCount();
	//���ڐ�0�Ȃ烊�^�[��
	if( itemCount == 0 )   return;
	//�ŏ��̍��ڂɂ���GetRealMenuItemIndex()�����s�i���[�v���Ŋe���ڂɃI�t�Z�b�g�l�Ƃ��đ����j
	AIndex	realMenuItemIndexOffset = GetRealMenuItemIndex(0);
	//�e���ڂ̃`�F�b�Noff
	for( AIndex index = 0; index < itemCount; index++ )
	{
		AMenuWrapper::SetCheckMark(mMenuRef,index+realMenuItemIndexOffset,false);
	}
	//�w��index�̂݃`�F�b�Non
	if( inIndex != kIndex_Invalid )
	{
		AMenuWrapper::SetCheckMark(mMenuRef,inIndex+realMenuItemIndexOffset,true);
	}
}

//#695
/**
ActionText����A�Ή����郁�j���[���ڂ̃C���f�b�N�X����������
*/
AIndex	ADynamicMenu::FindMenuItemIndexFromActionText( const AText& inActionText ) const
{
	return mMenuItemArray_SelectActionText.Find(inActionText);
}

#pragma mark ===========================

#pragma mark ---���̉��^����̉�

/**
���j���[���̉�
�T�u���j���[���܂߂āA���I���j���[�����ۂ̃��j���[��Ɏ��̉�����B
*/
void	ADynamicMenu::Realize()
{
	if( mIsRoot == true )
	{
		//���̃��j���[��ɑ��݂��铯��MenuItemID�̃��j���[���ڂ��폜����
		//�i����DynamicMenu�I�u�W�F�N�g�ɂ��ǉ����ꂽ���ڂ́A�����ō폜�����B�j
		AIndex	startIndex = GetRealMenuItemIndex(0);
		while( startIndex < AMenuWrapper::GetMenuItemCount(mMenuRef) )//#688 ���̃��j���[���startIndex�Ԗڂ̍��ڂ����邩�ǂ����̃`�F�b�N�ǉ�
		{
			//#688 startIndex�Ԗڂ̍��ڂ�MenuItemID����v���Ȃ��Ȃ�����I��
			if( AMenuWrapper::GetMenuItemID(mMenuRef,startIndex) != mMenuItemID )
			{
				break;
			}
			//startIndex�Ԗڂ̍��ڂ��폜
			AMenuWrapper::DeleteMenuItem(mMenuRef,startIndex);
		}
	}
	else
	{
		//�T�u���j���[�̓R���X�g���N�^�Ŏ��̉��ς�
		_ACError("",this);
		return;
	}
	//���j���[���ڂ̒ǉ�
	AItemCount	itemCount = GetMenuItemCount();
	for( AIndex index = 0; index < itemCount; index++ )
	{
		RealizeMenuItem(index);
	}
	//#857 mIsRealized = true;
	SetRealized(true);//#857
}

/**
���j���[���̉�����
*/
void	ADynamicMenu::Unrealize()
{
	if( mIsRoot == true )
	{
		//���̃��j���[��ɑ��݂��铯��MenuItemID�̃��j���[���ڂ��폜����
		//�i����DynamicMenu�I�u�W�F�N�g�ɂ��ǉ����ꂽ���ڂ́A�����ō폜�����B�j
		AIndex	startIndex = GetRealMenuItemIndex(0);
		while( startIndex < AMenuWrapper::GetMenuItemCount(mMenuRef) )//#688 ���̃��j���[���startIndex�Ԗڂ̍��ڂ����邩�ǂ����̃`�F�b�N�ǉ�
		{
			//#688 startIndex�Ԗڂ̍��ڂ�MenuItemID����v���Ȃ��Ȃ�����I��
			if( AMenuWrapper::GetMenuItemID(mMenuRef,startIndex) != mMenuItemID )
			{
				break;
			}
			//startIndex�Ԗڂ̍��ڂ��폜
			AMenuWrapper::DeleteMenuItem(mMenuRef,startIndex);
		}
	}
	else
	{
		//�T�u���j���[�̓R���X�g���N�^�Ŏ��̉��ς�
		_ACError("",this);
		return;
	}
	//#857 mIsRealized = false;
	SetRealized(false);//#857
}

/**
���j���[���ڎ��̉�
*/
void	ADynamicMenu::RealizeMenuItem( const AIndex inItemIndex )
{
	AIndex	realMenuItemIndex = GetRealMenuItemIndex(inItemIndex);
	//���j���[���ڒǉ�
	if( mMenuItemArray_Separator.Get(inItemIndex) == true )
	{
		AMenuWrapper::InsertMenuItem_Separator(mMenuRef,realMenuItemIndex,mMenuItemID);//#427
	}
	else
	{
		AText	text;//, actiontext;
		mMenuItemArray_MenuText.Get(inItemIndex,text);
		//mMenuItemArray_SelectActionText.Get(inItemIndex,actiontext);//win 080714
		AMenuWrapper::InsertMenuItem(mMenuRef,realMenuItemIndex,text,mMenuItemID,
				mMenuItemArray_ShortcutKey.Get(inItemIndex),mMenuItemArray_ShortcutModifierKeys.Get(inItemIndex));
		//#129
		AMenuWrapper::SetEnableMenuItem(mMenuRef,realMenuItemIndex,mMenuItemArray_Enabled.Get(inItemIndex));
		//�T�u���j���[�L��Ȃ�T�u���j���[��ݒ�
		AObjectID	subMenuObjectID = mMenuItemArray_SubMenuObjectID.Get(inItemIndex);
		if( subMenuObjectID != kObjectID_Invalid )
		{
			AMenuWrapper::SetSubMenu(mMenuRef,realMenuItemIndex,GetMenuManager().GetDynamicMenuByObjectID(subMenuObjectID).GetMenuRef());
		}
	}
}

/**
�T�u���j���[��ݒ肷��
*/
void	ADynamicMenu::SetSubMenu( const AIndex inItemIndex, const AObjectID inSubMenuObjectID )
{
	mMenuItemArray_SubMenuObjectID.Set(inItemIndex,inSubMenuObjectID);
	AMenuWrapper::SetSubMenu(mMenuRef,GetRealMenuItemIndex(inItemIndex),GetMenuManager().GetDynamicMenuByObjectID(inSubMenuObjectID).GetMenuRef());
}

/**
���̃��j���[�č\��
*/
void	ADynamicMenu::ReRealize()
{
	//���̃��j���[�č\��
	if( /*#857 mIsRealized*/IsRealized() == true )
	{
		//���̃��j���[��ɑ��݂��铯��MenuItemID�̃��j���[���ڂ��폜����
		AIndex	startIndex = GetRealMenuItemIndex(0);
		while( startIndex < AMenuWrapper::GetMenuItemCount(mMenuRef) )//#688 ���̃��j���[���startIndex�Ԗڂ̍��ڂ����邩�ǂ����̃`�F�b�N�ǉ�
		{
			//#688 startIndex�Ԗڂ̍��ڂ�MenuItemID����v���Ȃ��Ȃ�����I��
			if( AMenuWrapper::GetMenuItemID(mMenuRef,startIndex) != mMenuItemID )
			{
				break;
			}
			//startIndex�Ԗڂ̍��ڂ��폜
			AMenuWrapper::DeleteMenuItem(mMenuRef,startIndex);
		}
		//
		AItemCount	itemCount = GetMenuItemCount();
		for( AIndex index = 0; index < itemCount; index++ )
		{
			RealizeMenuItem(index);
		}
	}
}

//#857
//Realized����parray
//�i1��MenuItemID�ɑ΂��A������ADynamicMenu�I�u�W�F�N�g���Ή����Arealize����Ƃ��ɕK����������unrealized���Ȃ��̂ŁA
//��ɍŌ��realize���ꂽ1��realized�Ɣ���ł���悤�ɁAstatic�ȕϐ���p����j
//�e���j���[ID����realize����Ă���ADynamicMenu�I�u�W�F�N�g��ObjectID���L��
AArray<AMenuItemID>		ADynamicMenu::sCurrentRealizedMenuArray_MenuItemID;
AArray<AObjectID>		ADynamicMenu::sCurrentRealizedMenuArray_MenuObjectID;

//#857
/**
����Realize����Ă��邩�ǂ����̔���
*/
ABool	ADynamicMenu::IsRealized() const
{
	//���[�g���j���[�ȊO�͏��realized
	if( mIsRoot == false )
	{
		return true;
	}
	//����realize����Ă���ADynamicMenu�I�u�W�F�N�g���������A�{�I�u�W�F�N�g�ƈ�v���Ă�����true��Ԃ�
	AIndex	index = GetCurrentRealizedMenuArrayIndex();
	return (sCurrentRealizedMenuArray_MenuObjectID.Get(index)==GetObjectID());
}

//#857
/**
Realize���ꂽ���Ƃ�ݒ肷��
*/
void	ADynamicMenu::SetRealized( const ABool inRealized )
{
	//���[�g���j���[�ȊO�̏ꍇ�͉������Ȃ�
	if( mIsRoot == false )
	{
		return;
	}
	//realize���ꂽ�Ȃ�{�I�u�W�F�N�g��ObjectID, unrealilze���ꂽ�Ȃ�kObjectID_Invalid��ݒ肷��
	AObjectID	objectID = kObjectID_Invalid;
	if( inRealized == true )
	{
		objectID = GetObjectID();
	}
	//����realize����Ă���ADynamicMenu�I�u�W�F�N�g���������AObjectID��ݒ�
	AIndex	index = GetCurrentRealizedMenuArrayIndex();
	sCurrentRealizedMenuArray_MenuObjectID.Set(index,objectID);
}

//#857
/**
sCurrentRealizedMenuArray_MenuItemID����mMenuItemID�ɑΉ����鍀�ڂ�����
*/
AIndex	ADynamicMenu::GetCurrentRealizedMenuArrayIndex() const
{
	//�z�񂩂�AmMenuItemID�ɑΉ����鍀�ڂ�����
	AIndex	index = sCurrentRealizedMenuArray_MenuItemID.Find(mMenuItemID);
	if( index == kIndex_Invalid )
	{
		//mMenuItemID�ɑΉ����鍀�ڂ�������ΐ�������
		index = sCurrentRealizedMenuArray_MenuItemID.Add(mMenuItemID);
		sCurrentRealizedMenuArray_MenuObjectID.Add(kObjectID_Invalid);
	}
	return index;
}

#pragma mark ===========================

#pragma mark ---Enable/Disable�X�V
//#129

/**
Enable/Disable�X�V
*/
void	ADynamicMenu::UpdateEnableItems()
{
	if( /*#857 mIsRealized*/IsRealized() == false )   return;
	AItemCount	itemCount = GetMenuItemCount();
	for( AIndex index = 0; index < itemCount; index++ )
	{
		UpdateEnableItem(index);
	}
}

/**
Enable/Disable�X�V
*/
void	ADynamicMenu::UpdateEnableItem( const AIndex inItemIndex )
{
	//#688
	//�o�b�t�@���[�h����Enable/Disable�ݒ肵�Ȃ��iAppDelegate��validateMenuItem��Enable/Disable�ݒ肳���j
	if( AApplication::GetApplication().NVI_GetMenuManager().GetEnableMenuItemBufferredMode() == true )
	{
		return;
	}
	//
	AIndex	realMenuItemIndex = GetRealMenuItemIndex(inItemIndex);
	if( mMenuItemArray_Separator.Get(inItemIndex) == true )
	{
		AMenuWrapper::SetEnableMenuItem(mMenuRef,realMenuItemIndex,false);
	}
	else
	{
		AMenuWrapper::SetEnableMenuItem(mMenuRef,realMenuItemIndex,mMenuItemArray_Enabled.Get(inItemIndex));
	}
}

//#688
/**
Enable/Disable�̒l�擾�iAppDelegate��validateMenuItem����R�[�������Bmenu��auto validate�œ��삵�Ă���̂ŁA���̋@�\�ɍ��킹�邽�߁j
*/
void	ADynamicMenu::GetDynamicMenuEnableMenuItem( const AIndex inRealMenuItemIndex, ABool& isEnabled ) const
{
	AIndex	index = inRealMenuItemIndex - GetMenuManagerConst().GetRealMenuItemIndex(mMenuItemID);
	if( mMenuItemArray_Separator.Get(index) == true )
	{
		isEnabled = false;
	}
	else
	{
		isEnabled = mMenuItemArray_Enabled.Get(index);
	}
}

