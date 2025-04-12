// Copyright Epic Games, Inc. All Rights Reserved.

#include "BookMarkToolCommands.h"

#define LOCTEXT_NAMESPACE "FBookMarkToolModule"

void FBookMarkToolCommands::RegisterCommands()
{
	UI_COMMAND(OpenBookMarkTool, "BookMarkTool", "Bring up BookMarkTool window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
