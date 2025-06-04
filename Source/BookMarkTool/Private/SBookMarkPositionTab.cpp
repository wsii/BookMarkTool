// Fill out your copyright notice in the Description page of Project Settings.


#include "SBookMarkPositionTab.h"
#include "SlateOptMacros.h"
#include "BookMarkDataAsset.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "EditorDialogLibrary.h"
#include "IAssetViewport.h"
#include "IContentBrowserSingleton.h"
#include "LevelEditor.h"
#include "LevelEditorViewport.h"
#include "Selection.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "BookMarkPositionTab"

void SBookMarkPositionTab::Construct(const FArguments& InArgs)
{
	FSlateFontInfo FontInfo = GetFontSytle();
	FontInfo.Size = 12;
	//加载数据资产
	LoadBookMarkDataAsset();
	
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
						  .OnTextChanged(this, &SBookMarkPositionTab::HandleTextChanged) // 文本变化回调
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
							.OnClicked(this, &SBookMarkPositionTab::AddPostion)
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

TSharedRef<SListView<TSharedPtr<FPositionBookmarksContainer>>> SBookMarkPositionTab::ConstructPositionListView()
{
	PositionListView =
	SNew(SListView<TSharedPtr<FPositionBookmarksContainer>>)
	.SelectionMode(ESelectionMode::Single) // 单选
	.ListItemsSource(&StoredPosition) // this is a TArray of above TSharedPtr<FAssetData>
	.OnGenerateRow(this, &SBookMarkPositionTab::OnGenerateRowForPositionList)
	.OnMouseButtonClick(this,&SBookMarkPositionTab::OnPositionRowWidgetClicked);;
	// A Pointer can convert to be a ref.
	return PositionListView.ToSharedRef();
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
	GetStoredPosition();
	
}

TSharedRef<ITableRow> SBookMarkPositionTab::OnGenerateRowForPositionList(TSharedPtr<FPositionBookmarksContainer> PositionDataToDisplay,
                                                                     const TSharedRef<STableViewBase>& OwnerTable)
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
			 	ConstructPathButton(PositionDataToDisplay)
			 ]

		];

	return ListViewRowWidget;
}


void SBookMarkPositionTab::RefreshAllListView()
{
	if (Bookmarks.Get())
	{
		UEditorAssetLibrary::SaveLoadedAsset(Bookmarks.Get());
		Bookmarks->ClearGarbage();
		Bookmarks = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));

		GetStoredPosition();
	}
	
	PositionListView->RebuildList();
}

TSharedRef<STextBlock> SBookMarkPositionTab::ConstructNameTextBlock(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
	TSharedRef<STextBlock> ConstructTextBlock =
	SNew(STextBlock)
	.Margin(5.0f)
	.Text(FText::FromString(TextContent))
	.Font(FontToUse)
	.ColorAndOpacity(FColor::White);
	return ConstructTextBlock;
}

TSharedRef<STextBlock> SBookMarkPositionTab::ConstructPathTextBlock(const FString& TextContent,
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

TSharedRef<SButton> SBookMarkPositionTab::ConstructPathButton(const TSharedPtr<FPositionBookmarksContainer>& PositionDataToDisplay)
{
	TSharedRef<SButton> Button =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this, &SBookMarkPositionTab::OnDeletePositionButtonClicked,PositionDataToDisplay);

	return Button;
}


void SBookMarkPositionTab::OnPositionRowWidgetClicked(TSharedPtr<FPositionBookmarksContainer> ClickedData)
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


void SBookMarkPositionTab::GetStoredPosition()
{
	StoredPosition.Empty();
	if (UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		if (Bookmarks != nullptr && Bookmarks->PositionList.Num()>0)
		{
			// Bookmarks->FoldPathList[handle.index]
			for (const auto& Position : Bookmarks->PositionList)
			{
				if (Position.Name!="" && Position.MapPath.ToString()!="" && Position.MapPath.ToString().Equals(World->GetOuter()->GetPathName()) )
				{
					StoredPosition.Emplace(new FPositionBookmarksContainer(Position));
				}
				
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

FReply SBookMarkPositionTab::AddPostion()
{
	if (Bookmarks.Get())
	{
		UEditorAssetLibrary::SaveLoadedAsset(Bookmarks.Get());
		Bookmarks->ClearGarbage();
		Bookmarks = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));

		GetStoredPosition();
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

void SBookMarkPositionTab::HandleTextChanged(const FText& InText)
{
	CurrentActorName = InText.ToString();
}

FReply SBookMarkPositionTab::OnDeletePositionButtonClicked(TSharedPtr<FPositionBookmarksContainer> ClickedData)
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


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
