// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"

#include "PDWConditionCheck.generated.h"

struct FPDWInteractionPayload;
struct FPDWInteractReceiverPayload;

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class PDW_API UPDWConditionCheck : public UObject
{
	GENERATED_BODY()
	
public:

	virtual bool ResetCondition(){ return true;};
	virtual bool EvaluateCondition(const FPDWInteractionPayload& inPayload, const FPDWInteractReceiverPayload& inReceiverPayload){return true;}
};
