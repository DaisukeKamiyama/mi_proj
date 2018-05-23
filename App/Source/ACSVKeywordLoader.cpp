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

ACSVKeywordLoader

*/

#include "stdafx.h"

#include "ACSVKeywordLoader.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]ACSVKeywordLoader
#pragma mark ===========================
//�}���`�t�@�C���������s

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
ACSVKeywordLoader::ACSVKeywordLoader( AObjectArrayItem* inParent ) : AThread(inParent), mModeIndex(kIndex_Invalid), mCategoryIndex(kIndex_Invalid)
{
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

/**
�X���b�h���C�����[�`��
*/
void	ACSVKeywordLoader::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ACSVKeywordLoader::NVIDO_ThreadMain started.",this);
	try
	{
		GetApp().SPI_GetModePrefDB(mModeIndex).AddCategoryKeywordListFromCSV(mCSVText,mCategoryIndex,mAborted);
	}
	catch(...)
	{
		_ACError("ACSVKeywordLoader::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	//�����ʒm
	AObjectIDArray	objectIDArray;
	ABase::PostToMainInternalEventQueue(kInternalEvent_CSVKeywordLoaderCompleted,
										GetObjectID(),0,GetEmptyText(),objectIDArray);
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ACSVKeywordLoader::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---�X���b�h�N��

/**
�X���b�h�N��
*/
void	ACSVKeywordLoader::SPNVI_Run( const AIndex inModeIndex, const AText& inCSVText, const AIndex inCategoryIndex )
{
	if( NVI_IsThreadWorking() == true )   {_ACError("",this);return;}
	
	//�v���f�[�^���R�s�[
	mModeIndex = inModeIndex;
	mCSVText.SetText(inCSVText);
	mCategoryIndex = inCategoryIndex;
	
	//�X���b�h�N��
	NVI_Run();
}

