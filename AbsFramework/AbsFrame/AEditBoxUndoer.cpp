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

AEditBoxUndoer
ATextUndoerからコピー

*/

#include "stdafx.h"

#include "AEditBoxUndoer.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AEditBoxUndoer
#pragma mark ===========================
//Undo処理クラス
#pragma mark ---コンストラクタ／デストラクタ
AEditBoxUndoer::AEditBoxUndoer( ADocument_EditBox& inTextDocument ) : mTextDocument(inTextDocument)
,mDeletedTextArray(NULL)//#693,true)//#417
{
	/*#135
	mUndoMode = false;
	mUndoPosition = 0;
	mCanCat = false;
	//#210 未使用のためコメントアウトmNoContinue = false;//B0090
	*/
	Init();
}

#pragma mark ===========================

#pragma mark ---初期化

//#135
/**
データ初期化
*/
void	AEditBoxUndoer::Init()
{
	mUndoRecord_Tag.DeleteAll();
	mUndoRecord_Spos.DeleteAll();
	mUndoRecord_Epos.DeleteAll();
	mUndoRecord_DeletedTextObjectID.DeleteAll();
	mDeletedTextArray.DeleteAll();
	mUndoPosition = 0;
	mUndoMode = false;
	mCanCat = false;
}

#pragma mark ===========================

#pragma mark ---記録

//アクションタグを記録
void	AEditBoxUndoer::RecordActionTag( const AEditBoxUndoTag inAction )
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
void	AEditBoxUndoer::RecordInsertText( const ATextIndex inInsertTextIndex, const AItemCount inLength )
{
	//Undo実行中なら何もしない
	if( mUndoMode == true )   return;
	
	//挿入レコード追加
	AddUndoRecord(kEditBoxUndoTag_InsertText,inInsertTextIndex,inInsertTextIndex+inLength,kObjectID_Invalid);//#216
	
	//
	if( mCanCat == false )
	{
		mCanCat = true;
	}
	else if( /*#210 未使用のためコメントアウト(mNoContinue == false) &&*/
				(mUndoRecord_Tag.GetItemCount() >= 3) && (GetCurrentAction() == kEditBoxUndoTag_Typing) )
	{
		//前回のUndoRecordがInsertTextで、終了文字位置が今回の開始文字位置と同じなら、まとめて１つのRecordにする
		AEditBoxUndoTag	prevAction = kEditBoxUndoTag_NOP;
		for( AIndex i = GetUndoRecordItemCount()-3; i >= 0; i-- )
		{
			prevAction = mUndoRecord_Tag.Get(i);
			if( prevAction >= kEditBoxUndoTag_Typing )   break;
		}
		if( prevAction == kEditBoxUndoTag_Typing )
		{
			AIndex	prevRecordIndex = GetUndoRecordItemCount()-2;
			for( ; prevRecordIndex >= 0; prevRecordIndex-- )
			{
				AEditBoxUndoTag	tag = mUndoRecord_Tag.Get(prevRecordIndex);
				if( tag == kEditBoxUndoTag_InsertText || tag == kEditBoxUndoTag_DeleteText )   break;
			}
			if( (mUndoRecord_Tag.Get(prevRecordIndex) == kEditBoxUndoTag_InsertText) && 
						(/*#216mUndoRecord_Par*/mUndoRecord_Epos.Get(prevRecordIndex) == inInsertTextIndex) )
			{
				DeleteUndoRecord(prevRecordIndex+1,GetUndoRecordItemCount()-(prevRecordIndex+1));
				/*#216mUndoRecord_Par*/mUndoRecord_Epos.Set(GetUndoRecordItemCount()-1,inInsertTextIndex+inLength);
				mUndoPosition = GetUndoRecordItemCount();
			}
		}
	}
	//#210 未使用のためコメントアウト mNoContinue = false;//B0090
}

//テキスト削除を記録
void	AEditBoxUndoer::RecordDeleteText( const ATextIndex inStart, const ATextIndex inEnd )
{
	//Undo実行中なら何もしない
	if( mUndoMode == true )   return;
	
	//削除予定テキストを取得し、mDeletedTextArrayへ格納
	AIndex	deletedTextIndex = mDeletedTextArray.AddNewObject();
	GetTextDocument().SPI_GetText(inStart,inEnd,mDeletedTextArray.GetObject(deletedTextIndex));
	AObjectID	deletedTextObjectID = mDeletedTextArray.GetObject(deletedTextIndex).GetObjectID();
	
	//削除レコード追加
	AddUndoRecord(kEditBoxUndoTag_DeleteText,inStart,inStart,deletedTextObjectID);//#216
	
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
					((GetCurrentAction() == kEditBoxUndoTag_BSKey)/*||GetCurrentAction() == kEditBoxUndoTag_Typing*/) )
		{
			/*
			UndoRecordの構造
			[前回のActionTag]
			[前回のInsertText]<-GetUndoRecordItemCount()-3
			[今回のActionTag]<-GetUndoRecordItemCount()-2
			[今回のInsertText]
			*/
			//prevActionに前回のActionTagの内容を取得する
			AEditBoxUndoTag	prevAction = kEditBoxUndoTag_NOP;
			for( AIndex	i = GetUndoRecordItemCount()-3; i >= 0; i-- )
			{
				prevAction = mUndoRecord_Tag.Get(i);
				if( prevAction >= kEditBoxUndoTag_Typing )   break;
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
					AEditBoxUndoTag	tag = mUndoRecord_Tag.Get(prevRecordIndex);
					if( tag == kEditBoxUndoTag_InsertText || tag == kEditBoxUndoTag_DeleteText )   break;
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
				  case kEditBoxUndoTag_BSKey:
					{
						//結合候補レコードのTagがDeleteTextで、かつ、その開始位置が、
						//今回追加したレコードの終了位置に等しいなら、１レコードに結合する
						if( (mUndoRecord_Tag.Get(prevRecordIndex) == kEditBoxUndoTag_DeleteText) &&
									(mUndoRecord_Spos.Get(prevRecordIndex) == inEnd) )
						{
							//今回追加したレコードを削除
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

//Undo実行
void	AEditBoxUndoer::Undo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	//アクションタグしか無いものを削除
	DeleteDanglingActionTag();
	
	//Undo不可ならリターン
	if( mUndoPosition <= 0 )   return;
	
	mUndoMode = true;
	for( mUndoPosition--; mUndoPosition > 0; mUndoPosition-- )
	{
		//アクションタグの位置まで来たらループ終了
		if( IsActionTag(mUndoRecord_Tag.Get(mUndoPosition)) == true )   break;
		//mUndoPosition位置の挿入／削除レコードを、逆転して実行し、レコードを逆転して保存
		ReverseTextAction(mUndoPosition,outSelectTextIndex,outSelectTextLength);
	}
	mUndoMode = false;
}

//Redo実行
void	AEditBoxUndoer::Redo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	//Redo不可ならリターン
	if( mUndoPosition >= GetUndoRecordItemCount() )   return;
	
	mUndoMode = true;
	for( mUndoPosition++; mUndoPosition < GetUndoRecordItemCount(); mUndoPosition++ )
	{
		//アクションタグの位置まで来たらループ終了
		if( IsActionTag(mUndoRecord_Tag.Get(mUndoPosition)) == true )   break;
		//mUndoPosition位置の挿入／削除レコードを、逆転して実行し、レコードを逆転して保存
		ReverseTextAction(mUndoPosition,outSelectTextIndex,outSelectTextLength);
	}
	mUndoMode = false;
}

//指定位置の挿入／削除レコードを、逆転して実行し、レコードを逆転して保存
void	AEditBoxUndoer::ReverseTextAction( const AIndex inIndex, AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	AEditBoxUndoTag	tag = mUndoRecord_Tag.Get(inIndex);
	if( tag == kEditBoxUndoTag_InsertText )
	{
		AIndex	spos = mUndoRecord_Spos.Get(inIndex);
		AIndex	epos = /*#216mUndoRecord_Par*/mUndoRecord_Epos.Get(inIndex);
		
		//削除予定テキストを取得し、mDeletedTextArrayへ格納
		AIndex	deletedTextIndex = mDeletedTextArray.AddNewObject();
		GetTextDocument().SPI_GetText(spos,epos,mDeletedTextArray.GetObject(deletedTextIndex));
		AObjectID	deletedTextObjectID = mDeletedTextArray.GetObject(deletedTextIndex).GetObjectID();
		
		//
		mUndoRecord_Tag.Set(inIndex,kEditBoxUndoTag_DeleteText);
		/*#216mUndoRecord_Par*/mUndoRecord_DeletedTextObjectID.Set(inIndex,deletedTextObjectID);
		
		mCanCat = false;//#210 ==バグ修正
		
		GetTextDocument().SPI_DeleteText(spos,epos);
		outSelectTextIndex = spos;
		outSelectTextLength = 0;
	}
	else if( tag == kEditBoxUndoTag_DeleteText )
	{
		AIndex	spos = mUndoRecord_Spos.Get(inIndex);
		AObjectID	deletedTextObjectID = /*#216mUndoRecord_Par*/mUndoRecord_DeletedTextObjectID.Get(inIndex);
		
		//
		AText	text;
		text.SetText(mDeletedTextArray.GetObject(mDeletedTextArray.GetIndexByID(deletedTextObjectID)));
		GetTextDocument().SPI_InsertText(spos,text);
		
		//
		mDeletedTextArray.Delete1Object(mDeletedTextArray.GetIndexByID(deletedTextObjectID));
		mUndoRecord_Tag.Set(inIndex,kEditBoxUndoTag_InsertText);
		/*#216mUndoRecord_Par*/mUndoRecord_Epos.Set(inIndex,spos+text.GetItemCount());
		
		mCanCat = false;//#210 ==バグ修正
		
		outSelectTextIndex = spos;
		outSelectTextLength = text.GetItemCount();
	}
	else
	{
		_ACError("",NULL);
	}
}

#pragma mark ===========================

#pragma mark ---情報取得

//Undoメニュー表示用文字列取得
ABool	AEditBoxUndoer::GetUndoMenuText( AText& outText ) const 
{
	ABool	result = false;
	outText.SetLocalizedText("EditBoxUndoAction_UndoDisabled");
	for( AIndex i = mUndoPosition-1; i >= 0; i-- ) 
	{
		AEditBoxUndoTag	tag = mUndoRecord_Tag.Get(i);
		if( IsActionTag(tag) == true ) 
		{ 
			/*#94
			outText.SetLocalizedText("EditBoxUndoAction_",tag-kEditBoxUndoTag_Typing);
			AText	text;
			text.SetLocalizedText("EditBoxUndoAction_Undo");
			outText.AddText(text);
			*/
			AText	text;
			text.SetLocalizedText("EditBoxUndoAction_",tag-kEditBoxUndoTag_Typing);
			outText.SetLocalizedText("EditBoxUndoAction_Undo");
			outText.ReplaceParamText('0',text);
			result = true;
			break;
		}
	}
	return result;
}

//Redoメニュー表示用文字列取得
ABool	AEditBoxUndoer::GetRedoMenuText( AText& outText ) const
{
	ABool	result = false;
	outText.SetLocalizedText("EditBoxUndoAction_RedoDisabled");
	for( AIndex i = mUndoPosition; i < GetUndoRecordItemCount(); i++ ) 
	{
		AEditBoxUndoTag	tag = mUndoRecord_Tag.Get(i);
		if( IsActionTag(tag) == true ) 
		{
			/*#94
			outText.SetLocalizedText("EditBoxUndoAction_",tag-kEditBoxUndoTag_Typing);
			AText	text;
			text.SetLocalizedText("EditBoxUndoAction_Redo");
			outText.AddText(text);
			*/
			AText	text;
			text.SetLocalizedText("EditBoxUndoAction_",tag-kEditBoxUndoTag_Typing);
			outText.SetLocalizedText("EditBoxUndoAction_Redo");
			outText.ReplaceParamText('0',text);
			result = true;
			break;
		}
	}
	return result;
}

//
AEditBoxUndoTag	AEditBoxUndoer::GetCurrentAction() const
{
	for( AIndex index = mUndoPosition-1; index >= 0; index-- )
	{
		AEditBoxUndoTag	tag = mUndoRecord_Tag.Get(index);
		if( tag >= kEditBoxUndoTag_Typing )
		{
			return tag;
		}
	}
	return kEditBoxUndoTag_NOP;
}

#pragma mark ===========================

#pragma mark ---

//UndoRecordの最後にある、アクションタグのみで、挿入／削除レコードがないものを削除する
void	AEditBoxUndoer::DeleteDanglingActionTag()
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
ABool	AEditBoxUndoer::IsActionTag( const AEditBoxUndoTag inTag ) const
{
	return (inTag >= kEditBoxUndoTag_Typing);
}

//UndoRecord追加
void	AEditBoxUndoer::AddUndoRecord( const AEditBoxUndoTag inTag, const AIndex inStart, const AIndex inEnd, const AObjectID inDeletedTextObjectID )//#216const AIndex inPar )
{
	//現在のUndoPosition以降を削除
	mUndoRecord_Tag.Delete(mUndoPosition,mUndoRecord_Tag.GetItemCount()-mUndoPosition);
	mUndoRecord_Spos.Delete(mUndoPosition,mUndoRecord_Spos.GetItemCount()-mUndoPosition);
	//#216 mUndoRecord_Par.Delete(mUndoPosition,mUndoRecord_Par.GetItemCount()-mUndoPosition);
	mUndoRecord_Epos.Delete(mUndoPosition,mUndoRecord_Epos.GetItemCount()-mUndoPosition);//#216
	mUndoRecord_DeletedTextObjectID.Delete(mUndoPosition,mUndoRecord_DeletedTextObjectID.GetItemCount()-mUndoPosition);//#216
	
	//追加
	mUndoRecord_Tag.Add(inTag);
	mUndoRecord_Spos.Add(inStart);
	//#216 mUndoRecord_Par.Add(inPar);
	mUndoRecord_Epos.Add(inEnd);//#216
	mUndoRecord_DeletedTextObjectID.Add(inDeletedTextObjectID);//#216
	
	//UndoPosition更新
	mUndoPosition = GetUndoRecordItemCount();
}

//UndoRecord削除
void	AEditBoxUndoer::DeleteUndoRecord( const AIndex inIndex )
{
	//削除レコードの場合は保存したテキストを削除
	if( mUndoRecord_Tag.Get(inIndex) == kEditBoxUndoTag_DeleteText )
	{
		mDeletedTextArray.Delete1Object(mDeletedTextArray.GetIndexByID(/*#216mUndoRecord_Par*/mUndoRecord_DeletedTextObjectID.Get(inIndex)));
	}
	
	//削除
	mUndoRecord_Tag.Delete1(inIndex);
	mUndoRecord_Spos.Delete1(inIndex);
	//#216 mUndoRecord_Par.Delete1(inIndex);
	mUndoRecord_Epos.Delete1(inIndex);//#216
	mUndoRecord_DeletedTextObjectID.Delete1(inIndex);//#216
}

//UndoRecord削除
void	AEditBoxUndoer::DeleteUndoRecord( const AIndex inIndex, const AItemCount inCount )
{
	for( AIndex i = 0; i < inCount; i++ )
	{
		DeleteUndoRecord(inIndex);
	}
}

