// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MiniGames/PDWMiniGameInputBehaviour.h"
#include "PDWRamOnSplineBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWRamOnSplineBehaviour : public UPDWMiniGameInputBehaviour
{
	GENERATED_BODY()
	
public:
	void ExecuteBehaviour(const FInputActionInstance& inInputInstance) override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Configuration")
	float RamDuration = 3.0f;

	void UninitializeBehaviour() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnRamModeStop();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnRamModeStart();
private:

	FTimerHandle RamModeTimerHandle;
	bool bRamMode = false;
};
