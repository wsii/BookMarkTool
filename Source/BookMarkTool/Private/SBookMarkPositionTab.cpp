// Fill out your copyright notice in the Description page of Project Settings.


#include "SBookMarkPositionTab.h"
#include "SlateOptMacros.h"
#include "BookMarkDataAsset.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "EditorDialogLibrary.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/IAssetRegistry.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBookMarkPositionTab::Construct(const FArguments& InArgs)
{
	FSlateFontInfo FontInfo = GetFontSytle();
	FontInfo.Size = 12;
	//加载数据资产
	LoadBookMarkDataAsset();

	FTextKey LLOCTEXT_NAMESPACE;
	ChildSlot
	[
		SNew(SBox)
		.MaxDesiredWidth(1600.0f)
		.MaxDesiredHeight(200.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(5.0f)
			[
				// main slot
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SButton)
					.ContentPadding(FMargin(4.0, 2.0))
					.Text(FText::FromString(TEXT("打开配置资产")))
					.OnClicked(this, &SBookMarkPositionTab::OpenSettingDataAsset)
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
						.Text(FText::FromString(TEXT("世界位置")))
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
			+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Bottom) // 将按钮固定到底部
				.Padding(5.0f)
				[
					SNew(SBox)
					.HeightOverride(50.0f)
					[
						SNew(SHorizontalBox)
						// + SHorizontalBox::Slot()
						// .HAlign(HAlign_Left)
						// .VAlign(VAlign_Fill)
						// [
						// 	SNew(STextBlock)
						// 	.Text(FText::FromString(TEXT("位置名称")))
						// ]
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SEditableTextBox)
						  // .Text(LOCTEXT("DefaultText", "Enter text here..."))  // 设置默认文本
						  .HintText(LOCTEXT("HintText", "请输入要保存的坐标名称")) // 设置提示文本
						  // .OnTextChanged(this, &SMyWidget::HandleTextChanged) // 文本变化回调
						  // .OnTextCommitted(this, &SMyWidget::HandleTextCommitted) // 文本提交回调
						  .Font(FAppStyle::GetFontStyle("NormalFont")) // 设置字体
						  .SelectAllTextWhenFocused(true) // 获取焦点时全选文本
						  .ClearKeyboardFocusOnCommit(false) // 提交后是否保持焦点
							
						]
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.OnClicked(this, &SBookMarkPositionTab::OpenSettingDataAsset)
							.ButtonStyle(FAppStyle::Get(),"FlatButton.Success")
							.Content()
							[
								SNew(STextBlock)
								.Text(FText::FromString(TEXT("添加位置")))
							]
						]
					]
				]
		]
		
	];
	
}

TSharedRef<SListView<TSharedPtr<FFoldPathBookmarksContainer>>> SBookMarkPositionTab::ConstructFoldPathListView()
{
	FoldPathListView =
	SNew(SListView<TSharedPtr<FFoldPathBookmarksContainer>>)
	.SelectionMode(ESelectionMode::Single) // 单选
	.ListItemsSource(&StoredFoldPath) // this is a TArray of above TSharedPtr<FAssetData>
	.OnGenerateRow(this, &SBookMarkPositionTab::OnGenerateRowForFoldPathList)
	.OnMouseButtonClick(this,&SBookMarkPositionTab::OnPathCRowWidgetClicked);;
	// A Pointer can convert to be a ref.
	return FoldPathListView.ToSharedRef();
}

void SBookMarkPositionTab::LoadBookMarkDataAsset()
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
	
	//填充数据
	GetStoredAssetData();
}

TSharedRef<ITableRow> SBookMarkPositionTab::OnGenerateRowForFoldPathList(TSharedPtr<FFoldPathBookmarksContainer> FoldPathDataToDisplay,
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


TSharedRef<SListView<TSharedPtr<FAssetBookmarksContainer>>> SBookMarkPositionTab::ConstructAssetListView()
{
	AssetListView =
	SNew(SListView<TSharedPtr<FAssetBookmarksContainer>>)
	.SelectionMode(ESelectionMode::Single)
	.ListItemsSource(&StoredAssetData) // this is a TArray of above TSharedPtr<FAssetData>
	.OnGenerateRow(this, &SBookMarkPositionTab::OnGenerateRowForAssetList)
	.OnMouseButtonClick(this,&SBookMarkPositionTab::OnAssetRowWidgetClicked);
	// A Pointer can convert to be a ref.
	return AssetListView.ToSharedRef();
}

void SBookMarkPositionTab::RefreshAllListView()
{
	if (Bookmarks.Get())
	{
		UEditorAssetLibrary::SaveLoadedAsset(Bookmarks.Get());
		Bookmarks->ClearGarbage();
		Bookmarks = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));

		GetStoredFoldPath();
		GetStoredAssetData();
	}
	
	AssetListView->RebuildList();
	FoldPathListView->RebuildList();
}

TSharedRef<ITableRow> SBookMarkPositionTab::OnGenerateRowForAssetList(
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

TSharedRef<STextBlock> SBookMarkPositionTab::ConstructTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
	TSharedRef<STextBlock> ConstructTextBlock =
	SNew(STextBlock)
	.Margin(5.0f)
	.Text(FText::FromString(TextContent))
	.Font(FontToUse)
	.ColorAndOpacity(FColor::White);
	return ConstructTextBlock;
}

TSharedRef<STextBlock> SBookMarkPositionTab::ConstructAssetTextBlock(const FString& TextContent,
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

TSharedRef<SButton> SBookMarkPositionTab::ConstructPathButton(const TSharedPtr<FFoldPathBookmarksContainer>& AssetDataToDisplay)
{
	TSharedRef<SButton> Button =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this, &SBookMarkPositionTab::OnDeletePathButtonClicked,AssetDataToDisplay);

	return Button;
}

TSharedRef<SButton> SBookMarkPositionTab::ConstructAssetButton(
	const TSharedPtr<FAssetBookmarksContainer>& AssetDataToDisplay)
{
	TSharedRef<SButton> Button =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this, &SBookMarkPositionTab::OnDeleteAssetButtonClicked,AssetDataToDisplay);

	return Button;
}

void SBookMarkPositionTab::OnPathCRowWidgetClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData)
{
	TArray<FString> AssetsPathToSync;

	for (auto Element : FoldPathListView->GetSelectedItems())
	{
		AssetsPathToSync.Add(Element->Path.Path);
	}
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().SyncBrowserToFolders( AssetsPathToSync, false, true );
}

void SBookMarkPositionTab::OnAssetRowWidgetClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData)
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

void SBookMarkPositionTab::GetStoredFoldPath()
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

void SBookMarkPositionTab::GetStoredAssetData()
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

FReply SBookMarkPositionTab::OpenSettingDataAsset()
{

	if (Bookmarks.Get())
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Bookmarks.Get());
	}
	return FReply::Handled();
}

FReply SBookMarkPositionTab::OnDeletePathButtonClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData)
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

FReply SBookMarkPositionTab::OnDeleteAssetButtonClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData)
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
