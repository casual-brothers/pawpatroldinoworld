// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PDWMinigameGameplayBehaviour.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class PDW_API UPDWMinigameGameplayBehaviour : public UObject
{
	GENERATED_BODY()

public:

	virtual void ExecuteBehaviour(AActor*  inOwner,AActor* inInteracter, bool& bSuccessNotify);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ExecuteBehaviour(AActor* inOwner,AActor* inInteracter);

protected:

	UPROPERTY(EditAnywhere)
	bool bNotifySuccess = false;
};
