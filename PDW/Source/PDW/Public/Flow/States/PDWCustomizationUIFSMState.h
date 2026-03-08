// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "Data/PDWSaveStructures.h"
#include "PDWCustomizationUIFSMState.generated.h"

class UPDWCustomizationPage;
class APDWCustomizationLocation;

USTRUCT(BlueprintType)
struct PDW_API FVOCustomizationByTag
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> AudioToPlay;
};

/**
 * 
 */
UCLASS()
class PDW_API UPDWCustomizationUIFSMState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FVOCustomizationByTag> AudioByTag;

	UPROPERTY(EditAnywhere)
	FName LockedItemAudioId;
	
	UPROPERTY()
	UPDWCustomizationPage* CustomizationPage;
	
	UPROPERTY()
	FTransform OldPositionP1;
	
	UPROPERTY()
	FTransform OldPositionP2;

	UPROPERTY()
	FCustomizationData CurrentCustomization = {};
	
	UPROPERTY()
	FCustomizationData StartingCustomization = {};

	UPROPERTY()
	APDWCustomizationLocation* PupP1CustomizationLocation {};

	UPROPERTY()
	APDWCustomizationLocation* VehicleP1CustomizationLocation {};

	FName AudioToPlay;
	float CurrentTime;
	bool bPlayAudio;

	UFUNCTION()
	void OnFirstTeleportCompleted();

	UFUNCTION()
	void OnAllTeleportCompleted();

	UFUNCTION()
	void OnExitTeleportCompleted();

	UFUNCTION()
	void RestorePupPosition(APDWPlayerController* PDWPlayerController);

	UFUNCTION()
	void CustomizePup(APDWPlayerController* PDWPlayerController, FGameplayTag NewCustomization);

	UFUNCTION()
	void ResetToStartingCustomization();

	UFUNCTION()
	void OnTrasitionInOnEnterEnd();

	UFUNCTION()
	void OnTrasitionInOnExitEnd();

	UFUNCTION()
	void StartExitFlow();

	UFUNCTION()
	void PlayPupOnChangeCustomizationAudio(APDWPlayerController* PDWPlayerController, FString Parameter);

public:
	void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;
	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;

	void OnFSMStateExit_Implementation() override;
	void OnFSMStateUpdate_Implementation(float deltaTime) override;
};
