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

CocoaMacro

*/

/**
OS�R�[���o�b�N�u���b�N�J�n�����i�Ԃ�l�Ȃ����j
�A�v���P�[�V������퓮�쒆�`�F�b�N�A�C�x���g�g�����U�N�V�����O�����^�㏈���ACWindowImp���݃`�F�b�N
*/
#define OS_CALLBACKBLOCK_START(periodic) if( AApplication::NVI_GetEnableAPICBTraceMode() == true )\
{ fprintf(stderr,"(START:");fprintf(stderr,"%s",__FILE__);fprintf(stderr,"%d",__LINE__);fprintf(stderr,")"); }\
if( AApplication::GetApplication().NVI_IsWorking() == false ) { return; }\
AStEventTransactionPrePostProcess	oneevent(periodic);\
if( CWindowImp::ExistWindowImp(mWindowImp) == false )\
{ _ACError("CWindowImp object does not alive.",NULL); return; } try { @try{

/**
OS�R�[���o�b�N�u���b�N�J�n�����i�Ԃ�l���莞�j
�A�v���P�[�V������퓮�쒆�`�F�b�N�A�C�x���g�g�����U�N�V�����O�����^�㏈���ACWindowImp���݃`�F�b�N
*/
#define OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(periodic,retval) if( AApplication::NVI_GetEnableAPICBTraceMode() == true )\
{ fprintf(stderr,"(START:");fprintf(stderr,"%s",__FILE__);fprintf(stderr,"%d",__LINE__);fprintf(stderr,")"); }\
if( AApplication::GetApplication().NVI_IsWorking() == false ) { return retval; }\
AStEventTransactionPrePostProcess	oneevent(periodic);\
if( CWindowImp::ExistWindowImp(mWindowImp) == false )\
{ _ACError("CWindowImp object does not alive.",NULL); return retval; } try { @try{

/**
OS�R�[���o�b�N�u���b�N�J�n�����iCWindowImp�Ȃ����j
�A�v���P�[�V������퓮�쒆�`�F�b�N�A�C�x���g�g�����U�N�V�����O�����^�㏈��
*/
#define OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP(periodic) if( AApplication::NVI_GetEnableAPICBTraceMode() == true )\
{ fprintf(stderr,"(START:");fprintf(stderr,"%s",__FILE__);fprintf(stderr,"%d",__LINE__);fprintf(stderr,")"); }\
if( AApplication::GetApplication().NVI_IsWorking() == false ) { return; }\
AStEventTransactionPrePostProcess	oneevent(periodic); try { @try{

//#1056
/**
OS�R�[���o�b�N�u���b�N�J�n�����iCWindowImp�Ȃ����E�Ԃ�l���莞�j
�A�v���P�[�V������퓮�쒆�`�F�b�N�A�C�x���g�g�����U�N�V�����O�����^�㏈��
*/
#define OS_CALLBACKBLOCK_START_WITHOUTWINDOWIMP_WITHERRRETURNVALUE(periodic,retval) if( AApplication::NVI_GetEnableAPICBTraceMode() == true )\
{ fprintf(stderr,"(START:");fprintf(stderr,"%s",__FILE__);fprintf(stderr,"%d",__LINE__);fprintf(stderr,")"); }\
if( AApplication::GetApplication().NVI_IsWorking() == false ) { return retval; }\
AStEventTransactionPrePostProcess	oneevent(periodic); try { @try{

/**
OS�R�[���o�b�N�u���b�N�I������
*/
#define OS_CALLBACKBLOCK_END } @catch(NSException *theException) { _ACError("OS CALLBACKBLOCK @catch",NULL); } \
} catch(...)\
{ _ACError("OS CALLBACKBLOCK caught exception.",NULL); }\
if( AApplication::NVI_GetEnableAPICBTraceMode() == true )\
{ fprintf(stderr,"(START:");fprintf(stderr,"%s",__FILE__);fprintf(stderr,"%d",__LINE__);fprintf(stderr,")"); }

/**
�ʏ�L�[�A�N�V�����p�����}�N��
mKeyHandled��false�Ȃ�AAPICB_CocoaDoTextInput()�����s
*/
#define GENERAL_KEYACTION(keyaction) if( [((AppDelegate*)[NSApp delegate]) isKeyHandled] == NO ) {\
AText	text; ACocoa::SetTextFromNSString([[NSApp currentEvent] characters],text);\
AKeyBindKey	key = ACocoa::GetKeyBindKeyFromNSEvent([NSApp currentEvent],text);\
AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]);\
[((AppDelegate*)[NSApp delegate]) setKeyHandled:((mWindowImp->APICB_CocoaDoTextInput(mControlID,text,key,modifier,keyaction))?YES:NO)]; };


