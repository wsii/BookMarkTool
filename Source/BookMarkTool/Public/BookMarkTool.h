// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BookMarkDataAsset.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FBookMarkToolModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	// void ExtendAssetContextMenu();
	// void ExtendFolderContextMenu();

private:
	void RegisterMenus();
	
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	
	void PluginButtonClicked();

	void CreatBookmarkSettingsAsset();
private:
	
	TSharedPtr<class FUICommandList> PluginCommands;
	TSharedPtr<class SBookMarkMain> BookMarkMain;

};
