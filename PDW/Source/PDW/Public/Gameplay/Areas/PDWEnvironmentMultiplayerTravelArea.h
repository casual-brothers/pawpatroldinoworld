// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "GameplayTagContainer.h"
#include "PDWEnvironmentMultiplayerTravelArea.generated.h"

class ATargetPoint;
class UNebulaFlowDialog;

UCLASS()
class PDW_API APDWEnvironmentMultiplayerTravelArea : public ATriggerVolume
{
	GENERATED_BODY()
	
public:
	
	APDWEnvironmentMultiplayerTravelArea();

	
public:

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FGameplayTag EnvironmentType = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	TMap<FGameplayTag, ATargetPoint*> InsideTeleportPoints;
	
	UPROPERTY(EditAnywhere, Category = "Configuration")
	TMap<FGameplayTag, ATargetPoint*> OutsideTeleportPoints;

private:
	
	UPROPERTY(EditAnywhere, Category = "Setup")
	FName ChangeAreaDialogID = FName("AreYouSureChangeAreaMultiplayer");
	
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UNebulaFlowDialog> ChangeAreaDialog{ nullptr };

};

