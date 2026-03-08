// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MiniGames/ConditionsCheck/PDWQuantityConditionCheck.h"

bool UPDWQuantityConditionCheck::EvaluateCondition(const FPDWInteractionPayload& inPayload,const FPDWInteractReceiverPayload& inReceiverPayload)
{
	CurrentQuantity++;
	if (ConditionRule == EConditionRule::Equal)
	{
		return CurrentQuantity == TargetQuantity;
	}
	else if (ConditionRule == EConditionRule::Greater)
	{
		return CurrentQuantity > TargetQuantity;
	}
	else if (ConditionRule == EConditionRule::GreaterOrEqual)
	{
		return CurrentQuantity >= TargetQuantity;
	}
	else if (ConditionRule == EConditionRule::Less)
	{
		return CurrentQuantity < TargetQuantity;
	}
	else if (ConditionRule == EConditionRule::LessOrEqual)
	{
		return CurrentQuantity <= TargetQuantity;
	}
	
	return false;
}

bool UPDWQuantityConditionCheck::ResetCondition()
{
	CurrentQuantity = 0;
	return true;
}
