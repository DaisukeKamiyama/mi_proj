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

AWindow_KeyActionList
#738

*/

#include "stdafx.h"

#include "AWindow_KeyActionList.h"
#include "AView_DocInfo.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"

//#725
//ListView
const AControlID	kControlID_ListView = 101;


#pragma mark ===========================
#pragma mark [�N���X]AWindow_KeyActionList
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AWindow_KeyActionList::AWindow_KeyActionList():AWindow()
{
}
/**
�f�X�g���N�^
*/
AWindow_KeyActionList::~AWindow_KeyActionList()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�R���g���[���̃N���b�N���̃R�[���o�b�N
*/
ABool	AWindow_KeyActionList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	return result;
}

/**
�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_KeyActionList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}


#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_KeyActionList::NVIDO_Create( const ADocumentID inDocumentID )
{
	//
	NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_Bottom,true,false,false,false,false,true,true);
	
	//ListView����
	{
		AWindowPoint	pt = {0,0};
		AListViewFactory	listViewFactory(GetObjectID(),kControlID_ListView);
		NVM_CreateView(kControlID_ListView,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,listViewFactory);
		NVI_SetControlBindings(kControlID_ListView,true,true,true,true,false,false);
		
		//
		AFileAcc	folder;
		GetApp().SPI_GetLocalizedDataFolder(folder);
		AFileAcc	file;
		file.SpecifyChild(folder,"keyactions.csv");
		AText	text;
		file.ReadTo(text);
		//
		ACSVFile	csvFile(text,ATextEncodingWrapper::GetUTF8TextEncoding());
		ATextArray	titleArray,toolCommandArray;
		ANumberArray	outlineLevelArray, actionNumberArray;
		csvFile.GetColumn(0,outlineLevelArray);
		csvFile.GetColumn(1,titleArray);
		csvFile.GetColumn(2,actionNumberArray);
		csvFile.GetColumn(3,toolCommandArray);
		
		//
		NVI_GetListView(kControlID_ListView).SPI_RegisterColumn_Text(0,0,"",false);
		NVI_GetListView(kControlID_ListView).SPI_BeginSetTable();
		NVI_GetListView(kControlID_ListView).SPI_SetColumn_Text(0,titleArray);
		NVI_GetListView(kControlID_ListView).SPI_SetTable_OutlineFoldingLevel(outlineLevelArray);
		NVI_GetListView(kControlID_ListView).SPI_EndSetTable();
	}
	
}

/**
�V�K�^�u����
�����F��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
*/
void	AWindow_KeyActionList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
}

/**
Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
*/
void	AWindow_KeyActionList::NVIDO_Hide()
{
}

/**
*/
void	AWindow_KeyActionList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	NVI_RefreshWindow();
}

/**
*/
void	AWindow_KeyActionList::UpdateViewBounds()
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//list view�T�C�Y
	AWindowPoint	listViewPoint = {0,0};
	NVI_SetControlPosition(kControlID_ListView,listViewPoint);
	NVI_SetControlSize(kControlID_ListView,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top);
}




