// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Interaction/PDWInteractionBehaviour.h"
#include "PDWRayTraceBehaviour.h"
#include "PDWRenderTargetBehaviour.generated.h"

class USceneCaptureComponent2D;
class UPDWSightWidget;

/**
 * 
 */
UCLASS()
class PDW_API UPDWRenderTargetBehaviour : public UPDWInteractionBehaviour
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadWrite)
	UMaterialInstanceDynamic* CachedMaterialInstanceDynamic = nullptr;

	UPROPERTY(EditAnywhere, Category = "TRACE")
	FTraceConfig TraceConfiguration;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float BrushRadius = 300.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName TextureParameterName = NAME_None;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName SlotName = NAME_None;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 RenderTargetWidth = 1024;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 RenderTargetHeigth = 1024;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UMaterialInterface* UnwrapMaterial = nullptr;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPDWSightWidget> SightWidgetClass;

	UPROPERTY(EditAnywhere)
	float DistanceFromActor = 500.f;

	UPROPERTY()
	TObjectPtr<UPDWSightWidget> SightWidget = nullptr;

	void Paint(const FHitResult& inHit);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_Paint(const FHitResult& inHit);

	void TickBehaviour_Implementation(float DeltaTime) override;

	void InitBehaviour_Implementation(USceneComponent* NewOwnerComponent) override;

	UFUNCTION()
	virtual void MoveSight(const FHitResult& inHitResult, FVector3d WorldPos);

	UPROPERTY(BlueprintReadWrite)
	UMaterialInstanceDynamic* OldMaterialInstanceDynamic = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UMeshComponent* Comp = nullptr;

	void StopBehaviour_Implementation() override;

private:

	bool bCalculate = false;
};
