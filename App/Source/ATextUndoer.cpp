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

ATextUndoer

*/

#include "stdafx.h"

#include "ATextUndoer.h"
#include "ADocument_Text.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]ATextUndoer
#pragma mark ===========================
//Undo処理クラス
#pragma mark ---コンストラクタ／デストラクタ
ATextUndoer::ATextUndoer( ADocument_Text& inTextDocument ) : mTextDocument(inTextDocument)
,mDeletedTextArray(NULL)//#693,true)//#417
{
	mUndoMode = false;
	mUndoPosition = 0;
	//結合可能フラグ（内部フラグ）
	mCanCat = false;
	//結合禁止フラグ（外部からの指示でセットされる。内部のフラグ状態に関わらず、次回の結合を禁止する。ただし、次回のUndo/Redoでフラグ解除される。）
	//#210 未使用のためコメントアウトmNoContinue = false;//B0090 文字入力し、その最後の何文字かを文字選択して、文字入力（置換）すると、アンドゥ時に最初の文字入力の前の状態に戻る問題対応
}

#pragma mark ===========================

#pragma mark ---記録

//アクションタグを記録
void	ATextUndoer::RecordActionTag( const AUndoTag inAction )
{
	//アクションタグしか無いものを削除
	DeleteDanglingActionTag();
	
	//アクションタグ追加
	if( IsActionTag(inAction) == true )
	{
		AddUndoRecord(inAction,0,0,kObjectID_Invalid);//#216
	}
}

//テキスト挿入を記録
void	ATextUndoer::RecordInsertText( const ATextIndex inInsertTextIndex, const AItemCount inLength )
{
	//Undo実行中なら何もしない
	if( mUndoMode == true )   return;
	
	//#267
	ABool	inhibitConcat = false;
	AIndex	currentLastEditRecordPos = GetCurrentLastEditRecordPosition();
	if( currentLastEditRecordPos != kIndex_Invalid )
	{
		inhibitConcat = mUndoRecord_InhibitConcat.Get(currentLastEditRecordPos);
	}
	
	//挿入レコード追加
	AddUndoRecord(undoTag_InsertText,inInsertTextIndex,inInsertTextIndex+inLength,kObjectID_Invalid);//#216
	
	//#267
	if( inhibitConcat == true )   return;
	
	//結合可能フラグがOffなら、フラグOnにする。ただし、今回は結合しない。
	if( mCanCat == false )
	{
		mCanCat = true;
	}
	//結合禁止フラグOffかつ、Tagに3つ以上存在し、現在の処理がTypingなら結合判定を行う
	else if( /*#210 未使用のためコメントアウト(mNoContinue == false) &&*/
				(mUndoRecord_Tag.GetItemCount() >= 3) && (GetCurrentAction() == undoTag_Typing) )
	{
		//前回のUndoRecordがInsertTextで、終了文字位置が今回の開始文字位置と同じなら、まとめて１つのRecordにする
		
		/*
		UndoRecordの構造
		[前回のActionTag]
		[前回のInsertText]<-GetUndoRecordItemCount()-3
		[今回のActionTag]<-GetUndoRecordItemCount()-2
		[今回のInsertText]
		*/
		//prevActionに前回のActionTagの内容を取得する
		AUndoTag	prevAction = undoTag_NOP;
		for( AIndex i = GetUndoRecordItemCount()-3; i >= 0; i-- )
		{
			prevAction = mUndoRecord_Tag.Get(i);
			if( prevAction >= undoTag_Typing )   break;
		}
		//前回のActionTagがTypingなら結合判定続行
		if( prevAction == undoTag_Typing )
		{
			//このモジュールで追加したレコードより1つ前から遡ってInsertText/DeleteTextレコードの場所を探す
			//これが結合候補レコードとなる。インデックスがprevRecordIndexに格納される。
			//ループはActionTagレコードをとばすため。
			AIndex	prevRecordIndex = GetUndoRecordItemCount()-2;
			for( ; prevRecordIndex >= 0; prevRecordIndex-- )
			{
				AUndoTag	tag = mUndoRecord_Tag.Get(prevRecordIndex);
				if( tag == undoTag_InsertText || tag == undoTag_DeleteText )   break;
			}
			//結合候補レコードのTagがInsertTextで、かつ、その終了位置が、今回追加したレコードの開始位置に等しいなら、１レコードに結合する
			if( (mUndoRecord_Tag.Get(prevRecordIndex) == undoTag_InsertText) && 
						(/*#216mUndoRecord_Par*/mUndoRecord_Epos.Get(prevRecordIndex) == inInsertTextIndex) )
			{
				//結合候補レコード以降を削除
				DeleteUndoRecord(prevRecordIndex+1,GetUndoRecordItemCount()-(prevRecordIndex+1));
				//結合対象レコードの終了位置を更新する
				/*#216mUndoRecord_Par*/mUndoRecord_Epos.Set(GetUndoRecordItemCount()-1,inInsertTextIndex+inLength);
				//Undo位置を更新
				mUndoPosition = GetUndoRecordItemCount();
			}
		}
	}
	//結合禁止フラグOff
	//#210 未使用のためコメントアウト mNoContinue = false;//B0090
}

//テキスト削除を記録
void	ATextUndoer::RecordDeleteText( const ATextIndex inStart, const ATextIndex inEnd )
{
	//Undo実行中なら何もしない
	if( mUndoMode == true )   return;
	
	//#267
	ABool	inhibitConcat = false;
	AIndex	currentLastEditRecordPos = GetCurrentLastEditRecordPosition();
	if( currentLastEditRecordPos != kIndex_Invalid )
	{
		inhibitConcat = mUndoRecord_InhibitConcat.Get(currentLastEditRecordPos);
	}
	
	//削除予定テキストを取得し、mDeletedTextArrayへ格納
	AIndex	deletedTextIndex = mDeletedTextArray.AddNewObject();
	GetTextDocument().SPI_GetText(inStart,inEnd,mDeletedTextArray.GetObject(deletedTextIndex));
	AObjectID	deletedTextObjectID = mDeletedTextArray.GetObject(deletedTextIndex).GetObjectID();
	
	//削除レコード追加
	AddUndoRecord(undoTag_DeleteText,inStart,inStart,deletedTextObjectID);//#216
	
	//#267
	if( inhibitConcat == true )   return;
	
	if( false )
	{
		//
		mCanCat = true;
		//結合禁止フラグOff
		//#210 未使用のためコメントアウト mNoContinue = false;//B0090
	}
	else
	{
		//結合可能フラグがOffなら、フラグOnにする。ただし、今回は結合しない。
		if( mCanCat == false )
		{
			mCanCat = true;
		}
		//#210
		//結合禁止フラグOffかつ、Tagに3つ以上存在し、現在の処理がBSKeyなら結合判定を行う
		else if( /*#210 未使用のためコメントアウト (mNoContinue == false) &&*/
					(mUndoRecord_Tag.GetItemCount() >= 3) && 
					((GetCurrentAction() == undoTag_BSKey)/*||GetCurrentAction() == undoTag_Typing*/) )
		{
			/*
			UndoRecordの構造
			[前回のActionTag]
			[前回のDeleteText]<-GetUndoRecordItemCount()-3
			[今回のActionTag]<-GetUndoRecordItemCount()-2
			[今回のDeleteText]
			*/
			//prevActionに前回のActionTagの内容を取得する
			AUndoTag	prevAction = undoTag_NOP;
			for( AIndex	i = GetUndoRecordItemCount()-3; i >= 0; i-- )
			{
				prevAction = mUndoRecord_Tag.Get(i);
				if( prevAction >= undoTag_Typing )   break;
			}
			//前回のActionTagが今回のActionTagと同じなら結合判定続行
			if( prevAction == GetCurrentAction() )
			{
				//このモジュールで追加したレコードより1つ前から遡ってInsertText/DeleteTextレコードの場所を探す
				//これが結合候補レコードとなる。インデックスがprevRecordIndexに格納される。
				//ループはActionTagレコードをとばすため。
				AIndex	prevRecordIndex = GetUndoRecordItemCount()-2;
				for( ; prevRecordIndex >= 0; prevRecordIndex-- )
				{
					AUndoTag	tag = mUndoRecord_Tag.Get(prevRecordIndex);
					if( tag == undoTag_InsertText || tag == undoTag_DeleteText )   break;
				}
				//ActionTagによって処理分岐
				switch(prevAction) 
				{
					/*
					#210対応時、旧ソースから移植したが、ここの部分は不要に思えるのでコメントアウトした。
					よっｔ、テストもされていない。
					「文字を選択して、（上書きで）文字入力した場合」はユーザーの操作として”今までとは別の操作”である可能性が高いので、
					結合しないほうがベターだと思われる。
					また、ここを追加すると、B0090の問題対策として、文字選択時に外部からmNoContinueをtrueにする処理が必要。複雑になるわりにメリットがない。
					mNoContinueは、今まで常にfalseで使っていなかったので、今回、削除する。
					//文字を選択して、（上書きで）文字入力した場合
				  case undoTag_Typing:
					{
						if( (mUndoRecord_Tag.Get(prevRecordIndex) == undoTag_InsertText) &&
									(mUndoRecord_Epos.Get(prevRecordIndex) == inEnd) && 
									(mUndoRecord_Spos.Get(prevRecordIndex) <= inStart) ) 
						{
							//今回追加したレコードを削除
							DeleteUndoRecord(prevRecordIndex+1,GetUndoRecordItemCount()-(prevRecordIndex+1));
							//結合対象レコードの終了位置を更新する
							mUndoRecord_Epos.Set(GetUndoRecordItemCount()-1,
									mUndoRecord_Epos.Get(GetUndoRecordItemCount()-1)-(inEnd-inStart));
							//Undo位置を更新
							mUndoPosition = GetUndoRecordItemCount();
						}
						break;
					}
					*/
					//BSKeyを連続した場合
				  case undoTag_BSKey:
					{
						//結合候補レコードのTagがDeleteTextで、かつ、その開始位置が、
						//今回追加したレコードの終了位置に等しいなら、１レコードに結合する
						/*
						たとえば、"abc"の右端からBSKey2回押した場合、
						[BSKey]
						[2,"c"]←結合候補レコード（開始位置:2＝今回削除の終了位置）
						[BSKey]
						[1,"b"]
						deletedText:"b"
						処理１："b"を"c"の前に挿入
						処理２：開始位置:2を1に変更（"b"の長さ分だけ前に変更）
						（ちなみに、ActionTagを消すだけでも結合できた気もするが、この処理で実績があるため、こちらをそのまま使うこととした。）
						*/
						if( (mUndoRecord_Tag.Get(prevRecordIndex) == undoTag_DeleteText) &&
									(mUndoRecord_Spos.Get(prevRecordIndex) == inEnd) )
						{
							//結合候補レコード以降を削除
							DeleteUndoRecord(prevRecordIndex+1,GetUndoRecordItemCount()-(prevRecordIndex+1));
							//結合対象レコードの開始位置と、DeletedTextを更新する
							//今回削除テキスト取得
							AText	deletedText;
							GetTextDocument().SPI_GetText(inStart,inEnd,deletedText);
							//結合対象のDeletedTextのObjectID取得
							AObjectID	prevRecordDeletedTextObjectID = mUndoRecord_DeletedTextObjectID.Get(prevRecordIndex);
							//DeletedTextを結合
							mDeletedTextArray.GetObjectByID(prevRecordDeletedTextObjectID).InsertText(0,deletedText);
							//開始位置を更新
							mUndoRecord_Spos.Set(GetUndoRecordItemCount()-1,
									mUndoRecord_Spos.Get(GetUndoRecordItemCount()-1)-(inEnd-inStart));
							//Undo位置を更新
							mUndoPosition = GetUndoRecordItemCount();
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
		}
	}
}

#pragma mark ===========================

#pragma mark ---Undo/Redo実行

/**
Undo実行
@return trueの場合は、アクションが連続して存在しているため、redrawを未実行なので、コール側でredraw必要
*/
ABool	ATextUndoer::Undo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	//#219
	outSelectTextIndex = kIndex_Invalid;
	outSelectTextLength = 0;
	
	//アクションタグしか無いものを削除
	DeleteDanglingActionTag();
	
	//Undo不可ならリターン
	if( mUndoPosition <= 0 )   return false;
	
	//#846
	//連続しているアクション数を取得
	AItemCount	totalActionCount = 0;
	for( AIndex pos = mUndoPosition-1; pos > 0; pos-- )
	{
		if( IsActionTag(mUndoRecord_Tag.Get(pos)) == true )   break;
		totalActionCount++;
	}
	//アクション数が100より多い場合、redrawは実行せず、コール元でまとめて行う。
	ABool	redrawOnceMode = (totalActionCount>100);
	//モーダルセッション（キャンセル不可）
	AStEditProgressModalSession	modalSession(kEditProgressType_Undo,false,false,true);
	try
	{
		//
		AIndex	startUndoPosition = mUndoPosition-1;
		//前のアクションタグまでをReverse実行
		mUndoMode = true;
		for( mUndoPosition--; mUndoPosition > 0; mUndoPosition-- )
		{
			//#1502
			AStAutoReleasePool	pool;
			//#846
			//プログレス表示（キャンセルは不可）
			GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Undo,0,true,startUndoPosition-mUndoPosition,totalActionCount);
			
			//アクションタグの位置まで来たらループ終了
			if( IsActionTag(mUndoRecord_Tag.Get(mUndoPosition)) == true )   break;
			
			//mUndoPosition位置の挿入／削除レコードを、逆転して実行し、レコードを逆転して保存
			ReverseTextAction(mUndoPosition,outSelectTextIndex,outSelectTextLength,redrawOnceMode);
		}
		mUndoMode = false;
	}
	catch(...)
	{
		_ACError("",NULL);
		mUndoMode = false;
	}
	//
	return redrawOnceMode;
}

//Redo実行
ABool	ATextUndoer::Redo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	//#219
	outSelectTextIndex = kIndex_Invalid;
	outSelectTextLength = 0;
	
	//Redo不可ならリターン
	if( mUndoPosition >= GetUndoRecordItemCount() )   return false;
	
	//#846
	//連続しているアクション数を取得
	AItemCount	totalActionCount = 0;
	for( AIndex pos = mUndoPosition+1; pos < GetUndoRecordItemCount(); pos++ )
	{
		if( IsActionTag(mUndoRecord_Tag.Get(pos)) == true )   break;
		totalActionCount++;
	}
	//アクション数が100より多い場合、redrawは実行せず、コール元でまとめて行う。
	ABool	redrawOnceMode = (totalActionCount>100);
	//モーダルセッション（キャンセル不可）
	AStEditProgressModalSession	modalSession(kEditProgressType_Redo,false,false,true);
	try
	{
		//
		AIndex	startUndoPosition = mUndoPosition+1;
		//前のアクションタグまでをReverse実行
		mUndoMode = true;
		for( mUndoPosition++; mUndoPosition < GetUndoRecordItemCount(); mUndoPosition++ )
		{
			//#1502
			AStAutoReleasePool	pool;
			//#846
			//プログレス表示（キャンセルは不可）
			GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Redo,0,true,mUndoPosition-startUndoPosition,totalActionCount);
			
			//アクションタグの位置まで来たらループ終了
			if( IsActionTag(mUndoRecord_Tag.Get(mUndoPosition)) == true )   break;
			
			//mUndoPosition位置の挿入／削除レコードを、逆転して実行し、レコードを逆転して保存
			ReverseTextAction(mUndoPosition,outSelectTextIndex,outSelectTextLength,redrawOnceMode);
		}
		mUndoMode = false;
	}
	catch(...)
	{
		_ACError("",NULL);
		mUndoMode = false;
	}
	//
	return redrawOnceMode;
}

//指定位置の挿入／削除レコードを、逆転して実行し、レコードを逆転して保存
void	ATextUndoer::ReverseTextAction( const AIndex inIndex, AIndex& ioSelectTextIndex, AItemCount& ioSelectTextLength,
		const ABool inRedrawOnceMode )//#219 
{
	//#219
	AIndex	resultSpos = ioSelectTextIndex;
	AIndex	resultEpos = ioSelectTextIndex+ioSelectTextLength;
	//
	AUndoTag	tag = mUndoRecord_Tag.Get(inIndex);
	if( tag == undoTag_InsertText )
	{
		AIndex	spos = mUndoRecord_Spos.Get(inIndex);
		AIndex	epos = /*#216 mUndoRecord_Par*/mUndoRecord_Epos.Get(inIndex);
		
		//削除予定テキストを取得し、mDeletedTextArrayへ格納
		AIndex	deletedTextIndex = mDeletedTextArray.AddNewObject();
		GetTextDocument().SPI_GetText(spos,epos,mDeletedTextArray.GetObject(deletedTextIndex));
		AObjectID	deletedTextObjectID = mDeletedTextArray.GetObject(deletedTextIndex).GetObjectID();
		
		//
		mUndoRecord_Tag.Set(inIndex,undoTag_DeleteText);
		/*#216 mUndoRecord_Par*/mUndoRecord_DeletedTextObjectID.Set(inIndex,deletedTextObjectID);
		
		//Reverse実行した場合、その後、その処理に続けて同種処理を行っても結合しない
		mCanCat = false;//#210 ==バグ修正
		
		GetTextDocument().SPI_DeleteText(spos,epos,inRedrawOnceMode,inRedrawOnceMode);
		//#219 outSelectTextIndex = spos;
		//#219 outSelectTextLength = 0;
		
		//#219 もし要望があるなら設定で動作をかえる
		if( false )
		{
			//Undo/Redo後に挿入文字列を選択する設定の場合
			//（十分にテストしていません）
			
			//1.最後を削除した場合
			// 従来が{resultSpos,resultEpos}で、resultEpos==eposであり、sposがreultSposよりも後の場合、
			// 選択範囲をせばめる（終了位置を更新する）
			if( resultEpos == epos && resultSpos < spos )
			{
				resultEpos = spos;
			}
			//2.上記以外
			//それ以外の場合、削除した位置をresultSpos(=resultEpos)にする。
			else
			{
				resultSpos = spos;
				resultEpos = spos;
			}
		}
		else
		{
			//Undo/Redo後に挿入文字列の最後にキャレット移動する設定の場合
			
			resultSpos = spos;
			resultEpos = spos;
		}
	}
	else if( tag == undoTag_DeleteText )
	{
		AIndex	spos = mUndoRecord_Spos.Get(inIndex);
		AObjectID	deletedTextObjectID = /*#216 mUndoRecord_Par*/mUndoRecord_DeletedTextObjectID.Get(inIndex);
		
		//
		AText	text;
		text.SetText(mDeletedTextArray.GetObject(mDeletedTextArray.GetIndexByID(deletedTextObjectID)));
		GetTextDocument().SPI_InsertText(spos,text,inRedrawOnceMode,inRedrawOnceMode);
		
		//
		mDeletedTextArray.Delete1Object(mDeletedTextArray.GetIndexByID(deletedTextObjectID));
		mUndoRecord_Tag.Set(inIndex,undoTag_InsertText);
		/*#216 mUndoRecord_Par*/mUndoRecord_Epos.Set(inIndex,spos+text.GetItemCount());
		
		//Reverse実行した場合、その後、その処理に続けて同種処理を行っても結合しない
		mCanCat = false;//#210 ==バグ修正
		
		//#219 outSelectTextIndex = spos;
		//#219 outSelectTextLength = text.GetItemCount();
		
		//#219 もし要望があるなら設定で動作をかえる
		if( false )
		{
			//Undo/Redo後に挿入文字列を選択する設定の場合
			//（十分にテストしていません）
			
			//1.直後に追加された場合
			//従来が{resultSpos,resultEpos}で、resultEpos==sposの場合、
			//選択範囲を広くする
			if( resultEpos == spos )
			{
				resultEpos = spos+text.GetItemCount();
			}
			//2.上記以外
			//それ以外の場合は、挿入した文字列の範囲を設定する
			else
			{
				resultSpos = spos;
				resultEpos = spos+text.GetItemCount();
			}
		}
		else
		{
			//Undo/Redo後に挿入文字列の最後にキャレット移動する設定の場合
			
			resultSpos = spos+text.GetItemCount();
			resultEpos = spos+text.GetItemCount();
		}
	}
	else
	{
		_ACError("",NULL);
	}
	//#219
	ioSelectTextIndex = resultSpos;
	ioSelectTextLength = resultEpos-resultSpos;
}

#pragma mark ===========================

#pragma mark ---情報取得

//Undoメニュー表示用文字列取得
ABool	ATextUndoer::GetUndoMenuText( AText& outText ) const 
{
	ABool	result = false;
	outText.SetLocalizedText("UndoAction_UndoDisabled");
	for( AIndex i = mUndoPosition-1; i >= 0; i-- ) 
	{
		AUndoTag	tag = mUndoRecord_Tag.Get(i);
		if( IsActionTag(tag) == true ) 
		{ 
			/*#94
			outText.SetLocalizedText("UndoAction_",tag-undoTag_Typing);
			AText	text;
			text.SetLocalizedText("UndoAction_Undo");
			outText.AddText(text);
			*/
			AText	text;
			text.SetLocalizedText("UndoAction_",tag-undoTag_Typing);
			outText.SetLocalizedText("UndoAction_Undo");
			outText.ReplaceParamText('0',text);
			result = true;
			break;
		}
	}
	return result;
}

//Redoメニュー表示用文字列取得
ABool	ATextUndoer::GetRedoMenuText( AText& outText ) const
{
	ABool	result = false;
	outText.SetLocalizedText("UndoAction_RedoDisabled");
	for( AIndex i = mUndoPosition; i < GetUndoRecordItemCount(); i++ ) 
	{
		AUndoTag	tag = mUndoRecord_Tag.Get(i);
		if( IsActionTag(tag) == true ) 
		{
			/*#94
			outText.SetLocalizedText("UndoAction_",tag-undoTag_Typing);
			AText	text;
			text.SetLocalizedText("UndoAction_Redo");
			outText.AddText(text);
			*/
			AText	text;
			text.SetLocalizedText("UndoAction_",tag-undoTag_Typing);
			outText.SetLocalizedText("UndoAction_Redo");
			outText.ReplaceParamText('0',text);
			result = true;
			break;
		}
	}
	return result;
}

//
AUndoTag	ATextUndoer::GetCurrentAction() const
{
	for( AIndex index = mUndoPosition-1; index >= 0; index-- )
	{
		AUndoTag	tag = mUndoRecord_Tag.Get(index);
		if( tag >= undoTag_Typing )
		{
			return tag;
		}
	}
	return undoTag_NOP;
}

//#267
/**
*/
AIndex	ATextUndoer::GetCurrentLastEditRecordPosition() const
{
	for( AIndex index = mUndoPosition-1; index >= 0; index-- )
	{
		AUndoTag	tag = mUndoRecord_Tag.Get(index);
		if( tag < undoTag_Typing )
		{
			return index;
		}
	}
	return kIndex_Invalid;
}

//#267
/**
レコードに結合禁止フラグを設定する
*/
void	ATextUndoer::SetInhibitConcat()
{
	AIndex	index = GetCurrentLastEditRecordPosition();
	if( index != kIndex_Invalid )
	{
		mUndoRecord_InhibitConcat.Set(index,true);
	}
}

//#314
//UndoRecordの最後のアクションに、挿入／削除レコードがない(dangling)かどうかを調べる
ABool	ATextUndoer::IsCurrentActionDangling() const
{
	if( GetUndoRecordItemCount() == 0 )   return false;
	//一番最後のレコードがActionTagなら、挿入／削除レコードがないということ
	return (IsActionTag(mUndoRecord_Tag.Get(GetUndoRecordItemCount()-1)) == true);
}


#pragma mark ===========================

#pragma mark ---

//UndoRecordの最後にある、アクションタグのみで、挿入／削除レコードがないものを削除する
void	ATextUndoer::DeleteDanglingActionTag()
{
	//UndoRecordの最後から順に検索
	while( GetUndoRecordItemCount() > 0 )
	{
		if( IsActionTag(mUndoRecord_Tag.Get(GetUndoRecordItemCount()-1)) == true )
		{
			//アクションタグなら削除して、ループ継続
			DeleteUndoRecord(GetUndoRecordItemCount()-1);
		}
		else
		{
			//挿入／削除レコードにぶつかったらループ終了
			break;
		}
	}
	//mUndoPosiion更新
	if( mUndoPosition > GetUndoRecordItemCount() )
	{
		mUndoPosition = GetUndoRecordItemCount();
	}
}

//アクションタグかどうかを取得
ABool	ATextUndoer::IsActionTag( const AUndoTag inTag ) const
{
	return (inTag >= undoTag_Typing);
}

//UndoRecord追加
void	ATextUndoer::AddUndoRecord( const AUndoTag inTag, const AIndex inStart, const AIndex inEnd, const AObjectID inDeletedTextObjectID )//#216 const AIndex inPar )
{
	//現在のUndoPosition以降を削除
	mUndoRecord_Tag.Delete(mUndoPosition,mUndoRecord_Tag.GetItemCount()-mUndoPosition);
	mUndoRecord_Spos.Delete(mUndoPosition,mUndoRecord_Spos.GetItemCount()-mUndoPosition);
	//#216 mUndoRecord_Par.Delete(mUndoPosition,mUndoRecord_Par.GetItemCount()-mUndoPosition);
	mUndoRecord_Epos.Delete(mUndoPosition,mUndoRecord_Epos.GetItemCount()-mUndoPosition);//#216
	mUndoRecord_DeletedTextObjectID.Delete(mUndoPosition,mUndoRecord_DeletedTextObjectID.GetItemCount()-mUndoPosition);//#216
	mUndoRecord_InhibitConcat.Delete(mUndoPosition,mUndoRecord_InhibitConcat.GetItemCount()-mUndoPosition);//#267
	
	//追加
	mUndoRecord_Tag.Add(inTag);
	mUndoRecord_Spos.Add(inStart);
	//#216 mUndoRecord_Par.Add(inPar);
	mUndoRecord_Epos.Add(inEnd);//#216
	mUndoRecord_DeletedTextObjectID.Add(inDeletedTextObjectID);//#216
	mUndoRecord_InhibitConcat.Add(false);//#267
	
	//UndoPosition更新
	mUndoPosition = GetUndoRecordItemCount();
}

//UndoRecord削除
void	ATextUndoer::DeleteUndoRecord( const AIndex inIndex )
{
	//削除レコードの場合は保存したテキストを削除
	if( mUndoRecord_Tag.Get(inIndex) == undoTag_DeleteText )
	{
		mDeletedTextArray.Delete1Object(mDeletedTextArray.GetIndexByID(/*#216mUndoRecord_Par*/mUndoRecord_DeletedTextObjectID.Get(inIndex)));
	}
	
	//削除
	mUndoRecord_Tag.Delete1(inIndex);
	mUndoRecord_Spos.Delete1(inIndex);
	//#216 mUndoRecord_Par.Delete1(inIndex);
	mUndoRecord_Epos.Delete1(inIndex);//#216
	mUndoRecord_DeletedTextObjectID.Delete1(inIndex);//#216
	mUndoRecord_InhibitConcat.Delete1(inIndex);//#267
}

//UndoRecord削除
void	ATextUndoer::DeleteUndoRecord( const AIndex inIndex, const AItemCount inCount )
{
	for( AIndex i = 0; i < inCount; i++ )
	{
		DeleteUndoRecord(inIndex);
	}
}

