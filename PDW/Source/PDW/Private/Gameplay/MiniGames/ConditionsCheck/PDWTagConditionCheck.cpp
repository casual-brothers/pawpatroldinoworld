// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MiniGames/ConditionsCheck/PDWTagConditionCheck.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"

bool UPDWTagConditionCheck::EvaluateCondition(const FPDWInteractionPayload& inPayload,const FPDWInteractReceiverPayload& inReceiverPayload)
{
	return Query.Matches(inPayload.OwnerStates);
}
