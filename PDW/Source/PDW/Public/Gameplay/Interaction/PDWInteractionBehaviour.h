// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/GameplayTagsDefinitions.h"
#include "PDWInteractionBehaviour.generated.h"

class UPDWInteractionReceiverComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionBehaviourSignature, UPDWInteractionBehaviour*, Behaviour);

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class PDW_API UPDWInteractionBehaviour : public UObject
{
	GENERATED_BODY()

public:
	void InitBehaviour(USceneComponent* NewOwnerComponent, FGameplayTag NewBehaviourIdentifier);

	void ExecuteBehaviour(AActor* Executioner = nullptr , bool bRemoveBehaviourWhenFinished = false);

	void SetExecutioner(AActor* inExecutioner);

	AActor* GetExecutioner() { return BehaviourExecutioner; };

	UFUNCTION(BlueprintCallable)
	void StopBehaviour();

	void TickBehaviour(float DeltaTime);

	FGameplayTag GetBehaviourIdentifier()
	{
		return BehaviourIdentifier;
	}

	UFUNCTION(BlueprintCallable)
	void SetSkipSteps(const bool bSkip);

	class UWorld* GetWorld() const override;

protected:
	virtual void InitBehaviour_Implementation(USceneComponent* NewOwnerComponent){}
	virtual void ExecuteBehaviour_Implementation() {}
	virtual void TickBehaviour_Implementation(float DeltaTime) {}
	virtual void StopBehaviour_Implementation() {}
	virtual void UpdateInteracterStates();
	void NotifySuccess();


	UFUNCTION(BlueprintImplementableEvent)
	void BP_InitBehaviour_Implementation(USceneComponent* NewOwnerComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ExecuteBehaviour_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_TickBehaviour_Implementation(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_StopBehaviour_Implementation();

	UFUNCTION(BlueprintPure)
	USceneComponent* GetOwnerComponent() {return OwnerComponent;}

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bSkipSteps = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BehaviourDuration = -1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FGameplayTagContainer InteractionStateToAdd;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FGameplayTagContainer InteractionStateToRemove;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bNotifySuccess = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "bNotifySuccess",EditConditionHides))
	FGameplayTag TargetIDToNotify = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "bNotifySuccess",EditConditionHides))
	FGameplayTag EventID = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly)
	float CurrentBehaviourElapsedTime = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float BehaviourProgressPercentage = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag BehaviourIdentifier;

	UPROPERTY(BlueprintReadOnly)
	AActor* BehaviourExecutioner;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USceneComponent> OwnerComponent;

	bool ShouldRemoveBehaviourWhenFinished = false;
};