// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Components/ActorComponent.h"
#include "Data/PDWGameplayStructures.h"
#include "Components/SplineComponent.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Containers/EnumAsByte.h"
#include "PDWBreadCrumbsComponent.generated.h"

class UPCGGraphInterface;
class UInstancedStaticMeshComponent;
class UNavigationSystemV1;
class APDWPlayerController;
class APDWEnvironmentAudioArea;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PDW_API UPDWBreadCrumbsComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	int32 PathRecalculationFrameInterval = 7;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	TSubclassOf<AActor> BreadcrumbClassP1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	TSubclassOf<AActor> BreadcrumbClassP2 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	int32 MaxBreadcrumbs = 15;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	int32 BreadcrumbsDistance = 150;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	int32 BreadcrumbsMinDistanceFromPlayer = 300;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	int32 BreadcrumbsMinDistanceFromTarget = 200;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	float MinPathPointDistanceFromPlayer = 200;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	TObjectPtr<UCurveFloat> CurveAnimation;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	bool bFromPlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	float BreadcrumbAnimationDuration = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	float BreadcrumbsAnimationInterval = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FVector BreadcrumbLocalOffset = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	bool UseHierarchicalPathFinding = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	TEnumAsByte<ESplinePointType::Type> SplinePointType = ESplinePointType::Linear;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FVector PlayerProjectPointExtent = {50.f, 50.f, 250.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	FVector BreadcrumbProjectPointExtent = {50.f, 50.f, 250.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	bool ShowDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DESIGN")
	bool ShowWaypointDebug = false;

	UFUNCTION()
	void InitComponent(APDWPlayerController* _OwnerController);

	UFUNCTION(BlueprintCallable)
	void SetBreadCrumbsVisible(bool bNewVisibility);

	UFUNCTION(BlueprintCallable)
	bool GetBreadCrumbsVisible() { return bBreadCrumbsActive; };

	UFUNCTION(BlueprintCallable)
	void AddBreadCrumbTarget(const FPDWQuestTargetActor& NewTarget);

	UFUNCTION(BlueprintCallable)
	void RemoveBreadCrumbTarget(AActor* ToRemoveTarget);

	UFUNCTION(BlueprintCallable)
	TArray<FPDWQuestTargetActor> GetBreadCrumbTargets() { return CurrentTargets; };

	UFUNCTION(BlueprintCallable)
	void AddFakeTargets(const  TMap<int32, FVector> FakeTargets);

	UFUNCTION(BlueprintCallable)
	void RemoveFakeTargets(const int32 FakeTarget);

	UFUNCTION(BlueprintCallable)
	void ClearFakeTargets();

	UFUNCTION(BlueprintCallable)
	TMap<int32, FVector> GetFakeTargets() { return FakeTargetsLocation; };


	UFUNCTION(BlueprintCallable)
	void ClearBreadCrumbTargets();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateBreadcrumbsAnimationAndVisibility(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void SetOwnerPawn(APawn* NewOwner);

	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void CheckWaypoints();

protected:

	UPROPERTY()
	UNavigationSystemV1* navSystem;

	UPROPERTY()
	FNavAgentProperties NavProp;

	UPROPERTY()
	USplineComponent* SplineComp;



	UPROPERTY()
	int32 PathRecalculationCurrentFrame = 0;
	
	int32 TotalBreadcrumbs = 0;

	int32 BreadcrumbsToUse = 0;

	float CurrentTickAnimationTime = 0.f;


	UPROPERTY()
	TArray<AActor*> Breadcrumbs = TArray<AActor*>();

	UFUNCTION(BlueprintCallable)
	FVector GetCurrentTarget();

	bool bBreadCrumbsActive = false;

	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY()
	TMap<int32, FVector> FakeTargetsLocation = {};

	UPROPERTY()
	TArray<FPDWQuestTargetActor> CurrentTargets;

	UPROPERTY()
	TObjectPtr<APawn> OwnerPawn;

	UPROPERTY()
	APDWPlayerController* OwnerController;

	UPROPERTY()
	bool InGameplayState = false;

	UPROPERTY()
	bool SearchingPath = false;

	UPROPERTY()
	FVector WaypointTarget = FVector::ZeroVector;

	UPROPERTY()
	FGameplayTag WaypointLocationTag = FGameplayTag();

	UPROPERTY()
	bool AcceptPartial = false;

	UPROPERTY()
	FVector CurrentTargetLocation = FVector::ZeroVector;

	UPROPERTY()
	FGameplayTag PlayerWaypointArea = FGameplayTag();

	UPROPERTY()
	FString WaypointName = FString();

	FVector LastOwnerLocation;
	FVector LastTargetLocation;

	UFUNCTION()
	void OnGameplayStateEnter();

	UFUNCTION()
	void OnGameplayStateExit();

	UFUNCTION()
	void BuildNavigationPath();

	void PathToObjectiveFound(uint32 aPathId, ENavigationQueryResult::Type aResultType, FNavPathSharedPtr aNavPointer);

	void MoveBreadcrumbs();

	UFUNCTION()
	void OnAreaChange(const FGameplayTag& CurrentAreaId);

	UFUNCTION()
	void ResetWaypointVariables();
};
