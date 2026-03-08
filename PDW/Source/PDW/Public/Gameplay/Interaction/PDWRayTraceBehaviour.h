// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Interaction/PDWInteractionBehaviour.h"
#include "PDWRayTraceBehaviour.generated.h"

/**
 * 
 */

 USTRUCT(BlueprintType)
struct PDW_API FTraceConfig
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

 USTRUCT(BlueprintType)
struct PDW_API FMaterialConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FName MeshComponentTag = NAME_None;

	UPROPERTY(EditAnywhere)
	FName MaterialSlotNameToTrace = NAME_None;
	
	UPROPERTY(EditAnywhere)
	FMaterialParameterInfo MaterialInfo;
	
	UPROPERTY(EditAnywhere)
	float TargetMaterialValue = 1.0f;
};

UCLASS()
class PDW_API UPDWRayTraceBehaviour : public UPDWInteractionBehaviour
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, Category = "TRACE")
	FTraceConfig TraceConfiguration;

	UPROPERTY(EditAnywhere, Category = "MATERIAL")
	FMaterialConfig MaterialConfiguration;

	UPROPERTY(EditAnywhere, Category = "OPTIMIZATION")
	int32 FrameToSkip = 10;

	TWeakObjectPtr<UMaterialInstanceDynamic> CachedMaterialInstanceDynamic = nullptr;

	TWeakObjectPtr<UMeshComponent> CachedMeshComp = nullptr;

	void TickBehaviour_Implementation(float DeltaTime) override;

	void InitBehaviour_Implementation(USceneComponent* NewOwnerComponent) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnRayTraceHit(const FHitResult& inHitResult,float DeltaTime);

	UFUNCTION(BlueprintNativeEvent)
	void OnRayTraceHits(const TArray<FHitResult>& Hits,float DeltaTime);

	UFUNCTION(BlueprintCallable)
	UMaterialInstanceDynamic* GetMaterialInstanceDynamic();

	UFUNCTION(BlueprintCallable)
	void SetMaterialInstanceDynamic(UMaterialInstanceDynamic* inInstance);

	UFUNCTION(BlueprintCallable)
	UMeshComponent* GetCachedMeshComponent();
private:

	void PrepareMaterialInstanceDynamic(AActor* inMinigameActor);

	bool bCalculate = false;
	float MaterialValue = 0.0f;
	int32 CachedFrameToSkipSettings = 0;

};
