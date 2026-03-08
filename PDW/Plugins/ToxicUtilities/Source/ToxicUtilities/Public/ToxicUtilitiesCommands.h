// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "ToxicUtilitiesStyle.h"

class FToxicUtilitiesCommands : public TCommands<FToxicUtilitiesCommands>
{
public:

	FToxicUtilitiesCommands()
		: TCommands<FToxicUtilitiesCommands>(TEXT("ToxicUtilities"), NSLOCTEXT("Contexts", "ToxicUtilities", "ToxicUtilities Plugin"), NAME_None, FToxicUtilitiesStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
