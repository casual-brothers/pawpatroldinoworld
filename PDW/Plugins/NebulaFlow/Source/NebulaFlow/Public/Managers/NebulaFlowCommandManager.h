#pragma once

#include "HAL/IConsoleManager.h"
#include "HAL/Platform.h"
#include "UObject/NoExportTypes.h"

#include "NebulaFlowCommandManager.generated.h"

USTRUCT(BlueprintType)
struct FNebulaFlowSectionCommandsBackup
{
	GENERATED_USTRUCT_BODY()

public:

	FString SectionName = "";

	TMap<IConsoleVariable*, FString> CVarBackupMap = {};
};

UCLASS(config = Game, DefaultConfig)
class NEBULAFLOW_API UNebulaFlowCommandManager : public UObject
{

	GENERATED_BODY()

public:

	UNebulaFlowCommandManager(const FObjectInitializer& ObjectInitializer);

	virtual void ApplyCommands(const TCHAR* InSectionName, const TCHAR* InIniFilename = *GDeviceProfilesIni);

	virtual void RestoreCommands(const TCHAR* InSectionName);

private:

	UPROPERTY()
	TArray<FNebulaFlowSectionCommandsBackup> NebulaFlowSectionCommandsBackupList = {};
};