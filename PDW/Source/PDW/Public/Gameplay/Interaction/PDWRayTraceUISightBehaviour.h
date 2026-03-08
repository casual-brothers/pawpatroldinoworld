// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Gameplay/Interaction/PDWBaseRayTraceBehaviour.h"
#include "UI/Widgets/PDWSightWidget.h"
#include "PDWRayTraceUISightBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWRayTraceUISightBehaviour : public UPDWBaseRayTraceBehaviour
{
	GENERATED_BODY()
	
public:
	void OnRayTraceHit_Implementation(const FHitResult& inHitResult, float DeltaTime) override;

	void OnRayTraceHits_Implementation(const TArray<FHitResult>& Hits, float DeltaTime) override;

protected:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPDWSightWidget> SightWidgetClass;

	UPROPERTY(EditAnywhere)
	float DistanceFromActor = 500.f;

	UPROPERTY()
	TObjectPtr<UPDWSightWidget> SightWidget = nullptr;

	void InitBehaviour_Implementation(USceneComponent* NewOwnerComponent) override;

	void StopBehaviour_Implementation() override;

	UFUNCTION()
	virtual void MoveSight(const FHitResult& inHitResult, FVector3d WorldPos);
};
