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

#pragma once

#include "../../AbsFramework/Frame.h"

class AIdentifierList;

const ANumber	kSyntaxTokenStackIndex_First = -99999;
const ANumber	kSyntaxTokenStackIndex_Next = -99999;

enum ASyntaxDefinitionReservedWord
{
	kReservedWord_Invalid = -1,
	kReservedWord_state = 0,
	kReservedWord_sdfname,//R0000
	kReservedWord_sdfversion,//#349
	kReservedWord_initialstate,//R0000
	kReservedWord_lineskip,
	kReservedWord_lineend,
	kReservedWord_default,
	kReservedWord_tab,
	kReservedWord_defaultcolor,
	kReservedWord_defaultimportcolor,
	kReservedWord_defaultlocalcolor,
	kReservedWord_defaultkeyword,
	kReservedWord_SameColorAs,//#14
	kReservedWord_DisableDirective,//#467
	kReservedWord_AfterParentKeyword,//#717
	kReservedWord_nomemory,
	kReservedWord_notcode,
	kReservedWord_first,
	kReservedWord_category,
	kReservedWord_category_defaultcolor,//#349
	kReservedWord_category_defaultlocalcolor,//#349
	kReservedWord_category_defaultimportcolor,//#349
	kReservedWord_category_defaultkeyword,//#349
	kReservedWord_category_menubold,//#349
	kReservedWord_regexp,
	kReservedWord_EscapeNextChar,
	kReservedWord_SetVarWithTokenStack,
	kReservedWord_SetVarStartFromNextChar,
	kReservedWord_SetVarStartAtPrevToken,//RC2
	kReservedWord_SetVarStart,
	kReservedWord_SetVarEnd,
	kReservedWord_SetVarEndAtPrevToken,
	kReservedWord_SetVarEndAtNextChar,
	kReservedWord_SetVarLastWord,//B0306
	kReservedWord_CatVar,
	kReservedWord_RegisterParentChild,//#717
	kReservedWord_BlockStart,
	kReservedWord_ChangeState,
	kReservedWord_ChangeStateIfBlockEnd,
	kReservedWord_ChangeStateIfNotAfterAlphabet,//R0241
	kReservedWord_PushStateIfNotAfterAlphabet,//R0241
	kReservedWord_ChangeStateIfTokenEqualVar,//R0241
	kReservedWord_SetVarCurrentToken,//R0241
	kReservedWord_SetVarNextToken,
	kReservedWord_SetVarRegExpGroup1,
	kReservedWord_ChangeStateFromNextChar,//R0241
	kReservedWord_PushState,
	kReservedWord_PushStateFromNextLine,
	kReservedWord_PopState,
	kReservedWord_PopStateFromNextChar,
	kReservedWord_IncIndentFromNext,
	kReservedWord_DecIndentFromCurrent,
	kReservedWord_DecIndentFromNext,
	kReservedWord_DecIndentOnlyCurrent,
	kReservedWord_SetZeroIndentOnlyCurrent,
	kReservedWord_IndentLabel,
	kReservedWord_ClearTokenStack,
	kReservedWord_AddGlobalId,
	kReservedWord_AddLocalId,
	kReservedWord_Import,
	kReservedWord_comment,
	kReservedWord_displayname,
	kReservedWord_indentontheway,
	kReservedWord_stable,
	kReservedWord_menubold,
	kReservedWord_NoSpellCheck,//R0199
	kReservedWord_disableallcategory,//R0000 カテゴリー可否
	kReservedWord_globalstate,//B0000
	kReservedWord_enablecategory,
	kReservedWord_ImportLinkToProjectAll,//win 080722
	kReservedWord_StartAlphabet,//win 080722
	kReservedWord_ImportRecursive,//win 080725
	kReservedWord_ImportRecursiveAll,//win 080725
	kReservedWord_ContinuousAlphabet,
	kReservedWord_IdInfoAutoDisplay,//RC2
	kReservedWord_IdInfoArgStart,//RC2
	kReservedWord_IdInfoArgEnd,//RC2
	kReservedWord_IdInfoArgDelimiter,//RC2
	kReservedWord_ClassString,//R0243
	kReservedWord_InstanceString,//R0243
	kReservedWord_Directive,//#467
	kReservedWord_DirectiveIf,//#467
	kReservedWord_DirectiveIfdef,//#467
	kReservedWord_DirectiveIfndef,//#467
	kReservedWord_DirectiveElse,//#467
	kReservedWord_DirectiveElif,//#467
	kReservedWord_DirectiveEndif,//#467
	kReservedWord_start,//RC2
	kReservedWord_heredocumentend,
	kReservedWord_SetVarStartOrContinue,//RC2
	kReservedWord_SetVar,//RC2
	//kReservedWord_SetVarStaticText,//
	kReservedWord_SetVarTypeOfVar,//
	kReservedWord_ClearVar,
	kReservedWord_StateAttribute_ColorSlot,//#844
	kReservedWord_CategoryAttribute_ColorSlot,//#844
	kReservedWord_StateAttribute_SyntaxPart,//#844
	kReservedWord_GlobalAttribute_SyntaxPart,//#844
	kReservedWord_StateAttribute_IgnoreCase,
	kReservedWord_StateAttribute_EnableRecompletion,
	kReservedWord_StateAttribute_DisableIndent,
	kReservedWord_SyntaxWarning,
	kReservedWord_SyntaxError,
	kReservedWord_CategoryAttribute_DisplayName,
	kReservedWord_GlobalAttribute_DisableAutoIndent,//#997
};

enum ASyntaxDefinitionActionType
{
	kSyntaxDefinitionActionType_SetVarWithTokenStack = 0,
	kSyntaxDefinitionActionType_SetVarStartFromNextChar,
	kSyntaxDefinitionActionType_SetVarStart,
	kSyntaxDefinitionActionType_SetVarStartOrContinue,//RC2
	kSyntaxDefinitionActionType_SetVarStartAtPrevToken,//RC2
	kSyntaxDefinitionActionType_SetVar,//RC2
	kSyntaxDefinitionActionType_SetVarStaticText,//
	kSyntaxDefinitionActionType_SetVarTypeOfVar,//
	kSyntaxDefinitionActionType_ClearVar,//RC2
	kSyntaxDefinitionActionType_SetVarEnd,
	kSyntaxDefinitionActionType_SetVarEndAtPrevToken,
	kSyntaxDefinitionActionType_SetVarEndAtNextChar,
	kSyntaxDefinitionActionType_SetVarLastWord,//B0306
	kSyntaxDefinitionActionType_CatVar,
	kSyntaxDefinitionActionType_RegisterParentChild,//#717
	kSyntaxDefinitionActionType_BlockStart,
	kSyntaxDefinitionActionType_IncIndentFromNext,
	kSyntaxDefinitionActionType_DecIndentFromCurrent,
	kSyntaxDefinitionActionType_DecIndentFromNext,
	kSyntaxDefinitionActionType_DecIndentOnlyCurrent,
	kSyntaxDefinitionActionType_SetZeroIndentOnlyCurrent,
	kSyntaxDefinitionActionType_IndentLabel,
	kSyntaxDefinitionActionType_ClearTokenStack,
	kSyntaxDefinitionActionType_ChangeState,
	kSyntaxDefinitionActionType_ChangeStateIfBlockEnd,
	kSyntaxDefinitionActionType_ChangeStateIfNotAfterAlphabet,//R0241
	kSyntaxDefinitionActionType_PushStateIfNotAfterAlphabet,//R0241
	kSyntaxDefinitionActionType_ChangeStateIfTokenEqualVar,//R0241
	kSyntaxDefinitionActionType_SetVarCurrentToken,//R0241
	kSyntaxDefinitionActionType_SetVarNextToken,
	kSyntaxDefinitionActionType_SetVarRegExpGroup1,
	kSyntaxDefinitionActionType_ChangeStateFromNextChar,//R0241
	kSyntaxDefinitionActionType_PushState,
	kSyntaxDefinitionActionType_PushStateFromNextLine,
	kSyntaxDefinitionActionType_PopState,
	kSyntaxDefinitionActionType_PopStateFromNextChar,
	kSyntaxDefinitionActionType_AddGlobalId,
	kSyntaxDefinitionActionType_AddLocalId,
	kSyntaxDefinitionActionType_EscapeNextChar,
	kSyntaxDefinitionActionType_Import,
	kSyntaxDefinitionActionType_SyntaxWarning,
	kSyntaxDefinitionActionType_SyntaxError,
};

class ASyntaxDefinition;
class ASyntaxDefinitionState;
class ASyntaxDefinitionActionSequence;

//#693
/**
ASyntaxDefinitionActionSequenceの各actionデータのうちIdentifier用のもの
（メモリ削減のため、ASyntaxDefinitionActionSequence内のメンバーとせずに、必要な場合だけmIdDataArrayのメンバーとして確保するようにした）
*/
class ASyntaxDefinitionActionSequenceIdData : public AObjectArrayItem
{
  public:
	ASyntaxDefinitionActionSequenceIdData( AObjectArrayItem* inParent ):AObjectArrayItem(inParent),
	mIdentifierCategoryIndex(kIndex_Invalid),
	mIdentifierKeywordVariableIndex(kIndex_Invalid),
	mIdentifierMenuTextVariableIndex(kIndex_Invalid),
	mIdentifierInfoTextVariableIndex(kIndex_Invalid),
	mIdentifierCommentTextVariableIndex(kIndex_Invalid),
	mIdentifierParentKeywordIndex(kIndex_Invalid),
	mIdentifierTypeIndex(kIndex_Invalid),
	mIdentifierOutlineLevel(kIndex_Invalid)
	{}
	~ASyntaxDefinitionActionSequenceIdData()
	{}
	
  public:
	AIndex							mIdentifierCategoryIndex;
	AIndex							mIdentifierKeywordVariableIndex;
	AText							mIdentifierKeywordStaticText;//#348
	AIndex							mIdentifierMenuTextVariableIndex;
	AText							mIdentifierMenuTextStaticText;//#348
	AIndex							mIdentifierInfoTextVariableIndex;
	AText							mIdentifierInfoTextStaticText;//#348
	AIndex							mIdentifierCommentTextVariableIndex;//RC2
	AText							mIdentifierCommentTextStaticText;//RC2 //#348
	AIndex							mIdentifierParentKeywordIndex;//RC2
	AText							mIdentifierParentKeywordStaticText;//RC2 //#348
	AIndex							mIdentifierTypeIndex;//RC2
	AText							mIdentifierTypeStaticText;//RC2 //#348
	AHashTextArray					mIdentifierKeywordExceptionText;//#348
	AIndex							mIdentifierOutlineLevel;//#875
};

#pragma mark ===========================
#pragma mark [クラス]ASyntaxDefinitionActionSequence

//アクションシーケンス
class ASyntaxDefinitionActionSequence : public AObjectArrayItem
{
	//コンストラクタ、デストラクタ
  public:
	ASyntaxDefinitionActionSequence(  AObjectArrayItem* inParent, ASyntaxDefinitionState& inSyntaxDefinitionState );
	~ASyntaxDefinitionActionSequence();
  private:
	ASyntaxDefinitionState&					mSyntaxDefinitionState;
	
	//関連オブジェクト取得
  private:
	ASyntaxDefinitionState&		GetSyntaxDefinitionState() {return mSyntaxDefinitionState;}
	const ASyntaxDefinitionState&		GetSyntaxDefinitionStateConst() const {return mSyntaxDefinitionState;}
	ASyntaxDefinition&			GetSyntaxDefinition();
	
	//アクション追加
  public:
	AIndex						AddAction( const ASyntaxDefinitionActionType inActionType );
	AIndex						AddAction_var( const ASyntaxDefinitionActionType inActionType, const AIndex inVariableIndex );
	AIndex						AddAction_CatVar( const ASyntaxDefinitionActionType inActionType, 
								const AIndex inVariableIndex, const AIndex inVariableIndex1, const AIndex inVariableIndex2 );
	AIndex						AddAction_SetVar( const ASyntaxDefinitionActionType inActionType, 
								const AIndex inVariableIndex, const AIndex inVariableIndex1 );//RC2
	AIndex						AddAction_SetVarWithTokenStack( const AIndex inVariableIndex, const ANumber inStartNumber, const ANumber inEndNumber );
	AIndex						AddAction_ChangeState( const ASyntaxDefinitionActionType inActionType, const AIndex inStateIndex );
	AIndex						AddAction_ChangeStateIfVar( const ASyntaxDefinitionActionType inActionType, 
								const AIndex inVariableIndex, const AIndex inStateIndex );
	AIndex						AddAction_SetVarStaticText( const ASyntaxDefinitionActionType inActionType,
															const AIndex inVarIndex, const AText& inStaticText );
	AIndex						AddAction_AddGlobalId( const AIndex inCategoryIndex,
								const AIndex inKeywordVariableIndex, const AText& inKeywordStaticText, 
								const AIndex inMenuTextVariableIndex, const AText& inMenuTextStaticText, 
								const AIndex inInfoTextVariableIndex, const AText& inInfoTextStaticText,
								const AIndex inCommentTextVariableIndex, const AText& inCommentTextStaticText,//RC2
								const AIndex inParentKeywordVariableIndex, const AText& inParentKeywordStaticText,//RC2
								const AIndex inTypeVariableIndex, const AText& inTypeStaticText,//RC2
								const AText& inKeywordExceptionText, const AIndex inOutlineLevel );//#875
	AIndex						AddAction_AddLocalId( const AIndex inCategoryIndex,
								const AIndex inKeywordVariableIndex, const AText& inKeywordStaticText, 
								const AIndex inInfoTextVariableIndex, const AText& inInfoTextStaticText,
								const AIndex inTypeVariableIndex, const AText& inTypeStaticText,//R0243
								const AText& inKeywordExceptionText );
	
	//アクションデータ取得
  public:
	AItemCount					GetActionItemCount() const { return mTypeArray.GetItemCount(); }
	ASyntaxDefinitionActionType	GetActionType( const AIndex inIndex ) const { return mTypeArray.Get(inIndex); }
	AIndex						GetVariableIndex( const AIndex inIndex ) const { return mVariableIndexArray.Get(inIndex); }
	//#693 AIndex						GetVariable1Index( const AIndex inIndex ) const { return mVariableIndex1Array.Get(inIndex); }
	//#693 AIndex						GetVariable2Index( const AIndex inIndex ) const { return mVariableIndex2Array.Get(inIndex); }
	AIndex						GetCategoryIndex( const AIndex inIndex ) const 
	{
		//#693 return mIdentifierCategoryIndexArray.Get(inIndex); 
		AIndex	index = mIdDataIndexArray.Get(inIndex);
		return mIdDataArray.GetObjectConst(index).mIdentifierCategoryIndex;
	}
	//#693 ANumber						GetStartNumber( const AIndex inIndex ) const { return mStartNumberArray.Get(inIndex); }
	//#693 ANumber						GetEndNumber( const AIndex inIndex ) const { return mEndNumberArray.Get(inIndex); }
	//#693
	ANumber						GetParameter1( const AIndex inIndex ) const 
	{
		return mParameter1Array.Get(inIndex);
	}
	//#693
	ANumber						GetParameter2( const AIndex inIndex ) const
	{
		return mParameter2Array.Get(inIndex);
	}
	//
	AIndex						GetStateIndex( const AIndex inIndex ) const { return mStateIndexArray.Get(inIndex); }
	void						GetKeyword( const AIndex inIndex, AIndex& outKeywordVaiableIndex, AText& outKeywordStaticText ) const
	{
		/*#693
		outKeywordVaiableIndex = mIdentifierKeywordVariableIndexArray.Get(inIndex);
		mIdentifierKeywordStaticTextArray.Get(inIndex,outKeywordStaticText);
		*/
		AIndex	index = mIdDataIndexArray.Get(inIndex);
		outKeywordVaiableIndex = mIdDataArray.GetObjectConst(index).mIdentifierKeywordVariableIndex;
		outKeywordStaticText = mIdDataArray.GetObjectConst(index).mIdentifierKeywordStaticText;
	}
	void						GetMenuText( const AIndex inIndex, AIndex& outMenuTextVaiableIndex, AText& outMenuTextStaticText ) const
	{
		/*#693
		outMenuTextVaiableIndex = mIdentifierMenuTextVariableIndexArray.Get(inIndex);
		mIdentifierMenuTextStaticTextArray.Get(inIndex,outMenuTextStaticText);
		*/
		AIndex	index = mIdDataIndexArray.Get(inIndex);
		outMenuTextVaiableIndex = mIdDataArray.GetObjectConst(index).mIdentifierMenuTextVariableIndex;
		outMenuTextStaticText = mIdDataArray.GetObjectConst(index).mIdentifierMenuTextStaticText;
	}
	void						GetInfoText( const AIndex inIndex, AIndex& outInfoTextVaiableIndex, AText& outInfoTextStaticText ) const
	{
		/*#693
		outInfoTextVaiableIndex = mIdentifierInfoTextVariableIndexArray.Get(inIndex);
		mIdentifierInfoTextStaticTextArray.Get(inIndex,outInfoTextStaticText);
		*/
		AIndex	index = mIdDataIndexArray.Get(inIndex);
		outInfoTextVaiableIndex = mIdDataArray.GetObjectConst(index).mIdentifierInfoTextVariableIndex;
		outInfoTextStaticText = mIdDataArray.GetObjectConst(index).mIdentifierInfoTextStaticText;
	}
	void						GetCommentText( const AIndex inIndex, AIndex& outVaiableIndex, AText& outStaticText ) const//RC2
	{
		/*#693
		outVaiableIndex = mIdentifierCommentTextVariableIndexArray.Get(inIndex);
		mIdentifierCommentTextStaticTextArray.Get(inIndex,outStaticText);
		*/
		AIndex	index = mIdDataIndexArray.Get(inIndex);
		outVaiableIndex = mIdDataArray.GetObjectConst(index).mIdentifierCommentTextVariableIndex;
		outStaticText = mIdDataArray.GetObjectConst(index).mIdentifierCommentTextStaticText;
	}
	void						GetParentKeywordText( const AIndex inIndex, AIndex& outVaiableIndex, AText& outStaticText ) const//RC2
	{
		/*#693
		outVaiableIndex = mIdentifierParentKeywordIndexArray.Get(inIndex);
		mIdentifierParentKeywordStaticTextArray.Get(inIndex,outStaticText);
		*/
		AIndex	index = mIdDataIndexArray.Get(inIndex);
		outVaiableIndex = mIdDataArray.GetObjectConst(index).mIdentifierParentKeywordIndex;
		outStaticText = mIdDataArray.GetObjectConst(index).mIdentifierParentKeywordStaticText;
	}
	void						GetTypeText( const AIndex inIndex, AIndex& outVaiableIndex, AText& outStaticText ) const//RC2
	{
		/*#693
		outVaiableIndex = mIdentifierTypeIndexArray.Get(inIndex);
		mIdentifierTypeStaticTextArray.Get(inIndex,outStaticText);
		*/
		AIndex	index = mIdDataIndexArray.Get(inIndex);
		outVaiableIndex = mIdDataArray.GetObjectConst(index).mIdentifierTypeIndex;
		outStaticText = mIdDataArray.GetObjectConst(index).mIdentifierTypeStaticText;
	}
	ABool						CheckKeywordException( const AIndex inIndex, const AText& inText ) const
	{
		/*#693
		return (mIdentifierKeywordExceptionTextArray.GetObjectConst(inIndex).Find(inText) == kIndex_Invalid );
		*/
		AIndex	index = mIdDataIndexArray.Get(inIndex);
		return (mIdDataArray.GetObjectConst(index).mIdentifierKeywordExceptionText.Find(inText)==kIndex_Invalid);
	}
	//#875
	AIndex						GetOutlineLevel( const AIndex inIndex ) const
	{
		AIndex	index = mIdDataIndexArray.Get(inIndex);
		return (mIdDataArray.GetObjectConst(index).mIdentifierOutlineLevel);
	}
	
	//
  private:
	AArray<ASyntaxDefinitionActionType>		mTypeArray;
	AArray<short int>						mVariableIndexArray;//#693 メモリ削減 AIndex→ short int
	/*#693 mVariableIndex1Array, mStartNumberArrayを共用してmParameter1にし、
	mVariableIndex2Array, mEndNumberArrayを共用してmParameter2にする。
	AArray<AIndex>							mVariableIndex1Array;
	AArray<AIndex>							mVariableIndex2Array;
	AArray<ANumber>							mStartNumberArray;
	AArray<ANumber>							mEndNumberArray;
	*/
	AArray<ANumber>							mParameter1Array;//#693
	AArray<ANumber>							mParameter2Array;//#693
	AArray<short int>						mStateIndexArray;//#693 メモリ削減 AIndex→ short int
	AArray<short int>						mIdDataIndexArray;//#693
	AObjectArray<ASyntaxDefinitionActionSequenceIdData>	mIdDataArray;//#693
	/*#693
	AArray<AIndex>							mIdentifierCategoryIndexArray;
	AArray<AIndex>							mIdentifierKeywordVariableIndexArray;
	ATextArray							mIdentifierKeywordStaticTextArray;//#348
	AArray<AIndex>							mIdentifierMenuTextVariableIndexArray;
	ATextArray							mIdentifierMenuTextStaticTextArray;//#348
	AArray<AIndex>							mIdentifierInfoTextVariableIndexArray;
	ATextArray							mIdentifierInfoTextStaticTextArray;//#348
	AArray<AIndex>							mIdentifierCommentTextVariableIndexArray;//RC2
	ATextArray							mIdentifierCommentTextStaticTextArray;//RC2 //#348
	AArray<AIndex>							mIdentifierParentKeywordIndexArray;//RC2
	ATextArray							mIdentifierParentKeywordStaticTextArray;//RC2 //#348
	AArray<AIndex>							mIdentifierTypeIndexArray;//RC2
	ATextArray							mIdentifierTypeStaticTextArray;//RC2 //#348
	AObjectArray< AHashTextArray >		mIdentifierKeywordExceptionTextArray;//#348
	*/
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "ASyntaxDefinitionActionSequence"; }
	
};

//state毎に作成されるクラス
#pragma mark ===========================
#pragma mark [クラス]ASyntaxDefinitionState
//state毎に作成されるクラス

class ASyntaxDefinitionState : public AObjectArrayItem
{
	//コンストラクタ、デストラクタ
  public:
	ASyntaxDefinitionState(  AObjectArrayItem* inParent, ASyntaxDefinition& inSyntaxDefinition );
	~ASyntaxDefinitionState();
  private:
	ASyntaxDefinition&					mSyntaxDefinition;
	
	//関連オブジェクト取得
  public:
	ASyntaxDefinition&			GetSyntaxDefinition() {return mSyntaxDefinition;}
	
	//LineSkip（未使用）
  public:
	void						AddLineSkipChar( const AUChar inChar ) { mCharForSkipLine[inChar] = true; }
	ABool						IsLineSkipChar( const AUChar inChar ) const { return mCharForSkipLine[inChar]; }
  private:
	ABool									mCharForSkipLine[256];
	
	//アクションシーケンス管理
  public:
	ASyntaxDefinitionActionSequence&	GetActionSequence( const AIndex inSequenceIndex );
	AIndex						CreateActionSequence( const AText& inConditionTokenText );
	AIndex						CreateDefaultActionSequence();
	AIndex						CreateStartActionSequence();//RC2
	AIndex						CreateHereDocumentEndActionSequence();
	AIndex						FindActionSequenceIndex( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const;
	AIndex						GetDefaultActionSequenceIndex() const;
	AIndex						GetStartActionSequenceIndex() const;//RC2
	AIndex						GetHereDocumentEndSequenceIndex() const;
  private:
	AObjectArray<ASyntaxDefinitionActionSequence>	mActionSequenceArray;
	AHashTextArray							mActionSequenceConditionTextArray;//#348
	AHashTextArray							mActionSequenceConditionTextArray_UpperCase;//
	AObjectArray< AArray<AItemCount> >		mConditionTextByteCountArray;
	AArray<AIndex>							mConditionTextByteCountArrayIndexTable;//#693
	ABool									mExistDefaultConditionText;
	AIndex									mDefaultSequenceIndex;
	AIndex									mStartSequenceIndex;//RC2
	AIndex									mHereDocumentEndIndex;
	
	/*#844
	//Color
  public:
	void						SetStateColor( const AColor& inColor ) {mStateColor = inColor; mColorIsSet = true;}
	ABool						GetStateColor( AColor& outColor ) const;//#14 {outColor = mStateColor;return mColorIsSet;}
  private:
	AColor									mStateColor;
	ABool									mColorIsSet;
	*/
	/*#844
	//#14
	//他の状態と同じ色にする
  public:
	void						SetSameColorAs( const AIndex inStateIndex ) { mSameColorAs = inStateIndex; }
	AIndex						GetSameColorAs() const { return mSameColorAs; }
  private:
	AIndex									mSameColorAs;
	*/
	//
  public:
	void						SetStateDisplayName( const AText& inText ) { mDisplayName.SetText(inText); }
	const AText&				GetStateDisplayName() const { return mDisplayName; }
  private:
	AText									mDisplayName;
	
	/*#844
	//
  public:
	ABool						IsNoMemoryState() const {return mNoMemoryState;}
	void						SetNoMemoryState() { mNoMemoryState = true; }
  private:
	ABool									mNoMemoryState;
	*/
	//
  public:
	ABool						IsNotCodeState() const { return mNotCodeState; }
	void						SetNotCodeState() { mNotCodeState = true; }
  private:
	ABool									mNotCodeState;
	
	//
  public:
	ABool						IsCommentState() const { return mCommentState; }
	void						SetCommentState() { mCommentState = true; }
  private:
	ABool									mCommentState;
	
	//B0000
  public:
	ABool						IsGlobalState() const { return mGlobalState; }
	void						SetGlobalState() { mGlobalState = true; }
  private:
	ABool									mGlobalState;
	
	//
  public:
	ABool						IsIndentOnTheWay() const { return mIndentOnTheWay; }
	void						SetIndentOnTheWay() { mIndentOnTheWay = true; }
  private:
	ABool									mIndentOnTheWay;
	
	//
  public:
	ABool						IsStable() const { return mStable; }
	void						SetStable() { mStable = true; }
  private:
	ABool									mStable;
	
	//R0199
  public:
	ABool						IsNoSpellCheck() const { return mNoSpellCheck; }
	void						SetNoSpellCheck() { mNoSpellCheck = true; }
  private:
	ABool									mNoSpellCheck;
	
	//条件文字列大文字小文字無視
  public:
	ABool						IsIgnoreCase() const { return mIgnoreCase; }
	void						SetIgnoreCase() { mIgnoreCase = true; }
  private:
	ABool									mIgnoreCase;
	
	//補完再変換
  public:
	ABool						IsEnableRecompletion() const { return mEnableRecompletion; }
	void						SetEnableRecompletion() { mEnableRecompletion = true; }
  private:
	ABool									mEnableRecompletion;
	
	//インデントdisable
  public:
	void						SetDisableIndent() { mDisableIndent = true; }
	ABool						IsDisableIndent() const { return mDisableIndent; }
  private:
	ABool									mDisableIndent;
	
	//#467 directive無効stateかどうか
  public:
	ABool						IsDisableDirective() const { return mDisableDirective; }
	void						SetDisableDirective() { mDisableDirective = true; }
  private:
	ABool									mDisableDirective;
	
	//#717
	//補完候補表示用 クラス文字列の後の状態かどうか
  public:
	ABool						IsAfterClassState() const { return mAfterClassState; }
	void						SetAfterClassState() { mAfterClassState = true; }
	void						SetAfterClassState_ParentKeyword( const AText& inText ) { mAfterClassState_ParentKeyword.SetText(inText); }
	void						GetAfterClassState_ParentKeyword( AText& outText ) { outText.SetText(mAfterClassState_ParentKeyword); }
  private:
	ABool									mAfterClassState;
	AText									mAfterClassState_ParentKeyword;
	
	//
  public:
	const AArray<AItemCount>&			GetConditionTextByteCountArray( const AIndex inIndex ) const;//#693 const AUChar inChar ) const;
	const AArray<AIndex>&				GetConditionTextByteCountArrayIndexTable() const;//#693
	void						MakeConditionTextByteCountArray();
	
	//R0000 カテゴリー可否
  public:
	ABool						GetAllCategoryDisabled() const { return mAllCategoryDisabled; }
	void						SetAllCategoryDisabled() { mAllCategoryDisabled = true; }
	//#844 const AArray<AIndex>&		GetEnabledCategoryArray() const { return mEnabledCategoryArray; }
	//#844 void						AddEnebledCategoryArray( const AIndex inCategoryIndex ) { mEnabledCategoryArray.Add(inCategoryIndex); }
  private:
	ABool									mAllCategoryDisabled;
	//#844 AArray<AIndex>							mEnabledCategoryArray;
	
	//#844
	//カラースロット
  public:
	void						SetColorSlot( const ANumber inSlotNumber ) { mColorSlot = inSlotNumber; }
	AIndex						GetColorSlot() const { return mColorSlot; }
  private:
	AIndex									mColorSlot;
	
	//文法パート
  public:
	void						SetStateSyntaxPartIndex( const AIndex inSytaxPartIndex ) { mSyntaxPartIndex = inSytaxPartIndex; }
	AIndex						GetStateSyntaxPartIndex() const { return mSyntaxPartIndex; }
  private:
	AIndex									mSyntaxPartIndex;
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "ASyntaxDefinitionState"; }
	
};

#pragma mark ===========================
#pragma mark [クラス]ASyntaxDefinition
//文法定義を保持するクラス

class ASyntaxDefinition : public AObjectArrayItem
{
	//コンストラクタ、デストラクタ
  public:
	ASyntaxDefinition();
  private:
	void						RegisterReservedWord( AConstCharPtr inWordText, const ASyntaxDefinitionReservedWord inWordNumber );
	
	//
  public:
	void						DeleteAll();
	
	//構文解析
  public:
	ABool						Parse( const AFileAcc& inFileAcc );
  private:
	void						Parse_GetToken( const AUChar* inTextPtr, const AItemCount inTextLen, AIndex& ioPosition, 
											   AText& outToken, ASyntaxDefinitionReservedWord& outReservedWord, AUChar& outChar );
	void						Parse_GetText( const AUChar* inTextPtr, const AItemCount inTextLen, AIndex& ioPosition, 
											  const AUChar inDelimiter, AText& outText );
	void						Parse_GetNumber( const AUChar* inTextPtr, const AItemCount inTextLen, AIndex& ioPosition, ANumber& outNumber );
	void 						Parse_CheckTokenReservedWord( const AUChar* inTextPtr, const AItemCount inTextLen, AIndex& ioPosition, 
															 const AIndex inReservedWordNumber );
	void 						Parse_CheckToken( const AUChar* inTextPtr, const AItemCount inTextLen, AIndex& ioPosition, 
												 const AUChar inChar );
	
	//構文解析エラー情報
  public:
	ABool						GetParseResult() const { return mParseResult; }//#349
	AIndex						GetErrorLineNumber() const {return mCurrentLineNumber;}
	void						GetErrorText( AText& outText ) const;//#349
	static void					ParseSDFNameOnly( const AText& inText, AText& outSDFName );//#402
	
  private:
	AIndex									mCurrentLineNumber;
	ABool									mParseResult;//#349
	AIndex									mErrorNumber;//#349
	AChar									mErrorExpectedChar;//#349
	AIndex									mErrorExpectedReservedWordIndex;//#349
	
  private:
	AIndex									mSyntaxErrorVarIndex;
	AIndex									mSyntaxWarningVarIndex;
	
	//状態
  public:
	ASyntaxDefinitionState&		GetSyntaxDefinitionState( const AIndex inStateIndex ) {return mStateArray.GetObject(inStateIndex);}
	const ASyntaxDefinitionState&		GetSyntaxDefinitionStateConst( const AIndex inStateIndex ) const {return mStateArray.GetObjectConst(inStateIndex);}//#14
	AItemCount					GetStateCount() { return mStateArray.GetItemCount(); }
	const AText					GetStateName( const AIndex inStateIndex ) const { return mStateNameArray.GetTextConst(inStateIndex); }
  private:
	AObjectArray<ASyntaxDefinitionState>	mStateArray;
	AHashTextArray							mStateNameArray;
	
	//変数
  public:
	AItemCount					GetVariableCount() { return mVariableNameArray.GetItemCount(); }
	ABool						GetVariableBlockStartFlag( const AIndex inVariableIndex ) const 
	{ return mVariableArray_BlockStartFlag.Get(inVariableIndex); }//#695
	AIndex						FindVariableIndex( const AText& inVariableName ) const { return mVariableNameArray.Find(inVariableName); }
  private:
	AHashTextArray							mVariableNameArray;
	AArray<ABool>							mVariableArray_BlockStartFlag;//#695
	
	//予約語データ
  private:
	AHashTextArray							mReservedWord_Text;
	AArray<ASyntaxDefinitionReservedWord>	mReservedWord_Number;
	
	//SDF name R0000
  public:
	void						GetSDFName( AText& outText ) const { outText.SetText(mSDFName); }
  private:
	AText									mSDFName;
	
	//初期状態取得 R0000
  public:
	AIndex						GetInitialState( const AText& inFileURL, const AText& inSyntaxPartName ) const;//#998
  private:
	AHashTextArray							mInitialState_Suffix;
	AArray<AIndex>							mInitialState_Index;
	
	//簡易文法定義
  public:
	void						MakeSimpleDefinition( const AText& inCommentStart, const AText& inCommentEnd, const AText& inLineCommentStart,
													  const AText& inLiteral1Start, const AText& inLiteral1End, const AText& inLiteral2Start, const AText& inLiteral2End,
													  const AText& inEscapeText, const ABool inCommentStartIsRegExp,
													  const AText& inIncIndentRegExp, const AText& inDecInentRegExp );
	
	//キーワード／カテゴリー
  public:
	//#844 void						GetKeywordData( AIdentifierList& ioIdentifierList ) const;
	const AArray<AIndex>&		GetCategoryColorSlot() const { return mCategory_ColorSlot; }
	const ATextArray&			GetCategoryNameArray() const { return mCategory_Name; }
	//const AArray<AColor>&		GetCategoryDefaultColorArray() const { return mCategory_DefaultColor; }
	//const AArray<AColor>&		GetCategoryDefaultImportColorArray() const { return mCategory_DefaultImportColor; }
	//const AArray<AColor>&		GetCategoryDefaultLocalColorArray() const { return mCategory_DefaultLocalColor; }
	const AArray<ATextStyle>&	GetCategoryMenuTextStyleArray() const { return mCategory_MenuTextStyle; }
	//void						GetCategoryDisabledStateArray( AObjectArray<ABoolArray>& ioCategoryDisabledStateArray ) const;//R0000 カテゴリー可否
	//#844 void						GetStateColorSourceStateName( const AText& inName, AText& outSourceStateName ) const;//#14
  private:
	ATextArray								mKeywordArray;
	AArray<AIndex>							mKeywordArray_Category;
	ATextArray								mCategory_Name;
	AArray<AColor>							mCategory_DefaultColor;
	AArray<AColor>							mCategory_DefaultImportColor;
	AArray<AColor>							mCategory_DefaultLocalColor;
	AArray<ATextStyle>						mCategory_MenuTextStyle;
	AArray<AIndex>							mCategory_ColorSlot;
	
	/*#844
  public:
	void						GetStateColorArray( ATextArray& outNameArray, AArray<AColor>& outColorArray, ATextArray& outDisplayNameArray ) const;
	*/
	
	//win 080722
  public:
	ABool						GetImportLinkToProjectAll() const { return mImportLinkToProjectAll; }
  private:
	ABool									mImportLinkToProjectAll;
	
	/*#907
	//win 080722
  public:
	ABool						IsStartAlphabet( const AUChar inChar ) const { return mStartAlphabet[inChar]; }
	ABool						IsContinuousAlphabet( const AUChar inChar ) const { return mContinuousAlphabet[inChar]; }
	*/
  private:
	ABool									mStartAlphabet[256];
	ABool									mContinuousAlphabet[256];
	
	//win 080725
  public:
	ABool						IsImportRecursive( const AText& inSuffix ) const
	{
		if( mImportRecursiveAll == true )   return true;
		if( mImportRecursiveSuffix.FindIgnoreCase(inSuffix) != kIndex_Invalid )   return true;
		return false;
	}
  private:
	ATextArray								mImportRecursiveSuffix;
	ABool									mImportRecursiveAll;
	
	//RC2
  public:
	ABool						IsIdInfoAutoDisplayChar( const AUChar inChar ) const { return mIdInfoAutoDisplayChar[inChar]; }
	ABool						IsIdInfoStartChar( const AUChar inChar ) const { return mIdInfoArgStart[inChar]; }
	ABool						IsIdInfoEndChar( const AUChar inChar ) const { return mIdInfoArgEnd[inChar]; }
	ABool						IsIdInfoDelimiterChar( const AUChar inChar ) const { return mIdInfoArgDelimiter[inChar]; }
	AItemCount					GetIdInfoArgStartCharCount() const { return mIdInfoArgStartCharCount; }//#225 #853
  private:
	ABool									mIdInfoAutoDisplayChar[256];
	ABool									mIdInfoArgStart[256];
	ABool									mIdInfoArgEnd[256];
	ABool									mIdInfoArgDelimiter[256];
	AItemCount								mIdInfoArgStartCharCount;//#225 #853
	
	//正規表現
  public:
	void						GetRegExpArray( AObjectArray<ARegExp>& outRegExpArray, AArray<AIndex>& outStateArray, AArray<AIndex>& outSeqArray );
  private:
	void						RegisterRegExp( const AText& inRegExpText, const AIndex inStateIndex, const AIndex inSequenceIndex );
	ATextArray								mRegExp_Text;
	AArray<AIndex>							mRegExp_StateIndex;
	AArray<AIndex>							mRegExp_SequenceIndex;
	
	//R0243
  public:
	ABool						IsAfterClassString( const AText& inText, const AIndex inTextPos, AItemCount& outClassStringLength ) const;
	ABool						IsAfterInstanceString( const AText& inText, const AIndex inTextPos, AItemCount& outInstanceStringLength ) const;
	void						GetFirstClassString( AText& outText ) const;
  private:
	ATextArray								mClassStringArray;
	ATextArray								mInstanceStringArray;
	
	//#467 directive
  public:
	ABool						IsDirectiveString( const AText& inText ) const;
	ABool						IsDirectiveIfString( const AText& inText ) const;
	ABool						IsDirectiveIfdefString( const AText& inText ) const;
	ABool						IsDirectiveIfndefString( const AText& inText ) const;
	ABool						IsDirectiveElseString( const AText& inText ) const;
	ABool						IsDirectiveElifString( const AText& inText ) const;
	ABool						IsDirectiveEndifString( const AText& inText ) const;
  private:
	AHashTextArray							mDirectiveStringArray;
	AHashTextArray							mDirectiveIfStringArray;
	AHashTextArray							mDirectiveIfdefStringArray;
	AHashTextArray							mDirectiveIfndefStringArray;
	AHashTextArray							mDirectiveElseStringArray;
	AHashTextArray							mDirectiveElifStringArray;
	AHashTextArray							mDirectiveEndifStringArray;
	
	//文法パート #907
  public:
	AItemCount					GetSyntaxPartCount() const
	{ return mSyntaxPartTextArray.GetItemCount(); }
	void						GetSyntaxPartName( const AIndex inIndex, AText& outName ) const
	{ mSyntaxPartTextArray.Get(inIndex,outName); }
	AIndex						GetSyntaxPartIndexFromName( const AText& inName ) const 
	{ return mSyntaxPartTextArray.Find(inName); }
	const AHashTextArray&		GetSyntaxPartTextArray() const { return mSyntaxPartTextArray; }
	ABool						IsStartAlphabet( const AUChar inChar, const AIndex inSyntaxPartIndex ) const
	{
		if( inSyntaxPartIndex == kIndex_Invalid )
		{
			//文法パート指定無しの場合
			return mStartAlphabet[inChar];
		}
		else
		{
			//文法パート指定有りの場合
			return mSyntaxPartArray_StartAlphabet.GetObjectConst(inSyntaxPartIndex).Get(inChar);
		}
	}
	ABool						IsContinuousAlphabet( const AUChar inChar, const AIndex inSyntaxPartIndex ) const
	{
		if( inSyntaxPartIndex == kIndex_Invalid )
		{
			//文法パート指定無しの場合
			return mContinuousAlphabet[inChar];
		}
		else
		{
			//文法パート指定有りの場合
			return mSyntaxPartArray_ContinuousAlphabet.GetObjectConst(inSyntaxPartIndex).Get(inChar);
		}
	}
	ABool						IsTailAlphabet( const AUChar inChar, const AIndex inSyntaxPartIndex ) const
	{
		if( inSyntaxPartIndex == kIndex_Invalid )
		{
			//文法パート指定無しの場合
			return false;
		}
		else
		{
			//文法パート指定有りの場合
			return mSyntaxPartArray_TailAlphabet.GetObjectConst(inSyntaxPartIndex).Get(inChar);
		}
	}
	const ATextArray&			GetCorrespondStartArray( const AIndex inIndex ) const
	{ return mSyntaxPartArray_CorrespondStart.GetObjectConst(inIndex); }
	const ATextArray&			GetCorrespondEndArray(const AIndex inIndex) const
	{ return mSyntaxPartArray_CorrespondEnd.GetObjectConst(inIndex); }
	const void					GetSyntaxPartSeparatorName( const AIndex inIndex, AText& outName ) const 
	{ return mSyntaxPartArray_SeparatorName.Get(inIndex,outName); }
  private:
	AHashTextArray							mSyntaxPartTextArray;
	ATextArray								mSyntaxPartArray_SeparatorName;
	ANumberArray							mSyntaxPartArray_InitialStateIndex;
	AObjectArray< ABoolArray >				mSyntaxPartArray_StartAlphabet;
	AObjectArray< ABoolArray >				mSyntaxPartArray_ContinuousAlphabet;
	AObjectArray< ABoolArray >				mSyntaxPartArray_TailAlphabet;
	//
	AObjectArray< ATextArray >				mSyntaxPartArray_CorrespondStart;
	AObjectArray< ATextArray >				mSyntaxPartArray_CorrespondEnd;
	
	//#997
  public:
	ABool						GetDisableAutoIndent() const { return mDisableAutoIndent; }
  private:
	ABool									mDisableAutoIndent;
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "ASyntaxDefinition"; }
	
};

#pragma mark ===========================
#pragma mark [クラス]ASyntaxDefinitionStateFactory
class ASyntaxDefinitionStateFactory : public AAbstractFactoryForObjectArray<ASyntaxDefinitionState>
{
  public:
	ASyntaxDefinitionStateFactory( AObjectArrayItem* inParent , ASyntaxDefinition& inSyntaxDefinition )
			: mParent(inParent), mSyntaxDefinition(inSyntaxDefinition)
	{
	}
	ASyntaxDefinitionState*	Create() 
	{
		return new ASyntaxDefinitionState(mParent,mSyntaxDefinition);
	}
  private:
	AObjectArrayItem* mParent;
	ASyntaxDefinition&	mSyntaxDefinition;
};

#pragma mark ===========================
#pragma mark [クラス]ASyntaxDefinitionActionSequenceFactory
class ASyntaxDefinitionActionSequenceFactory : public AAbstractFactoryForObjectArray<ASyntaxDefinitionActionSequence>
{
  public:
	ASyntaxDefinitionActionSequenceFactory( AObjectArrayItem* inParent, ASyntaxDefinitionState& inSyntaxDefinitionState )
			: mParent(inParent), mSyntaxDefinitionState(inSyntaxDefinitionState)
	{
	}
	ASyntaxDefinitionActionSequence*	Create() 
	{
		return new ASyntaxDefinitionActionSequence(mParent,mSyntaxDefinitionState);
	}
  private:
	AObjectArrayItem* mParent;
	ASyntaxDefinitionState&	mSyntaxDefinitionState;
};



