// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameplayTagContainer.h"
#include "Gameplay/Interfaces/PDWPlayerInterface.h"
#include "PDWInteractionOutlineComponent.generated.h"

class UPDWInteractionReceiverComponent;
class APDWPlayerController;

USTRUCT(BlueprintType)
struct PDW_API FMaterialStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MaterialSlotName = NAME_None;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FMaterialParameterInfo MaterialInfo;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float ScanDisabledMaterialValue = 0.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float ScanEnabledMaterialValue = 1.0f;

};

USTRUCT(BlueprintType)
struct PDW_API FInteractionOutlineConfigurationStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (ForceInlineRow))
	TMap<FGameplayTag,FMaterialStruct> OutlineMaterialConfig;
};

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PDW_API UPDWInteractionOutlineComponent : public USphereComponent
{
	GENERATED_BODY()

	UPDWInteractionOutlineComponent();
protected:

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Config")
	FInteractionOutlineConfigurationStruct OutlineConfiguration;

	virtual void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type Reason) override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex);
	UFUNCTION()
	void OnPupSwap(const FGameplayTag& inNewPup,APDWPlayerController* inController);
	UFUNCTION()
	void OnInteractionReceiverChangeState(UPDWInteractionReceiverComponent* inComponent,const FGameplayTag& ChangedTag,const FGameplayTag& inNewTag);
	void EvaluateCurrentPupSkills(IPDWPlayerInterface* inPlayer,FGameplayTag inTag = FGameplayTag::EmptyTag);
private:

	TWeakObjectPtr<UPDWInteractionReceiverComponent> InteractionReceiver = nullptr;

	UPROPERTY()
	TMap<UMeshComponent*,UMaterialInterface*> OverlayMaterialsMap;

	UPROPERTY()
	TMap<UMeshComponent*,UMaterialInstanceDynamic*> OverlayMIDMap;

	void ForceEvaluation(FGameplayTag inTag = FGameplayTag::EmptyTag);
};
