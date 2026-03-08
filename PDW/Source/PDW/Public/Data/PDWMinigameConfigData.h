#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "PDWMinigameConfigData.generated.h"

class UPDWQuantityConditionCheck;
class UPDWMiniGameInputBehaviour;

#pragma region ENUM

UENUM(BlueprintType)
enum class EMiniGamePlayerConfiguration : uint8
{
	SinglePlayer =			0,
	MultiPlayer =			1,
};

UENUM(BlueprintType)
enum class EMiniGameCameraTransitionType : uint8
{
	BlendToCamera =				0,
	SequenceOverview =			1,
};

UENUM(BlueprintType)
enum class EMiniGamePlayerType : uint8
{
	P1 =			0,
	P2 =			1,
};

#pragma endregion


USTRUCT(BlueprintType)
struct PDW_API FMinigameInputBehaviourList
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced)
	TArray<UPDWMiniGameInputBehaviour*> InputBehaviour {};
};

USTRUCT(BlueprintType)
struct PDW_API FMiniGameBehaviourConfigStruct
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (ForceInlineRow))
	TMap<EMiniGamePlayerType,FMinigameInputBehaviourList> MiniGameBehaviour;
};

USTRUCT(BlueprintType)
struct PDW_API FMiniGameCameraConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseMinigameCamera = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSkipEndingMinicameraBlend = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseMinigameCamera",EditConditionHides))
	EMiniGameCameraTransitionType CameraTransitionType = EMiniGameCameraTransitionType::BlendToCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseMinigameCamera",EditConditionHides))
	float BlendTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseMinigameCamera",EditConditionHides))
	TEnumAsByte<EViewTargetBlendFunction> BlendFunction = VTBlend_Linear;

};

UCLASS(Blueprintable)
class PDW_API UPDWMinigameConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (ForceInlineRow),Category = "Configuration")
	FText MiniGameDisplayName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (ForceInlineRow),Category = "Configuration")
	FGameplayTag MiniGameIdentifierTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (ForceInlineRow),Category = "Configuration")
	FGameplayTagContainer MiniGameSpecificInteractions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (ForceInlineRow),Category = "Configuration")
	TMap<EMiniGamePlayerConfiguration,FMiniGameBehaviourConfigStruct> MiniGameConfig {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Instanced,meta = (ForceInlineRow),Category = "Configuration | EndingConditions")
	TMap<FGameplayTag,UPDWQuantityConditionCheck*> MiniGameObjectiveConfig {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration | Camera")
	FMiniGameCameraConfig CameraConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bIsReplayable = true;

};