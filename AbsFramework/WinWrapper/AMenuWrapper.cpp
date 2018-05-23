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

#include "stdafx.h"

#include "AMenuWrapper.h"
#include "../Frame.h"
#include "Commdlg.h"
#include <windowsx.h>
#include <commctrl.h>
#include <windows.h>
#include <strsafe.h>
#include <shellapi.h>
#include <shlobj.h>
#include <uxtheme.h>

AMenuRef	AMenuWrapper::sMainMenu = NULL;

void	AMenuWrapper::InitMenu()
{
	sMainMenu = ::LoadMenu(CAppImp::GetHResourceInstance(),MAKEINTRESOURCE(IDC_APPMENU));//satDLL
	//���j���[�I����WM_COMMAND�ł͂Ȃ�WM_MENUCOMMAND����M�ł���悤�ɁAMNS_NOTIFYBYPOS��ݒ肷��
	MENUINFO	menuinfo = {0};
	menuinfo.cbSize		= sizeof(menuinfo);
	menuinfo.fMask		= MIM_APPLYTOSUBMENUS|MIM_STYLE;
	menuinfo.dwStyle	= MNS_NOTIFYBYPOS;
	::SetMenuInfo(sMainMenu,&menuinfo);
	
	//Debug���j���[
	if( AApplication::GetApplication().NVI_GetEnableDebugMenu() == false )
	{
		AIndex	index = GetMenuItemCount(sMainMenu)-1;
		AText	text;
		GetMenuItemText(sMainMenu,index,text);
		if( text.Compare("DEBUG") == true )
		{
			DeleteMenuItem(sMainMenu,index);
		}
	}
	
	//�T�u���j���[����̍��ڂ�ID=0�ɐݒ肷��B
	SetID0ForSubMenuRoot(AMenuWrapper::GetRootMenu());
}

/**
���j���[�o�[�̃��j���[���ċA�I�Ɍ������āA�T�u���j���[���荀�ڂ����ׂ�ID=0�ɐݒ肷��
�y�ݒ藝�R�z
�E�ȑO�̏�����AMenuWrapper::GetMenuItemID()��0��Ԃ��Ă������A��������ƃc�[�����j���ł�Unrealize()�ō폜�ł��Ȃ��Ȃ�
�EAMenuManager::DisableAllMenuBarMenuItems()�̑ΏۊO�ɂ���B
�EADynamicMenu::Unrealize()�ł��܂��܃q�b�g���܂��̂�h���B
*/
void	AMenuWrapper::SetID0ForSubMenuRoot( const AMenuRef inMenuRef )
{
	for( AIndex index = 0; index < AMenuWrapper::GetMenuItemCount(inMenuRef); index++ )
	{
		AMenuRef	subMenuRef = AMenuWrapper::GetSubMenu(inMenuRef,index);
		if( subMenuRef != NULL )
		{
			SetMenuItemID(inMenuRef,index,0);
			SetID0ForSubMenuRoot(subMenuRef);
		}
	}
}

//���[�g���j���[�擾
AMenuRef	AMenuWrapper::GetRootMenu()
{
	return sMainMenu;
}

//���j���[����
AMenuRef	AMenuWrapper::CreateMenu()
{
	//return ::CreateMenu();CreateMenu()�̓��j���[�o�[�ɑ����H�|�b�v�A�b�v���j���[�Ő������\������Ȃ��B
	AMenuRef	menuRef = ::CreatePopupMenu();
	//���j���[�I����WM_COMMAND�ł͂Ȃ�WM_MENUCOMMAND����M�ł���悤�ɁAMNS_NOTIFYBYPOS��ݒ肷��
	MENUINFO	menuinfo = {0};
	menuinfo.cbSize		= sizeof(menuinfo);
	menuinfo.fMask		= MIM_APPLYTOSUBMENUS|MIM_STYLE;
	menuinfo.dwStyle	= MNS_NOTIFYBYPOS;
	::SetMenuInfo(menuRef,&menuinfo);
	return menuRef;
}

//���j���[�폜
void	AMenuWrapper::DeleteMenu( const AMenuRef inMenuRef )
{
	DeleteAllMenuItems(inMenuRef);//DynamicMenuID����̂���
	::DestroyMenu(inMenuRef);
}

//���j���[���ڐ��擾
AItemCount	AMenuWrapper::GetMenuItemCount( const AMenuRef inMenuRef )
{
	return ::GetMenuItemCount(inMenuRef);
}

//�T�u���j���[�擾
AMenuRef	AMenuWrapper::GetSubMenu( const AMenuRef inMenuRef, const AIndex inItemIndex )
{
	return ::GetSubMenu(inMenuRef,inItemIndex);
}

//�T�u���j���[�ݒ�
void	AMenuWrapper::SetSubMenu( const AMenuRef inMenuRef, const AIndex inItemIndex, const AMenuRef inSubMenuRef )
{
	MENUITEMINFOW	info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_SUBMENU;
	info.hSubMenu = inSubMenuRef;
	::SetMenuItemInfoW(inMenuRef,inItemIndex,TRUE,&info);
}

//MenuItemID�擾
AMenuItemID	AMenuWrapper::GetMenuItemID( const AMenuRef inMenuRef, const AIndex inItemIndex )
{
	//if( GetSubMenu(inMenuRef,inItemIndex) != NULL )   return 0;
	//���T�u���j���[�L���̔�������Ă������R���v���o���Ȃ��B
	//��L����������ƁAADynamicMenu::Unrealize()�ō폜�ΏۂɂȂ�Ȃ��̂Ŗ��B
	//AMenuManager::DisableAllMenuBarMenuItems()�ŃT�u���j���[�̐e���ڂ�disable�ΏۊO�ɂ��邽�߁H
	//AMenuManager::DisableAllMenuBarMenuItems()��ID=0�ȊO�̍��ڂ́Adisable�ɂȂ��Ă��܂��̂ŁA
	//�s��̒l�������Ă���ƁAEnable�ɂł��Ȃ��B
	//���\�[�X�G�f�B�^�ł̓T�u���j���[����̍��ڂɂ�ID�ݒ�s�B�������A�擾���Ă݂�ƕs��ȃf�[�^�������Ă���B
	//Mac�ł�ID�ݒ�s�B
	
	//
	MENUITEMINFOW	info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_ID;
	info.wID = 0;
	::GetMenuItemInfoW(inMenuRef,inItemIndex,TRUE,&info);
	return info.wID;
}

//MenuItemID�ݒ�
void	AMenuWrapper::SetMenuItemID( const AMenuRef inMenuRef, const AIndex inItemIndex, const AMenuItemID inMenuItemID )
{
	//
	MENUITEMINFOW	info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_ID;
	info.wID = inMenuItemID;
	::SetMenuItemInfoW(inMenuRef,inItemIndex,TRUE,&info);
}

//���j���[���ڃe�L�X�g�ݒ�
void	AMenuWrapper::SetMenuItemText( const AMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText )
{
	if( inText.Compare("-") == false )
	{
		AStCreateWcharStringFromAText	str(inText);
		MENUITEMINFOW	info;
		info.cbSize = sizeof(MENUITEMINFO);
		info.fMask = MIIM_STRING;
		info.dwTypeData = const_cast<LPWSTR>(str.GetPtr());
		::SetMenuItemInfoW(inMenuRef,inItemIndex,TRUE,&info);
	}
	else
	{
		MENUITEMINFOW	info;
		info.cbSize = sizeof(MENUITEMINFO);
		info.fMask = MIIM_FTYPE;
		info.fType = MFT_SEPARATOR;
		::SetMenuItemInfoW(inMenuRef,inItemIndex,TRUE,&info);
	}
}

//���j���[���ڃe�L�X�g�擾
void	AMenuWrapper::GetMenuItemText( const AMenuRef inMenuRef, const AIndex inItemIndex, AText& outText )
{
	outText.DeleteAll();
	int len = ::GetMenuStringW(inMenuRef,inItemIndex,NULL,0,MF_BYPOSITION);
	outText.Reserve(0,(len+1)*sizeof(wchar_t));
	{
		AStTextPtr	ptr(outText,0,outText.GetItemCount());
		::GetMenuStringW(inMenuRef,inItemIndex,reinterpret_cast<wchar_t*>(ptr.GetPtr()),static_cast<int>(ptr.GetByteCount()/sizeof(wchar_t)),MF_BYPOSITION);
	}
	outText.DeleteAfterUTF16NullChar();
	outText.ConvertToUTF8FromAPIUnicode();
}

//���j���[����Enable/Disable
void	AMenuWrapper::SetEnableMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex, const ABool inEnable )
{
	MENUITEMINFOW	info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	::GetMenuItemInfoW(inMenuRef,inItemIndex,TRUE,&info);
	if( inEnable == true )
	{
		info.fState &= (~MFS_DISABLED);//MFS_DISABLED:3
	}
	else
	{
		info.fState |= MFS_DISABLED;
	}
	::SetMenuItemInfoW(inMenuRef,inItemIndex,TRUE,&info);
}

//���j���[���ڒǉ��i�}���j
void	AMenuWrapper::InsertMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex, 
			const AText& inText, const AMenuItemID inItemID,
			const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	//
	{
		AText	text(inText);
		//
		if( inText.Compare("-") == false )
		{
			//�V���[�g�J�b�g�L�[�ݒ�iWindows�̏ꍇ�A�\���̂݁j
			if( inShortcutKey != 0 )
			{
				text.Add(kUChar_Tab);
				if( (inShortcutModifierKeys&kMenuShoftcutModifierKeysMask_Control) != 0 )
				{
					text.AddCstring("Ctrl+");
				}
				if( (inShortcutModifierKeys&kMenuShoftcutModifierKeysMask_Alt) != 0 )
				{
					text.AddCstring("Alt+");
				}
				if( (inShortcutModifierKeys&kMenuShoftcutModifierKeysMask_Shift) != 0 )
				{
					text.AddCstring("Shift+");
				}
				text.Add(inShortcutKey);
			}
			//
			AStCreateWcharStringFromAText	str(text);
			MENUITEMINFOW	info;
			info.cbSize = sizeof(MENUITEMINFO);
			info.fMask = MIIM_STRING | MIIM_ID;
			info.dwTypeData = const_cast<LPWSTR>(str.GetPtr());
			info.wID = inItemID;
			::InsertMenuItemW(inMenuRef,inItemIndex,TRUE,&info);
		}
		else
		{
			MENUITEMINFOW	info;
			info.cbSize = sizeof(MENUITEMINFO);
			info.fMask = MIIM_FTYPE;
			info.fType = MFT_SEPARATOR;
			::InsertMenuItemW(inMenuRef,inItemIndex,TRUE,&info);
		}
	}
}

//���j���[���ڒǉ�
/*
void	AMenuWrapper::AddMenuItem( const AMenuRef inMenuRef, const AText& inText, const AMenuItemID inItemID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	AMenuWrapper::InsertMenuItem(inMenuRef,AMenuWrapper::GetMenuItemCount(inMenuRef),inText,inItemID,inShortcutKey,inShortcutModifierKeys);
}
*/

//���j���[���ځi�Z�p���[�^�j�ǉ��i�}���j
void	AMenuWrapper::InsertMenuItem_Separator( const AMenuRef inMenuRef, const AIndex inItemIndex,
		const AMenuItemID inItemID )//#427
{
	MENUITEMINFOW	info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_FTYPE;
	info.fType = MFT_SEPARATOR;
	info.wID = inItemID;//#427
	::InsertMenuItemW(inMenuRef,inItemIndex,TRUE,&info);
}

//���j���[���ځi�Z�p���[�^�j�ǉ�
void	AMenuWrapper::AddMenuItem_Separator( const AMenuRef inMenuRef, const AMenuItemID inItemID )//#427
{
	AMenuWrapper::InsertMenuItem_Separator(inMenuRef,AMenuWrapper::GetMenuItemCount(inMenuRef),inItemID);//#427
}

//���j���[���ڍ폜
void	AMenuWrapper::DeleteMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex )
{
	//::DeleteMenuItem()�ŃT�u���j���[���폜����邩�ǂ����s���Ȃ̂ŁA�Ƃ肠�������O�őS�폜����B
	/*AMenuRef	subMenuRef = AMenuWrapper::GetSubMenu(inMenuRef,inItemIndex);
	if( subMenuRef != NULL )
	{
		AMenuWrapper::SetSubMenu(inMenuRef,inItemIndex,NULL);
		AMenuWrapper::DeleteAllMenuItems(subMenuRef);
		AMenuWrapper::DeleteMenu(subMenuRef);
	}*/
	//���ڍ폜
	::RemoveMenu(inMenuRef,inItemIndex,MF_BYPOSITION);
}

//���j���[���ڑS�폜
void	AMenuWrapper::DeleteAllMenuItems( const AMenuRef inMenuRef )
{
	while( AMenuWrapper::GetMenuItemCount(inMenuRef) > 0 )
	{
		AMenuWrapper::DeleteMenuItem(inMenuRef,0);
	}
}

//TextArray���烁�j���[���ڒǉ�
void	AMenuWrapper::AddMenuItemsFromTextArray( const AMenuRef inMenuRef, const ATextArray& inTextArray )
{
	for( AIndex index = 0; index < inTextArray.GetItemCount(); index++ )
	{
		AText	text;
		inTextArray.Get(index,text);
		AMenuWrapper::InsertMenuItem(inMenuRef,AMenuWrapper::GetMenuItemCount(inMenuRef),text,0,0,0);
	}
}

//TextArray���烁�j���[���ڐݒ�
void	AMenuWrapper::SetMenuItemsFromTextArray( const AMenuRef inMenuRef, const ATextArray& inTextArray )
{
	AMenuWrapper::DeleteAllMenuItems(inMenuRef);
	AMenuWrapper::AddMenuItemsFromTextArray(inMenuRef,inTextArray);
}

//���j���[���ڂɃ`�F�b�N�}�[�N�ݒ�
void	AMenuWrapper::SetCheckMark( const AMenuRef inMenuRef, const AIndex inItemIndex, const ABool inCheck )
{
	MENUITEMINFOW	info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	if( inCheck == true )
	{
		info.fState |= MFS_CHECKED;//MFS_CHECKED:8
	}
	else
	{
		info.fState &= (~MFS_CHECKED);
	}
	::SetMenuItemInfoW(inMenuRef,inItemIndex,TRUE,&info);
}

//���j���[���ڂɃV���[�g�J�b�g�ݒ�iWindows�̏ꍇ�A�\���̂݁j
void	AMenuWrapper::SetShortcut( const AMenuRef inMenuRef, const AIndex inItemIndex, const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	//���j���[������擾
	AText	text;
	GetMenuItemText(inMenuRef,inItemIndex,text);
	//�^�u�ȍ~���폜
	for( AIndex i = 0; i < text.GetItemCount(); i++ )
	{
		if( text.Get(i) == kUChar_Tab )
		{
			text.DeleteAfter(i);
			break;
		}
	}
	//�V���[�g�J�b�g�L�[�ݒ�iWindows�̏ꍇ�A�\���̂݁j
	if( inShortcutKey != 0 )
	{
		text.Add(kUChar_Tab);
		if( (inShortcutModifierKeys&kMenuShoftcutModifierKeysMask_Control) != 0 )
		{
			text.AddCstring("Ctrl+");
		}
		if( (inShortcutModifierKeys&kMenuShoftcutModifierKeysMask_Alt) != 0 )
		{
			text.AddCstring("Alt+");
		}
		if( (inShortcutModifierKeys&kMenuShoftcutModifierKeysMask_Shift) != 0 )
		{
			text.AddCstring("Shift+");
		}
		text.Add(inShortcutKey);
	}
	//���j���[
	SetMenuItemText(inMenuRef,inItemIndex,text);
}

//���j���[���ڂ̃`�F�b�N�}�[�N��S�폜
void	AMenuWrapper::ClearAllCheckMark( const AMenuRef inMenuRef )
{
	for( AIndex i = 0; i < AMenuWrapper::GetMenuItemCount(inMenuRef); i++ )
	{
		AMenuWrapper::SetCheckMark(inMenuRef,i,false);
	}
}

//����inText�ƈ�v���郁�j���[���ڂ̃C���f�b�N�X���擾
AIndex	AMenuWrapper::GetMenuItemIndexByText( const AMenuRef inMenuRef, const AText& inText )
{
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
}
/*win
//
AIndex	AMenuWrapper::GetMenuItemIndexByFont( const AMenuRef inMenuRef, const AFont inFont )
{
	for( AIndex i = 0; i < AMenuWrapper::GetMenuItemCount(inMenuRef); i++ )
	{
		if( AMenuWrapper::GetFontByMenuItemIndex(inMenuRef,i) == inFont )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

//
AFont	AMenuWrapper::GetFontByMenuItemIndex( const AMenuRef inMenuRef, const AIndex inMenuItem )
{
	if( inMenuItem < 0 || inMenuItem >= AMenuWrapper::GetMenuItemCount(inMenuRef) )   return 0;
	_ACError("not implemented",NULL);
	return 0;
}
*/
void	AMenuWrapper::InsertFontMenu( const AMenuRef inMenuRef, const AIndex inMenuItemIndex, const AMenuItemID inItemID )
{
	const ATextArray&	fontarray = CWindowImp::GetFontNameArray();
	for( AIndex i = 0; i < fontarray.GetItemCount(); i++ )
	{
		InsertMenuItem(inMenuRef,inMenuItemIndex,fontarray.GetTextConst(i),inItemID,0,0);
	}
}

AMenuRef	AMenuWrapper::RegisterContextMenu( AConstCharPtr inMenuName )
{
	AText	name;
	//CAppImp::GetLangText(name);
	name.SetCstring("jp_");
	name.AddCstring(inMenuName);
	name.ReplaceChar('/','_');
	AStCreateWcharStringFromAText	windowname(name);
	AMenuRef	menuref = ::LoadMenuW(CAppImp::GetHResourceInstance(),windowname.GetPtr());//satDLL
	AMenuRef	submenu = ::GetSubMenu(menuref,0);
	//���j���[�I����WM_COMMAND�ł͂Ȃ�WM_MENUCOMMAND����M�ł���悤�ɁAMNS_NOTIFYBYPOS��ݒ肷��
	MENUINFO	menuinfo = {0};
	menuinfo.cbSize		= sizeof(menuinfo);
	menuinfo.fMask		= MIM_APPLYTOSUBMENUS|MIM_STYLE;
	menuinfo.dwStyle	= MNS_NOTIFYBYPOS;
	::SetMenuInfo(submenu,&menuinfo);
	return submenu;
	//���b�� menu�폜�K�v
}

void	AMenuWrapper::ShowContextMenu( const AMenuRef inMenuRef, const AGlobalPoint& inMousePoint, const AWindowRef inWindowRef )
{
	::TrackPopupMenu(inMenuRef,TPM_LEFTALIGN,inMousePoint.x,inMousePoint.y,0,inWindowRef,NULL);
}

#pragma mark ===========================

#pragma mark --- ���j���[�ւ̃A�C�R���ǉ�

/*
���j���[�ւ̃A�C�R���ǉ��͕W��API�ŗp�ӂ���Ă��Ȃ��̂ŁA���L�L�ڂ̃R�[�h���g�p����B
http://msdn.microsoft.com/en-us/library/bb757020.aspx

DrawIconEx�̕`��T�C�Y��16*16�Œ�ɕύX
*/

typedef DWORD ARGB;

void InitBitmapInfo(__out_bcount(cbInfo) BITMAPINFO *pbmi, ULONG cbInfo, LONG cx, LONG cy, WORD bpp)
{
	ZeroMemory(pbmi, cbInfo);
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biCompression = BI_RGB;
	
	pbmi->bmiHeader.biWidth = cx;
	pbmi->bmiHeader.biHeight = cy;
	pbmi->bmiHeader.biBitCount = bpp;
}


HRESULT Create32BitHBITMAP(HDC hdc, const SIZE *psize, __deref_opt_out void **ppvBits, __out HBITMAP* phBmp)
{
	*phBmp = NULL;
	
	BITMAPINFO bmi;
	InitBitmapInfo(&bmi, sizeof(bmi), psize->cx, psize->cy, 32);
	
	HDC hdcUsed = hdc ? hdc : GetDC(NULL);
	if (hdcUsed)
	{
		*phBmp = CreateDIBSection(hdcUsed, &bmi, DIB_RGB_COLORS, ppvBits, NULL, 0);
		if (hdc != hdcUsed)
		{
			ReleaseDC(NULL, hdcUsed);
		}
	}
	return (NULL == *phBmp) ? E_OUTOFMEMORY : S_OK;
}

HRESULT AddBitmapToMenuItem(HMENU hmenu, int iItem, BOOL fByPosition, HBITMAP hbmp)
{
	HRESULT hr = E_FAIL;
	
	MENUITEMINFO mii = { sizeof(mii) };
	mii.fMask = MIIM_BITMAP;
	mii.hbmpItem = hbmp;
	if (SetMenuItemInfo(hmenu, iItem, fByPosition, &mii))
	{
		hr = S_OK;
	}
	
	return hr;
}

HRESULT ConvertToPARGB32(HDC hdc, __inout ARGB *pargb, HBITMAP hbmp, SIZE& sizImage, int cxRow)
{
	BITMAPINFO bmi;
	InitBitmapInfo(&bmi, sizeof(bmi), sizImage.cx, sizImage.cy, 32);
	
	HRESULT hr = E_OUTOFMEMORY;
	HANDLE hHeap = GetProcessHeap();
	void *pvBits = HeapAlloc(hHeap, 0, bmi.bmiHeader.biWidth * 4 * bmi.bmiHeader.biHeight);
	if (pvBits)
	{
		hr = E_UNEXPECTED;
		if (GetDIBits(hdc, hbmp, 0, bmi.bmiHeader.biHeight, pvBits, &bmi, DIB_RGB_COLORS) == bmi.bmiHeader.biHeight)
		{
			ULONG cxDelta = cxRow - bmi.bmiHeader.biWidth;
			ARGB *pargbMask = static_cast<ARGB *>(pvBits);
			
			for (ULONG y = bmi.bmiHeader.biHeight; y; --y)
			{
				for (ULONG x = bmi.bmiHeader.biWidth; x; --x)
				{
					if (*pargbMask++)
					{
						// transparent pixel
						*pargb++ = 0;
					}
					else
					{
						// opaque pixel
						*pargb++ |= 0xFF000000;
					}
				}
				
				pargb += cxDelta;
			}
			
			hr = S_OK;
		}
		
		HeapFree(hHeap, 0, pvBits);
	}
	
	return hr;
}

bool HasAlpha(__in ARGB *pargb, SIZE& sizImage, int cxRow)
{
	ULONG cxDelta = cxRow - sizImage.cx;
	for (ULONG y = sizImage.cy; y; --y)
	{
		for (ULONG x = sizImage.cx; x; --x)
		{
			if (*pargb++ & 0xFF000000)
			{
				return true;
			}
		}
		
		pargb += cxDelta;
	}
	
	return false;
}

HRESULT ConvertBufferToPARGB32(HPAINTBUFFER hPaintBuffer, HDC hdc, HICON hicon, SIZE& sizIcon)
{
	RGBQUAD *prgbQuad;
	int cxRow;
	HRESULT hr = GetBufferedPaintBits(hPaintBuffer, &prgbQuad, &cxRow);
	if (SUCCEEDED(hr))
	{
		ARGB *pargb = reinterpret_cast<ARGB *>(prgbQuad);
		if (!HasAlpha(pargb, sizIcon, cxRow))
		{
			ICONINFO info;
			if (GetIconInfo(hicon, &info))
			{
				if (info.hbmMask)
				{
					hr = ConvertToPARGB32(hdc, pargb, info.hbmMask, sizIcon, cxRow);
				}
				
				DeleteObject(info.hbmColor);
				DeleteObject(info.hbmMask);
			}
		}
	}
	
	return hr;
}

HRESULT AddIconToMenuItem(HMENU hmenu, int iMenuItem, BOOL fByPosition, HICON hicon, BOOL fAutoDestroy, __out_opt HBITMAP *phbmp)
{
	HRESULT hr = E_OUTOFMEMORY;
	HBITMAP hbmp = NULL;
	
	SIZE sizIcon;
	sizIcon.cx = GetSystemMetrics(SM_CXSMICON);
	sizIcon.cy = GetSystemMetrics(SM_CYSMICON);
	
	RECT rcIcon;
	SetRect(&rcIcon, 0, 0, sizIcon.cx, sizIcon.cy);
	
	HDC hdcDest = CreateCompatibleDC(NULL);
	if (hdcDest)
	{
		hr = Create32BitHBITMAP(hdcDest, &sizIcon, NULL, &hbmp);
		if (SUCCEEDED(hr))
		{
			hr = E_FAIL;
			
			HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcDest, hbmp);
			if (hbmpOld)
			{
				BLENDFUNCTION bfAlpha = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
				BP_PAINTPARAMS paintParams = {0};
				paintParams.cbSize = sizeof(paintParams);
				paintParams.dwFlags = BPPF_ERASE;
				paintParams.pBlendFunction = &bfAlpha;
				
				HDC hdcBuffer;
				HPAINTBUFFER hPaintBuffer = BeginBufferedPaint(hdcDest, &rcIcon, BPBF_DIB, &paintParams, &hdcBuffer);
				if (hPaintBuffer)
				{
					if (DrawIconEx(hdcBuffer, 0, 0, hicon, /*sizIcon.cx, sizIcon.cy*/16,16, 0, NULL, DI_NORMAL))
					{
						// If icon did not have an alpha channel, we need to convert buffer to PARGB.
						hr = ConvertBufferToPARGB32(hPaintBuffer, hdcDest, hicon, sizIcon);
					}
					
					// This will write the buffer contents to the
					// destination bitmap.
					EndBufferedPaint(hPaintBuffer, TRUE);
				}
				
				SelectObject(hdcDest, hbmpOld);
			}
		}
		
		DeleteDC(hdcDest);
	}
	
	if (SUCCEEDED(hr))
	{
		hr = AddBitmapToMenuItem(hmenu, iMenuItem, fByPosition, hbmp);
	}
	
	if (FAILED(hr))
	{
		DeleteObject(hbmp);
		hbmp = NULL;
	}
	
	if (fAutoDestroy)
	{
		DestroyIcon(hicon);
	}
	
	if (phbmp)
	{
		*phbmp = hbmp;
	}
	
	return hr;
}

//-----------------------------------------------------------------

//-- TrackPopupIconMenuEx -----------------------------------------
// Wrapper to TrackPopupMenuEx that shows how to use AddIconToMenuItem and how to manage the lifetime of the created bitmaps

struct ICONMENUENTRY
{
	UINT        idMenuItem;
	HINSTANCE   hinst;
	LPCWSTR     pIconId;
};

BOOL TrackPopupIconMenuEx(HMENU hmenu, UINT fuFlags, int x, int y, HWND hwnd, __in_opt LPTPMPARAMS lptpm,
			UINT nIcons, __in_ecount_opt(nIcons) const ICONMENUENTRY *pIcons)
{
	HRESULT hr = S_OK;
	BOOL fRet;
	
	MENUINFO menuInfo = { sizeof(menuInfo) };
	menuInfo.fMask = MIM_STYLE;
	
	if (nIcons)
	{
		for (UINT n = 0; SUCCEEDED(hr) && n < nIcons; ++n)
		{
			HICON hicon;
			hr = LoadIconMetric(pIcons[n].hinst, pIcons[n].pIconId, LIM_SMALL, &hicon);
			if (SUCCEEDED(hr))
			{
				hr = AddIconToMenuItem(hmenu, pIcons[n].idMenuItem, FALSE, hicon, TRUE, NULL);
			}
		}
		
		GetMenuInfo(hmenu, &menuInfo);
		
		MENUINFO menuInfoNew = menuInfo;
		menuInfoNew.dwStyle = (menuInfo.dwStyle & ~MNS_NOCHECK) | MNS_CHECKORBMP;
		SetMenuInfo(hmenu, &menuInfoNew);
	}
	
	if (SUCCEEDED(hr))
	{
		fRet = TrackPopupMenuEx(hmenu, fuFlags, x, y, hwnd, lptpm) ? S_OK : E_FAIL;
		hr = fRet ? S_OK : E_FAIL;
	}
	
	if (nIcons)
	{
		for (UINT n = 0; n < nIcons; ++n)
		{
			MENUITEMINFO mii = { sizeof(mii) };
			mii.fMask = MIIM_BITMAP;
			
			if (GetMenuItemInfo(hmenu, pIcons[n].idMenuItem, FALSE, &mii))
			{
				DeleteObject(mii.hbmpItem);
				mii.hbmpItem = NULL;
				SetMenuItemInfo(hmenu, pIcons[n].idMenuItem, FALSE, &mii);
			}
		}
		
		SetMenuInfo(hmenu, &menuInfo);
	}
	
	return SUCCEEDED(hr) ? fRet : FALSE;
}

//���j���[���ڃA�C�R���ݒ�
void	AMenuWrapper::SetMenuItemIcon( const AMenuRef inMenuRef, const AIndex inItemIndex, const AIconID inIconID )
{
	AOSVersion	ver = AEnvWrapper::GetOSVersion();
	if( ver >= kOSVersion_WindowsVista )
	{
		AddIconToMenuItem(inMenuRef,inItemIndex,TRUE,CWindowImp::GetHIconByIconID(inIconID),FALSE,NULL);
	}
	else
	{
		//XP�͖��Ή�
	}
}


