// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BookMarkDataAsset.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class BOOKMARKTOOL_API SBookMarkMain : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBookMarkMain)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void RefreshAllListView();

private:
	TWeakObjectPtr<UBookMarkDataAsset> Bookmarks;

	TArray<TSharedPtr<FAssetBookmarksContainer>> StoredAssetData;
	TArray<TSharedPtr<FFoldPathBookmarksContainer>> StoredFoldPath;
	TArray<TSharedPtr<FPositionBookmarksContainer>> StoredPosition;
	
	TSharedPtr<SListView<TSharedPtr<FAssetBookmarksContainer>>> AssetListView;
	TSharedRef<SListView<TSharedPtr<FAssetBookmarksContainer>>> ConstructAssetListView();

	TSharedPtr<SListView<TSharedPtr<FFoldPathBookmarksContainer>>> FoldPathListView;
	TSharedRef<SListView<TSharedPtr<FFoldPathBookmarksContainer>>> ConstructFoldPathListView();

	TSharedPtr<SListView<TSharedPtr<FPositionBookmarksContainer>>> PositionListView;
	TSharedRef<SListView<TSharedPtr<FPositionBookmarksContainer>>> ConstructPositionListView();
	
	void LoadBookMarkDataAsset();
	
	TSharedRef<ITableRow> OnGenerateRowForAssetList(TSharedPtr<FAssetBookmarksContainer> AssetDataToDisplay,
										   const TSharedRef<STableViewBase>& OwnerTable);

	TSharedRef<ITableRow> OnGenerateRowForFoldPathList(TSharedPtr<FFoldPathBookmarksContainer> AssetDataToDisplay,
										   const TSharedRef<STableViewBase>& OwnerTable);

	TSharedRef<ITableRow> OnGenerateRowForPositionList(TSharedPtr<FPositionBookmarksContainer> PositionDataToDisplay,
										   const TSharedRef<STableViewBase>& OwnerTable);
	
	TSharedRef<STextBlock> ConstructNameTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse);
	

	TSharedRef<SButton> ConstructAssetButton(const TSharedPtr<FAssetBookmarksContainer>& AssetDataToDisplay);
	TSharedRef<SButton> ConstructPathButton(const TSharedPtr<FFoldPathBookmarksContainer>& FoldPathDataToDisplay);
	TSharedRef<SButton> ConstructPositionButton(const TSharedPtr<FPositionBookmarksContainer>& PositionDataToDisplay);
	TSharedRef<SButton> ConstructPositionCopyButton(const TSharedPtr<FPositionBookmarksContainer>& PositionDataToDisplay);
	
	FSlateFontInfo GetFontSytle() const { return FCoreStyle::Get().GetFontStyle("EmbossedText"); }
	
	void OnAssetRowWidgetClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData);
	void OnPathCRowWidgetClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData);
	void OnPositionRowWidgetClicked(TSharedPtr<FPositionBookmarksContainer> ClickedData);
	
	void GetStoredData();

	FText CurrentText;
	FString CurrentActorName;
	
	void HandleTextChanged(const FText& InText);
	FReply AddPostion();
	
	FReply OpenSettingDataAsset();
	
	FReply RefreshAll();
	
	FReply OnDeleteAssetButtonClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData);

	FReply OnDeletePathButtonClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData);

	FReply OnDeletePositionButtonClicked(TSharedPtr<FPositionBookmarksContainer> ClickedData);
	FReply OnCopyPositionButtonClicked(TSharedPtr<FPositionBookmarksContainer> ClickedData);
};
