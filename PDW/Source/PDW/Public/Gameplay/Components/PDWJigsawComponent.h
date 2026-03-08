// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameplayTagContainer.h"
#include "PDWJigsawComponent.generated.h"

class UPDWPuzzlePieceComponent;
class APDWPlayerController;
class UFMODEvent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWJigsawComponent : public UBoxComponent
{
	GENERATED_BODY()

	// Sets default values for this component's properties
public:	
	UPDWJigsawComponent();

	void Reset();

	void EndPlay(EEndPlayReason::Type Reason) override;
private:

	UPROPERTY()
	TArray<UPDWPuzzlePieceComponent*> OverlappingPieces {};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Config")
	FName PuzzleID = NAME_None;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Config")
	UFMODEvent* AudioEvent = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bNotifySuccess = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "bNotifySuccess",EditConditionHides))
	FGameplayTag TargetIDToNotify = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "bNotifySuccess",EditConditionHides))
	FGameplayTag EventID = FGameplayTag::EmptyTag;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void CheckOrientation(UPDWPuzzlePieceComponent* inComp);

	UFUNCTION()
	void ShowHint(const FGameplayTag& inMinigameID);
};
