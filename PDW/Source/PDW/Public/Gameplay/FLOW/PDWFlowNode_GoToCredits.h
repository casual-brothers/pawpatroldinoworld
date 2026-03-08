// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/FLOW/PDWFlowNode_ChangeState.h"
#include "PDWFlowNode_GoToCredits.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Go To Credits"))
class PDW_API UPDWFlowNode_GoToCredits : public UPDWFlowNode_ChangeState
{
	GENERATED_UCLASS_BODY()
	
protected:

	void ChangeState() override;
};
