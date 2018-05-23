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

#include "stdafx.h"

#include "ATextArray.h"
#if 0
#pragma mark ===========================
#pragma mark [クラス]ATextArray
#pragma mark ===========================
//ATextの配列

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
ATextArray::ATextArray( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent)
{
}
//デストラクタ
ATextArray::~ATextArray()
{
}

void	ATextArray::Insert1( const AIndex inIndex, const AText& inText )
{
	mTextArray.InsertNew1Object(inIndex);
	Set(inIndex,inText);
}

void	ATextArray::Insert1_SwapContent( const AIndex inIndex, AText& inText )
{
	mTextArray.InsertNew1Object(inIndex);
	Set_SwapContent(inIndex,inText);
}

void	ATextArray::Delete1( const AIndex inIndex )
{
	mTextArray.Delete1Object(inIndex);
}

void	ATextArray::Set( const AIndex inIndex, const AText& inText )
{
	mTextArray.GetObject(inIndex).SetText(inText);
}

void	ATextArray::Set_SwapContent( const AIndex inIndex, AText& ioText )
{
	mTextArray.GetObject(inIndex).SwapContent(ioText);
}

void	ATextArray::Get( const AIndex inIndex, AText& outText ) const
{
	outText.SetText(mTextArray.GetObjectConst(inIndex));
}

const AText&	ATextArray::GetTextConst( const AIndex inIndex ) const
{
	return mTextArray.GetObjectConst(inIndex);
}

void	ATextArray::Add( const ATextArray& inTextArray )
{
	AItemCount	count = inTextArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AText	text;
		inTextArray.Get(i,text);
		Add(text);
	}
}

void	ATextArray::Set( const ATextArray& inTextArray )
{
	DeleteAll();
	Add(inTextArray);
}

void	ATextArray::Add( const AText& inText )
{
	Insert1(GetItemCount(),inText);
}

void	ATextArray::Add_SwapContent( AText& inText )
{
	Insert1_SwapContent(GetItemCount(),inText);
}

void	ATextArray::Add( AConstCharPtr inText )
{
	AText	text(inText);
	Add(text);
}

void	ATextArray::MoveObject( const AIndex inOldIndex, const AIndex inNewIndex )
{
	mTextArray.MoveObject(inOldIndex,inNewIndex);
}

void	ATextArray::DeleteAll()
{
	while( GetItemCount() > 0 )
	{
		Delete1(GetItemCount()-1);
	}
}

void	ATextArray::Implode( const AChar& inChar, AText& outText ) const
{
	outText.DeleteAll();
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AText	text;
		Get(i,text);
		outText.AddText(text);
		outText.Add(inChar);
	}
}

//指定デリミタ(inDelimiter)で文字列を区切り、TextArrayに順に格納する。
//デリミタが2文字連続する場合は、空白ATextが格納される
//最後の文字がデリミタの場合は、最後に空白ATextが格納される
void	ATextArray::ExplodeFromText( const AText& inText, const AUChar inDelimiter )
{
	//最初にTextArray全消去
	DeleteAll();
	//デリミタを見つける毎にTextArrayに追加
	AIndex	spos = 0;
	AIndex pos = 0;
	for( ; pos < inText.GetItemCount(); pos++ )
	{
		AUChar	ch = inText.Get(pos);
		if( ch == inDelimiter )
		{
			AText	text;
			inText.GetText(spos,pos-spos,text);
			Add(text);
			spos = pos+1;
		}
	}
	//最後のデリミタ以降の文字を追加
	AText	text;
	inText.GetText(spos,pos-spos,text);
	Add(text);
}

//
void	ATextArray::ExplodeLineFromText( const AText& inText )
{
	//CRへ変換
	AText	text;
	text.SetText(inText);
	text.ConvertReturnCodeToCR();
	//CRでExplode
	ExplodeFromText(text,kUChar_CR);
}

AIndex	ATextArray::Find( const AText& inText ) const
{
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AText	text;
		Get(i,text);
		if( text.Compare(inText) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

AIndex	ATextArray::FindIgnoreCase( const AText& inText ) const
{
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AText	text;
		Get(i,text);
		if( text.CompareIgnoreCase(inText) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

AItemCount	ATextArray::GetTextLength( const AIndex inIndex ) const
{
	return mTextArray.GetObjectConst(inIndex).GetItemCount();
}

void	ATextArray::Sort( const ABool inAscendant )
{
	if( GetItemCount() < 2 )   return;
	QuickSort(0,GetItemCount()-1,inAscendant);
}

void	ATextArray::QuickSort( AIndex first, AIndex last, const ABool inAscendant )
{
	AText	axis;
	Get((first+last)/2,axis);
	AIndex	i = first, j = last;
	for( ; ; )
	{
		for( ; ; )
		{
			if( inAscendant == true )
			{
				if( GetTextConst(i).IsLessThan(axis) == true )
				{
					i++;
					continue;
				}
			}
			else
			{
				if( axis.IsLessThan(GetTextConst(i)) == true )
				{
					i++;
					continue;
				}
			}
			break;
		}
		for( ; ; )
		{
			if( inAscendant == true )
			{
				if( axis.IsLessThan(GetTextConst(j)) == true )
				{
					j--;
					continue;
				}
			}
			else
			{
				if( GetTextConst(j).IsLessThan(axis) == true )
				{
					j--;
					continue;
				}
			}
			break;
		}
		if( i >= j )   break;
		mTextArray.SwapObject(i,j);
		i++;
		j--;
	}
	if( first < i-1 )   QuickSort(first,i-1,inAscendant);
	if( j+1 < last  )   QuickSort(j+1,last,inAscendant);
}

#pragma mark ===========================
#pragma mark [クラス]AHashTextArray
#pragma mark ===========================
//ATextの配列（ハッシュあり）

AHashTextArray::AHashTextArray( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent)
{
}

AHashTextArray::~AHashTextArray()
{
}

void	AHashTextArray::Insert1( const AIndex inIndex, const AText& inText )
{
	mTextArray.InsertNew1Object(inIndex);
	mTextArray.ObjectUpdateStart(inIndex);
	mTextArray.GetObject(inIndex).SetText(inText);
	mTextArray.ObjectUpdateEnd(inIndex);
}

AIndex	AHashTextArray::Add( const AText& inText )
{
	AIndex	index = GetItemCount();
	AHashTextArray::Insert1(index,inText);
	return index;
}

void	AHashTextArray::Delete1( const AIndex inIndex )
{
	mTextArray.Delete1Object(inIndex);
}

void	AHashTextArray::DeleteAll()
{
	while( GetItemCount() > 0 )
	{
		Delete1(GetItemCount()-1);
	}
}

void	AHashTextArray::Set( const AIndex inIndex, const AText& inText )
{
	mTextArray.ObjectUpdateStart(inIndex);
	mTextArray.GetObject(inIndex).SetText(inText);
	mTextArray.ObjectUpdateEnd(inIndex);
}

void	AHashTextArray::Get( const AIndex inIndex, AText& outText ) const
{
	outText.SetText(mTextArray.GetObjectConst(inIndex));
}

const AText&	AHashTextArray::GetTextConst( const AIndex inIndex ) const
{
	return mTextArray.GetObjectConst(inIndex);
}

//#348
/**
テキスト比較
*/
ABool	AHashTextArray::Compare( const AIndex inIndex, const AText& inText ) const
{
	return GetTextConst(inIndex).Compare(inText);
}

AIndex	AHashTextArray::Find( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	return mTextArray.Find(inTargetText,inTargetIndex,inTargetItemCount);
}

//#0
/**
一致するテキストのObjectIDのリストを取得
*/
void	AHashTextArray::FindObjectID( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount,
		AArray<AObjectID> &outObjectIDArray ) const
{
	mTextArray.FindObjectID(inTargetText,inTargetIndex,inTargetItemCount,outObjectIDArray);
}

//#348
AObjectID	AHashTextArray::FindObjectID( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	return mTextArray.FindObjectID(inTargetText,inTargetIndex,inTargetItemCount);
}

AObjectID	AHashTextArray::GetObjectIDOfText( const AIndex inIndex ) const
{
	return mTextArray.GetObjectConst(inIndex).GetObjectID();
}

AIndex	AHashTextArray::GetIndexByID( const AObjectID inObjectID ) const
{
	return mTextArray.GetIndexByID(inObjectID);
}
#endif

#pragma mark ===========================
#pragma mark [クラス]ATextArray
#pragma mark ===========================
//#348

/**
コンストラクタ
*/
ATextArray::ATextArray( AObjectArrayItem* inParent ) : AObjectArrayItem(NULL)
{
}

/**
デストラクタ
*/
ATextArray::~ATextArray()
{
}

#pragma mark ===========================

#pragma mark ---追加・削除

/**
追加
*/
void	ATextArray::Insert1( const AIndex inIndex, const AText& inText )
{
	//挿入位置取得
	AIndex	spos;
	if( inIndex >= mSposArray.GetItemCount() )
	{
		spos = mTextData.GetItemCount();
	}
	else
	{
		spos = mSposArray.Get(inIndex);
	}
	//テキストデータ挿入
	mTextData.InsertText(spos,inText);
	//位置データ挿入
	mSposArray.Insert1(inIndex,spos);
	//追加位置以降をずらす
	AItemCount	len = inText.GetItemCount();
	/*#695 高速化
	for( AIndex i = inIndex+1; i < mSposArray.GetItemCount(); i++ )
	{
		mSposArray.Set(i,mSposArray.Get(i)+len);
	}
	*/
	{
		AItemCount	itemCount = mSposArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mSposArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = inIndex+1; i < itemCount; i++ )
		{
			p[i] += len;
		}
	}
}

/**
削除
*/
void	ATextArray::Delete1( const AIndex inIndex )
{
	//削除範囲取得
	AIndex	spos, epos;
	GetRange(inIndex,spos,epos);
	//テキストデータ削除
	mTextData.Delete(spos,epos-spos);
	//位置データ削除
	mSposArray.Delete1(inIndex);
	//ずらす
	AItemCount	len = epos-spos;
	/*#695 高速化
	for( AIndex i = inIndex; i < mSposArray.GetItemCount(); i++ )
	{
		mSposArray.Set(i,mSposArray.Get(i)-len);
	}
	*/
	{
		AItemCount	itemCount = mSposArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mSposArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = inIndex; i < itemCount; i++ )
		{
			p[i] -= len;
		}
	}
}

/**
追加
*/
AIndex	ATextArray::Add( const AText& inText )
{
	Insert1(mSposArray.GetItemCount(),inText);
	return mSposArray.GetItemCount()-1;
}

/**
全削除
*/
void	ATextArray::DeleteAll()
{
	/*#695
	while( mSposArray.GetItemCount() > 0 )
	{
		Delete1(mSposArray.GetItemCount()-1);
	}
	*/
	mTextData.DeleteAll();
	mSposArray.DeleteAll();
}

/**
指定index以降を削除
*/
void	ATextArray::DeleteAfter( const AIndex inIndex )
{
	//項目数以降を削除の場合は、何もしないで終了（AArray::DeleteAfter()を実行しても問題無いが、mSposArray.Get()でエラーになるので。）
	if( inIndex == GetItemCount() )
	{
		return;
	}
	
	//mTextDataの指定indexの開始位置以降を削除する
	AIndex	spos = mSposArray.Get(inIndex);
	mTextData.DeleteAfter(spos);
	//mSposArrayの指定index以降を削除する
	mSposArray.DeleteAfter(inIndex);
}


//#695
/**
領域確保（空テキストが指定inIndexからinCount個設定される）
*/
void	ATextArray::Reserve( const AIndex inIndex, const AItemCount inCount )
{
	//挿入しようとしているindexの、mTextData内posを取得
	AIndex	pos = mTextData.GetItemCount();
	if( inIndex < mSposArray.GetItemCount() )
	{
		pos = mSposArray.Get(inIndex);
	}
	//spos arrayの領域確保
	mSposArray.Reserve(inIndex,inCount);
	//確保したspos arrayの項目に、上で取得したposを設定（全ての項目に同じpos）
	AStArrayPtr<AIndex>	arrayptr(mSposArray,0,mSposArray.GetItemCount());
	AIndex*	p = arrayptr.GetPtr();
	for( AIndex i = inIndex; i < inIndex + inCount; i++ )
	{
		p[i] = pos;
	}
}

#pragma mark ===========================

#pragma mark ---設定・取得

/**
テキスト設定
*/
void	ATextArray::Set( const AIndex inIndex, const AText& inText )
{
	//削除範囲取得
	AIndex	spos, epos;
	GetRange(inIndex,spos,epos);
	//テキストデータ削除
	mTextData.Delete(spos,epos-spos);
	//テキストデータ挿入
	mTextData.InsertText(spos,inText);
	//ずらす
	AIndex	offset = inText.GetItemCount()-(epos-spos);
	/*#695 高速化
	for( AIndex i = inIndex+1; i < mSposArray.GetItemCount(); i++ )
	{
		mSposArray.Set(i,mSposArray.Get(i)+offset);
	}
	*/
	{
		AItemCount	itemCount = mSposArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mSposArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = inIndex+1; i < itemCount; i++ )
		{
			p[i] += offset;
		}
	}
}

/**
テキスト取得
*/
void	ATextArray::Get( const AIndex inIndex, AText& outText ) const
{
	//範囲取得
	AIndex	spos, epos;
	GetRange(inIndex,spos,epos);
	//テキストデータ取得
	mTextData.GetText(spos,epos-spos,outText);
}

//#693 ATextArrayがATextオブジェクトの配列だったときは、そのATextへの参照を返していたが、
//ATextオブジェクトの配列ではなくなったので、オブジェクトのテンポラリコピーを返すように変更。
/**
const Textを取得
★速度検討必要
*/
const AText	ATextArray::GetTextConst( const AIndex inIndex ) const
{
	AText	result;
	Get(inIndex,result);
	return result;
}

/**
テキスト比較
*/
ABool	ATextArray::Compare( const AIndex inIndex, const AText& inText ) const
{
	//範囲取得
	AIndex	spos, epos;
	GetRange(inIndex,spos,epos);
	//テキストデータ比較
	return inText.Compare(mTextData,spos,epos-spos);
}

/**
テキスト比較 #695
*/
ABool	ATextArray::Compare( const AIndex inIndex, 
		const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	//範囲取得
	AIndex	spos, epos;
	GetRange(inIndex,spos,epos);
	//テキストデータ比較
	return inTargetText.Compare(inTargetIndex,inTargetItemCount,mTextData,spos,epos-spos);
}

/**
指定indexのテキストの長さを取得
*/
AItemCount	ATextArray::GetTextLen( const AIndex inIndex ) const
{
	AIndex	spos = 0, epos = 0;
	GetRange(inIndex,spos,epos);
	return epos-spos;
}

/**
範囲取得
*/
void	ATextArray::GetRange( const AIndex inIndex, AIndex& spos, AIndex& epos ) const
{
	spos = mSposArray.Get(inIndex);
	if( inIndex+1 >= mSposArray.GetItemCount() )
	{
		epos = mTextData.GetItemCount();
	}
	else
	{
		epos = mSposArray.Get(inIndex+1);
	}
}

/**
移動
*/
void	ATextArray::MoveObject( const AIndex inOldIndex, const AIndex inNewIndex )
{
	AText	text;
	Get(inOldIndex,text);
	Delete1(inOldIndex);
	Insert1(inNewIndex,text);
}

/**
Swap
*/
void	ATextArray::SwapObject( const AIndex inA, const AIndex inB )
{
	AText	textA, textB;
	Get(inA,textA);
	Get(inB,textB);
	Set(inA,textB);
	Set(inB,textA);
}

/**
ATextArrayから追加
*/
void	ATextArray::AddFromTextArray( const ATextArray& inTextArray )
{
	AItemCount	count = inTextArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AText	text;
		inTextArray.Get(i,text);
		Add(text);
	}
}

/**
ATextArrayから設定
*/
void	ATextArray::SetFromTextArray( const ATextArray& inTextArray )
{
	mTextData.SetText(inTextArray.mTextData);
	mSposArray.SetFromObject(inTextArray.mSposArray);
}

/**
AHashTextArrayから追加
*/
void	ATextArray::AddFromTextArray( const AHashTextArray& inTextArray )
{
	AItemCount	count = inTextArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AText	text;
		inTextArray.Get(i,text);
		Add(text);
	}
}

/**
ATextArrayから設定
*/
void	ATextArray::SetFromTextArray( const AHashTextArray& inTextArray )
{
	DeleteAll();
	AddFromTextArray(inTextArray);
}

#pragma mark ===========================

#pragma mark ---検索

/**
検索
*/
AIndex	ATextArray::Find( const AText& inTargetText, const AIndex inTargetIndex, 
		const AItemCount inTargetItemCount ) const
{
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		/*#723
		AText	text;
		Get(i,text);
		if( text.Compare(inTargetText,inTargetIndex,inTargetItemCount) == true )
		{
			return i;
		}
		*/
		//高速化 #723
		if( Compare(i,inTargetText,inTargetIndex,inTargetItemCount) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

//#723
/**
検索（Index取得・一致するもの全て）
*/
void	ATextArray::FindAll( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount, 
		AArray<AIndex>& outFoundIndex ) const
{
	//結果クリア
	outFoundIndex.DeleteAll();
	//各項目と比較
	AItemCount	itemCount = GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		if( Compare(i,inTargetText,inTargetIndex,inTargetItemCount) == true )
		{
			outFoundIndex.Add(i);
		}
	}
}

/**
検索（大文字小文字無視）
*/
AIndex	ATextArray::FindIgnoreCase( const AText& inText ) const
{
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AText	text;
		Get(i,text);
		if( text.CompareIgnoreCase(inText) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

/**
TextArrayの各要素を指定デリミタで区切ってTextを生成する。
最後にもデリミタが付く
*/
void	ATextArray::Implode( const AChar& inChar, AText& outText ) const
{
	outText.DeleteAll();
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AText	text;
		Get(i,text);
		outText.AddText(text);
		outText.Add(inChar);
	}
}

//指定デリミタ(inDelimiter)で文字列を区切り、TextArrayに順に格納する。
//デリミタが2文字連続する場合は、空白ATextが格納される
//最後の文字がデリミタの場合は、最後に空白ATextが格納される
void	ATextArray::ExplodeFromText( const AText& inText, const AUChar inDelimiter, const ABool inRemoveEmptyItem )
{
	//最初にTextArray全消去
	DeleteAll();
	//デリミタを見つける毎にTextArrayに追加
	AIndex	spos = 0;
	AIndex pos = 0;
	for( ; pos < inText.GetItemCount(); pos++ )
	{
		AUChar	ch = inText.Get(pos);
		if( ch == inDelimiter )
		{
			AText	text;
			inText.GetText(spos,pos-spos,text);
			if( inRemoveEmptyItem == false || text.GetItemCount() > 0 )
			{
				Add(text);
			}
			spos = pos+1;
		}
	}
	//最後のデリミタ以降の文字を追加
	AText	text;
	inText.GetText(spos,pos-spos,text);
	if( inRemoveEmptyItem == false || text.GetItemCount() > 0 )
	{
		Add(text);
	}
}

//
void	ATextArray::ExplodeLineFromText( const AText& inText )
{
	//CRへ変換
	AText	text;
	text.SetText(inText);
	text.ConvertReturnCodeToCR();
	//CRでExplode
	ExplodeFromText(text,kUChar_CR);
}

/**
ソート
*/
void	ATextArray::Sort( const ABool inAscendant )
{
	if( GetItemCount() < 2 )   return;
	QuickSort(0,GetItemCount()-1,inAscendant);
}

void	ATextArray::QuickSort( AIndex first, AIndex last, const ABool inAscendant )
{
	AText	axis;
	Get((first+last)/2,axis);
	AIndex	i = first, j = last;
	for( ; ; )
	{
		for( ; ; )
		{
			if( inAscendant == true )
			{
				if( GetTextConst(i).IsLessThan(axis) == true )
				{
					i++;
					continue;
				}
			}
			else
			{
				if( axis.IsLessThan(GetTextConst(i)) == true )
				{
					i++;
					continue;
				}
			}
			break;
		}
		for( ; ; )
		{
			if( inAscendant == true )
			{
				if( axis.IsLessThan(GetTextConst(j)) == true )
				{
					j--;
					continue;
				}
			}
			else
			{
				if( GetTextConst(j).IsLessThan(axis) == true )
				{
					j--;
					continue;
				}
			}
			break;
		}
		if( i >= j )   break;
		SwapObject(i,j);
		i++;
		j--;
	}
	if( first < i-1 )   QuickSort(first,i-1,inAscendant);
	if( j+1 < last  )   QuickSort(j+1,last,inAscendant);
}

//#717
/**
ソート
*/
/*後回し
void	ATextArray::SortUsingOrderArray( const ABool inAscendant, AArray<AIndex>& outOrderArray )
{
	if( GetItemCount() < 2 )   return;
	//
	outOrderArray.DeleteAll();
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		outOrderArray.Add(i);
	}
	//
	QuickSortUsingOrderArray(0,GetItemCount()-1,inAscendant,outOrderArray);
}

void	ATextArray::QuickSortUsingOrderArray( const AIndex first, const AIndex last, const ABool inAscendant, AArray<AIndex>& ioOrderArray )
{
	AText	axis;
	Get((first+last)/2,axis);
	AIndex	i = first, j = last;
	for( ; ; )
	{
		for( ; ; )
		{
			if( inAscendant == true )
			{
				if( GetTextConst(i).IsLessThan(axis) == true )
				{
					i++;
					continue;
				}
			}
			else
			{
				if( axis.IsLessThan(GetTextConst(i)) == true )
				{
					i++;
					continue;
				}
			}
			break;
		}
		for( ; ; )
		{
			if( inAscendant == true )
			{
				if( axis.IsLessThan(GetTextConst(j)) == true )
				{
					j--;
					continue;
				}
			}
			else
			{
				if( GetTextConst(j).IsLessThan(axis) == true )
				{
					j--;
					continue;
				}
			}
			break;
		}
		if( i >= j )   break;
		SwapObject(i,j);
		i++;
		j--;
	}
	if( first < i-1 )   QuickSortUsingOrderArray(first,i-1,inAscendant);
	if( j+1 < last  )   QuickSortUsingOrderArray(j+1,last,inAscendant);
}
*/

#pragma mark ===========================
#pragma mark [クラス]AHashTextArray
#pragma mark ===========================
//#348

//#693 ハッシュ用定義 -1はkUniqID_Invalid, -2はkUniqID_Unusedで使用しているので、-3, -4を使用する。
#define	kUniqIDValHash_Deleted (-3)
#define	kUniqIDValHash_NoData (-4)

/**
コンストラクタ
*/
AHashTextArray::AHashTextArray( AObjectArrayItem* inParent ) : mHash_DeletedCount(0)//#693 : mNextObjectIDCandidate(kObjectID_Min)
{
	//#693 メモリ節約のため必要になるまでHash生成しない MakeHash();
}

/**
デストラクタ
*/
AHashTextArray::~AHashTextArray()
{
}

#pragma mark ===========================

#pragma mark ---追加・削除

/**
追加
*/
void	AHashTextArray::Insert1( const AIndex inIndex, const AText& inText )
{
	//基底クラスのメソッドをコール
	ATextArray::Insert1(inIndex,inText);
	
	//UniqID生成・inIndexの要素に結びつけ #695
	mUniqIDArray.Insert1Item(inIndex);
	
	//ハッシュ更新
	UpdateHash_DataAdded(GetUniqIDByIndex(inIndex),inText);
	
}

//#695
/**
追加
*/
AIndex	AHashTextArray::Add( const AText& inText )
{
	AIndex	index = GetItemCount();
	Insert1(index,inText);
	return index;
}

/**
削除
*/
void	AHashTextArray::Delete1( const AIndex inIndex )
{
	//ハッシュ更新（削除） #695
	AText	deletingText;
	Get(inIndex,deletingText);
	AUniqID	uniqID = GetUniqIDByIndex(inIndex);
	UpdateHash_DataDeleting(uniqID,deletingText);
	
	//基底クラスのメソッドをコール
	ATextArray::Delete1(inIndex);
	
	//UniqID削除
	mUniqIDArray.Delete1Item(inIndex);
}

//#695
/**
全削除
*/
void	AHashTextArray::DeleteAll()
{
	ATextArray::DeleteAll();
	mUniqIDArray.DeleteAll();
	mHash.DeleteAll();
	mHash_DeletedCount = 0;
}

//#65
/**
指定textに一致する項目を全て削除
*/
void	AHashTextArray::DeleteItemsOfText( const AText& inText )
{
	for( AIndex index = 0; index < GetItemCount(); )
	{
		AText	text;
		Get(index,text);
		if( text.Compare(inText) == true )
		{
			Delete1(index);
		}
		else
		{
			index++;
		}
	}
}

#pragma mark ===========================

#pragma mark ---設定・取得

/**
テキスト設定
*/
void	AHashTextArray::Set( const AIndex inIndex, const AText& inText )
{
	//ハッシュ更新（削除） #695
	AText	deletingText;
	Get(inIndex,deletingText);
	AUniqID	uniqID = GetUniqIDByIndex(inIndex);
	UpdateHash_DataDeleting(uniqID,deletingText);
	
	//基底クラスのメソッドをコール
	ATextArray::Set(inIndex,inText);
	
	//ハッシュ更新
	RegisterToHash(GetUniqIDByIndex(inIndex),inText,0,inText.GetItemCount());
}

/**
移動
*/
void	AHashTextArray::MoveObject( const AIndex inOldIndex, const AIndex inNewIndex )
{
	AText	text;
	Get(inOldIndex,text);
	Delete1(inOldIndex);
	Insert1(inNewIndex,text);
}

/**
Swap
*/
void	AHashTextArray::SwapObject( const AIndex inA, const AIndex inB )
{
	AText	textA, textB;
	Get(inA,textA);
	Get(inB,textB);
	Set(inA,textB);
	Set(inB,textA);
}

//#853
/**
AHashTextArrayから追加
*/
void	AHashTextArray::AddFromTextArray( const ATextArray& inTextArray )
{
	AItemCount	count = inTextArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AText	text;
		inTextArray.Get(i,text);
		Add(text);
	}
}

//#853
/**
AHashTextArrayから設定
*/
void	AHashTextArray::SetFromTextArray( const ATextArray& inTextArray )
{
	DeleteAll();
	AddFromTextArray(inTextArray);
}

//#717
/**
AHashTextArrayから追加
*/
void	AHashTextArray::AddFromTextArray( const AHashTextArray& inTextArray )
{
	AItemCount	count = inTextArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AText	text;
		inTextArray.Get(i,text);
		Add(text);
	}
}

//#717
/**
ATextArrayから設定
*/
void	AHashTextArray::SetFromTextArray( const AHashTextArray& inTextArray )
{
	DeleteAll();
	AddFromTextArray(inTextArray);
}

#pragma mark ===========================

#pragma mark ---UniqID
//#693

/**
IndexからUniqID取得
*/
AUniqID	AHashTextArray::GetUniqIDByIndex( const AIndex inIndex ) const
{
	return mUniqIDArray.Get(inIndex);
}

/**
UniqIDからIndex取得
*/
AIndex	AHashTextArray::GetIndexByUniqID( const AUniqID inUniqID ) const
{
	return mUniqIDArray.Find(inUniqID);
}

#pragma mark ===========================

#pragma mark ---検索

/**
検索（UniqID取得・最初に一致したもの）
*/
AUniqID	AHashTextArray::FindUniqID( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	//#693 要素数0のときはhash検索しない（hash未生成の場合があるので0除算対策）
	if( GetItemCount() == 0 )
	{
		return kUniqID_Invalid;//#695 kIndex_Invalid;
	}
	
	//ハッシュサイズ取得
	AItemCount	hashsize = mHash.GetItemCount();
	
	//ハッシュ計算
	unsigned long	hash = GetHashValue(inTargetText,inTargetIndex,inTargetItemCount);
	AIndex	hashstartpos = hash%hashsize;
	
	//一発発見できるならポインタ使用しない
	{
		AUniqID	uniqID = mHash.Get(hashstartpos);
		if( uniqID.val == kUniqIDValHash_NoData )
		{
			return kUniqID_Invalid;//#695 kIndex_Invalid;
		}
		else if( uniqID.val != kUniqIDValHash_Deleted )
		{
			if( Compare(GetIndexByUniqID(uniqID),inTargetText,inTargetIndex,inTargetItemCount) == true )
			{
				return uniqID;
			}
		}
	}
	//高速化のため、ポインタ使用
	AStArrayPtr<AUniqID>	arrayptr(mHash,0,hashsize);
	AUniqID*	p = arrayptr.GetPtr();
	//ハッシュ検索
	AIndex	hashpos = hashstartpos;
	hashpos = ((hashpos==0)?hashsize-1:hashpos-1);//#271 最初の位置は上記で検索済みなので次の位置から始める
	while( true )
	{
		AUniqID	uniqID = p[hashpos];
		if( uniqID.val == kUniqIDValHash_NoData )   break;
		if( uniqID.val != kUniqIDValHash_Deleted )
		{
			if( Compare(GetIndexByUniqID(uniqID),inTargetText,inTargetIndex,inTargetItemCount) == true )
			{
				return uniqID;//#695
			}
		}
		//次のハッシュ位置へ
		hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
		if( hashpos == hashstartpos )   break;
	}
	return kUniqID_Invalid;//#695 kIndex_Invalid;
}

/**
検索（Index取得・一致するもの全て）
*/
void	AHashTextArray::FindUniqIDs( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount,
		AArray<AUniqID> &outUniqIDArray ) const
{
	outUniqIDArray.DeleteAll();
	
	//#693 要素数0のときはhash検索しない（hash未生成の場合があるので0除算対策）
	if( GetItemCount() == 0 )
	{
		return;
	}
	
	//ハッシュサイズ取得
	AItemCount	hashsize = mHash.GetItemCount();
	
	//ハッシュ計算
	unsigned long	hash = GetHashValue(inTargetText,inTargetIndex,inTargetItemCount);
	AIndex	hashstartpos = hash%hashsize;
	
	//高速化のため、ポインタ使用
	AStArrayPtr<AUniqID>	arrayptr(mHash,0,hashsize);
	AUniqID*	p = arrayptr.GetPtr();
	//ハッシュ検索
	AIndex	hashpos = hashstartpos;
	while( true )
	{
		AUniqID	uniqID = p[hashpos];
		if( uniqID.val == kUniqIDValHash_NoData )   break;
		if( uniqID.val != kUniqIDValHash_Deleted )
		{
			if( Compare(GetIndexByUniqID(uniqID),inTargetText,inTargetIndex,inTargetItemCount) == true )
			{
				outUniqIDArray.Add(uniqID);
			}
		}
		//次のハッシュ位置へ
		hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
		if( hashpos == hashstartpos )   break;
	}
}

/**
検索（Index取得・最初に一致したもの）
*/
AIndex	AHashTextArray::Find( const AText& inTargetText, const AIndex inTargetIndex, 
		const AItemCount inTargetItemCount ) const
{
	AUniqID	uniqID = FindUniqID(inTargetText,inTargetIndex,inTargetItemCount);
	if( uniqID == kUniqID_Invalid )   return kIndex_Invalid;
	return GetIndexByUniqID(uniqID);
}

/**
検索（Index取得・一致するもの全て）
*/
void	AHashTextArray::FindAll( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount, 
		AArray<AIndex>& outFoundIndex ) const
{
	//
	outFoundIndex.DeleteAll();
	//
	AArray<AUniqID>	uniqIDArray;
	FindUniqIDs(inTargetText,inTargetIndex,inTargetItemCount,uniqIDArray);
	for( AIndex i = 0; i < uniqIDArray.GetItemCount(); i++ )
	{
		outFoundIndex.Add(GetIndexByUniqID(uniqIDArray.Get(i)));
	}
}

/**
ハッシュテーブルを初期化し、対象Arrayの全データを登録する。
*/
void	AHashTextArray::MakeHash()
{
	//対象Arrayのサイズが大きすぎたらreturn
	if( GetItemCount() >= kItemCount_HashMaxSize/2 )
	{
		_ACError("cannot make hash because array is too big",this);
		return;
	}
	//ハッシュテーブル全削除
	mHash.DeleteAll();
	AItemCount	hashsize = GetItemCount()*3+128;
	//ハッシュサイズが最大ハッシュサイズよりも大きければ、補正する。
	if( hashsize > kItemCount_HashMaxSize )   hashsize = kItemCount_HashMaxSize;
	//ハッシュテーブル確保
	mHash.Reserve(0,hashsize);
	//ハッシュテーブル初期化
	{
		AUniqID	nodataUniqID = {kUniqIDValHash_NoData};//#695
		AStArrayPtr<AUniqID>	arrayptr(mHash,0,hashsize);//#695
		AUniqID*	p = arrayptr.GetPtr();//#695
		for( AIndex i = 0; i < hashsize; i++ )
		{
			//#695 mHash.Set(i,kIndex_HashNoData);
			p[i] = nodataUniqID;//#695
		}
	}
	//対象Arrayの現在のデータを全て登録
	AItemCount	datacount = GetItemCount();
	for( AIndex i = 0; i < datacount; i++ )
	{
		AIndex	spos, epos;
		GetRange(i,spos,epos);
		RegisterToHash(GetUniqIDByIndex(i),mTextData,spos,epos-spos);//#695
	}
	//"Deleted"個数初期化   #303 "Deleted"の個数をRehash判断に使用する
	mHash_DeletedCount = 0;
}

/**
ハッシュ更新（追加）
*/
void	AHashTextArray::UpdateHash_DataAdded( const AUniqID inUniqID, const AText& inText )//#695
{
	//ハッシュサイズが対象データ配列の現在のサイズの2倍よりも小さければ、ハッシュテーブルを再構成する。
	if( mHash.GetItemCount() < (GetItemCount()+mHash_DeletedCount)*2+32 )//#303 "Deleted"の個数をRehash判断に使用する #693 hash未生成の場合に生成するように"+32"を追加
	{
		MakeHash();
	}
	//そうでない場合は対象アイテムだけ追加登録する。
	else
	{
		//ハッシュに登録
		RegisterToHash(inUniqID,inText,0,inText.GetItemCount());//#695
	}
}

/**
ハッシュ更新（削除）
*/
void	AHashTextArray::UpdateHash_DataDeleting( const AUniqID inUniqID, const AText& inDeletingText )
{
	//ハッシュサイズ取得
	AItemCount	hashsize = mHash.GetItemCount();
	
	//ハッシュ値計算
	unsigned long	hash = GetHashValue(inDeletingText,0,inDeletingText.GetItemCount());
	AIndex	hashstartpos = hash%hashsize;
	
	{//#598 arrayptrの有効範囲を最小にする
		//高速化のため、ポインタ使用
		AStArrayPtr<AUniqID>	arrayptr(mHash,0,hashsize);
		AUniqID*	p = arrayptr.GetPtr();
		AIndex	hashpos = hashstartpos;
		while( true )
		{
			AUniqID	dataUniqID = p[hashpos];
			if( dataUniqID.val == kUniqIDValHash_NoData )   break;//#216
			if( dataUniqID.val != kUniqIDValHash_Deleted )//#216
			{
				if( dataUniqID.val == inUniqID.val )//#216
				{
					//削除実行
					if( p[((hashpos==0)?hashsize-1:hashpos-1)].val == kUniqIDValHash_NoData )
					{
						p[hashpos].val = kUniqIDValHash_NoData;//#216
						//win 080713 NoDataを書き込んだとき、前のハッシュ位置（物理的には次）がDeletedならNoDataに書き換え
						//[hashpos-1]:NoData
						//[hashpos  ]:NoData←今回書き込み
						//[hashpos+1]:Deleted←このDeletedはNoDataにする（hashposの位置のデータへのリンクをつなげるために存在していたDeletedなので。）
						//[hashpos+2]:Deleted←さらに次もDeletedならここもNoData
						AIndex	hp = hashpos+1;
						if( hp >= hashsize )   hp = 0;
						while( p[hp].val == kUniqIDValHash_Deleted )//#216
						{
							//DeletedならNoDataに書き換えて継続
							p[hp].val = kUniqIDValHash_NoData;//#216
							hp++;
							if( hp >= hashsize )   hp = 0;
							//"Deleted"を"NoData"にしたので"Deleted"個数をデクリメント   #303 "Deleted"の個数をRehash判断に使用する
							mHash_DeletedCount--;
						}
					}
					else
					{
						p[hashpos].val = kUniqIDValHash_Deleted;//#216
						//"Deleted"にしたので"Deleted"個数をインクリメント   #303 "Deleted"の個数をRehash判断に使用する
						mHash_DeletedCount++;
					}
					return;
				}
			}
			//次のハッシュ位置へ
			hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
			if( hashpos == hashstartpos )   break;
		}
	}
}

/**
ハッシュ登録

ハッシュテーブルに、対象Arrayの指定アイテムを登録する。
データIndex番号の移動なし
*/
void	AHashTextArray::RegisterToHash( const AUniqID inUniqID, 
		const AText& inText, const AIndex inTextIndex, const AItemCount inTextItemCount )
{
	//ハッシュサイズ取得
	AItemCount	hashsize = mHash.GetItemCount();
	
	//ハッシュ値計算
	unsigned long	hash = GetHashValue(inText,inTextIndex,inTextItemCount);
	AIndex	hashstartpos = hash%hashsize;
	
	{//#598 arrayptrの有効範囲を最小にする
		//高速化のため、ポインタ使用
		//#695 AStArrayPtr<AUniqID>	arrayptr(mHash,0,hashsize);
		//#695 AUniqID*	p = arrayptr.GetPtr();
		//高速化のため、直にAArray::mDataPtrを取得する。
		//AStArrayPtr<>は、あくまでコーディングミス防止なので、変更等が頻繁には発生しないAbsBase内コードでは、
		//必ずしも使う必要が無い。
		//AHashArrayへの10M項目追加で、9s→2sの効果。
		//rehash時のMakeHashにも使われているので、ここの高速化は重要。
		AUniqID*	p = mHash.GetDataPtrWithoutCheck();
		//登録箇所検索
		AIndex	hashpos = hashstartpos;
		while( true )
		{
			AUniqID	dataUniqID = p[hashpos];
			if( dataUniqID.val == kUniqIDValHash_NoData )   break;
			if( dataUniqID.val == kUniqIDValHash_Deleted )
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
		p[hashpos] = inUniqID;
	}
}

/**
ハッシュ値計算
*/
unsigned long	AHashTextArray::GetHashValue( const AText& inData, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	AItemCount	targetItemCount = inTargetItemCount;
	if( targetItemCount == kIndex_Invalid )
	{
		targetItemCount = inData.GetItemCount();
	}
	if( targetItemCount == 0 )
	{
		return 1234567;
	}
	else if( targetItemCount == 1 )
	{
		return inData.Get(inTargetIndex)*67890123;
	}
	else if( targetItemCount == 2 )
	{
		return inData.Get(inTargetIndex)*78901234 + inData.Get(inTargetIndex+1)*891012345;
	}
	else 
	{
		return inData.Get(inTargetIndex)*8901234 + inData.Get(inTargetIndex+1)*9012345 
			+ inData.Get(inTargetIndex+targetItemCount-2)*1234567 + inData.Get(inTargetIndex+targetItemCount-1) + targetItemCount*4567;
	}
}


