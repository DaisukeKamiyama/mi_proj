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

ATextArray

*/

#pragma once

#include "AText.h"
#include "AHashObjectArray.h"
#include "AUniqIDArray.h"

class AHashTextArray;

//#693
#if 0
/**
TextArrayクラス（Textの配列）
*/
class ATextArray : public AObjectArrayItem
{
  public:
	//コンストラクタ、デストラクタ
	ATextArray( AObjectArrayItem* inParent = NULL );
	virtual ~ATextArray();
	//
	void	Set( const ATextArray& inTextArray );
	void	Add( const ATextArray& inTextArray );
	//
	void	Insert1( const AIndex inIndex, const AText& inText );
	void	Insert1_SwapContent( const AIndex inIndex, AText& inText );
	void	Delete1( const AIndex inIndex );
	void	Set( const AIndex inIndex, const AText& inText );
	void	Set_SwapContent( const AIndex inIndex, AText& ioText );
	void	Get( const AIndex inIndex, AText& outText ) const;
	void	MoveObject( const AIndex inOldIndex, const AIndex inNewIndex );
	const AText&	GetTextConst( const AIndex inIndex ) const;
	AIndex	Find( const AText& inText ) const;
	AItemCount	GetItemCount() const {return mTextArray.GetItemCount();}
	AItemCount	GetTextLength( const AIndex inIndex ) const;
	
	//挿入、削除
	void	Add( const AText& inText );
	void	Add_SwapContent( AText& inText );
	void	Add( AConstCharPtr inText );
	void	DeleteAll();
	//Implode/Explode
	void	Implode( const AChar& inChar, AText& outText ) const;
	void	ExplodeFromText( const AText& inText, const AUChar inDelimiter );
	void	ExplodeLineFromText( const AText& inText );
	//検索
	AIndex	FindIgnoreCase( const AText& inText ) const;
  private:
	AObjectArray<AText>	mTextArray;
	
	//#598
	/**
	スレッドセーフEnable/Disable設定
	inEnable=trueの場合、ATextArrayの内部データ（ただしArray要素のObjectは含まない）についてはスレッドセーフとなる。
	（外部に渡したデータについてはスレッドセーフではない（内部データとの整合性保証されない）ので注意。
	たとえばGetItemCount()で項目数を取得した場合、その値はコール側でmutexをかけない限り、
	保証されない。（別スレッドでInsert/Deleteされる可能性がある。）
	また、Array要素のObjectもスレッドセーフではないので、必要なら別途mutex必要。
	デフォルトはEnable。
	*/
  public:
	void	EnableThreadSafe( const ABool inEnable )
	{
		mTextArray.EnableThreadSafe(inEnable);
	}
	
  public:
	void	Sort( const ABool inAscendant );
  private:
	void	QuickSort( AIndex first, AIndex last, const ABool inAscendant );
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "ATextArray"; }
	
};

/**
HashTextArrayクラス（Textの配列・ハッシュ検索可能）
*/
class AHashTextArray : public AObjectArrayItem
{
  public:
	//コンストラクタ、デストラクタ
	AHashTextArray( AObjectArrayItem* inParent = NULL );
	virtual ~AHashTextArray();
	//
	void				Insert1( const AIndex inIndex, const AText& inText );
	AIndex				Add( const AText& inText );
	void				Delete1( const AIndex inIndex );
	void				DeleteAll();
	void				Set( const AIndex inIndex, const AText& inText );
	void				Get( const AIndex inIndex, AText& outText ) const;
	const AText&		GetTextConst( const AIndex inIndex ) const;
	ABool				Compare( const AIndex inIndex, const AText& inText ) const;//#348
	AIndex				Find( const AText& inTargetText, const AIndex inTargetIndex = 0, const AItemCount inTargetItemCount = kIndex_Invalid ) const;
	void				FindObjectID( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount,
						AArray<AObjectID> &outObjectIDArray ) const;//#0
	AObjectID			FindObjectID( const AText& inTargetText, const AIndex inTargetIndex = 0, const AItemCount inTargetItemCount = kIndex_Invalid ) const;//#348
	AObjectID			GetObjectIDOfText( const AIndex inIndex ) const;
	AIndex				GetIndexByID( const AObjectID inObjectID ) const;
	AItemCount			GetItemCount() const {return mTextArray.GetItemCount();}
  private:
	AHashObjectArray<AText,AText>	mTextArray;
	
	//#598
	/**
	スレッドセーフEnable/Disable設定
	inEnable=trueの場合、AHashTextArrayの内部データ（ただしArray要素のObjectは含まない）についてはスレッドセーフとなる。
	（外部に渡したデータについてはスレッドセーフではない（内部データとの整合性保証されない）ので注意。
	たとえばGetItemCount()で項目数を取得した場合、その値はコール側でmutexをかけない限り、
	保証されない。（別スレッドでInsert/Deleteされる可能性がある。）
	また、Array要素のObjectもスレッドセーフではないので、必要なら別途mutex必要。
	デフォルトはEnable。
	*/
  public:
	void	EnableThreadSafe( const ABool inEnable )
	{
		mTextArray.EnableThreadSafe(inEnable);
	}
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AHashTextArray"; }
	
};
#endif

//#348
/**
ATextArrayクラス（Textの配列・メモリ節約）
*/
class ATextArray : public AObjectArrayItem
{
  public:
	ATextArray( AObjectArrayItem* inParent = NULL );
	virtual ~ATextArray();
	
	//追加・削除
  public:
	virtual void		Insert1( const AIndex inIndex, const AText& inText );
	virtual void		Delete1( const AIndex inIndex );
	virtual AIndex		Add( const AText& inText );
	virtual void		DeleteAll();
	void				DeleteAfter( const AIndex inIndex );
	virtual void		Reserve( const AIndex inIndex, const AItemCount inCount);//#695
	
	//設定・取得
  public:
	virtual void		Set( const AIndex inIndex, const AText& inText );
	virtual void		Get( const AIndex inIndex, AText& outText ) const;
	virtual ABool		Compare( const AIndex inIndex, const AText& inText ) const;
	virtual ABool		Compare( const AIndex inIndex, 
						const AText& inText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const;//#695
	virtual AItemCount	GetItemCount() const { return mSposArray.GetItemCount(); }
	virtual const AText	GetTextConst( const AIndex inIndex ) const;//#693
	virtual void		MoveObject( const AIndex inOldIndex, const AIndex inNewIndex );//#693
	virtual void		SwapObject( const AIndex inA, const AIndex inB );//#693
	//TextArrayから設定・追加
	virtual void		SetFromTextArray( const ATextArray& inTextArray );//#693
	virtual void		AddFromTextArray( const ATextArray& inTextArray );//#693
	//HashTextArrayから設定・追加
	virtual void		AddFromTextArray( const AHashTextArray& inTextArray );//#717
	virtual void		SetFromTextArray( const AHashTextArray& inTextArray );//#717
	
	//検索
  public:
	virtual AIndex		Find( const AText& inTargetText, const AIndex inTargetIndex = 0, 
						const AItemCount inTargetItemCount = kIndex_Invalid ) const;//#693
	virtual void		FindAll( const AText& inTargetText, const AIndex inTargetIndex, 
						const AItemCount inTargetItemCount, AArray<AIndex>& outFoundIndex ) const;//#723
	AIndex				FindIgnoreCase( const AText& inText ) const;//#693
	
	//Implode/Explode
	void				Implode( const AChar& inChar, AText& outText ) const;//#693
	void				ExplodeFromText( const AText& inText, const AUChar inDelimiter, const ABool inRemoveEmptyItem = false );//#693
	void				ExplodeLineFromText( const AText& inText );//#693
	
	//ソート
  public:
	virtual void		Sort( const ABool inAscendant );//#693
  private:
	void				QuickSort( AIndex first, AIndex last, const ABool inAscendant );//#693
	
	/*#717
	//
  public:
	void				SortUsingOrderArray( const ABool inAscendant, AArray<AIndex>& outOrderArray );
  private:
	void				QuickSortUsingOrderArray( const AIndex first, const AIndex last, const ABool inAscendant, AArray<AIndex>& ioOrderArray );
	*/
	
  public:
	AItemCount			GetTextLen( const AIndex inIndex ) const;//#450
  protected:
	void				GetRange( const AIndex inIndex, AIndex& spos, AIndex& epos ) const;
	
	//データ
  protected:
	AText							mTextData;
	AArray<AIndex>					mSposArray;
	
  public:
	AByteCount			GetTextDataSize() const { return mTextData.GetItemCount(); }
	
};

/**
AHashTextArrayクラス（Textの配列・メモリ節約）
*/
class AHashTextArray : public ATextArray
{
  public:
	AHashTextArray( AObjectArrayItem* inParent = NULL );
	~AHashTextArray();
	
	//追加・削除
  public:
	void				Insert1( const AIndex inIndex, const AText& inText );
	void				Delete1( const AIndex inIndex );
	AIndex				Add( const AText& inText );//#695
	void				DeleteAll();//#695
	void				DeleteItemsOfText( const AText& inText );//#65
	void				Reserve( const AIndex inIndex, const AItemCount inCount)
	{ _ACThrow("not supported",NULL); }//#695
	
	//設定・取得
  public:
	void				Set( const AIndex inIndex, const AText& inText );
	void				Get( const AIndex inIndex, AText& outText ) const 
	{ ATextArray::Get(inIndex,outText); }//#695
	ABool				Compare( const AIndex inIndex, const AText& inText ) const
	{ return ATextArray::Compare(inIndex,inText); }//#695
	ABool				Compare( const AIndex inIndex, 
						const AText& inText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
	{ return ATextArray::Compare(inIndex,inText,inTargetIndex,inTargetItemCount); }//#695
	AItemCount			GetItemCount() const 
	{ return ATextArray::GetItemCount(); }//#695
	const AText			GetTextConst( const AIndex inIndex ) const
	{ return ATextArray::GetTextConst(inIndex); }//#695
	void				MoveObject( const AIndex inOldIndex, const AIndex inNewIndex );//#695
	void				SwapObject( const AIndex inA, const AIndex inB );//#695
	//TextArrayから設定・追加
	void				SetFromTextArray( const ATextArray& inTextArray );//#853
	void				AddFromTextArray( const ATextArray& inTextArray );//#853
	//HashTextArrayから設定・追加
	void				AddFromTextArray( const AHashTextArray& inTextArray );//#717
	void				SetFromTextArray( const AHashTextArray& inTextArray );//#717
	
	//ソート
  public:
	void				Sort( const ABool inAscendant )
	{ _ACError("not supported",NULL); }//#695
	
	//UniqID #693
  public:
	AUniqID				GetUniqIDByIndex( const AIndex inIndex ) const;
	AIndex				GetIndexByUniqID( const AUniqID inUniqID ) const;
  private:
	AUniqIDArray					mUniqIDArray;
	
	/*#693
	//ObjectID
  public:
	AObjectID			GetObjectIDByIndex( const AIndex inIndex ) const;
	AIndex				GetIndexByObjectID( const AObjectID inObjectID ) const;
  private:
	ANumber				AssignObjectID();
	AHashArray<ANumber>				mObjectIDHashArray;
	
	ANumber							mNextObjectIDCandidate;
	
	static const ANumber			kObjectID_Min = 1;
	static const ANumber			kObjectID_Max = 0x7FFFFFFF;
	*/
	//検索
  public:
	AIndex				Find( const AText& inTargetText, const AIndex inTargetIndex = 0, 
						const AItemCount inTargetItemCount = kIndex_Invalid ) const;
	void				FindAll( const AText& inTargetText, const AIndex inTargetIndex, 
						const AItemCount inTargetItemCount, AArray<AIndex>& outFoundIndex ) const;
	AUniqID				FindUniqID( const AText& inTargetText, const AIndex inTargetIndex = 0, 
						const AItemCount inTargetItemCount = kIndex_Invalid ) const;//#693
	void				FindUniqIDs( const AText& inTargetText, const AIndex inTargetIndex, 
						const AItemCount inTargetItemCount, AArray<AUniqID> &outUniqIDArray ) const;//#693
	void				MakeHash();
  private:
	void				UpdateHash_DataAdded( const AUniqID inUniqID, const AText& inText );//#695
	void 				UpdateHash_DataDeleting( const AUniqID inUniqID, const AText& inDeletingText );//#695
	void				RegisterToHash( const AUniqID inUniqID, //#695
						const AText& inText, const AIndex inTextIndex, const AItemCount inTextItemCount );
	unsigned long		GetHashValue( const AText& inData, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const;
	
	//ハッシュ
	//配列データのインデックスを格納する。
  private:
	AArray<AUniqID>					mHash;//#695
	AItemCount						mHash_DeletedCount;
	
};



