// Copyright Epic Games, Inc. All Rights Reserved.

#include "ToxicUtilitiesCommands.h"

#define LOCTEXT_NAMESPACE "FToxicUtilitiesModule"

void FToxicUtilitiesCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "ToxicUtilities", "Execute ToxicUtilities action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
