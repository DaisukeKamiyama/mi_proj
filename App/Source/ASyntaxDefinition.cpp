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

ASyntaxDefinition

*/

#include "stdafx.h"

#include "ASyntaxDefinition.h"
#include "AIdentifierList.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]ASyntaxDefinitionActionSequence
#pragma mark ===========================
//アクションシーケンス毎に作成されるクラス

#pragma mark ---コンストラクタ／デストラクタ

//コンストラクタ
ASyntaxDefinitionActionSequence::ASyntaxDefinitionActionSequence( AObjectArrayItem* inParent, ASyntaxDefinitionState& inSyntaxDefinitionState ) : AObjectArrayItem(inParent), mSyntaxDefinitionState(inSyntaxDefinitionState)
{ 
}

ASyntaxDefinitionActionSequence::~ASyntaxDefinitionActionSequence()
{
}

#pragma mark ===========================

#pragma mark ---関連オブジェクト取得

ASyntaxDefinition&	ASyntaxDefinitionActionSequence::GetSyntaxDefinition()
{
	return GetSyntaxDefinitionState().GetSyntaxDefinition();
}

#pragma mark ===========================

#pragma mark ---アクション管理

//アクション追加
AIndex	ASyntaxDefinitionActionSequence::AddAction( const ASyntaxDefinitionActionType inActionType )
{
	mTypeArray.Add(inActionType);
	mVariableIndexArray.Add(kIndex_Invalid);
	/*#693 メモリ削減のため、mParameter1Array, mParameter2Arrayで共用
	mVariableIndex1Array.Add(kIndex_Invalid);
	mVariableIndex2Array.Add(kIndex_Invalid);
	mStartNumberArray.Add(0);
	mEndNumberArray.Add(0);
	*/
	mParameter1Array.Add(kIndex_Invalid);//#693
	mParameter2Array.Add(kIndex_Invalid);//#693
	mStateIndexArray.Add(kIndex_Invalid);
	mIdDataIndexArray.Add(kIndex_Invalid);//#693
	/*#693
	mIdentifierCategoryIndexArray.Add(kIndex_Invalid);
	mIdentifierKeywordVariableIndexArray.Add(kIndex_Invalid);
	mIdentifierKeywordStaticTextArray.Add(GetEmptyText());
	mIdentifierMenuTextVariableIndexArray.Add(kIndex_Invalid);
	mIdentifierMenuTextStaticTextArray.Add(GetEmptyText());
	mIdentifierInfoTextVariableIndexArray.Add(kIndex_Invalid);
	mIdentifierInfoTextStaticTextArray.Add(GetEmptyText());
	mIdentifierCommentTextVariableIndexArray.Add(kIndex_Invalid);//RC2
	mIdentifierCommentTextStaticTextArray.Add(GetEmptyText());//RC2
	mIdentifierParentKeywordIndexArray.Add(kIndex_Invalid);//RC2
	mIdentifierParentKeywordStaticTextArray.Add(GetEmptyText());//RC2
	mIdentifierTypeIndexArray.Add(kIndex_Invalid);//RC2
	mIdentifierTypeStaticTextArray.Add(GetEmptyText());//RC2
	mIdentifierKeywordExceptionTextArray.AddNewObject();
	*/
	return mTypeArray.GetItemCount()-1;
}

//アクション追加
AIndex	ASyntaxDefinitionActionSequence::AddAction_SetVarWithTokenStack( const AIndex inVariableIndex, const ANumber inStartNumber, const ANumber inEndNumber )
{
	AIndex	index = AddAction(kSyntaxDefinitionActionType_SetVarWithTokenStack);
	mVariableIndexArray.Set(index,inVariableIndex);
	//#693 メモリ削減のため、mParameter1Array, mParameter2Arrayで共用
	/*#693mStartNumberArray*/mParameter1Array.Set(index,inStartNumber);
	/*#693mEndNumberArray*/mParameter2Array.Set(index,inEndNumber);
	return index;
}

//アクション追加 var系
AIndex	ASyntaxDefinitionActionSequence::AddAction_var( const ASyntaxDefinitionActionType inActionType, const AIndex inVariableIndex )
{
	AIndex	index = AddAction(inActionType);
	mVariableIndexArray.Set(index,inVariableIndex);
	return index;
}

//アクション追加 var系
AIndex	ASyntaxDefinitionActionSequence::AddAction_CatVar( const ASyntaxDefinitionActionType inActionType, 
		const AIndex inVariableIndex, const AIndex inVariableIndex1, const AIndex inVariableIndex2 )
{
	AIndex	index = AddAction(inActionType);
	mVariableIndexArray.Set(index,inVariableIndex);
	//#693 メモリ削減のため、mParameter1Array, mParameter2Arrayで共用
	/*#693mVariableIndex1Array*/mParameter1Array.Set(index,inVariableIndex1);
	/*#693mVariableIndex2Array*/mParameter2Array.Set(index,inVariableIndex2);
	return index;
}

//アクション追加 var系 RC2
AIndex	ASyntaxDefinitionActionSequence::AddAction_SetVar( const ASyntaxDefinitionActionType inActionType, 
		const AIndex inVariableIndex, const AIndex inVariableIndex1 )
{
	AIndex	index = AddAction(inActionType);
	//#693 メモリ削減のため、mParameter1Array, mParameter2Arrayで共用
	mVariableIndexArray.Set(index,inVariableIndex);
	/*#693mVariableIndex1Array*/mParameter1Array.Set(index,inVariableIndex1);
	return index;
}

//アクション追加 ChangeState系
AIndex	ASyntaxDefinitionActionSequence::AddAction_ChangeState( const ASyntaxDefinitionActionType inActionType, const AIndex inStateIndex )
{
	AIndex	index = AddAction(inActionType);
	mStateIndexArray.Set(index,inStateIndex);
	return index;
}

//アクション追加 ChangeStateIfVar系
AIndex	ASyntaxDefinitionActionSequence::AddAction_ChangeStateIfVar( const ASyntaxDefinitionActionType inActionType, 
		const AIndex inVariableIndex, const AIndex inStateIndex )
{
	AIndex	index = AddAction(inActionType);
	mStateIndexArray.Set(index,inStateIndex);
	mVariableIndexArray.Set(index,inVariableIndex);
	return index;
}

/**
アクション追加 SetVarStaticText
*/
AIndex	ASyntaxDefinitionActionSequence::AddAction_SetVarStaticText( const ASyntaxDefinitionActionType inActionType,
																	 const AIndex inVarIndex, const AText& inStaticText )
{
	AIndex	index = AddAction(inActionType);
	mVariableIndexArray.Set(index,inVarIndex);
	//識別子データにstatic textを格納する
	AIndex	idDataArrayIndex = mIdDataArray.AddNewObject();
	mIdDataIndexArray.Set(index,idDataArrayIndex);
	ASyntaxDefinitionActionSequenceIdData&	idData = mIdDataArray.GetObject(idDataArrayIndex);
	idData.mIdentifierKeywordVariableIndex = kIndex_Invalid;
	idData.mIdentifierKeywordStaticText = inStaticText;
	return index;
}

//アクション追加
AIndex	ASyntaxDefinitionActionSequence::AddAction_AddGlobalId( const AIndex inCategoryIndex,
		const AIndex inKeywordVariableIndex, const AText& inKeywordStaticText, 
		const AIndex inMenuTextVariableIndex, const AText& inMenuTextStaticText, 
		const AIndex inInfoTextVariableIndex, const AText& inInfoTextStaticText,
		const AIndex inCommentTextVariableIndex, const AText& inCommentTextStaticText,//RC2
		const AIndex inParentKeywordVariableIndex, const AText& inParentKeywordStaticText,//RC2
		const AIndex inTypeVariableIndex, const AText& inTypeStaticText,//RC2
		const AText& inKeywordExceptionText, const AIndex inOutlineLevel )//#875
{
	AIndex	index = AddAction(kSyntaxDefinitionActionType_AddGlobalId);
	//#693
	AIndex	idDataArrayIndex = mIdDataArray.AddNewObject();
	mIdDataIndexArray.Set(index,idDataArrayIndex);
	//
	ASyntaxDefinitionActionSequenceIdData&	idData = mIdDataArray.GetObject(idDataArrayIndex);
	idData.mIdentifierCategoryIndex = inCategoryIndex;
	idData.mIdentifierKeywordVariableIndex = inKeywordVariableIndex;
	idData.mIdentifierKeywordStaticText = inKeywordStaticText;
	idData.mIdentifierMenuTextVariableIndex = inMenuTextVariableIndex;
	idData.mIdentifierMenuTextStaticText = inMenuTextStaticText;
	idData.mIdentifierInfoTextVariableIndex = inInfoTextVariableIndex;
	idData.mIdentifierInfoTextStaticText = inInfoTextStaticText;
	idData.mIdentifierCommentTextVariableIndex = inCommentTextVariableIndex;//RC2
	idData.mIdentifierCommentTextStaticText = inCommentTextStaticText;//RC2
	idData.mIdentifierParentKeywordIndex = inParentKeywordVariableIndex;//RC2
	idData.mIdentifierParentKeywordStaticText = inParentKeywordStaticText;//RC2
	idData.mIdentifierTypeIndex = inTypeVariableIndex;//RC2
	idData.mIdentifierTypeStaticText = inTypeStaticText;//RC2
	ATextArray	exceptionArray;
	inKeywordExceptionText.Explode(',',exceptionArray);
	for( AIndex i = 0; i < exceptionArray.GetItemCount(); i++ )
	{
		AText	text;
		exceptionArray.Get(i,text);
		idData.mIdentifierKeywordExceptionText.Add(text);
	}
	idData.mIdentifierOutlineLevel = inOutlineLevel;//#875
	return index;
}

//アクション追加
AIndex	ASyntaxDefinitionActionSequence::AddAction_AddLocalId( const AIndex inCategoryIndex,
		const AIndex inKeywordVariableIndex, const AText& inKeywordStaticText, 
		const AIndex inInfoTextVariableIndex, const AText& inInfoTextStaticText,
		const AIndex inTypeVariableIndex, const AText& inTypeStaticText,//R0243
		const AText& inKeywordExceptionText )
{
	AIndex	index = AddAction(kSyntaxDefinitionActionType_AddLocalId);
	//#693
	AIndex	idDataArrayIndex = mIdDataArray.AddNewObject();
	mIdDataIndexArray.Set(index,idDataArrayIndex);
	//
	ASyntaxDefinitionActionSequenceIdData&	idData = mIdDataArray.GetObject(idDataArrayIndex);
	idData.mIdentifierCategoryIndex = inCategoryIndex;
	idData.mIdentifierKeywordVariableIndex = inKeywordVariableIndex;
	idData.mIdentifierKeywordStaticText = inKeywordStaticText;
	idData.mIdentifierMenuTextVariableIndex = kIndex_Invalid;
	idData.mIdentifierInfoTextVariableIndex = inInfoTextVariableIndex;
	idData.mIdentifierInfoTextStaticText = inInfoTextStaticText;
	idData.mIdentifierCommentTextVariableIndex = kIndex_Invalid;//RC2
	idData.mIdentifierParentKeywordIndex = kIndex_Invalid;//RC2
	idData.mIdentifierTypeIndex = inTypeVariableIndex;//RC2
	idData.mIdentifierTypeStaticText = inTypeStaticText;//RC2
	ATextArray	exceptionArray;
	inKeywordExceptionText.Explode(',',exceptionArray);
	for( AIndex i = 0; i < exceptionArray.GetItemCount(); i++ )
	{
		AText	text;
		exceptionArray.Get(i,text);
		idData.mIdentifierKeywordExceptionText.Add(text);
	}
	return index;
}

#pragma mark ===========================
#pragma mark [クラス]ASyntaxDefinitionState
#pragma mark ===========================
//state毎に作成されるクラス

#pragma mark ---コンストラクタ／デストラクタ

//コンストラクタ
ASyntaxDefinitionState::ASyntaxDefinitionState( AObjectArrayItem* inParent, ASyntaxDefinition& inSyntaxDefinition ) : AObjectArrayItem(inParent),
		mSyntaxDefinition(inSyntaxDefinition), /*mColorIsSet(false),*/ mExistDefaultConditionText(true), /*mNoMemoryState(false),*/ mNotCodeState(false),
		mStable(false), mIndentOnTheWay(false), mCommentState(false), mDefaultSequenceIndex(kIndex_Invalid), 
		mAllCategoryDisabled(false), mGlobalState(false), mStartSequenceIndex(kIndex_Invalid)//RC2
		,mNoSpellCheck(false)//R0199
		,mDisableDirective(false)//#467
		//,mSameColorAs(kIndex_Invalid)//#14
,mColorSlot(kIndex_Invalid)//#844
,mAfterClassState(false)//#717
,mSyntaxPartIndex(kIndex_Invalid)//#844
,mIgnoreCase(false), mEnableRecompletion(false), mDisableIndent(false)
,mHereDocumentEndIndex(kIndex_Invalid)
{
	for( long i = 0; i < 256; i++ )   mCharForSkipLine[i] = false;
}

ASyntaxDefinitionState::~ASyntaxDefinitionState()
{
}

#pragma mark ===========================

#pragma mark ---アクションシーケンス管理

//アクションシーケンスオブジェクト取得
ASyntaxDefinitionActionSequence&	ASyntaxDefinitionState::GetActionSequence( const AIndex inSequenceIndex )
{
	return mActionSequenceArray.GetObject(inSequenceIndex);
}

//アクションシーケンス追加
AIndex	ASyntaxDefinitionState::CreateActionSequence( const AText& inConditionTokenText )
{
	mActionSequenceConditionTextArray.Add(inConditionTokenText);
	//条件テキストの大文字小文字無視用データ
	if( mIgnoreCase == true )
	{
		AText	text;
		text.SetText(inConditionTokenText);
		text.ChangeCaseUpper();
		mActionSequenceConditionTextArray_UpperCase.Add(text);
	}
	//
	ASyntaxDefinitionActionSequenceFactory	factory(this,*this);
	return mActionSequenceArray.AddNewObject(factory);
}

AIndex	ASyntaxDefinitionState::CreateDefaultActionSequence()
{
	mExistDefaultConditionText = true;
	mDefaultSequenceIndex = CreateActionSequence(GetEmptyText());
	return mDefaultSequenceIndex;
}

//RC2
//startアクションシーケンス生成
AIndex	ASyntaxDefinitionState::CreateStartActionSequence()
{
	AText	nulltext;
	nulltext.Add(0);//ヒットしないようにNULL文字にしておく
	mStartSequenceIndex = CreateActionSequence(nulltext);
	return mStartSequenceIndex;
}

/**
ヒアドキュメント終了アクションシーケンス生成
*/
AIndex	ASyntaxDefinitionState::CreateHereDocumentEndActionSequence()
{
	AText	nulltext;
	nulltext.Add(0);//ヒットしないようにNULL文字にしておく
	mHereDocumentEndIndex = CreateActionSequence(nulltext);
	return mHereDocumentEndIndex;
}

//アクションシーケンス検索
AIndex	ASyntaxDefinitionState::FindActionSequenceIndex( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	AIndex	index = mActionSequenceConditionTextArray.Find(inTargetText,inTargetIndex,inTargetItemCount);
	if( index != kIndex_Invalid )
	{
		return index;
	}
	else
	{
		//そのままのテキストで見つからない場合、かつ、大文字小文字無視の場合、大文字化して検索
		if( mIgnoreCase == true )
		{
			return mActionSequenceConditionTextArray_UpperCase.Find(inTargetText,inTargetIndex,inTargetItemCount);
		}
		else
		{
			return kIndex_Invalid;
		}
	}
}

AIndex	ASyntaxDefinitionState::GetDefaultActionSequenceIndex() const
{
	return mDefaultSequenceIndex;
}

//RC2
//startアクションシーケンス番号取得
AIndex	ASyntaxDefinitionState::GetStartActionSequenceIndex() const
{
	return mStartSequenceIndex;
}

/**
ヒアドキュメント終了アクションシーケンス取得
*/
AIndex	ASyntaxDefinitionState::GetHereDocumentEndSequenceIndex() const
{
	return mHereDocumentEndIndex;
}

#pragma mark ===========================

#pragma mark ---

/*#844
//状態色取得
//#14 SameColorAsを指定された状態の場合は、他の状態の色を参照して返す
ABool	ASyntaxDefinitionState::GetStateColor( AColor& outColor ) const
{
	if( mSameColorAs == kIndex_Invalid )
	{
		outColor = mStateColor;
		return mColorIsSet;
		
	}
	else
	{
		return mSyntaxDefinition.GetSyntaxDefinitionState(mSameColorAs).GetStateColor(outColor);
	}
}
*/

//
//たとえば、"//"と"/*"と"/XXX"と"*/"があれば、
//mConditionTextByteCountArrayのインデックス"/"には{3,2}、インデックス"*"には{2}が入る。
//これにより、文法認識時、最初の1バイトで、
void	ASyntaxDefinitionState::MakeConditionTextByteCountArray()
{
	//256個のエリアを作成
	mConditionTextByteCountArray.DeleteAll();
	mConditionTextByteCountArrayIndexTable.DeleteAll();//#693
	/*#693
	for( AIndex i = 0; i < 256; i++ )
	{
		mConditionTextByteCountArray.AddNewObject();
	}
	*/
	//IndexTable初期化（対応するcharが存在する場合のみ、この次の処理でmConditionTextByteCountArrayのitemを生成し、そのindexを格納する。
	for( AIndex i = 0; i < 256; i++ )
	{
		mConditionTextByteCountArrayIndexTable.Add(kIndex_Invalid);
	}
	
	//------------------バイト数テーブル生成------------------
	//最初の１バイトの値ごとに、mActionSequenceConditionTextArrayにマッチする可能性のあるバイト数を保存する
	for( AIndex index = 0; index < mActionSequenceConditionTextArray.GetItemCount(); index++ )
	{
		AText	text;
		mActionSequenceConditionTextArray.Get(index,text);
		AItemCount	byteCount = text.GetItemCount();
		if( byteCount == 0 )   continue;
		AUChar	firstch = text.Get(0);
		//firstchに対応するmConditionTextByteCountArrayのitemのindexを取得し、未生成なら、生成する
		AIndex	conditionTextByteCountArrayIndex = mConditionTextByteCountArrayIndexTable.Get(firstch);
		if( conditionTextByteCountArrayIndex == kIndex_Invalid )
		{
			conditionTextByteCountArrayIndex = mConditionTextByteCountArray.AddNewObject();
			mConditionTextByteCountArrayIndexTable.Set(firstch,conditionTextByteCountArrayIndex);
		}
		//該当するmConditionTextByteCountArrayのitemにbyteCount未登録なら登録する
		if( mConditionTextByteCountArray.GetObject(conditionTextByteCountArrayIndex).Find(byteCount) == kIndex_Invalid )
		{
			mConditionTextByteCountArray.GetObject(conditionTextByteCountArrayIndex).Add(byteCount);
		}
	}
	//------------------大文字小文字無視検索用の条件文字列テーブルからバイト数テーブル生成------------------
	for( AIndex index = 0; index < mActionSequenceConditionTextArray_UpperCase.GetItemCount(); index++ )
	{
		AText	text;
		mActionSequenceConditionTextArray_UpperCase.Get(index,text);
		AItemCount	byteCount = text.GetItemCount();
		if( byteCount == 0 )   continue;
		AUChar	firstch = text.Get(0);
		//firstchに対応するmConditionTextByteCountArrayのitemのindexを取得し、未生成なら、生成する
		AIndex	conditionTextByteCountArrayIndex = mConditionTextByteCountArrayIndexTable.Get(firstch);
		if( conditionTextByteCountArrayIndex == kIndex_Invalid )
		{
			conditionTextByteCountArrayIndex = mConditionTextByteCountArray.AddNewObject();
			mConditionTextByteCountArrayIndexTable.Set(firstch,conditionTextByteCountArrayIndex);
		}
		//該当するmConditionTextByteCountArrayのitemにbyteCount未登録なら登録する
		if( mConditionTextByteCountArray.GetObject(conditionTextByteCountArrayIndex).Find(byteCount) == kIndex_Invalid )
		{
			mConditionTextByteCountArray.GetObject(conditionTextByteCountArrayIndex).Add(byteCount);
		}
	}
	
	//全てを降順にソート
	for( AIndex i = 0; i < /*#693 256*/mConditionTextByteCountArray.GetItemCount(); i++ )
	{
		mConditionTextByteCountArray.GetObject(i).Sort(false);
	}
}

/*#693
const AArray<AItemCount>&	ASyntaxDefinitionState::GetConditionTextByteCountArray( const AUChar inChar ) const
{
	return mConditionTextByteCountArray.GetObjectConst(inChar);
}
*/

/**
mConditionTextByteCountArrayIndexTableを取得（firstch毎の、mConditionTextByteCountArrayのitemのindex（未生成ならkIndex_Invalid））
mConditionTextByteCountArrayIndexTable: index：charコードに対応  内容：mConditionTextByteCountArrayのindex
#693
*/
const AArray<AIndex>&	ASyntaxDefinitionState::GetConditionTextByteCountArrayIndexTable() const
{
	return mConditionTextByteCountArrayIndexTable;
}

/**
mConditionTextByteCountArrayを取得
mConditionTextByteCountArray: index: item追加順  内容:存在しうるbytecountのソート済み配列
#693
*/
const AArray<AItemCount>&	ASyntaxDefinitionState::GetConditionTextByteCountArray( const AIndex inIndex ) const
{
	return mConditionTextByteCountArray.GetObjectConst(inIndex);
}

#pragma mark ===========================
#pragma mark [クラス]ASyntaxDefinition
#pragma mark ===========================
//文法定義を保持するクラス

#pragma mark ★★★設計メモ★★★
/*
設計メモ

文法解析には以下２種類がある
①状態、識別子を認識する解析
②状態＋インデントのみを認識する解析（表示、インデント等に使用される）

①は、テキストの編集後、編集した行から前にさかのぼって、初期状態の行まで戻り、そこから解析を行う
②は、行の最初から解析する。（さかのぼらない）

行ごとに保持している情報は、状態とPushed状態の２つだけであり、これ以外の情報を保持することはできない。
よって、状態遷移は、この２つの情報だけで実行可能としなければならない。フラグや変数によって、状態遷移先を変えること（条件分岐）は不可。
（行ごとに保持する情報を増やせば条件分岐もできるが、速度やメモリ容量の問題があり、非採用とした。）


TokenStackには、読み込んだ文字がtokenごとに格納される。
空白、タブ、改行コードは保存されない。
単語は１つの格納場所に保存される。記号は１文字ごとに保存される。
※改行コードは、条件テキストの比較対象ではあるが、TokenStackには格納されない。

*/

#pragma mark ===========================
#pragma mark ---コンストラクタ／デストラクタ

//コンストラクタ
ASyntaxDefinition::ASyntaxDefinition() : AObjectArrayItem(NULL), mImportLinkToProjectAll(false),mImportRecursiveAll(false)
,mIdInfoArgStartCharCount(0)//#225
,mParseResult(true)//#349
,mSyntaxErrorVarIndex(kIndex_Invalid),mSyntaxWarningVarIndex(kIndex_Invalid)
,mDisableAutoIndent(false)//#997
{
	//予約語をハッシュ登録
	RegisterReservedWord("state",									kReservedWord_state);
	RegisterReservedWord("default",									kReservedWord_default);
	RegisterReservedWord("lineend",									kReservedWord_lineend);
	RegisterReservedWord("category",								kReservedWord_category);
	RegisterReservedWord("tab",										kReservedWord_tab);//未使用
	RegisterReservedWord("first",									kReservedWord_first);
	RegisterReservedWord("regexp",									kReservedWord_regexp);
	RegisterReservedWord("start",									kReservedWord_start);//RC2
	RegisterReservedWord("heredocumentend",							kReservedWord_heredocumentend);
	//GlobalAttribute
	RegisterReservedWord("GlobalAttribute_SDFName",					kReservedWord_sdfname);//R0000
	RegisterReservedWord("GlobalAttribute_SDFVersion",				kReservedWord_sdfversion);//#349
	RegisterReservedWord("GlobalAttribute_InitialState",			kReservedWord_initialstate);//R0000
	RegisterReservedWord("GlobalAttribute_ImportLinkToProjectAll",	kReservedWord_ImportLinkToProjectAll);//win 080722
	RegisterReservedWord("GlobalAttribute_StartAlphabet",			kReservedWord_StartAlphabet);//win 080722
	RegisterReservedWord("GlobalAttribute_ContinuousAlphabet",		kReservedWord_ContinuousAlphabet);//win 080722
	RegisterReservedWord("GlobalAttribute_ImportRecursive",			kReservedWord_ImportRecursive);//win 080725
	RegisterReservedWord("GlobalAttribute_ImportRecursiveAll",		kReservedWord_ImportRecursiveAll);//win 080725
	RegisterReservedWord("GlobalAttribute_IdInfoAutoDisplay",		kReservedWord_IdInfoAutoDisplay);//RC2
	RegisterReservedWord("GlobalAttribute_IdInfoArgStart",			kReservedWord_IdInfoArgStart);//RC2
	RegisterReservedWord("GlobalAttribute_IdInfoArgEnd",			kReservedWord_IdInfoArgEnd);//RC2
	RegisterReservedWord("GlobalAttribute_IdInfoArgDelimiter",		kReservedWord_IdInfoArgDelimiter);//RC2
	RegisterReservedWord("GlobalAttribute_ClassString",				kReservedWord_ClassString);//R0243
	RegisterReservedWord("GlobalAttribute_InstanceString",			kReservedWord_InstanceString);//R0243
	RegisterReservedWord("GlobalAttribute_Directive",				kReservedWord_Directive);//#467
	RegisterReservedWord("GlobalAttribute_DirectiveIf",				kReservedWord_DirectiveIf);//#467
	RegisterReservedWord("GlobalAttribute_DirectiveIfdef",			kReservedWord_DirectiveIfdef);//#467
	RegisterReservedWord("GlobalAttribute_DirectiveIfndef",			kReservedWord_DirectiveIfndef);//#467
	RegisterReservedWord("GlobalAttribute_DirectiveElse",			kReservedWord_DirectiveElse);//#467
	RegisterReservedWord("GlobalAttribute_DirectiveElif",			kReservedWord_DirectiveElif);//#467
	RegisterReservedWord("GlobalAttribute_DirectiveEndif",			kReservedWord_DirectiveEndif);//#467
	RegisterReservedWord("GlobalAttribute_SyntaxPart",				kReservedWord_GlobalAttribute_SyntaxPart);//#844
	RegisterReservedWord("GlobalAttribute_DisableAutoIndent",		kReservedWord_GlobalAttribute_DisableAutoIndent);//#997
	//StateAttribute
	RegisterReservedWord("StateAttribute_Stable",					kReservedWord_stable);
	RegisterReservedWord("StateAttribute_Comment",					kReservedWord_comment);
	/*obsolete*/RegisterReservedWord("StateAttribute_NoMemory",					kReservedWord_nomemory);
	RegisterReservedWord("StateAttribute_NotCode",					kReservedWord_notcode);
	RegisterReservedWord("StateAttribute_DisableAllCategory",		kReservedWord_disableallcategory);//R0000
	/*obsolete*/RegisterReservedWord("StateAttribute_EnableCategory",			kReservedWord_enablecategory);//R0000
	/*obsolete?*/RegisterReservedWord("StateAttribute_GlobalState",				kReservedWord_globalstate);//B0000
	RegisterReservedWord("StateAttribute_IndentOnTheWay",			kReservedWord_indentontheway);
	/*obsolete*/RegisterReservedWord("StateAttribute_DefaultColor",				kReservedWord_defaultcolor);
	/*obsolete*/RegisterReservedWord("StateAttribute_DefaultImportColor",		kReservedWord_defaultimportcolor);
	/*obsolete*/RegisterReservedWord("StateAttribute_DefaultLocalColor",		kReservedWord_defaultlocalcolor);
	/*obsolete*/RegisterReservedWord("StateAttribute_DisplayName",				kReservedWord_displayname);
	/*obsolete*/RegisterReservedWord("StateAttribute_DefaultKeyword",			kReservedWord_defaultkeyword);
	/*obsolete*/RegisterReservedWord("StateAttribute_LineSkip",					kReservedWord_lineskip);//未使用
	/*obsolete*/RegisterReservedWord("StateAttribute_MenuBold",					kReservedWord_menubold);
	RegisterReservedWord("StateAttribute_NoSpellCheck",				kReservedWord_NoSpellCheck);//R0199
	/*obsolete*/RegisterReservedWord("StateAttribute_SameStateAs",				kReservedWord_SameColorAs);//#14
	RegisterReservedWord("StateAttribute_DisableDirective",			kReservedWord_DisableDirective);//#467
	RegisterReservedWord("StateAttribute_AfterParentKeyword",		kReservedWord_AfterParentKeyword);//#717
	RegisterReservedWord("StateAttribute_ColorSlot",				kReservedWord_StateAttribute_ColorSlot);//#844
	RegisterReservedWord("StateAttribute_SyntaxPart",				kReservedWord_StateAttribute_SyntaxPart);//#844
	RegisterReservedWord("StateAttribute_IgnoreCase",				kReservedWord_StateAttribute_IgnoreCase);//
	RegisterReservedWord("StateAttribute_EnableRecompletion",		kReservedWord_StateAttribute_EnableRecompletion);//
	RegisterReservedWord("StateAttribute_DisableIndent",			kReservedWord_StateAttribute_DisableIndent);//
	//CategoryAttribute #349
	/*obsolete*/RegisterReservedWord("CategoryAttribute_DefaultColor",			kReservedWord_category_defaultcolor);//#349
	/*obsolete*/RegisterReservedWord("CategoryAttribute_DefaultLocalColor",		kReservedWord_category_defaultlocalcolor);//#349
	/*obsolete*/RegisterReservedWord("CategoryAttribute_DefaultImportColor",	kReservedWord_category_defaultimportcolor);//#349
	/*obsolete*/RegisterReservedWord("CategoryAttribute_DefaultKeyword",		kReservedWord_category_defaultkeyword);//#349
	RegisterReservedWord("CategoryAttribute_MenuBold",				kReservedWord_category_menubold);//#349
	RegisterReservedWord("CategoryAttribute_ColorSlot",				kReservedWord_CategoryAttribute_ColorSlot);//#844
	RegisterReservedWord("CategoryAttribute_DisplayName",			kReservedWord_CategoryAttribute_DisplayName);//
	//Action
	RegisterReservedWord("SetVarWithTokenStack",					kReservedWord_SetVarWithTokenStack);
	RegisterReservedWord("SetVarStartFromNextChar",					kReservedWord_SetVarStartFromNextChar);
	RegisterReservedWord("SetVarStartAtPrevToken",					kReservedWord_SetVarStartAtPrevToken);//RC2
	RegisterReservedWord("SetVarStart",								kReservedWord_SetVarStart);
	RegisterReservedWord("SetVarStartOrContinue",					kReservedWord_SetVarStartOrContinue);//RC2
	RegisterReservedWord("SetVarEnd",								kReservedWord_SetVarEnd);
	RegisterReservedWord("SetVarEndAtNextChar",						kReservedWord_SetVarEndAtNextChar);
	RegisterReservedWord("SetVarEndAtPrevToken",					kReservedWord_SetVarEndAtPrevToken);
	RegisterReservedWord("SetVarLastWord",							kReservedWord_SetVarLastWord);//B0306
	RegisterReservedWord("CatVar",									kReservedWord_CatVar);
	RegisterReservedWord("SetVar",									kReservedWord_SetVar);//RC2
	//RegisterReservedWord("SetVarStaticText",						kReservedWord_SetVarStaticText);//
	RegisterReservedWord("SetVarTypeOfVar",							kReservedWord_SetVarTypeOfVar);//
	RegisterReservedWord("ClearVar",								kReservedWord_ClearVar);//RC2
	/*未使用*/RegisterReservedWord("RegisterParentChild",						kReservedWord_RegisterParentChild);//#717
	RegisterReservedWord("BlockStart",								kReservedWord_BlockStart);
	RegisterReservedWord("ChangeState",								kReservedWord_ChangeState);
	RegisterReservedWord("ChangeStateIfBlockEnd",					kReservedWord_ChangeStateIfBlockEnd);
	RegisterReservedWord("ChangeStateIfNotAfterAlphabet",			kReservedWord_ChangeStateIfNotAfterAlphabet);//R0241
	RegisterReservedWord("PushStateIfNotAfterAlphabet",				kReservedWord_PushStateIfNotAfterAlphabet);//R0241
	RegisterReservedWord("ChangeStateIfTokenEqualVar",				kReservedWord_ChangeStateIfTokenEqualVar);//R0241
	RegisterReservedWord("SetVarCurrentToken",						kReservedWord_SetVarCurrentToken);//R0241
	RegisterReservedWord("SetVarNextToken",							kReservedWord_SetVarNextToken);
	RegisterReservedWord("SetVarRegExpGroup1",						kReservedWord_SetVarRegExpGroup1);
	RegisterReservedWord("ChangeStateFromNextChar",					kReservedWord_ChangeStateFromNextChar);//R0241
	RegisterReservedWord("PushState",								kReservedWord_PushState);
	RegisterReservedWord("PushStateFromNextLine",					kReservedWord_PushStateFromNextLine);
	RegisterReservedWord("PopState",								kReservedWord_PopState);
	RegisterReservedWord("PopStateFromNextChar",					kReservedWord_PopStateFromNextChar);
	RegisterReservedWord("IncIndentFromNext",						kReservedWord_IncIndentFromNext);
	RegisterReservedWord("DecIndentFromCurrent",					kReservedWord_DecIndentFromCurrent); 
	RegisterReservedWord("DecIndentFromNext",						kReservedWord_DecIndentFromNext); 
	RegisterReservedWord("DecIndentOnlyCurrent",					kReservedWord_DecIndentOnlyCurrent);
	RegisterReservedWord("SetZeroIndentOnlyCurrent",				kReservedWord_SetZeroIndentOnlyCurrent);
	RegisterReservedWord("IndentLabel",								kReservedWord_IndentLabel);
	RegisterReservedWord("EscapeNextChar",							kReservedWord_EscapeNextChar);
	RegisterReservedWord("AddGlobalId",								kReservedWord_AddGlobalId);
	RegisterReservedWord("AddLocalId",								kReservedWord_AddLocalId);
	RegisterReservedWord("ClearTokenStack",							kReservedWord_ClearTokenStack);
	RegisterReservedWord("Import",									kReservedWord_Import);
	RegisterReservedWord("SyntaxWarning",							kReservedWord_SyntaxWarning);
	RegisterReservedWord("SyntaxError",								kReservedWord_SyntaxError);
	//win 080722
	for( AIndex i = 0; i < 256; i++ )
	{
		mStartAlphabet[i] = false;
		mContinuousAlphabet[i] = false;
	}
	//デフォルトアルファベット
	for( AUChar i = 'A'; i <= 'Z'; i++ )
	{
		mStartAlphabet[i] = true;
		mContinuousAlphabet[i] = true;
	}
	for( AUChar i = 'a'; i <= 'z'; i++ )
	{
		mStartAlphabet[i] = true;
		mContinuousAlphabet[i] = true;
	}
	for( AUChar i = '0'; i <= '9'; i++ )
	{
		mStartAlphabet[i] = true;
		mContinuousAlphabet[i] = true;
	}
	mStartAlphabet['_'] = true;
	mContinuousAlphabet['_'] = true;
	//RC2
	for( AIndex i = 0; i < 256; i++ )
	{
		mIdInfoAutoDisplayChar[i] = false;
		mIdInfoArgStart[i] = false;
		mIdInfoArgEnd[i] = false;
		mIdInfoArgDelimiter[i] = false;
	}
}

void	ASyntaxDefinition::RegisterReservedWord( AConstCharPtr inWordText, const ASyntaxDefinitionReservedWord inWordNumber )
{
	AText	text;
	text.SetCstring(inWordText);
	mReservedWord_Text.Add(text);
	mReservedWord_Number.Add(inWordNumber);
}

void	ASyntaxDefinition::DeleteAll()
{
	mStateArray.DeleteAll();
	mStateNameArray.DeleteAll();
	mVariableNameArray.DeleteAll();
	mVariableArray_BlockStartFlag.DeleteAll();//#695
	mKeywordArray.DeleteAll();
	mKeywordArray_Category.DeleteAll();
	mCategory_Name.DeleteAll();
	mCategory_DefaultColor.DeleteAll();
	mCategory_DefaultImportColor.DeleteAll();
	mCategory_DefaultLocalColor.DeleteAll();
	mCategory_MenuTextStyle.DeleteAll();
	mCategory_ColorSlot.DeleteAll();//#844
	mImportRecursiveSuffix.DeleteAll();//B0000
	mRegExp_Text.DeleteAll();//B0000
	mRegExp_StateIndex.DeleteAll();//B0000
	mRegExp_SequenceIndex.DeleteAll();//B0000
	mInitialState_Suffix.DeleteAll();
	mInitialState_Index.DeleteAll();
	mImportLinkToProjectAll = false;
	mImportRecursiveAll = false;
	mSyntaxPartTextArray.DeleteAll();
	mSyntaxPartArray_StartAlphabet.DeleteAll();
	mSyntaxPartArray_ContinuousAlphabet.DeleteAll();
	mSyntaxPartArray_TailAlphabet.DeleteAll();
	mSyntaxPartArray_CorrespondStart.DeleteAll();
	mSyntaxPartArray_CorrespondEnd.DeleteAll();
	mSyntaxPartArray_SeparatorName.DeleteAll();
	mSyntaxPartArray_InitialStateIndex.DeleteAll();
	//#997
	mDisableAutoIndent = false;
}

#pragma mark ===========================

#pragma mark ---文法定義ファイルParse

//SyntaxDefinitionFileのParse実行
ABool	ASyntaxDefinition::Parse( const AFileAcc& inFileAcc )
{
	//#349
	DeleteAll();
	//SyntaxErrorのエラー文字列保存用変数
	AText	syntaxErrorVarName("_____lastsyntaxerror");
	mSyntaxErrorVarIndex = mVariableNameArray.Add(syntaxErrorVarName);
	mVariableArray_BlockStartFlag.Add(false);
	//SyntaxWarningのワーニング文字列保存用変数
	AText	syntaxWarningVarName("_____lastsyntaxwarning");
	mSyntaxWarningVarIndex = mVariableNameArray.Add(syntaxWarningVarName);
	mVariableArray_BlockStartFlag.Add(false);
	//
	AText	syntaxDefinitionText;
	inFileAcc.ReadTo(syntaxDefinitionText);
	syntaxDefinitionText.ConvertReturnCodeToCR();
	ABool	result = true;
	try
	{
		AStTextPtr	textptr(syntaxDefinitionText,0,syntaxDefinitionText.GetItemCount());
		const AUChar*	ptr = textptr.GetPtr();
		const AItemCount	textlength = textptr.GetByteCount();
		
		//==================パス1==================
		//state名、変数名全検出
		mCurrentLineNumber = 0;
		for( AIndex pos = 0; pos < textlength; )
		{
			//state, SetVarWithTokenStackを検索して、見つけたら、名前をそれぞれ登録
			AText	token;
			ASyntaxDefinitionReservedWord	reservedWordNumber;
			AUChar	ch;
			Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
			switch(reservedWordNumber)
			{
				//状態
			  case kReservedWord_state:
				{
					//state<HERE>(statename)
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);//(
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);//statename
					//状態名ｇ重複している場合は、エラー
					if( mStateNameArray.Find(token) != kIndex_Invalid )
					{
						throw 111;
					}
					//
					mStateNameArray.Add(token);
					ASyntaxDefinitionStateFactory	factory(this,*this);
					mStateArray.AddNewObject(factory);
					break;
				}
				//変数
			  case kReservedWord_SetVarWithTokenStack:
			  case kReservedWord_SetVarStartFromNextChar:
			  case kReservedWord_SetVarStartAtPrevToken://RC2
			  case kReservedWord_SetVarStart:
			  case kReservedWord_BlockStart:
			  case kReservedWord_CatVar:
			  case kReservedWord_SetVarLastWord://B0306
			  case kReservedWord_SetVarStartOrContinue://RC2
			  case kReservedWord_SetVar://RC2
			  case kReservedWord_SetVarTypeOfVar:
			  //case kReservedWord_SetVarStaticText:
			  case kReservedWord_ClearVar://RC2
			  case kReservedWord_SetVarCurrentToken://R0241
			  case kReservedWord_SetVarNextToken:
			  case kReservedWord_SetVarRegExpGroup1:
				{
					//SetVarWithTokenStack<HERE>(variablename,
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);//(
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);//variablename
					if( mVariableNameArray.Find(token) == kIndex_Invalid )
					{
						mVariableNameArray.Add(token);
						mVariableArray_BlockStartFlag.Add(reservedWordNumber==kReservedWord_BlockStart);//#695
					}
					break;
				}
				//カテゴリー
				case kReservedWord_category:
				{
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);//[
					Parse_GetText(ptr,textlength,pos,']',token);
					if( mCategory_Name.Find(token) == kIndex_Invalid )
					{
						mCategory_Name.Add(token);
						mCategory_DefaultColor.Add(kColor_Black);
						mCategory_DefaultImportColor.Add(kColor_Black);
						mCategory_DefaultLocalColor.Add(kColor_Black);
						mCategory_MenuTextStyle.Add(kTextStyle_Normal);
						mCategory_ColorSlot.Add(kIndex_Invalid);//#844
					}
					break;
				}
			  default:
				{
					break;
				}
			}
		}
		
		//==================パス2==================
		mCurrentLineNumber = 0;
		for( AIndex pos = 0; pos < textlength; )
		{
			AText	token;
			ASyntaxDefinitionReservedWord	reservedWordNumber;
			AUChar	ch;
			
			//<HERE>state[statename]
			Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
			
			//終了判定
			if( pos >= textlength )   break;
			
			//state
			if( reservedWordNumber == kReservedWord_state )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
				//状態のIndexを取得
				AIndex	stateIndex = mStateNameArray.Find(token);
				if( stateIndex == kIndex_Invalid )   throw 101;
				
				//state(statename)<HERE>={
				Parse_CheckToken(ptr,textlength,pos,')');
				Parse_CheckToken(ptr,textlength,pos,'{');
				while( pos < textlength )
				{
					//状態属性：state[statename]={<HERE>lineskip()　もしくは
					//アクションシーケンス：state[statename]={<HERE>{"X",
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == '{' )
					{
						//state[statename]={{<HERE>"X",
						Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
						AIndex	seqIndex = kIndex_Invalid;
						switch(reservedWordNumber)
						{
							//default（他の条件テキストに一致しなかった場合のアクションシーケンス）
						  case kReservedWord_default:
							{
								seqIndex = mStateArray.GetObject(stateIndex).CreateDefaultActionSequence();
								break;
							}
							//lineend（改行文字）
						  case kReservedWord_lineend:
							{
								AText	lineend;
								lineend.Add(kUChar_LineEnd);
								seqIndex = mStateArray.GetObject(stateIndex).CreateActionSequence(lineend);
								break;
							}
							//regexp
						  case kReservedWord_regexp:
							{
								Parse_CheckToken(ptr,textlength,pos,'"');
								Parse_GetText(ptr,textlength,pos,'\"',token);//GetStringは"の後ろまで読む
								seqIndex = mStateArray.GetObject(stateIndex).CreateActionSequence(GetEmptyText());
								RegisterRegExp(token,stateIndex,seqIndex);
								break;
							}
							//start（状態開始時に実行するアクションシーケンス）RC2
						  case kReservedWord_start:
							{
								seqIndex = mStateArray.GetObject(stateIndex).CreateStartActionSequence();
								break;
							}
							//heredocumentend
						  case kReservedWord_heredocumentend:
							{
								seqIndex = mStateArray.GetObject(stateIndex).CreateHereDocumentEndActionSequence();
								break;
							}
							//{<HERE>"X"
						  default:
							{
								if( ch != '\"' )   throw 102;
								Parse_GetText(ptr,textlength,pos,'\"',token);//GetStringは"の後ろまで読む
								seqIndex = mStateArray.GetObject(stateIndex).CreateActionSequence(token);
								break;
							}
						}
						//{"X"<HERE>,{
						Parse_CheckToken(ptr,textlength,pos,',');
						Parse_CheckToken(ptr,textlength,pos,'{');
						while( pos < textlength )
						{
							//各アクション
							//<HERE>actionname(
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							if( ch == '}' )   break;
							switch(reservedWordNumber)
							{
								//SetVarWithTokenStack
							  case kReservedWord_SetVarWithTokenStack:
								{
									//SetVarWithTokenStack<HERE>(varname,startnum,count)
									Parse_CheckToken(ptr,textlength,pos,'(');
									//変数名読み込み
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,',');
									ANumber	startnum, endnum;
									//開始トークンインデックス読み込み
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									if( reservedWordNumber == kReservedWord_first )
									{
										startnum = kSyntaxTokenStackIndex_First;
									}
									else
									{
										if( token.ParseIntegerNumber(startnum) == false )   throw 1;
									}
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									if( ch == kUChar_Comma )
									{
										//終了トークンインデックス読み込み
										Parse_GetNumber(ptr,textlength,pos,endnum);
										Parse_CheckToken(ptr,textlength,pos,')');
									}
									else if( ch == ')' )
									{
										endnum = kSyntaxTokenStackIndex_Next;
									}
									else throw 299;
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction_SetVarWithTokenStack(varIndex,startnum,endnum);
									break;
								}
								//SetVarStartFromNextChar
							  case kReservedWord_SetVarStartFromNextChar:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction_var(kSyntaxDefinitionActionType_SetVarStartFromNextChar,varIndex);
									break;
								}
								//SetVarStartAtPrevToken
							  case kReservedWord_SetVarStartAtPrevToken:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction_var(kSyntaxDefinitionActionType_SetVarStartAtPrevToken,varIndex);
									break;
								}
								//SetVarStart
							  case kReservedWord_SetVarStart:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction_var(kSyntaxDefinitionActionType_SetVarStart,varIndex);
									break;
								}
								//SetVarStartOrContinue RC2
							  case kReservedWord_SetVarStartOrContinue:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction_var(kSyntaxDefinitionActionType_SetVarStartOrContinue,varIndex);
									break;
								}
								//ClearVar RC2
							  case kReservedWord_ClearVar:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction_var(kSyntaxDefinitionActionType_ClearVar,varIndex);
									break;
								}
								//SetVarEnd
							  case kReservedWord_SetVarEnd:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction_var(kSyntaxDefinitionActionType_SetVarEnd,varIndex);
									break;
								}
								//SetVarEnd
							  case kReservedWord_SetVarEndAtNextChar:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
									AddAction_var(kSyntaxDefinitionActionType_SetVarEndAtNextChar,varIndex);
									break;
								}
								//SetVarEnd
							  case kReservedWord_SetVarEndAtPrevToken:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_var(kSyntaxDefinitionActionType_SetVarEndAtPrevToken,varIndex);
									break;
								}
								//SetVarLastWord B0306
							  case kReservedWord_SetVarLastWord:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_var(kSyntaxDefinitionActionType_SetVarLastWord,varIndex);
									break;
								}
								//SetVarCurrentToken R0241
							  case kReservedWord_SetVarCurrentToken:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_var(kSyntaxDefinitionActionType_SetVarCurrentToken,varIndex);
									break;
								}
								//SetVarNextToken
							  case kReservedWord_SetVarNextToken:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_var(kSyntaxDefinitionActionType_SetVarNextToken,varIndex);
									break;
								}
								//SetVarRegExpGroup1
							  case kReservedWord_SetVarRegExpGroup1:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_var(kSyntaxDefinitionActionType_SetVarRegExpGroup1,varIndex);
									break;
								}
								//BlockStart
							  case kReservedWord_BlockStart:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_var(kSyntaxDefinitionActionType_BlockStart,varIndex);
									break;
								}
								//CatVar
							  case kReservedWord_CatVar:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,',');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex1 = mVariableNameArray.Find(token);
									if( varIndex1 == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,',');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex2 = mVariableNameArray.Find(token);
									if( varIndex2 == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_CatVar(kSyntaxDefinitionActionType_CatVar,varIndex,varIndex1,varIndex2);
									break;
								}
								//SetVar RC2
							  case kReservedWord_SetVar:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,',');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									if( ch == '\"' )
									{
										//固定文字列を変数に設定
										AText	staticText;
										Parse_GetText(ptr,textlength,pos,'\"',staticText);
										Parse_CheckToken(ptr,textlength,pos,')');
										mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
												AddAction_SetVarStaticText(kSyntaxDefinitionActionType_SetVarStaticText,varIndex,staticText);
									}
									else
									{
										//変数を変数に設定
										AIndex	varIndex1 = mVariableNameArray.Find(token);
										if( varIndex1 == kIndex_Invalid )   throw 201;
										Parse_CheckToken(ptr,textlength,pos,')');
										mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
												AddAction_SetVar(kSyntaxDefinitionActionType_SetVar,varIndex,varIndex1);
									}
									break;
								}
								//SetVar RC2
							  case kReservedWord_SetVarTypeOfVar:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_var(kSyntaxDefinitionActionType_SetVarTypeOfVar,varIndex);
									break;
								}
								//#717
								//RegisterParentChild
							  case kReservedWord_RegisterParentChild:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,',');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex1 = mVariableNameArray.Find(token);
									if( varIndex1 == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_SetVar(kSyntaxDefinitionActionType_RegisterParentChild,varIndex,varIndex1);
									break;
								}
								//IncIndentFromNext
							  case kReservedWord_IncIndentFromNext:
								{
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction(kSyntaxDefinitionActionType_IncIndentFromNext);
									break;
								}
								//DecIndentFromCurrent
							  case kReservedWord_DecIndentFromCurrent:
								{
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction(kSyntaxDefinitionActionType_DecIndentFromCurrent);
									break;
								}
								//DecIndentFromCurrent
							  case kReservedWord_DecIndentFromNext:
								{
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction(kSyntaxDefinitionActionType_DecIndentFromNext);
									break;
								}
								//DecIndentOnlyCurrent
							  case kReservedWord_DecIndentOnlyCurrent:
								{
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction(kSyntaxDefinitionActionType_DecIndentOnlyCurrent);
									break;
								}
								//IndentLabel
							  case kReservedWord_IndentLabel:
								{
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction(kSyntaxDefinitionActionType_IndentLabel);
									break;
								}
								//SetZeroIndentOnlyCurrent
							  case kReservedWord_SetZeroIndentOnlyCurrent:
								{
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction(kSyntaxDefinitionActionType_SetZeroIndentOnlyCurrent);
									break;
								}
								//ClearTokenStack
							  case kReservedWord_ClearTokenStack:
								{
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction(kSyntaxDefinitionActionType_ClearTokenStack);
									break;
								}
								//EscapeNextChar
							  case kReservedWord_EscapeNextChar:
								{
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction(kSyntaxDefinitionActionType_EscapeNextChar);
									break;
								}
								//ChangeState
							  case kReservedWord_ChangeState:
								{
									//ChangeState<HERE>(statename)
									Parse_CheckToken(ptr,textlength,pos,'(');
									AText	statename;
									Parse_GetText(ptr,textlength,pos,')',statename);
									AIndex	dstStateIndex = mStateNameArray.Find(statename);
									if( dstStateIndex == kIndex_Invalid )   throw 202;
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_ChangeState(kSyntaxDefinitionActionType_ChangeState,dstStateIndex);
									break;
								}
								//ChangeStateIfBlockEnd
							  case kReservedWord_ChangeStateIfBlockEnd:
								{
									//ChangeStateIfBlockEnd<HERE>(statename)
									Parse_CheckToken(ptr,textlength,pos,'(');
									//
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,',');
									//
									AText	statename;
									Parse_GetText(ptr,textlength,pos,')',statename);
									AIndex	dstStateIndex = mStateNameArray.Find(statename);
									if( dstStateIndex == kIndex_Invalid )   throw 201;
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_ChangeStateIfVar(kSyntaxDefinitionActionType_ChangeStateIfBlockEnd,varIndex,dstStateIndex);
									break;
								}
								//ChangeStateIfNotAfterAlphabet R0241
							  case kReservedWord_ChangeStateIfNotAfterAlphabet:
								{
									//ChangeStateIfNotAfterAlphabet<HERE>(statename)
									Parse_CheckToken(ptr,textlength,pos,'(');
									AText	statename;
									Parse_GetText(ptr,textlength,pos,')',statename);
									AIndex	dstStateIndex = mStateNameArray.Find(statename);
									if( dstStateIndex == kIndex_Invalid )   throw 202;
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_ChangeState(kSyntaxDefinitionActionType_ChangeStateIfNotAfterAlphabet,dstStateIndex);
									break;
								}
								//PushState R0241
							  case kReservedWord_PushStateIfNotAfterAlphabet:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									AText	statename;
									Parse_GetText(ptr,textlength,pos,')',statename);
									AIndex	dstStateIndex = mStateNameArray.Find(statename);
									if( dstStateIndex == kIndex_Invalid )   throw 202;
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_ChangeState(kSyntaxDefinitionActionType_PushStateIfNotAfterAlphabet,dstStateIndex);
									break;
								}
								//ChangeStateIfTokenEqualVar R0241
							  case kReservedWord_ChangeStateIfTokenEqualVar:
								{
									//ChangeStateIfBlockEnd<HERE>(statename)
									Parse_CheckToken(ptr,textlength,pos,'(');
									//
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,',');
									//
									AText	statename;
									Parse_GetText(ptr,textlength,pos,')',statename);
									AIndex	dstStateIndex = mStateNameArray.Find(statename);
									if( dstStateIndex == kIndex_Invalid )   throw 202;
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_ChangeStateIfVar(kSyntaxDefinitionActionType_ChangeStateIfTokenEqualVar,varIndex,dstStateIndex);
									break;
								}
								//ChangeStateFromNextChar R0241
							  case kReservedWord_ChangeStateFromNextChar:
								{
									//ChangeState<HERE>(statename)
									Parse_CheckToken(ptr,textlength,pos,'(');
									AText	statename;
									Parse_GetText(ptr,textlength,pos,')',statename);
									AIndex	dstStateIndex = mStateNameArray.Find(statename);
									if( dstStateIndex == kIndex_Invalid )   throw 202;
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_ChangeState(kSyntaxDefinitionActionType_ChangeStateFromNextChar,dstStateIndex);
									break;
								}
								//PushState
							  case kReservedWord_PushState:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									AText	statename;
									Parse_GetText(ptr,textlength,pos,')',statename);
									AIndex	dstStateIndex = mStateNameArray.Find(statename);
									if( dstStateIndex == kIndex_Invalid )   throw 202;
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_ChangeState(kSyntaxDefinitionActionType_PushState,dstStateIndex);
									break;
								}
								//PushStateFromNextLine
							  case kReservedWord_PushStateFromNextLine:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									AText	statename;
									Parse_GetText(ptr,textlength,pos,')',statename);
									AIndex	dstStateIndex = mStateNameArray.Find(statename);
									if( dstStateIndex == kIndex_Invalid )   throw 202;
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_ChangeState(kSyntaxDefinitionActionType_PushStateFromNextLine,dstStateIndex);
									break;
								}
								//PopState
							  case kReservedWord_PopState:
								{
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction(kSyntaxDefinitionActionType_PopState);
									break;
								}
								//PopStateFromNextChar
							  case kReservedWord_PopStateFromNextChar:
								{
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction(kSyntaxDefinitionActionType_PopStateFromNextChar);
									break;
								}
								//Import
							  case kReservedWord_Import:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	varIndex = mVariableNameArray.Find(token);
									if( varIndex == kIndex_Invalid )   throw 201;
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction_var(kSyntaxDefinitionActionType_Import,varIndex);
									break;
								}
								//SyntaxWarning
							  case kReservedWord_SyntaxWarning:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_CheckToken(ptr,textlength,pos,'\"');
									//固定文字列を変数に設定
									AText	staticText;
									Parse_GetText(ptr,textlength,pos,'\"',staticText);
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_SetVarStaticText(kSyntaxDefinitionActionType_SyntaxWarning,
																	   mSyntaxWarningVarIndex,staticText);
									break;
								}
								//SyntaxError
							  case kReservedWord_SyntaxError:
								{
									Parse_CheckToken(ptr,textlength,pos,'(');
									Parse_CheckToken(ptr,textlength,pos,'\"');
									//固定文字列を変数に設定
									AText	staticText;
									Parse_GetText(ptr,textlength,pos,'\"',staticText);
									Parse_CheckToken(ptr,textlength,pos,')');
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
											AddAction_SetVarStaticText(kSyntaxDefinitionActionType_SyntaxError,
																	   mSyntaxErrorVarIndex,staticText);
									break;
								}
								//AddGlobalId
							  case kReservedWord_AddGlobalId:
								{
									//category, keyword, menutext, infotextは静的文字列("XXX")、変数どちらかを使用
									AIndex	categoryIndex = kIndex_Invalid;
									AIndex	keywordVariableIndex = kIndex_Invalid;
									AIndex	menuTextVariableIndex = kIndex_Invalid;
									AIndex	infoTextVariableIndex = kIndex_Invalid;
									AText	keywordStaticText, menuTextStaticText, infoTextStaticText;
									AIndex	commentTextVariableIndex = kIndex_Invalid;//RC2
									AText	commentStaticText;//RC2
									AIndex	parentTextVariableIndex = kIndex_Invalid;//RC2
									AText	parentStaticText;//RC2
									AIndex	typeTextVariableIndex = kIndex_Invalid;//RC2
									AText	typeStaticText;//RC2
									
									//AddGlobalId<HERE>(category,keyword,menutext,infotext)
									Parse_CheckToken(ptr,textlength,pos,'(');
									//(<HERE>[category],
									Parse_CheckToken(ptr,textlength,pos,'[');
									AText	categoryname;
									Parse_GetText(ptr,textlength,pos,']',categoryname);
									categoryIndex = mCategory_Name.Find(categoryname);
									if( categoryIndex == kIndex_Invalid )   throw 107;
									Parse_CheckToken(ptr,textlength,pos,',');
									//(category,<HERE>keyword
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									if( ch == '\"' )
									{
										Parse_GetText(ptr,textlength,pos,'\"',keywordStaticText);
									}
									else
									{
										keywordVariableIndex = mVariableNameArray.Find(token);
										if( keywordVariableIndex == kIndex_Invalid )   throw 201;
									}
									Parse_CheckToken(ptr,textlength,pos,',');
									//(category,keyword,<HERE>menutext
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									if( ch == '\"' )
									{
										Parse_GetText(ptr,textlength,pos,'\"',menuTextStaticText);
									}
									else
									{
										menuTextVariableIndex = mVariableNameArray.Find(token);
										if( menuTextVariableIndex == kIndex_Invalid )   throw 201;
									}
									Parse_CheckToken(ptr,textlength,pos,',');
									//(category,keyword,menutext,<HERE>infotext
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									if( ch == '\"' )
									{
										Parse_GetText(ptr,textlength,pos,'\"',infoTextStaticText);
									}
									else
									{
										infoTextVariableIndex = mVariableNameArray.Find(token);
										if( infoTextVariableIndex == kIndex_Invalid )   throw 201;
									}
									Parse_CheckToken(ptr,textlength,pos,',');
									//RC2 CommentText
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									if( ch == '\"' )
									{
										Parse_GetText(ptr,textlength,pos,'\"',commentStaticText);
									}
									else
									{
										commentTextVariableIndex = mVariableNameArray.Find(token);
										if( commentTextVariableIndex == kIndex_Invalid )   throw 201;
									}
									Parse_CheckToken(ptr,textlength,pos,',');
									//RC2 parent
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									if( ch == '\"' )
									{
										Parse_GetText(ptr,textlength,pos,'\"',parentStaticText);
									}
									else
									{
										parentTextVariableIndex = mVariableNameArray.Find(token);
										if( parentTextVariableIndex == kIndex_Invalid )   throw 201;
									}
									Parse_CheckToken(ptr,textlength,pos,',');
									//RC2 type
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									if( ch == '\"' )
									{
										Parse_GetText(ptr,textlength,pos,'\"',typeStaticText);
									}
									else
									{
										typeTextVariableIndex = mVariableNameArray.Find(token);
										if( typeTextVariableIndex == kIndex_Invalid )   throw 201;
									}
									Parse_CheckToken(ptr,textlength,pos,',');
									//(category,keyword,menutext,infotext,<HERE>"xxx,xxx")
									Parse_CheckToken(ptr,textlength,pos,'\"');
									AText	keywordExceptionText;
									Parse_GetText(ptr,textlength,pos,'\"',keywordExceptionText);
									//(category,keyword,menutext,infotext,"xxx,xxx"<HERE>)
									//#875
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									AIndex	outlineLevel = 1;//省略時はレベル1（SDF v1.0との互換性のため）
									if( ch == ',' )
									{
										Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
										outlineLevel = token.GetNumber();
										//
										Parse_CheckToken(ptr,textlength,pos,')');
									}
									else if( ch != ')' )
									{
										mErrorExpectedChar = ')';
										throw 3;
									}
									//アクション登録
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction_AddGlobalId(
											categoryIndex,keywordVariableIndex,keywordStaticText,
											menuTextVariableIndex,menuTextStaticText,infoTextVariableIndex,infoTextStaticText,
											commentTextVariableIndex,commentStaticText,//RC2
											parentTextVariableIndex,parentStaticText,//RC2
											typeTextVariableIndex,typeStaticText,//RC2
												keywordExceptionText,outlineLevel);//#875
									break;
								}
								//AddLocalId
							  case kReservedWord_AddLocalId:
								{
									//category, keyword, infotextは静的文字列("XXX")、変数どちらかを使用
									AIndex	categoryIndex = kIndex_Invalid;
									AIndex	keywordVariableIndex = kIndex_Invalid;
									AIndex	infoTextVariableIndex = kIndex_Invalid;
									AText	keywordStaticText, menuTextStaticText, infoTextStaticText;
									AIndex	typeTextVariableIndex = kIndex_Invalid;//R0243
									AText	typeStaticText;//R0243
									
									//AddGlobalId<HERE>(category,keyword,infotext)
									Parse_CheckToken(ptr,textlength,pos,'(');
									//(<HERE>[category],
									Parse_CheckToken(ptr,textlength,pos,'[');
									AText	categoryname;
									Parse_GetText(ptr,textlength,pos,']',categoryname);
									categoryIndex = mCategory_Name.Find(categoryname);
									Parse_CheckToken(ptr,textlength,pos,',');
									//(category,<HERE>keyword
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									if( ch == '\"' )
									{
										Parse_GetText(ptr,textlength,pos,'\"',keywordStaticText);
									}
									else
									{
										keywordVariableIndex = mVariableNameArray.Find(token);
										if( keywordVariableIndex == kIndex_Invalid )   throw 201;
									}
									Parse_CheckToken(ptr,textlength,pos,',');
									//(category,keyword,<HERE>infotext
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									if( ch == '\"' )
									{
										Parse_GetText(ptr,textlength,pos,'\"',infoTextStaticText);
									}
									else
									{
										infoTextVariableIndex = mVariableNameArray.Find(token);
										if( infoTextVariableIndex == kIndex_Invalid )   throw 201;
									}
									Parse_CheckToken(ptr,textlength,pos,',');
									//R0243 type
									Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
									if( ch == '\"' )
									{
										Parse_GetText(ptr,textlength,pos,'\"',typeStaticText);
									}
									else
									{
										typeTextVariableIndex = mVariableNameArray.Find(token);
										if( typeTextVariableIndex == kIndex_Invalid )   throw 201;
									}
									Parse_CheckToken(ptr,textlength,pos,',');
									//(category,keyword,infotext,<HERE>"xxx,xxx")
									Parse_CheckToken(ptr,textlength,pos,'\"');
									AText	keywordExceptionText;
									Parse_GetText(ptr,textlength,pos,'\"',keywordExceptionText);
									//(category,keyword,infotext,"xxx,xxx"<HERE>)
									Parse_CheckToken(ptr,textlength,pos,')');
									//アクション登録
									mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).AddAction_AddLocalId(
											categoryIndex,keywordVariableIndex,keywordStaticText,
											infoTextVariableIndex,infoTextStaticText,
											typeTextVariableIndex,typeStaticText,//R0243
											keywordExceptionText);
									break;
								}
							  default:
								{
									break;
								}
							}
							//{action(), action()<HERE>, action()
							//もしくは
							//{action(), action(), action()<HERE>}
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							//}なら終了
							if( ch == '}' )   break;
							//,以外ならエラー
							if( ch != ',' )   throw 103;
						}
						//{"X",{action(), action(), action()}<HERE>}
						Parse_CheckToken(ptr,textlength,pos,'}');
					}
					//state定義終了判定
					else if( ch == '}' )   break;
					//state属性
					else switch(reservedWordNumber)
					{
						//lineskip（未使用）
					  case kReservedWord_lineskip:
						{
							//lineskip<HERE>("X")
							Parse_CheckToken(ptr,textlength,pos,'(');
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							if( ch == '\"' )
							{
								Parse_GetText(ptr,textlength,pos,'\"',token);
								if( token.GetLength() == 1 )
								{
									mStateArray.GetObject(stateIndex).AddLineSkipChar(token.Get(0));
								}
								else throw 0;
							}
							else switch(reservedWordNumber)
							{
							  case kReservedWord_tab:
								{
									mStateArray.GetObject(stateIndex).AddLineSkipChar(kUChar_Tab);
									break;
								}
							  default:
								{
									throw 0;
									break;
								}
							}
							Parse_CheckToken(ptr,textlength,pos,')');
							break;
						}
						//defaultcolor
					  case kReservedWord_defaultcolor:
						{
							//defaultcolor<HERE>(XXXXXX)
							Parse_CheckToken(ptr,textlength,pos,'(');
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							if( ch == '\"' )
							{
								Parse_GetText(ptr,textlength,pos,'\"',token);
								if( token.GetLength() == 6 )
								{
									AColor	color = AColorWrapper::GetColorByHTMLFormatColor(token);
									//AUtil::GetColorBy6CharRGBText(token,color);
									//#844 mStateArray.GetObject(stateIndex).SetStateColor(color);
								}
								else throw 104;
							}
							Parse_CheckToken(ptr,textlength,pos,')');
							break;
						}
						//SyntaxPart
					  case kReservedWord_StateAttribute_SyntaxPart:
						{
							//StateAttribute_SyntaxPart<HERE>(XXXXXX)
							Parse_CheckToken(ptr,textlength,pos,'(');
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							if( ch == '\"' )
							{
								Parse_GetText(ptr,textlength,pos,'\"',token);
								if( token.GetLength() > 0 )
								{
									//
									AIndex	syntaxPartIndex = mSyntaxPartTextArray.Find(token);
									if( syntaxPartIndex == kIndex_Invalid )   throw 0;
									//状態のSyntax Part設定
									mStateArray.GetObject(stateIndex).SetStateSyntaxPartIndex(syntaxPartIndex);
								}
								else throw 104;
							}
							Parse_CheckToken(ptr,textlength,pos,')');
							break;
						}
						//#14
					  case kReservedWord_SameColorAs:
						{
							Parse_CheckToken(ptr,textlength,pos,'(');
							AText	statename;
							Parse_GetText(ptr,textlength,pos,')',statename);
							AIndex	dstStateIndex = mStateNameArray.Find(statename);
							if( dstStateIndex == kIndex_Invalid )   throw 105;
							if( dstStateIndex == stateIndex )   throw 105;//同じstateIndexはNG（データ取得時に無限ループになる）
							//#844 mStateArray.GetObject(stateIndex).SetSameColorAs(dstStateIndex);
							break;
						}
						//
					  case kReservedWord_StateAttribute_IgnoreCase:
						{
							mStateArray.GetObject(stateIndex).SetIgnoreCase();
							break;
						}
						//補完候補再表示をenableにする
					  case kReservedWord_StateAttribute_EnableRecompletion:
						{
							mStateArray.GetObject(stateIndex).SetEnableRecompletion();
							break;
						}
						//インデントdisableにする
					  case kReservedWord_StateAttribute_DisableIndent:
						{
							mStateArray.GetObject(stateIndex).SetDisableIndent();
							break;
						}
						//
					  case kReservedWord_nomemory:
						{
							//#844 mStateArray.GetObject(stateIndex).SetNoMemoryState();
							break;
						}
						//
					  case kReservedWord_notcode:
						{
							mStateArray.GetObject(stateIndex).SetNotCodeState();
							break;
						}
						//#467
					  case kReservedWord_DisableDirective:
						{
							mStateArray.GetObject(stateIndex).SetDisableDirective();
							break;
						}
						//#717
					  case kReservedWord_AfterParentKeyword:
						{
							mStateArray.GetObject(stateIndex).SetAfterClassState();
							/*
							Parse_CheckToken(ptr,textlength,pos,'(');
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							if( ch == '\"' )
							{
								Parse_GetText(ptr,textlength,pos,'\"',token);
								mStateArray.GetObject(stateIndex).SetAfterClassState_ParentKeyword(token);
							}
							Parse_CheckToken(ptr,textlength,pos,')');
							*/
							break;
						}
						//R0199
					  case kReservedWord_NoSpellCheck:
						{
							mStateArray.GetObject(stateIndex).SetNoSpellCheck();
							break;
						}
						//
					  case kReservedWord_comment:
						{
							mStateArray.GetObject(stateIndex).SetCommentState();
							break;
						}
						//
					  case kReservedWord_indentontheway:
						{
							mStateArray.GetObject(stateIndex).SetIndentOnTheWay();
							break;
						}
						//
					  case kReservedWord_stable:
						{
							mStateArray.GetObject(stateIndex).SetStable();
							break;
						}
					  case kReservedWord_globalstate://B0000
						{
							mStateArray.GetObject(stateIndex).SetGlobalState();
							break;
						}
						//
					  case kReservedWord_displayname:
						{
							//displayname<HERE>("XXXXXX")
							Parse_CheckToken(ptr,textlength,pos,'(');
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							if( ch == '\"' )
							{
								Parse_GetText(ptr,textlength,pos,'\"',token);
								mStateArray.GetObject(stateIndex).SetStateDisplayName(token);
							}
							else throw 106;
							Parse_CheckToken(ptr,textlength,pos,')');
							break;
						}
						//R0000 カテゴリー可否
					  case kReservedWord_disableallcategory:
						{
							mStateArray.GetObject(stateIndex).SetAllCategoryDisabled();
							break;
						}
						//R0000 カテゴリー可否
					  case kReservedWord_enablecategory:
						{
							Parse_CheckToken(ptr,textlength,pos,'(');
							Parse_CheckToken(ptr,textlength,pos,'[');
							AText	categoryname;
							Parse_GetText(ptr,textlength,pos,']',categoryname);
							AIndex	categoryIndex = mCategory_Name.Find(categoryname);
							if( categoryIndex == kIndex_Invalid )   throw 203;
							//#844 mStateArray.GetObject(stateIndex).AddEnebledCategoryArray(categoryIndex);
							Parse_CheckToken(ptr,textlength,pos,')');
							break;
						}
						//#844
					  case kReservedWord_StateAttribute_ColorSlot:
						{
							//CategoryAttribute_ColorSlot<HERE>(0)
							Parse_CheckToken(ptr,textlength,pos,'(');
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							mStateArray.GetObject(stateIndex).SetColorSlot(token.GetNumber());
							Parse_CheckToken(ptr,textlength,pos,')');
							break;
						}
					  default:
						{
							throw 0;
							break;
						}
					}
				}
			}
			//category
			else if( reservedWordNumber == kReservedWord_category )
			{
				AText	categoryname;
				Parse_CheckToken(ptr,textlength,pos,'[');
				Parse_GetText(ptr,textlength,pos,']',categoryname);
				AIndex	categoryIndex = mCategory_Name.Find(categoryname);
				if( categoryIndex == kIndex_Invalid )   throw 107;
				Parse_CheckToken(ptr,textlength,pos,'{');
				while( pos < textlength )
				{
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == '}' )
					{
						break;
					}
					else switch(reservedWordNumber)
					{
					  case kReservedWord_category_defaultcolor:
						{
							Parse_CheckToken(ptr,textlength,pos,'(');
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							if( ch == '\"' )
							{
								Parse_GetText(ptr,textlength,pos,'\"',token);
								if( token.GetLength() == 6 )
								{
									AColor	color = AColorWrapper::GetColorByHTMLFormatColor(token);
									//AUtil::GetColorBy6CharRGBText(token,color);
									mCategory_DefaultColor.Set(categoryIndex,color);
								}
								else throw 104;
							}
							Parse_CheckToken(ptr,textlength,pos,')');
							break;
						}
					  case kReservedWord_category_defaultimportcolor:
						{
							Parse_CheckToken(ptr,textlength,pos,'(');
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							if( ch == '\"' )
							{
								Parse_GetText(ptr,textlength,pos,'\"',token);
								if( token.GetLength() == 6 )
								{
									AColor	color = AColorWrapper::GetColorByHTMLFormatColor(token);;
									//AUtil::GetColorBy6CharRGBText(token,color);
									mCategory_DefaultImportColor.Set(categoryIndex,color);
								}
								else throw 104;
							}
							Parse_CheckToken(ptr,textlength,pos,')');
							break;
						}
					  case kReservedWord_category_defaultlocalcolor:
						{
							Parse_CheckToken(ptr,textlength,pos,'(');
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							if( ch == '\"' )
							{
								Parse_GetText(ptr,textlength,pos,'\"',token);
								if( token.GetLength() == 6 )
								{
									AColor	color = AColorWrapper::GetColorByHTMLFormatColor(token);;
									//AUtil::GetColorBy6CharRGBText(token,color);
									mCategory_DefaultLocalColor.Set(categoryIndex,color);
								}
								else throw 104;
							}
							Parse_CheckToken(ptr,textlength,pos,')');
							break;
						}
					  case kReservedWord_category_defaultkeyword:
						{
							Parse_CheckToken(ptr,textlength,pos,'(');
							while( pos < textlength )
							{
								Parse_CheckToken(ptr,textlength,pos,'\"');
								AText	keyword;
								Parse_GetText(ptr,textlength,pos,'\"',keyword);
								if( keyword.GetItemCount() > 0 )
								{
									mKeywordArray.Add(keyword);
									mKeywordArray_Category.Add(categoryIndex);
								}
								Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
								if( ch == kUChar_Comma )   continue;
								else if( ch == ')' )   break;
								else throw 108;
							}
							break;
						}
						//
					  case kReservedWord_category_menubold:
						{
							mCategory_MenuTextStyle.Set(categoryIndex,kTextStyle_Bold);
							break;
						}
						//#844
					  case kReservedWord_CategoryAttribute_ColorSlot:
						{
							Parse_CheckToken(ptr,textlength,pos,'(');
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							mCategory_ColorSlot.Set(categoryIndex,token.GetNumber());
							Parse_CheckToken(ptr,textlength,pos,')');
							break;
						}
						//CategoryAttribute_DisplayName
					  case kReservedWord_CategoryAttribute_DisplayName:
						{
							//CategoryAttribute_DisplayName(0,"name")
							Parse_CheckToken(ptr,textlength,pos,'(');
							Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
							//言語コード
							AIndex	languageIndex = token.GetNumber();
							//カテゴリ表示名
							Parse_CheckToken(ptr,textlength,pos,',');
							Parse_CheckToken(ptr,textlength,pos,'\"');
							Parse_GetText(ptr,textlength,pos,'\"',token);
							Parse_CheckToken(ptr,textlength,pos,')');
							//現在の言語と同じならカテゴリ名上書き
							if( GetApp().NVI_GetLanguage() == static_cast<ALanguage>(languageIndex) )
							{
								mCategory_Name.Set(categoryIndex,token);
							}
							break;
						}
					}
				}
			}
			//R0000
			else if( reservedWordNumber == kReservedWord_sdfname )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				Parse_CheckToken(ptr,textlength,pos,'\"');
				Parse_GetText(ptr,textlength,pos,'\"',mSDFName);
				Parse_CheckToken(ptr,textlength,pos,')');
			}
			//R0000
			else if( reservedWordNumber == kReservedWord_initialstate )
			{
				AText	suffix;
				Parse_CheckToken(ptr,textlength,pos,'(');
				Parse_CheckToken(ptr,textlength,pos,'\"');
				Parse_GetText(ptr,textlength,pos,'\"',suffix);
				Parse_CheckToken(ptr,textlength,pos,',');
				AText	statename;
				Parse_GetText(ptr,textlength,pos,')',statename);
				AIndex	stateIndex = mStateNameArray.Find(statename);
				if( stateIndex == kIndex_Invalid )   throw 202;
				mInitialState_Suffix.Add(suffix);
				mInitialState_Index.Add(stateIndex);
			}
			//win 080722
			else if( reservedWordNumber == kReservedWord_ImportLinkToProjectAll )
			{
				mImportLinkToProjectAll = true;
			}
			else if( reservedWordNumber == kReservedWord_StartAlphabet )
			{
				for( AIndex i = 0; i < 256; i++ )
				{
					mStartAlphabet[i] = false;
				}
				AText	t;
				Parse_CheckToken(ptr,textlength,pos,'(');
				Parse_CheckToken(ptr,textlength,pos,'\"');
				Parse_GetText(ptr,textlength,pos,'\"',t);
				Parse_CheckToken(ptr,textlength,pos,')');
				for( AIndex i = 0; i < t.GetItemCount(); i++ )
				{
					mStartAlphabet[t.Get(i)] = true;
				}
			}
			else if( reservedWordNumber == kReservedWord_ContinuousAlphabet )
			{
				for( AIndex i = 0; i < 256; i++ )
				{
					mContinuousAlphabet[i] = false;
				}
				AText	t;
				Parse_CheckToken(ptr,textlength,pos,'(');
				Parse_CheckToken(ptr,textlength,pos,'\"');
				Parse_GetText(ptr,textlength,pos,'\"',t);
				Parse_CheckToken(ptr,textlength,pos,')');
				for( AIndex i = 0; i < t.GetItemCount(); i++ )
				{
					mContinuousAlphabet[t.Get(i)] = true;
				}
			}
			//win 080725
			else if( reservedWordNumber == kReservedWord_ImportRecursive )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	suffix;
					Parse_GetText(ptr,textlength,pos,'\"',suffix);
					mImportRecursiveSuffix.Add(suffix);
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 109;
				}
			}
			else if( reservedWordNumber == kReservedWord_ImportRecursiveAll )
			{
				mImportRecursiveAll = true;
			}
			//RC2
			else if( reservedWordNumber == kReservedWord_IdInfoAutoDisplay )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	t;
					Parse_GetText(ptr,textlength,pos,'\"',t);
					for( AIndex i = 0; i < t.GetItemCount(); i++ )
					{
						mIdInfoAutoDisplayChar[t.Get(i)] = true;
						//#853 mIdInfoAutoDisplayCharCount++;//#225
					}
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
			else if( reservedWordNumber == kReservedWord_IdInfoArgStart )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	t;
					Parse_GetText(ptr,textlength,pos,'\"',t);
					for( AIndex i = 0; i < t.GetItemCount(); i++ )
					{
						mIdInfoArgStart[t.Get(i)] = true;
						mIdInfoArgStartCharCount++;//#853
					}
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
			else if( reservedWordNumber == kReservedWord_IdInfoArgEnd )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	t;
					Parse_GetText(ptr,textlength,pos,'\"',t);
					for( AIndex i = 0; i < t.GetItemCount(); i++ )
					{
						mIdInfoArgEnd[t.Get(i)] = true;
					}
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
			else if( reservedWordNumber == kReservedWord_IdInfoArgDelimiter )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	t;
					Parse_GetText(ptr,textlength,pos,'\"',t);
					for( AIndex i = 0; i < t.GetItemCount(); i++ )
					{
						mIdInfoArgDelimiter[t.Get(i)] = true;
					}
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
			//#997
			else if( reservedWordNumber == kReservedWord_GlobalAttribute_DisableAutoIndent )
			{
				mDisableAutoIndent = true;
			}
			//
			else if( reservedWordNumber == kReservedWord_GlobalAttribute_SyntaxPart )
			{
				//SyntaxPartのプロパティ設定
				Parse_CheckToken(ptr,textlength,pos,'(');
				AText	syntaxpart;
				Parse_CheckToken(ptr,textlength,pos,'\"');
				Parse_GetText(ptr,textlength,pos,'\"',syntaxpart);
				Parse_CheckToken(ptr,textlength,pos,',');
				AText	separatorName;
				Parse_CheckToken(ptr,textlength,pos,'\"');
				Parse_GetText(ptr,textlength,pos,'\"',separatorName);
				Parse_CheckToken(ptr,textlength,pos,',');
				AText	initialStateName;
				Parse_GetText(ptr,textlength,pos,',',initialStateName);
				AText	startAlphabet;
				Parse_CheckToken(ptr,textlength,pos,'\"');
				Parse_GetText(ptr,textlength,pos,'\"',startAlphabet);
				Parse_CheckToken(ptr,textlength,pos,',');
				AText	continuousAlphabet;
				Parse_CheckToken(ptr,textlength,pos,'\"');
				Parse_GetText(ptr,textlength,pos,'\"',continuousAlphabet);
				Parse_CheckToken(ptr,textlength,pos,',');
				AText	tailAlphabet;
				Parse_CheckToken(ptr,textlength,pos,'\"');
				Parse_GetText(ptr,textlength,pos,'\"',tailAlphabet);
				//SyntaxPart名登録
				AIndex	syntaxPartIndex = mSyntaxPartTextArray.Add(syntaxpart);
				//パート分割線に表示する名前
				mSyntaxPartArray_SeparatorName.Add(separatorName);
				//文法パートの開始state（デフォルト文法パート指定時に使用する）
				AIndex	stateIndex = mStateNameArray.Find(initialStateName);
				if( stateIndex == kIndex_Invalid )   throw 202;
				mSyntaxPartArray_InitialStateIndex.Add(stateIndex);
				//単語アルファベット設定
				mSyntaxPartArray_StartAlphabet.AddNewObject();
				for( AIndex i = 0; i < 256; i++ )
				{
					mSyntaxPartArray_StartAlphabet.GetObject(syntaxPartIndex).Add(false);
				}
				for( AIndex i = 0; i < startAlphabet.GetItemCount(); i++ )
				{
					AUChar	ch = startAlphabet.Get(i);
					mSyntaxPartArray_StartAlphabet.GetObject(syntaxPartIndex).Set(ch,true);
				}
				//
				mSyntaxPartArray_ContinuousAlphabet.AddNewObject();
				for( AIndex i = 0; i < 256; i++ )
				{
					mSyntaxPartArray_ContinuousAlphabet.GetObject(syntaxPartIndex).Add(false);
				}
				for( AIndex i = 0; i < continuousAlphabet.GetItemCount(); i++ )
				{
					AUChar	ch = continuousAlphabet.Get(i);
					mSyntaxPartArray_ContinuousAlphabet.GetObject(syntaxPartIndex).Set(ch,true);
				}
				//
				mSyntaxPartArray_TailAlphabet.AddNewObject();
				for( AIndex i = 0; i < 256; i++ )
				{
					mSyntaxPartArray_TailAlphabet.GetObject(syntaxPartIndex).Add(false);
				}
				for( AIndex i = 0; i < tailAlphabet.GetItemCount(); i++ )
				{
					AUChar	ch = tailAlphabet.Get(i);
					mSyntaxPartArray_TailAlphabet.GetObject(syntaxPartIndex).Set(ch,true);
				}
				//対応文字列初期化
				mSyntaxPartArray_CorrespondStart.AddNewObject();
				mSyntaxPartArray_CorrespondEnd.AddNewObject();
				//対応文字列読み込み
				AUChar	ch;
				Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
				while(ch == ',')
				{
					AText	correspondStart;
					Parse_CheckToken(ptr,textlength,pos,'\"');
					Parse_GetText(ptr,textlength,pos,'\"',correspondStart);
					Parse_CheckToken(ptr,textlength,pos,',');
					AText	correspondEnd;
					Parse_CheckToken(ptr,textlength,pos,'\"');
					Parse_GetText(ptr,textlength,pos,'\"',correspondEnd);
					//
					mSyntaxPartArray_CorrespondStart.GetObject(syntaxPartIndex).Add(correspondStart);
					mSyntaxPartArray_CorrespondEnd.GetObject(syntaxPartIndex).Add(correspondEnd);
					//
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
				}
				if( ch != ')' )   throw 110;
			}
			//R0243
			else if( reservedWordNumber == kReservedWord_ClassString )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	suffix;
					Parse_GetText(ptr,textlength,pos,'\"',suffix);
					mClassStringArray.Add(suffix);
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
			else if( reservedWordNumber == kReservedWord_InstanceString )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	suffix;
					Parse_GetText(ptr,textlength,pos,'\"',suffix);
					mInstanceStringArray.Add(suffix);
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
			//#467
			else if( reservedWordNumber == kReservedWord_Directive )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	suffix;
					Parse_GetText(ptr,textlength,pos,'\"',suffix);
					mDirectiveStringArray.Add(suffix);
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
			else if( reservedWordNumber == kReservedWord_DirectiveIf )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	suffix;
					Parse_GetText(ptr,textlength,pos,'\"',suffix);
					mDirectiveIfStringArray.Add(suffix);
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
			else if( reservedWordNumber == kReservedWord_DirectiveIfdef )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	suffix;
					Parse_GetText(ptr,textlength,pos,'\"',suffix);
					mDirectiveIfdefStringArray.Add(suffix);
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
			else if( reservedWordNumber == kReservedWord_DirectiveIfndef )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	suffix;
					Parse_GetText(ptr,textlength,pos,'\"',suffix);
					mDirectiveIfndefStringArray.Add(suffix);
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
			else if( reservedWordNumber == kReservedWord_DirectiveElse )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	suffix;
					Parse_GetText(ptr,textlength,pos,'\"',suffix);
					mDirectiveElseStringArray.Add(suffix);
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
			else if( reservedWordNumber == kReservedWord_DirectiveElif )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	suffix;
					Parse_GetText(ptr,textlength,pos,'\"',suffix);
					mDirectiveElifStringArray.Add(suffix);
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
			else if( reservedWordNumber == kReservedWord_DirectiveEndif )
			{
				Parse_CheckToken(ptr,textlength,pos,'(');
				while(true)
				{
					Parse_CheckToken(ptr,textlength,pos,'\"');
					AText	suffix;
					Parse_GetText(ptr,textlength,pos,'\"',suffix);
					mDirectiveEndifStringArray.Add(suffix);
					AUChar	ch;
					Parse_GetToken(ptr,textlength,pos,token,reservedWordNumber,ch);
					if( ch == ')' )   break;
					else if( ch != ',' )   throw 110;
				}
			}
		}
	}
	catch( int inErrorNumber )//#349
	{
		mErrorNumber = inErrorNumber;
		result = false;
		DeleteAll();
		fprintf(stderr,"SyntaxDefinition ParseError(Line:%d err:%d)\n",mCurrentLineNumber+1,mErrorNumber);
	}
	catch(...)
	{
		//#349 fprintf(stderr,"SyntaxDefinition ParseError(Line:%d)\n",mCurrentLineNumber+1);
		result = false;
		DeleteAll();//#349
	}
	//最低１つの状態（初期状態）が存在することを保証するため、状態が１つも無かったら、空状態を生成しておく
	//検討
	if( mStateArray.GetItemCount() == 0 )
	{
		mStateNameArray.Add(GetEmptyText());
		ASyntaxDefinitionStateFactory	factory(this,*this);
		mStateArray.AddNewObject(factory);
	}
	//
	for( AIndex i = 0; i < mStateArray.GetItemCount(); i++ )
	{
		mStateArray.GetObject(i).MakeConditionTextByteCountArray();
	}
	mParseResult = result;//#349
	return result;
}

//トークン読み込み、予約語検索
void	ASyntaxDefinition::Parse_GetToken( const AUChar* inTextPtr, const AItemCount inTextLen, AIndex& ioPosition, 
										  AText& outToken, ASyntaxDefinitionReservedWord& outReservedWord, AUChar& outChar )
{
	outChar = 0;
	//トークン１つ読み込む。ただし、改行コードを読んだ場合は、飛ばして、次のトークンを読み込む。
	while(ioPosition < inTextLen)
	{
		if( AText::SkipTabSpace(inTextPtr,inTextLen,ioPosition,inTextLen) == false )   break;
		//コメント記号(#)から改行まではとばす
		if( inTextPtr[ioPosition] == '#' )
		{
			for( ; ioPosition < inTextLen; ioPosition++ )
			{
				if( inTextPtr[ioPosition] == kUChar_LineEnd )
				{
					ioPosition++;
					break;
				}
			}
			mCurrentLineNumber++;
			continue;
		}
		//
		AText::GetToken(inTextPtr,inTextLen,ioPosition,outToken);
		if( outToken.GetLength() >= 1 )   outChar = outToken.Get(0);
		if( outChar != kUChar_LineEnd )   break;//改行コードは飛ばす
		mCurrentLineNumber++;
	}
	//予約語検索
	AIndex reservedWordIndex = mReservedWord_Text.Find(outToken);
	if( reservedWordIndex != kIndex_Invalid )
	{
		outReservedWord = mReservedWord_Number.Get(reservedWordIndex);
	}
	else
	{
		outReservedWord = kReservedWord_Invalid;
	}
}

//""で囲まれた文字列読み込み（inDelimiterまで読み込む）
void	ASyntaxDefinition::Parse_GetText( const AUChar* inTextPtr, const AItemCount inTextLen, AIndex& ioPosition, 
										 const AUChar inDelimiter, AText& outText )
{
	outText.DeleteAll();
	while(ioPosition < inTextLen )
	{
		AUChar ch = inTextPtr[ioPosition];
		ioPosition++;
		if( ch == kUChar_Backslash )
		{
			ch = inTextPtr[ioPosition];
			ioPosition++;
			switch(ch)
			{
			  case 't':
				{
					outText.Add(kUChar_Tab);
					break;
				}
			  case 'r':
				{
					outText.Add(kUChar_LineEnd);
					break;
				}
			  default:
				{
					outText.Add(ch);
					break;
				}
			}
		}
		else
		{
			if( ch == inDelimiter )   break;
			outText.Add(ch);
		}
	}
}

//整数読み込み
void	ASyntaxDefinition::Parse_GetNumber( const AUChar* inTextPtr, const AItemCount inTextLen, AIndex& ioPosition, ANumber& outNumber )
{
	AText	token;
	ASyntaxDefinitionReservedWord	reservedWordNumber;
	AUChar	ch;
	Parse_GetToken(inTextPtr,inTextLen,ioPosition,token,reservedWordNumber,ch);
	if( token.ParseIntegerNumber(outNumber) == false )   throw 1;
}

//トークンチェック
void 	ASyntaxDefinition::Parse_CheckTokenReservedWord( const AUChar* inTextPtr, const AItemCount inTextLen, AIndex& ioPosition, 
														const AIndex inReservedWordNumber )
{
	AText	token;
	ASyntaxDefinitionReservedWord	reservedWordNumber;
	AUChar	ch;
	Parse_GetToken(inTextPtr,inTextLen,ioPosition,token,reservedWordNumber,ch);
	mErrorExpectedReservedWordIndex = inReservedWordNumber;//#349
	if( reservedWordNumber != inReservedWordNumber )   throw 2;
}

//トークンチェック
void 	ASyntaxDefinition::Parse_CheckToken( const AUChar* inTextPtr, const AItemCount inTextLen, AIndex& ioPosition, const AUChar inChar )
{
	AText	token;
	ASyntaxDefinitionReservedWord	reservedWordNumber;
	AUChar	ch;
	Parse_GetToken(inTextPtr,inTextLen,ioPosition,token,reservedWordNumber,ch);
	mErrorExpectedChar = inChar;//#349
	if( ch != inChar )   throw 3;
}

/**
*/
void	ASyntaxDefinition::GetErrorText( AText& outText ) const
{
	switch(mErrorNumber)
	{
	  case 0:
	  default:
		{
			outText.SetCstring("Unknown Error (E000)");
			break;
		}
	  case 1:
		{
			outText.SetCstring("Syntax Error: Number should be written. (E001)");
			break;
		}
	  case 2:
		{
			outText.SetCstring("Syntax Error: Reserved word \"");
			outText.AddText(mReservedWord_Text.GetTextConst(mErrorExpectedReservedWordIndex));
			outText.AddCstring("\" should be written. (E002)");
			break;
		}
	  case 3:
		{
			outText.SetFormattedCstring("Syntax Error: Expected character %c is not written. (E003)",mErrorExpectedChar);
			break;
		}
	  case 101:
		{
			outText.SetCstring("State does not exist. (E101)");
			break;
		}
	  case 102:
		{
			outText.SetCstring("Match string should be written. (E102)");
			break;
		}
	  case 103:
		{
			outText.SetCstring("State sequence error. (E103)");
			break;
		}
	  case 104:
		{
			outText.SetCstring("Color should be written in 6 characters. (E104)");
			break;
		}
	  case 105:
		{
			outText.SetCstring("State error. (E105)");
			break;
		}
	  case 106:
		{
			outText.SetCstring("\"(double quotation) should be written. (E106)");
			break;
		}
	  case 107:
		{
			outText.SetCstring("Category does not exist. (E107)");
			break;
		}
	  case 108:
		{
			outText.SetCstring("Keyword array error. (E108)");
			break;
		}
	  case 109:
		{
			outText.SetCstring("Suffix array error. (E109)");
			break;
		}
	  case 110:
		{
			outText.SetCstring("Text array error. (E110)");
			break;
		}
	  case 111:
		{
			outText.SetCstring("State duplicated.. (E111)");
			break;
		}
	  case 201:
		{
			outText.SetCstring("Argument Error: Variable should be written. (E201)");
			break;
		}
	  case 202:
		{
			outText.SetCstring("Argument Error: State should be written. (E202)");
			break;
		}
	  case 203:
		{
			outText.SetCstring("Argument Error: Category should be written. (E203)");
			break;
		}
	  case 299:
		{
			outText.SetCstring("Argument Error. (E299)");
			break;
		}
	}
}

//#402
/**
SDFテキストからSDFNameだけを抽出する。
AApp::MakeBuiltinSDFList()用
*/
void	ASyntaxDefinition::ParseSDFNameOnly( const AText& inText, AText& outSDFName )
{
	outSDFName.DeleteAll();
	try
	{
		AIndex	pos = 0;
		AText	t;
		t.SetCstring("GlobalAttribute_SDFName");
		if( inText.FindText(pos,t,pos) == true )
		{
			pos += t.GetItemCount();
			if( inText.FindCstring(pos,"(",pos) == true )
			{
				pos++;
				if( inText.FindCstring(pos,"\"",pos) == true )
				{
					pos++;
					AIndex	spos = pos;
					if( inText.FindCstring(pos,"\"",pos) == true )
					{
						inText.GetText(spos,pos-spos,outSDFName);
						return;
					}
				}
			}
		}
	}
	catch(...)
	{
	}
	_ACError("no sdf name",NULL);
}

#pragma mark ===========================

#pragma mark ---初期状態取得

AIndex	ASyntaxDefinition::GetInitialState( const AText& inFileURL, const AText& inSyntaxPartName ) const//#998
{
	//文法パート名に対応する開始stateを取得
	AIndex	syntaxPartIndex = GetSyntaxPartIndexFromName(inSyntaxPartName);
	if( syntaxPartIndex != kIndex_Invalid )
	{
		return mSyntaxPartArray_InitialStateIndex.Get(syntaxPartIndex);
	}
	//文法パート名に対応する文法パートが無いときは、ファイル拡張子に従って開始state取得
	else
	{
		//
		AText	suffix;
		inFileURL.GetSuffix(suffix);//#998
		AIndex	index = mInitialState_Suffix.Find(suffix);
		if( index != kIndex_Invalid )
		{
			return mInitialState_Index.Get(index);
		}
		return 0;
	}
}

#pragma mark ===========================

#pragma mark ---簡易文法定義

//
void	ASyntaxDefinition::MakeSimpleDefinition( const AText& inCommentStart, const AText& inCommentEnd, const AText& inLineCommentStart,
												 const AText& inLiteral1Start, const AText& inLiteral1End, const AText& inLiteral2Start, const AText& inLiteral2End,
												 const AText& inEscapeText, const ABool inCommentStartIsRegExp,
												 const AText& inIncIndentRegExp, const AText& inDecIndentRegExp )//#972
{
	//#349
	DeleteAll();
	//#940
	//エスケープ文字に半角￥があったらバックスラッシュも追加する。
	AText	escapeText;
	escapeText.SetText(inEscapeText);
	AText	yen;
	yen.Add(0xC2);
	yen.Add(0xA5);
	AIndex	p = 0;
	if( escapeText.FindText(0,yen,p) == true )
	{
		escapeText.Add(0x5C);
	}
	//
	AText	statename;
	//idle状態追加
	AIndex	idleStateIndex = mStateArray.GetItemCount();
	statename.SetCstring("idle");
	{
		mStateNameArray.Add(statename);
		ASyntaxDefinitionStateFactory	factory(this,*this);
		mStateArray.AddNewObject(factory);
		{
			//escape
			AText	escapeChar;//B0427
			for( AIndex i = 0; i < escapeText.GetItemCount(); )//B0427
			{
				escapeText.Get1UTF8Char(i,escapeChar);//B0427
				AIndex	seqIndex = mStateArray.GetObject(idleStateIndex).CreateActionSequence(escapeChar);//B0427
				mStateArray.GetObject(idleStateIndex).GetActionSequence(seqIndex).
						AddAction(kSyntaxDefinitionActionType_EscapeNextChar);
			}
		}
	}
	//comment状態追加
	if( inCommentStart.GetItemCount() > 0 && inCommentEnd.GetItemCount() > 0 )
	{
		AIndex	stateIndex = mStateArray.GetItemCount();
		statename.SetCstring("comment");
		{
			mStateNameArray.Add(statename);
			ASyntaxDefinitionStateFactory	factory(this,*this);
			mStateArray.AddNewObject(factory);
		}
		//#844 mStateArray.GetObject(stateIndex).SetStateColor(inCommentColor);
		mStateArray.GetObject(stateIndex).SetNotCodeState();
		mStateArray.GetObject(stateIndex).SetCommentState();
		mStateArray.GetObject(stateIndex).SetColorSlot(32);//#844 カラースロット（コメント）設定
		{
			//pushstate(comment)
			AIndex	seqIndex;
			if( inCommentStartIsRegExp == false )
			{
				seqIndex = mStateArray.GetObject(idleStateIndex).CreateActionSequence(inCommentStart);
			}
			else
			{
				seqIndex = mStateArray.GetObject(idleStateIndex).CreateActionSequence(GetEmptyText());
				RegisterRegExp(inCommentStart,idleStateIndex,seqIndex);
			}
			mStateArray.GetObject(idleStateIndex).GetActionSequence(seqIndex).
			AddAction_ChangeState(kSyntaxDefinitionActionType_PushState,stateIndex);
		}
		{
			//popstate(comment)
			AIndex	seqIndex = mStateArray.GetObject(stateIndex).CreateActionSequence(inCommentEnd);
			mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
			AddAction(kSyntaxDefinitionActionType_PopStateFromNextChar);
		}
	}
	//linecomment状態追加
	if( inLineCommentStart.GetItemCount() > 0 )
	{
		AIndex	stateIndex = mStateArray.GetItemCount();
		statename.SetCstring("linecomment");
		{
			mStateNameArray.Add(statename);
			ASyntaxDefinitionStateFactory	factory(this,*this);
			mStateArray.AddNewObject(factory);
		}
		//#844 mStateArray.GetObject(stateIndex).SetStateColor(inCommentColor);
		mStateArray.GetObject(stateIndex).SetNotCodeState();
		mStateArray.GetObject(stateIndex).SetCommentState();
		mStateArray.GetObject(stateIndex).SetColorSlot(32);//#844 カラースロット（コメント）設定
		{
			//pushstate(comment)
			AIndex	seqIndex = mStateArray.GetObject(idleStateIndex).CreateActionSequence(inLineCommentStart);
			mStateArray.GetObject(idleStateIndex).GetActionSequence(seqIndex).
			AddAction_ChangeState(kSyntaxDefinitionActionType_PushState,stateIndex);
		}
		{
			AText	lineend;
			lineend.Add(kUChar_LineEnd);
			//popstate(comment)
			AIndex	seqIndex = mStateArray.GetObject(stateIndex).CreateActionSequence(lineend);
			mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
			AddAction(kSyntaxDefinitionActionType_PopStateFromNextChar);
		}
	}
	//literal1状態追加
	if( inLiteral1Start.GetItemCount() > 0 && inLiteral1End.GetItemCount() > 0 )
	{
		AIndex	stateIndex = mStateArray.GetItemCount();
		statename.SetCstring("literal1");
		{
			mStateNameArray.Add(statename);
			ASyntaxDefinitionStateFactory	factory(this,*this);
			mStateArray.AddNewObject(factory);
		}
		//#844 mStateArray.GetObject(stateIndex).SetStateColor(inLiteralColor);
		mStateArray.GetObject(stateIndex).SetNotCodeState();
		mStateArray.GetObject(stateIndex).SetColorSlot(33);//#844 カラースロット（文字列）設定
		{
			//pushstate(comment)
			AIndex	seqIndex = mStateArray.GetObject(idleStateIndex).CreateActionSequence(inLiteral1Start);
			mStateArray.GetObject(idleStateIndex).GetActionSequence(seqIndex).
			AddAction_ChangeState(kSyntaxDefinitionActionType_PushState,stateIndex);
		}
		{
			//popstate(comment)
			AIndex	seqIndex = mStateArray.GetObject(stateIndex).CreateActionSequence(inLiteral1End);
			mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
			AddAction(kSyntaxDefinitionActionType_PopStateFromNextChar);
		}
		{
			//escape
			AText	escapeChar;//B0427
			for( AIndex i = 0; i < escapeText.GetItemCount(); )//B0427
			{
				escapeText.Get1UTF8Char(i,escapeChar);//B0427
				AIndex	seqIndex = mStateArray.GetObject(stateIndex).CreateActionSequence(escapeChar);//B0427
				mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
						AddAction(kSyntaxDefinitionActionType_EscapeNextChar);
			}
		}
	}
	//literal2状態追加
	if( inLiteral2Start.GetItemCount() > 0 && inLiteral2End.GetItemCount() > 0 )
	{
		AIndex	stateIndex = mStateArray.GetItemCount();
		statename.SetCstring("literal2");
		{
			mStateNameArray.Add(statename);
			ASyntaxDefinitionStateFactory	factory(this,*this);
			mStateArray.AddNewObject(factory);
		}
		//#844 mStateArray.GetObject(stateIndex).SetStateColor(inLiteralColor);
		mStateArray.GetObject(stateIndex).SetNotCodeState();
		mStateArray.GetObject(stateIndex).SetColorSlot(33);//#844 カラースロット（文字列）設定
		{
			//pushstate(comment)
			AIndex	seqIndex = mStateArray.GetObject(idleStateIndex).CreateActionSequence(inLiteral2Start);
			mStateArray.GetObject(idleStateIndex).GetActionSequence(seqIndex).
			AddAction_ChangeState(kSyntaxDefinitionActionType_PushState,stateIndex);
		}
		{
			//popstate(comment)
			AIndex	seqIndex = mStateArray.GetObject(stateIndex).CreateActionSequence(inLiteral2End);
			mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
			AddAction(kSyntaxDefinitionActionType_PopStateFromNextChar);
		}
		{
			//escape
			AText	escapeChar;//B0427
			for( AIndex i = 0; i < escapeText.GetItemCount(); )//B0427
			{
				escapeText.Get1UTF8Char(i,escapeChar);//B0427
				AIndex	seqIndex = mStateArray.GetObject(stateIndex).CreateActionSequence(escapeChar);//B0427
				mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
						AddAction(kSyntaxDefinitionActionType_EscapeNextChar);
			}
		}
	}
	//other1状態追加
	/*if( inOther1Start.GetItemCount() > 0 && inOther1End.GetItemCount() > 0 )
	{
		AIndex	stateIndex = mStateArray.GetItemCount();
		statename.SetCstring("other1");
		{
			mStateNameArray.Add(statename);
			ASyntaxDefinitionStateFactory	factory(this,*this);
			mStateArray.AddNewObject(factory);
		}
		mStateArray.GetObject(stateIndex).SetNotCodeState();
		{
			//pushstate(comment)
			AIndex	seqIndex = mStateArray.GetObject(idleStateIndex).CreateActionSequence(inOther1Start);
			mStateArray.GetObject(idleStateIndex).GetActionSequence(seqIndex).
			AddAction_ChangeState(kSyntaxDefinitionActionType_PushState,stateIndex);
		}
		{
			//popstate(comment)
			AIndex	seqIndex = mStateArray.GetObject(stateIndex).CreateActionSequence(inOther1End);
			mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
			AddAction(kSyntaxDefinitionActionType_PopStateFromNextChar);
		}
	}
	//other2状態追加
	if( inOther2Start.GetItemCount() > 0 && inOther2End.GetItemCount() > 0 )
	{
		AIndex	stateIndex = mStateArray.GetItemCount();
		statename.SetCstring("other2");
		{
			mStateNameArray.Add(statename);
			ASyntaxDefinitionStateFactory	factory(this,*this);
			mStateArray.AddNewObject(factory);
		}
		mStateArray.GetObject(stateIndex).SetNotCodeState();
		{
			//pushstate(comment)
			AIndex	seqIndex = mStateArray.GetObject(idleStateIndex).CreateActionSequence(inOther2Start);
			mStateArray.GetObject(idleStateIndex).GetActionSequence(seqIndex).
			AddAction_ChangeState(kSyntaxDefinitionActionType_PushState,stateIndex);
		}
		{
			//popstate(comment)
			AIndex	seqIndex = mStateArray.GetObject(stateIndex).CreateActionSequence(inOther2End);
			mStateArray.GetObject(stateIndex).GetActionSequence(seqIndex).
			AddAction(kSyntaxDefinitionActionType_PopStateFromNextChar);
		}
	}*/
	
	//ブロック開始正規表現 #972
	if( inIncIndentRegExp.GetItemCount() > 0 )
	{
		AIndex	seqIndex = mStateArray.GetObject(idleStateIndex).CreateActionSequence(GetEmptyText());
		RegisterRegExp(inIncIndentRegExp,idleStateIndex,seqIndex);
		mStateArray.GetObject(idleStateIndex).GetActionSequence(seqIndex).
				AddAction(kSyntaxDefinitionActionType_IncIndentFromNext);
	}
	//ブロック終了正規表現 #972
	if( inDecIndentRegExp.GetItemCount() > 0 )
	{
		AIndex	seqIndex = mStateArray.GetObject(idleStateIndex).CreateActionSequence(GetEmptyText());
		RegisterRegExp(inDecIndentRegExp,idleStateIndex,seqIndex);
		mStateArray.GetObject(idleStateIndex).GetActionSequence(seqIndex).
				AddAction(kSyntaxDefinitionActionType_DecIndentFromCurrent);
	}
	
	//簡易文法使用時のIdInfoArgStart:'(', ArgEnd:')', ArgDelimiter:',' #853
	mIdInfoArgStart['('] = true;
	mIdInfoArgStartCharCount++;
	mIdInfoArgEnd[')'] = true;
	mIdInfoArgDelimiter[','] = true;
	
	//
	for( AIndex i = 0; i < mStateArray.GetItemCount(); i++ )
	{
		mStateArray.GetObject(i).MakeConditionTextByteCountArray();
	}
	//#349
	mParseResult = true;
}

#pragma mark ===========================

#pragma mark ---キーワード／カテゴリー

//#844
#if 0
//AModePrefDBからコールされる
void	ASyntaxDefinition::GetKeywordData( AIdentifierList& ioIdentifierList ) const
{
	//
	for( AIndex i = 0; i < mKeywordArray.GetItemCount(); i++ )
	{
		ioIdentifierList.AddIdentifier(kUniqID_Invalid,kUniqID_Invalid,kIdentifierType_KeywordInSyntaxDefinitionFile,
					mKeywordArray_Category.Get(i),mKeywordArray.GetTextConst(i),GetEmptyText(),GetEmptyText(),0,0,false,GetEmptyText(),GetEmptyText(),GetEmptyText(),
					false,0);//#593
	}
}
#endif

/*
bool	ASyntax::Test( AText& ioText )
{
	AFileAcc	fileacc;
	AText	path("/unittest/test1.syntax");
	fileacc.Specify(path);
	fileacc.ReadTo(mSyntaxText);
	mSyntaxText.NormalizeLineEnd();
	if( Compile() == false )
	{
		
		return false;
	}
	return true;
}
*/

/*#844
//状態色リスト取得
//（他の状態色を参照する状態はリストに入らない）
void	ASyntaxDefinition::GetStateColorArray( ATextArray& outNameArray, AArray<AColor>& outColorArray, ATextArray& outDisplayNameArray ) const 
{
	outNameArray.DeleteAll();
	outColorArray.DeleteAll();
	outDisplayNameArray.DeleteAll();
	for( AIndex i = 0; i < mStateArray.GetItemCount(); i++ )
	{
		AColor	color;
		if( mStateArray.GetObjectConst(i).GetStateColor(color) == true 
					&& mStateArray.GetObjectConst(i).GetSameColorAs() == kIndex_Invalid )//#14 SameColorAsで定義されていないことを条件追加（SameColorAs定義でもGetStateColor()はtrueになる）
		{
			AText	name;
			mStateNameArray.Get(i,name);
			outNameArray.Add(name);
			outColorArray.Add(color);
			if( mStateArray.GetObjectConst(i).GetStateDisplayName().GetItemCount() > 0 )
			{
				outDisplayNameArray.Add(mStateArray.GetObjectConst(i).GetStateDisplayName());
			}
			else
			{
				outDisplayNameArray.Add(name);
			}
		}
	}
}

//#14
//状態色参照元の状態名を取得
void	ASyntaxDefinition::GetStateColorSourceStateName( const AText& inName, AText& outSourceStateName ) const
{
	outSourceStateName.SetText(inName);
	//GetSameColorAs()をたどって、参照元の状態名を取得する
	AIndex	index = mStateNameArray.Find(inName);
	while( index != kIndex_Invalid )
	{
		mStateNameArray.Get(index,outSourceStateName);
		index = mStateArray.GetObjectConst(index).GetSameColorAs();
	}
}
*/

/*#844
//R0000 カテゴリー可否
void	ASyntaxDefinition::GetCategoryDisabledStateArray( AObjectArray<ABoolArray>& ioCategoryDisabledStateArray ) const
{
	//ioCategoryDisabledStateArrayは、ABoolArrayにcategory毎にdisableかどうかの値、AObjectArrayにstate毎のABoolArrayが入る
	AItemCount	categoryCount = mCategory_Name.GetItemCount();
	for( AIndex state = 0; state < mStateArray.GetItemCount(); state++ )
	{
		ABoolArray&	ba = ioCategoryDisabledStateArray.GetObject(state);
		if( mStateArray.GetObjectConst(state).GetAllCategoryDisabled() == true )
		{
			//全てのcategoryをdisableにする
			for( AIndex category = 0; category < categoryCount; category++ )
			{
				ba.Set(category,true);
			}
		}
		for( AIndex category = 0; category < categoryCount; category++ )
		{
			if( mStateArray.GetObjectConst(state).GetEnabledCategoryArray().Find(category) != kIndex_Invalid )
			{
				ba.Set(category,false);
			}
		}
	}
}
*/

#pragma mark ===========================

#pragma mark ---正規表現

void	ASyntaxDefinition::RegisterRegExp( const AText& inRegExpText, const AIndex inStateIndex, const AIndex inSequenceIndex )
{
	mRegExp_Text.Add(inRegExpText);
	mRegExp_StateIndex.Add(inStateIndex);
	mRegExp_SequenceIndex.Add(inSequenceIndex);
}

void	ASyntaxDefinition::GetRegExpArray( AObjectArray<ARegExp>& outRegExpArray, AArray<AIndex>& outStateArray, AArray<AIndex>& outSeqArray )
{
	outRegExpArray.DeleteAll();
	outStateArray.DeleteAll();
	outSeqArray.DeleteAll();
	for( AIndex i = 0; i < mRegExp_Text.GetItemCount(); i++ )
	{
		AText	text;
		mRegExp_Text.Get(i,text);
		outRegExpArray.GetObject(outRegExpArray.AddNewObject()).SetRE(text);
		outStateArray.Add(mRegExp_StateIndex.Get(i));
		outSeqArray.Add(mRegExp_SequenceIndex.Get(i));
	}
}

#pragma mark ===========================
//R0243

#pragma mark ---クラス文字列判定

ABool	ASyntaxDefinition::IsAfterClassString( const AText& inText, const AIndex inTextPos, AItemCount& outClassStringLength ) const
{
	outClassStringLength = 0;
	for( AIndex index = 0; index < mClassStringArray.GetItemCount(); index++ )
	{
		AItemCount	len = mClassStringArray.GetTextConst(index).GetItemCount();
		AIndex	pos = inTextPos - len;
		if( pos >= 0 )
		{
			if( mClassStringArray.GetTextConst(index).Compare(inText,pos,len) == true )
			{
				outClassStringLength = len;
				return true;
			}
		}
	}
	return false;
}

ABool	ASyntaxDefinition::IsAfterInstanceString( const AText& inText, const AIndex inTextPos, AItemCount& outInstanceStringLength ) const
{
	outInstanceStringLength = 0;
	for( AIndex index = 0; index < mInstanceStringArray.GetItemCount(); index++ )
	{
		AItemCount	len = mInstanceStringArray.GetTextConst(index).GetItemCount();
		AIndex	pos = inTextPos - len;
		if( pos >= 0 )
		{
			if( mInstanceStringArray.GetTextConst(index).Compare(inText,pos,len) == true )
			{
				outInstanceStringLength = len;
				return true;
			}
		}
	}
	return false;
}

//#723
/**
（最初の）class stringテキストを取得
*/
void	ASyntaxDefinition::GetFirstClassString( AText& outText ) const
{
	if( mClassStringArray.GetItemCount() == 0 )
	{
		outText.DeleteAll();
	}
	else
	{
		mClassStringArray.Get(0,outText);
	}
}

#pragma mark ===========================

#pragma mark ---Directive文字列判定
//#467

/**
*/
ABool	ASyntaxDefinition::IsDirectiveString( const AText& inText ) const
{
	return (mDirectiveStringArray.Find(inText)!=kIndex_Invalid);
}

/**
*/
ABool	ASyntaxDefinition::IsDirectiveIfString( const AText& inText ) const
{
	return (mDirectiveIfStringArray.Find(inText)!=kIndex_Invalid);
}

/**
*/
ABool	ASyntaxDefinition::IsDirectiveIfdefString( const AText& inText ) const
{
	return (mDirectiveIfdefStringArray.Find(inText)!=kIndex_Invalid);
}

/**
*/
ABool	ASyntaxDefinition::IsDirectiveIfndefString( const AText& inText ) const
{
	return (mDirectiveIfndefStringArray.Find(inText)!=kIndex_Invalid);
}

/**
*/
ABool	ASyntaxDefinition::IsDirectiveElseString( const AText& inText ) const
{
	return (mDirectiveElseStringArray.Find(inText)!=kIndex_Invalid);
}

/**
*/
ABool	ASyntaxDefinition::IsDirectiveElifString( const AText& inText ) const
{
	return (mDirectiveElifStringArray.Find(inText)!=kIndex_Invalid);
}

/**
*/
ABool	ASyntaxDefinition::IsDirectiveEndifString( const AText& inText ) const
{
	return (mDirectiveEndifStringArray.Find(inText)!=kIndex_Invalid);
}

