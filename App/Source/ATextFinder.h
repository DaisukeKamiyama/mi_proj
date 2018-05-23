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

ATextFinder

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AFindParameter
//
class AFindParameter
{
  public:
	AFindParameter() 
			: ignoreCase(true), wholeWord(false), fuzzy(true), regExp(false), loop(false), ignoreBackslashYen(true),
			recursive(true), displayPosition(true), //modeIndex(kIndex_Invalid),
			onlyVisibleFile(false), onlyTextFile(false)//B0313
			,ignoreSpaces(false)//#165
	{}
	~AFindParameter() {}
	//共通
	AText	findText;
	AText	replaceText;
	ABool	ignoreCase;
	ABool	wholeWord;
	ABool	fuzzy;
	ABool	regExp;
	ABool	loop;
	ABool	ignoreBackslashYen;
	ABool	ignoreSpaces;//#165
	//マルチファイル検索のみ
	AText	filterText;
	ABool	recursive;
	ABool	displayPosition;
	AText	folderPath;
	//AModeIndex	modeIndex;
	ABool	onlyVisibleFile;//B0313
	ABool	onlyTextFile;//B0313
	//メンバー追加時はSet()も修正しましょう！
	
	void	Set( const AFindParameter& inSrc );
	
	//使用禁止
	private:
	AFindParameter( const AFindParameter& );//コピー生成子
	AFindParameter& operator=( const AFindParameter& );//代入演算子
};

#pragma mark ===========================
#pragma mark [クラス]ATextFinder
//テキスト検索
class ATextFinder : public AObjectArrayItem //#750
{
	//コンストラクタ／デストラクタ
  public:
	ATextFinder( AObjectArrayItem* inParent = NULL );
	
	//検索実行
  public:
	ABool	ExecuteFind( const AText& inText, const AFindParameter& inParam, const ABool inReverse,//#789
			const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos,
			const ABool& inAbortFlag, //B0359
			const ABool inUseModalSession, ABool& outModalSessionAborted );
	void	ChangeReplaceText( const AText& inTargetText, const AText& inReplaceText, AText& outText ) const;
  private:
	ABool	BMFind( const AText& inText, const AFindParameter& inParam, const ABool inReverse,//#789
			const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos,
			const ABool& inAbortFlag, //B0359
			const ABool inUseModalSession, ABool& outModalSessionAborted );
	ABool	RegExpFind( const AText& inText, const AFindParameter& inParam, const ABool inReverse,//#789
			const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos,
			const ABool& inAbortFlag, //B0359
			const ABool inUseModalSession, ABool& outModalSessionAborted );
	
	//#166
  private:
	ABool	IsIgnoreChar( const AUCS4Char inUCS4Char, const ABool isIgnoreSpaces, const ABool isIgnoreOnbiki ) const;
	
  private:
	void	Init();
	ABool								mInited;
	AArray<AIndex>						mBMTable;
	//#733 AArray<AUCS4Char>					mFindText;//#412
	AUCS4Char							mFindText[kBufferSize_TextFinder_UCS4FindText];//#733
	AItemCount							mFindTextLen;//#733
	ARegExp								mRegExp;
	
	AText	mCurrentParameter_FindText;
	ABool	mCurrentParameter_IgnoreCase;
	ABool	mCurrentParameter_IgnoreBackslashYen;
	ABool	mCurrentParameter_IgnoreSpaces;//#166
	//#166 ABool	mCurrentParameter_Fuzzy;
	ABool	mCurrentParameter_Fuzzy_NFKD;//#166
	ABool	mCurrentParameter_Fuzzy_KanaType;//#166
	ABool	mCurrentParameter_Fuzzy_Onbiki;//#166
	ABool	mCurrentParameter_Fuzzy_KanaYuragi;//#166
	ABool	mCurrentParameter_Reverse;//#789
	
	static const AItemCount	kBMTableCount = 0x10000;
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "ATextFinder"; }
};
