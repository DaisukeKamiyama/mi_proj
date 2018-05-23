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

ASingletonObject

*/

#pragma once

#include "../AbsBase/ABaseTypes.h"

#pragma mark ===========================
#pragma mark [クラス]ASingletonObject
/**
アプリケーションに唯一しかないオブジェクトのための基底クラス
コピー／代入生成子、new/delet演算子を使用不可能にする
*/
class ASingletonObject
{
	//コンストラクタ／デストラクタ
  public:
	ASingletonObject() {};
	virtual ~ASingletonObject() {};
	
	//コピー生成子／代入演算子／newはサブクラスで明示的に定義されない限り、使用不可。
  protected:
	ASingletonObject( const ASingletonObject& );//コピー生成子
	ASingletonObject&	operator=( const ASingletonObject& );//代入演算子
	static void*	operator new(size_t);//new演算子
	static void*	operator new[](size_t);//new[]演算子
	//static void	operator delete(void*);//delete演算子
	//static void	operator delete[](void*);//delete[]演算子
};




