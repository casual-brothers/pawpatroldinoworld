// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWFlowNode_ApplyGameplayEffect.generated.h"

class UGameplayEffect;

UCLASS(NotBlueprintable, meta = (DisplayName = "Add Gameplay Effect", Keywords = "gas"))
class PDW_API UPDWFlowNode_ApplyGameplayEffect : public UFlowNode
{
	GENERATED_UCLASS_BODY()
public:
	virtual void ExecuteInput(const FName& PinName) override;
#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> EffectClassToActivate{};
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> EffectClassToDeactivate{};
};