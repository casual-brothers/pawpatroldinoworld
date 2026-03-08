// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/CapsuleComponent.h"
#include "Gameplay/Interaction/PDWInteractionBehaviour.h"
#include "Data/PDWInteractionReceiverConfigData.h"
#include "Data/PDWGameplayStructures.h"
#include "Components/ActorComponent.h"

#include "PDWInteractionReceiverComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("InteractionReceiverComp"), STATGROUP_InteractionReceiver, STATCAT_Advanced);

class UPDWInteractionComponent;
class UPDWOverlapBehaviour;
class UPDWConditionCheck;
class UPDWTagComponent;

USTRUCT(BlueprintType)
struct PDW_API FPDWInteractReceiverPayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	AActor* ReceiverActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag ReceiverStates = FGameplayTag::EmptyTag;

};

USTRUCT(BlueprintType)
struct PDW_API FPDWInteractionPayload
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	AActor* Executioner = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag InteractionType = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer InteractionInfo = FGameplayTagContainer();

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer OwnerStates = FGameplayTagContainer();

	UPROPERTY(BlueprintReadWrite)
	mutable FGameplayTag BehaviourExecutedIdentifier = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadWrite)
	mutable FGameplayTagContainer InteractionReceiverState = FGameplayTagContainer();

	UPROPERTY(BlueprintReadWrite)
	mutable UPDWInteractionReceiverComponent* InteractionReceiverRef = {};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionFocusSignature, UPDWInteractionComponent*, Component);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionReceiverSignature, const FPDWInteractionPayload&, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBecameQuestTargetSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FComponentStatusSignature, UPDWInteractionReceiverComponent*, Component,const FGameplayTag&, PrevTag,const FGameplayTag&, NewTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPupTreatsGiven);


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PDW_API UPDWInteractionReceiverComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void InitInteractions();

	// Sets default values for this component's properties
	UPDWInteractionReceiverComponent();

	void SetInteractableID(int32 NewID) { InteractableIDD = NewID; }

	UFUNCTION(BlueprintPure)
	int32 GetInteractableID() const { return InteractableIDD; }

	uint8 GetInteractionConfiguration();

	void AddInteracter(UPDWInteractionComponent* Interacted, const FGameplayTagContainer& SupportedInteractions);

	void RemoveInteracter(UPDWInteractionComponent* Interacted);

	UFUNCTION(BlueprintCallable)
	bool ProcessInteraction(const FPDWInteractionPayload& Payload,bool& bIsAutoSwap);

	UFUNCTION(BlueprintCallable)
	bool ProcessInteractionForced(const FPDWInteractionPayload& Payload);

	UFUNCTION(BlueprintCallable)
	void AddInteractionBehaviour(const FGameplayTag& BehaviourIdentifier, UPDWInteractionBehaviour* Behaviour);

	UFUNCTION(BlueprintCallable)
	void ExecuteInteractionBehaviour(const FGameplayTag& BehaviourIdentifier, AActor* Executioner = nullptr, bool RemoveBehaviourWhenFinished = false,bool bSkipSteps = false);

	UFUNCTION(BlueprintCallable)
	void RemoveBehaviour(const FGameplayTag& BehaviourIdentifier);

	void SetCurrentBehaviour(UPDWInteractionBehaviour* NewBehaviourInProgress);

	void BehaviourFinished(UPDWInteractionBehaviour* NewBehaviourInProgress);

	void EndUIOverlapBehaviour(UPDWInteractionComponent* Sender);

	void OnInputReceivedInteractionSuccess();

	UFUNCTION(BlueprintCallable)
	void AddInteractionTag(const FGameplayTagContainer& Tags)
	{
		for (auto& tag : Tags.GetGameplayTagArray())
		{
			DataConfigurationInstance->AllowedInteractions.AddTag(tag);
		}
	}

	UFUNCTION(BlueprintCallable)
	void RemoveInteractionTag(const FGameplayTagContainer& Tags)
	{
		DataConfigurationInstance->AllowedInteractions.RemoveTags(Tags);
	}

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetAllowedInteraction() {return DataConfigurationInstance->AllowedInteractions;};

	UFUNCTION(BlueprintCallable)
	void AddStateTag(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable)
	void RemoveStateTag(const FGameplayTag& Tag);

	UFUNCTION(BlueprintPure)
	bool HasStateTag(const FGameplayTag& Tag) const
	{
		return CurrentState == Tag;
	}

	UFUNCTION(BlueprintPure)
	bool HasInteractionTag(const FGameplayTag& Tag) const
	{
		return DataConfigurationInstance->AllowedInteractions.HasTag(Tag);
	}

	UFUNCTION(BlueprintPure)
	FGameplayTag GetCurrentState() const;

	UFUNCTION(BlueprintPure)
	FGameplayTagContainer GetAllowedInteractionInCurrentState();

	UFUNCTION(BlueprintPure)
	int32 GetPupTreatsToAdd() {return PupTreatsToAdd; };

	UFUNCTION(BlueprintPure)
	bool GetShouldSaveAfterInteraction() { return ShouldSaveAfterInteraction; };

		UFUNCTION(BlueprintPure)
	EPupTreatsEvent GetPupTreatsEvent() {return AddPupTreatsEvent; };

	UFUNCTION(BlueprintPure)
	FGameplayTagContainer GetAllowedInteractionBasedOnState(const FGameplayTag& inStateTag);

	void PostEditImport() override;

	UFUNCTION(BlueprintCallable)
	void ResetCollisions();

	UFUNCTION()
	TSubclassOf<UPDWInteractionReceiverConfigData> GetDataConfiguration() {return DataConfiguration; };

public:

	UPROPERTY(EditAnywhere)
	bool UseDLReset = true;

	UPROPERTY()
	bool NeedsReset = false;

	UPROPERTY(BlueprintAssignable)
	FInteractionFocusSignature OnBecomeInteractable;

	UPROPERTY(BlueprintAssignable)
	FInteractionFocusSignature OnStopBeingInteractable;

	UPROPERTY(BlueprintAssignable)
	FInteractionReceiverSignature OnInteract;

	UPROPERTY(BlueprintAssignable)
	FInteractionReceiverSignature OnInteractionSuccessful;

	UPROPERTY(BlueprintAssignable)
	FInteractionReceiverSignature OnInteractionFailed;

	UPROPERTY(BlueprintAssignable)
	FInteractionBehaviourSignature OnBehaviourFinished;

	UPROPERTY(BlueprintAssignable)
	FComponentStatusSignature OnStatusTagAdded;

	UPROPERTY(BlueprintAssignable)
	FComponentStatusSignature OnStatusTagRemoved;

	UPROPERTY(BlueprintAssignable)
	FOnPupTreatsGiven OnPupTreatsGiven;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void ForceBehaviourByState();
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ExecuteUIOverlapBehaviour(const FGameplayTag& inBehaviourTagToTrigger, UPDWInteractionComponent* Sender);
	void NotifyInteractionResultToUIOverlapBehaviour(const bool bSuccess, AActor* inActor);
	virtual bool EvaluateBehaviourCondition(const FGameplayTag& inBehaviourTag,const FPDWInteractionPayload& inPayload);

	UFUNCTION()
	void OnVehicleComponentDestroyed(UActorComponent* inComponent);

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex);

	UFUNCTION()
	void OnQuestTagAdded(const FGameplayTagContainer& UpdatedContainer, const FGameplayTag& NewTagAdded);

	UPROPERTY()
	TArray<TObjectPtr<UPDWInteractionComponent>> InteractionComponents;

	UPROPERTY(EditAnywhere, Category = "Configuration", BlueprintReadWrite)
	TSubclassOf<UPDWInteractionReceiverConfigData> DataConfiguration = nullptr;

	UPROPERTY(EditAnywhere, Category = "Configuration", BlueprintReadWrite)
	FGameplayTag ResetStateTag;
	
	UPROPERTY(EditAnywhere, Category = "Configuration", BlueprintReadWrite)
	int32 PupTreatsToAdd = 0;

	UPROPERTY(EditAnywhere, Category = "Configuration", BlueprintReadWrite)
	EPupTreatsEvent AddPupTreatsEvent = EPupTreatsEvent::OnStateChange;

	UPROPERTY(EditAnywhere, Category = "Configuration", BlueprintReadWrite)
	bool ShouldSaveAfterInteraction = false;
	
	UPROPERTY(EditAnywhere, Category = "Configuration", BlueprintReadWrite)
	bool bResetCollisionAtEndBehaviour = false;

	FGameplayTag DefaultStateTag;

	UPROPERTY()
	TObjectPtr<UPDWInteractionBehaviour> InteractionInProgress;

	UPROPERTY()
	TMap<UPDWInteractionComponent*,UPDWOverlapBehaviour*> OverlapBehavioursInProgress;

	UPROPERTY()
	FGameplayTag CurrentState = FGameplayTag::EmptyTag;

private:
	UPROPERTY(EditAnywhere,Category = "DEBUG")
	int32 InteractableIDD = -1;

	UPROPERTY()
	UPDWTagComponent* TagComp = nullptr;

	UPROPERTY()
	UPDWInteractionReceiverConfigData* DataConfigurationInstance = nullptr;
	const FGameplayTag GetHighestPriorityTagSupported(const FPDWInteractionPayload& inPayload);
};
