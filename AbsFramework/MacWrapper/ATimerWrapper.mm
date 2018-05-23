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

ATimerWrapper

*/

#include "ATimerWrapper.h"

/**
�L�����b�g�u�����N�^�C�}�[�l�擾
*/
ANumber	ATimerWrapper::GetCaretTime()
{
	//583/291=2
	//GetCaretTime()��34��Ԃ��Ă������A���肪�C�R�[�������̕s�����������̂ŁA���ۂɂ�35�񂾂����B
	//35��583ms
	return (35*1000.0/60.0/kTickMsec);//=10
	
#if SUPPORT_CARBON
	return ::GetCaretTime();
#endif
}

/**
���݂�TickCount�擾
*/
/*#688
ATickCount	ATimerWrapper::GetTickCount()
{
	return ::TickCount();
}
*/
