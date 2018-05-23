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

AObjectArrayItem

*/

#pragma once

#include "ABaseTypes.h"

class AFileAcc;
class AObjectArrayItem;

//#693 ABaseTypes.hから移動し、valをポインタに変更
/**
ObjectID定義
*/
struct AObjectID
{
	AObjectArrayItem*	val;
	//ポインタからObjectIDを作成 #904
	static AObjectID	GetObjectIDFromPtr( AObjectArrayItem* inPtr )
	{ AObjectID	objID = {inPtr}; return objID; }
	//operator
	bool operator==(AObjectID inObjectID) const { return (val==inObjectID.val); }
	bool operator!=(AObjectID inObjectID) const { return (val!=inObjectID.val); }
};
const AObjectID	kObjectID_Invalid = {NULL};


#pragma mark ===========================
#pragma mark [クラス]AObjectArrayItem
/**
AObjectArray<>に保持されるクラスの基底クラス
ObjectIDを管理する
*/
class AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	AObjectArrayItem( AObjectArrayItem* inParent );
	virtual ~AObjectArrayItem();
	virtual void	DoDeleted() {}//AObjectArrayからの削除直後にコールされる関数
	
	//debug
  public:
	virtual AByteCount	GetAllocatedSize() const;//#271 { return 0; }
	
	//ObjectID設定／取得
  public:
	//#693 inline void	SetObjectID( const AObjectID inObjectID ) { mObjectID = inObjectID; }
	inline AObjectID	GetObjectID() const { return mObjectID; }
  private:
	AObjectID	mObjectID;
	AObjectArrayItem*	mParentObjectArrayItem;
	
	//RetainCount制御 #717
	//RetainCount > 0 の間はAObjectArrayItemTrashでメモリ解放されない
	//AObjectArrayIDIndexed::GetObjectAndRetain()でincrementされる。
  public:
	void					IncrementRetainCount() { mRetainCount++; }
	void					DecrementRetainCount() { mRetainCount--; }
	AItemCount				GetRetainCount() const { return mRetainCount; }
  private:
	AItemCount							mRetainCount;
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AObjectArrayItem"; }
	const AObjectArrayItem*	GetParent() const { return mParentObjectArrayItem; }
	
	/*#693
	//ObjectID管理(static)
  private:
	static AObjectID	RegisterObject( AObjectArrayItem* inObject );
	static void	UnregisterObject( const AObjectID inObjectID );
	static AIndex	GetNextHashPos( const AIndex inHashPosition );
	static AIndex	FindHashIndexByObjectID( const AObjectID inObjectID );
	static void	RegisterToHash( const AObjectID inObjectID, AObjectArrayItem* inObject );
	static void	DeleteFromHash( const AObjectID inObjectID );
	static void	MakeHash();
	static void	WriteToHash( const AObjectID inObjectID, AObjectArrayItem* inObject );
	static unsigned long	GetHashValue( const AObjectID inObjecID );
	*/
	//デバッグ
  public:
	static void	GetStatics( AItemCount& outHashSize, AItemCount& outItemCount, AItemCount& outNoDataCount, AItemCount& outDeletedCount,
			ANumber& outNextCandidate );
	static void	DumpObjectID( const AFileAcc& inDumpFile );
	
	//コピー生成子／代入演算子はサブクラスで明示的に定義されない限り、使用不可。
  protected:
	AObjectArrayItem( const AObjectArrayItem& );//コピー生成子
	AObjectArrayItem& operator=( const AObjectArrayItem& );//代入演算子
};

/**
ブロックを抜けるときにAObjectArrayItemのDecrementRetainCount()をコールすることを保証するためのstackクラス
*/
class AStDecrementRetainCounter
{
  public:
	AStDecrementRetainCounter( AObjectArrayItem* inPtr ) : mPtr(inPtr) {}
	~AStDecrementRetainCounter()
	{ mPtr->DecrementRetainCount(); }
	//（retainされているので、ポインタはデストラクタまで有効。）
  private:
	AObjectArrayItem*	mPtr;
};


