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

ATextUndoer.h

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class ADocument_Text;

enum AUndoTag 
{
	undoTag_NOP = 0,
	undoTag_InsertText,
	undoTag_DeleteText,
	undoTag_Typing = 3,
	undoTag_Delete,
	undoTag_BSKey,
	undoTag_DELKey,
	undoTag_Cut,
	undoTag_Paste,
	undoTag_Replace,
	undoTag_Indent,
	undoTag_Shift,
	undoTag_Transliterate,
	undoTag_ChangeCase,
	undoTag_ReturnAtWrap,
	undoTag_DragDrop,
	undoTag_AppleEvent,
	undoTag_KukeiDelete,
	undoTag_KukeiCut,
	undoTag_KukeiPaste,
	undoTag_KukeiDragDrop,
	undoTag_Tool,
	undoTag_Replace1,
	undoTag_EditByOtherApp,
	undoTag_AbbrevInput,
	undoTag_InputCorrespond,
};

#pragma mark ===========================
#pragma mark [クラス]ATextUndoer
//Undo処理クラス
class ATextUndoer
{
	//コンストラクタ／デストラクタ
  public:
	ATextUndoer( ADocument_Text& inTextDocument );
  private:
	ADocument_Text&						GetTextDocument() { return mTextDocument; }
	ADocument_Text&						mTextDocument;
	
	//記録
  public:
	void					RecordActionTag( const AUndoTag inAction );
	void					RecordInsertText( const ATextIndex inInsertTextIndex, const AItemCount inLength );
	void					RecordDeleteText( const ATextIndex inStart, const ATextIndex inEnd );
	
	//Undo/Redo実行
  public:
	ABool					Undo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength );
	ABool					Redo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength );
  private:
	void					ReverseTextAction( const AIndex inIndex, AIndex& ioSelectTextIndex, AItemCount& ioSelectTextLength,
							const ABool inRedrawOnceMode );
	
	//情報取得
  public:
	ABool					GetUndoMenuText( AText& outText ) const;
	ABool					GetRedoMenuText( AText& outText ) const;
	AUndoTag				GetCurrentAction() const;
	AIndex					GetCurrentLastEditRecordPosition() const;//#267
	ABool					IsCurrentActionDangling() const;//#314
	
	//#267
  public:
	void					SetInhibitConcat();
	
  private:
	void					DeleteDanglingActionTag();
	ABool					IsActionTag( const AUndoTag inTag ) const;
	//#216 void					AddUndoRecord( const AUndoTag inTag, const AIndex inStart, const AIndex inPar );
	void					AddUndoRecord( const AUndoTag inTag, const AIndex inStart, const AIndex inEnd, const AObjectID inDeletedTextObjectID );//#216
	void					DeleteUndoRecord( const AIndex inIndex );
	void					DeleteUndoRecord( const AIndex inIndex, const AItemCount inCount );
	
  private:
	AItemCount				GetUndoRecordItemCount() const { return mUndoRecord_Tag.GetItemCount(); }
	AArray<AUndoTag>					mUndoRecord_Tag;
	AArray<AIndex>						mUndoRecord_Spos;
	//#216 AArray<AIndex>						mUndoRecord_Par;
	AArray<AIndex>						mUndoRecord_Epos;//#216
	AArray<AObjectID>					mUndoRecord_DeletedTextObjectID;//#216
	AArray<ABool>						mUndoRecord_InhibitConcat;//#267
	AObjectArrayIDIndexed<AText>		mDeletedTextArray;//#693
	
	AIndex								mUndoPosition;
	ABool								mUndoMode;
	ABool								mCanCat;
	//#210 未使用のためコメントアウトABool								mNoContinue;//検討
};

