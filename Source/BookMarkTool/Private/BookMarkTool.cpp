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
#include "SBookMarkToolTab.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

static const FName BookMarkToolTabName("BookMarkTool");

#define LOCTEXT_NAMESPACE "FBookMarkToolModule"

void FBookMarkToolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FBookMarkToolStyle::Initialize();
	FBookMarkToolStyle::ReloadTextures();

	FBookMarkToolCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FBookMarkToolCommands::Get().OpenBookMarkTool,
		FExecuteAction::CreateRaw(this, &FBookMarkToolModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FBookMarkToolModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(BookMarkToolTabName, FOnSpawnTab::CreateRaw(this, &FBookMarkToolModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FBookMarkToolTabTitle", "BookMarkTool"))
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

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(BookMarkToolTabName);
}

TSharedRef<SDockTab> FBookMarkToolModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[

			SNew(SBox)
			// .HAlign(HAlign_Center)
			// .VAlign(VAlign_Center)
			[
				SAssignNew(BookMarkToolTab,SBookMarkToolTab)
			]
		];
}

void FBookMarkToolModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(BookMarkToolTabName);
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
	TObjectPtr<UBookMarkDataAsset> BookmarkSettingsAsset;
	// // 加载 DataAsset
	BookmarkSettingsAsset = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));
	if (!BookmarkSettingsAsset)
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
	bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(BookmarkSettingsAsset);

	if (!bSaved)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save BookMarkDataAsset!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("BookMarkDataAsset saved successfully!"));
	}

	// 刷新界面
	if (BookMarkToolTab)
	{
		BookMarkToolTab->RefreshAllListView();
	}
}

void FBookMarkToolModule::AddAssetPath()
{
	TObjectPtr<UBookMarkDataAsset> BookmarkSettingsAsset;
	// // 加载 DataAsset
	BookmarkSettingsAsset = LoadObject<UBookMarkDataAsset>(nullptr, TEXT("/BookMarkTool/BookMarkDataAsset.BookMarkDataAsset"));
	// 刷新缓存
	BookmarkSettingsAsset->ReSetStorePath();
	
	if (!BookmarkSettingsAsset)
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
	bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(BookmarkSettingsAsset);

	if (!bSaved)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save BookMarkDataAsset!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("BookMarkDataAsset saved successfully!"));
	}

	// 刷新界面
	if (BookMarkToolTab)
	{
		BookMarkToolTab->RefreshAllListView();
	}
}

void FBookMarkToolModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FBookMarkToolCommands::Get().OpenBookMarkTool, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FBookMarkToolCommands::Get().OpenBookMarkTool));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}

	//注册扩展
	ExtendAssetContextMenu();
	ExtendFolderContextMenu();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBookMarkToolModule, BookMarkTool)