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

ABaseFunction

*/

#pragma once

#include "ABaseTypes.h"

class AObjectArrayItem;
class AArrayAbstract;

#pragma mark ===========================
#pragma mark [クラス]ABaseFunction
/**
AbsBaseの全体的な機構に対する制御を行う関数群
*/
class ABaseFunction
{
	//現在メインスレッド内かどうかの判定
  public:
	static ABool	InMainThread();
	
	//AObjectArrayItemゴミ箱制御
  public:
	static void	AddToObjectArrayItemTrash( AObjectArrayItem* inObjectPtr );
	static void	DoObjectArrayItemTrashDelete();
	
	/*#344 メモリープール機構削除（仮想メモリ環境では使用される状況がまずありえないので）
	//メモリ管理
  public:
	static void	ShowLowMemoryAlertLevel1();
	static ABool	IsLevel1MemoryPoolReleased();
	static ABool	IsLevel2MemoryPoolReleased();
	static void	ReleaseMemoryPoolLevel1();
	static void	ReleaseMemoryPoolLevel2();
	*/
	
	//エラー発生時処理（実際には__FILE__や__LINEを使用するため、本ファイル最後の#defineを使用する。）
  public:
	//致命的エラー
	static void	CriticalError( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem );
	static void	CriticalError( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, 
				const ANumber inErrorNumber, const AObjectArrayItem* inObjectArrayItem );
	//致命的Throw
	static void	CriticalThrow( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem );
	//エラー発生
	static void	Error( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem );
	//Throw発生
	static void	Throw( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem );
	//単なる情報ログのため（エラーではない）
	static void	Info( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem );
	
	static void ShowDebugger();

	//ユニットテスト
  public:
	static ABool	BaseComponentUnitTest();
	static void	LowMemoryTest();
	
	//正規表現テスト
  public:
	static ABool	TestRegExp();
	
	//統計情報 #271
  public:
	static void	HashStatics();
	
};

//エラー発生時処理
#define _AError(x,y) (ABaseFunction::Error(__FILE__,__LINE__,x,y))
#define _AThrow(x,y) (ABaseFunction::Throw(__FILE__,__LINE__,x,y))
#define _ACError(x,y) (ABaseFunction::CriticalError(__FILE__,__LINE__,x,y))
#define _ACErrorNum(x,n,y) (ABaseFunction::CriticalError(__FILE__,__LINE__,x,n,y))
#define _ACThrow(x,y) (ABaseFunction::CriticalThrow(__FILE__,__LINE__,x,y))
#define _AInfo(x,y) (ABaseFunction::Info(__FILE__,__LINE__,x,y))
