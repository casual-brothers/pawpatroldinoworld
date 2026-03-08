// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNebulaFlowFSM, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogNebulaFlowUI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogNebulaFlowOnLine, Log, All);

// #TODO_NebulaFlow

class FNebulaFlowModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
