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

#pragma once

#include "ABaseTypes.h"
#include "../AbsBase_Imp/ABaseWrapper.h"
#include "AObjectArrayItem.h"
#include "ABaseFunction.h"
#include "AThreadMutex.h"
#include "APurgeManager.h"

const AItemCount kInitialAllocateCountDefault = 16;//B0373
const AItemCount kReallocateStepCount = 32;//B0373

#pragma mark ===========================
#pragma mark [クラス]AArrayAbstract

/**
AArray<>の基底クラス
*/
class AArrayAbstract: public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	/**
	コンストラクタ
	*/
	AArrayAbstract( AObjectArrayItem* inParent );
	/**
	デストラクタ
	*/
	~AArrayAbstract();
	
	//要素数
  public:
	/**
	要素数取得
	*/
	AItemCount	GetItemCount() const {return mItemCount;}
  protected:
	AItemCount	mItemCount;
	
	//debug B0373
  public:
	static AByteCount	GetTotalAllocatedByteCount() { return sTotalAllocatedByteCount; }
  protected:
	static AByteCount	sTotalAllocatedByteCount;
	
};

#pragma mark ===========================
#pragma mark [クラス]AArrayCore<>

template<class T> class AStArrayPtr;
class AStCreateCstringFromAText;

//基本型の可変長配列のコア部分
//mDataPtrを直接アクセスする
//mDataPtrアクセスに使用される引数は全てチェックされる。
//AStArrayPtr経由で取得したポインタを誤って使用した場合以外、
//AArrayで未確保メモリエリアを参照することが無いことが論理的に保証される。
/**
可変長配列コア部分のクラス
*/
template<class T> class AArrayCore : public AArrayAbstract
{
	//mDataPtrはprivate定義であるが、OS APIやC文字列とのやり取りのためには、データへのポインタを使用した方が速度面で有利なので、
	//例外的に、下記クラス経由でGetDataPtr()を呼び出し、mDataPtrを参照、ポインタを取得する。
	friend class AStArrayPtr<T>;
	friend class AStCreateCstringFromAText;
#if IMPLEMENTATION_FOR_WINDOWS
	friend class AStCreateWcharStringFromAText;
#endif
	
	//コンストラクタ／デストラクタ
  public:
	/**
	コンストラクタ
	*/
	AArrayCore( AObjectArrayItem* inParent, const AItemCount inInitialAllocateCount, const AItemCount inReallocateStepCount ) 
		: AArrayAbstract(inParent), mLockCount(0), mDataPtr(NULL), mReallocateStepCount(inReallocateStepCount),
	mAllocatedSize(/*#693inInitialAllocateCount*sizeof(T)*/0), mThrowIfOutOfBounds(false)//B0000
	,mMutexEnabled(false)//#598
	,mInitialAllocateCount(inInitialAllocateCount)//#693
	,mPurged(false)//#693
	{
	}
  private:
	void	FirstMalloc() const
	{
		//#693
		mAllocatedSize = mInitialAllocateCount*sizeof(T);
		//メモリ割当
		mDataPtr = (T*)AMemoryWrapper::Malloc(mAllocatedSize);
		if( mDataPtr == NULL )
		{
			_ACThrow("cannot allocate memory (malloc NG) (critical)",this);
		}
		//debug
		sTotalAllocatedByteCount += mAllocatedSize;
	}
  public:
	/**
	デストラクタ
	*/
	virtual ~AArrayCore()
	{
		//#693 ファイルへのPurge中ならPurgeファイル削除 ★未テスト
		if( mPurged == true )
		{
			APurgeManager::GetPurgeManager().DeletePurge(this);
			mPurged = false;
		}
		//
		if( mDataPtr != NULL )
		{
			//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
			AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
			
			if( IsLockedByArrayPtr() == true )
			{
				//ArrayPtrによるロック中はデータポインタを解放しない（メモリリークとなるが、強制終了よりまし）
				//mMutexEnabled=trueのときは、ここに来ることはない。
				//（ArrayPtrによるロック中はmMutexは必ずロックされているため、ArrayPtrロックがはずれてからここに来る。）
				//ここはmMutexEnabled=falseのとき用の処理。
				_ACError("AArray deleted while Locked",this);
			}
			else
			{
				AMemoryWrapper::Free(mDataPtr);
				mDataPtr = NULL;
				//debug
				sTotalAllocatedByteCount -= mAllocatedSize;
			}
		}
	}
  public:
	/**
	Allocateされたサイズを取得
	*/
	AByteCount	GetAllocatedSize() const { return mAllocatedSize; }
  private:
	//データポインタ
	mutable T*	mDataPtr;
	//#693
	AItemCount	mInitialAllocateCount;
	//サイズ変更ステップ
	AItemCount	mReallocateStepCount;
	//メモリ割当サイズ
	mutable AByteCount	mAllocatedSize;
	
	//B0000
	/**
	指定Indexが範囲外の場合Throwするように設定する
	*/
  public:
	void	SetThrowIfOutOfBounds() { mThrowIfOutOfBounds = true; }
  private:
	ABool	mThrowIfOutOfBounds;
	
	/**
	ポインタの取得
	*/
  private:
	T*	GetDataPtr( const AIndex inIndex ) const
	{
		if( inIndex < 0 || inIndex > mItemCount )
		{
			_ACThrow("index error",this);
		}
		//#693 ファイルへのPurge中ならPurgeファイルから復元 ★未テスト
		if( mPurged == true )
		{
			mDataPtr = (T*)(APurgeManager::GetPurgeManager().RestoreFromPurge(this));
			mPurged = false;
		}
		//#693 メモリ未割り当てなら割り当て
		if( mDataPtr == NULL )
		{
			FirstMalloc();
		}
		//
		return &(mDataPtr[inIndex]);
	}
	/**
	ポインタの直接取得（速度重視のためどうしても必要な場合用。チェックを全くしないので注意必要）
	*/
  public:
	T* GetDataPtrWithoutCheck() const
	{
		return mDataPtr;
	}
	
	//挿入／削除コア
  public:
	/**
	メモリ領域挿入　添字inIndex以降にinCount個分の領域を挿入する
	*/
	void	Reserve( const AIndex inIndex, const AItemCount inCount )
	{
		//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		//ロック中は挿入不可
		if( IsLockedByArrayPtr() == true )
		{
			//mMutexEnabled=trueのときは、ここに来ることはない。
			//（ArrayPtrによるロック中はmMutexは必ずロックされているため、ArrayPtrロックがはずれてからここに来る。）
			//ここはmMutexEnabled=falseのとき用の処理。
			_ACThrow("AArray size is to be changed while Locked.",this);
		}
		//引数チェック
		if( inCount <= 0 )   return;
		if( inIndex < 0 )
		{
			_ACThrow("index out of bounds (<0)",this);
		}
		if( inIndex > mItemCount )
		{
			_ACThrow("index out of bounds (>mItemCount)",this);
		}
		
		//#693 ファイルへのPurge中ならPurgeファイルから復元 ★未テスト
		if( mPurged == true )
		{
			mDataPtr = (T*)(APurgeManager::GetPurgeManager().RestoreFromPurge(this));
			mPurged = false;
		}
		//#693 メモリ未割り当てなら割り当て
		if( mDataPtr == NULL )
		{
			FirstMalloc();
		}
		
		//メモリ領域拡大
		//新しく必要になるサイズが、現在のstorageサイズ-3個分以上であれば、メモリ領域を拡大する。
		//"-3"するのは、万が一、storageのちょっと後ろのデータをアクセスしたときでも強制終了させないための安全策
		if( (mItemCount+inCount)*sizeof(T) >= mAllocatedSize - 3*sizeof(T) )
		{
			AByteCount	newSize = (mItemCount+inCount)*sizeof(T);
			newSize += mReallocateStepCount*sizeof(T);//mReallocateStepCount分、余分にメモリ確保する。
			T*	newptr = (T*)AMemoryWrapper::Realloc(mDataPtr,newSize);//★200MBペースト時エラー
			if( newptr == NULL )
			{
				_ACThrow("cannot allocate memory (realloc NG) (critical)",this);
			}
			if( newptr != mDataPtr )
			{
				//ポインタへの値代入は、ポインタ値が変わったときだけにしている。
				//スレッドセーフ設定にしていない場合に、万一スレッド間でアクセスしてしまったときに、
				//ポインタエラーになるリスクを低くするため。
				mDataPtr = newptr;
			}
			//debug
			sTotalAllocatedByteCount += newSize-mAllocatedSize;
			//realloc OK
			mAllocatedSize = newSize;
		}
		//領域確保チェックはしない
		//（reallocがエラーなしで返ってきているので、領域確保は信頼できる。チェックはオーバーヘッドになる。）
		//内容の移動
		if( inIndex != mItemCount )
		{
			AMemoryWrapper::Memmove(&(mDataPtr[inIndex+inCount]),&(mDataPtr[inIndex]),(mItemCount-inIndex)*sizeof(T));
		}
		//mItemCount更新
		mItemCount += inCount;
	}
	/**
	削除
	*/
	void	Delete( const AIndex inIndex, const AItemCount inCount )
	{
		//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		//ロック中は削除不可
		if( IsLockedByArrayPtr() == true )
		{
			//mMutexEnabled=trueのときは、ここに来ることはない。
			//（ArrayPtrによるロック中はmMutexは必ずロックされているため、ArrayPtrロックがはずれてからここに来る。）
			//ここはmMutexEnabled=falseのとき用の処理。
			_ACThrow("AArray size is to be changed while Locked.",this);
		}
		
		//引数チェック
		if( inCount <= 0 )   return;
		if( inCount > mItemCount-inIndex )
		{
			_ACThrow("count out of bounds (>mItemCount-index)",this);
		}
		if( inIndex < 0 )
		{
			_ACThrow("index out of bounds (<0)",this);
		}
		if( inIndex > mItemCount )
		{
			_ACThrow("index out of bounds (>mItemCount)",this);
		}
		
		//#693 ファイルへのPurge中ならPurgeファイルから復元 ★未テスト
		if( mPurged == true )
		{
			mDataPtr = (T*)(APurgeManager::GetPurgeManager().RestoreFromPurge(this));
			mPurged = false;
		}
		
		//全削除かどうかの判定 #1275
		if( inIndex == 0 && inCount == mItemCount )
		{
			//全削除の場合 #1275
			
			//mItemCount更新
			mItemCount = 0;
			
			//メモリ解放
			if( mDataPtr != NULL )
			{
				//メモリ解放
				AMemoryWrapper::Free(mDataPtr);
				mDataPtr = NULL;
				//debug
				sTotalAllocatedByteCount -= mAllocatedSize;
				//
				mAllocatedSize = 0;
			}
		}
		else
		{
			//全削除以外の場合
			
			//内容の移動
			if( inIndex != mItemCount-inCount )
			{
				AMemoryWrapper::Memmove(&(mDataPtr[inIndex]),&(mDataPtr[inIndex+inCount]),(mItemCount-inIndex-inCount)*sizeof(T));
			}
			//新しく必要になるサイズが、現在のstorageサイズ-(mReallocateStepCount+10)個分未満であれば、メモリ領域を縮小する。
			if( mAllocatedSize > mReallocateStepCount*sizeof(T) && //最小サイズ（mReallocateStepCount）より小さくしないための条件
			(mItemCount-inCount)*sizeof(T) < mAllocatedSize - (mReallocateStepCount+10)*sizeof(T) ) 
			{
				AByteCount	newSize = (mItemCount-inCount)*sizeof(T);
				newSize += mReallocateStepCount*sizeof(T);
				T*	newptr = (T*)AMemoryWrapper::Realloc(mDataPtr,newSize);
				if( newptr == NULL )
				{
					//realloc NGの場合の処理
					//Reserve()の場合と違い、こちらは縮小なので、ふつうエラーになるはずが無く、また、メモリプール解放しても問題解決しない。
					_ACThrow("cannot allocate memory (realloc NG) (critical)",this);
				}
				//debug
				sTotalAllocatedByteCount += newSize-mAllocatedSize;
				//realloc OK
				mAllocatedSize = newSize;
				if( newptr != mDataPtr )
				{
					mDataPtr = newptr;
				}
			}
			//mItemCount更新
			mItemCount -= inCount;
		}
		/* #1275 削除後の項目数が0の場合、下でメモリ解放していたが、上で行なっていたメモリ領域の縮小等の処理が無駄になるので、削除後の項目数が0の場合はメモリ解放だけするようにする（処理を上に移動）。
		//#693
		//削除後の項目数が0ならメモリ解放する（再度Reserve()がコールされたらそこで再度FirstMalloc()がコールされる）
		if( mItemCount == 0 && mDataPtr != NULL )
		{
			//メモリ解放
			AMemoryWrapper::Free(mDataPtr);
			mDataPtr = NULL;
			//debug
			sTotalAllocatedByteCount -= mAllocatedSize;
			//
			mAllocatedSize = 0;
		}
		*/
	}
	//#695
	/**
	メモリ拡大時の再割り当て増加量の設定
	*/
  public:
	void	SetReallocateStep( const AItemCount inReallocateCount )
	{
		mReallocateStepCount = inReallocateCount;
	}
	/**
	現在の割り当て増加量設定の取得
	*/
	AItemCount	GetReallocateStep() const
	{
		return mReallocateStepCount;
	}
	
  public:
	//#695
	/**
	同じAArray配列からコピーして、指定inIndexへ挿入
	*/
	void	Insert( const AIndex inIndex, const AArrayCore<T>& inArray )
	{
		//コピー元配列の個数を取得して、本配列の領域確保
		AItemCount	itemCount = inArray.GetItemCount();
		Reserve(inIndex,itemCount);
		//データコピー
		for( AIndex i = 0; i < itemCount; i++ )
		{
			mDataPtr[inIndex+i] = inArray.mDataPtr[i];
		}
	}
	
	//データ取得／設定コア
  public:
	/**
	取得
	*/
	T Get( const AIndex inIndex ) const
	{
		//引数チェック
		if( inIndex < 0 )
		{
			//B0000 Get/Setに関しては、デフォルトではthrowしないほうが安全
			//Get/Set後の処理は実行させても無限ループ等発生する確率は低い。（配列個数は正常系でも変わらないので）
			//Get/Setでthrowさせてしまうと、害のほうが大きい（致命的ではないのにそこで処理がストップしてしまうことがある）
			//ただ、ポインタの配列に関しては、throwしたほうが安全なのでmThrowIfOutOfBoundsによって切り替える
			if( mThrowIfOutOfBounds == false )
			{
				_ACError("index out of bounds (<0)",this);
			}
			else
			{
				_ACThrow("index out of bounds (<0)",this);
			}
			T	tmp;
			AMemoryWrapper::Fill0(&tmp,sizeof(T));
			return tmp;
		}
		if( inIndex >= mItemCount )
		{
			//B0000 Get/Setに関しては、デフォルトではthrowしないほうが安全
			//Get/Set後の処理は実行させても無限ループ等発生する確率は低い。（配列個数は正常系でも変わらないので）
			//Get/Setでthrowさせてしまうと、害のほうが大きい
			//ただ、ポインタの配列に関しては、throwしたほうが安全なのでmThrowIfOutOfBoundsによって切り替える
			if( mThrowIfOutOfBounds == false )
			{
				_ACError("index out of bounds (>=mItemCount)",this);
			}
			else
			{
				_ACThrow("index out of bounds (>=mItemCount)",this);
			}
			T	tmp;
			AMemoryWrapper::Fill0(&tmp,sizeof(T));
			return tmp;
		}
		
		//#693 ファイルへのPurge中ならPurgeファイルから復元 ★未テスト
		if( mPurged == true )
		{
			mDataPtr = (T*)(APurgeManager::GetPurgeManager().RestoreFromPurge(this));
			mPurged = false;
		}
		
		if( mMutexEnabled == true )//AStMutexLockerのオブジェクト生成にも時間がかかるので高速化のため先に判定 #598
		{
			//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
			AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
			//取得
			return mDataPtr[inIndex];
		}
		else
		{
			//取得
			return mDataPtr[inIndex];
		}
	}
	/**
	設定
	*/
	void Set( const AIndex inIndex, const T inData )
	{
		//引数チェック
		if( inIndex < 0 )
		{
			//B0000 Get/Setに関しては、デフォルトではthrowしないほうが安全
			//Get/Set後の処理は実行させても無限ループ等発生する確率は低い。（配列個数は正常系でも変わらないので）
			//Get/Setでthrowさせてしまうと、害のほうが大きい
			//ただ、ポインタの配列に関しては、throwしたほうが安全なのでmThrowIfOutOfBoundsによって切り替える
			if( mThrowIfOutOfBounds == false )
			{
				_ACError("index out of bounds (<0)",this);
			}
			else
			{
				_ACThrow("index out of bounds (<0)",this);
			}
			return;
		}
		if( inIndex >= mItemCount )
		{
			//B0000 Get/Setに関しては、デフォルトではthrowしないほうが安全
			//Get/Set後の処理は実行させても無限ループ等発生する確率は低い。（配列個数は正常系でも変わらないので）
			//Get/Setでthrowさせてしまうと、害のほうが大きい
			//ただ、ポインタの配列に関しては、throwしたほうが安全なのでmThrowIfOutOfBoundsによって切り替える
			if( mThrowIfOutOfBounds == false )
			{
				_ACError("index out of bounds (>=mItemCount)",this);
			}
			else
			{
				_ACThrow("index out of bounds (>=mItemCount)",this);
			}
			return;
		}
		
		//#693 ファイルへのPurge中ならPurgeファイルから復元 ★未テスト
		if( mPurged == true )
		{
			mDataPtr = (T*)(APurgeManager::GetPurgeManager().RestoreFromPurge(this));
			mPurged = false;
		}
		
		if( mMutexEnabled == true )//AStMutexLockerのオブジェクト生成にも時間がかかるので高速化のため先に判定 #598
		{
			//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
			AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
			//設定
			mDataPtr[inIndex] = inData;
		}
		else
		{
			//設定
			mDataPtr[inIndex] = inData;
		}
	}
	
	//データ検索
  public:
	/**
	inTargetに一致するデータのindexを返す（最初に見つかった項目のindexを返す）
	*/
	AIndex	Find( const T inTarget ) const
	{
		//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		//検索
		for( AIndex i = 0; i < mItemCount; i++ )
		{
			if( mDataPtr[i] == inTarget )   return i;
		}
		return kIndex_Invalid;
	}
	
  public:
	/**
	内容入れ替え
	*/
	void	SwapContent( AArrayCore<T>& ioArrayObject )
	{
		//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		//#693 ファイルへのPurge中ならPurgeファイルから復元 ★未テスト
		if( mPurged == true )
		{
			mDataPtr = (T*)(APurgeManager::GetPurgeManager().RestoreFromPurge(this));
			mPurged = false;
		}
		//#693 メモリ未割り当てなら割り当て
		if( mDataPtr == NULL )
		{
			FirstMalloc();
		}
		
		//
		AItemCount	itemCount 			= mItemCount;
		T*	dataPtr						= mDataPtr;
		AItemCount	reallocateStepCount	= mReallocateStepCount;
		AByteCount	allocatedSize		= mAllocatedSize;
		
		mItemCount 						= ioArrayObject.mItemCount;
		mDataPtr						= ioArrayObject.mDataPtr;
		mReallocateStepCount			= ioArrayObject.mReallocateStepCount;
		mAllocatedSize					= ioArrayObject.mAllocatedSize;
		
		ioArrayObject.mItemCount		= itemCount;
		ioArrayObject.mDataPtr			= dataPtr;
		ioArrayObject.mReallocateStepCount = reallocateStepCount;
		ioArrayObject.mAllocatedSize	= allocatedSize;
	}
	
	//AStArrayPtrによるptr取得中のInsert/Delete防止
	//（コーディングミスによりInsert/Deleteをptr取得中にしてしまうのを防止する。
	//別スレッドからのInsert/Delete防止は、mMutexEnabled=trueにして、全体を排他制御する必要がある。
	//つまり、下記だけでは別スレッドとの排他制御はできない。（不整合発生しうるのは、ptr取得時だけではないため））
  private:
	/**
	Lock中かどうかを取得
	*/
	ABool	IsLockedByArrayPtr() const { return (mLockCount>0);}
	/**
	Lockする
	*/
	void	LockByArrayPtr() const 
	{
		//項目数0のときはmDataPtrがNULLの場合があるが、
		//項目数0なので、実際のアクセスが発生することもない。
		//（p[]の括弧内についてはいずれにしても使用する側で添字オーバーしないよう注意が必要。）
		AStMutexLocker	locker(mLockCountMutex);
		//ptr取得中は、ptr以外からのアクセスは禁止 #598
		if( mLockCount == 0 && mMutexEnabled == true )
		{
			mMutex.Lock();
		}
		mLockCount++;
	}
	/**
	Unlockする
	*/
	void	UnlockByArrayPtr() const 
	{
		AStMutexLocker	locker(mLockCountMutex);
		mLockCount--;
		//Mutex解除
		if( mLockCount == 0 && mMutexEnabled == true )
		{
			mMutex.Unlock();
		}
		//（念のため）LockCountマイナスなら0にする
		if( mLockCount < 0 )
		{
			_ACError("",this);
			mLockCount = 0;
		}
	}
	mutable AItemCount	mLockCount;
	mutable AThreadMutex	mLockCountMutex;
	
	//#693
	//ファイルへのPurge ★未テスト
  public:
	void	Purge()
	{
		//
		if( mDataPtr == NULL )   return;
		
		//Mutex取得待ち（mMutexEnabled=trueの場合に限る）
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		if( IsLockedByArrayPtr() == true )
		{
			//ArrayPtrによるロック中はPurgeしない
			_ACError("AArray purge while Locked",this);
		}
		else
		{
			//
			APurgeManager::GetPurgeManager().Purge(this,mDataPtr,mAllocatedSize);
			mPurged = true;
			mDataPtr = NULL;
			//mAllocatedSizeはそのままにする
			//debug
			sTotalAllocatedByteCount -= mAllocatedSize;
		}
	}
  private:
	mutable ABool mPurged;
	
	/**
	スレッドセーフEnable/Disable設定
	inEnable=trueの場合、AArrayの内部データについてはスレッドセーフとなる。
	（外部に渡したデータについてはスレッドセーフではない（内部データとの整合性保証されない）ので注意。
	たとえばGetItemCount()で項目数を取得した場合、その値はコール側でmutexをかけない限り、
	保証されない。（別スレッドでInsert/Deleteされる可能性がある。）
	デフォルトはDisable。（Get/Setにmutexをかけるので遅くなる。
	miの自動テスト実行処理時間比較：全てdisable:3分→全てenable:5分）
	*/
  public:
	void	EnableThreadSafe( const ABool inEnable ) { mMutexEnabled = inEnable; }
  private:
	//Arrayへのアクセスに対するMutex
  private:
	ABool	mMutexEnabled;
	mutable AThreadMutex	mMutex;
};

//可変長配列クラス
//基本型の可変長配列
//mDataPtrは直接アクセスしない（できない）
//AArrayCoreのReserve(), Delete(), Get(), Set()をコールすることで操作する
/**
可変長配列クラス
*/
template<class T> class AArray : public AArrayCore<T>
{
	//コンストラクタ／デストラクタ
  public:
	/**
	コンストラクタ
	*/
	AArray( AObjectArrayItem* inParent = NULL, const AItemCount inInitialAllocateCount = kInitialAllocateCountDefault, const AItemCount inReallocateStepCount = kReallocateStepCount ) //B0373
		: AArrayCore<T>(inParent,inInitialAllocateCount,inReallocateStepCount)
	{
	}
	virtual ~AArray()
	{
	}
	
	//挿入／追加
  public:
	/**
	１個のデータを挿入
	*/
	void	Insert1( const AIndex inIndex, const T& inItem )
	{
		AArrayCore<T>::Reserve(inIndex,1);
		AArrayCore<T>::Set(inIndex,inItem);
	}
	/**
	１個のデータを最後に追加
	*/
	AIndex	Add( const T& inItem )
	{
		AIndex	index = AArrayCore<T>::GetItemCount();
		Insert1(index,inItem);
		return index;
	}
	
	//削除
  public:
	/**
	１個のデータを削除
	*/
	void	Delete1( const AIndex inIndex )
	{
		AArrayCore<T>::Delete(inIndex,1);
	}
	/**
	全部のデータを削除
	*/
	void DeleteAll()
	{
		AArrayCore<T>::Delete(0,AArrayCore<T>::GetItemCount());
	}
	/**
	指定index以降を削除
	*/
	void DeleteAfter( const AIndex inIndex )
	{
		AArrayCore<T>::Delete(inIndex,AArrayCore<T>::GetItemCount()-inIndex);
	}
	/**
	一致する項目を削除
	*/
	void	FindAndDelete( const T& inItem )
	{
		AIndex	index = AArrayCore<T>::Find(inItem);//#1012
		if( index != kIndex_Invalid )
		{
			Delete1(index);
		}
	}
	
  public:
	/**
	オブジェクトの全データを追加
	@note AddFromObject()単体では未テスト
	*/
	void	AddFromObject( const AArray<T>& inSrc )//#717
	{
		/*#853 高速化
		for( AIndex index = 0; index < inSrc.GetItemCount(); index++ )
		{
			Add(inSrc.Get(index));
		}
		*/
		//追加前のdst側（*this)の項目数を記憶
		AItemCount	origDstItemCount = AArrayCore<T>::GetItemCount();
		//dst側のメモリ確保
		AArrayCore<T>::Reserve(origDstItemCount,inSrc.GetItemCount());
		//dst側ポインタ
		AStArrayPtr<T>	dstarrayptr(*this,0,AArrayCore<T>::GetItemCount());
		T*	dstp = dstarrayptr.GetPtr();
		//src側ポインタ
		AStArrayPtr<T>	srcarrayptr(inSrc,0,inSrc.GetItemCount());
		T*	srcp = srcarrayptr.GetPtr();
		//コピー
		AItemCount	srcItemCount = inSrc.GetItemCount();
		for( AIndex index = 0; index < srcItemCount; index++ )
		{
			dstp[origDstItemCount + index] = srcp[index];
		}
	}
	/**
	オブジェクトの全データを設定
	*/
	void	SetFromObject( const AArray<T>& inSrc )
	{
		DeleteAll();
		AddFromObject(inSrc);
	}
	
	//Swap
  public:
	/**
	添字inIndexAとinIndexBのデータ内容を入れ替える
	*/
	void	Swap( const AIndex inIndexA, const AIndex inIndexB )
	{
		T	tmp = AArrayCore<T>::Get(inIndexA);
		AArrayCore<T>::Set(inIndexA,AArrayCore<T>::Get(inIndexB));
		AArrayCore<T>::Set(inIndexB,tmp);
	}
	
	//Move
  public:
	/**
	移動
	*/
	void	Move( const AIndex inOldIndex, const AIndex inNewIndex )
	{
		T	tmp = AArrayCore<T>::Get(inOldIndex);
		Delete1(inOldIndex);
		Insert1(inNewIndex,tmp);
	}
	
	/*#695 高速化のためAArrayCore::Find()にて、直接ポインタを使用して検索するように変更
	//データ検索
  public:
	*
	inTargetに一致するデータのindexを返す
	*
	AIndex	Find( const T inTarget ) const
	{
		for( AIndex i = 0; i < AArrayCore<T>::GetItemCount(); i++ )
		{
			if( AArrayCore<T>::Get(i) == inTarget )   return i;
		}
		return kIndex_Invalid;
	}
	*/
	
	//Sort
  public:
	/**
	Sort
	@param inAscendant trueなら昇順
	*/
	void	Sort( const ABool inAscendant )
	{
		if( AArrayCore<T>::GetItemCount() <= 1 )   return;
		QuickSort(0,AArrayCore<T>::GetItemCount()-1,inAscendant);
	}
  private:
	/**
	Sort (QuickSort)
	@param inFirst  Start index
	@param inLast   End index (The item at inLast is included in sort items.)
	*/
	void	QuickSort( AIndex inFirst, AIndex inLast, const ABool inAscendant )
	{
		//Get pivot value
		T	pivot = AArrayCore<T>::Get((inFirst+inLast)/2);
		//Divide 
		AIndex	i = inFirst, j = inLast;
		for( ; ; )
		{
			for( ; ; )
			{
				T	data = AArrayCore<T>::Get(i);
				if( inAscendant == true )
				{
					if( data < pivot )
					{
						i++;
						continue;
					}
				}
				else
				{
					if( data > pivot )
					{
						i++;
						continue;
					}
				}
				break;
			}
			for( ; ; )
			{
				T	data = AArrayCore<T>::Get(j);
				if( inAscendant == true )
				{
					if( pivot < data )
					{
						j--;
						continue;
					}
				}
				else
				{
					if( pivot > data )
					{
						j--;
						continue;
					}
				}
				break;
			}
			if( i >= j )   break;
			Swap(i,j);
			i++;
			j--;
		}
		if( inFirst < i-1 )   QuickSort(inFirst,i-1,inAscendant);
		if( j+1 < inLast  )   QuickSort(j+1,inLast,inAscendant);
	}
	
};

//各種基本データ型配列の定義
typedef AArray<ABool> ABoolArray;
typedef AArray<ANumber> ANumberArray;
typedef AArray<AColor> AColorArray;
typedef AArray<AObjectID>	AObjectIDArray;




