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

AWindow_TraceLog

*/

#include "stdafx.h"

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AWindow_TraceLog.h"
#include "AAppPrefDB.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_TraceLog
#pragma mark ===========================
//�g���[�X���O�E�C���h�E

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_TraceLog::AWindow_TraceLog():AWindow(/*#175 NULL*/)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kTraceLogWindowPosition);
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_TraceLog::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/TraceLog");
	//#688 NVI_RegisterTextEditPane(1,true,true,false,false);
	NVI_CreateEditBoxView(1,true,true,false);//#688
	AText	fontname;//win
	AFontWrapper::GetAppDefaultFontName(fontname);//win #375
	NVI_SetControlTextFont(1,/*win AFontWrapper::GetLogFont()*/fontname,9);
}

#pragma mark ===========================

#pragma mark ---���O�ǉ�

//���O�ǉ�
void	AWindow_TraceLog::SPI_AddLog( const AText& inText )
{
	AText	text;
	text.SetText(inText);
#if IMPLEMENTATION_FOR_WINDOWS
	text.ReplaceCstring("\r","\r\n");
#endif
	mText.AddText(text);//#695 �����f�[�^�Ɋi�[
	if( NVI_IsWindowVisible() == true )//#695 �������̂��߃E�C���h�E�\�����̏ꍇ�̂݃R���g���[�����Ƀe�L�X�g�ǉ�
	{
		NVI_AddControlText(1,text);
	}
}

//#695
/**
�E�C���h�E�\��������
*/
void	AWindow_TraceLog::NVIDO_Show()
{
	//�����f�[�^���R���g���[���ɐݒ�
	NVI_SetControlText(1,mText);
}

