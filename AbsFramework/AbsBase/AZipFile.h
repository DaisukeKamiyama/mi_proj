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

AZipFile

*/

#pragma once

#pragma mark ===========================
#pragma mark [クラス]AZipFile

/**
zipファイルクラス
*/
class AZipFile
{
	
	//コンストラクタ／デストラクタ
  public:
	AZipFile( const AFileAcc& inZipFile );
	~AZipFile();
	
	//
  public:
	ABool				Zip( const AFileAcc& inFolder );//#427
	ABool				Unzip( const AFileAcc& inDstFolder );

	//
private:
	AFileAcc						mFile;
};

