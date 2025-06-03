// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FBookMarkToolModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void OpenAssetMark() const;
	void OpenFolderMark() const;
	void OpenPositionMark() const;
	
	void ExtendAssetContextMenu();
	void ExtendFolderContextMenu();

	void AddFoldPath();
	void AddAssetPath();
private:

	void RegisterMenus();

	/**
 * @brief 创建下拉插件菜单内容
 * @return 
 */
	TSharedRef<SWidget> GetDropdown() const;

	TSharedRef<class SDockTab> OnSpawnAssetTab(const class FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<class SDockTab> OnSpawnFolderTab(const class FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<class SDockTab> OnSpawnPositionTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	TSharedPtr<class SBookMarkAssetTab> BookMarkAssetTab;
	TSharedPtr<class SBookMarkFolderTab> BookMarkFolderTab;
	TSharedPtr<class SBookMarkPositionTab> BookMarkPositionTab;
};
