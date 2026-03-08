// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "Engine/TriggerVolume.h"
#include "GameplayTagContainer.h"
#include "PDWEnvironmentSkyArea.generated.h"

class APDWPlayerController;
class ANebulaGraphicsSkyActor;

UCLASS()
class PDW_API APDWEnvironmentSkyArea : public ATriggerVolume
{
	GENERATED_BODY()
	
public:
	
	APDWEnvironmentSkyArea();
		

public:

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION()
	FGameplayTag GetEnvironmentType() {return EnvironmentType; };

	UFUNCTION()
	FGameplayTag GetWaypointType() {return WaypointType; };

protected:

	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FGameplayTag EnvironmentType = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FGameplayTag WaypointType = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", meta = (Tooltip = "Cave should be ignored in multiplayer"))
	bool bIgnoreInMultiplayer = false;
	
	UPROPERTY()
	ANebulaGraphicsSkyActor* SkyActor = nullptr;

};
