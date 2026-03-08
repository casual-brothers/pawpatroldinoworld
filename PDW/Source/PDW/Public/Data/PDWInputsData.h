// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/PDWGameplayStructures.h"
#include "Engine/DataAsset.h"
#include "PDWInputsData.generated.h"


/**
 *
 */
UCLASS()
class PDW_API UPDWInputsData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DESIGN)
	TArray<UInputMappingContext*> AllIMCs{};
	
	// These are the IMCs that gets remapped together, so that if 2 action of different IMCs uses the same key and we want to remap them together, put the IMCs in the same array here
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Remapping")
	TArray<FPDWPairingRemappingIMcs> PairingIMCs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Remapping")
	TArray<FPDWInputMapsToUpdate> ContextKeyboardMapsInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Remapping")
	TArray<FPDWInputMapsToUpdate> ContextConsoleMapsInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Remapping")
	TArray<FKey> RemappableKeys{};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Remapping|Switch")
	TArray<FKey> RemappableKeysSwitch{};
};
