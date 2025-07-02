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
#include "SBookMarkMain.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "UObject/SavePackage.h"
#include "Widgets/Notifications/SNotificationList.h"

static const FName BookMarkMainTabName("BookMarkMain");

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

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(BookMarkMainTabName, FOnSpawnTab::CreateRaw(this, &FBookMarkToolModule::OnSpawnPluginTab))
	.SetDisplayName(LOCTEXT("FBookMarkToolTabTitle", "BookMarkMain"))
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

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(BookMarkMainTabName);
}

TSharedRef<SDockTab> FBookMarkToolModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	CreatBookmarkSettingsAsset();
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SAssignNew(BookMarkMain,SBookMarkMain)
			
		];
}


void FBookMarkToolModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(BookMarkMainTabName);
}

void FBookMarkToolModule::CreatBookmarkSettingsAsset()
{
	FString DevelperAssetPath = TEXT("/Game/Developers/"+FString(FPlatformProcess::UserName()).ToLower()+"/BookMarkTool/");
	if (!UEditorAssetLibrary::DoesDirectoryExist(DevelperAssetPath))
	{	// 确保目录存在
		UEditorAssetLibrary::MakeDirectory(DevelperAssetPath);
		// 创建数据资产
		UBookMarkDataAsset* NewDataAsset = NewObject<UBookMarkDataAsset>(
			GetTransientPackage(), 
			UBookMarkDataAsset::StaticClass(), 
			NAME_None, 
			RF_Public | RF_Standalone
		);
		// 保存资产
		FString PackageName = DevelperAssetPath.Append(TEXT("BookMarkDataAsset"));
		UPackage* Package = CreatePackage(*PackageName);
		NewDataAsset->Rename(*FPaths::GetBaseFilename(PackageName), Package);
		FAssetRegistryModule::AssetCreated(NewDataAsset);
		Package->MarkPackageDirty();

		// // 保存到磁盘
		// FString FilePath = DevelperAssetPath.Append(TEXT("BookMarkDataAsset.uasset"));
		//
		FSavePackageArgs SavePackageArgs;
		SavePackageArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SavePackageArgs.Error = GLog;
		SavePackageArgs.SaveFlags = SAVE_Async;
		//
		// bool bSuccess = UPackage::SavePackage(
		// 	Package,
		// 	NewDataAsset,
		// 	*FilePath,
		// 	SavePackageArgs
		// );
		// 5. 同步保存
		bool bSuccess = UPackage::SavePackage(
			Package,
			NewDataAsset,
			*FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension()),
			SavePackageArgs
		);

		if (bSuccess)
		{
			UE_LOG(LogTemp, Display, TEXT("Asset saved successfully: %s"), *PackageName);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save asset!"));
		}
	}
	//加载资源
	// Bookmarks = LoadObject<UBookMarkDataAsset>(nullptr, *DevelperPath.Append("BookMarkDataAsset.BookMarkDataAsset"));

}
//
// void FBookMarkToolModule::ExtendAssetContextMenu()
// {
// 	UToolMenu* AssetMenu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
// 	if (!AssetMenu) return;
//
// 	// Insert Top
// 	// FToolMenuSection& Section = AssetMenu->AddSection(
// 	// "AssetContextBookmark", LOCTEXT("AddBookMarkHeading", "AddBookMark"),FToolMenuInsert(FName("CommonAssetActions"),EToolMenuInsertType::Before));
//
// 	FToolMenuSection& Section = AssetMenu->AddSection(
// 	"AssetContextBookmark", LOCTEXT("AddBookMarkHeading", "AddBookMark"));
//
// 	Section.AddMenuEntry(
// 		FName("AddBookMark"),
// 		LOCTEXT("AddAssetBookMark", "Add To BookMark"),
// 		LOCTEXT("AssetBookMarkTooltip", "Add Asset BookMark"),
// 		FSlateIcon(FBookMarkToolStyle::GetStyleSetName(),"BookMarkTool.OpenBookMarkTool"),
// 		FUIAction(FExecuteAction::CreateRaw(this, &FBookMarkToolModule::AddAssetPath)));
// 	
// }
//
// void FBookMarkToolModule::ExtendFolderContextMenu()
// {
// 	UToolMenu* FolderMenu = UToolMenus::Get()->ExtendMenu("ContentBrowser.FolderContextMenu");
// 	if (!FolderMenu) return;
// 	
// 	// Insert Top
// 	// FToolMenuSection& Section = FolderMenu->AddSection(
// 	// 	"AddBookMark", LOCTEXT("AddBookMarkHeading", "AddBookMark"),FToolMenuInsert(FName("PathViewFolderOptions"),EToolMenuInsertType::Before));
// 	FToolMenuSection& Section = FolderMenu->AddSection(
// 		"AddBookMark", LOCTEXT("AddBookMarkHeading", "AddBookMark"));
//
// 	Section.AddMenuEntry(
// 		FName("AddFoldBookMark"),
// 		LOCTEXT("AddFoldBookMark", "Add To BookMark"),
// 		LOCTEXT("AddFoldBookMarkMenuTooltip", "Add Fold BookMark."),
// 		FSlateIcon(FBookMarkToolStyle::GetStyleSetName(),"BookMarkTool.OpenBookMarkTool"),
// 		FUIAction(FExecuteAction::CreateRaw(this, &FBookMarkToolModule::AddFoldPath)));
// }

void FBookMarkToolModule::RegisterMenus()
{
	bool IsMenuExist = UToolMenus::Get()->IsMenuRegistered("LevelEditor.MainMenu.UPTool");
	if (!IsMenuExist)
	{
		// 菜单不存在，进行注册
		UToolMenu* Menu = UToolMenus::Get()->RegisterMenu("LevelEditor.MainMenu.UPTool");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("BookMarkTool",
											 LOCTEXT("UPTool", "BookMarkTool"),
											 FToolMenuInsert(NAME_None, EToolMenuInsertType::Default));
	
			Section.AddMenuEntryWithCommandList(FBookMarkToolCommands::Get().OpenBookMarkTool, PluginCommands);
	
			// Section.AddSeparator("Test");
		}
		
		//将创建的菜单添加到已有的菜单中
		UToolMenu* MenuMain = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");
	
		MenuMain->AddSubMenu(
			"MainMenu",
			NAME_None,
			"UPTool",
			LOCTEXT("UPTool_Entry", "UPTool"),
			LOCTEXT("UPTool_Entry_Tip", "Extend UPTool Menu"));
	}
	else
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.UPTool");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("BookMarkTool",
								 LOCTEXT("UPTool", "BookMarkTool"),
								 FToolMenuInsert(NAME_None, EToolMenuInsertType::Default));
			Section.AddMenuEntryWithCommandList(FBookMarkToolCommands::Get().OpenBookMarkTool, PluginCommands);
		}
	}
	
	// UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	// {
	// 	FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings",LOCTEXT("书签工具", "书签工具"));
	// 	{
	//
	// 		FToolMenuEntry& BookMarkEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FBookMarkToolCommands::Get().OpenBookMarkTool));
	// 		BookMarkEntry.SetCommandList(PluginCommands);				
	// 		//这句话一定要加，否则菜单按钮没文字
	// 		// BookMarkEntry.StyleNameOverride = "CalloutToolbar";
	// 		Section.AddEntry(BookMarkEntry);
	// 		
	// 	}
	// }
	

	//注册扩展
	// ExtendAssetContextMenu();
	// ExtendFolderContextMenu();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBookMarkToolModule, BookMarkTool)