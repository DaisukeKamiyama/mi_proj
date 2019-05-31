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

ADataBase
設定データ等のデータを格納するための簡単なデータベース
動的なメンバーの生成が可能

*/

#include "stdafx.h"

#include "ADataBase.h"
#include "../Wrapper.h"
#include "AThreadMutex.h"


#pragma mark ===========================
#pragma mark [クラス]ADataBase
#pragma mark ===========================

//#693 mDBID廃止しObjectIDを使うようにしたので不要、排他制御の問題もあるため削除APtrManager<ADataBase>	gDataBasePtrManager;

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
ADataBase::ADataBase( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent), mEnableMutex(true)
{
	mCurrentRegisteringTableIndex = kIndex_Invalid;
	
	//#693 mDBID廃止しObjectIDを使うようにしたので不要、排他制御の問題もあるため削除mDBID = gDataBasePtrManager.Register(this);
}

/**
デストラクタ
*/
ADataBase::~ADataBase()
{
	//#693 mDBID廃止しObjectIDを使うようにしたので不要、排他制御の問題もあるため削除gDataBasePtrManager.Unregister(mDBID);
}

/**
ファイルへ保存（派生クラスへ定義）
*/
void	ADataBase::SaveToFile()
{
	//サブクラスで定義
}

#pragma mark ===========================

#pragma mark ---全体管理

/**
データ項目追加
*/
void	ADataBase::Register( ADataID inID, AConstCharPtr inFileKey, const ADataType& inDataType, const AIndex& inIndexInEach )
{
	//ID重複チェック
	AIndex	idcheck = mAll_ID.Find(inID);
	if( idcheck != kIndex_Invalid )
	{
		_ACError("same ID already exists",this);
		return;
	}
	
	//
	mAll_ID.Add(inID);
	mAll_DataType.Add(inDataType);
	mAll_IndexInEach.Add(inIndexInEach);
	mAll_IsStillDefault.Add(true);
	AText	text(inFileKey);
	mAll_FileKey.Add(text);//#344 Add_SwapContent(text);
}

/**
データ項目存在チェック
*/
ABool	ADataBase::IsDataExist( const ADataID inID ) const
{
	return (mAll_ID.Find(inID)!=kIndex_Invalid);
}

/**
*/
AIndex	ADataBase::GetIndexInEach( const ADataID inID, const ADataType inType ) const//#427 typeチェック追加
{
	AIndex	indexinall = mAll_ID.Find(inID);
	if( indexinall == kIndex_Invalid )
	{
		_ACError("cannot find pref data",this);
		return kIndex_Invalid;
	}
	//型チェック #427 GetData_xxxで型を間違ったときに検出できるようにするため。
	if( mAll_DataType.Get(indexinall) != inType )
	{
		_ACError("data type error",this);
		return kIndex_Invalid;
	}
	//
	return mAll_IndexInEach.Get(indexinall);
}

/**
データが設定されたらコールされる
*/
void	ADataBase::DataIsSet( ADataID inID )
{
	AIndex	index = mAll_ID.Find(inID);
	mAll_IsStillDefault.Set(index,false);
}

/**
データがデフォルト値に設定されたらコールされる
*/
void	ADataBase::DataIsSetToDefault( ADataID inID )
{
	AIndex	index = mAll_ID.Find(inID);
	mAll_IsStillDefault.Set(index,true);
}

/**
データがデフォルトのままかどうかをチェックする。単一データのみ使用可能
*/
ABool	ADataBase::IsStillDefault( ADataID inID ) const 
{
	//
	AIndex	index = mAll_ID.Find(inID);
	return mAll_IsStillDefault.Get(index);
}

/**
他のADataBaseの指定IDと同じ型のデータ項目を生成（内容はコピーしない）
*/
void	ADataBase::CreateDataFromOtherDB( ADataID inID, const ADataBase& inOtherDB, ADataID inIDInOtherDB )
{
	ADataType	dataType = inOtherDB.GetDataType(inIDInOtherDB);
	AIndex	indexInOtherDB = inOtherDB.GetIndexInEach(inIDInOtherDB,dataType);
	if( indexInOtherDB == kIndex_Invalid )   return;
	switch(dataType)
	{
	  case kDataType_Bool:
		{
			CreateData_Bool(inID,"",true);
			break;
		}
	  case kDataType_Number:
		{
			CreateData_Number(inID,"",0,inOtherDB.mNumber_Min.Get(indexInOtherDB),inOtherDB.mNumber_Max.Get(indexInOtherDB));
			break;
		}
	  case kDataType_FloatNumber:
		{
			CreateData_FloatNumber(inID,"",0,inOtherDB.mFloatNumber_Min.Get(indexInOtherDB),inOtherDB.mFloatNumber_Max.Get(indexInOtherDB));
			break;
		}
	  case kDataType_Point:
		{
			CreateData_Point(inID,"",0,0,inOtherDB.mPoint_XMin.Get(indexInOtherDB),inOtherDB.mPoint_XMax.Get(indexInOtherDB),
					inOtherDB.mPoint_YMin.Get(indexInOtherDB),inOtherDB.mPoint_YMax.Get(indexInOtherDB));
			break;
		}
	  case kDataType_Rect:
		{
			CreateData_Rect(inID,"",0,0,0,0,inOtherDB.mRect_LeftMin.Get(indexInOtherDB),inOtherDB.mRect_TopMin.Get(indexInOtherDB),
					inOtherDB.mRect_RightMax.Get(indexInOtherDB),inOtherDB.mRect_BottomMax.Get(indexInOtherDB));
			break;
		}
	  case kDataType_Color:
		{
			CreateData_Color(inID,"",kColor_Black);
			break;
		}
	  /*win case kDataType_Font:
		{
			CreateData_Font(inID,"",AFontWrapper::GetDefaultFont());
			break;
		}*/
	  case kDataType_Text:
		{
			CreateData_Text(inID,"",GetEmptyText(),inOtherDB.mText_LimitLength.Get(indexInOtherDB),
					inOtherDB.mText_MinLength.Get(indexInOtherDB),inOtherDB.mText_MaxLength.Get(indexInOtherDB));
			break;
		}
	  case kDataType_BoolArray:
		{
			CreateData_BoolArray(inID,"",true);
			break;
		}
	  case kDataType_NumberArray:
		{
			CreateData_NumberArray(inID,"",0,inOtherDB.mNumberArray_Min.Get(indexInOtherDB),inOtherDB.mNumberArray_Max.Get(indexInOtherDB));//#695 ,false);
			break;
		}
	  case kDataType_ColorArray:
		{
			CreateData_ColorArray(inID,"",kColor_Black);
			break;
		}
	  case kDataType_TextArray:
		{
			CreateData_TextArray(inID,"","",inOtherDB.mTextArray_LimitLength.Get(indexInOtherDB),
					inOtherDB.mTextArray_MinLength.Get(indexInOtherDB),inOtherDB.mTextArray_MaxLength.Get(indexInOtherDB));
			break;
		}
	  default:
		{
			_ACError("",this);
			break;
		}
	}
}

/**
他のADataBaseのデータ項目と内容をコピーする
*/
void	ADataBase::CopyDataFromOtherDB( const ADataBase& inOtherDB )
{
	for( AIndex index = 0; index < inOtherDB.mAll_ID.GetItemCount(); index++ )
	{
		CopyDataFromOtherDB(inOtherDB.mAll_ID.Get(index),inOtherDB,inOtherDB.mAll_ID.Get(index));
	}
}

/**
他のADataBaseの指定IDと同じ型のデータ項目を生成し内容をコピーする
*/
void	ADataBase::CopyDataFromOtherDB( ADataID inID, const ADataBase& inOtherDB, ADataID inIDInOtherDB )
{
	//データが未存在なら生成（制約条件等をコピーする）
	if( mAll_ID.Find(inID) == kIndex_Invalid )
	{
		CreateDataFromOtherDB(inID,inOtherDB,inIDInOtherDB);
	}
	//
	ADataType	dataType = GetDataType(inID);
	if( dataType != inOtherDB.GetDataType(inIDInOtherDB) )
	{
		_ACError("cannot copy because data type is diffrent",this);
		return;
	}
	switch(dataType)
	{
	  case kDataType_Bool:
		{
			SetData_Bool(inID,inOtherDB.GetData_Bool(inIDInOtherDB));
			break;
		}
	  case kDataType_Number:
		{
			SetData_Number(inID,inOtherDB.GetData_Number(inIDInOtherDB));
			break;
		}
	  case kDataType_FloatNumber:
		{
			SetData_FloatNumber(inID,inOtherDB.GetData_FloatNumber(inIDInOtherDB));
			break;
		}
	  case kDataType_Point:
		{
			APoint	data;
			inOtherDB.GetData_Point(inIDInOtherDB,data);
			SetData_Point(inID,data);
			break;
		}
	  case kDataType_Rect:
		{
			ARect	data;
			inOtherDB.GetData_Rect(inIDInOtherDB,data);
			SetData_Rect(inID,data);
			break;
		}
	  case kDataType_Color:
		{
			AColor	data;
			inOtherDB.GetData_Color(inIDInOtherDB,data);
			SetData_Color(inID,data);
			break;
		}
	  /*win case kDataType_Font:
		{
			SetData_Font(inID,inOtherDB.GetData_Font(inIDInOtherDB));
			break;
		}*/
	  case kDataType_Text:
		{
			AText	data;
			inOtherDB.GetData_Text(inIDInOtherDB,data);
			SetData_Text(inID,data);
			break;
		}
	  case kDataType_BoolArray:
		{
			SetData_BoolArray(inID,inOtherDB.GetData_ConstBoolArrayRef(inIDInOtherDB));
			break;
		}
	  case kDataType_NumberArray:
		{
			SetData_NumberArray(inID,inOtherDB.GetData_ConstNumberArrayRef(inIDInOtherDB));
			break;
		}
	  case kDataType_ColorArray:
		{
			SetData_ColorArray(inID,inOtherDB.GetData_ConstColorArrayRef(inIDInOtherDB));
			break;
		}
	  case kDataType_TextArray:
		{
			SetData_TextArray(inID,inOtherDB.GetData_ConstTextArrayRef(inIDInOtherDB));
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//#375 StillDefaultデータのコピー
	AIndex	otherDBAllIndex = inOtherDB.mAll_ID.Find(inID);
	if( otherDBAllIndex != kIndex_Invalid )
	{
		ABool	stillDefault = inOtherDB.mAll_IsStillDefault.Get(otherDBAllIndex);
		AIndex	allIndex = mAll_ID.Find(inID);
		if( allIndex != kIndex_Invalid )
		{
			mAll_IsStillDefault.Set(allIndex,stillDefault);
		}
	}
}

/**
データ項目の型を取得
*/
ADataType	ADataBase::GetDataType( ADataID inID ) const
{
	AIndex	indexinall = mAll_ID.Find(inID);
	if( indexinall == kIndex_Invalid )
	{
		_ACError("cannot find pref data",this);
		return kDataType_NotFound;
	}
	return mAll_DataType.Get(indexinall);
}

/**
全てのデータをデフォルトに設定
*/
void	ADataBase::SetAllDataToDefault()
{
	for( AIndex indexinall = 0; indexinall < mAll_ID.GetItemCount(); indexinall++ )
	{
		ADataID	dataID = mAll_ID.Get(indexinall);
		switch(GetDataType(dataID))
		{
		  case kDataType_Bool:
			{
				SetDefault_Bool(dataID);
				break;
			}
		  case kDataType_Number:
			{
				SetDefault_Number(dataID);
				break;
			}
		  case kDataType_FloatNumber:
			{
				SetDefault_FloatNumber(dataID);
				break;
			}
		  case kDataType_Point:
			{
				SetDefault_Point(dataID);
				break;
			}
		  case kDataType_Rect:
			{
				SetDefault_Rect(dataID);
				break;
			}
		  case kDataType_Text:
			{
				SetDefault_Text(dataID);
				break;
			}
		  case kDataType_Color:
			{
				SetDefault_Color(dataID);
				break;
			}
		  case kDataType_TextArray:
			{
				DeleteAll_TextArray(dataID);
				break;
			}
		  case kDataType_BoolArray:
			{
				DeleteAll_BoolArray(dataID);
				break;
			}
		  case kDataType_NumberArray:
			{
				DeleteAll_NumberArray(dataID);
				break;
			}
		  case kDataType_ColorArray:
			{
				DeleteAll_ColorArray(dataID);
				break;
			}
		  default:
			{
				_ACError("",this);
				break;
			}
		}
	}
}

#pragma mark ===========================

#pragma mark ---ファイル

#if IMPLEMENTATION_FOR_MACOSX
/**
PrefFileからデータ読み込み

File上にデータが存在するもののみ読み込まれる
File上にデータが存在しない場合、各mXXXArray_Dataの値はコール前の値（初期値やMacLegacyデータ）のままとなる。

LoadFromPrefFile()はWindows版と互換性無し。
*/
ABool	ADataBase::LoadFromPrefFile( const AFileAcc& inFileAcc, const ResType& inResType )//#1442
{
	if( mPrefFileImp.Load(inFileAcc,inResType) == false )
	{
		//B0000 ここに来る前に旧形式データを読み込み、かつ、新形式データが存在しなかった場合、この関数の最後のCorrectTable()が実行されない
		//テーブルの数の補正
		CorrectTable();
		return false;//#1442
	}
	
	for( AIndex indexinall = 0; indexinall < mAll_ID.GetItemCount(); indexinall++ )
	{
		AText	fileKey;
		mAll_FileKey.Get(indexinall,fileKey);
		if( fileKey.GetLength() == 0 )   continue;
		ADataID	dataID = mAll_ID.Get(indexinall);
		switch(GetDataType(dataID))
		{
		  case kDataType_Bool:
			{
				ABool	data;
				if( mPrefFileImp.GetData(fileKey,data) == true )
				{
					SetData_Bool(dataID,data);
				}
				break;
			}
		  case kDataType_Number:
			{
				ANumber	data;
				if( mPrefFileImp.GetData(fileKey,data) == true )
				{
					SetData_Number(dataID,data);
				}
				break;
			}
		  case kDataType_FloatNumber:
			{
				AFloatNumber	data;
				if( mPrefFileImp.GetData(fileKey,data) == true )
				{
					SetData_FloatNumber(dataID,data);
				}
				break;
			}
		  /*win case kDataType_Font:
			{
				AFont	data;
				if( mPrefFileImp.GetData(fileKey,data) == true )
				{
					SetData_Font(dataID,data);
				}
				break;
			}*/
		  case kDataType_Point:
			{
				APoint	data;
				if( mPrefFileImp.GetData(fileKey,data) == true )
				{
					SetData_Point(dataID,data);
				}
				break;
			}
		  case kDataType_Rect:
			{
				ARect	data;
				if( mPrefFileImp.GetData(fileKey,data) == true )
				{
					SetData_Rect(dataID,data);
				}
				break;
			}
		  case kDataType_Text:
			{
				AText	data;
				if( mPrefFileImp.GetData(fileKey,data) == true )
				{
					SetData_Text(dataID,data);
				}
				break;
			}
		  case kDataType_Color:
			{
				AColor	data;
				if( mPrefFileImp.GetData(fileKey,data) == true )
				{
					SetData_Color(dataID,data);
				}
				break;
			}
		  case kDataType_TextArray:
			{
				ATextArray	textArray;
				if( mPrefFileImp.GetData(fileKey,textArray) == true )
				{
					SetData_TextArray(dataID,textArray);//この関数内でデータ制限が行われる
				}
				break;
			}
		  case kDataType_BoolArray:
			{
				ABoolArray	boolArray;
				if( mPrefFileImp.GetData(fileKey,boolArray) == true )
				{
					SetData_BoolArray(dataID,boolArray);//この関数内でデータ制限が行われる
				}
				break;
			}
		  case kDataType_NumberArray:
			{
				ANumberArray	numberArray;
				if( mPrefFileImp.GetData(fileKey,numberArray) == true )
				{
					SetData_NumberArray(dataID,numberArray);//この関数内でデータ制限が行われる
				}
				break;
			}
		  case kDataType_ColorArray:
			{
				AColorArray	colorArray;
				if( mPrefFileImp.GetData(fileKey,colorArray) == true )
				{
					SetData_ColorArray(dataID,colorArray);//この関数内でデータ制限が行われる
				}
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
	
	//テーブルの数の補正
	CorrectTable();
	return true;//#1442
}

/**
PrefFileへデータ書き込み

WriteToPrefFile()はWindows版と互換性無し。
*/
void	ADataBase::WriteToPrefFile( const AFileAcc& inFileAcc/*#1034, const ResType& inResType*/ )
{
	for( AIndex indexinall = 0; indexinall < mAll_ID.GetItemCount(); indexinall++ )
	{
		AText	fileKey;
		mAll_FileKey.Get(indexinall,fileKey);
		if( fileKey.GetLength() == 0 )   continue;
		ADataID	dataID = mAll_ID.Get(indexinall);
		switch(GetDataType(dataID))
		{
		  case kDataType_Bool:
			{
				ABool	data = GetData_Bool(dataID);
				mPrefFileImp.SetData(fileKey,data);
				break;
			}
		  case kDataType_Number:
			{
				ANumber	data = GetData_Number(dataID);
				mPrefFileImp.SetData(fileKey,data);
				break;
			}
		  case kDataType_FloatNumber:
			{
				AFloatNumber	data = GetData_FloatNumber(dataID);
				mPrefFileImp.SetData(fileKey,data);
				break;
			}
		  /*win case kDataType_Font:
			{
				AFont	data = GetData_Font(dataID);
				mPrefFileImp.SetData(fileKey,data);
				break;
			}*/
		  case kDataType_Point:
			{
				APoint	data;
				GetData_Point(dataID,data);
				mPrefFileImp.SetData(fileKey,data);
				break;
			}
		  case kDataType_Rect:
			{
				ARect	data;
				GetData_Rect(dataID,data);
				mPrefFileImp.SetData(fileKey,data);
				break;
			}
		  case kDataType_Text:
			{
				AText	data;
				GetData_Text(dataID,data);
				mPrefFileImp.SetData(fileKey,data);
				break;
			}
		  case kDataType_Color:
			{
				AColor	data;
				GetData_Color(dataID,data);
				mPrefFileImp.SetData(fileKey,data);
				break;
			}
		  case kDataType_TextArray:
			{
				mPrefFileImp.SetData(fileKey,GetData_ConstTextArrayRef(dataID));
				break;
			}
		  case kDataType_BoolArray:
			{
				mPrefFileImp.SetData(fileKey,GetData_ConstBoolArrayRef(dataID));
				break;
			}
		  case kDataType_NumberArray:
			{
				mPrefFileImp.SetData(fileKey,GetData_ConstNumberArrayRef(dataID));
				break;
			}
		  case kDataType_ColorArray:
			{
				mPrefFileImp.SetData(fileKey,GetData_ConstColorArrayRef(dataID));
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
	
	mPrefFileImp.Write(inFileAcc/*#1034,inResType*/);
}
#endif

/**
テキスト形式設定ファイルからデータ読み込み
*/
ABool	ADataBase::LoadFromPrefTextFile( const AFileAcc& inFileAcc )
{
	ABool	result = true;
	
	//#881
	//未使用設定項目のテキストをmUnusedPrefTextに保存していく
	mUnusedPrefText.DeleteAll();
	
	//
	AText	text;
	inFileAcc.ReadTo(text);
	text.ConvertLineEndToCR();
	try
	{
		for( AIndex pos = 0; pos < text.GetItemCount(); )
		{
			//#881
			//スペース文字を読み飛ばす
			if( text.SkipTabSpaceLineEnd(pos,text.GetItemCount()) == false )
			{
				break;
			}
			//#881
			//開始位置を記憶
			AIndex	spos = pos;
			//
			//#889 if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
			//開始文字が#の場合は行末までコメントとみなして読み飛ばす。#889
			{
				//開始文字取得
				AText	token;
				if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
				//開始文字が#なら行末まで読み飛ばし
				if( token.Compare("#") == true )
				{
					for( ; pos < text.GetItemCount(); pos++ )
					{
						if( text.Get(pos) == kUChar_LineEnd )
						{
							break;
						}
					}
					//最初のfor文まで戻って継続
					continue;
				}
				//開始文字が#以外の場合、"以外ならエラー
				if( token.Compare("\"") == false )   throw 0;
			}
			//==================キー読み込み==================
			AText	filekey;
			text.ExtractUnescapedTextFromPrefText(pos,filekey,'\"');
			if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
			AIndex	indexinall = mAll_FileKey.Find(filekey);
			if( indexinall == kIndex_Invalid )
			{
				//====================対応するデータが見つからない場合（旧バージョンで起動した場合等）====================
				
				//#375 新規データ追加時、旧バージョンでエラー発生させないようにする _ACError("",NULL);
				//=
				if( LoadFromPrefTextFile_CheckToken(text,pos,'=') == false )   throw 0;
				//"または{
				AText	token;
				if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
				if( token.Compare("\"") == true )
				{
					//非配列データ
					//kDataType_Textの場合からデータ設定を除いた処理を行う
					AText	t;
					text.ExtractUnescapedTextFromPrefText(pos,t,'\"');
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
				}
				else if( token.Compare("{") == true )
				{
					//配列データ
					//kDataType_TextArrayの場合からデータ設定を除いた処理を行う
					while( pos < text.GetItemCount() )
					{
						AText	token;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						if( token.Get(0) == '}' )   break;
						else if( token.Get(0) != '\"' )   throw 0;
						AText	t;
						text.ExtractUnescapedTextFromPrefText(pos,t,'\"');
						if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						if( token.Get(0) == '}' )   break;
						else if( token.Get(0) != ',' )   throw 0;
					}
				}
				else
				{
					throw 0;
				}
				//#881
				//見つからないデータ部分のテキストを取得
				AText	unusedPrefText;
				text.GetText(spos,pos-spos,unusedPrefText);
				unusedPrefText.Add(kUChar_LineEnd);
				//mUnusedPrefTextに記録（pref textセーブ時に、最後にそのまま書き足すことで、新バージョンで起動してもデータが消えないようにしている）
				mUnusedPrefText.AddText(unusedPrefText);
				continue;
			}
			//==================対応するデータが見つかった場合==================
			//=
			if( LoadFromPrefTextFile_CheckToken(text,pos,'=') == false )   throw 0;
			//
			ADataID	dataID = mAll_ID.Get(indexinall);
			switch(GetDataType(dataID))
			{
			  case kDataType_Bool:
				{
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					AText	token;
					if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
					ABool	data = false;
					if( token.Compare("true") == true )
					{
						data = true;
					}
					SetData_Bool(dataID,data);
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					break;
				}
			  case kDataType_Number:
				{
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					AText	token;
					if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
					SetData_Number(dataID,token.GetNumber());
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					break;
				}
			  case kDataType_FloatNumber:
				{
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					AText	t;
					text.ExtractUnescapedTextFromPrefText(pos,t,'\"');
					SetData_FloatNumber(dataID,t.GetFloatNumber());
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					break;
				}
			  case kDataType_Point:
				{
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					APoint	pt = {0,0};
					AText	token;
					if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
					pt.x = token.GetNumber();
					if( LoadFromPrefTextFile_CheckToken(text,pos,',') == false )   throw 0;
					if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
					pt.y = token.GetNumber();
					SetData_Point(dataID,pt);
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					break;
				}
			  case kDataType_Rect:
				{
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					ARect	rect = {0,0,0,0};
					AText	token;
					if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
					rect.left	= token.GetNumber();
					if( LoadFromPrefTextFile_CheckToken(text,pos,',') == false )   throw 0;
					if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
					rect.top	= token.GetNumber();
					if( LoadFromPrefTextFile_CheckToken(text,pos,',') == false )   throw 0;
					if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
					rect.right	= token.GetNumber();
					if( LoadFromPrefTextFile_CheckToken(text,pos,',') == false )   throw 0;
					if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
					rect.bottom	= token.GetNumber();
					SetData_Rect(dataID,rect);
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					break;
				}
			  case kDataType_Text:
				{
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					AText	t;
					text.ExtractUnescapedTextFromPrefText(pos,t,'\"');
					SetData_Text(dataID,t);
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					break;
				}
			  case kDataType_Color:
				{
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					AText	token;
					if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
					AColor	data = AColorWrapper::GetColorByHTMLFormatColor(token);
					SetData_Color(dataID,data);
					if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
					break;
				}
			  case kDataType_TextArray:
				{
					DeleteAll_TextArray(dataID);
					if( LoadFromPrefTextFile_CheckToken(text,pos,'{') == false )   throw 0;
					while( pos < text.GetItemCount() )
					{
						AText	token;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						if( token.Get(0) == '}' )   break;
						else if( token.Get(0) != '\"' )   throw 0;
						AText	t;
						text.ExtractUnescapedTextFromPrefText(pos,t,'\"');
						Add_TextArray(dataID,t);
						if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						if( token.Get(0) == '}' )   break;
						else if( token.Get(0) != ',' )   throw 0;
					}
					break;
				}
			  case kDataType_BoolArray:
				{
					DeleteAll_BoolArray(dataID);
					if( LoadFromPrefTextFile_CheckToken(text,pos,'{') == false )   throw 0;
					while( pos < text.GetItemCount() )
					{
						AText	token;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						if( token.Get(0) == '}' )   break;
						else if( token.Get(0) != '\"' )   throw 0;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						ABool	data = false;
						if( token.Compare("true") == true )
						{
							data = true;
						}
						Add_BoolArray(dataID,data);
						if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						if( token.Get(0) == '}' )   break;
						else if( token.Get(0) != ',' )   throw 0;
					}
					break;
				}
			  case kDataType_NumberArray:
				{
					DeleteAll_NumberArray(dataID);
					if( LoadFromPrefTextFile_CheckToken(text,pos,'{') == false )   throw 0;
					while( pos < text.GetItemCount() )
					{
						AText	token;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						if( token.Get(0) == '}' )   break;
						else if( token.Get(0) != '\"' )   throw 0;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						ANumber	data = token.GetNumber();
						Add_NumberArray(dataID,data);
						if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						if( token.Get(0) == '}' )   break;
						else if( token.Get(0) != ',' )   throw 0;
					}
					break;
				}
			  case kDataType_ColorArray:
				{
					DeleteAll_ColorArray(dataID);
					if( LoadFromPrefTextFile_CheckToken(text,pos,'{') == false )   throw 0;
					while( pos < text.GetItemCount() )
					{
						AText	token;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						if( token.Get(0) == '}' )   break;
						else if( token.Get(0) != '\"' )   throw 0;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						AColor	data = AColorWrapper::GetColorByHTMLFormatColor(token);
						Add_ColorArray(dataID,data);
						if( LoadFromPrefTextFile_CheckToken(text,pos,'\"') == false )   throw 0;
						if( LoadFromPrefTextFile_GetToken(text,pos,token) == false )   throw 0;
						if( token.Get(0) == '}' )   break;
						else if( token.Get(0) != ',' )   throw 0;
					}
					break;
				}
			  default:
				{
					_ACError("",NULL);
					break;
				}
			}
		}
	}
	catch(...)
	{
		result = false;
	}
	CorrectTable();
	return result;
}

/**
token取得（LoadFromPrefTextFile()用）
*/
ABool	ADataBase::LoadFromPrefTextFile_GetToken( const AText& inText, AIndex& ioPos, AText& outToken ) const
{
	if( inText.SkipTabSpaceLineEnd(ioPos,inText.GetItemCount()) == false )   return false;
	inText.GetToken(ioPos,outToken);
	return true;
}

/**
tokenがinCharかどうかを返す（LoadFromPrefTextFile()用）
*/
ABool	ADataBase::LoadFromPrefTextFile_CheckToken( const AText& inText, AIndex& ioPos, const AUChar inChar ) const
{
	AText	token;
	if( LoadFromPrefTextFile_GetToken(inText,ioPos,token) == false )   return false;
	if( token.Get(0) != inChar )
	{
		_ACError("",NULL);
		return false;
	}
	return true;
}

/**
テキスト形式設定ファイルにデータ書き込み
*/
void	ADataBase::WriteToPrefTextFile( const AFileAcc& inFileAcc ) const
{
	AText	text;
	for( AIndex indexinall = 0; indexinall < mAll_ID.GetItemCount(); indexinall++ )
	{
		AText	fileKey;
		mAll_FileKey.Get(indexinall,fileKey);
		if( fileKey.GetLength() == 0 )   continue;
		//
		text.Add('\"');
		text.AddText(fileKey);
		text.Add('\"');
		text.AddCstring(" = ");
		//
		ADataID	dataID = mAll_ID.Get(indexinall);
		switch(GetDataType(dataID))
		{
		  case kDataType_Bool:
			{
				if( GetData_Bool(dataID) == true )
				{
					text.AddCstring("\"true\"");
				}
				else
				{
					text.AddCstring("\"false\"");
				}
				break;
			}
		  case kDataType_Number:
			{
				text.AddFormattedCstring("\"%d\"",GetData_Number(dataID));
				break;
			}
		  case kDataType_FloatNumber:
			{
				text.AddFormattedCstring("\"%g\"",GetData_FloatNumber(dataID));
				break;
			}
		  case kDataType_Point:
			{
				APoint	data;
				GetData_Point(dataID,data);
				text.AddFormattedCstring("\"%d,%d\"",data.x,data.y);
				break;
			}
		  case kDataType_Rect:
			{
				ARect	data;
				GetData_Rect(dataID,data);
				text.AddFormattedCstring("\"%d,%d,%d,%d\"",data.left,data.top,data.right,data.bottom);
				break;
			}
		  case kDataType_Text:
			{
				AText	data;
				GetData_Text(dataID,data);
				data.ConvertToPrefTextEscape();
				text.Add('\"');
				text.AddText(data);
				text.Add('\"');
				break;
			}
		  case kDataType_Color:
			{
				AColor	data;
				GetData_Color(dataID,data);
				AText	t;
				AColorWrapper::GetHTMLFormatColorText(data,t);
				text.Add('\"');
				text.AddText(t);
				text.Add('\"');
				break;
			}
		  case kDataType_TextArray:
			{
				text.AddCstring("\r{\r");
				AItemCount	count = GetItemCount_Array(dataID);
				for( AIndex i = 0; i < count; i++ )
				{
					text.Add('\t');
					AText	data;
					GetData_TextArray(dataID,i,data);
					data.ConvertToPrefTextEscape();
					text.Add('\"');
					text.AddText(data);
					text.Add('\"');
					if( i < count-1 )
					{
						text.AddCstring(",\r");
					}
				}
				text.AddCstring("\r}");
				break;
			}
		  case kDataType_BoolArray:
			{
				text.AddCstring("\r{\r");
				AItemCount	count = GetItemCount_Array(dataID);
				for( AIndex i = 0; i < count; i++ )
				{
					text.Add('\t');
					if( GetData_BoolArray(dataID,i) == true )
					{
						text.AddCstring("\"true\"");
					}
					else
					{
						text.AddCstring("\"false\"");
					}
					if( i < count-1 )
					{
						text.AddCstring(",\r");
					}
				}
				text.AddCstring("\r}");
				break;
			}
		  case kDataType_NumberArray:
			{
				text.AddCstring("\r{\r");
				AItemCount	count = GetItemCount_Array(dataID);
				for( AIndex i = 0; i < count; i++ )
				{
					text.Add('\t');
					text.AddFormattedCstring("\"%d\"",GetData_NumberArray(dataID,i));
					if( i < count-1 )
					{
						text.AddCstring(",\r");
					}
				}
				text.AddCstring("\r}");
				break;
			}
		  case kDataType_ColorArray:
			{
				text.AddCstring("\r{\r");
				AItemCount	count = GetItemCount_Array(dataID);
				for( AIndex i = 0; i < count; i++ )
				{
					text.Add('\t');
					AColor	color;
					GetData_ColorArray(dataID,i,color);
					AText	t;
					AColorWrapper::GetHTMLFormatColorText(color,t);
					text.Add('\"');
					text.AddText(t);
					text.Add('\"');
					if( i < count-1 )
					{
						text.AddCstring(",\r");
					}
				}
				text.AddCstring("\r}");
				break;
			}
		  default:
			{
				_ACError("",NULL);
				break;
			}
		}
		text.Add('\r');
	}
	//#881
	//今回認識できなかったデータの部分のテキストを書き込む（このバージョンよりも新しいバージョンで書き込んだpref textを読み込んだ場合に、新規データを消さないようにするため）
	text.AddText(mUnusedPrefText);
	//
	text.ConvertLineEndTo(10);//LFに変換して保存（svn, diffとの親和性を考えて）
	inFileAcc.CreateFile();
	inFileAcc.WriteFile(text);
}

#pragma mark ---単一データ

#pragma mark ---Bool
void	ADataBase::CreateData_Bool( ADataID inID, AConstCharPtr inFileKey, const ABool& inInitial )
{
	AIndex	index = mBool_Data.Add(inInitial);
	mBool_Default.Add(inInitial);
	Register(inID,inFileKey,kDataType_Bool,index);
}

ABool	ADataBase::GetData_Bool( ADataID inID ) const
{
	return mBool_Data.Get(GetIndexInEach(inID,kDataType_Bool));
}

void	ADataBase::SetData_Bool( ADataID inID, const ABool& inData )
{
	mBool_Data.Set(GetIndexInEach(inID,kDataType_Bool),inData);
	DataIsSet(inID);
}

//デフォルトに設定
void	ADataBase::SetDefault_Bool( ADataID inID )
{
	AIndex	index = GetIndexInEach(inID,kDataType_Bool);
	mBool_Data.Set(index,mBool_Default.Get(index));
	DataIsSetToDefault(inID);
}

#pragma mark ---Number
void	ADataBase::CreateData_Number( ADataID inID, AConstCharPtr inFileKey, const ANumber& inInitial, ANumber inMin, ANumber inMax )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumber_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = mNumber_Data.Add(inInitial);
	mNumber_Default.Add(inInitial);
	Register(inID,inFileKey,kDataType_Number,index);
	mNumber_Min.Add(inMin);
	mNumber_Max.Add(inMax);
}

ANumber	ADataBase::GetData_Number( ADataID inID ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumber_Data_Mutex,mEnableMutex);
	
	//
	return mNumber_Data.Get(GetIndexInEach(inID,kDataType_Number));
}

void	ADataBase::SetData_Number( ADataID inID, const ANumber& inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumber_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_Number);
	if( index == kIndex_Invalid )   return;
	
	ANumber	data = inData;
	if( data < mNumber_Min.Get(index) )   data = mNumber_Min.Get(index);
	if( data > mNumber_Max.Get(index) )   data = mNumber_Max.Get(index);
	mNumber_Data.Set(index,data);
	DataIsSet(inID);
}

//デフォルトに設定
void	ADataBase::SetDefault_Number( ADataID inID )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumber_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_Number);
	mNumber_Data.Set(index,mNumber_Default.Get(index));
	DataIsSetToDefault(inID);
}

#pragma mark ---FloatNumber

void	ADataBase::CreateData_FloatNumber( ADataID inID, AConstCharPtr inFileKey, const AFloatNumber& inInitial, 
		AFloatNumber inMin, AFloatNumber inMax )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mFloatNumber_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = mFloatNumber_Data.Add(inInitial);
	mFloatNumber_Default.Add(inInitial);
	Register(inID,inFileKey,kDataType_FloatNumber,index);
	mFloatNumber_Min.Add(inMin);
	mFloatNumber_Max.Add(inMax);
}

AFloatNumber	ADataBase::GetData_FloatNumber( ADataID inID ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mFloatNumber_Data_Mutex,mEnableMutex);
	
	//
	return mFloatNumber_Data.Get(GetIndexInEach(inID,kDataType_FloatNumber));
}

void	ADataBase::SetData_FloatNumber( ADataID inID, const AFloatNumber& inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mFloatNumber_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_FloatNumber);
	if( index == kIndex_Invalid )   return;
	
	AFloatNumber	data = inData;
	if( data < mFloatNumber_Min.Get(index) )   data = mFloatNumber_Min.Get(index);
	if( data > mFloatNumber_Max.Get(index) )   data = mFloatNumber_Max.Get(index);
	mFloatNumber_Data.Set(index,data);
	DataIsSet(inID);
}

//デフォルトに設定
void	ADataBase::SetDefault_FloatNumber( ADataID inID )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mFloatNumber_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_FloatNumber);
	mFloatNumber_Data.Set(index,mFloatNumber_Default.Get(index));
	DataIsSetToDefault(inID);
}

#pragma mark ---Point
void	ADataBase::CreateData_Point( ADataID inID, AConstCharPtr inFileKey, ANumber inInitialX, ANumber inInitialY,
		ANumber inXMin, ANumber inXMax, ANumber inYMin, ANumber inYMax )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mPoint_Data_Mutex,mEnableMutex);
	
	//
	APoint	initial;
	initial.x = inInitialX;
	initial.y = inInitialY;
	AIndex	index = mPoint_Data.Add(initial);
	mPoint_Default.Add(initial);
	Register(inID,inFileKey,kDataType_Point,index);
	mPoint_XMin.Add(inXMin);
	mPoint_XMax.Add(inXMax);
	mPoint_YMin.Add(inYMin);
	mPoint_YMax.Add(inYMax);
}

void	ADataBase::GetData_Point( ADataID inID, APoint& outData ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mPoint_Data_Mutex,mEnableMutex);
	
	//
	outData = mPoint_Data.Get(GetIndexInEach(inID,kDataType_Point));
}

void	ADataBase::SetData_Point( ADataID inID, const APoint& inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mPoint_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_Point);
	if( index == kIndex_Invalid )   return;
	
	APoint	data = inData;
	if( data.x < mPoint_XMin.Get(index) )   data.x = mPoint_XMin.Get(index);
	if( data.x > mPoint_XMax.Get(index) )   data.x = mPoint_XMax.Get(index);
	if( data.y < mPoint_YMin.Get(index) )   data.y = mPoint_YMin.Get(index);
	if( data.y > mPoint_YMax.Get(index) )   data.y = mPoint_YMax.Get(index);
	mPoint_Data.Set(index,data);
	DataIsSet(inID);
}

//デフォルトに設定
void	ADataBase::SetDefault_Point( ADataID inID )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mPoint_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_Point);
	mPoint_Data.Set(index,mPoint_Default.Get(index));
	DataIsSetToDefault(inID);
}

#pragma mark ---Rect

void	ADataBase::CreateData_Rect( ADataID inID, AConstCharPtr inFileKey, 
		ANumber inInitialLeft, ANumber inInitialTop, ANumber inInitialRight, ANumber inInitialBottom,
		ANumber inLeftMin, ANumber inTopMin, ANumber inRightMax, ANumber inBottomMax )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mRect_Data_Mutex,mEnableMutex);
	
	//
	ARect	initial;
	initial.left = inInitialLeft;
	initial.top = inInitialTop;
	initial.right = inInitialRight;
	initial.bottom = inInitialBottom;
	AIndex	index = mRect_Data.Add(initial);
	mRect_Default.Add(initial);
	Register(inID,inFileKey,kDataType_Rect,index);
	
	mRect_LeftMin.Add(inLeftMin);
	mRect_TopMin.Add(inTopMin);
	mRect_RightMax.Add(inRightMax);
	mRect_BottomMax.Add(inBottomMax);
}

void	ADataBase::GetData_Rect( ADataID inID, ARect& outData ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mRect_Data_Mutex,mEnableMutex);
	
	//
	outData = mRect_Data.Get(GetIndexInEach(inID,kDataType_Rect));
}

void	ADataBase::SetData_Rect( ADataID inID, const ARect& inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mRect_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_Rect);
	if( index == kIndex_Invalid )   return;
	
	ARect	data = inData;
	if( data.left < mRect_LeftMin.Get(index) )		data.left 	= mRect_LeftMin.Get(index);
	if( data.top < mRect_TopMin.Get(index) )		data.top 	= mRect_TopMin.Get(index);
	if( data.right > mRect_RightMax.Get(index) )	data.right 	= mRect_RightMax.Get(index);
	if( data.bottom > mRect_BottomMax.Get(index) )	data.bottom	= mRect_BottomMax.Get(index);
	mRect_Data.Set(index,data);
	DataIsSet(inID);
}

//デフォルトに設定
void	ADataBase::SetDefault_Rect( ADataID inID )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mRect_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_Rect);
	mRect_Data.Set(index,mRect_Default.Get(index));
	DataIsSetToDefault(inID);
}

#pragma mark ---Color

void	ADataBase::CreateData_Color( ADataID inID, AConstCharPtr inFileKey, const AColor& initial )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColor_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = mColor_Data.Add(initial);
	mColor_Default.Add(initial);
	Register(inID,inFileKey,kDataType_Color,index);
}

void	ADataBase::GetData_Color( ADataID inID, AColor& outData ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColor_Data_Mutex,mEnableMutex);
	
	//
	outData = mColor_Data.Get(GetIndexInEach(inID,kDataType_Color));
}

void	ADataBase::SetData_Color( ADataID inID, const AColor& inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColor_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_Color);
	if( index == kIndex_Invalid )   return;
	
	mColor_Data.Set(index,inData);
	DataIsSet(inID);
}

//デフォルトに設定
void	ADataBase::SetDefault_Color( ADataID inID )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColor_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_Color);
	mColor_Data.Set(index,mColor_Default.Get(index));
	DataIsSetToDefault(inID);
}

#pragma mark ---Text
void	ADataBase::CreateData_Text( ADataID inID, AConstCharPtr inFileKey, const AText& inInitial,
		ABool inLimitLength, AItemCount inMinLength, AItemCount inMaxLength )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mText_Data_Mutex,mEnableMutex);
	
	//
	AIndex	newIndex = mText_Data.Add(inInitial);//#693
	mText_Default.Add(inInitial);//#693
	//
	Register(inID,inFileKey,kDataType_Text,newIndex);
	
	mText_LimitLength.Add(inLimitLength);
	mText_MinLength.Add(inMinLength);
	mText_MaxLength.Add(inMaxLength);
}

void	ADataBase::GetData_Text( ADataID inID, AText& outText ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mText_Data_Mutex,mEnableMutex);
	
	//
	outText.DeleteAll();
	//#693 outText.SetText(mText_Data.GetObjectConst(GetIndexInEach(inID,kDataType_Text)));
	mText_Data.Get(GetIndexInEach(inID,kDataType_Text),outText);//#693
}

void	ADataBase::SetData_Text( ADataID inID, const AText& inText )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mText_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_Text);
	if( index == kIndex_Invalid )   return;
	
	AText	data;
	data.SetText(inText);
	if( mText_LimitLength.Get(index) == true )
	{
		data.LimitLength(mText_MinLength.Get(index),mText_MaxLength.Get(index));
	}
	//#693 mText_Data.GetObject(index).SetText(inText);
	mText_Data.Set(index,inText);//#693
	DataIsSet(inID);
}

//デフォルトに設定
void	ADataBase::SetDefault_Text( ADataID inID )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mText_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_Text);
	//#693 mText_Data.GetObject(index).SetText(mText_Default.GetObjectConst(index));
	mText_Data.Set(index,mText_Default.GetTextConst(index));//#693
	DataIsSetToDefault(inID);
}

//#688
/**
フォント名として存在しないテキストが設定されていた場合は、デフォルトのフォント名に設定する
*/
void	ADataBase::AdjustFontName( const ADataID inID )
{
	AText	fontname;
	GetData_Text(inID,fontname);
	if( AFontWrapper::IsFontEnabled(fontname) == false )
	{
		AFontWrapper::GetAppDefaultFontName(fontname);
		SetData_Text(inID,fontname);
	}
}

#pragma mark ---配列型データ

#pragma mark ---BoolArray
void	ADataBase::CreateData_BoolArray( ADataID inID, AConstCharPtr inFileKey, ABool inDefaultData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mBoolArray_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = mBoolArray_Data.AddNewObject();
	Register(inID,inFileKey,kDataType_BoolArray,index);
	mBoolArray_Default.Add(inDefaultData);
	SetupAsColumn_Table(inID);
}

ABool	ADataBase::GetDefault_BoolArray( ADataID inID )
{
	return mBoolArray_Default.Get(GetIndexInEach(inID,kDataType_BoolArray));
}

ABool	ADataBase::GetData_BoolArray( ADataID inID, AIndex inIndex ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mBoolArray_Data_Mutex,mEnableMutex);
	
	//
	return GetData_ConstBoolArrayRef(inID).Get(inIndex);
}

void	ADataBase::SetData_BoolArray( ADataID inID, AIndex inIndex, ABool inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mBoolArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_BoolArrayRef(inID).Set(inIndex,inData);
}

void	ADataBase::SetData_BoolArray( ADataID inID, const ABoolArray& inArray )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mBoolArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_BoolArrayRef(inID).SetFromObject(inArray);
}

void	ADataBase::Delete1_BoolArray( ADataID inID, AIndex inIndex )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mBoolArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_BoolArrayRef(inID).Delete1(inIndex);
}

void	ADataBase::DeleteAll_BoolArray( ADataID inID )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mBoolArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_BoolArrayRef(inID).DeleteAll();
}

void	ADataBase::Insert1_BoolArray( ADataID inID, AIndex inIndex, ABool inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mBoolArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_BoolArrayRef(inID).Insert1(inIndex,inData);
}

//#695
/**
領域確保（値は不定）
*/
void	ADataBase::Reserve_BoolArray( const ADataID inID, const AIndex inIndex, const AItemCount inCount )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mBoolArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_BoolArrayRef(inID).Reserve(inIndex,inCount);
}

void	ADataBase::Add_BoolArray( ADataID inID, ABool inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mBoolArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_BoolArrayRef(inID).Add(inData);
}

AItemCount	ADataBase::GetItemCount_BoolArray( ADataID inID ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mBoolArray_Data_Mutex,mEnableMutex);
	
	//
	return GetData_ConstBoolArrayRef(inID).GetItemCount();
}

ABoolArray&	ADataBase::GetData_BoolArrayRef( ADataID inID )
{
	return mBoolArray_Data.GetObject(GetIndexInEach(inID,kDataType_BoolArray));
}

const ABoolArray&	ADataBase::GetData_ConstBoolArrayRef( ADataID inID ) const
{
	return mBoolArray_Data.GetObjectConst(GetIndexInEach(inID,kDataType_BoolArray));
}

#pragma mark ---------- NumberArray型 ----------

void	ADataBase::CreateData_NumberArray( ADataID inID, AConstCharPtr inFileKey, ANumber inDefaultData, ANumber inMin, ANumber inMax )//#695, const ABool inEnableHash )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumberArray_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = mNumberArray_Data.AddNewObject();
	/*#695 AHashNumberArray→ANumberArray（既にHashとして必要不可欠な使用箇所がなく、Reserve_NumberArray時の処理時間コスト等も考慮すると、NumbarArrayの方がよい。必要なら別途HashNumberArrayを作る。
	if( inEnableHash == false )
	{
		mNumberArray_Data.GetObject(index).DisableHash();
	}
	*/
	Register(inID,inFileKey,kDataType_NumberArray,index);
	mNumberArray_Default.Add(inDefaultData);
	mNumberArray_Min.Add(inMin);
	mNumberArray_Max.Add(inMax);
	SetupAsColumn_Table(inID);
}

void	ADataBase::SetData_NumberArray( ADataID inID, AIndex inIndex, ANumber inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumberArray_Data_Mutex,mEnableMutex);
	
	//
	ANumber	data = inData;
	AIndex	index = GetIndexInEach(inID,kDataType_NumberArray);
	if( index == kIndex_Invalid )   return;
	if( data < mNumberArray_Min.Get(index) )   data = mNumberArray_Min.Get(index);
	if( data > mNumberArray_Max.Get(index) )   data = mNumberArray_Max.Get(index);
	GetData_NumberArrayRef(inID).Set(inIndex,data);
}

void	ADataBase::SetData_NumberArray( ADataID inID, const ANumberArray& inArray )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumberArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_NumberArrayRef(inID).SetFromObject(inArray);
}

ANumber	ADataBase::GetDefault_NumberArray( ADataID inID )
{
	return mNumberArray_Default.Get(GetIndexInEach(inID,kDataType_NumberArray));
}

ANumber	ADataBase::GetData_NumberArray( ADataID inID, AIndex inIndex ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumberArray_Data_Mutex,mEnableMutex);
	
	//
	return GetData_ConstNumberArrayRef(inID).Get(inIndex);
}

void	ADataBase::Delete1_NumberArray( ADataID inID, AIndex inIndex )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumberArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_NumberArrayRef(inID).Delete1(inIndex);
}

void	ADataBase::DeleteAll_NumberArray( ADataID inID )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumberArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_NumberArrayRef(inID).DeleteAll();
}

void	ADataBase::Insert1_NumberArray( ADataID inID, AIndex inIndex, ANumber inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumberArray_Data_Mutex,mEnableMutex);
	
	//
	ANumber	data = inData;
	AIndex	index = GetIndexInEach(inID,kDataType_NumberArray);
	if( index == kIndex_Invalid )   return;
	if( data < mNumberArray_Min.Get(index) )   data = mNumberArray_Min.Get(index);
	if( data > mNumberArray_Max.Get(index) )   data = mNumberArray_Max.Get(index);
	GetData_NumberArrayRef(inID).Insert1(inIndex,data);
}

//#695
/**
領域確保（値は不定）
*/
void	ADataBase::Reserve_NumberArray( const ADataID inID, const AIndex inIndex, const AItemCount inCount )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumberArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_NumberArrayRef(inID).Reserve(inIndex,inCount);
}

void	ADataBase::Insert_NumberArray( ADataID inID, AIndex inIndex, const ANumberArray& inNumberArray )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumberArray_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = GetIndexInEach(inID,kDataType_NumberArray);
	if( index == kIndex_Invalid )   return;
	ANumberArray	array;
	for( AIndex i = 0; i < inNumberArray.GetItemCount(); i++ )
	{
		ANumber	data = inNumberArray.Get(i);
		if( data < mNumberArray_Min.Get(index) )   data = mNumberArray_Min.Get(index);
		if( data > mNumberArray_Max.Get(index) )   data = mNumberArray_Max.Get(index);
		array.Add(data);
	}
	GetData_NumberArrayRef(inID).Insert(inIndex,array);
}

void	ADataBase::Add_NumberArray( ADataID inID, ANumber inData )
{
	Insert1_NumberArray(inID,GetItemCount_NumberArray(inID),inData);
}

AItemCount	ADataBase::GetItemCount_NumberArray( ADataID inID ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumberArray_Data_Mutex,mEnableMutex);
	
	//
	return GetData_ConstNumberArrayRef(inID).GetItemCount();
}

ANumberArray&	ADataBase::GetData_NumberArrayRef( ADataID inID )
{
	return mNumberArray_Data.GetObject(GetIndexInEach(inID,kDataType_NumberArray));
}

const ANumberArray&	ADataBase::GetData_ConstNumberArrayRef( ADataID inID ) const
{
	return mNumberArray_Data.GetObjectConst(GetIndexInEach(inID,kDataType_NumberArray));
}

AIndex	ADataBase::Find_NumberArray( const ADataID inID, const ANumber inData ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mNumberArray_Data_Mutex,mEnableMutex);
	
	//
	return GetData_ConstNumberArrayRef(inID).Find(inData);
}

#pragma mark ---------- ColorArray型 ----------
void	ADataBase::CreateData_ColorArray( ADataID inID, AConstCharPtr inFileKey, const AColor& inDefaultData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColorArray_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = mColorArray_Data.AddNewObject();
	Register(inID,inFileKey,kDataType_ColorArray,index);
	mColorArray_Default.Add(inDefaultData);
	SetupAsColumn_Table(inID);
}

void	ADataBase::SetData_ColorArray( ADataID inID, const AIndex& inIndex, const AColor& inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColorArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_ColorArrayRef(inID).Set(inIndex,inData);
}

void	ADataBase::SetData_ColorArray( ADataID inID, const AColorArray& inArray )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColorArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_ColorArrayRef(inID).SetFromObject(inArray);
}

void	ADataBase::Insert1_ColorArray( ADataID inID, const AIndex& inIndex, const AColor& inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColorArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_ColorArrayRef(inID).Insert1(inIndex,inData);
}

//#695
/**
領域確保（値は不定）
*/
void	ADataBase::Reserve_ColorArray( const ADataID inID, const AIndex inIndex, const AItemCount inCount )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColorArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_ColorArrayRef(inID).Reserve(inIndex,inCount);
}

void	ADataBase::Add_ColorArray( ADataID inID, const AColor& inData )
{
	Insert1_ColorArray(inID,GetItemCount_ColorArray(inID),inData);
}

void	ADataBase::GetData_ColorArray( ADataID inID, const AIndex& inIndex, AColor& outData ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColorArray_Data_Mutex,mEnableMutex);
	
	//
	outData = GetData_ConstColorArrayRef(inID).Get(inIndex);
}

void	ADataBase::Delete1_ColorArray( ADataID inID, const AIndex& inIndex )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColorArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_ColorArrayRef(inID).Delete1(inIndex);
}

void	ADataBase::DeleteAll_ColorArray( ADataID inID )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColorArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_ColorArrayRef(inID).DeleteAll();
}

AItemCount	ADataBase::GetItemCount_ColorArray( ADataID inID ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mColorArray_Data_Mutex,mEnableMutex);
	
	//
	return GetData_ConstColorArrayRef(inID).GetItemCount();
}

void	ADataBase::GetDefault_ColorArray( ADataID inID, AColor& outData )
{
	outData = mColorArray_Default.Get(GetIndexInEach(inID,kDataType_ColorArray));
}

const AColorArray&	ADataBase::GetData_ConstColorArrayRef( ADataID inID ) const
{
	return mColorArray_Data.GetObjectConst(GetIndexInEach(inID,kDataType_ColorArray));
}

AColorArray&	ADataBase::GetData_ColorArrayRef( ADataID inID )
{
	return mColorArray_Data.GetObject(GetIndexInEach(inID,kDataType_ColorArray));
}

#pragma mark ---------- TextArray型 ----------

void	ADataBase::CreateData_TextArray( ADataID inID, AConstCharPtr inFileKey, AConstCharPtr inDefaultData,
		ABool inLimitLength, AItemCount inMinLength, AItemCount inMaxLength )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mTextArray_Data_Mutex,mEnableMutex);
	
	//
	AIndex	index = mTextArray_Data.AddNewObject();
	Register(inID,inFileKey,kDataType_TextArray,index);
	AText	defaultdata(inDefaultData);
	mTextArray_Default.Add(defaultdata);
	//データ制約
	mTextArray_LimitLength.Add(inLimitLength);
	mTextArray_MinLength.Add(inMinLength);
	mTextArray_MaxLength.Add(inMaxLength);
	SetupAsColumn_Table(inID);
}

void	ADataBase::SetData_TextArray( const ADataID inID, const AIndex inIndex, const AText& inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mTextArray_Data_Mutex,mEnableMutex);
	
	//
	//制限チェック
	if( CheckLength_TextArray(inID,inData) == true )
	{
		GetData_TextArrayRef(inID).Set(inIndex,inData);
	}
	else
	{
		//制限文字数にきりつめ
		AText	text;
		text.SetText(inData);
		LimitLength_TextArray(inID,text);
		
		//データ設定
		GetData_TextArrayRef(inID).Set(inIndex,text);
	}
}

/*#693
//
//SetData_TextArrayよりこちらのほうが高速
void	ADataBase::SetData_TextArray_SwapContent( const ADataID inID, const AIndex inIndex, AText& ioData )
{
	//制限チェック
	if( CheckLength_TextArray(inID,ioData) == true )
	{
		//制限文字数にきりつめ
		LimitLength_TextArray(inID,ioData);
	}
	GetData_TextArrayRef(inID).Set_SwapContent(inIndex,ioData);
}
*/
void	ADataBase::SetData_TextArray( const ADataID inID, const ATextArray& inArray )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mTextArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_TextArrayRef(inID).SetFromTextArray(inArray);
}

void	ADataBase::Insert1_TextArray( ADataID inID, AIndex inIndex, const AText& inData )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mTextArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_TextArrayRef(inID).Insert1(inIndex,inData);
}

/*#693
void	ADataBase::Insert1_TextArray_SwapContent( ADataID inID, AIndex inIndex, AText& ioData )
{
	//#890
	AStMutexLocker	locker(mTextArray_Data_Mutex,mEnableMutex);
	
	//
	//制限チェック
	if( CheckLength_TextArray(inID,ioData) == true )
	{
		//制限文字数にきりつめ
		LimitLength_TextArray(inID,ioData);
	}
	GetData_TextArrayRef(inID).Insert1_SwapContent(inIndex,ioData);
}
*/

//#695
/**
領域確保（値は不定）
*/
void	ADataBase::Reserve_TextArray( const ADataID inID, const AIndex inIndex, const AItemCount inCount )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mTextArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_TextArrayRef(inID).Reserve(inIndex,inCount);
}

AIndex	ADataBase::Add_TextArray( ADataID inID, const AText& inData )
{
	AIndex	index = GetItemCount_TextArray(inID);
	Insert1_TextArray(inID,index,inData);
	return index;
}

void	ADataBase::AddRecentlyUsed_TextArray( ADataID inID, const AText& inData, const ANumber& inLimitCount )
{
	AIndex	dataIndex = GetIndexInEach(inID,kDataType_TextArray);
	//既に同じテキストが存在していたら、それを削除する
	AIndex	rowIndex = mTextArray_Data.GetObjectConst(dataIndex).Find(inData);
	if( rowIndex != kIndex_Invalid )
	{
		Delete1_TextArray(inID,rowIndex);
	}
	//追加
	Insert1_TextArray(inID,0,inData);
	//制限数より多くなったら最後を削除
	if( GetItemCount_TextArray(inID) > inLimitCount )
	{
		Delete1_TextArray(inID,GetItemCount_TextArray(inID)-1);
	}
}

void	ADataBase::GetData_TextArray( ADataID inID, AIndex inIndex, AText& outData ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mTextArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_ConstTextArrayRef(inID).Get(inIndex,outData);
}

const ATextArray&	ADataBase::GetData_ConstTextArrayRef( ADataID inID ) const
{
	return mTextArray_Data.GetObjectConst(GetIndexInEach(inID,kDataType_TextArray));
}

AIndex	ADataBase::Find_TextArray( ADataID inID, const AText& inText, const ABool& inIgnoreCase ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mTextArray_Data_Mutex,mEnableMutex);
	
	//
	if( inIgnoreCase == false )
	{
		return GetData_ConstTextArrayRef(inID).Find(inText);
	}
	else
	{
		return GetData_ConstTextArrayRef(inID).FindIgnoreCase(inText);
	}
}

void	ADataBase::Delete1_TextArray( ADataID inID, AIndex inIndex )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mTextArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_TextArrayRef(inID).Delete1(inIndex);
}

void	ADataBase::DeleteAll_TextArray( ADataID inID )
{
	//#890
	//排他制御
	AStMutexLocker	locker(mTextArray_Data_Mutex,mEnableMutex);
	
	//
	GetData_TextArrayRef(inID).DeleteAll();
}

AItemCount	ADataBase::GetItemCount_TextArray( ADataID inID ) const
{
	//#890
	//排他制御
	AStMutexLocker	locker(mTextArray_Data_Mutex,mEnableMutex);
	
	//
	return GetData_ConstTextArrayRef(inID).GetItemCount();
}

void	ADataBase::GetDefault_TextArray( ADataID inID, AText& outData )
{
	mTextArray_Default.Get(GetIndexInEach(inID,kDataType_TextArray),outData);
}

ABool	ADataBase::CheckLength_TextArray( const ADataID inID, const AText& inText ) const
{
	AIndex	index = GetIndexInEach(inID,kDataType_TextArray);
	if( index == kIndex_Invalid )   return true;
	if( mTextArray_LimitLength.Get(index) == true )
	{
		if( inText.GetItemCount() < mTextArray_MinLength.Get(index) )   return false;
		if( inText.GetItemCount() > mTextArray_MaxLength.Get(index) )   return false;
	}
	return true;
}

void	ADataBase::LimitLength_TextArray( ADataID inID, AText& ioData )
{
	AIndex	index = GetIndexInEach(inID,kDataType_TextArray);
	if( index == kIndex_Invalid )   return;
	if( mTextArray_LimitLength.Get(index) == true )
	{
		ioData.LimitLength(mTextArray_MinLength.Get(index),mTextArray_MaxLength.Get(index));
	}
}

ATextArray&	ADataBase::GetData_TextArrayRef( ADataID inID )
{
	return mTextArray_Data.GetObject(GetIndexInEach(inID,kDataType_TextArray));
}

#pragma mark --------- Array型共通 ----------

AItemCount	ADataBase::GetItemCount_Array( ADataID inID ) const
{
	switch(GetDataType(inID))
	{
	  case kDataType_BoolArray:
		{
			return GetItemCount_BoolArray(inID);
		}
	  case kDataType_NumberArray:
		{
			return GetItemCount_NumberArray(inID);
		}
	  case kDataType_TextArray:
		{
			return GetItemCount_TextArray(inID);
		}
	  case kDataType_ColorArray:
		{
			return GetItemCount_ColorArray(inID);
		}
	  default:
		{
			_ACError("not array type",this);
			return 1;
		}
	}
}

#pragma mark ---テーブル

/*
複数のArrayデータの集まりをテーブルとして扱うことができる。
下記のように、CreateData_XXXArrayをCreateTable_Start(), CreateTable_End()で囲むことによって、
間で作成したArrayを列とする、テーブルとして扱うことが出来、行の追加、削除、スワップ等が可能になる。
CreateTable_Start();
CreateData_XXXArray();
CreateData_XXXArray();
CreateData_XXXArray();
CreateTable_End();
*/

AItemCount	ADataBase::GetRowCount_Table( ADataID inFirstIDInGroup )
{
	return GetItemCount_Array(inFirstIDInGroup);
}

void	ADataBase::CreateTableStart()
{
	mCurrentRegisteringTableIndex = mTableIDArray.AddNewObject();
}

void	ADataBase::SetupAsColumn_Table( ADataID inID )
{
	if( mCurrentRegisteringTableIndex == kIndex_Invalid )   return;
	mTableIDArray.GetObject(mCurrentRegisteringTableIndex).Add(inID);
}

void	ADataBase::CreateTableEnd()
{
	mCurrentRegisteringTableIndex = kIndex_Invalid;
}

AIndex	ADataBase::FindTable( ADataID inFirstIDInGroup )
{
	for( AIndex tableIndex = 0; tableIndex < mTableIDArray.GetItemCount(); tableIndex++ )
	{
		if( mTableIDArray.GetObjectConst(tableIndex).GetItemCount() <= 0 )   continue;
		if( inFirstIDInGroup == mTableIDArray.GetObjectConst(tableIndex).Get(0) )
		{
			return tableIndex;
		}
	}
	_ACError("table not found",this);
	return kIndex_Invalid;
}

//テーブル内の１行を移動する（削除して挿入）
//inDeleteItemIndex番目の行を削除して、（削除後の）inNewItemIndex番目の位置にそのデータを挿入する。
void	ADataBase::MoveRow_Table( ADataID inFirstIDInGroup, const AIndex& inDeleteItemIndex, const AIndex& inNewItemIndex  )
{
	AIndex	tableIndex = FindTable(inFirstIDInGroup);
	if( tableIndex == kIndex_Invalid )   return;
	for( AIndex i = 0; i < mTableIDArray.GetObjectConst(tableIndex).GetItemCount(); i++ )
	{
		ADataID	id = mTableIDArray.GetObjectConst(tableIndex).Get(i);
		switch(GetDataType(id))
		{
		  case kDataType_BoolArray:
			{
				ABool	data = GetData_BoolArray(id,inDeleteItemIndex);
				Delete1_BoolArray(id,inDeleteItemIndex);
				Insert1_BoolArray(id,inNewItemIndex,data);
				break;
			}
		  case kDataType_NumberArray:
			{
				ANumber	data = GetData_NumberArray(id,inDeleteItemIndex);
				Delete1_NumberArray(id,inDeleteItemIndex);
				Insert1_NumberArray(id,inNewItemIndex,data);
				break;
			}
		  case kDataType_TextArray:
			{
				AText	data;
				GetData_TextArray(id,inDeleteItemIndex,data);
				Delete1_TextArray(id,inDeleteItemIndex);
				Insert1_TextArray(id,inNewItemIndex,data);
				break;
			}
		  case kDataType_ColorArray:
			{
				AColor	data;
				GetData_ColorArray(id,inDeleteItemIndex,data);
				Delete1_ColorArray(id,inDeleteItemIndex);
				Insert1_ColorArray(id,inNewItemIndex,data);
				break;
			}
		  default:
			{
				_ACError("not array type",this);
				break;
			}
		}
	}
}

//テーブル内の各データのinIndexで指定したものを一括削除する。
void	ADataBase::DeleteRow_Table( ADataID inFirstIDInGroup, AIndex inIndex )
{
	AIndex	tableIndex = FindTable(inFirstIDInGroup);
	if( tableIndex == kIndex_Invalid )   return;
	//テーブル内の各データごとのループ。各データのindex番目のデータを削除する。
	for( AIndex i = 0; i < mTableIDArray.GetObjectConst(tableIndex).GetItemCount(); i++ )
	{
		ADataID	id = mTableIDArray.GetObjectConst(tableIndex).Get(i);
		switch(GetDataType(id))
		{
		  case kDataType_BoolArray:
			{
				Delete1_BoolArray(id,inIndex);
				break;
			}
		  case kDataType_NumberArray:
			{
				Delete1_NumberArray(id,inIndex);
				break;
			}
		  case kDataType_TextArray:
			{
				Delete1_TextArray(id,inIndex);
				break;
			}
		  case kDataType_ColorArray:
			{
				Delete1_ColorArray(id,inIndex);
				break;
			}
		  default:
			{
				_ACError("not array type",this);
				break;
			}
		}
	}
}

//#695
/**
指定テーブルのデータを全削除
*/
void	ADataBase::DeleteAllRows_Table( ADataID inFirstIDInGroup )
{
	AIndex	tableIndex = FindTable(inFirstIDInGroup);
	if( tableIndex == kIndex_Invalid )   return;
	//テーブル内の各データごとのループ。各データのindex番目のデータを削除する。
	for( AIndex i = 0; i < mTableIDArray.GetObjectConst(tableIndex).GetItemCount(); i++ )
	{
		ADataID	id = mTableIDArray.GetObjectConst(tableIndex).Get(i);
		switch(GetDataType(id))
		{
		  case kDataType_BoolArray:
			{
				DeleteAll_BoolArray(id);
				break;
			}
		  case kDataType_NumberArray:
			{
				DeleteAll_NumberArray(id);
				break;
			}
		  case kDataType_TextArray:
			{
				DeleteAll_TextArray(id);
				break;
			}
		  case kDataType_ColorArray:
			{
				DeleteAll_ColorArray(id);
				break;
			}
		  default:
			{
				_ACError("not array type",this);
				break;
			}
		}
	}
}

//テーブル内の各データにデフォルトの値を１つ一括追加する。
AIndex	ADataBase::AddRow_Table( ADataID inFirstIDInGroup )//#427
{
	AIndex	tableIndex = FindTable(inFirstIDInGroup);
	if( tableIndex == kIndex_Invalid )   return kIndex_Invalid;//#427
	//テーブル内の各データごとのループ。各データにデフォルトの値を１つ追加する。
	for( AIndex i = 0; i < mTableIDArray.GetObjectConst(tableIndex).GetItemCount(); i++ )
	{
		ADataID	id = mTableIDArray.GetObjectConst(tableIndex).Get(i);
		switch(GetDataType(id))
		{
		  case kDataType_BoolArray:
			{
				Add_BoolArray(id,GetDefault_BoolArray(id));
				break;
			}
		  case kDataType_NumberArray:
			{
				Add_NumberArray(id,GetDefault_NumberArray(id));
				break;
			}
		  case kDataType_TextArray:
			{
				AText	text;
				GetDefault_TextArray(id,text);
				Add_TextArray(id,text);
				break;
			}
		  case kDataType_ColorArray:
			{
				AColor	data;
				GetDefault_ColorArray(id,data);
				Add_ColorArray(id,data);
				break;
			}
		  default:
			{
				_ACError("not array type",this);
				break;
			}
		}
	}
	return (GetItemCount_Array(inFirstIDInGroup)-1);
}

//#695
/**
テーブル内の各データにデフォルトの値を１つ一括追加する。
*/
void	ADataBase::InsertRow_Table( ADataID inFirstIDInGroup, const AIndex inRowIndex )
{
	AIndex	tableIndex = FindTable(inFirstIDInGroup);
	if( tableIndex == kIndex_Invalid )   return;//#427
	//テーブル内の各データごとのループ。各データにデフォルトの値を１つ追加する。
	for( AIndex i = 0; i < mTableIDArray.GetObjectConst(tableIndex).GetItemCount(); i++ )
	{
		ADataID	id = mTableIDArray.GetObjectConst(tableIndex).Get(i);
		switch(GetDataType(id))
		{
		  case kDataType_BoolArray:
			{
				Insert1_BoolArray(id,inRowIndex,GetDefault_BoolArray(id));
				break;
			}
		  case kDataType_NumberArray:
			{
				Insert1_NumberArray(id,inRowIndex,GetDefault_NumberArray(id));
				break;
			}
		  case kDataType_TextArray:
			{
				AText	text;
				GetDefault_TextArray(id,text);
				Insert1_TextArray(id,inRowIndex,text);
				break;
			}
		  case kDataType_ColorArray:
			{
				AColor	data;
				GetDefault_ColorArray(id,data);
				Insert1_ColorArray(id,inRowIndex,data);
				break;
			}
		  default:
			{
				_ACError("not array type",this);
				break;
			}
		}
	}
}

//#695
/**
テーブル内の各データに一括Reserveする。
*/
void	ADataBase::ReserveRow_Table( ADataID inFirstIDInGroup, const AIndex inRowIndex, const AItemCount inCount )
{
	AIndex	tableIndex = FindTable(inFirstIDInGroup);
	if( tableIndex == kIndex_Invalid )   return;//#427
	//テーブル内の各データごとのループ。各データにデフォルトの値を１つ追加する。
	for( AIndex i = 0; i < mTableIDArray.GetObjectConst(tableIndex).GetItemCount(); i++ )
	{
		ADataID	id = mTableIDArray.GetObjectConst(tableIndex).Get(i);
		switch(GetDataType(id))
		{
		  case kDataType_BoolArray:
			{
				Reserve_BoolArray(id,inRowIndex,inCount);
				break;
			}
		  case kDataType_NumberArray:
			{
				Reserve_NumberArray(id,inRowIndex,inCount);
				break;
			}
		  case kDataType_TextArray:
			{
				Reserve_TextArray(id,inRowIndex,inCount);
				break;
			}
		  case kDataType_ColorArray:
			{
				Reserve_ColorArray(id,inRowIndex,inCount);
				break;
			}
		  default:
			{
				_ACError("not array type",this);
				break;
			}
		}
	}
}

//#427
/**
他のDBオブジェクトから指定IDのテーブルの指定index(row)の内容を丸ごとコピーする
*/
void	ADataBase::InsertRowFromOtherDB_Table( const ADataID inFirstIDInGroup, const AIndex inIndex,
		const ADataBase& inOtherDB, const AIndex inIndexInOtherDB )
{
	AIndex	tableIndex = FindTable(inFirstIDInGroup);
	//テーブル内の各データごとのループ。各データにデフォルトの値を１つ追加する。
	for( AIndex i = 0; i < mTableIDArray.GetObjectConst(tableIndex).GetItemCount(); i++ )
	{
		ADataID	id = mTableIDArray.GetObjectConst(tableIndex).Get(i);
		switch(GetDataType(id))
		{
		  case kDataType_BoolArray:
			{
				Insert1_BoolArray(id,inIndex,inOtherDB.GetData_BoolArray(id,inIndexInOtherDB));
				break;
			}
		  case kDataType_NumberArray:
			{
				Insert1_NumberArray(id,inIndex,inOtherDB.GetData_NumberArray(id,inIndexInOtherDB));
				break;
			}
		  case kDataType_TextArray:
			{
				AText	text;
				inOtherDB.GetData_TextArray(id,inIndexInOtherDB,text);
				Insert1_TextArray(id,inIndex,text);
				break;
			}
		  case kDataType_ColorArray:
			{
				AColor	data;
				inOtherDB.GetData_ColorArray(id,inIndexInOtherDB,data);
				Insert1_ColorArray(id,inIndex,data);
				break;
			}
		  default:
			{
				_ACError("not array type",this);
				break;
			}
		}
	}
}

//B0000
void	ADataBase::CorrectTable_MakeColumnUnique( const ADataID inFirstIDInGroup, const ADataID inUniqueColumn )
{
	if( GetDataType(inFirstIDInGroup) != kDataType_TextArray )
	{
		_ACError("not text array",this);
		return;
	}
	AText	text1, text2;
	for( AIndex i = 0; i < GetItemCount_TextArray(inFirstIDInGroup); i++ )
	{
		GetData_TextArray(inFirstIDInGroup,i,text1);
		for( AIndex j = i+1; j < GetItemCount_TextArray(inFirstIDInGroup); )
		{
			GetData_TextArray(inFirstIDInGroup,j,text2);
			if( text1.Compare(text2) == true )
			{
				DeleteRow_Table(inFirstIDInGroup,j);
				//text2.OutputToStderr();
			}
			else
			{
				j++;
			}
		}
	}
}

//テーブル内の各データの数を合わせる。全テーブル対象。
//バージョンアップでデータが増えた場合のため。
void	ADataBase::CorrectTable()
{
	//各テーブルごとのループ
	for( AIndex tableIndex = 0; tableIndex < mTableIDArray.GetItemCount(); tableIndex++ )
	{
		if( mTableIDArray.GetObjectConst(tableIndex).GetItemCount() <= 0 )   continue;
		//テーブル内の項目数maxを取得する #734 最初の項目の数が不具合等により減少した場合もcorrectするように変更
		AItemCount	maxItemCount = 0;
		for( AIndex i = 0; i < mTableIDArray.GetObjectConst(tableIndex).GetItemCount(); i++ )
		{
			AItemCount	itemCount = GetItemCount_Array(mTableIDArray.GetObjectConst(tableIndex).Get(i));
			if( itemCount > maxItemCount )
			{
				maxItemCount = itemCount;
			}
		}
		//テーブル内の2番目以降のデータそれぞれについてのループ
		for( AIndex i = 0; i < mTableIDArray.GetObjectConst(tableIndex).GetItemCount(); i++ )
		{
			ADataID	id = mTableIDArray.GetObjectConst(tableIndex).Get(i);
			//DataIDがidのデータの数を、テーブルの最初のデータの数に合わせる。
			switch(GetDataType(id))
			{
			  case kDataType_BoolArray:
				{
					while( GetItemCount_BoolArray(id) < maxItemCount )
					{
						Add_BoolArray(id,GetDefault_BoolArray(id));
					}
					break;
				}
			  case kDataType_NumberArray:
				{
					while( GetItemCount_NumberArray(id) < maxItemCount )
					{
						Add_NumberArray(id,GetDefault_NumberArray(id));
					}
					break;
				}
			  case kDataType_TextArray:
				{
					while( GetItemCount_TextArray(id) < maxItemCount )
					{
						AText	text;
						GetDefault_TextArray(id,text);
						Add_TextArray(id,text);
					}
					break;
				}
			  case kDataType_ColorArray:
				{
					while( GetItemCount_ColorArray(id) < maxItemCount )
					{
						AColor	data;
						GetDefault_ColorArray(id,data);
						Add_ColorArray(id,data);
					}
					break;
				}
			  default:
				{
					_ACError("not array type",this);
					break;
				}
			}
		}
	}
}

#pragma mark ===========================
#pragma mark ---ダミーDBの取得

ADataBase			kDummyDB;

ADataBase&	GetDummyDB()
{
	return kDummyDB;
}
