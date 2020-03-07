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

ATextUndoer

*/

#include "stdafx.h"

#include "ATextUndoer.h"
#include "ADocument_Text.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]ATextUndoer
#pragma mark ===========================
//Undo�����N���X
#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
ATextUndoer::ATextUndoer( ADocument_Text& inTextDocument ) : mTextDocument(inTextDocument)
,mDeletedTextArray(NULL)//#693,true)//#417
{
	mUndoMode = false;
	mUndoPosition = 0;
	//�����\�t���O�i�����t���O�j
	mCanCat = false;
	//�����֎~�t���O�i�O������̎w���ŃZ�b�g�����B�����̃t���O��ԂɊւ�炸�A����̌������֎~����B�������A�����Undo/Redo�Ńt���O���������B�j
	//#210 ���g�p�̂��߃R�����g�A�E�gmNoContinue = false;//B0090 �������͂��A���̍Ō�̉��������𕶎��I�����āA�������́i�u���j����ƁA�A���h�D���ɍŏ��̕������͂̑O�̏�Ԃɖ߂���Ή�
}

#pragma mark ===========================

#pragma mark ---�L�^

//�A�N�V�����^�O���L�^
void	ATextUndoer::RecordActionTag( const AUndoTag inAction )
{
	//�A�N�V�����^�O�����������̂��폜
	DeleteDanglingActionTag();
	
	//�A�N�V�����^�O�ǉ�
	if( IsActionTag(inAction) == true )
	{
		AddUndoRecord(inAction,0,0,kObjectID_Invalid);//#216
	}
}

//�e�L�X�g�}�����L�^
void	ATextUndoer::RecordInsertText( const ATextIndex inInsertTextIndex, const AItemCount inLength )
{
	//Undo���s���Ȃ牽�����Ȃ�
	if( mUndoMode == true )   return;
	
	//#267
	ABool	inhibitConcat = false;
	AIndex	currentLastEditRecordPos = GetCurrentLastEditRecordPosition();
	if( currentLastEditRecordPos != kIndex_Invalid )
	{
		inhibitConcat = mUndoRecord_InhibitConcat.Get(currentLastEditRecordPos);
	}
	
	//�}�����R�[�h�ǉ�
	AddUndoRecord(undoTag_InsertText,inInsertTextIndex,inInsertTextIndex+inLength,kObjectID_Invalid);//#216
	
	//#267
	if( inhibitConcat == true )   return;
	
	//�����\�t���O��Off�Ȃ�A�t���OOn�ɂ���B�������A����͌������Ȃ��B
	if( mCanCat == false )
	{
		mCanCat = true;
	}
	//�����֎~�t���OOff���ATag��3�ȏ㑶�݂��A���݂̏�����Typing�Ȃ猋��������s��
	else if( /*#210 ���g�p�̂��߃R�����g�A�E�g(mNoContinue == false) &&*/
				(mUndoRecord_Tag.GetItemCount() >= 3) && (GetCurrentAction() == undoTag_Typing) )
	{
		//�O���UndoRecord��InsertText�ŁA�I�������ʒu������̊J�n�����ʒu�Ɠ����Ȃ�A�܂Ƃ߂ĂP��Record�ɂ���
		
		/*
		UndoRecord�̍\��
		[�O���ActionTag]
		[�O���InsertText]<-GetUndoRecordItemCount()-3
		[�����ActionTag]<-GetUndoRecordItemCount()-2
		[�����InsertText]
		*/
		//prevAction�ɑO���ActionTag�̓��e���擾����
		AUndoTag	prevAction = undoTag_NOP;
		for( AIndex i = GetUndoRecordItemCount()-3; i >= 0; i-- )
		{
			prevAction = mUndoRecord_Tag.Get(i);
			if( prevAction >= undoTag_Typing )   break;
		}
		//�O���ActionTag��Typing�Ȃ猋�����葱�s
		if( prevAction == undoTag_Typing )
		{
			//���̃��W���[���Œǉ��������R�[�h���1�O����k����InsertText/DeleteText���R�[�h�̏ꏊ��T��
			//���ꂪ������⃌�R�[�h�ƂȂ�B�C���f�b�N�X��prevRecordIndex�Ɋi�[�����B
			//���[�v��ActionTag���R�[�h���Ƃ΂����߁B
			AIndex	prevRecordIndex = GetUndoRecordItemCount()-2;
			for( ; prevRecordIndex >= 0; prevRecordIndex-- )
			{
				AUndoTag	tag = mUndoRecord_Tag.Get(prevRecordIndex);
				if( tag == undoTag_InsertText || tag == undoTag_DeleteText )   break;
			}
			//������⃌�R�[�h��Tag��InsertText�ŁA���A���̏I���ʒu���A����ǉ��������R�[�h�̊J�n�ʒu�ɓ������Ȃ�A�P���R�[�h�Ɍ�������
			if( (mUndoRecord_Tag.Get(prevRecordIndex) == undoTag_InsertText) && 
						(/*#216mUndoRecord_Par*/mUndoRecord_Epos.Get(prevRecordIndex) == inInsertTextIndex) )
			{
				//������⃌�R�[�h�ȍ~���폜
				DeleteUndoRecord(prevRecordIndex+1,GetUndoRecordItemCount()-(prevRecordIndex+1));
				//�����Ώۃ��R�[�h�̏I���ʒu���X�V����
				/*#216mUndoRecord_Par*/mUndoRecord_Epos.Set(GetUndoRecordItemCount()-1,inInsertTextIndex+inLength);
				//Undo�ʒu���X�V
				mUndoPosition = GetUndoRecordItemCount();
			}
		}
	}
	//�����֎~�t���OOff
	//#210 ���g�p�̂��߃R�����g�A�E�g mNoContinue = false;//B0090
}

//�e�L�X�g�폜���L�^
void	ATextUndoer::RecordDeleteText( const ATextIndex inStart, const ATextIndex inEnd )
{
	//Undo���s���Ȃ牽�����Ȃ�
	if( mUndoMode == true )   return;
	
	//#267
	ABool	inhibitConcat = false;
	AIndex	currentLastEditRecordPos = GetCurrentLastEditRecordPosition();
	if( currentLastEditRecordPos != kIndex_Invalid )
	{
		inhibitConcat = mUndoRecord_InhibitConcat.Get(currentLastEditRecordPos);
	}
	
	//�폜�\��e�L�X�g���擾���AmDeletedTextArray�֊i�[
	AIndex	deletedTextIndex = mDeletedTextArray.AddNewObject();
	GetTextDocument().SPI_GetText(inStart,inEnd,mDeletedTextArray.GetObject(deletedTextIndex));
	AObjectID	deletedTextObjectID = mDeletedTextArray.GetObject(deletedTextIndex).GetObjectID();
	
	//�폜���R�[�h�ǉ�
	AddUndoRecord(undoTag_DeleteText,inStart,inStart,deletedTextObjectID);//#216
	
	//#267
	if( inhibitConcat == true )   return;
	
	if( false )
	{
		//
		mCanCat = true;
		//�����֎~�t���OOff
		//#210 ���g�p�̂��߃R�����g�A�E�g mNoContinue = false;//B0090
	}
	else
	{
		//�����\�t���O��Off�Ȃ�A�t���OOn�ɂ���B�������A����͌������Ȃ��B
		if( mCanCat == false )
		{
			mCanCat = true;
		}
		//#210
		//�����֎~�t���OOff���ATag��3�ȏ㑶�݂��A���݂̏�����BSKey�Ȃ猋��������s��
		else if( /*#210 ���g�p�̂��߃R�����g�A�E�g (mNoContinue == false) &&*/
					(mUndoRecord_Tag.GetItemCount() >= 3) && 
					((GetCurrentAction() == undoTag_BSKey)/*||GetCurrentAction() == undoTag_Typing*/) )
		{
			/*
			UndoRecord�̍\��
			[�O���ActionTag]
			[�O���DeleteText]<-GetUndoRecordItemCount()-3
			[�����ActionTag]<-GetUndoRecordItemCount()-2
			[�����DeleteText]
			*/
			//prevAction�ɑO���ActionTag�̓��e���擾����
			AUndoTag	prevAction = undoTag_NOP;
			for( AIndex	i = GetUndoRecordItemCount()-3; i >= 0; i-- )
			{
				prevAction = mUndoRecord_Tag.Get(i);
				if( prevAction >= undoTag_Typing )   break;
			}
			//�O���ActionTag�������ActionTag�Ɠ����Ȃ猋�����葱�s
			if( prevAction == GetCurrentAction() )
			{
				//���̃��W���[���Œǉ��������R�[�h���1�O����k����InsertText/DeleteText���R�[�h�̏ꏊ��T��
				//���ꂪ������⃌�R�[�h�ƂȂ�B�C���f�b�N�X��prevRecordIndex�Ɋi�[�����B
				//���[�v��ActionTag���R�[�h���Ƃ΂����߁B
				AIndex	prevRecordIndex = GetUndoRecordItemCount()-2;
				for( ; prevRecordIndex >= 0; prevRecordIndex-- )
				{
					AUndoTag	tag = mUndoRecord_Tag.Get(prevRecordIndex);
					if( tag == undoTag_InsertText || tag == undoTag_DeleteText )   break;
				}
				//ActionTag�ɂ���ď�������
				switch(prevAction) 
				{
					/*
					#210�Ή����A���\�[�X����ڐA�������A�����̕����͕s�v�Ɏv����̂ŃR�����g�A�E�g�����B
					������A�e�X�g������Ă��Ȃ��B
					�u������I�����āA�i�㏑���Łj�������͂����ꍇ�v�̓��[�U�[�̑���Ƃ��āh���܂łƂ͕ʂ̑���h�ł���\���������̂ŁA
					�������Ȃ��ق����x�^�[���Ǝv����B
					�܂��A������ǉ�����ƁAB0090�̖��΍�Ƃ��āA�����I�����ɊO������mNoContinue��true�ɂ��鏈�����K�v�B���G�ɂȂ���Ƀ����b�g���Ȃ��B
					mNoContinue�́A���܂ŏ��false�Ŏg���Ă��Ȃ������̂ŁA����A�폜����B
					//������I�����āA�i�㏑���Łj�������͂����ꍇ
				  case undoTag_Typing:
					{
						if( (mUndoRecord_Tag.Get(prevRecordIndex) == undoTag_InsertText) &&
									(mUndoRecord_Epos.Get(prevRecordIndex) == inEnd) && 
									(mUndoRecord_Spos.Get(prevRecordIndex) <= inStart) ) 
						{
							//����ǉ��������R�[�h���폜
							DeleteUndoRecord(prevRecordIndex+1,GetUndoRecordItemCount()-(prevRecordIndex+1));
							//�����Ώۃ��R�[�h�̏I���ʒu���X�V����
							mUndoRecord_Epos.Set(GetUndoRecordItemCount()-1,
									mUndoRecord_Epos.Get(GetUndoRecordItemCount()-1)-(inEnd-inStart));
							//Undo�ʒu���X�V
							mUndoPosition = GetUndoRecordItemCount();
						}
						break;
					}
					*/
					//BSKey��A�������ꍇ
				  case undoTag_BSKey:
					{
						//������⃌�R�[�h��Tag��DeleteText�ŁA���A���̊J�n�ʒu���A
						//����ǉ��������R�[�h�̏I���ʒu�ɓ������Ȃ�A�P���R�[�h�Ɍ�������
						/*
						���Ƃ��΁A"abc"�̉E�[����BSKey2�񉟂����ꍇ�A
						[BSKey]
						[2,"c"]��������⃌�R�[�h�i�J�n�ʒu:2������폜�̏I���ʒu�j
						[BSKey]
						[1,"b"]
						deletedText:"b"
						�����P�F"b"��"c"�̑O�ɑ}��
						�����Q�F�J�n�ʒu:2��1�ɕύX�i"b"�̒����������O�ɕύX�j
						�i���Ȃ݂ɁAActionTag�����������ł������ł����C�����邪�A���̏����Ŏ��т����邽�߁A����������̂܂܎g�����ƂƂ����B�j
						*/
						if( (mUndoRecord_Tag.Get(prevRecordIndex) == undoTag_DeleteText) &&
									(mUndoRecord_Spos.Get(prevRecordIndex) == inEnd) )
						{
							//������⃌�R�[�h�ȍ~���폜
							DeleteUndoRecord(prevRecordIndex+1,GetUndoRecordItemCount()-(prevRecordIndex+1));
							//�����Ώۃ��R�[�h�̊J�n�ʒu�ƁADeletedText���X�V����
							//����폜�e�L�X�g�擾
							AText	deletedText;
							GetTextDocument().SPI_GetText(inStart,inEnd,deletedText);
							//�����Ώۂ�DeletedText��ObjectID�擾
							AObjectID	prevRecordDeletedTextObjectID = mUndoRecord_DeletedTextObjectID.Get(prevRecordIndex);
							//DeletedText������
							mDeletedTextArray.GetObjectByID(prevRecordDeletedTextObjectID).InsertText(0,deletedText);
							//�J�n�ʒu���X�V
							mUndoRecord_Spos.Set(GetUndoRecordItemCount()-1,
									mUndoRecord_Spos.Get(GetUndoRecordItemCount()-1)-(inEnd-inStart));
							//Undo�ʒu���X�V
							mUndoPosition = GetUndoRecordItemCount();
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
	}
}

#pragma mark ===========================

#pragma mark ---Undo/Redo���s

/**
Undo���s
@return true�̏ꍇ�́A�A�N�V�������A�����đ��݂��Ă��邽�߁Aredraw�𖢎��s�Ȃ̂ŁA�R�[������redraw�K�v
*/
ABool	ATextUndoer::Undo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	//#219
	outSelectTextIndex = kIndex_Invalid;
	outSelectTextLength = 0;
	
	//�A�N�V�����^�O�����������̂��폜
	DeleteDanglingActionTag();
	
	//Undo�s�Ȃ烊�^�[��
	if( mUndoPosition <= 0 )   return false;
	
	//#846
	//�A�����Ă���A�N�V���������擾
	AItemCount	totalActionCount = 0;
	for( AIndex pos = mUndoPosition-1; pos > 0; pos-- )
	{
		if( IsActionTag(mUndoRecord_Tag.Get(pos)) == true )   break;
		totalActionCount++;
	}
	//�A�N�V��������100��葽���ꍇ�Aredraw�͎��s�����A�R�[�����ł܂Ƃ߂čs���B
	ABool	redrawOnceMode = (totalActionCount>100);
	//���[�_���Z�b�V�����i�L�����Z���s�j
	AStEditProgressModalSession	modalSession(kEditProgressType_Undo,false,false,true);
	try
	{
		//
		AIndex	startUndoPosition = mUndoPosition-1;
		//�O�̃A�N�V�����^�O�܂ł�Reverse���s
		mUndoMode = true;
		for( mUndoPosition--; mUndoPosition > 0; mUndoPosition-- )
		{
			//#1502
			AStAutoReleasePool	pool;
			//#846
			//�v���O���X�\���i�L�����Z���͕s�j
			GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Undo,0,true,startUndoPosition-mUndoPosition,totalActionCount);
			
			//�A�N�V�����^�O�̈ʒu�܂ŗ����烋�[�v�I��
			if( IsActionTag(mUndoRecord_Tag.Get(mUndoPosition)) == true )   break;
			
			//mUndoPosition�ʒu�̑}���^�폜���R�[�h���A�t�]���Ď��s���A���R�[�h���t�]���ĕۑ�
			ReverseTextAction(mUndoPosition,outSelectTextIndex,outSelectTextLength,redrawOnceMode);
		}
		mUndoMode = false;
	}
	catch(...)
	{
		_ACError("",NULL);
		mUndoMode = false;
	}
	//
	return redrawOnceMode;
}

//Redo���s
ABool	ATextUndoer::Redo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	//#219
	outSelectTextIndex = kIndex_Invalid;
	outSelectTextLength = 0;
	
	//Redo�s�Ȃ烊�^�[��
	if( mUndoPosition >= GetUndoRecordItemCount() )   return false;
	
	//#846
	//�A�����Ă���A�N�V���������擾
	AItemCount	totalActionCount = 0;
	for( AIndex pos = mUndoPosition+1; pos < GetUndoRecordItemCount(); pos++ )
	{
		if( IsActionTag(mUndoRecord_Tag.Get(pos)) == true )   break;
		totalActionCount++;
	}
	//�A�N�V��������100��葽���ꍇ�Aredraw�͎��s�����A�R�[�����ł܂Ƃ߂čs���B
	ABool	redrawOnceMode = (totalActionCount>100);
	//���[�_���Z�b�V�����i�L�����Z���s�j
	AStEditProgressModalSession	modalSession(kEditProgressType_Redo,false,false,true);
	try
	{
		//
		AIndex	startUndoPosition = mUndoPosition+1;
		//�O�̃A�N�V�����^�O�܂ł�Reverse���s
		mUndoMode = true;
		for( mUndoPosition++; mUndoPosition < GetUndoRecordItemCount(); mUndoPosition++ )
		{
			//#1502
			AStAutoReleasePool	pool;
			//#846
			//�v���O���X�\���i�L�����Z���͕s�j
			GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Redo,0,true,mUndoPosition-startUndoPosition,totalActionCount);
			
			//�A�N�V�����^�O�̈ʒu�܂ŗ����烋�[�v�I��
			if( IsActionTag(mUndoRecord_Tag.Get(mUndoPosition)) == true )   break;
			
			//mUndoPosition�ʒu�̑}���^�폜���R�[�h���A�t�]���Ď��s���A���R�[�h���t�]���ĕۑ�
			ReverseTextAction(mUndoPosition,outSelectTextIndex,outSelectTextLength,redrawOnceMode);
		}
		mUndoMode = false;
	}
	catch(...)
	{
		_ACError("",NULL);
		mUndoMode = false;
	}
	//
	return redrawOnceMode;
}

//�w��ʒu�̑}���^�폜���R�[�h���A�t�]���Ď��s���A���R�[�h���t�]���ĕۑ�
void	ATextUndoer::ReverseTextAction( const AIndex inIndex, AIndex& ioSelectTextIndex, AItemCount& ioSelectTextLength,
		const ABool inRedrawOnceMode )//#219 
{
	//#219
	AIndex	resultSpos = ioSelectTextIndex;
	AIndex	resultEpos = ioSelectTextIndex+ioSelectTextLength;
	//
	AUndoTag	tag = mUndoRecord_Tag.Get(inIndex);
	if( tag == undoTag_InsertText )
	{
		AIndex	spos = mUndoRecord_Spos.Get(inIndex);
		AIndex	epos = /*#216 mUndoRecord_Par*/mUndoRecord_Epos.Get(inIndex);
		
		//�폜�\��e�L�X�g���擾���AmDeletedTextArray�֊i�[
		AIndex	deletedTextIndex = mDeletedTextArray.AddNewObject();
		GetTextDocument().SPI_GetText(spos,epos,mDeletedTextArray.GetObject(deletedTextIndex));
		AObjectID	deletedTextObjectID = mDeletedTextArray.GetObject(deletedTextIndex).GetObjectID();
		
		//
		mUndoRecord_Tag.Set(inIndex,undoTag_DeleteText);
		/*#216 mUndoRecord_Par*/mUndoRecord_DeletedTextObjectID.Set(inIndex,deletedTextObjectID);
		
		//Reverse���s�����ꍇ�A���̌�A���̏����ɑ����ē��폈�����s���Ă��������Ȃ�
		mCanCat = false;//#210 ==�o�O�C��
		
		GetTextDocument().SPI_DeleteText(spos,epos,inRedrawOnceMode,inRedrawOnceMode);
		//#219 outSelectTextIndex = spos;
		//#219 outSelectTextLength = 0;
		
		//#219 �����v�]������Ȃ�ݒ�œ����������
		if( false )
		{
			//Undo/Redo��ɑ}���������I������ݒ�̏ꍇ
			//�i�\���Ƀe�X�g���Ă��܂���j
			
			//1.�Ō���폜�����ꍇ
			// �]����{resultSpos,resultEpos}�ŁAresultEpos==epos�ł���Aspos��reultSpos������̏ꍇ�A
			// �I��͈͂����΂߂�i�I���ʒu���X�V����j
			if( resultEpos == epos && resultSpos < spos )
			{
				resultEpos = spos;
			}
			//2.��L�ȊO
			//����ȊO�̏ꍇ�A�폜�����ʒu��resultSpos(=resultEpos)�ɂ���B
			else
			{
				resultSpos = spos;
				resultEpos = spos;
			}
		}
		else
		{
			//Undo/Redo��ɑ}��������̍Ō�ɃL�����b�g�ړ�����ݒ�̏ꍇ
			
			resultSpos = spos;
			resultEpos = spos;
		}
	}
	else if( tag == undoTag_DeleteText )
	{
		AIndex	spos = mUndoRecord_Spos.Get(inIndex);
		AObjectID	deletedTextObjectID = /*#216 mUndoRecord_Par*/mUndoRecord_DeletedTextObjectID.Get(inIndex);
		
		//
		AText	text;
		text.SetText(mDeletedTextArray.GetObject(mDeletedTextArray.GetIndexByID(deletedTextObjectID)));
		GetTextDocument().SPI_InsertText(spos,text,inRedrawOnceMode,inRedrawOnceMode);
		
		//
		mDeletedTextArray.Delete1Object(mDeletedTextArray.GetIndexByID(deletedTextObjectID));
		mUndoRecord_Tag.Set(inIndex,undoTag_InsertText);
		/*#216 mUndoRecord_Par*/mUndoRecord_Epos.Set(inIndex,spos+text.GetItemCount());
		
		//Reverse���s�����ꍇ�A���̌�A���̏����ɑ����ē��폈�����s���Ă��������Ȃ�
		mCanCat = false;//#210 ==�o�O�C��
		
		//#219 outSelectTextIndex = spos;
		//#219 outSelectTextLength = text.GetItemCount();
		
		//#219 �����v�]������Ȃ�ݒ�œ����������
		if( false )
		{
			//Undo/Redo��ɑ}���������I������ݒ�̏ꍇ
			//�i�\���Ƀe�X�g���Ă��܂���j
			
			//1.����ɒǉ����ꂽ�ꍇ
			//�]����{resultSpos,resultEpos}�ŁAresultEpos==spos�̏ꍇ�A
			//�I��͈͂��L������
			if( resultEpos == spos )
			{
				resultEpos = spos+text.GetItemCount();
			}
			//2.��L�ȊO
			//����ȊO�̏ꍇ�́A�}������������͈̔͂�ݒ肷��
			else
			{
				resultSpos = spos;
				resultEpos = spos+text.GetItemCount();
			}
		}
		else
		{
			//Undo/Redo��ɑ}��������̍Ō�ɃL�����b�g�ړ�����ݒ�̏ꍇ
			
			resultSpos = spos+text.GetItemCount();
			resultEpos = spos+text.GetItemCount();
		}
	}
	else
	{
		_ACError("",NULL);
	}
	//#219
	ioSelectTextIndex = resultSpos;
	ioSelectTextLength = resultEpos-resultSpos;
}

#pragma mark ===========================

#pragma mark ---���擾

//Undo���j���[�\���p������擾
ABool	ATextUndoer::GetUndoMenuText( AText& outText ) const 
{
	ABool	result = false;
	outText.SetLocalizedText("UndoAction_UndoDisabled");
	for( AIndex i = mUndoPosition-1; i >= 0; i-- ) 
	{
		AUndoTag	tag = mUndoRecord_Tag.Get(i);
		if( IsActionTag(tag) == true ) 
		{ 
			/*#94
			outText.SetLocalizedText("UndoAction_",tag-undoTag_Typing);
			AText	text;
			text.SetLocalizedText("UndoAction_Undo");
			outText.AddText(text);
			*/
			AText	text;
			text.SetLocalizedText("UndoAction_",tag-undoTag_Typing);
			outText.SetLocalizedText("UndoAction_Undo");
			outText.ReplaceParamText('0',text);
			result = true;
			break;
		}
	}
	return result;
}

//Redo���j���[�\���p������擾
ABool	ATextUndoer::GetRedoMenuText( AText& outText ) const
{
	ABool	result = false;
	outText.SetLocalizedText("UndoAction_RedoDisabled");
	for( AIndex i = mUndoPosition; i < GetUndoRecordItemCount(); i++ ) 
	{
		AUndoTag	tag = mUndoRecord_Tag.Get(i);
		if( IsActionTag(tag) == true ) 
		{
			/*#94
			outText.SetLocalizedText("UndoAction_",tag-undoTag_Typing);
			AText	text;
			text.SetLocalizedText("UndoAction_Redo");
			outText.AddText(text);
			*/
			AText	text;
			text.SetLocalizedText("UndoAction_",tag-undoTag_Typing);
			outText.SetLocalizedText("UndoAction_Redo");
			outText.ReplaceParamText('0',text);
			result = true;
			break;
		}
	}
	return result;
}

//
AUndoTag	ATextUndoer::GetCurrentAction() const
{
	for( AIndex index = mUndoPosition-1; index >= 0; index-- )
	{
		AUndoTag	tag = mUndoRecord_Tag.Get(index);
		if( tag >= undoTag_Typing )
		{
			return tag;
		}
	}
	return undoTag_NOP;
}

//#267
/**
*/
AIndex	ATextUndoer::GetCurrentLastEditRecordPosition() const
{
	for( AIndex index = mUndoPosition-1; index >= 0; index-- )
	{
		AUndoTag	tag = mUndoRecord_Tag.Get(index);
		if( tag < undoTag_Typing )
		{
			return index;
		}
	}
	return kIndex_Invalid;
}

//#267
/**
���R�[�h�Ɍ����֎~�t���O��ݒ肷��
*/
void	ATextUndoer::SetInhibitConcat()
{
	AIndex	index = GetCurrentLastEditRecordPosition();
	if( index != kIndex_Invalid )
	{
		mUndoRecord_InhibitConcat.Set(index,true);
	}
}

//#314
//UndoRecord�̍Ō�̃A�N�V�����ɁA�}���^�폜���R�[�h���Ȃ�(dangling)���ǂ����𒲂ׂ�
ABool	ATextUndoer::IsCurrentActionDangling() const
{
	if( GetUndoRecordItemCount() == 0 )   return false;
	//��ԍŌ�̃��R�[�h��ActionTag�Ȃ�A�}���^�폜���R�[�h���Ȃ��Ƃ�������
	return (IsActionTag(mUndoRecord_Tag.Get(GetUndoRecordItemCount()-1)) == true);
}


#pragma mark ===========================

#pragma mark ---

//UndoRecord�̍Ō�ɂ���A�A�N�V�����^�O�݂̂ŁA�}���^�폜���R�[�h���Ȃ����̂��폜����
void	ATextUndoer::DeleteDanglingActionTag()
{
	//UndoRecord�̍Ōォ�珇�Ɍ���
	while( GetUndoRecordItemCount() > 0 )
	{
		if( IsActionTag(mUndoRecord_Tag.Get(GetUndoRecordItemCount()-1)) == true )
		{
			//�A�N�V�����^�O�Ȃ�폜���āA���[�v�p��
			DeleteUndoRecord(GetUndoRecordItemCount()-1);
		}
		else
		{
			//�}���^�폜���R�[�h�ɂԂ������烋�[�v�I��
			break;
		}
	}
	//mUndoPosiion�X�V
	if( mUndoPosition > GetUndoRecordItemCount() )
	{
		mUndoPosition = GetUndoRecordItemCount();
	}
}

//�A�N�V�����^�O���ǂ������擾
ABool	ATextUndoer::IsActionTag( const AUndoTag inTag ) const
{
	return (inTag >= undoTag_Typing);
}

//UndoRecord�ǉ�
void	ATextUndoer::AddUndoRecord( const AUndoTag inTag, const AIndex inStart, const AIndex inEnd, const AObjectID inDeletedTextObjectID )//#216 const AIndex inPar )
{
	//���݂�UndoPosition�ȍ~���폜
	mUndoRecord_Tag.Delete(mUndoPosition,mUndoRecord_Tag.GetItemCount()-mUndoPosition);
	mUndoRecord_Spos.Delete(mUndoPosition,mUndoRecord_Spos.GetItemCount()-mUndoPosition);
	//#216 mUndoRecord_Par.Delete(mUndoPosition,mUndoRecord_Par.GetItemCount()-mUndoPosition);
	mUndoRecord_Epos.Delete(mUndoPosition,mUndoRecord_Epos.GetItemCount()-mUndoPosition);//#216
	mUndoRecord_DeletedTextObjectID.Delete(mUndoPosition,mUndoRecord_DeletedTextObjectID.GetItemCount()-mUndoPosition);//#216
	mUndoRecord_InhibitConcat.Delete(mUndoPosition,mUndoRecord_InhibitConcat.GetItemCount()-mUndoPosition);//#267
	
	//�ǉ�
	mUndoRecord_Tag.Add(inTag);
	mUndoRecord_Spos.Add(inStart);
	//#216 mUndoRecord_Par.Add(inPar);
	mUndoRecord_Epos.Add(inEnd);//#216
	mUndoRecord_DeletedTextObjectID.Add(inDeletedTextObjectID);//#216
	mUndoRecord_InhibitConcat.Add(false);//#267
	
	//UndoPosition�X�V
	mUndoPosition = GetUndoRecordItemCount();
}

//UndoRecord�폜
void	ATextUndoer::DeleteUndoRecord( const AIndex inIndex )
{
	//�폜���R�[�h�̏ꍇ�͕ۑ������e�L�X�g���폜
	if( mUndoRecord_Tag.Get(inIndex) == undoTag_DeleteText )
	{
		mDeletedTextArray.Delete1Object(mDeletedTextArray.GetIndexByID(/*#216mUndoRecord_Par*/mUndoRecord_DeletedTextObjectID.Get(inIndex)));
	}
	
	//�폜
	mUndoRecord_Tag.Delete1(inIndex);
	mUndoRecord_Spos.Delete1(inIndex);
	//#216 mUndoRecord_Par.Delete1(inIndex);
	mUndoRecord_Epos.Delete1(inIndex);//#216
	mUndoRecord_DeletedTextObjectID.Delete1(inIndex);//#216
	mUndoRecord_InhibitConcat.Delete1(inIndex);//#267
}

//UndoRecord�폜
void	ATextUndoer::DeleteUndoRecord( const AIndex inIndex, const AItemCount inCount )
{
	for( AIndex i = 0; i < inCount; i++ )
	{
		DeleteUndoRecord(inIndex);
	}
}

