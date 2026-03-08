// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/PDWUIBaseElement.h"
#include "PDWUnlockableBox.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UPDWUnlockableBox : public UPDWUIBaseElement
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta=(AllowPrivateAccess=true))
	void SetUnlockableIcon(const TSoftObjectPtr<UTexture2D>& NewTexture);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUnlockableFound(bool IsFound);
};
