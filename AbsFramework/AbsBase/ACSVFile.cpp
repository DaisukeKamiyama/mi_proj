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

ACSVFile
#791

*/
/*
CSVファイルデータ読み込みクラス
RFC4180準拠
*/

#include "stdafx.h"

#include "ABase.h"
#include "ACSVFile.h"

#pragma mark ===========================
#pragma mark [クラス]ACSVFile
#pragma mark ===========================

#pragma mark ---コンストラクタ・デストラクタ

/**
コンストラクタ
*/
ACSVFile::ACSVFile( const AText& inCSVText, const ATextEncoding inEncoding, const AItemCount inMinColumnCount )
{
	//ファイル読み込み
	AText	text;
	text.SetText(inCSVText);
	
	//テキストエンコーディングをUTF8,CRに変更
	ABool	fallbackUsed = false;
	text.ConvertToUTF8(inEncoding,true,fallbackUsed,true);
	text.ConvertReturnCodeToCR();
	
	//最小数分のカラムを追加
	for( AIndex i = 0; i < inMinColumnCount; i++ )
	{
		mDataArray.AddNewObject();
	}
	//各行(row)ごとのループ
	AText	cell;
	AItemCount	len = text.GetItemCount();
	for( AIndex pos = 0; pos < len; )
	{
		//各カラムごとのループ
		AIndex	columnIndex = 0;
		ABool	rowEnd = false;
		while( rowEnd == false )
		{
			//1セル分読み込み
			rowEnd = ParseCell(text,pos,cell);
			//mDataArrayに対応するカラムが存在していなければ、mDataArrayの項目を追加する
			if( columnIndex >= mDataArray.GetItemCount() )
			{
				//mDataArrayの項目（カラム）を追加
				mDataArray.AddNewObject();
				//カラム0の行数-1個分、空のテキストを追加（残り1個はこの後でセル追加する）
				AItemCount	rowCount = mDataArray.GetObjectConst(0).GetItemCount();
				for( AIndex i = 0; i < rowCount-1; i++ )
				{
					mDataArray.GetObject(columnIndex).Add(GetEmptyText());
				}
			}
			//mDataArrayに値を設定（セル追加）
			mDataArray.GetObject(columnIndex).Add(cell);
			//次のカラムへ
			columnIndex++;
		}
		//テキスト内のセルが不足していた場合、mDataArrayの残りカラムについて、セルを追加
		for( ; columnIndex < mDataArray.GetItemCount(); columnIndex++ )
		{
			mDataArray.GetObject(columnIndex).Add(GetEmptyText());
		}
	}
	/*
	//最初の行をparseし、cellの数だけmDataArrayの項目を追加する
	for( AIndex pos = 0; pos < len; )
	{
		//mDataArrayの項目を追加
		mDataArray.AddNewObject();
		//1つのcell分を読み込み。行が終了したら、break
		AText	cell;
		if( ParseCell(text,pos,cell) == true )
		{
			//行が終了したらbreak
			break;
		}
	}
	//各行を読み込み、mDataArrayへ格納
	for( AIndex pos = 0; pos < len;  )
	{
		//cellの数だけループ
		AItemCount	itemCount = mDataArray.GetItemCount();
		ABool	lineEnded = false;
		for( AIndex index = 0; index < itemCount; index++ )
		{
			AText	cell;
			//行が終了していなければ、1つのcell分を読み込み。
			//カラム数不足の場合、先に行が終了する可能性があるが、その場合は、空文字列を追加することにする
			if( lineEnded == false )
			{
				lineEnded = ParseCell(text,pos,cell);
			}
			else
			{
				//カラム数不足発生
				//_ACError("",NULL);
			}
			//mDataArrayに値を設定
			mDataArray.GetObject(index).Add(cell);
		}
		//カラム数が超過していたら、行終了まで読み進める
		if( lineEnded == false )
		{
			//カラム数超過発生
			//_ACError("",NULL);
			//行終了まで読み進め
			while(lineEnded==false)
			{
				AText	cell;
				lineEnded = ParseCell(text,pos,cell);
			}
		}
	}
	*/
}

/**
1つ分のcellを読み込み
*/
ABool	ACSVFile::ParseCell( const AText& inText, AIndex& ioPos, AText& outCell ) const
{
	//返り値初期化
	outCell.DeleteAll();
	//ダブルクオーテーションでquotedされたモードかどうかのフラグ
	ABool	quoted = false;
	//行内ループ
	AItemCount	len = inText.GetItemCount();
	for( ; ioPos < len; ioPos++ )
	{
		//1文字取得
		AUChar	ch = inText.Get(ioPos);
		switch(ch)
		{
			//ダブルクオーテーションの場合
		  case '\"':
			{
				if( quoted == false )
				{
					//quoted状態でない場合
					
					//quoted状態へ以降
					quoted = true;
				}
				else
				{
					//quoted状態の場合
					
					//""なら"に変換
					if( ioPos+1 < len )
					{
						if( inText.Get(ioPos+1) == '\"' )
						{
							outCell.Add('\"');
							ioPos++;
							continue;
						}
					}
					//"だけならquoted状態解除
					quoted = false;
				}
				break;
			}
			//カンマ
		  case ',':
			{
				if( quoted == true )
				{
					//quoted状態の場合
					
					//カンマを追加
					outCell.Add(',');
				}
				else
				{
					//quoted状態でない場合
					
					//cell終了
					ioPos++;
					return false;
				}
				break;
			}
			//改行
		  case kUChar_LineEnd:
			{
				if( quoted == true )
				{
					//quoted状態の場合
					
					//改行を追加
					outCell.Add(kUChar_LineEnd);
				}
				else
				{
					//quoted状態でない場合
					
					//行終了
					ioPos++;
					return true;
				}
				break;
			}
			//それ以外の文字
		  default:
			{
				//文字追加
				outCell.Add(ch);
				break;
			}
		}
	}
	return true;
}

/**
デストラクタ
*/
ACSVFile::~ACSVFile()
{
}

#pragma mark ===========================

#pragma mark ---データ取得

/**
指定indexのカラムを取得
*/
void	ACSVFile::GetColumn( const AIndex inColumnIndex, ATextArray& outColumnData )
{
	//カラム存在しない場合は空を返す
	if( inColumnIndex >= mDataArray.GetItemCount() )
	{
		outColumnData.DeleteAll();
		return;
	}
	//指定カラムデータ取得
	outColumnData.SetFromTextArray(mDataArray.GetObjectConst(inColumnIndex));
}

/**
指定indexのカラムを取得
*/
void	ACSVFile::GetColumn( const AIndex inColumnIndex, ANumberArray& outColumnData )
{
	//カラム存在しない場合は空を返す
	if( inColumnIndex >= mDataArray.GetItemCount() )
	{
		outColumnData.DeleteAll();
		return;
	}
	//指定カラムデータ取得
	outColumnData.DeleteAll();
	const ATextArray&	textArray = mDataArray.GetObjectConst(inColumnIndex);
	for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
	{
		ANumber	num = textArray.GetTextConst(i).GetNumber();
		outColumnData.Add(num);
	}
}





