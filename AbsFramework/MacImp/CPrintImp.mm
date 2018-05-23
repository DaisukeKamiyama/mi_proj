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

CPrintImp

*/

#include "CPrintImp.h"
#include "CWindowImp.h"
#include "CTextDrawData.h"
#include "../AbsFrame/AApplication.h"
#include "../AbsFrame/ADocument.h"
#include "CWindowImpCocoaObjects.h"
#include "CocoaMacro.h"

#pragma mark ===========================
#pragma mark [�N���X]CPrintImp
#pragma mark ===========================

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^

/**
�R���X�g���N�^
*/
CPrintImp::CPrintImp( const ADocumentID inDocumentID ) : AObjectArrayItem(NULL), /*#1034 mPageFormat(NULL),
		mPrintSettings(NULL),*/ mDocumentID(inDocumentID), mContextRef(NULL), /*#1034 mUseSheet(true),*/ mXResolution(72.0), mYResolution(72.0)
,CocoaObjects(NULL)//#1034
,mPrintView(nil),mVerticalMode(false),mVerticalCTM(false)//#558
{
	//Cocoa�I�u�W�F�N�g���� #1034
	CocoaObjects = new CUserPaneCocoaObjects();
	
	//#1034
	mTextForMetricsCalculation.AddFromUCS4Char(0x0079);//'y'
	mTextForMetricsCalculation.AddFromUCS4Char(0xFFE3);//'�P'
	//mTextForMetricsCalculation.AddFromUCS4Char(0xFF3F);//'�Q'
	mTextForMetricsCalculation.ConvertFromUTF8ToUTF16();
	
	//#1034 OSStatus	err = noErr;
	
	//B0335
	/*long	response;
	::Gestalt(gestaltSystemVersion,&response);
	if( response >= 0x1050 )*/
	/*#1034
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		mUseSheet = false;
	}
	mPageFormatDoneProc = NewPMSheetDoneUPP(&PageSetupDoneProc);
	mPrintDoneProc = NewPMSheetDoneUPP(&PrintDoneProc);
	*/
	/*#1034
	err = ::ATSUCreateStyle(&mATSUTextStyle);
	if( err != noErr )   _ACErrorNum("ATSUCreateStyle() returned error: ",err,this);
	*/
}

/**
�f�X�g���N�^
*/
CPrintImp::~CPrintImp()
{
	//#1034 OSStatus	err = noErr;
	
	//Cocoa�I�u�W�F�N�g�폜 #1034
	delete CocoaObjects;
	CocoaObjects = NULL;
	
	/*#1034
	err = ::ATSUDisposeStyle(mATSUTextStyle);
	if( err != noErr )   _ACErrorNum("ATSUDisposeStyle() returned error: ",err,this);
	*/
	/*#1034
	DisposePMSheetDoneUPP(mPageFormatDoneProc);
	DisposePMSheetDoneUPP(mPrintDoneProc);
     */
}

#pragma mark ===========================

#pragma mark --- �y�[�W�ݒ�

/**
�y�[�W�ݒ�
*/
void	CPrintImp::PageSetup( /*#182 CWindowImp& inWindowImp*/ const AWindowRef inWindowRef )
{
	//#1034
	
	//�}�E�X�J�[�\���ݒ�
	ACursorWrapper::SetCursor(kCursorType_Arrow);
	
	//�y�[�W�ݒ�_�C�A���O
	[[NSApplication sharedApplication] runPageLayout:nil];
#if 0
	OSStatus	err = noErr;
	
	PMPrintSession	printSession = NULL;
	try
	{
		OSStatus	status = noErr;
		//PrintSession����
		status = ::PMCreateSession(&printSession);
		if( status != noErr )   throw 0;
		//PageFormat
		SetupPageFormat();//#1033
		if( mPageFormat == NULL )   throw 0;
		//#1033
		//PageFormat�L����
		status = ::PMSessionValidatePageFormat(printSession, mPageFormat, kPMDontWantBoolean);
		if( status != noErr )   throw 0;
		/*#1033
		if( mUseSheet == true )//B0335
		{
			status = ::PMSessionUseSheets(printSession,inWindowRef,mPageFormatDoneProc);
			if( status != noErr )   throw 0;
#if SUPPORT_CARBON
			CPrintImp*	ptr = this;
			err = ::SetWindowProperty(inWindowRef,'MMKE','Prnt',sizeof(CPrintImp*),&ptr);
			if( err != noErr )   _ACErrorNum("SetWindowProperty() returned error: ",err,this);
#endif
		}
		*/
		//#1034 ::SetThemeCursor(kThemeArrowCursor);
		ACursorWrapper::SetCursor(kCursorType_Arrow);//#1034
		Boolean	accepted;
		::PMSessionPageSetupDialog(printSession,mPageFormat,&accepted);
		//B0335
		//#1033 if( mUseSheet == false )
		{
			err = ::PMRelease(printSession);
			if( err != noErr )   _ACErrorNum("PMRelease() returned error: ",err,this);
		}
	}
	catch(...)
	{
		if( printSession != NULL )
		{
			err = ::PMRelease(printSession);
			if( err != noErr )   _ACErrorNum("PMRelease() returned error: ",err,this);
		}
	}
#endif
}

//#1034
#if 0
/**
�y�[�W�ݒ�f�[�^Setup
*/
void	CPrintImp::SetupPageFormat()//#1033
{
	/*#1033 Quartz2DBasics�T���v���̂Ƃ���ɍ�蒼��
	OSStatus	err = noErr;
	
	OSStatus	status;
	if( mPageFormat == NULL )
	{
		status = ::PMCreatePageFormat(&mPageFormat);
		status = ::PMSessionDefaultPageFormat(printSession,mPageFormat);
		if( status != noErr )
		{
			err = ::PMRelease(mPageFormat);
			if( err != noErr )   _ACErrorNum("PMRelease() returned error: ",err,this);
			mPageFormat = NULL;
		}
	}
	else
	{
		status = ::PMSessionValidatePageFormat(printSession,mPageFormat,kPMDontWantBoolean);
		if( status != noErr )
		{
			err = ::PMRelease(mPageFormat);
			if( err != noErr )   _ACErrorNum("PMRelease() returned error: ",err,this);
			mPageFormat = NULL;
		}
	}
	*/
	if( mPageFormat != NULL )   return;
	
	OSStatus status = noErr;
	//PageFormat�쐬�p�Z�b�V�����쐬
	PMPrintSession printSession = NULL;
	status = ::PMCreateSession(&printSession);
	if( status != noErr )
	{
		_ACErrorNum("PMCreateSession() returned error: ",status,this);
		throw 0;
	}
	//PageFormat����
	status = ::PMCreatePageFormat(&mPageFormat);
	if( status != noErr )
	{
		_ACErrorNum("PMCreatePageFormat() returned error: ",status,this);
		throw 0;
	}
	//PageFormat��default�ݒ�
	status = ::PMSessionDefaultPageFormat(printSession, mPageFormat);
	if( status != noErr )
	{
		::PMRelease(mPageFormat);
		mPageFormat = NULL;
		_ACErrorNum("PMSessionDefaultPageFormat() returned error: ",status,this);
		throw 0;
	}
	//PageFormat�쐬�p�Z�b�V�������
	status = ::PMRelease(printSession);
	if( status != noErr )
	{
		_ACErrorNum("PMRelease() returned error: ",status,this);
		throw 0;
	}
}
#endif

#pragma mark ===========================

#pragma mark --- �v�����g

/**
���
*/
void	CPrintImp::Print( /*#182 CWindowImp& inWindowImp*/const AWindowRef inWindowRef, const AText& inJobTitle )
{
	//PrintInfo�擾
	NSPrintInfo *printInfo = [NSPrintInfo sharedPrintInfo];
	
	//PrintInfo�]���ݒ�
	//�]����0�ɐݒ肵�Aprint view�͗p����t�ɍL����B
	//view���̕\���͈͂�EVT_StartPrintMode()�Őݒ肵�A�A�v�����ŗ]�����䂷��B
	//�iOS���̂����̗]������̎d�l���s���ȓ�������Ă��邽�߁B�j
	[printInfo setLeftMargin:0];
	[printInfo setRightMargin:0];
	[printInfo setTopMargin:0];
	[printInfo setBottomMargin:0];
	
	//�]���ݒ�l�擾
	AFloatNumber	leftMarginMM	= 10.0;
	AFloatNumber	rightMarginMM	= 10.0;
	AFloatNumber	topMarginMM		= 10.0;
	AFloatNumber	bottomMarginMM	= 10.0;
	AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).NVI_GetPrintMargin(leftMarginMM,rightMarginMM,topMarginMM,bottomMarginMM);
	AFloatNumber	leftMargin		= GetXInterfaceSizeByMM(leftMarginMM);
	AFloatNumber	rightMargin		= GetXInterfaceSizeByMM(rightMarginMM);
	AFloatNumber	topMargin		= GetYInterfaceSizeByMM(topMarginMM);
	AFloatNumber	bottomMargin	= GetYInterfaceSizeByMM(bottomMarginMM);
	
	//�p���T�C�Y�擾
	NSSize paperSize = [printInfo paperSize];
	
	//�]���ݒ蔽�f�����y�[�W�T�C�Y�擾
	CGFloat	pageWidth = paperSize.width - leftMargin - rightMargin;
	CGFloat	pageHeight = paperSize.height - topMargin - bottomMargin;
	
	//����͈�rect�擾
	ALocalRect	pageRect = {0};
	pageRect.left		= leftMargin;
	pageRect.top		= topMargin;
	pageRect.right		= pageRect.left + pageWidth;
	pageRect.bottom		= pageRect.top + pageHeight;
	
	//������[�h�J�n
	AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_StartPrintMode(pageRect,paperSize.width,paperSize.height);//#558
	
	//�y�[�W���擾
	AItemCount	totalPageCount = AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_GetPrintPageCount();
	
	//print view����
	NSRect frameRect = NSMakeRect(0,0,paperSize.width,paperSize.height);
	mPrintView = [[CocoaPrintView alloc] initWithFrame:frameRect printImp:this totalPageCount:totalPageCount];//#558
	
	//�}�E�X�J�[�\���ݒ�
	ACursorWrapper::SetCursor(kCursorType_Arrow);
	
	//������s
	NSPrintOperation *prop = [NSPrintOperation printOperationWithView:mPrintView printInfo:printInfo];//#558
	{
		//�W���u�� #1104
		AStCreateNSStringFromAText	str(inJobTitle);
		[prop setJobTitle:str.GetNSString()];
	}
	[prop setShowsPrintPanel: YES];
	[prop setShowsProgressPanel: YES];
	[prop runOperation];
	
	//print view���
	[mPrintView release];//#558
	mPrintView = nil;//#558
	
	//������[�h�I��
	AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_EndPrintMode();
	
#if 0
	OSStatus	err = noErr;
	
	PMPrintSession	printSession = NULL;
	ABool	documentPrintModeStarted = false;
	try
	{
		OSStatus	status = noErr;
		//PrintSession����
		status = ::PMCreateSession(&printSession);
		if( status != noErr )   throw 0;
		//PageFormat
		SetupPageFormat();//#1033
		if( mPageFormat == NULL )   throw 0;
		//#1033
		//PageFormat�L����
		status = ::PMSessionValidatePageFormat(printSession, mPageFormat, kPMDontWantBoolean);
		if( status != noErr )   throw 0;
		//
		status = ::PMCreatePrintSettings(&mPrintSettings);
		if( status != noErr )   throw 0;
		status = ::PMSessionDefaultPrintSettings(printSession,mPrintSettings);
		if( status != noErr)   throw 0;
		//#1033
		Boolean	result = false;
		status = ::PMSessionValidatePrintSettings(printSession,mPrintSettings,&result);
		if( status != noErr)   throw 0;
		//�W���u��
		{
			AStCreateCFStringFromAText	strref(inJobTitle);
			//#1034 ::PMSetJobNameCFString(mPrintSettings,strref.GetRef());
			::PMPrintSettingsSetJobName(mPrintSettings,strref.GetRef());
		}
		//
		/*PMResolution	res;
		res.hRes = mXResolution;
		res.vRes = mYResolution;
		::PMSetResolution(mPageFormat,&res);*/
		//�y�[�W�ԍ��͈�
		PMRect	pageRect;
		status = ::PMGetAdjustedPageRect(mPageFormat,&pageRect);
		ALocalRect	rect;
		rect.left		= pageRect.left;
		rect.top		= pageRect.top;
		rect.right		= pageRect.right;
		rect.bottom		= pageRect.bottom;
		//win 080802
		PMRect	paperRect;
		::PMGetAdjustedPaperRect(mPageFormat,&paperRect);//DPI�ϊ������{����(Adjusted)�̂��́A���S�̂̃T�C�Y
		AFloatNumber	leftMarginMM	= 10.0;
		AFloatNumber	rightMarginMM	= 10.0;
		AFloatNumber	topMarginMM		= 10.0;
		AFloatNumber	bottomMarginMM	= 10.0;
		AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).NVI_GetPrintMargin(leftMarginMM,rightMarginMM,topMarginMM,bottomMarginMM);//R0242
		AFloatNumber	leftMargin		= GetXInterfaceSizeByMM(leftMarginMM);
		AFloatNumber	rightMargin		= GetXInterfaceSizeByMM(rightMarginMM);
		AFloatNumber	topMargin		= GetYInterfaceSizeByMM(topMarginMM);
		AFloatNumber	bottomMargin	= GetYInterfaceSizeByMM(bottomMarginMM);
		if( pageRect.left	-paperRect.left		< leftMargin )
		{
			rect.left		= paperRect.left	+ leftMargin;
		}
		if( paperRect.right	-pageRect.right		< rightMargin )
		{
			rect.right		= paperRect.right	- rightMargin;
		}
		if( pageRect.top	-paperRect.top 		< topMargin )
		{
			rect.top		= paperRect.top		+ topMargin;
		}
		if( paperRect.bottom-pageRect.bottom 	< bottomMargin )
		{
			rect.bottom		= paperRect.bottom	- bottomMargin;
		}
		
		AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_StartPrintMode(rect);
		documentPrintModeStarted = true;
		AItemCount	totalPageCount = AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_GetPrintPageCount();
		::PMSetPageRange(mPrintSettings,1,totalPageCount);
		/*#1033
		if( mUseSheet == true )//B0335
		{
			status = ::PMSessionUseSheets(printSession,inWindowRef,mPrintDoneProc);
			if( status != noErr )   throw 0;
#if SUPPORT_CARBON
			CPrintImp*	ptr = this;
			err = ::SetWindowProperty(inWindowRef,'MMKE','Prnt',sizeof(CPrintImp*),&ptr);
			if( err != noErr )   _ACErrorNum("(SetWindowProperty) returned error: ",err,this);
#endif
		}
		*/
		//#1034 ::SetThemeCursor(kThemeArrowCursor);
		ACursorWrapper::SetCursor(kCursorType_Arrow);//#1034
		Boolean	accepted;
		err = ::PMSessionPrintDialog(printSession,mPrintSettings,mPageFormat,&accepted);
		if( err != noErr )   _ACErrorNum("PMSessionPrintDialog() returned error: ",err,this);
		//B0335
		//#1033 if( mUseSheet == false )
		{
			PrintLoop(printSession,accepted);
			err = ::PMRelease(printSession);
			if( err != noErr )   _ACErrorNum("PMRelease() returned error: ",err,this);
		}
	}
	catch(...)
	{
		if( documentPrintModeStarted == true )
		{
			AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_EndPrintMode();
		}
		if( printSession != NULL )
		{
			err = ::PMRelease(printSession);
			if( err != noErr )   _ACErrorNum("PMRelease() returned error: ",err,this);
		}
	}
#endif
}

//#1034
#if 0
/**
������s�i����_�C�A���OOK�������OS����R�[�������j
*/
void	CPrintImp::PrintLoop( PMPrintSession inPrintSession, const ABool inAccepted )
{
	OSStatus	err = noErr;
	
	if( inAccepted == true )
	{
		try
		{
			OSStatus	status;
			/*#1033
			CFStringRef s[1] = { kPMGraphicsContextCoreGraphics };
			CFArrayRef	graphicsContextsArray = ::CFArrayCreate(kCFAllocatorDefault,(const void**)s,1,&kCFTypeArrayCallBacks);
			status = ::PMSessionSetDocumentFormatGeneration (inPrintSession,kPMDocumentFormatPDF,graphicsContextsArray,NULL);
			::CFRelease(graphicsContextsArray);
			*/
			PMRect	pageRect;
			::PMGetAdjustedPageRect(mPageFormat,&pageRect);
			//win 080802
			PMRect	paperRect;
			::PMGetAdjustedPaperRect(mPageFormat,&paperRect);//DPI�ϊ������{����(Adjusted)�̂��́A���S�̂̃T�C�Y
			
			UInt32	totalPageCount = AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_GetPrintPageCount();
			
			UInt32	firstPage = 1, lastPage = totalPageCount;
			::PMGetFirstPage(mPrintSettings,&firstPage);
			::PMGetLastPage(mPrintSettings,&lastPage);
			if( lastPage > totalPageCount )
			{
				lastPage = totalPageCount;
			}
			//#1033
			::PMSetLastPage(mPrintSettings,lastPage,false);
			//#1033 status = ::PMSessionBeginDocument(inPrintSession,mPrintSettings,mPageFormat);
			status = ::PMSessionBeginCGDocument(inPrintSession,mPrintSettings,mPageFormat);
			if( status != noErr )   throw 0;
			for( UInt32 pageNumber = firstPage; pageNumber <= lastPage; pageNumber++ )
			{
				//#1033
				if( ::PMSessionError(inPrintSession) != noErr )
				{
					break;
				}
				//
				status = ::PMSessionBeginPage(inPrintSession,mPageFormat,NULL);
				if( status == noErr )
				{
					//#1033 status = ::PMSessionGetGraphicsContext(inPrintSession,NULL,reinterpret_cast<void**>(&mContextRef));
					status = ::PMSessionGetCGGraphicsContext(inPrintSession,&mContextRef);
					if( status == noErr )
					{
						//win 080802 CGContextTranslateCTM()�Ŏ擾������W�͎��S�̂̍�����(0,0)�ɂȂ��Ă�����ۂ�
						::CGContextTranslateCTM(mContextRef,-paperRect.left,paperRect.bottom-pageRect.top);//win 080802
						AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_PrintPage(pageNumber-1);
					}
				}
				::PMSessionEndPage(inPrintSession);
			}
			::PMSessionEndDocument(inPrintSession);
			//PMSessionError
		}
		catch(...)
		{
		}
	}
	err = ::PMRelease(mPrintSettings);
	if( err != noErr )   _ACErrorNum("PMRelease() returned error: ",err,this);
	mPrintSettings = NULL;
	AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_EndPrintMode();
}
#endif

//#1034
#if 0
/**
����I����callback
*/
pascal void	CPrintImp::PrintDoneProc( PMPrintSession printSession, WindowRef documentWindow, Boolean accepted )
{
	OSStatus	err = noErr;
	
	CPrintImp*	printImp;
#if SUPPORT_CARBON
	UInt32	actSize;
	err = ::GetWindowProperty(documentWindow,'MMKE','Prnt',sizeof(printImp),&actSize,&printImp);
#endif
	if( err == noErr && printImp != NULL )
	{
		printImp->PrintLoop(printSession,accepted);
	}
	else
	{
		_ACError("",NULL);
	}
#if SUPPORT_CARBON
	err = ::RemoveWindowProperty(documentWindow,'MMKE','Prnt');
	if( err != noErr )   _ACErrorNum("RemoveWindowProperty() returned error: ",err,NULL);
#endif
	err = ::PMRelease(printSession);
	if( err != noErr )   _ACErrorNum("PMRelease() returned error: ",err,NULL);
	
	AApplication::GetApplication().NVI_UpdateMenu();
}
#endif

//win 080729
/**
X������mm�P�ʃT�C�Y��Print API���̒P�ʂɕϊ�
*/
AFloatNumber	CPrintImp::GetXInterfaceSizeByMM( const AFloatNumber inMillimeter ) const
{
	return inMillimeter*mXResolution/25.4;
}

/**
Y������mm�P�ʃT�C�Y��Print API���̒P�ʂɕϊ�
*/
AFloatNumber	CPrintImp::GetYInterfaceSizeByMM( const AFloatNumber inMillimeter ) const
{
	return inMillimeter*mYResolution/25.4;
}

#pragma mark ===========================

#pragma mark ---�`�惋�[�`��

/**
�e�L�X�g�`��
*/
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, const AText& inText, const AJustification inJustification ) 
{
	DrawText(inDrawRect,inDrawRect,inText,mColor,mStyle,inJustification);
}

/**
�e�L�X�g�`��
*/
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AText& inText, const AJustification inJustification ) 
{
	DrawText(inDrawRect,inClipRect,inText,mColor,mStyle,inJustification);
}

/**
�e�L�X�g�`��
*/
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification ) 
{
	DrawText(inDrawRect,inDrawRect,inText,inColor,inStyle,inJustification);
}

/**
�e�L�X�g�`��
*/
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification ) 
{
	CTextDrawData	textDrawData(false);
	textDrawData.UTF16DrawText.SetText(inText);
	textDrawData.UTF16DrawText.ConvertFromUTF8ToUTF16();
	textDrawData.attrPos.Add(0);
	textDrawData.attrColor.Add(inColor);
	textDrawData.attrStyle.Add(inStyle);
	DrawText(inDrawRect,inClipRect,textDrawData,inJustification);
}

/**
�e�L�X�g�`��
*/
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, CTextDrawData& inTextDrawData, const AJustification inJustification ) 
{
	DrawText(inDrawRect,inDrawRect,inTextDrawData,inJustification);
}

/**
�e�L�X�g�`��
*/
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, CTextDrawData& inTextDrawData, const AJustification inJustification ) 
{
	OSStatus	err = noErr;
	
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	//lineAscent�́AATSUGetUnjustifiedBounds()�Ŏ擾����ƁA�\�����镶���ɂ����ascent���ω�����̂ŁA���L�Ŏ擾�B
	ANumber	lineHeight,lineAscent;
	GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
	
	//CTLine�擾
	CTLineRef	line = GetCTLineFromTextDrawData(inTextDrawData);
	//Justification�ݒ�
	ANumber	x = inDrawRect.left;
	ANumber	y = inDrawRect.top;//#450
	if( inJustification != kJustification_Left )
	{
		CGFloat	ascent = 9, descent = 3, leading = 3;
		double	width = ::CTLineGetTypographicBounds(line,&ascent,&descent,&leading);
		{
			switch(inJustification)
			{
			  case kJustification_Right:
				{
					x = inDrawRect.right - width;
					break;
				}
			  case kJustification_RightCenter://#450
				{
					x = inDrawRect.right - width;
					y = inDrawRect.top + (inDrawRect.bottom-inDrawRect.top)/2 - lineHeight/2;
					break;
				}
			  case kJustification_Center:
				{
					x = (inDrawRect.left+inDrawRect.right)/2 - width/2;
					break;
				}
			  case kJustification_LeftTruncated://#315
				{
					if( width > inDrawRect.right-inDrawRect.left )
					{
						x = inDrawRect.right - width;
					}
					break;
				}
			  case kJustification_CenterTruncated://#315
				{
					if( width > inDrawRect.right-inDrawRect.left )
					{
						x = inDrawRect.right - width;
					}
					else
					{
						x = (inDrawRect.left+inDrawRect.right)/2 - width/2;
					}
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
		}
	}
	::CGContextSaveGState(mContextRef);
	
	//Text�`��
	::CGContextSetTextMatrix(mContextRef, CGAffineTransformIdentity);
	::CGContextSetTextPosition(mContextRef,x,-(y+lineAscent));
	::CTLineDraw(line,mContextRef);
	
	::CGContextRestoreGState(mContextRef);
	
	//CTLineRef��CTextDrawData�̃f�X�g���N�^�Ŕj������
	
}

//#1034
/**
TextDrawData����CTLine�擾�i�������Ȃ琶���j
*/
CTLineRef	CPrintImp::GetCTLineFromTextDrawData( CTextDrawData& inTextDrawData ) 
{
	//�t�H���g���ݒ�Ȃ�ݒ肷��
	if( CocoaObjects->GetNormalFont() == NULL || CocoaObjects->GetBoldFont() == NULL || 
		CocoaObjects->GetItalicFont() == NULL || CocoaObjects->GetBoldItalicFont() == NULL )
	{
		UpdateFont();
	}
	
	if( inTextDrawData.GetCTLineRef() == NULL )
	{
		// ==================== �ŏ��̑����Ƀf�t�H���g������ ====================
		//�f�t�H���g�̃t�H���g�E�F��ݒ肷�邽�߂ɁA�ŏ��̍��ڂɑ���������
		inTextDrawData.attrPos.Insert1(0,0);
		inTextDrawData.attrColor.Insert1(0,mColor);
		inTextDrawData.attrStyle.Insert1(0,mStyle);
		
		// ==================== �\���e�L�X�g�擾 ====================
		
		//�\���e�L�X�g�擾
		AStCreateNSStringFromUTF16Text	str(inTextDrawData.UTF16DrawText);
		//�\���e�L�X�g��NSMutableAttributedString�擾
		NSMutableAttributedString *attrString = [[NSMutableAttributedString alloc] initWithString:str.GetNSString()];
		//attrString�̓C�x���g���[�v�I���ŉ���BCTLine�ŃR�s�[���Ă��邩�Q�Ƃ��Ă��邩�͕s�������A�Q�Ƃ��Ă���Ȃ�retain()���Ă���͂��B
		[attrString autorelease];
		//NSMutableAttributedString�̕ҏW�J�n
		[attrString beginEditing];
		//[attrString addAttribute:NSForegroundColorAttributeName 
		//value:[NSColor colorWithCalibratedRed:1.0 green:0.0 blue:0.0 alpha:1.0] range:NSMakeRange(0,[attrString length])];
		
		// ==================== �e�L�X�g����(RunStyle)�ݒ� ====================
		//inTextDrawData�ɋL�ڂ��ꂽ������ݒ�
		for( AIndex i = 0; i < inTextDrawData.attrPos.GetItemCount(); i++ )
		{
			// -------------------- Range�擾 --------------------
			//len�ɁA��������RunLength�́AUniChar�ł̒������i�[����B
			AItemCount	len;
			if( i+1 < inTextDrawData.attrPos.GetItemCount() )
			{
				//�Ō�ȊO�̗v�f�̏ꍇ
				len = inTextDrawData.attrPos.Get(i+1) - inTextDrawData.attrPos.Get(i);
			}
			else
			{
				//�Ō�̗v�f�̏ꍇ
				len = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(UniChar) - inTextDrawData.attrPos.Get(i);
			}
			//RunLength�̒�����0�Ȃ牽�����Ȃ�
			if( len <= 0 )   continue;
			//range�ݒ�
			NSRange	range = NSMakeRange(inTextDrawData.attrPos.Get(i),len);
			
			// -------------------- �����ݒ� --------------------
			
			//�t�H���g�ݒ�
			if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_Bold) == 0 )
			{
				if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_Italic) == 0 )
				{
					//�ʏ�t�H���g
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetNormalFont() range:range];
				}
				else
				{
					//italic�t�H���g
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetItalicFont() range:range];
				}
			}
			else
			{
				if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_Italic) == 0 )
				{
					//bold�t�H���g
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetBoldFont() range:range];
				}
				else
				{
					//bold+italic�t�H���g
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetBoldItalicFont() range:range];
				}
			}
			
			//�A���_�[���C��
			if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_Underline) != 0 )
			{
				[attrString addAttribute:NSUnderlineStyleAttributeName 
				value:[NSNumber numberWithInteger:NSUnderlineStyleSingle] range:range];
			}
			
			// �����F�ݒ�
			RGBColor	c = inTextDrawData.attrColor.Get(i);
			[attrString addAttribute:NSForegroundColorAttributeName 
			value:[NSColor colorWithCalibratedRed:(c.red/65535.0) green:(c.green/65535.0) blue:(c.blue/65535.0) alpha:1.0] range:range];
		}
		
		//#558
		//�c���������ݒ�
		if( mVerticalMode == true )
		{
			AddVerticalAttribute(attrString,str.GetNSString());
		}
		//NSMutableAttributedString�̕ҏW�I��
		[attrString endEditing];
		
		//CTLine����
		CTLineRef line = ::CTLineCreateWithAttributedString((CFAttributedStringRef)attrString);
		inTextDrawData.SetCTLineRef(line);
	}
	return inTextDrawData.GetCTLineRef();
}

/**
���`��
*/
void	CPrintImp::DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
		const AFloatNumber inAlpha, const AFloatNumber inLineDash, const AFloatNumber inLineWidth ) const
{
	if( mContextRef == NULL )   {_ACError("not in PrintPageLoop",this);return;}
	
	::CGContextSaveGState(mContextRef);
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextBeginPath(mContextRef);
	::CGContextMoveToPoint(mContextRef,inStartPoint.x,-inStartPoint.y);
	::CGContextAddLineToPoint(mContextRef,inEndPoint.x,-inEndPoint.y);
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	::CGContextSetLineWidth(mContextRef,inLineWidth);
	::CGContextSetShouldAntialias(mContextRef,false);
	if( inLineDash != 0.0 )
	{
		CGFloat	lengths[2];
		lengths[0] = inLineDash;
		lengths[1] = inLineDash;
		::CGContextSetLineDash(mContextRef,0.0,lengths,2);
	}
	::CGContextStrokePath(mContextRef);
	::CGContextRestoreGState(mContextRef);
}

//#1131
/**
�e�L�X�g�`�掞�̕����擾
*/
AFloatNumber	CPrintImp::GetDrawTextWidth( const AText& inText ) 
{
	CTextDrawData	textDrawData(false);
	textDrawData.UTF16DrawText.SetText(inText);
	textDrawData.UTF16DrawText.ConvertFromUTF8ToUTF16();
	textDrawData.attrPos.Add(0);
	textDrawData.attrColor.Add(mColor);
	textDrawData.attrStyle.Add(mStyle);
	return GetDrawTextWidth(textDrawData);
}

//#1131
/**
�e�L�X�g�`�掞�̕����擾
*/
AFloatNumber	CPrintImp::GetDrawTextWidth( CTextDrawData& inTextDrawData ) 
{
	//�e�L�X�g���擾
	CGFloat	ascent = 9, descent = 3, leading = 3;
	double	width = ::CTLineGetTypographicBounds(GetCTLineFromTextDrawData(inTextDrawData),&ascent,&descent,&leading);
	return width;
}

#pragma mark ===========================

#pragma mark ---�`�摮���ݒ�

//�e�L�X�g�f�t�H���g�����ݒ�
/*win void	CPrintImp::SetDefaultTextProperty( const AFont inFont, const AFloatNumber inFontSize, 
		const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing )
{
	mFont = inFont;
	mFontSize = inFontSize;
	mColor = inColor;
	mStyle = inStyle;
	mAntiAliasing = inAntiAliasing;
	
	//mATSUStyle�ݒ�
	CWindowImp::SetATSUStyle(mATSUTextStyle,mFont,mFontSize,mAntiAliasing);
	
}*/

//#1034
/**
�t�H���g�X�V
*/
void	CPrintImp::UpdateFont()
{
	//�ʏ�t�H���g�擾
	NSFont*	normalfont = CWindowImp::GetNSFont(mFontName,mFontSize,kTextStyle_Normal);
	CocoaObjects->SetNormalFont(normalfont);
	//bold�t�H���g�擾
	NSFont*	boldfont = CWindowImp::GetNSFont(mFontName,mFontSize,kTextStyle_Bold);
	CocoaObjects->SetBoldFont(boldfont);
	//italic�t�H���g�擾
	NSFont*	italicfont = CWindowImp::GetNSFont(mFontName,mFontSize,kTextStyle_Italic);
	CocoaObjects->SetItalicFont(italicfont);
	//bold+italic�t�H���g�擾
	NSFont*	bolditalicfont = CWindowImp::GetNSFont(mFontName,mFontSize,kTextStyle_Bold|kTextStyle_Italic);
	CocoaObjects->SetBoldItalicFont(bolditalicfont);
	/*
	NSLog([normalfont displayName]);
	NSLog([boldfont displayName]);
	NSLog([italicfont displayName]);
	NSLog([bolditalicfont displayName]);
	*/
}

/**
�e�L�X�gProperty�ݒ�
*/
void	CPrintImp::SetDefaultTextProperty( const AText& inFontName, const AFloatNumber inFontSize, 
		const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing )
{
	/*#688
	//
	if( AFontWrapper::GetFontByName(inFontName,mFont) == false )
	{
	}
	*/
	mFontName.SetText(inFontName);//#688
	mFontSize = inFontSize;
	mColor = inColor;
	mStyle = inStyle;
	mAntiAliasing = inAntiAliasing;
	
	/*#1034
	//mATSUStyle�ݒ�
	CWindowImp::SetATSUStyle(mATSUTextStyle,mFontName,mFontSize,mAntiAliasing,mColor,1.0,mStyle);
	*/
	//�t�H���g�X�V #1034
	UpdateFont();
}

/**
�e�L�X�g���擾
*/
void	CPrintImp::GetMetricsForDefaultTextProperty( ANumber& outLineHeight, ANumber& outLineAscent )
{
	CTextDrawData	textDrawData(false);
	textDrawData.UTF16DrawText.SetText(mTextForMetricsCalculation);
	textDrawData.attrPos.Add(0);
	textDrawData.attrColor.Add(mColor);
	textDrawData.attrStyle.Add(mStyle);
	CGFloat	ascent = 9, descent = 3, leading = 3;
	double	width = ::CTLineGetTypographicBounds(GetCTLineFromTextDrawData(textDrawData),&ascent,&descent,&leading);
	//leading������ƍs�̍����������Ȃ肷����̂ŁAascent+descent�ɂ���B
	outLineHeight = ceil(ascent+descent+2);
	outLineAscent = ceil(ascent+1);
	/*#1034
	ATSUTextMeasurement	ascent, descent, leading;
	ByteCount	actSize;
	::ATSUGetAttribute(mATSUTextStyle,kATSUAscentTag,sizeof(ATSUTextMeasurement),&ascent,&actSize);
	::ATSUGetAttribute(mATSUTextStyle,kATSUDescentTag,sizeof(ATSUTextMeasurement),&descent,&actSize);
	::ATSUGetAttribute(mATSUTextStyle,kATSULeadingTag,sizeof(ATSUTextMeasurement),&leading,&actSize);
	outLineHeight = ::Fix2Long(ascent) +::Fix2Long(descent) + ::Fix2Long(leading);
	outLineAscent =::Fix2Long(ascent);
	*/
}

/**
�`��
*/
void	CPrintImp::APICB_CocoaDrawRect( const ALocalRect& inDirtyRect )
{
	NSPrintOperation *prop = [NSPrintOperation currentOperation];
	NSInteger pageIndex = [prop currentPage];
	mContextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	
	//��L�Ŏ擾����Context�̍��W�n�́AHIView�̍��オ���_�Ay���̃v���X�͉������ƂȂ��Ă���B
	//�������AQuartz�ł́Ay���̃v���X��������ł��邱�Ƃ��O��ƂȂ��Ă���A
	//���̂܂܂��ƁAText��s�N�`���̕`�悪�㉺�t���܂ɂȂ��Ă��܂��B
	//�����������邽�߁Ay���̃v���X��������ɂȂ�悤�ɁA���L�������s���B
	//�i���̂��߁AContext�ւ̕`��́AHIView�̍��オ���_�ł͂��邪�Ay�̓v���X�}�C�i�X���t�ɕϊ�����K�v������B�j
	::CGContextScaleCTM(mContextRef, 1.0, -1.0);
	
	//�y�[�W�`��
	AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_PrintPage(pageIndex-1);
	
	//#558
	mContextRef = nil;
}

#pragma mark ===========================

#pragma mark ---�c����

//#558
/**
�c�����ݒ�
*/
void	CPrintImp::SetVerticalMode( const ABool inVerticalMode )
{
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
	{
		//�c�����t���O�ݒ�
		mVerticalMode = inVerticalMode;
		
		//���W�n�ݒ�
		if( mPrintView!= nil && mContextRef != nil && mVerticalCTM != mVerticalMode )
		{
			if( mVerticalMode == true )
			{
				CGContextRotateCTM(mContextRef,-M_PI/2.0);
				CGContextTranslateCTM(mContextRef,0,[mPrintView frame].size.width);
			}
			else
			{
				CGContextTranslateCTM(mContextRef,0,-[mPrintView frame].size.width);
				CGContextRotateCTM(mContextRef,M_PI/2.0);
			}
			mVerticalCTM = mVerticalMode;
		}
	}
}

//#558
/**
�c���������ݒ�
*/
void	CPrintImp::AddVerticalAttribute( NSMutableAttributedString *inAttrString, NSString* inString )
{
	//NSVerticalGlyphFormAttributeName��ݒ�
	NSRange	range = {0,[inString length]};
	[inAttrString addAttribute:NSVerticalGlyphFormAttributeName value:[NSNumber numberWithInt:1] range:range];
}

//#1034
//����pview
@implementation CocoaPrintView

#pragma mark ===========================
#pragma mark [�N���X]CocoaPrintView
#pragma mark ===========================

#pragma mark ===========================

#pragma mark ---�����ݒ�

/**
�����ݒ�
*/
- (id)initWithFrame:(NSRect)inFrame printImp:(CPrintImp*)inPrintImp totalPageCount:(NSInteger)totalPageCount
{
	//PrintImp�ݒ�
	mPrintImp = inPrintImp;
	//�y�[�W��
	mTotalPageCount = totalPageCount;
	//�p���������s
	self = [super initWithFrame:inFrame];
	return self;
}

#pragma mark ===========================

#pragma mark ---View�̃f�[�^�擾

/**
Y�������W���㉺�ϊ����邩�ǂ����iYES��Ԃ��j
*/
- (BOOL)isFlipped
{
	//YES��Ԃ����Ƃɂ�荶�オ���_�ɂȂ�B
	//�������A�e�L�X�g�`�擙�̏㉺���]�͂�����YES��Ԃ������ł͎��{����Ȃ��B
	//CUserPane::APICB_CocoaDrawRect()�ɂ�CGContextScaleCTM()�����s���邱�Ƃŏ㉺���]����B
	//�i�]��Carbon�ł�CGContext�����Ɠ����By�̓}�C�i�X�����֕`�悷�邱�ƂɂȂ�B�j
	return YES;
}

/**
�s����view���ǂ���
*/
- (BOOL)isOpaque
{
	//�s������Ԃ��B
	return YES;
}

#pragma mark ===========================

#pragma mark ---����f�[�^�擾

/**
�y�[�W����Ԃ�
*/
- (BOOL)knowsPageRange:(NSRangePointer)aRange 
{
	aRange->location = 1;
	aRange->length = mTotalPageCount;
	return YES;
}

/**
�e�y�[�W�̕`��̈��Ԃ�
*/
- (NSRect)rectForPage:(NSInteger)pageNumber 
{
	return self.frame;
}

#pragma mark ===========================

#pragma mark ---�`��

/**
��ʕ`��R�[���o�b�N����
*/
- (void)drawRect:(NSRect)dirtyRect
{
	OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(false);
	
	//dirtyRect����LocalRect�֕ϊ��iisFlipped=YES�Ȃ̂ł��̂܂܂̒l�j
	ALocalRect	localRect = {0,0,0,0};
	localRect.left		= dirtyRect.origin.x;
	localRect.top		= dirtyRect.origin.y;
	localRect.right		= localRect.left + dirtyRect.size.width;
	localRect.bottom	= localRect.top  + dirtyRect.size.height;
	//CUserPane�̕`�揈�����s
	mPrintImp->APICB_CocoaDrawRect(localRect);
	
	OS_CALLBACKBLOCK_END;
}

@end




