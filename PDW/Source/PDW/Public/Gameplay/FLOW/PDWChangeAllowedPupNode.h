// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nodes/FlowNode.h"
#include "PDWChangeAllowedPupNode.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, NotBlueprintable, meta = (DisplayName = "Change Allowed Pup"))
class UPDWChangeAllowedPupNode : public UFlowNode
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere)
	bool bResetAllowedPupsToDefault = false;

	UPROPERTY(EditAnywhere, meta = (Categories="ID.Character", EditCondition = "bResetAllowedPupsToDefault == false", EditConditionHides))
	FGameplayTagContainer NewAllowedPups;

	void OnLoad_Implementation() override;

	UFUNCTION()
	void ChangeAllowedPup();

public:
	UPDWChangeAllowedPupNode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void ExecuteInput(const FName& PinName) override;

#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif
};
