// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Interaction/PDWInteractionBehaviour.h"
#include "PDWBaseRayTraceBehaviour.generated.h"

 USTRUCT(BlueprintType)
struct PDW_API FBaseTraceConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float TraceLength = 300.0f;
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECollisionChannel::ECC_Camera;
	
	UPROPERTY(EditAnywhere)
	bool bSphereTrace = false;

	UPROPERTY(EditAnywhere,meta = (EditCondition ="bSphereTrace",EditConditionHides))
	float TraceRadius = 50.0f;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;

};

/**
 * 
 */
UCLASS()
class PDW_API UPDWBaseRayTraceBehaviour : public UPDWInteractionBehaviour
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "TRACE")
	FBaseTraceConfig TraceConfiguration;

	UPROPERTY(EditAnywhere, Category = "OPTIMIZATION")
	int32 FrameToSkip = 10;	

	UFUNCTION(BlueprintNativeEvent)
	void OnRayTraceHit(const FHitResult& inHitResult,float DeltaTime);

	UFUNCTION(BlueprintNativeEvent)
	void OnRayTraceHits(const TArray<FHitResult>& Hits,float DeltaTime);

private:

	bool bCalculate = false;
	int32 CachedFrameToSkipSettings = 0;
protected:
	void TickBehaviour_Implementation(float DeltaTime) override;


	void InitBehaviour_Implementation(USceneComponent* NewOwnerComponent) override;

};
