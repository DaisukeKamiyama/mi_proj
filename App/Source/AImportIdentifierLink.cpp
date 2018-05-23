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

AImportIdentifierLinkList

*/

#include "stdafx.h"

#include "AImportIdentifierLink.h"
#include "AApp.h"
#include "AImportFileManager.h"
#include "ATextInfo.h"

/*
ハッシュサイズは10倍とする。
Importの場合、同じ名前のキーワードが重複することが多く、ハッシュが団子になりやすい
そのため、ハッシュサイズを大きめにして、団子にぶつかる可能性を低くする。
＜AApp.cppでの例＞
ハッシュサイズ：158K(10倍)
FindCount: 21544
CompareCount: 14124
HitCount: 3069
MaxContinuous: 976
同程度のFindCountの場合に、
ハッシュサイズ5倍で、CompareCount: 98114
ハッシュサイズ4倍で、CompareCount: 306280

*/
const AItemCount	kImportIdentifierLinkListHashMultiple = 10;

//高速化のため、AImportIdentifierLinkクラスは削除しました。
//（AImportIdentifierLinkList内に配列としてデータを保持するように変更）

#pragma mark ===========================
#pragma mark [クラス]AImportIdentifierLinkList
#pragma mark ===========================
//インポート識別子リンクリスト
//各テキストドキュメントが所持するオブジェクト。
//インポートファイルの識別子情報へのリンク情報を保持する。
//下記の２つのprivateメンバ配列にリンクデータが保存される。
//リンクデータ（ハッシュ）
//ImportFileDataへのポインタ
//AArray<AImportFileData*>	mHash_ImportFileData;
//ImportFileData内のインデックス
//AArray<AIndex>				mHash_IdentifierIndex;


#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AImportIdentifierLinkList::AImportIdentifierLinkList() : AObjectArrayItem(NULL)
{
}
//デストラクタ
AImportIdentifierLinkList::~AImportIdentifierLinkList()
{
}

#pragma mark ===========================

#pragma mark ---追加／削除

//引数配列に格納されたObjectIDのImportFileData（ファイル毎に存在するオブジェクト）へのリンクを全て追加する
void	AImportIdentifierLinkList::UpdateIdentifierLinkList( const AHashArray<AObjectID>& inImportFileDataObjectIDArray )//#423
{
	//
	AItemCount	totalIdentifierCount = 0;
	for( AIndex i = 0; i < inImportFileDataObjectIDArray.GetItemCount(); i++ )
	{
		AObjectID	importFileDataObjectID = inImportFileDataObjectIDArray.Get(i);
		totalIdentifierCount += GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataObjectID).GetTextInfoConst().
				GetGlobalIdentifierListCount();
		/*
		//
		AText	filename;
		GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataObjectID).GetFile().GetFilename(filename);
		AText	t;
		t.SetFormattedCstring("Arg:%d:",importFileDataObjectID.val);
		t.AddText(filename);
		t.Add(13);
		t.OutputToStderr();
		*/
	}
	//#423
	if( mHash_ImportFileDataID.GetItemCount() <= 
				totalIdentifierCount*(kImportIdentifierLinkListHashMultiple/2) )
	{
		//==================ハッシュ再作成==================
		//現在のハッシュサイズがidentifierの数の5倍未満ならハッシュを新規作成する
		
		//
		InitHash(totalIdentifierCount);
		//ハッシュは新規作成され、下でImportFileDataを追加するので、revision dataも全削除
		mRevisionData_ImportFileDataID.DeleteAll();
		mRevisionData_RevisonNumber.DeleteAll();
		//Hashに登録
		//各ImportFileDataごとのループ
		for( AIndex i = 0; i < inImportFileDataObjectIDArray.GetItemCount(); i++ )
		{
			AObjectID	importFileDataObjectID = inImportFileDataObjectIDArray.Get(i);
			//#423 AImportFileData*	importFileData = &(GetApp().SPI_GetImportFileManager().GetImportFileDataByID(importFileDataObjectID));
			//#423 AItemCount	identifierCount = importFileData->GetTextInfoConst().GetGlobalIdentifierListCount();
			const AImportFileData&	importFileData = GetApp().SPI_GetImportFileManager().
									GetImportFileDataConstByID(importFileDataObjectID);
			AItemCount	identifierCount = importFileData.GetTextInfoConst().GetGlobalIdentifierListCount();//#423
			//ImportFileData内の全てのidentifierを登録
			for( AIndex j = 0; j < identifierCount; j++ )
			{
				RegisterHash(/*#423 importFileData*/importFileDataObjectID,j);
			}
			//RevisionData登録
			mRevisionData_ImportFileDataID.Add(importFileDataObjectID);
			mRevisionData_RevisonNumber.Add(importFileData.GetRevisionNumber());
			/*
			//
			AText	filename;
			GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataObjectID).GetFile().GetFilename(filename);
			AText	t;
			t.SetFormattedCstring("NewReg:%d:",importFileDataObjectID.val);
			t.AddText(filename);
			t.Add(13);
			t.OutputToStderr();
			*/
		}
	}
	else
	{
		//==================ハッシュに追加登録==================
		//現在のハッシュサイズがidentifierの数の5倍以上なら、Revisionが同じものは更新しない
		
		//新規importFileDataについて、
		//1. mRevisionData_ImportFileDataIDになければ、そのままハッシュ登録し、
		//revisionDataを登録
		//2. mRevisionData_ImportFileDataIDにあって、Revisionが同じなら、何もしない
		//3. mRevisionData_ImportFileDataIDにあって、Revisionが違うなら、
		//ハッシュからそのimportFileDataIDのデータを削除後、ハッシュ登録し、
		//revisionDataを更新
		for( AIndex i = 0; i < inImportFileDataObjectIDArray.GetItemCount(); i++ )
		{
			AObjectID	importFileDataObjectID = inImportFileDataObjectIDArray.Get(i);
			const AImportFileData&	importFileData = GetApp().SPI_GetImportFileManager().
									GetImportFileDataConstByID(importFileDataObjectID);
			AItemCount	identifierCount = importFileData.GetTextInfoConst().GetGlobalIdentifierListCount();//#423
			AIndex	revisionDataIndex = mRevisionData_ImportFileDataID.Find(importFileDataObjectID);
			if( revisionDataIndex == kIndex_Invalid )
			{
				//------------------新規ImportFileData登録の場合------------------
				
				//ハッシュ登録
				for( AIndex j = 0; j < identifierCount; j++ )
				{
					RegisterHash(importFileDataObjectID,j);
				}
				//RevisionData登録
				mRevisionData_ImportFileDataID.Add(importFileDataObjectID);
				mRevisionData_RevisonNumber.Add(importFileData.GetRevisionNumber());
				/*
				//
				AText	filename;
				GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataObjectID).GetFile().GetFilename(filename);
				AText	t;
				t.SetFormattedCstring("NewReg:%d:",importFileDataObjectID.val);
				t.AddText(filename);
				t.Add(13);
				t.OutputToStderr();
				*/
			}
			else
			{
				if( importFileData.GetRevisionNumber() == mRevisionData_RevisonNumber.Get(revisionDataIndex) )
				{
					//------------------登録済みImportFileDataの場合（revisionに変化無し）------------------
					
					//何もしない
				}
				else
				{
					//------------------登録済みImportFileDataの場合（revisionに変化あり）------------------
					
					//ハッシュから削除
					DeleteFromHash(importFileDataObjectID);
					//ハッシュ登録
					for( AIndex j = 0; j < identifierCount; j++ )
					{
						RegisterHash(importFileDataObjectID,j);
					}
					//RevisionData更新
					mRevisionData_RevisonNumber.Set(revisionDataIndex,importFileData.GetRevisionNumber());
					/*
					//
					AText	filename;
					GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataObjectID).GetFile().GetFilename(filename);
					AText	t;
					t.SetFormattedCstring("Update:%d:",importFileDataObjectID.val);
					t.AddText(filename);
					t.Add(13);
					t.OutputToStderr();
					*/
				}
			}
		}
	}
	//==================今回のinImportFileDataObjectIDArrayに含まれないImport File Dataを削除==================
	//inImportFileDataObjectIDArrayにないが、revisiondataにあるデータは、ハッシュ・revisionDataから削除する。
	for( AIndex i = 0; i < mRevisionData_ImportFileDataID.GetItemCount(); )
	{
		AObjectID	importFileDataObjectID = mRevisionData_ImportFileDataID.Get(i);
		if( inImportFileDataObjectIDArray.Find(importFileDataObjectID) == kIndex_Invalid )
		{
			//今回の新規データには含まれないので、ハッシュ・revisionDataから削除する
			
			//ハッシュから削除
			DeleteFromHash(importFileDataObjectID);
			//RevisionData削除
			mRevisionData_ImportFileDataID.Delete1(i);
			mRevisionData_RevisonNumber.Delete1(i);
			/*
			//
			AText	filename;
			GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataObjectID).GetFile().GetFilename(filename);
			AText	t;
			t.SetFormattedCstring("Del:%d:",importFileDataObjectID.val);
			t.AddText(filename);
			t.Add(13);
			t.OutputToStderr();
			*/
		}
		else
		{
			//次へ
			i++;
		}
	}
}

//リンクデータ全て削除
void	AImportIdentifierLinkList::DeleteAll()
{
	//#423 mHash_ImportFileData.DeleteAll();
	mHash_ImportFileDataID.DeleteAll();//#423
	mHash_IdentifierIndex.DeleteAll();
}

//指定ObjectIDのImportFileData内のデータへのリンクを全て削除
ABool	AImportIdentifierLinkList::Delete( const AObjectID inImportFileDataObjectID )
{
	//==================リンクリスト内に存在するImport File Dataかどうかチェック==================
	AIndex	index = mRevisionData_ImportFileDataID.Find(inImportFileDataObjectID);
	//mRevisionData_ImportFileDataIDにimport file dataが無ければ、ハッシュにも存在しないので、何もせず終了 #0 高速化
	if( index == kIndex_Invalid )
	{
		return false;
	}
	
	//==================削除==================
	//未使用のためコメントアウト ABool	deleted = false;
	//#423 AImportFileData*	importFileData = &(GetApp().SPI_GetImportFileManager().GetImportFileDataByID(inImportFileDataObjectID));
	ABool	result = DeleteFromHash(/*#423 importFileData*/inImportFileDataObjectID);
	//RevisionData削除 #423
	if( index != kIndex_Invalid )
	{
		mRevisionData_ImportFileDataID.Delete1(index);
		mRevisionData_RevisonNumber.Delete1(index);
	}
	return result;
}

#pragma mark ===========================

#pragma mark ---検索

//キーワード（最初に一致したもの）のカテゴリー取得
ABool	AImportIdentifierLinkList::FindKeyword( const AText& inKeywordText, AIndex& outCategoryIndex ) const
{
	AIndex	index = Find(inKeywordText);
	if( index == kIndex_Invalid )   return false;
	outCategoryIndex = //#423 mHash_ImportFileData.Get(index)->GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index));
			GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(mHash_ImportFileDataID.Get(index)).
			GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index));
	return true;
}

//キーワード（最初に一致したもの）のカテゴリーとInfoText取得
ABool	AImportIdentifierLinkList::FindKeyword( const AText& inKeywordText, AFileAcc& outFile, AIndex& outCategoryIndex, AText& outInfoText ) const
{
	AIndex	index = Find(inKeywordText);
	if( index == kIndex_Invalid )   return false;
	outCategoryIndex = //#423 mHash_ImportFileData.Get(index)->GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index));
			GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(mHash_ImportFileDataID.Get(index)).
			GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index));
	//#348 outInfoText.SetText(mHash_ImportFileData.Get(index)->GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index)));
	//#423 mHash_ImportFileData.Get(index)->GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index),outInfoText);//#348
	GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(mHash_ImportFileDataID.Get(index)).
			GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index),outInfoText);//#348
	//#423 outFile.CopyFrom(mHash_ImportFileData.Get(index)->GetFile());
	outFile.CopyFrom(
			GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(mHash_ImportFileDataID.Get(index)).GetFile());
	return true;
}

//R0243
//キーワード（最初に一致したもの）のタイプ取得
ABool	AImportIdentifierLinkList::FindKeywordType( const AText& inKeywordText, AText& outTypeText ) const
{
	AIndex	index = Find(inKeywordText);
	if( index == kIndex_Invalid )   return false;
	//#348 outTypeText.SetText(mHash_ImportFileData.Get(index)->GetTextInfoConst().GetTypeTextByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index)));
	//#423 mHash_ImportFileData.Get(index)->GetTextInfoConst().GetTypeTextByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index),outTypeText);//#348
	GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(mHash_ImportFileDataID.Get(index)).
			GetTextInfoConst().GetTypeTextByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index),outTypeText);//#348
	return true;
}

//#853 #717
/**
各種キーワード検索スレッド用統合キーワード検索処理
@return falseの場合は項目数がlimit over
@note ハッシュを使わないので低速なため、スレッド使用が必須だが、いろいろな検索方法ができる。
*/
ABool	AImportIdentifierLinkList::SearchKeywordImport( const AText& inWord, const AHashTextArray& inParentWord, 
													   const AKeywordSearchOption inOption,
													   const AIndex inCurrentStateIndex,
													   AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
													   ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
													   ATextArray& outCommentTextArray,
													   ATextArray& outCompletionTextArray, ATextArray& outURLArray,
													   AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
													   AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
													   AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray,
													   AArray<AItemCount>& outMatchedCountArray,
													   const ABool& inAbort  ) const
{
	//結果（制限項目数内か）フラグ
	ABool	result = true;
	//検索オプションが無い場合は、検索高速化のため、ハッシュから検索する。（補完候補検索の場合）
	if( inOption == kKeywordSearchOption_None )
	{
		//検索無し時処理
		
		//キーワードに一致するデータのインデックスを取得
		AArray<AIndex>	indexArray;
		Find(inWord,indexArray);//ハッシュ検索
		//それぞれのデータについて、出力配列へのデータ追加
		for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
		{
			//AbortフラグONなら終了
			if( inAbort == true )
			{
				break;
			}
			//Import File Dataと、dataIndex取得
			AIndex	index = indexArray.Get(i);
			AIndex	dataIndex = mHash_IdentifierIndex.Get(index);
			AObjectID	importFileDataID = mHash_ImportFileDataID.Get(index);
			//Import File Dataポインタ取得
			AImportFileData*	importFileDataPtr = 
								GetApp().SPI_GetImportFileManager().GetImportFileDataAndRetain(importFileDataID);
			if( importFileDataPtr != NULL )//既にImportFileDataが削除済みの場合は、NULLが返る
			{
				//上記のポインタretainを確実に解放するためのスタッククラス
				AStDecrementRetainCounter	releaser(importFileDataPtr);
				
				AImportFileData&	importFileData = *(importFileDataPtr);
				//#723
				//ImportFileDataの更新を行う間、words list finderがImportFileData(の中のTextInfo)にアクセスしないように排他する
				AStImportFileDataLocker	locker(importFileData);
				//
				//キーワード取得
				AText	keywordText;
				importFileData.GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex,keywordText);
				//親キーワード取得
				AText	parentKeywordText;
				importFileData.GetTextInfoConst().GetParentKeywordTextByGlobalIdentifierIndex(dataIndex,parentKeywordText);
				//Type取得
				AText	typeText;
				importFileData.GetTextInfoConst().GetTypeTextByGlobalIdentifierIndex(dataIndex,typeText);
				//InfoText取得
				AText	infoText;
				importFileData.GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex,infoText);
				//コメントテキスト取得
				AText	commentText;
				importFileData.GetTextInfoConst().GetCommentTextByGlobalIdentifierIndex(dataIndex,commentText);
				//カテゴリindex取得
				AIndex	categoryIndex = importFileData.GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(dataIndex);
				//カラースロット
				AIndex	colorSlotIndex = importFileData.GetTextInfoConst().GetColorSlotIndexByGlobalIdentifierIndex(dataIndex);
				//位置取得
				AUniqID	objectUniqID = importFileData.GetTextInfoConst().GetUniqIDByGlobalIdentifierIndex(dataIndex);
				ATextPoint	spt = {0}, ept = {0};
				importFileData.GetTextInfoConst().GetGlobalIdentifierRange(objectUniqID,spt,ept);
				//ファイルパス取得
				AText	filepath;
				importFileData.GetFile().GetPath(filepath);
				/*
				//
				if( compareParentKeyword == true )
				{
				fprintf(stderr,"  list:");
				parentKeywordText.OutputToStderr();
				fprintf(stderr,"::");
				keywordText.OutputToStderr();
				fprintf(stderr," ");
				if( parentKeywordText.Compare(inParentWord) == false )
				{
				continue;
				}
				}
				*/
				//
				outKeywordArray.Add(keywordText);
				outParentKeywordArray.Add(parentKeywordText);
				outTypeTextArray.Add(typeText);
				outInfoTextArray.Add(infoText);
				outCommentTextArray.Add(commentText);
				outCompletionTextArray.Add(GetEmptyText());
				outURLArray.Add(GetEmptyText());
				outCategoryIndexArray.Add(categoryIndex);
				outColorSlotIndexArray.Add(colorSlotIndex);
				outStartIndexArray.Add(importFileData.GetTextInfoConst().GetTextIndexFromTextPoint(spt));
				outEndIndexArray.Add(importFileData.GetTextInfoConst().GetTextIndexFromTextPoint(ept));
				outScopeArray.Add(kScopeType_Import);
				outFilePathArray.Add(filepath);
				outMatchedCountArray.Add(1);
				//制限数オーバーなら終了
				if( outKeywordArray.GetItemCount() >= kLimit_KeywordSearchResultCount )
				{
					return false;
				}
			}
		}
	}
	else
	{
		//検索オプション有り時処理（少し時間がかかる）
		
		//ハッシュから、ImportFileDataIDのarrayを生成
		AHashArray<AObjectID>	importFileDataIDArray;
		//ハッシュ内の全てのデータから、importFileDataIDを取得して、importFileDataIDArrayへ（重複しないように）格納。
		{
			AItemCount	hashCount = mHash_ImportFileDataID.GetItemCount();
			for( AIndex i = 0; i < hashCount; i++ )
			{
				AIndex	dataIndex = mHash_IdentifierIndex.Get(i);
				AObjectID	importFileDataID = mHash_ImportFileDataID.Get(i);
				if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   continue;
				if( importFileDataID == kObjectID_Invalid )   { _ACError("",NULL); continue; }
				//重複していなければ、importFileDataIDArrayへ追加
				if( importFileDataIDArray.Find(importFileDataID) == kIndex_Invalid )
				{
					importFileDataIDArray.Add(importFileDataID);
				}
			}
		}
		//検索実行
		//（AImportIdentifierLinkListのハッシュは使わず、直接TextInfo内のGlobalIdentifiierListから検索する。）
		//各ImportFileData毎のループ
		AItemCount	itemCount = importFileDataIDArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			AIndex	startIndex = outKeywordArray.GetItemCount();
			//Import File Dataポインタ取得
			AImportFileData*	importFileDataPtr = 
								GetApp().SPI_GetImportFileManager().GetImportFileDataAndRetain(importFileDataIDArray.Get(i));
			if( importFileDataPtr != NULL )//既にImportFileDataが削除済みの場合は、NULLが返る
			{
				//上記のポインタretainを確実に解放するためのスタッククラス
				AStDecrementRetainCounter	releaser(importFileDataPtr);
				
				AImportFileData&	importFileData = *(importFileDataPtr);
				//#723
				//ImportFileDataの更新を行う間、words list finderがImportFileData(の中のTextInfo)にアクセスしないように排他する
				AStImportFileDataLocker	locker(importFileData);
				//
				AArray<AScopeType>	scopeArray;
				if( importFileData.GetTextInfoConst().SearchKeywordGlobal(inWord,inParentWord,inOption,
																		  inCurrentStateIndex,
																		  outKeywordArray,outParentKeywordArray,
																		  outTypeTextArray,outInfoTextArray,outCommentTextArray,
																		  outCompletionTextArray,outURLArray,
																		  outCategoryIndexArray,outColorSlotIndexArray,
																		  outStartIndexArray,outEndIndexArray,scopeArray,
																		  outMatchedCountArray,inAbort) == false )
				{
					result = false;
				}
				//ファイルパスとscopeを追加。
				AText	path;
				importFileData.GetFile().GetPath(path);
				for( AIndex j = startIndex; j < outKeywordArray.GetItemCount(); j++ )
				{
					outScopeArray.Add(kScopeType_Import);
					outFilePathArray.Add(path);
				}
			}
		}
	}
	
	return result;
}

#if 0
//RC2
//補完入力候補を全て取得（出力先配列にはデータが追加され、削除はされない）
void	AImportIdentifierLinkList::GetAbbrevCandidate( const AText& inText, const AFileAcc& inDocFile, 
		ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
		AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray, AObjectArray<AFileAcc>& outFileArray ) const
{
	AText	docfilename;
	inDocFile.GetFilename(docfilename);
	docfilename.DeleteAfter(docfilename.GetSuffixStartPos());
	//
	AIndex	startIndex = outAbbrevCandidateArray.GetItemCount();
	AIndex	otherFileStartIndex = startIndex;
	//データ内を全て検索
	AItemCount	itemCount = /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AIndex	dataIndex = mHash_IdentifierIndex.Get(i);
		//#423 AImportFileData*	importFileData = mHash_ImportFileData.Get(i);
		AObjectID	importFileDataID = mHash_ImportFileDataID.Get(i);//#423
		if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   continue;
		if( /*#423 importFileData == NULL*/importFileDataID == kObjectID_Invalid )   _ACThrow("",NULL);
		
		//#423
		const AImportFileData&	importFileData = 
				GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataID);
		//リンクデータの最初の文字と、引数inTextが一致するかどうか調べる
		//#348 const AText&	keywordText = importFileData->GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex);
		AText	keywordText;//#348
		importFileData/*#423->*/.GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex,keywordText);//#348
		if( keywordText.GetItemCount() >= inText.GetItemCount() )
		{
			if( keywordText.CompareMin(inText) == true )
			{
				//if( outAbbrevCandidateArray.Find(keywordText) == kIndex_Invalid )重複チェック→やめた
				{
					AIndex	index = otherFileStartIndex;
					AIndex	end = outAbbrevCandidateArray.GetItemCount();
					AScopeType	scopeType = kScopeType_Import;
					//拡張子のみが違うファイルの場合は、startIndexからotherFileStartIndexの間に挿入する（早く省略入力に表れるように）
					//それ以外の場合はotherFileStartIndexから最後までの間に挿入する
					AText	filename;
					importFileData/*#423->*/.GetFile().GetFilename(filename);
					filename.DeleteAfter(filename.GetSuffixStartPos());
					if( docfilename.Compare(filename) == true )
					{
						index = startIndex;
						end = otherFileStartIndex;
						scopeType = kScopeType_Import_Near;
						otherFileStartIndex++;
					}
					//挿入箇所検索
					//3:A
					//4:B
					//5:D
					//6:E
					//Cを挿入→3:C>A, 4:C>B, 5:C<D →5の位置に挿入
					for( ; index < end; index++ )
					{
						if( keywordText.IsLessThan(outAbbrevCandidateArray.GetTextConst(index)) == true )
						{
							break;
						}
					}
					//出力配列へデータ追加
					outAbbrevCandidateArray.Insert1(index,keywordText);
					//#348 outInfoTextArray.Insert1(index,importFileData->GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex));
					AText	infoText;//#348
					importFileData/*#423->*/.GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex,infoText);//#348
					outInfoTextArray.Insert1(index,infoText);//#348
					outCategoryIndexArray.Insert1(index,importFileData/*#423->*/.GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(dataIndex));
					outScopeArray.Insert1(index,scopeType);
					outFileArray.InsertNew1Object(index);
					outFileArray.GetObject(index).CopyFrom(importFileData/*#423->*/.GetFile());
				}
			}
		}
	}
}

//R0243
//ParentKeywordから補完入力候補を全て取得（出力先配列にはデータが追加され、削除はされない）
void	AImportIdentifierLinkList::GetAbbrevCandidateByParentKeyword( const AText& inParentKeyword, 
		ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
		AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray, AObjectArray<AFileAcc>& outFileArray ) const
{
	//
	AIndex	startIndex = outAbbrevCandidateArray.GetItemCount();
	//データ内を全て検索
	AItemCount	itemCount = /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AIndex	dataIndex = mHash_IdentifierIndex.Get(i);
		//#423 AImportFileData*	importFileData = mHash_ImportFileData.Get(i);
		AObjectID	importFileDataID = mHash_ImportFileDataID.Get(i);//#423
		if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   continue;
		if( /*#423 importFileData == NULL*/importFileDataID == kObjectID_Invalid )   _ACThrow("",NULL);
		
		//#423
		const AImportFileData&	importFileData = 
				GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataID);
		//ParentKeywordが一致するかどうか調べる
		//#348 if( importFileData->GetTextInfoConst().GetParentKeywordTextByGlobalIdentifierIndex(dataIndex).Compare(inParentKeyword) == true )
		AText	parentKeywordText;//#348
		importFileData/*#423->*/.GetTextInfoConst().GetParentKeywordTextByGlobalIdentifierIndex(dataIndex,parentKeywordText);//#348
		if( parentKeywordText.Compare(inParentKeyword) == true )//#348
		{
			//
			//#348 const AText&	keywordText = importFileData->GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex);
			AText	keywordText;//#348
			importFileData/*#423->*/.GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex,keywordText);//#348
			AIndex	index = startIndex;
			AIndex	end = outAbbrevCandidateArray.GetItemCount();
			//挿入箇所検索
			//3:A
			//4:B
			//5:D
			//6:E
			//Cを挿入→3:C>A, 4:C>B, 5:C<D →5の位置に挿入
			for( ; index < end; index++ )
			{
				if( keywordText.IsLessThan(outAbbrevCandidateArray.GetTextConst(index)) == true )
				{
					break;
				}
			}
			//出力配列へデータ追加
			outAbbrevCandidateArray.Insert1(index,keywordText);
			//#348 outInfoTextArray.Insert1(index,importFileData->GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex));
			AText	infoText;//#348
			importFileData/*#423->*/.GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex,infoText);//#348
			outInfoTextArray.Insert1(index,infoText);//#348
			outCategoryIndexArray.Insert1(index,importFileData/*#423->*/.GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(dataIndex));
			outScopeArray.Insert1(index,kScopeType_Import);
			outFileArray.InsertNew1Object(index);
			outFileArray.GetObject(index).CopyFrom(importFileData/*#423->*/.GetFile());
		}
	}
}
#endif

//キーワードから識別子のリストを取得
void	AImportIdentifierLinkList::GetIdentifierListByKeyword( const AText& inText, 
		AArray<AIndex>& outCategoryIndexArray, ATextArray& outInfoText, AObjectArray<AFileAcc>& outFileArray,
		AArray<ATextIndex>& outStartArray, AArray<ATextIndex>& outEndArray,
		ATextArray& outCommentTextArray, ATextArray& outParentKeywordTextArray ) const//RC2
{
	//キーワードに一致するデータのインデックスを取得
	AArray<AIndex>	indexArray;
	Find(inText,indexArray);
	//それぞれのデータについて、出力配列へのデータ追加
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		AIndex	index = indexArray.Get(i);
		AIndex	dataIndex = mHash_IdentifierIndex.Get(index);
		//#423 AImportFileData*	importFileData = mHash_ImportFileData.Get(index);
		AObjectID	importFileDataID = mHash_ImportFileDataID.Get(index);//#423
		const AImportFileData&	importFileData = 
				GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataID);//#423
		
		//
		outCategoryIndexArray.Add(importFileData/*#423->*/.GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(dataIndex));
		//
		//#348 outInfoText.Add(importFileData->GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex));
		AText	infoText;//#348
		importFileData/*#423->*/.GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex,infoText);//#348
		outInfoText.Add(infoText);//#348
		//
		AFileAcc	file;
		outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(importFileData/*#423->*/.GetFile());
		//
		AUniqID	objectID = importFileData/*#423->*/.GetTextInfoConst().GetUniqIDByGlobalIdentifierIndex(dataIndex);
		ATextPoint	spt, ept;
		importFileData/*#423->*/.GetTextInfoConst().GetGlobalIdentifierRange(objectID,spt,ept);
		outStartArray.Add(importFileData/*#423->*/.GetTextInfoConst().GetTextIndexFromTextPoint(spt));
		outEndArray.Add(importFileData/*#423->*/.GetTextInfoConst().GetTextIndexFromTextPoint(ept));
		//
		//#348 outCommentTextArray.Add(importFileData->GetTextInfoConst().GetCommentTextByGlobalIdentifierIndex(dataIndex));
		AText	commentText;//#348
		importFileData/*#423->*/.GetTextInfoConst().GetCommentTextByGlobalIdentifierIndex(dataIndex,commentText);//#348
		outCommentTextArray.Add(commentText);//#348
		//
		//#348 outParentKeywordTextArray.Add(importFileData->GetTextInfoConst().GetParentKeywordTextByGlobalIdentifierIndex(dataIndex));
		AText	parentKeywordText;//#348
		importFileData/*#423->*/.GetTextInfoConst().GetParentKeywordTextByGlobalIdentifierIndex(dataIndex,parentKeywordText);//#348
		outParentKeywordTextArray.Add(parentKeywordText);//#348
	}
}

#pragma mark ===========================

#pragma mark ---ハッシュ操作

//ハッシュ初期化
void	AImportIdentifierLinkList::InitHash( const AItemCount inItemCount )
{
	//サイズが大きすぎたらreturn
	if( inItemCount >= kItemCount_HashMaxSize/2 )
	{
		_ACError("cannot make hash because array is too big",this);
		return;
	}
	AItemCount	hashsize = inItemCount*kImportIdentifierLinkListHashMultiple;//*3;
	//ハッシュテーブル全削除
	//#423 mHash_ImportFileData.DeleteAll();
	mHash_ImportFileDataID.DeleteAll();//#423
	mHash_IdentifierIndex.DeleteAll();
	//ハッシュサイズが最大ハッシュサイズよりも大きければ、補正する。
	if( hashsize > kItemCount_HashMaxSize )   hashsize = kItemCount_HashMaxSize;
	//ハッシュテーブル確保
	//#423 mHash_ImportFileData.Reserve(0,hashsize);
	mHash_ImportFileDataID.Reserve(0,hashsize);//#423
	mHash_IdentifierIndex.Reserve(0,hashsize);
	//ハッシュテーブル初期化
	//高速化のため、ポインタ使用
	//#423 AStArrayPtr<AImportFileData*>	arrayptr1(mHash_ImportFileData,0,mHash_ImportFileData.GetItemCount());
	AStArrayPtr<AObjectID>			arrayptr1(mHash_ImportFileDataID,0,mHash_ImportFileDataID.GetItemCount());//#423
	AStArrayPtr<AIndex>				arrayptr2(mHash_IdentifierIndex,0,mHash_IdentifierIndex.GetItemCount());
	//#423 AImportFileData**	p1 = arrayptr1.GetPtr();
	AObjectID*			p1 = arrayptr1.GetPtr();//#423
	AIndex*				p2 = arrayptr2.GetPtr();
	for( AIndex i = 0; i < hashsize; i++ )
	{
		//#423 p1[i] = NULL;
		p1[i] = kObjectID_Invalid;//#423
		p2[i] = kIndex_HashNoData;
	}
}

//ハッシュへ登録
void	AImportIdentifierLinkList::RegisterHash( /*#423 AImportFileData* inImportFileData*/
			const AObjectID inImportFileDataID , const AIndex inIndex )
{
	if( /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount() == 0 )   _ACThrow("",NULL);
	
	//
	//#348 const AText& keywordText = inImportFileData->GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(inIndex);
	AText	keywordText;//#348
	//#423 inImportFileData->GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(inIndex,keywordText);//#348
	GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(inImportFileDataID).
	GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(inIndex,keywordText);
	
	//ハッシュ値計算
	unsigned long	hash = GetHashValue(keywordText);
	AIndex	hashstartpos = hash%(/*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount());
	AIndex	hashpos = hashstartpos;//#598
	{//#598 arrayptrの有効範囲を最小にする
		//高速化のため、ポインタ使用
		AStArrayPtr</*#216 AObjectID*/AIndex>	arrayptr(mHash_IdentifierIndex,0,mHash_IdentifierIndex.GetItemCount());
		AIndex*	p = arrayptr.GetPtr();
		//登録箇所検索
		//p1[]: ImportFileDataへのポインタ  p2[]: ImportFileData内のIndex、および、Hash制御データ
		//#598 上へ移動AIndex	hashpos = hashstartpos;
		while( true )
		{
			AIndex	dataIndex = p[hashpos];
			if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   break;
			hashpos = GetNextHashPos(hashpos);
			if( hashpos == hashstartpos )
			{
				_ACError("no area in hash",this);
				return;
			}
		}
	}
	//登録
	mHash_IdentifierIndex.Set(hashpos,inIndex);
	//#423 mHash_ImportFileData.Set(hashpos,inImportFileData);
	mHash_ImportFileDataID.Set(hashpos,inImportFileDataID);//#423
}

//ハッシュから指定ImportFileData削除
ABool	AImportIdentifierLinkList::DeleteFromHash( /*#423 AImportFileData* inImportFileData*/
			const AObjectID inImportFileDataID )
{
	ABool	deleted = false;
	//高速化のため、ポインタ使用
	//#423 AStArrayPtr<AImportFileData*>	arrayptr1(mHash_ImportFileData,0,mHash_ImportFileData.GetItemCount());
	AStArrayPtr<AObjectID>			arrayptr1(mHash_ImportFileDataID,0,mHash_ImportFileDataID.GetItemCount());//#423
	AStArrayPtr<AIndex>				arrayptr2(mHash_IdentifierIndex,0,mHash_IdentifierIndex.GetItemCount());
	//#423 AImportFileData**	p1 = arrayptr1.GetPtr();
	AObjectID*			p1 = arrayptr1.GetPtr();//#423
	AIndex*				p2 = arrayptr2.GetPtr();
	AItemCount	hashsize = /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount();
	for( AIndex hashpos = 0; hashpos < hashsize; hashpos++ )
	{
		if( p1[hashpos] == /*#423 inImportFileData*/inImportFileDataID )
		{
			//削除実行
			if( p2[GetNextHashPos(hashpos)] == kIndex_HashNoData )
			{
				//次のハッシュ位置がNoDataなら、NoDataを書く
				p1[hashpos] = kObjectID_Invalid;//#423 NULL;
				p2[hashpos] = kIndex_HashNoData;
				//NoDataを書き込んだとき、前のハッシュ位置（物理的には次）がDeletedならNoDataに書き換え
				//[hashpos-1]:NoData
				//[hashpos  ]:NoData←今回書き込み
				//[hashpos+1]:Deleted←このDeletedはNoDataにする（hashposの位置のデータへのリンクをつなげるために存在していたDeletedなので。）
				//[hashpos+2]:Deleted←さらに次もDeletedならここもNoData
				AIndex	hp = hashpos+1;
				if( hp >= /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount() )   hp = 0;
				while( p2[hp] == kIndex_HashDeleted )
				{
					//DeletedならNoDataに書き換えて継続
					p1[hp] = kObjectID_Invalid;//#423 NULL;
					p2[hp] = kIndex_HashNoData;
					hp++;
					if( hp >= /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount() )   hp = 0;
				}
			}
			else
			{
				//次のハッシュ位置がNoData以外、Deletedを書く
				p1[hashpos] = kObjectID_Invalid;//#423 NULL;
				p2[hashpos] = kIndex_HashDeleted;
			}
			deleted = true;
		}
	}
	return deleted;
}

//R-PFM ハッシュパフォーマンス測定用
AItemCount	gImportIdentifierLinkListFind_FindCount = 0;
AItemCount	gImportIdentifierLinkListFind_CompareCount = 0;
AItemCount	gImportIdentifierLinkListFind_HitCount = 0;
AItemCount	gImportIdentifierLinkListFind_MaxContinuous = 0;

//最初にキーワードが一致した項目のインデックスを取得
AIndex	AImportIdentifierLinkList::Find( const AText& inKeywordText ) const
{
	if( /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount() == 0 )   return kIndex_Invalid;
	
	//#0 高速化
	//Import File Dataオブジェクトarray取得
	AStMutexLocker	mutexlocker(GetApp().SPI_GetImportFileManager().GetImportFileDataArrayMutex());
	const AObjectArrayIDIndexed< AImportFileData >&	importFileDataArray = GetApp().SPI_GetImportFileManager().GetImportFileDataArrayConst();
	
	//R-PFM
	gImportIdentifierLinkListFind_FindCount++;
	AItemCount	continuous = 0;
	
	//ハッシュ値計算
	unsigned long	hash = GetHashValue(inKeywordText);
	AIndex	hashstartpos = hash%(/*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount());
	
	//高速化のため、ポインタ使用
	//#423 AStArrayPtr<AImportFileData*>	arrayptr1(mHash_ImportFileData,0,mHash_ImportFileData.GetItemCount());
	AStArrayPtr<AObjectID>			arrayptr1(mHash_ImportFileDataID,0,mHash_ImportFileDataID.GetItemCount());//#423
	AStArrayPtr<AIndex>				arrayptr2(mHash_IdentifierIndex,0,mHash_IdentifierIndex.GetItemCount());
	//#423 AImportFileData**	p1 = arrayptr1.GetPtr();
	AObjectID*			p1 = arrayptr1.GetPtr();
	AIndex*				p2 = arrayptr2.GetPtr();
	//登録箇所検索
	//p1[]: ImportFileDataへのポインタ  p2[]: ImportFileData内のIndex、および、Hash制御データ
	AIndex	hashpos = hashstartpos;
	while( true )
	{
		AIndex	dataIndex = p2[hashpos];
		if( dataIndex == kIndex_HashNoData )   break;
		if( dataIndex != kIndex_HashDeleted )
		{
			//R-PFM
			gImportIdentifierLinkListFind_CompareCount++;
			continuous++;
			if( continuous > gImportIdentifierLinkListFind_MaxContinuous )
			{
				gImportIdentifierLinkListFind_MaxContinuous = continuous;
			}
			//万一NULL（不具合）ならthrow
			if( p1[hashpos] == /*#423 NULL*/kObjectID_Invalid )   _ACThrow("",NULL);
			//キーワードと一致するならハッシュデータのインデックスを返す
			//#348 if( p1[hashpos]->GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex).Compare(inKeywordText) == true )
			//#423 if( p1[hashpos]->GetTextInfoConst().CompareKeywordTextByGlobalIdentifierIndex(dataIndex,inKeywordText) == true )//#348
			//#717if( GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(p1[hashpos]).GetTextInfoConst().
			if( importFileDataArray.GetObjectConstByID(p1[hashpos]).GetTextInfoConst().//#0 高速化
						CompareKeywordTextByGlobalIdentifierIndex(dataIndex,inKeywordText) == true )
			{
				//R-PFM
				gImportIdentifierLinkListFind_HitCount++;
				//
				return hashpos;
			}
		}
		hashpos = GetNextHashPos(hashpos);
		if( hashpos == hashstartpos )   break;
	}
	return kIndex_Invalid;
}

//一致する項目の全てのインデックスを取得
void	AImportIdentifierLinkList::Find( const AText& inKeywordText, AArray<AIndex>& outIndexArray ) const
{
	outIndexArray.DeleteAll();
	
	if( /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount() == 0 )   return;
	
	//ハッシュ値計算
	unsigned long	hash = GetHashValue(inKeywordText);
	AIndex	hashstartpos = hash%(/*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount());
	
	//高速化のため、ポインタ使用
	//#423 AStArrayPtr<AImportFileData*>	arrayptr1(mHash_ImportFileData,0,mHash_ImportFileData.GetItemCount());
	AStArrayPtr<AObjectID>			arrayptr1(mHash_ImportFileDataID,0,mHash_ImportFileDataID.GetItemCount());//#423
	AStArrayPtr<AIndex>				arrayptr2(mHash_IdentifierIndex,0,mHash_IdentifierIndex.GetItemCount());
	//#423 AImportFileData**	p1 = arrayptr1.GetPtr();
	AObjectID*			p1 = arrayptr1.GetPtr();
	AIndex*				p2 = arrayptr2.GetPtr();
	//登録箇所検索
	//p1[]: ImportFileDataへのポインタ  p2[]: ImportFileData内のIndex、および、Hash制御データ
	AIndex	hashpos = hashstartpos;
	while( true )
	{
		AIndex	dataIndex = p2[hashpos];
		if( dataIndex == kIndex_HashNoData )   break;
		if( dataIndex != kIndex_HashDeleted )
		{
			//万一NULL（不具合）ならthrow
			if( p1[hashpos] == /*#423 NULL*/kObjectID_Invalid )   _ACThrow("",NULL);
			//キーワードと一致するならハッシュデータのインデックスを追加する
			//#348 if( p1[hashpos]->GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex).Compare(inKeywordText) == true )
			//#423 if( p1[hashpos]->GetTextInfoConst().CompareKeywordTextByGlobalIdentifierIndex(dataIndex,inKeywordText) == true )
			if( GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(p1[hashpos]).GetTextInfoConst().
						CompareKeywordTextByGlobalIdentifierIndex(dataIndex,inKeywordText) == true )
			{
				outIndexArray.Add(hashpos);
			}
		}
		hashpos = GetNextHashPos(hashpos);
		if( hashpos == hashstartpos )   break;
	}
}


