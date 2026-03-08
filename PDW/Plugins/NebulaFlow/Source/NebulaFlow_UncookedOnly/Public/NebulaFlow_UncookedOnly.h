#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

// Log
DECLARE_LOG_CATEGORY_EXTERN(LogNebulaFlowK2Node, Log, All);

class FNebulaFlow_UncookedOnlyModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};