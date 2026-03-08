// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlowComponent.h"
#include "StructUtils/InstancedStruct.h"
#include "InteractionFlowComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionStateChangeDelegate,const FGameplayTag&, OldState,const FGameplayTag&, NewState);
DECLARE_MULTICAST_DELEGATE_TwoParams(FInteractionTriggered,UInteractionFlowComponent* Component,const FInstancedStruct& InteractionInfo);


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NEBULAINTERACTIONSYSTEM_API UInteractionFlowComponent : public UFlowComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FInteractionStateChangeDelegate OnStateChanged;

	FInteractionTriggered OnInteractionTriggered;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Flow Configuration")
	FGameplayTag StartingState = FGameplayTag::EmptyTag;

	UFUNCTION(BlueprintCallable)
	void TriggerInteraction(const FInstancedStruct& inInteractionInfo);
	UFUNCTION(BlueprintCallable)
	const FGameplayTag GetInteractionState() const;
	UFUNCTION(BlueprintCallable)
	void SetInteractionState(const FGameplayTag& inNewState);

	void BeginPlay() override;

protected:

	FGameplayTag CurrentState = FGameplayTag::EmptyTag;
};
