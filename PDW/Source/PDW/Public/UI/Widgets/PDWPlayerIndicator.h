// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PDWPlayerIndicator.generated.h"

/**
 * 
 */
UCLASS(Abstract, MinimalAPI)
class UPDWPlayerIndicator : public UUserWidget
{
	GENERATED_BODY()
protected:
	
	UPROPERTY()
	EMiniGamePlayerType AssinedPlayer;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "InitIndicator"))
	void BP_InitIndicator(EMiniGamePlayerType inAssinedPlayer);

public:

	UFUNCTION(BlueprintCallable)
	void InitIndicator(EMiniGamePlayerType inAssinedPlayer);

	UFUNCTION(BlueprintPure)
	EMiniGamePlayerType GetAssinedPlayer() {return AssinedPlayer;};
};
