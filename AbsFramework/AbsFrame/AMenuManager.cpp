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

AMenuManager

*/

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AMenuManager
#pragma mark ===========================
//���j���[�Ǘ��N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

//�R���X�g���N�^
AMenuManager::AMenuManager() : AObjectArrayItem(NULL)
,mDynamicMenuArray(NULL)//#693,true)//#417koko
,mEnableMenuItemBufferredMode(false)
{
}
//�f�X�g���N�^
AMenuManager::~AMenuManager()
{
}

#pragma mark ===========================

#pragma mark ---���j���[�o�[�̃��j���[�̊Ǘ��f�[�^������

//���j���[�o�[�̃��j���[�̏�����
void	AMenuManager::InitMenuBarMenu()
{
	//���j���[API�����ݒ�
	AMenuWrapper::InitMenu();
	
	//���j���[���ڂ̓o�^
	AMenuRef	rootMenuRef = AMenuWrapper::GetRootMenu();
	for( AIndex rootIndex = 0; rootIndex < AMenuWrapper::GetMenuItemCount(rootMenuRef); rootIndex++ )
	{
		AMenuRef	menuRef = AMenuWrapper::GetSubMenu(rootMenuRef,rootIndex);
		if( menuRef != NULL )
		{
			RegisterMenuBarMenuItem(menuRef);
		}
	}
	//
	for( AIndex i = 0; i < mMenuBarMenuRefArray.GetItemCount(); i++ )
	{
		AMenuRef	menuRef = mMenuBarMenuRefArray.Get(i);
		for( AIndex j = 0; j < mMenuBarMenuItemArray_MenuRef.GetItemCount(); j++ )
		{
			if( mMenuBarMenuItemArray_MenuRef.Get(j) == menuRef )
			{
				mMenuBarMenuRefArray_ItemIndexArray.GetObject(i).Add(j);
			}
		}
	}
}

//���j���[�o�[�̃��j���[���ċA�I�Ɍ������āA���j���[���ڂ�mMenuBarMenuItemArray_XXX�֓o�^���Ă����B
void	AMenuManager::RegisterMenuBarMenuItem( const AMenuRef inMenuRef )
{
#if IMPLEMENTATION_FOR_MACOSX
	//#688 ���j���[�Ɏ���validate(setAutoenablesItems:YES)��ݒ�
	AMenuWrapper::EnableAutoEnableMenu(inMenuRef);
#endif
	//
	for( AIndex index = 0; index < AMenuWrapper::GetMenuItemCount(inMenuRef); index++ )
	{
		AMenuRef	subMenuRef = AMenuWrapper::GetSubMenu(inMenuRef,index);
		AMenuItemID	itemID = AMenuWrapper::GetMenuItemID(inMenuRef,index);
		//���j���[���ڂ̓o�^�iRealMenuItemIndex�̌v�Z�̕K�v��A�Z�p���[�^��MenuItemID��0�ł�����̂��܂߂āA�S���o�^����B�j
		mMenuBarMenuItemArray_MenuItemID.Add(itemID);
		mMenuBarMenuItemArray_MenuRef.Add(inMenuRef);
		mMenuBarMenuItemArray_MenuItemStaticIndex.Add(index);
		mMenuBarMenuItemArray_DynamicMenuID.Add(kObjectID_Invalid);
		mMenuBarMenuItemArray_IsDynamicMenu.Add(false);
		mMenuBarMenuItemArray_OnlyStatic.Add(true);
		//�T�u���j���[�L��Ȃ�ċA�R�[��
		if( subMenuRef != NULL )
		{
			RegisterMenuBarMenuItem(subMenuRef);
		}
		
		if( mMenuBarMenuRefArray.Find(inMenuRef) == kIndex_Invalid )
		{
			mMenuBarMenuRefArray.Add(inMenuRef);
			mMenuBarMenuRefArray_ItemIndexArray.AddNewObject();
		}
	}
}

#pragma mark ===========================

#pragma mark ---�Œ胁�j���[���ڐ���

//�S�Ẵ��j���[�o�[���j���[���ڂ�Disable�ɂ���
void	AMenuManager::DisableAllMenuBarMenuItems()
{
	/* AItemCount	itemCount = mMenuBarMenuItemArray_MenuItemID.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AMenuItemID	menuItemID = mMenuBarMenuItemArray_MenuItemID.Get(i);
		AMenuRef	menuRef = mMenuBarMenuItemArray_MenuRef.Get(i);
		AObjectID	dynamicMenuID = mMenuBarMenuItemArray_DynamicMenuID.Get(i);
		if( dynamicMenuID == kObjectID_Invalid && menuItemID != 0 )
		{
			AMenuWrapper::SetEnableMenuItem(menuRef,GetRealMenuItemIndex(menuItemID),false);
		}
		else if( dynamicMenuID != kObjectID_Invalid )
		{
			GetDynamicMenuByObjectID(dynamicMenuID).SetEnableMenu(false);
		}
	}*/
	/*#688 �o�b�t�@�[���[�h�Ή��̂��߁ASetEnableMenuItem()���g���悤�ɕύX
	AItemCount	count = mMenuBarMenuRefArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AMenuRef	menuRef = mMenuBarMenuRefArray.Get(i);
		AItemCount	count2 = AMenuWrapper::GetMenuItemCount(menuRef);
		for( AIndex j = 0; j < count2; j++ )
		{
			if( AMenuWrapper::GetMenuItemID(menuRef,j) != 0 )
			{
				AMenuWrapper::SetEnableMenuItem(menuRef,j,false);
			}
			AMenuItemID	menuItemID = AMenuWrapper::GetMenuItemID(menuRef,j);
		}
	}
	*/
	//InitMenuBarMenu()�Ŏ擾�����eMenuItemID�ɂ��āA����disable�ɂ���
	AItemCount	count = mMenuBarMenuItemArray_MenuItemID.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AMenuItemID	menuItemID = mMenuBarMenuItemArray_MenuItemID.Get(i);
		if( menuItemID != 0 )
		{
			SetEnableMenuItem(menuItemID,false);
		}
	}
}

//���j���[�e�L�X�g�ݒ�
void	AMenuManager::SetMenuItemText( const AMenuItemID inItemID, const AText& inText )
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   return;
	AMenuWrapper::SetMenuItemText(mMenuBarMenuItemArray_MenuRef.Get(index),GetRealMenuItemIndex(inItemID),inText);
}

//���j���[�e�L�X�g�擾
void	AMenuManager::GetMenuItemText( const AMenuItemID inItemID, AText& outText ) const
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   return;
	AMenuWrapper::GetMenuItemText(mMenuBarMenuItemArray_MenuRef.Get(index),GetRealMenuItemIndex(inItemID),outText);
}

//���j���[����Enable/Disable
void	AMenuManager::SetEnableMenuItem( const AMenuItemID inItemID, const ABool inEnable )
{
	//#688 �o�b�t�@�[���[�h�Ή�
	//�o�b�t�@�[���[�h���Ȃ�o�b�t�@�ւ̐ݒ�݂̂��ă��^�[��
	if( mEnableMenuItemBufferredMode == true )
	{
		AIndex	index = mEnableMenuItemBuffer_ItemID.Find(inItemID);
		if( index != kIndex_Invalid )
		{
			//�o�b�t�@��inItemID�̍��ڂ�����΁A���̃o�b�t�@��inEnable��ݒ�
			mEnableMenuItemBuffer_BufferEnabled.Set(index,inEnable);
		}
		else
		{
			//�o�b�t�@��inItemID�̍��ڂ��Ȃ���΁A�o�b�t�@�ǉ����āAinEnable��ݒ�
			mEnableMenuItemBuffer_ItemID.Add(inItemID);
			mEnableMenuItemBuffer_BufferEnabled.Add(inEnable);
			mEnableMenuItemBuffer_CurrentEnabled.Add(!inEnable);//����K�����f�����邽�ߔ��]���Đݒ�
		}
		return;
	}
	
	//#688 �o�b�t�@�[���[�h�Ή�
	//�o�b�t�@�[���[�h���łȂ���΃o�b�t�@�i�ƌ��݃f�[�^�j�ɐݒ�
	{
		AIndex	index = mEnableMenuItemBuffer_ItemID.Find(inItemID);
		if( index != kIndex_Invalid )
		{
			mEnableMenuItemBuffer_BufferEnabled.Set(index,inEnable);
			mEnableMenuItemBuffer_CurrentEnabled.Set(index,inEnable);
		}
	}
	
	//
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index != kIndex_Invalid )
	{
		if( mMenuBarMenuItemArray_DynamicMenuID.Get(index) != kObjectID_Invalid )
		{
			GetDynamicMenuByMenuItemID(inItemID).SetEnableMenu(inEnable);
			return;
		}
	}

	AArray<AIndex>	foundIndexArray;
	mMenuBarMenuItemArray_MenuItemID.FindAll(inItemID,foundIndexArray);
	for( AIndex i = 0; i < foundIndexArray.GetItemCount(); i++ )
	{
		AIndex	index = foundIndexArray.Get(i);
		AMenuWrapper::SetEnableMenuItem(mMenuBarMenuItemArray_MenuRef.Get(index),GetRealMenuItemIndexByMenuItemArrayIndex(index),inEnable);
	}
}

//#688
/**
�o�b�t�@�[���[�h�I�����āA���j���[���ڂ�enable/disable�ݒ�
*/
void	AMenuManager::EndEnableMenuItemBufferredMode()
{
	//�o�b�t�@�[���[�h����
	mEnableMenuItemBufferredMode = false;
	//�o�b�t�@����ǂݏo���āA�K�v�Ȃ烁�j���[���ڂ�enable/disable�ݒ�
	AItemCount	itemCount = mEnableMenuItemBuffer_ItemID.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		ABool	bufferEnabled = mEnableMenuItemBuffer_BufferEnabled.Get(i);
		ABool	currentEnabled = mEnableMenuItemBuffer_CurrentEnabled.Get(i);
		if( bufferEnabled != currentEnabled )
		{
			//���ݒl�ƃo�b�t�@�l���قȂ��Ă�����A���j���[���ڂ�enable/disable�ݒ�
			SetEnableMenuItem(mEnableMenuItemBuffer_ItemID.Get(i),bufferEnabled);
		}
	}
}

//#688
/**
�o�b�t�@�[�l�ǂݏo��
*/
ABool	AMenuManager::GetBufferredEnableMenuItem( const AMenuItemID inMenuItemID ) const
{
	AIndex	index = mEnableMenuItemBuffer_ItemID.Find(inMenuItemID);
	if( index == kIndex_Invalid )   return false;//EVT_UpdateMenu()�����s��Aenable�ɂ��ׂ����̂͑S��mEnableMenuItemBuffer_ItemID�ɓo�^�����͂�
	return mEnableMenuItemBuffer_BufferEnabled.Get(index);
}

//���j���[���ڂɃV���[�g�J�b�g�ݒ�
void	AMenuManager::SetMenuItemShortcut( const AMenuItemID inItemID, const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   return;
	AMenuWrapper::SetShortcut(mMenuBarMenuItemArray_MenuRef.Get(index),GetRealMenuItemIndex(inItemID),inShortcutKey,inShortcutModifierKeys);
}

//#798
/**
���j���[���ڂ̃V���[�g�J�b�g���擾
*/
void	AMenuManager::GetMenuItemShortcut( const AMenuItemID inItemID, ANumber& outShortcutKey, AMenuShortcutModifierKeys& outShortcutModifierKeys ) const
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   return;
	AMenuWrapper::GetShortcut(mMenuBarMenuItemArray_MenuRef.Get(index),GetRealMenuItemIndex(inItemID),outShortcutKey,outShortcutModifierKeys);
}

//#798
/**
���j���[���ڂ̕\���e�L�X�g���擾
*/
void	AMenuManager::GetMenuItemShortcutDisplayText( AMenuItemID inItemID, AText& outText ) const
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   return;
	AMenuWrapper::GetShortcutDisplayText(mMenuBarMenuItemArray_MenuRef.Get(index),GetRealMenuItemIndex(inItemID),outText);
}

//���j���[�e�L�X�g���瓾���鐔�l��inNumber�Ɠ������̂Ƀ`�F�b�N�}�[�N������i�t�H���g�T�C�Y���j���[���p�j
void	AMenuManager::CheckSameNumberMenuItem( const AArray<AMenuItemID>& inItemIDArray, const AFloatNumber inNumber )//win
{
	//������itemID�̒����猟�� win
	AArray<AIndex>	foundIndexArray;
	for( AIndex i = 0; i < inItemIDArray.GetItemCount(); i++ )
	{
		AArray<AIndex>	indexArray;
		mMenuBarMenuItemArray_MenuItemID.FindAll(inItemIDArray.Get(i),indexArray);
		for( AIndex j = 0; j < indexArray.GetItemCount(); j++ )
		{
			foundIndexArray.Add(indexArray.Get(j));
		}
	}
	//
	for( AIndex i = 0; i < foundIndexArray.GetItemCount(); i++ )
	{
		AIndex	index = foundIndexArray.Get(i);
		AMenuRef	menuRef = mMenuBarMenuItemArray_MenuRef.Get(index);
		AIndex	realMenuIetmIndex = GetRealMenuItemIndexByMenuItemArrayIndex(index);
		AText	text;
		AMenuWrapper::GetMenuItemText(menuRef,realMenuIetmIndex,text);
		if( text.GetFloatNumber() == inNumber )
		{
			AMenuWrapper::SetCheckMark(menuRef,realMenuIetmIndex,true);
		}
		else
		{
			AMenuWrapper::SetCheckMark(menuRef,realMenuIetmIndex,false);
		}
	}
}

//�`�F�b�N�}�[�N
void	AMenuManager::SetCheckMark( const AMenuItemID inItemID, const ABool inCheck )
{
	AArray<AIndex>	foundIndexArray;
	mMenuBarMenuItemArray_MenuItemID.FindAll(inItemID,foundIndexArray);
	for( AIndex i = 0; i < foundIndexArray.GetItemCount(); i++ )
	{
		AIndex	index = foundIndexArray.Get(i);
		AMenuRef	menuRef = mMenuBarMenuItemArray_MenuRef.Get(index);
		AIndex	realMenuIetmIndex = GetRealMenuItemIndexByMenuItemArrayIndex(index);
		AMenuWrapper::SetCheckMark(menuRef,realMenuIetmIndex,inCheck);
	}
}

//
void	AMenuManager::GetMenuActionText( const AMenuItemID inItemID, const AMenuRef inMenuRef, const AIndex inRealMenuItemIndex, AText& outText ) const
{
	//ContextMenu���猟�� #232
	AIndex	contextMenuIndex = mContextMenuArray_MenuRef.Find(inMenuRef);
	if( contextMenuIndex != kIndex_Invalid )
	{
		if( mContextMenuArray_ActionText.GetObjectConst(contextMenuIndex).GetItemCount() > 0 )
		{
			outText.SetText(mContextMenuArray_ActionText.GetObjectConst(contextMenuIndex).GetTextConst(inRealMenuItemIndex));
		}
		else
		{
			AMenuWrapper::GetMenuItemText(inMenuRef,inRealMenuItemIndex,outText);
		}
		return;
	}
	//
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( menuItemArrayIndex == kIndex_Invalid )
	{
		//R0199
		AMenuWrapper::GetMenuItemText(inMenuRef,inRealMenuItemIndex,outText);
		return;
	}
	if( mMenuBarMenuItemArray_DynamicMenuID.Get(menuItemArrayIndex) == kObjectID_Invalid )
	{
		AMenuWrapper::GetMenuItemText(inMenuRef,inRealMenuItemIndex,outText);
	}
	else
	{
		//AStMutexLocker	mutexlocker(mDynamicMenuArrayMutex);
		for( AIndex dynamicMenuObjectIndex = 0; dynamicMenuObjectIndex < mDynamicMenuArray.GetItemCount(); dynamicMenuObjectIndex++ )
		{
			const ADynamicMenu&	dynamicMenu = mDynamicMenuArray.GetObjectConst(dynamicMenuObjectIndex);
			if(	dynamicMenu.IsRealized() == true && 
				dynamicMenu.GetMenuRef() == inMenuRef &&
				dynamicMenu.GetMenuItemID() == inItemID )
			{
				dynamicMenu.GetActionTextByRealMenuItemIndex(inRealMenuItemIndex,outText);
				return;
			}
		}
		//#232
		AMenuWrapper::GetMenuItemText(inMenuRef,inRealMenuItemIndex,outText);
	}
}

//win
/**
���j���[�A�C�R���ݒ�
*/
void	AMenuManager::SetMenuItemIcon( const AMenuItemID inItemID, const AIconID inIconID, const AIndex inOffset )
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   return;
	AMenuWrapper::SetMenuItemIcon(mMenuBarMenuItemArray_MenuRef.Get(index),
				GetRealMenuItemIndex(inItemID)+inOffset,inIconID);
}

#pragma mark ===========================

#pragma mark ---RealMenuItemIndex�擾

/*
OS API�Ǝ󂯓n���ۂ̃��j���[���ڂ̃C���f�b�N�X��RealMenuItemIndex�ƒ�`����B
�r���ɓ��I���j���[���ڂ������Ă���ƁAInterfaceBuilder�Őݒ肳�ꂽ�C���f�b�N�X(StaticIndex)�ƁA���ۂ̃��j���[ItemIndex�͈قȂ�B
���̂��߁AGetRealMenuItemIndex()���\�b�h�ŕϊ����s���K�v������B
*/

//MenuItemID����RealMenuItemIndex���擾
AIndex	AMenuManager::GetRealMenuItemIndex( const AMenuItemID inItemID ) const
{
	//�����Ŏw�肵�����j���[���ڂ�mMenuBarMenuItemArray�̃C���f�b�N�X���擾����
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( menuItemArrayIndex == kIndex_Invalid )   { _ACError("",this); return kIndex_Invalid; }
	return GetRealMenuItemIndexByMenuItemArrayIndex(menuItemArrayIndex);
}

//MenuBarMenuItemArray�̃C���f�b�N�X����RealMenuItemIndex���擾
AIndex	AMenuManager::GetRealMenuItemIndexByMenuItemArrayIndex( const AIndex inMenuItemArrayIndex ) const
{
	//"OnlyStatic"�Ȃ�StaticIndex��Ԃ�
	if( mMenuBarMenuItemArray_OnlyStatic.Get(inMenuItemArrayIndex) == true )//#1477   return mMenuBarMenuItemArray_MenuItemStaticIndex.Get(inMenuItemArrayIndex);
	{
		//macOS���Ŏ����I�Ƀ��j���[���ڂ��ǉ�����邱�Ƃ�����̂ŁA�ÓI���j���[�ɂ��ẮA���ʂɁANSMenu����AMenuItemID(=tag)���������� #1477
		AIndex	index = AMenuWrapper::GetMenuItemIndexByID(mMenuBarMenuItemArray_MenuRef.Get(inMenuItemArrayIndex), mMenuBarMenuItemArray_MenuItemID.Get(inMenuItemArrayIndex));
		if( index != kIndex_Invalid )
		{
			return index;
		}
		else
		{
			//NSMenu���猩����Ȃ������ꍇ�́A�]���ʂ�A�����f�[�^����擾����B
			return mMenuBarMenuItemArray_MenuItemStaticIndex.Get(inMenuItemArrayIndex);
		}
	}
	
	//MenuRef�������ŁAstaticIndex���Ώ�MenuItem��Index�������������̂��������A�Y��������̂ɂ��Ď��ۂ̃��j���[���ڐ��𑫂��Ă����B
	AIndex	realMenuItemIndex = 0;
	AMenuRef	targetMenuRef = mMenuBarMenuItemArray_MenuRef.Get(inMenuItemArrayIndex);
	AIndex	targetStaticIndex = mMenuBarMenuItemArray_MenuItemStaticIndex.Get(inMenuItemArrayIndex);
	//AItemCount	itemCount = mMenuBarMenuItemArray_MenuItemID.GetItemCount();
	AIndex	menuRefArrayIndex = mMenuBarMenuRefArray.Find(targetMenuRef);
	if( menuRefArrayIndex == kIndex_Invalid )   return inMenuItemArrayIndex;
	const AArray<AIndex>&	itemIndexArray = mMenuBarMenuRefArray_ItemIndexArray.GetObjectConst(menuRefArrayIndex);
	AItemCount	itemCount = itemIndexArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AIndex	index = itemIndexArray.Get(i);
		if( /* mMenuBarMenuItemArray_MenuRef.Get(i) == targetMenuRef && 
				mMenuBarMenuItemArray_MenuItemStaticIndex.Get(i) < targetStaticIndex*/
			mMenuBarMenuItemArray_MenuItemStaticIndex.Get(index) < targetStaticIndex )
		{
			AObjectID	dynamicMenuObjectID = mMenuBarMenuItemArray_DynamicMenuID.Get(index);
			ABool	isDynamicMenu = mMenuBarMenuItemArray_IsDynamicMenu.Get(index);
			if( isDynamicMenu == false )
			{
				//�Œ胁�j���[�̏ꍇ��+1
				realMenuItemIndex++;
			}
			else if( dynamicMenuObjectID == kObjectID_Invalid )
			{
				//�Ȃɂ����Ȃ�
			}
			else
			{
				//���I���j���[�̏ꍇ�́A���ݑ��݂��鍀�ڐ����𑫂�
				realMenuItemIndex += GetDynamicMenuConstByObjectID(dynamicMenuObjectID).GetMenuItemCount();
			}
		}
	}
	return realMenuItemIndex;
}

#pragma mark ===========================

#pragma mark ---���I���j���[�̐����^�폜�^�擾

/*
���I���j���[
���I���j���[�́A���[�g�ƁA�T�u���j���[����\�������B������MenuItemID�͑S�ē���ł���B
���I���j���[�̐������@�ɂ͂Q��ނ���
1. RegisterDynamicMenuItem()�@�i���Ƀ��j���[�o�[�ɑ��݂��郁�j���[���ڂ𓮓I���ڂƂ��ēo�^�j
�N�����_�ł́A���j���[�ɂ͂ЂƂ܂Ƃ܂�̓��I���j���[�����ɑ΂��Ĉ�̃��j���[���ڂ����݂��Ă���B
���̍��ڂɂ���RegisterDynamicMenuItem()���R�[������ƁAADynamicMenu�N���X����������AObjectID��Ԃ��B
���I���j���[�ɑ΂��鏈���͑S�Ă���ObjectID����čs���B
���R�A���[�g�̂݁B
2. CreateDynamicMenu()�@�i�V�K�ɐ����j
CreateDynamicMenu()�ɂ�蓮�I���j���[���ɐ����A�\��������A���ۂ̃��j���[�֎��̉�����B
���[�g�A�T�u���j���[�ǂ��炩�̏ꍇ�����蓾��B
���[�g�̏ꍇ�́ARealizeDynamicMenu()/UnrealizeDynamicMenu()�Ŏ��̉��^����̉�����B

*/

//���j���[�o�[�Ɋ��ɑ��݂��郁�j���[���ڂ𓮓I���j���[�Ƃ��ēo�^����i�E�C���h�E���j���[���j
AObjectID	AMenuManager::RegisterDynamicMenuItem( const AMenuItemID inItemID )
{
	//�����Ŏw�肵�����j���[���ڂ�mMenuBarMenuItemArray�̃C���f�b�N�X���擾����
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   { _ACError("",this); return kObjectID_Invalid; }
	
	//���I���j���[�I�u�W�F�N�g����
	AObjectID	dynamicMenuObjectID = CreateDynamicMenu(inItemID,true);
	//���I���j���[��ObjectID��mMenuBarMenuItemArray�֕ۑ�
	mMenuBarMenuItemArray_DynamicMenuID.Set(index,dynamicMenuObjectID);
	mMenuBarMenuItemArray_IsDynamicMenu.Set(index,true);
	
	//�������j���[���̃��j���[���ڂ�"OnlyStatic"�t���O��false�ɂ���
	AMenuRef	menuRef = mMenuBarMenuItemArray_MenuRef.Get(index);
	for( AIndex i = 0; i < 	mMenuBarMenuItemArray_MenuItemID.GetItemCount(); i++ )
	{
		if( mMenuBarMenuItemArray_MenuRef.Get(i) == menuRef )
		{
			mMenuBarMenuItemArray_OnlyStatic.Set(i,false);
		}
	}
	
	//���I���j���[���̉�
	GetDynamicMenuByObjectID(dynamicMenuObjectID).Realize();
	return dynamicMenuObjectID;
}

//���j���[�o�[�ɂ܂����݂��Ă��Ȃ����j���[���ڂ𓮓I���j���[�Ƃ��ēo�^����
void	AMenuManager::ReserveDynamicMenuItem( const AMenuItemID inItemID )
{
	//�����Ŏw�肵�����j���[���ڂ�mMenuBarMenuItemArray�̃C���f�b�N�X���擾����
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   { _ACError("",this); return; }
	
	//
	mMenuBarMenuItemArray_IsDynamicMenu.Set(index,true);
	
	//�������j���[���̃��j���[���ڂ�"OnlyStatic"�t���O��false�ɂ���
	AMenuRef	menuRef = mMenuBarMenuItemArray_MenuRef.Get(index);
	for( AIndex i = 0; i < 	mMenuBarMenuItemArray_MenuItemID.GetItemCount(); i++ )
	{
		if( mMenuBarMenuItemArray_MenuRef.Get(i) == menuRef )
		{
			mMenuBarMenuItemArray_OnlyStatic.Set(i,false);
		}
	}
	
	//���̃��j���[��ɑ��݂��铯��MenuItemID�̃��j���[���ڂ��폜����
	for( AIndex i = 0; i < AMenuWrapper::GetMenuItemCount(menuRef); )
	{
		if( AMenuWrapper::GetMenuItemID(menuRef,i) == inItemID )
		{
			AMenuWrapper::DeleteMenuItem(menuRef,i);
		}
		else
		{
			i++;
		}
	}
}

//���I���j���[�I�u�W�F�N�g����
AObjectID	AMenuManager::CreateDynamicMenu( const AMenuItemID inItemID, const ABool inIsRoot )
{
	AMenuRef	menuRef = NULL;
	if( inIsRoot == true )
	{
		AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
		if( index == kIndex_Invalid )   {_ACThrow("invalid MenuItemID",this);}
		menuRef = mMenuBarMenuItemArray_MenuRef.Get(index);
	}
	AStMutexLocker	mutexlocker(mDynamicMenuArrayMutex);
	ADynamicMenuFactory	factory(this,*this,inItemID,menuRef);
	AIndex	index = mDynamicMenuArray.AddNewObject(factory);
	return mDynamicMenuArray.GetObject(index).GetObjectID();
}

//���I���j���[�I�u�W�F�N�g�폜
void	AMenuManager::DeleteDynamicMenuByObjectID( const AObjectID inDynamicMenuObjectID )
{
	AStMutexLocker	mutexlocker(mDynamicMenuArrayMutex);
	mDynamicMenuArray.Delete1Object(mDynamicMenuArray.GetIndexByID(inDynamicMenuObjectID));
}

//���I���j���[�I�u�W�F�N�g�擾
ADynamicMenu&	AMenuManager::GetDynamicMenuByObjectID( const AObjectID inDynamicMenuObjectID )
{
	AStMutexLocker	mutexlocker(mDynamicMenuArrayMutex);
	/*#272
	AIndex	dynamicMenuObjectIndex = mDynamicMenuArray.GetIndexByID(inDynamicMenuObjectID);
	if( dynamicMenuObjectIndex == kIndex_Invalid )   _ACThrow("dynamic menu not found",this);
	return mDynamicMenuArray.GetObject(dynamicMenuObjectIndex);
	*/
	return mDynamicMenuArray.GetObjectByID(inDynamicMenuObjectID);
}
ADynamicMenu&	AMenuManager::GetDynamicMenuConstByObjectID( const AObjectID inDynamicMenuObjectID ) const
{
	AStMutexLocker	mutexlocker(mDynamicMenuArrayMutex);
	/*#272
	AIndex	dynamicMenuObjectIndex = mDynamicMenuArray.GetIndexByID(inDynamicMenuObjectID);
	if( dynamicMenuObjectIndex == kIndex_Invalid )   _ACThrow("dynamic menu not found",this);
	return mDynamicMenuArray.GetObjectConst(dynamicMenuObjectIndex);
	*/
	return mDynamicMenuArray.GetObjectConstByID(inDynamicMenuObjectID);
}
ADynamicMenu&	AMenuManager::GetDynamicMenuByMenuItemID( const AMenuItemID inMenuItemID )
{
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(inMenuItemID);
	if( menuItemArrayIndex == kIndex_Invalid )   _ACThrow("menu menu not found",this);
	return GetDynamicMenuByObjectID(mMenuBarMenuItemArray_DynamicMenuID.Get(menuItemArrayIndex));
}
ADynamicMenu&	AMenuManager::GetDynamicMenuConstByMenuItemID( const AMenuItemID inMenuItemID ) const
{
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(inMenuItemID);
	if( menuItemArrayIndex == kIndex_Invalid )   _ACThrow("menu menu not found",this);
	return GetDynamicMenuConstByObjectID(mMenuBarMenuItemArray_DynamicMenuID.Get(menuItemArrayIndex));
}

//MenuItemID���瓮�I���j���[ObjectID���擾
AObjectID	AMenuManager::GetDynamicMenuObjectIDByMenuItemID( const AMenuItemID inMenuItemID )
{
	return GetDynamicMenuConstByMenuItemID(inMenuItemID).GetObjectID();
}

//���I���j���[���ڐ��擾
AItemCount	AMenuManager::GetDynamicMenuItemCountByObjectID( const AObjectID inDynamicMenuObjectID ) const
{
	return GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).GetMenuItemCount();
}
AItemCount	AMenuManager::GetDynamicMenuItemCountByMenuItemID( const AMenuItemID inMenuItemID ) const
{
	return GetDynamicMenuConstByMenuItemID(inMenuItemID).GetMenuItemCount();
}

//���I���j���[���̉��ς݂��ǂ������擾
ABool	AMenuManager::GetDynamicMenuRealizedByObjectID( const AObjectID inDynamicMenuObjectID ) const
{
	return GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).IsRealized();
}

//���I���j���[���̉�
void	AMenuManager::RealizeDynamicMenu( const AObjectID inDynamicMenuObjectID )
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).Realize();
	AMenuItemID	menuItemID = GetDynamicMenuByObjectID(inDynamicMenuObjectID).GetMenuItemID();
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(menuItemID);
	mMenuBarMenuItemArray_DynamicMenuID.Set(menuItemArrayIndex,inDynamicMenuObjectID);
	
	//�������j���[���̃��j���[���ڂ�"OnlyStatic"�t���O��false�ɂ���
	AMenuRef	menuRef = mMenuBarMenuItemArray_MenuRef.Get(menuItemArrayIndex);
	for( AIndex i = 0; i < 	mMenuBarMenuItemArray_MenuItemID.GetItemCount(); i++ )
	{
		if( mMenuBarMenuItemArray_MenuRef.Get(i) == menuRef )
		{
			mMenuBarMenuItemArray_OnlyStatic.Set(i,false);
		}
	}
	
}

//���I���j���[���̉�����
void	AMenuManager::UnrealizeDynamicMenu( const AObjectID inDynamicMenuObjectID )
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).Unrealize();
	AMenuItemID	menuItemID = GetDynamicMenuByObjectID(inDynamicMenuObjectID).GetMenuItemID();
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(menuItemID);
	mMenuBarMenuItemArray_DynamicMenuID.Set(menuItemArrayIndex,kObjectID_Invalid);
}

//#129
/**
Enable/Disable�X�V
*/
void	AMenuManager::UpdateDynamicMenuEnableMenuItem( const AMenuItemID inMenuItemID )
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inMenuItemID);
	if( index != kIndex_Invalid )
	{
		if( mMenuBarMenuItemArray_DynamicMenuID.Get(index) != kObjectID_Invalid )
		{
			GetDynamicMenuByMenuItemID(inMenuItemID).UpdateEnableItems();
		}
	}
}

//#688
/**
DynamicMenu�̊emenu item��Enable/Disable��Ԃ��擾
*/
ABool	AMenuManager::GetDynamicMenuEnableMenuItem( const AMenuItemID inMenuItemID, const AIndex inMenuItemIndex, ABool& isEnabled ) const
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inMenuItemID);
	if( index != kIndex_Invalid )
	{
		if( mMenuBarMenuItemArray_DynamicMenuID.Get(index) != kObjectID_Invalid )
		{
			GetDynamicMenuConstByMenuItemID(inMenuItemID).GetDynamicMenuEnableMenuItem(inMenuItemIndex,isEnabled);
			return true;
		}
	}
	return false;
}

//���I���j���[���t�H���g���j���[�Ƃ��Đݒ肷��
void	AMenuManager::SetDynamicMenuAsFontMenu( const AMenuItemID inItemID, const ABool inEnableDefaultFontItem )//#375
{
	GetDynamicMenuByMenuItemID(inItemID).SetAsFontMenu(inEnableDefaultFontItem);//#375
}

#pragma mark ===========================

#pragma mark ---���I���j���[�̍��ڒǉ��^�폜�^�擾

//���I���j���[���ڒǉ�
void	AMenuManager::AddDynamicMenuItemByMenuItemID( const AMenuItemID inItemID, const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
		const ABool inEnabled )//#129
{
	GetDynamicMenuByMenuItemID(inItemID).AddMenuItem(inMenuText,inActionText,inSubMenuID,inShortcutKey,inShortcutModifierKeys,inSeparator,inEnabled);
}
void	AMenuManager::AddDynamicMenuItemByObjectID( const AObjectID inDynamicMenuObjectID, const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
		const ABool inEnabled )//#129
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).AddMenuItem(inMenuText,inActionText,inSubMenuID,inShortcutKey,inShortcutModifierKeys,inSeparator,inEnabled);
}

//���I���j���[���ڒǉ�
void	AMenuManager::InsertDynamicMenuItemByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, 
		const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
		const ABool inEnabled )//#129
{
	GetDynamicMenuByMenuItemID(inItemID).InsertMenuItem(inItemIndex,inMenuText,inActionText,inSubMenuID,inShortcutKey,inShortcutModifierKeys,inSeparator,inEnabled);//#129
}
void	AMenuManager::InsertDynamicMenuItemByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, 
		const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
		const ABool inEnabled )//#129
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).InsertMenuItem(inItemIndex,inMenuText,inActionText,inSubMenuID,inShortcutKey,inShortcutModifierKeys,inSeparator,inEnabled);//#129
}

//���I���j���[���ڍ폜
void	AMenuManager::DeleteDynamicMenuItemByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex )
{
	GetDynamicMenuByMenuItemID(inItemID).DeleteMenuItem(inItemIndex);
}
void	AMenuManager::DeleteDynamicMenuItemByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex )
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).DeleteMenuItem(inItemIndex);
}

//���I���j���[���ڍ폜�i�S�폜�j
void	AMenuManager::DeleteAllDynamicMenuItemsByMenuItemID( const AMenuItemID inItemID )
{
	GetDynamicMenuByMenuItemID(inItemID).DeleteAllMenuItems();
}
void	AMenuManager::DeleteAllDynamicMenuItemsByObjectID( const AObjectID inDynamicMenuObjectID )
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).DeleteAllMenuItems();
}

//���I���j���[���ڈړ� R0173
void	AMenuManager::MoveDynamicMenuItemByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inOldIndex, const AIndex inNewIndex )
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).MoveMenuItem(inOldIndex,inNewIndex);
}

//���I���j���[���ڐݒ�iTextArray����j
void	AMenuManager::SetDynamicMenuItemByMenuItemID( const AMenuItemID inItemID, const ATextArray& inMenuTextArray, const ATextArray& inActionTextArray )
{
	GetDynamicMenuByMenuItemID(inItemID).SetMenuItem(inMenuTextArray,inActionTextArray);
}

//���I���j���[���ڂ�ActionText�擾�^�ݒ�i���I���j���[ObjectID, itemindex�ł̎w��j
void	AMenuManager::GetDynamicMenuItemActionTextByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, AText& outText ) const
{
	GetDynamicMenuConstByMenuItemID(inItemID).GetActionTextByMenuItemArrayIndex(inItemIndex,outText);
}
void	AMenuManager::GetDynamicMenuItemActionTextByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, AText& outText ) const
{
	GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).GetActionTextByMenuItemArrayIndex(inItemIndex,outText);
}
void	AMenuManager::SetDynamicMenuItemActionTextByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, const AText& inText )
{
	GetDynamicMenuByMenuItemID(inItemID).SetActionTextByMenuItemArrayIndex(inItemIndex,inText);
}
void	AMenuManager::SetDynamicMenuItemActionTextByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, const AText& inText )
{
	GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).SetActionTextByMenuItemArrayIndex(inItemIndex,inText);
}

//���I���j���[���ڂ̃e�L�X�g�擾�^�ݒ�i���I���j���[ObjectID, itemindex�ł̎w��j
void	AMenuManager::GetDynamicMenuItemMenuTextByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, AText& outText ) const
{
	GetDynamicMenuConstByMenuItemID(inItemID).GetMenuTextByMenuItemArrayIndex(inItemIndex,outText);
}
void	AMenuManager::GetDynamicMenuItemMenuTextByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, AText& outText ) const
{
	GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).GetMenuTextByMenuItemArrayIndex(inItemIndex,outText);
}
void	AMenuManager::SetDynamicMenuItemMenuTextByMenuItemID(  const AMenuItemID inItemID, const AIndex inItemIndex, const AText& inText )
{
	GetDynamicMenuByMenuItemID(inItemID).SetMenuTextByMenuItemArrayIndex(inItemIndex,inText);
}
void	AMenuManager::SetDynamicMenuItemMenuTextByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, const AText& inText )
{
	GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).SetMenuTextByMenuItemArrayIndex(inItemIndex,inText);
}

//���I���j���[���ڂ̃V���[�g�J�b�g�擾�^�ݒ�i���I���j���[ObjectID, itemindex�ł̎w��j
void	AMenuManager::GetDynamicMenuItemShortcutByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, 
		ANumber& outShortcut, AMenuShortcutModifierKeys& outModifiers ) const
{
	GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).GetShortcutByMenuItemArrayIndex(inItemIndex,outShortcut,outModifiers);
}
void	AMenuManager::SetDynamicMenuItemShortcutByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, 
			const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers ) const
{
	GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).SetShortcutByMenuItemArrayIndex(inItemIndex,inShortcut,inModifiers);
}
void	AMenuManager::SetDynamicMenuItemShortcutByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, 
															 const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers ) const
{
	GetDynamicMenuConstByMenuItemID(inItemID).SetShortcutByMenuItemArrayIndex(inItemIndex,inShortcut,inModifiers);
}

//���I���j���[���ڂ̃T�u���j���[ObjectID�擾�^�ݒ�
AObjectID	AMenuManager::GetDynamicMenuItemSubMenuObjectIDByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex ) const
{
	return GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).GetSubMenuObjectIDByMenuItemArrayIndex(inItemIndex);
}
void	AMenuManager::SetDynamicMenuSubMenu( const AMenuItemID inItemID, const AIndex inItemIndex, const AObjectID inSubMenuObjectID )
{
	GetDynamicMenuByMenuItemID(inItemID).SetSubMenu(inItemIndex,inSubMenuObjectID);
}

//B0308
//���I���j���[���ڂ̃`�F�b�N�}�[�N�ݒ�
void	AMenuManager::SetCheckMarkToDynamicMenu( const AMenuItemID inItemID, const AIndex inIndex )
{
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( menuItemArrayIndex == kIndex_Invalid )   return;
	AObjectID	dynamicMenuObjectID = mMenuBarMenuItemArray_DynamicMenuID.Get(menuItemArrayIndex);
	if( dynamicMenuObjectID == kObjectID_Invalid )   return;
	GetDynamicMenuByObjectID(dynamicMenuObjectID).SetCheckMark(inIndex);
}
//�iActionText����ݒ�j
void	AMenuManager::SetCheckMarkToDynamicMenuWithActionText( const AMenuItemID inItemID, const AText& inActionText )
{
	GetDynamicMenuByMenuItemID(inItemID).SetCheckMarkWithActionText(inActionText);
}

//#695
/**
ActionText����A�Ή����郁�j���[���ڂ̃C���f�b�N�X����������
*/
AIndex	AMenuManager::FindDynamicMenuItemIndexFromActionText( const AObjectID inDynamicMenuObjectID, const AText& inActionText ) const
{
	return GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).FindMenuItemIndexFromActionText(inActionText);
}

#pragma mark ===========================

#pragma mark ---Font���j���[

//�t�H���g���j���[�Ƃ��Đݒ�
void	AMenuManager::RegisterFontMenu( const AMenuRef inMenuRef /*win , const AControlRef inControlRef*/ )
{
//��	AMenuWrapper::DeleteAllMenuItems(inMenuRef);
//��	AMenuWrapper::InsertFontMenu(inMenuRef,0,0);
	//win �iCWindowImp�Ŏ��s�j::SetControl32BitMaximum(inControlRef,AMenuWrapper::GetMenuItemCount(inMenuRef));
}

#pragma mark ===========================

#pragma mark ---�R���e�N�X�g���j���[

/**
�R���e�N�X�g���j���[��o�^����
*/
void	AMenuManager::RegisterContextMenu( AConstCharPtr inMenuName, const AContextMenuID inContextMenuID )
{
	//
	AMenuRef	menuRef = AMenuWrapper::RegisterContextMenu(inMenuName);
	//
	mContextMenuArray_MenuRef.Add(menuRef);
	mContextMenuArray_ID.Add(inContextMenuID);
	mContextMenuArray_ActionText.AddNewObject();
}

//#0
/**
�R���e�N�X�g���j���[��o�^����i���\�[�X�����j
*/
void	AMenuManager::RegisterContextMenu( const AContextMenuID inContextMenuID )
{
	//
	AMenuRef	menuRef = AMenuWrapper::CreateMenu();
	//
	mContextMenuArray_MenuRef.Add(menuRef);
	mContextMenuArray_ID.Add(inContextMenuID);
	mContextMenuArray_ActionText.AddNewObject();
}

/*#688 �R���e�L�X�g���j���[��CUserPane�ŕ\������悤�ɕύX
//�R���e�N�X�g���j���[�\��
void	AMenuManager::ShowContextMenu( const AContextMenuID inContextMenuID, const AGlobalPoint& inMousePoint, const AWindowRef inWindowRef )//win 080712
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	AMenuWrapper::ShowContextMenu(menuRef,inMousePoint,inWindowRef);//win 080712
}
*/

//#688
/**
�R���e�N�X�g���j���[�擾
*/
AMenuRef	AMenuManager::GetContextMenu( const AContextMenuID inContextMenuID ) const
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return NULL;}
	return mContextMenuArray_MenuRef.Get(index);
}

//#232
/**
*/
AItemCount	AMenuManager::GetContextMenuItemCount( const AContextMenuID inContextMenuID ) const
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return 0;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	return AMenuWrapper::GetMenuItemCount(menuRef);
}

//�R���e�N�X�g���j���[�e�L�X�g�ݒ�
void	AMenuManager::SetContextMenuItemText( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, const AText& inText )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	AMenuWrapper::SetMenuItemText(menuRef,inMenuItemIndex,inText);
}

//#232
/**
�R���e�N�X�g���j���[�e�L�X�g�擾
*/
void	AMenuManager::GetContextMenuItemText( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, AText& outText ) const
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	AMenuWrapper::GetMenuItemText(menuRef,inMenuItemIndex,outText);
}

//#232
/**
*/
void	AMenuManager::SetContextMenuEnableMenuItem( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, const ABool inEnable )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	AMenuWrapper::SetEnableMenuItem(menuRef,inMenuItemIndex,inEnable);
}

/**
���I���j���[�ɂȂ����Ă���T�u���j���[���A�R���e�L�X�g���j���[���ڂ̃T�u���j���[�ɂ���
@param outSubMenuArray �T�u���j���[��menu ref�i�[array�i���̊֐��ɂ��P�ǉ������j
@param outOldParentMenuArray ���̐emenu ref�i�[array�i���̊֐��ɂ��P�ǉ������j
@param outOldParentItemIndexArray ���̐emenu item index�i���̊֐��ɂ��P�ǉ������j
*/
void	AMenuManager::SetContextMenuItemSubMenuFromDynamicMenu( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, 
		const AObjectID inSubDynamicMenuObjectID,
		AArray<AMenuRef>& outSubMenuArray, //#688
		AArray<AMenuRef>& outOldParentMenuArray, AArray<AIndex>& outOldParentItemIndexArray )//#688
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	
	//�R���e�L�X�g���j���[��menu ref�擾
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	//�T�u���j���[menu ref�擾
	AMenuRef	subMenuRef = GetDynamicMenuByObjectID(inSubDynamicMenuObjectID).GetMenuRef();
	//���̐e���j���[����T�u���j���[��detach���Amenuref, menu item index���擾���A�L������ #688
	AMenuRef	oldParentMenu = NULL;
	AIndex	oldParentItemIndex = kIndex_Invalid;
	//���̐e���j���[����؂藣��
	AMenuWrapper::DetachSubMenu(subMenuRef,oldParentMenu,oldParentItemIndex);
	//�Ԃ�l�ɋL��
	outSubMenuArray.Add(subMenuRef);
	outOldParentMenuArray.Add(oldParentMenu);
	outOldParentItemIndexArray.Add(oldParentItemIndex);
	//�R���e�L�X�g���j���[�ɃT�u���j���[�ݒ�
	AMenuWrapper::SetSubMenu(menuRef,inMenuItemIndex,subMenuRef);
}

//#232
/**
ContextMenu��TextArray��ݒ�
*/
void	AMenuManager::SetContextMenuTextArray( const AContextMenuID inContextMenuID, const ATextArray& inTextArray, const ATextArray& inActionTextArray )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	AMenuWrapper::SetMenuItemsFromTextArray(menuRef,inTextArray);
	mContextMenuArray_ActionText.GetObject(index).SetFromTextArray(inActionTextArray);
}

//#232
/**
ContextMenu��TextArrayMenu��ݒ�
*/
void	AMenuManager::SetContextMenuTextArrayMenu( const AContextMenuID inContextMenuID, const ATextArrayMenuID inTextArrayMenuID )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().RegisterMenu(mContextMenuArray_MenuRef.Get(index),inTextArrayMenuID);
}

//#232
/**
ContextMenu��MenuItemID��ݒ�
*/
void	AMenuManager::SetContextMenuMenuItemID( const AContextMenuID inContextMenuID, const AMenuItemID inMenuItemID )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	
	//MenuItemID��S�Ẵ��j���[���ڂɐݒ聨���j���[�I������EVTDO_DoMenuItemSelected()�ɑ΂��A�w��MenuItemID��ActionText���`������
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	for( AIndex i = 0; i < AMenuWrapper::GetMenuItemCount(menuRef); i++ )
	{
		AMenuWrapper::SetMenuItemID(menuRef,i,inMenuItemID);
	}
}

//#232
/**
*/
void	AMenuManager::SetContextMenuCheckMark( const AContextMenuID inContextMenuID, const AText& inText )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	for( AIndex i = 0; i < AMenuWrapper::GetMenuItemCount(menuRef); i++ )
	{
		AText	text;
		AMenuWrapper::GetMenuItemText(menuRef,i,text);
		if( text.Compare(inText) == true )
		{
			AMenuWrapper::SetCheckMark(menuRef,i,true);
		}
		else
		{
			AMenuWrapper::SetCheckMark(menuRef,i,false);
		}
	}
}

//win
/**
���j���[�A�C�R���ݒ�
*/
void	AMenuManager::SetContextMenuItemIcon( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, const AIconID inIconID )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	AMenuWrapper::SetMenuItemIcon(menuRef,inMenuItemIndex,inIconID);
}

