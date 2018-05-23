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

ADocument_IndexWindow

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#if IMPLEMENTATION_FOR_MACOSX
#include "AppleScript.h"
#endif
#import "IndexWindowDocument.h"

enum AIndexRowType
{
	kIndexRowType_TextPosition = 0
};

class AIndexGroup : public AObjectArrayItem
{
  public:
	AIndexGroup( AObjectArrayItem* inParent );
	~AIndexGroup();
	
  public:
	void	InsertItems_TextPosition(  const AIndex inItemIndex, const ATextArray& inFilePath, 
			const ATextArray& inHitText, const ATextArray& inPreHitText, const ATextArray& inPostHitText, const ATextArray& inPositionText, 
			const ATextArray& inPargraphText,//#465
			const ANumberArray& inStartPosition, const ANumberArray& inLength, const ANumberArray& inParagraph, const AText& inComment );
	void	SetGroupTitleText( const AText& inTitle1, const AText& inTitle2 );
	void	GetGroupTitleText( AText& outTitle1, AText& outTitle2 ) const;
	void	SetGroupBaseFolder( const AFileAcc& inBaseFolder );//#465
	void	GetGroupBaseFolder( AFileAcc& outBaseFolder ) const;//#465
	void	InsertItem_TextPosition( const AIndex inItemIndex, const AText& inFilePath,
			const AText& inHitText, const AText& inPreHitText, const AText& inPostHitText, const AText& inPositionText,
			const AText& inParagraphText,//#465
			const ANumber inStartPosition, const ANumber inLength, const ANumber inParagraph, const AText& inComment );
	void	SetItem_TextPosition( const AIndex inItemIndex, const AText& inFilePath,
			const AText& inHitText, const AText& inPreHitText, const AText& inPostHitText, const AText& inPositionText,
			const AText& inParagraphText,//#465
			const ANumber inStartPosition, const ANumber inLength, const ANumber inParagraph, const AText& inComment );
	void	Update_TextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount );
	void	DeleteItem( const AIndex inItemIndex );
	
	void	SetComment( const AIndex inItemIndex, const AText& inComment ) { mArray_CommentText.Set(inItemIndex,inComment); }
	void	SetFilePath( const AIndex inItemIndex, const AText& inText ) { mArray_FilePath.Set(inItemIndex,inText); }
	/* void	SetStartPosition( const AIndex inItemIndex, const ANumber inNumber ) { mArray_StartPosition.Set(inItemIndex,inNumber); }
	void	SetLength( const AIndex inItemIndex, const ANumber inNumber ) { mArray_Length.Set(inItemIndex,inNumber); }
	void	SetParagraph( const AIndex inItemIndex, const ANumber inNumber ) { mArray_Paragraph.Set(inItemIndex,inNumber); }*/
	
	AItemCount	GetItemCount() const;
	void	Get( const AIndex inItemIndex, 
			AText& outFilePath, AText& outHitText, AText& outPreHitText, AText& outPostHitText, AText& outPositionText, 
			ANumber& outStartPosition, ANumber& outLength, ANumber& outParagraph, AText& outComment ) const;
	void	GetParagraphText( const AIndex inItemIndex, AText& outParagraphText ) const;//#465
  private:
	void	InsertFileToItemIndexArray( const AText& inFilePath, const AIndex inItemIndex, const ABool inAddedAtLast );//#895
	AText					mTitle1;
	AText					mTitle2;
	AFileAcc				mBaseFolder;//#465
	
	AArray<AIndexRowType>	mArray_Type;
	ATextArray				mArray_HitText;
	ATextArray				mArray_PreHitText;
	ATextArray				mArray_PostHitText;
	ATextArray				mArray_FilePath;
	ATextArray				mArray_PositionText;
	ATextArray				mArray_ParagraphText;//#465
	ATextArray				mArray_CommentText;
	ANumberArray			mArray_StartPosition;//ドキュメントの最初からのTextIndex
	ANumberArray			mArray_Length;
	ANumberArray			mArray_Paragraph;//段落index（0～)
	
	//#895
	//各ファイルに対応する項目indexテーブル
	AHashTextArray					mFileToItemIndexArray_Path;
	AObjectArray< AArray<AIndex> >	mFileToItemIndexArray_IndexArray;
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AIndexGroup"; }
	
};

#pragma mark ===========================
#pragma mark [クラス]ADocument_IndexWindow
class ADocument_IndexWindow : public ADocument
{
	//コンストラクタ、デストラクタ
  public:
	ADocument_IndexWindow( AObjectArrayItem* inParent, const AObjectID inDocImpID );//#1034
	~ADocument_IndexWindow();
	
	//
  public:
	AItemCount				SPI_GetGroupCount() const { return mIndexGroup.GetItemCount(); }
  private:
	AObjectArray< AIndexGroup >		mIndexGroup;
	AArray<ABool>					mIndexGroup_Expanded;
	//初期設定
  public:
	//#92 void					Init( const AText& inTitle );
	void					Load( const AFileAcc& inFile );//#92 Init->Load
	AText								mTitle;
	
	//
  public:
	void					SPI_SetTitle( const AText& inTitle );
	void					SPI_Save( const ABool inSaveAs );
	
	//ドキュメントタイプ取得
  public:
	ADocumentType			NVIDO_GetDocumentType() const {return kDocumentType_IndexWindow;}
	
	//<インターフェイス>
	
	//表示用データ取得
  public:
	AItemCount				SPI_GetDisplayRowCount() const;
	void					SPI_GetDisplayRowData( const AIndex inDisplayRowIndex, 
			AText& outFilePath, AText& outHitText, AText& outPreHitText, AText& outPostHitText, AText& outPositionText, 
			ANumber& outStartPosition, ANumber& outLength, ANumber& outParagraph, AIndex& outGroupIndex, ABool& outIsGroup, AText& outComment ) const;
	//Documentデータ設定
  public:
	void					SPI_InsertGroup( const AIndex inIndex, const AText& inGroupTitleText1, const AText& inGroupTitleText2,
							const AFileAcc& inBaseFolder );//#465
	void					SPI_AddGroup( const AText& inGroupTitleText1, const AText& inGroupTitleText2,
							const AFileAcc& inBaseFolder );//#465
	void					SPI_InsertItems_TextPosition( const AIndex inGroupIndex, const AIndex inItemIndex, const ATextArray& inFilePath, 
			const ATextArray& inHitText, const ATextArray& inPreHitText, const ATextArray& inPostHitText, const ATextArray& inPositionText, 
			const ATextArray& inParagraphText,//#465
			const ANumberArray& inStartPosition, const ANumberArray& inLength, const ANumberArray& inParagraph, const AText& inComment );
	void					SPI_InsertItem_TextPosition( const AIndex inGroupIndex, const AIndex inItemIndex, const AText& inFilePath,
			const AText& inHitText, const AText& inPreHitText, const AText& inPostHitText, const AText& inPositionText,
			const AText& inParagraphText,//#465
			const ANumber inStartPosition, const ANumber inLength, const ANumber inParagraph, const AText& inComment );
	void					SPI_SetItem_TextPosition( const AIndex inGroupIndex, const AIndex inItemIndex, const AText& inFilePath,
			const AText& inHitText, const AText& inPreHitText, const AText& inPostHitText, const AText& inPositionText,
			const AText& inParagraphText,//#465
			const ANumber inStartPosition, const ANumber inLength, const ANumber inParagraph, const AText& inComment );
	void					SPI_AddItems_ToFirstGroup_TextPosition( const ATextArray& inFilePath, 
			const ATextArray& inHitText, const ATextArray& inPreHitText, const ATextArray& inPostHitText, const ATextArray& inPositionText, 
			const ATextArray& inParagraphText,//#465
			const ANumberArray& inStartPosition, const ANumberArray& inLength, const ANumberArray& inParagraph, const AText& inComment );
	void					SPI_AddItem_TextPosition( const AIndex inGroupIndex, const AText& inFilePath,
			const AText& inHitText, const AText& inPreHitText, const AText& inPostHitText, const AText& inPositionText,
			const AText& inParagraphText,//#465
			const ANumber inStartPosition, const ANumber inLength, const ANumber inParagraph, const AText& inComment );
	
	void					SPI_SetItemComment( const AIndex inGroupIndex, const AIndex inItemIndex, const AText& inComment );
	void					SPI_SetItemFile( const AIndex inGroupIndex, const AIndex inItemIndex, const AFileAcc& inFile );
	
	void					SPI_ExpandCollapse( const AIndex inIndex );
	void					SPI_Expand( const AIndex inGroupIndex );
	void					SPI_Collapse( const AIndex inGroupIndex );
	void					SPI_CollapseFirstGroup();
	ABool					SPI_IsGroupExpanded( const AIndex inIndex ) const;
	void					SPI_SelectGroup( const AIndex inGroupIndex );
	void					SPI_Select( const AIndex inGroupIndex, const AIndex inItemIndex );
	
	void					SPI_DeleteItem( const AIndex inGroupIndex, const AIndex inItemIndex );
	void					SPI_DeleteAllItemsInGroup( const AIndex inGroupIndex );
	
	void					SPI_DeleteGroup( const AIndex inGroupIndex );
	void					SPI_DeleteAllGroup();
	void					SPI_GetGroupTitle( const AIndex inGroupIndex, AText& outTitle1, AText& outTitle2 ) const;
	void					SPI_SetGroupTitle( const AIndex inGroupIndex, const AText& inGroupTitleText1, const AText& inGroupTitleText2 );
	void					SPI_GetGroupBaseFolder( const AIndex inGroupIndex, AFileAcc& outBaseFolder ) const;//#465
	AItemCount				SPI_GetItemCountInGroup( const AIndex inGroupIndex ) const { return mIndexGroup.GetObjectConst(inGroupIndex).GetItemCount(); }
  private:
	void					GetGroupItemIndexFromDisplayRow( const AIndex inDisplayRowIndex, AIndex& outGroupIndex, AIndex& outItemIndex ) const;
	void					GetDisplayRowIndexFromGroupItemIndex( const AIndex inGroupIndex, const AIndex inItemIndex, AIndex& outDisplayRowIndex ) const;
	
	//グループデータ取得 #465
  public:
	void					SPI_GetItemData( const AIndex inGroupIndex, const AIndex inItemIndex, 
							AText& outFilePath, AText& outHitText, 
							AText& outPreHitText, AText& outPostHitText, AText& outPositionText, 
							AText& outParagraphText,
							ANumber& outStartPosition, ANumber& outLength, ANumber& outParagraph, 
							AText& outComment ) const;
	
	//テキスト位置補正
  public:
	void					SPI_Update_TextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount );
	
	/*#379 View登録解除時にドキュメント解放するように変更
	//<所有クラス(AWindow)インターフェイス>
  public:
	void					OWICB_DoWindowClosed( const AWindowID inWindowID );
	*/
	
	void					SPI_RefreshRow( const AIndex inGroupIndex, const AIndex inItemIndex );
	
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	
	
	void					NVIDO_GetTitle( AText& outTitle ) const;
	
	//
  public:
	void					SPI_SetAskSavingMode( const ABool inAskSaving ) 
	{
		mAskSavingMode = inAskSaving;
		if( mAskSavingMode == true )
		{
			NVI_SetDirty(true);
		}
	}
	ABool					SPI_GetAskSavingMode() const { return mAskSavingMode; }
	private:
	ABool								mAskSavingMode;
	
	/*#92
	//ウインドウ選択
  public:
	void					SPI_SelectFirstWindow();
	
  public:
	AWindowID							mWindowID;
	*/
	
	/*#1034
#if IMPLEMENTATION_FOR_MACOSX
	//AppleScript
  public:
	CASIndexDoc&				SPI_GetASImp() { return mASImp; }
  private:
	CASIndexDoc							mASImp;
#endif
	*/
	
	//
  public:
	void					SPI_TryClose();
	void					SPI_ExecuteClose();
	void					SPI_InhibitClose( const ABool inInhibit );
	
	//AppleScript用ドキュメントデータ取得
  public:
	AItemCount				SPI_ASGetGroupCount() const { return mIndexGroup.GetItemCount(); }
	ABool					SPI_ASGetRecord( const AIndex inRecordIndex, 
							AFileAcc& outFile, ATextIndex& outStartPos, AItemCount& outLenght, ANumber& outparagraph, AText& outComment ) const;
	ABool					SPI_GetIndexFromASRecordIndex( const AIndex inASRecordIndex, AIndex& outGroupIndex, AIndex& outIndexInGroup ) const;
	ABool					SPI_GetASRecordIndex( const AIndex inGroupIndex, const AIndex inItemIndex, AIndex& outASRecordIndex ) const;
	
	//#92
	//AppleScript用
  public:
	void					SPI_ASGetColumnWidth( ANumber& outFileColumnWidth, ANumber& outInfoColumnWidth ) const;
	void					SPI_ASSetFileColumnWidth( const ANumber inNewWidth );
	void					SPI_ASSetInfoColumnWidth( const ANumber inNewWidth );
	AWindowID				SPI_ASGetWindowID() const;
  private:
};

#pragma mark ===========================
#pragma mark [クラス]AIndexWindowDocumentFactory
typedef AAbstractFactoryForObjectArray<ADocument>	ADocumentFactory;
class AIndexWindowDocumentFactory : public ADocumentFactory
{
  public:
	AIndexWindowDocumentFactory( AObjectArrayItem* inParent, const AObjectID inDocImpID ) : mParent(inParent), mDocImpID(inDocImpID)//#1034
	{
	}
	ADocument*	Create()
	{
		return new ADocument_IndexWindow(mParent,mDocImpID);//#1034
	}
  private:
	AObjectArrayItem*	mParent;
	AObjectID	mDocImpID;//#1034
};



