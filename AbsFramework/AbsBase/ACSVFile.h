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

#pragma once

#include "ATextArray.h"

/**
CSVファイルデータ読み込みクラス
RFC4180準拠
*/
class ACSVFile
{
	//コンストラクタ・デストラクタ
  public:
	ACSVFile( const AText& inCSVText, const ATextEncoding inEncoding, const AItemCount inMinColumnCount = 1 );
	~ACSVFile();
  private:
	ABool			ParseCell( const AText& inLine, AIndex& ioPos, AText& outCell ) const;
	
	//データ取得
  public:
	void			GetColumn( const AIndex inColumnIndex, ATextArray& outColumnData );
	void			GetColumn( const AIndex inColumnIndex, ANumberArray& outColumnData );
	
	//データ
  private:
	/** @note カラム数だけATextArrayをもつ */
	AObjectArray<ATextArray>	mDataArray;
};




