// Copyright Epic Games, Inc. All Rights Reserved.

#include "BookMarkTool.h"

#include "BookMarkDataAsset.h"
#include "BookMarkToolStyle.h"
#include "BookMarkToolCommands.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "IContentBrowserSingleton.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "SBookMarkAssetTab.h"
#include "SBookMarkFolderTab.h"
#include "SBookMarkPositionTab.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

static const FName BookMarkAssetTabName("BookMarkAsset");
static const FName BookMarkFolderTabName("BookMarkFolder");
static const FName BookMarkPositionTabName("BookMarkPosition");

#define LOCTEXT_NAMESPACE "FBookMarkToolModule"

void FBookMarkToolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FBookMarkToolStyle::Initialize();
	FBookMarkToolStyle::ReloadTextures();

	FBookMarkToolCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	// PluginCommands->MapAction(
	// 	FBookMarkToolCommands::Get().OpenBookMarkTool,
	// 	FExecuteAction::CreateRaw(this, &FBookMarkToolModule::PluginButtonClicked),
	// 	FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FBookMarkToolModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(BookMarkAssetTabName, FOnSpawnTab::CreateRaw(this, &FBookMarkToolModule::OnSpawnAssetTab))
		.SetDisplayName(LOCTEXT("FBookMarkToolTabTitle", "BookMarkAsset"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(BookMarkFolderTabName, FOnSpawnTab::CreateRaw(this, &FBookMarkToolModule::OnSpawnFolderTab))
	.SetDisplayName(LOCTEXT("FBookMarkToolTabTitle", "BookMarkFolder"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(BookMarkPositionTabName, FOnSpawnTab::CreateRaw(this, &FBookMarkToolModule::OnSpawnPositionTab))
	.SetDisplayName(LOCTEXT("FBookMarkToolTabTitle", "BookMarkPosition"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);


}

void FBookMarkToolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FBookMarkToolStyle::Shutdown();

	FBookMarkToolCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(BookMarkAssetTabName);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(BookMarkFolderTabName);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(BookMarkPositionTabName);
}

TSharedRef<SDockTab> FBookMarkToolModule::OnSpawnAssetTab(const FSpawnTabArgs& SpawnTabArgs)
{
	
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[

			SNew(SBox)
			// .HAlign(HAlign_Center)
			// .VAlign(VAlign_Center)
			[
				SAssignNew(BookMarkAssetTab,SBookMarkAssetTab)
			]
		];
}

TSharedRef<SDockTab> FBookMarkToolModule::OnSpawnFolderTab(const FSpawnTabArgs& SpawnTabArgs)
{
	
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[

			SNew(SBox)
			[
				SAssignNew(BookMarkFolderTab,SBookMarkFolderTab)
			]
		];
}

TSharedRef<SDockTab> FBookMarkToolModule::OnSpawnPositionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[

			SNew(SBox)
			[
				SAssignNew(BookMarkPositionTab,SBookMarkPositionTab)
			]
		];
}

void FBookMarkToolModule::OpenAssetMark() const
{
	FGlobalTabmanager::Get()->TryInvokeTab(BookMarkAssetTabName);
}

void FBookMarkToolModule::OpenFolderMark() const
{
	FGlobalTabmanager::Get()->TryInvokeTab(BookMarkFolderTabName);
}

void FBookMarkToolModule::OpenPositionMark() const
{
	FGlobalTabmanager::Get()->TryInvokeTab(BookMarkPositionTabName);
}

void FBookMarkToolModule::ExtendAssetContextMenu()
{
	UToolMenu* AssetMenu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
	if (!AssetMenu) return;

	// Insert Top
	// FToolMenuSection& Section = AssetMenu->AddSection(
	// "AssetContextBookmark", LOCTEXT("AddBookMarkHeading", "AddBookMark"),FToolMenuInsert(FName("CommonAssetActions"),EToolMenuInsertType::Before));

	FToolMenuSection& Section = AssetMenu->AddSection(
	"AssetContextBookmark", LOCTEXT("AddBookMarkHeading", "AddBookMark"));

	Section.AddMenuEntry(
		FName("AddBookMark"),
		LOCTEXT("AddAssetBookMark", "Add To BookMark"),
		LOCTEXT("AssetBookMarkTooltip", "Add Asset BookMark"),
		FSlateIcon(FBookMarkToolStyle::GetStyleSetName(),"BookMarkTool.OpenBookMarkTool"),
		FUIAction(FExecuteAction::CreateRaw(this, &FBookMarkToolModule::AddAssetPath)));
	
}

void FBookMarkToolModule::ExtendFolderContextMenu()
{
	UToolMenu* FolderMenu = UToolMenus::Get()->ExtendMenu("ContentBrowser.FolderContextMenu");
	if (!FolderMenu) return;
	
	// Insert Top
	// FToolMenuSection& Section = FolderMenu->AddSection(
	// 	"AddBookMark", LOCTEXT("AddBookMarkHeading", "AddBookMark"),FToolMenuInsert(FName("PathViewFolderOptions"),EToolMenuInsertType::Before));
	FToolMenuSection& Section = FolderMenu->AddSection(
		"AddBookMark", LOCTEXT("AddBookMarkHeading", "AddBookMark"));

	Section.AddMenuEntry(
		FName("AddFoldBookMark"),
		LOCTEXT("AddFoldBookMark", "Add To BookMark"),
		LOCTEXT("AddFoldBookMarkMenuTooltip", "Add Fold BookMark."),
		FSlateIcon(FBookMarkToolStyle::GetStyleSetName(),"BookMarkTool.OpenBookMarkTool"),
		FUIAction(FExecuteAction::CreateRaw(this, &FBookMarkToolModule::AddFoldPath)));
}

void FBookMarkToolModule::AddFoldPath()
{
	TWeakObjectPtr<UBookMarkDataAsset> BookmarkSettingsAsset;
	// // 加载 DataAsset
	BookmarkSettingsAsset = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));
	if (!BookmarkSettingsAsset.Get())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load BookMarkDataAsset!"));
		return;
	}

	// 刷新缓存
	BookmarkSettingsAsset->ReSetStorePath();
	
	BookmarkSettingsAsset->Modify();

	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	// 1. 获取左侧导航树选中的文件夹
	TArray<FString> NavigationFolders;
	ContentBrowserModule.Get().GetSelectedPathViewFolders(NavigationFolders);

	// 2. 获取右侧资源面板选中的文件夹
	TArray<FString> AssetViewFolders;
	ContentBrowserModule.Get().GetSelectedFolders(AssetViewFolders);

	// 合并结果
	TArray<FString> AllSelectedFolders;
	if (AssetViewFolders.Num()>0)
	{
		AllSelectedFolders.Append(AssetViewFolders);
	}
	else
	{
		AllSelectedFolders.Append(NavigationFolders);
	}
	
	if (AllSelectedFolders.Num() > 0)
	{
		for (auto Path : AllSelectedFolders)
		{
			if (BookmarkSettingsAsset->StorePath.Find(Path))
			{
				// Notify user that the bookmark is already in the list
				FNotificationInfo Info(LOCTEXT("BookmarksAlreadyInList", "Current Path is already in  Bookmarks."));
				Info.ExpireDuration = 5.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
				return;
			}
			BookmarkSettingsAsset->StorePath.Emplace(Path);
			
			FString FolderName = FPaths::GetCleanFilename(Path);
			FFoldPathBookmarksContainer NewFoldPathBookmarksContainer;
			NewFoldPathBookmarksContainer.Name = FolderName;
			NewFoldPathBookmarksContainer.Path.Path = Path;
			if(BookmarkSettingsAsset->FoldPathList.Add(NewFoldPathBookmarksContainer))
			{
			
				// Notify user that the bookmark was added
				FNotificationInfo Info(LOCTEXT("BookmarksAdded", "Added current Path to  Bookmarks."));
				Info.ExpireDuration = 5.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
			}
		}
	}

	// 3. 保存
	bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(BookmarkSettingsAsset.Get());

	if (!bSaved)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save BookMarkDataAsset!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("BookMarkDataAsset saved successfully!"));
	}

	// 刷新界面
	if (BookMarkAssetTab)
	{
		BookMarkAssetTab->RefreshAllListView();
	}
}

void FBookMarkToolModule::AddAssetPath()
{
	TWeakObjectPtr<UBookMarkDataAsset> BookmarkSettingsAsset;
	// // 加载 DataAsset
	BookmarkSettingsAsset = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));
	// 刷新缓存
	BookmarkSettingsAsset->ReSetStorePath();
	
	if (!BookmarkSettingsAsset.Get())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load BookMarkDataAsset!"));
		return;
	}

	BookmarkSettingsAsset->Modify();

	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);

	if (SelectedAssets.Num() > 0)
	{
		for (auto Asset : SelectedAssets)
		{
			FString AssetName = Asset.AssetName.ToString();
			FSoftObjectPath AssetSoftPath(Asset.ToSoftObjectPath());
			if (BookmarkSettingsAsset->StorePath.Find(AssetSoftPath.ToString()))
			{
				// Notify user that the bookmark is already in the list
				FNotificationInfo Info(LOCTEXT("BookmarksAlreadyInList", "Current Path is already in  Bookmarks."));
				Info.ExpireDuration = 5.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
				return;
			}
			BookmarkSettingsAsset->StorePath.Emplace(AssetSoftPath.ToString());
			
			FAssetBookmarksContainer NewAssetBookmarksContainer;
			NewAssetBookmarksContainer.Name = AssetName;
			NewAssetBookmarksContainer.Path = AssetSoftPath;
			
			if(BookmarkSettingsAsset->AssetList.Add(NewAssetBookmarksContainer))
			{
				// Notify user that the bookmark was added
				FNotificationInfo Info(LOCTEXT("BookmarksAdded", "Added current Asset to  Bookmarks."));
				Info.ExpireDuration = 5.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
			}
		}
	}

	// 3. 保存
	bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(BookmarkSettingsAsset.Get());

	if (!bSaved)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save BookMarkDataAsset!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("BookMarkDataAsset saved successfully!"));
	}

	// 刷新界面
	if (BookMarkAssetTab)
	{
		BookMarkAssetTab->RefreshAllListView();
	}
}

void FBookMarkToolModule::RegisterMenus()
{
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings",LOCTEXT("书签工具", "书签工具"));
			{

				FToolMenuEntry BookMarkEntry = FToolMenuEntry::InitComboButton(
					"BookMarkTool",
					FUIAction(),
					FOnGetContent::CreateRaw(this,&FBookMarkToolModule::GetDropdown),
					LOCTEXT("BookMarkTool", "书签工具"),
					LOCTEXT("BookMarkTool_Tooltip", "书签工具"),
					FSlateIcon(FBookMarkToolStyle::GetStyleSetName(), "BookMarkTool.OpenBookMarkTool")
				);
				//这句话一定要加，否则菜单按钮没文字
				BookMarkEntry.StyleNameOverride = "CalloutToolbar";
				Section.AddEntry(BookMarkEntry);
				
			}
		}
	}

	//注册扩展
	ExtendAssetContextMenu();
	ExtendFolderContextMenu();
}

TSharedRef<SWidget> FBookMarkToolModule::GetDropdown() const
{
	FMenuBuilder MenuBuilder(true, nullptr);

	// Add a button
	MenuBuilder.AddMenuEntry(
	LOCTEXT("ShowAssetMark", "资产"),
LOCTEXT("ShowAssetMarkTooltip", "打开资产收藏面板"),
		FSlateIcon(FBookMarkToolStyle::GetStyleSetName(), "BookMarkTool.OpenBookMarkTool"),
		FUIAction(
		FExecuteAction::CreateRaw(this,&FBookMarkToolModule::OpenAssetMark)
		)
	);
	
	// Add a line separator
	MenuBuilder.AddMenuSeparator();

	// Add a button
	MenuBuilder.AddMenuEntry(
	LOCTEXT("ShowFolderMark", "文件夹"),
LOCTEXT("ShowFolderMarkTooltip", "打开文件夹收藏面板"),

		FSlateIcon(FBookMarkToolStyle::GetStyleSetName(), "BookMarkTool.OpenBookMarkTool"),
		FUIAction(
		FExecuteAction::CreateRaw(this,&FBookMarkToolModule::OpenFolderMark),
		FIsActionChecked()
		)
	);

	// Add a line separator
	MenuBuilder.AddMenuSeparator();
	// Add a button 
	MenuBuilder.AddMenuEntry(
		LOCTEXT("ShowPosition", "世界位置"),
		LOCTEXT("ShowPositionTooltip", "打开关卡位置收藏面板"),
		FSlateIcon(FBookMarkToolStyle::Get().GetStyleSetName(), TEXT("BookMarkTool.OpenBookMarkTool")),
		FUIAction(
		FExecuteAction::CreateRaw(this,&FBookMarkToolModule::OpenPositionMark),
	FIsActionChecked()

		)
	);
	
	
	return MenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBookMarkToolModule, BookMarkTool)