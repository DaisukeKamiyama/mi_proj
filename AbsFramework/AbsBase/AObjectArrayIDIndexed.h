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

AObjectArrayIDIndexed

*/

#pragma once

#include "ABaseFunction.h"
#include "AObjectArrayItem.h"
#include "AStackOnlyObject.h"
#include "AHashArray.h"

#pragma mark ===========================
#pragma mark [クラス]AObjectArrayIDIndexed
/**
オブジェクトの可変長配列

このオブジェクトは、複数オブジェクトを保持する。
実際に保持するのはオブジェクトへのポインタだが、AObjectArrayオブジェクトのDelete系メソッドをコール、
もしくは、AObjectArrayオブジェクト自体を削除すると、保持される各オブジェクトもdeleteされる。

#693 以前はObjectIDとしては、AObjectArrayItem()にて割り当てられたアプリ内で一意のIDを設定していたが、
メモリ削減・高速化のため、IDにはポインタそのものを設定するように変更。
また、IDからObjectIDを取得する必要がある場合は、AObjectArrayIDIndexedオブジェクトの方を使用するように変更。（AArrayとAHashArrayを同時に格納するのはメモリを食うため）
*/
template<class T> class AObjectArrayIDIndexed : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	AObjectArrayIDIndexed( AObjectArrayItem* inParent = NULL )
	: AObjectArrayItem(inParent), mMutexEnabled(true)
	{
		//OutOfBoundsの場合はthrowするように設定
		mObjectIDArray.SetThrowIfOutOfBounds();
	}
	~AObjectArrayIDIndexed()
	{
		DeleteAll();
	}
	
	//配列データ
  private:
	AHashArray<AObjectID>		mObjectIDArray;//#693
	
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
	
	//ItemCount
	AItemCount GetItemCount() const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArrayメンバのMutexロック #598
		//配列データ要素数取得
		return mObjectIDArray.GetItemCount();
	}
	
	//ObjectID管理
  public:
	/**
	ObjectIDからIndex取得
	*/
	AIndex	GetIndexByID( const AObjectID inID ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArrayメンバのMutexロック #598
		//#693return mIDArray.Find(inID);
		//ObjectIDからindexを取得
		return mObjectIDArray.Find(inID);
	}
	
  public:
	//ObjectIDからオブジェクト取得
	T& GetObjectByID( const AObjectID inObjectID )
	{
		//Object生存チェック
		if( ExistObject(inObjectID) == false )   _ACThrow("Object not found",this);
		//オブジェクト取得（ObjectIDがポインタそのものなので、そのまま返す。生存チェックのためにこの関数を通すことが必要）
		return (T&)(*(inObjectID.val));
	}
	//ObjectIDからオブジェクト取得(const)
	T& GetObjectConstByID( const AObjectID inObjectID ) const
	{
		//Object生存チェック
		if( ExistObject(inObjectID) == false )   _ACThrow("Object not found",this);
		//オブジェクト取得（ObjectIDがポインタそのものなので、そのまま返す。生存チェックのためにこの関数を通すことが必要）
		return (T&)(*(inObjectID.val));
	}
	//#272
	/**
	Objectが存在するかどうかを返す
	*/
	ABool	ExistObject( const AObjectID inObjectID ) const
	{
		return (GetIndexByID(inObjectID)!=kIndex_Invalid);//#693
	}
	
	//#717
	/**
	Objectへのポインタを取得し、RetainCountをインクリメントする。
	スレッドからオブジェクトへアクセスするとき、スレッドの処理実行中にそのオブジェクトが削除される可能性がある場合、
	このメソッドを使ってオブジェクトへのポインタを取得する。
	スレッドからのアクセスが終了したらオブジェクトのDecrementRetainCount()をコールしてRetainCountをデクリメントする。
	RetainCountが1以上の間はオブジェクトはdeleteされない。（AObjectArrayIDIndexedから削除されても、AObjectArrayItemTrashにつながれて、
	メモリ解放はされない。）
	@note 返り値はNULLの場合がある（すでにオブジェクト削除済みの場合）。コール元でNULLチェック必要。
	*/
	T*	GetObjectAndRetain( const AObjectID inObjectID ) 
	{
		//このメソッドを使う場合は、mutex制御は必須
		if( mMutexEnabled == false )
		{
			_ACError("",NULL);
			mMutexEnabled = true;
		}
		//mutexロックする
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArrayメンバのMutexロック
		if( mObjectIDArray.Find(inObjectID) != kIndex_Invalid )
		{
			//ポインタ取得し、RetainCountをインクリメントする
			T*	object = (T*)(inObjectID.val);
			object->IncrementRetainCount();
			return object;
		}
		else
		{
			//inObjectIDのオブジェクトが存在しない場合はNULLを返す。
			//コール側でNULLチェック必要
			return NULL;
		}
	}
	
	//
  public:
	void	EnableThreadSafe( const ABool inEnable ) { mMutexEnabled = inEnable; }
  private:
	mutable AThreadMutex	mMutex;
	ABool	mMutexEnabled;
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AObjectArrayIDIndexed<>"; }
	
};

