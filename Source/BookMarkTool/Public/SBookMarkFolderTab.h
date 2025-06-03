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
class BOOKMARKTOOL_API SBookMarkFolderTab : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBookMarkFolderTab)
	{}
	SLATE_END_ARGS()
	
	/** Constructs this widget with InArgs */
	
	void Construct(const FArguments& InArgs);
	void RefreshAllListView();
	
private:
	TWeakObjectPtr<UBookMarkDataAsset> Bookmarks;
	TArray<TSharedPtr<FFoldPathBookmarksContainer>> StoredFoldPath;


	TSharedPtr<SListView<TSharedPtr<FFoldPathBookmarksContainer>>> FoldPathListView;
	TSharedRef<SListView<TSharedPtr<FFoldPathBookmarksContainer>>> ConstructFoldPathListView();

	
	void LoadBookMarkDataAsset();
	
	TSharedRef<ITableRow> OnGenerateRowForFoldPathList(TSharedPtr<FFoldPathBookmarksContainer> AssetDataToDisplay,
											   const TSharedRef<STableViewBase>& OwnerTable);

	
	TSharedRef<STextBlock> ConstructTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse);

	FSlateFontInfo GetFontSytle() const { return FCoreStyle::Get().GetFontStyle("EmbossedText"); }

	TSharedRef<SButton> ConstructPathButton(const TSharedPtr<FFoldPathBookmarksContainer>& AssetDataToDisplay);
	
	void OnPathCRowWidgetClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData);

	
	void GetStoredFoldPath();
	
	FReply OpenSettingDataAsset();

	FReply OnDeletePathButtonClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData);

};
