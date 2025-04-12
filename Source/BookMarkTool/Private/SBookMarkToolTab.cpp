// Fill out your copyright notice in the Description page of Project Settings.


#include "SBookMarkToolTab.h"
#include "SlateOptMacros.h"
#include "BookMarkDataAsset.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "EditorDialogLibrary.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/IAssetRegistry.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBookMarkToolTab::Construct(const FArguments& InArgs)
{
	FSlateFontInfo FontInfo = GetFontSytle();
	FontInfo.Size = 12;
	//加载数据资产
	LoadBookMarkDataAsset();
	
	ChildSlot
	[
		// main slot
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SButton)
			.ContentPadding(FMargin(4.0, 2.0))
			.Text(FText::FromString(TEXT("打开配置资产")))
			.OnClicked(this, &SBookMarkToolTab::OpenSettingDataAsset)
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
				.Text(FText::FromString(TEXT("资产(Shift打开)")))
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
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			// SNew(SSeparator)
			// .Thickness(2.0f) // 线宽
			// .Orientation(Orient_Vertical) // 设置为垂直方向
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
				.Text(FText::FromString(TEXT("文件夹")))
				.Font(FontInfo)
				.Margin(5.0f)
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

		// FoldList
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				ConstructFoldPathListView()
			]
		]
	];
	
}

TSharedRef<SListView<TSharedPtr<FFoldPathBookmarksContainer>>> SBookMarkToolTab::ConstructFoldPathListView()
{
	FoldPathListView =
	SNew(SListView<TSharedPtr<FFoldPathBookmarksContainer>>)
	.ItemHeight(24.f)
	.SelectionMode(ESelectionMode::Single) // 单选
	.ListItemsSource(&StoredFoldPath) // this is a TArray of above TSharedPtr<FAssetData>
	.OnGenerateRow(this, &SBookMarkToolTab::OnGenerateRowForFoldPathList)
	.OnMouseButtonClick(this,&SBookMarkToolTab::OnPathCRowWidgetClicked);;
	// A Pointer can convert to be a ref.
	return FoldPathListView.ToSharedRef();
}

void SBookMarkToolTab::LoadBookMarkDataAsset()
{
	//加载资源
	Bookmarks = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));

	if (Bookmarks == nullptr)
	{
		UEditorDialogLibrary::ShowMessage(
		FText::FromString("警告"),
		FText::FromString(TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset资源不存在")),
		EAppMsgType::OkCancel
		);
		return;
	}
	Bookmarks->ReSetStorePath();
	
	//填充数据
	GetStoredFoldPath();
	
	//填充数据
	GetStoredAssetData();
}

TSharedRef<ITableRow> SBookMarkToolTab::OnGenerateRowForFoldPathList(TSharedPtr<FFoldPathBookmarksContainer> FoldPathDataToDisplay,
                                                                     const TSharedRef<STableViewBase>& OwnerTable)
{
	//check,  in case to return a None ptr
	if (!FoldPathDataToDisplay.IsValid())
	{
		return SNew(STableRow<TSharedPtr<FFoldPathBookmarksContainer>>, OwnerTable);
	}

	//main
	const FString DisplayName = FoldPathDataToDisplay->Name;
	const FString DisplayPathName = FoldPathDataToDisplay->Path.Path;
	FSlateFontInfo BigFont = GetFontSytle();
	BigFont.Size = 10;

	// define each row(SHorizontalBox= CheckBox + textbox + button) of the OwnerTable
	// return a ref of this row function as a widget
	TSharedRef<STableRow<TSharedPtr<FFoldPathBookmarksContainer>>> ListViewRowWidget =
		SNew(STableRow<TSharedPtr<FFoldPathBookmarksContainer>>, OwnerTable).Padding(FMargin(0.5f))
		[
			SNew(SHorizontalBox)
			
			// Name
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Fill)
			.FillWidth(1)
			[
				ConstructTextBlock(DisplayPathName, BigFont)
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
			 	ConstructPathButton(FoldPathDataToDisplay)
			 ]

		];

	return ListViewRowWidget;
}


TSharedRef<SListView<TSharedPtr<FAssetBookmarksContainer>>> SBookMarkToolTab::ConstructAssetListView()
{
	AssetListView =
	SNew(SListView<TSharedPtr<FAssetBookmarksContainer>>)
	.ItemHeight(24.f)
	.SelectionMode(ESelectionMode::Single)
	.ListItemsSource(&StoredAssetData) // this is a TArray of above TSharedPtr<FAssetData>
	.OnGenerateRow(this, &SBookMarkToolTab::OnGenerateRowForAssetList)
	.OnMouseButtonClick(this,&SBookMarkToolTab::OnAssetRowWidgetClicked);
	// A Pointer can convert to be a ref.
	return AssetListView.ToSharedRef();
}

void SBookMarkToolTab::RefreshAllListView()
{
	if (Bookmarks)
	{
		UEditorAssetLibrary::SaveLoadedAsset(Bookmarks);
		Bookmarks->ClearGarbage();
		Bookmarks = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));

		GetStoredFoldPath();
		GetStoredAssetData();
	}
	
	AssetListView->RebuildList();
	FoldPathListView->RebuildList();
}

TSharedRef<ITableRow> SBookMarkToolTab::OnGenerateRowForAssetList(
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

TSharedRef<STextBlock> SBookMarkToolTab::ConstructTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
	TSharedRef<STextBlock> ConstructTextBlock =
	SNew(STextBlock)
	.Margin(5.0f)
	.Text(FText::FromString(TextContent))
	.Font(FontToUse)
	.ColorAndOpacity(FColor::White);
	return ConstructTextBlock;
}

TSharedRef<STextBlock> SBookMarkToolTab::ConstructAssetTextBlock(const FString& TextContent,
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

TSharedRef<SButton> SBookMarkToolTab::ConstructPathButton(const TSharedPtr<FFoldPathBookmarksContainer>& AssetDataToDisplay)
{
	TSharedRef<SButton> Button =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this, &SBookMarkToolTab::OnDeletePathButtonClicked,AssetDataToDisplay);

	return Button;
}

TSharedRef<SButton> SBookMarkToolTab::ConstructAssetButton(
	const TSharedPtr<FAssetBookmarksContainer>& AssetDataToDisplay)
{
	TSharedRef<SButton> Button =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this, &SBookMarkToolTab::OnDeleteAssetButtonClicked,AssetDataToDisplay);

	return Button;
}

void SBookMarkToolTab::OnPathCRowWidgetClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData)
{
	TArray<FString> AssetsPathToSync;

	for (auto Element : FoldPathListView->GetSelectedItems())
	{
		AssetsPathToSync.Add(Element->Path.Path);
	}
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().SyncBrowserToFolders( AssetsPathToSync, false, true );
}

void SBookMarkToolTab::OnAssetRowWidgetClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData)
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

void SBookMarkToolTab::GetStoredFoldPath()
{
	StoredFoldPath.Empty();
	if (Bookmarks != nullptr && Bookmarks->FoldPathList.Num()>0)
	{
		// Bookmarks->FoldPathList[handle.index]
		for (const auto& FoldPath : Bookmarks->FoldPathList)
		{
			if (FoldPath.Name!="" && FoldPath.Path.Path !="")
			{
				StoredFoldPath.Emplace(new FFoldPathBookmarksContainer(FoldPath));
			}
				
		}
	}
}

void SBookMarkToolTab::GetStoredAssetData()
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

FReply SBookMarkToolTab::OpenSettingDataAsset()
{

	if (Bookmarks)
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Bookmarks);
	}
	return FReply::Handled();
}

FReply SBookMarkToolTab::OnDeletePathButtonClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData)
{
	if (StoredFoldPath.Contains(ClickedData))
	{
		if (Bookmarks)
		{
			Bookmarks->Modify();
			Bookmarks->FoldPathList.RemoveAt(StoredFoldPath.Find(ClickedData));
		}
		StoredFoldPath.Remove(ClickedData);
		Bookmarks->StorePath.Remove(ClickedData->Path.Path);
	}
	
	RefreshAllListView();
	return FReply::Handled();
}

FReply SBookMarkToolTab::OnDeleteAssetButtonClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData)
{
	if (StoredAssetData.Contains(ClickedData))
	{
		if (Bookmarks)
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
