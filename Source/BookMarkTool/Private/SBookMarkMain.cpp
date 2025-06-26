// Fill out your copyright notice in the Description page of Project Settings.


#include "SBookMarkMain.h"

#include "Editor.h"
#include "GameFramework/Actor.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "EditorDialogLibrary.h"
#include "EngineUtils.h"
#include "IAssetViewport.h"
#include "IContentBrowserSingleton.h"
#include "LevelEditor.h"
#include "Selection.h"
#include "SlateOptMacros.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Misc/App.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "BookMarkMain"

void SBookMarkMain::Construct(const FArguments& InArgs)
{
	FSlateFontInfo FontInfo = GetFontSytle();
	FontInfo.Size = 12;
	//加载数据资产
	LoadBookMarkDataAsset();
	
	ChildSlot
	[
	
		SNew(SBox)
		.MaxDesiredWidth(1400.0f)
		.MaxDesiredHeight(500.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SSplitter)
			//left
			.PhysicalSplitterHandleSize(3.0f)
			.Style(FAppStyle::Get(), "DetailsView.Splitter")
			+ SSplitter::Slot().Value(0.3f)
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(5.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SButton)
						.ContentPadding(FMargin(4.0, 2.0))
						.Text(FText::FromString(TEXT("打开设置")))
						.OnClicked(this, &SBookMarkMain::OpenSettingDataAsset)
					]
					+ SVerticalBox::Slot()
					.MaxHeight(40)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Bottom)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("资产(+Shift单击打开)")))
							.Margin(5.0f)
							.Font(FontInfo)
						]
					
					]
					+ SVerticalBox::Slot()
					.MinHeight(1000)
					.FillHeight(1.0f)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SScrollBox)
						+SScrollBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							ConstructAssetListView()
						]
					]
				]
			]

			//Center
			+ SSplitter::Slot().Value(0.3f)
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(5.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.MaxHeight(60)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Bottom)
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("文件夹")))
							.Margin(5.0f)
							.Font(FontInfo)
						]
					]
					+ SVerticalBox::Slot()
					.MinHeight(1000)
					.FillHeight(1.0f)
					[
						SNew(SScrollBox)
						+SScrollBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							ConstructFoldPathListView()
						]
					]
				]
			]
			
			//Right
			+ SSplitter::Slot().Value(0.4f)
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Bottom)
				.Padding(5.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SButton)
						.ContentPadding(FMargin(4.0, 2.0))
						.Text(FText::FromString(TEXT("刷新")))
						.OnClicked(this, &SBookMarkMain::RefreshAll)
					]
					+ SVerticalBox::Slot()
					.MaxHeight(40)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Bottom)
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("世界位置")))
							.Font(FontInfo)
							.Margin(5.0f)
						]
					]
					+ SVerticalBox::Slot()
					.MinHeight(1000)
					.FillHeight(1.0f)
					[
						SNew(SScrollBox)
						+SScrollBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							ConstructPositionListView()
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
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SEditableTextBox)
						  // .Text(LOCTEXT("DefaultText", "Enter text here..."))  // 设置默认文本
						  .HintText(LOCTEXT("HintText", "请输入要保存的坐标名称")) // 设置提示文本
						  .OnTextChanged(this, &SBookMarkMain::HandleTextChanged) // 文本变化回调
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
							.OnClicked(this, &SBookMarkMain::AddPostion)
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
		]
		
	];
}

void SBookMarkMain::RefreshAllListView()
{
	if (Bookmarks.Get())
	{
		Bookmarks->Modify();
		bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(Bookmarks.Get());
		if (!bSaved)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save BookMarkDataAsset!"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("BookMarkDataAsset saved successfully!"));
		}
		Bookmarks->ClearGarbage();
		Bookmarks = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));

		GetStoredData();
	}
	
	PositionListView->RebuildList();
}

TSharedRef<SListView<TSharedPtr<FAssetBookmarksContainer>>> SBookMarkMain::ConstructAssetListView()
{
	AssetListView =
	SNew(SListView<TSharedPtr<FAssetBookmarksContainer>>)
	.SelectionMode(ESelectionMode::Single)
	.ListItemsSource(&StoredAssetData) // this is a TArray of above TSharedPtr<FAssetData>
	.OnGenerateRow(this, &SBookMarkMain::OnGenerateRowForAssetList)
	.OnMouseButtonClick(this,&SBookMarkMain::OnAssetRowWidgetClicked);
	// A Pointer can convert to be a ref.
	return AssetListView.ToSharedRef();
}

TSharedRef<SListView<TSharedPtr<FFoldPathBookmarksContainer>>> SBookMarkMain::ConstructFoldPathListView()
{
	FoldPathListView =
	SNew(SListView<TSharedPtr<FFoldPathBookmarksContainer>>)
	.SelectionMode(ESelectionMode::Single) // 单选
	.ListItemsSource(&StoredFoldPath) // this is a TArray of above TSharedPtr<FAssetData>
	.OnGenerateRow(this, &SBookMarkMain::OnGenerateRowForFoldPathList)
	.OnMouseButtonClick(this,&SBookMarkMain::OnPathCRowWidgetClicked);;
	// A Pointer can convert to be a ref.
	return FoldPathListView.ToSharedRef();
}

TSharedRef<SListView<TSharedPtr<FPositionBookmarksContainer>>> SBookMarkMain::ConstructPositionListView()
{
	PositionListView =
	SNew(SListView<TSharedPtr<FPositionBookmarksContainer>>)
	.SelectionMode(ESelectionMode::Single) // 单选
	.ListItemsSource(&StoredPosition) // this is a TArray of above TSharedPtr<FAssetData>
	.OnGenerateRow(this, &SBookMarkMain::OnGenerateRowForPositionList)
	.OnMouseButtonClick(this,&SBookMarkMain::OnPositionRowWidgetClicked);;
	// A Pointer can convert to be a ref.
	return PositionListView.ToSharedRef();
}

TSharedRef<ITableRow> SBookMarkMain::OnGenerateRowForAssetList(TSharedPtr<FAssetBookmarksContainer> AssetDataToDisplay,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	//check,  in case to return a None ptr
	if (!AssetDataToDisplay.IsValid())
	{
		return SNew(STableRow<TSharedPtr<FAssetBookmarksContainer>>, OwnerTable);
	}
	
	//main
	const FString DisplayAssetName = AssetDataToDisplay->Name;
	const FString DisplayClassName = AssetDataToDisplay->Path.ToString();
	// FSlateFontInfo SmallFont = GetFontSytle();
	// SmallFont.Size = 8;
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
				ConstructNameTextBlock(DisplayAssetName, BigFont)
			]
			
			
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

TSharedRef<ITableRow> SBookMarkMain::OnGenerateRowForFoldPathList(
	TSharedPtr<FFoldPathBookmarksContainer> FoldPathDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
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
				ConstructNameTextBlock(DisplayName, BigFont)
			]
			
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

TSharedRef<ITableRow> SBookMarkMain::OnGenerateRowForPositionList(
	TSharedPtr<FPositionBookmarksContainer> PositionDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	//check,  in case to return a None ptr
	if (!PositionDataToDisplay.IsValid())
	{
		return SNew(STableRow<TSharedPtr<FPositionBookmarksContainer>>, OwnerTable);
	}

	//main
	const FString DisplayName = PositionDataToDisplay->Name;
	const FString DisplayPathName = PositionDataToDisplay->MapPath.ToString();
	FSlateFontInfo BigFont = GetFontSytle();
	BigFont.Size = 10;

	// define each row(SHorizontalBox= CheckBox + textbox + button) of the OwnerTable
	// return a ref of this row function as a widget
	TSharedRef<STableRow<TSharedPtr<FPositionBookmarksContainer>>> ListViewRowWidget =
		SNew(STableRow<TSharedPtr<FPositionBookmarksContainer>>, OwnerTable).Padding(FMargin(0.5f))
		[
			SNew(SHorizontalBox)
			
			// Name
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Fill)
			.FillWidth(1)
			[
				ConstructNameTextBlock(DisplayName, BigFont)
			]

			// 2. copy button
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Fill)
			.AutoWidth()
			[
				ConstructPositionCopyButton(PositionDataToDisplay)
			]
			 // 3. delete button
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Fill)
			.AutoWidth()
			 [
				 ConstructPositionButton(PositionDataToDisplay)
			 ]

		];

	return ListViewRowWidget;
}

TSharedRef<STextBlock> SBookMarkMain::ConstructNameTextBlock(const FString& TextContent,
	const FSlateFontInfo& FontToUse)
{
	TSharedRef<STextBlock> ConstructTextBlock =
	SNew(STextBlock)
	.Margin(5.0f)
	.Text(FText::FromString(TextContent))
	.Font(FontToUse)
	.ColorAndOpacity(FColor::White);
	return ConstructTextBlock;
}


TSharedRef<SButton> SBookMarkMain::ConstructAssetButton(const TSharedPtr<FAssetBookmarksContainer>& AssetDataToDisplay)
{
	TSharedRef<SButton> Button =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this, &SBookMarkMain::OnDeleteAssetButtonClicked,AssetDataToDisplay);

	return Button;
}

TSharedRef<SButton> SBookMarkMain::ConstructPathButton(
	const TSharedPtr<FFoldPathBookmarksContainer>& FoldPathDataToDisplay)
{
	TSharedRef<SButton> Button =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this, &SBookMarkMain::OnDeletePathButtonClicked,FoldPathDataToDisplay);

	return Button;
}

TSharedRef<SButton> SBookMarkMain::ConstructPositionButton(
	const TSharedPtr<FPositionBookmarksContainer>& PositionDataToDisplay)
{
	TSharedRef<SButton> Button =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this, &SBookMarkMain::OnDeletePositionButtonClicked,PositionDataToDisplay);

	return Button;
}

TSharedRef<SButton> SBookMarkMain::ConstructPositionCopyButton(
	const TSharedPtr<FPositionBookmarksContainer>& PositionDataToDisplay)
{
	TSharedRef<SButton> Button =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Apply")))
	.OnClicked(this, &SBookMarkMain::OnCopyPositionButtonClicked,PositionDataToDisplay);

	return Button;
}

void SBookMarkMain::OnAssetRowWidgetClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData)
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

void SBookMarkMain::OnPathCRowWidgetClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData)
{
	TArray<FString> AssetsPathToSync;

	for (auto Element : FoldPathListView->GetSelectedItems())
	{
		AssetsPathToSync.Add(Element->Path.Path);
	}
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().SyncBrowserToFolders( AssetsPathToSync, false, true );
}

void SBookMarkMain::OnPositionRowWidgetClicked(TSharedPtr<FPositionBookmarksContainer> ClickedData)
{
	// 获取LevelEditor模块
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	// 获取当前活动的视口
	TSharedPtr<IAssetViewport> ActiveViewport = LevelEditorModule.GetFirstActiveViewport();


	if (ActiveViewport.IsValid())
	{
		FEditorViewportClient& ViewportClient = ActiveViewport->GetAssetViewportClient();
		FVector TempLocation = ClickedData->WorldPosition.GetLocation();

		// 计算摄像机位置（稍微后退，避免摄像机卡进模型）
		FVector CameraOffset = ViewportClient.GetViewRotation().Vector() * -1.0f; // 摄像机朝向的反方向
		FVector TargetCameraLocation = TempLocation + (CameraOffset * 100 * 2.0f); // 2倍半径距离
		
		// 设置摄像机位置和旋转

		ViewportClient.SetViewLocation(TargetCameraLocation);

		// 可选：调整摄像机距离（如稍微后退）
		ViewportClient.SetViewLocation(TempLocation - ViewportClient.GetViewRotation().Vector() * 300.0f);
        
		// 刷新视口
		ViewportClient.Invalidate();
	}
}


void SBookMarkMain::LoadBookMarkDataAsset()
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
	GetStoredData();
}

void SBookMarkMain::GetStoredData()
{
	StoredAssetData.Empty();
	StoredFoldPath.Empty();
	StoredPosition.Empty();

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
	if (Bookmarks != nullptr && Bookmarks->PositionList.Num()>0)
	{
		UWorld* World = GEditor->GetEditorWorldContext().World();
		for (const auto& Position : Bookmarks->PositionList)
		{
			if (Position.Name!="" && Position.MapPath.ToString()!=""&& Position.MapPath.ToString().Equals(World->GetOuter()->GetPathName()))
			{
				StoredPosition.Emplace(new FPositionBookmarksContainer(Position));
			}
			
		}
	}

	
	
}

void SBookMarkMain::HandleTextChanged(const FText& InText)
{
	CurrentActorName = InText.ToString();
}

FReply SBookMarkMain::AddPostion()
{
	if (Bookmarks.Get())
	{
		UEditorAssetLibrary::SaveLoadedAsset(Bookmarks.Get());
		Bookmarks->ClearGarbage();
		Bookmarks = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));

		GetStoredData();
	}
	if (UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		USelection* SelectedActors = GEditor->GetSelectedActors();
		if (SelectedActors->Num() >0 )
		{

			FPositionBookmarksContainer PositionBookmarksContainer;
			if (CurrentActorName != "")
			{
				PositionBookmarksContainer.Name = CurrentActorName;
			}
			else
			{
				PositionBookmarksContainer.Name =  Cast<AActor>(SelectedActors->GetSelectedObject(0))->GetActorLabel();
			}
			PositionBookmarksContainer.WorldPosition = Cast<AActor>(SelectedActors->GetSelectedObject(0))->GetTransform();
			PositionBookmarksContainer.MapPath = World->GetOuter()->GetPathName();

			if(Bookmarks->PositionList.Add(PositionBookmarksContainer))
			{
				// Notify user that the bookmark was added
				FNotificationInfo Info(LOCTEXT("BookMarkPositionTab", "Added current Postion to  Bookmarks."));
				Info.ExpireDuration = 5.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
			}
		}
		
	}

	// 3. 保存
	bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(Bookmarks.Get());

	if (!bSaved)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save BookMarkDataAsset!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("BookMarkDataAsset saved successfully!"));
	}

	// 刷新界面
	RefreshAllListView();
	
	return FReply::Handled();
}

FReply SBookMarkMain::OpenSettingDataAsset()
{
	if (Bookmarks.Get())
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Bookmarks.Get());
	}
	return FReply::Handled();
}

FReply SBookMarkMain::RefreshAll()
{
	RefreshAllListView();
	return FReply::Handled();
}

FReply SBookMarkMain::OnDeleteAssetButtonClicked(TSharedPtr<FAssetBookmarksContainer> ClickedData)
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

FReply SBookMarkMain::OnDeletePathButtonClicked(TSharedPtr<FFoldPathBookmarksContainer> ClickedData)
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

FReply SBookMarkMain::OnDeletePositionButtonClicked(TSharedPtr<FPositionBookmarksContainer> ClickedData)
{
	if (StoredPosition.Contains(ClickedData))
	{
		if (Bookmarks.Get())
		{
			Bookmarks->Modify();
			Bookmarks->PositionList.RemoveAt(StoredPosition.Find(ClickedData));
		}
		StoredPosition.Remove(ClickedData);
		// Bookmarks->StorePath.Remove(ClickedData->Path.Path);
	}
	
	RefreshAllListView();
	return FReply::Handled();
}

FReply SBookMarkMain::OnCopyPositionButtonClicked(TSharedPtr<FPositionBookmarksContainer> ClickedData)
{
	if (StoredPosition.Contains(ClickedData))
	{

		if (GEditor->GetEditorWorldContext().World())
		{
			USelection* SelectedActors = GEditor->GetSelectedActors();
			
			TArray<AActor*> SelectedActorsArray;
			SelectedActors->GetSelectedObjects<AActor>(SelectedActorsArray);
			if (SelectedActors && SelectedActors->Num() > 0)
			{
				// 开始事务（支持撤销）
				GEditor->BeginTransaction(LOCTEXT("PasteTransform", "Paste Transform"));
            
				for (AActor* Actor : SelectedActorsArray)
				{
					Actor->Modify();
					Actor->SetActorTransform(ClickedData->WorldPosition);
					Actor->PostEditMove(true);
				}

				// 结束事务
				GEditor->EndTransaction();
            
				// 刷新视口
				GEditor->RedrawLevelEditingViewports();
			}
		
		}
		// StoredPosition.Find(ClickedData);
		// 将transform转换为字符串

		// 复制到剪切板
		// FPlatformApplicationMisc::ClipboardCopy(*ContentToCopy);
		// 可选：显示通知
		// FNotificationInfo Info(FText::FromString("Transform copied to clipboard"));
		// Info.ExpireDuration = 3.0f;
		// FSlateNotificationManager::Get().AddNotification(Info);
	}
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
