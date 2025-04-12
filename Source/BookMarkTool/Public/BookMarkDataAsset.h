// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BookMarkDataAsset.generated.h"


// USTRUCT(BlueprintType)
// struct FLevelBookmarksContainer
// {
// 	GENERATED_BODY()
//
// public:
// 	UPROPERTY(EditAnywhere)
// 	FString Name;
//
// 	UPROPERTY(EditAnywhere, Category="", meta=(AllowedClasses="/Script/Engine.World"))
// 	FSoftObjectPath MapPath;
// 	
// };

USTRUCT(BlueprintType)
struct FFoldPathBookmarksContainer
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Path")
	FString Name;

	// UPROPERTY(EditAnywhere, Category="", meta=(AllowedClasses="/Script/Engine.World"))
	UPROPERTY(VisibleAnywhere, Category="Path",meta = (DisplayName = "Save Path", ContentDir))
	FDirectoryPath Path;

};


struct FFoldPathListElementHandle
{
	int32 index;
};

USTRUCT(BlueprintType)
struct FAssetBookmarksContainer
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Asset")
	FString Name;

	UPROPERTY(VisibleAnywhere, Category="Asset",meta = (DisplayName = "Save Path"))
	FSoftObjectPath Path;
	
};
/**
 * 
 */
UCLASS()
class BOOKMARKTOOL_API UBookMarkDataAsset : public UDataAsset
{
	GENERATED_BODY()

	public:
	void ReSetStorePath();

	UPROPERTY(EditAnywhere, Category="Asset")
	TArray<FAssetBookmarksContainer> AssetList;
	
	UPROPERTY(EditAnywhere, Category="Path")
	TArray<FFoldPathBookmarksContainer> FoldPathList;
	
	UPROPERTY(VisibleAnywhere, Category="Collection")
	TSet<FString> StorePath;
	
};


inline void UBookMarkDataAsset::ReSetStorePath()
{
	StorePath.Empty();
	for (FFoldPathBookmarksContainer Element : FoldPathList)
	{
		StorePath.Emplace(Element.Path.Path);
	}

	for (FAssetBookmarksContainer Element : AssetList)
	{
		StorePath.Emplace(Element.Path.ToString());
	}
}
