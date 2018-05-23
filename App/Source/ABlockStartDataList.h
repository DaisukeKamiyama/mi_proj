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

ABlockStartDataList
#695

*/

#pragma once

#include "../../AbsFramework/Frame.h"

#pragma mark ===========================
#pragma mark [クラス]ABlockStartDataList
class ABlockStartDataList : public AObjectArrayItem
{
	//コンストラクタ、デストラクタ
  public:
	ABlockStartDataList() : AObjectArrayItem(NULL) {}
	~ABlockStartDataList() {}
	
	//追加・削除
  public:
	AUniqID					Add( const AUniqID inPreviousID, const AIndex inVariableIndex, const AItemCount inIndentCount );
	void					DeleteIdentifiers( const AUniqID inFirstUniqID );
	void					DeleteAll();
	
	//取得
  public:
	AIndex					GetVariableIndex( const AUniqID inUniqID ) const;
	AItemCount				GetIndentCount( const AUniqID inUniqID ) const;
	AUniqID					GetNextUniqID( const AUniqID inUniqID ) const;
	
	AItemCount				GetItemCount() const { return mVariableIndexArray.GetItemCount(); }
	
	//データ
  private:
	AUniqIDArray						mUniqIDArray;
	AArray<AIndex>						mVariableIndexArray;
	AArray<AItemCount>					mIndentCountArray;
	AArray<AUniqID>						mNextUniqIDArray;
};

