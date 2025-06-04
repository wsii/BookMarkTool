// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UBookMarkDataAsset;

struct FPositionBookmarksContainer;
/**
 * 
 */
class BOOKMARKTOOL_API SBookMarkPositionTab : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBookMarkPositionTab)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	void RefreshAllListView();
	
private:
	FString CurrentActorName;
	TWeakObjectPtr<UBookMarkDataAsset> Bookmarks;
	TArray<TSharedPtr<FPositionBookmarksContainer>> StoredPosition;
	
	TSharedPtr<SListView<TSharedPtr<FPositionBookmarksContainer>>> PositionListView;
	TSharedRef<SListView<TSharedPtr<FPositionBookmarksContainer>>> ConstructPositionListView();
	
	TSharedRef<ITableRow> OnGenerateRowForPositionList(TSharedPtr<FPositionBookmarksContainer> PositionDataToDisplay,
											   const TSharedRef<STableViewBase>& OwnerTable);
	
	TSharedRef<STextBlock> ConstructNameTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse);
	TSharedRef<STextBlock> ConstructPathTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse);
	FSlateFontInfo GetFontSytle() const { return FCoreStyle::Get().GetFontStyle("EmbossedText"); }

	TSharedRef<SButton> ConstructPathButton(const TSharedPtr<FPositionBookmarksContainer>& PositionDataToDisplay);
	
	void OnPositionRowWidgetClicked(TSharedPtr<FPositionBookmarksContainer> ClickedData);
	
	void LoadBookMarkDataAsset();
	void GetStoredPosition();

	
	FReply OpenSettingDataAsset();
	FReply AddPostion();
	
	FText CurrentText;
	void HandleTextChanged(const FText& InText);

	FReply OnDeletePositionButtonClicked(TSharedPtr<FPositionBookmarksContainer> ClickedData);


};
