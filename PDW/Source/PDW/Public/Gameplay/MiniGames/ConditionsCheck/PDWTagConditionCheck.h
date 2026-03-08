// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MiniGames/ConditionsCheck/PDWConditionCheck.h"
#include "PDWTagConditionCheck.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWTagConditionCheck : public UPDWConditionCheck
{
	GENERATED_BODY()
	
public:
	bool EvaluateCondition(const FPDWInteractionPayload& inPayload,const FPDWInteractReceiverPayload& inReceiverPayload) override;

protected:

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FGameplayTagQuery Query;

};
