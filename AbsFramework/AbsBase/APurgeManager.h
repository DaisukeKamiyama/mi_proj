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

AArray

*/

//#693
//★未レビュー・未テスト

#pragma once

#include "ABaseTypes.h"
#include "../AbsBase_Imp/ABaseWrapper.h"

class APurgeManagerData;

#pragma mark ===========================
#pragma mark [クラス]APurgeManager

/**
*/
class APurgeManager
{
	//コンストラクタ・デストラクタ
  public:
	APurgeManager();
	~APurgeManager();
	
	//
  public:
	void	Purge( const void* inKeyMemoryPtr, void* inPurgeMemoryPtr, const AByteCount inByteCount );
	void*	RestoreFromPurge( const void* inKeyMemoryPtr );
	void	DeletePurge( const void* inKeyMemoryPtr );
	
	//
  private:
	APurgeManagerData*	mPurgeManagerData;
	
	//
  public:
	static APurgeManager&	GetPurgeManager()
	{
		//未生成なら生成する #941
		if( sPurgeManager == NULL )
		{
			new APurgeManager;
		}
		return (*sPurgeManager);
	}
  private:
	static APurgeManager*	sPurgeManager;
	
};

