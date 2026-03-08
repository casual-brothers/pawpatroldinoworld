// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/PDWBaseHUD.h"
#include "GameplayTagContainer.h"
#include "PDWGameplayHUD.generated.h"

class UPDWCharacterSelector;

/**
 *
 */
UCLASS()
class PDW_API UPDWGameplayHUD : public UPDWBaseHUD
{
	GENERATED_BODY()

public:

	virtual void InitHUD(const TArray<APlayerController*>& Players) override;
	
	UFUNCTION(BlueprintCallable)
	void ManageRequestCharacterSelector(APlayerController* Player, bool Open, bool WithSelection = false);

	UFUNCTION()
	void ManageEnterOverlappPen(FPDWDinoPenInfo PenInfo, APDWPlayerController* PlayerController);

	UFUNCTION()
	void ManageExitOverlappPen(APDWPlayerController* PlayerController);

protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UPDWCharacterSelector* Player1CharacterSelector;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UPDWCharacterSelector* Player2CharacterSelector;

	UFUNCTION(BlueprintImplementableEvent)
	void OnOpenCharacterSelector(APlayerController* Player, bool Open);

	UFUNCTION(BlueprintImplementableEvent)
	void OnOverlappEnterPen(FPDWDinoPenInfo PenInfo, APlayerController* Player);

	UFUNCTION(BlueprintImplementableEvent)
	void OnOverlappExitPen(APlayerController* Player);
};