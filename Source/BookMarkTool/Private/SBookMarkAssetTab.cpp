// Fill out your copyright notice in the Description page of Project Settings.


#include "SBookMarkAssetTab.h"
#include "SlateOptMacros.h"
#include "BookMarkDataAsset.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "EditorDialogLibrary.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/IAssetRegistry.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBookMarkAssetTab::Construct(const FArguments& InArgs)
{
	FSlateFontInfo FontInfo = GetFontSytle();
	FontInfo.Size = 12;
	//加载数据资产
	LoadBookMarkDataAsset();
	
	ChildSlot
	[
		SNew(SBox)
		.MaxDesiredWidth(1600.0f)
		.MaxDesiredHeight(300.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			// main slot
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SButton)
				.ContentPadding(FMargin(4.0, 2.0))
				.Text(FText::FromString(TEXT("打开配置资产")))
				.OnClicked(this, &SBookMarkAssetTab::OpenSettingDataAsset)
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				// 左侧分隔线
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(5.0f, 0.0f) // 文字与分隔线的间距
				[
					SNew(SSeparator)
					.Thickness(1.0f)
				]
				// 中间文字
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("资产(+Shift单击打开)")))
					.Margin(5.0f)
					.Font(FontInfo)
				]
				// 右侧分隔线
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(5.0f, 0.0f)
				[
					SNew(SSeparator)
					.Thickness(1.0f)
				]
			]
			//AssetList
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					ConstructAssetListView()	
				]
			]
		]
		
	];
	
}


void SBookMarkAssetTab::LoadBookMarkDataAsset()
{
	//加载资源
	Bookmarks = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));

	if (Bookmarks == nullptr)
	{
		UEditorDialogLibrary::ShowMessage(
		FText::FromString(TEXT("警告")),
		FText::FromString(TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset资源不存在")),
		EAppMsgType::OkCancel
		);
		return;
	}
	Bookmarks->ReSetStorePath();
	
	//填充数据
	GetStoredAssetData();
}


TSharedRef<SListView<TSharedPtr<FAssetBookmarksContainer>>> SBookMarkAssetTab::ConstructAssetListView()
{
	AssetListView =
	SNew(SListView<TSharedPtr<FAssetBookmarksContainer>>)
	.SelectionMode(ESelectionMode::Single)
	.ListItemsSource(&StoredAssetData) // this is a TArray of above TSharedPtr<FAssetData>
	.OnGenerateRow(this, &SBookMarkAssetTab::OnGenerateRowForAssetList)
	.OnMouseButtonClick(this,&SBookMarkAssetTab::OnAssetRowWidgetClicked);
	// A Pointer can convert to be a ref.
	return AssetListView.ToSharedRef();
}

void SBookMarkAssetTab::RefreshAllListView()
{
	if (Bookmarks.Get())
	{
		UEditorAssetLibrary::SaveLoadedAsset(Bookmarks.Get());
		Bookmarks->ClearGarbage();
		Bookmarks = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));
		GetStoredAssetData();
	}
	
	AssetListView->RebuildList();

}

TSharedRef<ITableRow> SBookMarkAssetTab::OnGenerateRowForAssetList(
	TSharedPtr<FAssetBookmarksContainer> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	//check,  in case to return a None ptr
	if (!AssetDataToDisplay.IsValid())
	{
		return SNew(STableRow<TSharedPtr<FAssetBookmarksContainer>>, OwnerTable);
	}
	
	//main
	const FString DisplayAssetName = AssetDataToDisplay->Name;
	const FString DisplayClassName = AssetDataToDisplay->Path.ToString();
	FSlateFontInfo SmallFont = GetFontSytle();
	SmallFont.Size = 8;
	FSlateFontInfo BigFont = GetFontSytle();
	BigFont.Size = 12;

	// define each row(SHorizontalBox= CheckBox + textbox + button) of the OwnerTable
	// return a ref of this row function as a widget
	TSharedRef<STableRow<TSharedPtr<FFoldPathBookmarksContainer>>> ListViewRowWidget =
		SNew(STableRow<TSharedPtr<FFoldPathBookmarksContainer>>, OwnerTable).Padding(FMargin(0.5f))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Center)
			  .FillWidth(.3f)
			[
				ConstructAssetTextBlock(DisplayClassName, SmallFont)
			]
			
			// Name
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Fill)
			  .FillWidth(1)
			[
				ConstructTextBlock(DisplayAssetName, BigFont)
			]
			
			//2. Path	
			// + SHorizontalBox::Slot()
			//   .HAlign(HAlign_Center)
			//   .VAlign(VAlign_Center)
			//   .FillWidth(.5f)
			// [
			// 	ConstructTextBlock(DisplayClassName, AssetClassFont)
			// ]
			
			// 3. delete button
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Fill)
			.AutoWidth()
			[
				ConstructAssetButton(AssetDataToDisplay)
			]

		];

	return ListViewRowWidget;
}

TSharedRef<STextBlock> SBookMarkAssetTab::ConstructTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
	TSharedRef<STextBlock> ConstructTextBlock =
	SNew(STextBlock)
	.Margin(5.0f)
	.Text(FText::FromString(TextContent))
	.Font(FontToUse)
	.ColorAndOpacity(FColor::White);
	return ConstructTextBlock;
}

TSharedRef<STextBlock> SBookMarkAssetTab::ConstructAssetTextBlock(const FString& TextContent,
	const FSlateFontInfo& FontToUse)
{
	FAssetData AssetData = IAssetRegistry::Get()->GetAssetByObjectPath(TextContent);
	FString ClassName = AssetData.AssetClassPath.GetAssetName().ToString();
	
	TSharedRef<STextBlock> ConstructTextBlock =
	SNew(STextBlock)
	.Margin(5.0f)
	.Text(FText::FromString(ClassName))
	.Font(FontToUse)
	.ColorAndOpacity(FColor::White);
	return ConstructTextBlock;
}

TSharedRef<SButton> SBookMarkAssetTab::ConstructAssetButton(
	const TSharedPtr<FAssetBookmarksContainer>& AssetDataToDisplay)
{
	TSharedRef<SButton> Button =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this, &SBookMarkAssetTab::OnDeleteAssetButtonClicked,AssetDataToDisplay);

	return Button;
}


void SBookMarkAssetTab::OnAssetRowWidgetClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData)
{
	TArray<FString> AssetsPathToSync;

	for (auto Element : AssetListView->GetSelectedItems())
	{
		AssetsPathToSync.Add(Element->Path.ToString());
		if (FSlateApplication::Get().GetModifierKeys().IsShiftDown())
		{
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Element->Path.ToString());
		}
		else
		{
			UEditorAssetLibrary::SyncBrowserToObjects(AssetsPathToSync);
		}
	}
	
}

void SBookMarkAssetTab::GetStoredAssetData()
{
	StoredAssetData.Empty();
	if (Bookmarks != nullptr &&Bookmarks->AssetList.Num()>0)
	{
		for (const auto& Asset : Bookmarks->AssetList)
		{
			if (Asset.Name!="" && Asset.Path != nullptr)
			{
				StoredAssetData.Emplace(new FAssetBookmarksContainer(Asset));
			}
		}
	}
}

FReply SBookMarkAssetTab::OpenSettingDataAsset()
{

	if (Bookmarks.Get())
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Bookmarks.Get());
	}
	return FReply::Handled();
}


FReply SBookMarkAssetTab::OnDeleteAssetButtonClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData)
{
	if (StoredAssetData.Contains(ClickedData))
	{
		if (Bookmarks.Get())
		{
			Bookmarks->Modify();
			Bookmarks->AssetList.RemoveAt(StoredAssetData.Find(ClickedData));
		}
		StoredAssetData.Remove(ClickedData);
		Bookmarks->StorePath.Remove(ClickedData->Path.ToString());
	}
	
	RefreshAllListView();
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
