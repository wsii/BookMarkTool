// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UBookMarkDataAsset;
struct FFoldPathBookmarksContainer;
struct FAssetBookmarksContainer;
/**
 * 
 */
class BOOKMARKTOOL_API SBookMarkToolTab : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBookMarkToolTab)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void RefreshAllListView();
	
private:
	UBookMarkDataAsset* Bookmarks;
	TArray<TSharedPtr<FFoldPathBookmarksContainer>> StoredFoldPath;
	TArray<TSharedPtr<FAssetBookmarksContainer>> StoredAssetData;

	TSharedPtr<SListView<TSharedPtr<FFoldPathBookmarksContainer>>> FoldPathListView;
	TSharedRef<SListView<TSharedPtr<FFoldPathBookmarksContainer>>> ConstructFoldPathListView();
	TSharedPtr<SListView<TSharedPtr<FAssetBookmarksContainer>>> AssetListView;
	TSharedRef<SListView<TSharedPtr<FAssetBookmarksContainer>>> ConstructAssetListView();
	
	void LoadBookMarkDataAsset();
	
	TSharedRef<ITableRow> OnGenerateRowForFoldPathList(TSharedPtr<FFoldPathBookmarksContainer> AssetDataToDisplay,
											   const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<ITableRow> OnGenerateRowForAssetList(TSharedPtr<FAssetBookmarksContainer> AssetDataToDisplay,
										   const TSharedRef<STableViewBase>& OwnerTable);
	

	TSharedRef<STextBlock> ConstructTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse);
	TSharedRef<STextBlock> ConstructAssetTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse);
	FSlateFontInfo GetFontSytle() const { return FCoreStyle::Get().GetFontStyle("EmbossedText"); }

	TSharedRef<SButton> ConstructPathButton(const TSharedPtr<FFoldPathBookmarksContainer>& AssetDataToDisplay);
	TSharedRef<SButton> ConstructAssetButton(const TSharedPtr<FAssetBookmarksContainer>& AssetDataToDisplay);
	
	void OnPathCRowWidgetClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData);
	void OnAssetRowWidgetClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData);
	
	void GetStoredFoldPath();
	void GetStoredAssetData();

	FReply OpenSettingDataAsset();

	FReply OnDeletePathButtonClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData);
	FReply OnDeleteAssetButtonClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData);
	// FReply OnDeleteButtonClicked();

	// TSharedRef<STextBlock> ConstructTextBlockForTabButton(const FString& content);

};
