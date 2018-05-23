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

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATextFinder.h"

#pragma mark ===========================
#pragma mark [クラス]ACSVKeywordLoader
/**
ACSVKeywordLoaderスレッドオブジェクト
*/
class ACSVKeywordLoader : public AThread
{
  public:
	ACSVKeywordLoader( AObjectArrayItem* inParent );
	
  public:
	void	SPNVI_Run( const AIndex inModeIndex, const AText& inCSVText, const AIndex inCategoryIndex );
  private:
	void	NVIDO_ThreadMain();
	
	//要求データ
  private:
	AIndex						mModeIndex;
	AText						mCSVText;
	AIndex						mCategoryIndex;
};


#pragma mark ===========================
#pragma mark [クラス]ACSVKeywordLoaderFactory
//
class ACSVKeywordLoaderFactory : public AThreadFactory
{
  public:
	ACSVKeywordLoaderFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	ACSVKeywordLoader*	Create() 
	{
		return new ACSVKeywordLoader(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};
