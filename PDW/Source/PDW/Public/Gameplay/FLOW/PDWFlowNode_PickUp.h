// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/FLOW/PDWFlowNode_OnNotifyFromActor.h"
#include "PDWFlowNode_PickUp.generated.h"

class UPDWPickUpComponent;
class APDWPlayerController;

UCLASS(NotBlueprintable, meta = (DisplayName = "Wait for pick up"))
class PDW_API UPDWFlowNode_PickUp : public UPDWFlowNode_OnNotifyFromActor
{
	GENERATED_BODY()
	
protected:

	void ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) override;


	void ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) override;

UFUNCTION()

	void OnPickUpEnd(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController);

};
