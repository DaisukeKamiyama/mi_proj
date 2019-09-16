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

AMenuWrapper

*/

#include "AMenuWrapper.h"
#include "../Frame.h"
#import <Cocoa/Cocoa.h>
#include "../AbsBase/Cocoa.h"
#import "CocoaContextualMenuController.h"

#pragma mark ===========================

#pragma mark ---�S��

/**
���j���[�������i���j���[�o�[�����j
*/
void	AMenuWrapper::InitMenu()
{
}

/**
���[�g���j���[�擾
*/
AMenuRef	AMenuWrapper::GetRootMenu()
{
	return [NSApp mainMenu];
}

#pragma mark ===========================

#pragma mark ---���j���[

/**
���j���[����
*/
AMenuRef	AMenuWrapper::CreateMenu()
{
	NSMenu*	menu = [[NSMenu alloc] initWithTitle:@""];
	//�f�t�H���g��auto enable��NO�ɂ���
	[menu setAutoenablesItems:NO];
	return menu;
}

/**
���j���[�폜
*/
void	AMenuWrapper::DeleteMenu( const AMenuRef inMenuRef )
{
	//���j���[���
	[ACocoa::GetNSMenu(inMenuRef) release];
}

/**
���j���[���ڐ��擾
*/
AItemCount	AMenuWrapper::GetMenuItemCount( const AMenuRef inMenuRef )
{
	return [ACocoa::GetNSMenu(inMenuRef) numberOfItems];
}

/**
���j���[�^�C�g���e�L�X�g�擾
*/
void	AMenuWrapper::GetMenuTitleText( const AMenuRef inMenuRef, AText& outText )
{
	ACocoa::SetTextFromNSString([ACocoa::GetNSMenu(inMenuRef) title],outText);
}

/**
Autoenable�ݒ�
*/
void	AMenuWrapper::EnableAutoEnableMenu( const AMenuRef inMenuRef )
{
	[ACocoa::GetNSMenu(inMenuRef) setAutoenablesItems:YES];
}

#pragma mark ===========================

#pragma mark ---���j���[���ځ@�ǉ��E�폜

#pragma mark ===========================

#pragma mark ---���j���[���ځ@���ݒ�E�擾

/**
�T�u���j���[�擾
*/
AMenuRef	AMenuWrapper::GetSubMenu( const AMenuRef inMenuRef, const AIndex inItemIndex )
{
	return [ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) submenu];
}

/**
�T�u���j���[�ݒ�
*/
void	AMenuWrapper::SetSubMenu( const AMenuRef inMenuRef, const AIndex inItemIndex, const AMenuRef inSubMenuRef )
{
	//�w�胁�j���[���ڂ̃T�u���j���[��ݒ�
	if( inSubMenuRef == NULL )
	{
		[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setSubmenu:nil];
	}
	else
	{
		[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setSubmenu:(ACocoa::GetNSMenu(inSubMenuRef))];
	}
}

/**
�T�u���j���[detach
@note Cocoa�ł͐e���j���[�𕡐��ݒ�o���Ȃ��̂ŁA���̃��j���[�̃T�u���j���[�ɂ���Ƃ��ɂ́A��ɂ��̊֐���detach���Ă���A�T�u���j���[�ɂ���B
*/
void	AMenuWrapper::DetachSubMenu( const AMenuRef inSubMenuRef, AMenuRef& outOldParentMenuRef, AIndex& outOldParentMenuItemIndex )
{
	//���ʏ�����
	outOldParentMenuRef = NULL;
	outOldParentMenuItemIndex = kIndex_Invalid;
	//detach����T�u���j���[�ƁA���̐e���擾
	NSMenu*	subMenu = ACocoa::GetNSMenu(inSubMenuRef);
	NSMenu*	oldParentMenu = [subMenu supermenu];
	if( oldParentMenu != nil )
	{
		//�e���j���[�A����сAitem index���L��
		outOldParentMenuRef = static_cast<AMenuRef>(oldParentMenu);
		outOldParentMenuItemIndex = [oldParentMenu indexOfItemWithSubmenu:subMenu];
		//detach�i�T�u���j���[�����j
		[[oldParentMenu itemAtIndex:outOldParentMenuItemIndex] setSubmenu:nil];
	}
}

/**
MenuItemID�擾
*/
AMenuItemID	AMenuWrapper::GetMenuItemID( const AMenuRef inMenuRef, const AIndex inItemIndex )
{
	return [ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) tag];
}

/**
MenuItemID�ݒ�
*/
void	AMenuWrapper::SetMenuItemID( const AMenuRef inMenuRef, const AIndex inItemIndex, const AMenuItemID inMenuItemID )
{
	[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setTag:inMenuItemID];
	
	//target/action�ݒ�
	//item id��0, -1�ȊO�̏ꍇ�̂ݐݒ肷��B�ipopup button �̏ꍇ�Apopup button �̕���action�����s�����������A
	//menuitem��action��ݒ肷��ƁApopup button��action�����s����Ȃ����߁B�j
	if( inMenuItemID != 0 && inMenuItemID != kMenuItemID_Invalid )
	{
		[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setTarget:[NSApp delegate]];
		[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setAction:@selector(doMenuActions:)];
	}
}

/**
���j���[���ڃe�L�X�g�擾
*/
void	AMenuWrapper::GetMenuItemText( const AMenuRef inMenuRef, const AIndex inItemIndex, AText& outText )
{
	ACocoa::SetTextFromNSString([ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) title],outText);
}

/**
���j���[���ڃe�L�X�g�ݒ�
*/
void	AMenuWrapper::SetMenuItemText( const AMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText )
{
	AStCreateNSStringFromAText	str(inText);
	[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setTitle:str.GetNSString()];
}

/**
���j���[����Enable/Disable
*/
void	AMenuWrapper::SetEnableMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex, const ABool inEnable )
{
	NSMenuItem*	menuItem = ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex);
	if( inEnable == true )
	{
		[menuItem setEnabled:YES];
	}
	else
	{
		[menuItem setEnabled:NO];
	}
}

/**
���j���[����Enable/Disable
*/
/*
void	AMenuWrapper::SetEnableMenuItemAll( const AMenuRef inMenuRef, const ABool inEnable )
{
	NSMenu*	menu = ACocoa::GetNSMenu(inMenuRef);
	AItemCount	menuItemCount = [menu numberOfItems];
	BOOL	enable = YES;
	if( inEnable == false )
	{
		enable = NO;
	}
	for( AIndex i = 0; i < menuItemCount; i++ )
	{
		[[menu itemAtIndex:i] setEnabled:enable];
	}
}
*/
/**
���j���[���ڒǉ��i�}���j
*/
void	AMenuWrapper::InsertMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText, 
		const AMenuItemID inItemID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	//���ڑ}��
	if( inText.Compare("-") == false )
	{
		//#1004
		//���j���[���ڂɒ����e�L�X�g��ݒ肷��Ə������d���Ȃ�̂ŁA��������B
		AText	text;
		text.SetText(inText);
		text.LimitLength(0,kMenuTextMax);
		//
		AStCreateNSStringFromAText	str(text);
		NSMenuItem*	menuItem = [ACocoa::GetNSMenu(inMenuRef) insertItemWithTitle:str.GetNSString() 
				action:nil/*@selector(doMenuActions:)*/ keyEquivalent:@"" atIndex:inItemIndex];
		//tag�ݒ�
		[menuItem setTag:inItemID];
		//key equivalent�ݒ�
		if( inShortcutKey != 0 )
		{
			AMenuWrapper::SetShortcut(inMenuRef,inItemIndex,inShortcutKey,inShortcutModifierKeys);
		}
		//target/action�ݒ�
		//item id��0, -1�ȊO�̏ꍇ�̂ݐݒ肷��B�ipopup button �̏ꍇ�Apopup button �̕���action�����s�����������A
		//menuitem��action��ݒ肷��ƁApopup button��action�����s����Ȃ����߁B�j
		if( inItemID != 0 && inItemID != kMenuItemID_Invalid )
		{
			[menuItem setTarget:[NSApp delegate]];
			[menuItem setAction:@selector(doMenuActions:)];
		}
		//enable�ɐݒ�
		[menuItem setEnabled:YES];
	}
	else
	{
		[ACocoa::GetNSMenu(inMenuRef) insertItem:[NSMenuItem separatorItem] atIndex:inItemIndex];
		//�Z�p���[�^�ɂ�MenuItemID��ݒ肷��B�iADynamicMenu::Realize()/Unrealize()�ɂāA�I���ʒu��MenuItemID�Ŕ��肵�Ă���̂ŁA
		//�Z�p���[�^�ɂ�MenuItemID��ݒ肷��K�v������B�j
		[[ACocoa::GetNSMenu(inMenuRef) itemAtIndex:inItemIndex] setTag:inItemID];
	}
}

/**
���j���[���ڒǉ�
*/
void	AMenuWrapper::AddMenuItem( const AMenuRef inMenuRef, const AText& inText, 
		const AMenuItemID inItemID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	AMenuWrapper::InsertMenuItem(inMenuRef,AMenuWrapper::GetMenuItemCount(inMenuRef),
				inText,inItemID,inShortcutKey,inShortcutModifierKeys);
}

/**
���j���[���ځi�Z�p���[�^�j�ǉ��i�}���j
*/
void	AMenuWrapper::InsertMenuItem_Separator( const AMenuRef inMenuRef, const AIndex inItemIndex,
		const AMenuItemID inItemID )//#427
{
	AText	text("-");
	AMenuWrapper::InsertMenuItem(inMenuRef,inItemIndex,text,inItemID,0,0);
}

/**
���j���[���ځi�Z�p���[�^�j�ǉ�
*/
void	AMenuWrapper::AddMenuItem_Separator( const AMenuRef inMenuRef, const AMenuItemID inItemID )//#427
{
	AMenuWrapper::InsertMenuItem_Separator(inMenuRef,AMenuWrapper::GetMenuItemCount(inMenuRef),inItemID);//#427
}

/**
���j���[���ڍ폜
@note �T�u���j���[��detach����A�폜�͂���Ȃ��B
*/
void	AMenuWrapper::DeleteMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex )
{
	//�T�u���j���[detach
	AMenuRef	subMenu = GetSubMenu(inMenuRef,inItemIndex);
	if( subMenu != nil )
	{
		AMenuRef	oldParentMenuRef = nil;
		AIndex	oldParentMenuItemIndex = kIndex_Invalid;
		DetachSubMenu(subMenu,oldParentMenuRef,oldParentMenuItemIndex);
	}
	//���j���[���ڍ폜
	[ACocoa::GetNSMenu(inMenuRef) removeItemAtIndex:inItemIndex];
}

/**
���j���[���ڑS�폜
*/
void	AMenuWrapper::DeleteAllMenuItems( const AMenuRef inMenuRef )
{
	while( AMenuWrapper::GetMenuItemCount(inMenuRef) > 0 )
	{
		AMenuWrapper::DeleteMenuItem(inMenuRef,0);
	}
}

/**
TextArray���烁�j���[���ڒǉ�
*/
void	AMenuWrapper::AddMenuItemsFromTextArray( const AMenuRef inMenuRef, const ATextArray& inTextArray )
{
	for( AIndex index = 0; index < inTextArray.GetItemCount(); index++ )
	{
		AText	text;
		inTextArray.Get(index,text);
		AMenuWrapper::InsertMenuItem(inMenuRef,AMenuWrapper::GetMenuItemCount(inMenuRef),text,0,0,0);
	}
}

/**
TextArray���烁�j���[���ڐݒ�
*/
void	AMenuWrapper::SetMenuItemsFromTextArray( const AMenuRef inMenuRef, const ATextArray& inTextArray )
{
	AMenuWrapper::DeleteAllMenuItems(inMenuRef);
	AMenuWrapper::AddMenuItemsFromTextArray(inMenuRef,inTextArray);
}

/**
���j���[���ڂɃ`�F�b�N�}�[�N�ݒ�
*/
void	AMenuWrapper::SetCheckMark( const AMenuRef inMenuRef, const AIndex inItemIndex, const ABool inCheck )
{
	if( inCheck == true )
	{
		[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setState:NSOnState];
	}
	else
	{
		[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setState:NSOffState];
	}
}

/**
���j���[���ڂɃV���[�g�J�b�g�ݒ�
*/
void	AMenuWrapper::SetShortcut( const AMenuRef inMenuRef, const AIndex inItemIndex, 
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	//Key equivalent�ݒ�
	//setKeyEquivalent�͑啶���̏ꍇ�����I��shift�L��Ƃ݂Ȃ��̂ŁA�������ɕϊ�
	ANumber	shortcutkey = inShortcutKey;
	if( shortcutkey >= 'A' && shortcutkey <= 'Z' )
	{
		shortcutkey += 'a'-'A';
	}
	//Key equivalent��NSString���擾
	AText	keyEqText;
	keyEqText.InsertFromUCS4Char(0,static_cast<AUCS4Char>(shortcutkey));
	AStCreateNSStringFromAText	keyeqstr(keyEqText);
	[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setKeyEquivalent:keyeqstr.GetNSString()];
	//Mask�ݒ�
	NSUInteger	mask = NSCommandKeyMask;
	if( (inShortcutModifierKeys & kModifierKeysMask_Control) != 0 )
	{
		mask |= NSControlKeyMask;
	}
	if( (inShortcutModifierKeys & kModifierKeysMask_Option) != 0 )
	{
		mask |= NSAlternateKeyMask;
	}
	if( (inShortcutModifierKeys & kModifierKeysMask_Shift) != 0 )
	{
		mask |= NSShiftKeyMask;
	}
	//�V���[�g�J�b�g�ݒ�
	[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setKeyEquivalentModifierMask:mask];
}

//#798
/**
���j���[���ڂ̃V���[�g�J�b�g�擾
*/
void	AMenuWrapper::GetShortcut( const AMenuRef inMenuRef, const AIndex inItemIndex, 
		ANumber& outShortcutKey, AMenuShortcutModifierKeys& outShortcutModifierKeys )
{
	AText	text;
	ACocoa::SetTextFromNSString([ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) keyEquivalent],text);
	AUCS4Char	ucs4char = 0;
	text.Convert1CharToUCS4(0,ucs4char);
	outShortcutKey = (ANumber)ucs4char;
	//
	outShortcutModifierKeys = kModifierKeysMask_None;
	NSUInteger	mask = [ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) keyEquivalentModifierMask];
	if( (mask&NSCommandKeyMask) != 0 )
	{
		outShortcutModifierKeys |= kModifierKeysMask_Command;
	}
	if( (mask&NSControlKeyMask) != 0 )
	{
		outShortcutModifierKeys |= kModifierKeysMask_Control;
	}
	if( (mask&NSAlternateKeyMask) != 0 )
	{
		outShortcutModifierKeys |= kModifierKeysMask_Option;
	}
	if( (mask&NSShiftKeyMask) != 0 )
	{
		outShortcutModifierKeys |= kModifierKeysMask_Shift;
	}
}

//#798
/**
���j���[���ڂ̃V���[�g�J�b�g�ݒ�
*/
void	AMenuWrapper::GetShortcutDisplayText( const AMenuRef inMenuRef, const AIndex inItemIndex, AText& outText )
{
	ANumber	shortcutkey = 0;
	AMenuShortcutModifierKeys	modifier = 0;
	GetShortcut(inMenuRef,inItemIndex,shortcutkey,modifier);
	//
	if( shortcutkey >= 'a' && shortcutkey <= 'z' )
	{
		shortcutkey -= 'a'-'A';
	}
	//
	outText.DeleteAll();
	outText.AddModifiersKeyText(modifier);
	//
	outText.AddFromUCS4Char(static_cast<AUCS4Char>(shortcutkey));
}

/**
���j���[���ڂ̃`�F�b�N�}�[�N��S�폜
*/
void	AMenuWrapper::ClearAllCheckMark( const AMenuRef inMenuRef )
{
	for( AIndex i = 0; i < AMenuWrapper::GetMenuItemCount(inMenuRef); i++ )
	{
		AMenuWrapper::SetCheckMark(inMenuRef,i,false);
	}
}

/**
����inText�ƈ�v���郁�j���[���ڂ̃C���f�b�N�X���擾
*/
AIndex	AMenuWrapper::GetMenuItemIndexByText( const AMenuRef inMenuRef, const AText& inText )
{
	/*
	for( AIndex i = 0; i < AMenuWrapper::GetMenuItemCount(inMenuRef); i++ )
	{
		AText	text;
		AMenuWrapper::GetMenuItemText(inMenuRef,i,text);
		if( inText.Compare(text) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
	*/
	AStCreateNSStringFromAText	str(inText);
	NSInteger	index = [ACocoa::GetNSMenu(inMenuRef) indexOfItemWithTitle:str.GetNSString()];
	if( index == -1 )
	{
		return kIndex_Invalid;
	}
	else
	{
		return index;
	}
}

//#1477
/**
����inID��tag����v���郁�j���[���ڂ̃C���f�b�N�X���擾
*/
AIndex	AMenuWrapper::GetMenuItemIndexByID( const AMenuRef inMenuRef, const AMenuItemID inID )
{
	NSInteger	index = [ACocoa::GetNSMenu(inMenuRef) indexOfItemWithTag:inID];
	if( index == -1 )
	{
		return kIndex_Invalid;
	}
	else
	{
		return index;
	}
}

/**
�t�H���g���j���[�ݒ�
*/
void	AMenuWrapper::InsertFontMenu( const AMenuRef inMenuRef, const AIndex inMenuItemIndex, const AMenuItemID inItemID )
{
	//�����Ή�
}

#pragma mark ===========================

#pragma mark ---�R���e�L�X�g���j���[

/**
�R���e�L�X�g���j���[�o�^
*/
AMenuRef	AMenuWrapper::RegisterContextMenu( AConstCharPtr inMenuName )
{
	//������"main/menuname"�Ƃ����`���Ȃ̂ŁA�����Nib�t�@�C������Menu���ɕ������āACFStringRef�𐶐�����
	AText	nibMenuName(inMenuName);
	ATextArray	nibMenuNameArray;
	nibMenuName.Explode('/',nibMenuNameArray);
	if( nibMenuNameArray.GetItemCount() != 2 )
	{
		_ACThrow("wrong nib menu name",NULL);
	}
	AText	nibName, menuName;
	nibMenuNameArray.Get(0,nibName);
	nibMenuNameArray.Get(1,menuName);
	
	//Nib �����[�h�B
	AStCreateNSStringFromAText	str(menuName);
	CocoaContextualMenuController*	menuController = [[CocoaContextualMenuController alloc] initWithNibName:str.GetNSString()];
	
	//���[�h����menu controller�o�R��context menu���擾
	NSMenu*	contextualMenu = [menuController getContextualMenu];
	if( contextualMenu == nil ) {_ACError("",NULL);return NULL;}
	
	//Menu��action/target��������
	ACocoa::InitMenuActionAndTarget(contextualMenu);
	//�������K�v target��view�̂ق����悢�����H#601�p�B
	
	//Auto enable��OFF
	[contextualMenu setAutoenablesItems:NO];
	//���j���[�S����enable
	for( AIndex i = 0; i < [contextualMenu numberOfItems]; i++ )
	{
		[[contextualMenu itemAtIndex:i] setEnabled:YES];
	}
	//
	return contextualMenu;
	
	/*
	OSStatus	err = noErr;
	
	//������"main/menuname"�Ƃ����`���Ȃ̂ŁA�����Nib�t�@�C������Menu���ɕ������āACFStringRef�𐶐�����
	AText	nibMenuName(inMenuName);
	ATextArray	nibMenuNameArray;
	nibMenuName.Explode('/',nibMenuNameArray);
	if( nibMenuNameArray.GetItemCount() != 2 )
	{
		_ACThrow("wrong nib menu name",NULL);
	}
	AText	nibName, menuName;
	nibMenuNameArray.Get(0,nibName);
	nibMenuNameArray.Get(1,menuName);
	AStCreateCFStringFromAText	nibNameRef(nibName);
	AStCreateCFStringFromAText	menuNameRef(menuName);
	
	//���j���[����
	AMenuRef	menuRef = NULL;
	if( nibName.Compare("main") == true )
	{
		err = ::CreateMenuFromNib(CAppImp::GetNibRef(),menuNameRef.GetRef(),&menuRef);
		if( err != noErr )   _ACErrorNum("CreateMenuFromNib() returned error: ",err,NULL);
	}
	else if( nibName.Compare("menu") == true )//#0
	{
		err = ::CreateMenuFromNib(CAppImp::GetMenuNibRef(),menuNameRef.GetRef(),&menuRef);
		if( err != noErr )   _ACErrorNum("CreateMenuFromNib() returned error: ",err,NULL);
	}
	else
	{
		IBNibRef	nibRef;
		err = ::CreateNibReference(nibNameRef.GetRef(),&nibRef);
		if( err != noErr )   _ACErrorNum("CreateNibReference() returned error: ",err,NULL);
		err = ::CreateMenuFromNib(nibRef,menuNameRef.GetRef(),&menuRef);
		if( err != noErr )   _ACErrorNum("CreateMenuFromNib() returned error: ",err,NULL);
		::DisposeNibReference(nibRef);
	}
	return menuRef;
	*/
}

/**
�R���e�L�X�g���j���[�\��
*/
/*#688
void	AMenuWrapper::ShowContextMenu( const AMenuRef inMenuRef, const AGlobalPoint& inMousePoint, const AWindowRef inWindowRef )//win 080713 inWindowRef��Windows�p
{
	OSStatus	err = noErr;
	
	Point	pt;
	pt.h = inMousePoint.x;
	pt.v = inMousePoint.y;
	UInt32	selectionType;
	SInt16	menuID;
	MenuItemIndex	menuItem;
	err = ::ContextualMenuSelect(inMenuRef,pt,false,kCMHelpItemRemoveHelp,"\p",NULL,&selectionType,&menuID,&menuItem);
	//if( err != noErr )   _ACErrorNum("ContextualMenuSelect() returned error: ",err,NULL);
	if( selectionType == kCMMenuItemSelected )
	{
		//���������i���j���[�o�[�̃��j���[�Ɠ������[�g�ŏ��������j
	}
}
*/

//#232
/**
���j���[�ɃA�C�R����o�^
*/
/*
void	AMenuWrapper::SetMenuItemIcon( const AControlID inID, const AIndex inIndex, const AIconID inIconID )
{
	::SetMenuItemIconHandle(GetMenuRef(inID),1+inIndex,kMenuIconRefType,(Handle)GetIconRef(inIconID));
	::EnableMenuItemIcon(GetMenuRef(inID),1+inIndex);
}
*/

//win
//���j���[���ڃA�C�R���ݒ�
void	AMenuWrapper::SetMenuItemIcon( const AMenuRef inMenuRef, const AIndex inItemIndex, const AIconID inIconID )
{
	//���Ή�
}

#pragma mark ===========================

#pragma mark ---�S��
#if SUPPORT_CARBON
/**
���j���[�������i���j���[�o�[�����j
*/
void	ACarbonMenuWrapper::InitMenu()
{
	OSStatus	err = noErr;
	
	//���j���[�o�[����
	IBNibRef	nibRef = CAppImp::GetMenuNibRef();//#0
	if( nibRef == NULL )   nibRef = CAppImp::GetNibRef();//#0
	err = ::SetMenuBarFromNib(nibRef,CFSTR("MenuBar"));//#0
	if( err != noErr )   _ACErrorNum("SetMenuBarFromNib() returned error: ",err,NULL);
	//B0252 ���ݒ胁�j���[�͈�xEnable����ƕ\�������悤�ɂȂ�BDisable�ɂ͂Ȃ�Ȃ��̂ŁA�ŏ��Ɉ��Enable����B
	::EnableMenuCommand(NULL,kHICommandPreferences);
	
	//OS�W����WindowList���j���[�̍폜
	//�E�C���h�E���j���[�͎��O�ŊǗ�����̂ŁAOSX�Ŏ����I�ɏ��������̂�h���B�i������c���ƕʂ̃��j���[�ɃE�C���h�E���X�g���o�����肷��B�j
	{
		MenuRef	menuref;
		MenuItemIndex	menuindex;
		while(true)
		{
			menuref = NULL;
			err = ::GetIndMenuItemWithCommandID(NULL,kHICommandWindowListSeparator,1,&menuref,&menuindex);
			if( menuref == NULL )   break;
			::DeleteMenuItem(menuref,menuindex);
		}
		while(true)
		{
			menuref = NULL;
			err = ::GetIndMenuItemWithCommandID(NULL,kHICommandWindowListTerminator,1,&menuref,&menuindex);
			if( menuref == NULL )   break;
			::DeleteMenuItem(menuref,menuindex);
		}
	}
	
	//Debug���j���[
	if( AApplication::GetApplication().NVI_GetEnableDebugMenu() == false )
	{
		MenuRef	menuref;
		MenuItemIndex	menuindex;
		menuref = NULL;
		err = ::GetIndMenuItemWithCommandID(NULL,'DEBG',1,&menuref,&menuindex);
		if( menuref != NULL )
		{
			::DeleteMenuItem(menuref,menuindex);
		}
	}
	
	/*#539
	//�w���v�o�^�폜
	//http://lists.apple.com/archives/carbon-development/2003/Nov/msg00167.html
	static const CFStringRef apple_help_ID = CFSTR("com.apple.help");
	::CFPreferencesSetAppValue(CFSTR("mi"),NULL,apple_help_ID);
	::CFPreferencesSetAppValue(CFSTR("net.mimikaki.mi"),NULL,apple_help_ID);
	::CFPreferencesAppSynchronize(apple_help_ID);
	
	//B0158
	CFBundleRef	appBundle = ::CFBundleGetMainBundle();
	CFURLRef	appBundleURL = ::CFBundleCopyBundleURL(appBundle);
	FSRef	fref;
	::CFURLGetFSRef(appBundleURL,&fref);
	err = ::AHRegisterHelpBook(&fref);
	//if( err != noErr )   _AError("help not found",NULL);
	*/
}
#endif

//#1034
#if 0 
//�J���_�C�A���O�̃J�X�^�����i���j���[����g�p���Ă���B
/**
���[�g���j���[�擾
*/
ACarbonMenuRef	ACarbonMenuWrapper::GetRootMenu()
{
	return ::AcquireRootMenu();
}

#pragma mark ===========================

#pragma mark ---���j���[

/**
���j���[����
*/
ACarbonMenuRef	ACarbonMenuWrapper::CreateMenu()
{
	OSStatus	err = noErr;
	
	ACarbonMenuRef	menuRef = NULL;
	err = ::CreateNewMenu(0,0,&menuRef);
	if( err != noErr )   _ACErrorNum("CreateNewMenu() returned error: ",err,NULL);
	return menuRef;
}

/**
���j���[�폜
*/
void	ACarbonMenuWrapper::DeleteMenu( const ACarbonMenuRef inMenuRef )
{
	::DisposeMenu(inMenuRef);
}

/**
���j���[���ڐ��擾
*/
AItemCount	ACarbonMenuWrapper::GetMenuItemCount( const ACarbonMenuRef inMenuRef )
{
	return ::CountMenuItems(inMenuRef);
}

/**
���j���[�^�C�g���e�L�X�g�擾
*/
void	ACarbonMenuWrapper::GetMenuTitleText( const ACarbonMenuRef inMenuRef, AText& outText )
{
	OSStatus	err = noErr;
	
	outText.DeleteAll();
	CFStringRef	strref = NULL;
	err = ::CopyMenuTitleAsCFString(inMenuRef,&strref);
	if( strref == NULL )   return;
	outText.SetFromCFString(strref);
	::CFRelease(strref);
}

#pragma mark ===========================

#pragma mark ---���j���[���ځ@�ǉ��E�폜

#pragma mark ===========================

#pragma mark ---���j���[���ځ@���ݒ�E�擾

/**
�T�u���j���[�擾
*/
ACarbonMenuRef	ACarbonMenuWrapper::GetSubMenu( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex )
{
	OSStatus	err = noErr;
	
	ACarbonMenuRef	menuRef = NULL;
	err = ::GetMenuItemHierarchicalMenu(inMenuRef,1+inItemIndex,&menuRef);
	return menuRef;
}

/**
�T�u���j���[�ݒ�
*/
void	ACarbonMenuWrapper::SetSubMenu( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const ACarbonMenuRef inSubMenuRef )
{
	OSStatus	err = noErr;
	
	err = ::SetMenuItemHierarchicalMenu(inMenuRef,1+inItemIndex,inSubMenuRef);
	if( err != noErr )   _ACErrorNum("SetMenuItemHierarchicalMenu() returned error: ",err,NULL);
}

/**
MenuItemID�擾
*/
AMenuItemID	ACarbonMenuWrapper::GetMenuItemID( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex )
{
	OSStatus	err = noErr;
	
	MenuCommand	commandID = 0;
	err = ::GetMenuItemCommandID(inMenuRef,1+inItemIndex,&commandID);
	return commandID;
}

/**
MenuItemID�ݒ�
*/
void	ACarbonMenuWrapper::SetMenuItemID( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const AMenuItemID inMenuItemID )
{
	OSStatus	err = noErr;
	
	err = ::SetMenuItemCommandID(inMenuRef,1+inItemIndex,inMenuItemID);
	if( err != noErr )   _ACErrorNum("SetMenuItemCommandID() returned error: ",err,NULL);
}

/**
���j���[���ڃe�L�X�g�擾
*/
void	ACarbonMenuWrapper::GetMenuItemText( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, AText& outText )
{
	OSStatus	err = noErr;
	
	outText.DeleteAll();
	CFStringRef	strref = NULL;
	err = ::CopyMenuItemTextAsCFString(inMenuRef,1+inItemIndex,&strref);
	if( strref == NULL )   return;
	outText.SetFromCFString(strref);
	::CFRelease(strref);
}

/**
���j���[���ڃe�L�X�g�ݒ�
*/
void	ACarbonMenuWrapper::SetMenuItemText( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText )
{
	OSStatus	err = noErr;
	
	AStCreateCFStringFromAText	strref(inText);
	err = ::SetMenuItemTextWithCFString(inMenuRef,1+inItemIndex,strref.GetRef());
	if( err != noErr )   _ACErrorNum("SetMenuItemTextWithCFString() returned error: ",err,NULL);
}

/**
���j���[����Enable/Disable
*/
void	ACarbonMenuWrapper::SetEnableMenuItem( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const ABool inEnable )
{
	if( inEnable == true )
	{
		::EnableMenuItem(inMenuRef,1+inItemIndex);
	}
	else
	{
		::DisableMenuItem(inMenuRef,1+inItemIndex);
	}
}

/**
���j���[���ڒǉ��i�}���j
*/
void	ACarbonMenuWrapper::InsertMenuItem( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText, 
		const AMenuItemID inItemID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	OSStatus	err = noErr;
	
	//���j���[���ڒǉ��{�e�L�X�g�ݒ�
	{
		AStCreateCFStringFromAText	strref(inText);
		err = ::InsertMenuItemTextWithCFString(inMenuRef,strref.GetRef(),inItemIndex,0,0);
		if( err != noErr )   _ACErrorNum("InsertMenuItemTextWithCFString() returned error: ",err,NULL);
	}
	//MenuItemID�̐ݒ�
	err = ::SetMenuItemCommandID(inMenuRef,1+inItemIndex,inItemID);
	if( err != noErr )   _ACErrorNum("SetMenuItemCommandID() returned error: ",err,NULL);
	//�V���[�g�J�b�g�L�[�ݒ�
	if( inShortcutKey != 0 )
	{
		err = ::SetMenuItemCommandKey(inMenuRef,1+inItemIndex,false,inShortcutKey);
		if( err != noErr )   _ACErrorNum("SetMenuItemCommandKey() returned error: ",err,NULL);
		err = ::SetMenuItemModifiers(inMenuRef,1+inItemIndex,inShortcutModifierKeys);
		if( err != noErr )   _ACErrorNum("SetMenuItemModifiers() returned error: ",err,NULL);
	}
}

/**
���j���[���ځi�Z�p���[�^�j�ǉ��i�}���j
*/
void	ACarbonMenuWrapper::InsertMenuItem_Separator( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex,
		const AMenuItemID inItemID )//#427
{
	OSStatus	err = noErr;
	
	AText	text("-");
	AStCreateCFStringFromAText	strref(text);
	err = ::InsertMenuItemTextWithCFString(inMenuRef,strref.GetRef(),inItemIndex,0,0);
	if( err != noErr )   _ACErrorNum("InsertMenuItemTextWithCFString() returned error: ",err,NULL);
	//MenuItemID�̐ݒ�
	err = ::SetMenuItemCommandID(inMenuRef,1+inItemIndex,inItemID);
	if( err != noErr )   _ACErrorNum("SetMenuItemCommandID() returned error: ",err,NULL);
}

/**
���j���[���ځi�Z�p���[�^�j�ǉ�
*/
void	ACarbonMenuWrapper::AddMenuItem_Separator( const ACarbonMenuRef inMenuRef, const AMenuItemID inItemID )//#427
{
	ACarbonMenuWrapper::InsertMenuItem_Separator(inMenuRef,ACarbonMenuWrapper::GetMenuItemCount(inMenuRef),inItemID);//#427
}

/**
���j���[���ڍ폜
*/
void	ACarbonMenuWrapper::DeleteMenuItem( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex )
{
	//::DeleteMenuItem()�ŃT�u���j���[���폜����邩�ǂ����s���Ȃ̂ŁA�Ƃ肠�������O�őS�폜����B
	/*ACarbonMenuRef	subMenuRef = ACarbonMenuWrapper::GetSubMenu(inMenuRef,inItemIndex);
	if( subMenuRef != NULL )
	{
		ACarbonMenuWrapper::SetSubMenu(inMenuRef,inItemIndex,NULL);
		ACarbonMenuWrapper::DeleteAllMenuItems(subMenuRef);
		ACarbonMenuWrapper::DeleteMenu(subMenuRef);
	}*/
	//���ڍ폜
	::DeleteMenuItem(inMenuRef,1+inItemIndex);
}

/**
���j���[���ڑS�폜
*/
void	ACarbonMenuWrapper::DeleteAllMenuItems( const ACarbonMenuRef inMenuRef )
{
	while( ACarbonMenuWrapper::GetMenuItemCount(inMenuRef) > 0 )
	{
		ACarbonMenuWrapper::DeleteMenuItem(inMenuRef,0);
	}
}

/**
TextArray���烁�j���[���ڒǉ�
*/
void	ACarbonMenuWrapper::AddMenuItemsFromTextArray( const ACarbonMenuRef inMenuRef, const ATextArray& inTextArray )
{
	for( AIndex index = 0; index < inTextArray.GetItemCount(); index++ )
	{
		AText	text;
		inTextArray.Get(index,text);
		ACarbonMenuWrapper::InsertMenuItem(inMenuRef,ACarbonMenuWrapper::GetMenuItemCount(inMenuRef),text,0,0,0);
	}
}

/**
TextArray���烁�j���[���ڐݒ�
*/
void	ACarbonMenuWrapper::SetMenuItemsFromTextArray( const ACarbonMenuRef inMenuRef, const ATextArray& inTextArray )
{
	ACarbonMenuWrapper::DeleteAllMenuItems(inMenuRef);
	ACarbonMenuWrapper::AddMenuItemsFromTextArray(inMenuRef,inTextArray);
}

/**
���j���[���ڂɃ`�F�b�N�}�[�N�ݒ�
*/
void	ACarbonMenuWrapper::SetCheckMark( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const ABool inCheck )
{
	::CheckMenuItem(inMenuRef,1+inItemIndex,inCheck);
}

/**
���j���[���ڂɃV���[�g�J�b�g�ݒ�
*/
void	ACarbonMenuWrapper::SetShortcut( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	OSStatus	err = noErr;
	
	err = ::SetMenuItemCommandKey(inMenuRef,1+inItemIndex,false,inShortcutKey);
	if( err != noErr )   _ACErrorNum("SetMenuItemCommandKey() returned error: ",err,NULL);
	err = ::SetMenuItemModifiers(inMenuRef,1+inItemIndex,inShortcutModifierKeys);
	if( err != noErr )   _ACErrorNum("SetMenuItemModifiers() returned error: ",err,NULL);
}

/**
���j���[���ڂ̃`�F�b�N�}�[�N��S�폜
*/
void	ACarbonMenuWrapper::ClearAllCheckMark( const ACarbonMenuRef inMenuRef )
{
	for( AIndex i = 0; i < ACarbonMenuWrapper::GetMenuItemCount(inMenuRef); i++ )
	{
		ACarbonMenuWrapper::SetCheckMark(inMenuRef,i,false);
	}
}

/**
����inText�ƈ�v���郁�j���[���ڂ̃C���f�b�N�X���擾
*/
AIndex	ACarbonMenuWrapper::GetMenuItemIndexByText( const ACarbonMenuRef inMenuRef, const AText& inText )
{
	for( AIndex i = 0; i < ACarbonMenuWrapper::GetMenuItemCount(inMenuRef); i++ )
	{
		AText	text;
		ACarbonMenuWrapper::GetMenuItemText(inMenuRef,i,text);
		if( inText.Compare(text) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

/**
�t�H���g���j���[�ݒ�
*/
void	ACarbonMenuWrapper::InsertFontMenu( const ACarbonMenuRef inMenuRef, const AIndex inMenuItemIndex, const AMenuItemID inItemID )
{
	/*ItemCount	fontCount;
	::ATSUFontCount(&fontCount);
	AArray<ATSUFontID>	fontIDArray;
	fontIDArray.Reserve(0,fontCount);
	AStArrayPtr<ATSUFontID>	fontIDArrayPtr(fontIDArray,0,fontIDArray.GetItemCount());
	::ATSUGetFontIDs(fontIDArrayPtr.GetPtr(),fontIDArray.GetItemCount(),&fontCount);
	for( AIndex index = 0; index < fontIDArray.GetItemCount(); index++ )
	{
		char	buf[4096];
		ByteCount	nameLength;
		ItemCount	nameIndex;
		::ATSUFindFontName(fontIDArray.Get(index),kFontFullName,kFontUnicodePlatform,kFontNoScriptCode,kFontNoLanguageCode,4096,buf,&nameLength,&nameIndex);
		AText	text;
		text.AddFromTextPtr(buf,nameLength);
		text.ConvertToUTF8FromUTF16();
		ACarbonMenuWrapper::InsertMenuItem(inMenuRef,inMenuItemIndex+index,text,inItemID,NULL,0);
		::SetMenuItemCommandID(inMenuRef,1+inMenuItemIndex+index,inItemID);
	}
	return;*/
	AItemCount	preCount = ACarbonMenuWrapper::GetMenuItemCount(inMenuRef);
	ItemCount	fontMenuItemCount;
	::CreateStandardFontMenu(inMenuRef,1+inMenuItemIndex,300,0/*kHierarchicalFontMenuOption*/,&fontMenuItemCount);
	//#521 Disable�t�H���g�̃��j���[���ڂ͍폜����
	for( AIndex index = preCount; index < ACarbonMenuWrapper::GetMenuItemCount(inMenuRef); )
	{
		//Font Number�擾
		FMFontFamily	fontFamily = 0;
		FMFontStyle	style = 0;
		::GetFontFamilyFromMenuSelection(inMenuRef,1+index,&fontFamily,&style);
		//Font�g�p�\���`�F�b�N
		ATSUFontID	fontID = 0;
		if( ::ATSUFONDtoFontID(fontFamily,NULL,&fontID) == noErr )
		{
			//�g�p�\�Ȃ玟��
			index++;
		}
		else
		{
			//�g�p�s�Ȃ烁�j���[���ڍ폜
			ACarbonMenuWrapper::DeleteMenuItem(inMenuRef,index);
		}
	}
	//
	AItemCount	added = ACarbonMenuWrapper::GetMenuItemCount(inMenuRef)-preCount;
	//MenuItemID�̐ݒ�
	for( AIndex i = 0; i < added; i++ )
	{
		::SetMenuItemCommandID(inMenuRef,1+inMenuItemIndex+i,inItemID);
	}
}

#pragma mark ===========================

#pragma mark ---�R���e�L�X�g���j���[
#if SUPPORT_CARBON
/**
�R���e�L�X�g���j���[�o�^
*/
ACarbonMenuRef	ACarbonMenuWrapper::RegisterContextMenu( AConstCharPtr inMenuName )
{
	OSStatus	err = noErr;
	
	//������"main/menuname"�Ƃ����`���Ȃ̂ŁA�����Nib�t�@�C������Menu���ɕ������āACFStringRef�𐶐�����
	AText	nibMenuName(inMenuName);
	ATextArray	nibMenuNameArray;
	nibMenuName.Explode('/',nibMenuNameArray);
	if( nibMenuNameArray.GetItemCount() != 2 )
	{
		_ACThrow("wrong nib menu name",NULL);
	}
	AText	nibName, menuName;
	nibMenuNameArray.Get(0,nibName);
	nibMenuNameArray.Get(1,menuName);
	AStCreateCFStringFromAText	nibNameRef(nibName);
	AStCreateCFStringFromAText	menuNameRef(menuName);
	
	//���j���[����
	ACarbonMenuRef	menuRef = NULL;
	if( nibName.Compare("main") == true )
	{
		err = ::CreateMenuFromNib(CAppImp::GetNibRef(),menuNameRef.GetRef(),&menuRef);
		if( err != noErr )   _ACErrorNum("CreateMenuFromNib() returned error: ",err,NULL);
	}
	else if( nibName.Compare("menu") == true )//#0
	{
		err = ::CreateMenuFromNib(CAppImp::GetMenuNibRef(),menuNameRef.GetRef(),&menuRef);
		if( err != noErr )   _ACErrorNum("CreateMenuFromNib() returned error: ",err,NULL);
	}
	else
	{
		IBNibRef	nibRef;
		err = ::CreateNibReference(nibNameRef.GetRef(),&nibRef);
		if( err != noErr )   _ACErrorNum("CreateNibReference() returned error: ",err,NULL);
		err = ::CreateMenuFromNib(nibRef,menuNameRef.GetRef(),&menuRef);
		if( err != noErr )   _ACErrorNum("CreateMenuFromNib() returned error: ",err,NULL);
		::DisposeNibReference(nibRef);
	}
	return menuRef;
}
#endif
/**
�R���e�L�X�g���j���[�\��
*/
void	ACarbonMenuWrapper::ShowContextMenu( const ACarbonMenuRef inMenuRef, const AGlobalPoint& inMousePoint, const AWindowRef inWindowRef )//win 080713 inWindowRef��Windows�p
{
	OSStatus	err = noErr;
	
	Point	pt;
	pt.h = inMousePoint.x;
	pt.v = inMousePoint.y;
	UInt32	selectionType;
	SInt16	menuID;
	MenuItemIndex	menuItem;
	err = ::ContextualMenuSelect(inMenuRef,pt,false,/* kCMHelpItemNoHelp*/kCMHelpItemRemoveHelp,"\p",NULL,&selectionType,&menuID,&menuItem);
	//if( err != noErr )   _ACErrorNum("ContextualMenuSelect() returned error: ",err,NULL);
	if( selectionType == kCMMenuItemSelected )
	{
		//���������i���j���[�o�[�̃��j���[�Ɠ������[�g�ŏ��������j
	}
}

//#232
/**
���j���[�ɃA�C�R����o�^
*/
/*
void	ACarbonMenuWrapper::SetMenuItemIcon( const AControlID inID, const AIndex inIndex, const AIconID inIconID )
{
	::SetMenuItemIconHandle(GetMenuRef(inID),1+inIndex,kMenuIconRefType,(Handle)GetIconRef(inIconID));
	::EnableMenuItemIcon(GetMenuRef(inID),1+inIndex);
}
*/

//win
//���j���[���ڃA�C�R���ݒ�
void	ACarbonMenuWrapper::SetMenuItemIcon( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const AIconID inIconID )
{
	//���Ή�
}
#endif
