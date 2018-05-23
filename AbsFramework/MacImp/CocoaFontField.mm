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

CocoaWindow

*/

#import "CocoaFontField.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"

@implementation CocoaFontField

#pragma mark ===========================
#pragma mark [�N���X]CocoaFontField
#pragma mark ===========================

#pragma mark ===========================

#pragma mark ---�����ݒ�

/**
�����ݒ�
*/
- (void)setup:(CWindowImp*)inWindowImp
{
	mWindowImp = inWindowImp;
	fontSizeData = 9.0;
}

#pragma mark ===========================

#pragma mark ---

/**
�t�H���g�ύX�{�^���N���b�N�������iIB�ɂāA���̊֐����A�t�H���g�ύX�{�^������action�łȂ��j
*/
- (IBAction) doFontChangeButton:(id)sender
{
	//���݂�(TextField��)�t�H���g���t�H���g�p�l���ɐݒ肷��
	[[NSFontManager sharedFontManager] setSelectedFont:[self font] isMultiple:NO];
	//�t�H���g�p�l���\��
	[[NSFontPanel sharedFontPanel] orderFront:nil];
	//delegate, target�������ɐݒ肷��
	[[NSFontManager sharedFontManager] setDelegate:self];
	[[NSFontManager sharedFontManager] setTarget:self];
}

/**
�t�H���g�ύX�������iNSFontManager��delegate�j
*/
- (void)changeFont:(id)sender
{
	//Apple�h�L�������g�ɋL�ڂ���Ă���R�[�h
	NSFont *oldFont = [self font];
	NSFont *newFont = [sender convertFont:oldFont];
	[self setFont:newFont];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_ValueChanged()���s
	mWindowImp->APICB_CocoaDoFontFieldChanged([self tag]);
	
	OS_CALLBACKBLOCK_END;
}

/**
�t�H���g�ݒ�
*/
- (void)setFont:(NSFont *)font
{
	//�t�H���g�T�C�Y�f�[�^�L���i�\���t�H���g�͏�����肾���A������͏�������j
	fontSizeData = [font pointSize];
	//�\���t�H���g�ݒ�i�������j
	CGFloat	fontsize = fontSizeData;
	if( fontsize > 16.0 )   fontsize = 16.0;
	NSFont	*displayFont = [NSFont fontWithName:[font fontName] size:fontsize];
	[super setFont:displayFont];
	//�t�H���g�e�L�X�g�ݒ�
	NSString*	str = [[[NSString alloc] initWithFormat:@"%@, %1.1f pt",[font displayName],[font pointSize]] autorelease];
	[self setStringValue:str];
}

/**
�t�H���g�T�C�Y�f�[�^�擾
*/
- (CGFloat)getFontSizeData
{
	return fontSizeData;
}

@end


