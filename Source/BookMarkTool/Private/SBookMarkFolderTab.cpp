// Fill out your copyright notice in the Description page of Project Settings.


#include "SBookMarkFolderTab.h"
#include "SlateOptMacros.h"
#include "BookMarkDataAsset.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "EditorDialogLibrary.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/IAssetRegistry.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBookMarkFolderTab::Construct(const FArguments& InArgs)
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
				.OnClicked(this, &SBookMarkFolderTab::OpenSettingDataAsset)
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
		]
		
	];
	
}

TSharedRef<SListView<TSharedPtr<FFoldPathBookmarksContainer>>> SBookMarkFolderTab::ConstructFoldPathListView()
{
	FoldPathListView =
	SNew(SListView<TSharedPtr<FFoldPathBookmarksContainer>>)
	.SelectionMode(ESelectionMode::Single) // 单选
	.ListItemsSource(&StoredFoldPath) // this is a TArray of above TSharedPtr<FAssetData>
	.OnGenerateRow(this, &SBookMarkFolderTab::OnGenerateRowForFoldPathList)
	.OnMouseButtonClick(this,&SBookMarkFolderTab::OnPathCRowWidgetClicked);;
	// A Pointer can convert to be a ref.
	return FoldPathListView.ToSharedRef();
}

void SBookMarkFolderTab::LoadBookMarkDataAsset()
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
	GetStoredFoldPath();
	
}

TSharedRef<ITableRow> SBookMarkFolderTab::OnGenerateRowForFoldPathList(TSharedPtr<FFoldPathBookmarksContainer> FoldPathDataToDisplay,
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




void SBookMarkFolderTab::RefreshAllListView()
{
	if (UBookMarkDataAsset* BookmarkPtr = Bookmarks.Get())
	{
		UEditorAssetLibrary::SaveLoadedAsset(BookmarkPtr);
		Bookmarks->ClearGarbage();
		// 重新加载数据
		Bookmarks = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));

		GetStoredFoldPath();

	}
	

	FoldPathListView->RebuildList();
}


TSharedRef<STextBlock> SBookMarkFolderTab::ConstructTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
	TSharedRef<STextBlock> ConstructTextBlock =
	SNew(STextBlock)
	.Margin(5.0f)
	.Text(FText::FromString(TextContent))
	.Font(FontToUse)
	.ColorAndOpacity(FColor::White);
	return ConstructTextBlock;
}



TSharedRef<SButton> SBookMarkFolderTab::ConstructPathButton(const TSharedPtr<FFoldPathBookmarksContainer>& AssetDataToDisplay)
{
	TSharedRef<SButton> Button =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this, &SBookMarkFolderTab::OnDeletePathButtonClicked,AssetDataToDisplay);

	return Button;
}



void SBookMarkFolderTab::OnPathCRowWidgetClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData)
{
	TArray<FString> AssetsPathToSync;

	for (auto Element : FoldPathListView->GetSelectedItems())
	{
		AssetsPathToSync.Add(Element->Path.Path);
	}
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().SyncBrowserToFolders( AssetsPathToSync, false, true );
}


void SBookMarkFolderTab::GetStoredFoldPath()
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


FReply SBookMarkFolderTab::OpenSettingDataAsset()
{

	if (Bookmarks.Get())
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Bookmarks.Get());
	}
	return FReply::Handled();
}

FReply SBookMarkFolderTab::OnDeletePathButtonClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData)
{
	if (StoredFoldPath.Contains(ClickedData))
	{
		if (Bookmarks.Get())
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



END_SLATE_FUNCTION_BUILD_OPTIMIZATION
