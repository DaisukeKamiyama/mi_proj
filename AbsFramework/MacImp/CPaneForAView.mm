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

CPaneForAView

*/

#include "CPaneForAView.h"
//#1012 #include "StSetPort.h"
#include "../Frame.h"
#include "CWindowImpCocoaObjects.h"
#import "CocoaUserPaneView.h"

#pragma mark ===========================
#pragma mark [�N���X]CPaneForAView
#pragma mark ===========================
//AView�p�R���g���[��

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
CPaneForAView::CPaneForAView( AObjectArrayItem* inParent, CWindowImp& inWindow, const AControlID inControlID )//#688const AControlRef inControlRef ) 
: CUserPane(inParent,inWindow,inControlID)//#688inControlRef)
{
	/*#852
	AText	fontname;//win
	AFontWrapper::GetDialogDefaultFontName(fontname);//win #375
	SetDefaultTextProperty(fontname,9.0,kColor_Black,kTextStyle_Normal,true);
	*/
/* 	HIViewTrackingAreaRef	ref;
	::HIViewNewTrackingArea(GetControlRef(),NULL,1,&ref);*/
}

/**
�f�X�g���N�^
*/
CPaneForAView::~CPaneForAView()
{
}

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---�C�x���g�n���h��

/**
�`��callback
*/
ABool	CPaneForAView::APICB_DoDraw()
{
	if( IsControlVisible() == false )   return true;
	//Quartz���S�ڍs�܂ł�Port���ݒ肷��
	//#1012 StSetPort	s(GetWindowRef());
	//�iAWindow���o�R���āj�Ή�����AView�̃C�x���g�n���h�������s
	GetAWin().EVT_DoDraw(GetControlID());
	return true;
}

/**
�}�E�X�N���b�N�ɑ΂��鏈��
*/
ABool	CPaneForAView::DoMouseDown( const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	if( IsControlVisible() == false )   return true;
	//Quartz���S�ڍs�܂ł�Port���ݒ肷��
	//#1012 StSetPort	s(GetWindowRef());
	//�E�C���h�E���W�n�̃N���b�N�ʒu�擾
	HIPoint	windowPoint;
	err = ::GetEventParameter(inEventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(windowPoint),NULL,&windowPoint);
	if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	//UserPane�̈ʒu�擾
	AWindowPoint	controlPoint;
	GetControlPosition(controlPoint);
	//���[�J�����W�n�iUserPane�̍�������_�Ƃ�����W�n�j�ɕϊ�
	ALocalPoint	localPoint;
	/*#688
	err = ::HIViewConvertPoint(&windowPoint,NULL,GetControlRef());
	if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
	localPoint.x = windowPoint.x;
	localPoint.y = windowPoint.y;
	*/
	GetWinConst().GetControlLocalPointFromEventParamWindowMouseLocation(GetControlID(),windowPoint,localPoint);//#688
	//�L�[�擾
	UInt32	modifiers = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	//�N���b�N�񐔎擾
	UInt32	clickCount = 1;
	err = ::GetEventParameter(inEventRef,kEventParamClickCount,typeUInt32,NULL,sizeof(clickCount),NULL,&clickCount);
	//�R���e�N�X�g���j���[�C�x���g�Ȃ�AEVT_DoContextMenu()���R�[�����Ă݂�
	if( ::IsShowContextualMenuEvent(inEventRef) == true )
	{
		if( GetAWin().EVT_DoContextMenu(GetControlID(),localPoint,modifiers,clickCount) == true )
		{
			return true;
		}
	}
	//�iAWindow���o�R���āj�Ή�����AView��IIFCB_DoMouseDown()�����s
	if( GetAWin().EVT_DoMouseDown(GetControlID(),localPoint,modifiers,clickCount) == false )//#232 �Ԃ�l��false�̏ꍇ����EVT_Clicked()���s
	{
		//
		if( clickCount == 1 )
		{
			GetAWin().EVT_Clicked(GetControlID(),modifiers);
		}
		else if( clickCount == 2 )
		{
			GetAWin().EVT_Clicked(GetControlID(),modifiers);//B0303 �E�C���h�E�A�N�e�B�x�[�g�N���b�N���P��ڂɂȂ��Ă��܂��AEVT_Clicked()���ĂׂȂ����Ƃ�����̂�
			GetAWin().EVT_DoubleClicked(GetControlID());
		}
	}
	//�iAWindow���o�R���āj�Ή�����AView��EVT_GetCursorType()�����s
	ACursorWrapper::SetCursor(GetAWin().EVT_GetCursorType(GetControlID(),localPoint,modifiers));
	return true;
}

/**
�}�E�X�z�C�[��callback
*/
ABool	CPaneForAView::DoMouseWheel( const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	if( IsControlVisible() == false )   return true;
	long	delta = 0;
	err = ::GetEventParameter(inEventRef,kEventParamMouseWheelDelta,typeLongInteger,NULL,sizeof(long),NULL,&delta);
	EventMouseWheelAxis	axis = kEventMouseWheelAxisY;
	err = ::GetEventParameter(inEventRef,kEventParamMouseWheelAxis,typeMouseWheelAxis,NULL,sizeof(EventMouseWheelAxis),NULL,&axis);
	ANumber	deltaX = 0;
	ANumber	deltaY = 0;
	if( axis == kEventMouseWheelAxisY )
	{
		deltaY = -delta;
	}
	else
	{
		deltaX = -delta;
	}
	//Quartz���S�ڍs�܂ł�Port���ݒ肷��
	//#1012 StSetPort	s(GetWindowRef());
	//�L�[�擾
	UInt32	modifiers = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	//�E�C���h�E���W�n�̃}�E�X�ʒu�擾
	HIPoint	windowPoint;
	err = ::GetEventParameter(inEventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(windowPoint),NULL,&windowPoint);
	if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	//UserPane�̈ʒu�擾
	AWindowPoint	controlPoint;
	GetControlPosition(controlPoint);
	//���[�J�����W�n�iUserPane�̍�������_�Ƃ�����W�n�j�ɕϊ�
	ALocalPoint	localPoint;
	/*#688
	err = ::HIViewConvertPoint(&windowPoint,NULL,GetControlRef());
	if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
	localPoint.x = windowPoint.x;
	localPoint.y = windowPoint.y;
	*/
	GetWinConst().GetControlLocalPointFromEventParamWindowMouseLocation(GetControlID(),windowPoint,localPoint);//#688
	//�iAWindow���o�R���āj�Ή�����AView�̃C�x���g�n���h�������s
	GetAWin().EVT_DoMouseWheel(GetControlID(),deltaX,deltaY,modifiers,localPoint);
	return true;
}

/**
�}�E�X�ړ�����callback
*/
ABool	CPaneForAView::DoMouseMoved( const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	if( IsControlVisible() == false )   return true;
	//Quartz���S�ڍs�܂ł�Port���ݒ肷��
	//#1012 StSetPort	s(GetWindowRef());
	//�E�C���h�E���W�n�̃}�E�X�ʒu�擾
	HIPoint	windowPoint;
	err = ::GetEventParameter(inEventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(windowPoint),NULL,&windowPoint);
	if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	//UserPane�̈ʒu�擾
	AWindowPoint	controlPoint;
	GetControlPosition(controlPoint);
	//���[�J�����W�n�iUserPane�̍�������_�Ƃ�����W�n�j�ɕϊ�
	ALocalPoint	localPoint;
	/*#688
	err = ::HIViewConvertPoint(&windowPoint,NULL,GetControlRef());
	if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
	localPoint.x = windowPoint.x;
	localPoint.y = windowPoint.y;
	*/
	GetWinConst().GetControlLocalPointFromEventParamWindowMouseLocation(GetControlID(),windowPoint,localPoint);//#688
	//�L�[�擾
	UInt32	modifiers = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	//B0000 080810
	//MouseTracking
	if( mEnableMouseLeaveEvent == true )
	{
		CAppImp::SetMouseTracking(GetWin().GetWindowRef(),GetControlID());
	}
	//B0000 080810
	DoAdjustCursor(inEventRef);//MouseMoved��true�ŕԂ���AdjustCursor��OS���痈�Ȃ��̂ŁA�����Ŏ��s
	//
	return GetAWin().EVT_DoMouseMoved(GetControlID(),localPoint,modifiers);
}

/**
�J�[�\������callback
*/
ABool	CPaneForAView::DoAdjustCursor( const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	if( IsControlVisible() == false )   return true;
	//Quartz���S�ڍs�܂ł�Port���ݒ肷��
	//#1012 StSetPort	s(GetWindowRef());
	//�E�C���h�E���W�n�̃}�E�X�ʒu�擾
	HIPoint	windowPoint;
	err = ::GetEventParameter(inEventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(windowPoint),NULL,&windowPoint);
	if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	//UserPane�̈ʒu�擾
	AWindowPoint	controlPoint;
	GetControlPosition(controlPoint);
	//���[�J�����W�n�iUserPane�̍�������_�Ƃ�����W�n�j�ɕϊ�
	ALocalPoint	localPoint;
	/*#688
	err = ::HIViewConvertPoint(&windowPoint,NULL,GetControlRef());
	if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
	localPoint.x = windowPoint.x;
	localPoint.y = windowPoint.y;
	*/
	GetWinConst().GetControlLocalPointFromEventParamWindowMouseLocation(GetControlID(),windowPoint,localPoint);//#688
	//�L�[�擾
	UInt32	modifiers = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	//�iAWindow���o�R���āj�Ή�����AView��EVT_GetCursorType()�����s
	ACursorWrapper::SetCursor(GetAWin().EVT_GetCursorType(GetControlID(),localPoint,modifiers));
	return true;
}

/**
�̈�ύX�C�x���g�n���h��
*/
ABool	CPaneForAView::APICB_DoControlBoundsChanged( const EventRef inEventRef )
{
	GetAWin().EVT_DoControlBoundsChanged(GetControlID());
	return true;
}

/**
DragTracking callback
*/
void	CPaneForAView::DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )
{
	//#364 EnableDrop()�œo�^����ScrapType�̂ݎ󂯕t����
	ABool	found = false;
	for( AIndex i = 0; i < mScrapTypeArray.GetItemCount(); i++ )
	{
		if( AScrapWrapper::ExistDragData(inDragRef,mScrapTypeArray.Get(i)) == true )
		{
			found = true;
			break;
		}
	}
	if( found == false )   return;
	//
	GetAWin().EVT_DoDragTracking(GetControlID(),inDragRef,inMousePoint,inModifierKeys,outIsCopyOperation);
}

/**
DragEnter callback
*/
void	CPaneForAView::DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys )
{
	//#364 EnableDrop�œo�^����ScrapType�̂ݎ󂯕t����
	ABool	found = false;
	for( AIndex i = 0; i < mScrapTypeArray.GetItemCount(); i++ )
	{
		if( AScrapWrapper::ExistDragData(inDragRef,mScrapTypeArray.Get(i)) == true )
		{
			found = true;
			break;
		}
	}
	if( found == false )   return;
	//
	GetAWin().EVT_DoDragEnter(GetControlID(),inDragRef,inMousePoint,inModifierKeys);
}

/**
DragLeave callback
*/
void	CPaneForAView::DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys )
{
	//#364 EnableDrop�œo�^����ScrapType�̂ݎ󂯕t����
	ABool	found = false;
	for( AIndex i = 0; i < mScrapTypeArray.GetItemCount(); i++ )
	{
		if( AScrapWrapper::ExistDragData(inDragRef,mScrapTypeArray.Get(i)) == true )
		{
			found = true;
			break;
		}
	}
	if( found == false )   return;
	//
	GetAWin().EVT_DoDragLeave(GetControlID(),inDragRef,inMousePoint,inModifierKeys);
}

/**
DragReceive callback
*/
void	CPaneForAView::DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys )
{
	//#364 EnableDrop�œo�^����ScrapType�̂ݎ󂯕t����
	ABool	found = false;
	for( AIndex i = 0; i < mScrapTypeArray.GetItemCount(); i++ )
	{
		if( AScrapWrapper::ExistDragData(inDragRef,mScrapTypeArray.Get(i)) == true )
		{
			found = true;
			break;
		}
	}
	if( found == false )   return;
	//
	GetAWin().EVT_DoDragReceive(GetControlID(),inDragRef,inMousePoint,inModifierKeys);
}

//B0000 080810
/**
�}�E�X�̈�O�ړ�callback
*/
ABool	CPaneForAView::DoMouseExited( const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//R0228
	if( IsControlVisible() == false )   return true;
	//Quartz���S�ڍs�܂ł�Port���ݒ肷��
	//#1012 StSetPort	s(GetWindowRef());
	//�E�C���h�E���W�n�̃N���b�N�ʒu�擾
	HIPoint	windowPoint;
	err = ::GetEventParameter(inEventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(windowPoint),NULL,&windowPoint);
	//UserPane�̈ʒu�擾
	AWindowPoint	controlPoint;
	GetControlPosition(controlPoint);
	//���[�J�����W�n�iUserPane�̍�������_�Ƃ�����W�n�j�ɕϊ�
	ALocalPoint	localPoint;
	/*#688
	err = ::HIViewConvertPoint(&windowPoint,NULL,GetControlRef());
	if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
	localPoint.x = windowPoint.x;
	localPoint.y = windowPoint.y;
	*/
	GetWinConst().GetControlLocalPointFromEventParamWindowMouseLocation(GetControlID(),windowPoint,localPoint);//#688
	//
	GetAWin().EVT_DoMouseLeft(GetControlID(),localPoint);
	return true;
}
#endif
//#138
/**
�R���g���[��Show/Hide
*/
void	CPaneForAView::PreProcess_SetShowControl( const ABool inShow )//#688
{
	/*#688
	//�p�������F�R���g���[���ɑ΂��鏈��
	CUserPane::SetShowControl(inShow);
	*/
	//AbsFrame�C�x���g�ʒm
	GetAWin().EVT_PreProcess_SetShowControl(GetControlID(),inShow);
}

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---�R���g���[���t�H�[�J�X����

//#135
/**
�t�H�[�J�X�ݒ莞��CWindowImp����R�[�������
*/
void	CPaneForAView::SetFocus()
{
	GetAWin().EVT_DoViewFocusActivated(GetControlID());
}

//#135
/**
�t�H�[�J�X����
*/
void	CPaneForAView::ResetFocus()
{
	GetAWin().EVT_DoViewFocusDeactivated(GetControlID());
}
#endif
#pragma mark ===========================

//R0231
#pragma mark ---����

/**
�����\��
*/
void	CPaneForAView::ShowDictionary( const AText& inWord, const ALocalPoint inLocalPoint ) const
{
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_6 )
	{
		//#1026 Mac OS X 10.9�ȍ~��HIDictionaryWindowShow()�����삵�Ȃ��̂ŁACocoa��API�ɒu�������ishowDefinitionForAttributedString��10.6�ȍ~�j
		
		//�ΏےP���AttributedString����
		NSMutableAttributedString *attrString = [[NSMutableAttributedString alloc] initWithString:inWord.CreateNSString(true)];
		[attrString autorelease];
		
		//NSMutableAttributedString�̕ҏW�J�n
		[attrString beginEditing];
		//range�ݒ�
		NSRange	range = NSMakeRange(0,[attrString length]);
		//�ʏ�t�H���g
		[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetNormalFont() range:range];
		if( GetVerticalMode() == true )
		{
			[attrString addAttribute:NSVerticalGlyphFormAttributeName value:[NSNumber numberWithInt:1] range:range];
		}
		//NSMutableAttributedString�̕ҏW�I��
		[attrString endEditing];
		
		//CocoaUserPaneView�擾
		CocoaUserPaneView*	view = GetWinConst().GetCocoaObjects().FindCocoaUserPaneViewByTag(GetControlID());
		if( view == nil )   {_ACError("",NULL);return;}
		//���W�ϊ�
		ALocalPoint	localPoint = inLocalPoint;
		if( GetVerticalMode() == true )
		{
			localPoint = GetWinConst().ConvertVerticalCoordinateFromAppToImp(GetControlID(),localPoint);
		}
		//�����\��
		NSPoint	pt = {localPoint.x,localPoint.y};
		[view showDefinitionForAttributedString:attrString atPoint:pt];
	}
	else
	{
		CFStringRef	str = inWord.CreateCFString();
		if( str == NULL )   {_ACError("",NULL);return;}
		CFRange	sel;
		sel.location = 0;
		sel.length = ::CFStringGetLength(str);
		/*#688
		AText	fontname;
		AFontWrapper::GetFontName(mFont,fontname);
		 */
		CFStringRef	fontnamestr = mFontName.CreateCFString();
		if( fontnamestr == NULL ) {_ACError("",NULL);::CFRelease(str);return;}
		CTFontRef	font = ::CTFontCreateWithName(fontnamestr,mFontSize,NULL);
		AGlobalPoint	globalpt;
		GetWinConst().GetGlobalPointFromControlLocalPoint(GetControlID(),inLocalPoint,globalpt);
		CGPoint	pt;
		pt.x = globalpt.x;
		pt.y = globalpt.y;
		::HIDictionaryWindowShow(NULL,str,sel,font,pt,false,NULL);
		::CFRelease(font);
		::CFRelease(fontnamestr);
		::CFRelease(str);
	}
}




