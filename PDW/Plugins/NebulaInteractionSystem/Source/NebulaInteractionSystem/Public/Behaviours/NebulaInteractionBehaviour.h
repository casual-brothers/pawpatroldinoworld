#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "Data/NebulaInteractionSystemStructs.h"

#include "NebulaInteractionBehaviour.generated.h"

class UNebulaInteractionReceiverComponent;

#pragma region DELEGATES
DECLARE_MULTICAST_DELEGATE_OneParam(FInteractionBehaviourSignature, const FInteractionBehaviourEventInfo& EventInfo);
DECLARE_MULTICAST_DELEGATE_OneParam(FInteractionBehaviourInterruptSignature, const FInteractionBehaviourInterruptInfo& EventInfo);
#pragma endregion

#pragma region ENUM

UENUM(BlueprintType)
enum class EBroadcastType : uint8
{
	Start		= 0,
	End			= 1,
	Interrupt	= 2,
};

#pragma endregion

#pragma region STRUCTS

USTRUCT(BlueprintType)
struct NEBULAINTERACTIONSYSTEM_API FBehaviourConfigurationStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag BehaviourID = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag InteractableTransitionToState = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer InteractorStatusAdded  = FGameplayTagContainer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer InteractorStatusRemoved  = FGameplayTagContainer();

// VEIRFICARE SE SERVE ANCORA
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNotifySuccess = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bNotifySuccess", EditConditionHides))
	FGameplayTag TargetIDToNotify = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bNotifySuccess", EditConditionHides))
	FGameplayTag EventID = FGameplayTag::EmptyTag;
//************
};

#pragma endregion

UCLASS(BlueprintType,Blueprintable,EditInlineNew, DefaultToInstanced)
class NEBULAINTERACTIONSYSTEM_API UNebulaInteractionBehaviour : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:

	FInteractionBehaviourSignature OnBehaviourStarted;
	FInteractionBehaviourSignature OnBehaviourFinished;
	FInteractionBehaviourInterruptSignature OnBehaviourInterruped;

	//************************************
	// Method:    InitBehaviour
	// FullName:  UNebulaInteractionBehaviour::InitBehaviour
	// Access:    public 
	// Returns:   void
	// Parameter: FBehaviourInfo inBehaviourInfo - contains information such the behaviour owner and the interactor.
	// Description: Initializes the behaviour.This is the first thing to call.
	//************************************
	void InitBehaviour(const FBehaviourInfo& inBehaviourInfo);


	//************************************
	// Method:    ExecuteBehaviour
	// FullName:  UNebulaInteractionBehaviour::ExecuteBehaviour
	// Access:    public 
	// Returns:   void
	// Description: Executes the behaviour.
	//************************************
	void ExecuteBehaviour();


	//************************************
	// Method:    TickBehaviour
	// FullName:  UNebulaInteractionBehaviour::TickBehaviour
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: float inDeltaTime
	// Description: Ticks the behaviour with the given delta time. This is called every frame while the behaviour is active.
	//************************************
	void TickBehaviour(const float inDeltaTime);


	//************************************
	// Method:    InterruptBehaviour
	// FullName:  UNebulaInteractionBehaviour::InterruptBehaviour
	// Access:    public 
	// Returns:   void
	// Parameter: AActor * inInterruptionSource
	// Description: Interrupts the behaviour with the given interruption source. This is called when the behaviour is interrupted by another actor.
	//************************************
	UFUNCTION(BlueprintCallable)
	void InterruptBehaviour(AActor* inInterruptionSource);

	//************************************
	// Method:    GetInteractionBehaviourID
	// FullName:  UNebulaInteractionBehaviour::GetInteractionBehaviourID
	// Access:    public 
	// Returns:   FGameplayTag
	// Qualifier: const
	// Description: Returns the ID of the behaviour. This is used to identify the behaviour in the system.
	//************************************
	UFUNCTION(BlueprintPure)
	FGameplayTag GetInteractionBehaviourID() const;

	//************************************
	// Method:    GetOwner
	// FullName:  UNebulaInteractionBehaviour::GetOwner
	// Access:    public 
	// Returns:   AActor*
	// Qualifier:
	// Description: Returns the owner  of the behaviour.
	//************************************
	UFUNCTION(BlueprintPure)
	AActor* GetOwner() const;

	//************************************
	// Method:    GetInteractor
	// FullName:  UNebulaInteractionBehaviour::GetInteractor
	// Access:    public 
	// Returns:   AActor*
	// Qualifier: const
	// Description: Returns the actor that triggered the interaction behaviour.
	//************************************
	UFUNCTION(BlueprintPure)
	AActor* GetInteractor() const;


	void Tick(float DeltaTime) override;


	TStatId GetStatId() const override;


	bool IsTickable() const override;

protected:

	//************************************
	//IMPLEMENT YOUR CODE LOGIC HERE
	//************************************
	virtual void InitBehaviour_Implementation(const FBehaviourInfo& inBehaviourInfo) {}
	virtual void ExecuteBehaviour_Implementation() {}
	virtual void TickBehaviour_Implementation(const float DeltaTime) {}
	virtual void StopBehaviour_Implementation(bool bInterrupted ) {}
	
	//************************************
	// Method:    StopBehaviour
	// FullName:  UNebulaInteractionBehaviour::StopBehaviour
	// Access:    public 
	// Returns:   void
	// Parameter: bool bInterrupted - whether the behaviour was interrupted or not. If true, the behaviour will be stopped and the interacter states will be updated accordingly.
	// Description: Called on behaviour end - SHOULDN'T BE CALLED TO INTERRUPT.
	//************************************
	UFUNCTION(BlueprintCallable)
	void StopBehaviour(bool bInterrupted = false);

	//************************************
	// Method:    UpdateInteractableState
	// FullName:  UNebulaInteractionBehaviour::UpdateInteractableState
	// Access:    virtual protected 
	// Returns:   void
	// Qualifier: const
	// Description: Call it to update interactable state.
	//************************************
	UFUNCTION(BlueprintCallable)
	virtual void UpdateInteractableState() const;

	//************************************
	// Method:    UpdateInteractorStatus
	// FullName:  UNebulaInteractionBehaviour::UpdateInteractorStatus
	// Access:    virtual protected 
	// Returns:   void
	// Qualifier: const
	// Description: Call it to update interactor status with the tags given.
	//************************************
	UFUNCTION(BlueprintCallable)
	virtual void UpdateInteractorStatus() const;

	//************************************
	// Method:    NotifySuccess
	// FullName:  UNebulaInteractionBehaviour::NotifySuccess
	// Access:    virtual protected 
	// Returns:   void
	// Qualifier:
	// Description: Notifies the system that the behaviour was successful. This is called when the behaviour is finished successfully.
	//************************************
	virtual void NotifySuccess();

	//************************************
	//IMPLEMENT YOUR BLUEPRINT LOGIC HERE
	//************************************
	UFUNCTION(BlueprintImplementableEvent)
	void BP_InitBehaviour_Implementation(const FBehaviourInfo& inBehaviourInfo);
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ExecuteBehaviour_Implementation();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_TickBehaviour_Implementation(float DeltaTime);
	UFUNCTION(BlueprintImplementableEvent)
	void BP_StopBehaviour_Implementation(bool bInterrupted);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Configuration")
	FBehaviourConfigurationStruct BehaviourConfiguration;

private:

	void BroadcastEvent(const EBroadcastType& inType, AActor* InterruptSource = nullptr) const;

	bool bTickEnabled = false;
	TWeakObjectPtr<AActor> Interactor = nullptr;
	TWeakObjectPtr<AActor> Owner = nullptr;
};