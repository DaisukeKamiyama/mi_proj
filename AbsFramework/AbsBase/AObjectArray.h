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

AObjectArray

*/

#pragma once

#include "ABaseFunction.h"
#include "AObjectArrayItem.h"
#include "AStackOnlyObject.h"
#include "AHashArray.h"

#pragma mark ===========================
#pragma mark [クラス]AAbstractFactoryForObjectArray
/**
AObjectArrayに格納されるデータを生成するためのクラス（AbstractFactoryパターン）

AAbstractFactoryForObjectArrayはデフォルト。InsertNew1Object()にインデックスだけを指定したときはデフォルトが使用される。
AObjectArray<T>でTに基底クラスを入れ、InsertNew1Objectで生成すべきサブクラスを決定したいときは、それに応じたFactoryを指定すること。
*/
template<class T> class AAbstractFactoryForObjectArray : public AStackOnlyObject
{
  public:
	AAbstractFactoryForObjectArray() {}
	virtual T*	Create() = 0;
};
template<class T> class ADefaultAbstractFactoryForObjectArray : public AAbstractFactoryForObjectArray<T>
{
  public:
	ADefaultAbstractFactoryForObjectArray( AObjectArrayItem* inParent )  :mParent(inParent) {}
	virtual T*	Create() {return new T(mParent);}
  protected:
	AObjectArrayItem*	mParent;
};

#pragma mark ===========================
#pragma mark [クラス]AObjectArray
/**
オブジェクトの可変長配列

このオブジェクトは、複数オブジェクトを保持する。
実際に保持するのはオブジェクトへのポインタだが、AObjectArrayオブジェクトのDelete系メソッドをコール、
もしくは、AObjectArrayオブジェクト自体を削除すると、保持される各オブジェクトもdeleteされる。

#693 以前はObjectIDとしては、AObjectArrayItem()にて割り当てられたアプリ内で一意のIDを設定していたが、
メモリ削減・高速化のため、IDにはポインタそのものを設定するように変更。
また、IDからObjectIDを取得する必要がある場合は、AObjectArrayIDIndexedオブジェクトの方を使用するように変更。（AArrayとAHashArrayを同時に格納するのはメモリを食うため）
AObjectArrayではIDからObject取得できないようにしました。（速度が遅くなるため。IDからオブジェクト取得必要ならAObjectArrayIDIndexedを使うこと。）
*/
template<class T> class AObjectArray : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	AObjectArray( AObjectArrayItem* inParent = NULL )
	: AObjectArrayItem(inParent), mMutexEnabled(true)
	{
		//OutOfBoundsの場合はthrowするように設定
		mObjectIDArray.SetThrowIfOutOfBounds();
	}
	~AObjectArray()
	{
		DeleteAll();
	}
	
	//配列データ
  private:
	AArray<AObjectID>		mObjectIDArray;//#693
	
  public:
	//新規オブジェクト新規挿入
	void	InsertNew1Object( const AIndex inIndex, AAbstractFactoryForObjectArray<T>& inFactory )
	{
		//オブジェクト生成
		T*	objectPtr = NULL;
		try
		{
			//AbstractFactoryを使用して生成
			objectPtr = inFactory.Create();
			if( objectPtr == NULL )   throw "";//現在のコンパイラではnew失敗はthrowするのが普通だが、古いものではNULLを返す場合もある。
		}
		catch(...)
		{
			//オブジェクトの生成失敗
			{
				//メモリプール無しの場合
				_ACThrow("cannot allocate memory (new NG) (critical)",this);
			}
		}
		//オブジェクトの生成OK
		{
			AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArrayメンバのMutexロック #598
			//配列データにObjectIDを登録
			mObjectIDArray.Insert1(inIndex,objectPtr->GetObjectID());
		}
	}
	//新規オブジェクト新規挿入（デフォルトAbstractFactory使用）
	void	InsertNew1Object( const AIndex inIndex )
	{
		ADefaultAbstractFactoryForObjectArray<T>	defaultFactory(this);
		InsertNew1Object(inIndex,defaultFactory);
	}
	//新規オブジェクト新規追加
	AIndex	AddNewObject( AAbstractFactoryForObjectArray<T>& inFactory )
	{
		AIndex	index = GetItemCount();
		InsertNew1Object(index,inFactory);
		return index;
	}
	//新規オブジェクト新規追加（デフォルトAbstractFactory使用）
	AIndex AddNewObject()
	{
		ADefaultAbstractFactoryForObjectArray<T>	defaultFactory(this);
		return AddNewObject(defaultFactory);
	}
	
	//オブジェクト取得
	T& GetObject( const AIndex inIndex )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArrayメンバのMutexロック #598
		//indexチェック
		if( inIndex < 0 || inIndex >= mObjectIDArray.GetItemCount() )
		{
			_ACThrow("index out of bounds",this);
		}
		//オブジェクト取得
		return (T&)(*(mObjectIDArray.Get(inIndex).val));
	}
	//オブジェクト取得(const)
	const T& GetObjectConst( const AIndex inIndex ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArrayメンバのMutexロック #598
		//indexチェック
		if( inIndex < 0 || inIndex >= mObjectIDArray.GetItemCount() )
		{
			_ACThrow("index out of bounds",this);
		}
		//オブジェクト取得
		return (T&)(*(mObjectIDArray.Get(inIndex).val));
	}
	
	//削除
	void	Delete1Object( const AIndex inIndex )
	{
		T*	objectPtr = &(GetObject(inIndex));//#693NULL;//#598
		//オブジェクトへ通知
		objectPtr->DoDeleted();
		{
			AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArrayメンバのMutexロック #598
			//配列データからObjectIDを削除
			mObjectIDArray.Delete1(inIndex);
		}
		//
		ABaseFunction::AddToObjectArrayItemTrash(objectPtr);
	}
	
	//AObjectArray<>配列内でのSwap（ポインタのみ交換）
	void	SwapObject( const AIndex inA, const AIndex inB )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArrayメンバのMutexロック #598
		//配列データSwap
		mObjectIDArray.Swap(inA,inB);
	}
	
	//Move
  public:
	void	MoveObject( const AIndex inOldIndex, const AIndex inNewIndex )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArrayメンバのMutexロック #598
		//配列データMove
		mObjectIDArray.Move(inOldIndex,inNewIndex);
	}
	
	//全要素削除
	void DeleteAll()
	{
		while( GetItemCount() > 0 )
		{
			Delete1Object(GetItemCount()-1);
		}
	}
	
	/**
	指定index以降削除
	*/
	/*未テスト
	void	DeleteAfter( const AIndex inIndex )
	{
		while( inIndex < GetItemCount() )
		{
			//最後のオブジェクトを削除
			Delete1Object(GetItemCount()-1);
		}
	}
	*/
	
	//ItemCount
	AItemCount GetItemCount() const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArrayメンバのMutexロック #598
		//配列データ要素数取得
		return mObjectIDArray.GetItemCount();
	}
	
	//
  public:
	void	EnableThreadSafe( const ABool inEnable ) { mMutexEnabled = inEnable; }
  private:
	mutable AThreadMutex	mMutex;
	ABool	mMutexEnabled;
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AObjectArray<>"; }
	
};

