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
#402

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATextFinder.h"

#pragma mark ===========================
#pragma mark [�N���X]ASameFolderLoader
/**
ASameFolderLoader�X���b�h�I�u�W�F�N�g
*/
class ASameFolderLoader : public AThread
{
  public:
	ASameFolderLoader( AObjectArrayItem* inParent );
	
  public:
	void	SPNVI_Run( const AIndex inSameFolderIndex, const AFileAcc& inFolder );
  private:
	void	NVIDO_ThreadMain();
	
	//�v���f�[�^
  private:
	AIndex						mSameFolderIndex;
	AFileAcc					mFolder;
};


#pragma mark ===========================
#pragma mark [�N���X]ASameFolderLoaderFactory
//
class ASameFolderLoaderFactory : public AThreadFactory
{
  public:
	ASameFolderLoaderFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	ASameFolderLoader*	Create() 
	{
		return new ASameFolderLoader(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};
