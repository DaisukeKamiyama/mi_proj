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

AppleScript

*/

#include "AppleScript.h"
#include "CAppleEvent.h"
#include "AApp.h"
#include "ADocument_IndexWindow.h"
#include "../../AbsFramework/MacWrapper/ODBEditorSuite.h"
#include <CoreFoundation/CoreFoundation.h>
#import "../../AbsFramework/MacImp/AppDelegate.h"


#pragma mark ===========================
#pragma mark [クラス]NSAppleEventDescriptor (MIScripting)
#pragma mark ===========================
//NSAppleEventDescriptorの拡張

@implementation NSAppleEventDescriptor (MIScripting)

/**
NSURLからNSAppleEventDescriptorを生成
*/
+ (NSAppleEventDescriptor *)descriptorWithURL:(NSURL *)url
{
	NSData*     urlData = (NSData *)CFBridgingRelease((CFURLCreateData(NULL, (__bridge CFURLRef)(url), kCFStringEncodingUTF8, TRUE)));
	return [NSAppleEventDescriptor descriptorWithDescriptorType:typeFileURL data:urlData];
}

/**
NSAppleEventDescriptorからNSURLを取得
*/
- (NSURL*)urlValue
{
	NSURL*	url = nil;
	NSData      *data = [self data];
	switch ( [self descriptorType] )
	{
	  case typeFileURL:
		{
			url = (NSURL *)CFBridgingRelease(CFURLCreateWithBytes(NULL, (const UInt8*)[data bytes], [data length], 
																  kCFStringEncodingUTF8, NULL));
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return url;
}

/**
ARectからNSAppleEventDescriptorを生成
*/
+ (NSAppleEventDescriptor *)descriptorWithARect:(ARect)arect
{
	Rect	rect = {0};
	rect.left		= arect.left;
	rect.top		= arect.top;
	rect.right		= arect.right;
	rect.bottom		= arect.bottom;
	NSData*     data = [NSData dataWithBytes:(void*)&(rect) length:sizeof(rect)];
	return [NSAppleEventDescriptor descriptorWithDescriptorType:'qdrt' data:data];
}

/**
NSAppleEventDescriptorからARectを取得
*/
- (ARect)arectValue
{
	ARect	arect = {0};
	switch ( [self descriptorType] )
	{
	  case 'qdrt':
		{
			Rect	rect = {0};
			[[self data] getBytes:&rect length:sizeof(rect)];
			arect.left		= rect.left;
			arect.top		= rect.top;
			arect.right		= rect.right;
			arect.bottom	= rect.bottom;
			break;
		}
	  case 'list':
		{
			NSInteger	count = [self numberOfItems];
			if( count == 4 )
			{
				arect.left		= [[self descriptorAtIndex:1] int32Value];
				arect.top		= [[self descriptorAtIndex:2] int32Value];
				arect.right		= [[self descriptorAtIndex:3] int32Value];
				arect.bottom	= [[self descriptorAtIndex:4] int32Value];
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return arect;
}

/**
APointからNSAppleEventDescriptorを生成
*/
+ (NSAppleEventDescriptor *)descriptorWithAPoint:(APoint)apoint
{
	Point	point = {0};
	point.h			= apoint.x;
	point.v			= apoint.y;
	NSData*     data = [NSData dataWithBytes:(void*)&(point) length:sizeof(point)];
	return [NSAppleEventDescriptor descriptorWithDescriptorType:'QDpt' data:data];
}

@end


#pragma mark ===========================
#pragma mark [クラス]NSDictionary (MIScripting)
#pragma mark ===========================

@implementation NSDictionary (MIScripting)

/**
NSAppleEventDescriptorからNSDictionaryへ変換する
（AppleScriptでrecordを使用する場合、このメソッドが使用される。
*/
+ (id)scriptingRecordWithDescriptor:(NSAppleEventDescriptor *)inRecordDesc
{
	//結果格納用dictionary生成
	NSMutableDictionary *dic = [NSMutableDictionary dictionary];
	[[dic retain] autorelease];
	//"record"というキーでNSAppleEventDescriptor全体を格納する
	[dic setValue:inRecordDesc forKey:@"record"];
	return dic;
}

@end


#pragma mark ===========================
#pragma mark [クラス]LazyObjectsArray
#pragma mark ===========================
//LazyObjectを格納するリストのクラス
//同じコマンドを実行中はLazyObjectを保持し、別のコマンドになったら保持していたLazyObjectをreleaseする。

@implementation LazyObjectsArray

/**
init
*/
- (id)init:(NSObject*)ownerObject
{
	self = [super init];
	if(self) 
	{
		//このLazyObjectsArrayを保持するオブジェクト（TextDocument等）を記憶
		mOwnerObject = ownerObject;
		[mOwnerObject retain];
		//LazyObjectを格納するarrayを生成
		mNSMutableArray = [NSMutableArray array];
		[mNSMutableArray retain];//allocではない生成APIはautoreleaseされているので、retain必要
		//現在のコマンド格納用変数
		mCurrentCommand = nil;
	}
	return self;
}

/**
dealloc
*/
- (void)dealloc
{
	//このLazyObjectsArrayを保持するオブジェクトを解放（initでretainしているため）
	[mOwnerObject release];
	//arrayを解放
	[mNSMutableArray release];
	//継承処理
	[super dealloc];
}

/**
LazyObjectを登録
*/
- (NSInteger)registerLazyObject:(LazyObjectsArrayItem*)lazyObject
{
	//現在のコマンドを取得
	NSScriptCommand *currentScriptCommand = [NSScriptCommand currentCommand];
	//現在のコマンドと同じかどうかを判定
	if( currentScriptCommand != mCurrentCommand )
	{
		//現在コマンドと違う場合は、arrayから要素を全削除
		[mNSMutableArray removeAllObjects];
		//現在コマンドを更新
		mCurrentCommand = currentScriptCommand;
	}
	//arrayへ追加
	[mNSMutableArray addObject:lazyObject];
	//LazyObjectへ自身を設定
	[lazyObject setContainerArray:self];
	//array内のindexを返す
	return [mNSMutableArray count]-1;
}

/**
array取得
*/
- (NSMutableArray*)array
{
	return mNSMutableArray;
}

/**
このLazyObjectsArrayを保持するオブジェクトのobject specifierを取得
*/
- (NSScriptObjectSpecifier*)ownerObjectSpecifier
{
	return [mOwnerObject objectSpecifier];
}

@end

#pragma mark ===========================
#pragma mark [クラス]LazyObjectsArrayItem
#pragma mark ===========================
//LazyObjectの基底クラス

@implementation LazyObjectsArrayItem

/**
格納されているarrayを設定
*/
- (void)setContainerArray:(LazyObjectsArray*)containerArray
{
	mContainerArray = containerArray;
}

/**
格納されているarrayを取得
*/
- (LazyObjectsArray*)containerArray
{
	return mContainerArray;
}

/**
格納されているarray内でのindexを取得
*/
- (NSInteger)indexInContainerArray
{
	return [mContainerArray indexOfObject:self];
}

@end


#pragma mark ===========================
#pragma mark [クラス]MIScriptCommand
#pragma mark ===========================

@implementation MIScriptCommand

@end


#pragma mark ===========================
#pragma mark [クラス]MIGetCommand
#pragma mark ===========================
//getコマンド

@implementation MIGetCommand

/**
デフォルトコマンド処理
*/
-(id)performDefaultImplementation
{
	try
	{
		//direct parameter取得
		NSScriptObjectSpecifier*	directParameter = [self directParameter];
		//key取得
		NSString*	key = [directParameter key];
		//property取得かelement取得かどうかの判定
		if( [directParameter isKindOfClass:[NSPropertySpecifier class]] == YES )
		{
			//property取得の場合、container specifierからcontainerオブジェクトを取得し、valueForKeyメソッドを実行する。
			NSScriptObjectSpecifier*	container = [directParameter containerSpecifier];
			id	obj = [container objectsByEvaluatingSpecifier];
			id result = [obj valueForKey:key];
			return result;
		}
		else
		{
			//element取得の場合、それぞれのオブジェクトのasContentを実行して、contentを取得。
			id	obj = [directParameter objectsByEvaluatingSpecifier];
			if( [obj isKindOfClass:[TextDocument class]] == YES )
			{
				// -------------------- document --------------------
				TextDocument*	textDocument = (TextDocument*)obj;
				return [textDocument asContent];
			}
			else if( [obj isKindOfClass:[CASTextDocElement class]] == YES )
			{
				// -------------------- paragraph, word, ... --------------------
				CASTextDocElement*	docElement = (CASTextDocElement*)obj;
				return [docElement asContent];
			}
			else if( [obj isKindOfClass:[IndexWindowDocument class]] == YES )
			{
				// -------------------- indexwindow --------------------
				IndexWindowDocument*	doc = (IndexWindowDocument*)obj;
				return [doc asContent];
			}
			else if( [obj isKindOfClass:[CASIndexGroup class]] == YES )
			{
				// -------------------- indexgroup --------------------
				CASIndexGroup*	element = (CASIndexGroup*)obj;
				return [element asContent];
			}
			else if( [obj isKindOfClass:[CASIndexRecord class]] == YES )
			{
				// -------------------- indexrecord --------------------
				CASIndexRecord*	element = (CASIndexRecord*)obj;
				return [element asContent];
			}
			else
			{
				//上記オブジェクト以外の場合、エラー
				[CocoaApp appleScriptNotHandledError];
				return nil;
			}
		}
	}
	catch(...)
	{
	}
	return nil;
}

@end


#pragma mark ===========================
#pragma mark [クラス]MISetCommand
#pragma mark ===========================
//setコマンド

@implementation MISetCommand

/**
デフォルトコマンド処理
*/
-(id)performDefaultImplementation
{
	try
	{
		//direct parameter取得
		NSScriptObjectSpecifier*	directParameter = [self directParameter];
		//key取得
		NSString*	key = [directParameter key];
		//引数取得
		NSDictionary *args = [self evaluatedArguments];
		if( args != nil )
		{
			//引数"to"の値を取得
			NSAppleEventDescriptor*	value = [args valueForKey:@"to"];
			if( value != nil )
			{
				//property設定かelement設定かどうかの判定
				if( [directParameter isKindOfClass:[NSPropertySpecifier class]] == YES )
				{
					//property設定の場合、container specifierからcontainerオブジェクトを取得し、setValueメソッドを実行する。
					NSScriptObjectSpecifier*	container = [directParameter containerSpecifier];
					id	obj = [container objectsByEvaluatingSpecifier];
					[obj setValue:value forKey:key];
				}
				else
				{
					//element設定の場合、それぞれのオブジェクトのasContentを実行して、contentを設定。
					id	obj = [directParameter objectsByEvaluatingSpecifier];
					if( [obj isKindOfClass:[TextDocument class]] == YES )
					{
						// -------------------- document --------------------
						TextDocument*	textDocument = (TextDocument*)obj;
						[textDocument setAsContent:value];
					}
					else if( [obj isKindOfClass:[CASTextDocElement class]] == YES )
					{
						// -------------------- paragraph, word, ... --------------------
						CASTextDocElement*	docElement = (CASTextDocElement*)obj;
						[docElement setAsContent:value];
					}
					else if( [obj isKindOfClass:[IndexWindowDocument class]] == YES )
					{
						// -------------------- indexwindow --------------------
						IndexWindowDocument*	doc = (IndexWindowDocument*)obj;
						[doc setAsContent:value];
					}
					else if( [obj isKindOfClass:[CASIndexGroup class]] == YES )
					{
						// -------------------- indexgroup --------------------
						CASIndexGroup*	element = (CASIndexGroup*)obj;
						[element setAsContent:value];
					}
					else if( [obj isKindOfClass:[CASIndexRecord class]] == YES )
					{
						// -------------------- indexrecord --------------------
						CASIndexRecord*	element = (CASIndexRecord*)obj;
						[element setAsContent:value];
					}
					else
					{
						//上記オブジェクト以外の場合、エラー
						[CocoaApp appleScriptNotHandledError];
					}
				}
			}
			else
			{
				//toが無い場合、エラー
				[CocoaApp appleScriptNotHandledError];
			}
		}
	}
	catch(...)
	{
	}
	return nil;
}

@end


#pragma mark ===========================
#pragma mark [クラス]MICreateCommand
#pragma mark ===========================
//make newコマンド

@implementation MICreateCommand

/**
デフォルトコマンド処理
CASApp::HandleCreateElementEvent, CASTextDoc::HandleCreateElementEvent(), CASIndexDoc::HandleCreateElementEventに対応する処理
*/
-(id)performDefaultImplementation
{
	try
	{
		//引数取得
		NSDictionary *args = [self evaluatedArguments];
		if( args != nil )
		{
			//設定テキスト格納変数
			AText	text;
			//生成するオブジェクトタイプを取得
			NSNumber*	objectTypeObj = (NSNumber*)[args valueForKey:@"ObjectClass"];
			if( objectTypeObj == nil )
			{
				//生成するオブジェクトタイプが指定されていない場合はエラー
				[CocoaApp appleScriptNotHandledError];
				return nil;
			}
			NSUInteger	objectType = [objectTypeObj intValue];
			//引数at（生成位置）取得
			NSPositionalSpecifier*	loc = [args valueForKey:@"Location"];
			//引数with data取得
			NSAppleEventDescriptor*	data = [args valueForKey:@"ObjectData"];
			if( data != nil )
			{
				//テキスト取得
				NSString*	data_str = [data stringValue];
				if( data_str != nil )
				{
					text.SetFromNSString(data_str);
				}
			}
			//引数with properties {}取得
			NSDictionary*	propDic = [args valueForKey:@"KeyDictionary"];
			
			// ==================== indexwindow生成 ====================
			if( objectType == kASObjectCode_IndexWindow )
			{
				//新規ドキュメント生成
				AText	wintitle;
				wintitle.SetLocalizedText("IndexWindow_Title_New");
				ADocumentID	docID = GetApp().SPI_GetOrCreateFindResultWindowDocument();
				if( docID != kObjectID_Invalid )
				{
					AObjectID	docImpID = GetApp().NVI_GetDocumentByID(docID).NVI_GetDocImpID();
					if( docImpID != kObjectID_Invalid )
					{
						IndexWindowDocument*	doc = (IndexWindowDocument*)(GetApp().GetImp().GetDocImpByID(docImpID).GetNSDocument());
						//with propertiesが設定されている場合は、それぞれについてsetValueを実行する
						if( propDic != nil )
						{
							[self setProperties:[propDic valueForKey:@"record"] forObject:doc];
						}
						//
						AText	grouptitle;
						grouptitle.SetLocalizedText("IndexWindow_NewGroupName");
						AFileAcc	basefolder;//#465（ここはベースフォルダ未対応）
						GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_AddGroup(grouptitle,GetEmptyText(),basefolder);
						return [doc objectSpecifier];
					}
				}
				return nil;
			}
			
			// ==================== document生成 ====================
			if( objectType == kASObjectCode_Document )
			{
				//新規ドキュメント生成
				ADocumentID	docID = GetApp().SPNVI_CreateNewTextDocument();
				if( docID != kObjectID_Invalid )
				{
					AObjectID	docImpID = GetApp().NVI_GetDocumentByID(docID).NVI_GetDocImpID();
					if( docImpID != kObjectID_Invalid )
					{
						TextDocument*	doc = (TextDocument*)(GetApp().GetImp().GetDocImpByID(docImpID).GetNSDocument());
						//text設定
						text.ConvertLineEndToCR();
						GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,text);
						//with propertiesが設定されている場合は、それぞれについてsetValueを実行する
						if( propDic != nil )
						{
							[self setProperties:[propDic valueForKey:@"record"] forObject:doc];
						}
						return [doc objectSpecifier];
					}
				}
				return nil;
			}
			
			// ==================== indexrecord/indexgroup生成 ====================
			if( objectType == kASObjectCode_IndexRecord || objectType == kASObjectCode_IndexGroup )
			{
				AIndex	insertGroupIndex = kIndex_Invalid;
				AIndex	insertItemIndex = kIndex_Invalid;
				if( loc != nil )
				{
					ADocumentID	docID = kObjectID_Invalid;
					IndexWindowDocument*	doc = nil;
					//引数locの対象オブジェクト取得
					id	obj = [[loc objectSpecifier] objectsByEvaluatingSpecifier];
					if( [obj isKindOfClass:[CASIndexRecord class]] == YES )
					{
						CASIndexRecord*	indexRecordElement = (CASIndexRecord*)obj;
						//DocumentID取得
						docID = [indexRecordElement documentID];
						//IndexWindowDocument取得
						doc = [indexRecordElement indexWindowDocument];
						//
						AIndex	groupIndex = [indexRecordElement groupIndex];
						AIndex	itemIndex = [indexRecordElement itemIndex];
						//生成位置取得
						switch([loc position])
						{
							//beginning of
						  case NSPositionBeginning:
							//before
						  case NSPositionBefore:
							{
								insertGroupIndex = groupIndex;
								insertItemIndex = itemIndex;
								break;
							}
							//end of
						  case NSPositionEnd:
							//after
						  case NSPositionAfter:
							{
								insertGroupIndex = groupIndex;
								insertItemIndex = itemIndex+1;
								break;
							}
						}
					}
					else if(  [obj isKindOfClass:[CASIndexGroup class]] == YES )
					{
						CASIndexGroup*	indexGroupElement = (CASIndexGroup*)obj;
						//DocumentID取得
						docID = [indexGroupElement documentID];
						//IndexWindowDocument取得
						doc = [indexGroupElement indexWindowDocument];
						//
						AIndex	groupIndex = [indexGroupElement groupIndex];
						//生成位置取得
						switch([loc position])
						{
							//beginning of
						  case NSPositionBeginning:
							{
								insertGroupIndex = groupIndex;
								insertItemIndex = 0;
								break;
							}
							//before
						  case NSPositionBefore:
							{
								insertGroupIndex = groupIndex-1;
								if( insertGroupIndex < 0 ) 
								{
									insertGroupIndex = 0;
									insertItemIndex = 0;
								}
								else
								{
									insertItemIndex = GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_GetItemCountInGroup(groupIndex);
								}
								break;
							}
							//end of
						  case NSPositionEnd:
							{
								insertGroupIndex = groupIndex;
								insertItemIndex = GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_GetItemCountInGroup(groupIndex);
								break;
							}
							//after
						  case NSPositionAfter:
							{
								insertGroupIndex = groupIndex+1;
								if( insertGroupIndex >= GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_GetGroupCount() )
								{
									insertGroupIndex = GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_GetGroupCount();
									insertItemIndex = GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_GetItemCountInGroup(groupIndex);
								}
								else
								{
									insertItemIndex = 0;
								}
								break;
							}
						}
					}
					//位置が不正の場合はエラー
					if( insertGroupIndex == kIndex_Invalid )
					{
						[self setScriptErrorNumber:-1708];
						[self setScriptErrorString:@"Argument 'at' is invalid position."];
					}
					
					//生成
					switch(objectType)
					{
						//indexrecord
					  case kASObjectCode_IndexRecord:
						{
							//indexrecord生成
							GetApp().SPI_GetIndexWindowDocumentByID(docID).
									SPI_InsertItem_TextPosition(insertGroupIndex,insertItemIndex,
																GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),
																GetEmptyText(),//#465
																0,0,0,GetEmptyText());
							//with propertiesが設定されている場合は設定
							if( propDic != nil )
							{
								NSAppleEventDescriptor*	rec = [propDic valueForKey:@"record"];
								[doc setItemFromDesc:insertGroupIndex itemIndex:insertItemIndex ae:rec];
							}
							//生成したcharacterに対応するNSIndexSpecifierを返す（container: index: ）
							AIndex	ASIndex = kIndex_Invalid;
							GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_GetASRecordIndex(insertGroupIndex,insertItemIndex,ASIndex);
							return [[[NSIndexSpecifier alloc] 
									initWithContainerClassDescription:[[doc objectSpecifier] keyClassDescription]
							containerSpecifier:[doc objectSpecifier] key:@"asIndexrecordElement" index:ASIndex+1] autorelease];
							break;
						}
						//indexgroup
					  case kASObjectCode_IndexGroup:
						{
							//indexgroup生成
							AFileAcc	basefolder;//#465
							GetApp().SPI_GetIndexWindowDocumentByID(docID).
									SPI_InsertGroup(insertGroupIndex,GetEmptyText(),GetEmptyText(),basefolder);//#465
							//Lazyオブジェクト生成(autorelease)
							CASIndexGroup*	groupObj = [[CASIndexGroup alloc] init:doc groupIndex:insertGroupIndex];
							[groupObj autorelease];
							//with propertiesが設定されている場合は、それぞれについてsetValueを実行する
							if( propDic != nil )
							{
								[self setProperties:[propDic valueForKey:@"record"] forObject:groupObj];
							}
							//生成したcharacterに対応するNSIndexSpecifierを返す（container: index: ）
							return [[[NSIndexSpecifier alloc] 
									initWithContainerClassDescription:[[doc objectSpecifier] keyClassDescription]
							containerSpecifier:[doc objectSpecifier] key:@"asIndexgroupElement" index:insertGroupIndex+1] autorelease];
							break;
						}
					}
				}
			}
			
			// ==================== 各種text element生成 ====================
			//生成位置を格納する変数
			ATextIndex	pos = kIndex_Invalid;
			if( loc != nil )
			{
				//引数locの対象オブジェクト取得
				id	obj = [[loc objectSpecifier] objectsByEvaluatingSpecifier];
				if( [obj isKindOfClass:[CASTextDocElement class]] == YES )
				{
					CASTextDocElement*	textDocElement = (CASTextDocElement*)obj;
					//with propertiesのうち、contentを取得（他は対応しない。ヘルプに記述済み）
					if( propDic != nil )
					{
						NSAppleEventDescriptor*	rec = [propDic valueForKey:@"record"];
						//
						NSAppleEventDescriptor*	content = [rec descriptorForKeyword:pContent];
						if( content != nil )
						{
							text.SetFromNSString([content stringValue]);
						}
					}
					//生成位置取得
					pos = [textDocElement startTextIndex];
					//DocumentID取得
					ADocumentID	docID = [textDocElement documentID];
					//TextDocument取得
					TextDocument*	doc = [textDocElement textDocument];
					//生成位置取得
					switch([loc position])
					{
						//beginning of
					  case NSPositionBeginning:
						{
							pos = [textDocElement startTextIndex];
							break;
						}
						//end of
					  case NSPositionEnd:
						{
							pos = [textDocElement endTextIndex];
							//paragraphの場合は、改行コードの前にする
							if( [textDocElement isKindOfClass:[CASParagraph class]] == YES )
							{
								if( pos > 0 )
								{
									if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextChar(pos-1) == kUChar_LineEnd )
									{
										pos--;
									}
								}
							}
							break;
						}
						//before
					  case NSPositionBefore:
						{
							pos = [textDocElement startTextIndex];
							//paragraphの場合は、改行コードの前にする
							if( [textDocElement isKindOfClass:[CASParagraph class]] == YES )
							{
								if( pos > 0 )
								{
									pos--;
								}
							}
							break;
						}
						//after
					  case NSPositionAfter:
						{
							pos = [textDocElement endTextIndex];
							break;
						}
					}
					//位置が不正の場合はエラー
					if( pos == kIndex_Invalid )
					{
						[self setScriptErrorNumber:-1708];
						[self setScriptErrorString:@"Argument 'at' is invalid position."];
					}
					
					//Undoアクションタグ記録
					GetApp().SPI_GetTextDocumentByID(docID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
					//生成
					switch(objectType)
					{
						//paragraph
					  case kASObjectCode_Paragraph:
						{
							//改行文字
							AText	lineend;
							lineend.Add(kUChar_LineEnd);
							//textが指定されているかどうかの判定
							if( text.GetItemCount() == 0 )
							{
								//text未指定の場合は、改行文字を挿入
								GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(pos,lineend);
								//paragraph index取得
								ATextPoint	textpoint = {0};
								GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextPointFromTextIndex(pos,textpoint,true);
								AIndex	paraIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphIndexByLineIndex(textpoint.y);
								//生成したparagraphに対応するNSIndexSpecifierを返す（container:document index:段落index）
								return [[[NSIndexSpecifier alloc] 
										initWithContainerClassDescription:[[doc objectSpecifier] keyClassDescription]
								containerSpecifier:[doc objectSpecifier] key:@"asParagraphElement" index:paraIndex] autorelease];
							}
							else
							{
								//text指定の場合は、そのtextを挿入（改行を調整）
								if( pos >= 1 )
								{
									if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextChar(pos-1) != kUChar_LineEnd )
									{
										GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(pos,lineend);
										pos++;
									}
								}
								if( pos < GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )
								{
									if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextChar(pos) != kUChar_LineEnd )
									{
										GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(pos,lineend);
									}
								}
								else
								{
									GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(pos,lineend);
								}
								GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(pos,text);
								//paragraph index取得
								ATextPoint	textpoint = {0};
								GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextPointFromTextIndex(pos+text.GetItemCount()+1,textpoint,true);
								AIndex	paraIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphIndexByLineIndex(textpoint.y);
								//生成したparagraphに対応するNSIndexSpecifierを返す（container:document index:段落index）
								return [[[NSIndexSpecifier alloc] 
										initWithContainerClassDescription:[[doc objectSpecifier] keyClassDescription]
								containerSpecifier:[doc objectSpecifier] key:@"asParagraphElement" index:paraIndex] autorelease];
							}
							break;
						}
						//word
					  case kASObjectCode_Word:
						{
							//text未指定の場合はエラー
							if( text.GetItemCount() == 0 )
							{
								[CocoaApp appleScriptNotHandledError];
								break;
							}
							//text挿入（スペース調整）
							AText	space;
							space.Add(kUChar_Space);
							if( pos > 0 )
							{
								if( GetApp().SPI_GetTextDocumentByID(docID).SPI_IsAsciiAlphabet(pos-1) == true )
								{
									GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(pos,space);
									pos++;
								}
							}
							if( pos < GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )
							{
								if( GetApp().SPI_GetTextDocumentByID(docID).SPI_IsAsciiAlphabet(pos) == true )
								{
									GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(pos,space);
								}
							}
							GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(pos,text);
							//word index取得
							AIndex	wordIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_ASGetWordCount(0,pos);
							//生成したwordに対応するNSIndexSpecifierを返す（container:document index:word index）
							return [[[NSIndexSpecifier alloc] 
									initWithContainerClassDescription:[[doc objectSpecifier] keyClassDescription]
							containerSpecifier:[doc objectSpecifier] key:@"asWordElement" index:wordIndex] autorelease];
							break;
						}
						//character
					  case kASObjectCode_Character:
						{
							//text挿入
							GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(pos,text);
							//character index取得
							AIndex	charIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_ASGetCharCount(0,pos);
							//生成したcharacterに対応するNSIndexSpecifierを返す（container:document index:character index）
							return [[[NSIndexSpecifier alloc] 
									initWithContainerClassDescription:[[doc objectSpecifier] keyClassDescription]
							containerSpecifier:[doc objectSpecifier] key:@"asCharacterElement" index:charIndex] autorelease];
							break;
						}
					}
				}
				else
				{
					//引数locの対象オブジェクトがtext document elementでなければエラー
					[CocoaApp appleScriptNotHandledError];
				}
			}
			else
			{
				//引数atが未指定の場合はエラー
				[CocoaApp appleScriptNotHandledError];
			}
		}
	}
	catch(...)
	{
	}
	return nil;
}

/**
with propertiesで設定された値を設定する
*/
-(void)setProperties:(NSAppleEventDescriptor*)props forObject:(NSObject*)obj
{
	NSInteger count = [props numberOfItems];
	for( NSInteger i = 1; i <= count; i++ )
	{
		NSString*	keyString = nil;
		AEKeyword	keyword = [props keywordForDescriptorAtIndex:i];
		switch(keyword)
		{
		  case 'pcnt':
			{
				keyString = @"asContent";
				break;
			}
		  case 'file':
			{
				keyString = @"asFile";
				break;
			}
		  case 'pnam':
			{
				keyString = @"asName";
				break;
			}
		  case 'imod':
			{
				keyString = @"asModified";
				break;
			}
		  case 'pmod':
			{
				keyString = @"asMode";
				break;
			}
		  case 'pchc':
			{
				keyString = @"asTextEncoding";
				break;
			}
		  case 'prtc':
			{
				keyString = @"asReturnCode";
				break;
			}
		  case 'pftp':
			{
				keyString = @"asType";
				break;
			}
		  case 'pfcr':
			{
				keyString = @"asCreator";
				break;
			}
		  case 'pwin':
			{
				keyString = @"asWindowindex";
				break;
			}
		  case 'pidx':
			{
				keyString = @"asIndex";
				break;
			}
		  case 'font':
			{
				keyString = @"asFont";
				break;
			}
		  case 'ptsz':
			{
				keyString = @"asSize";
				break;
			}
		  case 'psel':
			{
				keyString = @"asSelection";
				break;
			}
		  case 'comm':
			{
				keyString = @"asComment";
				break;
			}
		  default:
			{
				keyString = nil;
				break;
			}
		}
		//
		if( keyString != nil )
		{
			[obj setValue:[props descriptorForKeyword:keyword] forKey:keyString];
		}
	}
}

@end

#pragma mark ===========================
#pragma mark [クラス]MIOpenCommand
#pragma mark ===========================
//openコマンド

@implementation MIOpenCommand

/**
デフォルトコマンド処理
*/
-(id)performDefaultImplementation
{
	return nil;
}

@end

#pragma mark ===========================
#pragma mark [クラス]MIQuitCommand
#pragma mark ===========================
//quitコマンド

@implementation MIQuitCommand

/**
デフォルトコマンド処理
*/
-(id)performDefaultImplementation
{
	try
	{
		GetApp().NVI_TryQuit();//#1102
	}
	catch(...)
	{
	}
	return nil;
}

@end

#pragma mark ===========================
#pragma mark [クラス]MICountCommand
#pragma mark ===========================
//countコマンド

@implementation MICountCommand

/**
デフォルトコマンド処理
*/
-(id)performDefaultImplementation
{
	return nil;
}

@end

#pragma mark ===========================
#pragma mark [クラス]MIExistsCommand
#pragma mark ===========================
//existsコマンド

@implementation MIExistsCommand

/**
デフォルトコマンド処理
*/
-(id)performDefaultImplementation
{
	return @(NO);
}

@end

#pragma mark ===========================
#pragma mark [クラス]MIDeleteCommand
#pragma mark ===========================
//deleteコマンド

@implementation MIDeleteCommand

/**
デフォルトコマンド処理
*/
-(id)performDefaultImplementation
{
	return nil;
}

@end

#pragma mark ===========================
#pragma mark [クラス]MISelectCommand
#pragma mark ===========================
//selectコマンド

@implementation MISelectCommand

/**
デフォルトコマンド処理
*/
-(id)performDefaultImplementation
{
	return nil;
}

@end

#pragma mark ===========================
#pragma mark [クラス]MISaveCommand
#pragma mark ===========================
//saveコマンド

@implementation MISaveCommand

/**
デフォルトコマンド処理
*/
-(id)performDefaultImplementation
{
	return nil;
}

@end

#pragma mark ===========================
#pragma mark [クラス]MICloseCommand
#pragma mark ===========================
//closeコマンド

@implementation MICloseCommand

/**
デフォルトコマンド処理
*/
-(id)performDefaultImplementation
{
	return nil;
}

@end

#pragma mark ===========================
#pragma mark [クラス]MICollapseCommand
#pragma mark ===========================
//collapseコマンド

@implementation MICollapseCommand

/**
デフォルトコマンド処理
*/
-(id)performDefaultImplementation
{
	return nil;
}

@end

#pragma mark ===========================
#pragma mark [クラス]MIExpandCommand
#pragma mark ===========================
//expandコマンド

@implementation MIExpandCommand

/**
デフォルトコマンド処理
*/
-(id)performDefaultImplementation
{
	return nil;
}

@end



#if CARBON_APPLESCRIPT

#include "AppleScript.h"
#include "CAppleEvent.h"
#include "AApp.h"
#include "ADocument_IndexWindow.h"
//#include "../Abs/AUtil.h"
#include "../../AbsFramework/MacAppleEvent/PowerPlant/UAppleEventsMgr.h"
#include "../../AbsFramework/MacAppleEvent/PowerPlant/UExtractFromAEDesc.h"
#include "../../AbsFramework/MacAppleEvent/PowerPlant/LModelDirector.h"
#include "../../AbsFramework/MacWrapper/ODBEditorSuite.h"

// === AppleEvent Numbers ===

// Required Suite
const long	ae_OpenApp			= 1001;
const long	ae_OpenDoc			= 1002;
const long	ae_PrintDoc			= 1003;
const long	ae_Quit				= 1004;
const long	ae_ReopenApp		= 1005;
const long	ae_Preferences		= 1006;	// Mac OS X Preferences menu item

// Core Suite
const long	ae_Clone			= 2001;
const long	ae_Close			= 2002;
const long	ae_CountElements	= 2003;
const long	ae_CreateElement	= 2004;
const long	ae_Delete			= 2005;
const long	ae_DoObjectsExist	= 2006;
const long	ae_GetClassInfo		= 2007;
const long	ae_GetData			= 2008;
const long	ae_GetDataSize		= 2009;
const long	ae_GetEventInfo		= 2010;
const long	ae_Move				= 2011;
const long	ae_Open				= 1002;		// Same as ae_OpenDoc
const long	ae_Print			= 1003;		// Same as ae_PrintDoc
const long	ae_Save				= 2012;
const long	ae_SetData			= 2013;

// Miscellaneous Standards
const long	ae_ApplicationDied	= 3001;
const long	ae_BeginTransaction	= 3002;
const long	ae_Copy				= 3003;
const long	ae_CreatePublisher	= 3004;
const long	ae_Cut				= 3005;
const long	ae_DoScript			= 3006;
const long	ae_EditGraphic		= 3007;
const long	ae_EndTransaction	= 3008;
const long	ae_ImageGraphic		= 3009;
const long	ae_IsUniform		= 3010;
const long	ae_MakeObjVisible	= 3011;
const long	ae_Paste			= 3012;
const long	ae_Redo				= 3013;
const long	ae_Revert			= 3014;
const long	ae_TransactionTerm	= 3015;
const long	ae_Undo				= 3016;
const long	ae_Select			= 3017;
const long	ae_SwitchTellTarget = 3018;
const long	ae_CommandLineArgs	= 3019;//#263

const long		ae_SelectObject						= 9500;
const long		ae_Exist							= 11010;
const long		ae_Collapse							= 11001;
const long		ae_Expand							= 11002;
const long		ae_Deactivate						= 11020;
const long		ae_Replace							= 11003;

enum 
{
	cIndexWindow	= 'idxw',
	cIndexRecord	= 'idxr',
	cIndexGroup		= 'idxg',
	pModified = 'imod',
	pMode	= 'pmod',
	pFileF	= 'file',
	pStartPositionF	= 'spos',
	pEndPositionF	= 'epos',
	pCommentF	= 'comm',
	pLineNumber = 'cpar',
	pCreator = 'pfcr',
	pType = 'pftp',
	pCharacterCode = 'pchc',
	pReturnCode = 'prtc',
	pFileWidth = 'fwid',
	pInfoWidth = 'iwid',
	pFileOrder = 'ford',
	pInfoOrder = 'iord',
	pPathType = 'path',
	pWindowIndex = 'pwin',
	pHeader = 'head',
	pIdleHookTime = 'idle',
	pAskSaving = 'asks',
	pFolderP = 'fold',
	pWindowPosition = 'ppos'
};

#pragma mark ===========================
#pragma mark [クラス]CASApp
#pragma mark ===========================

CASApp::CASApp() : LModelObject(NULL,cApplication)
{
	//#263
	/*OSErr err = */::AEInstallEventHandler('misc','args',
			::NewAEEventHandlerUPP(&PowerPlant::LModelDirector::AppleEventHandler),ae_CommandLineArgs,false);
}
CASApp::~CASApp()
{
}

void	CASApp::HandleAppleEvent( const AppleEvent &inAppleEvent, AppleEvent &outAEReply, AEDesc &outResult, long inAENumber)
{
	switch(inAENumber) 
	{
	  case ae_Exist:
		{
			Boolean	result = false;
			AECreateDesc(typeBoolean,(Ptr)(&result),sizeof(Boolean),&outResult);
			break;
		}
	  case ae_Deactivate:
		{
			//既に不要？
			break;
		}
	  case ae_OpenDoc:
		{
			//fprintf(stderr,"CASApp::HandleAppleEvent() case ae_OpenDoc - start\n");
			CAppleEventReader	ae(&inAppleEvent);
			//AEからデータ抽出
			CAEDesc	docList;
			ae.GetParamDesc(keyDirectObject,typeAEList,docList);
			AText	tecname;
			ae.GetParamText(keyCharacterCode,typeUTF8Text,tecname);
			
			//ODB//B0103
			AText	odbSenderToken;
			ABool	odbMode = false;
			OSType	odbServer;
			AText	odbCustomPath;
			if( ae.GetParamOSType(keyFileSender,odbServer) == true )
			{
				ae.GetParamText(keyFileSenderToken,typeUTF8Text,odbSenderToken);
				ae.GetParamText(keyFileCustomPath,typeUTF8Text,odbCustomPath);
				odbMode = true;
				//odbCustomPath.OutputToStderr();
			}
			
			//docListそれぞれ開く
			for( AIndex i = 0; i < docList.GetListItemCount(); i++ )
			{
				AFileAcc	file;
				docList.GetListItem(i,file);
				GetApp().SPNVI_CreateDocumentFromLocalFile(file,tecname);
				ADocumentID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,file);
				if( odbMode == true )
				{
					GetApp().SPI_GetTextDocumentByID(docID).SPI_SetODBMode(odbServer,odbSenderToken,odbCustomPath);
				}
			}
			//fprintf(stderr,"CASApp::HandleAppleEvent() case ae_OpenDoc - end\n");
			break;
		}
	  case ae_PrintDoc:
		{
			break;
		}
	  case ae_Quit: 
		{
			DescType	theType;
			Size		theSize;
			SInt32		saveOption = kAEAsk;
			
			OSErr	err = ::AEGetParamPtr(&inAppleEvent, keyAESaveOptions,
					typeEnumeration, &theType, &saveOption,
					sizeof(SInt32), &theSize);
			if( err != noErr )
			{
				//処理無し
			}
			
			
			// We'll ignore the error here. Just proceeding with the
			// default of kAEAsk is reasonable.
			
			GetApp().NVI_Quit();
			
			break;
		}
		
	  case ae_SwitchTellTarget: 
		{
			PowerPlant::StAEDescriptor	targD;
			LModelObject	*newTarget = NULL;
			
			targD.GetOptionalParamDesc(inAppleEvent, keyAEData, typeWildCard);
			if (targD.mDesc.descriptorType != typeNull) {
				PowerPlant::StAEDescriptor	token;
				PowerPlant::LModelDirector::Resolve(targD.mDesc, token.mDesc);
				newTarget = GetModelFromToken(token);
			}
			SetTellTarget(newTarget);
			break;
		}
		/*#974
		//B0372
	  case ae_ReopenApp:
		{
			GetApp().SPI_CreateNewWindowAutomatic();
			break;
		}
	  case ae_OpenApp:
		{
			GetApp().SPI_CreateNewWindowAutomatic();
			break;
		}
		*/
		//#263
	  case ae_CommandLineArgs:
		{
			CAppleEventReader	ae(&inAppleEvent);
			//AEからデータ抽出
			AText	text;
			ae.GetParamText(keyDirectObject,typeUTF8Text,text);
			//TextArrayに構成して引数実行
			ATextArray	argsArray;
			for( AIndex pos = 0; pos < text.GetItemCount(); )
			{
				AText	line;
				text.GetLine(pos,line);
				argsArray.Add(line);
			}
			//引数実行
			AApplication::GetApplication().NVI_ExecCommandLineArgs(argsArray);
			break;
		}
	  default:
		{
			LModelObject::HandleAppleEvent(inAppleEvent, outAEReply,outResult, inAENumber);
			break;
		}
	}
}

PowerPlant::LModelObject*	CASApp::HandleCreateElementEvent( 
							DescType			inElemClass,
							DescType			inInsertPosition,
							LModelObject*		inTargetObject,
							const AppleEvent	&inAppleEvent,
							AppleEvent			&outAEReply)
{
	switch (inElemClass) 
	{
	  case cDocument:
		{
			ADocumentID	docID = GetApp().SPNVI_CreateNewTextDocument();
			CAppleEventReader	ae(&inAppleEvent);
			//テキスト内容
			AText	text;
			ae.GetParamText(keyAEData,typeUTF8Text,text);
			text.ConvertLineEndToCR();
			GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,text);
			//property
			AEDesc	props;
			::AECreateDesc(typeNull,NULL,0,&props);
			::AEGetParamDesc(&inAppleEvent,keyAEPropData,typeAERecord,&props);
			if( props.descriptorType != typeNull ) 
			{
				long	max;
				ThrowIfOSErr_(::AECountItems(&props,&max));
				for(long i = 1; i <= max; i++ ) 
				{
					DescType	keyword;
					AEDesc	value2;
					::AECreateDesc(typeNull,NULL,0,&value2);
					ThrowIfOSErr_(::AEGetNthDesc(&props,i,typeWildCard,&keyword,&value2));
					AEDesc	reply;
					AECreateDesc(typeNull,NULL,0,&reply);
					GetApp().SPI_GetTextDocumentByID(docID).SPI_GetASImp().SetAEProperty(keyword,value2,reply);
					::AEDisposeDesc(&reply);
					::AEDisposeDesc(&value2);
				}
				::AEDisposeDesc(&props);
			}
			break;
		}
	  case cIndexWindow:
		{
			AText	wintitle;
			wintitle.SetLocalizedText("IndexWindow_Title_New");
			ADocumentID	docID = GetApp().SPI_GetOrCreateFindResultWindowDocument();
			CAppleEventReader	ae(&inAppleEvent);
			//property
			AEDesc	props;
			::AECreateDesc(typeNull,NULL,0,&props);
			::AEGetParamDesc(&inAppleEvent,keyAEPropData,typeAERecord,&props);
			if( props.descriptorType != typeNull ) 
			{
				long	max;
				ThrowIfOSErr_(::AECountItems(&props,&max));
				for(long i = 1; i <= max; i++ ) 
				{
					DescType	keyword;
					AEDesc	value2;
					::AECreateDesc(typeNull,NULL,0,&value2);
					ThrowIfOSErr_(::AEGetNthDesc(&props,i,typeWildCard,&keyword,&value2));
					AEDesc	reply;
					AECreateDesc(typeNull,NULL,0,&reply);
					GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_GetASImp().SetAEProperty(keyword,value2,reply);
					::AEDisposeDesc(&reply);
					::AEDisposeDesc(&value2);
				}
				::AEDisposeDesc(&props);
			}
			//
			AText	grouptitle;
			grouptitle.SetLocalizedText("IndexWindow_NewGroupName");
			AFileAcc	basefolder;//#465（ここはベースフォルダ未対応）
			GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_AddGroup(grouptitle,GetEmptyText(),basefolder);
			break;
		}
	  case cWindow:
		ThrowOSErr_(errAEEventNotHandled);
		break;
	  default:
		return LModelObject::HandleCreateElementEvent(inElemClass,inInsertPosition, inTargetObject, inAppleEvent,outAEReply);
		break;
	}
	return NULL;
}

SInt32	CASApp::CountSubModels( DescType inModelID ) const
{
	SInt32	count = 0;
	switch(inModelID) 
	{
	  case cDocument:
		{
			count = GetApp().NVI_GetDocumentCount(kDocumentType_Text);
			break;
		}
	  case cIndexWindow:
		{
			count =  GetApp().NVI_GetDocumentCount(kDocumentType_IndexWindow);
			break;
		}
	  case cWindow:
		{
			count = GetApp().NVI_GetDocumentCount(kDocumentType_Text) + GetApp().NVI_GetDocumentCount(kDocumentType_IndexWindow);
			break;
		}
	  default:
		{
			count = LModelObject::CountSubModels(inModelID);
			break;
		}
	}
	return count;
}

void	CASApp::GetSubModelByPosition( DescType inModelID, SInt32 inPosition, AEDesc &outToken ) const
{
	//fprintf(stderr,"CASApp::GetSubModelByPosition - start\n");
	switch(inModelID) {
	  case cDocument:
		{
			ADocumentID	docID = GetApp().NVI_GetNthDocumentID(kDocumentType_Text,inPosition);
			if( docID != kObjectID_Invalid )
			{
				PutInToken(&(GetApp().SPI_GetTextDocumentByID(docID).SPI_GetASImp()),outToken);
			}
			else
			{
				ThrowOSErr_(errAENoSuchObject);
			}
			break;
		}
	  case cIndexWindow:
		{
			AObjectID	docID = GetApp().NVI_GetNthDocumentID(kDocumentType_IndexWindow,inPosition);
			if( docID != kObjectID_Invalid )
			{
				PutInToken(&(GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_GetASImp()),outToken);
			}
			else
			{
				ThrowOSErr_(errAENoSuchObject);
			}
			break;
		}
	  case cWindow:
		{
			AWindowID winID = GetApp().NVI_GetNthWindowID(kWindowType_Invalid,inPosition);
			if( winID != kObjectID_Invalid )
			{
				CASWin*	obj = new CASWin(this,winID);
				PutInToken(obj,outToken);
			}
			else
			{
				ThrowOSErr_(errAENoSuchObject);
			}
			break;
		}
	  default:
		LModelObject::GetSubModelByPosition(inModelID,inPosition,outToken);
		break;
	}
	//fprintf(stderr,"CASApp::GetSubModelByPosition - end\n");
}

void	CASApp::GetSubModelByName( DescType inModelID, Str255 inName, AEDesc &outToken ) const
{
	switch(inModelID) {
	  case cDocument:
		{
			AText	text(inName);
			AObjectID	docID = GetApp().NVI_GetDocumentIDByTitle(kDocumentType_Text,text);
			if( docID != /*#216 kIndex_Invalid*/kObjectID_Invalid )
			{
				PutInToken(&(GetApp().SPI_GetTextDocumentByID(docID).SPI_GetASImp()),outToken);
			}
			else
			{
				ThrowOSErr_(errAENoSuchObject);
			}
			break;
		}
	  case cIndexWindow:
		{
			AText	text(inName);
			AObjectID	docID = GetApp().NVI_GetDocumentIDByTitle(kDocumentType_IndexWindow,text);
			if( docID != /*#216 kIndex_Invalid*/kObjectID_Invalid )
			{
				PutInToken(&(GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_GetASImp()),outToken);
			}
			else
			{
				ThrowOSErr_(errAENoSuchObject);
			}
			break;
		}
	  default:
		LModelObject::GetSubModelByName(inModelID,inName,outToken);
		break;
	}
}

SInt32	CASApp::GetPositionOfSubModel( DescType inModelID, const LModelObject* inSubModel) const 
{
	return 1;
}

void	CASApp::GetAEProperty(
		DescType		inProperty,
		const AEDesc	&inRequestedType,
		AEDesc			&outPropertyDesc)  const
{
	switch(inProperty) 
	{
		/* case pIdleHookTime:
		{
		Int32	t = globals->mHookManager->mIdleHookTime;
		AECreateDesc(typeLongInteger,(Ptr)(&t),sizeof(Int32),&outPropertyDesc);
		break;
		}*/
		/*検討必要 case pFolderP:
		{
		FSSpec	folder;
		::FSMakeFSSpec(globals->mAppFileSpec.vRefNum,globals->mAppFileSpec.parID,"\p",&folder);
		AECreateDesc(typeFSS,(Ptr)(&(folder)),sizeof(FSSpec),&outPropertyDesc);
		break;
		}*///sdfcheck
	  default:
		LModelObject::GetAEProperty(inProperty,inRequestedType,outPropertyDesc);
		break;
	}
}

void	CASApp::SetAEProperty(
		DescType		inProperty,
		const AEDesc	&inValue,
		AEDesc&			outAEReply)
{
	switch(inProperty) 
	{
		/* case pIdleHookTime:
		Int32	t;
		UExtractFromAEDesc::TheSInt32(inValue,t);//D0004 PP2.2
		globals->mHookManager->mIdleHookTime = t;
		break;*/
	  default:
		LModelObject::SetAEProperty(inProperty,inValue,outAEReply);
		break;
	}
}

void	CASApp::MakeSelfSpecifier(
	AEDesc&		inSuperSpecifier,
	AEDesc&		outSelfSpecifier) const
{
	outSelfSpecifier.descriptorType = typeNull;
	outSelfSpecifier.dataHandle = NULL;
}


#pragma mark ===========================
#pragma mark [クラス]CASWin
#pragma mark ===========================

CASWin::CASWin( const CASApp* inApp, const AWindowID inWindowID ) : LModelObject((CASApp*)inApp,cWindow), mWindowID(inWindowID)
{
	SetLaziness(true);
}
CASWin::~CASWin()
{
}

void	CASWin::HandleAppleEvent( const AppleEvent &inAppleEvent, AppleEvent &outAEReply, AEDesc &outResult, long inAENumber)
{
	switch(inAENumber) 
	{
	  case ae_Exist:
		{
			Boolean	result = true;
			AECreateDesc(typeBoolean,(Ptr)(&result),sizeof(Boolean),&outResult);
			break;
		}
	  default:
		{
			LModelObject::HandleAppleEvent(inAppleEvent, outAEReply,outResult, inAENumber);
			break;
		}
	}
}

void	CASWin::GetAEProperty(
		DescType		inProperty,
		const AEDesc	&inRequestedType,
		AEDesc			&outPropertyDesc )  const
{
	switch(inProperty) 
	{
	  case pName:
		{
			AText	text;
			GetApp().NVI_GetWindowByID(mWindowID).NVI_GetCurrentTitle(text);
			CAEDesc::SetText(text,outPropertyDesc);
			break;
		}
	  case pIndex:
		{
			AItemCount	count = GetApp().NVI_GetWindowCount(kWindowType_Invalid);
			for( ANumber num = 1; num <= count; num++ )
			{
				if( GetApp().NVI_GetNthWindowID(kWindowType_Invalid,num) == mWindowID )
				{
					SInt32	n = num;
					::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&n),sizeof(n),&outPropertyDesc);
					break;
				}
			}
			break;
		}
	  case pBounds: 
		{
			ARect	rect;
			GetApp().NVI_GetWindowByID(mWindowID).NVI_GetWindowBounds(rect);
			Rect	bounds;
			bounds.left		= rect.left;
			bounds.top		= rect.top;
			bounds.right	= rect.right;
			bounds.bottom	= rect.bottom;
			::AECreateDesc(typeQDRectangle,(Ptr)&bounds,sizeof(Rect),&outPropertyDesc);
			break;
		}
	  case pWindowPosition: 
		{
			APoint	pt;
			GetApp().NVI_GetWindowByID(mWindowID).NVI_GetWindowPosition(pt);
			Point	qdpt;
			qdpt.h = pt.x;
			qdpt.v = pt.y;
			::AECreateDesc(typeQDPoint,(Ptr)&qdpt,sizeof(qdpt),&outPropertyDesc);
			break;
		}
	  case pHasCloseBox:
		{
			Boolean	b = GetApp().NVI_GetWindowByID(mWindowID).NVI_HasCloseButton();
			::AECreateDesc(typeBoolean,(Ptr)(&b),1,&outPropertyDesc);
			break;
		}
	  case pHasTitleBar:
		{
			Boolean	b = GetApp().NVI_GetWindowByID(mWindowID).NVI_HasTitleBar();
			::AECreateDesc(typeBoolean,(Ptr)(&b),1,&outPropertyDesc);
			break;
		}
	  case pIsFloating:
		{
			Boolean	b = GetApp().NVI_GetWindowByID(mWindowID).NVI_IsFloating();
			::AECreateDesc(typeBoolean,(Ptr)(&b),1,&outPropertyDesc);
			break;
		}
	  case pIsModal:
		{
			Boolean	b = GetApp().NVI_GetWindowByID(mWindowID).NVI_IsModal();
			::AECreateDesc(typeBoolean,(Ptr)(&b),1,&outPropertyDesc);
			break;
		}
	  case pIsResizable:
		{
			Boolean	b = GetApp().NVI_GetWindowByID(mWindowID).NVI_IsResizable();
			::AECreateDesc(typeBoolean,(Ptr)(&b),1,&outPropertyDesc);
			break;
		}
	  case pIsZoomable:
		{
			Boolean	b = GetApp().NVI_GetWindowByID(mWindowID).NVI_IsZoomable();
			::AECreateDesc(typeBoolean,(Ptr)(&b),1,&outPropertyDesc);
			break;
		}
	  case pIsZoomed:
		{
			Boolean	b = GetApp().NVI_GetWindowByID(mWindowID).NVI_IsZoomed();
			::AECreateDesc(typeBoolean,(Ptr)(&b),1,&outPropertyDesc);
			break;
		}
	  case pVisible:
		{
			Boolean	b = GetApp().NVI_GetWindowByID(mWindowID).NVI_IsWindowVisible();
			::AECreateDesc(typeBoolean,(Ptr)(&b),1,&outPropertyDesc);
			break;
		}
		
	  default:
		LModelObject::GetAEProperty(inProperty,inRequestedType,outPropertyDesc);
		break;
	}
}

void	CASWin::SetAEProperty(
		DescType		inProperty,
		const AEDesc	&inValue,
		AEDesc&			outAEReply)
{
	switch(inProperty) 
	{
	  case pName:
		{
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			GetApp().NVI_GetWindowByID(mWindowID).NVI_SetDefaultTitle(text);
			break;
		}
	  case pIndex:
		{
			SInt32 num;
			PowerPlant::UExtractFromAEDesc::TheSInt32(inValue,num);//D0004 PP2.2
			SetWindowOrder(mWindowID,num);
			break;
		}
	  case pBounds: 
		{
			Rect	bounds;
			PowerPlant::UExtractFromAEDesc::TheRect(inValue, bounds);
			ARect	rect;
			rect.left		= bounds.left;
			rect.top		= bounds.top;
			rect.right		= bounds.right;
			rect.bottom		= bounds.bottom;
			GetApp().NVI_GetWindowByID(mWindowID).NVI_SetWindowBounds(rect);
			break;
		}
	  case pWindowPosition: 
		{
			Point	qdpt;
			PowerPlant::UExtractFromAEDesc::ThePoint(inValue, qdpt);
			APoint	pt;
			pt.x = qdpt.h;
			pt.y = qdpt.v;
			GetApp().NVI_GetWindowByID(mWindowID).NVI_SetWindowPosition(pt);
			break;
		}
	  default:
		LModelObject::SetAEProperty(inProperty,inValue,outAEReply);
		break;
	}
}

void	CASWin::SetWindowOrder( const AWindowID inWindowID, const ANumber inNumber )
{
	if( inNumber <= 1 )
	{
		GetApp().NVI_GetWindowByID(inWindowID).NVI_SelectWindow();
	}
	else
	{
		AItemCount	count = GetApp().NVI_GetWindowCount(kWindowType_Invalid);
		ANumber currentnum = 1;
		for( ; currentnum <= count; currentnum++ )
		{
			if( GetApp().NVI_GetNthWindowID(kWindowType_Invalid,currentnum) == inWindowID )
			{
				break;
			}
		}
		ANumber	num = inNumber;
		if( currentnum == num )   return;
		if( currentnum < num )
		{
			num++;
		}
		AWindowID winID = GetApp().NVI_GetNthWindowID(kWindowType_Invalid,num-1);
		if( winID == kObjectID_Invalid )   ThrowOSErr_(errAEEventNotHandled);
		GetApp().NVI_GetWindowByID(inWindowID).NVI_SendBehind(winID);
	}
}

#pragma mark ===========================
#pragma mark [クラス]CASTextDoc
#pragma mark ===========================

CASTextDoc::CASTextDoc( CASApp* inApp, const ADocumentID inDocumentID ) : LModelObject(inApp,cDocument), mDocumentID(inDocumentID)
{
}
CASTextDoc::~CASTextDoc()
{
}

void	CASTextDoc::MakeSelfSpecifier(
	AEDesc	&inSuperSpecifier,
	AEDesc	&outSelfSpecifier) const
{
	AText	title;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_GetTitle(title);
	
	AEDesc	keyData;
	CAEDesc::SetText(title,keyData);
	
	OSErr	err = ::CreateObjSpecifier(cDocument, &inSuperSpecifier, formName, &keyData, false, &outSelfSpecifier);
	ThrowIfOSErr_(err);
}

void	CASTextDoc::HandleAppleEvent( const AppleEvent &inAppleEvent, AppleEvent &outAEReply, AEDesc &outResult, long inAENumber)
{
	switch(inAENumber) 
	{
	  case ae_Save: 
		{
			//#528 別名保存もありうるのでdirty判定不要 if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsDirty() == true )
			{
				CAppleEventReader	ae(&inAppleEvent);
				AFileAcc	file;
				AText	path;
				if( ae.GetParamFile(keyAEFile,file) == true )
				{
					file.CreateFile();//#528
					if( file.Exist() == true )
					{
						GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_SpecifyFile(file);
					}
				}
				else if( ae.GetParamText(keyAEFile,typeUTF8Text,path) == true )
				{
					file.Specify(path,kFilePathType_2);
					file.CreateFile();
					if( file.Exist() == false )
					{
						file.Specify(path,kFilePathType_1);
						file.CreateFile();
					}
					if( file.Exist() == false )
					{
						file.Specify(path);
						file.CreateFile();
					}
					if( file.Exist() == true )
					{
						GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_SpecifyFile(file);
					}
				}
				OSType	fileType;
				if( ae.GetParamOSType(keyAEFileType,fileType) == true )
				{
					GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFileType(fileType);
				}
				GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_Save(false);
			}
			break;
		}
	  case ae_Close:
		{
			CAppleEventReader	ae(&inAppleEvent);
			AFileAcc	file;
			AText	path;
			if( ae.GetParamFile(keyAEFile,file) == true )
			{
				if( file.Exist() == true )
				{
					GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_SpecifyFile(file);
				}
			}
			else if( ae.GetParamText(keyAEFile,typeUTF8Text,path) == true )
			{
				file.Specify(path,kFilePathType_2);
				file.CreateFile();
				if( file.Exist() == false )
				{
					file.Specify(path,kFilePathType_1);
					file.CreateFile();
				}
				if( file.Exist() == false )
				{
					file.Specify(path);
					file.CreateFile();
				}
				if( file.Exist() == true )
				{
					GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_SpecifyFile(file);
				}
			}
			DescType	askSave = kAEAsk;
			ae.GetParamEnum(keyAESaveOptions,askSave);
			switch(askSave)
			{
			  case 'ask ':
				{
					GetApp().SPI_TryCloseTextDocument(mDocumentID);//#379 SPI_GetTextDocumentByID(mDocumentID).SPI_TryClose();
					break;
				}
			  case 'yes ':
				{
					if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsDirty() == true )
					{
						GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_Save(false);
					}
					GetApp().SPI_TryCloseTextDocument(mDocumentID);//#379 SPI_GetTextDocumentByID(mDocumentID).SPI_TryClose();
					break;
				}
			  case 'no  ':
				{
					GetApp().SPI_ExecuteCloseTextDocument(mDocumentID);//#379 SPI_GetTextDocumentByID(mDocumentID).SPI_ExecuteClose();
					break;
				}
			}
			break;
		}
	  case ae_Exist:
		{
			Boolean	result = true;
			AECreateDesc(typeBoolean,(Ptr)(&result),sizeof(Boolean),&outResult);
			break;
		}
	  case ae_SelectObject:
	  case ae_Select:
		{
			GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_RevealDocumentWithAView();//#92SPI_SelectFirstWindow();
			break;
		}
	  case ae_Print: 
		{
			// Print one copy of all pages of document
			//未対応
			break;
		}
	  default:
		{
			LModelObject::HandleAppleEvent(inAppleEvent, outAEReply,outResult, inAENumber);
			break;
		}
	}
}

PowerPlant::LModelObject*	CASTextDoc::HandleCreateElementEvent( 
							DescType			inElemClass,
							DescType			inInsertPosition,
							LModelObject*		inTargetObject,
							const AppleEvent	&inAppleEvent,
							AppleEvent			&outAEReply)
{
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )   ThrowOSErr_(errAEEventNotHandled);
	CAppleEventReader	ae(&inAppleEvent);
	AText	text;
	ae.GetParamText(keyAEData,typeUTF8Text,text);
	if( text.GetItemCount() == 0 )
	{
		ae.GetPropertyText(pContents,text);
	}
	ATextIndex	pos = 0;
	DescType	targetType = inTargetObject->GetModelKind();
	if( targetType == cParagraph || targetType == cWord || targetType == cChar || 
		targetType == cInsertionPoint || targetType == cSelection )
	{
		CASTextDocElement*	target = dynamic_cast<CASTextDocElement*>(inTargetObject);
		switch(inInsertPosition) 
		{
		  case kAEBeginning:
			{
				pos = target->GetStartPos();
				break;
			}
		  case kAEEnd:
			{
				pos = target->GetEndPos();
				if( target->GetModelKind() == cParagraph )
				{
					if( pos > 0 )
					{
						if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextChar(pos-1) == kUChar_LineEnd )
						{
							pos--;
						}
					}
				}
				break;
			}
		  case kAEBefore:
			{
				pos = target->GetStartPos();
				if( target->GetModelKind() == cParagraph )
				{
					if( pos > 0 )
					{
						pos--;
					}
				}
				break;
			}
		  case kAEAfter:
			{
				pos = target->GetEndPos();
				break;
			}
		}
	}
	//Undoアクションタグ記録
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
	//
	LModelObject*	result = NULL;
	switch(inElemClass) 
	{
	  case cParagraph:
		{
			AText	lineend;
			lineend.Add(kUChar_LineEnd);
			if( text.GetItemCount() == 0 )
			{
				GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(pos,lineend);
				result = new CASParagraph(this,mDocumentID,pos,pos+1);
			}
			else
			{
				if( pos >= 1 )
				{
					if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextChar(pos-1) != kUChar_LineEnd )
					{
						GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(pos,lineend);
						pos++;
					}
				}
				if( pos < GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength() )
				{
					if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextChar(pos) != kUChar_LineEnd )
					{
						GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(pos,lineend);
					}
				}
				else
				{
					GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(pos,lineend);
				}
				GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(pos,text);
				result = new CASParagraph(this,mDocumentID,pos,pos+text.GetItemCount()+1);
			}
			break;
		}
	  case cWord:
		{
			if( text.GetItemCount() == 0 )   ThrowOSErr_(errAEEventNotHandled);
			AText	space;
			space.Add(kUChar_Space);
			if( pos > 0 )
			{
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_IsAsciiAlphabet(pos-1) == true )
				{
					GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(pos,space);
					pos++;
				}
			}
			if( pos < GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength() )
			{
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_IsAsciiAlphabet(pos) == true )
				{
					GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(pos,space);
				}
			}
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(pos,text);
			result = new CASWord(this,mDocumentID,pos,pos+text.GetItemCount());
			break;
		}
	  case cChar:
		{
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(pos,text);
			result = new CASChar(this,mDocumentID,pos,pos+text.GetItemCount());
			break;
		}
	  default:
		{
			result = NULL;
			break;
		}
	}
	return result;
}

SInt32	CASTextDoc::CountSubModels( DescType inModelID ) const
{
	SInt32	count = 0;
	switch(inModelID) 
	{
	  case cParagraph:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetParagraphCount(0,GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength());
			break;
		}
	  case cWord:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetWordCount(0,GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength());
			break;
		}
	  case cChar:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetCharCount(0,GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength());
			break;
		}
	  case cInsertionPoint:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetCharCount(0,GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength()) +1;
			break;
		}
	  case cSelection:
		{
			count = 1;
			break;
		}
	  default:
		{
			//処理無し
			break;
		}
	}
	return count;
}

void	CASTextDoc::GetSubModelByPosition( DescType inModelID, SInt32 inPosition, AEDesc &outToken ) const
{
	switch(inModelID)
	{
	  case cParagraph:
		{
			ATextIndex	start, end;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetParagraph(0,inPosition-1,start,end) == false )   ThrowOSErr_(errAENoSuchObject);
			CASParagraph*	obj = new CASParagraph(this,mDocumentID,start,end);
			PutInToken(obj,outToken);
			break;
		}
	  case cWord:
		{
			ATextIndex	start, end;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetWord(0,inPosition-1,start,end) == false )   ThrowOSErr_(errAENoSuchObject);
			CASWord*	obj = new CASWord(this,mDocumentID,start,end);
			PutInToken(obj,outToken);
			break;
		}
	  case cChar:
		{
			ATextIndex	start, end;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetChar(0,inPosition-1,start,end) == false )   ThrowOSErr_(errAENoSuchObject);
			CASChar*	obj = new CASChar(this,mDocumentID,start,end);
			PutInToken(obj,outToken);
			break;
		}
	  case cInsertionPoint:
		{
			//B0331
			ATextIndex	pos;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetInsertionPoint(0,inPosition-1,pos) == false )   ThrowOSErr_(errAENoSuchObject);
			CASInsertionPoint*	obj = new CASInsertionPoint(this,mDocumentID,pos,pos);
			PutInToken(obj,outToken);
			break;
		}
	  case cSelection:
		{
			//
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
			if( winID == kObjectID_Invalid )   ThrowOSErr_(errAEEventNotHandled);
			else
			{
				AArray<ATextIndex>	start, end;
				GetApp().SPI_GetTextWindowByID(winID).SPI_GetSelect(mDocumentID,start,end);
				if( start.GetItemCount() == 1 )
				{
					CASSelectionObject*	obj = new CASSelectionObject(this,mDocumentID,start.Get(0),end.Get(0));
					PutInToken(obj,outToken);
				}
				else
				{
					ThrowOSErr_(errAEEventNotHandled);
				}
			}
			break;
		}
	  default:
		{
			//処理無し
			break;
		}
	}
}

void	CASTextDoc::GetSubModelByName( DescType inModelID, Str255 inName, AEDesc &outToken ) const
{
	//処理無し
}

SInt32	CASTextDoc::GetPositionOfSubModel( DescType	inModelID, const LModelObject *inSubModel) const
{
	ATextIndex	spos = (dynamic_cast<const CASTextDocElement*>(inSubModel))->GetStartPos();
	switch(inModelID)
	{
	  case cParagraph:
		{
			return GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetParagraphCount(0,spos) +1;
		}
	  case cWord:
		{
			return GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetWordCount(0,spos) +1;
		}
	  case cChar:
		{
			return GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(0,spos) +1;
		}
	  case cInsertionPoint:
		{
			return GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(0,spos) +1;
		}
	  case cSelection:
		{
			return 1;
		}
	  default:
		{
			return LModelObject::GetPositionOfSubModel(inModelID, inSubModel);
		}
	}
}

void	CASTextDoc::GetAEProperty(
		DescType		inProperty,
		const AEDesc	&inRequestedType,
		AEDesc			&outPropertyDesc )  const
{
	switch(inProperty) 
	{
	  case pContents:
		{
			AText	text;
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetText(text);
			AStTextPtr	textptr(text,0,text.GetItemCount());
			::AECreateDesc(typeUTF8Text,(Ptr)(textptr.GetPtr()),textptr.GetByteCount(),&outPropertyDesc);
			break;
		}
	  case pWindowIndex:
		{
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
			AItemCount	count = GetApp().NVI_GetWindowCount(kWindowType_Invalid);
			for( ANumber num = 1; num <= count; num++ )
			{
				if( GetApp().NVI_GetNthWindowID(kWindowType_Invalid,num) == winID )
				{
					SInt32	n = num;
					::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&n),sizeof(n),&outPropertyDesc);
					break;
				}
			}
			break;
		}
	  case pMode:
		{
			AText	modename;
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetModeDisplayName(modename);
			CAEDesc::SetText(modename,outPropertyDesc);
			break;
		}
	  case pCreator:
		{
			OSType	creator = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFileCreator();
			AText	text;
			//AUtil::GetATextFromOSType(creator,text);
			text.SetFromOSType(creator);
			CAEDesc::SetText(text,outPropertyDesc);
			break;
		}
	  case pType:
		{
			OSType	type = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFileType();
			AText	text;
			//AUtil::GetATextFromOSType(type,text);
			text.SetFromOSType(type);
			CAEDesc::SetText(text,outPropertyDesc);
			break;
		}
	  case pCharacterCode:
		{
			AText	text;
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextEncoding(text);
			CAEDesc::SetText(text,outPropertyDesc);
			break;
		}
	  case pReturnCode:
		{
			OSType	returnCode = 'CR  ';
			switch(GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetReturnCode())
			{
			  case returnCode_CR:
				{
					returnCode = 'CR  ';
					break;
				}
			  case returnCode_CRLF:
				{
					returnCode = 'CRLF';
					break;
				}
			  case returnCode_LF:
				{
					returnCode = 'LF\000\000';
					break;
				}
			}
			::AECreateDesc(typeEnumeration,(Ptr)(&returnCode),sizeof(OSType),&outPropertyDesc);
			break;
		}
	  case pName:
		{
			AText	text;
			GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_GetTitle(text);
			CAEDesc::SetText(text,outPropertyDesc);
			break;
		}
	  case pModified:
		{
			Boolean	modified = GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsDirty();
			::AECreateDesc(typeBoolean,(Ptr)(&modified),1,&outPropertyDesc);
			break;
		}
	  case pFileF:
		{
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsFileSpecified() == false )   ThrowOSErr_(errAEEventNotHandled);
			else
			{
				AFileAcc	file;
				GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_GetFile(file);
				FSRef	fsref;
				file.GetFSRef(fsref);
				//B0438 ::AECreateDesc(typeFSRef,(Ptr)&fsref,sizeof(fsref),&outPropertyDesc);
				//B0438
				AliasHandle alias = NULL;
				::FSNewAlias(NULL, &fsref, &alias);
				if( alias != NULL )
				{
					::AECreateDesc(typeAlias,*alias,::GetHandleSize((Handle)alias),&outPropertyDesc);
					::DisposeHandle((Handle) alias);
				}
				/*B0438 メモ
				miで取得したファイルオブジェクトをIllustratorにて使用できない問題。
				おそらく、IllustratorではAECoerceDesc()を呼んでおらず、AEGetDescDataで、FSSpecへ格納していると思われる。
				そのため、2.1.6ではOK（FSSpecで格納）だが、2.1.8ではNG（FSRefで格納）となる。
				typeAliasで返した方が互換性高そうなのでtypeAliasで返すことにする。
				*/
				//テスト用コード
				/*AEDesc	aedesc;
				::AECreateDesc(typeFSRef,(Ptr)&fsref,sizeof(fsref),&aedesc);
				AEDesc	desc;
				OSErr	err = ::AECoerceDesc(&outPropertyDesc,typeFSS,&desc);
				FSSpec	data;
				err = ::AEGetDescData(&desc,&data,sizeof(data));
				AFileAcc	f;
				f.SpecifyByFSSpec(data);
				AText	path;
				f.GetPath(path);
				path.OutputToStderr();*/
			}
			break;
		}
	  case pIndex:
		{
			SInt32	n = GetDocumentIndex();
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&n),sizeof(n),&outPropertyDesc);
			break;
		}
	  case pSelection:
		{
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
			if( winID == kObjectID_Invalid )   ThrowOSErr_(errAEEventNotHandled);
			else
			{
				AText	text;
				GetApp().SPI_GetTextWindowByID(winID).SPI_GetSelectedText(mDocumentID,text);
				CAEDesc::SetText(text,outPropertyDesc);
			}
			break;
		}
	  case pHeader:
		{
			/*#725 InfoHeader廃止
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
			if( winID == kObjectID_Invalid )   ThrowOSErr_(errAEEventNotHandled);
			else
			{
				AText	text;
				GetApp().SPI_GetTextWindowByID(winID).SPI_GetInfoHeaderText(text);
				CAEDesc::SetText(text,outPropertyDesc);
			}
			*/
			CAEDesc::SetText(GetEmptyText(),outPropertyDesc);
			break;
		}
	  case pFont:
		{
			AText	text;
			//win AFontWrapper::GetFontName(GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFont(),text);
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFontName(text);//win
			CAEDesc::SetText(text,outPropertyDesc);
			break;
		}
	  case pSize:
		{
			SInt32	size = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFontSize();
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&size),sizeof(SInt32),&outPropertyDesc);
			break;
		}
	  default:
		LModelObject::GetAEProperty(inProperty,inRequestedType,outPropertyDesc);
		break;
	}
}

void	CASTextDoc::SetAEProperty(
		DescType		inProperty,
		const AEDesc	&inValue,
		AEDesc&			outAEReply)
{
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )   ThrowOSErr_(errAEEventNotHandled);
	switch(inProperty) 
	{
	  case pContents:
		{
			//Undoアクションタグ記録
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
			//削除
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(0,GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength());
			//
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			text.ConvertLineEndToCR();
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(0,text);
			break;
		}
	  case pWindowIndex:
		{
			SInt32 num;
			PowerPlant::UExtractFromAEDesc::TheSInt32(inValue,num);
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
			CASWin::SetWindowOrder(winID,num);
			break;
		}
	  case pIndex:
		{
			SInt32 num;
			PowerPlant::UExtractFromAEDesc::TheSInt32(inValue,num);
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
			if( num <= 1 )
			{
				GetApp().NVI_GetWindowByID(winID).NVI_SelectWindow();
				GetApp().NVI_GetWindowByID(winID).NVI_SelectTabByDocumentID(mDocumentID);
			}
			else
			{
				ANumber	currentnum = GetDocumentIndex();
				if( num == currentnum )   return;
				if( currentnum < num )
				{
					num++;
				}
				ADocumentID	prevDocID = GetApp().NVI_GetNthDocumentID(kDocumentType_Text,num-1);//sdfcheck
				if( prevDocID == kObjectID_Invalid )   ThrowOSErr_(errAEEventNotHandled);
				AWindowID	prevWinID = GetApp().SPI_GetTextDocumentByID(prevDocID).SPI_GetFirstWindowID();
				AIndex	prevTabIndex = GetApp().NVI_GetWindowByID(prevWinID).NVI_GetTabIndexByDocumentID(prevDocID);
				AIndex	prevTabZOrder = GetApp().NVI_GetWindowByID(prevWinID).NVI_GetTabZOrder(prevTabIndex);
				if( prevWinID != winID )
				{
					GetApp().NVI_GetWindowByID(winID).NVI_SendBehind(prevWinID);
				}
				else
				{
					AIndex	tabIndex = GetApp().NVI_GetWindowByID(winID).NVI_GetTabIndexByDocumentID(mDocumentID);
					AIndex	zorder = prevTabZOrder;
					if( GetApp().NVI_GetWindowByID(prevWinID).NVI_GetTabZOrder(tabIndex) > zorder )
					{
						zorder++;
					}
					GetApp().NVI_GetWindowByID(prevWinID).NVI_SetTabZOrder(tabIndex,zorder);
					GetApp().NVI_GetWindowByID(prevWinID).NVI_SelectWindow();
				}
			}
			break;
		}
	  case pSelection:
		{
			//
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			text.ConvertLineEndToCR();
			//
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
			if( winID == kObjectID_Invalid )   ThrowOSErr_(errAEEventNotHandled);
			else
			{
				AArray<ATextIndex>	start, end;
				GetApp().SPI_GetTextWindowByID(winID).SPI_GetSelect(mDocumentID,start,end);
				if( start.GetItemCount() == 1 )
				{
					//Undoアクションタグ記録
					GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
					//削除
					GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(start.Get(0),end.Get(0));
					//
					GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(start.Get(0),text);
				}
			}
			break;
		}
	  case pMode:
		{
			//
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			//
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetModeByDisplayName(text);
			break;
		}
	  case pCharacterCode:
		{
			//
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			//B0343
			if( ATextEncodingWrapper::CheckTextEncodingAvailability(text) == false )
			{
				break;
			}
			//
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetTextEncoding(text);
			break;
		}
	  case pReturnCode:
		{
			OSType	returnCode;
			PowerPlant::UExtractFromAEDesc::TheEnum(inValue,returnCode);
			switch(returnCode) 
			{
			  case 'CR  ':
				{
					GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetReturnCode(returnCode_CR);
					break;
				}
			  case 'CRLF':
				{
					GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetReturnCode(returnCode_CRLF);
					break;
				}
			  case 'LF\000\000':
				{
					GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetReturnCode(returnCode_LF);
					break;
				}
			}
			break;
		}
	  case pType:
		{
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFileType(text.GetOSTypeFromText());//AUtil::GetOSTypeFromAText(text));
			break;
		}
	  case pCreator:
		{
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFileCreator(text.GetOSTypeFromText());//AUtil::GetOSTypeFromAText(text));
			break;
		}
	  case pHeader:
		{
			/*#725 InfoHeader廃止
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
			if( winID == kObjectID_Invalid )   ThrowOSErr_(errAEEventNotHandled);
			else
			{
				GetApp().SPI_GetTextWindowByID(winID).SPI_SetInfoHeaderText(text);
			}
			*/
			break;
		}
	  case pName:
		{
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetTemporaryTitle(text);
			break;
		}
	  case pFont:
		{
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			/*win AFont	font;
			if( AFontWrapper::GetFontByName(text,font) == true )
			{
				GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFont(font);
			}
			else
			{
				ThrowOSErr_(errAEEventNotHandled);
			}*/
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFontName(text);
			break;
		}
	  case pSize:
		{
			SInt32	size;
			PowerPlant::UExtractFromAEDesc::TheSInt32(inValue,size);//D0004 PP2.2
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFontSize(size);
			break;
		}
	  default:
		LModelObject::SetAEProperty(inProperty,inValue,outAEReply);
		break;
	}
}

SInt32	CASTextDoc::GetDocumentIndex() const
{
	AItemCount	count = GetApp().NVI_GetDocumentCount(kDocumentType_Text);
	for( ANumber num = 1; num <= count; num++ )
	{
		if( GetApp().NVI_GetNthDocumentID(kDocumentType_Text,num) == mDocumentID )
		{
			return num;
		}
	}
	return 0;
}

#pragma mark ===========================
#pragma mark [クラス]CASTextDocElement
#pragma mark ===========================

CASTextDocElement::CASTextDocElement( const LModelObject* inSuper, DescType inKind, const ADocumentID inDocumentID, 
		const ATextIndex inStartPos, const ATextIndex inEndPos ) 
: LModelObject((LModelObject*)inSuper,inKind), mDocumentID(inDocumentID), mStartPos(inStartPos), mEndPos(inEndPos)
{
	SetLaziness(true);
}
CASTextDocElement::~CASTextDocElement()
{
}

void	CASTextDocElement::HandleAppleEvent( const AppleEvent &inAppleEvent, AppleEvent &outAEReply, AEDesc &outResult, long inAENumber)
{
	switch (inAENumber) 
	{
	  case ae_SelectObject:
	  case ae_Select:
		{
			ATextIndex	spos = mStartPos;
			ATextIndex	epos = mEndPos;
			OSErr	err;
			AEDesc	object;
			::AECreateDesc(typeNull,NULL,0,&object);
			err = ::AEGetParamDesc(&inAppleEvent,'to  ',typeObjectSpecifier,&object);
			PowerPlant::StAEDescriptor	token;
			if( err == noErr ) 
			{
				err = PowerPlant::LModelDirector::Resolve(object,token.mDesc);
			}
			CASTextDocElement	*toWhere = NULL;
			if( err == noErr ) 
			{
				toWhere = (CASTextDocElement*)GetModelFromToken(token);
			}
			::AEDisposeDesc(&object);
			if( toWhere != NULL ) 
			{
				ATextIndex	spos2 = toWhere->GetStartPos();
				if( spos2 < spos )   spos = spos2;
				ATextIndex	epos2 = toWhere->GetEndPos();
				if( epos < epos2 )   epos = epos2;
			}
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
			if( winID == kObjectID_Invalid )   ThrowOSErr_(errAEEventNotHandled);
			else
			{
				GetApp().SPI_GetTextWindowByID(winID).SPI_SetSelect(mDocumentID,spos,epos);
			}
			break;
		}
	  case ae_Exist:
		{
			Boolean	result = true;
			::AECreateDesc(typeBoolean,(Ptr)(&result),sizeof(Boolean),&outResult);
			break;
		}
	  case ae_Replace:
		{
			//?
			break;
		}
	  default:
		LModelObject::HandleAppleEvent(inAppleEvent,outAEReply,outResult,inAENumber);
		break;
	}
}

SInt32	CASTextDocElement::GetPositionOfSubModel( DescType	inModelID, const LModelObject *inSubModel) const
{
	ATextIndex	spos = dynamic_cast<const CASTextDocElement*>(inSubModel)->GetStartPos();
	switch(inModelID)
	{
	  case cParagraph:
		{
			return GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetParagraphCount(mStartPos,spos) +1;
		}
	  case cWord:
		{
			return GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetWordCount(mStartPos,spos) +1;
		}
	  case cChar:
		{
			return GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(mStartPos,spos) +1;
		}
	  case cInsertionPoint:
		{
			return GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(mStartPos,spos) +1;
		}
	  case cSelection:
		{
			return 1;
		}
	  default:
		{
			return LModelObject::GetPositionOfSubModel(inModelID, inSubModel);
		}
	}
}

void	CASTextDocElement::GetAEProperty(
		DescType		inProperty,
		const AEDesc	&inRequestedType,
		AEDesc			&outPropertyDesc)  const
{
	switch(inProperty) 
	{
	  case pContents:
		{
			AText	text;
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetText(mStartPos,mEndPos,text);
			AStTextPtr	textptr(text,0,text.GetItemCount());
			AECreateDesc(typeUTF8Text,(Ptr)(textptr.GetPtr()),textptr.GetByteCount(),&outPropertyDesc);
			break;
		}
	  default:
		LModelObject::GetAEProperty(inProperty,inRequestedType,outPropertyDesc);
		break;
	}
}

void	CASTextDocElement::SetAEProperty(
		DescType		inProperty,
		const AEDesc	&inValue,
		AEDesc&			outAEReply)
{
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )   ThrowOSErr_(errAEEventNotHandled);
	switch(inProperty) 
	{
	  case pContents:
		{
			//Undoアクションタグ記録
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
			//削除
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(mStartPos,mEndPos);
			//
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			text.ConvertLineEndToCR();
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(mStartPos,text);
			break;
		}
	  default:
		LModelObject::SetAEProperty(inProperty,inValue,outAEReply);
		break;
	}
}

void	CASTextDocElement::GetSubModelByPosition( DescType inModelID, Int32 inPosition, AEDesc &outToken ) const
{
	switch(inModelID) 
	{
	  case cParagraph:
	  case cWord:
	  case cChar:
	  case cInsertionPoint:
		{
			ThrowOSErr_(errAENoSuchObject);
			break;
		}
	  default:
		LModelObject::GetSubModelByPosition(inModelID,inPosition,outToken);
		break;
	}
}

void	CASTextDocElement::GetSubModelByName(
		DescType		inModelID,
		Str255			inName,
		AEDesc			&outToken) const
{
	switch(inModelID) 
	{
	  case cParagraph:
	  case cWord:
	  case cChar:
	  case cInsertionPoint:
		{
			ThrowOSErr_(errAENoSuchObject);
			break;
		}
	  default:
		LModelObject::GetSubModelByName(inModelID, inName, outToken);
		break;
	}
}

void	CASTextDocElement::HandleDelete( AppleEvent &outAEReply, AEDesc &outResult )
{
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )   ThrowOSErr_(errAEEventNotHandled);
	//Undoアクションタグ記録
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
	//削除
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(mStartPos,mEndPos);
}

PowerPlant::LModelObject *CASTextDocElement::GetInsertionContainer(DescType inInsertPosition) const
{
	/* HandleCreateElementEventは常にdocumentから行う*/
	return &(GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetASImp());
}


//HandleClose検討

#pragma mark ===========================
#pragma mark [クラス]CASParagraph
#pragma mark ===========================

CASParagraph::CASParagraph( const LModelObject* inSuper, const ADocumentID inDocumentID, const ATextIndex inStartPos, const ATextIndex inEndPos )
: CASTextDocElement(inSuper,cParagraph,inDocumentID,inStartPos,inEndPos)
{
}

CASParagraph::~CASParagraph()
{
}

void	CASParagraph::GetAEProperty(
		DescType		inProperty,
		const AEDesc	&inRequestedType,
		AEDesc			&outPropertyDesc) const
{
	switch(inProperty) 
	{
	  case pIndex:
		{
			SInt32	index = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetParagraphCount(0,mStartPos) +1;
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&index),sizeof(SInt32),&outPropertyDesc);
			break;
		}
	  default:
		CASTextDocElement::GetAEProperty(inProperty,inRequestedType,outPropertyDesc);
		break;
	}
}

SInt32	CASParagraph::CountSubModels( DescType inModelID) const
{
	SInt32	count = 0;
	switch(inModelID) 
	{
	  case cWord:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetWordCount(mStartPos,mEndPos);
			break;
		}
	  case cChar:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(mStartPos,mEndPos);
			break;
		}
	  case cInsertionPoint:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(mStartPos,mEndPos);
			if( mEndPos == GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength() )   count++;//paragraphの場合、CR後の挿入は不可
			break;
		}
	  default:
		count = LModelObject::CountSubModels(inModelID);
		break;
	}
	return count;
}

void	CASParagraph::GetSubModelByPosition( DescType inModelID, SInt32 inPosition, AEDesc &outToken ) const
{
	switch(inModelID) 
	{
	  case cWord:
		{
			ATextIndex	start, end;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetWord(mStartPos,inPosition-1,start,end) == false )   ThrowOSErr_(errAENoSuchObject);
			if( start >= mEndPos )   ThrowOSErr_(errAENoSuchObject);
			CASWord*	obj = new CASWord(this,mDocumentID,start,end);
			PutInToken(obj,outToken);
			break;
		}
	  case cChar:
		{
			ATextIndex	start, end;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetChar(mStartPos,inPosition-1,start,end) == false )   ThrowOSErr_(errAENoSuchObject);
			if( start >= mEndPos )   ThrowOSErr_(errAENoSuchObject);
			CASChar*	obj = new CASChar(this,mDocumentID,start,end);
			PutInToken(obj,outToken);
			break;
		}
	  case cInsertionPoint:
		{
			//B0331
			ATextIndex	pos;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetInsertionPoint(mStartPos,inPosition-1,pos) == false )   ThrowOSErr_(errAENoSuchObject);
			if( pos >/*B0331 =*/ mEndPos )   ThrowOSErr_(errAENoSuchObject);
			CASInsertionPoint*	obj = new CASInsertionPoint(this,mDocumentID,pos,pos);
			PutInToken(obj,outToken);
			break;
		}
	  default:
		LModelObject::GetSubModelByPosition(inModelID,inPosition,outToken);
		break;
	}
}

#pragma mark ===========================
#pragma mark [クラス]CASWord
#pragma mark ===========================

CASWord::CASWord( const LModelObject* inSuper, const ADocumentID inDocumentID, const ATextIndex inStartPos, const ATextIndex inEndPos )
: CASTextDocElement(inSuper,cWord,inDocumentID,inStartPos,inEndPos)
{
}

CASWord::~CASWord()
{
}

void	CASWord::GetAEProperty(
		DescType		inProperty,
		const AEDesc	&inRequestedType,
		AEDesc			&outPropertyDesc) const
{
	switch(inProperty) 
	{
	  case pIndex:
		{
			SInt32	index = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetWordCount(0,mStartPos) +1;
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&index),sizeof(SInt32),&outPropertyDesc);
			break;
		}
	  default:
		CASTextDocElement::GetAEProperty(inProperty,inRequestedType,outPropertyDesc);
		break;
	}
}

SInt32	CASWord::CountSubModels( DescType inModelID) const
{
	SInt32	count = 0;
	switch(inModelID) 
	{
	  case cChar:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(mStartPos,mEndPos);
			break;
		}
	  case cInsertionPoint:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(mStartPos,mEndPos) +1;
			break;
		}
	  default:
		count = LModelObject::CountSubModels(inModelID);
		break;
	}
	return count;
}

void	CASWord::GetSubModelByPosition( DescType inModelID, SInt32 inPosition, AEDesc &outToken ) const
{
	switch(inModelID) 
	{
	  case cChar:
		{
			ATextIndex	start, end;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetChar(mStartPos,inPosition-1,start,end) == false )   ThrowOSErr_(errAENoSuchObject);
			if( start >= mEndPos )   ThrowOSErr_(errAENoSuchObject);
			CASChar*	obj = new CASChar(this,mDocumentID,start,end);
			PutInToken(obj,outToken);
			break;
		}
	  case cInsertionPoint:
		{
			//B0331
			ATextIndex	pos;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetInsertionPoint(mStartPos,inPosition-1,pos) == false )   ThrowOSErr_(errAENoSuchObject);
			if( pos > mEndPos )   ThrowOSErr_(errAENoSuchObject);
			CASInsertionPoint*	obj = new CASInsertionPoint(this,mDocumentID,pos,pos);
			PutInToken(obj,outToken);
			break;
		}
	  default:
		LModelObject::GetSubModelByPosition(inModelID,inPosition,outToken);
		break;
	}
}

void	CASWord::HandleDelete( AppleEvent &outAEReply, AEDesc &outResult )
{
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )   ThrowOSErr_(errAEEventNotHandled);
	//Undoアクションタグ記録
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
	//削除
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(mStartPos,mEndPos);
	//前後スペース
	AUChar	ch1 = kUChar_Space, ch2 =0;
	if( mStartPos > 0 )   ch1 = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextChar(mStartPos-1);
	if( mStartPos < GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength() )   
	ch2 = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextChar(mStartPos);
	if( ch1 == kUChar_Space && ch2 == kUChar_Space )
	{
		GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(mStartPos,mStartPos+1);
	}
} 

#pragma mark ===========================
#pragma mark [クラス]CASChar
#pragma mark ===========================

CASChar::CASChar( const LModelObject* inSuper, const ADocumentID inDocumentID, const ATextIndex inStartPos, const ATextIndex inEndPos )
: CASTextDocElement(inSuper,cChar,inDocumentID,inStartPos,inEndPos)
{
}

CASChar::~CASChar()
{
}

void	CASChar::GetAEProperty(
		DescType		inProperty,
		const AEDesc	&inRequestedType,
		AEDesc			&outPropertyDesc) const
{
	switch(inProperty) 
	{
	  case pIndex:
		{
			SInt32	index = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(0,mStartPos) +1;
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&index),sizeof(SInt32),&outPropertyDesc);
			break;
		}
	  default:
		CASTextDocElement::GetAEProperty(inProperty,inRequestedType,outPropertyDesc);
		break;
	}
}

SInt32	CASChar::CountSubModels( DescType inModelID) const
{
	SInt32	count = 0;
	switch(inModelID) 
	{
	  case cInsertionPoint:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(mStartPos,mEndPos) +1;
			break;
		}
	  default:
		count = LModelObject::CountSubModels(inModelID);
		break;
	}
	return count;
}

void	CASChar::GetSubModelByPosition( DescType inModelID, SInt32 inPosition, AEDesc &outToken ) const
{
	switch(inModelID) 
	{
	  case cInsertionPoint:
		{
			//B0331
			ATextIndex	pos;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetInsertionPoint(mStartPos,inPosition-1,pos) == false )   ThrowOSErr_(errAENoSuchObject);
			if( pos > mEndPos )   ThrowOSErr_(errAENoSuchObject);
			CASInsertionPoint*	obj = new CASInsertionPoint(this,mDocumentID,pos,pos);
			PutInToken(obj,outToken);
			break;
		}
	  default:
		LModelObject::GetSubModelByPosition(inModelID,inPosition,outToken);
		break;
	}
}

#pragma mark ===========================
#pragma mark [クラス]CASInsertionPoint
#pragma mark ===========================

CASInsertionPoint::CASInsertionPoint( const LModelObject* inSuper, const ADocumentID inDocumentID, const ATextIndex inStartPos, const ATextIndex inEndPos )
: CASTextDocElement(inSuper,cInsertionPoint,inDocumentID,inStartPos,inEndPos)
{
}

CASInsertionPoint::~CASInsertionPoint()
{
}

void	CASInsertionPoint::GetAEProperty(
		DescType		inProperty,
		const AEDesc	&inRequestedType,
		AEDesc			&outPropertyDesc) const
{
	switch(inProperty) 
	{
	  case pIndex:
		{
			SInt32	index = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(0,mStartPos)+1;//B0062はCountInsertionPointsだったから
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&index),sizeof(SInt32),&outPropertyDesc);
			break;
		}
	  default:
		CASTextDocElement::GetAEProperty(inProperty,inRequestedType,outPropertyDesc);
		break;
	}
}

#pragma mark ===========================
#pragma mark [クラス]CASSelectionObject
#pragma mark ===========================

CASSelectionObject::CASSelectionObject( const LModelObject* inSuper, const ADocumentID inDocumentID, const ATextIndex inStartPos, const ATextIndex inEndPos )
: CASTextDocElement(inSuper,cSelection,inDocumentID,inStartPos,inEndPos)
{
}

CASSelectionObject::~CASSelectionObject()
{
}

SInt32	CASSelectionObject::CountSubModels( DescType inModelID) const
{
	SInt32	count = 0;
	switch(inModelID) 
	{
	  case cParagraph:
		{
			//B331(B0304) 071123
			if( mStartPos == mEndPos )
			{
				count = 1;
			}
			else
			{
				count = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetParagraphCount(mStartPos,mEndPos);
			}
			break;
		}
	  case cWord:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetWordCount(mStartPos,mEndPos);
			break;
		}
	  case cChar:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(mStartPos,mEndPos);
			break;
		}
	  case cInsertionPoint:
		{
			count = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(mStartPos,mEndPos)+1;//B0331 +1必要
			break;
		}
	  default:
		count = LModelObject::CountSubModels(inModelID);
		break;
	}
	return count;
}

void	CASSelectionObject::GetSubModelByPosition( DescType inModelID, SInt32 inPosition, AEDesc &outToken ) const
{
	switch(inModelID) 
	{
	  case cParagraph:
		{
			ATextIndex	start, end;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetParagraph(mStartPos,inPosition-1,start,end) == false )   ThrowOSErr_(errAENoSuchObject);
			if( start > mEndPos )   ThrowOSErr_(errAENoSuchObject);
			CASParagraph*	obj = new CASParagraph(this,mDocumentID,start,end);//B0304
			PutInToken(obj,outToken);
			break;
		}
	  case cWord:
		{
			ATextIndex	start, end;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetWord(mStartPos,inPosition-1,start,end) == false )   ThrowOSErr_(errAENoSuchObject);
			if( start > mEndPos )   ThrowOSErr_(errAENoSuchObject);
			CASWord*	obj = new CASWord(this,mDocumentID,start,end);
			PutInToken(obj,outToken);
			break;
		}
	  case cChar:
		{
			ATextIndex	start, end;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetChar(mStartPos,inPosition-1,start,end) == false )   ThrowOSErr_(errAENoSuchObject);
			if( start > mEndPos )   ThrowOSErr_(errAENoSuchObject);
			CASChar*	obj = new CASChar(this,mDocumentID,start,end);
			PutInToken(obj,outToken);
			break;
		}
	  case cInsertionPoint:
		{
			//B0331
			ATextIndex	pos;
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetInsertionPoint(mStartPos,inPosition-1,pos) == false )   ThrowOSErr_(errAENoSuchObject);
			if( pos > mEndPos )   ThrowOSErr_(errAENoSuchObject);
			CASInsertionPoint*	obj = new CASInsertionPoint(this,mDocumentID,pos,pos);
			PutInToken(obj,outToken);
			break;
		}
	  default:
		LModelObject::GetSubModelByPosition(inModelID,inPosition,outToken);
		break;
	}
}

void	CASSelectionObject::SetAEProperty(
		DescType		inProperty,
		const AEDesc	&inValue,
		AEDesc&			outAEReply)
{
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )   ThrowOSErr_(errAEEventNotHandled);
	switch(inProperty) 
	{
	  case pContents:
		{
			//Undoアクションタグ記録
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
			//削除
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(mStartPos,mEndPos);
			//
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			text.ConvertLineEndToCR();
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(mStartPos,text);
			//選択復元
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
			if( winID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(winID).SPI_SetSelect(mDocumentID,mStartPos,mStartPos+text.GetItemCount());
			}
			break;
		}
	  default:
		CASTextDocElement::SetAEProperty(inProperty,inValue,outAEReply);
		break;
	}
}

#pragma mark ===========================
#pragma mark [クラス]CASIndexDoc
#pragma mark ===========================

CASIndexDoc::CASIndexDoc( CASApp* inApp, const ADocumentID inDocumentID ) : LModelObject(inApp,cIndexWindow), mDocumentID(inDocumentID), mCache_TextInfo(NULL)
{
}
CASIndexDoc::~CASIndexDoc()
{
}

void	CASIndexDoc::HandleAppleEvent( const AppleEvent &inAppleEvent, AppleEvent &outAEReply, AEDesc &outResult, long inAENumber)
{
	switch(inAENumber) 
	{
	  case ae_Save: 
		{
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_Save(false);
			break;
		}
	  case ae_Close:
		{
			CAppleEventReader	ae(&inAppleEvent);
			DescType	askSave = kAEAsk;
			ae.GetParamEnum(keyAESaveOptions,askSave);
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
			break;
		}
	  case ae_Exist:
		{
			Boolean	result = true;
			AECreateDesc(typeBoolean,(Ptr)(&result),sizeof(Boolean),&outResult);
			break;
		}
	  default:
		{
			LModelObject::HandleAppleEvent(inAppleEvent, outAEReply,outResult, inAENumber);
			break;
		}
	}
}

PowerPlant::LModelObject*	CASIndexDoc::HandleCreateElementEvent( 
							DescType			inElemClass,
							DescType			inInsertPosition,
							LModelObject*		inTargetObject,
							const AppleEvent	&inAppleEvent,
							AppleEvent			&outAEReply)
{
	if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).NVI_IsReadOnly() == true )   ThrowOSErr_(errAEEventNotHandled);
	CAppleEventReader	ae(&inAppleEvent);
	AText	text;
	ae.GetParamText(keyAEData,typeUTF8Text,text);
	if( text.GetItemCount() == 0 )
	{
		ae.GetPropertyText(pContents,text);
	}
	AIndex	insertGroupIndex = 0;
	AIndex	insertItemIndex = 0;
	switch(inTargetObject->GetModelKind())
	{
	  case cIndexGroup:
		{
			AIndex	groupIndex = (dynamic_cast<CASIndexGroup*>(inTargetObject))->GetGroupIndex();
			switch(inInsertPosition)
			{
			  case kAEBeginning:
				{
					insertGroupIndex = groupIndex;
					insertItemIndex = 0;
					break;
				}
			  case kAEEnd:
				{
					insertGroupIndex = groupIndex;
					insertItemIndex = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetItemCountInGroup(groupIndex);
					break;
				}
			  case kAEBefore:
				{
					insertGroupIndex = groupIndex-1;
					if( insertGroupIndex < 0 ) 
					{
						insertGroupIndex = 0;
						insertItemIndex = 0;
					}
					else
					{
						insertItemIndex = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetItemCountInGroup(groupIndex);
					}
					break;
				}
			  case kAEAfter:
				{
					insertGroupIndex = groupIndex+1;
					if( insertGroupIndex >= GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetGroupCount() )
					{
						insertGroupIndex = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetGroupCount();
						insertItemIndex = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetItemCountInGroup(groupIndex);
					}
					else
					{
						insertItemIndex = 0;
					}
					break;
				}
			}
			break;
		}
	  case cIndexRecord:
		{
			AIndex	groupIndex = (dynamic_cast<CASIndexRecord*>(inTargetObject))->GetGroupIndex();
			AIndex	itemIndex = (dynamic_cast<CASIndexRecord*>(inTargetObject))->GetItemIndex();
			switch(inInsertPosition)
			{
			  case kAEBeginning:
			  case kAEBefore:
				{
					insertGroupIndex = groupIndex;
					insertItemIndex = itemIndex;
					break;
				}
			  case kAEEnd:
			  case kAEAfter:
				{
					insertGroupIndex = groupIndex;
					insertItemIndex = itemIndex+1;
					break;
				}
			}
			break;
		}
	}
	PowerPlant::LModelObject*	result = NULL;
	switch(inElemClass)
	{
	  case cIndexGroup:
		{
			AFileAcc	basefolder;//#465
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_InsertGroup(insertGroupIndex,
					GetEmptyText(),GetEmptyText(),basefolder);//#465
			result = new CASIndexGroup(this,mDocumentID,insertGroupIndex);
			//property
			AEDesc	props;
			::AECreateDesc(typeNull,NULL,0,&props);
			::AEGetParamDesc(&inAppleEvent,keyAEPropData,typeAERecord,&props);
			if( props.descriptorType != typeNull ) 
			{
				long	max;
				ThrowIfOSErr_(::AECountItems(&props,&max));
				for(long i = 1; i <= max; i++ ) 
				{
					DescType	keyword;
					AEDesc	value2;
					::AECreateDesc(typeNull,NULL,0,&value2);
					ThrowIfOSErr_(::AEGetNthDesc(&props,i,typeWildCard,&keyword,&value2));
					AEDesc	reply;
					AECreateDesc(typeNull,NULL,0,&reply);
					(dynamic_cast<CASIndexGroup*>(result))->SetAEProperty(keyword,value2,reply);
					::AEDisposeDesc(&reply);
					::AEDisposeDesc(&value2);
				}
				::AEDisposeDesc(&props);
			}
			break;
		}
	  case cIndexRecord:
		{
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_InsertItem_TextPosition(insertGroupIndex,insertItemIndex,
					GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),
					GetEmptyText(),//#465
					0,0,0,GetEmptyText());
			result = new CASIndexRecord(this,mDocumentID,insertGroupIndex,insertItemIndex);
			//property
			AEDesc	props;
			::AECreateDesc(typeNull,NULL,0,&props);
			::AEGetParamDesc(&inAppleEvent,keyAEPropData,typeAERecord,&props);
			if( props.descriptorType != typeNull ) 
			{
				SetItemFromDesc(insertGroupIndex,insertItemIndex,props);
				/* long	max;
				ThrowIfOSErr_(::AECountItems(&props,&max));
				for(long i = 1; i <= max; i++ ) 
				{
					DescType	keyword;
					AEDesc	value2;
					::AECreateDesc(typeNull,NULL,0,&value2);
					ThrowIfOSErr_(::AEGetNthDesc(&props,i,typeWildCard,&keyword,&value2));
					AEDesc	reply;
					AECreateDesc(typeNull,NULL,0,&reply);
					(dynamic_cast<CASIndexRecord*>(result))->SetAEProperty(keyword,value2,reply);
					::AEDisposeDesc(&reply);
					::AEDisposeDesc(&value2);
				}*/
				::AEDisposeDesc(&props);
			}
			break;
		}
	}
	return result;
}

SInt32	CASIndexDoc::CountSubModels( DescType inModelID ) const
{
	SInt32	count = 0;
	switch(inModelID) 
	{
	  case cIndexRecord:
		{
			AItemCount	c = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetGroupCount();
			for( AIndex i = 0; i < c; i++ )
			{
				count += GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetItemCountInGroup(i);
			}
			break;
		}
	  case cIndexGroup:
		{
			count = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetGroupCount();
			break;
		}
	}
	return count;
}

//
void	CASIndexDoc::GetSubModelByPosition( DescType inModelID, SInt32 inPosition, AEDesc &outToken ) const
{
	switch(inModelID)
	{
	  case cIndexRecord:
		{
			AIndex	groupIndex, itemIndex;
			if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetIndexFromASRecordIndex(inPosition-1,groupIndex,itemIndex) == true )
			{
				CASIndexRecord*	obj = new CASIndexRecord(this,mDocumentID,groupIndex,itemIndex);
				PutInToken(obj,outToken);
			}
			else
			{
				ThrowOSErr_(errAENoSuchObject);
			}
			break;
		}
	  case cIndexGroup:
		{
			if( inPosition-1 >= 0 && inPosition-1 < GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetGroupCount() )
			{
				CASIndexGroup*	obj = new CASIndexGroup(this,mDocumentID,inPosition-1);
				PutInToken(obj,outToken);
			}
			else
			{
				ThrowOSErr_(errAENoSuchObject);
			}
			break;
		}
	  default:
		{
			//処理無し
			break;
		}
	}
}

void	CASIndexDoc::GetSubModelByName( DescType inModelID, Str255 inName, AEDesc &outToken ) const
{
	//処理無し
}

//
SInt32	CASIndexDoc::GetPositionOfSubModel( DescType inModelID, const LModelObject *inSubModel) const
{
	switch(inModelID)
	{
	  case cIndexRecord:
		{
			AIndex	groupIndex = (dynamic_cast<const CASIndexRecord*>(inSubModel))->GetGroupIndex();
			AIndex	itemIndex = (dynamic_cast<const CASIndexRecord*>(inSubModel))->GetItemIndex();
			AIndex	index;
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(groupIndex,itemIndex,index);
			return index+1;
		}
	  case cIndexGroup:
		{
			return (dynamic_cast<const CASIndexGroup*>(inSubModel))->GetGroupIndex()+1;
		}
	  default:
		{
			return LModelObject::GetPositionOfSubModel(inModelID, inSubModel);
		}
	}
}

void	CASIndexDoc::GetAEProperty(
		DescType		inProperty,
		const AEDesc	&inRequestedType,
		AEDesc			&outPropertyDesc)  const
{
	switch(inProperty) 
	{
	  case pName:
		{
			AText	text;
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).NVI_GetTitle(text);
			CAEDesc::SetText(text,outPropertyDesc);
			break;
		}
	  case pIndex:
		{
			SInt32	n = GetDocumentIndex();
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&n),sizeof(n),&outPropertyDesc);
			break;
		}
	  case pWindowIndex:
		{
			AWindowID	winID = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetWindowID();//#92 SPI_GetWindowID();
			AItemCount	count = GetApp().NVI_GetWindowCount(kWindowType_Invalid);
			for( ANumber num = 1; num <= count; num++ )
			{
				if( GetApp().NVI_GetNthWindowID(kWindowType_Invalid,num) == winID )
				{
					SInt32	n = num;
					::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&n),sizeof(n),&outPropertyDesc);
					break;
				}
			}
			break;
		}
	  case pContents:
		{
			AItemCount	itemcount = CountSubModels(cIndexRecord);
			OSErr	err = ::AECreateList(NULL,0,false,&outPropertyDesc);
			if( err != noErr )   break;
			for( AIndex i = 0; i < itemcount; i++ ) 
			{
				AEDesc	props;
				err = ::AECreateList(NULL,0,true,&props);
				if( err != noErr )    break;
				AFileAcc	file;
				ATextIndex	spos;
				AItemCount	len;
				AText	comment;
				ANumber	para;
				if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetRecord(i,file,spos,len,para,comment) == false )
				{
					break;
				}
				FSRef	fsref;
				if( file.GetFSRef(fsref) == true )
				{
					//B0438 ::AEPutKeyPtr(&props,pFileF,typeFSRef,(Ptr)&fsref,sizeof(FSRef));
					//B0438
					AliasHandle alias = NULL;
					::FSNewAlias(NULL, &fsref, &alias);
					if( alias != NULL )
					{
						::AEPutKeyPtr(&props,pFileF,typeAlias,*alias,::GetHandleSize((Handle)alias));
						::DisposeHandle((Handle) alias);
					}
				}
				SInt32	start = spos;
				SInt32	end = spos+len;
				AEPutKeyPtr(&props,pStartPositionF,/*#1034 typeLongInteger*/typeSInt32,(Ptr)&start,sizeof(SInt32));
				AEPutKeyPtr(&props,pEndPositionF,/*#1034 typeLongInteger*/typeSInt32,(Ptr)&end,sizeof(SInt32));
				comment.ConvertFromUTF8ToUTF16();
				{
					AStTextPtr	textptr(comment,0,comment.GetItemCount());
					::AEPutKeyPtr(&props,pCommentF,'utxt',(Ptr)textptr.GetPtr(),textptr.GetByteCount());
				}
				::AEPutDesc(&outPropertyDesc,0,&props);
				::AEDisposeDesc(&props);
			}
			break;
		}
	  case pFileF:
		{
			if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).NVI_IsFileSpecified() == false )   ThrowOSErr_(errAEEventNotHandled);
			else
			{
				AFileAcc	file;
				GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).NVI_GetFile(file);
				FSRef	fsref;
				file.GetFSRef(fsref);
				//B0438::AECreateDesc(typeFSRef,(Ptr)&fsref,sizeof(fsref),&outPropertyDesc);
				//B0438
				AliasHandle alias = NULL;
				::FSNewAlias(NULL, &fsref, &alias);
				if( alias != NULL )
				{
					::AECreateDesc(typeAlias,*alias,::GetHandleSize((Handle)alias),&outPropertyDesc);
					::DisposeHandle((Handle) alias);
				}
			}
			break;
		}
	  case pFileWidth:
		{
			ANumber	fileWidth, infoWidth;
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetColumnWidth(fileWidth,infoWidth);//#92 SPI_GetWindow().SPI_GetColumnWidth(fileWidth,infoWidth);
			SInt32	w = fileWidth;
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&w),sizeof(SInt32),&outPropertyDesc);
			break;
		}
	  case pInfoWidth:
		{
			ANumber	fileWidth, infoWidth;
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetColumnWidth(fileWidth,infoWidth);//#92 SPI_GetWindow().SPI_GetColumnWidth(fileWidth,infoWidth);
			SInt32	w = infoWidth;
			AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&w),sizeof(SInt32),&outPropertyDesc);
			break;
		}
	  case pFileOrder:
		{
			SInt32	o = 1;
			AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&o),sizeof(SInt32),&outPropertyDesc);
			break;
		}
	  case pInfoOrder:
		{
			SInt32	o = 2;
			AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&o),sizeof(SInt32),&outPropertyDesc);
			break;
		}
		/* case pPathType:
		{
		OSType	pathType = 'full';
		if( mPartialPath ) {
		pathType = 'part';
		}
		AECreateDesc(typeEnumeration,(Ptr)(&pathType),sizeof(OSType),&outPropertyDesc);
		break;
		}*/
	  case pAskSaving:
		{
			Boolean	b = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetAskSavingMode();
			::AECreateDesc(typeBoolean,(Ptr)(&b),sizeof(Boolean),&outPropertyDesc);
			break;
		}
	  default:
		LModelObject::GetAEProperty(inProperty,inRequestedType,outPropertyDesc);
		break;
	}
}

void	CASIndexDoc::SetAEProperty(
		DescType		inProperty,
		const AEDesc	&inValue,
		AEDesc&			outAEReply)
{
	if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).NVI_IsReadOnly() == true )   ThrowOSErr_(errAEEventNotHandled);
	switch(inProperty) 
	{
	  case pContents:
		{
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_DeleteAllGroup();
			AFileAcc	basefolder;//#465（ここはベースフォルダ未対応）
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_AddGroup(GetEmptyText(),GetEmptyText(),basefolder);//#465
			long	count;
			::AECountItems(&inValue,&count);
			for( AIndex index = 0; index < count; index++ )
			{
				AEKeyword	keyword;
				AEDesc	rec;
				::AEGetNthDesc(&inValue,index+1,typeAERecord,&keyword,&rec);
				InsertItemFromDesc(0,index,rec);
				::AEDisposeDesc(&rec);
			}
			break;
		}
	  case pIndex:
		{
			SInt32 num;
			PowerPlant::UExtractFromAEDesc::TheSInt32(inValue,num);
			AWindowID	winID = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetWindowID();//#92 SPI_GetWindowID();
			if( num <= 1 )
			{
				GetApp().NVI_GetWindowByID(winID).NVI_SelectWindow();
				GetApp().NVI_GetWindowByID(winID).NVI_SelectTabByDocumentID(mDocumentID);
			}
			else
			{
				ANumber	currentnum = GetDocumentIndex();
				if( num == currentnum )   return;
				if( currentnum < num )
				{
					num++;
				}
				ADocumentID	prevDocID = GetApp().NVI_GetNthDocumentID(kDocumentType_IndexWindow,num-1);//sdfcheck
				if( prevDocID == kObjectID_Invalid )   ThrowOSErr_(errAEEventNotHandled);
				AWindowID	prevWinID = GetApp().SPI_GetIndexWindowDocumentByID(prevDocID).SPI_ASGetWindowID();//#92 SPI_GetWindowID();
				GetApp().NVI_GetWindowByID(winID).NVI_SendBehind(prevWinID);
			}
			break;
		}
	  case pWindowIndex:
		{
			SInt32 num;
			PowerPlant::UExtractFromAEDesc::TheSInt32(inValue,num);
			AWindowID	winID = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetWindowID();//#92 SPI_GetWindowID();
			CASWin::SetWindowOrder(winID,num);
			break;
		}
	  case pFileWidth:
		{
			//
			SInt32	width;
			PowerPlant::UExtractFromAEDesc::TheSInt32(inValue,width);
			//
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASSetFileColumnWidth(width);//#92 SPI_GetWindow().SPI_SetFileColumnWidth(width);
			break;
		}
	  case pInfoWidth:
		{
			//
			SInt32	width;
			PowerPlant::UExtractFromAEDesc::TheSInt32(inValue,width);
			//
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASSetInfoColumnWidth(width);//#92 SPI_GetWindow().SPI_SetInfoColumnWidth(width);
			break;
		}
	  case pPathType:
		{
			//★
			break;
		}
	  case pName:
		{
			//
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			//
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SetTitle(text);
			break;
		}
	  case pAskSaving:
		{
			Boolean	b;
			PowerPlant::UExtractFromAEDesc::TheBoolean(inValue,b);
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SetAskSavingMode(b);
			break;
		}
	}
}

const AItemCount	kPreHitTextLength = 100;
const AItemCount	kPostHitTextLength = 100;

void	CASIndexDoc::InsertItemFromDesc( const AIndex inGroupIndex, const AIndex inItemIndex, const AEDesc &inProps )
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).
			SPI_InsertItem_TextPosition(inGroupIndex,inItemIndex,
			GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),
			GetEmptyText(),//#465
			0,0,0,GetEmptyText());
	SetItemFromDesc(inGroupIndex,inItemIndex,inProps);
}
void	CASIndexDoc::SetItemFromDesc( const AIndex inGroupIndex, const AIndex inItemIndex, const AEDesc &inProps )
{
	CAppleEventReader	ae(&inProps);
	AText	comment;
	ae.GetParamText(pCommentF,typeUTF8Text,comment);
	AText	filepath, hittext, prehit, posthit, positiontext;
	AFileAcc	file;
	ae.GetParamFile(pFileF,file);
	if( file.IsSpecified() == false )
	{
		ae.GetParamText(pFileF,typeUTF8Text,filepath);
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
		
		if( ae.GetParamInteger(pLineNumber,paragraph) == true )
		{
			paragraph --;
			spos = mCache_TextInfo.GetLineStart(paragraph);
			len = mCache_TextInfo.GetLineLengthWithLineEnd(paragraph);
		}
		else
		{
			if( ae.GetParamInteger(pStartPositionF,spos) == false )   spos = 0;
			ANumber	epos;
			if( ae.GetParamInteger(pEndPositionF,epos) == false )   epos = 0;
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

SInt32	CASIndexDoc::GetDocumentIndex() const
{
	AItemCount	count = GetApp().NVI_GetDocumentCount(kDocumentType_IndexWindow);
	for( ANumber num = 1; num <= count; num++ )
	{
		if( GetApp().NVI_GetNthDocumentID(kDocumentType_IndexWindow,num) == mDocumentID )
		{
			return num;
		}
	}
	return 0;
}


#pragma mark ===========================
#pragma mark [クラス]CASIndexGroup
#pragma mark ===========================

CASIndexGroup::CASIndexGroup( const LModelObject* inSuper, const ADocumentID inDocumentID, const AIndex inGroupIndex )
  : LModelObject((LModelObject*)inSuper,cIndexGroup), mDocumentID(inDocumentID), mGroupIndex(inGroupIndex)
{
	SetLaziness(true);
}
CASIndexGroup::~CASIndexGroup()
{
}

void	CASIndexGroup::HandleAppleEvent( const AppleEvent &inAppleEvent, AppleEvent &outAEReply, AEDesc &outResult, long inAENumber )
{
	switch (inAENumber) 
	{
	  case ae_SelectObject:
	  case ae_Select:
		{
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SelectGroup(mGroupIndex);
		}
		break;
	  case ae_Collapse:
		{
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_Collapse(mGroupIndex);
			break;
		}
	  case ae_Expand:
		{
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_Expand(mGroupIndex);
			break;
		}
	  case ae_Exist:
		{
			Boolean	result = true;
			::AECreateDesc(typeBoolean,(Ptr)(&result),sizeof(Boolean),&outResult);
			break;
		}
	  default:
		{
			LModelObject::HandleAppleEvent(inAppleEvent, outAEReply,outResult, inAENumber);
			break;
		}
	}
}

SInt32	CASIndexGroup::GetPositionOfSubModel( DescType	inModelID, const LModelObject *inSubModel) const
{
	switch(inModelID)
	{
	  case cIndexRecord:
		{
			AIndex	subGroupIndex = (dynamic_cast<const CASIndexRecord*>(inSubModel))->GetGroupIndex();
			AIndex	subItemIndex = (dynamic_cast<const CASIndexRecord*>(inSubModel))->GetItemIndex();
			AIndex	subASIndex;
			if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(subGroupIndex,subItemIndex,subASIndex) == true )
			{
				AIndex	firstItemASIndex;
				GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(GetGroupIndex(),0,firstItemASIndex);
				return subASIndex-firstItemASIndex+1;
			}
			else
			{
				ThrowOSErr_(errAENoSuchObject);
				return 1;
			}
			break;
		}
	  default:
		{
			return LModelObject::GetPositionOfSubModel(inModelID, inSubModel);
		}
	}
}

void	CASIndexGroup::GetAEProperty(
		DescType		inProperty,
		const AEDesc	&inRequestedType,
		AEDesc			&outPropertyDesc)  const
{
	switch(inProperty) 
	{
	  case pContents:
		{
			AIndex	firstItemASIndex;
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(GetGroupIndex(),0,firstItemASIndex);
			//
			AItemCount	itemcount = CountSubModels(cIndexRecord);
			OSErr	err = ::AECreateList(NULL,0,false,&outPropertyDesc);
			if( err != noErr )   break;
			for( AIndex i = 0; i < itemcount; i++ ) 
			{
				AEDesc	props;
				err = ::AECreateList(NULL,0,true,&props);
				if( err != noErr )    break;
				AFileAcc	file;
				ATextIndex	spos;
				AItemCount	len;
				AText	comment;
				ANumber	para;
				if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetRecord(firstItemASIndex+i,file,spos,len,para,comment) == false )
				{
					break;
				}
				FSRef	fsref;
				if( file.GetFSRef(fsref) == true )
				{
					//B0438 ::AEPutKeyPtr(&props,pFileF,typeFSRef,(Ptr)&fsref,sizeof(FSRef));
					//B0438
					AliasHandle alias = NULL;
					::FSNewAlias(NULL, &fsref, &alias);
					if( alias != NULL )
					{
						::AEPutKeyPtr(&props,pFileF,typeAlias,*alias,::GetHandleSize((Handle)alias));
						::DisposeHandle((Handle) alias);
					}
				}
				SInt32	start = spos;
				SInt32	end = spos+len;
				AEPutKeyPtr(&props,pStartPositionF,/*#1034 typeLongInteger*/typeSInt32,(Ptr)&start,sizeof(SInt32));
				AEPutKeyPtr(&props,pEndPositionF,/*#1034 typeLongInteger*/typeSInt32,(Ptr)&end,sizeof(SInt32));
				comment.ConvertFromUTF8ToUTF16();
				{
					AStTextPtr	textptr(comment,0,comment.GetItemCount());
					::AEPutKeyPtr(&props,pCommentF,'utxt',(Ptr)textptr.GetPtr(),textptr.GetByteCount());
				}
				::AEPutDesc(&outPropertyDesc,0,&props);
				::AEDisposeDesc(&props);
			}
			break;
		}
	  case pIndex:
		{
			AIndex	index;
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(mGroupIndex,kIndex_Invalid,index);
			SInt32	n = index;
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&n),sizeof(n),&outPropertyDesc);
			break;
		}
	  case pCommentF:
		{
			AText	title1, title2;
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetGroupTitle(mGroupIndex,title1,title2);
			AText	text;
			text.SetText(title1);
			text.AddText(title2);
			AStTextPtr	textptr(text,0,text.GetItemCount());
			::AECreateDesc(typeUTF8Text,(Ptr)(textptr.GetPtr()),textptr.GetByteCount(),&outPropertyDesc);
			break;
		}
	  default:
		LModelObject::GetAEProperty(inProperty,inRequestedType,outPropertyDesc);
		break;
	}
}

void	CASIndexGroup::SetAEProperty(
		DescType		inProperty,
		const AEDesc	&inValue,
		AEDesc&			outAEReply)
{
	switch(inProperty) 
	{
	  case pContents:
		{
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_DeleteAllItemsInGroup(mGroupIndex);
			long	count;
			::AECountItems(&inValue,&count);
			for( AIndex index = 0; index < count; index++ )
			{
				AEKeyword	keyword;
				AEDesc	rec;
				::AEGetNthDesc(&inValue,index+1,typeAERecord,&keyword,&rec);
				GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASImp().InsertItemFromDesc(mGroupIndex,index,rec);
				::AEDisposeDesc(&rec);
			}
			break;
		}
	  case pIndex:
		{
			//★
			break;
		}
	  case pCommentF:
		{
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SetGroupTitle(mGroupIndex,text,GetEmptyText());
			break;
		}
	  default:
		LModelObject::SetAEProperty(inProperty,inValue,outAEReply);
		break;
	}
}

//
SInt32	CASIndexGroup::CountSubModels( DescType inModelID ) const
{
	SInt32	count = 0;
	switch(inModelID) 
	{
	  case cIndexRecord:
		{
			count = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetItemCountInGroup(mGroupIndex);
			break;
		}
	}
	return count;
}

void	CASIndexGroup::GetSubModelByName(
		DescType		inModelID,
		Str255			inName,
		AEDesc			&outToken) const
{
	switch(inModelID) 
	{
	  case cIndexRecord:
	  case cIndexGroup:
		{
			ThrowOSErr_(errAENoSuchObject);
			break;
		}
	  default:
		LModelObject::GetSubModelByName(inModelID, inName, outToken);
		break;
	}
}

//
void	CASIndexGroup::GetSubModelByPosition( DescType inModelID, SInt32 inPosition, AEDesc &outToken ) const
{
	switch(inModelID) 
	{
	  case cIndexRecord:
		{
			if( inPosition-1 < GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetItemCountInGroup(mGroupIndex) )
			{
				CASIndexRecord*	obj = new CASIndexRecord(this,mDocumentID,mGroupIndex,inPosition-1);
				PutInToken(obj,outToken);
			}
			else
			{
				ThrowOSErr_(errAENoSuchObject);
			}
			break;
		}
	  case cIndexGroup:
		ThrowOSErr_(errAENoSuchObject);
		break;
	  default:
		LModelObject::GetSubModelByPosition(inModelID,inPosition,outToken);
		break;
	}
}

//
void	CASIndexGroup::HandleDelete( AppleEvent &outAEReply, AEDesc &outResult )
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_DeleteGroup(mGroupIndex);
}

PowerPlant::LModelObject *CASIndexGroup::GetInsertionContainer(DescType inInsertPosition) const
{
	/* HandleCreateElementEventは常にdocumentから行う*/
	return &(GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASImp());
}

#pragma mark ===========================
#pragma mark [クラス]CASIndexRecord
#pragma mark ===========================

CASIndexRecord::CASIndexRecord( const LModelObject* inSuper, const ADocumentID inDocumentID, const AIndex inGroupIndex, const AIndex inItemIndex )
	: LModelObject((LModelObject*)inSuper,cIndexRecord), mDocumentID(inDocumentID), mGroupIndex(inGroupIndex), mItemIndex(inItemIndex)
{
	SetLaziness(true);
}
CASIndexRecord::~CASIndexRecord()
{
}

void	CASIndexRecord::HandleAppleEvent( const AppleEvent &inAppleEvent, AppleEvent &outAEReply, AEDesc &outResult, long inAENumber )
{
	switch (inAENumber) 
	{
	  case ae_SelectObject:
	  case ae_Select:
		{
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_Select(mGroupIndex,mItemIndex);
		}
		break;
	  case ae_Exist:
		{
			Boolean	result = true;
			AECreateDesc(typeBoolean,(Ptr)(&result),sizeof(Boolean),&outResult);
			break;
		}
	  default:
		{
			LModelObject::HandleAppleEvent(inAppleEvent, outAEReply,outResult, inAENumber);
			break;
		}
	}
}

SInt32	CASIndexRecord::GetPositionOfSubModel( DescType	inModelID, const LModelObject *inSubModel) const
{
	switch(inModelID)
	{
	  default:
		{
			return LModelObject::GetPositionOfSubModel(inModelID, inSubModel);
		}
	}
}

void	CASIndexRecord::GetAEProperty(
		DescType		inProperty,
		const AEDesc	&inRequestedType,
		AEDesc			&outPropertyDesc)  const
{
	switch(inProperty) 
	{
	  case pContents:
		{
			OSErr	err = AECreateList(NULL,0,true,&outPropertyDesc);
			if( err != noErr )   break;
			AIndex	ASIndex;
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(mGroupIndex,mItemIndex,ASIndex);
			//
			AFileAcc	file;
			ATextIndex	spos;
			AItemCount	len;
			AText	comment;
			ANumber	para;
			if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetRecord(ASIndex,file,spos,len,para,comment) == false )
			{
				break;
			}
			FSRef	fsref;
			if( file.GetFSRef(fsref) == true )
			{
				//B0438 ::AEPutKeyPtr(&outPropertyDesc,pFileF,typeFSRef,(Ptr)&fsref,sizeof(FSRef));
				//B0438
				AliasHandle alias = NULL;
				::FSNewAlias(NULL, &fsref, &alias);
				if( alias != NULL )
				{
					::AEPutKeyPtr(&outPropertyDesc,pFileF,typeAlias,*alias,::GetHandleSize((Handle)alias));
					::DisposeHandle((Handle) alias);
				}
			}
			SInt32	start = spos;
			SInt32	end = spos+len;
			AEPutKeyPtr(&outPropertyDesc,pStartPositionF,/*#1034 typeLongInteger*/typeSInt32,(Ptr)&start,sizeof(SInt32));
			AEPutKeyPtr(&outPropertyDesc,pEndPositionF,/*#1034 typeLongInteger*/typeSInt32,(Ptr)&end,sizeof(SInt32));
			comment.ConvertFromUTF8ToUTF16();
			{
				AStTextPtr	textptr(comment,0,comment.GetItemCount());
				::AEPutKeyPtr(&outPropertyDesc,pCommentF,'utxt',(Ptr)textptr.GetPtr(),textptr.GetByteCount());
			}
			break;
		}
	  case pIndex:
		{
			AIndex	index;
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(mGroupIndex,mItemIndex,index);
			SInt32	n = index;
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&n),sizeof(n),&outPropertyDesc);
			break;
		}
	  case pCommentF:
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
				break;
			}
			AStTextPtr	textptr(comment,0,comment.GetItemCount());
			::AECreateDesc(typeUTF8Text,(Ptr)(textptr.GetPtr()),textptr.GetByteCount(),&outPropertyDesc);
			break;
		}
	  case pFileF:
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
				break;
			}
			FSRef	fsref;
			if( file.GetFSRef(fsref) == true )
			{
				//B0438::AECreateDesc(typeFSRef,(Ptr)&fsref,sizeof(fsref),&outPropertyDesc);
				//B0438
				AliasHandle alias = NULL;
				::FSNewAlias(NULL, &fsref, &alias);
				if( alias != NULL )
				{
					::AECreateDesc(typeAlias,*alias,::GetHandleSize((Handle)alias),&outPropertyDesc);
					::DisposeHandle((Handle) alias);
				}
			}
			break;
		}
	  case pLineNumber:
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
				break;
			}
			SInt32	num = para+1;
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&num),sizeof(num),&outPropertyDesc);
			break;
		}
	  case pStartPositionF:
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
				break;
			}
			SInt32	num = spos+1;
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&num),sizeof(num),&outPropertyDesc);
			break;
		}
	  case pEndPositionF:
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
				break;
			}
			SInt32	num = spos+len+1;
			::AECreateDesc(/*#1034 typeLongInteger*/typeSInt32,(Ptr)(&num),sizeof(num),&outPropertyDesc);
			break;
		}
	  default:
		LModelObject::GetAEProperty(inProperty,inRequestedType,outPropertyDesc);
		break;
	}
}

void	CASIndexRecord::SetAEProperty(
		DescType		inProperty,
		const AEDesc	&inValue,
		AEDesc&			outAEReply)
{
	switch(inProperty) 
	{
	  case pContents:
		{
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASImp().SetItemFromDesc(mGroupIndex,mItemIndex,inValue);
			break;
		}
	  case pIndex:
		{
			//★
			break;
		}
	  case pFileF:
		{
			CAEDescReader	descreader(&inValue);
			AFileAcc	file;
			descreader.GetFile(file);
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SetItemFile(mGroupIndex,mItemIndex,file);
			break;
		}
	  case pCommentF:
		{
			CAEDescReader	descreader(&inValue);
			AText	text;
			descreader.GetText(text);
			GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SetItemComment(mGroupIndex,mItemIndex,text);
			break;
		}
	  default:
		LModelObject::SetAEProperty(inProperty,inValue,outAEReply);
		break;
	}
}

void	CASIndexRecord::GetSubModelByPosition( DescType inModelID, SInt32 inPosition, AEDesc &outToken ) const
{
	switch(inModelID) 
	{
	  case cIndexRecord:
	  case cIndexGroup:
		ThrowOSErr_(errAENoSuchObject);
		break;
	  default:
		LModelObject::GetSubModelByPosition(inModelID,inPosition,outToken);
		break;
	}
}

void	CASIndexRecord::GetSubModelByName(
		DescType		inModelID,
		Str255			inName,
		AEDesc			&outToken) const
{
	switch(inModelID) 
	{
	  case cIndexRecord:
	  case cIndexGroup:
		{
			ThrowOSErr_(errAENoSuchObject);
			break;
		}
	  default:
		LModelObject::GetSubModelByName(inModelID, inName, outToken);
		break;
	}
}

//
void	CASIndexRecord::HandleDelete( AppleEvent &outAEReply, AEDesc &outResult )
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_DeleteItem(mGroupIndex,mItemIndex);
}

PowerPlant::LModelObject *CASIndexRecord::GetInsertionContainer(DescType inInsertPosition) const
{
	/* HandleCreateElementEventは常にdocumentから行う*/
	return &(GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASImp());
}

#endif
//141011
/*
AppleScript問題調査まとめ
【問題】
openコマンドによりドキュメント生成される前に、次のコマンドが実行されてしまう。
【今までわかったこと】
・openコマンドはFinderで実行されている。
--------------
tell application "Finder"
set testfile to (file "test.txt" in desktop)
end tell
tell application "mi"
open testfile as "UTF-8"
---delay 1
get character code of document 1
get character code of document 1
end tell
--------------
上の結果は下記の通り。
--------------
tell application "Finder"
get file "test.txt" of desktop
--> document file "test.txt" of folder "Desktop" of folder "daisuke" of folder "Users" of startup disk
open document file "test.txt" of folder "Desktop" of folder "daisuke" of folder "Users" of startup disk given <<class chac>>:"UTF-8"
end tell
tell application "mi"
get character code of document 1
--> "Shift_JIS"
get character code of document 1
--> "UTF-8"
end tell
結果：
"UTF-8"
--------------


・CocoaのsetEventHandlerを使っても解決せず。
initWithContentsOfURLの前にhandleAppleEventがコールされる（後にコールされることもある）

・3.0.0b5では問題なかった理由は不明。
3.0.0b6で、一旦内部イベントkInternalEvent_AppleEventOpenDocを投げる方法にしたが、
それを元に戻しても問題は発生する。
3.0.0b5ではXcode3環境でビルドしているので何か設定が関連している可能性はある。
mi.sdefは3.0.0b5でも使用していた。
また、openコマンドをFinderで実行しているのは、3.0.0b5でも同じだった。


・現状、mi.sdefとmiScripting.rを両方入れている
この場合、mi.sdefは正しく認識されないようなので、mi.sdefのみにしたい。

・Info.plistにNSAppleScriptEnabledを入れるとまともに動作しなくなる（set character code to xでエラー）。理由不明。

DTS回答 2014/10/16
====================
Follow-up: 612462051

ATTN: Daisuke Kamiyama
The way you're getting the file yields a Finder object specifier; commands on an object specifier go to the owning application regardless of what "tell" block they're in, so that "open"command is going to Finder.  Turning "testfile" into a raw alias will fix it, either by adding "as alias" to the end of the Finder "get" command, or by using "path to" instead of Finder.

tell application "Finder"
set testfile to (file "test.txt" in desktop) as alias	― <== this will fix your issue
end tell

― OR THIS ―
set testfile to path to desktop & “test.txt” ― <== this will fix your issue also

tell application “mi”
open testfile as "UTF-8"
get character code of document 1
get character code of document 1
end tell

Note: While researching this issue I noticed that most applications (Omni-ware, BBEdit, TextEdit, etc.) typically use the “set theNewDoc to make document with properties: {path:thePath,…}” syntax instead of “open theFile”. The advantage being that it returns an AppleScript object that you can reference directly instead of relying on undefinded behavior by doing the open followed by “front document” (It’s possible that a newly opened document may not be the frontmost document… that behaviour is not in the AppleScript specification.)

--
I hope you find this information of use. Please feel free to contact me by replying to this eMail (using the follow-up
number above) or call me at (408) 974-0668 if you want to discuss this further. I can also be reached via iChat or IM at: geodts@mac.com.
--
George Warner
Schizophrenic Optimization Scientists
Apple DTS.
====================


＜確実に特定のバージョンでテストする方法＞
・対象ドキュメントにて、開くアプリを指定する。
「常にこのアプリケーションで開く」チェックボックスはONにする。
・AppleScriptのtell application "mi"のmiを適当な名前に変更して、実行時にアプリを選択する。


*/


