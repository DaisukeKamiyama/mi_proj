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

ACurlThread
#427

*/

#pragma once

#include "../../AbsFramework/Frame.h"

enum ECurlRequestType
{
	kCurlRequestType_None = 0,
	kCurlRequestType_HTTP = 1
};

#pragma mark ===========================
#pragma mark [クラス]ACurlThread
//マルチファイル検索スレッドオブジェクト
class ACurlThread : public AThread
{
	//コンストラクタ
  public:
	ACurlThread( AObjectArrayItem* inParent );
	
	//スレッドメイン
  private:
	void			NVIDO_ThreadMain();
	static size_t	ReadToMemory( void *ptr, size_t size, size_t nmemb, void *data );
	
	//スレッド起動
  public:
	void			SPNVI_Run( const ECurlRequestType inRequestType, const AText& inURL, 
					const ACurlTransactionType inTransactionType, const ATextArray& inInfoTextArray );
	
	//結果取得
  public:
	void			GetResult( AText& outText, ECurlRequestType& outRequestType, AText& outURL, 
					ACurlTransactionType& outTransactionType, ATextArray& outInfoTextArray );
	
	//データ
  private:
	AText						mResultText;
	ECurlRequestType			mRequestType;
	AText						mURL;
	ACurlTransactionType		mTransactionType;
	ATextArray					mInfoTextArray;
};


#pragma mark ===========================
#pragma mark [クラス]ACurlThreadFactory
//
class ACurlThreadFactory : public AThreadFactory
{
  public:
	ACurlThreadFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	ACurlThread*	Create() 
	{
		return new ACurlThread(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};

