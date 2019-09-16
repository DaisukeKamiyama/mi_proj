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

#pragma mark ---全体

/**
メニュー初期化（メニューバー生成）
*/
void	AMenuWrapper::InitMenu()
{
}

/**
ルートメニュー取得
*/
AMenuRef	AMenuWrapper::GetRootMenu()
{
	return [NSApp mainMenu];
}

#pragma mark ===========================

#pragma mark ---メニュー

/**
メニュー生成
*/
AMenuRef	AMenuWrapper::CreateMenu()
{
	NSMenu*	menu = [[NSMenu alloc] initWithTitle:@""];
	//デフォルトはauto enableをNOにする
	[menu setAutoenablesItems:NO];
	return menu;
}

/**
メニュー削除
*/
void	AMenuWrapper::DeleteMenu( const AMenuRef inMenuRef )
{
	//メニュー解放
	[ACocoa::GetNSMenu(inMenuRef) release];
}

/**
メニュー項目数取得
*/
AItemCount	AMenuWrapper::GetMenuItemCount( const AMenuRef inMenuRef )
{
	return [ACocoa::GetNSMenu(inMenuRef) numberOfItems];
}

/**
メニュータイトルテキスト取得
*/
void	AMenuWrapper::GetMenuTitleText( const AMenuRef inMenuRef, AText& outText )
{
	ACocoa::SetTextFromNSString([ACocoa::GetNSMenu(inMenuRef) title],outText);
}

/**
Autoenable設定
*/
void	AMenuWrapper::EnableAutoEnableMenu( const AMenuRef inMenuRef )
{
	[ACocoa::GetNSMenu(inMenuRef) setAutoenablesItems:YES];
}

#pragma mark ===========================

#pragma mark ---メニュー項目　追加・削除

#pragma mark ===========================

#pragma mark ---メニュー項目　情報設定・取得

/**
サブメニュー取得
*/
AMenuRef	AMenuWrapper::GetSubMenu( const AMenuRef inMenuRef, const AIndex inItemIndex )
{
	return [ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) submenu];
}

/**
サブメニュー設定
*/
void	AMenuWrapper::SetSubMenu( const AMenuRef inMenuRef, const AIndex inItemIndex, const AMenuRef inSubMenuRef )
{
	//指定メニュー項目のサブメニューを設定
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
サブメニューdetach
@note Cocoaでは親メニューを複数設定出来ないので、他のメニューのサブメニューにするときには、先にこの関数でdetachしてから、サブメニューにする。
*/
void	AMenuWrapper::DetachSubMenu( const AMenuRef inSubMenuRef, AMenuRef& outOldParentMenuRef, AIndex& outOldParentMenuItemIndex )
{
	//結果初期化
	outOldParentMenuRef = NULL;
	outOldParentMenuItemIndex = kIndex_Invalid;
	//detachするサブメニューと、その親を取得
	NSMenu*	subMenu = ACocoa::GetNSMenu(inSubMenuRef);
	NSMenu*	oldParentMenu = [subMenu supermenu];
	if( oldParentMenu != nil )
	{
		//親メニュー、および、item indexを記憶
		outOldParentMenuRef = static_cast<AMenuRef>(oldParentMenu);
		outOldParentMenuItemIndex = [oldParentMenu indexOfItemWithSubmenu:subMenu];
		//detach（サブメニュー解除）
		[[oldParentMenu itemAtIndex:outOldParentMenuItemIndex] setSubmenu:nil];
	}
}

/**
MenuItemID取得
*/
AMenuItemID	AMenuWrapper::GetMenuItemID( const AMenuRef inMenuRef, const AIndex inItemIndex )
{
	return [ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) tag];
}

/**
MenuItemID設定
*/
void	AMenuWrapper::SetMenuItemID( const AMenuRef inMenuRef, const AIndex inItemIndex, const AMenuItemID inMenuItemID )
{
	[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setTag:inMenuItemID];
	
	//target/action設定
	//item idが0, -1以外の場合のみ設定する。（popup button の場合、popup button の方のactionを実行させたいが、
	//menuitemのactionを設定すると、popup buttonのactionが実行されないため。）
	if( inMenuItemID != 0 && inMenuItemID != kMenuItemID_Invalid )
	{
		[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setTarget:[NSApp delegate]];
		[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setAction:@selector(doMenuActions:)];
	}
}

/**
メニュー項目テキスト取得
*/
void	AMenuWrapper::GetMenuItemText( const AMenuRef inMenuRef, const AIndex inItemIndex, AText& outText )
{
	ACocoa::SetTextFromNSString([ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) title],outText);
}

/**
メニュー項目テキスト設定
*/
void	AMenuWrapper::SetMenuItemText( const AMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText )
{
	AStCreateNSStringFromAText	str(inText);
	[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setTitle:str.GetNSString()];
}

/**
メニュー項目Enable/Disable
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
メニュー項目Enable/Disable
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
メニュー項目追加（挿入）
*/
void	AMenuWrapper::InsertMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText, 
		const AMenuItemID inItemID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	//項目挿入
	if( inText.Compare("-") == false )
	{
		//#1004
		//メニュー項目に長いテキストを設定すると処理が重くなるので、制限する。
		AText	text;
		text.SetText(inText);
		text.LimitLength(0,kMenuTextMax);
		//
		AStCreateNSStringFromAText	str(text);
		NSMenuItem*	menuItem = [ACocoa::GetNSMenu(inMenuRef) insertItemWithTitle:str.GetNSString() 
				action:nil/*@selector(doMenuActions:)*/ keyEquivalent:@"" atIndex:inItemIndex];
		//tag設定
		[menuItem setTag:inItemID];
		//key equivalent設定
		if( inShortcutKey != 0 )
		{
			AMenuWrapper::SetShortcut(inMenuRef,inItemIndex,inShortcutKey,inShortcutModifierKeys);
		}
		//target/action設定
		//item idが0, -1以外の場合のみ設定する。（popup button の場合、popup button の方のactionを実行させたいが、
		//menuitemのactionを設定すると、popup buttonのactionが実行されないため。）
		if( inItemID != 0 && inItemID != kMenuItemID_Invalid )
		{
			[menuItem setTarget:[NSApp delegate]];
			[menuItem setAction:@selector(doMenuActions:)];
		}
		//enableに設定
		[menuItem setEnabled:YES];
	}
	else
	{
		[ACocoa::GetNSMenu(inMenuRef) insertItem:[NSMenuItem separatorItem] atIndex:inItemIndex];
		//セパレータにもMenuItemIDを設定する。（ADynamicMenu::Realize()/Unrealize()にて、終了位置をMenuItemIDで判定しているので、
		//セパレータにもMenuItemIDを設定する必要がある。）
		[[ACocoa::GetNSMenu(inMenuRef) itemAtIndex:inItemIndex] setTag:inItemID];
	}
}

/**
メニュー項目追加
*/
void	AMenuWrapper::AddMenuItem( const AMenuRef inMenuRef, const AText& inText, 
		const AMenuItemID inItemID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	AMenuWrapper::InsertMenuItem(inMenuRef,AMenuWrapper::GetMenuItemCount(inMenuRef),
				inText,inItemID,inShortcutKey,inShortcutModifierKeys);
}

/**
メニュー項目（セパレータ）追加（挿入）
*/
void	AMenuWrapper::InsertMenuItem_Separator( const AMenuRef inMenuRef, const AIndex inItemIndex,
		const AMenuItemID inItemID )//#427
{
	AText	text("-");
	AMenuWrapper::InsertMenuItem(inMenuRef,inItemIndex,text,inItemID,0,0);
}

/**
メニュー項目（セパレータ）追加
*/
void	AMenuWrapper::AddMenuItem_Separator( const AMenuRef inMenuRef, const AMenuItemID inItemID )//#427
{
	AMenuWrapper::InsertMenuItem_Separator(inMenuRef,AMenuWrapper::GetMenuItemCount(inMenuRef),inItemID);//#427
}

/**
メニュー項目削除
@note サブメニューはdetachされ、削除はされない。
*/
void	AMenuWrapper::DeleteMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex )
{
	//サブメニューdetach
	AMenuRef	subMenu = GetSubMenu(inMenuRef,inItemIndex);
	if( subMenu != nil )
	{
		AMenuRef	oldParentMenuRef = nil;
		AIndex	oldParentMenuItemIndex = kIndex_Invalid;
		DetachSubMenu(subMenu,oldParentMenuRef,oldParentMenuItemIndex);
	}
	//メニュー項目削除
	[ACocoa::GetNSMenu(inMenuRef) removeItemAtIndex:inItemIndex];
}

/**
メニュー項目全削除
*/
void	AMenuWrapper::DeleteAllMenuItems( const AMenuRef inMenuRef )
{
	while( AMenuWrapper::GetMenuItemCount(inMenuRef) > 0 )
	{
		AMenuWrapper::DeleteMenuItem(inMenuRef,0);
	}
}

/**
TextArrayからメニュー項目追加
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
TextArrayからメニュー項目設定
*/
void	AMenuWrapper::SetMenuItemsFromTextArray( const AMenuRef inMenuRef, const ATextArray& inTextArray )
{
	AMenuWrapper::DeleteAllMenuItems(inMenuRef);
	AMenuWrapper::AddMenuItemsFromTextArray(inMenuRef,inTextArray);
}

/**
メニュー項目にチェックマーク設定
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
メニュー項目にショートカット設定
*/
void	AMenuWrapper::SetShortcut( const AMenuRef inMenuRef, const AIndex inItemIndex, 
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	//Key equivalent設定
	//setKeyEquivalentは大文字の場合自動的にshift有りとみなすので、小文字に変換
	ANumber	shortcutkey = inShortcutKey;
	if( shortcutkey >= 'A' && shortcutkey <= 'Z' )
	{
		shortcutkey += 'a'-'A';
	}
	//Key equivalentのNSStringを取得
	AText	keyEqText;
	keyEqText.InsertFromUCS4Char(0,static_cast<AUCS4Char>(shortcutkey));
	AStCreateNSStringFromAText	keyeqstr(keyEqText);
	[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setKeyEquivalent:keyeqstr.GetNSString()];
	//Mask設定
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
	//ショートカット設定
	[ACocoa::GetNSMenuItem(ACocoa::GetNSMenu(inMenuRef),inItemIndex) setKeyEquivalentModifierMask:mask];
}

//#798
/**
メニュー項目のショートカット取得
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
メニュー項目のショートカット設定
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
メニュー項目のチェックマークを全削除
*/
void	AMenuWrapper::ClearAllCheckMark( const AMenuRef inMenuRef )
{
	for( AIndex i = 0; i < AMenuWrapper::GetMenuItemCount(inMenuRef); i++ )
	{
		AMenuWrapper::SetCheckMark(inMenuRef,i,false);
	}
}

/**
引数inTextと一致するメニュー項目のインデックスを取得
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
引数inIDとtagが一致するメニュー項目のインデックスを取得
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
フォントメニュー設定
*/
void	AMenuWrapper::InsertFontMenu( const AMenuRef inMenuRef, const AIndex inMenuItemIndex, const AMenuItemID inItemID )
{
	//★未対応
}

#pragma mark ===========================

#pragma mark ---コンテキストメニュー

/**
コンテキストメニュー登録
*/
AMenuRef	AMenuWrapper::RegisterContextMenu( AConstCharPtr inMenuName )
{
	//引数は"main/menuname"という形式なので、これをNibファイル名とMenu名に分解して、CFStringRefを生成する
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
	
	//Nib をロード。
	AStCreateNSStringFromAText	str(menuName);
	CocoaContextualMenuController*	menuController = [[CocoaContextualMenuController alloc] initWithNibName:str.GetNSString()];
	
	//ロードしたmenu controller経由でcontext menuを取得
	NSMenu*	contextualMenu = [menuController getContextualMenu];
	if( contextualMenu == nil ) {_ACError("",NULL);return NULL;}
	
	//Menuのaction/targetを初期化
	ACocoa::InitMenuActionAndTarget(contextualMenu);
	//★検討必要 targetはviewのほうがよいかも？#601用。
	
	//Auto enableはOFF
	[contextualMenu setAutoenablesItems:NO];
	//メニュー全項目enable
	for( AIndex i = 0; i < [contextualMenu numberOfItems]; i++ )
	{
		[[contextualMenu itemAtIndex:i] setEnabled:YES];
	}
	//
	return contextualMenu;
	
	/*
	OSStatus	err = noErr;
	
	//引数は"main/menuname"という形式なので、これをNibファイル名とMenu名に分解して、CFStringRefを生成する
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
	
	//メニュー生成
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
コンテキストメニュー表示
*/
/*#688
void	AMenuWrapper::ShowContextMenu( const AMenuRef inMenuRef, const AGlobalPoint& inMousePoint, const AWindowRef inWindowRef )//win 080713 inWindowRefはWindows用
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
		//処理無し（メニューバーのメニューと同じルートで処理される）
	}
}
*/

//#232
/**
メニューにアイコンを登録
*/
/*
void	AMenuWrapper::SetMenuItemIcon( const AControlID inID, const AIndex inIndex, const AIconID inIconID )
{
	::SetMenuItemIconHandle(GetMenuRef(inID),1+inIndex,kMenuIconRefType,(Handle)GetIconRef(inIconID));
	::EnableMenuItemIcon(GetMenuRef(inID),1+inIndex);
}
*/

//win
//メニュー項目アイコン設定
void	AMenuWrapper::SetMenuItemIcon( const AMenuRef inMenuRef, const AIndex inItemIndex, const AIconID inIconID )
{
	//未対応
}

#pragma mark ===========================

#pragma mark ---全体
#if SUPPORT_CARBON
/**
メニュー初期化（メニューバー生成）
*/
void	ACarbonMenuWrapper::InitMenu()
{
	OSStatus	err = noErr;
	
	//メニューバー生成
	IBNibRef	nibRef = CAppImp::GetMenuNibRef();//#0
	if( nibRef == NULL )   nibRef = CAppImp::GetNibRef();//#0
	err = ::SetMenuBarFromNib(nibRef,CFSTR("MenuBar"));//#0
	if( err != noErr )   _ACErrorNum("SetMenuBarFromNib() returned error: ",err,NULL);
	//B0252 環境設定メニューは一度Enableすると表示されるようになる。Disableにはならないので、最初に一回Enableする。
	::EnableMenuCommand(NULL,kHICommandPreferences);
	
	//OS標準のWindowListメニューの削除
	//ウインドウメニューは自前で管理するので、OSXで自動的に処理されるのを防ぐ。（これを残すと別のメニューにウインドウリストが出来たりする。）
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
	
	//Debugメニュー
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
	//ヘルプ登録削除
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
//開くダイアログのカスタム部品メニューから使用している。
/**
ルートメニュー取得
*/
ACarbonMenuRef	ACarbonMenuWrapper::GetRootMenu()
{
	return ::AcquireRootMenu();
}

#pragma mark ===========================

#pragma mark ---メニュー

/**
メニュー生成
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
メニュー削除
*/
void	ACarbonMenuWrapper::DeleteMenu( const ACarbonMenuRef inMenuRef )
{
	::DisposeMenu(inMenuRef);
}

/**
メニュー項目数取得
*/
AItemCount	ACarbonMenuWrapper::GetMenuItemCount( const ACarbonMenuRef inMenuRef )
{
	return ::CountMenuItems(inMenuRef);
}

/**
メニュータイトルテキスト取得
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

#pragma mark ---メニュー項目　追加・削除

#pragma mark ===========================

#pragma mark ---メニュー項目　情報設定・取得

/**
サブメニュー取得
*/
ACarbonMenuRef	ACarbonMenuWrapper::GetSubMenu( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex )
{
	OSStatus	err = noErr;
	
	ACarbonMenuRef	menuRef = NULL;
	err = ::GetMenuItemHierarchicalMenu(inMenuRef,1+inItemIndex,&menuRef);
	return menuRef;
}

/**
サブメニュー設定
*/
void	ACarbonMenuWrapper::SetSubMenu( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const ACarbonMenuRef inSubMenuRef )
{
	OSStatus	err = noErr;
	
	err = ::SetMenuItemHierarchicalMenu(inMenuRef,1+inItemIndex,inSubMenuRef);
	if( err != noErr )   _ACErrorNum("SetMenuItemHierarchicalMenu() returned error: ",err,NULL);
}

/**
MenuItemID取得
*/
AMenuItemID	ACarbonMenuWrapper::GetMenuItemID( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex )
{
	OSStatus	err = noErr;
	
	MenuCommand	commandID = 0;
	err = ::GetMenuItemCommandID(inMenuRef,1+inItemIndex,&commandID);
	return commandID;
}

/**
MenuItemID設定
*/
void	ACarbonMenuWrapper::SetMenuItemID( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const AMenuItemID inMenuItemID )
{
	OSStatus	err = noErr;
	
	err = ::SetMenuItemCommandID(inMenuRef,1+inItemIndex,inMenuItemID);
	if( err != noErr )   _ACErrorNum("SetMenuItemCommandID() returned error: ",err,NULL);
}

/**
メニュー項目テキスト取得
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
メニュー項目テキスト設定
*/
void	ACarbonMenuWrapper::SetMenuItemText( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText )
{
	OSStatus	err = noErr;
	
	AStCreateCFStringFromAText	strref(inText);
	err = ::SetMenuItemTextWithCFString(inMenuRef,1+inItemIndex,strref.GetRef());
	if( err != noErr )   _ACErrorNum("SetMenuItemTextWithCFString() returned error: ",err,NULL);
}

/**
メニュー項目Enable/Disable
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
メニュー項目追加（挿入）
*/
void	ACarbonMenuWrapper::InsertMenuItem( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText, 
		const AMenuItemID inItemID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	OSStatus	err = noErr;
	
	//メニュー項目追加＋テキスト設定
	{
		AStCreateCFStringFromAText	strref(inText);
		err = ::InsertMenuItemTextWithCFString(inMenuRef,strref.GetRef(),inItemIndex,0,0);
		if( err != noErr )   _ACErrorNum("InsertMenuItemTextWithCFString() returned error: ",err,NULL);
	}
	//MenuItemIDの設定
	err = ::SetMenuItemCommandID(inMenuRef,1+inItemIndex,inItemID);
	if( err != noErr )   _ACErrorNum("SetMenuItemCommandID() returned error: ",err,NULL);
	//ショートカットキー設定
	if( inShortcutKey != 0 )
	{
		err = ::SetMenuItemCommandKey(inMenuRef,1+inItemIndex,false,inShortcutKey);
		if( err != noErr )   _ACErrorNum("SetMenuItemCommandKey() returned error: ",err,NULL);
		err = ::SetMenuItemModifiers(inMenuRef,1+inItemIndex,inShortcutModifierKeys);
		if( err != noErr )   _ACErrorNum("SetMenuItemModifiers() returned error: ",err,NULL);
	}
}

/**
メニュー項目（セパレータ）追加（挿入）
*/
void	ACarbonMenuWrapper::InsertMenuItem_Separator( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex,
		const AMenuItemID inItemID )//#427
{
	OSStatus	err = noErr;
	
	AText	text("-");
	AStCreateCFStringFromAText	strref(text);
	err = ::InsertMenuItemTextWithCFString(inMenuRef,strref.GetRef(),inItemIndex,0,0);
	if( err != noErr )   _ACErrorNum("InsertMenuItemTextWithCFString() returned error: ",err,NULL);
	//MenuItemIDの設定
	err = ::SetMenuItemCommandID(inMenuRef,1+inItemIndex,inItemID);
	if( err != noErr )   _ACErrorNum("SetMenuItemCommandID() returned error: ",err,NULL);
}

/**
メニュー項目（セパレータ）追加
*/
void	ACarbonMenuWrapper::AddMenuItem_Separator( const ACarbonMenuRef inMenuRef, const AMenuItemID inItemID )//#427
{
	ACarbonMenuWrapper::InsertMenuItem_Separator(inMenuRef,ACarbonMenuWrapper::GetMenuItemCount(inMenuRef),inItemID);//#427
}

/**
メニュー項目削除
*/
void	ACarbonMenuWrapper::DeleteMenuItem( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex )
{
	//::DeleteMenuItem()でサブメニューが削除されるかどうか不明なので、とりあえず自前で全削除する。
	/*ACarbonMenuRef	subMenuRef = ACarbonMenuWrapper::GetSubMenu(inMenuRef,inItemIndex);
	if( subMenuRef != NULL )
	{
		ACarbonMenuWrapper::SetSubMenu(inMenuRef,inItemIndex,NULL);
		ACarbonMenuWrapper::DeleteAllMenuItems(subMenuRef);
		ACarbonMenuWrapper::DeleteMenu(subMenuRef);
	}*/
	//項目削除
	::DeleteMenuItem(inMenuRef,1+inItemIndex);
}

/**
メニュー項目全削除
*/
void	ACarbonMenuWrapper::DeleteAllMenuItems( const ACarbonMenuRef inMenuRef )
{
	while( ACarbonMenuWrapper::GetMenuItemCount(inMenuRef) > 0 )
	{
		ACarbonMenuWrapper::DeleteMenuItem(inMenuRef,0);
	}
}

/**
TextArrayからメニュー項目追加
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
TextArrayからメニュー項目設定
*/
void	ACarbonMenuWrapper::SetMenuItemsFromTextArray( const ACarbonMenuRef inMenuRef, const ATextArray& inTextArray )
{
	ACarbonMenuWrapper::DeleteAllMenuItems(inMenuRef);
	ACarbonMenuWrapper::AddMenuItemsFromTextArray(inMenuRef,inTextArray);
}

/**
メニュー項目にチェックマーク設定
*/
void	ACarbonMenuWrapper::SetCheckMark( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const ABool inCheck )
{
	::CheckMenuItem(inMenuRef,1+inItemIndex,inCheck);
}

/**
メニュー項目にショートカット設定
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
メニュー項目のチェックマークを全削除
*/
void	ACarbonMenuWrapper::ClearAllCheckMark( const ACarbonMenuRef inMenuRef )
{
	for( AIndex i = 0; i < ACarbonMenuWrapper::GetMenuItemCount(inMenuRef); i++ )
	{
		ACarbonMenuWrapper::SetCheckMark(inMenuRef,i,false);
	}
}

/**
引数inTextと一致するメニュー項目のインデックスを取得
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
フォントメニュー設定
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
	//#521 Disableフォントのメニュー項目は削除する
	for( AIndex index = preCount; index < ACarbonMenuWrapper::GetMenuItemCount(inMenuRef); )
	{
		//Font Number取得
		FMFontFamily	fontFamily = 0;
		FMFontStyle	style = 0;
		::GetFontFamilyFromMenuSelection(inMenuRef,1+index,&fontFamily,&style);
		//Font使用可能かチェック
		ATSUFontID	fontID = 0;
		if( ::ATSUFONDtoFontID(fontFamily,NULL,&fontID) == noErr )
		{
			//使用可能なら次へ
			index++;
		}
		else
		{
			//使用不可ならメニュー項目削除
			ACarbonMenuWrapper::DeleteMenuItem(inMenuRef,index);
		}
	}
	//
	AItemCount	added = ACarbonMenuWrapper::GetMenuItemCount(inMenuRef)-preCount;
	//MenuItemIDの設定
	for( AIndex i = 0; i < added; i++ )
	{
		::SetMenuItemCommandID(inMenuRef,1+inMenuItemIndex+i,inItemID);
	}
}

#pragma mark ===========================

#pragma mark ---コンテキストメニュー
#if SUPPORT_CARBON
/**
コンテキストメニュー登録
*/
ACarbonMenuRef	ACarbonMenuWrapper::RegisterContextMenu( AConstCharPtr inMenuName )
{
	OSStatus	err = noErr;
	
	//引数は"main/menuname"という形式なので、これをNibファイル名とMenu名に分解して、CFStringRefを生成する
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
	
	//メニュー生成
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
コンテキストメニュー表示
*/
void	ACarbonMenuWrapper::ShowContextMenu( const ACarbonMenuRef inMenuRef, const AGlobalPoint& inMousePoint, const AWindowRef inWindowRef )//win 080713 inWindowRefはWindows用
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
		//処理無し（メニューバーのメニューと同じルートで処理される）
	}
}

//#232
/**
メニューにアイコンを登録
*/
/*
void	ACarbonMenuWrapper::SetMenuItemIcon( const AControlID inID, const AIndex inIndex, const AIconID inIconID )
{
	::SetMenuItemIconHandle(GetMenuRef(inID),1+inIndex,kMenuIconRefType,(Handle)GetIconRef(inIconID));
	::EnableMenuItemIcon(GetMenuRef(inID),1+inIndex);
}
*/

//win
//メニュー項目アイコン設定
void	ACarbonMenuWrapper::SetMenuItemIcon( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const AIconID inIconID )
{
	//未対応
}
#endif
