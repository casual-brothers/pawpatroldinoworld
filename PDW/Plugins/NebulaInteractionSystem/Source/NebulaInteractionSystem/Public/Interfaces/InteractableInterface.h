#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "InteractableInterface.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};
/*
THIS INTERFACE SHOULD BE GIVEN TO THE ACTOR THAT WILL HAVE INTERACTION FLOW 
*/
class NEBULAINTERACTIONSYSTEM_API IInteractableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	const FGameplayTag GetInteractionState() const;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void SetInteractionState(const FGameplayTag & inNewState);
	
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	const FGameplayTagContainer GetInteractionStatus() const;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void AddInteractionStatus(const FGameplayTagContainer& inStatusToAdd);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void RemoveInteractionStatus(const FGameplayTagContainer& inStatusToRemove);
};