#pragma once

#include "Components/ActorComponent.h"

#include "NebulaGraphicsBillboardComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class NEBULAGRAPHICS_API UNebulaGraphicsBillboardComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:

	UNebulaGraphicsBillboardComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction);

	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsBillboardComponent")
	void UseCustomTarget(const bool bInUseCustomTarget);
	
	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsBillboardComponent")
	void SetCustomTarget(const FVector& InTarget);

	UFUNCTION(BlueprintCallable, Category = "NebulaGraphicsBillboardComponent")
	void GetCustomTarget(FVector& OutTarget) const;

private:

	bool bUseCustomTarget = false;

	UPROPERTY()
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY()
	FRotator TargetRotator = FRotator::ZeroRotator;

	UPROPERTY()
	APlayerController* PlayerController = nullptr;
};