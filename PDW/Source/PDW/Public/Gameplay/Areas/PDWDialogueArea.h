// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Managers/PDWDialogueSubSystem.h"
#include "PDWDialogueArea.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API APDWDialogueArea : public ATriggerBox
{
	GENERATED_BODY()
	
public:
	
	APDWDialogueArea();

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:

	UPROPERTY(EditAnywhere,Category = "Config")
	FConversation AreaConversation;

private:

	bool bTriggered = false;
};
