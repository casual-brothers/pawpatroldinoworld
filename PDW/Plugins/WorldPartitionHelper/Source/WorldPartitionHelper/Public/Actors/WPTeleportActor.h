#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WPBaseStreamingSourceActor.h"
#include "WPTeleportActor.generated.h"



UCLASS()
class WORLDPARTITIONHELPER_API AWPTeleportActor : public AWPBaseStreamingSourceActor
{
	GENERATED_BODY()

public:

	AWPTeleportActor();

	virtual void StartTeleport(TArray<AActor*>& inTeleportingActors,const TArray<FTransform>& inPositions);

protected:
	
	virtual void Teleport();

	UPROPERTY()
	TArray<AActor*> TeleportingActors = {};

	TArray<FTransform> TargetLocations = {};

	void OnStreamingCompleted() override;

};