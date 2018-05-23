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

ASameFolderLoader

*/

#include "stdafx.h"

#include "ASameFolderLoader.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]ASameFolderLoader
#pragma mark ===========================
//�}���`�t�@�C���������s

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
ASameFolderLoader::ASameFolderLoader( AObjectArrayItem* inParent ) : AThread(inParent), mSameFolderIndex(kIndex_Invalid)
{
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

/**
�X���b�h���C�����[�`��
*/
void	ASameFolderLoader::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ASameFolderLoader::NVIDO_ThreadMain started.",this);
	try
	{
		//2�b�ԃE�G�C�g
		GetApp().NVI_SleepWithTimer(2);
		
		//�t�H���_�����݂��Ă����瓯���t�H���_�̃f�[�^���\��
		if( mFolder.Exist() == true )
		{
			GetApp().SPI_MakeFilenameListSameFolder(mSameFolderIndex,mFolder);
		}
	}
	catch(...)
	{
		_ACError("ASameFolderLoader::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	//�t�H���_�p�X�擾
	AText	folderpath;
	mFolder.GetPath(folderpath);
	//�����ʒm
	AObjectIDArray	objectIDArray;
	ABase::PostToMainInternalEventQueue(kInternalEvent_SameFolderLoaderCompleted,
										GetObjectID(),0,folderpath,objectIDArray);
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ASameFolderLoader::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---�X���b�h�N��

/**
�X���b�h�N��
*/
void	ASameFolderLoader::SPNVI_Run( const AIndex inSameFolderIndex, const AFileAcc& inFolder )
{
	if( NVI_IsThreadWorking() == true )   {_ACError("",this);return;}
	
	//�v���f�[�^���R�s�[
	mSameFolderIndex = inSameFolderIndex;
	mFolder = inFolder;
	
	//�X���b�h�N��
	NVI_Run();
}

