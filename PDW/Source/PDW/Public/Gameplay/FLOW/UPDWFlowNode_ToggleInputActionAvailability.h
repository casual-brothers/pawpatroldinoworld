// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "Nodes/FlowNode.h"
#include "UPDWFlowNode_ToggleInputActionAvailability.generated.h"

class UInputAction;

UCLASS(NotBlueprintable, meta = (DisplayName = "Toggle Input Action Availability"))
class PDW_API UUPDWFlowNode_ToggleInputActionAvailability : public UFlowNode
{
	GENERATED_BODY()
	
	UUPDWFlowNode_ToggleInputActionAvailability(const FObjectInitializer& ObjectInitializer);
	
public:
	
	virtual void ExecuteInput(const FName& PinName) override;

	void OnLoad_Implementation() override;

	void OnPassThrough_Implementation() override;

	void Implementation();

	UPROPERTY(EditAnywhere, Category = "Configuration")
	TSoftObjectPtr<UInputAction> InputAction;
		
	UPROPERTY(EditAnywhere, Category = "Configuration")
	bool bEnable = false;

};

