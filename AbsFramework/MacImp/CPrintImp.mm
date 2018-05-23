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
#pragma mark [クラス]CPrintImp
#pragma mark ===========================

#pragma mark --- コンストラクタ／デストラクタ

/**
コンストラクタ
*/
CPrintImp::CPrintImp( const ADocumentID inDocumentID ) : AObjectArrayItem(NULL), /*#1034 mPageFormat(NULL),
		mPrintSettings(NULL),*/ mDocumentID(inDocumentID), mContextRef(NULL), /*#1034 mUseSheet(true),*/ mXResolution(72.0), mYResolution(72.0)
,CocoaObjects(NULL)//#1034
,mPrintView(nil),mVerticalMode(false),mVerticalCTM(false)//#558
{
	//Cocoaオブジェクト生成 #1034
	CocoaObjects = new CUserPaneCocoaObjects();
	
	//#1034
	mTextForMetricsCalculation.AddFromUCS4Char(0x0079);//'y'
	mTextForMetricsCalculation.AddFromUCS4Char(0xFFE3);//'￣'
	//mTextForMetricsCalculation.AddFromUCS4Char(0xFF3F);//'＿'
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
デストラクタ
*/
CPrintImp::~CPrintImp()
{
	//#1034 OSStatus	err = noErr;
	
	//Cocoaオブジェクト削除 #1034
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

#pragma mark --- ページ設定

/**
ページ設定
*/
void	CPrintImp::PageSetup( /*#182 CWindowImp& inWindowImp*/ const AWindowRef inWindowRef )
{
	//#1034
	
	//マウスカーソル設定
	ACursorWrapper::SetCursor(kCursorType_Arrow);
	
	//ページ設定ダイアログ
	[[NSApplication sharedApplication] runPageLayout:nil];
#if 0
	OSStatus	err = noErr;
	
	PMPrintSession	printSession = NULL;
	try
	{
		OSStatus	status = noErr;
		//PrintSession生成
		status = ::PMCreateSession(&printSession);
		if( status != noErr )   throw 0;
		//PageFormat
		SetupPageFormat();//#1033
		if( mPageFormat == NULL )   throw 0;
		//#1033
		//PageFormat有効化
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
ページ設定データSetup
*/
void	CPrintImp::SetupPageFormat()//#1033
{
	/*#1033 Quartz2DBasicsサンプルのとおりに作り直し
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
	//PageFormat作成用セッション作成
	PMPrintSession printSession = NULL;
	status = ::PMCreateSession(&printSession);
	if( status != noErr )
	{
		_ACErrorNum("PMCreateSession() returned error: ",status,this);
		throw 0;
	}
	//PageFormat生成
	status = ::PMCreatePageFormat(&mPageFormat);
	if( status != noErr )
	{
		_ACErrorNum("PMCreatePageFormat() returned error: ",status,this);
		throw 0;
	}
	//PageFormatにdefault設定
	status = ::PMSessionDefaultPageFormat(printSession, mPageFormat);
	if( status != noErr )
	{
		::PMRelease(mPageFormat);
		mPageFormat = NULL;
		_ACErrorNum("PMSessionDefaultPageFormat() returned error: ",status,this);
		throw 0;
	}
	//PageFormat作成用セッション解放
	status = ::PMRelease(printSession);
	if( status != noErr )
	{
		_ACErrorNum("PMRelease() returned error: ",status,this);
		throw 0;
	}
}
#endif

#pragma mark ===========================

#pragma mark --- プリント

/**
印刷
*/
void	CPrintImp::Print( /*#182 CWindowImp& inWindowImp*/const AWindowRef inWindowRef, const AText& inJobTitle )
{
	//PrintInfo取得
	NSPrintInfo *printInfo = [NSPrintInfo sharedPrintInfo];
	
	//PrintInfo余白設定
	//余白は0に設定し、print viewは用紙一杯に広げる。
	//view内の表示範囲はEVT_StartPrintMode()で設定し、アプリ側で余白制御する。
	//（OS側のこれらの余白制御の仕様が不可解な動作をしているため。）
	[printInfo setLeftMargin:0];
	[printInfo setRightMargin:0];
	[printInfo setTopMargin:0];
	[printInfo setBottomMargin:0];
	
	//余白設定値取得
	AFloatNumber	leftMarginMM	= 10.0;
	AFloatNumber	rightMarginMM	= 10.0;
	AFloatNumber	topMarginMM		= 10.0;
	AFloatNumber	bottomMarginMM	= 10.0;
	AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).NVI_GetPrintMargin(leftMarginMM,rightMarginMM,topMarginMM,bottomMarginMM);
	AFloatNumber	leftMargin		= GetXInterfaceSizeByMM(leftMarginMM);
	AFloatNumber	rightMargin		= GetXInterfaceSizeByMM(rightMarginMM);
	AFloatNumber	topMargin		= GetYInterfaceSizeByMM(topMarginMM);
	AFloatNumber	bottomMargin	= GetYInterfaceSizeByMM(bottomMarginMM);
	
	//用紙サイズ取得
	NSSize paperSize = [printInfo paperSize];
	
	//余白設定反映したページサイズ取得
	CGFloat	pageWidth = paperSize.width - leftMargin - rightMargin;
	CGFloat	pageHeight = paperSize.height - topMargin - bottomMargin;
	
	//印刷範囲rect取得
	ALocalRect	pageRect = {0};
	pageRect.left		= leftMargin;
	pageRect.top		= topMargin;
	pageRect.right		= pageRect.left + pageWidth;
	pageRect.bottom		= pageRect.top + pageHeight;
	
	//印刷モード開始
	AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_StartPrintMode(pageRect,paperSize.width,paperSize.height);//#558
	
	//ページ数取得
	AItemCount	totalPageCount = AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_GetPrintPageCount();
	
	//print view生成
	NSRect frameRect = NSMakeRect(0,0,paperSize.width,paperSize.height);
	mPrintView = [[CocoaPrintView alloc] initWithFrame:frameRect printImp:this totalPageCount:totalPageCount];//#558
	
	//マウスカーソル設定
	ACursorWrapper::SetCursor(kCursorType_Arrow);
	
	//印刷実行
	NSPrintOperation *prop = [NSPrintOperation printOperationWithView:mPrintView printInfo:printInfo];//#558
	{
		//ジョブ名 #1104
		AStCreateNSStringFromAText	str(inJobTitle);
		[prop setJobTitle:str.GetNSString()];
	}
	[prop setShowsPrintPanel: YES];
	[prop setShowsProgressPanel: YES];
	[prop runOperation];
	
	//print view解放
	[mPrintView release];//#558
	mPrintView = nil;//#558
	
	//印刷モード終了
	AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_EndPrintMode();
	
#if 0
	OSStatus	err = noErr;
	
	PMPrintSession	printSession = NULL;
	ABool	documentPrintModeStarted = false;
	try
	{
		OSStatus	status = noErr;
		//PrintSession生成
		status = ::PMCreateSession(&printSession);
		if( status != noErr )   throw 0;
		//PageFormat
		SetupPageFormat();//#1033
		if( mPageFormat == NULL )   throw 0;
		//#1033
		//PageFormat有効化
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
		//ジョブ名
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
		//ページ番号範囲
		PMRect	pageRect;
		status = ::PMGetAdjustedPageRect(mPageFormat,&pageRect);
		ALocalRect	rect;
		rect.left		= pageRect.left;
		rect.top		= pageRect.top;
		rect.right		= pageRect.right;
		rect.bottom		= pageRect.bottom;
		//win 080802
		PMRect	paperRect;
		::PMGetAdjustedPaperRect(mPageFormat,&paperRect);//DPI変換等を施した(Adjusted)のちの、紙全体のサイズ
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
印刷実行（印刷ダイアログOK押下後にOSからコールされる）
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
			::PMGetAdjustedPaperRect(mPageFormat,&paperRect);//DPI変換等を施した(Adjusted)のちの、紙全体のサイズ
			
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
						//win 080802 CGContextTranslateCTM()で取得する座標は紙全体の左下が(0,0)になっているっぽい
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
印刷終了時callback
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
X方向のmm単位サイズをPrint API内の単位に変換
*/
AFloatNumber	CPrintImp::GetXInterfaceSizeByMM( const AFloatNumber inMillimeter ) const
{
	return inMillimeter*mXResolution/25.4;
}

/**
Y方向のmm単位サイズをPrint API内の単位に変換
*/
AFloatNumber	CPrintImp::GetYInterfaceSizeByMM( const AFloatNumber inMillimeter ) const
{
	return inMillimeter*mYResolution/25.4;
}

#pragma mark ===========================

#pragma mark ---描画ルーチン

/**
テキスト描画
*/
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, const AText& inText, const AJustification inJustification ) 
{
	DrawText(inDrawRect,inDrawRect,inText,mColor,mStyle,inJustification);
}

/**
テキスト描画
*/
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AText& inText, const AJustification inJustification ) 
{
	DrawText(inDrawRect,inClipRect,inText,mColor,mStyle,inJustification);
}

/**
テキスト描画
*/
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification ) 
{
	DrawText(inDrawRect,inDrawRect,inText,inColor,inStyle,inJustification);
}

/**
テキスト描画
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
テキスト描画
*/
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, CTextDrawData& inTextDrawData, const AJustification inJustification ) 
{
	DrawText(inDrawRect,inDrawRect,inTextDrawData,inJustification);
}

/**
テキスト描画
*/
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, CTextDrawData& inTextDrawData, const AJustification inJustification ) 
{
	OSStatus	err = noErr;
	
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	//lineAscentは、ATSUGetUnjustifiedBounds()で取得すると、表示する文字によってascentが変化するので、下記で取得。
	ANumber	lineHeight,lineAscent;
	GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
	
	//CTLine取得
	CTLineRef	line = GetCTLineFromTextDrawData(inTextDrawData);
	//Justification設定
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
					//処理なし
					break;
				}
			}
		}
	}
	::CGContextSaveGState(mContextRef);
	
	//Text描画
	::CGContextSetTextMatrix(mContextRef, CGAffineTransformIdentity);
	::CGContextSetTextPosition(mContextRef,x,-(y+lineAscent));
	::CTLineDraw(line,mContextRef);
	
	::CGContextRestoreGState(mContextRef);
	
	//CTLineRefはCTextDrawDataのデストラクタで破棄する
	
}

//#1034
/**
TextDrawDataからCTLine取得（未生成なら生成）
*/
CTLineRef	CPrintImp::GetCTLineFromTextDrawData( CTextDrawData& inTextDrawData ) 
{
	//フォント未設定なら設定する
	if( CocoaObjects->GetNormalFont() == NULL || CocoaObjects->GetBoldFont() == NULL || 
		CocoaObjects->GetItalicFont() == NULL || CocoaObjects->GetBoldItalicFont() == NULL )
	{
		UpdateFont();
	}
	
	if( inTextDrawData.GetCTLineRef() == NULL )
	{
		// ==================== 最初の属性にデフォルトを入れる ====================
		//デフォルトのフォント・色を設定するために、最初の項目に属性を入れる
		inTextDrawData.attrPos.Insert1(0,0);
		inTextDrawData.attrColor.Insert1(0,mColor);
		inTextDrawData.attrStyle.Insert1(0,mStyle);
		
		// ==================== 表示テキスト取得 ====================
		
		//表示テキスト取得
		AStCreateNSStringFromUTF16Text	str(inTextDrawData.UTF16DrawText);
		//表示テキストのNSMutableAttributedString取得
		NSMutableAttributedString *attrString = [[NSMutableAttributedString alloc] initWithString:str.GetNSString()];
		//attrStringはイベントループ終了で解放。CTLineでコピーしているか参照しているかは不明だが、参照しているならretain()しているはず。
		[attrString autorelease];
		//NSMutableAttributedStringの編集開始
		[attrString beginEditing];
		//[attrString addAttribute:NSForegroundColorAttributeName 
		//value:[NSColor colorWithCalibratedRed:1.0 green:0.0 blue:0.0 alpha:1.0] range:NSMakeRange(0,[attrString length])];
		
		// ==================== テキスト属性(RunStyle)設定 ====================
		//inTextDrawDataに記載された属性を設定
		for( AIndex i = 0; i < inTextDrawData.attrPos.GetItemCount(); i++ )
		{
			// -------------------- Range取得 --------------------
			//lenに、処理するRunLengthの、UniCharでの長さを格納する。
			AItemCount	len;
			if( i+1 < inTextDrawData.attrPos.GetItemCount() )
			{
				//最後以外の要素の場合
				len = inTextDrawData.attrPos.Get(i+1) - inTextDrawData.attrPos.Get(i);
			}
			else
			{
				//最後の要素の場合
				len = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(UniChar) - inTextDrawData.attrPos.Get(i);
			}
			//RunLengthの長さが0なら何もしない
			if( len <= 0 )   continue;
			//range設定
			NSRange	range = NSMakeRange(inTextDrawData.attrPos.Get(i),len);
			
			// -------------------- 属性設定 --------------------
			
			//フォント設定
			if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_Bold) == 0 )
			{
				if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_Italic) == 0 )
				{
					//通常フォント
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetNormalFont() range:range];
				}
				else
				{
					//italicフォント
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetItalicFont() range:range];
				}
			}
			else
			{
				if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_Italic) == 0 )
				{
					//boldフォント
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetBoldFont() range:range];
				}
				else
				{
					//bold+italicフォント
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetBoldItalicFont() range:range];
				}
			}
			
			//アンダーライン
			if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_Underline) != 0 )
			{
				[attrString addAttribute:NSUnderlineStyleAttributeName 
				value:[NSNumber numberWithInteger:NSUnderlineStyleSingle] range:range];
			}
			
			// 文字色設定
			RGBColor	c = inTextDrawData.attrColor.Get(i);
			[attrString addAttribute:NSForegroundColorAttributeName 
			value:[NSColor colorWithCalibratedRed:(c.red/65535.0) green:(c.green/65535.0) blue:(c.blue/65535.0) alpha:1.0] range:range];
		}
		
		//#558
		//縦書き属性設定
		if( mVerticalMode == true )
		{
			AddVerticalAttribute(attrString,str.GetNSString());
		}
		//NSMutableAttributedStringの編集終了
		[attrString endEditing];
		
		//CTLine生成
		CTLineRef line = ::CTLineCreateWithAttributedString((CFAttributedStringRef)attrString);
		inTextDrawData.SetCTLineRef(line);
	}
	return inTextDrawData.GetCTLineRef();
}

/**
線描画
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
テキスト描画時の幅を取得
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
テキスト描画時の幅を取得
*/
AFloatNumber	CPrintImp::GetDrawTextWidth( CTextDrawData& inTextDrawData ) 
{
	//テキスト幅取得
	CGFloat	ascent = 9, descent = 3, leading = 3;
	double	width = ::CTLineGetTypographicBounds(GetCTLineFromTextDrawData(inTextDrawData),&ascent,&descent,&leading);
	return width;
}

#pragma mark ===========================

#pragma mark ---描画属性設定

//テキストデフォルト属性設定
/*win void	CPrintImp::SetDefaultTextProperty( const AFont inFont, const AFloatNumber inFontSize, 
		const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing )
{
	mFont = inFont;
	mFontSize = inFontSize;
	mColor = inColor;
	mStyle = inStyle;
	mAntiAliasing = inAntiAliasing;
	
	//mATSUStyle設定
	CWindowImp::SetATSUStyle(mATSUTextStyle,mFont,mFontSize,mAntiAliasing);
	
}*/

//#1034
/**
フォント更新
*/
void	CPrintImp::UpdateFont()
{
	//通常フォント取得
	NSFont*	normalfont = CWindowImp::GetNSFont(mFontName,mFontSize,kTextStyle_Normal);
	CocoaObjects->SetNormalFont(normalfont);
	//boldフォント取得
	NSFont*	boldfont = CWindowImp::GetNSFont(mFontName,mFontSize,kTextStyle_Bold);
	CocoaObjects->SetBoldFont(boldfont);
	//italicフォント取得
	NSFont*	italicfont = CWindowImp::GetNSFont(mFontName,mFontSize,kTextStyle_Italic);
	CocoaObjects->SetItalicFont(italicfont);
	//bold+italicフォント取得
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
テキストProperty設定
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
	//mATSUStyle設定
	CWindowImp::SetATSUStyle(mATSUTextStyle,mFontName,mFontSize,mAntiAliasing,mColor,1.0,mStyle);
	*/
	//フォント更新 #1034
	UpdateFont();
}

/**
テキスト情報取得
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
	//leadingを入れると行の高さが高くなりすぎるので、ascent+descentにする。
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
描画
*/
void	CPrintImp::APICB_CocoaDrawRect( const ALocalRect& inDirtyRect )
{
	NSPrintOperation *prop = [NSPrintOperation currentOperation];
	NSInteger pageIndex = [prop currentPage];
	mContextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	
	//上記で取得したContextの座標系は、HIViewの左上が原点、y軸のプラスは下方向となっている。
	//しかし、Quartzでは、y軸のプラスが上方向であることが前提となっており、
	//このままだと、Textやピクチャの描画が上下逆さまになってしまう。
	//これを回避するため、y軸のプラスが上方向になるように、下記処理を行う。
	//（このため、Contextへの描画は、HIViewの左上が原点ではあるが、yはプラスマイナスを逆に変換する必要がある。）
	::CGContextScaleCTM(mContextRef, 1.0, -1.0);
	
	//ページ描画
	AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_PrintPage(pageIndex-1);
	
	//#558
	mContextRef = nil;
}

#pragma mark ===========================

#pragma mark ---縦書き

//#558
/**
縦書き設定
*/
void	CPrintImp::SetVerticalMode( const ABool inVerticalMode )
{
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
	{
		//縦書きフラグ設定
		mVerticalMode = inVerticalMode;
		
		//座標系設定
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
縦書き属性設定
*/
void	CPrintImp::AddVerticalAttribute( NSMutableAttributedString *inAttrString, NSString* inString )
{
	//NSVerticalGlyphFormAttributeNameを設定
	NSRange	range = {0,[inString length]};
	[inAttrString addAttribute:NSVerticalGlyphFormAttributeName value:[NSNumber numberWithInt:1] range:range];
}

//#1034
//印刷用view
@implementation CocoaPrintView

#pragma mark ===========================
#pragma mark [クラス]CocoaPrintView
#pragma mark ===========================

#pragma mark ===========================

#pragma mark ---初期設定

/**
初期設定
*/
- (id)initWithFrame:(NSRect)inFrame printImp:(CPrintImp*)inPrintImp totalPageCount:(NSInteger)totalPageCount
{
	//PrintImp設定
	mPrintImp = inPrintImp;
	//ページ数
	mTotalPageCount = totalPageCount;
	//継承処理実行
	self = [super initWithFrame:inFrame];
	return self;
}

#pragma mark ===========================

#pragma mark ---Viewのデータ取得

/**
Y方向座標を上下変換するかどうか（YESを返す）
*/
- (BOOL)isFlipped
{
	//YESを返すことにより左上が原点になる。
	//ただし、テキスト描画等の上下反転はここでYESを返すだけでは実施されない。
	//CUserPane::APICB_CocoaDrawRect()にてCGContextScaleCTM()を実行することで上下反転する。
	//（従来CarbonでのCGContext処理と同じ。yはマイナス方向へ描画することになる。）
	return YES;
}

/**
不透明viewかどうか
*/
- (BOOL)isOpaque
{
	//不透明を返す。
	return YES;
}

#pragma mark ===========================

#pragma mark ---印刷データ取得

/**
ページ数を返す
*/
- (BOOL)knowsPageRange:(NSRangePointer)aRange 
{
	aRange->location = 1;
	aRange->length = mTotalPageCount;
	return YES;
}

/**
各ページの描画領域を返す
*/
- (NSRect)rectForPage:(NSInteger)pageNumber 
{
	return self.frame;
}

#pragma mark ===========================

#pragma mark ---描画

/**
画面描画コールバック処理
*/
- (void)drawRect:(NSRect)dirtyRect
{
	OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(false);
	
	//dirtyRectからLocalRectへ変換（isFlipped=YESなのでそのままの値）
	ALocalRect	localRect = {0,0,0,0};
	localRect.left		= dirtyRect.origin.x;
	localRect.top		= dirtyRect.origin.y;
	localRect.right		= localRect.left + dirtyRect.size.width;
	localRect.bottom	= localRect.top  + dirtyRect.size.height;
	//CUserPaneの描画処理実行
	mPrintImp->APICB_CocoaDrawRect(localRect);
	
	OS_CALLBACKBLOCK_END;
}

@end




