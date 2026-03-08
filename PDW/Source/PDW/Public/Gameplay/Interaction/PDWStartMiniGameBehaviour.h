// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "Gameplay/Interaction/PDWInteractionBehaviour.h"
#include "Gameplay/Interfaces/PDWPlayerInterface.h"
#include "PDWStartMiniGameBehaviour.generated.h"



UENUM(BlueprintType,meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EAutoSwapType : uint8
{
	None =					0<<0,
	PupOnly =				1<<0,
	VehicleOnly =			1<<1,
};
ENUM_CLASS_FLAGS(EAutoSwapType);

/**
 * 
 */
UCLASS()
class PDW_API UPDWStartMiniGameBehaviour : public UPDWInteractionBehaviour
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	FGameplayTag GetAllowedPup() { return AllowedPup; };

protected:

	void ExecuteBehaviour_Implementation() override;

	UFUNCTION()
	void OnMinigameJoinRefused();

	UFUNCTION()
	void OnMinigameLeft(const FMiniGameEventSignature& inSignature);

	UPROPERTY(EditAnywhere, Category = "Configuration", BlueprintReadWrite)
	FGameplayTag AllowedPup = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = EAutoSwapType), Category = "Configuration")
	uint8 AutoSwapConfiguration;

	bool bMinigameTriggered = false;
};
