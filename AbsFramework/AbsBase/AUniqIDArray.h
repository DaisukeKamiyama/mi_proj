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

AUniqIDArray
#693

*/

#pragma once

#include "ABaseTypes.h"
#include "AObjectArrayItem.h"
#include "AHashArray.h"
#include "AStArrayPtr.h"

/**
一意となるIDを生成し、Index番号と結び付けを行う配列クラス
*/
class AUniqIDArray
{
	//コンストラクタ・デストラクタ
  public:
	AUniqIDArray( const AItemCount inInitialAllocateCount = 64, const AItemCount inReallocateStepCount = 64 );
	~AUniqIDArray();
	
	//UniqID生成・削除（+Index番号との結びつけ）
  public:
	void				InsertItems( const AIndex inIndex, const AItemCount inInsertCount );
	void				Insert1Item( const AIndex inIndex );
	AIndex				AddItem();
	void				DeleteItems( const AIndex inIndex, const AItemCount inDeleteCount );
	void				Delete1Item( const AIndex inIndex );
	void				DeleteAll();
  private:
	void				AssignUniqIDs( const AIndex inIndex, const AItemCount inCount );
	
	//取得・検索
  public:
	AUniqID				Get( const AIndex inIndex ) const;
	AIndex				Find( const AUniqID inUniqID ) const;
	
	//UnusedItem管理（削除マーク項目）
  public:
	AIndex				ReuseUnusedItem();
	void				Unuse1Item( AIndex inIndex );
	
	/**
	メモリ拡大時の再割り当て増加量の設定
	*/
  public:
	void	SetReallocateStep( const AItemCount inReallocateCount )
	{
		mUniqIDArray.SetReallocateStep(inReallocateCount);
		mUniqIDToIndexArray.SetReallocateStep(inReallocateCount);
	}
	
	//データ
  private:
	//主配列
	//UniqIDを格納する配列
	AArray<AUniqID>						mUniqIDArray;
	
	//補助配列
	//index:UniqID順  中身:主配列のindex
	AArray<AIndex>						mUniqIDToIndexArray;
	static const ANumber				kUniqIDValMin = 1;
	
	//mUnusedItemIndexArray: Unuseマークをつけた項目の配列indexを格納する
	AArray<AIndex>						mUnusedItemIndexArray;
	//メモリ割り当てパラメータ
	static const AItemCount				kUnusedItemIndexArray_InitialAllocateCount = 10000;
	static const AItemCount				kUnusedItemIndexArray_ReallocateStepCount = 10000;
	
	//mUnusignedUniqIDValArray: mUniqIDToIndexArrayに存在するUniqIDのうち未割り当て（値がkIndex_Invalid）のものを格納する
	AArray<ANumber>						mUnusignedUniqIDValArray;
	//メモリ割り当てパラメータ
	static const AItemCount				kUnusignedUniqIDValArray_InitialAllocateCount = 10000;
	static const AItemCount				kUnusignedUniqIDValArray_ReallocateStepCount = 10000;
	
};

