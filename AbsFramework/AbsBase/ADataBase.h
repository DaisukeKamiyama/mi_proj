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

#pragma once

#include "ABase.h"
#include "../AbsBase_Imp/APrefFileImp.h"
//#include "ATypes.h"
#include "AObjectArrayItem.h"

enum ADataType
{
	kDataType_Unknown = -1,
	kDataType_Bool = 0,
	kDataType_Number,
	kDataType_FloatNumber,
	kDataType_Text,
	kDataType_Point,
	kDataType_Rect,
	//win kDataType_Font,
	kDataType_Color,
	kDataType_BoolArray,
	kDataType_NumberArray,
	kDataType_TextArray,
	kDataType_ColorArray,
	kDataType_NotFound
};

/**
データ管理クラス
設定データ等を格納・検索・ファイル書き出し・読み込み可能な簡易データベースクラス
*/
class ADataBase : public AObjectArrayItem
{
  public:
	//コンストラクタ、デストラクタ
	ADataBase( AObjectArrayItem* inParent = NULL );
	virtual ~ADataBase();
	
	//#890
  private:
	ABool	mEnableMutex;
	
  public:
	virtual void	SaveToFile();
	
	ADataType	GetDataType( ADataID inID ) const;
	
	//ファイル読み書き
  public:
#if IMPLEMENTATION_FOR_MACOSX
	//リソースファイルから読み込む方式
	void	LoadFromPrefFile( const AFileAcc& inFileAcc, const ResType& inResType = NULL );
	void	WriteToPrefFile( const AFileAcc& inFileAcc/*#1034, const ResType& inResType = NULL*/ );
#endif
	//テキストファイル（独自フォーマットのテキスト）から読み込む方式
	ABool	LoadFromPrefTextFile( const AFileAcc& inFileAcc );
	void	WriteToPrefTextFile( const AFileAcc& inFileAcc ) const;
  private:
	ABool	LoadFromPrefTextFile_GetToken( const AText& inText, AIndex& ioPos, AText& outToken ) const;
	ABool	LoadFromPrefTextFile_CheckToken( const AText& inText, AIndex& ioPos, const AUChar inChar ) const;
  private:
	AText	mUnusedPrefText;//#881

	//データ登録／取得／設定
  public:
	//Bool
	void	CreateData_Bool( ADataID inID, AConstCharPtr inFileKey, const ABool& inInitial );
	ABool	GetData_Bool( ADataID inID ) const;
	void	SetData_Bool( ADataID inID, const ABool& inData );
	void	SetDefault_Bool( ADataID inID );
	
	//Number
	void	CreateData_Number( ADataID inID, AConstCharPtr inFileKey, const ANumber& inInitial, ANumber inMin, ANumber inMax );
	ANumber	GetData_Number( ADataID inID ) const;
	void	SetData_Number( ADataID inID, const ANumber& inData );
	void	SetDefault_Number( ADataID inID );
	
	//FloatNumber
	void	CreateData_FloatNumber( ADataID inID, AConstCharPtr inFileKey, const AFloatNumber& inInitial, 
			AFloatNumber inMin, AFloatNumber inMax );
	AFloatNumber	GetData_FloatNumber( ADataID inID ) const;
	void	SetData_FloatNumber( ADataID inID, const AFloatNumber& inData );
	void	SetDefault_FloatNumber( ADataID inID );
	
	//Font
	/*win 
	void	CreateData_Font( ADataID inID, AConstCharPtr inFileKey, const AFont& inInitial );
	AFont	GetData_Font( ADataID inID ) const;
	void	SetData_Font( ADataID inID, const AFont& inData );
	*/
	//Point
	void	CreateData_Point( ADataID inID, AConstCharPtr inFileKey, ANumber inInitialX, ANumber inInitialY,
			ANumber inXMin, ANumber inXMax, ANumber inYMin, ANumber inYMax );
	void	GetData_Point( ADataID inID, APoint& outData ) const;
	void	SetData_Point( ADataID inID, const APoint& inData );
	void	SetDefault_Point( ADataID inID );
	
	//Rect
	void	CreateData_Rect( ADataID inID, AConstCharPtr inFileKey, 
			ANumber inInitialLeft, ANumber inInitialTop, ANumber inInitialRight, ANumber inInitialBottom,
			ANumber inLeftMin, ANumber inTopMin, ANumber inRightMax, ANumber inBottomMax);
	void	GetData_Rect( ADataID inID, ARect& outData ) const;
	void	SetData_Rect( ADataID inID, const ARect& inData );
	void	SetDefault_Rect( ADataID inID );
	
	//Color
	void	CreateData_Color( ADataID inID, AConstCharPtr inFileKey, const AColor& inInitial );
	void	GetData_Color( ADataID inID, AColor& outData ) const;
	void	SetData_Color( ADataID inID, const AColor& inData );
	void	SetDefault_Color( ADataID inID );
	
	//Text
	void	CreateData_Text( ADataID inID, AConstCharPtr inFileKey, const AText& inInitial,
			ABool inLimitLength = false, AItemCount inMinLength = 0, AItemCount inMaxLength = 9999 );
	void	GetData_Text( ADataID inID, AText& outData ) const;
	void	SetData_Text( ADataID inID, const AText& inData );
	void	SetDefault_Text( ADataID inID );
	void	AdjustFontName( const ADataID inID );//#688
	
	//BoolArray
	void	CreateData_BoolArray( ADataID inID, AConstCharPtr inFileKey, ABool inDefaultData );
	//void	CopyFromDataBase_BoolArray( ADataID inID, const ADataBase& inSrcDataBase, ADataID inSrcID );
	ABool	GetDefault_BoolArray( ADataID inID );
	ABool	GetData_BoolArray( ADataID inID, AIndex inIndex ) const;
	void	SetData_BoolArray( ADataID inID, AIndex inIndex, ABool inData );
	void	SetData_BoolArray( ADataID inID, const ABoolArray& inArray );
	void	Delete1_BoolArray( ADataID inID, AIndex inIndex );
	void	DeleteAll_BoolArray( ADataID inID );
	void	Insert1_BoolArray( ADataID inID, AIndex inIndex, ABool inData );
	void	Reserve_BoolArray( const ADataID inID, const AIndex inIndex, const AItemCount inCount );//#695
	void	Add_BoolArray( ADataID inID, ABool inData );
	void	AddDefault_BoolArray( ADataID inID );
	AItemCount	GetItemCount_BoolArray( ADataID inID ) const;
	ABoolArray&	GetData_BoolArrayRef( ADataID inID );
	const ABoolArray&	GetData_ConstBoolArrayRef( ADataID inID ) const;
	//NumberArray
	void	CreateData_NumberArray( ADataID inID, AConstCharPtr inFileKey, ANumber inDefaultData, ANumber inMin, ANumber inMax );//#695 , const ABool inEnableHash );
	//void	CopyFromDataBase_NumberArray( ADataID inID, const ADataBase& inSrcDataBase, ADataID inSrcID );
	ANumber	GetDefault_NumberArray( ADataID inID );
	ANumber	GetData_NumberArray( ADataID inID, AIndex inIndex ) const;
	void	SetData_NumberArray( ADataID inID, AIndex inIndex, ANumber inData );
	void	SetData_NumberArray( ADataID inID, const ANumberArray& inArray );
	void	Delete1_NumberArray( ADataID inID, AIndex inIndex );
	void	DeleteAll_NumberArray( ADataID inID );
	void	Insert1_NumberArray( ADataID inID, AIndex inIndex, ANumber inData );
	void	Reserve_NumberArray( const ADataID inID, const AIndex inIndex, const AItemCount inCount );//#695
	void	Insert_NumberArray( ADataID inID, AIndex inIndex, const ANumberArray& inNumberArray );
	void	Add_NumberArray( ADataID inID, ANumber inData );
	void	AddDefault_NumberArray( ADataID inID );
	AItemCount	GetItemCount_NumberArray( ADataID inID ) const;
	ANumberArray&	GetData_NumberArrayRef( ADataID inID );
	const ANumberArray&	GetData_ConstNumberArrayRef( ADataID inID ) const;
	AIndex	Find_NumberArray( const ADataID inID, const ANumber inData ) const;
	
	//ColorArray
	void	CreateData_ColorArray( ADataID inID, AConstCharPtr inFileKey, const AColor& inDefaultData );
	void	SetData_ColorArray( ADataID inID, const AIndex& inIndex, const AColor& inData );
	void	SetData_ColorArray( ADataID inID, const AColorArray& inArray );
	void	Insert1_ColorArray( ADataID inID, const AIndex& inIndex, const AColor& inData );
	void	Reserve_ColorArray( const ADataID inID, const AIndex inIndex, const AItemCount inCount );//#695
	void	Add_ColorArray( ADataID inID, const AColor& inData );
	void	GetData_ColorArray( ADataID inID, const AIndex& inIndex, AColor& outData ) const;
	const AColorArray&	GetData_ConstColorArrayRef( ADataID inID ) const;
	void	Delete1_ColorArray( ADataID inID, const AIndex& inIndex );
	void	DeleteAll_ColorArray( ADataID inID );
	AItemCount	GetItemCount_ColorArray( ADataID inID ) const;
	void	GetDefault_ColorArray( ADataID inID, AColor& outData );
	AColorArray&	GetData_ColorArrayRef( ADataID inID );
	
	//TextArray
	void	CreateData_TextArray( ADataID inID, AConstCharPtr inFileKey, AConstCharPtr inDefaultData,
			ABool inLimitLength = false, AItemCount inMinLength = 0, AItemCount inMaxLength = 9999 );
	//void	CopyFromDataBase_TextArray( ADataID inID, const ADataBase& inSrcDataBase, ADataID inSrcID );
	void	GetDefault_TextArray( ADataID inID, AText& outText );
	void	LimitLength_TextArray( ADataID inID, AText& ioData );
	ABool	CheckLength_TextArray( const ADataID inID, const AText& inText ) const;
	void	GetData_TextArray( ADataID inID, AIndex inIndex, AText& outData ) const;
	void	SetData_TextArray( const ADataID inID, const AIndex inIndex, const AText& inData );
	void	SetData_TextArray( const ADataID inID, const ATextArray& inArray );
	//#693void	SetData_TextArray_SwapContent( const ADataID inID, const AIndex inIndex, AText& ioData );
	void	Delete1_TextArray( ADataID inID, AIndex inIndex );
	void	DeleteAll_TextArray( ADataID inID );
	void	Insert1_TextArray( ADataID inID, AIndex inIndex, const AText& inData );
	void	Reserve_TextArray( const ADataID inID, const AIndex inIndex, const AItemCount inCount );//#695
	//#693void	Insert1_TextArray_SwapContent( ADataID inID, AIndex inIndex, AText& ioData );
	AIndex	Add_TextArray( ADataID inID, const AText& inData );
	void	AddDefault_TextArray( ADataID inID );
	AItemCount	GetItemCount_TextArray( ADataID inID ) const;
	ATextArray&	GetData_TextArrayRef( ADataID inID );
	const ATextArray&	GetData_ConstTextArrayRef( ADataID inID ) const;
	AIndex	Find_TextArray( ADataID inID, const AText& inText, const ABool& inIgnoreCase = false ) const;
	void	AddRecentlyUsed_TextArray( ADataID inID, const AText& inData, const ANumber& inLimitCount );
	
	AItemCount	GetItemCount_Array( ADataID inID ) const;
	
	//テーブル
  public:
	void	CreateTableStart();
	void	CreateTableEnd();
	
	AItemCount	GetRowCount_Table( ADataID inFirstIDInGroup );
	void	DeleteRow_Table( ADataID inFirstIDInGroup, AIndex inIndex );
	void	DeleteAllRows_Table( ADataID inFirstIDInGroup );//#695
	AIndex	AddRow_Table( ADataID inFirstIDInGroup );//#427
	void	InsertRow_Table( ADataID inFirstIDInGroup, const AIndex inRowIndex );//#695
	void	ReserveRow_Table( ADataID inFirstIDInGroup, const AIndex inRowIndex, const AItemCount inCount );//#695
	void	InsertRowFromOtherDB_Table( const ADataID inFirstIDInGroup, const AIndex inIndex,
			const ADataBase& inOtherDB, const AIndex inIndexInOtherDB );//#427
	void	MoveRow_Table( ADataID inFirstIDInGroup, const AIndex& inDeleteItemIndex, const AIndex& inNewItemIndex  );
	void	CorrectTable_MakeColumnUnique( const ADataID inFirstIDInGroup, const ADataID inUniqueColumn );//B0000
	void	CorrectTable();
  private:
	AIndex	FindTable( ADataID inFirstIDInGroup );
	void	SetupAsColumn_Table( ADataID inID );
	
  protected:
	//Bool
	AArray<ABool>		mBool_Data;
	AArray<ABool>		mBool_Default;
	//Number
	AArray<ANumber>		mNumber_Data;
	AArray<ANumber>		mNumber_Min;
	AArray<ANumber>		mNumber_Max;
	AArray<ANumber>		mNumber_Default;
	mutable AThreadMutex	mNumber_Data_Mutex;
	//FloatNumber
	AArray<AFloatNumber>	mFloatNumber_Data;
	AArray<AFloatNumber>	mFloatNumber_Min;
	AArray<AFloatNumber>	mFloatNumber_Max;
	AArray<AFloatNumber>	mFloatNumber_Default;
	mutable AThreadMutex	mFloatNumber_Data_Mutex;
	//Point
	AArray<APoint>		mPoint_Data;
	AArray<ANumber>		mPoint_XMin;
	AArray<ANumber>		mPoint_XMax;
	AArray<ANumber>		mPoint_YMax;
	AArray<ANumber>		mPoint_YMin;
	AArray<APoint>		mPoint_Default;
	mutable AThreadMutex	mPoint_Data_Mutex;
	//Rect
	AArray<ARect>		mRect_Data;
	AArray<ANumber>		mRect_LeftMin;
	AArray<ANumber>		mRect_TopMin;
	AArray<ANumber>		mRect_RightMax;
	AArray<ANumber>		mRect_BottomMax;
	AArray<ARect>		mRect_Default;
	mutable AThreadMutex	mRect_Data_Mutex;
	//Color
	AArray<AColor>		mColor_Data;
	AArray<AColor>		mColor_Default;
	mutable AThreadMutex	mColor_Data_Mutex;
	//Text
	ATextArray			mText_Data;
	AArray<ABool>		mText_LimitLength;
	AArray<AItemCount>	mText_MinLength;
	AArray<AItemCount>	mText_MaxLength;
	ATextArray			mText_Default;
	mutable AThreadMutex	mText_Data_Mutex;
	//BoolArray
	AObjectArray<ABoolArray>	mBoolArray_Data;
	ABoolArray			mBoolArray_Default;
	mutable AThreadMutex	mBoolArray_Data_Mutex;
	//NumberArray
	AObjectArray<ANumberArray>	mNumberArray_Data;//#695
	ANumberArray		mNumberArray_Default;
	ANumberArray		mNumberArray_Min;
	ANumberArray		mNumberArray_Max;
	mutable AThreadMutex	mNumberArray_Data_Mutex;
	//TextArray
	AObjectArray<ATextArray>	mTextArray_Data;
	ATextArray			mTextArray_Default;
	AArray<ABool>		mTextArray_LimitLength;
	AArray<AItemCount>	mTextArray_MinLength;
	AArray<AItemCount>	mTextArray_MaxLength;
	mutable AThreadMutex	mTextArray_Data_Mutex;
	//ColorArray
	AObjectArray<AColorArray>	mColorArray_Data;
	AColorArray			mColorArray_Default;
	AColorArray			mDummyColorArray;
	mutable AThreadMutex	mColorArray_Data_Mutex;
	
	//
	AObjectArray<AHashNumberArray>	mTableIDArray;
	AIndex						mCurrentRegisteringTableIndex;
	
	//全体管理
  public:
	ABool	IsDataExist( const ADataID inID ) const;
	ABool	IsStillDefault( ADataID inID ) const;
	void	CreateDataFromOtherDB( ADataID inID, const ADataBase& inOtherDB, ADataID inIDInOtherDB );
	void	CopyDataFromOtherDB( ADataID inID, const ADataBase& inOtherDB, ADataID inIDInOtherDB );
	void	CopyDataFromOtherDB( const ADataBase& inOtherDB );
	void	SetAllDataToDefault();
  protected:
	void	Register( ADataID inID, AConstCharPtr inFileKey, const ADataType& inDataType, const AIndex& inIndexInEach );
	void	DataIsSet( ADataID inID );
	void	DataIsSetToDefault( ADataID inID );
  private:
	AIndex	GetIndexInEach( const ADataID inID, const ADataType inType ) const;//#427
	AHashArray<ADataID>		mAll_ID;
	AArray<ADataType>	mAll_DataType;
	AArray<AIndex>		mAll_IndexInEach;
	AArray<ABool>		mAll_IsStillDefault;
	AHashTextArray		mAll_FileKey;//#344 ATextArray->AHashTextArray
	
  private:
#if IMPLEMENTATION_FOR_MACOSX
	APrefFileImp	mPrefFileImp;
#endif
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "ADataBase"; }
	
};

//ダミーDBの取得
extern ADataBase&	GetDummyDB();


