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

TextDocument.mm

*/

#import <Cocoa/Cocoa.h>
#include "../../AbsFramework/AbsBase/Cocoa.h"
#include "../../AbsFramework/MacWrapper/ODBEditorSuite.h"
#include "AApp.h"
#import "../../AbsFramework/MacImp/AppDelegate.h"

#pragma mark ===========================
#pragma mark [クラス]TextDocument
#pragma mark ===========================

@implementation TextDocument

/**
init
*/
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

/**
dealloc
*/
- (void)dealloc
{
	//LazyObjectsArray解放
	[mParagraphLazyObjectsArray release];
	[mWordLazyObjectsArray release];
	[mCharacterLazyObjectsArray release];
	[mInsertionPointLazyObjectsArray release];
	[mSelectionObjectLazyObjectsArray release];
	//継承処理
	[super dealloc];
}

/**
initWithContentsOfURL
*/
- (id)initWithContentsOfURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError
{
	//fprintf(stderr,"TextDocument#initWithContentsOfURL - start\n");
	self = [super init];//WithContentsOfURL:absoluteURL ofType:typeName error:outError];
	//self.fileURL = absoluteURL;
	if (self) 
	{
		//メンバ変数初期化
		[self initMemberVariables];
		
		//==================テキストエンコーディング取得==================
		//
		AText	tecname;
		//
		NSAppleEventDescriptor*	evtDesc = [[NSAppleEventManager sharedAppleEventManager] currentAppleEvent];
		if( evtDesc != nil )
		{
			NSAppleEventDescriptor*	tecDesc = [evtDesc descriptorForKeyword:keyCharacterCode];
			if( tecDesc != nil )
			{
				NSString*	tecStr = [tecDesc stringValue];
				if( tecStr != nil )
				{
					ACocoa::SetTextFromNSString(tecStr,tecname);
				}
			}
		}
		
		//==================ドキュメント生成==================
		AText	path;
		path.SetCstring([[absoluteURL path] UTF8String]);
		AFileAcc	file;
		file.Specify(path);
		//#1006 ここで直接ドキュメント生成はせず、内部イベントへキューイングことにする ADocumentID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(path,tecname);
		//==================ODB==================
		//#1006
		//ODBデータ取得
		ABool	odbMode = false;
		OSType	odbServer = 0;
		AText	odbSenderToken;
		AText	odbCustomPath;
		if( evtDesc != nil )
		{
			NSAppleEventDescriptor*	odbServerDesc = [evtDesc paramDescriptorForKeyword:keyFileSender];
			NSAppleEventDescriptor*	odbSenderTokenDesc = [evtDesc paramDescriptorForKeyword:keyFileSenderToken];
			NSAppleEventDescriptor*	odbCustomPathDesc = [evtDesc paramDescriptorForKeyword:keyFileCustomPath];
			if( odbServerDesc != nil && odbSenderTokenDesc != nil && odbCustomPathDesc != nil )
			{
				odbServer = [odbServerDesc typeCodeValue];
				NSString*	odbSenderTokenStr = [odbSenderTokenDesc stringValue];
				if( odbSenderTokenStr != nil )
				{
					ACocoa::SetTextFromNSString(odbSenderTokenStr,odbSenderToken);
				}
				NSString*	odbCustomPathStr = [odbCustomPathDesc stringValue];
				if( odbCustomPathStr != nil )
				{
					ACocoa::SetTextFromNSString(odbCustomPathStr,odbCustomPath);
				}
				//#1006 ここで直接ドキュメント生成はせず、内部イベントへキューイングことにする GetApp().SPI_GetTextDocumentByID(docID).SPI_SetODBMode(odbServer,odbSenderToken,odbCustomPath);
				odbMode = true;
			}
		}
		
		//ドキュメント生成するための内部イベントを作成
		//ファイルパス取得
		AText	filepath;
		file.GetPath(filepath);
		//NULL文字区切りでドキュメント情報をtextに格納
		AText	text;
		text.SetText(filepath);
		text.Add(0);
		text.AddText(tecname);
		if( odbMode == true )
		{
			text.Add(0);
			AText	odbServerText;
			odbServerText.SetFromOSType(odbServer);
			text.AddText(odbServerText);
			text.Add(0);
			text.AddText(odbSenderToken);
			text.Add(0);
			text.AddText(odbCustomPath);
		}
		
		//CDocImpオブジェクト生成 #1078
		AObjectID	docImpID = GetApp().GetImp().CreateDocImp();
		//NSDocument(self)設定 #1078
		GetApp().GetImp().GetDocImpByID(docImpID).SetNSDocument(self);
		
		//内部イベントへキューイング
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(docImpID);//#1078
		ABase::PostToMainInternalEventQueue(kInternalEvent_AppleEventOpenDoc,kObjectID_Invalid,0,text,objectIDArray);
	}
	//fprintf(stderr,"TextDocument#initWithContentsOfURL - end\n");
	return self;
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
	mParagraphLazyObjectsArray          = [[LazyObjectsArray alloc] init:self];
	mWordLazyObjectsArray               = [[LazyObjectsArray alloc] init:self];
	mCharacterLazyObjectsArray          = [[LazyObjectsArray alloc] init:self];
	mInsertionPointLazyObjectsArray     = [[LazyObjectsArray alloc] init:self];
	mSelectionObjectLazyObjectsArray    = [[LazyObjectsArray alloc] init:self];
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

- (void)showWindows
{
	//fprintf(stderr,"- (void)showWindows  ");
}

- (void)makeWindowControllers
{
	//fprintf(stderr,"- (void)makeWindowControllers  ");
}

#pragma mark ===========================

#pragma mark --- AppleScript (document)

#pragma mark ===========================

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
			key:@"asDocumentElement" 
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
CASTextDoc::GetSubModelByPosition()に対応
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifierで指定の場合 ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
        //NSIndexSpecifierで指定されているindexを取得
		AIndex	index = [indexSpec index];
		// ---------- paragraph ----------
		if( [[indexSpec key] isEqual:@"asParagraphElement"] == YES )
		{
			try
			{
                //indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfParagraphs];
				}
				//指定indexのparagraphの開始位置・終了位置取得
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetParagraph(0,index,startTextIndex,endTextIndex) == false )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASParagraph*	obj = [[CASParagraph alloc] init:self start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	lazyIndex = [mParagraphLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				//指定paragraphが存在しない場合はエラー
				[CocoaApp appleScriptNoSuchObjectError:@"The paragraph not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- word ----------
		else if( [[indexSpec key] isEqual:@"asWordElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfWords];
				}
				//指定indexのwordの開始位置・終了位置取得
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetWord(0,index,startTextIndex,endTextIndex) == false )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASWord*	obj = [[CASWord alloc] init:self start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	lazyIndex = [mWordLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				//指定wordが存在しない場合はエラー
				[CocoaApp appleScriptNoSuchObjectError:@"The word not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- character ----------
		else if( [[indexSpec key] isEqual:@"asCharacterElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfCharacters];
				}
				//指定indexのcharacterの開始位置・終了位置取得
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetChar(0,index,startTextIndex,endTextIndex) == false )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASCharacter*	obj = [[CASCharacter alloc] init:self start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	lazyIndex = [mCharacterLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				//指定characterが存在しない場合はエラー
				[CocoaApp appleScriptNoSuchObjectError:@"The character not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- insertion point ----------
		else if( [[indexSpec key] isEqual:@"asInsertionPointElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfInsertionPoints];
				}
				//指定indexのinsertion pointの開始位置・終了位置取得
				ATextIndex	textIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetInsertionPoint(0,index,textIndex) == false )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASInsertionPoint*	obj = [[CASInsertionPoint alloc] init:self textIndex:textIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	lazyIndex = [mInsertionPointLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				//指定insertion pointが存在しない場合はエラー
				[CocoaApp appleScriptNoSuchObjectError:@"The insertion point not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- selection object ----------
		else if( [[indexSpec key] isEqual:@"asSelectionObjectElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfSelectionObjects];
				}
				//ドキュメントに対応するウインドウのWindowID取得
				AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
				if( winID == kObjectID_Invalid )   throw 0;
				//現在の選択範囲を取得（矩形選択中は複数になる）
				AArray<ATextIndex>	start,end;
				GetApp().SPI_GetTextWindowByID(winID).SPI_GetSelect(mDocumentID,start,end);
				//indexが範囲外の場合はエラー
				if( index >= start.GetItemCount() )   throw 0;
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				//Lazyオブジェクト生成(autorelease)
				CASSelectionObject*	obj = [[CASSelectionObject alloc] init:self start:start.Get(index) end:end.Get(index)];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	lazyIndex = [mSelectionObjectLazyObjectsArray registerLazyObject:obj];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				//指定selection objectが存在しない場合はエラー
				[CocoaApp appleScriptNoSuchObjectError:@"The selection object not found."];
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
paragraph element (lazy object array)取得
*/
- (NSArray *)asParagraphElement
{
	return [mParagraphLazyObjectsArray array];
}

/**
word element (lazy object array)取得
*/
- (NSArray *)asWordElement
{
	return [mWordLazyObjectsArray array];
}

/**
character element (lazy object array)取得
*/
- (NSArray *)asCharacterElement
{
	return [mCharacterLazyObjectsArray array];
}

/**
insertion point element (lazy object array)取得
*/
- (NSArray *)asInsertionPointElement
{
	return [mInsertionPointLazyObjectsArray array];
}

/**
selection object element (lazy object array)取得
*/
- (NSArray *)asSelectionObjectElement
{
	return [mSelectionObjectLazyObjectsArray array];
}

#pragma mark ===========================

#pragma mark --- property

// -------------------- content プロパティ --------------------

/**
content取得
*/
-(NSAppleEventDescriptor*)asContent
{
	AText	text;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetText(text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
content設定
*/
- (void)setAsContent:(id)value
{
	//documentがread only時はエラー
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	
	//Undoアクションタグ記録
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
	//削除
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(0,GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength());
	//text挿入
	AText	text;
	text.SetFromNSString([value stringValue]);
	text.ConvertLineEndToCR();
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(0,text);
}

// -------------------- name プロパティ --------------------

/**
name取得
*/
- (id)asName
{
	AText	text;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_GetTitle(text);
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
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetTemporaryTitle(text);
}

// -------------------- file プロパティ --------------------

/**
file取得
*/
- (id)asFile
{
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_IsRemoteFileMode() == true )
	{
		AText	urltext;
		GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetRemoteFileURL(urltext);
		NSURL*	url = [NSURL URLWithString:urltext.CreateNSString(true)];
		return [NSAppleEventDescriptor descriptorWithURL:url];
	}
	else if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsFileSpecified() == true )
	{
		AFileAcc	file;
		GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_GetFile(file);
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

// -------------------- modified プロパティ --------------------

/**
modified取得
*/
- (id)asModified
{
	Boolean	b = GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsDirty();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- mode プロパティ --------------------

/**
mode取得
*/
- (id)asMode
{
	AText	text;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetModeDisplayName(text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
mode設定
*/
- (void)setAsMode:(id)mode
{
	AText	modetext;
	modetext.SetFromNSString([mode stringValue]);
	//
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetModeByDisplayName(modetext);
}

// -------------------- text encoding プロパティ --------------------

/**
text encoding取得
*/
- (id)asTextEncoding
{
	AText	text;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextEncoding(text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
text encoding設定
*/
- (void)setAsTextEncoding:(id)tec
{
	//documentがread only時はエラー
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//
	AText	tectext;
	tectext.SetFromNSString([tec stringValue]);
	//利用可能text encoding名でない場合はエラー
	if( ATextEncodingWrapper::CheckTextEncodingAvailability(tectext) == false )
	{
		[CocoaApp appleScriptNotHandledError];
		return;
	}
	//text encoding設定
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetTextEncoding(tectext);
}

// -------------------- return code プロパティ --------------------

/**
return code取得
*/
- (id)asReturnCode
{
	ANumber	returnCode = 0;
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
			returnCode = 0x4C460000;
			break;
		}
	}
	return [NSAppleEventDescriptor descriptorWithEnumCode:returnCode];
}

/**
return code設定
*/
- (void)setAsReturnCode:(id)returnCode
{
	//documentがread only時はエラー
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//
	switch([returnCode enumCodeValue]) 
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
	  case 0x4C460000:
		{
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetReturnCode(returnCode_LF);
			break;
		}
	}
}

// -------------------- type プロパティ --------------------

/**
file type取得
*/
- (id)asType
{
	OSType	type = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFileType();
	AText	text;
	text.SetFromOSType(type);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
file type設定
*/
- (void)setAsType:(id)value
{
	//documentがread only時はエラー
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	AText	text;
	text.SetFromNSString([aeDesc stringValue]);
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFileType(text.GetOSTypeFromText());
}

// -------------------- creator プロパティ --------------------

/**
file creator取得
*/
- (id)asCreator
{
	OSType	creator = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFileCreator();
	AText	text;
	text.SetFromOSType(creator);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
file creator設定
*/
- (void)setAsCreator:(id)value
{
	//documentがread only時はエラー
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	AText	text;
	text.SetFromNSString([aeDesc stringValue]);
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFileCreator(text.GetOSTypeFromText());
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
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
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
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	SInt32	number = [aeDesc int32Value];
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
	CocoaWindow*	win = GetApp().SPI_GetTextWindowByID(winID).NVI_GetWindowImp().GetCocoaWindow();
	[win setWindowOrder:number];
	//★下位レイヤで順序設定して問題ないか？
}

// -------------------- font プロパティ --------------------

/**
font取得
*/
- (id)asFont
{
	AText	text;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFontName(text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
font設定
*/
- (void)setAsFont:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	AText	text;
	text.SetFromNSString([aeDesc stringValue]);
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFontName(text);
}

// -------------------- size プロパティ --------------------

/**
font size取得
*/
- (id)asSize
{
	SInt32	fontsize = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFontSize();
	return [NSAppleEventDescriptor descriptorWithInt32:fontsize];
}

/**
font size設定
*/
- (void)setAsSize:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	SInt32	fontsize = [aeDesc int32Value];
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFontSize(fontsize);
}

// -------------------- index プロパティ --------------------

/**
index（TextDocumentのウインドウz-order順）取得
*/
- (id)asIndex
{
	//index取得
	AIndex	index = GetApp().NVI_GetOrderIndexOfDocument(mDocumentID,kDocumentType_Text,true);
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
index（TextDocumentのウインドウz-order順）設定
*/
- (void)setAsIndex:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//指定index（number)取得
	SInt32	num = [aeDesc int32Value];
	//対象window取得
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
	if( num <= 1 )
	{
		//index=1に設定する場合は、最前面にする
		GetApp().NVI_GetWindowByID(winID).NVI_SelectWindow();
		GetApp().NVI_GetWindowByID(winID).NVI_SelectTabByDocumentID(mDocumentID);
	}
	else
	{
		//index=1以外に設定する場合は、直前の順番のウインドウ（タブ）の後ろに配置する
		
		//ADocumentリスト取得
		AArray<ADocumentID>	docIDArray;
		GetApp().NVI_GetOrderedDocumentIDArray(docIDArray,kDocumentType_Text,true);
		//現在のindexと同じなら何もせずreturn、現在のindexよりも後に配置するなら指定indexを+1する。
		AIndex	currentindex = docIDArray.Find(mDocumentID);
		if( currentindex == kIndex_Invalid )
		{
			return;
		}
		ANumber	currentnum = currentindex+1;
		if( num == currentnum )   return;
		if( currentnum < num )
		{
			num++;
		}
		//移動先z-orderの直前のドキュメントのウインドウ・タブを取得
		AIndex	prevDocIndex = (num-1)-1;
		if( prevDocIndex < 0 )
		{
			return;
		}
		ADocumentID	prevDocID = docIDArray.Get(prevDocIndex);
		AWindowID	prevWinID = GetApp().SPI_GetTextDocumentByID(prevDocID).SPI_GetFirstWindowID();
		AIndex	prevTabIndex = GetApp().NVI_GetWindowByID(prevWinID).NVI_GetTabIndexByDocumentID(prevDocID);
		AIndex	prevTabZOrder = GetApp().NVI_GetWindowByID(prevWinID).NVI_GetTabZOrder(prevTabIndex);
		if( prevWinID != winID )
		{
			//直前のドキュメントとウインドウが違う場合は、そのウインドウの背後に移動する
			GetApp().NVI_GetWindowByID(winID).NVI_SendBehind(prevWinID);
		}
		else
		{
			//直前のドキュメントとウインドウが同じ場合は、そのウインドウ内の直前のタブの背後に移動する
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
}

// -------------------- selection プロパティ --------------------

/**
selection取得
*/
-(NSAppleEventDescriptor*)asSelection
{
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
	if( winID == kObjectID_Invalid )
	{
		[CocoaApp appleScriptNotHandledError];
		return nil;
	}
	AText	text;
	GetApp().SPI_GetTextWindowByID(winID).SPI_GetSelectedText(mDocumentID,text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}
//#1145
-(NSAppleEventDescriptor*)asSelected
{
	return [self asSelection];
}

/**
selection設定
*/
-(void)setAsSelection:(id)value
{
	//documentがread only時はエラー
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//文字列取得
	AText	text;
	text.SetFromNSString([value stringValue]);
	text.ConvertLineEndToCR();
	//ウインドウ取得
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
	if( winID == kObjectID_Invalid )
	{
		[CocoaApp appleScriptNotHandledError];
		return;
	}
	//選択範囲取得
	AArray<ATextIndex>	start, end;
	GetApp().SPI_GetTextWindowByID(winID).SPI_GetSelect(mDocumentID,start,end);
	if( start.GetItemCount() == 1 )
	{
		//Undoアクションタグ記録
		GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
		//選択範囲削除
		GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(start.Get(0),end.Get(0));
		//挿入
		GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(start.Get(0),text);
	}
}
//#1145
-(void)setAsSelected:(id)value
{
	[self setAsSelection:value];
}

#pragma mark ===========================

#pragma mark --- command handler

/**
saveコマンド
*/
- (id)handleSaveScriptCommand:(MISaveCommand*)command
{
	//documentがread only時はエラー
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//
	NSDictionary *args = [command evaluatedArguments];
	if( args != nil )
	{
		//引数inの処理（ファイル保存先設定）
		[self handleSaveAndCloseCommand_InArg:args];
		//引数asの処理
		NSNumber*	argAs = [args valueForKey:@"as"];
		if( argAs != nil )
		{
			//file type設定
			OSType	fileType = [argAs intValue];
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFileType(fileType);
		}
	}
	//保存実行
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_Save(false);
    return nil;
}

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
		//引数inの処理（ファイル保存先設定）
		[self handleSaveAndCloseCommand_InArg:args];
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
			GetApp().SPI_TryCloseTextDocument(mDocumentID);
			break;
		}
	  case 'yes ':
		{
			if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsDirty() == true )
			{
				GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_Save(false);
			}
			GetApp().SPI_TryCloseTextDocument(mDocumentID);
			break;
		}
	  case 'no  ':
		{
			GetApp().SPI_ExecuteCloseTextDocument(mDocumentID);
			break;
		}
	}
    return nil;
}

/**
引数inの処理（ファイル保存先設定）
*/
- (void)handleSaveAndCloseCommand_InArg:(NSDictionary*)args
{
	NSURL*	argIn = [args valueForKey:@"in"];
	if( argIn != nil )
	{
		AText	path;
		path.SetCstring([[argIn path] UTF8String]);
		//file "～"と指定した時はURLにfile:///:Users:daisuke:...などと入っており、UTF8Stringにより/:Users:daisuke...などとなるので、最初の/を削除し、:を/へ変換する。
		if( path.CompareMin("/:") == true )
		{
			path.Delete1(0);
			path.ReplaceChar(':','/');
		}
		//ファイル設定
		AFileAcc	file;
		file.Specify(path);
		file.CreateFile();
		if( file.Exist() == true )
		{
			GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_SpecifyFile(file);
		}
	}
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
	  case kASObjectCode_Paragraph:
		{
			count = [self countOfParagraphs];
			break;
		}
	  case kASObjectCode_Word:
		{
			count = [self countOfWords];
			break;
		}
	  case kASObjectCode_Character:
		{
			count = [self countOfCharacters];
			break;
		}
	  case kASObjectCode_InsertionPoint:
		{
			count = [self countOfInsertionPoints];
			break;
		}
	  case kASObjectCode_SelectionObject:
		{
			count = [self countOfSelectionObjects];
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
count paragraphs
*/
- (AItemCount)countOfParagraphs
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetParagraphCount(0,GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength());
}

/**
count words
*/
- (AItemCount)countOfWords
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetWordCount(0,GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength());
}

/**
count characters
*/
- (AItemCount)countOfCharacters
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetCharCount(0,GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength());
}

/**
count insertion points
*/
- (AItemCount)countOfInsertionPoints
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetCharCount(0,GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength()) +1;
}

/**
count selection objects
*/
- (AItemCount)countOfSelectionObjects
{
	//ドキュメントに対応するウインドウのWindowID取得
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
	//現在の選択範囲を取得（矩形選択中は複数になる）
	AArray<ATextIndex>	start,end;
	GetApp().SPI_GetTextWindowByID(winID).SPI_GetSelect(mDocumentID,start,end);
	return start.GetItemCount();
}

/**
selectコマンドハンドラ
*/
- (id)handleSelectScriptCommand:(MIDeleteCommand*)command
{
	GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_RevealDocumentWithAView();
    return nil;
}

@end

#pragma mark ===========================
#pragma mark [クラス]CASTextDocElement
#pragma mark ===========================
@implementation CASTextDocElement

/**
init
*/
- (id)init:(TextDocument*)textDocument start:(AIndex)start end:(AIndex)end
{
	self = [super init];
	if(self) 
	{
		//ドキュメント
		mTextDocument = textDocument;
		mDocumentID = [textDocument documentID];
		//テキスト開始位置・終了位置
		mStartTextIndex = start;
		mEndTextIndex = end;
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
テキスト開始位置取得
*/
- (ATextIndex)startTextIndex;
{
	return mStartTextIndex;
}

/**
テキスト終了位置取得
*/
- (ATextIndex)endTextIndex
{
	return mEndTextIndex;
}

/**
TextDocument取得
*/
- (TextDocument*)textDocument
{
	return mTextDocument;
}

#pragma mark ===========================

#pragma mark --- property

/**
content取得
*/
-(NSAppleEventDescriptor*)asContent
{
	AText	text;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetText(mStartTextIndex,mEndTextIndex,text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
content設定
*/
- (void)setAsContent:(id)value
{
	//documentがread only時はエラー
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//Undoアクションタグ記録
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
	//削除
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(mStartTextIndex,mEndTextIndex);
	//text挿入
	AText	text;
	text.SetFromNSString([value stringValue]);
	text.ConvertLineEndToCR();
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(mStartTextIndex,text);
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
	//documentがread only時はエラー
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//Undoアクションタグ記録
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
	//削除
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(mStartTextIndex,mEndTextIndex);
	return nil;
}

/**
selectコマンドハンドラ
*/
- (id)handleSelectScriptCommand:(MIDeleteCommand*)command
{
	//開始位置、終了位置取得
	ATextIndex	spos = mStartTextIndex;
	ATextIndex	epos = mEndTextIndex;
	//
	NSDictionary *args = [command evaluatedArguments];
	if( args != nil )
	{
		//to引数があるときは、そのオブジェクトまでを選択範囲にする
		id	toObj = [args valueForKey:@"to"];
		if( toObj != nil )
		{
			if( [toObj isKindOfClass:[CASTextDocElement class]] == YES )
			{
				CASTextDocElement*	toTextDocElement = (CASTextDocElement*)toObj;
				ATextIndex	spos2 = [toTextDocElement startTextIndex];
				if( spos2 < spos )   spos = spos2;
				ATextIndex	epos2 = [toTextDocElement endTextIndex];
				if( epos < epos2 )   epos = epos2;
			}
		}
	}
	//ウインドウ取得
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
	if( winID == kObjectID_Invalid )
	{
		[CocoaApp appleScriptNotHandledError];
	}
	else
	{
		//選択
		GetApp().SPI_GetTextWindowByID(winID).SPI_SetSelect(mDocumentID,spos,epos);
	}
    return nil;
}

@end

#pragma mark ===========================
#pragma mark [クラス]CASParagraph
#pragma mark ===========================
@implementation CASParagraph

/**
init
*/
- (id)init:(TextDocument*)textDocument start:(AIndex)start end:(AIndex)end
{
	self = [super init:textDocument start:start end:end];
	if(self) 
	{
		//子elementのlazy object格納array生成
		mWordLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
		mCharacterLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
		mInsertionPointLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
	}
	return self;
}

/**
dealloc
*/
- (void)dealloc
{
	//子elementのlazy object格納array解放
	[mWordLazyObjectsArray release];
	[mCharacterLazyObjectsArray release];
	[mInsertionPointLazyObjectsArray release];
	//継承処理
	[super dealloc];
}

#pragma mark ===========================

#pragma mark --- object specifier

/**
objectSpecifier取得
*/
- (NSScriptObjectSpecifier *)objectSpecifier 
{
	//paragraph index取得
	ATextPoint	textpoint = {0};
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextPointFromTextIndex(mStartTextIndex,textpoint,true);
	AIndex	paraIndex = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetParagraphIndexByLineIndex(textpoint.y);
	//NSIndexSpecifier（container:document, index:段落index）を返す
	return [[[NSIndexSpecifier alloc] initWithContainerClassDescription:[[mTextDocument objectSpecifier] keyClassDescription] 
			containerSpecifier:[mTextDocument objectSpecifier] 
			key:@"asParagraphElement" 
	index:paraIndex] autorelease];
}

#pragma mark ===========================

#pragma mark --- element

/**
specifierで指定された子elementのindex（子element array内でのindex）を取得する
CASParagraph::GetSubModelByPosition()に対応
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifierで指定の場合 ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//
		AIndex	index = [indexSpec index];
		// ---------- word ----------
		if( [[indexSpec key] isEqual:@"asWordElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfWords];
				}
				//指定indexのwordの開始位置・終了位置取得
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetWord(mStartTextIndex,index,startTextIndex,endTextIndex) == false )   throw 0;
				if( startTextIndex >= mEndTextIndex )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASWord*	obj = [[CASWord alloc] init:mTextDocument start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	index = [mWordLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				[CocoaApp appleScriptNoSuchObjectError:@"The word not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- character ----------
		else if( [[indexSpec key] isEqual:@"asCharacterElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfCharacters];
				}
				//指定indexのcharacterの開始位置・終了位置取得
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetChar(mStartTextIndex,index,startTextIndex,endTextIndex) == false )   throw 0;
				if( startTextIndex >= mEndTextIndex )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASCharacter*	obj = [[CASCharacter alloc] init:mTextDocument start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	index = [mCharacterLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				[CocoaApp appleScriptNoSuchObjectError:@"The character not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- insertion point ----------
		else if( [[indexSpec key] isEqual:@"asInsertionPointElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfInsertionPoints];
				}
				//指定indexのinsertion pointの開始位置・終了位置取得
				ATextIndex	textIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetInsertionPoint(mStartTextIndex,index,textIndex) == false )   throw 0;
				if( textIndex > mEndTextIndex )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASInsertionPoint*	obj = [[CASInsertionPoint alloc] init:mTextDocument textIndex:textIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	index = [mInsertionPointLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				[CocoaApp appleScriptNoSuchObjectError:@"The insertion point not found."];
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
		// ==================== 上記以外で指定の場合はnilを返す ====================
		// プロパティ取得の場合もここに来るので、エラーにせず、nilを返す
		return nil;
	}
}

/**
word element (lazy object array)取得
*/
- (NSArray *)asWordElement
{
	return [mWordLazyObjectsArray array];
}

/**
character element (lazy object array)取得
*/
- (NSArray *)asCharacterElement
{
	return [mCharacterLazyObjectsArray array];
}

/**
insertion point element (lazy object array)取得
*/
- (NSArray *)asInsertionPointElement
{
	return [mInsertionPointLazyObjectsArray array];
}

#pragma mark ===========================

#pragma mark --- property

// -------------------- index プロパティ --------------------

/**
index（ドキュメントの最初からの段落番号）取得
*/
- (id)asIndex
{
	//paragraph index取得
	ATextPoint	textpoint = {0};
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextPointFromTextIndex(mStartTextIndex,textpoint,true);
	AIndex	index = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetParagraphIndexByLineIndex(textpoint.y);
	if( index != kIndex_Invalid )
	{
		return [NSAppleEventDescriptor descriptorWithInt32:index+1];
	}
	else
	{
		return nil;
	}
}


#pragma mark ===========================

#pragma mark --- command handler

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
	  case kASObjectCode_Word:
		{
			count = [self countOfWords];
			break;
		}
	  case kASObjectCode_Character:
		{
			count = [self countOfCharacters];
			break;
		}
	  case kASObjectCode_InsertionPoint:
		{
			count = [self countOfInsertionPoints];
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
count paragraphs
*/
- (AItemCount)countOfWords
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetWordCount(mStartTextIndex,mEndTextIndex);
}

/**
count words
*/
- (AItemCount)countOfCharacters
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetCharCount(mStartTextIndex,mEndTextIndex);
}

/**
count insertion points
*/
- (AItemCount)countOfInsertionPoints
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetCharCount(mStartTextIndex,mEndTextIndex) +1;
}

@end


#pragma mark ===========================
#pragma mark [クラス]CASWord
#pragma mark ===========================
@implementation CASWord

/**
init
*/
- (id)init:(TextDocument*)textDocument start:(AIndex)start end:(AIndex)end
{
	self = [super init:textDocument start:start end:end];
	if(self) 
	{
		//子elementのlazy object格納array生成
		mCharacterLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
		mInsertionPointLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
	}
	return self;
}

/**
dealloc
*/
- (void)dealloc
{
	//子elementのlazy object格納array解放
	[mCharacterLazyObjectsArray release];
	[mInsertionPointLazyObjectsArray release];
	//継承処理
	[super dealloc];
}

#pragma mark ===========================

#pragma mark --- object specifier

/**
objectSpecifier取得
*/
- (NSScriptObjectSpecifier *)objectSpecifier 
{
	//word index取得
	AIndex	wordIndex = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetWordCount(0,mStartTextIndex);
	//
	return [[[NSIndexSpecifier alloc] initWithContainerClassDescription:[[mTextDocument objectSpecifier] keyClassDescription] 
			containerSpecifier:[mTextDocument objectSpecifier] 
			key:@"asWordElement" 
	index:wordIndex] autorelease];
}

#pragma mark ===========================

#pragma mark --- element

/**
specifierで指定された子elementのindex（子element array内でのindex）を取得する
CASWord::GetSubModelByPosition()に対応
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifierで指定の場合 ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//
		AIndex	index = [indexSpec index];
		// ---------- character ----------
		if( [[indexSpec key] isEqual:@"asCharacterElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfCharacters];
				}
				//指定indexのcharacterの開始位置・終了位置取得
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetChar(mStartTextIndex,index,startTextIndex,endTextIndex) == false )   throw 0;
				if( startTextIndex >= mEndTextIndex )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASCharacter*	obj = [[CASCharacter alloc] init:mTextDocument start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	index = [mCharacterLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				[CocoaApp appleScriptNoSuchObjectError:@"The character not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- insertion point ----------
		else if( [[indexSpec key] isEqual:@"asInsertionPointElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfInsertionPoints];
				}
				//指定indexのinsertion pointの開始位置・終了位置取得
				ATextIndex	textIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetInsertionPoint(mStartTextIndex,index,textIndex) == false )   throw 0;
				if( textIndex > mEndTextIndex )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASInsertionPoint*	obj = [[CASInsertionPoint alloc] init:mTextDocument textIndex:textIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	index = [mInsertionPointLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				[CocoaApp appleScriptNoSuchObjectError:@"The insertion point not found."];
				return nil;
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
		// ==================== 上記以外で指定の場合はnilを返す ====================
		// プロパティ取得の場合もここに来るので、エラーにせず、nilを返す
		return nil;
	}
}

/**
character element (lazy object array)取得
*/
- (NSArray *)asCharacterElement
{
	return [mCharacterLazyObjectsArray array];
}

/**
insertion point element (lazy object array)取得
*/
- (NSArray *)asInsertionPointElement
{
	return [mInsertionPointLazyObjectsArray array];
}


#pragma mark ===========================

#pragma mark --- property

// -------------------- index プロパティ --------------------

/**
index（ドキュメントの最初からの単語番号）取得
*/
- (id)asIndex
{
	AIndex	index = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetWordCount(0,mStartTextIndex);
	if( index != kIndex_Invalid )
	{
		return [NSAppleEventDescriptor descriptorWithInt32:index+1];
	}
	else
	{
		return nil;
	}
}

#pragma mark ===========================

#pragma mark --- command handler

/**
deleteコマンドハンドラ
*/
- (id)handleDeleteScriptCommand:(MIDeleteCommand*)command
{
	//read onlyの場合はエラー
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError];
	}
	
	//Undoアクションタグ記録
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
	//削除
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(mStartTextIndex,mEndTextIndex);
	//前後スペース調整
	AUChar	ch1 = kUChar_Space, ch2 =0;
	if( mStartTextIndex > 0 )   ch1 = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextChar(mStartTextIndex-1);
	if( mStartTextIndex < GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength() )   
	ch2 = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextChar(mStartTextIndex);
	if( ch1 == kUChar_Space && ch2 == kUChar_Space )
	{
		GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(mStartTextIndex,mStartTextIndex+1);
	}
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
	  case kASObjectCode_Character:
		{
			count = [self countOfCharacters];
			break;
		}
	  case kASObjectCode_InsertionPoint:
		{
			count = [self countOfInsertionPoints];
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
count characters
*/
- (AItemCount)countOfCharacters
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetCharCount(mStartTextIndex,mEndTextIndex);
}

/**
count insertion points
*/
- (AItemCount)countOfInsertionPoints
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetCharCount(mStartTextIndex,mEndTextIndex) +1;
}

@end

#pragma mark ===========================
#pragma mark [クラス]CASCharacter
#pragma mark ===========================
@implementation CASCharacter

/**
init
*/
- (id)init:(TextDocument*)textDocument start:(AIndex)start end:(AIndex)end
{
	self = [super init:textDocument start:start end:end];
	if(self) 
	{
		//子elementのlazy object格納array生成
		mInsertionPointLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
	}
	return self;
}

/**
dealloc
*/
- (void)dealloc
{
	//子elementのlazy object格納array解放
	[mInsertionPointLazyObjectsArray release];
	//継承処理
	[super dealloc];
}

#pragma mark ===========================

#pragma mark --- object specifier

/**
objectSpecifier取得
*/
- (NSScriptObjectSpecifier *)objectSpecifier 
{
	//character index取得
	AIndex	charIndex = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(0,mStartTextIndex);
	//
	return [[[NSIndexSpecifier alloc] initWithContainerClassDescription:[[mTextDocument objectSpecifier] keyClassDescription] 
			containerSpecifier:[mTextDocument objectSpecifier] 
			key:@"asCharacterElement" 
	index:charIndex] autorelease];
}

#pragma mark ===========================

#pragma mark --- element

/**
specifierで指定された子elementのindex（子element array内でのindex）を取得する
CASCharacter::GetSubModelByPosition()に対応
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifierで指定の場合 ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//
		AIndex	index = [indexSpec index];
		// ---------- insertion point ----------
		if( [[indexSpec key] isEqual:@"asInsertionPointElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfInsertionPoints];
				}
				//指定indexのinsertion pointの開始位置・終了位置取得
				ATextIndex	textIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetInsertionPoint(mStartTextIndex,index,textIndex) == false )   throw 0;
				if( textIndex > mEndTextIndex )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASInsertionPoint*	obj = [[CASInsertionPoint alloc] init:mTextDocument textIndex:textIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	index = [mInsertionPointLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				[CocoaApp appleScriptNoSuchObjectError:@"The insertion point not found."];
				return nil;
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
		// ==================== 上記以外で指定の場合はnilを返す ====================
		// プロパティ取得の場合もここに来るので、エラーにせず、nilを返す
		return nil;
	}
}

/**
insertion point element (lazy object array)取得
*/
- (NSArray *)asInsertionPointElement
{
	return [mInsertionPointLazyObjectsArray array];
}

#pragma mark ===========================

#pragma mark --- property

// -------------------- index プロパティ --------------------

/**
index（ドキュメントの最初からのcharacter番号）取得
*/
- (id)asIndex
{
	AIndex	index = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(0,mStartTextIndex);
	if( index != kIndex_Invalid )
	{
		return [NSAppleEventDescriptor descriptorWithInt32:index+1];
	}
	else
	{
		return nil;
	}
}

#pragma mark ===========================

#pragma mark --- command handler

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
	  case kASObjectCode_InsertionPoint:
		{
			count = [self countOfInsertionPoints];
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
count insertion points
*/
- (AItemCount)countOfInsertionPoints
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetCharCount(mStartTextIndex,mEndTextIndex) +1;
}

@end

#pragma mark ===========================
#pragma mark [クラス]CASInsertionPoint
#pragma mark ===========================
@implementation CASInsertionPoint

/**
init
*/
- (id)init:(TextDocument*)textDocument textIndex:(AIndex)textIndex
{
	self = [super init:textDocument start:textIndex end:textIndex];
	if(self) 
	{
	}
	return self;
}

#pragma mark ===========================

#pragma mark --- object specifier

/**
objectSpecifier取得
*/
- (NSScriptObjectSpecifier *)objectSpecifier 
{
	//character index取得
	AIndex	charIndex = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(0,mStartTextIndex);
	//
	return [[[NSIndexSpecifier alloc] initWithContainerClassDescription:[[mTextDocument objectSpecifier] keyClassDescription] 
			containerSpecifier:[mTextDocument objectSpecifier] 
			key:@"asInsertionPointElement" 
	index:charIndex] autorelease];
}

#pragma mark ===========================

#pragma mark --- property

// -------------------- index プロパティ --------------------

/**
index（ドキュメントの最初からのinsertion point番号）取得
*/
- (id)asIndex
{
	AIndex	index = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(0,mStartTextIndex);
	if( index != kIndex_Invalid )
	{
		return [NSAppleEventDescriptor descriptorWithInt32:index+1];
	}
	else
	{
		return nil;
	}
}

#pragma mark ===========================

#pragma mark --- command handler

@end


#pragma mark ===========================
#pragma mark [クラス]CASSelectionObject
#pragma mark ===========================
@implementation CASSelectionObject

/**
init
*/
- (id)init:(TextDocument*)textDocument start:(AIndex)start end:(AIndex)end
{
	self = [super init:textDocument start:start end:end];
	if(self) 
	{
		//子elementのlazy object格納array生成
		mParagraphLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
		mWordLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
		mCharacterLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
		mInsertionPointLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
	}
	return self;
}

/**
dealloc
*/
- (void)dealloc
{
	//子elementのlazy object格納array解放
	[mParagraphLazyObjectsArray release];
	[mWordLazyObjectsArray release];
	[mCharacterLazyObjectsArray release];
	[mInsertionPointLazyObjectsArray release];
	//継承処理
	[super dealloc];
}

#pragma mark ===========================

#pragma mark --- object specifier

/**
objectSpecifier取得
*/
- (NSScriptObjectSpecifier *)objectSpecifier 
{
	//selection objectのindexはとりあえず0固定で返す。
	//
	return [[[NSIndexSpecifier alloc] initWithContainerClassDescription:[[mTextDocument objectSpecifier] keyClassDescription] 
			containerSpecifier:[mTextDocument objectSpecifier] 
			key:@"asSelectionObjectElement" 
	index:0] autorelease];
}

#pragma mark ===========================

#pragma mark --- element

/**
specifierで指定された子elementのindex（子element array内でのindex）を取得する
CASSelectionObject::GetSubModelByPosition()に対応
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifierで指定の場合 ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//
		AIndex	index = [indexSpec index];
		// ---------- paragraph ----------
		if( [[indexSpec key] isEqual:@"asParagraphElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfParagraphs];
				}
				//指定indexのparagraphの開始位置・終了位置取得
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetParagraph(mStartTextIndex,index,startTextIndex,endTextIndex) == false )   throw 0;
				if( startTextIndex > mEndTextIndex )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASParagraph*	obj = [[CASParagraph alloc] init:mTextDocument start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	index = [mParagraphLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				//指定paragraphが存在しない場合はエラー
				[CocoaApp appleScriptNoSuchObjectError:@"The paragraph not found."];
				return nil;
			}
		}
		// ---------- word ----------
		else if( [[indexSpec key] isEqual:@"asWordElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfWords];
				}
				//指定indexのwordの開始位置・終了位置取得
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetWord(mStartTextIndex,index,startTextIndex,endTextIndex) == false )   throw 0;
				if( startTextIndex > mEndTextIndex )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASWord*	obj = [[CASWord alloc] init:mTextDocument start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	index = [mWordLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				//指定wordが存在しない場合はエラー
				[CocoaApp appleScriptNoSuchObjectError:@"The word not found."];
				return nil;
			}
		}
		// ---------- character ----------
		else if( [[indexSpec key] isEqual:@"asCharacterElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfCharacters];
				}
				//指定indexのcharacterの開始位置・終了位置取得
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetChar(mStartTextIndex,index,startTextIndex,endTextIndex) == false )   throw 0;
				if( startTextIndex > mEndTextIndex )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASCharacter*	obj = [[CASCharacter alloc] init:mTextDocument start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	index = [mCharacterLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				//指定characterが存在しない場合はエラー
				[CocoaApp appleScriptNoSuchObjectError:@"The character not found."];
				return nil;
			}
		}
		// ---------- insertion point ----------
		else if( [[indexSpec key] isEqual:@"asInsertionPointElement"] == YES )
		{
			try
			{
				//indexがマイナスの場合は最後から数える（マイナスは"last"等で指定された場合）
				if( index < 0 )
				{
					index += [self countOfInsertionPoints];
				}
				//指定indexのinsertion pointの開始位置・終了位置取得
				ATextIndex	textIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetInsertionPoint(mStartTextIndex,index,textIndex) == false )   throw 0;
				if( textIndex > mEndTextIndex )   throw 0;
				//Lazyオブジェクト生成(autorelease)
				CASInsertionPoint*	obj = [[CASInsertionPoint alloc] init:mTextDocument textIndex:textIndex];
				[obj autorelease];
				//Lazyオブジェクト登録、index取得
				NSInteger	index = [mInsertionPointLazyObjectsArray registerLazyObject:obj];
				//返り値Array生成
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				//指定insertion pointが存在しない場合はエラー
				[CocoaApp appleScriptNoSuchObjectError:@"The insertion point not found."];
				return nil;
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
		// ==================== 上記以外で指定の場合はnilを返す ====================
		// プロパティ取得の場合もここに来るので、エラーにせず、nilを返す
		return nil;
	}
}

/**
paragraph element (lazy object array)取得
*/
- (NSArray *)asParagraphElement
{
	return [mParagraphLazyObjectsArray array];
}

/**
word element (lazy object array)取得
*/
- (NSArray *)asWordElement
{
	return [mWordLazyObjectsArray array];
}

/**
character element (lazy object array)取得
*/
- (NSArray *)asCharacterElement
{
	return [mCharacterLazyObjectsArray array];
}

/**
insertion point element (lazy object array)取得
*/
- (NSArray *)asInsertionPointElement
{
	return [mInsertionPointLazyObjectsArray array];
}


#pragma mark ===========================

#pragma mark --- command handler

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
	  case kASObjectCode_Paragraph:
		{
			count = [self countOfParagraphs];
			break;
		}
	  case kASObjectCode_Word:
		{
			count = [self countOfWords];
			break;
		}
	  case kASObjectCode_Character:
		{
			count = [self countOfCharacters];
			break;
		}
	  case kASObjectCode_InsertionPoint:
		{
			count = [self countOfInsertionPoints];
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
count paragraphs
*/
- (AItemCount)countOfParagraphs
{
	if( mStartTextIndex == mEndTextIndex )
	{
		return 1;
	}
	else
	{
		return GetApp().SPI_GetTextDocumentByID(mDocumentID).
		SPI_ASGetParagraphCount(mStartTextIndex,mEndTextIndex);
	}
}

/**
count words
*/
- (AItemCount)countOfWords
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetWordCount(mStartTextIndex,mEndTextIndex);
}

/**
count characters
*/
- (AItemCount)countOfCharacters
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetCharCount(mStartTextIndex,mEndTextIndex);
}

/**
count insertion points
*/
- (AItemCount)countOfInsertionPoints
{
	return GetApp().SPI_GetTextDocumentByID(mDocumentID).
	SPI_ASGetCharCount(mStartTextIndex,mEndTextIndex) +1;
}

@end

