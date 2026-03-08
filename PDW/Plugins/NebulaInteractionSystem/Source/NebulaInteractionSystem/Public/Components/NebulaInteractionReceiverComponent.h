#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/CapsuleComponent.h"
#include "Data/NebulaInteractionSystemStructs.h"

#include "NebulaInteractionReceiverComponent.generated.h"

class UInteractionFlowComponent;

#pragma region STRUCTS

#pragma endregion

#pragma region DELEGATES
#pragma endregion

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NEBULAINTERACTIONSYSTEM_API UNebulaInteractionReceiverComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:
	UNebulaInteractionReceiverComponent();

	//************************************
	// Method:    ProcessInteraction
	// FullName:  UNebulaInteractionReceiverComponent::ProcessInteraction
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: const FNebulaInteractionPayload & inPayload
	// Description: with the given payload will notify the flow interaction component with the payload.
	//************************************
	UFUNCTION(BlueprintCallable)
	bool ProcessInteraction(FInteractionInfo& inPayload);

	void BeginPlay() override;
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	UFUNCTION(BlueprintPure)
	FGameplayTag GetInteractionActionTag() const;

protected:

	UPROPERTY(Editanywhere, BlueprintReadWrite,Category = "Interaction Receiver Configuration")
	FGameplayTag InteractionActionTag = FGameplayTag();

	UPROPERTY(Editanywhere, BlueprintReadWrite,Category = "Interaction Receiver Configuration")
	bool bBindToInputAction = false;

	UPROPERTY()
	UInteractionFlowComponent* FlowComponent = nullptr;

};