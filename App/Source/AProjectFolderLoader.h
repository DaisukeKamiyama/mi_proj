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

AProjectFolderLoader
#402

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATextFinder.h"

#pragma mark ===========================
#pragma mark [クラス]AProjectFolderLoader
/**
ProjectFolderLoaderスレッドオブジェクト
*/
class AProjectFolderLoader : public AThread
{
  public:
	AProjectFolderLoader( AObjectArrayItem* inParent );
	
  public:
	void	SPNVI_Run( const AFileAcc& inProjectFolder, const ABool inDontDisplayInvisible );
	void	SPNVI_Run( const AObjectArray<AFileAcc>& inProjectFolderArray, const ABool inDontDisplayInvisible );
  private:
	void	NVIDO_ThreadMain();
	void	LoadFolderRecursive( const AIndex inRequestIndex, const AFileAcc& inFolder, const AIndex inDepth );
	
	//結果取得
  public:
	void	SPI_GetRequestedData( ATextArray& outProjectFolderArray ) const;
	void	SPI_GetResult( const AIndex inRequestIndex, ATextArray& outResult ) const;
	void	SPI_GetLoadedFolders( AObjectArray<AFileAcc>& outFolders ) const;
	
	//要求データ
  private:
	ATextArray					mRequestArray_ProjectFolder;
	ABool						mDontDisplayInvisible;
	
	//結果
  private:
	AObjectArray<ATextArray>	mResult;
	AObjectArray<AFileAcc>		mFolders;
};


#pragma mark ===========================
#pragma mark [クラス]AProjectFolderLoaderFactory
//
class AProjectFolderLoaderFactory : public AThreadFactory
{
  public:
	AProjectFolderLoaderFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	AProjectFolderLoader*	Create() 
	{
		return new AProjectFolderLoader(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};
