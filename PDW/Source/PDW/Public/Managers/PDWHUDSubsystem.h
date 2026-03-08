// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Gameplay/MiniGames/PDWMiniGameInputBehaviour.h"
#include "StructUtils/InstancedStruct.h"
#include "Data/PDWMinigameConfigData.h"
#include "PDWDialogueSubSystem.h"
#include "Data/PDWGameplayStructures.h"
#include "UI/Widgets/PDWTutorialNavButton.h"
#include "GameplayTagContainer.h"

#include "PDWHUDSubsystem.generated.h"

DECLARE_DELEGATE_OneParam(FNavButtonTriggerCallback, const FInputActionInstance& Instance);
DECLARE_DELEGATE_ThreeParams(FNavButtonProgressCallback, const FInputActionInstance& Instance, float PreviousTime, float CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHUDReady);

class UPDWBaseHUD;
class UPDWSightWidget;

UENUM(BlueprintType)
enum class EHUDControls : uint8
{
	None UMETA(Hidden),
	Create,
	Hide,
	Destroy,
	Show
};

UENUM(BlueprintType)
enum class EHUDType : uint8
{
	Gameplay,
	Minigame
};

/**
 * 
 */
UCLASS()
class PDW_API UPDWHUDSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:

	UPROPERTY()
	UPDWBaseHUD* CurrentHUD = nullptr;

	UPROPERTY()
	UPDWMinigameConfigData* CurrentMinigameData;

	UPROPERTY()
	FConversation ConversationToPlay {};

	UPROPERTY()
	TMap<APDWPlayerController*, FGameplayTag> CurrentPlayersInPenArea;

	UPROPERTY()
	TArray<UPDWSightWidget*> Sights;

	UPROPERTY()
	FGameplayTagContainer CurrentSmellTargetMinigame;

	UFUNCTION()
	void OnPenAreaBeginOverlapp(FPDWDinoPenInfo PenInfo, AActor* OverlappedActor);

	UFUNCTION()
	void OnPenAreaEndOverlap(AActor* OverlappedActor);

	UFUNCTION()
	void PenNidoNeedMessage(FPDWDinoPenNeed DinoNeed);

public:
	void OnWorldBeginPlay(UWorld& InWorld) override;

	void Deinitialize() override;

	static UPDWHUDSubsystem* Get(UObject* WorldContextObject);
	
	UFUNCTION(Blueprintcallable)
	void ManageHUD(const EHUDControls& Control, EHUDType Hudtype = EHUDType::Gameplay);

	UFUNCTION(Blueprintcallable)
	void RequestToggleSwapCharacter(APlayerController* Sender, bool Open, bool WithSelection = false);

	void RequestInputActionIcon(APlayerController* Sender, FInstancedStruct& InputActionToShow, FNavButtonTriggerCallback InTriggerCallBack = FNavButtonTriggerCallback(), FNavButtonProgressCallback InProgressCallBack = FNavButtonProgressCallback());
	void RequestSightInputActionIcon(APlayerController* Sender, FInstancedStruct& InputActionToShow, FNavButtonTriggerCallback InTriggerCallBack = FNavButtonTriggerCallback(), FNavButtonProgressCallback InProgressCallBack = FNavButtonProgressCallback());

	UFUNCTION(BlueprintCallable)
	void BP_RequestInputActionIcon(APlayerController* Sender, UPARAM(ref)FInstancedStruct& InputActionToShow);

	UFUNCTION()
	void RequestMinigameData(UPDWMinigameConfigData* MinigameData);

	UFUNCTION(BlueprintCallable)
	void RequestHUDDialogue(const FConversation& Conversation);

	UFUNCTION(BlueprintCallable)
	void EndCurrentHUDDialogue();

	UFUNCTION(BlueprintCallable)
	void RequestTutorialInputAction(const FPDWTutorialButtonData& TutorialButtonData);

	UFUNCTION(BlueprintCallable)
	void EndCurrentTutorialInputAction();

	UFUNCTION(BlueprintCallable)
	void CheckHUDWidgetsConfiguration();

	UFUNCTION(BlueprintCallable)
	UPDWSightWidget* CreateSight(const TSubclassOf<UPDWSightWidget> SightToCreateClass, APDWPlayerController* OwnerWidget);

	UFUNCTION(BlueprintCallable)
	void RemoveSight(APDWPlayerController* OwnerWidget);

	UFUNCTION(BlueprintCallable)
	void RestoreSight(APDWPlayerController* OwnerWidget);

	UFUNCTION(BlueprintCallable)
	void ShowSmellTarget(FGameplayTagContainer MinigameID);
	
	UFUNCTION()
	UPDWBaseHUD* GetCurrentHUD(){ return CurrentHUD; };

	UPROPERTY()
	FOnHUDReady OnHUDReady;
};