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

AProjectPrefDB
RC3
*/

#pragma once

#include "../../AbsFramework/Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AProjectPrefDB
//プロジェクト設定DB
class AProjectPrefDB : public ADataBase
{
  public:
	AProjectPrefDB( AObjectArrayItem* inParent, const AText& inProjectFilePath );
	
	void					Load();
	void					Save();
	
  public:
	const static APrefID				kExternalComment_PathAndModuleName		= 1001;
	const static APrefID				kExternalComment_Comment				= 1002;
	
	//プロジェクトパス
  public:
	const AText&			GetProjectFilePath() const { return mProjectFilePath; }
	void					GetProjectFolderPath( AText& outFolderPath ) const;//#138
  private:
	AText								mProjectFilePath;
};

#pragma mark ===========================
#pragma mark [クラス]AProjectPrefDBFactory
//プロジェクト設定DB作成用AbstructFactory
class AProjectPrefDBFactory : public  AAbstractFactoryForObjectArray<AProjectPrefDB>
{
  public:
	AProjectPrefDBFactory( AObjectArrayItem* inParent, const AText& inPath ) : mParent(inParent), mPath(inPath)
	{
	}
	AProjectPrefDB*	Create()
	{
		return new AProjectPrefDB(mParent,mPath);
	}
  private:
	AObjectArrayItem* mParent;
	const AText&	mPath;
};

