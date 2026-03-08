// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MiniGames/ConditionsCheck/PDWConditionCheck.h"
#include "PDWQuantityConditionCheck.generated.h"

UENUM(BlueprintType)
enum class EConditionRule : uint8
{
	Less =				0,
	Greater =			1,
	LessOrEqual =		2,
	GreaterOrEqual =	3,
	Equal =				4,
};

UCLASS()
class PDW_API UPDWQuantityConditionCheck : public UPDWConditionCheck
{
	GENERATED_BODY()
	
public:

	bool EvaluateCondition(const FPDWInteractionPayload& inPayload,const FPDWInteractReceiverPayload& inReceiverPayload) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EConditionRule ConditionRule = EConditionRule::Equal;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)

	uint8 TargetQuantity = 0;


	bool ResetCondition() override;

private:

	uint8 CurrentQuantity = 0;
};
