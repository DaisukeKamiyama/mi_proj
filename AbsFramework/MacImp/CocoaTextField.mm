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

#import "CocoaTextField.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"

@implementation CocoaTextField

#pragma mark ===========================
#pragma mark [�N���X]CocoaTextField
#pragma mark ===========================

#pragma mark ===========================

#pragma mark ---�����ݒ�

/**
�����ݒ�
*/
- (void)setup:(CWindowImp*)inWindowImp
{
	mWindowImp = inWindowImp;
}

#pragma mark ===========================

#pragma mark ---�e�L�X�g�ύX������

/**
�e�L�X�g�ύX�������i�ꕶ�����͂��ƂɃR�[�������j
*/
- (void)textDidChange:(NSNotification *)inNotification
{
	//�p���������s
	[super textDidChange:inNotification];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_TextInputted()���s
	mWindowImp->APICB_CocoaDoTextFieldTextChanged([self tag]);
	
	OS_CALLBACKBLOCK_END;
}

@end


