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

AModePref

*/

#pragma once

#include "../../AbsFramework/AbsBase/ABase.h"
#include "ATypes.h"
#include "ASyntaxDefinition.h"
#include "AIdentifierList.h"
#include "AImportIdentifierLink.h"
#if IMPLEMENTATION_FOR_MACOSX
#include "AMacLegacyPref.h"
#endif
#include "AColorSchemeDB.h"

/*
モードデータまとめ

＜ファイルに対して読み書きするもの＞
・DB
・ツール
・ツールバー
・旧keywords.txtデータ

＜ファイルから読み出しのみするもの、キャッシュ的なデータ＞
・システムヘッダ mSystemHeaderFileArray, mSystemHeaderIdentifierLinkList
・キーワード構成文字 mIsAlphabet, mIsContinuousAlphabet, mIsTailAlphabet
・文法定義
・キーワード（文法定義内キーワード＋設定キーワード）
・正規表現キーワード
・ユーザー設定キーワード
・キーワード説明
・文法定義state色
・拡張子

*/

#pragma mark ===========================
#pragma mark [クラス]AModePrefDB
//モード設定データ
class AModePrefDB : public ADataBase
{
	//コンストラクタ／デストラクタ
  public:
	AModePrefDB( AObjectArrayItem* inParent );
	~AModePrefDB();
	
	//基本データ・取得処理
	//（mi本体、modeprefconverter両方で使用する）
  private:
	AModeID	mModeID;
	//#920 AFileAcc	mModeFile;
	AFileAcc								mModeFolder;//#920
  public:
	void						GetModeRawName( AText& outName ) const;//#427
	void						GetModeFolder( AFileAcc& outFolder ) const;
	void						GetDataFolder( AFileAcc& outFolder ) const;//#353
	
	//
	static void	LoadLegacyKeywordTextIntoEmptyCatgory( const AText& inDataText, AModePrefDB& ioDB );//#427
  private:
	//旧キーワードデータ読み込み用 #427
	//MacLegacy読み込みのソースコードを変更しないために、このデータを用意した。MergeLegacyKeywordData()にてマージする。
	AObjectArray<ATextArray>				mLegacyKeywordTempDataArray;
	
#ifndef MODEPREFCONVERTER
	//modeprefconverterでは使用しない箇所 #1034
	//多言語名称 #305
  public:
	void						GetModeMultiLanguageName( const ALanguage inLanguage, AText& outName ) const;
	void						SetModeMultiLanguageName( const ALanguage inLanguage, const AText& inName );
	
	//初期化
  public:
	void						InitMode( const AModeIndex& inModeID, const AFileAcc& inModeFolder );//#920
  private:
	/*#844 
	void						MergeAutoUpdate_JumpSetup( AModePrefDB& inAutoUpdateDB, 
								const ABool inAutoUpdateUserDataDBExist, const AModePrefDB& inAutoUpdateUserDataDB );//#427 #624
								*/
	void						MergeAutoUpdate_KeywordCategory( AModePrefDB& inAutoUpdateDB,
								const ABool inAutoUpdateUserDataDBExist, const AModePrefDB& inAutoUpdateUserDataDB );//#427 #624
	
	//ロード／保存
  public:
	void						LoadOrWaitLoadComplete( const ABool inInitialLoad = false );//R0000
	void						LoadDB();//R0000 #852
	ABool						IsLoaded();
	void						DeleteAllModeData();//R0000
	void	SaveToFile();
	//#350 void	WriteAdditionalKeywords();
	//#844 void	MakeUserKeywordDataText( AText& outDataText ) const;//#350
	//#350 void	LoadAdditionalKeywords();
	void	UpdateBackgroundImage();
	void	UpdateSyntaxDefinition();
	//#350void	GetXMLFile( AText& outText ) const;
	//#427 void	SaveModeBackup();//R0000
	//#844 ABool	RevertToLaunchBackup();//R0000
	void						ModeBackup( const ABool inReveal );
	void						CreateModeUpdateDistribution( const AFileAcc& inFolder );//#427
	ABool						IsModeEnabled() const;//#914
  private:
	void						SaveSuffixDB();//#852
	void						MergeLegacyKeywordData();//#427
	ABool									mDBLoaded;//#427 #852
	AThreadMutex							mLoadMutex;//#890
	
	//自動更新 #427
  public:
	void						AutoUpdate( const AFileAcc& inUpdateModeFolder );
	void						SetForceAutoUpdate( const ABool inForceAutoUpdate ) { mForceAutoUpdate = inForceAutoUpdate; }
	ABool						GetForceAutoUpdate() const { return mForceAutoUpdate; }
	void						AutoUpdate_SetLoadingInfo( const ANumber64bit inRevision, const AText& inName ) 
	{
		mLoadingRevision = inRevision; 
		mLoadingModeName.SetText(inName);
	}
	void						AutoUpdate_GetLoadingModeName( AText& outName )
	{
		outName.SetText(mLoadingModeName);
	}
  private:
	void						CopyToAutoUpdateFolder( const AFileAcc& inSrcModeFolder );
	ABool								mForceAutoUpdate;
	ANumber64bit						mLoadingRevision;
	AText								mLoadingModeName;
	
  private:
	ABool	mLoaded;
	
	//データ取得オーバーライド #868
  public:
	ABool						GetModeData_Bool( ADataID inID ) const;
	void						GetModeData_Text( ADataID inID, AText& outText ) const;
	ANumber						GetModeData_Number( ADataID inID ) const;
	AFloatNumber				GetModeData_FloatNumber( ADataID inID ) const;
	void						GetModeData_Color( ADataID inID, AColor& outData ) const;
  private:
	ABool						IsSameAsNormal( const ADataID inID ) const;
	
	//#889
  public:
	void						ApplyFromColorScheme();
	
	//システムヘッダ #253
  public:
	void						UpdateSystemHeaderFileData();
	void						DoImportFileRecognized();
	ABool						DeleteImportFileData( const AObjectID inImportFileDataObjectID, const AFileAcc& inFile );
	void						DeleteAllImportFileData();//#253
	const AImportIdentifierLinkList&	GetSystemHeaderIdentifierLinkList() const 
	{ return mSystemHeaderIdentifierLinkList; }
  private:
	AObjectArray< AFileAcc >			mSystemHeaderFileArray;
	AHashTextArray						mSystemHeaderFileArray_NormalizedPath;
	AImportIdentifierLinkList			mSystemHeaderIdentifierLinkList;
	/** @note 本スレッド：書き込み時のみ排他 サブスレッド：読み込み時に排他（書き込みは行わない） */
	mutable AThreadMutex				mSystemHeaderIdentifierLinkListMutex;//#717
	
	//プレビュープラグイン
  public:
	ABool						GetPreviewPlugin( AText& outPluginText ) const;
  private:
	void						LoadPreviewPlugin();
	AText								mPreviewPluginText;
	ABool								mPreviewPluginExist;
	
	//プラグイン #994
  public:
	void						RegisterPlugin( const AText& inPluginID, const AText& inPluginName, 
												const ABool inDefaultEnabled, const AText& inDefaultOptions );
	void						SetPluginDisplayName( const AText& inPluginID, const AText& inLangName, const AText& inDisplayName );
	ABool						GetPluginDisplayName( const AText& inPluginID, AText& outDisplayName ) const;
	ABool						GetPluginEnabled( const AText& inPluginID ) const;
	void						GetPluginOptions( const AText& inPluginID, AText& outOptions ) const;
	void						LoadPlugins();
	
	//キーワード構成文字
	//SDFを使う場合は常にSDFに定義されたアルファベット定義を使う。
	//SDFを使わない場合はモード設定で設定したアルファベット定義を使う
  public:
	void	UpdateIsAlphabetTable();
	ABool	IsAlphabet( const AUChar inChar, const AIndex inSyntaxPartIndex ) const
	{
		if( mSDFIsSimpleDefinition == true )
		{
			return mIsAlphabet[inChar];
		}
		else
		{
			return GetSyntaxDefinitionConst().IsStartAlphabet(inChar,inSyntaxPartIndex) || 
			GetSyntaxDefinitionConst().IsContinuousAlphabet(inChar,inSyntaxPartIndex) ||
			GetSyntaxDefinitionConst().IsTailAlphabet(inChar,inSyntaxPartIndex);//#1265 IsStartAlphabet()だけになっていたので、IsContinuousAlphabet()、IsTailAlphabet()もつけた
		}
	}
	ABool	IsContinuousAlphabet( const AUChar inChar, const AIndex inSyntaxPartIndex ) const
	{
		if( mSDFIsSimpleDefinition == true )
		{
			return mIsContinuousAlphabet[inChar];
		}
		else
		{
			return GetSyntaxDefinitionConst().IsContinuousAlphabet(inChar,inSyntaxPartIndex);
		}
	}
	ABool	IsTailAlphabet( const AUChar inChar, const AIndex inSyntaxPartIndex ) const
	{
		if( mSDFIsSimpleDefinition == true )
		{
			return mIsTailAlphabet[inChar];
		}
		else
		{
			return GetSyntaxDefinitionConst().IsTailAlphabet(inChar,inSyntaxPartIndex);
		}
	}
  private:
	ABool	mIsAlphabet[256];
	ABool	mIsContinuousAlphabet[256];
	ABool	mIsTailAlphabet[256];
	
	
	//キーワード情報検索 #853
  public:
	ABool						SPI_SearchKeywordSystemHeader( const AText& inWord, const AHashTextArray& inParentWord, const AKeywordSearchOption inOption,
															  const AIndex inCurrentStateIndex,
															  AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
															  ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
															  ATextArray& outCommentTextArray,
															  ATextArray& outCompletionTextArray, ATextArray& outURLArray,
															  AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
															  AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
															  AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray,
															  AArray<AItemCount>& outMatchedCountArray,
															  const ABool& inAbort  ) const;
	ABool						SPI_SearchKeywordKeywordList( const AText& inWord, const AHashTextArray& inParentWord, const AKeywordSearchOption inOption,
															 const AIndex inCurrentStateIndex,
															 AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
															 ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
															 ATextArray& outCommentTextArray,
															 ATextArray& outCompletionTextArray, ATextArray& outURLArray,
															 AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
															 AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
															 AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray,
															 AArray<AItemCount>& outMatchedCountArray,
															 const ABool& inAbort  ) const;
	void						FindKeywordIdInfo( const AText& inKeyword, 
								ATextArray& outExplanationArray, AArray<AIndex>& outCategoryIndexArray, ATextArray& outParentKeywordArray ) const	;
	//ツール
  public:
	//基本制御
	void						LoadTool();
	AIndex						AddToolItem( const AObjectID inMenuObjectID, const AFileAcc& inFile, 
								const ABool inWriteOrderFile, const ABool inUpdateMenu, const ABool inAutoUpdate );//B0321
	void						InsertToolItem( const AObjectID inMenuObjectID, const AIndex inItemIndex, const AFileAcc& inFile, 
								const ABool inWriteOrderFile, const ABool inUpdateMenu, const ABool inAutoUpdate );
	void						InsertStaticTextToolItem( const AObjectID inMenuObjectID, const AIndex inItemIndex, 
								const AText& inStaticText,
								const ABool inWriteOrderFile, const ABool inUpdateMenu, const ABool inAutoUpdate );//#427
	void						DeleteToolItem( const AObjectID inMenuObjectID, const AIndex inItemIndex,
								const ABool inWriteOrderFile, const ABool inUpdateMenu );
	void						MoveToolItem( const AObjectID inMenuObjectID, const AIndex inOldIndex, const AIndex inNewIndex );//R0173
	AItemCount					GetToolItemCount( const AObjectID inMenuObjectID ) const;
	ABool						FindToolIndexByFile( const AFileAcc& inFile, AIndex& outArrayIndex, AIndex& outItemIndex ) const;//B0406
	ABool						GetToolMenuObjectIDFromFile( const AFileAcc& inFile, 
													 AObjectID& outMenuObjectID, AArray<AObjectID>& outMenuObjectIDArray ) const;
	//ABool						FindToolArrayIndexByFile( const AFileAcc inFolder, AIndex& outArrayIndex ) const;//#427
	void						UpdateToolMenu( const AObjectID inMenuObjectID );//R0173
	AIndex						GetToolMenuItemIndex( const AObjectID inMenuObjectID, const AIndex inToolItemIndex ) const;//R0173
	//メニューへのAttach/Detach
	void						AttachToolMenu();
	void						DetachToolMenu();
	//設定
	void						DeleteToolItemAndFile( const AObjectID inMenuObjectID, const AIndex inItemIndex );
	ABool						SetToolName( const AFileAcc& inFile, const AText& inNewName, AFileAcc& outNewFile );
	void						SetToolShortcut( const AFileAcc& inFile, const ANumber inShortcut, 
								const AMenuShortcutModifierKeys inModifiers );//B0406
	void						SetEnableToolItem( const AObjectID inMenuObjectID, const AIndex inItemIndex, const ABool inEnabled );//R0173
	void						SetGrayoutToolItem( const AObjectID inMenuObjectID, const AIndex inItemIndex, const ABool inGrayout );//#129
	void						SetToolMultiLanguageName( const AObjectID inMenuObjectID, const AIndex inItemIndex, 
								const ALanguage inLanguage, const AText& inName );//#305
	//取得
	void						GetToolFile( const AObjectID inMenuObjectID, const AIndex inItemIndex, AFileAcc& outFile ) const;//B0406
	void						GetToolShortcut( const AObjectID inMenuObjectID, const AIndex inItemIndex, 
								ANumber& outShortcut, AMenuShortcutModifierKeys& outModifiers ) const;//B0406 #427
	AObjectID					GetToolSubMenuID( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const;//R0173
	AObjectID					GetToolMenuRootObjectID() const { return mToolMenuRootObjectID; }
	ABool						IsToolEnabled( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const;//R0173
	ABool						IsToolGrayout( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const;//#129
	void						GetToolMultiLanguageName( const AObjectID inMenuObjectID, const AIndex inItemIndex, 
								const ALanguage inLanguage, AText& outName ) const;//#305
	void						GetToolDisplayName( const AObjectID inMenuObjectID, const AIndex inItemIndex, AText& outName ) const;//#305
	ABool						GetToolAutoUpdateFlag( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const;//#427
	ABool						GetToolIsStaticText( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const;//#427
	ABool						GetToolFolderIsAutoUpdate( const AObjectID inMenuObjectID ) const;//#427
	//orderファイル
	void						WriteToolOrderFile( const AObjectID inMenuObjectID );
	//Mac OS X
	void						LoadTool_LegacyShortcutData( const AFileAcc& inToolFile, 
								ANumber& outShortcutKey, ABool& outShortcutShiftKey, ABool& outShortcutControlKey, 
								ABool& outShortcutOptionKey );
	/*#844
	void						WriteTool_LegacyShortcutData( const AFileAcc& inToolFile, 
															 const ANumber inShortcutKey, const ABool inShortcutShiftKey, const ABool inShortcutControlKey );
															 */
	AIndex						GetToolMenuRootAppToolStartIndex() const { return mToolMenuRoot_AppToolStartIndex; }
	AIndex						GetToolMenuRootAppToolEndIndex() const { return mToolMenuRoot_AppToolEndIndex; }
	AIndex						GetToolMenuRootUserToolStartIndex() const { return mToolMenuRoot_UserToolStartIndex; }
	AIndex						GetToolMenuRootUserToolEndIndex() const { return mToolMenuRoot_UserToolEndIndex; }
	AIndex						GetUserToolInsertIndex( const AObjectID inMenuObjectID ) const;
	void						GetKeyToolStartMenu( const AIndex inSyntaxPartIndex, const ABool inUserTool,
													AObjectID& outMenuObjectID, 
													AIndex& outStartIndex, AIndex& outEndIndex ) const;
	//
	void						GetToolFolderFromMenuObjectID( const AObjectID inMenuObjectID, AFileAcc& outToolFolder )
	{
		AIndex	index = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
		outToolFolder = mToolMenuArray_Folder.GetObjectConst(index);
	}
  private:
	void						WriteToolOrderFile( const AIndex arrayIndex, const ABool autoUpdate );//#0 
	void						LoadToolRecursive( const AFileAcc& inFolder, const AObjectID inDynamicMenuObjectID,
								const ABool inAutoUpdate );//#427
	void						GetToolPrefFile( const AIndex inToolMenuArrayIndex, const ABool inAutoUpdate, AFileAcc& outToolPrefFile ) const;//#427
	AObjectID								mToolMenuRootObjectID;
	AArray<AObjectID>						mToolMenuArray_MenuObjectID;//R0173
	AObjectArray< AFileAcc >				mToolMenuArray_Folder;//#305
	AArray<ABool>							mToolMenuArray_AutoUpdateFolder;//#427
	AObjectArray< AObjectArray<AFileAcc> >	mToolItemArray_File;//R0173
	AObjectArray< AArray<AObjectID> >		mToolItemArray_SubMenuObjectID;//R0173
	AObjectArray< AArray<ABool> >			mToolItemArray_Enabled;//R0173
	AObjectArray< AArray<ANumber> >			mToolItemArray_Shortcut;//R0173
	AObjectArray< AArray<AMenuShortcutModifierKeys> >	mToolItemArray_Modifiers;//R0173
	AObjectArray< AArray<ABool> >			mToolItemArray_Grayout;//#129
	AObjectArray< AArray<ABool> >			mToolItemArray_AutoUpdate;//#427
	AObjectArray< ATextArray >				mToolItemArray_StaticText;//#427
	AIndex									mToolMenuRoot_AppToolStartIndex;//#427
	AIndex									mToolMenuRoot_AppToolEndIndex;//#427
	AIndex									mToolMenuRoot_UserToolStartIndex;//#427
	AIndex									mToolMenuRoot_UserToolEndIndex;//#427
	//#305
	AObjectArray< ATextArray >				mToolItemArray_JapaneseName;
	AObjectArray< ATextArray >				mToolItemArray_EnglishName;
	AObjectArray< ATextArray >				mToolItemArray_FrenchName;
	AObjectArray< ATextArray >				mToolItemArray_GermanName;
	AObjectArray< ATextArray >				mToolItemArray_SpanishName;
	AObjectArray< ATextArray >				mToolItemArray_ItalianName;
	AObjectArray< ATextArray >				mToolItemArray_DutchName;
	AObjectArray< ATextArray >				mToolItemArray_SwedishName;
	AObjectArray< ATextArray >				mToolItemArray_NorwegianName;
	AObjectArray< ATextArray >				mToolItemArray_DanishName;
	AObjectArray< ATextArray >				mToolItemArray_FinnishName;
	AObjectArray< ATextArray >				mToolItemArray_PortugueseName;
	AObjectArray< ATextArray >				mToolItemArray_SimplifiedChineseName;
	AObjectArray< ATextArray >				mToolItemArray_TraditionalChineseName;
	AObjectArray< ATextArray >				mToolItemArray_KoreanName;
	AObjectArray< ATextArray >				mToolItemArray_PolishName;
	AObjectArray< ATextArray >				mToolItemArray_PortuguesePortugalName;
	AObjectArray< ATextArray >				mToolItemArray_RussianName;
	
	//ツールバー
  public:
	void						LoadToolbar();
	//#724 AIconID						GetToolbarIconFromToolContent( const AFileAcc& inFile ) const;//#232
	AItemCount					GetToolbarItemCount() const;
	void						GetToolbarItem( const AIndex inIndex, 
								AToolbarItemType& outType, AFileAcc& outFile, AText& outName, ABool& outEnabled ) const;
	ATextArray&					GetToolbarItemFilenameArray( const AIndex inIndex );
	AIndex						AddToolbarItem( const AToolbarItemType inType, const AFileAcc& inFile, const ABool inAutoUpdate );//B0321 #427
	void						InsertToolbarItem( const AIndex inItemIndex, const AToolbarItemType inType, const AFileAcc& inFile, const ABool inAutoUpdate );//#427
	void						DeleteToolbarItem( const AIndex inIndex );
	void						WriteToolbarItemOrderFile();
	void						MoveToolbarItem( const AIndex inOldIndex, const AIndex inNewIndex );
	//B0406 ABool	RenameToolbarItem( const AIndex inIndex, const AText& inNewName );
	ABool						SetToolbarName( const AFileAcc& inFile, const AText& inNewName, AFileAcc& outNewFile );
	void						SetEnableToolbarItem( const AIndex inIndex, const ABool inEnable );
	void						SetEnableToolbarItemAll();//#232
	void						SetEnableToolbarItemAll( const ABool inEnable/*#844 , const ABool inAutoUpdateItems, const ABool inUserItems*/ );//#619
	ABool						FindToolbarIndexByFile( const AFileAcc& inFile, AIndex& outItemIndex ) const;
	void						GetToolbarMultiLanguageName( const AIndex inItemIndex, const ALanguage inLanguage, AText& outName ) const;//#305
	void						SetToolbarMultiLanguageName( const AIndex inItemIndex, const ALanguage inLanguage, const AText& inName );//#305
	void						GetToolbarDisplayName( const AIndex inItemIndex, AText& outName ) const;//#305
	ABool						GetToolbarAutoUpdateFlag( const AIndex inItemIndex );//#427
	void						GetUniqToolbarName( AText& ioName ) const;//#724
	void						CreateNewToolbar( const AText& inToolName, const AText& inToolText );
	void						GetHiddenMacroBarItemNameArray( ATextArray& outNameArray ) const;
  private:
	AToolbarItemType			GetToolbarTypeFromFile( const AFileAcc& inFile );//#427
	void						GetFilenameArray( const AFileAcc& inFolder, ATextArray& outTextArray );
	void						GetToolbarPrefFile( /*#844 const ABool inAutoUpdate,*/ AFileAcc& outToolPrefFile ) const;//#427
	AArray<AToolbarItemType>				mToolbarItem_Type;
	AObjectArray<AFileAcc>					mToolbarItem_File;
	//#724 AArray<AIconID>							mToolbarItem_IconID;
	AArray<ABool>							mToolbarItem_Enabled;
	AObjectArray<ATextArray>				mToolbarItem_FilenameArray;
	//#844 AArray<ABool>							mToolbarItem_AutoUpdate;//#427
	//#305
	ATextArray								mToolbarItem_JapaneseName;
	ATextArray								mToolbarItem_EnglishName;
	ATextArray								mToolbarItem_FrenchName;
	ATextArray								mToolbarItem_GermanName;
	ATextArray								mToolbarItem_SpanishName;
	ATextArray								mToolbarItem_ItalianName;
	ATextArray								mToolbarItem_DutchName;
	ATextArray								mToolbarItem_SwedishName;
	ATextArray								mToolbarItem_NorwegianName;
	ATextArray								mToolbarItem_DanishName;
	ATextArray								mToolbarItem_FinnishName;
	ATextArray								mToolbarItem_PortugueseName;
	ATextArray								mToolbarItem_SimplifiedChineseName;
	ATextArray								mToolbarItem_TraditionalChineseName;
	ATextArray								mToolbarItem_KoreanName;
	ATextArray								mToolbarItem_PolishName;
	ATextArray								mToolbarItem_PortuguesePortugalName;
	ATextArray								mToolbarItem_RussianName;
	
	//文法定義
  public:
	ASyntaxDefinition&	GetSyntaxDefinition() {return mSyntaxDefinition;}
	const ASyntaxDefinition&	GetSyntaxDefinitionConst() const {return mSyntaxDefinition;}
  private:
	ASyntaxDefinition	mSyntaxDefinition;
	ABool									mSDFIsSimpleDefinition;//#907
	
	//キーワード／カテゴリー
  public:
	ABool						FindKeyword( const AText& inKeyword, AColor& outColor, ATextStyle& outStyle, ABool& outPriorToOtherColor, const AIndex inStateIndex );//R0195 R0000
	void						GetCategoryColor( const AIndex inCategoryIndex, AColor& outColor ) const;
	ATextStyle					GetCategoryTextStyle( const AIndex inCategoryIndex ) const;
	void						GetCategoryImportColor( const AIndex inCategoryIndex, AColor& outColor ) const;
	void						GetCategoryLocalColor( const AIndex inCategoryIndex, AColor& outColor ) const;
	void						GetCategoryName( const AIndex inCategoryIndex, AText& outName ) const;
	ATextStyle					GetCategoryMenuTextStyle( const AIndex inCategoryIndex ) const;
	ABool						GetCategoryPriorToOtherColor( const AIndex inCategoryIndex ) const;//R0195
	AItemCount					GetCategoryCount() const;//#890
	/*#717
	void						GetAbbrevCandidateFromKeywordList( const AText& inText,
								ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
								AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray ) const;//RC2
	*/
	/*#427
	AObjectID					GetAdditionalKeywordCategoryObjectIDByIndex( const AIndex inIndex ) const;//B0406
	AIndex						GetAdditionalKeywordCategoryIndexByObjectID( const AObjectID inObjectID ) const;//B0406
	void						GetAdditionalKeywordForModePrefWindow( 
								const AObjectID inCateogryObjectID, AText& outText ) const;
	void						SetAdditionalKeywordFromModePrefWindow( 
								const AObjectID inCateogryObjectID, const AText& inText );
	*/
	void						UpdateUserKeywordCategoryAndKeyword();
	void						AddNormalKeywordCategoryAndKeyword();
	void						GetUniqCategoryName( const AText& inName, AText& outName ) const;
	void						StartAddCSVKeywordThread( const AText& inResultText, const AIndex inCategoryIndex );
	void						AddCategoryKeywordListFromCSV( const AText& inResultText, const AIndex inCategoryIndex, ABool& inAbortFlag );//#791
	ABool						GetCategoryDisabled( const AIndex inStateIndex, const AIndex inCategoryIndex ) const;//#877
	void						UpdateSyntaxDefinitionCategoryColor();//#844
	void						UpdateUserKeywordCategoryColor();//#889
	ABool						FindKeywordTypeFromKeywordList( const AText& inKeyword, AText& outType )
	{
		AStMutexLocker	locker(mKeywordListMutex);
		ABoolArray	tmpArray;
		return mKeywordList.GetTypeTextByKeywordText(inKeyword,true,tmpArray,outType);
	}
	void						GetColorSlotData( const AIndex inSlotIndex, AColor& outColor, ATextStyle& outTextStyle ) const;
	void						GetKeywordCSVFileFromPath( const AText& inPath, AFileAcc& outFile ) const;
	AItemCount					GetKeywordListItemCount() const { return mKeywordList.GetItemCount(); }//#1160
  private:
	void						MakeCategoryArrayAndKeywordList();
	AIdentifierList							mKeywordList;
	/** @note 本スレッド：書き込み時のみ排他 サブスレッド：読み込み時に排他（書き込みは行わない） */
	mutable AThreadMutex					mKeywordListMutex;//#717
	//
	ATextArray								mCategoryArray_Name;
	AArray<AColor>							mCategoryArray_Color;
	AArray<AColor>							mCategoryArray_ImportColor;
	AArray<AColor>							mCategoryArray_LocalColor;
	AArray<ATextStyle>						mCategoryArray_TextStyle;
	AArray<ATextStyle>						mCategoryArray_MenuTextStyle;
	AArray<ABool>							mCategoryArray_PriorToOtherColor;//R0195
	//R0000 カテゴリー可否 ABoolArrayの配列要素はcategory毎、AObjectArrayの配列要素はstate毎
	ABool									mCategoryDisabledStateArrayAllFalse;
	AObjectArray<ABoolArray>				mCategoryDisabledStateArray;
	mutable AThreadMutex					mCategoryDisabledStateArrayMutex;
	AIndex									mAdditionalCategoryStartIndex;
	//AArray<AUniqID>							mUserKeywordIdentifierUniqIDArray;
	static const AIndex							kKeywordColorSlotIndexMin = 0;
	static const AIndex							kKeywordColorSlotIndexMax = 7;
	
  public:
	void						GetKeywordRegExp( AObjectArray<ARegExp>& outRegExpArray, AArray<AIndex>& outCategoryIndexArray ) const;
  private:
	ATextArray								mKeywordRegExp_Text;
	AArray<AIndex>							mKeywordRegExp_CategoryIndex;
	
	/*#427 カスタムキーワードデータはmodepref.mi内に入れる
	//ユーザー設定キーワード
  public:
	AObjectArray<ATextArray>&				GetAdditionalCategoryKeywordArray() { return mAdditionalCategoryArray_KeywordArray; }
  private:
	AObjectArray<ATextArray>				mAdditionalCategoryArray_KeywordArray;
	*/
	
	//#147 キーワード説明テキスト
	/*
  public:
	void						FindKeywordExplanation( const AText& inKeyword, 
	ATextArray& outExplanationArray, AArray<AIndex>& outCategoryIndexArray ) const;
	*/
  private:
	void						LoadKeywordsExplanation();//#427
	void						ParseKeywordExplanationText( const AText& inKeywordExplanationText );
	AHashTextArray							mKeywordExplanationTextArray_Keyword;
	ATextArray								mKeywordExplanationTextArray_CategoryName;
	ATextArray								mKeywordExplanationTextArray_Explanation;
	
	//
  public:
	void						GetSyntaxDefinitionCategoryColorByName( const AText& inName, 
								AColor& outColor, AColor& outImportColor, AColor& outLocalColor, ATextStyle& outMenuTextStyle,
								ATextStyle& outTextStyle ) const;//R0195 #844
	void						GetLetterColorForState( const AIndex inStateIndex, AColor& outColor, ATextStyle& outStyle, ABool& outStateColorValid ) const;
	void						UpdateSyntaxDefinitionStateColorArray();
  private:
	//AIndex						FindOrAddSyntaxDefinitionCategoryData( const AText& inName );//B0000
	AArray<AColor>							mSyntaxDefinitionStateColorArray;
	AArray<ABool>							mSyntaxDefinitionStateColorValidArray;
	AArray<ATextStyle>						mSyntaxDefinitionStateArray_TextStyle;//#844
	
	//stateのカラースロットを取得
  public:
	AIndex						GetColorSlotIndexByStateIndex( const AIndex inStateIndex ) const
	{ return mSyntaxDefinition.GetSyntaxDefinitionStateConst(inStateIndex).GetColorSlot(); }
	
	//拡張子
  public:
	void						UpdateSuffixHash();
	ABool						IsThisModeSuffix( const AText& inFilename ) const;
	ABool						IsThisModeSelectFilePath( const AText& inFilePath ) const;//R0210
	const AHashTextArray&		GetSuffixArray() const {return mSuffixHashArray;}//win 080714
  private:
	AHashTextArray							mSuffixHashArray;
	mutable AThreadMutex					mSuffixHashArrayMutex;
	
  public:
	void	GetOpenSelectedFileCandidate( const AText& inFilePathText, AObjectArray<AFileAcc>& outCandidate ) const;
	
	//キーワードファイル(.import)で指定したヘッダファイルフォルダの格納する #1065
  private:
	AText									mImportFilePathsByKeywordCategory;
	
	//インポート・エクスポート
  public:
	void						ImportDragDropFromText( const AText& inText );
	void						ExportDragDropToText( AText& outText ) const;
	
	void						ImportCorrespond( const AText& inText );
	void						ExportCorrespond( AText& outText ) const;
	
	void						ImportKeyBindFromText( const AText& inText, const ABool inAdditional );//B0000
	void						ExportKeyBindToText( AText& outText ) const;
	
	void						ImportRegExpIndent( const AText& inText );
	void						ExportRegExpIndent( AText& outText ) const;
	
	//#844 void						ImportKeywords( const AText& inText );//R0068
	//#844 void						ExportKeywords( AText& outText );//R0068
	//#844 void						NormalizeAdditionalCategoryName();//R0068
	
	void	GetData_ToolNameArray( AText& inFolderPath, ANumber inNumber, ATextArray& outNameArray );
	
	//ATextArray*	GetData_TextArrayRef( APrefID inID );
	//AArray<ANumber>*	GetData_NumberArrayRef( APrefID inID );
	//ATextArray*	GetSuffixArrayRef();
	
	void						GetModeDisplayName( AText& outName ) const;
	void	GetModeFile( AFileAcc& outFileAcc ) const;
	
	ABool						ChangeModeFileName( const AText& inName );//#427
	
	//キーバインド
  public:
	void						GetData_KeyBindAction( 
								const AKeyBindKey inKey, const AModifierKeys inModiferKeys,
								AKeyBindAction& outAction, AText& outText,
													  ABool& outDefault, ABool& outBindAvailable ) const;//#382
	/*#481
	void						GetData_KeyBindAction( 
								const AKeyBindKey inKey, const AModifierKeys inModiferKeys,
								AKeyBindAction& outAction, AText& outText );//#382
								*/
	ABool						SetData_KeybindAction( 
								const AKeyBindKey& inKey, const AModifierKeys inModiferKeys, 
													  const AKeyBindAction& inAction, const AText& inText );
	ABool	                    Remove_KeybindAction( const AKeyBindKey& inKey, const AModifierKeys inModiferKeys );
	ABool						FindKeyFromKeybindAction( const AKeyBindAction inAction, AKeyBindKey& outKey, AModifierKeys& outModifierKeys ) const;
	ABool						GetKeyTextFromKeybindAction( const AKeyBindAction inAction, AText& outKeyText ) const;
	/*#481
  private:
	void						RemoveKeyBindsSameAsDefault();
	*/
	
	//多言語 #427
  public:
	void						RemoveMultiLanguageModeNames();
	void						SetNormalModeNames();
	
	//各種フォルダ取得
  public:
	void						GetPluginsFolder( const ABool inAutoUpdate, AFileAcc& outFolder ) const;//#994
	void						GetToolFolder( const ABool inAutoUpdate, AFileAcc& outFolder ) const;//#427
	void						GetToolbarFolder( /*#844 const ABool inAutoUpdate,*/ AFileAcc& outFolder ) const;//#427
	void						GetAutoUpdateFolder( AFileAcc& outFolder ) const;//#427
	void						GetAutoUpdateUserDataFolder( const AConstCstringPtr inChildPath, AFileAcc& outFolder ) const;//#427
	ABool						IsSystemHeaderFile( const AFileAcc& inFile ) const;//#467
	
  public:
	ABool	UnitTest( AText& ioText );
	
  private:
	void	LoadSuffix();
	
  public:
	AIndex	GetBackgroundImageIndex() const {return mBackgroundImageIndex;}
  private:
	AIndex	mBackgroundImageIndex;
	
	
	//ATextArray	mKeywordCategoryName;
	//ATextArray	mJumpSetupName;
	
	//Indent RegExp
  public:
	//void	UpdateIndentRegExp();
	//ARegExp&	GetIndentRegExp( const AIndex inIndex );
	void	SetIndentRegExp( AObjectArray<ARegExp>& outRegExpArray ) const;
  private:
	//AObjectArray<ARegExp>	mIndentRegExp;
	
	//見出し設定 #890
  public:
	AIndex						GetJumpSetupCategoryIndex( const AIndex inIndex ) const ;
	void						SetJumpSetupRegExp( AObjectArray<ARegExp>& outRegExpArray ) const;
	void						UpdateJumpSetup();
	ANumber						GetJumpSetup_DisplayMultiply( const AIndex inIndex ) const;//#890
	ABool						GetJumpSetup_ColorizeLine( const AIndex inIndex ) const;
	AColor						GetJumpSetup_LineColor( const AIndex inIndex ) const;
	ABool						GetJumpSetup_RegisterToMenu( const AIndex inIndex ) const;
	void						GetJumpSetup_MenuText( const AIndex inIndex, AText& outMenuText ) const;
	ABool						GetJumpSetup_RegisterAsKeyword( const AIndex inIndex ) const;
	ANumber						GetJumpSetup_KeywordGroup( const AIndex inIndex ) const;
	ABool						GetJumpSetup_ColorizeRegExpGroup( const AIndex inIndex ) const;
	ANumber						GetJumpSetup_ColorizeRegExpGroup_Number( const AIndex inIndex ) const;
	AColor						GetJumpSetup_ColorizeRegExpGroup_Color( const AIndex inIndex ) const;
	ANumber						GetJumpSetupOutlineLevel( const AIndex inIndex ) const;
	void						DeleteJumpSetupAll();
  private:
	AThreadMutex							mJumpSetupMutex;
	ATextArray								mJumpSetup_RegExpText;
	ATextArray								mJumpSetup_LineStartText;
	ANumberArray							mJumpSetup_DisplayMultiply;
	ABoolArray								mJumpSetup_ColorizeLine;
	AColorArray								mJumpSetup_LineColor;
	ABoolArray								mJumpSetup_RegisterToMenu;
	ATextArray								mJumpSetup_MenuText;
	ABoolArray								mJumpSetup_RegisterAsKeyword;
	ANumberArray							mJumpSetup_KeywordGroup;
	ABoolArray								mJumpSetup_ColorizeRegExpGroup;
	ANumberArray							mJumpSetup_ColorizeRegExpGroup_Number;
	AColorArray								mJumpSetup_ColorizeRegExpGroup_Color;
	ANumberArray							mJumpSetup_OutlineLevel;
	ABoolArray								mJumpSetup_SimplerSetting;
	ABoolArray								mJumpSetup_Enable;
	AIndex									mJumpSetupCategoryStartIndex;
	
	//color scheme #889
  public:
	void						UpdateColorScheme();
	void						SaveLastColors();
	void						ExportColors( const AFileAcc& inFile );
  private:
	AColorSchemeDB							mColorSchemeDB;
	
	//自動インデントDisable設定 #997
  public:
	ABool						GetDisableAutoIndent() const { return mSyntaxDefinition.GetDisableAutoIndent(); }
	
#else
	//modeprefconverterでのみ使用する箇所 #1034
  public:
	//#1034
	void	LoadMacLegacyAndSave( const AModeIndex& inModeID, const AFileAcc& inModeFolder );
	//Legacy
	
	void	LoadMacLegacy();
	void	LoadMacLegacy_Pref( short inResRefNum );
	void	LoadMacLegacy_GetPrefDefault( SMacLegacyModePref& outPref );
	void	LoadMacLegacy_DragDrop( short inResRefNum );
	void	LoadMacLegacy_Correspond( short inResRefNum );
	void	LoadMacLegacy_Indent( short inResRefNum );
	void	LoadMacLegacy_JumpSetup( short inResRefNum );
	void	LoadMacLegacy_KeyBind( short inResRefNum );
	void	LoadMacLegacy_ConvertPrefEndianToBig( SMacLegacyModePref& ioPref );
	void	LoadMacLegacy_ConvertPrefEndianToHost( SMacLegacyModePref& ioPref );
	void	LoadMacLegacy_Keyword( short inResRefNum, const AText& inText );
#endif
	
  public:
	const static APrefID	kToolList								= 102;
	const static APrefID	kToolbarList							= 103;
	//win const static APrefID	kDefaultFont							= 104;
	//#688 const static APrefID	kDefaultFont_compat						= 104;//win
	const static APrefID	kDefaultFontSize						= 105;
	const static APrefID	kDefaultTextEncoding					= 106;
	const static APrefID	kDefaultWrapMode						= 107;
	const static APrefID	kDefaultTabWidth						= 108;
	const static APrefID	kDefaultReturnCode						= 109;
	const static APrefID	kLetterColor					= 128;
	const static APrefID	kBackgroundColor				= 129;
	const static APrefID	kLineMargin						= 130;
	const static APrefID	kDisplayBackgroundImage			= 131;
	const static APrefID	kBackgroundImageFile			= 133;
	const static APrefID	kDisplayToolbar					= 135;
	const static APrefID	kDisplayJumpList				= 137;
	const static APrefID	kDisplayRuler					= 138;
	const static APrefID	kRulerBaseLetter				= 139;
	const static APrefID	kRulerScaleWithTabLetter		= 140;
	const static APrefID	kDisplayEachLineNumber			= 143;
	const static APrefID	kDisplayEachLineNumber_AsParagraph		= 144;
	//
	const static APrefID	kDisplayReturnCode				= 145;
	const static APrefID	kDisplayTabCode					= 146;
	const static APrefID	kDisplaySpaceCode				= 147;
	const static APrefID	kDisplayZenkakuSpaceCode		= 148;
	//
	const static APrefID	kSpecialCodeColor				= 149;
	const static APrefID	kDisplayGenkoyoshi				= 150;
	const static APrefID	kCheckBrace						= 151;
	const static APrefID	kSelectInsideByQuadClick		= 164;
	const static APrefID	kQuadClickText					= 153;
	const static APrefID	kInputSpacesByTabKey			= 155;
	const static APrefID	kAddRemoveSpaceByDragDrop		= 156;
	const static APrefID	kInputBackslashByYenKey			= 157;
	const static APrefID	kDontSaveResouceFork			= 158;
	const static APrefID	kSaveUTF8BOM					= 159;
	const static APrefID	kCheckCharsetWhenSave			= 160;
	const static APrefID	kSaveWindowPosition				= 162;
	const static APrefID	kDefaultWrapMode_LetterCount	= 163;
	const static APrefID	kBackgroundImageTransparency	= 165;
	const static APrefID	kSniffTextEncoding_BOM			= 166;
	const static APrefID	kSniffTextEncoding_Charset		= 167;
	const static APrefID	kSniffTextEncoding_Legacy		= 168;
	const static APrefID	kConvert5CToA5WhenOpenJIS		= 170;
	const static APrefID	kCommentStart					= 181;
	const static APrefID	kCommentEnd						= 182;
	const static APrefID	kCommentLineStart				= 183;
	const static APrefID	kUseRegExpCommentStart			= 184;
	const static APrefID	kLiteralStart1					= 186;
	const static APrefID	kLiteralEnd1					= 187;
	const static APrefID	kLiteralStart2					= 188;
	const static APrefID	kLiteralEnd2					= 189;
	const static APrefID	kEscapeString					= 191;
	const static APrefID	kOtherStart1					= 192;
	const static APrefID	kOtherEnd1						= 193;
	const static APrefID	kOtherStart2					= 194;
	const static APrefID	kOtherEnd2						= 195;
	const static APrefID	kCharsForWord					= 196;
	const static APrefID	kCharsForHeadOfWord				= 197;
	const static APrefID	kCharsForTailOfWord				= 198;
	const static APrefID	kApplyCharsForWordToDoubleClick	= 199;
	
	const static APrefID	kUseCIndent						= 243;
	const static APrefID	kCIndent_Normal					= 244;
	const static APrefID	kCIndent_Fukubun				= 245;
	const static APrefID	kCIndent_Label					= 246;
	const static APrefID	kUseCSyntaxColor				= 247;
	const static APrefID	kCSyntaxColor_Function			= 248;
	const static APrefID	kCSyntaxColor_ExternalFunction	= 249;
	const static APrefID	kCSyntaxColor_Class				= 250;
	const static APrefID	kCSyntaxColor_GlobalVariable	= 251;
	const static APrefID	kCSyntaxColor_LocalVariable		= 252;
	const static APrefID	kCSyntaxColor_ExternalVariable	= 253;
	const static APrefID	kCSyntaxColor_EnumType			= 254;
	const static APrefID	kCSyntaxColor_Enum				= 255;
	const static APrefID	kCSyntaxColor_Typedef			= 256;
	const static APrefID	kCSyntaxColor_Struct			= 257;
	const static APrefID	kCSyntaxColor_Union				= 258;
	const static APrefID	kCSyntaxColor_Define			= 259;
	const static APrefID	kTeXCompiler					= 260;
	const static APrefID	kTeXPreviewer					= 261;
	
	const static APrefID	kWrapWithLetterCount_LetterCount= 262;
	
	const static APrefID	kIndentOnTheWay					= 301;
	const static APrefID	kIndentForLabel					= 302;
	const static APrefID	kIndentOnTheWaySecondIndent		= 303;
	const static APrefID	kUseSyntaxDefinitionIndent		= 304;
	
	const static APrefID	kIndentWidth					= 305;
	const static APrefID	kIndentExeLetters				= 306;
	
	const static APrefID	kSetDefaultCreator				= 307;
	const static APrefID	kDefaultCreator					= 308;
	const static APrefID	kForceMiTextFile				= 309;
	
	//R0199
	const static APrefID	kDisplaySpellCheck				= 310;
	
	const static APrefID	kSetDefaultTextEncodingWhenNotDecided	= 311;//B0390
	
	const static APrefID	kUseRegExpIndent				= 312;//B0392
	
	const static APrefID	kDefaultFontName						= 313;//win
	
	const static APrefID	kSpellCheckLanguage						= 323;//R0199
	
	const static APrefID	kDragDrop_ActionForNoSuffix				= 328;//#91
	
	const static APrefID	kStartSpaceToIndent						= 331;//#117
	
	const static APrefID	kIndentTabIsAlwaysTabIfIndentOff		= 332;//#281
	
	const static APrefID	kZenkakuSpaceIndent						= 333;//#318
	
	const static APrefID	kImportSystemHeaderFiles				= 336;//#253
	const static APrefID	kSystemHeaderFiles						= 337;//#253
	
	const static APrefID	kUseUserDefinedSDF						= 339;//#349
	const static APrefID	kUserDefinedSDFFilePath					= 340;//#349
	
	const static APrefID	kModeComment							= 341;//#180
	const static APrefID	kModeUpdateURL							= 342;//#426
	
	const static APrefID	kSyntaxDefinitionCategory_Name			= 263;
	const static APrefID	kSyntaxDefinitionCategory_Color			= 264;
	const static APrefID	kSyntaxDefinitionCategory_ImportColor	= 265;
	const static APrefID	kSyntaxDefinitionCategory_LocalColor	= 266;
	const static APrefID	kSyntaxDefinitionState_Name				= 267;
	const static APrefID	kSyntaxDefinitionState_Color			= 268;
	
	const static APrefID	kSDFHeaderDisplayMultiply				= 343;//#493
	
	const static APrefID	kRecognizeConditionalCompileDirective	= 344;//#467
	const static APrefID	kConditionalCompileDisabledColor		= 345;//#467
	
	const static APrefID	kShowUnderlineAtCurrentLine				= 346;//#496
	const static APrefID	kCurrentLineUnderlineColor				= 347;//#496
	
	const static APrefID	kIndent0ForEmptyLine					= 356;//#639
	const static APrefID	kIndentWhenPaste						= 357;//#646
	
	const static APrefID	kEnableMode								= 402;//#868
	
	const static APrefID	kSameAsNormalMode_Font					= 403;//#868
	const static APrefID	kSameAsNormalMode_TextDisplay			= 404;//#868
	const static APrefID	kSameAsNormalMode_BackgroundDisplay		= 405;//#868
	const static APrefID	kSameAsNormalMode_TextLayout			= 406;//#868
	const static APrefID	kSameAsNormalMode_TextProperty			= 407;//#868
	const static APrefID	kSameAsNormalMode_InputtSettings		= 408;//#868
	const static APrefID	kSameAsNormalMode_SpellCheck			= 409;//#868
	
	//#450
	const static APrefID	kUseLineFolding							= 410;//#450
	const static APrefID	kUseDefaultFoldingSetting				= 411;//#450
	const static APrefID	kDefaultFoldingLineText					= 412;//#450
	
	//#889
	const static APrefID	kColorSchemeName						= 414;//#889
	
	//#889
	const static APrefID	kDarkenImportLightenLocal				= 415;//#889
	
	//
	const static APrefID	kFindHighlightColor						= 416;
	const static APrefID	kFindHighlightColorPrev					= 417;
	const static APrefID	kCurrentWordHighlightColor				= 418;
	const static APrefID	kTextMarkerHightlightColor				= 419;
	const static APrefID	kFirstSelectionColor					= 420;
	
	//
	const static APrefID	kSameAsNormalMode_Colors				= 421;//#868
	
	//
	const static APrefID	kNoIndent								= 422;//#912
	
	//
	const static APrefID	kIndentOnTheWay_ParenthesisStartPos		= 423;
	const static APrefID	kIndentOnTheWay_SingleColon				= 424;
	//
	const static APrefID	kIndentAutoDetect						= 425;
	//#481
	const static APrefID	kKeyBindDefaultIsNormalMode				= 426;
	const static APrefID	kKeyBindDefaultIsOSKeyBind				= 427;
	
	//
	const static APrefID	kEnableModeUpdateFromWeb				= 428;
	
	//
	const static APrefID	kEnableSDFCorrespond					= 429;
	
	//
	const static APrefID	kModeUpdateSourceModeName				= 430;
	
	//比較表示色の濃さ
	const static APrefID	kDiffColorOpacity						= 431;
	
	//
	const static APrefID	kSelectionOpacity						= 432;
	
	//
	const static APrefID	kDefaultSyntaxPart						= 433;
	
	//
	const static APrefID	kShiftleftByBackspace					= 434;
	
	//ブロック開始、終了正規表現 #972
	const static APrefID	kIncIndentRegExp						= 435;
	const static APrefID	kDecIndentRegExp						= 436;
	
	//縦書きモード #558
	const static APrefID	kVerticalMode							= 437;
	
	//制御コード色 #1142
	const static APrefID	kControlCodeColor						= 438;
	
	//#349 const static APrefID	kUseSyntaxDefinitionFile		= 2001;
	const static APrefID	kUseBuiltinSyntaxDefinitionFile	= 2002;
	const static APrefID	kBuiltinSyntaxDefinitionName	= 2003;
	const static APrefID	kAdditionalCategory_Name		= 2004;
	const static APrefID	kAdditionalCategory_Color		= 2005;
	const static APrefID	kAdditionalCategory_Bold		= 2006;
	const static APrefID	kAdditionalCategory_RegExp		= 2007;
	const static APrefID	kAdditionalCategory_IgnoreCase	= 2008;
	const static APrefID	kAdditionalCategory_Underline	= 2009;//R0195
	const static APrefID	kAdditionalCategory_Italic		= 2010;//R0195
	const static APrefID	kAdditionalCategory_PriorToOtherColor= 2011;//R0195
	
	const static APrefID	kSyntaxDefinitionCategory_Bold		= 2012;//R0195
	const static APrefID	kSyntaxDefinitionCategory_Italic	= 2013;//R0195
	const static APrefID	kSyntaxDefinitionCategory_Underline	= 2014;//R0195
	
	const static APrefID	kModeSelectFilePath				= 2015;//R0210
	
	const static APrefID	kAdditionalCategory_Disabled	= 2016;//R0000
	const static APrefID	kAdditionalCategory_AutoUpdate	= 2017;//#427
	const static APrefID	kAdditionalCategory_Keywords	= 2018;//#427
	const static APrefID	kAdditionalCategory_UseCSV				= 2019;//#791
	const static APrefID	kAdditionalCategory_CSVPath				= 2020;//#791
	const static APrefID	kAdditionalCategory_KeywordColumn		= 2021;//#791
	const static APrefID	kAdditionalCategory_ExplanationColumn	= 2022;//#791
	const static APrefID	kAdditionalCategory_ParentKeywordColumn	= 2023;//#791
	const static APrefID	kAdditionalCategory_TypeTextColumn		= 2024;//#791
	const static APrefID	kAdditionalCategory_ColorSlotColumn		= 2025;//#791
	const static APrefID	kAdditionalCategory_ColorSlot			= 2026;//#791
	const static APrefID	kAdditionalCategory_ValidSyntaxPart		= 2027;//#791
	const static APrefID	kAdditionalCategory_CompletionTextColumn= 2028;//#791
	const static APrefID	kAdditionalCategory_URLColumn			= 2029;//#791
	
	const static APrefID	kUseRegExpCommentEnd			= 9001;//
	
	const static APrefID	kToolListIcon					= 1001;
	const static APrefID	kToolListFilename				= 1002;
	const static APrefID	kToolbarListIcon				= 1003;
	const static APrefID	kToolbarListFilename			= 1004;
	const static APrefID	kKeywordCategoryName			= 1006;
	const static APrefID	kDragDrop_Suffix				= 1008;
	const static APrefID	kDragDrop_Text					= 1009;
	const static APrefID	kCorrespond_StartText			= 1010;
	const static APrefID	kCorrespond_EndText				= 1011;
	const static APrefID	kIndent_RegExp					= 1012;
	const static APrefID	kIndent_OffsetCurrentLine		= 1013;
	const static APrefID	kIndent_OffsetNextLine			= 1014;
	const static APrefID	kJumpSetup_Name					= 1015;
	const static APrefID	kJumpSetup_RegExp				= 1016;
	const static APrefID	kJumpSetup_MenuText				= 1017;
	const static APrefID	kJumpSetup_SelectionGroup		= 1018;
	const static APrefID	kJumpSetup_KeywordGroup			= 1019;
	const static APrefID	kJumpSetup_ColorizeLine			= 1020;
	const static APrefID	kJumpSetup_LineColor			= 1021;
	const static APrefID	kJumpSetup_KeywordColor			= 1022;
	const static APrefID	kJumpSetup_RegisterToMenu		= 1023;
	const static APrefID	kJumpSetup_RegisterAsKeyword	= 1024;
	const static APrefID	kSuffix							= 1025;
	const static APrefID	kKeyBind_Key					= 1026;
	const static APrefID	kKeyBind_ControlKey				= 1027;
	const static APrefID	kKeyBind_OptionKey				= 1028;
	const static APrefID	kKeyBind_CommandKey				= 1029;
	const static APrefID	kKeyBind_ShiftKey				= 1030;
	const static APrefID	kKeyBind_Action					= 1031;
	const static APrefID	kKeyBind_Text					= 1032;
	const static APrefID	kJumpSetup_AnchorGroup			= 1033;//R0212
	
	const static APrefID	kJapaneseName					= 1034;//#305
	const static APrefID	kEnglishName					= 1035;//#305
	const static APrefID	kFrenchName						= 1036;//#305
	const static APrefID	kGermanName						= 1037;//#305
	const static APrefID	kSpanishName					= 1038;//#305
	const static APrefID	kItalianName					= 1039;//#305
	const static APrefID	kDutchName						= 1040;//#305
	const static APrefID	kSwedishName					= 1041;//#305
	const static APrefID	kNorwegianName					= 1042;//#305
	const static APrefID	kDanishName						= 1043;//#305
	const static APrefID	kFinnishName					= 1044;//#305
	const static APrefID	kPortugueseName					= 1045;//#305
	const static APrefID	kSimplifiedChineseName			= 1046;//#305
	const static APrefID	kTraditionalChineseName			= 1047;//#305
	const static APrefID	kKoreanName						= 1048;//#305
	const static APrefID	kPolishName						= 1049;//#305
	const static APrefID	kPortuguesePortugalName			= 1050;//#305
	const static APrefID	kRussianName					= 1051;//#305
	
	const static APrefID	kDefaultFontNameOld							= 1052;//#375
	
	const static APrefID	kJumpSetup_DisplayMultiply					= 1057;//#493
	
	const static APrefID	kAutoUpdateRevisionText			= 1058;//#427
	const static APrefID	kLastAutoUpdateDateTime			= 1059;//#427
	
	const static APrefID	kJumpSetup_ColorizeRegExpGroup			= 1061;//#603
	const static APrefID	kJumpSetup_ColorizeRegExpGroup_Number	= 1062;//#603
	const static APrefID	kJumpSetup_ColorizeRegExpGroup_Color	= 1063;//#603
	
	const static APrefID	kJumpSetup_OutlineLevel					= 1064;//#130
	const static APrefID	kJumpSetup_SimplerSetting				= 1065;//#130
	const static APrefID	kJumpSetup_LineStartText				= 1066;//#130
	const static APrefID	kJumpSetup_RegisterToMenu2				= 1067;//#130
	
	const static APrefID	kJumpSetup_KeywordColorSlot				= 1068;//#889
	
	const static APrefID	kAutoUpdateFromAppDateTimeText			= 1069;
	
	const static APrefID	kJumpSetup_Enable						= 1070;
	
	//#994
	const static APrefID	kPluginData_Id							= 1071;
	const static APrefID	kPluginData_Enabled						= 1072;
	const static APrefID	kPluginData_Name						= 1073;
	const static APrefID	kPluginDisplayName_Id_Language			= 1074;
	const static APrefID	kPluginData_Options						= 1075;
	const static APrefID	kPluginDisplayName_DisplayName			= 1076;
	
	//#844
	const static APrefID	kSyntaxColorSlot0_Color					= 7511;
	const static APrefID	kSyntaxColorSlot1_Color					= 7521;
	const static APrefID	kSyntaxColorSlot2_Color					= 7531;
	const static APrefID	kSyntaxColorSlot3_Color					= 7541;
	const static APrefID	kSyntaxColorSlot4_Color					= 7551;
	const static APrefID	kSyntaxColorSlot5_Color					= 7561;
	const static APrefID	kSyntaxColorSlot6_Color					= 7571;
	const static APrefID	kSyntaxColorSlot7_Color					= 7581;
	const static APrefID	kSyntaxColorSlotComment_Color			= 7611;
	const static APrefID	kSyntaxColorSlotLiteral_Color			= 7621;
	//#844
	const static APrefID	kSyntaxColorSlot0_Bold					= 7512;
	const static APrefID	kSyntaxColorSlot1_Bold					= 7522;
	const static APrefID	kSyntaxColorSlot2_Bold					= 7532;
	const static APrefID	kSyntaxColorSlot3_Bold					= 7542;
	const static APrefID	kSyntaxColorSlot4_Bold					= 7552;
	const static APrefID	kSyntaxColorSlot5_Bold					= 7562;
	const static APrefID	kSyntaxColorSlot6_Bold					= 7572;
	const static APrefID	kSyntaxColorSlot7_Bold					= 7582;
	const static APrefID	kSyntaxColorSlotComment_Bold			= 7612;
	const static APrefID	kSyntaxColorSlotLiteral_Bold			= 7622;
	//#844
	const static APrefID	kSyntaxColorSlot0_Italic				= 7513;
	const static APrefID	kSyntaxColorSlot1_Italic				= 7523;
	const static APrefID	kSyntaxColorSlot2_Italic				= 7533;
	const static APrefID	kSyntaxColorSlot3_Italic				= 7543;
	const static APrefID	kSyntaxColorSlot4_Italic				= 7553;
	const static APrefID	kSyntaxColorSlot5_Italic				= 7563;
	const static APrefID	kSyntaxColorSlot6_Italic				= 7573;
	const static APrefID	kSyntaxColorSlot7_Italic				= 7583;
	const static APrefID	kSyntaxColorSlotComment_Italic			= 7613;
	const static APrefID	kSyntaxColorSlotLiteral_Italic			= 7623;
	//#844
	const static APrefID	kSyntaxColorSlot0_Underline				= 7514;
	const static APrefID	kSyntaxColorSlot1_Underline				= 7524;
	const static APrefID	kSyntaxColorSlot2_Underline				= 7534;
	const static APrefID	kSyntaxColorSlot3_Underline				= 7544;
	const static APrefID	kSyntaxColorSlot4_Underline				= 7554;
	const static APrefID	kSyntaxColorSlot5_Underline				= 7564;
	const static APrefID	kSyntaxColorSlot6_Underline				= 7574;
	const static APrefID	kSyntaxColorSlot7_Underline				= 7584;
	const static APrefID	kSyntaxColorSlotComment_Underline		= 7614;
	const static APrefID	kSyntaxColorSlotLiteral_Underline		= 7624;
	
	
	//#844
	//========================バージョン3.0でobsoleteにした設定========================
  private:
	//
	const static APrefID	kDefaultEnableAntiAlias					= 110;
	const static APrefID	kDefaultWindowWidth						= 111;
	const static APrefID	kDefaultWindowHeight					= 112;
	//
	const static APrefID	kDefaultPrintOption_UsePrintFont		= 114;
	//#688 const static APrefID	kDefaultPrintOption_PrintFont_compat	= 115;
	const static APrefID	kDefaultPrintOption_PrintFontSize		= 116;
	const static APrefID	kDefaultPrintOption_PrintPageNumber		= 117;
	//#688 const static APrefID	kDefaultPrintOption_PageNumberFont_compat= 118;
	const static APrefID	kDefaultPrintOption_PageNumberFontSize	= 119;
	const static APrefID	kDefaultPrintOption_PrintFileName		= 120;
	//#688 const static APrefID	kDefaultPrintOption_FileNameFont_compat	= 121;
	const static APrefID	kDefaultPrintOption_FileNameFontSize	= 122;
	const static APrefID	kDefaultPrintOption_PrintSeparateLine	= 123;
	const static APrefID	kDefaultPrintOption_PrintLineNumber		= 124;
	//#688 const static APrefID	kDefaultPrintOption_LineNumberFont_compat= 125;
	const static APrefID	kDefaultPrintOption_ForceWordWrap		= 126;
	const static APrefID	kDefaultPrintOption_LineMargin			= 127;
	const static APrefID	kDefaultPrintOption_PrintFontNameOld		= 1053;//#375
	const static APrefID	kDefaultPrintOption_PageNumberFontNameOld	= 1054;//#375
	const static APrefID	kDefaultPrintOption_FileNameFontNameOld		= 1055;//#375
	const static APrefID	kDefaultPrintOption_LineNumberFontNameOld	= 1056;//#375
	const static APrefID	kDefaultPrintOption_PrintFontName		= 314;//win
	const static APrefID	kDefaultPrintOption_PageNumberFontName	= 315;//win
	const static APrefID	kDefaultPrintOption_FileNameFontName	= 316;//win
	const static APrefID	kDefaultPrintOption_LineNumberFontName	= 317;//win
	//
	const static APrefID	kDisplayInformationHeader		= 134;
	//
	const static APrefID	kDisplayPositionButton			= 136;
	//
	const static APrefID	kDisplayLineNumberButton		= 141;
	const static APrefID	kDisplayLineNumberButton_AsParagraph	= 142;
	//
	const static APrefID	kIdInfoWindowDisplayComment				= 318;//RC2
	//
	const static APrefID	kPrintMargin_Left						= 324;//R0242
	const static APrefID	kPrintMargin_Right						= 325;//R0242
	const static APrefID	kPrintMargin_Top						= 326;//R0242
	const static APrefID	kPrintMargin_Bottom						= 327;//R0242
	//
	const static APrefID	kSelectInsideBraceByDoubleClick	= 152;
	//
	const static APrefID	kSaveDOSEOF						= 161;
	//
	const static APrefID	kUseDefaultSuffixForSave				= 329;//#207
	const static APrefID	kDefaultSuffixForSaveText				= 330;//#207
	//
	const static APrefID	kUseEmbeddedSDF							= 338;//#349
	//
	const static APrefID	kDisplayAbbrevCandidateWindow			= 319;//RC2
	const static APrefID	kHitCountForAbbrevCandidiateWindow		= 320;//RC2
	//
    const static APrefID	kKinsokuBuraasge				= 154;
    //
	const static APrefID	kSaveDocumentPrefToResource				= 334;//#332
    const static APrefID	kSaveDocumentPrefToAppPref				= 335;//#332
    //
	const static APrefID	kSaveTextEncodingToEA					= 321;//R0230
	const static APrefID	kDefaultDisplayCrossCaret				= 322;//R0185
	//
	const static APrefID	kEnableAutoUpdateTool					= 348;//#427
	const static APrefID	kEnableAutoUpdateToolbar				= 349;//#427
	const static APrefID	kEnableAutoUpdateKeyword				= 350;//#427
	const static APrefID	kEnableAutoUpdateJumpSetup				= 351;//#427
	const static APrefID	kEnableUserTool							= 352;//#427
	const static APrefID	kEnableUserToolbar						= 353;//#427
	const static APrefID	kEnableUserKeyword						= 354;//#427 ただし未使用（常にON扱い）
	const static APrefID	kEnableUserJumpSetup					= 355;//#427 ただし未使用（常にON扱い）
	//
	const static APrefID	kJumpSetup_AutoUpdate			= 1060;//#427
	const static APrefID	kShowDialogWhenTextEncodingNotDecided	= 169;
	//#844
	const static APrefID	kCommentColor					= 185;
	const static APrefID	kLiteralColor					= 190;
	
	
	
};


