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

AThread_SyntaxRecognizer
#698

*/

#include "stdafx.h"

#include "AThread_SyntaxRecognizer.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AThread_SyntaxRecognizer
#pragma mark ===========================
//マルチファイル検索実行

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AThread_SyntaxRecognizer::AThread_SyntaxRecognizer( AObjectArrayItem* inParent ) : AThread(inParent), mTextInfo(this),
		mTextDocumentID(kObjectID_Invalid), mRecognizerJudgedAsEnd(false), mRecognizeEndLineIndex(kIndex_Invalid),
		mModeIndex(kIndex_Invalid)
{
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

/**
スレッドメインルーチン
*/
void	AThread_SyntaxRecognizer::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_SyntaxRecognizer::NVIDO_ThreadMain started.",this);
	try
	{
		while( NVM_IsAborted() == false )//NVI_AbortThread()がコールされていたらスレッド終了
		{
			//pause解除まで待つ
			NVM_WaitForUnpaused();
			//Abortされていたら終了（文法認識完了時、メインスレッドからNVI_AbortThread()+Unpause()により終了される。）
			if( NVM_IsAborted() == true )
			{
				break;
			}
			//文法認識実行
			AArray<AUniqID>	deletedIdentifier;
			AArray<AUniqID>	addedGlobalIdentifier;
			AArray<AIndex>	addedGlobalIdentifierLineIndex;
			ABool	importChanged = false;
			{
				AStMutexLocker	locker(mTextInfoMutex);
				mRecognizeEndLineIndex = mTextInfo.ExecuteRecognizeSyntax(mText,kIndex_Invalid,
																		  deletedIdentifier,addedGlobalIdentifier,addedGlobalIdentifierLineIndex,importChanged,mAborted);
				if( NVM_IsAborted() == true )   break;
				//ExecuteRecognizeSyntax()により認識完了と判断されたかどうかをmRecognizerJudgedAsEndに格納する
				//（認識完了と判断されたらその行で認識終了となっている）
				mRecognizerJudgedAsEnd = (mRecognizeEndLineIndex < mTextInfo.GetLineCount());
			}
			//fprintf(stderr,"SyntaxRecognizerThreadPause:%d ",mRecognizeEndLineIndex);
			//スレッドをpause状態にする
			NVI_Pause();
			//メインスレッドへ今回依頼分終了を通知
			if( NVM_IsAborted() == false )
			{
				AObjectIDArray	objectIDArray;
				objectIDArray.Add(mTextDocumentID);
				ABase::PostToMainInternalEventQueue(kInternalEvent_SyntaxRecognizerPaused,GetObjectID(),0,GetEmptyText(),objectIDArray);
			}
			//スレッド終了は、メインスレッドで判断する。
			//文法認識完了時（FindFirstUnrecognizedLine()により未認識がないと判断されたとき）は、
			//NVI_AbortThread()+Unpause()によりスレッド終了する。
		}
	}
	catch(...)
	{
		_ACError("AThread_SyntaxRecognizer::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	//text infoのreg expオブジェクトをpurge
	{
		AStMutexLocker	locker(mTextInfoMutex);
		mTextInfo.PurgeRegExpObject();
	}
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_SyntaxRecognizer::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---スレッド設定・起動

/**
初期設定
*/
void	AThread_SyntaxRecognizer::SPI_Init( const ADocumentID inTextDocumentID )
{
	mTextDocumentID = inTextDocumentID;
}

/**
文法認識コンテキストを設定（メインスレッドからこのスレッドへ情報コピー）
*/
void	AThread_SyntaxRecognizer::SPI_SetContext( const AModeIndex inModeIndex, const ATextInfo& inSrcTextInfo, const AIndex inStartLineIndex )
{
	AStMutexLocker	locker(mTextInfoMutex);
	//モード設定
	if( mModeIndex != inModeIndex )
	{
		mModeIndex = inModeIndex;
		//TextInfoにモード設定
		mTextInfo.SetMode(mModeIndex);
	}
	//コンテキストを取得
	mTextInfo.CopyRecognizeContext(inSrcTextInfo,inStartLineIndex);
}

/**
テキスト設定（メインスレッドからこのスレッドへ情報コピー）
*/
void	AThread_SyntaxRecognizer::SPI_SetText( const AText& inText, const AIndex inIndex, const AItemCount inCount )
{
	inText.GetText(inIndex,inCount,mText);
}

/**
TextInfoの対象行の情報を設定（メインスレッドからこのスレッドへ情報コピー）
*/
void	AThread_SyntaxRecognizer::SPI_SetTextInfo( const ATextInfo& inSrcTextInfo, 
		const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	AStMutexLocker	locker(mTextInfoMutex);
	mTextInfo.CopyLineInfoToSyntaxRecognizer(inSrcTextInfo,inStartLineIndex,inEndLineIndex);
}

/**
text infoの正規表現オブジェクトをpurge
*/
void	AThread_SyntaxRecognizer::SPI_PurgeTextinfoRegExp()
{
	//この関数はメインスレッドからコールされる。（そのため、mTextInfoMutexの排他が必要）
	AStMutexLocker	locker(mTextInfoMutex);
	mTextInfo.PurgeRegExpObject();
}

#pragma mark ===========================

#pragma mark ---スレッド終了後処理

/**
スレッド内データ削除
*/
void	AThread_SyntaxRecognizer::ClearData()
{
	//文法認識完了時にデータを削除（メモリ節約）
	//SPI_Init()で設定したデータは削除しない
	mText.DeleteAll();
	{
		AStMutexLocker	locker(mTextInfoMutex);
		mTextInfo.DeleteLineInfoAll();
	}
}

