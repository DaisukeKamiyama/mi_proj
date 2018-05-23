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

IndexWindowDocument.mm

*/

#import <Cocoa/Cocoa.h>
#include "../../AbsFramework/AbsBase/Cocoa.h"
#include "AApp.h"
#import "../../AbsFramework/MacImp/AppDelegate.h"
#import "IndexWindowDocument.h"

#pragma mark ===========================
#pragma mark [クラス]IndexWindowDocument
#pragma mark ===========================

@implementation IndexWindowDocument

- (id)init
{
	self = [super init];
	if (self) 
	{
		//メンバ変数初期化
		[self initMemberVariables];
	}
	return self;
}

- (void)dealloc
{
	//LazyObjectsArray解放
	[mIndexGroupLazyObjectsArray release];
	[mIndexRecordLazyObjectsArray release];
	//継承処理
	[super dealloc];
}

/**
メンバー変数初期化
init系メソッドからコールされる
*/
- (void)initMemberVariables
{
	//DocumentID初期化
	mDocumentID = kObjectID_Invalid;
	//LazyObjectsArray生成
	mIndexGroupLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
	mIndexRecordLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
}

/**
ADocumentのDocumentID設定
（ADocumentの生成時にコールされる）
*/
- (void)setDocumentID:(ADocumentID)docID
{
	mDocumentID = docID;
}

/**
ADocumentのDocumentID取得
*/
- (ADocumentID)documentID
{
	return mDocumentID;
}

/**
並行読み込みするか否か
NOを返す
*/
+ (BOOL)canConcurrentlyReadDocumentsOfType:(NSString *)typeName
{
	return NO;
}

#pragma mark ===========================

#pragma mark --- AppleScript (document)

#pragma mark ===========================

#pragma mark --- object specifier

/**
objectSpecifier取得
*/
- (NSScriptObjectSpecifier *)objectSpecifier 
{
	//DocumentのuniqIDを取得
	AUniqID	docUniqID = GetApp().NVI_GetDocumentUniqID(mDocumentID);
	//applicationのclass description取得
	NSScriptClassDescription *containerClassDesc = (NSScriptClassDescription *)
			[NSScriptClassDescription classDescriptionForClass:[CocoaApp class]];
	//NSUniqueIDSpecifier（container:application uniqueID:uniqID）を返す
	return [[[NSUniqueIDSpecifier alloc] initWithContainerClassDescription:containerClassDesc 
			containerSpecifier:nil 
			key:@"asIndexwindow" 
	uniqueID:[NSNumber numberWithInt:docUniqID.val]] autorelease];
}

/**
uniqueid取得
*/
- (NSNumber*)uniqueid
{
	AUniqID	docUniqID = GetApp().NVI_GetDocumentUniqID(mDocumentID);
	return [NSNumber numberWithInt:docUniqID.val];
}

#pragma mark ===========================

#pragma mark --- element

/**
specifierで指定された子elementのindex（子element array内でのindex）を取得する
CASIndexWindowDoc::GetSubModelByPosition()に対応
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifierで指定の場合 ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//
		AIndex	index = [indexSpec index];
		// ---------- indexgroup ----------
		if( [[indexSpec key] isEqual:@"asIndexgroupElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfIndexgroups];
				}
				//Lazyオブジェクト生成(autorelease)
				CASIndexGroup*	obj = [[CASIndexGroup alloc] init:self groupIndex:index];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	lazyIndex = [mIndexGroupLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				[CocoaApp appleScriptNoSuchObjectError:@"The indexgroup not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- indexrecord ----------
		else if( [[indexSpec key] isEqual:@"asIndexrecordElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfIndexrecords];
				}
				//指定indexのindexrecordのgroupIndex, itemIndex取得
				AIndex	groupIndex = 0, itemIndex = 0;
				if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).
					SPI_GetIndexFromASRecordIndex(index,groupIndex,itemIndex) == false )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASIndexRecord*	obj = [[CASIndexRecord alloc] init:self groupIndex:groupIndex itemIndex:itemIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	lazyIndex = [mIndexRecordLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				[CocoaApp appleScriptNoSuchObjectError:@"The indexrecord not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- 上記以外オブジェクトの場合はエラー ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else
	{
		// ==================== 上記以外で指定の場合はエラー ====================
		// プロパティ取得の場合もここに来るので、エラーにせず、nilを返す
		return nil;
	}
}

/**
*/
- (NSArray *)asIndexgroupElement
{
	return [mIndexGroupLazyObjectsArray array];
}

/**
*/
- (NSArray *)asIndexrecordElement
{
	return [mIndexRecordLazyObjectsArray array];
}

#pragma mark ===========================

#pragma mark --- property

// -------------------- content プロパティ --------------------

/**
content取得
*/
-(NSAppleEventDescriptor*)asContent
{
	AIndex	index = 0;
	NSAppleEventDescriptor *aeDescList = [NSAppleEventDescriptor listDescriptor];
	AItemCount	groupCount = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetGroupCount();
	for( AIndex groupIndex = 0; groupIndex < groupCount; groupIndex++ )
	{
		AItemCount	itemCount = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetItemCountInGroup(groupIndex);
		for( AIndex itemIndex = 0; itemIndex < itemCount; itemIndex++ )
		{
			NSAppleEventDescriptor*	desc = [self getDescFromItem:groupIndex itemIndex:itemIndex];
			[aeDescList insertDescriptor:desc atIndex:index+1];
			index++;
		}
	}
	return aeDescList;
}

/**
content設定
*/
- (void)setAsContent:(id)value
{
	NSAppleEventDescriptor*	aeDescList = (NSAppleEventDescriptor*)value;
	//全削除
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_DeleteAllGroup();
	//グループを1つ追加（このグループに全てのindexrecordを入れる）
	AFileAcc	basefolder;//#465（ここはベースフォルダ未対応）
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_AddGroup(GetEmptyText(),GetEmptyText(),basefolder);//#465
	//引数のaeDescListの各要素ごとのループ
	NSInteger count = [aeDescList numberOfItems];
	for( AIndex i = 0; i < count ;i++ )
	{
		//各要素を取得
		NSAppleEventDescriptor*	aeDesc = [aeDescList descriptorAtIndex:i+1];
		//indexrecordを追加
		GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).
				SPI_InsertItem_TextPosition(0,i,
											GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),
											GetEmptyText(),//#465
											0,0,0,GetEmptyText());
		//indexrecordに値を設定
		[self setItemFromDesc:0 itemIndex:i ae:aeDesc];
	}
}

// -------------------- index プロパティ --------------------

/**
index取得
*/
- (id)asIndex
{
	return [NSAppleEventDescriptor descriptorWithInt32:1];
}

/**
index（TextDocumentのウインドウz-order順）設定
*/
- (void)setAsIndex:(id)value
{
	//処理なし
}

// -------------------- file プロパティ --------------------

/**
file取得
*/
- (id)asFile
{
	AFileAcc	file;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).NVI_GetFile(file);
	AText	path;
	file.GetPath(path);
	NSURL*	url =  [NSURL fileURLWithPath:path.CreateNSString(true) isDirectory:NO];
	return [NSAppleEventDescriptor descriptorWithURL:url];
}

// -------------------- name プロパティ --------------------

/**
name取得
*/
- (id)asName
{
	AText	text;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).NVI_GetTitle(text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
name設定
*/
- (void)setAsName:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//temporary title設定
	AText	text;
	text.SetFromNSString([aeDesc stringValue]);
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SetTitle(text);
}

// -------------------- modified プロパティ --------------------

/**
modified取得
*/
- (id)asModified
{
	Boolean	b = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).NVI_IsDirty();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- filewidth プロパティ --------------------

/**
filewidth取得
*/
- (id)asFilewidth
{
	ANumber	fileWidth, infoWidth;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetColumnWidth(fileWidth,infoWidth);	
	return [NSAppleEventDescriptor descriptorWithInt32:fileWidth];
}

/**
filewidth設定
*/
- (void)setAsFilewidth:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	SInt32	width = [aeDesc int32Value];
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASSetFileColumnWidth(width);
}

// -------------------- infowidth プロパティ --------------------

/**
infowidth取得
*/
- (id)asInfowidth
{
	ANumber	fileWidth, infoWidth;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetColumnWidth(fileWidth,infoWidth);	
	return [NSAppleEventDescriptor descriptorWithInt32:infoWidth];
}

/**
infowidth設定
*/
- (void)setAsInfowidth:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	SInt32	width = [aeDesc int32Value];
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASSetInfoColumnWidth(width);
}

// -------------------- fileorder プロパティ --------------------

/**
fileorder取得
*/
- (id)asFileorder
{
	return [NSAppleEventDescriptor descriptorWithInt32:1];
}

// -------------------- infoorder プロパティ --------------------

/**
infoorder取得
*/
- (id)asInfoorder
{
	return [NSAppleEventDescriptor descriptorWithInt32:2];
}

// -------------------- asksaving プロパティ --------------------

/**
asksaving取得
*/
- (id)asAsksaving
{
	Boolean	b = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetAskSavingMode();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

/**
asksaving設定
*/
- (void)setAsAsksaving:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	Boolean	b = [aeDesc booleanValue];
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SetAskSavingMode(b);
}

// -------------------- windowindex プロパティ --------------------

/**
windowindex取得
*/
- (id)asWindowindex
{
	//AWindowリスト取得
	AArray<AWindowID>	winIDArray;
	GetApp().NVI_GetOrderedWindowIDArray(winIDArray,kWindowType_Invalid);
	//このドキュメントのウインドウのwindowIDを取得
	AWindowID	winID = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetWindowID();
	//indexを取得
	AIndex	index = winIDArray.Find(winID);
	if( index != kIndex_Invalid )
	{
		return [NSAppleEventDescriptor descriptorWithInt32:index+1];
	}
	else
	{
		return nil;
	}
}

/**
windowindex設定
*/
- (void)setAsWindowindex:(id)value
{
	/*
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	SInt32	number = [aeDesc int32Value];
	AWindowID	winID = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetWindowID();
	CocoaWindow*	win = GetApp().NVI_GetWindowByID(winID).NVI_GetWindowImp().GetCocoaWindow();
	[win setWindowOrder:number];
	//★下位レイヤで順序設定して問題ないか？
	*/
}

// -------------------- indexrecord値取得・設定 --------------------

/**
NSAppleEventDescriptorからindexrecordの値を設定
*/
- (void)setItemFromDesc:(AIndex)inGroupIndex itemIndex:(AIndex)inItemIndex ae:(NSAppleEventDescriptor*)inAE
{
	const AItemCount	kPreHitTextLength = 100;
	const AItemCount	kPostHitTextLength = 100;
	
	NSAppleEventDescriptor*	commentDesc = [inAE descriptorForKeyword:pCommentF];
	AText	comment;
	if( commentDesc != nil )
	{
		comment.SetFromNSString([commentDesc stringValue]);
	}
	AText	filepath, hittext, prehit, posthit, positiontext;
	NSAppleEventDescriptor*	fileDesc = [inAE descriptorForKeyword:pFileF];
	if( fileDesc != nil )
	{
		filepath.SetCstring([[[fileDesc urlValue] path] UTF8String]);
	}
	AFileAcc	file;
	file.Specify(filepath);
	if( file.IsSpecified() == false )
	{
		filepath.SetFromNSString([fileDesc stringValue]);
		filepath.Insert1(0,':');
		file.Specify(filepath,kFilePathType_2);
	}
	file.GetPath(filepath);
	
	ANumber	spos = 0, len = 0, paragraph = kIndex_Invalid;
	if( file.Exist() == true && file.IsFolder() == false/*xcode3 080804*/ )
	{
		if( file.Compare(mCache_File) == false )
		{
			AModeIndex	modeIndex;
			AText	tecname;
			GetApp().SPI_LoadTextFromFileOrDocument(kLoadTextPurposeType_ASIndexDoc,file,mCache_Text,modeIndex,tecname);
			ATextInfo	textInfo(NULL);
			textInfo.SetMode(modeIndex);
			ABool	abortFlag = false;
			mCache_TextInfo.CalcLineInfoAllWithoutView(mCache_Text,abortFlag);//win
			//#695 AIndex	startLineIndex, endLineIndex;
			ABool	importChanged = false;
			AArray<AUniqID>	addedIdentifier;
			AArray<AIndex>		addedIdentifierLineIndex;
			//#695 AArray<AUniqID>	deletedIdentifier;//win
			AText	path;//#998
			file.GetPath(path);//#998
			mCache_TextInfo.RecognizeSyntaxAll(mCache_Text,path,//#695 startLineIndex,endLineIndex,deletedIdentifier, #998
											   addedIdentifier,addedIdentifierLineIndex,importChanged,abortFlag);//R0000 #698
			mCache_File.CopyFrom(file);
		}
		
		NSAppleEventDescriptor*	lineNumberDesc = [inAE descriptorForKeyword:pLineNumber];
		if( lineNumberDesc != nil )
		{
			paragraph = [lineNumberDesc int32Value];
			paragraph --;
			spos = mCache_TextInfo.GetLineStart(paragraph);
			len = mCache_TextInfo.GetLineLengthWithLineEnd(paragraph);
		}
		else
		{
			spos = 0;
			NSAppleEventDescriptor*	startPositionDesc = [inAE descriptorForKeyword:pStartPositionF];
			if( startPositionDesc != nil )
			{
				spos = [startPositionDesc int32Value];
			}
			ANumber	epos = 0;
			NSAppleEventDescriptor*	endPositionDesc = [inAE descriptorForKeyword:pEndPositionF];
			if( endPositionDesc != nil )
			{
				epos = [endPositionDesc int32Value];
			}
			len = epos-spos;
			spos--;
			if( spos < 0 )   spos = 0;
			if( len > mCache_Text.GetItemCount() )   len = mCache_Text.GetItemCount();
			ATextPoint	pt;
			mCache_TextInfo.GetTextPointFromTextIndex(spos,pt);
			paragraph = pt.y;
		}
		mCache_Text.GetText(spos,len,hittext);
		ATextIndex	p = spos-kPreHitTextLength;
		if( p < 0 )   p = 0;
		mCache_Text.GetText(p,spos-p,prehit);
		p = spos+len+kPostHitTextLength;
		if( p >= mCache_Text.GetItemCount() )   p = mCache_Text.GetItemCount();
		mCache_Text.GetText(spos+len,p-(spos+len),posthit);
	}
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).
			SPI_SetItem_TextPosition(inGroupIndex,inItemIndex,filepath,hittext,prehit,posthit,positiontext,
									 GetEmptyText(),//#465
									 spos,len,paragraph,comment);
}

/**
indexrecordからNSAppleEventDescriptorを取得
*/
-(NSAppleEventDescriptor*)getDescFromItem:(AIndex)inGroupIndex itemIndex:(AIndex)inItemIndex
{
	//指定group index, item indexのデータを取得
	AIndex	ASIndex;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(inGroupIndex,inItemIndex,ASIndex);
	AFileAcc	file;
	ATextIndex	spos;
	AItemCount	len;
	AText	comment;
	ANumber	para;
	if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetRecord(ASIndex,file,spos,len,para,comment) == false )
	{
		[CocoaApp appleScriptNotHandledError];
		return nil;
	}
	
	//結果格納用record descriptor生成
	NSAppleEventDescriptor *aeDesc = [NSAppleEventDescriptor recordDescriptor];
	//file格納
	if( file.Exist() == true )
	{
		AText	path;
		file.GetPath(path);
		NSURL*	url =  [NSURL fileURLWithPath:path.CreateNSString(true) isDirectory:NO];
		[aeDesc setDescriptor:[NSAppleEventDescriptor descriptorWithURL:url] forKeyword:pFileF];
	}
	//テキスト範囲格納
	SInt32	start = spos;
	SInt32	end = spos+len;
	[aeDesc setDescriptor:[NSAppleEventDescriptor descriptorWithInt32:spos+1] forKeyword:pStartPositionF];
	[aeDesc setDescriptor:[NSAppleEventDescriptor descriptorWithInt32:spos+1+len] forKeyword:pEndPositionF];
	//コメント格納
	[aeDesc setDescriptor:[NSAppleEventDescriptor descriptorWithString:comment.CreateNSString(true)] forKeyword:pCommentF];
	return aeDesc;
}

#pragma mark ===========================

#pragma mark --- command handler

/**
closeコマンド
*/
- (id)handleCloseScriptCommand:(MICloseCommand*)command
{
	//asksave変数初期化
	NSUInteger askSave = 'ask ';
	//引数取得
	NSDictionary *args = [command evaluatedArguments];
	if( args != nil )
	{
		//asksave引数取得
		NSNumber*	argSaving = [args valueForKey:@"saving"];
		if( argSaving != nil )
		{
			askSave = [argSaving intValue];
		}
	}
	//asksaveに従って閉じる処理
	switch(askSave)
	{
	  case 'ask ':
		{
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_TryClose();
			break;
		}
	  case 'yes ':
		{
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_Save(false);
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_TryClose();
			break;
		}
	  case 'no  ':
		{
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ExecuteClose();
			break;
		}
	}
	return nil;
}

/**
existsコマンドハンドラ
*/
- (id)handleExistsScriptCommand:(MIExistsCommand*)command
{
	return @(YES);
}

/**
countコマンドハンドラ
*/
- (id)handleCountScriptCommand:(MICountCommand*)command
{
	//
	NSDictionary *args = [command evaluatedArguments];
	NSUInteger	objectType = [((NSNumber*)[args valueForKey:@"ObjectClass"]) intValue];
	AItemCount	count = 0;
	switch(objectType)
	{
	  case kASObjectCode_IndexGroup:
		{
			count = [self countOfIndexgroups];
			break;
		}
	  case kASObjectCode_IndexRecord:
		{
			count = [self countOfIndexrecords];
			break;
		}
	  default:
		{
			[CocoaApp appleScriptNotHandledError];
			break;
		}
	}
	return [NSNumber numberWithInteger:count];
}

/**
indexgroup count
*/
- (AItemCount)countOfIndexgroups
{
	return GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetGroupCount();
}

/**
indexrecord count
*/
- (AItemCount)countOfIndexrecords
{
	AIndex	count = 0;
	AItemCount	c = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetGroupCount();
	for( AIndex i = 0; i < c; i++ )
	{
		count += GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetItemCountInGroup(i);
	}
	return count;
}

/**
*/
- (id)handleSelectScriptCommand:(MIDeleteCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).NVI_RevealDocumentWithAView();
    return nil;
}

@end

#pragma mark ===========================
#pragma mark [クラス]CASIndexGroup
#pragma mark ===========================
@implementation CASIndexGroup

/**
init
*/
- (id)init:(IndexWindowDocument*)indexWindowDocument groupIndex:(AIndex)groupIndex
{
	self = [super init];
	if(self) 
	{
		//ドキュメント
		mIndexWindowDocument = indexWindowDocument;
		mDocumentID = [mIndexWindowDocument documentID];
		//group index設定
		mGroupIndex = groupIndex;
		//子elementのlazy object格納array生成
		mIndexRecordLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
	}
	return self;
}

/**
dealloc
*/
- (void)dealloc
{
	//子elementのlazy object格納array解放
	[mIndexRecordLazyObjectsArray release];
	//継承処理
	[super dealloc];
}

/**
DocumentID取得
*/
- (ADocumentID)documentID
{
	return mDocumentID;
}

/**
group index取得
*/
- (ATextIndex)groupIndex;
{
	return mGroupIndex;
}

/**
IndexWindowDocument取得
*/
- (IndexWindowDocument*)indexWindowDocument
{
	return mIndexWindowDocument;
}

#pragma mark ===========================

#pragma mark --- element

/**
specifierで指定された子elementのindex（子element array内でのindex）を取得する
CASIndexWindowDoc::GetSubModelByPosition()に対応
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifierで指定の場合 ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//
		AIndex	index = [indexSpec index];
		// ---------- indexrecord ----------
		if( [[indexSpec key] isEqual:@"asIndexrecordElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfIndexrecords];
				}
				//Lazyオブジェクト生成(autorelease)
				CASIndexRecord*	obj = [[CASIndexRecord alloc] init:mIndexWindowDocument groupIndex:mGroupIndex itemIndex:index];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	lazyIndex = [mIndexRecordLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				[CocoaApp appleScriptNoSuchObjectError:@"The indexrecord not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- 上記以外オブジェクトの場合はエラー ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else
	{
		// ==================== 上記以外で指定の場合はエラー ====================
		// プロパティ取得の場合もここに来るので、エラーにせず、nilを返す
		return nil;
	}
}

/**
indexrecord  element (lazy object array)取得
*/
- (NSArray *)asIndexrecordElement
{
	return [mIndexRecordLazyObjectsArray array];
}

#pragma mark ===========================

#pragma mark --- property

/**
content取得
*/
-(NSAppleEventDescriptor*)asContent
{
	NSAppleEventDescriptor *aeDescList = [NSAppleEventDescriptor listDescriptor];
	AItemCount	count = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetItemCountInGroup(mGroupIndex);
	for( AIndex itemIndex = 0; itemIndex < count; itemIndex++ )
	{
		NSAppleEventDescriptor*	desc = [mIndexWindowDocument getDescFromItem:mGroupIndex itemIndex:itemIndex];
		[aeDescList insertDescriptor:desc atIndex:itemIndex+1];
	}
	return aeDescList;
}

/**
content設定
*/
- (void)setAsContent:(id)value
{
	NSAppleEventDescriptor*	aeDescList = (NSAppleEventDescriptor*)value;
	//グループ内のindexrecordを全削除
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_DeleteAllItemsInGroup(mGroupIndex);
	//引数の各要素ごとのループ
	NSInteger count = [aeDescList numberOfItems];
	for( AIndex i = 0; i < count ;i++ )
	{
		//要素取得
		NSAppleEventDescriptor*	aeDesc = [aeDescList descriptorAtIndex:i+1];
		//indexrecord追加
		GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).
				SPI_InsertItem_TextPosition(mGroupIndex,i,
											GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),
											GetEmptyText(),//#465
											0,0,0,GetEmptyText());
		//indexrecordの値設定
		[mIndexWindowDocument setItemFromDesc:mGroupIndex itemIndex:i ae:aeDesc];
	}
}

// -------------------- index プロパティ --------------------

/**
index取得
*/
- (id)asIndex
{
	AIndex	index = kIndex_Invalid;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(mGroupIndex,kIndex_Invalid,index);
	return [NSAppleEventDescriptor descriptorWithInt32:index+1];
}

// -------------------- comment プロパティ --------------------

/**
comment取得
*/
- (id)asComment
{
	AText	title1, title2;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetGroupTitle(mGroupIndex,title1,title2);
	AText	text;
	text.SetText(title1);
	text.AddText(title2);
	//
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
comment設定
*/
- (void)setAsComment:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	AText	text;
	text.SetFromNSString([aeDesc stringValue]);
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SetGroupTitle(mGroupIndex,text,GetEmptyText());
}

#pragma mark ===========================

#pragma mark --- command handler

/**
existsコマンドハンドラ
*/
- (id)handleExistsScriptCommand:(MIExistsCommand*)command
{
	return @(YES);
}

/**
deleteコマンドハンドラ
*/
- (id)handleDeleteScriptCommand:(MIDeleteCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_DeleteGroup(mGroupIndex);
	return nil;
}

/**
selectコマンドハンドラ
*/
- (id)handleSelectScriptCommand:(MIDeleteCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SelectGroup(mGroupIndex);
	return nil;
}

/**
collapseコマンドハンドラ
*/
- (id)handleCollapseScriptCommand:(MICollapseCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_Collapse(mGroupIndex);
	return nil;
}

/**
expandコマンドハンドラ
*/
- (id)handleExpandScriptCommand:(MIExpandCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_Expand(mGroupIndex);
	return nil;
}

/**
countコマンドハンドラ
*/
- (id)handleCountScriptCommand:(MICountCommand*)command
{
	//
	NSDictionary *args = [command evaluatedArguments];
	NSUInteger	objectType = [((NSNumber*)[args valueForKey:@"ObjectClass"]) intValue];
	AItemCount	count = 0;
	switch(objectType)
	{
	  case kASObjectCode_IndexRecord:
		{
			count = [self countOfIndexrecords];
			break;
		}
	  default:
		{
			[CocoaApp appleScriptNotHandledError];
			break;
		}
	}
	return [NSNumber numberWithInteger:count];
}

/**
indexrecord count
*/
- (AItemCount)countOfIndexrecords
{
	return GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetItemCountInGroup(mGroupIndex);
}

@end

#pragma mark ===========================
#pragma mark [クラス]CASIndexRecord
#pragma mark ===========================
@implementation CASIndexRecord

/**
init
*/
- (id)init:(IndexWindowDocument*)indexWindowDocument groupIndex:(AIndex)groupIndex itemIndex:(AIndex)itemIndex
{
	self = [super init];
	if(self) 
	{
		//ドキュメント
		mIndexWindowDocument = indexWindowDocument;
		mDocumentID = [mIndexWindowDocument documentID];
		//group index, item index設定
		mGroupIndex = groupIndex;
		mItemIndex = itemIndex;
	}
	return self;
}

/**
DocumentID取得
*/
- (ADocumentID)documentID
{
	return mDocumentID;
}

/**
group index取得
*/
- (ATextIndex)groupIndex;
{
	return mGroupIndex;
}

/**
item index取得
*/
- (ATextIndex)itemIndex;
{
	return mItemIndex;
}

/**
IndexWindowDocument取得
*/
- (IndexWindowDocument*)indexWindowDocument
{
	return mIndexWindowDocument;
}

#pragma mark ===========================

#pragma mark --- property

/**
content取得
*/
-(NSAppleEventDescriptor*)asContent
{
	return [mIndexWindowDocument getDescFromItem:mGroupIndex itemIndex:mItemIndex];
}

/**
content設定
*/
- (void)setAsContent:(id)value
{
	NSAppleEventDescriptor*	desc = (NSAppleEventDescriptor*)value;
	[mIndexWindowDocument setItemFromDesc:mGroupIndex itemIndex:mItemIndex ae:desc];
}

// -------------------- index プロパティ --------------------

/**
index取得
*/
- (id)asIndex
{
	AIndex	index = kIndex_Invalid;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(mGroupIndex,mItemIndex,index);
	return [NSAppleEventDescriptor descriptorWithInt32:index+1];
}

// -------------------- file プロパティ --------------------

/**
file取得
*/
- (id)asFile
{
	AIndex	ASIndex;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(mGroupIndex,mItemIndex,ASIndex);
	AFileAcc	file;
	ATextIndex	spos;
	AItemCount	len;
	AText	comment;
	ANumber	para;
	if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetRecord(ASIndex,file,spos,len,para,comment) == false )
	{
		[CocoaApp appleScriptNotHandledError];
		return nil;
	}
	//
	if( file.Exist() == true )
	{
		AText	path;
		file.GetPath(path);
		NSURL*	url =  [NSURL fileURLWithPath:path.CreateNSString(true) isDirectory:NO];
		return [NSAppleEventDescriptor descriptorWithURL:url];
	}
	else
	{
		return nil;
	}
}

/**
file設定
*/
- (void)setAsFile:(id)value
{
	//
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	NSURL*	url = [aeDesc urlValue];
	if( url == nil )
	{
		[CocoaApp appleScriptNotHandledError];
		return;
	}
	AText	filepath;
	filepath.SetCstring([[url path] UTF8String]);
	AFileAcc	file;
	file.Specify(filepath);
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SetItemFile(mGroupIndex,mItemIndex,file);
}

// -------------------- startposition プロパティ --------------------

/**
startposition取得
*/
- (id)asStartposition
{
	AIndex	ASIndex;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(mGroupIndex,mItemIndex,ASIndex);
	AFileAcc	file;
	ATextIndex	spos;
	AItemCount	len;
	AText	comment;
	ANumber	para;
	if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetRecord(ASIndex,file,spos,len,para,comment) == false )
	{
		[CocoaApp appleScriptNotHandledError];
		return nil;
	}
	//
	return [NSAppleEventDescriptor descriptorWithInt32:spos+1];
}

// -------------------- endposition プロパティ --------------------

/**
endposition取得
*/
- (id)asEndposition
{
	AIndex	ASIndex;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(mGroupIndex,mItemIndex,ASIndex);
	AFileAcc	file;
	ATextIndex	spos;
	AItemCount	len;
	AText	comment;
	ANumber	para;
	if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetRecord(ASIndex,file,spos,len,para,comment) == false )
	{
		[CocoaApp appleScriptNotHandledError];
		return nil;
	}
	//
	return [NSAppleEventDescriptor descriptorWithInt32:spos+1+len];
}

// -------------------- paragraph プロパティ --------------------

/**
paragraph取得
*/
- (id)asParagraph
{
	AIndex	ASIndex;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(mGroupIndex,mItemIndex,ASIndex);
	AFileAcc	file;
	ATextIndex	spos;
	AItemCount	len;
	AText	comment;
	ANumber	para;
	if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetRecord(ASIndex,file,spos,len,para,comment) == false )
	{
		[CocoaApp appleScriptNotHandledError];
		return nil;
	}
	//
	return [NSAppleEventDescriptor descriptorWithInt32:para+1];
}

// -------------------- comment プロパティ --------------------

/**
comment取得
*/
- (id)asComment
{
	AIndex	ASIndex;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(mGroupIndex,mItemIndex,ASIndex);
	AFileAcc	file;
	ATextIndex	spos;
	AItemCount	len;
	AText	comment;
	ANumber	para;
	if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetRecord(ASIndex,file,spos,len,para,comment) == false )
	{
		[CocoaApp appleScriptNotHandledError];
		return nil;
	}
	//
	return [NSAppleEventDescriptor descriptorWithString:comment.CreateNSString(true)];
}

/**
comment設定
*/
- (void)setAsComment:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	AText	text;
	text.SetFromNSString([aeDesc stringValue]);
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SetItemComment(mGroupIndex,mItemIndex,text);
}

#pragma mark ===========================

#pragma mark --- command handler

/**
existsコマンドハンドラ
*/
- (id)handleExistsScriptCommand:(MIExistsCommand*)command
{
	return @(YES);
}

/**
deleteコマンドハンドラ
*/
- (id)handleDeleteScriptCommand:(MIDeleteCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_DeleteItem(mGroupIndex,mItemIndex);
	return nil;
}

/**
selectコマンドハンドラ
*/
- (id)handleSelectScriptCommand:(MIDeleteCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_Select(mGroupIndex,mItemIndex);
	return nil;
}

@end



