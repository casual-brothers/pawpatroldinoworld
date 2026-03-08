#pragma once

#include "Engine/DataTable.h"
#include "Templates/SubclassOf.h"
#include "Core/NebulaFlowCoreStructures.h"
#include "NebulaFlowNavbarDataStructures.h"
#include "NebulaFlowDialogDataStructures.generated.h"

class UNebulaFlowDialogButton;
class UFMODEvent;
class UNebulaFlowDialog;
class UInputAction;
class UInputMappingContext;

UENUM(Blueprintable, BlueprintType)
enum class EDialogType : uint8
{
	EGameDialog,
	ESystemDialog,
	EPopupDialog
};

USTRUCT(Blueprintable, BlueprintType)
struct NEBULAFLOW_API FDialogButtonConfiguration
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "UI")
		FNavbarButtonData NavButtonData;
	UPROPERTY(EditAnywhere, Category = "UI")
		FString ButtonResponse;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UNebulaFlowDialogButton> DialogButtonClass;
		
	UPROPERTY(EditAnywhere, Category = "UI", meta = (ClampMin = 0, ToolTip = "If the modal dowsn't require agreement, the answer given from multiple players with higher priority will be taken"))
		int32 ResponsePriority = 0;

};

USTRUCT(Blueprintable, BlueprintType)
struct NEBULAFLOW_API FDialogConfiguration
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "UI")
		EDialogType DialogType = EDialogType::EGameDialog;

	UPROPERTY(EditAnywhere, Category = "UI", meta = (MultiLine = true))
		FText DialogText{};

	UPROPERTY(EditAnywhere, Category = "UI")
		TSoftObjectPtr<UTexture2D> AdditionalTexture {};

    UPROPERTY(EditAnywhere, Category = "UI")
		TMap<EPlatformTargets, FText> PlatformDialogTexts{};

	UPROPERTY(EditAnywhere, Category = "UI")
		bool bHasSecondaryText = false;

	UPROPERTY(EditAnywhere, Category = "UI", meta = (EditCondition = "bHasSecondaryText"))
		FText SecondaryDialogText{};

	UPROPERTY(EditAnywhere, Category = "UI", meta = (EditCondition = "bHasSecondaryText"))
		TMap<EPlatformTargets, FText> PlatformSecondaryDialogTexts{};

	UPROPERTY(EditAnywhere, Category = "UI")
		TArray<FDialogButtonConfiguration> ButtonConfigurations;

	UPROPERTY(EditAnywhere, Category = "Input")
		TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UFMODEvent* VoiceOverEvent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ProgrammerName = FString("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UNebulaFlowDialog> DialogClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UNebulaFlowDialog> DialogSyncClass{};
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bNeedAllPlayersAnswer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bNeedAllPlayersAnswer))
		bool bMustPlayersAgree = false;
		
};

USTRUCT(Blueprintable, BlueprintType)
struct NEBULAFLOW_API FDialogConfigTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "UI")
	FDialogConfiguration DialogConfiguration;

};

