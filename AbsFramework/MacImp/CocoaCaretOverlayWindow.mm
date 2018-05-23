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

CocoaCaretOverlayWindow

*/

#import "CocoaCaretOverlayWindow.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"

@implementation CocoaCaretOverlayWindow

#pragma mark ===========================
#pragma mark [�N���X]CocoaCaretOverlayWindow
#pragma mark ===========================

/**
�}�E�X�C�x���g���������Ȃ��i���߂���j
���ꂪ�Ȃ��ƁA�L�����b�g����N���b�N�����Ƃ��ɁA�L�����b�g���}�E�X�C�x���g���������Ă��܂��A���C���E�C���h�E�Ƀ}�E�X�C�x���g���ʒm����Ȃ��B
*/
- (BOOL)ignoresMouseEvents
{
	return YES;
}

@end


@implementation CocoaCaretOverlayView

/**
 ������
 */
- (id)initWithFrame:(NSRect)frameRect
{
	//�`��f�[�^����
	drawCaretLine = false;
	drawCaretRect = false;
	//�p���������s
	self = [super initWithFrame:frameRect];
	if( self != nil )
	{
	}
	return self;
}

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

//���L�����b�g���ӂ̍X�V�̈�
const ANumber	kCaretLineUpdateMargin = 2;

/**
�L�����b�g�`��f�[�^�폜
*/
- (void)clearCaret
{
	//������CGContextScaleCTM()�͈͊O�Ȃ̂ŁA���W�n��ALocalPoint�Ɠ����B
	
	//�����͈́i�X�V�͈́j
	NSRect	updateRect = {0};
	//���`��f�[�^���݂��Ă���������ӂ������͈́i�X�V�͈́j�ɐݒ�
	if( drawCaretLine == true )
	{
		//�L�����b�g���ӂ�rect�v�Z
		updateRect.origin.x = GetMinValue(caretStartPoint.x,caretEndPoint.x) - kCaretLineUpdateMargin;
		updateRect.origin.y = GetMinValue(caretStartPoint.y,caretEndPoint.y) - kCaretLineUpdateMargin;
		updateRect.size.width = abs(caretEndPoint.x-caretStartPoint.x) + kCaretLineUpdateMargin*2;
		updateRect.size.height = abs(caretEndPoint.y-caretStartPoint.y) + kCaretLineUpdateMargin*2;
		//���`��f�[�^���݃t���OOFF
		drawCaretLine = false;
	}
	//rect�`��f�[�^���݂��Ă�����rect���������͈́i�X�V�͈́j�ɐݒ�
	if( drawCaretRect == true )
	{
		//view�S�̂��X�V�͈͂ɐݒ�
		updateRect = [self bounds];
		//rect�`��f�[�^���݃t���OOFF
		drawCaretRect = false;
	}
	//�`��X�V
	[self setNeedsDisplayInRect:updateRect];
}

/**
�L�����b�g�`��f�[�^�ݒ�i���j
*/
- (void)setCaretLineFrom:(ALocalPoint)spt to:(ALocalPoint)ept width:(ANumber)width color:(AColor)color isDash:(ABool)isDash isShow:(ABool)isShow
{
	//������CGContextScaleCTM()�͈͊O�Ȃ̂ŁA���W�n��ALocalPoint�Ɠ����B
	
	//���݂̃L�����b�g������
	[self clearCaret];
	
	//show�̏ꍇ�́A�`��f�[�^�ݒ�
	if( isShow == true )
	{
		//�`��f�[�^�ݒ�
		drawCaretLine = true;
		caretStartPoint = spt;
		caretEndPoint = ept;
		caretLineWidth = width;
		caretColor = color;
		caretIsDashed = isDash;
		//�`��X�Vrect
		NSRect	updateRect = {0};
		//�L�����b�g���ӂ�rect�v�Z
		updateRect.origin.x = GetMinValue(caretStartPoint.x,caretEndPoint.x) - kCaretLineUpdateMargin;
		updateRect.origin.y = GetMinValue(caretStartPoint.y,caretEndPoint.y) - kCaretLineUpdateMargin;
		updateRect.size.width = abs(caretEndPoint.x-caretStartPoint.x) + kCaretLineUpdateMargin*2;
		updateRect.size.height = abs(caretEndPoint.y-caretStartPoint.y) + kCaretLineUpdateMargin*2;
		//�`��X�V
		[self setNeedsDisplayInRect:updateRect];
	}
}

/**
�L�����b�g�`��f�[�^�ݒ�irect�j
*/
- (void)setCaretRect:(ALocalRect)rect width:(ANumber)width color:(AColor)color isShow:(ABool)isShow
{
	//������CGContextScaleCTM()�͈͊O�Ȃ̂ŁA���W�n��ALocalPoint�Ɠ����B
	
	//���݂̃L�����b�g������
	[self clearCaret];
	
	//show�̏ꍇ�́A�`��f�[�^�ݒ�
	if( isShow == true )
	{
		//�`��f�[�^�ݒ�
		drawCaretRect = true;
		caretRect = rect;
		caretLineWidth = width;
		caretColor = color;
		caretIsDashed = false;
		//�`��X�V
		[self setNeedsDisplayInRect:[self bounds]];
	}
}

/**
�`��
*/
- (void)drawRect:(NSRect)dirtyRect
{
	//Context�擾
	CGContextRef	contextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	::CGContextSaveGState(contextRef);
	::CGContextScaleCTM(contextRef, 1.0, -1.0);
	
	//view���S�����i�Ƃ͂����Ă��AsetNeedsDisplayInRect�Ŏw�肵��invalid rect�͈͓̔��݂̂�draw�����B�j
	CGRect	eraseRect = {0};
	eraseRect.origin.x = 0;
	eraseRect.origin.y = -[self frame].size.height;
	eraseRect.size.width = [self frame].size.width;
	eraseRect.size.height = [self frame].size.height;
	::CGContextClearRect(contextRef,eraseRect);
	
	//�`��f�[�^��������΁A���^�[��
	if( drawCaretLine == false && drawCaretRect == false )
	{
		return;
	}
	
	/*�e�X�g�p����
	CGRect	rect = {0};
	rect.origin.x = 0;
	rect.origin.y = -eraseRect.size.height;
	rect.size.width = eraseRect.size.width;
	rect.size.height = eraseRect.size.height;
	::CGContextSetRGBFillColor(contextRef,1,0,0,1);
	::CGContextFillRect(contextRef,rect);
	*/
	
	//�F�擾
	float	red = caretColor.red;
	float	green = caretColor.green;
	float	blue = caretColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	if( drawCaretLine == true )
	{
		//���`��
		::CGContextBeginPath(contextRef);
		::CGContextSetRGBStrokeColor(contextRef,red,green,blue,1.0);
		::CGContextSetLineWidth(contextRef,caretLineWidth);
		::CGContextSetShouldAntialias(contextRef,false);
		::CGContextMoveToPoint(contextRef,caretStartPoint.x,-caretStartPoint.y-1);
		::CGContextAddLineToPoint(contextRef,caretEndPoint.x,-caretEndPoint.y-1);
		if( caretIsDashed == true )
		{
			float	lengths[2];
			lengths[0] = 1.0;
			lengths[1] = 1.0;
			::CGContextSetLineDash(contextRef,0.0,lengths,2);
		}
		::CGContextStrokePath(contextRef);
	}
	if( drawCaretRect == true )
	{
		//rect�`��
		::CGContextSetRGBStrokeColor(contextRef,red,green,blue,1.0);
		::CGContextSetLineWidth(contextRef,caretLineWidth);
		::CGContextSetShouldAntialias(contextRef,false);
		CGRect	r = {0};
		r.origin.x = caretRect.left;
		r.origin.y = -caretRect.bottom;
		r.size.width = caretRect.right - caretRect.left;
		r.size.height = caretRect.bottom - caretRect.top;
		::CGContextStrokeRect(contextRef,r);
	}
	
	//graphic state restore
	::CGContextRestoreGState(contextRef);
}

@end


