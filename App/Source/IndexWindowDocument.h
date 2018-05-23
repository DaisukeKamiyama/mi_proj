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
#include "ADocument_IndexWindow.h"
#include "AApp.h"
#include "ATextInfo.h"

#import "AppleScript.h"

@class CASIndexGroup;
@class CASIndexRecord;

#pragma mark ===========================
#pragma mark [クラス]IndexWindowDocument
#pragma mark ===========================
@interface IndexWindowDocument : NSDocument
{
	//DocumentID
	ADocumentID					mDocumentID;
	
	//LazyObjectsArray
	LazyObjectsArray*			mIndexGroupLazyObjectsArray;
	LazyObjectsArray*			mIndexRecordLazyObjectsArray;
	
	//indexrecord値設定用キャッシュ
	ATextInfo					mCache_TextInfo;
	AFileAcc					mCache_File;
	AText						mCache_Text;
}
- (void)setDocumentID:(ADocumentID)docID;
- (ADocumentID)documentID;
@end

#pragma mark ===========================
#pragma mark [クラス]CASIndexGroup
#pragma mark ===========================
@interface CASIndexGroup : LazyObjectsArrayItem
{
	//IndexWindowDocument
	IndexWindowDocument*		mIndexWindowDocument;
	//DocumentID
	ADocumentID					mDocumentID;
	//index
	AIndex						mGroupIndex;
	
	//LazyObjectsArray
	LazyObjectsArray*			mIndexRecordLazyObjectsArray;
}
- (id)init:(IndexWindowDocument*)indexWindowDocument groupIndex:(AIndex)groupIndex;
- (ADocumentID)documentID;
- (IndexWindowDocument*)indexWindowDocument;
- (ATextIndex)groupIndex;
-(NSAppleEventDescriptor*)content;
@end

#pragma mark ===========================
#pragma mark [クラス]CASIndexRecord
#pragma mark ===========================
@interface CASIndexRecord : LazyObjectsArrayItem
{
	//IndexWindowDocument
	IndexWindowDocument*		mIndexWindowDocument;
	//DocumentID
	ADocumentID					mDocumentID;
	//index
	AIndex						mGroupIndex;
	AIndex						mItemIndex;
}
- (id)init:(IndexWindowDocument*)indexWindowDocument groupIndex:(AIndex)groupIndex itemIndex:(AIndex)itemIndex;
- (ADocumentID)documentID;
- (IndexWindowDocument*)indexWindowDocument;
- (ATextIndex)groupIndex;
- (ATextIndex)itemIndex;
-(NSAppleEventDescriptor*)content;
@end

