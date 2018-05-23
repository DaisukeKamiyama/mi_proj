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
#pragma mark [�N���X]TextDocument
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
		//�����o�ϐ�������
		[self initMemberVariables];
	}
	return self;
}

/**
dealloc
*/
- (void)dealloc
{
	//LazyObjectsArray���
	[mParagraphLazyObjectsArray release];
	[mWordLazyObjectsArray release];
	[mCharacterLazyObjectsArray release];
	[mInsertionPointLazyObjectsArray release];
	[mSelectionObjectLazyObjectsArray release];
	//�p������
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
		//�����o�ϐ�������
		[self initMemberVariables];
		
		//==================�e�L�X�g�G���R�[�f�B���O�擾==================
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
		
		//==================�h�L�������g����==================
		AText	path;
		path.SetCstring([[absoluteURL path] UTF8String]);
		AFileAcc	file;
		file.Specify(path);
		//#1006 �����Œ��ڃh�L�������g�����͂����A�����C�x���g�փL���[�C���O���Ƃɂ��� ADocumentID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(path,tecname);
		//==================ODB==================
		//#1006
		//ODB�f�[�^�擾
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
				//#1006 �����Œ��ڃh�L�������g�����͂����A�����C�x���g�փL���[�C���O���Ƃɂ��� GetApp().SPI_GetTextDocumentByID(docID).SPI_SetODBMode(odbServer,odbSenderToken,odbCustomPath);
				odbMode = true;
			}
		}
		
		//�h�L�������g�������邽�߂̓����C�x���g���쐬
		//�t�@�C���p�X�擾
		AText	filepath;
		file.GetPath(filepath);
		//NULL������؂�Ńh�L�������g����text�Ɋi�[
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
		
		//CDocImp�I�u�W�F�N�g���� #1078
		AObjectID	docImpID = GetApp().GetImp().CreateDocImp();
		//NSDocument(self)�ݒ� #1078
		GetApp().GetImp().GetDocImpByID(docImpID).SetNSDocument(self);
		
		//�����C�x���g�փL���[�C���O
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(docImpID);//#1078
		ABase::PostToMainInternalEventQueue(kInternalEvent_AppleEventOpenDoc,kObjectID_Invalid,0,text,objectIDArray);
	}
	//fprintf(stderr,"TextDocument#initWithContentsOfURL - end\n");
	return self;
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
	mParagraphLazyObjectsArray          = [[LazyObjectsArray alloc] init:self];
	mWordLazyObjectsArray               = [[LazyObjectsArray alloc] init:self];
	mCharacterLazyObjectsArray          = [[LazyObjectsArray alloc] init:self];
	mInsertionPointLazyObjectsArray     = [[LazyObjectsArray alloc] init:self];
	mSelectionObjectLazyObjectsArray    = [[LazyObjectsArray alloc] init:self];
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
			key:@"asDocumentElement" 
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
CASTextDoc::GetSubModelByPosition()�ɑΉ�
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifier�Ŏw��̏ꍇ ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
        //NSIndexSpecifier�Ŏw�肳��Ă���index���擾
		AIndex	index = [indexSpec index];
		// ---------- paragraph ----------
		if( [[indexSpec key] isEqual:@"asParagraphElement"] == YES )
		{
			try
			{
                //index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfParagraphs];
				}
				//�w��index��paragraph�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetParagraph(0,index,startTextIndex,endTextIndex) == false )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASParagraph*	obj = [[CASParagraph alloc] init:self start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	lazyIndex = [mParagraphLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				//�w��paragraph�����݂��Ȃ��ꍇ�̓G���[
				[CocoaApp appleScriptNoSuchObjectError:@"The paragraph not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- word ----------
		else if( [[indexSpec key] isEqual:@"asWordElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfWords];
				}
				//�w��index��word�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetWord(0,index,startTextIndex,endTextIndex) == false )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASWord*	obj = [[CASWord alloc] init:self start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	lazyIndex = [mWordLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				//�w��word�����݂��Ȃ��ꍇ�̓G���[
				[CocoaApp appleScriptNoSuchObjectError:@"The word not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- character ----------
		else if( [[indexSpec key] isEqual:@"asCharacterElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfCharacters];
				}
				//�w��index��character�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetChar(0,index,startTextIndex,endTextIndex) == false )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASCharacter*	obj = [[CASCharacter alloc] init:self start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	lazyIndex = [mCharacterLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				//�w��character�����݂��Ȃ��ꍇ�̓G���[
				[CocoaApp appleScriptNoSuchObjectError:@"The character not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- insertion point ----------
		else if( [[indexSpec key] isEqual:@"asInsertionPointElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfInsertionPoints];
				}
				//�w��index��insertion point�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	textIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetInsertionPoint(0,index,textIndex) == false )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASInsertionPoint*	obj = [[CASInsertionPoint alloc] init:self textIndex:textIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	lazyIndex = [mInsertionPointLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				//�w��insertion point�����݂��Ȃ��ꍇ�̓G���[
				[CocoaApp appleScriptNoSuchObjectError:@"The insertion point not found."];
				return [NSMutableArray array];
			}
		}
		// ---------- selection object ----------
		else if( [[indexSpec key] isEqual:@"asSelectionObjectElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfSelectionObjects];
				}
				//�h�L�������g�ɑΉ�����E�C���h�E��WindowID�擾
				AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
				if( winID == kObjectID_Invalid )   throw 0;
				//���݂̑I��͈͂��擾�i��`�I�𒆂͕����ɂȂ�j
				AArray<ATextIndex>	start,end;
				GetApp().SPI_GetTextWindowByID(winID).SPI_GetSelect(mDocumentID,start,end);
				//index���͈͊O�̏ꍇ�̓G���[
				if( index >= start.GetItemCount() )   throw 0;
				//�Ԃ�lArray����
				NSMutableArray*	result = [NSMutableArray array];
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASSelectionObject*	obj = [[CASSelectionObject alloc] init:self start:start.Get(index) end:end.Get(index)];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	lazyIndex = [mSelectionObjectLazyObjectsArray registerLazyObject:obj];
				[result addObject:[NSNumber numberWithInteger:lazyIndex]];
				return result;
			}
			catch(...)
			{
				//�w��selection object�����݂��Ȃ��ꍇ�̓G���[
				[CocoaApp appleScriptNoSuchObjectError:@"The selection object not found."];
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
paragraph element (lazy object array)�擾
*/
- (NSArray *)asParagraphElement
{
	return [mParagraphLazyObjectsArray array];
}

/**
word element (lazy object array)�擾
*/
- (NSArray *)asWordElement
{
	return [mWordLazyObjectsArray array];
}

/**
character element (lazy object array)�擾
*/
- (NSArray *)asCharacterElement
{
	return [mCharacterLazyObjectsArray array];
}

/**
insertion point element (lazy object array)�擾
*/
- (NSArray *)asInsertionPointElement
{
	return [mInsertionPointLazyObjectsArray array];
}

/**
selection object element (lazy object array)�擾
*/
- (NSArray *)asSelectionObjectElement
{
	return [mSelectionObjectLazyObjectsArray array];
}

#pragma mark ===========================

#pragma mark --- property

// -------------------- content �v���p�e�B --------------------

/**
content�擾
*/
-(NSAppleEventDescriptor*)asContent
{
	AText	text;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetText(text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
content�ݒ�
*/
- (void)setAsContent:(id)value
{
	//document��read only���̓G���[
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	
	//Undo�A�N�V�����^�O�L�^
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
	//�폜
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(0,GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextLength());
	//text�}��
	AText	text;
	text.SetFromNSString([value stringValue]);
	text.ConvertLineEndToCR();
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(0,text);
}

// -------------------- name �v���p�e�B --------------------

/**
name�擾
*/
- (id)asName
{
	AText	text;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_GetTitle(text);
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
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetTemporaryTitle(text);
}

// -------------------- file �v���p�e�B --------------------

/**
file�擾
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

// -------------------- modified �v���p�e�B --------------------

/**
modified�擾
*/
- (id)asModified
{
	Boolean	b = GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsDirty();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- mode �v���p�e�B --------------------

/**
mode�擾
*/
- (id)asMode
{
	AText	text;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetModeDisplayName(text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
mode�ݒ�
*/
- (void)setAsMode:(id)mode
{
	AText	modetext;
	modetext.SetFromNSString([mode stringValue]);
	//
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetModeByDisplayName(modetext);
}

// -------------------- text encoding �v���p�e�B --------------------

/**
text encoding�擾
*/
- (id)asTextEncoding
{
	AText	text;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextEncoding(text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
text encoding�ݒ�
*/
- (void)setAsTextEncoding:(id)tec
{
	//document��read only���̓G���[
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//
	AText	tectext;
	tectext.SetFromNSString([tec stringValue]);
	//���p�\text encoding���łȂ��ꍇ�̓G���[
	if( ATextEncodingWrapper::CheckTextEncodingAvailability(tectext) == false )
	{
		[CocoaApp appleScriptNotHandledError];
		return;
	}
	//text encoding�ݒ�
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetTextEncoding(tectext);
}

// -------------------- return code �v���p�e�B --------------------

/**
return code�擾
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
return code�ݒ�
*/
- (void)setAsReturnCode:(id)returnCode
{
	//document��read only���̓G���[
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

// -------------------- type �v���p�e�B --------------------

/**
file type�擾
*/
- (id)asType
{
	OSType	type = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFileType();
	AText	text;
	text.SetFromOSType(type);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
file type�ݒ�
*/
- (void)setAsType:(id)value
{
	//document��read only���̓G���[
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

// -------------------- creator �v���p�e�B --------------------

/**
file creator�擾
*/
- (id)asCreator
{
	OSType	creator = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFileCreator();
	AText	text;
	text.SetFromOSType(creator);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
file creator�ݒ�
*/
- (void)setAsCreator:(id)value
{
	//document��read only���̓G���[
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
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
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
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	SInt32	number = [aeDesc int32Value];
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
	CocoaWindow*	win = GetApp().SPI_GetTextWindowByID(winID).NVI_GetWindowImp().GetCocoaWindow();
	[win setWindowOrder:number];
	//�����ʃ��C���ŏ����ݒ肵�Ė��Ȃ����H
}

// -------------------- font �v���p�e�B --------------------

/**
font�擾
*/
- (id)asFont
{
	AText	text;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFontName(text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
font�ݒ�
*/
- (void)setAsFont:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	AText	text;
	text.SetFromNSString([aeDesc stringValue]);
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFontName(text);
}

// -------------------- size �v���p�e�B --------------------

/**
font size�擾
*/
- (id)asSize
{
	SInt32	fontsize = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFontSize();
	return [NSAppleEventDescriptor descriptorWithInt32:fontsize];
}

/**
font size�ݒ�
*/
- (void)setAsSize:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	SInt32	fontsize = [aeDesc int32Value];
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFontSize(fontsize);
}

// -------------------- index �v���p�e�B --------------------

/**
index�iTextDocument�̃E�C���h�Ez-order���j�擾
*/
- (id)asIndex
{
	//index�擾
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
index�iTextDocument�̃E�C���h�Ez-order���j�ݒ�
*/
- (void)setAsIndex:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//�w��index�inumber)�擾
	SInt32	num = [aeDesc int32Value];
	//�Ώ�window�擾
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
	if( num <= 1 )
	{
		//index=1�ɐݒ肷��ꍇ�́A�őO�ʂɂ���
		GetApp().NVI_GetWindowByID(winID).NVI_SelectWindow();
		GetApp().NVI_GetWindowByID(winID).NVI_SelectTabByDocumentID(mDocumentID);
	}
	else
	{
		//index=1�ȊO�ɐݒ肷��ꍇ�́A���O�̏��Ԃ̃E�C���h�E�i�^�u�j�̌��ɔz�u����
		
		//ADocument���X�g�擾
		AArray<ADocumentID>	docIDArray;
		GetApp().NVI_GetOrderedDocumentIDArray(docIDArray,kDocumentType_Text,true);
		//���݂�index�Ɠ����Ȃ牽������return�A���݂�index������ɔz�u����Ȃ�w��index��+1����B
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
		//�ړ���z-order�̒��O�̃h�L�������g�̃E�C���h�E�E�^�u���擾
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
			//���O�̃h�L�������g�ƃE�C���h�E���Ⴄ�ꍇ�́A���̃E�C���h�E�̔w��Ɉړ�����
			GetApp().NVI_GetWindowByID(winID).NVI_SendBehind(prevWinID);
		}
		else
		{
			//���O�̃h�L�������g�ƃE�C���h�E�������ꍇ�́A���̃E�C���h�E���̒��O�̃^�u�̔w��Ɉړ�����
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

// -------------------- selection �v���p�e�B --------------------

/**
selection�擾
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
selection�ݒ�
*/
-(void)setAsSelection:(id)value
{
	//document��read only���̓G���[
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//������擾
	AText	text;
	text.SetFromNSString([value stringValue]);
	text.ConvertLineEndToCR();
	//�E�C���h�E�擾
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
	if( winID == kObjectID_Invalid )
	{
		[CocoaApp appleScriptNotHandledError];
		return;
	}
	//�I��͈͎擾
	AArray<ATextIndex>	start, end;
	GetApp().SPI_GetTextWindowByID(winID).SPI_GetSelect(mDocumentID,start,end);
	if( start.GetItemCount() == 1 )
	{
		//Undo�A�N�V�����^�O�L�^
		GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
		//�I��͈͍폜
		GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(start.Get(0),end.Get(0));
		//�}��
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
save�R�}���h
*/
- (id)handleSaveScriptCommand:(MISaveCommand*)command
{
	//document��read only���̓G���[
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//
	NSDictionary *args = [command evaluatedArguments];
	if( args != nil )
	{
		//����in�̏����i�t�@�C���ۑ���ݒ�j
		[self handleSaveAndCloseCommand_InArg:args];
		//����as�̏���
		NSNumber*	argAs = [args valueForKey:@"as"];
		if( argAs != nil )
		{
			//file type�ݒ�
			OSType	fileType = [argAs intValue];
			GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_SetFileType(fileType);
		}
	}
	//�ۑ����s
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_Save(false);
    return nil;
}

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
		//����in�̏����i�t�@�C���ۑ���ݒ�j
		[self handleSaveAndCloseCommand_InArg:args];
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
����in�̏����i�t�@�C���ۑ���ݒ�j
*/
- (void)handleSaveAndCloseCommand_InArg:(NSDictionary*)args
{
	NSURL*	argIn = [args valueForKey:@"in"];
	if( argIn != nil )
	{
		AText	path;
		path.SetCstring([[argIn path] UTF8String]);
		//file "�`"�Ǝw�肵������URL��file:///:Users:daisuke:...�ȂǂƓ����Ă���AUTF8String�ɂ��/:Users:daisuke...�ȂǂƂȂ�̂ŁA�ŏ���/���폜���A:��/�֕ϊ�����B
		if( path.CompareMin("/:") == true )
		{
			path.Delete1(0);
			path.ReplaceChar(':','/');
		}
		//�t�@�C���ݒ�
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
	//�h�L�������g�ɑΉ�����E�C���h�E��WindowID�擾
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
	//���݂̑I��͈͂��擾�i��`�I�𒆂͕����ɂȂ�j
	AArray<ATextIndex>	start,end;
	GetApp().SPI_GetTextWindowByID(winID).SPI_GetSelect(mDocumentID,start,end);
	return start.GetItemCount();
}

/**
select�R�}���h�n���h��
*/
- (id)handleSelectScriptCommand:(MIDeleteCommand*)command
{
	GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_RevealDocumentWithAView();
    return nil;
}

@end

#pragma mark ===========================
#pragma mark [�N���X]CASTextDocElement
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
		//�h�L�������g
		mTextDocument = textDocument;
		mDocumentID = [textDocument documentID];
		//�e�L�X�g�J�n�ʒu�E�I���ʒu
		mStartTextIndex = start;
		mEndTextIndex = end;
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
�e�L�X�g�J�n�ʒu�擾
*/
- (ATextIndex)startTextIndex;
{
	return mStartTextIndex;
}

/**
�e�L�X�g�I���ʒu�擾
*/
- (ATextIndex)endTextIndex
{
	return mEndTextIndex;
}

/**
TextDocument�擾
*/
- (TextDocument*)textDocument
{
	return mTextDocument;
}

#pragma mark ===========================

#pragma mark --- property

/**
content�擾
*/
-(NSAppleEventDescriptor*)asContent
{
	AText	text;
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetText(mStartTextIndex,mEndTextIndex,text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
content�ݒ�
*/
- (void)setAsContent:(id)value
{
	//document��read only���̓G���[
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//Undo�A�N�V�����^�O�L�^
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
	//�폜
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(mStartTextIndex,mEndTextIndex);
	//text�}��
	AText	text;
	text.SetFromNSString([value stringValue]);
	text.ConvertLineEndToCR();
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_InsertText(mStartTextIndex,text);
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
	//document��read only���̓G���[
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError_DocumentReadOnly];
	}
	//Undo�A�N�V�����^�O�L�^
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
	//�폜
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(mStartTextIndex,mEndTextIndex);
	return nil;
}

/**
select�R�}���h�n���h��
*/
- (id)handleSelectScriptCommand:(MIDeleteCommand*)command
{
	//�J�n�ʒu�A�I���ʒu�擾
	ATextIndex	spos = mStartTextIndex;
	ATextIndex	epos = mEndTextIndex;
	//
	NSDictionary *args = [command evaluatedArguments];
	if( args != nil )
	{
		//to����������Ƃ��́A���̃I�u�W�F�N�g�܂ł�I��͈͂ɂ���
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
	//�E�C���h�E�擾
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetFirstWindowID();
	if( winID == kObjectID_Invalid )
	{
		[CocoaApp appleScriptNotHandledError];
	}
	else
	{
		//�I��
		GetApp().SPI_GetTextWindowByID(winID).SPI_SetSelect(mDocumentID,spos,epos);
	}
    return nil;
}

@end

#pragma mark ===========================
#pragma mark [�N���X]CASParagraph
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
		//�qelement��lazy object�i�[array����
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
	//�qelement��lazy object�i�[array���
	[mWordLazyObjectsArray release];
	[mCharacterLazyObjectsArray release];
	[mInsertionPointLazyObjectsArray release];
	//�p������
	[super dealloc];
}

#pragma mark ===========================

#pragma mark --- object specifier

/**
objectSpecifier�擾
*/
- (NSScriptObjectSpecifier *)objectSpecifier 
{
	//paragraph index�擾
	ATextPoint	textpoint = {0};
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetTextPointFromTextIndex(mStartTextIndex,textpoint,true);
	AIndex	paraIndex = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_GetParagraphIndexByLineIndex(textpoint.y);
	//NSIndexSpecifier�icontainer:document, index:�i��index�j��Ԃ�
	return [[[NSIndexSpecifier alloc] initWithContainerClassDescription:[[mTextDocument objectSpecifier] keyClassDescription] 
			containerSpecifier:[mTextDocument objectSpecifier] 
			key:@"asParagraphElement" 
	index:paraIndex] autorelease];
}

#pragma mark ===========================

#pragma mark --- element

/**
specifier�Ŏw�肳�ꂽ�qelement��index�i�qelement array���ł�index�j���擾����
CASParagraph::GetSubModelByPosition()�ɑΉ�
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifier�Ŏw��̏ꍇ ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//
		AIndex	index = [indexSpec index];
		// ---------- word ----------
		if( [[indexSpec key] isEqual:@"asWordElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfWords];
				}
				//�w��index��word�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetWord(mStartTextIndex,index,startTextIndex,endTextIndex) == false )   throw 0;
				if( startTextIndex >= mEndTextIndex )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASWord*	obj = [[CASWord alloc] init:mTextDocument start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	index = [mWordLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
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
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfCharacters];
				}
				//�w��index��character�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetChar(mStartTextIndex,index,startTextIndex,endTextIndex) == false )   throw 0;
				if( startTextIndex >= mEndTextIndex )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASCharacter*	obj = [[CASCharacter alloc] init:mTextDocument start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	index = [mCharacterLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
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
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfInsertionPoints];
				}
				//�w��index��insertion point�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	textIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetInsertionPoint(mStartTextIndex,index,textIndex) == false )   throw 0;
				if( textIndex > mEndTextIndex )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASInsertionPoint*	obj = [[CASInsertionPoint alloc] init:mTextDocument textIndex:textIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	index = [mInsertionPointLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
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
		// ---------- ��L�ȊO�I�u�W�F�N�g�̏ꍇ�̓G���[ ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else
	{
		// ==================== ��L�ȊO�Ŏw��̏ꍇ��nil��Ԃ� ====================
		// �v���p�e�B�擾�̏ꍇ�������ɗ���̂ŁA�G���[�ɂ����Anil��Ԃ�
		return nil;
	}
}

/**
word element (lazy object array)�擾
*/
- (NSArray *)asWordElement
{
	return [mWordLazyObjectsArray array];
}

/**
character element (lazy object array)�擾
*/
- (NSArray *)asCharacterElement
{
	return [mCharacterLazyObjectsArray array];
}

/**
insertion point element (lazy object array)�擾
*/
- (NSArray *)asInsertionPointElement
{
	return [mInsertionPointLazyObjectsArray array];
}

#pragma mark ===========================

#pragma mark --- property

// -------------------- index �v���p�e�B --------------------

/**
index�i�h�L�������g�̍ŏ�����̒i���ԍ��j�擾
*/
- (id)asIndex
{
	//paragraph index�擾
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
#pragma mark [�N���X]CASWord
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
		//�qelement��lazy object�i�[array����
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
	//�qelement��lazy object�i�[array���
	[mCharacterLazyObjectsArray release];
	[mInsertionPointLazyObjectsArray release];
	//�p������
	[super dealloc];
}

#pragma mark ===========================

#pragma mark --- object specifier

/**
objectSpecifier�擾
*/
- (NSScriptObjectSpecifier *)objectSpecifier 
{
	//word index�擾
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
specifier�Ŏw�肳�ꂽ�qelement��index�i�qelement array���ł�index�j���擾����
CASWord::GetSubModelByPosition()�ɑΉ�
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifier�Ŏw��̏ꍇ ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//
		AIndex	index = [indexSpec index];
		// ---------- character ----------
		if( [[indexSpec key] isEqual:@"asCharacterElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfCharacters];
				}
				//�w��index��character�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetChar(mStartTextIndex,index,startTextIndex,endTextIndex) == false )   throw 0;
				if( startTextIndex >= mEndTextIndex )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASCharacter*	obj = [[CASCharacter alloc] init:mTextDocument start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	index = [mCharacterLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
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
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfInsertionPoints];
				}
				//�w��index��insertion point�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	textIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetInsertionPoint(mStartTextIndex,index,textIndex) == false )   throw 0;
				if( textIndex > mEndTextIndex )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASInsertionPoint*	obj = [[CASInsertionPoint alloc] init:mTextDocument textIndex:textIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	index = [mInsertionPointLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
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
		// ---------- ��L�ȊO�I�u�W�F�N�g�̏ꍇ�̓G���[ ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else
	{
		// ==================== ��L�ȊO�Ŏw��̏ꍇ��nil��Ԃ� ====================
		// �v���p�e�B�擾�̏ꍇ�������ɗ���̂ŁA�G���[�ɂ����Anil��Ԃ�
		return nil;
	}
}

/**
character element (lazy object array)�擾
*/
- (NSArray *)asCharacterElement
{
	return [mCharacterLazyObjectsArray array];
}

/**
insertion point element (lazy object array)�擾
*/
- (NSArray *)asInsertionPointElement
{
	return [mInsertionPointLazyObjectsArray array];
}


#pragma mark ===========================

#pragma mark --- property

// -------------------- index �v���p�e�B --------------------

/**
index�i�h�L�������g�̍ŏ�����̒P��ԍ��j�擾
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
delete�R�}���h�n���h��
*/
- (id)handleDeleteScriptCommand:(MIDeleteCommand*)command
{
	//read only�̏ꍇ�̓G���[
	if( GetApp().SPI_GetTextDocumentByID(mDocumentID).NVI_IsReadOnly() == true )
	{
		[CocoaApp appleScriptNotHandledError];
	}
	
	//Undo�A�N�V�����^�O�L�^
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_RecordUndoActionTag(undoTag_AppleEvent);
	//�폜
	GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_DeleteText(mStartTextIndex,mEndTextIndex);
	//�O��X�y�[�X����
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
#pragma mark [�N���X]CASCharacter
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
		//�qelement��lazy object�i�[array����
		mInsertionPointLazyObjectsArray = [[LazyObjectsArray alloc] init:self];
	}
	return self;
}

/**
dealloc
*/
- (void)dealloc
{
	//�qelement��lazy object�i�[array���
	[mInsertionPointLazyObjectsArray release];
	//�p������
	[super dealloc];
}

#pragma mark ===========================

#pragma mark --- object specifier

/**
objectSpecifier�擾
*/
- (NSScriptObjectSpecifier *)objectSpecifier 
{
	//character index�擾
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
specifier�Ŏw�肳�ꂽ�qelement��index�i�qelement array���ł�index�j���擾����
CASCharacter::GetSubModelByPosition()�ɑΉ�
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifier�Ŏw��̏ꍇ ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//
		AIndex	index = [indexSpec index];
		// ---------- insertion point ----------
		if( [[indexSpec key] isEqual:@"asInsertionPointElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfInsertionPoints];
				}
				//�w��index��insertion point�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	textIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetInsertionPoint(mStartTextIndex,index,textIndex) == false )   throw 0;
				if( textIndex > mEndTextIndex )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASInsertionPoint*	obj = [[CASInsertionPoint alloc] init:mTextDocument textIndex:textIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	index = [mInsertionPointLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
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
		// ---------- ��L�ȊO�I�u�W�F�N�g�̏ꍇ�̓G���[ ----------
		else
		{
			[CocoaApp appleScriptNotHandledError];
			return nil;
		}
	}
	else
	{
		// ==================== ��L�ȊO�Ŏw��̏ꍇ��nil��Ԃ� ====================
		// �v���p�e�B�擾�̏ꍇ�������ɗ���̂ŁA�G���[�ɂ����Anil��Ԃ�
		return nil;
	}
}

/**
insertion point element (lazy object array)�擾
*/
- (NSArray *)asInsertionPointElement
{
	return [mInsertionPointLazyObjectsArray array];
}

#pragma mark ===========================

#pragma mark --- property

// -------------------- index �v���p�e�B --------------------

/**
index�i�h�L�������g�̍ŏ������character�ԍ��j�擾
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
#pragma mark [�N���X]CASInsertionPoint
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
objectSpecifier�擾
*/
- (NSScriptObjectSpecifier *)objectSpecifier 
{
	//character index�擾
	AIndex	charIndex = GetApp().SPI_GetTextDocumentByID(mDocumentID).SPI_ASGetCharCount(0,mStartTextIndex);
	//
	return [[[NSIndexSpecifier alloc] initWithContainerClassDescription:[[mTextDocument objectSpecifier] keyClassDescription] 
			containerSpecifier:[mTextDocument objectSpecifier] 
			key:@"asInsertionPointElement" 
	index:charIndex] autorelease];
}

#pragma mark ===========================

#pragma mark --- property

// -------------------- index �v���p�e�B --------------------

/**
index�i�h�L�������g�̍ŏ������insertion point�ԍ��j�擾
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
#pragma mark [�N���X]CASSelectionObject
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
		//�qelement��lazy object�i�[array����
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
	//�qelement��lazy object�i�[array���
	[mParagraphLazyObjectsArray release];
	[mWordLazyObjectsArray release];
	[mCharacterLazyObjectsArray release];
	[mInsertionPointLazyObjectsArray release];
	//�p������
	[super dealloc];
}

#pragma mark ===========================

#pragma mark --- object specifier

/**
objectSpecifier�擾
*/
- (NSScriptObjectSpecifier *)objectSpecifier 
{
	//selection object��index�͂Ƃ肠����0�Œ�ŕԂ��B
	//
	return [[[NSIndexSpecifier alloc] initWithContainerClassDescription:[[mTextDocument objectSpecifier] keyClassDescription] 
			containerSpecifier:[mTextDocument objectSpecifier] 
			key:@"asSelectionObjectElement" 
	index:0] autorelease];
}

#pragma mark ===========================

#pragma mark --- element

/**
specifier�Ŏw�肳�ꂽ�qelement��index�i�qelement array���ł�index�j���擾����
CASSelectionObject::GetSubModelByPosition()�ɑΉ�
*/
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(NSScriptObjectSpecifier *)specifier
{
	if( [specifier isKindOfClass:[NSIndexSpecifier class]] == YES )
	{
		// ==================== NSIndexSpecifier�Ŏw��̏ꍇ ====================
		
		NSIndexSpecifier*	indexSpec = (NSIndexSpecifier*)specifier;
		//
		AIndex	index = [indexSpec index];
		// ---------- paragraph ----------
		if( [[indexSpec key] isEqual:@"asParagraphElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfParagraphs];
				}
				//�w��index��paragraph�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetParagraph(mStartTextIndex,index,startTextIndex,endTextIndex) == false )   throw 0;
				if( startTextIndex > mEndTextIndex )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASParagraph*	obj = [[CASParagraph alloc] init:mTextDocument start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	index = [mParagraphLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				//�w��paragraph�����݂��Ȃ��ꍇ�̓G���[
				[CocoaApp appleScriptNoSuchObjectError:@"The paragraph not found."];
				return nil;
			}
		}
		// ---------- word ----------
		else if( [[indexSpec key] isEqual:@"asWordElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfWords];
				}
				//�w��index��word�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetWord(mStartTextIndex,index,startTextIndex,endTextIndex) == false )   throw 0;
				if( startTextIndex > mEndTextIndex )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASWord*	obj = [[CASWord alloc] init:mTextDocument start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	index = [mWordLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				//�w��word�����݂��Ȃ��ꍇ�̓G���[
				[CocoaApp appleScriptNoSuchObjectError:@"The word not found."];
				return nil;
			}
		}
		// ---------- character ----------
		else if( [[indexSpec key] isEqual:@"asCharacterElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfCharacters];
				}
				//�w��index��character�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	startTextIndex = 0, endTextIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetChar(mStartTextIndex,index,startTextIndex,endTextIndex) == false )   throw 0;
				if( startTextIndex > mEndTextIndex )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASCharacter*	obj = [[CASCharacter alloc] init:mTextDocument start:startTextIndex end:endTextIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	index = [mCharacterLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				//�w��character�����݂��Ȃ��ꍇ�̓G���[
				[CocoaApp appleScriptNoSuchObjectError:@"The character not found."];
				return nil;
			}
		}
		// ---------- insertion point ----------
		else if( [[indexSpec key] isEqual:@"asInsertionPointElement"] == YES )
		{
			try
			{
				//index���}�C�i�X�̏ꍇ�͍Ōォ�琔����i�}�C�i�X��"last"���Ŏw�肳�ꂽ�ꍇ�j
				if( index < 0 )
				{
					index += [self countOfInsertionPoints];
				}
				//�w��index��insertion point�̊J�n�ʒu�E�I���ʒu�擾
				ATextIndex	textIndex = 0;
				if( GetApp().SPI_GetTextDocumentByID(mDocumentID).
					SPI_ASGetInsertionPoint(mStartTextIndex,index,textIndex) == false )   throw 0;
				if( textIndex > mEndTextIndex )   throw 0;
				//Lazy�I�u�W�F�N�g����(autorelease)
				CASInsertionPoint*	obj = [[CASInsertionPoint alloc] init:mTextDocument textIndex:textIndex];
				[obj autorelease];
				//Lazy�I�u�W�F�N�g�o�^�Aindex�擾
				NSInteger	index = [mInsertionPointLazyObjectsArray registerLazyObject:obj];
				//�Ԃ�lArray����
				NSMutableArray*	result = [NSMutableArray array];
				[result addObject:[NSNumber numberWithInteger:index]];
				return result;
			}
			catch(...)
			{
				//�w��insertion point�����݂��Ȃ��ꍇ�̓G���[
				[CocoaApp appleScriptNoSuchObjectError:@"The insertion point not found."];
				return nil;
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
		// ==================== ��L�ȊO�Ŏw��̏ꍇ��nil��Ԃ� ====================
		// �v���p�e�B�擾�̏ꍇ�������ɗ���̂ŁA�G���[�ɂ����Anil��Ԃ�
		return nil;
	}
}

/**
paragraph element (lazy object array)�擾
*/
- (NSArray *)asParagraphElement
{
	return [mParagraphLazyObjectsArray array];
}

/**
word element (lazy object array)�擾
*/
- (NSArray *)asWordElement
{
	return [mWordLazyObjectsArray array];
}

/**
character element (lazy object array)�擾
*/
- (NSArray *)asCharacterElement
{
	return [mCharacterLazyObjectsArray array];
}

/**
insertion point element (lazy object array)�擾
*/
- (NSArray *)asInsertionPointElement
{
	return [mInsertionPointLazyObjectsArray array];
}


#pragma mark ===========================

#pragma mark --- command handler

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

