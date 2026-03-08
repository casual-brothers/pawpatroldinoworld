// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "PDWInteractionComponentWidget.generated.h"

class UPDWInteractionReceiverComponent;
class UPDWInteractionComponent;

/**
 * 
 */
UCLASS()
class PDW_API UPDWInteractionComponentWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetReceiverComponent(UPDWInteractionReceiverComponent* InOwner);
	void SetInteracterComponent(UPDWInteractionComponent* InInteracter);
	void UpdateSupportedInteraction(FGameplayTag SupportedInteractions);

	UFUNCTION(BlueprintPure)
	UPDWInteractionReceiverComponent* GetReceiverComp() const {return Owner;}

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateSupportedInteraction(FGameplayTag SupportedInteractions);

	
	TObjectPtr<UPDWInteractionReceiverComponent> Owner;
	TObjectPtr<UPDWInteractionComponent> Interacter;
};
