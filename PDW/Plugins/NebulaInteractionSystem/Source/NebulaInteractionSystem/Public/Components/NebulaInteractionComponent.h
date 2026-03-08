#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "Data/NebulaInteractionSystemStructs.h"

#include "NebulaInteractionComponent.generated.h"

class UNebulaInteractionReceiverComponent;

#pragma region ENUM
#pragma endregion

#pragma region STRUCT

USTRUCT(BlueprintType)
struct NEBULAINTERACTIONSYSTEM_API FNebulaInteractionConfigurationStruct
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FGameplayTagContainer AllowedInteractions = FGameplayTagContainer();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bAutomaticInteraction = true;
};

#pragma endregion

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEBULAINTERACTIONSYSTEM_API UNebulaInteractionComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:	
	
	UNebulaInteractionComponent();

	//************************************
	// Method:    TriggerInteraction
	// FullName:  UNebulaInteractionComponent::TriggerInteraction
	// Access:    virtual public 
	// Returns:   bool
	// Description: Triggers the interaction with the current receiver component.
	//************************************
	UFUNCTION(BlueprintCallable)
	virtual bool TriggerInteraction(UNebulaInteractionReceiverComponent* InteractionReceiverComp);
	
	UFUNCTION(BlueprintCallable)
	void AddInteractionStatus(const FGameplayTagContainer& inContainer);

	UFUNCTION(BlueprintCallable)
	void RemoveinteractionStatus(const FGameplayTagContainer& inContainer);

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetInteractionStatus() const;

	UFUNCTION(BlueprintCallable)
	void AddAllowedInteraction(const FGameplayTagContainer& inContainer);

protected:

	UFUNCTION()
	virtual void OnInputReceived(const FInputActionInstance& inInputInstance);
	virtual void BindInteractionToInput(FGameplayTag inActionTag = FGameplayTag::EmptyTag);
	virtual void UnBindInteractionToInput(FGameplayTag inActionTag = FGameplayTag::EmptyTag);
	virtual void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type Reason) override;
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Configuration")
	FNebulaInteractionConfigurationStruct InteractionConfiguration;

	//Container filled with all the status tags of the user
	FGameplayTagContainer InteractionStatusContainer = FGameplayTagContainer();

private:
	
	void GenerateInteractionPayload(FInteractionInfo& outPayload);
	UPROPERTY()
	TArray<UNebulaInteractionReceiverComponent*> ReceiverComponentsWaitingForInput {};
};