// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "BookMarkToolStyle.h"

class FBookMarkToolCommands : public TCommands<FBookMarkToolCommands>
{
public:

	FBookMarkToolCommands()
		: TCommands<FBookMarkToolCommands>(TEXT("BookMarkTool"), NSLOCTEXT("Contexts", "BookMarkTool", "BookMarkTool Plugin"), NAME_None, FBookMarkToolStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenBookMarkTool;
};