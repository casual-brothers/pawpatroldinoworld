// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "Gameplay/MiniGames/PDWMiniGameInputBehaviour.h"
#include "UI/NebulaFlowDialog.h"
#include "Managers/PDWEventSubsytem.h"
#include "Gameplay/MiniGames/ConditionsCheck/PDWQuantityConditionCheck.h"
#include "Data/PDWMinigameConfigData.h"
#include "Gameplay/Interfaces/PDWMinigameActorInterface.h"
#include "PDWMinigameConfigComponent.generated.h"


class UPDWMiniGameInputBehaviour;
class UPDWMinigameConfigData;
class UInputMappingContext;
class UFlowComponent;
class APDWVehiclePawn;

#pragma region STRUCTS

USTRUCT(BlueprintType)
struct PDW_API FSwapConfiguration
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag AllowedPup{ FGameplayTag::EmptyTag };

	UPROPERTY(EditAnywhere)
	uint8 AutoSwapConfiguration{ 0 };
};

USTRUCT(BlueprintType)
struct PDW_API FMiniGameEventSignature
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag EventTag{ FGameplayTag::EmptyTag };

	UPROPERTY(BlueprintReadWrite)
	UPDWMinigameConfigComponent* MiniGameComponent{ nullptr };

};

USTRUCT(BlueprintType)
struct PDW_API FMinigameFlowEventSignature
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag inEventTag{ FGameplayTag::EmptyTag };
	
	UPROPERTY(BlueprintReadWrite)
	UPDWMinigameConfigComponent* MiniGameComponent{ nullptr };
	//ADD EXTRA PROPERTRY IF NEEDED
};

USTRUCT(BlueprintType)
struct PDW_API FMinigameStatesCached
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer PlayerStates{};
	
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer PlayerAllowedInteractions{};

};

#pragma endregion

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMinigameCompleteDelegate, const FMiniGameEventSignature&, inSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMinigameInitializationDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotifySuccessFromMinigame,const FInteractionEventSignature&, inEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMinigameInitializationDelegateComplete,const TArray<APDWPlayerController*>&,Controllers);

/*
#TODO DM: verify on multy
*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWMinigameConfigComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class UPDWMiniGameFunctionLibrary;

public:	
	// Sets default values for this component's properties
	UPDWMinigameConfigComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bShowSuccessMessage{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bSkipJoinRequestMessage{ false };

	UPROPERTY(BlueprintAssignable)
	FMinigameCompleteDelegate OnMinigameComplete;

	UPROPERTY(BlueprintAssignable)
	FMinigameInitializationDelegate OnMinigameInitialization;

	UPROPERTY(BlueprintAssignable)
	FMinigameInitializationDelegateComplete OnMinigameInitializationComplete;

	UPROPERTY(BlueprintAssignable)
	FMinigameCompleteDelegate OnMinigameLeft;

	UPROPERTY(BlueprintAssignable)
	FOnNotifySuccessFromMinigame OnNotifySuccessFromMinigame;

	void InitializeMiniGame(APDWPlayerController* inInteracter,const FSwapConfiguration& inSwapConfig);

	UFUNCTION(BlueprintCallable)
	void InitializeTargetActorsBehaviour();

	UFUNCTION(BlueprintCallable)
	void NotifyEvent(const FGameplayTag& inEvent);

	UFUNCTION()
	void OnNotifySuccessFromMinigameActor(const FInteractionEventSignature& inEvent);

	UFUNCTION(BlueprintCallable)
	UFlowComponent* GetMinigameFlowComponent();

	UFUNCTION(BlueprintCallable)
	void SetMinigameState(const FGameplayTag& inState);

	UFUNCTION(BlueprintCallable)
	void HidePlayer(APDWPlayerController* inController,bool bHide);

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetCurrentState() const;

	UFUNCTION(BlueprintCallable)
	UPDWMinigameConfigData* GetMinigameConfigurationData() { return MinigameConfigurationDataInstance; }

	UFUNCTION(BlueprintCallable)
	void GetPlayerControllers(TArray<APDWPlayerController*>& outControllers);

	void GetMinigameTargetActors(TArray<AActor*>& OutActors) const {OutActors = MinigameTargetActors;};
	UFUNCTION(BlueprintCallable)
	void SetMinigameTargetActor(AActor* inActor) {MinigameTargetActors.AddUnique(inActor);};
	UFUNCTION()
	TMap<FGameplayTag,TSubclassOf<UPDWMinigameConfigData>> GetStateMiniGameConfigurationData() {return StateMiniGameConfigurationData; };

protected:
	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void TriggerBehaviours();
	void CleanBehaviours();
	void SetUpCamera(APDWPlayerController* inController);
	void BlendCameraBack();
	void TeleportPlayer(APDWPlayerController* inController, int32 Index = 0);
	void MiniGameComplete();
	void MiniGameLeft(bool bReset = true);

	void SplineRunCompleteTeleport(const int32 Index, APDWPlayerController* Controller);

	void RestartMiniGame();
	void OnRequestToJoinAnswered(FString inAnswer,APDWPlayerController* inController);
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool SwapCharacter(APDWPlayerController* inController);
	void FinishControllerInitialization( APDWPlayerController* inController);

	UFUNCTION()
	void OnSwapPupComplete(APDWPlayerController* inController);

	UFUNCTION()
	void OnSwapVehicleComplete(const bool bInVehicle,APDWPlayerController* inController);

	UPROPERTY(EditAnywhere, meta = (FunctionReference,AllowFunctionLibraries,PrototypeFunction = "/Script/PDW.PDWMiniGameFunctionLibrary.Prototype_FunctionCheck", DefaultBindingName = "TestFunction"))
	TMap<FGameplayTag,FMemberReference> FunctionCheck;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (ForceInlineRow))
	TMap<FGameplayTag,TSubclassOf<UPDWMinigameConfigData>> StateMiniGameConfigurationData {};

	UPROPERTY()
	UNebulaFlowDialog* DialogInstance = nullptr;

	UPROPERTY()
	UFlowComponent* MiniGameFlow = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bHidePlayerTwo = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FGameplayTag MiniGameStartingState = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bDisableVehiclePhysics = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bInitializeTargetActorsOnMinigameStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration",meta =(ToolTip = "used for splinerun game"))
	bool bUseSpecificEndPosition = false;

	FGameplayTag CurrentState = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	TArray<AActor*> MinigameTargetActors = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	bool bUseVehicleMontage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration",meta = (EditCondition = "bUseVehicleMontage"))
	UAnimMontage* VehicleMontage = nullptr;

	UPROPERTY()
	APDWPlayerController* InteracterPlayer;

private:

	FMinigameStatesCached PlayerOneCachedState;
	FMinigameStatesCached PlayerTwoCachedState;

	UPROPERTY()
	UPDWMinigameConfigData* MinigameConfigurationDataInstance = nullptr;

	FOnInteractionExecuted InteractionDelegate;

	FSwapConfiguration SwapConfig;

	UPROPERTY()
	APDWPlayerController* ControllerToUseForCamera = nullptr;

	UPROPERTY()
	
	TArray<APDWPlayerController*> StoredPlayerControllers {};

	EMiniGamePlayerConfiguration CurrentConfiguration = EMiniGamePlayerConfiguration::SinglePlayer;

	UPROPERTY()
	TMap<APDWPlayerController*, bool> ForceVehicleSwapMap;

	void PlayClosingMontage(APDWVehiclePawn* Vehicle);
	void PlayOpeningMontage(APDWVehiclePawn* Vehicle);
	void GenerateMiniGameConfigurationInstance();
	void FillCachedStates(const int32 inControllerIndex,APDWPlayerController* inController);
	void InitializeMinigameTargetActors();
	void UnInitializeMinigameTargetActors(bool bReset);
	UFUNCTION()
	void InizializeMinigameSinglePlayerAfterTransition();
	UFUNCTION()
	void InizializeMinigameMultiPlayerAfterTransition();

	UFUNCTION()
	void OnJoinAccepted();

	//bool bForceVehicleSwap = false;
	int32 SwapsToWait = 0;
	bool bFlowStarted = false;
};
