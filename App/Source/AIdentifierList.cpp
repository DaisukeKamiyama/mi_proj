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

AIdentifierList

*/

#include "stdafx.h"

#include "AIdentifierList.h"
#include "AApp.h"

//�L�[���[�h�e�L�X�g����̏ꍇ�̃}�[�N����
const AUChar	kEmptyKeywordTextMark = 0x00;

#pragma mark ===========================
#pragma mark [�N���X]AIdentifierList
#pragma mark ===========================
//
#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AIdentifierList::AIdentifierList( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent)//#348, mIdentifierArray(NULL,10)
//,mIdentifierArray_KeywordText(NULL)//#348
,mDontCheckDuplicateIfInfoTextExists(false)
{
}
//�f�X�g���N�^
AIdentifierList::~AIdentifierList()
{
}

#pragma mark ===========================

#pragma mark ---���ʎq�̒ǉ��^�폜

/**
���ʎq�ǉ�
*/
AUniqID	AIdentifierList::AddIdentifier( const AUniqID inLineUniqID, //#695
									   const AUniqID inPreviousIdentifierUniqID, const AIdentifierType inType, 
									   const AIndex inCategoryIndex, const AText& inKeywordText, 
									   const AText& inMenuText, const AText& inInfoText,
									   const ATextIndex inStartIndex, const ATextIndex inEndIndex, 
									   const ABool inIgnoreCase, //B0368
									   const AText& inCommentText, const AText& inParentKeywordText, const AText& inTypeText,
									   const AText& inCompletionText, const AText& inURLText,
									   const ABool inLocalRangeDelimiter, const AIndex inOutlineLevel, const AIndex inColorSlot )//RC2 #593 #130 #844
{
	//�ϊ������iAIdentifier���珈���ړ��j
	AText	menuText(inMenuText);
	menuText.ReplaceChar(kUChar_Tab,kUChar_Space);//Tab���X�y�[�X�ɕϊ�
	AText	infoText(inInfoText);
	infoText.ReplaceChar(kUChar_Tab,kUChar_Space);//Tab���X�y�[�X�ɕϊ�
	AText	keywordText(inKeywordText);
	//#1266
	if( inIgnoreCase == true )
	{
		//�L�[���[�h�������������ĕۑ�
		keywordText.ChangeCaseLowerFast();
	}
	//�L�[���[�h�e�L�X�g����̏ꍇ�́Ahash���c�q�ɂȂ�Ȃ��悤�A�K���ȁi�����q�b�g���Ȃ��j�e�L�X�g��ݒ肷�� #695
	//AIdentifierList::SearchKeyword()�ɂāA�����̑ΏۊO�Ƃ���B
	if( keywordText.GetItemCount() == 0 )
	{
		keywordText.Add(kEmptyKeywordTextMark);
		keywordText.AddNumber(inLineUniqID.val);
	}
	
	//�z��֒ǉ�
	AIndex	index = mIdentifierArray_KeywordText.Add(keywordText);
	AUniqID	addedUniqID = mIdentifierArray_KeywordText.GetUniqIDByIndex(index);
	
	mIdentifierArray_LineUniqID.Add(inLineUniqID);//#695
	mIdentifierArray_MenuText.Add(menuText);
	mIdentifierArray_InfoText.Add(infoText);
	mIdentifierArray_CommentText.Add(inCommentText);
	mIdentifierArray_ParentKeywordText.Add(inParentKeywordText);
	mIdentifierArray_TypeText.Add(inTypeText);
	mIdentifierArray_NextUniqID.Add(kUniqID_Invalid);
	mIdentifierArray_IdentifierType.Add(inType);
	mIdentifierArray_CategoryIndex.Add(inCategoryIndex);
	mIdentifierArray_StartIndex.Add(inStartIndex);
	mIdentifierArray_EndIndex.Add(inEndIndex);
	mIdentifierArray_IgnoreCase.Add(inIgnoreCase);
	mIdentifierArray_LocalRangeDelimiter.Add(inLocalRangeDelimiter);//#593
	mIdentifierArray_OutlineLevel.Add(inOutlineLevel);//#130
	mIdentifierArray_ColorSlot.Add(inColorSlot);//#844
	mIdentifierArray_CompletionText.Add(inCompletionText);
	mIdentifierArray_URL.Add(inURLText);
	//���X�g�ڑ�
	//inPreviousIdentifierUniqID��kUniqID_Invalid�łȂ��ꍇ�i�����X�g�擪�ȊO�j�A�O��Next�Ɏ��I�u�W�F�N�g��ݒ肷��B
	if( inPreviousIdentifierUniqID != kUniqID_Invalid )
	{
		//�O��Identifier��NextUniqID�ɁA�ǉ�����Identifier��UniqID��ݒ肷��B
		AIndex	prevIndex = mIdentifierArray_KeywordText.GetIndexByUniqID(inPreviousIdentifierUniqID);
		if( mIdentifierArray_NextUniqID.Get(prevIndex) == kUniqID_Invalid )
		{
			mIdentifierArray_NextUniqID.Set(prevIndex,addedUniqID);
		}
		else
		{
			//prev��UniqID�ݒ�ς݁i�Ō���ǉ��łȂ��j�ꍇ��throw
			_ACThrow("not last object",this);
		}
	}
	
	return addedUniqID;
}

/**
���ʎq�폜
���X�g�ɘA�Ȃ鎯�ʎq��S�č폜����B
*/
void	AIdentifierList::DeleteIdentifiers( const AUniqID inFirstUniqID )
{
	/*#348
	//�܂����̎��ʎq�ȍ~���폜�i�ċA�j
	AUniqID	nextUniqID = mIdentifierArray.GetObjectByID(inFirstUniqID).GetNextUniqID();
	if( nextUniqID != kUniqID_Invalid )
	{
		DeleteIdentifiers(nextUniqID);
	}
	//���g���폜
	//index�͂����ōēx��������K�v������B�i�ċA�R�[���Ŏ��ȍ~���폜�ς݂Ȃ̂ŁA�ŏ��Ƃ�Index���ς���Ă���B�j
	AIndex	index = mIdentifierArray.GetIndexByUniqID(inFirstUniqID);//�����K�v�i���x�j
	mIdentifierArray.Delete1Object(index);
	*/
	//�܂����̎��ʎq�ȍ~���폜�i�ċA�j
	AIndex	index0 = mIdentifierArray_KeywordText.GetIndexByUniqID(inFirstUniqID);
	AUniqID	nextUniqID = mIdentifierArray_NextUniqID.Get(index0);
	if( nextUniqID == inFirstUniqID )   {_ACError("uniq id error (loop)",NULL);return;}
	if( nextUniqID != kUniqID_Invalid )
	{
		DeleteIdentifiers(nextUniqID);
	}
	//���g���폜
	//index�͂����ōēx��������K�v������B�i�ċA�R�[���Ŏ��ȍ~���폜�ς݂Ȃ̂ŁA�ŏ��Ƃ�Index���ς���Ă���B�j
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inFirstUniqID);
	mIdentifierArray_KeywordText.Delete1(index);
	mIdentifierArray_LineUniqID.Delete1(index);//#695
	mIdentifierArray_MenuText.Delete1(index);
	mIdentifierArray_InfoText.Delete1(index);
	mIdentifierArray_CommentText.Delete1(index);
	mIdentifierArray_ParentKeywordText.Delete1(index);
	mIdentifierArray_TypeText.Delete1(index);
	mIdentifierArray_NextUniqID.Delete1(index);
	mIdentifierArray_IdentifierType.Delete1(index);
	mIdentifierArray_CategoryIndex.Delete1(index);
	mIdentifierArray_StartIndex.Delete1(index);
	mIdentifierArray_EndIndex.Delete1(index);
	mIdentifierArray_IgnoreCase.Delete1(index);
	mIdentifierArray_LocalRangeDelimiter.Delete1(index);//#593
	mIdentifierArray_OutlineLevel.Delete1(index);//#130
	mIdentifierArray_ColorSlot.Delete1(index);//#844
	mIdentifierArray_CompletionText.Delete1(index);
	mIdentifierArray_URL.Delete1(index);
}

//#641
/**
���ʎq�S�폜
*/
void	AIdentifierList::DeleteAllIdentifiers()
{
	mIdentifierArray_KeywordText.DeleteAll();
	mIdentifierArray_LineUniqID.DeleteAll();//#695
	mIdentifierArray_MenuText.DeleteAll();
	mIdentifierArray_InfoText.DeleteAll();
	mIdentifierArray_CommentText.DeleteAll();
	mIdentifierArray_ParentKeywordText.DeleteAll();
	mIdentifierArray_TypeText.DeleteAll();
	mIdentifierArray_NextUniqID.DeleteAll();
	mIdentifierArray_IdentifierType.DeleteAll();
	mIdentifierArray_CategoryIndex.DeleteAll();
	mIdentifierArray_StartIndex.DeleteAll();
	mIdentifierArray_EndIndex.DeleteAll();
	mIdentifierArray_IgnoreCase.DeleteAll();
	mIdentifierArray_LocalRangeDelimiter.DeleteAll();//#593
	mIdentifierArray_OutlineLevel.DeleteAll();//#130
	mIdentifierArray_ColorSlot.DeleteAll();//#844
	mIdentifierArray_CompletionText.DeleteAll();
	mIdentifierArray_URL.DeleteAll();
}

#pragma mark ===========================

#pragma mark ---�f�[�^�擾�iUniqID����擾�j

/**
UniqID����L�[���[�h�e�L�X�g�擾
*/
ABool	AIdentifierList::GetKeywordText( const AUniqID inUniqID, AText& outText ) const
{
	//#348 outText.SetText(mIdentifierArray.GetObjectConstByID(inUniqID).GetKeywordText());
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	mIdentifierArray_KeywordText.Get(index,outText);//#348
	ABool	keywordValid = false;
	if( outText.GetItemCount() > 0 )
	{
		//0x00�Ŏn�܂�L�[���[�h�̓L�[���[�h�e�L�X�g�����ۂɂ͋�ł��邱�Ƃ������B�ihash�c�q�΍�j
		if( outText.Get(0) != kEmptyKeywordTextMark )
		{
			keywordValid = true;
		}
	}
	return keywordValid;
}

//#695
/**
�sUniqID���擾
*/
AUniqID	AIdentifierList::GetLineUniqID( const AUniqID inUniqID ) const
{
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);
	return mIdentifierArray_LineUniqID.Get(index);
}

/**
UniqID����J�e�S���[Index�擾
*/
AIndex	AIdentifierList::GetCategoryIndex( const AUniqID inUniqID ) const
{
	//#348 return mIdentifierArray.GetObjectConstByID(inUniqID).GetCategoryIndex();
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	return mIdentifierArray_CategoryIndex.Get(index);//#348
}

/**
UniqID���烁�j���[�e�L�X�g�擾
*/
void	AIdentifierList::GetMenuText( const AUniqID inUniqID, AText& outText ) const
{
	//#348 outText.SetText(mIdentifierArray.GetObjectConstByID(inUniqID).GetMenuText());
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	mIdentifierArray_MenuText.Get(index,outText);//#348
}

/**
UniqID������e�L�X�g�擾
*/
void	AIdentifierList::GetInfoText( const AUniqID inUniqID, AText& outText ) const
{
	//#348 outText.SetText(mIdentifierArray.GetObjectConstByID(inUniqID).GetInfoText());
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	mIdentifierArray_InfoText.Get(index,outText);//#348
}

//RC2
/**
UniqID����R�����g�e�L�X�g�擾
*/
void	AIdentifierList::GetCommentText( const AUniqID inUniqID, AText& outText ) const
{
	//#348 outText.SetText(mIdentifierArray.GetObjectConstByID(inUniqID).GetCommentText());
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	mIdentifierArray_CommentText.Get(index,outText);//#348
}

//RC2
/**
UniqID����Parent�L�[���[�h�e�L�X�g�擾
*/
void	AIdentifierList::GetParentKeywordText( const AUniqID inUniqID, AText& outText ) const
{
	//#348 outText.SetText(mIdentifierArray.GetObjectConstByID(inUniqID).GetParentKeywordText());
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	mIdentifierArray_ParentKeywordText.Get(index,outText);//#348
}

/**
UniqID����A���o�����j���[�\���Ώۂ��ǂ������擾
*/
ABool	AIdentifierList::IsMenuIdentifier( const AUniqID inUniqID ) const
{
	//#348 return (mIdentifierArray.GetObjectConstByID(inUniqID).GetMenuText().GetItemCount() > 0);
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	AText	menuText;//#348
	mIdentifierArray_MenuText.Get(index,menuText);//#348
	return (menuText.GetItemCount() > 0);//#348
}

/**
UniqID����A���X�g�̎���UniqID���擾
*/
AUniqID	AIdentifierList::GetNextUniqID( const AUniqID inUniqID ) const
{
	//#348 return mIdentifierArray.GetObjectConstByID(inUniqID).GetNextUniqID();
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	return mIdentifierArray_NextUniqID.Get(index);//#348
}

/**
UniqID����A�J�nIndex���擾
*/
AIndex	AIdentifierList::GetStartIndex( const AUniqID inUniqID ) const
{
	//#348 return mIdentifierArray.GetObjectConstByID(inUniqID).GetStartIndex();
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	return mIdentifierArray_StartIndex.Get(index);//#348
}

/**
UniqID����A�I��Index���擾
*/
AIndex	AIdentifierList::GetEndIndex( const AUniqID inUniqID ) const
{
	//#348 return mIdentifierArray.GetObjectConstByID(inUniqID).GetEndIndex();
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	return mIdentifierArray_EndIndex.Get(index);//#348
}

/**
UniqID����AType�e�L�X�g���擾
*/
void	AIdentifierList::GetTypeText( const AUniqID inUniqID, AText& outText ) const
{
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	mIdentifierArray_TypeText.Get(index,outText);//#348
}

/**
UniqID����A���ʎq�^�C�v���擾
*/
AIdentifierType	AIdentifierList::GetIdentifierType( const AUniqID inUniqID ) const
{
	//#348 return mIdentifierArray.GetObjectConstByID(inUniqID).GetIdentifierType();
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	return mIdentifierArray_IdentifierType.Get(index);//#348
}

//#593
/**
UniqID����A���[�J�����ʎq�͈̔͋�؂�ƂȂ邩�ǂ������擾
*/
ABool	AIdentifierList::IsLocalRangeDelimiter( const AUniqID inUniqID ) const
{
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);
	return mIdentifierArray_LocalRangeDelimiter.Get(index);
}

//#130
/**
UniqID����AOutline level�擾
*/
AIndex	AIdentifierList::GetOutlineLevel( const AUniqID inUniqID ) const
{
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);
	return mIdentifierArray_OutlineLevel.Get(index);
}

//#844
/**
UniqID����A�J���[�X���b�g�擾
*/
AIndex	AIdentifierList::GetKeywordColorSlot( const AUniqID inUniqID ) const
{
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);
	return mIdentifierArray_ColorSlot.Get(index);
}

#pragma mark ===========================

#pragma mark ---�f�[�^�擾�i�L�[���[�h���猟���j

//#853 #717
/**
�e��L�[���[�h�����X���b�h�p�����L�[���[�h��������
@note �n�b�V�����g��Ȃ��̂Œᑬ�Ȃ��߁A�X���b�h�g�p���K�{�����A���낢��Ȍ������@���ł���B
outLineUniqIDArray�ȊO�́A���ڂ�~�ς���B
*/
ABool	AIdentifierList::SearchKeyword( const AText& inWord, const AHashTextArray& inParentWord, const AKeywordSearchOption inOption,
									   const AModeIndex inModeIndex, const AIndex inCurrentStateIndex,
									   const AHashArray<AUniqID>& inLimitLineUniqIDArray,
									   AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
									   ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
									   ATextArray& outCommentTextArray,
									   ATextArray& outCompletionTextArray, ATextArray& outURLArray,
									   AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
									   AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
									   AArray<AItemCount>& outMatchedCountArray,
									   AArray<AUniqID>& outLineUniqIDArray,
									   const ABool& inAbort ) const
{
	//outLineUniqIDArray�̂ݍ��ڍ폜
	//����ȊO�̌��ʔz��́A���ڂ�~�ς���̂ŁA�폜���Ȃ��B
	outLineUniqIDArray.DeleteAll();
	/*
	//�������[�h���擾
	AText	origword;
	origword.SetText(inWord);
	//���g���[�h��啶�������������I�v�V�����L��Ȃ珬�����ɕϊ�
	if( (inOption&kKeywordSearchOption_IgnoreCases) != 0 )
	{
		word.ChangeCaseLowerFast();
	}
	*/
	//�������������ꍇ�̌����L�[���[�h���擾
	AText	wordLowerCase;
	wordLowerCase.SetText(inWord);
	wordLowerCase.ChangeCaseLower();
	//�e�L�[���[�h�Ɣ�r���邩�ǂ����̃t���O��ݒ�
	ABool	compareParentKeyword = ((inOption&kKeywordSearchOption_CompareParentKeyword)!=0);
	//�����Ώۂ̍s�����肳��Ă��邩�ǂ����̃t���O��ݒ�
	ABool	limitLineUniqID = (inLimitLineUniqIDArray.GetItemCount() > 0 );
	//�eidentifier���̃��[�v
	AItemCount	itemCount = mIdentifierArray_KeywordText.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//abort�t���Oon�Ȃ�I���i�ʃX���b�h����ݒ肳���j
		if( inAbort == true )
		{
			break;
		}
		//�Ώۍs����Ȃ�A�w��s�Ɋ܂܂��s�łȂ��ꍇ�́A����identifier�ցB
		if( limitLineUniqID == true )
		{
			if( inLimitLineUniqIDArray.Find(mIdentifierArray_LineUniqID.Get(i)) == kIndex_Invalid )
			{
				continue;
			}
		}
		//�e�L�[���[�h�Ƃ̔�r�t���OON�Ȃ�A�e�L�[���[�h���w��e�L�[���[�h���X�g���ɑ��݂��Ȃ��Ȃ�A����identifier��
		if( compareParentKeyword == true )
		{
			AText	parentKeywordText;
			mIdentifierArray_ParentKeywordText.Get(i,parentKeywordText);
			if( inParentWord.Find(parentKeywordText) == kIndex_Invalid )
			{
				continue;
			}
		}
		//�L�[���[�h�擾
		AText	keywordText;
		mIdentifierArray_KeywordText.Get(i,keywordText);
		//�L�[���[�h����Ȃ玟��identifier��
		if( keywordText.GetItemCount() == 0 )
		{
			continue;
		}
		//�L�[���[�h��0x00�Ŏn�܂�Ȃ玟��identifier��
		if( keywordText.Get(0) == kEmptyKeywordTextMark )
		{
			continue;
		}
		//�J�e�S��=-1�Ȃ�ΏۊO�B����identifier�ցB
		AIndex	categoryIndex = mIdentifierArray_CategoryIndex.Get(i);
		if( categoryIndex == kIndex_Invalid )
		{
			continue;
		}
		//�啶�������������Ȃ�A���ʎq�̃L�[���[�h���������ɂ���B
		ABool	ignoreCase = false;
		if( (inOption&kKeywordSearchOption_IgnoreCases) != 0 || mIdentifierArray_IgnoreCase.Get(i) == true )
		{
			//���ʎq�̃L�[���[�h����������
			keywordText.ChangeCaseLowerFast();
			//�啶�������������t���O��ON
			ignoreCase = true;
		}
		//��r�L�[���[�h�擾�i�啶�������������Ȃ���������������������̂��擾�A�啶�������������łȂ���Έ������̂܂܁j
		const AText&	word = (ignoreCase?wordLowerCase:inWord);
		//��v�t���O
		ABool	matched = false;
		//�������[�h����Ȃ��Ɉ�v
		if( word.GetItemCount() == 0 )
		{
			matched = true;
		}
		//�������[�h����łȂ��Ȃ�A�L�[���[�h�Ɣ�r
		else
		{
			//������v
			if( (inOption&kKeywordSearchOption_Partial) != 0 )
			{
				if( keywordText.GetItemCount() >= word.GetItemCount() )
				{
					AIndex	pos = 0;
					matched = keywordText.FindText(0,word,pos);
				}
			}
			//�O����v
			else if( (inOption&kKeywordSearchOption_Leading) != 0 )
			{
				if( keywordText.GetItemCount() >= word.GetItemCount() )
				{
					matched = keywordText.CompareMin(word);
				}
			}
			//�S�̈�v
			else
			{
				matched = keywordText.Compare(word);
			}
		}
		//InfoText�����܂ރI�v�V����ON�̏ꍇ�Ainfo text���畔����v�Ō���
		if( matched == false )
		{
			if( (inOption&kKeywordSearchOption_IncludeInfoText) != 0 )
			{
				AText	infoText;
				mIdentifierArray_InfoText.Get(i,infoText);
				AIndex	pos = 0;
				matched = infoText.FindText(0,word,pos);
			}
		}
		//���݂�state�ŗL���ȃJ�e�S���̂ݑΏۂƂ��錟���I�v�V�����̏���
		if( (inOption&kKeywordSearchOption_OnlyEnabledCategoryForCurrentState) != 0 && inCurrentStateIndex != kIndex_Invalid )
		{
			if( GetApp().SPI_GetModePrefDB(inModeIndex).GetCategoryDisabled(inCurrentStateIndex,categoryIndex) == true )
			{
				matched = false;
			}
		}
		//��v�����ꍇ�́A���ʔz��֒ǉ�
		if( matched == true )
		{
			//�L�[���[�h�e�L�X�g�擾
			AText	keyword;
			mIdentifierArray_KeywordText.Get(i,keyword);
			//�e�L�[���[�h�擾
			AText	parentKeywordText;
			mIdentifierArray_ParentKeywordText.Get(i,parentKeywordText);
			//���e�L�X�g�擾
			AText	infoText;
			mIdentifierArray_InfoText.Get(i,infoText);
			//�L�[���[�h�d���`�F�b�N
			//�����I�v�V����kKeywordSearchOption_InhibitDuplicatedKeyword��ON�̂Ƃ��͏d���`�F�b�N���s��
			//�i�������AmDontCheckDuplicateIfInfoTextExists��true�ŁA���A���e�L�X�g�����݂���Ƃ��́A�d���`�F�b�N���Ȃ��B�L�[���[�h���\���̂��߁B�j
			ABool	checkDuplicate = ((inOption&kKeywordSearchOption_InhibitDuplicatedKeyword)!= 0);
			if( mDontCheckDuplicateIfInfoTextExists == true && infoText.GetItemCount() > 0 )
			{
				checkDuplicate = false;
			}
			AIndex	duplicatedKeywordArrayIndex = kIndex_Invalid;
			if( checkDuplicate == true )
			{
				duplicatedKeywordArrayIndex = outKeywordArray.Find(keyword);
			}
			if( duplicatedKeywordArrayIndex != kIndex_Invalid )
			{
				//------------------�d���L�[���[�h�L��̏ꍇ------------------
				//���ɓ����L�[���[�h������ꍇ�͒ǉ�������matched count��+1����
				outMatchedCountArray.Set(duplicatedKeywordArrayIndex,outMatchedCountArray.Get(duplicatedKeywordArrayIndex)+1);
				//parent keyword����v����Ƃ��́A�e�f�[�^���㏑������
				if( inParentWord.Find(parentKeywordText) != kIndex_Invalid )
				{
					//����A�d���֎~�I�v�V�����͌�⌟�������ł����g��Ȃ��̂ŁA�\���f�[�^�݂̂��㏑������B
					//start index���́A�ŏ��Ɍ��������f�[�^�̂܂܂Ƃ���Bfile path���̃f�[�^�͂��̊֐��̊O�Őݒ肷��̂ŁA
					//�������㏑����ǉ�����Ȃ�A�֐��C���^�[�t�F�C�X�ύX�K�v�B
					outParentKeywordArray.Set(duplicatedKeywordArrayIndex,parentKeywordText);
					outTypeTextArray.Set(duplicatedKeywordArrayIndex,mIdentifierArray_TypeText.GetTextConst(i));
					outInfoTextArray.Set(duplicatedKeywordArrayIndex,infoText);
					outCommentTextArray.Set(duplicatedKeywordArrayIndex,mIdentifierArray_CommentText.GetTextConst(i));
					outCompletionTextArray.Set(duplicatedKeywordArrayIndex,mIdentifierArray_CompletionText.GetTextConst(i));
					outURLArray.Set(duplicatedKeywordArrayIndex,mIdentifierArray_URL.GetTextConst(i));
					outCategoryIndexArray.Set(duplicatedKeywordArrayIndex,categoryIndex);
				}
			}
			else
			{
				//------------------�d���L�[���[�h�����̏ꍇ------------------
				//�������ʒǉ�
				outKeywordArray.Add(keyword);
				outParentKeywordArray.Add(parentKeywordText);
				outTypeTextArray.Add(mIdentifierArray_TypeText.GetTextConst(i));
				outInfoTextArray.Add(infoText);
				outCommentTextArray.Add(mIdentifierArray_CommentText.GetTextConst(i));
				outCompletionTextArray.Add(mIdentifierArray_CompletionText.GetTextConst(i));
				outURLArray.Add(mIdentifierArray_URL.GetTextConst(i));
				outCategoryIndexArray.Add(categoryIndex);
				outStartIndexArray.Add(mIdentifierArray_StartIndex.Get(i));
				outColorSlotIndexArray.Add(mIdentifierArray_ColorSlot.Get(i));
				outEndIndexArray.Add(mIdentifierArray_EndIndex.Get(i));
				outLineUniqIDArray.Add(mIdentifierArray_LineUniqID.Get(i));
				outMatchedCountArray.Add(1);
				//�����l�I�[�o�[�̏ꍇ�́A�I���B
				if( outKeywordArray.GetItemCount() >= kLimit_KeywordSearchResultCount )
				{
					return false;
				}
			}
		}
	}
	return true;
}

#if 0
//RC2
/**
�⊮���͌��ǉ��ioutAbbrevCandidateArray���ɂ͌��f�[�^���ǉ������B�ŏ���DeleteAll()�͂���Ȃ��j
*/
void	AIdentifierList::GetAbbrevCandidate( const AText& inText,
		ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
		AArray<AIndex>& outCategoryIndexArray ) const
{
	//#348 AItemCount	itemCount = mIdentifierArray.GetItemCount();
	AItemCount	itemCount = mIdentifierArray_KeywordText.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//#348 const AText&	keywordText = mIdentifierArray.GetObjectConst(i).GetKeywordText();
		AText	keywordText;//#348
		mIdentifierArray_KeywordText.Get(i,keywordText);//#348
		if( keywordText.GetItemCount() >= inText.GetItemCount() )
		{
			if( keywordText.CompareMin(inText) == true )
			{
				//�d���L�[���[�h��ǉ����Ȃ��悤�ɂ��悤�Ƃ�������߂��i���E�C���h�E�\���ŏ�񂪏o�Ȃ��Ȃ邽�߁jif( outAbbrevCandidateArray.Find(keywordText) == kIndex_Invalid )
				{
					outAbbrevCandidateArray.Add(keywordText);
					//#348 outInfoTextArray.Add(mIdentifierArray.GetObjectConst(i).GetInfoText());
					//#348 outCategoryIndexArray.Add(mIdentifierArray.GetObjectConst(i).GetCategoryIndex());
					AText	infoText;//#348
					mIdentifierArray_InfoText.Get(i,infoText);//#348
					outInfoTextArray.Add(infoText);//#348
					outCategoryIndexArray.Add(mIdentifierArray_CategoryIndex.Get(i));//#348
				}
			}
		}
	}
}

//R0243
/**
�⊮���͌��ǉ��ioutAbbrevCandidateArray���ɂ͌��f�[�^���ǉ������B�ŏ���DeleteAll()�͂���Ȃ��j
Parent�L�[���[�h�i�N���X�����j���猟��
*/
void	AIdentifierList::GetAbbrevCandidateByParentKeyword( const AText& inParentKeyword,
		ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
		AArray<AIndex>& outCategoryIndexArray ) const
{
	//#348 AItemCount	itemCount = mIdentifierArray.GetItemCount();
	AItemCount	itemCount = mIdentifierArray_KeywordText.GetItemCount();//#348
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//#348 const AText&	keywordText = mIdentifierArray.GetObjectConst(i).GetKeywordText();
		//#348 if( mIdentifierArray.GetObjectConst(i).GetParentKeywordText().Compare(inParentKeyword) == true )
		if( mIdentifierArray_ParentKeywordText.Compare(i,inParentKeyword) == true )//#348
		{
			AText	keywordText;//#348
			mIdentifierArray_KeywordText.Get(i,keywordText);//#348
			outAbbrevCandidateArray.Add(keywordText);
			//#348 outInfoTextArray.Add(mIdentifierArray.GetObjectConst(i).GetInfoText());
			//#348 outCategoryIndexArray.Add(mIdentifierArray.GetObjectConst(i).GetCategoryIndex());
			AText	infoText;//#348
			mIdentifierArray_InfoText.Get(i,infoText);//#348
			outInfoTextArray.Add(infoText);//#348
			outCategoryIndexArray.Add(mIdentifierArray_CategoryIndex.Get(i));//#348
		}
	}
}
#endif

/**
�L�[���[�h�����񂩂环�ʎq��UniqID�̃��X�g���擾
*/
void	AIdentifierList::GetIdentifierListByKeyword( const AText& inText, AArray<AUniqID>& outIdentifierIDArray ) const
{
	//
	outIdentifierIDArray.DeleteAll();
	
	//�����L�[���[�h�e�L�X�g����Ȃ牽���������^�[��
	if( inText.GetItemCount() == 0 )   return;
	
	//#348 mIdentifierArray.FindUniqID(inText,0,kIndex_Invalid,outIdentifierIDArray);
	mIdentifierArray_KeywordText.FindUniqIDs(inText,0,kIndex_Invalid,outIdentifierIDArray);//#348
}

/**
�L�[���[�h�����񂩂环�ʎq��UniqID���擾�i�ŏ��Ɍ����������́j
*/
AUniqID	AIdentifierList::GetIdentifierByKeywordText( const AText& inKeywordText, 
			const ABool inNotUseDisabledCategoryArray, const ABoolArray& inDisabledCategoryArray ) const//R0000 �J�e�S���[��
{
	AIndex	index = GetIdentifierIndexByKeywordText(inKeywordText,inNotUseDisabledCategoryArray,inDisabledCategoryArray);//#348
	if( index == kIndex_Invalid )   return kUniqID_Invalid;//#348
	return mIdentifierArray_KeywordText.GetUniqIDByIndex(index);//#348
}

/**
�L�[���[�h�����񂩂环�ʎq��Index���擾�i�ŏ��Ɍ����������́j
*/
AIndex	AIdentifierList::GetIdentifierIndexByKeywordText( const AText& inKeywordText, 
			const ABool inNotUseDisabledCategoryArray, const ABoolArray& inDisabledCategoryArray ) const//R0000 �J�e�S���[��
{
	//#629 IgnoreCase�����S�Ċ܂߂āA�J�e�S���ԍ�����ԑ傫�����̂̂�Ԃ��悤�ɕύX
	
	//�����L�[���[�h�e�L�X�g����Ȃ牽���������^�[��
	if( inKeywordText.GetItemCount() == 0 )   return kIndex_Invalid;
	
	//�啶�����������̂܂܂Ō���
	AArray<AIndex>	indexArray;
	mIdentifierArray_KeywordText.FindAll(inKeywordText,0,kIndex_Invalid,indexArray);
	//�����������Č���
	AUChar	ch = inKeywordText.Get(0);
	if( IsUTF8Multibyte(ch) == false )//�ŏ��̕������}���`�o�C�g�����Ȃ�ignore case�������Ȃ�
	{
		AText	lowerCaseText;
		lowerCaseText.SetText(inKeywordText);
		if( lowerCaseText.ChangeCaseLowerFast() == true )//�ϊ����Ȃ������Ȃ猟���s�v
		{
			AArray<AIndex>	ignoreCaseIndexArray;
			mIdentifierArray_KeywordText.FindAll(lowerCaseText,0,kIndex_Invalid,ignoreCaseIndexArray);
			//IgnoreCase�ɐݒ肳��Ă��鎯�ʎq�̂�indexArray�փR�s�[����
			AItemCount	count = ignoreCaseIndexArray.GetItemCount();
			for( AIndex i = 0; i < count; i++ )
			{
				AIndex	index = ignoreCaseIndexArray.Get(i);
				if( mIdentifierArray_IgnoreCase.Get(index) == true )
				{
					indexArray.Add(index);
				}
			}
		}
	}
	//
	AIndex	foundLargestCategoryIndex = kIndex_Invalid;
	AIndex	foundIdentifierIndex = kIndex_Invalid;
	//indexArray�̒�����ADisable�Ɏw�肳�ꂽ�J�e�S���ł͂Ȃ����̂̂����A�J�e�S���ԍ����ő�̂��̂�����
	AItemCount	count = indexArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AIndex	index = indexArray.Get(i);
		AIndex	categoryIndex = mIdentifierArray_CategoryIndex.Get(index);
		//disabled�J�e�S���łȂ����𔻒�
		if( inNotUseDisabledCategoryArray == true ||
					inDisabledCategoryArray.Get(categoryIndex) == false )
		{
			//�J�e�S���ԍ������܂łōő傩�ǂ����𔻒�
			if( foundLargestCategoryIndex == kIndex_Invalid ||
						categoryIndex > foundLargestCategoryIndex )
			{
				//���܂łōő�̃J�e�S���ԍ��Ȃ̂ŁAindex��categoryIndex���L��
				foundLargestCategoryIndex = categoryIndex;
				foundIdentifierIndex = index;
			}
		}
	}
	return foundIdentifierIndex;
	/*#629
	//B0368 return mIdentifierArray.FindUniqID(inKeywordText);
	AIndex	resultIndex = kIndex_Invalid;
	if( inNotUseDisabledCategoryArray == true )
	{
		resultIndex = mIdentifierArray_KeywordText.Find(inKeywordText);
	}
	else
	{
		//R0000 �J�e�S���[��
		AArray<AIndex>	indexArray;
		mIdentifierArray_KeywordText.FindAll(inKeywordText,0,kIndex_Invalid,indexArray);
		AItemCount	count = indexArray.GetItemCount();
		for( AIndex i = 0; i < count; i++ )
		{
			resultIndex = indexArray.Get(i);
			if( inDisabledCategoryArray.Get(mIdentifierArray_CategoryIndex.Get(resultIndex)) == false )
			{
				return resultIndex;
			}
		}
		resultIndex = kIndex_Invalid;
	}
	if( resultIndex != kIndex_Invalid )   return resultIndex;
	
	//B0368
	//�����������Č����B�������AAIdentifier�I�u�W�F�N�g��IgnoreCase�̂��̂̂݃q�b�g�B
	if( inKeywordText.GetItemCount() == 0 )   return kIndex_Invalid;
	//B0000 if( Macro_IsAlphabet(inKeywordText.Get(0)) == false )   return kUniqID_Invalid;
	AUChar	ch = inKeywordText.Get(0);
	if( IsUTF8Multibyte(ch) == true )   return kIndex_Invalid;//�ŏ��̕������}���`�o�C�g�����Ȃ�ignore case�������Ȃ�
	
	AText	lowerCaseText;
	lowerCaseText.SetText(inKeywordText);
	if( lowerCaseText.ChangeCaseLowerFast() == false )   return kIndex_Invalid;//B0000 �ϊ����Ȃ������Ȃ猟���s�v
	AArray<AIndex>	indexArray;
	mIdentifierArray_KeywordText.FindAll(lowerCaseText,0,kIndex_Invalid,indexArray);
	AItemCount	count = indexArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		resultIndex = indexArray.Get(i);
		if( mIdentifierArray_IgnoreCase.Get(resultIndex) == true )
		{
			if( inNotUseDisabledCategoryArray == true )
			{
				return resultIndex;
			}
			else
			{
				//R0000 �J�e�S���[��
				if( inDisabledCategoryArray.Get(mIdentifierArray_CategoryIndex.Get(resultIndex)) == false )
				{
					return resultIndex;
				}
			}
		}
	}
	return kIndex_Invalid;
	*/
}

/**
�L�[���[�h�����񂩂猟���A�J�e�S��Index���擾
*/
AIndex	AIdentifierList::GetCategoryIndexByKeywordText( const AText& inKeywordText, 
		const ABool inNotUseDisabledCategoryArray, const ABoolArray& inDisabledCategoryArray,
		AIndex& outColorSlot ) const//R0000 �J�e�S���[�� #844
{
	outColorSlot = kIndex_Invalid;
	AIndex	index = GetIdentifierIndexByKeywordText(inKeywordText,inNotUseDisabledCategoryArray,inDisabledCategoryArray);//#348
	if( index == kIndex_Invalid )   return kIndex_Invalid;//#348
	
	//#844
	outColorSlot = mIdentifierArray_ColorSlot.Get(index);
	//
	return mIdentifierArray_CategoryIndex.Get(index);//#348
}

//R0243
/**
�L�[���[�h�����񂩂猟���AType�e�L�X�g���擾
*/
ABool	AIdentifierList::GetTypeTextByKeywordText( const AText& inKeywordText, 
		const ABool inNotUseDisabledCategoryArray, const ABoolArray& inDisabledCategoryArray, AText& outTypeText ) const
{
	outTypeText.DeleteAll();
	AIndex	index = GetIdentifierIndexByKeywordText(inKeywordText,inNotUseDisabledCategoryArray,inDisabledCategoryArray);//#348
	if( index == kIndex_Invalid )   return false;//#348
	mIdentifierArray_TypeText.Get(index,outTypeText);//#348
	return true;
}

#pragma mark ===========================

#pragma mark ---�f�[�^�擾�iIndex����擾�j

/**
Index����L�[���[�h�e�L�X�g�擾
*/
void	AIdentifierList::GetKeywordTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mIdentifierArray_KeywordText.Get(inIndex,outText);//#348
}

//#348
/**
Index����L�[���[�h�e�L�X�g��r
*/
ABool	AIdentifierList::CompareKeywordTextByIdentifierIndex( const AIndex inIndex, const AText& inText ) const
{
	return mIdentifierArray_KeywordText.Compare(inIndex,inText);
}

/**
Index����J�e�S���[Index�擾
*/
AIndex	AIdentifierList::GetCategoryIndexByIdentifierIndex( const AIndex inIndex ) const
{
	//#348 return mIdentifierArray.GetObjectConst(inIndex).GetCategoryIndex();
	return mIdentifierArray_CategoryIndex.Get(inIndex);//#348
}

/**
Index����J���[�X���b�gIndex�擾
*/
AIndex	AIdentifierList::GetColorSlotIndexByIdentifierIndex( const AIndex inIndex ) const
{
	return mIdentifierArray_ColorSlot.Get(inIndex);
}

/**
Index������e�L�X�g�擾
*/
void	AIdentifierList::GetInfoTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mIdentifierArray_InfoText.Get(inIndex,outText);//#348
}

/**
Index����UniqID�擾
*/
AUniqID	AIdentifierList::GetUniqIDByIdentifierIndex( const AIndex inIndex ) const
{
	//#348 return mIdentifierArray.GetObjectConst(inIndex).GetUniqID();
	return mIdentifierArray_KeywordText.GetUniqIDByIndex(inIndex);//#348
}

/**
Index����R�����g�e�L�X�g�擾
*/
void	AIdentifierList::GetCommentTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mIdentifierArray_CommentText.Get(inIndex,outText);//#348
}

/**
Index����Parent�L�[���[�h�e�L�X�g�擾
*/
void	AIdentifierList::GetParentKeywordTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mIdentifierArray_ParentKeywordText.Get(inIndex,outText);//#348
}

/**
Index����Type�e�L�X�g�擾
*/
void	AIdentifierList::GetTypeTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mIdentifierArray_TypeText.Get(inIndex,outText);//#348
}

/**
�g�p�T�C�Y�擾�i�f�o�b�O�p�j
*/
AByteCount	AIdentifierList::GetAllocatedSize() const
{
	return (mIdentifierArray_MenuText.GetTextDataSize()+
	mIdentifierArray_InfoText.GetTextDataSize()+
	mIdentifierArray_CommentText.GetTextDataSize()+
	mIdentifierArray_ParentKeywordText.GetTextDataSize()+
	mIdentifierArray_TypeText.GetTextDataSize());
}

