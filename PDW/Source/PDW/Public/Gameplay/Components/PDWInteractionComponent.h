// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameplayTagContainer.h"
#include "PDWInteractionComponent.generated.h"

class UPDWInteractionReceiverComponent;
struct FPDWInteractionPayload;
class APDWPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractiomStateRemoved,const FGameplayTagContainer&, outTagsRemoved);

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	Overlap			=			0,
	InputReceived	=			1,
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWInteractionComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPDWInteractionComponent();

	UPROPERTY(BlueprintAssignable)
	FOnInteractiomStateRemoved OnInteractionStateRemoved;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool InteractAction(bool& bIsAutoSwap,EInteractionType InteractiontType = EInteractionType::Overlap );
	
	void SetOwnerInteractionStates(const FGameplayTagContainer& inContainer);

	UFUNCTION(BlueprintCallable)
	void AddOwnerInteractionStates(const FGameplayTagContainer& inContainer);

	UFUNCTION(BlueprintCallable)
	void RemoveinteractionStates(const FGameplayTagContainer& inContainer);

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetOwnerStates() const;

	UFUNCTION(BlueprintCallable)
	void AddAllowedInteraction(const FGameplayTagContainer& inContainer);

	UFUNCTION(BlueprintCallable)
	virtual void RemoveAllowedInteraction(const FGameplayTagContainer& inContainer);

	UFUNCTION(BlueprintCallable)
	void SetCurrentInteractionReceiver(UPDWInteractionReceiverComponent* inReceiver);

	UFUNCTION(BlueprintCallable)
	virtual void SetAllowedInteraction(const FGameplayTagContainer& inContainer);

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetAllowedInteractions() const;

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetSkillTypeInteractions() const;

	UFUNCTION(BlueprintCallable)
	UPDWInteractionReceiverComponent* GetCurrentInteractionReceiver();

protected:

	//USED FOR NON PLAYERCONTROLLER POSSESED ITEM
	UPROPERTY()
	FGameplayTagContainer InteractionComponentState;

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer AllowedInteractions;

	UPROPERTY()
	FGameplayTagContainer SkillTypeInteractions;

	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UPDWInteractionReceiverComponent> CurrentReceiverComponent;

	UPROPERTY()
	TArray<TObjectPtr<UPDWInteractionReceiverComponent>> ReceiverComponents;
	
	virtual void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type Reason) override;
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex);

	UFUNCTION()
	void OnChangeCharacterEvent(const FGameplayTag& inNewPup,APDWPlayerController* inController);

	UFUNCTION()
	virtual void OnCharacterPossessed();
private:
	
	void GenerateInteractionPayload(FPDWInteractionPayload& outPayload);

	void EvaluateClosestInteraction();
	
	bool bEvaluateClosestInteraction = false;
};
