// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWBreadCrumbsComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Managers/QuestSubsystem.h"
#include "Gameplay/Actors/PDW_NavigationWaypoint.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/Areas/PDWEnvironmentSkyArea.h"

UPDWBreadCrumbsComponent::UPDWBreadCrumbsComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.0f;
}

void UPDWBreadCrumbsComponent::InitComponent(APDWPlayerController* _OwnerController)
{
	OwnerController = _OwnerController;
	OwnerPawn = OwnerController->GetPawn();
	if (!OwnerPawn) return;
	NavProp = OwnerPawn->GetNavAgentPropertiesRef();
	SplineComp = NewObject<USplineComponent>(this, USplineComponent::StaticClass(), TEXT("SplineComp"));
	SplineComp->SetupAttachment(OwnerController->GetRootComponent());
	SplineComp->RegisterComponent();
	TSubclassOf<AActor> CurrentBreadcrumbMesh = UPDWGameplayFunctionLibrary::IsSecondPlayer(OwnerController) ? BreadcrumbClassP2 : BreadcrumbClassP1;
	navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);

	if (OwnerController)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		for (int32 i = 0; i < MaxBreadcrumbs; i++)
		{
			AActor* Breadcrumb = GetWorld()->SpawnActor<AActor>(CurrentBreadcrumbMesh, FTransform(), SpawnInfo);
			Breadcrumb->SetOwner(OwnerPawn);
			Breadcrumbs.Add(Breadcrumb);
		}

		// Make P2 tick when P1 is not ticking
		int32 PlayerIndex = OwnerController->GetLocalPlayer()->GetControllerId();
		if (PlayerIndex != 0)
		{
			APDWPlayerController* P1 = Cast<APDWPlayerController>(UPDWGameplayFunctionLibrary::GetNebulaPlayerControllerOne(this));
			if (P1)
			{
				UPDWBreadCrumbsComponent* P1Breadcrumbs = P1->FindComponentByClass<UPDWBreadCrumbsComponent>();
				PathRecalculationCurrentFrame = P1Breadcrumbs->PathRecalculationCurrentFrame + PathRecalculationFrameInterval/2;
			}
		}

	}
}

void UPDWBreadCrumbsComponent::SetBreadCrumbsVisible(bool bNewVisibility)
{
	bBreadCrumbsActive = bNewVisibility;
}

void UPDWBreadCrumbsComponent::AddBreadCrumbTarget(const FPDWQuestTargetActor& NewTarget)
{
	CurrentTargets.Add(NewTarget);
}

void UPDWBreadCrumbsComponent::AddFakeTargets(const TMap<int32, FVector> FakeTargets)
{
	FakeTargetsLocation.Append(FakeTargets);
}

void UPDWBreadCrumbsComponent::RemoveFakeTargets(const int32 FakeTarget)
{
	FakeTargetsLocation.Remove(FakeTarget);

	if (CurrentTargets.Num() == 0 && FakeTargetsLocation.Num() == 0)
	{
		ClearBreadCrumbTargets();
	}
}

void UPDWBreadCrumbsComponent::ClearFakeTargets()
{
	FakeTargetsLocation.Empty();
}

void UPDWBreadCrumbsComponent::RemoveBreadCrumbTarget(AActor* ToRemoveTarget)
{
	int32 ToRemoveIndex = -1;

	for (int32 i = 0; i < CurrentTargets.Num(); i++)
	{
		if (CurrentTargets[i].TargetActor == ToRemoveTarget)
		{
			ToRemoveIndex = i;
		}
	}

	if (ToRemoveIndex != -1)
	{
		CurrentTargets.RemoveAt(ToRemoveIndex);
	}

	if (CurrentTargets.Num() == 0 && FakeTargetsLocation.Num() == 0)
	{
		ClearBreadCrumbTargets();
	}
}

FVector UPDWBreadCrumbsComponent::GetCurrentTarget()
{
	if(WaypointTarget != FVector::ZeroVector)
		return WaypointTarget;

	FVector OwnerLocation = OwnerController->GetPawn()->GetActorLocation();
	float ClosestDistance = -1.f;
	FVector ClosestTarget{};

	if (CurrentTargets.Num() == 0)
	{
		for (TPair<int32, FVector>& Target : FakeTargetsLocation)
		{
			float Distance = FVector::Dist(OwnerLocation, Target.Value);
			if (ClosestDistance == -1.f || ClosestDistance > Distance)
			{
				ClosestDistance = Distance;
				ClosestTarget = Target.Value;
			}
		}
	}
	else
	{
		for (FPDWQuestTargetActor& Target : CurrentTargets)
		{
			FVector TargetLocation = Target.TargetActor ?
				Target.TargetActor->GetActorLocation() + Target.TargetActor->GetActorRotation().RotateVector(Target.BreadcrumbTargetOffset) :
				Target.TargetLocation;

			float Distance = FVector::Dist(OwnerLocation, TargetLocation);
			if (ClosestDistance == -1.f || ClosestDistance > Distance)
			{
				ClosestDistance = Distance;
				ClosestTarget = TargetLocation;
			}
		}
	}

	if (ShowDebug)
	{
		bool IsFirstPlayer = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld()) == OwnerController;
		int32 Index = IsFirstPlayer ? 0 : 1000;
		FString PlayerString = IsFirstPlayer ? "P1" : "P2";
		FColor StringColor = IsFirstPlayer ? FColor::Orange : FColor::Yellow;

		GEngine->AddOnScreenDebugMessage(Index++, 0.5f, StringColor, FString::Printf(TEXT("[%s] ClosestTarget: %s"), *PlayerString, *ClosestTarget.ToString()));
		for (TPair<int32, FVector>& Target : FakeTargetsLocation)
		{
			GEngine->AddOnScreenDebugMessage(Index++, 0.5f, StringColor, FString::Printf(TEXT("[%s] FakeTarget: %s"), *PlayerString, *Target.Value.ToString()));
		}
		for (FPDWQuestTargetActor& Target : CurrentTargets)
		{
			if (Target.TargetActor)
			{
				FVector DebugTargetLocation = Target.TargetActor->GetActorLocation() + Target.TargetActor->GetActorRotation().RotateVector(Target.BreadcrumbTargetOffset);
				GEngine->AddOnScreenDebugMessage(Index++, 0.5f, StringColor, FString::Printf(TEXT("[%s] Target: %s"), *PlayerString, *DebugTargetLocation.ToString()));
			}
		}
	}

	return ClosestTarget;
}

void UPDWBreadCrumbsComponent::OnGameplayStateEnter()
{
	InGameplayState = true;
}

void UPDWBreadCrumbsComponent::OnGameplayStateExit()
{
	InGameplayState = false;
}

void UPDWBreadCrumbsComponent::BuildNavigationPath()
{
	CurrentTargetLocation = GetCurrentTarget();
	if (CurrentTargetLocation == FVector::ZeroVector || SearchingPath) return;

	UNavigationPath* NavPath = nullptr;
	FPathFindingQuery navParams;
	navParams.EndLocation = CurrentTargetLocation;
	FNavLocation PosOnNavmesh = FNavLocation();
	navSystem->ProjectPointToNavigation(OwnerController->GetPawn()->GetActorLocation(), PosOnNavmesh, PlayerProjectPointExtent);
	navParams.StartLocation = PosOnNavmesh.Location;
	ANavigationData* navData = navSystem->MainNavData;
	navParams.QueryFilter = UNavigationQueryFilter::GetQueryFilter<UNavigationQueryFilter>(*navData);
	navParams.SetAllowPartialPaths(true);
	navParams.NavData = navData;
	navParams.SetRequireNavigableEndLocation(false);

	FNavPathQueryDelegate del;
	del.BindUObject(this, &UPDWBreadCrumbsComponent::PathToObjectiveFound);
	SearchingPath = true;
	navSystem->FindPathAsync(NavProp, navParams, del, UseHierarchicalPathFinding ? EPathFindingMode::Hierarchical : EPathFindingMode::Regular);
}

void UPDWBreadCrumbsComponent::PathToObjectiveFound(uint32 aPathId, ENavigationQueryResult::Type aResultType, FNavPathSharedPtr aNavPointer)
{
	SearchingPath = false;
	if (!bBreadCrumbsActive)
	{
		return;
	}

	SplineComp->ClearSplinePoints(true);
	FVector OwnerLocation = OwnerController->GetPawn()->GetActorLocation();
	bool OwnerReached = false;
	int32 PathPointsCount = aNavPointer ? aNavPointer->GetPathPoints().Num() : 0;
	TArray<FNavPathPoint> NavPoints = aNavPointer->GetPathPoints();

	if (!(aNavPointer && PathPointsCount > 0))
	{
		//no path found
		for (int32 i = 0; i < Breadcrumbs.Num(); i++)
		{
			Breadcrumbs[i]->SetActorHiddenInGame(true);
		}
		return;
	}

	//Add points to spline
	for (int32 i = 0; i < PathPointsCount; i++)
	{
		SplineComp->AddSplinePoint(NavPoints[i], ESplineCoordinateSpace::World, false);
		SplineComp->SetSplinePointType(i, SplinePointType);
		//DrawDebugSphere(GetWorld(), NavPoints[i], 70.0f, 8, FColor::Red, false, PathRecalculationInterval);		
	}	

		if (ShowWaypointDebug)
	{
		TArray<APDW_NavigationWaypoint*> Waypoints = UQuestSubsystem::Get(this)->GetWaypointsByArea(PlayerWaypointArea).Waypoints;
		for(int32 i = 0; i < Waypoints.Num(); i++)
		{
			if (Waypoints[i] && Waypoints[i]->WaypointLink)
			{
				GEngine->AddOnScreenDebugMessage(i, 2.f, FColor::Purple, FString::Printf(TEXT("%s To %s available here"), *Waypoints[i]->AreaTag.GetTagName().ToString(), *Waypoints[i]->WaypointLink->AreaTag.GetTagName().ToString()));
			}
		}

		APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
		if (!GM) return;
		for(APDWEnvironmentSkyArea* SkyArea : GM->SkyAreas)
		{
			if (SkyArea && SkyArea->EncompassesPoint(CurrentTargetLocation))
			{
				GEngine->AddOnScreenDebugMessage(123121, 2.f, FColor::Magenta, FString::Printf(TEXT("TargetArea: %s"), *SkyArea->GetWaypointType().GetTagName().ToString()));
				break;
			}
		}

		GEngine->AddOnScreenDebugMessage(12312136, 2.f, FColor::Red, FString::Printf(TEXT("IsPartial: %d"), aNavPointer->IsPartial()));
/*		GEngine->AddOnScreenDebugMessage(12312137, 2.f, FColor::Emerald, FString::Printf(TEXT("WaypointName: %s"), *WaypointName));	*/	
		GEngine->AddOnScreenDebugMessage(12312138, 2.f, FColor::Emerald, FString::Printf(TEXT("WaypointLocation: %s"), *WaypointTarget.ToString()));
		GEngine->AddOnScreenDebugMessage(12312139, 2.f, FColor::Emerald, FString::Printf(TEXT("WaypointLocationTag: %s"), *WaypointLocationTag.GetTagName().ToString()));
		GEngine->AddOnScreenDebugMessage(12312130, 2.f, FColor::Magenta, FString::Printf(TEXT("PlayerCurrentLocation: %s"), *PlayerWaypointArea.GetTagName().ToString()));
	}
}

void UPDWBreadCrumbsComponent::MoveBreadcrumbs()
{
	SplineComp->RemoveSplinePoint(0);
	while (SplineComp->GetNumberOfSplinePoints() > 1 && FVector::DistSquared(SplineComp->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World), OwnerController->GetPawn()->GetActorLocation()) < MinPathPointDistanceFromPlayer * MinPathPointDistanceFromPlayer)
	{
		SplineComp->RemoveSplinePoint(0);
	}
	SplineComp->AddSplinePointAtIndex(OwnerController->GetPawn()->GetActorLocation(), 0, ESplineCoordinateSpace::World, true);
	SplineComp->SetSplinePointType(0, SplinePointType);

	//for (int32 i = 0; i < SplineComp->GetNumberOfSplinePoints(); i++)
	//{
	//	DrawDebugSphere(GetWorld(), SplineComp->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World), 70.0f, 8, FColor::Red, false, PathRecalculationInterval);
	//}
	//for (int32 i=1; i<=200; ++i)
	//{
	//	DrawDebugLine(GetWorld(), SplineComp->GetLocationAtDistanceAlongSpline(SplineComp->GetSplineLength()* i/200.0f, ESplineCoordinateSpace::World), SplineComp->GetLocationAtDistanceAlongSpline(SplineComp->GetSplineLength()*(i-1)/200.0f, ESplineCoordinateSpace::World), FColor::Yellow, false, -1.f, (uint8)0U, 5.f);
	//}

	TotalBreadcrumbs = FMath::Max(0, (SplineComp->GetSplineLength() - BreadcrumbsMinDistanceFromPlayer) / (BreadcrumbsDistance));
	BreadcrumbsToUse = FMath::Min(TotalBreadcrumbs, MaxBreadcrumbs);
	float CurrentBreadcrumbDistance = BreadcrumbsMinDistanceFromPlayer;
	if (!bFromPlayer)
	{
		CurrentBreadcrumbDistance += FMath::Fmod(SplineComp->GetSplineLength(), BreadcrumbsDistance);
	}
	TArray<FVector> Locations = TArray<FVector>();
	Locations.Reserve(BreadcrumbsToUse);
	// set location
	for (int32 j = 0; j < BreadcrumbsToUse; j++)
	{
		FVector PosOnSpline = SplineComp->GetLocationAtDistanceAlongSpline(CurrentBreadcrumbDistance, ESplineCoordinateSpace::World);
		FNavLocation PosOnNavmesh = FNavLocation();
		navSystem->ProjectPointToNavigation(PosOnSpline, PosOnNavmesh, BreadcrumbProjectPointExtent);
		Locations.Add(PosOnNavmesh.Location);
		CurrentBreadcrumbDistance += BreadcrumbsDistance;
	}
	for (int32 j = 0; j < BreadcrumbsToUse; j++)
	{
		// compute rotation
		FVector& Prev = Locations[FMath::Max(j - 1, 0)];
		FVector& Next = Locations[FMath::Min(j + 1, BreadcrumbsToUse - 1)];

		FVector ForwardDir = (Next - Prev).GetSafeNormal();

		auto RotationMatrix = FRotationMatrix::MakeFromX(ForwardDir);
		FRotator FinalRotation = RotationMatrix.Rotator();
		

		// add local location offset
		FVector WorldMove = FinalRotation.RotateVector(BreadcrumbLocalOffset);
		Breadcrumbs[j]->SetActorLocationAndRotation(Locations[j] + WorldMove, FinalRotation);
	}
	for (int32 j = BreadcrumbsToUse; j < MaxBreadcrumbs; j++)
	{
		Breadcrumbs[j]->SetActorHiddenInGame(true);
	}
}

void UPDWBreadCrumbsComponent::OnAreaChange(const FGameplayTag& CurrentAreaId)
{
	if(PlayerWaypointArea.MatchesTagExact(CurrentAreaId)) return;

	PlayerWaypointArea = CurrentAreaId;
	CheckWaypoints();

	if (ShowWaypointDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("OnAreaChange: %s"), *CurrentAreaId.ToString()));
	}
}

void UPDWBreadCrumbsComponent::CheckWaypoints()
{
	if(!OwnerController || !OwnerController->GetPawn()) return;

	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
	if (!GM) return;

	ResetWaypointVariables();
	CurrentTargetLocation = GetCurrentTarget();
	if(CurrentTargetLocation == FVector::ZeroVector) return;

	FGameplayTag TargetArea = {};

	for (APDWEnvironmentSkyArea* SkyArea : GM->SkyAreas)
	{
		if(!SkyArea) continue;

		if (TargetArea == FGameplayTag::EmptyTag && SkyArea->EncompassesPoint(CurrentTargetLocation))
		{
			TargetArea = SkyArea->GetWaypointType();
		}
		if (PlayerWaypointArea == FGameplayTag::EmptyTag && SkyArea->EncompassesPoint(OwnerController->GetPawn()->GetActorLocation()))
		{
			PlayerWaypointArea = SkyArea->GetWaypointType();
		}
	}

	if (TargetArea.IsValid() && !PlayerWaypointArea.MatchesTagExact(TargetArea))
	{
		UQuestSubsystem* QuestSystem = UQuestSubsystem::Get(this);
		if (QuestSystem)
		{
			TArray<APDW_NavigationWaypoint*> Waypoints = QuestSystem->GetWaypointsByArea(PlayerWaypointArea).Waypoints;

			float BestDistance = -1.f;
			APDW_NavigationWaypoint* BestTarget = nullptr;

			for (APDW_NavigationWaypoint* Waypoint : Waypoints)
			{
				if (!Waypoint || !Waypoint->WaypointLink)
					continue;

				if (Waypoint->WaypointLink->AreaTag == TargetArea)
				{
					BestTarget = Waypoint->WaypointLink;
					break;
				}

				float Distance = FVector::Dist(Waypoint->WaypointLink->GetActorLocation(), CurrentTargetLocation);

				if (BestDistance == -1.f || Distance < BestDistance)
				{
					BestTarget = Waypoint->WaypointLink;
					BestDistance = Distance;
				}
			}

			if (BestTarget)
			{
				WaypointTarget = BestTarget->GetActorLocation();
				//WaypointName = BestTarget->GetActorLabel();
				WaypointLocationTag = BestTarget->AreaTag;
			}
		}
	}	
}

void UPDWBreadCrumbsComponent::ResetWaypointVariables()
{
	WaypointTarget = FVector::ZeroVector;
	WaypointLocationTag = FGameplayTag();
	//WaypointName = FString();
}

void UPDWBreadCrumbsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!SplineComp) return;
	if (CurrentTargets.Num() == 0 && FakeTargetsLocation.Num() == 0) return;

	PathRecalculationCurrentFrame++;
	SplineComp->SetWorldRotation(FRotator::ZeroRotator);

	if (PathRecalculationCurrentFrame >= PathRecalculationFrameInterval)
	{
		PathRecalculationCurrentFrame = 0;
		BuildNavigationPath();
	}

	CurrentTickAnimationTime += DeltaTime;

	MoveBreadcrumbs();
	UpdateBreadcrumbsAnimationAndVisibility(DeltaTime);

}

void UPDWBreadCrumbsComponent::UpdateBreadcrumbsAnimationAndVisibility(float DeltaTime)
{
	FVector PlayerLocation = OwnerController->GetPawn()->GetActorLocation();
	for (int32 i = 0; i < Breadcrumbs.Num(); ++i)
	{
		AActor* Breadcrumb = Breadcrumbs[i];

		float CurrentBreadcrumbAnimationTime = FMath::Fmod(CurrentTickAnimationTime - i * BreadcrumbsAnimationInterval, BreadcrumbAnimationDuration);
		float OldBreadcrumbAnimationTime = FMath::Fmod(CurrentTickAnimationTime - DeltaTime - i * BreadcrumbsAnimationInterval, BreadcrumbAnimationDuration);

		if (CurveAnimation)
		{
			Breadcrumb->SetActorScale3D(CurveAnimation->GetFloatValue(CurrentBreadcrumbAnimationTime) * FVector::OneVector);
		}

		bool bTooNearToPlayer = FVector::DistSquared(Breadcrumb->GetActorLocation(), PlayerLocation) < BreadcrumbsMinDistanceFromPlayer * BreadcrumbsMinDistanceFromPlayer;
		bool bTooNearToTarget = FVector::DistSquared(Breadcrumb->GetActorLocation(), SplineComp->GetLocationAtSplinePoint(SplineComp->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World)) < BreadcrumbsMinDistanceFromTarget * BreadcrumbsMinDistanceFromTarget;
		Breadcrumb->SetActorHiddenInGame(((bTooNearToPlayer && !bFromPlayer) || bTooNearToTarget) || (i > TotalBreadcrumbs || !bBreadCrumbsActive));

	}
}

void UPDWBreadCrumbsComponent::SetOwnerPawn(APawn* NewOwner)
{
	for (int32 i = 0; i < Breadcrumbs.Num(); i++)
	{
		Breadcrumbs[i]->SetOwner(NewOwner);
	}
}

void UPDWBreadCrumbsComponent::BeginPlay()
{
	Super::BeginPlay();

	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
	if (EventSubsystem)
	{
		EventSubsystem->OnGameplayStateEnter.AddUniqueDynamic(this, &UPDWBreadCrumbsComponent::OnGameplayStateEnter);
		EventSubsystem->OnGameplayStateExit.AddUniqueDynamic(this, &UPDWBreadCrumbsComponent::OnGameplayStateExit);
		EventSubsystem->OnLocationEnter.AddUniqueDynamic(this, &UPDWBreadCrumbsComponent::OnAreaChange);
	}
}

void UPDWBreadCrumbsComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
	if (EventSubsystem)
	{
		EventSubsystem->OnGameplayStateEnter.RemoveDynamic(this, &UPDWBreadCrumbsComponent::OnGameplayStateEnter);
		EventSubsystem->OnGameplayStateExit.RemoveDynamic(this, &UPDWBreadCrumbsComponent::OnGameplayStateExit);
		EventSubsystem->OnLocationEnter.RemoveDynamic(this, &UPDWBreadCrumbsComponent::OnAreaChange);
	}

	Super::EndPlay(EndPlayReason);
}

void UPDWBreadCrumbsComponent::ClearBreadCrumbTargets()
{
	CurrentTarget = nullptr;
	FakeTargetsLocation.Empty();
	CurrentTargets.Empty();
	ResetWaypointVariables();

	if (SplineComp)
	{
		SplineComp->ClearSplinePoints(true);
	}
	for (int32 i = 0; i < Breadcrumbs.Num(); i++)
	{
		Breadcrumbs[i]->SetActorHiddenInGame(true);
	}
}
