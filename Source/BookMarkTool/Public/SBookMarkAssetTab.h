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
class BOOKMARKTOOL_API SBookMarkAssetTab : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBookMarkAssetTab)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void RefreshAllListView();
	
private:
	TWeakObjectPtr<UBookMarkDataAsset> Bookmarks;

	TArray<TSharedPtr<FAssetBookmarksContainer>> StoredAssetData;
	
	TSharedPtr<SListView<TSharedPtr<FAssetBookmarksContainer>>> AssetListView;
	TSharedRef<SListView<TSharedPtr<FAssetBookmarksContainer>>> ConstructAssetListView();
	
	void LoadBookMarkDataAsset();
	
	TSharedRef<ITableRow> OnGenerateRowForAssetList(TSharedPtr<FAssetBookmarksContainer> AssetDataToDisplay,
										   const TSharedRef<STableViewBase>& OwnerTable);
	

	TSharedRef<STextBlock> ConstructTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse);
	TSharedRef<STextBlock> ConstructAssetTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse);
	FSlateFontInfo GetFontSytle() const { return FCoreStyle::Get().GetFontStyle("EmbossedText"); }
	
	TSharedRef<SButton> ConstructAssetButton(const TSharedPtr<FAssetBookmarksContainer>& AssetDataToDisplay);
	
	void OnAssetRowWidgetClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData);
	
	void GetStoredAssetData();

	FReply OpenSettingDataAsset();
	
	FReply OnDeleteAssetButtonClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData);


};
