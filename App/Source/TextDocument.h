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


#import <Cocoa/Cocoa.h>
#include "../../AbsFramework/Frame.h"
#include "ADocument_Text.h"

#import "AppleScript.h"

@class CASParagraph;
@class CASWord;
@class CASCharacter;
@class CASInsertionPoint;
@class CASSelectionObject;

#pragma mark ===========================
#pragma mark [クラス]TextDocument
#pragma mark ===========================
//NSDocumentクラス
@interface TextDocument : NSDocument
{
	//DocumentID
	ADocumentID					mDocumentID;
	
	//LazyObjectsArray
	LazyObjectsArray*			mParagraphLazyObjectsArray;
	LazyObjectsArray*			mWordLazyObjectsArray;
	LazyObjectsArray*			mCharacterLazyObjectsArray;
	LazyObjectsArray*			mInsertionPointLazyObjectsArray;
	LazyObjectsArray*			mSelectionObjectLazyObjectsArray;
}
- (void)setDocumentID:(ADocumentID)docID;
- (ADocumentID)documentID;
@end

#pragma mark ===========================
#pragma mark [クラス]CASTextDocElement
#pragma mark ===========================
//ドキュメントの各Lazy elementの基底クラス
@interface CASTextDocElement : LazyObjectsArrayItem
{
	//TextDocument
	TextDocument*				mTextDocument;
	//DocumentID
	ADocumentID					mDocumentID;
	//テキスト範囲
	ATextIndex					mStartTextIndex;
	ATextIndex					mEndTextIndex;
}
- (id)init:(TextDocument*)textDocument start:(AIndex)start end:(AIndex)end;
- (ADocumentID)documentID;
- (TextDocument*)textDocument;
- (ATextIndex)startTextIndex;
- (ATextIndex)endTextIndex;
- (void)setDocumentID:(ADocumentID)docID;
-(NSAppleEventDescriptor*)content;
@end

#pragma mark ===========================
#pragma mark [クラス]CASParagraph
#pragma mark ===========================
//paragraph
@interface CASParagraph : CASTextDocElement
{
	//LazyObjectsArray
	LazyObjectsArray*			mWordLazyObjectsArray;
	LazyObjectsArray*			mCharacterLazyObjectsArray;
	LazyObjectsArray*			mInsertionPointLazyObjectsArray;
}
- (id)init:(TextDocument*)textDocument start:(AIndex)start end:(AIndex)end;
@end

#pragma mark ===========================
#pragma mark [クラス]CASParagraph
#pragma mark ===========================
//word
@interface CASWord : CASTextDocElement
{
	//LazyObjectsArray
	LazyObjectsArray*			mCharacterLazyObjectsArray;
	LazyObjectsArray*			mInsertionPointLazyObjectsArray;
}
- (id)init:(TextDocument*)textDocument start:(AIndex)start end:(AIndex)end;
@end

#pragma mark ===========================
#pragma mark [クラス]CASCharacter
#pragma mark ===========================
//character
@interface CASCharacter : CASTextDocElement
{
	//LazyObjectsArray
	LazyObjectsArray*			mInsertionPointLazyObjectsArray;
}
- (id)init:(TextDocument*)textDocument start:(AIndex)start end:(AIndex)end;
@end

#pragma mark ===========================
#pragma mark [クラス]CASInsertionPoint
#pragma mark ===========================
//insertion point
@interface CASInsertionPoint : CASTextDocElement
{
}
- (id)init:(TextDocument*)textDocument textIndex:(AIndex)textIndex;
@end

#pragma mark ===========================
#pragma mark [クラス]CASSelectionObject
#pragma mark ===========================
//selection object
@interface CASSelectionObject : CASTextDocElement
{
	//LazyObjectsArray
	LazyObjectsArray*			mParagraphLazyObjectsArray;
	LazyObjectsArray*			mWordLazyObjectsArray;
	LazyObjectsArray*			mCharacterLazyObjectsArray;
	LazyObjectsArray*			mInsertionPointLazyObjectsArray;
}
- (id)init:(TextDocument*)textDocument start:(AIndex)start end:(AIndex)end;
@end
