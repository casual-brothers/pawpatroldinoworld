#pragma once

#include "Engine/TriggerVolume.h"
#include "GameplayTagContainer.h"

#include "NebulaGraphicsSkyAreaTriggerVolume.generated.h"

class ANebulaGraphicsSkyActor;

UCLASS()
class NEBULAGRAPHICS_API ANebulaGraphicsSkyAreaTriggerVolume : public ATriggerVolume
{
	GENERATED_BODY()

	ANebulaGraphicsSkyAreaTriggerVolume();

public:

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FGameplayTag SkyArea = FGameplayTag::EmptyTag;

	virtual void BeginPlay() override;
	
private:

	UPROPERTY()
	ANebulaGraphicsSkyActor* NebulaGraphicsSkyActor = nullptr;
};