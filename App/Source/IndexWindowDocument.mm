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
#pragma mark [�N���X]IndexWindowDocument
#pragma mark ===========================

@implementation IndexWindowDocument

- (id)init
{
	self = [super init];
	if (self) 
	{
		//�����o�ϐ�������
		[self initMemberVariables];
	}
	return self;
}

- (void)dealloc
{
	//LazyObjectsArray���
	[mIndexGroupLazyObjectsArray release];
	[mIndexRecordLazyObjectsArray release];
	//�p������
	[super dealloc];
}

/**
�����o�[�ϐ�������
init�n���\�b�h����R�[�������
*/
- (void)initMemberVariables
{
	//DocumentID������
	mDocumentID = kObjectID_Invalid;
	//LazyObjectsArray����
	mIndexGroupLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
	mIndexRecordLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
}

/**
ADocument��DocumentID�ݒ�
�iADocument�̐������ɃR�[�������j
*/
- (void)setDocumentID:(ADocumentID)docID
{
	mDocumentID = docID;
}

/**
ADocument��DocumentID�擾
*/
- (ADocumentID)documentID
{
	return mDocumentID;
}

/**
���s�ǂݍ��݂��邩�ۂ�
NO��Ԃ�
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
objectSpecifier�擾
*/
- (NSScriptObjectSpecifier *)objectSpecifier 
{
	//Document��uniqID���擾
	AUniqID	docUniqID = GetApp().NVI_GetDocumentUniqID(mDocumentID);
	//application��class description�擾
	NSScriptClassDescription *containerClassDesc = (NSScriptClassDescription *)
			[NSScriptClassDescription classDescriptionForClass:[CocoaApp class]];
	//NSUniqueIDSpecifier�icontainer:application uniqueID:uniqID�j��Ԃ�
	return [[[NSUniqueIDSpecifier alloc] initWithContainerClassDescription:containerClassDesc 
			containerSpecifier:nil 
			key:@"asIndexwindow" 
	uniqueID:[NSNumber numberWithInt:docUniqID.val]] autorelease];
}

/**
uniqueid�擾
*/
- (NSNumber*)uniqueid
{
	AUniqID	docUniqID = GetApp().NVI_GetDocumentUniqID(mDocumentID);
	return [NSNumber numberWithInt:docUniqID.val];
}

#pragma mark ===========================

#pragma mark --- element

/**
specifier�Ŏw�肳�ꂽ�qelement��index�i�qelement array���ł�index�j���擾����
CASIndexWindowDoc::GetSubModelByPosition()�ɑΉ�
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifier�Ŏw��̏ꍇ ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//
		AIndex	index = [indexSpec index];
		// ---------- indexgroup ----------
		if( [[indexSpec key] isEqual:@"asIndexgroupElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfIndexgroups];
				}
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASIndexGroup*	obj = [[CASIndexGroup alloc] init:self groupIndex:index];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	lazyIndex = [mIndexGroupLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
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
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfIndexrecords];
				}
				//�w��index��indexrecord��groupIndex, itemIndex�擾
				AIndex	groupIndex = 0, itemIndex = 0;
				if( GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).
					SPI_GetIndexFromASRecordIndex(index,groupIndex,itemIndex) == false )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASIndexRecord*	obj = [[CASIndexRecord alloc] init:self groupIndex:groupIndex itemIndex:itemIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	lazyIndex = [mIndexRecordLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
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
		// ---------- ��L�ȊO�I�u�W�F�N�g�̏ꍇ�̓G���[ ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else
	{
		// ==================== ��L�ȊO�Ŏw��̏ꍇ�̓G���[ ====================
		// �v���p�e�B�擾�̏ꍇ�������ɗ���̂ŁA�G���[�ɂ����Anil��Ԃ�
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

// -------------------- content �v���p�e�B --------------------

/**
content�擾
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
content�ݒ�
*/
- (void)setAsContent:(id)value
{
	NSAppleEventDescriptor*	aeDescList = (NSAppleEventDescriptor*)value;
	//�S�폜
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_DeleteAllGroup();
	//�O���[�v��1�ǉ��i���̃O���[�v�ɑS�Ă�indexrecord������j
	AFileAcc	basefolder;//#465�i�����̓x�[�X�t�H���_���Ή��j
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_AddGroup(GetEmptyText(),GetEmptyText(),basefolder);//#465
	//������aeDescList�̊e�v�f���Ƃ̃��[�v
	NSInteger count = [aeDescList numberOfItems];
	for( AIndex i = 0; i < count ;i++ )
	{
		//�e�v�f���擾
		NSAppleEventDescriptor*	aeDesc = [aeDescList descriptorAtIndex:i+1];
		//indexrecord��ǉ�
		GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).
				SPI_InsertItem_TextPosition(0,i,
											GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),
											GetEmptyText(),//#465
											0,0,0,GetEmptyText());
		//indexrecord�ɒl��ݒ�
		[self setItemFromDesc:0 itemIndex:i ae:aeDesc];
	}
}

// -------------------- index �v���p�e�B --------------------

/**
index�擾
*/
- (id)asIndex
{
	return [NSAppleEventDescriptor descriptorWithInt32:1];
}

/**
index�iTextDocument�̃E�C���h�Ez-order���j�ݒ�
*/
- (void)setAsIndex:(id)value
{
	//�����Ȃ�
}

// -------------------- file �v���p�e�B --------------------

/**
file�擾
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

// -------------------- name �v���p�e�B --------------------

/**
name�擾
*/
- (id)asName
{
	AText	text;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).NVI_GetTitle(text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
name�ݒ�
*/
- (void)setAsName:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//temporary title�ݒ�
	AText	text;
	text.SetFromNSString([aeDesc stringValue]);
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SetTitle(text);
}

// -------------------- modified �v���p�e�B --------------------

/**
modified�擾
*/
- (id)asModified
{
	Boolean	b = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).NVI_IsDirty();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- filewidth �v���p�e�B --------------------

/**
filewidth�擾
*/
- (id)asFilewidth
{
	ANumber	fileWidth, infoWidth;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetColumnWidth(fileWidth,infoWidth);	
	return [NSAppleEventDescriptor descriptorWithInt32:fileWidth];
}

/**
filewidth�ݒ�
*/
- (void)setAsFilewidth:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	SInt32	width = [aeDesc int32Value];
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASSetFileColumnWidth(width);
}

// -------------------- infowidth �v���p�e�B --------------------

/**
infowidth�擾
*/
- (id)asInfowidth
{
	ANumber	fileWidth, infoWidth;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetColumnWidth(fileWidth,infoWidth);	
	return [NSAppleEventDescriptor descriptorWithInt32:infoWidth];
}

/**
infowidth�ݒ�
*/
- (void)setAsInfowidth:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	SInt32	width = [aeDesc int32Value];
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASSetInfoColumnWidth(width);
}

// -------------------- fileorder �v���p�e�B --------------------

/**
fileorder�擾
*/
- (id)asFileorder
{
	return [NSAppleEventDescriptor descriptorWithInt32:1];
}

// -------------------- infoorder �v���p�e�B --------------------

/**
infoorder�擾
*/
- (id)asInfoorder
{
	return [NSAppleEventDescriptor descriptorWithInt32:2];
}

// -------------------- asksaving �v���p�e�B --------------------

/**
asksaving�擾
*/
- (id)asAsksaving
{
	Boolean	b = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetAskSavingMode();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

/**
asksaving�ݒ�
*/
- (void)setAsAsksaving:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	Boolean	b = [aeDesc booleanValue];
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SetAskSavingMode(b);
}

// -------------------- windowindex �v���p�e�B --------------------

/**
windowindex�擾
*/
- (id)asWindowindex
{
	//AWindow���X�g�擾
	AArray<AWindowID>	winIDArray;
	GetApp().NVI_GetOrderedWindowIDArray(winIDArray,kWindowType_Invalid);
	//���̃h�L�������g�̃E�C���h�E��windowID���擾
	AWindowID	winID = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetWindowID();
	//index���擾
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
windowindex�ݒ�
*/
- (void)setAsWindowindex:(id)value
{
	/*
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	SInt32	number = [aeDesc int32Value];
	AWindowID	winID = GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_ASGetWindowID();
	CocoaWindow*	win = GetApp().NVI_GetWindowByID(winID).NVI_GetWindowImp().GetCocoaWindow();
	[win setWindowOrder:number];
	//�����ʃ��C���ŏ����ݒ肵�Ė��Ȃ����H
	*/
}

// -------------------- indexrecord�l�擾�E�ݒ� --------------------

/**
NSAppleEventDescriptor����indexrecord�̒l��ݒ�
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
indexrecord����NSAppleEventDescriptor���擾
*/
-(NSAppleEventDescriptor*)getDescFromItem:(AIndex)inGroupIndex itemIndex:(AIndex)inItemIndex
{
	//�w��group index, item index�̃f�[�^���擾
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
	
	//���ʊi�[�precord descriptor����
	NSAppleEventDescriptor *aeDesc = [NSAppleEventDescriptor recordDescriptor];
	//file�i�[
	if( file.Exist() == true )
	{
		AText	path;
		file.GetPath(path);
		NSURL*	url =  [NSURL fileURLWithPath:path.CreateNSString(true) isDirectory:NO];
		[aeDesc setDescriptor:[NSAppleEventDescriptor descriptorWithURL:url] forKeyword:pFileF];
	}
	//�e�L�X�g�͈͊i�[
	SInt32	start = spos;
	SInt32	end = spos+len;
	[aeDesc setDescriptor:[NSAppleEventDescriptor descriptorWithInt32:spos+1] forKeyword:pStartPositionF];
	[aeDesc setDescriptor:[NSAppleEventDescriptor descriptorWithInt32:spos+1+len] forKeyword:pEndPositionF];
	//�R�����g�i�[
	[aeDesc setDescriptor:[NSAppleEventDescriptor descriptorWithString:comment.CreateNSString(true)] forKeyword:pCommentF];
	return aeDesc;
}

#pragma mark ===========================

#pragma mark --- command handler

/**
close�R�}���h
*/
- (id)handleCloseScriptCommand:(MICloseCommand*)command
{
	//asksave�ϐ�������
	NSUInteger askSave = 'ask ';
	//�����擾
	NSDictionary *args = [command evaluatedArguments];
	if( args != nil )
	{
		//asksave�����擾
		NSNumber*	argSaving = [args valueForKey:@"saving"];
		if( argSaving != nil )
		{
			askSave = [argSaving intValue];
		}
	}
	//asksave�ɏ]���ĕ��鏈��
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
exists�R�}���h�n���h��
*/
- (id)handleExistsScriptCommand:(MIExistsCommand*)command
{
	return @(YES);
}

/**
count�R�}���h�n���h��
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
#pragma mark [�N���X]CASIndexGroup
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
		//�h�L�������g
		mIndexWindowDocument = indexWindowDocument;
		mDocumentID = [mIndexWindowDocument documentID];
		//group index�ݒ�
		mGroupIndex = groupIndex;
		//�qelement��lazy object�i�[array����
		mIndexRecordLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
	}
	return self;
}

/**
dealloc
*/
- (void)dealloc
{
	//�qelement��lazy object�i�[array���
	[mIndexRecordLazyObjectsArray release];
	//�p������
	[super dealloc];
}

/**
DocumentID�擾
*/
- (ADocumentID)documentID
{
	return mDocumentID;
}

/**
group index�擾
*/
- (ATextIndex)groupIndex;
{
	return mGroupIndex;
}

/**
IndexWindowDocument�擾
*/
- (IndexWindowDocument*)indexWindowDocument
{
	return mIndexWindowDocument;
}

#pragma mark ===========================

#pragma mark --- element

/**
specifier�Ŏw�肳�ꂽ�qelement��index�i�qelement array���ł�index�j���擾����
CASIndexWindowDoc::GetSubModelByPosition()�ɑΉ�
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifier�Ŏw��̏ꍇ ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//
		AIndex	index = [indexSpec index];
		// ---------- indexrecord ----------
		if( [[indexSpec key] isEqual:@"asIndexrecordElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfIndexrecords];
				}
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASIndexRecord*	obj = [[CASIndexRecord alloc] init:mIndexWindowDocument groupIndex:mGroupIndex itemIndex:index];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	lazyIndex = [mIndexRecordLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
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
		// ---------- ��L�ȊO�I�u�W�F�N�g�̏ꍇ�̓G���[ ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else
	{
		// ==================== ��L�ȊO�Ŏw��̏ꍇ�̓G���[ ====================
		// �v���p�e�B�擾�̏ꍇ�������ɗ���̂ŁA�G���[�ɂ����Anil��Ԃ�
		return nil;
	}
}

/**
indexrecord  element (lazy object array)�擾
*/
- (NSArray *)asIndexrecordElement
{
	return [mIndexRecordLazyObjectsArray array];
}

#pragma mark ===========================

#pragma mark --- property

/**
content�擾
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
content�ݒ�
*/
- (void)setAsContent:(id)value
{
	NSAppleEventDescriptor*	aeDescList = (NSAppleEventDescriptor*)value;
	//�O���[�v����indexrecord��S�폜
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_DeleteAllItemsInGroup(mGroupIndex);
	//�����̊e�v�f���Ƃ̃��[�v
	NSInteger count = [aeDescList numberOfItems];
	for( AIndex i = 0; i < count ;i++ )
	{
		//�v�f�擾
		NSAppleEventDescriptor*	aeDesc = [aeDescList descriptorAtIndex:i+1];
		//indexrecord�ǉ�
		GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).
				SPI_InsertItem_TextPosition(mGroupIndex,i,
											GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),
											GetEmptyText(),//#465
											0,0,0,GetEmptyText());
		//indexrecord�̒l�ݒ�
		[mIndexWindowDocument setItemFromDesc:mGroupIndex itemIndex:i ae:aeDesc];
	}
}

// -------------------- index �v���p�e�B --------------------

/**
index�擾
*/
- (id)asIndex
{
	AIndex	index = kIndex_Invalid;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(mGroupIndex,kIndex_Invalid,index);
	return [NSAppleEventDescriptor descriptorWithInt32:index+1];
}

// -------------------- comment �v���p�e�B --------------------

/**
comment�擾
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
comment�ݒ�
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
exists�R�}���h�n���h��
*/
- (id)handleExistsScriptCommand:(MIExistsCommand*)command
{
	return @(YES);
}

/**
delete�R�}���h�n���h��
*/
- (id)handleDeleteScriptCommand:(MIDeleteCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_DeleteGroup(mGroupIndex);
	return nil;
}

/**
select�R�}���h�n���h��
*/
- (id)handleSelectScriptCommand:(MIDeleteCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_SelectGroup(mGroupIndex);
	return nil;
}

/**
collapse�R�}���h�n���h��
*/
- (id)handleCollapseScriptCommand:(MICollapseCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_Collapse(mGroupIndex);
	return nil;
}

/**
expand�R�}���h�n���h��
*/
- (id)handleExpandScriptCommand:(MIExpandCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_Expand(mGroupIndex);
	return nil;
}

/**
count�R�}���h�n���h��
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
#pragma mark [�N���X]CASIndexRecord
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
		//�h�L�������g
		mIndexWindowDocument = indexWindowDocument;
		mDocumentID = [mIndexWindowDocument documentID];
		//group index, item index�ݒ�
		mGroupIndex = groupIndex;
		mItemIndex = itemIndex;
	}
	return self;
}

/**
DocumentID�擾
*/
- (ADocumentID)documentID
{
	return mDocumentID;
}

/**
group index�擾
*/
- (ATextIndex)groupIndex;
{
	return mGroupIndex;
}

/**
item index�擾
*/
- (ATextIndex)itemIndex;
{
	return mItemIndex;
}

/**
IndexWindowDocument�擾
*/
- (IndexWindowDocument*)indexWindowDocument
{
	return mIndexWindowDocument;
}

#pragma mark ===========================

#pragma mark --- property

/**
content�擾
*/
-(NSAppleEventDescriptor*)asContent
{
	return [mIndexWindowDocument getDescFromItem:mGroupIndex itemIndex:mItemIndex];
}

/**
content�ݒ�
*/
- (void)setAsContent:(id)value
{
	NSAppleEventDescriptor*	desc = (NSAppleEventDescriptor*)value;
	[mIndexWindowDocument setItemFromDesc:mGroupIndex itemIndex:mItemIndex ae:desc];
}

// -------------------- index �v���p�e�B --------------------

/**
index�擾
*/
- (id)asIndex
{
	AIndex	index = kIndex_Invalid;
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_GetASRecordIndex(mGroupIndex,mItemIndex,index);
	return [NSAppleEventDescriptor descriptorWithInt32:index+1];
}

// -------------------- file �v���p�e�B --------------------

/**
file�擾
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
file�ݒ�
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

// -------------------- startposition �v���p�e�B --------------------

/**
startposition�擾
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

// -------------------- endposition �v���p�e�B --------------------

/**
endposition�擾
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

// -------------------- paragraph �v���p�e�B --------------------

/**
paragraph�擾
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

// -------------------- comment �v���p�e�B --------------------

/**
comment�擾
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
comment�ݒ�
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
exists�R�}���h�n���h��
*/
- (id)handleExistsScriptCommand:(MIExistsCommand*)command
{
	return @(YES);
}

/**
delete�R�}���h�n���h��
*/
- (id)handleDeleteScriptCommand:(MIDeleteCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_DeleteItem(mGroupIndex,mItemIndex);
	return nil;
}

/**
select�R�}���h�n���h��
*/
- (id)handleSelectScriptCommand:(MIDeleteCommand*)command
{
	GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID).SPI_Select(mGroupIndex,mItemIndex);
	return nil;
}

@end



