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

CUserPane

*/

#include "CUserPane.h"
#include "CWindowImp.h"
//#1012 #include "StSetPort.h"
#include "../Frame.h"
#include "CTextDrawData.h"
#include "CWindowImpCocoaObjects.h"
#include <CoreFoundation/CoreFoundation.h>

#import <Cocoa/Cocoa.h>
#import "CocoaUserPaneView.h"
#import "AppDelegate.h"

extern NSRect	ConvertFromWindowRectToNSRect( NSView* inContentView, const AWindowRect inWindowRect );
extern NSPoint	ConvertFromWindowPointToNSPoint( NSView* inContentView, const AWindowPoint inWindowPoint );

//��s�ɕ\���ł���UTF16�o�C�g���̍ő� 20000�o�C�g(1000000����) #572 #1045 1����100��
//#1045 const AByteCount	kLineTextDrawLengthMax = 2000000;//#1045 20000��2000000

#pragma mark ===========================
#pragma mark [�N���X]CUserPane
#pragma mark ===========================
//UserPane�́AOS���W���T�|�[�g���Ă��Ȃ��R���g���[�����������邽�߂̊e�N���X�̊��N���X
//Interface Builder��ł�User Pane�Ƃ��Ē�`���A�`���N���b�N���̓��쓙���T�u�N���X�Œ�`����B

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
CUserPane::CUserPane( AObjectArrayItem* inParent, CWindowImp& inWindowImp, //688const AControlRef inControlRef
			const AControlID inControlID /*#688 , const ABool inInstallEventHandler*/ ) 
: AObjectArrayItem(inParent), mWindowImp(inWindowImp), /*#688 mControlRef(inControlRef)*/ mControlID(inControlID), 
		mCatchReturn(false), mCatchTab(false), mAutoSelectAllBySwitchFocus(true), mContextRef(NULL),mDrawTextCount(0),
		mEnableMouseLeaveEvent(false)//B0000 080810
		//#360,mSupportFocus(true)//#291
//#688,mDrawUpdateRegion(NULL)
,mCocoaDrawDirtyRect(kLocalRect_0000)//#688
,mDropShadowColor(kColor_White)//#725
,mFontSize(9.0), mColor(kColor_Black), mStyle(kTextStyle_Normal), mAntiAliasing(true)//#852
//#1034 ,mATSUTextStyle(NULL)//,mATSUTextStyleInited(false)//#852
,CocoaObjects(NULL)//#1034
,mTextAlpha(1.0)
,mLineHeight(0),mLineAscent(0)
,mAdjustScrollForRoundedCorner(true)//#947
,mVerticalMode(false)//#558
{
	//Cocoa�I�u�W�F�N�g���� #1034
	CocoaObjects = new CUserPaneCocoaObjects();
	
	OSStatus	err = noErr;
	
	/*#852
	err = ::ATSUCreateStyle(&mATSUTextStyle);
	if( err != noErr )   _ACErrorNum("ATSUCreateStyle() returned error: ",err,this);
	*/
	//mATSUTextStyle = CAppImp::CreateDefaultATSUStyle();//#852
#if SUPPORT_CARBON
	if( inInstallEventHandler == true )
	{
		InstallEventHandler();
	}
#endif
	//#636
	mTextForMetricsCalculation.AddFromUCS4Char(0x0079);//'y'
	mTextForMetricsCalculation.AddFromUCS4Char(0xFFE3);//'�P'
	//mTextForMetricsCalculation.AddFromUCS4Char(0xFF3F);//'�Q'
	mTextForMetricsCalculation.ConvertFromUTF8ToUTF16();
	//#852
	//�f�t�H���g�t�H���g���擾
	AFontWrapper::GetDialogDefaultFontName(mFontName);
}

/**
�f�X�g���N�^
*/
CUserPane::~CUserPane()
{
	//Cocoa�I�u�W�F�N�g�폜 #1034
	delete CocoaObjects;
	CocoaObjects = NULL;
	
	/*#1034
	OSStatus	err = noErr;
	if( mATSUTextStyle != NULL )//#852
	{
		err = ::ATSUDisposeStyle(mATSUTextStyle);
		if( err != noErr )   _ACErrorNum("ATSUDisposeStyle() returned error: ",err,this);
	}
	*/
}

/**
AWindow�擾
*/
AWindow&	CUserPane::GetAWin()
{
	return mWindowImp.GetAWin();
}

#pragma mark ===========================

#pragma mark ---�R���g���[�����擾

/**
������ControlID�擾
*/
/*#688
AControlID	CUserPane::GetControlID() const
{
	return GetWinConst().GetControlID(mControlRef);
}
*/

//#688
/**
ControlRef���擾
Cocoa�Ή��ɂ��UserPane�ł�ControlRef�͕ێ����Ȃ��悤�ɂ���̂ŁAWindowImp����擾
*/
/*#688
AControlRef	CUserPane::GetControlRef() const
{
	return GetWinConst().GetControlRef(mControlID);
}
*/

/**
�R���g���[���̈ʒu�擾
*/
void	CUserPane::GetControlPosition( AWindowPoint& outPoint ) const
{
	GetWinConst().GetControlPosition(GetControlID(),outPoint);
}

/**
�R���g���[���S�̂�Rect�擾(LocalRect)
*/
void	CUserPane::GetControlLocalFrameRect( ALocalRect& outRect ) const
{
	outRect.left 	= 0;
	outRect.top 	= 0;
	outRect.right 	= GetWinConst().GetControlWidth(GetControlID());
	outRect.bottom 	= GetWinConst().GetControlHeight(GetControlID());
}

/**
�R���g���[���S�̂�Rect�擾(WindowRect)
*/
void	CUserPane::GetControlWindowFrameRect( AWindowRect& outRect ) const
{
	ALocalRect	localRect;
	GetControlLocalFrameRect(localRect);
	GetWinConst().GetWindowRectFromControlLocalRect(GetControlID(),localRect,outRect);
}

/**
WindowRef�擾
*/
AWindowRef	CUserPane::GetWindowRef()
{
	return mWindowImp.GetWindowRef();
}

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark --- �C�x���g�n���h��

/**
�̈�ύX�C�x���g�n���h��
*/
ABool	CUserPane::APICB_DoControlBoundsChanged( const EventRef inEventRef )
{
	//�f�t�H���g����F�Ȃ�
	return true;
}

/**
�}�E�X�z�C�[���C�x���g�R�[���o�b�N
*/
ABool	CUserPane::DoMouseWheel( const EventRef inEventRef )
{
	//�f�t�H���g����F�Ȃ�
	return false;
}

/**
�}�E�X�ړ��C�x���g�R�[���o�b�N
*/
ABool	CUserPane::DoMouseMoved( const EventRef inEventRef )
{
	//�f�t�H���g����F�Ȃ�
	return false;
}

/**
�J�[�\�������R�[���o�b�N
*/
ABool	CUserPane::DoAdjustCursor( const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	err = ::SetThemeCursor(kThemeArrowCursor);
	if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,this);
	return true;
}

/**
�}�E�X�̈�O�ړ��R�[���o�b�N
*/
ABool	CUserPane::DoMouseExited( const EventRef inEventRef )
{
	//�f�t�H���g����F�Ȃ�
	return true;
}

#pragma mark ===========================

#pragma mark ---�R���g���[���t�H�[�J�X����

//virtual, public
/**
�t�H�[�J�X�l��
*/
void	CUserPane::SetFocus() 
{
	//�f�t�H���g����F��������
}

//virtual, public
/**
�t�H�[�J�X����
*/
void	CUserPane::ResetFocus()
{
	//�f�t�H���g����F��������
}

#pragma mark ===========================

#pragma mark ---���j���[�R�}���h����

//virtual, public
/**
���j���[�R�}���h����
*/
ABool	CUserPane::DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	//�f�t�H���g����F��������
	return false;
}

/**
���j���[�X�V����
*/
void	CUserPane::UpdateMenu()
{
	//�f�t�H���g����F��������
	return;
}
#endif

#pragma mark ===========================

#pragma mark --- �R���g���[�������ݒ�C���^�[�t�F�C�X

/**
�R���g���[��Enable/Disable
*/
void	CUserPane::SetEnableControl( const ABool inEnable )
{
	return GetWin().SetEnableControl(GetControlID(),inEnable);//#688 CWindowImp���ɃR���g���[���̏������W��
}

void	CUserPane::PreProcess_SetEnableControl( const ABool inEnable )//#688
{
	/*#688
	//#530
	if( ::IsControlEnabled(mControlRef) == inEnable )   return;
	//
	if( inEnable == true )
	{
		::EnableControl(mControlRef);
	}
	else
	{
		::DisableControl(mControlRef);
	}
	RefreshControl();
	*/
}

/**
�R���g���[����Enable���ǂ������擾
*/
ABool	CUserPane::IsControlEnabled() const
{
	//#688 return ::IsControlEnabled(mControlRef);
	return GetWinConst().IsControlEnabled(GetControlID());//#688 CWindowImp���ɃR���g���[���̏������W��
}

/**
�R���g���[��Show/Hide
*/
void	CUserPane::SetShowControl( const ABool inShow )
{
	return GetWin().SetShowControl(GetControlID(),inShow);//#688 CWindowImp���ɃR���g���[���̏������W��
}

void	CUserPane::PreProcess_SetShowControl( const ABool inShow )//#688
{
	/*#688
	if( inShow )
	{
		::ShowControl(mControlRef);
	}
	else
	{
		::HideControl(mControlRef);
	}
	*/
}

/**
�R���g���[����Show/Hide��Ԏ擾
*/
ABool	CUserPane::IsControlVisible() const
{
	//#688 return ::IsControlVisible(mControlRef);
	return GetWinConst().IsControlVisible(GetControlID());//#688 CWindowImp���ɃR���g���[���̏������W��
}

/**
�R���g���[����Active���ǂ������擾
*/
ABool	CUserPane::IsControlActive() const
{
	return GetWinConst().IsActive();
}

//win�Ή�
/**
�f�t�H���g�̃J�[�\����ݒ�
*/
void	CUserPane::SetDefaultCursor( const ACursorType inCursorType )
{
	//MacOSX�̏ꍇ�́A��������
}

/**
�}�E�X�̈�O�ړ��C�x���g�ېݒ�
*/
void	CUserPane::EnableMouseLeaveEvent()
{
	mEnableMouseLeaveEvent = true;
	/*
	���L���@�ł͓��삵�Ȃ��̂ŕ����ύX
	AApplication��EVTDO_MouseMoved���g�p����
	MouseTrackingRegionID	id;
	id.signature = 0;
	id.id = GetControlID();
	RgnHandle	rectrgn = ::NewRgn();
	Rect	frameRect;
	frameRect.left 		= 0;
	frameRect.top 		= 0;
	frameRect.right 	= GetWinConst().GetControlWidth(GetControlID());
	frameRect.bottom 	= GetWinConst().GetControlHeight(GetControlID());
	::RectRgn(rectrgn,&frameRect);
	MouseTrackingRef	trackingRef = NULL;
	::CreateMouseTrackingRegion(GetWindowRef(),rectrgn,NULL,kMouseTrackingOptionsLocalClip,id,NULL,NULL,&trackingRef);
	::DisposeRgn(rectrgn);
	
	OSStatus	err;
	EventHandlerRef	handlerRef;
	EventTypeSpec	typeSpec;
	typeSpec.eventClass = kEventClassMouse;
	typeSpec.eventKind = kEventMouseExited;
	err = ::InstallEventHandler(::GetControlEventTarget(mControlRef),::NewEventHandlerUPP(CUserPane::STATIC_APICB_MouseExitedHandler),
			1,&typeSpec,&mWindowImp,&handlerRef);
	if( err != noErr )   _ACError("",this);
	*/
}

//#282
/**
Control�T�C�Y�ݒ�
*/
void	CUserPane::SetControlSize( const ANumber inWidth, const ANumber inHeight )
{
	GetWin().SetControlSize(GetControlID(),inWidth,inHeight);
}

//#282
/**
Control�ʒu�ݒ�
*/
void	CUserPane::SetControlPosition( const AWindowPoint& inWindowPoint )
{
	GetWin().SetControlPosition(GetControlID(),inWindowPoint);
}

//#455
/**
Binding�ݒ�
*/
void	CUserPane::SetControlBindings( 
		const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
		const ABool inWidthFixed, const ABool inHeightFixed )
{
	GetWin().SetControlBindings(GetControlID(),inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
}

/**
�L�[�t�H�[�J�X�ۂ�ݒ�
*/
void	CUserPane::SetAcceptFocus( const ABool inAcceptFocus )
{
	//view���擾
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	if( view == nil )   { _ACError("",NULL); return; }
	//�t�H�[�J�X�ۂ�ݒ�
	[view setAcceptFocus:inAcceptFocus];
}

//#1090
/**
view���s�������ǂ�����ݒ�
*/
void	CUserPane::SetOpaque( const ABool inOpaque )
{
	//view���擾
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	if( view == nil )   { _ACError("",NULL); return; }
	//�s�������ǂ�����ݒ�
	[view setOpaque:inOpaque];
}

/*#688
#pragma mark ===========================

#pragma mark --- �ύX�ʒm
*/
/**
�E�C���h�E�֕ύX��ʒm����
*/
/*#688
void	CUserPane::TellChangedToWindow()
{
	GetWin().CB_UserPaneValueChanged(GetControlID());//#688GetWin().GetControlID(mControlRef));
}
 */

#pragma mark ===========================

#pragma mark --- InlineInput�p

//#688
/**
InputMethod�ɑI���ʒu���Z�b�g��ʒm����
*/
void	CUserPane::ResetSelectedRangeForTextInput()
{
	//view���擾
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	if( view == nil )   { _ACError("",NULL); return; }
	//�I���ʒu���Z�b�g
	[view resetSelectedRangeForTextInput];
}

#pragma mark ===========================

#pragma mark ---�`�摮���ݒ�

//�e�L�X�g�f�t�H���g�����ݒ�
/*win void	CUserPane::SetDefaultTextProperty( const AFont inFont, const AFloatNumber inFontSize, 
		const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing )
{
	//
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
void	CUserPane::UpdateFont()
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
�e�L�X�g�f�t�H���g�����ݒ�
*/
void	CUserPane::SetDefaultTextProperty( const AText& inFontName, const AFloatNumber inFontSize, 
		const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing, const AFloatNumber inAlpha )
{
	/*#1034
	//ATSUStyle�������Ȃ琶��
	ABool	atsuTextStyleCreated = false;
	if( mATSUTextStyle == NULL )
	{
		OSStatus err = ::ATSUCreateStyle(&mATSUTextStyle);
		if( err != noErr )   _ACErrorNum("ATSUCreateStyle() returned error: ",err,this);
		atsuTextStyleCreated = true;
	}
	*/
	//#852
	//mATSUTextStyleInited = true;
	
	/*#688
	//
	AFontNumber	fontnum = 0;//#450
	if( AFontWrapper::GetFontByName(inFontName,fontnum) == false )//#450
	{
	}
	*/
	//�ǂꂩ�̃p�����[�^���Ⴄ�Ƃ�����ATSUStyle�Đݒ� #450
	if( /*#1034atsuTextStyleCreated == true || */
		CocoaObjects->GetNormalFont() == NULL || CocoaObjects->GetBoldFont() == NULL || 
		CocoaObjects->GetItalicFont() == NULL || CocoaObjects->GetBoldItalicFont() == NULL ||
		mFontName.Compare(inFontName) == false || mFontSize != inFontSize || 
		AColorWrapper::CompareColor(mColor,inColor) == false || mStyle != inStyle || mAntiAliasing != inAntiAliasing ||//#450
		mTextAlpha != inAlpha )
	{
		mFontName.SetText(inFontName);//#688
		mFontSize = inFontSize;
		mColor = inColor;
		mStyle = inStyle;
		mAntiAliasing = inAntiAliasing;
		mTextAlpha = inAlpha;
		
		/*#1034
		//mATSUStyle�ݒ�
		CWindowImp::SetATSUStyle(mATSUTextStyle,mFontName,mFontSize,mAntiAliasing,mColor,mTextAlpha,mStyle);
		*/
		//�t�H���g�X�V #1034
		UpdateFont();
	}
	
	//#0
	UpdateMetrics();
}
/**
�e�L�X�g�f�t�H���g�����ݒ�
*/
void	CUserPane::SetDefaultTextProperty( const AColor& inColor, const ATextStyle inStyle, const AFloatNumber inAlpha )
{
	/*#1034
	//ATSUStyle�������Ȃ琶��
	ABool	atsuTextStyleCreated = false;
	if( mATSUTextStyle == NULL )
	{
		OSStatus err = ::ATSUCreateStyle(&mATSUTextStyle);
		if( err != noErr )   _ACErrorNum("ATSUCreateStyle() returned error: ",err,this);
		atsuTextStyleCreated = true;
	}
	//#852
	//mATSUTextStyleInited = true;
	*/
	//�ǂꂩ�̃p�����[�^���Ⴄ�Ƃ�����ATSUStyle�Đݒ� #450
	if( /*#1034 atsuTextStyleCreated == true ||*/ 
		CocoaObjects->GetNormalFont() == NULL || CocoaObjects->GetBoldFont() == NULL || 
		CocoaObjects->GetItalicFont() == NULL || CocoaObjects->GetBoldItalicFont() == NULL ||
		AColorWrapper::CompareColor(mColor,inColor) == false || mStyle != inStyle ||//#450
		inAlpha != mTextAlpha )
	{
		//
		mColor = inColor;
		mStyle = inStyle;
		mTextAlpha = inAlpha;
		
		/*#1034
		//mATSUStyle�ݒ�
		CWindowImp::SetATSUStyle(mATSUTextStyle,mFontName,mFontSize,mAntiAliasing,mColor,mTextAlpha,mStyle);
		*/
		//�t�H���g�X�V #1034
		UpdateFont();
	}
	
	//#0
	UpdateMetrics();
}

#if 0
//#852
/**
ATSUStyle�擾
*/
ATSUStyle	CUserPane::GetATSUTextStyle() const
{
	/*
	if( mATSUTextStyleInited == false )
	{
		//CWindowImp::SetATSUStyle(mATSUTextStyle,mFontName,mFontSize,mAntiAliasing,mColor,mStyle);
		mATSUTextStyle = CAppImp::CreateDefaultATSUStyle();
		mATSUTextStyleInited = true;
	}
	return mATSUTextStyle;
	*/
	if( mATSUTextStyle == NULL )
	{
		//���ݒ�Ȃ�f�t�H���gATSUStyle��Ԃ�
		return CAppImp::GetDefaultATSUStyle();
	}
	else
	{
		return mATSUTextStyle;
	}
}
#endif

#pragma mark ===========================

#pragma mark ---�`�惋�[�`��

/**
�e�L�X�g�\��
*/
void	CUserPane::DrawText( const ALocalRect& inDrawRect, const AText& inText, const AJustification inJustification )
{
	DrawText(inDrawRect,inDrawRect,inText,inJustification);
}

/**
�e�L�X�g�\��
*/
void	CUserPane::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AText& inText, const AJustification inJustification )
{
	OSStatus	err = noErr;
	
	//return DrawText(inDrawRect,inClipRect,inText,mColor,mStyle,inJustification);
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	/*#1034
	//B0000 �������@DrawTextData���쐬�����A�܂��A�f�t�H���g��mATSUStyle�݂̂��g�p��run style��ݒ肵�Ȃ�
	//TextLayout����
	AText	utf16text;
	utf16text.SetText(inText);
	utf16text.ConvertFromUTF8ToUTF16();
	ATSUTextLayout	textLayout = CreateTextLayout(utf16text,true);
	*/
	
	//�`��
	CTLineRef	line = CreateDefaultCTLine(inText);
	DrawTextCore(inDrawRect,inClipRect,inJustification,line,false,kIndex_Invalid,kIndex_Invalid,kColor_Black,0.8);
	
	/*#1034
	//TextLayout�폜
	err = ::ATSUDisposeTextLayout(textLayout);
	if( err != noErr )   _ACErrorNum("ATSUDisposeTextLayout() returned error: ",err,this);
	*/
	
	//CTLine��� #1034
	::CFRelease(line);
}

/**
�e�L�X�g�\��
*/
void	CUserPane::DrawText( const ALocalRect& inDrawRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification )
{
	DrawText(inDrawRect,inDrawRect,inText,inColor,inStyle,inJustification);
}

/**
�e�L�X�g�\��
*/
void	CUserPane::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
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
�e�L�X�g�\��
*/
void	CUserPane::DrawText( const ALocalRect& inDrawRect, CTextDrawData& inTextDrawData, const AJustification inJustification )
{
	DrawText(inDrawRect,inDrawRect,inTextDrawData,inJustification);
}

/**
�e�L�X�g�\��
*/
void	CUserPane::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, CTextDrawData& inTextDrawData, const AJustification inJustification )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	/* ��CoreText�ł͖�蔭�����Ȃ���������Ȃ��̂ŕۗ�
	//#572 1�s��10000�����𒴂���ꍇ�́A1000000�����܂ł݂̂�\������B #1045 1����100��
	//UTF16�ł͂Ȃ��e�L�X�g���ԈႦ��UTF-16�Ƃ��ĕ\�������1�s���ُ�ɒ����Ȃ�A�ُ�ɕ\�����Ԃ������邽�߁B
	if( inTextDrawData.UTF16DrawText.GetItemCount() > kLineTextDrawLengthMax )
	{
		//2000000�o�C�g�ɐ؂�l��
		AText	truncatedUTF16Text;
		inTextDrawData.UTF16DrawText.GetText(0,kLineTextDrawLengthMax,truncatedUTF16Text);
		
		//TextLayout����
		inTextDrawData.ClearTextLayout();
		inTextDrawData.SetTextLayout(CreateTextLayout(truncatedUTF16Text,false));
		
		//�`��
		DrawTextCore(inDrawRect,inClipRect,inJustification,
					 inTextDrawData.GetTextLayout(),
					 inTextDrawData.drawSelection,
					 inTextDrawData.selectionStart,inTextDrawData.selectionEnd,inTextDrawData.selectionColor,inTextDrawData.selectionAlpha);
		return;
	}
	*/
	
	//��font fallback
	
	//�`��
	DrawTextCore(inDrawRect,inClipRect,inJustification,GetCTLineFromTextDrawData(inTextDrawData),
				 inTextDrawData.drawSelection,
				 inTextDrawData.selectionStart,inTextDrawData.selectionEnd,inTextDrawData.selectionColor,inTextDrawData.selectionAlpha, inTextDrawData.selectionFrameAlpha,inTextDrawData.selectionBackgroundColor);//#1316
	
	//R0199�@�X�y���`�F�b�N
	if( mVerticalMode == false )//#558
	{
		for( AIndex i = 0; i < inTextDrawData.misspellStartArray.GetItemCount(); i++ )
		{
			ANumber	lineHeight,lineAscent;
			GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
			
			//R0243
			//ascent���C����1������Ԑ������̂��x�X�g�iascent���C����������ƁA�����Ƌ߂����āA�������ǂ߂Ȃ��B�j
			//1������2 pixel�̐��������̂ŁAascent����3pt�K�v
			//�����AlineHeight��lineAscent+3��菬�����ꍇ�́Aascent���C������������Ƃɂ���B
			ANumber	yoffset = 1;
			if( lineHeight < lineAscent+3 )
			{
				yoffset = 0;
			}
			
			ALocalPoint	spt, ept;
			spt.x = inDrawRect.left;
			spt.y = inDrawRect.top+lineAscent+yoffset;//R0243 +1;//+2;
			ept = spt;
			CGFloat	offset2 = 0;
			CGFloat	offset = ::CTLineGetOffsetForStringIndex(GetCTLineFromTextDrawData(inTextDrawData),inTextDrawData.misspellStartArray.Get(i),&offset2);
			spt.x += offset;
			offset = ::CTLineGetOffsetForStringIndex(GetCTLineFromTextDrawData(inTextDrawData),inTextDrawData.misspellEndArray.Get(i),&offset2);
			ept.x += offset;
			DrawLine(spt,ept,kColor_Red,0.8,3.0,2.0);//R0199 2pt���̐��ɂ���
		}
	}
}

//#1034
/**
�f�t�H���g�t�H���g�ł�CTLine����
*/
CTLineRef	CUserPane::CreateDefaultCTLine( const AText& inText ) 
{
	//�t�H���g���ݒ�Ȃ�ݒ肷��
	if( CocoaObjects->GetNormalFont() == NULL || CocoaObjects->GetBoldFont() == NULL || 
		CocoaObjects->GetItalicFont() == NULL || CocoaObjects->GetBoldItalicFont() == NULL )
	{
		UpdateFont();
	}
	
	//�\���e�L�X�g�擾
	AStCreateNSStringFromAText	str(inText);
	//�\���e�L�X�g��NSMutableAttributedString�擾
	NSMutableAttributedString *attrString = [[NSMutableAttributedString alloc] initWithString:str.GetNSString()];
	//attrString�̓C�x���g���[�v�I���ŉ���BCTLine�ŃR�s�[���Ă��邩�Q�Ƃ��Ă��邩�͕s�������A�Q�Ƃ��Ă���Ȃ�retain()���Ă���͂��B
	[attrString autorelease];
	//NSMutableAttributedString�̕ҏW�J�n
	[attrString beginEditing];
	
	// ==================== �e�L�X�g�����ݒ� ====================
	//range�ݒ�
	NSRange	range = NSMakeRange(0,[attrString length]);
	//�t�H���g�ݒ�
	if( (mStyle&kTextStyle_Bold) == 0 )
	{
		if( (mStyle&kTextStyle_Italic) == 0 )
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
		if( (mStyle&kTextStyle_Italic) == 0 )
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
	//#558
	//�c���������ݒ�
	if( mVerticalMode == true )
	{
		AddVerticalAttribute(attrString,str.GetNSString());
	}
	
	//�A���_�[���C��
	if( (mStyle&kTextStyle_Underline) != 0 )
	{
		[attrString addAttribute:NSUnderlineStyleAttributeName 
		value:[NSNumber numberWithInteger:NSUnderlineStyleSingle] range:range];
	}
	
	// �����F�ݒ�
	[attrString addAttribute:NSForegroundColorAttributeName 
	value:[NSColor colorWithCalibratedRed:(mColor.red/65535.0) green:(mColor.green/65535.0) 
	blue:(mColor.blue/65535.0) alpha:mTextAlpha] range:range];
	
	//NSMutableAttributedString�̕ҏW�I��
	[attrString endEditing];
	
	//CTLine����
	return ::CTLineCreateWithAttributedString((CFAttributedStringRef)attrString);
}

//#1034
/**
TextDrawData����CTLine�擾�i�������Ȃ琶���j
*/
CTLineRef	CUserPane::GetCTLineFromTextDrawData( CTextDrawData& inTextDrawData )
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
			
			//�h���b�v�V���h�E
			if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_DropShadow) != 0 )
			{
				NSShadow *shadow = [[[NSShadow alloc] init] autorelease];
				[shadow setShadowColor:[NSColor colorWithCalibratedRed:(mDropShadowColor.red/65535.0) 
				green:(mDropShadowColor.green/65535.0) 
				blue:(mDropShadowColor.blue/65535.0) 
				alpha:1.0]];
				[shadow setShadowBlurRadius:1.0];
				[shadow setShadowOffset:NSMakeSize(0.0f, -1.01f)];
				[attrString addAttribute:NSShadowAttributeName value:shadow range:range];
			}
			
			// �����F�ݒ�
			RGBColor	c = inTextDrawData.attrColor.Get(i);
			[attrString addAttribute:NSForegroundColorAttributeName 
			value:[NSColor colorWithCalibratedRed:(c.red/65535.0) green:(c.green/65535.0) blue:(c.blue/65535.0) alpha:mTextAlpha] range:range];
			
		}
		
		//#1316
		// ==================== �e�L�X�g����(RunStyle)�ݒ�@�ǉ��� ====================
		//inTextDrawData�ɋL�ڂ��ꂽ������ݒ�
		for( AIndex i = 0; i < inTextDrawData.additionalAttrPos.GetItemCount(); i++ )
		{
			// -------------------- Range�擾 --------------------
			//len�ɁA��������RunLength�́AUniChar�ł̒������i�[����B
            AItemCount	len = inTextDrawData.additionalAttrLength.Get(i);
			//RunLength�̒�����0�Ȃ牽�����Ȃ�
			if( len <= 0 )   continue;
			//range�ݒ�
			NSRange	range = NSMakeRange(inTextDrawData.additionalAttrPos.Get(i),len);
			
			// -------------------- �����ݒ� --------------------
			
			//�t�H���g�ݒ�
			if( (inTextDrawData.additionalAttrStyle.Get(i)&kTextStyle_Bold) == 0 )
			{
				if( (inTextDrawData.additionalAttrStyle.Get(i)&kTextStyle_Italic) == 0 )
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
				if( (inTextDrawData.additionalAttrStyle.Get(i)&kTextStyle_Italic) == 0 )
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
			if( (inTextDrawData.additionalAttrStyle.Get(i)&kTextStyle_Underline) != 0 )
			{
				[attrString addAttribute:NSUnderlineStyleAttributeName 
				value:[NSNumber numberWithInteger:NSUnderlineStyleSingle] range:range];
			}
			
			//�h���b�v�V���h�E
			if( (inTextDrawData.additionalAttrStyle.Get(i)&kTextStyle_DropShadow) != 0 )
			{
				NSShadow *shadow = [[[NSShadow alloc] init] autorelease];
				[shadow setShadowColor:[NSColor colorWithCalibratedRed:(mDropShadowColor.red/65535.0) 
						green:(mDropShadowColor.green/65535.0) 
						blue:(mDropShadowColor.blue/65535.0) 
				alpha:1.0]];
				[shadow setShadowBlurRadius:1.0];
				[shadow setShadowOffset:NSMakeSize(0.0f, -1.01f)];
				[attrString addAttribute:NSShadowAttributeName value:shadow range:range];
			}
			
			// �����F�ݒ�
			RGBColor	c = inTextDrawData.additionalAttrColor.Get(i);
			[attrString addAttribute:NSForegroundColorAttributeName 
			value:[NSColor colorWithCalibratedRed:(c.red/65535.0) green:(c.green/65535.0) blue:(c.blue/65535.0) alpha:mTextAlpha] range:range];
			
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

//#558
/**
�c���������ݒ�
*/
void	CUserPane::AddVerticalAttribute( NSMutableAttributedString *inAttrString, NSString* inString )
{
	//NSVerticalGlyphFormAttributeName��ݒ�
	NSRange	range = {0,[inString length]};
	[inAttrString addAttribute:NSVerticalGlyphFormAttributeName value:[NSNumber numberWithInt:1] range:range];
	
	/*#558 U+0400�ȏ�̏ꍇ�̂ݏc���������ɂ���e�X�g
	���S�̂ɏc�����ݒ肵�Ă��t�H���g����ł́A�A���t�@�x�b�g���̂݉������\���ɂȂ��Ă����̂ŁA�S�̂ɏc�����ݒ肷�邱�Ƃɂ���
	ABool	verticalCharMode = false;
	AItemCount	count = [inString length];
	NSRange	range = {0,0};
	for( AIndex i = 0; i < count; i++ )
	{
		unichar	ch = [inString characterAtIndex:i];
		if( ch >= 0x400 )
		{
			if( verticalCharMode == false )
			{
				range.location = i;
			}
			verticalCharMode = true;
		}
		else
		{
			if( verticalCharMode == true )
			{
				range.length = i - range.location;
				//
				[inAttrString addAttribute:NSVerticalGlyphFormAttributeName value:[NSNumber numberWithInt:1] range:range];
			}
			verticalCharMode = false;
		}
	}
	if( verticalCharMode == true )
	{
		range.length = count - range.location;
		[inAttrString addAttribute:NSVerticalGlyphFormAttributeName value:[NSNumber numberWithInt:1] range:range];
	}
	*/
}

#if 0
/**
�e�L�X�g�\��
*/
void	CUserPane::DrawTextCore( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AJustification inJustification,
								const ATSUTextLayout inTextLayout, const ABool inDrawSelection, 
								const AIndex inSelectionStart, const AIndex inSelectionEnd, const AColor inSelectionColor, const AFloatNumber inSelectionAlpha )
{
	OSStatus	status = noErr;
	
	//lineAscent�́AATSUGetUnjustifiedBounds()�Ŏ擾����ƁA�\�����镶���ɂ����ascent���ω�����̂ŁA���L�Ŏ擾�B
	ANumber	lineHeight,lineAscent;
	GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
	
	//
	ANumber	x = inDrawRect.left;
	ANumber	y = inDrawRect.top;//#450
	if( inJustification != kJustification_Left )
	{
		ATSUTextMeasurement	start, end, ascent, descent;
		status = ::ATSUGetUnjustifiedBounds(inTextLayout,kATSUFromTextBeginning,kATSUToTextEnd,&start,&end,&ascent,&descent);
		//ANumber	textWidth = ConvertFixedToInt(end-start);
		{
			switch(inJustification)
			{
			  case kJustification_Right:
				{
					x = inDrawRect.right - ConvertFixedToInt(end-start);
					break;
				}
			  case kJustification_RightCenter://#450
				{
					x = inDrawRect.right - ConvertFixedToInt(end-start);
					y = inDrawRect.top + (inDrawRect.bottom-inDrawRect.top)/2 - lineHeight/2;
					break;
				}
			  case kJustification_Center:
				{
					x = (inDrawRect.left+inDrawRect.right)/2 - ConvertFixedToInt(end-start)/2;
					break;
				}
			  case kJustification_LeftTruncated://#315
				{
					if( ConvertFixedToInt(end-start) > inDrawRect.right-inDrawRect.left )
					{
						x = inDrawRect.right - ConvertFixedToInt(end-start);
					}
					break;
				}
			  case kJustification_CenterTruncated://#315
				{
					if( ConvertFixedToInt(end-start) > inDrawRect.right-inDrawRect.left )
					{
						x = inDrawRect.right - ConvertFixedToInt(end-start);
					}
					else
					{
						x = (inDrawRect.left+inDrawRect.right)/2 - ConvertFixedToInt(end-start)/2;
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
	
	//Clip
	::CGContextBeginPath(mContextRef);
	::CGContextAddRect(mContextRef,GetCGRectFromARect(inClipRect));
	::CGContextClosePath(mContextRef);
	::CGContextClip(mContextRef);
	
	//Selection�`��
	if( inDrawSelection == true )
	{
		ALocalRect	rect = inDrawRect;
		Boolean	caretIsSplit;
		ATSUCaret	mainCaret, secondCaret;
		if( inSelectionStart != kIndex_Invalid )
		{
			status = ::ATSUOffsetToPosition(inTextLayout,inSelectionStart,true,&mainCaret,&secondCaret,&caretIsSplit);
			rect.left = inDrawRect.left + ::Fix2Long(mainCaret.fX);
		}
		if( inSelectionEnd != kIndex_Invalid )
		{
			status = ::ATSUOffsetToPosition(inTextLayout,inSelectionEnd,true,&mainCaret,&secondCaret,&caretIsSplit);
			rect.right = inDrawRect.left + ::Fix2Long(mainCaret.fX);
		}
		PaintRect(rect,inSelectionColor,inSelectionAlpha);
	}
	
	//Text�`��
	
	status = ::ATSUDrawText(inTextLayout,kATSUFromTextBeginning, kATSUToTextEnd,
							ConvertIntToFixed(x), -(ConvertIntToFixed(y)+ConvertIntToFixed(lineAscent)) );
	
	::CGContextRestoreGState(mContextRef);
	/*test
	::CGContextScaleCTM(mContextRef, 1.0, 1.0);
	AStCreateNSStringFromAText	str(inText);
	NSPoint	pt = {0};
	pt.x = x;
	pt.y = y;// + lineAscent;
	[str.GetNSString() drawAtPoint:pt withAttributes:nil];
	::CGContextScaleCTM(mContextRef, 1.0, -1.0);
	*/
	//TextLayout�폜
	//������::ATSUDisposeTextLayout(textLayout);
	//textLayout��CTextDrawData�̃f�X�g���N�^�Ŕj������
	
	//UnitTest�p
	mDrawTextCount++;
	
	//B0000 ������ return textWidth;
}
#endif

//#1034
/**
�e�L�X�g�\��
*/
void	CUserPane::DrawTextCore( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AJustification inJustification,
								 const CTLineRef inCTLineRef, const ABool inDrawSelection, 
								 const AIndex inSelectionStart, const AIndex inSelectionEnd, const AColor inSelectionColor, const AFloatNumber inSelectionAlpha, const AFloatNumber inSelectionFrameAlpha, const AColor inSelectionBackgroundColor )//#1316
{
	OSStatus	status = noErr;
	
	//lineAscent�́AATSUGetUnjustifiedBounds()�Ŏ擾����ƁA�\�����镶���ɂ����ascent���ω�����̂ŁA���L�Ŏ擾�B
	AFloatNumber	lineHeight,lineAscent;//#1316 ANumber��AFloatNumber
	GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
	
	//
	AFloatNumber	x = inDrawRect.left;//#1316 ANumber��AFloatNumber
	AFloatNumber	y = inDrawRect.top;//#450 #1316 ANumber��AFloatNumber
	if( inJustification != kJustification_Left )
	{
		CGFloat	ascent = 9, descent = 3, leading = 3;
		double	width = ::CTLineGetTypographicBounds(inCTLineRef,&ascent,&descent,&leading);
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
	
	//Clip
	::CGContextBeginPath(mContextRef);
	::CGContextAddRect(mContextRef,GetCGRectFromARect(inClipRect));
	::CGContextClosePath(mContextRef);
	::CGContextClip(mContextRef);
	
	//Selection�`��
	if( inDrawSelection == true )
	{
		ALocalRect	rect = inDrawRect;
		if( inSelectionStart != kIndex_Invalid )
		{
			CGFloat	offset2 = 0;
			CGFloat	offset = ::CTLineGetOffsetForStringIndex(inCTLineRef,inSelectionStart,&offset2);
			rect.left = inDrawRect.left + offset;
		}
		if( inSelectionEnd != kIndex_Invalid )
		{
			CGFloat	offset2 = 0;
			CGFloat	offset = ::CTLineGetOffsetForStringIndex(inCTLineRef,inSelectionEnd,&offset2);
			rect.right = inDrawRect.left + offset;
		}
		if( inSelectionAlpha > 0.0 )
		{
			//�I��F
			PaintRect(rect,inSelectionColor,inSelectionAlpha);
		}
		if( inSelectionFrameAlpha > 0.0 )
		{
			//���łɕ`�悳��Ă��铧���F�Əd�Ȃ�ƕ����������Â炭�Ȃ�̂ŁA��U�w�i�F�ŕs�����y�C���g����B
			PaintRect(rect,inSelectionBackgroundColor,1.0);
			//�I��F
			PaintRect(rect,inSelectionColor,inSelectionAlpha);
			//�t���[��
			FrameRect(rect,inSelectionColor,inSelectionFrameAlpha,true,true,true,true,1.0);
		}
	}
	
	//Text�`��
	::CGContextSetTextMatrix(mContextRef, CGAffineTransformIdentity);
	AFloatNumber	texty = y+lineAscent;//#1316 ANumber��AFloatNumber
	if( mVerticalMode == true )
	{
		//�c�����̏ꍇ���傢���i���j�ɂȂ�悤�ɂ���i�Ƃ肠�������s����ɂ�钲���E�E�E�j #558
		texty = y+lineAscent + (lineHeight-lineAscent)/4;
	}
	::CGContextSetTextPosition(mContextRef,x,-(texty));
	::CTLineDraw(inCTLineRef,mContextRef);
	
	::CGContextRestoreGState(mContextRef);
	
	//CTLineRef��CTextDrawData�̃f�X�g���N�^�Ŕj������
	
	//UnitTest�p
	mDrawTextCount++;
}

#if 0
/**
TextLayout����
*/
ATSUTextLayout	CUserPane::CreateTextLayout( const AText& inUTF16Text, const ABool inUseFontFallback ) const //#703
{
	return CWindowImp::CreateTextLayout(/*#852 mATSUTextStyle*/GetATSUTextStyle(),inUTF16Text,mAntiAliasing,inUseFontFallback,mContextRef);
}
#endif

//#1012
#if 0 
/**
Icon�`��
*/
void	CUserPane::DrawIcon( const ALocalRect& inRect, const AIconID inIconID, const ABool inEnabledColor,
		const ABool inDefaultSize ) const //win 080722
{
	OSStatus	err = noErr;
	
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	IconTransformType	type = kTransformNone;
	if( inEnabledColor == false )
	{
		type = kTransformDisabled;
	}
	AIconRef	iconRef = CWindowImp::GetIconRef(inIconID);
	if( iconRef != NULL )
	{
		CGRect	cgrect = GetCGRectFromARect(inRect);
		err = ::PlotIconRefInContext(mContextRef,&cgrect,/*kAlignAbsoluteCenter*/kAlignNone,type,/*#232 &kColor_White*/NULL,kPlotIconRefNormalFlags,iconRef);
		if( err != noErr )   _ACErrorNum("PlotIconRefInContext() returned error: ",err,this);
	}
	else
	{
		RgnHandle	svRgn;
		svRgn = ::NewRgn();
		::GetClip(svRgn);
		
		AWindowRect	localFrameWindowRect;
		GetControlWindowFrameRect(localFrameWindowRect);
		Rect	frameRect;
		frameRect.left		= localFrameWindowRect.left;
		frameRect.top		= localFrameWindowRect.top;
		frameRect.right		= localFrameWindowRect.right;
		frameRect.bottom	= localFrameWindowRect.bottom;
		::ClipRect(&frameRect);
		
		//
		CIconHandle	iconhandle = CWindowImp::GetCIconHandle(inIconID);
		AWindowRect	windowRect;
		GetWinConst().GetWindowRectFromControlLocalRect(GetControlID(),inRect,windowRect);
		Rect	rect;
		rect.left 		= ((windowRect.left+windowRect.right)/2)-16;
		rect.right		= ((windowRect.left+windowRect.right)/2)+16;
		rect.top		= ((windowRect.top+windowRect.bottom)/2)-16;
		rect.bottom		= ((windowRect.top+windowRect.bottom)/2)+16;
		err = ::PlotCIconHandle(&rect,kAlignAbsoluteCenter,type,iconhandle);
		if( err != noErr )   _ACErrorNum("PlotCIconHandle() returned error: ",err,this);
		
		//Clip����
		::SetClip(svRgn);
		::DisposeRgn(svRgn);
	}
}
#endif

/**
Icon�`��i�t�@�C������j
*/
void	CUserPane::DrawIconFromFile( const ALocalRect& inRect, const AFileAcc& inFile, const ABool inEnabledColor ) const
{
	OSStatus	err = noErr;
	
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	/*#1012
	FSSpec	fsspec;
	inFile.GetFSSpec(fsspec);
	IconRef	iconRef;
	SInt16	label;
	if( ::GetIconRefFromFile(&fsspec,&iconRef,&label) == noErr )
	{
		CGRect	cgrect = GetCGRectFromARect(inRect);
		IconTransformType	type = kTransformNone;
		if( inEnabledColor == false )
		{
			type = kTransformDisabled;
		}
		err = ::PlotIconRefInContext(mContextRef,&cgrect,kAlignAbsoluteCenter,type,&kColor_White,kPlotIconRefNormalFlags,iconRef);
	}
	*/
	//�t�@�C���p�X�擾
	AText	path;
	inFile.GetPath(path);
	AStCreateNSStringFromAText	str(path);
	//�t�@�C���A�C�R����image�擾
	NSImage*	image = [[NSWorkspace sharedWorkspace] iconForFile:str.GetNSString()];
	//�`���T�C�Y�擾
	ANumber	width = inRect.right-inRect.left;
	ANumber	height = inRect.bottom-inRect.top;
	//�`���rect�擾
	NSRect	drawrect = {0};
	drawrect.origin.x = inRect.left;
	drawrect.origin.y = -(inRect.top+height);
	drawrect.size.width 	= width;
	drawrect.size.height 	= height;
	//�C���[�Wrect
	NSRect	srcrect = {0};
	srcrect.origin.x = 0;
	srcrect.origin.y = 0;
	srcrect.size.width 		= [image size].width;
	srcrect.size.height 	= [image size].height;
	//�C���[�W�`��
	[image drawInRect:drawrect fromRect:srcrect operation:NSCompositeSourceOver fraction:1.0];
}

/**
���p�`�`��
*/
void	CUserPane::PaintPoly( const AArray<ALocalPoint>& inPath, const AColor& inColor, const AFloatNumber inAlpha )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextSetRGBFillColor(mContextRef,red,green,blue,inAlpha);
	::CGContextBeginPath(mContextRef);
	::CGContextMoveToPoint(mContextRef,inPath.Get(inPath.GetItemCount()-1).x,-inPath.Get(inPath.GetItemCount()-1).y-1);
	for( AIndex i = 0; i < inPath.GetItemCount(); i++ )
	{
		::CGContextAddLineToPoint(mContextRef,inPath.Get(i).x,-inPath.Get(i).y-1);
	}
	::CGContextSetShouldAntialias(mContextRef,true);
	::CGContextFillPath(mContextRef);
	::CGContextRestoreGState(mContextRef);
}

/**
���p�`Frame�`��
*/
void	CUserPane::FramePoly( const AArray<ALocalPoint>& inPath, const AColor& inColor, const AFloatNumber inAlpha )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	::CGContextBeginPath(mContextRef);
	::CGContextMoveToPoint(mContextRef,inPath.Get(inPath.GetItemCount()-1).x,-inPath.Get(inPath.GetItemCount()-1).y-1);
	for( AIndex i = 0; i < inPath.GetItemCount(); i++ )
	{
		::CGContextAddLineToPoint(mContextRef,inPath.Get(i).x,-inPath.Get(i).y-1);
	}
	::CGContextSetShouldAntialias(mContextRef,true);
	::CGContextStrokePath(mContextRef);
	::CGContextRestoreGState(mContextRef);
}

//#724
/**
PaintRoundedRect��paint
*/
void	CUserPane::PaintRoundedRect( const ALocalRect& inRect, 
									const AColor& inStartColor, const AColor& inEndColor, const AGradientType inGradientType, 
									const AFloatNumber inStartAlpha, const AFloatNumber inEndAlpha,
									const ANumber inR, 
									const ABool inLeftTopSide, const ABool inRightTopSide, 
									const ABool inLeftBottomSide, const ABool inRightBottomSide )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	//�F�ݒ�
	float	red = inStartColor.red;
	float	green = inStartColor.green;
	float	blue = inStartColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextSetRGBFillColor(mContextRef,red,green,blue,(inStartAlpha+inEndAlpha)/2.0);
	//�p�X�J�n
	::CGContextBeginPath(mContextRef);
	//
	CGFloat	left = inRect.left;
	CGFloat	right = inRect.right;
	CGFloat	top = -inRect.top;
	CGFloat	bottom = -inRect.bottom;
	CGFloat	rad = (top-bottom)/2;
	if( inR < rad )
	{
		rad = inR;
	}
	CGFloat	x1 = left + rad;
	CGFloat	x2 = right - rad;
	CGFloat	y1 = top - rad;
	CGFloat y2 = bottom + rad;
	//��̐�
	::CGContextMoveToPoint(mContextRef,x2,top);
	::CGContextAddLineToPoint(mContextRef,x1,top);
	//����
	if( inLeftTopSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,left,top,left,y1,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,left,top);
	}
	//���̐�
	::CGContextAddLineToPoint(mContextRef,left,y2);
	//����
	if( inLeftBottomSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,left,bottom,x1,bottom,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,left,bottom);
	}
	//���̐�
	::CGContextAddLineToPoint(mContextRef,x2,bottom);
	//�E��
	if( inRightBottomSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,right,bottom,right,y2,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,right,bottom);
	}
	//�E�̐�
	::CGContextAddLineToPoint(mContextRef,right,y1);
	//�E��
	if( inRightTopSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,right,top,x2,top,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,right,top);
		::CGContextAddLineToPoint(mContextRef,x2,top);
	}
	//�p�X�I��
	::CGContextClosePath(mContextRef);
	//�A���`�G�C���A�XON
	::CGContextSetShouldAntialias(mContextRef,true);
	//gradient�L��
	if( inGradientType == kGradientType_None || AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_5 ||
		(inStartColor.red == inEndColor.red && inStartColor.green == inEndColor.green && inStartColor.blue == inEndColor.blue &&
		 inStartAlpha == inEndAlpha ) ) //#1275 start��end�̐F�E�A���t�@�������ꍇ��gradient�Ȃ��Ƃ��Ĉ���
	{
		//gradient����
		
		::CGContextFillPath(mContextRef);
	}
	else
	{
		//gradient�L��
		//CGContextDrawLinearGradient()��10.5�����͖��Ή�
		
		::CGContextClip(mContextRef);
		//gradient����
		CGColorRef	topColor = CreateCGColor(inStartColor,inStartAlpha);
		CGColorRef	bottomColor = CreateCGColor(inEndColor,inEndAlpha);
		const void *colorRefs[2] = {topColor,bottomColor};
		CFArrayRef colorArray = ::CFArrayCreate(kCFAllocatorDefault, colorRefs, 2, &kCFTypeArrayCallBacks);
		CGFloat locations[2] = {0.0,1.0};
		CGGradientRef gradient = ::CGGradientCreateWithColors(NULL, colorArray, locations);
		//gradient�`��
		CGPoint	spt = {inRect.left,-inRect.top};
		CGPoint	ept = {inRect.right,-inRect.top};
		if( inGradientType == kGradientType_Vertical )
		{
			spt.x = inRect.left;
			spt.y = -inRect.top;
			ept.x = inRect.left;
			ept.y = -inRect.bottom;
		}
		::CGContextDrawLinearGradient(mContextRef,gradient,spt,ept,0);
		//gradient���
		::CGGradientRelease(gradient);
		::CFRelease(colorArray);
		::CFRelease(bottomColor);
		::CFRelease(topColor);
	}
	//GState���X�g�A
	::CGContextRestoreGState(mContextRef);
}

//#724
/**
PaintRoundedRect��frame
*/
void	CUserPane::FrameRoundedRect( const ALocalRect& inRect, 
									const AColor& inColor, const AFloatNumber inAlpha,
									const ANumber inR, 
									const ABool inLeftTopSide, const ABool inRightTopSide, 
									const ABool inLeftBottomSide, const ABool inRightBottomSide,
									const ABool inDrawLeftLine, const ABool inDrawTopLine,
									const ABool inDrawRightLine, const ABool inDrawBottomLine,
									const AFloatNumber inLineWidth )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	//�F�ݒ�
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	/*test
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		CGSize	offset = {1.5,-1.5};
		CGColorRef	color = CreateCGColor(mDropShadowColor,1.0);//
		::CGContextSetShadowWithColor(mContextRef,offset,1.0,color);
		::CFRelease(color);
	}
	*/
	//�p�X�J�n
	::CGContextBeginPath(mContextRef);
	//
	ALocalRect	rect = inRect;
	/*test
	rect.top		+= inLineWidth;
	rect.left		+= inLineWidth;
	rect.bottom		-= inLineWidth;
	rect.right		-= inLineWidth;
	*/
	//
	CGFloat	left = rect.left;
	CGFloat	right = rect.right;
	CGFloat	top = -rect.top;
	CGFloat	bottom = -rect.bottom;
	CGFloat	rad = (top-bottom)/2;
	if( inR < rad )
	{
		rad = inR;
	}
	CGFloat	x1 = left + rad;
	CGFloat	x2 = right - rad;
	CGFloat	y1 = top - rad;
	CGFloat y2 = bottom + rad;
	//��̐�
	if( inDrawTopLine == true )
	{
		::CGContextMoveToPoint(mContextRef,x2,top);
		::CGContextAddLineToPoint(mContextRef,x1,top);
	}
	else
	{
		::CGContextMoveToPoint(mContextRef,x1,top);
	}
	//����
	if( inLeftTopSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,left,top,left,y1,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,left,top);
	}
	//���̐�
	if( inDrawLeftLine == true )
	{
		::CGContextAddLineToPoint(mContextRef,left,y2);
	}
	else
	{
		::CGContextMoveToPoint(mContextRef,left,y2);
	}
	//����
	if( inLeftBottomSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,left,bottom,x1,bottom,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,left,bottom);
	}
	//���̐�
	if( inDrawBottomLine == true )
	{
		::CGContextAddLineToPoint(mContextRef,x2,bottom);
	}
	else
	{
		::CGContextMoveToPoint(mContextRef,x2,bottom);
	}
	//�E��
	if( inRightBottomSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,right,bottom,right,y2,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,right,bottom);
	}
	//�E�̐�
	if( inDrawRightLine == true )
	{
		::CGContextAddLineToPoint(mContextRef,right,y1);
	}
	else
	{
		::CGContextMoveToPoint(mContextRef,right,y1);
	}
	//�E��
	if( inRightTopSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,right,top,x2,top,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,right,top);
		::CGContextAddLineToPoint(mContextRef,x2,top);
	}
	//�p�X�I��
	//::CGContextClosePath(mContextRef);
	//�A���`�G�C���A�XON
	::CGContextSetShouldAntialias(mContextRef,true);
	//����0.2
	::CGContextSetLineWidth(mContextRef,inLineWidth);
	//�`��
	::CGContextStrokePath(mContextRef);
	//GState���X�g�A
	::CGContextRestoreGState(mContextRef);
}

/**
*/
void	CUserPane::EraseRect( const ALocalRect& inRect )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	/*frame view���������ɂȂ��Ă��܂�
	::CGContextSaveGState(mContextRef);
	::CGContextClearRect(mContextRef,GetCGRectFromARect(inRect));
	::CGContextRestoreGState(mContextRef);
	*/
	//�Ƃ肠�������������Ƃ���
}

/**
Rect�h��Ԃ�
*/
void	CUserPane::PaintRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha,
		const ABool inDropShadow )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	if( inDropShadow == true && AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		CGSize	offset = {1.5,-1.5};
		CGColorRef	color = CreateCGColor(mDropShadowColor,1.0);//
		::CGContextSetShadowWithColor(mContextRef,offset,1.0,color);
		::CFRelease(color);
	}
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextSetRGBFillColor(mContextRef,red,green,blue,inAlpha);
	//#1275 NSBezierPath�̕`��p�����[�^�̕���������������Ȃ��̂�NSBezierPath���g�����Ƃɂ��� ::CGContextFillRect(mContextRef,GetCGRectFromARect(inRect));
	::CGContextSetShouldAntialias(mContextRef,false);
	[NSBezierPath fillRect:NSRectFromCGRect(GetCGRectFromARect(inRect))];//#1275
	::CGContextRestoreGState(mContextRef);
}

//#634
/**
���������O���f�[�V������PaintRect
*/
void	CUserPane::PaintRectWithVerticalGradient( const ALocalRect& inRect, 
		const AColor& inTopColor, const AColor& inBottomColor, 
		const AFloatNumber inTopAlpha, const AFloatNumber inBottomAlpha )
{
	/*
	if( inDrawUsingLines == true )
	{
		//���𕡐��������ƂŎ�������
		//OS X 10.5�����ł��g�p�\�B
		//�������s���������Ə������Ԃ�������
		AFloatNumber	percent = 0;
		AFloatNumber	h = (inRect.bottom-inRect.top-1);
		AFloatNumber	percentInc = 100.0/h;
		for( ANumber y = inRect.top; y < inRect.bottom; y++ )
		{
			AColor	color = AColorWrapper::GetGradientColor(inTopColor,inBottomColor,percent);
			ALocalPoint	spt = {inRect.left,y};
			ALocalPoint	ept = {inRect.right-1,y};
			DrawLine(spt,ept,color,inAlpha);
			percent += percentInc;
		}
	}
	else
	{
	*/
	if( AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_5 )
	{
		//CGContextDrawLinearGradient()��10.5�����͖��Ή��Ȃ̂�50%��Paint����
		AColor	color = AColorWrapper::GetGradientColor(inTopColor,inBottomColor,50.0);
		PaintRect(inRect,color,(inTopAlpha+inBottomAlpha)/2.0);
	}
	else
	{
		if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
		
		//GState�ۑ�
		::CGContextSaveGState(mContextRef);
		//Rect�p�X
		::CGContextBeginPath(mContextRef);
		::CGContextAddRect(mContextRef,GetCGRectFromARect(inRect));
		::CGContextClip(mContextRef);
		//gradient����
		CGColorRef	topColor = CreateCGColor(inTopColor,inTopAlpha);
		CGColorRef	bottomColor = CreateCGColor(inBottomColor,inBottomAlpha);
		const void *colorRefs[2] = {topColor,bottomColor};
		CFArrayRef colorArray = ::CFArrayCreate(kCFAllocatorDefault, colorRefs, 2, &kCFTypeArrayCallBacks);
		CGFloat locations[2] = {0.0,1.0};
		CGGradientRef gradient = ::CGGradientCreateWithColors(NULL, colorArray, locations);
		//gradient�`��
		CGPoint	spt = {inRect.left,-inRect.top};
		CGPoint	ept = {inRect.left,-inRect.bottom};
		::CGContextDrawLinearGradient(mContextRef,gradient,spt,ept,0);
		//gradient���
		::CGGradientRelease(gradient);
		::CFRelease(colorArray);
		::CFRelease(bottomColor);
		::CFRelease(topColor);
		//GState���X�g�A
		::CGContextRestoreGState(mContextRef);
	}
	//}
}

//#634
/**
���������O���f�[�V������PaintRect
*/
void	CUserPane::PaintRectWithHorizontalGradient( const ALocalRect& inRect, 
		const AColor& inLeftColor, const AColor& inRightColor, const AFloatNumber inLeftAlpha, const AFloatNumber inRightAlpha )
{
	if( AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_5 )
	{
		//CGContextDrawLinearGradient()��10.5�����͖��Ή��Ȃ̂�50%��Paint����
		AColor	color = AColorWrapper::GetGradientColor(inLeftColor,inRightColor,50.0);
		PaintRect(inRect,color,(inLeftAlpha+inRightAlpha)/2.0);
	}
	else
	{
		if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
		
		//GState�ۑ�
		::CGContextSaveGState(mContextRef);
		//Rect�p�X
		::CGContextBeginPath(mContextRef);
		::CGContextAddRect(mContextRef,GetCGRectFromARect(inRect));
		::CGContextClip(mContextRef);
		//gradient����
		CGColorRef	topColor = CreateCGColor(inLeftColor,inLeftAlpha);
		CGColorRef	bottomColor = CreateCGColor(inRightColor,inRightAlpha);
		const void *colorRefs[2] = {topColor,bottomColor};
		CFArrayRef colorArray = ::CFArrayCreate(kCFAllocatorDefault, colorRefs, 2, &kCFTypeArrayCallBacks);
		CGFloat locations[2] = {0.0,1.0};
		CGGradientRef gradient = ::CGGradientCreateWithColors(NULL, colorArray, locations);
		//gradient�`��
		CGPoint	spt = {inRect.left,-inRect.top};
		CGPoint	ept = {inRect.right,-inRect.top};
		::CGContextDrawLinearGradient(mContextRef,gradient,spt,ept,0);
		//gradient���
		::CGGradientRelease(gradient);
		::CFRelease(colorArray);
		::CFRelease(bottomColor);
		::CFRelease(topColor);
		//GState���X�g�A
		::CGContextRestoreGState(mContextRef);
	}
}

//#634
/**
AColor����CGColor�擾
CGColor�̓R�[�����ŉ���K�v
@note Mac OS X 10.5�ȏ�̂ݎg�p�\
*/
CGColorRef	CUserPane::CreateCGColor( const AColor inColor, const AFloatNumber inAlpha ) const
{
	CGFloat	red = inColor.red;
	CGFloat	green = inColor.green;
	CGFloat	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	return ::CGColorCreateGenericRGB(red,green,blue,inAlpha);
}

/**
Rect�̃t���[����`��
*/
void	CUserPane::FrameRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha,
							 const ABool inDrawLeftLine, const ABool inDrawTopLine,
							 const ABool inDrawRightLine, const ABool inDrawBottomLine,
							 const AFloatNumber inLineWidth )
{
	//Retina�ł̕\���c��𔭐������Ȃ����߁A���Ŏ�������̂ł͂Ȃ��Arect�Ŏ�������
	if( inDrawLeftLine == true )
	{
		ALocalRect	leftEdge = inRect;
		leftEdge.right = leftEdge.left+inLineWidth;
		if( inDrawTopLine == true )
		{
			leftEdge.top += inLineWidth;//#1316 �����F�ŕ`���Ǝl�����Z���Ȃ���΍�
		}
		if( inDrawBottomLine == true )
		{
			leftEdge.bottom -= inLineWidth;//#1316 �����F�ŕ`���Ǝl�����Z���Ȃ���΍�
		}
		PaintRect(leftEdge,inColor,inAlpha);
	}
	if( inDrawRightLine == true )
	{
		ALocalRect	rightEdge = inRect;
		rightEdge.left = rightEdge.right-inLineWidth;
		if( inDrawTopLine == true )
		{
			rightEdge.top += inLineWidth;//#1316 �����F�ŕ`���Ǝl�����Z���Ȃ���΍�
		}
		if( inDrawBottomLine == true )
		{
			rightEdge.bottom -= inLineWidth;//#1316 �����F�ŕ`���Ǝl�����Z���Ȃ���΍�
		}
		PaintRect(rightEdge,inColor,inAlpha);
	}
	if( inDrawTopLine == true )
	{
		ALocalRect	topEdge = inRect;
		topEdge.bottom = topEdge.top+inLineWidth;
		PaintRect(topEdge,inColor,inAlpha);
	}
	if( inDrawBottomLine == true )
	{
		ALocalRect	bottomEdge = inRect;
		bottomEdge.top = bottomEdge.bottom-inLineWidth;
		PaintRect(bottomEdge,inColor,inAlpha);
	}
}

//#871
/**
�~��`��
*/
void	CUserPane::DrawCircle( const ALocalRect& inDrawRect, const AColor& inColor, const ANumber inLineWidth, const AFloatNumber inAlpha )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextBeginPath(mContextRef);
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	::CGContextSetLineWidth(mContextRef,inLineWidth);
	::CGContextSetShouldAntialias(mContextRef,true);
	CGPoint	pt = {0};
	pt.x = (inDrawRect.left+inDrawRect.right);
	pt.x /= 2;
	pt.y = (inDrawRect.top+inDrawRect.bottom);
	pt.y /= 2;
	CGFloat	radius = (inDrawRect.right-inDrawRect.left)/2;
	::CGContextAddArc(mContextRef,pt.x,-pt.y,radius,0,2*3.15,0);
	::CGContextStrokePath(mContextRef);
	::CGContextRestoreGState(mContextRef);
	//::CGContextFlush(mContextRef);
}

/**
�t�H�[�J�X�����O��`��
*/
void	CUserPane::DrawFocusFrame( const ALocalRect& inRect )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	::CGContextSetShouldAntialias(mContextRef,true);
	::CGContextSetRGBFillColor(mContextRef,1.0,1.0,1.0,0.0);
	//���]���W�n�ł̓E�C���h�E�̏�̂ق��Ńt�H�[�J�X�����O�������Ɖ��������Ă��܂��̂ŁA�񔽓]���W�n�ɖ߂� #1082
	::CGContextScaleCTM(mContextRef, 1.0, -1.0);
	//�񔽓]���W�n��rect�擾
	CGRect	r;
	r.origin.x = inRect.left;
	r.origin.y = inRect.top;
	r.size.width = inRect.right-inRect.left;
	r.size.height = inRect.bottom-inRect.top;
	//�t�H�[�J�X�����O�`��
	NSSetFocusRingStyle(NSFocusRingOnly);
	::CGContextFillRect(mContextRef,r);
	::CGContextRestoreGState(mContextRef);
}

/**
inDrawRect�Ŏw�肷��̈��w�i�摜�ŕ`�悷��
*/
void	CUserPane::DrawBackgroundImage( const ALocalRect& inEraseRect, const AImageRect& inImageFrameRect, 
		const ANumber inLeftOffset, const AIndex inBackgroundImageIndex, const AFloatNumber inAlpha )
{
	if( true )
	{
		GetWin().DrawBackgroundImage(mControlID,inEraseRect,inImageFrameRect,inLeftOffset,inBackgroundImageIndex,inAlpha);
		PaintRect(inEraseRect,kColor_White,inAlpha);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		StSetPort	s(GetWindowRef());
		
		//
		AWindowRect	eraseWindowRect;
		GetWin().GetWindowRectFromControlLocalRect(GetControlID(),inEraseRect,eraseWindowRect);
		Rect	eraseRect;
		eraseRect.left		= eraseWindowRect.left;
		eraseRect.top		= eraseWindowRect.top;
		eraseRect.right		= eraseWindowRect.right;
		eraseRect.bottom	= eraseWindowRect.bottom;
		
		//Pane�̍���̃|�C���g�̃E�C���h�E���W��localOriginWindowPoint�Ɏ擾����
		ALocalPoint	localOrigin = {0,0};
		AWindowPoint	localOriginWindowPoint;
		GetWin().GetWindowPointFromControlLocalPoint(GetControlID(),localOrigin,localOriginWindowPoint);
		
		//UpdateRegion�ƁADrawRect�Ƃ̏d��������Clip����B
		//UpdateRegion��Clip���Ȃ��ƁACGContext�̂ق��̓f�t�H���g��update�G���A�ŃN���b�v����Ă���̂ŁAbitmap�����R�s�[�����G���A���o���Ă��܂��B
		//�܂��AmDrawUpdateRegion��Control�̃��[�J�����W�n�Ȃ̂ŁA�E�C���h�E���W�n�ɕϊ�����B
		RgnHandle	svRgn = ::NewRgn();
		::GetClip(svRgn);
		RgnHandle	updateWindowRgn = ::NewRgn();
		::CopyRgn(mDrawUpdateRegion,updateWindowRgn);
		::OffsetRgn(updateWindowRgn,localOriginWindowPoint.x,localOriginWindowPoint.y);
		RgnHandle	rectrgn = ::NewRgn();
		::RectRgn(rectrgn,&eraseRect);
		RgnHandle	rgn = ::NewRgn();
		::SectRgn(updateWindowRgn,rectrgn,rgn);
		::SetClip(rgn);
		::DisposeRgn(updateWindowRgn);
		::DisposeRgn(rgn);
		::DisposeRgn(rectrgn);
		
		GWorldPtr	gw;
		Rect	picrect;
		GetWin().GetBackgroundData(inBackgroundImageIndex,gw,picrect);
		
		PixMapHandle	pix = ::GetGWorldPixMap(gw);
		
		short	picwidth,picheight;
		picwidth = picrect.right - picrect.left;
		picheight = picrect.bottom - picrect.top;
		if( picwidth == 0 )   picwidth = 1;
		
		ANumber	startx = inImageFrameRect.left/picwidth;
		startx *= picwidth;
		ANumber	starty = inImageFrameRect.top/picheight;
		starty *= picheight;
		for( ANumber x = startx; x < inImageFrameRect.right + inLeftOffset; x += picwidth )
		{
			for( ANumber y = starty; y < inImageFrameRect.bottom; y += picheight )
			{
				//Image��\��ʒu��Image���W�n�Ŏ擾
				AImageRect	pictimage;
				pictimage.left 		= x - inLeftOffset;
				pictimage.top		= y;
				pictimage.right		= x - inLeftOffset + picwidth;
				pictimage.bottom	= y + picheight;
				//Window���W�n�ɕϊ�
				Rect	rect;
				rect.left		= pictimage.left 	- inImageFrameRect.left 	+ localOriginWindowPoint.x;
				rect.top		= pictimage.top		- inImageFrameRect.top 		+ localOriginWindowPoint.y;
				rect.right		= pictimage.right	- inImageFrameRect.left 	+ localOriginWindowPoint.x;
				rect.bottom		= pictimage.bottom	- inImageFrameRect.top 		+ localOriginWindowPoint.y;
				Rect	tmpRect;
				if( ::SectRect(&eraseRect,&rect,&tmpRect) == true ) 
				{
					::LockPixels(pix);
					::CopyBits((BitMap*)*pix,::GetPortBitMapForCopyBits(::GetWindowPort(GetWindowRef())),&picrect,&rect,srcCopy,NULL);
					::UnlockPixels(pix);
				}
			}
		}
		::SetClip(svRgn);
		::DisposeRgn(svRgn);
		
		if( true )
		{
			PaintRect(inEraseRect,kColor_White,inAlpha);
		}
	}
#endif
}

/**
���`��
*/
void	CUserPane::DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
		const AFloatNumber inAlpha, const AFloatNumber inLineDash, const AFloatNumber inLineWidth,
		const ABool inDropShadow )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	//#924
	//�c���A������PaintRect�őΉ�����
	if( inStartPoint.x == inEndPoint.x && inLineDash == 0.0 )
	{
		//�c��
		ALocalRect	rect = {0};
		rect.left		= inStartPoint.x;
		rect.top		= inStartPoint.y;
		rect.right		= inEndPoint.x + inLineWidth;
		rect.bottom		= inEndPoint.y + inLineWidth;
		PaintRect(rect,inColor,inAlpha,inDropShadow);
		return;
	}
	if( inStartPoint.y == inEndPoint.y && inLineDash == 0.0 )
	{
		//����
		ALocalRect	rect = {0};
		rect.left		= inStartPoint.x;
		rect.top		= inStartPoint.y;
		rect.right		= inEndPoint.x + inLineWidth;
		rect.bottom		= inEndPoint.y + inLineWidth;
		PaintRect(rect,inColor,inAlpha,inDropShadow);
		return;
	}
	
	::CGContextSaveGState(mContextRef);
	//
	if( inDropShadow == true && AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		CGSize	offset = {1.5,-1.5};
		CGColorRef	color = CreateCGColor(mDropShadowColor,1.0);
		::CGContextSetShadowWithColor(mContextRef,offset,1.0,color);
		::CFRelease(color);
	}
	//
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	/*#1275 NSBezierPath�̕`��p�����[�^�̕���������������Ȃ��̂�NSBezierPath���g�����Ƃɂ���
	::CGContextBeginPath(mContextRef);
	::CGContextMoveToPoint(mContextRef,inStartPoint.x,-inStartPoint.y-1);//win 080618
	::CGContextAddLineToPoint(mContextRef,inEndPoint.x,-inEndPoint.y-1);//win 080618
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
	*/
	NSBezierPath*	path = [[[NSBezierPath alloc] init] autorelease];
	[path moveToPoint:NSMakePoint(inStartPoint.x,-inStartPoint.y-1)];
	[path lineToPoint:NSMakePoint(inEndPoint.x,-inEndPoint.y-1)];
	[path setLineWidth:inLineWidth];
	if( inLineDash != 0.0 )
	{
		CGFloat	lengths[2];
		lengths[0] = inLineDash;
		lengths[1] = inLineDash;
        [path setLineDash:lengths count:2 phase:0.0];
	}
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	::CGContextSetShouldAntialias(mContextRef,false);
	[path stroke];
	::CGContextRestoreGState(mContextRef);
}

//#723
/**
�Ȑ��`��
*/
void	CUserPane::DrawCurvedLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
		const AFloatNumber inAlpha, const AFloatNumber inLineDash, const AFloatNumber inLineWidth )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	//x��y���������͒ʏ�̒����ɂ���
	if( inStartPoint.x == inEndPoint.x || inStartPoint.y == inEndPoint.y )
	{
		ALocalPoint	spt = inStartPoint;
		ALocalPoint	ept = inEndPoint;
		ept.x -= inLineWidth;
		DrawLine(spt,ept,inColor,inAlpha,inLineDash,inLineWidth);
		return;
	}
	
	//�J�n�ʒu�E�I���ʒu�E���_
	CGPoint	startPoint = {inStartPoint.x,-inStartPoint.y-1};
	CGPoint	endPoint = {inEndPoint.x,-inEndPoint.y-1};
	CGPoint	middlePoint = {(startPoint.x+endPoint.x)/2,(startPoint.y+endPoint.y)/2};
	//gstate�ۑ�
	::CGContextSaveGState(mContextRef);
	//�F�擾
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	//�p�X�J�n
	::CGContextBeginPath(mContextRef);
	//�x�W�F�`��
	::CGContextMoveToPoint(mContextRef,startPoint.x,startPoint.y);
	::CGContextAddArcToPoint(mContextRef,middlePoint.x,startPoint.y,middlePoint.x,middlePoint.y,5.0);
	::CGContextAddArcToPoint(mContextRef,middlePoint.x,endPoint.y,endPoint.x,endPoint.y,5.0);
	//�F�ݒ�
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	//�s��
	::CGContextSetLineWidth(mContextRef,inLineWidth);
	//�A���`�G�C���A�XON
	::CGContextSetShouldAntialias(mContextRef,true);
	//line dash
	if( inLineDash != 0.0 )
	{
		CGFloat	lengths[2];
		lengths[0] = inLineDash;
		lengths[1] = inLineDash;
		::CGContextSetLineDash(mContextRef,0.0,lengths,2);
	}
	//�p�X�`��
	::CGContextStrokePath(mContextRef);
	//gstate���A
	::CGContextRestoreGState(mContextRef);
}

//B0000 ������
/**
���`�惂�[�h�ݒ�
*/
void	CUserPane::SetDrawLineMode( const AColor& inColor, const AFloatNumber inAlpha, 
		const AFloatNumber inLineDash, const AFloatNumber inLineWidth )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
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
}

//B0000 ������
/**
���`�惂�[�h����
*/
void	CUserPane::RestoreDrawLineMode()
{
	::CGContextRestoreGState(mContextRef);
}

//B0000 ������
/**
���`��
*/
void	CUserPane::DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextBeginPath(mContextRef);
	::CGContextMoveToPoint(mContextRef,inStartPoint.x,-inStartPoint.y-1);//win 080618
	::CGContextAddLineToPoint(mContextRef,inEndPoint.x,-inEndPoint.y-1);//win 080618
	::CGContextStrokePath(mContextRef);
}

/**
�L�����b�g�pXOR���`��iAPICB_DoDraw()�O�̕`��p�j
*/
void	CUserPane::DrawXorCaretLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const ABool inClipToFrame, 
									 const ABool inFlush, const ABool inDash, const ANumber inPenSize, const AFloatNumber inAlpha )//#1398
{
	//�R���g���[����\�����͉������Ȃ�
	if( IsControlVisible() == false )
	{
		return;
	}
	
	//#1012 10.3�͑Ή����Ȃ� if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )//CGContextSetBlendMode��10.4�ȍ~�Ή��BRetina�Ή��̂��߂ɂ�QD�͎g�p�o���Ȃ��B
	{
		//GraphicsContext�擾
		//���݁AdrawRect()���Ȃ�A����context���g��
		CGContextRef	contextRef = mContextRef;
		ABool	focusLocked = false;
		if( contextRef == nil )
		{
			//drawRect()�O�Ȃ�Aview��lockFocus���āAcontext�擾
			CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
			if( [view lockFocusIfCanDraw] == NO )   return;
			//context�擾
			contextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
			//drawRect()���̕`��ƍ��킹�邽�ߍ��W�ϊ�����
			::CGContextScaleCTM(contextRef, 1.0, -1.0);
			focusLocked = true;
			
			//#558
			//�c�������W�n�ɂ���
			if( mVerticalMode == true )
			{
				ChangeToVerticalCTM(contextRef);
			}
		}
		//save gstate
		::CGContextSaveGState(contextRef);
		//clip
		if( inClipToFrame == true )
		{
			//view�̃T�C�Y�擾
			ANumber	viewwidth = GetWin().GetControlWidth(mControlID);
			ANumber	viewheight = GetWin().GetControlHeight(mControlID);
			//�c�������[�h�̏ꍇ�́A�r���[�͈͂��c���ϊ� #558
			if( mVerticalMode == true )
			{
				ANumber	swap = viewwidth;
				viewwidth = viewheight;
				viewheight = swap;
			}
			//�r���[��clip
			CGRect	cliprect = {0};
			cliprect.origin.x = 0;
			cliprect.origin.y = -viewheight;
			cliprect.size.width = viewwidth;
			cliprect.size.height = viewheight;
			::CGContextClipToRect(contextRef,cliprect);
		}
		//�`��
		::CGContextBeginPath(contextRef);
		::CGContextMoveToPoint(contextRef,inStartPoint.x,-inStartPoint.y-1);
		::CGContextAddLineToPoint(contextRef,inEndPoint.x,-inEndPoint.y-1);
		::CGContextSetRGBStrokeColor(contextRef,1.0,1.0,1.0,inAlpha);//#1398
		::CGContextSetLineWidth(contextRef,inPenSize);
		::CGContextSetShouldAntialias(contextRef,false);
		::CGContextSetBlendMode(contextRef,kCGBlendModeExclusion);//kCGBlendModeDifference);//kCGBlendModeExclusion);
		if( inDash == true )
		{
			CGFloat	lengths[2];
			lengths[0] = 1.0;
			lengths[1] = 1.0;
			::CGContextSetLineDash(contextRef,0.0,lengths,2);
		}
		::CGContextStrokePath(contextRef);
		::CGContextSynchronize(contextRef);
		//#1034
		if( inFlush == true )
		{
			::CGContextFlush(contextRef);
		}
		//restore gstate
		::CGContextRestoreGState(contextRef);
		//context��release�s�v�Ǝv����B���̃^�C�~���O��autorelease����ƃG���[�\������邽�߁B
		if( focusLocked == true )
		{
			CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
			[view unlockFocus];
		}
	}
	//#1012 10.3�͑Ή����Ȃ�
#if 0
	else
	{
		//caret�^�C�}�[�^�C���A�E�g������APICB_DoDraw()�O�Ȃ̂ŁACGContext���g���Ȃ��B
		StSetPort	s(GetWindowRef());
		//::RGBForeColor(&kColor_Black);
		
		//�t���[����Clip
		RgnHandle	svRgn;
		svRgn = ::NewRgn();
		::GetClip(svRgn);
		
		//clip
		if( inClipToFrame == true )
		{
			//Local���W��view�̃t���[�����擾
			ALocalRect	localFrame = {0};
			ANumber	viewwidth = GetWin().GetControlWidth(mControlID);
			ANumber	viewheight = GetWin().GetControlHeight(mControlID);
			localFrame.left		= 0;
			localFrame.top		= 0;
			localFrame.right	= viewwidth;
			localFrame.bottom	= viewheight;
			//Window���W�֕ϊ�
			AWindowRect	localFrameWindowRect = {0};
			GetWin().GetWindowRectFromControlLocalRect(mControlID,localFrame,localFrameWindowRect);
			//clip
			Rect	frameRect = {0};
			frameRect.left		= localFrameWindowRect.left;
			frameRect.top		= localFrameWindowRect.top;
			frameRect.right		= localFrameWindowRect.right;
			frameRect.bottom	= localFrameWindowRect.bottom;
			::ClipRect(&frameRect);
			if( mContextRef != NULL )
			{
				frameRect.left	= -1000;
				frameRect.top = -1000;
				frameRect.right = 1000;
				frameRect.bottom = 1000;
				::EraseRect(&frameRect);
			}
		}
		
		//������Window���W�n�ɕϊ����āA�`��
		AWindowPoint	spt,ept;
		GetWin().GetWindowPointFromControlLocalPoint(mControlID,inStartPoint,spt);
		GetWin().GetWindowPointFromControlLocalPoint(mControlID,inEndPoint,ept);
		if( inDash == true )
		{
			Pattern	pat;
			::StuffHex(&pat,"\pAA55AA55AA55AA55");
			::PenPat(&pat);
		}
		else
		{
			Pattern	pat;
			::PenPat(::GetQDGlobalsBlack(&pat));
		}
		::PenSize(inPenSize,inPenSize);
		::PenMode(patXor);
		::MoveTo(spt.x,spt.y);
		::LineTo(ept.x,ept.y);
		if( mContextRef != NULL )
		{
			::MoveTo(0,0);
			::LineTo(1000,1000);
			::MoveTo(100,0);
			::LineTo(1000,-1000);
		}
		::PenMode(0);
		::PenSize(1,1);
		
		//Clip����
		::SetClip(svRgn);
		::DisposeRgn(svRgn);
	}
#endif
}

/**
�L�����b�g�pXOR���`��iAPICB_DoDraw()�O�̕`��p�j
*/
void	CUserPane::DrawXorCaretRect( const ALocalRect& inLocalRect, const ABool inClipToFrame, const ABool inFlush )
{
	//�R���g���[����\�����͉������Ȃ�
	if( IsControlVisible() == false )
	{
		return;
	}
	//#1012 10.3�͑Ή����Ȃ� if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )//CGContextSetBlendMode��10.4�ȍ~�Ή��BRetina�Ή��̂��߂ɂ�QD�͎g�p�o���Ȃ��B
	{
		//GraphicsContext�擾
		//���݁AdrawRect()���Ȃ�A����context���g��
		CGContextRef	contextRef = mContextRef;
		ABool	focusLocked = false;
		if( contextRef == nil )
		{
			//drawRect()�O�Ȃ�Aview��lockFocus���āAcontext�擾
			CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
			[view lockFocus];
			//context�擾
			contextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
			//drawRect()���̕`��ƍ��킹�邽�ߍ��W�ϊ�����
			::CGContextScaleCTM(contextRef, 1.0, -1.0);
			focusLocked = true;
			
			//#558
			//�c�������W�n�ɂ���
			if( mVerticalMode == true )
			{
				ChangeToVerticalCTM(contextRef);
			}
		}
		//save gstate
		::CGContextSaveGState(contextRef);
		//clip
		if( inClipToFrame == true )
		{
			//view�̃T�C�Y�擾
			ANumber	viewwidth = GetWin().GetControlWidth(mControlID);
			ANumber	viewheight = GetWin().GetControlHeight(mControlID);
			//�c�������[�h�̏ꍇ�́A�r���[�͈͂��c���ϊ� #558
			if( mVerticalMode == true )
			{
				ANumber	swap = viewwidth;
				viewwidth = viewheight;
				viewheight = swap;
			}
			//�r���[��clip
			CGRect	cliprect = {0};
			cliprect.origin.x = 0;
			cliprect.origin.y = -viewheight;
			cliprect.size.width = viewwidth;
			cliprect.size.height = viewheight;
			::CGContextClipToRect(contextRef,cliprect);
		}
		
		//�`��
		::CGContextSetRGBStrokeColor(contextRef,1.0,1.0,1.0,1.0);
		::CGContextSetLineWidth(contextRef,1);
		::CGContextSetShouldAntialias(contextRef,false);
		::CGContextSetBlendMode(contextRef,kCGBlendModeExclusion);
		::CGContextStrokeRect(contextRef,GetCGRectFromARect(inLocalRect));
		::CGContextSynchronize(contextRef);
		//#1034
		if( inFlush == true )
		{
			::CGContextFlush(contextRef);
		}
		//restore gstate
		::CGContextRestoreGState(contextRef);
		//context��release�s�v�Ǝv����B���̃^�C�~���O��autorelease����ƃG���[�\������邽�߁B
		if( focusLocked == true )
		{
			CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
			[view unlockFocus];
		}
	}
	//#1012 10.3�͑Ή����Ȃ�
#if 0
	else
	{
		//caret�^�C�}�[�^�C���A�E�g������APICB_DoDraw()�O�Ȃ̂ŁACGContext���g���Ȃ��B
		StSetPort	s(GetWindowRef());
		//::RGBForeColor(&kColor_Black);
		
		//�t���[����Clip
		RgnHandle	svRgn;
		svRgn = ::NewRgn();
		::GetClip(svRgn);
		
		if( inClipToFrame == true )
		{
			AWindowRect	localFrameWindowRect;
			GetControlWindowFrameRect(localFrameWindowRect);
			Rect	frameRect;
			frameRect.left		= localFrameWindowRect.left;
			frameRect.top		= localFrameWindowRect.top;
			frameRect.right		= localFrameWindowRect.right;
			frameRect.bottom	= localFrameWindowRect.bottom;
			::ClipRect(&frameRect);
		}
		
		//������Window���W�n�ɕϊ����āA�`��
		AWindowRect	windowRect;
		GetWin().GetWindowRectFromControlLocalRect(GetControlID(),inLocalRect,windowRect);
		Rect	rect;
		rect.left	= windowRect.left;
		rect.top	= windowRect.top;
		rect.right	= windowRect.right;
		rect.bottom	= windowRect.bottom;
		::PenMode(patXor);
		::FrameRect(&rect);
		::PenMode(0);
		
		//Clip����
		::SetClip(svRgn);
		::DisposeRgn(svRgn);
	}
#endif
}

/**
LocalPoint->CGPoint
*/
CGPoint	CUserPane::GetCGPointFromAPoint( const ALocalPoint& inPoint ) const
{
	CGPoint	point;
	point.x = inPoint.x;
	point.y = -inPoint.y;
	return point;
}

/**
LocalRect->CGRect
*/
CGRect	CUserPane::GetCGRectFromARect( const ALocalRect& inRect ) const
{
	CGRect	rect;
	rect.origin.x = inRect.left;
	rect.origin.y = -inRect.bottom;
	rect.size.width = inRect.right-inRect.left;
	rect.size.height = inRect.bottom-inRect.top;
	return rect;
}

#if SUPPORT_CARBON
/**
DoDraw()�O��CGContext��ݒ�
*/
void	CUserPane::SetupCGContextOutOfDoDraw()
{
	if( mContextRef != NULL )   return;
	::QDBeginCGContext(::GetWindowPort(GetWindowRef()),&mContextRef);
	//context�̍��W�n�����オ(0,0)�ɂ���
	//grafPort�̌��_������Ă��Ă��K��(0,0)������
	Rect	winRect;
	::GetWindowBounds(GetWindowRef(),kWindowContentRgn,&winRect);
	::CGContextTranslateCTM(mContextRef,0.0,winRect.bottom-winRect.top);
}

/**
DoDraw()�O�Őݒ肵��CGContext�����
*/
void	CUserPane::EndCGContextOutOfDoDraw()
{
	if( mContextRef == NULL )   return;
	::QDEndCGContext(::GetWindowPort(GetWindowRef()),&mContextRef);
	mContextRef = NULL;
}
#endif

//B0000
/**
�����`��t���b�V������

OS X�͎��̃C�x���g�g�����U�N�V�������A���Ă���܂ŕ`��C���[�W������o�b�t�@�ɂ��߂Ă���
*/
void	CUserPane::RedrawImmediately()
{
	if( true )
	{
		GetWin().UpdateWindow();
	}
#if SUPPORT_CARBON
	else
	{
		::QDFlushPortBuffer(::GetWindowPort(GetWindowRef()),NULL);
	}
#endif
}

//#688
/**
view��live resize�����ǂ�����Ԃ�
*/
ABool	CUserPane::IsInLiveResize() const
{
	//Resize completed�������i��viewDidEndLiveResize�j���AinLiveResize��YES�ɂȂ��Ă��܂��̂ŁA
	//Resize completed�������̓t���O���g�p���āAfalse��Ԃ��悤�ɂ���B
	if( GetWinConst().IsDoingResizeCompleted() == true )
	{
		return false;
	}
	
	//view���擾
	CocoaUserPaneView*	view = GetWinConst().GetCocoaObjectsConst().FindCocoaUserPaneViewByTag(mControlID);
	if( view == nil )   { _ACError("",NULL); return false; }
	
	//live resize�����ǂ���
	return ([view inLiveResize]==YES);
}

#pragma mark ===========================

#pragma mark ---�C���[�W
//#725

//�C���[�W�f�[�^
AHashArray<AImageID>	gImageIDArray;
AArray<NSImage*>	gImageArray;
AArray<NSImage*>	gDarkImageArray;//#1316
ATextArray	gImageArray_FilePath;//#1090
ATextArray	gImageArray_Name;//#1090
AArray<ANumber>	gImageArray_Width;//#1090
AArray<ANumber>	gImageArray_Height;//#1090
//AObjectArray<AFileAcc>	gImageArray_File;

/**
�C���[�W�o�^
*/
void	CUserPane::RegisterImageByFile( const AImageID inImageID, const AFileAcc& inImageFile )
{
	//�t�@�C���p�X�擾
	AText	filepath;
	inImageFile.GetPath(filepath);
	//Array�Ɋi�[ #1090
	gImageIDArray.Add(inImageID);
	gImageArray.Add(nil);
	gDarkImageArray.Add(nil);//#1316
	gImageArray_FilePath.Add(filepath);
	gImageArray_Name.Add(GetEmptyText());
	gImageArray_Width.Add(0);
	gImageArray_Height.Add(0);
}

/**
�C���[�W�o�^
*/
void	CUserPane::RegisterImageByName( const AImageID inImageID, const AText& inImageFileName )
{
	//Array�Ɋi�[ #1090
	gImageIDArray.Add(inImageID);
	gImageArray.Add(nil);
	gDarkImageArray.Add(nil);//#1316
	gImageArray_FilePath.Add(GetEmptyText());
	gImageArray_Name.Add(inImageFileName);
	gImageArray_Width.Add(0);
	gImageArray_Height.Add(0);
}
void	CUserPane::RegisterImageByName( const AImageID inImageID, const AUCharPtr inImageFileName )
{
	AText	imageFileName(inImageFileName);
	RegisterImageByName(inImageID,imageFileName);
}

/**
�C���[�W���I���f�}���h�Ń��[�h����
*/
AIndex	CUserPane::LoadImage( const AImageID inImageID )
{
	//�C���[�W��index�擾
	AIndex	index = gImageIDArray.Find(inImageID);
	//�C���[�W�����[�h�Ȃ�A�C���[�W�擾�E�ݒ�
	if( gImageArray.Get(index) == nil )
	{
		//�C���[�W�t�@�C���p�X�擾
		AText	filepath;
		gImageArray_FilePath.Get(index,filepath);
		if( filepath.GetItemCount() > 0 )
		{
			//�t�@�C���p�X�Ŏw�肳��Ă���ꍇ
			
			//�t�@�C���p�XNSString�擾
			AStCreateNSStringFromAText	filepathstr(filepath);
			//NSImage����
			NSImage*	image = [[NSImage alloc] initByReferencingFile:filepathstr.GetNSString()];//�|�C���^��ێ�����̂�retainCount=1�̂܂�(init�ɂ��)�ɂ���
			//image, width, height�ݒ�
			gImageArray.Set(index,image);
			NSSize	size = [image size];
			gImageArray_Width.Set(index,size.width);
			gImageArray_Height.Set(index,size.height);
		}
		else
		{
			//�C���[�W���Ŏw�肳��Ă���ꍇ
			
			//�C���[�W���擾
			AText	imageFileName;
			gImageArray_Name.Get(index,imageFileName);
			//suffix�폜
			imageFileName.DeleteAfter(imageFileName.GetSuffixStartPos());
			//�t�@�C����NSString�擾
			AStCreateNSStringFromAText	filenamestr(imageFileName);
			//NSImage����
			NSImage*	image = [NSImage imageNamed:filenamestr.GetNSString()];//�|�C���^��ێ�����̂�retainCount=1�̂܂�(init�ɂ��)�ɂ���
			//image, width, height�ݒ�
			gImageArray.Set(index,image);
			NSSize	size = [image size];
			gImageArray_Width.Set(index,size.width);
			gImageArray_Height.Set(index,size.height);
			
			//�_�[�N���[�h�p�C���[�W #1316
			AText	darkImageFileName;
			darkImageFileName.SetText(imageFileName);
			darkImageFileName.AddCstring("_dark");
			AStCreateNSStringFromAText	darkfilenamestr(darkImageFileName);
			NSImage*	darkImage = [NSImage imageNamed:darkfilenamestr.GetNSString()];
			gDarkImageArray.Set(index,darkImage);
		}
	}
	return index;
}

/**
*/
void	CUserPane::UnregisterImage( const AImageID inImageID )
{
	AIndex	index = gImageIDArray.Find(inImageID);
	if( index != kIndex_Invalid )
	{
		gImageIDArray.Delete1(index);
		gImageArray.Delete1(index);
		gDarkImageArray.Delete1(index);//#1316
		gImageArray_FilePath.Delete1(index);//#1090
		gImageArray_Name.Delete1(index);//#1090
		gImageArray_Width.Delete1(index);//#1090
		gImageArray_Height.Delete1(index);//#1090
	}
}

/**
�C���[�W�`��
*/
void	CUserPane::DrawImage( const AImageID inImageID, const ALocalPoint& inPoint, 
							  const ANumber inWidth, const ANumber inHeight )
{
	//NSImage�擾
	AIndex	index = LoadImage(inImageID);
	NSImage*	image = gImageArray.Get(index);
	//#1316
	if( AApplication::GetApplication().NVI_IsDarkMode() == true )
	{
		NSImage*	darkImage = gDarkImageArray.Get(index);
		if( darkImage != nil )
		{
			image = darkImage;
		}
	}
	ANumber	imagewidth = gImageArray_Width.Get(index);//#1090 [image size].width;
	ANumber	imageheight = gImageArray_Height.Get(index);//#1090 [image size].height;
	//�`��T�C�Y�擾�i�����w�肪�Ȃ���΃C���[�W�T�C�Y���g�p�j
	ANumber	drawwidth = imagewidth;
	ANumber	drawheight = imageheight;
	if( inWidth > 0 )
	{
		drawwidth = inWidth;
	}
	if( inHeight > 0 )
	{
		drawheight = inHeight;
	}
	//�T�C�Y0�Ȃ牽�����Ȃ�
	if( imagewidth == 0 || imageheight == 0 )   return;
	
	//�`���rect�擾
	NSRect	drawrect = {0};
	drawrect.origin.x = inPoint.x;
	drawrect.origin.y = -(inPoint.y+drawheight);
	drawrect.size.width 	= drawwidth;
	drawrect.size.height 	= drawheight;
	//�C���[�Wrect
	NSRect	srcrect = {0};
	srcrect.origin.x = 0;
	srcrect.origin.y = 0;
	srcrect.size.width 		= imagewidth;
	srcrect.size.height 	= imageheight;
	//�`��
	[image drawInRect:drawrect fromRect:srcrect operation:NSCompositeSourceOver fraction:1.0];
}

/**
�C���[�W�T�C�Y�擾
*/
ANumber	CUserPane::GetImageWidth( const AImageID inImageID ) const
{
	AIndex	index = LoadImage(inImageID);
	/*#1090
	NSImage*	image = gImageArray.Get(index);
	return [image size].width;
	*/
	return gImageArray_Width.Get(index);//#1090
}
ANumber	CUserPane::GetImageHeight( const AImageID inImageID ) const
{
	AIndex	index = LoadImage(inImageID);
	/*#1090
	NSImage*	image = gImageArray.Get(index);
	return [image size].height;
	*/
	return gImageArray_Height.Get(index);//#1090
}

/**
�C���[�W�`��iNSImage*�w��j
*/
void	DrawImage( NSImage* inImage, const ALocalPoint inPoint, const ANumber inWidth, const ANumber inHeight )
{
	//�`���rect�擾
	NSRect	drawrect = {0};
	drawrect.origin.x = inPoint.x;
	drawrect.origin.y = -(inPoint.y+inHeight);
	drawrect.size.width 	= inWidth;
	drawrect.size.height 	= inHeight;
	//�C���[�Wrect
	NSRect	srcrect = {0};
	srcrect.origin.x = 0;
	srcrect.origin.y = 0;
	srcrect.size.width 		= inWidth;
	srcrect.size.height 	= inHeight;
	//�`��
	[inImage drawInRect:drawrect fromRect:srcrect operation:NSCompositeSourceOver fraction:1.0];
}

/**
�C���[�W�`��i���J��Ԃ��j
*/
void	CUserPane::DrawImageFlexibleWidth( const AImageID inImageID0, const AImageID inImageID1, const AImageID inImageID2,
		const ALocalPoint& inPoint, const ANumber inWidth )
{
	//image, width, height�擾
	//#1090
	//index�擾
	AIndex	index[3] = {0};
	index[0] = LoadImage(inImageID0);
	index[1] = LoadImage(inImageID1);
	index[2] = LoadImage(inImageID2);
	//
	NSImage*	image[3] = {0};
	/*#1090
	image[0] = gImageArray.Get(gImageIDArray.Find(inImageID0));
	image[1] = gImageArray.Get(gImageIDArray.Find(inImageID1));
	image[2] = gImageArray.Get(gImageIDArray.Find(inImageID2));
	*/
	ANumber	width[3] = {0}, height[3] = {0};
	for( AIndex i = 0; i < 3; i++ )
	{
		//image�擾
		image[i] = gImageArray.Get(index[i]);
		//width, height�擾
		width[i] = gImageArray_Width.Get(index[i]);//#1090 [image[i] size].width;
		if( width[i] == 0 )   return;
		height[i] = gImageArray_Height.Get(index[i]);//#1090 [image[i] size].height;
		if( height[i] == 0 )   return;
	}
	//pt
	ALocalPoint	pt = inPoint;
	//Image0�`��
	::DrawImage(image[0],pt,width[0],height[0]);
	//x�ړ�
	pt.x += width[0];
	//�J��Ԃ��J�E���g�擾
	ANumber	flWidth = inWidth - width[0] - width[2];
	ANumber	flWidthCount = (flWidth+width[1]-1)/width[1];
	for( AIndex i = 0; i < flWidthCount; i++ )
	{
		ANumber	w = width[1];
		//�J��Ԃ��Ō�̍��ڂ̏ꍇ�A�T�C�Y�𒲐�����B
		if( i == flWidthCount-1 )
		{
			w = inPoint.x + inWidth - width[2] - pt.x;
		}
		//Image1�`��
		::DrawImage(image[1],pt,w,height[1]);
		//x�ړ�
		pt.x += width[1];
	}
	//x�ړ�
	pt.x = inPoint.x + inWidth - width[2];
	//Image2�`��
	::DrawImage(image[2],pt,width[2],height[2]);
}

/**
�C���[�W�`��i�c�J��Ԃ��j
*/
void	CUserPane::DrawImageFlexibleHeight( const AImageID inImageID0, const AImageID inImageID1, const AImageID inImageID2,
		const ALocalPoint& inPoint, const ANumber inHeight )
{
	//image, width, height�擾
	//#1090
	//index�擾
	AIndex	index[3] = {0};
	index[0] = LoadImage(inImageID0);
	index[1] = LoadImage(inImageID1);
	index[2] = LoadImage(inImageID2);
	//
	NSImage*	image[3] = {0};
	/*#1090
	image[0] = gImageArray.Get(gImageIDArray.Find(inImageID0));
	image[1] = gImageArray.Get(gImageIDArray.Find(inImageID1));
	image[2] = gImageArray.Get(gImageIDArray.Find(inImageID2));
	*/
	ANumber	width[3] = {0}, height[3] = {0};
	for( AIndex i = 0; i < 3; i++ )
	{
		//image�擾
		image[i] = gImageArray.Get(index[i]);
		//width, height�擾
		width[i] = gImageArray_Width.Get(index[i]);//#1090 [image[i] size].width;
		if( width[i] == 0 )   return;
		height[i] = gImageArray_Height.Get(index[i]);//#1090 [image[i] size].height;
		if( height[i] == 0 )   return;
	}
	//pt
	ALocalPoint	pt = inPoint;
	//Image0�`��
	::DrawImage(image[0],pt,width[0],height[0]);
	//y�ړ�
	pt.y += height[0];
	//�J��Ԃ��J�E���g�擾
	ANumber	flHeight = inHeight - height[0] - height[2];
	ANumber	flHeightCount = (flHeight+height[1]-1)/height[1];
	for( AIndex i = 0; i < flHeightCount; i++ )
	{
		ANumber	h = height[1];
		//�J��Ԃ��Ō�̍��ڂ̏ꍇ�A�T�C�Y�𒲐�����B
		if( i == flHeightCount-1 )
		{
			h = inPoint.y + inHeight - height[2] - pt.y;
		}
		//Image1�`��
		::DrawImage(image[1],pt,width[1],h);
		//y�ړ�
		pt.y += height[1];
	}
	//y�ړ�
	pt.y = inPoint.y + inHeight - height[2];
	//Image2�`��
	::DrawImage(image[2],pt,width[2],height[2]);
}

/**
�C���[�W�`��i�c���J��Ԃ��j
*/
void	CUserPane::DrawImageFlexibleWidthAndHeight( 
		const AImageID inImageID0, const AImageID inImageID1, const AImageID inImageID2,
		const AImageID inImageID3, const AImageID inImageID4, const AImageID inImageID5,
		const AImageID inImageID6, const AImageID inImageID7, const AImageID inImageID8,
		const ALocalPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	//image, width, height�擾
	//#1090
	AIndex	index[9] = {0};
	index[0] = LoadImage(inImageID0);
	index[1] = LoadImage(inImageID1);
	index[2] = LoadImage(inImageID2);
	index[3] = LoadImage(inImageID3);
	index[4] = LoadImage(inImageID4);
	index[5] = LoadImage(inImageID5);
	index[6] = LoadImage(inImageID6);
	index[7] = LoadImage(inImageID7);
	index[8] = LoadImage(inImageID8);
	//
	NSImage*	image[9] = {0};
	/*#1090
	image[0] = gImageArray.Get(gImageIDArray.Find(inImageID0));
	image[1] = gImageArray.Get(gImageIDArray.Find(inImageID1));
	image[2] = gImageArray.Get(gImageIDArray.Find(inImageID2));
	image[3] = gImageArray.Get(gImageIDArray.Find(inImageID3));
	image[4] = gImageArray.Get(gImageIDArray.Find(inImageID4));
	image[5] = gImageArray.Get(gImageIDArray.Find(inImageID5));
	image[6] = gImageArray.Get(gImageIDArray.Find(inImageID6));
	image[7] = gImageArray.Get(gImageIDArray.Find(inImageID7));
	image[8] = gImageArray.Get(gImageIDArray.Find(inImageID8));
	*/
	ANumber	width[9] = {0}, height[9] = {0};
	for( AIndex i = 0; i < 9; i++ )
	{
		//image�擾
		image[i] = gImageArray.Get(index[i]);
		//width, height�擾
		width[i] = gImageArray_Width.Get(index[i]);//#1090 [image[i] size].width;
		if( width[i] == 0 )   return;
		height[i] = gImageArray_Height.Get(index[i]);//#1090 [image[i] size].height;
		if( height[i] == 0 )   return;
	}
	//pt
	ALocalPoint	pt = inPoint;
	
	//=============��i=============
	
	{
		//Image0�`��
		::DrawImage(image[0],pt,width[0],height[0]);
		//x�ړ�
		pt.x += width[0];
		//�J��Ԃ��J�E���g�擾
		ANumber	flWidth = inWidth - width[0] - width[2];
		ANumber	flWidthCount = (flWidth+width[1]-1)/width[1];
		for( AIndex i = 0; i < flWidthCount; i++ )
		{
			ANumber	w = width[1];
			//�J��Ԃ��Ō�̍��ڂ̏ꍇ�A�T�C�Y�𒲐�����B
			if( i == flWidthCount-1 )
			{
				w = inPoint.x + inWidth - width[2] - pt.x;
			}
			//Image1�`��
			::DrawImage(image[1],pt,w,height[1]);
			//x�ړ�
			pt.x += width[1];
		}
		//x�ړ�
		pt.x = inPoint.x + inWidth - width[2];
		//Image2�`��
		::DrawImage(image[2],pt,width[2],height[2]);
		//x,y�ړ�
		pt.x = 0;
		pt.y += height[0];
	}
	
	//=============���i=============
	
	//�J��Ԃ��J�E���g�擾
	ANumber	flHeight = inHeight - height[0] - height[6];
	ANumber	flHeightCount = (flHeight+height[3]-1)/height[3];
	for( AIndex j = 0; j < flHeightCount; j++ )
	{
		//Image3�`��
		::DrawImage(image[3],pt,width[3],height[3]);
		//x�ړ�
		pt.x += width[3];
		//�J��Ԃ��J�E���g�擾
		ANumber	flWidth = inWidth - width[3] - width[5];
		ANumber	flWidthCount = (flWidth+width[4]-1)/width[4];
		for( AIndex i = 0; i < flWidthCount; i++ )
		{
			ANumber	w = width[4];
			ANumber	h = height[4];
			//�J��Ԃ��Ō�̍��ڂ̏ꍇ�A�T�C�Y�𒲐�����B
			if( i == flWidthCount-1 )
			{
				w = inPoint.x + inWidth - width[5] - pt.x;
			}
			if( j == flHeightCount-1 )
			{
				h = inPoint.y + inHeight - height[6] - pt.y;
			}
			//Image4�`��
			::DrawImage(image[4],pt,w,h);
			//x�ړ�
			pt.x += width[4];
		}
		//x�ړ�
		pt.x = inPoint.x + inWidth - width[5];
		//Image5�`��
		::DrawImage(image[5],pt,width[5],height[5]);
		//x,y�ړ�
		pt.x = 0;
		pt.y += height[3];
	}
	
	//=============���i=============
	
	{
		//x,y�ړ�
		pt.x = 0;
		pt.y = inPoint.y + inHeight - height[6];
		//Image6�`��
		::DrawImage(image[6],pt,width[6],height[6]);
		//x�ړ�
		pt.x += width[0];
		//�J��Ԃ��J�E���g�擾
		ANumber	flWidth = inWidth - width[0] - width[8];
		ANumber	flWidthCount = (flWidth+width[7]-1)/width[7];
		for( AIndex i = 0; i < flWidthCount; i++ )
		{
			ANumber	w = width[7];
			//�J��Ԃ��Ō�̍��ڂ̏ꍇ�A�T�C�Y�𒲐�����B
			if( i == flWidthCount-1 )
			{
				w = inPoint.x + inWidth - width[8] - pt.x;
			}
			//Image7�`��
			::DrawImage(image[7],pt,w,height[7]);
			//x�ړ�
			pt.x += width[7];
		}
		//x�ړ�
		pt.x = inPoint.x + inWidth - width[8];
		//Image8�`��
		::DrawImage(image[8],pt,width[8],height[8]);
	}
}

#pragma mark ===========================

#pragma mark ---Refresh

/**
�`��Refresh�i�w��Rect�j
*/
void	CUserPane::RefreshRect( const ALocalRect& inLocalRect )
{
	GetWin().RefreshControlRect(GetControlID(),inLocalRect);//#688
	/*
	OSStatus	err = noErr;
	
	* HIRect	rect;
	rect.origin.x = inLocalRect.left;
	rect.origin.y = inLocalRect.top;
	rect.size.width = inLocalRect.right - inLocalRect.left;
	rect.size.height = inLocalRect.bottom - inLocalRect.top;
	::HIViewSetNeedsDisplayInRect(mControlRef,&rect,true);
	::HIViewRender(mControlRef);*
	
	ALocalRect	localFrameRect;
	GetControlLocalFrameRect(localFrameRect);
	//R0108������ �t���[���T�C�Y�g���~���O�̌��ʁAlocalRect.top > localRect.bottom �ƂȂ�ƁADoDraw����UpdateRegion����ŁA�S�͈͑ΏۂɂȂ��Ă��܂��H�̂�
	//RefreshRect���t���[���O�̂Ƃ��͉������Ȃ��悤�ɂ���
	if( inLocalRect.bottom < localFrameRect.top )   return;
	if( inLocalRect.top > localFrameRect.bottom )   return;
	if( inLocalRect.right < localFrameRect.left )   return;
	if( inLocalRect.left > localFrameRect.right )   return;
	//OSX10.3�ł�HIViewSetNeedsDisplayInRect���g���Ȃ��̂ŁA���L�ɕύX
	//�t���[���T�C�Y�Ńg���~���O���Ă����Ȃ���HIViewSetNeedsDisplayInRegion�̂ق��͂��܂������Ȃ����Ƃ�����
	ALocalRect	localRect = inLocalRect;
	if( localRect.top < localFrameRect.top )   localRect.top = localFrameRect.top;
	if( localRect.bottom > localFrameRect.bottom )   localRect.bottom = localFrameRect.bottom;
	if( localRect.left < localFrameRect.left )   localRect.left = localFrameRect.left;
	if( localRect.right > localFrameRect.right )   localRect.right = localFrameRect.right;
	Rect	rect;
	rect.left	= localRect.left;
	rect.right	= localRect.right;
	rect.top	= localRect.top;
	rect.bottom	= localRect.bottom;
	RgnHandle	rgn = ::NewRgn();
	::RectRgn(rgn,&rect);
	err = ::HIViewSetNeedsDisplayInRegion(mControlRef,rgn,true);
	if( err != noErr )   _ACErrorNum("HIViewSetNeedsDisplayInRegion() returned error: ",err,this);
	::DisposeRgn(rgn);
	err = ::HIViewRender(mControlRef);
	if( err != noErr )   _ACErrorNum("HIViewRender() returned error: ",err,this);
	*/
}

/**
�`�惊�t���b�V���i�R���g���[���S�́j
*/
void	CUserPane::RefreshControl()
{
	if( IsControlVisible() == false )   return;
	ALocalRect	rect;
	GetControlLocalFrameRect(rect);
	RefreshRect(rect);
}

//#688
/**
�`�悷�ׂ��̈���������`�悷��(EVT_DoDraw()���s����)�i�o�b�t�@�ւ̕`��ƂȂ�j
*/
void	CUserPane::ExecuteDoDrawImmediately()
{
	//fprintf(stderr,"%16X ExecuteDoDrawImmediately() - Start\n",(int)(GetObjectID().val));
	
	//view���擾
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	
	//�`����s
	[view displayIfNeeded];
	
	//fprintf(stderr,"%16X ExecuteDoDrawImmediately() - End\n",(int)(GetObjectID().val));
}

#pragma mark ===========================

#pragma mark ---�`��p���擾

/**
�e�L�X�g�̈ʒu����`�掞��X���W�𓾂�
*/
ANumber	CUserPane::GetImageXByTextPosition( CTextDrawData& inTextDrawData, const AIndex inTextPosition )
{
	/*#1045
	//#572
	if( inTextDrawData.UTF16DrawText.GetItemCount() > kLineTextDrawLengthMax )
	{
		return 0;
	}
	*/
	
	/*#1034
	//TextLayout����
	if( inTextDrawData.GetTextLayout() == NULL )
	{
		inTextDrawData.SetTextLayout(CreateTextLayout(inTextDrawData.UTF16DrawText,inTextDrawData.IsFontFallbackEnabled()));
	}
	
	OSStatus	status = noErr;
	ATSUCaret	mainCaret, secondCaret;
	Boolean	caretIsSplit;//����
	*/
	UniCharArrayOffset	uniOffset = 0;
	if( inTextPosition < inTextDrawData.OriginalTextArray_UnicodeOffset.GetItemCount() )
	{
		uniOffset = inTextDrawData.OriginalTextArray_UnicodeOffset.Get(inTextPosition);
	}
	else
	{
		uniOffset = inTextDrawData.OriginalTextArray_UnicodeOffset.Get(inTextDrawData.OriginalTextArray_UnicodeOffset.GetItemCount()-1);
	}
	//�e�L�X�g�ʒu����X���W���擾
	CGFloat	offset2 = 0;
	CGFloat	offset = ::CTLineGetOffsetForStringIndex(GetCTLineFromTextDrawData(inTextDrawData),uniOffset,&offset2);
	return offset;
	
	/*#1034
	status = ::ATSUOffsetToPosition(inTextDrawData.GetTextLayout(),offset,true,&mainCaret,&secondCaret,&caretIsSplit);
	//�������iCTextDrawData�f�X�g���N�^�ō폜�����j::ATSUDisposeTextLayout(textLayout);
	//mainCaret.fX += ConvertFloatToFixed(0.5);
	return ::Fix2Long(mainCaret.fX);
	*/
}

/**
�e�L�X�g�̈ʒu����`�掞��X���W�𓾂�
*/
ANumber	CUserPane::GetImageXByUnicodeOffset( CTextDrawData& inTextDrawData, const AIndex inUnicodeOffset )
{
	/*#1045
	//#572
	if( inTextDrawData.UTF16DrawText.GetItemCount() > kLineTextDrawLengthMax )
	{
		return 0;
	}
	*/
	
	/*#1034
	//TextLayout����
	if( inTextDrawData.GetTextLayout() == NULL )
	{
		inTextDrawData.SetTextLayout(CreateTextLayout(inTextDrawData.UTF16DrawText,inTextDrawData.IsFontFallbackEnabled()));
	}
	
	OSStatus	status = noErr;
	ATSUCaret	mainCaret, secondCaret;
	Boolean	caretIsSplit;//����
	status = ::ATSUOffsetToPosition(inTextDrawData.GetTextLayout(),inUnicodeOffset,true,&mainCaret,&secondCaret,&caretIsSplit);
	//�������iCTextDrawData�f�X�g���N�^�ō폜�����j::ATSUDisposeTextLayout(textLayout);
	//mainCaret.fX += ConvertFloatToFixed(0.5);
	return ::Fix2Long(mainCaret.fX);
	*/
	
	//�e�L�X�g�ʒu����X���W���擾
	CGFloat	offset2 = 0;
	CGFloat	offset = ::CTLineGetOffsetForStringIndex(GetCTLineFromTextDrawData(inTextDrawData),inUnicodeOffset,&offset2);
	return offset;
}

/**
�`�掞��X���W����e�L�X�g�̈ʒu�𓾂�
*/
AIndex	CUserPane::GetTextPositionByImageX( CTextDrawData& inTextDrawData, const ANumber inImageX )
{
	/*#1045
	//#572
	if( inTextDrawData.UTF16DrawText.GetItemCount() > kLineTextDrawLengthMax )
	{
		return 0;
	}
	*/
	
	/*#1034
	//TextLayout����
	if( inTextDrawData.GetTextLayout() == NULL )
	{
		inTextDrawData.SetTextLayout(CreateTextLayout(inTextDrawData.UTF16DrawText,inTextDrawData.IsFontFallbackEnabled()));
	}
	//
	OSStatus	status = noErr;
	UniCharArrayOffset	primaryOffset,secondaryOffset;
	primaryOffset = 0;
	Boolean	isLeading;
	status = ::ATSUPositionToOffset(inTextDrawData.GetTextLayout(),::Long2Fix(inImageX-1),Long2Fix(0),&primaryOffset,&isLeading,&secondaryOffset);
	//�������iCTextDrawData�f�X�g���N�^�ō폜�����j::ATSUDisposeTextLayout(textLayout);
	
	return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(primaryOffset);
	*/
	
	//X���W����e�L�X�g�ʒu���擾
	CGPoint	point = {inImageX,0};
	//#1147 Mac OS X 10.11beta�΍�
	if( point.x <= 0 )
	{
		point.x = 0.1;
	}
	CGFloat	ascent = 9, descent = 3, leading = 3;
	double	width = ::CTLineGetTypographicBounds(GetCTLineFromTextDrawData(inTextDrawData),&ascent,&descent,&leading);
	if( point.x >= width )
	{
		point.x = width - 0.1;
	}
	//
	AIndex uniOffset = ::CTLineGetStringIndexForPosition(GetCTLineFromTextDrawData(inTextDrawData),point);
	if( uniOffset > 0 )
	{
		return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(uniOffset);
	}
	else
	{
		return 0;
	}
}

/**
�e�L�X�g�`�掞�̕����擾
*/
AFloatNumber	CUserPane::GetDrawTextWidth( const AText& inText ) 
{
	CTextDrawData	textDrawData(false);
	textDrawData.UTF16DrawText.SetText(inText);
	textDrawData.UTF16DrawText.ConvertFromUTF8ToUTF16();
	textDrawData.attrPos.Add(0);
	textDrawData.attrColor.Add(mColor);
	textDrawData.attrStyle.Add(mStyle);
	
	/*#1034
	textDrawData.SetTextLayout(CreateTextLayout(textDrawData.UTF16DrawText,true));
	ATSUTextMeasurement	start, end, ascent, descent;
	::ATSUGetUnjustifiedBounds(textDrawData.GetTextLayout(),kATSUFromTextBeginning,kATSUToTextEnd,&start,&end,&ascent,&descent);
	//�������iCTextDrawData�f�X�g���N�^�ō폜�����j::ATSUDisposeTextLayout(textLayout);
	return ConvertFixedToFloat(end-start);
	*/
	return GetDrawTextWidth(textDrawData);
}

/**
�e�L�X�g�`�掞�̕����擾
*/
AFloatNumber	CUserPane::GetDrawTextWidth( CTextDrawData& inTextDrawData ) 
{
	/*#1034
	//TextLayout����
	if( inTextDrawData.GetTextLayout() == NULL )
	{
		inTextDrawData.SetTextLayout(CreateTextLayout(inTextDrawData.UTF16DrawText,inTextDrawData.IsFontFallbackEnabled()));
	}
	//width�擾
	ATSUTextMeasurement	start, end, ascent, descent;
	::ATSUGetUnjustifiedBounds(inTextDrawData.GetTextLayout(),kATSUFromTextBeginning,kATSUToTextEnd,&start,&end,&ascent,&descent);
	return ConvertFixedToFloat(end-start);
	*/
	
	//�e�L�X�g���擾
	CGFloat	ascent = 9, descent = 3, leading = 3;
	double	width = ::CTLineGetTypographicBounds(GetCTLineFromTextDrawData(inTextDrawData),&ascent,&descent,&leading);
	return width;
}

/**
Draw���ׂ��̈�����ǂ����̔���
*/
ABool	CUserPane::IsRectInDrawUpdateRegion( const ALocalRect& inLocalRect ) const
{
	if( true )
	{
		//Cocoa�E�C���h�E�̏ꍇ
		//Cocoa�̏ꍇ�AdrawRect�̈�����rect�ł���Aregion�͋�api�Ȃ̂ŁAmDrawUpdateRegion�͎g�킸�A
		//mCocoaDrawDirtyRect���g���B
		
		ALocalRect	intersect = {0,0,0,0};
		return GetIntersectRect(inLocalRect,mCocoaDrawDirtyRect,intersect);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbon�E�C���h�E�̏ꍇ
		
		if( mDrawUpdateRegion == NULL )   {_ACError("not in APICB_DoDraw()",this);return false;}
		Rect	rect;
		rect.left	= inLocalRect.left;
		rect.top	= inLocalRect.top;
		rect.right	= inLocalRect.right;
		rect.bottom	= inLocalRect.bottom;
		//test
		/*AWindowRect	windowRect;
		GetWinConst().GetWindowRectFromControlLocalRect(GetControlID(),inLocalRect,windowRect);
		rect.left	= windowRect.left;
		rect.top	= windowRect.top;
		rect.right	= windowRect.right;
		rect.bottom	= windowRect.bottom;*/
		return ::RectInRgn(&rect,mDrawUpdateRegion);
	}
#endif
}

/**
���݂�TextProperty�ł̍s�̍������̏����擾
*/
void	CUserPane::GetMetricsForDefaultTextProperty( ANumber& outLineHeight, ANumber& outLineAscent ) 
{
	if( mLineHeight == 0 )
	{
		UpdateMetrics();
	}
	outLineHeight = mLineHeight;
	outLineAscent = mLineAscent;
}
//#1316
void	CUserPane::GetMetricsForDefaultTextProperty( AFloatNumber& outLineHeight, AFloatNumber& outLineAscent ) 
{
	if( mLineHeight == 0 )
	{
		UpdateMetrics();
	}
	outLineHeight = mLineHeight;
	outLineAscent = mLineAscent;
}

/**
�s�̍������̏��X�V
*/
void	CUserPane::UpdateMetrics()
{
	//#1090
	//���ł�metrics�擾�������Ƃ̂���t�H���g���E�t�H���g�T�C�Y�E�X�^�C���̑g�ݍ��킹�̏ꍇ�́A�L���b�V������擾����
	//�t�H���g���E�t�H���g�T�C�Y�E�X�^�C���̑g�ݍ��킹��key�擾
	AText	key;
	key.AddText(mFontName);
	key.Add(kUChar_Tab);
	key.AddNumber((ANumber)(mFontSize*10.0));
	key.Add(kUChar_Tab);
	key.AddNumber((ANumber)(mStyle));
	key.Add(kUChar_Tab);//#558
	key.AddNumber((ANumber)(mVerticalMode?1:0));//#558
	//�L���b�V������
	AIndex	cacheIndex = sMetricsCacheArray_Key.Find(key);
	if( cacheIndex != kIndex_Invalid )
	{
		//�L���b�V������擾
		mLineHeight = sMetricsCacheArray_LineHeight.Get(cacheIndex);
		mLineAscent = sMetricsCacheArray_LineAscent.Get(cacheIndex);
	}
	else
	{
		CTextDrawData	textDrawData(false);
		textDrawData.UTF16DrawText.SetText(mTextForMetricsCalculation);
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(mColor);
		textDrawData.attrStyle.Add(mStyle);
		CGFloat	ascent = 9, descent = 3, leading = 3;
		double	width = ::CTLineGetTypographicBounds(GetCTLineFromTextDrawData(textDrawData),&ascent,&descent,&leading);
		//leading������ƍs�̍����������Ȃ肷����̂ŁAascent+descent�ɂ���B
		mLineHeight = ceil(ascent+descent+2);
		mLineAscent = ceil(ascent+1);
		//�L���b�V���֒ǉ�
		sMetricsCacheArray_Key.Add(key);
		sMetricsCacheArray_LineHeight.Add(mLineHeight);
		sMetricsCacheArray_LineAscent.Add(mLineAscent);
	}
	
#if 0 
	OSStatus	err = noErr;
	
	ATSUTextMeasurement	ascent = ::Long2Fix(10);
	ATSUTextMeasurement	descent = ::Long2Fix(5);
	ATSUTextMeasurement	leading = ::Long2Fix(2);
	
	// ================ ascent�̎擾�iATSUGetUnjustifiedBounds()�Ŏ擾�j================ 
	
	//#636 ��փt�H���g���{���̃t�H���g�����T�C�Y���傫���Ȃ��āAascent���Ɏ��܂�Ȃ����Ƃ�����iHelvetica 11pt)
	//���Ƃ��΁AHelvetica 24pt�̏ꍇ�A�p���݂̂̃e�L�X�g��ATSUGetUnjustifiedBounds()�����ascent:18 pixel, descent:6 pixel�����A
	//���{��S�p���܂ނƁAascent:21.1 pixel, descent:14.9 pixel�ƂȂ�B
	//�΍�Ƃ��āA�P���܂ރe�L�X�g��ATSUGetUnjustifiedBounds()�����s���Aascent�͂�������g�p����
	//descent���܂ނƍs�̍����������Ȃ肷���i�s�Ԃ��󂫂����Ɍ�����j�̂ŁAdescent(+leading)��ATSUGetAttribute��
	//�擾�����l�̂ق����g�p����B
	CTextDrawData	textDrawData(false);
	textDrawData.UTF16DrawText.SetText(mTextForMetricsCalculation);
	textDrawData.attrPos.Add(0);
	textDrawData.attrColor.Add(mColor);
	textDrawData.attrStyle.Add(mStyle);
	textDrawData.SetTextLayout(CreateTextLayout(textDrawData.UTF16DrawText,true));
	ATSUTextMeasurement	start = 0, end = 0;
	::ATSUGetUnjustifiedBounds(textDrawData.GetTextLayout(),kATSUFromTextBeginning,kATSUToTextEnd,&start,&end,&ascent,&descent);
	
	// ================ descent, leading�̎擾�iATSUGetAttribute()�Ŏ擾�j================ 
	
	//�Ȃ��AATSUGetAttribute()�̓f�t�H���g�l�̂܂܂̏ꍇ�AkATSUNotSetErr��Ԃ����Ƃ�����
	ByteCount	actSize;
	//#636 ascent��ATSUGetUnjustifiedBounds()�Ŏ擾err = ::ATSUGetAttribute(mATSUTextStyle,kATSUAscentTag,sizeof(ATSUTextMeasurement),&ascent,&actSize);
	//if( err != noErr )   _ACErrorNum("ATSUGetAttribute() returned error: ",err,this);
	err = ::ATSUGetAttribute(/*#852 mATSUTextStyle*/GetATSUTextStyle(),kATSUDescentTag,sizeof(ATSUTextMeasurement),&descent,&actSize);
	//if( err != noErr )   _ACErrorNum("ATSUGetAttribute() returned error: ",err,this);
	err = ::ATSUGetAttribute(/*#852 mATSUTextStyle*/GetATSUTextStyle(),kATSULeadingTag,sizeof(ATSUTextMeasurement),&leading,&actSize);
	//if( err != noErr )   _ACErrorNum("ATSUGetAttribute() returned error: ",err,this);
	mLineHeight = ::Fix2Long(ascent) +::Fix2Long(descent) + ::Fix2Long(leading) +1;
	mLineAscent =::Fix2Long(ascent) +1;
#endif
}
//metrics�L���b�V��
AHashTextArray	CUserPane::sMetricsCacheArray_Key;//#1090
ANumberArray		CUserPane::sMetricsCacheArray_LineHeight;//#1090
ANumberArray		CUserPane::sMetricsCacheArray_LineAscent;//#1090

/**
���݂̃}�E�X�ʒu�擾
*/
/*win
void	CUserPane::GetMousePoint( ALocalPoint& outLocalPoint ) 
{
	StSetPort	s(GetWindowRef());
	Point	pt;
	::GetMouse(&pt);
	AWindowPoint	windowPoint;
	windowPoint.x = pt.h;
	windowPoint.y = pt.v;
	GetWin().GetControlLocalPointFromWindowPoint(GetControlID(),windowPoint,outLocalPoint);
}
*/

#pragma mark ===========================

#pragma mark ---�X�N���[��

/**
Pane�S�̂��X�N���[������

�Ȃ��APane��DragBox�ɂ��Ԃ����Ă���ƁA�Ȃ����A�X�N���[������UpdateRgn��DragBox�ȊO��Pane�S�̂ɂȂ��Ă��܂��̂ŁADragBox�Ƃ��Ԃ���Ȃ��悤�ɂ���K�v������B
*/
void	CUserPane::Scroll( const ANumber inDeltaX, const ANumber inDeltaY )
{
	GetWin().ScrollControl(GetControlID(),inDeltaX,inDeltaY);//#688
	/*#688
	OSStatus	err = noErr;
	
	err = ::HIViewScrollRect(mControlRef,NULL,inDeltaX,inDeltaY);
	if( err != noErr )   _ACErrorNum("HIViewScrollRect() returned error: ",err,this);
	*/
}

#pragma mark ===========================

#pragma mark ---Drag

//#236
/**
Drop�\�ɐݒ肷��
*/
void	CUserPane::EnableDrop( const AArray<AScrapType>& inScrapType )
{
	if( true )
	{
		//view���擾
		CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
		if( view == nil )   { _ACError("",NULL); return; }
		//scrap�^�C�v��array�ɐݒ�
		NSMutableArray *array = [[[NSMutableArray alloc] initWithCapacity:256] autorelease];
		for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
		{
			[array addObject:(ACocoa::GetPasteboardTypeNSString(inScrapType.Get(i)))];
		}
		//scrap�^�C�v�ݒ�
		[view registerForDraggedTypes:array];
	}
#if SUPPORT_CARBON
	else
	{
		GetWin().RegisterDropEnableControl(GetControlID());
		//#364
		mScrapTypeArray.DeleteAll();
		for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
		{
			mScrapTypeArray.Add(inScrapType.Get(i));
		}
	}
#endif
}

/**
�E�C���h�E�̈ꕔ�̃C���[�W��drag image�Ƃ���Drag���s
*/
ABool	CUserPane::DragTextWithWindowImage( const ALocalPoint& inMousePoint,
			const AArray<AScrapType>& inScrapType, const ATextArray& inData, 
			const AWindowRect& inWindowRect, const ANumber inImageWidth, const ANumber inImageHeight )
{
	//view���擾
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	if( view == nil )   { _ACError("",NULL); return false; }
	
	//Drag�ppasteboard�擾
	NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
	//scrap�^�C�v�ݒ�
	NSMutableArray *array = [[[NSMutableArray alloc] initWithCapacity:256] autorelease];
	for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
	{
		[array addObject:(ACocoa::GetPasteboardTypeNSString(inScrapType.Get(i)))];
	}
	[pboard declareTypes:array owner:view];
	//�f�[�^�iUTF-8�e�L�X�g�j�ݒ�
	//DragText()�ɓn�����f�[�^��#688�Ή��ɂ��UTF-8�e�L�X�g�ɕύX�i�O��UTF-16��legacy encoding�j
	for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
	{
		AText	data;
		inData.Get(i,data);
		//#931
		//���s�R�[�h��LF�֕ϊ�
		data.ConvertLineEndToLF();
		//
		AStCreateNSStringFromAText	datastr(data);
		[pboard setString:datastr.GetNSString() forType:(ACocoa::GetPasteboardTypeNSString(inScrapType.Get(i)))];
	}
	//
	//Image����
	NSView*	contentView = GetWin().GetCocoaObjects().GetContentView();
	NSData* theData = [contentView dataWithPDFInsideRect:ConvertFromWindowRectToNSRect(contentView,inWindowRect)];//��release�m�F
	NSPDFImageRep* pdfRep = [NSPDFImageRep imageRepWithData:theData];//��release�m�F
	NSImage* pdfImage = [[[NSImage alloc] initWithSize:NSMakeSize(inImageWidth,inImageHeight)] autorelease];
	[pdfImage addRepresentation:pdfRep];
	
	//Drag�̌��ʂƂ���view���폜�����\��������̂ŁA������retain���Ă����B�iautorelease�ɂ���ō폜�����j
	[[view retain] autorelease];
	//Dropped�t���OOFF
	sDropped = false;
	//Drag���s
	NSPoint	dragpt = {0};
	dragpt.x = inMousePoint.x;
	dragpt.y = inMousePoint.y;
	[view dragImage:pdfImage at:dragpt offset:NSMakeSize(0,0)
	event:[NSApp currentEvent] pasteboard:pboard source:view slideBack:YES];
	return sDropped;
}

//Dropped�t���O
ABool	CUserPane::sDropped = false;

/**
Drag����
@param inData UTF-8�e�L�X�g
*/
ABool	CUserPane::DragText( const ALocalPoint& inMousePoint, 
		const AArray<ALocalRect>& inDragRect, const AArray<AScrapType>& inScrapType, const ATextArray& inData,
		const AImageID inImageID )
{
	if( true )
	{
		//view���擾
		CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
		if( view == nil )   { _ACError("",NULL); return false; }
		
		//Drag�ppasteboard�擾
		NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
		//scrap�^�C�v�ݒ�
		NSMutableArray *array = [[[NSMutableArray alloc] initWithCapacity:256] autorelease];
		for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
		{
			[array addObject:(ACocoa::GetPasteboardTypeNSString(inScrapType.Get(i)))];
		}
		[pboard declareTypes:array owner:view];
		//�f�[�^�iUTF-8�e�L�X�g�j�ݒ�
		//DragText()�ɓn�����f�[�^��#688�Ή��ɂ��UTF-8�e�L�X�g�ɕύX�i�O��UTF-16��legacy encoding�j
		for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
		{
			AText	data;
			inData.Get(i,data);
			//#931
			//���s�R�[�h��LF�֕ϊ�
			data.ConvertLineEndToLF();
			//
			AStCreateNSStringFromAText	datastr(data);
			[pboard setString:datastr.GetNSString() forType:(ACocoa::GetPasteboardTypeNSString(inScrapType.Get(i)))];
		}
		//dragpt
		NSPoint	dragpt = {0};
		//Image����
		NSImage*	dragimage = nil;
		if( inImageID != kImageID_Invalid )
		{
			//Image�w��̏ꍇ
			
			//(0,0)
			dragpt.x = 0;
			dragpt.y = 0;
			//Image�擾
			AIndex	imageIndex = LoadImage(inImageID);
			dragimage = gImageArray.Get(imageIndex);
		}
		else
		{
			//Image���w��̏ꍇ
			
			dragpt.x = 0;
			dragpt.y = [view frame].size.height;
			//Image size��view�̃T�C�Y�ɂ���
			NSSize	imagesize = [view frame].size;
			dragimage = [[[NSImage alloc] initWithSize:imagesize] autorelease];
			//context�擾
			[dragimage lockFocus];
			mContextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
			::CGContextScaleCTM(mContextRef, 1.0, -1.0);
			//drag image��`��
			//lockFocusFlipped��10.6�ȍ~�����g���Ȃ��̂ŁA���̏�����flip�iimageheight-�j���s���Ă���B
			ANumber	imageheight = imagesize.height;
			AItemCount	dragRectItemCount = inDragRect.GetItemCount();
			if( dragRectItemCount > 0 )
			{
				AArray<ALocalPoint>	polyPoint;
				ALocalPoint	pt = {0};
				if( mVerticalMode == false )
				{
					//
					pt.x = inDragRect.Get(0).left;
					pt.y = imageheight - inDragRect.Get(0).top;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(0).right;
					pt.y = imageheight - inDragRect.Get(0).top;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(0).right;
					pt.y = imageheight - inDragRect.Get(dragRectItemCount-1).top;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(dragRectItemCount-1).right;
					pt.y = imageheight - inDragRect.Get(dragRectItemCount-1).top;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(dragRectItemCount-1).right;
					pt.y = imageheight - inDragRect.Get(dragRectItemCount-1).bottom;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(dragRectItemCount-1).left;
					pt.y = imageheight - inDragRect.Get(dragRectItemCount-1).bottom;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(dragRectItemCount-1).left;
					pt.y = imageheight - inDragRect.Get(0).bottom;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(0).left;
					pt.y = imageheight - inDragRect.Get(0).bottom;
					polyPoint.Add(pt);
				}
				else
				{
					//�c�������[�h�p #558
					//�erect�̍��W�n�ϊ�
					AArray<ALocalRect>	dragRect;
					for( AIndex i = 0; i < inDragRect.GetItemCount(); i++ )
					{
						ALocalRect	r = inDragRect.Get(i);
						r = GetWin().ConvertVerticalCoordinateFromAppToImp(GetControlID(),r);
						dragRect.Add(r);
					}
					//
					pt.x = dragRect.Get(0).right;
					pt.y = imageheight - dragRect.Get(0).top -2;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(0).right;
					pt.y = imageheight - dragRect.Get(0).bottom;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(dragRectItemCount-1).right;
					pt.y = imageheight - dragRect.Get(0).bottom;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(dragRectItemCount-1).right;
					pt.y = imageheight - dragRect.Get(dragRectItemCount-1).bottom;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(dragRectItemCount-1).left;
					pt.y = imageheight - dragRect.Get(dragRectItemCount-1).bottom;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(dragRectItemCount-1).left;
					pt.y = imageheight - dragRect.Get(dragRectItemCount-1).top -2;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(0).left;
					pt.y = imageheight - dragRect.Get(dragRectItemCount-1).top -2;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(0).left;
					pt.y = imageheight - dragRect.Get(0).top -2;
					polyPoint.Add(pt);
				}
				//
				FramePoly(polyPoint,kColor_Gray50Percent,0.5);
			}
			//context���
			mContextRef = NULL;
			[dragimage unlockFocus];
		}
		//Drag�̌��ʂƂ���view���폜�����\��������̂ŁA������retain���Ă����B�iautorelease�ɂ���ō폜�����j
		[[view retain] autorelease];
		//Dropped�t���OOFF
		sDropped = false;
		//Drag���s
		[view dragImage:dragimage at:dragpt offset:NSMakeSize(0,0)
		event:[NSApp currentEvent] pasteboard:pboard source:view slideBack:YES];
		return sDropped;
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		DragRef	dragRef;
		err = ::NewDrag(&dragRef);
		if( err != noErr )   _ACErrorNum("NewDrag() returned error: ",err,this);
		
		RgnHandle	rgnHandle = ::NewRgn();
		::OpenRgn();
		for( AIndex i = 0; i < inDragRect.GetItemCount(); i++ )
		{
			AGlobalRect	globalDragRect;
			GetWin().GetGlobalRectFromControlLocalRect(GetControlID(),inDragRect.Get(i),globalDragRect);
			Rect	rect;
			rect.left		= globalDragRect.left;
			rect.top		= globalDragRect.top;
			rect.right		= globalDragRect.right;
			rect.bottom		= globalDragRect.bottom;
			::FrameRect(&rect);
		}
		::CloseRgn(rgnHandle);
		RgnHandle	innerRgn = ::NewRgn();
		::CopyRgn(rgnHandle,innerRgn);
		::InsetRgn(innerRgn,1,1);
		::DiffRgn(rgnHandle,innerRgn,rgnHandle); 
		::DisposeRgn(innerRgn);
		Rect	regionBounds;
		::GetRegionBounds(rgnHandle,&regionBounds);
		::SetDragItemBounds(dragRef,1,&regionBounds);
		if( err != noErr )   _ACErrorNum("SetDragItemBounds() returned error: ",err,this);
		
		AGlobalPoint	globalMousePoint;
		GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),inMousePoint,globalMousePoint);
		
		for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
		{
			AText	data;
			inData.Get(i,data);
			AStTextPtr	ptr(data,0,data.GetItemCount());
			err = ::AddDragItemFlavor(dragRef,1,inScrapType.Get(i),ptr.GetPtr(),ptr.GetByteCount(),0);
			if( err != noErr )   _ACErrorNum("AddDragItemFlavor() returned error: ",err,this);
		}
		
		EventRecord	event;
		event.what = mouseDown;
		event.message = 0;
		event.when = TickCount();
		event.where.h = globalMousePoint.x;
		event.where.v = globalMousePoint.y;
		
		err = ::TrackDrag(dragRef,&event,rgnHandle);
		if( err != noErr )   _ACErrorNum("TrackDrag() returned error: ",err,this);
		err = ::DisposeDrag(dragRef);
		if( err != noErr )   _ACErrorNum("DisposeDrag() returned error: ",err,this);
	}
#endif
}

#pragma mark ===========================

#pragma mark ---Contextual menu

/**
�R���e�L�X�g���j���[�\��
*/
void	CUserPane::ShowContextMenu( const AContextMenuID inContextMenuID, const AGlobalPoint& inMousePoint )
{
	//view�擾
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	if( view == nil )   { _ACError("",NULL); return; }
	//context menu�擾
	AMenuRef	menuRef = AApplication::GetApplication().NVI_GetMenuManager().GetContextMenu(inContextMenuID);
	//context menu�\��
	CAppImp::SetContextMenuTarget(GetAWin().GetObjectID(),mControlID);
	[NSMenu popUpContextMenu:(ACocoa::GetNSMenu(menuRef)) withEvent:[NSApp currentEvent] forView:view];
	CAppImp::SetContextMenuTarget(kObjectID_Invalid,kControlID_Invalid);
}

#pragma mark ===========================

#pragma mark ---�C�x���g�n���h���i�e��C�x���g��������Cocoa����̃R�[���o�b�N�j
//#688

/*
CocoaUserPaneView���̊e�R�[���o�b�N�i-mouseDown:���j��CWindowImp���̃��\�b�h�������̃��\�b�h
�̃��[�g�ŃR�[�������B
*/

/**
�`��
*/
void	CUserPane::APICB_CocoaDrawRect( const ALocalRect& inDirtyRect )
{
	//�R���g���[����Visible�łȂ���Ή������Ȃ�
	if( IsControlVisible() == false )   return;
	
	//Context�ݒ�
	mCocoaDrawDirtyRect = inDirtyRect;
	mContextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	
	//��L�Ŏ擾����Context�̍��W�n�́AHIView�̍��オ���_�Ay���̃v���X�͉������ƂȂ��Ă���B
	//�������AQuartz�ł́Ay���̃v���X��������ł��邱�Ƃ��O��ƂȂ��Ă���A
	//���̂܂܂��ƁAText��s�N�`���̕`�悪�㉺�t���܂ɂȂ��Ă��܂��B
	//�����������邽�߁Ay���̃v���X��������ɂȂ�悤�ɁA���L�������s���B
	//�i���̂��߁AContext�ւ̕`��́AHIView�̍��オ���_�ł͂��邪�Ay�̓v���X�}�C�i�X���t�ɕϊ�����K�v������B�j
	::CGContextScaleCTM(mContextRef, 1.0, -1.0);
	
	//#558
	//�c�������W�n�ɂ���
	if( mVerticalMode == true )
	{
		//���W�n�ϊ�
		ChangeToVerticalCTM(mContextRef);
		//dirty rect���W�n�ϊ�
		mCocoaDrawDirtyRect = mWindowImp.ConvertVerticalCoordinateFromImpToApp(GetControlID(),inDirtyRect);
	}
	
	//AWindow::EVT_DoDraw()���s
	GetAWin().EVT_DoDraw(GetControlID());
	
	//Context������
	mCocoaDrawDirtyRect = kLocalRect_0000;
	mContextRef = NULL;
}

#if SUPPORT_CARBON

#pragma mark ===========================

#pragma mark ---�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j�iCarbon�j

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus	CUserPane::STATIC_APICB_DrawHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(UP-DH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( CWindowImp::sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;
	
	OSStatus	result = eventNotHandledErr;
	ControlRef	controlRef = NULL;
	err = ::GetEventParameter(inEventRef,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&controlRef);
	if( controlRef == NULL )
	{
		_ACError("cannot get control",NULL);
		return result;
	}
	CGContextRef	contextRef = NULL;
	//CGContext�擾
	//�Ȃ��A�E�C���h�E��Composite�ɂ���K�v������B�łȂ��ƁACGContext�͎擾�ł��Ȃ��B�icontextRef��NULL�ƂȂ�j
	err = ::GetEventParameter(inEventRef,kEventParamCGContextRef,typeCGContextRef,NULL,sizeof(CGContextRef),NULL,&(contextRef));
	if( contextRef == NULL )
	{
		_ACError("cannot get context (window is not composite?)",NULL);
		return result;
	}
	//
	//���[�J�����W�Ŏ擾�����
	RgnHandle	rgnHandle = NULL;
	err = ::GetEventParameter(inEventRef,kEventParamRgnHandle,typeQDRgnHandle,NULL,sizeof(RgnHandle),NULL,&rgnHandle);
	
	//CUserPane�I�u�W�F�N�g�C���X�^���X�̃C�x���g�n���h�����s
	try
	{
		//��L�Ŏ擾����Context�̍��W�n�́AHIView�̍��オ���_�Ay���̃v���X�͉������ƂȂ��Ă���B
		//�������AQuartz�ł́Ay���̃v���X��������ł��邱�Ƃ��O��ƂȂ��Ă���A
		//���̂܂܂��ƁAText��s�N�`���̕`�悪�㉺�t���܂ɂȂ��Ă��܂��B
		//�����������邽�߁Ay���̃v���X��������ɂȂ�悤�ɁA���L�������s���B
		//�i���̂��߁AContext�ւ̕`��́AHIView�̍��オ���_�ł͂��邪�Ay�̓v���X�}�C�i�X���t�ɕϊ�����K�v������B�j
		::CGContextScaleCTM(contextRef, 1.0, -1.0);
		AControlID	controlID = window->GetControlID(controlRef);
		window->GetUserPane(controlID).mContextRef = contextRef;
		window->GetUserPane(controlID).mDrawUpdateRegion = rgnHandle;
		if( window->GetUserPane(controlID).APICB_DoDraw() == true )
		{
			result = noErr;
		}
		window->GetUserPane(controlID).mContextRef = NULL;
		window->GetUserPane(controlID).mDrawUpdateRegion = NULL;
	}
	catch(...)
	{
		_ACError("CUserPane::STATIC_APICB_DrawHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[UP-DH]");
	return result;
}

/**
�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
*/
pascal OSStatus CUserPane::STATIC_APICB_ControlBoundsChangedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(UP-CBCH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( CWindowImp::sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;
	
	OSStatus	result = eventNotHandledErr;
	ControlRef	controlRef = NULL;
	err = ::GetEventParameter(inEventRef,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&controlRef);
	if( controlRef == NULL )
	{
		_ACError("cannot get control",NULL);
		return result;
	}
	
	//CUserPane�I�u�W�F�N�g�C���X�^���X�̃C�x���g�n���h�����s
	try
	{
		AControlID	controlID = window->GetControlID(controlRef);
		if( window->GetUserPane(controlID).APICB_DoControlBoundsChanged(inEventRef) == true )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CUserPane::STATIC_APICB_ControlBoundsChangedHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[UP-CBCH]");
	return result;
}

//�C�x���g�n���h���iOS�R�[���o�b�N�pstatic�j
/*B0000 080810 pascal OSStatus CUserPane::STATIC_APICB_MouseExitedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(UP-MEH)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImp�I�u�W�F�N�g�̐����`�F�b�N
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( CWindowImp::sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;
	
	OSStatus	result = eventNotHandledErr;
	ControlRef	controlRef = NULL;
	::GetEventParameter(inEventRef,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&controlRef);
	if( controlRef == NULL )
	{
		_ACError("cannot get control",NULL);
		return result;
	}
	
	//CUserPane�I�u�W�F�N�g�C���X�^���X�̃C�x���g�n���h�����s
	try
	{
		AControlID	controlID = window->GetControlID(controlRef);
		if( window->GetUserPane(controlID).DoMouseExited(inEventRef) == true )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CUserPane::STATIC_APICB_MouseExitedHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[UP-MEH]");
	return result;
}
*/

/**
�C�x���g�n���h���C���X�g�[��
*/
void CUserPane::InstallEventHandler()
{
	//
	OSStatus	err;
	EventHandlerRef	handlerRef;
	EventTypeSpec	typeSpec;
	typeSpec.eventClass = kEventClassControl;
	typeSpec.eventKind = kEventControlDraw;
	err = ::InstallEventHandler(::GetControlEventTarget(GetControlRef()),::NewEventHandlerUPP(CUserPane::STATIC_APICB_DrawHandler),
			1,&typeSpec,&mWindowImp,&handlerRef);
	if( err != noErr )   _ACError("",this);
	
	typeSpec.eventClass = kEventClassControl;
	typeSpec.eventKind = kEventControlBoundsChanged;
	err = ::InstallEventHandler(::GetControlEventTarget(GetControlRef()),::NewEventHandlerUPP(CUserPane::STATIC_APICB_ControlBoundsChangedHandler),
			1,&typeSpec,&mWindowImp,&handlerRef);
	if( err != noErr )   _ACError("",this);
	
}
#endif

#pragma mark ===========================

#pragma mark ---�c����

//#558
/**
�c�����p���W�n�ϊ�
*/
void	CUserPane::ChangeToVerticalCTM( CGContextRef inContextRef )
{
	ALocalRect	localRect = {0};
	GetControlLocalFrameRect(localRect);
	CGContextRotateCTM(inContextRef,-M_PI/2.0);
	CGContextTranslateCTM(inContextRef,0,localRect.right);
}

//#558
/**
�c�����ݒ�
*/
void	CUserPane::SetVerticalMode( const ABool inVerticalMode )
{
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
	{
		//�c�����t���O�ݒ�
		mVerticalMode = inVerticalMode;
		
		//CocoaUserPaneView�ɏc�������[�h�ݒ�
		CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
		[view setVerticalMode:inVerticalMode];
		
		//metrics�X�V
		UpdateMetrics();
	}
}

//#1309
/**
�T�[�r�X���j���[��
*/
void	CUserPane::SetServiceMenuAvailable( const ABool inAvailable )
{
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	[view setServiceMenuAvailable:inAvailable];
}

