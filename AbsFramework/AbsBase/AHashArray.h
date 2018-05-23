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

AHashArray

*/

#pragma once

#include "ABaseTypes.h"
#include "AObjectArrayItem.h"
#include "AArray.h"
#include "AStArrayPtr.h"

#pragma mark ===========================
#pragma mark ---[クラス]AHashArray<>
/**
可変長配列（ハッシュ検索）クラス
AArrayオブジェクトをコンポジションで持ち、そのデータをハッシュ検索できる。
*/
template <class T> class AHashArray : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	//コンストラクタ
	AHashArray( AObjectArrayItem* inParent = NULL, const AItemCount inInitialAllocateCount = 64, const AItemCount inReallocateStepCount = 64 ) 
			: AObjectArrayItem(inParent), mData(NULL,inInitialAllocateCount,inReallocateStepCount), mEnableHash(true)
	,mHash_DeletedCount(0)//#303
	,mMutexEnabled(false)//#598
	{
		//#693 メモリ節約のため要素が追加されるまではハッシュ生成しない MakeHash();
	}
	//デストラクタ
	virtual ~AHashArray()
	{
	}
	
	//Hash制御のEnable/Disableを設定する #695
	//多量のInsert/Deleteを行うときは、下記の手順のほうが速い場合がある。
	//1. SetEnableHash(false);
	//2. 多量のInsert/Deleteを行う
	//3. SetEnableHash(true);
	//4. MakeHash();
  public:
	void	SetEnableHash( const ABool inEnableHash )
	{
		mEnableHash = inEnableHash;
	}
  private:
	ABool	mEnableHash;
	
  public:
	//１個のデータを挿入
	void	Insert1( const AIndex inIndex, const T& inItem )
	{
		//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		//データ配列に追加
		mData.Insert1(inIndex,inItem);
		
		//ハッシュ更新
		if( mEnableHash == true )
		{
			UpdateHash_DataAdded(inIndex,1);
		}
	}
	//Arrayから複数のデータを挿入
	void	Insert( const AIndex inIndex, const AArray<T>& inArray )
	{
		//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		//データ配列に追加
		AItemCount	itemcount = inArray.GetItemCount();
		for( AIndex i = 0; i < itemcount; i++ )
		{
			mData.Insert1(inIndex+i,inArray.Get(i));
		}
		//ハッシュ更新
		if( mEnableHash == true )
		{
			UpdateHash_DataAdded(inIndex,itemcount);
		}
	}
	//１個のデータを追加
	AIndex	Add( const T& inItem )
	{
		AIndex	index = GetItemCount();
		Insert1(index,inItem);
		return index;
	}
	
	//１個のデータを削除
	void	Delete1( const AIndex inIndex )
	{
		//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		//データ配列から削除
		mData.Delete1(inIndex);
		
		//ハッシュ更新
		if( mEnableHash == true )
		{
			UpdateHash_DataDeleted(inIndex);
		}
	}
	
	//全部のデータを削除
	void DeleteAll()
	{
		/*#695 高速化
		while(GetItemCount()>0)
		{
			Delete1(GetItemCount()-1);
		}
		*/
		mData.DeleteAll();
		mHash.DeleteAll();
		mHash_DeletedCount = 0;
	}
	
	//データ取得
	T Get( const AIndex inIndex ) const
	{
		if( mMutexEnabled == true )//AStMutexLockerのオブジェクト生成にも時間がかかるので高速化のため先に判定 #598
		{
			//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
			AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
			
			return mData.Get(inIndex);
		}
		else
		{
			return mData.Get(inIndex);
		}
	}
	
	//データ設定
	void Set( const AIndex inIndex, const T& inData )
	{
		if( mMutexEnabled == true )//AStMutexLockerのオブジェクト生成にも時間がかかるので高速化のため先に判定 #598
		{
			//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
			AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
			
			//設定前のデータ値を取得 #695
			T	oldData = mData.Get(inIndex);
			
			//データ配列に設定
			mData.Set(inIndex,inData);
			
			//ハッシュ更新
			if( mEnableHash == true )
			{
				UpdateHash_DataChanged(inIndex,oldData);//#695
			}
		}
		else
		{
			//設定前のデータ値を取得 #695
			T	oldData = mData.Get(inIndex);
			
			//データ配列に設定
			mData.Set(inIndex,inData);
			
			//ハッシュ更新
			if( mEnableHash == true )
			{
				UpdateHash_DataChanged(inIndex,oldData);//#695
			}
		}
	}
	
	//Swap
	void	Swap( const AIndex inIndexA, const AIndex inIndexB )
	{
		T	tmp = Get(inIndexA);
		Set(inIndexA,Get(inIndexB));
		Set(inIndexB,tmp);
	}
	
	//Move #693
	void	Move( const AIndex inOldIndex, const AIndex inNewIndex )
	{
		T	tmp = Get(inOldIndex);
		Delete1(inOldIndex);
		Insert1(inNewIndex,tmp);
	}
	
	//#676
	/**
	配列全体を他のAHashArrayからコピーする
	*/
	void	SetFromArray( const AHashArray<T>& inSrcArray )
	{
		//全削除
		DeleteAll();
		
		//１要素ずつコピー追加
		AItemCount	itemCount = inSrcArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			Add(inSrcArray.Get(i));
		}
	}
	
	//検索
	AIndex	Find( const T& inTargetData ) const
	{
		//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		if( mEnableHash == true )
		{
			//#693 要素0の場合はmHash未生成の場合があるので、下の処理で0除算させないよう、ここで判断
			if( mData.GetItemCount() == 0 )
			{
				return kIndex_Invalid;
			}
			
			//統計情報#271
			gStatics_AHashArray_FindCount++;
			gStatics_AHashArray_CompareCount++;
			
			//ハッシュサイズ取得
			AItemCount	hashsize = mHash.GetItemCount();
			unsigned long	hash = GetHashValue(inTargetData);
			AIndex	hashstartpos = hash%hashsize;
			
			//一発発見できるならポインタ使用しない
			AIndex	dI = mHash.Get(hashstartpos);
			if( dI == kIndex_HashNoData )
			{
				//統計情報#271
				gStatics_AHashArray_FirstDataNull++;
				//
				return kIndex_Invalid;
			}
			else if( dI != kIndex_HashDeleted )
			{
				if( mData.Get(dI) == inTargetData )
				{
					//統計情報#271
					gStatics_AHashArray_FirstDataHit++;
					//
					return dI;
				}
			}
			{//#598 arrayptrの有効範囲を最小にする
				//高速化のため、ポインタ使用
				AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
				AIndex*	p = arrayptr.GetPtr();
				//ハッシュ検索
				AIndex	hashpos = hashstartpos;
				hashpos = ((hashpos==0)?hashsize-1:hashpos-1);//#271 最初の位置は上記で検索済みなので次の位置から始める
				while( true )
				{
					gStatics_AHashArray_CompareCount++;//統計情報#271
					AIndex	dataIndex = p[hashpos];
					if( dataIndex == kIndex_HashNoData )   break;
					if( dataIndex != kIndex_HashDeleted )
					{
						if( mData.Get(dataIndex) == inTargetData )
						{
							return dataIndex;
						}
					}
					//次のハッシュ位置へ
					hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
					if( hashpos == hashstartpos )   break;
				}
			}
			return kIndex_Invalid;
		}
		else
		{
			return mData.Find(inTargetData);
		}
	}
	void	FindAll( const T& inTargetData, AArray<AIndex>& outFoundIndex ) const
	{
		//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		outFoundIndex.DeleteAll();
		if( mEnableHash == true )
		{
			//#693 要素0の場合はmHash未生成の場合があるので、下の処理で0除算させないよう、ここで判断
			if( mData.GetItemCount() == 0 )
			{
				return;
			}
			
			//ハッシュサイズ取得
			AItemCount	hashsize = mHash.GetItemCount();
			unsigned long	hash = GetHashValue(inTargetData);
			AIndex	hashstartpos = hash%hashsize;
			
			{//#598 arrayptrの有効範囲を最小にする
				//高速化のため、ポインタ使用
				AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
				AIndex*	p = arrayptr.GetPtr();
				//ハッシュ検索
				AIndex	hashpos = hashstartpos;
				while( true )
				{
					AIndex	dataIndex = p[hashpos];
					if( dataIndex == kIndex_HashNoData )   break;
					if( dataIndex != kIndex_HashDeleted )
					{
						if( mData.Get(dataIndex) == inTargetData )
						{
							outFoundIndex.Add(dataIndex);
						}
					}
					//次のハッシュ位置へ
					hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
					if( hashpos == hashstartpos )   break;
				}
			}
			return;
		}
		else
		{
			_ACError("not yet supported",this);
		}
	}
	
	//ItemCount
	AItemCount	GetItemCount() const 
	{
		//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		return mData.GetItemCount();
	}
	
  public:
	//#695
	/**
	メモリ拡大時の再割り当て増加量の設定
	*/
	void	SetReallocateStep( const AItemCount inReallocateCount )
	{
		mData.SetReallocateStep(inReallocateCount);
	}
	
  private:
	//Hash値取得
	//各T毎に定義すること。
	unsigned long	GetHashValue( const T inData ) const;
	
  public:
	//ハッシュテーブルを初期化し、対象Arrayの全データを登録する。
	void	MakeHash( const AItemCount inMinimumHashSize = kIndex_Invalid )//#693 使用側からハッシュサイズを指定してMakeHashできるようにした。
	{
		//対象Arrayのサイズが大きすぎたらreturn
		if( mData.GetItemCount() >= kItemCount_HashMaxSize/2 )
		{
			_ACError("cannot make hash because array is too big",this);
			return;
		}
		//ハッシュテーブル全削除
		mHash.DeleteAll();
		AItemCount	hashsize = (mData.GetItemCount()+mData.GetReallocateStep())*3+128;
		//inMinimumHashSizeが指定されており、かつ、上記で設定した値よりも大きいときは、ハッシュサイズをinMinimumHashSizeにする
		if( inMinimumHashSize != kIndex_Invalid )
		{
			if( hashsize < inMinimumHashSize )
			{
				hashsize = inMinimumHashSize;
			}
		}
		//ハッシュサイズが最大ハッシュサイズよりも大きければ、補正する。
		if( hashsize > kItemCount_HashMaxSize )   hashsize = kItemCount_HashMaxSize;
		//ハッシュテーブル確保
		mHash.Reserve(0,hashsize);
		//ハッシュテーブル初期化
		{
			AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
			AIndex*	p = arrayptr.GetPtr();
			for( AIndex i = 0; i < hashsize; i++ )
			{
				//#695 mHash.Set(i,kIndex_HashNoData);
				p[i] = kIndex_HashNoData;
			}
		}
		//対象Arrayの現在のデータを全て登録
		AItemCount	datacount = mData.GetItemCount();
		for( AIndex i = 0; i < datacount; i++ )
		{
			RegisterToHash(i);
		}
		//"Deleted"個数初期化   #303 "Deleted"の個数をRehash判断に使用する
		mHash_DeletedCount = 0;
		//統計情報#271
		gStatics_AHashArray_MakeHashCount++;
	}
		
	//ハッシュ更新（追加）
	void	UpdateHash_DataAdded( const AIndex inDataIndex, const AItemCount inAddedDataCount )
	{
		//この関数はmDataへの追加後にコールされる（すでにmDataの要素数は元の数＋inAddedDataCountになっている）。
		//inDataIndexは追加開始位置。
		
		//ハッシュサイズが対象データ配列の現在のサイズの2倍よりも小さければ、ハッシュテーブルを再構成する。
		if( mHash.GetItemCount() < (mData.GetItemCount()+mHash_DeletedCount)*2+32 )//#303 "Deleted"の個数をRehash判断に使用する
		{
			MakeHash();
		}
		//そうでない場合は対象アイテムだけ追加登録する。
		else
		{
			//元の配列の最後に追加した場合以外は、追加データのインデックスより後のインデックスを+1する。
			if( inDataIndex < mData.GetItemCount() - inAddedDataCount )
			{
				AItemCount	hashsize = mHash.GetItemCount();
				AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
				AIndex*	p = arrayptr.GetPtr();
				for( AIndex hashpos = 0; hashpos < hashsize; hashpos++ )
				{
					AIndex	dataIndex = p[hashpos];
					if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   continue;
					if( dataIndex >= inDataIndex )
					{
						p[hashpos] = dataIndex + inAddedDataCount;
					}
				}
			}
			/*#695高速化
			//ハッシュサイズ取得
			AItemCount	hashsize = mHash.GetItemCount();
			AItemCount	itemCount = mData.GetItemCount();
			//高速化のため、ポインタ使用
			{//#598 arrayptrの有効範囲を最小にする
				AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
				AIndex*	p = arrayptr.GetPtr();
				//追加データのindexより後のデータを全て検索し、ハッシュに登録してあるindexを+inAddedDataCountする。
				for( AIndex index = inDataIndex+inAddedDataCount; index < itemCount; index++ )
				{
					//ハッシュ値計算
					unsigned long	hash = GetHashValue(mData.Get(index));
					AIndex	hashstartpos = hash%hashsize;
					
					//検索
					AIndex	hashpos = hashstartpos;
					while( true )
					{
						AIndex	dataIndex = p[hashpos];
						if( dataIndex == kIndex_HashNoData )   {_ACThrow("",this);break;}
						if( dataIndex+inAddedDataCount == index )
						{
							//登録Indexを+inAddedDataCount
							p[hashpos] = dataIndex+inAddedDataCount;
							break;
						}
						//次のハッシュ位置へ
						hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
						if( hashpos == hashstartpos )   {_ACThrow("",this);break;}
					}
				}
			}
			*/
			//ハッシュに登録
			RegisterToHash(inDataIndex);
		}
	}
	
	//ハッシュ更新（データの中身書き換えによるHash値の変更）
	void UpdateHash_DataChanged( const AIndex inDataIndex, const T inOldData )//#695
	{
		//ハッシュサイズ取得
		AItemCount	hashsize = mHash.GetItemCount();
		//ハッシュ値取得 #695
		unsigned long	hash = GetHashValue(inOldData);
		AIndex	hashstartpos = hash%hashsize;
		//
		ABool	found = false;
		{//#598 arrayptrの有効範囲を最小にする
			//高速化のため、ポインタ使用
			AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
			AIndex*	p = arrayptr.GetPtr();
			//ハッシュ検索
			//#695 for( AIndex hashpos = 0; hashpos < hashsize; hashpos++ )
			AIndex	hashpos = hashstartpos;//#695
			while( true )//#695
			{
				AIndex	dataIndex = p[hashpos];
				//#695 if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   continue;
				if( dataIndex == kIndex_HashNoData )   break;//#695
				if( dataIndex == inDataIndex )
				{
					if( p[(hashpos==0)?hashsize-1:hashpos-1] == kIndex_HashNoData )
					{
						p[hashpos] = kIndex_HashNoData;
						//win 080713 NoDataを書き込んだとき、前のハッシュ位置（物理的には次）がDeletedならNoDataに書き換え
						//[hashpos-1]:NoData
						//[hashpos  ]:NoData←今回書き込み
						//[hashpos+1]:Deleted←このDeletedはNoDataにする（hashposの位置のデータへのリンクをつなげるために存在していたDeletedなので。）
						//[hashpos+2]:Deleted←さらに次もDeletedならここもNoData
						AIndex	hp = hashpos+1;
						if( hp >= hashsize )   hp = 0;
						while( p[hp] == kIndex_HashDeleted )
						{
							//DeletedならNoDataに書き換えて継続
							p[hp] = kIndex_HashNoData;
							hp++;
							if( hp >= hashsize )   hp = 0;
							//"Deleted"を"NoData"にしたので"Deleted"個数をデクリメント   #303 "Deleted"の個数をRehash判断に使用する
							mHash_DeletedCount--;
						}
					}
					else
					{
						p[hashpos] = kIndex_HashDeleted;
						//"Deleted"にしたので"Deleted"個数をインクリメント   #303 "Deleted"の個数をRehash判断に使用する
						mHash_DeletedCount++;
					}
					////#598 実行位置を下へ移動 RegisterToHash(inDataIndex);
					found = true;
					break;//win 080713 同じindexが複数登録されていることはない（かつ、ここでbreakしないと新規登録したデータにひっかかるのでは？）
				}
				//#695
				//次のハッシュ位置へ
				hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
				if( hashpos == hashstartpos )   break;
			}
		}
		//#598
		if( found == true )
		{
			RegisterToHash(inDataIndex);
		}
	}
	
	//ハッシュ更新（削除）
	void UpdateHash_DataDeleted( const AIndex inDataIndex )
	{
		//ハッシュサイズ取得
		AItemCount	hashsize = mHash.GetItemCount();
		//高速化のため、ポインタ使用 #695
		AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
		AIndex*	p = arrayptr.GetPtr();
		//
		for( AIndex hashpos = 0; hashpos < hashsize; hashpos++ )
		{
			AIndex	dataIndex = p[hashpos];//#695 mHash.Get(hashpos);
			if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   continue;
			if( dataIndex == inDataIndex )
			{
				if( /*#695 mHash.Get*/p[((hashpos==0)?hashsize-1:hashpos-1)] == kIndex_HashNoData )
				{
					//#695 mHash.Set(hashpos,kIndex_HashNoData);
					p[hashpos] = kIndex_HashNoData;//#695
					//win 080713 NoDataを書き込んだとき、前のハッシュ位置（物理的には次）がDeletedならNoDataに書き換え
					//[hashpos-1]:NoData
					//[hashpos  ]:NoData←今回書き込み
					//[hashpos+1]:Deleted←このDeletedはNoDataにする（hashposの位置のデータへのリンクをつなげるために存在していたDeletedなので。）
					//[hashpos+2]:Deleted←さらに次もDeletedならここもNoData
					AIndex	hp = hashpos+1;
					if( hp >= hashsize )   hp = 0;
					while( /*#695 mHash.Get(hp)*/p[hp] == kIndex_HashDeleted )
					{
						//DeletedならNoDataに書き換えて継続
						//#695 mHash.Set(hp,kIndex_HashNoData);
						p[hp] = kIndex_HashNoData;//#695
						hp++;
						if( hp >= hashsize )   hp = 0;
						//"Deleted"を"NoData"にしたので"Deleted"個数をデクリメント   #303 "Deleted"の個数をRehash判断に使用する
						mHash_DeletedCount--;
					}
				}
				else
				{
					//#695 mHash.Set(hashpos,kIndex_HashDeleted);
					p[hashpos] = kIndex_HashDeleted;//#695
					//"Deleted"にしたので"Deleted"個数をインクリメント   #303 "Deleted"の個数をRehash判断に使用する
					mHash_DeletedCount++;
				}
			}
			//追加データのインデックスより後のインデックスを-1する。
			else if( dataIndex > inDataIndex )
			{
				//#695 mHash.Set(hashpos,dataIndex-1);
				p[hashpos] = dataIndex-1;//#695
			}
		}
	}
	
	//ハッシュテーブルに、対象Arrayの指定アイテムを登録する。
	//データIndex番号の移動なし
	void RegisterToHash( const AIndex inDataIndex )
	{
		//ハッシュサイズ取得
		AItemCount	hashsize = mHash.GetItemCount();
		
		//ハッシュ値計算
		unsigned long	hash = GetHashValue(mData.Get(inDataIndex));
		AIndex	hashstartpos = hash%hashsize;
		
		//登録箇所検索
		{//#598 arrayptrの有効範囲を最小にする
			//高速化のため、ポインタ使用
			//#695 AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
			//#695 AIndex*	p = arrayptr.GetPtr();
			//高速化のため、直にAArray::mDataPtrを取得する。
			//AStArrayPtr<>は、あくまでコーディングミス防止なので、変更等が頻繁には発生しないAbsBase内コードでは、
			//必ずしも使う必要が無い。
			//AHashArrayへの10M項目追加で、9s→2sの効果。
			//rehash時のMakeHashにも使われているので、ここの高速化は重要。
			AIndex*	p = mHash.GetDataPtrWithoutCheck();
			//ハッシュ検索
			AIndex	hashpos = hashstartpos;
			while( true )
			{
				AIndex	dataIndex = p[hashpos];
				if( dataIndex == kIndex_HashNoData )   break;
				if( dataIndex == kIndex_HashDeleted )
				{
					//"Deleted"にデータ上書きするので"Deleted"個数をデクリメント   #303 "Deleted"の個数をRehash判断に使用する
					mHash_DeletedCount--;
					break;
				}
				//次のハッシュ位置へ
				hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
				if( hashpos == hashstartpos )
				{
					_ACError("no area in hash",this);
					return;
				}
			}
			
			//登録
			p[hashpos] = inDataIndex;
		}
	}
	
	//データ
  public:
	AItemCount	GetHashSize() const { return mHash.GetItemCount(); }
  private:
	//ハッシュ
	//配列データのインデックスを格納する。
	AArray<AIndex>	mHash;
	AItemCount	mHash_DeletedCount;//#303
	//配列データ
	AArray<T>	mData;
	
	//OutOfBoundsの場合にthrowする設定
  public:
	void	SetThrowIfOutOfBounds() { mData.SetThrowIfOutOfBounds(); }
	
	//#598
	/**
	スレッドセーフEnable/Disable設定
	inEnable=trueの場合、AHashArrayの内部データについてはスレッドセーフとなる。
	（外部に渡したデータについてはスレッドセーフではない（内部データとの整合性保証されない）ので注意。
	たとえばGetItemCount()で項目数を取得した場合、その値はコール側でmutexをかけない限り、
	保証されない。（別スレッドでInsert/Deleteされる可能性がある。）
	デフォルトはDisable。
	*/
  public:
	void	EnableThreadSafe( const ABool inEnable ) { mMutexEnabled = inEnable; }
  private:
	//HashArray内部データへのアクセスに対するMutex
  private:
	ABool	mMutexEnabled;
	mutable AThreadMutex	mMutex;
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AHashArray<>"; }
	
};

