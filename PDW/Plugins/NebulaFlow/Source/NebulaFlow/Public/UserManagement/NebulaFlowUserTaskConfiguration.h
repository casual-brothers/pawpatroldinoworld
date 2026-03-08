#pragma once

#include "Engine/DataTable.h"
#include "TaskManagement/NebulaFlowTaskInterface.h"
#include "NebulaFlowUserTaskConfiguration.generated.h"

UENUM(BlueprintType)
enum class EUserTaskType : uint8
{
	EUserLoginChangedPrimaryPlayer,
	EUserLoginChangedSecondaryPlayer,
	EMultipleController,
	EMissingPlayerOwner,
	EDefinePlayerOwner,
	EAppResume,
	ESwitchRemapJoycon,
	EDeviceConnected,
	EDefineSecondaryPlayer,
	EChangeUser,
	EMissingSecondaryPlayer,
	ESaveFailed,
	ERedefinePlayerOwner,
	ESwitchAppletShown,
	EControllerConnectionConnectedPrimaryPlayer,
	EControllerConnectionConnectedSecondaryPlayer,
	EControllerConnectionDisconnectedPrimaryPlayer,
	EControllerConnectionDisconnectedSecondaryPlayer,
};

USTRUCT(BlueprintType, Blueprintable)
struct NEBULAFLOW_API FNebulaFlowUserTaskConfiguration : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUserTaskType TaskType = EUserTaskType::EAppResume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UNebulaFlowTaskInterface> TaskClass;
};