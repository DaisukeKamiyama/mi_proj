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

AEditBoxUndoer.h
ATextUndoerからコピー

*/

#pragma once

#include "../AbsBase/ABase.h"

class ADocument_EditBox;

enum AEditBoxUndoTag 
{
	kEditBoxUndoTag_NOP = 0,
	kEditBoxUndoTag_InsertText,
	kEditBoxUndoTag_DeleteText,
	kEditBoxUndoTag_Typing = 3,
	kEditBoxUndoTag_Delete,
	kEditBoxUndoTag_BSKey,
	kEditBoxUndoTag_DELKey,
	kEditBoxUndoTag_Cut,
	kEditBoxUndoTag_Paste,
	kEditBoxUndoTag_DragDrop
};

#pragma mark ===========================
#pragma mark [クラス]AEditBoxUndoer
/**
AView_EditBox/ADocument_EditBox用Undo処理クラス
*/
class AEditBoxUndoer
{
	//コンストラクタ／デストラクタ
  public:
	AEditBoxUndoer( ADocument_EditBox& inTextDocument );
  private:
	ADocument_EditBox&					GetTextDocument() { return mTextDocument; }
	ADocument_EditBox&					mTextDocument;
	
	//初期化 #135
  public:
	void					Init();
	
	//記録
  public:
	void					RecordActionTag( const AEditBoxUndoTag inAction );
	void					RecordInsertText( const ATextIndex inInsertTextIndex, const AItemCount inLength );
	void					RecordDeleteText( const ATextIndex inStart, const ATextIndex inEnd );
	
	//Undo/Redo実行
  public:
	void					Undo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength );
	void					Redo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength );
  private:
	void					ReverseTextAction( const AIndex inIndex, AIndex& outSelectTextIndex, AItemCount& outSelectTextLength );
	
	//情報取得
  public:
	ABool					GetUndoMenuText( AText& outText ) const;
	ABool					GetRedoMenuText( AText& outText ) const;
	AEditBoxUndoTag			GetCurrentAction() const;
	
  private:
	void					DeleteDanglingActionTag();
	ABool					IsActionTag( const AEditBoxUndoTag inTag ) const;
	//#216void					AddUndoRecord( const AEditBoxUndoTag inTag, const AIndex inStart, const AIndex inPar );
	void					AddUndoRecord( const AEditBoxUndoTag inTag, const AIndex inStart, const AIndex inEnd, const AObjectID inDeletedTextObjectID );//#216
	void					DeleteUndoRecord( const AIndex inIndex );
	void					DeleteUndoRecord( const AIndex inIndex, const AItemCount inCount );
	
  private:
	AItemCount				GetUndoRecordItemCount() const { return mUndoRecord_Tag.GetItemCount(); }
	AArray<AEditBoxUndoTag>				mUndoRecord_Tag;
	AArray<AIndex>						mUndoRecord_Spos;
	//#216 AArray<AIndex>						mUndoRecord_Par;
	AArray<AIndex>						mUndoRecord_Epos;//#216
	AArray<AObjectID>					mUndoRecord_DeletedTextObjectID;//#216
	AObjectArrayIDIndexed<AText>		mDeletedTextArray;//#693
	
	AIndex								mUndoPosition;
	ABool								mUndoMode;
	ABool								mCanCat;
	//#210 未使用のためコメントアウトABool								mNoContinue;//検討
};

